#include <U8g2lib.h>

//tento program reguluje teplotu nastavitelnou rotacnim encoderem 
//vyuziva PD vyvazovani
//je psan pro arduino nano

 #define outputA 8
 #define outputB 9
 int counter = 140; 
 int aState;
 int aLastState;  

U8G2_SSD1306_128X32_UNIVISION_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

int ThermistorPin = 0;
int Vo;
float R1 = 200;
float logR2, R2, T;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;
int lastT = 0;

float P_index = 1.0;
float D_index = 7.0;

long heat;

void drawCounter(int cislo, int heat){//zobrazi na displej hodnoty zahivani a cilove teploty
  u8g2.setFont(u8g2_font_profont17_tf);
  u8g2.setCursor(30, 30);
  u8g2.print(cislo);

  u8g2.print((char)176);
  u8g2.print("C ");
  u8g2.print(heat);
}
 
 
 void setup() { 
   pinMode (outputA,INPUT);
   pinMode (outputB,INPUT);
   pinMode(5, OUTPUT);
  
   u8g2.begin();
   
  
   aLastState = digitalRead(outputA);  

  Vo = analogRead(ThermistorPin);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
  T = T - 273.15;
  
  
 } 
 void loop() { 
  heat = int(counter*P_index + (counter-T)*D_index );
  if (heat > 250){
    heat = 250;//nastaveni maximalniho vyhravu
  }
  if (heat < 0){
    heat = 0;
  }
  for (int x = 0; x<10; x++){
   aState = digitalRead(outputA);
   if (aState != aLastState){     
     if (digitalRead(outputB) != aState){ 
      if (counter <= (280-8)){
       counter += 2;
      }
     } else {
      if (counter > 0){
       counter -= 2;
      }
     }
      heat = int(counter*P_index + (counter-T)*D_index );
      if (heat > 255){
        heat = 255;
      }
      if (heat < 0){
        heat = 0;
      }
      u8g2.firstPage();
  do {

    drawCounter(counter, heat);
    drawTempature(int(T));
  } while ( u8g2.nextPage() );
  
   }
   
   aLastState = aState;
  }
  ///////////
  analogWrite(5, heat);

  ///////////
  Vo = analogRead(ThermistorPin);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
  T = T - 273.15;

  if (abs(T - lastT) > 1.8){
    u8g2.firstPage();
  do {

    drawCounter(counter, heat);
    drawTempature(int(T));
  } while ( u8g2.nextPage() );
  lastT = T;
  }
   delay(0);
 }
void drawTempature(int cislo){//zobrazi na displej teplotu
  
  u8g2.setFont(u8g2_font_profont17_tf);
  u8g2.setCursor(30, 15);

  u8g2.print(cislo);
  u8g2.print((char)176);
  u8g2.print("C");
    
}
