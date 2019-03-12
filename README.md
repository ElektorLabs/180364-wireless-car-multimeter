# 180364
Software for the Wirless Car Multimeter

## Getting Started

Read https://www.elektormagazine.com/labs/wireless-car-voltmeter to see what you need as hardware base for this project and how to wire it.

You need basically:
- Raspberry Pi ( Model 3b+ or Zero)
- LoRa Nexus Module ( https://www.elektor.com/lora-nexus-board-arduino-mini-shape )
- RMF95 ( https://www.elektor.com/rfm95-ultra-lora-transceiver-module-868-915-mhz )
- a buch of jumperwire , few resistors, DC/DC Converter, ...

## Requierments 
- For the Raspberry pi you need to install few librarys, the bcm2835 and mosquitto.
- For the Arduino you need a patched version of the DS2401 library, PaulStoffregen's OneWire Library, the LowPower library, a patched radiohead version and the DHT library from adafruit. The ones are inclided with the Arduinofolder.

For the rest follow the instructions given at the Labs-page.






