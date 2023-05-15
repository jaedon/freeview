/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory System Driver Interface
*
* Copyright (c) 2009 by Humax Co., Ltd.
* All right reserved
******************************************************************************/

/*******************************************************************************
* header and definitions
*******************************************************************************/
/* global header files */

/* chip manufacture's  header files */

/* humax header files */
#include "htype.h"
#include "vkernel.h"

#include "fvk.h"
#include "fdi_common.h"
#include "fdi_uart.h"
#include "fdi_system.h"

#include "di_err.h"
#include "di_flash.h"
#include "di_eeprom.h"
#include "di_nvram.h"
#include "di_system.h"
#include "di_factory.h"
#include "di_crypt.h"
#include "di_power.h"

#include "drv_err.h"
#if defined(CONFIG_MICOM)
#include "drv_micom.h"
#endif

#if defined(CONFIG_CAS)
#include "drv_csd.h"
#endif

#include "nvram_map.h"
#if defined(CONFIG_CAS_NA)
#include "flash_map.h"
#endif

#if (VERIMATRIX_BETACRYPT_NSC_ENABLE==YES)
#include "fdi_cas_vmxbc_api.h" // bc_consts.h + bc_types.h + bc_main.h 을 하나로 통합.
#endif

/* SCS fusing 관련 config 들..  Block size / Key index */
/* FUSE_BLOCK_SIZE : SCS checkd block size in KB , -1 은 사용 하지 않을 경우의 값.  */
/* FUSE_KEY_INDEX : SCS key index  , -1 은 사용 하지 않을 경우의 값.  */

/*
#if (PRODUCT_ICORDHDPLUS==YES)
#define FUSE_BLOCK_SIZE		128
#define FUSE_KEY_INDEX		0x0D
#define NAGRA_CSC_PROVIDER_ID 0xFFFFFFFF
#elif (PRODUCT_HDFOXPLUS==YES)
#define FUSE_BLOCK_SIZE		128
#define FUSE_KEY_INDEX		0x0D
#define NAGRA_CSC_PROVIDER_ID 0x00003411
#elif(PRODUCT_PRHD2000C==YES)
#define FUSE_BLOCK_SIZE		-1
#define FUSE_KEY_INDEX		0x02
#define NAGRA_CSC_PROVIDER_ID 0x00000401
#elif(PRODUCT_HD5500T==YES)
#define FUSE_BLOCK_SIZE		-1
#define FUSE_KEY_INDEX		0x02
#define NAGRA_CSC_PROVIDER_ID 0x00000401
#elif(PRODUCT_DIGIPLUSC_HD==YES)
#define FUSE_BLOCK_SIZE		-1
#define FUSE_KEY_INDEX		0x02
#define NAGRA_CSC_PROVIDER_ID 0x00000401
#else
#define FUSE_BLOCK_SIZE		-1
#define FUSE_KEY_INDEX		-1
#define NAGRA_CSC_PROVIDER_ID 0xFFFFFFFF
#endif
*/

#ifndef NAGRA_CSC_PROVIDER_ID
#define NAGRA_CSC_PROVIDER_ID 0xFFFFFFFF
#endif
#define FUSE_BLOCK_SIZE		-1
#define FUSE_KEY_INDEX		0x0

/*******************************************************************************
* Debug level
*******************************************************************************/

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/
#define SYSTEM_ID_SIZE 			4

#define IR_FLASH_CPCB_OFFSET 0x01D60000

#define WIFI_MAC_ADDR "WLAN_MAC_ADDR"
#define WIFI_MAC_SIZE 6

#if (VERIMATRIX_BETACRYPT_NSC_ENABLE==YES)
#define MAX_NSC_DATA_LEN		(48)
#define MAX_NSC_BOXID_LEN	(8)

typedef struct
{
	unsigned char ucData[MAX_NSC_DATA_LEN];
	unsigned short usLength;
	unsigned char ucBoxId[MAX_NSC_BOXID_LEN];
	unsigned char *pucVendorId;
	unsigned char *pucVendorData;
	unsigned char *pucProvId;
	unsigned char *pucProvData;
} FDI_SYSTEM_VMXBC_NSc_t;

FDI_SYSTEM_VMXBC_NSc_t	stFdiSystemVmxBcNSc;
#endif

/*******************************************************************************
* External variables and functions
*******************************************************************************/
/* external variables  and functions */

/*******************************************************************************
* Global variables and structures
*******************************************************************************/
/* global variables and structures */

/*******************************************************************************
* function : FDI_SYSTEM_Open
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_Open(void)
{
	return FDI_NO_ERR;
}

/*******************************************************************************
* function : FDI_SYSTEM_GetFieldSize
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
unsigned long FDI_SYSTEM_GetFieldSize(FDI_SYSTEM_DATA_e eData)
{
	unsigned long ulDataSize = 0;

	switch(eData)
	{
		case FDI_SYSTEM_DATA_SN:
			ulDataSize = SERIAL_SIZE;
			break;
		case FDI_SYSTEM_DATA_MAC:
			ulDataSize = 6;
			break;
		case FDI_SYSTEM_DATA_HDCPKEY:
			ulDataSize = HDCP_SIZE;
			break;
		case FDI_SYSTEM_DATA_CHIPID:
#if defined(CONFIG_SUPPORT_CHIPID_64X)
			ulDataSize = 8;
#else
			ulDataSize = 4;
#endif
			break;
		case FDI_SYSTEM_DATA_CIPLUSKEY:
			ulDataSize = 8480;
			break;
		case FDI_SYSTEM_DATA_DTCPKEY:
#if defined(DTCP_SIZE)
			ulDataSize = DTCP_SIZE;
#else
			ulDataSize = 348;
#endif
			break;
		case FDI_SYSTEM_DATA_PAIRKEY:
#if defined(DATA_PAIRKEY)
			ulDataSize = DATA_PAIRKEY;
#else
			ulDataSize = 500;
#endif
			break;
		case FDI_SYSTEM_DATA_DSTBID:
			ulDataSize = 6;
			break;
		case FDI_SYSTEM_DATA_VAEXTRADATA:
			ulDataSize = 8192;
			break;
		case FDI_SYSTEM_DATA_RF4CE_IEEE_ADDR:
			ulDataSize = 8;
			break;
		case FDI_SYSTEM_DATA_HDD_SN:
			ulDataSize = 20;
			break;
		case FDI_SYSTEM_IRDETO_PRIVATEDATA:
			ulDataSize = 16;
#if (VERIMATRIX_BETACRYPT_NSC_ENABLE==YES)
		case FDI_SYSTEM_BC_NSC_DATA:
			ulDataSize = 32708;
#endif
		default:
			break;
	}
	return ulDataSize;
}

/*******************************************************************************
* function : FDI_SYSTEM_Init
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_GetSystemId(unsigned long *pulSysId)
{
	unsigned long ulSysId;

#if defined(CONFIG_DI10)
	DI_EEPROM_GetField(DI_EEPROM_FIELD_SYSTEM_ID, &ulSysId, SYSTEM_ID_SIZE);
	*pulSysId = ulSysId;
#endif
#if defined(CONFIG_DI20)
	(void)DI_NVRAM_GetField(DI_NVRAM_FIELD_SYSTEM_ID, 0, &ulSysId, SYSTEM_ID_SIZE);
	*pulSysId = ulSysId;
#endif

	return FDI_NO_ERR;
}

/*******************************************************************************
* function : FDI_SYSTEM_Init
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_GetMicomVersion(unsigned char *pucVersion)
{
	unsigned int unMicomVer, unUpperVer, unLowerVer1;
	DI_ERR_CODE err = 0;

#if	(FDI_MICOM_VERION_TYPE == FDI_VERSION_TYPE_NUMERIC_WITH_STRING)

	err = DI_SYSTEM_GetSystemInfo(DI_SYS_MICOM_STRVER, pucVersion, 10);

	if(err != DI_ERR_OK)
			return FDI_ERR;

#if (FDI_MICOM_VERSION_DISPLAY_DECIMAL == YES)
	sprintf(pucVersion, "%c%c%c%c%c%c %d.%d.%02d",pucVersion[0],pucVersion[1],pucVersion[2],pucVersion[3],pucVersion[4],pucVersion[5],
						pucVersion[7],pucVersion[8], pucVersion[9]);
#else
	sprintf(pucVersion, "%c%c%c%c%c%c %X.%X.%02X",pucVersion[0],pucVersion[1],pucVersion[2],pucVersion[3],pucVersion[4],pucVersion[5],
						pucVersion[7],pucVersion[8], pucVersion[9]);
#endif

#else
	DRV_MICOM_GetVersion(&unMicomVer);
	unUpperVer = (unMicomVer >> 8) & 0xFF;
	unLowerVer1 = unMicomVer & 0xFF;
	sprintf(pucVersion, "%02X.%02X", unUpperVer, unLowerVer1);
#endif

	return FDI_NO_ERR;
}

/*******************************************************************************
* function : FDI_SYSTEM_Init
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_GetAppVersion(unsigned char *pucVersion)
{
	unsigned long ulVersion;
	DI_ERR_CODE ret;

#if	(FDI_APPLICATION_VERION_TYPE == FDI_VERSION_TYPE_NUMERIC_WITH_STRING)
	//New Version with Model Prefix
	ret = DI_NVRAM_GetField(DI_NVRAM_FIELD_APP_VERSION, 0, pucVersion, 9);

	if(ret != DI_ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[%s] : DI_FLASH_GetField(DI_FLASH_FIELD_APP_VER) Error = %d\n", __func__, ret));
		return FDI_ERR;
	}
	else
	{
		sprintf(pucVersion, "%c%c%c%c%c%c %d.%02d.%02d", pucVersion[0],pucVersion[1],pucVersion[2],pucVersion[3],pucVersion[4],pucVersion[5],pucVersion[6],pucVersion[7],pucVersion[8]);
	}

#else
   //Old type version
	ret = DI_NVRAM_GetField(DI_NVRAM_FIELD_APP_VERSION, 0, &ulVersion, 4);

	if(ret != DI_ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[%s] : DI_FLASH_GetField(DI_FLASH_FIELD_APP_VER) Error = %d\n", __func__, ret));
		return FDI_ERR;
	}
	else
	{
		sprintf(pucVersion, "%d.%02x.%02x", (ulVersion >> 16) & 0xff, (ulVersion >> 8) & 0xff, ulVersion & 0xff);

	}
#endif
	return FDI_NO_ERR;
}

/*******************************************************************************
* function : FDI_SYSTEM_Init
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_GetLoaderVersion(unsigned char *pucVersion)
{

#if (LOADER_TYPE_IRDETO_LOADER==YES)

#define IR_BBCB_SIZE 24
#define IR_BBCB_OFFSET	(0x01CA0000 - IR_BBCB_SIZE)
#define IR_MAJOR_LOADER_VER_OFFSET 4
#define IR_MINOR_LOADER_VER_OFFSET 5
#define IR_TYPE_LOADER_VER_OFFSET 6

	unsigned char aucBBCB[IR_BBCB_SIZE];
	unsigned char ucMajorLoaderVer, ucMinorLoaderVer, ucTypeLoaderVer;

	/* read BBCB area */
	#if 0
	//NOR
	DI_FLASH_Read(IR_BBCB_OFFSET, aucBBCB, IR_BBCB_SIZE);
	#else
	//NAND
	DI_NVRAM_GetField(DI_NVRAM_FIELD_IRDETO_BBCB, 0, aucBBCB, IR_BBCB_SIZE);
	#endif

	ucMajorLoaderVer = aucBBCB[IR_MAJOR_LOADER_VER_OFFSET];
	ucMinorLoaderVer = aucBBCB[IR_MINOR_LOADER_VER_OFFSET];
	ucTypeLoaderVer = aucBBCB[IR_TYPE_LOADER_VER_OFFSET];

	//700320 12.0 DVT test application Software Requirements Specification.pdf
	//for the loader 3.2.the loader Ver filed should be displayed as “Loader major. Loader type. Loader minor” format.
	sprintf(pucVersion, "%d.%d.%d", ucMajorLoaderVer, ucTypeLoaderVer, ucMinorLoaderVer);


#elif	(FDI_LOADER_VERION_TYPE == FDI_VERSION_TYPE_NUMERIC_WITH_STRING)
// New version type with model prefix
	unsigned char aucLdrVersion[20];
	DI_ERR_CODE eRet;

	eRet = DI_NVRAM_GetField(DI_NVRAM_FIELD_LOADER_VERSION, 0, aucLdrVersion, 8);
	if (eRet != DI_ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_GetLoaderVersion] DI_FLASH_GetField failed(0x%08X)!!\n", eRet));
		sprintf(pucVersion, "??.??");
	}
	else
	{
		sprintf(pucVersion, "%c%c%c%c%c%c %d.%02d", aucLdrVersion[0],aucLdrVersion[1],aucLdrVersion[2],aucLdrVersion[3],aucLdrVersion[4],aucLdrVersion[5],aucLdrVersion[6],aucLdrVersion[7]);
	}


#else
//OLD Style Loader version
	unsigned short usVersion = 0;

	DI_EEPROM_GetField(DI_EEPROM_FIELD_LOADER_VERSION, &usVersion, 2);

	int	nInitial;
	char cInitial;
	int	nTmp;

	nInitial = ((usVersion)&0xF000) >> 12;

	switch(nInitial)
	{
	case 0x0:
		cInitial = 'A';
		break;
	case 0x1:
		cInitial = 'C';
		break;
	case 0x2:
		cInitial = 'F';
		break;
	case 0x3:
		cInitial = 'H';
		break;
	case 0x4:
		cInitial = 'L';
		break;
	case 0x5:
		cInitial = 'U';
		break;
	case 0x6:
		cInitial = 'Y';
		break;
	case 0x7:
		cInitial = 'a';
		break;
	case 0x8:
		cInitial = 'b';
		break;
	case 0x9:
		cInitial = 'h';
		break;
	case 0xa:
		cInitial = 'n';
		break;
	case 0xb:
		cInitial = 'o';
		break;
	case 0xc:
		cInitial = 'r';
		break;
	case 0xd:
		cInitial = 'u';
		break;
	}

	nTmp = (usVersion);
	nTmp &= 0x0F00;
	nTmp = nTmp >> 8;

	sprintf(pucVersion, "%c%01x.%02x", cInitial, nTmp, usVersion&0x00FF);
#endif

	return FDI_NO_ERR;
}

/*******************************************************************************
* function : FDI_SYSTEM_GetHWRevision
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_GetHWRevision(unsigned int *bStatus)
{
	unsigned int tHWRevision;

	DI_SYSTEM_GetSystemInfo(DI_SYS_HW_REVISION, &tHWRevision, 1);
	*bStatus = tHWRevision;

	return FDI_NO_ERR;
}

/*******************************************************************************
* function : FDI_SYSTEM_GetSCSStatus
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_GetSCSStatus(BOOL *bStatus)
{
	DI_SYS_FUSING_INFO_t tInfo;

	DI_SYSTEM_GetSystemInfo(DI_SYS_FUSING_INFO, &tInfo, sizeof(DI_SYS_FUSING_INFO_t));
	*bStatus = tInfo.ucSCS;

	return FDI_NO_ERR;
}

/*******************************************************************************
* function : FDI_SYSTEM_GetJTAGStatus
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_GetJTAGStatus(BOOL *bStatus)
{
	DI_SYS_FUSING_INFO_t tInfo;

	DI_SYSTEM_GetSystemInfo(DI_SYS_FUSING_INFO, &tInfo, sizeof(DI_SYS_FUSING_INFO_t));
	*bStatus = tInfo.ucJTAG;

	return FDI_NO_ERR;
}

/*******************************************************************************
* function : FDI_SYSTEM_GetOTPStatus
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_GetOTPStatus(BOOL *bStatus)
{
	DI_SYS_FUSING_INFO_t tInfo;

	DI_SYSTEM_GetSystemInfo(DI_SYS_FUSING_INFO, &tInfo, sizeof(DI_SYS_FUSING_INFO_t));
	*bStatus = tInfo.ucOTP;

	return FDI_NO_ERR;
}

/*******************************************************************************
* function : FDI_SYSTEM_GetCwMode
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_GetCwMode(BOOL *bStatus)
{
	DI_SYS_FUSING_INFO_t tInfo;

	DI_SYSTEM_GetSystemInfo(DI_SYS_FUSING_INFO, &tInfo, sizeof(DI_SYS_FUSING_INFO_t));
	*bStatus = tInfo.ucCWE;

	return FDI_NO_ERR;
}


/*******************************************************************************
* function : FDI_SYSTEM_GetDataSectionStatus
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_GetDataSectionStatus(unsigned char *pucSectionData,BOOL *bStatus)
{
	int result  = 1;
	DI_SYS_OTP_DATA_SECTION_INFO_t	otpDataSectionInfo;
	DI_ERR_CODE eDiErr = 0;

	if(pucSectionData == NULL)
	{
		*bStatus = result;
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_GetMarketID] : Invalid Parameter!! \n"));
		return FDI_ERR_INVALID_PARAMETER;
	}

	FVK_MEM_Set((void *)&otpDataSectionInfo, 0, sizeof(otpDataSectionInfo));
	otpDataSectionInfo.ucIndex = 0;
	eDiErr = DI_SYSTEM_GetSystemInfo(DI_SYS_FUSE_OTP_DATA_SECTION, &otpDataSectionInfo, sizeof(DI_SYS_OTP_DATA_SECTION_INFO_t));
	if (eDiErr == DI_ERR_OK)
	{
		result = FVK_MEM_Compare((void *)&otpDataSectionInfo.ucDataSection[0], (void *)&pucSectionData[0], DI_SYS_OTP_DATA_SECTION_LENGTH);
	}

	*bStatus = result;

	return FDI_NO_ERR;
}


/*******************************************************************************
* function :	FDI_SYSTEM_GetMarketID
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_GetMarketID(unsigned int *punMarketID)
{
	DI_SYS_FUSING_INFO_t tInfo;
	DI_ERR_CODE eDiErr;

	if( punMarketID == NULL )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_GetMarketID] : Invalid Parameter!! \n"));
		return FDI_ERR;
	}

	DI_SYSTEM_GetSystemInfo(DI_SYS_FUSING_INFO, &tInfo, sizeof(DI_SYS_FUSING_INFO_t));
	*punMarketID = tInfo.ulMarketID;

	return FDI_NO_ERR;
}

/*******************************************************************************
* function :	FDI_SYSTEM_SetMarketID
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_SetMarketID(unsigned int unMarketID)
{
	DI_ERR_CODE eDiErr = FDI_NO_ERR;
	DI_SYS_FUSING_INFO_t tInfo;

	FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_SetMarketID] : Market ID : 0x%x \n", unMarketID));

	if( unMarketID != 0x00 )
	{
		memset((void *)&tInfo, 0, sizeof(tInfo));

		tInfo.ulMarketID = unMarketID;
		eDiErr = DI_SYSTEM_SetSystemInfo(DI_SYS_FUSING_INFO, &tInfo, sizeof(DI_SYS_FUSING_INFO_t));
	}

	return eDiErr;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_GetSerialNumber(unsigned char *pucSN)
{
	DI_ERR_CODE eDiErr;
	int i;

	if( pucSN == NULL )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_GetSerialNumber] : Invalid Parameter!! \n"));
		return FDI_ERR;
	}

#if defined(CONFIG_DI10)
	eDiErr = DI_FLASH_GetField(DI_FLASH_FIELD_SERIAL_NO, pucSN, FDI_SYSTEM_DATA_SN_SIZE);
#endif
#if defined(CONFIG_DI20)
	eDiErr = DI_NVRAM_GetField(DI_NVRAM_FIELD_SERIAL_NO, 0, pucSN, FDI_SYSTEM_DATA_SN_SIZE);
#endif
	if( eDiErr != DI_ERR_OK )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_GetSerialNumber] : DI_FLASH_GetField Error!! \n"));
		return FDI_ERR;
	}

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
FDI_ERR_CODE FDI_SYSTEM_SetSerialNumber(unsigned char *pucSN)
{
	DI_ERR_CODE eDiErr;

	if( pucSN == NULL )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_GetSerialNumber] : Invalid Parameter!! \n"));
		return FDI_ERR;
	}

#if defined(CONFIG_DI10)
	eDiErr = DI_FLASH_SetField(DI_FLASH_FIELD_SERIAL_NO, pucSN, FDI_SYSTEM_DATA_SN_SIZE);
#endif
#if defined(CONFIG_DI20)
	eDiErr = DI_NVRAM_SetField(DI_NVRAM_FIELD_SERIAL_NO, 0, pucSN, FDI_SYSTEM_DATA_SN_SIZE);
#endif
	if( eDiErr != DI_ERR_OK )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_GetSerialNumber] : DI_FLASH_GetField Error!! \n"));
		return FDI_ERR;
	}

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
FDI_ERR_CODE FDI_SYSTEM_GetMacAddress(unsigned char *pucMacAddr)
{
	DI_ERR_CODE eDiErr;

	if( pucMacAddr == NULL )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_GetMacAddress] : Invalid Parameter!! \n"));
		return FDI_ERR;
	}

#if defined(CONFIG_DI10)
	eDiErr = DI_FLASH_GetField(DI_FLASH_FIELD_MAC_ADDR, pucMacAddr, FDI_SYSTEM_DATA_MAC_SIZE);
#endif
#if defined(CONFIG_DI20)
	eDiErr = DI_NVRAM_GetField(DI_NVRAM_FIELD_MAC_ADDR, 0, pucMacAddr, FDI_SYSTEM_DATA_MAC_SIZE);
#endif
	if( eDiErr != DI_ERR_OK )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_GetMacAddress] : DI_FLASH_GetField Error!! \n"));
		return FDI_ERR;
	}

	return FDI_NO_ERR;
}

#if defined(CONFIG_DI20)
FDI_ERR_CODE FDI_SYSTEM_GetSecondMacAddress(unsigned char *pucMacAddr)
{
	DI_ERR_CODE eDiErr;

	if( pucMacAddr == NULL )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_GetSecondMacAddress] : Invalid Parameter!! \n"));
		return FDI_ERR;
	}

	eDiErr = DI_NVRAM_Read(DI_NVRAM_FS_PARTITION_RW,MAC2_ADDR,0, pucMacAddr, FDI_SYSTEM_DATA_MAC_SIZE);
	if( eDiErr != DI_ERR_OK )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_GetSecondMacAddress] : DI_NVRAM_Read Error!! \n"));
		return FDI_ERR;
	}

	return FDI_NO_ERR;
}

FDI_ERR_CODE FDI_SYSTEM_GetThirdMacAddress(unsigned char *pucMacAddr)
{
	DI_ERR_CODE eDiErr;

	if( pucMacAddr == NULL )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_GetThirdMacAddress] : Invalid Parameter!! \n"));
		return FDI_ERR;
	}

	eDiErr = DI_NVRAM_Read(DI_NVRAM_FS_PARTITION_RW,MAC3_ADDR,0, pucMacAddr, FDI_SYSTEM_DATA_MAC_SIZE);
	if( eDiErr != DI_ERR_OK )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_GetThirdMacAddress] : DI_NVRAM_Read Error!! \n"));
		return FDI_ERR;
	}

	return FDI_NO_ERR;
}

#endif


FDI_ERR_CODE FDI_SYSTEM_GetWifiMacAddress(unsigned char *pucMacAddr)
{
	DI_ERR_CODE eDiErr;

	if( pucMacAddr == NULL )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_GetWifiMacAddress] : Invalid Parameter!! \n"));
		return FDI_ERR;
	}

	eDiErr = DI_NVRAM_Read(DI_NVRAM_FS_PARTITION_RO, WIFI_MAC_ADDR, 0, pucMacAddr, WIFI_MAC_SIZE);
	if( eDiErr != DI_ERR_OK )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_GetWifiMacAddress] : DI_NVRAM_WriteError!! \n"));
		return FDI_ERR;
	}

	return FDI_NO_ERR;
}

#if (CABLE_MODEM_ENABLE==YES)
FDI_ERR_CODE FDI_SYSTEM_GetCmMacAddress(unsigned char *pucMacAddr)
{
	DI_ERR_CODE eDiErr;

	if( pucMacAddr == NULL )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_GetCmMacAddress] : Invalid Parameter!! \n"));
		return FDI_ERR;
	}

	eDiErr = DI_NVRAM_GetField(DI_NVRAM_FIELD_CM_MAC_ADDR, 0, pucMacAddr, FDI_SYSTEM_DATA_MAC_SIZE);
	if( eDiErr != DI_ERR_OK )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_GetMacAddress] : DI_FLASH_GetField Error!! \n"));
		return FDI_ERR;
	}
#if 0
	eDiErr = DI_CM_GetCmMac(pucMacAddr);
	if( eDiErr != DI_ERR_OK )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_GetCmMacAddress] : DI_FLASH_GetField Error!! \n"));
		return FDI_ERR;
	}
#endif
	return FDI_NO_ERR;
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
FDI_ERR_CODE FDI_SYSTEM_SetMacAddress(unsigned char *pucMacAddr)
{
	DI_ERR_CODE eDiErr;

	if( pucMacAddr == NULL )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_GetMacAddress] : Invalid Parameter!! \n"));
		return FDI_ERR;
	}

#if defined(CONFIG_DI10)
	eDiErr = DI_FLASH_SetField(DI_FLASH_FIELD_MAC_ADDR, pucMacAddr, FDI_SYSTEM_DATA_MAC_SIZE);
#endif
#if defined(CONFIG_DI20)
	eDiErr = DI_NVRAM_SetField(DI_NVRAM_FIELD_MAC_ADDR, 0, pucMacAddr, FDI_SYSTEM_DATA_MAC_SIZE);
#endif
	if( eDiErr != DI_ERR_OK )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_GetMacAddress] : DI_FLASH_GetField Error!! \n"));
		return FDI_ERR;
	}

	return FDI_NO_ERR;
}

#if defined(CONFIG_DI20)
FDI_ERR_CODE FDI_SYSTEM_SetSecondMacAddress(unsigned char *pucMacAddr)
{
	DI_ERR_CODE eDiErr;

	if( pucMacAddr == NULL )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_SetSecondMacAddress] : Invalid Parameter!! \n"));
		return FDI_ERR;
	}

	eDiErr = DI_NVRAM_Write(DI_NVRAM_FS_PARTITION_RW,MAC2_ADDR,0, pucMacAddr, FDI_SYSTEM_DATA_MAC_SIZE);
	if( eDiErr != DI_ERR_OK )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_SetSecondMacAddress] : DI_NVRAM_Read Error!! \n"));
		return FDI_ERR;
	}

	return FDI_NO_ERR;
}

FDI_ERR_CODE FDI_SYSTEM_SetThirdMacAddress(unsigned char *pucMacAddr)
{
	DI_ERR_CODE eDiErr;

	if( pucMacAddr == NULL )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_SetThirdMacAddress] : Invalid Parameter!! \n"));
		return FDI_ERR;
	}

	eDiErr = DI_NVRAM_Write(DI_NVRAM_FS_PARTITION_RW,MAC3_ADDR,0, pucMacAddr, FDI_SYSTEM_DATA_MAC_SIZE);
	if( eDiErr != DI_ERR_OK )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_SetThirdMacAddress] : DI_NVRAM_Read Error!! \n"));
		return FDI_ERR;
	}

	return FDI_NO_ERR;
}
#endif
#if defined(FACTORY_USE_BT)
/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_GetBTMacAddress(unsigned char *pucMacAddr)
{
	DI_ERR_CODE eDiErr;

	if( pucMacAddr == NULL )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_GetBTMacAddress] : Invalid Parameter!! \n"));
		return FDI_ERR;
	}

	eDiErr = DI_NVRAM_Read(DI_NVRAM_FS_PARTITION_RO, (HUINT8 *)BTMAC_ADDR, 0, pucMacAddr, FDI_SYSTEM_DATA_MAC_SIZE);
	if( eDiErr != DI_ERR_OK )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_GetBTMacAddress] : DI_NVRAM_FS_PARTITION_RW Error!! \n"));
		return FDI_ERR;
	}

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
FDI_ERR_CODE FDI_SYSTEM_SetBTMacAddress(unsigned char *pucMacAddr)
{
	DI_ERR_CODE eDiErr;

	if( pucMacAddr == NULL )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_SetBTMacAddress] : Invalid Parameter!! \n"));
		return FDI_ERR;
	}

	eDiErr = DI_NVRAM_Write(DI_NVRAM_FS_PARTITION_RO, (HUINT8 *)BTMAC_ADDR, 0, pucMacAddr, FDI_SYSTEM_DATA_MAC_SIZE);
	if( eDiErr != DI_ERR_OK )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_SetBTMacAddress] : DI_NVRAM_FS_PARTITION_RW Error!! \n"));
		return FDI_ERR;
	}

	return FDI_NO_ERR;
}
#endif

FDI_ERR_CODE FDI_SYSTEM_SetWifiMacAddress(unsigned char *pucMacAddr)
{
	DI_ERR_CODE eDiErr;

	if( pucMacAddr == NULL )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_SetWifiMacAddress] : Invalid Parameter!! \n"));
		return FDI_ERR;
	}

	eDiErr = DI_NVRAM_Write(DI_NVRAM_FS_PARTITION_RO, WIFI_MAC_ADDR, 0, pucMacAddr, WIFI_MAC_SIZE);
	if( eDiErr != DI_ERR_OK )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_SetWifiMacAddress] : DI_NVRAM_WriteError!! \n"));
		return FDI_ERR;
	}

	return FDI_NO_ERR;
}

#if (CABLE_MODEM_ENABLE==YES)
FDI_ERR_CODE FDI_SYSTEM_SetCmMacAddress(unsigned char *pucMacAddr)
{
	DI_ERR_CODE eDiErr;

	if( pucMacAddr == NULL )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_SetCmMacAddress] : Invalid Parameter!! \n"));
		return FDI_ERR;
	}

	eDiErr = DI_NVRAM_SetField(DI_NVRAM_FIELD_CM_MAC_ADDR, 0, pucMacAddr, FDI_SYSTEM_DATA_MAC_SIZE);
	if( eDiErr != DI_ERR_OK )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_GetMacAddress] : DI_FLASH_GetField Error!! \n"));
		return FDI_ERR;
	}

	eDiErr = DI_CM_SetCmMac(pucMacAddr);
	if( eDiErr != DI_ERR_OK )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_SetCmMacAddress] : DI_FLASH_SetField Error!! \n"));
		return FDI_ERR;
	}

	return FDI_NO_ERR;
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
FDI_ERR_CODE FDI_SYSTEM_GetHdcpKey(unsigned char *pucHdcpkey, unsigned long ulSize)
{
	DI_ERR_CODE eDiErr;

	if( pucHdcpkey == NULL )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_GetMacAddress] : Invalid Parameter!! \n"));
		return FDI_ERR;
	}

#if defined(CONFIG_DI10)
	eDiErr = DI_FLASH_GetField(DI_FLASH_FIELD_HDCP_KEY, pucHdcpkey, ulSize);
#endif
#if defined(CONFIG_DI20)
	eDiErr = DI_NVRAM_GetField(DI_NVRAM_FIELD_HDCP_KEY, 0, pucHdcpkey, ulSize);
#endif
	if( eDiErr != DI_ERR_OK )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_GetMacAddress] : DI_FLASH_GetField Error!! \n"));
		return FDI_ERR;
	}
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
FDI_ERR_CODE FDI_SYSTEM_SetHdcpKey(unsigned char *pucHdcpkey, unsigned long ulSize)
{
	DI_ERR_CODE eDiErr;

	if( pucHdcpkey == NULL )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_GetMacAddress] : Invalid Parameter!! \n"));
		return FDI_ERR;
	}

#if defined(CONFIG_DI10)
	eDiErr = DI_FLASH_SetField(DI_FLASH_FIELD_HDCP_KEY, pucHdcpkey, ulSize);
#endif
#if defined(CONFIG_DI20)
	eDiErr = DI_NVRAM_SetField(DI_NVRAM_FIELD_HDCP_KEY, 0, pucHdcpkey, ulSize);
#endif
	if( eDiErr != DI_ERR_OK )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_GetMacAddress] : DI_FLASH_GetField Error!! \n"));
		return FDI_ERR;
	}

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
FDI_ERR_CODE FDI_SYSTEM_GetChipId(unsigned char *pucChipId)
{
#if defined(CONFIG_SUPPORT_CHIPID_64X)
	DI_SYSTEM_GetSystemInfo(DI_SYS_CHIP_ID_X64, pucChipId, 8);
#else
	unsigned long ulChipId;

	DI_SYSTEM_GetSystemInfo(DI_SYS_CHIP_ID, &ulChipId, 0);

	pucChipId[0] = (0xff000000&ulChipId)>>24;
	pucChipId[1] = (0x00ff0000&ulChipId)>>16;
	pucChipId[2] = (0x0000ff00&ulChipId)>>8;
	pucChipId[3] = (0x000000ff&ulChipId);
	FDI_PRINT(0, ("[FDI_SYSTEM_GetChipId] : %X\n", ulChipId));
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
FDI_ERR_CODE FDI_SYSTEM_GetChipIdCheckNumber(unsigned char *pucChipIdChkNo)
{
	unsigned long ulChipIdChkNo;

	DI_SYSTEM_GetSystemInfo(DI_SYS_CHIP_CHECK_NO, &ulChipIdChkNo, 0);

	pucChipIdChkNo[0] = (0xff000000&ulChipIdChkNo)>>24;
	pucChipIdChkNo[1] = (0x00ff0000&ulChipIdChkNo)>>16;
	pucChipIdChkNo[2] = (0x0000ff00&ulChipIdChkNo)>>8;
	pucChipIdChkNo[3] = (0x000000ff&ulChipIdChkNo);

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
FDI_ERR_CODE FDI_SYSTEM_SetChipId(unsigned char *pucChipId)
{
	return FDI_FEATURE_NOT_SUPPORTED;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_GetDtcpKey(unsigned char *pucDtcpkey, unsigned long ulSize)
{
	DI_ERR_CODE eDiErr;

	if( pucDtcpkey == NULL )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_GetMacAddress] : Invalid Parameter!! \n"));
		return FDI_ERR;
	}

#if defined(CONFIG_DI10)
	eDiErr = DI_FLASH_GetField(DI_FLASH_FIELD_DTCP_KEY, pucDtcpkey, ulSize);
#endif
#if defined(CONFIG_DI20)
	eDiErr = DI_NVRAM_GetField(DI_NVRAM_FIELD_DTCP_KEY, 0, pucDtcpkey, ulSize);
#endif
	if( eDiErr != DI_ERR_OK )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_GetMacAddress] : DI_FLASH_GetField Error!! \n"));
		return FDI_ERR;
	}
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
FDI_ERR_CODE FDI_SYSTEM_SetDtcpKey(unsigned char *pucDtcpkey, unsigned long ulSize)
{
	DI_ERR_CODE eDiErr;

	if( pucDtcpkey == NULL )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_SetDtcpKey] : Invalid Parameter!! \n"));
		return FDI_ERR;
	}

#if defined(CONFIG_DI10)
	eDiErr = DI_FLASH_SetField(DI_FLASH_FIELD_DTCP_KEY, pucDtcpkey, ulSize);
#endif
#if defined(CONFIG_DI20)
	eDiErr = DI_NVRAM_SetField(DI_NVRAM_FIELD_DTCP_KEY, 0, pucDtcpkey, ulSize);
#endif
	if( eDiErr != DI_ERR_OK )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_SetDtcpKey] : DI_FLASH_GetField Error!! \n"));
		return FDI_ERR;
	}

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
FDI_ERR_CODE FDI_SYSTEM_SetCiPlusKeyData(unsigned char *pucCiPluskey, unsigned long ulSize)
{
	DI_ERR_CODE eDiErr;

	if( pucCiPluskey == NULL )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_SetCiPlusKeyData] : Invalid Parameter!! \n"));
		return FDI_ERR;
	}

#if defined(CONFIG_DI10)
	eDiErr = DI_FLASH_SetField(DI_FLASH_FIELD_CI_PLUS_KEY, pucCiPluskey, ulSize);
#endif
#if defined(CONFIG_DI20)
	eDiErr = DI_NVRAM_SetField(DI_NVRAM_FIELD_CI_PLUS_KEY, 0, pucCiPluskey, ulSize);
#endif
	if( eDiErr != DI_ERR_OK )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_SetCiPlusKeyData] : DI_FLASH_SetField Error!! \n"));
		return FDI_ERR;
	}

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
FDI_ERR_CODE FDI_SYSTEM_GetCiPlusKeyData(unsigned char *pucCiPluskey, unsigned long ulSize)
{
	DI_ERR_CODE eDiErr;

	if( pucCiPluskey == NULL )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_GetCiPlusKeyData] : Invalid Parameter!! \n"));
		return FDI_ERR;
	}

#if defined(CONFIG_DI10)
	eDiErr = DI_FLASH_GetField(DI_FLASH_FIELD_CI_PLUS_KEY, pucCiPluskey, ulSize);
#endif
#if defined(CONFIG_DI20)
	eDiErr = DI_NVRAM_GetField(DI_NVRAM_FIELD_CI_PLUS_KEY, 0, pucCiPluskey, ulSize);
#endif
	if( eDiErr != DI_ERR_OK )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_GetCiPlusKeyData] : DI_FLASH_GetField Error!! \n"));
		return FDI_ERR;
	}

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
FDI_ERR_CODE FDI_SYSTEM_GetPairKey(unsigned char *pucPairkey, unsigned long ulSize)
{
	DI_ERR_CODE eDiErr;

	if( pucPairkey == NULL )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_GetPairKey] : Invalid Parameter!! \n"));
		return FDI_ERR;
	}

#if defined(CONFIG_DI10)
	eDiErr = DI_FLASH_GetField(DI_FLASH_FIELD_PARING_DATA, pucPairkey, ulSize);
#endif
#if defined(CONFIG_DI20)
#if defined(CONFIG_CAS_NA)
	{
		stCrypt_t stCryptInfo;
		FVK_MEM_Set((void *)&stCryptInfo, 0x00, sizeof(stCrypt_t));

		stCryptInfo.pSrcBuffer = (HUINT8 *)DI_CRYPT_MemAlloc(NAGRA_PK_BLOCK_SIZE);
		if(stCryptInfo.pSrcBuffer == NULL)
		{
			FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_GetPairKey] : Crypt src buffer malloc error!! \n"));
			return FDI_ERR;
		}
		stCryptInfo.pDstBuffer = (HUINT8 *)DI_CRYPT_MemAlloc(NAGRA_PK_BLOCK_SIZE);
		if(stCryptInfo.pDstBuffer == NULL)
		{
			FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_GetPairKey] : Crypt dst buffer malloc error!! \n"));
			DI_CRYPT_MemFree(stCryptInfo.pSrcBuffer);
			return FDI_ERR;
		}

		FVK_MEM_Set((void *)stCryptInfo.pSrcBuffer, 0x00, NAGRA_PK_BLOCK_SIZE);

		eDiErr = DI_NANDFLASH_Read(NAGRA_PK_LOGICAL_OFFSET,stCryptInfo.pSrcBuffer,NAGRA_PK_BLOCK_SIZE);
		if(eDiErr != DI_ERR_OK)
		{
			FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_GetPairKey] : DI_NANDFLASH_Write error!! \n"));
			DI_CRYPT_MemFree(stCryptInfo.pSrcBuffer);
			DI_CRYPT_MemFree(stCryptInfo.pDstBuffer);
			return FDI_ERR;
		}

		stCryptInfo.etCryptKey = DI_CRYPT_INTERNAL_CAS_KEY;
		stCryptInfo.ulBuffSize = NAGRA_PK_BLOCK_SIZE;

		eDiErr = DI_CRYPT_Decrypt(&stCryptInfo);
		if(eDiErr != DI_ERR_OK)
		{
			FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_GetPairKey] : Crypt encryption error!! \n"));
		}
		else
		{
			FVK_MEM_Copy((void *)pucPairkey, (void *)stCryptInfo.pDstBuffer, ulSize);
		}
		DI_CRYPT_MemFree(stCryptInfo.pSrcBuffer);
		DI_CRYPT_MemFree(stCryptInfo.pDstBuffer);
	}
#else
	eDiErr = DI_NVRAM_GetField(DI_NVRAM_FIELD_PARING_DATA, 0, pucPairkey, ulSize);
#endif
#endif
	if( eDiErr != DI_ERR_OK )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_GetPairKey] : DI_FLASH_GetField Error!! \n"));
		return FDI_ERR;
	}

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
FDI_ERR_CODE FDI_SYSTEM_SetPairKey(unsigned char *pucPairkey, unsigned long ulSize)
{
	DI_ERR_CODE eDiErr;

	if( pucPairkey == NULL )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_SetPairKey] : Invalid Parameter!! \n"));
		return FDI_ERR;
	}

#if defined(CONFIG_DI10)
	eDiErr = DI_FLASH_SetField(DI_FLASH_FIELD_PARING_DATA, pucPairkey, ulSize);
#endif
#if defined(CONFIG_DI20)
#if defined(CONFIG_CAS_NA)
	{
		stCrypt_t stCryptInfo;
		FVK_MEM_Set((void *)&stCryptInfo, 0x00, sizeof(stCrypt_t));

		stCryptInfo.pSrcBuffer = (HUINT8 *)DI_CRYPT_MemAlloc(NAGRA_PK_BLOCK_SIZE);
		if(stCryptInfo.pSrcBuffer == NULL)
		{
			FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_SetPairKey] : Crypt src buffer malloc error!! \n"));
			return FDI_ERR;
		}
		stCryptInfo.pDstBuffer = (HUINT8 *)DI_CRYPT_MemAlloc(NAGRA_PK_BLOCK_SIZE);
		if(stCryptInfo.pDstBuffer == NULL)
		{
			FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_SetPairKey] : Crypt dst buffer malloc error!! \n"));
			DI_CRYPT_MemFree(stCryptInfo.pSrcBuffer);
			return FDI_ERR;
		}

		FVK_MEM_Set((void *)stCryptInfo.pSrcBuffer, 0x00, NAGRA_PK_BLOCK_SIZE);
		FVK_MEM_Copy((void *)stCryptInfo.pSrcBuffer, (void *)pucPairkey, ulSize);

		stCryptInfo.etCryptKey = DI_CRYPT_INTERNAL_CAS_KEY;
		stCryptInfo.ulBuffSize = NAGRA_PK_BLOCK_SIZE;

		eDiErr = DI_CRYPT_Encrypt(&stCryptInfo);
		if(eDiErr != DI_ERR_OK)
		{
			FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_SetPairKey] : Crypt encryption error!! \n"));
			DI_CRYPT_MemFree(stCryptInfo.pSrcBuffer);
			DI_CRYPT_MemFree(stCryptInfo.pDstBuffer);
			return FDI_ERR;
		}

		eDiErr = DI_NANDFLASH_Write(NAGRA_PK_LOGICAL_OFFSET,stCryptInfo.pDstBuffer,NAGRA_PK_BLOCK_SIZE);
		if(eDiErr != DI_ERR_OK)
		{
			FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_SetPairKey] : DI_NANDFLASH_Write error!! \n"));
		}

		DI_CRYPT_MemFree(stCryptInfo.pSrcBuffer);
		DI_CRYPT_MemFree(stCryptInfo.pDstBuffer);
	}
#else
	eDiErr = DI_NVRAM_SetField(DI_NVRAM_FIELD_PARING_DATA, 0, pucPairkey, ulSize);
#endif
#endif
	if( eDiErr != DI_ERR_OK )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_SetPairKey] : PK Write Error!! \n"));
		return FDI_ERR;
	}

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
FDI_ERR_CODE FDI_SYSTEM_GetDstbId(unsigned char *pucDstbId, unsigned long ulSize)
{
	DI_ERR_CODE eDiErr;

	if( pucDstbId == NULL )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_GetDstbId] : Invalid Parameter!! \n"));
		return FDI_ERR;
	}

#if defined(CONFIG_DI10)
	eDiErr = DI_FLASH_GetField(DI_FLASH_FIELD_DSTB_ID, pucDstbId, ulSize);
#endif
#if defined(CONFIG_DI20)
	eDiErr = DI_NVRAM_GetField(DI_NVRAM_FIELD_DSTB_ID, 0, pucDstbId, ulSize);
#endif
	if( eDiErr != DI_ERR_OK )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_GetDstbId] : DI_FLASH_GetField Error!! \n"));
		return FDI_ERR;
	}

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
FDI_ERR_CODE FDI_SYSTEM_SetDstbId(unsigned char *pucDstbId, unsigned long ulSize)
{
	DI_ERR_CODE eDiErr;

	if( pucDstbId == NULL )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_SetDstbId] : Invalid Parameter!! \n"));
		return FDI_ERR;
	}

#if defined(CONFIG_DI10)
	eDiErr = DI_FLASH_SetField(DI_FLASH_FIELD_DSTB_ID, pucDstbId, ulSize);
#endif
#if defined(CONFIG_DI20)
	eDiErr = DI_NVRAM_SetField(DI_NVRAM_FIELD_DSTB_ID, 0, pucDstbId, ulSize);
#endif
	if( eDiErr != DI_ERR_OK )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_SetDstbId] : DI_FLASH_GetField Error!! \n"));
		return FDI_ERR;
	}

	return FDI_NO_ERR;
}

/*******************************************************************************
* function : FDI_SYSTEM_CheckFactoryMode
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_CheckFactoryMode(BOOL *pbFlag)
{
	if( pbFlag == NULL )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_GetMacAddress] : Invalid Parameter!! \n"));
		return FDI_ERR;
	}

	*pbFlag = TRUE;

	return FDI_NO_ERR;
}

/*******************************************************************************
* function : FDI_SYSTEM_CheckMesCompleted
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_CheckMesCompleted(BOOL *pbMesCompleted)
{
	if( pbMesCompleted == NULL )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_CheckMesCompleted] : Invalid Parameter!! \n"));
		return FDI_ERR;
	}

	*pbMesCompleted = TRUE;

	return FDI_NO_ERR;
}

/*******************************************************************************
* function : FDI_SYSTEM_DefaultSet
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_EraseFactoryLoader()
{
#if 0
#if 1
	HUINT8 eMode;
	eMode = 0x00;
	DI_EEPROM_SetField(DI_EEPROM_FIELD_USAGE_ID,&eMode,sizeof(HUINT8));
#else

	DI_ERR_CODE eDiErr;
	eDiErr = DI_FACTORY_DefaultSet();
	if( eDiErr != DI_ERR_OK )
	{
		return FDI_ERR;
	}
#endif
#endif
	return FDI_NO_ERR;
}

/*******************************************************************************
* function : FDI_SYSTEM_EnableSecureBoot
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_EnableSecureBoot()
{
	DI_ERR_CODE eDiErr;
	DI_SYS_FUSING_INFO_t tInfo;

	FVK_MEM_Set((void *)&tInfo, 0, sizeof(tInfo));

	tInfo.ucSCS = TRUE;
	tInfo.ulParms[0] = FUSE_BLOCK_SIZE;
	tInfo.ulParms[1] = FUSE_KEY_INDEX;

	eDiErr = DI_SYSTEM_SetSystemInfo(DI_SYS_FUSING_INFO, &tInfo, sizeof(DI_SYS_FUSING_INFO_t));

	return eDiErr;
}

/*******************************************************************************
* function : FDI_SYSTEM_EnableJtag
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_EnableJtag()
{
	DI_ERR_CODE eDiErr;
	DI_SYS_FUSING_INFO_t tInfo;

	FVK_MEM_Set((void *)&tInfo, 0, sizeof(tInfo));

	tInfo.ucJTAG = TRUE;
	eDiErr = DI_SYSTEM_SetSystemInfo(DI_SYS_FUSING_INFO, &tInfo, sizeof(DI_SYS_FUSING_INFO_t));

	return eDiErr;
}

/*******************************************************************************
* function : FDI_SYSTEM_EnableCwMode
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_EnableCwMode()
{
	DI_ERR_CODE eDiErr;
	DI_SYS_FUSING_INFO_t tInfo;

	FVK_MEM_Set((void *)&tInfo, 0, sizeof(tInfo));

	tInfo.ucCWE = TRUE;
	eDiErr = DI_SYSTEM_SetSystemInfo(DI_SYS_FUSING_INFO, &tInfo, sizeof(DI_SYS_FUSING_INFO_t));

	return eDiErr;
}

/*******************************************************************************
* function : FDI_SYSTEM_EnableOTPLock
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_EnableOTPLock()
{
	DI_ERR_CODE eDiErr;
	DI_SYS_FUSING_INFO_t tInfo;

	FVK_MEM_Set((void *)&tInfo, 0, sizeof(tInfo));

	tInfo.ucOTP = TRUE;
	eDiErr = DI_SYSTEM_SetSystemInfo(DI_SYS_FUSING_INFO, &tInfo, sizeof(DI_SYS_FUSING_INFO_t));

	return eDiErr;
}

/*******************************************************************************
* function : FDI_SYSTEM_EnableOTPChipset
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_EnableOTPChipset()
{
	DI_ERR_CODE eDiErr;
	DI_SYS_FUSING_INFO_t tInfo;

#if (PRODUCT_YSR2000==YES)
	eDiErr=DI_ERR_ERROR; /* add default error */
#else
	FVK_MEM_Set((void *)&tInfo, 0, sizeof(tInfo));

	tInfo.ucOTPChipset = TRUE;
	eDiErr = DI_SYSTEM_SetSystemInfo(DI_SYS_FUSING_INFO, &tInfo, sizeof(DI_SYS_FUSING_INFO_t));
#endif
	return eDiErr;
}


/*******************************************************************************
* function : FDI_SYSTEM_EnableOTPSectionData
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_EnableOTPDataSection( unsigned char *pucSectionData )
{
	DI_ERR_CODE eDiErr = 0;

	DI_SYS_OTP_DATA_SECTION_INFO_t	otpDataSectionInfo;

	if(pucSectionData == NULL)
		return FDI_ERR_INVALID_PARAMETER;

	FVK_MEM_Set((void *)&otpDataSectionInfo, 0, sizeof(otpDataSectionInfo));
	otpDataSectionInfo.ucIndex = 0;
	FVK_MEM_Copy((void *)&otpDataSectionInfo.ucDataSection[0], (void *)&pucSectionData[0], DI_SYS_OTP_DATA_SECTION_LENGTH);


	eDiErr = DI_SYSTEM_SetSystemInfo(DI_SYS_FUSE_OTP_DATA_SECTION, &otpDataSectionInfo, sizeof(DI_SYS_OTP_DATA_SECTION_INFO_t));
	if (eDiErr != DI_ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("DI_SYSTEM_SetSystemInfo(DI_SYS_FUSE_OTP_DATA_SECTION) error\n"));

		return FDI_ERR;
	}
	else
	{
		FDI_PRINT(FDI_PRT_ERROR, ("DI_SYSTEM_SetSystemInfo(DI_SYS_FUSE_OTP_DATA_SECTION) OK\n"));
	}

	return FDI_NO_ERR;

}

/*******************************************************************************
* function : FDI_SYSTEM_FusingDSC
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_FusingDSC(void)
{
	DI_ERR_CODE eDiErr = 0;
	unsigned long FusingInfo = 0;

	eDiErr = DI_SYSTEM_SetSystemInfo(DI_SYS_FUSE_DSC, &FusingInfo, 0);
	if (eDiErr != DI_ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[%s,%d] ERROR\n", __FUNCTION__, __LINE__));
		return FDI_ERR;
	}

	return FDI_NO_ERR;
}

/*******************************************************************************
* function : FDI_SYSTEM_SetSwReset
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_SetSwReset(void)
{
	DI_ERR_CODE eDiErr;

	eDiErr = DI_POWER_SetMainChipReset();
	if( eDiErr != DI_ERR_OK )
	{
		return FDI_ERR;
	}
	return FDI_NO_ERR;
}

/*******************************************************************************
* function : FDI_SYSTEM_SetStanby
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_SetStandby(void)
{
	DI_ERR_CODE eDiErr;

	eDiErr = DI_POWER_SetPowerOff(DI_POWER_OFFMODE_STANDBY_NORMAL);
	if( eDiErr != DI_ERR_OK )
	{
		return FDI_ERR;
	}
	return FDI_NO_ERR;
}



/*******************************************************************************
* function : FDI_SYSTEM_SetSwReset
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_GetCASerialNumber(unsigned char *pucSn)
{
	DI_ERR_CODE eDiErr;
	unsigned char aucPairkey[FDI_SYSTEM_DATA_PAIRKEY_SIZE];
	unsigned long CheckSum = 0;
	unsigned long CaSnNum = 0;
	unsigned char *DownBuffer;

#if defined(CONFIG_DI10)
	eDiErr = DI_FLASH_GetField(DI_FLASH_FIELD_PARING_DATA, aucPairkey, FDI_SYSTEM_DATA_DTCPKEY_SIZE);
#endif
#if defined(CONFIG_DI20)
#if defined(CONFIG_CAS_NA)
	eDiErr = DI_SYSTEM_GetSystemInfo(DI_SYS_STB_CA_SN, &CaSnNum, 0);
#else
	eDiErr = DI_NVRAM_GetField(DI_NVRAM_FIELD_PARING_DATA, 0, aucPairkey, FDI_SYSTEM_DATA_DTCPKEY_SIZE);
#endif
#endif
	if( eDiErr != DI_ERR_OK )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_GetCiPlusKeyData] : DI_FLASH_GetField Error!! \n"));
		return FDI_ERR;
	}

#if defined(CONFIG_DI20) && defined(CONFIG_CAS_NA)
#else
	DownBuffer = (unsigned char *)(aucPairkey+4);

	CaSnNum = DownBuffer[0] << 24 | DownBuffer[1] << 16 | DownBuffer[2] << 8 | DownBuffer[3];
#endif
	CheckSum = ((CaSnNum/100)%23 + (CaSnNum%100))%100;

	sprintf(pucSn, "%10d %d", CaSnNum, CheckSum);

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
FDI_ERR_CODE FDI_SYSTEM_GetVAExtraData(unsigned char *pucVAExtraData)
{
	DI_ERR_CODE eDiErr;
	int i;

#if defined(CONFIG_DI10)
	if( pucVAExtraData == NULL )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_GetVAExtraData] : Invalid Parameter!! \n"));
		return FDI_ERR;
	}

    eDiErr = DI_EEPROM_Read(0x6000, pucVAExtraData, FDI_SYSTEM_DATA_VAEXTRADATA_SIZE);
	if( eDiErr != DI_ERR_OK )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_GetVAExtraData] : DI_EEPROM_Read Error!! \n"));
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
FDI_ERR_CODE FDI_SYSTEM_SetVAExtraData(unsigned char *pucVAExtraData)
{
	DI_ERR_CODE eDiErr;

#if defined(CONFIG_DI10)
	if( pucVAExtraData == NULL )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_SetVAExtraData] : Invalid Parameter!! \n"));
		return FDI_ERR;
	}

    eDiErr = DI_EEPROM_Write(0x6000, pucVAExtraData, FDI_SYSTEM_DATA_VAEXTRADATA_SIZE);
	if( eDiErr != DI_ERR_OK )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_SetVAExtraData] : DI_EEPROM_Write Error!! \n"));
		return FDI_ERR;
	}
#endif

	return FDI_NO_ERR;
}

/*******************************************************************************
* function :	FDI_SYSTEM_GetSTBOwnerId
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_GetSTBOwnerId(unsigned short *pusSTBOwnerId)
{
	DI_ERR_CODE eDiErr;
	DI_SYS_FUSING_INFO_t fusingInfo;

	eDiErr = DI_SYSTEM_GetSystemInfo(DI_SYS_FUSING_INFO, &fusingInfo, sizeof(DI_SYS_FUSING_INFO_t));
	if (eDiErr != DI_ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("DI_SYSTEM_GetSystemInfo(DI_SYS_FUSING_INFO, STBOwnerID) error(0x%x)\n", eDiErr));

		return FDI_ERR;
	}
	else
	{
		*pusSTBOwnerId = fusingInfo.usSTBOwnerID;
		FDI_PRINT(FDI_PRT_ERROR, ("DI_SYSTEM_GetSystemInfo(DI_SYS_FUSING_INFO, STBOwnerID(0x%04x)) OK\n", *pusSTBOwnerId));
		return FDI_NO_ERR;
	}
}

/*******************************************************************************
* function :	FDI_SYSTEM_SetSTBOwnerId
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_SetSTBOwnerId(unsigned short usSTBOwnerId)
{
	DI_ERR_CODE eDiErr;
	DI_SYS_FUSING_INFO_t fusingInfo;

	FVK_MEM_Set((void *)&fusingInfo, 0, sizeof(fusingInfo));

	fusingInfo.usSTBOwnerID = usSTBOwnerId;

	eDiErr = DI_SYSTEM_SetSystemInfo(DI_SYS_FUSING_INFO, &fusingInfo, sizeof(DI_SYS_FUSING_INFO_t));
	if (eDiErr != DI_ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("DI_SYSTEM_SetSystemInfo(DI_SYS_FUSING_INFO, STBOwnerID) error(0x%x)\n", eDiErr));

		return FDI_ERR;
	}
	else
	{
		FDI_PRINT(FDI_PRT_ERROR, ("DI_SYSTEM_SetSystemInfo(DI_SYS_FUSING_INFO, STBOwnerID(0x%04x)) OK\n", fusingInfo.usSTBOwnerID));
		return FDI_NO_ERR;
	}

}



/*******************************************************************************
* function : FDI_SYSTEM_SetCSCKey
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static unsigned short intrnl_fdi_system_crc16_verctor[16] =
{
    0x0000, 0x1021, 0x2042, 0x3063,
    0x4084, 0x50A5, 0x60C6, 0x70E7,
    0x8108, 0x9129, 0xA14A, 0xB16B,
    0xC18C, 0xD1AD, 0xE1CE, 0xF1EF
};
static unsigned short INTRNL_FDI_SYSTEM_GetCrc16Ex(unsigned char *pvBuf, unsigned short n)
{
    unsigned short crc_v = 0x0000;
    int length = n;

    for ( ; length ; length--)
    {
        crc_v = (crc_v << 4 ) ^ intrnl_fdi_system_crc16_verctor[ (( *(((unsigned char*)& crc_v )+1)^ *pvBuf ) >> 4 )];
        crc_v = (crc_v << 4 ) ^ intrnl_fdi_system_crc16_verctor[ ( *(((unsigned char*)& crc_v )+1) >> 4 )^ (*pvBuf++ & 0x0F )];
    }
	return crc_v ;
}
static unsigned int INTRNL_FDI_SYSTEM_GetCSCCheckNumber(unsigned char *pBuffer)
{
#if defined(CONFIG_CAS)
    unsigned int checkNumber = 0x00000000;
    DRV_CSDGetCSCCheckNumber(&checkNumber, pBuffer);
    return checkNumber;
#else
	return 0;
#endif
}
#if 0 /*CSC Key Sample */
unsigned char temp_key_value[151] =
{
    0x00, 0x00, 0x00, 0x97, /* Length */
    0x89, 0x31, 0x4A, 0x5B, /* NUID */
    0x00, 0x01, /* Version */
    0x04, 0x01, /* Provider Id */
    0x00, /* Chipset CW Secure architecture */
    0x00, /* Offset */
    0x01, /* Tag */
    0x81, /* Length */

    0x10, /* Key Size */
    0x60, 0xBB, 0x4C, 0xC4, 0x41, 0x1F, 0x5D, 0xD1, /* Key */
    0x6B, 0x35, 0xAA, 0x9A, 0x28, 0x7C, 0x30, 0x56,
    0xC9, 0x7D, 0xEB, 0xE7, 0x26, 0xEE, 0xE4, 0x84,
    0xF1, 0xB1, 0xD6, 0x11, 0x6C, 0xB5, 0xCE, 0x63,
    0x9F, 0xC2, 0x3C, 0x2D, 0xDC, 0x00, 0x51, 0xD5,
    0x06, 0x88, 0x27, 0xE2, 0x50, 0x66, 0x9E, 0xDE,
    0xE8, 0x97, 0x17, 0x85, 0xC8, 0x73, 0xF9, 0x66,
    0xFD, 0xF3, 0x84, 0xCF, 0x6F, 0x2E, 0xA9, 0x9C,
    0xFA, 0x11, 0x6E, 0x0F, 0xE3, 0xA0, 0x12, 0xA2,
    0xDF, 0xC8, 0xB9, 0x19, 0xAC, 0xFF, 0x8B, 0xBD,
    0x7C, 0xD8, 0x03, 0x4F, 0x73, 0xDA, 0xED, 0xB6,
    0xD4, 0x14, 0xF7, 0xA4, 0x37, 0xAF, 0x7D, 0x66,
    0xB1, 0x77, 0x33, 0xEE, 0xE3, 0xAE, 0x31, 0x80,
    0x94, 0xC6, 0x48, 0x4C, 0x69, 0xFA, 0x9E, 0x06,
    0x62, 0x2E, 0x9A, 0x9D, 0x22, 0xF9, 0xAC, 0xD1,
    0x0D, 0xDF, 0xDE, 0x70, 0x91, 0xAD, 0x08, 0x6B,

    0xAC, 0xDD, 0xFD, 0xCC, /* Check Number */
    0xE2, 0xCB /* CRC */
};
#endif
static FDI_ERR_CODE INTRNL_FDI_SYSTEM_GetValidityCSCKeyData(unsigned char *pCSCData, unsigned int csclength, unsigned int *pValidity)
{
	unsigned int chipIdInChip = 0x00000000;
    unsigned int chipIdInCSC = 0x00000000;
    unsigned int tempInt = 0x00000000;
    unsigned int nagra_chipid_length = 4;
    unsigned int diff_count = 0;
    unsigned int iIndex = 0;
    unsigned int record_length_field = 0;
    unsigned int provider_id = 0x00000000;
    unsigned int crcInCSC = 0x00000000;
    unsigned int crcInCalc = 0x00000000;
    unsigned int cscCheckNumberInCalc = 0x00000000;
    unsigned int cscCheckNumberInCSC = 0x00000000;
    unsigned char *pucCSCBuf = NULL;
	FDI_ERR_CODE eFDiErr = FDI_ERR;
    /********************/
    /* Check the length */
    /********************/

    if ( (pCSCData == NULL) || (csclength == 0) ||( pValidity == NULL ))
    {
        FDI_PRINT(FDI_PRT_ERROR, ("[INTRNL_FDI_SYSTEM_GetValidityCSCKeyData] : Parameter Error\n"));
        return FDI_ERR;
    }

    *pValidity = 0;

    /*record length should be same as to csc length*/
    tempInt = pCSCData[0];
    record_length_field = record_length_field | (tempInt << 24 );
    tempInt = pCSCData[1];
    record_length_field = record_length_field | (tempInt << 16 );
    tempInt = pCSCData[2];
    record_length_field = record_length_field | (tempInt << 8 );
    tempInt = pCSCData[3];
    record_length_field = record_length_field | (tempInt );

    FDI_PRINT(FDI_PRT_ERROR, ("[INTRNL_FDI_SYSTEM_GetValidityCSCKeyData] : RecordLength : %d\n", record_length_field));
    FDI_PRINT(FDI_PRT_ERROR, ("[INTRNL_FDI_SYSTEM_GetValidityCSCKeyData] : CSCLength : %d\n", csclength));

    if ( record_length_field != csclength )
    {
		FDI_PRINT(FDI_PRT_ERROR, ("[INTRNL_FDI_SYSTEM_GetValidityCSCKeyData] : record length fail \n"));
        FDI_PRINT(FDI_PRT_ERROR, ("[INTRNL_FDI_SYSTEM_GetValidityCSCKeyData] : record length %d, csclength %d \n", record_length_field, csclength));
		return FDI_ERR;
    }
#if 0	/* Ignore the check with length field */
#if defined(CONFIG_CAS_NA_NOCS_3X)
#else
    /* length field should be less 22 bytes than record length field */
    tempInt = pCSCData[15];
    tempInt = tempInt + 22;
    /* Totally 22 bytes
       Record Length: 4 bytes
       NUID : 4bytes
       Version : 2 bytes
       ProvID : 2 bytes
       ChipSet CW Secure Architecture : 1 byte
       Offset : 1 byte
       Tag : 1 byte
       Length : 1 byte
       CSC Data Check Check Number : 4 bytes
       CRC Value : 2 bytes */
    FDI_PRINT(FDI_PRT_ERROR, ("[INTRNL_FDI_SYSTEM_GetValidityCSCKeyData] : RecordLength : %d\n", record_length_field));
    FDI_PRINT(FDI_PRT_ERROR, ("[INTRNL_FDI_SYSTEM_GetValidityCSCKeyData] : length : %d\n", (tempInt-22)));

    if ( tempInt != record_length_field )
    {
		FDI_PRINT(FDI_PRT_ERROR, ("[INTRNL_FDI_SYSTEM_GetValidityCSCKeyData] : length fail \n"));
		return FDI_ERR;
    }
#endif
#endif
    /****************/
    /* CRC 16 Check */
    /****************/
    pucCSCBuf = (char *) FVK_MEM_Alloc(csclength);
    if ( pucCSCBuf == NULL )
    {
		FDI_PRINT(FDI_PRT_ERROR, ("[INTRNL_FDI_SYSTEM_GetValidityCSCKeyData] : mem alloc fail \n"));
		return FDI_ERR;
    }
    FVK_MEM_Copy(pucCSCBuf, pCSCData, csclength);
    crcInCalc = INTRNL_FDI_SYSTEM_GetCrc16Ex(pucCSCBuf,record_length_field-2);
    tempInt = pCSCData[record_length_field-2];
    crcInCSC = crcInCSC | (tempInt << 8 );
    tempInt = pCSCData[record_length_field-1];
    crcInCSC = crcInCSC | (tempInt);
    if ( pucCSCBuf != NULL )
    {
        FVK_MEM_Free(pucCSCBuf);
        pucCSCBuf = NULL;
    }

    FDI_PRINT(FDI_PRT_ERROR, ("[INTRNL_FDI_SYSTEM_GetValidityCSCKeyData] : CRC in CSC : %d\n", crcInCSC));
    FDI_PRINT(FDI_PRT_ERROR, ("[INTRNL_FDI_SYSTEM_GetValidityCSCKeyData] : CRC in Calc : %d\n", crcInCalc));

    if ( crcInCalc != crcInCSC )
    {
		FDI_PRINT(FDI_PRT_ERROR, ("[INTRNL_FDI_SYSTEM_GetValidityCSCKeyData] : crc different \n"));
		return FDI_ERR;
    }


    /**************/
    /* NUID Check */
    /**************/
	if ( DI_SYSTEM_GetSystemInfo(DI_SYS_CHIP_ID, &chipIdInChip, 0) != DI_ERR_OK )
    {
		FDI_PRINT(FDI_PRT_ERROR, ("[INTRNL_FDI_SYSTEM_GetValidityCSCKeyData] : chip id get fail \n"));
		return FDI_ERR;
    }
    tempInt = pCSCData[4];
    chipIdInCSC = chipIdInCSC | ( tempInt << 24 );
    tempInt = pCSCData[5];
    chipIdInCSC = chipIdInCSC | ( tempInt << 16 );
    tempInt = pCSCData[6];
    chipIdInCSC = chipIdInCSC | ( tempInt << 8 );
    tempInt = pCSCData[7];
    chipIdInCSC = chipIdInCSC | ( tempInt );

    FDI_PRINT(FDI_PRT_ERROR, ("[INTRNL_FDI_SYSTEM_GetValidityCSCKeyData] : ChipID in CSC : %08x\n", chipIdInCSC));
    FDI_PRINT(FDI_PRT_ERROR, ("[INTRNL_FDI_SYSTEM_GetValidityCSCKeyData] : ChipID in Chip : %08x\n", chipIdInChip));

    if ( chipIdInCSC != chipIdInChip )
    {
		FDI_PRINT(FDI_PRT_ERROR, ("[INTRNL_FDI_SYSTEM_GetValidityCSCKeyData] : chip is different \n"));
		return FDI_ERR;
    }


    /********************/
    /* ProviderID Check */
    /********************/
    tempInt = pCSCData[10];
    provider_id = provider_id | ( tempInt << 8 );
    tempInt = pCSCData[11];
    provider_id = provider_id | ( tempInt );

    FDI_PRINT(FDI_PRT_ERROR, ("[INTRNL_FDI_SYSTEM_GetValidityCSCKeyData] : ProviderID in CSC : %08x\n", provider_id));
    FDI_PRINT(FDI_PRT_ERROR, ("[INTRNL_FDI_SYSTEM_GetValidityCSCKeyData] : ProviderID in Def : %08x\n", NAGRA_CSC_PROVIDER_ID));

    if ( provider_id != NAGRA_CSC_PROVIDER_ID )
    {
		FDI_PRINT(FDI_PRT_ERROR, ("[INTRNL_FDI_SYSTEM_GetValidityCSCKeyData] : Provider Id is Different \n"));
		return FDI_ERR;
    }

    /************************************/
    /* CSC Data Check Number Validation */
    /************************************/
    pucCSCBuf = FVK_MEM_Alloc(16);
    if ( pucCSCBuf == NULL )
    {
		FDI_PRINT(FDI_PRT_ERROR, ("[INTRNL_FDI_SYSTEM_GetValidityCSCKeyData] : Mem Alooc Fail 2 \n"));
		return FDI_ERR;
    }

    FVK_MEM_Set(pucCSCBuf, 0x00, 16);
    FVK_MEM_Copy(pucCSCBuf, &pCSCData[0], 16); /* Check Number should be calculated by 16 crc data bytes from 0 */
    cscCheckNumberInCalc = INTRNL_FDI_SYSTEM_GetCSCCheckNumber(pucCSCBuf);
    tempInt = pCSCData[record_length_field-6];
    cscCheckNumberInCSC = cscCheckNumberInCSC | (tempInt << 24);
    tempInt = pCSCData[record_length_field-5];
    cscCheckNumberInCSC = cscCheckNumberInCSC | (tempInt << 16);
    tempInt = pCSCData[record_length_field-4];
    cscCheckNumberInCSC = cscCheckNumberInCSC | (tempInt << 8);
    tempInt = pCSCData[record_length_field-3];
    cscCheckNumberInCSC = cscCheckNumberInCSC | tempInt ;

    if ( pucCSCBuf != NULL )
    {
        FVK_MEM_Free(pucCSCBuf);
        pucCSCBuf = NULL;
    }

    FDI_PRINT(FDI_PRT_ERROR, ("[INTRNL_FDI_SYSTEM_GetValidityCSCKeyData] : CheckNumber in CSC : %08x\n", cscCheckNumberInCSC));
    FDI_PRINT(FDI_PRT_ERROR, ("[INTRNL_FDI_SYSTEM_GetValidityCSCKeyData] : CheckNumber in Def : %08x\n", cscCheckNumberInCalc));

    if ( cscCheckNumberInCalc != cscCheckNumberInCSC )
    {
		FDI_PRINT(FDI_PRT_ERROR, ("[INTRNL_FDI_SYSTEM_GetValidityCSCKeyData] : CheckNumber Fail \n"));
		return FDI_ERR;
    }
    *pValidity = 1;
    return FDI_NO_ERR;
}

FDI_ERR_CODE FDI_SYSTEM_SetCSCKey(unsigned char *pucCSC, unsigned int csclength)
{
    unsigned int diff_count = 0;
    unsigned int tempInt = 0;
    unsigned char *pucCSCBuf = NULL;
	DI_ERR_CODE eDiErr = FDI_ERR;
    unsigned int iValidity = 0;

    if ( pucCSC == NULL || csclength == 0 )
    {
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_SetCSCKey] : Param Error \n"));
		return FDI_ERR;
    }

    if ( INTRNL_FDI_SYSTEM_GetValidityCSCKeyData(pucCSC, csclength, &iValidity) != FDI_NO_ERR )
    {
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_SetCSCKey] : Validation Fail \n"));
		return FDI_ERR;
    }

    if ( iValidity != 1 )
    {
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_SetCSCKey] : Validation Fail 2 \n"));
		return FDI_ERR;
    }
    /***********************/
    /* Write Down to FLASH */
    /***********************/
#if defined(CONFIG_DI10)
    eDiErr = DI_FLASH_SetField(DI_FLASH_FIELD_CSC_KEY, pucCSC, csclength);
#endif
#if defined(CONFIG_DI20)
#if defined(CONFIG_CAS_NA)
	eDiErr = DI_NANDFLASH_Write(NAGRA_CSC_LOGICAL_OFFSET, pucCSC, csclength);
#else
	eDiErr = DI_NVRAM_SetField(DI_NVRAM_FIELD_CSC_KEY, 0, pucCSC, csclength);
#endif
#endif
    if ( eDiErr != DI_ERR_OK )
    {
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_SetCSCKey] : Flash Write Fail \n"));
		return FDI_ERR;
    }
    /***********************/
    /* Read And Compare    */
    /***********************/
    pucCSCBuf = (char *) FVK_MEM_Alloc(csclength);
    if ( pucCSCBuf == NULL )
    {
    	FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_SetCSCKey] : Alloc2 Fail \n"));
		return FDI_ERR;
    }

#if defined(CONFIG_DI10)
    eDiErr = DI_FLASH_GetField(DI_FLASH_FIELD_CSC_KEY, pucCSCBuf, csclength);
#endif
#if defined(CONFIG_DI20)
#if defined(CONFIG_CAS_NA)
	eDiErr = DI_NANDFLASH_Read(NAGRA_CSC_LOGICAL_OFFSET, pucCSCBuf, csclength);
#else
	eDiErr = DI_NVRAM_GetField(DI_NVRAM_FIELD_CSC_KEY, 0, pucCSCBuf, csclength);
#endif
#endif
    if ( eDiErr != DI_ERR_OK )
    {
    	FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_SetCSCKey] : Flash Read Fail\n"));
        FVK_MEM_Free(pucCSCBuf);
		return FDI_ERR;
    }

    diff_count = 0;
    for ( tempInt = 0 ; tempInt < csclength ; tempInt++ )
    {
        if ( pucCSCBuf[tempInt] != pucCSC[tempInt] )
        {
            diff_count++;
        }
    }

    if ( diff_count > 0 )
    {
    	FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_SetCSCKey] : Compare Fail\n"));
        FVK_MEM_Free(pucCSCBuf);
		return FDI_ERR;
    }
    FVK_MEM_Free(pucCSCBuf);
	return FDI_NO_ERR;
}

/* FDI_SYSTEM_GetCSCValidity need 11 bytes:
   aucBuf[0] = validity : 0x00 - Fail 0x77 - Valide
   aucBuf[1] - aucBuf[4] : nuid in CSC
   aucBuf[5] - aucBuf[6] : CRC in CSC
   aucBuf[7] - aucBuf[10] : CheckNum in CSC */
FDI_ERR_CODE FDI_SYSTEM_GetCSCValidity(unsigned char *pValidationInfo)
{
    unsigned char *pucCSCBuf = NULL;
  	DI_ERR_CODE eDiErr = FDI_ERR;

#if defined(RESERVED_CSC_KEY_SIZE)
    unsigned int csclength = RESERVED_CSC_KEY_SIZE;
#else
    unsigned int csclength = 1024;
#endif
    unsigned int tempInt = 0x00000000;
    unsigned int diff_count = 0;
    unsigned int iIndex = 0;
    unsigned int record_length_field = 0;
    unsigned int iValidity = 0;

    pValidationInfo[0] = 0x00;

    /* Read Written CSC Data */
    pucCSCBuf = (char *) FVK_MEM_Alloc(csclength); /* Max CSC Key length */
    if ( pucCSCBuf == NULL )
    {
    	FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_SetCSCKey] : Alloc2 Fail \n"));
		return FDI_ERR;
    }

#if defined(CONFIG_DI10)
    eDiErr = DI_FLASH_GetField(DI_FLASH_FIELD_CSC_KEY, pucCSCBuf, csclength);
#endif
#if defined(CONFIG_DI20)
#if defined(CONFIG_CAS_NA)
	eDiErr = DI_NANDFLASH_Read(NAGRA_CSC_LOGICAL_OFFSET, pucCSCBuf, csclength);
#else
	eDiErr = DI_NVRAM_GetField(DI_NVRAM_FIELD_CSC_KEY, 0, pucCSCBuf, csclength);
#endif
#endif
    if ( eDiErr != DI_ERR_OK )
    {
    	FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_SetCSCKey] : Flash Read Fail\n"));
        FVK_MEM_Free(pucCSCBuf);
		return FDI_ERR;
    }

    tempInt = pucCSCBuf[0];
    record_length_field = record_length_field | (tempInt << 24 );
    tempInt = pucCSCBuf[1];
    record_length_field = record_length_field | (tempInt << 16 );
    tempInt = pucCSCBuf[2];
    record_length_field = record_length_field | (tempInt << 8 );
    tempInt = pucCSCBuf[3];
    record_length_field = record_length_field | (tempInt );

    if ( INTRNL_FDI_SYSTEM_GetValidityCSCKeyData(pucCSCBuf, record_length_field, &iValidity) != FDI_NO_ERR )
    {
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_SetCSCKey] : Validation Fail \n"));
        FVK_MEM_Free(pucCSCBuf);
		return FDI_ERR;
    }

    pValidationInfo[0] = iValidity;
    if ( iValidity == 0 )
    {
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_SetCSCKey] : Validation Fail \n"));
        FVK_MEM_Free(pucCSCBuf);
		return FDI_ERR;
    }

    pValidationInfo[5] = pucCSCBuf[record_length_field-2];
    pValidationInfo[6] = pucCSCBuf[record_length_field-1];
    pValidationInfo[1] = pucCSCBuf[4];
    pValidationInfo[2] = pucCSCBuf[5];
    pValidationInfo[3] = pucCSCBuf[6];
    pValidationInfo[4] = pucCSCBuf[7];
    pValidationInfo[11] = pucCSCBuf[10];
    pValidationInfo[12] = pucCSCBuf[11];
    pValidationInfo[7] = pucCSCBuf[record_length_field-6];
    pValidationInfo[8] = pucCSCBuf[record_length_field-5];
    pValidationInfo[9] = pucCSCBuf[record_length_field-4];
    pValidationInfo[10] = pucCSCBuf[record_length_field-3];

    FVK_MEM_Free(pucCSCBuf);

    return FDI_NO_ERR;
}

/*******************************************************************************
* function : FDI_SYSTEM_EncryptCCB
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_EncryptCCB(BOOL* bSuccess)
{
#if (DEFAULT_SETTING_LIST&DEFAULT_CCB_ENCRYPT)
	unsigned char* pBuffer=NULL;
	unsigned char* pCPCB=NULL;
	unsigned char* pOriginalData=NULL;
	unsigned int uiStartAddr=0;
	unsigned int uiCCBStartAddr=0;
	unsigned int uiBlockSize=0;
	unsigned int uiBlockNum=0;
	unsigned int uiTargetBlock=0;
	int iResult;
	DI_ERR_CODE dResult;
	stCrypt_t stCrytInfo;
	FDI_ERR_CODE fResult=FDI_NO_ERR;

	if(bSuccess == NULL)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("parameter pointer is null!!\n"));
		return FDI_ERR;
	}

	VK_MEM_Memset(&stCrytInfo, 0, sizeof(stCrypt_t));

/*************************
*	0. memory allocation
*************************/
	DRV_FLASH_GetCapabilities(&uiBlockSize, &uiBlockNum);
	uiTargetBlock = IR_FLASH_CPCB_OFFSET/uiBlockSize;

	stCrytInfo.pSrcBuffer = DI_CRYPT_MemAlloc(CCB_Crc32 - CCB_Part_Crc_Info_Start);
	if(stCrytInfo.pSrcBuffer == NULL)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("Out of Memory!\n"));
		return FDI_ERR;
	}

	stCrytInfo.pDstBuffer = DI_CRYPT_MemAlloc(CCB_Crc32 - CCB_Part_Crc_Info_Start);
	if(stCrytInfo.pDstBuffer == NULL)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("Out of Memory!\n"));
		DI_CRYPT_MemFree(stCrytInfo.pSrcBuffer);
		return FDI_ERR;
	}

	pBuffer = FVK_MEM_Alloc(uiBlockSize);
	if(pBuffer == NULL)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[INTRN_FDI_SYSTEM_CCBCheck]: Out of Memory!\n"));
		DI_CRYPT_MemFree(stCrytInfo.pSrcBuffer);
		DI_CRYPT_MemFree(stCrytInfo.pDstBuffer);
		return FDI_ERR;
	}

	pCPCB = FVK_MEM_Alloc(IR_CPCB_SIZE);
	if(pCPCB == NULL)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[INTRN_FDI_SYSTEM_CCBCheck]: Out of Memory!\n"));
		FVK_MEM_Free(pBuffer);
		DI_CRYPT_MemFree(stCrytInfo.pSrcBuffer);
		DI_CRYPT_MemFree(stCrytInfo.pDstBuffer);
		return FDI_ERR;
	}

	pOriginalData = FVK_MEM_Alloc(IR_CPCB_SIZE);
	if(pOriginalData == NULL)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[INTRN_FDI_SYSTEM_CCBCheck]: Out of Memory!\n"));
		FVK_MEM_Free(pBuffer);
		FVK_MEM_Free(pCPCB);
		DI_CRYPT_MemFree(stCrytInfo.pSrcBuffer);
		DI_CRYPT_MemFree(stCrytInfo.pDstBuffer);
		return FDI_ERR;
	}

/*************************
*	1. CCB Read
*************************/
	uiStartAddr = uiBlockSize*uiTargetBlock;
	uiCCBStartAddr = IR_FLASH_CPCB_OFFSET-uiStartAddr;

	dResult = DI_FLASH_Read(uiStartAddr, pBuffer, uiBlockSize);
	if(dResult != DI_ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("DI_FLASH_Read failed!\n"));
		fResult = FDI_ERR;
		goto done;
	}

	VK_MEM_Memcpy(pCPCB, pBuffer+uiCCBStartAddr, IR_CPCB_SIZE);
	VK_MEM_Memcpy(pOriginalData, pCPCB, IR_CPCB_SIZE);
	VK_MEM_Memcpy(stCrytInfo.pSrcBuffer, pCPCB+CCB_Part_Crc_Info_Start, CCB_Crc32-CCB_Part_Crc_Info_Start);

/*************************
*	2. CCB Encryption
*************************/
	stCrytInfo.etCryptMode = DI_CRYPT_TDES;
	stCrytInfo.ulBuffSize = CCB_Crc32-CCB_Part_Crc_Info_Start;
	stCrytInfo.etCryptKey = DI_CRYPT_DEFAULT_SYSTEM_KEY;
	stCrytInfo.pKey = NULL;

	dResult = DI_CRYPT_Encrypt(&stCrytInfo);
	if(dResult != DI_ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("DI_CRYPT_Encrypt failed!\n"));
		fResult = FDI_ERR;
		goto done;
	}

	VK_MEM_Memcpy(pCPCB+CCB_Part_Crc_Info_Start, stCrytInfo.pDstBuffer, CCB_Crc32-CCB_Part_Crc_Info_Start);
	VK_MEM_Memcpy(pBuffer+uiCCBStartAddr, pCPCB, IR_CPCB_SIZE);

/*************************
*	3. Erase & Write to the flash
*************************/
	dResult = DI_FLASH_EraseBlock(uiTargetBlock);
	if(dResult != DI_ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("DI_FLASH_EraseBlock failed!\n"));
		fResult = FDI_ERR;
		goto done;
	}

	dResult = DI_FLASH_Write(uiStartAddr, pBuffer, uiBlockSize);
	if(dResult != DI_ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("DI_FLASH_Write failed!\n"));
		fResult = FDI_ERR;
		goto done;
	}

/*************************
*	4. Read again
*************************/
	dResult = DI_FLASH_Read(uiStartAddr, pBuffer, uiBlockSize);
	if(dResult != DI_ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("DI_FLASH_Read failed!\n"));
		fResult = FDI_ERR;
		goto done;
	}

	VK_MEM_Memcpy(pCPCB, pBuffer+uiCCBStartAddr, IR_CPCB_SIZE);
	VK_MEM_Memcpy(stCrytInfo.pSrcBuffer, pCPCB+CCB_Part_Crc_Info_Start, CCB_Crc32-CCB_Part_Crc_Info_Start);

/*************************
*	5. Decrypt the read data
*************************/
	stCrytInfo.etCryptMode = DI_CRYPT_TDES;
	stCrytInfo.ulBuffSize = CCB_Crc32-CCB_Part_Crc_Info_Start;
	stCrytInfo.etCryptKey = DI_CRYPT_DEFAULT_SYSTEM_KEY;
	stCrytInfo.pKey = NULL;

	dResult = DI_CRYPT_Decrypt(&stCrytInfo);
	if(dResult != DI_ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("DI_CRYPT_Decrypt failed!\n"));
		fResult = FDI_ERR;
		goto done;
	}

	VK_MEM_Memcpy(pCPCB+CCB_Part_Crc_Info_Start, stCrytInfo.pDstBuffer, CCB_Crc32-CCB_Part_Crc_Info_Start);

/*************************
*	6. Compare decrypted data & original data
*************************/
	iResult = memcmp(pOriginalData, pCPCB, IR_CPCB_SIZE);
	if(iResult < 0)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("CCB Encryption Fail\n"));
		*bSuccess = FALSE;
	}
	else
	{
		FDI_PRINT(FDI_PRT_ERROR, ("CCB Encryption Success\n"));
		*bSuccess = TRUE;
	}

done:
	FVK_MEM_Free(pBuffer);
	FVK_MEM_Free(pCPCB);
	FVK_MEM_Free(pOriginalData);
	DI_CRYPT_MemFree(stCrytInfo.pSrcBuffer);
	DI_CRYPT_MemFree(stCrytInfo.pDstBuffer);

	return fResult;
#else
	return FDI_ERR;
#endif
}

FDI_ERR_CODE FDI_SYSTEM_GetCpuTemperature(unsigned char ucDeviceId, unsigned long *pulTemp)
{
	DI_ERR_CODE eDiErr;
#if (CABLE_MODEM_ENABLE==YES)
	int CmSavedTemp = 0,CmCurrentTemp = 0;
#endif
	DI_SYS_CPU_INFO_t cpuInfo;

	if(ucDeviceId == 0)
	{
		VK_MEM_Memset(&cpuInfo, 0x00, sizeof(DI_SYS_CPU_INFO_t));

		eDiErr = DI_SYSTEM_GetSystemInfo(DI_SYS_CPU_INFO, (void *)&cpuInfo, sizeof(DI_SYS_CPU_INFO_t));
		if(eDiErr != DI_ERR_OK)
		{
			FDI_PRINT(FDI_PRT_ERROR, ("DI_SYSTEM_GetSystemInfo Fail\n"));
		return FDI_ERR;
	}
		else
		{
			FDI_PRINT(0, ("[FDI_SYSTEM_GetCpuTemperature] : Temp %d, Voltage %d \n",cpuInfo.ulTemp/1000,cpuInfo.ulVoltage/1000));

			*pulTemp = cpuInfo.ulTemp/1000;
			return FDI_NO_ERR;
		}
	}
	else if(ucDeviceId == 1)
	{
#if (CABLE_MODEM_ENABLE==YES)
		DI_CM_GetCmSavedTemp(&CmSavedTemp,&CmCurrentTemp);
		FDI_PRINT(0, ("[FDI_SYSTEM_GetCpuTemperature] : Cur %d, Max %d \n",CmCurrentTemp/10,CmSavedTemp/10));

		*pulTemp = CmSavedTemp/10;
		return FDI_NO_ERR;
#else
		/* implement...*/
		return FDI_ERR;
#endif
	}
	return FDI_ERR;
}

/*******************************************************************************
* function : FDI_SYSTEM_SetIEEEAddr
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_SetIEEEAddr(unsigned char *pucIEEEAddr)
{
#if(RF4CE_RSSI_CHECK_ENABLE==YES)
	DRV_Error eDrvErr;

	eDrvErr = DRV_MICOM_SetIEEEAddr(pucIEEEAddr);
	if(eDrvErr != DRV_OK)
	{
		return FDI_ERR;
	}
#endif
	return FDI_NO_ERR;
}

/*******************************************************************************
* function : FDI_SYSTEM_GetIEEEAddr
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_GetIEEEAddr(unsigned char *pucIEEEAddr)
{
#if(RF4CE_RSSI_CHECK_ENABLE==YES)
	DRV_Error eDrvErr;

	eDrvErr = DRV_MICOM_GetIEEEAddr(pucIEEEAddr);
	if(eDrvErr != DRV_OK)
	{
		return FDI_ERR;
	}
#endif
	return FDI_NO_ERR;
}

/*******************************************************************************
* function : FDI_SYSTEM_SetRf4ceFactoryMode
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_SetRf4ceFactoryMode(void)
{
#if(RF4CE_RSSI_CHECK_ENABLE==YES)
	DRV_Error eDrvErr;

	eDrvErr = DRV_MICOM_SetRf4ceFactoryMode();
	if(eDrvErr != DRV_OK)
	{
		return FDI_ERR;
	}
#endif
	return FDI_NO_ERR;
}

/*******************************************************************************
* function : FDI_SYSTEM_RegisterRssiCallback
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_RegisterRssiCallback(pfnFDI_SYSTEM_RF4CE_RSSI_Notify pfnNotify)
{
#if(RF4CE_RSSI_CHECK_ENABLE==YES)
	DRV_Error eDrvErr;

	eDrvErr = DRV_MICOM_RegisterRf4ceRssiEventCallback((pfnDRV_UCOM_RF4CE_RSSI_Notify)pfnNotify);
	if(eDrvErr != DRV_OK)
	{
		return FDI_ERR;
	}
#endif
	return FDI_NO_ERR;
}

/*******************************************************************************
* function : FDI_SYSTEM_SetSystemInfo
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_SetSystemInfo(FDI_SYSTEM_INFO_e eSysInfo, void *pvInfo, HUINT32 ulInfoSize)
{
	DRV_Error eDrvErr = FDI_ERR;
	eDrvErr = DI_SYSTEM_SetSystemInfo(eSysInfo, pvInfo, ulInfoSize);
	if(eDrvErr != DRV_OK)
	{
		return FDI_ERR;
	}

	return FDI_NO_ERR;
}


/*******************************************************************************
* function : FDI_SYSTEM_RegisterEventNotify
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_RegisterEventNotify(FDI_SYSTEM_EVENT eSysEvent, FDI_SYSTEM_CALLBACK pfSysEventCallback)
{
	DRV_Error eDrvErr = FDI_ERR;

	eDrvErr = DI_SYSTEM_RegisterEventNotify(eSysEvent, pfSysEventCallback);
	if(eDrvErr != DRV_OK)
	{
		return FDI_ERR;
	}

	return FDI_NO_ERR;
}


/*******************************************************************************
* function : FDI_SYSTEM_SetPrivateData
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_SetPrivateData(unsigned char *pucData)
{
	DI_ERR_CODE eDiErr;

	if( pucData == NULL )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_GetSerialNumber] : Invalid Parameter!! \n"));
		return FDI_ERR;
	}

	eDiErr = DI_NVRAM_Write(DI_NVRAM_FS_PARTITION_RO,IRDETO_UC_PRIVATE_DATA,0, pucData, FDI_SYSTEM_DATA_IRDETO_PRIVATEDATA_SIZE);
	if( eDiErr != DI_ERR_OK )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_SetPrivateData] : DI_NVRAM_Read Error!! \n"));
		return FDI_ERR;
	}

	return FDI_NO_ERR;
}

/*******************************************************************************
* function : FDI_SYSTEM_GetPrivateData
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SYSTEM_GetPrivateData(unsigned char *pucData)
{
	DI_ERR_CODE eDiErr;
	int i;

	if( pucData == NULL )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_GetSerialNumber] : Invalid Parameter!! \n"));
		return FDI_ERR;
	}

	eDiErr = DI_NVRAM_Read(DI_NVRAM_FS_PARTITION_RO,IRDETO_UC_PRIVATE_DATA,0, pucData, FDI_SYSTEM_DATA_IRDETO_PRIVATEDATA_SIZE);
	if( eDiErr != DI_ERR_OK )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_GetPrivateData] : DI_FLASH_GetField Error!! \n"));
		return FDI_ERR;
	}

	return FDI_NO_ERR;
}

/*******************************************************************************
* function : FDI_SYSTEM_SetOtaDnType
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/

FDI_ERR_CODE FDI_SYSTEM_SetOtaDnType(FDI_SYSTEM_DOWNLOAD_TYPE_e *peOtaDnType)
{
	DI_ERR_CODE eDiErr;

	if( peOtaDnType == NULL )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_SetOtaDnType] : Invalid Parameter!! \n"));
		return FDI_ERR;
	}

	eDiErr = DI_NVRAM_Write(DI_NVRAM_FS_PARTITION_RW, (HUINT8 *)OTA_DN_TYPE, 0, peOtaDnType, sizeof(FDI_SYSTEM_DOWNLOAD_TYPE_e));
	if( eDiErr != DI_ERR_OK )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_SetOtaDnType] : DI_NVRAM_FS_PARTITION_RW Error!! \n"));
		return FDI_ERR;
	}

	return FDI_NO_ERR;
}

#if (VERIMATRIX_BETACRYPT_NSC_ENABLE==YES)
//#define NSC_TEST
#ifdef NSC_TEST
unsigned char NSc_VendorId[8]=
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

unsigned char NSc_VendorData[106]=
{
	0x60, 0x08, 0x00, 0x00, 0x96, 0x76, 0xc5, 0x65,
	0x8a, 0xe7, 0xc6, 0x50, 0xd1, 0x46, 0x5a, 0xed,
	0xac, 0x04, 0x47, 0x32, 0x99, 0x67, 0x2e, 0x4c,
	0xc0, 0x32, 0x88, 0x02, 0x9a, 0x36, 0x0d, 0x13,
	0xbb, 0x54, 0x18, 0x75, 0x4c, 0x64, 0x9f, 0xf9,
	0x60, 0xea, 0xfa, 0xde, 0x18, 0x9b, 0xe9, 0x0c,
	0x23, 0x05, 0x8c, 0x07, 0x5c, 0xd2, 0x30, 0x97,
	0x43, 0xbb, 0x9c, 0x44, 0xff, 0xab, 0xfb, 0x99,
	0xf0, 0x7f, 0x55, 0xc9, 0xb6, 0x6a, 0xa5, 0x9d,
	0x76, 0x64, 0xf1, 0x5b, 0xa1, 0xe9, 0x41, 0x3a,
	0xc3, 0x4c, 0x83, 0xed, 0x62, 0xd8, 0x37, 0x1e,
	0x90, 0x58, 0x79, 0xfc, 0x73, 0xfc, 0x4e, 0xfc,
	0x1a, 0x9b, 0x86, 0xdb, 0x18, 0xd8, 0x91, 0x39,
	0xbd, 0x8f
};

unsigned char NSc_ProviderId[8]=
{
	0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01
};

unsigned char NSc_ProviderData[106] =
{
	0x60, 0x08, 0x00, 0x00, 0xAB, 0x17, 0xDF, 0x9B,
	0x07, 0xBA, 0x1C, 0x7B, 0xBD, 0xB6, 0xE7, 0x4A,
	0xA3, 0x5A, 0x61, 0x49, 0x4B, 0xDA, 0x31, 0xFA,
	0xD1, 0xFB, 0x8F, 0x5B, 0xDA, 0xCD, 0x50, 0x1B,
	0xC8, 0x21, 0x7B, 0xB3, 0x1D, 0x16, 0x37, 0x2F,
	0xB2, 0x99, 0x22, 0x03, 0xF5, 0xE3, 0xA1, 0x87,
	0x40, 0xB5, 0xC1, 0xAF, 0xCF, 0x7E, 0xCB, 0x1B,
	0x09, 0x47, 0x7E, 0x7D, 0xE8, 0xB1, 0x0A, 0x23,
	0x84, 0xFE, 0x08, 0x1A, 0x7B, 0x8C, 0x29, 0xBA,
	0x7E, 0xFE, 0x89, 0xCC, 0x73, 0x80, 0x90, 0x9E,
	0x40, 0x9A, 0x40, 0x3D, 0x61, 0x6F, 0x3A, 0xAB,
	0xE9, 0xB4, 0xF6, 0x42, 0x88, 0x96, 0xB5, 0xF1,
	0xE5, 0x0B, 0x9E, 0x7C, 0xE3, 0x27, 0x70, 0x6A,
	0x52, 0x77
};
HUINT8 STBData[360] =
{
	0x1f, 0xf9, 0x7b, 0xa3, 0x05, 0x96, 0xee, 0x61, 0x24, 0xe3, 0x83, 0x0f, 0xb9, 0xed, 0x17, 0xb8,
	0x6b, 0x6c, 0xe9, 0x7e, 0x47, 0xe7, 0xc6, 0x8a, 0xc2, 0x1a, 0xd0, 0x2c, 0x45, 0x86, 0xb1, 0x5f,
	0x20, 0x55, 0x8a, 0xea, 0x64, 0x47, 0xd0, 0xb8, 0xde, 0xb9, 0xa5, 0x91, 0x3e, 0x84, 0x99, 0x68,
	0x26, 0xd0, 0xea, 0x2b, 0x88, 0x95, 0xa8, 0xe6, 0x88, 0xf3, 0xf3, 0xe6, 0x67, 0xcc, 0xa3, 0xc2,
	0xa8, 0xa5, 0x2c, 0x54, 0x2d, 0x2d, 0xf5, 0x61, 0xf8, 0xa4, 0x0d, 0x09, 0x5b, 0x25, 0xcf, 0xfd,
	0x08, 0xa6, 0x8a, 0xbe, 0x27, 0x24, 0x16, 0xdb, 0xea, 0xa3, 0xa2, 0xb7, 0x74, 0x7a, 0x97, 0xce,
	0x69, 0x0a, 0x69, 0xab, 0x80, 0x3f, 0x46, 0x63, 0xd7, 0x52, 0xe6, 0x10, 0x66, 0xa2, 0x2d, 0xb7,
	0xb6, 0x8c, 0x29, 0xcf, 0xa9, 0x4f, 0x19, 0xac, 0x80, 0x1f, 0xa1, 0x6c, 0x9e, 0xe3, 0x11, 0x21,
	0x4c, 0xcb, 0xba, 0x19, 0x71, 0xd1, 0x21, 0xa4, 0x41, 0xdc, 0x71, 0x65, 0xae, 0x29, 0x52, 0x92,
	0x1f, 0xd8, 0xff, 0x13, 0x4b, 0xfa, 0x6c, 0x87, 0xdd, 0xe3, 0x70, 0xf4, 0xb7, 0xe6, 0xf5, 0x06,
	0xfb, 0x62, 0x62, 0x16, 0xc3, 0x84, 0x08, 0x24, 0xff, 0x5f, 0x46, 0xec, 0x98, 0x95, 0x40, 0x2b,
	0x90, 0xaf, 0x7c, 0x5c, 0x49, 0xb0, 0x19, 0x11, 0x95, 0x2f, 0x23, 0x17, 0xd6, 0x07, 0x63, 0x0a,
	0xe7, 0x60, 0x98, 0xcb, 0xbf, 0x60, 0x8f, 0xc6, 0x0e, 0x59, 0x0d, 0x3d, 0x4b, 0x48, 0xde, 0xd7,
	0x8b, 0xe9, 0x0f, 0x6d, 0x52, 0x92, 0xd5, 0xbc, 0x53, 0xed, 0xad, 0xa9, 0x0f, 0x5f, 0x84, 0xe1,
	0x23, 0x53, 0xd0, 0xe3, 0x31, 0x94, 0xe6, 0x3c, 0x3e, 0xae, 0x8c, 0x4a, 0x78, 0x3b, 0x65, 0xe6,
	0xdc, 0x0f, 0x7f, 0x9b, 0x53, 0xcf, 0x30, 0x8b, 0x1c, 0x32, 0xe1, 0xa4, 0x5d, 0x9c, 0xa6, 0xf2,
	0x79, 0xb8, 0xce, 0x3d, 0x4d, 0x48, 0xd5, 0x63, 0x0c, 0xb8, 0x13, 0xd3, 0x6e, 0x2f, 0x48, 0xed,
	0x7a, 0xc2, 0x3b, 0xac, 0xc4, 0x9c, 0xc4, 0xf4, 0x4f, 0xe6, 0xe0, 0xcf, 0xf7, 0x62, 0x21, 0x42,
	0x74, 0x53, 0x23, 0x96, 0x4a, 0x92, 0x61, 0x66, 0x25, 0x45, 0x7c, 0x9d, 0xfc, 0x94, 0x62, 0xc9,
	0xe5, 0xc7, 0x09, 0x5c, 0x29, 0x8f, 0x74, 0xbf, 0x37, 0x44, 0x62, 0xe9, 0xb1, 0xe9, 0xbf, 0x84,
	0x4e, 0xd6, 0x30, 0x75, 0x56, 0x62, 0x26, 0xbf, 0x3e, 0xee, 0xe0, 0x53, 0x46, 0x13, 0xa3, 0x10,
	0xfe, 0xfb, 0x01, 0x47, 0x76, 0x20, 0xf2, 0x56, 0x29, 0xee, 0x75, 0x3b, 0xd5, 0x86, 0xc7, 0xfa,
	0x25, 0x85, 0x2e, 0x0e, 0x91, 0xc2, 0x4f, 0x9f
};
#endif

unsigned char FDI_SYSTEM_VMX_WriteNvmData(unsigned char bBlockId, unsigned char* pabSrc, unsigned short wLength)
{
	DI_ERR_CODE eDiErr;

	FDI_PRINT(FDI_PRT_ERROR, ("WriteNvmData() : \n"));

	if(bBlockId != 2)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_VMX_WriteNvmData] No Area for NSc \n"));
		return 1;
	}

	if( pabSrc == NULL )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_VMX_WriteNvmData] pabSrc is null \n"));
		return 1;
	}

	eDiErr = DI_NVRAM_Write(DI_NVRAM_FS_PARTITION_RO, VMX_BC_NSC_DATA, 0, pabSrc, FDI_SYSTEM_DATA_VMX_BC_NSC_DATA_SIZE);
	if( eDiErr != DI_ERR_OK )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_VMX_WriteNvmData] : DI_FLASH_SetField Error!! \n"));
		return 1;
	}

	return 0;
}

unsigned char FDI_SYSTEM_VMX_ReadNvmData(uint8_t bBlockId, uint8_t* pabDest, uint16_t wLength)
{
	DI_ERR_CODE eDiErr;

	FDI_PRINT(FDI_PRT_ERROR, ("ReadNvmData() : \n"));

	if(bBlockId != 2)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_VMX_ReadNvmData] No Area for NSc \n"));
		return 1;
	}

	if( pabDest == NULL )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_VMX_ReadNvmData] pabSrc is null \n"));
		return 1;
	}

	eDiErr = DI_NVRAM_Read(DI_NVRAM_FS_PARTITION_RO, VMX_BC_NSC_DATA, 0, pabDest, FDI_SYSTEM_DATA_VMX_BC_NSC_DATA_SIZE);
	if( eDiErr != DI_ERR_OK )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SYSTEM_VMX_ReadNvmData] : DI_FLASH_SetField Error!! \n"));
		return 1;
	}

	return 0;
}

void FDI_SYSTEM_VMX_SendBoxId(unsigned char* pabBoxId)
{
	FDI_SYSTEM_VMXBC_NSc_t	*pNSc = &stFdiSystemVmxBcNSc;

	if(pNSc == NULL)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("pNSc is null\n"));
		return;
	}

	FVK_MEM_Set(pNSc->ucBoxId, 0, MAX_NSC_BOXID_LEN);
	FVK_MEM_Copy(pNSc->ucBoxId, pabBoxId, MAX_NSC_BOXID_LEN);

	FDI_PRINT(FDI_PRT_ERROR, ("boxId [%02x %02x %02x %02x %02x %02x %02x %02x]\n",
		pNSc->ucBoxId[0], pNSc->ucBoxId[1], pNSc->ucBoxId[2], pNSc->ucBoxId[3], pNSc->ucBoxId[4], pNSc->ucBoxId[5], pNSc->ucBoxId[6], pNSc->ucBoxId[7]));
}

void FDI_SYSTEM_VMX_Task(unsigned int argc, void *argv)
{
	while (1)
	{
		BC_Task();
		FVK_TASK_Sleep(10);
	}
}

FDI_ERR_CODE FDI_SYSTEM_VMXBC_SetValueForInitNScComm(
					unsigned char *pucVendorId, unsigned short ucVendorIdLength,
					unsigned char *pucVendorData, unsigned short ucVendorDataLength,
					unsigned char *pucProvId, unsigned short ucProvIdLength,
					unsigned char *pucProvData, unsigned short ucProvDataLength)
{
	FDI_SYSTEM_VMXBC_NSc_t	*pNSc = &stFdiSystemVmxBcNSc;
	unsigned char tmpBoxId[MAX_NSC_BOXID_LEN];

	if(pNSc == NULL)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("pNSc is null\n"));
		return FDI_ERR;
	}

	FVK_MEM_Copy(&tmpBoxId[0], &pNSc->ucBoxId[0] , MAX_NSC_BOXID_LEN);
	FVK_MEM_Set(pNSc, 0, sizeof(FDI_SYSTEM_VMXBC_NSc_t));
	FVK_MEM_Copy(&pNSc->ucBoxId[0], &tmpBoxId[0], MAX_NSC_BOXID_LEN);

	pNSc->pucVendorId = FVK_MEM_Alloc(ucVendorIdLength+1);
	if(pNSc->pucVendorId ==NULL)
		goto fdi_err;

	pNSc->pucVendorData = FVK_MEM_Alloc(ucVendorDataLength+1);
	if(pNSc->pucVendorData == NULL)
		goto fdi_err;

	pNSc->pucProvId = FVK_MEM_Alloc(ucProvIdLength+1);
	if(pNSc->pucProvId == NULL)
		goto fdi_err;

	pNSc->pucProvData = FVK_MEM_Alloc(ucProvDataLength+1);
	if(pNSc->pucProvData == NULL)
		goto fdi_err;

	FVK_MEM_Set(pNSc->pucVendorId, 0, ucVendorIdLength+1);
	FVK_MEM_Set(pNSc->pucVendorData, 0, ucVendorDataLength+1);
	FVK_MEM_Set(pNSc->pucProvId, 0, ucProvIdLength+1);
	FVK_MEM_Set(pNSc->pucProvData, 0, ucProvDataLength+1);

	FVK_MEM_Copy(pNSc->pucVendorId, pucVendorId, ucVendorIdLength);
	FVK_MEM_Copy(pNSc->pucVendorData, pucVendorData, ucVendorDataLength);
	FVK_MEM_Copy(pNSc->pucProvId, pucProvId, ucProvIdLength);
	FVK_MEM_Copy(pNSc->pucProvData, pucProvData, ucProvDataLength);

	return FDI_NO_ERR;
fdi_err:
	if(pNSc->pucVendorId)
	{
		FVK_MEM_Free(pNSc->pucVendorId);
		pNSc->pucVendorId = NULL;
	}
	if(pNSc->pucVendorData)
	{
		FVK_MEM_Free(pNSc->pucVendorData);
		pNSc->pucVendorData = NULL;
	}
	if(pNSc->pucProvId)
	{
		FVK_MEM_Free(pNSc->pucProvId);
		pNSc->pucProvId = NULL;
	}
	if(pNSc->pucProvData)
	{
		FVK_MEM_Free(pNSc->pucProvData);
		pNSc->pucProvData = NULL;
	}

	return FDI_ERR;
}

FDI_ERR_CODE FDI_SYSTEM_VMX_GetNSc(unsigned char *pucData, unsigned short *pusLength)
{
	signed short ret_BcInit = k_BcSuccess;
	signed short ret_CheckNSc = k_BcSuccess;
	FDI_SYSTEM_VMXBC_NSc_t	*pNSc = &stFdiSystemVmxBcNSc;
	unsigned long ulVmxTaskID;
	unsigned int nErr;

	if(pNSc == NULL)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("pNSc is null\n"));
		return FDI_ERR;
	}

	ret_BcInit = BC_Init();

	nErr = FVK_TASK_Create((void*)FDI_SYSTEM_VMX_Task, FVK_TASK_DEFAULT_PRIORITY, FVK_TASK_LARGE_STACK_SIZE, "VMX TASK", NULL, &ulVmxTaskID);
	if (!nErr)
	{
		nErr = FVK_TASK_Start(ulVmxTaskID);
	}
	else
	{
		FDI_PRINT(FDI_PRT_ERROR, ("FVK_TASK_Create : fail\n"));
		return FDI_ERR;
	}

	if(ret_BcInit == k_BcNSc)		/* NSc로 성공적으로 init되는 경우 */
	{
		FDI_PRINT(FDI_PRT_ERROR, ("BC_Init() : NSc solution is initialised successfully - Ok\n"));
	}
	else if(ret_BcInit == k_BcSuccess) // NSc로 부팅이 안된상태에서 library init가 성공적으로 동작된 경우.
	{
		ret_CheckNSc = BC_CheckNSc();
		if(ret_CheckNSc == k_BcNotSupported)
		{
			FDI_PRINT(FDI_PRT_ERROR, ("BC_CheckNSc() : no NSc implementation exists\n"));
		}
		else if(ret_CheckNSc == k_BcSuccess) // NSc가 library에 include 되고 이미 individualization 되어 있다. // Field에서는 이상태이어야한다.
		{
			FDI_PRINT(FDI_PRT_ERROR, ("BC_CheckNSc() : the NSc implementation exists and the set top box is already individualized\n"));
		}
		else if(ret_CheckNSc == k_BcError)
		{
			FDI_PRINT(FDI_PRT_ERROR, ("BC_CheckNSc() : the NSc implementation exists and the set top box is not yet individualized\n"));
#ifdef NSC_TEST
			FDI_SYSTEM_VMXBC_SetValueForInitNScComm(NSc_VendorId, 8, NSc_VendorData, 106, NSc_ProviderId, 8, NSc_ProviderData, 106);
#endif
			if(pNSc->pucVendorId == NULL) // 임시 테스트를위해 적요함.
			{
				FDI_PRINT(FDI_PRT_ERROR, ("Vendor and Provider data didn't received from Client : Error\n"));
				return FDI_ERR;
			}

			pNSc->usLength = MAX_NSC_DATA_LEN;

			if(BC_InitNScComm(pNSc->ucData, &pNSc->usLength, pNSc->ucBoxId, pNSc->pucVendorId, pNSc->pucVendorData, pNSc->pucProvId, pNSc->pucProvData) != k_BcSuccess)
			{
				FDI_PRINT(FDI_PRT_ERROR, ("BC_InitNScComm() : Error\n"));
				return FDI_ERR;
			}
			VK_MEM_Memcpy(pucData, pNSc->ucData, pNSc->usLength);
			* pusLength = pNSc->usLength;
		}
		else
		{
			FDI_PRINT(FDI_PRT_ERROR, ("BC_CheckNSc() = %d : Error\n", ret_CheckNSc));
		}
	}
	else if(ret_BcInit == k_BcError)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("BC_Init() : - Fail : SC_IOCTL() or initialising the library internals failed\n"));
	}
	else
	{
		FDI_PRINT(FDI_PRT_ERROR, ("BC_Init() : - Error\n"));
	}

	return FDI_NO_ERR;
}

FDI_ERR_CODE FDI_SYSTEM_VMX_SetNSc(unsigned char *pucData, unsigned short usLength)
{
	FDI_ERR_CODE ret = FDI_NO_ERR;
	FDI_SYSTEM_VMXBC_NSc_t	*pNSc = &stFdiSystemVmxBcNSc;

	if(pNSc == NULL)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("pNSc is null\n"));
		return;
	}

	if(BC_InitNSc(pucData, &usLength) != k_BcSuccess)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("BC_InitNSc() : - Error\n"));
		ret = FDI_ERR;
	}

	if(pNSc->pucVendorId)
	{
		FVK_MEM_Free(pNSc->pucVendorId);
		pNSc->pucVendorId = NULL;
	}
	if(pNSc->pucVendorData)
	{
		FVK_MEM_Free(pNSc->pucVendorData);
		pNSc->pucVendorData = NULL;
	}
	if(pNSc->pucProvId)
	{
		FVK_MEM_Free(pNSc->pucProvId);
		pNSc->pucProvId = NULL;
	}
	if(pNSc->pucProvData)
	{
		FVK_MEM_Free(pNSc->pucProvData);
		pNSc->pucProvData = NULL;
	}

	return ret;
}
#endif


