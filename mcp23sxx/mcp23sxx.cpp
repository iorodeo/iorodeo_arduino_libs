// ----------------------------------------------------------------------------
// mcp23sxx.h
//
// Provides an SPI based interface to the MCP23SXX
// I/O expanders with serial interface.
//
// For the cmd, AAA is the 3-bit MCP23S17 device hardware address.
// Useful for letting up to 8 chips sharing same SPI Chip select
// #define MCP23S17_READ  B0100AAA1
// #define MCP23S17_WRITE B0100AAA0
//
// The default SPI Control Register - SPCR = B01010000;
// interrupt disabled,spi enabled,msb 1st,master,clk low when idle,
// sample on leading edge of clk,system clock/4 rate (fastest).
// Enable the digital pins 11-13 for SPI (the MOSI,MISO,SPICLK)#
//
// Author: Peter Polidoro, IO Rodeo Inc.
// ----------------------------------------------------------------------------
#include "WProgram.h"
#include <Spi.h>
#include "mcp23sxx.h"

//---------- constructor ----------------------------------------------------

MCP23SXX::MCP23SXX(resolutions resolution, int csPin) {
  this->resolution = resolution;
  setupCS(csPin);
  setupDevice(0b000);
}

MCP23SXX::MCP23SXX(resolutions resolution, int csPin, byte aaa_hw_addr) {
  // Set the aaa hardware address for this chip by tying the
  // MCP23SXX's pins (A0, A1, and A2) to either 5v or GND.
  this->resolution = resolution;
  setupCS(csPin);
  setupDevice(aaa_hw_addr);
}

//------------------ private -----------------------------------------------

// ----------------------------------------------------------------------------
// MCP23SXX::setupCS
//
// Set chip select pin for SPI Bus, and start high (disabled)
// ----------------------------------------------------------------------------
void MCP23SXX::setupCS(int csPin) {
  csInvertFlag = false;
  ::pinMode(csPin,OUTPUT);
  ::digitalWrite(csPin,HIGH);
  this->csPin = csPin;
}

void MCP23SXX::setupDevice(byte aaa_hw_addr) {
  this->aaa_hw_addr = aaa_hw_addr;
  this->read_cmd  = 0b01000000 | aaa_hw_addr<<1 | 1<<0; // MCP23SXX_READ  = B0100AAA1
  this->write_cmd = 0b01000000 | aaa_hw_addr<<1 | 0<<0; // MCP23SXX_WRITE = B0100AAA0

  // If MCP23S17, set BANK=1 so addresses will be the same as MCP23S08
  if (resolution == MCP23S17) {
    setData(IOCONA, BANK);
  }

  // Enable MIRROR, SEQOP, HAEN
  setData(IOCON, (getData(IOCON) | MIRROR | SEQOP | HAEN));
}

byte MCP23SXX::getData(byte addr) {
  byte data;
  ::digitalWrite(csPin, LOW);
  Spi.transfer(read_cmd);
  Spi.transfer(addr);
  data = Spi.transfer(0x0/*dummy data for read*/);
  ::digitalWrite(csPin, HIGH);
  return data;
}

word MCP23SXX::getData(byte addr) {
  byte low_byte;
  byte high_byte;
  ::digitalWrite(csPin, LOW);
  Spi.transfer(read_cmd);
  Spi.transfer(addr);
  low_byte  = Spi.transfer(0x0/*dummy data for read*/);
  high_byte = Spi.transfer(0x0/*dummy data for read*/);
  ::digitalWrite(csPin, HIGH);
  return word(high_byte,low_byte);
}

void MCP23SXX::setData(byte addr, byte data) {
  ::digitalWrite(csPin, LOW);
  Spi.transfer(write_cmd);
  Spi.transfer(addr);
  Spi.transfer(data);
  ::digitalWrite(csPin, HIGH);
}

void MCP23SXX::setData(byte addr, word data) {
  ::digitalWrite(csPin, LOW);
  Spi.transfer(write_cmd);
  Spi.transfer(addr);
  Spi.transfer(lowByte(data));
  Spi.transfer(highByte(data));
  Spi.transfer(data);
  ::digitalWrite(csPin, HIGH);
}

//---------- public ----------------------------------------------------

void MCP23SXX::pinMode(bool mode) {
  byte input_pins;
  if(mode == OUTPUT) {
    input_pins = 0x00;
  } else {
    input_pins = 0xFF;
  }

  if (resolution == MCP23S17) {
    setData(IODIRA, input_pins);
    setData(IODIRB, input_pins);
  } else if (resolution == MCP23S08) {
    setData(IODIR, input_pins);
  }
  this->mode = mode;
}

void MCP23SXX::port(bool value) {
  if (mode == INPUT) {
    byte GPPU_;
    if (resolution == MCP23S08) {
      GPPU_ = GPPU;
    } else if (pin < 8) {
      GPPU_ = GPPUA;
    } else {
      GPPU_ = GPPUB;
    }

    if (value == HIGH) {
      if (resolution == MCP23S08) {
        setData(GPPU, 0xFF);
      } else {
        setData(GPPUA, 0xFF);
        setData(GPPUB, 0xFF);
      }
    } else if (value == LOW) {
      if (resolution == MCP23S08) {
        setData(GPPU, 0x00);
      } else {
        setData(GPPUA, 0x00);
        setData(GPPUB, 0x00);
      }
    }
  }
}

void MCP23SXX::port(byte value) {
  if (resolution == MCP23S17) {
    setData(GPIO,(word)value);
  } else {
    setData(GPIO,value);
  }
}

void MCP23SXX::port(word value) {
  if (resolution == MCP23S08) {
    setData(GPIO,(byte)value);
  } else {
    setData(GPIO,value);
  }
}

byte MCP23SXX::port() {
  return getData(GPIO);
}

word MCP23SXX::port() {
  return getData(GPIO);
}

void MCP23SXX::pinMode(int pin, bool mode) {
  byte IODIR_;
  if (resolution == MCP23S08) {
    IODIR_ = IODIR;
  } else if (pin < 8) {
    IODIR_ = IODIRA;
  } else {
    IODIR_ = IODIRB;
  }

  if (mode == INPUT) {
    setData(IODIR_, getData(IODIR_) | 1<<pin );
  } else {
    setData(IODIR_, getData(IODIR_) & ~(1<<pin) );
  }
  this->mode = mode;
}

void MCP23SXX::digitalWrite(int pin, bool value) {
  if (mode == OUTPUT) {
    byte GPIO_;
    if (resolution == MCP23S08) {
      GPIO_ = GPIO;
    } else if (pin < 8) {
      GPIO_ = GPIOA;
    } else {
      GPIO_ = GPIOB;
    }

    if (value) {
      setData(GPIO_, getData(GPIO_) | 1<<pin );
    } else {
      setData(GPIO_, getData(GPIO_) & ~(1<<pin) );
    }
  } else if (mode == INPUT) {
    byte GPPU_;
    if (resolution == MCP23S08) {
      GPPU_ = GPPU;
    } else if (pin < 8) {
      GPPU_ = GPPUA;
    } else {
      GPPU_ = GPPUB;
    }

    if (value) {
      setData(GPPU_, getData(GPPU_) | 1<<pin );
    } else {
      setData(GPPU_, getData(GPPU_) & ~(1<<pin) );
    }
  }
}

int MCP23SXX::digitalRead(int pin) {
  byte GPIO_;
  byte offset=0;
  if (resolution == MCP23S08) {
    GPIO_ = GPIO;
  } else if (pin < 8) {
    GPIO_ = GPIOA;
  } else {
    GPIO_ = GPIOB;
    offset = 8;
  }
  (int)(getData(GPIO_) & 1<<(pin-offset));
}



