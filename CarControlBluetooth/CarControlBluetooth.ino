#define SPEED_RIGHT 5
#define DIR_RIGHT 4

#define SPEED_LEFT 6
#define DIR_LEFT 7

#define FORWARD  HIGH
#define BACK  LOW

// === Скорости ===
#define DEFAULT_SPEED   180  
#define ROTATE_SPEED    200  

// === Функции движения ===
void forward(int speed){ move(FORWARD, speed, FORWARD, speed); }
void backward(int speed){ move(BACK, speed, BACK, speed); }    
void turn_left(int steepness){ move(FORWARD, steepness, FORWARD, 255); }
void turn_right(int steepness){ move(FORWARD, 255, FORWARD, steepness); }
void rotate_right(int speed){ move(FORWARD, speed, BACK, speed); }   
void rotate_left(int speed) { move(BACK, speed, FORWARD, speed); }
void stopMotors(){ move(LOW, 0, LOW, 0); }  // полная остановка

// === Заглушка move() — замените на свою реализацию, если есть ===
void move(int dirLeft, int speedLeft, int dirRight, int speedRight) {
  digitalWrite(DIR_LEFT, dirLeft);
  analogWrite(SPEED_LEFT, speedLeft);
  digitalWrite(DIR_RIGHT, dirRight);
  analogWrite(SPEED_RIGHT, speedRight);
}



#include <SoftwareSerial.h>

SoftwareSerial btSerial(10, 11); // RX, TX — подключение к модулю HC-05/HC-06

void setup() {
  Serial.begin(57600);
  while (!Serial) { ; }
  Serial.println(F("🤖 Robot ready. Bluetooth on pins 10/11"));

  // Bluetooth Serial
  btSerial.begin(9600);  
  btSerial.println(F("BT:Connected"));


  pinMode(SPEED_LEFT, OUTPUT);
  pinMode(DIR_LEFT, OUTPUT);
  pinMode(SPEED_RIGHT, OUTPUT);
  pinMode(DIR_RIGHT, OUTPUT);
  
  stopMotors();  
}

// === Обработка одной команды ===
void executeCommand(char cmd) {
  switch(cmd) {
    case 'F': 
      forward(DEFAULT_SPEED); 
      break;
    case 'B': 
      backward(DEFAULT_SPEED); 
      break;
    case 'S': 
      stopMotors(); 
      break;
    case 'R': 
      rotate_right(ROTATE_SPEED);  
      break;
    case 'L': 
      rotate_left(ROTATE_SPEED);   
      break;
    default:
      return; 
  }
}

void loop() {
  while (btSerial.available()) {
    char cmd = btSerial.read();
    if (cmd == ' ' || cmd == '\n' || cmd == '\r' || cmd == '\t') continue;
    executeCommand(toupper(cmd));
  }


}
