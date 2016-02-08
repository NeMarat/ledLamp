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

uint64_t cur_ms;
uint64_t last_ms;

bool btn_pressed;

void encoder_slave () {
  enc_slave = digitalRead(ENC_PIN_2);
}

void encoder_main () {
  if (enc_slave == HIGH) { enc_val++; } 
   else { enc_val--; }
}

void adjust_col (cur_col c_olor, int8_t v_al) {
  switch (c_olor) {
    case COL_RED: red_pwm += v_al; EEPROM.write(RED_MEM, red_pwm); break; // it shoud go back to 0 if > 255 because type overflow
    case COL_GRN: grn_pwm += v_al; EEPROM.write(GRN_MEM, grn_pwm); break;
    case BLU_GRN: blu_pwm += v_al; EEPROM.write(BLU_MEM, blu_pwm); break;
  }
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

  btn_pressed=false;
}

void loop() {
  cur_ms=millis;
  if (digitalWrite(BTN_PIN) == LOW && btn_pressed==false) {
    last_ms=millis();
    btn_pressed=true;
    if (cur_state == ADJ_NONE) { cur_state=ADJ_BRIGHT; }
  } 
  if (digitalWrite(BTN_PIN) == HIGH && btn_pressed==true) {
    if (cur_ms - last_ms > 2000) { cur_state=ADJ_RED; }
     else { if (cur_state == ADJ_BRIGHT) { cur_state=ADJ_NONE; }
            if (cur_state == ADJ_RED) { cur_state=ADJ_GRN; }
            if (cur_state == ADJ_GRN) { cur_state=ADJ_BLU; }
            if (cur_state == ADJ_BLU) { cur_state=ADJ_RED; }
            if (cur_state == ADJ_NONE) { cur_state=ADJ_BRIGHT; }
     }
    btn_pressed=false;
  }

}
