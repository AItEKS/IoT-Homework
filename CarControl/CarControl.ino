#define DIR_LEFT 7
#define SPEED_LEFT 6

#define DIR_RIGHT 4
#define SPEED_RIGHT 5

#define TRIG_FRONT 8
#define ECHO_FRONT 9

#define TRIG_LEFT 10
#define ECHO_LEFT 11

#define LEFT_FORWARD HIGH
#define LEFT_BACKWARD LOW

#define RIGHT_FORWARD HIGH
#define RIGHT_BACKWARD LOW

#define THR 7

#define WF 7
#define WC 4

enum RobotState {
  STATE_FORWARD,
  STATE_ROTATE_RIGHT,
  STATE_ROTATE_LEFT,
  STATE_TURN_RIGHT,
  STATE_TURN_LEFT
};

RobotState currState = STATE_FORWARD;

float distFront = 0;
float distLeft = 0;

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

int getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  float duration = pulseIn(echoPin, HIGH, 30000); 
  
  if (duration == 0) return 999;
  
  return duration * 0.034 / 2;
}

void setup() {
  Serial.begin(9600);
  
  pinMode(TRIG_FRONT, OUTPUT);
  pinMode(ECHO_FRONT, INPUT);

  pinMode(DIR_RIGHT, OUTPUT);
  pinMode(SPEED_RIGHT, OUTPUT);
  pinMode(DIR_LEFT, OUTPUT);
  pinMode(SPEED_LEFT, OUTPUT);

  digitalWrite(DIR_LEFT, LEFT_FORWARD);
  digitalWrite(DIR_RIGHT, RIGHT_FORWARD);
  analogWrite(SPEED_LEFT, 150);
  analogWrite(SPEED_RIGHT, 150);
  delay(3000);
  analogWrite(SPEED_LEFT, 0);
  analogWrite(SPEED_RIGHT, 0);
}

void loop() {
  distFront = getDistance(TRIG_FRONT, ECHO_FRONT);
  distLeft = getDistance(TRIG_LEFT, ECHO_LEFT);

  switch (currState) {
    case STATE_FORWARD:
      forward(255);

      if (distFront <= THR) {
        currState = STATE_ROTATE_RIGHT;
        Serial.println("Switch state: FORWARD -> ROTATE_RIGHT");
      }
      else if (distLeft > WF) {
        currState = STATE_TURN_LEFT;
        Serial.println("Switch state: FORWARD -> TURN_LEFT");
      }
      else if (distLeft < WC) {
        currState = STATE_TURN_RIGHT;
        Serial.println("Switch state: FORWARD -> TURN_RIGHT");
      }

      break;

    case STATE_ROTATE_RIGHT:
      rotate_right(127);

      if (distFront > (THR + 10)) { 
        currState = STATE_FORWARD;
        Serial.println("Switch state: ROTATE_RIGHT -> FORWARD");
      }

      break;

    case STATE_TURN_LEFT:
      turn_left(127);

      if (distFront < THR) {
        currState = STATE_ROTATE_RIGHT;
        Serial.println("Switch state: TURN_LEFT -> ROTATE_RIGHT");
      }
      else if (distLeft <= WF) {
        currState = STATE_FORWARD;
        Serial.println("Switch state: TURN_LEFT -> FORWARD");
      }
      
      break;
    
    case STATE_TURN_RIGHT:
      turn_right(127);

      if (distFront < THR) {
        currState = STATE_ROTATE_RIGHT;
        Serial.println("Switch state: TURN_RIGHT -> ROTATE_RIGHT");
      }
      else if (distLeft > WC) {
        currState = STATE_FORWARD;
        Serial.println("Switch state: TURN_RIGHT -> FORWARD");
      }

      break;
  }

  delay(10);
}
