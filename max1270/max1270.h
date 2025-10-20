// ----------------------------------------------------------------------------
// max1270.h
//
// Provides an SPI based uint16_terface to the MAX1270 multirange data acquisition IC
// from Maxim.
//
// Author: Will Dickson, IO Rodeo Inc.
// ----------------------------------------------------------------------------
#ifndef _MAX1270_H_
#define _MAX1270_H_

constexpr uint8_t MAX1270_NUMCHAN = 8;

class MAX1270 {

    public:

        MAX1270(uint8_t csPin);
        MAX1270(uint8_t csPin, uint8_t sstrb);

        void initialize();

        void set_spi_freq(uint32_t spi_freq);
        uint32_t get_spi_freq();

        void set_bipolar();
        void set_bipolar(uint8_t chan);

        void set_unipolar();
        void set_unipolar(uint8_t chan);

        void set_range_5V();
        void set_range_5V(uint8_t chan);

        void set_range_10V();
        void set_range_10V(uint8_t chan);
        
        void set_internal_clock();
        void set_external_clock();

        int16_t  sample(uint8_t chan);
        void sample_all(int16_t value[]);

        float sample_volts(uint8_t chan);
        void sample_all_volts(float value[]);

        uint8_t num_chan();

        void set_use_sstrb(bool value);
        bool get_use_sstrb();
        uint8_t sstrb_pin();

    private:
        uint8_t cs_ = 2;                 // Chip select DIO
        uint8_t sstrb_ = 4;              // Sample strobe DIO pin
        bool use_sstrb_ = false;         // Whether or not sstrb is used
        uint32_t spi_freq_ = 2000000;  // SPI frequency 
                                                  //
        uint8_t chan_range_[MAX1270_NUMCHAN];     // Channel Ranges
        uint8_t chan_polarity_[MAX1270_NUMCHAN];  // Channel Polarity
        uint8_t mode_;                            // Clock selection
                                                
        uint8_t get_chan_mask(uint8_t chan);
        uint8_t get_ctl_byte(uint8_t chan);
};

#endif
