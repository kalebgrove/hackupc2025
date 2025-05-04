#include <WiFi.h>
#include <HTTPClient.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <Preferences.h>

struct Travel {
  String flightNumber;
  String gate;
  String destination;
  String boarding;
  String departure;
  String status;
};

std::vector<Travel> travels;
Preferences prefs;
WebServer server(80);

const char* ssid = "ESP32_AP";
const char* password = "esp12345";
const char* fallback_ssid = "ESP32_AP2";
const char* fallback_pass = "esp12345";
const char* serverUrl = "http://192.168.4.1/data.json";

unsigned long lastMillis = 0;
const unsigned long interval = 60000;  // 1 minute

void saveData() {
  prefs.begin("travel", false);
  prefs.putUInt("count", travels.size());
  for (int i = 0; i < travels.size(); i++) {
    prefs.putString(("f" + String(i)).c_str(), travels[i].flightNumber);
    prefs.putString(("g" + String(i)).c_str(), travels[i].gate);
    prefs.putString(("d" + String(i)).c_str(), travels[i].destination);
    prefs.putString(("b" + String(i)).c_str(), travels[i].boarding);
    prefs.putString(("p" + String(i)).c_str(), travels[i].departure);
    prefs.putString(("s" + String(i)).c_str(), travels[i].status);
  }
  prefs.end();
}

void loadData() {
  prefs.begin("travel", true);
  int count = prefs.getUInt("count", 0);
  travels.clear();
  for (int i = 0; i < count; i++) {
    Travel t;
    t.flightNumber = prefs.getString(("f" + String(i)).c_str(), "");
    t.gate = prefs.getString(("g" + String(i)).c_str(), "");
    t.destination = prefs.getString(("d" + String(i)).c_str(), "");
    t.boarding = prefs.getString(("b" + String(i)).c_str(), "");
    t.departure = prefs.getString(("p" + String(i)).c_str(), "");
    t.status = prefs.getString(("s" + String(i)).c_str(), "");
    travels.push_back(t);
  }
  prefs.end();
}

void handleRoot() {
  prefs.begin("news", true);
  String news = prefs.getString("latest", "There is no news");
  prefs.end();
  String page = "<h1>Flights</h1><ul>";
  for (auto& t : travels) {
    page += "<li><b>" + t.flightNumber + "</b> | " + t.destination +
            " | Gate: " + t.gate + " | Boarding: " + t.boarding +
            " | Departure: " + t.departure + " | Status: " + t.status + "</li>";
  }
  page += "</ul><h2>Vueling Flights</h2>";
  page += R"rawliteral(
<canvas id="gameCanvas" width="600" height="500" style="border:1px solid #000;width:100%;max-width:600px;"></canvas>
<br><button onclick="startGame()" style="margin-top:20px;font-size:24px;padding:15px 30px;">Start Game</button>
<h3>News:</h3><p>)rawliteral" + news + R"rawliteral(</p>
<script>
let ctx, canvas, interval;
let plane = { x: 150, y: 0, w: 40, h: 30, speed: 2, dest: "", gate: "" };
let catcher = { x: 160, y: 310, w: 80, h: 10 };
let score = 0;
let flights = [];

function drawPlane(p) {
  ctx.save();
  ctx.translate(p.x + p.w / 2, p.y + p.h / 2);
  ctx.rotate(90 * Math.PI / 180);
  ctx.font = "24px Arial";
  ctx.fillText("\u2708\ufe0f " + p.dest + " G" + p.gate, -p.w / 2, p.h / 2);
  ctx.restore();
}

function drawCatcher() {
  ctx.fillStyle = "black";
  ctx.fillRect(catcher.x, catcher.y, catcher.w, catcher.h);
}

function moveCatcher(e) {
  let rect = canvas.getBoundingClientRect();
  catcher.x = e.clientX - rect.left - catcher.w / 2;
}

function gameLoop() {
  ctx.clearRect(0, 0, canvas.width, canvas.height);
  plane.y += plane.speed;
  if (plane.y > canvas.height) resetPlane();
  if (plane.y + plane.h >= catcher.y && plane.x + plane.w >= catcher.x && plane.x <= catcher.x + catcher.w) {
    score++;
    plane.speed += 0.2;
    resetPlane();
  }
  drawPlane(plane);
  drawCatcher();
  ctx.fillStyle = "green";
  ctx.font = "30px Arial";
  ctx.fillText("Score: " + score, 10, 40);
}

function resetPlane() {
  plane.x = Math.random() * (canvas.width - plane.w);
  plane.y = 0;
  if (flights.length > 0) {
    let f = flights[Math.floor(Math.random() * flights.length)];
    plane.dest = f.destination;
    plane.gate = f.gate;
  }
}

function startGame() {
  if (interval) clearInterval(interval);
  resetPlane();
  interval = setInterval(gameLoop, 20);
}

window.onload = () => {
  canvas = document.getElementById("gameCanvas");
  ctx = canvas.getContext("2d");
  canvas.addEventListener("mousemove", moveCatcher);
  fetch("/data.json")
    .then(res => res.json())
    .then(data => { flights = data; });
};
</script>
)rawliteral";
  server.send(200, "text/html", page);
}

void handleDataJson() {
  StaticJsonDocument<8192> doc;
  for (auto& t : travels) {
    JsonObject o = doc.createNestedObject();
    o["flightNumber"] = t.flightNumber;
    o["gate"] = t.gate;
    o["destination"] = t.destination;
    o["boarding"] = t.boarding;
    o["departure"] = t.departure;
    o["status"] = t.status;
  }
  String out;
  serializeJson(doc, out);
  server.send(200, "application/json", out);
}

void setupWebServer() {
  server.on("/", handleRoot);
  server.on("/data.json", handleDataJson);
  server.begin();
  Serial.println("Web server started.");
}

void fetchData() {
  WiFi.begin(ssid, password);
  unsigned long startAttempt = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 8000) {
    delay(200);
    Serial.print(".");
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected to WiFi, fetching data...");
    HTTPClient http;
    http.begin(serverUrl);
    int httpCode = http.GET();
    if (httpCode > 0) {
      String payload = http.getString();
      DynamicJsonDocument doc(8192);
      if (!deserializeJson(doc, payload)) {
        travels.clear();
        for (JsonObject obj : doc.as<JsonArray>()) {
          Travel t;
          t.flightNumber = obj["flightNumber"].as<String>();
          t.gate = obj["gate"].as<String>();
          t.destination = obj["destination"].as<String>();
          t.boarding = obj["boarding"].as<String>();
          t.departure = obj["departure"].as<String>();
          t.status = obj["status"].as<String>();
          travels.push_back(t);

          Serial.println("========== FLIGHT ==========");
          Serial.println("Flight Number: " + t.flightNumber);
          Serial.println("Gate        : " + t.gate);
          Serial.println("Destination : " + t.destination);
          Serial.println("Boarding    : " + t.boarding);
          Serial.println("Departure   : " + t.departure);
          Serial.println("Status      : " + t.status);
          Serial.println("============================");
        }
        saveData();
      } else {
        Serial.println("Failed to parse JSON");
      }
    } else {
      Serial.println("HTTP GET failed");
    }
    http.end();
    WiFi.disconnect(true);
  } else {
    Serial.println("WiFi connection failed");
  }
}

void setup() {
  Serial.begin(115200);

  prefs.begin("mode", true);
  bool fetchMode = prefs.getBool("fetch_mode", true);
  prefs.end();

  if (fetchMode) {
    Serial.println("Starting in FETCH mode...");
    fetchData();
    prefs.begin("mode", false);
    prefs.putBool("fetch_mode", false);
    prefs.end();
    delay(1000);
    ESP.restart();
  } else {
    Serial.println("Starting in LOCAL WEB mode...");
    WiFi.softAP(fallback_ssid, fallback_pass);
    loadData();
    setupWebServer();
    lastMillis = millis();
  }
}

void loop() {
  server.handleClient();

  if (millis() - lastMillis > interval) {
    prefs.begin("mode", false);
    prefs.putBool("fetch_mode", true);
    prefs.end();
    Serial.println("1 minute passed, switching to FETCH mode...");
    delay(1000);
    ESP.restart();
  }
}
