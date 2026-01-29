#pragma once

#include <Arduino.h>

class MotorController {
public:
    // コンストラクタ：ピン番号とPWMチャンネルを指定
    MotorController(uint8_t pin, uint8_t channel);

    // 初期化処理
    void begin();

    // スピード設定 (0 〜 255)
    void setSpeed(uint8_t speed);

    // 停止 (スピード 0)
    void stop();

    // 全開 (スピード 255)
    void fullSpeed();

private:
    uint8_t _pin;
    uint8_t _channel;
    const double _freq = 5000.0; // PWM周波数 (5kHz)
    const uint8_t _resolution = 8; // 分解能 (8bit = 0-255)
};
