#include "display.hpp"

namespace pliskin {
    void display_ST7735::_printTriangle(const t_trianglePoints &triangle, const uint16_t color, const bool fill)
    {
        if (fill)
            fillTriangle(triangle.x[0], triangle.y[0], triangle.x[1], triangle.y[1], triangle.x[2], triangle.y[2], color);
        else
            drawTriangle(triangle.x[0], triangle.y[0], triangle.x[1], triangle.y[1], triangle.x[2], triangle.y[2], color);
    }


    

    // global instance

    /*
        data/command (labeled "A0") = A0/PC0
        Reset = A1/PC1
        cable select = A2/PC2

        (SPI on the Nano V3: MOSI = D11/PB3, MISO = D12/PB4, SCK = D13/PB5)
        The TFT only receives data -> "SCK" = D13; "SDA" = MOSI = D11
    */
    display_ST7735 display(A2, A0, A1);    

    static const int16_t 
        _bubbleCenterX = display.width() / 2,
        _bubbleCenterY = display.height() - _bubbleCenterX,
        _bubbleRadius = DISPLAY_BUBBLE_RADIUS / 4,
        _bubbleCrossLen = DISPLAY_BUBBLE_RADIUS / 2,
        _barLength = display.width() - DISPLAY_BAR_WIDTH,
        _barIndicatorWidth = 16,
        _barEffectiveLength = _barLength - _barIndicatorWidth - 4,  // add an offset, so the frame doesn't get deleted by removing the previous triangles (prevent overlap)
        _barCenterShortSide = (_barLength / 2) + DISPLAY_BAR_WIDTH,
        _barCenterLongSide = (display.height() - _barCenterShortSide);

    static const t_trianglePoints
        _longSideCenterTriangle =
        {
            {DISPLAY_BAR_WIDTH - 2, 1, 1},  // x0 .. x2
            {_barCenterLongSide, (_barCenterLongSide - (DISPLAY_BAR_WIDTH / 2)), (_barCenterLongSide + (DISPLAY_BAR_WIDTH / 2))}    // y0 .. y2
        },
        _shortSideCenterTriangle =
        {
            {_barCenterShortSide, (_barCenterShortSide - (DISPLAY_BAR_WIDTH / 2)), (_barCenterShortSide + (DISPLAY_BAR_WIDTH / 2))},
            {display.height() - DISPLAY_BAR_WIDTH + 1, display.height() - 2, display.height() - 2}
        };


    void display_ST7735::setup(void)
    {
        pinMode(_backlight_pin, OUTPUT);
        begin();
        initR(INITR_18BLACKTAB);
        fillScreen(ST7735_BLACK);
        setRotation(2);
    }
    
    void display_ST7735::setBrightness(const int brightness)
    {
        analogWrite(_backlight_pin, brightness);
    }
    
    void display_ST7735::printFrame(const uint_fast16_t contentFlags)
    {
        clear();

        if (contentFlags & bubbleLevel)
        {
            switch (_bubble.style)
            {
                case circleDot:
                    drawCircle(width() / 2, height() - width() / 2, DISPLAY_BUBBLE_RADIUS, _defaultColor);
                    break;

                case bars:
                    // long side
                    drawRect(0, height() - width(), DISPLAY_BAR_WIDTH, _barLength, _defaultColor);                              
                    _bubble.barIndicators.longSideIndicator.x[0] = DISPLAY_BAR_WIDTH;
                    _bubble.barIndicators.longSideIndicator.y[0] = _barCenterLongSide;
                    _bubble.barIndicators.longSideIndicator.x[1] = DISPLAY_BAR_WIDTH + _barIndicatorWidth;
                    _bubble.barIndicators.longSideIndicator.y[1] = _barCenterLongSide - (_barIndicatorWidth / 2);
                    _bubble.barIndicators.longSideIndicator.x[2] = _bubble.barIndicators.longSideIndicator.x[1];
                    _bubble.barIndicators.longSideIndicator.y[2] = _barCenterLongSide + (_barIndicatorWidth / 2);
                    _printTriangle(_bubble.barIndicators.longSideIndicator, _defaultColor);
                    _bubble.barIndicators.longIndicatorFilled = false;

                    // move the points so the outline stays visible
                    _bubble.barIndicators.longSideIndicator.x[0] += 1;
                    _bubble.barIndicators.longSideIndicator.x[1] -= 1;
                    _bubble.barIndicators.longSideIndicator.y[1] += 1;
                    _bubble.barIndicators.longSideIndicator.x[2] -= 1;
                    _bubble.barIndicators.longSideIndicator.y[2] -= 1;

                    _bubble.barIndicators.longSidePrev = _longSideCenterTriangle;

                    drawRect(DISPLAY_BAR_WIDTH, height() - DISPLAY_BAR_WIDTH, _barLength, DISPLAY_BAR_WIDTH, _defaultColor);    // short side
                    _bubble.barIndicators.shortSideIndicator.x[0] = _barCenterShortSide;
                    _bubble.barIndicators.shortSideIndicator.y[0] = height() - DISPLAY_BAR_WIDTH;
                    _bubble.barIndicators.shortSideIndicator.x[1] = _barCenterShortSide - (_barIndicatorWidth / 2);
                    _bubble.barIndicators.shortSideIndicator.y[1] = _bubble.barIndicators.shortSideIndicator.y[0] - _barIndicatorWidth;
                    _bubble.barIndicators.shortSideIndicator.x[2] = _barCenterShortSide + (_barIndicatorWidth / 2);
                    _bubble.barIndicators.shortSideIndicator.y[2] = _bubble.barIndicators.shortSideIndicator.y[1];
                    _printTriangle(_bubble.barIndicators.shortSideIndicator, _defaultColor);

                    // move the points so the outline stays visible
                    _bubble.barIndicators.shortSideIndicator.y[0] -= 1;
                    _bubble.barIndicators.shortSideIndicator.x[1] += 1;
                    _bubble.barIndicators.shortSideIndicator.y[1] += 1;
                    _bubble.barIndicators.shortSideIndicator.x[2] -= 1;
                    _bubble.barIndicators.shortSideIndicator.y[2] += 1;

                    _bubble.barIndicators.shortSidePrev = _shortSideCenterTriangle;
                    break;
            }
        }
        
    }
    
    void display_ST7735::updateLevel(const int16_t horizontal, const int16_t vertical)
    {
        const int16_t
            coercedH = max(-100, min(100, horizontal)),
            coercedV = max(-100, min(100, vertical));

        switch (_bubble.style)
        {
            case circleDot:
                _updateCircleDot(coercedH, coercedV);
                break;

            case bars:
                _updateBars(coercedH, coercedV);
                break;
        }
    }
    
    void display_ST7735::_updateCircleDot(const int16_t horizontal, const int16_t vertical)
    {
        const uint16_t 
            crossLenX = (vertical ? _bubbleRadius : _bubbleCrossLen),
            crossLenY = (horizontal ? _bubbleRadius : _bubbleCrossLen);

        int16_t coercedX = horizontal * (DISPLAY_BUBBLE_RADIUS / 2) / 100;
        int16_t coercedY = vertical * (DISPLAY_BUBBLE_RADIUS / 2) / 100;

        if (_bubble.dot.x || _bubble.dot.y) // both are zero on first call
        {
            // drawCircle(_bubble.dot.x, _bubble.dot.y, _bubbleRadius, _backgroundColor);  // delete the old bubble
            drawRect(_bubble.dot.x, _bubble.dot.y, _bubbleRadius, _bubbleRadius, _backgroundColor);  // delete the old bubble
            drawLine(_bubbleCenterX - _bubbleCrossLen, _bubbleCenterY, _bubbleCenterX + _bubbleCrossLen, _bubbleCenterY, _backgroundColor);    // delete the cross
            drawLine(_bubbleCenterX, _bubbleCenterY - _bubbleCrossLen, _bubbleCenterX, _bubbleCenterY + _bubbleCrossLen, _backgroundColor);
        }

        // draw a cross in the center

        drawLine(_bubbleCenterX - crossLenX, _bubbleCenterY, _bubbleCenterX + crossLenX, _bubbleCenterY, _defaultColor);
        drawLine(_bubbleCenterX, _bubbleCenterY - crossLenY, _bubbleCenterX, _bubbleCenterY + crossLenY, _defaultColor);

        // draw the new bubble
        
        _bubble.dot.x = _bubbleCenterX + coercedX;
        _bubble.dot.y = _bubbleCenterY + coercedY;

        // drawCircle(_bubble.dot.x, _bubble.dot.y, _bubbleRadius, _defaultColor);
        drawRect(_bubble.dot.x, _bubble.dot.y, _bubbleRadius, _bubbleRadius, _defaultColor);
    }
    
    void display_ST7735::_updateBars(const int16_t horizontal, const int16_t vertical)
    {
        const int16_t 
            scaledShortSideOffset = (horizontal * _barEffectiveLength) / (2 * 100),
            scaledLongSideOffset = (vertical * _barEffectiveLength) / (2 * 100);

        const bool
            fillHorizontalIndicator = abs(horizontal) < 3,
            fillVerticalIndicator = abs(vertical) < 3;

        t_trianglePoints
            shortSideTriangle = _shortSideCenterTriangle,
            longSideTriangle = _longSideCenterTriangle;

        shortSideTriangle.x[0] += scaledShortSideOffset;
        longSideTriangle.y[0] += scaledLongSideOffset;

        // fill or clear the "in level" indicators next to the bars
        if (fillHorizontalIndicator != _bubble.barIndicators.shortIndicatorFilled)
        {
            _printTriangle(_bubble.barIndicators.shortSideIndicator, (fillHorizontalIndicator ? _defaultColor : _backgroundColor), true);
            _bubble.barIndicators.shortIndicatorFilled = fillHorizontalIndicator;
        }

        if (fillVerticalIndicator != _bubble.barIndicators.longIndicatorFilled)
        {
            _printTriangle(_bubble.barIndicators.longSideIndicator, (fillVerticalIndicator ? _defaultColor : _backgroundColor), true);
            _bubble.barIndicators.longIndicatorFilled = fillVerticalIndicator;
        }

        // move the inner triangles?
        if (shortSideTriangle.x[0] != _bubble.barIndicators.shortSidePrev.x[0]) // this will not execute if the device is perfectly leveled (which is very unlikely, so no special case)
        {
            shortSideTriangle.x[1] += scaledShortSideOffset;
            shortSideTriangle.x[2] += scaledShortSideOffset;
            
            // delete the old triangle
            _printTriangle(_bubble.barIndicators.shortSidePrev, _backgroundColor, true);

            //print the new one
            _printTriangle(shortSideTriangle, _defaultColor, true);
            _bubble.barIndicators.shortSidePrev = shortSideTriangle;
        }

        if (longSideTriangle.y[0] != _bubble.barIndicators.longSidePrev.y[0]) // this will not execute if the device is perfectly leveled (which is very unlikely, so no special case)
        {
            longSideTriangle.y[1] += scaledLongSideOffset;
            longSideTriangle.y[2] += scaledLongSideOffset;
            
            // delete the old triangle
            _printTriangle(_bubble.barIndicators.longSidePrev, _backgroundColor, true);

            //print the new one
            _printTriangle(longSideTriangle, _defaultColor, true);
            _bubble.barIndicators.longSidePrev = longSideTriangle;
        }
    }




}