/*
 * (c) 2011-2013 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

/******************************************************************************/
/**
 * @file	  		nx_prism_util_na.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/


/******************************************************************************/
#define _________Private_Include______________________________________________
/******************************************************************************/
#include <nx_common.h>
#include <nx_core.h>
#include <nx_prism_app.h>
#include <nx_prism_util.h>
#include <nx_prism_util_na.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/

HINT8		*NX_PRISM_UTIL_NA_GetResStringIdByErrorType(NX_CasNaMessage_Type eNaMessage)
{
	NX_APP_Debug("[Debug] NA CAS Error Message:(0x%x)\n", eNaMessage);

	switch (eNaMessage)
	{
		case eNxCasNaMessage_Type_None:								return	NULL;
		case eNxCasNaMessage_SMARTCARD_OK:							return	NULL;

	//reated by smartcard
		case eNxCasNaMessage_SMARTCARD_COM_ERROR:					return	RES_NA_MSG_SMARTCARD_COM_ERROR_ID;
		case eNxCasNaMessage_SMARTCARD_MUTE:						return	RES_NA_MSG_SMARTCARD_MUTE_ID;
		case eNxCasNaMessage_SMARTCARD_INVALID:						return	RES_NA_MSG_SMARTCARD_INVALID_ID;
		case eNxCasNaMessage_SMARTCARD_BLACKLISTED:					return	RES_NA_MSG_SMARTCARD_BLACKLISTED_ID;
		case eNxCasNaMessage_SMARTCARD_SUSPENDED:					return	RES_NA_MSG_SMARTCARD_SUSPENDED_ID;
		case eNxCasNaMessage_SMARTCARD_NEVER_PAIRED:				return	RES_NA_MSG_SMARTCARD_NEVER_PAIRED_ID;
		case eNxCasNaMessage_SMARTCARD_NOT_PAIRED:					return	RES_NA_MSG_SMARTCARD_NOT_PAIRED_ID;
		case eNxCasNaMessage_SMARTCARD_EXPIRED:						return	RES_NA_MSG_SMARTCARD_EXPIRED_ID;
		case eNxCasNaMessage_SMARTCARD_NOT_CERTIFIED:				return	RES_NA_MSG_SMARTCARD_NOT_CERTIFIED_ID;
		case eNxCasNaMessage_SMARTCARD_REMOVED:						return	RES_NA_MSG_SMARTCARD_REMOVED_ID;

	//reated by access
		case eNxCasNaMessage_ACCESS_FREE:							return	NULL;
		case eNxCasNaMessage_ACCESS_GRANTED:						return	NULL;
		case eNxCasNaMessage_ACCESS_GRANTED_FOR_PREVIEW:			return	NULL;
		case eNxCasNaMessage_ACCESS_GRANTED_BUT_WARNING_PREVIEW:	return	NULL;
		case eNxCasNaMessage_ACCESS_GRANTED_PPT:					return	NULL;
		case eNxCasNaMessage_ACCESS_DENIED:							return	RES_NA_MSG_ACCESS_DENIED_ID;
		case eNxCasNaMessage_ACCESS_NO_VALID_SMARTCARD:				return	RES_NA_MSG_ACCESS_NO_VALID_SMARTCARD_ID;
		case eNxCasNaMessage_ACCESS_DENIED_BUT_PREVIEW:				return	NULL;
		case eNxCasNaMessage_ACCESS_BLACKOUT:						return	RES_NA_MSG_ACCESS_BLACKOUT_ID;
		case eNxCasNaMessage_ACCESS_DENIED_BUT_PPT:					return	NULL;
		case eNxCasNaMessage_ACCESS_DENIED_NO_VALID_CREDIT:			return	RES_NA_MSG_ACCESS_DENIED_NO_VALID_CREDIT_ID;
		case eNxCasNaMessage_ACCESS_DENIED_FOR_PARENTAL_RATING:		return	NULL;
		case eNxCasNaMessage_ACCESS_DENIED_COPY_PROTECTED:			return	RES_NA_MSG_ACCESS_DENIED_COPY_PROTECTED_ID;
		case eNxCasNaMessage_ACCESS_DENIED_PARENTAL_CONTROL:		return	NULL;
		case eNxCasNaMessage_ACCESS_DENIED_DIALOG_REQUIRED:			return	RES_NA_MSG_ACCESS_DENIED_DIALOG_REQUIRED_ID;
		case eNxCasNaMessage_ACCESS_DENIED_PAIRING_REQUIRED:		return	RES_NA_MSG_ACCESS_DENIED_PAIRING_REQUIRED_ID;
		case eNxCasNaMessage_ACCESS_DENIED_CS_PAIRING_REQUIRED:		return	RES_NA_MSG_ACCESS_DENIED_CS_PAIRING_REQUIRED_ID;

		default:
			NX_APP_Error("Error!!! Invalid Error Message:(0x%x)\n", eNaMessage);
			return	NULL;
	}

}


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

