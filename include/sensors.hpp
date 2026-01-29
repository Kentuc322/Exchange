#pragma once

#include <Adafruit_AHTX0.h>

class Sensors {
private:
    Adafruit_AHTX0 aht;
    int moisturePin;
public:
    Sensors(int moistPin);
    void begin();
    float readHumidity();
    float readTemperature();
    float readMoisture();
};