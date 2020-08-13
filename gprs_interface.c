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
* tcp_interface.c
*
* Module provide functionality to perform TCP operations like init tcp,
* terminate tcp, send tcp etc.
*
*------------------------------------------------------------------------------
*/

/*
*------------------------------------------------------------------------------
* File				: tcp_interface.c
* Created by		: Shubham Trivedi
* Last changed by	: Shubham Trivedi
* Last changed		: 01/11/2014
*------------------------------------------------------------------------------
*
* Revision 1.0 01/11/2014 Sam
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

//static UINT8 extractTCPResponse(CHAR8* msg_data, CHAR8* response);

/*
*------------------------------------------------------------------------------
* Private Functions
*------------------------------------------------------------------------------
*/
/*
*------------------------------------------------------------------------------
* UINT8 extractTCPResponse(CHAR8* msg_data, CHAR8* response)
*
* Summary   : Extract response string from tcp read data.
*
* Input     : CHAR8* msg_data - pionter to tcp response message string.
*             CHAR8* response - pointer to get the extrcted response.
*
* Output    : UINT8 - error code TRUE/FALSE
*
*------------------------------------------------------------------------------
*/
UINT8 extractTCPResponse(CHAR8* msg_data, CHAR8* response)
{
    CHAR8 *p_char;
    CHAR8 *p_char1;
    UINT8  ret_val = TRUE;

    if((NULL != msg_data) && (NULL != response))
    {
        /* extract first filed string. */
        p_char = strchr((CHAR8*)(msg_data),0x0a);
        if(NULL != p_char)
        {
            p_char1 = p_char + 1;
            p_char = strchr((CHAR8*)(p_char1),0x0d);
            if (p_char != NULL)
            {
                *p_char = 0; /* end of string. */
                strcpy((CHAR8*)response, (const CHAR8 *)(p_char1));
            }
            else
            {
                ret_val = FALSE;
            }
        }
        else
        {
            ret_val = FALSE;
        }
    }
    else
    {
        ret_val = FALSE;
    }
    return (ret_val);
}

/*
*------------------------------------------------------------------------------
* Public Functions
*------------------------------------------------------------------------------
*/
/*
*------------------------------------------------------------------------------
* INT8 GPRS_Attach(void)
*
* Summary   :  Attache GPRS
*
* Input		: None
*
* Output	: INT8 - function state FUN_STATE_ENTER/FUN_STATE_ERROR/FUN_STATE_EXIT
*
*------------------------------------------------------------------------------
*/
INT8 GPRS_Attach(void)
{
	INT8 resp 	= FUN_STATE_ENTER;

	/*Check whether GPRS service is attached or not.*/
	memset(tempGsmBuff, 0, sizeof(tempGsmBuff));
    sprintf(tempGsmBuff, (const far rom CHAR8*)"AT+CGATT=1\r");
    resp = SendATCmdWaitResp(tempGsmBuff, 5000, 1000, (const rom INT8*)"OK", 5);
	if(AT_RESP_OK == resp)
	{
#ifdef DEBUG_TCP
        Print("GPRS_Attach() - OK\r\n");
#endif
		resp 	= FUN_STATE_EXIT;
	}
	else
	if((resp == AT_RESP_ERR_NO_RESP) || (resp == AT_RESP_ERR_DIF_RESP))
	{
#ifdef DEBUG_TCP_ERROR
        Print("GPRS_Attach() - Error\r\n");
#endif
		/* Bearer 1 is not open. */
		resp 	= FUN_STATE_ERROR;
	}
	return resp;
}

/*
*------------------------------------------------------------------------------
* INT8 GPRS_IsAttached(void)
*
* Summary   : Checks if GPRS is attached? n=1 if attached
*
* Input		: UINT8 bearer - bearer number to check.
*
* Output	: INT8 - function state FUN_STATE_ENTER/FUN_STATE_ERROR/FUN_STATE_EXIT
*
*------------------------------------------------------------------------------
*/
INT8 GPRS_IsAttached(void)
{

	INT8 resp 	= FUN_STATE_ENTER;

	/*Check whether GPRS service is attached or not.*/
	memset(tempGsmBuff, 0, sizeof(tempGsmBuff));
    sprintf(tempGsmBuff, (const far rom CHAR8*)"AT+CGATT?\r");
    resp = SendATCmdWaitResp(tempGsmBuff, 5000, 1000, (const rom INT8*)"OK", 5);
	if(AT_RESP_OK == resp)
	{
#ifdef DEBUG_TCP
        Print("GPRS_IsAttached() - OK\r\n");
#endif
		/* Service is attached. */
		resp 	= FUN_STATE_EXIT;
	}
	else
	if((resp == AT_RESP_ERR_NO_RESP) || (resp == AT_RESP_ERR_DIF_RESP))
	{
#ifdef DEBUG_TCP_ERROR
        Print("GPRS_IsAttached() - Error\r\n");
#endif
		/* Bearer 1 is not open. */
		resp 	= FUN_STATE_ERROR;
	}
	return resp;
}

/*
*------------------------------------------------------------------------------
* INT8 GPRS_SetConnectionType(UINT8 type)
*
* Summary   : Set connection type.
*
* Input     : UINAT8 type : 0 - Single connection
*                           1 - Multiple connections
*
* Output    : INT8 - function state FUN_STATE_ENTER/FUN_STATE_ERROR/FUN_STATE_EXIT
*------------------------------------------------------------------------------
*/
INT8 GPRS_SetConnectionType(UINT8 type)
{
    INT8 ret_val = FUN_STATE_ENTER;
    INT8 resp;

    /* Issue command to set the connection type. */
    memset(tempGsmBuff, 0, sizeof(tempGsmBuff));
    sprintf(tempGsmBuff, (const far rom CHAR8*)"AT+CIPMUX=%d\r", type);
    resp = SendATCmdWaitResp(tempGsmBuff, 5000, 1000, (const rom INT8*)"OK", 5);
    if(AT_RESP_OK == resp)
    {
#ifdef DEBUG_TCP

        Print("GPRS_SetConnectionType() - OK\r\n");
#endif
        /* connection type set . */
        ret_val =FUN_STATE_EXIT;
    }
    else
    if((resp == AT_RESP_ERR_NO_RESP) || (resp == AT_RESP_ERR_DIF_RESP))
    {
#ifdef DEBUG_TCP_ERROR

        Print("GPRS_SetConnectionType() - Error\r\n");
#endif
        /* response was not received in specific time or response was not desired. */
        ret_val =FUN_STATE_ERROR;
    }

    return (ret_val);
}

/*
*------------------------------------------------------------------------------
* INT8 GPRS_StartWirelessConnection(void)
*
* Summary   : Bring up wireless connection with GPRS
*
* Input     : None
*
* Output    : INT8 - function state FUN_STATE_ENTER/FUN_STATE_ERROR/FUN_STATE_EXIT
*
*------------------------------------------------------------------------------
*/
INT8 GPRS_StartWirelessConnection(void)
{
    INT8 resp   = FUN_STATE_ENTER;

    memset(tempGsmBuff, 0, sizeof(tempGsmBuff));
    sprintf(tempGsmBuff, (const far rom CHAR8*)"AT+CIICR\r");
    resp = SendATCmdWaitResp(tempGsmBuff, 10000, 1000, (const rom INT8*)"OK", 5);
    if(AT_RESP_OK == resp)
    {
#ifdef DEBUG_TCP

        Print("GPRS_StartWirelessConnection() - OK\r\n");
#endif
        resp = FUN_STATE_EXIT;
    }
    else
    if((resp == AT_RESP_ERR_NO_RESP) || (resp == AT_RESP_ERR_DIF_RESP))
    {
#ifdef DEBUG_TCP_ERROR

        Print("GPRS_StartWirelessConnection() - Error\r\n");
#endif
        resp = FUN_STATE_ERROR;
    }

    return resp;
}

/*
*------------------------------------------------------------------------------
* INT8 GPRS_GetLocalIp(void)
*
* Summary   : Get Local Ip
*
* Input     : CHAR8 *ip - pointer to get local ip
*
* Output    : INT8 - function state FUN_STATE_ENTER/FUN_STATE_ERROR/FUN_STATE_EXIT
*
*------------------------------------------------------------------------------
*/
INT8 GPRS_GetLocalIp(CHAR8 *ip)
{
    INT8 resp   = FUN_STATE_ENTER;
    RX_HANDLE1 *pstRxHandle1 = &stRxHandle1;

    /* Issue local ip get command. */
    memset(tempGsmBuff, 0, sizeof(tempGsmBuff));
    sprintf(tempGsmBuff, (const far rom CHAR8*)"AT+CIFSR\r");
    resp = SendATCmdWaitResp(tempGsmBuff, 5000, 1000, (const rom INT8*)"OK", 5);
    if((resp == AT_RESP_ERR_NO_RESP))
    {
#ifdef DEBUG_TCP
        Print("GPRS_GetLocalIp() - Error\r\n");
#endif
        resp = FUN_STATE_ERROR;
    }
	else
	if(resp == AT_RESP_ERR_DIF_RESP)
	{
        /* some error in extracting ip address need to fix SAM - 29-July-2020 */
       //extractTCPResponse((CHAR8*)&pstRxHandle1->mCommBuffer[2], ip);
#ifdef DEBUG_TCP
		//DebugPrintRam(ip);
        Print("GPRS_GetLocalIp() - OK\r\n");
#endif
        resp = FUN_STATE_EXIT;
	}
    return resp;
}

/*
*------------------------------------------------------------------------------
* INT8 GPRS_SetTransferMode(UINT8 mode)
*
* Summary   : Set data transfer mode
*
* Input     : UINAT8 mode : 0 - Normal mode
*                           1 - unvarnished transmission mode
*
* Output    : INT8 - function state FUN_STATE_ENTER/FUN_STATE_ERROR/FUN_STATE_EXIT
*------------------------------------------------------------------------------
*/
INT8 GPRS_SetTransferMode(UINT8 mode)
{
    INT8 ret_val = FUN_STATE_ENTER;
    INT8 resp;

    /* Issue command to set data transfer mode. */
    memset(tempGsmBuff, 0, sizeof(tempGsmBuff));
    sprintf(tempGsmBuff, (const far rom CHAR8*)"AT+CIPMODE=%d\r", mode);
    resp = SendATCmdWaitResp(tempGsmBuff, 5000, 1000, (const rom INT8*)"OK", 5);
    if(AT_RESP_OK == resp)
    {
#ifdef DEBUG_TCP
        Print("GPRS_SetTransferMode() - OK\r\n");
#endif
        /* transfer mode set . */
        ret_val =FUN_STATE_EXIT;
    }
    else
    if((resp == AT_RESP_ERR_NO_RESP) || (resp == AT_RESP_ERR_DIF_RESP))
    {
#ifdef DEBUG_TCP
        Print("GPRS_SetTransferMode() - Error\r\n");
#endif
        /* response was not received in specific time. */
        ret_val =FUN_STATE_ERROR;
    }
    return (ret_val);
}

/*
*------------------------------------------------------------------------------
* INT8 GPRS_CloseTcpUdpConnection(void)
*
* Summary   : Close TCP/UDP Connection
*
* Input     : None
*
* Output    : INT8 - function state FUN_STATE_ENTER/FUN_STATE_ERROR/FUN_STATE_EXIT
*------------------------------------------------------------------------------
*/
INT8 GPRS_CloseTcpUdpConnection(void)
{
    INT8 ret_val = FUN_STATE_ENTER;
    INT8 resp;

    /* Issue command to close tcp connection. */
    memset(tempGsmBuff, 0, sizeof(tempGsmBuff));
    sprintf(tempGsmBuff, (const far rom CHAR8*)"AT+CIPCLOSE\r");
    resp = SendATCmdWaitResp(tempGsmBuff, 5000, 1000, (const rom INT8*)"OK", 5);
    if(AT_RESP_OK == resp)
    {
#ifdef DEBUG_TCP
        Print("GPRS_CloseTcpUdpConnection() - OK\r\n");
#endif
         ret_val =FUN_STATE_EXIT;
    }
    else
    if((resp == AT_RESP_ERR_NO_RESP) || (resp == AT_RESP_ERR_DIF_RESP))
    {
#ifdef DEBUG_TCP
        Print("GPRS_CloseTcpUdpConnection() - Error\r\n");
#endif
        /* response was not received in specific time. */
        ret_val =FUN_STATE_ERROR;
    }
    return (ret_val);
}

/*
*------------------------------------------------------------------------------
* INT8 GPRS_ShutDownGprsContext(void)
*
* Summary   : Deactivate GPRS PDP context
*
* Input     : None
*
* Output    : INT8 - function state FUN_STATE_ENTER/FUN_STATE_ERROR/FUN_STATE_EXIT
*------------------------------------------------------------------------------
*/
INT8 GPRS_ShutDownGprsContext(void)
{
    INT8 ret_val = FUN_STATE_ENTER;
    INT8 resp;

    /* Issue command to shut off GPRS context.*/
    memset(tempGsmBuff, 0, sizeof(tempGsmBuff));
    sprintf(tempGsmBuff, (const far rom CHAR8*)"AT+CIPSHUT\r");
    resp = SendATCmdWaitResp(tempGsmBuff, 5000, 1000, (const rom INT8*)"SHUT OK", 5);
    if(AT_RESP_OK == resp)
    {
#ifdef DEBUG_TCP
        Print("GPRS_ShutDownGprsContext() - OK\r\n");
#endif
         ret_val =FUN_STATE_EXIT;
    }
    else
    if((resp == AT_RESP_ERR_NO_RESP) || (resp == AT_RESP_ERR_DIF_RESP))
    {
#ifdef DEBUG_TCP
        Print("GPRS_ShutDownGprsContext() - Error\r\n");
#endif
        /* response was not received in specific time. */
        ret_val =FUN_STATE_ERROR;
    }
    return (ret_val);
}

/*
*------------------------------------------------------------------------------
* INT8 GPRS_SetupApnUsrPsw(void)
*
* Summary   : Start task and set apn, user and password
*
* Input     : CHAR8 *apn,CHAR8 *user, CHAR8 *psw
*
* Output    : INT8 - function state FUN_STATE_ENTER/FUN_STATE_ERROR/FUN_STATE_EXIT
*------------------------------------------------------------------------------
*/
INT8 GPRS_SetupApnUsrPsw(CHAR8 *apn, CHAR8 *user, CHAR8 *psw)
{
    INT8 ret_val = FUN_STATE_ENTER;
    INT8 resp;

    /* Issue command to shut off GPRS context.*/
    memset(tempGsmBuff, 0, sizeof(tempGsmBuff));
    /* Set APN Name, user and password. */
    sprintf(tempGsmBuff, (const far rom char*)"AT+CSTT=\"%s\",\"%s\",\"%s\"\r", apn, user, psw);
    resp = SendATCmdWaitResp(tempGsmBuff, 5000, 1000, (const rom INT8*)"OK", 5);
    if(AT_RESP_OK == resp)
    {
#ifdef DEBUG_TCP
        Print("GPRS_SetupApnUsrPsw() - OK\r\n");
#endif
         ret_val =FUN_STATE_EXIT;
    }
    else
    if((resp == AT_RESP_ERR_NO_RESP) || (resp == AT_RESP_ERR_DIF_RESP))
    {
#ifdef DEBUG_TCP
        Print("GPRS_SetupApnUsrPsw() - Error\r\n");
#endif
        /* response was not received in specific time. */
        ret_val =FUN_STATE_ERROR;
    }
    return (ret_val);
}

/*
*------------------------------------------------------------------------------
* INT8 GPRS_StratTCPServer(UINT16 port)
*
* Summary   : Enable TCP server
*
* Input     : UINT16 port - port number
*
* Output    : INT8 - function state FUN_STATE_ENTER/FUN_STATE_ERROR/FUN_STATE_EXIT
*------------------------------------------------------------------------------
*/
INT8 GPRS_StratTCPServer(UINT16 port)
{
    INT8 ret_val = FUN_STATE_ENTER;
    INT8 resp;

    /* Issue command to start server. */
    memset(tempGsmBuff, 0, sizeof(tempGsmBuff));
    sprintf(tempGsmBuff, (const far rom CHAR8*)"AT+CIPSERVER=1,%d\r", port);
    resp = SendATCmdWaitResp(tempGsmBuff, 5000, 1000, (const rom INT8*)"OK", 5);
    if(AT_RESP_OK == resp)
    {
#ifdef DEBUG_TCP
        Print("GPRS_StratTCPServer() - OK\r\n");
#endif
        /* server started. */
        ret_val =FUN_STATE_EXIT;
    }
    else
    if((resp == AT_RESP_ERR_NO_RESP) || (resp == AT_RESP_ERR_DIF_RESP))
    {
#ifdef DEBUG_TCP
        Print("GPRS_StratTCPServer() - Error\r\n");
#endif
        /* response was not received in specific time. */
        ret_val =FUN_STATE_ERROR;
    }
    return (ret_val);
}

/*
*------------------------------------------------------------------------------
* INT8 GPRS_StartTCPClient(CHAR *ip_domain, CHAR *port)
*
* Summary   : Start TCP client connection 
*
* Input     : CHAR *ip_domain, CHAR *port
*
* Output    : INT8 - function state FUN_STATE_ENTER/FUN_STATE_ERROR/FUN_STATE_EXIT
*------------------------------------------------------------------------------
*/
INT8 GPRS_StartTCPClient(CHAR *ip_domain, CHAR *port)
{
    INT8 ret_val = FUN_STATE_ENTER;
    INT8 resp;
    static UINT8 pass = 0;
    if(0 == pass)
    {
        /* Issue command to shut off GPRS context.*/
        memset(tempGsmBuff, 0, sizeof(tempGsmBuff));
       	sprintf(tempGsmBuff, (const far rom char*)"AT+CIPSTART=\"TCP\",\"%s\",\"%s\"\r", ip_domain, port);
        //resp = SendATCmdWaitResp(tempGsmBuff, 5000, 5000, (const rom INT8*)"OK", 5);
        resp = SendATCmdWaitResp(tempGsmBuff, 5000, 5000, (const rom INT8*)"CONNECT OK", 5);
        if(AT_RESP_OK == resp)
        {
#ifdef DEBUG_TCP
            Print("GPRS_StartTCPClient() - Start OK\r\n");
#endif
           // pass = 1;
			//RxInit1(10000, 200);
            pass = 0;
            ret_val =FUN_STATE_EXIT;            
        }
        else
        if((resp == AT_RESP_ERR_NO_RESP) || (resp == AT_RESP_ERR_DIF_RESP))
        {
#ifdef DEBUG_TCP
            Print("GPRS_StartTCPClient() - Error\r\n");
#endif
            /* response was not received in specific time. */
            pass = 0;
            ret_val =FUN_STATE_ERROR;
        }
    }

    return (ret_val);
}

/*
*------------------------------------------------------------------------------
* INT8 GPRS_SendData(const UINT8 *message_str, UINT16 mesg_size)
*
* Summary   : Send data through socket
*
* Input     : const UINT8 *message_str  - data to send  (MAX data size 2048 bytes)
*           : UINT16 mesg_size - size of message string.
* Output    : INT8 - current function state.
*------------------------------------------------------------------------------
*/
INT8 GPRS_SendData(const UINT8 *message_str, UINT16 mesg_size)
{
    static UINT8 pass = 0;
    INT8 ret_val = FUN_STATE_ENTER;
    INT8 resp;

    if(0 == pass)
    {
        /* clear temp array for command load.*/
        memset(tempGsmBuff, 0, sizeof(tempGsmBuff));
        sprintf(tempGsmBuff, (const far rom char*)"AT+CIPSEND\r");
        resp = SendATCmdWaitResp(tempGsmBuff, 3000, 200,(const rom INT8*)">", 5);
        if(AT_RESP_OK == resp)
        {
            /* Wrap return > after set command. goto data send pass.*/
            pass = 1;
        }
        else
        if((resp == AT_RESP_ERR_NO_RESP) || (resp == AT_RESP_ERR_DIF_RESP))
        {
#ifdef DEBUG_TCP
            Print("GPRS_SendData() - Prompt Error\r\n");
#endif
            /* response was not received in specific time.*/
            pass = 0;
            ret_val = FUN_STATE_ERROR;
        }
    }
    else
    if(1 == pass)
    {
        /* Begins receive of serial data, when data length is met, starts transmission of data.*/
        memset(tempGsmBuff, 0, sizeof(tempGsmBuff));
        //sprintf(tempGsmBuff, (const far rom char*)"%s", message_str);
        memcpy(tempGsmBuff, message_str, mesg_size);
		tempGsmBuff[mesg_size] = CTRL_Z;
        mesg_size++;
        resp = SendATCmdWaitResp1(tempGsmBuff, 10000, 5000, (const rom INT8*)"SEND OK", 5, mesg_size);
        if(AT_RESP_OK == resp)
        {
#ifdef DEBUG_TCP
            Print("GPRS_SendData() - Send OK\r\n");
#endif
            /* data send success. */
            pass = 0;
            ret_val = FUN_STATE_EXIT;
        }
        else
        if((resp == AT_RESP_ERR_NO_RESP) || (resp == AT_RESP_ERR_DIF_RESP))
        {
#ifdef DEBUG_TCP
            Print("GPRS_SendData() - Send Error\r\n");
#endif
            /* response was not received in specific time. */
            pass = 0;
            ret_val = FUN_STATE_ERROR;
        }
   }

    return (ret_val);
}


/*
*------------------------------------------------------------------------------
* INT8 GPRS_TCPStatus(CHAR *stat)
*
* Summary   : Check TCP connection status
*
* Input     : CHAR *stat - TCP connection status.
*
* Output    : INT8 - function state FUN_STATE_ENTER/FUN_STATE_ERROR/FUN_STATE_EXIT
*------------------------------------------------------------------------------
*/
INT8 GPRS_TCPStatus(UINT8 *stat)
{
    INT8 ret_val = FUN_STATE_ENTER;
    INT8 resp;

    /* Issue command to start server. */
    memset(tempGsmBuff, 0, sizeof(tempGsmBuff));
    sprintf(tempGsmBuff, (const far rom CHAR8*)"AT+CIPSTATUS\r");
    resp = SendATCmdWaitResp(tempGsmBuff, 5000, 1000, (const rom INT8*)"CONNECT OK", 5);
    if(AT_RESP_OK == resp)
    {
#ifdef DEBUG_TCP
        Print("GPRS_TCPStatus() - CONNECT OK\r\n");
#endif
        *stat = CONNECT_OK;
        ret_val =FUN_STATE_EXIT;
    }
    else
    if(resp == AT_RESP_ERR_NO_RESP)
    {
#ifdef DEBUG_TCP
        Print("GPRS_TCPStatus() - ERROR\r\n");
#endif
        *stat = TCP_STATE_NONE;
        ret_val =FUN_STATE_ERROR;
    }
    else
    if(resp == AT_RESP_ERR_DIF_RESP)
    {
        if(IsStringReceived1((const rom INT8*)"IP INITIAL"))
        {
#ifdef DEBUG_TCP
        Print("GPRS_TCPStatus() - IP INITIAL\r\n");
#endif            
            *stat = IP_INITIAL;
            ret_val =FUN_STATE_EXIT;
        }
        else
        if(IsStringReceived1((const rom INT8*)"IP START"))
        {
#ifdef DEBUG_TCP
        Print("GPRS_TCPStatus() - IP START\r\n");
#endif            
            *stat = IP_START;
            ret_val =FUN_STATE_EXIT;
        }
        else
        if(IsStringReceived1((const rom INT8*)"IP CONFIG"))
        {
#ifdef DEBUG_TCP
        Print("GPRS_TCPStatus() - IP CONFIG\r\n");
#endif            
            *stat = IP_CONFIG;
            ret_val =FUN_STATE_EXIT;
        }
        else
        if(IsStringReceived1((const rom INT8*)"IP GPRSACT"))
        {
#ifdef DEBUG_TCP
        Print("GPRS_TCPStatus() - IP GPRSACT\r\n");
#endif            
            *stat = IP_GPRSACT;
            ret_val =FUN_STATE_EXIT;
        }
        else
        if(IsStringReceived1((const rom INT8*)"IP STATUS"))
        {
#ifdef DEBUG_TCP
        Print("GPRS_TCPStatus() - IP STATUS\r\n");
#endif            
            *stat = IP_STATUS;
            ret_val =FUN_STATE_EXIT;
        }
        else
        if(IsStringReceived1((const rom INT8*)"TCP CLOSING"))
        {
#ifdef DEBUG_TCP
        Print("GPRS_TCPStatus() - TCP CLOSING\r\n");
#endif            
            *stat = TCP_CONNECTING;
            ret_val =FUN_STATE_EXIT;
        }
        else
        if(IsStringReceived1((const rom INT8*)"TCP CLOSING"))
        {
#ifdef DEBUG_TCP
        Print("GPRS_TCPStatus() - TCP CLOSING\r\n");
#endif            
            *stat = TCP_CLOSING;
            ret_val =FUN_STATE_EXIT;
        }
        else
        if(IsStringReceived1((const rom INT8*)"TCP CLOSED"))
        {
#ifdef DEBUG_TCP
        Print("GPRS_TCPStatus() - CLOSEDr\n");
#endif            
            *stat = TCP_CLOSED;
            ret_val =FUN_STATE_EXIT;
        }
     }

    return (ret_val);
}

/*
*  End of tcp_interface.c
*/

