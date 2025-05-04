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

const char* ssid = "blackout";
const char* password = "12345678";
const char* fallback_ssid = "ESP32_AP";
const char* fallback_pass = "esp12345";
const char* serverUrl = "http://10.192.136.63:5000/flights";

bool onlineMode = false;
unsigned long lastFetch = 0;
const unsigned long fetchInterval = 5000;

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

void handleAdmin() {
  if (server.method() == HTTP_POST) {
    Travel t;
    t.flightNumber = server.arg("flightNumber");
    t.gate = server.arg("gate");
    t.destination = server.arg("destination");
    t.boarding = server.arg("boarding");
    t.departure = server.arg("departure");
    t.status = server.arg("status");
    travels.push_back(t);
    saveData();
    server.sendHeader("Location", "/admin");
    server.send(303);
  } else {
    String page = "<h2>Add New Flight</h2><form method='POST' style='display: flex; flex-direction: column;'>";
    page += "Flight Number: <input name='flightNumber' style='margin: 5px; padding: 10px; font-size: 16px;' required><br>";
    page += "Gate: <input name='gate' style='margin: 5px; padding: 10px; font-size: 16px;' required><br>";
    page += "Destination: <input name='destination' style='margin: 5px; padding: 10px; font-size: 16px;' required><br>";
    page += "Boarding: <input name='boarding' style='margin: 5px; padding: 10px; font-size: 16px;' required><br>";
    page += "Departure: <input name='departure' style='margin: 5px; padding: 10px; font-size: 16px;' required><br>";
    page += "Status: <input name='status' style='margin: 5px; padding: 10px; font-size: 16px;' required><br>";
    page += "<input type='submit' value='Add' style='padding: 10px 20px; font-size: 18px; background-color: #4CAF50; color: white; border: none; cursor: pointer;'>";
    page += "</form><br><a href='/' style='text-decoration: none; color: #4CAF50;'>Back to Home</a>";
    server.send(200, "text/html", page);
  }
}

void handleNews() {
  if (server.method() == HTTP_POST) {
    String news = server.arg("news");
    prefs.begin("news", false);
    prefs.putString("latest", news);
    prefs.end();
    server.sendHeader("Location", "/news");
    server.send(303);
  } else {
    prefs.begin("news", true);
    String news = prefs.getString("latest", "There is no news");
    prefs.end();
    String page = "<h2>Edit News</h2><form method='POST' style='display: flex; flex-direction: column;'>";
    page += "<textarea name='news' rows='4' cols='40' style='margin: 5px; padding: 10px; font-size: 16px;'>" + news + "</textarea><br>";
    page += "<input type='submit' value='Update' style='padding: 10px 20px; font-size: 18px; background-color: #4CAF50; color: white; border: none; cursor: pointer;'>";
    page += "</form><br><a href='/' style='text-decoration: none; color: #4CAF50;'>Back to Home</a>";
    server.send(200, "text/html", page);
  }
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
<h3 style="font-size:24px">News:</h3><p>)rawliteral" + news + R"rawliteral(</p>
<script>
let ctx, canvas, interval;
let plane = { x: 150, y: 0, w: 40, h: 30, speed: 2, dest: "", gate: "" };
let catcher = { x: 160, y: 310, w: 80, h: 10 };
let score = 0;
let flights = [];

function drawPlane(p) {
  ctx.save();
  ctx.translate(p.x + p.w / 2, p.y + p.h / 2);
  ctx.rotate(90 * Math.PI / 180); // Rotate plane to point downwards
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
  let x = e.clientX - rect.left;
  catcher.x = x - catcher.w / 2;
}

function gameLoop() {
  ctx.clearRect(0, 0, canvas.width, canvas.height);
  plane.y += plane.speed;
  if (plane.y > canvas.height) resetPlane();

  if (
    plane.y + plane.h >= catcher.y &&
    plane.x + plane.w >= catcher.x &&
    plane.x <= catcher.x + catcher.w
  ) {
    score++;
    plane.speed += 0.2;
    resetPlane();
  }

  drawPlane(plane);
  drawCatcher();
  ctx.fillStyle = "green";
  ctx.font = "30px Arial"; // Increased font size for score
  ctx.fillText("Score: " + score, 10, 40); // Increased position for score
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
  server.on("/admin", handleAdmin);
  server.on("/news", handleNews);
  server.on("/data.json", handleDataJson);
  server.begin();
  //Serial.println("Web server started.");
}

void setup() {
  //Serial.begin(115200);
  WiFi.begin(ssid, password);
  unsigned long startAttempt = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 2000) {
    delay(200);
    //Serial.print("...");
  }

  if (WiFi.status() == WL_CONNECTED) {
    //Serial.println("\nConnected to WiFi: " + WiFi.localIP().toString());
    onlineMode = true;
  } else {
    WiFi.softAP(fallback_ssid, fallback_pass);
    //Serial.println("AP IP: " + WiFi.softAPIP().toString());
    loadData();
  }

  setupWebServer();
}

void loop() {
  server.handleClient();

  if (onlineMode && millis() - lastFetch > fetchInterval) {
    lastFetch = millis();
    HTTPClient http;
    http.begin(serverUrl);
    int httpCode = http.GET();

    if (httpCode > 0) {
      String payload = http.getString();
      //Serial.println("Fetched from SQL: " + payload);

      travels.clear();
      DynamicJsonDocument doc(8192);
      DeserializationError error = deserializeJson(doc, payload);

      if (!error) {
        for (JsonArray row : doc.as<JsonArray>()) {
          if (row.size() >= 6) {
            Travel t;
            t.flightNumber = row[0].as<String>();
            t.gate = row[1].as<String>();
            t.destination = row[2].as<String>();
            t.boarding = row[3].as<String>();
            t.departure = row[4].as<String>();
            t.status = row[5].as<String>();
            travels.push_back(t);

            // Serial.println("Flight: " + t.flightNumber + " | Gate: " + t.gate +
            //   " | Dest: " + t.destination + " | Boarding: " + t.boarding +
            //   " | Departure: " + t.departure + " | Status: " + t.status);
          }
        }
        saveData();
      } else {
        Serial.println("JSON parse failed.");
      }
    } else {
      Serial.println("HTTP GET failed.");
      ESP.restart();
    }

    http.end();
  }
}
