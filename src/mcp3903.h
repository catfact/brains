#include "Arduino.h"
#include "SPI.h"

class MCP3903 {
  
 public:
  //command byte
  static const byte DEVICE_ADDR = 0x40;

  //***** register map *****
  //readonly channel registers 
  //24bit MSB first, left justified
  static const byte REG_CH_0 = 0x00;
  static const byte REG_CH_1 = 0x01;
  static const byte REG_CH_2 = 0x02;
  static const byte REG_CH_3 = 0x03;
  static const byte REG_CH_4 = 0x04;
  static const byte REG_CH_5 = 0x05;

  //Delta-sigma modulators output value 24bit
  //bit 23-20 COMPn_CH5
  //bit 19-16 COMPn_CH4
  //bit 15-12 COMPn_CH3
  //bit 11-8  COMPn_CH2
  //bit 7-4   COMPn_CH1
  //bit 3-9   COMPn_CH0
  //default 0011 0011 0011 0011 0011 0011
  static const byte REG_MOD = 0x06;
	
  //phase delay config register 24bit
  //bit 23-16 CH4 relative to CH5 phase delay (2's comp positive means lag)
  //bit 15-8  CH2 relative to CH3 phase delay
  //bit 7-0   CH0 relative to CH1 phase delay
  static const byte REG_PHASE = 0x07;

  //gain config register 24bit
  //bit 23, 22, 21 CH5, bit 20 BOOST_CH5
  //bit 19 BOOST CH4, bit 18, 17, 16 CH4
  //bit 15, 14, 13 CH3, bit 12 BOOST_CH3
  //bit 11 BOOST CH2, bit 10,  9,  8 CH2
  //bit 7,  6,  5  CH1, bit 4  BOOST_CH1
  //bit 3  BOOST CH0, bit 2,  1,  0  CH0
  //
  //Channel Gain
  //111 (reserved gain = 1)
  //110 (reserved gain = 1)
  //101 gain = 32
  //100 gain = 16
  //011 gain = 8
  //010 gain = 4
  //001 gain = 2
  //000 gain = 1
  //
  //BOOST 
  //1 (channel current x2)
  //0 (channel current normal)
  static const byte REG_GAIN = 0x08;

  static const byte GAIN_1 = 0x0;
  static const byte GAIN_2 = 0x1;
  static const byte GAIN_4 = 0x2;
  static const byte GAIN_8 = 0x3;
  static const byte GAIN_16 = 0x4;
  static const byte GAIN_32 = 0x5;
	
  //status/communication register 24bit
  //DEFAULT: 0x804000
  //bit 23-22 Address Loop Setting
  //11 = cycle through entire reg map
  //10 = loop through reg TYPES (default)
  //01 = loop through reg GROUPS
  //00 = continue read single reg (not incremented)
  //
  //bit 21: Write mode bit (internal use only)
  //1 = static addressing write mode
  //0 = incremental addressing write mode (default)
  //
  //bit 20-15 WIDTH_CHn
  //1 = 24 bit 
  //0 = 16 bit (default)
  //
  //bit 14 DR_LTY (data ready latency control for DRA, DRB and DRC pins)
  //1 = no latency conversion, data ready pulses after 3 DRCLK periods (default)
  //0 = unsettled data is available after every DRCLK period
  //
  //bit 13 DR_HIZ: data ready pin inactive state control for DRA, DRB and DRC
  //1 = the default state is a logic high when data is NOT ready
  //0 = the default state is a high impedance when data is not ready (default)
  //
  //bit 12 DR_LINK (data ready link control
  //1 = data ready link on, all channels linked and data ready pulses from the most lagging ADC
  //	are present on each DR pin
  //0 = Data ready link off (default)
  //
  //bit 11-10 DRC_MODE
  //11 = both data ready pulses from CH4 CH5 are output on DRC pin
  //10 = data ready pulses from CH5 are output on DRC data ready pulse from CH4 not present
  //01 = data ready pulses from CH4 are output on DRC data ready pulse from CH5 not present
  //00 = data ready pulses from lagging ADC channel are output on DRC
  //
  //bit 9-8 DRB_MODE (CH3, CH2 see DRC_MODE)
  //bit 7-6 DRA_MODE (CH1, CH0 see DRC_MODE)
  //bit 5-0 DRSTATUS_CHn (Data Ready Status)
  //1 = data not ready (default)
  //0 = data ready
  //  static const byte REG_STATUS_COMM = 0x09;
  static const byte REG_STATUS_COMM = 0b1001;

  //config register 24bit
  //DEFAULT: 0x000FD0
  //bit 23-18: RESET_CHn
  //1 = reset mode for channel n ON
  //0 = reset mode for channel n off (default)
  //
  //bit 17-12 SHUTDOWN_CHn
  //1 = shutdown mode for channel n on
  //0 = shutdown mode for channel n off (default)
  //
  //bit 11-6 DITHER_CHn 
  //1 = dithering channel n on (default)
  //0 = dithering channel n off 
  //
  //bit 5-4 OSR (over sampling ratio)
  //11 = 256
  //10 = 128
  //01 = 64 (default)
  //00 = 32
  //
  //bit 3-2 PRESCALE
  //11 = AMCLK = MCLK/8
  //10 = AMCLK = MCLK/4
  //01 = AMCLK = MCLK/2
  //00 = AMCLK = MCLK (default)
  //
  //bit 1 EXTVREF (internal reference shutdown control)
  //1 = internal Vref disabled
  //0 = internal Vref enabled (default)
  //
  //bit 0 EXTCLK (clock mode)
  //1 = CLOCK mode (internal osc disabled)
  //0 = XT mode (crystal between OSC1/OSC2) (default)
  static const byte REG_CONFIG = 0x0A;
  //***************************

  //loop chn0/1, chn2/3, chn4/5
  // MOD/PHASE/GAIN, STAT/CONFIG
  static const byte READ_OPTION_GROUP = 0b00000001;

  // loop chn0-5, MOD/PHASE/GAIN/STAT/CONFIG
  static const byte READ_OPTION_TYPE = 0b00010000;
	
  //loop through entire reg map
  static const byte READ_OPTION_ALL = 0b00010001;

  static const byte OSR_32 = 0x0;
  static const byte OSR_64 = 0x1;
  static const byte OSR_128 = 0x2;
  static const byte OSR_256 = 0x3;


//----------------
//---- methods

  static void reset(int part);
  static void reset(int part, byte osr);
  static unsigned long readRegister(int part, byte reg);
  static void writeRegister(int part, byte reg, unsigned long data);
  //  static unsigned long readControlRegister(int part);
  //  static unsigned long readStatusCommRegister(int part);
  static double readADC(int part, byte channel);
  static void setGain(int part, byte channel, byte gain);
  static void setGain(int part, byte channel, byte gain, byte boost);



};
