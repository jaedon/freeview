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
/*******************************************************************
	File Description
********************************************************************/
/**
@file				cxcore_pvr.h
@brief			cxcore_pvr.h

	Description:   \n
	Module: MW/CAS \n

	Copyright (c) 2009 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

#ifndef	__CXCORE_PVR_H__
#define	__CXCORE_PVR_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables 	********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition 	********************/
/*******************************************************************/
#define CONAX_PVR_KEY_SIZE				PAL_PVR_CRYPTO_KEY_LEN


/*******************************************************************/
/********************      Typedef  	   *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions 	********************/
/*******************************************************************/
HERROR CXPVR_InitCore(void);

HERROR CXAPI_PVR_ReadRootKey(void);
HERROR CXAPI_PVR_GenerateRootKey(void);
HERROR CXAPI_PVR_ReadPvrKey(HUINT8* pucPvrKey);
HERROR CXAPI_PVR_UpdateFingerprintToMetaFile(Handle_t hAction, CxFingerprint_t* pstCxFingerprint);
HERROR CXAPI_PVR_UpdateUriToMetaFile(Handle_t hAction, DxCopyrightControl_ConaxDrmInfo_t* pstDrmInfo);

HERROR CXAPI_PVR_Pb_GetSessionKey(Handle_t hAction, HUINT8* pucPbKey);
HERROR CXAPI_PVR_Rec_GetSessionKey(Handle_t hAction, HUINT8* pucRecKey);

HERROR CXAPI_PVR_ScInserted(Handle_t hSession);

HERROR CXPVR_GetMaturityRate(Handle_t hAction, CxMaturityRatingValue_t *pstMaturityRate);
HERROR CXPVR_SetMaturityRate(Handle_t hAction, CxMaturityRatingValue_t *pstMaturityRate);

#if defined(CONFIG_MW_CAS_CONAX_MR_RESERVATION)
HERROR CXAPI_PVR_ReadCaPin(void);
#endif

// API function (which can be invoked from outside of the conax core)

#endif /* !__CXCORE_STREAM_H__ */
