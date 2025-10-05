# Реализация асинхронной последовательной передачи данных (UART) 

## Задача 1: TX - передача

**Гипотеза решения:** ​Данные для отправки помещаются в программный буфер (по байтам). Передатчик делит байты на биты и при каждом срабатывании таймера изменяет состояние вывода TX в соответствии со следующим битом. Таймер задаёт точную длительность бита. Поддерживаются стартовый и стоповый биты (формат 8N1).

### Программная реализация: анализ ключевых моментов кода
```cpp
#define F_CPU 16000000UL

// Настройка портов
#define TX_PORT PORTD
#define TX_DDR DDRD
#define TX_PIN PD3

// Размер буфера
#define TX_BUFFER_SIZE 64

// Для работы с буферо и отправленным байтом
volatile char tx_buffer[TX_BUFFER_SIZE];
volatile uint8_t tx_tail = 0;
volatile uint8_t tx_head = 0;
volatile uint8_t tx_byte_to_send;
volatile uint8_t tx_bit_index;

volatile uint16_t timer_ticks_per_bit;
```
Объявление основных переменных и констант.


```cpp
void uart_init(uint16_t baud_rate) {
  const uint8_t prescaler = 8;
  timer_ticks_per_bit = (F_CPU / prescaler) / baud_rate;

  TX_DDR = TX_DDR | (1 << TX_PIN);
  TX_PORT = TX_PORT | (1 << TX_PIN); // по дефолту устанавливаем HIGH-сигнал на пин

  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  OCR1A = timer_ticks_per_bit; // настраиваем сравнение для прерывания по времени
  TCCR1B = TCCR1B | (1 << WGM12);
  TCCR1B = TCCR1B | (1 << CS11); // для prescaler=8

  sei();
}
```
В функции `void uart_init(uint16_t baud_rate)` происходит вся начальная конфигурация. Настраиваем здесь порт отдачи сигнала(по дефолту ставим `HIGH` на пин), 16-битный таймер `Timer 1` и сравнение `OCR1A` для прерывания.

![alt text](image.png)

```cpp
void uart_send(char b) {
  uint8_t next_head = (tx_head + 1) % TX_BUFFER_SIZE;

  while (next_head == tx_tail) {}

  tx_buffer[tx_head] = b;
  tx_head = next_head;

  cli();
  // если прерывание для отправки битов выключено
  if ((TIMSK1 & (1 << OCIE1A)) == 0) {
    tx_byte_to_send = tx_buffer[tx_tail];
    tx_tail = (tx_tail + 1) % TX_BUFFER_SIZE;
    tx_bit_index = 0;
    TCNT1 = 0; // обнуление таймера
    TIMSK1 = TIMSK1 | (1 << OCIE1A); // включить прерывание 
  }
  sei();
}
```
Функция `void uart_send(char b)` отвечает за логику отправки одного байта(символа). Здесь реализована идея кольцевого буфера - из буфера записываем в глобальную переменную `tx_byte_to_send` помещаем байт на отправку и сдвигаем индексы `tx_tail` и `tx_head`.

```cpp
void uart_send_string(const char *msg) {
  while (*msg) {
    uart_send(*msg++);
  }
}
```
Функция `void uart_send_string(const char *msg)` отправляет полное сообщение, используя цикл и реализованную функцию отправки одного байта `void uart_send(char b)`.

```cpp
ISR(TIMER1_COMPA_vect) {
  switch (tx_bit_index) {
    case 0:
      TX_PORT = TX_PORT & ~(1 << TX_PIN);
      break;
    case 1:
    ...
    ...
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
```
`ISR(TIMER1_COMPA_tx_bit_indexvect)` - прерывание по сравнению. Проверяет текущий `tx_bit_index`: 
1. При значении 0 устанавливаем `LOW` сигнал в `TX_PORT`, тем самым обозначая передачу байта
2. При значении от 1-8 передаем `HIGH` и `LOW` сигнал в соответствии с 1 и 0.
3. При значении 9 передаём `HIGH` сигнал, тем самым обозначая конец передачи байта.
4. `default` - повторение отправки если `tx_head != tx_tail`(значит есть, что отправлять), а иначе отключаем прерывание.
```cpp
void setup() {
  uart_init(9600);
}


void loop() {
  uart_send_string("Hello World! 12345\r\n");

  for (volatile uint64_t i = 0; i < 200000; ++i) {} //вместо delay 
}
```

Настройка собственного `TX` и тестирование с искусственной задержкой.

### Демострация работы TX

![Демонстрация работы TX](my_tx.mp4)