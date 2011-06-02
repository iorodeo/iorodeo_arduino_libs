#include "SerialLCD.h"

SerialLCD::SerialLCD() {
}

SerialLCD::SerialLCD(Print &port) {
    setPort(port);
}

void SerialLCD::setPort(Print &port) {
    _portPtr = &port;
}

void SerialLCD::clearScreen(void) {
    // Clear the LCD display sets x and y offsets to 0,0
    _portPtr -> print(0x7C,BYTE);
    _portPtr -> print(0x00,BYTE);
}

void SerialLCD::print(char *data) {
    // Print data to the LCD
    _portPtr -> print(data);
}

void SerialLCD::print(char *data, byte x, byte y) {
    // Print data at position given by x and y.
    setPos(x,y);
    print(data);
}

void SerialLCD::setBrightness(byte value) {
    // Sets the LCD backlight brightness - adjusts duty cycle.
    // value should be between 0 and 100.
    _portPtr -> print(0x7C,BYTE);
    _portPtr -> print(0x02,BYTE);
    _portPtr -> print(value,BYTE);
}

void SerialLCD::setX(byte x) {
    // Set the x coordinate for where the next character will be drawn
    _portPtr -> print(0x7C,BYTE);
    _portPtr -> print(0x18,BYTE);
    _portPtr -> print(x,BYTE);
}

void SerialLCD::setY(byte y) {
    // Set the y coordinate for where the next character will be drawn
    _portPtr -> print(0x7C,BYTE);
    _portPtr -> print(0x19,BYTE);
    _portPtr -> print(y,BYTE);
}

void SerialLCD::setPos(byte x, byte y) {
    // Set x and y coordinates for where next character will be drawn
    setX(x);
    setY(y);
}

void SerialLCD::setPixel(byte x, byte y, byte state) {
    // Set pixel and coordinate x,y to value given by state.
    _portPtr -> print(0x7C,BYTE);
    _portPtr -> print(0x10,BYTE);
    _portPtr -> print(x,BYTE);
    _portPtr -> print(y,BYTE);
    _portPtr -> print(state,BYTE);
}

void SerialLCD::drawLine(
        byte startX, 
        byte startY, 
        byte endX, 
        byte endY, 
        byte state
        )
{
    _portPtr -> print(0x7C,BYTE);
    _portPtr -> print(0x0C,BYTE);
    _portPtr -> print(startX,BYTE);
    _portPtr -> print(startY,BYTE);
    _portPtr -> print(endX,BYTE);
    _portPtr -> print(endY,BYTE);
    _portPtr -> print(state,BYTE);
}

void SerialLCD::drawBox(
        byte topLeftX, 
        byte topLeftY, 
        byte bottomRightX, 
        byte bottomRightY, 
        byte state
        )
{
    _portPtr -> print(0x7C,BYTE);
    _portPtr -> print(0x0F,BYTE);
    _portPtr -> print(topLeftX,BYTE);
    _portPtr -> print(topLeftY,BYTE);
    _portPtr -> print(bottomRightX,BYTE);
    _portPtr -> print(bottomRightY,BYTE);
    _portPtr -> print(state,BYTE);
}

void SerialLCD::drawCircle(
        byte startX, 
        byte startY, 
        byte radius, 
        byte state
        )
{
    _portPtr -> print(0x7C,BYTE);
    _portPtr -> print(0x03,BYTE);
    _portPtr -> print(startX,BYTE);
    _portPtr -> print(startY,BYTE);
    _portPtr -> print(radius,BYTE);
    _portPtr -> print(state,BYTE);
}

void SerialLCD::drawFilledBox(
        byte topLeftX, 
        byte topLeftY, 
        byte bottomRightX, 
        byte bottomRightY, 
        byte state
        )
{
    _portPtr -> print(0x7C,BYTE);
    _portPtr -> print(0x12,BYTE);
    _portPtr -> print(topLeftX,BYTE);
    _portPtr -> print(topLeftY,BYTE);
    _portPtr -> print(bottomRightX,BYTE);
    _portPtr -> print(bottomRightY,BYTE);
    _portPtr -> print(state,BYTE);
}

void SerialLCD::toggleAuxFont() {
    _portPtr -> print(0x7C,BYTE);
    _portPtr -> print(0x08,BYTE);
}

void SerialLCD::toggleRevMode() {
    _portPtr -> print(0x7C,BYTE);
    _portPtr -> print(0x14,BYTE);
}

void SerialLCD::toggleSplash() {
    _portPtr -> print(0x7C,BYTE);
    _portPtr -> print(0x15,BYTE);
}

