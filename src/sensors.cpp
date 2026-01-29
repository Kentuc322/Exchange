#include <Adafruit_AHTX0.h>
#include <Wire.h>
#include "sensors.hpp"


class Sensors {
private:
    Adafruit_AHTX0 aht;
    int moist;

public:
    Sensors() : aht() {}

    void begin(int moi) {
        if (!aht.begin()) {
            // Handle sensor initialization failure
            while (1); delay(10);
        }
        moist = moi;
        pinMode(moist, INPUT);
    }

    float readHumidity() {
        sensors_event_t humidity, temp;
        aht.getEvent(&humidity, &temp);
        return humidity.relative_humidity;
    }

    float readTemperature() {
        sensors_event_t humidity, temp;
        aht.getEvent(&humidity, &temp);
        return temp.temperature;
    }

    float readMoisture() {
        // Placeholder for moisture reading logic
        // Replace with actual sensor reading code
        return analogRead(moist);
    }
};