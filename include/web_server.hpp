#pragma once

#include <Arduino.h>
#include <WebServer.h>
#include <WiFi.h>

// Forward declarations
class Sensors;
class MotorController;

void setupWebServer(
    WebServer &server, 
    Sensors &sensors,
    MotorController &fan1,
    MotorController &waterPump
);
