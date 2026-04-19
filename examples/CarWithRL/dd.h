#include <WiFi.h>
#include <WebSocketsServer.h>
#include <uD.h>
#include <ArduinoJson.h>
#include <WebServer.h>
#include <SD.h>
#include <SPI.h>

// ===== Hotspot Credentials =====
const char* ssid = "ESP32_Hotspot";
const char* password = "12345678";  // Minimum 8 characters
bool CNTD = false;
unsigned long startTime = 0;
unsigned long lastStoreTime = 0;
const char* f_Name = "/TestFile.csv";

// ===== Instances =====
WebSocketsServer webSocket = WebSocketsServer(81);
WebServer server(80);  // HTTP Server for file downloads

// ===== Motor Variables =====
int leftSpeed;
int rightSpeed;
const int DEADZONE = 30;

//Forward declaration
void joystickDrive(int x, int y);

// ===== Client Event Handler =====
void webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {

  switch (type) {

    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.print("Client Connected: ");
        CNTD = true;
        Serial.println(ip);
        break;
      }

    case WStype_DISCONNECTED:
      Serial.println("Client Disconnected");
      CNTD = false;
      startTime = millis();
      break;


    case WStype_TEXT:
      {
        // Serial.print("Received From Client: ");
        // Serial.println((char*)payload);

        // ----- Parse JSON -----
        // Increased size to handle file lists
        StaticJsonDocument<1024> doc;

        DeserializationError error = deserializeJson(doc, payload);

        if (!error) {

          const char* msgType = doc["type"];

          // --- 1. Joystick Control ---
          if (strcmp(msgType, "joystick") == 0) {
            // Extract values
            int xValue = doc["x"];
            int yValue = doc["y"];

            // ----- Apply Deadzone -----
            if (abs(xValue) < DEADZONE) xValue = 0;
            if (abs(yValue) < DEADZONE) yValue = 0;

            // Serial.printf("X: %d, Y: %d\n", xValue, yValue);
            joystickDrive(xValue, yValue);
          }

          // --- 2. Text Message ---
          else if (strcmp(msgType, "message") == 0) {
            Serial.print("Msg: ");
            String text = doc["text"];
            Serial.println(text);
          }

          // --- 3. List Files (New Feature) ---
          else if (strcmp(msgType, "list_files") == 0) {
            File root = SD.open("/");
            if (!root) {
              Serial.println("Failed to open SD root");
            } else {
              StaticJsonDocument<1024> responseDoc;
              responseDoc["type"] = "file_list";
              JsonArray files = responseDoc.createNestedArray("files");

              File file = root.openNextFile();
              while (file) {
                if (!file.isDirectory()) {
                  files.add(String(file.name()));
                }
                file = root.openNextFile();
              }

              String jsonString;
              serializeJson(responseDoc, jsonString);
              webSocket.sendTXT(num, jsonString);
            }
          }

        } else {
          Serial.print("JSON Parsing Failed: ");
          Serial.println(error.c_str());
        }

        break;
      }
  }
}

void setup() {

  Serial.begin(115200);

  // ===== Start Hotspot =====
  WiFi.softAP(ssid, password);

  Serial.println("Hotspot Started");
  Serial.print("AP IP Address: ");
  Serial.println(WiFi.softAPIP());

  // ===== Start WebSocket =====
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  Serial.println("WebSocket Server Started");

  // ===== Initialize SD & Data Logger =====
  // Assuming uD initializes the SD card internally.
  // If uD does NOT initialize SD.begin(), you may need to uncomment the next line:
  // if(!SD.begin()) { Serial.println("SD Begin Failed"); }

  uD.begin();
  if (uD.dataLoggerBegin()) {
    Serial.println("Data logger initialized");
    delay(100);
    uD.createCSVWithHeader(f_Name, "timeStamp", "LEFT", "MIDDLE", "RIGHT", "LEFT_M", "RIGHT_M");
  } else Serial.println("Unable to initialize Data logger");

  // ===== Start HTTP Server (For Downloads) =====
  server.on("/download", HTTP_GET, []() {
    if (server.hasArg("name")) {
      String fileName = server.arg("name");
      if (!fileName.startsWith("/")) fileName = "/" + fileName;

      if (SD.exists(fileName)) {
        File file = SD.open(fileName, "r");

        // ===== Tell browser actual filename =====
        server.sendHeader("Content-Disposition", "attachment; filename=\"" + fileName.substring(1) + "\"");

        // Optional: set correct MIME for CSV
        server.streamFile(file, "text/csv");

        file.close();
      } else {
        server.send(404, "text/plain", "File Not Found");
      }
    } else {
      server.send(400, "text/plain", "Bad Request");
    }
  });

  // ===== Delete File API =====
  server.on("/delete", HTTP_GET, []() {
    if (server.hasArg("name")) {

      String fileName = server.arg("name");
      if (!fileName.startsWith("/")) fileName = "/" + fileName;

      if (SD.exists(fileName)) {

        if (SD.remove(fileName)) {
          server.send(200, "text/plain", "File Deleted Successfully");
          Serial.println("Deleted: " + fileName);
        } else {
          server.send(500, "text/plain", "Delete Failed");
        }

      } else {
        server.send(404, "text/plain", "File Not Found");
      }

    } else {
      server.send(400, "text/plain", "Bad Request");
    }
  });


  server.begin();
  Serial.println("HTTP Server Started");
}

void loop() {

  webSocket.loop();
  server.handleClient();  // Handle HTTP download requests

  // ===== Send Serial Data To Client =====
  if (Serial.available()) {
    String msg = Serial.readStringUntil('\n');
    webSocket.broadcastTXT(msg);  // Send to all connected clients
  }

  if (millis() - lastStoreTime > 1115 && CNTD) {
    uD.appendCSV(f_Name, millis() - startTime, uD.getDistance(LEFT), uD.getDistance(MIDDLE), uD.getDistance(RIGHT), leftSpeed, rightSpeed);
    lastStoreTime = millis();
  }
}


/* ----------------------------------------------------
   Joystick Drive Function
---------------------------------------------------- */

int mapMotorSpeed(int val) {
  val = constrain(val, -100, 100);

  if (val > 0) {
    return map(val, 0, 100, 160, 255);
  } else if (val < 0) {
    return map(val, -100, 0, -255, -160);
  }

  return 0;
}

void joystickDrive(int x, int y) {
  x = constrain(x, -100, 100);
  y = constrain(y, -100, 100);

  // Dead zone to avoid jitter
  if (abs(x) < 5) x = 0;
  if (abs(y) < 5) y = 0;

  int left = y + x;
  int right = y - x;

  left = constrain(left, -100, 100);
  right = constrain(right, -100, 100);

  leftSpeed = mapMotorSpeed(left);
  rightSpeed = mapMotorSpeed(right);


  uD.driveLR(leftSpeed, rightSpeed);

  // Debug print
  // Serial.print("Left Speed: ");
  // Serial.print(leftSpeed);
  // Serial.print("  Right Speed: ");
  // Serial.println(rightSpeed);
}