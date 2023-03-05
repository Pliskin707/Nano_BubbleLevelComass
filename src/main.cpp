#include <Arduino.h>
#include <HardwareSerial.h>
#include <Wire.h>
#include <LSM303.h>

#include "adc_c.h"

#include "display/display.hpp"
#include "calibration/calib.hpp"

#define L_LED (LED_BUILTIN)
using namespace pliskin_adc;
using namespace pliskin;
LSM303 sensorInertiaCompass;
static const char sensorFstr[] PROGMEM  = "%+06d";

void setup() {
  // put your setup code here, to run once:
  pinMode(L_LED, OUTPUT);
  // Serial.begin(115200);
  display.setup();
  display.enableDisplay(ON);
  display.setTextColor(ST7735_RED, ST7735_BLACK);
  display.printFrame();

  Wire.begin();
  sensorInertiaCompass.init();
  sensorInertiaCompass.enableDefault();
  sensorInertiaCompass.setTimeout(100);
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

  // // test level display
  // {
  //   static int16_t h = 0, v = 0, seq = 0;

  //   switch (seq)
  //   {
  //     case 0:
  //       if (++h == 100) seq++;
  //       break;

  //     case 1:
  //       if (++v == 100) seq++;
  //       break;

  //     case 2:
  //       if (h > -100) 
  //       {
  //         h--;
  //         v--;
  //       }
  //       else
  //         seq++;
  //       break;

  //     default:
  //       seq = h = v = 0;
  //   }
  //   display.updateLevel(h, v);
  // }

  static uint32_t lastSensorRead = 0;
  if ((millis() - lastSensorRead) > 250)
  {
    lastSensorRead = millis();

    // display.fillRect(40, 40, display.width() - 40, display.width() - 40, 0x000F);

    char sensorString[40];
    sensorInertiaCompass.read();
    display.setCursor(40, 40);
    display.print("Mag:");

    display.setCursor(64, 40);
    snprintf_P(sensorString, sizeof(sensorString), sensorFstr, sensorInertiaCompass.m.x);
    display.print(sensorString);

    display.setCursor(64, 50);
    snprintf_P(sensorString, sizeof(sensorString), sensorFstr, sensorInertiaCompass.m.y);
    display.print(sensorString);

    display.setCursor(64, 60);
    snprintf_P(sensorString, sizeof(sensorString), sensorFstr, sensorInertiaCompass.m.z);
    display.print(sensorString);

    

    display.setCursor(40, 80);
    display.print("Acc:");

    display.setCursor(64, 80);
    snprintf_P(sensorString, sizeof(sensorString), sensorFstr, sensorInertiaCompass.a.x);
    display.print(sensorString);

    display.setCursor(64, 90);
    snprintf_P(sensorString, sizeof(sensorString), sensorFstr, sensorInertiaCompass.a.y);
    display.print(sensorString);

    display.setCursor(64, 100);
    snprintf_P(sensorString, sizeof(sensorString), sensorFstr, sensorInertiaCompass.a.z);
    display.print(sensorString);

    int32_t
      v = sensorInertiaCompass.a.x,
      h = sensorInertiaCompass.a.y;

    if (getCalibDataValid())
    {
      display.setCursor(100, 0);
      display.print("C");

      const auto& calibData = getCalibData();
      v -= calibData.acc[0];
      h -= calibData.acc[1];
    }

    v *= 400;
    v /= INT16_MAX;

    h *= 400;
    h /= INT16_MAX;

    display.updateLevel(h, v);
  }
  
  static bool calibWritten = false;
  if ((millis() > 20000) && !calibWritten)
  {
    calibWritten = true;

    t_calib_v1
      calibData = {0};

    calibData.acc[0] = sensorInertiaCompass.a.x;
    calibData.acc[1] = sensorInertiaCompass.a.y;
    calibData.acc[2] = sensorInertiaCompass.a.z;

    calibData.mag[0] = sensorInertiaCompass.m.x;
    calibData.mag[1] = sensorInertiaCompass.m.y;
    calibData.mag[2] = sensorInertiaCompass.m.z;

    setCalibData(calibData);
  }
  
  // _delay_ms(10);
}