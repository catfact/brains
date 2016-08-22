#ifndef _SLABS_TEENSY_ADC_H_
#define _SLABS_TEENSY_ADC_H_

#include "SPI.h"

#define MCP3903_EXT_CLK 1

class AdcPins {
public:

#ifdef MCP3903_EXT_CLK
  static const int MCLK = 6;
#endif
  
  static const int MOSI = 11;
  static const int MISO = 12;
  static const int CLK = 13;
  
  static const int CS0 = 16;
  static const int CS1 = 17;
// TODO: assign GPIO for other chip selects...

  // group chip select pins
  static const int NUM_PARTS = 2;
  static const int CS_PINS[NUM_PARTS]; 
  static const int cs(int part) { return CS_PINS[part]; }
};

class AdcConfig {
public:
  static const int MCLK_HZ = 4000000;
  static const int SPI_HZ = 8000000;
  static const int SPI_ORDER = MSBFIRST;
  static const int SPI_MODE = SPI_MODE0;
};


extern void adc_init(void);

extern void adc_read(int startChannel, int numChannels, uint32_t* data);

#endif
