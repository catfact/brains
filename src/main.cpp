#include "WProgram.h"
#include "usb_serial.h"
#include "SPI.h"
#include "adc.h"
// use teensy lib, not arduino lib
//#include "Serial.h"

#include "serial_protocol.h"

#define BTSERIAL Serial1

// refresh rate in milliseconds
static const int PERIOD_MS = 20;

// raw data from ADC
static uint32_t adcData[NUM_CHANNELS];

static void tx_adc(void);

//--------------
// test signal
static uint32_t dacValue = 0;
static uint32_t dacValue_inc = 32;
static uint32_t dacValueMax;
static uint32_t update_count = 0;
//-------------

bool useInternalAdc = false;

void setUseInternalAdc(bool val) {
  useInternalAdc = val;
  if(val) {
    dacValueMax = (1 << 12) - 1;
  } else {
    dacValueMax  = (((1 << 12) >> 2));
  }
}

//extern "C"
int main(void)
{
  setUseInternalAdc(false);
  
  adc_init();

  BTSERIAL.begin(115200);
  
  analogWriteResolution(12);
  analogReadResolution(16); 
  
  while (1) {
    delay(PERIOD_MS);
  
    if(useInternalAdc) {
      adcData[0] = analogRead(A0);
    } else {
      // read ADC
      adc_read(0, 1, &(adcData[0]));
    }

	//------------
	// test signal
    update_count++;
    if(update_count > 16) {
      update_count = 0;
      dacValue += dacValue_inc;
      if(dacValue > dacValueMax) { dacValue = 0; }
      analogWriteDAC0( dacValue + 0x10);
    }
	//-----------
    
    // write data to serial
    tx_adc();

    // // check usb serial input
    while(usb_serial_available()) {
      int b = usb_serial_getchar();
      setUseInternalAdc((b == 0x01));
    }    
  }
}

// transmit adc data
static char tx_adc_buf[1024];
//static int count=0;
void tx_adc() {
  sprintf(tx_adc_buf, "%d\r\n", adcData[0] );
  // test
  //sprintf(tx_adc_buf, "%d\r\n", count++ );
  
  // send same output to usb serial...
  usb_serial_write((const void*)tx_adc_buf, strlen(tx_adc_buf));
  usb_serial_flush_output();
  // ... and bluetooth serial
  BTSERIAL.print(tx_adc_buf);
}
