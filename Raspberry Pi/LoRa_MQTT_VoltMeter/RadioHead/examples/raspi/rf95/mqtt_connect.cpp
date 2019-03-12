#include <cstdio>
#include <cstring>

#include "mqtt_connect.h"
#include <mosquittopp.h>

/* A few lines of configuration are needed to be read in 	*/
/* or if they not exist we build the default 			*/

mqtt_connect::mqtt_connect(const char *id, const char *host, int port) : mosquittopp(id)
{
	int keepalive = 60;
	bool clean_session = true;

	/* Connect immediately. This could also be done by calling
	 * mqtt_tempconv->connect(). */
	connect(host, port, keepalive, clean_session);
};

void mqtt_connect::on_connect(int rc)
{
	printf("Connected with code %d.\n", rc);
	if(rc == 0){

	}
}

void mqtt_connect::on_subscribe(uint16_t mid, int qos_count, const uint8_t *granted_qos)
{
	printf("Subscription succeeded.\n");
}

void mqtt_connect::publish( char* channel, char* message){
	publish(NULL, channel, strlen(message), (uint8_t *)message);
}

