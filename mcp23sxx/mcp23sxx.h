// ----------------------------------------------------------------------------
// mcp23sxx.h
//
// Provides an SPI based interface to the MCP23SXX
// I/O expanders with serial interface.
//
// Author: Peter Polidoro, IO Rodeo Inc.
// ----------------------------------------------------------------------------

#ifndef MCP23SXX_H_
#define MCP23SXX_H_

class MCP23SXX {
 public:
  enum resolutions {MCP23S08, MCP23S17};
  // You must specify the clock select pin
  MCP23SXX(int csPin);
  // You must specify the resolution and the clock select pin
  /* MCP23SXX(resolutions resolution, int csPin); */
  // Optionally, up to 8 devices can share the same clock select pin
  /* MCP23SXX(resolutions resolution, int csPin, byte aaa_hw_addr); */

  // You must specify the resolution
  void init(resolutions resolution);
  // Optionally, up to 8 devices can share the same clock select pin
  /* void init(resolutions resolution, byte aaa_hw_addr); */
  /* int readPowerControlRegister(); */

  // Work on all pins
  void pinMode(int mode);
  int pinMode();
  void port(word value);
  byte port();
  /* word port(); */

  // Work on individual pins 0-15
  void pinMode(int pin, int mode);
  void digitalWrite(int pin, bool value);
  int  digitalRead(int pin);

 private:
  // Private Constants
  const static int MCP23S08_PIN_COUNT = 8;
  const static int MCP23S17_PIN_COUNT = 16;

  const static byte IOCONA = 0x0A;   // Config register is here on power up
  const static byte IOCON  = 0x05;   // Config register is here after switch to BANK=1

  // Config options
  // We set BANK, that re-maps all the addresses
  const static byte BANK   = 0b10000000; // Sets BANK=1
  const static byte MIRROR = 0b01000000;
  const static byte SEQOP  = 0b00100000; // Not needed if BANK=0 (default)
  const static byte DISSLW = 0b00010000; // Not needed for SPI communication?
  const static byte HAEN   = 0b00001000; // Enable the AAA 3-bit chip select
  const static byte ODR    = 0b00000100; // Enable open drain
  const static byte INTPOL = 0b00000010; // Set interrupt polarity HIGH

  const static byte IODIRA = 0x00;
  const static byte IODIRB = 0x10;
  const static byte IODIR  = IODIRA;

  const static byte GPPUA  = 0x06;
  const static byte GPPUB  = 0x16;
  const static byte GPPU   = GPPUA;

  const static byte GPIOA  = 0x09;
  const static byte GPIOB  = 0x19;
  const static byte GPIO   = GPIOA;

  int resolution;
  int csPin;
  int pinCount;
  int pinCountMax;
  bool csInvertFlag;
  byte aaa_hw_addr;

  byte read_cmd;
  byte write_cmd;

  word mode;

  void setupCS(int csPin);
  void setupDevice(byte aaa_hw_addr);

  byte getData(byte addr);
  /* word getData(byte addr); */
  void setData(byte addr, byte data);
  /* void setData(byte addr, word data); */
};

#endif // Mcp23sxx_h



