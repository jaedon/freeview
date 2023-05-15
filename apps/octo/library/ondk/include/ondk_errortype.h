
/**************************************************************
 *	@file		mlib.h
 *	Minimized Graphic Library for DirectFB
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2012/03/07
 *	@author			humax
 *	@brief			Minimized Graphic Library for DirectFB
 **************************************************************/


/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ⓒ 2011-2012 Humax Co., Ltd.
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


#ifndef ____ONDK_ERRORTYPE_H____
#define ____ONDK_ERRORTYPE_H____


typedef	enum _ONDK_Result
{
	ONDK_RET_OK = 0x00000000,		/* No error occured. */
	ONDK_RET_FAIL,					/* A general or unknown error occured. */
	ONDK_RET_INITFAIL,				/* A general initialization error occured. */
	ONDK_RET_DUPINIT,				/* A general duplication initialization error occured. */
	ONDK_RET_EOF,            		/* Reached end of file. */
	ONDK_RET_SUSPENDED,       		/* The requested object is suspended. */
	ONDK_RET_DESTROYED,       		/* The requested object has been destroyed. */
	ONDK_RET_BUSY,         			/* The resource or device is busy. */
	ONDK_RET_IO,              		/* A general I/O error occured. */
	ONDK_RET_INVARG,          		/* An invalid argument has been specified. */
	ONDK_RET_UNSUPPORTED,     		/* The requested operation or an argument is (currently) not supported. */
	ONDK_RET_TIMEOUT,         		/* The operation timed out. */
	ONDK_RET_NOLOCALMEMORY,   		/* There's not enough local system memory. */


	/* OAPI or HAPI or DAPI... Message CONTORL */
	ONDK_MESSAGE_OK					= ONDK_RET_OK,
	ONDK_MESSAGE_BREAK				= 0x01000000,
	ONDK_MESSAGE_PASS,
	ONDK_MESSAGE_CONSUMED,
	ONDK_MESSAGE_NONCONSUMED,


	/* GWM... Message CONTORL */
	GWM_MESSAGE_BREAK				= ONDK_MESSAGE_BREAK,
	GWM_MESSAGE_PASS				= ONDK_MESSAGE_PASS,

	/* GWM Error enums */
	GWM_RET_OK						= ONDK_RET_OK,
	GWM_RET_FAIL					= ONDK_RET_FAIL,
	GWM_RET_INITIAL_FAILED 			= ONDK_RET_INITFAIL,
	GWM_RET_NO_OBJECT				= 0x03000000,
	GWM_RET_LINK_CONFLICT,

	GWM_RET_Q_UNDERFLOW,			/* QUEUE Module */
	GWM_RET_OVERFLOW,

	GWM_RET_OUT_OF_RANGE,			/* Object Module */
	GWM_RET_OUT_OF_MEMORY,

	// argument 관련 error들
	GWM_RET_SOURCE_NULL,
	GWM_RET_TARGET_NULL,


	GWM_RET_CANNOT_SHOW,
	GWM_RET_CANNOT_HIDE,
	GWM_RET_ALREADY_SHOW,
	GWM_RET_ALREADY_HIDE,
	GWM_RET_DONT_NEED_TO_DRAW,

	GWT_RET_ALREADY_START,
	GWT_RET_ALREADY_STOP,

	// List에 필요한 Error Msg
	GWT_RET_LIST_NOMORE_MOVE,
	GWT_RET_LIST_NOENABLE_ITEM,

	GWT_RET_NO_FOCUS_OBJECT,	// set focus error, object not exist
	GWT_RET_DONOT_INITIATED,

	GWT_RET_MAX
} ONDK_Result_t;


#endif





