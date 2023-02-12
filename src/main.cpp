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
  // Serial.begin(115200);
  display.setup();
  display.enableDisplay(ON);
  display.setTextColor(ST7735_RED, ST7735_BLACK);
  display.printFrame();
}

void loop() {
  static bool toggle = false;
  toggle ^= true;
  digitalWrite(L_LED, toggle);
  display.setBrightness(toggle);

  static uint32_t lastSupplyRead = 0;
  if (millis() - lastSupplyRead > 1000)
  {
    lastSupplyRead = millis();
    const auto supplyMilliVolt = adc_get_vcc_mV();
    char buf[50];
    div_t qr = div(supplyMilliVolt, 1000);
    snprintf_P(buf, sizeof(buf), PSTR("Supply: %u.%03u Volt\n"), qr.quot, qr.rem);

    // Serial.print(buf);
    display.setCursor(0, 0);
    display.println(buf);
  }

  // test level display
  {
    static int16_t h = 0, v = 0, seq = 0;

    switch (seq)
    {
      case 0:
        if (++h == 100) seq++;
        break;

      case 1:
        if (++v == 100) seq++;
        break;

      case 2:
        if (h > -100) 
        {
          h--;
          v--;
        }
        else
          seq++;
        break;

      default:
        seq = h = v = 0;
    }
    display.updateLevel(h, v);
  }
  
  _delay_ms(10);
}