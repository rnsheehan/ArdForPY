/****************************************************************
Controlling the Sparkfun MiniGen using an Adafruit M4 Itsybitsy

****************************************************************/

#ifndef SparkFun_MiniGen_ISBY_h
#define SparkFun_MiniGen_ISBY_h

#include <Arduino.h>
#include <SPI.h>

enum MODE {TRIANGLE, SINE, SQUARE, SQUARE_2};
enum FREQREG {FREQ0, FREQ1};
enum PHASEREG {PHASE0, PHASE1};
enum FREQADJUSTMODE {FULL, COARSE, FINE};

void SPIWrite(uint16_t data);
uint32_t freqCalc(float desiredFrequency);
void adjustFreq(FREQREG reg, uint32_t newFreq);
void adjustFreq(FREQREG reg, FREQADJUSTMODE mode, uint32_t newFreq);
void adjustPhaseShift(PHASEREG reg, uint16_t newPhase);
void setFreqAdjustMode(FREQADJUSTMODE newMode);
void MiniGen_setMode(MODE newMode);
void MiniGen_setFrequency(float frequency);
void MiniGen_reset(void);

#endif