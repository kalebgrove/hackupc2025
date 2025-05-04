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

struct Weather {
    Real temperature;
    Real humidity;
    String condition;
}

std::vector<Weather> wthr;
std::vector<Travel> travels;
WebServer server(80);

const char* ssid = "blackout_";
const char* password = "12345678";
const char* fallback_ssid = "ESP32_AP";
const char* fallback_pass = "esp12345";
const char* serverUrl = "http://10.192.136.63:5000/flights";
const char* serverUrlWeather = "http://10.192.136.63:5000/weather";

bool onlineMode = false;
unsigned long lastFetch = 0;
const unsigned long fetchInterval = 5000;

void saveDataFlightFlight() {
    Preferences prefs;
    prefs.begin("travel", false);
    prefs.putUInt("count", travels.size());  // Guardar la cantidad de vuelos
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

void loadDataFlightFlight() {
    Preferences prefs;
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
    saveDataFlightFlight();
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

void handleFlights() {
    String page = "<h2>Flights List</h2><table border='1'><tr><th>Flight Number</th><th>Gate</th><th>Destination</th><th>Boarding Time</th><th>Departure Time</th><th>Status</th></tr>";

    // Recorrer el vector de vuelos y mostrarlos en una tabla HTML
    for (const auto& t : travels) {
        page += "<tr><td>" + t.flightNumber + "</td><td>" + t.gate + "</td><td>" + t.destination + "</td><td>" + t.boarding + "</td><td>" + t.departure + "</td><td>" + t.status + "</td></tr>";
    }

    page += "</table>";
    server.send(200, "text/html", page);  // Mostrar la página con la lista de vuelos
}


void handleRoot() {
    String page = "<h2>Add New Flight</h2><form method='POST' action='/addFlight'>";
    page += "Flight Number: <input name='flightNumber' required><br>";
    page += "Gate: <input name='gate' required><br>";
    page += "Destination: <input name='destination' required><br>";
    page += "Boarding: <input name='boarding' required><br>";
    page += "Departure: <input name='departure' required><br>";
    page += "Status: <input name='status' required><br>";
    page += "<input type='submit' value='Add Flight'>";
    page += "</form><br><a href='/flights'>View Flights</a>";
    server.send(200, "text/html", page);  // Mostrar la página HTML al usuario
}

void handleAddFlight() {
    String flightNumber = server.arg("flightNumber");
    String gate = server.arg("gate");
    String destination = server.arg("destination");
    String boarding = server.arg("boarding");
    String departure = server.arg("departure");
    String status = server.arg("status");

    // Crear un nuevo vuelo y agregarlo al vector
    Travel newFlight = {flightNumber, gate, destination, boarding, departure, status};
    travels.push_back(newFlight);  // Agregar el vuelo al vector

    saveDataFlight();  // Guardar los datos actualizados en memoria

    // Redirigir al usuario a la página de vuelos
    server.sendHeader("Location", "/flights");  
    server.send(303);  // Código de redirección
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
  server.on("/", HTTP_GET, handleRoot);
  server.on("/flights", HTTP_GET, handleFlights);
  server.on("/add-flights", HTTP_POST, handleAddFlight);
  server.on("/admin", handleAdmin);
  server.on("/news", handleNews);
  server.on("/data.json", handleDataJson);
  server.begin();
  Serial.println("Web server started.");
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  unsigned long startAttempt = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 2000) {
    delay(200);
    Serial.print("...");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected to WiFi: " + WiFi.localIP().toString());
    onlineMode = true;
  } else {
    WiFi.softAP(fallback_ssid, fallback_pass);
    Serial.println("AP IP: " + WiFi.softAPIP().toString());
    loadDataFlight();
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
      Serial.println("Fetched from SQL: " + payload);

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

            Serial.println("Flight: " + t.flightNumber + " | Gate: " + t.gate +
              " | Dest: " + t.destination + " | Boarding: " + t.boarding +
              " | Departure: " + t.departure + " | Status: " + t.status);
          }
        }
        saveDataFlight();
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