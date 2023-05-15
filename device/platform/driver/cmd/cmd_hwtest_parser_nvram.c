#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <unistd.h>

#include "vkernel.h"
#include "htype.h"
#include "taskdef.h"
#include "cmdtool.h"
#include "drv_nvram.h"

#include "di_crypt.h"

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#define TEST_EXT_DEBUG
#define TEST_EXT_ENTER_LEAVE_DEBUG

#ifdef TEST_EXT_DEBUG
#define PrintDebug			VK_printf
#define PrintError			VK_printf
#else
#define PrintDebug			while (0) ((int (*)(char *, ...)) 0)
#define PrintError			VK_printf
#endif

#ifdef TEST_EXT_ENTER_LEAVE_DEBUG
#define PrintEnter				VK_printf("(+)%s\n", __FUNCTION__)
#define PrintExit				VK_printf("(-)%s\n", __FUNCTION__)
#else
#define PrintEnter				while (0) ((int (*)(char *, ...)) 0)
#define PrintExit				while (0) ((int (*)(char *, ...)) 0)
#endif

#define AssertDI(arg1, arg2) if(arg1 != DI_ERR_OK) {VK_printf(\
					"ERROR code : 0x%x, At %s() function, FILE: %s, LINE: %d\n",arg1, __func__, __FILE__,__LINE__); return arg2;}

#define AssertDRV(arg1, arg2) if(arg1 != DRV_OK) {VK_printf(\
					"ERROR code : 0x%x, At %s() function, FILE: %s, LINE: %d\n",arg1, __func__, __FILE__,__LINE__); return arg2;}

#define UNUSED(x) (void)(x)

#define HWTEST_CMD		szCmd
#define HWTEST_PARAM	szParam
#define HWTEST_PARAM1	szParam1
#define HWTEST_PARAM2	szParam2
#define GET_ARGS		int iResult=CMD_ERR; \
						char *HWTEST_CMD=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM1=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM2=CMD_NextArg((char **)&szArgs);
#define CMD_IS(x)		(HWTEST_CMD!=NULL && VK_strcmp(HWTEST_CMD, x)==0)
#define PARAM_IS(x)		(HWTEST_PARAM!=NULL && VK_strcmp(HWTEST_PARAM, x)==0)
#define PARAM1_IS(x)	(HWTEST_PARAM1!=NULL && VK_strcmp(HWTEST_PARAM1, x)==0)
#define PARAM2_IS(x)	(HWTEST_PARAM2!=NULL && VK_strcmp(HWTEST_PARAM2, x)==0)
#define READABLE_IN_HEX(sz,val) CMD_ReadAsHex(sz, &val)
#define READABLE_IN_DEC(sz,val) CMD_ReadAsDec(sz, &val)

int CMD_HwTest_Parser_Nvram(void *szArgs)
{
	GET_ARGS;
	if (0)
	{
	}
	else if (CMD_IS("w"))
	{
		if (PARAM_IS("ro"))
		{
			HUINT8	readValue[256];

			VK_memset(readValue, 0, 256);
			DRV_NVRAM_Write(DRV_NVRAM_PARTITION_RO, (HUINT8*)HWTEST_PARAM1, 0, (HUINT8*)HWTEST_PARAM2, VK_strlen(HWTEST_PARAM2));
			DRV_NVRAM_Read(DRV_NVRAM_PARTITION_RO, (HUINT8*)HWTEST_PARAM1, 0, readValue, VK_strlen(HWTEST_PARAM2));
			CMD_Printf("nvram write %s:%s -> %s:%s\n", HWTEST_PARAM1, HWTEST_PARAM2, HWTEST_PARAM1, readValue);
			CMD_Printf("\n");
			iResult = 0;
		}
		else if (PARAM_IS("rw"))
		{
			HUINT8	hexValue[4];
			HUINT8	readValue[256];

			VK_memset(hexValue, 0, 4);
			VK_memset(readValue, 0, 256);

			if(HWTEST_PARAM2[0] == '0' && HWTEST_PARAM2[1] == 'x')
			{
				HINT32	nValue = 0;
				VK_sscanf(HWTEST_PARAM2, "0x%x", hexValue);

				DRV_NVRAM_Write(DRV_NVRAM_PARTITION_RW, (HUINT8*)HWTEST_PARAM1, 0, (HUINT8*)hexValue, 4);
				DRV_NVRAM_Read(DRV_NVRAM_PARTITION_RW, (HUINT8*)HWTEST_PARAM1, 0, (HUINT8*)&nValue, 4);
				CMD_Printf("nvram write %s:%s -> %s:0x%x\n", HWTEST_PARAM1, HWTEST_PARAM2, HWTEST_PARAM1, nValue);
			}
			else
			{
				DRV_NVRAM_Write(DRV_NVRAM_PARTITION_RW, (HUINT8*)HWTEST_PARAM1, 0, (HUINT8*)HWTEST_PARAM2, VK_strlen(HWTEST_PARAM2));
				DRV_NVRAM_Read(DRV_NVRAM_PARTITION_RW, (HUINT8*)HWTEST_PARAM1, 0, readValue, VK_strlen(HWTEST_PARAM2));
				CMD_Printf("nvram write %s:%s -> %s:%s\n", HWTEST_PARAM1, HWTEST_PARAM2, HWTEST_PARAM1, readValue);
			}
			CMD_Printf("\n");
			iResult = 0;
		}
	}
	else if (CMD_IS("r"))
	{
		HUINT8	*pRValue;
		int num;

		if (PARAM_IS("ro"))
		{
			DRV_NVRAM_GetLength(DRV_NVRAM_PARTITION_RO, (const HUINT8*)HWTEST_PARAM1, (HUINT32*)&num);
			pRValue = VK_malloc(num);
			DRV_NVRAM_Read(DRV_NVRAM_PARTITION_RO, (HUINT8*)HWTEST_PARAM1, 0, pRValue, num);
			CMD_Printf("nvram read %s\n", HWTEST_PARAM1);
			DI_UART_Dump(pRValue,num,16);
			CMD_Printf("\n");
			iResult = 0;
		}
		else if (PARAM_IS("rw"))
		{
			DRV_NVRAM_GetLength(DRV_NVRAM_PARTITION_RW, (const HUINT8*)HWTEST_PARAM1, (HUINT32*)&num);
			pRValue = VK_malloc(num);
			DRV_NVRAM_Read(DRV_NVRAM_PARTITION_RW, (HUINT8*)HWTEST_PARAM1, 0, pRValue, num);
			CMD_Printf("nvram read %s\n", HWTEST_PARAM1);
			DI_UART_Dump(pRValue,num,16);
			CMD_Printf("\n");
			iResult = 0;
		}

		if (READABLE_IN_DEC(HWTEST_PARAM1, num))
		{

		}
	}
	else if (CMD_IS("dir"))
	{
		DRV_NVRAM_Dir();
		CMD_Printf("\n");
		iResult = 0;
	}
	else if (CMD_IS("data"))
	{
		DRV_NVRAM_DirData();
		CMD_Printf("\n");
		iResult = 0;
	}
	else if (CMD_IS("del"))
	{
		if (PARAM_IS("ro"))
		{
			DRV_NVRAM_Remove(DRV_NVRAM_PARTITION_RO, (HUINT8*)HWTEST_PARAM1);
		}
		else if (PARAM_IS("rw"))
		{
			DRV_NVRAM_Remove(DRV_NVRAM_PARTITION_RW, (HUINT8*)HWTEST_PARAM1);
		}
		iResult = 0;
	}
	else if (CMD_IS("check"))
	{
		HBOOL	orgData, backData, blockCheck;
		DRV_Error	drv_error;

		drv_error = DRV_NVRAM_CheckData(DRV_NVRAM_PARTITION_RO, &orgData, &backData, &blockCheck);
		CMD_Printf("[DRV_NVRAM_PARTITION_RO] orgData (%d) backData (%d) blockCheck (%d)\n", orgData, backData, blockCheck);
		drv_error = DRV_NVRAM_CheckData(DRV_NVRAM_PARTITION_RW, &orgData, &backData, &blockCheck);
		CMD_Printf("[DRV_NVRAM_PARTITION_RW] orgData (%d) backData (%d) blockCheck (%d)\n", orgData, backData, blockCheck);
		iResult = 0;
	}
	else if (CMD_IS("waging"))
	{
		HUINT32		count;
		HCHAR		szStr[128];
		DRV_Error	drvError;

		for (count = 0; count < 1024; count++)
		{
			VK_memset(szStr, 0, 128);
			VK_sprintf(szStr, "%d", count);
			drvError = DRV_NVRAM_Write(DRV_NVRAM_PARTITION_RW, (HUINT8*)"TEST.BIN", 0, (HUINT8*)szStr, (HUINT32)VK_strlen(szStr));
			if (drvError != DRV_OK)
			{
				CMD_Printf("waging : error DRV_NVRAM_Write error-(%d)\n", drvError);
			}
			CMD_Printf("write count (%d)\n", count+1);
		}
		iResult = 0;
	}
	else if (CMD_IS("kptest"))
	{
#if defined(CONFIG_OS_UCOS)
#else
#if defined(CONFIG_CRYPTO)
		unsigned char KP_ORG_FILE[20] = "FSAT_ORG_KP.tgz";
		unsigned char KP_ENC_FILE[30] = "FSAT_KP.tgz";
		unsigned int dataLen = 0;
		unsigned char *pData = NULL;
		DRV_Error	drvErr = 0,sdrvErr = 0;
		stCrypt_t	CryptInfo;
		FILE * fp;
		long tfilelen = 0;
		unsigned char *fileData = NULL;
		DRV_Error aError = 0;

		aError = drv_NVRAM_GetLength(DRV_NVRAM_PARTITION_RO, KP_ORG_FILE, &dataLen);
		if((aError == DRV_OK)&&(dataLen > 0))
		{
			sdrvErr = DRV_NVRAM_Remove(DRV_NVRAM_PARTITION_RO,KP_ORG_FILE);
			if (sdrvErr != DRV_OK)
			{
				CMD_Printf("Error DRV_NVRAM_Remove Pure Data\n");
				goto END;
			}
		}

		aError = drv_NVRAM_GetLength(DRV_NVRAM_PARTITION_RO, KP_ENC_FILE, &dataLen);
		if((aError == DRV_OK)&&(dataLen > 0))
		{
			sdrvErr = DRV_NVRAM_Remove(DRV_NVRAM_PARTITION_RO,KP_ENC_FILE);
			if (sdrvErr != DRV_OK)
			{
				CMD_Printf("Error DRV_NVRAM_Remove ENC Data\n");
				goto END;
			}
		}

		if(HWTEST_PARAM == NULL)
		{
			CMD_Printf("Error :: Wrong parameter\n");
			goto END;
		}
		fp = fopen(HWTEST_PARAM, "rb");
		if (!fp)
		{
			CMD_Printf("Error :: Empty\n");
			goto END;
		}

		fseek( fp, 0, SEEK_END );
		tfilelen = ftell( fp );

		if(tfilelen != 0)
		{
			fileData = VK_MEM_Alloc(tfilelen);

			if(fileData != NULL)
			{
				rewind(fp);

				if (fread(fileData, tfilelen, 1, fp) != 1)
				{
					close(fp);
					VK_MEM_Free(fileData);
					goto END;
				}
			}
			else
			{
				close(fp);
				VK_MEM_Free(fileData);
				goto END;
			}

		}
		close(fp);

		sdrvErr = DRV_NVRAM_Write(DRV_NVRAM_PARTITION_RO, KP_ORG_FILE, 0, fileData, tfilelen);
		if (sdrvErr != DRV_OK)
		{
			CMD_Printf("Error DRV_NVRAM_Write Pure Data\n");
			VK_MEM_Free(fileData);
			goto END;
		}

		CMD_Printf("\n");
		DRV_NVRAM_Dir();
		CMD_Printf("\n");

		VK_TASK_Sleep(200);

		aError = drv_NVRAM_GetLength(DRV_NVRAM_PARTITION_RO, KP_ORG_FILE, &dataLen);
		if((aError == 0)&&(dataLen > 0))
		{
			pData = VK_MEM_Alloc(dataLen);
			if(pData != NULL)
			{
				drvErr = DRV_NVRAM_Read(DRV_NVRAM_PARTITION_RO, KP_ORG_FILE, 0, pData, dataLen);
				if(drvErr == 0)
				{
					VK_MEM_Memset((void *)&CryptInfo, 0, sizeof(stCrypt_t));
					CryptInfo.etCryptMode = DI_CRYPT_TDES;
					CryptInfo.etCryptKey = DI_CRYPT_DEFAULT_SYSTEM_KEY;
					CryptInfo.pKey = NULL;

					CryptInfo.ulKeySize = 16;
					CryptInfo.pSrcBuffer = DI_CRYPT_MemAlloc(dataLen);
					if (CryptInfo.pSrcBuffer == NULL)
					{
						CMD_Printf("Error DI_CRYPT_MemAlloc pSrcBuffer NULL\n");
						goto STEP1;
					}

					VK_MEM_Memcpy((void *)CryptInfo.pSrcBuffer, (void *)pData, dataLen);

					CryptInfo.pDstBuffer = DI_CRYPT_MemAlloc(dataLen);
					if (CryptInfo.pDstBuffer == NULL)
					{
						CMD_Printf("Error DI_CRYPT_MemAlloc pDstBuffer NULL\n");
						goto STEP2;
					}

					CryptInfo.ulBuffSize = dataLen;
					sdrvErr = DI_CRYPT_Encrypt(&CryptInfo);
					if (sdrvErr != DRV_OK)
					{
						CMD_Printf("Error DI_CRYPT_Encrypt\n");
						goto STEP3;
					}

					sdrvErr = DRV_NVRAM_Write(DRV_NVRAM_PARTITION_RO, KP_ENC_FILE, 0, CryptInfo.pDstBuffer, dataLen);
					if (sdrvErr != DRV_OK)
					{
						CMD_Printf("Error DRV_NVRAM_Write Encryption Data\n");
						goto STEP3;
					}

					sdrvErr = DRV_NVRAM_Remove(DRV_NVRAM_PARTITION_RO,KP_ORG_FILE);
					if (sdrvErr != DRV_OK)
					{
						CMD_Printf("Error DRV_NVRAM_Remove Pure Data\n");
					}

				}
			}
			else
			{
				goto END;
			}
		}

STEP3:
		DI_CRYPT_MemFree(CryptInfo.pDstBuffer);
STEP2:
		DI_CRYPT_MemFree(CryptInfo.pSrcBuffer);
STEP1:
		VK_MEM_Free(pData);
END:

		CMD_Printf("\n");
		DRV_NVRAM_Dir();
		CMD_Printf("\n");
#endif
#endif
	}
	else
	{
		CMD_Printf("Command : nvram : unknown parameter\n");
		//CMD_Printf("  ex) nvram r [filename]\n");
		CMD_Printf("  ex) nvram w [ro|rw]] [filename] [string]\n");
		CMD_Printf("  ex) nvram w [ro|rw]] [filename] [0x10000000]\n");
		CMD_Printf("  ex) nvram r [ro|rw]] [filename] \n");
		CMD_Printf("  ex) nvram del [ro|rw] [filename]\n");
		CMD_Printf("  ex) nvram dir\n");
		//CMD_Printf("  ex) nvram stress [off|on]\n");
		iResult = 0;
	}

	return iResult;
}

