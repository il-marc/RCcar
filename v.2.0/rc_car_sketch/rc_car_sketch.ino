/*********************************************************
* RCcar
*
* This sketch, for the Arduino, driving cars, depending on 
* commands received by bluetooth module HC-04.
*
* by Ilia Marchenko
* 
* https://github.com/il-marc/RCcar
**********************************************************/
// commands definition
#define FORWARD_COMMAND  'w'
#define BACKWARD_COMMAND 's'
#define STOP_COMMAND     ' '
#define LEFT_COMMAND     'a'
#define RIGHT_COMMAND    'd'
#define CENTER_COMMAND   'q'
#define VOLTAGE_COMMAND  'V'
#define SPEED_COMMAND    'S'
// PINSs definition
#define VOLTAGE_PIN A0
#define SERVO_PIN 11
#define DRIVER_ENABLE 10
#define DRIVER_INPUT_1 9
#define DRIVER_INPUT_2 8
// servo settings
#define SERVO_LEFT 544
#define SERVO_MID 1500
#define SERVO_RIGHT 2400
#include <Servo.h>

Servo servo;
// the setup routine runs once when you press reset
void setup() {
  // servo setting up
  servo.attach(SERVO_PIN);
  center();
  // L293DD driver setting up
  pinMode(DRIVER_ENABLE, OUTPUT);
  pinMode(DRIVER_INPUT_1, OUTPUT);
  pinMode(DRIVER_INPUT_2, OUTPUT);
  brake();
  // bluetooth HC-04 setting up
  Serial.begin(9600);
}
// the loop routine runs over and over again forever
void loop() {
  if (Serial.available()) {
    switch (Serial.read()) {
      case FORWARD_COMMAND: forward();
        break;
      case BACKWARD_COMMAND: backward();
        break;
      case STOP_COMMAND: brake();
        break;
      case LEFT_COMMAND: left();
        break;
      case RIGHT_COMMAND: right();
        break;
      case CENTER_COMMAND: center();
        break;
      case VOLTAGE_COMMAND: measureVoltage();        
        break;
      case SPEED_COMMAND: measureSpeed();        
        break;
    }                     
  } 
}

void forward() {
  digitalWrite(DRIVER_ENABLE,LOW);   
  digitalWrite(DRIVER_INPUT_1,HIGH);
  digitalWrite(DRIVER_INPUT_2,LOW);
  digitalWrite(DRIVER_ENABLE,HIGH);  
}

void backward() {
  digitalWrite(DRIVER_ENABLE,LOW);   
  digitalWrite(DRIVER_INPUT_1,LOW);
  digitalWrite(DRIVER_INPUT_2,HIGH);
  digitalWrite(DRIVER_ENABLE,HIGH); 
}

void brake() {
  digitalWrite(DRIVER_ENABLE,LOW);
  digitalWrite(DRIVER_INPUT_1,HIGH);
  digitalWrite(DRIVER_INPUT_2,HIGH);
  digitalWrite(DRIVER_ENABLE,HIGH); 
}

void left() {
  servo.writeMicroseconds(SERVO_LEFT);
}

void right() {
  servo.writeMicroseconds(SERVO_RIGHT);
}

void center() {  
  servo.writeMicroseconds(SERVO_MID);
}

void measureVoltage() {
  int voltageValue;
  char voltageValueByte;
  voltageValue = analogRead(voltageValue);
  voltageValueByte = map(voltageValue, 0, 1023, 0, 255);
  Serial.write(VOLTAGE_COMMAND);
  Serial.write(voltageValueByte);
  Serial.write('\0');
}
void measureSpeed() {
  Serial.print(SPEED_COMMAND);
  Serial.write('\0');
  Serial.write('\0');
}
