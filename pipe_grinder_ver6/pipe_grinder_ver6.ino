#include<SoftwareSerial.h>

#define PWM 11
#define DIR 12
#define BRK 13
#define RXD 8
#define TXD 7
#define BUTTON 2
#define Buzzer 4
SoftwareSerial bluetooth(RXD, TXD);
int preTime = 0;

bool runGrinder(unsigned long cur, int time){
    bool check = true;
    while(check){
      char emo = bluetooth.read();
      if (emo == '0'){
        Serial.println("강제 중지!");
        return false;
        }
      if (millis()-cur >= (time*1000)){
        Serial.println(millis()-cur);
        return check;
        }
    }
}

void stopMotor(int duty){
    Serial.println("동작 정지");
    analogWrite(PWM,0);     // duty cycle 0
    digitalWrite(BRK,HIGH); //브레이크 온
}

bool fwMotor(int duty, unsigned long cur, int time){
    Serial.println("정회전");
    digitalWrite(BRK,LOW);  // 브레이크 오프
    analogWrite(PWM,duty);    // duty 0~255
    digitalWrite(DIR,HIGH); // 정회전
    return runGrinder(cur, time);
}

bool bwMotor(int duty, unsigned long cur, int time){
    Serial.println("역회전");
    digitalWrite(BRK,LOW);  // 브레이크 오프
    analogWrite(PWM,duty);    // duty 0~255
    digitalWrite(DIR,LOW);  // 역회전
    return runGrinder(cur, time);
}

void setup() {
    Serial.begin(9600);
    bluetooth.begin(9600);
    pinMode(BUTTON, OUTPUT);
    pinMode(PWM, OUTPUT);
    pinMode(DIR, OUTPUT);
    pinMode(BRK, OUTPUT);
    analogWrite(PWM,0); // duty cycle 0%
    digitalWrite(DIR,LOW); // 정회전
    digitalWrite(BRK,LOW);  // 브레이크 없음
    attachInterrupt(digitalPinToInterrupt(2),BUTTONST,CHANGE);
}

void loop() {
    if (bluetooth.available()){
        
        char cmd = bluetooth.read();
        if (cmd == '5'){
            bool flag = true;
            unsigned long bwcur = millis();
            flag = bwMotor(40, bwcur, 30);
            stopMotor(0);
        }
        if (cmd == '1'){// 정회전, 정지
            Serial.println("연마 시작");
            bool flag = true;
            int run_count = 4;
            for (int i = 0; i < run_count; i++){
              
              if (flag){
                unsigned long fwcur = millis();
                flag = fwMotor(40, fwcur, 31);
              }
              if (flag){
                unsigned long bwcur = millis();
                flag = bwMotor(40, bwcur, 30);
              } 
            }
            stopMotor(0);
        }
    }
}
void BUTTONST(){
    if(digitalRead(BUTTON)==HIGH){
        analogWrite(PWM,0);
        digitalWrite(BRK,HIGH);
        digitalWrite(BRK,HIGH);
        tone(Buzzer,2000);
        Serial.println("EM");
    }
}