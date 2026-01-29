#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "motors.hpp"
#include "sensors.hpp"
#include "constants.hpp"
#include "web_server.hpp"

Sensors sensors(A0);
WebServer server(80);

auto fan1 = MotorController(Config::FAN1_PIN, Config::FAN1_CHANNEL); // create motor controller: pin 5, channel 0: fan 1
auto fan2 = MotorController(Config::FAN2_PIN, Config::FAN2_CHANNEL); // create motor controller: pin 6, channel 1: fan 2
auto water_pump = MotorController(Config::WATER_PUMP_PIN, Config::WATER_PUMP_CHANNEL); // create motor controller: pin 7, channel 2: water pump

void setup() {
    Serial.begin(115200); // initialize serial communication
    
    // Connect to WiFi
    Serial.println("Connecting to WiFi...");
    WiFi.begin(Config::WIFI_SSID, Config::WIFI_PASSWORD);
    
    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED && 
           millis() - startTime < Config::WIFI_TIMEOUT_MS) {
        delay(500);
        Serial.print(".");
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWiFi connected!");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\nWiFi connection failed!");
    }
    
    sensors.begin(); // initialize sensors
    fan1.begin(); // initialize motor controller
    fan2.begin(); // initialize motor controller
    water_pump.begin(); // initialize motor controller
    setupWebServer(server, sensors, fan1, fan2, water_pump); // setup web server routes
}

void loop() {
    // Handle web server requests
    server.handleClient();
    
    // Read sensor data
    float humidity = sensors.readHumidity();
    float temperature = sensors.readTemperature();
    float moisture = sensors.readMoisture();

    if (isnan(humidity) || isnan(temperature)) {
        // Sensor read error handling
        Serial.println("Failed to read from AHT sensor!");
        return;
    }

    Serial.print("Humidity: "); Serial.print(humidity); Serial.print(" %\t");
    Serial.print("Temperature: "); Serial.print(temperature); Serial.print(" °C\t");
    Serial.print("Moisture: "); Serial.print(moisture); Serial.println(" %");

    // control logic based on sensor readings (need to be considered)
    if (moisture < Config::SOIL_THRESHOLD) {
        water_pump.fullSpeed(); // turn on water pump
    } else {
        water_pump.stop(); // turn off water pump
    }

    if (temperature > Config::TEMP_THRESHOLD || humidity > Config::HUMIDITY_THRESHOLD) {
        fan1.fullSpeed(); // turn on fan 1
        fan2.fullSpeed(); // turn on fan 2
    } else {
        fan1.stop(); // turn off fan 1
        fan2.stop(); // turn off fan 2
    }

    delay(1000);
}