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

// Need to fogure out the best naming convention for the outputs
int DCPINA = 9; // pin for outputting DC Values, it is a digital pin adapted for the purpose
int DCPINB = 6; // pin for outputting DC Values, it is a digital pin adapted for the purpose

const char readCmdStr = 'r'; // read data command string for reading max AC inputu
const char writeCmdStr = 'w'; // write data command string for writing frequency values

const char writeAngStrA = 'a'; // write analog output from DCPINA
const char writeAngStrB = 'b'; // write analog output from DCPINB
const char readAngStr = 'l'; // read analog input

String ERR_STRING = "Error: Cannot parse input correctly"; // error message 
String idCmd = "IDN"; // define the command that tells the board to print the device name
String DEV_NAME = "Dev Name: uHeater Controller"; // identify the board with a string

void setup() {
  // put your setup code here, to run once:

  pinMode(DCPINA, OUTPUT); // this will change
  pinMode(DCPINB, OUTPUT); // this will change
  
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

}
