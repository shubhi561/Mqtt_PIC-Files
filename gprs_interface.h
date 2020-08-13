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
* gprs_interface.h
*
* Include file for tcp_interface module.
*
*------------------------------------------------------------------------------
*/

#ifndef GPRS_INTERFACE_H
#define GPRS_INTERFACE_H

/*
*------------------------------------------------------------------------------
* File				: gprs_interface.h
* Created by		: Sam
* Last changed by	: Sam
* Last changed		: 24/07/2014
*------------------------------------------------------------------------------
*
* Revision 1.0 24/07/2014 Sam
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
* Public Defines
*------------------------------------------------------------------------------
*/
 enum GPRS_TCP_STATE
 {
     IP_INITIAL,
     IP_START,
     IP_CONFIG,
     IP_GPRSACT,
     IP_STATUS,
     TCP_CONNECTING,
     CONNECT_OK,
     TCP_CLOSING,
     TCP_CLOSED,
     TCP_STATE_NONE
 };
/*
*------------------------------------------------------------------------------
* Public Macros
*------------------------------------------------------------------------------
*/

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
extern INT8 GPRS_Attach(void);
extern INT8 GPRS_IsAttached(void);
extern INT8 GPRS_SetConnectionType(UINT8 type);
extern INT8 GPRS_StartWirelessConnection(void);
extern INT8 GPRS_GetLocalIp(CHAR8 *ip);
extern INT8 GPRS_SetTransferMode(UINT8 mode);
extern INT8 GPRS_CloseTcpUdpConnection(void);
extern INT8 GPRS_ShutDownGprsContext(void);
extern INT8 GPRS_SetupApnUsrPsw(CHAR8 *apn,CHAR8 *user, CHAR8 *psw);
extern INT8 GPRS_StratTCPServer(UINT16 port);
extern INT8 GPRS_StartTCPClient(CHAR *ip_domain, CHAR *port);
extern INT8 GPRS_TCPStatus(UINT8 *stat);
extern INT8 GPRS_SendData(const UINT8 *message_str, UINT16 mesg_size);
#endif
/*
*  End of gprs_interface.h
*/





