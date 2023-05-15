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
//#include <autoconf.h>
#include <htype.h>
#include "di_err.h"

#include <ldvt.h>
//#include "irdeto_setting.h"
#if 1 // jichoi. if defined(BUILD_TEMP1)
//#include "ir_loader_v5.h"
#endif

#include "tir_nvr.h"
#include "tir_sup.h"
#include "tir_api.h"
#include <pal_sys.h>
#include <pal_fuse.h>

#include "util.h"

//#include <irldvt_ir_sup.h>

#include "LoaderCoreAPI_ApplicationHelper.h"

#if defined(CONFIG_DEBUG)
#define PrintFunc HLIB_CMD_Printf
#define PrintDebug HLIB_CMD_Printf
#define PrintErr HLIB_CMD_Printf
#else
#define PrintFunc(...)
#define PrintDebug(...)
#define PrintErr(...)
#endif

extern Boot_Block_Control_Block g_tIr_bbcb;
extern HUINT8 g_tIr_cpcb[IR_CPCB_SIZE];


HUINT32	TIRSTATUS_GetLoaderMajor(HUINT8 *pucData)
{
	*pucData = g_tIr_bbcb.loader_major_version;
	PrintDebug("%s, data = %d\n", __FUNCTION__, *pucData);
	return 0;
}

HUINT32	TIRSTATUS_GetLoaderMinor(HUINT8 *pucData)
{
	*pucData = g_tIr_bbcb.loader_minor_version;
	PrintDebug("%s, data = %d\n", __FUNCTION__, *pucData);
	return 0;
}

HUINT32	TIRSTATUS_GetLoaderBuildVer(HUINT16 *pucData)
{
	*pucData = g_tIr_bbcb.loader_version_buildnumber;

	return 0;
}

HUINT32	TIRSTATUS_GetManuf_Id(HUINT16 *pucData)
{
	*pucData = g_tIr_bbcb.manufacturer_id;
	PrintDebug("%s, data = %d\n", __FUNCTION__, *pucData);
	return 0;
}

HUINT32	TIRSTATUS_GetHW_Ver(HUINT16 *pucData)
{
	*pucData = g_tIr_bbcb.hardware_version;
	PrintDebug("%s, data = %d\n", __FUNCTION__, *pucData);
	return 0;
}

HUINT32	TIRSTATUS_GetOUI(HUINT32 *pucData)
{
	*pucData = (g_tIr_bbcb.OUI[0]<<16) | (g_tIr_bbcb.OUI[1]<<8) | (g_tIr_bbcb.OUI[2]) ;
	PrintDebug("%s, data = %d\n", __FUNCTION__, *pucData);
	return 0;
}

HUINT32	TIRSTATUS_GetBootCheckAlg(HUINT8 *pucData)
{
	*pucData = g_tIr_bbcb.boot_check_algorithm;
	PrintDebug("%s, data = %d\n", __FUNCTION__, *pucData);
	return 0;
}


HUINT32	TIRSTATUS_GetRandomNum(HUINT32 *pulData)
{
	*pulData = get32bit(&g_tIr_bbcb.random_number[0]);

	PrintDebug("%s, data = %d\n", __FUNCTION__, *pulData);
	return 0;
}

HUINT32	TIRSTATUS_GetSerialNum(HUINT32 *pulData)
{
	*pulData = get32bit(&g_tIr_bbcb.serial_number[0]);
	PrintDebug("%s, data = %d\n", __FUNCTION__, *pulData);
	return 0;
}

HUINT32	TIRSTATUS_GetDownloadSeqNum(HUINT16 *pucData)
{
	lc_uint16 dl_seq_num;

	if(LC_SUCCESS != LoaderCoreAPI_GetDownloadSequenceNumber(&dl_seq_num) )
	{
		return HIR_NOT_OK;
	}
	return HIR_OK;
}




HUINT32	TIRSTATUS_GetISIGNVerString(HUINT8 *pszStr)
{

	lc_last_download_info_st last_dl_info;
	if(LC_SUCCESS != LC_GetDownloadInformation(&last_dl_info) )
	{
		sprintf(pszStr, "-.-.-" );
		return HIR_NOT_OK;
	}

	sprintf(pszStr, "%c.%c.%c%c",
		last_dl_info.isign_version[0],
		last_dl_info.isign_version[1],
		last_dl_info.isign_version[2],
		last_dl_info.isign_version[3] );

	return HIR_OK;
}

HUINT32	TIRSTATUS_GetISIGNSerialNum(HUINT8 *pszStr)
{

	lc_last_download_info_st last_dl_info;
	if(LC_SUCCESS != LC_GetDownloadInformation(&last_dl_info) )
	{
		sprintf(pszStr, "----" );
		return HIR_NOT_OK;
	}

	memcpy(pszStr, last_dl_info.isign_serial_number, sizeof(last_dl_info.isign_serial_number) );

	return HIR_OK;
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





HUINT32	TIROTA_SetOTAFlag(void)
{
#if 0
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
#else
	HUINT32 lc_ret;
	lc_bool dn_flag = 1;
	lc_download_type dn_type = LCH_OTA_DOWNLOAD;

	lc_ret = LoaderCoreAPI_SetDownloadFlag( dn_flag, dn_type);

	if(lc_ret != LC_SUCCESS)
	{
		dbgerr1("LoaderCoreAPI_SetDownloadFlag failed. ret = %d\n", lc_ret);
		return 1;
	}
	dbgprt0("LoaderCoreAPI_SetDownloadFlag OK\n");
{// dbg.
	unsigned char ld_pt[308];
	char fileName[128] = {0,};
	int ret = ERR_OK;

	sprintf(fileName, NVRAM_CAS_IRV5_MPL_FILENAME_PFX"%d", MODULE_DRV_ID_LOADER_PT);
	ret = DI_NVRAM_Read(1 /*PAL_NVRAM_PARTITION_RW*/, fileName, 0, ld_pt,  sizeof(ld_pt) );
	dbgdump("read Loader PT", ld_pt, sizeof(ld_pt) );
	printf("Wow~~\n");
	sleep(2);
}

	return 0;

#endif

}

HUINT32 TIRSTATUS_GetUKInfo(IrV5UKInfo *pUkInfo)
{
	lc_uint16	sysid, key_ver, sig_ver;
	HUINT32 lc_ret;
	lc_ret = LoaderCoreAPI_GetUKInfo(&sysid, &key_ver, &sig_ver);

	// if got error occured we must fill following fields. jichoi.
	pUkInfo->sysid = sysid;
	pUkInfo->key_ver = key_ver;
	pUkInfo->sig_ver = sig_ver;

	return lc_ret;
}
HUINT32 TIRSTATUS_GetVariantInfo(IrV5VariantInfo *pVariantInfo)
{

	lc_uint16 variant, subvariant;

	if(LC_SUCCESS != LoaderCoreAPI_GetVariant(&variant, &subvariant) )
	{
		return HIR_NOT_OK;
	}

	pVariantInfo->variant = variant;
	pVariantInfo->subvariant = subvariant;

	return HIR_OK;
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

	HIR_ERR 			hir;
	CH_SAT_TuneParam_t	stTuneParam;


	stTuneParam.etAntennaType 	= etAntennaType;
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

{
	lc_dvb_ssu_parameters ssu_param;
	lc_buffer_st PrivateData={NULL, 0};

	ssu_param.pid = usPid;

	PrivateData.bytes = malloc( LC_MANUFACTURER_PRIVATE_INFO_LENGTH );
	PrivateData.length = LC_MANUFACTURER_PRIVATE_INFO_LENGTH;

	H2N32(&ulTunerId);
	H2N32(&ulLNBFreq);
	memcpy(PrivateData.bytes + IR_PARTITION_LOADER_OFFSET_ulTunerID, &ulTunerId , sizeof(HUINT32) );
	memcpy(PrivateData.bytes + IR_PARTITION_LOADER_OFFSET_ulLNBFre, &ulLNBFreq , sizeof(HUINT32) );

	memcpy(PrivateData.bytes + IR_PARTITION_LOADER_OFFSET_DI_CHANNEL_PARAM, &stTuneParam, sizeof(CH_SAT_TuneParam_t) );

	hir = LoaderCoreAPI_WriteDVBSSUParameters(&ssu_param, &PrivateData);
	if(hir != LC_SUCCESS)
	{
#ifdef V5_DEV_CREATE_LOADERPT
		dbgerr1("LoaderCoreAPI_WriteDVBSSUParameters failed. ret = %d, try Create LD Partition.\n", hir);
		hir = LC_CreateLoaderPartition();
		if(hir == LC_SUCCESS)
		{
			hir = LoaderCoreAPI_WriteDVBSSUParameters(&ssu_param, &PrivateData);
			if(hir != LC_SUCCESS)
			{
				dbgerr1("Retry-LoaderCoreAPI_WriteDVBSSUParameters failed. ret = %d\n", hir);
				hir =  HIR_NOT_OK;
			}
			else
			{
				dbgprt0("Retry-LoaderCoreAPI_WriteDVBSSUParameters OK\n");
			}
		}
		else
		{
			dbgerr1("LC_CreateLoaderPartition failed. ret = %d\n", hir);
			hir =  HIR_NOT_OK;
		}
#else
		dbgerr1("LoaderCoreAPI_WriteDVBSSUParameters failed. ret = %d\n", hir);
		hir =  HIR_NOT_OK;
#endif
	}

	dbgdump("PrivateData Write", PrivateData.bytes + IR_PARTITION_LOADER_OFFSET_DI_CHANNEL_PARAM, LC_MANUFACTURER_PRIVATE_INFO_LENGTH );
	if(PrivateData.bytes)
		free(PrivateData.bytes);

	dbgprt1("LoaderCoreAPI_WriteDVBSSUParameters ret = %d\n", hir);
}

	return hir;
}


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

	HUINT32 			ulLNBFreq;
	CH_SAT_TuneParam_t	stTuneParam;
	HUINT32 			ulTuningFreq;

#if 0
	hir = TIR_E2P_Partition_Read(TIRNVR_LOADER, IR_PARTITION_LOADER_OFFSET_DI_CHANNEL_PARAM, &stTuneParam, sizeof(CH_SAT_TuneParam_t) );
	hir |= TIR_E2P_Partition_Read(TIRNVR_LOADER, IR_PARTITION_LOADER_OFFSET_wServicePID, &usPidTemp, sizeof(HUINT16) );
	hir |= TIR_E2P_Partition_Read(TIRNVR_LOADER, IR_PARTITION_LOADER_OFFSET_ucDnldTableID, &ucTableIdTemp, sizeof(HUINT8) );
	hir |= TIR_E2P_Partition_Read(TIRNVR_LOADER, IR_PARTITION_LOADER_OFFSET_ulTunerID, &ulTunerId, sizeof(HUINT32) );
	hir |= TIR_E2P_Partition_Read(TIRNVR_LOADER, IR_PARTITION_LOADER_OFFSET_ulLNBFre, &ulLNBFreq, sizeof(HUINT32) );

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

	*ulLnbFreq			= ulLNBFreq;

	*usPid				= usPidTemp;
	*ucTableId			= ucTableIdTemp;
	*ulTunerGroupId		= ulTunerId;

	hir |= TIR_E2P_Partition_Read(TIRNVR_LOADER, IR_PARTITION_LOADER_OFFSET_ulDisplayTuningFreq, &ulTuningFreq, sizeof(HUINT32) );
	*ulFrequency = ulTuningFreq;

#else
{
	lc_dvb_ssu_parameters ssu_param;
	lc_buffer_st PrivateData={NULL, 0};

	/* Read Private Data & dvb ssu param */
	hir = LoaderCoreAPI_ReadDVBSSUParameters(&ssu_param, &PrivateData);
	if(hir != LC_SUCCESS)
	{
		dbgerr1("LoaderCoreAPI_ReadDVBSSUParameters failed. ret = %d\n", hir);
		if(PrivateData.bytes)
			free(PrivateData.bytes);

		return HIR_NOT_OK;
	}

	ulTunerId = ir_getbits32(PrivateData.bytes + IR_PARTITION_LOADER_OFFSET_ulTunerID);
	ulLNBFreq = ir_getbits32(PrivateData.bytes + IR_PARTITION_LOADER_OFFSET_ulLNBFre);

	dbgdump("PrivateData Read", PrivateData.bytes + IR_PARTITION_LOADER_OFFSET_DI_CHANNEL_PARAM, LC_MANUFACTURER_PRIVATE_INFO_LENGTH );
	memcpy(&stTuneParam, PrivateData.bytes + IR_PARTITION_LOADER_OFFSET_DI_CHANNEL_PARAM, sizeof(CH_SAT_TuneParam_t) );

	if(PrivateData.bytes)
		free(PrivateData.bytes);

	/* Convert param */
	*usPid				= ssu_param.pid;
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

	*ulLnbFreq			= ulLNBFreq;
	*ulTunerGroupId		= ulTunerId;

	dbgprt1("stTuneParam.ulSymbolRate = %d\n", stTuneParam.ulSymbolRate);
	dbgprt2("ulLNBFreq = %d, stTuneParam.ulFrequency = %d\n", ulLNBFreq, stTuneParam.ulFrequency );

	*ulFrequency = ulLNBFreq + stTuneParam.ulFrequency;

}
#endif


	PrintDebug("TP Freq[%d]\n", *ulFrequency);
	return 0;
}

#endif


HERROR TIRSTATUE_GetFusingInfo(HUINT8 *pucSCS, HUINT8 *pucJTAG, HUINT8 *pucOTP, HUINT8 *pucCWE)
{
	Sys_FusingInfo_t	stFusingInfo;

	HERROR		err;
	dbgtrc;
	if(pucSCS == NULL || pucJTAG == NULL || pucOTP == NULL || pucCWE == NULL)
	{
		return ERR_FAIL;
	}
	dbgtrc;
#if 0
	err = PAL_FUSE_GetInfo(&stFusingInfo);
#else
	dbgwarn0("\"PAL_FUSE_GetInfo\" is abnormal state!!! the forced return ERR_FAIL\n");
	err = ERR_FAIL;
#endif
	dbgtrc;
	if(err != ERR_OK)
	{
	dbgtrc;
		return ERR_FAIL;
	}
	dbgtrc;
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


