#include "Arduino.h"
#include "config.h"
#include <Wire.h>
#include "debouncer.cpp"

#include <Rotary.h>   // platformio lib install 275
#include "uRTCLib.h"  // platformio lib install 1371


// time setting & encoder
Rotary encoder = Rotary( ENCODER_PIN_UP, ENCODER_PIN_DOWN );
Debouncer btn_debounce( 390 );

// RTC stuff
uRTCLib rtc;
uint8_t sec;

// state variables
bool pulse_direction;
uint8_t serial_data;
bool btn_action = false;
bool ignore_sec_zero = false;


// Serial debug macros
#ifdef ENABLE_SERIAL_DBG
  #define serial_debug( args... ) Serial.print( args );
  #define serial_debugln( args... ) Serial.println( args );
#else
  #define serial_debug( args );
  #define serial_debugln( args );
#endif



//
// Clock / Pulse functions
//


// update rtc clock
void zero_rtc() {
  rtc.set( 0, 0, 0, 0, 5, 11, 19 );
	// RTCLib::set(byte second, byte minute, byte hour, byte dayOfWeek, byte dayOfMonth, byte month, byte year)
}


// goto next clock blade
void pulse() {
  if ( pulse_direction ){
    digitalWrite( PIN_CLOCK1, HIGH);
    digitalWrite(PIN_CLOCK2, LOW);
    serial_debugln("+");
  }
  else {
    digitalWrite( PIN_CLOCK1, LOW );
    digitalWrite( PIN_CLOCK2, HIGH );
    serial_debugln("-");
  }
  pulse_direction = !pulse_direction;
  delay(160);
  digitalWrite( PIN_CLOCK1, LOW );
  digitalWrite( PIN_CLOCK2, LOW );
}


//
// Encoder functions
//

// gets called when encoder button pressed
void encoder_btn_interrupt() {
  // check if press is bounce
  if ( !btn_debounce.check() ) {
    return;
  }
  btn_action = true;
}


// process encoder input
// gets called every loop run
void encoder_loop() {

  // Check if encoder has moved
  unsigned char enc_dir = encoder.process();

  // if yes, process encoder movement
  if ( enc_dir ) {
    if ( enc_dir == DIR_CW ){
      zero_rtc();
      pulse();
      ignore_sec_zero = true;
    }
     else {
      zero_rtc();
      ignore_sec_zero = true;
    }
  }
}


//
// Setup & main loop
//

// initial setup
void setup() {

  // init encoder
  pinMode( ENCODER_PIN_UP,   INPUT_PULLUP );
  pinMode( ENCODER_PIN_DOWN, INPUT_PULLUP );
  pinMode( ENCODER_PIN_BTN,  INPUT_PULLUP );

  // button press interrupt
  attachInterrupt(
    digitalPinToInterrupt( ENCODER_PIN_BTN ),
    encoder_btn_interrupt,
    CHANGE
  );
  sei();

  digitalWrite( PIN_CLOCK1, LOW );
  digitalWrite( PIN_CLOCK2, LOW );

  // init serial
  #ifdef ENABLE_SERIAL_DBG
    Serial.begin( SERIAL_BAUD );
  #endif

  // init RTC
  Wire.begin();
  rtc.set_rtc_address( RTC_ADDR );
	rtc.set_model( RTC_MODEL );

  // turn off leds
  pinMode( LED_BUILTIN_TX, OUTPUT );
  digitalWrite( LED_BUILTIN_TX, HIGH );
  pinMode( LED_BUILTIN_RX, OUTPUT );
  digitalWrite( LED_BUILTIN_RX, HIGH );

}


// loop function
void loop() {

  // process time setting
  encoder_loop();

  // read rtc clock
  sec = rtc.second();
  //serial_debugln(sec);

  // if second is 0 and action should not be ignored (from encoder set)
  if (sec == 0  && !ignore_sec_zero ){
    serial_debugln("tick");
    pulse();
    delay(999);
    ignore_sec_zero = false;
  }
  if (sec != 0) {
      ignore_sec_zero = false;
  }

  // press on button gets triggered from interrupt
  if (btn_action) {
    btn_action = false;
    serial_debugln("btn");
    for (uint8_t i=0; i<15; i++){
      pulse();
    }
    zero_rtc();
    ignore_sec_zero = true;
  }

}
