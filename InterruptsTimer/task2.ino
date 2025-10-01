volatile unsigned long total_millis_count = 0; //эт нужно чтобы переменная читалась не из кэша, а из оперативки непосредственно
const unsigned long f_cpu = 16000000;
const int prescaler = 64;
const int my_hz = 1000;

//Частота прерывания=Тактовая частота МК​ / ((Предделитель)×(OCR2A+1))
const int ocr = f_cpu / prescaler / my_hz - 1;

void setup() {
  Serial.begin(9600);

  OCR2A = ocr;

  TCCR2A = 0;
  TCCR2B = 0;

  TCCR2A = TCCR2A | (1 << WGM21);

  TCCR2B = TCCR2B | (1 << CS22);

  TIMSK2 = TIMSK2 | (1 << OCIE2A);

  sei();
}

unsigned long my_millis() {
  unsigned long m;

  uint8_t oldSREG = SREG; 
  cli();

  m = total_millis_count;

  SREG = oldSREG;

  return m;
}

ISR(TIMER2_COMPA_vect) {
  total_millis_count++;
}

void loop() {
  Serial.print("Millis diff = ");
  Serial.println(my_millis()-millis());
  
  delay(100);
}