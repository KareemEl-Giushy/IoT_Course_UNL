#include <Arduino.h>

long perviousMillis = 0;

int interval = 1000;

void setup() {
    pinMode(PIN_LED_13, OUTPUT);
    // pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {

    long currentMillis = millis();

    if ((currentMillis  - perviousMillis) > interval) {
        perviousMillis = currentMillis;
        // save the last time you blinked an LED
        digitalWrite(PIN_LED_13, !digitalRead(PIN_LED_13));
    }
}