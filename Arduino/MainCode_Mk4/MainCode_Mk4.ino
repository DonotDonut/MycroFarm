/* Description of Class:
 *
 * Edited by: Timothy Caole, Angelica Quinto, Anthony Le, Enrique Arley-Imhoff, James Oelund
 * the following is derived from a combination of code from
 * https://wiki.dfrobot.com/Gravity_I2C_Oxygen_Sensor_SKU_SEN0322
 * Install following Libraries:
 */
#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DFRobot_OxygenSensor.h>
#include <DHT.h>
#include <FastLED.h>
#include <arduino-timer.h>




// Constants for O2 sensor
#define Oxygen_IICAddress ADDRESS_3
#define COLLECT_NUMBER 10
DFRobot_OxygenSensor oxygen;


// Constants for DHT sensor
#define DHT_TYPE DHT11
#define DHT_PIN 7
DHT dht(DHT_PIN, DHT_TYPE);


//Constants for fans
const int intakeFanPin1 = 2;
const int intakeFanPin2 = 3;
const int exhaustFanPin3 = 4;
const int exhaustFanPin4 = 5;


// Ultrasonic Pins
const int trigPin = 40;  // declaring Pin 40 as ultrasonic sensor contol
const int echoPin = 41;  // declaring Pin 41 as serial input from sensor


// Ultrasonic Variables
float duration, distance;  // variables to hold sound echo duration and auto timer = timer_create_default(); //calculate distance


// Relay pins
const int relayPin = 9;  // relay control Pin 9


// Constants for LED
#define DATA_PIN 8 // Define the data pin
#define NUM_LEDS 13 // Define the number of LEDs in your strip {CHECK}
CRGB leds[NUM_LEDS]; // Define the LED array




// Servo Motor Variables
//Servo Rmotor;  // Right Motor from Spray    [WILL HAVE TO GET BACK TO THIS]
//Servo Lmotor;  // Left Moto from Spray


// Constants for CO2 sensor
#define MG_PIN (A0)
#define BOOL_PIN (A2)
#define DC_GAIN (1)
#define READ_SAMPLE_INTERVAL (50)
#define READ_SAMPLE_TIMES (5)
#define ZERO_POINT_VOLTAGE (2.51 / DC_GAIN)
#define REACTION_VOLTAGE (0.030)


// CO2 Curve array
float CO2Curve[3] = { 2.602, ZERO_POINT_VOLTAGE, (REACTION_VOLTAGE / (2.602 - 3)) };


// LCD Display Object
LiquidCrystal_I2C lcd(0x27, 16, 2);


//SD Card Module CS and Object
const int chipSelect = 53;  // Change this to the CS pin connected to your SD card module
File dataFile;              // Variable name




void setup() {
  Serial.begin(9600);


  FastLED.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS); // Set up the LED strip


  //Intialize DHT11
  dht.begin();


  //Intitalize LCD
  lcd.init();
  lcd.backlight();


  // Ultrasonic Sensor
  pinMode(echoPin, INPUT);
  pinMode(trigPin, OUTPUT);


  // Motors
  //Lmotor.attach(NUM);       //[WILL HAVE TO GET BACK TO THIS]
  // Rmotor.attach(NUM);


  //Initialize Fans
  pinMode(intakeFanPin1, OUTPUT);
  pinMode(intakeFanPin2, OUTPUT);
  pinMode(exhaustFanPin3, OUTPUT);
  pinMode(exhaustFanPin4, OUTPUT);


  // Initialize Oxygen Sensor
  oxygen.begin(Oxygen_IICAddress);


  // Check if SD card is present
  if (!SD.begin(chipSelect)) {
    Serial.println("Error initializing SD card. Exiting...");
    return;
  }
  Serial.println(" ");
  Serial.println("SD card is ready to use.");


  // Open the data file for writing
  dataFile = SD.open("data.txt", FILE_WRITE);
  if (dataFile) {
    dataFile.println("Seconds Humidity Temperature O2 CO2");
    dataFile.close();
  } else {
    Serial.println("Error opening data.txt");
  }


  Serial.println("\nDEVICES POWERED SUCCESSFULLY\n");
}




void loop() {
  // Check for data collection or error
  //printErrorOrDataCollected();    //USED IN TESTING [NO LONGER NEEDED BUT EXTREMELY USEFUL FOR SENSOR ERRORS]


  Serial.println("Seconds Humidity Temperature O2 CO2");
  // Read and print CO2 sensor data
  readAndPrintCO2Sensor();


  // Read and print Oxygen data
  readAndPrintOxygenData();


  // Read and print Humidity and Temperature
  readAndPrintHumidityTemperature();


  float humidityValue = dht.readHumidity();
  float volts = MGRead(MG_PIN);
  int co2Value = MGGetPercentage(volts, CO2Curve);
  float o2Value = oxygen.getOxygenData(COLLECT_NUMBER);
  float temperatureValue = dht.readTemperature() * 9 / 5 + 32;


  //Fan Control
  fanControl(humidityValue, co2Value, o2Value, temperatureValue);


  //Humidity Control
  //ActivateSpray();  [WILL HAVE TO GET BACK TO THIS]


  //Lighting Control
  UVLightControls();


  solidColor(CRGB::Blue, 10); // Set the color to solid blue with a brightness of 50


  Serial.println("\n");
}




void printErrorOrDataCollected() {
  if (digitalRead(BOOL_PIN)) {
    Serial.println("!!! DATA ERROR !!!");
  } else {
    Serial.println("\n");
  }
  Serial.println();
}




void readAndPrintCO2Sensor() {
  float volts = MGRead(MG_PIN);
  int percentage = MGGetPercentage(volts, CO2Curve);


  // Log data to file
  dataFile = SD.open("data.txt", FILE_WRITE);
  if (dataFile) {
    dataFile.print((millis() / 1000) + 1);
    dataFile.print(" ");
    dataFile.print(dht.readHumidity());
    dataFile.print(" ");
    dataFile.print((dht.readTemperature() * 9 / 5) + 32);
    dataFile.print(" ");
    dataFile.print(oxygen.getOxygenData(COLLECT_NUMBER), 2);
    dataFile.print(" ");
    if (percentage == -1) {
      dataFile.println("<400");
    } else {
      dataFile.println(abs(percentage));
    }


    dataFile.close();  // Close the file
  } else {
    Serial.println("Error opening data.txt");
  }


  // Print to serial monitor
  Serial.print(millis() / 1000);
  Serial.print(" ");
  Serial.print(dht.readHumidity());
  Serial.print(" ");
  Serial.print((dht.readTemperature() * 9 / 5) + 32);
  Serial.print(" ");
  Serial.print(oxygen.getOxygenData(COLLECT_NUMBER), 2);
  Serial.print(" ");
  if (percentage == -1) {
    Serial.println("<400");
  } else {
    Serial.println(abs(percentage));
  }


  //print to LCD module
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CO2: ");
  if (percentage == -1) {
    lcd.print("<400 ppm");
  } else {
    lcd.print(abs(percentage));
    lcd.print(" ppm");
  }
}




void readAndPrintOxygenData() {
  float oxygenData = oxygen.getOxygenData(COLLECT_NUMBER);


  lcd.setCursor(0, 1);
  lcd.print("O2: ");
  lcd.print(oxygenData, 2);
  lcd.print("%vol");


  delay(2000);
  lcd.clear();
}




void readAndPrintHumidityTemperature() {
  float humidity = dht.readHumidity();
  float temperatureC = dht.readTemperature();
  float temperatureF = (temperatureC * 9 / 5) + 32;


  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperatureF);
  lcd.print("F");
  lcd.setCursor(0, 1);
  lcd.print("Humidity: ");
  lcd.print(humidity);
  lcd.print("%");
  delay(2000);
}




void fanControl(float humidityValue, int co2Value, float o2Value, float temperatureValue) {
  if (humidityValue < 50.00 || co2Value > 2000 || o2Value < 20.00) {  //if Humidity > 90% or Co2 > 1200 ppm or O2 < 19%


    digitalWrite(exhaustFanPin3, HIGH);
    digitalWrite(exhaustFanPin4, HIGH);  //Turn on Exhaust Fans


    digitalWrite(intakeFanPin1, HIGH);
    digitalWrite(intakeFanPin2, HIGH);  //Turn off Intake Fans


  } else if (humidityValue > 90.00 || temperatureValue > 80.00 || o2Value > 23.00) {  // If temperature > 80 or humidity > 90%


    digitalWrite(intakeFanPin1, HIGH);
    digitalWrite(intakeFanPin2, HIGH);  //Turn on Intake Fans
    digitalWrite(exhaustFanPin3, HIGH);
    digitalWrite(exhaustFanPin4, HIGH);
  } else {


    digitalWrite(intakeFanPin1, LOW);
    digitalWrite(intakeFanPin2, LOW);  //Idle when conditions are met
    digitalWrite(exhaustFanPin3, LOW);
    digitalWrite(exhaustFanPin4, LOW);
  }
}




bool runUV() { // run UV cycle with timer functionality
    digitalWrite(relayPin, HIGH);
    timer.in(60000,digitalWrite(relayPin, LOW));
    return false;
}




void UVLightControls() {
  digitalWrite(trigPin, LOW);   // start sensor trigger pin at low
  delayMicroseconds(2);         // wait 2 microseconds
  digitalWrite(trigPin, HIGH);  // switch sensor trigger pin to high
  delayMicroseconds(10);        // leave trigger on for 10 microseconds to send sonic burst
  digitalWrite(trigPin, LOW);   // return trigger to low state


  duration = pulseIn(echoPin, HIGH);   // store measured sonic pulse travel time in microseconds
  distance = (duration * 0.0343) / 2;  // convert sonic pulse travel time to a distance value in cm


  Serial.print("Distance: ");  // display the distance reading every 0.5 seconds on Serial Monitor
  Serial.print(distance);      // console output
  Serial.println(" cm");


  if (distance > 10.5) {                 
    digitalWrite(relayPin, LOW); // turn UV off if distance > 10.5cm
  } else timer.every(3600000, runUV()); // restore UV cycle if distance less than 10.5 cm
}




float MGRead(int mg_pin) {
  int i;
  float v = 0;


  for (i = 0; i < READ_SAMPLE_TIMES; i++) {
    v += analogRead(mg_pin);
    delay(READ_SAMPLE_INTERVAL);
  }
  v = (v / READ_SAMPLE_TIMES) * 5 / 1024;
  return v;
}


int MGGetPercentage(float volts, float *pcurve) {
  if ((volts / DC_GAIN) >= ZERO_POINT_VOLTAGE) {
    return -1;
  } else {
    return pow(10, ((volts / DC_GAIN) - pcurve[1]) / pcurve[2] + pcurve[0]);
  }
}


void solidColor(CRGB color, int brightness) {
  FastLED.setBrightness(brightness); // Set the brightness


  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = color; // Set the color for each LED
  }


  FastLED.show(); // Update the LED strip
  delay(1000);     // Keep the color for 1 second (adjust as needed)
}




/***
void ActivateSpray() {
  if
    !(85 < humidity < 90) {  // if humidity is not in ideal conditions, 80% - 90%
                             // spray actived, motors activate spray
      Lmotor.write(90);
      Rmotor.write(90);
    }
  else {
    // spray not activated, motors go to resting position
    Lmotor.write(0);
    Rmotor.write(0);
  }
}
***/


