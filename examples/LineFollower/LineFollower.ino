#include <uD.h>

#define BASE_SPEED 145
#define TURN_SPEED 155 // Slightly faster for sharp corrections

void setup() {
  uD.begin();
  uD.enableIRMode(); // [cite: 64]
  // Serial is okay for setup, but keep it minimal in the loop
  Serial.begin(115200); 
}

void loop() {
  // 1. QUICK SENSOR READ
  int left = uD.getIR(LEFT);     // [cite: 36, 64]
  int mid = uD.getIR(MIDDLE);    // [cite: 36, 64]
  int right = uD.getIR(RIGHT);   // [cite: 37, 64]
  
  // Create the 3-bit sensor pattern 
  int sensorValue = (left << 2) | (mid << 1) | right;

  // 2. IMMEDIATE ACTUATION (No Delays!)
  switch(sensorValue) {
    case 0b010: // Only Middle sees black (Perfect!)
      uD.drive(FWD, BASE_SPEED); // [cite: 60]
      break;

    case 0b110: // Left and Middle see black (Veering Right)
    case 0b100: // Only Left sees black
      uD.drive(LFT, TURN_SPEED); // [cite: 32]
      break;

    case 0b011: // Right and Middle see black (Veering Left)
    case 0b001: // Only Right sees black
      uD.drive(RIT, TURN_SPEED); // [cite: 33]
      break;

    case 0b000: // Lost the line! 
      // Option: Stop or slowly reverse to find it
      uD.stop(); // [cite: 33, 61]
      break;

    default:
      uD.drive(FWD, BASE_SPEED);
      break;
  }

  // 3. MINIMAL DEBUGGING
  // Only print if absolutely necessary, or use a timer to print once every 500ms
}
