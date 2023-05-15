/***************************************************************************
 *     Copyright (c) 2003-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvsb_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/9 $
 * $brcm_Date: 12/22/08 3:39p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vsb/3510/bvsb_priv.h $
 * 
 * Hydra_Software_Devel/9   12/22/08 3:39p dliu
 * PR50626: Add status2 information to status structures
 * 
 * Hydra_Software_Devel/8   2/2/04 2:01p enavarro
 * PR 9466: added BVSB_P_WaitForApiEvent(), BVSB_P_DecodeInterrupt(), and
 * BVSB_P_EnableHostInterrupt(); added hApiEvent; changed
 * BVSB_WriteRegister/BVSB_ReadRegister macros to not call
 * BVSB_P_AccessRegister(); removed BVSB_P_AccessRegister()
 * 
 * Hydra_Software_Devel/7   1/28/04 10:06a enavarro
 * PR 9466: removed hChip from BVSB_P_Handle struct (not used)
 * 
 * Hydra_Software_Devel/6   11/4/03 4:47p enavarro
 * added BVSB_P_AccessRegister()
 * 
 * Hydra_Software_Devel/4   9/11/03 4:26p enavarro
 * fixed compiler errors/warnings in linux
 * 
 * Hydra_Software_Devel/3   9/5/03 10:10a enavarro
 * got it to compile
 * 
 * Hydra_Software_Devel/2   8/28/03 4:38p enavarro
 * add #ifdef __cplusplus; fixed bug in BVSB_CHK_RETCODE macro
 * 
 * Hydra_Software_Devel/1   8/28/03 10:44a enavarro
 * initial version
 *
 ***************************************************************************/

#ifndef BVSB_PRIV_H__
#define BVSB_PRIV_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "bvsb.h"

typedef enum BVSB_AccessType
{
   BVSB_AccessType_eWrite,
   BVSB_AccessType_eRead
} BVSB_AccessType;


#define BVSB_CHK_RETCODE(func) \
   { if ((retCode = BERR_TRACE(func)) != BERR_SUCCESS) goto done; }

#if 0
#define BVSB_WriteRegister(hVSB, offset, val) \
   BVSB_P_AccessRegister(hVSB, BVSB_AccessType_eWrite, offset, val)

#define BVSB_ReadRegister(hVSB, offset, val) \
   BVSB_P_AccessRegister(hVSB, BVSB_AccessType_eRead, offset, val)
#else
#define BVSB_WriteRegister(hVSB, offset, val) \
   BREG_I2C_Write(hVSB->hRegister, hVSB->settings.chipAddr, offset, val, 1)
   
#define BVSB_ReadRegister(hVSB, offset, val) \
   BREG_I2C_Read(hVSB->hRegister, hVSB->settings.chipAddr, offset, val, 1)   
#endif

typedef struct BVSB_P_Handle
{
   BREG_I2C_Handle     hRegister;
   BVSB_Settings       settings;
   BKNI_EventHandle    hInterruptEvent;
   BKNI_EventHandle    hApiEvent;
   BKNI_EventHandle    hLockStateChangeEvent;
   BKNI_EventHandle    hHabEvent;
   BKNI_EventHandle    hInitEvent;
   bool                bLocked;
   BVSB_AcqParams      acqParams;
   uint32_t            corrErrCount;
   uint32_t            ucorrBlockCount;
} BVSB_P_Handle;


/* I2C access functions */
#if 0
BERR_Code BVSB_P_AccessRegister(BVSB_Handle hVSB, BVSB_AccessType accessType, uint8_t offset, uint8_t *val);
#endif

/* AP control functions */
BERR_Code BVSB_P_ResetAp(BVSB_Handle hVSB);
BERR_Code BVSB_P_RunAp(BVSB_Handle hVSB);
BERR_Code BVSB_P_IdleAp(BVSB_Handle hVSB);

/* HAB access functions */
BERR_Code BVSB_P_SendHabCommand(BVSB_Handle hVSB, uint8_t write_len, uint8_t *write_buf, uint8_t read_len, uint8_t *read_buf);

/* interrupt functions */
BERR_Code BVSB_P_DisableInitDoneInterrupt(BVSB_Handle hVSB);
BERR_Code BVSB_P_EnableInitDoneInterrupt(BVSB_Handle hVSB);
BERR_Code BVSB_P_DisableHabInterrupt(BVSB_Handle hVSB);
BERR_Code BVSB_P_EnableHabInterrupt(BVSB_Handle hVSB);
BERR_Code BVSB_P_DisableLossLockInterrupt(BVSB_Handle hVSB);
BERR_Code BVSB_P_EnableLossLockInterrupt(BVSB_Handle hVSB);
BERR_Code BVSB_P_DisableLockInterrupt(BVSB_Handle hVSB);
BERR_Code BVSB_P_EnableLockInterrupt(BVSB_Handle hVSB);
BERR_Code BVSB_P_DisableAllInterrupts(BVSB_Handle hVSB);
BERR_Code BVSB_P_WaitForApiEvent(BVSB_Handle hVSB, BKNI_EventHandle hEvent, int timeoutMsec);
BERR_Code BVSB_P_DecodeInterrupt(BVSB_Handle hVSB);
BERR_Code BVSB_P_EnableHostInterrupt(BVSB_Handle hVSB, bool bEnable);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef BVSB_PRIV_H__ */
/* End of File */

