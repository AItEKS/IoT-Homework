#define DIR_LEFT 7
#define SPEED_LEFT 6

#define DIR_RIGHT 4
#define SPEED_RIGHT 5

#define LEFT_FORWARD HIGH
#define LEFT_BACKWARD LOW

#define RIGHT_FORWARD LOW
#define RIGHT_BACKWARD HIGH

void move(bool left_dir, int left_speed, bool right_dir, int right_speed) {
  digitalWrite(DIR_LEFT, left_dir);
  digitalWrite(DIR_RIGHT, right_dir);
  analogWrite(SPEED_LEFT, left_speed);
  analogWrite(SPEED_RIGHT, right_speed);
}

void setup() {
  pinMode(DIR_RIGHT, OUTPUT);
  pinMode(SPEED_RIGHT, OUTPUT);
  pinMode(DIR_LEFT, OUTPUT);
  pinMode(SPEED_LEFT, OUTPUT);

  digitalWrite(DIR_RIGHT, RIGHT_FORWARD);
  digitalWrite(DIR_LEFT, LEFT_FORWARD);
  digitalWrite(SPEED_RIGHT, LOW);
  digitalWrite(SPEED_LEFT, HIGH);
  delay(2000);
  digitalWrite(DIR_RIGHT, RIGHT_FORWARD);
  digitalWrite(DIR_LEFT, LEFT_BACKWARD);
  digitalWrite(SPEED_RIGHT, HIGH);
  digitalWrite(SPEED_LEFT, LOW);

  delay(2000);
  move(LEFT_FORWARD, 255, RIGHT_FORWARD, 0);
  delay(2000);
  move(LEFT_FORWARD, 0, RIGHT_FORWARD, 255);
  delay(2000);
  move(LEFT_FORWARD, 0, RIGHT_FORWARD, 0);
}

void loop() {
}
