#include <SPI.h>

unsigned long delay_val = 100; // delay value in units of ms
float FMIN = 20.0; // Minimum frequency setting for the board
float FMAX = 3000000.0; // Maximum frequency setting for the board 3 MHz

int val = 0;
int chan = 0;
int N = 512;
uint16_t _FSYNCPin = 10;
uint16_t configReg = 0;
SPISettings mySettings;

enum MODE {TRIANGLE, SINE, SQUARE, SQUARE_2};
enum FREQREG {FREQ0, FREQ1};
enum PHASEREG {PHASE0, PHASE1};
enum FREQADJUSTMODE {FULL, COARSE, FINE};

void SPIWrite(uint16_t data);
uint32_t freqCalc(float desiredFrequency);
//void adjustFreq(FREQREG reg, uint16_t newFreq);
void adjustFreq(FREQREG reg, uint32_t newFreq);
//void adjustFreq(FREQREG reg, FREQADJUSTMODE mode, uint16_t newFreq);
void adjustFreq(FREQREG reg, FREQADJUSTMODE mode, uint32_t newFreq);
void adjustPhaseShift(PHASEREG reg, uint16_t newPhase);
void setFreqAdjustMode(FREQADJUSTMODE newMode);
void setMode(MODE newMode);

void setup() {
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  analogReadResolution(12);

  mySettings = SPISettings(8000000, MSBFIRST, SPI_MODE2);

  //SPI.setDataMode(SPI_MODE2);  // Clock idle high, data capture on falling edge
  pinMode(_FSYNCPin, OUTPUT);  // Make the FSYCPin an output; this is analogous
  //  to chip select in most systems.
  pinMode(10, OUTPUT);
  digitalWrite(_FSYNCPin, HIGH);
  SPI.begin();

  Serial.begin(9600);
  while (!Serial); // wait until Serial is ready

  // start reset
  uint32_t defaultFreq = freqCalc(200.0);
  SPI.beginTransaction(mySettings);
  adjustFreq(FREQ0, FULL, defaultFreq);
  adjustFreq(FREQ1, FULL, defaultFreq);
  adjustPhaseShift(PHASE0, 0x0000);
  adjustPhaseShift(PHASE1, 0x0000);
  SPIWrite(0x0100);
  SPIWrite(0x0000);
  // end reset

  delay(delay_val);
  setMode(SINE);
  delay(delay_val);
  setFreqAdjustMode(FULL);
  delay(delay_val);
  float frequency = 500.0; // Sets the frequency to be 500 Hz
  uint32_t freqReg = freqCalc(frequency); // freqCalc() makes a useful 32-bit value out of the frequency value
  adjustFreq(FREQ0, freqReg); // Adjust the frequency. This is a full 32-bit write.
  SPI.endTransaction();
}

void loop() {

  if (Serial.available()) {
    String input = Serial.readString();
    char ch = input[0];

    if (ch == 'c') {
      input.remove(0, 1);
      chan = input.toInt();
      Serial.print("channel changed to ");
      Serial.print(chan);
      Serial.println();
    }
    if (ch == 'r') {
      input.remove(0, 1);
      val = input.toInt();
      int i, Ainput[val];

      if (chan == 0) {
        for (i = 0; i < val; i++) {
          Ainput[i] = analogRead(A2);
        }
      }
      else if (chan == 1) {
        for (i = 0; i < val; i++) {
          Ainput[i] = analogRead(A3);
        }
      }
      else if (chan == 2) {
        for (i = 0; i < val; i++) {
          Ainput[i] = analogRead(A4);
        }
      }
      else if (chan == 3) {
        for (i = 0; i < val; i++) {
          Ainput[i] = analogRead(A5);
        }
      }
      else if (chan == 4) {
        for (i = 0; i < val; i++) {
          Ainput[i] = analogRead(A6);
        }
      }

      Serial.print("Output = ");
      for (i = 0; i < val; i++) {
        if (i > 0) Serial.print(",");
        Serial.print(Ainput[i]);
      }
      Serial.println();
    }
    else if (ch == 'f') {
      input.remove(0, 1);
      float frequency = min( max( FMIN, input.toFloat() ), FMAX ); // save the frequency value, force it to output between limits
      // FMIN = 0.1 kHz, FMAX = 3000 kHz
      unsigned long freqReg = freqCalc(frequency);  // freqCalc() makes a useful 32-bit value out of the frequency value
      //      for (int i = 0; i < 100; i++) {
      SPI.beginTransaction(mySettings);
      adjustFreq(FREQ0, freqReg); // Adjust the frequency. This is a full 32-bit write.
      SPI.endTransaction();
      Serial.print("Frequency changed to ");
      Serial.print(frequency);
      Serial.println();
    }
    else if (ch == 'R') {
      input.remove(0, 1);
      val = input.toInt();
      int i, A0input[val], A1input[val];

      for (i = 0; i < val; i++) {
        A0input[i] = analogRead(A2);
        A1input[i] = analogRead(A3);
      }

      Serial.print("Output = ");
      for (i = 0; i < val; i++) {
        if (i > 0) Serial.print(",");
        Serial.print(A0input[i]);
        Serial.print(",");
        Serial.print(A1input[i]);
      }
      Serial.println();
    }
    else if (ch == 'N') {
      input.remove(0, 1); // extract A, then get the average values
      N = input.toInt();
      if (N < 1) N = 1;
      Serial.print("Number of data points changed to ");
      Serial.print(N);
      Serial.println();
    }
    else if (ch == 'F') {
      input.remove(0, 1); // extract F, then get the frequency value
      float frequency = min( max( FMIN, input.toFloat() ), FMAX ); // save the frequency value, force it to output between limits
      // FMIN = 0.1 kHz, FMAX = 3000 kHz
      //      unsigned long freqReg = gen.freqCalc(frequency);  // freqCalc() makes a useful 32-bit value out of the frequency value
      //      gen.adjustFreq(MiniGen::FREQ0, freqReg); // Adjust the frequency. This is a full 32-bit write.

      delay(500);
      // now get the data for 2 channels
      int A, B;
      double X, Y;
      X = Y = 0.0;
      A = B = 0;
      int Amin, Amax, Bmin, Bmax;
      Amin = Bmin = 2000;
      Amax = Bmax = -1;
      for (int i = 0; i < N; i++) {
        A = analogRead(A2); X += A;
        if (A > Amax) Amax = A; else if (A < Amin) Amin = A;
        B = analogRead(A3); Y += B;
        if (B > Bmax) Bmax = B; else if (B < Bmin) Bmin = B;
      }
      double Mult = 5.0 / 1024.0;
      X = (double)(Amax - Amin) * Mult;
      Y = (double)(Bmax - Bmin) * Mult;

      Serial.print(X, 6);
      Serial.print(",");
      Serial.print(Y, 6);

      Serial.println();
    }
    else if (ch == 'M') {
      input.remove(0, 2); // extract M: then get the mode value string
      SPI.beginTransaction(mySettings);
      if (input == "TRIANGLE") setMode(TRIANGLE);
      else if (input == "SQUARE") setMode(SQUARE);
      else if (input == "SINE") setMode(SINE);
      else input = "ERROR - UNRECOGNISED VALUE";
      SPI.endTransaction();
      Serial.print("Mode = ");
      Serial.println(input);
    }
  }

}


void SPIWrite(uint16_t data)
{
  digitalWrite(_FSYNCPin, LOW);
  SPI.transfer((byte)(data >> 8));
  SPI.transfer((byte)data);
  digitalWrite(_FSYNCPin, HIGH);
  //  Serial.print("SPI:");
  //  Serial.println(data,HEX);
}

uint32_t freqCalc(float desiredFrequency)
{
  return (uint32_t) (desiredFrequency / .0596);
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

void setMode(MODE newMode)
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
  SPIWrite(configReg); // Now write our shadow copy to the part.
}
