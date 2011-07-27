#include "WProgram.h"
#include <SPI.h>
#include "ad57x4r.h"
#include <Streaming.h>

#define LOOP_DELAY 100
#define DAC_CS 10

const int value_min = 0;
const int value_max = 4095;
const int value_inc = 100;

AD57X4R dac = AD57X4R(DAC_CS);
int powerControlRegister;
unsigned int value = value_min;

void setup() {

  // Setup serial and SPI communications
  Serial.begin(115200);
  SPI.setDataMode(SPI_MODE2);
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV8);
  SPI.begin();

  // Initialize DAC
  dac.init(AD57X4R::AD5724R, AD57X4R::UNIPOLAR_5V, AD57X4R::ALL);

  // Check to make sure powerControlRegister set properly
  powerControlRegister = dac.readPowerControlRegister();
  Serial << "powerControlRegister =  " << _BIN(powerControlRegister) << endl;
}


void loop() {
  Serial << "value = " << _DEC(value) << endl;
  if (value <= value_max) {
    value += value_inc;
  } else {
    value = value_min;
  }
  dac.update(value,AD57X4R::ALL);
  delay(LOOP_DELAY);
}
