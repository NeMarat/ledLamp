#include <Arduino.h>
#include <EEPROM.h>

#define RED_MEM 512
#define GRN_MEM 513
#define BLU_MEM 514

#define ENC_PIN_1 2 
#define ENC_PIN_2 3
#define BTN_PIN   1

typedef enum { ADJ_NONE = 0, ADJ_BRIGHT, ADJ_RED, ADJ_GRN, ADJ_BLU } cur_state;
typedef enum { COL_RED = 0, COL_GRN, COL_BLU } cur_col;

uint8_t enc_slave = LOW;
int8_t enc_val = 0;

void encoder_slave () {
  enc_slave = digitalRead(ENC_PIN_2);
}

void encoder_main {
  if (enc_slave == HIGH) { enc_val++; } 
   else { enc_val--; }
}

void adjust_col (cur_col c_olor, int8_t v_al) {
  switch (c_olor) {
    case COL_RED: red_pwm += v_al; break; // it shoud go back to 0 if > 255 because type overflow
    case COL_GRN: grn_pwm += v_al; break;
    case BLU_GRN: blu_pwm += v_al; break;
  }
}

void writeULong (unsigned long l, int address) {
  EEPROM.write(address + 0, *((byte*)&l + 0)); //byte #1 from long
  EEPROM.write(address + 1, *((byte*)&l + 1)); // byte #2
  EEPROM.write(address + 2, *((byte*)&l + 2)); // #3
  EEPROM.write(address + 3, *((byte*)&l + 3)); // #4
}

unsigned long readULong(int address) {
  unsigned long l = 0;
  *((byte*)&l + 0) = EEPROM.read(address + 0);
  *((byte*)&l + 1) = EEPROM.read(address + 1);
  *((byte*)&l + 2) = EEPROM.read(address + 2);
  *((byte*)&l + 3) = EEPROM.read(address + 3);
  return l;
}

void setup() {
  uint8_t red_pwm = EEPROM.read(RED_MEM);
  uint8_t grn_pwm = EEPROM.read(GRN_MEM);
  uint8_t blu_pwm = EEPROM.read(BLU_MEM);

  pinMode(ENC_PIN_1, INPUT);
  pinMode(ENC_PIN_2, INPUT);
  pinMode(BTN_PIN, INPUT);
  digitalWrite(ENC_PIN_1, HIGH);
  digitalWrite(ENC_PIN_2, HIGH);
  digitalWrite(BTN_PIN, HIGH);

  attachInterrupt(0, encoder_main, FALLING);
  attachInterrupt(1, encoder_slave, CHANGE);

}

void loop() 
{

}
