/**************************************************************
 *	@file		ondk.h
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

#ifndef __ONDK_STRINGS_H__
#define __ONDK_STRINGS_H__

typedef void	*(*DEF_CB_GetProcId)	(void);

typedef struct tagONDKSTR_Settings_t
{
	DEF_CB_GetProcId 	cbGetProcId;
	HINT32				nProcMaxCount;

} ONDKSTR_Settings_t;



extern	ONDK_Result_t ONDK_SetResourceFile(HUINT8 *valuelistFile);
extern 	ONDK_Result_t ONDK_SetLanguage(HINT8 *language);
//extern 	HUINT8 *ONDK_GetStringReLoad(HINT8 *str_id);
extern 	ONDK_Result_t  ONDK_STRINGS_Init(void);
extern	ONDK_Result_t  ONDK_STRINGS_Settings(ONDKSTR_Settings_t *pSettings);

extern 	HUINT8 *ONDK_GetString(HINT8 *str_id);
extern 	void	ONDK_ReleaseString(HINT8 *str_id);
extern	void 	ONDK_ReleaseStringByProcId(HUINT32 procId);
extern	void 	DBG_ONDK_STRINGS_Dump(HUINT32 procId);

#endif /* __ONDK_STRINGS_H__ */

