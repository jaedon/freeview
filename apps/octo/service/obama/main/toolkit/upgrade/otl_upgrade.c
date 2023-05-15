/*******************************************************************
	File Description
********************************************************************/
/**
	@file	otl_upgrade.c
	@brief	otl_upgrade.c

	Description:
	Module: MW / \n

	Copyright (c) 2011 HUMAX Co., Ltd.								\n
	All rights reserved.											\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <otl.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#define	OTL_UPGRADE_MAX_UPGRADE_INST		10
#define	OTL_UPGRADE_SLOT_EMPTY				0

/*******************************************************************************/
/***************************  Typedefs           *******************************/
/*******************************************************************************/
typedef struct tagUpgradeInst
{
	Handle_t			 hUpgrade;
	HUINT32				 ulTag;
	OtlUpgrade_Info_t	 stInfo;
} otlUpgrade_Inst_t;

/*******************************************************************************/
/***************************  Static Variables   ******************************/
/*******************************************************************************/
STATIC otlUpgrade_Inst_t		s_astUpgradeInst[OTL_UPGRADE_MAX_UPGRADE_INST];

/*******************************************************************************/
/***************************  Functions          *******************************/
/*******************************************************************************/

/*******************************************************************************/
/***************************  Local Functions    *******************************/
/*******************************************************************************/

STATIC otlUpgrade_Inst_t *otl_upgrade_FindInstByTag(HUINT32 ulTag)
{
	HUINT32	i;

	for (i=0; i<OTL_UPGRADE_MAX_UPGRADE_INST; i++)
	{
		if (ulTag == s_astUpgradeInst[i].ulTag)
		{
			return &s_astUpgradeInst[i];
		}
	}

	return NULL;
}

STATIC otlUpgrade_Inst_t *otl_upgrade_FindInstByHandle(Handle_t hUpgrade)
{
	HUINT32	i;

	for (i=0; i<OTL_UPGRADE_MAX_UPGRADE_INST; i++)
	{
		if (hUpgrade == s_astUpgradeInst[i].hUpgrade)
		{
			return &s_astUpgradeInst[i];
		}
	}

	return NULL;
}

STATIC INLINE otlUpgrade_Inst_t *otl_upgrade_FindEmptyInst(void)
{
	return otl_upgrade_FindInstByTag(OTL_UPGRADE_SLOT_EMPTY);
}

STATIC HERROR otl_upgrade_GetUserData(otlUpgrade_Inst_t *pstInst)
{
	HERROR	 hErr;
	void	*pvUserData;

	if (NULL == pstInst)
		return ERR_FAIL;

	if (NULL == pstInst->stInfo.fnGetUserData)
		return ERR_FAIL;

	pvUserData = NULL;
	hErr = pstInst->stInfo.fnGetUserData(&pvUserData);

	if (ERR_OK != hErr || NULL == pvUserData)
		return ERR_FAIL;

	pstInst->stInfo.pvUserData = pvUserData;

	return ERR_OK;
}

STATIC HBOOL	otl_upgrade_CheckValidInfo(OtlUpgrade_Info_t *pstInfo)
{
	if (NULL == pstInfo)
		return FALSE;

	if (NULL == pstInfo->pvUserData)
		return FALSE;

	if (NULL == pstInfo->fnInit)
		return FALSE;

	if (NULL == pstInfo->fnGetUserData)
		return FALSE;

	if (NULL == pstInfo->fnSetUserData)
		return FALSE;

	if (NULL == pstInfo->fnCheckComplete)
		return FALSE;

	if (NULL == pstInfo->fnOpenTarget)
		return FALSE;

	if (NULL == pstInfo->fnCloseTarget)
		return FALSE;

	if (NULL == pstInfo->fnRead)
		return FALSE;

	if (NULL == pstInfo->fnWrite)
		return FALSE;

	return TRUE;
}

/*******************************************************************************/
/****************************** Global Functions *******************************/
/*******************************************************************************/

HERROR		OTL_UPGRADE_Init(void)
{
	HxSTD_MemSet(s_astUpgradeInst, 0, sizeof(otlUpgrade_Inst_t)*OTL_UPGRADE_MAX_UPGRADE_INST);

	return ERR_OK;
}

Handle_t	OTL_UPGRADE_Open(HUINT32 ulTag, OtlUpgrade_Info_t *pstInfo)
{
	HERROR				 hErr;
	otlUpgrade_Inst_t	*pstInst;

	if (OTL_UPGRADE_SLOT_EMPTY == ulTag)
		return UPGRADE_HANDLE_INVALID;

	if (otl_upgrade_CheckValidInfo(pstInfo) == FALSE)
		return UPGRADE_HANDLE_INVALID;

	pstInst = otl_upgrade_FindInstByTag(ulTag);
	if (NULL != pstInst)
		return UPGRADE_HANDLE_INVALID;

	pstInst = otl_upgrade_FindEmptyInst();
	if (NULL == pstInst)
		return UPGRADE_HANDLE_INVALID;

	HxSTD_MemSet(pstInst, 0, sizeof(otlUpgrade_Inst_t));
	HxSTD_MemCopy(&pstInst->stInfo, pstInfo, sizeof(OtlUpgrade_Info_t));

	pstInst->hUpgrade		= UPGRADE_HANDLE_INVALID;

	// Init
	hErr = pstInst->stInfo.fnInit();
	if (ERR_OK != hErr)
		return UPGRADE_HANDLE_INVALID;

	// Read User Data
	hErr = otl_upgrade_GetUserData(pstInst);
	if (ERR_OK != hErr)
		return UPGRADE_HANDLE_INVALID;

	// Create Destination File or Open Destination Device
	hErr = pstInst->stInfo.fnOpenTarget(ulTag, pstInst->stInfo.pvUserData);
	if (ERR_OK != hErr)
		return UPGRADE_HANDLE_INVALID;

	pstInst->ulTag			= ulTag;
	pstInst->hUpgrade		= (Handle_t)pstInst;

	return pstInst->hUpgrade;
}

HERROR		OTL_UPGRADE_Close(Handle_t hUpgrade)
{
	HERROR				hErr;
	otlUpgrade_Inst_t	*pstInst;

	pstInst = otl_upgrade_FindInstByHandle(hUpgrade);
	if (NULL == pstInst)
	{
		return ERR_FAIL;
	}

	// Flush
	hErr = pstInst->stInfo.fnSetUserData(pstInst->stInfo.pvUserData);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("setUserData failed. \n");
	}

	// Close
	if (pstInst->stInfo.fnCloseTarget)
	{
		hErr = pstInst->stInfo.fnCloseTarget(pstInst->stInfo.pvUserData);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("closeTarget failed. \n");
		}
	}

	pstInst->stInfo.pvUserData = NULL;

	HxSTD_MemSet(pstInst, 0, sizeof(otlUpgrade_Inst_t));
	pstInst->ulTag			= OTL_UPGRADE_SLOT_EMPTY;
	pstInst->hUpgrade		= UPGRADE_HANDLE_INVALID;

	return ERR_OK;
}

HERROR		OTL_UPGRADE_GetUserData(Handle_t hUpgrade, void **ppvUserData)
{
	HERROR				 hErr;
	otlUpgrade_Inst_t	*pstInst;

	if (NULL == ppvUserData)
		return ERR_FAIL;

	pstInst = otl_upgrade_FindInstByHandle(hUpgrade);
	if (NULL == pstInst)
	{
		return ERR_FAIL;
	}

	if (NULL == pstInst->stInfo.fnGetUserData)
	{
		return ERR_FAIL;
	}

	hErr = pstInst->stInfo.fnGetUserData(ppvUserData);
	if (ERR_OK != hErr)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR		OTL_UPGRADE_SetUserData(Handle_t hUpgrade, void *pvUserData)
{
	HERROR				 hErr;
	otlUpgrade_Inst_t	*pstInst;

	if (NULL == pvUserData)
		return ERR_FAIL;

	pstInst = otl_upgrade_FindInstByHandle(hUpgrade);
	if (NULL == pstInst)
	{
		return ERR_FAIL;
	}

	if (NULL == pstInst->stInfo.fnSetUserData)
	{
		return ERR_FAIL;
	}

	hErr = pstInst->stInfo.fnSetUserData(pvUserData);
	if (ERR_OK != hErr)
	{
		return ERR_FAIL;
	}

	pstInst->stInfo.pvUserData = pvUserData;

	return ERR_OK;
}


HINT32		OTL_UPGRADE_Read(Handle_t hUpgrade, HUINT8 *pucBuff, HUINT32 ulOffset, HUINT32 ulSize)
{
	otlUpgrade_Inst_t	*pstInst;

	pstInst = otl_upgrade_FindInstByHandle(hUpgrade);
	if (NULL == pstInst)
	{
		return -1;
	}

	if (NULL == pstInst->stInfo.fnRead)
	{
		return -1;
	}

	otl_upgrade_GetUserData(pstInst);

	return pstInst->stInfo.fnRead(pstInst->stInfo.pvUserData, pucBuff, ulOffset, ulSize);
}

HINT32		OTL_UPGRADE_Write(Handle_t hUpgrade, const HUINT8 *pucBuff, HUINT32 ulOffset, HUINT32 ulSize, HBOOL *pbComplete)
{
	HERROR				 hErr;
	HINT32				 nRet;
	otlUpgrade_Inst_t	*pstInst;

	pstInst = otl_upgrade_FindInstByHandle(hUpgrade);
	if (NULL == pstInst)
	{
		return -1;
	}

	if (NULL == pstInst->stInfo.fnWrite)
	{
		return -1;
	}

	nRet = pstInst->stInfo.fnWrite(pstInst->stInfo.pvUserData, (const HUINT8 *)pucBuff, ulOffset, ulSize);
	if (nRet == ERR_OK)
	{
		if (pstInst->stInfo.fnSetUserData)
		{
			hErr = pstInst->stInfo.fnSetUserData(pstInst->stInfo.pvUserData);
			if (hErr != ERR_OK)
			{
				//HxLOG_Error("setUserData failed. hErr(%d) \n",hErr);
			}
		}

		if (pbComplete)
		{
			*pbComplete = FALSE;
			if (pstInst->stInfo.fnCheckComplete)
			{
				*pbComplete = pstInst->stInfo.fnCheckComplete(pstInst->stInfo.pvUserData);
			}
		}
	}

	return nRet;
}

HBOOL OTL_UPGRADE_CheckComplete(Handle_t hUpgrade)
{
	otlUpgrade_Inst_t	*pstInst;

	pstInst = otl_upgrade_FindInstByHandle(hUpgrade);
	if (NULL == pstInst)
	{
		return FALSE;
	}

	if (pstInst->stInfo.fnCheckComplete)
	{
		return pstInst->stInfo.fnCheckComplete(pstInst->stInfo.pvUserData);
	}

	return FALSE;
}

/*********************** End of File ******************************/
