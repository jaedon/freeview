/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  ttx_top.c
	@brief

	Description:
	Module:

	Copyright (c) 2009 HUMAX Co., Ltd.			\n
	All rights reserved.						\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <prism.h>

#include <ttx_top.h>
#include <ttx_errchk.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/




/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#define ____TOP_DEBUG____

#ifdef CONFIG_DEBUG
	HUINT32 g_TtxTopDebugLevel = 0x00;
#endif





#define	____TOP_MACRO____


#define	TOP_MAX_BTT_LINK		10
#define	TOP_MAX_BTT_PROPERTY	800
#define TOP_MAX_AIT_ITEM		46
#define	TOP_MAX_MPT_PROPERTY	800
#define	TOP_MAX_MPT_EX_ITEM		110

#define	TTX_MXX_TO_TOP_MXX(mxx)	(((mxx) < 100) ? (mxx) + 700 : (mxx) - 100)
#define	TOP_MXX_TO_TTX_MXX(mxx)	(((mxx) + 100) % 800)

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

#define ____TOP_PRIVATE_TYPE____

enum
{
	eTOP_BTT_NO_PAGE,	// no page
	// TODO: 아래 속성들이... 뭔지 파악 안됨
	eTOP_BTT_0x1,
	eTOP_BTT_0x2,
	eTOP_BTT_0x3,
	eTOP_BTT_0x4,
	eTOP_BTT_0x5,
	eTOP_BTT_GROUP	= eTOP_BTT_0x5,

	eTOP_BTT_0x6,
	eTOP_BTT_0x7,
	eTOP_BTT_BLOCK	= eTOP_BTT_0x7,

	eTOP_BTT_0x8,
	eTOP_BTT_0x9,
	eTOP_BTT_0xA,
	eTOP_BTT_0xB,
	eTOP_BTT_0xC,
	eTOP_BTT_0xD,
	eTOP_BTT_0xE,
	eTOP_BTT_0xF,

	eTOP_BTT_LAST	= eTOP_BTT_0xF
};	// BTT Property Enum
typedef HUINT8	BTT_Property_t;

enum
{
	eTOP_MPT_,

	eTOP_MPT_LAST
};	// MPT Property Enum
typedef HUINT8	MPT_Property_t;


typedef struct
{
	TTX_TOP_Type_t	type;			///< TOP Type
	HUINT16			usPageNum;		///< Page Number
	void 	*		pvTable;		///< TOP Table Instance
} BTT_Link_t;

typedef struct
{
	HUINT16			usPageNum;		///< Page Number
	HUINT16			usSubcode;		///< Subcode
	/**
	 * BTT Page Properties
	 */
	BTT_Property_t	aProperty[TOP_MAX_BTT_PROPERTY];
	/**
	 * Page Linking Table
	 */
	BTT_Link_t		aLink[TOP_MAX_BTT_LINK];
	HUINT16			usBTTList;		///< BTT-List
	void			*pvNext;		///< Next BTT
} BTT_t;

typedef struct
{
	TTX_TOP_Link_t	title;			///< TOP Title
	HUINT8			ucTEMP;			// TODO: ???
} AIT_Item_t;

typedef struct
{
	HUINT16			usPageNum;		///< Page Number
	HUINT16			usSubcode;		///< Subcode

	/**
	 * Additional Information Items
	 */
	AIT_Item_t		aItem[TOP_MAX_AIT_ITEM];
} AIT_t;

typedef struct
{
	HUINT16			usPageNum;		///< Page Number
	HUINT16			usSubcode;		///< Subcode
	/**
	 * MPT Page Properties
	 */
	MPT_Property_t	aProperty[TOP_MAX_MPT_PROPERTY];
} MPT_t;

typedef struct
{
	HUINT16			usPageNum;		///< Page Number
	HUINT8			aucTEMP[5];		// TODO: ???
} MPT_EX_Item_t;

typedef struct
{
	HUINT16			usPageNum;		///< Page Number
	HUINT16			usSubcode;		///< Subcode
	/**
	 * Multi-Page Extention Items
	 */
	MPT_EX_Item_t	aItem[TOP_MAX_MPT_EX_ITEM];
} MPT_EX_t;

typedef struct
{
	BTT_t		*pBTT;		///< Basic TOP Table
	HUINT32		ulVersion;	///< Current Table Version
} TTX_TOP_t;



#define ____TOP_PRIVATE_COMMON____


#define ____TOP_BTT____


/**
 * BTT로부터 type과 page num에 해당되는 linking 정보를 가져온다.
 *
 * @param	pBTT		BTT
 * @param	type		TOP type
 * @param	usPageNum	top page number
 * @return	Page Linking / NULL
 */
static BTT_Link_t * local_top_btt_FindTable (BTT_t *pBTT, const TTX_TOP_Type_t type, const HUINT16 usPageNum)
{
	HINT32		i;
	BTT_Link_t	*pLink;

	for (i = 0 ; i < TOP_MAX_BTT_LINK ; i++)
	{
		pLink	= &(pBTT->aLink[i]);
		if (pLink->type == type && pLink->usPageNum == usPageNum)
		{
			return pLink;
		}
	}

/*
	if (pBTT->pvNext)
		return local_top_btt_FindTable(pBTT->pvNext, type, usPageNum);
*/
	return NULL;
}


/**
 * BTT Page Properties로부터 Block을 찾는다.
 *
 * @param	pBTT		BTT
 * @param	usBase		Base,  (100 ~ 899) - 100 값
 * @param	pusBlock	Block, (100 ~ 899) - 100 값
 * @return	ERR_OK / ERR_FAIL
 */
static HERROR	local_top_btt_FindBlock (BTT_t *pBTT, const HUINT16 usBase, HUINT16 *pusBlock)
{
	HINT32		i;
	HUINT16		usPageNum;
	BTT_Property_t	property;

	for (i = 0 ; i < 800 ; i++)
	{
		usPageNum	= (i + usBase) % 800;
		property	= pBTT->aProperty[usPageNum];

		// TODO: 0x3도 멈추는지 확인 필요...
		if(property == eTOP_BTT_0x3)
		{
			HxLOG_Critical("[%s:%d] Critical Eror!!!\n", __FUNCTION__, __HxLINE__);
		}
		if (eTOP_BTT_NO_PAGE < property && property <= eTOP_BTT_BLOCK)
		{
			*pusBlock	= usPageNum;

			return ERR_OK;
		}
	}

	return ERR_FAIL;
}


/**
 * BTT Page Properties로부터 Group을 찾는다.
 *
 * @param	pBTT		BTT
 * @param	usBase		Base,  (100 ~ 899) - 100 값
 * @param	pusGroup	Group, (100 ~ 899) - 100 값
 * @return	ERR_OK / ERR_FAIL
 */
static HERROR local_top_btt_FindGroup (BTT_t *pBTT, const HUINT16 usBase, HUINT16 *pusGroup)
{
	HINT32		i;
	HUINT16		usPageNum;
	BTT_Property_t	property;

	for (i = 0 ; i < 800 ; i++)
	{
		usPageNum	= (i + usBase) % 800;
		property	= pBTT->aProperty[usPageNum];

		// TODO: 0x3도 멈추는지 확인 필요...
		if(property == eTOP_BTT_0x3)
		{
			HxLOG_Critical("[%s:%d] Critical Eror!!!\n", __FUNCTION__, __HxLINE__);
		}
		if (eTOP_BTT_NO_PAGE < property && property <= eTOP_BTT_GROUP)
		{
			*pusGroup	= usPageNum;

			return ERR_OK;
		}
	}

	return ERR_FAIL;
}


/**
 * BTT Page 로부터 PageNum을 찾는다.
 *
 * @param	pBTT		BTT
 * @param	usBase		Base, (100 ~ 899) - 100 값
 * @param	pusPage		Page, (100 ~ 899) - 100 값
 * @param	bNext		if TRUE find next page, else find prev page
 * @return	ERR_OK / ERR_FAIL
 */
static HERROR local_top_btt_FindPage (BTT_t *pBTT, const HUINT16 usBase, HUINT16 *pusPage, const HBOOL bNext)
{
	HINT32	i		= 0;
	HUINT16	usPage	= usBase;

	for (i = 0 ; i < 800 ; i++)
	{
		if (bNext == TRUE)
			usPage = (usPage == 799) ? 0 : usPage + 1;
		else	// bPrev
			usPage = (usPage == 0) ? 799 : usPage - 1;

		if (pBTT->aProperty[usPage] != eTOP_BTT_NO_PAGE)
		{
			*pusPage	= usPage;
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}


/**
 * BTT로부터 page number에 해당되는 타입을 얻는다.
 *
 * @param	pBTT		BTT
 * @param	usPageNum	page number (TOP page num type, 0[=100]~799[=899])
 * @return	BTT Property
 */
static BTT_Property_t local_top_btt_GetProperty (BTT_t *pBTT, const HUINT16 usPageNum)
{
	return pBTT->aProperty[usPageNum % 800];
}


/**
 * Packet으로부터 Property를 파싱한다.
 *
 * @param	pProperty	BTT Page Properties
 * @param	pucPkt		Received Packet
 * @param	bUpdated	update_indicator
 * @return	ERR_OK/ERR_FAIL
 */
static HERROR local_top_btt_ParseProperty (BTT_Property_t *pProperty, const HUINT8 *pucPkt, const HBOOL bUpdated)
{
	if (pucPkt == NULL)
	{
		if (bUpdated == FALSE)
		{
			return ERR_FAIL;	// Skip
		}
		HxSTD_MemSet(pProperty, 0, 40);
	}
	else
	{
		HxSTD_MemCopy(pProperty, (void *)pucPkt, 40);
	}

	return ERR_OK;
}


/**
 * BTT Page Link 정보를 파싱
 *
 * @param	pLink	Dest
 * @param	pucLink	Src
 *				+-------+---+---+---+---+---+---+---+---+
 * 				| index | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
 * 				| value | M | X | X | ? | ? | ? | ? | T |
 * 				+-------+---+---+---+---+---+---+---+---+
 *				T := top type (1: MPT, 2: AIT, 3: MPT-EX)
 * @return	ERR_OK / ERR_FAIL
 */
static HERROR local_top_btt_ParseLink (BTT_Link_t *pLink, const HUINT8 *pucLink)
{
	TTX_TOP_Type_t	type;

	if (0x8 < pucLink[0])	// 0x8 < Magazine
	{
		HxLOG_Info("ParseLink Fail!!, Invalid magazine[0x%02X]\n", pucLink[0]);

		return ERR_FAIL;
	}

	switch (pucLink[7])
	{
	case 1:	type	= eTTX_TOP_MPT;		break;
	case 2:	type	= eTTX_TOP_AIT;		break;
	case 3:	type	= eTTX_TOP_MPT_EX;	break;
	default:
		HxLOG_Info("ParseLink Fail!!, Invalid table type[0x%02X]\n", pucLink[7]);
		return ERR_FAIL;
	}

	pLink->type			= type;
	pLink->usPageNum	= (HUINT16) ( ((pucLink[0] & 0xF) << 8)
									| ((pucLink[1] & 0xF) << 4)
									| ((pucLink[2] & 0xF) << 0) );

	return ERR_OK;
}


/**
 * BTT Page Linking Table을 Parsing
 *
 * @param	pLink		BTT Link
 * @param	pucPkt		row data
 * @param	bUpdated	update_indicator
 * @return	ERR_OK / ERR_FAIL
 */
static HERROR local_top_btt_ParseLinkingTable (BTT_Link_t *pLink, const HUINT8 *pucPkt, const HBOOL bUpdated)
{
	HINT32		i;
	HERROR		err;
	BTT_Link_t	bttLink;

	if (pucPkt == NULL)
	{
		if (bUpdated == FALSE)
		{
			return ERR_FAIL;	// Skip
		}
		HxSTD_MemSet(pLink, 0, sizeof(BTT_Link_t) * 5);

		return ERR_OK;
	}

	HxSTD_MemSet(&bttLink, 0, sizeof(BTT_Link_t));

	for (i = 0 ; i < 5 ; i++)
	{
		err	= local_top_btt_ParseLink(&bttLink, pucPkt + i * 8);
		if (err != ERR_OK
			|| (pLink[i].type == bttLink.type && pLink[i].usPageNum == bttLink.usPageNum))
		{
			continue;	// Skip
		}

		// Linking Table에 변화가 있으면, 해당 link에 달려 있는 Table을 삭제하고 다시 받아야 한다.
		if (pLink[i].pvTable)
		{
			AP_Free(pLink[i].pvTable);
		}
		HxSTD_MemCopy(pLink + i, &bttLink, sizeof(BTT_Link_t));
	}

	return ERR_OK;
}



/**
 * Receive Basic TOP Table
 *
 * @param	pTOP	instance of 'TOP'
 * @param	pTable	BTT row data
 * @return	ERR_OK / ERR_TOP_MALLOC_FAIL
 */
static HERROR local_top_btt_Receive	(TTX_TOP_t *pTOP, const TTX_TOP_Table_t *pTable)
{
	HINT32	i;
	HERROR	err;
	BTT_t	*pBTT		= pTOP->pBTT;		// TODO: BTT-List 값에 의해 BTT가 여러개 나올 수도 있겠지만... 우선 처리하지 않는다...
	HBOOL	bUpdated	= pTable->bUpdated;

	HxLOG_Print("[%s]\n", __FUNCTION__);

	if (pBTT == NULL)
	{
		pBTT = (BTT_t *) AP_Calloc(sizeof(BTT_t));
		if (pBTT == NULL)
		{
			return ERR_TOP_MALLOC_FAIL;
		}
		bUpdated = TRUE;
	}

	for (i = 0 ; i < 23 ; i++)
	{
		if (i < 20)
		{
			err	= local_top_btt_ParseProperty(
								pBTT->aProperty + i * 40,
								pTable->apData[i],
								pTable->bUpdated
							);
			if(err != ERR_OK)
			{
				// log
			}
		}
		else if (i < 22)
		{
			err	= local_top_btt_ParseLinkingTable(
								&(pBTT->aLink[(i == 20) ? 0 : 5]),
								pTable->apData[i],
								pTable->bUpdated
							);
			if(err != ERR_OK)
			{
				// log
			}
		}
		else	// 22..
		{
			// TODO:
			HxLOG_Error("Receive BTT List Packet\n");
		}
	}

	pBTT->usPageNum	= pTable->usPageNum;
	pBTT->usSubcode	= pTable->usSubcode;

	pTOP->pBTT	= pBTT;

	if (bUpdated == TRUE)
	{
		pTOP->ulVersion++;
		return ERR_TOP_UPDATED;
	}
	return ERR_OK;
}



#define ____TOP_AIT____

/**
 * AIT로부터 page number에 부합되는 Title을 찾는다.
 *
 * @param	pAIT		Additional Information Table
 * @param	usPageNum	page number
 * @param	pTitle		Title
 * @return	ERR_OK / ERR_FAIL
 */
static HERROR	local_top_ait_FindTitle (const AIT_t *pAIT, const HUINT16 usPageNum, TTX_TOP_Link_t *pTitle)
{
	HINT32	i;

	for (i = 0 ; i < TOP_MAX_AIT_ITEM ; i++)
	{
		if (pAIT->aItem[i].title.bEnabled == FALSE)
			continue;

		if (pAIT->aItem[i].title.usPageNum == usPageNum)
		{
			HxSTD_MemCopy(pTitle, (void *)&(pAIT->aItem[i].title), sizeof(TTX_TOP_Link_t));

			return ERR_OK;
		}
	}

	return ERR_FAIL;
}


/**
 * 20 byte의 pucData로부터 AIT Item을 추출
 *
 * @param	pItem		AIT Item
 * @param	pucData		20 data bytes
 * @param	bUpdated	update_indicator
 * @return	ERR_OK / ERR_FAIL
 */
static HERROR local_top_ait_ParseItem (AIT_Item_t *pItem, const HUINT8 *pucData, const HBOOL bUpdated)
{
	HUINT16	usPageNum;
	HUINT16	usSubcode;

	if (pucData == NULL)
	{
		if (bUpdated == TRUE)
		{
			HxSTD_MemSet(pItem, 0, sizeof(AIT_Item_t));
			return ERR_OK;
		}
		return ERR_FAIL;
	}

	usPageNum	= (HUINT16)(pucData[0] * 100 + pucData[1] * 10 + pucData[2]);
	usSubcode	= (HUINT16)((pucData[3] << 12) | (pucData[4] << 8) | (pucData[5] << 4) | pucData[6]);

	if (899 < usPageNum || 0x3F7F < usSubcode)
	{
		if (bUpdated == TRUE)
		{
			HxSTD_MemSet(pItem, 0, sizeof(AIT_Item_t));
		}
		return ERR_FAIL;
	}

	pItem->title.usPageNum	= usPageNum % 800;
	pItem->title.usSubcode	= usSubcode;
	pItem->title.bEnabled	= TRUE;
	HxSTD_MemCopy(pItem->title.aucTitle, (void *)(pucData + 8), TOP_AIT_TITLE_LEN);
	pItem->ucTEMP	= pucData[7];	// TODO: 이건 뭥미???

	return ERR_OK;
}


/**
 * TOP Table로부터 AIT 파싱
 *
 * @param	pAIT	AIT
 * @param	pTable	TOP Table
 * @return	ERR_OK
 */
static HERROR local_top_ait_Parse (AIT_t *pAIT, const TTX_TOP_Table_t *pTable)
{
	HINT32	i;
	HUINT8	*pucData;

	for (i = 0 ; i < TOP_MAX_AIT_ITEM ; i++)
	{
		pucData	= pTable->apData[i / 2];
		if (pucData != NULL)
		{
			pucData	+= (i % 2) * 20;
		}

		(void) local_top_ait_ParseItem(&(pAIT->aItem[i]), pucData, pTable->bUpdated);
	}

	return ERR_OK;
}



/**
 * Receive Additional Information Table
 *
 * @param	pTOP	instance of 'TOP'
 * @param	pTable	row data 'AIT'
 * @return	ERR_TOP_MALLOC_FAIL / ERR_FAIL / return local_top_ait_Parse()
 */
static HERROR local_top_ait_Receive (TTX_TOP_t *pTOP, const TTX_TOP_Table_t *pTable)
{
	BTT_Link_t	*pLink;
	AIT_t		*pAIT;
	HERROR		err;
	HBOOL		bUpdated	= pTable->bUpdated;

	pLink	= local_top_btt_FindTable(pTOP->pBTT, pTable->type, pTable->usPageNum);
	if (pLink == NULL)
	{
		return ERR_FAIL;
	}

	pAIT	= (AIT_t *)pLink->pvTable;
	if (pAIT == NULL)
	{
		pAIT = (AIT_t *)AP_Calloc(sizeof(AIT_t));
		if (pAIT == NULL)
		{
			return ERR_TOP_MALLOC_FAIL;
		}
		bUpdated	= TRUE;
	}

	pLink->pvTable	= pAIT;
	pAIT->usPageNum	= pTable->usPageNum;
	pAIT->usSubcode	= pTable->usSubcode;

	err = local_top_ait_Parse(pAIT, pTable);
	if (err != ERR_OK)
	{
		return err;
	}

	if (bUpdated == TRUE)
	{
		pTOP->ulVersion++;
		return ERR_TOP_UPDATED;
	}
	return ERR_OK;
}


#define ____TOP_MPT____


/**
 * Packet으로부터 Property를 파싱한다.
 *
 * @param	pProperty	MPT Page Properties
 * @param	pucPkt		Received Packet
 * @param	bUpdated	update_indicator
 * @return	ERR_OK/ERR_FAIL
 */
static HERROR local_top_mpt_ParseProperty (MPT_Property_t *pProperty, const HUINT8 *pucPkt, const HBOOL bUpdated)
{
	if (pucPkt == NULL)
	{
		if (bUpdated == FALSE)
		{
			return ERR_FAIL;	// Skip
		}
		HxSTD_MemSet(pProperty, 0, 40);
	}
	else
	{
		HxSTD_MemCopy(pProperty, (void *)pucPkt, 40);
	}

	return ERR_OK;
}


/**
 * Receive Multi-Page Table
 *
 * @param	pTOP	instance of 'TOP'
 * @param	pTable	MPT row data
 * @return	ERR_OK / ERR_FAIL
 */
static HERROR local_top_mpt_Receive (TTX_TOP_t *pTOP, const TTX_TOP_Table_t *pTable)
{
	HINT32		i;
	HERROR		err;
	BTT_Link_t	*pLink;
	MPT_t		*pMPT;
	HBOOL		bUpdated	= pTable->bUpdated;

	pLink	= local_top_btt_FindTable(pTOP->pBTT, pTable->type, pTable->usPageNum);
	if (pLink == NULL)
	{
		return ERR_FAIL;
	}

	pMPT	= (MPT_t *)pLink->pvTable;
	if (pMPT == NULL)
	{
		pMPT = (MPT_t *)AP_Calloc(sizeof(MPT_t));
		if (pMPT == NULL)
		{
			return ERR_TOP_MALLOC_FAIL;
		}
		bUpdated	= TRUE;
	}

	for (i = 0 ; i < 20 ; i++)
	{
		err	= local_top_mpt_ParseProperty(
								pMPT->aProperty + i * 40,
								pTable->apData[i],
								pTable->bUpdated
							);
		if(err != ERR_OK)
		{
			// log
		}
	}

	pLink->pvTable	= pMPT;
	pMPT->usPageNum	= pTable->usPageNum;
	pMPT->usSubcode	= pTable->usSubcode;

	if (bUpdated == TRUE)
	{
		pTOP->ulVersion++;
		return ERR_TOP_UPDATED;
	}

	return ERR_OK;
}

#define ____TOP_MPT_EX____



/**
 * 8 byte의 pucData로부터 MPT-EX Item을 추출
 *
 * @param	pItem		MPT-EX Item
 * @param	pucData		8 data bytes
 * @param	bUpdated	update_indicator
 * @return	ERR_OK / ERR_FAIL
 */
static HERROR local_top_mpt_ex_ParseItem (MPT_EX_Item_t *pItem, const HUINT8 *pucData, const HBOOL bUpdated)
{
	HUINT16	usPageNum;

	if (pucData == NULL)
	{
		if (bUpdated == TRUE)
		{
			HxSTD_MemSet(pItem, 0, sizeof(MPT_EX_Item_t));
			return ERR_OK;
		}
		return ERR_FAIL;
	}

	usPageNum	= (HUINT16)(pucData[0] * 100 + pucData[1] * 10 + pucData[2]);

	if (899 < usPageNum)
	{
		if (bUpdated == TRUE)
		{
			HxSTD_MemSet(pItem, 0, sizeof(AIT_Item_t));
		}
		return ERR_FAIL;
	}

	pItem->usPageNum	= usPageNum % 800;
	pItem->aucTEMP[0]	= pucData[3];
	pItem->aucTEMP[1]	= pucData[4];
	pItem->aucTEMP[2]	= pucData[5];
	pItem->aucTEMP[3]	= pucData[6];
	pItem->aucTEMP[4]	= pucData[7];	// TODO:.... WHAT THE...??

	return ERR_OK;
}



/**
 * TOP Table로부터 MPT-EX 파싱
 *
 * @param	pMPT_EX	MPT-EX
 * @param	pTable	TOP Table
 * @return	ERR_OK
 */
static HERROR local_top_mpt_ex_Parse (MPT_EX_t *pMPT_EX, const TTX_TOP_Table_t *pTable)
{
	HINT32	i;
	HUINT8	*pucData;

	for (i = 0 ; i < TOP_MAX_MPT_EX_ITEM ; i++)
	{
		pucData	= pTable->apData[i / 5];
		if (pucData != NULL)
		{
			pucData	+= (i % 5) * 8;
		}

		(void) local_top_mpt_ex_ParseItem(&(pMPT_EX->aItem[i]), pucData, pTable->bUpdated);
	}

	return ERR_OK;
}


/**
 * Receive Multi-Page Extension Table
 *
 * @param	pTOP	instance of 'TOP'
 * @param	pTable	MPT-EX row data
 * @return	ERR_OK / ERR_FAIL
 */
static HERROR local_top_mpt_ex_Receive (TTX_TOP_t *pTOP, const TTX_TOP_Table_t *pTable)
{
	HERROR		err;
	BTT_Link_t	*pLink;
	HBOOL		bUpdated	= pTable->bUpdated;

	pLink	= local_top_btt_FindTable(pTOP->pBTT, pTable->type, pTable->usPageNum);
	if (pLink == NULL)
	{
		return ERR_FAIL;
	}

	if (pLink->pvTable == NULL)
	{
		pLink->pvTable = (MPT_EX_t *)AP_Calloc(sizeof(MPT_EX_t));
		if (pLink->pvTable == NULL)
		{
			return ERR_TOP_MALLOC_FAIL;
		}
		bUpdated	= TRUE;
	}

	err = local_top_mpt_ex_Parse(pLink->pvTable, pTable);
	if (err != ERR_OK)
	{
		return err;
	}

	if (bUpdated == TRUE)
	{
		pTOP->ulVersion++;
		return ERR_TOP_UPDATED;
	}
	return ERR_OK;
}

#define	____TOP_PRIVATE____


/**
 * Receive TOP Table
 *
 * @param	pTOP	instance of 'TOP'
 * @param	pTable	TOP Table row data
 * @return	ERR_FAIL / ...
 */
static HERROR local_top_Receive (TTX_TOP_t *pTOP, const TTX_TOP_Table_t *pTable)
{
	switch (pTable->type)
	{
	case eTTX_TOP_BTT:		return local_top_btt_Receive(pTOP, pTable);
	case eTTX_TOP_AIT:		return local_top_ait_Receive(pTOP, pTable);
	case eTTX_TOP_MPT:		return local_top_mpt_Receive(pTOP, pTable);
	case eTTX_TOP_MPT_EX:	return local_top_mpt_ex_Receive(pTOP, pTable);
	default:
		break;
	}

	return ERR_FAIL;
}


/**
 * Make Default TOP Title From Page number
 *
 * @param	pTitle		title
 * @param	usPageNum	page number
 */
static void	local_top_MakeTitle (TTX_TOP_Link_t *pTitle, const HUINT16 usPageNum)
{
	if (pTitle->bEnabled)
		return;

	HxSTD_MemSet(pTitle->aucTitle, 0x20, TOP_AIT_TITLE_LEN);

	pTitle->bEnabled	= TRUE;
	pTitle->usPageNum	= usPageNum;
	pTitle->usSubcode	= 0x3F7F;
	pTitle->aucTitle[5]	= (HUINT8)(((usPageNum / 100) == 0) ? '8' : (usPageNum / 100) + '0');
	pTitle->aucTitle[6]	= (HUINT8)((usPageNum % 100) / 10 + '0');
	pTitle->aucTitle[7]	= (HUINT8)(usPageNum % 10 + '0');
}


/**
 * Page number에 해당되는 페이지에 대해 TOP 정보로 부터 Block, Group 정보를 가져와 링크한다.
 *
 * @param	pTOP		instance of 'TOP'
 * @param	usPageNum	page number
 * @param	aLink		Color Key Link
 * @return	ERR_OK
 */
static HERROR local_top_GetTOPLink (TTX_TOP_t *pTOP, const HUINT16 usPageNum, TTX_TOP_Link_t aLink[])
{
	HINT32		i		= 0;
	HERROR		err		= ERR_FAIL;
	HUINT16		usBase	= 0;
	BTT_t		*pBTT	= NULL;
	BTT_Link_t	*pLink	= NULL;
	HUINT16		usPrev, usNext, usBlock, usGroup;

	pBTT	= pTOP->pBTT;

	// 0~99를 800~899로 매핑하던 TTX 페이지 번호 방식을 TOP index인 0==100~799==899로 바꿔준다.
	usBase	= TTX_MXX_TO_TOP_MXX(usPageNum);
	usPrev	= usNext = usBlock = usGroup = 0;

	// KEY 'RED'
	err		= local_top_btt_FindPage(pBTT, usBase, &usPrev, FALSE);
	usPrev	= (err == ERR_OK) ? usPrev : (usBase == 0) ? 799 : usBase - 1;

	// KEY 'GREEN'
	err		= local_top_btt_FindPage(pBTT, usBase, &usNext,  TRUE);
	usNext	= (err == ERR_OK) ? usNext : (usBase + 1) % 800;

	// KEY 'YELLOW'
	err		= local_top_btt_FindBlock(pBTT, usBase + 1, &usBlock);
	if (err != ERR_OK)
	{
		usBlock	= (usBase + (10 - (usBase % 10))) % 800;	// 찾지 못하면 다음 10의 자리 숫자 를 다음 블록으로 본다.
	}

	// KEY 'BLUE'
	err		= local_top_btt_FindGroup(pBTT, usBlock, &usGroup);
	if (err != ERR_OK)
	{
		usGroup	= (usBase + (100 - (usBase % 100))) % 800;	// 찾지 못하면 다음 100의 자리 숫자를 다음 그룹으로 본다.
	}

	usPrev	= TOP_MXX_TO_TTX_MXX(usPrev);
	usNext	= TOP_MXX_TO_TTX_MXX(usNext);
	usBlock	= TOP_MXX_TO_TTX_MXX(usBlock);
	usGroup = TOP_MXX_TO_TTX_MXX(usGroup);

	aLink[0].bEnabled	= TRUE;		aLink[0].usPageNum	= usPrev;	aLink[0].usSubcode	= 0x3F7F;
	aLink[1].bEnabled	= TRUE;		aLink[1].usPageNum	= usNext;	aLink[1].usSubcode	= 0x3F7F;
	aLink[2].bEnabled	= aLink[3].bEnabled = FALSE;

	// Get Title From 'Additional Information Table'
	for (i = 0 ; i < TOP_MAX_BTT_LINK ; i++)
	{
		pLink	= &(pBTT->aLink[i]);
		if (pLink->type != eTTX_TOP_AIT || pLink->pvTable == NULL)
			continue;

		if (aLink[2].bEnabled == FALSE)	// Yellow
		{
			(void) local_top_ait_FindTitle(pLink->pvTable, usBlock, aLink + 2);
		}
		if (aLink[3].bEnabled == FALSE)	// Blue
		{
			(void) local_top_ait_FindTitle(pLink->pvTable, usGroup, aLink + 3);
		}
	}

	if (aLink[2].bEnabled == FALSE)	// Yellow
	{
		local_top_MakeTitle(aLink + 2, usBlock);
	}
	if (aLink[3].bEnabled == FALSE)	// Blue
	{
		local_top_MakeTitle(aLink + 3, usGroup);
	}

	return ERR_OK;
}



/**
 * Page number에 해당되는 페이지에 대해 TOP 정보로 부터 Trace 정보를 가져와 링크한다.
 *
 * @param	pTOP		instance of 'TOP'
 * @param	usPageNum	page number
 * @param	aLink		Color Key Link
 * @return	ERR_OK
 */
static HERROR local_top_GetTraceLink (TTX_TOP_t *pTOP, const HUINT16 usPageNum, HUINT16 aLink[])
{
	HINT32	i;
	HERROR	err;
	HUINT16	usTopMXX	= TTX_MXX_TO_TOP_MXX(usPageNum);

	err	= local_top_btt_FindPage(pTOP->pBTT, usTopMXX, &(aLink[0]), TRUE);	// 다음
	if (err != ERR_OK)
		return ERR_FAIL;

	err	= local_top_btt_FindPage(pTOP->pBTT, aLink[0], &(aLink[1]), TRUE);	// 다다음

	if (err != ERR_OK)
		return ERR_FAIL;

	err	= local_top_btt_FindPage(pTOP->pBTT, aLink[1], &(aLink[2]), TRUE);	// 다다다음
	if (err != ERR_OK)
		return ERR_FAIL;

	err	= local_top_btt_FindPage(pTOP->pBTT, usTopMXX, &(aLink[3]), FALSE); // 이전
	if (err != ERR_OK)
		return ERR_FAIL;

	for (i = 0 ; i < 4 ; i++)
		aLink[i]	= TOP_MXX_TO_TTX_MXX(aLink[i]);

	return ERR_OK;
}


/**
 * Page number와 Subcode에 해당되는 페이지가 Block or Group인지 판단
 *
 * @param	pTOP		instance of 'TOP'
 * @param	usPageNum	page number
 * @param	usSubcode	subcode
 * @return	ERR_OK / ERR_FAIL
 */
static HERROR local_top_IsBlockOrGroup (TTX_TOP_t *pTOP, const HUINT16 usPageNum, const HUINT16 usSubcode)
{
	BTT_Property_t	property;

	(void)usSubcode;

	property	= local_top_btt_GetProperty(pTOP->pBTT, TTX_MXX_TO_TOP_MXX(usPageNum));

	if (property <= eTOP_BTT_GROUP)
	{
		return ERR_OK;
	}

	return ERR_FAIL;
}


/**
 * Page number와 Subcode에 해당되는 페이지가 BTT에 존재하는지 판단
 *
 * @param	pTOP		instance of 'TOP'
 * @param	usPageNum	page number
 * @param	usSubcode	subcode
 * @return	ERR_OK / ERR_FAIL
 */
static HERROR local_top_IsPageExist (TTX_TOP_t *pTOP, const HUINT16 usPageNum, const HUINT16 usSubcode)
{
	BTT_Property_t	property;

	(void)usSubcode;

	property	= local_top_btt_GetProperty(pTOP->pBTT, TTX_MXX_TO_TOP_MXX(usPageNum));

	if (property != eTOP_BTT_NO_PAGE)
	{
		return ERR_OK;
	}

	return ERR_FAIL;
}

#define ____TOP_PUBLIC____


Handle_t TTX_TOP_Create (void)
{
	TTX_TOP_t	*pTOP;

#if defined(CONFIG_MW_TTX_TOP)
	pTOP	= (TTX_TOP_t *)AP_Malloc(sizeof(TTX_TOP_t));
	if (pTOP == NULL)
	{
		return HANDLE_NULL;
	}
#else
	pTOP	= (TTX_TOP_t *)HANDLE_NULL;
#endif

	return (Handle_t)pTOP;
}


void TTX_TOP_Destroy (Handle_t hTOP)
{
	HINT32	i;
	TTX_TOP_t	*pTOP	= (TTX_TOP_t *)hTOP;
	BTT_t		*pBTT;

	if (hTOP == HANDLE_NULL || pTOP == NULL)
	{
		return;
	}

	pBTT	= pTOP->pBTT;
	if (pBTT)
	{
		for (i = 0 ; i < TOP_MAX_BTT_LINK ; i++)
		{
			if (pBTT->aLink[i].pvTable)
				AP_Free(pBTT->aLink[i].pvTable);
		}
	}
	AP_Free(pTOP);
}


TTX_TOP_Type_t TTX_TOP_GetType (Handle_t hTOP, const HUINT16 usPageNum)
{
	HINT32	i;
	TTX_TOP_t	*pTOP	= (TTX_TOP_t *)hTOP;
	BTT_t		*pBTT;

	if (hTOP == HANDLE_NULL || pTOP == NULL)
	{
		return eTTX_TOP_UNKNOWN;
	}

	if (usPageNum == 0x01F0)
	{
		return eTTX_TOP_BTT;
	}

	pBTT	= pTOP->pBTT;
	if (pBTT == NULL)
	{
		return eTTX_TOP_UNKNOWN;
	}

	for (i = 0 ; i < TOP_MAX_BTT_LINK ; i++)
	{
		if (pBTT->aLink[i].usPageNum == usPageNum)
		{
			return pBTT->aLink[i].type;
		}
	}

	return eTTX_TOP_UNKNOWN;
}


HUINT32 TTX_TOP_GetCurVersion (Handle_t hTOP)
{
	TTX_TOP_t	*pTOP	= (TTX_TOP_t *)hTOP;

	if (hTOP == HANDLE_NULL || pTOP == NULL)
	{
		return ERR_TOP_INVALID_ARGUMENT;
	}

	return pTOP->ulVersion;
}


HERROR TTX_TOP_Receive (Handle_t hTOP, TTX_TOP_Table_t *pTable)
{
	TTX_TOP_t	*pTOP	= (TTX_TOP_t *)hTOP;

	if (hTOP == HANDLE_NULL || pTOP == NULL || pTable == NULL)
	{
		return ERR_TOP_INVALID_ARGUMENT;
	}

	pTable->type	= TTX_TOP_GetType(hTOP, pTable->usPageNum);
	if (pTable->type == eTTX_TOP_UNKNOWN)
	{
		return ERR_FAIL;
	}

	return local_top_Receive(pTOP, pTable);
}


HERROR TTX_TOP_GetTOPLink (Handle_t hTOP, const HUINT16 usPageNum, TTX_TOP_Link_t aLink[])
{
	TTX_TOP_t	*pTOP	= (TTX_TOP_t *)hTOP;

	if (hTOP == HANDLE_NULL || pTOP == NULL || aLink == NULL)
	{
		return ERR_TOP_INVALID_ARGUMENT;
	}

	if (pTOP->pBTT == NULL)
	{
		return ERR_TOP_NO_BTT;
	}

	return local_top_GetTOPLink(pTOP, usPageNum % 800, aLink);
}


HERROR TTX_TOP_GetTraceLink (Handle_t hTOP, const HUINT16 usPageNum, HUINT16 ausTrace[])
{
	TTX_TOP_t	*pTOP	= (TTX_TOP_t *)hTOP;

	if (hTOP == HANDLE_NULL || pTOP == NULL || ausTrace == NULL)
	{
		return ERR_TOP_INVALID_ARGUMENT;
	}

	if (pTOP->pBTT == NULL)
	{
		return ERR_TOP_NO_BTT;
	}

	return local_top_GetTraceLink(pTOP, usPageNum % 800, ausTrace);
}


HERROR TTX_TOP_IsBlockOrGroup (Handle_t hTOP, const HUINT16 usPageNum, const HUINT16 usSubcode)
{
	TTX_TOP_t	*pTOP	= (TTX_TOP_t *)hTOP;

	if (hTOP == HANDLE_NULL || pTOP == NULL)
	{
		return ERR_TOP_INVALID_ARGUMENT;
	}

	if (pTOP->pBTT == NULL)
	{
		return ERR_TOP_NO_BTT;
	}

	return local_top_IsBlockOrGroup(pTOP, usPageNum, usSubcode);
}


HERROR TTX_TOP_IsPageExist (Handle_t hTOP, const HUINT16 usPageNum, const HUINT16 usSubcode)
{
	TTX_TOP_t	*pTOP	= (TTX_TOP_t *)hTOP;

	if (hTOP == HANDLE_NULL || pTOP == NULL)
	{
		return ERR_TOP_INVALID_ARGUMENT;
	}

	if (pTOP->pBTT == NULL)
	{
		return ERR_TOP_NO_BTT;
	}

	return local_top_IsPageExist(pTOP, usPageNum, usSubcode);
}

// end of file

