#include <EEPROM.h>
#include <SoftwareSerial.h>

// === Пины ===
#define DIR_RIGHT   4
#define SPEED_RIGHT 5
#define SPEED_LEFT  6
#define DIR_LEFT    7

SoftwareSerial bt(10, 11);

// === Настройки ===
#define BASE_SPEED   200
#define DEFAULT_T90  400
#define DEFAULT_T180 800
#define DEFAULT_T270 1200
#define DEFAULT_T360 1600

// === EEPROM ===
#define ADDR_VALID   0
#define ADDR_FWD_L   1
#define ADDR_FWD_R   2
#define ADDR_BWD_L   3
#define ADDR_BWD_R   4
#define ADDR_ROTL_L  5
#define ADDR_ROTL_R  6
#define ADDR_ROTR_L  7
#define ADDR_ROTR_R  8
#define ADDR_SPD_L   9
#define ADDR_SPD_R   10
#define ADDR_T90     11
#define EEPROM_MAGIC 0xAB

const bool DIR_COMBOS[4][2] = {
  {LOW,  LOW },
  {HIGH, LOW },
  {LOW,  HIGH},
  {HIGH, HIGH}
};

enum Mode   { DIR_CAL, SPEED_BAL, TURN_TIME, RUN };
enum Action { ACT_FWD = 0, ACT_BWD = 1, ACT_ROTL = 2, ACT_ROTR = 3 };

bool     cal_left_dir[4]  = { LOW, HIGH, HIGH, LOW };
bool     cal_right_dir[4] = { LOW, HIGH, LOW, HIGH };
int      speed_left       = BASE_SPEED;
int      speed_right      = BASE_SPEED;
uint16_t turn_ms[4]       = { DEFAULT_T90, DEFAULT_T180, DEFAULT_T270, DEFAULT_T360 };

Mode    current_mode   = DIR_CAL;
Action  current_action = ACT_FWD;
uint8_t combo_idx      = 0;
uint8_t turn_idx       = 0;

// === Движение ===
void move_raw(bool ld, int ls, bool rd, int rs) {
  digitalWrite(DIR_LEFT,   ld);
  digitalWrite(DIR_RIGHT,  rd);
  analogWrite(SPEED_LEFT,  ls);
  analogWrite(SPEED_RIGHT, rs);
}

void stop_car() { move_raw(LOW, 0, LOW, 0); }

void do_action(Action a) {
  move_raw(cal_left_dir[a], speed_left, cal_right_dir[a], speed_right);
}

// === EEPROM ===
void eeprom_save() {
  EEPROM.write(ADDR_FWD_L,  (uint8_t)cal_left_dir[ACT_FWD]);
  EEPROM.write(ADDR_FWD_R,  (uint8_t)cal_right_dir[ACT_FWD]);
  EEPROM.write(ADDR_BWD_L,  (uint8_t)cal_left_dir[ACT_BWD]);
  EEPROM.write(ADDR_BWD_R,  (uint8_t)cal_right_dir[ACT_BWD]);
  EEPROM.write(ADDR_ROTL_L, (uint8_t)cal_left_dir[ACT_ROTL]);
  EEPROM.write(ADDR_ROTL_R, (uint8_t)cal_right_dir[ACT_ROTL]);
  EEPROM.write(ADDR_ROTR_L, (uint8_t)cal_left_dir[ACT_ROTR]);
  EEPROM.write(ADDR_ROTR_R, (uint8_t)cal_right_dir[ACT_ROTR]);
  EEPROM.write(ADDR_SPD_L,  (uint8_t)speed_left);
  EEPROM.write(ADDR_SPD_R,  (uint8_t)speed_right);
  for (int i = 0; i < 4; i++) {
    int a = ADDR_T90 + i * 2;
    EEPROM.write(a,     (uint8_t)(turn_ms[i] >> 8));
    EEPROM.write(a + 1, (uint8_t)(turn_ms[i] & 0xFF));
  }
  EEPROM.write(ADDR_VALID, EEPROM_MAGIC);
}

void eeprom_load() {
  cal_left_dir[ACT_FWD]   = EEPROM.read(ADDR_FWD_L);
  cal_right_dir[ACT_FWD]  = EEPROM.read(ADDR_FWD_R);
  cal_left_dir[ACT_BWD]   = EEPROM.read(ADDR_BWD_L);
  cal_right_dir[ACT_BWD]  = EEPROM.read(ADDR_BWD_R);
  cal_left_dir[ACT_ROTL]  = EEPROM.read(ADDR_ROTL_L);
  cal_right_dir[ACT_ROTL] = EEPROM.read(ADDR_ROTL_R);
  cal_left_dir[ACT_ROTR]  = EEPROM.read(ADDR_ROTR_L);
  cal_right_dir[ACT_ROTR] = EEPROM.read(ADDR_ROTR_R);
  speed_left  = EEPROM.read(ADDR_SPD_L);
  speed_right = EEPROM.read(ADDR_SPD_R);
  for (int i = 0; i < 4; i++) {
    int a = ADDR_T90 + i * 2;
    turn_ms[i] = ((uint16_t)EEPROM.read(a) << 8) | EEPROM.read(a + 1);
  }
}

// === Логи ===
const char* action_name(Action a) {
  switch(a) {
    case ACT_FWD:  return "FORWARD";
    case ACT_BWD:  return "BACKWARD";
    case ACT_ROTL: return "ROTATE_LEFT";
    case ACT_ROTR: return "ROTATE_RIGHT";
  }
  return "?";
}

void print_status() {
  switch (current_mode) {
    case DIR_CAL:
      Serial.print(F("DIR_CAL | action="));
      Serial.print(action_name(current_action));
      Serial.print(F(" | combo="));
      Serial.print(combo_idx);
      Serial.print(F(" L="));
      Serial.print(DIR_COMBOS[combo_idx][0]);
      Serial.print(F(" R="));
      Serial.println(DIR_COMBOS[combo_idx][1]);
      break;
    case SPEED_BAL:
      Serial.print(F("SPEED_BAL | L="));
      Serial.print(speed_left);
      Serial.print(F(" R="));
      Serial.println(speed_right);
      break;
    case TURN_TIME:
      Serial.print(F("TURN_TIME | angle="));
      Serial.print(turn_idx * 90 + 90);
      Serial.print(F("° | ms="));
      Serial.println(turn_ms[turn_idx]);
      break;
    case RUN:
      Serial.println(F("RUN"));
      break;
  }
}

// === Обработка команд ===
void handle_command(char c) {
  switch (current_mode) {

    // --------------------------------------------------
    // РЕЖИМ 1: калибровка направлений
    // F=запустить  B=стоп  T=следующая комбинация
    // START(A)=подтвердить  R=вперёд  P=назад  X=далее
    // --------------------------------------------------
    case DIR_CAL:
      if (c == 'F') {
        move_raw(DIR_COMBOS[combo_idx][0], speed_left,
                 DIR_COMBOS[combo_idx][1], speed_right);
      }
      else if (c == 'B') { stop_car(); }
      else if (c == 'T') {                          // △ — следующая комбинация DIR
        stop_car();
        combo_idx = (combo_idx + 1) % 4;
        move_raw(DIR_COMBOS[combo_idx][0], speed_left,
                 DIR_COMBOS[combo_idx][1], speed_right);
      }
      else if (c == 'A') {                          // START — подтвердить
        cal_left_dir[current_action]  = DIR_COMBOS[combo_idx][0];
        cal_right_dir[current_action] = DIR_COMBOS[combo_idx][1];
        stop_car();
        if (current_action < ACT_ROTR) {
          current_action = (Action)(current_action + 1);
          combo_idx = 0;
        }
      }
      else if (c == 'R') {                          // ▶ — следующее действие
        stop_car();
        if (current_action < ACT_ROTR) { current_action = (Action)(current_action + 1); combo_idx = 0; }
      }
      else if (c == 'P') {                          // PAUSE — предыдущее действие
        stop_car();
        if (current_action > ACT_FWD)  { current_action = (Action)(current_action - 1); combo_idx = 0; }
      }
      else if (c == 'X') { stop_car(); current_mode = SPEED_BAL; }
      break;

    // --------------------------------------------------
    // РЕЖИМ 2: балансировка скоростей
    // F=вперёд  B=стоп
    // R=право+  L=право-  C=лево+  T=лево-  S=сброс
    // X=далее
    // --------------------------------------------------
    case SPEED_BAL:
      if      (c == 'F') { do_action(ACT_FWD); }
      else if (c == 'B') { stop_car(); }
      else if (c == 'R') { speed_right = constrain(speed_right + 5, 0, 255); }
      else if (c == 'L') { speed_right = constrain(speed_right - 5, 0, 255); }
      else if (c == 'C') { speed_left  = constrain(speed_left  + 5, 0, 255); } // ○ — лево+
      else if (c == 'T') { speed_left  = constrain(speed_left  - 5, 0, 255); } // △ — лево-
      else if (c == 'S') { speed_left = speed_right = BASE_SPEED; }
      else if (c == 'X') { stop_car(); current_mode = TURN_TIME; turn_idx = 0; }
      break;

    // --------------------------------------------------
    // РЕЖИМ 3: время поворотов
    // F=тест поворота  R=+50мс  L=-50мс
    // START(A)=следующий угол  S=сброс  X=сохранить
    // --------------------------------------------------
    case TURN_TIME: {
      const uint16_t DEF[4] = { DEFAULT_T90, DEFAULT_T180, DEFAULT_T270, DEFAULT_T360 };
      if (c == 'F') {
        do_action(ACT_ROTL);
        delay(turn_ms[turn_idx]);
        stop_car();
      }
      else if (c == 'R') { turn_ms[turn_idx] = constrain((int)turn_ms[turn_idx] + 50, 50, 9999); }
      else if (c == 'L') { turn_ms[turn_idx] = constrain((int)turn_ms[turn_idx] - 50, 50, 9999); }
      else if (c == 'A') { if (turn_idx < 3) turn_idx++; }
      else if (c == 'S') { turn_ms[turn_idx] = DEF[turn_idx]; }
      else if (c == 'X') { stop_car(); eeprom_save(); current_mode = RUN; }
      break;
    }

    // --------------------------------------------------
    // РЕЖИМ 4: езда
    // F=вперёд  B=назад  L=влево  R=вправо
    // START(A)=стоп  X=сброс калибровки
    // --------------------------------------------------
    case RUN:
      if      (c == 'F') do_action(ACT_FWD);
      else if (c == 'B') do_action(ACT_BWD);
      else if (c == 'L') do_action(ACT_ROTL);
      else if (c == 'R') do_action(ACT_ROTR);
      else if (c == 'A') stop_car();
      else if (c == 'X') {
        stop_car();
        EEPROM.write(ADDR_VALID, 0x00);
        current_mode   = DIR_CAL;
        current_action = ACT_FWD;
        combo_idx      = 0;
        speed_left = speed_right = BASE_SPEED;
      }
      break;
  }

  print_status();
}

void setup() {
  Serial.begin(57600);

  pinMode(DIR_RIGHT,   OUTPUT);
  pinMode(SPEED_RIGHT, OUTPUT);
  pinMode(DIR_LEFT,    OUTPUT);
  pinMode(SPEED_LEFT,  OUTPUT);

  bt.begin(9600);
  stop_car();

  if (EEPROM.read(ADDR_VALID) == EEPROM_MAGIC) {
    eeprom_load();
    current_mode = RUN;
    Serial.println(F("Калибровка загружена — режим RUN"));
  } else {
    Serial.println(F("Нет калибровки — режим DIR_CAL"));
  }

  print_status();
}

void loop() {
  if (bt.available() > 0) {
    char c = (char)bt.read();
    if (c == ' ' || c == '\n' || c == '\r' || c == '\t') return;
    Serial.print(F("RX: "));
    Serial.println(c);
    handle_command(c);
  }
}
