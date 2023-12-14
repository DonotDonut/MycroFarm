#include <DHT.h>
#include <LiquidCrystal.h>
#include <Servo.h>

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


Servo MG996R; // motor object to control servo motor 
int pos = 0;    // variable to store the servo position


void setup() {
  Serial.begin(9600);
  dht.begin(); // Initalize the DHT Sensor (DHT11)
  lcd.begin(LCD_COLUMNS, LCD_ROWS); // Initalize the LCD display 

  MG996R.attach(13); // Attached servo motor to pin 13 
}


void loop() {
  delay(2000); // delay for 2 secs 
  float humidity = dht.readHumidity(); // read humidity from DHT sensor 
  float temperatureC = dht.readTemperature(); // read temperature (in celsius) from DHT sensor 
  float temperatureF = (temperatureC * 9 / 5) + 32; // convert celsius to fahrenheit  Temperature

  // check if sensor readings are valid 
  if (isnan(humidity) || isnan(temperatureC)) { 
    // error message is sent to the serial monitor and displayed on he LCD display
    Serial.println("Failed to read from the DHT sensor!");
    lcd.clear();
    lcd.print("Sensor Error");
  } else {
    // Prints the humidity and sensor reading on the serial monitor 
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.print("%\t");
    Serial.print("Temperature: ");
    Serial.print(temperatureF);
    Serial.println("°F");

  // update LCD display w/ temperature and humidity readings 
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


 /* Checking for idea conditions 
  * Temperature:  65-70 fahrenheit  (81-21 celsius )
  * Humidity: +90-95% 
  */ 

  if (humidity < 90) {
    // Motor presses down spary bottle, spraying the the mushroom
    for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    MG996R.write(pos);              // tell servo to go to position in variable 'pos'
    delay(500);                       // waits 500 ms for the servo to reach the position
  }
  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    MG996R.write(pos);              // tell servo to go to position in variable 'pos'
    delay(500);                       // waits 500 ms for the servo to reach the position
  }
  }  

  if (temperatureF > 65) {
    // fan off 
    // uv lights on 
  } 

  if (temperatureF > 70) {
    // fan on 
    // uv lights off
  } 
}
