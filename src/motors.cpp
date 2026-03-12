#include "motors.hpp"
#include <Arduino.h>

// コンストラクタ
MotorController::MotorController(uint8_t pin, uint8_t channel) 
    : _pin(pin), _channel(channel) {}

// 初期化：ピンの設定とPWMの割り当て
void MotorController::begin() {
    Serial.print("Initializing motor on pin ");
    Serial.println(_pin);
    
    pinMode(_pin, OUTPUT);
    Serial.println("  - Pin configured as OUTPUT");
    // PWM Setup
    #if ESP_ARDUINO_VERSION_MAJOR >= 3
        // For ESP32 Arduino core v3.0.0 and later, we can set the PWM frequency and resolution directly on the channel
        ledcAttachChannel(_pin, _freq, _resolution, _channel);
        Serial.println("  - PWM setup completed");
    #else
        // For older versions, we need to set the frequency and resolution globally for the timer
        // Note: This will affect all channels using the same timer, so it's less flexible
        const int timer = _channel / 4; // Each timer controls 4 channels
        ledcSetup(_channel, _freq, _resolution);
        ledcAttachPin(_pin, _channel);
        Serial.println("  - PWM setup completed");
    #endif
    
    // 最初は停止しておく
    stop();
    Serial.println("  - Motor stopped");
}

// スピード設定 (0 - 255)
void MotorController::setSpeed(uint8_t speed) {
    // 範囲外の値を制限する (念のため)
    if (speed > 255) speed = 255;
    
    ledcWrite(_channel, speed);
}

// 停止
void MotorController::stop() {
    ledcWrite(_channel, 0);
}
// 最大出力
void MotorController::fullSpeed() {
    ledcWrite(_channel, 255);
}