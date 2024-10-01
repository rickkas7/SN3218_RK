#ifndef __SN3218_RK_H
#define __SN3218_RK_H

#include "Particle.h"

// Github: https://github.com/rickkas7/SN3218_RK
// License: MIT

/**
 * @brief Class for interfacing with a SN3218 LED PWM controller
 * 
 * Because the address of the SN3218 is not configurable you will typically only
 * have one instance of this class as a global variable. It's not explicitly a
 * singleton because you could have more than one instance if you have multiple
 * I2C busses or an I2C MUX.
 * 
 * Note that there should be an instance of this class for each physical chip.
 * Because the chip does not support reading registers, the current LED on
 * and off state is stored as a class variable so you can easily change just
 * a single LED.
 */
class SN3218_RK {
public:
    /**
     * @brief Constructor for an instance of the class
     * 
     * @param i2cAddress I2C address 
     * 
     */
    SN3218_RK(uint8_t i2cAddress = DEFAULT_I2C_ADDRESS);

    /**
     * @brief Set the PWM value for a given channel
     * 
     * @param channel 0 - 17 (note: this is zero-based not OUT1 - OUT18!)
     * @param value 0-255 (0 is off, 255 is 100% on)
     * @return true if the channel is valid and I2C operation succeeded
     * 
     * The power-on default is 0, so you pretty much need to set it to another value
     * to turn on an output pin.
     * 
     * There are on and off states separate from the PWM setting.
     * 
     * This does not immediately affect the outputs; you must call update() to do that after
     * setting the PWM values or on/off state.
     */
    bool setPWM(uint8_t channel, uint8_t value);

    /**
     * @brief Turns on a single LED 
     * 
     * @param channel 0 - 17 (note: this is zero-based not OUT1 - OUT18!)
     * @return true if the channel is valid and I2C operation succeeded
     * 
     * On and off are separate from the PWM setting. Turning the LED on uses the previously
     * set PWM value. Note that the default PWM is 0, which effectively will prevent the
     * output from turning on if not set.
     * 
     * This does not immediately affect the outputs; you must call update() to do that after
     * setting the PWM values or on/off state.
     */
    bool ledOn(uint8_t channel);

    /**
     * @brief Turns off single LED 
     * 
     * @param channel 0 - 17 (note: this is zero-based not OUT1 - OUT18!)
     * @return true if the channel is valid and I2C operation succeeded
     * 
     * This does not immediately affect the outputs; you must call update() to do that after
     * setting the PWM values or on/off state.
     */
    bool ledOff(uint8_t channel);

    /**
     * @brief Toggles the state of an LED on or off, opposite of what it previously was.
     * 
     * @param channel 0 - 17 (note: this is zero-based not OUT1 - OUT18!)
     * @return true if the channel is valid and I2C operation succeeded
     * 
     * This does not immediately affect the outputs; you must call update() to do that after
     * setting the PWM values or on/off state.
     */
    bool ledToggle(uint8_t channel);

    /**
     * @brief Gets the current state of the specifed LED channel
     * 
     * @param channel 0 - 17 (note: this is zero-based not OUT1 - OUT18!)
     * @return true if LED is currently on, false if off (or channel is not valid)
     */
    bool ledState(uint8_t channel) const;

    /**
     * @brief Turns on or off multiple LEDs using a bitmask
     * 
     * @param andMask 
     * @param orMask 
     * @return true if the I2C operation succeeded
     * 
     * This does not immediately affect the outputs; you must call update() to do that after
     * setting the PWM values or on/off state.
     * 
     * The mask is such that 0x00000001 is OUT1, 0x00000002 is OUT2, ...
     */
    bool ledControl(uint32_t andMask, uint32_t orMask);


    /**
     * @brief Update registers from temporary registers
     * 
     * @return true if the I2C operation succeeded
     */
    bool update();

    /**
     * @brief Resets the chip to default values
     * 
     * @return true if the I2C operation succeeded
     */
    bool reset();

    /**
     * @brief Put the device in software shutdown mode
     * 
     * @return true if the I2C operation succeeded
     */
    bool shutdown();

    /**
     * @brief Wake the device from shutdown mode and resume normal operation
     * 
     * @return true if the I2C operation succeeded
     */
    bool wake();

    /**
     * @brief Write a single register value
     * 
     * @param reg Register to write to (see REG_ constants in this class)
     * @param value Value to write (1 byte)
     * @return true if the I2C operation succeeded
     * 
     * This chip does not support reading register values!
     */
    bool writeRegister(uint8_t reg, uint8_t value);


    /**
     * @brief Write multiple sequential registers
     * 
     * @param reg First register to write to (see REG_ constants in this class)
     * @param values Pointer to an array of uint8_t values
     * @param numValues Number of values to write
     * @return true if the I2C operation succeeded
     * 
     * This chip does not support reading register values!
     */
    bool writeMultipleRegisters(uint8_t reg, const uint8_t *values, size_t numValues);


    static const uint8_t REG_SHUTDOWN = 0x00; //!< Shut down the chip
    static const uint8_t REG_PWM_FIRST = 0x01; //!< First of the 18 PWM registers 
    static const uint8_t REG_LED_CONTROL1 = 0x13; //!< LED on/off control register 1
    static const uint8_t REG_LED_CONTROL2 = 0x14; //!< LED on/off control register 2
    static const uint8_t REG_LED_CONTROL3 = 0x15; //!< LED on/off control register 3
    static const uint8_t REG_UPDATE = 0x16; //!< Update the outputs to match the registers
    static const uint8_t REG_RESET = 0x17; //!< Software reset the chip

    static const uint8_t DEFAULT_I2C_ADDRESS = 0x24; //!< 0b1010100 (this might actually be 0x54)

    static const uint8_t CHANNEL_LAST = 17; //!< 0-based! Used for bounds checking channel values

protected:
    uint8_t i2cAddress = DEFAULT_I2C_ADDRESS; //!< I2C address for the chip
    uint32_t currentLedState = 0; //!< Current LED on/off states
};

#endif // __SN3218_RK_H