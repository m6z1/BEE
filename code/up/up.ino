// 라이브러리 호출
#include <Servo.h>   
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// 핀
#define tonePin A2
#define led_Right 2
#define rev_Right 3
#define servo_Right 4
#define servo_Left 5
#define ledPin 6
#define led_Left 8
#define rev_Left 9
#define moter_Right 12
#define moter_Left 13

// 서보모터
Servo servoLeft;
Servo servoRight;
Servo LWiper;
Servo RWiper;

int left_value = 130;
int right_value = 20;

void setup(){   
 // LED
  pinMode(ledPin, OUTPUT);
  
  //부저
  pinMode(tonePin, OUTPUT);

 
  
  // 서보모터
  pinMode(rev_Right, INPUT); pinMode(led_Right, OUTPUT);
  pinMode(rev_Left, INPUT); pinMode(led_Left, OUTPUT);

  // 서보모터
  LWiper.attach(servo_Left);
  RWiper.attach(servo_Right);

  // 주행
  servoLeft.attach(moter_Left);
  servoRight.attach(moter_Right);
}
 
void loop()  
{ 
  //LED
  digitalWrite(ledPin, HIGH); 
  
  //서보모터
  wipermove(left_value, right_value);

  //주행
  int irLeft = irDetect(led_Left, rev_Left, 38000);
  int irRight = irDetect(led_Right, rev_Right, 38000);

 //주행
  if((irLeft == 0) && (irRight == 0)){
    backward(1000);
    turnLeft(400);
    tone(tonePin, 220);
    delay(20);
    noTone(tonePin);
  }
  else if(irLeft == 0){ 
    backward(1000);
    turnRight(150);
    tone(tonePin, 220);
    delay(10);
  }
  else if(irRight == 0){
    backward(1000);
    turnLeft(150);
    tone(tonePin, 220);
    delay(10);
  }
  else{
    forward(20);
    noTone(tonePin);
  }
}

int irDetect(int irLedPin, int irReceiverPin, long frequency)
{ tone(irLedPin, frequency, 8);
  delay(1);
  int ir = digitalRead(irReceiverPin);
  delay(1);
  return ir;}
  
void forward(int time)
{ servoLeft.writeMicroseconds(1700);
  servoRight.writeMicroseconds(1380);
  delay(time);}

void turnLeft(int time)
{ servoLeft.writeMicroseconds(1300);
  servoRight.writeMicroseconds(1300);
  delay(time);}

void turnRight(int time)
{ servoLeft.writeMicroseconds(1700);
  servoRight.writeMicroseconds(1700);
  delay(time);}

void backward(int time)
{ servoLeft.writeMicroseconds(1300);
  servoRight.writeMicroseconds(1700);
  delay(time);}

void wipermove(int left_value, int right_value)
{
  for (int i=0; i<80; i++){
    LWiper.write(left_value--);
    RWiper.write(right_value++);
  }
  delay(300);
  
  for (int j=0; j<80; j++) {
    LWiper.write(left_value++);
    RWiper.write(right_value--);
  }
  delay(300);
}
