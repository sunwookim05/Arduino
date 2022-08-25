#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h" // DHT11를 사용하기 위한 라이브러리 호출하기

#define DHTPIN 3
#define DHTTYPE DHT11

int piezoPin = 2; // 피에조 부저의 +극을 오렌지보드 2번에 연결
int airCon = 7;  // RGB LED의 파란색 핀을 디지털 7번 연결
int notes[] = { 4186, 4698, 5274, 5587, 6271 };
// 가까울 시 울릴 경보의 멜로디 작성
boolean flag = 0;
int C = 1047; // 도
int D = 1175; // 레 
int E = 1319; // 미 
int F = 1397; // 파 
int G = 1568; // 솔 
int A = 1760; // 라 
int B = 1976; // 시 

int piezoPin = 2; // 피에조 부저의 +극을 오렌지보드 2번에 연결
int cdsPin = A0; // 조도센서를 오렌지보드 A0번에 연결
int irPin = A1;            //적외선장애물센서를 오렌지보드 A1번에 연결
int servo = 6;  // 서보모터를 오렌지보드 A0번에 연결
int piezoPin = 2; // 피에조 부저의 +극을 오렌지보드 2번에 연결
int notes[] = { 4186, 6271 };
int tempo = 200; // 멜로디의 음 길이 설정
int flag = 1;  // 블라인드의 상태를 저장하는 변수

LiquidCrystal_I2C TV(0x3F, 16, 2);
Servo blind;  // 서보모터를 blind라는 이름으로 선언

int notes[25] = { G, G, A, A, G, G, E, G, G, E, E, D, G, G, A, A, G, G, E, G, E,
      D, E, C };
// 학교종이 땡땡땡 멜로디 작성
int tempo = 200; // 멜로디의 음 길이 설정

void setup() {
   // 시리얼 통신을 위해 통신속도(Baudrate)를 9600으로 설정
   dht.begin();
   Serial.begin(9600);
   pinMode(piezoPin, OUTPUT); // 피에조 핀을 출력핀으로 설정\
        TV.init();
   TV.backlight();
   TV.setCursor(0, 0);
   TV.print("Looking TV....");
   blind.attach(servo);  // 서보모터를 6번핀에 연결
   blind.write(80); // 서보모토의 기본값을 80도로 설정
   pinMode(piezoPin, OUTPUT);  // 피에조 핀을 출력핀으로 설정
   pinMode(airCon, OUTPUT);  // 3색 LED중 파란색 핀을 출력핀으로 설정
}

void loop() {
   float humidity = dht.readHumidity();
   float temperature = dht.readTemperature();
   int IRval = analogRead(irPin);

   if (isnan(humidity) || isnan(temperature)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
   }

   float discomfortIndex = (1.8 * temperature)
         - (0.55 * (1 - humidity / 100.0) * (1.8 * temperature - 26)) + 32;
   Serial.println(discomfortIndex);

   // 불쾌지수값이 70보다 높을 경우
   if (discomfortIndex > 70) {
      // 에어콘(파란색 LED)를 켜주면서 멜로디를 재생
      digitalWrite(airCon, HIGH);
      if (flag == 0) {
         for (int i = 0; i < 5; i++) {
            tone(piezoPin, notes[i], 300);
            delay(150);
         }
         flag = 1;
      }
   }

   else {
      // 에어콘(파란색 LED)를 끔
      digitalWrite(airCon, LOW);
      flag = 0;
   }

   // 조도센서 값 측정 후 시리얼 모니터에 출력
   int cdsValue = analogRead(cdsPin);
   Serial.println(cdsValue);
   if (cdsVal < 400) {
      // 현재 블라인드가 올라가있으면
      if (flag == 0) {
         // 멜로디를 출력하고
         for (int i = 0; i < 2; i++) {
            tone(piezoPin, notes[i], tempo);
            delay(100);
         }
         // 블라인드를 내림
         blind.write(180);
         // 블라인드가 내려가 있는 상태로 설정
         flag = 1;
      }
   }

   // 날이 어두우면(빛이 어두워지면)
   else {
      // 블라인드가 내려가 있으면
      if (flag == 1) {
         // 블라인드를 올림
         blind.write(80);
         // 블라인드가 올라가 있는 상태로 설정
         flag = 0;
      }
   }
   if (IRval < 100) {          //TV가 켜져있고 거리가 가깝다면 TV를 끈다
      if (flag == 1) {
         flag = 0;
         TV.clear();
         TV.noBacklight();
      }
      tone(2, 1000, 300);       //거리가 가깝다면 피에조부저에서 0.3초 간격으로 알람이 울린다
      delay(600);
   }

   else if (IRval > 900 && flag == 0) {  //TV가 꺼져있고 거리가 멀어지면 TV를 켠다
      flag = 1;
      TV.backlight();
      TV.print("Looking TV....");
      noTone(2);
   }
   // 조도센서로 측정되는 빛의 밝기가 어두울 경우
   if (cdsValue > 700) {
      // 멜로디 재생
      for (int i = 0; i < 12; i++) {
         tone(piezoPin, notes[i], tempo);
         delay(300);
      }
      delay(100); // 멜로디 중간에 짧게 멈추는 용도

      for (int i = 12; i < 25; i++) {
         tone(piezoPin, notes[i], tempo);
         delay(300);
      }
   }
}
