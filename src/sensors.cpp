#include <Adafruit_AHTX0.h>
#include <Wire.h>
#include "sensors.hpp"


Sensors::Sensors(int moistPin) : aht(), moisturePin(moistPin) {}


void Sensors::begin() {
    /*
    if (!aht.begin()) {
        // Handle sensor initialization failure
        while (1) {
            delay(10);
        }
    }
        */
    pinMode(moisturePin, INPUT);
}

float Sensors::readHumidity() {
    try {
    sensors_event_t humidity, temp;
    aht.getEvent(&humidity, &temp);
    return humidity.relative_humidity;
    } catch (const std::exception& e) {
        // Handle sensor read error
        Serial.print("Error reading humidity: ");
        Serial.println(e.what());
        return NAN; // Return NaN to indicate an error
    }
}

float Sensors::readTemperature() {
    try {
    sensors_event_t humidity, temp;
    aht.getEvent(&humidity, &temp);
    return temp.temperature;
    } catch (const std::exception& e) {
        // Handle sensor read error
        Serial.print("Error reading temperature: ");
        Serial.println(e.what());
        return NAN; // Return NaN to indicate an error
    }
}

float Sensors::readMoisture() {
    try {
        // Placeholder for moisture reading logic
        // Replace with actual sensor reading code
        return analogRead(moisturePin);
    } catch (const std::exception& e) {
        // Handle sensor read error
        Serial.print("Error reading moisture: ");
        Serial.println(e.what());
        return NAN; // Return NaN to indicate an error
    }
}
