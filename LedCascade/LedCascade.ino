volatile unsigned int val = 0;
const int pinA = 7;
const int pinB = 6;
const int pinC = 5;
const int pinD = 4;
const int pinE = 3;


void setup() {
  cli();

  DDRD = DDRD | (1 << pinA) | (1 << pinB) | (1 << pinC) | (1 << pinD) | (1 << pinE);

  OCR1A = 62500;
  TCCR1A = 0;
  TCCR1B = 0;
  
  TCCR1B = TCCR1B | (1 << WGM12);
  TCCR1B = TCCR1B | (1 << CS12);
  TIMSK1 = TIMSK1 | (1 << OCIE1A);

  sei();
}

void my_digit_write() {
  val++;

  int x = 1000; 
  bool first = val % x == 0;
  bool second = val % (x * 2) == 0;
  bool third = val % (x * 3) == 0;
  bool fourth = val % (x * 4) == 0;
  bool fifth = val % (x * 5) == 0;

  PORTD = PORTD ^ (first << pinA);
  PORTD = PORTD ^ (second << pinB);
  PORTD = PORTD ^ (third << pinC);
  PORTD = PORTD ^ (fourth << pinD);
  PORTD = PORTD ^ (fifth << pinE);
}

ISR(TIMER1_COMPA_vect) {
  my_digit_write();
}


void loop() {
  // put your main code here, to run repeatedly:
}
