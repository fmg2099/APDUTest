/**************************************************************************/
/*! 	
	Ejemplo de lectura de tarjetas ISO14443A con un AID específico
*/
/**************************************************************************/
#if 0

#include <SPI.h>
#include <PN532_SPI.h>
#include <PN532.h>
//10 es el pin usado para SS (chip select)
#define PN532_SS 10  // Arduino pin label D10 (will be mapped by core)
PN532_SPI pn532spi(SPI, PN532_SS);
PN532 nfc(pn532spi);

#elif 1
  #include <PN532_HSU.h>
  #include <PN532.h>

//En el Arduino Nano ESP32 los pines Rx y Tx corresponden al serial 0
  PN532_HSU pn532hsu(Serial0);
  PN532 nfc(pn532hsu);
#else 
  #include <Wire.h>
  #include <PN532_I2C.h>
  #include <PN532.h>
  PN532_I2C pn532i2c(Wire);
  PN532 nfc(pn532i2c);	
#endif

//tabla de IDs de usuarios permitidos
#define USERID_LEN 4
uint8_t Users[]={
        0xDE, 0xAD, 0xCA, 0xFE,  
        0xc0, 0xc0, 0xb0, 0xb0,
		0xBE, 0xBE, 0x20, 0x99
};

//funcion de ayuda para encender las luces RGB
void rgb(bool R, bool G, bool B)
{
	//invertidos para arduino nano
	digitalWrite(LED_RED, !R);
	digitalWrite(LED_GREEN, !G);
	digitalWrite(LED_BLUE, !B);
}

void loadingscreen()
{
	for(int i=0; i<10; i++)
	{
		rgb(1,1,0);
		delay(50);
		rgb(0,0,0);
		delay(50);
	}
}

//Comparar bytes con la tabla de usuarios
bool checkUser( uint8_t userId[USERID_LEN] )
{
	Serial.print("checkUser: ");
	//usamos la funcion de ayuda para imrpimir los bytes del userId
	nfc.PrintHex(userId, USERID_LEN);
	//para conteos se prefieren los numeros cardinales, representados con size_t
	const size_t userCount = sizeof(Users)/USERID_LEN;
	//para cada usuario...
	for(size_t u=0; u<userCount; u++)
	{
		//accedemos a los bytes de cada userID haciendo un offset
		//apuntador const para evitar romperlo
		const uint8_t* user = &Users[u * USERID_LEN];
		//comparamos los bytes, memcmp da 0 cuando todos los bytes son iguales
		if (memcmp( userId, user, USERID_LEN )==0  )
		{
			return true;
		}
	}
	return false;
}

void setup(void) {

	pinMode(LED_BUILTIN, OUTPUT);
	pinMode(LED_RED, OUTPUT);
	pinMode(LED_GREEN, OUTPUT);
	pinMode(LED_BLUE, OUTPUT);
	loadingscreen();
	//blanca para indicar que esta inicializando serial
	rgb(1,1,1);
	Serial.begin(115200);
	while(!Serial){;}
	rgb(0,0,0);
	Serial.print("Conexion serial lista, inicializando lector NFC...\n");

	#if defined(ARDUINO_ARCH_ESP32)
	Serial.print("ARDUINO_ARCH_ESP32\n");
		#endif	

	//nfc.begin llama a la interfaz begin y wakeup
	//nfc.begin();
	//inicializar para arduino nano esp32
	Serial0.begin(115200);
	//ahora el wakeup
	Serial0.write(0x55);
	Serial0.write(0x55);
	Serial0.write(0);
	Serial0.write(0);
	Serial0.write(0);
	if(Serial0.available())
	{
		Serial.println("Dump serial1 buffer: ");
	}
	while(Serial0.available())
	{
		uint8_t ret = Serial0.read();
        Serial.print(' '); Serial.print((ret>>4)&0x0F, HEX); Serial.print(ret&0x0F, HEX);
	}
	
	uint32_t versiondata = nfc.getFirmwareVersion();
	if (! versiondata) {
		Serial.println("PN532 no encontrada");
		rgb(true, false, false);while (1);
	}
	// Got ok data, print it out!
	Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
	Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
	Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
	
	// configure board to read RFID tags
	nfc.SAMConfig();
	
	Serial.println("Esperando ISO14443A Card ...");

	
}


void loop(void) {

	uint8_t success;
	uint8_t response[255];
	uint8_t responseLength;
	success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, response, &responseLength);
	if (success) {
		Serial.println("Found a device!");
		Serial.println("nfc.readPassiveTargetID: ");
		Serial.print("responseLength: "); Serial.println(responseLength);
		Serial.print("response: ");
		nfc.PrintHex(response, (uint32_t)responseLength);
		Serial.print("InListedTag:"); Serial.println(nfc.getInListedTag());
		delay(100);
		// Establecemos el AID para que Android sepa que app debe responder a esta peticion
		// AID: F0 12 34 56 78 el mismo que se establece en apduservice.xml
		// Structura: CLA INS P1 P2 Lc [DATA] Length
		uint8_t selectApp[] = { 
		0x00, 0xA4, 0x04, 0x00, 0x05, 
		0xF0, 0x12, 0x34, 0x56, 0x78, 0x00
		};
		uint8_t relayResponse[64];
		uint8_t relayResponseLength = 64;
		success = nfc.inDataExchange(selectApp, sizeof(selectApp), relayResponse, &relayResponseLength);
		if (success) {
			Serial.print("Respuesta del Android: ");
			nfc.PrintHex(relayResponse, relayResponseLength);

			// si la respuesta termina en 0x90 0x00, significa que hubo exito en la peticion
			if (relayResponse[relayResponseLength-2] == 0x90) 
			{
				Serial.println("EXITO en la transaccion de datos");
				//ahora toca leer el payload y comprobar si es un usuario autorizado
				uint8_t userId[4];
				//forma ultra bajo nivel para copiar n bytes
				memcpy(userId , relayResponse, 4);
				if ( relayResponseLength>=6 &&  checkUser (userId ) )
				{
					Serial.println("AUTORIZADO");
					rgb(0,1,0);
					delay(500); 
					rgb(0,0,0);
				}
				else
				{
					Serial.println("NO AUTORIZO");
					rgb(1,0,0);
					delay(500);
					rgb(0,0,0);
				}
			}
		} else {
			Serial.println("Failed to select AID. Is the app in the foreground?");
			rgb(1,0,0);
			delay(500);
			rgb(0,0,0);
		}
		delay(1000);
	}

}//loop
