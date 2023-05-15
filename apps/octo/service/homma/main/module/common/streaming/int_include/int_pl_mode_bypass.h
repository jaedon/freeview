
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


#ifndef	__INT_PL_STREAMING_MODE_BYPASS__
#define	__INT_PL_STREAMING_MODE_BYPASS__



/* *************************************************************************************************** */
#define ___DEFINE________________________________________________________________________________________
/* *************************************************************************************************** */



/* *************************************************************************************************** */
#define ___TYPEDEF_______________________________________________________________________________________
/* *************************************************************************************************** */



/* *************************************************************************************************** */
#define ___FUNCTION_HEADER_______________________________________________________________________________
/* *************************************************************************************************** */
extern	HOM_Result_e 			PL_Streaming_ModeByPass_Init(void);
extern	void 					PL_Streaming_ModeByPass_DeInit(void);

extern	PLStreaming_ModeInst_t *PL_Streaming_ModeByPass_SetupInstance(PLStreaming_ModeInst_t *pstInst);
extern	void					PL_Streaming_ModeByPass_DeleteInstance(PLStreaming_ModeInst_t *pstInst);
extern	HOM_Result_e 			PL_Streaming_ModeByPass_ResConnect (PLStreaming_ModeInst_t *pstInst);
extern	HOM_Result_e 			PL_Streaming_ModeByPass_ResConflict (PLStreaming_ModeInst_t *pstInst, HINT32 eNotifyType, HINT32 nRequestPid, HCHAR *fromProcessName);
extern	HOM_Result_e			PL_Streaming_ModeByPass_GetM3U8(PLStreaming_ModeInst_t *pstInst, const HCHAR *pRequestPath, HUINT32 *pulDataSize, HUINT8 **ppucData);
extern	HOM_Result_e			PL_Streaming_ModeByPass_ReleaseM3U8(PLStreaming_ModeInst_t *pstInst, const HCHAR *pRequestPath, HUINT8 *pucData);
extern	HOM_Result_e			PL_Streaming_ModeByPass_GetTS(PLStreaming_ModeInst_t *pstInst,  const HCHAR *pRequestPath, HUINT32 *pulDataSize, HUINT8 **ppucData);
extern	HOM_Result_e			PL_Streaming_ModeByPass_ReleaseTS(PLStreaming_ModeInst_t *pstInst, const HCHAR *pRequestPath, HUINT8 *pucData);
extern	HOM_Result_e 			PL_Streaming_ModeByPass_ExecCmd(PLStreaming_ModeInst_t *pstInst,  const HCHAR *pRequestPath);
extern	HOM_Result_e 			PL_Streaming_ModeByPass_GetUIDByStartInfo (const PLStreaming_StartInfo_t *pstStartInfo, HUID *ret_uUID);
extern	HOM_Result_e			PL_Streaming_ModeByPass_StartInfoToSetupInfo(const HCHAR *pszPrefix, const PLStreaming_StartInfo_t *pstStartInfo, PLStreaming_SetupInfo_t *ret_pstSetupInfo);
extern 	HOM_Result_e			PL_Streaming_ModeByPass_GetProfileItemHandle(PLStreaming_ModeInst_t *pstInst, Handle_t *ret_phProfileItem);
extern	HOM_Result_e 			PL_Streaming_ModeByPass_GetPIDs(PLStreaming_ModeInst_t *pstInst, HINT32 *ret_pnPIDsCount, HUINT16 *ret_arrusPIDs);
extern	HOM_Result_e 			PL_Streaming_ModeByPass_GetInfo(PLStreaming_ModeInst_t *pstInst, PLStreaming_InfoType_e eInfoType, const HCHAR *pRequestPath, HUINT32 *pulDataSize, HCHAR **ppucData);
extern	HOM_Result_e			PL_Streaming_ModeByPass_ReleaseInfo(PLStreaming_ModeInst_t *pstInst, PLStreaming_InfoType_e eInfoType, const HCHAR *pRequestPath, HCHAR *pucData);


/* *************************************************************************************************** */
#define ___DEBUG_FUNCTION_HEADER_________________________________________________________________________
/* *************************************************************************************************** */

extern 	void					PL_Streaming_ModeByPass_Dump(PLStreaming_ModeInst_t *pstInst);

#endif /* __INT_PL_STREAMING_MODE_BYPASS__ */

