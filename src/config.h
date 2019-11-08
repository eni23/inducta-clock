#ifndef __CONFIG_H
  #define __CONFIG_H

  #define SERIAL_BAUD       19200

  #define PIN_CLOCK1        5
  #define PIN_CLOCK2        6

  #define ENCODER_PIN_UP    8
  #define ENCODER_PIN_DOWN  9
  #define ENCODER_PIN_BTN   7 // pin 7 is needed for interrupt

  #define RTC_MODEL         1 // 1=DS1307, 2=DS3231, 3=DS3232
  #define RTC_ADDR          0x68

  #define ENABLE_SERIAL_DBG true


#endif
