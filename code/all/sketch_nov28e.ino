// 라이브러리 호출
#include <Servo.h>   
#include <Wire.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>

// 핀
#define tonePin A2
#define btnPin A3
#define led_Right 2
#define rev_Right 3
#define servo_Right 4
#define servo_Left 5
#define ledPin 6
#define humanPin 7
#define led_Left 8
#define rev_Left 9
#define BT_TXD 10
#define BT_RXD 11
#define moter_Right 12
#define moter_Left 13

//블루투스
SoftwareSerial bluetooth(BT_RXD, BT_TXD);

// 서보모터
Servo servoLeft;
Servo servoRight;
Servo LWiper;
Servo RWiper;

int left_value = 130;
int right_value = 20;

// 인체감지센서
int statusPIR = 0;

// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);
String msg="";
String reset_msg="PUSH BUTTON!";
byte arrow[8] = {
  B00000,
  B00100,
  B01110,
  B11111,
  B00100,
  B00100,
  B00100,
  B00000
};

byte BEE[8] = {
  B00000,
  B11011,
  B01010,
  B00100,
  B01110,
  B11111,
  B01110,
  B00000
};

byte bt1[8] = {
  B00000,
  B11101,
  B00101,
  B00101,
  B00000,
  B10000,
  B11110,
  B00000
};

byte bt2[8] = {
  B01110,
  B00010,
  B00010,
  B11111,
  B10001,
  B11111,
  B10001,
  B11111
};

byte bt3[8] = {
  B00000,
  B10101,
  B10101,
  B11111,
  B10101,
  B11101,
  B00001,
  B00000
};

byte bt4[8] = {
  B01110,
  B01000,
  B01110,
  B01000,
  B01110,
  B11111,
  B01000,
  B01111
};



void setup(){   
 // LED
  pinMode(ledPin, OUTPUT);
  
  //부저
  pinMode(tonePin, OUTPUT);

  // 버튼
  pinMode(btnPin, INPUT);
  
  // LCD
  lcd.begin();
  lcd.createChar(0, arrow);
  lcd.createChar(1, BEE);
  lcd.createChar(2, bt1);
  lcd.createChar(3, bt2);
  lcd.createChar(4, bt3);
  lcd.createChar(5, bt4);
  printLcd();

  // 인체감지센서
  pinMode(humanPin, INPUT);
  
  
  // 서보모터
  pinMode(rev_Right, INPUT); pinMode(led_Right, OUTPUT);
  pinMode(rev_Left, INPUT); pinMode(led_Left, OUTPUT);

  // 서보모터
  LWiper.attach(servo_Left);
  RWiper.attach(servo_Right);

  // 주행
  servoLeft.attach(moter_Left);
  servoRight.attach(moter_Right);

  // 시리얼 통신
  Serial.begin(9600);

  //블루투스 통신
  bluetooth.begin(9600);
  bluetooth.println("BEE 연결 완료");
  bluetooth.println("주행을 시작합니다.");
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
  
  // 블루투스
  // 아두이노 -> 안드로이드
  if (Serial.available()) {
    char toSend = (char)Serial.read();
    bluetooth.print(toSend);
  }

  // 안드로이드 -> 아두이노
  if(bluetooth.available()){
    char from=(char)bluetooth.read();
    msg=msg+from;
  }else{
    if(msg!=""){
      if(msg=="r"&&msg!=reset_msg){
        msg=reset_msg;
      }
      changeMsg();
    }
  }
  //인체감지센서
  int human_on = digitalRead(humanPin);
  if((human_on==HIGH) && (statusPIR == LOW)){
    bluetooth.print("생존자발견");
    Serial.println("생존자발견");
    statusPIR=HIGH;
  }else{
    if((human_on == LOW) && (statusPIR == HIGH)){
      statusPIR=LOW;}
  }
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


void printLcd(){
  lcd.backlight();
  lcd.clear();
  
  setFirst();
  setSecond();
}

void setFirst(){
  lcd.setCursor(0, 0);
  lcd.write(byte(0));
  lcd.setCursor(2, 0);
  lcd.print(reset_msg);
  lcd.setCursor(15, 0);
  lcd.write(byte(0));
}

void setSecond(){
  lcd.setCursor(0, 1);
  lcd.write(0);
  lcd.print(" ");
  lcd.write(4);
  lcd.write(5);
  lcd.print(" ");
  lcd.write(1);
  lcd.print(" ");
  lcd.write(2);
  lcd.write(3);
  lcd.print(" ");
  lcd.write(1);
  lcd.print(" ");
  lcd.write(4);
  lcd.write(5);
  lcd.print(" ");
  lcd.write(0);
}

void changeMsg(){
  lcd.setCursor(2,0);
  for(int i=2; i<14; i++){
    lcd.print(" ");
  }
  lcd.setCursor(2,0);
  lcd.print(msg);
  msg="";
}
