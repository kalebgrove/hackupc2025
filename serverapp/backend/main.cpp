#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h> // Or SD.h for SD card
#include <SQLite3_ESP32.h> // Library for SQLite
#include <ArduinoJson.h>  // For JSON parsing
#include <HTTPClient.h> // For making HTTP requests

// WiFi Credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

bool isBlackout = false;

// SQLite Database Path (Internal Flash)
const char* dbPath = "/spiffs/flight_data.db";
// For SD Card:
// const char* dbPath = "/sd/flight_data.db";

// Web Server
AsyncWebServer server(80);

// Database connection
sqlite3* db;

// Forward declarations
void initDatabase();
void createTables();
void insertInitialData();
void setupRoutes();
String getWeatherData();
String getFlightGateData();
void updateFlightStatus(int flightNumber, const String& newStatus);
void printDatabaseError(sqlite3* db);

// Function to initialize the SQLite database
void initDatabase() {
  Serial.println("Initializing SQLite database...");
  int rc = sqlite3_open(dbPath, &db);
  if (rc != SQLITE_OK) {
    Serial.printf("Can't open database: %s\n", sqlite3_errmsg(db));
    // Consider adding a retry mechanism or a way to signal a fatal error.
    while(1); // Stop execution.  Cannot continue without DB.
  } else {
    Serial.println("Opened database successfully");
  }
}

// Function to create the necessary tables
void createTables() {
    char* zErrMsg = 0;
    const char* createFlightTableSQL =
        "CREATE TABLE IF NOT EXISTS flights ("
        "    flight_number INTEGER PRIMARY KEY,"
        "    flight_gate    TEXT NOT NULL,"
        "    boarding_time  TEXT NOT NULL,"
        "    departure_time TEXT NOT NULL,"
        "    status         TEXT NOT NULL"
        ");";

    const char* createWeatherTableSQL =
        "CREATE TABLE IF NOT EXISTS weather ("
        "    id INTEGER PRIMARY KEY,"
        "    conditions TEXT NOT NULL,"
        "    temperature REAL NOT NULL,"
        "    humidity    REAL NOT NULL"
        ");";

    const char* createCatastropheTableSQL =
        "CREATE TABLE IF NOT EXISTS catastrophes ("
        "    id INTEGER PRIMARY KEY,"
        "    type TEXT NOT NULL,"
        "    location TEXT NOT NULL,"
        "    description TEXT NOT NULL"
        ");";

     const char* createMapTableSQL =
        "CREATE TABLE IF NOT EXISTS map ("
        "    id INTEGER PRIMARY KEY,"
        "    image_data BLOB NOT NULL" // Store the image as BLOB (binary data)
        ");";

    // Execute the table creation statements
    int rc;
    rc = sqlite3_exec(db, createFlightTableSQL, NULL, NULL, &zErrMsg);
    if (rc != SQLITE_OK) {
        Serial.printf("SQL error creating flight table: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        Serial.println("Flight table created successfully");
    }

    rc = sqlite3_exec(db, createWeatherTableSQL, NULL, NULL, &zErrMsg);
    if (rc != SQLITE_OK) {
        Serial.printf("SQL error creating weather table: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        Serial.println("Weather table created successfully");
    }

    rc = sqlite3_exec(db, createCatastropheTableSQL, NULL, NULL, &zErrMsg);
    if (rc != SQLITE_OK) {
        Serial.printf("SQL error creating catastrophe table: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        Serial.println("Catastrophe table created successfully");
    }

    rc = sqlite3_exec(db, createMapTableSQL, NULL, NULL, &zErrMsg);
    if (rc != SQLITE_OK) {
        Serial.printf("SQL error creating map table: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        Serial.println("Map table created successfully");
    }
}

// Function to insert initial data into the tables
void insertInitialData() {
  char* zErrMsg = 0;
  // Check if tables are empty before inserting.
  sqlite3_stmt* stmt;
  int rc;

  // Check if flights table is empty
    rc = sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM flights", -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        Serial.println("Error checking flight table: " + String(sqlite3_errmsg(db)));
        return;
    }
    sqlite3_step(stmt);
    int flightCount = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);

  if (flightCount == 0) {
        const char* insertFlightSQL =
            "INSERT INTO flights (flight_number, flight_gate, boarding_time, departure_time, status) VALUES "
            "(123, 'A1', '10:00', '10:30', 'On Time'),"
            "(456, 'B5', '11:30', '12:00', 'Delayed'),"
            "(789, 'C2', '13:00', '13:30', 'Cancelled');";
        rc = sqlite3_exec(db, insertFlightSQL, NULL, NULL, &zErrMsg);
        if (rc != SQLITE_OK) {
            Serial.printf("SQL error inserting flights: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        } else {
            Serial.println("Flights inserted successfully");
        }
  }

  // Check if weather table is empty
    rc = sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM weather", -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        Serial.println("Error checking weather table: " + String(sqlite3_errmsg(db)));
        return;
    }
    sqlite3_step(stmt);
    int weatherCount = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);

  if (weatherCount == 0) {
        const char* insertWeatherSQL =
            "INSERT INTO weather (conditions, temperature, humidity) VALUES "
            "('Sunny', 25.5, 60.2);";

        rc = sqlite3_exec(db, insertWeatherSQL, NULL, NULL, &zErrMsg);
        if (rc != SQLITE_OK) {
            Serial.printf("SQL error inserting weather: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        } else {
            Serial.println("Weather inserted successfully");
        }
  }

    // Check if catastrophes table is empty
    rc = sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM catastrophes", -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        Serial.println("Error checking catastrophes table: " + String(sqlite3_errmsg(db)));
        return;
    }
    sqlite3_step(stmt);
    int catastropheCount = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);

  if (catastropheCount == 0) {
        const char* insertCatastropheSQL =
            "INSERT INTO catastrophes (type, location, description) VALUES "
            "('Earthquake', 'Near Airport', 'Minor tremors reported');";
        rc = sqlite3_exec(db, insertCatastropheSQL, NULL, NULL, &zErrMsg);
        if (rc != SQLITE_OK) {
            Serial.printf("SQL error inserting catastrophes: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        } else {
            Serial.println("Catastrophes inserted successfully");
        }
  }

  // Insert Map Data.  This is CRUCIAL.  The map *must* be in the database.
  //  For a real application, you'd load this from a file.  For this example,
  //  we'll use a *very* small, placeholder image.  You MUST replace this.
    rc = sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM map", -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        Serial.println("Error checking map table: " + String(sqlite3_errmsg(db)));
        return;
    }
    sqlite3_step(stmt);
    int mapCount = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);

  if (mapCount == 0) {
        //  REPLACE THIS WITH REAL IMAGE DATA.  This is a placeholder.
        const unsigned char mapData[] = {
          0x42, 0x4D, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x28, 0x00,
          0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x18, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00
        };
        const int mapDataSize = sizeof(mapData);

        sqlite3_stmt* insertMapStmt;
        const char* insertMapSQL = "INSERT INTO map (image_data) VALUES (?)";

        rc = sqlite3_prepare_v2(db, insertMapSQL, -1, &insertMapStmt, NULL);
        if (rc != SQLITE_OK) {
            Serial.printf("SQL error preparing map insert: %s\n", sqlite3_errmsg(db));
            return;
        }

        // Bind the image data as a BLOB.  THIS IS THE KEY STEP.
        sqlite3_bind_blob(insertMapStmt, 1, mapData, mapDataSize, SQLITE_STATIC);

        rc = sqlite3_step(insertMapStmt);
        if (rc != SQLITE_DONE) {
             Serial.printf("SQL error inserting map data: %s\n", sqlite3_errmsg(db));
        }
        else {
            Serial.println("Map data inserted successfully");
        }
        sqlite3_finalize(insertMapStmt);
  }
}

// Function to retrieve flight data from the database
String getFlightData(string flightnum) {
  String json = "[";
  sqlite3_stmt* stmt;
  const char* query = "SELECT flight_number, flight_gate, boarding_time, departure_time, status FROM flights WHERE flight_number = ?";
  int rc = sqlite3_prepare_v2(db, query, [flightnum], -1, &stmt, NULL);
  if (rc != SQLITE_OK) {
    Serial.println("Error preparing flight query: " + String(sqlite3_errmsg(db)));
    return "[]"; // Return empty JSON array on error
  }

  while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
    json += "{";
    json += "\"flight_number\":" + String(sqlite3_column_int(stmt, 0)) + ",";
    json += "\"flight_gate\":\"" + String((const char*)sqlite3_column_text(stmt, 1)) + "\",";
    json += "\"boarding_time\":\"" + String((const char*)sqlite3_column_text(stmt, 2)) + "\",";
    json += "\"departure_time\":\"" + String((const char*)sqlite3_column_text(stmt, 3)) + "\",";
    json += "\"status\":\"" + String((const char*)sqlite3_column_text(stmt, 4)) + "\"";
    json += "},";
  }
  sqlite3_finalize(stmt);
  if (json.length() > 1) {
    json.remove(json.length() - 1); // Remove trailing comma, if any
  }
  json += "]";
  return json;
}

// Function to retrieve weather data
String getWeatherData() {
  String json = "[";
  sqlite3_stmt* stmt;
  const char* query = "SELECT conditions, temperature, humidity FROM weather";
  int rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
  if (rc != SQLITE_OK) {
    Serial.println("Error preparing weather query: " + String(sqlite3_errmsg(db)));
    return "[]";
  }

  while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
    json += "{";
    json += "\"conditions\":\"" + String((const char*)sqlite3_column_text(stmt, 0)) + "\",";
    json += "\"temperature\":" + String(sqlite3_column_double(stmt, 1)) + ",";
    json += "\"humidity\":" + String(sqlite3_column_double(stmt, 2));
    json += "},";
  }
  sqlite3_finalize(stmt);
  if (json.length() > 1) {
    json.remove(json.length() - 1);
  }
  json += "]";
  return json;
}

// Function to retrieve catastrophe data
String getCatastropheData() {
  String json = "[";
  sqlite3_stmt* stmt;
  const char* query = "SELECT type, location, description FROM catastrophes";
  int rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
  if (rc != SQLITE_OK) {
    Serial.println("Error preparing catastrophe query: " + String(sqlite3_errmsg(db)));
    return "[]";
  }

  while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
    json += "{";
    json += "\"type\":\"" + String((const char*)sqlite3_column_text(stmt, 0)) + "\",";
    json += "\"location\":\"" + String((const char*)sqlite3_column_text(stmt, 1)) + "\",";
    json += "\"description\":\"" + String((const char*)sqlite3_column_text(stmt, 2)) + "\"";
    json += "},";
  }
  sqlite3_finalize(stmt);
  if (json.length() > 1) {
    json.remove(json.length() - 1);
  }
  json += "]";
  return json;
}

// Function to retrieve map data from the database
void getMapData(AsyncResponseStream *response) {
    sqlite3_stmt* stmt;
    const char* query = "SELECT image_data FROM map WHERE id = 1"; // Assuming map is stored with ID 1
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        Serial.println("Error preparing map query: " + String(sqlite3_errmsg(db)));
        response->setCode(500); // Internal Server Error
        response->print("Error retrieving map data");
        return;
    }

    if ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        const void* blob = sqlite3_column_blob(stmt, 0);
        int blobSize = sqlite3_column_bytes(stmt, 0);

        // Set the content type for the image.  Crucial.
        response->setContentType("image/bmp"); // Or image/jpeg, image/png, etc.
        response->send(blob, blobSize); // Send the raw image data
    } else {
        response->setCode(404); // Not Found
        response->print("Map data not found");
    }
    sqlite3_finalize(stmt);
}

// Function to update flight status
void updateFlightStatus(int flightNumber, const String& newStatus) {
  char* zErrMsg = 0;
  sqlite3_stmt* stmt;
  const char* updateSQL = "UPDATE flights SET status = ? WHERE flight_number = ?";
  int rc = sqlite3_prepare_v2(db, updateSQL, -1, &stmt, NULL);
  if (rc != SQLITE_OK) {
    Serial.printf("Error preparing update statement: %s\n", sqlite3_errmsg(db));
    return;
  }

  // Bind the new status and flight number to the prepared statement.
  sqlite3_bind_text(stmt, 1, newStatus.c_str(), newStatus.length(), SQLITE_STATIC);
  sqlite3_bind_int(stmt, 2, flightNumber);

  rc = sqlite3_step(stmt);
  if (rc != SQLITE_DONE) {
    Serial.printf("Error updating flight status: %s\n", sqlite3_errmsg(db));
    sqlite3_free(zErrMsg); //important
  } else {
    Serial.println("Flight status updated successfully");
  }
  sqlite3_finalize(stmt);
}

// Function to fetch weather data from an external API
String fetchWeatherDataFromAPI() {
    HTTPClient http;
    // Replace with your actual weather API endpoint.  This is a placeholder.
    String weatherAPIUrl = "http://api.weatherapi.com/v1/current.json?key=YOUR_WEATHER_API_KEY&q=Girona";
    http.begin(weatherAPIUrl);
    int httpCode = http.GET();
    if (httpCode > 0) {
        if (httpCode == HTTP_STATUS_OK) {
            String payload = http.getString();
            http.end();
            return payload; // Return the JSON string
        } else {
            Serial.printf("Error fetching weather data: %d\n", httpCode);
            http.end();
            return "{\"error\":\"Failed to fetch weather data\"}"; // Return a JSON error object
        }
    } else {
        Serial.printf("HTTP error: %s\n", http.errorToString(httpCode).c_str());
        http.end();
        return "{\"error\":\"HTTP error during weather data fetch\"}";
    }
}

// Function to fetch flight gate updates from an external API
String fetchFlightGateUpdatesFromAPI() {
  HTTPClient http;
  // Replace with your actual flight gate API endpoint. This is a placeholder.
  String flightGateAPIUrl = "https://opensky-network.org/api/flights/arrival?airport=LEGE&begin=1704062400&end=1704066000"; // Example
  http.begin(flightGateAPIUrl);
  int httpCode = http.GET();
  if (httpCode > 0) {
    if (httpCode == HTTP_STATUS_OK) {
      String payload = http.getString();
      http.end();
      return payload;
    } else {
      Serial.printf("Error fetching flight gate data: %d\n", httpCode);
      http.end();
      return "{\"error\":\"Failed to fetch flight gate data\"}";
    }
  } else {
    Serial.printf("HTTP error: %s\n", http.errorToString(httpCode).c_str());
    http.end();
    return "{\"error\":\"HTTP error during flight gate data fetch\"}";
  }
}

// Function to update weather data in the database
void updateWeatherDataInDB(const String& jsonData) {
    // Use ArduinoJson to parse the JSON data
    DynamicJsonDocument doc(1024); // Adjust size as needed.  Crucial.
    DeserializationError error = deserializeJson(doc, jsonData);
    if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return; // IMPORTANT:  Return on error.  Do NOT proceed with bad data.
    }

    // Extract the data.  Check for the existence of the data.
    if (doc.containsKey("error")) {
        Serial.println("Error in weather JSON data.  Not updating DB.");
        return;
    }
    const String conditions = doc["current"]["condition"]["text"];
    float temperature = doc["current"]["temp_c"];
    float humidity = doc["current"]["humidity"];

    char* zErrMsg = 0;
    sqlite3_stmt* stmt;
    const char* updateSQL = "UPDATE weather SET conditions = ?, temperature = ?, humidity = ? WHERE id = 1"; // Assuming ID is 1
    int rc = sqlite3_prepare_v2(db, updateSQL, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        Serial.printf("Error preparing weather update statement: %s\n", sqlite3_errmsg(db));
        return;
    }

    // Bind the values.
    sqlite3_bind_text(stmt, 1, conditions.c_str(), conditions.length(), SQLITE_STATIC);
    sqlite3_bind_double(stmt, 2, temperature);
    sqlite3_bind_double(stmt, 3, humidity);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        Serial.printf("Error updating weather data in DB: %s\n", sqlite3_errmsg(db));
        sqlite3_free(zErrMsg);
    } else {
        Serial.println("Weather data updated in DB successfully.");
    }
    sqlite3_finalize(stmt);
}

// Function to update flight gate data in the database
void updateFlightGateDataInDB(const String& jsonData) {
    // Use ArduinoJson to parse the JSON data
    DynamicJsonDocument doc(4096);  // Adjust size as needed
    DeserializationError error = deserializeJson(doc, jsonData);

    if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return;
    }

    // Check for error
     if (doc.containsKey("error")) {
        Serial.println("Error in flight gate JSON data.  Not updating DB.");
        return;
    }

    // The JSON structure from the OpenSky Network is an array of flight objects.
    for (JsonVariant v : doc.as<JsonArray>()) {
        int flightNumber = -1; // Initialize to an invalid value
        String flightGate = "Unknown";
        String departureTime = "Unknown";

        // Extract the flight number.  Check for null.
        if (!v["flight"].isNull()) {
             String flight = v["flight"].as<String>(); // Example: "DLH1234"
             // Extract the numeric part.
             String numberPart = flight.substring(3); // Get "1234"
             flightNumber = numberPart.toInt();
        }

        // Extract the gate.
        if (!v["gate"].isNull()) {
            flightGate = v["gate"].as<String>();
        }
        // Extract departure time
        if (!v["time_departure"].isNull()){
            departureTime = v["time_departure"].as<String>();
        }

        // Only update if we have a valid flight number.
        if (flightNumber != -1) {
            char* zErrMsg = 0;
            sqlite3_stmt* stmt;
            const char* updateSQL = "UPDATE flights SET flight_gate = ?, departure_time = ? WHERE flight_number = ?";
            int rc = sqlite3_prepare_v2(db, updateSQL, -1, &stmt, NULL);
            if (rc != SQLITE_OK) {
                Serial.printf("Error preparing flight gate update: %s\n", sqlite3_errmsg(db));
                return;
            }

            sqlite3_bind_text(stmt, 1, flightGate.c_str(), flightGate.length(), SQLITE_STATIC);
            sqlite3_bind_text(stmt, 2, departureTime.c_str(), departureTime.length(), SQLITE_STATIC);
            sqlite3_bind_int(stmt, 3, flightNumber);

            rc = sqlite3_step(stmt);
            if (rc != SQLITE_DONE) {
                Serial.printf("Error updating flight gate in DB: %s\n", sqlite3_errmsg(db));
                sqlite3_free(zErrMsg);
            } else {
                Serial.printf("Flight gate for flight %d updated to %s\n", flightNumber, flightGate.c_str());
            }
            sqlite3_finalize(stmt);
        }
    }
}
// Function to print SQLite errors
void printDatabaseError(sqlite3* db) {
    if (db) {
        Serial.printf("SQLite error: %s\n", sqlite3_errmsg(db));
    }
}

// Function to set up the web server routes
void setupRoutes() {
  // Serve the main HTML page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", "text/html");
  });

  // Serve the CSS file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/style.css", "text/css");
  });

   // Serve the JavaScript file
  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/script.js", "text/javascript");
  });

  // Serve the map image from the database
    server.on("/map", HTTP_GET, [](AsyncWebServerRequest *request) {
        AsyncResponseStream *response = request->beginResponseStream("image/bmp"); // Or the correct type
        getMapData(response); // Call function to send image data
    });

  // RESTful API endpoints
  server.on("/flights", HTTP_GET, [](AsyncWebServerRequest *request) {
    String json = getFlightData();
    request->send(200, "application/json", json);
  });

  server.on("/weather", HTTP_GET, [](AsyncWebServerRequest *request) {
    String json = getWeatherData();
    request->send(200, "application/json", json);
  });

   server.on("/catastrophes", HTTP_GET, [](AsyncWebServerRequest *request) {
    String json = getCatastropheData();
    request->send(200, "application/json", json);
  });

  server.on("/updateFlightStatus", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("flightNumber") && request->hasParam("status")) {
      int flightNumber = request->getParam("flightNumber")->value().toInt();
      String newStatus = request->getParam("status")->value();
      updateFlightStatus(flightNumber, newStatus);
      request->send(200, "text/plain", "Flight status updated");
    } else {
      request->send(400, "text/plain", "Missing flightNumber or status parameter");
    }
  });
}

void setup() {
  Serial.begin(115200);

  // Initialize SPIFFS (or SD card)
  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount SPIFFS");
    // Mount the SD card if SPIFFS fails
    // if (!SD.begin()) {
    //    Serial.println("Failed to mount SD card");
    //    while (1);  // Stop.  Cannot continue without storage.
    // }
  }

  // Initialize WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: " + WiFi.localIP().toString());

  // Initialize SQLite database
  initDatabase();
  // Create tables (if they don't exist)
  createTables();
  // Insert initial data (if tables are empty)
  insertInitialData();

  // Set up routes
  setupRoutes();

  // Start the server
  server.begin();

  //  Set up periodic tasks using a timer.
  //  Update weather and flight data periodically.
  //  These should be non-blocking.
  static unsigned long previousMillis = 0;
  const long interval = 300000; // 5 minutes (300000 milliseconds)

  // Lambda function to be called by the timer.
  auto updateData = []() {
        String weatherData = fetchWeatherDataFromAPI();
        updateWeatherDataInDB(weatherData);

        String flightGateData = fetchFlightGateUpdatesFromAPI();
        updateFlightGateDataInDB(flightGateData);
  };
  // Create a timer to call the updateData function every interval.
  timer1_attachInterrupt(updateData);
  timer1_enable(TIM_DIV16, TIMER_AUTORELOAD_EN);  // 80MHz / 16 = 5MHz timer
  timer1_setCount(5000000); // 5MHz / 1Hz = 5,000,000
}

void loop() {
  // put your main code here, to run repeatedly:
  //  The server and the timer interrupts run in the background.
  delay(10000);
}
