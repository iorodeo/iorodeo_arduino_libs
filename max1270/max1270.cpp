// ----------------------------------------------------------------------------
// max1270.cpp
//
// Provides an SPI based uint8_terface to the MAX1270 multirange data acquisition IC
// from Maxim.
//
// Author: Will Dickson, IO Rodeo Inc.
// ----------------------------------------------------------------------------
#if defined(ARDUINO) && ARDUINO >= 100
    #include "Arduino.h"
#else
    #include "WProgram.h"
#endif
#include "SPI.h"
#include "max1270.h"
#include "Streaming.h"

// Definitions
const uint8_t CLK_INTERNAL  = 0b00000000;
const uint8_t CLK_EXTERNAL  = 0b00000001;
const uint8_t RANGE_5V      = 0b00000000;
const uint8_t RANGE_10V     = 0b00001000;
const uint8_t BIPOLAR       = 0b00000100;
const uint8_t UNIPOLAR      = 0b00000000;
const uint8_t CTL_BYTE_INIT = 0b10000000;

const uint32_t NO_SSTRB_CONVERSION_WAIT_US = 20;

// LSB to voltages for the different gain and polarity combinations
const float LSB2VOLTS_5V_BIPOLAR   = 0.00244140625;
const float LSB2VOLTS_5V_UNIPOLAR  = 0.001220703125;
const float LSB2VOLTS_10V_BIPOLAR  = 0.0048828125;
const float LSB2VOLTS_10V_UNIPOLAR = 0.00244140625;


// ----------------------------------------------------------------------------
// MAX1270::MAX1270
//
// Constructor - when not using sstrb
// ----------------------------------------------------------------------------
MAX1270::MAX1270(uint8_t cs_pin) {
    mode_ = 0;
    cs_ = cs_pin;
    use_sstrb_ = false;
}

// ----------------------------------------------------------------------------
// MAX1270::MAX1270
//
// Constructor - when using sstrb
// ----------------------------------------------------------------------------

MAX1270::MAX1270(uint8_t cs_pin, uint8_t sstrb) {
    mode_ = 0;
    cs_ = cs_pin;
    sstrb_ = sstrb;
    use_sstrb_ = true;
}

// ---------------------------------------------------------------------------
// Public Methods
// ----------------------------------------------------------------------------

void MAX1270::initialize() {
    pinMode(cs_,OUTPUT);
    digitalWrite(cs_,HIGH);
    if (use_sstrb_) {
        pinMode(sstrb_, INPUT);
    }
    // Set default polarity, range and clock
    set_bipolar();
    set_range_10V();
    set_internal_clock();
}


// ----------------------------------------------------------------------------
// MAX1270::sample
//
// Returns an analog sample for the given channel.
// ----------------------------------------------------------------------------
int16_t MAX1270::sample(uint8_t chan) {
    uint8_t ctl_byte;
    uint16_t msb;
    uint16_t lsb;
    uint16_t sgn;
    int16_t sample;

    // Enable chip SPI communications
    SPI.beginTransaction(SPISettings(spi_freq_, MSBFIRST, SPI_MODE0));
    digitalWrite(cs_,LOW);

    uint16_t count_hi = 0;
    uint16_t count_lo = 0;

    // Transfer control byte
    ctl_byte = get_ctl_byte(chan);
    SPI.transfer(ctl_byte);

    if (use_sstrb_) {
        // Wait for sstrb to go low and then high again
        while(digitalRead(sstrb_) == HIGH) {}; 
        while(digitalRead(sstrb_) == LOW)  {}; 
    }
    else {
        delayMicroseconds(NO_SSTRB_CONVERSION_WAIT_US);
    }

    // Read sample
    msb = SPI.transfer(0x00);
    lsb = SPI.transfer(0x00);

    // Convert sample from 12bit to 16bit number
    lsb = lsb >> 4;
    sgn = (0b10000000 & msb) >> 7;
    msb = msb << 4;
    if (sgn == 1) {
        msb = 0b1111000000000000 | msb;
    }
    sample = msb | lsb;

    // Disable chip SPI communications
    digitalWrite(cs_,HIGH);
    SPI.endTransaction();

    return sample;
}

// ----------------------------------------------------------------------------
// MAX1270::sample_all
//
// Gets an analog sample from all input channels
// ----------------------------------------------------------------------------
void MAX1270::sample_all(int16_t values[] ) {
    for (uint8_t i=0; i<MAX1270_NUMCHAN; i++) {
        values[i] = sample(i);
    }
}


// ----------------------------------------------------------------------------
// MAX1270::sample_volts
//
// Gets an analog sample from the given channel and converts to a floating pouint8_t
// voltage value
//
// Note, may want to move this to fixed pouint8_t instead floating pouint8_t.
// ----------------------------------------------------------------------------
float MAX1270::sample_volts(uint8_t chan) {
    int16_t value_int;
    float value_volts = 0;
    if ((chan >= 0) && (chan < MAX1270_NUMCHAN)) {
        value_int = sample(chan);
        switch (chan_range_[chan] | chan_polarity_[chan]) {

            case (RANGE_5V | BIPOLAR):
                value_volts = LSB2VOLTS_5V_BIPOLAR*value_int;
                break;

            case (RANGE_5V | UNIPOLAR):
                value_volts = LSB2VOLTS_5V_UNIPOLAR*value_int;
                break;

            case (RANGE_10V | BIPOLAR):
                value_volts = LSB2VOLTS_10V_BIPOLAR*value_int;
                break;

            case (RANGE_10V | UNIPOLAR):
                value_volts = LSB2VOLTS_10V_UNIPOLAR*value_int;
                break;

            default:
                break;
        }
    }
    return value_volts;
}

// ----------------------------------------------------------------------------
// MAX1270::sample_all_volts
//
// Gets an analog sample from all input channels and converts to a floating
// pouint8_t voltage value.
//
// Note, may want to move this to fixed pouint8_t instead of floating pouint8_t.
// ----------------------------------------------------------------------------
void MAX1270::sample_all_volts(float values[] ) {
    for (uint8_t i=0; i<MAX1270_NUMCHAN; i++) {
        values[i] = sample_volts(i);
    }
}
// ----------------------------------------------------------------------------
// MAX1270::set_internal_clock
//
// Set the MAX1270 to use its uint8_ternal clock
// ----------------------------------------------------------------------------
void MAX1270::set_internal_clock() {
    mode_ &= 0b10;
    mode_ |= CLK_INTERNAL;
}

// ----------------------------------------------------------------------------
// MAX1270::set_external_clock
//
// Set the MAX1270 to use an external clock source.
// ----------------------------------------------------------------------------
void MAX1270::set_external_clock() {
    mode_ &= 0b10;
    mode_ |= CLK_EXTERNAL;
}

// ----------------------------------------------------------------------------
// MAX1270::set_bipolar
//
// Set given the analog input channel to bipolar operation
// ----------------------------------------------------------------------------
void MAX1270::set_bipolar(uint8_t chan) {
    if ((chan >=0) && (chan < MAX1270_NUMCHAN)) {
        chan_polarity_[chan] = BIPOLAR;
    }
}

// ----------------------------------------------------------------------------
// MAX1270::set_bipolar
//
// Set all analog input channels to bipolar operation
// ----------------------------------------------------------------------------
void MAX1270::set_bipolar() {
    for (uint8_t i=0; i<MAX1270_NUMCHAN;i++) {
        set_bipolar(i);
    }
}

// ----------------------------------------------------------------------------
// MAX1270::set_unipolar
//
// Set the given analog input channel to unipolar operation
// ----------------------------------------------------------------------------
void MAX1270::set_unipolar(uint8_t chan) {
    if ((chan >= 0) && (chan < MAX1270_NUMCHAN)) {
        chan_polarity_[chan] = UNIPOLAR;
    }
}

// ----------------------------------------------------------------------------
// MAX1270::set_unipolar
//
// Set all analog input channels to unipolar operation
// ----------------------------------------------------------------------------
void MAX1270::set_unipolar() {
    for (uint8_t i=0; i<MAX1270_NUMCHAN;i++) {
        set_unipolar(i);
    }
}

// ---------------------------------------------------------------------------
// MAX1270::set_range_10V
//
// Sets the range of the given analog input channel to 10V. Note, if polarity
// is set to bipolar than range will be (-10V, 10V) and if polarity is set to
// unipolar than range will be (0,10V).
// ---------------------------------------------------------------------------
void MAX1270::set_range_10V(uint8_t chan) {
    if ((chan >= 0) && (chan < MAX1270_NUMCHAN)) {
        chan_range_[chan] = RANGE_10V;
    }
}

// --------------------------------------------------------------------------
// MAX1270::set_range_10V
//
// Sets the range of all analog inputs to 10V.  Note, if polarity  is set to
// bipolar than range will be (-10V, 10V) and if polarity is set to unipolar
// than range will be (0,10V).
// --------------------------------------------------------------------------
void MAX1270::set_range_10V() {
    for (uint8_t i=0; i<MAX1270_NUMCHAN; i++) {
        set_range_10V(i);
    }
}

// ----------------------------------------------------------------------------
// MAX1270::set_range_5V
//
// Sets the range of the given analog input channel to 5V. Note, if polarity is
// set to bipolar than range will be (-5V, 5V) and if polarity is set to unipolar
// than range will be (0,5V).
// ----------------------------------------------------------------------------
void MAX1270::set_range_5V(uint8_t chan) {
    if ((chan >= 0) && (chan < MAX1270_NUMCHAN)) {
        chan_range_[chan] = RANGE_5V;
    }
}

// ----------------------------------------------------------------------------
// MAX1270::set_range_5V
//
// Sets the range of all analog inputs to 5V. Note, if polarity  is set to
// bipolar than range will be (-5V, 5V) and if polarity is set to unipolar
// than range will be (0,5V).
// ----------------------------------------------------------------------------
void MAX1270::set_range_5V() {
    for (uint8_t i=0; i<MAX1270_NUMCHAN; i++) {
        set_range_5V(i);
    }
}

// ----------------------------------------------------------------------------
// MAX1270::num_chan
//
// Returns the number of analog input channels. Note, this is the same as the
// definition MAX1270_NUMCHAN.
// ---------------------------------------------------------------------------
uint8_t MAX1270::num_chan() {
    return MAX1270_NUMCHAN;
}

// ----------------------------------------------------------------------------
// MAX1270::set_use_sstrb
//
// Sets whether or not the sstrb pin should be used to determine when a 
// converions is complete.
// ---------------------------------------------------------------------------
void MAX1270::set_use_sstrb(bool value) {
    use_sstrb_ = value;

}

// ----------------------------------------------------------------------------
// MAX1270::set_use_sstrb
//
// Returns the current use_sstrb setting. 
// ---------------------------------------------------------------------------
bool MAX1270::get_use_sstrb() {
    return use_sstrb_;
}

// ----------------------------------------------------------------------------
// MAX1270::set_use_sstrb
//
// Returns the sstrb pin number
// ---------------------------------------------------------------------------
uint8_t MAX1270::sstrb_pin() {
    return sstrb_;
}

// ----------------------------------------------------------------------------
// MAX1270::set_spi_freq
//
// Set the SPI transfer frequency
// ---------------------------------------------------------------------------
void MAX1270::set_spi_freq(uint32_t spi_freq) {
    spi_freq_ = spi_freq;
}

// ----------------------------------------------------------------------------
// MAX1270::get_spi_freq
//
// Get the current value for the SPI transfere frequency
// ---------------------------------------------------------------------------
uint32_t MAX1270::get_spi_freq() {
    return spi_freq_;
}

// ----------------------------------------------------------------------------
// Private Methods
//-----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// MAX1270::get_chan_mask
//
// Returns control byte channel selection mask. Bits 4,5, and 6 of the control
// byte select the analog input channel.
// ----------------------------------------------------------------------------
uint8_t MAX1270::get_chan_mask(uint8_t chan) {
    uint8_t mask = 0;
    if ((chan >= 0) && (chan < MAX1270_NUMCHAN)) {
        mask = chan << 4;
    }
    return mask;
}

// ---------------------------------------------------------------------------
// MAX1270::get_ctl_byte
//
// Returns the control byte for the given channel based on the current settings
// such as range, polarity, clock selection.
// ----------------------------------------------------------------------------
uint8_t MAX1270::get_ctl_byte(uint8_t chan) {
    uint8_t ctl_byte = CTL_BYTE_INIT;
    ctl_byte |= get_chan_mask(chan);
    ctl_byte |= chan_range_[chan];
    ctl_byte |= chan_polarity_[chan];
    ctl_byte |= mode_;
    return ctl_byte;
}



