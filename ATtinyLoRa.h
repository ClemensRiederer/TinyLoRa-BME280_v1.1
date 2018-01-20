#ifndef TINY_LORA_H
#define TINY_LORA_H

#include "Pins.h"
#include <Arduino.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

class TinyLoRa
{
	public:
	uint16_t frameCounter;
	void begin(void);
	void sendData(unsigned char *Data, unsigned char Data_Length, unsigned int Frame_Counter_Tx);

	private:
	uint8_t randomNum;
	static const unsigned char S_Table[16][16];
	void RFM_Send_Package(unsigned char *RFM_Tx_Package, unsigned char Package_Length);
	void RFM_Write(unsigned char RFM_Address, unsigned char RFM_Data);
	unsigned char RFM_Read(unsigned char RFM_Address);
	void Encrypt_Payload(unsigned char *Data, unsigned char Data_Length, unsigned int Frame_Counter, unsigned char Direction);
	void Calculate_MIC(unsigned char *Data, unsigned char *Final_MIC, unsigned char Data_Length, unsigned int Frame_Counter, unsigned char Direction);
	void Generate_Keys(unsigned char *K1, unsigned char *K2);
	void Shift_Left(unsigned char *Data);
	void XOR(unsigned char *New_Data,unsigned char *Old_Data);
	void AES_Encrypt(unsigned char *Data, unsigned char *Key);
	void AES_Add_Round_Key(unsigned char *Round_Key, unsigned char (*State)[4]);
	unsigned char AES_Sub_Byte(unsigned char Byte);
	void AES_Shift_Rows(unsigned char (*State)[4]);
	void AES_Mix_Collums(unsigned char (*State)[4]);
	void AES_Calculate_Round_Key(unsigned char Round, unsigned char *Round_Key);

};

#endif
