/******************************************************************************
SparkFunBME280.cpp
BME280 Arduino and Teensy Driver
Marshall Taylor @ SparkFun Electronics
May 20, 2015
https://github.com/sparkfun/BME280_Breakout

Resources:
Uses Wire.h for i2c operation
Uses SPI.h for SPI operation

Development environment specifics:
Arduino IDE 1.6.4
Teensy loader 1.23

This code is released under the [MIT License](http://opensource.org/licenses/MIT).
Please review the LICENSE.md file included with this example. If you have any questions 
or concerns with licensing, please contact techsupport@sparkfun.com.
Distributed as-is; no warranty is given.
******************************************************************************/
//See SparkFunBME280.h for additional topology notes.

#include "ATtinyBME280.h"
#include <tinySPI.h>


//****************************************************************************//
//
//  Configuration section
//
//  This uses the stored SensorSettings to start the IMU
//  configure before calling .begin();
//
//****************************************************************************//
void BME280::begin()
{
	
	//Check the settings structure values to determine how to setup the device
	uint8_t dataToWrite = 0;  //Temporary variable

	//Set the oversampling control words.
	//config will only be writeable in sleep mode, so first insure that.
	writeRegister(BME280_CTRL_MEAS_REG, 0x00);

	//Set the config word
	dataToWrite = (STANDBY << 0x5) & 0xE0;
	dataToWrite |= (FILTER << 0x02) & 0x1C;
	writeRegister(BME280_CONFIG_REG, dataToWrite);

	//Set ctrl_hum first, then ctrl_meas to activate ctrl_hum
	dataToWrite = HUMID_OVERSAMPLE & 0x07; //all other bits can be ignored // hardcoced
	writeRegister(BME280_CTRL_HUMIDITY_REG, dataToWrite);

	//set ctrl_meas
	//First, set temp oversampling
	dataToWrite = (TEMP_OVERSAMPLE << 0x5) & 0xE0;
	//Next, pressure oversampling
	dataToWrite |= (PRESS_OVERSAMPLE << 0x02) & 0x1C;
	//Last, set mode
	dataToWrite |= (SLEEP_MODE) & 0x03;
	//Load the byte
	writeRegister(BME280_CTRL_MEAS_REG, dataToWrite);
	
}
//****************************************************************************//
//
//  Force a reading
//
// Set the sensor in "forced mode" to force a reading.
// After the reading the sensor will go back to sleep mode.
//
//****************************************************************************//
void BME280::forceRead( void ) {
	// We set the sensor in "forced mode" to force a reading.
    // After the reading the sensor will go back to sleep mode.
    uint8_t value = readRegister(BME280_CTRL_MEAS_REG);
    value = (value & 0xFC) + 0x01;
    writeRegister(BME280_CTRL_MEAS_REG, value);

    // Measurement Time (as per BME280 datasheet section 9.1)
    // T_max(ms) = 1.25
    //  + (2.3 * T_oversampling)
    //  + (2.3 * P_oversampling + 0.575)
    //  + (2.4 * H_oversampling + 0.575)
    //  ~ 9.3ms for current settings
    _delay_ms(10);
}

//****************************************************************************//
//
//  Pressure Section
//
//****************************************************************************//
unsigned int BME280::readPressure( void )
{

  int16_t DIG_P2 = ((int16_t)((readRegister(BME280_DIG_P2_MSB_REG) << 8) + readRegister(BME280_DIG_P2_LSB_REG)));
  int16_t DIG_P5 = ((int16_t)((readRegister(BME280_DIG_P5_MSB_REG) << 8) + readRegister(BME280_DIG_P5_LSB_REG)));
  int16_t DIG_P6 = ((int16_t)((readRegister(BME280_DIG_P6_MSB_REG) << 8) + readRegister(BME280_DIG_P6_LSB_REG)));
  int16_t DIG_P8 = ((int16_t)((readRegister(BME280_DIG_P8_MSB_REG) << 8) + readRegister(BME280_DIG_P8_LSB_REG)));
  
  // Pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8 fractional bits).
  // Output value of “24674867” represents 24674867/256 = 96386.2 Pa = 963.862 hPa
  // Returns an unsigned int
  int32_t adc_P = ((uint32_t)readRegister(BME280_PRESSURE_MSB_REG) << 12) | ((uint32_t)readRegister(BME280_PRESSURE_LSB_REG) << 4) | ((readRegister(BME280_PRESSURE_XLSB_REG) >> 4) & 0x0F);
  
  int64_t var1, var2, p_acc;
  var1 = ((int64_t)t_fine) - 128000;
  var2 = var1 * var1 * (int64_t)DIG_P6;
  var2 = var2 + ((var1 * (int64_t)DIG_P5)<<17);
  var2 = var2 + (((int64_t)DIG_P4)<<35);
  var1 = ((var1 * var1 * (int64_t)DIG_P3)>>8) + ((var1 * (int64_t)DIG_P2)<<12);
  var1 = (((((int64_t)1)<<47)+var1))*((int64_t)DIG_P1)>>33;
  if (var1 == 0)
  {
    return 0; // avoid exception caused by division by zero
  }
  p_acc = 1048576 - adc_P;
  p_acc = (((p_acc<<31) - var2)*3125)/var1;
  var1 = (((int64_t)DIG_P9) * (p_acc>>13) * (p_acc>>13)) >> 25;
  var2 = (((int64_t)DIG_P8) * p_acc) >> 19;
  p_acc = ((p_acc + var1 + var2) >> 8) + (((int64_t)DIG_P7)<<4);
  
  return (unsigned int)((p_acc / 256.0) / 10);
  
}

//****************************************************************************//
//
//  Humidity Section
//
//****************************************************************************//
unsigned char BME280::readHumidity( void )
{
  
  // Humidity in %RH as unsigned 32 bit integer in Q22. 10 format (22 integer and 10 fractional bits).
  // Output value of “47445” represents 47445/1024 = 46. 333 %RH
  // Returns an unsigned char 46 %RH
  int32_t adc_H = ((uint32_t)readRegister(BME280_HUMIDITY_MSB_REG) << 8) | ((uint32_t)readRegister(BME280_HUMIDITY_LSB_REG));
  
  int32_t var1;
  var1 = (t_fine - ((int32_t)76800));
  var1 = (((((adc_H << 14) - (((int32_t)DIG_H4) << 20) - (((int32_t)DIG_H5) * var1)) +
  ((int32_t)16384)) >> 15) * (((((((var1 * ((int32_t)DIG_H6)) >> 10) * (((var1 * ((int32_t)DIG_H3)) >> 11) + ((int32_t)32768))) >> 10) + ((int32_t)2097152)) *
  ((int32_t)DIG_H2) + 8192) >> 14));
  var1 = (var1 - (((((var1 >> 15) * (var1 >> 15)) >> 7) * ((int32_t)DIG_H1)) >> 4));
  var1 = (var1 < 0 ? 0 : var1);
  var1 = (var1 > 419430400 ? 419430400 : var1);

  return (unsigned char)((var1>>12) / 1024.0);

}



//****************************************************************************//
//
//  Temperature Section
//
//****************************************************************************//

int BME280::readTempC( void )
{
  // Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
  // t_fine carries fine temperature as global value

  //get the reading (adc_T);
  int32_t adc_T = ((uint32_t)readRegister(BME280_TEMPERATURE_MSB_REG) << 12) | ((uint32_t)readRegister(BME280_TEMPERATURE_LSB_REG) << 4) | ((readRegister(BME280_TEMPERATURE_XLSB_REG) >> 4) & 0x0F);

  //By datasheet, calibrate
  int64_t var1, var2;

  var1 = ((((adc_T>>3) - ((int32_t)DIG_T1<<1))) * ((int32_t)DIG_T2)) >> 11;
  var2 = (((((adc_T>>4) - ((int32_t)DIG_T1)) * ((adc_T>>4) - ((int32_t)DIG_T1))) >> 12) * ((int32_t)DIG_T3)) >> 14;
  t_fine = var1 + var2;
	
  return (t_fine * 5 + 128) >> 8;
}

uint8_t BME280::readRegister(uint8_t offset)
{
	//Return value
	uint8_t result;
	uint8_t numBytes = 1;

	// take the chip select low to select the device:
	digitalWrite(NSS_BME280, LOW);
	// send the device the register you want to read:
	SPI.transfer(offset | 0x80);  //Ored with "read request" bit
	// send a value of 0 to read the first byte returned:
	result = SPI.transfer(0x00);
	// take the chip select high to de-select:
	digitalWrite(NSS_BME280, HIGH);

	return result;
}

void BME280::writeRegister(uint8_t offset, uint8_t dataToWrite)
{

	// take the chip select low to select the device:
	digitalWrite(NSS_BME280, LOW);
	// send the device the register you want to read:
	SPI.transfer(offset & 0x7F);
	// send a value of 0 to read the first byte returned:
	SPI.transfer(dataToWrite);
	// decrement the number of bytes left to read:
	// take the chip select high to de-select:
	digitalWrite(NSS_BME280, HIGH);

}
