// Want to be able to output on all PWM outputs simultaneously for the purposes of biasing a set of micro-heaters
// There are 8 PWM outputs on the AM, need to be able write to all of them
// Idea is to power the AM externally so that there won't be any issues with power consumption via USB
// R. Sheehan 20 - 10 - 2025

// For queries on any of the Arduino commands see https://www.arduino.cc/reference/en/

// Constants used in the sketch
int loud = 0; // boolean needed for printing comments, debug commands etc, loud = 1 => print
unsigned long delay_val = 500; // delay value in units of ms

float incomingByte = 0; // Variable to read in desire frequency from serial monitor
int deleteRead = 0; // Variable to read in variable so it is deleted from the buffer. Not used. 

float VOFF = 1.65; // This is the DC offset applied to the output of the SparkFun MiniGen VOFF = Vin/2, Vin = 3.3V 
float VMIN = 0.0; // This is the min DC value that can be handled by the Arduino Micro
float VMAX = 5.0; // This is the max DC value that can be handled by the Arduino Micro 

int PLACES = 2; // Output voltage readings to the nearest 10 millivolt

int PWM1 = 4; // pin for outputting DC Values, it is a digital pin adapted for the purpose
int PWM2 = 5; // pin for outputting DC Values, it is a digital pin adapted for the purpose
int PWM3 = 6; // pin for outputting DC Values, it is a digital pin adapted for the purpose
int PWM4 = 9; // pin for outputting DC Values, it is a digital pin adapted for the purpose
int PWM5 = 10; // pin for outputting DC Values, it is a digital pin adapted for the purpose
int PWM6 = 11; // pin for outputting DC Values, it is a digital pin adapted for the purpose
int PWM7 = 12; // pin for outputting DC Values, it is a digital pin adapted for the purpose
int PWM8 = 13; // pin for outputting DC Values, it is a digital pin adapted for the purpose

int PWM_pins[8] = {PWM1, PWM2, PWM3, PWM4, PWM5, PWM6, PWM7, PWM8}; // Make an array of the PWM pin numbers to facilitate looping over all the pins

const char writePWM = 'PWM'; // write analog output from PWM<x>
const char writeAll = 'PWM'; // write analog output from all PWM pins, requires an array of input values
const char readAI = 'l'; // read analog input

String ERR_STRING = "Error: Cannot parse input correctly"; // error message 
String idCmd = "IDN"; // define the command that tells the board to print the device name
String DEV_NAME = "Dev Name: uHeater Controller"; // identify the board with a string

void setup() {
  // put your setup code here, to run once:

  pinMode(PWM1, OUTPUT); 
  pinMode(PWM2, OUTPUT); 
  pinMode(PWM3, OUTPUT); 
  pinMode(PWM4, OUTPUT); 
  pinMode(PWM5, OUTPUT); 
  pinMode(PWM6, OUTPUT); 
  pinMode(PWM7, OUTPUT); 
  pinMode(PWM8, OUTPUT); 
  
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
        
        if(input[0] == writePWM){ // write analog output on one of pin 4, 5, 6, 9, 10, 11, 12, 13
            // output on PWM<x> is followed by RC LPF comprising R = 1 kOhm, C = 100 uF
            // the idea is that you would read a voltage value from the input stream write out that value using PWM on one of the analog pins
            // except this isn't what PWM does. PWM does not vary amplitude only pulse duty cycle, however, by varying the pulse duty cycle length
            // I can alter the average voltage that is being output by the arduino micro. 
            
            // use the substring function to extract the pin number and the voltage value from the input
            // you can test the operation by inputting the call and printing the results
            // There is also an indexOf() function that could prove useful

            input.remove(0,1); // remove the write analog command from the start of the string
            // VMIN = 0 V, VMAX = 5 V
            float Vout = min( max( VMIN, input.toFloat() ), VMAX ); // save the output value, force it to output between limits
            int DC =  int( 51 * Vout ); // compute duty cycle based on DC = 255*(Vout/5) = 51 * Vout
            //int pin = 9; 
  
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
  
            //analogWrite(pin, 0); // always off
            //analogWrite(pin, 64); // 25% duty cycle
            //analogWrite(pin, 127); // 50% duty cycle
            //analogWrite(pin, 191); // 75% duty cycle
            //analogWrite(pin, 255); // 100% duty cycle
            //delay(delay_val);       
        }
        else if(input[0] == readAI){ // test to see if read voltage command is required
          
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
        else if(input.equals(idCmd)){
            Serial.println(DEV_NAME); 
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
    float n_bits = 1023;  
    float Vmax = 5.0; // This assumes a 5V reference for the board, this is applicable for the Arduino Micro
    float voltage = sensorValue*(Vmax / n_bits); 
  
    return voltage; 
  }
  else{
    return 0.0;   
  }
}
