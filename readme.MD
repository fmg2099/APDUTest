# Android NFC + Arduino + PN352

Este proyecto contiene una aplicación Android y un sketch de Arduino para demostrar el uso de Android HCE (Host Card Emulation) como sistema de identificación y acceso en un sistema físico. 

Mediante  el protocolo APDU establece una comunicación de bajo nivel para intercambiar un ID de 4 bytes representando a un usuario. 

## Hardware

* Arduino Nano ESP32
* Elechouse PN532 v3 en modo HSU (MUY importante)
* Dispositivo móvil Android mayor a API 24

## Bibliotecas

Utiliza una versión modificada específicamente para funcionar en Arduino Nano ESP32 de las bibliotecas de [Elechouse PN532](https://github.com/elechouse/PN532).

*pegar el link del repo conlas biblotecas modificadas*

## Referencias

Manual de usuario de la placa PN532

https://www.elechouse.com/elechouse/images/product/PN532_module_V3/PN532_%20Manual_V3.pdf

Tutorial para crear una aplicación APDU

https://medium.com/@androidcrypto/how-to-use-host-based-card-emulation-hce-in-android-a-beginner-tutorial-java-32974dd89529

Protocolo APDU

https://en.wikipedia.org/wiki/Smart_card_application_protocol_data_unit

Lista de Application Identifiers ISO14443A

https://www.eftlab.com/knowledge-base/complete-list-of-application-identifiers-aid

## Licencia

TBD
