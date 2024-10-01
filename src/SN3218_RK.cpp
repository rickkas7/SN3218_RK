#include "SN3218_RK.h"


SN3218_RK::SN3218_RK(uint8_t i2cAddress) : i2cAddress(i2cAddress) {

}


bool SN3218_RK::setPWM(uint8_t channel, uint8_t value) {
    if (channel > CHANNEL_LAST) {
        return false;
    }

    return writeRegister(REG_PWM_FIRST + channel, value);
}


bool SN3218_RK::ledOn(uint8_t channel) {
    if (channel > CHANNEL_LAST) {
        return false;
    }

    return ledControl(0, 1 << channel);
}

bool SN3218_RK::ledOff(uint8_t channel) {
    if (channel > CHANNEL_LAST) {
        return false;
    }

    return ledControl(~(1 << channel), 0);
}

bool SN3218_RK::ledToggle(uint8_t channel) {
    bool bResult = false;
    
    if (ledState(channel)) {
        bResult = ledOff(channel);
    }
    else {
        bResult = ledOn(channel);
    }
    return bResult;
}

bool SN3218_RK::ledState(uint8_t channel) const {
    if (channel > CHANNEL_LAST) {
        return false;
    }

    return (currentLedState & (1 << channel)) != 0;
}



bool SN3218_RK::ledControl(uint32_t andMask, uint32_t orMask) {
    
    currentLedState &= andMask;
    currentLedState |= orMask;

    uint8_t values[3];
    values[0] = currentLedState & 0x3f;
    values[1] = (currentLedState >> 5) & 0x3f;
    values[2] = (currentLedState >> 10) & 0x3f;

    return writeMultipleRegisters(REG_LED_CONTROL1, values, sizeof(values));
}


bool SN3218_RK::update() {
    return writeRegister(REG_UPDATE, 0);
}

bool SN3218_RK::reset() {
    currentLedState = 0;
    return writeRegister(REG_RESET, 0);
}

bool SN3218_RK::shutdown() {
    return writeRegister(REG_SHUTDOWN, 0);
}

bool SN3218_RK::wake() {
    return writeRegister(REG_SHUTDOWN, 1);
}


bool SN3218_RK::writeRegister(uint8_t reg, uint8_t value) {
    return writeMultipleRegisters(reg, &value, 1);
}


bool SN3218_RK::writeMultipleRegisters(uint8_t reg, const uint8_t *values, size_t numValues) {

    Wire.beginTransmission(i2cAddress);

    Wire.write(reg);
    for(size_t ii = 0; ii < numValues; ii++) {
        Wire.write(values[ii]);
    }

    int stat = Wire.endTransmission();    // stop transmitting

    return (stat == 0);
}

