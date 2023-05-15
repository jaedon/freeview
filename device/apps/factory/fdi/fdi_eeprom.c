/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author : 
* description :		 Factory Application
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/

/*******************************************************************************
* header and definitions
*******************************************************************************/
/* global header files */

/* chip manufacture's  header files */

/* humax header files */
#include "fdi_common.h"
#include "fdi_eeprom.h"

#include "di_err.h"
#include "di_eeprom.h"

/*******************************************************************************
* Debug level
*******************************************************************************/

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/

/*******************************************************************************
* External variables and functions
*******************************************************************************/
/* external variables  and functions */

/*******************************************************************************
* Global variables and structures
*******************************************************************************/
/* global variables and structures */
unsigned long g_ulEepromSize=0;

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_EEPROM_Open(void)
{
	int i;
	DI_ERR_CODE eDiErr;

#if defined(CONFIG_DI10)
	eDiErr = DI_EEPROM_GetCapabilities(&g_ulEepromSize);
	if( eDiErr != DI_ERR_OK )
	{
		return FDI_ERR;
	}

	FDI_PRINT(0, ("[FDI_EEPROM_Open] : EEPROM Size=%x\n", g_ulEepromSize));
#endif

	return FDI_NO_ERR;

}

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_EEPROM_GetCapability(unsigned long *pulEepromSize)
{
    *pulEepromSize = g_ulEepromSize;
	return FDI_NO_ERR;
}

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_EEPROM_Read(unsigned long ulStartAddr, unsigned char *pucBuf, unsigned long ulSize)
{
	DI_ERR_CODE eDiErr;

#if defined(CONFIG_DI10)
#if defined(CONFIG_EEPROM_EMU)
	eDiErr = DI_EEPROM_Read(ulStartAddr, pucBuf, ulSize);
#else
	eDiErr = DRV_EEPROM_ReadUncached(ulStartAddr, pucBuf, ulSize);
#endif
	if( eDiErr != DI_ERR_OK )
	{
		return FDI_ERR;
	}
#endif
	return FDI_NO_ERR;
}

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_EEPROM_Write(unsigned long ulStartAddr, unsigned char *pucBuf, unsigned long ulSize)
{
	DI_ERR_CODE eDiErr;

#if defined(CONFIG_DI10)
#if defined(CONFIG_EEPROM_EMU)
	eDiErr = DI_EEPROM_Write(ulStartAddr, pucBuf, ulSize);
#else
	eDiErr = DRV_EEPROM_WriteUncached(ulStartAddr, pucBuf, ulSize);
#endif
	if( eDiErr != DI_ERR_OK )
	{
		return FDI_ERR;
	}	
#endif
	return FDI_NO_ERR;
}

/* end of file */
