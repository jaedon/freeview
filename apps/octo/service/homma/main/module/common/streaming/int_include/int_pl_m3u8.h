
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


#ifndef	__PL_M3U8_H__
#define	__PL_M3U8_H__


#include <_hlib_string.h>

/* *************************************************************************************************** */
#define ___DEFINE________________________________________________________________________________________
/* *************************************************************************************************** */

/* *************************************************************************************************** */
#define ___TYPEDEF_______________________________________________________________________________________
/* *************************************************************************************************** */
typedef struct _PLM3U8 PL_M3U8_t;

#define		PLM3U8_QUEUESIZE_INFINITY	0x7fffffff

/* *************************************************************************************************** */
#define ___FUNCTION_HEADER_______________________________________________________________________________
/* *************************************************************************************************** */

PL_M3U8_t	*PL_M3U8_New(HINT32 queueSize);
void		PL_M3U8_Delete(PL_M3U8_t *pSelf);
void		PL_M3U8_Clear(PL_M3U8_t *pSelf);


HERROR		PL_M3U8_AppendTs(PL_M3U8_t *pSelf, HCHAR *name, HINT32 bandwidth_k, HINT32 index);
HERROR		PL_M3U8_AppendEndList(PL_M3U8_t *pSelf);
HERROR		PL_M3U8_AppendM3U8(PL_M3U8_t *pSelf, HCHAR *name, HINT32 bandwidth_k);
HERROR		PL_M3U8_AppendInteger(PL_M3U8_t *pSelf, HINT32 integer);
HERROR		PL_M3U8_AppendString(PL_M3U8_t *pSelf, HCHAR *string);
HERROR		PL_M3U8_AppendHxSTR(PL_M3U8_t *pSelf, HxSTR_t *string);
HERROR		PL_M3U8_AppendKey(PL_M3U8_t *pSelf, HCHAR *kindof_encrypt, HCHAR *key);
HERROR		PL_M3U8_AppendMediaSequence(PL_M3U8_t *pSelf, HINT32 start_file_number);
HERROR		PL_M3U8_AppendDiscontinuity(PL_M3U8_t *pSelf);
HERROR		PL_M3U8_AppendAllowCacheNo(PL_M3U8_t *pSelf);
HERROR		PL_M3U8_AppendStreamInf(PL_M3U8_t *pSelf, HINT32 program_id, HINT32 bandwidth,  HCHAR *codec, HCHAR *resoultion);
HERROR		PL_M3U8_AppendTargetDuration(PL_M3U8_t *pSelf, HINT32 second);
HERROR		PL_M3U8_AppendInf(PL_M3U8_t *pSelf, HINT32 second, HCHAR *title);
HERROR		PL_M3U8_GetSize(PL_M3U8_t *pSelf, HINT32 *pSize);
HxSTR_t		*PL_M3U8_ToBuffer(PL_M3U8_t *pSelf);
HERROR		PL_M3U8_AppendInfWithTS(PL_M3U8_t *pSelf, HINT32 inf_second, HCHAR *inf_title, HCHAR *ts_name, HINT32 ts_bandwidth_k, HINT32 ts_startindex, HINT32 ts_endindex);

HERROR		PL_M3U8_GetFirstTS(PL_M3U8_t *pSelf, HxSTR_t **ret_str);
HERROR		PL_M3U8_GetNextTS(PL_M3U8_t *pSelf, const HxSTR_t *cur_str, HxSTR_t **ret_str);

HERROR		PL_M3U8_GetFirstM3U8(PL_M3U8_t *pSelf, HxSTR_t **ret_str);
HERROR		PL_M3U8_GetNextM3U8(PL_M3U8_t *pSelf, const HxSTR_t *cur_str, HxSTR_t **ret_str);


/* *************************************************************************************************** */
#define ___DEBUG_FUNCTION_HEADER_________________________________________________________________________
/* *************************************************************************************************** */

void	PL_M3U8_Dump(PL_M3U8_t *pSelf);


#endif /* __PL_M3U8_H__ */

