#include "WProgram.h"
#include "usb_serial.h"
#include "SPI.h"
#include "adc.h"
//#include "Serial.h"

#include "serial_protocol.h"

#define BTSERIAL Serial1

static const int PERIOD_MS = 200;

// raw data from ADC
static uint32_t adcData[NUM_CHANNELS];

static void tx_adc(void);

static uint32_t dacValue = 0;
static uint32_t dacValue_inc = 32;

static const uint32_t DAC_VALUE_MAX = (((1 << 12) >> 2));
static uint32_t update_count = 0;

bool useInternalAdc = true;

//extern "C"
int main(void)
{ 
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

    update_count++;
    if(update_count > 16) {
      update_count = 0;
      // write test data to DAC
      dacValue += dacValue_inc;
      if(dacValue > DAC_VALUE_MAX) { dacValue = 0; }
      analogWriteDAC0( dacValue + 0x10);
    }
    
    // write data to serial
    tx_adc();

    // 
    
  }
}

// transmit adc data
static char tx_adc_buf[1024];

void tx_adc() {
  sprintf(tx_adc_buf, "%08x\r\n", adcData[0] );

  // send same output to usb serial...
  usb_serial_write((const void*)tx_adc_buf, strlen(tx_adc_buf));
  usb_serial_flush_output();
  // ... and bluetooth serial
  BTSERIAL.print(tx_adc_buf);
}
