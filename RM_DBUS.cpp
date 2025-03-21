/*
Created by Tianyi Han
March 20, 2025

Based on:
BMS_SBUS by Stu Aitken https://github.com/boldstelvis/BMC_SBUS
FUTABA_SBUS by Michael Baker https://github.com/mikeshub/FUTABA_SBUS
Initial reverse engineering of SBUS by Uwe Gartmann https://developer.mbed.org/users/Digixx/notebook/futaba-s-bus-controlled-by-mbed/
*/

#include "RM_DBUS.h"
#include <cstdint>

void RM_DBUS::begin() {
  // Initialize private data arrays
  // dbus_data is formatted for correct serial output
  // channel is the internal per channel position and is more straightforward - one int16_t per channel

  uint8_t loc_dbus_data[18] = {0x0f, 0xff, 0x07, 0xff, 0x07, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  int16_t loc_channel[13] = {1023, 1023, 1023, 1023, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  // Setup serial port to transmit at 100k baud and use 1 parity and 2 stop bits
  SERIALPORT.begin(BAUDRATE, SERIAL_8E2, 16, 17);  // TX on GPIO 17

  // Setup public data arrays
  memcpy(dbus_data, loc_dbus_data, 18);
  memcpy(channel, loc_channel, 13 * sizeof(int16_t));
}

void RM_DBUS::write_channel(uint8_t ch, int16_t data) {
  if ((ch > 0) && (ch <= 12)) {
    data = constrain(data, 0, 2047); // Keep within min/max values
    channel[ch - 1] = data; // Expects a non-zero starting index to the channel
  }
}

void RM_DBUS::send() {
  // Send data over serial port
  SERIALPORT.write(dbus_data, 18); // According to docs for Serial, we can send the array as is without a loop
}

void RM_DBUS::update() {
  /*
   * Convert the channel array into the dbus_data array.
   * The mapping is as follows:
   *
   * | Channel Index | Description               | dbus_data Mapping                         |
   * |---------------|---------------------------|-------------------------------------------|
   * | channel[0]    | rc.ch0 (Throttle)         | dbus_data[0] = ch0[7:0]                   |
   * |               |                           | dbus_data[1] = ch0[10:8]                  |
   * |---------------|---------------------------|-------------------------------------------|
   * | channel[1]    | rc.ch1 (Yaw)              | dbus_data[1] |= ch1[4:0] << 3             |
   * |               |                           | dbus_data[2] = ch1[10:5]                  |
   * |---------------|---------------------------|-------------------------------------------|
   * | channel[2]    | rc.ch2 (Pitch)            | dbus_data[2] |= ch2[1:0] << 6             |
   * |               |                           | dbus_data[3] = ch2[9:2]                   |
   * |               |                           | dbus_data[4] = ch2[10]                    |
   * |---------------|---------------------------|-------------------------------------------|
   * | channel[3]    | rc.ch3 (Roll)             | dbus_data[4] |= ch3[6:0] << 1             |
   * |               |                           | dbus_data[5] = ch3[10:7]                  |
   * |---------------|---------------------------|-------------------------------------------|
   * | channel[4]    | rc.s1 (3 way switch 1     | dbus_data[5] |= s1 << 4                   |
   * |---------------|---------------------------|-------------------------------------------|
   * | channel[5]    | rc.s2 (3 way switch 2     | dbus_data[5] |= s2 << 6                   |
   * |---------------|---------------------------|-------------------------------------------|
   * | channel[6]    | rc.wheel (Wheel)          | dbus_data[16] = wheel[7:0]                |
   * |               |                           | dbus_data[17] = wheel[10:8]               |
   * |---------------|---------------------------|-------------------------------------------|
   * | channel[7]    | mouse.x (Mouse X-axis)    | dbus_data[6] = mouse_x[7:0]               |
   * |               |                           | dbus_data[7] = mouse_x[15:8]              |
   * |---------------|---------------------------|-------------------------------------------|
   * | channel[8]    | mouse.y (Mouse Y-axis)    | dbus_data[8] = mouse_y[7:0]               |
   * |               |                           | dbus_data[9] = mouse_y[15:8]              |
   * |---------------|---------------------------|-------------------------------------------|
   * | channel[9]    | mouse.z (Mouse Z-axis)    | dbus_data[10] = mouse_z[7:0]              |
   * |               |                           | dbus_data[11] = mouse_z[15:8]             |
   * |---------------|---------------------------|-------------------------------------------|
   * | channel[10]   | mouse_left                | dbus_data[12] = mouse_left                |
   * |---------------|---------------------------|-------------------------------------------|
   * | channel[11]   | mouse_right               | dbus_data[13] = mouse_right               |
   * |---------------|---------------------------|-------------------------------------------|
   * | channel[12]   | keyboard_code             | dbus_data[14] = keyboard_code[7:0]        |
   * |               |                           | dbus_data[15] = keyboard_code[15:8]       |
   */

  // Convert rc.ch0 to dbus_data[0] and dbus_data[1]
  int16_t ch0 = static_cast<int16_t>(channel[0]);
  dbus_data[0] = ch0 & 0xFF; 
  dbus_data[1] = (ch0 >> 8) & 0x07; 

  // Convert rc.ch1 to dbus_data[1] and dbus_data[2]
  int16_t ch1 = static_cast<int16_t>(channel[1]);
  dbus_data[1] |= (ch1 << 3) & 0xF8; 
  dbus_data[2] = (ch1 >> 5) & 0x1F; 

  // Convert rc.ch2 to dbus_data[2], dbus_data[3], and dbus_data[4]
  int16_t ch2 = static_cast<int16_t>(channel[2]);
  dbus_data[2] |= (ch2 << 6) & 0xC0; 
  dbus_data[3] = (ch2 >> 2) & 0xFF; 
  dbus_data[4] = (ch2 >> 10) & 0x01; 

  // Convert rc.ch3 to dbus_data[4] and dbus_data[5]
  int16_t ch3 = static_cast<int16_t>(channel[3]);
  dbus_data[4] |= (ch3 << 1) & 0xFE; 
  dbus_data[5] = (ch3 >> 7) & 0x0F; 

  // Convert rc.s1 and rc.s2 to dbus_data[5]
  dbus_data[5] |= (static_cast<uint8_t>(channel[4]) << 4) & 0x30; 
  dbus_data[5] |= (static_cast<uint8_t>(channel[5]) << 6) & 0xc0; 

  // Convert rc.wheel to dbus_data[16] and dbus_data[17]
  int16_t wheel = static_cast<int16_t>(channel[6]);
  dbus_data[16] = wheel & 0xFF; 
  dbus_data[17] = (wheel >> 8) & 0x07; 

  // Convert mouse.x to dbus_data[6] and dbus_data[7]
  int16_t mouse_x = static_cast<int16_t>(channel[7]);
  dbus_data[6] = mouse_x & 0xFF; 
  dbus_data[7] = (mouse_x >> 8) & 0xFF; 

  // Convert mouse.y to dbus_data[8] and dbus_data[9]
  int16_t mouse_y = static_cast<int16_t>(channel[8]);
  dbus_data[8] = mouse_y & 0xFF; 
  dbus_data[9] = (mouse_y >> 8) & 0xFF; 

  // Convert mouse.z to dbus_data[10] and dbus_data[11]
  int16_t mouse_z = static_cast<int16_t>(channel[9]);
  dbus_data[10] = mouse_z & 0xFF; 
  dbus_data[11] = (mouse_z >> 8) & 0xFF; 

  // Convert mouse_left and mouse_right to dbus_data[12] and dbus_data[13]
  dbus_data[12] = static_cast<uint8_t>(channel[10]); 
  dbus_data[13] = static_cast<uint8_t>(channel[11]); 

  // Convert keyboard_code to dbus_data[14] and dbus_data[15]
  uint16_t key_code = static_cast<uint16_t>(channel[12]);
  dbus_data[14] = key_code & 0xFF; 
  dbus_data[15] = (key_code >> 8) & 0xFF; 
}