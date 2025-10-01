volatile bool state = false;

uint8_t ofv = 0; 

// (16 МГц / 1024) / 256 = 15625 / 256 ≈ 61 Гц. Период ~16.4 мс.
// Чтобы получить интервал 250 мс, нам нужно:
// 250 мс / 16.384 мс ≈ 15.25
// Округляем до 15.
const uint8_t OVERFLOWS_PER_READ = 15; 

void setup() {
  Serial.begin(9600);
  
  cli();
  TCCR2A = 0;
  TCCR2B = 0;

  TCCR2B = TCCR2B | (1 << CS22) | (1 << CS21) | (1 << CS20);
  
  TIMSK2 = TIMSK2 | (1 << TOIE2);
  
  sei(); 
}

ISR(TIMER2_OVF_vect) {
  ofv++;
  if (ofv >= OVERFLOWS_PER_READ) {
    ofv = 0;
    state = !state;
  }
}

void loop() {
    if (state) {
      state = false; 
    
      int sensorValue = analogRead(A0);
      Serial.print("Value on pin A0: ");
      Serial.println(sensorValue);
  }
}