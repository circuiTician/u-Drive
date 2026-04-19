#include "uD.h"

/* ---------- Pin Definitions ---------- */

// Motor driver
#define LIN1 13
#define LIN2 12
#define RIN1 27
#define RIN2 14

// IR / Ultrasonic shared trig pins
#define PIN_LEFT 32
#define PIN_MIDDLE 33
#define PIN_RIGHT 25
#define ECHO_PIN 34

// Buzzer & LED
#define BUZZER 26
#define PIXEL_PIN 2

// Battery
#define BATTERY_PIN 35

//SD card
#define SD_CS 5


/* ---------- PWM Setup ---------- */
#define PWM_FREQ 20000
#define PWM_RES 8

#define CH_L1 0
#define CH_R1 1
#define CH_BUZZ 2

Adafruit_NeoPixel pixel(1, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

uDClass uD;

/* ---------- BEGIN ---------- */

void uDClass::begin()
{
    pinMode(LIN2, OUTPUT);
    pinMode(RIN2, OUTPUT);

    ledcSetup(CH_L1, PWM_FREQ, PWM_RES);
    ledcSetup(CH_R1, PWM_FREQ, PWM_RES);
    ledcSetup(CH_BUZZ, 2000, PWM_RES);

    ledcAttachPin(LIN1, CH_L1);
    ledcAttachPin(RIN1, CH_R1);
    ledcAttachPin(BUZZER, CH_BUZZ);

    pixel.begin();
    pixel.clear();
    pixel.show();

}

/* ---------- Data logging ---------- */

bool uDClass::dataLoggerBegin()
{
    if(!SD.begin(SD_CS))
    {
        Serial.println("SD Card Mount Failed");
        return false;
    }

    Serial.println("SD Card Initialized");
    return true;
}

void uDClass::createCSV(const char* filename)
{
    File file = SD.open(filename, FILE_WRITE);

    if(file)
    {
        Serial.println("CSV File Created");
        file.close();
    }
    else
    {
        Serial.println("Failed to Create File");
    }
}

bool uDClass::createCSVWithHeader(const char* filename,
                                  const char* col1,
                                  const char* col2,
                                  const char* col3,
                                  const char* col4,
                                  const char* col5,
                                  const char* col6)
{
    if(SD.exists(filename))
    {
      Serial.println("File Exists");
      return true;
    }
    else
    {
       File file = SD.open(filename, FILE_WRITE);
       if(file)
       {
           file.print(col1);   file.print(",");
           file.print(col2);   file.print(",");
           file.print(col3);   file.print(",");
           file.print(col4);   file.print(",");
           file.print(col5);   file.print(",");
           file.println(col6);

           file.close();

           Serial.println("File Created With Custom Header");
           return true;
       }
    }

    Serial.println("Failed Creating File & Header");
    return false;
}


void uDClass::appendCSV(const char* filename,
                        float timeStamp,
                        float sensorDataL,
                        float sensorDataM,
                        float sensorDataR,
                        int motorL,
                        int motorR)
{
    File file = SD.open(filename, FILE_APPEND);

    if(file)
    {
        file.print(timeStamp);   file.print(",");
        file.print(sensorDataL); file.print(",");
        file.print(sensorDataM); file.print(",");
        file.print(sensorDataR); file.print(",");
        file.print(motorL);      file.print(",");
        file.println(motorR);

        file.close();
    }
    else
    {
        Serial.println("Error Writing Data");
    }
}




/* ---------------------------------- */

/* ---------- Motor Control ---------- */



void uDClass::driveLR(int leftSpeed, int rightSpeed)
{
    leftSpeed  = constrain(leftSpeed,  -255, 255);
    rightSpeed = constrain(rightSpeed, -255, 255);

    setLeftMotor(leftSpeed);
    setRightMotor(rightSpeed);
}

void uDClass::setLeftMotor(int speed)
{
    speed = constrain(speed, -255, 255);
    ledcWrite(CH_L1, abs(speed));
    if(speed >= 0)
    {
        digitalWrite(LIN2, 0);
    }
    else
    {
        digitalWrite(LIN2, 1);
    }
}

void uDClass::setRightMotor(int speed)
{
    speed = constrain(speed, -255, 255);
    ledcWrite(CH_R1, abs(speed));
    if(speed >= 0)
    {
        digitalWrite(RIN2, 0);
    }
    else
    {
        digitalWrite(RIN2, 1);
    }
}

void uDClass::drive(int direction, int speed)
{
    switch(direction)
    {
        case FWD:
            setLeftMotor(speed);
            setRightMotor(speed);
            break;

        case BWD:
            setLeftMotor(-speed);
            setRightMotor(-speed);
            break;

        case LFT:
            setLeftMotor(-speed);
            setRightMotor(speed);
            break;

        case RIT:
            setLeftMotor(speed);
            setRightMotor(-speed);
            break;

        case STP:
            stop();
            break;
    }
}

void uDClass::stop()
{
    setLeftMotor(0);
    setRightMotor(0);
}

/* ---------- LED ---------- */

void uDClass::colourRGB(int colour, int brightness)
{
    brightness = constrain(brightness,0,255);

    uint8_t r=0,g=0,b=0;

    switch(colour)
    {
        case RED: r=255; break;
        case GREEN: g=255; break;
        case BLUE: b=255; break;
        case YELLOW: r=255; g=255; break;
        case PINK: r=255; b=255; break;
        case CYAN: g=255; b=255; break;
        case WHITE: r=g=b=255; break;
        case BLACK: default: break;
    }

    r = (r * brightness) / 255;
    g = (g * brightness) / 255;
    b = (b * brightness) / 255;

    pixel.setPixelColor(0, pixel.Color(r,g,b));
    pixel.show();
}

/* ---------- Buzzer ---------- */

void uDClass::buz(int count, int delayMs)
{
    for(int i=0;i<count;i++)
    {
        ledcWriteTone(CH_BUZZ, 2000);
        delay(delayMs);

        ledcWriteTone(CH_BUZZ, 0);
        delay(delayMs);
    }
}

/* ---------- IR Sensors ---------- */

void uDClass::enableIRMode()
{
    pinMode(PIN_LEFT, INPUT);
    pinMode(PIN_MIDDLE, INPUT);
    pinMode(PIN_RIGHT, INPUT);
}

int uDClass::getIR(int position)
{
    //enableIRMode();

    int pin;

    if(position == LEFT) pin = PIN_LEFT;
    else if(position == MIDDLE) pin = PIN_MIDDLE;
    else pin = PIN_RIGHT;

    return !digitalRead(pin); // active LOW
}

/* ---------- Ultrasonic ---------- */

void uDClass::enableUltrasonicMode()
{
    Serial.println("OK");
    pinMode(PIN_LEFT, OUTPUT);
    pinMode(PIN_MIDDLE, OUTPUT);
    pinMode(PIN_RIGHT, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
}

float uDClass::getDistance(int position)
{
    //enableUltrasonicMode();

    int trig;

    if(position == LEFT) trig = PIN_LEFT;
    else if(position == MIDDLE) trig = PIN_MIDDLE;
    else trig = PIN_RIGHT;

    digitalWrite(trig, LOW);
    delayMicroseconds(2);
    digitalWrite(trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig, LOW);

    long duration = pulseIn(ECHO_PIN, HIGH, 30000);
    //Serial.println("pulsein: " + String(duration));

    float distance = duration * 0.034 / 2.0;
    return distance;
}

/* ---------- Battery ---------- */

float uDClass::getBatteryVoltage()
{
    int raw = analogRead(BATTERY_PIN);
    float voltage = (raw * 3.3 / 4095.0) * 3.127;
    return voltage;
}
