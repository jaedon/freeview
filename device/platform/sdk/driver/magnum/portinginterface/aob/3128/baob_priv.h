/***************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: baob_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/6 $
 * $brcm_Date: 9/28/12 2:34p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/aob/3128/baob_priv.h $
 * 
 * Hydra_Software_Devel/6   9/28/12 2:34p atanugul
 * SW3128-228: Add support for  BAOB_GetVersionInfo()
 * 
 * Hydra_Software_Devel/5   4/4/12 10:50a atanugul
 * SW3128-131: Add BAOB_GetConfigSettings() and BAOB_SetConfigSettings()
 * API
 * 
 * Hydra_Software_Devel/4   1/18/12 12:07p atanugul
 * SW3128-110: move Nyquist Filter Selection from BAOB_AcquireParam to
 * BAOB_Settings
 * 
 * Hydra_Software_Devel/3   12/22/11 3:38p atanugul
 * SW3128-100: Check for power status before sending HAB commands
 * 
 * Hydra_Software_Devel/2   12/14/11 2:04p atanugul
 * SW3128-2: Add BAOB_GetSoftDecision() support for 3128
 * 
 * Hydra_Software_Devel/1   4/14/11 5:03p atanugul
 * SW3128-2: Add 3128 AOB Support
 * 
 ****************************************************************************/

#ifndef BAOB_PRIV_H__
#define BAOB_PRIV_H__

#include "bchp.h"
#include "bhab.h"
#include "baob.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Defines raw HAB test mesg hdr (struct) */
#define HAB_MSG_HDR(OPCODE,N,CORE_TYPE,CORE_ID) \
    { ((uint8_t)(((uint16_t)(OPCODE)) >> 2)), \
    (((uint8_t)(0x03 & (OPCODE)) << 6) | ((uint8_t)((N)>>4))), \
    ((((uint8_t)(((N)& 0x0F) << 4))) | ((uint8_t)(0x0F & (CORE_TYPE)))), \
    ((uint8_t)(CORE_ID)) }
    
#define BAOB_CORE_TYPE		0x2
#define BAOB_CORE_ID		0x0
#define CORE_TYPE_GLOBAL	0x0

typedef enum BAOB_OpCodesDS{
        BAOB_eAcquire = 0x10,
        BAOB_eAcquireParamsWrite = 0x11,
        BAOB_eAcquireParamsRead = 0x91,
        BAOB_eAnnexASymbolRateWrite = 0x12,
        BAOB_eAnnexASymbolRateRead = 0x92,
        BAOB_eScanParamsWrite = 0x13,
        BAOB_eScanParamsRead = 0x93,
        BAOB_eAcqWordsWrite = 0x14,
        BAOB_eAcqWordsRead = 0x94,
        BAOB_eResetStatus = 0x15,
        BAOB_eRequestAsyncStatus = 0x16,
        BAOB_eGetStatus = 0x96,
        BAOB_eGetConstellation = 0xA3,
        BAOB_eGetVersion = 0xB9,
        BAOB_eGetVersionInfo = 0xBA,
        BAOB_ePowerCtrlOn = 0x19,
        BAOB_ePowerCtrlOff = 0x18,
        BAOB_ePowerCtrlRead = 0x19
}BAOB_OpCodesDS;

/***************************************************************************
Summary:
	The handle for Out-of-Band Downstream module.

Description:

See Also:
	BAOB_Open()

****************************************************************************/

typedef struct BAOB_P_Handle
{
    uint32_t magicId;					/* Used to check if structure is corrupt */
    BCHP_Handle hChip;
    BREG_Handle hRegister;
    BINT_Handle hInterrupt;
    BHAB_DevId devId;
    BHAB_Handle hHab;
    BAOB_CallbackFunc pCallback[BAOB_Callback_eLast];
    void *pCallbackParam[BAOB_Callback_eLast];
    bool enableFEC;						/* enable OOB FEC*/   
    BKNI_MutexHandle mutex;				/* mutex to protect lock status*/
    bool isLock;						/* current lock status */
    unsigned long ifFreq;				/* IF frequency used. */
    bool bPowerdown;
    BAOB_NyquistFilter nyquist;         /* specifies Nyquist filter rolloff */
    BAOB_OutputMode outputMode; 
} BAOB_P_Handle;


#ifdef __cplusplus
}
#endif

#endif

