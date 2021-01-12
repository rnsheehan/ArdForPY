// the setup function runs once you press reset or power the board

const int digOut = 12; 

void setup() {
  // put your setup code here, to run once:
  // initialise digital pin 13 as an output
  pinMode(digOut, OUTPUT); 
  pinMode(13, OUTPUT); 
}

// the loop function runs over and over again forever
void loop() {
  int count = 0; 
  int count_max = 10; 
  while(count < count_max){
    // put your main code here, to run repeatedly:
    digitalWrite(13, HIGH); // turn the LED on (HIGH is the voltage level)
    digitalWrite(digOut, HIGH);  // turn the LED off by making the voltage LOW
    delay(2000);            // wait for 1 s
    digitalWrite(13, LOW); // turn the LED on (HIGH is the voltage level)
    digitalWrite(digOut, LOW);  // turn the LED off by making the voltage LOW
    delay(1000);            // wait for 1 s
    count++; 
  }
}
