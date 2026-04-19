 
#include <uD.h>

// Logic Constants
const float THRESHOLD = 20.0;
const int BASE_SPD = 180;
const int ESCAPE_SPD = 200;

// Deadlock Detection Variables
unsigned long lastTurnTime = 0;
int turnCounter = 0;
const unsigned long TURN_WINDOW = 1500; // 1.5 seconds
const int MAX_TURNS = 3; 

void setup() {
  uD.begin();
  uD.enableUltrasonicMode();
  Serial.begin(115200);
  uD.colourRGB(WHITE, 100);
}

void triggerEscape() {
  Serial.println("DEADLOCK DETECTED: Executing Escape Maneuver");
  uD.stop();
  uD.colourRGB(PINK, 255);
  
  // 1. Significant Reverse 
  uD.drive(BWD, BASE_SPD);
  delay(881); 
  uD.stop();

  // 2. Scan 180 Degrees
  // We perform a long spin while sampling the best path
  float maxDist = 0;
  int bestDir = RIT;

  uD.drive(RIT, ESCAPE_SPD);
  unsigned long scanStart = millis();
  
  while (millis() - scanStart < 800) { // Spin for 2 seconds
    float currentM = uD.getDistance(MIDDLE);
    if (currentM > maxDist) {
      maxDist = currentM;
    }
    delay(50);
  }
  
  uD.stop();
  turnCounter = 0; // Reset counter after escape
}

void handleTurn(int direction) {
  unsigned long currentTime = millis();
  
  // Check if this turn is happening too soon after the last one
  if (currentTime - lastTurnTime < TURN_WINDOW) {
    turnCounter++;
  } else {
    turnCounter = 1; 
  }
  
  lastTurnTime = currentTime;

  if (turnCounter >= MAX_TURNS) {
    triggerEscape();
  } else {
    uD.drive(direction, ESCAPE_SPD);
    delay(400);
  }
}

void loop() {
  float distM = uD.getDistance(MIDDLE); 
  float distL = uD.getDistance(LEFT);   
  float distR = uD.getDistance(RIGHT);  

  // Filter out invalid 0 or -1 readings [cite: 13, 14]
  if (distM <= 0) distM = 100; 

  if (distM < THRESHOLD) {
    uD.stop();
    delay(100); // Brake for inertia 

    if (distL > distR && distL > THRESHOLD) {
      handleTurn(LFT);
    } else {
      handleTurn(RIT);
    }
  } else {
    // Normal Pathing
    uD.colourRGB(GREEN, 150);
    uD.drive(FWD, BASE_SPD); 
  }
  
  delay(30);
}