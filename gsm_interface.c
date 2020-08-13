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
* gsm_interface.c
*
* GSM/GPRS Communication interface module
*
*------------------------------------------------------------------------------
*/

/*
*------------------------------------------------------------------------------
* File				: gsm_interface.c
* Created by		: Shubham Trivedi
* Last changed by	: Shubham Trivedi
* Last changed		: 20/05/2020
*------------------------------------------------------------------------------
*
* Revision 1.0 07/10/2013 
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

UINT8 passSendATcmd = 0;
#pragma udata SerData
INT8 tempGsmBuff[MAX_GSM_STRING_SIZE];
#pragma udata

/*
*------------------------------------------------------------------------------
* Private Variables (static)
*------------------------------------------------------------------------------
*/

static UINT8 	sendRespPass 	= 0;

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
* INT8 SetEchoOff(void)
*
* Summary	: Set Command response echo mode
*
* Input		: void
*
* Output	: INT8 - current function state.
*------------------------------------------------------------------------------
*/
INT8 SetEchoOff(void)
{
	INT8 ret_val = FUN_STATE_ENTER;
	INT8 resp;

	/* Issue Echo mode set command. */
	memset(tempGsmBuff, 0, sizeof(tempGsmBuff));
	sprintf(tempGsmBuff, (const far rom char*)"ATE0\r\n");
	resp = SendATCmdWaitResp(tempGsmBuff, 1000, 50, (const rom INT8*)"OK", 5);
	if(AT_RESP_OK == resp)
	{
		ret_val =FUN_STATE_EXIT;
	}
	else
	if((resp == AT_RESP_ERR_NO_RESP) || (resp == AT_RESP_ERR_DIF_RESP))
	{
		ret_val =FUN_STATE_ERROR;
	}
	return (ret_val);
}

/*
*------------------------------------------------------------------------------
* INT8 ResetModem(void)
*
* Summary	: Reset the modem. this command doesn't return any ack value, just
*             wait to time out and return error response.
*
* Input		: None
*
* Output	: INT8 - current function state.
*------------------------------------------------------------------------------
*/
INT8 ResetModem(void)
{	
	INT8 ret_val = FUN_STATE_ENTER;
	INT8 resp;

	/* Issue reset modem command. */
	memset(tempGsmBuff, 0, sizeof(tempGsmBuff));
	sprintf(tempGsmBuff, (const far rom char*)"AT+CFUN=1,1\r");
	resp = SendATCmdWaitResp(tempGsmBuff, 1000, 1000, (const rom INT8*)"OK", 5);
	if(AT_RESP_OK == resp)
	{
		ret_val =FUN_STATE_EXIT;
	}
	else
	if((resp == AT_RESP_ERR_NO_RESP) || (resp == AT_RESP_ERR_DIF_RESP))
	{
		ret_val =FUN_STATE_ERROR;
	}
  	return (ret_val);
}

/*
*------------------------------------------------------------------------------
* INT8 GSM::SendATCmdWaitResp(const INT8 *AT_cmd_string,
*                UINT16 start_comm_tmout, UINT16 max_interchar_tmout,
*                const rom INT8 *response_string,
*                UINT8 no_of_attempts)
*
* Summary	: Method sends AT command and waits for response
*
* Input		: None
*
* Output	: AT_RESP_IN_PROG  = 0 - geting response in progress.
*             AT_RESP_ERR_NO_RESP = 1, no response received.
*      		  AT_RESP_ERR_DIF_RESP = 2, response_string is different from the required response.
*             AT_RESP_OK = 3,           response_string was included in the response.
*
*------------------------------------------------------------------------------
*/
INT8 SendATCmdWaitResp(const INT8 *AT_cmd_string,
                UINT16 start_comm_tmout, UINT16 max_interchar_tmout,
                const rom INT8 *response_string,
                UINT8 no_of_attempts)
{

	UINT8 status;
	INT8 ret_val = AT_RESP_IN_PROG;

	if(0 == passSendATcmd)
	{
		RxInit1(start_comm_tmout, max_interchar_tmout);
#ifdef GSM_PORT_LEVEL_DEBUG
		PutrsUSART2Ram(AT_cmd_string);
#endif
		PutStringOnGsmPort(AT_cmd_string);
		passSendATcmd = 1;
	}
	else
	if(1 == passSendATcmd)
	{

		status = Rx1WaitResp(start_comm_tmout, max_interchar_tmout);
		if(status != RX_NOT_FINISHED1)
		{
			if(status == RX_FINISHED1)
			{
				/* something was received but what was received?. */
				if(IsStringReceived1((const rom CHAR8*)response_string))
				{
					/* response is OK => finish. */
					ret_val = AT_RESP_OK;
					passSendATcmd = 0;
					return (ret_val);
				}
				else
				{
					passSendATcmd = 0;
					ret_val = AT_RESP_ERR_DIF_RESP;
				}
			}
			else
			{
				/* nothing was received. */
				ret_val = AT_RESP_ERR_NO_RESP;
				passSendATcmd = 0;
			}
		}
	}
	return (ret_val);
}

/*
*------------------------------------------------------------------------------
* INT8 GSM::SendATCmdWaitResp1(const INT8 *AT_cmd_string,
*                UINT16 start_comm_tmout, UINT16 max_interchar_tmout,
*                const rom INT8 *response_string,
*                UINT8 no_of_attempts)
*
* Summary	: Method sends AT command and waits for response
*
* Input		: None
*
* Output	: AT_RESP_IN_PROG  = 0 - geting response in progress.
*             AT_RESP_ERR_NO_RESP = 1, no response received.
*      		  AT_RESP_ERR_DIF_RESP = 2, response_string is different from the required response.
*             AT_RESP_OK = 3,           response_string was included in the response.
*
*------------------------------------------------------------------------------
*/
INT8 SendATCmdWaitResp1(const INT8 *AT_cmd_string,
                UINT16 start_comm_tmout, UINT16 max_interchar_tmout,
                const rom INT8 *response_string,
                UINT8 no_of_attempts, UINT16 len)
{

	UINT8 status;
	INT8 ret_val = AT_RESP_IN_PROG;

	if(0 == passSendATcmd)
	{
		RxInit1(start_comm_tmout, max_interchar_tmout);
#ifdef GSM_PORT_LEVEL_DEBUG
		PutrsUSART2Ram(AT_cmd_string);
#endif
        //PutArrayUSART1Ram((const UINT8*)AT_cmd_string, len);
        PutArrayUSART2Ram((const UINT8*)AT_cmd_string, len);       
		passSendATcmd = 1;
	}
	else
	if(1 == passSendATcmd)
	{
		status = Rx1WaitResp(start_comm_tmout, max_interchar_tmout);
		if(status != RX_NOT_FINISHED1)
		{
			if(status == RX_FINISHED1)
			{
				/* something was received but what was received?. */
				if(IsStringReceived1((const rom CHAR8*)response_string))
				{
					/* response is OK => finish. */
					ret_val = AT_RESP_OK;
					passSendATcmd = 0;
					return (ret_val);
				}
				else
				{
					passSendATcmd = 0;
					ret_val = AT_RESP_ERR_DIF_RESP;
				}
			}
			else
			{
				/* nothing was received. */
				ret_val = AT_RESP_ERR_NO_RESP;
				passSendATcmd = 0;
			}
		}
	}
	return (ret_val);
}

/*
*------------------------------------------------------------------------------
* INT8 CheckForSimRegistration(void)
*
* Summary	: Check for SIM registration with the network.
*
* Input		: None
*
* Output	: INT8 - current function state.
*------------------------------------------------------------------------------
*/
INT8 CheckForSimRegistration(void)
{
	static BOOL simreg_pass = 0;
	INT8 ret_val = FUN_STATE_ENTER;
	INT8 resp;
	if(0 == simreg_pass)
	{
		/* Check for SIM registration with the home network. */
		memset(tempGsmBuff, 0, sizeof(tempGsmBuff));
		sprintf(tempGsmBuff, (const far rom char*)"AT+CREG?\r");
		resp = SendATCmdWaitResp(tempGsmBuff, 2000, 1000, (const rom INT8*)"+CREG: 0,1", 5);
		if(AT_RESP_OK == resp)
		{
			simreg_pass = 0;
			ret_val =FUN_STATE_EXIT;
		}
		else
		if((resp == AT_RESP_ERR_NO_RESP) || (resp == AT_RESP_ERR_DIF_RESP))
		{
			simreg_pass = 1;
		}
	}
	else
	if(1 == simreg_pass)
	{
		/* Check for SIM registration with the roming network. */
		memset(tempGsmBuff, 0, sizeof(tempGsmBuff));
		sprintf(tempGsmBuff, (const far rom char*)"AT+CREG?\r");
		resp = SendATCmdWaitResp(tempGsmBuff, 2000, 1000, (const rom INT8*)"+CREG: 0,5", 5);
		if(AT_RESP_OK == resp)
		{
			simreg_pass = 0;
			ret_val =FUN_STATE_EXIT;
		}
		else
		if((resp == AT_RESP_ERR_NO_RESP) || (resp == AT_RESP_ERR_DIF_RESP))
		{
			simreg_pass = 0;
			ret_val =FUN_STATE_ERROR;
		}
	}
  	return (ret_val);
}

/*
*------------------------------------------------------------------------------
* INT8 CheckForSimReady(void)
*
* Summary	: Check for SIM is ready or not
*
* Input		: None
*
* Output	: INT8 - current function state.
*------------------------------------------------------------------------------
*/
INT8 CheckForSimReady(void)
{
	INT8 ret_val = FUN_STATE_ENTER;
	INT8 resp;

	/* Check for SIM registration with the network. */
	memset(tempGsmBuff, 0, sizeof(tempGsmBuff));
	sprintf(tempGsmBuff, (const far rom char*)"AT+CPIN?\r");
	resp = SendATCmdWaitResp(tempGsmBuff, 2000, 1000, (const rom INT8*)"+CPIN: READY", 5);
	if(AT_RESP_OK == resp)
	{
		ret_val =FUN_STATE_EXIT;
	}
	else
	if((resp == AT_RESP_ERR_NO_RESP) || (resp == AT_RESP_ERR_DIF_RESP))
	{
		ret_val =FUN_STATE_ERROR;
	}
  	return (ret_val);
}
