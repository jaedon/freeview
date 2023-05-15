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
/*******************************************************************************
*
* ImeAPI.c
*
* Revision 1.00 - 2005-03-28
*	- Implemented & modified by JK Baek
*	- modified by ccash@humaxdigital.com (2012/0925)
*
********************************************************************************/

#if	1

//#include "octo_common.h"
#include	<hlib.h>

#include "imeapi.h"
#include "../apiask.h"

#define _____IME_CANDIDATER_____

static IME_ELEM 		*s_imeData;					// text data linked list
static HINT32			s_imeIndex;					// index of the current item
static HINT32			s_imeNum;					// total number of items

HBOOL				IME_Init(void)
{
	s_imeData = NULL;
	s_imeIndex = 0;
	s_imeNum = 0;

	return TRUE;
}

HBOOL				IME_Destroy(void)
{
	IME_DeleteItem();

	s_imeData = NULL;
	s_imeIndex = 0;
	s_imeNum = 0;

	return TRUE;
}

HBOOL				IME_AddItem(HUINT8 *inText, HINT32 inLength)
{
	IME_ELEM		*pData = s_imeData;
	IME_ELEM		*pNew = NULL;
	char				*pStr = NULL;
	int				i = 0;

	if (s_imeNum == 0)
	{
		s_imeData = (IME_ELEM *)HLIB_STD_MemAlloc(sizeof(IME_ELEM));
		pStr = (char *)HLIB_STD_MemAlloc(inLength + 1);

		HxSTD_MemSet(pStr, 0x00, inLength + 1);		//memset
		HxSTD_StrNCpy(pStr, (char*)inText, inLength);		//strncpy

		s_imeData->text = pStr;
		s_imeData->pNext = NULL;

		s_imeNum = 1;
	}
	else
	{
		while (pData)
		{
			i++;

			if (pData->pNext == NULL)
				break;

			pData = pData->pNext;
		}
		pNew = (IME_ELEM *)HLIB_STD_MemAlloc(sizeof(IME_ELEM));
		pStr = (char *)HLIB_STD_MemAlloc(inLength + 1);

		HxSTD_MemSet(pStr, 0x00, inLength + 1);		//memset
		HxSTD_StrNCpy(pStr, (char*)inText, inLength);		//strncpy

		pNew->text = pStr;
		pNew->pNext = NULL;

		pData->pNext = pNew;

		s_imeNum++;
	}

	return TRUE;
}

HBOOL				IME_DeleteItem(void)
{
	IME_ELEM		*pData = s_imeData;
	IME_ELEM		*pFirst = NULL;

	if (s_imeNum > 0)
	{
		while (pData)
		{
			if (pData)
			{
				pFirst = pData->pNext;

				if (pData->text != NULL)
				{
					HLIB_STD_MemFree(pData->text);
					pData->text = NULL;
				}

				HLIB_STD_MemFree(pData);
				pData = NULL;
			}
			pData = pFirst;
		}
		s_imeNum = 0;
	}

	return TRUE;
}

HUINT8 *			IME_GetItemText(HINT32 index)
{
	IME_ELEM		*pData = s_imeData;
	int				i = 0;

	while (pData)
	{
		if (index == i)
			return (HUINT8*)pData->text;

		pData = pData->pNext;
		i++;
	}

	return NULL;
}

HINT32				IME_GetItemNum(void)
{
	return s_imeNum;
}

HINT32				IME_GetCurrentSelectedItemIndex(void)
{
	return s_imeIndex;
}

void					IME_SetCurrentSelectedItemIndex(HINT32 selected_index)
{
	if (s_imeNum > 0)
	{
		if (selected_index < 0)
			s_imeIndex = 0;
		else if (selected_index > s_imeNum)
			s_imeIndex = s_imeNum - 1;
		else
			s_imeIndex = selected_index;
	}
	else
		s_imeIndex = -1;
}


#define _____ASK_API_WRAPPER_____

HBOOL				HIME_Initialize(T_IME *self)
{
	HxSTD_MemSet(self, 0, sizeof(T_IME));	//memset
	apiask_28_OpenDic();
	return TRUE;
}

HBOOL				HIME_Finalize(T_IME *self)
{
	apiask_29_CloseDic();
	return TRUE;
}

HINT32				HIME_LengthenCandidate(T_IME *self)
{
	return apiask_33_LongerKouho(self->fCandidate, self->fContinuation);
}

HINT32				HIME_ShortenCandidate(T_IME *self)
{
	return apiask_34_ShorterKouho(self->fCandidate, self->fContinuation);
}

HINT32				HIME_NextWord(T_IME *self)
{
	return apiask_36_NextBunsetu(self->fCandidate, self->fContinuation);
}

HUINT8 *			HIME_NextCandidate(T_IME *self, HBOOL in_buffer)
{
	HINT32			ret;

	if (in_buffer) {
		ret = apiask_21_NextKouho(self->fTmp, self->fContinuation);
		if (ret != 0)
			return (HUINT8 *)self->fTmp;
		else
			return NULL;
	} else {
		ret = apiask_21_NextKouho(self->fCandidate, self->fContinuation);
		if (ret != 0)
			return (HUINT8 *)self->fCandidate;
		else
			return NULL;
	}
}



HINT32				HIME_StartConversion(T_IME *self, HUINT8 *in_string)
{
	return apiask_19_Henkan((char *)in_string, self->fCandidate, self->fContinuation);
}

HINT32				HIME_ConvertKatakanaCandidate(T_IME *self, HUINT8 *outStr)
{
	// outStr should be a larger buffer than the size of BunsetuMax
	HINT32			ret;
	if (outStr == NULL)
		return -1;
	ret = apiask_37_FirstHenkanToKata ((char*)outStr, self->fContinuation);
	return ret;
}

HINT32				HIME_ConvertHiraganaCandidate(T_IME *self, HUINT8 *outStr)
{
	// outStr should be a larger buffer than the size of BunsetuMax
	HINT32			ret;
	if (outStr == NULL)
		return -1;
	ret = apiask_38_FirstHenkanToHira ((char*)outStr, self->fContinuation);
	return ret;
}

HINT32				HIME_ConvertHankakuCandidate(T_IME *self, HUINT8 *outStr)
{
	return apiask_39_FirstHenkanToHan((char*)outStr, self->fContinuation);
}

HINT32				HIME_ConvertZenkakuCandidate(T_IME *self, HUINT8 *outStr)
{
	return apiask_40_FirstHenkanToZen((char*)outStr, self->fContinuation);
}

HUINT8 *			HIME_ContinuationString(T_IME *self)
{
	return (HUINT8 *)self->fContinuation;
}

HUINT8 *			HIME_CandidateString(T_IME *self)
{
	return (HUINT8 *)self->fCandidate;
}
#endif

