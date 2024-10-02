#include "SN3218_RK.h"

SerialLogHandler logHandler(LOG_LEVEL_TRACE);
SYSTEM_THREAD(ENABLED);
SYSTEM_MODE(SEMI_AUTOMATIC);

SN3218_RK ledDriver;


bool i2cscan(TwoWire &wire);
void runTest();

void setup() {
    waitFor(Serial.isConnected, 10000);
    delay(2000);

    ledDriver.begin();
    ledDriver.reset();
    ledDriver.wake();

    i2cscan(Wire);   
    Particle.connect();
}

void loop() {
    runTest();
}


bool i2cscan(TwoWire &wire) {
    bool bFound = false;

	Log.info("Scanning I2C bus...");

	int numDevices = 0;

	// Address 0x79 to 0x7f are reserved, don't scan them
	for(byte address = 1; address < 0x78; address++) {
		wire.beginTransmission(address);
		byte error = wire.endTransmission();

		if (error == 0) {
			const char *deviceName = NULL;

			switch(address) {
                case 0x28: 
                    deviceName = "STUSB4500 USB-C PD Controller";
                    break;

                case 0x36:
                    deviceName = "MAX17043 Fuel Gauge";
                    break;

                case 0x48:
                    // This is both the temperature sensor and the ADS1015
                    deviceName = "TMP112A temperature sensor or ADS1015 ADC";
                    break;

                case 0x54:
                    deviceName = "SN3218 LED Driver";
                    break;

                case 0x61:
                    deviceName = "KG200Z LoRaWAN radio";
                    break;

                case 0x69:
                    deviceName = "AM1805 RTC/Watchdog";
                    break;

                case 0x6b:
                    deviceName = "bq24195 PMIC";
                    break;
			}
			if (deviceName != NULL) {
				Log.info("%s found at address 0x%02x", deviceName, address);
                bFound = true;
			}
			else {
				Log.info("Unknown I2C device found at address 0x%02x", address);
			}

			numDevices++;
		}
		else if (error == 4) {
			Log.info("Unknown error at address 0x%2x", address);
		}
	}

	Log.info("%d devices found", numDevices);

    return bFound;
}


void runTest() {
    static uint8_t channel = 0;

    if (channel == 0) {
        Log.info("LED test starting");
        ledDriver.ledControl(0, 0); // Turn all off
    }

    // Log.trace("testing channel=%d", channel);
    ledDriver.setPWM(channel, 128);
    ledDriver.ledOn(channel);
    ledDriver.update();
    delay(1000);            
    ledDriver.ledOff(channel);
    ledDriver.update();

    if (++channel >= 18) {
        channel = 0;
        Log.info("LED test finished");
    }

    
}
