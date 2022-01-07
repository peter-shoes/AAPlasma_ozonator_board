#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_LiquidCrystal.h>
#include <DFRobot_DHT20.h>

// PIN DEFS
const int btn_heat  = 22;
const int btn_fan = 24;
const int btn_oz1 = 26;
const int btn_oz2 = 28;
const int btn_neb1 = 30;
const int btn_neb2 = 32;
const int btn_cycle = 34;
const int btn_rec = 36;

const int btn_heat_led  = 23;
const int btn_fan_led = 25;
const int btn_oz1_led = 27;
const int btn_oz2_led = 29;
const int btn_neb1_led = 31;
const int btn_neb2_led = 33;
const int btn_cycle_led = 35;
const int btn_rec_led = 37;

const int relay_heat = 45;
const int relay_fan = 45;
const int relay_oz1 = 45;
const int relay_oz2 = 45;
const int relay_neb1 = 45;
const int relay_neb2 = 45;

// DIGITAL VARS
byte btn_heat_state = 0;
byte btn_fan_state = 0;
byte btn_oz1_state = 0;
byte btn_oz2_state = 0;
byte btn_neb1_state = 0;
byte btn_neb2_state = 0;
byte btn_cycle_state = 0;
byte btn_rec_state = 0;

// MEASUREMENT VARS
float temp_val = 0;
float hum_val = 0;
float o3_high_val = 0;
float o3_low_val = 0;

// INIT PERIPHERALS
Adafruit_LiquidCrystal lcd(0);
DFRobot_DHT20 dht20;

// FUNC DEFS
void btns();
void relays();
void temp_hum();
void o3_high();
void o3_low();
void cycle(bool io);
void rec(bool io);
void disp();
void timer(int mins, String txt);
void flip_flop(int check_pin, int led_pin, byte *state);