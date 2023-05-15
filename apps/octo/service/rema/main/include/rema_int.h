/*******************************************************************
File Description
********************************************************************/
/**
@file	  re_init.h
@brief

Description:  									\n
Module: 								\n

Copyright (c) 2013 HUMAX Co., Ltd.				\n
All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
*
*/
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



#ifndef	__HOM_INT_H__
#define	__HOM_INT_H__


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include "CWMP_CPE_API.h"
#include "CWMP_define.h"
#include "CWMP_STB_API.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

#ifndef CONFIG_DEBUG
#define CONFIG_SUPPORT_REMA_VALGRID //?�른곳으�??�전?�야??
#define REMA_LOG(...) REMA_TEMP_printf(__FUNCTION__, __LINE__, __VA_ARGS__);//추후 hlog�?변경예?

#define	RemLOG_Print				REMA_LOG
#define	RemLOG_Error				REMA_LOG
#define	RemLOG_Critical				REMA_LOG
#define	RemLOG_Warning				REMA_LOG
#define	RemLOG_Message				REMA_LOG
#define	RemLOG_Info					REMA_LOG
#define	RemLOG_Debug				REMA_LOG
#define	RemLOG_Trace				REMA_LOG

#define REM_FUNC_ENTER 				REMA_LOG("(++)Enter!!\n")
#define REM_FUNC_LEAVE				REMA_LOG("(--)Leave!!\n")

#define REM_ENTER 					
#define REM_LEAVE(x)				return x

#else

#define REMA_LOG(...) REMA_TEMP_printf(__FUNCTION__, __LINE__, __VA_ARGS__);//추후 hlog�?변경예?


#define	RemLOG_Print				HxLOG_Print
//#define	RemLOG_Print				REMA_LOG
#define	RemLOG_Error				HxLOG_Error
#define	RemLOG_Critical				HxLOG_Critical
#define	RemLOG_Warning				HxLOG_Warning
#define	RemLOG_Message				HxLOG_Message
#define	RemLOG_Info				HxLOG_Info
#define	RemLOG_Debug				HxLOG_Debug
#define	RemLOG_Trace				HxLOG_Trace

#define REM_FUNC_ENTER 				REMA_LOG("(++)Enter!!\n")
#define REM_FUNC_LEAVE				REMA_LOG("(--)Leave!!\n")

#define REM_ENTER 				HxLOG_Debug("[%s():%d] Enter!!\n",__FUNCTION__,__LINE__)
#define REM_LEAVE(x)			HxLOG_Debug("[%s():%d] Leave!!\n",__FUNCTION__,__LINE__);return x

#endif

#define  HCWMP_GETTER_PARAM		void *pvCtx, int nPDid, const char *pszParamName, hCwmpParamType_e eParamType, char *pszBuf, int nBufSize, int *pnOutLen
#define  HCWMP_SETTER_PARAM		void *pvCtx, int nPDid, const char *pszParamName, hCwmpParamType_e eParamType, const char *pszValue, size_t unSize
#define  HCWMP_UPDATER_PARAM		void *pvCtx, hCwmpDMManager_t *pstDMManager, int nPd, const char *pszObjectName
#define  HCWMP_ADDER_PARAM		void *pvCtx, hCwmpDMManager_t *pstDMManager, int nPd, const char *pszObjectName, unsigned int *pAddedNum
#define  HCWMP_DELETER_PARAM		void *pvCtx, hCwmpDMManager_t *pstDMManager, int nPd, const char *pszObjectName


#define REM_STR_CHECKNULL(a)		(!a) ? "ok": "NULL"

#ifdef CONFIG_DEBUG
#define PRINT_JSON(json) \
	{	HCHAR *pString;	pString = HLIB_JSON_Encode(json, NULL, 0);	HxLOG_Debug("json:[%s]\n",pString);	HLIB_STD_MemFree(pString);}
#else
#define PRINT_JSON(json)
#endif

#define SIZE_16			17
#define SIZE_32			33
#define SIZE_64			65
#define SIZE_128		129
#define SIZE_256		257
#define SIZE_512		512
#define SIZE_1024	1024

#define	CHECK_NULL(a)		{if (a == NULL) {RemLOG_Critical("Null Pointer\n"); REM_LEAVE(ERR_FAIL);}}
#define PARAM_ATOI(variable)		((variable != NULL) ? atoi(variable) : 0)


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
static inline void REMA_TEMP_printf(const char *func, int line,  const char *fmt, ...)
{
    va_list ap;
    printf("[%s]@%d : ", func, line);
    //printf("[%s]@%d : ", func, line);

    va_start(ap, fmt);
    vprintf(fmt, ap );
    va_end(ap);
    //printf("\n");
};
/*******************************************************************/
/********************      extern Functions     ********************/
/*******************************************************************/
#endif
