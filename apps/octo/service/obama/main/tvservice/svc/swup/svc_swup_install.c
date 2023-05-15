/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  mw_sw_install.c
	@brief

	Description:
	Module: 			 	\n
	Remarks : 										\n

	Copyright (c) 2012 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <octo_common.h>

#include <pal_sys.h>
#include <pal_param.h>

#include <svc_si.h>
#include <svc_swup.h>

#include "_svc_swup.h"


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	eSWUP_STATE_Start,
	eSWUP_STATE_Stop,
	eSWUP_STATE_Writing,
	eSWUP_STATE_Cancle,
	eSWUP_STATE_Successed,
	eSWUP_STATE_Failed,
} SvcSwUpdate_Status_e;

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HUINT32					s_ulSwupInstallTaskId = 0;
STATIC SvcSwUpdate_Status_e		s_eInstallStatus = eSWUP_STATE_Stop;


/*******************************************************************/
/*****************     Private Functions       *********************/
/*******************************************************************/
STATIC HERROR		svc_swup_ClearEmergencySettingFlag(PAL_EMERGENCY_OTA_CHANNEL_t eEmergeFlag);

STATIC void		svc_swup_InstallSetStatus(SvcSwUpdate_Status_e eStatus)
{
	s_eInstallStatus = eStatus;
}

STATIC SvcSwUpdate_Status_e	svc_swup_InstallGetStatus(void)
{
	return s_eInstallStatus;
}

STATIC void		svc_swup_Task(void *pvParam)
{
	HERROR		hErr	=	ERR_FAIL;
	HINT32		nRet 	= 0;
	HINT32 		nPieceNum 	= 0, nPieceIdx = 0;
	HUINT32		ulFileSize 	= 0, ulWriteSize = 0, ulCalcProgress = 0, ulBackupProgress = 0, ulReadSize = 0;
	HUINT8		*pucSrcFile 	= NULL;
	FILE 		*fp			= NULL;
	HUINT32		ulPieceSize = 0;

	if(pvParam == NULL)
		return ;

	svc_swup_InstallSetStatus(eSWUP_STATE_Start);

#if 0
// Get File Size
{
	HxFILE_t	hFile;
	HOffset_t	ullOffset ;

	hErr = HLIB_FILE_Size(HxFILE_t hSelf,HUINT64 * ret_size)

	ulFileSize = ullOffset;
}
#endif


	fp 	= fopen((char *)pvParam, "rb");
	if (fp == NULL){
		HxLOG_Error("Failed File Open!!\n");
		svc_swup_UtilPostMsgToMgr(eSEVT_SWUP_INSTALL_WRITE_IMAGE_FAIL, (Handle_t)NULL, 0, 0, 0);
		return ;
	}

	// get file size
	nRet	= fseek(fp, 0, SEEK_END);
	ulFileSize = ftell(fp);
	if (ulFileSize < 0)
	{
		HxLOG_Error("Failed FileSize Unknown!!\n");
		goto err_exit;
#if 0
		fclose(fp);
		svc_swup_UtilPostMsgToMgr(eSEVT_SWUP_INSTALL_WRITE_IMAGE_FAIL, (Handle_t)NULL, 0, 0, 0);
		return ;
#endif
	}

	hErr = PAL_SYS_Upgrade_Setup(ulFileSize, PalSys_UPGRADE_PIECE_SIZE_FULL, &nPieceNum, &ulPieceSize);
	if ( hErr != ERR_OK || 0 == ulPieceSize)
	{
		//svc_swup_UtilPostMsgToMgr(eSEVT_SWUP_INSTALL_WRITE_IMAGE_FAIL, (Handle_t)NULL, 0, 0, 0);
		HxLOG_Error("Failed FileSize Err!!\n");
		goto err_exit;
	}

 	pucSrcFile = (HUINT8*)HLIB_STD_MemAlloc( sizeof(HUINT8) * ulPieceSize );
	if (pucSrcFile == NULL)
	{
		HxLOG_Error("Failed Mem Alloc !!\n");
		goto err_exit;
 	}

	nRet = fseek(fp, 0, SEEK_SET);

	ulWriteSize = 0;

	/* Write 도중 Power Off가 된다면, ubifs의 data는 valid하지 않음. - Write 완료 전까지는 BG Flag를 내림 */
	svc_swup_ClearEmergencySettingFlag(PAL_EMERGENCY_OTA_CHANNEL_BG);

	svc_swup_InstallSetStatus(eSWUP_STATE_Writing);

	// start image writing to RAWFS
	for ( nPieceIdx = 0; nPieceIdx < nPieceNum; nPieceIdx++)
	{
 		HxSTD_MemSet(pucSrcFile, 0, sizeof(HUINT8) * ulPieceSize);

		// writing per ulPieceSize
		if ((ulFileSize - ulWriteSize) > ulPieceSize)
		{
			ulReadSize = fread(pucSrcFile, 1, (sizeof(HUINT8) *  ulPieceSize), fp);
			if ( ulReadSize != ulPieceSize)
			{
				HxLOG_Critical("Failed OTA File Read!!. (ulReadSize : 0x%X)\n", ulReadSize);
				svc_swup_InstallSetStatus(eSWUP_STATE_Cancle);
				goto err_exit;
 			}

			nRet	= PAL_SYS_Upgrade_WriteData(nPieceIdx, pucSrcFile,  ( sizeof(HUINT8) * ulPieceSize));
			ulWriteSize 	+= ulPieceSize;
 		}
		else
		{
			//writing rest data
			ulReadSize = fread(pucSrcFile, 1,  ulFileSize - ulWriteSize, fp);
			if ( ulReadSize != (ulFileSize - ulWriteSize))
			{
				HxLOG_Critical("Failed OTA File Read!!. (ulReadSize : 0x%X)\n", ulReadSize);
				svc_swup_InstallSetStatus(eSWUP_STATE_Cancle);
				goto err_exit;
 			}
			nRet	= PAL_SYS_Upgrade_WriteData(nPieceIdx, pucSrcFile, ulFileSize - ulWriteSize);
			ulWriteSize 	+= ulFileSize - ulWriteSize;
 		}

		if ( nRet < 0 )
		{
			HxLOG_Error("Failed PAL_SYS_Upgrade_WriteData .. \n");
			goto err_exit;
 		}

#if 1
		// 2012.06.22 작업. ( 일단 install progress 의 % 게산을 여기서 한다. X/100)
		ulCalcProgress = (HFLOAT64) ( (HFLOAT64)ulWriteSize/(HFLOAT64)ulFileSize) * (HFLOAT64)100 ;
		if ( ulBackupProgress != ulCalcProgress)
		{
			svc_swup_UtilPostMsgToMgr(eSEVT_SWUP_INSTALL_PROGRESS, (Handle_t)NULL, 0, ulCalcProgress,0);
			ulBackupProgress = ulCalcProgress;
		}

#endif
		// if received cancle command
		if (svc_swup_InstallGetStatus() == eSWUP_STATE_Cancle)
		{
			svc_swup_UtilPostMsgToMgr(eSEVT_SWUP_INSTALL_WRITE_IMAGE_FAIL, (Handle_t)NULL, 0, 0, 0);
			goto err_exit;
 		}

		HLIB_STD_TaskSleep(20);		// test
	}

	fclose(fp);
 	HLIB_STD_MemFree(pucSrcFile);

	svc_swup_UtilPostMsgToMgr(eSEVT_SWUP_INSTALL_WRITE_IMAGE_SUCCESS, (Handle_t)NULL, 0, 0, 0);

	HLIB_STD_TaskSleep(500);			// test

	return ;

err_exit:
 	svc_swup_UtilPostMsgToMgr(eSEVT_SWUP_INSTALL_WRITE_IMAGE_FAIL, (Handle_t)NULL, 0, 0, 0);
	if (fp) 		fclose(fp);
	if (pucSrcFile)	HLIB_STD_MemFree(pucSrcFile);
 	return;
}

STATIC HERROR	svc_swup_WriteEmergencyUpdateInfoByRF(SvcSwUpdate_EmergencyUpdateInfo_t *pstInfo)
{
	HERROR			nRet;
	SWUP_InfoSlot_e	eSwupSlot = eSWUP_INFO_SLOT_0;

	if (pstInfo->eSource != eDxSWUPDATE_SOURCE_Rf)
	{
		HxLOG_Error("Not Match EmergentyUpdate Type\n");
		return ERR_FAIL;
	}

	if (PalParam_Activated())
	{
		nRet = PAL_PARAM_SetField(ePAL_PARAM_SWUP_TYPE1, (void *)&pstInfo->unInfo.stRfInfo.eDownloadType, sizeof(SWUP_DownloadType_e));
		if (nRet != ERR_OK)
		{
			HxLOG_Error("Failed PAL_PARAM_SetField(ePAL_PARAM_SWUP_TYPE1) !\n");
			return nRet;
		}

		nRet = PAL_PARAM_SetField(ePAL_PARAM_ES_PID1, (void *)&pstInfo->unInfo.stRfInfo.usPid, sizeof(HUINT16));
		if (nRet != ERR_OK)
		{
			HxLOG_Error("Failed PAL_PARAM_SetField(ePAL_PARAM_ES_PID1) !\n");
			return nRet;
		}

		nRet = PAL_PARAM_SetField(ePAL_PARAM_APP_VERSION_NEXT, (void *)&pstInfo->ulDataVersion, sizeof(HUINT32));
		if (nRet != ERR_OK)
		{
			HxLOG_Error("Failed PAL_PARAM_SetField(ePAL_PARAM_APP_VERSION_NEXT) !\n");
			return nRet;
		}
	}
	else
	{
		nRet	= PAL_SYS_SetOtaType(eSwupSlot, pstInfo->unInfo.stRfInfo.eDownloadType);
		if (nRet != ERR_OK)
		{
			HxLOG_Error("Failed PAL_SYS_SetOtaType() !\n");
			return nRet;
		}

		nRet	= PAL_SYS_SetOtaPid(eSwupSlot, pstInfo->unInfo.stRfInfo.usPid);
		if (nRet != ERR_OK)
		{
			HxLOG_Error("Failed PAL_SYS_SetOtaPid() !\n");
			return nRet;
		}

		nRet	= PAL_SYS_SetSystemVersion(eVERSION_TYPE_NEXTOTA, pstInfo->ulDataVersion);
		if (nRet != ERR_OK)
		{
			HxLOG_Error("Failed PAL_SYS_SetSystemVersion() !\n");
			return nRet;
		}
	}

	switch(pstInfo->unInfo.stRfInfo.eDeliType)
	{
		case eDxDELIVERY_TYPE_SAT:
			{
				DxSatTuningInfo_t	*pSatTuningInfo	= NULL;
				HBOOL				b22kTone		= FALSE;

				pSatTuningInfo = &pstInfo->unInfo.stRfInfo.stChTuningInfo.sat;

				if (pSatTuningInfo == NULL)
				{
					HxLOG_Error("Failed TuningInfo is NULL !\n");
					return ERR_FAIL;
				}

				switch(pSatTuningInfo->antennaType)
				{
					case eDxANT_TYPE_LNB_ONLY:
						break;
					case eDxANT_TYPE_DISEQC:
						b22kTone	= pSatTuningInfo->antInfo.info.diseqc.b22kTone;
						break;
					case eDxANT_TYPE_SCD:
						b22kTone	= pSatTuningInfo->antInfo.info.scd.b22kTone;
						break;
					case eDxANT_TYPE_SMATV:
						break;
					case eDxANT_TYPE_MONOBLOCK_LNB:
						break;
				}

				nRet	= PAL_SYS_SetSwUpdateTuningInfoSat(eSwupSlot,
												pSatTuningInfo->tuningInfo.ulFrequency,
												pSatTuningInfo->tuningInfo.ulSymbolRate,
												pSatTuningInfo->tuningInfo.eFecCodeRate,
												pSatTuningInfo->tuningInfo.ePolarization,
												pSatTuningInfo->tuningInfo.eTransSpec,
												pSatTuningInfo->tuningInfo.ePskMod,
												pSatTuningInfo->tuningInfo.ePilot,
												pSatTuningInfo->antennaType,
												/* LNB && Common */
												pSatTuningInfo->antInfo.info.lnb.ulLnbFreq,
												pSatTuningInfo->antInfo.info.lnb.lnbVoltage,
												b22kTone,
												/* DISQEC */
												pSatTuningInfo->antInfo.info.diseqc.diseqcVersion,
												pSatTuningInfo->antInfo.info.diseqc.diseqcInput,
												pSatTuningInfo->antInfo.info.diseqc.toneBurst,
												pSatTuningInfo->antInfo.info.diseqc.ucMotorPosition,
												pSatTuningInfo->antInfo.info.diseqc.sSatLonggitude,
												/* SCD */
												pSatTuningInfo->antInfo.info.scd.scdInput,
												pSatTuningInfo->antInfo.info.scd.ulScdUserBandNum,
												pSatTuningInfo->antInfo.info.scd.ulScdUserBandFreq,
												pSatTuningInfo->antInfo.info.scd.ulScdUserBandNum1,
												pSatTuningInfo->antInfo.info.scd.ulScdUserBandFreq1);
				if (nRet != ERR_OK)
				{
					HxLOG_Error("Failed PAL_SYS_SetSwUpdateTuningInfoSat() !\n");
					return nRet;
				}
			}
			break;

		case eDxDELIVERY_TYPE_TER:
			{
				DxTerTuningInfo_t	*pTerTuningInfo	= NULL;

				//pTerTuningInfo	= &pstInfo->unInfo.stRfInfo.stTuningInfo.ter;
				pTerTuningInfo 	= &pstInfo->unInfo.stRfInfo.stChTuningInfo.ter;

				if (pTerTuningInfo == NULL)
				{
					HxLOG_Error("Failed TuningInfo is NULL !\n");
					return ERR_FAIL;
				}

				nRet	= PAL_SYS_SetSwUpdateTuningInfoTer(eSwupSlot,
												pTerTuningInfo->ulFrequency,
												pTerTuningInfo->eBandWidth,
												pTerTuningInfo->eConstellation,
												pTerTuningInfo->eHierachy,
												pTerTuningInfo->eCodeRate,
												pTerTuningInfo->eTransMode,
												pTerTuningInfo->eGuardInterval,
												pTerTuningInfo->eStream,
												pTerTuningInfo->eOffset,
												/* DVB-T2 */
												pTerTuningInfo->eSystem,
												pTerTuningInfo->stT2Param.ulPlpId,
												pTerTuningInfo->stT2Param.preambleFormat,
												pTerTuningInfo->stT2Param.mixed,
												pTerTuningInfo->stT2Param.pilotPattern,
												pTerTuningInfo->stT2Param.extenedCarrier,
												pTerTuningInfo->stT2Param.PAPRreduction,
												pTerTuningInfo->stT2Param.numPlpBlocks);
				if (nRet != ERR_OK)
				{
					HxLOG_Error("Failed PAL_SYS_SetSwUpdateTuningInfoTer() !\n");
					return nRet;
				}
			}
			break;
		case eDxDELIVERY_TYPE_CAB:
			{
				DxCabTuningInfo_t	*pCabTuningInfo	= NULL;

				//pCabTuningInfo	= &pstInfo->unInfo.stRfInfo.stTuningInfo.cab;
				pCabTuningInfo	= &pstInfo->unInfo.stRfInfo.stChTuningInfo.cab;

				if (pCabTuningInfo == NULL)
				{
					HxLOG_Error("Failed TuningInfo is NULL !\n");
					return ERR_FAIL;
				}

				nRet	= PAL_SYS_SetSwUpdateTuningInfoCab (eSwupSlot,
												pCabTuningInfo->ulFrequency,
												pCabTuningInfo->ulSymbolRate,
												pCabTuningInfo->constellation,
												pCabTuningInfo->spectrum);
				if (nRet != ERR_OK)
				{
					HxLOG_Error("Failed PAL_SYS_SetSwUpdateTuningInfoTer() !\n");
					return nRet;
				}
			}
			break;
		default:
			break;
	}

	return ERR_OK;
}

STATIC HERROR svc_swup_WriteEmergencyUpdateInfoByIP(SvcSwUpdate_EmergencyUpdateInfo_t * pstInfo)
{
	HERROR				nRet;
	INET_IP_CONFIG_e	ipConfig;

	if (pstInfo->eSource != eDxSWUPDATE_SOURCE_Ip)
	{
		HxLOG_Error("Not Match EmergentyUpdate Type\n");
		return ERR_FAIL;
	}

	if (pstInfo->unInfo.stIpInfo.bHdcp == TRUE)
		ipConfig 	= eINET_IP_CONFIG_DHCP;
	else
		ipConfig	= eINET_IP_CONFIG_MANUAL;


	if (PalParam_Activated())
	{
		nRet = PAL_PARAM_SetField(ePAL_PARAM_NET_IP_CONFIG, (void *)&ipConfig, sizeof(INET_IP_CONFIG_e));
		if (nRet != ERR_OK)
		{
			HxLOG_Error("Failed PAL_PARAM_SetField(ePAL_PARAM_NET_IP_CONFIG) !\n");
			return nRet;
		}

		if (ipConfig == eINET_IP_CONFIG_MANUAL)
		{
			INET_IP_SETTING_t	stSetting;

			HxSTD_MemCopy(stSetting.aucIp,			&pstInfo->unInfo.stIpInfo.aucIpSetting[0], sizeof(HUINT8)*INET_ADDR_LENGTH);
			HxSTD_MemCopy(stSetting.aucMask,		&pstInfo->unInfo.stIpInfo.aucIpSetting[1], sizeof(HUINT8)*INET_ADDR_LENGTH);
			HxSTD_MemCopy(stSetting.aucGateway, 	&pstInfo->unInfo.stIpInfo.aucIpSetting[2], sizeof(HUINT8)*INET_ADDR_LENGTH);
			HxSTD_MemCopy(stSetting.aucDns, 		&pstInfo->unInfo.stIpInfo.aucIpSetting[3], sizeof(HUINT8)*INET_ADDR_LENGTH);

			nRet = PAL_PARAM_SetField(ePAL_PARAM_NET_IP_SETTING, (void *)&stSetting, sizeof(INET_IP_SETTING_t));
			if (nRet != ERR_OK)
			{
				HxLOG_Error("Failed PAL_PARAM_SetField(ePAL_PARAM_NET_IP_SETTING) !\n");
				return nRet;
			}
		}

		nRet = PAL_PARAM_SetField(ePAL_PARAM_OTA_FTP_INFO, (void *)&(pstInfo->unInfo.stIpInfo.stEmInfo), sizeof(SwUpdate_IpEmergencyInfo_t) );
		if (nRet != ERR_OK)
		{
			HxLOG_Error("Failed PAL_PARAM_SetField(ePAL_PARAM_OTA_FTP_INFO) !\n");
			return nRet;
		}
	}
	else
	{
		nRet	= PAL_SYS_SetNetworkIpConfig(ipConfig);
		if (nRet != ERR_OK)
		{
			HxLOG_Error("Failed PAL_SYS_SetNetworkIpConfig() !\n");
			return nRet;
		}

		if (ipConfig == eINET_IP_CONFIG_MANUAL)
		{
			nRet	= PAL_SYS_SetNetworkIp( (HULONG)pstInfo->unInfo.stIpInfo.aucIpSetting[0],
											(HULONG)pstInfo->unInfo.stIpInfo.aucIpSetting[1],
											(HULONG)pstInfo->unInfo.stIpInfo.aucIpSetting[2],
											(HULONG)pstInfo->unInfo.stIpInfo.aucIpSetting[3]);
			if (nRet != ERR_OK)
			{
				HxLOG_Error("Failed PAL_SYS_SetNetworkIp() !\n");
				return nRet;
			}
		}

		nRet	= PAL_SYS_SetFTPInfo( (HUINT8*)&(pstInfo->unInfo.stIpInfo.stEmInfo) );
		if (nRet != ERR_OK)
		{
			HxLOG_Error("Failed PAL_SYS_SetFTPInfo() !\n");
			return nRet;
		}
	}

	return ERR_OK;
}

STATIC HERROR svc_swup_WriteEmergencyUpdateInfoByFILE(SvcSwUpdate_EmergencyUpdateInfo_t * pstInfo)
{
	if (pstInfo->eSource != eDxSWUPDATE_SOURCE_File)
	{
		HxLOG_Error("Not Match EmergentyUpdate Type\n");
		return ERR_FAIL;
	}
	return ERR_OK;
}

STATIC HERROR		svc_swup_WriteEmergencyChannelType(DxSwUpdate_Source_e eSource, DxDeliveryType_e eDeliType)
{
	HERROR		hErr;
	PAL_EMERGENCY_OTA_CHANNEL_t	eEmergeChannel = 0;

	hErr = PAL_SYS_ReadEmergencySignalling(PAL_EMERGENCY_FS_PARTITION_RW, PAL_EMERGENCY_OTA_CHANNEL_NAME,
											0, (void *)&eEmergeChannel, sizeof(PAL_EMERGENCY_OTA_CHANNEL_t));
	if(hErr != ERR_OK)
		eEmergeChannel = 0;

#if defined(CONFIG_PROD_HDR1000S)
	/* Field 에 깔린 BOX들 때문에 들어가야 하는 코드임. */
	if(eEmergeChannel & 0x76)
		eEmergeChannel = 0;
#endif

	HxLOG_Debug("eSource : 0x%02X , eEmergeChannel : 0x%02x \n", eSource, eEmergeChannel);

	if (eSource == eDxSWUPDATE_SOURCE_Rf)
	{
		switch(eDeliType)
		{
			case eDxDELIVERY_TYPE_SAT:
				eEmergeChannel	|= PAL_EMERGENCY_OTA_CHANNEL_SAT;	break;
			case eDxDELIVERY_TYPE_TER:
				eEmergeChannel	|= PAL_EMERGENCY_OTA_CHANNEL_TER;	break;
			case eDxDELIVERY_TYPE_CAB:
				eEmergeChannel	|= PAL_EMERGENCY_OTA_CHANNEL_CAB;	break;
			case eDxDELIVERY_TYPE_MULTICAST:
				eEmergeChannel	|= PAL_EMERGENCY_OTA_CHANNEL_FTP;	break;
			default:
				return ERR_FAIL;
		}
	}
	else if(eSource == eDxSWUPDATE_SOURCE_Ip)
	{
		eEmergeChannel |= PAL_EMERGENCY_OTA_CHANNEL_FTP;
	}
	else if(eSource == eDxSWUPDATE_SOURCE_File)
	{
		eEmergeChannel |= PAL_EMERGENCY_OTA_CHANNEL_BG;
	}

	HxLOG_Debug("Set Emergency OTA Flag : 0x%02X\n", eEmergeChannel);

	return PAL_SYS_WriteEmergencySignalling(PAL_EMERGENCY_FS_PARTITION_RW, PAL_EMERGENCY_OTA_CHANNEL_NAME,
											0, &eEmergeChannel, sizeof(PAL_EMERGENCY_OTA_CHANNEL_t));
}

STATIC HERROR		svc_swup_WriteEmergencySettingAfterWriteSucc(DxDeliveryType_e eDeliType)
{
	HERROR hErr;
	PAL_EMERGENCY_OTA_CHANNEL_t eEmergeChannel	= 0;

	hErr = PAL_SYS_ReadEmergencySignalling(PAL_EMERGENCY_FS_PARTITION_RW, PAL_EMERGENCY_OTA_CHANNEL_NAME,
											0, (void *)&eEmergeChannel, sizeof(PAL_EMERGENCY_OTA_CHANNEL_t));
	if(hErr != ERR_OK)
		eEmergeChannel = 0;

#if defined(CONFIG_PROD_HDR1000S)
	/* Field 에 깔린 BOX들 때문에 들어가야 하는 코드임. */
	if(eEmergeChannel & 0x76)
		eEmergeChannel = 0;
#endif

	HxLOG_Debug("Load Emergency OTA Flag : 0x%02X\\n", eEmergeChannel);

	switch(eDeliType)
	{
		case eDxDELIVERY_TYPE_SAT:
			eEmergeChannel	|=	PAL_EMERGENCY_OTA_CHANNEL_SAT;		break;
		case eDxDELIVERY_TYPE_TER:
			eEmergeChannel	|=	PAL_EMERGENCY_OTA_CHANNEL_TER;		break;
		case eDxDELIVERY_TYPE_CAB:
			eEmergeChannel	|=	PAL_EMERGENCY_OTA_CHANNEL_CAB;		break;
		case eDxDELIVERY_TYPE_MULTICAST:
			eEmergeChannel	|=	PAL_EMERGENCY_OTA_CHANNEL_FTP;		break;
		default:
			return ERR_FAIL;
	}

	eEmergeChannel |= PAL_EMERGENCY_OTA_CHANNEL_BG;

	HxLOG_Debug("Load Emergency OTA Flag : 0x%02X\n", eEmergeChannel);

	return PAL_SYS_WriteEmergencySignalling(PAL_EMERGENCY_FS_PARTITION_RW, PAL_EMERGENCY_OTA_CHANNEL_NAME,
											0, &eEmergeChannel, sizeof(PAL_EMERGENCY_OTA_CHANNEL_t));
	}

STATIC HERROR		svc_swup_ClearEmergencySettingFlag(PAL_EMERGENCY_OTA_CHANNEL_t eEmergeFlag)
{
	HERROR hErr;
	PAL_EMERGENCY_OTA_CHANNEL_t eEmergeChannel	= 0;

	hErr = PAL_SYS_ReadEmergencySignalling(PAL_EMERGENCY_FS_PARTITION_RW, PAL_EMERGENCY_OTA_CHANNEL_NAME,
											0, (void *)&eEmergeChannel, sizeof(PAL_EMERGENCY_OTA_CHANNEL_t));
	if(hErr != ERR_OK)
		eEmergeChannel = 0;

	HxLOG_Debug("Load Emergency OTA Flag : 0x%02X\n", eEmergeChannel);
	eEmergeChannel &= (~eEmergeFlag);
	HxLOG_Debug("Load Emergency OTA Flag : 0x%02X\n", eEmergeChannel);

	return PAL_SYS_WriteEmergencySignalling(PAL_EMERGENCY_FS_PARTITION_RW, PAL_EMERGENCY_OTA_CHANNEL_NAME,
											0, &eEmergeChannel, sizeof(PAL_EMERGENCY_OTA_CHANNEL_t));
}

/*******************************************************************/
/*****************      Global Functions       *********************/
/*******************************************************************/
HERROR		SVC_SWUPDATE_InstallWriteImage(DxSwUpdate_Source_e eSource, HUINT8 *pucSourceFile)
{
	HINT32		nResult = 0;

	//HLIB_LOG_SetLevel(DOMAIN_TVSERVICE_SWUP_INSTALL, HxLOG_FATAL);

	if (pucSourceFile == NULL)
	{
		HxLOG_Error("source file path of writeImage is NULL!\n");
		return ERR_FAIL;
	}

	switch(eSource)
	{
		case eDxSWUPDATE_SOURCE_File:
			if (NULL == pucSourceFile)
				return ERR_FAIL;
			nResult = VK_TASK_Create(	svc_swup_Task,
								MW_SWUP_INSTALL_TASK_PRIORITY,
								MW_SWUP_INSTALL_TASK_SIZE,
								(const char*)"mw_panel_task",
								(void*)pucSourceFile,
								(unsigned long*)&s_ulSwupInstallTaskId,
								0);
			if (nResult != VK_OK)
			{
				HxLOG_Error("[%s.%d] VK_TASK_Create fail!!\n",__FUNCTION__,__LINE__);
				return ERR_FAIL;
			}

			nResult	= VK_TASK_Start(s_ulSwupInstallTaskId);
			if (nResult != VK_OK)
			{
				HxLOG_Print("VK_TASK_Start fail!!\n");
				return ERR_FAIL;
			}
			break;

		default:
			break;
	}

	return ERR_OK;
}

HERROR		SVC_SWUPDATE_InstallWriteEmergencySignalInfo(SvcSwUpdate_EmergencyUpdateInfo_t *pstEmergeInfo)
{
	if(pstEmergeInfo == NULL)
		return ERR_FAIL;

	if(pstEmergeInfo->eSource == eDxSWUPDATE_SOURCE_Rf)
	{
		DbSvc_TuningInfo_t	*pstSvcTuningInfo	= &pstEmergeInfo->unInfo.stRfInfo.stSvcTsInfo.stTuningParam;
		HxLOG_Debug(
			"\n\t eSource                 : 0x%X (RF)"
			"\n\t eDelitype_in_stRfInfo   : 0x%X"
			"\n\t eDelitype_in_stSvcInfo  : 0x%X"
			"\n\t eDownloadType           : 0x%X"
			"\n\t usPid                   : 0x%X"
			"\n\t fecCodeRate             : 0x%X"
			"\n\t ulSymbolrate            : 0x%X (%d)"
			"\n\t ulFrequency             : 0x%X (%d)"
			"\n\t ePilot                  : 0x%X"
			"\n\t ePolarization           : 0x%X"
			"\n\t ePskMod                 : 0x%X"
			"\n\t eRollOff                : 0x%X"
			"\n\t eSatType                : 0x%X"
			"\n\t eTransSpec              : 0x%X"
			"\n\t ucAntId                 : 0x%X\n\n\n"
			,pstEmergeInfo->eSource
			,pstEmergeInfo->unInfo.stRfInfo.eDeliType
			,pstEmergeInfo->unInfo.stRfInfo.stSvcTsInfo.eDeliType
			,pstEmergeInfo->unInfo.stRfInfo.eDownloadType
			,pstEmergeInfo->unInfo.stRfInfo.usPid
			,pstSvcTuningInfo->sat.eFecCodeRate
			,pstSvcTuningInfo->sat.ulSymbolRate, pstSvcTuningInfo->sat.ulSymbolRate
			,pstSvcTuningInfo->sat.ulFrequency, pstSvcTuningInfo->sat.ulFrequency
			,pstSvcTuningInfo->sat.ePilot
			,pstSvcTuningInfo->sat.ePolarization
			,pstSvcTuningInfo->sat.ePskMod
			,pstSvcTuningInfo->sat.eRollOff
			,pstSvcTuningInfo->sat.eSatType
			,pstSvcTuningInfo->sat.eTransSpec
			,pstSvcTuningInfo->sat.ucAntId);

		svc_swup_WriteEmergencyChannelType(pstEmergeInfo->eSource, pstEmergeInfo->unInfo.stRfInfo.eDeliType);
	}
	else if(pstEmergeInfo->eSource == eDxSWUPDATE_SOURCE_Ip)
	{
		svc_swup_WriteEmergencyChannelType(pstEmergeInfo->eSource, pstEmergeInfo->unInfo.stRfInfo.eDeliType);
	}

	if (SVC_SWUPDATE_InstallWriteEmergencyUpdateInfo(pstEmergeInfo->eSource, pstEmergeInfo) == ERR_OK)
	{
		HxLOG_Debug("\n\n\nSuccessed!!! WriteEmergencyUpdateInfo !!!\n\n\n");
		return ERR_OK;
	}
	else
	{
		HxLOG_Debug("\n\n\nFailed!!! WriteEmergencyUpdateInfo !!!\n\n\n");
		return ERR_FAIL;
	}
}

HERROR		SVC_SWUPDATE_InstallWriteEmergencySettingAfterWriteSucc(DxDeliveryType_e eDeliType)
{
	HxLOG_Debug("\n\n\nSetting Emergency Finaly Set eDelitype : %d\n\n\n",eDeliType);
	return svc_swup_WriteEmergencySettingAfterWriteSucc(eDeliType);
}

HERROR		SVC_SWUPDATE_InstallWriteEmergencyUpdateInfo(DxSwUpdate_Source_e eSource, SvcSwUpdate_EmergencyUpdateInfo_t *pstInfo)
{
	HERROR	hErr = ERR_FAIL;

	if (pstInfo == NULL)
	{
		HxLOG_Error("SvcSwUpdate_EmergencyUpdateInfo_t is NULL!!\n");
		return ERR_FAIL;
	}

	switch(eSource)
	{
		case eDxSWUPDATE_SOURCE_Rf:
			hErr = svc_swup_WriteEmergencyUpdateInfoByRF(pstInfo);
			break;

		case eDxSWUPDATE_SOURCE_Ip:
			hErr = svc_swup_WriteEmergencyUpdateInfoByIP(pstInfo);
			break;

		case eDxSWUPDATE_SOURCE_File:
			hErr = svc_swup_WriteEmergencyUpdateInfoByFILE(pstInfo);
			break;

		default:
			break;
	}

	if(hErr != ERR_OK)
	{
		HxLOG_Error("FAILED SVC_SWUPDATE_InstallWriteEmergencyUpdateInfo !!\n");
		svc_swup_UtilPostMsgToMgr(eSEVT_SWUP_INSTALL_WRITE_IMAGE_SUCCESS, (Handle_t)NULL, 0, 0, 0);
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR	SVC_SWUPDATE_InstallClearEmergencySettingFlag(HUINT32 ulEmergeFlag)
{
	return svc_swup_ClearEmergencySettingFlag(ulEmergeFlag);
}

HERROR		SVC_SWUPDATE_InstallCancel(void)
{
	svc_swup_InstallSetStatus(eSWUP_STATE_Cancle);

	// TO DO

	return ERR_OK;
}

HERROR		SVC_SWUPDATE_InstallFinalize(HUINT32 ulVer)
{
	HERROR		hErr;

	if (PalParam_Activated())
	{
		hErr = PAL_PARAM_SetField(ePAL_PARAM_APP_VERSION_NEXT, (void *)&ulVer, sizeof(HUINT32));
		if (hErr != ERR_OK)
		{
			HxLOG_Error("Failed ePAL_PARAM_APP_VERSION_NEXT !\n");
			return hErr;
		}

		ulVer = (HUINT32)eSWUP_DOWNLOAD_FILE;
		hErr = PAL_PARAM_SetField(ePAL_PARAM_SWUP_TYPE1, (void *)&ulVer, sizeof(SWUP_DownloadType_e));
		if (hErr != ERR_OK)
		{
			HxLOG_Error("Failed ePAL_PARAM_SWUP_TYPE1 !\n");
			return hErr;
		}

		ulVer = (HUINT32)eSWUP_CHANNEL_FLASH;
		hErr = PAL_PARAM_SetField(ePAL_PARAM_SWUP_CH_TYPE1, (void *)&ulVer, sizeof(SWUP_ChannelType_e));
		if (hErr != ERR_OK)
		{
			HxLOG_Error("Failed ePAL_PARAM_SWUP_CH_TYPE1 !\n");
			return hErr;
		}

		ulVer = (HUINT32)eSWUP_FLAG_INFO_DETECTED;
		hErr = PAL_PARAM_SetField(ePAL_PARAM_SWUP_FLAG1, (void *)&ulVer, sizeof(SWUP_Flag_e));
		if (hErr != ERR_OK)
		{
			HxLOG_Error("Failed ePAL_PARAM_SWUP_FLAG1 !\n");
			return hErr;
		}
	}
	else
	{
		hErr	= PAL_SYS_SetSystemVersion(eVERSION_TYPE_NEXTOTA, ulVer);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("Failed PAL_SYS_SetSystemVersion() !\n");
			return hErr;
		}

		hErr	= PAL_SYS_SetOtaType(eSWUP_INFO_SLOT_0, eSWUP_DOWNLOAD_FILE);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("Failed PAL_SYS_SetOtaType() !\n");
			return hErr;
		}

		hErr	= PAL_SYS_SetChannelType(eSWUP_INFO_SLOT_0, eSWUP_CHANNEL_FLASH);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("Failed PAL_SYS_SetChannelType() !\n");
			return hErr;
		}

		hErr 	= PAL_SYS_SetOtaFlag(eSWUP_INFO_SLOT_0, eSWUP_FLAG_INFO_DETECTED);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("Failed PAL_SYS_SetOtaFlag() !\n");
			return hErr;
		}
	}

	HxLOG_Debug("Successed S/W upgrade SetFlag\n\n");
	return ERR_OK;
}

