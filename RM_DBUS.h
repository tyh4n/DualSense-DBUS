#ifndef RM_DBUS_h
#define RM_DBUS_h

#include <Arduino.h>

#define BAUDRATE 100000
#define SERIALPORT Serial1  

/**
 * @name RM_DBUS
 * @brief Module to receive and interpret data from the DJI DBUS receiver.
 * @pre DBUS pins are configured properly in board.h.
 * @note See DJI DBUS documentation for details on the protocol and data format.
 * @usage 1. Invoke begin() to initialize the module.
 *        2. Use write_channel() to set channel values.
 *        3. Call update() to process and pack data into the DBUS format.
 *        4. Use send() to transmit the DBUS data over the serial port.
 */

class RM_DBUS {
public:
    uint8_t dbus_data[18];  // DBUS data frame (18 bytes)
    int16_t channel[13];   // Channel data (13 channels: 4 RC, 2 switches, 1 wheel, 3 mouse axes, 2 mouse buttons, 1 keyboard code)

    /**
     * @brief Initialize the DBUS module.
     * @details Sets up the serial port and initializes the DBUS data frame.
     */
    void begin(void);

    /**
     * @brief Set the value for a specific channel.
     * @param ch The channel index (1–12).
     * @param data The value to set for the channel.
     */
    void write_channel(uint8_t ch, int16_t data);

    /**
     * @brief Send the DBUS data frame over the serial port.
     */
    void send(void);

    /**
     * @brief Update the DBUS data frame with the current channel values.
     * @details Converts the channel data into the DBUS protocol format.
     */
    void update(void);
};

#endif