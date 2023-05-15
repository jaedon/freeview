/**
 * cmd_hwtest_interface.c
*/

/**
 * @defgroup		CMD_HWTEST
 * @author			Chung Dong Chul
 * @note
 * @brief
 * @file 			cmd_hwtest_interface.c
*/

#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "htype.h"
#include "cmd_hwtest_interface.h"
#include "cmd_hwtest_parser_system.h"
#include "cmdtool.h"
#include "drv_smart.h"
#include "di_audio.h"
#include "di_channel.h"
#include "di_hdmi.h"
#include "di_scart.h"
#include "di_video.h"
#include "di_osd.h"
#include "di_key.h"
#include "di_usb.h"
#include "cmdtool.h"
#include "di_system.h"
#include "di_hdd.h"
#if defined(CONFIG_USB_HID)
#include "di_usb_hid.h"
#endif
#if defined(CONFIG_FRONTEND_SAT)
#include "di_channel_s.h"
#endif
#if defined(CONFIG_FRONTEND_CAB)
#include "di_channel_c.h"
#endif
#if defined(CONFIG_FRONTEND_TER)
#include "di_channel_t.h"
#endif
#include "vkernel.h"
#include "di_usb.h"
#if defined(CONFIG_CI)
#include "di_ci.h"
#endif
#if defined(CONFIG_MMC)
#include "di_mmc.h"
#endif

#define USE_POSIX_API_ON_EEP_EMU
#if defined(USE_POSIX_API_ON_EEP_EMU)
#include <sys/types.h>
#include <unistd.h>
#endif
#if defined(CONFIG_FAN)
#include "drv_fan.h"
#endif

#if 0
static void (*s_fpHwTest_DisplayMsg)(char *pMsg);
static void (*s_fpHwTest_DisplayInfo)(char *pMsg);
static void (*s_fpHwTest_DisplayProgressBar)(char *pMsg, int iProgress);
#endif
#define CMD_HWTEST_PRINT_ERROR() VK_printf("Error(%s,%d)\n", __FUNCTION__, __LINE__);

void HI_ReadTemp(unsigned char* pucTemp)
{
    if ( pucTemp == NULL )
    {
        CMD_HWTEST_PRINT_ERROR();
        VK_TASK_Sleep(1000);
        return;
    }
#if defined(CONFIG_SMART)
	DRV_SMART_Read_Attr_Temp(pucTemp);
#else
	*pucTemp=0;
#endif
}

#if defined(CONFIG_CHANNEL)
unsigned long HI_CH_GetNumOfInstance(void)
{
	HUINT32 num_tuner;
	DI_CH_GetNumOfInstance(&num_tuner);
	return num_tuner;
}

void HI_CH_DI_RegisterCallback(HI_CH_Type_e chType, unsigned long instanceId, pfnHI_CH_NotifySignalInfo pfnCallback)
{
	if ( chType == HI_CH_TYPE_SAT )
	{
#if defined(CONFIG_FRONTEND_SAT)
		DI_CH_S_RegisterCallback( instanceId, (pfnDI_CH_NotifySignalInfo)pfnCallback);
#endif
	}
	else if ( chType == HI_CH_TYPE_CAB )
	{
#if defined(CONFIG_FRONTEND_CAB)
		DI_CH_C_RegisterCallback( instanceId, (pfnDI_CH_NotifySignalInfo)pfnCallback);
#endif
	}
	else if ( chType == HI_CH_TYPE_TER )
	{
#if defined(CONFIG_FRONTEND_TER)
		DI_CH_T_RegisterCallback( instanceId, (pfnDI_CH_NotifySignalInfo)pfnCallback);
#endif
	}
	else if ( chType == HI_CH_TYPE_ANA )
	{
#if defined (FRONTEND_ANA)
		DI_CH_A_RegisterCallback( instanceId, (pfnDI_CH_NotifySignalInfo)pfnCallback);
#endif
	}
}

void HI_CH_DI_GetLockingInfo(HI_CH_Type_e chType, unsigned long instanceId, HI_CH_LockingInfo *pLockingInfo)
{
	(void)instanceId;
	(void)pLockingInfo;

	if ( chType == HI_CH_TYPE_SAT )
	{
#if defined(CONFIG_FRONTEND_SAT)
		CH_SAT_TuneParam_t satInfo;
		DI_CH_S_GetLockedInfo(instanceId, &satInfo);
		pLockingInfo->ul_Frequency = satInfo.ulFrequency;
		pLockingInfo->ul_SymbolRate = satInfo.ulSymbolRate;
		pLockingInfo->ul_Sat_Diseqc = satInfo.etAntennaType;
		pLockingInfo->ul_Sat_FEC = satInfo.etFecCodeRate;
		pLockingInfo->ul_Sat_Modulation = satInfo.etModulation;
		pLockingInfo->ul_Sat_Pilot = satInfo.etPilot;
		pLockingInfo->ul_Sat_Polarity = satInfo.etPolarization;
		pLockingInfo->ul_Sat_DvbMode = satInfo.etTransSpec;
		pLockingInfo->ul_Sat_ucTone = satInfo.etToneBurst;
#endif
	}
	else if ( chType == HI_CH_TYPE_CAB )
	{
#if defined(CONFIG_FRONTEND_CAB)

#endif
	}
	else if ( chType == HI_CH_TYPE_TER )
	{
#if defined(CONFIG_FRONTEND_TER)

#endif
	}
	else if ( chType == HI_CH_TYPE_ANA )
	{
#if defined (FRONTEND_ANA)

#endif
	}
}

void HI_CH_DI_GetChannelStatus(HI_CH_Type_e chType, unsigned long instanceId, HI_CH_ChannelStatus *pChStatus)
{
	(void)instanceId;
	(void)pChStatus;

	if ( chType == HI_CH_TYPE_SAT )
	{
#if defined(CONFIG_FRONTEND_SAT)
		DI_CH_S_GetChannelStatus(
			instanceId,
			&pChStatus->ul_Sat_Ber,
			&pChStatus->ul_Sat_RfAgc,
			&pChStatus->ul_Sat_IfAgc,
			&pChStatus->ul_Sat_Uncorrect,
			&pChStatus->f_Sat_Power);

#endif
	}
	else if ( chType == HI_CH_TYPE_CAB )
	{
#if defined(CONFIG_FRONTEND_CAB)

#endif
	}
	else if ( chType == HI_CH_TYPE_TER )
	{
#if defined(CONFIG_FRONTEND_TER)

#endif
	}
	else if ( chType == HI_CH_TYPE_ANA )
	{
#if defined (FRONTEND_ANA)

#endif
	}
}
void HI_CH_DI_CancelLock(unsigned long instanceId, HI_CH_Type_e chType)
{
	if ( chType == HI_CH_TYPE_SAT )
	{
#if defined(CONFIG_FRONTEND_SAT)
		DI_CH_S_CancelLock(instanceId);
#endif
	}
	else if ( chType == HI_CH_TYPE_CAB )
	{
#if defined(CONFIG_FRONTEND_CAB)
                DI_CH_C_CancelLock(instanceId);
#endif
	}
	else if ( chType == HI_CH_TYPE_TER )
	{
#if defined(CONFIG_FRONTEND_TER)
		DI_CH_T_CancelLock(instanceId);

#endif
	}
}
int HI_CH_DI_RequestLock(unsigned long requestId, unsigned long instanceId, HI_CH_Type_e chType, HI_TuneParams tuneParams)
{
	if ( chType == HI_CH_TYPE_SAT )
	{
#if defined(CONFIG_FRONTEND_SAT)
		CH_SAT_TuneParam_t satTune;
		satTune.etAntennaType = tuneParams.satParams.etAntennaType;
		satTune.ulFrequency = tuneParams.satParams.ulFrequency;
		satTune.ulSymbolRate = tuneParams.satParams.ulSymbolRate;
		satTune.etFecCodeRate = tuneParams.satParams.etFecCodeRate;
		satTune.etPolarization = tuneParams.satParams.etPolarization;
		satTune.etTransSpec = tuneParams.satParams.etTransSpec;
		satTune.etModulation = tuneParams.satParams.etModulation;
		satTune.etPilot = tuneParams.satParams.etPilot;
		satTune.etRollOff = tuneParams.satParams.etRollOff;
		satTune.etLnbVoltage = tuneParams.satParams.etLnbVoltage;
		satTune.etDiseqcVersion = tuneParams.satParams.etDiseqcVersion;
		satTune.etDiseqcInput = tuneParams.satParams.etDiseqcInput;
		satTune.etToneBurst = tuneParams.satParams.etToneBurst;
		satTune.b22kTone = tuneParams.satParams.b22kTone;
		satTune.ulScdMasterUnitId = tuneParams.satParams.ulScdMasterUnitId;
		satTune.ulScdUserBandNum = tuneParams.satParams.ulScdUserBandNum;
		satTune.ulScdUserBandFreq = tuneParams.satParams.ulScdUserBandFreq;
		VK_printf("%s_Called\n", __FUNCTION__);
		return DI_CH_S_RequestLock(instanceId,
			requestId,
			&satTune);
#endif
	}
	else if ( chType == HI_CH_TYPE_CAB )
	{
#if defined(CONFIG_FRONTEND_CAB)
		CH_CAB_TuneParam_t cabTune;
		cabTune.ulFrequency = tuneParams.cabParams.ulFrequency;
		cabTune.ulSymbolRate = tuneParams.cabParams.ulSymbolRate;
		cabTune.etModulation = tuneParams.cabParams.etModulation;
		cabTune.etSpectrum = tuneParams.cabParams.etSpectrum;
		cabTune.nNetId = tuneParams.cabParams.nNetId;
		cabTune.nTsId = tuneParams.cabParams.nTsId;
		return DI_CH_C_RequestLock(instanceId,
			requestId,
			&cabTune);
#endif
	}
	else if ( chType == HI_CH_TYPE_TER )
	{
#if defined(CONFIG_FRONTEND_TER)
		CH_TER_TuneParam_t terTune;
		DI_CH_T_SetAntennaPower( instanceId, DI_CH_ANTENNA_ON );

		terTune.ulFrequency = tuneParams.terParams.ulFrequency;
		terTune.etBandWidth = tuneParams.terParams.etBandWidth;
		terTune.etModulation = tuneParams.terParams.etModulation;
		terTune.etCodeRate = tuneParams.terParams.etCodeRate;
		terTune.etTransMode = tuneParams.terParams.etTransMode;
		terTune.etGuardInterval = tuneParams.terParams.etGuardInterval;
		terTune.etStream = tuneParams.terParams.etStream;
		terTune.etOffset = tuneParams.terParams.etOffset;
		terTune.etTerSystem = tuneParams.terParams.etTerSystem;
		terTune.ulFrequency = tuneParams.terParams.ulFrequency;
		terTune.ulFrequency = tuneParams.terParams.ulFrequency;
		terTune.stT2Param.ulPlpId = tuneParams.terParams.ulPlpId;
		terTune.stT2Param.etPreambleFormat = tuneParams.terParams.etPreambleFormat;
		terTune.stT2Param.etPreambleMixed = tuneParams.terParams.etPreambleMixed;
		terTune.stT2Param.etPilotPattern = tuneParams.terParams.etPilotPattern;
		terTune.stT2Param.etExtendedCarrier = tuneParams.terParams.etExtendedCarrier;
		terTune.stT2Param.etPaprMode = tuneParams.terParams.etPaprMode;
		terTune.stT2Param.ulPlpNumBlocks = tuneParams.terParams.ulPlpNumBlocks;
		VK_printf("%s-Called for Ter\n", __FUNCTION__);
		return DI_CH_T_RequestLock(instanceId,
			requestId,
			&terTune);
#endif
	}
	else if ( chType == HI_CH_TYPE_ANA )
	{
#if defined (FRONTEND_ANA)

#endif
	}

	return -1;
}


void HI_CH_DI_GetSignalInfo(HI_CH_Type_e chType, unsigned long instanceId, HI_CH_SignalInfo *pChStatus)
{
	if ( chType == HI_CH_TYPE_SAT )
	{
#if defined(CONFIG_FRONTEND_SAT)
		DI_CH_SignalQuality_t signalQuality;
		DI_CH_S_GetSignalInfo(instanceId, &signalQuality);
		pChStatus->ul_Strength = signalQuality.ulStrength;
		pChStatus->ul_Quality = signalQuality.ulQuality;
		pChStatus->ul_Agc = signalQuality.ulAgc;
		pChStatus->f_Ber = signalQuality.fBer;
		pChStatus->f_Snr = signalQuality.fSnr;
		pChStatus->ul_Emergencyflag = signalQuality.bEmergencyflag;
		pChStatus->ul_CorrectedNo = signalQuality.ulCorrectedNo;
		pChStatus->ul_UnCorrectedNo = signalQuality.ulUnCorrectedNo;
		pChStatus->ul_UnErroredNo = signalQuality.ulUnErroredNo;
		pChStatus->ul_InbandBer = signalQuality.ulInbandBer;
		pChStatus->ul_UnCorrectedFlag = signalQuality.ulUnCorrectedFlag;
		pChStatus->ul_RfAgc = signalQuality.ulRfAgc;
		pChStatus->ul_IfAgc = signalQuality.ulIfAgc;
		pChStatus->f_SignalInputPower = signalQuality.fSignalInputPower;
#endif
	}
	else if ( chType == HI_CH_TYPE_CAB )
	{
#if defined(CONFIG_FRONTEND_CAB)
		DI_CH_SignalQuality_t signalQuality;
		DI_CH_C_GetSignalInfo(instanceId, &signalQuality);
		pChStatus->ul_Strength = signalQuality.ulStrength;
		pChStatus->ul_Quality = signalQuality.ulQuality;
		pChStatus->ul_Agc = signalQuality.ulAgc;
		pChStatus->f_Ber = signalQuality.fBer;
		pChStatus->f_Snr = signalQuality.fSnr;
		pChStatus->ul_Emergencyflag = signalQuality.bEmergencyflag;
		pChStatus->ul_CorrectedNo = signalQuality.ulCorrectedNo;
		pChStatus->ul_UnCorrectedNo = signalQuality.ulUnCorrectedNo;
		pChStatus->ul_UnErroredNo = signalQuality.ulUnErroredNo;
		pChStatus->ul_InbandBer = signalQuality.ulInbandBer;
		pChStatus->ul_UnCorrectedFlag = signalQuality.ulUnCorrectedFlag;
		pChStatus->ul_RfAgc = signalQuality.ulRfAgc;
		pChStatus->ul_IfAgc = signalQuality.ulIfAgc;
		pChStatus->f_SignalInputPower = signalQuality.fSignalInputPower;
#endif
	}
	else if ( chType == HI_CH_TYPE_TER )
	{
#if defined(CONFIG_FRONTEND_TER)
		DI_CH_SignalQuality_t signalQuality;
		DI_CH_T_GetSignalInfo(instanceId, &signalQuality);
		pChStatus->ul_Strength = signalQuality.ulStrength;
		pChStatus->ul_Quality = signalQuality.ulQuality;
		pChStatus->ul_Agc = signalQuality.ulAgc;
		pChStatus->f_Ber = signalQuality.fBer;
		pChStatus->f_Snr = signalQuality.fSnr;
		pChStatus->ul_Emergencyflag = signalQuality.bEmergencyflag;
		pChStatus->ul_CorrectedNo = signalQuality.ulCorrectedNo;
		pChStatus->ul_UnCorrectedNo = signalQuality.ulUnCorrectedNo;
		pChStatus->ul_UnErroredNo = signalQuality.ulUnErroredNo;
		pChStatus->ul_InbandBer = signalQuality.ulInbandBer;
		pChStatus->ul_UnCorrectedFlag = signalQuality.ulUnCorrectedFlag;
		pChStatus->ul_RfAgc = signalQuality.ulRfAgc;
		pChStatus->ul_IfAgc = signalQuality.ulIfAgc;
		pChStatus->f_SignalInputPower = signalQuality.fSignalInputPower;
#endif
	}
	else if ( chType == HI_CH_TYPE_ANA )
	{
#if defined (FRONTEND_ANA)

#endif
	}
}

unsigned int HI_CH_DI_GetLockedInfo(unsigned int ulInstanceId, unsigned char ucChType)
{
	int	nRet;

	if ( ucChType == 0 )				// HW Test : HWTEST_TUNER_SAT = 0
	{
#if defined(CONFIG_FRONTEND_SAT)
		CH_SAT_TuneParam_t satTune;
		nRet = DI_CH_S_GetLockedInfo(ulInstanceId, &satTune);
		if(nRet == DI_CH_OK)
		{
			return 0;
		}
#endif
	}
	else if ( ucChType == 1 )			// HW Test : HWTEST_TUNER_CAB = 1
	{
#if defined(CONFIG_FRONTEND_CAB)
		CH_CAB_TuneParam_t cabTune;
		nRet = DI_CH_C_GetLockedInfo(ulInstanceId, &cabTune);
		if(nRet == DI_CH_OK)
		{
			return 1;
		}
#endif
	}
	else if ( ucChType == 2 )			// HW Test : HWTEST_TUNER_TER = 2
	{
#if defined(CONFIG_FRONTEND_TER)
		CH_TER_TuneParam_t terTune;
		nRet = DI_CH_T_GetLockedInfo(ulInstanceId, &terTune);
		if(nRet == DI_CH_OK)
		{
			return 2;
		}
#endif
	}

	return 0xff;
}
void HI_CH_S_GetSignalInfo(unsigned int ulInstanceId, DI_CH_SignalQuality_t *pstHwTest )
{
	if ( ( pstHwTest == NULL ) )
    {
        CMD_HWTEST_PRINT_ERROR();
        VK_TASK_Sleep(1000);
        return;
    }
#if defined(CONFIG_FRONTEND_SAT)
	DI_CH_S_GetSignalInfo(ulInstanceId, pstHwTest);
#else
	HWTEST_UNUSED(ulInstanceId);
#endif
}

void HI_CH_C_GetSignalInfo(unsigned int ulInstanceId, DI_CH_SignalQuality_t *pstHwTest )
{
    if ( ( pstHwTest == NULL ) )
    {
        CMD_HWTEST_PRINT_ERROR();
        VK_TASK_Sleep(1000);
        return;
    }
#if defined(CONFIG_FRONTEND_CAB)
	DI_CH_C_GetSignalInfo(ulInstanceId, pstHwTest);
#else
	HWTEST_UNUSED(ulInstanceId);
#endif
}

void HI_CH_T_GetSignalInfo(unsigned int ulInstanceId, DI_CH_SignalQuality_t *pstHwTest )
{
	if ( ( pstHwTest == NULL ) )
    {
        CMD_HWTEST_PRINT_ERROR();
        VK_TASK_Sleep(1000);
        return;
    }
#if defined(CONFIG_FRONTEND_TER)
	DI_CH_T_GetSignalInfo(ulInstanceId, pstHwTest);
#else
	HWTEST_UNUSED(ulInstanceId);
#endif
}

#endif /* End of CONFIG_CHANNEL */

#define DRV_HWTEST_DB_FILENAME			"/var/lib/humaxtv/hwtest.db"
unsigned char s_aucNvram_buf[32*1024];
int hi_NVRAM_CheckFile  (void)
{
#if defined(USE_POSIX_API_ON_EEP_EMU)
   	int fd = -1;
	off_t file_size = 0;

	fd = open( DRV_HWTEST_DB_FILENAME, O_RDONLY);
	if(fd > -1)
	{
		file_size = lseek(fd, 0, SEEK_END );
		close(fd);
	}

	if (file_size != sizeof(s_aucNvram_buf))
	{
		VK_MEM_Memset(s_aucNvram_buf, 0xff, sizeof(s_aucNvram_buf));
		fd = open( DRV_HWTEST_DB_FILENAME, O_WRONLY|O_TRUNC|O_CREAT );
		if(fd == -1) return 1;
		write(fd, s_aucNvram_buf, sizeof(s_aucNvram_buf));
		fsync(fd);
		close(fd);
	}
#else
   	FILE* fp = NULL;
	int	file_size = 0;

	fp = fopen( DRV_HWTEST_DB_FILENAME, "rb" );
	if( NULL != fp )
	{
		fseek(fp, 0, SEEK_END );
		file_size = ftell(fp);
		fclose( fp );
	}

	if (file_size != 8*1024)
	{
		VK_memset(s_aucNvram_buf, 0xff, 8*1024);
		fp = fopen( DRV_HWTEST_DB_FILENAME, "wb" );
		if( NULL == fp ) return 1;

		fwrite( s_aucNvram_buf, 1, 8*1024, fp );

		fclose( fp );
	}
#endif
    return 0;
}

int hi_NVRAM_Write(HUINT32  usAddress, HUINT8 *pucBuffer, HUINT32  ulSize)
{
#if defined(USE_POSIX_API_ON_EEP_EMU)
	int fd = -1;

	if ( (pucBuffer == NULL) || (ulSize == 0)) return 1;
	hi_NVRAM_CheckFile();
	fd = open( DRV_HWTEST_DB_FILENAME, O_WRONLY );
	if(fd == -1) return 1;

	VK_MEM_Memcpy((unsigned char*)(s_aucNvram_buf+usAddress), (unsigned char*)pucBuffer, ulSize);
	write(fd, s_aucNvram_buf,sizeof(s_aucNvram_buf));
	fsync(fd);
	close(fd);
#else
   	FILE* fp = NULL;
	int	file_size;

	if ( (pucBuffer == NULL) || (ulSize == 0)) return 1;
	hi_NVRAM_CheckFile();
	fp = fopen( DRV_HWTEST_DB_FILENAME, "w" );
	if( NULL == fp ) return 1;

	VK_memcpy(s_aucNvram_buf+usAddress, pucBuffer, ulSize);
	fwrite( s_aucNvram_buf, 1, 8*1024, fp );

	fclose( fp );
#endif
    return 0;
}

int hi_NVRAM_Read  ( HUINT32  usAddress,
                                       HUINT8 *  pucBuffer,
                                       HUINT32  ulSize
                                     )
{
#if defined(USE_POSIX_API_ON_EEP_EMU)
	int fd = -1;
    ssize_t	lengthByPage;

    if ( (pucBuffer == NULL) || (ulSize == 0)) return 1;

	hi_NVRAM_CheckFile();
	fd = open( DRV_HWTEST_DB_FILENAME , O_RDONLY );
	if(fd == -1) return 1;

	lengthByPage = read(fd, s_aucNvram_buf, sizeof(s_aucNvram_buf));
	if( 0 > lengthByPage ){
		close(fd);
		return 1;
	}
	VK_MEM_Memcpy(pucBuffer, s_aucNvram_buf+usAddress, ulSize);

	close(fd);
#else
	FILE* fp = NULL;
    HUINT32		lengthByPage;
	int	file_size;

    if ( (pucBuffer == NULL) || (ulSize == 0)) return 1;

	hi_NVRAM_CheckFile();
	fp = fopen( DRV_HWTEST_DB_FILENAME , "rb" );
	if( NULL == fp ) return 1;

	lengthByPage = fread( s_aucNvram_buf,1 ,8*1024 , fp );
	if( 0 > lengthByPage ){
		fclose( fp );
		return 1;
	}
	VK_memcpy(pucBuffer, s_aucNvram_buf+usAddress, ulSize);

	fclose( fp );
#endif
    return 0;
}

int HI_EEPROM_Read(unsigned long address, unsigned char *data, unsigned long numToRead)
{
    if ( data == NULL )
    {
        CMD_HWTEST_PRINT_ERROR();
        VK_TASK_Sleep(1000);
        return 1;
    }
	return (int)hi_NVRAM_Read(address, data, numToRead);
}

int HI_EEPROM_Write(unsigned long address, unsigned char *data, unsigned long numToWrite)
{
    if ( data == NULL )
    {
        CMD_HWTEST_PRINT_ERROR();
        VK_TASK_Sleep(1000);
        return 1;
    }
	return (int)hi_NVRAM_Write(address, data, numToWrite);
}

int HI_HDMI_EnableOutput(int iEnable)
{
	return DI_HDMI_EnableOutput(iEnable);
}

int HI_HDMI_EnableHDCP(int iEnable)
{
	return DI_HDMI_EnableHDCP(iEnable);
}

int HI_HDMI_GetHDResolution(int *hdResolution)
{
	DI_HDMI_AVAILABLE_FORMATS	stCapa;
	DI_ERR_CODE 	 errError = DI_ERR_OK;

	errError = DI_HDMI_GetCapability(&stCapa);
	if(errError != DI_ERR_OK)
	{
		VK_printf("Error Reading HDMI Capability\n");
		*hdResolution = HI_VIDEO_HD_RESOLUTION_1080I_50HZ;
		return DI_ERR_ERROR;
	}
	else VK_printf("HDMI Capability Read : %x\n", stCapa.ulVideoFormats );

	if(stCapa.ulVideoFormats & DI_HDMI_VFORMAT_1080P)
	{
		*hdResolution = HI_VIDEO_HD_RESOLUTION_1080P;
	}
	else if(stCapa.ulVideoFormats & DI_HDMI_VFORMAT_1080P_50HZ)
	{
		*hdResolution = HI_VIDEO_HD_RESOLUTION_1080P_50HZ;
	}
	else if(stCapa.ulVideoFormats & DI_HDMI_VFORMAT_1080I)
	{
		*hdResolution = HI_VIDEO_HD_RESOLUTION_1080I;
	}
	else if(stCapa.ulVideoFormats & DI_HDMI_VFORMAT_1080I_50HZ)
	{
		*hdResolution = HI_VIDEO_HD_RESOLUTION_1080I_50HZ;
	}
	else if(stCapa.ulVideoFormats & DI_HDMI_VFORMAT_720P)
	{
		*hdResolution = HI_VIDEO_HD_RESOLUTION_720P;
	}
	else if(stCapa.ulVideoFormats & DI_HDMI_VFORMAT_720P_50HZ)
	{
		*hdResolution = HI_VIDEO_HD_RESOLUTION_720P_50HZ;
	}
	else if(stCapa.ulVideoFormats & (DI_HDMI_VFORMAT_576P | DI_HDMI_VFORMAT_576P_4_3))
	{
		*hdResolution = HI_VIDEO_HD_RESOLUTION_576P;
	}
	else if(stCapa.ulVideoFormats & (DI_HDMI_VFORMAT_576I | DI_HDMI_VFORMAT_576I_4_3) )
	{
		*hdResolution = HI_VIDEO_HD_RESOLUTION_576I;
	}
	else if(stCapa.ulVideoFormats & (DI_HDMI_VFORMAT_480P | DI_HDMI_VFORMAT_480P_4_3))
	{
		*hdResolution = HI_VIDEO_HD_RESOLUTION_480P;
	}
	else if(stCapa.ulVideoFormats & (DI_HDMI_VFORMAT_480I | DI_HDMI_VFORMAT_480I_4_3 | DI_HDMI_VFORMAT_480I_J))
	{
		*hdResolution = HI_VIDEO_HD_RESOLUTION_480I;
	}
	else
	{
		// DEFAULT
		*hdResolution = HI_VIDEO_HD_RESOLUTION_1080I_50HZ;
		VK_printf("HDMI Resoultion to default 1080i\n");
	}

	return ERR_OK;
}



int HI_HDMI_RegisterEventNotify(HI_HDMI_EVENT eEvent, CALLBACK_TYPE1 pfCallback)
{
    if ( pfCallback == NULL )
    {
        CMD_HWTEST_PRINT_ERROR();
        VK_TASK_Sleep(1000);
        return 1;
    }
	return DI_HDMI_RegisterEventNotify(eEvent, pfCallback);
}

int HI_VK_TASK_Create(void (*start_func)(void *), unsigned long prio,unsigned long stack_size, const char *name, void *arg, unsigned long *taskId, int IsJoinable)
{
    if ( ( start_func == NULL ) ||
         ( taskId == NULL ))
    {

        CMD_HWTEST_PRINT_ERROR();
        VK_TASK_Sleep(1000);
        return 1;
    }

	return VK_TASK_Create(start_func, prio, stack_size, name, arg, taskId, IsJoinable);
}

int HI_VK_TASK_Start(unsigned long taskId)
{
	return VK_TASK_Start(taskId);
}

int HI_VK_TASK_Stop(unsigned long taskId)
{
	return VK_TASK_Stop(taskId);
}

int HI_VK_TASK_Sleep(unsigned long ultime)
{
	return VK_TASK_Sleep(ultime);
}

int HI_VK_TASK_Destroy(unsigned long taskId)
{
	return VK_TASK_Destroy(taskId);
}

int HI_VK_MSG_Create(unsigned long q_count, unsigned long msg_size, const char *q_name, unsigned long *q_id, HI_VK_SUSPENDTYPE eSuspendType)
{
    if ( q_id == NULL )
    {
        CMD_HWTEST_PRINT_ERROR();
        VK_TASK_Sleep(1000);
        return 1;
    }
	return VK_MSG_Create(q_count, msg_size, q_name, q_id, eSuspendType);
}

int HI_VK_MSG_Send(unsigned long q_id, const void *msg, unsigned long size)
{
    if ( msg == NULL )
    {
        CMD_HWTEST_PRINT_ERROR();
        VK_TASK_Sleep(1000);
        return 1;
    }
	return VK_MSG_Send(q_id,  msg,  size);
}

int HI_VK_MSG_Receive(unsigned long q_id, void *msg, unsigned long size)
{
    if ( msg == NULL )
    {
        CMD_HWTEST_PRINT_ERROR();
        VK_TASK_Sleep(1000);
        return 1;
    }
	return VK_MSG_Receive(q_id, msg,  size);
}

int HI_VK_MSG_ReceiveTimeout(unsigned long q_id, void *msg, unsigned long size, unsigned long timeout)
{
    if ( msg == NULL )
    {
        CMD_HWTEST_PRINT_ERROR();
        VK_TASK_Sleep(1000);
        return 1;
    }
	return VK_MSG_ReceiveTimeout(q_id, msg,  size,  timeout);
}

int HI_VK_SEM_Create(unsigned long * sem_id, const char *name, HI_VK_SUSPENDTYPE eSuspendType)
{
    if ( sem_id == NULL )
    {
        CMD_HWTEST_PRINT_ERROR();
        VK_TASK_Sleep(1000);
        return 1;
    }
	return VK_SEM_Create(sem_id,  name, eSuspendType);
}

int HI_VK_SEM_Get(unsigned long sem_id)
{
	return VK_SEM_Get(sem_id);
}

int HI_VK_SEM_Release(unsigned long sem_id)
{
	return VK_SEM_Release(sem_id);
}

void *HI_VK_MEM_Alloc(unsigned long size)
{
	return DD_MEM_Alloc(size);
}

int HI_VK_MEM_Free(void *ap)
{
    if ( ap == NULL )
    {
        CMD_HWTEST_PRINT_ERROR();
        return 1;
    }
	DD_MEM_Free(ap);
	return 0;
}
unsigned long HI_VK_MEM_Memcpy(void *dest, void *src, unsigned long size)
{
    if ( ( dest == NULL ) || ( src == NULL ) || (size == 0 ))
    {
        CMD_HWTEST_PRINT_ERROR();
        if ( dest == NULL )
        {
            VK_printf("dst is null \n");
        }

        if ( src == NULL )
        {
            VK_printf("src is null\n");
        }

        if ( size == 0 )
        {
            VK_printf("size is 0 \n");
        }
        return 0;
    }
    VK_MEM_Memcpy( (void *)dest, (void *)src, size);
    return size;
}

unsigned long HI_VK_MEM_Memset(void *dest, unsigned char c, unsigned  long count)
{
    if ( ( dest == NULL ) || (count == 0 ))
    {
        CMD_HWTEST_PRINT_ERROR();
        return 0;
    }
    VK_MEM_Memset((void *)dest, c, count);
    return count;
}


int HI_VIDEO_RegisterEventCallback(unsigned long ulDeviceId, HI_VIDEO_NOTIFY_EVT nEvent, CALLBACK_TYPE1 fnVideoCallback, unsigned long* pulCallerId)
{
    if ( ( fnVideoCallback == NULL ) || ( pulCallerId == NULL ))
    {
        CMD_HWTEST_PRINT_ERROR();
        VK_TASK_Sleep(1000);
        return 1;
    }
	return DI_VIDEO_RegisterEventCallback(ulDeviceId, nEvent, fnVideoCallback, (HUINT32 *)pulCallerId);
}

int HI_SCART_RegisterEventNotify(HI_SCART_EVENT eScartEvent, CALLBACK_TYPE1 pfScartEventCallback)
{
    if ( pfScartEventCallback == NULL )
    {
        CMD_HWTEST_PRINT_ERROR();
        VK_TASK_Sleep(1000);
        return 1;
    }
#if defined(CONFIG_SCART)
	return DI_SCART_RegisterEventNotify(eScartEvent, pfScartEventCallback);
#else
	return 0;
#endif

}

int HI_AUDIO_RegisterEventCallback(unsigned long ulDeviceId, HI_AUDIO_NOTIFY_EVT nEvent, CALLBACK_TYPE1 fnAudioCallback, unsigned long* pulCallerId)
{
    if ( (fnAudioCallback == NULL) || (pulCallerId == NULL) )
    {
        CMD_HWTEST_PRINT_ERROR();
        VK_TASK_Sleep(1000);
        return 1;
    }
	return DI_AUDIO_RegisterEventCallback(ulDeviceId, nEvent, fnAudioCallback, (HUINT32 *)pulCallerId);
}

#if !defined(CONFIG_DIRECTFB) || defined(CONFIG_PRODUCT_IMAGE_HWTEST)
int HI_OSD_CreateSurface(HI_OSD_LAYER nOsdLayer, HI_OSD_RECT rect, HI_OSD_PIXEL_FORMAT format, HI_OSD_HANDLE *pOsdHandle)
{
    DI_OSD_RECT rect_di;
    if ( pOsdHandle == NULL )
    {
        CMD_HWTEST_PRINT_ERROR();
        VK_TASK_Sleep(1000);
        return 1;
    }
    rect_di.x = rect.x;
    rect_di.y = rect.y;
    rect_di.width = rect.width;
    rect_di.height = rect.height;
	return DI_OSD_CreateSurface(nOsdLayer, rect_di, format, pOsdHandle);
}

int HI_OSD_GetFrameBuffer(HI_OSD_HANDLE hOsdHandle, void **pFrameBuffer)
{

	return DI_OSD_GetFrameBuffer(hOsdHandle, pFrameBuffer);
}

int HI_OSD_GetOsdHandle(HI_OSD_LAYER nOsdLayer, HI_OSD_HANDLE *pOsdHandle)
{
    extern DRV_Error DRV_OSD_GetOsdHandle(DI_OSD_LAYER nOsdLayer, DI_OSD_HANDLE *pOsdHandle);
    if ( pOsdHandle == NULL )
    {
        CMD_HWTEST_PRINT_ERROR();
        VK_TASK_Sleep(1000);
        return 1;
    }
	return DRV_OSD_GetOsdHandle(nOsdLayer, pOsdHandle);
}
#endif

int HI_CI_RegisterCallback (void (*CiCallbackFunc)(unsigned short usSlotId, unsigned short flag))
{
    if ( CiCallbackFunc == NULL )
    {
        CMD_HWTEST_PRINT_ERROR();
        VK_TASK_Sleep(1000);
        return 1;
    }
#if defined(CONFIG_CI)
	return DI_CI_RegisterCallback(CiCallbackFunc);
#else
	return 0;
#endif
}

void HI_KEY_RegisterKeyEventCallback(pfnHI_KEY_Notify pfnNotify)
{
    if ( pfnNotify == NULL )
    {
        CMD_HWTEST_PRINT_ERROR();
        VK_TASK_Sleep(1000);
        return;
    }
	DI_KEY_RegisterKeyEventCallback((pfnDI_KEY_Notify)pfnNotify);
}

int HI_USB_PWR_Ctrl(DI_USB_PWR_t OnOff)
{
#if	defined(CONFIG_USB)
	DI_USB_PWR_Ctrl(OnOff);
	return DI_USB_HOST_CONTROLLER_PWR_Ctrl(OnOff);
#else
    HWTEST_UNUSED(OnOff);
	return 0;
#endif
}

int HI_USB_HID_RegisterHIDEventCallback (
	const HI_USB_HID_DEVICE eDevices,
	pfnHI_USB_HID_Notify pfnNotify)
{
    if ( pfnNotify == NULL )
    {
        CMD_HWTEST_PRINT_ERROR();
        VK_TASK_Sleep(1000);
        return 1;
    }
#if defined(CONFIG_USB_HID)
	return DI_USB_HID_RegisterHIDEventCallback(eDevices,(pfnDI_USB_HID_Notify)pfnNotify);
#else
	return 0;
#endif
}

void HI_USB_RegisterCallback(pfnHI_USB_NotifyFUNC pfnNotify)
{
    if ( pfnNotify == NULL )
    {
        CMD_HWTEST_PRINT_ERROR();
        VK_TASK_Sleep(1000);
        return;
    }
#if defined(CONFIG_USB)
	DI_USB_RegisterCallback((pfnDI_USB_NotifyFUNC)pfnNotify);
#else
	return;
#endif
}

void HI_HDD_RegisterCallback(pfnHI_HDD_NotifyFUNC pfnNotify)
{
    if ( pfnNotify == NULL )
    {
        CMD_HWTEST_PRINT_ERROR();
        VK_TASK_Sleep(1000);
        return;
    }
#if	defined(CONFIG_RECORD) && defined(CONFIG_HDD_SMART)
	DI_HDD_RegisterCallback((pfnDI_HDD_NotifyFUNC)pfnNotify);
#else
	return;
#endif
}

#if defined(CONFIG_MMC)
int HI_MMC_RegisterCallbackFunc(pfnDI_MMC_CallbackFunc pfnCallback)
{
	return (int)DI_MMC_RegisterCallbackFunc(pfnCallback);
}
int HI_MMC_RegisterSvcCallbackFunc(pfnDI_MMC_SvcCallbackFunc pfnSvcCallback)
{
	return (int)DI_MMC_RegisterSvcCallbackFunc(pfnSvcCallback);
}
int HI_MMC_PowerControl(DI_MMC_PORT_E port, DI_MMC_PWR_E on)
{
	return (int)DI_MMC_PowerControl(port, on);
}

int HI_MMC_Mount(DI_MMC_PORT_E port, HUINT8 *mount_dir)
{
	return (int)DI_MMC_Mount(port, mount_dir);
}
int HI_MMC_Umount(DI_MMC_PORT_E port)
{
	return (int)DI_MMC_Umount(port);
}

int HI_MMC_Format(DI_MMC_PORT_E port)
{
	return (int)DI_MMC_Format(port);
}

int HI_MMC_GetMMCInfo(DI_MMC_PORT_E port, DI_MMC_INFO_T *tMmcInfo)
{
	return (int)DI_MMC_GetMMCInfo(port, tMmcInfo);
}

#endif

void HI_HDD_Dst_RegisterCallback(pfnHI_HDD_Dst_NotifyFUNC pfnNotify)
{
#if	defined(CONFIG_RECORD)
	extern int P_Hdd_RegisterCallback(pfnHI_HDD_Dst_NotifyFUNC NotifyFUNC);
#endif

    if ( pfnNotify == NULL )
    {
        CMD_HWTEST_PRINT_ERROR();
        VK_TASK_Sleep(1000);
        return;
    }
#if	defined(CONFIG_RECORD)
	P_Hdd_RegisterCallback(pfnNotify);
#else
	return;
#endif

}

void HI_HDD_RegisterErrHandleCallback(pfnDI_HDD_ErrNotify pfnNotify)
{
#if	defined(CONFIG_RECORD) && defined(CONFIG_HDD_SMART)
	DI_HDD_RegisterErrHandleCallback(pfnNotify);
#endif

	return;
}

void HI_HDD_RestorePendingSector(const char *devname, unsigned long long sector)
{
#if	defined(CONFIG_RECORD) && defined(CONFIG_HDD_SMART)
	DI_HDD_RestorePendingSector(devname, sector);
#endif

	return;
}

int HI_HDD_PWR_Ctrl(HI_HDD_PWR OnOff)
{
#if	defined(CONFIG_RECORD) && defined(CONFIG_HDD_SMART)
	return DI_HDD_PWR_Ctrl(OnOff);
#else
    HI_HDD_PWR powerOff;
    powerOff = OnOff;
	return 0;
#endif
}


int HI_HDD_PowerControl(int portnum, HI_HDD_PWR OnOff)
{
#if	defined(CONFIG_RECORD) && defined(CONFIG_HDD_SMART)
	return DI_HDD_ControlPower(portnum, OnOff);
#else
    HI_HDD_PWR powerOff;
    powerOff = OnOff;
	return 0;
#endif
}

void HI_CMD_Init(void)
{
	CMD_Init();
	CMD_DI_Init();
	CMD_SetCurrentGroup("DI");
}

void HI_CreateI2C_TestTask(void)
{
    P_CMD_HwTest_CreateI2C_TestTask();
}


#if defined(CONFIG_CHANNEL)
int HI_Ch_GetNumOfInstance(void)
{
    unsigned int num_instance = 0;
    DI_CH_GetNumOfInstance((HUINT32 *)&num_instance);
    return num_instance;
}

int HI_CH_S_GetMaxNumOfRfInput(int nNumOfInstance)
{
#if defined (CONFIG_SUPPORT_SELECTABLE_RF)
	DI_CH_TunerCapacity_t *pstTunerCap;
	int nRet = DI_CH_OK;
	int nIndex;
	unsigned int ulNumOfInput_Temp;
	int nMaxNumOfInput=0;
	pstTunerCap = DD_MEM_Alloc(sizeof(DI_CH_TunerCapacity_t));

	for ( nIndex=0 ; nIndex<nNumOfInstance ; nIndex++ )
	{
		nRet = DI_CH_S_GetTunerCapability(nIndex,pstTunerCap);
		ulNumOfInput_Temp = pstTunerCap->ulNumOfInput;
		if (nMaxNumOfInput < ulNumOfInput_Temp)
		{
			nMaxNumOfInput = ulNumOfInput_Temp;
		}
	}
	return nMaxNumOfInput;
#else
	HWTEST_UNUSED(nNumOfInstance);
    return 0;
#endif
}

#endif /* End of CONFIG_CHANNEL */

void HI_RegisterUIFuncs(pfnHI_ProgressbarUI progFunc, pfnHI_InfoUI infoFunc, pfnHI_MsgUI msgFunc, pfnHI_TimeoutInfoUI timeoutInfoUI)
{
    CMD_RegisterUIFunctions(progFunc, infoFunc, msgFunc, timeoutInfoUI);
    return;
}
int HI_GetSystemInfos(HI_SYSTEM_INFO infoType, void *pInfo, unsigned int infoSize)
{
    if ( (pInfo == NULL) || ( infoSize == 0))
    {
        return 1;
    }
    switch(infoType)
    {
        case HI_SYSTEM_INFO_CHIP_ID:
            DI_SYSTEM_GetSystemInfo(DI_SYS_CHIP_ID, pInfo, infoSize);
            break;
    }
    return 0;
}

/* These APIs will be deprecated */
void HI_CH_S_GetChannelStatus(unsigned int  ulInstanceId, int *pulBer, unsigned int *pulRfAgc, unsigned int *pulIfAgc, unsigned int *pulUncorrect, float *pfPower)
{
    if ( ( pulBer == NULL ) ||
         ( pulRfAgc == NULL ) ||
         ( pulIfAgc == NULL ) ||
         ( pulUncorrect == NULL ) ||
         ( pfPower == NULL ) )
    {
        CMD_HWTEST_PRINT_ERROR();
        VK_TASK_Sleep(1000);
        return;
    }
#if defined(CONFIG_FRONTEND_SAT)
	DI_CH_S_GetChannelStatus(ulInstanceId, pulBer, pulRfAgc, pulIfAgc, pulUncorrect, pfPower);
#else
	HWTEST_UNUSED(ulInstanceId);
	*pulBer=0;
	*pulRfAgc=0;
	*pulIfAgc=0;
	*pulUncorrect=0;
	*pfPower=0;
#endif
}

void HI_CH_C_GetLockInfo( unsigned int ulInstanceId, int *nSnr, int *nber_decimal, int *nber_float, int *nber_exp, int *nUncorrected, int *nSignallevel)
{
    if ( ( nSnr == NULL ) ||
         ( nber_decimal == NULL ) ||
         ( nber_float == NULL ) ||
         ( nber_exp == NULL ) ||
         ( nUncorrected == NULL ) ||
         ( nber_float == NULL ) ||
         ( nSignallevel == NULL ) )
    {
        CMD_HWTEST_PRINT_ERROR();
        VK_TASK_Sleep(1000);
        return;
    }

#if defined(CONFIG_FRONTEND_CAB)
	DI_CH_C_GetLockInfo(ulInstanceId, nSnr, nber_decimal, nber_float, nber_exp, nUncorrected, nSignallevel);
#else
	HWTEST_UNUSED(ulInstanceId);
	*nSnr=0;
	*nber_decimal=0;
	*nber_float=0;
	*nber_exp=0;
	*nUncorrected=0;
	*nSignallevel=0;
#endif
}

static int P_DEVINFO_GetString_RcuName(char *pMsg, int size, unsigned int *ext_data)
{
	extern char RCU_NAME[];

	(void)ext_data;

	if (!pMsg && !size) {
		return -1;
	}

	VK_snprintf(pMsg, size, "%s\n", RCU_NAME);

	return 0;
}

static int P_DEVINFO_GetString_CPUInfo(char *pMsg, int size, unsigned int *ext_data)
{
#if defined(CONFIG_SUPPORT_CPU_TEMP)
	extern unsigned int max_cpu_temp;
	extern HBOOL cpu_log;

	DI_SYS_CPU_INFO_t cpuInfo;
	DI_ERR_CODE diError;
	static int cpuvmin = -1, cpuvmax = -1, cputmin = -30000, cputmax = -30000;

	if (!pMsg) {
		return -1;
	}

	diError = DI_SYSTEM_GetSystemInfo(DI_SYS_CPU_INFO, (void *)&cpuInfo, sizeof(DI_SYS_CPU_INFO_t));
	if (diError != DI_ERR_OK) {
		CMD_Printf("[P_DEVINFO_Get_CPUInfo] DI_SYSTEM_GetSystemInfo(DI_SYS_CPU_INFO) error(0x%x)\n", diError);
		return -1;
	}

	if (cpuvmin < 0) {
		cpuvmin = cpuvmax = (int)cpuInfo.ulVoltage;
	}
	else if (cpuvmin > (int)cpuInfo.ulVoltage) {
		cpuvmin = (int)cpuInfo.ulVoltage;
	}
	else if (cpuvmax < (int)cpuInfo.ulVoltage) {
		cpuvmax = (int)cpuInfo.ulVoltage;
	}

	if (cputmin < -29999) {
		cputmin = cputmax = (int)cpuInfo.ulTemp;
	}
	else if (cputmin > (int)cpuInfo.ulTemp) {
		cputmin = (int)cpuInfo.ulTemp;
	}
	else if (cputmax < (int)cpuInfo.ulTemp) {
		cputmax = (int)cpuInfo.ulTemp;
	}

	if (ext_data != NULL) {
		ext_data[0] = (unsigned int)(cpuInfo.ulTemp/1000);
		ext_data[1] = (unsigned int)max_cpu_temp;
	}

	VK_snprintf(pMsg, size, "%d'C (%d'C~%d'C)\n%d.%03dV (%d.%03dV~%d.%03dV)",
		(int)cpuInfo.ulTemp / 1000, (int)cputmin / 1000, (int)cputmax / 1000,
		cpuInfo.ulVoltage / 1000, cpuInfo.ulVoltage % 1000, cpuvmin / 1000, cpuvmin % 1000, cpuvmax / 1000, cpuvmax % 1000);
	if (cpu_log == TRUE) {
		VK_printf("%s\n",pMsg);
	}
#endif

	return 0;
}

static int P_DEVINFO_GetString_FanInfo(char *pMsg, int size, unsigned int *ext_data)
{
#if defined(CONFIG_FAN)
#if defined(CONFIG_DI_SHARED_LIB)
#if defined(CONFIG_PRODUCT_IMAGE_HWTEST)
	DRV_FAN_SETTING tFanInfo;
	static int hddvmin=-1, hddvmax=-1, fanrpmmin = -1, fanrpmmax = -1;

	if(!pMsg || !size) {
		return -1;
	}

	if (DRV_FAN_GetInfo(&tFanInfo) != DRV_OK) {
		return -1;
	}

	if(fanrpmmin < 0) {
		fanrpmmin=fanrpmmax=tFanInfo.ulFanSpeed_Freq;
	}
	else if(fanrpmmin > tFanInfo.ulFanSpeed_Freq) {
		fanrpmmin = tFanInfo.ulFanSpeed_Freq;
	}
	else if(fanrpmmax < tFanInfo.ulFanSpeed_Freq) {
		fanrpmmax = tFanInfo.ulFanSpeed_Freq;
	}

	if (ext_data != NULL) {
		ext_data[0] = (unsigned int)tFanInfo.ulVoltValue;
		ext_data[1] = (unsigned int)tFanInfo.ulFanSpeed_Freq*60;
	}

	VK_snprintf(pMsg, size, "%dV\n%drpm (%d~%d)\n", tFanInfo.ulVoltValue, tFanInfo.ulFanSpeed_Freq*60, fanrpmmin*60, fanrpmmax*60);
#endif
#else
	DRV_FAN_SETTING tFanInfo;
	static int hddvmin=-1, hddvmax=-1, fanrpmmin = -1, fanrpmmax = -1;

	if(!pMsg || !size) {
		return -1;
	}

	if (DRV_FAN_GetInfo(&tFanInfo) != DRV_OK) {
		return -1;
	}

	if(fanrpmmin < 0) {
		fanrpmmin=fanrpmmax=tFanInfo.ulFanSpeed_Freq;
	}
	else if(fanrpmmin > tFanInfo.ulFanSpeed_Freq) {
		fanrpmmin = tFanInfo.ulFanSpeed_Freq;
	}
	else if(fanrpmmax < tFanInfo.ulFanSpeed_Freq) {
		fanrpmmax = tFanInfo.ulFanSpeed_Freq;
	}

	if (ext_data != NULL) {
		ext_data[0] = (unsigned int)tFanInfo.ulVoltValue;
		ext_data[1] = (unsigned int)tFanInfo.ulFanSpeed_Freq*60;
	}

	VK_snprintf(pMsg, size, "%dV\n%drpm (%d~%d)\n", tFanInfo.ulVoltValue, tFanInfo.ulFanSpeed_Freq*60, fanrpmmin*60, fanrpmmax*60);

#endif
#else
	(void)ext_data;

	VK_snprintf(pMsg, size, "Not supported!\n");
#endif

	return 0;
}

static int P_DEVINFO_GetString_NetInfo(char *pMsg, unsigned int size, unsigned int *ext_data)
{
#if defined(CONFIG_ETHERNET)
	char			hostname[80] = {0,};
	struct hostent	*hptr;
	int				ndx;
	char 			tmsg[256];
	int				fd;

	(void)ext_data;

	if(!pMsg || !size) {
		return -1;
	}

	//clear message buffer
	VK_MEM_Memset(pMsg, 0, size);

	//check eth0 line status
	fd = open("/sys/class/net/eth0/carrier", O_RDONLY);
	if(fd < 0) {
		VK_strcat(pMsg, "Offline\n");
	}
	else {
		read(fd, tmsg, 1);
		(tmsg[0]=='1') ? VK_strcat(pMsg, "Online\n"):VK_strcat(pMsg, "Offline\n");
		close(fd);
	}

	//get ip address
	if(gethostname(hostname, sizeof(hostname)) != 0) {
		return -1;
	}
	if((hptr = gethostbyname(hostname)) == NULL) {
		return -1;
	}
	if (hptr->h_addr_list[0] == NULL) {
		return -1;
	}

	//write ip address to buffer
	for(ndx=0; hptr->h_addr_list[ndx]!=NULL; ndx++) {
		VK_snprintf(tmsg, sizeof(tmsg), "IP (%s)\n", inet_ntoa(*(struct in_addr*)hptr->h_addr_list[ndx]));
		VK_strcat(pMsg, tmsg);
	}
#else
	VK_snprintf(pMsg, size, "Not supported!\n");
#endif

	return 0;
}

static int P_DEVINFO_GetString_HddInfo(char *pMsg, unsigned int size, unsigned int *ext_data)
{
#if defined(CONFIG_HDD_SMART)
	int temp;
	static int hddvmin=-1, hddvmax=-1;

	if(!pMsg || !size) {
		return -1;
	}

	if (DI_SYSTEM_GetSystemInfo(DI_SYS_SMART_TEMP, &temp, sizeof(int)) != DI_ERR_OK) {
		return -1;
	}

	if(hddvmin < 0) {
		hddvmin=hddvmax=temp;
	}
	else if(hddvmin > temp) {
		hddvmin = temp;
	}
	else if(hddvmax < temp) {
		hddvmax = temp;
	}

	if (ext_data != NULL) {
		ext_data[0] = (temp < 0) ? 0 : (unsigned int)temp;
	}

	VK_snprintf(pMsg, size, "%02u'C (%d'C~%d'C)", temp, hddvmin, hddvmax);
#else
	VK_snprintf(pMsg, size, "Not supported!\n");
#endif

	return 0;
}


HINT32 HI_DEVINFO_GetInfo(HI_DEVINFO_TYPE_E type, char *strbuf, int size, unsigned int *ext_data)
{
	int rc;

	if (!strbuf || !size) {
		return -1;
	}

	switch(type) {
	case HI_DEVINFO_TYPE_CPU:
		rc = P_DEVINFO_GetString_CPUInfo(strbuf, size, ext_data);
		break;
	case HI_DEVINFO_TYPE_HDD:
		rc = P_DEVINFO_GetString_HddInfo(strbuf, size, ext_data);
		break;
	case HI_DEVINFO_TYPE_FAN:
		rc = P_DEVINFO_GetString_FanInfo(strbuf, size, ext_data);
		break;
	case HI_DEVINFO_TYPE_NET:
		rc = P_DEVINFO_GetString_NetInfo(strbuf, size, ext_data);
		break;
	case HI_DEVINFO_TYPE_RCU:
		rc = P_DEVINFO_GetString_RcuName(strbuf, size, ext_data);
		break;
	case HI_DEVINFO_TYPE_USB:
	case HI_DEVINFO_TYPE_BER:
	default:
		break;
	}

	return (HINT32)rc;
}
