#include <ATtinyBME280.h>
#include <ATtinyLoRa.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <tinySPI.h>

#define SLEEP_TOTAL 37 // 37*8s = 296s ~5min

// use your keys from TTN
uint8_t NwkSkey[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t AppSkey[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t DevAddr[4] = { 0x00, 0x00, 0x00, 0x00 };

// sleep cycles
volatile uint8_t sleep_count = 0;

// Global sensor object
BME280 bme280;
// Global LoRaWan object
TinyLoRa lora;

void setup()
{
  
  // set slave select pins from RFM95 and BME280 as outputs
  DDRB |= (1 << NSS_BME280) | (1 << NSS_RFM);
  // set NSS_BME280 and NSS_RFM high
  PORTB |= (1 << NSS_BME280) | (1 << NSS_RFM);
  
  // Turn on the watch dog timer
  watchdogOn();

  SPI.begin();
  
  // init LoRa
  lora.begin();
  // init BME280
  bme280.begin();
  
  SPI.end();
}

void loop()
{
  //Disable ADC 
  ADCSRA &= ~(1<<ADEN); 
  
  goToSleep();

  if (sleep_count >= SLEEP_TOTAL) { 

    SPI.begin();
    
    bme280.forceRead();
    
    int16_t temp = bme280.readTempC();
    uint8_t hum = bme280.readHumidity();
    uint16_t pressure = bme280.readPressure();
  
    unsigned char Data[5];
    Data[0] = (temp >> 8) & 0xff;
    Data[1] = temp & 0xff;
    Data[2] = hum & 0xff;
    Data[3] = (pressure >> 8) & 0xff;
    Data[4] = pressure & 0xff;
   
    lora.sendData(Data, sizeof(Data), lora.frameCounter);
    lora.frameCounter++;
    
    // reset sleep count
    sleep_count = 0;

    SPI.end();
    
    // set DO, SCK as input and low prevents current draw in sleep mode.
    DDRB &= ~((1 << PB1) | (1 << PB2));
    PORTB &= ~((1 << PB0) | (1 << PB1) | (1 << PB2));
  }
}

void watchdogOn() {
  // clear various "reset" flags
  MCUSR = 0;
  // allow changes, disable reset
  WDTCR = (1 << WDCE) | (1 << WDE);
  // set interrupt mode and an interval 
  WDTCR = (1 << WDIE) | (1 << WDP3) | (1 << WDP0); // set WDIE, and 8 seconds delay
  wdt_reset();  // pat the dog
}

void goToSleep() {
  // Set sleep mode.
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  // Enter sleep mode.
  sleep_cpu();
  // After waking from watchdog interrupt the code continues from this point.
  
  // Disable sleep mode after waking.
  sleep_disable();
}

ISR(WDT_vect) {
  sleep_count++; // keep track of how many sleep cycles have been completed.
}
