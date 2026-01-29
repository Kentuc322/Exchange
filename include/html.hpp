#pragma once

#include <Arduino.h>

const char index_html[] PROGMEM = R"rawliteral()
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

    /* Status Indicator */
    #connection-status { position: fixed; bottom: 10px; right: 10px; font-size: 0.8rem; color: #888; }
  </style>
</head>
<body>

  <h1>🌱 Watering Robot Monitor</h1>

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
        <div class="sensor-unit">Soil Moisture (Raw)</div>
      </div>
    </div>

    <!-- Control Section -->
    <div class="card">
      <h2>Manual Controls</h2>
      
      <div class="control-group">
        <span class="control-label">Fan 1 (Cooling)</span>
        <div class="btn-group">
          <button class="btn-on" onclick="control('fan1', 'fullSpeed')">FULL</button>
          <button class="btn-off" onclick="control('fan1', 'stop')">STOP</button>
        </div>
      </div>

      <div class="control-group">
        <span class="control-label">Fan 2 (Exhaust)</span>
        <div class="btn-group">
          <button class="btn-on" onclick="control('fan2', 'fullSpeed')">FULL</button>
          <button class="btn-off" onclick="control('fan2', 'stop')">STOP</button>
        </div>
      </div>

      <div class="control-group">
        <span class="control-label">Water Pump</span>
        <div class="btn-group">
          <button class="btn-on" onclick="control('pump', 'fullSpeed')">POUR</button>
          <button class="btn-off" onclick="control('pump', 'stop')">STOP</button>
        </div>
      </div>
    </div>
  </div>

  <div id="connection-status">Connecting...</div>

<script>
  // センサーデータを定期的に取得 (AJAX Polling)
  function fetchSensorData() {
    fetch('/status')
      .then(response => response.json())
      .then(data => {
        document.getElementById('temp').innerText = data.temperature.toFixed(1);
        document.getElementById('hum').innerText = data.humidity.toFixed(1);
        document.getElementById('moist').innerText = data.moisture;
        document.getElementById('connection-status').innerText = 'System Online';
        document.getElementById('connection-status').style.color = 'green';
      })
      .catch(error => {
        console.error('Error:', error);
        document.getElementById('connection-status').innerText = 'Connection Lost';
        document.getElementById('connection-status').style.color = 'red';
      });
  }

  // 制御コマンドの送信
  function control(device, action) {
    fetch(`/control?device=${device}&action=${action}`)
      .then(response => {
        if(response.ok) console.log(`${device} set to ${action}`);
      });
  }

  // 1秒ごとに更新
  setInterval(fetchSensorData, 1000);
  fetchSensorData(); // 初回実行
</script>
</body>
</html>
)rawliteral";