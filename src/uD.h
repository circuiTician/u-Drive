#ifndef UD_H
#define UD_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <SD.h>
#include <SPI.h>


/* ---------- Direction Macros ---------- */
#define FWD  1
#define BWD  2
#define LFT  3
#define RIT  4
#define STP  5

/* ---------- Position Macros ---------- */
#define LEFT   0
#define MIDDLE 1
#define RIGHT  2

/* ---------- Colour Macros ---------- */
#define RED     1
#define GREEN   2
#define BLUE    3
#define YELLOW  4
#define PINK    5
#define CYAN    6
#define WHITE   7
#define BLACK   8

class uDClass
{
public:
    void begin();

    /* Motor Control */
    void drive(int direction, int speed);
    void driveLR(int leftSpeed, int rightSpeed);
    void stop();

    // ===== DATA LOGGER =====
    bool dataLoggerBegin();
    bool createCSVWithHeader(const char* filename,
                         const char* col1,
                         const char* col2,
                         const char* col3,
                         const char* col4,
                         const char* col5,
                         const char* col6);

    void createCSV(const char* filename);
    void appendCSV(const char* filename,
               float timeStamp,
               float sensorDataL,
               float sensorDataM,
               float sensorDataR,
               int motorL,
               int motorR);


    /* LED */
    void colourRGB(int colour, int brightness);

    /* Buzzer */
    void buz(int count, int delayMs);

    /* Sensors */
    int getIR(int position);
    float getDistance(int position);

    /* Power */
    float getBatteryVoltage();

    /* Mode helpers */
    void enableIRMode();
    void enableUltrasonicMode();


private:

    /* Motor helpers */
    void setLeftMotor(int speed);
    void setRightMotor(int speed);

};

extern uDClass uD;

#endif
#ifndef UD_H
#define UD_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

/* ---------- Direction Macros ---------- */
#define FWD  1
#define BWD  2
#define LFT  3
#define RIT  4
#define STP  5

/* ---------- Position Macros ---------- */
#define LEFT   0
#define MIDDLE 1
#define RIGHT  2

/* ---------- Colour Macros ---------- */
#define RED     1
#define GREEN   2
#define BLUE    3
#define YELLOW  4
#define PINK    5
#define CYAN    6
#define WHITE   7
#define BLACK   8

class uDClass
{
public:
    void begin();

    /* Motor Control */
    void drive(int direction, int speed);
    void driveLR(int leftSpeed, int rightSpeed);
    void stop();

    /* LED */
    void colourRGB(int colour, int brightness);

    /* Buzzer */
    void buz(int count, int delayMs);

    /* Sensors */
    int getIR(int position);
    float getDistance(int position);

    /* Power */
    float getBatteryVoltage();

    /* Mode helpers */
    void enableIRMode();
    void enableUltrasonicMode();

private:

    /* Motor helpers */
    void setLeftMotor(int speed);
    void setRightMotor(int speed);
};

extern uDClass uD;

#endif
