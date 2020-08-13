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
* app_spec_ui.h
*
* Include file for tcp_client.h
*
*------------------------------------------------------------------------------
*/

#ifndef TCP_CLIENT_H
#define TCP_CLIENT__H

/*
*------------------------------------------------------------------------------
* File				: tcp_client.h
* Created by		: Sam
* Last changed by	: Sam
* Last changed      : 06/11/2016
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
* Hardware Port Allocation
*------------------------------------------------------------------------------
*/

/*
*------------------------------------------------------------------------------
* Public Defines
*------------------------------------------------------------------------------
*/
 
enum GPRS_STATRUP
{
    GPRS_ST_SIM_READY = 0,
    GPRS_ST_ECHOOFF,
    GPRS_ST_NETWORK_REG,
    GPRS_ST_NETWORK_ATTACHED,
    GPRS_ST_SHUT_PDP_CONTEX,
    GPRS_ST_CONNECTION_TYPE,
    GPRS_ST_SET_APN_USR_PSW,
    GPRS_ST_BRINGUP_WIRELESS,
    GPRS_ST_GET_IP_ASSIGNED,
    GPRS_ST_START_TCP_CLIENT,
    GPRS_ST_TCP_START_ERROR,
    STARTUP_LAST_ITEM
};

enum TCP_CLIENT
{
    TC_ST_ECHOOFF = 0,
    TC_ST_SHUT_PDP_CONTEX,     
    TC_ST_CONNECTION_TYPE,
    TC_ST_CONNECTION_MODE,
    TC_ST_NETWORK_ATTACH,
    /*-------------------------*/ 
    TC_ST_NETWORK_REG,
    TC_ST_NETWORK_ATTACHED,
    TC_ST_SET_APN_USR_PSW,
    TC_ST_BRINGUP_WIRELESS,
    TC_ST_GET_IP_ASSIGNED,
    TC_ST_START_TCP_CLIENT,
    TC_ST_GET_STATUS,
    TC_ST_TCP_START_ERROR,
    TC_STARTUP_LAST_ITEM
};

/*
*------------------------------------------------------------------------------
* Public Macros
*------------------------------------------------------------------------------
*/

/*Modem reset pin will be held at high for the period of MODEM_ERROR_RESET_TIMEOUT */
#define 	MODEM_ERROR_RESET_TIMEOUT		(UINT32)(50)	/*50 ms reset period. */

#define 	MODEM_CMD_RETRY_TIME			(UINT32)(2000)	/*2000 ms retry period. */
/* SIM network registration time out, wait till the time out for network registration. if fail to register to network restart the modem. */
#define 	SIM_NETWOR_REG_TIME_OUT			(UINT32)((UINT32)1000 * (UINT32)60 * (UINT32)30)	/* 30Min. */

/* Modem reset interval after any error case. */
#define		TCP_ERROR_RESET_TIMEOUT			(UINT32)(5000)		/* 1 min. */


/* TCP client socket alive test interval. */
#define	KEEP_ALIVE_INTERVAL					(UINT32)((UINT32)1000 * (UINT32)60 * (UINT32)2)	/* 20Min. */
//#define	KEEP_ALIVE_INTERVAL					(UINT32)((UINT32)2000

/*
*------------------------------------------------------------------------------
* Public Data Types
*------------------------------------------------------------------------------
*/

/* Create user defined data type for tcp client function pointer. */
typedef void (*tcpState)(void * args);
typedef tcpState (*TCP_STATE)(void *args);

/* Structure holds the Wifi tcpState machine params. . */
typedef struct _TCP_HANDLE
{
    UINT8       mTcCurrState;
    UINT8       nTcNextState;
	UINT8       m_PassError;
    UINT8       mRetryCount;
    UINT8       mServiceState;
    UINT8       mMsgType;
    UINT8       mCommand;
    UINT8       mConnId;
    UINT32      mTimeOutCounter;
    UINT32      mNetworRegTimeout;
}TCP_HANDLE;

/*
*------------------------------------------------------------------------------
* Public Variables (extern)
*------------------------------------------------------------------------------
*/

extern TCP_HANDLE tcpVars;
extern CHAR8       localIp[20];

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

extern void InitializeTCPClientInterface(void);
extern void TcpClientTask(void);
extern void TcpClinetStartupState(void *args);
extern void TcpErrorState(void *args);

#endif
/*
*  End of tcp_client.h
*/
