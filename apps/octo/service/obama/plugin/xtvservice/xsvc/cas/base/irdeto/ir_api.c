
#include "ir_platform.h"

#include <di_err.h>
#include <di_nvram.h>
#include "di_channel.h"
#if defined(CONFIG_MW_CH_SATELLITE)
#include "di_channel_s.h"
#elif defined(CONFIG_MW_CH_CABLE)
#include "di_channel_c.h"
#elif defined(CONFIG_MW_CH_TERRESTRIAL)
#include "di_channel_t.h"
#else
#error "irdeto not support this channel type"
#endif

#include "pal_sys.h"
#include <pal_sc.h>
#include "svc_sys.h"

#include <_svc_cas_sub_dev_api.h>
#if defined (CONFIG_MW_CAS_IRDETO_SCELL)
#include "s3_vd.h"
#include "s3_status.h"
#include "s3_maturity_rating.h"
#endif

#if defined (CONFIG_MW_CAS_IRDETO_SCELL)
#include "vd_humax.h"
#endif
#include "ir_svc.h"
#include "ir_nvr.h"
#include "ir_api.h"
#include "ir_loader.h"
#include "ir_msg.h"  // text message
#if defined(CONFIG_MW_CAS_IRDETO_FTA_BLOCK)
#include "ir_fta_block.h"
#endif

#if defined(CONFIG_MW_CAS_IRDETO_SOL)
#include <db_param.h>
#endif

#include <isosvc.h>
#include <ir_status.h>

#include "_svc_cas_mgr_pal_port.h"

#define		MACRO_GET_VALID_VALUE(a, b) 		((a == -1) ? a : b)


/********************************************************************
*	Global variables
********************************************************************/


/********************************************************************
*	Static variables
********************************************************************/

static HUINT8		s_aucCPCB[IR_CPCB_SIZE];
static IHL_STATUS s_stIrLoaderStatus;
static Boot_Block_Control_Block s_stBBCB;

#if defined(CONFIG_MW_CAS_IRDETO_SECURE)
static HUINT8 s_virtualCwMode = 0;

static ia_word32 fusingInfo_SCS = 0;
static ia_word32 fusingInfo_JTAG = 0;
static ia_word32 fusingInfo_OTP = 0;
static ia_word32 fusingInfo_CWE = 0;
#endif

/********************************************************************
*	Static functions Prototypes
*	The following functions will be used as inner functions
********************************************************************/

AXI_STATUSCODE_IDX	CAS_IR_DRV_GetErrorCodeIndex(ia_status_st *pStatus);

#if defined(IRPLAT_SYS_SAVE_SMARTCARD_NUMBER) && defined (CONFIG_OP_ALMAJD)
AXI_STATUSCODE_IDX	CAS_IR_DRV_AlMajd_E30_GetErrorCodeIndex (void);
#endif
/********************************************************************
*	Extern functions Prototypes
*	The following functions will be used as extern functions
********************************************************************/
extern IHL_S16 I_HLSUP_GetStatusInfo (IHL_PSTATUS pstStatus);

// ewlee 20100818 - move to here from ir_drv.c file
#define	_______IR_DRV_UTIL___________________
/********************************************************************************
********************************************************************************/
HUINT16	CAS_IR_DRV_Get16Bits(HUINT8 *base)
{
	return (base[0] << 8) | (base[1]);
}

/********************************************************************************
********************************************************************************/
HUINT32	CAS_IR_DRV_Get24Bits(HUINT8 *base)
{
	return (base[0] << 16) | (base[1] << 8) | base[2];
}

/********************************************************************************
********************************************************************************/
HUINT32	CAS_IR_DRV_Get32Bits(HUINT8 *base)
{
	return (base[0] << 24) | (base[1] << 16) | (base[2] << 8) | (base[3]);
}

HUINT32	CAS_IR_DRV_Bcd2Dec(HUINT32 dwVal)
{
	HUINT32	dwRetVal;

	dwRetVal = 0;

	dwRetVal +=(dwVal & 0x0f) ;
	dwVal >>= 4 ;
	dwRetVal +=(dwVal & 0x0f) * 10;
	dwVal >>= 4 ;
	dwRetVal +=(dwVal & 0x0f) * 100;
	dwVal >>= 4 ;
	dwRetVal +=(dwVal & 0x0f) * 1000;
	dwVal >>= 4 ;
	dwRetVal +=(dwVal & 0x0f) * 10000;
	dwVal >>= 4 ;
	dwRetVal +=(dwVal & 0x0f) * 100000;
	dwVal >>= 4 ;
	dwRetVal +=(dwVal & 0x0f) * 1000000;
	dwVal >>= 4 ;
	dwRetVal +=(dwVal & 0x0f) * 10000000;
	dwVal >>= 4 ;

	return(dwRetVal);
}


#define _____LOADER_STATUS_____

/********************************************************************
 Function   : local_cas_ir_Read_BBCB
 Description: read bbcb data in the loader block.

 Input      :
 Output     :
 Return     :
 ********************************************************************/
HINT32 local_cas_ir_Read_BBCB(Boot_Block_Control_Block *pBBCB)
{
	HUINT32	result = ERR_OK;
	HUINT8 aucBBCB[IR_BBCB_SIZE];

	HxLOG_Print("%s\n", __FUNCTION__ );

#if defined(IRPLAT_TEST_INCOMPLETED_LOADER)
	return result ;
#endif

	result = svc_cas_DevSysGetField(eCAS_DEV_FLASH_FIELD_IRDETO_BBCB, 0, aucBBCB, IR_BBCB_SIZE);
	if (ERR_OK != result)
	{
		HxLOG_Error("svc_cas_DevSysGetField %08X", result);
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	/* When you tried to get real value from nvram, you must not use a structure copy method due to fragmentation, jichoi. */
	pBBCB->usCRC16 			= CAS_IR_DRV_Get16Bits(aucBBCB + BBCB_CRC16_OFFSET);
	pBBCB->ucManufacturerCode	= aucBBCB[BBCB_MANCODE_OFFSET] ;
	pBBCB->ucHardwareCode		= aucBBCB[BBCB_HWCODE_OFFSET] ;
	pBBCB->ucLoaderMajor		= aucBBCB[BBCB_LOADERMAJOR_OFFSET] ;
	pBBCB->ucLoaderMinor		= aucBBCB[BBCB_LOADERMINOR_OFFSET] ;
	pBBCB->ucLoaderType		= aucBBCB[BBCB_LOADERTYPE_OFFSET] ;
	pBBCB->ulSerialNumber		= CAS_IR_DRV_Get32Bits(aucBBCB + BBCB_SERIALNUM_OFFSET);
	pBBCB->ulRandomNumber 	= CAS_IR_DRV_Get32Bits(aucBBCB + BBCB_RANDOMNUM_OFFSET);

	HxLOG_Print("\n\n___________________BBCB__DATA_______________________\n");
	HxLOG_Print("usCRC16 = 0x%x\n", pBBCB->usCRC16);
	HxLOG_Print("ucManufacturerCode = 0x%x\n", pBBCB->ucManufacturerCode);
	HxLOG_Print("ucHardwareCode = 0x%x\n", pBBCB->ucHardwareCode);
	HxLOG_Print("ucLoaderMajor = 0x%x\n", pBBCB->ucLoaderMajor);
	HxLOG_Print("ucLoaderMinor = 0x%x\n", pBBCB->ucLoaderMinor);
	HxLOG_Print("ucLoaderType = 0x%x\n", pBBCB->ucLoaderType);
	HxLOG_Print("ulSerialNumber = 0x%x\n", (HUINT32)pBBCB->ulSerialNumber);
	HxLOG_Print("ulRandomNumber = 0x%x\n", (HUINT32)pBBCB->ulRandomNumber);
	HxLOG_Print("\n_____________________________________________________\n");

#if defined(CONFIG_PROD_ICORDHDPLUSME)
		if(pBBCB->ucHardwareCode != 62)
		{
			return ERR_FAIL;
		}
#endif

#if defined(CONFIG_PROD_VTCHD06)
		if(pBBCB->ucHardwareCode != 67)
		{
			return ERR_FAIL;
		}
#endif

#if defined(CONFIG_PROD_UDFOXHD)
		if(pBBCB->ucHardwareCode != 68)
		{
			return ERR_FAIL;
		}
#endif

	return ERR_OK;

}

HINT32 local_cas_ir_Read_CPCB(HUINT8 * pCPCB)
{
	HUINT32	result = ERR_OK;
	HxLOG_Info("\n");

#if defined(IRPLAT_TEST_INCOMPLETED_LOADER)
	return result;
#endif

#if defined(CONFIG_MW_CAS_IRDETO_SECURE)
	result = svc_cas_DevSysGetField(eCAS_DEV_FLASH_FIELD_IRDETO_CPCB_ENCRYPTED, 0, pCPCB, IR_CPCB_SIZE);
#else
	result = svc_cas_DevSysGetField(eCAS_DEV_FLASH_FIELD_IRDETO_CPCB, 0, pCPCB, IR_CPCB_SIZE);
#endif
	if (ERR_OK != result)
	{
		HxLOG_Print(" svc_cas_DevSysGetField %08X", result);
		HxLOG_Critical("\n\n");

		return ERR_FAIL;
	}

	return ERR_OK;
}

HINT32 local_cas_ir_ReadLoaderStatus(IHL_STATUS * pLoaderStatus )
{
	IHL_S16 nErrCode;

	HxLOG_Info("\n");


	memset(pLoaderStatus, 0, sizeof(IHL_STATUS));

#if defined(IRPLAT_TEST_INCOMPLETED_LOADER)
	pLoaderStatus->u16SysID = 0xFFFF;
	pLoaderStatus->s16SysIDValid = 0;
	pLoaderStatus->u16KeyVer = 0;
	pLoaderStatus->s16KeyVerValid = 0;
	pLoaderStatus->u16SigVer = 0;
	pLoaderStatus->s16SigVerValid = 0;
	pLoaderStatus->s16VariantValid = 0;
	pLoaderStatus->u8BlockFailMask = 0;
#else
	nErrCode = I_HLSUP_GetStatusInfo(pLoaderStatus);
	if( nErrCode )
	{
		HxLOG_Error("[I_HLSUP_GetStatusInfo ERROR : %d]\n", nErrCode);
		return ERR_FAIL;
	}
#endif

#if defined(IRPLAT_TEST_FORCE_HUMAX_VARIANT)
		/* it 	must be humax manufacturer ID for descrambing Humax pisys stream, jichoi*/
		pLoaderStatus->u16Variant = 0xF;
#endif

	HxLOG_Print("[Irdeto Status Information]\n");
	HxLOG_Print("usSysID = %d(0x%x)\n", 		pLoaderStatus->u16SysID, pLoaderStatus->u16SysID);
	HxLOG_Print("ssSysIDValid = %d(0x%x)\n", pLoaderStatus->s16SysIDValid, pLoaderStatus->s16SysIDValid);
	HxLOG_Print("usKeyVer = %d(0x%x)\n", 	pLoaderStatus->u16KeyVer, pLoaderStatus->u16KeyVer);
	HxLOG_Print("ssKeyVerValid = %d(0x%x)\n", pLoaderStatus->s16KeyVerValid, pLoaderStatus->s16KeyVerValid);
	HxLOG_Print("usSigVer = %d(0x%x)\n", 	pLoaderStatus->u16SigVer, pLoaderStatus->u16SigVer);
	HxLOG_Print("ssSigVerValid = %d(0x%x)\n", pLoaderStatus->s16SigVerValid, pLoaderStatus->s16SigVerValid);
	HxLOG_Print("usVariant = %d(0x%x)\n", 	pLoaderStatus->u16Variant, pLoaderStatus->u16Variant);
	HxLOG_Print("ssVariantValid = %d(0x%x)\n", pLoaderStatus->s16VariantValid, pLoaderStatus->s16VariantValid);

	return ERR_OK;
}

static HERROR local_cas_ir_ChangeString(HUINT8 *srcStr, HUINT8 *destStr)
{
	HUINT8 szTmpMsg[20];
	HUINT32 i=0, j=0;
	HUINT32	ulLen;

	MWC_UTIL_DvbUtf8Sprintf(szTmpMsg, "%s", srcStr);

	ulLen = svc_cas_MgrUtilStrLen(szTmpMsg);
	j=0;
	for(i=0; i < ulLen; i ++)
	{
		destStr[j] = szTmpMsg[i];
		j++;
		destStr[j] = ' ';
		j++;
	}

	destStr[j-1] = '\0';

	return ERR_OK;
}

static HERROR local_cas_ir_makeStatusInfoString(unsigned short infoValue, HUINT32 valueValid, HUINT8 *valueToStr)
{
	HUINT16		i=0;
	HUINT32		lenSzTmpDest=0, lenCheck=0;
	HUINT8		szTmpDest[10]={0,}, szLenCheck[10]={0,};

	if(MACRO_GET_VALID_VALUE(valueValid, infoValue) == 0xFFFF)		//-1
	{
		MWC_UTIL_DvbUtf8Sprintf(valueToStr, " X 0 1");
		goto FUNC_END;
	}
	else if(MACRO_GET_VALID_VALUE(valueValid, infoValue) == 0)
	{
		MWC_UTIL_DvbUtf8Sprintf(valueToStr, " X X 0");
		goto FUNC_END;
	}

	//len check
	MWC_UTIL_DvbUtf8Sprintf(szLenCheck, "%d", MACRO_GET_VALID_VALUE(valueValid, infoValue));
	lenCheck = svc_cas_MgrUtilStrLen(szLenCheck);

	if(lenCheck<=3)
	{
		MWC_UTIL_DvbUtf8Sprintf(szTmpDest, "%03d", MACRO_GET_VALID_VALUE(valueValid, infoValue));		//MWC_UTIL_DvbUtf8Sprintf
		lenSzTmpDest = svc_cas_MgrUtilStrLen(szTmpDest);

		for(i=0; i <4 ; i++)
		{
			if(szTmpDest[i] == '0')
				szTmpDest[i] = 'X';
			else
				break;
		}
		MWC_UTIL_DvbStrcpy(valueToStr, szTmpDest);
		valueToStr[4] = '\0';
	}
	else
	{
		snprintf(valueToStr, 16, "%d", MACRO_GET_VALID_VALUE(valueValid, infoValue));		//MWC_UTIL_DvbUtf8Sprintf
		lenSzTmpDest = svc_cas_MgrUtilStrLen(valueToStr);
		valueToStr[lenSzTmpDest] = '\0';
	}
	MWC_UTIL_DvbUtf8Sprintf(szTmpDest, "%s", valueToStr);

	local_cas_ir_ChangeString(szTmpDest, valueToStr);


FUNC_END:

	MWC_UTIL_DvbUtf8Sprintf(szTmpDest, "%s", valueToStr);
	MWC_UTIL_DvbStrcpyStrOnly(valueToStr, szTmpDest);

	return ERR_OK;

}

HINT32 xsvc_cas_IrLoaderInit(void)
{
	HINT32	result = ERR_OK;

	HxLOG_Info("\n");

	if(  ERR_OK != local_cas_ir_Read_BBCB(&s_stBBCB) )
	{
		HxLOG_Error("in local_cas_ir_Read_BBCB !\n");
		result |= ERR_FAIL;
	}
	if(  ERR_OK != local_cas_ir_Read_CPCB(s_aucCPCB) )
	{
		HxLOG_Error("in local_cas_ir_Read_CPCB !\n");
		result |= ERR_FAIL;
	}
	if(  ERR_OK != local_cas_ir_ReadLoaderStatus(&s_stIrLoaderStatus) )
	{
		HxLOG_Error("in local_cas_ir_ReadLoaderStatus !\n");
		result |= ERR_FAIL;
	}
	return result;
}

HIR_ERR xsvc_cas_IrLoaderGetManuCode(HUINT8 *pucManCode)
{
	HxLOG_Info("\n");

	*pucManCode = s_stBBCB.ucManufacturerCode;

	return HIR_OK;
}

HIR_ERR xsvc_cas_IrLoaderGetManuCodeString(HUINT8 *pucManCode)
{
	HxLOG_Info("\n");

	local_cas_ir_makeStatusInfoString(s_stBBCB.ucManufacturerCode, 0, pucManCode);

	return HIR_OK;
}
#if 0
HIR_ERR XSVC_CAS_IR_LOADER_GetClientVersion(HUINT8 *pucClientVersion)
{
	HUINT16			usStrLen;

	HxLOG_Info("\n");

	xsvc_cas_IrGetClientVersion(&usStrLen, &pucClientVersion);

	return HIR_OK;
}
#endif

HIR_ERR xsvc_cas_IrLoaderGetHWCode(HUINT8 *pucHWCode)
{
	HxLOG_Info("\n");

	*pucHWCode = s_stBBCB.ucHardwareCode;

	return HIR_OK;
}

HIR_ERR xsvc_cas_IrLoaderGetHWCodeString(HUINT8 *pucHWCode)
{
	HxLOG_Info("\n");

	local_cas_ir_makeStatusInfoString(s_stBBCB.ucHardwareCode, 0, pucHWCode);

	return HIR_OK;
}

HIR_ERR xsvc_cas_IrLoaderGetVariantString(HUINT8 *pucVariant)
{
	HxLOG_Info("\n");

	local_cas_ir_makeStatusInfoString(s_stIrLoaderStatus.u16Variant, s_stIrLoaderStatus.s16VariantValid, pucVariant);

	return HIR_OK;
}

HIR_ERR xsvc_cas_IrLoaderGetSysIdString(HUINT8 *pucSysId)
{
	HxLOG_Info("\n");

	local_cas_ir_makeStatusInfoString(s_stIrLoaderStatus.u16SysID, s_stIrLoaderStatus.s16SysIDValid, pucSysId);

	return HIR_OK;
}

HIR_ERR xsvc_cas_IrLoaderGetKeyVerString(HUINT8 *pucKeyVer)
{
	HxLOG_Info("\n");

	local_cas_ir_makeStatusInfoString(s_stIrLoaderStatus.u16KeyVer, s_stIrLoaderStatus.s16KeyVerValid, pucKeyVer);

	return HIR_OK;
}

HIR_ERR xsvc_cas_IrLoaderGetSigVerString(HUINT8 *pucSigVer)
{
	HxLOG_Info("\n");

	local_cas_ir_makeStatusInfoString(s_stIrLoaderStatus.u16SigVer, s_stIrLoaderStatus.s16SigVerValid, pucSigVer);

	return HIR_OK;
}

HIR_ERR xsvc_cas_IrLoaderGetPrivateData(HUINT8 *pPrivateData)
{
	HxLOG_Info("\n");

	// TODO: 필요시 추가

	return HIR_OK;
}

/********************************************************************
 Function   : xsvc_cas_IrLoaderGetStatusInfo
 Description: Get irdeto partition status information

 Input      : pointer of irdeto partition status information
			 typedef struct {
				unsigned short	usSysID;
				signed short	ssSysIDValid;
				unsigned short	usKeyVer;
				signed short	ssKeyVerValid;
				unsigned short	usSigVer;
				signed short	ssSigVerValid;
				unsigned short	usVariant;
				signed short	ssVariantValid;
				HUINT8	ucBlockFailMask;
			} IR_STATUS;
 Output     :
 Return     : success : 0
              fail	  : -1
 ********************************************************************/
HIR_ERR xsvc_cas_IrLoaderGetStatusInfo(IHL_STATUS *pstIRStatus)
{
	*pstIRStatus = s_stIrLoaderStatus;
	return HIR_OK;
}

HIR_ERR xsvc_cas_IrLoaderGetLoadSeqNum(HUINT8 *pucLDNum)
{
	//loader code와 같은 block에 위치하므로, DCM으로 일지 않도록 한다.
	//DCM에 등록해서 읽을경우, flash block이 unlock되므로 not work
	*pucLDNum = s_aucCPCB[CCB_Load_Sequence_Num];
	return HIR_OK;

}
HIR_ERR	xsvc_cas_IrLoaderGetSignDate(HUINT16 *pusMJD)
{
	HUINT8	*pucMJD = (HUINT8*)pusMJD;

	pucMJD[1] = s_aucCPCB[CCB_Sign_Time_Date];
	pucMJD[0] = s_aucCPCB[CCB_Sign_Time_Date + 1];


//	memcpy(pusMJD, &s_aucCPCB[CCB_Sign_Time_Date], 2);
//	*pusMJD = *((unsigned short *)(&s_aucCPCB[CCB_Sign_Time_Date]));
	HxLOG_Info("%s, Mjd = %04X\n", __FUNCTION__, *pusMJD);

	return HIR_OK;
}
HIR_ERR	xsvc_cas_IrLoaderGetSignTime(HxDATETIME_Time_t *pstTime)
{

	if(HLIB_DATETIME_ConvertBcdTimeToDecimalTime(&s_aucCPCB[CCB_Sign_Time_Date + 2], 3, pstTime) != ERR_OK)
	{
		return HIR_NOT_OK;
	}

	return HIR_OK;
}

HIR_ERR	xsvc_cas_IrLoaderGetDnldDate(HUINT16 *pusMJD)
{
	HUINT8	*pucMJD = (HUINT8*)pusMJD;

	HxLOG_Info("(s_aucCPCB[CCB_Download_Time_Date]=0x%X\n", s_aucCPCB[CCB_Download_Time_Date]);
	HxLOG_Info("(s_aucCPCB[CCB_Download_Time_Date+1]=0x%X\n", s_aucCPCB[CCB_Download_Time_Date+1]);

	pucMJD[1] = *((unsigned short *)(&s_aucCPCB[CCB_Download_Time_Date]));
	pucMJD[0] = *((unsigned short *)(&s_aucCPCB[CCB_Download_Time_Date + 1]));
	HxLOG_Info("%s, Mjd = %04X\n", __FUNCTION__, *pusMJD);

	return HIR_OK;
}

HIR_ERR	xsvc_cas_IrLoaderGetDnldTime(HxDATETIME_Time_t *pstTime)
{
	HxLOG_Info("(s_aucCPCB[CCB_Download_Time_Date+2]=0x%X\n", s_aucCPCB[CCB_Download_Time_Date+2]);
	HxLOG_Info("(s_aucCPCB[CCB_Download_Time_Date+3]=0x%X\n", s_aucCPCB[CCB_Download_Time_Date+3]);
	HxLOG_Info("(s_aucCPCB[CCB_Download_Time_Date+4]=0x%X\n", s_aucCPCB[CCB_Download_Time_Date+4]);

	if(HLIB_DATETIME_ConvertBcdTimeToDecimalTime(&s_aucCPCB[CCB_Download_Time_Date + 2], 3, pstTime) != ERR_OK)
	{
		return HIR_NOT_OK;
	}

	return HIR_OK;
}

HIR_ERR xsvc_cas_IrLoaderGetSerialNum(HUINT32 *pSerNum)
{
	*pSerNum = s_stBBCB.ulSerialNumber;
	HxLOG_Print(" Serial Num : %d\n", *pSerNum);
	return HIR_OK;
}

HIR_ERR xsvc_cas_IrLoaderGetRandomNum(HUINT32 *pRandomNum)
{
	*pRandomNum = s_stBBCB.ulRandomNumber;
	return HIR_OK;
}

HIR_ERR xsvc_cas_IrLoaderGetLoaderMajor(HUINT8 *pLoaderMajor)
{
	*pLoaderMajor = s_stBBCB.ucLoaderMajor;

	return HIR_OK;
}

HIR_ERR xsvc_cas_IrLoaderGetLoaderMinor(HUINT8 *pLoaderMinor)
{
	*pLoaderMinor = s_stBBCB.ucLoaderMinor;

	return HIR_OK;
}

HIR_ERR xsvc_cas_IrLoaderGetLoaderType(HUINT8 *pLoaderMinor)
{
	*pLoaderMinor = s_stBBCB.ucLoaderType;

	return HIR_OK;
}





#if defined(CONFIG_MW_CAS_IRDETO_SECURE)
HIR_ERR xsvc_cas_IrSecureChipGetCSSN(HUINT32 *pwCSSN)
{
	svc_cas_PalPortSysGetChipId((HUINT32 *)pwCSSN);
	HxLOG_Print(" pwCSSN : %d\n", (HUINT32)*pwCSSN);

	return HIR_OK;
}

#endif



#if defined(CONFIG_MW_CAS_IRDETO_SECURE)

/**********************************************************************************
*         Not used Function for 2010.05.06
***********************************************************************************/
HIR_ERR	CAS_IR_SECURECHIP_SetVirtualCWMode(HUINT8 ulcwMode)
{
	if(ulcwMode != 0 && ulcwMode != 1)
		return HIR_NOT_OK;

	s_virtualCwMode = ulcwMode;
	return HIR_OK;
}

/**********************************************************************************
*         Not used Function for 2010.05.06
***********************************************************************************/
HIR_ERR	CAS_IR_SECURECHIP_GetVirtualCWMode(HUINT8* pulcwMode)
{
	if(pulcwMode == NULL)
		return HIR_NOT_OK;

	*pulcwMode = s_virtualCwMode;
	return HIR_OK;
}

HIR_ERR CAS_IR_SECURECHIP_UpdateFusingInfo(void)
{
	HUINT8 ucSCS, ucJTAG, ucOTP, ucCWE;

	if (svc_cas_PalPortSysGetFusingInfo(&ucSCS, &ucJTAG, &ucOTP, &ucCWE) != ERR_OK)
	{
		HxLOG_Info("[CAS_IR_SECURECHIP_UpdateFusingInfo] fail to get fuse statue\n");
		return HIR_NOT_OK;
	}

	fusingInfo_SCS = (ia_word32)ucSCS;
	fusingInfo_JTAG = (ia_word32)ucJTAG;
	fusingInfo_OTP = (ia_word32)ucOTP;
	fusingInfo_CWE = (ia_word32)ucCWE;

	return HIR_OK;
}

HIR_ERR CAS_IR_SECURECHIP_GetCrypto(ia_byte *pbCrypto)
{
/*
	0 : Not allowed
	1 : TDES
	Others - Reserved
	this data got from Irdeto Korea.
*/
	*pbCrypto = 1;
	HxLOG_Print(" pbCrypto : %d\n", *pbCrypto);
	return HIR_OK;
}

HIR_ERR CAS_IR_SECURECHIP_GetRsaBoot(ia_word32 *pwRsaBoot)
{
	*pwRsaBoot  = fusingInfo_SCS;
	HxLOG_Print("CAS_IR_SECURECHIP_GetRsaBoot : %d\n", (HUINT32)*pwRsaBoot);
	return HIR_OK;
}

HIR_ERR CAS_IR_SECURECHIP_GetJtag(ia_word32 *pwJtag)
{
	*pwJtag = fusingInfo_JTAG;
	HxLOG_Print("CAS_IR_SECURECHIP_GetJtag : %d\n", (HUINT32)*pwJtag);

	return HIR_OK;
}

HIR_ERR CAS_IR_SECURECHIP_GetCwMode(ia_word32 *pwCwMode)
{
	if(s_virtualCwMode == 1)
	{
		*pwCwMode = s_virtualCwMode;
		return HIR_OK;
	}

	*pwCwMode = fusingInfo_CWE;
	HxLOG_Print("CAS_IR_SECURECHIP_GetCwMode : %d\n", (HUINT32)*pwCwMode);

	return HIR_OK;
}
#endif


#define _____IRDETO_OTA______


//SVC_SYS_SetOtaTuningInfo
/**********************************************************************************
*         Not used Function for 2010.05.06
***********************************************************************************/
HIR_ERR  xsvc_cas_IrOtaSystemReboot(HUINT32 ulMiliSecMargin)
{
	HxLOG_Print("ulMiliSecMargin = %d\n", ulMiliSecMargin);

	VK_TASK_Sleep(ulMiliSecMargin);

// TODO: shko make  CARM_SYS_Reboot function in cas2
	return HIR_OK;
}

// just for ld dvt app
/**********************************************************************************
*         Not used Function for 2010.05.06
***********************************************************************************/
HIR_ERR CAS_IR_OTA_ReadFlags(HUINT16 *pusDownloadServicePID, HUINT8 *pucTableID)
{
	HIR_ERR hir;

	hir  = CAS_IR_NVR_Partition_Read(IRNVR_PART_LOADER, IR_PARTITION_LOADER_OFFSET_wServicePID, (HUINT8 *)pusDownloadServicePID, sizeof(HUINT16) );
	hir |= CAS_IR_NVR_Partition_Read(IRNVR_PART_LOADER, IR_PARTITION_LOADER_OFFSET_ucDnldTableID, pucTableID, sizeof(HUINT8) );
	if(hir != HIR_OK)
	{
		HxLOG_Print("CAS_IR_NVR_Partition_Read:error\n");
		return HIR_NOT_OK;
	}
	HxLOG_Print("pusDownloadServicePID = 0x%X, pucTableID = 0x%X\n", *pusDownloadServicePID,  *pucTableID);
	return HIR_OK;
}

HIR_ERR xsvc_cas_IrOtaReadOTAInfo(HUINT16 *pusDownloadServicePID, HUINT8 *pucTableID)
{
	return CAS_IR_OTA_ReadFlags(pusDownloadServicePID, pucTableID);
}

HIR_ERR xsvc_cas_IrOtaClearOTAInfo(void)
{
	HIR_ERR		hir;
	HUINT8		wSvcPID = 0;
	HUINT8		ucTbID = 0;

	hir  = CAS_IR_NVR_Partition_Write(IRNVR_PART_LOADER, IR_PARTITION_LOADER_OFFSET_wServicePID, &wSvcPID, sizeof(HUINT16) );
	hir |= CAS_IR_NVR_Partition_Write(IRNVR_PART_LOADER, IR_PARTITION_LOADER_OFFSET_ucDnldTableID, &ucTbID, sizeof(HUINT8) );
	if(hir != HIR_OK)
	{
		HxLOG_Print("CAS_IR_NVR_Partition_Write:error\n");
		return HIR_NOT_OK;
	}
	return HIR_OK;
}

HIR_ERR xsvc_cas_IrOtaClearOTATuningInfo(void)
{
#if defined(CONFIG_MW_CH_SATELLITE)
	HIR_ERR		hir;
	CH_SAT_TuneParam_t stChannelParam;

	memset(&stChannelParam, 0x00, sizeof(CH_SAT_TuneParam_t));
	hir = CAS_IR_NVR_Partition_Write(IRNVR_PART_LOADER, IR_PARTITION_LOADER_OFFSET_DI_CHANNEL_PARAM, (HUINT8 *)&stChannelParam, sizeof(CH_SAT_TuneParam_t) );

	if(hir != HIR_OK)
	{
		HxLOG_Print("CAS_IR_NVR_Partition_Write:error\n");
		return HIR_NOT_OK;
	}
	return HIR_OK;
#else
	return HIR_OK;
#endif
}


HIR_ERR xsvc_cas_IrOtaSetFlagUp(HBOOL flagDoOTA, HUINT16 usDownloadServicePID, HUINT8 ucTableID)
{
	HIR_ERR hir;
	HUINT8 ucOtaFlag ;
	HUINT16 wSvcPID = usDownloadServicePID;
	HUINT8 ucTbID = ucTableID;

	HxLOG_Print("flagDoOTA = %d, usDownloadServicePID = 0x%X, ucTableID = 0x%X\n", flagDoOTA,  usDownloadServicePID,  ucTableID);

	if(TRUE !=  flagDoOTA)
	{
		ucOtaFlag =0;
		hir = CAS_IR_NVR_Partition_Write(IRNVR_PART_LOADER, IR_PARTITION_LOADER_OFFSET_ucSWDLFlag, &ucOtaFlag, sizeof(HUINT8) );
		if(hir != HIR_OK)
		{
			HxLOG_Print("CAS_IR_NVR_Partition_Write:error\n");
			return HIR_NOT_OK;
		}
		return HIR_OK;
	}

	ucOtaFlag = 1;
	hir  = CAS_IR_NVR_Partition_Write(IRNVR_PART_LOADER, IR_PARTITION_LOADER_OFFSET_wServicePID, (HUINT8 *)&wSvcPID, sizeof(HUINT16) );
	hir |= CAS_IR_NVR_Partition_Write(IRNVR_PART_LOADER, IR_PARTITION_LOADER_OFFSET_ucDnldTableID, &ucTbID, sizeof(HUINT8) );
	hir |= CAS_IR_NVR_Partition_Write(IRNVR_PART_LOADER, IR_PARTITION_LOADER_OFFSET_ucSWDLFlag, &ucOtaFlag, sizeof(HUINT8) );
	if(hir != HIR_OK)
	{
		HxLOG_Print("CAS_IR_NVR_Partition_Write:error\n");
		return HIR_NOT_OK;
	}
	return HIR_OK;
}
//it's used for dvt app
/**********************************************************************************
*         Not used Function for 2010.05.06
***********************************************************************************/
HIR_ERR xsvc_cas_IrOtaReadChannelParam( HUINT32 ulTunerId, void * pvDiChannel_param)
{
#if defined(CONFIG_MW_CH_SATELLITE)
	CH_SAT_TuneParam_t stChannelParam;
#elif defined(CONFIG_MW_CH_CABLE)
	CH_CAB_TuneParam_t stChannelParam;
#elif defined(CONFIG_MW_CH_TERRESTRIAL)
	CH_TER_TuneParam_t stChannelParam;
#endif
	HIR_ERR hir;
	HUINT32 ulTunerChannel = 	ulTunerId;

	HxLOG_Print("ulTunerId = %d\n", ulTunerId );

	hir = CAS_IR_NVR_Partition_Read(IRNVR_PART_LOADER, IR_PARTITION_LOADER_OFFSET_ulTunerID, (HUINT8 *)&ulTunerChannel, sizeof(HUINT32) );
	if(hir != HIR_OK)
	{
		HxLOG_Print("IR_NVR_Partition_read:error\n");
		return HIR_NOT_OK;
	}

#if defined(CONFIG_MW_CH_SATELLITE)
	hir = CAS_IR_NVR_Partition_Read(IRNVR_PART_LOADER, IR_PARTITION_LOADER_OFFSET_DI_CHANNEL_PARAM, (HUINT8 *)&stChannelParam, sizeof(CH_SAT_TuneParam_t) );
	if(hir != HIR_OK)
	{
		HxLOG_Print("IR_NVR_Partition_read:error\n");
		return HIR_NOT_OK;
	}
	*((CH_SAT_TuneParam_t *)pvDiChannel_param ) = stChannelParam;

#elif defined(CONFIG_MW_CH_CABLE)
	hir = CAS_IR_NVR_Partition_Read(IRNVR_PART_LOADER, IR_PARTITION_LOADER_OFFSET_DI_CHANNEL_PARAM, &stChannelParam, sizeof(CH_CAB_TuneParam_t) );
	if(hir != HIR_OK)
	{
		HxLOG_Print("IR_NVR_Partition_read:error\n");
		return HIR_NOT_OK;
	}
	*((CH_CAB_TuneParam_t *)pvDiChannel_param ) = stChannelParam;
#elif defined(CONFIG_MW_CH_TERRESTRIAL)
	hir = CAS_IR_NVR_Partition_Read(IRNVR_PART_LOADER, IR_PARTITION_LOADER_OFFSET_DI_CHANNEL_PARAM, &stChannelParam, sizeof(CH_TER_TuneParam_t) );
	if(hir != HIR_OK)
	{
		HxLOG_Print("IR_NVR_Partition_read:error\n");
		return HIR_NOT_OK;
	}
	*((CH_TER_TuneParam_t *)pvDiChannel_param ) = stChannelParam;
#else
#error "current irdeto doesn't support this channel type"
#endif
	return HIR_OK;
}


HIR_ERR xsvc_cas_IrOtaSetChannelParam( HUINT32 ulTunerId, void * pvDiChannel_param)
{
#if defined(CONFIG_MW_CH_SATELLITE)
	CH_SAT_TuneParam_t stChannelParam;
#elif defined(CONFIG_MW_CH_CABLE)
	CH_CAB_TuneParam_t stChannelParam;
#elif defined(CONFIG_MW_CH_TERRESTRIAL)
	CH_TER_TuneParam_t stChannelParam;
#endif
	HIR_ERR hir;
	HUINT32 ulTunerChannel = 	ulTunerId;

	HxLOG_Print("ulTunerId = %d, pvDiChannel_param = 0x%X \n", ulTunerId,  (HUINT32)pvDiChannel_param );

	hir = CAS_IR_NVR_Partition_Write(IRNVR_PART_LOADER, IR_PARTITION_LOADER_OFFSET_ulTunerID, (HUINT8 *)&ulTunerChannel, sizeof(HUINT32) );
	if(hir != HIR_OK)
	{
		HxLOG_Print("CAS_IR_NVR_Partition_Write:error\n");
		return HIR_NOT_OK;
	}

#if defined(CONFIG_MW_CH_SATELLITE)
	stChannelParam = *((CH_SAT_TuneParam_t *) pvDiChannel_param );
	hir = CAS_IR_NVR_Partition_Write(IRNVR_PART_LOADER, IR_PARTITION_LOADER_OFFSET_DI_CHANNEL_PARAM, (HUINT8 *)&stChannelParam, sizeof(CH_SAT_TuneParam_t) );
	if(hir != HIR_OK)
	{
		HxLOG_Print("CAS_IR_NVR_Partition_Write:error\n");
		return HIR_NOT_OK;
	}
#elif defined(CONFIG_MW_CH_CABLE)
	stChannelParam = *((CH_CAB_TuneParam_t *) pvDiChannel_param );
	hir = CAS_IR_NVR_Partition_Write(IRNVR_PART_LOADER, IR_PARTITION_LOADER_OFFSET_DI_CHANNEL_PARAM, (HUINT8 *)&stChannelParam, sizeof(CH_CAB_TuneParam_t) );
	if(hir != HIR_OK)
	{
		HxLOG_Print("CAS_IR_NVR_Partition_Write:error\n");
		return HIR_NOT_OK;
	}
#elif defined(CONFIG_MW_CH_TERRESTRIAL)
	stChannelParam = *((CH_TER_TuneParam_t *) pvDiChannel_param );
	hir = CAS_IR_NVR_Partition_Write(IRNVR_PART_LOADER, IR_PARTITION_LOADER_OFFSET_DI_CHANNEL_PARAM, (HUINT8 *)&stChannelParam, sizeof(CH_TER_TuneParam_t) );
	if(hir != HIR_OK)
	{
		HxLOG_Print("CAS_IR_NVR_Partition_Write:error\n");
		return HIR_NOT_OK;
	}
#else
#error "current irdeto doesn't support this channel type"
#endif

	return HIR_OK;
}

#define _____MENU_____
#define _____MAIL_MESSAGE_____
HIR_ERR	xsvc_cas_IrGetMailList( HUINT8 *pucNumberOfSavedMail, HUINT8 *paucMailIndexList)
{
	HxLOG_Info("\n");

	CAS_IR_MSG_DRV_GetMailInfo(pucNumberOfSavedMail, paucMailIndexList);

	if( NULL != pucNumberOfSavedMail  )
	{
		return HIR_OK;
	}

	return HIR_NOT_OK;
}

HIR_ERR	xsvc_cas_IrGetMailBox( HUINT8 ucMailIndex, MAILBOX_TABLE *pMailBox )
{
	HxLOG_Info("ucMailIndex: %d \n", ucMailIndex);

	if( TRUE != CAS_IR_MSG_DRV_GetMailBox(ucMailIndex, pMailBox) )
	{
		HxLOG_Print("CAS_IR_MSG_DRV_GetMailBox error\n");
		return HIR_NOT_OK;
	}

	return HIR_OK;
}

HBOOL xsvc_cas_IrCheckValidOfMail( HUINT8 ucMailIndex)
{
	HUINT16	usValid;

	HxLOG_Info("\n");


	if( TRUE != CAS_IR_MSG_DRV_GetValid(ucMailIndex, &usValid) )
	{
		HxLOG_Print("CAS_IR_MSG_DRV_GetValid error\n");
		return FALSE;
	}

	if( IR_MAIL_VALID_OK != usValid )
	{
		HxLOG_Print("CAS_IR_MSG_DRV_GetValid error\n");
		return FALSE;
	}

	return TRUE;
}

HBOOL	xsvc_cas_IrGetMailReadState( HUINT8 ucMailIndex )
{
	HUINT16 	sRead;

	HxLOG_Info("\n");

	if( TRUE != CAS_IR_MSG_DRV_GetReadState(ucMailIndex, &sRead) )
	{
		HxLOG_Print("CAS_IR_MSG_DRV_GetReadState error\n");
		return FALSE;
	}

	if ( IR_MAIL_READ_STATUS_READ != sRead)
	{
		HxLOG_Print("not read state\n");
		return FALSE;
	}

	return TRUE;
}

HIR_ERR	xsvc_cas_IrSetMailReadState( HUINT8 ucMailIndex, HBOOL bRead )
{
	HUINT16 	sReadStatus;

	HxLOG_Info("\n");

	if (TRUE == bRead)
	{
		sReadStatus = IR_MAIL_READ_STATUS_READ;
	}
	else
	{
		sReadStatus = IR_MAIL_READ_STATUS_NOT_READ;
	}

	if( TRUE != CAS_IR_MSG_DRV_SetReadState(ucMailIndex, sReadStatus) )
	{
		return HIR_NOT_OK;
	}

	return HIR_OK;
}

HUINT8	xsvc_cas_IrDoYouHaveUnreadMail(void)
{
	HUINT8 bUnreadMailExist;

	HxLOG_Info("\n");

	bUnreadMailExist = CAS_IR_MSG_DRV_DoYouHaveUnreadMail();

	return bUnreadMailExist;
}

HIR_ERR	xsvc_cas_IrDeleteMail(HUINT8   ucMailIndex)
{
	HxLOG_Info("\n");

	CAS_IR_MSG_DRV_DeleteMail(ucMailIndex);

	return HIR_OK;
}

void xsvc_cas_IrAllClearExpiredNormalAttributeMail(void)
{
	HxLOG_Info("\n");

	CAS_IR_MSG_DRV_AllClearExpiredNormalAttributeMail();
}

/* this api is used only for factory reset if there op request are */
HIR_ERR CAS_IR_MAIL_EraseFile(void)
{
	HxLOG_Info("\n");
	return CAS_IR_NVR_FlashEraseMailFile();
}


#define _____FINGER_PRINT_____
/**********************************************************************************
*         Not used Function for 2010.05.06
***********************************************************************************/
HIR_ERR	xsvc_cas_IrGetFingerPrintCode(HUINT8 ucScSlotId, HINT8 *pnCode)	// null-terminated
{
	IR_API_SC_INFO	IrScInfo;

	HxLOG_Info("\n");

	xsvc_cas_IrScInfo( ucScSlotId, &IrScInfo);

	HxSTD_memcpy(pnCode, IrScInfo.szHashedID, IR_SC_INFO_ID_LENGTH);

	return HIR_OK;
}



#define _____MATURITY_RATING_____
#if defined(CONFIG_MW_CAS_IRDETO_MATURITY_RATING)
/********************************************************************
*	function name :	 CAS_IR_MR_UpdatePIN
*	input : . play handel, decimal subscriber or user pin.
*	output :
*	description : for update services' maturity rating resource.
*         Not used Function for 2010.05.06
********************************************************************/
HINT32 xsvc_cas_IrMrUpdatePIN(Handle_t hAction, HUINT8 * aucDecimalPIN)
{
	HUINT8 i , aucBCDPIN[4];
	HUINT8 *p = aucDecimalPIN;

	/* translate decimal into BCD format value. */
	for(i=0; i< 4; i++)
	{
		if(aucDecimalPIN[i] > 0x09)
		{
			HxLOG_Print("[API_IR] MR_UpdatePIN: Error: pin[%d]=%d input range is out of valid.\n", i, aucDecimalPIN[i] );
			return HIR_NOT_OK;
		}
	}

	aucBCDPIN[0]= (0x10 * aucDecimalPIN[0]) |(aucDecimalPIN[1] * 0x01);
	aucBCDPIN[1]= (0x10 * aucDecimalPIN[2]) |(aucDecimalPIN[3] * 0x01);

	HxLOG_Print("[IR_API] CAS_IR_MR_UpdatePIN:aucBCDPIN[0] = 0x%02X, aucBCDPIN[1] = 0x%02X\n", aucBCDPIN[0], aucBCDPIN[1]);

	if(HIR_OK != CAS_IR_DRV_MR_UpdateResource(hAction, aucBCDPIN) )
	{
		HxLOG_Print("[API_IR] MR_UpdatePIN: Unkown handle(hAction=0x%X) Error:\n", hAction);
		return HIR_NOT_OK;
	}

	return HIR_OK;
}


/********************************************************************
*	function name :	 CAS_IR_MR_ReadAllUserPF
*	input : . play handel, only decimal subscriber pin
*	output :
*	description : It's query api for user profiles in smartcards.
*         Not used Function for 2010.05.06
********************************************************************/
HINT32 xsvc_cas_IrMrReadAllUserPF(HUINT32 resourceId, HUINT8 * aucDecimalPIN)
{
	HUINT8 	i, aucBCDPIN[2];
	msg_mr_user_profile_data_st stUserProfile;
	IR_API_SC_INFO *pstSCinfo;

	/* translate decimal into BCD format value. */
	for(i=0; i< 4; i++)
	{
		if(aucDecimalPIN[i]  > 0x09)
		{
			HxLOG_Print("[API_IR] CAS_IR_MR_ReadAllUserPF: Error: pin[%d] input[0x%X]  is out of valid range.\n", i, aucDecimalPIN[i]);
			return HIR_NOT_OK;
		}
	}

	aucBCDPIN[0]= (0x10 * aucDecimalPIN[0]) |(aucDecimalPIN[1] * 0x01);
	aucBCDPIN[1]= (0x10 * aucDecimalPIN[2]) |(aucDecimalPIN[3] * 0x01);

	HxLOG_Print("[IR_API] MR_ReadAllUserPF:aucBCDPIN[0] = 0x%02X, aucBCDPIN[1] = 0x%02X\n", aucBCDPIN[0], aucBCDPIN[1] );

	/* To Read all users profile at once, do try read profile on index 0, if subscriber pin is corret on the fisrt trying read next profiles by index 6.*/
	stUserProfile.bProfileID = 0;

	XSVC_CAS_IR_DRV_MR_UserProfileQuery(resourceId, aucBCDPIN, IA_MR_USER_PROFILE_READ , &stUserProfile);

	return HIR_OK;
}

/********************************************************************
*	function name :	 CAS_IR_MR_SetUserPF
*	input : .
*	output :
*	description : It's query api for set new user profiles in smartcards.
*         Not used Function for 2010.05.06
********************************************************************/
HINT32 xsvc_cas_IrMrSetUserPF(HUINT8 ucUserIdx, HUINT8 ucAge, HUINT8 * aucDecimalPIN)
{
	HUINT8 	i, aucBCDPIN[2];
	msg_mr_user_profile_data_st stUserProfile;
	IR_API_SC_INFO *pstSCinfo;

	/* translate decimal into BCD format value. */
	for(i=0; i< 4; i++)
	{
		if(aucDecimalPIN[i]  > 0x09)
		{
			HxLOG_Print("[API_IR] CAS_IR_MR_SetUserPF: Error: pin[%d] input[0x%X]  is out of valid range.\n", i, aucDecimalPIN[i]);
			return HIR_NOT_OK;
		}
	}

	aucBCDPIN[0]= (0x10 * aucDecimalPIN[0]) |(aucDecimalPIN[1] * 0x01);
	aucBCDPIN[1]= (0x10 * aucDecimalPIN[2]) |(aucDecimalPIN[3] * 0x01);

	HxLOG_Print("[IR_API] CAS_IR_MR_SetUserPF:aucBCDPIN[0] = 0x%02X, aucBCDPIN[1] = 0x%02X\n", aucBCDPIN[0], aucBCDPIN[1] );

	stUserProfile.bProfileID = ucUserIdx;
	stUserProfile.bUserAge= ucAge;

	CAS_IR_DRV_MR_UserProfileQuery(0, aucBCDPIN, IA_MR_USER_PROFILE_SET , &stUserProfile);

	return HIR_OK;
}

/********************************************************************
*	function name :	 CAS_IR_MR_DeleteUserPF
*	input : .
*	output :
*	description : It's query api for delete user profiles in smartcards.
*         Not used Function for 2010.05.06
********************************************************************/
HINT32 xsvc_cas_IrMrDeleteUserPF(HUINT8 ucUserIdx, HUINT8 * aucDecimalPIN)
{
	HUINT8 	i, aucBCDPIN[2];
	msg_mr_user_profile_data_st stUserProfile;
	IR_API_SC_INFO *pstSCinfo;

	/* translate decimal into BCD format value. */
	for(i=0; i< 4; i++)
	{
		if(aucDecimalPIN[i]  > 0x09)
		{
			HxLOG_Print("[API_IR] CAS_IR_MR_DeleteUserPF: Error: pin[%d] input[0x%X]  is out of valid range.\n", i, aucDecimalPIN[i]);
			return HIR_NOT_OK;
		}
	}

	aucBCDPIN[0]= (0x10 * aucDecimalPIN[0]) |(aucDecimalPIN[1] * 0x01);
	aucBCDPIN[1]= (0x10 * aucDecimalPIN[2]) |(aucDecimalPIN[3] * 0x01);

	HxLOG_Print("[IR_API] CAS_IR_MR_DeleteUserPF:aucBCDPIN[0] = 0x%02X, aucBCDPIN[1] = 0x%02X\n", aucBCDPIN[0], aucBCDPIN[1] );

	stUserProfile.bProfileID = ucUserIdx;

	XSVC_CAS_IR_DRV_MR_UserProfileQuery(0, aucBCDPIN, IA_MR_USER_PROFILE_DELETE, &stUserProfile);

	return HIR_OK;
}

#endif

#if defined(CONFIG_MW_CAS_IRDETO_HOMINGCHANNEL)
#if defined(CONFIG_OP_ALMAJD)
HUINT32 xsvc_cas_IrGetAlmajdHomingChannel (HUINT8 ucSlot, IR_SC_HOME_CHANNEL_INFO *pHomeChannel)
{
	HIR_ERR	hError;
	HUINT8		data[IR_PARTION_CUSTOM_HOME_TP0_SIZE];
	HUINT8		*p;
	HUINT8		ucTemp;
	HUINT8		ucVal;
	HUINT32	dwTemp;



	if (pHomeChannel == NULL)
	{
		return ERR_FAIL;
	}

	hError = CAS_IR_NVR_ReadHomingChannel (ucSlot, data, IR_PARTION_CUSTOM_HOME_TP0_SIZE);
	if (hError != HIR_OK)
	{
		return hError;
	}

	pHomeChannel->ucPriority = data[3] & 0x01;

	p = data;
	pHomeChannel->dwFreq =(HUINT32)CAS_IR_DRV_Bcd2Dec(CAS_IR_DRV_Get32Bits(p));
	p += 4;

	// orbital position(2byte)
	pHomeChannel->wOrbitPos =(HUINT16)CAS_IR_DRV_Bcd2Dec(CAS_IR_DRV_Get16Bits(p));
	p+= 2;

	// symbol rate (28bits)
	dwTemp = CAS_IR_DRV_Get32Bits(p);
	pHomeChannel->dwSymbolRate =(HUINT32)CAS_IR_DRV_Bcd2Dec((dwTemp & 0xfffffff0) >> 4);
	p+= 4;

	ucTemp = *p++;
	// Mux Verifier Mode(4bits)
	pHomeChannel->eMuxVerifierMode = ((ucTemp & 0xf0) >> 4);

	// FEC inner(4bits)
	pHomeChannel->eFECInner = (ucTemp & 0x0f);

	ucTemp = *p++;
	// west east flag(1bit)
	ucVal =(ucTemp & 0x80) >> 7;
	pHomeChannel->eWestEastFlag = ucVal;

	// polarization (2bits)
	pHomeChannel->ePolar =(ucTemp & 0x60) >> 5;

	// Roll-Off (2bits)
	pHomeChannel->eRollOff =(ucTemp & 0x18) >> 3;

	// Modulation System (1bit)
	pHomeChannel->eModulSystem=(ucTemp & 0x04) >> 2;

	// Modulation System (2bit)
	pHomeChannel->eModulType =(ucTemp & 0x03);

#if 0
	HxLOG_Print ("+++++++++++++++++++ Freq : %d\n", pHomeChannel->dwFreq);
	HxLOG_Print ("+++++++++++++++++++ Obit Pos : %d\n", pHomeChannel->wOrbitPos);
	HxLOG_Print ("+++++++++++++++++++ Symbol : %d\n", pHomeChannel->dwSymbolRate);
	HxLOG_Print ("+++++++++++++++++++ Mux verify : %d\n", pHomeChannel->eMuxVerifierMode);
	HxLOG_Print ("+++++++++++++++++++ FECInner : %d\n", pHomeChannel->eFECInner);
	HxLOG_Print ("+++++++++++++++++++ WestEast : %d\n", pHomeChannel->eWestEastFlag);
	HxLOG_Print ("+++++++++++++++++++ Polar : %d\n", pHomeChannel->ePolar);
	HxLOG_Print ("+++++++++++++++++++ RollOff : %d\n", pHomeChannel->eRollOff);
	HxLOG_Print ("+++++++++++++++++++ Mod system : %d\n", pHomeChannel->eModulSystem);
	HxLOG_Print ("+++++++++++++++++++ mod type : %d\n", pHomeChannel->eModulType);
	HxLOG_Print ("+++++++++++++++++++ Priority : %d\n", pHomeChannel->ucPriority);
#endif

	return ERR_OK;

}
#endif
#endif

#if defined(CONFIG_OP_ALMAJD)
HUINT32 xsvc_cas_IrGetRegionCode (HUINT16 *pusRegionCode)
{

	return CAS_IR_NVR_ReadRegionCode (pusRegionCode, 2);

}

HUINT32 xsvc_cas_IrGetFirstBoot (HUINT8 *pucFirstBoot)
{
	return CAS_IR_NVR_ReadFirstBoot (pucFirstBoot, 1);
}

HUINT32 xsvc_cas_IrResetFirstBoot (void)
{
	HUINT8 ucFirstBoot;

	ucFirstBoot = 0;
	return CAS_IR_NVR_SaveFirstBoot (&ucFirstBoot, 1);
}

HUINT32 xsvc_cas_IrReadNationality (HUINT8 *pucNationality)
{
	return CAS_IR_NVR_ReadNationality (pucNationality, 4);
}

HUINT32 xsvc_cas_IrSaveNationality (HUINT8 *pucNationality)
{
	return CAS_IR_NVR_SaveNationality (pucNationality, 4);
}

#endif

/**********************************************************************************
*         Not used Function for 2010.05.06
*		  in IRPLAT_SYS_PVR define
***********************************************************************************/
#if defined(IRPLAT_SYS_PVR)
HUINT32	xsvc_cas_IrPvrIsRecordable (Handle_t hPlay)
{
	HUINT32	nRet;
	HUINT8	ucRecordMode;

	nRet = CAS_IR_DRV_GetSvcRecordMode (hPlay, &ucRecordMode);
	if (nRet == ERR_OK)
	{
		switch (ucRecordMode)
		{
			case	IR_RECORD_TYPE_INVALID :
			case	IR_RECORD_TYPE_CLEAR:
			case	IR_RECORD_TYPE_DESCRAMBLE:
			case	IR_RECORD_TYPE_ENCRYPTION:
			case	IR_RECORD_TYPE_CHECKMSK:
				return TRUE;

			case	IR_RECORD_TYPE_FORBIDDEN:
			case	IR_RECORD_TYPE_NOMSK:
				return FALSE;
		}
	}

	return TRUE;
}

HUINT32	xsvc_cas_IrPvrIsNoMSK (Handle_t hPlay)
{
	HUINT32	nRet;
	HUINT8	ucRecordMode;

	nRet = CAS_IR_DRV_GetSvcRecordMode (hPlay, &ucRecordMode);
	if (nRet == ERR_OK)
	{
		switch (ucRecordMode)
		{
			case	IR_RECORD_TYPE_NOMSK:
				return TRUE;
			case	IR_RECORD_TYPE_INVALID :
			case	IR_RECORD_TYPE_CLEAR:
			case	IR_RECORD_TYPE_DESCRAMBLE:
			case	IR_RECORD_TYPE_ENCRYPTION:
			case	IR_RECORD_TYPE_FORBIDDEN:
			case	IR_RECORD_TYPE_CHECKMSK:
				return FALSE;
		}
	}

	return FALSE;
}

HUINT32	xsvc_cas_IrPvrIsEncryption (Handle_t hPlay)
{
	HUINT32	nRet;
	HUINT8	ucRecordMode;

	nRet = CAS_IR_DRV_GetSvcRecordMode (hPlay, &ucRecordMode);
	if (nRet == ERR_OK)
	{
		switch (ucRecordMode)
		{
			case	IR_RECORD_TYPE_ENCRYPTION:
				return TRUE;
			case	IR_RECORD_TYPE_NOMSK:
			case	IR_RECORD_TYPE_INVALID :
			case	IR_RECORD_TYPE_CLEAR:
			case	IR_RECORD_TYPE_DESCRAMBLE:
			case	IR_RECORD_TYPE_FORBIDDEN:
			case	IR_RECORD_TYPE_CHECKMSK:
				return FALSE;
		}
	}

	return FALSE;
}

HUINT32	xsvc_cas_IrPvrIsDescrambled (Handle_t hPlay)
{
	HUINT32	nRet;
	HUINT8 	ucRecordMode;

	nRet = CAS_IR_DRV_GetSvcRecordMode (hPlay, &ucRecordMode);
	if (nRet == ERR_OK)
	{
		switch (ucRecordMode)
		{
			case	IR_RECORD_TYPE_INVALID:
			case	IR_RECORD_TYPE_CLEAR:
			case	IR_RECORD_TYPE_DESCRAMBLE:
			case	IR_RECORD_TYPE_CHECKMSK:
				return TRUE;
			case	IR_RECORD_TYPE_ENCRYPTION:
			case	IR_RECORD_TYPE_FORBIDDEN:
			case	IR_RECORD_TYPE_NOMSK:
				return FALSE;
		}
	}

	return FALSE;
}

#endif

/**********************************************************************************
*         Not used Function for 2010.05.06
***********************************************************************************/
HIR_ERR	xsvc_cas_IrGetSvcCasKind(Handle_t hAction, HUINT8 *ucSvcCasKind)
{
#if 0	// ewlee 20100816
	HUINT32 ret;
	HUINT16 *ausPids;
	HUINT32 ulCaPidNum=0;
	eIrDrvCaType eSvcCasKind;

	HxLOG_Info("\n") );

	ret = CAS_IR_DRV_GetSvcCasKind(hAction, &eSvcCasKind);
	if(ret != HIR_OK)
	{
		*ucSvcCasKind = eIrDrvCaType_None;
		HxLOG_Print("[xsvc_cas_IrGetSvcCasKind]ERROR in CAS_IR_DRV_GetSvcCasKind ret=0x%x\n", ret);
		return	HIR_NOT_OK;
	}

	*ucSvcCasKind = eSvcCasKind;
#endif
	return HIR_OK;
}


#define _____SOL_____
/********************************************************************
*	function name :	 CAS_IR_ActivateSOLAtBootup
*	input : usBatLen, usNitLen은 table id를 포함한 raw section pointer의 전체 길이임.
*	output :
*	description :
*         Not used Function for 2010.05.06
********************************************************************/
HINT32 CAS_IR_ActivateSOLAtBootup(HUINT8 *pucBat, HUINT16 usBatLen, HUINT8* pucNit, HUINT16 usNitLen)
{
	HxLOG_Info("\n");

#if defined(IRPLAT_FN_SOL)
	if((pucBat != NULL) && (usBatLen > 0))
		CAS_IR_CAI_UpdateBat(0, pucBat, usBatLen);

	if((pucNit != NULL) && (usNitLen > 0))
		CAS_IR_CAI_UpdateNit(0, pucNit, usNitLen);
#endif
	return HIR_OK;
}

#if defined(CONFIG_OP_DIGITURK)
HERROR xsvc_cas_IrSetStandbyFlag(HUINT8 ucFlag)
{
	HIR_ERR hir;

	HxLOG_Info("flag [%d]\n", ucFlag);

	hir = CAS_IR_NVR_Partition_Write(IRNVR_PART_LOADER, IR_PARTITION_LOADER_OFFSET_ucStandbyFlag, &ucFlag, sizeof(HUINT8) );
	if(hir != HIR_OK)
	{
			return ERR_FAIL;
	}
	return ERR_OK;
}

HERROR xsvc_cas_IrSetLanguageCode(HUINT8 *szLangCode)
{
	HIR_ERR hir;

	HxLOG_Info("lang [%s]\n", szLangCode);

	hir = CAS_IR_NVR_Partition_Write(IRNVR_PART_LOADER, IR_PARTITION_LOADER_OFFSET_szLanguageCode, szLangCode, sizeof(HUINT8)*3 );
	if(hir != HIR_OK)
	{
			return ERR_FAIL;
	}
	return ERR_OK;
}

HERROR xsvc_cas_IrSetErrorCode(HUINT32 ulErrorCode)
{
	HIR_ERR hir;

	hir = CAS_IR_NVR_Partition_Write(IRNVR_PART_LOADER, IR_PARTITION_LOADER_OFFSET_ulLastErrorCode, (HUINT8*)&ulErrorCode, sizeof(HUINT32) );
	if(hir != HIR_OK)
	{
			return ERR_FAIL;
	}
	return ERR_OK;
}

HERROR xsvc_cas_IrGetErrorCode(HUINT32 *pulErrorCode)
{
	HIR_ERR hir;

	if(pulErrorCode == NULL)
	{
		return ERR_FAIL;
	}

	hir = CAS_IR_NVR_Partition_Read(IRNVR_PART_LOADER, IR_PARTITION_LOADER_OFFSET_ulLastErrorCode, (HUINT8*)pulErrorCode, sizeof(HUINT32) );
	if(hir != HIR_OK)
	{
			return ERR_FAIL;
	}
	return ERR_OK;
}
#endif

// ewlee 20100816 - this API is basically for checking whether the current channel is scrambled by Softcell or UC.
// However, we do not know how to detect yet.
HUINT16 xsvc_cas_IrGetSvcType(void *pHandle)
{
#if defined (CONFIG_MW_CAS_IRDETO_UCAS)
	return IR_CAS_TYPE_UC;
#endif
#if defined (CONFIG_MW_CAS_IRDETO_SCELL)
	return IR_CAS_TYPE_SCELL;
#endif

	return IR_CAS_TYPE_NONE;
}

#define _____GLOBAL_____
AXI_STRING_IDX	xsvc_cas_IrDrvGetStringIdxFromErrorCode(IR_STATE *pStatus)
{
#if defined (CONFIG_MW_CAS_IRDETO_SCELL) && !defined(CONFIG_MW_CAS_IRDETO_UCAS)
	ia_status_st iaStatus;

	HxLOG_Info("\n");

	if(pStatus == NULL)
	{
		HxLOG_Print("null pointer input\n");
		return AXI_STATUSCODE_IDX_None;
	}

	iaStatus.eSeverity = pStatus->eSeverity;
	iaStatus.eSource = pStatus->eSource;
	iaStatus.wStatus = pStatus->usStatus;

	return	CAS_IR_DRV_GetErrorCodeIndex(&iaStatus);
#else
	return 0;
#endif
}

#define _____SMARTCARD_STATUS_____
HIR_ERR xsvc_cas_IrScInfo(HUINT8 ucScSlotId, IR_API_SC_INFO *pstIrApiScInfo)
{
#if defined (CONFIG_MW_CAS_IRDETO_SCELL) && !defined(CONFIG_MW_CAS_IRDETO_UCAS)
	IR_DRV_SC_INFO stIrDrvScInfo;

	HxLOG_Info("[xsvc_cas_IrScInfo] ucScSlotId: %d\n", ucScSlotId);

	XSVC_CAS_IR_DRV_GetScInfo(ucScSlotId, &stIrDrvScInfo);

	/*map with structure */
	*pstIrApiScInfo = stIrDrvScInfo.stIrScApiInfo;

	return HIR_OK;
#else
	return HIR_NOT_OK;
#endif
}

#if defined(CONFIG_OP_DIGITURK)
HIR_ERR XSVC_CAS_IR_SC_HashCode(HUINT8 ucScSlotId, HUINT8 *pszHashCode)
{
#if defined (CONFIG_MW_CAS_IRDETO_SCELL) && !defined(CONFIG_MW_CAS_IRDETO_UCAS)

	if(pszHashCode != NULL)
	{
		HUMAX_VdSc_HashCodeConvert();
		return HIR_OK;
	}
#endif
	return HIR_NOT_OK;
}
#endif

/* () 가 없는 smart card number 값 */
HIR_ERR xsvc_cas_IrScNumber(HUINT8 ucScSlotId, HUINT8 *pNumber)
{
	IR_DRV_SC_INFO 	stIrDrvScInfo;

	if (pNumber == NULL)
	{
		return HIR_NOT_OK;
	}

	XSVC_CAS_IR_DRV_GetScInfo(ucScSlotId, &stIrDrvScInfo);
	if(stIrDrvScInfo.CardPhysicalStatus == eCAS_SCI_SC_PRESENT)
	{
		HxSTD_memcpy (pNumber, stIrDrvScInfo.stIrScApiInfo.szNumber, IR_SC_INFO_NUMBER_LENGTH);
		pNumber[10] = pNumber[12];
		pNumber[11] = '\0';
		return HIR_OK;
	}
	else
	{
		return HIR_NOT_OK;
	}
}



#define _____SOL_____
#if defined(CONFIG_MW_CAS_IRDETO_SOL)
SOL_SERVICE_STATE	xsvc_cas_IrGetSolStatus(Handle_t hAction, HUINT16 ONID, HUINT16 TSID)
{
	HBOOL				bSOL = FALSE;

	DB_PARAM_GetItem(MENUCFG_IR_SOL_SETTING, (HUINT32 *)&bSOL, 0);

	if(bSOL == TRUE)
	{
		return CAS_IR_SOL_is_service_blocked(hAction, ONID, TSID);
	}
	else
	{
		return SOL_VISIBLE_SERVICE;
	}
}
#endif

#if defined(CONFIG_MW_CAS_IRDETO_FTA_BLOCK)
HUINT32	xsvc_cas_IrGetFTABlockStatus(Handle_t hAction)
{
	return CAS_IR_FTA_BLOCK_is_service_blocked (hAction);
}
#endif

/********************************************************************************
********************************************************************************/
AXI_STATUSCODE_IDX	CAS_IR_DRV_GetErrorCodeIndex(ia_status_st *pStatus)
{
	unsigned long src = pStatus->eSource;
	unsigned short wStatus= pStatus->wStatus;

	HxLOG_Info("\n");

	switch(pStatus->eSeverity)
	{
	case STATUS_FATAL:

		switch(src)
		{
			case SOURCE_SOFTCELL:
				if(wStatus == 101)		return AXI_STATUSCODE_IDX_F101_2;
				else if(wStatus == 102)	return AXI_STATUSCODE_IDX_F102_2;
				break;
		}
		HxLOG_Print("MUST be CHECKED !!!(pStatus->eSource= %d, wStatus =%d)\n",(HINT32) src , (HINT32)wStatus);
		return AXI_STATUSCODE_IDX_None;

	case STATUS_ERROR:
	{
		switch(src)
		{
			case SOURCE_SOFTCELL:
				if(wStatus == 100)	return AXI_STATUSCODE_IDX_E100_2;
				break;
			case SOURCE_SERVICES:
				if( wStatus == 101)	return AXI_STATUSCODE_IDX_E101_3;
				break;
			case SOURCE_SMARTCARD:
				switch (wStatus)
				{
					case	30:		return AXI_STATUSCODE_IDX_E30_4;
					case	4:		return AXI_STATUSCODE_IDX_E04_4;
					case	5:		return AXI_STATUSCODE_IDX_E05_4;
					case	6:		return AXI_STATUSCODE_IDX_E06_4;
					case	16:		return AXI_STATUSCODE_IDX_E16_4;
					case	18:		return AXI_STATUSCODE_IDX_E18_4;
					case	19:		return AXI_STATUSCODE_IDX_E19_4;
					case	20:		return AXI_STATUSCODE_IDX_E20_4;
					case	21:		return AXI_STATUSCODE_IDX_E21_4;
					case	32:		return AXI_STATUSCODE_IDX_E32_4;
					case	33:		return AXI_STATUSCODE_IDX_E33_4;
					case	100:	return AXI_STATUSCODE_IDX_E100_4;
					case	101:	return AXI_STATUSCODE_IDX_E101_4;
					case	107:	return AXI_STATUSCODE_IDX_E107_4;
					case	108:	return AXI_STATUSCODE_IDX_E108_4;
					case	109:	return AXI_STATUSCODE_IDX_E109_4;
					case	116:		return AXI_STATUSCODE_IDX_E116_4;
					case	117:		return AXI_STATUSCODE_IDX_E117_4;
					case	118:		return AXI_STATUSCODE_IDX_E118_4;
					case	124:	return AXI_STATUSCODE_IDX_E124_4;
					case	127:	return AXI_STATUS_CODE_IDX_E127_4;
					case	128:	return AXI_STATUS_CODE_IDX_E128_4;
					case	129:	return AXI_STATUSCODE_IDX_E129_4;
					case	130:	return AXI_STATUS_CODE_IDX_E130_4;
					case	131:	return AXI_STATUS_CODE_IDX_E131_4;
					case	132:	return AXI_STATUS_CODE_IDX_E132_4;
					case	133:	return AXI_STATUSCODE_IDX_E133_4;
					case	134:	return AXI_STATUS_CODE_IDX_E134_4;
					case	135:	return AXI_STATUS_CODE_IDX_E135_4;
					case	136:	return AXI_STATUSCODE_IDX_E136_4; // for maturity rating.
					case	137:	return AXI_STATUSCODE_IDX_E137_4; // for maturity rating.
					case	138:	return AXI_STATUSCODE_IDX_E138_4; // for maturity rating.
					case	140:		return AXI_STATUSCODE_IDX_E140_4;
					case	145:		return AXI_STATUSCODE_IDX_E145_4;
					case	151:		return AXI_STATUSCODE_IDX_E151_4;
					case 152: return AXI_STATUS_CODE_IDX_E152_4;// for preview
					case	154:		return AXI_STATUSCODE_IDX_E154_4;
					case	201:		return AXI_STATUSCODE_IDX_E201_4;

				}
				break;
			case SOURCE_MOD_ECM_SOURCE:
				if(wStatus == 104)		return AXI_STATUSCODE_IDX_E104_9;
				else if(wStatus == 106)	return AXI_STATUSCODE_IDX_E106_9;
				break;
			case SOURCE_MOD_EMM_SOURCE:
				if( wStatus == 102)		return AXI_STATUSCODE_IDX_E102_6;
				else if( wStatus == 105)	return AXI_STATUSCODE_IDX_E105_6;
				break;
			case SOURCE_CUSTOM_CA:
				if(wStatus == 80)			return AXI_STATUSCODE_IDX_E80_35;
				else if(wStatus == 81) 	return AXI_STATUSCODE_IDX_E81_35;
				break;
			case SOURCE_MOD_DVB_DESCRAMBLER:
				if(wStatus == 101) 		return AXI_STATUSCODE_IDX_E101_10;
				break;
			case SOURCE_MOD_MONITORING:
				if(wStatus == 101)		return AXI_STATUSCODE_IDX_E101_11;
				break;
			case SOURCE_IPPV:
				if(wStatus == 24)			return AXI_STATUSCODE_IDX_E24_12;
				else if(wStatus == 27) 	return AXI_STATUSCODE_IDX_E27_12;
				else if(wStatus == 28)		return AXI_STATUSCODE_IDX_E28_12;
				break;
			case SOURCE_SMARTCARD_MARRIAGE:
				if(wStatus == 17)			return AXI_STATUSCODE_IDX_E17_13;
				break;
			case SOURCE_MOD_SESSION_MANAGEMENT:
				if(wStatus == 101)		return AXI_STATUSCODE_IDX_E101_29;
				else if(wStatus == 102)	return AXI_STATUSCODE_IDX_E102_29;
				else if(wStatus == 103)	return AXI_STATUSCODE_IDX_E103_29;
				else if(wStatus == 104)	return AXI_STATUSCODE_IDX_E104_29;
				break;
			case SOURCE_MOD_SESSION_MANAGEMENT_DRM:
				switch (wStatus)
				{
					case	110:		return AXI_STATUSCODE_IDX_E110_37;
					case	111:		return AXI_STATUSCODE_IDX_E111_37;
					case	112:		return AXI_STATUSCODE_IDX_E112_37;
					case	115:		return AXI_STATUSCODE_IDX_E115_37;
					case	117:		return AXI_STATUSCODE_IDX_E117_37;
					case	118:		return AXI_STATUSCODE_IDX_E118_37;
					default:
						break;
				}
				break;
			case SOURCE_MOD_CRYPTO_OPERATION:
				if(wStatus == 101)		return AXI_STATUSCODE_IDX_E101_30;
				break;
			case SOURCE_MOD_MACROVISION:
				if(wStatus == 101)		return AXI_STATUSCODE_IDX_E101_31;
				break;

		}
		HxLOG_Print("STATUS_ERROR: MUST be CHECKED !!!(pStatus->eSource= %d, wStatus =%d)\n",(HINT32)src , (HINT32)wStatus );
		return AXI_STATUSCODE_IDX_None;
	}

	case STATUS_WARNING:
		HxLOG_Print("STATUS_WARNING:MUST be CHECKED !!!(pStatus->eSource= %d, wStatus =%d)\n",(HINT32)src ,(HINT32) wStatus);
		return AXI_STATUSCODE_IDX_None;

	case STATUS_INFO:
		switch(src)
		{
			case SOURCE_SMARTCARD:
				if(wStatus == 7 ) 		return AXI_STATUSCODE_IDX_I07_4;
				else if(wStatus == 34)	return AXI_STATUSCODE_IDX_I34_4;
				else if(wStatus == 102) 	return AXI_STATUSCODE_IDX_I102_4;
				break;
			case SOURCE_MOD_ECM_SOURCE:
				if(wStatus == 34)
				{
					HxLOG_Print("jichoi: ecm waiting !!!\n");
					return AXI_STATUSCODE_IDX_I34_9;
				}
				break;
			case SOURCE_MOD_SESSION_MANAGEMENT_DRM:
				switch (wStatus)
				{
					case	113:		return AXI_STATUSCODE_IDX_I113_37;
					case	114:		return AXI_STATUSCODE_IDX_I114_37;
				}
				break;
		}
		HxLOG_Print("STATUS_INFO: MUST be CHECKED !!!(pStatus->eSource= %d, wStatus =%d)\n",(HINT32)src , (HINT32)wStatus);
		return AXI_STATUSCODE_IDX_None;

	case STATUS_INFO_DND:
		switch(src)
		{
			case SOURCE_SERVICES:
				if(wStatus == 100) return AXI_STATUSCODE_IDX_D100_3;
				break;
			case SOURCE_SMARTCARD:
				switch (wStatus)
				{
					case	0:		return AXI_STATUSCODE_IDX_D00_4;
					case	29:		return AXI_STATUSCODE_IDX_D29_4;
					case	55:		return AXI_STATUSCODE_IDX_I55_4;
					case	126:	return AXI_STATUSCODE_IDX_D126_4;
				}
				break;
			case SOURCE_DVB_EMM_SERVICE:
				if(wStatus == 100) 		return AXI_STATUSCODE_IDX_D100_5;
				break;
			case SOURCE_MOD_EMM_SOURCE:
				if(wStatus == 100)		return AXI_STATUSCODE_IDX_D100_6;
				else if( wStatus == 101) 	return AXI_STATUSCODE_IDX_D101_6;
				else if( wStatus == 103) 	return AXI_STATUSCODE_IDX_D103_6;
				else if( wStatus == 104) 	return AXI_STATUSCODE_IDX_D104_6;
				break;
			case SOURCE_DVB_DESCRAMBLE_SERVICE:
				if(wStatus == 100)		return AXI_STATUSCODE_IDX_D100_8;
				break;
			case SOURCE_MOD_ECM_SOURCE:
				if(wStatus == 100)		return AXI_STATUSCODE_IDX_D100_9;
				else if( wStatus == 101) 	return AXI_STATUSCODE_IDX_D101_9;
				else if( wStatus == 102) 	return AXI_STATUSCODE_IDX_D102_9;
				else if( wStatus == 103) 	return AXI_STATUSCODE_IDX_D103_9;
				else if( wStatus == 105) 	return AXI_STATUSCODE_IDX_D105_9;
				break;
			case SOURCE_MOD_DVB_DESCRAMBLER:
				if(wStatus == 100)		return AXI_STATUSCODE_IDX_D100_10;
				break;
			case SOURCE_MOD_MONITORING:
				if(wStatus == 100)		return AXI_STATUSCODE_IDX_D100_11;
				break;
			case SOURCE_IPPV:
				if(wStatus == 22) 			return AXI_STATUSCODE_IDX_D22_12;
				else if( wStatus == 23) 	return AXI_STATUSCODE_IDX_D23_12;
				else if( wStatus == 25) 	return AXI_STATUSCODE_IDX_D25_12;
				else if( wStatus == 26) 	return AXI_STATUSCODE_IDX_D26_12;
				break;
			case SOURCE_PVR_RECORD_SERVICE:
				if(wStatus == 100) 		return AXI_STATUSCODE_IDX_D100_27;
				break;
			case SOURCE_PVR_PLAYBACK_SERVICE:
				if(wStatus == 100) 		return AXI_STATUSCODE_IDX_D100_28;
				break;
			case SOURCE_MOD_SESSION_MANAGEMENT:
				if(wStatus == 100)		return AXI_STATUSCODE_IDX_D100_29;
				break;
			case SOURCE_MOD_SESSION_MANAGEMENT_DRM:
				switch (wStatus)
				{
					case	116:		return AXI_STATUSCODE_IDX_D116_37;
				}
				break;
			case SOURCE_MOD_CRYPTO_OPERATION:
				if(wStatus == 100)		return AXI_STATUSCODE_IDX_D100_30;
				else if( wStatus == 102) 	return AXI_STATUSCODE_IDX_D102_30;
				break;
			case SOURCE_MOD_MACROVISION:
				if(wStatus == 100)		return AXI_STATUSCODE_IDX_D100_31;
				break;
		}
		HxLOG_Print("STATUS_INFO_DND: MUST be CHECKED !!!(pStatus->eSource= %d, wStatus =%d)\n",(HINT32)src ,(HINT32) wStatus);
		return AXI_STATUSCODE_IDX_None;

	default:
		HxLOG_Print("eSeverity=%d<--MUST be CHECKED !!!(pStatus->eSource= %d, wStatus =%d)\n",pStatus->eSeverity, (HINT32)src , (HINT32)wStatus);
		return AXI_STATUSCODE_IDX_None;
	}
	return 0;
}

#if defined(IRPLAT_SYS_SAVE_SMARTCARD_NUMBER) && defined (CONFIG_OP_ALMAJD)
AXI_STATUSCODE_IDX	CAS_IR_DRV_AlMajd_E30_GetErrorCodeIndex (void)
{
	if (CAS_IR_NVR_SavedNumberIsInitial () == TRUE)
	{
		return AXI_STATUSCODE_IDX_ALMAJD_255; // AlMajd Error, 생산 초기 상태 아직 smartcard 가 pair 되지 않았다
	}
	else if (HUMAX_VdSc_IsSameWithSavedSmartcardNumber() == FALSE)
	{
		return AXI_STATUSCODE_IDX_ALMAJD_254; // AlMajd Error, 한번 pair 가 되었는데 다른 card 를 넣었다.
	}

	return AXI_STATUSCODE_IDX_ALMAJD_240; // ECM key 가 update 되지 못했다
}
#endif


#define _____MHP_____

/********************************************************************************
********************************************************************************/

/********************************************************************************
********************************************************************************/
HUINT8	xsvc_cas_IrDrvCheckValidEvtCode(HINT32 nEvtCode)
{
	switch(nEvtCode)
	{
		case IR_EVT_AppStatus_CardOk:
		case IR_EVT_AppStatus_CardOut:
		case IR_EVT_AppStatus_CardInvalid:
		case IR_EVT_AppStatus_CardFault:
		case IR_EVT_AppStatus_CardIn:
		case IR_EVT_AppStatus_EEPROMDataInvalid:
			return 1;

		case IR_EVT_SvcStatus_FTAService:
			return 2;

		case IR_EVT_SvcStatus_ServiceFull:
		case IR_EVT_SvcStatus_PMTFormatError:
		case IR_EVT_SvcStatus_UpdateEEPROM:
		case IR_EVT_SvcStatus_NoProductMatch:
		case IR_EVT_SvcStatus_SectorError:
		case IR_EVT_SvcStatus_RSAFails:
		case IR_EVT_SvcStatus_ProductExpired:
		case IR_EVT_SvcStatus_RegionBlackout:
		case IR_EVT_SvcStatus_UserBitmapFilter:
		case IR_EVT_SvcStatus_PreviewTime:
		case IR_EVT_SvcStatus_NoView:
		case IR_EVT_SvcStatus_Purchased:
		case IR_EVT_SvcStatus_AlreadyPurchase:
		case IR_EVT_SvcStatus_EventRecorded:
		case IR_EVT_SvcStatus_IPPVFull:
		case IR_EVT_SvcStatus_CreditLimit:
		case IR_EVT_SvcStatus_Descrambed:
		case IR_EVT_SvcStatus_KeyMismatch:
		case IR_EVT_SvcStatus_PKeyError:
		case IR_EVT_SvcStatus_IncorrectSlot:
		case IR_EVT_SvcStatus_KeyMismatch33:
		case IR_EVT_SvcStatus_OutOfKeys:
		case IR_EVT_SvcStatus_OutOfStreams:
		case IR_EVT_SvcStatus_GeneralErrors:
		case IR_EVT_SvcStatus_ProductExpiryNoted:
		case IR_EVT_SvcStatus_NotActivated:
		case IR_EVT_SvcStatus_LppvStoreFull:
		case IR_EVT_SvcStatus_LppvNoOrder:
		case IR_EVT_SvcStatus_LppvNoBuyWindow:
		case IR_EVt_SvcStatus_LppvNoCredit:
		case IR_EVt_SvcStatus_LppvTooExpensive:
		case IR_EVt_SvcStatus_PpvParameterError:
			return 3;

		case IR_EVT_SvcStatus_NoStreamMatch:
		case IR_EVT_SvcStatus_UnknownStatus:
			return 4;

		default:
			return 0;
	}
}

void xsvc_cas_IrSendJumpEvent(void)
{

	HVD_DMX_SendJumpEvent();
}

HERROR	xsvc_cas_IrGetUiData(HUINT8 *pucUiData, DxCAS_UiDataType_e eType)
{
	HERROR hErr = ERR_FAIL;

	HxLOG_Debug("eType: %d \n", eType);
	switch(eType)
	{
		case eDxCAS_UIDATATYPE_IRDETO_CLIENTVERSION:
			hErr = xsvc_cas_IrGetClientVersionString(pucUiData);
			break;

		case eDxCAS_UIDATATYPE_IRDETO_MANUFACTURERCODE:
			hErr = xsvc_cas_IrLoaderGetManuCodeString(pucUiData);
			break;

		case eDxCAS_UIDATATYPE_IRDETO_HWCODE:
			hErr = xsvc_cas_IrLoaderGetHWCodeString(pucUiData);
			break;

		case eDxCAS_UIDATATYPE_IRDETO_VARIANT:
			hErr = xsvc_cas_IrLoaderGetVariantString(pucUiData);
			break;

		case eDxCAS_UIDATATYPE_IRDETO_SYSTEMID:
			hErr = xsvc_cas_IrLoaderGetSysIdString(pucUiData);
			break;

		case eDxCAS_UIDATATYPE_IRDETO_KEYVERSION:
			hErr = xsvc_cas_IrLoaderGetKeyVerString(pucUiData);
			break;

		case eDxCAS_UIDATATYPE_IRDETO_SIGNATUREVERSION:
			hErr = xsvc_cas_IrLoaderGetSigVerString(pucUiData);
			break;

		case eDxCAS_UIDATATYPE_IRDETO_PRIVATEDATA:
			hErr = xsvc_cas_IrLoaderGetPrivateData(pucUiData);
			break;

		default:
			HxLOG_Debug("not defined eType: %d \n", eType);
			hErr = ERR_FAIL;
			break;

	}

	HxLOG_Debug("hErr: %d \n", hErr);
	return hErr;
}

