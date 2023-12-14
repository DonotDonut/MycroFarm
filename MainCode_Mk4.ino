/* 
  Written by Enrique Imhoff, Timothy Caole, James Ivan, Anthony Le, Angelica Quinto 

  Controls the LED light strips for the RGB Gamer Chair and provides
  different modalities for various lighting effects, both user-controlled
  and automatically triggered.

  References used:
  - For CO2 Sensor: https://wiki.dfrobot.com/CO2_Sensor_SKU_SEN0159 
  - For Ultrasonic Sensor: https://randomnerdtutorials.com/complete-guide-for-ultrasonic-sensor-hc-sr04/ 
  - For O2 Sensor: https://wiki.dfrobot.com/Gravity_I2C_Oxygen_Sensor_SKU_SEN0322 
  - For Fans: 
  - For 
 */

#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h> // install LquidyCrystal I2C by Frank de Brabander 
#include <DFRobot_OxygenSensor.h> // install DFRobot_BloodOxygenSensor by DRRobot 
#include <DHT.h> // Install DHT sensor Library by Adafruit
#include <FastLED.h> // Install FastLED by Daniel Grarcia 

// Constants for O2 sensor
#define Oxygen_IICAddress ADDRESS_3
#define COLLECT_NUMBER 10

// Constants for DHT sensor
#define DHT_TYPE DHT11
#define DHT_PIN 7

//Constants for fans
const int intakeFanPin1 = 2;
const int intakeFanPin2 = 3;
const int exhaustFanPin3 = 4;
const int exhaustFanPin4 = 5;

// Contstans for Ultrasonic 
const int trigPin = 11;   
const int echoPin = 12;   
float duration, distance; // variables to hold sound echo duration and calculate distance

// Constants for Relay 
const int relayPin = 8;  // relay control Pin 8

// Constants for LED 
#define NUM_COLORS_TWINKLE 5
#define NUM_COLORS_SOLID 9
#define NUM_LEDS 61
#define LED_BRIGHTNESS 64
// Object for the LED strip color palette
CRGB g_LEDs[NUM_LEDS] = {0};

// Constants for CO2 sensor
#define MG_PIN (A0) //mg? 
#define BOOL_PIN (A2) 
#define DC_GAIN (1)
#define READ_SAMPLE_INTERVAL (50)
#define READ_SAMPLE_TIMES (5)
#define ZERO_POINT_VOLTAGE (2.51 / DC_GAIN) // why? 
#define REACTION_VOLTAGE (0.030) // why? 

// Objects and instances
DFRobot_OxygenSensor oxygen;
DHT dht(DHT_PIN, DHT_TYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int chipSelect = 53;  // Change this to the CS pin connected to your SD card module
File dataFile;              // Corrected variable name

// CO2 Curve array
float CO2Curve[3] = { 2.602, ZERO_POINT_VOLTAGE, (REACTION_VOLTAGE / (2.602 - 3)) };

void setup() {
  Serial.begin(9600);
  
  //Initalize Ultarsonic
  pinMode(trigPin, OUTPUT);  // set up ultrasonic sensor control as output
  pinMode(echoPin, INPUT);   // set up echo signal receiving pin as input

  // Initalize Relay
  pinMode(relayPin, OUTPUT); // set up relay control pin as output

  //Intialize DHT11
  dht.begin();

  //Intitalize LCD
  lcd.init();
  lcd.backlight();

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
  if (humidityValue < 20.00 || co2Value > 2000 || o2Value < 19.00) {  //if Humidity > 90% or Co2 > 1200 ppm or O2 < 19%

    digitalWrite(exhaustFanPin3, HIGH);
    digitalWrite(exhaustFanPin4, HIGH);  //Turn on Exhaust Fans

    digitalWrite(intakeFanPin1, LOW);
    digitalWrite(intakeFanPin2, LOW);  //Turn off Intake Fans

  } else if (humidityValue > 90.00 || temperatureValue > 80.00 ||  o2Value > 23.00) {  // If temperature > 80 or humidity > 90%

    digitalWrite(intakeFanPin1, HIGH);
    digitalWrite(intakeFanPin2, HIGH);  //Turn on Intake Fans

    // Turn off exhaust fans
    digitalWrite(exhaustFanPin3, LOW);  //Turn off Exhaust Fans
    digitalWrite(exhaustFanPin4, LOW);
  } else {

    digitalWrite(intakeFanPin1, LOW);
    digitalWrite(intakeFanPin2, LOW);  //Idle when conditions are met
    digitalWrite(exhaustFanPin3, LOW);
    digitalWrite(exhaustFanPin4, LOW);
  }
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

void UVLightControls (){
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

/* 
  FastLED color palette for solidMode
 */
static const CRGB solidColors[NUM_COLORS_SOLID] = 
{
  CRGB::Red,
  CRGB::Green,
  CRGB::Blue,
  CRGB::Magenta,
  CRGB::Yellow,
  CRGB::Cyan,
  CRGB::Purple,
  CRGB::White,
  CRGB::Gray
};