/*******************************************************************************
* $Workfile:$
* $Modtime:$
* Author : 
* description :		 Factory Uart Driver Interface 
*
* Copyright (c) 2009 by Humax Co., Ltd.
* All right reserved
*******************************************************************************/

/*******************************************************************************
* header and definitions
*******************************************************************************/
/* global header files */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#if !defined(CONFIG_OS_RTOS)
#include <termio.h>
#include <termios.h>
#endif

/* chip manufacture's  header files */

/* humax header files */
#include "fvk.h"
#include "fdi_err.h"
#include "fdi_common.h"
#include "fdi_crypt.h"

#include "vkernel.h"
#include "di_crypt.h"
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

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_CRYPT_Open(void)
{
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
FDI_ERR_CODE FDI_CRYPT_Decrypt(unsigned char *pucInData, unsigned char *pucOutData,unsigned long ulSize)
{
	DI_ERR_CODE eErr = DI_ERR_OK;
	stCrypt_t stCryptInfo;

	if(pucInData == NULL || pucOutData == NULL || ulSize == 0)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_CRYPT_Decrypt]parameter Error pucInData 0x%x pucOutData 0x%x ulSize 0x%x\n",pucInData,pucOutData,ulSize));
		return FDI_ERR;
	}
	
	VK_MEM_Memset((void *)&stCryptInfo, 0, sizeof(stCrypt_t));
	stCryptInfo.etCryptMode = DI_CRYPT_TDES_DTCPIP;
	stCryptInfo.etCryptKey = DI_CRYPT_DEFAULT_SYSTEM_KEY;
	stCryptInfo.etCryptData = DI_CRYPT_BLOCK;
	stCryptInfo.pKey = NULL;

	stCryptInfo.ulKeySize = 16;
	stCryptInfo.pSrcBuffer = DI_CRYPT_MemAlloc(ulSize);
	if (stCryptInfo.pSrcBuffer == NULL)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_CRYPT_Decrypt]pSrcBuffer NULL\n"));
		return FDI_ERR;
	}

	memcpy((void *)stCryptInfo.pSrcBuffer, (void *)pucInData, ulSize);

	stCryptInfo.pDstBuffer = stCryptInfo.pSrcBuffer;
	if (stCryptInfo.pDstBuffer == NULL)
	{
		DI_CRYPT_MemFree(stCryptInfo.pSrcBuffer);
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_CRYPT_Decrypt]  pDstBuffer NULL\n"));
		return FDI_ERR;
	}

	stCryptInfo.ulBuffSize = ulSize;

	eErr = DI_CRYPT_Decrypt(&stCryptInfo);
	if (eErr != DI_ERR_OK)
	{
		DI_CRYPT_MemFree(stCryptInfo.pSrcBuffer);		
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_CRYPT_Decrypt] Error DI_CRYPT_Encrypt \n"));
		return FDI_ERR;
	}
	
	memcpy((void *)pucOutData, (void *)stCryptInfo.pDstBuffer, ulSize);

	return FDI_NO_ERR;
}

/* end of file */

