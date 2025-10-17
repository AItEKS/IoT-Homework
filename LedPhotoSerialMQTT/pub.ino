int SENSOR_PIN = A0;
bool is_stream = false;

void sendSensorValue() {
  int sensorValue = analogRead(SENSOR_PIN);
  Serial.print("SENSOR_VALUE:"); 
  Serial.println(sensorValue);
}

void setup() {
  pinMode(SENSOR_PIN, INPUT);
  Serial.begin(9600);

}

void loop() {
  if (Serial.available() > 0) {
    char command = Serial.read();
    
    if (command == 'p') {
      is_stream = false;
      sendSensorValue();
    } else if (command == 's') {
      Serial.println("STREAM_STARTED");
      is_stream = true;
    }
  }

  if (is_stream) {
    sendSensorValue();
    delay(100);
  }
}
