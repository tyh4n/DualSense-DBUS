/*
BMC_SBUS version 0.1.0 May 14th 2016 Stu Aitken
A simplified library for writing out serial servo data in Futaba SBUS format.

Explictly written to control Blackmagic Design micro cameras, but should work generally.

Note: For RC use, various important flags such as failsafe have been omitted here.
      Please refer to the original source library in this case.

Based on FUTABA_SBUS library written by Michael Baker.
See: https://github.com/mikeshub/FUTABA_SBUS
Also see: http://forum.arduino.cc/index.php/topic,99708.0.html

Initial reverse engineering of SBUS protocol by Uwe Gartmann.
See: https://developer.mbed.org/users/Digixx/notebook/futaba-s-bus-controlled-by-mbed/

SBUS ©FUTABA CORPORATION 2016.
*/

#include "BMC_SBUS.h"

void BMC_SBUS::begin() {
  // Initialize private data arrays.
  // sbus_data is formatted for correct serial output.
  // Note that the actual 11-bit SBUS data for each channel is embedded across multiple data bytes in a very strange order.
  // Byte 1 and bytes 24 and 25 should be left as is.
  // The first is a start byte, the last is a stop byte, and the second last holds various flags.
  // servos is the internal per-channel position and is more straightforward - one int16_t per channel.

  uint8_t loc_sbusData[25] = {0x0f, 0x01, 0x04, 0x20, 0x00, 0xff, 0x07, 0x40, 0x00, 0x02, 0x10, 0x80, 0x2c, 0x64, 0x21, 0x0b, 0x59, 0x08, 0x40, 0x00, 0x02, 0x10, 0x80, 0x00, 0x00};
  int16_t loc_servos[18] = {1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 0, 0};

  // Setup serial port to transmit at 100k baud and use 1 parity and 2 stop bits.
  SERIALPORT.begin(BAUDRATE, SERIAL_8E2, 16, 17);  // RX on GPIO 16, TX on GPIO 17

  // Setup public data arrays.
  memcpy(sbusData, loc_sbusData, 25);
  memcpy(servos, loc_servos, 18);
}

void BMC_SBUS::Servo(uint8_t ch, int16_t position) {
  // Set servo position on a single channel.

  if ((ch > 0) && (ch <= 16)) {
    constrain(position, 0, 2048);  // Keep within min/max values.
    servos[ch - 1] = position;    // Expects a non-zero starting index to the channel.
  }
}

void BMC_SBUS::Send(void) {
  // Send data over the serial port.
  SERIALPORT.write(sbusData, 25);  // According to docs for Serial, we can send the array as is without a loop.
}

void BMC_SBUS::Update(void) {
  // Update positions for all servo channels within the SBUS data frame.
  // Ignores digital servos and any failsafe mode stuff that was originally written.

  // Clear out existing SBUS data for all channel data bytes.
  // Ignores the first and last bytes in the array (start and stop bytes).
  // Mapping loop relies on initial 0 values - do not omit this step!

  // Data mapping
  //  [--sbusData[24]--][--sbusData[23]--]...[--sbusData[1]--][--sbusData[0]--]
  //                    [--servos[16]--].......[--servos[1]--]

  uint8_t i;
  for (i = 1; i < 24; i++) {
    sbusData[i] = 0;
  }

  // Reset counters.
  ch = 0;
  bit_in_servo = 0;
  byte_in_sbus = 1;
  bit_in_sbus = 0;

  // Format SBUS data - maps servo data array to SBUS data array 1 bit at a time.
  // Correctly deals with the little-endian byte order in the process.

  for (i = 0; i < 176; i++) {  // 16 channels * 11 bits = 176 bits.
    if (servos[ch] & (1 << bit_in_servo)) {  // Bitwise AND to check if the correct servo data bit is set to 1.
      sbusData[byte_in_sbus] |= (1 << bit_in_sbus);  // Bitwise OR sets the correct SBUS data bit if true.
    }

    // Increment bit counters.
    bit_in_sbus++;
    bit_in_servo++;

    // If end of SBUS byte, reset SBUS bit counter and increment SBUS byte counter.
    if (bit_in_sbus == 8) {
      bit_in_sbus = 0;
      byte_in_sbus++;
    }

    // If we have reached bit 11 in the servo data, increment channel index and reset servo bit counter.
    if (bit_in_servo == 11) {
      bit_in_servo = 0;
      ch++;
    }
  }
}