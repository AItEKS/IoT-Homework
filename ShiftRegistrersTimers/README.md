# Управление семисегментным индикатором через каскадное соединение сдвиговых регистров

## Первый этап
### Аппаратная часть
Была собрана схема из двух сдвиговых регистров и двух семисегментных индикаторов.

![alt text](image.png)

### Программная часть
Было сделано прерывание по таймеру в 1 секунду. 

```cpp
  cli();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  
  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << CS12);
  OCR1A = 62499;
  TIMSK1 |= (1 << OCIE1A);
  sei();
```

Но на данном этапе пока не реилзован `delay()` и не убран `digitalWrite()`.

[Tinkercad](https://www.tinkercad.com/things/kmnLW25u2nl-copy-of-cascadingregisters/editel?returnTo=https%3A%2F%2Fwww.tinkercad.com%2Fdashboard%2Fdesigns%2Fall)