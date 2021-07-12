// Sketch for testing analog output capabilities from PWM pins
// For queries on any of the Arduino commands see https://www.arduino.cc/reference/en/
// R. Sheehan 12 - 7 - 2021

// Constants used in the sketch
int loud = 0; // boolean needed for printing comments, debug commands etc, loud = 1 => print
int deleteRead = 0; // Variable to read in variable so it is deleted from the buffer.
int DCPINA = 9; // pin for outputting DC Values, it is a digital pin adapted for the purpose
int DCPINB = 6; // pin for outputting DC Values, it is a digital pin adapted for the purpose
int PLACES = 3; // Output voltage readings to the nearest millivolt

unsigned long delay_val = 500; // delay value in units of ms

float incomingByte = 0; // Variable to read in desire frequency from serial monitor
float VMIN = 0.0; // This is the min DC value that can be handled by the Arduino Micro
float VMAX = 5.0; // This is the max DC value that can be handled by the Arduino Micro 

// Commands to be sent to AM via serial console
const char writeAngStrA = 'a'; // write analog output from DCPINA
const char writeAngStrB = 'b'; // write analog output from DCPINB
const char readAngStr = 'l'; // read analog input

String ERR_STRING = "Error: Cannot parse input correctly"; // error message 

void setup() {
  // put your setup code here, to run once:
  
  pinMode(DCPINA, OUTPUT); 
  pinMode(DCPINB, OUTPUT);
  pinMode(A0, INPUT); 
  pinMode(A1, INPUT); 
  pinMode(A2, INPUT); 
  pinMode(A3, INPUT); 
  pinMode(A4, INPUT); 
  pinMode(A5, INPUT);  
  
  Serial.begin(9600); // Set up serial monitor at 9600 BAUD, BAUD can probably increase
  delay(delay_val); // Delay for opening the serial monitor
}

void loop() {
  // put your main code here, to run repeatedly:

  // Indicates if the specified Serial port is ready.
  // for details see https://www.arduino.cc/reference/en/language/functions/communication/serial/ifserial/
  if(Serial){
    
    // Loop checks for characters entered into serial monitor. 
    // Characters must be typed in bar and then the "enter" button must be pressed.
    // for details see https://www.arduino.cc/reference/en/language/functions/communication/serial/available/
    if (Serial.available() > 0) { // If there are characters (data) in the buffer
  
      // read the command that has been input
      // then tell the board to take appropriate action
      // either read some voltage values or write some voltage values from DCPINA or DCPINB
      
      String input = Serial.readString(); 
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
        
        if(input[0] == writeAngStrA){ // write analog output on one of pin 3, 5, 6, 9, 10, 11, 13
            // output on DCPINA is followed by RC LPF comprising R = 1 kOhm, C = 100 uF
                        
            input.remove(0,1); // remove the write analog command from the start of the string
            // VMIN = 0 V, VMAX = 5 V
            float Vout = min( max( VMIN, input.toFloat() ), VMAX ); // save the output value, force it to output between limits
            int DC =  int( 51 * Vout ); // compute duty cycle based on DC = 255*(Vout/5) = 51 * Vout
  
            if(loud){
              Serial.println("Perform steps necessary for write analog command"); 
              Serial.print("The desired voltage is: "); 
              Serial.print(Vout); 
              Serial.println(" V");  
              Serial.print("The necessary duty cycle is : "); 
              Serial.println(51 * Vout); 
              Serial.print("The actual duty cycle is : "); 
              Serial.println(DC);
            }
  
            analogWrite(DCPINA, DC); // variable length duty cycle whose average output is Vout
            delay(delay_val); // give the device time to settle at its new voltage  
        }
        else if(input[0] == writeAngStrB){ // write analog output on one of pin 3, 5, 6, 9, 10, 11, 13
            // output on DCPINB is followed by RC LPF comprising R = 1 kOhm, C = 100 uF      
              
            input.remove(0,1); // remove the write analog command from the start of the string
            
            // VMIN = 0 V, VMAX = 5 V
            float Vout = min( max( VMIN, input.toFloat() ), VMAX ); // save the output value, force it to output between limits
            int DC =  int( 51 * Vout ); // compute duty cycle based on DC = 255*(Vout/5) = 51 * Vout
  
            if(loud){
              Serial.println("Perform steps necessary for write analog command"); 
              Serial.print("The desired voltage is: "); 
              Serial.print(Vout); 
              Serial.println(" V");  
              Serial.print("The necessary duty cycle is : "); 
              Serial.println(51 * Vout); 
              Serial.print("The actual duty cycle is : "); 
              Serial.println(DC);
            }
  
            analogWrite(DCPINB, DC); // variable length duty cycle whose average output is Vout
            delay(delay_val); // give the device time to settle at its new voltage     
        }
        else if(input[0] == readAngStr){ // test to see if read voltage command is required
          
          // input was the read voltage command
          if(loud){
            Serial.println("Perform steps necessary for read command");  
            Serial.print("Voltage at pin A0: "); Serial.println(analogVoltageRead(A0));  
            Serial.print("Voltage at pin A1: "); Serial.println(analogVoltageRead(A1));
          }
  
          // During operation you will only want to look at the voltages that are being read at the analog pins
          // No need for text messages to be printed to the console. 
          Serial.print( analogVoltageRead(A0), PLACES); 
          Serial.print(" , "); 
          Serial.print( analogVoltageRead(A1), PLACES); 
          Serial.print(" , "); 
          Serial.print( analogVoltageRead(A2), PLACES); 
          Serial.print(" , "); 
          Serial.print( analogVoltageRead(A3), PLACES); 
          Serial.print(" , "); 
          Serial.print( analogVoltageRead(A4), PLACES); 
          Serial.print(" , "); 
          Serial.print( analogVoltageRead(A5), PLACES); 
          Serial.println(""); 
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
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  // R. Sheehan 4 - 11 - 2020

  // function can be used for either of Arduino Micro or ItsyBitsyM4
  // Vmax value must be defined depending on board

  // The pins A0->A5 are numbered 14 --> 19

  bool c1 = pin >= A0; 
  bool c2 = pin <= A5; 
  bool c3 = c1 && c2; 

  if(c3){
    int sensorValue = analogRead(pin); 
    float n_bits = 1023.0;  
    float Vmax = 5.0; // This assumes a 5V reference for the board, this is applicable for the Arduino Micro
    //float Vmax = 3.3; // This assumes a 3.3V reference for the board, this is applicable for the ItsyBitsyM4
    float voltage = sensorValue*(Vmax / n_bits); 
  
    return voltage; 
  }
  else{
    return 0.0;   
  }
}
