/***************************************************************************
 *     Copyright (c) 2007-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bast_4506_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/30 $
 * $brcm_Date: 4/18/12 10:16a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ast/4506/bast_4506_priv.h $
 * 
 * Hydra_Software_Devel/30   4/18/12 10:16a enavarro
 * SWSATFE-189: added bInit to AST handle struct
 * 
 * Hydra_Software_Devel/29   10/5/11 9:51a enavarro
 * SWSATFE-150: spur canceller not enabled in 4506 AST
 * 
 * Hydra_Software_Devel/28   9/9/11 2:34p enavarro
 * SWSATFE-146: move vco avoidance algorithm to upper software layer
 * 
 * Hydra_Software_Devel/27   1/27/11 3:46p enavarro
 * SW7346-21: added BAST_SetPeakScanSymbolRateRange(); added
 * BAST_GetPeakScanSymbolRateRange()
 * 
 * Hydra_Software_Devel/26   1/20/11 5:16p ronchan
 * SWSATFE-99: added chip-independent fsk channel config api
 * 
 * Hydra_Software_Devel/25   12/7/10 2:30p enavarro
 * SWSATFE-94: store crystal freq in AST handle
 * 
 * Hydra_Software_Devel/24   11/29/10 4:13p enavarro
 * SWSATFE-80: implemented BAST_SetNetworkSpec() and BAST_GetNetworkSpec()
 * API functions
 * 
 * Hydra_Software_Devel/23   9/30/10 5:55p ronchan
 * SWSATFE-80: added API functions to set/get diseqc settings
 * 
 * Hydra_Software_Devel/22   9/29/10 4:09p enavarro
 * SWSATFE-80: added implementation functions for set/get output transport
 * settings
 * 
 * Hydra_Software_Devel/21   9/29/10 10:22a enavarro
 * SWSATFE-80: added implementation function for
 * BAST_GetSignalDetectStatus
 * 
 * Hydra_Software_Devel/20   9/17/10 11:34a enavarro
 * SWSATFE-80: added BAST_SetSearchRange(), BAST_GetSearchRange(),
 * BAST_SetTunerFilter(), and BAST_SetAmcScramblingSeq()
 * 
 * Hydra_Software_Devel/19   10/22/09 3:03p ronchan
 * SWSATFE-7: add function definitions for idata and xdata functions
 * 
 * Hydra_Software_Devel/18   7/6/09 10:20a enavarro
 * PR 56603: added BAST_4506_P_ConfigAgc()
 * 
 * Hydra_Software_Devel/17   5/12/09 3:17p ronchan
 * PR 54946: changed voltage type for BAST_GetDiseqcVoltage()
 * 
 * Hydra_Software_Devel/16   5/5/09 9:39a enavarro
 * PR 53392: added numTuners to BAST_4506_P_Handle; added tuner_ctl to
 * BAST_4506_P_ChannelHandle
 * 
 * Hydra_Software_Devel/15   9/23/08 2:06p ronchan
 * PR 32395: added chip id to device handle
 * 
 * Hydra_Software_Devel/14   9/17/08 5:07p enavarro
 * PR 32395: added BAST_4506_P_EnableSpurCanceller()
 * 
 * Hydra_Software_Devel/13   9/15/08 9:20a enavarro
 * PR 32395: add API functions to better manage external LNA
 * 
 * Hydra_Software_Devel/12   7/11/08 5:05p ronchan
 * PR 32395: fixed compiler warnings for nexus
 * 
 * Hydra_Software_Devel/11   5/30/08 10:10a enavarro
 * PR 32395: added BAST_GetStatusEventHandle() to API function table
 * 
 * Hydra_Software_Devel/10   5/29/08 4:42p enavarro
 * PR 32395: added BAST_PeakScan(), BAST_GetPeakScanStatus(),
 * BAST_GetPeakScanEventHandle(), and BAST_EnableStatusInterrupts();
 * added hPeakScanEvent to BAST_4506_P_ChannelHandle struct
 * 
 * Hydra_Software_Devel/9   4/28/08 3:30p ronchan
 * PR 32395: added vsense functions
 * 
 * Hydra_Software_Devel/8   2/1/08 9:11a enavarro
 * PR 32395: added BAST_4506_P_DeepSleep()
 * 
 * Hydra_Software_Devel/7   11/29/07 9:16a enavarro
 * PR 32395: changed BAST_GetLnaStatus() to return pointer to
 * BAST_LnaStatus struct
 * 
 * Hydra_Software_Devel/5   8/6/07 11:54a enavarro
 * PR 32395: added functions to access indirect registers
 * 
 * Hydra_Software_Devel/4   7/30/07 3:06p enavarro
 * PR 33579: Added BAST_GetLnaStatus()
 * 
 * Hydra_Software_Devel/3   7/9/07 4:48p enavarro
 * PR 32848: pass in uint32_t to BAST_PowerDown() and BAST_PowerUp()
 * 
 * Hydra_Software_Devel/2   6/22/07 10:52a enavarro
 * PR 32395: fixed compile errors
 * 
 * Hydra_Software_Devel/1   6/21/07 3:40p enavarro
 * PR 32395: initial version
 *
 ***************************************************************************/

#ifndef _BAST_4506_PRIV_H__
#define _BAST_4506_PRIV_H__

#include "bast_4506.h"


#define BAST_CHK_RETCODE(x) \
   { if ((retCode = (x)) != BERR_SUCCESS) goto done; }

#define BAST_WriteHostRegister(h, offset, val) \
   BREG_I2C_Write(((BAST_4506_P_Handle *)(h->pImpl))->hRegister, h->settings.i2c.chipAddr, offset, val, 1)

#define BAST_ReadHostRegister(h, offset, val) \
   BREG_I2C_Read(((BAST_4506_P_Handle *)(h->pImpl))->hRegister, h->settings.i2c.chipAddr, offset, val, 1)

#define BAST_4506_MAX_CHANNELS 2

/* window addresses passed to BAST_P_SetApWindow() */
#define BAST_WINDOW_IRAM      0x00000000
#define BAST_WINDOW_HAB       0x00010000


/***************************************************************************
Summary:
   Structure for chip-specific portion of the BAST handle
Description:
   This is the chip-specific component of the BAST_Handle.
See Also:
   None.
****************************************************************************/
typedef struct BAST_4506_P_Handle
{
   BREG_I2C_Handle  hRegister;         /* handle to the master I2C device used to access registers */
   BKNI_EventHandle hInterruptEvent;   /* interrupt event handle */   
   BKNI_EventHandle hApiEvent;         /* API event handle */   
   BKNI_EventHandle hInitDoneEvent;    /* AP initialization done event handle */
   BKNI_EventHandle hHabDoneEvent;     /* HAB done event handle */
   BKNI_EventHandle hFtmEvent;         /* FSK event handle */
   bool             bInit;             /* true if AST PI was initialized */
   uint32_t         xtal;              /* crystal frequency */
   uint16_t         chipId;            /* chip id */
   uint8_t          last_page_16_15;   /* most recent position of the 128 byte window into the AP address space */
   uint8_t          last_page_14_7;    /* most recent position of the 128 byte window into the AP address space */
   uint8_t          last_mbox_15_8;    /* most recent position of the IO MBOX window */
   uint8_t          last_bsc_slave_addr; /* last BSC slave addr */
   uint8_t          numTuners;         /* number of internal tuners on the chip */
} BAST_4506_P_Handle;


/***************************************************************************
Summary:
   Structure for chip-specific portion of the BAST channel handle
Description:
   This is the chip-specific component of the BAST_ChannelHandle.
See Also:
   None.
****************************************************************************/
typedef struct BAST_4506_P_ChannelHandle
{
   BKNI_EventHandle hLockChangeEvent;  /* change of lock status event handle */
   BKNI_EventHandle hDiseqcEvent;      /* diseqc event handle */
   BKNI_EventHandle hTunerEvent;       /* tuner event handle */
   BKNI_EventHandle hStatusEvent;      /* status threshold event handle */
   BKNI_EventHandle hPeakScanEvent;    /* peak scan done event handle */
} BAST_4506_P_ChannelHandle;


/******************************************************************************
Summary:
   Reads a register via the IO_MBOX interface.
Description:
   This function uses the IO_MBOX interface to access internal core registers. 
   This function assumes that the accessed core has already been unlocked in 
   the bus bridge.
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BAST_4506_P_ReadMbox(
   BAST_Handle h,    /* [in] BAST device handle */
   uint16_t    reg,  /* [in] RBUS register address */
   uint32_t    *val  /* [out] value read from register */
);


/******************************************************************************
Summary:
   Writes to a register via the IO_MBOX interface.
Description:
   This function uses the IO_MBOX interface to access internal core registers.
   This function assumes that the accessed core has already been unlocked in 
   the bus bridge.
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BAST_4506_P_WriteMbox(
   BAST_Handle h,    /* [in] BAST device handle */
   uint16_t    reg,  /* [in] RBUS register address */
   uint32_t    *val  /* [in] value to write */
);


/******************************************************************************
Summary:
   Read/Modify/Write the value of an ISB register.
Description:
   This function sends an HAB command to read a register, logically AND its 
   value to and_mask, then logically OR in or_mask, and writes the result back
   into the ISB register.
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BAST_4506_P_HabIsbEdit(
   BAST_ChannelHandle h, /* [in] BAST channel handle */
   uint16_t isb_addr,    /* [in] ISB address */
   uint32_t and_mask,    /* [in] AND mask */
   uint32_t or_mask      /* [in] OR mask */
);


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
BERR_Code BAST_4506_P_ReadHab(
   BAST_Handle h,   /* [in] BAST handle */
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
BERR_Code BAST_4506_P_WriteHab(
   BAST_Handle h,   /* [in] BAST handle */
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
BERR_Code BAST_4506_P_ReadMemory(
   BAST_Handle h,  /* [in] BAST handle */
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
BERR_Code BAST_4506_P_WriteMemory(
   BAST_Handle h,       /* [in] BAST handle */ 
   uint16_t addr,       /* [in] starting address in AP RAM */
   const uint8_t *buf,  /* [in] specifies the data to write */
   uint16_t n           /* [in] number of bytes to write */
);


/******************************************************************************
Summary:
   Positions the 128 byte window in the AP address space.
Description:
   The last 128-byte window position is stored in the BAST_Handle struct,
   so I2C accesses will occur only if the window has moved.
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BAST_4506_P_SetApWindow(
   BAST_Handle h,    /* [in] BAST handle */
   uint32_t window   /* [in] base address of the 128-byte window */
);


/******************************************************************************
Summary:
   Resets the AP.
Description:
   This function puts the AP in reset state.
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BAST_4506_P_ResetAp(
   BAST_Handle h   /* [in] BAST handle */
);


/******************************************************************************
Summary:
   Runs the AP.
Description:
   This function takes the AP to out of reset/idle state and starts running.
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BAST_4506_P_RunAp(
   BAST_Handle h   /* [in] BAST handle */
);


/******************************************************************************
Summary:
   Disables all host interrupts.
Description:
   This function clears the host interrupt enable bits on the BCM4506.
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BAST_4506_P_DisableInterrupts(
   BAST_Handle h   /* [in] BAST handle */
);


/******************************************************************************
Summary:
   Returns the BERR_Code corresponding to the given BAST_ApStatus.
Description:
   This function returns ths BERR_Code of any errors in the given BAST_ApStatus.
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BAST_4506_P_DecodeError(
   BAST_Handle h,           /* [in] BAST handle */
   BAST_ApStatus *pApStatus /* [in] AP status returned by BAST_GetApStatus */
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
BERR_Code BAST_4506_P_SendHabCommand(
   BAST_Handle h,      /* [in] BAST handle */
   uint8_t *write_buf, /* [in] specifies the HAB command to send */
   uint8_t write_len,  /* [in] number of bytes in the HAB command */ 
   uint8_t *read_buf,  /* [out] holds the data read from the HAB */ 
   bool    bChecksum   /* [in] true = automatically compute checksum */
);


/******************************************************************************
Summary:
   This function sends the command already written in the HAB.
Description:
   This function asserts HABR, waits for the AP to release the HAB, then 
   optionally checks for a service acknowlegement from the AP.  If bCheckForAck
   is true, then read_len must be greater than 0.  This function is called by
   BAST_4506_P_SendHabCommand().
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BAST_4506_P_ServiceHab(
   BAST_Handle h,   /* [in] BAST handle */
   uint8_t *read_buf,  /* [out] holds the data read from the HAB */ 
   uint8_t read_len,   /* [in] number of bytes to read from the HAB */
   uint8_t ack_byte,   /* [in] value of the ack byte to expect */
   bool    bChecksum   /* [in] true = automatically compute checksum */
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
BERR_Code BAST_4506_P_CheckHab(
   BAST_Handle h  /* [in] BAST handle */
);


/******************************************************************************
Summary:
   Enables/Disables the L1 host interrupt.
Description:
   Enables/Disables the L1 host interrupt by calling the application-supplied 
   callback routine the the BAST settings.
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BAST_4506_P_EnableHostInterrupt(
   BAST_Handle h, /* [in] BAST PI handle */
   bool bEnable   /* [in] true=enables the L1 interrupt on the host processor */
);


/******************************************************************************
Summary:
   Enables/Disables the downstream lock interrupt.
Description:
   This function enables or disables an interrupt to be generated upon change 
   in downstream lock status.
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BAST_4506_P_EnableLockInterrupt(
   BAST_ChannelHandle h, /* [in] BAST channel handle */
   bool bEnable   /* [in] true = enable lock interrupts, false = disables lock interrupts */
);


/******************************************************************************
Summary:
   Enables/Disables the FTM interrupt.
Description:
   This function enables or disables the FTM interrupt.
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BAST_4506_P_EnableFtmInterrupt(
   BAST_Handle h, /* [in] BAST handle */
   bool bEnable   /* [in] true = enable the BSC interrupt */
);


/******************************************************************************
Summary:
   Sets certain BKNI_Events based on the source of the BCM4506 host interrupt.
Description:
   This function reads and clears the BCM4506 interrupt status registers, 
   clears the enable mask of the interrupts, and sets BKNI_Events based on the 
   interrupt sources.
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BAST_4506_P_DecodeInterrupt(
   BAST_Handle h /* [in] BAST handle */
);


/******************************************************************************
Summary:
   Waits for a BAST event. 
Description:
   This function waits for an interrupt within the given timeout period.  Any
   interrupts are then decoded via I2C.  If the given event has been signaled,
   then this function returns BERR_SUCCESS.
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BAST_4506_P_WaitForEvent(
   BAST_Handle h,             /* [in] BAST handle */
   BKNI_EventHandle hEvent,   /* [in] event to wait on */
   int timeoutMsec            /* [in] timeout in milliseconds */
);


/******************************************************************************
Summary:
   Write to an indirect host register
Description:
   This function writes to an indirect host register.
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BAST_4506_P_WriteHostIndirectRegister(
   BAST_Handle h,    /* [in] BAST handle */
   uint8_t     reg,  /* [in] address of indirect register */
   uint8_t     *pVal /* [in] value to write */
);


/******************************************************************************
Summary:
   Read an indirect host register
Description:
   This function reads an indirect host register.
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BAST_4506_P_ReadHostIndirectRegister(
   BAST_Handle h,    /* [in] BAST handle */
   uint8_t     reg,  /* [in] address of indirect register */
   uint8_t     *pVal /* [out] value read */
);


/* BCM4506 implementation of API functions */
BERR_Code BAST_4506_P_Open(BAST_Handle *, BCHP_Handle, void*, BINT_Handle, const BAST_Settings *pDefSettings);
BERR_Code BAST_4506_P_Close(BAST_Handle);
BERR_Code BAST_4506_P_GetTotalChannels(BAST_Handle, uint32_t *);
BERR_Code BAST_4506_P_OpenChannel(BAST_Handle, BAST_ChannelHandle *, uint32_t chnNo, const BAST_ChannelSettings *pChnDefSettings);
BERR_Code BAST_4506_P_CloseChannel(BAST_ChannelHandle);
BERR_Code BAST_4506_P_GetDevice(BAST_ChannelHandle, BAST_Handle *);
BERR_Code BAST_4506_P_InitAp(BAST_Handle, const uint8_t *);
BERR_Code BAST_4506_P_SoftReset(BAST_Handle);
BERR_Code BAST_4506_P_GetApStatus(BAST_Handle, BAST_ApStatus *);
BERR_Code BAST_4506_P_GetApVersion(BAST_Handle, uint16_t*, uint8_t*, uint32_t*, uint8_t*, uint8_t*);
BERR_Code BAST_4506_P_ConfigGpio(BAST_Handle, uint32_t, uint32_t);
BERR_Code BAST_4506_P_SetGpio(BAST_Handle, uint32_t, uint32_t);
BERR_Code BAST_4506_P_GetGpio(BAST_Handle, uint32_t, uint32_t *);
BERR_Code BAST_4506_P_TuneAcquire(BAST_ChannelHandle, const uint32_t, const BAST_AcqSettings *);
BERR_Code BAST_4506_P_GetChannelStatus(BAST_ChannelHandle, BAST_ChannelStatus *);
BERR_Code BAST_4506_P_GetLockStatus(BAST_ChannelHandle, bool *);
BERR_Code BAST_4506_P_ResetStatus(BAST_ChannelHandle);
BERR_Code BAST_4506_P_SetDiseqcTone(BAST_ChannelHandle, bool);
BERR_Code BAST_4506_P_GetDiseqcTone(BAST_ChannelHandle, bool*);
BERR_Code BAST_4506_P_SetDiseqcVoltage(BAST_ChannelHandle, bool);
BERR_Code BAST_4506_P_GetDiseqcVoltage(BAST_ChannelHandle, uint8_t*);
BERR_Code BAST_4506_P_EnableVsenseInterrupts(BAST_ChannelHandle, bool);
BERR_Code BAST_4506_P_SendACW(BAST_ChannelHandle, uint8_t);
BERR_Code BAST_4506_P_SendDiseqcCommand(BAST_ChannelHandle, const uint8_t *pSendBuf, uint8_t sendBufLen);
BERR_Code BAST_4506_P_GetDiseqcStatus(BAST_ChannelHandle, BAST_DiseqcStatus *pStatus);
BERR_Code BAST_4506_P_ResetDiseqc(BAST_ChannelHandle, uint8_t options);
BERR_Code BAST_4506_P_ResetFtm(BAST_Handle);
BERR_Code BAST_4506_P_ReadFtm(BAST_Handle, uint8_t *pBuf, uint8_t *n);
BERR_Code BAST_4506_P_WriteFtm(BAST_Handle, uint8_t *pBuf, uint8_t n);
BERR_Code BAST_4506_P_PowerDownFtm(BAST_Handle);
BERR_Code BAST_4506_P_PowerUpFtm(BAST_Handle);
BERR_Code BAST_4506_P_WriteMi2c(BAST_ChannelHandle, uint8_t, uint8_t*, uint8_t);
BERR_Code BAST_4506_P_ReadMi2c(BAST_ChannelHandle, uint8_t, uint8_t*, uint8_t, uint8_t *, uint8_t);
BERR_Code BAST_4506_P_GetSoftDecisionBuf(BAST_ChannelHandle, int16_t*, int16_t*);
BERR_Code BAST_4506_P_ReadAgc(BAST_ChannelHandle, BAST_Agc, uint32_t*);
BERR_Code BAST_4506_P_WriteAgc(BAST_ChannelHandle, BAST_Agc, uint32_t*);
BERR_Code BAST_4506_P_FreezeAgc(BAST_ChannelHandle, BAST_Agc, bool);
BERR_Code BAST_4506_P_FreezeEq(BAST_ChannelHandle, bool);
BERR_Code BAST_4506_P_PowerDown(BAST_ChannelHandle, uint32_t);
BERR_Code BAST_4506_P_PowerUp(BAST_ChannelHandle, uint32_t);
BERR_Code BAST_4506_P_ReadRegister(BAST_ChannelHandle, uint32_t, uint32_t*);
BERR_Code BAST_4506_P_WriteRegister(BAST_ChannelHandle, uint32_t, uint32_t*);
BERR_Code BAST_4506_P_ReadConfig(BAST_ChannelHandle, uint16_t, uint8_t*, uint8_t);
BERR_Code BAST_4506_P_WriteConfig(BAST_ChannelHandle, uint16_t, uint8_t*, uint8_t);
BERR_Code BAST_4506_P_GetInterruptEventHandle(BAST_Handle, BKNI_EventHandle*);
BERR_Code BAST_4506_P_GetLockStateChangeEventHandle(BAST_ChannelHandle, BKNI_EventHandle*);
BERR_Code BAST_4506_P_GetFtmEventHandle(BAST_Handle, BKNI_EventHandle*);
BERR_Code BAST_4506_P_GetDiseqcEventHandle(BAST_ChannelHandle, BKNI_EventHandle*);
BERR_Code BAST_4506_P_GetDiseqcVsenseEventHandles(BAST_ChannelHandle, BKNI_EventHandle*, BKNI_EventHandle*);
BERR_Code BAST_4506_P_HandleInterrupt_isr(BAST_Handle);
BERR_Code BAST_4506_P_ProcessInterruptEvent(BAST_Handle);
BERR_Code BAST_4506_P_AbortAcq(BAST_ChannelHandle);
BERR_Code BAST_4506_P_ConfigLna(BAST_Handle, BAST_LnaIntConfig int_config, BAST_LnaExtConfig ext_config);
BERR_Code BAST_4506_P_GetLnaStatus(BAST_Handle, BAST_LnaStatus *pStatus);
BERR_Code BAST_4506_P_DeepSleep(BAST_Handle h);
BERR_Code BAST_4506_P_PeakScan(BAST_ChannelHandle, uint32_t);
BERR_Code BAST_4506_P_GetPeakScanStatus(BAST_ChannelHandle, BAST_PeakScanStatus*);
BERR_Code BAST_4506_P_GetPeakScanEventHandle(BAST_ChannelHandle, BKNI_EventHandle*);
BERR_Code BAST_4506_P_EnableStatusInterrupts(BAST_ChannelHandle, bool);
BERR_Code BAST_4506_P_GetStatusEventHandle(BAST_ChannelHandle, BKNI_EventHandle*);
BERR_Code BAST_4506_P_ConfigBcm3445(BAST_Handle h, BAST_Bcm3445Settings *pSettings);
BERR_Code BAST_4506_P_MapBcm3445ToTuner(BAST_ChannelHandle h, BAST_Mi2cChannel mi2c, BAST_Bcm3445OutputChannel out);
BERR_Code BAST_4506_P_GetBcm3445Status(BAST_ChannelHandle h, BAST_Bcm3445Status *pStatus);
BERR_Code BAST_4506_P_ConfigAgc(BAST_Handle h, uint32_t ctl);
BERR_Code BAST_4506_P_ReadIdata(BAST_Handle h, uint8_t page, uint8_t addr, uint8_t *buf, uint8_t n);
BERR_Code BAST_4506_P_ReadXdataPage(BAST_Handle h, uint8_t page, uint16_t offset, uint8_t *buf, uint8_t n);
BERR_Code BAST_4506_P_SetSearchRange(BAST_Handle, uint32_t);
BERR_Code BAST_4506_P_GetSearchRange(BAST_Handle, uint32_t*);
BERR_Code BAST_4506_P_SetAmcScramblingSeq(BAST_ChannelHandle, uint32_t xseed, uint32_t plhdrscr1, uint32_t plhdrscr2, uint32_t plhdrscr3);
BERR_Code BAST_4506_P_SetTunerFilter(BAST_ChannelHandle, uint32_t);
BERR_Code BAST_4506_P_GetSignalDetectStatus(BAST_ChannelHandle, BAST_SignalDetectStatus*);
BERR_Code BAST_4506_P_SetOutputTransportSettings(BAST_ChannelHandle, BAST_OutputTransportSettings*);
BERR_Code BAST_4506_P_GetOutputTransportSettings(BAST_ChannelHandle, BAST_OutputTransportSettings*);
BERR_Code BAST_4506_P_SetDiseqcSettings(BAST_ChannelHandle, BAST_DiseqcSettings*);
BERR_Code BAST_4506_P_GetDiseqcSettings(BAST_ChannelHandle, BAST_DiseqcSettings*);
BERR_Code BAST_4506_P_SetNetworkSpec(BAST_Handle, BAST_NetworkSpec);
BERR_Code BAST_4506_P_GetNetworkSpec(BAST_Handle, BAST_NetworkSpec*);
BERR_Code BAST_4506_P_SetFskChannel(BAST_Handle h, BAST_FskChannel fskTxChannel, BAST_FskChannel fskRxChannel);
BERR_Code BAST_4506_P_GetFskChannel(BAST_Handle h, BAST_FskChannel *fskTxChannel, BAST_FskChannel *fskRxChannel);
BERR_Code BAST_4506_P_SetPeakScanSymbolRateRange(BAST_ChannelHandle, uint32_t, uint32_t);
BERR_Code BAST_4506_P_GetPeakScanSymbolRateRange(BAST_ChannelHandle, uint32_t*, uint32_t*);

#ifdef __cplusplus
}
#endif

#endif /* BAST_4506_P_PRIV_H__ */

