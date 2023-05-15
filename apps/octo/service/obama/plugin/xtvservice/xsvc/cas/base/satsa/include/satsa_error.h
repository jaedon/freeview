/* $Header:   //BASE/archives/Group_MHP/Premium_MHP/mw/cas/satsa/include/satsa_error.h-arc   1.0   Jan 22 2007 11:42:12   kimys  $ */
/******************************************************************************
 * $Workfile:   satsa_error.h  $
 * $Modtime:   Jan 22 2007 10:10:54  $
 * Author : ChangHyun Lim(POOH)
 * description : SATSA interface
 *
 * Copyright (c) 2006 by Humax Co., Ltd.
 * All right reserved
 ******************************************************************************/

#ifndef _SATSA_ERROR_H_
#define _SATSA_ERROR_H_

/********************************************************************
 Header Files
 ********************************************************************/
#define SATSA_SC_INVALID_HANDLE	-1
/********************************************************************
 Constant/Macro Definition
 ********************************************************************/
typedef enum
{
  SATSA_NO_ERROR,
	 /**< Success.
	 */
  SATSA_ERROR_SESSION_ID,
	 /**< The session identifier is wrong.
	 */
  SATSA_ERROR_CARD_REMOVED,
	 /**< There is no smartcard to communicate with.
	 */
  SATSA_ERROR_CARD_MUTE,
	 /**< The smartcard does not send any data. May be upside down.
	 */
  SATSA_ERROR_TIMEOUT,
	 /**< The smartcard didn't answer in the required time.
	 */
  SATSA_ERROR_MODE,
	 /**< The operation requested cannot be performed due to a mode
	  *   constraint.
	 */
  SATSA_ERROR_CONFLICT,
	 /**< The smartcard mode cannot be changed, as it would lead to
	  *   a mode conflict.
	 */
  SATSA_ERROR
	 /**< Other error.
	 */
} SATSA_ERROR_CODE;

/********************************************************************
 Structure and Type Definition
 ********************************************************************/

/********************************************************************
 Global Variable
 ********************************************************************/

/********************************************************************
 Global Function
 ********************************************************************/

#endif /* _SATSA_ERROR_H_ */


