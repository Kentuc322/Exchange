#include "firebase_service.hpp"

#include <Arduino.h>
#include "constants.hpp"

FirebaseService::FirebaseService(Sensors &sensors, MotorController &fan1, MotorController &waterPump)
    : _sensors(sensors),
      _fan1(fan1),
      _waterPump(waterPump),
      _firebaseReady(false),
      _controlsStreamStarted(false),
      _isWatering(false),
      _isVentilating(false),
      _wateringStartMs(0),
      _lastSensorUploadMs(0),
      _lastStreamRetryMs(0) {}

bool FirebaseService::begin() {
    _firebaseConfig.api_key = Config::FIREBASE_API_KEY;
    _firebaseConfig.database_url = Config::FIREBASE_DATABASE_URL;

    _firebaseAuth.user.email = Config::FIREBASE_USER_EMAIL;
    _firebaseAuth.user.password = Config::FIREBASE_USER_PASSWORD;

    Firebase.reconnectNetwork(true);
    Firebase.begin(&_firebaseConfig, &_firebaseAuth);

    Serial.println("Authenticating to Firebase with email/password...");
    unsigned long start = millis();
    while (!Firebase.ready() && (millis() - start) < Config::FIREBASE_AUTH_TIMEOUT_MS) {
        Serial.print('.');
        delay(250);
    }
    Serial.println();

    _firebaseReady = Firebase.ready();
    if (!_firebaseReady) {
        Serial.println("Firebase authentication failed or timed out.");
        return false;
    }

    Serial.println("Firebase authentication success.");
    buildFirebasePaths();
    syncControlsFromFirebase();
    startControlsStream();
    return true;
}

void FirebaseService::loop() {
    if (!_firebaseReady || !Firebase.ready()) {
        return;
    }

    const unsigned long nowMs = millis();

    if (!_controlsStreamStarted && (nowMs - _lastStreamRetryMs) >= STREAM_RETRY_INTERVAL_MS) {
        _lastStreamRetryMs = nowMs;
        startControlsStream();
    }

    processControlsStream();

    if ((nowMs - _lastSensorUploadMs) >= SENSOR_UPLOAD_INTERVAL_MS) {
        _lastSensorUploadMs = nowMs;
        uploadSensorData();
    }

    runWateringFailsafe(nowMs);
}

bool FirebaseService::isReady() const {
    return _firebaseReady;
}

float FirebaseService::sanitizeSensorValue(float value) const {
    return isnan(value) ? 0.0f : value;
}

void FirebaseService::applyMotorStates() {
    if (Config::ENABLE_WATER_PUMP) {
        if (_isWatering) {
            _waterPump.fullSpeed();
        } else {
            _waterPump.stop();
        }
    }

    if (Config::ENABLE_FAN1) {
        if (_isVentilating) {
            _fan1.fullSpeed();
        } else {
            _fan1.stop();
        }
    }
}

void FirebaseService::setWateringState(bool enabled, bool resetTimer) {
    _isWatering = enabled;
    if (enabled) {
        if (resetTimer || _wateringStartMs == 0) {
            _wateringStartMs = millis();
        }
    } else {
        _wateringStartMs = 0;
    }
    applyMotorStates();
}

void FirebaseService::setVentilatingState(bool enabled) {
    _isVentilating = enabled;
    applyMotorStates();
}

void FirebaseService::buildFirebasePaths() {
    String basePath;
    basePath.reserve(40);
    basePath = "/";
    basePath += Config::FIREBASE_DEVICE_ID;

    _firebaseSensorsPath = basePath;
    _firebaseSensorsPath += "/sensors";

    _firebaseControlsPath = basePath;
    _firebaseControlsPath += "/controls";

    _firebaseWateringPath = _firebaseControlsPath;
    _firebaseWateringPath += "/is_watering";

    _firebaseVentilatingPath = _firebaseControlsPath;
    _firebaseVentilatingPath += "/is_ventilating";
}

void FirebaseService::syncControlsFromFirebase() {
    bool hasWatering = false;
    bool hasVentilating = false;
    bool remoteWatering = false;
    bool remoteVentilating = false;

    if (Firebase.RTDB.getBool(&_fbdoWrite, _firebaseWateringPath.c_str())) {
        remoteWatering = _fbdoWrite.boolData();
        hasWatering = true;
    } else {
        Serial.printf("Failed to read %s: %s\n", _firebaseWateringPath.c_str(), _fbdoWrite.errorReason().c_str());
    }

    if (Firebase.RTDB.getBool(&_fbdoWrite, _firebaseVentilatingPath.c_str())) {
        remoteVentilating = _fbdoWrite.boolData();
        hasVentilating = true;
    } else {
        Serial.printf("Failed to read %s: %s\n", _firebaseVentilatingPath.c_str(), _fbdoWrite.errorReason().c_str());
    }

    if (hasWatering) {
        setWateringState(remoteWatering, true);
    }

    if (hasVentilating) {
        setVentilatingState(remoteVentilating);
    }
}

void FirebaseService::startControlsStream() {
    if (Firebase.RTDB.beginStream(&_fbdoStream, _firebaseControlsPath.c_str())) {
        _controlsStreamStarted = true;
        Serial.printf("Controls stream started: %s\n", _firebaseControlsPath.c_str());
    } else {
        _controlsStreamStarted = false;
        Serial.printf("Controls stream start failed: %s\n", _fbdoStream.errorReason().c_str());
    }
}

void FirebaseService::processControlsStream() {
    if (!_controlsStreamStarted) {
        return;
    }

    if (!Firebase.RTDB.readStream(&_fbdoStream)) {
        const int httpCode = _fbdoStream.httpCode();
        if (httpCode != FIREBASE_ERROR_HTTP_CODE_OK && httpCode != FIREBASE_ERROR_HTTP_CODE_NO_CONTENT) {
            Serial.printf("Stream read error: %s\n", _fbdoStream.errorReason().c_str());
            _controlsStreamStarted = false;
        }
        return;
    }

    if (_fbdoStream.streamTimeout()) {
        Serial.println("Controls stream timeout, waiting for reconnect...");
    }

    if (!_fbdoStream.streamAvailable()) {
        return;
    }

    const String path = _fbdoStream.dataPath();
    const int dataType = _fbdoStream.dataTypeEnum();

    if (dataType == fb_esp_rtdb_data_type_boolean) {
        const bool value = _fbdoStream.boolData();
        if (path == "/is_watering") {
            setWateringState(value, true);
            Serial.printf("Remote control: is_watering = %s\n", value ? "true" : "false");
        } else if (path == "/is_ventilating") {
            setVentilatingState(value);
            Serial.printf("Remote control: is_ventilating = %s\n", value ? "true" : "false");
        }
        return;
    }

    if (dataType == fb_esp_rtdb_data_type_json || path == "/") {
        syncControlsFromFirebase();
    }
}

void FirebaseService::uploadSensorData() {
    float humidity = NAN;
    float temperature = NAN;
    const float moisture = _sensors.readMoisture();

    if (_sensors.isAHTAvailable()) {
        humidity = _sensors.readHumidity();
        temperature = _sensors.readTemperature();
    }

    FirebaseJson sensorsJson;
    sensorsJson.set("temperature", sanitizeSensorValue(temperature));
    sensorsJson.set("humidity", sanitizeSensorValue(humidity));
    sensorsJson.set("soil_moisture", sanitizeSensorValue(moisture));

    if (!Firebase.RTDB.setJSON(&_fbdoWrite, _firebaseSensorsPath.c_str(), &sensorsJson)) {
        Serial.printf("Sensor upload failed: %s\n", _fbdoWrite.errorReason().c_str());
        return;
    }

    Serial.printf(
        "Sensor upload ok: T=%.1f, H=%.1f, Soil=%.1f\n",
        sanitizeSensorValue(temperature),
        sanitizeSensorValue(humidity),
        sanitizeSensorValue(moisture)
    );
}

void FirebaseService::runWateringFailsafe(unsigned long nowMs) {
    if (!_isWatering || _wateringStartMs == 0) {
        return;
    }

    if ((nowMs - _wateringStartMs) < WATERING_FAILSAFE_MS) {
        return;
    }

    Serial.println("Watering failsafe triggered. Stopping pump and resetting Firebase flag.");
    setWateringState(false, false);

    FirebaseJson controlJson;
    controlJson.set("is_watering", false);
    if (!Firebase.RTDB.updateNode(&_fbdoWrite, _firebaseControlsPath.c_str(), &controlJson)) {
        Serial.printf("Failed to reset is_watering in Firebase: %s\n", _fbdoWrite.errorReason().c_str());
    }
}
