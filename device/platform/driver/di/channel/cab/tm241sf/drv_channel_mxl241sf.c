/* $Header: */
/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
// File Name:		drv_channel_mxl241sf.c
// Original Author: Jae-Deok, Cho
// File Description: Maxlinear MXL241SF Driver
// Module: Channel
// Remarks:
//	2011/04/28 - Initial Version

/*******************************************************************/
/* Copyright (c) 2006 HUMAX Co., Ltd. 							   */
/* All rights reserved.											   */
/*******************************************************************/


/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
// Start Including Header Files
#include "htype.h"
#include "vkernel.h"

#include "di_channel.h"
#include "di_channel_priv.h"
#include "di_channel_attr.h"
#include "drv_channel.h"
#include "drv_i2c.h"
#include "drv_gpio.h"

#include "drv_channel_mxl241sf.h"
#include "MxL241SF_PhyCtrlApi.h"
#include "MxL241SF_PhyCfg.h"
#include "MxL241SF_OEM_Drv.h"

// End Including Headers


/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
// Start #define
#define MASTER_CHANNEL			0
// End #define


/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
// Start typedef

// End typedef


/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/
// Start global variablee
extern HINT32 show_di_ch;

// End global variable


/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/
// Start global variablee
MxL241SF_SignalQuality_t	g_stMXL241SFSignalInfo[CONFIG_NUM_OF_CAB_CHANNEL];

// End global variable


/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
// Start static variablee
static unsigned char		s_ucIsMxl241sfLock[CONFIG_NUM_OF_CAB_CHANNEL];

// End static variable



#define __CH_C_PRIVATE_FUNCITONS__

/*******************************************************************/
/*********************** Function Description***********************/
/*******************************************************************/
// Function Name: Drv_MXL241SF_I2C_Read
// Function Description: I2C reading operation to MXL241SF
// Parameter: Channel ID / Register Address / Buffer to store the reading value from register
// Return Value: True / False

int Drv_MXL241SF_I2C_Read(int nChannelId, HUINT16 uRegAddr, HUINT16 *puBuffer)
{
	HUINT8			ucDevAddr;
	MXL_STATUS		nRet = MXL_TRUE;

	if (nChannelId == MASTER_MXL241SF)
	{
		ucDevAddr = MASTER_MXL241SF_I2C_ADDR;
	}
	else
	{
		ucDevAddr = SLAVE_MXL241SF_I2C_ADDR;
	}

	nRet = Ctrl_ReadRegister(ucDevAddr, uRegAddr, puBuffer);
	if (nRet != MXL_TRUE)
	{
		CH_DI_Print(0, ("Drv_MXL241SF_I2C_Read[%d] :: I2C Read Fail.. nRet(%d)\n", nChannelId, nRet));
	}

	return nRet;
}


/*******************************************************************/
/*********************** Function Description***********************/
/*******************************************************************/
// Function Name: Drv_MXL241SF_I2C_Write
// Function Description: I2C writing operation to MXL241SF
// Parameter: Channel ID / Register Address / Value to write to the register
// Return Value: True / False

int Drv_MXL241SF_I2C_Write(int nChannelId,  HUINT16 uRegAddr, HUINT16 uBuffer)
{
	HUINT8			ucDevAddr;
	MXL_STATUS		nRet = MXL_TRUE;

	if (nChannelId == MASTER_MXL241SF)
	{
		ucDevAddr = MASTER_MXL241SF_I2C_ADDR;
	}
	else
	{
		ucDevAddr = SLAVE_MXL241SF_I2C_ADDR;
	}

	nRet = Ctrl_WriteRegister(ucDevAddr, uRegAddr, uBuffer);
	if (nRet != MXL_TRUE)
	{
		CH_DI_Print(0, ("Drv_MXL241SF_I2C_Write[%d] :: I2C Write Fail.. nRet(%d)\n", nChannelId, nRet));
	}

	return nRet;
}


unsigned int Convert_MXL_Modulation(CH_CAB_Modulation_e etModulation)
{
	HUINT8		ucMxlModulation;

	switch (etModulation)
	{
		case	DI_CH_CMOD_16QAM	:	ucMxlModulation = QAM16;	break;
		case	DI_CH_CMOD_32QAM	:	ucMxlModulation = QAM32;	break;
		case	DI_CH_CMOD_64QAM	:	ucMxlModulation = QAM64; 	break;
		case	DI_CH_CMOD_128QAM	:	ucMxlModulation = QAM128;	break;
		case	DI_CH_CMOD_256QAM	:	ucMxlModulation = QAM256;	break;
		default	:	ucMxlModulation = QAM64;	break;
	}

	return	ucMxlModulation;
}

static void Get_RF_Power_Level(int nChannelId, float *pfPowerLevel)
{
	HUINT8					ucI2cAddr = MASTER_MXL241SF;

	MXL_STATUS				status;
	MXL_TUNER_RX_PWR_T		MxL241SF_RfPowerReadback;

	ucI2cAddr = (nChannelId == MASTER_MXL241SF) ? MASTER_MXL241SF_I2C_ADDR : SLAVE_MXL241SF_I2C_ADDR;

	MxL241SF_RfPowerReadback.I2cSlaveAddr = ucI2cAddr;
	status = MxLWare_API_GetTunerStatus(MXL_TUNER_RF_RX_PWR_REQ, (void*)&MxL241SF_RfPowerReadback);
	if (status == MXL_TRUE)
	{
		// Convert dBm -> dBuV (+108.75)
		*pfPowerLevel	= (HFLOAT32)((MxL241SF_RfPowerReadback.RxPwr + 108750)/1000.0);
		CH_DI_Print(1,("Get_RF_Power_Level[%d] :: RF Input Power Level = %3.3f [dBuV]\n", nChannelId, *pfPowerLevel));
	}
	else
	{
		*pfPowerLevel	= 0;
		CH_DI_Print(0,("Get_RF_Power_Level[%d] :: MXL_TUNER_RF_RX_PWR_REQ.. Fail..\n",nChannelId));
	}
}


static void Get_SNR(int nChannelId, float *pfSnr)
{
	HUINT8					ucI2cAddr = MASTER_MXL241SF;

	MXL_STATUS				status;
	MXL_DEMOD_SNR_INFO_T	MxL241SF_SnrInfo;

	ucI2cAddr = (nChannelId == MASTER_MXL241SF) ? MASTER_MXL241SF_I2C_ADDR : SLAVE_MXL241SF_I2C_ADDR;

	MxL241SF_SnrInfo.I2cSlaveAddr = ucI2cAddr;

	status = MxLWare_API_GetDemodStatus(MXL_DEMOD_SNR_REQ,(void*)&MxL241SF_SnrInfo);
	if (status == MXL_TRUE)
	{
		*pfSnr	= (HFLOAT32)((MxL241SF_SnrInfo.SNR)/1000.0);
		CH_DI_Print(1,("Get_SNR[%d] :: SNR = %3.3f [dB] \n",nChannelId, *pfSnr));
	}
	else
	{
		*pfSnr	= 0;
		CH_DI_Print(0,("Get_SNR[%d] :: MXL_DEMOD_SNR_REQ.. Fail..\n",nChannelId));
	}

}


static void Get_BER(int nChannelId, unsigned long long *pulBer, unsigned long long *pulUncodedBer, unsigned long long *pulCer)
{
	HUINT8					ucI2cAddr = MASTER_MXL241SF;

	MXL_STATUS				status;
	MXL_DEMOD_BER_INFO_T	MxL241SF_BerInfo;

	ucI2cAddr = (nChannelId == MASTER_MXL241SF) ? MASTER_MXL241SF_I2C_ADDR : SLAVE_MXL241SF_I2C_ADDR;

	MxL241SF_BerInfo.I2cSlaveAddr = ucI2cAddr;

	status = MxLWare_API_GetDemodStatus(MXL_DEMOD_BER_UNCODED_BER_CER_REQ,(void*)&MxL241SF_BerInfo);
	if (status == MXL_TRUE)
	{
		*pulBer			= MxL241SF_BerInfo.BER;
		*pulUncodedBer	= MxL241SF_BerInfo.UncodedBER;
		*pulCer			= MxL241SF_BerInfo.CER;
		CH_DI_Print(1,("Get_BER[%d] :: BER = %llu, Uncoded BER = %llu, CER = %llu \n",nChannelId, *pulBer, *pulUncodedBer, *pulCer));
	}
	else
	{
		*pulBer			= 0;
		*pulUncodedBer	= 0;
		*pulCer			= 0;
		CH_DI_Print(0,("Get_BER[%d] :: MXL_DEMOD_BER_UNCODED_BER_CER_REQ.. Fail..\n",nChannelId));
	}
}


static void Get_Error_Count(int nChannelId, unsigned long long *pulCorrected, unsigned long long *pulUncorrected, unsigned long long *pulUnerrored)
{
	HUINT8					ucI2cAddr = MASTER_MXL241SF;

	MXL_STATUS				status;
	MXL_DEMOD_STAT_COUNT_T	MxL241SF_ErrorCountInfo;

	ucI2cAddr = (nChannelId == MASTER_MXL241SF) ? MASTER_MXL241SF_I2C_ADDR : SLAVE_MXL241SF_I2C_ADDR;

	MxL241SF_ErrorCountInfo.I2cSlaveAddr = ucI2cAddr;

	status = MxLWare_API_GetDemodStatus(MXL_DEMOD_STAT_COUNTERS_REQ,(void*)&MxL241SF_ErrorCountInfo);
	if (status == MXL_TRUE)
	{
		*pulCorrected	= MxL241SF_ErrorCountInfo.AccCwCorrCount;
		*pulUncorrected	= MxL241SF_ErrorCountInfo.AccCwErrCount;
		*pulUnerrored	= MxL241SF_ErrorCountInfo.AccCwUnerrCount;
		CH_DI_Print(1,("Get_Error_Count[%d] :: Corrected = %u, UnCorrected = %u, UnErrored = %u \n",nChannelId, *pulCorrected, *pulUncorrected, *pulUnerrored));
	}
	else
	{
		*pulCorrected	= 0;
		*pulUncorrected	= 0;
		*pulUnerrored	= 0;
		CH_DI_Print(0,("Get_Error_Count[%d] :: MXL_DEMOD_STAT_COUNTERS_REQ.. Fail..\n",nChannelId));
	}
}


static int Check_Frontend_Lock(int nChannelId, unsigned char *pucIsLock)
{

	HUINT8						ucI2cAddr = MASTER_MXL241SF;
	HINT32						nRet = DI_CH_OK;

	MXL_STATUS					status;
	MXL_TUNER_LOCK_STATUS_T 	MxL241SF_TunerLockStatus;
	MXL_DEMOD_LOCK_STATUS_T		MxL241SF_DemodLockStatus;

	ucI2cAddr = (nChannelId == MASTER_MXL241SF) ? MASTER_MXL241SF_I2C_ADDR : SLAVE_MXL241SF_I2C_ADDR;

	MxL241SF_TunerLockStatus.I2cSlaveAddr = ucI2cAddr;
	MxL241SF_DemodLockStatus.I2cSlaveAddr = ucI2cAddr;

	// Check Tuner Status
	status = MxLWare_API_GetTunerStatus(MXL_TUNER_LOCK_STATUS_REQ, (void*)&MxL241SF_TunerLockStatus);
	if (status == MXL_TRUE)
	{
		if (MxL241SF_TunerLockStatus.TunerLockStatus == MXL_LOCKED)
		{
			CH_DI_Print(1,("Check_Frontend_Lock[%d] :: MXL_TUNER_LOCK_STATUS_REQ.. Tuner Lock!! \n", nChannelId));

			// Check Demod Status
			status = MxLWare_API_GetDemodStatus(MXL_DEMOD_FEC_LOCK_REQ, (void*)&MxL241SF_DemodLockStatus);
			if (status == MXL_TRUE)
			{
				if (MxL241SF_DemodLockStatus.Status == MXL_LOCKED)
				{
					CH_DI_Print(1,("Check_Frontend_Lock[%d] :: MXL_DEMOD_FEC_LOCK_REQ.. Demod Lock!! \n", nChannelId));
					*pucIsLock = TRUE;
					nRet = DI_CH_OK;
				}
				else
				{
					CH_DI_Print(1,("Check_Frontend_Lock[%d] :: MXL_DEMOD_FEC_LOCK_REQ.. Demod UnLock!! \n", nChannelId));
					*pucIsLock = FALSE;
					nRet = DI_CH_OK;
				}
			}
			else
			{
				CH_DI_Print(0,("Check_Frontend_Lock[%d] :: MXL_DEMOD_FEC_LOCK_REQ.. Demod Status Reading Fail.. \n", nChannelId));
				*pucIsLock = FALSE;
				nRet = DI_CH_ERR;
			}
		}
		else
		{
			CH_DI_Print(1,("Check_Frontend_Lock[%d] :: MXL_TUNER_LOCK_STATUS_REQ.. Tuner UnLock!! \n", nChannelId));
			*pucIsLock = FALSE;
			nRet = DI_CH_OK;
		}
	}
	else
	{
		CH_DI_Print(0,("Check_Frontend_Lock[%d] :: MXL_TUNER_LOCK_STATUS_REQ.. Tuner Status Reading Fail.. \n", nChannelId));
		*pucIsLock = FALSE;
		nRet = DI_CH_ERR;
	}

	return nRet;
}


static int Set_Initialize_Config(int nChannelId)
{
	HUINT8							ucI2cAddr = MASTER_MXL241SF;
	HINT32							nRet = DI_CH_OK;

	MXL_STATUS						status;
	MXL_RESET_CFG_T					MxL241SF_Reset;
	MXL_DEV_INFO_T					MxL241SF_DeviceInfo;
	MXL_OVERWRITE_DEFAULT_CFG_T		MxL241SF_OverwriteDefault;
	MXL_XTAL_CFG_T					MxL241SF_XtalSetting;
	MXL_TOP_MASTER_CFG_T			MxL241SF_PowerUpTuner;
	MXL_MPEG_OUT_CFG_T				MxL241SF_MpegOut;

	ucI2cAddr = (nChannelId == MASTER_MXL241SF) ? MASTER_MXL241SF_I2C_ADDR : SLAVE_MXL241SF_I2C_ADDR;

	// Do SW Reset
	MxL241SF_Reset.I2cSlaveAddr = ucI2cAddr;
	status = MxLWare_API_ConfigDevice(MXL_DEV_SOFT_RESET_CFG, (void*)&MxL241SF_Reset);
	if (status != MXL_TRUE)
	{
		CH_DI_Print(0,("Set_Initialize_Config[%d] :: MXL_DEV_SOFT_RESET_CFG.. Fail..\n",nChannelId));
		nRet = DI_CH_ERR;
	}

	// Read Back Device id and version
	MxL241SF_DeviceInfo.I2cSlaveAddr = ucI2cAddr;
	status = MxLWare_API_GetDeviceStatus(MXL_DEV_ID_VERSION_REQ, (void*)&MxL241SF_DeviceInfo);
	if (status != MXL_TRUE)
	{
		CH_DI_Print(0,("Set_Initialize_Config[%d] :: MXL_DEV_ID_VERSION_REQ.. Fail..\n",nChannelId));
		nRet = DI_CH_ERR;
	}
	else
	{
		CH_DI_Print(2,("Set_Initialize_Config[%d] :: DevId = 0x%x, Version = 0x%x\n",nChannelId, MxL241SF_DeviceInfo.DevId, MxL241SF_DeviceInfo.DevVer));
	}

	// Overwrite default
	MxL241SF_OverwriteDefault.I2cSlaveAddr = ucI2cAddr;
	status = MxLWare_API_ConfigDevice(MXL_DEV_OVERWRITE_DEFAULT_CFG, (void*)&MxL241SF_OverwriteDefault);
	if (status != MXL_TRUE)
	{
		CH_DI_Print(0,("Set_Initialize_Config[%d] :: MXL_DEV_OVERWRITE_DEFAULT_CFG.. Fail..\n",nChannelId));
		nRet = DI_CH_ERR;
	}

	// XTAL and Clock out setting
	MxL241SF_XtalSetting.I2cSlaveAddr = ucI2cAddr;
	MxL241SF_XtalSetting.XtalEnable = MXL_ENABLE;
	MxL241SF_XtalSetting.DigXtalFreq = XTAL_24MHz;
	MxL241SF_XtalSetting.XtalBiasCurrent = 1;
	MxL241SF_XtalSetting.XtalCap = 25; /*  justin 10pF (default) -> 25pF  */
	MxL241SF_XtalSetting.XtalClkOutEnable = (nChannelId == MASTER_MXL241SF) ? MXL_ENABLE : MXL_DISABLE;
	MxL241SF_XtalSetting.XtalClkOutGain =  0xa;
	MxL241SF_XtalSetting.LoopThruEnable = MXL_ENABLE;

	status = MxLWare_API_ConfigDevice(MXL_DEV_XTAL_SETTINGS_CFG, (void*)&MxL241SF_XtalSetting);
	if (status != MXL_TRUE)
	{
		CH_DI_Print(0,("Set_Initialize_Config[%d] :: MXL_DEV_XTAL_SETTINGS_CFG.. Fail..\n",nChannelId));
		nRet = DI_CH_ERR;
	}

	// Power Up Tuner
	MxL241SF_PowerUpTuner.I2cSlaveAddr = ucI2cAddr;
	MxL241SF_PowerUpTuner.TopMasterEnable = MXL_ENABLE;

	status = MxLWare_API_ConfigTuner(MXL_TUNER_TOP_MASTER_CFG, (void*)&MxL241SF_PowerUpTuner);
	if (status != MXL_TRUE)
	{
		CH_DI_Print(0,("Set_Initialize_Config[%d] :: MXL_TUNER_TOP_MASTER_CFG.. Fail..\n",nChannelId));
		nRet = DI_CH_ERR;
	}

	// Delay 1ms
	VK_TASK_Sleep(1);

	// MPEG out setting
	MxL241SF_MpegOut.I2cSlaveAddr = ucI2cAddr;
	MxL241SF_MpegOut.SerialOrPar = MPEG_DATA_SERIAL;
	MxL241SF_MpegOut.LsbOrMsbFirst = MPEG_SERIAL_MSB_1ST;
	MxL241SF_MpegOut.MpegSyncPulseWidth = MPEG_SYNC_WIDTH_BYTE;
	MxL241SF_MpegOut.MpegValidPol = MPEG_ACTIVE_HIGH;
	MxL241SF_MpegOut.MpegClkPol = MPEG_CLK_POSITIVE;
	MxL241SF_MpegOut.MpegSyncPol = MPEG_ACTIVE_HIGH;
	MxL241SF_MpegOut.MpegClkFreq = MPEG_CLK_57MHz;
	MxL241SF_MpegOut.MpegClkSource = MPEG_CLK_INTERNAL;

	status = MxLWare_API_ConfigDemod(MXL_DEMOD_MPEG_OUT_CFG, (void*)&MxL241SF_MpegOut);
	if (status != MXL_TRUE)
	{
		CH_DI_Print(0,("Set_Initialize_Config[%d] :: MXL_DEMOD_MPEG_OUT_CFG.. Fail..\n",nChannelId));
		nRet = DI_CH_ERR;
	}

	return nRet;
}


static int Set_RF_Tune(int nChannelId, CH_CAB_TuneParam_t *pstCabTuneParam)
{
	HUINT8					ucI2cAddr = MASTER_MXL241SF;
	HINT32					nRet = DI_CH_OK;
	HINT32					nResult = DI_CH_OK;
	HUINT8					ucSymbolType = 0;
	HUINT8					ucAnnexType = 0;
	HUINT8					ucSpectrum = 0;

	MXL_STATUS				status;
	MXL_AGC_T				MxL241SF_Agc;
	MXL_ANNEX_CFG_T			MxL241SF_AnnexType;
	MXL_ADCIQFLIP_CFG_T		MxL241SF_AdcIpFlip;
	MXL_SYMBOL_RATE_T		MxL241SF_SymbolRate;
	MXL_RF_TUNE_CFG_T				MxL241SF_ChanTune;
	MXL_CHAN_DEPENDENT_T			MxL241SF_ChanDependent;

	ucI2cAddr = (nChannelId == MASTER_MXL241SF) ? MASTER_MXL241SF_I2C_ADDR : SLAVE_MXL241SF_I2C_ADDR;

	/* JDCHO - Have to manually set these parameters */
	ucSymbolType = SYM_TYPE_USER_DEFINED_J83A;
	ucAnnexType = ANNEX_A;

	// AGC configuration
	MxL241SF_Agc.I2cSlaveAddr = ucI2cAddr;
	MxL241SF_Agc.FreezeAgcGainWord = MXL_NO_FREEZE;

	status = MxLWare_API_ConfigTuner(MXL_TUNER_AGC_SETTINGS_CFG, (void*)&MxL241SF_Agc);
	if (status != MXL_TRUE)
	{
		CH_DI_Print(0,("Set_RF_Tune[%d] :: MXL_TUNER_AGC_SETTINGS_CFG.. Fail..\n",nChannelId));
		nRet = DI_CH_ERR;
	}

	// Symbol rate
	MxL241SF_SymbolRate.I2cSlaveAddr = ucI2cAddr;
	MxL241SF_SymbolRate.SymbolType = ucSymbolType;
	MxL241SF_SymbolRate.SymbolRate = (HFLOAT32)pstCabTuneParam->ulSymbolRate;	// Unit : MSymbol/s

	status = MxLWare_API_ConfigDemod(MXL_DEMOD_SYMBOL_RATE_CFG, (void*)&MxL241SF_SymbolRate);
	if (status != MXL_TRUE)
	{
		CH_DI_Print(0,("Set_RF_Tune[%d] :: MXL_DEMOD_SYMBOL_RATE_CFG.. Fail..\n",nChannelId));
		return DI_CH_ERR;
	}

	// Config Annex Type
	// AutoDetection Disable
	MxL241SF_AnnexType.I2cSlaveAddr = ucI2cAddr;
	MxL241SF_AnnexType.AnnexType = ucAnnexType;

	MxL241SF_AnnexType.AutoDetectQamType = MXL_DISABLE;
	MxL241SF_AnnexType.AutoDetectMode = MXL_DISABLE;
	MxL241SF_AnnexType.QamType = Convert_MXL_Modulation(pstCabTuneParam->etModulation);
	status = MxLWare_API_ConfigDemod(MXL_DEMOD_ANNEX_QAM_TYPE_CFG, (void*)&MxL241SF_AnnexType);
	if (status != MXL_TRUE)
	{
		CH_DI_Print(0,("Set_RF_Tune[%d] :: MXL_DEMOD_ANNEX_QAM_TYPE_CFG.. Fail..\n",nChannelId));
		nRet = DI_CH_ERR;
	}

	// Tune RF with channel frequency and bandwidth
	MxL241SF_ChanTune.I2cSlaveAddr = ucI2cAddr;
	if (pstCabTuneParam->ulSymbolRate < 6000)
		MxL241SF_ChanTune.BandWidth = 6;											// Uint : MHz
	else
		MxL241SF_ChanTune.BandWidth = 8;
	MxL241SF_ChanTune.Frequency = pstCabTuneParam->ulFrequency * KHZ_TO_HZ;			// Unit : Hz

	status = MxLWare_API_ConfigTuner(MXL_TUNER_CHAN_TUNE_CFG, (void*)&MxL241SF_ChanTune );
	if (status != MXL_TRUE)
	{
		CH_DI_Print(0,("Set_RF_Tune[%d] :: MXL_TUNER_CHAN_TUNE_CFG.. Fail..\n",nChannelId));
		nRet = DI_CH_ERR;
	}

	// Do not forget to put time delay at least 25ms
	VK_TASK_Sleep(25);

	// Channel dependent setting
	MxL241SF_ChanDependent.I2cSlaveAddr = ucI2cAddr;
	MxL241SF_ChanDependent.ChanDependentCfg = MXL_DISABLE;
	status = MxLWare_API_ConfigTuner(MXL_TUNER_CHAN_DEPENDENT_TUNE_CFG, (void*)&MxL241SF_ChanDependent);
	if (status != MXL_TRUE)
	{
		CH_DI_Print(0,("Set_RF_Tune[%d] :: MXL_TUNER_CHAN_DEPENDENT_TUNE_CFG.. Fail..\n",nChannelId));
		nRet = DI_CH_ERR;
	}

	/* JDCHO - Have to manually set this parameter, because there is no automatic spectrum-inversion in MXL241SF */
	// Set MXL_DISABLE for Annex-B and OOB as a default <- Ignore
	if (pstCabTuneParam->etSpectrum != DI_CH_INVERSION_INVERTED)
	{
		ucSpectrum = MXL_ENABLE;
}
	else
{
		ucSpectrum = MXL_DISABLE;
	}

	// Configure spectrum-inversion
	MxL241SF_AdcIpFlip.I2cSlaveAddr = ucI2cAddr;

	// Enable I/Q path flip
	MxL241SF_AdcIpFlip.AdcIqFlip = ucSpectrum;

	status = MxLWare_API_ConfigDemod(MXL_DEMOD_ADC_IQ_FLIP_CFG, (void*)&MxL241SF_AdcIpFlip);
	if (status != MXL_TRUE)
	{
		CH_DI_Print(0,("Set_RF_Tune[%d] :: MXL_DEMOD_ADC_IQ_FLIP_CFG.. Fail..\n",nChannelId));
		nRet = DI_CH_ERR;
	}

	// Do not forget to put time delay at least 300ms
	VK_TASK_Sleep(300);

	// Check Frontend Lock
	nResult = Check_Frontend_Lock(nChannelId, &s_ucIsMxl241sfLock[nChannelId]);
	if (nResult == DI_CH_OK)
	{
		/* JDCHO - If not locked, change spectrum inversion state and  re-check locking status*/
		if (s_ucIsMxl241sfLock[nChannelId] != TRUE)
		{
			MxL241SF_AdcIpFlip.AdcIqFlip = !ucSpectrum;
			status = MxLWare_API_ConfigDemod(MXL_DEMOD_ADC_IQ_FLIP_CFG, (void*)&MxL241SF_AdcIpFlip);
			if (status != MXL_TRUE)
			{
				CH_DI_Print(0,("Set_RF_Tune[%d] :: MXL_DEMOD_ADC_IQ_FLIP_CFG.. Fail..\n",nChannelId));
				nRet = DI_CH_ERR;
			}
			CH_DI_Print(1,("Set_RF_Tune[%d] :: Change Spectrum Inversion Param :: Before(%d):After(%d)\n",nChannelId,ucSpectrum,MxL241SF_AdcIpFlip.AdcIqFlip));
			VK_TASK_Sleep(300);

			Check_Frontend_Lock(nChannelId, &s_ucIsMxl241sfLock[nChannelId]);
		}
		else
		{
			nRet = DI_CH_OK;
		}
	}
	else
	{
		CH_DI_Print(0,("Set_RF_Tune[%d] :: Frontend Lock Checking Fail.. \n", nChannelId));
		nRet = DI_CH_ERR;
	}

	return nRet;

}


static unsigned int Get_Strength_Info(int nChannelId, float *pfPowerLevel)
{
	HUINT32		ulStrength = 0;

	Get_RF_Power_Level(nChannelId, pfPowerLevel);

	if(*pfPowerLevel >= 55)			ulStrength = 100;
	else if(*pfPowerLevel >= 53)	ulStrength = 90;
	else if(*pfPowerLevel >= 51)	ulStrength = 80;
	else if(*pfPowerLevel >= 49)	ulStrength = 70;
	else if(*pfPowerLevel >= 47)	ulStrength = 60;
	else if(*pfPowerLevel >= 45)	ulStrength = 50;
	else if(*pfPowerLevel >= 43)	ulStrength = 40;
	else if(*pfPowerLevel >= 41)	ulStrength = 30;
	else if(*pfPowerLevel >= 39)	ulStrength = 20;
	else 	ulStrength = 10;

	return ulStrength;
}


static unsigned int Get_Quality_Info(int nChannelId, float *pfSnr)
{
	HUINT32		ulQuality = 0;

	Get_SNR(nChannelId, pfSnr);

	if (*pfSnr >= 32)		ulQuality = 100;
	else if (*pfSnr >= 31)	ulQuality = 90;
	else if (*pfSnr >= 30)	ulQuality = 80;
	else if (*pfSnr >= 29)	ulQuality = 70;
	else if (*pfSnr >= 28)	ulQuality = 60;
	else if (*pfSnr >= 27)	ulQuality = 50;
	else if (*pfSnr >= 26)	ulQuality = 40;
	else if (*pfSnr >= 25)	ulQuality = 30;
	else if (*pfSnr >= 24)	ulQuality = 20;
	else if (*pfSnr >= 23)	ulQuality = 10;
	else	ulQuality = 5;

	return ulQuality;
}


static void Update_Signal_Info(int nChannelId, unsigned char ucIsLock)
{
	HFLOAT32	fPowerLevel		= 0;
	HUINT32		ulStrength		= 0;
	HFLOAT32	fSnr			= 0;
	HUINT32		ulQauality		= 0;
	HUINT64		ulBer			= 0;
	HUINT64		ulUncodedBer	= 0;
	HUINT64		ulCer			= 0;
	HUINT64		ulCorrected		= 0;
	HUINT64		ulUncorrected	= 0;
	HUINT64		ulUnerrored		= 0;

	if (ucIsLock == TRUE)
	{
		ulStrength = Get_Strength_Info(nChannelId, &fPowerLevel);
		ulQauality = Get_Quality_Info(nChannelId, &fSnr);
		Get_BER(nChannelId, &ulBer, &ulUncodedBer, &ulCer);
		Get_Error_Count(nChannelId, &ulCorrected, &ulUncorrected, &ulUnerrored);

		g_stMXL241SFSignalInfo[nChannelId].ulStrength		= ulStrength;
		g_stMXL241SFSignalInfo[nChannelId].ulQauality		= ulQauality;
		g_stMXL241SFSignalInfo[nChannelId].fPowerLevel		= fPowerLevel;
		g_stMXL241SFSignalInfo[nChannelId].fSnr				= fSnr;
		g_stMXL241SFSignalInfo[nChannelId].ulBer			= ulBer;
		g_stMXL241SFSignalInfo[nChannelId].ulUncodedBer		= ulUncodedBer;
		g_stMXL241SFSignalInfo[nChannelId].ulCer			= ulCer;
		g_stMXL241SFSignalInfo[nChannelId].ulCorrected		= ulCorrected;
		g_stMXL241SFSignalInfo[nChannelId].ulUncorrected	= ulUncorrected;
		g_stMXL241SFSignalInfo[nChannelId].ulUnerrored		= ulUnerrored;

		CH_DI_Print(2,("=========== Update_Signal_Info[%d] =========== \n",nChannelId));
		CH_DI_Print(2,("  ulStrength       = %u [%%]\n",g_stMXL241SFSignalInfo[nChannelId].ulStrength));
		CH_DI_Print(2,("  ulQauality       = %u [%%]\n",g_stMXL241SFSignalInfo[nChannelId].ulQauality));
		CH_DI_Print(2,("  fPowerLevel      = %3.3f [dBuV] \n",g_stMXL241SFSignalInfo[nChannelId].fPowerLevel));
		CH_DI_Print(2,("  fSnr             = %3.3f [dB] \n",g_stMXL241SFSignalInfo[nChannelId].fSnr));
		CH_DI_Print(2,("  ulBer            = %3.2e \n",g_stMXL241SFSignalInfo[nChannelId].ulBer / 1000000000.0));
		CH_DI_Print(2,("  ulUncodedBer     = %3.2e \n",g_stMXL241SFSignalInfo[nChannelId].ulUncodedBer / 1000000000.0));
		CH_DI_Print(2,("  ulCer            = %3.2e \n",g_stMXL241SFSignalInfo[nChannelId].ulCer / 1000000000.0));
		CH_DI_Print(2,("  ulCorrected      = %llu \n",g_stMXL241SFSignalInfo[nChannelId].ulCorrected));
		CH_DI_Print(2,("  ulUncorrected    = %llu \n",g_stMXL241SFSignalInfo[nChannelId].ulUncorrected));
		CH_DI_Print(2,("  ulUnerrored      = %llu \n",g_stMXL241SFSignalInfo[nChannelId].ulUnerrored));
		CH_DI_Print(2,("============================================ \n"));

	}
	else
	{
		g_stMXL241SFSignalInfo[nChannelId].ulStrength		= 0;
		g_stMXL241SFSignalInfo[nChannelId].ulQauality		= 0;
		g_stMXL241SFSignalInfo[nChannelId].fPowerLevel		= 0;
		g_stMXL241SFSignalInfo[nChannelId].fSnr				= 0;
		g_stMXL241SFSignalInfo[nChannelId].ulBer			= 0;
		g_stMXL241SFSignalInfo[nChannelId].ulUncodedBer		= 0;
		g_stMXL241SFSignalInfo[nChannelId].ulCer			= 0;
		g_stMXL241SFSignalInfo[nChannelId].ulCorrected		= 0;
		g_stMXL241SFSignalInfo[nChannelId].ulUncorrected	= 0;
		g_stMXL241SFSignalInfo[nChannelId].ulUnerrored		= 0;

		CH_DI_Print(0,("Update_Signal_Info[%d] :: Channel Unlocked! \n",nChannelId));
	}
}


#define __CH_C_PUBLIC_FUNCITONS__

void DRV_C_InstallApi( void )
{
	CAB_InitDevice 			= &MXL241SF_InitDevice;
	CAB_SetTune 			= &MXL241SF_SetTune;
	CAB_CheckLock 			= &MXL241SF_CheckLock;
	CAB_GetStrengthQuality 	= &MXL241SF_GetStrengthQuality;
	CAB_SetPowerState 		= &MXL241SF_SetPowerState;
	CAB_EnableTsOutput 		= &MXL241SF_EnableTsOutput;
	CAB_DisableTsOutput 	= &MXL241SF_DisableTsOutput;
	CAB_ResetGpio 			= &MXL241SF_ResetGpio;
	CAB_Initialize 			= &MXL241SF_Initialize;

	CH_DI_Print(0, ("DRV_C_InstallApi()  OK! \n"));

	return;
}

int MXL241SF_Initialize(int nChannelId)
{
	HINT32	nRet = DI_CH_OK;

	CH_UNUSED(nChannelId);

	return nRet;
}
int MXL241SF_InitDevice( int nChannelId )
{
	HINT32	nRet = DI_CH_OK;
	int		nUnitId = 0;
	HUINT32	ulI2cChannel = I2C_CHANNEL_CHANNEL;

	nUnitId = GetDeviceUnitId( nChannelId );

	if( nUnitId == MASTER_CHANNEL )
	{
		ulI2cChannel = I2C_CHANNEL_CHANNEL;
	}
	else
	{
#if defined(CONFIG_I2C_MAP_FOR_2TUNER)
		ulI2cChannel = I2C_CHANNEL_CHANNEL2;
#else
		ulI2cChannel = I2C_CHANNEL_CHANNEL;
#endif
	}

	DRV_I2c_SetClock( ulI2cChannel, DRV_I2C_CLOCK_100K );

	nRet = MXL241SF_ResetGpio(nChannelId);
	if (nRet != DI_CH_OK)
	{
		CH_DI_Print(2, ("MXL241SF_InitDevice[%d] :: Channel GPIO Reset Fail.. nRet(0x%x) \n", nChannelId, nRet));
	}

	nRet = Set_Initialize_Config(nChannelId);
	if (nRet != DI_CH_OK)
	{
		CH_DI_Print(2, ("MXL241SF_InitDevice[%d] :: Device Init Configuration Fail.. nRet(0x%x) \n", nChannelId, nRet));
	}

	return nRet;
}

int MXL241SF_SetTune( int nChannelId, HUINT32 ulWorkingId, CH_CAB_TuneParam_t *pstCabTuneParam )
{
	HUINT8						ucI2cAddr = MASTER_MXL241SF;
	HINT32						nRet = DI_CH_OK;

	CH_UNUSED(ulWorkingId);

	ucI2cAddr = (nChannelId == MASTER_MXL241SF) ? MASTER_MXL241SF_I2C_ADDR : SLAVE_MXL241SF_I2C_ADDR;

	// CancelLock is called from MW before SetTune is Completed
	if (GetTuneStatus(nChannelId) == CH_STATUS_CANCLE)
	{
		CH_DI_Print(0,("Channel[%d] is Canceled by MW before finishing the locking process...\n", nChannelId));
		return DI_CH_CANCEL_TUNE;
	}

	nRet = Set_RF_Tune(nChannelId, pstCabTuneParam);
	if (nRet != DI_CH_OK)
	{
		CH_DI_Print(0,("MXL241SF_SetTune[%d] :: RF Tuning Fail.. \n", nChannelId));
		return DI_CH_ERR;
	}

	if (s_ucIsMxl241sfLock[nChannelId] == TRUE)
		nRet = DI_CH_OK;
	else
		nRet = DI_CH_ERR;

	return nRet;
}

int MXL241SF_GetStatus(int nChannelId, HUINT32 frequency)
{
	CH_UNUSED(nChannelId);
	CH_UNUSED(frequency);

	return DI_CH_OK;
}

HBOOL MXL241SF_CheckLock( int nChannelId )
{
	HINT32		nRet = DI_CH_OK;

	// Check Tuner Lock
	nRet = Check_Frontend_Lock(nChannelId, &s_ucIsMxl241sfLock[nChannelId]);
	if (nRet != DI_CH_OK)
	{
		CH_DI_Print(0,("MXL241SF_SetTune[%d] :: Frontend Lock Checking Fail.. \n", nChannelId));
		nRet = FALSE;
	}

	Update_Signal_Info(nChannelId, s_ucIsMxl241sfLock[nChannelId]);

	return s_ucIsMxl241sfLock[nChannelId];

	return TRUE;
}

int MXL241SF_GetStrengthQuality( int nChannelId, DI_CH_SignalQuality_t *pstSignalInfo)
{
	if (s_ucIsMxl241sfLock[nChannelId]== TRUE)
	{
		pstSignalInfo->ulStrength			= g_stMXL241SFSignalInfo[nChannelId].ulStrength;
		pstSignalInfo->ulQuality			= g_stMXL241SFSignalInfo[nChannelId].ulQauality;
		pstSignalInfo->fSignalInputPower	= g_stMXL241SFSignalInfo[nChannelId].fPowerLevel;
		pstSignalInfo->fSnr					= g_stMXL241SFSignalInfo[nChannelId].fSnr;
		pstSignalInfo->fBer					= (HFLOAT32)g_stMXL241SFSignalInfo[nChannelId].ulUncodedBer;
		pstSignalInfo->ulCorrectedNo		= (HUINT32)g_stMXL241SFSignalInfo[nChannelId].ulCorrected;
		pstSignalInfo->ulUnCorrectedNo		= (HUINT32)g_stMXL241SFSignalInfo[nChannelId].ulUncorrected;
		pstSignalInfo->ulUnErroredNo		= (HUINT32)g_stMXL241SFSignalInfo[nChannelId].ulUnerrored;
		pstSignalInfo->ulRfAgc				= 0;
		pstSignalInfo->ulIfAgc				= 0;
		pstSignalInfo->ulAgc				= 0;

		CH_DI_Print(3,("=========== MXL241SF_GetStrengthQuality[%d] =========== \n",nChannelId));
		CH_DI_Print(3,("  ulStrength           = %u [%%]\n",pstSignalInfo->ulStrength));
		CH_DI_Print(3,("  ulQauality           = %u [%%]\n",pstSignalInfo->ulQuality));
		CH_DI_Print(3,("  fSignalInputPower    = %3.3f [dBuV] \n",pstSignalInfo->fSignalInputPower));
		CH_DI_Print(3,("  fSnr                 = %3.3f [dB] \n",pstSignalInfo->fSnr));
		CH_DI_Print(3,("  ulUncodedBer         = %3.2e \n",pstSignalInfo->fBer / 1000000000.0));
		CH_DI_Print(3,("  ulCorrectedNo        = %u \n",pstSignalInfo->ulCorrectedNo));
		CH_DI_Print(3,("  ulUnCorrectedNo      = %u \n",pstSignalInfo->ulUnCorrectedNo));
		CH_DI_Print(3,("  ulUnErroredNo        = %u \n",pstSignalInfo->ulUnErroredNo));
		CH_DI_Print(3,("============================================ \n"));
	}
	else
	{
		pstSignalInfo->ulStrength			= 0;
		pstSignalInfo->ulQuality			= 0;
		pstSignalInfo->fSignalInputPower	= 0;
		pstSignalInfo->fSnr					= 0;
		pstSignalInfo->fBer					= 0;
		pstSignalInfo->ulCorrectedNo		= 0;
		pstSignalInfo->ulUnCorrectedNo		= 0;
		pstSignalInfo->ulUnErroredNo		= 0;
		pstSignalInfo->ulRfAgc				= 0;
		pstSignalInfo->ulIfAgc				= 0;
		pstSignalInfo->ulAgc				= 0;
	}

	return DI_CH_OK;
}

int MXL241SF_SetPowerState( int nChannelId, CH_Power_e etPower )
{
	CH_UNUSED(nChannelId);
	CH_UNUSED(etPower);

	return DI_CH_OK;
}

int MXL241SF_EnableTsOutput( int nChannelId )
{
	CH_UNUSED(nChannelId);

	return DI_CH_OK;
}

int MXL241SF_DisableTsOutput( int nChannelId )
{
	CH_UNUSED(nChannelId);

	return DI_CH_OK;
}

int MXL241SF_UpdateTunedParam( int nChannelId, CH_CAB_TuneParam_t * pstTuneParam )
{
	CH_UNUSED(nChannelId);
	CH_UNUSED(pstTuneParam);

	return DI_CH_OK;
}

int MXL241SF_ResetGpio(int nChannelId)
{
	HUINT32 	nUnitId = 0;
	HINT32		nRet = DI_CH_OK;

	nUnitId = GetDeviceUnitId( nChannelId );

	CH_DI_Print(1,("MXL241SF_ResetGpio[%d] :: nChannelID[%d] / nUnitId[%d] \n", nChannelId, nChannelId, nUnitId));

	if(nChannelId == MASTER_MXL241SF)
	{
		DRV_GPIO_Write(GPIO_ID_nCH0_RST, GPIO_LOW);
		VK_TASK_Sleep(10);

		DRV_GPIO_Write(GPIO_ID_nCH0_RST, GPIO_HI);
		VK_TASK_Sleep(10);
	}
	else if(nChannelId == SLAVE_MXL241SF)
	{
		DRV_GPIO_Write(GPIO_ID_nCH1_RST, GPIO_LOW);
		VK_TASK_Sleep(10);

		DRV_GPIO_Write(GPIO_ID_nCH1_RST, GPIO_HI);
		VK_TASK_Sleep(10);
	}
	else
	{
		CH_DI_Print(0, ("MXL241SF_ResetGpio[%d] :: nChannelId is invalid \n", nChannelId));
		nRet = DI_CH_ERR;
	}

	return nRet;
}

void MXL241SF_DumpRegister( int nChannelId)
{
	CH_UNUSED(nChannelId);
}

void MXL241SF_GetAGC(int nChannelId, HUINT8 *ucpRFAgc, HUINT8 *ucpIFAgc)
{
	CH_UNUSED(nChannelId);
	CH_UNUSED(ucpRFAgc);
	CH_UNUSED(ucpIFAgc);
}

void MXL241SF_GetEwsInfo(int nChannelId, HUINT8 *ucIsEws)
{
	CH_UNUSED(nChannelId);
	CH_UNUSED(ucIsEws);
}

int MXL241SF_CheckTsmfLock(int nChannelId, HUINT32 frequency)
{
	CH_UNUSED(nChannelId);
	CH_UNUSED(frequency);

	return DI_CH_OK;
}

void MXL241SF_TsmfData(int nChannelId, HUINT16 *tsNo, HINT32 *tsID, HINT32 *netID)
{
	CH_UNUSED(nChannelId);
	CH_UNUSED(tsNo);
	CH_UNUSED(tsID);
	CH_UNUSED(netID);
}

void MXL241SF_QAMStop(int nChannelId)
{
	CH_UNUSED(nChannelId);
}




/*********************** End of File ******************************/

