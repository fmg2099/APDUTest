/*
	Ejemplo de manejador de tags NFC

	Comentar o descomentar las secciones según sea necesario
*/

#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>


PN532_I2C pn532_i2c(Wire);
NfcAdapter nfc = NfcAdapter(pn532_i2c);


void setup(void) {
    Serial.begin(115200);
	delay(1000);
    Serial.println("NDEF Reader");
    nfc.begin();
}

void loop(void) {
    Serial.println("\nScan a NFC tag\n");
    if (nfc.tagPresent())
    {
        NfcTag tag = nfc.read();
        tag.print();


		// Escribir tag
		// NdefMessage message = NdefMessage();
		// message.addTextRecord("xX_2099_Xx");
		// bool success = nfc.write(message);
		// if (success) {
		// 	Serial.println("Success. Try reading this tag with your phone.");        
		// } else {
		// 	Serial.println("Write failed.");
		// }

		//borrar
		/*
		bool success=nfc.clean();
		if (success) {
					Serial.println("borrada. .");        
					} else {
					Serial.println("borrado failed.");
					}
		*/

		//formatear tarjeta física como NDEF en vez de mifare
		// bool success = nfc.format();
		// if (success) {
		// 	Serial.println("\nSuccess, tag formatted as NDEF.");
		// } else {
		// 	Serial.println("\nFormat failed.");
		// }
    delay(5000);
	}
}