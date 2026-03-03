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

void forward(int speed) {
  move(LEFT_FORWARD, speed, RIGHT_FORWARD, speed);
}

void backward(int speed) {
  move(LEFT_BACKWARD, speed, RIGHT_BACKWARD, speed);
}

void turn_left(int steepness) {
  move(LEFT_FORWARD, 255, RIGHT_FORWARD, steepness);
}

void turn_right(int steepness) {
  move(LEFT_FORWARD, steepness, RIGHT_FORWARD, 255);
}


void rotate_left(int speed) {
  move(LEFT_BACKWARD, speed, RIGHT_FORWARD, speed);

}

void rotate_right(int speed) {
  move(LEFT_FORWARD, speed, RIGHT_BACKWARD, speed);
}

void setup() {
  pinMode(DIR_RIGHT, OUTPUT);
  pinMode(SPEED_RIGHT, OUTPUT);
  pinMode(DIR_LEFT, OUTPUT);
  pinMode(SPEED_LEFT, OUTPUT);

  forward(100);
  delay(2000);
  backward(100);
  delay(2000);
  turn_left(100);
  delay(2000);
  turn_right(100);
  delay(2000);
  rotate_left(100);
  delay(2000);
  rotate_right(100);
  delay(2000);
  forward(0);
}

void loop() {
}
