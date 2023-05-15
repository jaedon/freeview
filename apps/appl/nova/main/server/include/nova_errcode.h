/**************************************************************
*	http://www.humaxdigital.com
*	@author			humax
**************************************************************/


/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ¨Ï 2011-2012 Humax Co., Ltd.
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
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/



#ifndef __NOVA_ERRCODE_H__
#define __NOVA_ERRCODE_H__


typedef enum
{
	// General ErrCode
	NERR_OK = 0,
	NERR_ERROR					= 10000,
	NERR_INVALIDARGS,
	NERR_INIT,
	NERR_DEINIT,
	NERR_INVALIDRESULT,
	NERR_NORESOURCE,
	NERR_NOTEXIST,
	NERR_ALREADY_EXIST,
	NERR_TIMEOUT,
	NERR_NULLPOINTER,
	NERR_OUTOFMEMORY,
	NERR_NOTALLOWED,
	NERR_CREATE,
	NERR_DESTROY,	
	NERR_OPEN,
	NERR_READ,
	NERR_WRITE,
	NERR_CLOSE,
	NERR_START,
	NERR_STOP,
	NERR_UNSUPPORT,
	NERR_NOTIMPLEMENTED,

	// TUNER ErrCode
	NERR_TUNER_INVALID_TUNERID	= 20000,
	NERR_TUNER_CONFLICT,
	NERR_TUNER_NOTLOCKED,
	NERR_TUNER_ALLOCATE,
	NERR_TUNER_RELEASE,
	NERR_TUNER_START,
	NERR_TUNER_STOP,
	NERR_TUNER_LOADANTENNA,

	// SAPI ErrCode
	NERR_SAPI_REGISTER_NOFIFIER = 30000,
	NERR_SAPI_UNREGISTER_NOFIFIER,
	NERR_SAPI_MAKESCHEDULE,
	NERR_SAPI_CANCELSCHEDULE,
	NERR_SAPI_UPDATESCHEDULE,
	NERR_SAPI_GETSCHEDULE,
	NERR_SAPI_GETAVAILABLESLOT,
	NERR_SAPI_INVALID_SLOTID,

	// OAPI ErrCode
	NERR_OAPI_INIT				= 40000,
	NERR_OAPI_DEINIT,
	NERR_OAPI_ISLOCKED,
	NERR_OAPI_GETSIGNALINFO,


	// RINGBUF ErrCode
	NERR_RINGBUF_CREATE			= 50000,
	NERR_RINGBUF_WRITE,
	NERR_RINGBUF_READ,
	NERR_RINGBUF_GETSTATUS,
	NERR_RINGBUF_DESTROY,
	NERR_RINGBUF_CLEAR,
	NERR_RINGBUF_INVALIDHANLDE,

	NERR_MAX
} NOVA_ErrCode_e;


#endif
