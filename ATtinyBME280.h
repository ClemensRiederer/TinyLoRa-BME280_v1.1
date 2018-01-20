/******************************************************************************
SparkFunBME280.h
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

// Test derived class for base class SparkFunIMU
#ifndef __BME280_H__
#define __BME280_H__

#include <Arduino.h>
#include "Pins.h"


//Register names:
#define BME280_DIG_P2_LSB_REG			0x90
#define BME280_DIG_P2_MSB_REG			0x91
#define BME280_DIG_P5_LSB_REG			0x96
#define BME280_DIG_P5_MSB_REG			0x97
#define BME280_DIG_P6_LSB_REG			0x98
#define BME280_DIG_P6_MSB_REG			0x99
#define BME280_DIG_P8_LSB_REG			0x9C
#define BME280_DIG_P8_MSB_REG			0x9D

#define BME280_CHIP_ID_REG				0xD0 //Chip ID
#define BME280_RST_REG					0xE0 //Softreset Reg
#define BME280_CTRL_HUMIDITY_REG		0xF2 //Ctrl Humidity Reg
#define BME280_STAT_REG					0xF3 //Status Reg
#define BME280_CTRL_MEAS_REG			0xF4 //Ctrl Measure Reg
#define BME280_CONFIG_REG				0xF5 //Configuration Reg
#define BME280_PRESSURE_MSB_REG			0xF7 //Pressure MSB
#define BME280_PRESSURE_LSB_REG			0xF8 //Pressure LSB
#define BME280_PRESSURE_XLSB_REG		0xF9 //Pressure XLSB
#define BME280_TEMPERATURE_MSB_REG		0xFA //Temperature MSB
#define BME280_TEMPERATURE_LSB_REG		0xFB //Temperature LSB
#define BME280_TEMPERATURE_XLSB_REG		0xFC //Temperature XLSB
#define BME280_HUMIDITY_MSB_REG			0xFD //Humidity MSB
#define BME280_HUMIDITY_LSB_REG			0xFE //Humidity LSB

// calibration values from the register hardcoded
#define DIG_T1 28500
#define DIG_T2 26420
#define DIG_T3 50
#define DIG_P1 37988
#define DIG_P3 3024
#define DIG_P4 10752
#define DIG_P7 9900
#define DIG_P9 4285
#define DIG_H1 75
#define DIG_H2 361
#define DIG_H3 0
#define DIG_H4 321
#define DIG_H5 50
#define DIG_H6 30

//Settings and configuration
//#define NORMAL_MODE 0x03 // Normal mode
#define SLEEP_MODE 0x00// sleep mode
#define FILTER 0x00   // filter off
#define STANDBY 0x00  // 0.5ms
#define TEMP_OVERSAMPLE 0x01  // 1 * oversampling
#define PRESS_OVERSAMPLE 0x01 // 1 * oversampling
#define HUMID_OVERSAMPLE 0x01 // 1 * oversampling


//This is the man operational class of the driver.

class BME280
{
  public:

    int32_t t_fine;

    //This also gets the SensorCalibration constants
    void begin( void );
    
    // force reading an go to sleep
    void forceRead( void );
	
    //Returns the values.
    unsigned int readPressure( void );
    unsigned char readHumidity( void );
    int readTempC( void );

    //Reads one register
    uint8_t readRegister(uint8_t);

    //Writes a byte;
    void writeRegister(uint8_t, uint8_t);
    
};
#endif
