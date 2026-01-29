#include <Arduino.h>
#include "motors.hpp"
#include "sensors.hpp"

Sensors sensors;

auto motor1 = MotorController(5, 0); // create motor controller: pin 5, channel 0
auto motor2 = MotorController(6, 1); // create motor controller: pin 6, channel 1
auto motor3 = MotorController(7, 2); // create motor controller: pin 7, channel 2

void setup() {
    sensors.begin(A0); // initialize sensors with moisture sensor pin A0
    motor1.begin(); // initialize motor controller
    motor2.begin(); // initialize motor controller
    motor3.begin(); // initialize motor controller
}

void loop() {
  // put your main code here, to run repeatedly:
}