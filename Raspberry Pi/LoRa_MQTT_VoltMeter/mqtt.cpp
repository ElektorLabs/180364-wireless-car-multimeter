#include "mqtt.h"

#ifdef DEBUG
#include <iostream>
#endif

/**************************************************************************************************
 *    Function      : mqtt_client
 *    Description   : constructor
 *    Input         : const char *id, const char *host, int port
 *    Output        : none
 *....Remarks       : Takes the client id, host and port to establish a connection
 **************************************************************************************************/
mqtt_client::mqtt_client(const char *id, const char *host, int port) : mosquittopp(id)
{
    int keepalive = DEFAULT_KEEP_ALIVE;
    connect(host, port, keepalive);
}



/**************************************************************************************************
 *    Function      : ~mqtt_client
 *    Description   : default destructor
 *    Input         : none
 *    Output        : none
 *....Remarks       : none
 **************************************************************************************************/
mqtt_client::~mqtt_client()
{
}

/**************************************************************************************************
 *    Function      : on_connect
 *    Description   : this will be called if we connect to a broker
 *    Input         : int
 *    Output        : none
 *....Remarks       : none
 **************************************************************************************************/
void mqtt_client::on_connect(int rc)
{
    if (!rc)
    {
        #ifdef DEBUG
            std::cout << "Connected - code " << rc << std::endl;
        #endif
    }
}

/**************************************************************************************************
 *    Function      : do_publish
 *    Description   : this will publish a message to a given channel as non perisent
 *    Input         : char* channel, char* message 
 *    Output        : none
 *....Remarks       : none
 **************************************************************************************************/
void mqtt_client::do_publish(char* channel, char* message ){
    publish(NULL, channel, strlen(message), (char*)message,1,false);
}
