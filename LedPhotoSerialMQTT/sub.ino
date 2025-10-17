const int led_pin = 7;

const char LED_ON = 'u';
const char LED_OFF = 'd';
const char LED_BLINK = 'b';

char LED_state;

void setup() {
  pinMode(led_pin, OUTPUT);
  Serial.begin(9600);
}

void handleLED() {
  if (LED_state == LED_ON) {
    digitalWrite(led_pin, HIGH);
  } else if (LED_state == LED_OFF) {
    digitalWrite(led_pin, LOW);
  } else if (LED_state == LED_BLINK) {
    digitalWrite(led_pin, HIGH);
    delay(500);
    digitalWrite(led_pin, LOW);
    delay(500);
  }
}

void updateState() {
  if (Serial.available()>0)
  { 
     LED_state = Serial.read();
    if (LED_state == LED_ON) {
      Serial.println("LED_GOES_ON");
    } else if (LED_state == LED_OFF) {
      Serial.println("LED_GOES_OFF");
    } else if (LED_state == LED_BLINK) {
      Serial.println("LED_GOES_BLINK");
    }
  }
}

void loop() {
  updateState();
  handleLED();
}
