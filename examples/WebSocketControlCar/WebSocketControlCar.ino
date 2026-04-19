#include <WiFi.h>
#include <WebSocketsServer.h>
#include <uD.h>
#include <ArduinoJson.h>

// ===== Hotspot Credentials =====
const char* ssid = "ESP32_Hotspot";
const char* password = "12345678";   // Minimum 8 characters

// ===== WebSocket Server =====
WebSocketsServer webSocket = WebSocketsServer(81);

const int DEADZONE = 30;

// ===== Client Event Handler =====
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

  switch(type) {

    case WStype_CONNECTED:
    {
      IPAddress ip = webSocket.remoteIP(num);
      Serial.print("Client Connected: ");
      Serial.println(ip);
      break;
    }

    case WStype_DISCONNECTED:
      Serial.println("Client Disconnected");
      break;

    
    case WStype_TEXT: {
      Serial.print("Received From Client: ");
      Serial.println((char*)payload);

      // ----- Parse JSON -----
      StaticJsonDocument<200> doc;

      DeserializationError error = deserializeJson(doc, payload);

      if (!error) {

        // Extract values
        int xValue = doc["x"];
        int yValue = doc["y"];

        // ----- Apply Deadzone -----
        if (abs(xValue) < DEADZONE) xValue = 0;
        if (abs(yValue) < DEADZONE) yValue = 0;

        Serial.print("X = ");
        Serial.println(xValue);

        Serial.print("Y = ");
        Serial.println(yValue);

        joystickDrive(xValue, yValue);
      }
      else {
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
  
  uD.begin();
}

void loop() {

  webSocket.loop();

  // ===== Send Serial Data To Client =====
  if (Serial.available()) {
    String msg = Serial.readStringUntil('\n');
    webSocket.broadcastTXT(msg);   // Send to all connected clients
  }
}


/* ----------------------------------------------------
   Joystick Drive Function
---------------------------------------------------- */

int mapMotorSpeed(int val)
{
  val = constrain(val, -100, 100);

  if(val > 0)
  {
    return map(val, 0, 100, 160, 255);
  }
  else if(val < 0)
  {
    return map(val, -100, 0, -255, -160);
  }

  return 0;
}

void joystickDrive(int x, int y)
{
  x = constrain(x, -100, 100);
  x = x*(-1);
  y = constrain(y, -100, 100);

  // Dead zone to avoid jitter
  if(abs(x) < DEADZONE) x = 0;
  if(abs(y) < DEADZONE) y = 0;

  int left  = y + x;
  int right = y - x;

  left  = constrain(left,  -100, 100);
  right = constrain(right, -100, 100);


  int leftSpeed  = mapMotorSpeed(left);
  int rightSpeed = mapMotorSpeed(right);


  uD.driveLR(leftSpeed, rightSpeed);

  // Debug print
  Serial.print("Left Speed: ");
  Serial.print(leftSpeed);
  Serial.print("  Right Speed: ");
  Serial.println(rightSpeed);
}
