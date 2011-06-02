#ifndef _SERIAL_LCD_H_
#define _SERIAL_LCD_H_
#include "WProgram.h"


class SerialLCD {
    private:
        Print *_portPtr;
    public:
        SerialLCD();
        SerialLCD(Print &port);
        void setPort(Print &port);
        void clearScreen(void);
        void setBrightness(byte value);
        void print(char *data);
        void print(char *data, byte x, byte y);
        void setX(byte x);
        void setY(byte y);
        void setPos(byte x, byte y);
        void setPixel(byte x, byte y, byte state);
        void drawLine(byte startX, byte startY, byte endX, byte endY, byte state);
        void drawBox(byte topLeftX, byte topLeftY, byte bottomRightX, byte bottomRightY, byte state);
        void drawCircle(byte startX, byte startY, byte radius, byte state);
        void drawFilledBox(byte topLeftX, byte topLeftY, byte bottomRightX, byte bottomRightY, byte state);
        void toggleAuxFont();
        void toggleRevMode(); 
        void toggleSplash();
};

#endif
