int latchPin = 5;      
int clockPin = 3;
int dataPin = 7;       

volatile long clockCount = 0; 

bool digits[10][8] = {
  {1,1,0,1,1,1,0,1},  // 0
  {0,1,0,1,0,0,0,0},  // 1
  {1,1,0,0,1,1,1,0},  // 2
  {1,1,0,1,1,0,1,0},  // 3
  {0,1,0,1,0,0,1,1},  // 4
  {1,0,0,1,1,0,1,1},  // 5
  {1,0,1,1,1,1,1,1},  // 6
  {1,1,0,1,0,0,0,0},  // 7
  {1,1,0,1,1,1,1,1},  // 8
  {1,1,1,1,1,0,1,1}   // 9
};

void setup() 
{
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);  
  pinMode(clockPin, OUTPUT);
  digitalWrite(clockPin, LOW);
  Serial.begin(9600);

  cli();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  
  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << CS12);
  OCR1A = 62499;
  TIMSK1 |= (1 << OCIE1A);
  
  sei();
}

ISR(TIMER1_COMPA_vect) {
  clockCount++;
  clockCount = clockCount % 60; 
}

void show_two_digits(int number) {
  if(number < 0 || number > 99) {
    return;
  }
  
  int tens = number / 10;
  int ones = number % 10;
  
  digitalWrite(latchPin, LOW);
  
  for(int i = 7; i >= 0; i--) {
    shift_bit(digits[tens][i]);
  }
  
  for(int i = 7; i >= 0; i--) {
    shift_bit(digits[ones][i]);
  }
  
  digitalWrite(latchPin, HIGH);
}

void shift_bit(bool val) {
  digitalWrite(dataPin, !val);
  digitalWrite(clockPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(clockPin, LOW);
}

void loop() 
{
  if (Serial.available()) {
      clockCount = Serial.parseInt();
  }

  show_two_digits(clockCount);
    
  Serial.print("Time: ");
  if(clockCount < 10) Serial.print("0");
  Serial.println(clockCount);
}
