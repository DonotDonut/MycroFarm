/* Description of Class: 
 *
 * Edited by: Timothy Caole, Angelica Quinto
 * the following is derived from a combination of code from
 * https://wiki.dfrobot.com/Gravity_I2C_Oxygen_Sensor_SKU_SEN0322 
 * 
 *
 * 
 * Install following Libraries: 
 * 
 * 
 * 
 *
 */ 


#include <DHT.h> // Library for Humidity and Temperature DHT-11 Sensor 
#include <Wire.h> // Library for ? 
#include <LiquidCrystal_I2C.h> // Library for DHT-11 Sensor 
#include <Servo.h> // Library for Motor, using LKY62 and MG996R Digi Hi Torque
#include <DFRobot_OxygenSensor.h> // Library for O2 Sensor
#include <LCD03.h> // ? 

LiquidCrystal_I2C lcd(0x27, 16, 2);  // Set the LCD I2C address, 16 columns, 2 rows

#define MG_PIN (A0)
#define BOOL_PIN (A2)
#define DC_GAIN (1)
#define READ_SAMPLE_INTERVAL (50)
#define READ_SAMPLE_TIMES (5)
#define ZERO_POINT_VOLTAGE (2.42 / DC_GAIN)
#define REACTION_VOLTAGE (0.030)

// Humidity & Temperature Sensor DHT11 Pins 
#define DHT_TYPE DHT11
#define DHT_PIN 7

// Ultrasonic Pins  
const int trigPin = 9;   // declaring Pin 9 as ultrasonic sensor contol
const int echoPin = 10;   // declaring Pin 10 as serial input from sensor

// Ultrasnoic Variables
float duration, distance; // variables to hold sound echo duration and calculate distance

// Relay pins 
const int relayPin = 8;  // relay control Pin 8

// Servo Motor Variables 
Servo Rmotor; // Right Motor from Spray 
Servo Lmotor; // Left Moto from Spray 

// O2 Sensor Variables 
#define Oxygen_IICAddress ADDRESS_3
#define COLLECT_NUMBER  10    // Collect number, the collection range is 1-100.
DFRobot_OxygenSensor oxygen;
LCD03 lcd; 


DHT dht(DHT_PIN, DHT_TYPE);
float CO2Curve[3] = { 2.602, ZERO_POINT_VOLTAGE, (REACTION_VOLTAGE / (2.602 - 3)) };

void setup() {
  Serial.begin(9600); // Serial Port Begins
  dht.begin();
  pinMode(BOOL_PIN, INPUT);
  digitalWrite(BOOL_PIN, HIGH);
  Serial.print("\n");
  Serial.print("DEVICES POWERED SUCCESSFULLY");
  Serial.print("\n");

  // LCD 
  lcd.init();       //initialize the lcd
  lcd.backlight();  //open the backlight

  // Ultrasonic Sensor 
  pinMode(echoPin, INPUT);
  pinMode(ledPin, OUTPUT);

  // Motors 
  Lmotor.attach(11);
  Rmotor.attach(9);

  // O2 Sensor 
  while (!oxygen.begin(Oxygen_IICAddress)) { // Checking O2 Sensor Connection
    Serial.println("I2C device number error!");
    delay(1000);
  }
  Serial.println("I2C connect success!");
  lcd.begin(16, 2);
}

void loop() {
  //------------------------------------------------------------------------------------------
  //Variables
  int percentage;
  float volts;
  float humidity = dht.readHumidity();
  float temperatureC = dht.readTemperature();
  float temperatureF = (temperatureC * 9 / 5) + 32;
  //---------------------------------------------------------------------------------------

  //----------------------------------------------------------------------------------------
  //ERROR CHECKING
  if (digitalRead(BOOL_PIN)) {
    Serial.print("===DATA COLLECTED===");
  } else {
    Serial.print("!!!ERROR [CHECK CONNECTIONS]!!!");
  }
  Serial.print("\n");
  //----------------------------------------------------------------------------------------

  volts = MGRead(MG_PIN);
  percentage = MGGetPercentage(volts, CO2Curve);

  //----------------------------------------------------------------------------------------
  //SENSOR VOLTAGE READING (SERIAL)
  Serial.print(" ***SENSOR VALUES***");
  Serial.print("\n");
  volts = MGRead(MG_PIN);
  Serial.print("Sensor Voltage Reading:");
  Serial.print(volts);
  Serial.print("V           ");
  Serial.print(" ");

  // -------------------------------------------------------------------------------------
  //LCD MODULE FOR VOLTAGE READINGS
  //lcd.setCursor(0, 0);
  //lcd.print("Voltage: ");
  //lcd.print(volts);
  //lcd.print("V           ");
  //lcd.print(" ");
  //--------------------------------------------------------------------------------

  //----------------------------------------------------------------------------------
  //CO2 READING (SERIAL)
  percentage = MGGetPercentage(volts, CO2Curve);
  Serial.print("CO2 Concentration:");
  if (percentage == -1) {
    Serial.print("<400");
  } else {
    Serial.print(percentage);
  }

  Serial.print("ppm");
  Serial.print("\n");
  //-------------------------------------------------------------------------------------


  //-------------------------------------------------------------------------------------
  // LCD MODULE FOR CO2
  lcd.setCursor(0, 0);
  lcd.print("CO2: ");
  if (percentage == -1) {
    lcd.print("<400 ppm");
  } else {
    lcd.print(percentage);
    lcd.print(" ppm");
  }
  delay(2750);
  lcd.clear();
  //-------------------------------------------------------------------------------------------

  //-------------------------------------------------------------------
  //HUMIDITY SENSOR MODULE
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

    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(temperatureF);
    lcd.print("F");
    lcd.setCursor(0, 1);
    lcd.print("Humidity: ");
    lcd.print(humidity);
    lcd.print("%");
  
  }
  //-----------------------------------------------------------------

  //-----------------------------------------------------------------
  delay(2750); //2.75 seconds 

  ActivateSpray(); 
  UVLightControls();
}


/* 
Description of Method: 
*/
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

/*
 * Description of Method: 
 * Arduino controls UV light and LED based on ultrasonic sensor reading 
 */
void UVLightControls(){
  digitalWrite(trigPin, LOW);  // start sensor trigger pin at low
  delayMicroseconds(2);        // wait 2 microseconds
  digitalWrite(trigPin, HIGH); // switch sensor trigger pin to high
  delayMicroseconds(10);       // leave trigger on for 10 microseconds to send sonic burst
  digitalWrite(trigPin, LOW);  // return trigger to low state

  duration = pulseIn(echoPin, HIGH);  // store measured sonic pulse travel time in microseconds
  distance = (duration*0.0343)/2;     // convert sonic pulse travel time to a distance value in cm

  Serial.print("Distance: ");  // display the distance reading every 0.5 seconds on Serial Monitor
  Serial.print(distance);    // console output
  Serial.println(" cm");
  delay(500);                  //

  if(distance>20){                    // switch relay state if distance greater than 20cm (test value)
    digitalWrite(relayPin,HIGH);      // restore relay state if distance less than 20cm
  } else digitalWrite(relayPin,LOW);  //

}

/*
 * Description of Method: 
 * Arduino activates motor to spray water on the mushroom based on humidity sensor reading 
 */
void ActivateSpray(){
    if !( 85< humidity < 90) { // if humidity is not in ideal conditions, 80% - 90% 
  // spray actived, motors activate spray 
    Lmotor.write(90);
    Rmotor.write(90);
  } else {
    // spray not activated, motors go to resting position 
    Lmotor.write(0);
    Rmotor.write(0);
  }
}

void O2SensorReading(){
  float oxygenData = oxygen.getOxygenData(COLLECT_NUMBER); // Collects oxygen data in float
  
  // Check if the value is within the specified range
  if (oxygenData >= 18.0 && oxygenData <= 22.0) { 
    // Range of ideal oxygen measurements
    Serial.print("The Oxygen Concentration is: ");
    Serial.print(oxygenData, 2);  // Display the value with 2 decimal places
    Serial.println("%vol");  // Print a newline character
    
    lcd.clear();
    lcd.print("O2: ");
    lcd.print(oxygenData, 2);
  } else { 
    // Out of range 
    Serial.print("The Oxygen Concentration is: ");
    Serial.print(oxygenData, 2);  // Display the value with 2 decimal places
    Serial.println("%vol. It's out of range!");
    
    lcd.clear();
    lcd.print("O2: ");
    lcd.print(oxygenData, 2);
  }

  delay(2000); // Delays for 2ms until loops back again 
}