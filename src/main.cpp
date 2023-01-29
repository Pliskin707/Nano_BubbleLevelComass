#include <Arduino.h>
#include <HardwareSerial.h>

#include "adc_c.h"

#include "display/display.hpp"

#define L_LED (LED_BUILTIN)
using namespace pliskin_adc;
using namespace pliskin;

void setup() {
  // put your setup code here, to run once:
  pinMode(L_LED, OUTPUT);
  Serial.begin(115200);
  display.setup();
  display.enableDisplay(ON);
  display.setTextColor(ST7735_RED, ST7735_BLACK);
}

void loop() {
  static bool toggle = false;
  toggle ^= true;
  digitalWrite(L_LED, toggle);
  display.setBrightness(toggle);

  const auto supplyMilliVolt = adc_get_vcc_mV();
  char buf[50];
  div_t qr = div(supplyMilliVolt, 1000);
  snprintf_P(buf, sizeof(buf), PSTR("Supply: %u.%03u Volt\n"), qr.quot, qr.rem);

  Serial.print(buf);
  display.setCursor(0, 0);
  display.println(buf);
  
  _delay_ms(1000);
}