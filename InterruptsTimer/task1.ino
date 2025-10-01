volatile bool state = true;
volatile uint8_t ofv = 0; 

// 8 битный таймер
// Частота тиков таймера: F_timer = F_CPU / Prescaler = 16 000 000 / 1024 = 15 625 Гц.
// Время одного тика: T_tick = 1 / 15 625 = 0.000064 с (64 микросекунды).
// Максимальный период одного прерывания: T_overflow = T_tick * 256 = 0.000064 * 256 = 0.016384 с (или ~16.4 мс).
// Чтобы мигать раз в секунду (период 500 мс), нам нужно ~30 прерываний.
// 500 мс / 16.384 мс ≈ 30.5
const uint8_t OVERFLOWS_PER_TOGGLE = 31; 

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  
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

void loop() {
  digitalWrite(LED_BUILTIN, state);
}
