#include <Adafruit_AHTX0.h>


class Sensors {
private:
    Adafruit_AHTX0 aht;
    int moist;

public:
    Sensors() : aht() {}

    void begin(int moist) {
        if (!aht.begin()) {
            // Handle sensor initialization failure
            while (1);
        }
        moist = moist;
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