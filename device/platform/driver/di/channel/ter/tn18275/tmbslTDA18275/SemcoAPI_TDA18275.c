/********************************************************************
* SemcoAPI_TDA18275.c
* TDA18275 Functions
* General Standard API
* By skheo 2013/04/05
*********************************************************************/
/***********************

History.
2013.04.05 - skheo first version

************************/


// For I2C interface
#ifdef _SEMCO
#include "lib.h"
#include "SEMCO_tx_process.h"
#include "SEMCO_rx_process.h"
#else
#include "UserDefinedIIC.h"
#endif

#include "SemcoAPI_TDA18275.h"
#include "tmbslTDA18275_local.h"
#include "SemcoAPI_CXD2842.h"

/* Initialize TDA18275 */
UINT32 SemcoAPI_TDA18275_Init(int nDeviceId)
{
   /* Variable declarations */
   tmErrorCode_t err = TM_OK;
   tmbslFrontEndDependency_t sSrvTunerFunc;

#if 0
   tmTDA18275StandardMode_t stdMode = tmTDA18275_DVBT_8MHz;
   UInt32 uRF = 770166000;
   tmbslFrontEndState_t PLLLock = tmbslFrontEndStateUnknown;
   UInt32 uIF = 0;
#endif /* 0 */

   /* Driver layer struct set-up to link with user written functions */
   sSrvTunerFunc.sIo.Write             = TDA18273_I2cWrite;
   sSrvTunerFunc.sIo.Read              = TDA18273_I2cRead;
   sSrvTunerFunc.sTime.Get             = Null;
   sSrvTunerFunc.sTime.Wait            = TDA18273_Wait;
   sSrvTunerFunc.sDebug.Print          = Null;
   sSrvTunerFunc.sMutex.Init           = Null;
   sSrvTunerFunc.sMutex.DeInit         = Null;
   sSrvTunerFunc.sMutex.Acquire        = Null;
   sSrvTunerFunc.sMutex.Release        = Null;
   sSrvTunerFunc.dwAdditionalDataSize  = 0;
   sSrvTunerFunc.pAdditionalData       = Null;

   /* TDA18275 Driver initialization  */
   err = tmbslTDA18275_Open(nDeviceId, &sSrvTunerFunc);

   if(err == TM_OK)
   {
    unsigned char Data, readAddress=0x00;
		/* TDA18275 Hardware initialization */
        //SemcoAPI_I2cRepeaterEnable(1);
        TDA18273_I2cRead(nDeviceId, 1, &readAddress, 1, &Data);
        if(Data != 0xff)
    		err = tmbslTDA18275_HwInit(nDeviceId);
        //SemcoAPI_I2cRepeaterEnable(0);
   }

	 return err;
}

/* Deinitialize TDA18273 */
UINT32 SemcoAPI_TDA18275_Close(int nDeviceId)
{
	/* Deinitialize driver */
	tmErrorCode_t err=0;

	err = tmbslTDA18275_Close(nDeviceId);

	return err;
}

/* Set standard mode */
UINT32 SemcoAPI_TDA18275_SetStandardMode(int nDeviceId, TDA18275StandardMode_t stdMode)
{
	tmErrorCode_t err=0;
    //SemcoAPI_I2cRepeaterEnable(1);
	err = tmbslTDA18275_SetStandardMode(nDeviceId, stdMode);
    //SemcoAPI_I2cRepeaterEnable(0);
    return err;
}
/* Get standard mode */
UINT32 SemcoAPI_TDA18275_GetStandardMode(int nDeviceId, TDA18275StandardMode_t *pStdMode)
{
	tmErrorCode_t err=0;
    //SemcoAPI_I2cRepeaterEnable(1);
	err = tmbslTDA18275_GetStandardMode(nDeviceId, pStdMode);
    //SemcoAPI_I2cRepeaterEnable(0);
    return err;
}

/* Set RF frequency */
UINT32 SemcoAPI_TDA18275_SetFrequency(int nDeviceId, UINT32 nFreq_Hz)
{
	tmErrorCode_t err=0;
    //SemcoAPI_I2cRepeaterEnable(1);
	err = tmbslTDA18275_SetRF(nDeviceId, nFreq_Hz);
    //SemcoAPI_I2cRepeaterEnable(0);

	return err;
}

/* Get PLL Lock status */
UINT32 SemcoAPI_TDA18275_GetLockStatus(int nDeviceId, tmbslFrontEndState_t * ptrPLLLock)
{
	tmErrorCode_t err=0;
    //SemcoAPI_I2cRepeaterEnable(1);
	err = tmbslTDA18275_GetLockStatus(0, ptrPLLLock);
    //SemcoAPI_I2cRepeaterEnable(0);

	return err;
}

/* Get IF frequency */
UINT32 SemcoAPI_TDA18275_GetIF(int nDeviceId, UINT32 *ptrlpnIF_Hz)
{
	tmErrorCode_t err=0;
    //SemcoAPI_I2cRepeaterEnable(1);
	err = tmbslTDA18275_GetIF(0, ptrlpnIF_Hz);
    //SemcoAPI_I2cRepeaterEnable(0);

    return err;
}
/* Get Power Level */
/* example */
/*
UInt8 nRSSI;
int RSSI;

SemcoTDA18273_GetPowerLevel(&nRSSI);

RSSI = (nRSSI * 0.5) - 107;

VK_printf("%d",RSSI);
*/
UINT32 SemcoAPI_TDA18275_GetPowerLevel(int nDeviceId, UINT8 *pPowerLevel)
{
	tmErrorCode_t err=0;
    //SemcoAPI_I2cRepeaterEnable(1);
    err = tmbslTDA18275_GetPowerLevel(nDeviceId, pPowerLevel);
    //SemcoAPI_I2cRepeaterEnable(0);
    return err;
}

/* Set Power State */
UINT32 SemcoAPI_TDA18275_SetPowerState(int nDeviceId, tmPowerState_t powerState)
{
	tmErrorCode_t err=0;

    //SemcoAPI_I2cRepeaterEnable(1);
    err = tmbslTDA18275_SetPowerState(nDeviceId, powerState);
    //SemcoAPI_I2cRepeaterEnable(0);

    return err;
}
/* Get Power State */
UINT32 SemcoAPI_TDA18275_GetPowerState(int nDeviceId, tmPowerState_t *pPowerState)
{
	tmErrorCode_t err=0;
    //SemcoAPI_I2cRepeaterEnable(1);
    err = tmbslTDA18275_GetPowerState(nDeviceId, pPowerState);
    //SemcoAPI_I2cRepeaterEnable(0);

    return err;
}
/* Set Fine Frequency */
/* step : step +1 or -1 of 125 KHz */
UINT32 SemcoAPI_TDA18275_SetFineRF(int nDeviceId, char step)
{
	tmErrorCode_t err=0;
    //SemcoAPI_I2cRepeaterEnable(1);
    err = tmbslTDA18275_SetFineRF(nDeviceId, step);
    //SemcoAPI_I2cRepeaterEnable(0);

    return err;
}

UINT32 SemcoAPI_TDA18275_GetThermo(int nDeviceId, UInt8 * pThermo)
{
	tmErrorCode_t err=0;
    err = tmbslTDA18275_GetThermo(nDeviceId, pThermo);
    return err;
}

#if 0
/* Wake Up*/
void SemcoTDA18273_On(int nDeviceId)
{
  tmbslTDA18273SetPowerState(nDeviceId,(tmTDA18273PowerState_t)tmPowerOn);
}

/* Stand By*/
void SemcoTDA18273_StandBy(int nDeviceId)
{
	tmbslTDA18273SetPowerState(nDeviceId,(tmTDA18273PowerState_t)tmPowerStandby);
}

/* MFT */
/* step : +1, -1 */
unsigned char SemcoTDA18273_MFT(int nDeviceId,char step)
{
	int ret=0;

	ret = tmbslTDA18273RFFineTuning(nDeviceId,step);

	return (unsigned char)ret;
}

/* Get Power Level */
/*
 =========== RSSI  °è»ê ¿¹ ==============
UInt8 nRSSI;
int RSSI;

SemcoTDA18273_GetPowerLevel(&nRSSI);

RSSI = (nRSSI * 0.5) - 107;

VK_printf("%d",RSSI);
*/
int SemcoTDA18273_GetPowerLevel(int nDeviceId, unsigned char *nRSSI)
{
  int ret=0;

  ret = tmbslTDA18273GetPowerLevel(nDeviceId, nRSSI);

  return ret;
}

/* Check RF filter robustness */
int SemcoTDA18273_CheckRFFilterRobustness(int nDeviceId, tmTDA18273RFFilterRating *lpFilterRating)
{
	int ret=0;

	ret = tmbslTDA18273CheckRFFilterRobustness(nDeviceId, lpFilterRating);

    return ret;
}



/* Get Temperature */
int SemcoTDA18273_GetTemperature(int nDeviceId, unsigned char *lpThermo)
{
	 int ret;

	 ret = tmddTDA18273GetTM_D(nDeviceId,lpThermo);

	 return ret;
}

#endif /* 0 */


