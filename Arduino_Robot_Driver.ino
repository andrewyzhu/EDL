// define pins
const int pinON = 6;         // connect pin 6 to ON/OFF switch via 1k resistor, active HIGH
volatile int enc_count_Left = 0;// initialize global counters to 0
volatile int enc_count_Right = 0;
const int pinCW_Left = 7;    // connect pin 7 to clock-wise PMOS gate
const int pinCC_Left = 8;    // connect pin 8 to counter-clock-wise PMOS gate
const int pinSpeed_Left = 9; // connect pin 9 to speed reference
const int pinCW_Right = 10;    // connect pin 10 to clock-wise PMOS gate
const int pinCC_Right = 11;    // connect pin 11 to counter-clock-wise PMOS gate
const int pinSpeed_Right = 12; // connect pin 12 to speed reference

void setup() {
  Serial.begin(9600);       // start serial communication via USB at 9600 bits per second (baud)
  //pinMode(pinON, INPUT);    // set on/off switch pin as input

  attachInterrupt(2, count_Left, RISING);
  attachInterrupt(3, count_Right, RISING);

  pinMode(pinON,INPUT);
  pinMode(pinCW_Left,OUTPUT);
  pinMode(pinCC_Left,OUTPUT);
  pinMode(pinSpeed_Right,OUTPUT);
  pinMode(pinCW_Right,OUTPUT);
  pinMode(pinCC_Right,OUTPUT);
  pinMode(pinSpeed_Right,OUTPUT);
  pinMode(13,OUTPUT);             // on-board LED
  digitalWrite(13,LOW);           // turn LED off
  digitalWrite(pinCW_Left,LOW);   // stop clockwise
  digitalWrite(pinCC_Left,LOW);   // stop counter-clockwise
  digitalWrite(pinCW_Right,LOW);   // stop clockwise
  digitalWrite(pinCC_Right,LOW);   // stop counter-clockwise
  analogWrite(pinSpeed_Left,50);  // set speed reference, duty-cycle = 50/255
  analogWrite(pinSpeed_Right,50);  // set speed reference, duty-cycle = 50/255
}

void count_Left(){
  enc_count_Left++;
}

void count_Right(){
  enc_count_Right++;
}

void loop() {
  enc_count_Left = 0;
  enc_count_Right = 0;
//  int onoff_switch = digitalRead(pinON);   // read switch on/off state
//  Serial.println(onoff_switch);            // send value as ASCII-encoded decimal 
int Diff = abs(enc_count_Left - enc_count_Right);
Serial.print("LEFT: ");
Serial.println(enc_count_Left);
Serial.print("RIGHT: ");
Serial.println(enc_count_Right);
Serial.print("DIFF ");
Serial.println(Diff);
digitalWrite(pinCW_Left,HIGH);            // go clockwise
digitalWrite(pinCW_Right,HIGH);            // go clockwise
  do {} while ((enc_count_Left < 3*12*64) && (enc_count_Right < 3*12*64));   // do nothing until wheel makes 3 rotations
  digitalWrite(pinCW_Left,LOW);             // stop
  digitalWrite(pinCW_Right,LOW);
  delay(1000);                              // wait 0.5 seconds
}

