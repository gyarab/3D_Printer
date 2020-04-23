 #include "Arduino.h"
#include "CNC.h"
#include <AccelStepper.h>
#include <math.h>

#define motorInterfaceType 1

#define motor1Pin1  30
#define motor1Pin2  32

#define motor1Pin3  34
#define motor1Pin4  36

#define motor1Pin5  38
#define motor1Pin6  40

#define motor1Pin7  42
#define motor1Pin8  44

AccelStepper stepper1(motorInterfaceType, motor1Pin2, motor1Pin1);//vertikal (X)
AccelStepper stepper2(motorInterfaceType, motor1Pin4, motor1Pin3);//Deska (Y)
AccelStepper stepper3(motorInterfaceType, motor1Pin6, motor1Pin5);// Ram (Z)
AccelStepper stepper4(motorInterfaceType, motor1Pin8, motor1Pin7);//extruder (E)


CNC::CNC(){
    Serial.begin(115200);
    stepper1.setMaxSpeed(mSpeed);
    stepper2.setMaxSpeed(mSpeed);
    stepper3.setMaxSpeed(mZspeed);
    stepper4.setMaxSpeed(mSpeed);
    
    stepper1.setAcceleration(200000);
    stepper2.setAcceleration(200000);
    stepper3.setAcceleration(200000);
    stepper4.setAcceleration(200000);
    
    stepper1.setSpeed(mSpeed);
    stepper2.setSpeed(mSpeed);
    stepper3.setSpeed(mZspeed);
    stepper4.setSpeed(mSpeed);
    
    stepper1.move(1);
    stepper2.move(1);
    stepper3.move(1);
    stepper4.move(1);
    
    stepper1.run();
    stepper2.run();
    stepper3.run();
    stepper4.run();
  
    int indukcni_spinac = A1;

}

//vrátí číselnou hodnotu z pole charů (instruction), která následuje po (key) 
double CNC::search_string(char key, char instruction[], byte string_size){
  char temp[10] = "";
  
  for (int i=0; i<string_size; i++)
    {
      if (instruction[i] == key)
      {
        i++;      
        int k = 0;
        while (instruction[i] != ' ' && instruction[i] != NULL)
        {
          temp[k] = instruction[i];
          i++; k++;
        }
        return strtod(temp, NULL);
      }
    }  
  return -9999;
}
//nastaví následující souřadnice a uloží předchozí kvůli příkazu G2, G3. přepočítá je na kroky.
boolean CNC::setNextXYZ(char instruction[], byte size){
    currentXYZ.x = nextXYZ.x;
    currentXYZ.y = nextXYZ.y;
    currentXYZ.z = nextXYZ.z;
    currentXYZ.e = nextXYZ.e;
    
    boolean vraceni = false;
    
    float distanceX = (float)(search_string('X', instruction, size));
    float distanceY = (float)(search_string('Y', instruction, size));
    float distanceZ = (float)(search_string('Z', instruction, size));
    float distanceE = (float)(search_string('E', instruction, size));

    if (distanceX > -9000){
      nextXYZ.x = (long)(currentUnit*distanceX*XYindex*kalibraceOsyX);
      vraceni = true;
    }
    if (distanceY > -9000){
      nextXYZ.y = (long)(currentUnit*distanceY*XYindex*kalibraceOsyY);
      vraceni = true;
    }
    if (distanceZ > -9000){
      nextXYZ.z = (long)(-currentUnit*distanceZ*kalibraceOsyZ);
      vraceni = true;
    }
    if (distanceE > -9000){
      nextXYZ.e = (long)(distanceE*currentUnit*kalibraceExtruderu);
      vraceni = true;
    }else{
      nextXYZ.e = (long)(0.0);
    }
    
    
    return vraceni;
}
//go to XYZ. asynchroně (osa x a osa y nemusí dorazit ve stejnou chvíli), bez extruze.
void CNC::moveG0(){ 

  
  stepper1.moveTo(-nextXYZ.x);
  stepper2.moveTo(-nextXYZ.y);
  stepper3.moveTo(-nextXYZ.z);

  stepper1.setSpeed(mSpeed);
  stepper2.setSpeed(mSpeed);
  stepper3.setSpeed(mZspeed);

  stepper1.enableOutputs();
  stepper2.enableOutputs();
  stepper3.enableOutputs();

  digitalWrite(enableZmotorsPin, HIGH);//enable Z motors
  while(abs(stepper3.distanceToGo()) > 0){
    stepper3.runSpeedToPosition();
  }

  stepper3.disableOutputs();
  digitalWrite(enableZmotorsPin, LOW);//disable Z motors
  
  while ( abs(stepper1.distanceToGo()) + abs(stepper2.distanceToGo()) > 0){
    stepper1.run();
    stepper2.run();
  }

  stepper1.disableOutputs();
  stepper2.disableOutputs();
  
  
}


//Pohyb po osách X,Y,Z,E. nejprve se posune Z, poté X,Y,E tak, že dorazí ve stejnou chvíli.
void CNC::moveG1(){
  

  stepper1.enableOutputs();
  stepper2.enableOutputs();
  stepper3.enableOutputs();


  stepper1.moveTo(-nextXYZ.x);
  stepper2.moveTo(-nextXYZ.y);
  stepper3.moveTo(-nextXYZ.z);

  
  
  if (relativeExtrusion){
    stepper4.move(nextXYZ.e);
  }else{
    stepper4.moveTo(nextXYZ.e);
  }
  float dis1 = abs(stepper1.distanceToGo());
  float dis2 = abs(stepper2.distanceToGo());
  float dis4 = abs(stepper4.distanceToGo());

  
  float secondSpeed;
  float extruderSpeed;

  digitalWrite(enableZmotorsPin, HIGH);//enable Z motors

  stepper3.enableOutputs();
    
  stepper3.setMaxSpeed(mZspeed);
  while(abs(stepper3.distanceToGo()) > 0){
    stepper3.run();
  }
  digitalWrite(enableZmotorsPin, LOW);//disable Z motors
  
  stepper3.disableOutputs();
  if (int(dis1 + dis2) == 0){
      extruderSpeed = mSpeed;

      stepper4.setMaxSpeed(extruderSpeed);
  }else{
    if (dis1 >= dis2){

     stepper1.setMaxSpeed(mSpeed);
     secondSpeed = dis2*mSpeed/dis1;
     
     stepper2.setMaxSpeed(secondSpeed);

     extruderSpeed = dis4*(mSpeed)/(dis1);

     stepper4.setMaxSpeed(extruderSpeed);
     
  }else{

     stepper2.setMaxSpeed(mSpeed);
     secondSpeed = dis1*mSpeed/dis2;

     stepper1.setMaxSpeed(secondSpeed);
     extruderSpeed = dis4*(mSpeed)/(dis2);

     stepper4.setMaxSpeed(extruderSpeed);

  }
}
  while ( abs(stepper1.distanceToGo()) + abs(stepper2.distanceToGo()) + abs(stepper4.distanceToGo()) > 0){
    stepper1.run();
    stepper2.run();
    stepper4.run();
  }
  
}
//maticový výpočet pro rotaci bodů
float CNC::getRotatedX(double X, double Y, float sinUhlu, float cosUhlu){
  return (X*cosUhlu - Y*sinUhlu);
}
//maticový výpočet pro rotaci bodů
float CNC::getRotatedY(double X, double Y, float sinUhlu, float cosUhlu){
  return (X*sinUhlu + Y*cosUhlu);
}
//příkaz G2, G3. vypočítá dráhu oblouku a posune se po ní.
void CNC::moveG2andG3(double centerX, double centerY, int index){ //index = 1: G2, index = -1: G3
  int segments = 16;
  float alpha = float(3.14/segments);

  double extrude = nextXYZ.e;
  
  float lastX = nextXYZ.x;
  float lastY = nextXYZ.y;

  float beta = (atan2(centerY, centerX)); // relative IJ
  
  float sinBeta = sin(-beta);
  float cosBeta = cos(-beta);

  float rotatedLastX = getRotatedX(lastX - currentXYZ.x, lastY - currentXYZ.y, sinBeta, cosBeta);

  float r = getRotatedX(currentXYZ.x + centerX - currentXYZ.x, currentXYZ.y + centerY - currentXYZ.y, sinBeta, cosBeta);
  int i = 1;
  float x;
  float y;

  sinBeta = sin(beta);
  cosBeta = cos(beta);
  
  int maxX = rotatedLastX-r*(1 - cos(alpha));
  
  while (true){
    x = r*(1 - cos(alpha*i));
    if (x >= maxX){
      break;
    }

    y = index * sqrt(x * (2*r - x));
    nextXYZ.x = (int)getRotatedX(x, y, sinBeta, cosBeta) + currentXYZ.x;
    nextXYZ.y = (int)getRotatedY(x, y, sinBeta, cosBeta) + currentXYZ.y;
    nextXYZ.e = double(extrude/segments);
    moveG1();
    
    i += 1;
  }
  
  nextXYZ.x = lastX;
  nextXYZ.y = lastY;
  moveG1();
}

//zkalibruje osy S,Y,Z. (posune je do základni polohy)
void CNC::calibrate(){
    stepper1.setCurrentPosition(0);
    stepper2.setCurrentPosition(0);
    nextXYZ.x = -12000/2;
    nextXYZ.y = -12000/2;
    nextXYZ.z = -6000/2;
    moveG1();
    stepper1.setCurrentPosition(0);
    stepper2.setCurrentPosition(0);
    nextXYZ.x = 500/2;
    nextXYZ.y = 5600/2;
    moveG1();
    stepper1.setCurrentPosition(0);
    stepper2.setCurrentPosition(0);
    nextXYZ.x = 0;
    nextXYZ.y = 0;
    nextXYZ.z = 0;

    
    int presnost = 10;
    byte tresholdVal = 80;
    byte val = analogRead(indukcni_spinac);
    if (val <10){
      while(val < tresholdVal){
          stepper3.setCurrentPosition(0);
          nextXYZ.z = 100;
          moveG1();
          val = analogRead(indukcni_spinac);
      }
      while(val > tresholdVal){
          stepper3.setCurrentPosition(0);
          nextXYZ.z = -presnost;
          moveG1();
          val = analogRead(indukcni_spinac);
      }
    }else{
       while(val > tresholdVal){
          stepper3.setCurrentPosition(0);
          nextXYZ.z = -100;
          moveG1();
          val = analogRead(indukcni_spinac);
          Serial.println(val);
      }
      while(val < tresholdVal){
          stepper3.setCurrentPosition(0);
          nextXYZ.z = presnost;
          moveG1();
          val = analogRead(indukcni_spinac);
      }
    }
    nextXYZ.z = 3400;
    moveG1();
    nextXYZ.x = 1000/2;
    nextXYZ.y = -5000/2;
    moveG1();
    
    currentXYZ.x = 0;
    currentXYZ.y = 0;
    currentXYZ.z = 0;
    nextXYZ.x = 0;
    nextXYZ.y = 0;
    nextXYZ.z = 0;
    stepper1.setCurrentPosition(0);
    stepper2.setCurrentPosition(0);
    stepper3.setCurrentPosition(0);
    stepper4.setCurrentPosition(0);
  
}

//přečte instrukci a zavolá adekvátní funkci.
void CNC::process_string(char instruction[], byte size){
    int command;
    
  if (instruction[0] == 'G'){
    command = (int)(search_string('G', instruction, size));
    if (command == 0){
      setNextXYZ(instruction, size);
      
      moveG0();
      //go to XYZ jak rychle to pujde
    }else if(command == 1){
      if (setNextXYZ(instruction, size)){
      

      moveG1();
      }
      
    }else if(command == 2){//move in clockvise arc
      setNextXYZ(instruction, size);
      //int distanceE = (int)(search_string('E', instruction, size));
      double centerI = currentUnit*(double)(search_string('I', instruction, size));
      double centerJ = currentUnit*(double)(search_string('J', instruction, size));

      moveG2andG3(centerI, centerJ, 1);
      
      
      
    }else if(command == 3){//move in counterClockvise arc
      setNextXYZ(instruction, size);
      //int distanceE = (int)(search_string('E', instruction, size));
      double centerI = currentUnit*(double)(search_string('I', instruction, size));
      double centerJ = currentUnit*(double)(search_string('J', instruction, size));

      moveG2andG3(centerI, centerJ, -1);
      
      
      
    }else if(command == 4){
      delay((int)(search_string('P', instruction, size)));
      
    }else if(command == 20){
      currentUnit = stepsPerInch;
      
    }else if(command == 21){
      currentUnit = stepsPerMilimeter;
      
    }else if(command == 42){
      calibrate();
      
    }else if(command == 69){//release motors
      stepper1.disableOutputs();
      stepper2.disableOutputs();
      stepper4.disableOutputs();
      
    }else if(command == 82){
      relativeExtrusion = true;
    }else if(command == 82){
      relativeExtrusion = false;
    }else if(command == 92){
      nextXYZ.x = 0;
      nextXYZ.y = 0;
      nextXYZ.z = 0;
      nextXYZ.e = 0;

      currentXYZ.x = 0;
      currentXYZ.y = 0;
      currentXYZ.z = 0;
      currentXYZ.e = 0;
      
      stepper1.setCurrentPosition(0);
      stepper2.setCurrentPosition(0);
      stepper3.setCurrentPosition(0);
      stepper4.setCurrentPosition(0);
    }

  }else if (instruction[0] == 'M'){
    command = (int)(search_string('M', instruction, size));
    
    if (command == 300){
      if ((int)(search_string('S', instruction, size)) == 30){
        nextXYZ.z = 0;
      }else{
        nextXYZ.z = 0.8*(-currentUnit*kalibraceOsyZ);
      }
      moveG0();
      
  }
  
  }

  Serial.println("b");
}
//čeká, dokud se do bufferu neuloží byte. ten přečte a vrátí jeho hodnotu. (tento byte představuje počet následujících bytů v instrukci)
byte CNC::BytesAvailible(){
  while (!Serial.available()){
  }
  byte len = Serial.read();
  Serial.println(len);
  return (len);
}

//přečte a vykoná instrukci přijatou přes USB.
void CNC::parseAndExecute(){
  
  byte availibleBytes = BytesAvailible();

  char line[availibleBytes];
    for(byte x = 0; x< availibleBytes; x++){
      while (!Serial.available()){
        }
      line[x] = char(Serial.read());
    }
    
    process_string(line, availibleBytes);
    
}
