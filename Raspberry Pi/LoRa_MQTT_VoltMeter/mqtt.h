#ifndef SIMPLECLIENT_MQTT_H
#define SIMPLECLIENT_MQTT_H

#include <mosquittopp.h>
#include <cstring>
#include <cstdio>


#define DEFAULT_KEEP_ALIVE 60

/* mini class to wrapp arround the mosquitto lib */
class mqtt_client : public mosqpp::mosquittopp
{
public:
    /* The constrictor for a new mqtt_client object */
    mqtt_client (const char *id, const char *host, int port);
    ~mqtt_client();
    /* Events that can be handeld by this library*/
    void on_connect(int rc);
    
    /* Publishing */
    void do_publish( char* channel, char* message);
};

#endif //SIMPLECLIENT_MQTT_H
