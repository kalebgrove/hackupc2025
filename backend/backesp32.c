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
    DynamicJsonDocument doc(1024);
    
    // Add each flight to the JSON document
    for (const auto& t : travels) {
        JsonObject flight = doc.createNestedObject();
        flight["flightNumber"] = t.flightNumber;
        flight["gate"] = t.gate;
        flight["destination"] = t.destination;
        flight["boarding"] = t.boarding;
        flight["departure"] = t.departure;
        flight["status"] = t.status;
    }

    // Convert the JSON document to a string
    String out;
    serializeJson(doc, out);

    // Send the flight data as JSON
    server.send(200, "application/json", out);  // Serve the flight data as JSON
  }
}

void handleFlights() {
    // Create a JSON document
    StaticJsonDocument<1024> doc;
    
    // Loop through the travels vector and add each flight to the JSON document
    for (const auto& t : travels) {
        JsonObject flight = doc.createNestedObject();
        flight["flightNumber"] = t.flightNumber;
        flight["gate"] = t.gate;
        flight["destination"] = t.destination;
        flight["boarding"] = t.boarding;
        flight["departure"] = t.departure;
        flight["status"] = t.status;
    }

    // Convert the JSON document to a string
    String out;
    serializeJson(doc, out);

    // Send the JSON data as the response
    server.send(200, "application/json", out);  // Serve as JSON
}

void handleRoot() {
    // Create a JSON document to store the flight data
    StaticJsonDocument<1024> doc;

    // Loop through the travels vector and add each flight to the JSON document
    for (const auto& t : travels) {
        JsonObject flight = doc.createNestedObject();
        flight["flightNumber"] = t.flightNumber;
        flight["gate"] = t.gate;
        flight["destination"] = t.destination;
        flight["boarding"] = t.boarding;
        flight["departure"] = t.departure;
        flight["status"] = t.status;
    }

    // Convert the JSON document to a string
    String out;
    serializeJson(doc, out);

    // Send the JSON data as the response
    server.send(200, "application/json", out);  // Serve as JSON
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

void handleUpdateFlight() {
    String flightNumber = server.arg("flightNumber");
    String gate = server.arg("gate");
    String destination = server.arg("destination");
    String boarding = server.arg("boarding");
    String departure = server.arg("departure");
    String status = server.arg("status");

    for (auto& t : travels) {
        if (t.flightNumber == flightNumber) {
            t.gate = gate;
            t.destination = destination;
            t.boarding = boarding;
            t.departure = departure;
            t.status = status;
            saveDataFlightFlight();  // Save the updated data
            server.send(200, "application/json", "{\"status\":\"success\"}");
            return;
        }
    }

    // If flight not found
    server.send(404, "application/json", "{\"status\":\"error\",\"message\":\"Flight not found\"}");
}

void setupWebServer() {
  server.on("/", HTTP_GET, handleRoot);
  server.on("/flights", HTTP_GET, handleFlights);
  server.on("/add-flights", HTTP_POST, handleAddFlight);
  server.on("/upd-flights", HTTP_PUT, handleUpdateFlight);
  server.on("/weather", HTTP_GET, handleWeather);
  server.on("/admin", handleAdmin);
  server.on("/data.json", handleDataJson);
  server.begin();
  server.sendHeader("Access-Control-Allow-Origin", "*");
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

void saveDataWeather() {
    Preferences prefs;
    prefs.begin("weather", false);
    prefs.putUInt("count", wthr.size());  // Store the number of weather entries
    for (int i = 0; i < wthr.size(); i++) {
        prefs.putString(("temp" + String(i)).c_str(), wthr[i].temperature);
        prefs.putString(("hum" + String(i)).c_str(), wthr[i].humidity);
        prefs.putString(("cond" + String(i)).c_str(), wthr[i].condition);
    }
    prefs.end();
}

void loadDataWeather() {
    Preferences prefs;
    prefs.begin("weather", true);
    int count = prefs.getUInt("count", 0);
    wthr.clear();
    for (int i = 0; i < count; i++) {
        Weather w;
        w.temperature = prefs.getString(("temp" + String(i)).c_str(), "");
        w.humidity = prefs.getString(("hum" + String(i)).c_str(), "");
        w.condition = prefs.getString(("cond" + String(i)).c_str(), "");
        wthr.push_back(w);
    }
    prefs.end();
}

void handleWeather() {
    if (onlineMode) {
        // If there's internet, fetch the latest weather data from the API
        HTTPClient http;
        http.begin(weatherApiUrl);
        int httpCode = http.GET();

        if (httpCode > 0) {
            String payload = http.getString();
            Serial.println("Weather data fetched: " + payload);

            // Optionally, you can deserialize and process the JSON data here
            DynamicJsonDocument doc(1024);
            deserializeJson(doc, payload);

            // Update weather data vector with fetched data
            wthr.clear();  // Clear old data
            for (JsonObject obj : doc.as<JsonArray>()) {
                Weather weather;
                weather.temperature = obj["temperature"].as<String>();
                weather.humidity = obj["humidity"].as<String>();
                weather.condition = obj["condition"].as<String>();
                wthr.push_back(weather);
            }
            saveDataWeather();  // Save fetched data locally
        } else {
            Serial.println("Failed to fetch weather data.");
        }

        http.end();
    }

    // Serve the weather data (either fetched or stored locally)
    String weatherJson = "[";
    for (const auto& w : wthr) {
        JsonObject weather = doc.createNestedObject();
        weather["temperature"] = w.temperature;
        weather["humidity"] = w.humidity;
        weather["condition"] = w.condition;
    }
    String out;
    serializeJson(doc, out);

    // Send the JSON data as the response
    server.send(200, "application/json", out);  // Serve as JSON
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