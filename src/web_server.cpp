#include "web_server.hpp"
#include "html.hpp"
#include "motors.hpp"
#include "sensors.hpp"
#include "constants.hpp"

void setupWebServer(
    WebServer &server,
    Sensors &sensors,
    MotorController &fan1,
    MotorController &fan2,
    MotorController &waterPump
) {
    // ルートハンドラ：HTMLページを返す
    server.on("/", HTTP_GET, [&server]() {
        server.send_P(200, "text/html", index_html);
    });

    // ステータスハンドラ：センサーデータをJSONで返す
    server.on("/status", HTTP_GET, [&server, &sensors]() {
        float humidity = sensors.readHumidity();
        float temperature = sensors.readTemperature();
        float moisture = sensors.readMoisture();

        // Build JSON response more efficiently
        String json;
        json.reserve(80); // Pre-allocate memory
        json = "{";
        json += "\"humidity\":" + String(humidity, 1) + ",";
        json += "\"temperature\":" + String(temperature, 1) + ",";
        json += "\"moisture\":" + String(moisture, 0);
        json += "}";

        server.send(200, "application/json", json);
    });

    // 制御ハンドラ：モーターの制御コマンドを受け取る
    server.on("/control", HTTP_GET, [&server, &fan1, &fan2, &waterPump]() {
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
        } else if (device == "fan2") {
            if (action == "fullSpeed") {
                fan2.fullSpeed();
                success = true;
            } else if (action == "stop") {
                fan2.stop();
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
