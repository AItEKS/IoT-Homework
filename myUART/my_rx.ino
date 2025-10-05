#define F_CPU 16000000UL

#define RX_PORT PORTD
#define RX_DDR DDRD
#define RX_PIN PD2
#define RX_PIN_REG  PIND

#define RX_BUFFER_SIZE 64


volatile char rx_buffer[RX_BUFFER_SIZE];
volatile uint8_t rx_head = 0;
volatile uint8_t rx_tail = 0;
volatile uint8_t rx_byte_received;
volatile uint8_t rx_bit_index;

volatile uint16_t timer_ticks_per_bit;


void uart_init(long baud_rate) {
  const uint8_t prescaler = 8;
  timer_ticks_per_bit = (F_CPU / prescaler) / baud_rate;

  RX_DDR = RX_DDR & ~(1 << RX_PIN);
  RX_PORT = RX_PORT | (1 << RX_PIN);

  EICRA = EICRA | (1 << ISC01);
  EICRA = EICRA & ~(1 << ISC00);
  EIMSK = EIMSK | (1 << INT0);

  TCCR1A = 0;
  TCCR1B = 0;

  TCCR1B = TCCR1B | (1 << WGM12);
  TCCR1B = TCCR1B | (1 << CS11);

  sei();
}

uint8_t uart_available() {
  return (rx_head - rx_tail + RX_BUFFER_SIZE) % RX_BUFFER_SIZE;
}

char uart_read() {
  if (rx_head == rx_tail) {
    return -1;
  }
  char b = rx_buffer[rx_tail];
  rx_tail = (rx_tail + 1) % RX_BUFFER_SIZE;
  return b;
}

// случилось прерывание - с LOW -> HIGH
ISR(INT0_vect) {
    EIMSK = EIMSK & ~(1 << INT0); // отключаем это прерывание 
    
    TCNT1 = 0; // сбрасываем таймер 
    OCR1B = timer_ticks_per_bit + (timer_ticks_per_bit / 2); // сравнение устанавливаем на 1.5*Tbit

    rx_bit_index = 0;
    rx_byte_received = 0;

    TIMSK1 = TIMSK1 | (1 << OCIE1B); // включаем прервание по совпадению таймера
}

// прерывание на сраванение
ISR(TIMER1_COMPB_vect) {
    OCR1B += timer_ticks_per_bit; // настравиваем следующее прерывание

    if (rx_bit_index < 8) { // считываем 8 бит данных
        rx_byte_received = rx_byte_received >> 1;
        if (RX_PIN_REG & (1 << RX_PIN)) {
            rx_byte_received = rx_byte_received | 0x80; // устанавливаем в верном порядке биты.
        }
    } else { // это стоп-бит
        uint8_t next_head = (rx_head + 1) % RX_BUFFER_SIZE;
        if (next_head != rx_tail) {
            rx_buffer[rx_head] = rx_byte_received;
            rx_head = next_head;
        }

        TIMSK1 = TIMSK1 & ~(1 << OCIE1B); // выключаем прервание по времени
        EIMSK = EIMSK | (1 << INT0); // включаем прервание на передачу в порт
    }
    rx_bit_index++;
}

void setup() {
  uart_init(9600);
  
  //Для теста
  Serial.begin(9600);
}

void loop() {
  if (uart_available()) {
    char c = uart_read();

    Serial.print(c);
  }
}
