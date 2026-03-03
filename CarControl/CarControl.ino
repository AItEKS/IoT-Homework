#define DIR_LEFT 7
#define SPEED_LEFT 6

#define DIR_RIGHT 4
#define SPEED_RIGHT 5

void setup() {
  pinMode(DIR_LEFT, OUTPUT);
  pinMode(SPEED_LEFT, OUTPUT);
  pinMode(DIR_RIGHT, OUTPUT);
  pinMode(SPEED_RIGHT, OUTPUT);
  digitalWrite(DIR_LEFT, HIGH);
  digitalWrite(DIR_RIGHT, LOW);
  digitalWrite(SPEED_LEFT, HIGH);
  digitalWrite(SPEED_RIGHT, LOW);
  delay(2000);
  digitalWrite(DIR_LEFT, LOW);
  digitalWrite(DIR_RIGHT, LOW);
  digitalWrite(SPEED_LEFT, LOW);
  digitalWrite(SPEED_RIGHT, HIGH);
}

void loop() {
}
