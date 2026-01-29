#pragma once

#include <Adafruit_AHTX0.h>

class Sensors {
private:
    Adafruit_AHTX0 aht;
public:
    Sensors();
    void begin(int moist);
    float readHumidity();
    float readTemperature();
    float readMoisture();
};