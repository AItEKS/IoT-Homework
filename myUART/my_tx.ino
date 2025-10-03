#define F_CPU 16000000UL

#define TX_PORT PORTD
#define TX_DDR DDRD
#define TX_PIN PD3

#define TX_BUFFER_SIZE 64


volatile char tx_buffer[TX_BUFFER_SIZE];
volatile uint8_t tx_tail = 0;
volatile uint8_t tx_head = 0;
volatile uint8_t tx_byte_to_send;
volatile uint8_t tx_bit_index;

volatile uint16_t timer_ticks_per_bit;

// void uart_set_baudrate(int rate) {
//   int Tbit = 1 / rate;
// }

void uart_init(uint16_t baud_rate) {
  const uint8_t prescaler = 8;
  timer_ticks_per_bit = (F_CPU / prescaler) / baud_rate;

  TX_DDR = TX_DDR | (1 << TX_PIN);
  TX_PORT = TX_PORT | (1 << TX_PIN); // по дефолту устанавливаем HIGH-сигнал на пин

  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  OCR1A = timer_ticks_per_bit; // сравниваем с этим значением
  TCCR1B = TCCR1B | (1 << WGM12);
  TCCR1B = TCCR1B | (1 << CS11); // для прескалера 8

  sei();
}

void uart_send(char b) {
  uint8_t next_head = (tx_head + 1) % TX_BUFFER_SIZE;

  while (next_head == tx_tail) {}

  tx_buffer[tx_head] = b;
  tx_head = next_head;

  cli();
  if ((TIMSK1 & (1 << OCIE1A)) == 0) {
    tx_byte_to_send = tx_buffer[tx_tail];
    tx_tail = (tx_tail + 1) % TX_BUFFER_SIZE;
    tx_bit_index = 0;
    TCNT1 = 0;
    TIMSK1 = TIMSK1 | (1 << OCIE1A);
  }
  sei();
}

void uart_send_string(const char *msg) {
  while (*msg) {
    uart_send(*msg++);
  }
}

ISR(TIMER1_COMPA_vect) {
  switch (tx_bit_index) {
    case 0:
      TX_PORT = TX_PORT & ~(1 << TX_PIN);
      break;
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
      if (tx_byte_to_send & 1) {
        TX_PORT = TX_PORT | (1 << TX_PIN); // HIGH-сигнал
      } else {
        TX_PORT = TX_PORT & ~(1 << TX_PIN); // LOW-сигнал
      }
      tx_byte_to_send = tx_byte_to_send >> 1;
      break;
    case 9:
      TX_PORT = TX_PORT | (1 << TX_PIN);
      break;
    default:
      if (tx_head != tx_tail) {
        tx_byte_to_send = tx_buffer[tx_tail];
        tx_tail = (tx_tail + 1) % TX_BUFFER_SIZE;
        tx_bit_index = 0;
        return;
      } else {
        TIMSK1 = TIMSK1 & ~(1 << OCIE1A);
      }
      break;
  }
  tx_bit_index++;
}


void setup() {
  uart_init(9600);
}


void loop() {
  uart_send_string("Hello World! 12345\r\n");

  for (volatile uint64_t i = 0; i < 200000; ++i) {} //вместо delay 
}
