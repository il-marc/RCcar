#define ENABLE1 11
#define ENABLE2 9
#define INPUT1 12
#define INPUT2 13
#define INPUT3 8
#define INPUT4 10

char incomingByte = 0;

void setup() {
  pinMode(ENABLE1, OUTPUT);
  pinMode(ENABLE2, OUTPUT);
  pinMode(INPUT1, OUTPUT);
  pinMode(INPUT2, OUTPUT);
  pinMode(INPUT3, OUTPUT);
  pinMode(INPUT4, OUTPUT);
  Serial.begin(9600);
}
void loop() {
        if (Serial.available() > 0) {
                incomingByte = Serial.read();
                Serial.print(incomingByte);
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
                  default: Serial.print(" error ");
                    break;
                }                     
        } 
}
void forward() {
  digitalWrite(ENABLE2,LOW);   
  digitalWrite(INPUT3,HIGH);
  digitalWrite(INPUT4,LOW);
  digitalWrite(ENABLE2,HIGH);  
}
void backward() {
  digitalWrite(ENABLE2,LOW);   
  digitalWrite(INPUT3,LOW);
  digitalWrite(INPUT4,HIGH);
  digitalWrite(ENABLE2,HIGH); 
}
void neutrally() {
  digitalWrite(ENABLE2,LOW); 
}
void brake() {
  digitalWrite(ENABLE2,LOW);
  digitalWrite(INPUT3,HIGH);
  digitalWrite(INPUT4,HIGH);
  digitalWrite(ENABLE2,HIGH); 
}
void left() {
  digitalWrite(ENABLE1,LOW);
  digitalWrite(INPUT1,HIGH);
  digitalWrite(INPUT2,LOW); 
  digitalWrite(ENABLE1,HIGH); 
}
void right() {
  digitalWrite(ENABLE1,LOW);  
  digitalWrite(INPUT1,LOW);  
  digitalWrite(INPUT2,HIGH); 
  digitalWrite(ENABLE1,HIGH); 
}
void center() {  
  digitalWrite(ENABLE1,LOW);  
  digitalWrite(INPUT1,HIGH); 
  digitalWrite(INPUT2,HIGH); 
  digitalWrite(ENABLE1,HIGH);
}
