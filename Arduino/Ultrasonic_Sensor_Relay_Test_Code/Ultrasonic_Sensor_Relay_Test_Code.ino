const int trigPin = 9;   // declaring Pin 9 as ultrasonic sensor contol
const int echoPin = 10;   // declaring Pin 10 as serial input from sensor
float duration, distance; // variables to hold sound echo duration and calculate distance
const int relayPin = 8;  // relay control Pin 8

void setup() {
  
  pinMode(trigPin, OUTPUT);  // set up ultrasonic sensor control as output
  pinMode(echoPin, INPUT);   // set up echo signal receiving pin as input
  pinMode(relayPin, OUTPUT); // set up relay control pin as output
  Serial.begin(9600);   // run Arduino serial functionality

}

void loop() {
  
  digitalWrite(trigPin, LOW);  // start sensor trigger pin at low
  delayMicroseconds(2);        // wait 2 microseconds
  digitalWrite(trigPin, HIGH); // switch sensor trigger pin to high
  delayMicroseconds(10);       // leave trigger on for 10 microseconds to send sonic burst
  digitalWrite(trigPin, LOW);  // return trigger to low state

  duration = pulseIn(echoPin, HIGH);  // store measured sonic pulse travel time in microseconds
  distance = (duration*0.0343)/2;     // convert sonic pulse travel time to a distance value in cm

  Serial.print("Distance: ");  // display the distance reading every 0.5 seconds on Serial Monitor
  Serial.println(distance);    // console output
  delay(500);                  //

  if(distance>20){                    // switch relay state if distance greater than 20cm (test value)
    digitalWrite(relayPin,HIGH);      // restore relay state if distance less than 20cm
  } else digitalWrite(relayPin,LOW);  //

}
