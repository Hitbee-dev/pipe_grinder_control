#include <SoftwareSerial.h>
#include <Adafruit_VL53L0X.h>

#define PWM 11
#define DIR 12
#define BRK 13
#define RXD 8
#define TXD 7
#define BUTTON 2
#define Buzzer 4
SoftwareSerial bluetooth(RXD, TXD);
Adafruit_VL53L0X lox = Adafruit_VL53L0X();
int preTime = 0;

int distanceSensor(){
    VL53L0X_RangingMeasurementData_t measure;
    
    Serial.print("Reading a measurement... ");
    lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!
  
    if (measure.RangeStatus != 4) {  // phase failures have incorrect data
      Serial.print("Distance (mm): ");
      Serial.println(measure.RangeMilliMeter);
      return measure.RangeMilliMeter;
    } else {
      Serial.println(" out of range ");
      return 0;
    }
    return -1;
}

bool runGrinder(unsigned long cur, int time){
//    distanceSensor();
    bool check = true;
    while(check){
      int distanceS = distanceSensor();
      Serial.println(distanceS);
      char emo = bluetooth.read();
      if (emo == '0'){
        Serial.println("강제 중지!");
        return false;
        }
      if (distanceS < 86){
        Serial.println("시작 지점에 도착");
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

    // wait until serial port opens for native USB devices
    while (! Serial) {
      delay(1);
    }
    
    Serial.println("Adafruit VL53L0X test");
    if (!lox.begin()) {
      Serial.println(F("Failed to boot VL53L0X"));
      while(1);
    }
    // power 
    Serial.println(F("VL53L0X API Simple Ranging example\n\n")); 
}

void loop() {
//    distanceSensor();
  
    if (bluetooth.available()){
        
        char cmd = bluetooth.read();

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
        if (cmd == '2'){
            bool flag = true;
            unsigned long fwcur = millis();
            flag = fwMotor(40, fwcur, 30);
            stopMotor(0);
        }
        if (cmd == '3'){
            bool flag = true;
            unsigned long bwcur = millis();
            flag = bwMotor(40, bwcur, 30);
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