#include <WiFi.h>
#include <WebServer.h>

// =====================================================
// WIFI
// =====================================================

const char* ssid = "RobotCar";
const char* password = "12345678";

WebServer server(80);

// =====================================================
// MOTOR PINS
// =====================================================

// LEFT MOTOR SIDE
#define IN1 4
#define IN2 5
#define ENA 6

// RIGHT MOTOR SIDE
#define IN3 7
#define IN4 15
#define ENB 16

// =====================================================
// MOTOR SETTINGS
// =====================================================

int motorSpeed = 180;

// Safety timeout in milliseconds
// Robot stops if no movement command is received
const unsigned long safetyTimeout = 5000;

unsigned long lastCommand = 0;

// =====================================================
// STOP
// =====================================================

void stopRobot() {

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  ledcWrite(ENA, 0);
  ledcWrite(ENB, 0);
}

// =====================================================
// FORWARD
// =====================================================

void forward() {

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  ledcWrite(ENA, motorSpeed);
  ledcWrite(ENB, motorSpeed);

  lastCommand = millis();
}

// =====================================================
// BACKWARD
// =====================================================

void backward() {

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  ledcWrite(ENA, motorSpeed);
  ledcWrite(ENB, motorSpeed);

  lastCommand = millis();
}

// =====================================================
// LEFT
// LEFT SIDE STOPPED
// RIGHT SIDE MOVES
// =====================================================

void right() {

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  ledcWrite(ENA, 0);
  ledcWrite(ENB, motorSpeed);

  lastCommand = millis();
}

// =====================================================
// RIGHT
// RIGHT SIDE STOPPED
// LEFT SIDE MOVES
// =====================================================

void left() {

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  ledcWrite(ENA, motorSpeed);
  ledcWrite(ENB, 0);

  lastCommand = millis();
}

// =====================================================
// FORWARD LEFT
// =====================================================

void forwardRight() {

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  ledcWrite(ENA, motorSpeed / 4);
  ledcWrite(ENB, motorSpeed);

  lastCommand = millis();
}

// =====================================================
// FORWARD RIGHT
// =====================================================

void forwardLeft() {

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  ledcWrite(ENA, motorSpeed);
  ledcWrite(ENB, motorSpeed / 4);

  lastCommand = millis();
}

// =====================================================
// BACKWARD LEFT
// =====================================================

void backwardLeft() {

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  ledcWrite(ENA, motorSpeed / 4);
  ledcWrite(ENB, motorSpeed);

  lastCommand = millis();
}

// =====================================================
// BACKWARD RIGHT
// =====================================================

void backwardRight() {

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  ledcWrite(ENA, motorSpeed);
  ledcWrite(ENB, motorSpeed / 4);

  lastCommand = millis();
}

// =====================================================
// ROTATE LEFT
// =====================================================

void rotateLeft() {

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  ledcWrite(ENA, motorSpeed);
  ledcWrite(ENB, motorSpeed);

  lastCommand = millis();
}

// =====================================================
// ROTATE RIGHT
// =====================================================

void rotateRight() {

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  ledcWrite(ENA, motorSpeed);
  ledcWrite(ENB, motorSpeed);

  lastCommand = millis();
}

// =====================================================
// HTML PAGE
// =====================================================

const char MAIN_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
<title>RC-04 // Ground Unit Control</title>
<style>

/* =====================================================
   TOKENS
   ===================================================== */
:root{
  --bg:            #14171a;
  --panel-raised:  #2c3238;
  --panel-edge:    #3a4149;
  --amber:         #ffb000;
  --hazard:        #f2c14e;
  --danger:        #cf3a2e;
  --danger-dark:   #7d211a;
  --text:          #e8e6df;
  --text-dim:      #7d858c;
  --green-led:     #4ade80;
  --red-led:       #ef4444;
  --mono: "SFMono-Regular", Consolas, "Liberation Mono", Menlo, monospace;
  --label: "Segoe UI Semibold", "Arial Narrow", Arial, sans-serif;
}

*{
  box-sizing:border-box;
  user-select:none;
  -webkit-user-select:none;
  -webkit-touch-callout:none;
  -webkit-tap-highlight-color:transparent;
}
html,body{ margin:0; padding:0; width:100%; height:100%; }
body{
  background:var(--bg);
  color:var(--text);
  font-family:var(--label);
  overflow:hidden;
}

/* =====================================================
   HAZARD STRIPE
   ===================================================== */
.hazard{
  height:6px;
  background:repeating-linear-gradient(45deg,
    #1a1a1a 0 10px,
    var(--hazard) 10px 20px);
  opacity:.85;
}

/* =====================================================
   HEADER
   ===================================================== */
.header{
  height:64px;
  display:flex;
  align-items:center;
  justify-content:space-between;
  padding:0 26px;
  background:linear-gradient(180deg,#1c2126,#181c20);
  border-bottom:1px solid var(--panel-edge);
}
.brand{ display:flex; flex-direction:column; }
.brand .id{
  font-size:9px; letter-spacing:3px; color:var(--text-dim);
  font-family:var(--mono);
}
.brand .name{
  font-size:19px; font-weight:700; letter-spacing:1.5px;
}
.link{
  display:flex; align-items:center; gap:10px;
  font-family:var(--mono); font-size:11px; letter-spacing:1.5px; color:var(--text-dim);
}
.led{
  width:11px; height:11px; border-radius:50%;
  background:var(--green-led);
  box-shadow:0 0 8px 2px rgba(74,222,128,.65), inset 0 0 2px rgba(0,0,0,.4);
  border:1px solid rgba(0,0,0,.5);
  transition:background .15s, box-shadow .15s;
}
.led.lost{
  background:var(--red-led);
  box-shadow:0 0 8px 2px rgba(239,68,68,.65), inset 0 0 2px rgba(0,0,0,.4);
  animation:ledBlink 1s steps(1) infinite;
}
@keyframes ledBlink{ 50%{ opacity:.35; } }

/* =====================================================
   MAIN LAYOUT
   ===================================================== */
.main{
  height:calc(100vh - 64px - 6px - 6px - 36px);
  display:flex;
  justify-content:center;
  align-items:center;
  gap:34px;
  padding:18px;
}

.panel{
  position:relative;
  background:linear-gradient(155deg,#262c31,#20252a);
  border:1px solid var(--panel-edge);
  border-radius:8px;
  box-shadow:
    inset 0 1px 0 rgba(255,255,255,.03),
    inset 0 -1px 0 rgba(0,0,0,.4),
    0 10px 24px rgba(0,0,0,.35);
}
.bolt{
  position:absolute;
  width:9px; height:9px; border-radius:50%;
  background:radial-gradient(circle at 35% 30%, #6b7176, #24282c 70%);
  box-shadow:inset 0 0 1px rgba(0,0,0,.8), 0 1px 0 rgba(255,255,255,.05);
}
.bolt::after{
  content:"";
  position:absolute; top:50%; left:50%;
  width:6px; height:1px; background:#141618;
  transform:translate(-50%,-50%) rotate(35deg);
}
.bolt.tl{ top:9px; left:9px; }
.bolt.tr{ top:9px; right:9px; }
.bolt.bl{ bottom:9px; left:9px; }
.bolt.br{ bottom:9px; right:9px; }

/* =====================================================
   DRIVE MODULE
   ===================================================== */
.driveModule{
  width:308px;
  padding:34px 26px 26px;
}
.moduleLabel{
  font-size:9px; letter-spacing:3px; color:var(--text-dim);
  font-family:var(--mono); text-align:center; margin-bottom:16px;
}

.pad{
  display:grid;
  grid-template-columns:78px 78px 78px;
  grid-template-rows:78px 78px 78px;
  gap:7px;
  justify-content:center;
  touch-action:none; /* stop the browser hijacking presses as scroll/zoom */
}

.dirBtn{
  touch-action:none;
  border:1px solid var(--panel-edge);
  border-radius:9px;
  background:linear-gradient(180deg,var(--panel-raised),#242a2f);
  color:var(--text-dim);
  font-family:var(--mono);
  font-size:22px;
  display:flex; align-items:center; justify-content:center;
  box-shadow:0 3px 0 rgba(0,0,0,.35), inset 0 1px 0 rgba(255,255,255,.04);
  transition:transform .05s ease-out, box-shadow .05s ease-out,
             color .1s ease-out, background .1s ease-out;
}
.dirBtn.active{
  color:var(--amber);
  background:linear-gradient(180deg,#3a3120,#2a2418);
  box-shadow:0 1px 0 rgba(0,0,0,.35), inset 0 0 12px rgba(255,176,0,.25);
  transform:translateY(2px);
  text-shadow:0 0 10px rgba(255,176,0,.8);
}

/* e-stop, center cell */
.estopCell{
  display:flex; align-items:center; justify-content:center;
}
.estop{
  touch-action:none;
  width:60px; height:60px; border-radius:50%;
  background:radial-gradient(circle at 38% 32%, #e8564a, var(--danger) 55%, var(--danger-dark) 100%);
  border:3px solid #4a1512;
  box-shadow:
    0 4px 0 #5c1712,
    inset 0 2px 4px rgba(255,255,255,.25),
    0 0 0 3px var(--bg),
    0 0 0 6px var(--hazard);
  position:relative;
  transition:transform .05s ease-out, box-shadow .05s ease-out;
}
.estop::after{
  content:"";
  position:absolute; inset:8px; border-radius:50%;
  border:1px solid rgba(0,0,0,.25);
}
.estop.active{
  transform:translateY(3px);
  box-shadow:
    0 1px 0 #5c1712,
    inset 0 2px 6px rgba(0,0,0,.35),
    0 0 0 3px var(--bg),
    0 0 0 6px var(--hazard);
}

/* =====================================================
   TELEMETRY MODULE
   ===================================================== */
.telemetryModule{
  width:270px;
  padding:22px 24px 24px;
}

.readout{
  position:relative;
  background:#0f0c06;
  border:1px solid #3a2f10;
  border-radius:6px;
  padding:16px 16px 14px;
  overflow:hidden;
  margin-bottom:22px;
}
.readout::before{
  content:"";
  position:absolute; inset:0;
  background:repeating-linear-gradient(
    180deg, rgba(255,255,255,.025) 0 1px, transparent 1px 3px);
  pointer-events:none;
}
.readoutLabel{
  font-size:9px; letter-spacing:2.5px; color:#8a6a1f;
  font-family:var(--mono); margin-bottom:6px;
}
.readoutValue{
  font-family:var(--mono);
  font-size:25px; font-weight:600;
  color:var(--amber);
  text-shadow:0 0 10px rgba(255,176,0,.55), 0 0 2px rgba(255,176,0,.9);
  letter-spacing:1px;
}
.cursor{ animation:blink 1s steps(1) infinite; }
@keyframes blink{ 50%{ opacity:0; } }

.faderBlock{ margin-bottom:20px; }
.faderHeader{
  display:flex; justify-content:space-between; align-items:baseline;
  font-size:10.5px; letter-spacing:1.5px; color:var(--text-dim);
  margin-bottom:9px;
}
.faderHeader span.val{
  font-family:var(--mono); color:var(--amber); font-size:12px; letter-spacing:0;
}

input[type=range]{
  -webkit-appearance:none;
  width:100%; height:22px;
  background:transparent;
}
input[type=range]::-webkit-slider-runnable-track{
  height:6px; border-radius:3px;
  background:
    repeating-linear-gradient(90deg, var(--panel-edge) 0 1px, transparent 1px 10%),
    #14181c;
  box-shadow:inset 0 1px 3px rgba(0,0,0,.6);
}
input[type=range]::-webkit-slider-thumb{
  -webkit-appearance:none;
  width:14px; height:22px; margin-top:-8px;
  border-radius:3px;
  background:linear-gradient(180deg,#4a5157,#2b3136);
  border:1px solid #14181c;
  box-shadow:0 0 0 1px rgba(255,255,255,.05) inset, 0 2px 4px rgba(0,0,0,.5);
}
input[type=range]::-moz-range-track{
  height:6px; border-radius:3px;
  background:#14181c;
  box-shadow:inset 0 1px 3px rgba(0,0,0,.6);
}
input[type=range]::-moz-range-thumb{
  width:14px; height:22px; border-radius:3px;
  background:linear-gradient(180deg,#4a5157,#2b3136);
  border:1px solid #14181c;
}

.statusRow{
  display:flex; justify-content:space-between; align-items:center;
  padding-top:16px; border-top:1px solid var(--panel-edge);
  font-family:var(--mono); font-size:10.5px; letter-spacing:1px; color:var(--text-dim);
}
.statusRow b{ color:var(--text); font-weight:600; letter-spacing:1.5px; }

/* =====================================================
   FOOTER
   ===================================================== */
.footer{
  height:36px;
  display:flex; justify-content:center; align-items:center;
  background:#0f1215;
  color:#4c545c;
  font-size:9px; letter-spacing:2px;
  font-family:var(--mono);
}

/* =====================================================
   MOBILE
   ===================================================== */
@media (max-width:720px){
  .main{
    flex-direction:column;
    gap:16px;
    height:auto;
    padding:16px;
    overflow-y:auto;
  }
  body{ overflow:auto; }
  .driveModule{ width:min(92vw,320px); }
  .telemetryModule{ width:min(92vw,320px); padding:20px; }
  .pad{
    grid-template-columns:72px 72px 72px;
    grid-template-rows:72px 72px 72px;
  }
}
</style>
</head>
<body>

<div class="hazard"></div>

<div class="header">
  <div class="brand">
    <div class="id">UNIT-04 / GROUND VEHICLE</div>
    <div class="name">ROBOT CAR</div>
  </div>
  <div class="link">
    <div class="led" id="connectionDot"></div>
    <span id="status">LINK OK</span>
  </div>
</div>

<div class="main">

  <!-- ============ DRIVE MODULE ============ -->
  <div class="panel driveModule">
    <div class="bolt tl"></div><div class="bolt tr"></div>
    <div class="bolt bl"></div><div class="bolt br"></div>

    <div class="moduleLabel">DRIVE // DIRECTIONAL INPUT</div>

    <div class="pad">
      <button class="dirBtn" data-cmd="rotateLeft">⟲</button>
      <button class="dirBtn" data-cmd="forward">↑</button>
      <button class="dirBtn" data-cmd="rotateRight">⟳</button>

      <button class="dirBtn" data-cmd="left">←</button>
      <div class="estopCell">
        <button class="estop" id="estopBtn" aria-label="STOP"></button>
      </div>
      <button class="dirBtn" data-cmd="right">→</button>

      <button class="dirBtn" data-cmd="forwardLeft">↖</button>
      <button class="dirBtn" data-cmd="backward">↓</button>
      <button class="dirBtn" data-cmd="forwardRight">↗</button>
    </div>
  </div>

  <!-- ============ TELEMETRY MODULE ============ -->
  <div class="panel telemetryModule">
    <div class="bolt tl"></div><div class="bolt tr"></div>
    <div class="bolt bl"></div><div class="bolt br"></div>

    <div class="readout">
      <div class="readoutLabel">CURRENT COMMAND</div>
      <div class="readoutValue"><span id="commandDisplay">STOP</span><span class="cursor">_</span></div>
    </div>

    <div class="faderBlock">
      <div class="faderHeader">
        <span>MOTOR THROTTLE</span>
        <span class="val" id="speedValue">70%</span>
      </div>
      <input type="range" min="0" max="100" value="70" id="speed" oninput="changeSpeed(this.value)">
    </div>

    <div class="statusRow">
      <span>LINK</span>
      <b id="statusMono">CONNECTED</b>
    </div>
  </div>

</div>

<div class="footer">FACULTY OF ENGINEERING — ROBOTICS PROJECT</div>
<div class="hazard"></div>

<script>
/* =====================================================
   STATE
   ===================================================== */
let currentCommand = "";
let keepAlive = null;

const LABELS = {
  forward:"FORWARD", backward:"BACKWARD",
  left:"LEFT", right:"RIGHT",
  forwardLeft:"FWD + LEFT", forwardRight:"FWD + RIGHT",
  backwardLeft:"REV + LEFT", backwardRight:"REV + RIGHT",
  rotateLeft:"ROTATE CCW", rotateRight:"ROTATE CW",
  stop:"STOP"
};

/* =====================================================
   NETWORK
   ===================================================== */
function sendCommand(command){
  fetch("/" + command, { method:"GET", cache:"no-store" })
    .then(res => setConnected(res.ok))
    .catch(() => setConnected(false));
}

function setConnected(connected){
  const status = document.getElementById("status");
  const statusMono = document.getElementById("statusMono");
  const dot = document.getElementById("connectionDot");

  if (connected){
    status.innerText = "LINK OK";
    statusMono.innerText = "CONNECTED";
    dot.classList.remove("lost");
  } else {
    status.innerText = "LINK LOST";
    statusMono.innerText = "NO SIGNAL";
    dot.classList.add("lost");
  }
}

/* =====================================================
   DRIVE COMMANDS
   ===================================================== */
function startCommand(btn, command){
  if (currentCommand === command) return;
  currentCommand = command;

  document.querySelectorAll(".dirBtn").forEach(b => b.classList.remove("active"));
  if (btn) btn.classList.add("active");
  document.getElementById("commandDisplay").innerText = LABELS[command] || command.toUpperCase();

  sendCommand(command);

  if (keepAlive !== null) clearInterval(keepAlive);

  /* Resend every second so the ESP32 safety timeout doesn't cut the motors
     while the button is still held. */
  keepAlive = setInterval(() => sendCommand(currentCommand), 1000);
}

function stopCommand(btn){
  if (btn) btn.classList.remove("active");
  currentCommand = "";

  if (keepAlive !== null){
    clearInterval(keepAlive);
    keepAlive = null;
  }

  document.getElementById("commandDisplay").innerText = "STOP";
  sendCommand("stop");
}

/* =====================================================
   WIRE UP DIRECTION BUTTONS
   Uses pointer capture so a small finger movement off the
   button can't cancel the press early on touchscreens.
   ===================================================== */
document.querySelectorAll(".dirBtn").forEach(btn => {
  const cmd = btn.dataset.cmd;

  btn.addEventListener("pointerdown", e => {
    e.preventDefault();
    btn.setPointerCapture(e.pointerId);
    startCommand(btn, cmd);
  });

  const release = e => {
    if (btn.hasPointerCapture(e.pointerId)) btn.releasePointerCapture(e.pointerId);
    stopCommand(btn);
  };

  btn.addEventListener("pointerup", release);
  btn.addEventListener("pointercancel", release);
});

/* =====================================================
   E-STOP
   ===================================================== */
const estopBtn = document.getElementById("estopBtn");
estopBtn.addEventListener("pointerdown", e => {
  e.preventDefault();
  estopBtn.classList.add("active");
  document.querySelectorAll(".dirBtn").forEach(b => b.classList.remove("active"));
  currentCommand = "";
  if (keepAlive !== null){ clearInterval(keepAlive); keepAlive = null; }
  document.getElementById("commandDisplay").innerText = "STOP";
  sendCommand("stop");
});
["pointerup","pointercancel","pointerleave"].forEach(evt =>
  estopBtn.addEventListener(evt, () => estopBtn.classList.remove("active"))
);

/* =====================================================
   SPEED
   ===================================================== */
function changeSpeed(value){
  document.getElementById("speedValue").innerText = value + "%";

  fetch("/setSpeed?value=" + value, { method:"GET", cache:"no-store" })
    .then(res => { if (res.ok) setConnected(true); })
    .catch(() => setConnected(false));
}

/* =====================================================
   CONNECTION MONITOR
   ===================================================== */
setInterval(() => {
  fetch("/status", { method:"GET", cache:"no-store" })
    .then(res => setConnected(res.ok))
    .catch(() => setConnected(false));
}, 3000);

/* =====================================================
   PREVENT CONTEXT MENU
   ===================================================== */
document.addEventListener("contextmenu", e => e.preventDefault());

/* =====================================================
   STOP IF PAGE LOSES FOCUS
   ===================================================== */
window.addEventListener("blur", () => {
  if (currentCommand !== "") stopCommand(document.querySelector(".dirBtn.active"));
});
</script>

</body>
</html>
)rawliteral";


// =====================================================
// SET SPEED
// =====================================================

void handleSetSpeed() {

    if (!server.hasArg("value")) {

        server.send(
            400,
            "text/plain",
            "Missing value"
        );

        return;
    }


    int value =
        server.arg("value").toInt();


    value =
        constrain(
            value,
            0,
            100
        );


    motorSpeed =
        map(
            value,
            0,
            100,
            0,
            255
        );


    lastCommand = millis();


    server.send(
        200,
        "text/plain",
        "SPEED"
    );
}


// =====================================================
// SETUP
// =====================================================

void setup() {

    Serial.begin(115200);


    // =================================================
    // MOTOR PINS
    // =================================================

    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);

    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);


    // =================================================
    // PWM
    // =================================================

    ledcAttach(
        ENA,
        1000,
        8
    );

    ledcAttach(
        ENB,
        1000,
        8
    );


    // =================================================
    // INITIAL STOP
    // =================================================

    stopRobot();


    // =================================================
    // WIFI ACCESS POINT
    // =================================================

    WiFi.mode(WIFI_AP);

    /*
       Disable WiFi power saving.
       This helps maintain a more stable
       connection with the phone.
    */

    WiFi.setSleep(false);


    IPAddress localIP(
        192,
        168,
        4,
        1
    );

    IPAddress gateway(
        192,
        168,
        4,
        1
    );

    IPAddress subnet(
        255,
        255,
        255,
        0
    );


    WiFi.softAPConfig(
        localIP,
        gateway,
        subnet
    );


    /*
       Channel 6
       Hidden SSID = false
       Maximum clients = 4
    */

    WiFi.softAP(
        ssid,
        password,
        6,
        false,
        4
    );


    // =================================================
    // SERIAL INFORMATION
    // =================================================

    Serial.println();
    Serial.println(
        "================================"
    );

    Serial.println(
        "       ROBOT CAR ESP32-S3"
    );

    Serial.println(
        "================================"
    );

    Serial.print(
        "SSID: "
    );

    Serial.println(ssid);

    Serial.print(
        "PASSWORD: "
    );

    Serial.println(password);

    Serial.print(
        "IP ADDRESS: "
    );

    Serial.println(
        WiFi.softAPIP()
    );


    // =================================================
    // HOME PAGE
    // =================================================

    server.on(
        "/",
        []() {

            server.send_P(
                200,
                "text/html",
                MAIN_PAGE
            );

        }
    );


    // =================================================
    // FORWARD
    // =================================================

    server.on(
        "/forward",
        []() {

            forward();

            server.send(
                200,
                "text/plain",
                "FORWARD"
            );

        }
    );


    // =================================================
    // BACKWARD
    // =================================================

    server.on(
        "/backward",
        []() {

            backward();

            server.send(
                200,
                "text/plain",
                "BACKWARD"
            );

        }
    );


    // =================================================
    // LEFT
    // =================================================

    server.on(
        "/left",
        []() {

            left();

            server.send(
                200,
                "text/plain",
                "LEFT"
            );

        }
    );


    // =================================================
    // RIGHT
    // =================================================

    server.on(
        "/right",
        []() {

            right();

            server.send(
                200,
                "text/plain",
                "RIGHT"
            );

        }
    );


    // =================================================
    // FORWARD LEFT
    // =================================================

    server.on(
        "/forwardLeft",
        []() {

            forwardLeft();

            server.send(
                200,
                "text/plain",
                "FORWARD LEFT"
            );

        }
    );


    // =================================================
    // FORWARD RIGHT
    // =================================================

    server.on(
        "/forwardRight",
        []() {

            forwardRight();

            server.send(
                200,
                "text/plain",
                "FORWARD RIGHT"
            );

        }
    );


    // =================================================
    // BACKWARD LEFT
    // =================================================

    server.on(
        "/backwardLeft",
        []() {

            backwardLeft();

            server.send(
                200,
                "text/plain",
                "BACKWARD LEFT"
            );

        }
    );


    // =================================================
    // BACKWARD RIGHT
    // =================================================

    server.on(
        "/backwardRight",
        []() {

            backwardRight();

            server.send(
                200,
                "text/plain",
                "BACKWARD RIGHT"
            );

        }
    );


    // =================================================
    // ROTATE LEFT
    // =================================================

    server.on(
        "/rotateLeft",
        []() {

            rotateLeft();

            server.send(
                200,
                "text/plain",
                "ROTATE LEFT"
            );

        }
    );


    // =================================================
    // ROTATE RIGHT
    // =================================================

    server.on(
        "/rotateRight",
        []() {

            rotateRight();

            server.send(
                200,
                "text/plain",
                "ROTATE RIGHT"
            );

        }
    );


    // =================================================
    // STOP
    // =================================================

    server.on(
        "/stop",
        []() {

            stopRobot();

            lastCommand = millis();

            server.send(
                200,
                "text/plain",
                "STOP"
            );

        }
    );


    // =================================================
    // SPEED
    // =================================================

    server.on(
        "/setSpeed",
        handleSetSpeed
    );


    // =================================================
    // STATUS
    // =================================================

    server.on(
        "/status",
        []() {

            server.send(
                200,
                "text/plain",
                "CONNECTED"
            );

        }
    );


    // =================================================
    // START SERVER
    // =================================================

    server.begin();


    lastCommand =
        millis();


    Serial.println(
        "Web server started"
    );

    Serial.println(
        "Open: http://192.168.4.1"
    );

}


// =====================================================
// LOOP
// =====================================================

void loop() {

    server.handleClient();


    // =================================================
    // SAFETY TIMEOUT
    // =================================================

    if (
        millis() - lastCommand
        > safetyTimeout
    ) {

        stopRobot();

    }

}