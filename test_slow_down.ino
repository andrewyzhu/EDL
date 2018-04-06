/*
ECEN2830 motor position control example (left wheel only)
*/

// define pins
const int pinON = 8;         // connect pin 6 to ON/OFF switch, active HIGH
const int pinCW_Left = 7;    // connect pin 7 to clock-wise PMOS gate
const int pinCC_Left = 6;    // connect pin 8 to counter-clock-wise PMOS gate
const int pinCW_Right = 4;
const int pinCC_Right = 5;
const int pinSpeed_Left = 11; // connect pin 9 to speed reference
const int pinSpeed_Right = 10;
const int wheelspeed = 50;

// encoder counter variable
volatile int enc_count_Left = 0; // "volatile" means the variable is stored in RAM
volatile int enc_count_Right = 0;

// setup pins and initial values
void setup() {
  pinMode(pinON,INPUT);
  pinMode(pinCW_Left,OUTPUT);
  pinMode(pinCC_Left,OUTPUT);
  pinMode(pinSpeed_Left,OUTPUT);
  pinMode(pinCW_Right,OUTPUT);
  pinMode(pinCC_Right,OUTPUT);
  pinMode(pinSpeed_Right,OUTPUT);
  pinMode(13,OUTPUT);             // on-board LED
  digitalWrite(13,LOW);           // turn LED off
  digitalWrite(pinCW_Left,LOW);   // stop clockwise
  digitalWrite(pinCC_Left,LOW);   // stop counter-clockwise
  digitalWrite(pinCW_Right,LOW);   // stop clockwise
  digitalWrite(pinCC_Right,LOW);   // stop counter-clockwise
  analogWrite(pinSpeed_Left,wheelspeed);  // set speed reference, duty-cycle = 50/255
  analogWrite(pinSpeed_Right,wheelspeed);
  /* 
    Connect left-wheel encoder output to pin 2 (external Interrupt 0) via a 1k resistor
    Rising edge of the encoder signal triggers an interrupt 
    count_Left is the interrupt service routine attached to Interrupt 0 (pin 2)
  */
  attachInterrupt(0, count_Left, RISING);
  attachInterrupt(1,count_Right,RISING);
}

/*
  Interrupt 0 service routine
  Increment enc_count_Left on each rising edge of the 
  encoder signal connected to pin 2
*/ 
void count_Left(){
  enc_count_Left++;
}

void count_Right(){
  enc_count_Right++;
}

void loop() {
  do {
    enc_count_Left = 0;                     // reset encoder counter to 0
    enc_count_Right = 0;
  } while (digitalRead(pinON) == LOW);      // wait for ON switch
  delay(1000);
  digitalWrite(13,HIGH);                    // turn LED on
  digitalWrite(pinCW_Left,HIGH);            // go clockwise
  digitalWrite(pinCW_Right,HIGH);
  do {} while ((enc_count_Left < (1148-.45*wheelspeed)) && (enc_count_Right < (1148-.45*wheelspeed)));   // do nothing until wheel makes 3 rotations 1148
  digitalWrite(pinCW_Left,LOW);             // stop
  digitalWrite(pinCW_Right,LOW);
  digitalWrite(13,LOW);                     // turn LED off
  delay(1000);                              // wait 1 second

  enc_count_Left = 0;                     // reset encoder counter to 0
  enc_count_Right = 0;
  
  digitalWrite(13,HIGH);                    // turn LED on
  digitalWrite(pinCW_Left,HIGH);            // go clockwise
  digitalWrite(pinCC_Right,HIGH);
  do {} while ((enc_count_Left < (808-.22*wheelspeed)) && (enc_count_Right < (808-.22*wheelspeed)));   // do nothing until wheel makes 3 rotations
  digitalWrite(pinCW_Left,LOW);             // stop
  digitalWrite(pinCC_Right,LOW);
  digitalWrite(13,LOW);                     // turn LED off
  delay(1000);                              // wait 1 second

  enc_count_Left=0;
  enc_count_Right=0;

  digitalWrite(13,HIGH);                    // turn LED on
  digitalWrite(pinCW_Left,HIGH);            // go clockwise
  digitalWrite(pinCW_Right,HIGH);
  do {} while ((enc_count_Left < (1148-.45*wheelspeed)) && (enc_count_Right < (1148-.45*wheelspeed)));   // do nothing until wheel makes 3 rotations
  digitalWrite(pinCW_Left,LOW);             // stop
  digitalWrite(pinCW_Right,LOW);
  digitalWrite(13,LOW);                     // turn LED off
  delay(1000);                              // wait 1 second

  enc_count_Left = 0;                     // reset encoder counter to 0
  enc_count_Right = 0;

  digitalWrite(13,HIGH);                    // turn LED on
  digitalWrite(pinCW_Right,HIGH);            // go clockwise
  digitalWrite(pinCC_Left,HIGH);
  do {} while ((enc_count_Left < (808-.22*wheelspeed)) && (enc_count_Right < (808-.22*wheelspeed)));   // do nothing until wheel makes 3 rotations
  digitalWrite(pinCW_Right,LOW);             // stop
  digitalWrite(pinCC_Left,LOW);
  digitalWrite(13,LOW);                     // turn LED off
  delay(1000);                              // wait 1 second
}
