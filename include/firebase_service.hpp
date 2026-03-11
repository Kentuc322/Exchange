#pragma once

#include <Arduino.h>
#include <Firebase_ESP_Client.h>
#include "motors.hpp"
#include "sensors.hpp"

class FirebaseService {
public:
    FirebaseService(Sensors &sensors, MotorController &fan1, MotorController &waterPump);

    bool begin();
    void loop();
    bool isReady() const;

private:
    // methods / setters
    float sanitizeSensorValue(float value) const;
    void applyMotorStates();
    void setWateringState(bool enabled, bool resetTimer);
    void setVentilatingState(bool enabled);
    void buildFirebasePaths();
    void syncControlsFromFirebase();
    void startControlsStream();
    void processControlsStream();
    void uploadSensorData();
    void runWateringFailsafe(unsigned long nowMs);

    Sensors &_sensors;
    MotorController &_fan1;
    MotorController &_waterPump;

    FirebaseData _fbdoWrite;
    FirebaseData _fbdoStream;
    FirebaseAuth _firebaseAuth;
    FirebaseConfig _firebaseConfig;

    bool _firebaseReady;
    bool _controlsStreamStarted;
    bool _isWatering;
    bool _isVentilating;

    unsigned long _wateringStartMs;
    unsigned long _lastSensorUploadMs;
    unsigned long _lastStreamRetryMs;

    String _firebaseSensorsPath;
    String _firebaseControlsPath;
    String _firebaseWateringPath;
    String _firebaseVentilatingPath;

    static constexpr unsigned long SENSOR_UPLOAD_INTERVAL_MS = 1000;
    static constexpr unsigned long WATERING_FAILSAFE_MS = 10000;
    static constexpr unsigned long STREAM_RETRY_INTERVAL_MS = 5000;
};
