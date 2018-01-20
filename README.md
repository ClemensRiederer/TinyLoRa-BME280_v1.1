# TinyLoRa-BME280 v1.1
Arduino library for a Atmospheric Sensor BME280 LoRaWan Node.
The LoRa Node measures barometric pressure, humidity, and temperature every 15 min. 
After the measurements, the ATtiny85 goes into sleep mode and is awakened by the watchdog timer.
The RFM module sends the values to the TTN backend with Activation by Personalization (ABP) 
a fixed spreading factor and one of the four random channels.
The example sketch that fits in the 8K Bytes Flash Memory of the ATtiny85

# installing
Install the zip file in your Arduino IDE.
Click on the toolbar menu: Sketch -> Include Library -> Add .ZIP Library -> "downloaded zip file"
Once done click OK to save and restart the Arduino IDE.

This library uses the tinySPI library http://github.com/JChristensen/tinySPI
for communication with the BME280 and the Hope RFM95 module.
Install this library too.

# changes
Change the NwkSkey,AppSkey,DevAddr acording your own TTN keys.
Change the interval of measurements and radio transmission with the SLEEP_TOTAL var
Change Spreading factor as you like.

# needed parts
* 1x ATtiny85V 10PU Microcontroller
* 1x BME280 Breakout Board
* 1x RFM95 Transceiver Module
* 1x Breadboard
* 2x Li-Ion Rechargeable Batteries 1.2 V
* 1x Battery Holder
* 1x 22 AWG Solid Wire
* 1x Arduino UNO (for programming)
* Jumper Wires for programming

# wiring everything up
* ATtiny85VCC ---> Breadboard VCC
* ATtiny85 GND ---> Breadboard GND
* ATtiny85 PB3 ---> CS BME280 (Slave Select)
* ATtiny85 PB4 ---> NSS RFM95 (Slave Select)
* ATtiny85 PB2 ---> SCK RFM59  ---> SCK BME280
* ATtiny85 PB1(DO) ---> MOSI RFM95 ---> SDI BME280 (Data going into the BME280)
* ATtiny85 PB0(DI) ---> MISO RFM95 ---> SDO BME280 (Data coming out of the BME280)

