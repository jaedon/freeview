
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

#ifndef	__TRAPI_INT__
#define	__TRAPI_INT__

/* *************************************************************************************************** */
#define ___HEADER________________________________________________________________________________________
/* *************************************************************************************************** */
#include <hlib.h>
/* *************************************************************************************************** */
#define ___DEFINE________________________________________________________________________________________
/* *************************************************************************************************** */
#define TRAPI_PROCESSNAME_LEN 	64
#define TRAPI_FUNC_LEN 		 	256
#define TRAPI_FILEPATH_LEN 	 	512
#define TRAPI_LOG_LEN 		 	512

#define TRAPI_RPC_ERIN					"trapi.erin"
#define TRAPI_RPC_SEND_ACCEPT_PRIVACY	"trapi.AcceptPrivacy"
#define TRAPI_RPC_SEND_STATISTICS_LOG	"trapi.StatisticsLog"
#define TRAUMA_RPC_onResponse			"trauma.onResponse"
#define TRAUMA_DB_ACCEPT_PRIVACY 		"accept_privacy"

/* *************************************************************************************************** */
#define ___TYPEDEF_______________________________________________________________________________________
/* *************************************************************************************************** */
typedef struct{
	HUINT32			ulLine;
	HxLOG_Level_e	eLevel;
	UNIXTIME		ulSystime;
	HUINT32			ulErr_code;
	HCHAR			szProcess[TRAPI_PROCESSNAME_LEN];
	HCHAR 			szFilePath[TRAPI_FILEPATH_LEN];
	HCHAR 			szFunction[TRAPI_FUNC_LEN];
	HCHAR			szLog[TRAPI_LOG_LEN];
}TRAPI_ERIN_t;

/* *************************************************************************************************** */
#define ___FUNCTION_HEADER_______________________________________________________________________________
/* *************************************************************************************************** */
void	trapi_rpc_erin_send(TRAPI_ERIN_t *pData);
void 	trapi_rpc_acceptprivacy_send(HBOOL bAccept);
void 	trapi_rpc_statisticsLog_send(HCHAR *pszLog);

void 	trapi_rpc_addListener(HINT32 nRequestId,void *pvUser,void* onResponse);

/* *************************************************************************************************** */
#define ___DEBUG_FUNCTION_HEADER_________________________________________________________________________
/* *************************************************************************************************** */

#endif //__TRAPI_INT__
