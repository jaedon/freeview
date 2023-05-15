/*******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory Application
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
*******************************************************************************/

/*******************************************************************************
* header and definitions
*******************************************************************************/
/* global header files */

/* chip manufacture's  header files */

/* humax header files */
#include "fvk.h"
#include "fai_common.h"
#include "fai_msgbox.h"
#include "fai_testmenu.h"
#include "fai_testparams.h"
#include "fai_util.h"
#include "fai_mes.h"
#include "fai_system.h"
#include "fai_util.h"
#include "fai_tmd.h"
#include "fdi_mes.h"
#include "fdi_system.h"
#include "fdi_panel.h"
#include "fdi_key.h"
#include "fdi_crypt.h"
#include "fdi_hdd.h"
/* model specific configuration header */
#include "factorysw_config.h"

/*******************************************************************************
* Debug level
*******************************************************************************/

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/
#define FAI_MES_RX_TIMEOUT         10*1000 /* 10 sec */

#define FAI_MES_STX                0x02
#define FAI_MES_ETX                0x03
#define FAI_MES_ACK                0x06

#define FAI_MES_CMD_CHECK_PC       0x50
#define FAI_MES_CMD_OK_PC          0x51
#define FAI_MES_CMD_DOWNLOAD       0x52
#define FAI_MES_CMD_END            0x53
#define FAI_MES_CMD_UPLOAD         0x54
#define FAI_MES_CMD_OPT            0x56
#define FAI_MES_CMD_CHECK_DATA	   0x58
#define FAI_MES_DATA_OPT_CHECK     0xBB

#define FAI_MES_ERR_DOWN_LEN       0xE1
#define FAI_MES_ERR_DOWN_CRC       0xE2
#define FAI_MES_ERR_DOWN_COMPARE   0xE3
#define FAI_MES_ERR_DOWN_VERIFY    0xE4

#define FAI_MES_FORMAT_VERSION     5

#define FAI_MES_CHIPID_BYTE_CNT    4
#define FAI_MES_SSTINFO_BYTE_CNT   14
#define FAI_MES_ANTIFUSE_BYTE_CNT  8

#define FAI_MES_HEADER_BLOCK_SIZE 	12
#define FAI_MES_DEFAULT_DATA_BLOCK_SIZE 	8

typedef struct
{
	unsigned short usHeaderSize;
	unsigned short usHeaderCRC16;
	unsigned short usFormatVer;
	unsigned short usDataBlockCnt;
	unsigned long  ulDataTotalSize;
} FAI_MES_HEADER_BLOCK;

typedef struct
{
	unsigned short usDataSize;
	unsigned short usDataCRC16;
	unsigned short usDataFlag;
	unsigned short usDataOrgSize;
	unsigned char *pucDataBuf;
} FAI_MES_DATA_BLOCK;

typedef enum
{
	FAI_MES_DATA_FLAG_SN = 0x10,
	FAI_MES_DATA_FLAG_MAC,
	FAI_MES_DATA_FLAG_HDCP,
	FAI_MES_DATA_FLAG_CHIPID,
	FAI_MES_DATA_FLAG_DTCP,
	FAI_MES_DATA_FLAG_PAIRKEY,
	FAI_MES_DATA_FLAG_NDSID,
	FAI_MES_DATA_FLAG_SSTINFO,
	FAI_MES_DATA_FLAG_SIM,
	FAI_MES_DATA_FLAG_CI_DATA,
	FAI_MES_DATA_FLAG_CI_ID = 0x20,
	FAI_MES_DATA_FLAG_MACADDR_2,
	FAI_MES_DATA_FLAG_DID,
	FAI_MES_DATA_FLAG_SVP_DATA,
	FAI_MES_DATA_FLAG_2ND_SN,
	FAI_MSS_DATA_FLAG_SVP_RETURN_DATA,
	FAI_MES_DATA_FLAG_DSTB_ID,
	FAI_MES_DATA_FLAG_CSC_KEY = 0x27,
	FAI_MSS_DATA_FLAG_VIACCESS_BBX_DATA,
	FAI_MSS_DATA_FLAG_MENUFACTURE_DATE,
	FAI_MSS_DATA_FLAG_TP_AUTHENTICATION,
	FAI_MSS_DATA_FLAG_SM_AUTHENTICATION = 0x31,
	FAI_MES_DATA_FLAG_HDD_SN = 0x37,
	FAI_MES_DATA_FLAG_MACADDR_3 = 0x38,
	FAI_MES_DATA_FLAG_MACADDR_4 = 0x39,
	FAI_MES_DATA_FLAG_MACADDR_EXT = 0x40,
	FAI_MSS_DATA_FLAG_RF4CE_IEEE_ADDR = 0x54,
	FAI_MSS_DATA_FLAG_IRDETO_PRIVATEDATA = 0x61,
#if defined(FACTORY_USE_WIFI)
	FAI_MES_DATA_FLAG_WIFIMAC1 = 0x78,
#endif
	FAI_MSS_DATA_FLAG_LOADER_VER = 0xA0,
	FAI_MSS_DATA_FLAG_APP_VER = 0xA1,
	FAI_MSS_DATA_FLAG_MICOM_VER = 0xA2,
	FAI_MSS_DATA_FLAG_SYSTEM_ID = 0xA3,
	FAI_MSS_DATA_FLAG_SDCARD_SN = 0xA4,	
	FAI_MES_DATA_FLAG_VERIMATRIX_NSC_DATA, /* upload item  -> PASL client */
	FAI_MES_DATA_FLAG_VERIMATRIX_CAS_DATA /* download item <-PASL */
} FAI_MES_DATA_FLAG;

typedef struct
{
	unsigned char ucIsRsaMode;
	unsigned char ucIsJtag;
	unsigned char ucIsCwMode;
	unsigned short usSysId;
} FAI_MES_SST_INFO;

/* Macro */
#define FAI_MES_SEND_BYTE(x)      if(INTRN_FAI_MES_SendByte(x)) return FAI_ERR;
#define FAI_MES_RECV_BYTE(x)      if(INTRN_FAI_MES_RecvByte(x)) return FAI_ERR;
#define FAI_MES_SEND_BUFFER(x, y) if(INTRN_FAI_MES_SendBuffer(x, y)) return FAI_ERR;
#define FAI_MES_RECV_BUFFER(x, y) if(INTRN_FAI_MES_RecvBuffer(x, y)) return FAI_ERR;

/*******************************************************************************
* External variables and functions
*******************************************************************************/
/* external variables  and functions */

/*******************************************************************************
* Global variables and structures
*******************************************************************************/
/* global variables and structures */

FAI_ERR_CODE INTRN_FAI_MES_DownloadPrepare(unsigned int unDeviceType, unsigned int unKeyCode);
static FAI_ERR_CODE INTRN_FAI_MES_StartSection(void);
static FAI_ERR_CODE INTRN_FAI_MES_UploadSection(void);
static FAI_ERR_CODE INTRN_FAI_MES_UploadData(unsigned short usDataFlag, unsigned short usDataSize);
static FAI_ERR_CODE INTRN_FAI_MES_DownloadSection(void);
static FAI_ERR_CODE INTRN_FAI_MES_CheckDownloadDataSection(void);
static FAI_ERR_CODE INTRN_FAI_MES_OTPSection(void);
static FAI_ERR_CODE INTRN_FAI_MES_End(void);
static FAI_ERR_CODE INTRN_FAI_MES_SendByte(unsigned char ucByte);
static FAI_ERR_CODE INTRN_FAI_MES_RecvByte(unsigned char ucRevByte);
static FAI_ERR_CODE INTRN_FAI_MES_SendBuffer(unsigned char *pucBuf, unsigned long ulBufSize);
static FAI_ERR_CODE INTRN_FAI_MES_RecvBuffer(unsigned char *pucBuf, unsigned long ulBufSize);
static FAI_ERR_CODE INTRN_FAI_MES_GetHeaderBlock(unsigned char *pucBuf, FAI_MES_HEADER_BLOCK *ptMesHeaderBlock);
static FAI_ERR_CODE INTRN_FAI_MES_GetDataBlock(unsigned char *pucBuf, FAI_MES_DATA_BLOCK *ptMesDataBlock);
static FAI_ERR_CODE INTRN_FAI_MES_SetSerialNumber(unsigned char *pucBuf, unsigned short usSize);
static FAI_ERR_CODE INTRN_FAI_MES_SetMacAddress(unsigned char *pucBuf, unsigned short usSize);
static FAI_ERR_CODE INTRN_FAI_MES_SetHdcpKey(unsigned char *pucBuf, unsigned short usSize);
static FAI_ERR_CODE INTRN_FAI_MES_SetDtcpKey(unsigned char *pucBuf, unsigned short usSize);
static FAI_ERR_CODE INTRN_FAI_MES_SetCiPlusKeyData(unsigned char *pucBuf, unsigned short usSize);
static FAI_ERR_CODE INTRN_FAI_MES_SetPairKey(unsigned char *pucBuf, unsigned short usSize);
static FAI_ERR_CODE INTRN_FAI_MES_SetDstbId(unsigned char *pucBuf, unsigned short usSize);
static FAI_ERR_CODE INTRN_FAI_MES_SetVAExtraData(unsigned char *pucBuf, unsigned short usSize);
static FAI_ERR_CODE INTRN_FAI_MES_SetCSCKey(unsigned char *pucBuf, unsigned short usSize);
static FAI_ERR_CODE INTRN_FAI_MES_Download();
#if defined(CONFIG_DI20)
static FAI_ERR_CODE INTRN_FAI_MES_SetSecondMacAddress(unsigned char * pucBuf,unsigned short usSize);
static FAI_ERR_CODE INTRN_FAI_MES_SetThirdMacAddress(unsigned char * pucBuf,unsigned short usSize);
#endif
#if defined(FACTORY_USE_CABLEMODEM)			
static FAI_ERR_CODE INTRN_FAI_MES_SetCmMacAddress(unsigned char * pucBuf,unsigned short usSize);
#endif
static FAI_ERR_CODE INTRN_FAI_MES_SetIEEEAddr(unsigned char *pucBuf, unsigned short usSize);
static FAI_ERR_CODE INTRN_FAI_MES_SetPrivateData(unsigned char *pucBuf, unsigned short usSize);
static FAI_ERR_CODE INTRN_FAI_MES_SetVerimatrixCasData(unsigned char *pucBuf, unsigned short usSize);

#if defined(FACTORY_USE_BT)
static FAI_ERR_CODE INTRN_FAI_MES_SetBTMacAddress(unsigned char *pucBuf, unsigned short usSize);
#endif

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_MES_Init(void)
{

	return FAI_NO_ERR;
}
	
FAI_ERR_CODE FAI_MES_Start(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	unsigned char ucTmdErrCode[2];
	FAI_TMD_RESULT_T tTMDResult;
	FAI_ERR_CODE eFaiErr;
	FAI_TMD_ERR eTmdErr = FAI_TMD_ERR_OK;

	FVK_MEM_Set( (void *)&tTMDResult, 0x00, sizeof(tTMDResult));
	
	if( ucArgc != 0 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Argument Count Error=%d\n",  __func__, ucArgc));
		
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
		FAI_TMD_SetResult(&tTMDResult);
		return FAI_ERR;
	}

	FVK_TASK_Sleep( (unsigned long)2000);

	eFaiErr = INTRN_FAI_MES_Download();
	if(eFaiErr!= FAI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AUDIO_HwMute] : FDI_AUDIO_SetHWtMute Error\n"));
		eTmdErr = FAI_TMD_ERR_FAIL;
		switch(eFaiErr)
		{
			case FAI_ERR_MES_CONNECT:
				ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_MES;
				ucTmdErrCode[1] = FAI_MES_TMD_ERR_CONNECT;
				break;
			case FAI_ERR_MES_UPLOAD:
				ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_MES;
				ucTmdErrCode[1] = FAI_MES_TMD_ERR_UPLOAD;
				break;
			case FAI_ERR_MES_DOWNLOAD:
				ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_MES;
				ucTmdErrCode[1] = FAI_MES_TMD_ERR_DOWNLOAD;
				break;
			case FAI_ERR_MES_CHECK_DOWNLOAD:
				ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_MES;
				ucTmdErrCode[1] = FAI_MES_TMD_ERR_CHECK_DOWNLOAD;
				break;
			case FAI_ERR_MES_OTP:
				ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_MES;
				ucTmdErrCode[1] = FAI_MES_TMD_ERR_OTP;
				break;
			default:
				ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
				ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_FAIL;
				break;
		}
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
		goto END;
	}

END:
	tTMDResult.eResult= eTmdErr;
	FAI_TMD_SetResult(&tTMDResult);
	
	return FAI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_MES_Download_UI(void)
{
	FAI_MSGBOX_H hCurMsgBox;
	//[tenchi] prevent
	char szMesPrepareInfo[3][FAI_MSG_STR_MAX]= {0,0,0};

	sprintf(szMesPrepareInfo[0], "Do you want MES Download??");
	sprintf(szMesPrepareInfo[1], "Please, Push \"%s\" key from Front Panel.", FKeyCode2Str(MES_DOWNLOAD_UI_FRONT_INPUT));
	sprintf(szMesPrepareInfo[2], "Otherwise, Push \"EXIT/BACK\" key from RCU.");
		
	FDI_PANEL_Display(FAI_MES_DOWNLOAD_READY_STR);
	
  	FAI_MSGBOX_Create(&hCurMsgBox,
									"DOWNLOAD MODE",
									szMesPrepareInfo,
									3,
									INTRN_FAI_MES_DownloadPrepare);
 	
	return FAI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if (PRODUCT_YSR2000==YES)
#if ( WIFI_MODULE_TYPE == WIFI_MODULE_QT3490)
FAI_ERR_CODE INTRN_FAI_MES_Get_QT_WIFI_State(int *uiState)
{
	FAI_ERR_CODE eFaiErr = FAI_ERR;
	unsigned char szSystemCommand[100];
	FILE *fp;
	unsigned char input[256];
	char	*pLine;
	int i=0;
	int nAccess;
	
	while(1)
	{
	
		access("/tmp/res",0);
	 	if(nAccess == 0)
	 	{
	 		VK_SYSTEM_Command("rm -rf /tmp/res");
	 	}
		FVK_MEM_Set(szSystemCommand, 0x00, sizeof(szSystemCommand));
		FVK_MEM_Set(input, 0x00, sizeof(input));
		sprintf(szSystemCommand, "call_qcsapi_sockrpc --host 169.254.119.1 get_bootcfg_param calstate > /tmp/res");
		VK_SYSTEM_Command(szSystemCommand);

		*uiState=0;

		//state or wifi connect가 가능한지 확인하기 위한 코드
		fp = fopen("/tmp/res","r");
		if(fp != NULL)
		{
			pLine = fgets(input, 255, fp);
			if(pLine != NULL)
			{			
				pLine = NULL;
				if((pLine = strstr(input,"RPC:")) != NULL)
				{
					eFaiErr = FAI_ERR;
					printf("[INTRN_FAI_MES_Get_QT_WIFI_State] wait for wifi connecting...[%d]\n",i);
				}
				else if(!strncmp(input,"0",1))
				{
					printf("[INTRN_FAI_MES_Get_QT_WIFI_State] invalid wifi calstate[0]...\n");
					*uiState=0;
					eFaiErr=FAI_NO_ERR; /* cal state !=3 한 경우 no error처리 */
				}			
				else if(!strncmp(input,"1",1))
				{
					printf("[INTRN_FAI_MES_Get_QT_WIFI_State] invalid wifi calstate[1]...\n");
					*uiState=1;
					eFaiErr=FAI_NO_ERR; /* cal state !=3 한 경우 no error처리 */
				}
				else if(!strncmp(input,"2",1))
				{
					printf("[INTRN_FAI_MES_Get_QT_WIFI_State] invalid wifi calstate[2]...\n");
					*uiState=2;
					eFaiErr=FAI_NO_ERR; /* cal state !=3 한 경우 no error처리 */
				}
				else if(!strncmp(input,"3",1))
				{
					printf("[INTRN_FAI_MES_Get_QT_WIFI_State] valid wifi calstate...[3]\n");
					*uiState=3;
					eFaiErr=FAI_NO_ERR; /* cal state !=3 한 경우 no error처리 */
				}						
				else
				{
					eFaiErr = FAI_NO_ERR;
					printf("[INTRN_FAI_MES_Get_QT_WIFI_State] invalid wifi calstate\n");
				}
			}
			fclose(fp);
		}

		if((eFaiErr==FAI_NO_ERR)||(*uiState>2))
		{
			break;
		}
		else
		{
			FVK_TASK_Sleep(500);
			if(i++>60)
			{
				/* max wait 25 seconds for wifi ic booting */
				break;
			}
			else
			{
				continue;
			}
		}		
	}
	return eFaiErr;
}
#endif
#endif


/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/

FAI_ERR_CODE INTRN_FAI_MES_DownloadPrepare(unsigned int unDeviceType, unsigned int unKeyCode)
{
	FAI_MSGBOX_H hCurMsgBox;
	FAI_ERR_CODE eFaiErr = FAI_ERR;
	FDI_ERR_CODE eFdiErr = FDI_ERR;
	int *ulState;
	
	if( unDeviceType == TEST_INPUT_TYPE_FKEY && unKeyCode == MES_DOWNLOAD_UI_FRONT_INPUT )
	{
		//FAI_ERR_CHECK(FAI_MSGBOX_GetCurrentMsgBox(&hCurMsgBox));
		//FAI_ERR_CHECK(FAI_MSGBOX_Destoy(hCurMsgBox));
		FAI_MSGBOX_GetCurrentMsgBox(&hCurMsgBox);
		FAI_MSGBOX_Destoy(hCurMsgBox);
		
		FDI_PANEL_Display(FAI_MES_DOWNLOAD_START_STR);

#if (PRODUCT_YSR2000==YES)
#if ( WIFI_MODULE_TYPE == WIFI_MODULE_QT3490)
		FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_YELLOW);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "MES Download", RGB_COLOR_YELLOW);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "WIFI IC", RGB_COLOR_YELLOW);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_4, "Checking", RGB_COLOR_YELLOW);		

		eFaiErr = INTRN_FAI_MES_Get_QT_WIFI_State(&ulState);
		if((eFaiErr!=FAI_NO_ERR)||(ulState<3))
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_DownloadPrepare] WIFI Calstate Invalid state!!(%d)\n",ulState));
			goto TEST_FAIL;
		}
#endif
#endif
		FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_YELLOW);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "MES Download", RGB_COLOR_YELLOW);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "0%", RGB_COLOR_YELLOW);

#if(MES_DOWNLOAD_UPLOAD_LIST&MES_SDCARD_SN)
		FDI_MMC_Mount(1, "/mnt/sd0");
		FVK_TASK_Sleep(2000);
#endif
		/* connect */
		eFdiErr = FDI_MES_Open();
		if( eFdiErr != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_DownloadPrepare] FDI_MES_Open Error : 0x%08x\n", eFdiErr));
			goto TEST_FAIL;
		}

		/* start section */
		eFaiErr = INTRN_FAI_MES_StartSection();
		if( eFaiErr != FAI_NO_ERR )
		{
			goto TEST_FAIL;
		}
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "10%", RGB_COLOR_YELLOW);
		FVK_TASK_Sleep( (unsigned long)100);
		
		/* upload section */
		eFaiErr = INTRN_FAI_MES_UploadSection();
		if( eFaiErr != FAI_NO_ERR )
		{
			goto TEST_FAIL;
		}

		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "30%", RGB_COLOR_YELLOW);
		FVK_TASK_Sleep( (unsigned long)100);

		/* download section */
		eFaiErr = INTRN_FAI_MES_DownloadSection();
		if( eFaiErr != FAI_NO_ERR )
		{
			goto TEST_FAIL;
		}
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "70%", RGB_COLOR_YELLOW);
		FVK_TASK_Sleep( (unsigned long)100);

		eFaiErr = INTRN_FAI_MES_CheckDownloadDataSection();
		if( eFaiErr != FAI_NO_ERR )
		{
			goto TEST_FAIL;
		}

		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "80%", RGB_COLOR_YELLOW);		
		FVK_TASK_Sleep( (unsigned long)100);

		/* */
		eFaiErr = INTRN_FAI_MES_OTPSection();
		if( eFaiErr != FAI_NO_ERR )
		{
			goto TEST_FAIL;
		}
		FVK_TASK_Sleep(100);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "90%", RGB_COLOR_YELLOW);

		/* */
		eFaiErr = INTRN_FAI_MES_End();
		if( eFaiErr != FAI_NO_ERR )
		{
			goto TEST_FAIL;
		}
		FDI_MES_Close();
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "100%", RGB_COLOR_YELLOW);

		FDI_PANEL_Display(FAI_MES_DOWNLOAD_PASS_STR);
		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "MES Download", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Pass", RGB_COLOR_BLACK);

#if (INFORMATION_OSD_UI_ENABLE==YES)
		FAI_SYSTEM_DisplayInfoMessageBox();
#endif		
	}
	else if( unDeviceType == TEST_INPUT_TYPE_RCU && (unKeyCode == RCU_KEY_EXIT||unKeyCode == RCU_KEY_BACK) )
	{
		//FAI_ERR_CHECK(FAI_MSGBOX_GetCurrentMsgBox(&hCurMsgBox));
		//FAI_ERR_CHECK(FAI_MSGBOX_Destoy(hCurMsgBox));
		FAI_MSGBOX_GetCurrentMsgBox(&hCurMsgBox);
		FAI_MSGBOX_Destoy(hCurMsgBox);
	}

	return eFaiErr;
TEST_FAIL:
	FDI_MES_Close();
	FDI_PANEL_Display(FAI_MES_DOWNLOAD_FAIL_STR);
	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_RED);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "MES Download", RGB_COLOR_RED);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Fail", RGB_COLOR_RED);
	
	return eFaiErr;
}


static FAI_ERR_CODE INTRN_FAI_MES_Download()
{
	FAI_MSGBOX_H hCurMsgBox;
	FAI_ERR_CODE eFaiErr = FAI_ERR;
	FDI_ERR_CODE eFdiErr = FDI_ERR;
	int nErrCnt = 0;
	int *ulState;
		
	/* connect */
	
#if (PRODUCT_YSR2000==YES)
#if ( WIFI_MODULE_TYPE == WIFI_MODULE_QT3490)
	eFaiErr = INTRN_FAI_MES_Get_QT_WIFI_State(&ulState);
	if((eFaiErr!=FAI_NO_ERR)||(ulState<3))
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_Download] WIFI Calstate Invalid state!!(%d)\n",ulState));
		goto TEST_FAIL;
	}
#endif
#if(MES_DOWNLOAD_UPLOAD_LIST&MES_SDCARD_SN)
		FDI_MMC_Mount(1, "/mnt/sd0");
		FVK_TASK_Sleep(2000);
#endif
#endif

	for(nErrCnt = 0; nErrCnt < 5; nErrCnt++)
	{
		eFdiErr = FDI_MES_Open();
		if( eFdiErr == FDI_NO_ERR )
		{
			break;
		}		
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_DownloadPrepare] FDI_MES_Open Error : 0x%08x nErrCnt[%d/5]\n", eFdiErr,nErrCnt));
		FVK_TASK_Sleep(1000);
	}
	
	if( nErrCnt == 5 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_DownloadPrepare] FDI_MES_Open Error : 0x%08x\n", eFdiErr));
		eFaiErr = FAI_ERR_MES_CONNECT;
		goto TEST_FAIL;
	}

	/* start section */
	eFaiErr = INTRN_FAI_MES_StartSection();
	if( eFaiErr != FAI_NO_ERR )
	{
		goto TEST_FAIL;
	}
	FVK_TASK_Sleep(100);
	
	/* upload section */
	eFaiErr = INTRN_FAI_MES_UploadSection();
	if( eFaiErr != FAI_NO_ERR )
	{
		eFaiErr = FAI_ERR_MES_UPLOAD;
		goto TEST_FAIL;
	}

	FVK_TASK_Sleep(100);

	/* download section */
	eFaiErr = INTRN_FAI_MES_DownloadSection();
	if( eFaiErr != FAI_NO_ERR )
	{
		eFaiErr = FAI_ERR_MES_DOWNLOAD;
		goto TEST_FAIL;
	}
	FVK_TASK_Sleep(100);

	eFaiErr = INTRN_FAI_MES_CheckDownloadDataSection();
	if( eFaiErr != FAI_NO_ERR )
	{
		eFaiErr = FAI_ERR_MES_CHECK_DOWNLOAD;
		goto TEST_FAIL;
	}

	FVK_TASK_Sleep(100);

	/* */
	eFaiErr = INTRN_FAI_MES_OTPSection();
	if( eFaiErr != FAI_NO_ERR )
	{
		eFaiErr = FAI_ERR_MES_OTP;
		goto TEST_FAIL;
	}
	FVK_TASK_Sleep(100);
	/* */
	eFaiErr = INTRN_FAI_MES_End();
	if( eFaiErr != FAI_NO_ERR )
	{
		goto TEST_FAIL;
	}
	
TEST_FAIL:	
	FDI_MES_Close();

	return eFaiErr;
}



/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static FAI_ERR_CODE INTRN_FAI_MES_StartSection(void)
{
	int i;
	FDI_ERR_CODE  eFdiErr;
	unsigned char ucByte;

	FAI_MES_SEND_BYTE(FAI_MES_STX);
	FAI_MES_SEND_BYTE(FAI_MES_CMD_CHECK_PC);
	FAI_MES_SEND_BYTE(FAI_MES_ETX);

	FAI_MES_RECV_BYTE(FAI_MES_STX);
	FAI_MES_RECV_BYTE(FAI_MES_CMD_OK_PC);
	FAI_MES_RECV_BYTE(FAI_MES_ETX);

	return FAI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static FAI_ERR_CODE INTRN_FAI_MES_UploadSection(void)
{
	FAI_ERR_CODE  eFaiErr;
	unsigned char ucByte;
	unsigned char aucHeaderBlock[12];
	unsigned long ulDataSize = 0;
	unsigned short usCrc16, usNumOfDataBlk = 0;
	int i;
	
	FAI_MES_RECV_BYTE(FAI_MES_STX);
	FAI_MES_RECV_BYTE(FAI_MES_CMD_UPLOAD);

	/* send header block */
	aucHeaderBlock[0] = 0x00;	// header block size
	aucHeaderBlock[1] = FAI_MES_HEADER_BLOCK_SIZE;
	aucHeaderBlock[4] = 0x00;	// version
	aucHeaderBlock[5] = FAI_MES_FORMAT_VERSION;
#if(MES_DOWNLOAD_UPLOAD_LIST&MES_SERIAL_NUMBER)
	usNumOfDataBlk++;
	ulDataSize += FAI_MES_DEFAULT_DATA_BLOCK_SIZE+FDI_SYSTEM_DATA_SN_SIZE;
#endif
#if(MES_DOWNLOAD_UPLOAD_LIST&MES_MAC_ADDRESS_1)
	usNumOfDataBlk++;
	ulDataSize += FAI_MES_DEFAULT_DATA_BLOCK_SIZE+FDI_SYSTEM_DATA_MAC_SIZE;
#endif
#if(MES_DOWNLOAD_UPLOAD_LIST&MES_HDCP_KEY)
	usNumOfDataBlk++;
	ulDataSize += FAI_MES_DEFAULT_DATA_BLOCK_SIZE+FDI_SYSTEM_DATA_HDCPKEY_SIZE;
#endif
#if(MES_DOWNLOAD_UPLOAD_LIST&MES_CHIP_ID)
	usNumOfDataBlk++;
	ulDataSize += FAI_MES_DEFAULT_DATA_BLOCK_SIZE+FDI_SYSTEM_DATA_CHIPID_SIZE;
#endif
#if(MES_DOWNLOAD_UPLOAD_LIST&MES_DTCP_KEY)
	usNumOfDataBlk++;
	ulDataSize += FAI_MES_DEFAULT_DATA_BLOCK_SIZE+FDI_SYSTEM_DATA_DTCPKEY_SIZE;
#endif
#if(MES_DOWNLOAD_UPLOAD_LIST&MES_NAGRA_PAIRING_DATA)
	usNumOfDataBlk++;
	ulDataSize += FAI_MES_DEFAULT_DATA_BLOCK_SIZE+FDI_SYSTEM_DATA_PAIRKEY_SIZE;
#endif
#if(MES_DOWNLOAD_UPLOAD_LIST&MES_DSTB_ID)
		usNumOfDataBlk++;
		ulDataSize += FAI_MES_DEFAULT_DATA_BLOCK_SIZE+FDI_SYSTEM_DATA_DSTBID_SIZE;
#endif
#if(MES_DOWNLOAD_UPLOAD_LIST&MES_NAGRA_CHIP_ID)
	usNumOfDataBlk++;
	ulDataSize += FAI_MES_DEFAULT_DATA_BLOCK_SIZE+(FDI_SYSTEM_DATA_CHIPID_SIZE*2);
#endif
#if(MES_DOWNLOAD_UPLOAD_LIST&MES_RF4CE_IEEE_ADDR)
	usNumOfDataBlk++;
	ulDataSize += FAI_MES_DEFAULT_DATA_BLOCK_SIZE+(FDI_SYSTEM_DATA_RF4CE_IEEE_ADDR_SIZE);
#endif
#if(MES_DOWNLOAD_UPLOAD_LIST&MES_MAC_ADDRESS_EXT)
	usNumOfDataBlk++;
	ulDataSize += FAI_MES_DEFAULT_DATA_BLOCK_SIZE+(FDI_SYSTEM_DATA_MAC_SIZE);
#endif
#if(MES_DOWNLOAD_UPLOAD_LIST&MES_HDD_SN)
	usNumOfDataBlk++;
	ulDataSize += FAI_MES_DEFAULT_DATA_BLOCK_SIZE+(FDI_SYSTEM_DATA_HDD_SN_SIZE);
	FAI_PRINT(FAI_PRT_ERROR, ("INTRN_FAI_MES_UploadSection] MES_HDD_SN, usNumOfDataBlk(%d) ulDataSize(%d)\n", usNumOfDataBlk, ulDataSize));
#endif
#if(MES_DOWNLOAD_UPLOAD_LIST&MES_VERIMATRIX_NSC_DATA)
	usNumOfDataBlk++;
	ulDataSize += FAI_MES_DEFAULT_DATA_BLOCK_SIZE+(48/*FDI_SYSTEM_DATA_VERIMATRIX_NSC_DATA_SIZE*/);
	FAI_PRINT(FAI_PRT_ERROR, ("INTRN_FAI_MES_UploadSection] FDI_SYSTEM_DATA_VERIMATRIX_NSC_DATA, usNumOfDataBlk(%d) ulDataSize(%d)\n", usNumOfDataBlk, ulDataSize));
#endif
#if(MES_DOWNLOAD_UPLOAD_LIST&MES_SDCARD_SN)
	usNumOfDataBlk++;
	ulDataSize += FAI_MES_DEFAULT_DATA_BLOCK_SIZE+(4/*FDI_SYSTEM_DATA_SDCARD_SN_SIZE*/);
	FAI_PRINT(FAI_PRT_ERROR, ("INTRN_FAI_MES_UploadSection] FDI_SYSTEM_DATA_SDCARD_SN, usNumOfDataBlk(%d) ulDataSize(%d)\n", usNumOfDataBlk, ulDataSize));
#endif

	aucHeaderBlock[6] = (usNumOfDataBlk >> 8);			// num of block
	aucHeaderBlock[7] = ((usNumOfDataBlk<<8) >> 8);
	aucHeaderBlock[8] = (0xff000000&ulDataSize)>>24;	// data size
	aucHeaderBlock[9] = (0x00ff0000&ulDataSize)>>16;
	aucHeaderBlock[10] = (0x0000ff00&ulDataSize)>>8;
	aucHeaderBlock[11] = (0x000000ff&ulDataSize);

	usCrc16 = FAI_UTIL_GetCRC16(0, &aucHeaderBlock[4], 8);
	aucHeaderBlock[2] = (usCrc16 >> 8);
	aucHeaderBlock[3] = ((usCrc16<<8) >> 8);
	FDI_MES_SendData(aucHeaderBlock, 12);

#if(MES_DOWNLOAD_UPLOAD_LIST&MES_SERIAL_NUMBER)
	eFaiErr = INTRN_FAI_MES_UploadData(FAI_MES_DATA_FLAG_SN, FDI_SYSTEM_DATA_SN_SIZE);
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_UploadSection] INTRN_FAI_MES_UploadData Error\n"));
		return FAI_ERR;
	}
#endif

#if(MES_DOWNLOAD_UPLOAD_LIST&MES_MAC_ADDRESS_1)
	eFaiErr = INTRN_FAI_MES_UploadData(FAI_MES_DATA_FLAG_MAC, FDI_SYSTEM_DATA_MAC_SIZE);
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_UploadSection] INTRN_FAI_MES_UploadData Error\n"));
		return FAI_ERR;
	}
#endif

#if(MES_DOWNLOAD_UPLOAD_LIST&MES_HDCP_KEY)
	eFaiErr = INTRN_FAI_MES_UploadData(FAI_MES_DATA_FLAG_HDCP, FDI_SYSTEM_DATA_HDCPKEY_SIZE);
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_UploadSection] INTRN_FAI_MES_UploadData Error\n"));
		return FAI_ERR;
	}
#endif

#if(MES_DOWNLOAD_UPLOAD_LIST&MES_CHIP_ID)
	eFaiErr = INTRN_FAI_MES_UploadData(FAI_MES_DATA_FLAG_CHIPID, FDI_SYSTEM_DATA_CHIPID_SIZE);
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_UploadSection] INTRN_FAI_MES_UploadData Error\n"));
		return FAI_ERR;
	}
#endif

#if(MES_DOWNLOAD_UPLOAD_LIST&MES_NAGRA_CHIP_ID)
	eFaiErr = INTRN_FAI_MES_UploadData(FAI_MES_DATA_FLAG_CHIPID, FDI_SYSTEM_DATA_CHIPID_SIZE*2);
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_UploadSection] INTRN_FAI_MES_UploadData Error\n"));
		return FAI_ERR;
	}
#endif

#if(MES_DOWNLOAD_UPLOAD_LIST&MES_DTCP_KEY)
	eFaiErr = INTRN_FAI_MES_UploadData(FAI_MES_DATA_FLAG_DTCP, FDI_SYSTEM_DATA_DTCPKEY_SIZE);
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_UploadSection] INTRN_FAI_MES_UploadData Error\n"));
		return FAI_ERR;
	}
#endif

#if(MES_DOWNLOAD_UPLOAD_LIST&MES_NAGRA_PAIRING_DATA)
	eFaiErr = INTRN_FAI_MES_UploadData(FAI_MES_DATA_FLAG_PAIRKEY, FDI_SYSTEM_DATA_PAIRKEY_SIZE);
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_UploadSection] INTRN_FAI_MES_UploadData Error\n"));
		return FAI_ERR;
	}
#endif
#if(MES_DOWNLOAD_UPLOAD_LIST&MES_DSTB_ID)
		eFaiErr = INTRN_FAI_MES_UploadData(FAI_MES_DATA_FLAG_DSTB_ID, FDI_SYSTEM_DATA_DSTBID_SIZE);
		if( eFaiErr != FAI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_UploadSection] INTRN_FAI_MES_UploadData Error\n"));
			return FAI_ERR;
		}
#endif

#if(MES_DOWNLOAD_UPLOAD_LIST&MES_RF4CE_IEEE_ADDR)
	eFaiErr = INTRN_FAI_MES_UploadData(FAI_MSS_DATA_FLAG_RF4CE_IEEE_ADDR, FDI_SYSTEM_DATA_RF4CE_IEEE_ADDR_SIZE);
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_UploadSection] INTRN_FAI_MES_UploadData Error\n"));
		return FAI_ERR;
	}
#endif
#if(MES_DOWNLOAD_UPLOAD_LIST&MES_MAC_ADDRESS_EXT)
	eFaiErr = INTRN_FAI_MES_UploadData(FAI_MES_DATA_FLAG_MACADDR_EXT, FDI_SYSTEM_DATA_MAC_SIZE);
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_UploadSection] INTRN_FAI_MES_UploadData Error\n"));
		return FAI_ERR;
	}
#endif
#if(MES_DOWNLOAD_UPLOAD_LIST&MES_HDD_SN)
	FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_UploadSection] MES_HDD_SN\n"));
	eFaiErr = INTRN_FAI_MES_UploadData(FAI_MES_DATA_FLAG_HDD_SN, FDI_SYSTEM_DATA_HDD_SN_SIZE);
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_UploadSection] INTRN_FAI_MES_UploadData Error\n"));
		return FAI_ERR;
	}
#endif

#if(MES_DOWNLOAD_UPLOAD_LIST&MES_VERIMATRIX_NSC_DATA)
	FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_UploadSection] MES_VERIMATRIX_NSC_DATA\n"));
	eFaiErr = INTRN_FAI_MES_UploadData(FAI_MES_DATA_FLAG_HDD_SN, 48/*FDI_SYSTEM_DATA_VERIMATRIX_NSC_DATA_SIZE*/);
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_UploadSection] INTRN_FAI_MES_UploadData Error\n"));
		return FAI_ERR;
	}
#endif
#if(MES_DOWNLOAD_UPLOAD_LIST&MES_SDCARD_SN)
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_UploadSection] MES_SDCARD_SN\n"));
		eFaiErr = INTRN_FAI_MES_UploadData(FAI_MSS_DATA_FLAG_SDCARD_SN, 4);
		if( eFaiErr != FAI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_UploadSection] INTRN_FAI_MES_UploadData Error\n"));
			return FAI_ERR;
		}
#endif

	FDI_MES_ReceiveData(&ucByte, 1, FAI_MES_RX_TIMEOUT);

	FVK_TASK_Sleep(100);

	FAI_MES_RECV_BYTE(FAI_MES_ETX);

	return FAI_NO_ERR;
}

/*******************************************************************************
* function : INTRN_FAI_MES_SetUploadData
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static FAI_ERR_CODE INTRN_FAI_MES_UploadData(unsigned short usDataFlag, unsigned short usDataSize)
{
	FDI_ERR_CODE eFdiErr;
	unsigned char *pucBuf;
	unsigned short usDataBlockSize, usCrc16;
	int i;
	FDI_HDD_INFO_t hddInfo;
	
	FAI_PRINT(FAI_PRT_DBGINFO, ("[INTRN_FAI_MES_UploadData] : usDataFlag=0x%04x\n", usDataFlag));
	FAI_PRINT(FAI_PRT_DBGINFO, ("[INTRN_FAI_MES_UploadData] : usDataSize=0x%04x\n", usDataSize));

	/* mem alloc */
	pucBuf = FVK_MEM_Alloc(FAI_MES_DEFAULT_DATA_BLOCK_SIZE+usDataSize);
	if( pucBuf == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_UploadData] FVK_MEM_Alloc Error\n"));
		return FAI_ERR;
	}
	
	switch(usDataFlag)
	{
		case FAI_MES_DATA_FLAG_SN:
			eFdiErr = FDI_SYSTEM_GetSerialNumber(&pucBuf[8]);
			if(eFdiErr != FDI_NO_ERR)
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_UploadData] : FDI_SYSTEM_GetSerialNumber Error!!\n"));
				goto FAIL;
			}
			break;
		case FAI_MES_DATA_FLAG_MAC:
			eFdiErr = FDI_SYSTEM_GetMacAddress(&pucBuf[8]);
			if(eFdiErr != FDI_NO_ERR)
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_UploadData] : FDI_SYSTEM_GetSerialNumber Error!!\n"));
				goto FAIL;
			}
			break;
		case FAI_MES_DATA_FLAG_HDCP:
			eFdiErr = FDI_SYSTEM_GetHdcpKey(&pucBuf[8], usDataSize);
			if(eFdiErr != FDI_NO_ERR)
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_UploadData] : FDI_SYSTEM_GetChipId Error!!\n"));
				goto FAIL;
			}
			break;
		case FAI_MES_DATA_FLAG_CHIPID:
			eFdiErr = FDI_SYSTEM_GetChipId(&pucBuf[8]);
			if(eFdiErr != FDI_NO_ERR)
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_UploadData] : FDI_SYSTEM_GetChipId Error!!\n"));
				goto FAIL;
			}

			if( usDataSize > FDI_SYSTEM_DATA_CHIPID_SIZE )
			{
				eFdiErr = FDI_SYSTEM_GetChipIdCheckNumber(&pucBuf[8+FDI_SYSTEM_DATA_CHIPID_SIZE]);
				if(eFdiErr != FDI_NO_ERR)
				{
					FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_UploadData] : FDI_SYSTEM_GetChipIdCheckNumber Error!!\n"));
					goto FAIL;
				}
			}
			
			break;
		case FAI_MES_DATA_FLAG_DTCP:
			eFdiErr = FDI_SYSTEM_GetDtcpKey(&pucBuf[8], usDataSize);
			if(eFdiErr != FDI_NO_ERR)
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_UploadData] : FDI_SYSTEM_GetChipId Error!!\n"));
				goto FAIL;
			}
			
			break;
		case FAI_MES_DATA_FLAG_PAIRKEY:
			eFdiErr = FDI_SYSTEM_GetPairKey(&pucBuf[8], usDataSize);
			if(eFdiErr != FDI_NO_ERR)
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_UploadData] : FDI_SYSTEM_GetChipId Error!!\n"));
				goto FAIL;
			}
			break;
		case FAI_MES_DATA_FLAG_NDSID:
			break;
		case FAI_MES_DATA_FLAG_SSTINFO:
			break;
		case FAI_MES_DATA_FLAG_SIM:
			break;
		case FAI_MES_DATA_FLAG_CI_DATA:
			eFdiErr = FDI_SYSTEM_GetCiPlusKeyData(&pucBuf[8], usDataSize);
			if(eFdiErr != FDI_NO_ERR)
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_UploadData] : FDI_SYSTEM_GetIsoCode Error!!\n"));
				goto FAIL;
			}
			break;
		case FAI_MES_DATA_FLAG_CI_ID:
			break;
#if defined(FACTORY_USE_WIFI)
		case FAI_MES_DATA_FLAG_MACADDR_EXT:
			eFdiErr = FDI_WIFI_GetMacAddr(0,&pucBuf[8]);
			break;
		case FAI_MES_DATA_FLAG_WIFIMAC1:
#if ( WIFI_MODULE_TYPE == WIFI_MODULE_QT3490)
			eFdiErr = FAI_WIFI_QT3940_GetMacaddr(&pucBuf[8]);
#endif
			break;
#endif			
		case FAI_MES_DATA_FLAG_DID:
			break;
		case FAI_MES_DATA_FLAG_SVP_DATA:
			break;
		case FAI_MES_DATA_FLAG_DSTB_ID:
			eFdiErr = FDI_SYSTEM_GetDstbId(&pucBuf[8], usDataSize);
			if(eFdiErr != FDI_NO_ERR)
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_UploadData] : FDI_SYSTEM_GetIsoCode Error!!\n"));
				goto FAIL;
			}
			break;
			
		case FAI_MES_DATA_FLAG_HDD_SN:
#if defined(FACTORY_USE_HDD)
			eFdiErr = FDI_HDD_GetInfo(&hddInfo);
			if(eFdiErr != FDI_NO_ERR)
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_UploadData] : FDI_HDD_GetInfo Error!!\n"));
				goto FAIL;
			}
			FVK_MEM_Copy(&pucBuf[8], (void *)&hddInfo.aucSerialNo[0], FDI_SYSTEM_DATA_HDD_SN_SIZE);
#endif			
			break;
        case FAI_MSS_DATA_FLAG_VIACCESS_BBX_DATA:
            eFdiErr = FDI_SYSTEM_GetVAExtraData(&pucBuf[8]);
            if(eFdiErr != FDI_NO_ERR)
            {
                FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_UploadData] : FDI_SYSTEM_GetIsoCode Error!!\n"));
                return FAI_ERR;
            }
            break;
        case FAI_MES_DATA_FLAG_VERIMATRIX_NSC_DATA:
#if 0 /* only using Cardless Verimatrix BetaCrypt Model */
            eFdiErr = FDI_SYSTEM_GetVerimatrixNscData(&pucBuf[8]);
            if(eFdiErr != FDI_NO_ERR)
            {
                FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_UploadData] : FDI_SYSTEM_GetVerimatrixNscData Error!!\n"));
                return FAI_ERR;
            }
#endif			
            break;
			
		case FAI_MSS_DATA_FLAG_RF4CE_IEEE_ADDR:
			eFdiErr = FDI_SYSTEM_GetIEEEAddr(&pucBuf[8]);
			if(eFdiErr != FDI_NO_ERR)
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_UploadData] : FDI_SYSTEM_GetIEEEAddr Error!!\n"));
				return FAI_ERR;
			}
			break;
#if(MES_DOWNLOAD_UPLOAD_LIST&MES_SDCARD_SN)			
		case FAI_MSS_DATA_FLAG_SDCARD_SN:
			eFdiErr = FDI_MMC_GetSerialNumber(&pucBuf[8]);
			if(eFdiErr != FDI_NO_ERR)
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_UploadData] : FDI_MMC_GetSerialNumber Error!!\n"));
				return FAI_ERR;
			}
			break;
#endif			
		default:
			break;
	}

	usDataBlockSize = FAI_MES_DEFAULT_DATA_BLOCK_SIZE-2+usDataSize;

	pucBuf[0] = (usDataBlockSize >> 8);
	pucBuf[1] = ((usDataBlockSize<<8) >> 8);
	pucBuf[4] = (usDataFlag >> 8);
	pucBuf[5] = ((usDataFlag<<8) >> 8);
	pucBuf[6] = (usDataSize >> 8);
	pucBuf[7] = ((usDataSize<<8) >> 8);
	usCrc16 = FAI_UTIL_GetCRC16(0, pucBuf+4, usDataBlockSize-2);
	pucBuf[2] = (usCrc16 >> 8);
	pucBuf[3] = ((usCrc16<<8) >> 8);

	/* send data */
	FDI_MES_SendData(pucBuf, usDataBlockSize+2);

	for( i = 0; i < usDataBlockSize+2; i++ )
	{
		FAI_PRINT(FAI_PRT_DBGINFO, ("0x%02x ", pucBuf[i]));
	}
	/* mem free */
	FVK_MEM_Free(pucBuf);
	return FAI_NO_ERR;
	
FAIL:
	FVK_MEM_Free(pucBuf);
	return FAI_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static FAI_ERR_CODE INTRN_FAI_MES_DownloadSection(void)
{
	FAI_ERR_CODE eFaiErr;
	FDI_ERR_CODE eFdiErr;
	FAI_MES_HEADER_BLOCK tHeaderBlock;
	FAI_MES_DATA_BLOCK tDataBlock;
	unsigned char aucHeaderBuf[FAI_MES_HEADER_BLOCK_SIZE];
	unsigned char *pucDataBuf, *pucBufIdx;
	unsigned char ucByte;
	unsigned long i;
			
	FAI_MES_SEND_BYTE(FAI_MES_STX);
	FAI_MES_SEND_BYTE(FAI_MES_CMD_DOWNLOAD);

	/* recevie header block */
	eFdiErr = FDI_MES_ReceiveData(aucHeaderBuf, FAI_MES_HEADER_BLOCK_SIZE, FAI_MES_RX_TIMEOUT);
	if(eFdiErr != FDI_NO_ERR)
	{
		return FAI_ERR;
	}

	/* parsing header block */
	eFaiErr = INTRN_FAI_MES_GetHeaderBlock(aucHeaderBuf, &tHeaderBlock);
	if(eFaiErr != FAI_NO_ERR)
	{
		FAI_MES_SEND_BYTE(FAI_MES_ERR_DOWN_CRC);
		return eFaiErr;
	}

	/* mem alloc for data blocks */
	pucDataBuf = FVK_MEM_Alloc(tHeaderBlock.ulDataTotalSize);
	if( pucDataBuf == NULL )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_DownloadSection] : FVK_MEM_Alloc Error\n"));
		return FAI_ERR;
	}

	/* receive data block */
	eFaiErr = INTRN_FAI_MES_RecvBuffer(pucDataBuf, tHeaderBlock.ulDataTotalSize);
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_DownloadSection] : INTRN_FAI_MES_RecvBuffer Error\n"));
		FAI_MES_SEND_BYTE(FAI_MES_ERR_DOWN_LEN);
		goto FAIL;
	}

	pucBufIdx = pucDataBuf;
	while(tHeaderBlock.usDataBlockCnt--)
	{
		eFaiErr = INTRN_FAI_MES_GetDataBlock(pucBufIdx, &tDataBlock);
		if(eFaiErr != FAI_NO_ERR)
		{
			FAI_MES_SEND_BYTE(FAI_MES_ERR_DOWN_CRC);
			goto FAIL;
		}

		switch(tDataBlock.usDataFlag)
		{
			case FAI_MES_DATA_FLAG_SN:
				eFaiErr = INTRN_FAI_MES_SetSerialNumber(tDataBlock.pucDataBuf, tDataBlock.usDataOrgSize);
				if( eFaiErr != FAI_NO_ERR )
				{
					FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_DownloadSection] : INTRN_FAI_MES_RecvBuffer Error\n"));
					FAI_MES_SEND_BYTE(FAI_MES_ERR_DOWN_VERIFY);
					goto FAIL;
				}
				break;
			case FAI_MES_DATA_FLAG_MAC:
				//printf("%c",tDataBlock.pucDataBuf);
				eFaiErr = INTRN_FAI_MES_SetMacAddress(tDataBlock.pucDataBuf, tDataBlock.usDataOrgSize);
				if( eFaiErr != FAI_NO_ERR )
				{
					FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_DownloadSection] : INTRN_FAI_MES_RecvBuffer Error\n"));
					FAI_MES_SEND_BYTE(FAI_MES_ERR_DOWN_VERIFY);
					goto FAIL;
				}
				break;
			case FAI_MES_DATA_FLAG_HDCP:
				eFaiErr = INTRN_FAI_MES_SetHdcpKey(tDataBlock.pucDataBuf, tDataBlock.usDataOrgSize);
				if( eFaiErr != FAI_NO_ERR )
				{
					FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_DownloadSection] : INTRN_FAI_MES_RecvBuffer Error\n"));
					FAI_MES_SEND_BYTE(FAI_MES_ERR_DOWN_VERIFY);
					goto FAIL;
				}
				break;
			case FAI_MES_DATA_FLAG_CHIPID:
				break;
			case FAI_MES_DATA_FLAG_DTCP:
				eFaiErr = INTRN_FAI_MES_SetDtcpKey(tDataBlock.pucDataBuf, tDataBlock.usDataOrgSize);
				if( eFaiErr != FAI_NO_ERR )
				{
					FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_DownloadSection] : INTRN_FAI_MES_SetDtcpkey Error\n"));
					FAI_MES_SEND_BYTE(FAI_MES_ERR_DOWN_VERIFY);
					goto FAIL;
				}
				break;
			case FAI_MES_DATA_FLAG_PAIRKEY:
				eFaiErr = INTRN_FAI_MES_SetPairKey(tDataBlock.pucDataBuf, tDataBlock.usDataOrgSize);
				if( eFaiErr != FAI_NO_ERR )
				{
					FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_DownloadSection] : INTRN_FAI_MES_SetPairKey Error\n"));
					FAI_MES_SEND_BYTE(FAI_MES_ERR_DOWN_VERIFY);
					goto FAIL;
				}
				break;
			case FAI_MES_DATA_FLAG_NDSID:
				break;
			case FAI_MES_DATA_FLAG_SSTINFO:
				break;
			case FAI_MES_DATA_FLAG_SIM:
				break;
			case FAI_MES_DATA_FLAG_CI_DATA:
				eFaiErr = INTRN_FAI_MES_SetCiPlusKeyData(tDataBlock.pucDataBuf, tDataBlock.usDataOrgSize);
				if( eFaiErr != FAI_NO_ERR )
				{
					FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_DownloadSection] : INTRN_FAI_MES_SetCiPlusKeyData Error\n"));
					FAI_MES_SEND_BYTE(FAI_MES_ERR_DOWN_VERIFY);
					goto FAIL;
				}
				break;
			case FAI_MES_DATA_FLAG_CI_ID:
				break;
#if defined(FACTORY_USE_BT)
			case FAI_MES_DATA_FLAG_MACADDR_2:
				eFdiErr = INTRN_FAI_MES_SetBTMacAddress(tDataBlock.pucDataBuf, tDataBlock.usDataOrgSize);
				if(eFdiErr != FDI_NO_ERR)
				{
					FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_DownloadSection] : INTRN_FAI_MES_SetBTMacAddress Error!!\n"));
					goto FAIL;
				}
				break;
#endif
#if defined(CONFIG_DI10) /* only use PRODUCT_KHD1000T */
			case FAI_MES_DATA_FLAG_MACADDR_2:	
				eFaiErr = FAI_SYSTEM_SetWIFIMacAddr(tDataBlock.pucDataBuf);
				if( eFaiErr != FAI_NO_ERR )
				{
					FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_DownloadSection] : INTRN_FAI_MES_RecvBuffer Error\n"));
					FAI_MES_SEND_BYTE(FAI_MES_ERR_DOWN_VERIFY);
					goto FAIL;
				}
				break;
#endif
#if defined(CONFIG_DI20)
			case FAI_MES_DATA_FLAG_MACADDR_3:
				eFaiErr = INTRN_FAI_MES_SetThirdMacAddress(tDataBlock.pucDataBuf, tDataBlock.usDataOrgSize);
				if( eFaiErr != FAI_NO_ERR )
				{
					FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_DownloadSection] : INTRN_FAI_MES_RecvBuffer Error\n"));
					FAI_MES_SEND_BYTE(FAI_MES_ERR_DOWN_VERIFY);
					goto FAIL;
				}
				break;
#endif				
#if defined(FACTORY_USE_CABLEMODEM)				
			case FAI_MES_DATA_FLAG_MACADDR_4:
				eFaiErr = INTRN_FAI_MES_SetCmMacAddress(tDataBlock.pucDataBuf, tDataBlock.usDataOrgSize);
				if( eFaiErr != FAI_NO_ERR )
				{
					FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_DownloadSection] : INTRN_FAI_MES_RecvBuffer Error\n"));
					FAI_MES_SEND_BYTE(FAI_MES_ERR_DOWN_VERIFY);
					goto FAIL;
				}
				break;
#endif
#if defined(FACTORY_USE_WIFI)
#if (PRODUCT_YSR2000==YES)
			case FAI_MES_DATA_FLAG_MACADDR_2:
#endif				
			case FAI_MES_DATA_FLAG_WIFIMAC1 : 
#if ( WIFI_MODULE_TYPE == WIFI_MODULE_QT3490)				
				eFdiErr = FAI_WIFI_QT3940_SetMacaddr(tDataBlock.pucDataBuf);/* write to wifi ic */
//				eFdiErr |= FAI_SYSTEM_SetWIFIMacAddr(tDataBlock.pucDataBuf);/* write to nvram */
				if(eFdiErr != FDI_NO_ERR)
				{
					FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_DownloadSection] : FAI_MES_DATA_FLAG_WIFIMAC1 Error!!\n"));
					goto FAIL;
				}
#endif
				break;
#endif

			case FAI_MES_DATA_FLAG_DID:
				break;
			case FAI_MES_DATA_FLAG_SVP_DATA:
				break;
			case FAI_MES_DATA_FLAG_DSTB_ID:
				eFaiErr = INTRN_FAI_MES_SetDstbId(tDataBlock.pucDataBuf, tDataBlock.usDataOrgSize);
				if( eFaiErr != FAI_NO_ERR )
				{
					FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_DownloadSection] : INTRN_FAI_MES_SetDstbId Error\n"));
					FAI_MES_SEND_BYTE(FAI_MES_ERR_DOWN_VERIFY);
					goto FAIL;
				}
				break;
            case FAI_MES_DATA_FLAG_CSC_KEY:
				eFaiErr = INTRN_FAI_MES_SetCSCKey(tDataBlock.pucDataBuf, tDataBlock.usDataOrgSize);
				if( eFaiErr != FAI_NO_ERR )
				{
					FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_DownloadSection] : INTRN_FAI_MES_SetCiPlusKeyData Error\n"));
					FAI_MES_SEND_BYTE(FAI_MES_ERR_DOWN_VERIFY);
					goto FAIL;
				}
				break;
           case FAI_MSS_DATA_FLAG_VIACCESS_BBX_DATA:
                eFaiErr = INTRN_FAI_MES_SetVAExtraData(tDataBlock.pucDataBuf, tDataBlock.usDataOrgSize);
                if( eFaiErr != FAI_NO_ERR )
                {
                    FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_DownloadSection] : INTRN_FAI_MES_SetVAExtraData Error\n"));
                    FAI_MES_SEND_BYTE(FAI_MES_ERR_DOWN_VERIFY);
                    return FAI_ERR;
                }
	                break;
			case FAI_MSS_DATA_FLAG_RF4CE_IEEE_ADDR:
				eFaiErr = INTRN_FAI_MES_SetIEEEAddr(tDataBlock.pucDataBuf, tDataBlock.usDataOrgSize);
				if( eFaiErr != FAI_NO_ERR )
				{
					FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_DownloadSection] : INTRN_FAI_MES_SetVAExtraData Error\n"));
					FAI_MES_SEND_BYTE(FAI_MES_ERR_DOWN_VERIFY);
					return FAI_ERR;
				}
				break;
			case FAI_MSS_DATA_FLAG_IRDETO_PRIVATEDATA:

				eFaiErr = INTRN_FAI_MES_SetPrivateData(tDataBlock.pucDataBuf, tDataBlock.usDataOrgSize);
				if( eFaiErr != FAI_NO_ERR )
				{
					FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_DownloadSection] : INTRN_FAI_MES_PrivateData Error\n"));
					FAI_MES_SEND_BYTE(FAI_MES_ERR_DOWN_VERIFY);
					return FAI_ERR;
				}
				break;
			case FAI_MES_DATA_FLAG_VERIMATRIX_CAS_DATA:
				eFaiErr = INTRN_FAI_MES_SetVerimatrixCasData(tDataBlock.pucDataBuf, tDataBlock.usDataOrgSize);
				if( eFaiErr != FAI_NO_ERR )
				{
					FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_DownloadSection] : INTRN_FAI_MES_SetVerimatrixCasData Error\n"));
					FAI_MES_SEND_BYTE(FAI_MES_ERR_DOWN_VERIFY);
					return FAI_ERR;
				}
				break;				
			default:
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_DownloadSection] invalid data flag:%04x\n", tDataBlock.usDataFlag));
				FAI_MES_SEND_BYTE(FAI_MES_ERR_DOWN_COMPARE);
				goto FAIL;
		}
		pucBufIdx += tDataBlock.usDataSize+2;
	}

	FVK_MEM_Free(pucDataBuf);

	FAI_MES_SEND_BYTE(FAI_MES_ETX);

	return FAI_NO_ERR;

FAIL:
	FVK_MEM_Free(pucDataBuf);
	return FAI_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static FAI_ERR_CODE INTRN_FAI_MES_CheckDownloadDataSection(void)
{
	FAI_ERR_CODE  eFaiErr;
	unsigned char ucByte;
	unsigned char aucHeaderBlock[12];
	unsigned long ulDataSize = 0;
	unsigned short usCrc16, usNumOfDataBlk = 0;
	int i;
	
	FAI_MES_RECV_BYTE(FAI_MES_STX);
#if 0 
	FAI_MES_RECV_BYTE(0x56);
#else	/* 0x56 is OTP Command, 0x58 is right value for download check */
	FAI_MES_RECV_BYTE(FAI_MES_CMD_CHECK_DATA);
#endif

	/* send header block */
	aucHeaderBlock[0] = 0x00;	// header block size
	aucHeaderBlock[1] = FAI_MES_HEADER_BLOCK_SIZE;
	aucHeaderBlock[4] = 0x00;	// version
	aucHeaderBlock[5] = FAI_MES_FORMAT_VERSION;
#if(MES_DOWNLOAD_CHECK_LIST&MES_SERIAL_NUMBER)
	usNumOfDataBlk++;
	ulDataSize += FAI_MES_DEFAULT_DATA_BLOCK_SIZE+FDI_SYSTEM_DATA_SN_SIZE;
#endif
#if(MES_DOWNLOAD_CHECK_LIST&MES_MAC_ADDRESS_1)
	usNumOfDataBlk++;
	ulDataSize += FAI_MES_DEFAULT_DATA_BLOCK_SIZE+FDI_SYSTEM_DATA_MAC_SIZE;
#endif
#if(MES_DOWNLOAD_CHECK_LIST&MES_HDCP_KEY)
	usNumOfDataBlk++;
	ulDataSize += FAI_MES_DEFAULT_DATA_BLOCK_SIZE+FDI_SYSTEM_DATA_HDCPKEY_SIZE;
#endif
#if(MES_DOWNLOAD_CHECK_LIST&MES_CHIP_ID)
	usNumOfDataBlk++;
	ulDataSize += FAI_MES_DEFAULT_DATA_BLOCK_SIZE+FDI_SYSTEM_DATA_CHIPID_SIZE;
#endif
#if(MES_DOWNLOAD_CHECK_LIST&MES_DTCP_KEY)
	usNumOfDataBlk++;
	ulDataSize += FAI_MES_DEFAULT_DATA_BLOCK_SIZE+FDI_SYSTEM_DATA_DTCPKEY_SIZE;
#endif
#if(MES_DOWNLOAD_CHECK_LIST&MES_NAGRA_PAIRING_DATA)
	usNumOfDataBlk++;
	ulDataSize += FAI_MES_DEFAULT_DATA_BLOCK_SIZE+FDI_SYSTEM_DATA_PAIRKEY_SIZE;
#endif
#if(MES_DOWNLOAD_CHECK_LIST&MES_DSTB_ID)
			usNumOfDataBlk++;
			ulDataSize += FAI_MES_DEFAULT_DATA_BLOCK_SIZE+FDI_SYSTEM_DATA_DSTBID_SIZE;
#endif
#if(MES_DOWNLOAD_CHECK_LIST&MES_VIA_BBX_DATA)
        usNumOfDataBlk++;
        ulDataSize += FAI_MES_DEFAULT_DATA_BLOCK_SIZE+FDI_SYSTEM_DATA_VAEXTRADATA_SIZE;
#endif
#if 0
#if(MES_DOWNLOAD_CHECK_LIST&MES_VERIMATRIX_CAS_DATA)
        usNumOfDataBlk++;
        ulDataSize += FAI_MES_DEFAULT_DATA_BLOCK_SIZE+32000/*FDI_SYSTEM_DATA_VERIMATRIX_CAS_DATA_SIZE*/;
#endif
#endif
#if(MES_DOWNLOAD_UPLOAD_LIST&MES_RF4CE_IEEE_ADDR)
	usNumOfDataBlk++;
	ulDataSize += FAI_MES_DEFAULT_DATA_BLOCK_SIZE+(FDI_SYSTEM_DATA_RF4CE_IEEE_ADDR_SIZE);
#endif
	aucHeaderBlock[6] = (usNumOfDataBlk >> 8);			// num of block
	aucHeaderBlock[7] = ((usNumOfDataBlk<<8) >> 8);
	aucHeaderBlock[8] = (0xff000000&ulDataSize)>>24;	// data size
	aucHeaderBlock[9] = (0x00ff0000&ulDataSize)>>16;
	aucHeaderBlock[10] = (0x0000ff00&ulDataSize)>>8;
	aucHeaderBlock[11] = (0x000000ff&ulDataSize);

	usCrc16 = FAI_UTIL_GetCRC16(0, &aucHeaderBlock[4], 8);
	aucHeaderBlock[2] = (usCrc16 >> 8);
	aucHeaderBlock[3] = ((usCrc16<<8) >> 8);
	FDI_MES_SendData(aucHeaderBlock, 12);

#if(MES_DOWNLOAD_CHECK_LIST&MES_SERIAL_NUMBER)
	eFaiErr = INTRN_FAI_MES_UploadData(FAI_MES_DATA_FLAG_SN, FDI_SYSTEM_DATA_SN_SIZE);
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_UploadSection] INTRN_FAI_MES_UploadData Error\n"));
		return FAI_ERR;
	}
#endif

#if(MES_DOWNLOAD_CHECK_LIST&MES_MAC_ADDRESS_1)
	eFaiErr = INTRN_FAI_MES_UploadData(FAI_MES_DATA_FLAG_MAC, FDI_SYSTEM_DATA_MAC_SIZE);
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_UploadSection] INTRN_FAI_MES_UploadData Error\n"));
		return FAI_ERR;
	}
#endif

#if(MES_DOWNLOAD_CHECK_LIST&MES_HDCP_KEY)
	eFaiErr = INTRN_FAI_MES_UploadData(FAI_MES_DATA_FLAG_HDCP, FDI_SYSTEM_DATA_HDCPKEY_SIZE);
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_UploadSection] INTRN_FAI_MES_UploadData Error\n"));
		return FAI_ERR;
	}
#endif

#if(MES_DOWNLOAD_CHECK_LIST&MES_CHIP_ID)
	eFaiErr = INTRN_FAI_MES_UploadData(FAI_MES_DATA_FLAG_CHIPID, FDI_SYSTEM_DATA_CHIPID_SIZE);
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_UploadSection] INTRN_FAI_MES_UploadData Error\n"));
		return FAI_ERR;
	}
#endif

#if(MES_DOWNLOAD_CHECK_LIST&MES_DTCP_KEY)
	eFaiErr = INTRN_FAI_MES_UploadData(FAI_MES_DATA_FLAG_DTCP, FDI_SYSTEM_DATA_DTCPKEY_SIZE);
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_UploadSection] INTRN_FAI_MES_UploadData Error\n"));
		return FAI_ERR;
	}
#endif

#if(MES_DOWNLOAD_CHECK_LIST&MES_NAGRA_PAIRING_DATA)
	eFaiErr = INTRN_FAI_MES_UploadData(FAI_MES_DATA_FLAG_PAIRKEY, FDI_SYSTEM_DATA_PAIRKEY_SIZE);
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_UploadSection] INTRN_FAI_MES_UploadData Error\n"));
		return FAI_ERR;
	}
#endif

#if(MES_DOWNLOAD_CHECK_LIST&MES_CI_PLUS_KEY_DATA)
		eFaiErr = INTRN_FAI_MES_UploadData(FAI_MES_DATA_FLAG_CI_DATA, FDI_SYSTEM_DATA_CIPLUSKEY_SIZE);
		if( eFaiErr != FAI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_UploadSection] INTRN_FAI_MES_UploadData Error\n"));
			return FAI_ERR;
		}
#endif

#if(MES_DOWNLOAD_CHECK_LIST&MES_DSTB_ID)
			eFaiErr = INTRN_FAI_MES_UploadData(FAI_MES_DATA_FLAG_DSTB_ID, FDI_SYSTEM_DATA_DSTBID_SIZE);
			if( eFaiErr != FAI_NO_ERR )
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_UploadSection] INTRN_FAI_MES_UploadData Error\n"));
				return FAI_ERR;
			}
#endif
#if(MES_DOWNLOAD_CHECK_LIST&MES_VIA_BBX_DATA)
        eFaiErr = INTRN_FAI_MES_UploadData(FAI_MSS_DATA_FLAG_VIACCESS_BBX_DATA, FDI_VAEXTRADATA_SIZE);
        if( eFaiErr != FAI_NO_ERR )
        {
            FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_UploadSection] INTRN_FAI_MES_UploadData Error\n"));
            return FAI_ERR;
        }
#endif
#if(MES_DOWNLOAD_UPLOAD_LIST&MES_RF4CE_IEEE_ADDR)
	eFaiErr = INTRN_FAI_MES_UploadData(FAI_MSS_DATA_FLAG_RF4CE_IEEE_ADDR, FDI_SYSTEM_DATA_RF4CE_IEEE_ADDR_SIZE);
	if( eFaiErr != FAI_NO_ERR )
	{
	    FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_UploadSection] INTRN_FAI_MES_UploadData Error\n"));
	    return FAI_ERR;
	}
#endif

	FDI_MES_ReceiveData(&ucByte, 1, FAI_MES_RX_TIMEOUT);

	FVK_TASK_Sleep(100);

	FAI_MES_RECV_BYTE(FAI_MES_ETX);

	return FAI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static FAI_ERR_CODE INTRN_FAI_MES_OTPSection(void)
{
	FDI_ERR_CODE  eFdiErr;
	unsigned char ucByte;

	FAI_MES_RECV_BYTE(FAI_MES_STX);
	FAI_MES_RECV_BYTE(FAI_MES_CMD_OPT);

	eFdiErr = FDI_MES_ReceiveData(&ucByte, sizeof(ucByte), FAI_MES_RX_TIMEOUT);
	if(eFdiErr != FDI_NO_ERR)
	{
		return FAI_ERR;
	}
	
	if(ucByte != FAI_MES_DATA_OPT_CHECK)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_OTPSection] ucByte(0x%02x) is not 0x%02x\n\n", ucByte, FAI_MES_DATA_OPT_CHECK));
		return FAI_ERR;
	}

	FAI_MES_SEND_BYTE(FAI_MES_DATA_OPT_CHECK);

	return FAI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static FAI_ERR_CODE INTRN_FAI_MES_End(void)
{
	FAI_MES_SEND_BYTE(FAI_MES_STX);
	FAI_MES_SEND_BYTE(FAI_MES_CMD_END);
	FAI_MES_SEND_BYTE(FAI_MES_ETX);

	return FAI_NO_ERR;
}

/*******************************************************************************
* function : INTRN_FAI_MES_SendByte
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static FAI_ERR_CODE INTRN_FAI_MES_SendByte(unsigned char ucByte)
{
	FDI_ERR_CODE  eFdiErr;
	unsigned char ucRevByte;

	eFdiErr = FDI_MES_SendData(&ucByte, sizeof(ucByte));
	if(eFdiErr != FDI_NO_ERR)
	{
		return FAI_ERR;
	}
	
	eFdiErr = FDI_MES_ReceiveData(&ucRevByte, sizeof(ucRevByte), FAI_MES_RX_TIMEOUT);
	if(eFdiErr != FDI_NO_ERR)
	{
		return FAI_ERR;
	}

	if(ucRevByte != FAI_MES_ACK)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SendByte] Invalid Ack : 0x%02x\n", ucRevByte));
		return FAI_ERR;
	}

	return FAI_NO_ERR;
}

/*******************************************************************************
* function : INTRN_FAI_MES_RecvByte
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static FAI_ERR_CODE INTRN_FAI_MES_RecvByte(unsigned char ucRevByte)
{
	FDI_ERR_CODE  eFdiErr;
	unsigned char ucByte;

	eFdiErr = FDI_MES_ReceiveData(&ucByte, sizeof(ucByte), FAI_MES_RX_TIMEOUT);
	if(eFdiErr != FDI_NO_ERR)
	{
		return FAI_ERR;
	}

	if(ucByte != ucRevByte)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_RecvByte] ucRevByte(0x%02x) is not expected byte (0x%02x).\n", ucByte, ucRevByte));
		return FAI_ERR;
	}

	ucByte = FAI_MES_ACK;
	eFdiErr = FDI_MES_SendData(&ucByte, sizeof(ucByte));
	if(eFdiErr != FDI_NO_ERR)
	{
		return FAI_ERR;
	}

	return FAI_NO_ERR;
}


/*******************************************************************************
* function : INTRN_FAI_MES_SendBuffer
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static FAI_ERR_CODE INTRN_FAI_MES_SendBuffer(unsigned char *pucBuf, unsigned long ulBufSize)
{
	FDI_ERR_CODE  eFdiErr;
	unsigned char ucRevByte;

	eFdiErr = FDI_MES_SendData(pucBuf, ulBufSize);
	if(eFdiErr != FDI_NO_ERR)
	{
		return FAI_ERR;
	}

	eFdiErr = FDI_MES_ReceiveData(&ucRevByte, sizeof(ucRevByte), FAI_MES_RX_TIMEOUT);
	if(eFdiErr != FDI_NO_ERR)
	{
		return FAI_ERR;
	}

	if(ucRevByte != FAI_MES_ACK)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SendBuffer] Invalid Ack : 0x%02x\n", ucRevByte));
		return FAI_ERR;
	}

	return FAI_NO_ERR;
}


/*******************************************************************************
* function : INTRN_FAI_MES_RecvBuffer
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static FAI_ERR_CODE INTRN_FAI_MES_RecvBuffer(unsigned char *pucBuf, unsigned long ulBufSize)
{
	FDI_ERR_CODE  eFdiErr;
	unsigned char ucByte;

	eFdiErr = FDI_MES_ReceiveData(pucBuf, ulBufSize, FAI_MES_RX_TIMEOUT);
	if(eFdiErr != FDI_NO_ERR)
	{
		return FAI_ERR;
	}

	ucByte = FAI_MES_ACK;
	eFdiErr = FDI_MES_SendData(&ucByte, sizeof(ucByte));
	if(eFdiErr != FDI_NO_ERR)
	{
		return FAI_ERR;
	}
	
	return FAI_NO_ERR;
}

/*******************************************************************************
* function : INTRN_FAI_MES_GetHeaderBlock
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static FAI_ERR_CODE INTRN_FAI_MES_GetHeaderBlock(unsigned char *pucBuf, FAI_MES_HEADER_BLOCK *ptMesHeaderBlock)
{
	/* parsing */
	ptMesHeaderBlock->usHeaderSize = FAI_UTIL_GetBits(pucBuf, 0, 0, 16);
	ptMesHeaderBlock->usHeaderCRC16 = FAI_UTIL_GetBits(pucBuf, 2, 0, 16);
	ptMesHeaderBlock->usFormatVer = FAI_UTIL_GetBits(pucBuf, 4, 0, 16);
	ptMesHeaderBlock->usDataBlockCnt = FAI_UTIL_GetBits(pucBuf, 6, 0, 16);
	ptMesHeaderBlock->ulDataTotalSize =FAI_UTIL_GetBits(pucBuf, 8, 0, 32);

	FAI_PRINT(FAI_PRT_DBGINFO, ("[INTRN_FAI_MES_GetHeaderBlock] usHeaderSize : 0x%04x\n", ptMesHeaderBlock->usHeaderSize));
	FAI_PRINT(FAI_PRT_DBGINFO, ("[INTRN_FAI_MES_GetHeaderBlock] usHeaderCRC16 : 0x%04x\n", ptMesHeaderBlock->usHeaderCRC16));
	FAI_PRINT(FAI_PRT_DBGINFO, ("[INTRN_FAI_MES_GetHeaderBlock] usFormatVer : 0x%04x\n", ptMesHeaderBlock->usFormatVer));
	FAI_PRINT(FAI_PRT_DBGINFO, ("[INTRN_FAI_MES_GetHeaderBlock] usDataBlockCnt : 0x%04x\n", ptMesHeaderBlock->usDataBlockCnt));
	FAI_PRINT(FAI_PRT_DBGINFO, ("[INTRN_FAI_MES_GetHeaderBlock]ulDataTotalSize : 0x%04x\n", ptMesHeaderBlock->ulDataTotalSize));

	/* check data size */
	if(ptMesHeaderBlock->usHeaderSize != FAI_MES_HEADER_BLOCK_SIZE)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_GetHeaderBlock] Header size Error : 0x%04x\n", ptMesHeaderBlock->usHeaderSize));
		return FAI_ERR;
	}

	/* check version */
	if(ptMesHeaderBlock->usFormatVer != FAI_MES_FORMAT_VERSION)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_GetHeaderBlock] Header version Error : 0x%04x\n", ptMesHeaderBlock->usFormatVer));
		return FAI_ERR;
	}

	/* check crc */
	if( FAI_UTIL_GetCRC16(0, pucBuf+4, ptMesHeaderBlock->usHeaderSize-4) != ptMesHeaderBlock->usHeaderCRC16 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_GetHeaderBlock] : Header Block CRC Error!!\n"));
		return FAI_ERR;
	}

	return FAI_NO_ERR;
}

/*******************************************************************************
* function : INTRN_FAI_MES_RecvDataBlock
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static FAI_ERR_CODE INTRN_FAI_MES_GetDataBlock(unsigned char *pucBuf, FAI_MES_DATA_BLOCK *ptMesDataBlock)
{
	unsigned short usCrc16;
	
	/* parsing data block's header */
	ptMesDataBlock->usDataSize = FAI_UTIL_GetBits(pucBuf, 0, 0, 16);
	ptMesDataBlock->usDataCRC16 = FAI_UTIL_GetBits(pucBuf, 2, 0, 16);
	ptMesDataBlock->usDataFlag = FAI_UTIL_GetBits(pucBuf, 4, 0, 16);
	ptMesDataBlock->usDataOrgSize = FAI_UTIL_GetBits(pucBuf, 6, 0, 16);
	ptMesDataBlock->pucDataBuf = pucBuf+8;

	FAI_PRINT(FAI_PRT_DBGINFO, ("[INTRN_FAI_MES_GetDataBlock] usDataSize : 0x%04x\n", ptMesDataBlock->usDataSize));
	FAI_PRINT(FAI_PRT_DBGINFO, ("[INTRN_FAI_MES_GetDataBlock] usDataCRC16 : 0x%04x\n", ptMesDataBlock->usDataCRC16));
	FAI_PRINT(FAI_PRT_DBGINFO, ("[INTRN_FAI_MES_GetDataBlock] usDataFlag : 0x%04x\n", ptMesDataBlock->usDataFlag));
	FAI_PRINT(FAI_PRT_DBGINFO, ("[INTRN_FAI_MES_GetDataBlock] usDataOrgSize : 0x%04x\n", ptMesDataBlock->usDataOrgSize));

	/* Header block CRC check */
	usCrc16 = FAI_UTIL_GetCRC16(0, pucBuf+4, ptMesDataBlock->usDataSize-2);
	FAI_PRINT(FAI_PRT_DBGINFO, ("[INTRN_FAI_MES_GetDataBlock] Calc CRC : 0x%04x\n", usCrc16));
	if( usCrc16 != ptMesDataBlock->usDataCRC16 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_GetDataBlock] : Data Block CRC Error!!\n"));
		return FAI_ERR;
	}

	return FAI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static FAI_ERR_CODE INTRN_FAI_MES_SetSerialNumber(unsigned char *pucBuf, unsigned short usSize)
{
	FDI_ERR_CODE  eFdiErr;
	unsigned char *pucVerifyBuf;
	int i;
	
	if( usSize != FDI_SYSTEM_DATA_SN_SIZE)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetSerialNumber] Invaild Serialnumber Size : 0x%08x\n", usSize));
		return FAI_ERR;
	}

	eFdiErr = FDI_SYSTEM_SetSerialNumber(pucBuf);
	if( eFdiErr != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetSerialNumber] FDI_SYSTEM_SetSerialNumber Error : 0x%08x\n", eFdiErr));
		return FAI_ERR;
	}

	pucVerifyBuf = (unsigned char *)FVK_MEM_Alloc(FDI_SYSTEM_DATA_SN_SIZE);
	if( pucVerifyBuf == NULL)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetSerialNumber] FVK_MEM_Alloc Error.\n"));
		return FAI_ERR;
	}

	/* Verify */
	eFdiErr = FDI_SYSTEM_GetSerialNumber(pucVerifyBuf);
	if( eFdiErr != FDI_NO_ERR )
	{
		FVK_MEM_Free(pucVerifyBuf);
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetSerialNumber] FDI_SYSTEM_GetSerialNumber Error : 0x%08x\n", eFdiErr));
		return FAI_ERR;
	}

	eFdiErr = FVK_MEM_Compare((void*)pucBuf, (void*)pucVerifyBuf, FDI_SYSTEM_DATA_SN_SIZE);
	if( eFdiErr != FDI_NO_ERR )
	{
		FVK_MEM_Free(pucVerifyBuf);
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetSerialNumber] SerialNumber verification fail.\n"));
		return FAI_ERR;
	}

	FVK_MEM_Free(pucVerifyBuf);

	return FAI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static FAI_ERR_CODE INTRN_FAI_MES_SetMacAddress(unsigned char *pucBuf, unsigned short usSize)
{
	FDI_ERR_CODE  eFdiErr;
	unsigned char *pucVerifyBuf;
	int i;
	
	if( usSize != FDI_SYSTEM_DATA_MAC_SIZE)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetMacAddress] Invaild MacAddress Size : 0x%08x\n", usSize));
		return FAI_ERR;
	}

	eFdiErr = FDI_SYSTEM_SetMacAddress(pucBuf);
	if( eFdiErr != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetMacAddress] FDI_SYSTEM_SetMacAddress Error : 0x%08x\n", eFdiErr));
		return FAI_ERR;
	}

	pucVerifyBuf = (unsigned char *)FVK_MEM_Alloc(FDI_SYSTEM_DATA_MAC_SIZE);
	if( pucVerifyBuf == NULL)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetMacAddress] FVK_MEM_Alloc Error.\n"));
		return FAI_ERR;
	}

	/* Verify */
	eFdiErr = FDI_SYSTEM_GetMacAddress(pucVerifyBuf);
	if( eFdiErr != FDI_NO_ERR )
	{
		FVK_MEM_Free(pucVerifyBuf);
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetMacAddress] FDI_SYSTEM_GetMacAddress Error : 0x%08x\n", eFdiErr));
		return FAI_ERR;
	}

	eFdiErr = FVK_MEM_Compare((void*)pucBuf, (void*)pucVerifyBuf, FDI_SYSTEM_DATA_MAC_SIZE);
	if( eFdiErr != FDI_NO_ERR )
	{
		FVK_MEM_Free(pucVerifyBuf);
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetMacAddress] MacAddress verification fail.\n"));
		return FAI_ERR;
	}

	FVK_MEM_Free(pucVerifyBuf);

	return FAI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static FAI_ERR_CODE INTRN_FAI_MES_SetSecondMacAddress(unsigned char *pucBuf, unsigned short usSize)
{
	FDI_ERR_CODE  eFdiErr;
	unsigned char *pucVerifyBuf;
	int i;
	
	if( usSize != FDI_SYSTEM_DATA_MAC_SIZE)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetMacAddress] Invaild MacAddress Size : 0x%08x\n", usSize));
		return FAI_ERR;
	}

#if 1//defined(CONFIG_PROD_KHD1000T)
	eFdiErr = FDI_SYSTEM_SetWifiMacAddress(pucBuf);
#else
	eFdiErr = FDI_SYSTEM_SetSecondMacAddress(pucBuf);
#endif
	if( eFdiErr != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetMacAddress] FDI_SYSTEM_SetMacAddress Error : 0x%08x\n", eFdiErr));
		return FAI_ERR;
	}

	pucVerifyBuf = (unsigned char *)FVK_MEM_Alloc(FDI_SYSTEM_DATA_MAC_SIZE);
	if( pucVerifyBuf == NULL)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetMacAddress] FVK_MEM_Alloc Error.\n"));
		return FAI_ERR;
	}

	/* Verify */

#if 1//defined(CONFIG_PROD_KHD1000T)
	eFdiErr = FDI_SYSTEM_GetWifiMacAddress(pucVerifyBuf);
#else
	eFdiErr = FDI_SYSTEM_GetSecondMacAddress(pucVerifyBuf);
#endif
	if( eFdiErr != FDI_NO_ERR )
	{
		FVK_MEM_Free(pucVerifyBuf);
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetMacAddress] FDI_SYSTEM_GetMacAddress Error : 0x%08x\n", eFdiErr));
		return FAI_ERR;
	}

	eFdiErr = FVK_MEM_Compare((void*)pucBuf, (void*)pucVerifyBuf, FDI_SYSTEM_DATA_MAC_SIZE);
	if( eFdiErr != FDI_NO_ERR )
	{
		FVK_MEM_Free(pucVerifyBuf);
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetMacAddress] MacAddress verification fail.\n"));
		return FAI_ERR;
	}

	FVK_MEM_Free(pucVerifyBuf);

	return FAI_NO_ERR;
}

#if defined(CONFIG_DI20)
/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static FAI_ERR_CODE INTRN_FAI_MES_SetThirdMacAddress(unsigned char *pucBuf, unsigned short usSize)
{
	FDI_ERR_CODE  eFdiErr;
	unsigned char *pucVerifyBuf;
	int i;
	
	if( usSize != FDI_SYSTEM_DATA_MAC_SIZE)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetMacAddress] Invaild MacAddress Size : 0x%08x\n", usSize));
		return FAI_ERR;
	}

	eFdiErr = FDI_SYSTEM_SetThirdMacAddress(pucBuf);
	if( eFdiErr != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetMacAddress] FDI_SYSTEM_SetMacAddress Error : 0x%08x\n", eFdiErr));
		return FAI_ERR;
	}

	pucVerifyBuf = (unsigned char *)FVK_MEM_Alloc(FDI_SYSTEM_DATA_MAC_SIZE);
	if( pucVerifyBuf == NULL)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetMacAddress] FVK_MEM_Alloc Error.\n"));
		return FAI_ERR;
	}

	/* Verify */
	eFdiErr = FDI_SYSTEM_GetThirdMacAddress(pucVerifyBuf);
	if( eFdiErr != FDI_NO_ERR )
	{
		FVK_MEM_Free(pucVerifyBuf);
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetMacAddress] FDI_SYSTEM_GetMacAddress Error : 0x%08x\n", eFdiErr));
		return FAI_ERR;
	}

	eFdiErr = FVK_MEM_Compare((void*)pucBuf, (void*)pucVerifyBuf, FDI_SYSTEM_DATA_MAC_SIZE);
	if( eFdiErr != FDI_NO_ERR )
	{
		FVK_MEM_Free(pucVerifyBuf);
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetMacAddress] MacAddress verification fail.\n"));
		return FAI_ERR;
	}

	FVK_MEM_Free(pucVerifyBuf);

	return FAI_NO_ERR;
}
#endif

#if defined(FACTORY_USE_CABLEMODEM)			
/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static FAI_ERR_CODE INTRN_FAI_MES_SetCmMacAddress(unsigned char *pucBuf, unsigned short usSize)
{
	FDI_ERR_CODE  eFdiErr;
	unsigned char *pucVerifyBuf;
	int i;
	
	if( usSize != FDI_SYSTEM_DATA_MAC_SIZE)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetMacAddress] Invaild MacAddress Size : 0x%08x\n", usSize));
		return FAI_ERR;
	}

	eFdiErr = FDI_SYSTEM_SetCmMacAddress(pucBuf);
	if( eFdiErr != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetMacAddress] FDI_SYSTEM_SetMacAddress Error : 0x%08x\n", eFdiErr));
		return FAI_ERR;
	}

	pucVerifyBuf = (unsigned char *)FVK_MEM_Alloc(FDI_SYSTEM_DATA_MAC_SIZE);
	if( pucVerifyBuf == NULL)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetMacAddress] FVK_MEM_Alloc Error.\n"));
		return FAI_ERR;
	}

	/* Verify */
	eFdiErr = FDI_SYSTEM_GetCmMacAddress(pucVerifyBuf);
	if( eFdiErr != FDI_NO_ERR )
	{
		FVK_MEM_Free(pucVerifyBuf);
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetMacAddress] FDI_SYSTEM_GetMacAddress Error : 0x%08x\n", eFdiErr));
		return FAI_ERR;
	}

	eFdiErr = FVK_MEM_Compare((void*)pucBuf, (void*)pucVerifyBuf, FDI_SYSTEM_DATA_MAC_SIZE);
	if( eFdiErr != FDI_NO_ERR )
	{
		FVK_MEM_Free(pucVerifyBuf);
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetMacAddress] MacAddress verification fail.\n"));
		return FAI_ERR;
	}

	FVK_MEM_Free(pucVerifyBuf);

	return FAI_NO_ERR;
}
#endif

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static FAI_ERR_CODE INTRN_FAI_MES_SetHdcpKey(unsigned char *pucBuf, unsigned short usSize)
{
	FDI_ERR_CODE  eFdiErr;
	unsigned char *pucVerifyBuf;
	int i;

#if 1
	if( usSize != FDI_SYSTEM_DATA_HDCPKEY_SIZE)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetHdcpkey] Invaild HDCP Key Size : 0x%08x\n", usSize));
		return FAI_ERR;
	}
#endif
	
	eFdiErr = FDI_SYSTEM_SetHdcpKey(pucBuf, usSize);
	if( eFdiErr != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetHdcpkey] FDI_SYSTEM_SetHdcpkey Error : 0x%08x\n", eFdiErr));
		return FAI_ERR;
	}

	pucVerifyBuf = (unsigned char *)FVK_MEM_Alloc(usSize);
	if( pucVerifyBuf == NULL)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetHdcpkey] FVK_MEM_Alloc Error.\n"));
		return FAI_ERR;
	}

	/* Verify */
	eFdiErr = FDI_SYSTEM_GetHdcpKey(pucVerifyBuf, usSize);
	if( eFdiErr != FDI_NO_ERR )
	{
		FVK_MEM_Free(pucVerifyBuf);
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetHdcpkey] FDI_SYSTEM_GetHdcpkey Error : 0x%08x\n", eFdiErr));
		return FAI_ERR;
	}

	eFdiErr = FVK_MEM_Compare((void*)pucBuf, (void*)pucVerifyBuf, usSize);
	if( eFdiErr != FDI_NO_ERR )
	{
		FVK_MEM_Free(pucVerifyBuf);
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetHdcpkey] HDCP Key verification fail.\n"));
		return FAI_ERR;
	}
	
	FVK_MEM_Free(pucVerifyBuf);
	return FAI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static FAI_ERR_CODE INTRN_FAI_MES_SetDtcpKey(unsigned char *pucBuf, unsigned short usSize)
{
	FDI_ERR_CODE  eFdiErr;
	unsigned char *pucVerifyBuf;
#if (SYSINFO_DTCP_ENCRYPT_ENABLE==YES)	
	unsigned long ulCrc = 0 ;
	unsigned char aulCrc[4];
	unsigned char *tmpBuf;
#endif
	int i;
#if 1
	if( usSize != FDI_SYSTEM_DATA_DTCPKEY_SIZE)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetDtcpkey] Invaild DTCP Key Size : 0x%08x\n", usSize));
		return FAI_ERR;
	}
#endif
	
#if (SYSINFO_DTCP_ENCRYPT_ENABLE==YES)
	if(memcmp(pucBuf,"DTCP-IP:",8))
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetDtcpKey] : Invalid DTCP Header \n"));
		return FAI_ERR;
	}
	if(memcmp(pucBuf+360,":DTCP-IP",8))
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetDtcpKey] : Invalid DTCP tail \n"));
		return FAI_ERR;
	}
	tmpBuf = FVK_MEM_Alloc(352);
	eFdiErr = FDI_CRYPT_Decrypt(pucBuf+8,tmpBuf,352);
	if( eFdiErr != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetDtcpkey] FDI_CRYPT_Decrypt Error : 0x%08x\n", eFdiErr));
		FVK_MEM_Free(tmpBuf);
		return FAI_ERR;
	}
	ulCrc = FAI_UTIL_GetCRC32Fast(tmpBuf, 348);

	aulCrc[0] = (ulCrc&0xFF000000)>>24;
	aulCrc[1] = (ulCrc&0x00FF0000)>>16;
	aulCrc[2] = (ulCrc&0x0000FF00)>>8;
	aulCrc[3] = (ulCrc&0x000000FF);

	if(memcmp(aulCrc,&tmpBuf[348],4))
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetDtcpKey] : CRC did not match \n"));
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetDtcpKey] : ulCrc 0x%x \n",ulCrc));				
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetDtcpKey] : 0x%x 0x%x 0x%x 0x%x \n",aulCrc[0],aulCrc[1],aulCrc[2],aulCrc[3]));		
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetDtcpKey] : 0x%x 0x%x 0x%x 0x%x \n",tmpBuf[348],tmpBuf[349],tmpBuf[350],tmpBuf[351]));				
		FVK_MEM_Free(tmpBuf);
		return FAI_ERR;
	}
	FVK_MEM_Free(tmpBuf);

#endif
	eFdiErr = FDI_SYSTEM_SetDtcpKey(pucBuf, usSize);
	if( eFdiErr != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetDtcpkey] FDI_SYSTEM_SetDtcpkey Error : 0x%08x\n", eFdiErr));
		return FAI_ERR;
	}

	pucVerifyBuf = (unsigned char *)FVK_MEM_Alloc(usSize);
	if( pucVerifyBuf == NULL)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetDtcpkey] FVK_MEM_Alloc Error.\n"));
		return FAI_ERR;
	}

	/* Verify */
	eFdiErr = FDI_SYSTEM_GetDtcpKey(pucVerifyBuf, usSize);
	if( eFdiErr != FDI_NO_ERR )
	{
		FVK_MEM_Free(pucVerifyBuf);
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetDtcpkey] FDI_SYSTEM_GetDtcpkey Error : 0x%08x\n", eFdiErr));
		return FAI_ERR;
	}

	eFdiErr = FVK_MEM_Compare((void*)pucBuf, (void*)pucVerifyBuf, usSize);
	if( eFdiErr != FDI_NO_ERR )
	{
		FVK_MEM_Free(pucVerifyBuf);
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetDtcpkey] DTCP Key verification fail.\n"));
		return FAI_ERR;
	}

	FVK_MEM_Free(pucVerifyBuf);
	return FAI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static FAI_ERR_CODE INTRN_FAI_MES_SetCiPlusKeyData(unsigned char *pucBuf, unsigned short usSize)
{
	FDI_ERR_CODE  eFdiErr;
	unsigned char *pucVerifyBuf;
	int i;
#if 1
	if( usSize != FDI_SYSTEM_DATA_CIPLUSKEY_SIZE)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetDtcpkey] Invaild DTCP Key Size : 0x%08x\n", usSize));
		return FAI_ERR;
	}
#endif
	
	eFdiErr = FDI_SYSTEM_SetCiPlusKeyData(pucBuf, usSize);
	if( eFdiErr != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetCiPlusKeyData] FDI_SYSTEM_SetCiPlusKeyData Error : 0x%08x\n", eFdiErr));
		return FAI_ERR;
	}

	pucVerifyBuf = (unsigned char *)FVK_MEM_Alloc(usSize);
	if( pucVerifyBuf == NULL)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetCiPlusKeyData] FVK_MEM_Alloc Error.\n"));
		return FAI_ERR;
	}

	/* Verify */
	eFdiErr = FDI_SYSTEM_GetCiPlusKeyData(pucVerifyBuf, usSize);
	if( eFdiErr != FDI_NO_ERR )
	{
		FVK_MEM_Free(pucVerifyBuf);
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetCiPlusKeyData] FDI_SYSTEM_GetCiPlusKeyData Error : 0x%08x\n", eFdiErr));
		return FAI_ERR;
	}

	eFdiErr = FVK_MEM_Compare((void*)pucBuf, (void*)pucVerifyBuf, usSize);
	if( eFdiErr != FDI_NO_ERR )
	{
		FVK_MEM_Free(pucVerifyBuf);
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetCiPlusKeyData] DTCP Key verification fail.\n"));
		return FAI_ERR;
	}

	FVK_MEM_Free(pucVerifyBuf);
	return FAI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static FAI_ERR_CODE INTRN_FAI_MES_SetPairKey(unsigned char *pucBuf, unsigned short usSize)
{
	FDI_ERR_CODE  eFdiErr;
	unsigned char *pucVerifyBuf;
	int i;
#if 0
	if( usSize != FDI_SYSTEM_DATA_PAIRKEY_SIZE)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetDtcpkey] Invaild Pair Key Size : 0x%08x\n", usSize));
		return FAI_ERR;
	}
#endif
	
	eFdiErr = FDI_SYSTEM_SetPairKey(pucBuf, usSize);
	if( eFdiErr != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetPairKey] FDI_SYSTEM_SetPairkey Error : 0x%08x\n", eFdiErr));
		return FAI_ERR;
	}

	pucVerifyBuf = (unsigned char *)FVK_MEM_Alloc(usSize);
	if( pucVerifyBuf == NULL)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetPairKey] FVK_MEM_Alloc Error.\n"));
		return FAI_ERR;
	}

	/* Verify */
	eFdiErr = FDI_SYSTEM_GetPairKey(pucVerifyBuf, usSize);
	if( eFdiErr != FDI_NO_ERR )
	{
		FVK_MEM_Free(pucVerifyBuf);
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetPairKey] FDI_SYSTEM_GetPairkey Error : 0x%08x\n", eFdiErr));
		return FAI_ERR;
	}

	eFdiErr = FVK_MEM_Compare((void*)pucBuf, (void*)pucVerifyBuf, usSize);
	if( eFdiErr != FDI_NO_ERR )
	{
		FVK_MEM_Free(pucVerifyBuf);
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetPairKey] Pair Key verification fail.\n"));
		return FAI_ERR;
	}

	FVK_MEM_Free(pucVerifyBuf);
	return FAI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static FAI_ERR_CODE INTRN_FAI_MES_SetDstbId(unsigned char *pucBuf, unsigned short usSize)
{
	FDI_ERR_CODE  eFdiErr;
	unsigned char *pucVerifyBuf;
	int i;
	
	FAI_PRINT(FAI_PRT_DBGINFO, ("[INTRN_FAI_MES_SetDstbId] : ++!!\n"));

	if( usSize != FDI_SYSTEM_DATA_DSTBID_SIZE)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetDstbId] Invaild Serialnumber Size : 0x%08x\n", usSize));
		return FAI_ERR;
	}

	eFdiErr = FDI_SYSTEM_SetDstbId(pucBuf, FDI_SYSTEM_DATA_DSTBID_SIZE);
	if( eFdiErr != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetDstbId] FDI_SYSTEM_SetSerialNumber Error : 0x%08x\n", eFdiErr));
		return FAI_ERR;
	}

	pucVerifyBuf = (unsigned char *)FVK_MEM_Alloc(FDI_SYSTEM_DATA_DSTBID_SIZE);
	if( pucVerifyBuf == NULL)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetDstbId] FVK_MEM_Alloc Error.\n"));
		return FAI_ERR;
	}

	/* Verify */
	eFdiErr = FDI_SYSTEM_GetDstbId(pucVerifyBuf, FDI_SYSTEM_DATA_DSTBID_SIZE);
	if( eFdiErr != FDI_NO_ERR )
	{
		FVK_MEM_Free(pucVerifyBuf);
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetDstbId] FDI_SYSTEM_GetSerialNumber Error : 0x%08x\n", eFdiErr));
		return FAI_ERR;
	}

	eFdiErr = FVK_MEM_Compare((void*)pucBuf, (void*)pucVerifyBuf, FDI_SYSTEM_DATA_DSTBID_SIZE);
	if( eFdiErr != FDI_NO_ERR )
	{
		FVK_MEM_Free(pucVerifyBuf);
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetDstbId] SerialNumber verification fail.\n"));
		return FAI_ERR;
	}

	FVK_MEM_Free(pucVerifyBuf);

	return FAI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static FAI_ERR_CODE INTRN_FAI_MES_SetCSCKey(unsigned char *pucBuf, unsigned short usSize)
{
	FDI_ERR_CODE  eFdiErr;
	unsigned char *pucVerifyBuf;
	int i;

	FAI_PRINT(FAI_PRT_DBGINFO, ("[INTRN_FAI_MES_SetCSCKey] : ++!!\n"));

	if( (usSize == 0) || (pucBuf == NULL) )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetCSCKey] Invaild CSC Key Size : 0x%08x\n", usSize));
		return FAI_ERR;
	}

    eFdiErr = FDI_SYSTEM_SetCSCKey(pucBuf, usSize);
    if ( eFdiErr != FDI_NO_ERR )
    {
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetCSCKey] Invaild CSC Key Size : 0x%08x\n", usSize));
        return FAI_ERR;
    }

	FAI_PRINT(FAI_PRT_DBGINFO, ("[INTRN_FAI_MES_SetCSCKey] : --!!\n"));

	return FAI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static FAI_ERR_CODE INTRN_FAI_MES_SetVAExtraData(unsigned char *pucBuf, unsigned short usSize)
{
	FDI_ERR_CODE  eFdiErr;
	unsigned char *pucVerifyBuf;
	int i;

	if( usSize != FDI_SYSTEM_DATA_VAEXTRADATA_SIZE)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetVAExtraData] Invaild VA Extra Data Size : 0x%08x\n", usSize));
		return FAI_ERR;
	}

	eFdiErr = FDI_SYSTEM_SetVAExtraData(pucBuf);
	if( eFdiErr != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetVAExtraData] FDI_SYSTEM_SetVAExtraData Error : 0x%08x\n", eFdiErr));
		return FAI_ERR;
	}

	pucVerifyBuf = (unsigned char *)FVK_MEM_Alloc(FDI_SYSTEM_DATA_VAEXTRADATA_SIZE);
	if( pucVerifyBuf == NULL)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetVAExtraData] FVK_MEM_Alloc Error.\n"));
		return FAI_ERR;
	}

	/* Verify */
	eFdiErr = FDI_SYSTEM_GetVAExtraData(pucVerifyBuf);
	if( eFdiErr != FDI_NO_ERR )
	{
		FVK_MEM_Free(pucVerifyBuf);
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetVAExtraData] FDI_SYSTEM_GetVAExtraData Error : 0x%08x\n", eFdiErr));
		return FAI_ERR;
	}

	eFdiErr = FVK_MEM_Compare((void*)pucBuf, (void*)pucVerifyBuf, FDI_SYSTEM_DATA_VAEXTRADATA_SIZE);
	if( eFdiErr != FDI_NO_ERR )
	{
		FVK_MEM_Free(pucVerifyBuf);
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetVAExtraData] VA Extra Data verification fail.\n"));
		return FAI_ERR;
	}

	FVK_MEM_Free(pucVerifyBuf);

	return FAI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static FAI_ERR_CODE INTRN_FAI_MES_SetIEEEAddr(unsigned char *pucBuf, unsigned short usSize)
{
	FDI_ERR_CODE  eFdiErr;
	unsigned char *pucVerifyBuf;
	int i;

	if( usSize != FDI_SYSTEM_DATA_RF4CE_IEEE_ADDR_SIZE)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetVAExtraData] Invaild VA Extra Data Size : 0x%08x\n", usSize));
		return FAI_ERR;
	}

	eFdiErr = FDI_SYSTEM_SetIEEEAddr(pucBuf);
	if( eFdiErr != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetVAExtraData] FDI_SYSTEM_SetVAExtraData Error : 0x%08x\n", eFdiErr));
		return FAI_ERR;
	}

	pucVerifyBuf = (unsigned char *)FVK_MEM_Alloc(FDI_SYSTEM_DATA_RF4CE_IEEE_ADDR_SIZE);
	if( pucVerifyBuf == NULL)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetVAExtraData] FVK_MEM_Alloc Error.\n"));
		return FAI_ERR;
	}

	/* Verify */
	eFdiErr = FDI_SYSTEM_GetIEEEAddr(pucVerifyBuf);
	if( eFdiErr != FDI_NO_ERR )
	{
		FVK_MEM_Free(pucVerifyBuf);
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetVAExtraData] FDI_SYSTEM_GetVAExtraData Error : 0x%08x\n", eFdiErr));
		return FAI_ERR;
	}

	eFdiErr = FVK_MEM_Compare((void*)pucBuf, (void*)pucVerifyBuf, FDI_SYSTEM_DATA_RF4CE_IEEE_ADDR_SIZE);
	if( eFdiErr != FDI_NO_ERR )
	{
		FVK_MEM_Free(pucVerifyBuf);
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetVAExtraData] VA Extra Data verification fail.\n"));
		return FAI_ERR;
	}

	FVK_MEM_Free(pucVerifyBuf);

	return FAI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static FAI_ERR_CODE INTRN_FAI_MES_SetPrivateData(unsigned char *pucBuf, unsigned short usSize)
{
	FDI_ERR_CODE  eFdiErr;
	unsigned char *pucVerifyBuf;
	int i;
	
	if( usSize != FDI_SYSTEM_DATA_IRDETO_PRIVATEDATA_SIZE)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetPrivateData] Invaild IRDETO_PrivateData Size : 0x%08x\n", usSize));
		return FAI_ERR;
	}

	eFdiErr = FDI_SYSTEM_SetPrivateData(pucBuf);
	if( eFdiErr != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetPrivateData] FDI_SYSTEM_SetPrivateData Error : 0x%08x\n", eFdiErr));
		return FAI_ERR;
	}

	pucVerifyBuf = (unsigned char *)FVK_MEM_Alloc(FDI_SYSTEM_DATA_IRDETO_PRIVATEDATA_SIZE);
	if( pucVerifyBuf == NULL)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetPrivateData] FVK_MEM_Alloc Error.\n"));
		return FAI_ERR;
	}

	/* Verify */
	eFdiErr = FDI_SYSTEM_GetPrivateData(pucVerifyBuf);
	if( eFdiErr != FDI_NO_ERR )
	{
		FVK_MEM_Free(pucVerifyBuf);
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetPrivateData] FDI_SYSTEM_GetPrivateData Error : 0x%08x\n", eFdiErr));
		return FAI_ERR;
	}

	eFdiErr = FVK_MEM_Compare((void*)pucBuf, (void*)pucVerifyBuf, FDI_SYSTEM_DATA_IRDETO_PRIVATEDATA_SIZE);
	if( eFdiErr != FDI_NO_ERR )
	{
		FVK_MEM_Free(pucVerifyBuf);
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetPrivateData] IRDETO_PrivateData verification fail.\n"));
		return FAI_ERR;
	}

	FVK_MEM_Free(pucVerifyBuf);

	return FAI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static FAI_ERR_CODE INTRN_FAI_MES_SetVerimatrixCasData(unsigned char *pucBuf, unsigned short usSize)
{
	FDI_ERR_CODE  eFdiErr;
	unsigned char *pucVerifyBuf;
	int i;
	
	if( usSize != 32000/*FDI_SYSTEM_DATA_VERIMATRIX_CAS_DATA_SIZE*/)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetVerimatrixCasData] Invaild INTRN_FAI_MES_SetVerimatrixData Size : 0x%08x\n", usSize));
		return FAI_ERR;
	}

	//eFdiErr = FDI_SYSTEM_SetVerimatrixData(pucBuf);
	if( eFdiErr != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetVerimatrixCasData] FDI_SYSTEM_SetVerimatrixData Error : 0x%08x\n", eFdiErr));
		return FAI_ERR;
	}

	pucVerifyBuf = (unsigned char *)FVK_MEM_Alloc(32000/*FDI_SYSTEM_DATA_VERIMATRIX_CAS_DATA_SIZE*/);
	if( pucVerifyBuf == NULL)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetVerimatrixCasData] FVK_MEM_Alloc Error.\n"));
		return FAI_ERR;
	}

	/* Verify */
	//eFdiErr = FDI_SYSTEM_GetVerimatrixData(pucVerifyBuf);
	if( eFdiErr != FDI_NO_ERR )
	{
		FVK_MEM_Free(pucVerifyBuf);
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetVerimatrixCasData] FDI_SYSTEM_GetVerimatrixData Error : 0x%08x\n", eFdiErr));
		return FAI_ERR;
	}

	eFdiErr = FVK_MEM_Compare((void*)pucBuf, (void*)pucVerifyBuf, 32000/*FDI_SYSTEM_DATA_VERIMATRIX_CAS_DATA_SIZE*/);
	if( eFdiErr != FDI_NO_ERR )
	{
		FVK_MEM_Free(pucVerifyBuf);
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetVerimatrixCasData] FDI_SYSTEM_DATA_VERIMATRIX_CAS_DATA_SIZE verification fail.\n"));
		return FAI_ERR;
	}

	FVK_MEM_Free(pucVerifyBuf);

	return FAI_NO_ERR;
}

#if defined(FACTORY_USE_BT)
static FAI_ERR_CODE INTRN_FAI_MES_SetBTMacAddress(unsigned char *pucBuf, unsigned short usSize)
{
	FDI_ERR_CODE  eFdiErr;
	unsigned char *pucVerifyBuf;
	int i;
	
	if( usSize != FDI_SYSTEM_DATA_MAC_SIZE)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetBTMacAddress] Invaild MacAddress Size : 0x%08x\n", usSize));
		return FAI_ERR;
	}

	eFdiErr = FDI_SYSTEM_SetBTMacAddress(pucBuf);
	if( eFdiErr != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetBTMacAddress] FDI_SYSTEM_SetBTMacAddress Error : 0x%08x\n", eFdiErr));
		return FAI_ERR;
	}

	pucVerifyBuf = (unsigned char *)FVK_MEM_Alloc(FDI_SYSTEM_DATA_MAC_SIZE);
	if( pucVerifyBuf == NULL)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetBTMacAddress] FVK_MEM_Alloc Error.\n"));
		return FAI_ERR;
	}

	/* Verify */
	eFdiErr = FDI_SYSTEM_GetBTMacAddress(pucVerifyBuf);
	if( eFdiErr != FDI_NO_ERR )
	{
		FVK_MEM_Free(pucVerifyBuf);
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetBTMacAddress] FDI_SYSTEM_GetBTMacAddress Error : 0x%08x\n", eFdiErr));
		return FAI_ERR;
	}

	eFdiErr = FVK_MEM_Compare((void*)pucBuf, (void*)pucVerifyBuf, FDI_SYSTEM_DATA_MAC_SIZE);
	if( eFdiErr != FDI_NO_ERR )
	{
		FVK_MEM_Free(pucVerifyBuf);
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MES_SetBTMacAddress] MacAddress verification fail.\n"));
		return FAI_ERR;
	}

	FVK_MEM_Free(pucVerifyBuf);

	return FAI_NO_ERR;
}
#endif

/* end of file */

