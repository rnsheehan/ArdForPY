/*
  ReadAnalogVoltage

  Reads an analog input on pin 0, converts it to voltage, and prints the result to the Serial Monitor.
  Graphical representation is available using Serial Plotter (Tools > Serial Plotter menu).
  Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/ReadAnalogVoltage
*/

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input on analog pin 0:
  int sensorValue = analogRead(A5);
  //analogWrite(A1, 4*sensorValue); 
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  //float voltage = sensorValue * (3.3 / 1023.0);
  float voltage = 0.0; 
  analogVoltageWrite(A0, voltage); 
  analogVoltageWrite(A1, voltage); 
  // print out the value you read:
  //Serial.println(voltage);
  
  Serial.print(sensorValue);
  Serial.print(" , "); 
  Serial.print(analogVoltageRead(A5)); 
  
  /*Serial.print(A0);
  Serial.print(",");
  Serial.print(A1);
  Serial.print(",");
  Serial.print(A2);
  Serial.print(",");
  Serial.print(A3);
  Serial.print(",");
  Serial.print(A4);
  Serial.print(",");
  Serial.print(A5);*/
  
  Serial.println(); 
  delay(2000);            // wait for 1 s
}

float analogVoltageRead(int pin)
{
  // function for converting a bit-reading to a voltage value
  // analogRead values go from 0 to 1023, analogWrite values from 0 to 255
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 3.3V):
  // R. Sheehan 4 - 11 - 2020

  // The pins A0->A5 are numbered 14 --> 19

  bool c1 = pin >= A0; 
  bool c2 = pin <= A5; 
  bool c3 = c1 && c2; 

  if(c3){
    int sensorValue = analogRead(pin); 
    float n_bits = 1023;  
    float Vmax = 3.3;
    float voltage = sensorValue*(Vmax / n_bits); 
  
    return voltage; 
  }
  else{
    return 0.0;   
  }
}

void analogVoltageWrite(int pin, float voltage)
{
  // function for converting a voltage value to bit value for analogWrite
  // analogRead values go from 0 to 1023, analogWrite values from 0 to 255
  // R. Sheehan 4 - 11 - 2020

  bool c1 = pin == A0;
  bool c2 = pin == A1; 
  bool c3 = c1 || c2; // must be A0 or || to write out voltage
  bool c4 = voltage >= 0.0; // voltage value must be in range
  bool c5 = voltage < 3.3; 
  bool c10 = c3 && c4 && c5; 

  if(c10){
    float n_bits = 1023;  
    float Vmax = 3.3;
    int writeVal = int(((4.0*n_bits)/Vmax)*voltage);

    analogWrite(pin, writeVal);   
  }
  else{
    analogWrite(A0, 0);
  }
}
