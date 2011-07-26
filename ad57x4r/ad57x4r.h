// ----------------------------------------------------------------------------
// ad57x4r.h
//
// Provides an SPI based interface to the AD57X4R
// Complete, Quad, 12-/14-/16-Bit, Serial Input,
// Unipolar/Bipolar Voltage Output DACs.
//
// Author: Peter Polidoro, IO Rodeo Inc.
// ----------------------------------------------------------------------------
#ifndef _AD57X4R_H_
#define _AD57X4R_H_

class AD57X4R {
 private:
  int cs;
  struct {
    byte header;
    union {
      unsigned int unipolar;
      int bipolar;
    } data;
  } input_shift_register;
  bool unipolar;
  bool csInvertFlag;
  void setHeader(byte value, byte bit_shift, byte bit_count);
  void setReadWrite(byte value);
  void setRegisterSelect(byte value);
  void setDACAddress(byte value);
  int send_input_shift_register();
  void csEnable();
  void csDisable();
 public:
  enum output_ranges {UNIPOLAR_5V, UNIPOLAR_10V, BIPOLAR_5V, BIPOLAR_10V};
  enum channels {A, B, C, D, ALL};
  AD57X4R(int csPin);
  AD57X4R(int csPin, output_ranges output_range);
  void setOutputRange(output_ranges output_range);
  void setOutputRange(output_ranges output_range, channels channel);
  void setCSInvert();
  void setCSNormal();
};


#endif
