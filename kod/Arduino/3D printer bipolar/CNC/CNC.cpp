#include "Arduino.h"
#include "CNC.h"

CNC::CNC(){
    Serial.begin(9600);
    Serial.println("bagr");
}

void CNC::bagr(){
    Serial.println("bagr");
}
