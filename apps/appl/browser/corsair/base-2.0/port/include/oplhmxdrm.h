/*
 * (c) 2011-2012 Humax Co., Ltd. 
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
/**************************************************************************************/
/**
 * @file browser_hmxdrm.h
 * defines APIs's header
 *
 * APIs for Humax DRM
 *
 * @author  Jongwon Kim(jwkim@humaxdigital.com)
 * @date    2012/02/08
 * @attention   Copyright (c) 2012 Humax - All rights reserved.
 * @note		dhkim2가 phxx에서 hmxdrm으로 변경.
**************************************************************************************
**/

#ifndef __OPL_HMXDRM_H__
#define __OPL_HMXDRM_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum 
{
	eOPL_HMXDRM_SYSTEM_STATUS_READY = 0, 		/**< The DRM system is fully initialised and ready. */
	eOPL_HMXDRM_SYSTEM_STATUS_UNKNOWN,			/**< Unknown DRM system. */
	eOPL_HMXDRM_SYSTEM_STATUS_INITIALISING,		/**< The DRM system is initialising and not ready to start communicating with the application. */
	eOPL_HMXDRM_SYSTEM_STATUS_ERROR				/**< There is a problem with the DRM system. It may be possible to communicate with it to obtain more information. */
} eOPL_HMXDRM_SYSTEM_STATUS_t;

typedef enum {
	eOPL_HMXDRM_ERR_OK							= 0,
	eOPL_HMXDRM_ERR_FAIL,
	eOPL_HMXDRM_ERR_UNDEFINED,
	eOPL_HMXDRM_ERR_NOT_SUPPORTED
} eOPL_HMXDRM_Error;


eOPL_HMXDRM_Error OPL_HMXDRM_Set_VMX_Cas_Info(const char *pszCasUrl, const char *pszVendorName);

eOPL_HMXDRM_Error OPL_HMXDRM_Set_Playready_Drm(const char *pszLaUrl, const char *pszDeviceId, int pszControlFlag);

eOPL_HMXDRM_Error OPL_HMXDRM_Get_VMX_Identifier(char **pszIdentifier);

eOPL_HMXDRM_Error OPL_HMXDRM_Drm_System_Status(int *pnStatus);

eOPL_HMXDRM_Error OPL_HMXDRM_Drm_Rights_Error(char **pszErrStatus);

eOPL_HMXDRM_Error OPL_HMXDRM_Unset_VMX_Cas_Info(void);

#ifdef __cplusplus
}
#endif

#endif
