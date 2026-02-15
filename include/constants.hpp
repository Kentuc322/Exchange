#pragma once
#include <Arduino.h>

namespace Config {
    // Hardware enable flags - set to false if hardware is not connected
    constexpr bool ENABLE_FAN1 = false;
    constexpr bool ENABLE_WATER_PUMP = false;
    constexpr bool ENABLE_AHT_SENSOR = true; // Will be auto-detected anyway
    
    constexpr int FAN1_CHANNEL = 0;
    constexpr int WATER_PUMP_CHANNEL = 2;

    constexpr int FAN1_PIN = 5;
    constexpr int WATER_PUMP_PIN = 7;

    constexpr int MOISTURE_SENSOR_PIN = 32;

    constexpr float HUMIDITY_THRESHOLD = 70.0; // Example threshold
    constexpr float TEMP_THRESHOLD = 30.0; // Example threshold
    constexpr int SOIL_THRESHOLD = 500; // Example threshold

    // WiFi settings
    constexpr const char* WIFI_SSID = "WateringRobotNetwork";
    constexpr const char* WIFI_PASSWORD = "SecurePass123";
    constexpr int WIFI_TIMEOUT_MS = 20000; // 10 seconds
};
