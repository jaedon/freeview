/**************************************************************************************/
/**
 * @file pl_crypt.c
 * defines APIs's body
 *
 * crypt module
 *
 * @attention   Copyright (c) 2013 Humax - All rights reserved.
 *
**************************************************************************************
**/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <vkernel.h>

#include <mxkernel.h>
#include <mxdtcp.h>

#include <di_crypt.h>
#include <pl_crypt.h>


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define SRM_FILE			"/var/lib/humaxtv_user/dtcp.srm"
#define SRM_FOLDER			"/var/lib/humaxtv_user/"
#define SW_IMAGE_SRM_FILE 	"/usr/dlna/dtcp.srm"


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	HBOOL					isInit;
	HBOOL					isStarted;
} PL_DTCP_Context_t;


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
/* not used */
//STATIC PL_DTCP_Context_t s_PL_DTCP_Context;



#define ________________Internal_Functions_________________________
void *__pl_crypt_MemAlloc(HUINT32 size)
{
	void		*pSrcBuf;

	pSrcBuf = DI_CRYPT_MemAlloc(size);
	if (pSrcBuf == NULL)
	{
		HxLOG_Error("[__pl_dms_crypt_MemAlloc] DI_CRYPT_MemAlloc error!! pSrcBuf is NULL\n");
		return NULL;
	}

	return pSrcBuf;
}

void __pl_crypt_MemFree(void *p)
{
	if (p == NULL)
	{
		HxLOG_Error("[__pl_dms_crypt_MemFree] NULL pointer\n");
		return;
	}

	DI_CRYPT_MemFree(p);
}

int __pl_crypt_AES_Encrypt(unsigned char *pMainKey, unsigned char *pSharedKey, unsigned int ulKeySize, unsigned char *pSrcBuffer, unsigned char *pDstBuffer, unsigned int ulBuffSize)
{
	HUINT32 nResult = ERR_FAIL;
	stCrypt_t tCrytInfo;

	HxSTD_MemSet(&tCrytInfo, 0x0, sizeof(tCrytInfo));

	tCrytInfo.etCryptMode = DI_CRYPT_AES;
	tCrytInfo.etCryptKey = DI_CRYPT_USER_KEY;
	tCrytInfo.etCryptData = DI_CRYPT_BLOCK;
	tCrytInfo.pKey = (HUINT8 *)pMainKey;
	tCrytInfo.ulKeySize = (HUINT32)ulKeySize;
	tCrytInfo.ulBuffSize = (HUINT32)ulBuffSize;
	tCrytInfo.pSrcBuffer = (HUINT8 *)pSrcBuffer;
	tCrytInfo.pDstBuffer = (HUINT8 *)pDstBuffer;
	tCrytInfo.pIvKey = (HUINT8 *)pSharedKey;

	if(DI_CRYPT_Encrypt(&tCrytInfo) != DI_ERR_OK)
	{
		HxLOG_Error("Error DI_CRYPT_Encrypt\n");
		return nResult;
	}

	nResult = ERR_OK;
	return nResult;
}

int __pl_crypt_AES_Decrypt(unsigned char *pMainKey, unsigned char *pSharedKey, unsigned int ulKeySize, unsigned char *pSrcBuffer, unsigned char *pDstBuffer, unsigned int ulBuffSize)
{
	HUINT32 nResult = ERR_FAIL;
	stCrypt_t tCrytInfo;

	HxSTD_MemSet(&tCrytInfo, 0x0, sizeof(tCrytInfo));

	tCrytInfo.etCryptMode = DI_CRYPT_AES;
	tCrytInfo.etCryptKey = DI_CRYPT_USER_KEY;
	tCrytInfo.etCryptData = DI_CRYPT_BLOCK;
	tCrytInfo.pKey = (HUINT8 *)pMainKey;
	tCrytInfo.ulKeySize = (HUINT32)ulKeySize;
	tCrytInfo.ulBuffSize = (HUINT32)ulBuffSize;
	tCrytInfo.pSrcBuffer = (HUINT8 *)pSrcBuffer;
	tCrytInfo.pDstBuffer = (HUINT8 *)pDstBuffer;
	tCrytInfo.pIvKey = (HUINT8 *)pSharedKey;

	if(DI_CRYPT_Decrypt(&tCrytInfo) != DI_ERR_OK)
	{
		HxLOG_Error("Error DI_CRYPT_Decrypt\n");
		return nResult;
	}

	nResult = ERR_OK;
	return nResult;
}

STATIC HBOOL __pl_crypt_IsFileExist(const HCHAR *pathname)
{
	struct stat st;
	return (0 == stat(pathname, &st) && S_ISREG(st.st_mode)) ? TRUE : FALSE;
}


#define ________________Public_Interfaces__________________________

/* Initialized DTCP  */
HOM_Result_e PL_DTCP_Init( void )
{
	HOM_Result_e		eRet = eHOM_Result_Error;
	HINT8				pDtcpIpKey[1024] = {0,};
	HCHAR 				szCmdBuf[4096] = {0,};
	HINT32				nDtcpIpKeySize = 0;
	MXDTCP_CALLBACK 	tDtcpCallback;
	int					dtcp_result = 0;

	/* SRM 이 없으면, Rootfs에 있는 기본 SRM을 복사 한다.  */
	if(__pl_crypt_IsFileExist(SRM_FILE) != TRUE)
	{
		if(__pl_crypt_IsFileExist(SW_IMAGE_SRM_FILE) == TRUE)
		{
			HxSTD_PrintToStrN(szCmdBuf, sizeof(szCmdBuf), "cp %s %s", SW_IMAGE_SRM_FILE, SRM_FOLDER);
			VK_SYSTEM_Command(szCmdBuf);
		}
		else
		{
			HxLOG_Error("[%s][%d] Error: No Default DTCP SRM file (%s)\n", __FUNCTION__, __LINE__, SW_IMAGE_SRM_FILE);
		}
	}

	if(__pl_crypt_IsFileExist(SRM_FILE) == TRUE)
	{
		mxDtcp_SetSrmPath(SRM_FILE);
	}
	else
	{
		HxLOG_Error("[%s][%d] Error: No DTCP SRM file (%s)\n", __FUNCTION__, __LINE__, SRM_FILE);
	}

	OAPI_SYSINFO_STB_GetDtcpIpKeySize(&nDtcpIpKeySize);
	OAPI_SYSINFO_STB_GetDtcpIpKey(pDtcpIpKey, 1024);

	dtcp_result = mxDtcp_SetSecureData((char *)pDtcpIpKey, nDtcpIpKeySize);
	HxLOG_Print("[%s][%d] mxDtcp_SetSecureData result=%d ###\n", __FUNCTION__, __LINE__, dtcp_result);

	if(dtcp_result == MXDTCP_SUCCESS)
	{
		tDtcpCallback.fnMemAlloc = __pl_crypt_MemAlloc;
		tDtcpCallback.fnMemFree = __pl_crypt_MemFree;
		tDtcpCallback.fnEncrypt = __pl_crypt_AES_Encrypt;
		tDtcpCallback.fnDecrypt = __pl_crypt_AES_Decrypt;
		tDtcpCallback.fnEncryptAesKey = NULL;

		(void)mxDtcp_SetCallback(&tDtcpCallback);

		eRet = eHOM_Result_Ok;
	}

	return eRet;
}


