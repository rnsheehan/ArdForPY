#include <SPI.h>
#include "SparkFun_MiniGen.h"
MiniGen gen;
unsigned long delay_val = 100; // delay value in units of ms
float FMIN = 20.0; // Minimum frequency setting for the board
float FMAX = 3000000.0; // Maximum frequency setting for the board 3 MHz

int val = 0;  
int chan;       

void setup() {
  Serial.begin(9600);
  while(!Serial); // wait until Serial is ready

  gen.reset(); 
  delay(delay_val); 
  gen.setMode(MiniGen::SINE);  
  delay(delay_val); 
  gen.setFreqAdjustMode(MiniGen::FULL);
  delay(delay_val); 
  float frequency = 200.0; // Sets the frequency to be 200 Hz
  unsigned long freqReg = gen.freqCalc(frequency); // freqCalc() makes a useful 32-bit value out of the frequency value
  gen.adjustFreq(MiniGen::FREQ0, freqReg); // Adjust the frequency. This is a full 32-bit write.
}

void loop() {

  if (Serial.available()) {
    String input=Serial.readString();
    char ch=input[0];

    if (ch=='r') {
      input.remove(0,1);
      val=input.toInt();     
      int i,Ainput[val];

      for (i=0;i<val;i++) {
         Ainput[i]=analogRead(A0);
      }
      
      Serial.print("Output = ");
      for (i=0;i<val;i++) {
        if (i>0) Serial.print(",");
        Serial.print(Ainput[i]);
      }
      Serial.println();
    }
    else if (ch=='f') {
      input.remove(0,1);
      float frequency = min( max( FMIN, input.toFloat() ), FMAX ); // save the frequency value, force it to output between limits
      // FMIN = 0.1 kHz, FMAX = 3000 kHz
      unsigned long freqReg = gen.freqCalc(frequency);  // freqCalc() makes a useful 32-bit value out of the frequency value
      gen.adjustFreq(MiniGen::FREQ0, freqReg); // Adjust the frequency. This is a full 32-bit write.
      Serial.print("Frequency changed to ");
      Serial.print(frequency);
      Serial.println();    
    }
  }

}
