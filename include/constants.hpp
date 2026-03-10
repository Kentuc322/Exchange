#pragma once
#include <Arduino.h>
#include "secrets.hpp"

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

    // WiFi settings (actual values are defined in include/secrets.hpp)
    constexpr const char* WIFI_SSID = Secrets::WIFI_SSID;
    constexpr const char* WIFI_PASSWORD = Secrets::WIFI_PASSWORD;
    constexpr int WIFI_TIMEOUT_MS = 20000;
    constexpr int WIFI_RECONNECTION_MS = 30000;

    // Firebase settings (actual values are defined in include/secrets.hpp)
    constexpr const char* FIREBASE_API_KEY = Secrets::FIREBASE_API_KEY;
    constexpr const char* FIREBASE_DATABASE_URL = Secrets::FIREBASE_DATABASE_URL;
    constexpr const char* FIREBASE_USER_EMAIL = Secrets::FIREBASE_USER_EMAIL;
    constexpr const char* FIREBASE_USER_PASSWORD = Secrets::FIREBASE_USER_PASSWORD;
    constexpr const char* FIREBASE_DEVICE_ID = "device_01";
    constexpr int FIREBASE_AUTH_TIMEOUT_MS = 30000;
};
