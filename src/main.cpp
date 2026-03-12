#include <Arduino.h>
#include <WebServer.h>

#include "constants.hpp"
#include "firebase_service.hpp"
#include "motors.hpp"
#include "sensors.hpp"
#include "web_server.hpp"
#include "wifi_manager.hpp"

Sensors sensors(Config::MOISTURE_SENSOR_PIN);
WebServer server(80);

MotorController fan1(Config::FAN1_PIN, Config::FAN1_CHANNEL);
MotorController waterPump(Config::WATER_PUMP_PIN, Config::WATER_PUMP_CHANNEL);
FirebaseService firebaseService(sensors, fan1, waterPump);

bool wifiConnected = false;
unsigned long wifiReconnectionMs = 0;

void setupMotors() {
    Serial.println("[4/6] Initializing motor controllers...");

    if (Config::ENABLE_FAN1) {
        Serial.println("  - Initializing fan1 (pin 5, channel 0)...");
        fan1.begin();
        Serial.println("  - Fan1 initialized");
    } else {
        Serial.println("  - Fan1 disabled");
    }

    if (Config::ENABLE_WATER_PUMP) {
        Serial.println("  - Initializing water pump (pin 7, channel 2)...");
        waterPump.begin();
        Serial.println("  - Water pump initialized");
    } else {
        Serial.println("  - Water pump disabled");
    }

    Serial.println("  (^_^)  Motor controllers initialized!");
}

void setup() {
    delay(1000);
    Serial.begin(115200);
    const unsigned long serialWaitStart = millis();
    while (!Serial && (millis() - serialWaitStart) < 3000) {
        delay(10);
    }
    Serial.setDebugOutput(true);
    delay(200);
    Serial.println("\n\n=== System Starting ===");
    Serial.printf("Boot timestamp(ms): %lu\n", millis());

    Serial.println("[1/6] Connecting to WiFi...");
    wifiConnected = connectToWiFiStation();
    Serial.printf("[1/6] done (wifiConnected=%s, t=%lu ms)\n", wifiConnected ? "true" : "false", millis());

    Serial.println("[2/6] Setting up web server...");
    setupWebServer(server, sensors, fan1, waterPump);
    Serial.println("  (^_^)  Web server initialized!");
    Serial.printf("[2/6] done (t=%lu ms)\n", millis());

    Serial.println("[3/6] Initializing sensors...");
    sensors.begin();
    Serial.println("  (^_^)  Sensors initialized!");
    Serial.printf("[3/6] done (t=%lu ms)\n", millis());

    setupMotors();
    Serial.printf("[4/6] done (t=%lu ms)\n", millis());

    Serial.println("[5/6] Initializing Firebase auth...");
    if (wifiConnected) {
        firebaseService.begin();
    } else {
        wifiReconnectionMs = millis();
        Serial.println("Skipping Firebase init because WiFi is not connected.");
    }
    Serial.printf("[5/6] done (firebaseReady=%s, t=%lu ms)\n", firebaseService.isReady() ? "true" : "false", millis());

    Serial.println("[6/6] Setup completed!!!\n");
}

void loop() {
    if (!wifiConnected && (millis() - wifiReconnectionMs) >= Config::WIFI_RECONNECTION_MS) {
        Serial.println("Reconnecting...");
        wifiConnected = connectToWiFiStation();
        if (wifiConnected) {
            Serial.println("WiFi Reconnected!");
            firebaseService.begin();
        } else {
            Serial.println("WiFi Reconnection Failed");
            wifiReconnectionMs = millis();
        }
    }

    server.handleClient();
    firebaseService.loop();

    // Read sensors data
    float humidity = NAN;
    float temperature = NAN;
    float moisture = sensors.readMoisture();

    // Only read temperature and humidity if AHT sensor is available
    if (sensors.isAHTAvailable()) {
        humidity = sensors.readHumidity();
        temperature = sensors.readTemperature();
        
        if (isnan(humidity) || isnan(temperature)) {
            Serial.println("Failed to read from AHT sensor!");
        } else {
            Serial.print("Humidity: "); Serial.print(humidity); Serial.print(" %\t");
            Serial.print("Temperature: "); Serial.print(temperature); Serial.print(" °C\t");
        }
    }

    Serial.print("Moisture: "); Serial.print(moisture); Serial.println(" %");

    // control logic based on sensor readings (need to be considered)
    if (Config::ENABLE_WATER_PUMP) {
        if (moisture < Config::SOIL_THRESHOLD) {
            waterPump.fullSpeed(); // turn on water pump
        } else {
            waterPump.stop(); // turn off water pump
        }
    }

    // Only control fans if temperature and humidity sensors are available
    if (sensors.isAHTAvailable() && !isnan(temperature) && !isnan(humidity)) {
        if (temperature > Config::TEMP_THRESHOLD || humidity > Config::HUMIDITY_THRESHOLD) {
            if (Config::ENABLE_FAN1) fan1.fullSpeed();
        } else {
            if (Config::ENABLE_FAN1) fan1.stop();
        }
    } else {
        // Keep fans off if no temperature/humidity data
        if (Config::ENABLE_FAN1) fan1.stop();
    }

    delay(10);
}
