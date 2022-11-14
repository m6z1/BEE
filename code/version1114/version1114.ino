#include <Servo.h>

Servo LWiper;
Servo RWiper;

int LeftWiper = 6;
int RightWiper = 5;
int Lvalue = 155;
int Rvalue = 0;

void setup() {
  LWiper.attach(LeftWiper);
  RWiper.attach(RightWiper);
  Serial.begin(9600);
}
void loop() {
  wipermove(Lvalue, Rvalue);
}


void wipermove(int Lvalue, int Rvalue)
{
  for (int i=0; i<155; i++){
    LWiper.write(Lvalue--);
    RWiper.write(Rvalue++);
  }
  delay(300);
  
  for (int j=0; j<155; j++) {
    LWiper.write(Lvalue++);
    RWiper.write(Rvalue--);
  }
  delay(300);
}
