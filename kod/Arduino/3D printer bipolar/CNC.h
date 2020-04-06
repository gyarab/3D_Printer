
#ifndef CNC_h
#define CNC_h

#include <Arduino.h>
#include <AccelStepper.h>
#include <math.h>



class Pozice{
    public: long x = 0;
    public: long y = 0;
    public: long z = 0;
    public: long e = 0;
    public: int backlashX = 1;
    public: int backlashY = 1;

    
};

class CNC{
  public:
    CNC();
    int bagr();
    double search_string(char key, char instruction[], byte string_size);
    
    void moveG0();
    void moveG1();
    void calibrate();
    
    void parseAndExecute();

    
    
  private:
    int indukcni_spinac = A1;
    
    
    //AccelStepper stepper1;
    //AccelStepper stepper2;
    //AccelStepper stepper3;

    
    int _i;
    float mSpeed = 600.0;
    float mZspeed = 600.0;
    
    bool centered = false;

    
    Pozice currentXYZ;
    Pozice nextXYZ;

    int enableZmotorsPin = 13;
    
    float XYindex = 1; //vynasobi X a Y (defaultne 1)
    
    float kalibraceOsyZ = 24.906;//kalibrace osy z (kalibraceOsyZ*stepsPerMilimeter = 1mm)
    float kalibraceExtruderu = 5.7;
    float kalibraceOsyX = 1.042;
    float kalibraceOsyY = 1.042;

   
    float stepsPerMilimeter = 83.5/2;
    float stepsPerInch = int(25.4*stepsPerMilimeter);
    float currentUnit = stepsPerMilimeter;

    boolean relativeExtrusion = true;
    
    boolean setNextXYZ(char instruction[], byte size);
    float getRotatedX(double X, double Y, float sinUhlu, float cosUhlu);
    float getRotatedY(double X, double Y, float sinUhlu, float cosUhlu);
    void moveG2andG3(double centerX, double centerY, int index = 1);
    
    void process_string(char instruction[], byte size);
    byte BytesAvailible();
    
};

#endif
