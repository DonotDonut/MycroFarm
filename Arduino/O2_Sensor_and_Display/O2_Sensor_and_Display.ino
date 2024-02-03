
/*
  * Code created by ZhixinLiu (zhixin.liu@dfrobot.com)
  * https://wiki.dfrobot.com/Gravity_I2C_Oxygen_Sensor_SKU_SEN0322
  * Code modified by Angelica Quinto and Timothy Caole

  *I2C Oxygen Sensor Pins for mega
    * VCC: 5V
    * SDA: SDA
    * SCL: SCL
    * GND: GND
  *

  *Summary of code: The sensor measures the oxygen in the air. 
  *The range of oxygen level needed is 18-21% or 180k-210 ppm.
  *If the measure is out of range, outputs a message of "out of range". 

*/
#include <DFRobot_OxygenSensor.h>
#include <LCD03.h>

#define Oxygen_IICAddress ADDRESS_3
#define COLLECT_NUMBER  10    // Collect number, the collection range is 1-100.

DFRobot_OxygenSensor oxygen;
LCD03 lcd; 

void setup(void)
{
  Serial.begin(9600);
  lcd.begin(16, 2);

  while (!oxygen.begin(Oxygen_IICAddress))
  {
    Serial.println("I2C device number error!");
    delay(1000);
  }

  Serial.println("I2C connect success!");
  lcd.begin(16, 2);
}

// A loop that takes O2 input and sends a message if it is in and out of range with measurement.  
void loop(void)
{
  float oxygenData = oxygen.getOxygenData(COLLECT_NUMBER); // Collects oxygen data in float
  
  // Check if the value is within the specified range
  if (oxygenData >= 18.0 && oxygenData <= 22.0)
  { 
    // Range of ideal oxygen measurements
    Serial.print("The Oxygen Concentration is: ");
    Serial.print(oxygenData, 2);  // Display the value with 2 decimal places
    Serial.println("%vol");  // Print a newline character
    
    lcd.clear();
    lcd.print("O2: ");
    lcd.print(oxygenData, 2);
  }
  else
  { 
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


