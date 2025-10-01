bool state = true;
uint8_t ofv = 0; 

const uint8_t OVERFLOWS_PER_TOGGLE = 31; 

volatile uint8_t current_mode = 0;

const unsigned long BLINK_INTERVAL_A = 1000;
const unsigned long BLINK_INTERVAL_B = 50;

unsigned long previous_millis = 0;
bool millis_state = LOW; 

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  Serial.println("Commands: 0, 1, 2.");

  cli();
  TCCR2A = 0;
  TCCR2B = 0;

  TCCR2B = TCCR2B | (1 << CS22) | (1 << CS21) | (1 << CS20);
  TIMSK2 = TIMSK2 | (1 << TOIE2);
  sei();
}

ISR(TIMER2_OVF_vect) {
  ofv++;
  if (ofv >= OVERFLOWS_PER_TOGGLE) {
    ofv = 0;
    state = !state;
  }
}

void blink_millis_A() {
  unsigned long current_millis = millis();
  
  if (current_millis - previous_millis >= BLINK_INTERVAL_A) {
    previous_millis = current_millis;
    millis_state = !millis_state;
  }
  digitalWrite(LED_BUILTIN, millis_state);
}

//BAD!!!
void blink_millis_B() {
  digitalWrite(LED_BUILTIN, millis_state);
  delay(BLINK_INTERVAL_B);
  millis_state = !millis_state;
  digitalWrite(LED_BUILTIN, millis_state);
  delay(BLINK_INTERVAL_B);
}

void loop() {
  if (Serial.available() > 0) {
    char inCmd = Serial.read();

    switch (inCmd) {
      case '0':
        current_mode = 0;
        Serial.println("Switched to Mode 0 (ISR Blink)");
        break;
      case '1':
        current_mode = 1;
        previous_millis = millis(); 
        Serial.println("Switched to Mode 1 (Millis Blink)");
        break;
      case '2':
        current_mode = 2; 
        Serial.println("Switched to Mode 2 (Delaaay)");
        break;
    }
  }

  switch (current_mode) {
    case 0:
      digitalWrite(LED_BUILTIN, state); 
      break;
    case 1:
      blink_millis_A();
      break;
    case 2:
      blink_millis_B();
      break;
  }
}