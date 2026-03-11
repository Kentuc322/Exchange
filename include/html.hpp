#pragma once

#include <Arduino.h>

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="ja">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Watering Robot Dashboard</title>
  <style>
    :root {
      --bg-color: #f0f2f5;
      --card-bg: #ffffff;
      --primary: #4a90e2;
      --danger: #e74c3c;
      --success: #2ecc71;
      --text: #333;
    }
    body { font-family: 'Segoe UI', sans-serif; background: var(--bg-color); color: var(--text); margin: 0; padding: 20px; display: flex; flex-direction: column; align-items: center; }
    h1 { margin-bottom: 20px; font-weight: 600; color: #444; }
    
    .dashboard { display: grid; grid-template-columns: repeat(auto-fit, minmax(300px, 1fr)); gap: 20px; width: 100%; max-width: 800px; }
    
    .card { background: var(--card-bg); border-radius: 15px; padding: 20px; box-shadow: 0 4px 15px rgba(0,0,0,0.05); transition: transform 0.2s; }
    .card:hover { transform: translateY(-2px); }
    .card h2 { margin-top: 0; font-size: 1.2rem; color: #666; border-bottom: 2px solid #f0f0f0; padding-bottom: 10px; }
    .auth-card { width: 100%; max-width: 800px; margin-bottom: 20px; }
    .auth-row { display: flex; align-items: center; justify-content: space-between; gap: 12px; flex-wrap: wrap; }
    .auth-user { color: #555; font-size: 0.95rem; }
    
    /* Sensor Styles */
    .sensor-grid { display: grid; grid-template-columns: 1fr 1fr; gap: 15px; }
    .sensor-item { text-align: center; }
    .sensor-value { font-size: 2.5rem; font-weight: bold; color: var(--primary); }
    .sensor-unit { font-size: 1rem; color: #888; }
    .moisture-box { margin-top: 15px; text-align: center; }

    /* Control Styles */
    .control-group { display: flex; justify-content: space-between; align-items: center; margin-bottom: 15px; }
    .control-label { font-weight: 500; font-size: 1.1rem; }
    .btn-group { display: flex; gap: 10px; }
    
    button { border: none; padding: 10px 20px; border-radius: 8px; cursor: pointer; font-weight: bold; transition: opacity 0.2s; color: white; }
    button:active { transform: scale(0.98); }
    button:hover { opacity: 0.9; }
    
    .btn-on { background: var(--success); box-shadow: 0 4px 10px rgba(46, 204, 113, 0.3); }
    .btn-off { background: var(--danger); box-shadow: 0 4px 10px rgba(231, 76, 60, 0.3); }
    .btn-auth { background: #4a90e2; box-shadow: 0 4px 10px rgba(74, 144, 226, 0.3); }
    .btn-muted { background: #7f8c8d; box-shadow: 0 4px 10px rgba(127, 140, 141, 0.3); }
    button[disabled] { cursor: not-allowed; opacity: 0.6; box-shadow: none; }

    /* Status Indicator */
    #connection-status { position: fixed; bottom: 10px; right: 10px; font-size: 0.8rem; color: #888; }
  </style>
</head>
<body>

  <h1>🌱 Watering Robot Monitor</h1>

  <div class="card auth-card">
    <h2>Authentication</h2>
    <div class="auth-row">
      <div class="auth-user" id="auth-user">Not signed in</div>
      <div class="btn-group">
        <button id="login-btn" class="btn-auth">Google Login</button>
        <button id="logout-btn" class="btn-muted" disabled>Logout</button>
      </div>
    </div>
  </div>

  <div class="dashboard">
    <!-- Monitor Section -->
    <div class="card">
      <h2>Environment Sensors</h2>
      <div class="sensor-grid">
        <div class="sensor-item">
          <div class="sensor-value" id="temp">--</div>
          <div class="sensor-unit">Temperature (°C)</div>
        </div>
        <div class="sensor-item">
          <div class="sensor-value" id="hum">--</div>
          <div class="sensor-unit">Humidity (%)</div>
        </div>
      </div>
      <div class="moisture-box">
        <div class="sensor-value" id="moist">--</div>
        <div class="sensor-unit">Soil Moisture (%)</div>
      </div>
    </div>

    <!-- Control Section -->
    <div class="card">
      <h2>Manual Controls</h2>
      
      <div class="control-group">
        <span class="control-label">Fan 1 (Cooling)</span>
        <div class="btn-group">
          <button class="btn-on requires-auth" onclick="control('fan1', 'fullSpeed')">FULL</button>
          <button class="btn-off requires-auth" onclick="control('fan1', 'stop')">STOP</button>
        </div>
      </div>

      <div class="control-group">
        <span class="control-label">Water Pump</span>
        <div class="btn-group">
          <button class="btn-on requires-auth" onclick="control('pump', 'fullSpeed')">POUR</button>
          <button class="btn-off requires-auth" onclick="control('pump', 'stop')">STOP</button>
        </div>
      </div>
    </div>
  </div>

  <div id="connection-status">Connecting...</div>

<script type="module">
  import { initializeApp } from 'https://www.gstatic.com/firebasejs/10.14.1/firebase-app.js';
  import {
    getAuth,
    GoogleAuthProvider,
    signInWithPopup,
    signOut,
    onAuthStateChanged
  } from 'https://www.gstatic.com/firebasejs/10.14.1/firebase-auth.js';
  import {
    getDatabase,
    ref,
    onValue,
    update
  } from 'https://www.gstatic.com/firebasejs/10.14.1/firebase-database.js';

  // Step 1: Replace with values from your Firebase project settings.
  const firebaseConfig = {
    apiKey: 'YOUR_API_KEY',
    authDomain: 'YOUR_PROJECT_ID.firebaseapp.com',
    databaseURL: 'https://YOUR_PROJECT_ID-default-rtdb.firebaseio.com',
    projectId: 'YOUR_PROJECT_ID',
    appId: 'YOUR_APP_ID'
  };

  const app = initializeApp(firebaseConfig);
  const auth = getAuth(app);
  const db = getDatabase(app);
  const provider = new GoogleAuthProvider();
  const devicePath = 'device_01';
  const sensorsRef = ref(db, `${devicePath}/sensors`);
  const controlsRef = ref(db, `${devicePath}/controls`);

  const statusEl = document.getElementById('connection-status');
  const authUserEl = document.getElementById('auth-user');
  const tempEl = document.getElementById('temp');
  const humEl = document.getElementById('hum');
  const moistEl = document.getElementById('moist');
  const loginBtn = document.getElementById('login-btn');
  const logoutBtn = document.getElementById('logout-btn');
  const authRequiredButtons = document.querySelectorAll('.requires-auth');
  let stopSensorsListener = null;
  let stopControlsListener = null;

  function setStatus(message, color) {
    statusEl.innerText = message;
    statusEl.style.color = color;
  }

  function setControlsEnabled(enabled) {
    authRequiredButtons.forEach((button) => {
      button.disabled = !enabled;
    });
  }

  function formatSensorValue(value) {
    return (typeof value === 'number' && Number.isFinite(value)) ? value.toFixed(1) : '--';
  }

  function renderSensors(data = {}) {
    tempEl.innerText = formatSensorValue(data.temperature);
    humEl.innerText = formatSensorValue(data.humidity);
    moistEl.innerText = formatSensorValue(data.soil_moisture);
  }

  function stopRealtimeListeners() {
    if (typeof stopSensorsListener === 'function') {
      stopSensorsListener();
      stopSensorsListener = null;
    }
    if (typeof stopControlsListener === 'function') {
      stopControlsListener();
      stopControlsListener = null;
    }
  }

  function startRealtimeListeners() {
    stopRealtimeListeners();

    stopSensorsListener = onValue(
      sensorsRef,
      (snapshot) => {
        renderSensors(snapshot.val() || {});
        setStatus('Realtime sensor stream active', 'green');
      },
      (error) => {
        console.error('Sensor stream error:', error);
        setStatus('Sensor stream error', '#e74c3c');
      }
    );

    stopControlsListener = onValue(
      controlsRef,
      (snapshot) => {
        const controls = snapshot.val() || {};
        console.log('controls updated:', controls);
      },
      (error) => {
        console.error('Controls stream error:', error);
      }
    );
  }

  async function handleLogin() {
    try {
      await signInWithPopup(auth, provider);
    } catch (error) {
      console.error('Google sign-in failed:', error);
      alert('Googleログインに失敗しました。FirebaseのAuthorized domains設定を確認してください。');
    }
  }

  async function handleLogout() {
    try {
      await signOut(auth);
    } catch (error) {
      console.error('Sign-out failed:', error);
      alert('ログアウトに失敗しました。');
    }
  }

  loginBtn.addEventListener('click', handleLogin);
  logoutBtn.addEventListener('click', handleLogout);

  onAuthStateChanged(auth, (user) => {
    if (user) {
      authUserEl.innerText = `Signed in: ${user.displayName || user.email}`;
      loginBtn.disabled = true;
      logoutBtn.disabled = false;
      setControlsEnabled(true);
      setStatus('Signed in. Connecting realtime stream...', 'green');
      startRealtimeListeners();
      return;
    }

    stopRealtimeListeners();
    renderSensors();
    authUserEl.innerText = 'Not signed in';
    loginBtn.disabled = false;
    logoutBtn.disabled = true;
    setControlsEnabled(false);
    setStatus('Please sign in with Google', '#888');
  });

  // Step 2: Write control states to /device_01/controls.
  window.control = async function control(device, action) {
    if (!auth.currentUser) {
      alert('操作前にGoogleログインしてください。');
      return;
    }

    const isOn = action === 'fullSpeed';
    let payload = null;

    if (device === 'pump') {
      payload = { is_watering: isOn };
    } else if (device === 'fan1') {
      payload = { is_ventilating: isOn };
    }

    if (!payload) {
      console.warn('Unsupported device/action:', device, action);
      return;
    }

    try {
      await update(controlsRef, payload);
      setStatus(`Control updated: ${JSON.stringify(payload)}`, '#2ecc71');
    } catch (error) {
      console.error('Failed to update controls:', error);
      setStatus('Failed to update controls', '#e74c3c');
    }
  };

  renderSensors();
  setControlsEnabled(false);
  setStatus('Checking authentication...', '#666');
</script>
</body>
</html>
)rawliteral";