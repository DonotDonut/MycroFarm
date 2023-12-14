
#include <Servo.h>
Servo Rmotor;
Servo Lmotor; 

int const RmotorPin= 9;
int const LmotorPin= 10;



#define ledPin 13
int trigPin = 12;    // Trigger
int echoPin = 11;    // Echo
long duration, cm, inches;
 



void setup() {
  //Serial Port begin
  Serial.begin (9600);

  //Define inputs and outputs
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin, OUTPUT);

 

  Rmotor.attach(RmotorPin);
  Lmotor.attach(LmotorPin);

}
 
void loop() {

  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
 
  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);
 
  // Convert the time into a distance
  // distance = (traveltime/2) speed of sound 
  // speeds of sound is 343m/s 
  cm = (duration/2) / 29.1;     // Divide by 29.1 or multiply by 0.0343
  inches = (duration/2) / 74;   // Divide by 74 or multiply by 0.0135
  
  Serial.print(inches);
  Serial.print("in, ");
  Serial.print(cm);
  Serial.print("cm");
  Serial.println();
  
  delay(250);

  // checking if lid is on 
  if (inches <= 10) {
    //lid is on the box led is on 
    digitalWrite(ledPin, HIGH);
    Rmotor.write(90);
    Rmotor.write(90);
  } else {
    // lid is off the box then led is off 
    digitalWrite(ledPin, LOW); 
    Rmotor.write(0);
    Rmotor.write(0);
  }
}


