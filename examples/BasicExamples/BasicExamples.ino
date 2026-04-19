/*
  ----------------------------------------------------
  uD Library Functional Test / Demo Sketch
  ----------------------------------------------------

  This sketch demonstrates and tests:

  1. Motor movement in all directions
  2. Pixel LED colour control
  3. Buzzer beep patterns
  4. IR sensor reading
  5. Ultrasonic distance measurement
  6. Battery voltage monitoring
  7. Motor driver fault detection

  ----------------------------------------------------
*/

#include <uD.h>   // Include your robot library

/* ----------------------------------------------------
   SETUP FUNCTION
---------------------------------------------------- */

void setup()
{
  Serial.begin(115200);

  // Initialize uD library hardware
  uD.begin();

  Serial.println("uD Robot Functional Test Started");
}


/* ----------------------------------------------------
   LOOP FUNCTION
---------------------------------------------------- */

void loop()
{

  /* ---------------- Motor Driver Fault Check ---------------- */

  if(uD.motorFault())
  {
    Serial.println("Motor Driver Fault Detected!");

    // Turn LED RED
    uD.colourRGB(RED,255);

    // Continuous warning beep
    uD.buz(3,100);

    // Stop robot for safety
    uD.stop();

    delay(2000);
    return;
  }


  /* ---------------- Battery Voltage Monitoring ---------------- */

  float battery = uD.getBatteryVoltage();

  Serial.print("Battery Voltage: ");
  Serial.println(battery);

  // Battery status LED indication
  if(battery < 6.5)
  {
    uD.colourRGB(RED,200);
  }
  else
  {
    uD.colourRGB(GREEN,200);
  }

  delay(500);



  /* ---------------- Motor Movement Test ---------------- */

  Serial.println("Moving Forward");
  uD.drive(FWD,200);
  delay(2000);

  Serial.println("Moving Backward");
  uD.drive(BWD,200);
  delay(2000);

  Serial.println("Turning Left");
  uD.drive(LFT,200);
  delay(1500);

  Serial.println("Turning Right");
  uD.drive(RIT,200);
  delay(1500);

  Serial.println("Stopping Motors");
  uD.stop();
  delay(1500);



  /* ---------------- Pixel LED Colour Test ---------------- */

  Serial.println("Testing Pixel Colours");

  uD.colourRGB(RED,255);
  delay(500);

  uD.colourRGB(GREEN,255);
  delay(500);

  uD.colourRGB(BLUE,255);
  delay(500);

  uD.colourRGB(YELLOW,255);
  delay(500);

  uD.colourRGB(PINK,255);
  delay(500);

  uD.colourRGB(CYAN,255);
  delay(500);

  uD.colourRGB(WHITE,255);
  delay(500);

  uD.colourRGB(BLACK,255);
  delay(500);



  /* ---------------- Buzzer Test ---------------- */

  Serial.println("Testing Buzzer");

  // 2 beeps with 150ms gap
  uD.buz(2,150);

  delay(1000);



  /* ---------------- IR Sensor Test ---------------- */

  Serial.println("IR Sensor Readings");

  int irLeft   = uD.getIR(LEFT);
  int irMiddle = uD.getIR(MIDDLE);
  int irRight  = uD.getIR(RIGHT);

  Serial.print("IR Left   : ");
  Serial.println(irLeft);

  Serial.print("IR Middle : ");
  Serial.println(irMiddle);

  Serial.print("IR Right  : ");
  Serial.println(irRight);

  delay(1500);



  /* ---------------- Ultrasonic Distance Test ---------------- */

  Serial.println("Ultrasonic Distance");

  float dLeft   = uD.getDistance(LEFT);
  float dMiddle = uD.getDistance(MIDDLE);
  float dRight  = uD.getDistance(RIGHT);

  Serial.print("Distance Left   : ");
  Serial.print(dLeft);
  Serial.println(" cm");

  Serial.print("Distance Middle : ");
  Serial.print(dMiddle);
  Serial.println(" cm");

  Serial.print("Distance Right  : ");
  Serial.print(dRight);
  Serial.println(" cm");

  delay(2000);



  /* ---------------- Obstacle Demonstration ---------------- */

  Serial.println("Obstacle Demo Running");

  float front = uD.getDistance(MIDDLE);

  if(front < 20)   // obstacle detected
  {
    Serial.println("Obstacle Detected");

    uD.stop();
    uD.colourRGB(RED,255);
    uD.buz(1,200);

    delay(500);

    // Turn robot to avoid obstacle
    uD.drive(RIT,200);
    delay(800);
  }
  else
  {
    uD.colourRGB(GREEN,200);
    uD.drive(FWD,180);
  }

  delay(1500);



  /* ---------------- End of Test Cycle ---------------- */

  Serial.println("----- Test Cycle Completed -----");
  Serial.println();

  delay(3000);
}
