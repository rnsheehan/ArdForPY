#include "src\\SparkFun_MiniGen_ISBY.h"

unsigned long delay_val = 100; // delay value in units of ms
float FMIN = 20.0; // Minimum frequency setting for the board
float FMAX = 3000000.0; // Maximum frequency setting for the board 3 MHz

int val = 0;
int chan = 0;
int N = 512;

void setup() {
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  analogReadResolution(12);

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
      MiniGen_setFrequency(frequency);
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
      MiniGen_setFrequency(frequency);
 
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
      if (input == "TRIANGLE") MiniGen_setMode(TRIANGLE);
      else if (input == "SQUARE") MiniGen_setMode(SQUARE);
      else if (input == "SINE") MiniGen_setMode(SINE);
      else input = "ERROR - UNRECOGNISED VALUE";
      Serial.print("Mode = ");
      Serial.println(input);
    }
  }

}
