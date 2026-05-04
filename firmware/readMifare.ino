/**************************************************************************/
/*! 	
	Ejemplo modificado de lectura de tarjetas MIFARE
		This example will wait for any ISO14443A card or tag, and
		depending on the size of the UID will attempt to read from it.
	 
		If the card has a 4-byte UID it is probably a Mifare
		Classic card, and the following steps are taken:
	 
		- Authenticate block 4 (the first block of Sector 1) using
			the default KEYA of 0XFF 0XFF 0XFF 0XFF 0XFF 0XFF
		- If authentication succeeds, we can then read any of the
			4 blocks in that sector (though only block 4 is read here)
	 
		If the card has a 7-byte UID it is probably a Mifare
		Ultralight card, and the 4 byte pages can be read directly.
		Page 4 is read by default since this is the first 'general-
		purpose' page on the tags.

		To enable debug message, define DEBUG in PN532/PN532_debug.h
*/
/**************************************************************************/

// #if 0
//	 #include <SPI.h>
//	 #include <PN532_SPI.h>
//	 #include "PN532.h"

//	 PN532_SPI pn532spi(SPI, 10);
//	 PN532 nfc(pn532spi);
// #elif 0
//	 #include <PN532_HSU.h>
//	 #include <PN532.h>
			
//	 PN532_HSU pn532hsu(Serial1);
//	 PN532 nfc(pn532hsu);
// #else 
//	 #include <Wire.h>
//	 #include <PN532_I2C.h>
//	 #include <PN532.h>
//	 PN532_I2C pn532i2c(Wire);
//	 PN532 nfc(pn532i2c);	
// #endif

//Bibliotecas cuando se usa i2C (menos alambres)
#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>
PN532_I2C pn532i2c(Wire);
PN532 nfc(pn532i2c);	

void rgb(bool R, bool G, bool B)
{
	//invertidos para arduino nano
	digitalWrite(LED_RED, !R);
	digitalWrite(LED_GREEN, !G);
	digitalWrite(LED_BLUE, !B);
}

void setup(void) {
	
	pinMode(LED_BUILTIN, OUTPUT);
	pinMode(LED_RED, OUTPUT);
	pinMode(LED_GREEN, OUTPUT);
	pinMode(LED_BLUE, OUTPUT);
	rgb(0,0,0);
	Serial.begin(115200);
	delay(1000);
	Serial.println("Inicializando lector NFC");

	nfc.begin();

	uint32_t versiondata = nfc.getFirmwareVersion();
	if (! versiondata) {
		Serial.print("Didn't find PN53x board");
		while (1); // halt
	}
	// Got ok data, print it out!
	Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
	Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
	Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
	
	// configure board to read RFID tags
	nfc.SAMConfig();
	
	Serial.println("Waiting for an ISO14443A Card ...");
}


void loop(void) {
	uint8_t success;
	uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };	// Buffer to store the returned UID
	uint8_t uidLength;												// Length of the UID (4 or 7 bytes depending on ISO14443A card type)
		
	// Wait for an ISO14443A type cards (Mifare, etc.).	When one is found
	// 'uid' will be populated with the UID, and uidLength will indicate
	// if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
	success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
	if (success) {
		Serial.print("	UID Value: ");
		nfc.PrintHex(uid, uidLength);
		//probablemente sea un fono
		if (uidLength == 4)
		{
			Serial.print("UID lenght is 4");
			uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
			success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, keya);
			if (success)
			{
				uint8_t data[16];
				success = nfc.mifareclassic_ReadDataBlock(4, data);
				if (success)
				{
					// Data seems to have been read ... spit it out
					Serial.println("Reading Block 4:");
					nfc.PrintHexChar(data, 16);
					Serial.println("");
					// Wait a bit before reading the card again
					delay(1000);
				}
				else
				{
					Serial.println("error reading block data");
				}
			}
			else
			{
				Serial.println("authentication failed");
				delay(1000);
			}
		}
		if (uidLength == 7)
		{
			// We probably have a Mifare Ultralight card ...
			Serial.println("Seems to be a Mifare Ultralight tag (7 byte UID)");
		
			// Try to read the first general-purpose user page (#4)
			Serial.println("Reading page 4");
			uint8_t data[32];
			success = nfc.mifareultralight_ReadPage (4, data);
			if (success)
			{
				// Data seems to have been read ... spit it out
				nfc.PrintHexChar(data, 4);
				Serial.println("");
				rgb(0,1,0);
				delay(250);
				rgb(0,0,0);
				// Wait a bit before reading the card again
				delay(750);
			}
			else
			{
				Serial.println("Ooops ... unable to read the requested page!?");
				rgb(1,0,0);
				delay(250);
				rgb(0,0,0);
			}
		}
	}	
	
}

