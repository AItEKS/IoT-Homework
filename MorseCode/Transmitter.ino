volatile char txBuffer[64];
volatile uint8_t txHead = 0;
volatile uint8_t txTail = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  while (Serial.available()) {
    char c = Serial.read();
    
    if (c == '.' || c == '-' || c == ' ') {
      uint8_t next = (txHead + 1) % 64;
      if (next != txTail) {
        txBuffer[txHead] = c;
        txHead = next;
      }
    }
  }
  
  while (txHead != txTail) {
    char c = txBuffer[txTail];
    txTail = (txTail + 1) % 64;
    
    Serial.print(c);
    delay(50);
  }
}
