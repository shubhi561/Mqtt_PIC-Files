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
* Include file for gsm/gprs communication interface module.
*
*------------------------------------------------------------------------------
*/

#ifndef GSM_GPRS_INTERFACE_H
#define GSM_GPRS_INTERFACE_H

/*
*------------------------------------------------------------------------------
* File				: gsm_interface.h
* Created by		: Sam
* Last changed by	: Sam
* Last changed		: 20/05/2020
*------------------------------------------------------------------------------
*
* Revision 1.1 20/05/2020 Sam
* Modified for the current project
* Revision 1.0 07/10/2013 Sam
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

enum AT_RESP_ENUM
{
	AT_RESP_IN_PROG = 0,   /* response reception in progress. */
	AT_RESP_ERR_NO_RESP,   /* nothing received. */
	AT_RESP_ERR_DIF_RESP,   /* response_string is different from the response. */
	AT_RESP_OK,             /* response_string was included in the response. */
	AT_RESP_LAST_ITEM
};

/*
*------------------------------------------------------------------------------
* Public Macros
*------------------------------------------------------------------------------
*/

/* Ascii character for ctr+z. End of a SMS.*/
#define 	CTRL_Z 						(26)
/* Ascii character for carriage return. */
#define 	CR    						(13)
/* Ascii character for line feed. */
#define 	LF    						(10)

/* some constants for the IsRxFinished() method. */
#define 	RX_NOT_STARTED      		(0)
#define 	RX_ALREADY_STARTED  		(1)
#define 	MAX_GSM_STRING_SIZE			(UINT8)(250)

/* SIM network registration time out, wait till the time out for network registration. if fail to register to network restart the modem. */
#define 	SIM_NETWOR_REG_TIME_OUT			(UINT32)((UINT32)1000 * (UINT32)60 * (UINT32)30)	/* 30Min. */

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

extern INT8 tempGsmBuff[MAX_GSM_STRING_SIZE];

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
extern void InitializeGSMCommunication(void);
extern void GsmCommunicationTask(void);
extern INT8 SendATCmdWaitResp(const INT8 *AT_cmd_string,
                UINT16 start_comm_tmout,
				UINT16 max_interchar_tmout,
                const rom CHAR8 *response_string,
                UINT8 no_of_attempts);
INT8 SendATCmdWaitResp1(const INT8 *AT_cmd_string,
                UINT16 start_comm_tmout, UINT16 max_interchar_tmout,
                const rom INT8 *response_string,
                UINT8 no_of_attempts, UINT16 len);
extern UINT8 GetSMS(UINT8 position, CHAR8 *phone_number, CHAR8 *SMS_text, CHAR8 *sms_status, UINT8 max_SMS_len);
extern INT8 CheckForSimRegistration(void);
extern INT8 CheckForSimReady(void);
extern INT8 SetEchoOff(void);

#endif
/*
*  End of gsm_interface.h
*/



