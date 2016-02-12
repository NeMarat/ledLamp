#include <Arduino.h>
#include <EEPROM.h>

#define RED_MEM 512
#define GRN_MEM 513
#define BLU_MEM 514

#define ENC_PIN_1 2 
#define ENC_PIN_2 3
#define BTN_PIN   1

#define RED_PIN 11
#define GRN_PIN 10
#define BLU_PIN 9

enum cur_state_t : uint8_t { ADJ_NONE = 0, ADJ_BRIGHT, ADJ_COL };
enum cur_col_t : uint8_t { COL_RED = 0, COL_GRN, COL_BLU };

cur_state_t cur_state;
cur_col_t cur_col;

uint8_t enc_slave = LOW;
int8_t enc_val = 0;

uint8_t red_pwm = 0;
uint8_t grn_pwm = 0;
uint8_t blu_pwm = 0;

uint64_t cur_ms;
uint64_t last_ms;

bool btn_pressed;

void encoder_slave () {
  enc_slave = digitalRead(ENC_PIN_2);
}

void encoder_main () {
  if (enc_slave == HIGH) { enc_val+=5; } 
   else { enc_val-=5; }
}

void apply_colors (void) {
  analogWrite(RED_PIN, red_pwm);
  analogWrite(GRN_PIN, grn_pwm);
  analogWrite(BLU_PIN, blu_pwm);
  EEPROM.write(RED_MEM, red_pwm);
  EEPROM.write(GRN_MEM, grn_pwm);
  EEPROM.write(BLU_MEM, blu_pwm);
}

void adjust_col (cur_col_t c_olor, int8_t v_al) {
  switch (c_olor) {
    case COL_RED: red_pwm += v_al; break; // it shoud go back to 0 if > 255 because type overflow
    case COL_GRN: grn_pwm += v_al; break;
    case COL_BLU: blu_pwm += v_al; break;
  }
  apply_colors();
}

void adjust_brg (int8_t v_al) {
  red_pwm+=v_al;
  grn_pwm+=v_al;
  blu_pwm+=v_al;
  apply_colors();
}

void setup() {

  pinMode(ENC_PIN_1, INPUT);
  pinMode(ENC_PIN_2, INPUT);
  pinMode(BTN_PIN, INPUT);
  pinMode(RED_PIN, OUTPUT);
  pinMode(GRN_PIN, OUTPUT);
  pinMode(BLU_PIN, OUTPUT);
  digitalWrite(ENC_PIN_1, HIGH);
  digitalWrite(ENC_PIN_2, HIGH);
  digitalWrite(BTN_PIN, HIGH);

  attachInterrupt(0, encoder_main, FALLING);
  attachInterrupt(1, encoder_slave, CHANGE);

  red_pwm = EEPROM.read(RED_MEM);
  grn_pwm = EEPROM.read(GRN_MEM);
  blu_pwm = EEPROM.read(BLU_MEM);

  analogWrite(RED_PIN, red_pwm);
  analogWrite(GRN_PIN, grn_pwm);
  analogWrite(BLU_PIN, blu_pwm);

  btn_pressed=false;
}

void loop() {
  cur_ms=millis();
  if (digitalRead(BTN_PIN) == LOW && btn_pressed==false) {
    last_ms=millis();
    btn_pressed=true;
    if (cur_state == ADJ_NONE) { cur_state=ADJ_BRIGHT; }
  } 
  if (digitalRead(BTN_PIN) == HIGH && btn_pressed==true) {
    if (cur_ms - last_ms > 2000) { cur_state=ADJ_COL; cur_col=COL_RED; }
     else { if (cur_state == ADJ_COL) { 
              if (cur_col >= COL_BLU) { cur_col = COL_RED; } 
               else { uint8_t(cur_col) = cur_col + 1; } 
            } 
     }
    btn_pressed=false;
    last_ms=millis();
  }
  if (btn_pressed == false && cur_ms - last_ms > 5000 && cur_state != ADJ_NONE) {
    cur_state = ADJ_BRIGHT; 
  }
  
  if (cur_state == ADJ_COL && enc_val != 0) {
    adjust_col(cur_col, enc_val);
  }
  if (cur_state == ADJ_BRIGHT && enc_val != 0) {
    adjust_brg(enc_val);
  }
}
