// Version of DCWriteRead that is compatible with the Adafruit ItsyBitsy M4
// Keep the same Read / Write Commands, update the method used to read DC signals
// R. Sheehan 20 - 4 - 2021

// With a view to implementing a DC measurements using the Arduino Micro test and evaluate the operation of the board
// Keep commands consistent with SigGen Sketch
// R. Sheehan 16 - 11 - 2020

// Details of Pinouts can be found here
// https://learn.adafruit.com/introducing-adafruit-itsybitsy-m4/pinouts 

// Constants used in the sketch
int loud = 0; // boolean needed for printing comments, debug commands etc, loud = 1 => print
unsigned long delay_val = 1000; // delay value in units of ms

float incomingByte = 0; // Variable to read in desire frequency from serial monitor
int deleteRead = 0; // Variable to read in variable so it is deleted from the buffer. Not used. 

float VOFF = 1.65; // This is the DC offset applied to the output of the SparkFun MiniGen VOFF = Vin/2, Vin = 3.3V 
float VMIN = 0.0; // This is the min DC value that can be handled by the Itsy-Bitsy M4
float VMAX = 3.3; // This is the max DC value that can be handled by the Itsy-Bitsy M4 

//int PLACES = 4; // Output voltage readings to the nearest millivolt, it's only really accurate to 10 mV though
int PLACES = 3; // Output voltage readings to the nearest 10 millivolt

//int DCPINA = 9; // pin for outputting DC Values, it is a digital pin adapted for the purpose
//int DCPINB = 6; // pin for outputting DC Values, it is a digital pin adapted for the purpose

const char readCmdStr = 'r'; // read data command string for reading max AC input
const char writeCmdStr = 'w'; // write data command string for writing frequency values
const char writeAngStrA = 'a'; // write analog output from DCPINA
const char writeAngStrB = 'b'; // write analog output from DCPINB
const char readAngStr = 'l'; // read analog input

String ERR_STRING = "Error: Cannot parse input correctly"; // error message 

void setup() {
  // put your setup code here, to run once:

  pinMode(A0, OUTPUT); // Use A0 as analog output channel 0 - 3.3 V
  pinMode(A1, OUTPUT); // Use A1 as analog output channel 0 - 3.3 V
  //pinMode(A1, INPUT); // Use A1 as analog output channel 0 - 3.3 V
  pinMode(A2, INPUT); // Use remaining channels as analog input 0 - 3.3 V on all
  pinMode(A3, INPUT); 
  pinMode(A4, INPUT); 
  pinMode(A5, INPUT);  
  pinMode(A6, INPUT); // Secret AI! 

  analogReadResolution(12); // in this case n-bits = 4096
  analogWriteResolution(10); // in this case n-bits = 4096
  
  Serial.begin(9600); // Set up serial monitor at 9600 BAUD, BAUD can probably increase
  delay(delay_val); // Delay for opening the serial monitor 

  // prevent analog write bug from occurring
  analogWrite(A0, 0); 
  analogWrite(A1, 0); 
  analogWrite(A0, 0); 
  analogWrite(A1, 0);
}

void loop() {
  // put your main code here, to run repeatedly:

  // Indicates if the specified Serial port is ready.
  // for details see https://www.arduino.cc/reference/en/language/functions/communication/serial/ifserial/
  if(Serial){
    
    // Loop checks for characters entered into serial monitor. 
    // Characters must be typed in bar and then the "enter" button must be pressed.
    // for details see https://www.arduino.cc/reference/en/language/functions/communication/serial/available/
    if (Serial.available() > 0) { //If there are characters (data) in the buffer
  
      // read the command that has been input
      // then tell the board to take appropriate action
      // either read some voltage values or write a frequency
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
        
        if(input[0] == writeAngStrA){ // write analog output on pin A0
            
            input.remove(0,1); // remove the write analog command from the start of the string
            // VMIN = 0 V, VMAX = 3.3 V
            float Vout = min( max( VMIN, input.toFloat() ), VMAX ); // save the output value, force it to output between limits
  
            if(loud){
              Serial.println("Perform steps necessary for write analog command"); 
              Serial.print("The desired voltage from A0 is: "); 
              Serial.print(Vout); 
              Serial.println(" V"); 
            }
  
            analogVoltageWrite(A0, Vout); 
            //delay(delay_val); // give the device time to settle at its new voltage
        }
        else if(input[0] == writeAngStrB){ // write analog output on pin A1
            input.remove(0,1); // remove the write analog command from the start of the string
            // VMIN = 0 V, VMAX = 3.3 V
            float Vout = min( max( VMIN, input.toFloat() ), VMAX ); // save the output value, force it to output between limits
  
            if(loud){
              Serial.println("Perform steps necessary for write analog command"); 
              Serial.print("The desired voltage from A1 is: "); 
              Serial.print(Vout); 
              Serial.println(" V"); 
            }
  
            analogVoltageWrite(A1, Vout); 
            //delay(delay_val); // give the device time to settle at its new voltage
        }
        else if(input[0] == readAngStr){ // test to see if read voltage command is required          
          // input was the read voltage command
          if(loud){
            Serial.println("Perform steps necessary for read command");  
            Serial.print("Voltage at pin A2: "); Serial.println(analogVoltageRead(A2));  
            Serial.print("Voltage at pin A3: "); Serial.println(analogVoltageRead(A3));
            Serial.print("Voltage at pin A4: "); Serial.println(analogVoltageRead(A4));  
            Serial.print("Voltage at pin A5: "); Serial.println(analogVoltageRead(A5));
          }
          else{  
            // During operation you will only want to look at the voltages that are being read at the analog pins
            // No need for messages to be printed to the console. 
            //Serial.print( analogVoltageRead(A1), PLACES); 
            //Serial.print(" , ");
            Serial.print( analogVoltageRead(A2), PLACES); 
            Serial.print(" , "); 
            Serial.print( analogVoltageRead(A3), PLACES); 
            Serial.print(" , "); 
            Serial.print( analogVoltageRead(A4), PLACES); 
            Serial.print(" , "); 
            Serial.print( analogVoltageRead(A5), PLACES); 
            Serial.print(" , "); 
            Serial.print( analogVoltageRead(A6), PLACES); 
            Serial.println(""); 
          }
        }
        else{ // The command was input incorrectly
          
          Serial.println(ERR_STRING); 
        }
        
      }
      else{ // The command was input incorrectly
        
        Serial.println(ERR_STRING); 
      }
      
    } // end if(Serial.available()>0)

  } // end if(Serial)
  
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
    //float n_bits = 1023; // 1024 = 2^10 this assumes 10-bit resolution, this is applicable for Arduino Micro
    float n_bits = 4096; // 4096 = 2^12 this assumes 12-bit resolution, this is applicable for Arduino Micro
    //float Vmax = 5.0; // This assumes a 5V reference for the board, this is applicable for the Arduino Micro
    float Vmax = 3.3; // This assumes a 3.3V reference for the board, this is applicable for the ItsyBitsyM4
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
    float n_bits = 1023; // 1024 = 2^10 this assumes 10-bit resolution, this is applicable for Arduino Micro
    //float n_bits = 4096; // 4096 = 2^12 this assumes 12-bit resolution, this is applicable for Arduino Micro
    //float Vmax = 5.0; // This assumes a 5V reference for the board, this is applicable for the Arduino Micro
    float Vmax = 3.3; // This assumes a 3.3V reference for the board, this is applicable for the ItsyBitsyM4
    //int writeVal = int(((4.0*n_bits)/Vmax)*voltage);
    int writeVal = int((n_bits/Vmax)*voltage);

    analogWrite(pin, writeVal);   
  }
  else{
    analogWrite(A0, 0);
  }
}
