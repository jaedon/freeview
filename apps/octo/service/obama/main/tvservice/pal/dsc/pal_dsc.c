/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  	pal_dsc.c
	@brief	  descramble.

	Description: 	\n
	Module: PAL/DSC			 	\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#include <octo_common.h>

#include <pal_dsc.h>

#include <di_demux.h>
#include <di_dsc.h>

#if (defined(CONFIG_MW_CAS) || defined(CONFIG_MW_CI_PLUS))

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#define palDsc_MAX_NUM_OF_ID			60
#define palDsc_MAX_NUM_OF_DMX 			1

#define palDsc_USED						TRUE
#define palDsc_NOT_USED					FALSE
#define palDsc_RESOURCE_NONE			0xffffffff

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	HBOOL 						bUsed;
	PalDsc_DscType_e 			eDscType;
	PalDsc_DataType_e			eDscDataType;
	HUINT32 					ulDemuxId;
	HUINT32 					ulDscId;		// DI 에서 사용하는 ID
	HUINT32						ulDscIndex;	// MW 에서 사용하는 논리적인 ID (0, 1, 2, ... 순으로 할당된다)
	HUINT16 					usPid;
} palDsc_Info_t;

/*
	Descrambler
	1. CI+
	2. DVB Normal
	3.
*/
/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC palDsc_Info_t		s_astDscInfo[palDsc_MAX_NUM_OF_ID];

/********************************************************************/
/********************      proto-type functions  ********************/
/********************************************************************/



/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
HERROR PAL_DSC_Init(void)
{
	HUINT32 i;

	HxLOG_Info(" \n");

	for(i = 0; i < palDsc_MAX_NUM_OF_ID; i++)
	{
		s_astDscInfo[i].bUsed = palDsc_NOT_USED;
		s_astDscInfo[i].eDscType = ePAL_DSC_DSCTYPE_NONE;
		s_astDscInfo[i].eDscDataType = ePAL_DSC_DATATYPE_NONE;
		s_astDscInfo[i].ulDemuxId = palDsc_RESOURCE_NONE;
		s_astDscInfo[i].ulDscId = palDsc_RESOURCE_NONE;
		s_astDscInfo[i].ulDscIndex = palDsc_RESOURCE_NONE;
		s_astDscInfo[i].usPid = PID_NULL;
	}

	return ERR_OK;
}

STATIC palDsc_Info_t *PAL_DSC_Alloc(HUINT32 ulDemuxId, HUINT32 *pulDscIndex)
{
	HUINT32 i;

	for(i = 0; i < palDsc_MAX_NUM_OF_ID; i++)
	{
		if(s_astDscInfo[i].bUsed == palDsc_NOT_USED)
		{
			*pulDscIndex = i;
			return &s_astDscInfo[i];
		}
	}

	*pulDscIndex = palDsc_RESOURCE_NONE;

	HxLOG_Error(" \n");

	return NULL;
}

STATIC palDsc_Info_t *PAL_DSC_Find(HUINT32 ulDscIndex)
{
	if(ulDscIndex >= palDsc_MAX_NUM_OF_ID)
	{
		HxLOG_Error(" \n");
		return NULL;
	}

	return &s_astDscInfo[ulDscIndex];
}

STATIC HERROR PAL_DSC_Free(HUINT32 ulDscIdx)
{
	if(ulDscIdx >= palDsc_MAX_NUM_OF_ID)
	{
		HxLOG_Error(" \n");
		return ERR_FAIL;
	}

	s_astDscInfo[ulDscIdx].bUsed = palDsc_NOT_USED;
	s_astDscInfo[ulDscIdx].eDscType = ePAL_DSC_DSCTYPE_NONE;
	s_astDscInfo[ulDscIdx].eDscDataType = ePAL_DSC_DATATYPE_NONE;
	s_astDscInfo[ulDscIdx].ulDemuxId = palDsc_RESOURCE_NONE;
	s_astDscInfo[ulDscIdx].ulDscId = palDsc_RESOURCE_NONE;
	s_astDscInfo[ulDscIdx].ulDscIndex = palDsc_RESOURCE_NONE;
	s_astDscInfo[ulDscIdx].usPid = PID_NULL;

	return ERR_OK;
}

HERROR PAL_DSC_Open(HUINT32 ulDemuxId, PalDsc_DscType_e eDscType, PalDsc_DataType_e eDataType, HUINT32 *pulDscIndex)
{
	HUINT32 ulDscId = palDsc_RESOURCE_NONE;
	HERROR nResult = ERR_FAIL;
	palDsc_Info_t *pDsc = NULL;
	HUINT32 ulDscIndex = palDsc_RESOURCE_NONE;
	DI_DSC_DESC_TYPE eDiDscType = DI_DSC_DESC_DVB;

	HxLOG_Info("DmxId(%d), DscType(%d), DataType(%d) \n", ulDemuxId, eDscType, eDataType);

	// Param Check
	if(ulDemuxId == palDsc_RESOURCE_NONE || pulDscIndex == NULL)
	{
		HxLOG_Error(" \n");
		return ERR_FAIL;
	}

	if(eDscType == ePAL_DSC_DSCTYPE_NONE || eDataType == ePAL_DSC_DATATYPE_NONE)
	{
		HxLOG_Error(" \n");
		return ERR_FAIL;
	}

	switch (eDscType)
	{
		case ePAL_DSC_DSCTYPE_DVB:
			eDiDscType = DI_DSC_DESC_DVB;
			break;

		case ePAL_DSC_DSCTYPE_AES_CLEAR:
			eDiDscType = DI_DSC_DESC_AES_CLEARMODE;					// BSPARK : CI Plus Descrambler Setting.
			break;

		case ePAL_DSC_DSCTYPE_AES_BLOCK:
			eDiDscType = DI_DSC_DESC_AES;
			break;

		case ePAL_DSC_DSCTYPE_DES_CLEAR:
			eDiDscType = DI_DSC_DESC_DES_CLEARMODE;
			break;

		case ePAL_DSC_DSCTYPE_DES_BLOCK:
			eDiDscType = DI_DSC_DESC_DES;
			break;

		case ePAL_DSC_DSCTYPE_NONE:
		case ePAL_DSC_DSCTYPE_MAX:
			break;
	}

	nResult = DI_DSC_Open(ulDemuxId, eDiDscType, &ulDscId, NULL);
	if(nResult != ERR_OK)
	{
		HxLOG_Error(" \n");
		return ERR_FAIL;
	}

	pDsc = PAL_DSC_Alloc(ulDemuxId, &ulDscIndex);
	if(pDsc == NULL)
	{
		HxLOG_Error(" \n");
		return ERR_FAIL;
	}

	// Dsc Status Update
	pDsc->bUsed = palDsc_USED;
	pDsc->eDscType = eDscType;
	pDsc->eDscDataType = eDataType;
	pDsc->ulDemuxId = ulDemuxId;
	pDsc->ulDscId = ulDscId;
	pDsc->ulDscIndex = ulDscIndex;

	// Mapping DscId
	*pulDscIndex = ulDscIndex;

	return ERR_OK;
}

HERROR PAL_DSC_CloseAll(void)
{
	HUINT32 i;
	HERROR nResult = ERR_FAIL;

	for(i = 0; i < palDsc_MAX_NUM_OF_ID; i++)
	{
		if (s_astDscInfo[i].bUsed == palDsc_USED)
		{
			if (s_astDscInfo[i].eDscType == ePAL_DSC_DSCTYPE_NONE
				|| s_astDscInfo[i].eDscDataType == ePAL_DSC_DATATYPE_NONE
				|| s_astDscInfo[i].ulDemuxId == palDsc_RESOURCE_NONE
				|| s_astDscInfo[i].ulDscId == palDsc_RESOURCE_NONE
				|| s_astDscInfo[i].ulDscIndex == palDsc_RESOURCE_NONE
				|| s_astDscInfo[i].usPid == PID_NULL)
			{
				HxLOG_Error(" \n");
			}
			else
			{
				nResult = PAL_DSC_Close(s_astDscInfo[i].ulDscIndex);
				if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");
			}
		}
	}

	return ERR_OK;
}

HERROR PAL_DSC_Close(HUINT32 ulDscIndex)
{
	HERROR nResult = ERR_FAIL;
	palDsc_Info_t *pDsc = NULL;

	HxLOG_Info("Desc Index (%d)  \n", ulDscIndex);

	// Param Check
	if(ulDscIndex >= palDsc_MAX_NUM_OF_ID)
	{
		HxLOG_Error("ulDscIndex %d > palDsc_MAX_NUM_OF_ID %d\r\n", ulDscIndex, palDsc_MAX_NUM_OF_ID);
		return ERR_FAIL;
	}

	pDsc = PAL_DSC_Find(ulDscIndex);
	if(pDsc == NULL)
	{
		HxLOG_Error(" \n");
		return ERR_FAIL;
	}

	if (pDsc->usPid != PID_NULL)
	{
		nResult = DI_DSC_ClearPid(pDsc->ulDscId, pDsc->usPid);
		if(nResult != ERR_OK)
		{
			HxLOG_Error(" \n");
			return ERR_FAIL;
		}
	}

	nResult = DI_DSC_Close(pDsc->ulDscId);
	if(nResult != ERR_OK)
	{
		HxLOG_Error(" \n");
		return ERR_FAIL;
	}

	// Dsc Status Update
	nResult = PAL_DSC_Free(ulDscIndex);
	if(nResult != ERR_OK)
	{
		HxLOG_Error(" \n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR PAL_DSC_SetPid(HUINT32 ulDscIndex, HUINT16 usPid)
{
	HERROR nResult = ERR_FAIL;
	palDsc_Info_t *pDsc = NULL;

	HxLOG_Info("Desc Index (%d) Pid (0x%x) \n", ulDscIndex, usPid);

	// Param Check
	if(ulDscIndex >= palDsc_MAX_NUM_OF_ID)
	{
		HxLOG_Error("ulDscIndex %d > palDsc_MAX_NUM_OF_ID %d\r\n", ulDscIndex, palDsc_MAX_NUM_OF_ID);
		return ERR_FAIL;
	}

	if(usPid == PID_NULL)
	{
		HxLOG_Error(" \n");
		return ERR_FAIL;
	}

	pDsc = PAL_DSC_Find(ulDscIndex);
	if(pDsc == NULL)
	{
		HxLOG_Error(" \n");
		return ERR_FAIL;
	}

	nResult = DI_DSC_SetPid(pDsc->ulDscId, usPid);
	if(nResult != ERR_OK)
	{
		HxLOG_Error(" \n");
		return ERR_FAIL;
	}

	// Dsc Status Update
	pDsc->usPid = usPid;

	return ERR_OK;
}

HERROR PAL_DSC_ResetPid(HUINT32 ulDscIndex, HUINT16 usPid)
{
	HERROR nResult = ERR_FAIL;
	palDsc_Info_t *pDsc = NULL;

	HxLOG_Info("Desc Index (%d) Pid (0x%x) \n", ulDscIndex, usPid);

	// Param Check
	if(ulDscIndex >= palDsc_MAX_NUM_OF_ID)
	{
		HxLOG_Error("ulDscIndex %d > palDsc_MAX_NUM_OF_ID %d\r\n", ulDscIndex, palDsc_MAX_NUM_OF_ID);
		return ERR_FAIL;
	}

	if(usPid == PID_NULL)
	{
		HxLOG_Error(" \n");
		return ERR_FAIL;
	}

	pDsc = PAL_DSC_Find(ulDscIndex);
	if(pDsc == NULL)
	{
		HxLOG_Error(" \n");
		return ERR_FAIL;
	}

	nResult = DI_DSC_ClearPid(pDsc->ulDscId, usPid);
	if(nResult != ERR_OK)
	{
		HxLOG_Error(" \n");
		return ERR_FAIL;
	}

	// Dsc Status Update
	pDsc->usPid = PID_NULL;

	return ERR_OK;
}

HERROR PAL_DSC_SetKey(HUINT32 ulDscIndex, PalDsc_KeyParity_e eParity, HUINT8 *pucKeyData)
{
	HERROR nResult = ERR_FAIL;
	palDsc_Info_t *pDsc = NULL;

	HxLOG_Info("Desc Index (%d) \n", ulDscIndex);

	// Param Check
	if(ulDscIndex >= palDsc_MAX_NUM_OF_ID)
	{
		HxLOG_Error("ulDscIndex %d > palDsc_MAX_NUM_OF_ID %d\r\n", ulDscIndex, palDsc_MAX_NUM_OF_ID);
		return ERR_FAIL;
	}

	if(pucKeyData == NULL)
	{
		HxLOG_Error(" \n");
		return ERR_FAIL;
	}

	pDsc = PAL_DSC_Find(ulDscIndex);
	if(pDsc == NULL)
	{
		HxLOG_Error(" \n");
		return ERR_FAIL;
	}

	nResult = DI_DSC_SetKey(pDsc->ulDscId, eParity, pucKeyData, 8, NULL, 0);
	if(nResult != ERR_OK)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR PAL_DSC_SetProtectionKey(HUINT32 ulDscIndex, HUINT8 *pucKeyData)
{
	HERROR nResult = ERR_FAIL;
	palDsc_Info_t *pDsc = NULL;

	HxLOG_Info("Desc Index (%d) \n", ulDscIndex);

	// Param Check
	if(ulDscIndex >= palDsc_MAX_NUM_OF_ID)
	{
		HxLOG_Error("ulDscIndex %d > palDsc_MAX_NUM_OF_ID %d\r\n", ulDscIndex, palDsc_MAX_NUM_OF_ID);
		return ERR_FAIL;
	}

	if(pucKeyData == NULL)
	{
		HxLOG_Error(" \n");
		return ERR_FAIL;
	}

	pDsc = PAL_DSC_Find(ulDscIndex);
	if(pDsc == NULL)
	{
		HxLOG_Error(" \n");
		return ERR_FAIL;
	}

	nResult = DI_DSC_SetProtectionKey(pDsc->ulDscId, pucKeyData, 16);
	if(nResult != ERR_OK)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR PAL_DSC_ClearProtectionKey(HUINT32 ulDscIndex)
{
	HERROR nResult = ERR_FAIL;
	palDsc_Info_t *pDsc = NULL;

	HxLOG_Info("Desc Index (%d) \n", ulDscIndex);

	// Param Check
	if(ulDscIndex >= palDsc_MAX_NUM_OF_ID)
	{
		HxLOG_Error("ulDscIndex %d > palDsc_MAX_NUM_OF_ID %d\r\n", ulDscIndex, palDsc_MAX_NUM_OF_ID);
		return ERR_FAIL;
	}

	pDsc = PAL_DSC_Find(ulDscIndex);
	if(pDsc == NULL)
	{
		HxLOG_Error(" \n");
		return ERR_FAIL;
	}

	nResult = DI_DSC_ClearProtectionKey(pDsc->ulDscId);
	if(nResult != ERR_OK)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR PAL_DSC_SetAesKey(HUINT32 ulDscIndex, HUINT8 *pucKeyData, HUINT32 ulKeySize)
{
	HERROR 			nResult = ERR_FAIL;
	palDsc_Info_t		*pDsc = NULL;
	unsigned char 	iv[]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

	HxLOG_Info("Desc Index (%d) \n", ulDscIndex);

	// Param Check
	if(ulDscIndex >= palDsc_MAX_NUM_OF_ID)
	{
		HxLOG_Error("ulDscIndex %d > palDsc_MAX_NUM_OF_ID %d\r\n", ulDscIndex, palDsc_MAX_NUM_OF_ID);
		return ERR_FAIL;
	}

	if(pucKeyData == NULL)
	{
		HxLOG_Error(" \n");
		return ERR_FAIL;
	}

	pDsc = PAL_DSC_Find(ulDscIndex);
	if(pDsc == NULL)
	{
		HxLOG_Error("PAL_DSC_Find fail \n");
		return ERR_FAIL;
	}

	nResult = DI_DSC_SetKey(pDsc->ulDscId, DI_DSC_DESC_ODD, pucKeyData, ulKeySize, iv, sizeof(iv));
	if(nResult != ERR_OK)
	{
		return ERR_FAIL;
	}

	nResult = DI_DSC_SetKey(pDsc->ulDscId, DI_DSC_DESC_EVEN, pucKeyData, ulKeySize, NULL, 0);
	if(nResult != ERR_OK)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

#if defined(CONFIG_MW_CAS_VERIMATRIX_IPTV)
HERROR PAL_DSC_SetVmxIptvDemuxId(HUINT32 ulDemixId)
{
	HERROR 			nResult = ERR_FAIL;

	nResult = BRCM_SetVmxDemuxId(ulDemixId);
	if(nResult != ERR_OK)
	{
		HxLOG_Error("BRCM_SetVmxDemuxId fail! ulDemixId(%d) \n", ulDemixId);
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR PAL_DSC_SetVmxIptvEncryptionAlgorithm(HUINT8 ucSvcIdx, HUINT8 ucEncAlgo)
{
	HERROR 			nResult = ERR_FAIL;

	nResult = BRCM_SetAlgorithmConfig(ucSvcIdx, ucEncAlgo);
	if(nResult != ERR_OK)
	{
		HxLOG_Error("BRCM_SetAlgorithm fail! ucSvcIdx(0x%X) ucEncAlgo(%d) \n", ucSvcIdx, ucEncAlgo);
		return ERR_FAIL;
	}

	return ERR_OK;
}
#endif


#ifdef CONFIG_MW_CI_PLUS
HERROR PAL_DSC_SetKeyForCiPlus(HUINT32 ulDscIndex, PalDsc_DscType_e eDscType, PalDsc_KeyParity_e eParity, HUINT8 *pucKeyData, HUINT8 *pucInitVec)
{
	HERROR nResult = ERR_FAIL;
	palDsc_Info_t *pDsc = NULL;
	HUINT32 ulKeyLength = 0, ulIvLength = 0;
	DI_DSC_DESC_ODD_EVEN di_parity = 0;

	HxLOG_Info("Desc Index (%d), DscType (%d), Parity (%d), KeyData (%p), InitVec (%p) \n",
		ulDscIndex, eDscType, eParity, pucKeyData, pucInitVec);

	// Param Check
	if(ulDscIndex >= palDsc_MAX_NUM_OF_ID)
	{
		HxLOG_Error("ulDscIndex %d > palDsc_MAX_NUM_OF_ID %d\r\n", ulDscIndex, palDsc_MAX_NUM_OF_ID);
		return ERR_FAIL;
	}

	if(eDscType != ePAL_DSC_DSCTYPE_DES_CLEAR && eDscType != ePAL_DSC_DSCTYPE_AES_CLEAR) // DES 또는 AES 가 아니면 에러 !
	{
		HxLOG_Error(" \n");
		return ERR_FAIL;
	}

	if(eDscType == ePAL_DSC_DSCTYPE_DES_CLEAR && pucInitVec != NULL) // DES 인데 IV 가 NULL 이 아니면 에러 !
	{
		HxLOG_Error(" \n");
		return ERR_FAIL;
	}

	if(eDscType == ePAL_DSC_DSCTYPE_AES_CLEAR && pucInitVec == NULL) // AES 인데 IV 가 NULL 이면 에러 !
	{
		HxLOG_Error(" \n");
		return ERR_FAIL;
	}

	if(eParity != ePAL_DSC_KEYPARITY_EVEN && eParity != ePAL_DSC_KEYPARITY_ODD) // even 또는 odd 가 아니면 에러 !
	{
		HxLOG_Error(" \n");
		return ERR_FAIL;
	}

	if(pucKeyData == NULL)
	{
		HxLOG_Error(" \n");
		return ERR_FAIL;
	}

	pDsc = PAL_DSC_Find(ulDscIndex);
	if(pDsc == NULL)
	{
		HxLOG_Error(" \n");
		return ERR_FAIL;
	}

	if(pDsc->eDscType != eDscType) // Open 시 설정된 값이랑 다르면 에러 !
	{
		HxLOG_Error(" \n");
		return ERR_FAIL;
	}

	di_parity = (eParity == ePAL_DSC_KEYPARITY_ODD) ? DI_DSC_DESC_ODD : DI_DSC_DESC_EVEN;
	ulKeyLength = (eDscType == ePAL_DSC_DSCTYPE_AES_CLEAR) ? 16 : 8;
	ulIvLength = (eDscType == ePAL_DSC_DSCTYPE_AES_CLEAR) ? 16 : 0;

	nResult = DI_DSC_SetKey(pDsc->ulDscId, di_parity, pucKeyData, ulKeyLength, pucInitVec, ulIvLength);
	if(nResult != ERR_OK)
	{
		HxLOG_Error(" \n");
		return ERR_FAIL;
	}

	return ERR_OK;
}
#endif

HERROR PAL_DSC_PrintCurrentKey(HUINT32 ulDscIndex)
{
#if defined(CONFIG_DEBUG)
	HLIB_CMD_Printf(" \n");
#endif

	return ERR_OK;
}

HERROR PAL_DSC_PrintDscStatus(HUINT32 ulDscIndex)
{
#if defined(CONFIG_DEBUG)
	if (s_astDscInfo[ulDscIndex].bUsed == 1)
	{
		HLIB_CMD_Printf("\n------ DscStatus (%d) ----------\n\n", ulDscIndex);
		HLIB_CMD_Printf("bUsed 		: %d \n", s_astDscInfo[ulDscIndex].bUsed);
		HLIB_CMD_Printf("eDscType 	: %d \n", s_astDscInfo[ulDscIndex].eDscType);
		HLIB_CMD_Printf("eDscDataType	: %d \n", s_astDscInfo[ulDscIndex].eDscDataType);
		HLIB_CMD_Printf("ulDemuxId 	: %d \n", s_astDscInfo[ulDscIndex].ulDemuxId);
		HLIB_CMD_Printf("ulDscId 	: %x \n", s_astDscInfo[ulDscIndex].ulDscId);
		HLIB_CMD_Printf("usPid 		: 0x%04x \n", s_astDscInfo[ulDscIndex].usPid);
		HLIB_CMD_Printf("\n------ End of DscStatus (%d) ----------\n\n", ulDscIndex);
	}
#endif

	return ERR_OK;
}

HERROR PAL_DSC_PrintAllDscStatus(void)
{
#if defined(CONFIG_DEBUG)
	HUINT32 i = 0;

	for(i=0; i<palDsc_MAX_NUM_OF_ID; i++)
	{
		PAL_DSC_PrintDscStatus(i);
	}
#endif

	return ERR_OK;
}

#endif // #if (defined(CONFIG_MW_CAS) || defined(CONFIG_MW_CI_PLUS))

/* end of file */
