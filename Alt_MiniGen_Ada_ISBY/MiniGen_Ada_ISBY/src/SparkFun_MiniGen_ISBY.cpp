/****************************************************************
Controlling the Sparkfun MiniGen using an Adafruit M4 Itsybitsy

****************************************************************/
#include "SparkFun_MiniGen_ISBY.h"

uint16_t _FSYNCPin = 10;
uint16_t configReg = 0;
SPISettings mySettings;

void SPIWrite(uint16_t data)
{
  digitalWrite(_FSYNCPin, LOW);
  SPI.transfer((byte)(data >> 8));
  SPI.transfer((byte)data);
  digitalWrite(_FSYNCPin, HIGH);
  //  Serial.print("SPI:");
  //  Serial.println(data,HEX);
}

void MiniGen_reset(void)
{
  mySettings = SPISettings(40000000, MSBFIRST, SPI_MODE2);
  pinMode(_FSYNCPin, OUTPUT);  // Make the FSYCPin an output; this is analogous
  //  to chip select in most systems.
  pinMode(10, OUTPUT);
  digitalWrite(_FSYNCPin, HIGH);
  SPI.begin();

  uint32_t defaultFreq = freqCalc(200.0);
  SPI.beginTransaction(mySettings);
  adjustFreq(FREQ0, FULL, defaultFreq);
  adjustFreq(FREQ1, FULL, defaultFreq);
  adjustPhaseShift(PHASE0, 0x0000);
  adjustPhaseShift(PHASE1, 0x0000);
  SPIWrite(0x0100);
  SPIWrite(0x0000);
  // end reset
  SPI.endTransaction();  
}

void MiniGen_setFrequency(float frequency)
{
  SPI.beginTransaction(mySettings);
  uint32_t freqReg = freqCalc(frequency); // freqCalc() makes a useful 32-bit value out of the frequency value
  adjustFreq(FREQ0, freqReg); // Adjust the frequency. This is a full 32-bit write.
  SPI.endTransaction();
}

uint32_t freqCalc(float desiredFrequency)
{
    // the factor 0.0596 is the frequency resolution for the AD9837 chip
    // FR = Fmax / (-1+2^BR), Fmax = 16 MHz, BR = 28
    // For the alternative chip
    // Fmax = 25 MHz, BR = 28 => FR = 0.0931 Hz
  //return (uint32_t) (desiredFrequency / .0596);
  return (uint32_t) (desiredFrequency / .0931);
}

void adjustPhaseShift(PHASEREG reg, uint16_t newPhase)
{
  // First, let's blank the top four bits. Just because it's the right thing
  //  to do, you know?
  newPhase &= ~0xF000;
  // Now, we need to set the top three bits to properly route the data.
  //  D15:D13 = 110 for PHASE0...
  if (reg == PHASE0) newPhase |= 0xC000;
  // ... and D15:D13 = 111 for PHASE1.
  else               newPhase |= 0xE000;
  SPIWrite(newPhase);
}

void setFreqAdjustMode(FREQADJUSTMODE newMode)
{
  // Start by clearing the bits in question.
  configReg &= ~0x3000;
  // Now, adjust the bits to match the truth table above.
  switch (newMode)
  {
    case COARSE:  // D13:12 = 01
      configReg |= 0x1000;
      break;
    case FINE:    // D13:12 = 00
      break;
    case FULL:    // D13:12 = 1x (we use 10)
      configReg |= 0x2000;
      break;
  }
  SPIWrite(configReg);
}

void adjustFreq(FREQREG reg, uint32_t newFreq)
{
  // We need to split the 32-bit input into two 16-bit values, blank the top
  //  two bits of those values, and set the top two bits according to the
  //  value of reg.
  // Start by acquiring the low 16-bits...
  uint16_t temp = (uint16_t)newFreq;
  // ...and blanking the first two bits.
  temp &= ~0xC000;
  // Now, set the top two bits according to the reg parameter.
  if (reg == FREQ0) temp |= 0x4000;
  else            temp |= 0x8000;
  // Now, we can write temp out to the device.
  SPIWrite(temp);
  // Okay, that's the lower 14 bits. Now let's grab the upper 14.
  temp = (uint16_t)(newFreq >> 14);
  // ...and now, we can just repeat the process.
  temp &= ~0xC000;
  // Now, set the top two bits according to the reg parameter.
  if (reg == FREQ0) temp |= 0x4000;
  else            temp |= 0x8000;
  // Now, we can write temp out to the device.
  SPIWrite(temp);
}

void adjustFreq(FREQREG reg, FREQADJUSTMODE mode, uint32_t newFreq)
{
  setFreqAdjustMode(mode);
  // Now, we can just call the normal 32-bit write.
  adjustFreq(reg, newFreq);
}

void MiniGen_setMode(MODE newMode)
{
  // We want to adjust the three bits in the config register that we're
  //  interested in without screwing up anything else. Unfortunately, this
  //  part is write-only, so we need to maintain a local shadow, adjust that,
  //  then write it.
  configReg &= ~0x002A; // Clear D5, D3, and D1.
  // This switch statement sets the appropriate bit in the config register.
  switch (newMode)
  {
    case TRIANGLE:
      configReg |= 0x0002;
      break;
    case SQUARE_2:
      configReg |= 0x0020;
      break;
    case SQUARE:
      configReg |= 0x0028;
      break;
    case SINE:
      configReg |= 0x0000;
      break;
  }
  SPI.beginTransaction(mySettings);
  SPIWrite(configReg); // Now write our shadow copy to the part.
  SPI.endTransaction();
}