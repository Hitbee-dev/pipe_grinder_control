#include<SoftwareSerial.h>

#define PWM 11
#define DIR 12
#define BRK 13
#define RXD 8
#define TXD 7
#define BUTTON 2
#define Buzzer 4
SoftwareSerial bluetooth(RXD, TXD);

void fwMotor(int duty){
    digitalWrite(BRK,LOW);  // 브레이크 오프
    analogWrite(PWM,duty);    // duty 0~255
    digitalWrite(DIR,HIGH); // 정회전
    Serial.println("정회전");
}

void bwMotor(int duty){
    digitalWrite(BRK,LOW);  // 브레이크 오프
    analogWrite(PWM,duty);    // duty 0~255
    digitalWrite(DIR,LOW);  // 역회전
    Serial.println("역회전");
}

void stopMotor(int duty){
    analogWrite(PWM,0);     // duty cycle 0
    digitalWrite(BRK,HIGH); //브레이크 온
    Serial.println("동작 정지");
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
	char tg = '1';      //toogle
    if (cmd == '1'){    // 정회전, 역회전, 정지
        Serial.println("1");
        fwMotor(50);
        delay(40000);

        bwMotor(50);
        delay(40000);

        stopMotor(0);
    } else if (cmd == '2'){// 정회전, 정지
        Serial.println("2");
        fwMotor(50);
        delay(5000);
        
        stopMotor(0);
    } else if (cmd == '3'){// 역회전, 정지
        Serial.println("3");  
        bwMotor(50);
        delay(5000);

        stopMotor(0);
    } else if (cmd == '5'){// 테스트
        Serial.println("5");
		fwMotor(50);
		delay(27000);

		bwMotor(50);
		delay(26000);
		
        stopMotor(0);

        fwMotor(40);
        delay(36000);

        bwMotor(40);
        delay(35000);
        
        stopMotor(0);

        fwMotor(40);
        delay(36000);

        bwMotor(40);
        delay(35000);

        stopMotor(0);

        fwMotor(40);
        delay(36000);

        bwMotor(40);
        delay(35000);

        stopMotor(0);
    } else if(cmd == '6'){
        Serial.println(tg);
		if(tg == '1'){
			analogWrite(PWM,0);
			digitalWrite(BRK,HIGH);
			Serial.println("PAUSE");
			tg = '2';
            Serial.println(tg);
		} else if(tg == '2'){
			digitalWrite(BRK,LOW);
			analogWrite(PWM,40);
			Serial.println("RESUME");
			tg = '1';
		}
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