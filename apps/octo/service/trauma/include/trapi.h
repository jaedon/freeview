
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

#ifndef	__TRAPI__
#define	__TRAPI__

/* *************************************************************************************************** */
#define ___DEFINE________________________________________________________________________________________
/* *************************************************************************************************** */
#ifdef CONFIG_TRAUMA_ERIN
	#define TRAPI_ERIN TRAPI_GetLog
#else
	#ifdef CONFIG_DEBUG
		#define TRAPI_ERIN HLIB_LOG_ErinPrint
	#else
		#define	TRAPI_ERIN while (0) ((int (*)(const char *, ...)) 0)
	#endif
#endif

/* *************************************************************************************************** */
#define ___TYPEDEF_______________________________________________________________________________________
/* *************************************************************************************************** */
typedef enum
{
	eTRAPI_ERIN_START 	= 0,
	eTRAPI_ERIN_TEST_1	= 0x01010001, /* will be delete.. */
	eTRAPI_ERIN_TEST_2	= 0x01010002, /* will be delete.. */
	eTRAPI_ERIN_MAX,
}TRAPI_ERIN_e;

/* *************************************************************************************************** */
#define ___FUNCTION_HEADER_______________________________________________________________________________
/* *************************************************************************************************** */
#define	TxERIN_Print(err_code, ...)		TRAPI_ERIN(__HxFILE__, __FUNCTION__, __HxLINE__, HxLOG_PRINT, err_code, __VA_ARGS__)
#define	TxERIN_Error(err_code,...)		TRAPI_ERIN(__HxFILE__, __FUNCTION__, __HxLINE__, HxLOG_ERROR, err_code, __VA_ARGS__)
#define	TxERIN_Critical(err_code,...)	TRAPI_ERIN(__HxFILE__, __FUNCTION__, __HxLINE__, HxLOG_CRITICAL, err_code, __VA_ARGS__)
#define	TxERIN_Warning(err_code,...)	TRAPI_ERIN(__HxFILE__, __FUNCTION__, __HxLINE__, HxLOG_WARNING, err_code,__VA_ARGS__)
#define	TxERIN_Message(err_code,...)	TRAPI_ERIN(__HxFILE__, __FUNCTION__, __HxLINE__, HxLOG_MESSAGE, err_code, __VA_ARGS__)
#define	TxERIN_Info(err_code,...)		TRAPI_ERIN(__HxFILE__, __FUNCTION__, __HxLINE__, HxLOG_INFO, err_code, __VA_ARGS__)
#define	TxERIN_Debug(err_code,...)		TRAPI_ERIN(__HxFILE__, __FUNCTION__, __HxLINE__, HxLOG_DEBUG, err_code, __VA_ARGS__)
#define	TxERIN_Trace(err_code)			TRAPI_ERIN(__HxFILE__, __FUNCTION__, __HxLINE__, HxLOG_TRACE, err_code, "\tTR[%s:%d:%s]\n", __FUNCTION__, __HxLINE__, __HxFILE__)

void	TRAPI_GetLog(const HCHAR *pszFile, const HCHAR *pszFunc, HUINT32 ulLine, HxLOG_Level_e eLev,HUINT32 ulErr_code, const HCHAR *pszFormat, ...);

#ifdef CONFIG_TRAUMA_PROCESS
void 	TRAPI_GetAcceptPrivacy(HBOOL *pbAccept);
void 	TRAPI_SetAcceptPrivacy(HBOOL bAccept); 	// and try device login
void	TRAPI_StatisticsLog(HCHAR *pszData);	// example : "CODE":"SSTART","SW_VER":"1234","ETC":"TEST""
#else
void 	TRAPI_GetAcceptPrivacy(HBOOL *pbAccept) { HxLOG_Warning("CONFIG_TRAUMA_PROCESS = N\n");}
void 	TRAPI_SetAcceptPrivacy(HBOOL bAccept)	{ HxLOG_Warning("CONFIG_TRAUMA_PROCESS = N\n");}
void	TRAPI_StatisticsLog(HCHAR *pszData)		{ HxLOG_Warning("CONFIG_TRAUMA_PROCESS = N\n");}

#endif
/* *************************************************************************************************** */
#define ___DEBUG_FUNCTION_HEADER_________________________________________________________________________
/* *************************************************************************************************** */

#endif //__TRAPI__
