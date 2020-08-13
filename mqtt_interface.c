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
* mqtt_interface.c
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

/*
*------------------------------------------------------------------------------
* Include Files
*------------------------------------------------------------------------------
*/

#include "../includes.h"
#include "mqtt_interface.h"

/*
*------------------------------------------------------------------------------
* Private Defines
*------------------------------------------------------------------------------
*/
typedef enum
{
    MQTT_STARTUP,
	MQTT_CHECK_FOR_DATA
}MQTT_CLIENT_STATES;


typedef enum
{
    TCP_SIM_READY,
	TCP_ECHO_OFF,
    TCP_SIM_REG,
	TCP_GPRS_ATTACHED,
	TCP_IP_SHUT,
	TCP_IP_INIT_STATE,
	TCP_MUX0,
	TCP_MODE1,
	TCP_SET_APN,
	TCP_UP_WIRELES,
    TCP_GET_IP,
	TCP_GET_CON_STATE,
    //TCP_DATA_MANUAL_READ,
	TCP_START_TCP_CONN
}TCP_SETUP_STATES;

/*
*------------------------------------------------------------------------------
* Private Macros
*------------------------------------------------------------------------------
*/

#define		MQTT_CONNECT_DELAY			(UINT32)(5000)		/* 5 sec. */

/*
*------------------------------------------------------------------------------
* Private Data Types
*------------------------------------------------------------------------------
*/

/*
*------------------------------------------------------------------------------
* Public Variables
*------------------------------------------------------------------------------
*/
UINT8   MqttServiceStates = MQTT_STATE_CONNECT;
UINT32  MqttDataTxTimout  = 0;
UINT32  MqttPingTimeOut   = 0;
UINT32  MqttConnectDelay  = 0;

/*
*------------------------------------------------------------------------------
* Private Variables (static)
*------------------------------------------------------------------------------
*/

UINT16 lastMessaseID = 0;
#pragma udata BigData
UINT8 packetBuf[MESSAGE_BUFFER_LENGTH];
#pragma udata


char clientID[] ="MLSMARTDOG/mlsd002";
char will_topic[] = "MLSMARTDOG/mlsd002/telemetry/POST/?msgid=1";
char will_payload[] ="{\"userCount\":\"0001\",\"mesrTemp\":\"30.5\"}";
char will_topic_dummy[] = "";
char will_payload_dummy[] ="";
char ServerUser[20];
char ServerPsw[20];


/*
*------------------------------------------------------------------------------
* Public Constants
*------------------------------------------------------------------------------
*/

/*
*------------------------------------------------------------------------------
* Private Constants (static)
*------------------------------------------------------------------------------
*/

/*
*------------------------------------------------------------------------------
* Private Function Prototypes (static)
*------------------------------------------------------------------------------
*/

/*
*------------------------------------------------------------------------------
* Private Functions
*------------------------------------------------------------------------------
*/

/*
*------------------------------------------------------------------------------
* UINT16 generateMessageID(void)
*
* Summary	: Generate 16 bit incremental msg id
*
* Input		: None
*
* Output	: UINT16 - new id generated
*------------------------------------------------------------------------------
*/
UINT16 generateMessageID(void)
{
    if (lastMessaseID < 65535)
    {
        return ++lastMessaseID;
    }
    else
    {
        lastMessaseID = 0;
        return lastMessaseID;
    }
}

/*
*------------------------------------------------------------------------------
* UINT16 stringToUint16(UINT8* pstring)
*
* Summary	: Initialize mqtt stack variables.
*
* Input		: UINT8* pstring - pointer to ascii string
*
* Output	: 
*------------------------------------------------------------------------------
*/
UINT16 stringToUint16(UINT8* pstring)
{
	UINT16 value = 0;
	while ('0' == *pstring || *pstring == ' ' || *pstring == '"')
	{
		pstring++;
	}
	while ('0' <= *pstring && *pstring <= '9')
	{
		value *= 10;
		value += *pstring - '0';
		pstring++;
	}
	return value;
}

/*
*------------------------------------------------------------------------------
* UINT8* addStringToBuf(UINT8 *pInputBuf, CHAR8 *pstring)
*
* Summary	: Insert sting in to buffer
*
* Input		: UINT8 *pInputBuf - pointer to buffer,
*             CHAR8 *pstring - pointer to the string to insert.
*
* Output	: UINT8*  - buffer string end pointer
*------------------------------------------------------------------------------
*/
UINT8* addStringToBuf(UINT8 *pInputBuf, CHAR8 *pstring)
{
  UINT16 length = strlen((CHAR8*)pstring);
  pInputBuf[0] = length >> 8;
  pInputBuf[1] = length & 0xFF;
  pInputBuf+=2;
  strncpy((CHAR8*)pInputBuf, (const CHAR8 *)pstring, length);
  return pInputBuf + length;
}

/*
*------------------------------------------------------------------------------
* Public Functions
*------------------------------------------------------------------------------
*/

/*
*------------------------------------------------------------------------------
* void TestMqttPacketFormation(void)
*
* Summary   : None
*
* Input     : None
*
* Output    : None
*------------------------------------------------------------------------------
*/
void TestMqttPacketFormation(void)
{
    strcpy(ServerUser,(CHAR8*)&stSysConfig.mServerUserName[0]);
    strcpy(ServerPsw, (CHAR8*)&stSysConfig.mServermPassword[0]);
    memset((CHAR8*)packetBuf, 0, MESSAGE_BUFFER_LENGTH);
    //MqttConnect(CHAR8 *pClientIdentifier, BOOL userNameFlag, BOOL passwordFlag, CHAR8 *pUserName, CHAR8 *pPassword, BOOL cleanSession, UINT8 willFlag, UINT8 willQoS, UINT8 willRetain, UINT16 keepAlive, CHAR8 *pWillTopic, CHAR8 *pWillMessage)
    MqttConnect((CHAR8*)clientID, 1, 1, (CHAR8*)ServerUser, (CHAR8*)ServerPsw, 1, 0, 0, 0, 60, will_topic_dummy, will_payload_dummy); 
    memset((CHAR8*)packetBuf, 0, MESSAGE_BUFFER_LENGTH);
    //BOOL MqttPublish(CHAR8 dup, CHAR8 Qos, CHAR8 retain, UINT16 messageID, char *pTopic, CHAR8 *pMessage);
    MqttPublish(0, 0, 0, 10, will_topic, will_payload);
}
    
/*
*------------------------------------------------------------------------------
* BOOL MqttSendPacket(UINT8 *buffer, UINT8 msg_size)
*
* Summary   : Send packet to mqtt server 
*
* Input     : UINT8 *buffer - pointer to the message buffer 
*             UINT8 msg_size - size of the message
*
* Output    : INT8 - function state FUN_STATE_ENTER/FUN_STATE_ERROR/FUN_STATE_EXIT
*------------------------------------------------------------------------------
*/
BOOL MqttSendPacket(UINT8 *buffer, UINT8 msg_size)
{
    static UINT8    pass = 0;
    UINT8           tcpState;
	INT8            resp;

    if(0 == pass)
    {
        resp = GPRS_TCPStatus(&tcpState);
        if(FUN_STATE_EXIT == resp)
        {
           if(CONNECT_OK == tcpState)
           {
#ifdef DEBUG_MQTT_CLIENT
            Print("sendPacket() - TCP Connection status  OK\r\n");
#endif         
                pass = 1;
           }
           else
           {
               pass = 0;
               return FALSE;
           }
        }
        else
        if(FUN_STATE_ERROR == resp)
        {
#ifdef DEBUG_MQTT_CLIENT
            Print("sendPacket() - TCP Connection status Error\r\n");
#endif                       
        }
    }
    else
    if(1== pass)
 	{
        /* Send request to tcp server. */
        memset(tempGsmBuff, 0, sizeof(tempGsmBuff));
        resp = GPRS_SendData((const UINT8*)buffer, msg_size);
        if(FUN_STATE_EXIT == resp)
        {
#ifdef DEBUG_MQTT_CLIENT
            Print("sendPacket() - Send OK\r\n");
#endif
            pass = 0;
            return TRUE;
        }
        else
        if(FUN_STATE_ERROR == resp)
        {
#ifdef DEBUG_MQTT_CLIENT
            Print("sendPacket() -Send  Error\r\n");
#endif
            pass = 0;
            return FALSE;
        }
    }
}

/*
*------------------------------------------------------------------------------
* INT8 MqttConnect(CHAR8 *pClientIdentifier, BOOL userNameFlag, BOOL passwordFlag, CHAR8 *pUserName, 
* CHAR8 *pPassword, BOOL cleanSession, UINT8 willFlag, UINT8 willQoS, UINT8 willRetain, UINT16 keepAlive, CHAR8 *pWillTopic, CHAR8 *pWillMessage)
*
* Summary	: Client requests a connection to a Server
*
* Input		:          
*        pClientIdentifier  :Is a string that uniquely identifies the client to the server.
                            :It must be unique across all clients connecting to a single server.(So it will be better for you to change that).
                            :It's length must be greater than 0 and less than 24
                            :Example "qwerty"
          userNameFlag      :Indicates whether UserName is present
                            :Possible values (0,1)
                            :Default value 0 (Disabled)
          pPasswordFlag     :Valid only when  UserNameFlag is 1, otherwise its value is disregarded.
                            :Indicates whether Password is present
                            :Possible values (0,1)
                            :Default value 0 (Disabled)
          pUserName         :Mandatory when UserNameFlag is 1, otherwise its value is disregarded.
                            :The UserName corresponding to the user who is connecting, which can be used for authentication.
          pPassword         :valid only when  UserNameFlag and PasswordFlag are 1 , otherwise its value is disregarded.
                            :The password corresponding to the user who is connecting, which can be used for authentication.
          cleanSession      :If not set (0), then the server must store the subscriptions of the client after it disconnects.
                            :If set (1), then the server must discard any previously maintained information about the client and treat the connection as "clean".
                            :Possible values (0,1)
                            :Default value 1
          willFlag          :This flag determines whether a WillMessage published on behalf of the client when client is disconnected involuntarily.
                            :If the WillFlag is set, the WillQoS, WillRetain, WillTopic, WilMessage fields are valid.
                            :Possible values (0,1)
                            :Default value 0 (Disables will Message)
          willQoS           :Valid only when  WillFlag is 1, otherwise its value is disregarded.
                            :Determines the QoS level of WillMessage
                            :Possible values (0,1,2)
                            :Default value 0 (QoS 0)
          willRetain        :Valid only when  WillFlag is 1, otherwise its value is disregarded.
                            :Determines whether the server should retain the Will message.
                            :Possible values (0,1)
                            :Default value 0
          keepAlive         :The Keep Alive is a time interval measured in seconds. Expressed as a 16-bit word.
          pWillTopic        :Mandatory when  WillFlag is 1, otherwise its value is disregarded.
                            :The Will Message will published to this topic (WillTopic) in case of involuntary client disconnection.
          pWillMessage      :Mandatory when  WillFlag is 1, otherwise its value is disregarded.
                            :This message (WillMessage) will published to WillTopic in case of involuntary client disconnection.
* Output	: INT8 - function state FUN_STATE_ENTER/FUN_STATE_ERROR/FUN_STATE_EXIT
*------------------------------------------------------------------------------
*/
INT8 MqttConnect(CHAR8 *pClientIdentifier, BOOL userNameFlag, BOOL passwordFlag, CHAR8 *pUserName, CHAR8 *pPassword, BOOL cleanSession, UINT8 willFlag, UINT8 willQoS, UINT8 willRetain, UINT16 keepAlive, CHAR8 *pWillTopic, CHAR8 *pWillMessage)
{
    UINT8* ptrPacketBuff = packetBuf;
	static UINT16 length = 0;
    CHAR8  protcol[] = "MQTT";
    INT8 resp;
    INT8 ret_val = FUN_STATE_ENTER;
    static UINT8 pass = 0;
 
    if(0 == pass)
    {
        length = 0;
        memset((CHAR8*)packetBuf, 0, MESSAGE_BUFFER_LENGTH);
        ptrPacketBuff[0] = (MQTT_CTRL_CONNECT << 4);
        ptrPacketBuff   += 2;
        ptrPacketBuff   = addStringToBuf(ptrPacketBuff, (CHAR8*)protcol);
        ptrPacketBuff[0] = MQTT_PROTOCOL_LEVEL;
        ptrPacketBuff++;

        if(1 == cleanSession)
        {
            ptrPacketBuff[0] = MQTT_CONN_CLEANSESSION;
        }
        if (1 == willFlag) 
        {
            ptrPacketBuff[0] |= MQTT_CONN_WILLFLAG;
            if(1 == willQoS)
            {
                ptrPacketBuff[0] |= MQTT_CONN_WILLQOS_1;
            }
            else 
            if(2 == willQoS)
            {
                ptrPacketBuff[0] |= MQTT_CONN_WILLQOS_2;
            }
            if(1 == willRetain)
            {
                ptrPacketBuff[0] |= MQTT_CONN_WILLRETAIN;
            }
        }
        if (1 == userNameFlag)
        {
            ptrPacketBuff[0] |= MQTT_CONN_USERNAMEFLAG;
        }
        if (1 == passwordFlag)
        {
            ptrPacketBuff[0] |= MQTT_CONN_PASSWORDFLAG;
        }
        ptrPacketBuff++;

        ptrPacketBuff[0] = (keepAlive >> 8);
        ptrPacketBuff++;
        ptrPacketBuff[0] = (keepAlive & 0xFF);
        ptrPacketBuff++;
        if (strlen(pClientIdentifier) != 0) 
        {
            ptrPacketBuff = addStringToBuf(ptrPacketBuff, (CHAR8*)pClientIdentifier);
        } 
        else 
        {
            ptrPacketBuff[0] = 0x0;
            ptrPacketBuff++;
            ptrPacketBuff[0] = 0x0;
            ptrPacketBuff++;
        }
        if (pWillTopic && strlen(pWillTopic) != 0) 
        {
            ptrPacketBuff = addStringToBuf(ptrPacketBuff, pWillTopic);
            ptrPacketBuff = addStringToBuf(ptrPacketBuff, pWillMessage);
        }

        if (1 == userNameFlag) 
        {
            ptrPacketBuff = addStringToBuf(ptrPacketBuff, pUserName);
        }
        if (1 == passwordFlag)
        {
            ptrPacketBuff = addStringToBuf(ptrPacketBuff, pPassword);
        }
        length = ptrPacketBuff - packetBuf;
        packetBuf[1] = length-2; 
        pass = 1;
    }
    else
    if(1 == pass)
    {
        /* Send data over tcp . */
        resp = GPRS_SendData((const UINT8*)packetBuf, length);
        if(FUN_STATE_EXIT == resp)
        {
#ifdef DEBUG_MQTT_CLIENT
            Print("MqttConnect() - Connect OK\r\n");
#endif
            pass = 0;
            ret_val = FUN_STATE_EXIT;
        }
        else
        if(FUN_STATE_ERROR == resp)
        {
#ifdef DEBUG_MQTT_CLIENT
            Print("MqttConnect() - Connect  Error\r\n");
#endif
            pass = 0;
            ret_val = FUN_STATE_ERROR;            
        }
    }
    return ret_val;
}

/*
*------------------------------------------------------------------------------
* INT8 MqttPublish(CHAR8 dub, char Qos, CHAR8 retain, UINT16 messageID, CHAR8 *pTopic, CHAR8 *pMessage)
*
* Summary	: publish a topic and message.  
*
* Input	
    DUP       :This flag is set when the client or server attempts to re-deliver a PUBLISH message
              :This applies to messages where the value of QoS is greater than zero (0)
              :Possible values (0,1)
              :Default value 0
    QoS       :Quality of Service
              :This flag indicates the level of assurance for delivery of a PUBLISH message
              :Possible values (0,1,2)
              :Default value 0
    RETAIN    :if the Retain flag is set (1), the server should hold on to the message after it has been delivered to the current subscribers.
              :When a new subscription is established on a topic, the last retained message on that topic is sent to the subscriber
              :Possible values (0,1)
              :Default value 0
    Message ID:The Message Identifier (Message ID) field
              :Used only in messages where the QoS levels greater than 0
    Topic     :Publishing topic
    Message   :Publishing Message

* Output	: INT8 - function state FUN_STATE_ENTER/FUN_STATE_ERROR/FUN_STATE_EXIT
*------------------------------------------------------------------------------
*/
INT8 MqttPublish(CHAR8 dup, CHAR8 Qos, CHAR8 retain, UINT16 messageID, char *pTopic, CHAR8 *pMessage)
{
	UINT8 *ptrPacketBuff = packetBuf;
	static UINT16 length = 0;
	UINT16 datalen;
    INT8 resp;
    INT8 ret_val = FUN_STATE_ENTER;
    static UINT8 pass = 0;
    
    if(0 == pass)
    {
        length = 0;
        memset((CHAR8*)packetBuf, 0, MESSAGE_BUFFER_LENGTH);        
        datalen = strlen(pMessage);
        length += 2;
        length += strlen(pTopic);
        if(Qos > 0) {
            length += 2;
        }
        length += datalen;
        ptrPacketBuff[0] = (MQTT_CTRL_PUBLISH << 4) | (Qos << 1);
        if(1 == dup)
        {
            ptrPacketBuff[0] = ptrPacketBuff[0] | MQTT_DUP_MASK;  
        }
        if(1 == retain)
        {
            ptrPacketBuff[0] = ptrPacketBuff[0] | MQTT_RETAIN_MASK;  
        }
        ptrPacketBuff++;
        do {
            UINT8 encodedByte = length % 128;
            length /= 128;
            if ( length > 0 ) 
            {
                encodedByte |= 0x80;
            }
            ptrPacketBuff[0] = encodedByte;
            ptrPacketBuff++;
        } while ( length > 0 );
        ptrPacketBuff = addStringToBuf(ptrPacketBuff, pTopic);
        if(Qos > 0) 
        {
            ptrPacketBuff[0] = (messageID >> 8) & 0xFF;
            ptrPacketBuff[1] = messageID & 0xFF;
            ptrPacketBuff+=2;
        }
        memmove(ptrPacketBuff, (const CHAR8*)pMessage, datalen);
        ptrPacketBuff += datalen;
        length = ptrPacketBuff - packetBuf;
        pass = 1;
    }
    else
    if(1 == pass)
    {
        /* Send data over tcp . */
        resp = GPRS_SendData((const UINT8*)packetBuf, length);
        if(FUN_STATE_EXIT == resp)
        {
#ifdef DEBUG_MQTT_CLIENT
            Print("MqttPublish() - Publish OK\r\n");
#endif
            pass = 0;
            ret_val = FUN_STATE_EXIT;
        }
        else
        if(FUN_STATE_ERROR == resp)
        {
#ifdef DEBUG_MQTT_CLIENT
            Print("MqttPublish() - Publish  Error\r\n");
#endif
            pass = 0;
            ret_val = FUN_STATE_ERROR;            
        }
    }
    return ret_val; 
}

/*
*------------------------------------------------------------------------------
* BOOL MqttSubscribe(CHAR8 DUP, UINT16 messageID, CHAR8 *pSubTopic, CHAR8 subQoS)
*
* Summary	: subscribe for topic 
*
* Input	
          DUP       :This flag is set when the client or server attempts to re-deliver a SUBSCRIBE message
                    :This applies to messages where the value of QoS is greater than zero (0)
                    :Possible values (0,1)
                    :Default value 0
          Message ID:The Message Identifier (Message ID) field
                    :Used only in messages where the QoS levels greater than 0 (SUBSCRIBE message is at QoS =1)
          SubTopic  :Topic names to which  subscription is needed
          SubQoS    :QoS level at which the client wants to receive messages
                    :Possible values (0,1,2)
                    :Default value 0
* Output	: BOOL - TRUE/FALSE
*------------------------------------------------------------------------------
*/
BOOL MqttSubscribe(CHAR8 DUP, UINT16 messageID, CHAR8 *pSubTopic, CHAR8 subQoS)
{
	UINT8 *ptrPacketBuff = packetBuf;
	static UINT8 length;
    INT8 resp;
    INT8 ret_val = FUN_STATE_ENTER;
    static UINT8 pass = 0;
    
    if(0 == pass)
    {
        memset((CHAR8*)packetBuf, 0, MESSAGE_BUFFER_LENGTH);        
        ptrPacketBuff[0] = MQTT_CTRL_SUBSCRIBE << 4 | MQTT_QOS_1 << 1;
        ptrPacketBuff += 2;

        ptrPacketBuff[0] = (messageID >> 8) & 0xFF;
        ptrPacketBuff[1] = messageID & 0xFF;
        ptrPacketBuff+=2;

        ptrPacketBuff = addStringToBuf(ptrPacketBuff, pSubTopic);

        ptrPacketBuff[0] = subQoS;
        ptrPacketBuff++;

        length = ptrPacketBuff - packetBuf;
        packetBuf[1] = length - 2;    
        pass = 1;
    }
    else
    if(1 == pass)
    {
        /* Send data over tcp . */
        resp = GPRS_SendData((const UINT8*)packetBuf, length);
        if(FUN_STATE_EXIT == resp)
        {
#ifdef DEBUG_MQTT_CLIENT
            Print("MqttSubscribe() - Subscribe OK\r\n");
#endif
            pass = 0;
            ret_val = FUN_STATE_EXIT;
        }
        else
        if(FUN_STATE_ERROR == resp)
        {
#ifdef DEBUG_MQTT_CLIENT
            Print("MqttSubscribe() - Subscribe  Error\r\n");
#endif
            pass = 0;
            ret_val = FUN_STATE_ERROR;            
        }
    }
    return ret_val;    
}

/*
*------------------------------------------------------------------------------
* BOOL MqttPingRequest(void)
*
* Summary	: Send mqtt ping request and wait for the response. 
*
* Input     : None
* Output	: BOOL - TRUE/FALSE
*------------------------------------------------------------------------------
*/
BOOL MqttPingRequest(void)
{
    BOOL resp;
    INT8 ret_val = FUN_STATE_ENTER;    
    packetBuf[0] = MQTT_CTRL_PINGREQ << 4;
    packetBuf[1] = 0;

    /* Send data over tcp . */
    resp = GPRS_SendData((const UINT8*)packetBuf, 2);
    if(FUN_STATE_EXIT == resp)
    {
 #ifdef DEBUG_MQTT_CLIENT
        Print("MqttPingRequest() - Publish OK\r\n");
#endif
        ret_val = FUN_STATE_EXIT;
    }
    else
    if(FUN_STATE_ERROR == resp)
    {
#ifdef DEBUG_MQTT_CLIENT
        Print("MqttPingRequest() - Publish  Error\r\n");
#endif
        ret_val = FUN_STATE_ERROR;            
    }
    return ret_val;  
}

/*
*------------------------------------------------------------------------------
* BOOL MqttDisconnect(void)
*
* Summary	: Send mqtt disconnect request
*
* Input     : None
* Output	: BOOL - TRUE/FALSE
*------------------------------------------------------------------------------
*/
BOOL MqttDisconnect(void)
{
    BOOL resp;
    packetBuf[0] = MQTT_CTRL_DISCONNECT << 4;
    packetBuf[1] = 0;
    //resp = MqttSendPacket(packetBuf, 2);
    return resp;  
}

/*
*------------------------------------------------------------------------------
* void InitializeMqttClient(void)
*
* Summary	: Initialize mqtt client  task
*
* Input		: None
*
* Output	: None
*------------------------------------------------------------------------------
*/
void InitializeMqttClient(void)
{
    /* Initialize serial interface for mod bus communication. */
    InitializeReceiveInterface1();
    InitializeTCPClientInterface();
}

/*
*------------------------------------------------------------------------------
* void MqttClientServiceState(void *args)
*
* Summary	: state handle mqtt client operation
*
* Input		: void *args - state related arguments.
*
* Output	: None
*------------------------------------------------------------------------------
*/
void MqttClientServiceState(void *args)
{
	INT8 resp;

	switch(MqttServiceStates)
	{
        case MQTT_STATE_CONNECT:
        {
            strcpy(ServerUser,(CHAR8*)&stSysConfig.mServerUserName[0]);
            strcpy(ServerPsw, (CHAR8*)&stSysConfig.mServermPassword[0]);
            //MqttConnect(CHAR8 *pClientIdentifier, BOOL userNameFlag, BOOL passwordFlag, CHAR8 *pUserName, CHAR8 *pPassword, BOOL cleanSession, UINT8 willFlag, UINT8 willQoS, UINT8 willRetain, UINT16 keepAlive, CHAR8 *pWillTopic, CHAR8 *pWillMessage)
            resp = MqttConnect((CHAR8*)clientID, 1, 1, (CHAR8*)ServerUser, (CHAR8*)ServerPsw, 1, 0, 0, 0, 60, will_topic_dummy, will_payload_dummy);
            if(FUN_STATE_EXIT == resp)
            {
#ifdef DEBUG_MQTT_CLIENT
                Print("MqttClientServiceState() - MQTT_STATE_CONNECT OK\r\n");
#endif
                MqttConnectDelay = millis();
                MqttServiceStates = MQTT_STATE_CONNECT_AFTER_DLY;
              }
            else
            if(FUN_STATE_ERROR == resp)
            {
#ifdef DEBUG_MQTT_CLIENT
                Print("MqttClientServiceState() - MQTT_STATE_CONNECT  Error\r\n");
#endif
                MqttServiceStates = MQTT_STATE_ERROR;
            }
        }
        break;    
        case MQTT_STATE_CONNECT_AFTER_DLY:
        {
            if((millis() - MqttConnectDelay) > MQTT_CONNECT_DELAY)
            {
#ifdef DEBUG_MQTT_CLIENT
                Print("MqttClientServiceState() - MQTT_STATE_CONNECT_AFTER_DLY \r\n");
#endif                
                sprintf(will_payload, (const far rom CHAR8*)"{\"userCount\":\"%05d\",\"mesrTemp\":\"30.5\"}", generateMessageID());
                MqttServiceStates = MQTT_STATE_PUBLISH;
            }
        }
        break;
        case MQTT_STATE_PUBLISH:
        {
            //BOOL MqttPublish(CHAR8 dup, CHAR8 Qos, CHAR8 retain, UINT16 messageID, char *pTopic, CHAR8 *pMessage);
            resp = MqttPublish(0, 0, 0, 10, will_topic, will_payload); 
            if(FUN_STATE_EXIT == resp)
            {
#ifdef DEBUG_MQTT_CLIENT
                Print("MqttClientServiceState() - MQTT_STATE_PUBLISH OK\r\n");
#endif
                MqttDataTxTimout  = millis();
                MqttPingTimeOut   = millis();                
                MqttServiceStates = MQTT_STATE_CHK_TX_PING_INTERVAL;
            }
            else
            if(FUN_STATE_ERROR == resp)
            {
#ifdef DEBUG_MQTT_CLIENT
                Print("MqttClientServiceState() - MQTT_STATE_PUBLISH  Error\r\n");
#endif
                MqttDataTxTimout  = millis();
                MqttPingTimeOut   = millis();                
                MqttServiceStates = MQTT_STATE_CHK_TX_PING_INTERVAL;
            }            
        }
        break;        
        case MQTT_STATE_CHK_TX_PING_INTERVAL:
        {
            /* check for keep alive timeout. */
            if((millis() - MqttPingTimeOut) > 50000)
            {
                                
                MqttServiceStates = MQTT_STATE_PING;  
            }
            else
            /* check for data transmit interval */
            if((millis() - MqttDataTxTimout) > 5000)
            {
                sprintf(will_payload, (const far rom CHAR8*)"{\"userCount\":\"%05d\",\"mesrTemp\":\"30.5\"}", generateMessageID());                
                MqttServiceStates = MQTT_STATE_PUBLISH; 
            }
        }
        break;        
        case MQTT_STATE_PING:
        {
            MqttPingRequest();
            MqttPingTimeOut   = millis();
            MqttServiceStates = MQTT_STATE_PING;  
        }
        break;        
        case MQTT_STATE_ERROR:
        {
            MqttServiceStates = MQTT_STATE_PUBLISH; 
        }
        break;
		default:break;
	}
}
/*
*  End of mqtt_interface.c
*/
