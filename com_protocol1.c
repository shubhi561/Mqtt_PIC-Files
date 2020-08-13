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
* com_protocol1.c
*
* Interface module serial protocol for receiving data without start and end deilmeter.
*
*------------------------------------------------------------------------------
*/

/*
*------------------------------------------------------------------------------
* File				: com_protocol1.c
* Created by		: Sam
* Last changed by	: Sam
* Last changed		: 13/05/2020
*------------------------------------------------------------------------------
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

#include "../includes.h"

/*
*------------------------------------------------------------------------------
* Private Defines
*------------------------------------------------------------------------------
*/

typedef enum
{
	RX_STATE_IDLE1,
	RX_STATE_RECV_START1,
	RX_STATE_COMPLETE1
}XBEE_RECEIVER_STATE1;

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

RX_HANDLE1 	stRxHandle1;

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
* UINT8 IsRxFinished1(void)
*
* Summary	: Rx state machine
*
* Input		: None
*
* Output	: UINT8 -
*             RX_NOT_FINISHED1
*			  RX_TMOUT_ERR1
*			  RX_FINISHED1
*
*------------------------------------------------------------------------------
*/
UINT8 IsRxFinished1(void)
{
 	UINT8 num_of_bytes, current_bytes = 0;
 	UINT8 ret_val = RX_NOT_FINISHED1;
	RX_HANDLE1 *pstRxHandle1 = &stRxHandle1;

  	if(pstRxHandle1->mRxState == RX_NOT_STARTED1)
  	{
		/* still no character received => check timeout. */
		if((UINT32)(millis() - pstRxHandle1->mPrevCharInTime) >= pstRxHandle1->mStartReceptionTimeout)
		{
			/* timeout elapsed => GSM module didn't start with response
			* so communication is takes as finished
			*/
			pstRxHandle1->mCommBuffer[pstRxHandle1->mCommBufferLen] = 0x00;
			ret_val = RX_TMOUT_ERR1;
			//Print("Start char timeout\r");
		}
    }
  	if(pstRxHandle1->mRxState == RX_ALREADY_STARTED1)
  	{
      	/* finally check the inter-character timeout. */
    	if((UINT32)(millis() - pstRxHandle1->mPrevCharInTime) >= pstRxHandle1->mInterCharTimeout)
    	{
			/* timeout between received character was reached
		     * reception is finished.
		     */
    		//num_of_bytes = CirBuffAvailableBytes();
			num_of_bytes = pstRxHandle1->mRecvCounter;

	    	/* read all received bytes. */
	    	while (num_of_bytes)
	    	{
	    		num_of_bytes--;
	      		if(pstRxHandle1->mCommBufferLen < COMM_BUF1_LEN)
	      		{
	        		/* we have still place in the GSM internal comm. buffer =>
	        		 * move available bytes from circular buffer
	        		 * to the rx buffer.
	        		 */
					pstRxHandle1->mCommBuffer[pstRxHandle1->mCommBufferLen] = pstRxHandle1->mRecvBuffer[current_bytes];
					current_bytes++;
	        		pstRxHandle1->mCommBufferLen++;
					pstRxHandle1->mCommBuffer[pstRxHandle1->mCommBufferLen] = 0x00;
					/* and finish currently received characters
					 * so after each character we have
					 * valid string finished by the 0x00.
					 */
	      		}
	      		else
	      		{
					/* comm buffer is full, other incoming characters
					 * will be discarded
					 * but despite of we have no place for other characters
					 * we still must to wait until
					 * inter-character timeout is reached
					 * so just readout character from circular RS232 buffer
					 * to find out when communication id finished(no more characters
					 * are received in inter-INT8 timeout).
					 */
					//CirBuffRead();
	      		}
	    	}
			/* for sure finish string again
			* but it is not necessary.
			*/
      		pstRxHandle1->mCommBuffer[pstRxHandle1->mCommBufferLen] = 0x00;
      		ret_val = RX_FINISHED1;
    	}
  	}
  	return (ret_val);
}

/*
*------------------------------------------------------------------------------
* void CrlearRxBuff1(void)
*
* Summary	: Reset data receive handler function variables.
*
* Input		: None
*
* Output	: None
*------------------------------------------------------------------------------
*/
void CrlearRxBuff1(void)
{
	RX_HANDLE1 *pstRxHandle1 = &stRxHandle1;
	pstRxHandle1->mRecvCounter = 0;
	memset((INT8*)pstRxHandle1->mRecvBuffer, 0, COMM_BUF1_LEN);
	memset((INT8*)pstRxHandle1->mCommBuffer, 0, COMM_BUF1_LEN + 1);
}

/*
*------------------------------------------------------------------------------
* Public Functions
*------------------------------------------------------------------------------
*/

/*
*------------------------------------------------------------------------------
* void RxInit1(UINT32 start_comm_tmout, UINT32 max_interchar_tmout)
*
* Summary	: Initialize receive state machine.
*
* Input		: UINT32 start_comm_tmout - maximum waiting time for receiving the first response
*             character (in msec.).
*             UINT32 max_interchar_tmout - maximum tmout between incoming characters
*             in msec. if there is no other incoming character longer then specified
*             tmout(in msec) receiving process is considered as finished.
*
* Output	: None
*
*------------------------------------------------------------------------------
*/
void RxInit1(UINT32 start_comm_tmout, UINT32 max_interchar_tmout)
{
	RX_HANDLE1 *pstRxHandle1 = &stRxHandle1;

	pstRxHandle1->mRxHandle                = RX_STATE_IDLE1;
	pstRxHandle1->mRxState                 = RX_NOT_STARTED1;
	pstRxHandle1->mStartReceptionTimeout    = start_comm_tmout;
	pstRxHandle1->mInterCharTimeout         = max_interchar_tmout;
	pstRxHandle1->mPrevCharInTime 			= millis();
	pstRxHandle1->mCommBuffer[0] 			= 0x00; /* End of string. */
	pstRxHandle1->mCommBufferLen 			= 0;	/* Len set to zero. */
	CrlearRxBuff1();
}

/*
*------------------------------------------------------------------------------
* UINT8 IsStringReceived1(const rom CHAR8 *compare_string)
*
* Summary	: Method checks received bytes
*
* Input		: const rom CHAR8 *compare_string - pointer to the string which should be find
*
* Output	: 0 - string was NOT received
*        	  1 - string was received
*
*------------------------------------------------------------------------------
*/
UINT8 IsStringReceived1(const rom CHAR8 *compare_string)
{
	CHAR8 *ch;
	UINT8 ret_val = 0;
	RX_HANDLE1 *pstRxHandle1 = &stRxHandle1;

	if(pstRxHandle1->mCommBufferLen)
	{
		ch = strstrrampgm((const CHAR8*)pstRxHandle1->mCommBuffer, (const far rom CHAR8*)compare_string);
		if (ch != NULL)
		{
			ret_val = 1;
		}
		else
		{
			/* do nothing. */
		}
	}
	return (ret_val);
}

/*
*------------------------------------------------------------------------------
* UINT8 Rx1WaitResp(UINT16 start_comm_tmout, UINT16 max_interchar_tmout)
*
* Summary	: Wait for response.
*
* Input		: UINT16 start_comm_tmout - time out for response.
*             UINT16 max_interchar_tmout - inter char timeout once the comm starts.
*
* Output	: UINT8 -
*             RX_NOT_FINISHED
*			  RX_TMOUT_ERR
*			  RX_FINISHED
*
*------------------------------------------------------------------------------
*/
UINT8 Rx1WaitResp(UINT16 start_comm_tmout, UINT16 max_interchar_tmout)
{
	UINT8 status;
	status = IsRxFinished1();
  	return (status);
}

/*
*------------------------------------------------------------------------------
* void ReceiveHanlde1(UINT8 data)
*
* Summary	: receive handler for serial communication.
*
* Input		: None
*
* Output	: None
*------------------------------------------------------------------------------
*/
void ReceiveHanlde1(UINT8 data)
{
	RX_HANDLE1 *pstRxHandle1 = &stRxHandle1;

	pstRxHandle1->mRecvBuffer[pstRxHandle1->mRecvCounter] = data;
	pstRxHandle1->mRecvCounter++;
	if(RX_STATE_IDLE1 == pstRxHandle1->mRxHandle)
	{
		pstRxHandle1->mRxHandle = RX_STATE_RECV_START1;
		/* at least one character received => so init inter-character
		 * counting process again and go to the next state.
		 */
		pstRxHandle1->mPrevCharInTime = millis(); // init timeout for inter-character space
		pstRxHandle1->mRxState = RX_ALREADY_STARTED1;
	}
	else
	if(RX_STATE_RECV_START1 == pstRxHandle1->mRxHandle)
	{
		/* Reset inter char time out when every char receive. */
		pstRxHandle1->mPrevCharInTime = millis();
	}
}

/*
*------------------------------------------------------------------------------
* void InitializeReceiveInterface1(void)
*
* Summary	: Initializes receive interface state machine.
*
* Input		: None
*
* Output	: None
*
*------------------------------------------------------------------------------
*/
void InitializeReceiveInterface1(void)
{
	CrlearRxBuff1();
	/* With 9600 baud at startup for gsm. */
	InitializeUart2();    
}

/*
*  End of com_protocol1.c
*/