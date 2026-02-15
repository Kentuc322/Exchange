#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "motors.hpp"
#include "sensors.hpp"
#include "constants.hpp"
#include "web_server.hpp"

Sensors sensors(Config::MOISTURE_SENSOR_PIN);
WebServer server(80);

MotorController fan1 = MotorController(Config::FAN1_PIN, Config::FAN1_CHANNEL); // create motor controller: pin 5, channel 0: fan 1
MotorController fan2 = MotorController(Config::FAN2_PIN, Config::FAN2_CHANNEL); // create motor controller: pin 6, channel 1: fan 2
MotorController water_pump = MotorController(Config::WATER_PUMP_PIN, Config::WATER_PUMP_CHANNEL); // create motor controller: pin 7, channel 2: water pump

void setup() {
    delay(1000); // Wait 1 second
    Serial.begin(115200); // initialize serial communication
    delay(500);
    Serial.println("\n\n=== System Starting ===");
    
    // Connect to WiFi
    Serial.println("[1/5] Connecting to WiFi...");
    WiFi.softAP(Config::WIFI_SSID, Config::WIFI_PASSWORD);
    IPAddress IP = WiFi.softAPIP();
    Serial.println("WiFi connected!");
    Serial.print("IP address: ");
    Serial.println(IP);
    
    Serial.println("[2/5] Setting up web server...");
    setupWebServer(server, sensors, fan1, fan2, water_pump); // setup web server routes
    Serial.println("  (^_^)  Web server initialized!");
    
    Serial.println("[3/5] Initializing sensors...");
    sensors.begin(); // initialize sensors
    Serial.println("  (^_^)  Sensors initialized!");
    
    Serial.println("[4/5] Initializing motor controllers...");
    
    if (Config::ENABLE_FAN1) {
        Serial.println("  - Initializing fan1 (pin 5, channel 0)...");
        fan1.begin(); // initialize motor controller
        Serial.println("  - Fan1 initialized");
    } else {
        Serial.println("  - Fan1 disabled");
    }
    
    if (Config::ENABLE_FAN2) {
        Serial.println("  - Initializing fan2 (pin 6, channel 1)...");
        fan2.begin(); // initialize motor controller
        Serial.println("  - Fan2 initialized");
    } else {
        Serial.println("  - Fan2 disabled");
    }
    
    if (Config::ENABLE_WATER_PUMP) {
        Serial.println("  - Initializing water pump (pin 7, channel 2)...");
        water_pump.begin(); // initialize motor controller
        Serial.println("  - Water pump initialized");
    } else {
        Serial.println("  - Water pump disabled");
    }
    
    Serial.println("  (^_^)  Motor controllers initialized!");
    
    Serial.println("[5/5] Setup completed!!!\n");
}

void loop() {
    // Handle web server requests
    server.handleClient();
    
    // Read sensor data
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
            water_pump.fullSpeed(); // turn on water pump
        } else {
            water_pump.stop(); // turn off water pump
        }
    }

    // Only control fans if temperature and humidity sensors are available
    if (sensors.isAHTAvailable() && !isnan(temperature) && !isnan(humidity)) {
        if (temperature > Config::TEMP_THRESHOLD || humidity > Config::HUMIDITY_THRESHOLD) {
            if (Config::ENABLE_FAN1) fan1.fullSpeed();
            if (Config::ENABLE_FAN2) fan2.fullSpeed();
        } else {
            if (Config::ENABLE_FAN1) fan1.stop();
            if (Config::ENABLE_FAN2) fan2.stop();
        }
    } else {
        // Keep fans off if no temperature/humidity data
        if (Config::ENABLE_FAN1) fan1.stop();
        if (Config::ENABLE_FAN2) fan2.stop();
    }

    delay(1000);
}