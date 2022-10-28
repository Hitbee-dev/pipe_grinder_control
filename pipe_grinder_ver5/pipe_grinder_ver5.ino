#include<SoftwareSerial.h>

#define PWM 11
#define DIR 12
#define BRK 13
#define RXD 8
#define TXD 7
#define BUTTON 2
#define Buzzer 4
SoftwareSerial bluetooth(RXD, TXD);
bool flag;
int run_count;

void runGrinder(unsigned long cur, int time){
    bool check = true;
    while(check){
      char emo = bluetooth.read();
      if (emo == '0'){
        Serial.println("강제 중지!");
        flag = false;
        break;
        }
      if (millis()-cur >= (time*1000)){
        Serial.println(millis()-cur);
        break;
        }
    }  
}

void stopMotor(int duty){
    Serial.println("동작 정지");
    analogWrite(PWM,0);     // duty cycle 0
    digitalWrite(BRK,HIGH); //브레이크 온
}

void fwMotor(int duty, unsigned long cur, int time){
    Serial.println("정회전");
    digitalWrite(BRK,LOW);  // 브레이크 오프
    analogWrite(PWM,duty);    // duty 0~255
    digitalWrite(DIR,HIGH); // 정회전
    runGrinder(cur, time);
}

void bwMotor(int duty, unsigned long cur, int time){
    Serial.println("역회전");
    digitalWrite(BRK,LOW);  // 브레이크 오프
    analogWrite(PWM,duty);    // duty 0~255
    digitalWrite(DIR,LOW);  // 역회전
    runGrinder(cur, time);
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
        run_count = 0;
        flag = true;
        if (cmd == '1'){// 정회전, 정지
            Serial.println("연마 시작");  
            while(flag){
                if (run_count == 4){
                    Serial.println("연마 종료");
                    break;
                }
                unsigned long cur = millis(); 
                fwMotor(40, cur, 36);
                if (flag){
                  bwMotor(40, cur, 35);
                  }
                run_count++;
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