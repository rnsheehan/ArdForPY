#include "src\\SparkFun_MiniGen_IBM4.h"

unsigned long delay_val = 100; // delay value in units of ms
float FMIN = 5.0; // Minimum frequency setting for the board
float FMAX = 3000000.0; // Maximum frequency setting for the board 3 MHz

int val = 0;
int chan = 0;
int N = 512;

void setup() {
  pinMode(A2, INPUT);
  analogReadResolution(12);

//  Serial.begin(460800);
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

    if (ch == 'r') {
      input.remove(0, 1);
      val = input.toInt();
      int i, Ainput[val];

      for (i = 0; i < val; i++) {
        Ainput[i] = analogRead(A2);
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
