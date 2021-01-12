// With a view to implementing a frequency response measurement this code should tell the SparkFun MiniGen to output a certain frequency 
// Then, assuming it is connected to some load, read in the amplitude of the signal that is input into the load
// and the amplitude of the signal output from the load
// i.e. it should read the input amplitude and output response of some load
// The board should be sitting there waiting for an input frequency value
// I need to be able to get the arduino to write to the sig gen and tell it to change the frequency
// and then I need it to be able to read the voltage from the AI pins
// to switch between writing a frequency and reading a voltage necessiates some string processing on my part
// R. Sheehan 12 - 11 - 2020

//Include relevant libraries
//In ths case SPI for communicating with the Minigen and the Minigens own libraries to allow custom Minigen commands
#include <SPI.h>
#include <SparkFun_MiniGen.h>

// Set Minigen by the name gen. See gen used in custom Minigen command such as gen.adjustFreq(). 
// If you use a different name then the commands will need to be edited to reflect this. 
MiniGen gen;

// Constants used in the sketch
int loud = 1; // boolean needed for printing comments, debug commands etc, loud = 1 => print
unsigned long delay_val = 5000; // delay value in units of ms

float incomingByte = 0; // Variable to read in desire frequency from serial monitor
int deleteRead = 0; // Variable to read in variable so it is deleted from the buffer. Not used. 

float VOFF = 1.65; // This is the DC offset applied to the output of the SparkFun MiniGen VOFF = Vin/2, Vin = 3.3V 
float VMIN = 0.0; // This is the min DC value that can be handled by the Arduino Micro
float VMAX = 5.0; // This is the max DC value that can be handled by the Arduino Micro 
float FMIN = 100.0; // Minimum frequency setting for the board, it can actually go lower than this but I'm setting 100 Hz as lower limit
float FDEFAULT = 1000.0; // Default frequency of board after setup, board will default to f = 100 Hz after calling gen.reset
float FMAX = 3000000.0; // Maximum frequency setting for the board 3 MHz

int PLACES = 3; // Output voltage readings to the nearest millivolt

const char readCmdStr = 'r'; // read data command string
const char writeCmdStr = 'w'; // write data command string
const char writeAngStr = 'a'; // write analog output
const char readAngStr = 'l'; // read analog input

String ERR_STRING = "Error: Cannot parse input correctly"; // error message 

void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(9600); // Set up serial monitor at 9600 BAUD, BAUD can probably increase
  delay(delay_val); // Delay for opening the serial monitor 
 
  gen.reset(); // Clear the registers in the AD9837 chip, so we're starting from a known location, board outputs at f = 100 Hz
  if(loud){
    Serial.println("Reset!"); // Test Code
  }
  delay(delay_val); // Wait for Minigen to be reset. At power up, the output frequency will be 100Hz.

  gen.setMode(MiniGen::SINE); // I didnt add this in the working code but will comment it here as a reminder. Not necessary as the minigen begins with sine
  delay(delay_val); // Delay for opening the serial monitor 
 
  gen.setFreqAdjustMode(MiniGen::FULL); // a FULL write takes longer but writes the entire frequency word, so you can change from any frequency to any other frequency.
  if(loud){
    Serial.println("Full!");
  }
  delay(delay_val); // Delay for opening the serial monitor 
  
  static float frequency = FDEFAULT; // Set a starting frequency (Probably unneccessary)
  if(loud){
    Serial.println("frequency = 1000.0"); 
  }
  delay(delay_val); // Delay for opening the serial monitor 
  
  unsigned long freqReg = gen.freqCalc(frequency); // freqCalc() makes a useful 32-bit value out of the frequency value
  if(loud){
    Serial.println("calculated"); 
  }
  delay(delay_val); // Delay for opening the serial monitor 
  
  gen.adjustFreq(MiniGen::FREQ0, freqReg); // Adjust the frequency. This is a full 32-bit write.
  if(loud){
    Serial.println("adjusted");
  }
  delay(delay_val); // Delay for opening the serial monitor
  if(loud){
    Serial.println("ready");
  } 
}

void loop() {
  // put your main code here, to run repeatedly:

  // Loop checks for characters entered into serial monitor. 
  // Characters must be typed in bar and then the "enter" button must be pressed.
  if (Serial.available() > 0) { //If there are characters (data) in the buffer

    // read the command that has been input
    // then tell the board to take appropriate action
    // either read some voltage values or write a frequency
    //Serial.println("Write something"); 
    String input = Serial.readString(); //  
    deleteRead = Serial.read();  // read the incoming byte again to delete it from the buffer as the above command does not delete the data.
    if(loud){
      Serial.println("You have entered: "); 
      Serial.println(input); 
    }

    if(isAlpha(input[0])){ // check that the command was input correctly

      if(loud){
        Serial.println("Input starts with a letter"); 
        Serial.println(input[0]);
      }
      
      if(input[0] == readCmdStr){ // test to see if read voltage command is required
        
        // input was the read voltage command
        //Serial.println("Perform steps necessary for read command");  
        //Serial.print("Voltage at pin A0: "); Serial.println(analogVoltageRead(A0));  
        //Serial.print("Voltage at pin A1: "); Serial.println(analogVoltageRead(A1));

        // During operation you will only want to look at the voltages that are being read at the analog pins
        // No need for messages to be printed to the console. 
//        Serial.print(analogVoltageRead(A0)); 
//        Serial.print(" , "); 
//        Serial.print(analogVoltageRead(A1)); 
//        Serial.print(" , "); 
//        Serial.print(analogVoltageRead(A2)); 
//        Serial.print(" , "); 
//        Serial.print(analogVoltageRead(A3)); 
//        Serial.print(" , "); 
//        Serial.print(analogVoltageRead(A4)); 
//        Serial.print(" , "); 
//        Serial.println(analogVoltageRead(A5)); 

        // This loop will perform multiple measurements of the voltages on pins A0 and A1 and reaturn the max values found
        int Nreads = 5000; 
        int count = 0; 
        float A0max = 0.0, A1max = 0.0, A0val, A1val; 
        while(count < Nreads){
          
          if( (A0val = analogVoltageRead(A0)) > A0max){
            A0max = A0val;     
          }

          if( (A1val = analogVoltageRead(A1)) > A1max){
            A1max = A1val;     
          }
          
          count++; 
        }

        A0max -= VOFF; // Substract the DC offset value from the input signal reading
        
        // During operation you will only want to look at the voltages that are being read at the analog pins
        // No need for messages to be printed to the console.         
        Serial.print(A0max, PLACES); 
        Serial.print(" , "); 
        Serial.print(A1max, PLACES);         
        Serial.println(); 
      }
      else if(input[0] == writeCmdStr){ // test to see if write frequency command is required
        
        // input was the write frequency command
        // parse the string to obtain the desired frequency value
        // if no numeric value is entered after 'w' toFloat is robust enough to return 0.0
        // toFloat also ignores any non-numeric characters that may occur
        // See https://www.arduino.cc/reference/en/language/variables/data-types/string/functions/tofloat/
        
        input.remove(0,1); // remove the write frequency command from the start of the string
        // FMIN = 0.1 kHz, FMAX = 3000 kHz
        float frequency = min( max( FMIN, input.toFloat() ), FMAX ); // save the frequency value, force it to output between limits
        
        if(loud){
          Serial.println("Perform steps necessary for write frequency command"); 
          Serial.print("The desired frequency is: "); 
          Serial.print(frequency); 
          Serial.println(" Hz"); 
        }

        // Update the Frequency on the SparkFun MiniGen Board
        unsigned long freqReg = gen.freqCalc(frequency);  // freqCalc() makes a useful 32-bit value out of the frequency value

        gen.adjustFreq(MiniGen::FREQ0, freqReg); // Adjust the frequency. This is a full 32-bit write.

        Serial.println("Frequency Updated");         
      }
      else{ // The command was input incorrectly
        
        Serial.println(ERR_STRING); 
      }
      
    }
    else{ // The command was input incorrectly
      
      Serial.println(ERR_STRING); 
    }
    
  } // end Serial.available()
  
} // end loop

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
    float Vmax = 5.0; // This assumes a 5V reference for the board, this is applicable for the Arduino Micro
    //float Vmax = 3.3; // This assumes a 3.3V reference for the board, this is applicable for the ItsyBitsyM4
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