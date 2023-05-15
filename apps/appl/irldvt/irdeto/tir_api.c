/*
 * (c) 2011-2012 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/
#include <hlib.h>
#include <autoconf.h>
#include <htype.h>
#include "di_err.h"

#include <ldvt.h>
#if defined(CONFIG_MW_SYS_NAND_FLASH_ONLY)
#include "ir_loader.h"
#else
#include "tir_loader.h"
#endif
#include "tir_nvr.h"
#include "tir_sup.h"
#include "tir_api.h"
#if defined(CONFIG_OP_DIGITURK)
#include "pal_ch_sat.h"
#endif
#include <pal_sys.h>
#include <pal_fuse.h>

//#include <irldvt_ir_sup.h>

#if defined(CONFIG_DEBUG)
#define PrintFunc printf
#define PrintDebug printf
#define PrintErr printf
#else
#define PrintFunc(...)
#define PrintDebug(...)
#define PrintErr(...)
#endif

extern Boot_Block_Control_Block g_tIr_bbcb;
extern HUINT8 g_tIr_cpcb[IR_CPCB_SIZE];
#if defined(CONFIG_OP_DIGITURK)
extern HUINT32	_ulUserDefLnbFreq;
#endif


HUINT32	TIRSTATUS_GetSerialNum(HUINT32 *pulData)
{
	*pulData = g_tIr_bbcb.ulSerialNumber;
	PrintDebug("%s, data = %d\n", __FUNCTION__, *pulData);
	return 0;
}

HUINT32	TIRSTATUS_GetLoaderMajor(HUINT8 *pucData)
{
	*pucData = g_tIr_bbcb.ucLoaderMajor;
	PrintDebug("%s, data = %d\n", __FUNCTION__, *pucData);
	return 0;
}

HUINT32	TIRSTATUS_GetLoaderMinor(HUINT8 *pucData)
{
	*pucData = g_tIr_bbcb.ucLoaderMinor;
	PrintDebug("%s, data = %d\n", __FUNCTION__, *pucData);
	return 0;
}

HUINT32	TIRSTATUS_GetLoaderType(HUINT8 *pucData)
{
	*pucData = g_tIr_bbcb.ucLoaderType;
	PrintDebug("%s, data = %d\n", __FUNCTION__, *pucData);
	return 0;
}

HUINT32	TIRSTATUS_GetManuCode(HUINT8 *pucData)
{
	*pucData = g_tIr_bbcb.ucManufacturerCode;
	PrintDebug("%s, data = %d\n", __FUNCTION__, *pucData);
	return 0;
}

HUINT32	TIRSTATUS_GetHWCode(HUINT8 *pucData)
{
	*pucData = g_tIr_bbcb.ucHardwareCode;
	PrintDebug("%s, data = %d\n", __FUNCTION__, *pucData);
	return 0;
}

HUINT32	TIRSTATUS_GetLoadSeqNum(HUINT8 *pucData)
{
	*pucData = g_tIr_cpcb[CCB_Load_Sequence_Num];
	PrintDebug("%s, data = %d\n", __FUNCTION__, *pucData);
	return 0;
}

HUINT32	TIRSTATUS_GetRandomNum(HUINT32 *pulData)
{
	*pulData = g_tIr_bbcb.ulRandomNumber;
	PrintDebug("%s, data = %d\n", __FUNCTION__, *pulData);

	return 0;
}

HUINT32	TIRSTATUS_GetCRCBlockCountInFlash(HUINT8 *pCRCPartCnt)
{
	int					i;
	unsigned long		offcrc, p_crc_32;
	unsigned char		tmp_p_cnt, tmp_nc_cnt, non_crc_mask;

	tmp_p_cnt = g_tIr_cpcb[CCB_Partition_Count];

	tmp_nc_cnt = 0;
	for( i = 0; i < tmp_p_cnt; i++)
	{
		offcrc = (unsigned long)i * CCB_Part_One_Crc_Info_Length;

		non_crc_mask = g_tIr_cpcb[CCB_Part_Crc_Info_Start + offcrc];
#if defined(CONFIG_PROD_IR2000SD)
		p_crc_32 = *((unsigned short *)(&g_tIr_cpcb[CCB_Part_Crc_Info_Start + offcrc + CCB_Part_Crc_Info_P_CRC_32]));
		p_crc_32 = (p_crc_32 <<16) | (*((unsigned short *)(&g_tIr_cpcb[CCB_Part_Crc_Info_Start + offcrc + CCB_Part_Crc_Info_P_CRC_32 + 2])));
#else
		p_crc_32 = *((unsigned long *)(&g_tIr_cpcb[CCB_Part_Crc_Info_Start + offcrc + CCB_Part_Crc_Info_P_CRC_32]));
#endif
		if(non_crc_mask == 0xA5 && p_crc_32 == 0xA5A5A5A5)
				tmp_nc_cnt++;
	}
	*pCRCPartCnt = tmp_p_cnt - tmp_nc_cnt;

	return 0;
}

HUINT32	TIRSTATUS_GetBin2irdVerString(HUINT8 *pszStr)
{
	unsigned char	strVersion[16];

	memcpy (strVersion, &g_tIr_cpcb[CCB_Bin2Ird_Version], sizeof(unsigned char)*4);
	sprintf(pszStr, "%c.%c%c%c", strVersion[0], strVersion[1], strVersion[2], strVersion[3]);

	return 0;
}

// TODO: Followings should be fixed.
// TODO: It's little endian system.
HUINT32	TIRSTATUS_GetBin2irdSerNumString(HUINT8 *pszStr)
{
	strncpy(pszStr, &g_tIr_cpcb[CCB_Bin2Ird_Serial_Num], 4);
//	*pszStr = *((unsigned long *)(&g_tIr_cpcb[CCB_Bin2Ird_Serial_Num]));
	PrintDebug ("%s, Str = %s\n", __FUNCTION__, pszStr);

	return 0;
}



HUINT32	tIr_bcd2dec(HUINT32 dwVal)
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



HUINT32	TIRSTATUS_GetSignDate(HUINT16 *pusMJD)
{
	HUINT8	*pucMJD = (HUINT8*)pusMJD;

	pucMJD[1] = g_tIr_cpcb[CCB_Sign_Time_Date];
	pucMJD[0] = g_tIr_cpcb[CCB_Sign_Time_Date + 1];


//	memcpy(pusMJD, &g_tIr_cpcb[CCB_Sign_Time_Date], 2);
//	*pusMJD = *((unsigned short *)(&g_tIr_cpcb[CCB_Sign_Time_Date]));
	PrintDebug ("%s, Mjd = %04X\n", __FUNCTION__, *pusMJD);

	return 0;
}
HUINT32	TIRSTATUS_GetSignTime(TIME_T *pstTime)
{

	pstTime->ucHour  = tIr_bcd2dec(g_tIr_cpcb[CCB_Sign_Time_Date + 2]);
	pstTime->ucMinute=  tIr_bcd2dec(g_tIr_cpcb[CCB_Sign_Time_Date + 3]);
	pstTime->ucSecond = tIr_bcd2dec(g_tIr_cpcb[CCB_Sign_Time_Date + 4]);

	return 0;
}

HUINT32	TIRSTATUS_GetDnldDate(HUINT16 *pusMJD)
{
	HUINT8	*pucMJD = (HUINT8*)pusMJD;

	PrintDebug("(g_tIr_cpcb[CCB_Download_Time_Date]=0x%X\n", g_tIr_cpcb[CCB_Download_Time_Date]  );
	PrintDebug("(g_tIr_cpcb[CCB_Download_Time_Date+1]=0x%X\n", g_tIr_cpcb[CCB_Download_Time_Date+1]  );

	pucMJD[1] = g_tIr_cpcb[CCB_Download_Time_Date];
	pucMJD[0] = g_tIr_cpcb[CCB_Download_Time_Date + 1];
	PrintDebug ("%s, Mjd = %04X\n", __FUNCTION__, *pusMJD);

	return 0;
}
HUINT32	TIRSTATUS_GetDnldTime(TIME_T *pstTime)
{
	PrintDebug("(g_tIr_cpcb[CCB_Download_Time_Date+2]=0x%X\n", g_tIr_cpcb[CCB_Download_Time_Date+2]  );
	PrintDebug("(g_tIr_cpcb[CCB_Download_Time_Date+3]=0x%X\n", g_tIr_cpcb[CCB_Download_Time_Date+3]  );
	PrintDebug("(g_tIr_cpcb[CCB_Download_Time_Date+4]=0x%X\n", g_tIr_cpcb[CCB_Download_Time_Date+4]  );

	pstTime->ucHour  = tIr_bcd2dec(g_tIr_cpcb[CCB_Download_Time_Date + 2]);
	pstTime->ucMinute=  tIr_bcd2dec(g_tIr_cpcb[CCB_Download_Time_Date + 3]);
	pstTime->ucSecond = tIr_bcd2dec(g_tIr_cpcb[CCB_Download_Time_Date + 4]);

	return 0;
}

HUINT32	TIRSTATUS_GetDnldDuration(HUINT16 *pusData)
{
#if 1 // by jcihoi
	PrintDebug("(g_tIr_cpcb[CCB_Download_Duration]=0x%X\n",	g_tIr_cpcb[CCB_Download_Duration]);
	PrintDebug("(g_tIr_cpcb[CCB_Download_Duration+1]=0x%X\n",	g_tIr_cpcb[CCB_Download_Duration+1]);

	*pusData = (g_tIr_cpcb[CCB_Download_Duration] <<8 ) | (g_tIr_cpcb[CCB_Download_Duration + 1]);


#else
	HUINT8	*pucData = (HUINT8*)pusData;
	pucData[1] = g_tIr_cpcb[CCB_Download_Duration];
	pucData[0] = g_tIr_cpcb[CCB_Download_Duration + 1];
#endif
	return 0;
}

HUINT32	TIRSTATUS_GetImageSize(HUINT32 *pul_ImgSize)
{
	int				i;
	unsigned char	tmp_p_cnt, tmp_nc_cnt, non_crc_mask;
	unsigned long	tmp_p_length;
	unsigned long	tmp_img_size;
	unsigned long	offcrc, p_crc_32;
	HUINT8	*pucData;

	tmp_p_cnt = g_tIr_cpcb[CCB_Partition_Count];

	tmp_nc_cnt = 0;
	tmp_img_size = 0;

	PrintDebug("%s, tmp_p_cnt = %d\n", __FUNCTION__,  tmp_p_cnt );

	for( i = 0; i < tmp_p_cnt; i++ )
	{
		offcrc = CCB_Part_Crc_Info_Start + ((unsigned long)i * CCB_Part_One_Crc_Info_Length);

		non_crc_mask = g_tIr_cpcb[offcrc];
		p_crc_32 = *((unsigned long *)(&g_tIr_cpcb[offcrc + 4]));

		pucData = &g_tIr_cpcb[offcrc];
		tmp_p_length = (*(pucData) << 24) | (*(pucData + 1) << 16) | (*(pucData + 2) <<8) | (*(pucData + 3));
//		tmp_p_length = *((unsigned long *)(&g_tIr_cpcb[offcrc]));
		tmp_p_length &= 0x00FFFFFF;

		tmp_img_size += tmp_p_length;
	}

	*pul_ImgSize = tmp_img_size;

	return 0;
}


HUINT32	TIROTA_SetOTAFlag(void)
{
	unsigned char		ucSWDnFlag;

	ucSWDnFlag = 1;

	if(0 != TIR_E2P_Partition_Write( TIRNVR_LOADER, IR_PARTITION_LOADER_OFFSET_ucSWDLFlag, &ucSWDnFlag, sizeof(HUINT8)) )
	{
		PrintErr("%s, %d, TIR_E2P_Partition_Write error\n", __FUNCTION__, __LINE__);
	}
	else
	{
		PrintDebug("OK\n");
	}
	VK_TASK_Sleep(1000);
	ucSWDnFlag = 0;

	return 0;
}

IHL_S16 TIRSTATUS_GetStatusInfo(IHL_STATUS *pstStatus)
{

	printf("\n[%s],[%d]++++++++++++++++++++++++++\n",__FUNCTION__,__LINE__);
	IHL_S16  s16RetVal, s16KeyStatus;

	s16RetVal = IRLDVT_I_HLSUP_GetStatusInfo( pstStatus );
	if( s16RetVal == I_HLSUP_OK )
	{
		switch( pstStatus->u8BlockFailMask )
		{
			case  0:
			case  2:
			case  4:
			case  6:
			case  8:
			case 10:
				s16KeyStatus = I_HLSUP_KS_OK;
				break;
			case  3:
			case  7:
			case 11:
			case 15:
				s16KeyStatus = I_HLSUP_KS_NEW;
				break;
			case  1:
			case  5:
			case  9:
			case 12:
			case 13:
			case 14:
				s16KeyStatus = I_HLSUP_KS_KDO; /* Key Download Only */
				break;
		}
	}
	else
	{
		printf("\n[%s],[%d] return ERR ++++++++++++++++++++++++++\n",__FUNCTION__,__LINE__);
		s16KeyStatus = I_HLSUP_KS_ERR;
	}
	printf("\n[%s],[%d] return status++++++++++++++++++++++++++\n",__FUNCTION__,__LINE__);
	return s16KeyStatus;
}

#ifdef	CONFIG_MW_CH_CABLE
HUINT32	IROTA_WriteOTAInfo(HUINT32 ulFreq, HUINT32 ulSR, HUINT32 ulCons, HUINT32 ulSpec, HUINT16 usPid, HUINT8 ucTableId)
{
	HIR_ERR hir;
	CH_CAB_TuneParam_t	stTuneParam;

	HUINT16 usPidTemp = usPid;
	HUINT8 ucTableIdTemp= ucTableId;

	stTuneParam.ulFrequency = ulFreq;
	stTuneParam.ulSymbolRate = ulSR;
	stTuneParam.etSpectrum = ulSpec;
	stTuneParam.etModulation = ulCons;

	hir = TIR_E2P_Partition_Write(TIRNVR_LOADER, IR_PARTITION_LOADER_OFFSET_DI_CHANNEL_PARAM, &stTuneParam, sizeof(CH_CAB_TuneParam_t) );
	hir |= TIR_E2P_Partition_Write(TIRNVR_LOADER, IR_PARTITION_LOADER_OFFSET_wServicePID, &usPidTemp, sizeof(HUINT16) );
	hir |= TIR_E2P_Partition_Write(TIRNVR_LOADER, IR_PARTITION_LOADER_OFFSET_ucDnldTableID, &ucTableIdTemp, sizeof(HUINT8) );
	if(hir != HIR_OK)
	{
		PrintErr("CAS_IR_NVR_Partition_Write:error\n") ;
		return 1;
	}

	PrintDebug("%s, Freq = %d\n",__FUNCTION__,  stTuneParam.ulFrequency);
	PrintDebug("%s, SR = %d\n", __FUNCTION__,   stTuneParam.ulSymbolRate);
	PrintDebug("%s, Constel =%d\n", __FUNCTION__ ,stTuneParam.etSpectrum);
	PrintDebug("%s, Modulation =%d\n",__FUNCTION__ , stTuneParam.etModulation);

	PrintDebug("%s, Pid = 0x%X\n", __FUNCTION__ ,usPidTemp);
	PrintDebug("%s, Tb Id =0x%X\n",__FUNCTION__ , ucTableIdTemp);

	return 0;
}

HUINT32	IROTA_ReadOTAInfo(HUINT32 *pulFreq, HUINT32 *pulSR, HUINT32 *pulCons, HUINT32 *pulSpec, HUINT16 *pusPid, HUINT8 *pucTableId)
{
	HIR_ERR hir;
	CH_CAB_TuneParam_t	stTuneParam;


	hir = TIR_E2P_Partition_Read(TIRNVR_LOADER, IR_PARTITION_LOADER_OFFSET_DI_CHANNEL_PARAM, &stTuneParam, sizeof(CH_CAB_TuneParam_t) );
	if(hir != HIR_OK)
	{
		PrintErr("CAS_IR_NVR_Partition_Write:error\n") ;
		return 1;
	}

	*pulFreq = stTuneParam.ulFrequency;
	*pulSR = stTuneParam.ulSymbolRate;
	*pulCons = stTuneParam.etModulation;
	*pulSpec = stTuneParam.etSpectrum;


	hir = TIR_E2P_Partition_Read(TIRNVR_LOADER, IR_PARTITION_LOADER_OFFSET_wServicePID, pusPid, sizeof(HUINT16) );
	hir |= TIR_E2P_Partition_Read(TIRNVR_LOADER, IR_PARTITION_LOADER_OFFSET_ucDnldTableID, pucTableId, sizeof(HUINT8) );
	if(hir != HIR_OK)
	{
		PrintErr("CAS_IR_NVR_Partition_Read:error\n") ;
		return 1;
	}

	PrintDebug("%s ,*pulFreq = %d\n", __FUNCTION__, *pulFreq);
	PrintDebug("%s ,*pulSR = %d\n",  __FUNCTION__,*pulSR);
	PrintDebug("%s ,*pulCons =%d\n", __FUNCTION__, *pulCons);
	PrintDebug("%s ,*pulSpec =%d\n", __FUNCTION__, *pulSpec);
	PrintDebug("%s ,*pusPid =0x%X\n", __FUNCTION__, *pusPid);
	PrintDebug("%s ,*pucTableId =0x%X\n", __FUNCTION__, *pucTableId);

	return 0;
}
#elif	CONFIG_MW_CH_SATELLITE
HUINT32	IROTA_WriteOTAInfoSat( 	CH_SAT_AntennaType_e	etAntennaType,
								HUINT32		 			ulTunerGroupId,
								HUINT32 				ulFrequency,
								HUINT32 				ulSymbolRate,
								DxSat_CodeRate_e			fecCodeRate,
								DxSat_Polarization_e		polarization,
								DxSat_TransportSpec_e		transSpec,
								DxSat_PskModulation_e		pskMod,
								DxSat_Pilot_e 			pilot,
								HUINT32 				ulLnbFreq,
								DxSAT_LnbVoltage_e		lnbVoltage,
								HBOOL					b22kTone,
								DxSAT_DiseqcVer_e 		diseqcVersion,
								DxSAT_DiseqcInput_e		diseqcInput,
								DxSAT_ToneBurst_e 		toneBurst,
								HUINT16 				usPid,
								HUINT8 					ucTableId)
{


	HUINT8 				ucTableIdTemp = ucTableId;
	HUINT16 			usPidTemp = usPid;
	HUINT32 			ulTunerId = ulTunerGroupId;
	HUINT32 			ulLNBFreq = ulLnbFreq;
#if defined(CONFIG_OP_DIGITURK)
	HUINT8				ucLnbType;
#endif
	HIR_ERR 			hir;
	CH_SAT_TuneParam_t	stTuneParam;


	stTuneParam.etAntennaType 	= etAntennaType;

#if defined(CONFIG_OP_DIGITURK)
	/* 주의 : DB, LNB info, MW 의 주파수는 MHz 단위임. DI는 kHz 단위임 */
	if (ulLnbFreq == CH_UNIVERSAL_LNB_FREQ)
	{
		ulLNBFreq = PAL_LNB_GetActualLnbFreq(ulFrequency);
		//stTuneParam.b22kTone = PAL_LNB_GetActual22KTone(ulFrequency);
	}
	else if(ulLnbFreq >= (HUINT32)CH_MDU5_LNB_FREQ && ulLnbFreq <= (HUINT32)CH_MDU1_LNB_FREQ)
	{
		ulLNBFreq = PAL_LNB_GetActualMduLnbFreq(ulLnbFreq, polarization, ulFrequency);
		//stTuneParam.b22kTone = FALSE;
	}
	else
	{
		stTuneParam.b22kTone = b22kTone;
	}
#endif

	stTuneParam.ulFrequency 	= (HUINT32)abs((HINT32)ulFrequency - (HINT32)ulLNBFreq);
	stTuneParam.ulSymbolRate	= ulSymbolRate;
	stTuneParam.etFecCodeRate	= _dvtApp_sat_ConvPAL2DI_CodeRate(fecCodeRate);
	stTuneParam.etPolarization	= _dvtApp_sat_ConvPAL2DI_Polarization(polarization);
	stTuneParam.etTransSpec		= _dvtApp_sat_ConvPAL2DI_TransSpec(transSpec);
	stTuneParam.etModulation	= _dvtApp_sat_ConvPAL2DI_Modulation(pskMod);
	stTuneParam.etPilot			= _dvtApp_sat_ConvPAL2DI_Pilot(pilot);
	stTuneParam.etRollOff		= DI_CH_ROLL_035;
	stTuneParam.etLnbVoltage	= _dvtApp_sat_ConvPAL2DI_LnbVoltage(lnbVoltage);
	stTuneParam.etDiseqcVersion	= _dvtApp_sat_ConvPAL2DI_DiseqcVersion(diseqcVersion);
	stTuneParam.etDiseqcInput	= _dvtApp_sat_ConvPAL2DI_DiseqcInput(diseqcInput);
	stTuneParam.etToneBurst		= _dvtApp_sat_ConvPAL2DI_ToneBurst(toneBurst);
	stTuneParam.b22kTone		= b22kTone;

	hir = TIR_E2P_Partition_Write(TIRNVR_LOADER, IR_PARTITION_LOADER_OFFSET_DI_CHANNEL_PARAM, (void *)&stTuneParam, sizeof(CH_SAT_TuneParam_t) );
	hir |= TIR_E2P_Partition_Write(TIRNVR_LOADER, IR_PARTITION_LOADER_OFFSET_wServicePID, &usPidTemp, sizeof(HUINT16) );
	hir |= TIR_E2P_Partition_Write(TIRNVR_LOADER, IR_PARTITION_LOADER_OFFSET_ucDnldTableID, &ucTableIdTemp, sizeof(HUINT8) );
	hir |= TIR_E2P_Partition_Write(TIRNVR_LOADER, IR_PARTITION_LOADER_OFFSET_ulTunerID, &ulTunerId, sizeof(HUINT32) );
#if defined(CONFIG_OP_DIGITURK)
	ucLnbType = _dvtApp_sat_Conv_LnbFreq2LnbType(ulLnbFreq);
	PrintDebug("LNBType [%d], ulLnbFreq =%d\n", ucLnbType, ulLnbFreq);
	hir |= TIR_E2P_Partition_Write(TIRNVR_LOADER, IR_PARTITION_LOADER_OFFSET_ucLNBTYPE, &ucLnbType, sizeof(HUINT8) );
	if((ulFrequency > _ulUserDefLnbFreq) && (ucLnbType == 0/*eLNB_Type_UserDefined*/))
	{
		ulLNBFreq = (_ulUserDefLnbFreq | 0x80000000);
	}
	else
	{
		ulLnbFreq = _ulUserDefLnbFreq;

	}
	hir |= TIR_E2P_Partition_Write(TIRNVR_LOADER, IR_PARTITION_LOADER_OFFSET_ulLNBFre, &ulLNBFreq, sizeof(HUINT32) );
#else
	hir |= TIR_E2P_Partition_Write(TIRNVR_LOADER, IR_PARTITION_LOADER_OFFSET_ulLNBFre, &ulLNBFreq, sizeof(HUINT32) );
	hir |= TIR_E2P_Partition_Write(TIRNVR_LOADER, IR_PARTITION_LOADER_OFFSET_ulDisplayTuningFreq, &ulFrequency, sizeof(HUINT32) );
#endif

	if(hir != HIR_OK)
	{
		PrintErr("CAS_IR_NVR_Partition_Write:error\n") ;
		return 1;
	}

	PrintDebug("%s, Freq = %d\n",__FUNCTION__,  stTuneParam.ulFrequency);
	PrintDebug("%s, SR = %d\n", __FUNCTION__,   stTuneParam.ulSymbolRate);
	PrintDebug("%s, etFecCodeRate =%d\n", __FUNCTION__ ,stTuneParam.etFecCodeRate);
	PrintDebug("%s, etPolarization =%d\n", __FUNCTION__ , stTuneParam.etPolarization);
	PrintDebug("%s, etTransSpec =%d\n", __FUNCTION__ , stTuneParam.etTransSpec);
	PrintDebug("%s, etModulation =%d\n", __FUNCTION__ , stTuneParam.etModulation);
	PrintDebug("%s, etPilot =%d\n", __FUNCTION__ , stTuneParam.etPilot);
	PrintDebug("%s, etRollOff =%d\n", __FUNCTION__ , stTuneParam.etRollOff);
	PrintDebug("%s, etLnbVoltage =%d\n", __FUNCTION__ , stTuneParam.etLnbVoltage);
	PrintDebug("%s, etDiseqcVersion =%d\n", __FUNCTION__ , stTuneParam.etDiseqcVersion);
	PrintDebug("%s, etDiseqcInput =%d\n", __FUNCTION__ , stTuneParam.etDiseqcInput);
	PrintDebug("%s, etToneBurst =%d\n", __FUNCTION__ , stTuneParam.etToneBurst);
	PrintDebug("%s, b22kTone =%d\n", __FUNCTION__ , stTuneParam.b22kTone);

	PrintDebug("%s, ulLNBFreq =%d\n", __FUNCTION__ , ulLNBFreq);

	PrintDebug("%s, Pid = 0x%X\n", __FUNCTION__ ,usPidTemp);
	PrintDebug("%s, Tb Id =0x%X\n",__FUNCTION__ , ucTableIdTemp);


	return 0;
}

#if defined(CONFIG_OP_DIGITURK)
HUINT32 TIR_OTA_GetActualMduLnbFreq(HUINT32 ulLnbFreq, DxSat_Polarization_e ePol, HUINT32 ulFreq, HUINT8 *pucAdd)
{
// MDU1 :  11.450~11.700 GHz, H -> LO 10.45 GHz
//         11.450~11.700 GHz, V -> LO 10.00 GHz
// MDU2 : 10.780~10.950 GHz, H -> LO 9.830 GHz , 11.450~11.700 GHz, H -> LO 9.925 GHz
//         10.780~10.950 GHz or 11.450~11.700 GHz,
// MDU3  :10.780~10.950 GHz, H -> LO 12.930 GHz, 11.450~11.700 GHz, H -> LO  9.830 GHz
//         10.780~10.950 GHz, V -> LO  9.830 GHz, 11.450~11.700 GHz, V -> LO 12.930 GHz
// MDU4 :  10.780~10.950 GHz, H -> LO 12.930 GHz, 11.450~11.700 GHz, H -> LO 12.650 GHz
//         10.780~10.950 GHz, V -> LO 12.650 GHz, 11.428~11.700 GHz, V -> LO 12,930 GHz
// MDU5 :  10.740~10.950 GHz, H -> LO 12.584 GHz, 11.450~11.700 GHz, H -> LO 12.646 GHz
//         10.740~10.950 GHz, V -> LO 12.894 GHz, 11.440~11.680 GHz, V -> LO 12,976 GHz

	HUINT32		ulActLnbFreq = 0;

	switch(ulLnbFreq)
	{
		case CH_MDU1_LNB_FREQ:
			if(ePol == eDxSAT_POLAR_HOR)
				ulActLnbFreq = 10450;
			else
				ulActLnbFreq = 10000;

			*pucAdd = TRUE;
			break;

		case CH_MDU2_LNB_FREQ:
			if(ePol == eDxSAT_POLAR_HOR)
			{
				if(ulFreq >= 1525 && ulFreq <= 1775)
				{
					ulActLnbFreq = 9925;
				}
				else if(ulFreq >= 950 && ulFreq <= 1120)
				{
					ulActLnbFreq = 9830;
				}
			}
			else
			{
				if(ulFreq >= 1230 && ulFreq <= 1400)
				{
					ulActLnbFreq = 9550;
				}
				else if(ulFreq >= 1900 && ulFreq <= 2150)
				{
					ulActLnbFreq = 9550;
				}
			}
			*pucAdd = TRUE;
			break;

		case CH_MDU3_LNB_FREQ:
			if(ePol == eDxSAT_POLAR_HOR)
			{
				if(ulFreq >= 1620 && ulFreq <= 1870)
				{
					ulActLnbFreq = 9830;
					*pucAdd = TRUE;
				}
				else if(ulFreq >= 1980 && ulFreq <= 2150)
				{
					ulActLnbFreq = 12930;
					*pucAdd = FALSE;
				}
			}
			else
			{
				if(ulFreq >= 950 && ulFreq <= 1120)
				{
					ulActLnbFreq = 9830;
					*pucAdd = TRUE;
				}
				else if(ulFreq >= 1230 && ulFreq <= 1480)
				{
					ulActLnbFreq = 12930;
					*pucAdd = FALSE;
				}
			}
			break;

		case CH_MDU4_LNB_FREQ:
			if(ePol == eDxSAT_POLAR_HOR)
			{
				if(ulFreq >= 1335 && ulFreq <= 1585)
				{
					ulActLnbFreq = 13035;
				}
				else if(ulFreq >= 1980 && ulFreq <= 2150)
				{
					ulActLnbFreq = 12930;
				}
				*pucAdd = FALSE;
			}
			else
			{
				if(ulFreq >= 950 && ulFreq <= 1221)
				{
					ulActLnbFreq = 12650;
				}
				else if(ulFreq >= 1700 && ulFreq <= 1870)
				{
					ulActLnbFreq = 12650;
				}
				*pucAdd = FALSE;
			}
			break;

		case CH_MDU5_LNB_FREQ:
			if(ePol == eDxSAT_POLAR_HOR)
			{
				if(ulFreq >= 1637 && ulFreq <= 1840)
				{
					ulActLnbFreq = 12584;
				}
				else if(ulFreq >= 950 && ulFreq <= 1194)
				{
					ulActLnbFreq = 12646;
				}
				*pucAdd = FALSE;
			}
			else
			{
				if(ulFreq >= 1948 && ulFreq <= 2150)
				{
					ulActLnbFreq = 12894;
				}
				else if(ulFreq >= 1299 && ulFreq <= 1533)
				{
					ulActLnbFreq = 12976;
				}
				*pucAdd = FALSE;
			}
			break;

		default:
			ulActLnbFreq = 0;
			break;

	}

#if defined(CONFIG_DEBUG)
	/* for debugging */
	if(ulActLnbFreq == 0)
		PrintDebug("[TIR_OTA_GetActualMduLnbFreq] ERROR : out of range freq : %d \n", ulFreq);
	else
		PrintDebug("[TIR_OTA_GetActualMduLnbFreq] LnbFreq for cur MDU -> %d , bAdd[%d]\n", ulActLnbFreq, *pucAdd);
#endif

	return ulActLnbFreq;

}
#endif

HUINT32	IROTA_ReadOTAInfoSat( 		CH_SAT_AntennaType_e	*etAntennaType,
									HUINT32		 			*ulTunerGroupId,
									HUINT32 				*ulFrequency,
									HUINT32 				*ulSymbolRate,
									DxSat_CodeRate_e			*fecCodeRate,
									DxSat_Polarization_e		*polarization,
									DxSat_TransportSpec_e		*transSpec,
									DxSat_PskModulation_e		*pskMod,
									DxSat_Pilot_e 			*pilot,
									HUINT32 				*ulLnbFreq,
									DxSAT_LnbVoltage_e		*lnbVoltage,
									HBOOL					*b22kTone,
									DxSAT_DiseqcVer_e 		*diseqcVersion,
									DxSAT_DiseqcInput_e		*diseqcInput,
									DxSAT_ToneBurst_e 		*toneBurst,
									HUINT16 				*usPid,
									HUINT8 					*ucTableId)
{
	HIR_ERR 			hir;
	HUINT8 				ucTableIdTemp;
	HUINT16 			usPidTemp;
	HUINT32 			ulTunerId;
#if defined(CONFIG_OP_DIGITURK)
	HUINT8				ucLnbType;
	HUINT8				ucAdd = TRUE;
#endif
	HUINT32 			ulLNBFreq;
	CH_SAT_TuneParam_t	stTuneParam;
	HUINT32 			ulTuningFreq;

	hir = TIR_E2P_Partition_Read(TIRNVR_LOADER, IR_PARTITION_LOADER_OFFSET_DI_CHANNEL_PARAM, &stTuneParam, sizeof(CH_SAT_TuneParam_t) );
	hir |= TIR_E2P_Partition_Read(TIRNVR_LOADER, IR_PARTITION_LOADER_OFFSET_wServicePID, &usPidTemp, sizeof(HUINT16) );
	hir |= TIR_E2P_Partition_Read(TIRNVR_LOADER, IR_PARTITION_LOADER_OFFSET_ucDnldTableID, &ucTableIdTemp, sizeof(HUINT8) );
	hir |= TIR_E2P_Partition_Read(TIRNVR_LOADER, IR_PARTITION_LOADER_OFFSET_ulTunerID, &ulTunerId, sizeof(HUINT32) );
#if defined(CONFIG_OP_DIGITURK)
	hir |= TIR_E2P_Partition_Read(TIRNVR_LOADER, IR_PARTITION_LOADER_OFFSET_ucLNBTYPE, &ucLnbType, sizeof(HUINT8));
	hir |= TIR_E2P_Partition_Read(TIRNVR_LOADER, IR_PARTITION_LOADER_OFFSET_ulLNBFre, &_ulUserDefLnbFreq, sizeof(HUINT32) );
#else
	hir |= TIR_E2P_Partition_Read(TIRNVR_LOADER, IR_PARTITION_LOADER_OFFSET_ulLNBFre, &ulLNBFreq, sizeof(HUINT32) );
#endif

#if defined(CONFIG_OP_DIGITURK)
	if(stTuneParam.etPolarization == DI_CH_POLAR_AUTO)	//Auto는 없도록 하자! MDU LNB에서 문제가 됨.
	{
		stTuneParam.etPolarization = DI_CH_POLAR_HOR;
	}

	ulLNBFreq = _dvtApp_sat_Conv_LnbType2LnbFreq(ucLnbType);
	PrintDebug("LNB type[%d], LNB Freq[0x%08x]\n", ucLnbType, ulLNBFreq);
	if (ulLNBFreq == CH_UNIVERSAL_LNB_FREQ)
	{
		stTuneParam.b22kTone;
		if(stTuneParam.b22kTone)
		{
			ulLNBFreq = 10600;
		}
		else
		{
			ulLNBFreq = 9750;
		}
		ucAdd = TRUE;
	}
	else if(ulLNBFreq >= (HUINT32)CH_MDU5_LNB_FREQ && ulLNBFreq <= (HUINT32)CH_MDU1_LNB_FREQ)
	{
		ulLNBFreq = TIR_OTA_GetActualMduLnbFreq(ulLNBFreq, _dvtApp_sat_ConvDI2PAL_Polarization(stTuneParam.etPolarization), stTuneParam.ulFrequency, &ucAdd);
	}
	else
	{
		if((ulLNBFreq & 0x80000000) == 0x80000000)
		{
			ucAdd = TRUE;
		}
		else
		{
			ucAdd = FALSE;
		}
		ulLNBFreq = (ulLNBFreq & 0x7FFFFFFF);
		_ulUserDefLnbFreq = ulLNBFreq;
	}

	PrintDebug("Real LNB Freq[%d]\n", ulLNBFreq);
#endif

	*etAntennaType		= stTuneParam.etAntennaType;
	*ulSymbolRate		= stTuneParam.ulSymbolRate;
	*fecCodeRate		= _dvtApp_sat_ConvDI2PAL_CodeRate(stTuneParam.etFecCodeRate);
	*polarization		= _dvtApp_sat_ConvDI2PAL_Polarization(stTuneParam.etPolarization);
	*transSpec			= _dvtApp_sat_ConvDI2PAL_TransSpec(stTuneParam.etTransSpec);
	*pskMod				= _dvtApp_sat_ConvDI2PAL_Modulation(stTuneParam.etModulation);
	*pilot				= _dvtApp_sat_ConvDI2PAL_Pilot(stTuneParam.etPilot);
	*lnbVoltage			= _dvtApp_sat_ConvDI2PAL_LnbVoltage(stTuneParam.etLnbVoltage);
	*diseqcVersion		= _dvtApp_sat_ConvDI2PAL_DiseqcVersion(stTuneParam.etDiseqcVersion);
	*diseqcInput		= _dvtApp_sat_ConvDI2PAL_DiseqcInput(stTuneParam.etDiseqcInput);
	*toneBurst			= _dvtApp_sat_ConvDI2PAL_ToneBurst(stTuneParam.etToneBurst);
	*b22kTone			= stTuneParam.b22kTone;
#if defined(CONFIG_OP_DIGITURK)
	*ulLnbFreq			= _dvtApp_sat_Conv_LnbType2LnbFreq(ucLnbType);
#else
	*ulLnbFreq			= ulLNBFreq;
#endif
	*usPid				= usPidTemp;
	*ucTableId			= ucTableIdTemp;
	*ulTunerGroupId		= ulTunerId;

#if defined(CONFIG_OP_DIGITURK)
	if(ucAdd)
	{
		*ulFrequency	= stTuneParam.ulFrequency + ulLNBFreq;
	}
	else
	{
		*ulFrequency	= ulLNBFreq - stTuneParam.ulFrequency;
	}
#else
	hir |= TIR_E2P_Partition_Read(TIRNVR_LOADER, IR_PARTITION_LOADER_OFFSET_ulDisplayTuningFreq, &ulTuningFreq, sizeof(HUINT32) );
	*ulFrequency = ulTuningFreq;
#endif

	PrintDebug("TP Freq[%d]\n", *ulFrequency);
	return 0;
}

#endif


#if defined(CONFIG_OP_DIGITURK)
HIR_ERR TIR_OTA_SetStandbyFlag(HUINT8 ucFlag)
{
	HIR_ERR hir;

	hir = CAS_IR_NVR_Partition_Write(TIRNVR_LOADER, IR_PARTITION_LOADER_OFFSET_ucStandbyFlag, &ucFlag, sizeof(HUINT8) );
	if(hir != HIR_OK)
	{
			return 1;
	}
	return 0;
}

HIR_ERR TIR_OTA_GetStandbyFlag(HUINT8 *pucFlag)
{
	HIR_ERR hir;

	hir = CAS_IR_NVR_Partition_Read(TIRNVR_LOADER, IR_PARTITION_LOADER_OFFSET_ucStandbyFlag, pucFlag, sizeof(HUINT8) );
	if(hir != HIR_OK)
	{
			return 1;
	}
	return 0;
}

HIR_ERR TIR_OTA_SetLanguageCode(HUINT8 *szLangCode)
{
	HIR_ERR hir;

	hir = CAS_IR_NVR_Partition_Write(TIRNVR_LOADER, IR_PARTITION_LOADER_OFFSET_szLanguageCode, szLangCode, sizeof(HUINT8)*3 );
	if(hir != HIR_OK)
	{
			return 1;
	}
	return 0;
}

HIR_ERR TIR_OTA_GetLanguageCode(HUINT8 *szLangCode)
{
	HIR_ERR hir;

	hir = CAS_IR_NVR_Partition_Read(TIRNVR_LOADER, IR_PARTITION_LOADER_OFFSET_szLanguageCode, szLangCode, sizeof(HUINT8)*3 );
	if(hir != HIR_OK)
	{
			return 1;
	}
	return 0;
}

HIR_ERR TIR_OTA_SetErrorCode(HUINT32 ulErrorCode)
{
	HIR_ERR hir;

	hir = CAS_IR_NVR_Partition_Write(TIRNVR_LOADER, IR_PARTITION_LOADER_OFFSET_ulLastErrorCode, &ulErrorCode, sizeof(HUINT32) );
	if(hir != HIR_OK)
	{
			return 1;
	}
	return 0;
}

HIR_ERR TIR_OTA_GetErrorCode(HUINT32 *pulErrorCode)
{
	HIR_ERR hir;

	if(pulErrorCode == NULL)
	{
		return 1;
	}

	hir = CAS_IR_NVR_Partition_Read(TIRNVR_LOADER, IR_PARTITION_LOADER_OFFSET_ulLastErrorCode, pulErrorCode, sizeof(HUINT32) );
	if(hir != HIR_OK)
	{
			return 1;
	}
	return 0;
}
#endif

#if defined(CONFIG_MW_CAS_IRDETO_SECURE)
HERROR TIRSTATUE_GetFusingInfo(HUINT8 *pucSCS, HUINT8 *pucJTAG, HUINT8 *pucOTP, HUINT8 *pucCWE)
{
	Sys_FusingInfo_t	stFusingInfo;

	HERROR		err;

	if(pucSCS == NULL || pucJTAG == NULL || pucOTP == NULL || pucCWE == NULL)
	{
		return ERR_FAIL;
	}

	err = PAL_FUSE_GetInfo(&stFusingInfo);

	if(err != ERR_OK)
	{
		return ERR_FAIL;
	}

	*pucSCS		= stFusingInfo.ucSCS;
	*pucJTAG	= stFusingInfo.ucJTAG;
	*pucOTP		= stFusingInfo.ucOTP;
	*pucCWE		= stFusingInfo.ucCWE;

	return ERR_OK;
}


HERROR TIRSTATUE_SECURECHIP_GetCSSN(HUINT32 *pwCSSN)
{
	if(pwCSSN == NULL)
	{
		return ERR_FAIL;
	}

	PAL_SYS_GetChipId(pwCSSN);

	return ERR_OK;
}
#endif


