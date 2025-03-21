#ifndef BMC_SBUS_h
#define BMC_SBUS_h

#include <Arduino.h>

#define BAUDRATE 100000  // SBUS baud rate (100kbps)
#define SERIALPORT Serial1  // Serial port used for SBUS communication

/**
 * @name BMC_SBUS
 * @brief Module to send servo data in Futaba SBUS format.
 * @pre SBUS pins are configured properly in board.h.
 * @note This library is simplified for controlling Blackmagic Design micro cameras.
 *       For RC use, additional flags such as failsafe are omitted.
 * @usage 1. Invoke begin() to initialize the SBUS module.
 *        2. Use Servo() to set servo positions.
 *        3. Call Update() to format the SBUS data frame.
 *        4. Use Send() to transmit the SBUS data over the serial port.
 */
class BMC_SBUS {
public:
    uint8_t sbusData[25];  // SBUS data frame (25 bytes)
    int16_t servos[18];    // Servo positions (16 channels + 2 digital channels)

    /**
     * @brief Initialize the SBUS module.
     * @details Sets up the serial port and initializes the SBUS data frame.
     */
    void begin(void);

    /**
     * @brief Set the position for a specific servo channel.
     * @param ch The channel index (1–16).
     * @param position The position value to set for the servo (0–2048).
     */
    void Servo(uint8_t ch, int16_t position);

    /**
     * @brief Send the SBUS data frame over the serial port.
     */
    void Send(void);

    /**
     * @brief Update the SBUS data frame with the current servo positions.
     * @details Converts the servo data into the SBUS protocol format.
     */
    void Update(void);

private:
    uint8_t byte_in_sbus;  // Current byte position in the SBUS data frame
    uint8_t bit_in_sbus;   // Current bit position in the SBUS data frame
    uint8_t ch;            // Current channel index
    uint8_t bit_in_servo;  // Current bit position in the servo data
};

#endif