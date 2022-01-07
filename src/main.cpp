#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_LiquidCrystal.h>
#include <DFRobot_DHT20.h>
#include <defs.hpp>

void setup() {
  // START SERIAL
  Serial.begin(9600);

  // SET PINMODES
  pinMode(btn_heat, INPUT_PULLUP);
  pinMode(btn_fan, INPUT_PULLUP);
  pinMode(btn_oz1, INPUT_PULLUP);
  pinMode(btn_oz2, INPUT_PULLUP);
  pinMode(btn_neb1, INPUT_PULLUP);
  pinMode(btn_neb2, INPUT_PULLUP);
  pinMode(btn_cycle, INPUT_PULLUP);
  pinMode(btn_rec, INPUT_PULLUP);

  pinMode(btn_heat_led, OUTPUT);
  pinMode(btn_fan_led, OUTPUT);
  pinMode(btn_oz1_led, OUTPUT);
  pinMode(btn_oz2_led, OUTPUT);
  pinMode(btn_neb1_led, OUTPUT);
  pinMode(btn_neb2_led, OUTPUT);
  pinMode(btn_cycle_led, OUTPUT);
  pinMode(btn_rec_led, OUTPUT);

  pinMode(relay_heat, OUTPUT);
  pinMode(relay_fan, OUTPUT);
  pinMode(relay_oz1, OUTPUT);
  pinMode(relay_oz2, OUTPUT);
  pinMode(relay_neb1, OUTPUT);
  pinMode(relay_neb2, OUTPUT);

  // START LCD
  lcd.begin(16, 2);
  lcd.setBacklight(HIGH);
  lcd.setCursor(0,0);
  lcd.print("====AAPLASMA====");
  lcd.setCursor(0,1);
  lcd.print("OZONATOR CONTROL");
  delay(2000);
  // START DHT20
  // while(dht20.begin()){
  //   Serial.println("Initialize sensor failed");
  //   delay(1000);
  // }
  lcd.clear();
  // MQ-131 SENSOR HEAT UP
  // timer(1, "HEATING MQ-131:");
  // lcd.clear();
  
}

void loop() {
  // CHECK BUTTONS
  btns();
  // CYCLE
  if (btn_cycle_state == 3) cycle();
  // RECORD
  if (btn_rec_state == 3) rec();
  // SWITCH RELAYS
  // relays();
  // CHECK TEMP/HUMIDITY
  // temp_hum();
  // CHECK OZONE HIGH
  // o3_high();
  // CHECK OZONE LOW
  // o3_low();
  // DISPLAY
  // disp();
}

void btns() {
  flip_flop(btn_heat, btn_heat_led, &btn_heat_state);
  flip_flop(btn_fan, btn_fan_led, &btn_fan_state);
  flip_flop(btn_oz1, btn_oz1_led, &btn_oz1_state);
  flip_flop(btn_oz2, btn_oz2_led, &btn_oz2_state);
  flip_flop(btn_neb1, btn_neb1_led, &btn_neb1_state);
  flip_flop(btn_neb2, btn_neb2_led, &btn_neb2_state);
  flip_flop(btn_cycle, btn_cycle_led, &btn_cycle_state);
  flip_flop(btn_rec, btn_rec_led, &btn_rec_state);
}

void relays() {
  // this also controls the heater fan
  if (btn_heat_state) digitalWrite(relay_heat, HIGH);
  else digitalWrite(relay_heat, LOW);

  if (btn_fan_state) digitalWrite(relay_fan, HIGH);
  else digitalWrite(relay_fan, LOW);

  if (btn_oz1_state) digitalWrite(relay_oz1, HIGH);
  else digitalWrite(relay_oz1, LOW);

  if (btn_oz2_state) digitalWrite(relay_oz2, HIGH);
  else digitalWrite(relay_oz2, LOW);

  if (btn_neb1_state) digitalWrite(relay_neb1, HIGH);
  else digitalWrite(relay_neb1, LOW);

  if (btn_neb2_state) digitalWrite(relay_neb2, HIGH);
  else digitalWrite(relay_neb2, LOW);
}

void temp_hum() {
  temp_val = dht20.getTemperature();
  hum_val = dht20.getHumidity();
}

void o3_high() {}

void o3_low() {}

void cycle() {
  // all times are in minutes
  int ozone_on = 1 * 60000;
  int mix_1 = 1 * 60000;
  int heat_on = 1 * 60000;
  int mix_2 = 1 * 60000;
  int cycle_start = millis();

  // turn off all relays if they are on
  digitalWrite(relay_heat, LOW);
  digitalWrite(relay_fan, LOW);
  digitalWrite(relay_oz1, LOW);
  digitalWrite(relay_oz2, LOW);
  digitalWrite(relay_neb1, LOW);
  digitalWrite(relay_neb2, LOW);

  if (btn_cycle_state != 0) {
    digitalWrite(relay_oz1, HIGH);
    digitalWrite(relay_oz2, HIGH);
  }
  while (((cycle_start - millis()) < ozone_on) && btn_cycle_state != 0) {
    flip_flop(btn_cycle, btn_cycle_led, &btn_cycle_state);
    flip_flop(btn_rec, btn_rec_led, &btn_rec_state);
  }
  if (btn_cycle_state != 0) {
    digitalWrite(relay_oz1, LOW);
    digitalWrite(relay_oz2, LOW);
    digitalWrite(relay_fan, HIGH);
  }
  while (((cycle_start - millis()) < mix_1) && btn_cycle_state != 0) {
    flip_flop(btn_cycle, btn_cycle_led, &btn_cycle_state);
    flip_flop(btn_rec, btn_rec_led, &btn_rec_state);
  }
  if (btn_cycle_state != 0) {
    digitalWrite(relay_fan, LOW);
    digitalWrite(relay_heat, HIGH);
  }
  
  while (((cycle_start - millis()) < heat_on) && btn_cycle_state != 0) {
    flip_flop(btn_cycle, btn_cycle_led, &btn_cycle_state);
    flip_flop(btn_rec, btn_rec_led, &btn_rec_state);
  }
  if (btn_cycle_state != 0) {}
  digitalWrite(relay_heat, LOW);
  digitalWrite(relay_fan, HIGH);
  while (((cycle_start - millis()) < mix_2) && btn_cycle_state != 0) {
    flip_flop(btn_cycle, btn_cycle_led, &btn_cycle_state);
    flip_flop(btn_rec, btn_rec_led, &btn_rec_state);
  }
  digitalWrite(relay_fan, LOW);
}

void rec(bool io) {
  if (io) Serial.println("<REC&1>");
  else Serial.println("<REC&0>");
}

void disp() {
  // lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("T:");
  lcd.print(temp_val);
  lcd.setCursor(9,0);
  lcd.print("H:");
  lcd.print(hum_val);
  lcd.setCursor(0,1);
  lcd.print("O3:");
  // if the reading is below what the mq-131 can do, display the low val
  // else display the max of the two readings
  if (o3_low_val < 10) lcd.print(o3_low_val);
  else lcd.print(max(o3_low_val, o3_high_val));
}

void timer(int mins, String txt) {

  int milli = millis();
  int mins_val = mins;

  while((int)(millis() - milli) < (60000 * mins)) {
    lcd.setCursor(0,0);
    lcd.print(txt);
    lcd.setCursor(0,1);
    int secs_elapsed = (millis()-milli)/1000;
    if (mins_val < 10) lcd.print(0);
    lcd.print(mins_val);
    lcd.print(":");
    int secs_val = 60 - (secs_elapsed)%60;
    if (secs_val == 60) {
      secs_val = 0;
      mins_val--;
    }
    if (secs_val < 10) lcd.print(0);
    lcd.print(secs_val);
    delay(1000);
    lcd.clear();
  }
  milli = millis();
  lcd.setCursor(0,0);
  lcd.print(txt);
  lcd.setCursor(0,1);
  lcd.print("DONE!");
  delay(5000);
}

void flip_flop(int check_pin, int led_pin, byte* state) {
  switch( *state )
  {
    case 0://------------------------ I'm off and in restmode
      if ( digitalRead(check_pin) == LOW )
      { // switch relay OFF
        // switch LED OFF
        digitalWrite(led_pin, LOW);
        if (check_pin == btn_rec) rec(0);
        if (check_pin == btn_cycle) cycle(0);
        *state = 1;
      }
      break;
    case 1://------------------------ I'm in ON mode, w8 4 keyrelease
      if ( digitalRead(check_pin) == HIGH )
        *state = 2;
      break;
    case 2://------------------------ I'm ON and in restmode
      if (  digitalRead(check_pin) == LOW )
      { // switch relay ON
        // switch LED ON
        digitalWrite(led_pin, HIGH);
        if (check_pin == btn_rec) rec(1);
        if (check_pin == btn_cycle) cycle(1);
        *state = 3;
      }
      break;
    case 3://------------------------ I'm in OFF mode, w8 4 keyrelease
      if ( digitalRead(check_pin) == HIGH )
        *state = 0;
      break;
  }
}