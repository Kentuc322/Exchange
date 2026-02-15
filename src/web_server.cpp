#include "web_server.hpp"
#include "html.hpp"
#include "motors.hpp"
#include "sensors.hpp"
#include "constants.hpp"

void setupWebServer(
    WebServer &server,
    Sensors &sensors,
    MotorController &fan1,
    MotorController &waterPump
) {
    // root handler: serve the main HTML page
    server.on("/", HTTP_GET, [&server]() {
        server.send_P(200, "text/html", index_html);
    });

    // status handler: return sensor data as JSON
    server.on("/status", HTTP_GET, [&server, &sensors]() {
        float humidity = sensors.readHumidity();
        float temperature = sensors.readTemperature();
        float moisture = sensors.readMoisture();

        // Build JSON response
        String humidityValue = isnan(humidity) ? "null" : String(humidity, 1);
        String temperatureValue = isnan(temperature) ? "null" : String(temperature, 1);
        String moistureValue = isnan(moisture) ? "null" : String(moisture, 1);
        String json;
        json.reserve(96); // Pre-allocate memory
        json = "{";
        json += "\"humidity\":" + humidityValue + ",";
        json += "\"temperature\":" + temperatureValue + ",";
        json += "\"moisture\":" + moistureValue;
        json += "}";

        server.send(200, "application/json", json);
    });

    // control handler: receive control commands for fans and pump
    server.on("/control", HTTP_GET, [&server, &fan1, &waterPump]() {
        if (!server.hasArg("device") || !server.hasArg("action")) {
            server.send(400, "text/plain", "Missing parameters");
            return;
        }
        
        String device = server.arg("device");
        String action = server.arg("action");
        bool success = false;

        if (device == "fan1") {
            if (action == "fullSpeed") {
                fan1.fullSpeed();
                success = true;
            } else if (action == "stop") {
                fan1.stop();
                success = true;
            }
        } else if (device == "pump") {
            if (action == "fullSpeed") {
                waterPump.fullSpeed();
                success = true;
            } else if (action == "stop") {
                waterPump.stop();
                success = true;
            }
        }
        
        if (success) {
            server.send(200, "text/plain", "OK");
        } else {
            server.send(400, "text/plain", "Invalid device or action");
        }
    });

    server.begin();
}
