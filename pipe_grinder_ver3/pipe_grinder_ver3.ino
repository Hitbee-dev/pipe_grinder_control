#include <SoftwareSerial.h>
#define PWM 11
#define DIR 12
#define BRK 13
#define RXD 8
#define TXD 7
#define BUTTON 6
SoftwareSerial bluetooth(RXD, TXD);
boolean state = true;
void origin(){
    
}

void fwMotor(int times) {
  digitalWrite(BRK,LOW);//브레이크 오프
  analogWrite(PWM,50); // duty 0~255
  digitalWrite(DIR,LOW);  //정회전
  Serial.println("정회전");
  unsigned long checktime = millis();
  while(state){
    char flag = bluetooth.read();
    if (checktime >= times){
      break;
    } else if (flag == '0') {
      state = false;
      break;
    }
  }
}

void bwMotor(int times) {
  digitalWrite(BRK,LOW);//브레이크 오프
  analogWrite(PWM,50); // duty 0~255
  digitalWrite(DIR,HIGH);  //역회전
  Serial.println("역회전");
  unsigned long checktime = millis();
  while(state){
    char flag = bluetooth.read();
    if (checktime >= times || flag == '0'){
      break;
    } else if (flag == '0') {
      state = false;
      break;
    }
  }
}

void stopMotor() {
  analogWrite(PWM,0);// duty cycle 0
  digitalWrite(BRK,HIGH);//브레이크 온
  Serial.println("정지");  
}

void runMotor(int times) {
  fwMotor(times);
  (state != false) ? bwMotor(times) : stopMotor();
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
  stopMotor();
  // analogWrite(PWM,0); // duty cycle 0%
  // digitalWrite(DIR,LOW); // 정회전
  // digitalWrite(BRK,LOW);  // 브레이크 없음
}
void loop() {             
  if (bluetooth.available()) {
    char cmd = bluetooth.read();
    Serial.println(cmd);
    if (cmd == '1'){
      state = true;
      runMotor(26000);
    } else if (cmd == '2'){
      state = true;
      fwMotor(5000);
      stopMotor();
    } else if (cmd == '3'){
      state = true;
      bwMotor(5000);
      stopMotor();
    } else {
      state = true;
      stopMotor();
     }
  }
 }   
