#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DFRobot_DHT20.h>
#include <defs.hpp>

void setup() {
  // START SERIAL
  Serial.begin(9600);

  // SET PINMODES
  pinMode(heat.btn_pin, INPUT_PULLUP);
  pinMode(fan.btn_pin, INPUT_PULLUP);
  pinMode(oz_1.btn_pin, INPUT_PULLUP);
  pinMode(oz_2.btn_pin, INPUT_PULLUP);
  pinMode(neb_1.btn_pin, INPUT_PULLUP);
  pinMode(neb_2.btn_pin, INPUT_PULLUP);
  pinMode(cycle.btn_pin, INPUT_PULLUP);
  pinMode(rec.btn_pin, INPUT_PULLUP);

  pinMode(heat.led_pin, OUTPUT);
  pinMode(fan.led_pin, OUTPUT);
  pinMode(oz_1.led_pin, OUTPUT);
  pinMode(oz_2.led_pin, OUTPUT);
  pinMode(neb_1.led_pin, OUTPUT);
  pinMode(neb_2.led_pin, OUTPUT);
  pinMode(cycle.led_pin, OUTPUT);
  pinMode(rec.led_pin, OUTPUT);

  pinMode(heat.relay_pin_1, OUTPUT);
  pinMode(heat.relay_pin_2, OUTPUT);
  pinMode(fan.relay_pin_1, OUTPUT);
  pinMode(oz_1.relay_pin_1, OUTPUT);
  pinMode(oz_2.relay_pin_1, OUTPUT);
  pinMode(neb_1.relay_pin_1, OUTPUT);
  pinMode(neb_2.relay_pin_1, OUTPUT);

  // START LCD
  lcd.init();
  lcd.backlight();
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

  // turn off LEDS
  digitalWrite(heat.led_pin, LOW);
  digitalWrite(fan.led_pin, LOW);
  digitalWrite(oz_1.led_pin, LOW);
  digitalWrite(oz_2.led_pin, LOW);
  digitalWrite(neb_1.led_pin, LOW);
  digitalWrite(neb_2.led_pin, LOW);
  digitalWrite(cycle.led_pin, LOW);
  digitalWrite(rec.led_pin, LOW);
  
}

void loop() {
  // CHECK BUTTONS
  btns();
  // UPDATE STATES
  update_states();
  // SWITCH RELAYS
  relays();
  // CHECK CYCLE
  // if (btn_cycle_state == 3) cycle();
  // CHECK REC
  // todo: see if this is the correct placement
  // if (btn_rec_state == 3) rec(1);
  // CHECK READINGS
  check_readings();
  // DISPLAY
  disp();
}

void check_readings() {
  temp_hum();
  o3_high();
  o3_low();
}

void btns() {
  /*
  Run the flip flop on all of the buttons

  (maybe not needed?)
  */
  flip_flop(&heat);
  flip_flop(&fan);
  flip_flop(&oz_1);
  flip_flop(&oz_2);
  flip_flop(&neb_1);
  flip_flop(&neb_2);
  flip_flop(&cycle);
  flip_flop(&rec);
}

void update_states() {
  /*
  In theory, this would update the prev state of the structs.

  Not implemented here, but it is implemented in the relays and flip flop func

  Maybe it should be implemented here (?)
  */
}

void relays() {
  /*
  This funstion will check if the button state has changed in the last cycle.

  If the state has changed, it will switch the relay to the appropriate state (I/O).

  Then, the prev state of the struct will be updated to reflect in the next cycle
  that the state has not changed.
  */
  if (heat.state != heat.prev_state) {
    if (heat.state == 2) {
      digitalWrite(heat.relay_pin_1, HIGH);
      digitalWrite(heat.relay_pin_2, HIGH);
    }
    else {
      digitalWrite(heat.relay_pin_1, LOW);
      digitalWrite(heat.relay_pin_1, LOW);
    }
    heat.prev_state = heat.state;
  }
  
  if (fan.state != fan.prev_state) {
    if (fan.state == 2) digitalWrite(fan.relay_pin_1, HIGH);
    else digitalWrite(fan.relay_pin_1, LOW);
    fan.prev_state = fan.state;
  }
  
  if (oz_1.state != oz_1.prev_state) {
    if (oz_1.state == 2) digitalWrite(oz_1.relay_pin_1, HIGH);
    else digitalWrite(oz_1.relay_pin_1, LOW);
    oz_1.prev_state = oz_1.state;
  }
  
  if (oz_2.state != oz_2.prev_state) {
    if (oz_2.state == 2) digitalWrite(oz_2.relay_pin_1, HIGH);
    else digitalWrite(oz_2.relay_pin_1, LOW);
    oz_2.prev_state = oz_2.state;
  }
  
  if (neb_1.state != neb_1.prev_state) {
    if (neb_1.state == 2) digitalWrite(neb_1.relay_pin_1, HIGH);
    else digitalWrite(neb_1.relay_pin_1, LOW);
    neb_1.prev_state = neb_2.state;
  }
  
  if (neb_2.state != neb_2.prev_state) {
    if (neb_2.state == 2) digitalWrite(neb_2.relay_pin_1, HIGH);
    else digitalWrite(neb_2.relay_pin_1, LOW);
    neb_2.prev_state = neb_2.state;
  }
}

void temp_hum() {
  /*
  This is just a function to get the temp and humidity from the DHT22
  */
  temp_val = dht20.getTemperature();
  hum_val = dht20.getHumidity();
}

void o3_high() {
  /*
  Function to get the data from the MQ-131, Ozone 2-click
  */
}

void o3_low() {
  /*
  Function to get the data from the other ozone sensor
  */
}

void run_cycle() {
  /*
  Function to run the auto cycle
  All times are in minutes

  This can be modified later, but it basically goes like this:
    1) ozone on
    2) ozone off and mix with fan
    3) heater on and heater fan on
    4) heater and heater fan off and mixer fan on
  */
  unsigned int ozone_on = 1 * 60000;
  unsigned int mix_1 = 1 * 60000;
  unsigned int heat_on = 1 * 60000;
  unsigned int mix_2 = 1 * 60000;
  unsigned int cycle_start = millis();

  // turn off all relays if they are on
  heat.state, oz_1.state, oz_2.state, neb_1.state, neb_2.state = 0;
  relays();

  while (cycle.state != 0) {
    // todo: get a timer on the screen here, also turn on the correct LEDs
    
    // ozone on
    if (cycle.state != 0) {
    digitalWrite(oz_1.relay_pin_1, HIGH);
    digitalWrite(oz_2.relay_pin_1, HIGH);
    }
    while (((cycle_start - millis()) < ozone_on) && cycle.state != 0) {
      flip_flop(&cycle);
      flip_flop(&rec);
    }

    // ozone off and mix
    if (cycle.state != 0) {
      digitalWrite(oz_1.relay_pin_1, LOW);
      digitalWrite(oz_2.relay_pin_1, LOW);
      digitalWrite(fan.relay_pin_1, HIGH);
    }
    while (((cycle_start - millis()) < mix_1) && cycle.state != 0) {
      flip_flop(&cycle);
      flip_flop(&rec);
    }

    // end mix and heat on
    if (cycle.state != 0) {
      digitalWrite(fan.relay_pin_1, LOW);
      digitalWrite(heat.relay_pin_1, HIGH);
      digitalWrite(heat.relay_pin_2, HIGH);
    }
    while (((cycle_start - millis()) < heat_on) && cycle.state != 0) {
      flip_flop(&cycle);
      flip_flop(&rec);
    }

    // end heat and mix
    if (cycle.state != 0) {
      digitalWrite(heat.relay_pin_1, LOW);
      digitalWrite(heat.relay_pin_2, LOW);
      digitalWrite(fan.relay_pin_1, HIGH);
    }
    while (((cycle_start - millis()) < mix_2) && cycle.state != 0) {
      flip_flop(&cycle);
      flip_flop(&rec);
    }
    // always turn off this fan anyway
    digitalWrite(fan.relay_pin_1, LOW);
  }
}

void run_rec(bool io) {
  if (io) Serial.println("<REC&1>");
  else Serial.println("<REC&0>");
}

void disp() {
  /*
  Main display module function to show heat, hum and o3

  This will update as often as it gets the chance, which may be bad

  This checks high and low ozone sensors, but displays one:
    if the reading is below what the MQ-131 can do, display low val
    otherwise display the max of the two readings
  */
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("T:");
  lcd.print(temp_val);
  lcd.setCursor(9,0);
  lcd.print("H:");
  lcd.print(hum_val);
  lcd.setCursor(0,1);
  lcd.print("O3:");
  if (o3_low_val < 10) lcd.print(o3_low_val);
  else lcd.print(max(o3_low_val, o3_high_val));
}

void timer(int mins, String txt) {
  /*
  This is a totally disgusting function to simply display a timer with a title

  This really sucks
  */

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

void flip_flop(btn_interface *inter) {
  /*
  This is the main button driver function

  Implements a state machine to hold the button I/O in a certain state

  Additionally, when the state switches, it will change the cuttent and prev state in the interface.
  Again, previous state should probably be dealt with somewhere else.
  */
  switch( inter->state )
  {
    case 0://------------------------ I'm off and in restmode
      if ( digitalRead(inter->btn_pin) == LOW )
      { // switch relay OFF
        // switch LED OFF
        inter->prev_state = 0;
        inter->state = 1;
      }
      break;
    case 1://------------------------ I'm in ON mode, w8 4 keyrelease
      if ( digitalRead(inter->btn_pin) == HIGH )
        inter->prev_state = 1;
        inter->state = 2;
      break;
    case 2://------------------------ I'm ON and in restmode
      if (  digitalRead(inter->btn_pin) == LOW )
      { // switch relay ON
        // switch LED ON
        inter->prev_state - 2;
        inter->state = 3;
      }
      break;
    case 3://------------------------ I'm in OFF mode, w8 4 keyrelease
      if ( digitalRead(inter->btn_pin) == HIGH )
        inter->prev_state = 3;
        inter->state = 0;
      break;
  }
}