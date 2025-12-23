#define DATA_PIN   7
#define CLOCK_PIN  3
#define LATCH_PIN  5

#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1

volatile char morseBuffer[8];
volatile uint8_t morseIndex = 0;
volatile bool decodeFlag = false;

const uint8_t digits[] = {
  0b11111100, 0b01100000, 0b11011010, 0b11110010, 0b01100110,
  0b10110110, 0b10111110, 0b11100000, 0b11111110, 0b11110110
};

const uint8_t letters[26] = {
  0b11101110, 0b00111110, 0b10011100, 0b01111010, 0b10011110, // A-E
  0b10001110, 0b10111100, 0b01101110, 0b00001100, 0b01110000, // F-J
  0b01101110, 0b00011100, 0b11101100, 0b00101010, 0b11111100, // K-O
  0b11001110, 0b11100110, 0b00001010, 0b10110110, 0b00011110, // P-T
  0b01111100, 0b01111100, 0b01111100, 0b01101110, 0b01110110, // U-Y 
  0b11011010  // Z
};

struct MorseEntry {
  const char* code;
  char letter;
};

const MorseEntry morseTable[] = {
  {".-", 'A'},     {"-...", 'B'},   {"-.-.", 'C'},   {"-..", 'D'},
  {".", 'E'},      {"..-.", 'F'},   {"--.", 'G'},    {"....", 'H'},
  {"..", 'I'},     {".---", 'J'},   {"-.-", 'K'},    {".-..", 'L'},
  {"--", 'M'},     {"-.", 'N'},     {"---", 'O'},    {".--.", 'P'},
  {"--.-", 'Q'},   {".-.", 'R'},    {"...", 'S'},    {"-", 'T'},
  {"..-", 'U'},    {"...-", 'V'},   {".--", 'W'},    {"-..-", 'X'},
  {"-.--", 'Y'},   {"--..", 'Z'},
  {"-----", '0'},  {".----", '1'},  {"..---", '2'},  {"...--", '3'},
  {"....-", '4'},  {".....", '5'},  {"-....", '6'},  {"--...", '7'},
  {"---..", '8'},  {"----.", '9'},
  {NULL, '?'}
};

void uart_init(unsigned int ubrr) {
  UBRR0H = (unsigned char)(ubrr >> 8);
  UBRR0L = (unsigned char)ubrr;
  
  UCSR0B = (1 << RXEN0) | (1 << RXCIE0);
  
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void setup() {
  
  uart_init(MYUBRR);
  
  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);
  
  displaySegments(0); 
  
  sei();
}

ISR(USART_RX_vect) {
  char c = UDR0; 
  
  if (c == '.' || c == '-') {
    if (morseIndex < 7) {
      morseBuffer[morseIndex++] = c;
      morseBuffer[morseIndex] = '\0';
    }
  } 
  else if (c == ' ') {
    decodeFlag = true; 
  }
}

void loop() {
  
  if (decodeFlag) {
    decodeFlag = false; 
    
    if (morseIndex > 0) {
      char letter = decodeMorse((const char*)morseBuffer);
      displayChar(letter);
      
      morseIndex = 0;
      morseBuffer[0] = '\0';
    }
  }
}

char decodeMorse(const char* code) {
  for (int i = 0; morseTable[i].code != NULL; i++) {
    if (strcmp(code, morseTable[i].code) == 0) {
      return morseTable[i].letter;
    }
  }
  return '?'; 
}

void displaySegments(uint8_t data) {
  digitalWrite(LATCH_PIN, LOW);
  
  for (int i = 7; i >= 0; i--) {
    digitalWrite(CLOCK_PIN, LOW);
    digitalWrite(DATA_PIN, (data >> i) & 1);
    digitalWrite(CLOCK_PIN, HIGH);
  }
  
  digitalWrite(LATCH_PIN, HIGH);
}



void displayChar(char c) {
  uint8_t seg = 0;
  
  if (c >= '0' && c <= '9') {
    seg = digits[c - '0'];
  } 
  else if (c >= 'A' && c <= 'Z') {
    seg = letters[c - 'A'];
  }
  else if (c >= 'a' && c <= 'z') {
    seg = letters[c - 'a'];
  }
  else {
    seg = 0b00000010;  
  }
  
  displaySegments(seg);
}
