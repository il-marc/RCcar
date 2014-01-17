#include <Servo.h>

#define VOLTAGE_PIN A0
#define SERVO_PIN 11
#define ENABLE 10
#define INPUT1 9
#define INPUT2 8

Servo servo;
char incomingByte = 0;
int voltageValue;
char voltageValueByte;

void setup() {
  servo.attach(SERVO_PIN);
  pinMode(ENABLE, OUTPUT);
  pinMode(INPUT1, OUTPUT);
  pinMode(INPUT2, OUTPUT);
  Serial.begin(9600);
}
void loop() {
  if (Serial.available() > 0) {
    incomingByte = Serial.read();
    switch (incomingByte) {
      case 'w': forward();
        break;
      case 's': backward();
        break;
      case ' ': brake();
        break;
      case 'a': left();
        break;
      case 'd': right();
        break;
      case 'q': center();
        break;
      case 'v': 
        voltageValue = analogRead(voltageValue);
        voltageValueByte = map(voltageValue, 0, 1023, 0, 255);
        Serial.print(voltageValueByte);
        break;
    }                     
  } 
}
void forward() {
  digitalWrite(ENABLE,LOW);   
  digitalWrite(INPUT1,HIGH);
  digitalWrite(INPUT2,LOW);
  digitalWrite(ENABLE,HIGH);  
}
void backward() {
  digitalWrite(ENABLE,LOW);   
  digitalWrite(INPUT1,LOW);
  digitalWrite(INPUT2,HIGH);
  digitalWrite(ENABLE,HIGH); 
}
void brake() {
  digitalWrite(ENABLE,LOW);
  digitalWrite(INPUT1,HIGH);
  digitalWrite(INPUT2,HIGH);
  digitalWrite(ENABLE,HIGH); 
}
void left() {
  servo.write(75);
}
void right() {
  servo.write(105); 
}
void center() {  
  servo.write(90);
}
