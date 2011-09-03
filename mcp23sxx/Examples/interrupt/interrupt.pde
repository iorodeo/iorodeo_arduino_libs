#include "WProgram.h"
#include <SPI.h>
#include <mcp23sxx.h>
#include <Streaming.h>

#define LOOP_DELAY 100
#define IO_CS  3
#define IO_INT_PIN_A 5
#define IO_INT_PIN_B 6
#define INT_PIN 2

MCP23SXX IO = MCP23SXX(IO_CS);
int pinValue;
int flagRegister;

void callback() {
  Serial << "callback..." << endl;
  flagRegister = IO.getInterruptFlagRegister();
  if ((1<<(IO_INT_PIN_A-1)) & flagRegister) {
    pinValue = IO.digitalRead(IO_INT_PIN_A);
    if (pinValue == LOW) {
      Serial << "Button pressed." << endl;
    } else {
      Serial << "Button released." << endl;
    }
  }
}

void setup()
{
  // Setup serial communications
  Serial.begin(115200);

  // Setup SPI communications
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV8);
  SPI.begin();

  // Initialize IO
  IO.init(MCP23SXX::MCP23S08);

  // Initialize IO interrupt pin A as input and turn on pullup resistor
  IO.pinMode(IO_INT_PIN_A, INPUT);
  IO.digitalWrite(IO_INT_PIN_A, HIGH);

  // Enable IO interrupt pin A, compare to previous value
  IO.enableInterrupt(IO_INT_PIN_A);

  // Enable arduino interrupt
  attachInterrupt(0,callback,FALLING);
}

void loop()
{
  delay(LOOP_DELAY);
}


