
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


#ifndef	__INT_PL_STREAMINGKEY__
#define	__INT_PL_STREAMINGKEY__



/* *************************************************************************************************** */
#define ___DEFINE________________________________________________________________________________________
/* *************************************************************************************************** */
#define PL_STREAMINGKEY_MAX_LENGTH		DxRSV_STREAMKEY_LEN


#define PL_STREAMINGKEY_FORMAT				"%s+0x%08X+0x%08X=%s"
#define PL_STERAMINGKEY_PREFIX_FORMAT		"%s"
#define PL_STERAMINGKEY_UID_FORMAT			"0x%08X"
#define PL_STERAMINGKEY_PROFILE_FORMAT		"0x%08X"
#define PL_STERAMINGKEY_DEVICEID_FORMAT		"%s"


#define PL_STERAMINGKEY_SPACE_LENGTH		1
#define PL_STERAMINGKEY_PREFIX_LENGTH		10
#define PL_STERAMINGKEY_UID_LENGTH			10
#define PL_STERAMINGKEY_PROFILE_LENGTH		10

#define PL_STREAMINGKEY_STREMINFOID_LENGTH	((PL_STERAMINGKEY_PREFIX_LENGTH+PL_STERAMINGKEY_UID_LENGTH+PL_STERAMINGKEY_PROFILE_LENGTH)+(PL_STERAMINGKEY_SPACE_LENGTH*3))


#define PL_STERAMINGKEY_DEVICEID_LENGTH		127



/* *************************************************************************************************** */
#define ___TYPEDEF_______________________________________________________________________________________
/* *************************************************************************************************** */
typedef struct _PL_StreamingKey_t PL_StreamingKey_t;


/* *************************************************************************************************** */
#define ___FUNCTION_HEADER_______________________________________________________________________________
/* *************************************************************************************************** */




extern	HOM_Result_e 		PL_StreamingKey_Init(void);
extern	void 				PL_StreamingKey_DeInit(void);



extern	PL_StreamingKey_t 	*PL_StreamingKey_New(const HCHAR *pszPrefix, HUID uUID, Handle_t hMediaProfile, const HCHAR *pszDeviceID);
extern	PL_StreamingKey_t 	*PL_StreamingKey_NewByString(const HCHAR *pszStreamString);
extern	PL_StreamingKey_t 	*PL_StreamingKey_Dup(PL_StreamingKey_t *pstStreamingkey);
extern	PL_StreamingKey_t 	*PL_StreamingKey_ConditionalDup(const PL_StreamingKey_t *pstSrc, const HCHAR *pszPrefix, const HUID uUID, const Handle_t pMediaProfile, const HCHAR *pszDeviceID);
extern	void 				PL_StreamingKey_Delete(PL_StreamingKey_t *pstStreamingkey);
extern	HOM_Result_e 		PL_StreamingKey_GetItem(const PL_StreamingKey_t *pstStreamingKey, HCHAR *ret_pszPrefix, HUID *ret_puUID, Handle_t *ret_phMediaProfile, HCHAR *ret_pszDeviceID);
extern	HCHAR 				*PL_StreamingKey_GetBuffer(const PL_StreamingKey_t *pstStreamingKey);
extern	HINT32				PL_StreamingKey_Copy(PL_StreamingKey_t *pstDst, const PL_StreamingKey_t *pstSrc);
extern	HINT32 				PL_StreamingKey_Compare(const PL_StreamingKey_t *pstSrc1, const PL_StreamingKey_t *pstSrc2);
extern	HINT32 				PL_StreamingKey_CompareWithoutProfile(const PL_StreamingKey_t *pstSrc1, const PL_StreamingKey_t *pstSrc2);
extern	HINT32 				PL_StreamingKey_Length(const PL_StreamingKey_t *pstStreamingKey);





/* *************************************************************************************************** */
#define ___DEBUG_FUNCTION_HEADER_________________________________________________________________________
/* *************************************************************************************************** */
extern	void				PL_StreamingKey_Dump (PL_StreamingKey_t *pstStreamingkey);

#endif /* __INT_PL_STREAMINGKEY__ */

