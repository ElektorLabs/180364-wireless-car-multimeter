/*
    LoRa Receiver Part for Wireless car multimeter
    this requieres the bcm2835 library from Mike McCauley to be installed
    also we need the mosquitto and mosquttopp library on the system 

*/
#include "mqtt.h"

#include <bcm2835.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string>


// This defines the LoRa baord we use, in this case just wires
#define FLYINGWIRE

// Configuration for the LoRa Part
#define RF_FREQUENCY  869.50
#define RF_NODE_ID    1

/* This are the defines for the MQTT part */
#define CLIENT_ID "Client_ID"
#define BROKER_ADDRESS "127.0.0.1"
#define MQTT_PORT 1883;
#define MQTT_TOPIC_RAW_DATA "lora_raw"
#define MQTT_TOPIC_BAT "car\\battery\\voltage"
#define MQTT_TOPIC_INTERIOR_TEMP "car\\interior\\temperature"
#define MQTT_TOPIC_INTERIOR_HUM "car\\interior\\humidity"


// Now we include RasPi_Boards.h so this will expose defined 
// constants with CS/IRQ/RESET/on board LED pins definition
#include "../RasPiBoards.h"


#include <RH_RF69.h>
#include <RH_RF95.h>


// Create an instance of a driver
// We use the RFM95 in polled mode so we don't need to supply a irq pin
RH_RF95 rf95(RF_CS_PIN);

//Flag for Ctrl-C
volatile sig_atomic_t force_exit = false;


/**************************************************************************************************
 *    Function      : sig_handler
 *    Description   : This will be called if we receive a break and stop the programm
 *    Input         : int 
 *    Output        : none
 *....Remarks       : this sets the force_exit flag to let the superloop terminate
 **************************************************************************************************/
void sig_handler(int sig)
{
  printf("\n%s Break received, exiting!\n", __BASEFILE__);
  force_exit=true;
}


/**************************************************************************************************
 *    Function      : main
 *    Description   : this is the main function to keep everything going
 *    Input         : int , char *argv[]
 *    Output        : none
 *....Remarks       : force_exit == true will terminate the superloop
 **************************************************************************************************/
int main(int argc, char *argv[])
{
    /* We first need to have our mqtt_client pointer as we build it with a new statement later */
    class mqtt_client *iot_client;
    /* rc is used for the returncode of the mqtt calls */
    int rc;

    /* We copy the defines here to variable, if we later may attach a config file we only need to 
    modify it here 
    */
    char client_id[] = CLIENT_ID;
    char host[] = BROKER_ADDRESS;
    int port = MQTT_PORT;
    
    /* This are the local message buffer, set to 4k each */
    char msgbuffer[4096]= {0,};
    char mqtt_msg[4096]= {0,};
    char msg_temp[2048]= {0,};
  
  /* we register the signalhandler for the break */
  signal(SIGINT, sig_handler);
  printf( "%s\n", __BASEFILE__);

  /* Next is to initalize the bcm2835 library */
  if (!bcm2835_init()) {
    fprintf( stderr, "%s bcm2835_init() Failed\n\n", __BASEFILE__ );
    return 1;
  }
  
  printf( "RF95 CS=GPIO%d", RF_CS_PIN);

#ifdef RF_IRQ_PIN
  printf( ", IRQ=GPIO%d", RF_IRQ_PIN );
  // IRQ Pin input/pull down
  pinMode(RF_IRQ_PIN, INPUT);
  bcm2835_gpio_set_pud(RF_IRQ_PIN, BCM2835_GPIO_PUD_DOWN);
  // Now we can enable Rising edge detection
  bcm2835_gpio_ren(RF_IRQ_PIN);
#endif
  
#ifdef RF_RST_PIN
  printf( ", RST=GPIO%d", RF_RST_PIN );
  // Pulse a reset on module
  pinMode(RF_RST_PIN, OUTPUT);
  digitalWrite(RF_RST_PIN, LOW );
  bcm2835_delay(150);
  digitalWrite(RF_RST_PIN, HIGH );
  bcm2835_delay(100);
#endif

  if (!rf95.init()) {
    fprintf( stderr, "\nRF95 module init failed, Please verify wiring/module\n" );
  } else {
    // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

    // The default transmitter power is 13dBm, using PA_BOOST.
    // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
    // you can set transmitter powers from 5 to 23 dBm:
    //  driver.setTxPower(23, false);
    // If you are using Modtronix inAir4 or inAir9,or any other module which uses the
    // transmitter RFO pins and not the PA_BOOST pins
    // then you can configure the power transmitter power for -1 to 14 dBm and with useRFO true. 
    // Failure to do that will result in extremely low transmit powers.
    // rf95.setTxPower(14, true);


    // RF95 Modules don't have RFO pin connected, so just use PA_BOOST
    // check your country max power useable, in EU it's +14dB
    rf95.setTxPower(14, false);

    // You can optionally require this module to wait until Channel Activity
    // Detection shows no activity on the channel before transmitting by setting
    // the CAD timeout to non-zero:
    //rf95.setCADTimeout(10000);

    // Adjust Frequency
    rf95.setFrequency(RF_FREQUENCY);
    
    // If we need to send something
    rf95.setThisAddress(RF_NODE_ID);
    rf95.setHeaderFrom(RF_NODE_ID);
    
    // Be sure to grab all node packet 
    // we're sniffing to display, it's a demo
    rf95.setPromiscuous(true);

    // We're ready to listen for incoming message
    rf95.setModeRx();

    printf( " OK NodeID=%d @ %3.2fMHz\n", RF_NODE_ID, RF_FREQUENCY );
    printf( "Listening packet...\n" );

    /* Next is the MQTT connection we need to establish */
    mosqpp::lib_init();

    /* this is hidden and currently deactivated */
    /*
    if (argc > 1){
        strcpy (host, argv[1]);
    }
    */
    //We build a new mqtt_cleint object 
    iot_client = new mqtt_client(client_id, host, port);
   
    

    //Begin the main body of code
    while (!force_exit) {
      
#ifdef RF_IRQ_PIN
      // We have a IRQ pin ,pool it instead reading
      // Modules IRQ registers from SPI in each loop
      
      // Rising edge fired ?
      if (bcm2835_gpio_eds(RF_IRQ_PIN)) {
        // Now clear the eds flag by setting it to 1
        bcm2835_gpio_set_eds(RF_IRQ_PIN);
        //printf("Packet Received, Rising event detect for pin GPIO%d\n", RF_IRQ_PIN);
#endif
        /* We run the client loop */
        rc = iot_client->loop();
        /* Next is to check if the Modem has left the LoRa mode */
        if(false == rf95.IsLoRaMode()){
            /* Okay we need a complete new reinit   */
            printf("Modem reinit, first a reset");
            /* First we do a reset */
            #ifdef RF_RST_PIN
              printf( ", RST=GPIO%d", RF_RST_PIN );
              // Pulse a reset on module
              pinMode(RF_RST_PIN, OUTPUT);
              digitalWrite(RF_RST_PIN, LOW );
              bcm2835_delay(150);
              digitalWrite(RF_RST_PIN, HIGH );
              bcm2835_delay(100);
            #endif
            
            if (!rf95.init()) {
                fprintf( stderr, "\nRF95 module init failed, Please verify wiring/module\n" );
              } else {
                // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

                // The default transmitter power is 13dBm, using PA_BOOST.
                // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
                // you can set transmitter powers from 5 to 23 dBm:
                //  driver.setTxPower(23, false);
                // If you are using Modtronix inAir4 or inAir9,or any other module which uses the
                // transmitter RFO pins and not the PA_BOOST pins
                // then you can configure the power transmitter power for -1 to 14 dBm and with useRFO true. 
                // Failure to do that will result in extremely low transmit powers.
                // rf95.setTxPower(14, true);


                // RF95 Modules don't have RFO pin connected, so just use PA_BOOST
                // check your country max power useable, in EU it's +14dB
                rf95.setTxPower(14, false);

                // You can optionally require this module to wait until Channel Activity
                // Detection shows no activity on the channel before transmitting by setting
                // the CAD timeout to non-zero:
                //rf95.setCADTimeout(10000);

                // Adjust Frequency
                rf95.setFrequency(RF_FREQUENCY);
                
                // If we need to send something
                rf95.setThisAddress(RF_NODE_ID);
                rf95.setHeaderFrom(RF_NODE_ID);
                
                // Be sure to grab all node packet 
                // we're sniffing to display, it's a demo
                rf95.setPromiscuous(true);

                // We're ready to listen for incoming message
                rf95.setModeRx();

                printf( " OK NodeID=%d @ %3.2fMHz\n", RF_NODE_ID, RF_FREQUENCY );
                printf( "Listening packet...\n" );
              }
            
            
            /*                                      */
            
        }
        /* At this point we check if we have new packes */
        if (rf95.available()) { 

          // Should be a message for us now
          uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
          uint8_t len  = sizeof(buf);
          uint8_t from = rf95.headerFrom();
          uint8_t to   = rf95.headerTo();
          uint8_t id   = rf95.headerId();
          uint8_t flags= rf95.headerFlags();;
          int8_t rssi  = rf95.lastRssi();
          /* We grab the data from the modem */
          if (rf95.recv(buf, &len)) {
            /* We present the date to the console and to the user */
            printf("Packet[%02d] #%d => #%d %ddB: ", len, from, to, rssi);
            printbuffer(buf, len);
       
            /* Put message to the mqtt broker, if connection to broker is ready */
            snprintf(msgbuffer, sizeof(msgbuffer) , "{\"messagelen\": %u, \"from\": %u, \"to\": %u, \"rssi\": %i, \"payload\": [",len, from, to, rssi);
            /* This is ugly but may work as we need a buffer for snprintf that is different form the source data */
            for( uint32_t i=0;i<len;i++){
            
                if(i!=(len-1) ){
                    snprintf(msg_temp, sizeof(msg_temp),"%u, ",buf[i]);
                } else {
                    snprintf(msg_temp, sizeof(msg_temp),"%u ]} ",buf[i]);
                }
                snprintf(mqtt_msg, sizeof(mqtt_msg),"%s %s",msgbuffer,msg_temp);
                snprintf(msgbuffer, sizeof(msgbuffer),"%s",mqtt_msg);
            
            }
            printf("\n\r");
            printf("JSON: %s",mqtt_msg);
            printf("\n\r");
            
            /* We check if the last loop has shown no error and we can publish data */
            if(0==rc){
                /* What we got we forward as raw data to the mqtt */
                iot_client->do_publish(MQTT_TOPIC_RAW_DATA,mqtt_msg);
                rc = iot_client->loop();
            } 
            /* Next is to try a decoding for the message */
            if(len == 12 ){
            /* Could be one of ours */
                if( ( from == 0x20) && (to==0x10) ){
                   /* We found a messurment package */
                   
                   /* We grab the voltage from the package */
                   uint16_t voltage_millivolt = ( buf[8] + (buf[9]*256) );
                   
                   /* and also the uuid */
                   uint64_t uuid = (* ( (uint64_t*)(&buf[0]) ) );
                   /* Clear the buffer */
                   bzero(mqtt_msg, sizeof( mqtt_msg ));
                   /* we now assamble the message */
                   snprintf(mqtt_msg, sizeof(mqtt_msg),"{\"uuid\": %llu,\"voltage\": %u,\"uint\": \"mV\"}", uuid,voltage_millivolt);    
                   printf("Send %s to Topic %s \n\r",mqtt_msg, MQTT_TOPIC_BAT  );
                   
                   /* We send the voltage to the configure topic */
                   if(0==rc){
                    iot_client->do_publish(MQTT_TOPIC_BAT,mqtt_msg);
                    rc = iot_client->loop();
                   } 
                   
                   // We grab the temperature */
                   int8_t temperature = (int8_t)(buf[10]);
                   if(temperature>-127){ /* if we have a valid value */
                       bzero(mqtt_msg, sizeof( mqtt_msg ));
                       snprintf(mqtt_msg, sizeof(mqtt_msg),"{\"uuid\": %llu,\"temperature\": %d,\"uint\": \"celsius\"}", uuid,temperature);    
                       printf("Send %s to Topic %s \n\r", mqtt_msg , MQTT_TOPIC_INTERIOR_TEMP);
                       if(0==rc){
                        iot_client->do_publish(MQTT_TOPIC_INTERIOR_TEMP,mqtt_msg);
                        rc = iot_client->loop();
                       } 
                   }
                   
                   uint8_t humidity = buf[11];
                   if(humidity<=100){
                       bzero(mqtt_msg, sizeof( mqtt_msg ));
                       snprintf(mqtt_msg, sizeof(mqtt_msg),"{\"uuid\": %llu, \"humidity\": %u,\"uint\": \"percent\"}",uuid, humidity);    
                       printf("Send %s to Topic %s \n\r",mqtt_msg, MQTT_TOPIC_INTERIOR_HUM );
                       if(0==rc){
                        iot_client->do_publish(MQTT_TOPIC_INTERIOR_HUM,mqtt_msg);
                        rc = iot_client->loop();
                       } 
                   }
                }
            }
            
            
            
           
          } else {
            Serial.print("receive failed");
          }
          printf("\n");
        }
            
        if (rc)
        {
            printf("Connect error!\n\r");
            iot_client->reconnect();
        }
        
        
#ifdef RF_IRQ_PIN
      }
#endif
 
      // Let OS doing other tasks
      // For timed critical appliation you can reduce or delete
      // this delay, but this will charge CPU usage, take care and monitor
      bcm2835_delay(5);
    }
  }

  printf( "\n%s Ending\n", __BASEFILE__ );
  delete iot_client; //free up memory
  mosqpp::lib_cleanup();
  bcm2835_close();
  return 0;

}

