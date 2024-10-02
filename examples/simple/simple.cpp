#include "SN3218_RK.h"

SerialLogHandler logHandler(LOG_LEVEL_TRACE);
SYSTEM_THREAD(ENABLED);
SYSTEM_MODE(AUTOMATIC);

SN3218_RK ledDriver;

int functionHandler(String cmd);

void setup() {
    // Uncomment this to see debugging messages
    // waitFor(Serial.isConnected, 10000); delay(2000);

    // LED driver setup
    ledDriver.begin();
    ledDriver.reset();
    ledDriver.wake();

    Particle.function("led", functionHandler);
}

void loop() {
}

int functionHandler(String cmd) {
    JSONValue outerObj = JSONValue::parseCopy(cmd);

    JSONObjectIterator iter(outerObj);
    while(iter.next()) {
        const char *key = (const char *) iter.name();
        int ledNum = 0;
        if (sscanf(key, "pwm%d", &ledNum) == 1) {
            int valueInt = iter.value().toInt();
            if (valueInt < 0 || valueInt > 255) {
                valueInt = 0;
            }

            ledDriver.setPWM(ledNum, valueInt);
            Log.info("pwm %d=%d", ledNum, valueInt);
        }

        if (sscanf(key, "led%d", &ledNum) == 1) {
            int valueInt = iter.value().toInt();
            if (valueInt) {
                ledDriver.ledOn(ledNum);
                Log.info("led %d on", ledNum);
            }
            else {
                ledDriver.ledOff(ledNum);
                Log.info("led %d off", ledNum);
            }
        }
    }

    ledDriver.update();

    return 0;
}
