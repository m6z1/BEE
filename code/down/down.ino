#include <Servo.h>   
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include <DFRobotDFPlayerMini.h>

#define btnPin 9
#define BT_TXD 10
#define BT_RXD 11
#define humanPin 7
#define MP3Pin_1 4
#define MP3Pin_2 5

//블루투스
SoftwareSerial bluetooth(BT_RXD, BT_TXD);

// 인체감지센서
int statusPIR = 0;

//버튼
int statusBtn = 0;

//MP3 플레이어
SoftwareSerial MP3Module(MP3Pin_1, MP3Pin_2);

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


void setup()     
{
  //블루투스 통신
  bluetooth.begin(9600);

  // 버튼
  pinMode(btnPin, INPUT);
  
  //인체감지센서
  pinMode(humanPin, INPUT);
  
  // LCD
  lcd.begin();
  lcd.createChar(0, arrow);
  lcd.createChar(1, BEE);
  lcd.createChar(2, bt1);
  lcd.createChar(3, bt2);
  lcd.createChar(4, bt3);
  lcd.createChar(5, bt4);
  printLcd();

  // MP3 플레이어
  MP3Module.begin(9600);
  if (!MP3Player.begin(MP3Module)) {
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1. Please recheck the connection!"));
    Serial.println(F("2. Please insert the SD card!");
    while(true);
  }
  delay(1);
  MP3Player.volume(15);
}
 
void loop()  
{ 
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
    statusPIR=HIGH;
  }else{
    if((human_on == LOW) && (statusPIR == HIGH)){
      statusPIR=LOW;
    } 
  }

  //버튼
  int btn_on = digitalRead(btnPin);
  if((btn_on==HIGH) && (statusBtn == LOW)){
    bluetooth.print("구조요청");
    statusBtn=HIGH;
  }else{
    if((btn_on == LOW) && (statusBtn == HIGH)){
      statusBtn=LOW;
    } 
  }

  // MP3 플레이어
  MP3Player.play(1);
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
