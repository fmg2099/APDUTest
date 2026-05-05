/**************************************************************************/
/*! 	
	Ejemplo de lectura de tarjetas ISO14443A con un AID específico
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
  uint8_t response[255];
  uint8_t responseLength;
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, response, &responseLength);
  if (success) {
    Serial.println("Found a device!");
    // 2. Select the Application ID F2233445566
    // Structure: CLA INS P1 P2 Lc [DATA] Length
    uint8_t selectApp[] = { 
        0x00, 0xA4, 0x04, 0x00, 0x06, 
        0xF2, 0x23, 0x34, 0x45, 0x56, 0x60
    };
    uint8_t relayResponse[64];
    uint8_t relayResponseLength = 64;

    success = nfc.inDataExchange(selectApp, sizeof(selectApp), relayResponse, &relayResponseLength);

    if (success) {
      Serial.print("Response from Android: ");
      nfc.PrintHexChar(relayResponse, relayResponseLength);
      
      // If Android returns 0x90 0x00, it means the connection is established!
      if (relayResponse[relayResponseLength-2] == 0x90) {
          Serial.println("Handshake Success!");
      }
    } else {
      Serial.println("Failed to select AID. Is the app in the foreground?");
    }


    delay(1000);
  }
	
}
