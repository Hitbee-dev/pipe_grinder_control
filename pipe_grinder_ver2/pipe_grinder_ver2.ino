#include <SoftwareSerial.h>
#define PWM 11
#define DIR 12
#define BRK 13
#define RXD 8
#define TXD 7
#define BUTTON 6
SoftwareSerial bluetooth(RXD, TXD);
boolean state = false;

void origin(){
    
}

void fwMotor(int times) {
  state = true;
  digitalWrite(BRK,LOW);//브레이크 오프
  analogWrite(PWM,50); // duty 0~255
  digitalWrite(DIR,LOW);  //정회전
  
  unsigned long idle = millis();
  Serial.println(idle);
  while((times*1000) >= idle){
    
    Serial.println(idle);
  }
}

void bwMotor(int times) {
  state = true;
  digitalWrite(BRK,LOW);//브레이크 오프
  analogWrite(PWM,50); // duty 0~255
  digitalWrite(DIR,HIGH);  //역회전
}

void stopMotor() {
  state = false;
  analogWrite(PWM,0);// duty cycle 0
  digitalWrite(BRK,HIGH);//브레이크 온  
}

void runMotor(int times) {
  fwMotor(times);
  bwMotor(times);
  stopMotor();
}

void setup()
{
  Serial.begin(9600);
  bluetooth.begin(9600);
  pinMode(BUTTON, OUTPUT);
  pinMode(PWM, OUTPUT);
  pinMode(DIR, OUTPUT);
  pinMode(BRK, OUTPUT);
  analogWrite(PWM,0); // duty cycle 0%
  digitalWrite(DIR,LOW); // 정회전
  digitalWrite(BRK,LOW);  // 브레이크 없음
}
void loop() {             
  if (bluetooth.available()) {
    char cmd = bluetooth.read();
    Serial.println(cmd);
    if (cmd == '1' && state == false){
      runMotor(26);
    } else if (cmd == '2' && state == false){
      fwMotor(5);
      stopMotor();
    } else if (cmd == '3' && state == false){
      bwMotor(5);
      stopMotor();
    } else {
      stopMotor();
      Serial.println("Err");
     }
  }
 }   
