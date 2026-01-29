#include "motors.hpp"
#include <Arduino.h>

// コンストラクタ
MotorController::MotorController(uint8_t pin, uint8_t channel) 
    : _pin(pin), _channel(channel) {}

// 初期化：ピンの設定とPWMの割り当て
void MotorController::begin() {
    pinMode(_pin, OUTPUT);
    
    // PWMの設定 (チャンネル, 周波数, 分解能)
    ledcSetup(_channel, _freq, _resolution);
    
    // ピンをチャンネルに接続
    ledcAttachPin(_pin, _channel);
    
    // 最初は停止しておく
    stop();
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