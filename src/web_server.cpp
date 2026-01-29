#include "web_server.hpp"
#include "html.hpp"
#include "motors.hpp"
#include "sensors.hpp"
#include "constants.hpp"

void setupWebServer(WebServer &server) {
    // ルートハンドラ：HTMLページを返す
    server.on("/", HTTP_GET, []() {
        server.send_P(200, "text/html", index_html);
    });

    // ステータスハンドラ：センサーデータをJSONで返す
    server.on("/status", HTTP_GET, []() {
        float humidity = sensors.readHumidity();
        float temperature = sensors.readTemperature();
        float moisture = sensors.readMoisture();

        String json = "{";
        json += "\"humidity\":" + String(humidity, 1) + ",";
        json += "\"temperature\":" + String(temperature, 1) + ",";
        json += "\"moisture\":" + String(moisture);
        json += "}";

        server.send(200, "application/json", json);
    });

    // 制御ハンドラ：モーターの制御コマンドを受け取る
    server.on("/control", HTTP_GET, []() {
        String device = server.arg("device");
        String action = server.arg("action");

        if (device == "fan1") {
            if (action == "fullSpeed") {
                fan1.fullSpeed();
            } else if (action == "stop") {
                fan1.stop();
            }
        } else if (device == "fan2") {
            if (action == "fullSpeed") {
                fan2.fullSpeed();
            } else if (action == "stop") {
                fan2.stop();
            }
        } else if (device == "pump") {
            if (action == "fullSpeed") {
                water_pump.fullSpeed();
            } else if (action == "stop") {
                water_pump.stop();
            }
        }

        server.send(200, "text/plain", "OK");
    });

    server.begin();
}