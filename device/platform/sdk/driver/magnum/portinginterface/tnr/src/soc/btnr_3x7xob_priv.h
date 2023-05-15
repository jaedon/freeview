/***************************************************************************
 *     (c)2005-2011 Broadcom Corporation
 *  
 *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 *  no license (express or implied), right to use, or waiver of any kind with respect to the
 *  Software, and Broadcom expressly reserves all rights in and to the Software and all
 *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.  
 *   
 * 
 * $brcm_Workfile: btnr_3x7xob_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 9/7/12 10:26a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/tnr/src/soc/btnr_3x7xob_priv.h $
 * 
 * Hydra_Software_Devel/1   9/7/12 10:26a farshidf
 * SW7425-3765: Remove symbolic links and flatten out TNR directory
 * structure
 * 
 * Hydra_Software_Devel/1   12/8/11 11:28p farshidf
 * SW7552-170: first version of OOB tuner for B0 verification
 * 
 * 
 ***************************************************************************/

/***************************************************************************
 * Definitions
 ***************************************************************************/
#ifndef _BTNR_3x7xOB_PRIV_H__
#define _BTNR_3x7xOB_PRIV_H__

#if __cplusplus
extern "C" {
#endif

#include "btnr_ob_struct.h"
#include "bmem.h"


#define BREG_WriteField(h, Register, Field, Data) \
  BREG_Write32(h, BCHP_##Register, ((BREG_Read32(h, BCHP_##Register) & \
  ~((uint32_t)BCHP_MASK(Register, Field))) | \
  BCHP_FIELD_DATA(Register, Field, Data)))


#define BREG_ReadField(h, Register, Field) \
  ((BREG_Read32(h, BCHP_##Register) & BCHP_MASK(Register,Field)) >> \
  BCHP_SHIFT(Register,Field))



/***************************************************************************
 * Summary:
 *   Structure for chip-specific TNR handle
 *    
 * Description:
 *   This is the chip-specific component of the BTNR_Handle.
 *     
 * See Also:
 *   None.
 *      
 ***************************************************************************/
typedef struct BTNR_Ob_P_3x7x_Handle
{
  uint32_t magicId;                   /* Used to check if structure is corrupt */
  BREG_Handle hRegister;
  BTNR_Ob_3x7x_TuneParams_t *pTunerParams;
  BTNR_Ob_3x7x_TuneStatus_t *pTunerStatus;
  BTMR_TimerHandle hTimer;
  BMEM_Heap_Handle hHeap;
  BKNI_EventHandle hInterruptEvent;
} BTNR_Ob_P_3x7x_Handle;



typedef struct BTNR_Ob_P_3x7x_Handle *BTNR_Ob_3x7x_Handle;


/***************************************************************************
*	API function interface
 ***************************************************************************/

BERR_Code BTNR_Ob_3x7x_Close(
    BTNR_Handle hDev                    /* [in] Device handle */
    );
BERR_Code BTNR_Ob_3x7x_SetSettings(
		  BTNR_Ob_3x7x_Handle hDev,    /* [in] Device handle */
		  BTNR_Settings *settings     /* [in] TNR settings. */
		);
BERR_Code BTNR_Ob_3x7x_GetSettings(
    BTNR_Ob_3x7x_Handle hDev,    /* [in] Device handle */
    BTNR_Settings *settings     /* [out] TNR settings. */
    );
BERR_Code BTNR_Ob_3x7x_SetRfFreq(
    BTNR_Ob_3x7x_Handle hDev,            /* [in] Device handle */
    uint32_t rfFreq,                    /* [in] Requested tuner freq., in Hertz */
    BTNR_TunerMode tunerMode            /* [in] Requested tuner mode */
    );
BERR_Code BTNR_Ob_3x7x_GetRfFreq(
    BTNR_Ob_3x7x_Handle hDev,            /* [in] Device handle */
    uint32_t *rfFreq,                   /* [output] Returns tuner freq., in Hertz */
    BTNR_TunerMode *tunerMode);

BERR_Code BTNR_Ob_3x7x_GetInfo(
    BTNR_Ob_3x7x_Handle hDev,            /* [in] Device handle */
    BTNR_TunerInfo *tnrInfo             /* [out] Tuner information */
    );

BERR_Code BTNR_Ob_3x7x_PowerUp (BTNR_Ob_3x7x_Handle );
BERR_Code BTNR_Ob_3x7x_PowerDown(BTNR_Ob_3x7x_Handle );
BERR_Code BTNR_Ob_3x7x_SetPowerSaver(
    BTNR_Ob_3x7x_Handle hDev,                    /* [in] Device handle */
    BTNR_PowerSaverSettings *pwrSettings /* [in] Power saver settings. */
    );
BERR_Code BTNR_Ob_3x7x_GetPowerSaver(
    BTNR_Ob_3x7x_Handle hDev,                    /* [in] Device handle */
    BTNR_PowerSaverSettings *pwrSettings        /* [in] Power saver settings. */
    );


#ifdef __cplusplus
}
#endif

#endif /* _BTNR_OB_3x7x_PRIV_H__ */