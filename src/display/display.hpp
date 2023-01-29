#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include <Adafruit_ST7735.h>
#include <SPI.h>

namespace pliskin {

class display_ST7735: public Adafruit_ST7735
{
    using Adafruit_ST7735::Adafruit_ST7735;

    private:
        const uint16_t _backlight_pin = PB1;

    public:
        void setup (void);
        void setBrightness (int brightness = 1);
};

extern display_ST7735 display;

}

#endif // __DISPLAY_H__