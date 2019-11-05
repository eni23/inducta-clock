#include "Arduino.h"
#include "config.h"
#include <Wire.h>
#include "debouncer.cpp"

#include <Rotary.h>   // platformio lib install 275
#include <DS1307.h>   // platformio lib install 145



// time setting & encoder
Rotary encoder = Rotary( ENCODER_PIN_UP, ENCODER_PIN_DOWN );
Debouncer btn_debounce( 390 );

// RTC stuff
DS1307 rtc;
uint8_t sec, min, hour, day, month;
uint16_t year;

// state variables
bool pulse_direction;
uint8_t serial_data;


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
  rtc.set( 0, 0, 0, 5, 11, 2019 );
}


// goto next clock blade
void pulse() {
  if ( pulse_direction ){
    digitalWrite( PIN_CLOCK1, HIGH);
    digitalWrite(PIN_CLOCK2, LOW);
  }
  else {
    digitalWrite( PIN_CLOCK1, LOW );
    digitalWrite( PIN_CLOCK2, HIGH );
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
  serial_debugln("btn press");
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
    }
     else {
      zero_rtc();
      pulse();
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

  rtc.get( &sec, &min, &hour, &day, &month, &year );
  if (sec == 0){
    pulse();
  }

}
