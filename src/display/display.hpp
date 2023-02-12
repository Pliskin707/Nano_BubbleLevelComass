#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include <Adafruit_ST7735.h>
#include <SPI.h>

namespace pliskin {

#define DISPLAY_BUBBLE_RADIUS   (63)    //! Bubble level circle radius
#define DISPLAY_BAR_WIDTH       (16)
typedef enum
{
    nothing = 0,
    bubbleLevel = 1,
    compass = 2
} e_displayContent;

typedef enum
{
    circleDot = 0,
    bars
} e_bubbleStyle;

// the first point (x0/y0) is always pointing to the center
typedef struct
{
    int16_t x[3];
    int16_t y[3];
} t_trianglePoints;
class display_ST7735: public Adafruit_ST7735
{
    using Adafruit_ST7735::Adafruit_ST7735;

    private:
        const uint16_t _backlight_pin = PB1;
        const uint16_t _backgroundColor = ST7735_BLACK;
        uint16_t _defaultColor = ST7735_RED;
        struct 
        {
            e_bubbleStyle style = bars;
            struct
            {
                uint16_t x = 0;
                uint16_t y = 0;
            } dot;

            struct 
            {
                t_trianglePoints longSideIndicator;
                t_trianglePoints shortSideIndicator;
                t_trianglePoints longSidePrev = {0};
                t_trianglePoints shortSidePrev = {0};
                bool longIndicatorFilled = false;
                bool shortIndicatorFilled = false;
            } barIndicators;
        } _bubble;

        

        void _clearPrevBubble (void);
        void _printTriangle (const t_trianglePoints &triangle, const uint16_t color, const bool fill = false);
        void _updateCircleDot (const int16_t horizontal, const int16_t vertical);
        void _updateBars (const int16_t horizontal, const int16_t vertical);

    public:
        void setup (void);
        void setBrightness (const int brightness = 1);
        void clear (void) {fillScreen(_backgroundColor);};
        void printFrame (const uint_fast16_t contentFlags = bubbleLevel | compass);
        void updateLevel (const int16_t horizontal, const int16_t vertical);    // for now [-100..100] (don't know the values of the sensor yet)
};

extern display_ST7735 display;

}

#endif // __DISPLAY_H__