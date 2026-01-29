#pragma once
#include <Arduino.h>

namespace Config {
    constexpr int FAN1_CHANNEL = 0;
    constexpr int FAN2_CHANNEL = 1;
    constexpr int WATER_PUMP_CHANNEL = 2;

    constexpr int FAN1_PIN = 5;
    constexpr int FAN2_PIN = 6;
    constexpr int WATER_PUMP_PIN = 7;

    constexpr int MOISTURE_SENSOR_PIN = A0;
};