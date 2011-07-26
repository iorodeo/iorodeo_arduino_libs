// ----------------------------------------------------------------------------
// ad57x4r.h
//
// Provides an SPI based interface to the AD57X4R
// Complete, Quad, 12-/14-/16-Bit, Serial Input,
// Unipolar/Bipolar Voltage Output DACs.
//
// Author: Peter Polidoro, IO Rodeo Inc.
// ----------------------------------------------------------------------------
#include "WProgram.h"
#include "SPI.h"
#include "ad57x4r.h"

// Read/Write Bit
#define READ                 1
#define WRITE                0
#define READ_WRITE_BIT_SHIFT 7
#define READ_WRITE_BIT_COUNT 1

// Register Select Bits
#define REGISTER_SELECT_DAC                 0b000
#define REGISTER_SELECT_OUTPUT_RANGE_SELECT 0b001
#define REGISTER_SELECT_POWER_CONTROL       0b010
#define REGISTER_SELECT_CONTROL             0b011
#define REGISTER_SELECT_BIT_SHIFT           3
#define REGISTER_SELECT_BIT_COUNT           3

// DAC Address Bits
#define DAC_ADDRESS_A         0b000
#define DAC_ADDRESS_B         0b001
#define DAC_ADDRESS_C         0b010
#define DAC_ADDRESS_D         0b011
#define DAC_ADDRESS_ALL       0b100
#define DAC_ADDRESS_BIT_SHIFT 0
#define DAC_ADDRESS_BIT_COUNT 3

// Output Ranges
#define OUTPUT_RANGE_UNIPOLAR_5V  0b000
#define OUTPUT_RANGE_UNIPOLAR_10V 0b001
#define OUTPUT_RANGE_BIPOLAR_5V   0b011
#define OUTPUT_RANGE_BIPOLAR_10V  0b100

// ----------------------------------------------------------------------------
// AD57X4R::AD57X4R
//
// Constructor
// ----------------------------------------------------------------------------
AD57X4R::AD57X4R(int csPin) {
  cs = csPin;
  csInvertFlag = false;
  pinMode(cs,OUTPUT);
  digitalWrite(cs,HIGH);
  // set defaults
  input_shift_register.header = 0;
  input_shift_register.data.unipolar = 0;
  unipolar = true;
  setOutputRange(UNIPOLAR_5V);
}

// ----------------------------------------------------------------------------
// AD57X4R::setHeader
//
// Set header bits
// ----------------------------------------------------------------------------
void AD57X4R::setHeader(byte value, byte bit_shift, byte bit_count) {
  byte bit_mask = 0;
  for (byte bit=0; bit<bit_count; bit++) {
    bitSet(bit_mask,(bit+bit_shift));
  }
  byte header = input_shift_register.header;
  header &= ~bit_mask;
  value = value << bit_shift;
  header |= value;
  input_shift_register.header = header;
}

// ----------------------------------------------------------------------------
// AD57X4R::setReadWrite
//
// Set header read/write bit to value
// ----------------------------------------------------------------------------
void AD57X4R::setReadWrite(byte value) {
  setHeader(value,READ_WRITE_BIT_SHIFT,READ_WRITE_BIT_COUNT);
}

// ----------------------------------------------------------------------------
// AD57X4R::setRegisterSelect
//
// Set header register select bits
// ----------------------------------------------------------------------------
void AD57X4R::setRegisterSelect(byte value) {
  setHeader(value,REGISTER_SELECT_BIT_SHIFT,REGISTER_SELECT_BIT_COUNT);
}

// ----------------------------------------------------------------------------
// AD57X4R::setDACAddress
//
// Set header DAC address bits
// ----------------------------------------------------------------------------
void AD57X4R::setDACAddress(byte value) {
  setHeader(value,DAC_ADDRESS_BIT_SHIFT,DAC_ADDRESS_BIT_COUNT);
}

// ----------------------------------------------------------------------------
// AD57X4R::csEnable
//
// Enable spi communications
// ----------------------------------------------------------------------------
void AD57X4R::csEnable() {
  if (csInvertFlag == false) {
    digitalWrite(cs,LOW);
  }
  else {
    digitalWrite(cs,HIGH);
  }
}

// ----------------------------------------------------------------------------
// AD57X4R::csDisable
//
// Disable spi communications
// ----------------------------------------------------------------------------
void AD57X4R::csDisable() {
  if (csInvertFlag == false) {
    digitalWrite(cs,HIGH);
  }
  else {
    digitalWrite(cs,LOW);
  }
}

// ----------------------------------------------------------------------------
// AD57X4R::send16BitCmd
//
// Sends a 16 bit command with the given addr and data
// ----------------------------------------------------------------------------
int AD57X4R::send16BitCmd(uint8_t addr, uint8_t cmd, int data) {
  uint8_t outByte0;
  uint8_t outByte1;
  uint8_t inByte0;
  uint8_t inByte1;
  int data9Bits;
  int retData;

  // Enable SPI communication
  csEnable();

  // Restrict data to 9 bits
  data9Bits = (data & 0b0000000111111111);

  // Create and send command bytes
  outByte0 =  (addr & 0b00001111) << 4;
  outByte0 |= (cmd  & 0b00000011) << 2;
  outByte0 |= (uint8_t) (data9Bits >> 8);
  outByte1 = (uint8_t)( data9Bits & 0b11111111);
  inByte0 = SPI.transfer(outByte0);
  inByte1 = SPI.transfer(outByte1);

  // Disable SPI communication
  csDisable();

  // Packup return data
  inByte0 = (inByte0 & 0b00000001);
  retData =  (((int) inByte0)<< 8) | (inByte1);
  return retData;
}

// ----------------------------------------------------------------------------
// AD57X4R::send8BitCmd
//
// Sends a 8 bit command with the given address and data
// ----------------------------------------------------------------------------
void AD57X4R::send8BitCmd(uint8_t addr, uint8_t cmd) {
  uint8_t byte;

  // Enable SPI communication
  csEnable();

  // Create and send command byte
  byte = (addr & 0b00001111) << 4;
  byte |= (cmd & 0b00000011) << 2;
  SPI.transfer(byte);

  // Disable SPI communication
  csDisable();
}

// ---------------------------------------------------------------------------
// AD57X4R::initialize
//
// Initializes the TCON register which connects terminals A, B and wipers for
// both potentiometers.
// ---------------------------------------------------------------------------
void AD57X4R::initialize() {
  uint8_t byte0;
  uint8_t byte1;
  // Enable SPI communication
  digitalWrite(cs,LOW);
  // Send command
  send16BitCmd(ADDR_TCON_REGISTER, CMD_WRITE_DATA, 0b0000000111111111);
  // Disable SPI communication
  digitalWrite(cs,HIGH);
}

// ---------------------------------------------------------------------------
// AD57X4R::setOutputRange
//
// Sets the output range
// ---------------------------------------------------------------------------
void AD57X4R::setOutputRange(output_ranges output_range) {
  setReadWrite(WRITE);
  setRegisterSelect(REGISTER_SELECT_OUTPUT_RANGE_SELECT);
  setDACAddress(DAC_ADDRESS_ALL);
}

// ----------------------------------------------------------------------------
// AD57X4R::setCSInvert
//
// use inverted chip select - high to enable
// ----------------------------------------------------------------------------
void AD57X4R::setCSInvert() {
  csInvertFlag = true;
}

// ----------------------------------------------------------------------------
// AD57X4R::setCSNormal
//
// use normal chip select - low to enable
// ----------------------------------------------------------------------------
void AD57X4R::setCSNormal() {
  csInvertFlag = false;
}
