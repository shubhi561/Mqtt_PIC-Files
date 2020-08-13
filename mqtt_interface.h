/*
*------------------------------------------------------------------------------
*                           _             _
*   ___  __ _ _ __ ___  ___| | ___   __ _(_)_  __
*  / __|/ _` | '_ ` _ \/ __| |/ _ \ / _` | \ \/ /
*  \__ \ (_| | | | | | \__ \ | (_) | (_| | |>  <
*  |___/\__,_|_| |_| |_|___/_|\___/ \__, |_/_/\_\
*                                   |___/
*
* Copyright (c) 2013, All Right Reserved, www.samslogix.com
*
* The copyright notice above does not evidence any
* actual or intended publication of such source code.
*
* mqtt_interface.h
*
* Module provides function Library for GSM MQTT Client
*
*------------------------------------------------------------------------------
*/

/*
*------------------------------------------------------------------------------
* File				: mqtt_interface.c
* Created by		: Nithin K. Kurian, Dhanish Vijayan, Elementz Engineers Guild Pvt. Ltd, July 2, 2016. and http://www.electronicwings.com
* Last changed by	: Sam
* Last changed		: 20/05/2020
*------------------------------------------------------------------------------
*
* Revision 1.0 20/05/2020 Sam
* Initial revision -ported from ardunio lib by Nithin K. Kurian, Dhanish Vijayan, Elementz Engineers Guild Pvt. Ltd, July 2, 2016.
* and  http://www.electronicwings.com
*
*------------------------------------------------------------------------------
*/

#ifndef MQTT_INTERFACE_H
#define MQTT_INTERFACE_H

/*
*------------------------------------------------------------------------------
* Include Files
*------------------------------------------------------------------------------
*/

#include "../includes.h"

/*
*------------------------------------------------------------------------------
* Hardware Port Allocation
*------------------------------------------------------------------------------
*/

/*
*------------------------------------------------------------------------------
* Public Defines
*------------------------------------------------------------------------------
*/
enum MQTT_CLIENT_SERVICE_STATES
{
	MQTT_STATE_CONNECT = 0,
    MQTT_STATE_CONNECT_AFTER_DLY,
    MQTT_STATE_PUBLISH,
    MQTT_STATE_CHK_TX_PING_INTERVAL,
    MQTT_STATE_PING,
    MQTT_STATE_ERROR,
    MQTT_STATE_LAST_ITEM
};

/*
*------------------------------------------------------------------------------
* Public Macros
*------------------------------------------------------------------------------
*/

#define TOPIC_BUFFER_LENGTH     20    //Maximum mLength allowed mTopic
#define MESSAGE_BUFFER_LENGTH   250  //Maximum mLength allowed data

#define MQTT_PROTOCOL_LEVEL		4

#define MQTT_CTRL_CONNECT		0x1     //Client request to MqttConnect to Server                Client          Server
#define MQTT_CTRL_CONNECTACK	0x2     //Connect Acknowledgment                             Server/Client   Server/Client
#define MQTT_CTRL_PUBLISH		0x3     //Publish message                                    Server/Client   Server/Client
#define MQTT_CTRL_PUBACK		0x4     //Publish Acknowledgment                             Server/Client   Server/Client
#define MQTT_CTRL_PUBREC		0x5     //Publish Received (assured delivery part 1)         Server/Client   Server/Client
#define MQTT_CTRL_PUBREL		0x6     //Publish Release (assured delivery part 2)          Server/Client   Server/Client
#define MQTT_CTRL_PUBCOMP		0x7     //Publish Complete (assured delivery part 3)         Server/Client   Server/Client
#define MQTT_CTRL_SUBSCRIBE		0x8     //Client Subscribe request                           Client          Server
#define MQTT_CTRL_SUBACK		0x9     //Subscribe Acknowledgment                           Server          Client
#define MQTT_CTRL_UNSUBSCRIBE	0xA     //Client Unsubscribe request                         Client          Server
#define MQTT_CTRL_UNSUBACK		0xB     //Unsubscribe Acknowledgment                         Server          Client
#define MQTT_CTRL_PINGREQ		0xC     //PING Request                                       Client          Server
#define MQTT_CTRL_PINGRESP		0xD     //PING Response                                      Server          Client
#define MQTT_CTRL_DISCONNECT	0xE     //Client is Disconnecting                            Client          Server

/* QoS value bit 2 bit 1 Description
*   0       0       0   At most once    Fire and Forget         <=1
*   1       0       1   At least once   Acknowledged delivery   >=1
*   3       1       1   Reserved
*/
#define MQTT_QOS_1				0x1
#define MQTT_QOS_0				0x0

// QoS value bit 2 bit 1 Description
//   0       0       0   At most once    Fire and Forget         <=1
//   1       0       1   At least once   Acknowledged delivery   >=1
//   2       1       0   Exactly once    Assured delivery        =1
//   3       1       1   Reserved
#define MQTT_DUP_MASK      		8   // Duplicate delivery   Only for QoS>0
#define MQTT_QOS_MASK      		6   // Quality of Service
#define MQTT_QOS_SCALE     		2   // (()&QoS)/QoS_Scale
#define MQTT_RETAIN_MASK   		1   // RETAIN flag

/* Adjust as necessary, in seconds */
#define MQTT_CONN_KEEPALIVE		60

#define MQTT_CONN_USERNAMEFLAG	0x80
#define MQTT_CONN_PASSWORDFLAG	0x40
#define MQTT_CONN_WILLRETAIN	0x20
#define MQTT_CONN_WILLQOS_1		0x08
#define MQTT_CONN_WILLQOS_2		0x18
#define MQTT_CONN_WILLFLAG		0x04
#define MQTT_CONN_CLEANSESSION	0x02

#define DEFAULT_BUFFER_SIZE		200
#define DEFAULT_TIMEOUT			10000
#define DEFAULT_CRLF_COUNT		2


#define GPRS_APN                "airtelgprs.com"            /*"live.vodafone.com" */
#define GPRS_USERNAME           ""                          /* Apn user here */
#define GPRS_PASSWORD           ""                          /* Apn psw key here */
#define PROTOCOL                "MQTT"                      /* communication protocol */
#define DYO_SERVER              "mqtt.dev.dyocense.com"		/* Server name*/
#define DYO_SERVER_PORT         "1883"                      /* Server port */
#define DYO_USERNAME            "mlsd002"                   /* Enter username here */
#define DYO_PSW_KEY             "mlsd002"                   /* Enter psw key here */
#define DYO_TOPIC               "Enter Feed Key"            /* Enter topic key */
#define DYO_DATA_TX_INTERVAL    "10"                        /* In seconds */

/*
*------------------------------------------------------------------------------
* Public Data Types
*------------------------------------------------------------------------------
*/

/*
*------------------------------------------------------------------------------
* Public Variables (extern)
*------------------------------------------------------------------------------
*/

/*
*------------------------------------------------------------------------------
* Public Constants (extern)
*------------------------------------------------------------------------------
*/

/*
*------------------------------------------------------------------------------
* Public Function Prototypes (extern)
*------------------------------------------------------------------------------
*/

void InitializeMqttClient(void);
extern INT8 MqttConnect(CHAR8 *pClientIdentifier, BOOL userNameFlag, BOOL passwordFlag, CHAR8 *pUserName, CHAR8 *pPassword, BOOL cleanSession, UINT8 willFlag, UINT8 willQoS, UINT8 willRetain, UINT16 keepAlive, CHAR8 *pWillTopic, CHAR8 *pWillMessage);
extern INT8 MqttPublish(CHAR8 dup, CHAR8 Qos, CHAR8 retain, UINT16 messageID, char *pTopic, CHAR8 *pMessage);
extern BOOL MqttSubscribe(CHAR8 DUP, UINT16 messageID, CHAR8 *pSubTopic, CHAR8 subQoS);
extern BOOL MqttPingRequest(void);
extern BOOL MqttDisconnect(void);
extern void MqttClientServiceState(void *args);
extern void TestMqttPacketFormation(void);
#endif
/*
*  End of mqtt_interface.h
*/
