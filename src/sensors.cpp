#include <Adafruit_AHTX0.h>
#include <Wire.h>
#include "sensors.hpp"


Sensors::Sensors(int moistPin) : aht(), moisturePin(moistPin), ahtAvailable(false) {}



void Sensors::begin() {
    Serial.print("  - Moisture sensor configured on pin ");
    Serial.println(moisturePin);
    
    // Try to initialize AHT sensor with timeout protection
    Serial.println("  - Attempting to initialize AHT sensor...");
    
    // Check if I2C device exists before calling begin()
    Wire.begin();
    Wire.beginTransmission(0x38); // AHT default I2C address
    byte error = Wire.endTransmission();
    
    if (error == 0) {
        // Device found, try to initialize
        Serial.println("  - AHT sensor detected on I2C bus");
        if (aht.begin()) {
            Serial.println("  - AHT sensor initialized successfully!");
            ahtAvailable = true;
        } else {
            Serial.println("  - WARNING: AHT sensor found but initialization failed");
            ahtAvailable = false;
        }
    } else {
        // No device found
        Serial.println("  - No AHT sensor detected (temperature/humidity unavailable)");
        ahtAvailable = false;
    }
}

bool Sensors::isAHTAvailable() {
    return ahtAvailable;
}

float Sensors::readHumidity() {
    if (!ahtAvailable) {
        return NAN; // Return NaN if sensor is not available
    }
    
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
    if (!ahtAvailable) {
        return NAN; // Return NaN if sensor is not available
    }
    
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
        int rawValue = analogRead(moisturePin);
        
        // Convert to percentage (0-100%)
        // - High value (e.g., 4095) when dry (not in water/soil)
        // - Low value (e.g., 0-2000) when wet
        float moisture = map(rawValue, 4095, 0, 0, 100);
        
        // Clamp to 0-100 range
        if (moisture < 0) moisture = 0;
        if (moisture > 100) moisture = 100;
        
        // Debug
        /*
        Serial.print("[Moisture Debug] Raw: ");
        Serial.print(rawValue);
        Serial.print(" -> ");
        Serial.print(moisture);
        Serial.println("%");
        */
        
        return moisture;
    } catch (const std::exception& e) {
        // Handle sensor read error
        Serial.print("Error reading moisture: ");
        Serial.println(e.what());
        return NAN; // Return NaN to indicate an error
    }
}