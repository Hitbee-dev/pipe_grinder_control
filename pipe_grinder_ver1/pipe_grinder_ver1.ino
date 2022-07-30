#include <SoftwareSerial.h>
#define PWM 11
#define DIR 12
#define BRK 13
#define RXD 8
#define TXD 7
#define BUTTON 6
SoftwareSerial bluetooth(RXD, TXD);



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
void loop()
{             
  if (bluetooth.available()){

    char cmd = bluetooth.read();
    
    switch(cmd){
      case '1': //1: 높이조절 후 파이프 연마 시작
    digitalWrite(BRK,LOW);//브레이크 오프
    analogWrite(PWM,50); // duty 0~255
  digitalWrite(DIR,LOW);  //정회전
  delay(26000);//26초

  analogWrite(PWM,50); // duty 0~255
  digitalWrite(DIR,HIGH);  //역회전
  delay(26000);//26초
  analogWrite(PWM,0);// duty cycle 0
    digitalWrite(BRK,HIGH);//브레이크 온
    
  break;

    case '3': // 3: 파이프 높이조절용 역회전
    digitalWrite(BRK,LOW); //브레이크 오프
    analogWrite(PWM,50); // duty 0~255
    digitalWrite(DIR,HIGH); //역회전
    delay(5000); // 5초
    analogWrite(PWM,0);// duty cycle 0
    digitalWrite(BRK,HIGH); // 브레이크 온

    break;

    case '2': // 2: 파이프 높이조절용 정회전
    digitalWrite(BRK,LOW); //브레이크 오프
    analogWrite(PWM,50); // duty 0~255
    digitalWrite(DIR,LOW); //정회전
    delay(5000); // 5초
    analogWrite(PWM,0);// duty cycle 0
    digitalWrite(BRK,HIGH); // 브레이크 온

    break;

    
    case '0': // 0: 정지 브레이크 온
    analogWrite(PWM,0);// duty cycle 0
    digitalWrite(BRK,HIGH);//브레이크 온
    break;

      
      
      default: //나머지 값은 모터정지 및 'Err' 출력
      analogWrite(PWM,0);// duty cycle 0
    digitalWrite(BRK,HIGH);//브레이크 온
    Serial.println("Err");
    }
}
 }   
