#include <Spi.h>
#include "WProgram.h"
#include <mcp23sxx.h>
#include <Streaming.h>

#define IO_CS  3

MCP23SXX IO = MCP23SXX(MCP23SXX::MCP23S08, IO_CS);

void setup()
{
  // Set all pins to be inputs
  IO.pinMode(INPUT);

  // Set pullup resistors on all pins
  IO.port(HIGH);

}

// Cycle the output lines at a staggered rate, pin/2 milliseconds
long timeoutInterval = 1;
long previousMillis = 0;
uint16_t counter = 0x0000;

void timeout()
{
  Mcp23s17.port(counter++);
}  

void loop()
{
  // Change all pins at once, 16-bit value
  Mcp23s17.port(0x0F0F);

  // Write to individual pins
  Mcp23s17.digitalWrite(8,LOW);
  Mcp23s17.digitalWrite(12,HIGH);

  // Read all pins at once, 16-bit value
  uint16_t pinstate = Mcp23s17.port();

  // Set pin 14 (GPIO B6) to be an input
  Mcp23s17.pinMode(14,INPUT);

  // Read the value of pin 14
  Mcp23s17.digitalRead(14);
  // handle timeout function, if any
  if (  millis() - previousMillis > timeoutInterval )
  {
    timeout();
    previousMillis = millis();
  }
  // Loop.
}


