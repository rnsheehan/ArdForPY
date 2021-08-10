#include "src\\SparkFun_MiniGen_ISBY.h"

unsigned long delay_val = 100; // delay value in units of ms
float FMIN = 20.0; // Minimum frequency setting for the board
float FMAX = 3000000.0; // Maximum frequency setting for the board 3 MHz

int val = 0;
int chan = 0;
int N = 512;
int PLACES = 3; // Output voltage readings to the nearest millivolt

double Mult = (3.3 / 4096.0); // multiplicative constant for the voltage readings

void setup() {
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  //analogReadResolution(12);

  Mult = 3.3 / 1024.0; // 4096 = 2^10
  //Mult = (3.3 / 4096.0); // 4096 = 2^12

  Serial.begin(9600);
  while (!Serial); // wait until Serial is ready

  MiniGen_reset();
  MiniGen_setMode(SINE);
  MiniGen_setFrequency(500.0); // Sets the frequency to be 500 Hz
}

void loop() {

  if (Serial.available()) {
    String input = Serial.readString();
    char ch = input[0];

    if (ch == 'c') {
      // change the channel on which data is being sampled
      // cmd examples: c0, c1, c2, c3
      input.remove(0, 1);
      chan = input.toInt();
      Serial.print("channel changed to ");
      Serial.print(chan);
      Serial.println();
    }
    if (ch == 'r') {
      // sample data from a specific input channel
      // command examples: r5000 
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
      // change the frequency at which the board is operating
      // cmd examples: f500, f1234
      input.remove(0, 1);
      float frequency = min( max( FMIN, input.toFloat() ), FMAX ); // save the frequency value, force it to output between limits
      // FMIN = 0.1 kHz, FMAX = 3000 kHz
      MiniGen_setFrequency(frequency);
      Serial.print("Frequency changed to ");
      Serial.print(frequency);
      Serial.println();
    }
    else if (ch == 'R') {
      // sample data on two specific channels
      // cmd example: R512
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
      // Change the number of data points to be read
      // not sure what need there is for this given then r and R require numeric input
      input.remove(0, 1); // extract A, then get the average values
      N = input.toInt();
      if (N < 1) N = 1;
      Serial.print("Number of data points changed to ");
      Serial.print(N);
      Serial.println();
    }
    else if (ch == 'F') {
      // Change the frequency to some value, measure the Vpp of signal on A2 and A3
      // cmd examples: F500
      input.remove(0, 1); // extract F, then get the frequency value
      float frequency = min( max( FMIN, input.toFloat() ), FMAX ); // save the frequency value, force it to output between limits
      // FMIN = 0.1 kHz, FMAX = 3000 kHz
      MiniGen_setFrequency(frequency);
 
      delay(500);
//      // now get the data for 2 channels
//      int A, B;
//      double X, Y;
//      X = Y = 0.0;
//      A = B = 0;
//      int Amin, Amax, Bmin, Bmax;
//      Amin = Bmin = 2000;
//      Amax = Bmax = -1;
//      for (int i = 0; i < N; i++) {
//        A = analogRead(A2); 
//        if (A > Amax) Amax = A; else if (A < Amin) Amin = A;
//        B = analogRead(A3); 
//        if (B > Bmax) Bmax = B; else if (B < Bmin) Bmin = B;
//      }
//      //double Mult = 3.3 / 4096.0;
//      X = (double)(Amax - Amin) * Mult;
//      Y = (double)(Bmax - Bmin) * Mult;
//
//      Serial.print(X, 6);
//      Serial.print(",");
//      Serial.print(Y, 6);
//      Serial.println();

        // This loop will perform multiple measurements of the voltages on pins A0 and A1 and reaturn the max values found
        int Nreads = 100; 
        int count = 0; 
        //float A0max = 0.0, A1max = 0.0, A0min = 6.0, A1min = 6.0, A0hi, A0lo, A1hi, A1lo, A0val, A1val; 
        int A0max = -1, A1max = -1, A0min = 5000, A1min = 5000, A0hi, A0lo, A1hi, A1lo; 
        float A0val, A1val, mult; 
        while(count < Nreads){

          //A0val = analogVoltageRead(A2); // get the voltage reading from pin A2          
          //A1val = analogVoltageRead(A3); // get the voltage reading from pin A3
          A0val = analogRead(A2); // get the voltage reading from pin A2          
          A1val = analogRead(A3); // get the voltage reading from pin A3

          // test readings to see if they are max values
          if( A0val > A0max){
            A0max = A0val;     
          }

          if( A1val > A1max){
            A1max = A1val;     
          }

          // test readings to see if they are min values
          if( A0val < A0min){
            A0min = A0val;     
          }

          if( A1val < A1min){
            A1min = A1val;     
          }
          
          count++; 
        }

        // Store measured peak-to-peak voltage in A0val and A1val
        mult = 5.0/1024.0; 
        
        A0val = float( A0max - A0min ) * Mult; 

        A1val = float( A1max - A1min ) * Mult; 
                
        // During operation you will only want to look at the voltages that are being read at the analog pins
        // No need for messages to be printed to the console.         
        Serial.print(A0val, PLACES); 
        Serial.print(" , "); 
        Serial.print(A1val, PLACES);         
        Serial.println(); 
    }
    else if (ch == 'M') {
      // change the wave-type being output by the chip
      // cmd example: M:SINE, M:SQUARE, M:TRIANGLE
      // LabVIEW handles the line endings better than the console
      // SINE != SINE\n
      input.remove(0, 2); // extract M: then get the mode value string
      if (input == "TRIANGLE") MiniGen_setMode(TRIANGLE);
      else if (input == "SQUARE") MiniGen_setMode(SQUARE);
      else if (input == "SINE") MiniGen_setMode(SINE);
      else input = "ERROR - UNRECOGNISED VALUE";
      Serial.print("Mode = ");
      Serial.println(input);
    }
  }

}
