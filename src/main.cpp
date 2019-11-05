#include "Arduino.h"
#include "config.h"
//#include <Wire.h>
#include "debouncer.cpp"

#include <Rotary.h>   // platformio lib install 275
// #include <DS3231.h>   // platformio lib install 1379


// time setting & encoder
Rotary encoder = Rotary(ENCODER_PIN_UP, ENCODER_PIN_DOWN);
Debouncer btn_debounce( 390 );

bool pulse_direction;
uint8_t serial_data;

// Serial debug macros
#ifdef ENABLE_SERIAL_DBG
  #define serial_debug(args...) Serial.print(args);
  #define serial_debugln(args...) Serial.println(args);
#else
  #define serial_debug(args);
  #define serial_debugln(args);
#endif


//
// Clock / Pulse functions
//
void pulse(){
  if (pulse_direction){
    serial_debugln("pulse +");
    digitalWrite(PIN_CLOCK1, HIGH);
    digitalWrite(PIN_CLOCK2, LOW);
  }
  else {
    serial_debugln("pulse -");
    digitalWrite(PIN_CLOCK1, LOW);
    digitalWrite(PIN_CLOCK2, HIGH);
  }
  pulse_direction = !pulse_direction;
  delay(160);
  digitalWrite(PIN_CLOCK1, LOW);
  digitalWrite(PIN_CLOCK2, LOW);
}




//
// Encoder functions
//

// gets called when encoder button pressed
void encoder_btn_interrupt(){
  // check if press is bounce
  if ( !btn_debounce.check() ){
    return;
  }
  serial_debugln("btn press");

}

// process encoder input
// gets called every loop run
void encoder_loop(){

  // Check if encoder has moved
  unsigned char enc_dir = encoder.process();

  // if yes, process encoder movement
  if ( enc_dir ){
    if (enc_dir == DIR_CW){
      pulse();
    } else {
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

  digitalWrite(PIN_CLOCK1, LOW);
  digitalWrite(PIN_CLOCK2, LOW);

  // init serial & rs485 SoftwareSerial
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
  if (Serial.available()) {
    serial_data = Serial.read();
    if (serial_data == 32 || serial_data == 13){
      pulse();
    }
    serial_debugln(serial_data);
  }
  //pulse();


}
