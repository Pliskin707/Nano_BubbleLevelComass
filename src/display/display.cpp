#include "display.hpp"

namespace pliskin {

    // global instance
    // display_ST7735 display(PD7, dc, rst);    

    /*
        data/command (labeled "A0") = A0/PC0
        Reset = A1/PC1
        cable select = A2/PC2

        (SPI on the Nano V3: MOSI = D11/PB3, MISO = D12/PB4, SCK = D13/PB5)
        The TFT only receives data -> "SCK" = D13; "SDA" = MOSI = D11
    */
    display_ST7735 display(A2, A0, A1);    


    void display_ST7735::setup(void)
    {
        pinMode(_backlight_pin, OUTPUT);
        begin();
        // setSPISpeed(20000000);
        initR(INITR_18BLACKTAB);
        fillScreen(ST7735_BLACK);
        setRotation(2);
    }
    
    void display_ST7735::setBrightness(int brightness)
    {
        analogWrite(_backlight_pin, brightness);
    }




}