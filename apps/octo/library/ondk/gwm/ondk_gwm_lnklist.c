/**************************************************************
*	@file		ondk.c
*	Minimized Graphic Library for DirectFB
*
*	http://www.humaxdigital.com
*
*	@data			2012/09/17
*	@author			humax
*	@brief			Minimized Graphic Library for DirectFB
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



/* ======================================================================== */
/* Include. 																						*/
/* ======================================================================== */
#include <ondk_gwm.h>

#include "ondk_gwm_lnklist.h"


/* ======================================================================== */
/* Private Macros. 																					*/
/* ======================================================================== */

/* ======================================================================== */
/* Global/Extern Variables.																			*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Types.																					*/
/* ======================================================================== */
struct	tagLinkObject
{
	LINK_OBJECT		nextLink;
};

/* ======================================================================== */
/* Private Constants. 																				*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Variables. 																				*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Function prototypes. 																		*/
/* ======================================================================== */

/* ======================================================================== */
/* Functions. 																						*/
/* ======================================================================== */

	LINK_OBJECT 	gwm_lnklst_GetNext(LINK_OBJECT item)
{
    GWM_assert(item);
	return	item->nextLink;
}



ONDK_Result_t	gwm_lnklst_AddItemWithCompareDescend(LINK_OBJECT *root, LINK_OBJECT new_item, int compare_func(LINK_OBJECT, LINK_OBJECT) )
{			// 비교함수에서 큰 값이 앞에 존재한다. 내림차순으로 비교.
	LINK_OBJECT		temp;

	GWM_assert(root);
	GWM_assert(new_item);
	GWM_assert(compare_func);

	if (new_item->nextLink != NULL)
	{	GWM_DBG_Print( DBGMSG_ERROR, ("새로 추가하는 노드가 다른 링크에 연결되어 있을 수 있습니다.\n"));
		return	GWM_RET_LINK_CONFLICT;
	}

	temp = *root;
	if (temp==NULL)		// 아직 하나도 없다.
	{	*root = new_item;
		return	GWM_RET_OK;
	}

	if (compare_func(new_item, *root) <= 0)
	{	new_item->nextLink = *root;
		*root = new_item;
		return	GWM_RET_OK;
	}

	while(temp->nextLink != NULL)
	{
		if (compare_func(new_item, temp->nextLink) <= 0)
			break;
		temp = temp->nextLink;
	}
	new_item->nextLink = temp->nextLink;
	temp->nextLink = new_item;

	return	GWM_RET_OK;
}



ONDK_Result_t	gwm_lnklst_RemoveItemWithCompare(LINK_OBJECT *root, LINK_OBJECT item, int compare_func(LINK_OBJECT , LINK_OBJECT ) )
{			// 비교함수에서 0을 리턴하는 조건의 Object를 삭제.
	LINK_OBJECT		temp, prev;

	GWM_assert(root);
	GWM_assert(item);
	GWM_assert(compare_func);

	if (*root == NULL)		// 리스트에 하나도 없다.
	{	return	GWM_RET_NO_OBJECT;
	}

	temp = *root;
	if (compare_func(item, *root)==0)	// 맨 첫번째 노드인 경우.
	{	*root = (*root)->nextLink;
		temp->nextLink = NULL;
		return	GWM_RET_OK;
	}

	do
	{	prev = temp;
		temp = temp->nextLink;
		if (compare_func(item, temp)==0)
			break;
	} while(temp != NULL);

	if (temp != NULL)			// 원하는 아이템을 찾았다면,
	{	prev->nextLink = temp->nextLink;
		temp->nextLink = NULL;
		return	GWM_RET_OK;
	}
								// 원하는 아이템을 찾지 못했다면,
	return	GWM_RET_NO_OBJECT;
}


/* End of File. ---------------------------------------------------------- */



