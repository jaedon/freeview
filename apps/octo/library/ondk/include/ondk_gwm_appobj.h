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

#ifndef	__GWM_APPOBJ_H__
#define	__GWM_APPOBJ_H__

#include <ondk_gwm.h>
/* ======================================================================== */
/* Global/Extern Variables.																			*/
/* ======================================================================== */

#define		GWM_APP_FAKE_MASK	0x00000001
#define		GWM_APP_HIDDEN_MASK		0x00000002
#define		GWM_APP_UI_ATTR_MASK		0x00000004

/* ======================================================================== */
/* Private Constants. 																				*/
/* ======================================================================== */


/* ======================================================================== */
/* Private Types.																					*/
/* ======================================================================== */


/* ======================================================================== */
/* Private Variables. 																				*/
/* ======================================================================== */

/* ======================================================================== */
/* GWM private function prototypes. 																	*/
/* ======================================================================== */
extern ONDK_GWM_App_t			ONDK_GWM_APP_GetNextApp(ONDK_GWM_App_t app);
extern ONDK_Result_t			ONDK_GWM_APP_Call(ONDK_GWM_App_t app, int message, Handle_t handle, int param1, int param2, int param3, void *pvData);
extern void				    ONDK_GWM_APP_DisableDimMode(void);

extern HUINT32				    ONDK_GWM_APP_CountAppByUiState (GWM_UiState_t eGwmUiState);
extern ONDK_GWM_App_t			ONDK_GWM_APP_GetAppByUiState (GWM_UiState_t eGwmUiState, HUINT32 ulIndex);
extern void				    ONDK_GWM_APP_SetUiState (GWM_UiState_t eGwmUiState);
extern HERROR				    ONDK_GWM_APP_GetUiState (ONDK_GWM_Callback_t pfCallback, GWM_UiState_t *peGwmUiState);

#endif	//__GWM_APPOBJ_H__

