/*
 * created by Rui Santos, https://randomnerdtutorials.com 
 * File edited by Timothy Caole 
 * Complete Guide for Ultrasonic Sensor HC-SR04
 *
    Ultrasonic sensor Pins:
        VCC: +5VDC
        Trig : Trigger (INPUT) - Pin11
        Echo: Echo (OUTPUT) - Pin 12
        GND: GND
 */

#include <Servo.h>
Servo MG996R;

#include <DHT.h>
#include <LiquidCrystal.h>


#define DHT_TYPE DHT11
#define DHT_PIN 7
#define LCD_RS 12
#define LCD_EN 11
#define LCD_D4 5
#define LCD_D5 4
#define LCD_D6 3
#define LCD_D7 2
#define LCD_COLUMNS 16
#define LCD_ROWS 2


DHT dht(DHT_PIN, DHT_TYPE);
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);


#define ledPin 52
int trigPin = 7;    // Trigger
int echoPin = 8;    // Echo
long duration, cm, inches;
 



void setup() {
  //Serial Port begin
  Serial.begin (9600);

  //Define inputs and outputs
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin, OUTPUT);

  dht.begin();
  lcd.begin(LCD_COLUMNS, LCD_ROWS);
}
 
void loop() {
  LightsOn(); 
  MistAndMonitor(); 
  
  

}

/*
 * Summary of code: The sensor measure the distance and in a certain 
 * distance the led will turn off and on. 
 * 
 */ 
void LightsOn(){

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
  } else {
    // lid is off the box then led is off 
    digitalWrite(ledPin, LOW); 
  }
}

/*
 * Summary of code: The sensor measures the temperature and humidity.  
 * 
 */ 
void MistAndMonitor() {
   delay(2000);
  float humidity = dht.readHumidity();
  float temperatureC = dht.readTemperature();
  float temperatureF = (temperatureC * 9 / 5) + 32;


  if (isnan(humidity) || isnan(temperatureC)) {
    Serial.println("Failed to read from the DHT sensor!");
    lcd.clear();
    lcd.print("Sensor Error");
  } else {
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.print("%\t");
    Serial.print("Temperature: ");
    Serial.print(temperatureF);
    Serial.println("°F");


    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(temperatureF);
    lcd.print("F");
    lcd.setCursor(0, 1);
    lcd.print("Humidity: ");
    lcd.print(humidity);
    lcd.print("%");

  }
} 

