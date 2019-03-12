#include "DHT.h"
#include <SPI.h>
#define RH_RF95_IRQLESS
#include <RH_RF95.h>

#include <OneWire.h>
#include "DS2401.h"
#include <Wire.h>

#include <avr/wdt.h>
#include "LowPower.h"
 
#define RFM95_CS 10
/* We use a polling mode to get rid of the irq pin */
#define RFM95_INT 3

#define RF95_FREQ 869.5
/* Waittime in seconds */
#define WAIT_TIME ( 120 ) 


#define my_id  0x20
#define target_id 0x10

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);
// Instance for the AD1015 
//Adafruit_ADS1015 ads1015;

OneWire oneWire(2);

DS2401 ds24(&oneWire);
DHT dht;



typedef struct {
  uint8_t uuid[8];
  uint16_t voltage;
  int8_t temp;
  uint8_t humidity;
} radiopacket_t;

radiopacket_t packet;

bool modem_init( void );
void modem_reinit(void );
void adc_init( void );
uint16_t ReadVBat( void );
void setup() 
{
  wdt_disable();
  Serial.begin(115200);
  delay(1000);
  dht.setup(A0,DHT::AUTO_DETECT);
  delay(1000);
  Serial.print(F("Humidity:"));
  Serial.print(dht.getHumidity());
  Serial.print(F("% / Temperature:"));
  Serial.print(dht.getTemperature());
  Serial.println(F("°C"));
  
  if (ds24.init())
  {  
    Serial.println(F("Found OneWire Device"));
  }
  else
  {
    Serial.println(F("ERROR: No OneWire Device Found"));
  }

  if (ds24.isDS2401())
  {
    Serial.println(F("IS DS2401"));
    ds24.GetSerial(packet.uuid, sizeof(packet.uuid));
    Serial.print("UUID:"); 
    Serial.println(ds24.GetSerial());
  }
  else
  {
    Serial.println(F("ERROR: Is Not DS2401"));
  }
  
  
  if(false == modem_init() ){
    /* something is broken here */
    Serial.println(F("Modem not responding"));
    wdt_enable(WDTO_1S);
    while(1==1){
     _NOP();  
    }
  }

}

void loop() 
{ 
  uint8_t humidity = dht.getHumidity();
  int8_t temperature = dht.getTemperature();
  if(dht.getStatus()!=DHT::ERROR_NONE){
    humidity = 255;
    temperature = -127;
  }
  /* We include the serial in the packet */
  packet.voltage = ReadVBat();
  packet.temp=temperature;
  packet.humidity = humidity;
  Serial.println("Send data");
 
  rf95.setThisAddress(my_id);
  rf95.setHeaderFrom(my_id);
  rf95.setHeaderTo(target_id);  
  rf95.send((uint8_t *)&packet, sizeof(packet));

  Serial.println("Enter Sleep mode");
  rf95.sleep();
  delay(250);
  for(uint8_t i=0;i< ( WAIT_TIME / 8 ) ;i++){
    LowPower.idle(SLEEP_8S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART0_OFF, TWI_OFF);
    /* we could every 8 seconds do stuff here */
  }
  Serial.println("Wake up");
  modem_reinit();
}

/* after 2 miniuts we aggain do a init */
bool modem_init( void ){
  bool done = true;
  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    done = false;
   
  }
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    done = false;
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
  rf95.setTxPower(14, false);  
  return done;
}

uint16_t ReadVBat( void ){
 /* We have 3.3V Max at Vin with 1024 steps meaning 0,003223V per Bit
  *  Also we have a voltage devider with factor 6,6 in front meaning 
  *  3,3V corresponde to 21,78V, giving us 0,02129V ( 21.29mV) per Bit
  */
  int vbat = analogRead(A1);
  uint16_t mV = (uint16_t)( (float)(21.41)*(float)(vbat) );
  return mV; 
}

void modem_reinit(){
  bool done = true;
  while (!rf95.reinit()) {
    Serial.println("LoRa radio reinit failed");
    done = false;
  }
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    done = false;
  }
  Serial.print("Set Freq to:");
  Serial.println(RF95_FREQ);
  rf95.setTxPower(14, false);  
  return done;
}



