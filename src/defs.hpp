#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DFRobot_DHT20.h>

typedef struct {
    int btn_pin;
    int led_pin;
    int relay_pin_1;
    int relay_pin_2;
    byte state = 0;
    byte prev_state = 4;
} btn_interface;

// STRUCT DEFS
btn_interface heat = {22, 23, 39, 45, 0, 4};
btn_interface fan = {24, 25, 41, NULL};
btn_interface oz_1 = {26, 27, 43, NULL};
btn_interface oz_2 = {28, 29, 45, NULL};
btn_interface neb_1 = {30, 31, 45, NULL};
btn_interface neb_2 = {32, 33, 45, NULL};
btn_interface cycle = {34, 35, NULL, NULL};
btn_interface rec = {36, 37, NULL, NULL};

// MEASUREMENT VARS
float temp_val = 0;
float hum_val = 0;
float o3_high_val = 0;
float o3_low_val = 0;

// INIT PERIPHERALS
LiquidCrystal_I2C lcd(0x27, 16, 2);
DFRobot_DHT20 dht20;

// FUNC DEFS
void btns();
void update_states();
void relays();
void check_readings();
void temp_hum();
void o3_high();
void o3_low();
void run_cycle();
void run_rec(bool io);
void disp();
void timer(int mins, String txt);
void flip_flop(btn_interface *inter);