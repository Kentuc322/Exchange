#include <Arduino.h>
#include "motors.hpp"

void setup() {
    auto motor1 = MotorController(5, 0); // ピン5、チャンネル0のモーターコントローラーを作成
    auto motor2 = MotorController(6, 1); // ピン6、チャンネル1のモーターコントローラーを作成
    auto motor3 = MotorController(7, 2); // ピン7、チャンネル2のモーターコントローラーを作成
    motor1.begin(); // モーターコントローラーの初期化
    motor2.begin(); // モーターコントローラーの初期化
    motor3.begin(); // モーターコントローラーの初期化
}

void loop() {
  // put your main code here, to run repeatedly:
}