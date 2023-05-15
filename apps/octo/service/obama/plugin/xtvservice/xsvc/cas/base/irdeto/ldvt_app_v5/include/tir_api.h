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

#ifndef  ___TIR_API_H___
#define ___TIR_API_H___

//	include channel functions
#if 1
#include "di_channel.h"
#if defined(CONFIG_MW_CH_SATELLITE)
#include "di_channel_s.h"
#elif defined(CONFIG_MW_CH_CABLE)
#include "di_channel_c.h"
#else
#error "not defined"
#endif
#endif
#include <irldvt_stb_params.h>


typedef struct _IrV5UKInfo
{
	lc_uint16	sysid;
	lc_uint16	key_ver;
	lc_uint16	sig_ver;

}IrV5UKInfo;

typedef struct _IrV5VariantInfo
{
	lc_uint16	variant;
	lc_uint16	subvariant;
}IrV5VariantInfo;

/*
typedef struct
{
	HUINT8		ucSCS;
	HUINT8		ucJTAG;
	HUINT8		ucOTP;
	HUINT8		ucCWE;
	HUINT32		ulParams[4];
	HUINT16		usSTBOwnerID;
	HUINT8		ucRemuxFusing;
	HUINT8		ucMarketID;
} Sys_FusingInfo_t;
*/

HUINT32	TIRSTATUS_GetSerialNum(HUINT32 *pulData);
HUINT32	TIRSTATUS_GetLoaderMajor(HUINT8 *pucData);
HUINT32	TIRSTATUS_GetLoaderMinor(HUINT8 *pucData);
HUINT32	TIRSTATUS_GetLoaderBuildVer(HUINT16 *pucData);
HUINT32	TIRSTATUS_GetManuf_Id(HUINT16 *pucData);
HUINT32	TIRSTATUS_GetHW_Ver(HUINT16 *pucData);
HUINT32	TIRSTATUS_GetDownloadSeqNum(HUINT16 *pucData);
HUINT32	TIRSTATUS_GetRandomNum(HUINT32 *pulData);
HUINT32	TIRSTATUS_GetCRCBlockCountInFlash(HUINT8 *pCRCPartCnt);
HUINT32	TIRSTATUS_GetISIGNVerString(HUINT8 *pszStr);
HUINT32	TIRSTATUS_GetBin2irdSerNumString(HUINT8 *pszStr);
HUINT32	tIr_bcd2dec(HUINT32 dwVal);
HUINT32	TIRSTATUS_GetSignDate(HUINT16 *pusMJD);
HUINT32	TIRSTATUS_GetSignTime(TIME_T *pstTime);
HUINT32	TIRSTATUS_GetDnldDate(HUINT16 *pusMJD);
HUINT32	TIRSTATUS_GetDnldTime(TIME_T *pstTime);
HUINT32	TIRSTATUS_GetDnldDuration(HUINT16 *pusData);
HUINT32	TIRSTATUS_GetImageSize(HUINT32 *pul_ImgSize);
HUINT32	TIROTA_SetOTAFlag(void);

#ifdef	CONFIG_MW_CH_CABLE
HUINT32	IROTA_WriteOTAInfo(HUINT32 ulFreq, HUINT32 ulSR, HUINT32 ulCons, HUINT32 ulSpec, HUINT16 usPid, HUINT8 ucTableId);
HUINT32	IROTA_ReadOTAInfo(HUINT32 *pulFreq, HUINT32 *pulSR, HUINT32 *pulCons, HUINT32 *pulSpec, HUINT16 *pusPid, HUINT8 *pucTableId);
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
									HUINT8 					ucTableId);

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
									HUINT8 					*ucTableId);

#endif


HERROR TIRSTATUE_GetFusingInfo(HUINT8 *pucSCS, HUINT8 *pucJTAG, HUINT8 *pucOTP, HUINT8 *pucCWE);
HERROR TIRSTATUE_SECURECHIP_GetCSSN(HUINT32 *pwCSSN);

extern HIR_ERR TIR_E2P_Partition_Write( TIRNVR_Type ePartType, HUINT32 ulPartOffset, void *pucData, HUINT32 ulSize);
#endif

