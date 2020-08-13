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
* com_protocol1.h
*
* Include file for com_protocol1 module.
*
*------------------------------------------------------------------------------
*/

#ifndef COM_PROTOCOL1
#define COM_PROTOCOL1

/*
*------------------------------------------------------------------------------
* File				: com_protocol1.h
* Created by		: Sam
* Last changed by	: Sam
* Last changed		: 13/05/2020
*------------------------------------------------------------------------------
*
* Revision 1.1 13/05/2020 Sam
* Updated as geniric protocol
* Revision 1.0 10/04/2015 Sam
* Initial revision
*
*------------------------------------------------------------------------------
*/

/*
*------------------------------------------------------------------------------
* Include Files
*------------------------------------------------------------------------------
*/

/*
*------------------------------------------------------------------------------
* Public Defines
*------------------------------------------------------------------------------
*/

enum RX_STATE_ENUM1
{
	RX_NOT_FINISHED1 = 0,      	/* not finished yet. */
	RX_FINISHED1,             	/* finished, some character was received. */
	RX_FINISHED_STR_RECV1,     	/* finished and expected string received. */
	RX_FINISHED_STR_NOT_RECV1, 	/* finished, but expected string not received. */
	RX_TMOUT_ERR1,            	/* finished, no character received. */
								/* initial communication timeout occurred. */
	RX_LAST_ITEM1
};

/*
*------------------------------------------------------------------------------
* Public Macros
*------------------------------------------------------------------------------
*/

/* length for the internal communication buffer. */
#define 	COMM_BUF1_LEN        		(UINT16)(50)

/* some constants for the IsRxFinished() method. */
#define 	RX_NOT_STARTED1      		(UINT8)(0)
#define 	RX_ALREADY_STARTED1  		(UINT8)(1)

/*
*------------------------------------------------------------------------------
* Public Data Types
*------------------------------------------------------------------------------
*/

typedef struct
{
	UINT8 mRecvCounter;
	UINT8 mPassSendData;
	/* internal state of rx state machine. */
	UINT8 mRxState;
	UINT8 mRxHandle;
	/* num. of characters in the buffer. */
	UINT8 mCommBufferLen;
	/* Max timeout for starting reception in msec. */
	UINT32 mStartReceptionTimeout;
	/* Holds inter char time out value in msec. */
	UINT32 mInterCharTimeout;
	/* Previous char received time in msec. */
	UINT32 mPrevCharInTime;
	/* serial data receive buffer. */
	UINT8 mRecvBuffer[COMM_BUF1_LEN];
	/* communication buffer +1 for 0x00 termination. */
	CHAR8 mCommBuffer[COMM_BUF1_LEN + 1];
}RX_HANDLE1;


/*
*------------------------------------------------------------------------------
* Public Variables (extern)
*------------------------------------------------------------------------------
*/

extern RX_HANDLE1 	stRxHandle1;

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

extern void InitializeReceiveInterface1(void);
extern UINT8 IsRxFinished1(void);
extern void ReceiveHanlde1(UINT8 data);
extern void RxInit1(UINT32 start_comm_tmout, UINT32 max_interchar_tmout);
extern UINT8 IsStringReceived1(const rom CHAR8 *compare_string);
extern UINT8 Rx1WaitResp(UINT16 start_comm_tmout, UINT16 max_interchar_tmout);
extern void CrlearRxBuff1(void);


#endif
/*
*  End of com_protocol1.h
*/