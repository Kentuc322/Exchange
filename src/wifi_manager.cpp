#include <Arduino.h>
#include <WiFi.h>
#include "constants.hpp"
#include "wifi_manager.hpp"

bool connectToWiFiStation() {
    // WiFi mode: Station
    WiFi.mode(WIFI_STA);
    WiFi.begin(Config::WIFI_SSID, Config::WIFI_PASSWORD);

    Serial.print("Connecting to WiFi SSID ");
    Serial.print(Config::WIFI_SSID);
    Serial.print(": ");

    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && (millis() - start) < Config::WIFI_TIMEOUT_MS) {
        Serial.print('.');
        delay(500);
    }
    Serial.println();

    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi connection failed.");
        return false;
    }

    Serial.println("WiFi connected.");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    return true;
}
