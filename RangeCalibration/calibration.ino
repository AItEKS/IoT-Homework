#define TRIG_PIN 5   
#define ECHO_PIN 4  
#define IR_PIN A0    

bool isRunning = false;
bool isFinished = false;

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
}

long getUltrasonicDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH);
  return duration * 0.034 / 2;
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command.startsWith("START")) {
        isFinished = false;
        isRunning = true;
        digitalWrite(LED_BUILTIN, HIGH);
    } 
    else if (command == "STOP") {
       isRunning = false;
       isFinished = true;
       digitalWrite(LED_BUILTIN, LOW);
    }
  }

  if (isRunning) {
    long dist = getUltrasonicDistance();
    int irVal = analogRead(IR_PIN);

    Serial.print(dist);
    Serial.print(" ");
    Serial.println(irVal);
    
    delay(50);
  }

  if (isFinished) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
  }
}