#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "blackout";
const char* password = "12345678";
const char* serverUrl = "http://10.192.136.63:5000/data";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  HTTPClient http;
  http.begin(serverUrl);
  
  int httpCode = http.GET();
  if (httpCode > 0) {
    String payload = http.getString();
    Serial.println("Received data: " + payload);
  } else {
    Serial.println("Error on HTTP request");
  }
  http.end();
  delay(10000); // Fetch data every 10 seconds
}
