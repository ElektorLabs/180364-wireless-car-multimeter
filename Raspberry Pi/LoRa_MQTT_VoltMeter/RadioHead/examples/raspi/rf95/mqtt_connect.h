
#ifndef MQTTACCESS_H
#define MQTTACCESS_H

#include </usr/include/mosquitto.h>
#include </usr/include/mosquittopp.h>


class mqtt_connect : public mosquittopp::mosquittopp
{
	public:
		mqtt_connect(const char *id, const char *host, int port);
		~mqtt_connect();

		void on_connect(int rc);
		void on_subscribe(uint16_t mid, int qos_count, const uint8_t *granted_qos);
		void publish( char* channel, char* message);
};

#endif
