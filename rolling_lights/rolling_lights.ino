/*
 Rolling Lights
 Sketch for an Arduino gadget that simlulate the red light of the high tech car in TV drama.
 
TODO: use PWM for more realistic effect, as on-and-off can't done simultaniously.

TODO: how to control more LEDs with only 13 digital outpin?


 
 */

int sensorPin = A0; // select the input pin for the input device

const int numberOfLEDs = 10;
int counter =0;
int ms_delay=50; // 10ms is to fast for small array


void setup() {
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);

  // setting each pin to LOW so as not to light the LED
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  digitalWrite(8, LOW);
  digitalWrite(9, LOW);
  digitalWrite(10, LOW);
  digitalWrite(11, LOW);

  // no senoring

  // no  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:


  for(int i=0; i <=numberOfLEDs; i++)
  {
    digitalWrite(i+1, HIGH);
    delay(ms_delay);
  }

  // ... and then turning them off.
  for(int i=0; i <=numberOfLEDs; i++)
  {
    digitalWrite(i+1, LOW);
    delay(ms_delay);
  }

  // reverse from other side
  // ATTENTION: be aware of the digital pin to write on
  for(int i=numberOfLEDs-1; i >=0; i--)
  {
    digitalWrite(i+2, HIGH);
    delay(ms_delay);
  }

  // ... and then turning them off.
  for(int i=numberOfLEDs-1; i >=0; i--)
  {
    digitalWrite(i+2, LOW);
    delay(ms_delay);
  }
 
}
