/********************************************************************
* SemcoAPI_TDA18275.h
* TDA18275 Header
* General Standard API
* By skheo 2013/04/05
*********************************************************************/

#ifndef _SEMCOAPI_TDA18275_H
#define _SEMCOAPI_TDA18275_H

#if defined( __cplusplus )
extern "C"                     /* Use "C" external linkage                  */
{
#endif

#include "tmNxTypes.h"
#include "tmFrontEnd.h"
#include "tmbslFrontEndTypes.h"
#include "tmCompId.h"

#include "tmbslTDA18275.h"
#include "tmbslTDA18275_RegDef.h"
#define SEMCO_DBG 1
#ifdef SEMCO_DBG
#define SEMCO_PRINTF VK_printf
#else
#define SEMCO_PRINTF
#endif /* SEMCO_DBG */

/* Initialize TDA18273 */
UINT32 SemcoAPI_TDA18275_Init(int nDeviceId);

/* Deinitialize TDA18273 */
UINT32 SemcoAPI_TDA18275_Close(int nDeviceId);
/* Set standard mode */
UINT32 SemcoAPI_TDA18275_SetStandardMode(int nDeviceId, TDA18275StandardMode_t stdMode);
UINT32 SemcoAPI_TDA18275_GetStandardMode(int nDeviceId, TDA18275StandardMode_t *pStdMode);
/* Set RF frequency */
UINT32 SemcoAPI_TDA18275_SetFrequency(int nDeviceId, UINT32 nFreq_Hz);
/* Get PLL Lock status */
UINT32 SemcoAPI_TDA18275_GetLockStatus(int nDeviceId, tmbslFrontEndState_t * ptrPLLLock);
/* Get IF frequency */
UINT32 SemcoAPI_TDA18275_GetIF(int nDeviceId, UINT32 *ptrlpnIF_Hz);
/* Get Power Level */
UINT32 SemcoAPI_TDA18275_GetPowerLevel(int nDeviceId, UINT8 *pPowerLevel);
/* Set Power State */
UINT32 SemcoAPI_TDA18275_SetPowerState(int nDeviceId, tmPowerState_t powerState);
/* Get Power State */
UINT32 SemcoAPI_TDA18275_GetPowerState(int nDeviceId, tmPowerState_t *pPowerState);
/* Set Fine Frequency */
UINT32 SemcoAPI_TDA18275_SetFineRF(int nDeviceId, char step);
UINT32 SemcoAPI_TDA18275_GetThermo(int nDeviceId, UInt8 * pThermo);
#if 0
/* Wake Up*/
void SemcoTDA18273_On(int nDeviceId);
/* Stand By*/
void SemcoTDA18273_StandBy(int nDeviceId);
/* MFT */
unsigned char SemcoTDA18273_MFT(int nDeviceId,char step);
/* Get Power Level */
int SemcoTDA18273_GetPowerLevel(int nDeviceId, unsigned char *nRSSI);
/* Check RF filter robustness */
int SemcoTDA18273_CheckRFFilterRobustness(int nDeviceId, tmTDA18273RFFilterRating *lpFilterRating);
int SemcoTDA18273_GetTemperature(int nDeviceId, unsigned char *lpThermo);
#endif /* 0 */

#if defined( __cplusplus )
}
#endif


#endif //_SEMCOAPI_TDA18275_H


