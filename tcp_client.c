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
* tcp_client.c
*
* module to provide tcp client connection functionalyties.
*
*------------------------------------------------------------------------------
*/

/*
*------------------------------------------------------------------------------
* File				: tcp_client.c
* Created by		: Shubham Trivedi
* Last changed by	: Shubham Trivedi
* Last changed		: 06/11/2016
*------------------------------------------------------------------------------
*
* Revision 1.0 06/11/2016 Sam
* Initial revision
*
*------------------------------------------------------------------------------
*/

/*
*------------------------------------------------------------------------------
* Include Files
*------------------------------------------------------------------------------
*/

#include "../includes.h"

/*
*------------------------------------------------------------------------------
* Private Defines
*------------------------------------------------------------------------------
*/

/*
*------------------------------------------------------------------------------
* Private Macros
*------------------------------------------------------------------------------
*/

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

TCP_STATE  	tcpCurrState;
TCP_HANDLE 	tcpVars;
CHAR8		locBuff[25];
UINT8		tcpState;
CHAR8       localIp[20];
UINT8       tcpStatus;

/*
*------------------------------------------------------------------------------
* Private Variables (static)
*------------------------------------------------------------------------------
*/

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
* Public Functions
*------------------------------------------------------------------------------
*/

/*
*------------------------------------------------------------------------------
* void EnterNewStateTcp(TCP_STATE newState)
*
* Summary   : assign new function pointer for tcp current state 
*
* Input     : TCP_STATE newState -  pointer to the new state function
*
* Output    : None
*------------------------------------------------------------------------------
*/
void EnterNewStateTcp(TCP_STATE newState)
{
    /* Assign next state to execute. */
    tcpCurrState = newState;
}

/*
*------------------------------------------------------------------------------
* void TcpClinetStartupState(void *args)
*
* Summary   : Start up sate  - Restart GSM modem and initialise TCP client
*
* Input     : void *args - state related arguments.
*
* Output    : None
*------------------------------------------------------------------------------
*/
void TcpClinetStartupState(void *args)
{
    INT8            resp;
    TCP_HANDLE     *pstTcpHandle;
    pstTcpHandle = (TCP_HANDLE*)args;
                
    switch(pstTcpHandle->mTcCurrState)
    {
        case TC_ST_ECHOOFF:
        {
            /* Turn OFF echo reply to save input buffer space. */
            resp = SetEchoOff();
            if(FUN_STATE_EXIT == resp)
            {
#ifdef DEBUG_TCP_CLIENT
        		Print("TcpClinetStartupState() - TC_ST_ECHOOFF - OK\r\n");
#endif
				pstTcpHandle->mRetryCount = 0;
                pstTcpHandle->mTcCurrState = TC_ST_SHUT_PDP_CONTEX;
            }
            else
            if(FUN_STATE_ERROR == resp)
            {
#ifdef DEBUG_TCP_CLIENT
        		Print("TcpClinetStartupState() - TC_ST_ECHOOFF - Error\r\n");
#endif
            }
        }
        break;
        case TC_ST_SHUT_PDP_CONTEX:
        {
            resp = GPRS_ShutDownGprsContext();
            if(FUN_STATE_EXIT == resp)
            {
#ifdef DEBUG_TCP_CLIENT
        Print("TcpClinetStartupState() - TC_ST_SHUT_PDP_CONTEX - OK\r\n");
#endif
				pstTcpHandle->mRetryCount = 0;
                pstTcpHandle->mTcCurrState = TC_ST_CONNECTION_TYPE;
            }
            else
            if(FUN_STATE_ERROR == resp)
            {
				pstTcpHandle->mRetryCount++;
				if(	pstTcpHandle->mRetryCount > 3)
				{
#ifdef DEBUG_TCP_CLIENT
        			Print("TcpClinetStartupState() - TC_ST_SHUT_PDP_CONTEX - Error\r\n");
#endif
					pstTcpHandle->mRetryCount = 0;
               		pstTcpHandle->mTcCurrState = TC_ST_TCP_START_ERROR;
				}
            }
        }
        break;
        case TC_ST_CONNECTION_TYPE:
        {
            resp = GPRS_SetConnectionType(0);
            if(FUN_STATE_EXIT == resp)
            {
#ifdef DEBUG_TCP_CLIENT
        		Print("TcpClinetStartupState() - TC_ST_CONNECTION_TYPE - OK\r\n");
#endif
				pstTcpHandle->mRetryCount = 0;
                pstTcpHandle->mTcCurrState = TC_ST_CONNECTION_MODE;
            }
            else
            if(FUN_STATE_ERROR == resp)
            {
				pstTcpHandle->mRetryCount++;
				if(	pstTcpHandle->mRetryCount > 3)
				{
#ifdef DEBUG_TCP_CLIENT
        			Print("TcpClinetStartupState() - TC_ST_CONNECTION_TYPE - Error\r\n");
#endif
					pstTcpHandle->mRetryCount = 0;
                	pstTcpHandle->mTcCurrState = TC_ST_TCP_START_ERROR;
				}
            }
        }
        break;                
        case TC_ST_CONNECTION_MODE:
        {
            resp = GPRS_SetTransferMode(0);
            if(FUN_STATE_EXIT == resp)
            {
#ifdef DEBUG_TCP_CLIENT
        		Print("TcpClinetStartupState() - TC_ST_CONNECTION_MODE - OK\r\n");
#endif
				pstTcpHandle->mRetryCount = 0;
                pstTcpHandle->mTcCurrState = TC_ST_NETWORK_ATTACH;
            }
            else
            if(FUN_STATE_ERROR == resp)
            {
				pstTcpHandle->mRetryCount++;
				if(	pstTcpHandle->mRetryCount > 3)
				{
#ifdef DEBUG_TCP_CLIENT
        			Print("TcpClinetStartupState() - TC_ST_CONNECTION_MODE - Error\r\n");
#endif
					pstTcpHandle->mRetryCount = 0;
                	pstTcpHandle->mTcCurrState = TC_ST_TCP_START_ERROR;
				}
            }
        }
        break;     
        case TC_ST_NETWORK_ATTACH:
        {
            resp = GPRS_Attach();
            if(FUN_STATE_EXIT == resp)
            {
#ifdef DEBUG_TCP_CLIENT
        		Print("TcpClinetStartupState() - TC_ST_NETWORK_ATTACH - OK\r\n");
#endif
                pstTcpHandle->mNetworRegTimeout = millis();                
				pstTcpHandle->mRetryCount = 0;
                pstTcpHandle->mTcCurrState = TC_ST_NETWORK_REG;
            }
            else
            if(FUN_STATE_ERROR == resp)
            {
				pstTcpHandle->mRetryCount++;
				if(	pstTcpHandle->mRetryCount > 3)
				{
#ifdef DEBUG_TCP_CLIENT
        			Print("TcpClinetStartupState() - TC_ST_NETWORK_ATTACH - Error\r\n");
#endif
					pstTcpHandle->mRetryCount = 0;
                	pstTcpHandle->mTcCurrState = TC_ST_TCP_START_ERROR;
				}
            }
        }
        break; 
/*----------------------------------------------------------------------------*/        
        case TC_ST_NETWORK_REG:
        {
            resp = CheckForSimRegistration();
            if(FUN_STATE_EXIT == resp)
            {
#ifdef DEBUG_TCP_CLIENT
        		Print("TcpClinetStartupState() - TC_ST_NETWORK_REG - OK\r\n");
#endif
				pstTcpHandle->mRetryCount = 0;
                pstTcpHandle->mTcCurrState = TC_ST_NETWORK_ATTACHED;
            }
            else
            if(FUN_STATE_ERROR == resp)
            {
#ifdef DEBUG_TCP_CLIENT
                Print("TcpClinetStartupState() - TC_ST_NETWORK_REG - Error\r\n");
#endif
                /* 
                * Be in this state till the SIM get registered to the network, if the modem not registered within 
                * defined time restart the modem
                */
                if((millis() - pstTcpHandle->mNetworRegTimeout) > SIM_NETWOR_REG_TIME_OUT)
                {
                    /* Still we have problem with retry also so go to Error State */
                    pstTcpHandle->mRetryCount = 0;
                    pstTcpHandle->mTcCurrState = TC_ST_TCP_START_ERROR;
                }
            }
        }
        break;
        case TC_ST_NETWORK_ATTACHED:
        {
            resp = GPRS_IsAttached();
            if(FUN_STATE_EXIT == resp)
            {
#ifdef DEBUG_TCP_CLIENT
        		Print("TcpClinetStartupState() - TC_ST_NETWORK_ATTACHED - OK\r\n");
#endif
				pstTcpHandle->mRetryCount = 0;
                pstTcpHandle->mTcCurrState = TC_ST_SET_APN_USR_PSW;
            }
            else
            if(FUN_STATE_ERROR == resp)
            {
				pstTcpHandle->mRetryCount++;
				if(	pstTcpHandle->mRetryCount > 3)
				{
#ifdef DEBUG_TCP_CLIENT
        			Print("TcpClinetStartupState() - TC_ST_NETWORK_ATTACHED - Error\r\n");
#endif
					pstTcpHandle->mRetryCount = 0;
                	pstTcpHandle->mTcCurrState = TC_ST_TCP_START_ERROR;
				}
            }
        }
        break;
        case TC_ST_SET_APN_USR_PSW:
        {
            resp = GPRS_SetupApnUsrPsw((CHAR8*)&stSysConfig.mApn[0],(CHAR8*)&stSysConfig.mApnUser[0], (CHAR8*)&stSysConfig.mApnPsw[0]);
            if(FUN_STATE_EXIT == resp)
            {
#ifdef DEBUG_TCP_CLIENT
        		Print("TcpClinetStartupState() - TC_ST_SET_APN_USR_PSW - OK\r\n");
#endif
				pstTcpHandle->mRetryCount = 0;
                pstTcpHandle->mTcCurrState = TC_ST_BRINGUP_WIRELESS;
            }
            else
            if(FUN_STATE_ERROR == resp)
            {
				pstTcpHandle->mRetryCount++;
				if(	pstTcpHandle->mRetryCount > 3)
				{
#ifdef DEBUG_TCP_CLIENT
        			Print("TcpClinetStartupState() - TC_ST_SET_APN_USR_PSW - Error\r\n");
#endif
					pstTcpHandle->mRetryCount = 0;
                	pstTcpHandle->mTcCurrState = TC_ST_TCP_START_ERROR;
				}
			}
        }
        break;
        case TC_ST_BRINGUP_WIRELESS:
        {
            resp = GPRS_StartWirelessConnection();
            if(FUN_STATE_EXIT == resp)
            {
#ifdef DEBUG_TCP_CLIENT
        		Print("TcpClinetStartupState() - TC_ST_BRINGUP_WIRELESS - OK\r\n");
#endif
				pstTcpHandle->mRetryCount = 0;
                pstTcpHandle->mTcCurrState = TC_ST_GET_IP_ASSIGNED;
            }
            else
            if(FUN_STATE_ERROR == resp)
            {
				pstTcpHandle->mRetryCount++;
				if(	pstTcpHandle->mRetryCount > 3)
				{
#ifdef DEBUG_TCP_CLIENT
        			Print("TcpClinetStartupState() - TC_ST_BRINGUP_WIRELESS - Error\r\n");
#endif
					pstTcpHandle->mRetryCount = 0;
                	pstTcpHandle->mTcCurrState = TC_ST_TCP_START_ERROR;
				}
            }
        }
        break;
        case TC_ST_GET_IP_ASSIGNED:
        {
            resp = GPRS_GetLocalIp((CHAR*)&localIp);
            if(FUN_STATE_EXIT == resp)
            {
#ifdef DEBUG_TCP_CLIENT
        		Print("TcpClinetStartupState() - TC_ST_GET_IP_ASSIGNED - OK\r\n");
#endif
				pstTcpHandle->mRetryCount = 0;
                pstTcpHandle->mTcCurrState = TC_ST_START_TCP_CLIENT;
            }
            else
            if(FUN_STATE_ERROR == resp)
            {
				pstTcpHandle->mRetryCount++;
				if(	pstTcpHandle->mRetryCount > 3)
				{
#ifdef DEBUG_TCP_CLIENT
        			Print("TcpClinetStartupState() - TC_ST_GET_IP_ASSIGNED - Error\r\n");
#endif
					pstTcpHandle->mRetryCount = 0;
                	pstTcpHandle->mTcCurrState = TC_ST_TCP_START_ERROR;
				}
            }
        }
        break;
        case TC_ST_START_TCP_CLIENT:
        {
			/* Start TCP client socket. */
            resp = GPRS_StartTCPClient((CHAR8*)&stSysConfig.mServer[0], (CHAR8*)&stSysConfig.mSPort[0]);
            if(FUN_STATE_EXIT == resp)
            {
#ifdef DEBUG_TCP_CLIENT
        		Print("TcpClinetStartupState() - TC_ST_START_TCP_CLIENT - OK\r\n");
#endif
                //pstTcpHandle->mRetryCount = 0;
               // pstTcpHandle->mTcCurrState = TC_ST_GET_STATUS;                
                      
				pstTcpHandle->mRetryCount = 0;
				pstTcpHandle->mTcCurrState = 0;
				/* Goto handle mqtt client service . */
				EnterNewStateTcp((TCP_STATE)MqttClientServiceState);
                
            }
            else
            if(FUN_STATE_ERROR == resp)
            {
				pstTcpHandle->mRetryCount++;
				if(	pstTcpHandle->mRetryCount > 3)
				{
#ifdef DEBUG_TCP_CLIENT
        			Print("TcpClinetStartupState() - TC_ST_START_TCP_CLIENT - Error\r\n");
#endif
					pstTcpHandle->mRetryCount = 0;
                	pstTcpHandle->mTcCurrState = TC_ST_TCP_START_ERROR;
				}
            }
        }
        break;
        case TC_ST_GET_STATUS:
        {
            resp = GPRS_TCPStatus((UINT8*)&tcpStatus);
            if(FUN_STATE_EXIT == resp)
            {
                if(CONNECT_OK == tcpStatus)
                {
#ifdef DEBUG_TCP_CLIENT
                    Print("TcpClinetStartupState() - TC_ST_GET_STATUS - OK\r\n");
#endif
                    pstTcpHandle->mRetryCount = 0;
                    pstTcpHandle->mTcCurrState = 0;
                    /* Goto handle mqtt client service . */
                    EnterNewStateTcp((TCP_STATE)MqttClientServiceState);
                    //EnterNewStateTcp((TCP_STATE)TcpClinetStartupState);
                }
            }
            else
            if(FUN_STATE_ERROR == resp)
            {
                pstTcpHandle->mRetryCount++;
                if(	pstTcpHandle->mRetryCount > 3)
                {
    #ifdef DEBUG_TCP_CLIENT
                    Print("TcpClinetStartupState() - TC_ST_GET_STATUS - Error\r\n");
    #endif
                    pstTcpHandle->mRetryCount = 0;
                    pstTcpHandle->mTcCurrState = TC_ST_TCP_START_ERROR;
                }
            }
        }
        break;
        case TC_ST_TCP_START_ERROR:
        {

#ifdef DEBUG_TCP_CLIENT
		        Print("TcpClinetStartupState() - TC_ST_TCP_START_ERROR \r\n");
#endif
				tcpVars.m_PassError 				= 0;
				pstTcpHandle->mRetryCount = 0;
				pstTcpHandle->mTimeOutCounter     	= millis();
				EnterNewStateTcp((TCP_STATE)TcpErrorState);
        }
        break;
        default:break;
    }
}

/*
*------------------------------------------------------------------------------
* void TcpErrorState(void *args)
*
* Summary	: state gsm module erraor recovery function
*
* Input		: void *args - state related arguments.
*
* Output	: None
*------------------------------------------------------------------------------
*/
void TcpErrorState(void *args)
{
	TCP_HANDLE 		*pstTcpHandle;
	pstTcpHandle	= (TCP_HANDLE*)args;

	if(0 == pstTcpHandle->m_PassError)
	{
#ifdef DEBUG_TCP_CLIENT
		Print("TcpErrorState() - Initialize Modem and USART ..\r\n");
#endif
		/* Not recovered from error, so reset the modem and re initialise the uart.*/
		pstTcpHandle->m_PassError = 1 ;
        //ResetModem();
		ENB_GSM_RST();
		/* Initialize USART for GSM com  */
		//InitialiseSerialInterface();
		pstTcpHandle->mTimeOutCounter = millis();
	}
	if(1 == pstTcpHandle->m_PassError)
	{
		if((UINT32)(millis() - pstTcpHandle->mTimeOutCounter) > MODEM_ERROR_RESET_TIMEOUT)
		{
#ifdef DEBUG_TCP_CLIENT
			Print("TcpErrorState() - Initialize Modem and USART -  Done.\r\n");
#endif
			DIS_GSM_RST();
			CrlearRxBuff1();
			/* Initialize TCP state machine vars to default.*/
			tcpVars.mTcCurrState        = TC_ST_ECHOOFF;
			tcpVars.m_PassError 		= 0;
			tcpVars.mServiceState       = 0;
			tcpVars.mRetryCount         = 0;
			tcpVars.mCommand            = 0;
			tcpVars.mConnId             = 0;
			tcpVars.mMsgType            = 0;
	    	/*Re-enter to TCP client startup state. */
	    	EnterNewStateTcp((TCP_STATE)TcpClinetStartupState);
		}
	}
}
/*
*------------------------------------------------------------------------------
* void InitializeTCPClientInterface(void)
*
* Summary   : Initializes tcp client interface state machine.
*
* Input     : None
*
* Output    : None
*
*------------------------------------------------------------------------------
*/
void InitializeTCPClientInterface(void)
{
  	CrlearRxBuff1();
    /* Initialize TCP state machine vars to default.*/
    tcpVars.mTcCurrState        = TC_ST_ECHOOFF;
	tcpVars.m_PassError 		= 0;
    tcpVars.mServiceState       = 0;
    tcpVars.mRetryCount         = 0;
    tcpVars.mCommand            = 0;
    tcpVars.mConnId             = 0;
    tcpVars.mMsgType            = 0;
    tcpVars.mTimeOutCounter     = millis();
    
    /* Load default parameters. */
#if 0    
    strcpypgm2ram((CHAR8*)&stSysConfig.mDevicePsw[0]	 	, (far const rom CHAR8*)"0000000000" );
	strcpypgm2ram((CHAR8*)&stSysConfig.mDeviceId[0]		 	, (far const rom CHAR8*)"9876543210" );
	strcpypgm2ram((CHAR8*)&stSysConfig.mApn[0]		 		, (far const rom CHAR8*)"airtelgprs.com" ); /*airtelgprs.com  "live.vodafone.com" */
    strcpypgm2ram((CHAR8*)&stSysConfig.mApnUser[0]          , (far const rom CHAR8*)"" );
    strcpypgm2ram((CHAR8*)&stSysConfig.mApnPsw[0]           , (far const rom CHAR8*)"" ); 
	strcpypgm2ram((CHAR8*)&stSysConfig.mServer[0]		 	, (far const rom CHAR8*)"mqtt.dev.dyocense.com" );
    strcpypgm2ram((CHAR8*)&stSysConfig.mSPort[0]		 	, (far const rom CHAR8*)"8886" );
    strcpypgm2ram((CHAR8*)&stSysConfig.mServerUserName[0]   , (far const rom CHAR8*)"mlsd002" );
    strcpypgm2ram((CHAR8*)&stSysConfig.mServermPassword[0]  , (far const rom CHAR8*)"mlsd002" );
#endif
    strcpypgm2ram((CHAR8*)&stSysConfig.mApn[0]		 		, (far const rom CHAR8*)GPRS_APN); 
    strcpypgm2ram((CHAR8*)&stSysConfig.mApnUser[0]          , (far const rom CHAR8*)GPRS_USERNAME);
    strcpypgm2ram((CHAR8*)&stSysConfig.mApnPsw[0]           , (far const rom CHAR8*)GPRS_PASSWORD); 
	strcpypgm2ram((CHAR8*)&stSysConfig.mServer[0]		 	, (far const rom CHAR8*)DYO_SERVER);
    strcpypgm2ram((CHAR8*)&stSysConfig.mSPort[0]		 	, (far const rom CHAR8*)DYO_SERVER_PORT);
    strcpypgm2ram((CHAR8*)&stSysConfig.mServerUserName[0]   , (far const rom CHAR8*)DYO_USERNAME);
    strcpypgm2ram((CHAR8*)&stSysConfig.mServermPassword[0]  , (far const rom CHAR8*)DYO_PSW_KEY);
    strcpypgm2ram((CHAR8*)&stSysConfig.mServerProtocol[0]   , (far const rom CHAR8*)PROTOCOL);
	strcpypgm2ram((CHAR8*)&stSysConfig.mInterval[0]		 	, (far const rom CHAR8*)DYO_DATA_TX_INTERVAL);  
              
    /*Enter to TCP client startup state. */
    EnterNewStateTcp((TCP_STATE)TcpClinetStartupState);

    /* Setup task to run. */
    SCH_AddTask(TcpClientTask, 200,TASK_10MSEC_PERIOD);
}

/*
*------------------------------------------------------------------------------
* void TcpClientTask(void)
*
* Summary	: Process TCP Client communication commands and responses.
*
* Input		: None
*
* Output	: None
*------------------------------------------------------------------------------
*/
void TcpClientTask(void)
{
	if(NULL != tcpCurrState)
	{
		/* Execute current Wifi state. */
		*(tcpCurrState)(&tcpVars);
	}
}

/*
*  End of tcp_client.c
*/
