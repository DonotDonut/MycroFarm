// https://wiki.dfrobot.com/Gravity_I2C_Oxygen_Sensor_SKU_SEN0322
/*!
 * @file getOxygenData.ino
 * @brief Read oxygen concentration ,The unit is concentration percent (% vol).
 * @n step: we must first determine the iic device address, will dial the code switch A0, A1 (ADDRESS_0 for [0 0]), (ADDRESS_1 for [1 0]), (ADDRESS_2 for [0 1]), (ADDRESS_3 for [1 1]).
 * @n Then calibrate the oxygen sensor
 * @n note: it takes time to stable oxygen concentration, about 10 minutes.
 * @n The experimental phenomenon is that a certain percentage of oxygen concentration is printed on the serial port.
 * @n Because the oxygen concentration in oxygen air is certain, the data will not be greater than 25% vol.
 * @copyright Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license The MIT License (MIT)
 * @author ZhixinLiu(zhixin.liu@dfrobot.com)
 * @version V1.0
 * @date 2021-10-22
 * @url https://github.com/DFRobot/DFRobot_OxygenSensor
 */
#include "DFRobot_OxygenSensor.h" // instal sen0332 Library 

/**
 * i2c slave Address, The default is ADDRESS_3.
 * ADDRESS_0   0x70  i2c device address.
 * ADDRESS_1   0x71
 * ADDRESS_2   0x72
 * ADDRESS_3   0x73
 */

 // modified by Tim & Angelica 
#define Oxygen_IICAddress ADDRESS_3
#define COLLECT_NUMBER  10             // collect number, the collection range is 1-100.
DFRobot_OxygenSensor oxygen;

void setup(void)
{
  Serial.begin(9600);
  while(!oxygen.begin(Oxygen_IICAddress)){
    Serial.println("I2c device number error !");
    delay(1000);
    
  }
  Serial.println("I2c connect success !");
}

void loop(void)
{
  float oxygenData = oxygen.getOxygenData(COLLECT_NUMBER);
  
  // Check if the value is within the specified range
  if (oxygenData >= 18.0 && oxygenData <= 22.0) {
    Serial.print("The Oxygen Concentration is: ");
    Serial.print(oxygenData, 2);  // Display the value with 2 decimal places
    Serial.println("%vol");  // Print a newline character
  } else {
    Serial.print("The Oxygen Concentration is: ");
    Serial.print(oxygenData, 2);  // Display the value with 2 decimal places
    Serial.println("%vol. It's out of range!");
  }
  // add value to excel sheet 
  delay(2000);
  

}
