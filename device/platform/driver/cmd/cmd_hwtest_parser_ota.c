/*********************************************************************
*	author :
*
*	file descriptions....
*		:
*
*	Copyright (c) 2004 by Humax Co., Ltd.
*	All rights reserved
********************************************************************/
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <ctype.h>

#include "htype.h"
#include "vkernel.h"
#include "di_uart.h"
#include "cmdtool.h"
#include "di_err.h"
#include "di_nvram.h"
#include "nvram_map.h"
#include "di_ftp.h"
#include "di_channel.h"
#include "di_channel_c.h"
#include "di_channel_s.h"

#define CMD_OK					0
#define CMD_ERR					1

#define HWTEST_CMD		szCmd
#define HWTEST_PARAM	szParam
#define HWTEST_PARAM1	szParam1
#define HWTEST_PARAM2	szParam2
#define HWTEST_PARAM3   szParam3
#define GET_ARGS		int iResult=CMD_ERR; \
						char *HWTEST_CMD=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM1=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM2=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM3=CMD_NextArg((char **)&szArgs);
#define CMD_IS(x)		(HWTEST_CMD!=NULL && VK_strcmp(HWTEST_CMD, x)==0)
#define PARAM_IS(x)		(HWTEST_PARAM!=NULL && VK_strcmp(HWTEST_PARAM, x)==0)
#define PARAM1_IS(x)	(HWTEST_PARAM1!=NULL && VK_strcmp(HWTEST_PARAM1, x)==0)
#define PARAM2_IS(x)	(HWTEST_PARAM2!=NULL && VK_strcmp(HWTEST_PARAM2, x)==0)
#define PARAM3_IS(x)	(HWTEST_PARAM3!=NULL && VK_strcmp(HWTEST_PARAM3, x)==0)
#define READABLE_IN_HEX(sz,val) CMD_ReadAsHex(sz, &val)
#define READABLE_IN_DEC(sz,val) CMD_ReadAsDec(sz, &val)

#define GET_NEXT_ARG(variable)		char *variable = CMD_NextArg((char **)&szArgs)

#define AssertDI(arg1, arg2) if(arg1 != DI_ERR_OK) {CMD_Printf(\
					"ERROR code : 0x%x, At %s() function, FILE: %s, LINE: %d\n",arg1, __func__, __FILE__,__LINE__); return arg2;}

typedef struct
{
	HBOOL bAvailable;
	HUINT32 ulLnbFreq;
	CH_SAT_DiseqcInput_e etDiseqcInput;				/**< Input position */
	HBOOL b22KTone;
}DI_CMD_AntennaInfo_t;

static DI_CMD_AntennaInfo_t s_stAntennaInfo;

int _parseipaddr(const char *ipaddr, HUINT8 *dest)
{
    int a,b,c,d;
    char *x;

    /* make sure it's all digits and dots. */
    x = (char *) ipaddr;
    while (*x) {
        if ((*x == '.') || ((*x >= '0') && (*x <= '9'))) {
            x++;
            continue;
            }
        return -1;
        }

    x = (char *) ipaddr;
    a = VK_atoi(ipaddr);
    x = VK_strchr(x,'.');
    if (!x) return -1;
    b = VK_atoi(x+1);
    x = VK_strchr(x+1,'.');
    if (!x) return -1;
    c = VK_atoi(x+1);
    x = VK_strchr(x+1,'.');
    if (!x) return -1;
    d = VK_atoi(x+1);

    if ((a < 0) || (a > 255)) return -1;
    if ((b < 0) || (b > 255)) return -1;
    if ((c < 0) || (c > 255)) return -1;
    if ((d < 0) || (d > 255)) return -1;

    dest[0] = (HUINT8) a;
    dest[1] = (HUINT8) b;
    dest[2] = (HUINT8) c;
    dest[3] = (HUINT8) d;

    return 0;
}

DI_ERR_CODE _setFtpOtaPath(HUINT8 *pOtaInfo)
{
	HINT32	i;
	HINT32	res;
	HUINT8	*pStartIndex=0;
	HUINT8	*pEndIndex=0;
	HUINT8	aInfoBuff[FTP_DIR_PATH_LENGTH + FTP_FILE_NAME_LENGTH];
	DI_FTP_OTA_SETTING_T *pFtpOtaSet;

	if(pOtaInfo ==NULL)
	{
		DI_UART_Print("[_setFtpOtaPath] pOtaInfo is NULL !!! \n");
		return DI_ERR_ERROR;
	}

	/* Alloc OTA Struct */
	pFtpOtaSet = DD_MEM_Alloc(sizeof(DI_FTP_OTA_SETTING_T));
	VK_memset(pFtpOtaSet, 0x0, sizeof(DI_FTP_OTA_SETTING_T));

	/* Get IP address */
	VK_memset(aInfoBuff, 0, FTP_DIR_PATH_LENGTH + FTP_FILE_NAME_LENGTH);
	for (i=0; i<(FTP_DIR_PATH_LENGTH + FTP_FILE_NAME_LENGTH); i++)
	{
		if (pOtaInfo[i] == ':' || pOtaInfo[i] == 0)
		{
			pStartIndex = pOtaInfo+i+1;
			break;
		}
		aInfoBuff[i] = pOtaInfo[i];
	}

	res = _parseipaddr(aInfoBuff, pFtpOtaSet->aServerIp);
	if (res != 0)
	{
		DD_MEM_Free(pFtpOtaSet);
		DI_UART_Print("[_setFtpOtaPath] error %08X : _parseipaddr\n", res);
		return res;
	}

	DI_UART_Print("[_setFtpOtaPath] IP : %d.%d.%d.%d\n", pFtpOtaSet->aServerIp[0], pFtpOtaSet->aServerIp[1], pFtpOtaSet->aServerIp[2], pFtpOtaSet->aServerIp[3]);

	/* Get Port Number */
#if 0
	pEndIndex = VK_strchr(pStartIndex,':');
	pPortNum = DD_MEM_Alloc(pEndIndex-pStartIndex+1);
	VK_strncpy(pPortNum, pStartIndex, pEndIndex-pStartIndex);
	pPortNum[pEndIndex-pStartIndex] = NULL;
	pFtpOtaSet->usPortNum = VK_atoi(pPortNum);
	DD_MEM_Free((void *)pPortNum);
	pStartIndex = pEndIndex+1;
	DI_UART_Print("[DI_ETH_SetFtpOtaPath] Port Number : %d \n", pFtpOtaSet->usPortNum);
#endif

	/* Get User Name */
	pEndIndex = VK_strchr(pStartIndex,':');
	VK_strncpy(pFtpOtaSet->aUserName, pStartIndex, pEndIndex-pStartIndex);
	pStartIndex = pEndIndex+1;
	DI_UART_Print("[_setFtpOtaPath] User Name : %s \n", pFtpOtaSet->aUserName);

	/* Get Passwd */
	pEndIndex = VK_strchr(pStartIndex,':');
	VK_strncpy(pFtpOtaSet->aPasswd, pStartIndex, pEndIndex-pStartIndex);
	pStartIndex = pEndIndex+1;
	DI_UART_Print("[_setFtpOtaPath] Passwd : %s \n", pFtpOtaSet->aPasswd);

	/* Get DIR */
	pEndIndex = VK_strchr(pStartIndex,':');
	VK_strncpy(pFtpOtaSet->aDirPath, pStartIndex, pEndIndex-pStartIndex);
	pStartIndex = pEndIndex+1;
	DI_UART_Print("[_setFtpOtaPath] DIR : %s \n", pFtpOtaSet->aDirPath);

	/* Get File Name */
	if((FTP_FILE_NAME_LENGTH -1) < VK_strlen(pStartIndex))
	{
		DI_UART_Print("[_setFtpOtaPath] Length of file name is overflow \n");
		return DI_ERR_ERROR;
	}
	else
	{
		VK_strncpy(pFtpOtaSet->aFileName, pStartIndex, VK_strlen(pStartIndex));
		DI_UART_Print("[_setFtpOtaPath] File Name : %s \n", pFtpOtaSet->aFileName);
	}

	/* Write to NVRAM */
	res = DRV_NVRAM_SetField(DI_NVRAM_FIELD_OTA_FTP_INFO, 0, (void *)pFtpOtaSet, sizeof(DI_FTP_OTA_SETTING_T));
	if (res)
	{
		DI_UART_Print("[_setFtpOtaPath] error %08X : DI_EEPROM_SetField\n", res);
		DD_MEM_Free(pFtpOtaSet);
		return res;
	}

	/* Free OTA Struct */
	DD_MEM_Free(pFtpOtaSet);

	return DI_ERR_OK;
}

int CMD_HwTest_Parser_OTA(void *szArgs)
{
	GET_ARGS;

	if CMD_IS("setpid")
	{
		HUINT32 ulPid;
		HUINT16	usPid;

		CMD_ReadAsHex (HWTEST_PARAM, &ulPid);

		usPid = (HUINT16)ulPid;
		if(DI_NVRAM_Write (DI_NVRAM_FS_PARTITION_RW, ES_PID1, 0, &usPid, sizeof(HUINT16)) != DI_ERR_OK)
		{
			DI_UART_Print ("[%s:%d] Error\n", __FUNCTION__, __LINE__);
			return CMD_ERR;
		}

		DI_UART_Print ("[%s:%d] Set OTA PID (0x%04X)\n", __FUNCTION__, __LINE__, usPid);
	}
	else if CMD_IS("setotaver")
	{
		#define APPL_MODEL_INITIAL_LEN	(6)
		#define APPL_VER_LEN			(4)

		HUINT32	ulAppVersion;
		HUINT8	ucVersion[APPL_MODEL_INITIAL_LEN + APPL_VER_LEN] = {0,};

		CMD_ReadAsHex (HWTEST_PARAM, &ulAppVersion);

		if(DI_NVRAM_Read(DI_NVRAM_FS_PARTITION_RW, APP_VERSION_NEXT, 0, &ucVersion, APPL_MODEL_INITIAL_LEN) != DI_ERR_OK)
		{
			DI_UART_Print ("[%s:%d] Error\n", __FUNCTION__, __LINE__);
			return CMD_ERR;
		}

		ucVersion[APPL_MODEL_INITIAL_LEN]		= (HUINT8)(0x000000FF & (ulAppVersion >> 16));
		ucVersion[APPL_MODEL_INITIAL_LEN + 1]	= (HUINT8)(0x000000FF & (ulAppVersion >> 8));
		ucVersion[APPL_MODEL_INITIAL_LEN + 2]	= (HUINT8)(0x000000FF & ulAppVersion);

		if(DI_NVRAM_Write(DI_NVRAM_FS_PARTITION_RW, APP_VERSION_NEXT, 0, &ucVersion, APPL_MODEL_INITIAL_LEN + APPL_VER_LEN) != DI_ERR_OK)
		{
			DI_UART_Print ("[%s:%d] Error\n", __FUNCTION__, __LINE__);
			return CMD_ERR;
		}

		DI_UART_Print ("[%s:%d] Set next SW version (0x%08X)\n", __FUNCTION__, __LINE__, ulAppVersion);
	}
	else if CMD_IS("setant")
	{
		VK_memset(&s_stAntennaInfo, 0, sizeof(DI_CMD_AntennaInfo_t));

		CMD_ReadAsDec(HWTEST_PARAM, &s_stAntennaInfo.ulLnbFreq);

		if PARAM1_IS("ON")
			s_stAntennaInfo.b22KTone = TRUE;
		else if PARAM1_IS("OFF")
			s_stAntennaInfo.b22KTone = FALSE;
		else
			s_stAntennaInfo.b22KTone = FALSE;

		if PARAM2_IS("A")
			s_stAntennaInfo.etDiseqcInput = DI_CH_DISEQC_INPUT_A;
		else if PARAM2_IS("B")
			s_stAntennaInfo.etDiseqcInput = DI_CH_DISEQC_INPUT_B;
		else if PARAM2_IS("C")
			s_stAntennaInfo.etDiseqcInput = DI_CH_DISEQC_INPUT_C;
		else if PARAM2_IS("D")
			s_stAntennaInfo.etDiseqcInput = DI_CH_DISEQC_INPUT_D;
		else
			s_stAntennaInfo.etDiseqcInput = DI_CH_DISEQC_INPUT_NONE;

		DI_UART_Print("[%s] LnbFreq : %d\n", __FUNCTION__, s_stAntennaInfo.ulLnbFreq);
		DI_UART_Print("[%s] 22KTone    : %d\n", __FUNCTION__, s_stAntennaInfo.b22KTone);
		DI_UART_Print("[%s] DiseqcInput   : %d\n", __FUNCTION__, s_stAntennaInfo.etDiseqcInput);

		s_stAntennaInfo.bAvailable = TRUE;
	}
	else if CMD_IS("settp")
	{
		HUINT32 ulFrequency, ulSymbolRate, ulModulation, ulTunerId;
		
		if PARAM_IS("SAT")
		{
			CH_SAT_TuneParam_t	stSatTuneParam;

			VK_memset(&stSatTuneParam, 0x0, sizeof(CH_SAT_TuneParam_t));
			
			CMD_ReadAsDec(HWTEST_PARAM1, &ulFrequency);
			CMD_ReadAsDec(HWTEST_PARAM2, &ulSymbolRate);
			
			stSatTuneParam.ulFrequency	= ulFrequency;
			stSatTuneParam.ulSymbolRate	= ulSymbolRate;
		
			if PARAM3_IS ("1/2")
			{
				stSatTuneParam.etFecCodeRate = DI_CH_SCODERATE_1_2;
			}
			else if PARAM3_IS ("2/3")
			{
				stSatTuneParam.etFecCodeRate = DI_CH_SCODERATE_2_3;
			}
			else if PARAM3_IS ("3/4")
			{
				stSatTuneParam.etFecCodeRate = DI_CH_SCODERATE_3_4;
			}
			else if PARAM3_IS ("5/6")
			{
				stSatTuneParam.etFecCodeRate = DI_CH_SCODERATE_5_6;
			}
			else if PARAM3_IS ("7/8")
			{
				stSatTuneParam.etFecCodeRate = DI_CH_SCODERATE_7_8;
			}
			else
			{
				DI_UART_Print ("[%s:%d] Error\n", __FUNCTION__, __LINE__);
				return CMD_ERR;
			}
			
			if (s_stAntennaInfo.bAvailable == TRUE)
			{
				stSatTuneParam.etAntennaType 	= DI_CH_ANT_TYPE_DISEQC;
				stSatTuneParam.ulFrequency	= (s_stAntennaInfo.ulLnbFreq > stSatTuneParam.ulFrequency) ? (s_stAntennaInfo.ulLnbFreq - stSatTuneParam.ulFrequency) : (stSatTuneParam.ulFrequency - s_stAntennaInfo.ulLnbFreq);
				stSatTuneParam.etPolarization 	= DI_CH_POLAR_AUTO;
				stSatTuneParam.etTransSpec 	= DI_CH_TRANS_DVBS;
				stSatTuneParam.etModulation	= DI_CH_PSK_QPSK;
				stSatTuneParam.etPilot 		= DI_CH_PSK_AUTO;
				stSatTuneParam.etRollOff 	= DI_CH_ROLL_035;
				stSatTuneParam.etLnbVoltage 	= DI_CH_LNB_VOLT_STD;
				stSatTuneParam.etDiseqcVersion 	= DI_CH_DISEQC_VER_1_0;
				stSatTuneParam.etDiseqcInput 	= s_stAntennaInfo.etDiseqcInput;
				stSatTuneParam.etToneBurst 	= DI_CH_TONE_BURST_NONE;
				stSatTuneParam.b22kTone 	= s_stAntennaInfo.b22KTone;

				if(DI_NVRAM_Write(DI_NVRAM_FS_PARTITION_RW, CH_INFO_S, 0, &stSatTuneParam, sizeof(CH_SAT_TuneParam_t)) != DI_ERR_OK)
				{
					DI_UART_Print ("[%s:%d] Error\n", __FUNCTION__, __LINE__);
					return CMD_ERR;
				}

				DI_UART_Print("[%s] SAT Antenna Type    : %d\n", __FUNCTION__, stSatTuneParam.etAntennaType);
				DI_UART_Print("[%s] SAT TP Real Freq    : %d\n", __FUNCTION__, stSatTuneParam.ulFrequency);
				DI_UART_Print("[%s] SAT TP SymbolRate   : %d\n", __FUNCTION__, stSatTuneParam.ulSymbolRate);
				DI_UART_Print("[%s] SAT TP Code Rate    : %d\n", __FUNCTION__, stSatTuneParam.etFecCodeRate);
				DI_UART_Print("[%s] SAT TP DiSEqC Input : DiSEqC-%c\n", __FUNCTION__, 'A' + stSatTuneParam.etDiseqcInput - DI_CH_DISEQC_INPUT_A);
				DI_UART_Print("[%s] SAT TP 22KTone      : %s\n", __FUNCTION__, (stSatTuneParam.b22kTone != FALSE) ? "ON" : "OFF");
				DI_UART_Print("\n\n");				
			}
			else
			{
				DI_UART_Print ("[%s:%d] Error : Please Set antenna info\n", __FUNCTION__, __LINE__);
				return CMD_ERR;
			}
			
			ulTunerId = 0;
			if(DI_NVRAM_Write(DI_NVRAM_FS_PARTITION_RW, TUNER_ID1, 0, &ulTunerId, sizeof(HUINT8)) != DI_ERR_OK)
			{
				DI_UART_Print ("[%s:%d] Error\n", __FUNCTION__, __LINE__);
				return CMD_ERR;
			}
		}
		else if PARAM_IS("CAB")
		{
			CH_CAB_TuneParam_t	stCabTuneParam;

			CMD_ReadAsDec(HWTEST_PARAM1, &ulFrequency);
			CMD_ReadAsDec(HWTEST_PARAM2, &ulSymbolRate);
			CMD_ReadAsDec(HWTEST_PARAM3, &ulModulation);

			VK_memset(&stCabTuneParam, 0x0, sizeof(CH_CAB_TuneParam_t));
			
			stCabTuneParam.ulFrequency	= ulFrequency;
			stCabTuneParam.ulSymbolRate	= ulSymbolRate;
			stCabTuneParam.etSpectrum	= DI_CH_INVERSION_AUTO;
			switch(ulModulation)
			{
				case 16:	stCabTuneParam.etModulation = DI_CH_CMOD_16QAM;		break;
				case 32:	stCabTuneParam.etModulation = DI_CH_CMOD_32QAM;		break;
				case 64:	stCabTuneParam.etModulation = DI_CH_CMOD_64QAM;		break;
				case 128:	stCabTuneParam.etModulation = DI_CH_CMOD_128QAM;	break;
				case 256:	stCabTuneParam.etModulation = DI_CH_CMOD_256QAM;	break;
				default:	stCabTuneParam.etModulation = DI_CH_CMOD_AUTO;		break;
			}

			if(DI_NVRAM_Write(DI_NVRAM_FS_PARTITION_RW, CH_INFO_C, 0, &stCabTuneParam, sizeof(CH_CAB_TuneParam_t)) != DI_ERR_OK)
			{
				DI_UART_Print ("[%s:%d] Error\n", __FUNCTION__, __LINE__);
				return CMD_ERR;
			}

			DI_UART_Print("[%s] Cable TP Frequency    : %d\n", __FUNCTION__, stCabTuneParam.ulFrequency);
			DI_UART_Print("[%s] Cable TP ulSymbolRate : %d\n", __FUNCTION__, stCabTuneParam.ulSymbolRate);
			DI_UART_Print("[%s] Cable TP Modulation   : %d\n", __FUNCTION__, stCabTuneParam.etModulation);

			ulTunerId = 0;
			if(DI_NVRAM_Write(DI_NVRAM_FS_PARTITION_RW, TUNER_ID1, 0, &ulTunerId, sizeof(HUINT8)) != DI_ERR_OK)
			{
				DI_UART_Print ("[%s:%d] Error\n", __FUNCTION__, __LINE__);
				return CMD_ERR;
			}
		}
		else
		{
			DI_UART_Print ("[%s:%d] Error\n", __FUNCTION__, __LINE__);
			return CMD_ERR;
		}
	}
	else if CMD_IS("setftpinfo")
	{
		if(_setFtpOtaPath(HWTEST_PARAM) != DI_ERR_OK)
		{
			DI_UART_Print ("[%s:%d] Error\n", __FUNCTION__, __LINE__);
			return CMD_ERR;
		}
	}
	else if CMD_IS("usb2raw")
	{
		HUINT32			 ulFileSize, ulReadSize;
		HUINT32			ulUsbNum=0,	 ulRetryCnt;
		HUINT8			*pucBuffer = NULL;
		HCHAR			 szFilePath[48];
		FILE	 		*fp = NULL;
		HINT32			 hResult = ERR_FAIL;
		HCHAR			*szFileName;

		if(HWTEST_PARAM != NULL)
			VK_snprintf (szFilePath, 48, "%s/%s", "/media/drive1", HWTEST_PARAM);
		else
			VK_snprintf (szFilePath, 48, "%s/%s", "/media/drive1", "hdpvr.hdf");

		szFilePath[47] = '\0';

		fp = fopen (szFilePath, "rb");
		if (NULL == fp)
		{
			DI_UART_Print ("[%s:%d] Error:\n", __FUNCTION__, __LINE__);
			goto END_FUNC;
		}

		fseek (fp, 0, SEEK_END);
		ulFileSize = ftell(fp);
		if (ulFileSize == 0)
		{
			DI_UART_Print ("[%s:%d] Error:\n", __FUNCTION__, __LINE__);
			goto END_FUNC;
		}

		DI_UART_Print ("[%s] Size : %d\n", __FUNCTION__, ulFileSize);

		pucBuffer = VK_MEM_Alloc(ulFileSize);
		if (NULL == pucBuffer)
		{
			DI_UART_Print ("[%s:%d] Error:\n", __FUNCTION__, __LINE__);
			goto END_FUNC;
		}

		fseek(fp, 0, SEEK_SET);

		ulReadSize = fread(pucBuffer, sizeof(HUINT8), ulFileSize, fp);
		if (ulReadSize != ulFileSize)
		{
			DI_UART_Print ("[%s:%d] Errro (ReqSize:%d, RealSize:%d)\n", __FUNCTION__, __LINE__, ulFileSize, ulReadSize);
			goto END_FUNC;
		}

		if(DI_NVRAM_Write(DI_NVRAM_PARTITION_W_RAWFS, NULL, 0, pucBuffer, ulFileSize) != DI_ERR_OK)
		{
			DI_UART_Print ("[%s:%d] DI_NVRAM_Write Err\n", __FUNCTION__, __LINE__);
			goto END_FUNC;
		}

		hResult = ERR_OK;

	END_FUNC:
		if (NULL != fp)
		{
			fclose(fp);
		}

		if (NULL != pucBuffer)
		{
			VK_MEM_Free(pucBuffer);
		}

		DI_UART_Print ("[%s] write %s!!\n", __FUNCTION__, hResult == ERR_OK ? "ok" : "error");
	}
	else if CMD_IS("setotatype")
	{
		DI_NVRAM_OTA_TYPE_E		eOtaType;
		DI_NVRAM_OTA_CHANNEL_E	eOtaChan1;
		DI_NVRAM_OTA_FLAG_E		eOtaFlag1 = DI_NVRAM_OTA_FLAG_DETECTED;
		DI_ERR_CODE				eDiErr = DI_ERR_OK;

		if PARAM_IS("BG")
		{
			eOtaType = DI_NVRAM_OTA_FILE;
			eOtaChan1 = DI_NVRAM_OTA_CHANNEL_FLASH;
		}
		else if	PARAM_IS("SAT")
		{
			eOtaType = DI_NVRAM_OTA_SSU;
			eOtaChan1 = DI_NVRAM_OTA_CHANNEL_SAT;
		}
		else if	PARAM_IS("CAB")
		{
			eOtaType = DI_NVRAM_OTA_SSU;
			eOtaChan1 = DI_NVRAM_OTA_CHANNEL_CAB;
		}
		else if	PARAM_IS("TER")
		{
			eOtaType = DI_NVRAM_OTA_SSU;
			eOtaChan1 = DI_NVRAM_OTA_CHANNEL_TER;
		}
		else if	PARAM_IS("FTP")
		{
			eOtaType = DI_NVRAM_OTA_FILE;
			eOtaChan1 = DI_NVRAM_OTA_CHANNEL_FTP;
		}
		else
		{
			DI_UART_Print ("[%s:%d] Error\n", __FUNCTION__, __LINE__);
			return CMD_ERR;
		}

		eDiErr |= DI_NVRAM_Write (DI_NVRAM_FS_PARTITION_RW, OTA_TYPE1, 0, &eOtaType, sizeof(DI_NVRAM_OTA_TYPE_E));
		eDiErr |= DI_NVRAM_Write (DI_NVRAM_FS_PARTITION_RW, OTA_FLAG1, 0, &eOtaFlag1, sizeof(DI_NVRAM_OTA_FLAG_E));
		eDiErr |= DI_NVRAM_Write (DI_NVRAM_FS_PARTITION_RW, CH_TYPE1, 0, &eOtaChan1, sizeof(DI_NVRAM_OTA_CHANNEL_E));

		if(eDiErr != DI_ERR_OK)
		{
			DI_UART_Print ("[%s:%d] Error\n", __FUNCTION__, __LINE__);
			return CMD_ERR;
		}

		DI_UART_Print ("[%s:%d] Set OTA flag success, The system will reboot !!!\n", __FUNCTION__, __LINE__);

		VK_TASK_Sleep(2000);

		DI_NANDFLASH_UmountPartitions();
		DI_POWER_SetMainChipReset();
	}
	else
	{
		return CMD_ERR;
	}
}

#undef HWTEST_CMD
#undef HWTEST_PARAM
#undef HWTEST_PARAM1
#undef GET_ARGS
#undef CMD_IS
#undef PARAM_IS
#undef PARAM1_IS
#undef READABLE_IN_HEX
#undef READABLE_IN_DEC
#undef GET_NEXT_ARG
