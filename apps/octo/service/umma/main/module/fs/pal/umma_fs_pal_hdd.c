/**
	@file     	pal_int_stor.c
	@brief    	internal storage event handler

	Description: 		\n
	Module: 		pal/fs			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include "umma_fs_pal_fs.h"
#include "umma_fs_pal_hdd.h"

#include "vkernel.h"
#include "htype.h"

#include "di_hdd.h"
#include	"se_uapi.h"

#ifdef CONFIG_DEBUG
#define	_DEBUG_
//#define	_DEBUG_ENTER_EXIT_
#endif

static PAL_FS_Event_Callback_Func		s_pfPalIntStor_NotifyCb;
static HBOOL s_b2ndIntFound = FALSE;

STATIC void local_palint_ExecuteCallback (HUINT32 ulEvent, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	if (s_pfPalIntStor_NotifyCb != NULL)
	{
		(s_pfPalIntStor_NotifyCb)(ulEvent, ulParam1, ulParam2, ulParam3);
	}
}

STATIC HERROR local_palint_GetDevIndexByDevName (char *szDevName, PAL_FS_StorDevIdx_t *peDevIdx)
{
	HUINT32		 ulIndex, ulStrLen;
	HUINT8		 szDevNode[16];
	HUINT8		*pucStr;

	if (szDevName == NULL || peDevIdx == NULL)			{ return ERR_FAIL; }

	VK_memset (szDevNode, 0, 16);
	VK_strncpy (szDevNode, szDevName, 16);
	szDevNode[5] = '\0';

	ulStrLen = VK_strlen (szDevNode);
	for (ulIndex = 0; ulIndex < ulStrLen; ulIndex ++)
	{
		if (szDevNode[ulIndex] >= 'A' && szDevNode[ulIndex] <= 'Z')
		{
			szDevNode[ulIndex] = (szDevNode[ulIndex] - 'A') + 'a';
		}
	}

	pucStr = strstr (szDevNode, "sd");
	if (pucStr != NULL)
	{
		if (pucStr[2] >= 'a' && pucStr[2] <= 'z')
		{
			*peDevIdx = (pucStr[2] - 'a') + ePAL_FS_DEV_sda;
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

HERROR UMMA_FS_PAL_HDD_RegisterCallback (PAL_FS_Event_Callback_Func pfEvtCallback)
{
	if (pfEvtCallback != NULL)
	{
		s_pfPalIntStor_NotifyCb = pfEvtCallback;
	}

	return ERR_OK;
}

DI_ERR_CODE		UMMA_FS_PAL_HDD_EventNotify(DI_HDD_Event_t etEventType, void *pucHddInfo)
{
	HUINT32				 ulEvent, ulParam1, ulParam2, ulParam3;
	PAL_FS_StorDevIdx_t	 eDevIdx;
	hdd_info_t			*pHddInfo = (hdd_info_t *)pucHddInfo;
	HERROR				 hErr;

	if (pucHddInfo == NULL)
	{
		HxLOG_Error("no HDD info, invalid event, ignored!\n");
		return DI_ERR_ERROR;
	}

	// start propagation to notice HDD power changes
	if (etEventType == DI_HDD_Event_Pwron)
	{
		if (pHddInfo->devCount > 0)
		{
			// pHddInfo->devName 는 'sda' 'sdb' 등의 string이 온다.
			// 이를 PAL_FS_StorDevIdx_t 로 변환하여 M/W 단으로 올려주도록 한다.
			hErr = local_palint_GetDevIndexByDevName (pHddInfo->devName, &eDevIdx);
			if (hErr != ERR_OK)
			{
				HxLOG_Print("[UMMA_FS_PAL_HDD_EventNotify] wrong device name (%s)\n", pHddInfo->devName);
				return DI_ERR_ERROR;
			}

			ulEvent		= PAL_FS_EVENT_HDD_ON;
			ulParam1	= (HUINT32)eDevIdx;
			ulParam2	= (HUINT32)pHddInfo->portnum; // devCount는 의미가 없다. pHddInfo->devCount;
			ulParam3	= (HUINT32)0;

			local_palint_ExecuteCallback (ulEvent, ulParam1, ulParam2, ulParam3);
		}
		else
		{
			HxLOG_Print("HDD powered-On, but no info.\n");
			return DI_ERR_ERROR;
		}
	}
	else if (etEventType == DI_HDD_Event_Pwroff)
	{
		// pHddInfo->devName 는 'sda' 'sdb' 등의 string이 온다.
		// 이를 PAL_FS_StorDevIdx_t 로 변환하여 M/W 단으로 올려주도록 한다.
		hErr = local_palint_GetDevIndexByDevName (pHddInfo->devName, &eDevIdx);
		if (hErr != ERR_OK)
		{
			HxLOG_Print("[UMMA_FS_PAL_HDD_EventNotify] wrong device name (%s)\n", pHddInfo->devName);
			return DI_ERR_ERROR;
		}

		ulEvent		= PAL_FS_EVENT_HDD_OFF;
		ulParam1	= (HUINT32)eDevIdx;
		ulParam2	= (HUINT32)pHddInfo->portnum; // devCount는 의미가 없다. pHddInfo->devCount;
		ulParam3	= (HUINT32)0;

		local_palint_ExecuteCallback (ulEvent, ulParam1, ulParam2, ulParam3);
	}
	else
	{
		HxLOG_Print("Unknown event.\n");
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}

int 	UMMA_FS_PAL_HDD_Init(void)
{
#ifndef WIN32
	SEUAPI_HDD_Initialize();
	SEUAPI_HDD_RegisterCallback(UMMA_FS_PAL_HDD_EventNotify);

#endif

	return ERR_OK;
}

int		UMMA_FS_PAL_HDD_PWR_On(void)
{
#ifndef	WIN32
	SEUAPI_HDD_PWR_Ctrl(DI_HDD_PWR_ON);
#endif

	return ERR_OK;
}

int		UMMA_FS_PAL_HDD_PWR_Off(void)
{
#ifndef	WIN32
	SEUAPI_HDD_PWR_Ctrl(DI_HDD_PWR_OFF);
#endif

	return ERR_OK;
}

HERROR UMMA_FS_PAL_HDD_ControlPower(PAL_HDD_PortNum_t ePortnum, HBOOL bOn)
{
	DI_HDD_PORT_NUM_E eDiPortnum;
	DI_HDD_PWR bDiPwr;

	if (ePortnum == PAL_HDD_PORT_NUM_1)
	{
		eDiPortnum = DI_HDD_PORT_NUM_1;
	}
	else if (ePortnum == PAL_HDD_PORT_NUM_2)
	{
		eDiPortnum = DI_HDD_PORT_NUM_2;
	}
	else if (ePortnum == PAL_HDD_PORT_NUM_3)
	{
		eDiPortnum = DI_HDD_PORT_NUM_3;
	}
	else
	{
		return ERR_FAIL;
	}

	if (bOn)
	{
		bDiPwr = DI_HDD_PWR_ON;
	}
	else
	{
		bDiPwr = DI_HDD_PWR_OFF;
	}

	return SEUAPI_HDD_ControlPower(eDiPortnum, bDiPwr);

}


#if defined(CONFIG_MW_SECURITY_FILE_SYSTEM)
int 		UMMA_FS_PAL_HDD_GetEncryptionKey(PAL_HDD_PortNum_t ePortNum, HINT8 *pKey)
{
	if (pKey == NULL)
		return ERR_FAIL;

	return SEUAPI_HDD_GetEncryptionKey((DI_HDD_PORT_NUM_E)ePortNum, pKey);
}
#endif


void UMMA_FS_PAL_HDD_Check2ndIntHdd(void)
{
	// 정해진 시간 이내에 2nd int hdd가 발견되지 않으면 2nd int hdd용 power 공급을 차단한다.
	if (s_b2ndIntFound == FALSE)
	{
		SEUAPI_HDD_ControlPower(DI_HDD_PORT_NUM_2, DI_HDD_PWR_OFF);
	}
}

void UMMA_FS_PAL_HDD_2ndIntHddDetected(HBOOL bExist)
{
	if (bExist)
	{
		s_b2ndIntFound = TRUE;
	}
	else
	{
		// 2nd int hdd가 제거 되면 2nd int hdd용 power 공급을 차단한다.
		s_b2ndIntFound = FALSE;
		SEUAPI_HDD_ControlPower(DI_HDD_PORT_NUM_2, DI_HDD_PWR_OFF);
	}
}


