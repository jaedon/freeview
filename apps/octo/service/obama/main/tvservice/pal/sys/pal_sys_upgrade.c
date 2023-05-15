/**
	@file     pal_sys_upgrade.c

	Description: flash control module
	Module: pal/flash

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <octo_common.h>

#include <di_err.h>
#include <di_nvram.h>
#include <pal_sys.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define palSys_UPGRADE_PIECE_SIZE_DEFAULT			126976			// 124 KB = 124*1024
#define palSys_UPGRADE_DATA_SIZE_128M				1024*1024*128	// 128 MB


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct tagUpgradeInfo
{
	HBOOL	bInit;
	HUINT32	ulDataSize;
	HUINT32	ulPieceSize;
	HUINT32	ulPieceNum;
	HUINT32 ulUpgradeMaxSize;
} palSys_UpgradeInfo_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC palSys_UpgradeInfo_t	s_stUpgradeInfo;

/*******************************************************************/
/*****************      Static Functions      **********************/
/*******************************************************************/

STATIC HUINT32 pal_sys_UpgradeGetUpgradeMaxSize(void)
{
	return s_stUpgradeInfo.ulUpgradeMaxSize;
}

HERROR	PAL_SYS_UpgradeInit(HUINT32 ulUpgradeMaxSize)
{
	HxSTD_memset(&s_stUpgradeInfo, 0, sizeof(palSys_UpgradeInfo_t));
	s_stUpgradeInfo.ulPieceSize = palSys_UPGRADE_PIECE_SIZE_DEFAULT;
	if (ulUpgradeMaxSize)
	{
		s_stUpgradeInfo.ulUpgradeMaxSize = ulUpgradeMaxSize;
	}
	else
	{
		s_stUpgradeInfo.ulUpgradeMaxSize = palSys_UPGRADE_DATA_SIZE_128M;
	}

	return ERR_OK;
}

HERROR	PAL_SYS_Upgrade_Setup(HUINT32	 ulDataSize, 		// In Bytes
							HUINT32		 ulPieceSize, 		// In Bytes : Default 128 KB
							HUINT32		*pulPieceNum,
							HUINT32		*pulPieceSize)
{
	if (0 == ulDataSize || pal_sys_UpgradeGetUpgradeMaxSize() < ulDataSize)
	{
		HxLOG_Error("0 == ulDataSize || %d < ulDataSize \n", pal_sys_UpgradeGetUpgradeMaxSize);
		return ERR_FAIL;
	}

	if (0 == ulPieceSize)
	{
		HxLOG_Error("0 == ulPieceSize\n");
		return ERR_FAIL;
	}

	s_stUpgradeInfo.ulDataSize		= ulDataSize;
	s_stUpgradeInfo.ulPieceSize		= (PalSys_UPGRADE_PIECE_SIZE_FULL == ulPieceSize) ? palSys_UPGRADE_PIECE_SIZE_DEFAULT : ulPieceSize;
	s_stUpgradeInfo.ulPieceNum		= (ulDataSize + s_stUpgradeInfo.ulPieceSize - 1) / s_stUpgradeInfo.ulPieceSize;

	if (pulPieceNum)
	{
		*pulPieceNum = s_stUpgradeInfo.ulPieceNum;
	}

	if (pulPieceSize)
	{
		*pulPieceSize = s_stUpgradeInfo.ulPieceSize;
	}

	return ERR_OK;
}

HINT32	PAL_SYS_Upgrade_ReadData(HUINT32	 ulPieceIdx,
								HUINT8		*pucData,
								HUINT32		 ulSize)
{
	HINT32			nDataSize;
	HUINT32			ulOffset;
	DI_ERR_CODE		ulErr;

	if (ulPieceIdx >= s_stUpgradeInfo.ulPieceNum)
	{
		HxLOG_Error("ulPieceIdx (%d) >= s_stUpgradeInfo.ulPieceNum (%d)\n", ulPieceIdx, s_stUpgradeInfo.ulPieceNum);
		return -1;
	}

	if (NULL == pucData)
	{
		HxLOG_Error("NULL == pucData\n");
		return -1;
	}

	if (0 == ulSize)
	{
		HxLOG_Error("0 == ulSize\n");
		return -1;
	}

	ulOffset = ulPieceIdx * s_stUpgradeInfo.ulPieceSize;

	if (s_stUpgradeInfo.ulDataSize < (ulOffset + ulSize))
	{
		HxLOG_Error("s_stUpgradeInfo.ulDataSize (%d) < (%d)\n", s_stUpgradeInfo.ulDataSize, (ulOffset + ulSize));
		return -1;
	}

	if (ulSize == PalSys_UPGRADE_PIECE_SIZE_FULL)
	{
		nDataSize = s_stUpgradeInfo.ulPieceSize;
	}
	else
	{
		nDataSize = (HINT32)ulSize;
	}

	HxLOG_Debug("ulOffset [%d], nDataSize [%d]\n", ulOffset, nDataSize);

	ulErr = DI_NVRAM_GetField(DI_NVRAM_FIELD_RAWFS, ulOffset, (void *)pucData, nDataSize);

	HxLOG_Debug("DI_NVRAM_SetField() return [%d] \n", ulErr);

	return (DI_ERR_OK == ulErr) ? nDataSize : -1;
}

HINT32	PAL_SYS_Upgrade_WriteData(HUINT32		 ulPieceIdx,
								const HUINT8	*pucData,
								HUINT32			 ulSize)
{
	HINT32			nDataSize;
	HUINT32			ulOffset;
	DI_ERR_CODE		ulErr;
	if (ulPieceIdx >= s_stUpgradeInfo.ulPieceNum)
	{
		HxLOG_Error("ulPieceIdx (%d) >= s_stUpgradeInfo.ulPieceNum (%d)\n", ulPieceIdx, s_stUpgradeInfo.ulPieceNum);
		return -1;
	}

	if (NULL == pucData)
	{
		HxLOG_Error("NULL == pucData\n");
		return -1;
	}

	if (0 == ulSize)
	{
		HxLOG_Error("0 == ulSize\n");
		return -1;
	}

	ulOffset = ulPieceIdx * s_stUpgradeInfo.ulPieceSize;

	if (s_stUpgradeInfo.ulDataSize < (ulOffset + ulSize))
	{
		HxLOG_Error("s_stUpgradeInfo.ulDataSize (%d) < (%d)\n", s_stUpgradeInfo.ulDataSize, (ulOffset + ulSize));
		return -1;
	}

	if (ulSize == PalSys_UPGRADE_PIECE_SIZE_FULL)
	{
		nDataSize = s_stUpgradeInfo.ulPieceSize;
	}
	else
	{
		nDataSize = (HINT32)ulSize;
	}

	HxLOG_Debug("ulOffset [%d], nDataSize [%d]\n", ulOffset, nDataSize);

	ulErr = DI_NVRAM_SetField(DI_NVRAM_FIELD_RAWFS, ulOffset, (void *)pucData, nDataSize);

	HxLOG_Debug("DI_NVRAM_SetField() return [%d] \n", ulErr);

	return (DI_ERR_OK == ulErr) ? nDataSize : -1;
}


/* end of file */

