/**************************************************************
*
*	http://www.humaxdigital.com
*
*	@data			2012/10/24
*	@author			humax
**************************************************************/


/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ⓒ 2011-2012 Humax Co., Ltd.
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


/**********************************************************************************************************/
#define ____INCLUDE_______________________
/**********************************************************************************************************/
#include "../int_streaming.h"



/**********************************************************************************************************/
#define ____DEFINE________________________
/**********************************************************************************************************/

/**********************************************************************************************************/
#define ____TYPEDEF________________________
/**********************************************************************************************************/

typedef enum __plStreamingSpecItemEnum
{
	ePLStreamingSpec_None 		=	0,
	ePLStreamingSpec_HLS_BufferSize,

	ePLStreamingSpec_SATIP_BufferSize,

	ePLStreamingSpec_BYPASS_BufferSize,


	ePLStreamingSpec_EndOfEnum
} _plStreamingSpecItemEnum_e;

typedef struct __plStreamingSpecItem
{
	_plStreamingSpecItemEnum_e eKindOf;

	HBOOL	bIsInteager;
	HINT32	nValue;
	HCHAR	*pszValue;
} _plStreamingSpecItem_t;

STATIC _plStreamingSpecItem_t s_stStreamingSpec_MinSize[] =
{
	{ePLStreamingSpec_HLS_BufferSize,		TRUE,	(17 * 1024 * 1128),	NULL},
	{ePLStreamingSpec_SATIP_BufferSize,		TRUE,	( 8 * 1024 * 1128),	NULL},
	{ePLStreamingSpec_BYPASS_BufferSize,	TRUE,	(188 * 7 * 2500),	NULL},

	{ePLStreamingSpec_EndOfEnum, 			TRUE,	0,					NULL},
};


STATIC _plStreamingSpecItem_t s_stStreamingSpec_Base[] = /* HMS-1000S/EU, HMS-1000T, HDR-3000T */
{
	{ePLStreamingSpec_HLS_BufferSize,		TRUE,	(25 * 1024 * 1128),	NULL},
	{ePLStreamingSpec_SATIP_BufferSize,		TRUE,	(10 * 1024 * 1128),	NULL},
	{ePLStreamingSpec_BYPASS_BufferSize,	TRUE,	(188 * 7 * 2500),	NULL},

	{ePLStreamingSpec_EndOfEnum, 			TRUE,	0,					NULL},
};


STATIC _plStreamingSpecItem_t s_stStreamingSpec_MaxSize[] =
{
	{ePLStreamingSpec_HLS_BufferSize,		TRUE,	(30 * 1024 * 1128),	NULL},
	{ePLStreamingSpec_SATIP_BufferSize,		TRUE,	(20 * 1024 * 1128),	NULL},
	{ePLStreamingSpec_BYPASS_BufferSize,	TRUE,	(188 * 7 * 2500),	NULL},  // about 3.2 MB

	{ePLStreamingSpec_EndOfEnum, 			TRUE,	0,					NULL},
};

/**********************************************************************************************************/
#define ____INTERNAL_STRUCTURES_AND_VARIABLES_______________________
/**********************************************************************************************************/

STATIC PLStreaimng_Spec_e	s_eStreamingUseSpec;




/**********************************************************************************************************/
#define ____PRIVATE_FUNCTIONS_______________________
/**********************************************************************************************************/

STATIC PLStreaimng_Spec_e _plstreaming_Spec_GetSpec (void)
{
	return s_eStreamingUseSpec;
}

STATIC void _plstreaming_Spec_SetSpec (PLStreaimng_Spec_e	eStreamingUseSpec)
{
	s_eStreamingUseSpec = eStreamingUseSpec;
}

STATIC _plStreamingSpecItem_t	*_plstreaming_Spec_FindItem (_plStreamingSpecItemEnum_e	eSpecItemEnum, _plStreamingSpecItem_t pItemArr[])
{
	HINT32 	i = 0;
	HINT32	nMax = 0;

	nMax = sizeof(s_stStreamingSpec_Base) / sizeof(_plStreamingSpecItem_t);	// 최대 loop는 base로. (모든 아이템은 base에 default값으로 구현되어 있어야 함)

	for(i=0; i < nMax; i++)
	{
		if(ePLStreamingSpec_EndOfEnum == pItemArr[i].eKindOf)
		{
			break;
		}
		if(eSpecItemEnum == pItemArr[i].eKindOf)
		{
			return &pItemArr[i];
		}
	}

	return NULL;
}

STATIC _plStreamingSpecItem_t	*_plstreaming_Spec_SelectItem (PLStreaimng_Spec_e eSpec, _plStreamingSpecItemEnum_e	eSpecItemEnum)
{
	_plStreamingSpecItem_t *pSpecItem = NULL;

	switch(eSpec)
	{
	case ePLStreaming_Spec_MinSize:
		pSpecItem = _plstreaming_Spec_FindItem(eSpecItemEnum, s_stStreamingSpec_MinSize);
		break;

	case ePLStreaming_Spec_MaxSize:
		pSpecItem = _plstreaming_Spec_FindItem(eSpecItemEnum, s_stStreamingSpec_MaxSize);
		break;

	default:
		pSpecItem = NULL;
		break;
	}

	if(NULL != pSpecItem)
	{
		return pSpecItem;
	}


	/* base는 값이 없으면 항상 검색 */
	pSpecItem = _plstreaming_Spec_FindItem(eSpecItemEnum, s_stStreamingSpec_Base);
	if(NULL == pSpecItem)
	{
		HxLOG_Critical	("check spec item!!\r\n");
		HxLOG_Error		("check spec item!!\r\n");
		HxLOG_Warning	("check spec item!!\r\n");
	}

	return pSpecItem;
}


STATIC HINT32	_plstreaming_Spec_GetInteagerItem (PLStreaimng_Spec_e eSpec, _plStreamingSpecItemEnum_e	eSpecItemEnum)
{
	_plStreamingSpecItem_t *pSpecItem = NULL;


	pSpecItem = _plstreaming_Spec_SelectItem(eSpec, eSpecItemEnum);
	if(NULL == pSpecItem)
	{
		HxLOG_Error	("check spec item!!\r\n");
		return 0;
	}

	if(TRUE != pSpecItem->bIsInteager)
	{
		HxLOG_Error ("(%d) item is not InteargerType!!\r\n", eSpecItemEnum);
		return 0;
	}

	return pSpecItem->nValue;
}

STATIC HCHAR	*_plstreaming_Spec_GetStringItem (PLStreaimng_Spec_e eSpec, _plStreamingSpecItemEnum_e	eSpecItemEnum)
{
	_plStreamingSpecItem_t *pSpecItem = NULL;


	pSpecItem = _plstreaming_Spec_SelectItem(eSpec, eSpecItemEnum);
	if(NULL == pSpecItem)
	{
		HxLOG_Error	("check spec item!!\r\n");
		return 0;
	}

	if(TRUE == pSpecItem->bIsInteager)
	{
		HxLOG_Error ("(%d) item is not StringType!!\r\n", eSpecItemEnum);
		return 0;
	}

	return pSpecItem->pszValue;
}

/**********************************************************************************************************/
#define ____PUBLIC_FUNCTIONS_______________________
/**********************************************************************************************************/

/**
 *
 *
 *
 * @param
 * @return
 */
PLStreaimng_Spec_e PL_Streaming_Spec_GetSpec (void)
{
	PLStreaimng_Spec_e eStreamingUseSpec = eDxSchedule_Spec_Base;

	ENTER_FUNCTION;

	eStreamingUseSpec = _plstreaming_Spec_GetSpec();

	LEAVE_FUNCTION;

	return eStreamingUseSpec;
}


/**
 *
 *
 *
 * @param
 * @return
 */
HINT32 PL_Streaming_Spec_HLS_GetBufferSize (void)
{
	HINT32 nSize;

	ENTER_FUNCTION;

	nSize = _plstreaming_Spec_GetInteagerItem(_plstreaming_Spec_GetSpec(), ePLStreamingSpec_HLS_BufferSize);

	LEAVE_FUNCTION;

	return nSize;
}


/**
 *
 *
 *
 * @param
 * @return
 */
HINT32 PL_Streaming_Spec_SATIP_GetBufferSize (void)
{
	HINT32 nSize;

	ENTER_FUNCTION;

	nSize = _plstreaming_Spec_GetInteagerItem(_plstreaming_Spec_GetSpec(), ePLStreamingSpec_SATIP_BufferSize);

	LEAVE_FUNCTION;

	return nSize;
}

/**
 *
 *
 *
 * @param
 * @return
 */
HINT32	 PL_Streaming_Spec_ByPass_GetBufferSize (void)
{
	HINT32 nSize;

	ENTER_FUNCTION;

	nSize = _plstreaming_Spec_GetInteagerItem(_plstreaming_Spec_GetSpec(), ePLStreamingSpec_BYPASS_BufferSize);

	LEAVE_FUNCTION;

	return nSize;
}



/**
 *
 *
 *
 * @param
 * @return
 */
HOM_Result_e PL_Streaming_Spec_Init(PLStreaimng_Spec_e	eStreamingUseSpec)
{
	ENTER_FUNCTION;

	_plstreaming_Spec_SetSpec(eStreamingUseSpec);

	LEAVE_FUNCTION;

	return eHOM_Result_Ok;
}


/**********************************************************************************************************/
#define ____DEBUG_FUNCTIONS_______________________
/**********************************************************************************************************/
void	PL_Streaming_Spec_NotUsedFunc_RemoveWarning (void)
{
	(void)_plstreaming_Spec_GetStringItem;
	(void)s_stStreamingSpec_MaxSize;
}


