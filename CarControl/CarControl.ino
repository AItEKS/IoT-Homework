#define DIR_LEFT 7
#define SPEED_LEFT 6

#define DIR_RIGHT 4
#define SPEED_RIGHT 5

#define TRIG_FRONT 8
#define ECHO_FRONT 9

#define TRIG_LEFT 10
#define ECHO_LEFT 11

#define LEFT_FORWARD LOW
#define LEFT_BACKWARD HIGH

#define RIGHT_FORWARD LOW
#define RIGHT_BACKWARD HIGH

#define THR 12

#define WF 9
#define WC 5

#define SPEED_STRAIGHT   50
#define SPEED_TURN_OUTER 55
#define SPEED_TURN_INNER 45
#define SPEED_ROTATE     45

enum RobotState {
  STATE_FORWARD,
  STATE_ROTATE_RIGHT,
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

void forward() {
  move(LEFT_FORWARD, SPEED_STRAIGHT, RIGHT_FORWARD, SPEED_STRAIGHT);
}

void turn_left() {
  move(LEFT_FORWARD, SPEED_TURN_OUTER, RIGHT_FORWARD, SPEED_TURN_INNER);
}

void turn_right() {
  move(LEFT_FORWARD, SPEED_TURN_INNER, RIGHT_FORWARD, SPEED_TURN_OUTER);
}

void rotate_right() {
  move(LEFT_BACKWARD, SPEED_ROTATE, RIGHT_FORWARD, SPEED_ROTATE);
}

float getDistance(int trigPin, int echoPin, float lastVal) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  unsigned long duration = pulseIn(echoPin, HIGH, 15000); 
  
  if (duration == 0) {
    return 100.0;
  }
  
  float newDist = duration * 0.034 / 2.0;
  
  if (lastVal == 100.0) return newDist;
  
  return (newDist * 0.4) + (lastVal * 0.6);
}

void setup() {
  Serial.begin(9600);
  
  pinMode(TRIG_FRONT, OUTPUT);
  pinMode(ECHO_FRONT, INPUT);
  
  pinMode(TRIG_LEFT, OUTPUT);
  pinMode(ECHO_LEFT, INPUT);

  pinMode(DIR_RIGHT, OUTPUT);
  pinMode(SPEED_RIGHT, OUTPUT);
  pinMode(DIR_LEFT, OUTPUT);
  pinMode(SPEED_LEFT, OUTPUT);
  
  delay(500);
  distFront = getDistance(TRIG_FRONT, ECHO_FRONT, 20);
  distLeft = getDistance(TRIG_LEFT, ECHO_LEFT, 10);
}

void loop() {
  distFront = getDistance(TRIG_FRONT, ECHO_FRONT, distFront);
  distLeft = getDistance(TRIG_LEFT, ECHO_LEFT, distLeft);

  switch (currState) {
    case STATE_FORWARD:
      forward();

      if (distFront <= THR) {
        currState = STATE_ROTATE_RIGHT;
        Serial.println("-> ROTATE_RIGHT");
      }
      else if (distLeft > WF) {
        currState = STATE_TURN_LEFT;
        Serial.println("-> TURN_LEFT");
      }
      else if (distLeft < WC) {
        currState = STATE_TURN_RIGHT;
        Serial.println("-> TURN_RIGHT");
      }
      break;

    case STATE_ROTATE_RIGHT:
      rotate_right();

      if (distFront > (THR + 15)) { 
        currState = STATE_FORWARD;
        Serial.println("-> FORWARD");
      }
      break;

    case STATE_TURN_LEFT:
      turn_left();

      if (distFront <= THR) {
        currState = STATE_ROTATE_RIGHT;
        Serial.println("-> ROTATE_RIGHT");
      }
      else if (distLeft <= WF && distLeft >= WC) {
        currState = STATE_FORWARD;
        Serial.println("-> FORWARD");
      }
      break;
    
    case STATE_TURN_RIGHT:
      turn_right();

      if (distFront <= THR) {
        currState = STATE_ROTATE_RIGHT;
        Serial.println("-> ROTATE_RIGHT");
      }
      else if (distLeft >= WC && distLeft <= WF) {
        currState = STATE_FORWARD;
        Serial.println("-> FORWARD");
      }
      break;
  }

  delay(10);
}