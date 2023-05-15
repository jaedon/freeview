/***************************************************************************
 *     Copyright (c) 2003-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvsb_3563_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/8 $
 * $brcm_Date: 10/29/07 9:09a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vsb/3563/bvsb_3563_priv.h $
 * 
 * Hydra_Software_Devel/8   10/29/07 9:09a dliu
 * PR35880: Support PAL-D/I/M/N audio output. (feature)
 * (Check in for Jerry Yang)
 * 
 * Hydra_Software_Devel/7   6/7/07 2:35p jerryy
 * PR25778: 3563 bring up for front end
 * 
 * Hydra_Software_Devel/5   4/10/07 3:16p vobadm
 * PR25778: 3563 bringup, imply the new signal detection for 3563
 * 
 * Hydra_Software_Devel/4   1/23/07 8:12p jerryy
 * PR27303: add set threshold interface in VSB PI
 * 
 * Hydra_Software_Devel/3   11/28/06 3:30p dliu
 * PR23867: Added back audio config definition
 * 
 * Hydra_Software_Devel/2   8/24/06 4:13p dliu
 * PR23867: Initial check-in
 * 
 ***************************************************************************/

#ifndef _BVSB_3563_PRIV_H__
#define _BVSB_3563_PRIV_H__

#include "bvsb_3563.h"


#define BVSB_CHK_RETCODE(x) \
   { if ((retCode = (x)) != BERR_SUCCESS) goto done; }

/* bit definitions used in audio HAB commands */
#define BVSB_AUDIO_CONFIG_DECODE_MODE_MASK     0x03
#define BVSB_AUDIO_CONFIG_DECODE_MODE_MONO     0x00
#define BVSB_AUDIO_CONFIG_DECODE_MODE_STEREO   0x01
#define BVSB_AUDIO_CONFIG_DECODE_MODE_SAP      0x02
#define BVSB_AUDIO_CONFIG_DECODE_MODE_SAP_MONO 0x03
#define BVSB_AUDIO_CONFIG_SR_MASK              0x0C
#define BVSB_AUDIO_CONFIG_SR_32KHZ             0x00
#define BVSB_AUDIO_CONFIG_SR_44_1KHZ           0x04
#define BVSB_AUDIO_CONFIG_SR_48KHZ             0x08
#define BVSB_AUDIO_CONFIG_DAC_SELECT_MASK      0x20
#define BVSB_AUDIO_CONFIG_DAC_SELECT_JDA       0x00
#define BVSB_AUDIO_CONFIG_DAC_SELECT_HIFIDAC   0x20
#define BVSB_AUDIO_CONFIG_SEL_MASK             0x40
#define BVSB_AUDIO_CONFIG_SEL_AUDIO_DAC        0x00
#define BVSB_AUDIO_CONFIG_SEL_I2S              0x40
#define BVSB_AUDIO_CONFIG_MUTE                 0x80
#define BVSB_AUDIO_SAP_DETECTED                0x01
#define BVSB_AUDIO_STEREO_DETECTED             0x02

/*used to define unchanged thresholds*/
#define BVSB_P_UNCHANGED_THRESHOLD   0x0

/***************************************************************************
Summary:
	Structure for chip-specific VSB handle

Description:
	This is the chip-specific component of the BVSB_Handle.

See Also:
	None.

****************************************************************************/
typedef struct BVSB_3563_P_Handle
{
   BREG_Handle      hRegister;         /* handle used to access AP registers */
   BKNI_EventHandle hLockChangeEvent;  /* change of lock status event handle */
   BKNI_EventHandle hInitDoneEvent;    /* AP initialization done event handle */
   BKNI_EventHandle hHabDoneEvent;     /* HAB done event handle */
   BKNI_EventHandle hSignalLevelChangeEvent;   /* signal level changed  detected */
   BKNI_EventHandle hVideoKillEvent;   /* signal drop below where we need to kill video */
   BKNI_EventHandle hUnlockEvent;
   BKNI_EventHandle hLockEvent;
} BVSB_3563_P_Handle;


/******************************************************************************
Summary:
   Reads from the host access buffer.
Description:
   This function positions the 128 byte window in the AP address space to the
   host access buffer area.  (n) bytes are then read into (buf) starting from 
   the given HAB (offset) address.
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BVSB_3563_P_ReadHab(
   BVSB_Handle h,   /* [in] BVSB PI Handle */
   uint8_t offset,  /* [in] starting offset within the HAB to read */
   uint8_t *buf,    /* [out] holds the data read */
   uint8_t n        /* [in] number of bytes to read */ 
);


/******************************************************************************
Summary:
   Writes to the host access buffer.
Description:
   This function positions the 128 byte window in the AP address space to the
   host access buffer area.  (n) bytes in (buf) are then written starting from 
   the given HAB (offset) address.
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BVSB_3563_P_WriteHab(
   BVSB_Handle h,   /* [in] BVSB PI Handle */
   uint8_t offset,  /* [in] starting offset in the HAB to write */
   uint8_t *buf,    /* [in] specifies the data to write */
   uint8_t n        /* [in] number of bytes to write */
);


/******************************************************************************
Summary:
   Reads from the AP RAM/ROM space.
Description:
   This function is normally used for verifying the contents of the AP RAM 
   after microcode download.  This function assumes that the AP is already in 
   idle or reset state.
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BVSB_3563_P_ReadMemory(
   BVSB_Handle h,  /* [in] BVSB PI Handle */
   uint16_t addr,  /* [in] starting address */
   uint8_t *buf,   /* [out] holds the data read */
   uint16_t n      /* [in] number of bytes to read */
);


/******************************************************************************
Summary:
   Writes to the AP RAM space.
Description:
   This function is normally used for downloading the microcode.  This function
   assumes that the AP is already in idle or reset state.
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BVSB_3563_P_WriteMemory(
   BVSB_Handle h, /* [in] BVSB PI Handle */ 
   uint16_t addr, /* [in] starting address in AP RAM */
   uint8_t *buf,  /* [in] specifies the data to write */
   uint16_t n     /* [in] number of bytes to write */
);


/******************************************************************************
Summary:
   Resets the AP.
Description:
   This function puts the AP in reset state.
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BVSB_3563_P_ResetAp(
   BVSB_Handle h   /* [in] BVSB PI Handle */
);


/******************************************************************************
Summary:
   Runs the AP.
Description:
   This function takes the AP to out of reset/idle state and starts running.
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BVSB_3563_P_RunAp(
   BVSB_Handle h   /* [in] BVSB PI Handle */
);


/******************************************************************************
Summary:
   Disables all host interrupts.
Description:
   This function clears the host interrupt enable bits on the BCM3563.
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BVSB_3563_P_DisableInterrupts(
   BVSB_Handle h   /* [in] BVSB PI Handle */
);


/******************************************************************************
Summary:
   Returns the BERR_Code corresponding to the given BVSB_ApStatus.
Description:
   This function returns ths BERR_Code of any errors in the given BVSB_ApStatus.
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BVSB_3563_P_DecodeError(
   BVSB_Handle h,           /* [in] BVSB PI Handle */
   BVSB_ApStatus *pApStatus /* [in] AP status returned by BVSB_GetApStatus */
);


/******************************************************************************
Summary:
   This function sends an HAB command.
Description:
   This function writes a command to the HAB, asserts HABR, waits for the AP to 
   release the HAB, then optionally checks for a service acknowlegement from 
   the AP.  If bCheckForAck is true, then read_len must be greater than 0.
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BVSB_3563_P_SendHabCommand(
   BVSB_Handle h,      /* [in] BVSB PI Handle */
   uint8_t *write_buf, /* [in] specifies the HAB command to send */
   uint8_t write_len,  /* [in] number of bytes in the HAB command */ 
   uint8_t *read_buf,  /* [out] holds the data read from the HAB */ 
   uint8_t read_len,   /* [in] number of bytes to read from the HAB */
   bool bCheckForAck   /* [in] true = determine if the AP has serviced the command */
);


/******************************************************************************
Summary:
   This function sends the command already written in the HAB.
Description:
   This function asserts HABR, waits for the AP to release the HAB, then 
   optionally checks for a service acknowlegement from the AP.  If bCheckForAck
   is true, then read_len must be greater than 0.  This function is called by
   BVSB_3563_P_SendHabCommand().
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BVSB_3563_P_ServiceHab(
   BVSB_Handle h,   /* [in] BVSB PI Handle */
   uint8_t *read_buf,  /* [out] holds the data read from the HAB */ 
   uint8_t read_len,   /* [in] number of bytes to read from the HAB */
   bool bCheckForAck,  /* [in] true = determine if the AP has serviced the command */
   uint8_t ack_byte    /* [in] value of the ack byte to expect */
);


/******************************************************************************
Summary:
   This function checks if the host is currently permitted to send an HAB 
   command.
Description:
   The HAB is available to the host if all 3 conditions currently apply: 
   1) The AP is running.
   2) The AP has initialized.
   3) The AP is not currently servicing the HAB (i.e. HABR bit is 0)
Returns:
   BERR_Code - BERR_SUCCESS if HAB is available, otherwise BERR_Code is reason
   why HAB is not available
******************************************************************************/
BERR_Code BVSB_3563_P_CheckHab(
   BVSB_Handle h  /* [in] BVSB PI Handle */
);

/******************************************************************************
Summary:
   Enables/Disables the inband channel lock interrupt.
Description:
   This function enables or disables an interrupt to be generated upon change 
   in inband channel lock status.
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BVSB_3563_P_EnableLockInterrupt(

   BVSB_Handle h, /* [in] BVSB PI Handle */
   bool bEnable   /* [in] true = enable lock interrupts, false = disables lock interrupts */
);


/******************************************************************************
Summary:
   Enables/Disables the BSC interrupt.
Description:
   This function enables or disables the BSC transaction done interrupt.
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BVSB_3563_P_EnableBscInterrupt(
   BVSB_Handle h, /* [in] BVSB PI Handle */
   bool bEnable      /* [in] true = enable the BSC interrupt */
);


/******************************************************************************
Summary:
   Initiates VSB acquisition on the BCM3563.
Description:
   This function sends the Acquire_VSB HAB command to the AP.
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BVSB_3563_P_AcquireVsb(
   BVSB_Handle h,                   /* [in] BVSB PI Handle */
   const BVSB_InbandParams *pParams /* [in] inband acquisition parameters */
);


/******************************************************************************
Summary:
   Initiates QAM acquisition on the BCM3563.
Description:
   This function sends the Acquire_QAM HAB command to the AP.
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BVSB_3563_P_AcquireQam(
   BVSB_Handle h,                   /* [in] BVSB PI Handle */
   const BVSB_InbandParams *pParams /* [in] inband acquisition parameters */
);


/******************************************************************************
Summary:
   Initiates NTSC acquisition on the BCM3563.
Description:
   This function sends the Acquire_NTSC HAB command to the AP.
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BVSB_3563_P_AcquireNtsc(
   BVSB_Handle h,                   /* [in] BVSB PI Handle */
   const BVSB_InbandParams *pParams /* [in] inband acquisition parameters */
);


/* BCM3563 implementation of API functions */
BERR_Code BVSB_3563_P_Open(BVSB_Handle *, BCHP_Handle, void*, BINT_Handle, const struct BVSB_Settings*);
BERR_Code BVSB_3563_P_Close(BVSB_Handle);
BERR_Code BVSB_3563_P_InitAp(BVSB_Handle, const uint8_t *);
BERR_Code BVSB_3563_P_GetApStatus(BVSB_Handle, BVSB_ApStatus*);
BERR_Code BVSB_3563_P_GetApVersion(BVSB_Handle, uint16_t*, uint16_t*, uint8_t*, uint8_t*, uint8_t*);
BERR_Code BVSB_3563_P_ReadRegister(BVSB_Handle, uint32_t, uint32_t*);
BERR_Code BVSB_3563_P_WriteRegister(BVSB_Handle, uint32_t, uint32_t*);
BERR_Code BVSB_3563_P_AcquireInband(BVSB_Handle, const BVSB_InbandParams *);
BERR_Code BVSB_3563_P_GetVsbStatus(BVSB_Handle, BVSB_VsbStatus *);
BERR_Code BVSB_3563_P_GetQamStatus(BVSB_Handle, BVSB_QamStatus *);
BERR_Code BVSB_3563_P_GetNtscStatus(BVSB_Handle, BVSB_NtscStatus *);
BERR_Code BVSB_3563_P_ResetInbandStatus(BVSB_Handle);
BERR_Code BVSB_3563_P_AcquireOob(BVSB_Handle, const BVSB_OobParams *);
BERR_Code BVSB_3563_P_GetOobStatus(BVSB_Handle, BVSB_OobStatus *);
BERR_Code BVSB_3563_P_ResetOobStatus(BVSB_Handle);
BERR_Code BVSB_3563_P_GetBtscStatus(BVSB_Handle, BVSB_BtscStatus *);
BERR_Code BVSB_3563_P_SetInbandOi(BVSB_Handle, BVSB_InbandOiSettings *);
BERR_Code BVSB_3563_P_GetSoftDecisionBuf(BVSB_Handle, int16_t*, int16_t*);
BERR_Code BVSB_3563_P_GetOobSoftDecisionBuf(BVSB_Handle, int16_t*, int16_t*);
BERR_Code BVSB_3563_P_WriteConfig(BVSB_Handle, uint16_t, uint8_t*, uint8_t);
BERR_Code BVSB_3563_P_ReadConfig(BVSB_Handle, uint16_t, uint8_t*, uint8_t);
BERR_Code BVSB_3563_P_GetLockStateChangeEventHandle(BVSB_Handle, BKNI_EventHandle*);
BERR_Code BVSB_3563_P_HandleInterrupt_isr(BVSB_Handle);
BERR_Code BVSB_3563_P_DetectChannelSignal(BVSB_Handle, BVSB_DetectType, BVSB_SignalMode* );
BERR_Code BVSB_3563_P_GetUnlockstatusEventHandle(BVSB_Handle, BKNI_EventHandle* );
BERR_Code BVSB_3563_P_GetLockstatusEventHandle(BVSB_Handle, BKNI_EventHandle* );
BERR_Code BVSB_3563_P_SetInbandIfFreq(BVSB_Handle, uint32_t);
BERR_Code BVSB_3563_P_SetQamSymbol(BVSB_Handle, uint32_t);
BERR_Code BVSB_3563_P_SetIfdPullInRange(BVSB_Handle, BVSB_Ifd_PullInRange);
BERR_Code BVSB_3563_P_SetThreshold(BVSB_Handle);
BERR_Code BVSB_3563_P_SetQamSettings(BVSB_Handle, bool);
BERR_Code BVSB_3563_P_SetIfdPixCarrier(BVSB_Handle, uint32_t);
BERR_Code BVSB_3563_P_SetTmConfig(BVSB_Handle, void*);
BERR_Code BVSB_3563_P_GetTmConfig(BVSB_Handle, void*);
#ifdef __cplusplus
}
#endif

#endif /* BVSB_3563_P_PRIV_H__ */

