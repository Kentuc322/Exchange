#include <Adafruit_AHTX0.h>
#include <Wire.h>
#include "sensors.hpp"


Sensors::Sensors(int moistPin) : aht(), moisturePin(moistPin) {}


void Sensors::begin() {
    if (!aht.begin()) {
        // Handle sensor initialization failure
        while (1) {
            delay(10);
        }
    }
    pinMode(moisturePin, INPUT);
}

float Sensors::readHumidity() {
    sensors_event_t humidity, temp;
    aht.getEvent(&humidity, &temp);
    return humidity.relative_humidity;
}

float Sensors::readTemperature() {
    sensors_event_t humidity, temp;
    aht.getEvent(&humidity, &temp);
    return temp.temperature;
}

float Sensors::readMoisture() {
    // Placeholder for moisture reading logic
    // Replace with actual sensor reading code
    return analogRead(moisturePin);
}
