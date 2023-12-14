#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DFRobot_OxygenSensor.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define MG_PIN (A0)
#define BOOL_PIN (A2)
#define DC_GAIN (1)
#define READ_SAMPLE_INTERVAL (50)
#define READ_SAMPLE_TIMES (5)
#define ZERO_POINT_VOLTAGE (2.51 / DC_GAIN)
#define REACTION_VOLTAGE (0.030)
#define DHT_TYPE DHT11
#define DHT_PIN 7
#define Oxygen_IICAddress ADDRESS_3
#define COLLECT_NUMBER 10

DFRobot_OxygenSensor oxygen;
DHT dht(DHT_PIN, DHT_TYPE);
float CO2Curve[3] = {2.602, ZERO_POINT_VOLTAGE, (REACTION_VOLTAGE / (2.602 - 3))};

void setup() {
  Serial.begin(9600);
  dht.begin();
  lcd.init();
  lcd.backlight();

  while (!oxygen.begin(Oxygen_IICAddress)) {
    Serial.println("O2 Sensor Error!");
    delay(10000);
  }
  Serial.println("Sensor Connected!");

  pinMode(BOOL_PIN, INPUT);
  digitalWrite(BOOL_PIN, HIGH);

  Serial.print("\n");
  Serial.print("DEVICES POWERED SUCCESSFULLY");
  Serial.print("\n");
}

void printErrorOrDataCollected() {
  if (digitalRead(BOOL_PIN)) {
    Serial.print("===DATA COLLECTED===");
  } else {
    Serial.print("!!!ERROR [CHECK CONNECTIONS]!!!");
  }
  Serial.print("\n");
}

void readAndPrintCO2Sensor() {
  float volts = MGRead(MG_PIN);
  int percentage = MGGetPercentage(volts, CO2Curve);

  Serial.print(" ***SENSOR VALUES***");
  Serial.print("\n");
  Serial.print("Sensor Voltage Reading:");
  Serial.print(volts);
  Serial.print("V           ");
  Serial.print("CO2 Concentration:");
  if (percentage == -1) {
    Serial.print("<400");
  } else {
    Serial.print(percentage);
  }
  Serial.print("ppm");
  Serial.print("\n");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CO2: ");
  if (percentage == -1) {
    lcd.print("<400 ppm");
  } else {
    lcd.print(percentage);
    lcd.print(" ppm");
  }
}

void readAndPrintOxygenData() {
  float oxygenData = oxygen.getOxygenData(COLLECT_NUMBER);

  Serial.print("The Oxygen Concentration is: ");
  Serial.print(oxygenData, 2);
  Serial.println("%vol");

  lcd.setCursor(0, 1);
  lcd.print("O2: ");
  lcd.print(oxygenData, 2);
  lcd.print("%vol");
  delay(2750);
  lcd.clear();
}

void readAndPrintHumidityTemperature() {
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

void loop() {
  printErrorOrDataCollected();
  readAndPrintCO2Sensor();
  readAndPrintOxygenData();
  readAndPrintHumidityTemperature();

  delay(2750);
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

