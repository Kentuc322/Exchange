#include <Arduino.h>
#include "motors.hpp"
#include "sensors.hpp"
#include "constants.hpp"

Sensors sensors(A0);

auto fan1 = MotorController(5, 0); // create motor controller: pin 5, channel 0: fan 1
auto fan2 = MotorController(6, 1); // create motor controller: pin 6, channel 1: fan 2
auto water_pump = MotorController(7, 2); // create motor controller: pin 7, channel 2: water pump

void setup() {
    Serial.begin(115200); // initialize serial communication
    sensors.begin(); // initialize sensors
    fan1.begin(); // initialize motor controller
    fan2.begin(); // initialize motor controller
    water_pump.begin(); // initialize motor controller
}

void loop() {
    // put your main code here, to run repeatedly:
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