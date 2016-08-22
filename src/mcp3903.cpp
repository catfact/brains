#include "adc.h"
#include "mcp3903.h"


void MCP3903::reset(int part) {
  reset(part, OSR_256);
}

void MCP3903::reset(int part, byte osr) {
  // send reset for all channels
  unsigned long cmd1 = 0xfc0fd0;
  // send config
  unsigned long cmd2 = 0x000fc0 | osr << 4;
  writeRegister(part, REG_CONFIG, cmd1);
  writeRegister(part, REG_CONFIG, cmd2);
}

unsigned long MCP3903::readRegister(int part, byte reg) {
  byte cmdByte = DEVICE_ADDR | reg << 1 | 1;
  unsigned long r = 0;

  digitalWrite(AdcPins::cs(part), LOW);
  
  SPI.transfer(cmdByte);
  r = (unsigned long) SPI.transfer(0x0) << 16;
  r |= (unsigned long) SPI.transfer(0x0) << 8;
  r |= (unsigned long) SPI.transfer(0x0);
  digitalWrite(AdcPins::cs(part), HIGH);

  return r;
}

void MCP3903::writeRegister(int part, byte reg, unsigned long data) {
  byte cmdByte = DEVICE_ADDR | reg << 1;
	
  byte b2 = (data & 0xff0000) >> 16;
  byte b1 = (data & 0x00ff00) >> 8;
  byte b0 = data & 0x0000ff;

  digitalWrite(AdcPins::cs(part), LOW);
  SPI.transfer(cmdByte);
  SPI.transfer(b2);
  SPI.transfer(b1);
  SPI.transfer(b0);
  digitalWrite(AdcPins::cs(part), HIGH);	
}

void MCP3903::setGain(int part, byte channel, byte gain) {
  setGain(part, channel, gain, 0);
}

void MCP3903::setGain(int part, byte channel, byte gain, byte boost) {
  unsigned long r = readRegister(part, REG_GAIN);

  byte idx = channel * 4;
  unsigned long chGain = 0;

  if (channel % 2 == 0) //0, 2, 4
    {
      chGain = (boost << 3) | gain;
    }
  else //1, 3, 5 
    {
      chGain = boost | (gain << 1);
    }
	
  r &= ~(0xf << idx);
  r |= chGain << idx;

  writeRegister(part, REG_GAIN, r);
}

