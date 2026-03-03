#define DIR_1 4
#define SPEED_1 5

#define DIR_2 7
#define SPEED_2 6

void setup() {
  pinMode(DIR_1, OUTPUT);
  pinMode(SPEED_1, OUTPUT);
  pinMode(DIR_2, OUTPUT);
  pinMode(SPEED_2, OUTPUT);

}

void loop() {
  digitalWrite(DIR_1, LOW);
  digitalWrite(DIR_2, HIGH);
  digitalWrite(SPEED_1, LOW);
  digitalWrite(SPEED_2, HIGH);
}
