/*********************************************************
* RCcar
*
* This sketch, for the Arduino, driving car, depending on 
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
#define TURN_COMMAND     'q'
#define VOLTAGE_COMMAND  'V'
#define SPEED_COMMAND    'S'
// PINSs definition
#define VOLTAGE_PIN A3
#define SERVO_PIN 3
#define L_PIN 13
#define DRIVER_ENABLE 11
#define DRIVER_INPUT_1 A1
#define DRIVER_INPUT_2 A2
#include <Servo.h>

Servo servo;
volatile int LState = LOW;
int motorSpeed;
int pos;
// the setup routine runs once when you press reset
void setup() {
  // bluetooth HC-04 setting up
  Serial.begin(9600);
  while (!Serial) {}
  // servo setting up
  servo.attach(SERVO_PIN);
  // L293DD driver setting up
  pinMode(DRIVER_ENABLE, OUTPUT);
  pinMode(DRIVER_INPUT_1, OUTPUT);
  pinMode(DRIVER_INPUT_2, OUTPUT);
  attachInterrupt(0, abort, CHANGE);
}
// the loop routine runs over and over again forever
void loop() {
  if (Serial.available() == 2) {
    char incByte = Serial.read();
    //КОСТЫЛЬ
    //При пулчении числа более 128 в буфере появляется 194 или 195
    if (Serial.peek() == 194 or Serial.peek() == 195)
    {
      Serial.read();
      while (!Serial.available()) {}
    }
    switch (incByte) {
      case FORWARD_COMMAND:  
        motorSpeed = Serial.read()*2;
        forward(motorSpeed);
        break;
      case BACKWARD_COMMAND: 
        motorSpeed = Serial.read()*2;
        backward(motorSpeed);
        break;
      case STOP_COMMAND: 
        Serial.read();
        brake();
        break;
      case TURN_COMMAND: 
        pos = Serial.read();
        turn(pos);
        break;
      case VOLTAGE_COMMAND: 
        Serial.read();
        measureVoltage();        
        break;
      //not implemented
      case SPEED_COMMAND: 
        Serial.read();
        measureSpeed();        
        break;
      default: 
        break;
    }                     
  } 
}

void forward(int motorSpeed) {
  analogWrite(DRIVER_ENABLE,0);   
  digitalWrite(DRIVER_INPUT_1,HIGH);
  digitalWrite(DRIVER_INPUT_2,LOW);
  analogWrite(DRIVER_ENABLE,motorSpeed); 
}

void backward(int motorSpeed) {
  analogWrite(DRIVER_ENABLE,0);   
  digitalWrite(DRIVER_INPUT_1,LOW);
  digitalWrite(DRIVER_INPUT_2,HIGH);
  analogWrite(DRIVER_ENABLE,motorSpeed); 
}

void brake() {
  analogWrite(DRIVER_ENABLE,LOW);
  digitalWrite(DRIVER_INPUT_1,HIGH);
  digitalWrite(DRIVER_INPUT_2,HIGH);
  analogWrite(DRIVER_ENABLE,255); 
}
void turn(int pos) {
  servo.write(pos);
}
void abort() {
  LState = !LState;
  digitalWrite(L_PIN, LState);
  brake();
  if (pos != 90)
  {
    pos = 90;
    turn(pos);
  }
}
void measureVoltage() {
  int voltageValue;
  char voltageValueByte;
  voltageValue = analogRead(VOLTAGE_PIN);
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
