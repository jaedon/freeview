/*************************************************************************
*     Copyright (c) 2005-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bthd_7550_platform.c $
* $brcm_Revision: Hydra_Software_Devel/1 $
* $brcm_Date: 9/11/12 11:46a $
*
* [File Description:]
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/thd/src/7550/bthd_7550_platform.c $
* 
* Hydra_Software_Devel/1   9/11/12 11:46a farshidf
* SW7425-3819: Remove symbolic links and flatten out THD directory
* structure
* 
* Hydra_Software_Devel/88   8/3/11 5:26p farshidf
* SWDTV-7399: fix compile issue
* 
* Hydra_Software_Devel/87   7/25/11 4:17p farshidf
* SWDTV-7399: new mode added for GuardMode to support more mode  in ISDBT
* 
* Hydra_Software_Devel/86   2/22/11 2:33p farshidf
* SW7550-675: stop the ISDB-T core to dump data to memory when we clsoe
* the THD
* 
* Hydra_Software_Devel/85   11/2/10 10:29a farshidf
* SW7550-607: update the rev version
* 
* Hydra_Software_Devel/84   10/28/10 6:26p farshidf
* SW7550-607: update the BBS interface reg
* 
* Hydra_Software_Devel/83   5/20/10 5:09p farshidf
* SW7550-432: add more comment for coverity check
* 
* Hydra_Software_Devel/82   5/19/10 2:58p farshidf
* SW7550-440: merge to main branch
* 
* Hydra_Software_Devel/THD_7550_40_17/1   4/30/10 4:24p rperlow
* Unset RESERVED2_EN_CPMOD to enable AC1 ECO for A1
* 
* Hydra_Software_Devel/80   4/20/10 11:31a farshidf
* SW7550-38: fix typo mistake
* 
* Hydra_Software_Devel/79   4/19/10 6:51p farshidf
* SW7550-38: error checking
* 
* Hydra_Software_Devel/78   4/19/10 1:28p farshidf
* SW7550-38: namiing update
* 
* Hydra_Software_Devel/77   4/19/10 11:12a farshidf
* SW7550-38: merge the THD changes from branch 16
* 
* Hydra_Software_Devel/THD_7550_40_16/2   4/19/10 11:11a farshidf
* SW7550-38: add the memory allocation for A1 chip which requires the
* linux to boot with bmem=100M@28M
* 
* Hydra_Software_Devel/THD_7550_40_16/1   4/15/10 7:15p farshidf
* SW7550-38: add the memory allocation for isdbt
* 
* Hydra_Software_Devel/76   4/8/10 6:03p farshidf
* SW7550-38: add error meassge in case of out of memory
* 
* Hydra_Software_Devel/THD_7550_40_15/7   4/6/10 3:59p farshidf
* SW7550-38: update the register name
* 
* Hydra_Software_Devel/THD_7550_40_15/6   3/23/10 6:15p mbsingh
* Added debug message to print inputs coming into tuneacquire() command
* 
* Hydra_Software_Devel/THD_7550_40_15/5   3/23/10 2:23p rperlow
* o Fix to clear AcquireStartMode before setting scan and auto mode bits
* 
* Hydra_Software_Devel/THD_7550_40_15/4   3/22/10 3:16p mbsingh
* A. Backed Off the No Signal threshold based on the detailed
* measurements. Some of the correlation values near sensitivity were too
* near.
* 
* B. Scaled the threshold based on FFT mode (2k,4k,8k) to get rid og more
* No Signal channels. Scaling by 2x and 4x could have been chosen for 4k
* and 8k.  Took a little conservative 2x and 3x scaling.
* 
* Note: The ISDB-T scan-time on the lab feed went from about 41sec to 22
* sec. with this threshold scaling weeding our more channels
* 
* Hydra_Software_Devel/THD_7550_40_15/3   3/11/10 12:20a rperlow
* o Changed Nexus use of Auto TransGuardMode to Auto_ISDBT or Auto_DVBT
* based on which standard is being used.
* 
* Hydra_Software_Devel/THD_7550_40_15/2   3/5/10 10:49p rperlow
* Manual merge of phase 3 changes.
* 
* Hydra_Software_Devel/THD_7550_40_15/1   2/26/10 9:39a farshidf
* SW7550-38: reset the Thd event
* 
* Hydra_Software_Devel/71   2/11/10 6:59p farshidf
* SW7550-38: fix bbs interface re_aqcuire
* 
* Hydra_Software_Devel/70   2/11/10 6:50p farshidf
* SW7550-38: fix the process function
* 
* Hydra_Software_Devel/69   2/11/10 5:28p farshidf
* SW7550-38: remove the lock check from thread to existing timer
* 
* Hydra_Software_Devel/68   2/11/10 11:30a farshidf
* SW7550-38: add the new lock function from Randy
* 
* Hydra_Software_Devel/66   2/3/10 6:08p farshidf
* SW7550-38: typo fix
* 
* Hydra_Software_Devel/65   2/3/10 6:05p farshidf
* SW7550-38: store the previous lock event and send an event in case we
* switch from unlock to lock after acquisition
* 
* Hydra_Software_Devel/64   2/2/10 6:17p farshidf
* SW7550-38: update BBS and to send new acquire command from BBS while we
* are in auto mode
* 
* Hydra_Software_Devel/THD_7550_40_14/1   1/29/10 6:16p farshidf
* SW7550-38: enable the software workaroung only once and stop teh timer
* for each new acquire
* 
* Hydra_Software_Devel/62   1/29/10 4:11p farshidf
* SW7550-38: change the seetings channel estimator and FFt window
* 
* Hydra_Software_Devel/61   1/12/10 9:55a farshidf
* SW7550-38: merge SW7550-161
* 
* Hydra_Software_Devel/SW7550-161-take2/2   1/12/10 9:58p pinglong
* SW7550-161: initialize the status of EWS fire when
* NEXUS_Frontend_TuneOfdm() is called
* 
* Hydra_Software_Devel/SW7550-161-take2/1   1/12/10 9:47p pinglong
* SW7550-161: initialize the status of EWS fire when
* NEXUS_Frontend_TuneOfdm() is called
* 
* Hydra_Software_Devel/60   1/11/10 11:09a farshidf
* SW7550-38: merge SW7550-161
* 
* Hydra_Software_Devel/SW7550-161/2   1/9/10 4:53p pinglong
* SW7550-161: when the status is unlock, set EWS event if EWS has been
* started.
* 
* Hydra_Software_Devel/SW7550-161/1   1/8/10 9:01p pinglong
* SW7550-161: When the cable is unplugged, we need to send an EWS event.
* 
* Hydra_Software_Devel/57   1/7/10 5:49p farshidf
* SW7550-38: fix the EWS and lock/unlock event seeting
* 
* Hydra_Software_Devel/56   1/6/10 11:16a farshidf
* SW7550-38: remove the printf
* 
* Hydra_Software_Devel/55   1/5/10 4:39p farshidf
* SW7550-38: add the EWS callback
* 
* Hydra_Software_Devel/54   1/5/10 2:40p farshidf
* SW7550-38: add the stop and start EWS
* 
* Hydra_Software_Devel/53   12/29/09 5:30p farshidf
* SW7550-38: add a thd reset at thd open to reset the THD core to avoid
* no signal issue at the intial power up in some chip
* 
* Hydra_Software_Devel/52   12/29/09 4:31p farshidf
* SW7550-38: send one event per status changes
* 
* Hydra_Software_Devel/51   12/23/09 9:17a farshidf
* SW7550-38: fix the EWS check
* 
* Hydra_Software_Devel/50   12/22/09 2:57p farshidf
* SW7550-38: unset the flag after status update
* 
* Hydra_Software_Devel/49   12/21/09 3:31p farshidf
* SW7550-38: hack the code for Nexus BBS support
* 
* Hydra_Software_Devel/48   12/21/09 1:26p farshidf
* SW7550-38: make compatible to Nexus
* 
* Hydra_Software_Devel/47   12/18/09 6:41p farshidf
* SW7550-38: compile fix
* 
* Hydra_Software_Devel/46   12/18/09 6:36p farshidf
* SW7550-38: add the status update for nexus BBS
* 
* Hydra_Software_Devel/45   12/14/09 9:37a farshidf
* SW7550-38: add Resetevent
* 
* Hydra_Software_Devel/44   12/11/09 6:38p farshidf
* SW7550-38: add the event check
* 
* Hydra_Software_Devel/43   12/11/09 6:30p farshidf
* SW7550-38: code clean up
* 
* Hydra_Software_Devel/42   12/8/09 6:35p farshidf
* SW7550-38: remove the resetirq call due to nexus crash
* 
* Hydra_Software_Devel/41   12/8/09 3:28p farshidf
* SW7550-38: remove non os calls
* 
* Hydra_Software_Devel/40   12/4/09 3:41p farshidf
* SW7550-38: add the reset status
* 
* Hydra_Software_Devel/THD_7550_40_9/1   12/4/09 3:26p jputnam
* Clear status structure upon new acquire command from host
* 
* Hydra_Software_Devel/38   12/4/09 12:41p farshidf
* SW7550-38: noos fix
* 
* Hydra_Software_Devel/37   12/4/09 10:17a farshidf
* SW7550-38: workaround for NoOS
* 
* Hydra_Software_Devel/35   12/3/09 2:30p farshidf
* SW7550-38: add the nexus status check
* 
* Hydra_Software_Devel/34   12/3/09 2:22p farshidf
* SW7550-38: fix the FFT IRQ repition
* 
* Hydra_Software_Devel/33   12/1/09 2:03p farshidf
* SW7550-38: add auto acquire for noos
* 
* Hydra_Software_Devel/32   12/1/09 11:56a farshidf
* SW7550-38: use irq for FBCNT zero
* 
* Hydra_Software_Devel/31   12/1/09 11:01a farshidf
* SW7550-38: fix the warning
* 
* Hydra_Software_Devel/SW7550_84/1   11/30/09 11:05p farshidf
* SW7550-84: fic the seg falut issue
* 
* Hydra_Software_Devel/SW7550_84/1   11/30/09 11:04p farshidf
* SW7550-38: check-in the crash fix
* 
* Hydra_Software_Devel/SW7550_84/1   11/30/09 11:03p farshidf
* SW7550-84: fix the crash problem
* 
* Hydra_Software_Devel/24   11/24/09 12:18p farshidf
* SW7550-38: change it to Internal tuner
* 
* Hydra_Software_Devel/23   11/23/09 2:37p farshidf
* SW7550-38: disable irq for noos
* 
* Hydra_Software_Devel/21   11/20/09 1:43p farshidf
* SW7550-38: remove warning in the code
* 
* Hydra_Software_Devel/20   11/20/09 1:19p farshidf
* SW7550-38: switch to external tuner
* 
* Hydra_Software_Devel/19   11/20/09 11:38a farshidf
* SW7550-38: merge the branch
* 
* Hydra_Software_Devel/THD_7550_40_7/2   11/20/09 11:14a jputnam
* Added new TMCCMode acquire parameter separate from TPSMode
* 
* Hydra_Software_Devel/THD_7550_40_7/1   11/19/09 10:17p jputnam
* Temporarily disable interrupt callbacks until they are functional
* 
* Hydra_Software_Devel/18   11/19/09 4:23p farshidf
* SW7550-38: use reserved2 for BBS
* 
* Hydra_Software_Devel/17   11/19/09 3:47p farshidf
* SW7550-38: put the timer under nexus
* 
* Hydra_Software_Devel/16   11/19/09 3:33p farshidf
* SW7550-38: merge the latest branch
* 
* Hydra_Software_Devel/THD_7550_40_6/1   11/19/09 9:57a jputnam
* Changed QDSAFE default to internal tuner path
* 
* Hydra_Software_Devel/14   11/17/09 5:11p farshidf
* SW7550-38: integrate the latest THD code for Nexus interface
* 
* Hydra_Software_Devel/THD_7550_40_5/10   11/17/09 2:47p rperlow
* Fixed bug in comment
* 
* Hydra_Software_Devel/THD_7550_40_5/9   11/17/09 10:03a rperlow
* Change to avoid overwriing MIPS exception vectors.
* 
* Hydra_Software_Devel/THD_7550_40_5/8   11/16/09 9:16p rperlow
* Changes for MIPS Exception Vectiors
* 
* Hydra_Software_Devel/THD_7550_40_5/7   11/16/09 7:54p jputnam
* Added DS PLL programming and power-up for ISDB-T external tuner mode
* 
* Hydra_Software_Devel/THD_7550_40_5/6   11/16/09 9:40a jputnam
* Set QDSAFE PGA to minimum gain (0dB)
* 
* Hydra_Software_Devel/THD_7550_40_5/5   11/16/09 8:00a jputnam
* Removed unnecessary PLL and AFE configuration from P_Init_Clk
* Enable THD AGC PWM outputs to chip pins for external tuner
* 
* Hydra_Software_Devel/THD_7550_40_5/4   11/13/09 9:37a jputnam
* Moved call to SetDefaultSettings for non-OS to avoid overwriting
* AcquireParams written by BBS
* 
* Hydra_Software_Devel/THD_7550_40_5/3   11/12/09 2:24p jputnam
* Wait for Acquire command from BBS
* Continuously update status memory for BBS
* 
* Hydra_Software_Devel/THD_7550_40_5/2   11/11/09 9:22p jputnam
* Renamed THDStatus fields to match AcquireParam field names
* 
* Hydra_Software_Devel/THD_7550_40_5/1   11/11/09 4:27p jputnam
* Added writes for QDSAFE power-up
* 
* Hydra_Software_Devel/13   11/11/09 12:30p farshidf
* SW7550-38: new changes for the platform
* 
* Hydra_Software_Devel/12   11/10/09 2:20p farshidf
* SW7550-38: add the temp fix to setup  the AFE for DVB-T/ISDB-T
* terrestrial operation
* 
* Hydra_Software_Devel/11   11/10/09 2:14p farshidf
* SW7550-38: add the EWS event
* 
* Hydra_Software_Devel/10   11/6/09 6:07p farshidf
* SW7550-38: changes to match nexus
* 
* Hydra_Software_Devel/9   11/6/09 2:18p farshidf
* SW7550-38: add the new acquire param
* 
* Hydra_Software_Devel/8   11/5/09 2:54p farshidf
* SW7550-38: add the EWS changes
* 
* Hydra_Software_Devel/7   11/2/09 10:33a farshidf
* SW7550-38: remove timer for emulation
* 
* Hydra_Software_Devel/5   11/1/09 1:56p farshidf
* SW7550-38: addthe latest changes
* 
* Hydra_Software_Devel/4   11/1/09 1:32p farshidf
* SW7550-38: changes added to move down the Timer to PI
* 
* Hydra_Software_Devel/3   11/1/09 1:29p farshidf
* SW7550-38: merge the latest
* 
* Hydra_Software_Devel/THD_7550_40/2   10/9/09 12:45p farshidf
* PR41450: port the 7550 changes: status, dynamic param
* 
* Hydra_Software_Devel/THD_7550_40/1   9/21/09 4:07p jputnam
* SW7550-40:  Fixed VELOCE_EMULATION ifdef in P_TuneAcquire
* - Cleaned-up FI coefficient tables
* - Reworked WriteFICoef() function
* - Cosmetic cleanup of indentation and comments
* 
* Hydra_Software_Devel/2   9/16/09 6:45p farshidf
* SW7550-38: update the write field macro
* 
* Hydra_Software_Devel/1   7/31/09 4:26p farshidf
* PR41450: add the 7550 thd code
* 
***************************************************************************/
#include "bstd.h"

#include "bchp_7550.h"
#include "bkni.h"
#include "bthd.h"
#include "bthd_priv.h"
#include "bchp_thd_core.h"
#include "bthd_7550_priv.h"
#include "bthd_7550_platform.h"
#include "bchp_int_id_thd_intr2.h"
#include "bchp_memc_gen_0.h"
#include "bchp_sun_top_ctrl.h"


BDBG_MODULE(bthd_7550_platform);


/***************************************************************************
* BTHD_7550_P_ThdEvent_isr()
***************************************************************************/
void BTHD_7550_P_ThdEvent_isr(void *p, int param)
{
  BTHD_Handle h = (BTHD_Handle)p;
  BTHD_7550_P_Handle *pHandle;

  BDBG_ASSERT(p);
  BDBG_ASSERT(h);
  BDBG_ASSERT(h->pImpl);

  pHandle = (BTHD_7550_P_Handle *)(h->pImpl);

  switch(param) {  
  case THD_EVENT_FFT_SYNC:
    BKNI_SetEvent(pHandle->hFwSyncEvent);
    break;
  case THD_EVENT_SP_SYNC:
    BKNI_SetEvent(pHandle->hSpSyncEvent);
    break;
  case THD_EVENT_TPS_SYNC:
    BKNI_SetEvent(pHandle->hTpsSyncEvent);
    break;
  case THD_EVENT_TMCC_SYNC:
    BKNI_SetEvent(pHandle->hTmccSyncEvent);
    break;
  case THD_EVENT_FEC_SYNC:
    BKNI_SetEvent(pHandle->hFecSyncEvent);
    break;
  case THD_EVENT_FW_CORR_MAX_RDY:
    BKNI_SetEvent(pHandle->hFwCorrMaxEvent);
    break;
  case THD_EVENT_FBCNT_ZERO:
    BKNI_SetEvent(pHandle->hFbcntZeroEvent);
    break;
  }
}



/******************************************************************************
BTHD_7550_P_Open() done
******************************************************************************/
BERR_Code BTHD_7550_P_Open(
                           BTHD_Handle *pthd,       /* [out] BTHD handle */
                           BCHP_Handle hChip,    /* [in] chip handle */
                           void* hRegister,              /* Register handle */
                           BINT_Handle hInterrupt,             /* Interrupt handle */
                           const BTHD_Settings *pDefSettings /* [in] default settings */
                           )
{
  BERR_Code retCode;
  BTHD_Handle hDev;
  BTHD_7550_P_Handle *p7550;
  uint32_t BufSrc;
  BTMR_Settings sTimerSettings;
  BSTD_UNUSED(hInterrupt);
  BSTD_UNUSED(hChip);
  BDBG_ASSERT(hRegister);
  BDBG_ASSERT(pDefSettings);
  BDBG_ASSERT(pDefSettings->hTmr);
  /* allocate memory for the handle */
  hDev = (BTHD_Handle)BKNI_Malloc(sizeof(BTHD_P_Handle));
  BDBG_ASSERT(hDev);
  p7550 = (BTHD_7550_P_Handle *)BKNI_Malloc(sizeof(BTHD_7550_P_Handle));
  BDBG_ASSERT(p7550);
  hDev->pImpl = (void*)p7550;
  BKNI_Memcpy(&hDev->settings, pDefSettings, sizeof(*pDefSettings));

  BDBG_MSG(("BTHD_7550_P_Open \n"));
  /* initialize our handle */
  p7550->hRegister = (BREG_Handle)hRegister;

  BREG_AtomicUpdate32(((BTHD_7550_P_Handle *)(hDev->pImpl))->hRegister, BCHP_SUN_TOP_CTRL_SW_RESET, 		 BCHP_MASK(SUN_TOP_CTRL_SW_RESET, thd_sw_reset),		 BCHP_FIELD_DATA(SUN_TOP_CTRL_SW_RESET, thd_sw_reset, 1));	
  BREG_AtomicUpdate32(((BTHD_7550_P_Handle *)(hDev->pImpl))->hRegister, BCHP_SUN_TOP_CTRL_SW_RESET, 		 BCHP_MASK(SUN_TOP_CTRL_SW_RESET, thd_sw_reset),		 BCHP_FIELD_DATA(SUN_TOP_CTRL_SW_RESET, thd_sw_reset, 0));
 
#if (BCHP_VER >= BCHP_VER_A1)
  p7550->pIsdbtMemory = (uint32_t *)BMEM_AllocAligned(hDev->settings.hHeap, 2211840, 22, 0 );
  if (! p7550->pIsdbtMemory )
  {
		retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
		goto done;
  }
  BMEM_ConvertAddressToOffset(hDev->settings.hHeap, p7550->pIsdbtMemory, &BufSrc );
  BKNI_Memset( p7550->pIsdbtMemory, 0x00,  2211840);
  BufSrc = BufSrc >> 22;
  BTHD_7550_P_WriteField(((BTHD_7550_P_Handle *)(hDev->pImpl))->hRegister, THD_CORE_RESERVED2, SCB_ADDRESS_OFFSET, BufSrc);
  BTHD_7550_P_WriteField(((BTHD_7550_P_Handle *)(hDev->pImpl))->hRegister, THD_CORE_RESERVED2, EN_CPMOD, 0);
#endif  

  p7550->pAcquireParam = (BTHD_7550_P_AcquireParam *)BMEM_AllocAligned(hDev->settings.hHeap, sizeof(BTHD_7550_P_AcquireParam), 0, 0 );
  if (! p7550->pAcquireParam )
  {
		retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
		goto done;
  }
  BMEM_ConvertAddressToOffset(hDev->settings.hHeap, p7550->pAcquireParam, &BufSrc );
  BKNI_Memset( p7550->pAcquireParam, 0x00, sizeof( BTHD_7550_P_AcquireParam ) );
  BREG_Write32(((BTHD_7550_P_Handle *)(hDev->pImpl))->hRegister, BCHP_THD_CORE_RESERVED1, BufSrc);
  p7550->pStatus = (BTHD_P_7550_THDStatus *)BMEM_AllocAligned(hDev->settings.hHeap, sizeof(BTHD_P_7550_THDStatus), 0, 0 );
  if (! p7550->pStatus )
  {
		retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
		goto done;
  }
  BMEM_ConvertAddressToOffset(hDev->settings.hHeap, p7550->pStatus, &BufSrc );
  p7550->pAcquireParam->StatusStructureAddress = BufSrc;
  BKNI_Memset( p7550->pStatus, 0x00, sizeof( BTHD_P_7550_THDStatus ) );

  BDBG_MSG(("THD:Creating THD Core interrupts\n"));

  /* THD Core interrupts */

  BTHD_CHK_RETCODE(BINT_CreateCallback( &(p7550->hFwCorrMaxCallback), hInterrupt, 
    BCHP_INT_ID_FW_CORR_MAX_RDY_INTR, BTHD_7550_P_ThdEvent_isr, (void*)hDev, THD_EVENT_FW_CORR_MAX_RDY ));
  BTHD_CHK_RETCODE(BINT_CreateCallback( &(p7550->hFwSyncCallback), hInterrupt, 
    BCHP_INT_ID_FW_SYNC_INTR, BTHD_7550_P_ThdEvent_isr, (void*)hDev, THD_EVENT_FFT_SYNC));
  BTHD_CHK_RETCODE(BINT_CreateCallback( &(p7550->hSpSyncCallback), hInterrupt, 
    BCHP_INT_ID_SP_SYNC_INTR, BTHD_7550_P_ThdEvent_isr, (void*)hDev, THD_EVENT_SP_SYNC ));
  BTHD_CHK_RETCODE(BINT_CreateCallback( &(p7550->hTpsSyncCallback), hInterrupt, 
    BCHP_INT_ID_TPS_SYNC_INTR, BTHD_7550_P_ThdEvent_isr, (void*)hDev, THD_EVENT_TPS_SYNC ));
  BTHD_CHK_RETCODE(BINT_CreateCallback( &(p7550->hTmccSyncCallback), hInterrupt,
    BCHP_INT_ID_TMCC_SYNC_INTR, BTHD_7550_P_ThdEvent_isr, (void*)hDev, THD_EVENT_TMCC_SYNC ));
  BTHD_CHK_RETCODE(BINT_CreateCallback( &(p7550->hFecSyncCallback), hInterrupt, 
    BCHP_INT_ID_FEC_SYNC_INTR, BTHD_7550_P_ThdEvent_isr, (void*)hDev, THD_EVENT_FEC_SYNC ));
  BTHD_CHK_RETCODE(BINT_CreateCallback( &(p7550->hFbcCntCallback), hInterrupt, 
    BCHP_INT_ID_FBCNT_ZERO_INTR, BTHD_7550_P_ThdEvent_isr, (void*)hDev, THD_EVENT_FBCNT_ZERO ));


  /* Create timer for status lock check */
  BTMR_GetDefaultTimerSettings(&sTimerSettings);
    sTimerSettings.type = BTMR_Type_eCountDown;
  sTimerSettings.cb_isr = (BTMR_CallbackFunc)BTHD_7550_P_TimerFunc;
  sTimerSettings.pParm1 = (void*)hDev;
  sTimerSettings.parm2 = 0;
  sTimerSettings.exclusive = false;

  retCode = BTMR_CreateTimer (hDev->settings.hTmr, &p7550->hTimer, &sTimerSettings);
  if ( retCode != BERR_SUCCESS )
  {
    BDBG_ERR(("BTHD_Open: Create Timer Failed"));
    retCode = BERR_TRACE(retCode);
    goto done;
  }	
  /* create events */


  BTHD_CHK_RETCODE(BKNI_CreateEvent(&(p7550->hLockEvent)));
  BTHD_CHK_RETCODE(BKNI_CreateEvent(&(p7550->hFwCorrMaxEvent)));
  BTHD_CHK_RETCODE(BKNI_CreateEvent(&(p7550->hFwSyncEvent)));
  BTHD_CHK_RETCODE(BKNI_CreateEvent(&(p7550->hSpSyncEvent)));
  BTHD_CHK_RETCODE(BKNI_CreateEvent(&(p7550->hTpsSyncEvent)));
  BTHD_CHK_RETCODE(BKNI_CreateEvent(&(p7550->hFecSyncEvent)));
  BTHD_CHK_RETCODE(BKNI_CreateEvent(&(p7550->hTmccSyncEvent)));
  BTHD_CHK_RETCODE(BKNI_CreateEvent(&(p7550->hInterruptEvent)));
  BTHD_CHK_RETCODE(BKNI_CreateEvent(&(p7550->hEWSEvent)));
  BTHD_CHK_RETCODE(BKNI_CreateEvent(&(p7550->hFbcntZeroEvent)));



  *pthd = hDev;

done:
  return retCode;
}

/******************************************************************************
BTHD_7550_P_ResetIrq()
******************************************************************************/
void BTHD_7550_P_ResetThdEvent(BTHD_Handle h)
{
  BDBG_ASSERT(h);
  BDBG_MSG(("BTHD_7550_P_ResetThdEvent"));

  BKNI_ResetEvent(((BTHD_7550_P_Handle *)(h->pImpl))->hFwCorrMaxEvent);
  BKNI_ResetEvent(((BTHD_7550_P_Handle *)(h->pImpl))->hFwSyncEvent);
  BKNI_ResetEvent(((BTHD_7550_P_Handle *)(h->pImpl))->hSpSyncEvent);
  BKNI_ResetEvent(((BTHD_7550_P_Handle *)(h->pImpl))->hTpsSyncEvent);
  BKNI_ResetEvent(((BTHD_7550_P_Handle *)(h->pImpl))->hTmccSyncEvent);
  BKNI_ResetEvent(((BTHD_7550_P_Handle *)(h->pImpl))->hFecSyncEvent);
  BKNI_ResetEvent(((BTHD_7550_P_Handle *)(h->pImpl))->hFbcntZeroEvent);
}

/******************************************************************************
BTHD_7550_P_Close() done
******************************************************************************/
BERR_Code BTHD_7550_P_Close(BTHD_Handle h)
{
  BTHD_7550_P_Handle *p7550;
  BTHD_Settings DefSettings;

  BDBG_ASSERT(h);
  p7550 = (BTHD_7550_P_Handle *)(h->pImpl);
  DefSettings = h->settings;

  BTHD_7550_P_WriteField(p7550->hRegister, THD_CORE_GLB, BE_MODE, 0 );	           /* stop the ISDB-T dump to memory*/


  if (p7550->hTimer != NULL)
	BTMR_DestroyTimer(p7550->hTimer);
  BINT_DisableCallback( ((BTHD_7550_P_Handle *)(h->pImpl))->hFwCorrMaxCallback);
  BINT_DisableCallback( ((BTHD_7550_P_Handle *)(h->pImpl))->hFwSyncCallback);
  BINT_DisableCallback( ((BTHD_7550_P_Handle *)(h->pImpl))->hSpSyncCallback);
  BINT_DisableCallback( ((BTHD_7550_P_Handle *)(h->pImpl))->hTpsSyncCallback);
  BINT_DisableCallback( ((BTHD_7550_P_Handle *)(h->pImpl))->hTmccSyncCallback);
  BINT_DisableCallback( ((BTHD_7550_P_Handle *)(h->pImpl))->hFecSyncCallback);
  BINT_DisableCallback( ((BTHD_7550_P_Handle *)(h->pImpl))->hFbcCntCallback);

  /* since those events mighy already be deleted by ResetEvent function */


  BKNI_DestroyEvent(((BTHD_7550_P_Handle *)(h->pImpl))->hFwCorrMaxEvent);
  BKNI_DestroyEvent(((BTHD_7550_P_Handle *)(h->pImpl))->hFwSyncEvent);
  BKNI_DestroyEvent(((BTHD_7550_P_Handle *)(h->pImpl))->hSpSyncEvent);
  BKNI_DestroyEvent(((BTHD_7550_P_Handle *)(h->pImpl))->hTpsSyncEvent);
  BKNI_DestroyEvent(((BTHD_7550_P_Handle *)(h->pImpl))->hTmccSyncEvent);
  BKNI_DestroyEvent(((BTHD_7550_P_Handle *)(h->pImpl))->hFecSyncEvent);
  BKNI_DestroyEvent(((BTHD_7550_P_Handle *)(h->pImpl))->hFbcntZeroEvent);
  BKNI_DestroyEvent(((BTHD_7550_P_Handle *)(h->pImpl))->hLockEvent);
  BKNI_DestroyEvent(((BTHD_7550_P_Handle *)(h->pImpl))->hInterruptEvent);
  BKNI_DestroyEvent(((BTHD_7550_P_Handle *)(h->pImpl))->hEWSEvent);


#if (BCHP_VER >= BCHP_VER_A1)
  if (p7550->pIsdbtMemory != NULL)
     BMEM_Free(DefSettings.hHeap, p7550->pIsdbtMemory);
#endif
  if (p7550->pAcquireParam != NULL)
	BMEM_Free(DefSettings.hHeap, p7550->pAcquireParam);
  if (p7550->pStatus != NULL)
	BMEM_Free(DefSettings.hHeap, p7550->pStatus);
  if (h->pImpl != NULL)
	BKNI_Free((void*)h->pImpl);
  if (h != NULL)
	BKNI_Free((void*)h);

  return BERR_SUCCESS;
}

/******************************************************************************
BTHD_7550_P_Init() done
******************************************************************************/
BERR_Code BTHD_7550_P_Init(        
                           BTHD_Handle h,            /* [in] BTHD handle */
                           const uint8_t *pHexImage, /* [in] pointer to microcode image. Set to NULL to use default image */
                           uint32_t imageLength      /* [in] length of image. Not used in 7550 */
                           )
{

  BERR_Code retCode;
  BSTD_UNUSED(pHexImage);
  BSTD_UNUSED(imageLength);
  ((BTHD_7550_P_Handle *)(h->pImpl))->pStatus->ReacquireCount = 0;
  ((BTHD_7550_P_Handle *)(h->pImpl))->pAcquireParam->SoftwareWorkaroundMode = 0;
  BTHD_7550_P_Init_clk(h);
  BDBG_MSG(("BTHD_7550_P_Init \n"));
  retCode = BERR_SUCCESS;
  return retCode;
}


/******************************************************************************
BTHD_7550_P_Init() done
******************************************************************************/
void BTHD_7550_P_Init_clk(BTHD_Handle h)
{
  BTHD_7550_P_Handle *p7550 = (BTHD_7550_P_Handle *)(h->pImpl);

  /* reset all THD status to prevent false lock condition */
  BTHD_7550_P_WriteField(p7550->hRegister, THD_CORE_RST, RS_ERC_RST, 1 );	           /* Reset FEC error counters*/
  BTHD_7550_P_WriteField(p7550->hRegister, THD_CORE_RST, RS_ERC_RST, 0 );  

}

/******************************************************************************
BTHD_7550_P_GetLockStateChangeEvent()
******************************************************************************/
BERR_Code BTHD_7550_P_GetLockStateChangeEvent(BTHD_Handle h, BKNI_EventHandle* hEvent)
{
  *hEvent = ((BTHD_7550_P_Handle *)(h->pImpl))->hLockEvent;	
  return BERR_SUCCESS;
}

/******************************************************************************
BTHD_7550_P_GetInterruptEventHandle()
******************************************************************************/
BERR_Code BTHD_7550_P_GetInterruptEventHandle(BTHD_Handle h, BKNI_EventHandle* hEvent)
{
  *hEvent = ((BTHD_7550_P_Handle *)(h->pImpl))->hInterruptEvent;
  return BERR_SUCCESS;

}
/******************************************************************************
BTHD_7550_P_GetEWSEventHandle()
******************************************************************************/
BERR_Code BTHD_7550_P_GetEWSEventHandle(BTHD_Handle h, BKNI_EventHandle* hEvent)
{
  *hEvent = ((BTHD_7550_P_Handle *)(h->pImpl))->hEWSEvent;
  return BERR_SUCCESS;
}

/******************************************************************************
BTHD_7550_P_GetThdLockStatus()
******************************************************************************/
BERR_Code BTHD_7550_P_GetThdLockStatus(BTHD_Handle h, bool *pLockStatus)
{

  BTHD_7550_P_Handle *p7550 = (BTHD_7550_P_Handle *)(h->pImpl);
  *pLockStatus = (bool) ((p7550->ThdLockStatus >> THD_LockStatusBit_SystemLock) & 1);    
  return BERR_SUCCESS;	
}

/******************************************************************************
BTHD_7550_P_ProcessInterruptEvent()
******************************************************************************/
BERR_Code BTHD_7550_P_ProcessInterruptEvent(BTHD_Handle h)
{
  BTHD_7550_P_Handle *p7550 = (BTHD_7550_P_Handle *)(h->pImpl);
  static uint8_t EWS_Setting=0;
  static uint32_t TMCC_Err=0;
  uint32_t TMCC_SYNC_value, TMCC_Alert_value, TMCC_Err_update;
  static bool lock_status;


  lock_status = ((p7550->ThdLockStatus>> THD_LockStatusBit_SystemLock) & 1);
  BTHD_7550_P_Lock(h, 0);
  if ((p7550->pAcquireParam->Standard == THD_Standard_ISDBT) && ((p7550->ThdLockStatus>> THD_LockStatusBit_SystemLock) & 1))
  {
    /* EWS bit flips when the cable is not connected, so we need to read the error count to make sure the cable is connected before */
    /* reporting the EWS status event change */
    TMCC_SYNC_value = BTHD_7550_P_ReadField(p7550->hRegister, THD_CORE_STATUS, TMCC_SYNC);
    if (TMCC_SYNC_value)
    {
      TMCC_Err_update = BREG_Read32(p7550->hRegister, BCHP_THD_CORE_TMCC_UBERC);
      if (TMCC_Err == TMCC_Err_update)
      {
        TMCC_Alert_value = BTHD_7550_P_ReadField(p7550->hRegister, THD_CORE_TMCC_MISC, ALERT);
        /* If the channel is lock after tuning, set EWS event when EWS is on */
        if(p7550->pAcquireParam->EventMode & THD_Event_LockStartEWS)
        {
          if (1 == TMCC_Alert_value)
          {
            p7550->pAcquireParam->EventMode &= ~THD_Event_LockStartEWS;
            p7550->pAcquireParam->EventMode &= ~THD_Event_UnlockStopEWS;
            BKNI_SetEvent(p7550->hEWSEvent);
            EWS_Setting = TMCC_Alert_value;
            TMCC_Err = TMCC_Err_update;
          }
        }
        /* In other cases, set EWS event when EWS changes */
        else if (EWS_Setting != TMCC_Alert_value)
        {
          p7550->pAcquireParam->EventMode &= ~THD_Event_UnlockStopEWS;
          BKNI_SetEvent(p7550->hEWSEvent);
          EWS_Setting = TMCC_Alert_value;
          TMCC_Err = TMCC_Err_update;
        }
      }
    }
  }

  if (p7550->pAcquireParam->NexusStatusMode & THD_NexusStatusMode_EnableStatusForNexus)
  {
    BTHD_7550_P_Status(h);
    p7550->pAcquireParam->NexusStatusMode &= (~THD_NexusStatusMode_EnableStatusForNexus);
  }

  if (p7550->pAcquireParam->AcquireStartMode & THD_AcquireStartMode_ResetStatus) {
    p7550->pAcquireParam->AcquireStartMode &= ~THD_AcquireStartMode_ResetStatus;
    BDBG_MSG(("BTHD_7550_P_AcquireThd: Reset status\n"));
    BTHD_7550_P_ResetStatusHW(h);
    BTHD_7550_P_ResetStatus(h);
    p7550->pStatus->ReacquireCount = 0;
  }

  if (p7550->pAcquireParam->AcquireStartMode & THD_AcquireStartMode)
  {
    p7550->pAcquireParam->AcquireStartMode &= ~THD_AcquireStartMode;
		BTMR_StopTimer(p7550->hTimer);
    p7550->ThdLockStatus = 0;
		BTHD_7550_P_ResetThdEvent(h);
    p7550->pStatus->ReacquireCount = 0;
		BTHD_7550_P_ThdTop(h);
    return BERR_SUCCESS;
  }
  else
  {
    if (p7550->pAcquireParam->AcquireStartMode & (THD_AcquireStartMode_Auto | THD_AcquireStartMode_Scan))
    {
      if ((p7550->ThdLockStatus>> THD_LockStatusBit_SystemLock) & 1)
      {
        BDBG_MSG(("Still locked\n"));
        if (lock_status != ((p7550->ThdLockStatus>> THD_LockStatusBit_SystemLock) & 1))
          BKNI_SetEvent(p7550->hLockEvent);
				BTMR_StopTimer(p7550->hTimer);
				BTMR_StartTimer(p7550->hTimer, 25000);   /* the timer is in Micro second */
      }
      else
      {
        BDBG_MSG(("not locked\n"));
        p7550->ThdLockStatus = 0;
        if (p7550->pAcquireParam->EventMode & THD_Event_LockUpdate)
        {
          p7550->pAcquireParam->EventMode &= ~THD_Event_LockUpdate;
          BKNI_SetEvent(p7550->hLockEvent);
          /* If the channel is unlock, set EWS event if EWS has been started before. For the unlock channel after tuning , don't set EWS event */
          if (!(p7550->pAcquireParam->EventMode & THD_Event_UnlockStopEWS) && EWS_Setting == 1)
          {
            BKNI_SetEvent(p7550->hEWSEvent);
          }
          EWS_Setting = 0;
        }
				BTHD_7550_P_ResetThdEvent(h);
				BTHD_7550_P_ThdTop(h);
			}
      }
		else
		{
			BTMR_StopTimer(p7550->hTimer);
			BTMR_StartTimer(p7550->hTimer, 25000);   /* the timer is in Micro second */
    }
  }
  return BERR_SUCCESS;
}

/******************************************************************************
BTHD_7550_P_TimerFunc()
******************************************************************************/
BERR_Code BTHD_7550_P_TimerFunc(void *myParam1, int myParam2)
{

  BTHD_Handle local_h = (BTHD_Handle)myParam1;
  BSTD_UNUSED(myParam2);
  BKNI_SetEvent(((BTHD_7550_P_Handle *)(local_h->pImpl))->hInterruptEvent); 
  return BERR_SUCCESS;
}

/***************************************************************************
* BTHD_7550_P_AppDVBTSettings()
***************************************************************************/
void BTHD_7550_P_AppDVBTSettings(BTHD_Handle h, const BTHD_InbandParams *pParams)
{
  BTHD_7550_P_Handle *p7550 = (BTHD_7550_P_Handle *)(h->pImpl);
  BDBG_ASSERT(h);
  BDBG_ASSERT(pParams);

  BDBG_MSG(("BTHD_7550_P_AppDVBTSettings"));
  switch (pParams->decodeMode)
  {
  case BTHD_Decode_Lp:
    p7550->pAcquireParam->PriorityMode  = THD_PriorityMode_Low;
    break;
  default:
    BDBG_WRN(("Unrecognized PriorityMode setting - defaulting to THD_PriorityMode_High"));
    /* fall through */
  case BTHD_Decode_Hp:
    p7550->pAcquireParam->PriorityMode  = THD_PriorityMode_High;
    break;
  }
  if (pParams->bTpsAcquire == false)
  {
    p7550->pAcquireParam->TPSMode = THD_TPSMode_Manual;

    switch (pParams->eModulation)
    {
    case BTHD_Modulation_eQpsk:
      p7550->pAcquireParam->Qam  = THD_Qam_Qpsk;
      break;
    case BTHD_Modulation_e16Qam:
      p7550->pAcquireParam->Qam  = THD_Qam_16Qam;
      break;
    default:
      BDBG_WRN(("Unrecognized modulaton setting - defaulting to THD_Qam_64Qam"));
      /* fall through */
    case BTHD_Modulation_e64Qam:
      p7550->pAcquireParam->Qam  = THD_Qam_64Qam;
      break;
    }

    switch (pParams->eCodeRateHP)
    {
    case BTHD_CodeRate_e2_3:
      p7550->pAcquireParam->CodeRateHP  = THD_CodeRate_2_3;
      break;
    case BTHD_CodeRate_e3_4:
      p7550->pAcquireParam->CodeRateHP  = THD_CodeRate_3_4;
      break;
    case BTHD_CodeRate_e5_6:
      p7550->pAcquireParam->CodeRateHP  = THD_CodeRate_5_6;
      break;
    case BTHD_CodeRate_e7_8:
      p7550->pAcquireParam->CodeRateHP  = THD_CodeRate_7_8;
      break;
    default:
      BDBG_WRN(("Unrecognized CodeRateHP setting - defaulting to BTHD_CodeRate_e1_2"));
      /* fall through */
    case BTHD_CodeRate_e1_2:
      p7550->pAcquireParam->CodeRateHP  = THD_CodeRate_1_2;
      break;
    }

    switch (pParams->eCodeRateLP)
    {
    case BTHD_CodeRate_e2_3:
      p7550->pAcquireParam->CodeRateLP  = THD_CodeRate_2_3;
      break;
    case BTHD_CodeRate_e3_4:
      p7550->pAcquireParam->CodeRateLP  = THD_CodeRate_3_4;
      break;
    case BTHD_CodeRate_e5_6:
      p7550->pAcquireParam->CodeRateLP  = THD_CodeRate_5_6;
      break;
    case BTHD_CodeRate_e7_8:
      p7550->pAcquireParam->CodeRateLP  = THD_CodeRate_7_8;
      break;
    default:
      BDBG_WRN(("Unrecognized CodeRateLP setting - defaulting to BTHD_CodeRate_e1_2"));
      /* fall through */
    case BTHD_CodeRate_e1_2:
      p7550->pAcquireParam->CodeRateLP  = THD_CodeRate_1_2;
      break;
    }


    switch (pParams->eHierarchy)
    {
    case BTHD_Hierarchy_1:
      p7550->pAcquireParam->Hierarchy  = THD_Hierarchy_1;
      break;
    case BTHD_Hierarchy_2:
      p7550->pAcquireParam->Hierarchy  = THD_Hierarchy_2;
      break;
    case BTHD_Hierarchy_4:
      p7550->pAcquireParam->Hierarchy  = THD_Hierarchy_4;
      break;
    default:
      BDBG_WRN(("Unrecognized Hierarchy setting - defaulting to THD_Hierarchy_None"));
      /* fall through */
    case BTHD_Hierarchy_0:
      p7550->pAcquireParam->Hierarchy  = THD_Hierarchy_None;
      break;
    }
  }
  else
    p7550->pAcquireParam->TPSMode = THD_TPSMode_Auto;

}
/***************************************************************************
* BTHD_7550_P_AppISDBTSettings()
***************************************************************************/
void BTHD_7550_P_AppISDBTSettings(BTHD_Handle h, const BTHD_InbandParams *pParams)
{
  BTHD_7550_P_Handle *p7550 = (BTHD_7550_P_Handle *)(h->pImpl);
  BDBG_ASSERT(h);
  BDBG_ASSERT(pParams);

  BDBG_MSG(("BTHD_7550_P_AppISDBTSettings"));
  if (pParams->bTmccAcquire == false)
  {
    p7550->pAcquireParam->ISDBT_TMCCMode = THD_TMCCMode_Manual;

    switch (pParams->bTmccAcquire)
    {
    case 0:
      p7550->pAcquireParam->ISDBT_PR  = THD_Pr_Disable;
      break;
    default:
      BDBG_WRN(("Unrecognized TmccAcquire setting - defaulting to THD_Pr_Enable"));
	   /* fall through */
    case 1:
      p7550->pAcquireParam->ISDBT_PR  = THD_Pr_Enable;
      break;
    }

    switch (pParams->eIsdbtAModulation)
    {
    case BTHD_Modulation_eQpsk:
      p7550->pAcquireParam->ISDBT_A_Qam  = THD_Qam_Qpsk;
      break;
    case BTHD_Modulation_e64Qam:
      p7550->pAcquireParam->ISDBT_A_Qam  = THD_Qam_64Qam;
      break;
    default:
      BDBG_WRN(("Unrecognized A modulaton setting - defaulting to THD_Qam_16Qam"));
      /* fall through */
    case BTHD_Modulation_e16Qam:
      p7550->pAcquireParam->ISDBT_A_Qam  = THD_Qam_16Qam;
      break;
    }

    switch (pParams->eIsdbtACodeRate)
    {
    case BTHD_CodeRate_e2_3:
      p7550->pAcquireParam->ISDBT_A_CodeRate  = THD_CodeRate_2_3;
      break;
    case BTHD_CodeRate_e3_4:
      p7550->pAcquireParam->ISDBT_A_CodeRate  = THD_CodeRate_3_4;
      break;
    case BTHD_CodeRate_e5_6:
      p7550->pAcquireParam->ISDBT_A_CodeRate  = THD_CodeRate_5_6;
      break;
    case BTHD_CodeRate_e7_8:
      p7550->pAcquireParam->ISDBT_A_CodeRate  = THD_CodeRate_7_8;
      break;
    default:
      BDBG_WRN(("Unrecognized ISDBT_A_CodeRate setting - defaulting to BTHD_CodeRate_e1_2"));
      /* fall through */
    case BTHD_CodeRate_e1_2:
      p7550->pAcquireParam->ISDBT_A_CodeRate  = THD_CodeRate_1_2;
      break;
    }

    switch (pParams->eIsdbtATimeInterleaving)
    {
    case BTHD_IsdbtTimeInterleaving_1X:
      p7550->pAcquireParam->ISDBT_A_TimeInt  = THD_TimeInt_1X;
      break;
    case BTHD_IsdbtTimeInterleaving_2X:
      p7550->pAcquireParam->ISDBT_A_TimeInt  = THD_TimeInt_2X;
      break;
    case BTHD_IsdbtTimeInterleaving_3X:
      p7550->pAcquireParam->ISDBT_A_TimeInt  = THD_TimeInt_4X;
      break;
    default:
      BDBG_WRN(("Unrecognized ISDBT_A_TimeInt setting - defaulting to THD_TimeInt_0X"));
      /* fall through */
    case BTHD_IsdbtTimeInterleaving_0X:
      p7550->pAcquireParam->ISDBT_A_TimeInt  = THD_TimeInt_0X;
      break;
    }
    p7550->pAcquireParam->ISDBT_A_Segments = pParams->eIsdbtASegments;

    /********************************** B ****************************/
    switch (pParams->eIsdbtBModulation)
    {
    case BTHD_Modulation_eQpsk:
      p7550->pAcquireParam->ISDBT_B_Qam  = THD_Qam_Qpsk;
      break;
    case BTHD_Modulation_e64Qam:
      p7550->pAcquireParam->ISDBT_B_Qam  = THD_Qam_64Qam;
      break;
    default:
      BDBG_WRN(("Unrecognized B modulaton setting - defaulting to THD_Qam_16Qam"));
      /* fall through */
    case BTHD_Modulation_e16Qam:
      p7550->pAcquireParam->ISDBT_B_Qam  = THD_Qam_16Qam;
      break;
    }

    switch (pParams->eIsdbtBCodeRate)
    {
    case BTHD_CodeRate_e2_3:
      p7550->pAcquireParam->ISDBT_B_CodeRate  = THD_CodeRate_2_3;
      break;
    case BTHD_CodeRate_e3_4:
      p7550->pAcquireParam->ISDBT_B_CodeRate  = THD_CodeRate_3_4;
      break;
    case BTHD_CodeRate_e5_6:
      p7550->pAcquireParam->ISDBT_B_CodeRate  = THD_CodeRate_5_6;
      break;
    case BTHD_CodeRate_e7_8:
      p7550->pAcquireParam->ISDBT_B_CodeRate  = THD_CodeRate_7_8;
      break;
    default:
      BDBG_WRN(("Unrecognized ISDBT_B_CodeRate setting - defaulting to BTHD_CodeRate_e1_2"));
      /* fall through */
    case BTHD_CodeRate_e1_2:
      p7550->pAcquireParam->ISDBT_B_CodeRate  = THD_CodeRate_1_2;
      break;
    }

    switch (pParams->eIsdbtBTimeInterleaving)
    {
    case BTHD_IsdbtTimeInterleaving_1X:
      p7550->pAcquireParam->ISDBT_B_TimeInt  = THD_TimeInt_1X;
      break;
    case BTHD_IsdbtTimeInterleaving_2X:
      p7550->pAcquireParam->ISDBT_B_TimeInt  = THD_TimeInt_2X;
      break;
    case BTHD_IsdbtTimeInterleaving_3X:
      p7550->pAcquireParam->ISDBT_B_TimeInt  = THD_TimeInt_4X;
      break;
    default:
      BDBG_WRN(("Unrecognized ISDBT_B_TimeInt setting - defaulting to THD_TimeInt_0X"));
      /* fall through */
    case BTHD_IsdbtTimeInterleaving_0X:
      p7550->pAcquireParam->ISDBT_B_TimeInt  = THD_TimeInt_0X;
      break;
    }

    p7550->pAcquireParam->ISDBT_B_Segments = pParams->eIsdbtBSegments;
    /********************************** C ****************************/
    switch (pParams->eIsdbtCModulation)
    {
    case BTHD_Modulation_eQpsk:
      p7550->pAcquireParam->ISDBT_C_Qam  = THD_Qam_Qpsk;
      break;
    case BTHD_Modulation_e64Qam:
      p7550->pAcquireParam->ISDBT_C_Qam  = THD_Qam_64Qam;
      break;
    default:
      BDBG_WRN(("Unrecognized C modulaton setting - defaulting to THD_Qam_16Qam"));
      /* fall through */
    case BTHD_Modulation_e16Qam:
      p7550->pAcquireParam->ISDBT_C_Qam  = THD_Qam_16Qam;
      break;
    }

    switch (pParams->eIsdbtCCodeRate)
    {
    case BTHD_CodeRate_e2_3:
      p7550->pAcquireParam->ISDBT_C_CodeRate  = THD_CodeRate_2_3;
      break;
    case BTHD_CodeRate_e3_4:
      p7550->pAcquireParam->ISDBT_C_CodeRate  = THD_CodeRate_3_4;
      break;
    case BTHD_CodeRate_e5_6:
      p7550->pAcquireParam->ISDBT_C_CodeRate  = THD_CodeRate_5_6;
      break;
    case BTHD_CodeRate_e7_8:
      p7550->pAcquireParam->ISDBT_C_CodeRate  = THD_CodeRate_7_8;
      break;
    default:
      BDBG_WRN(("Unrecognized ISDBT_C_CodeRate setting - defaulting to BTHD_CodeRate_e1_2"));
      /* fall through */
    case BTHD_CodeRate_e1_2:
      p7550->pAcquireParam->ISDBT_C_CodeRate  = THD_CodeRate_1_2;
      break;
    }

    switch (pParams->eIsdbtCTimeInterleaving)
    {
    case BTHD_IsdbtTimeInterleaving_1X:
      p7550->pAcquireParam->ISDBT_C_TimeInt  = THD_TimeInt_1X;
      break;
    case BTHD_IsdbtTimeInterleaving_2X:
      p7550->pAcquireParam->ISDBT_C_TimeInt  = THD_TimeInt_2X;
      break;
    case BTHD_IsdbtTimeInterleaving_3X:
      p7550->pAcquireParam->ISDBT_C_TimeInt  = THD_TimeInt_4X;
      break;
    default:
      BDBG_WRN(("Unrecognized ISDBT_C_TimeInt setting - defaulting to THD_TimeInt_0X"));
      /* fall through */
    case BTHD_IsdbtTimeInterleaving_0X:
      p7550->pAcquireParam->ISDBT_C_TimeInt  = THD_TimeInt_0X;
      break;
    }
    p7550->pAcquireParam->ISDBT_C_Segments = pParams->eIsdbtCSegments;
  }
  else
    p7550->pAcquireParam->ISDBT_TMCCMode = THD_TMCCMode_Auto;
}

/***************************************************************************
* BTHD_7550_P_TuneAcquire()
***************************************************************************/
BERR_Code BTHD_7550_P_TuneAcquire(BTHD_Handle h, const BTHD_InbandParams *pParams)
{   
  BTHD_7550_P_Handle *p7550 = (BTHD_7550_P_Handle *)(h->pImpl);
  uint32_t Prev_val;
  BERR_Code retCode = BERR_SUCCESS;
  BDBG_ASSERT(h);
  BDBG_ASSERT(pParams);

  BTMR_StopTimer(p7550->hTimer);
  Prev_val = p7550->pAcquireParam->SoftwareWorkaroundMode;
  BKNI_Memset( p7550->pStatus, 0x00, sizeof( BTHD_P_7550_THDStatus ) ); /* Clear status structure upon new TuneAcquire command from host */
  p7550->pAcquireParam->SoftwareWorkaroundMode = Prev_val;

  BDBG_MSG(("Input to TuneAcquire: Mode=%d, DecodeMode=%d, Cci=%d, iffreq=%d, bw=%d, bTPSAcq=%d, eModeGaurdAcq=%d, \n eAcquisitionMode=%d, TmccAcq=%d, Tunerfreq=%d, PullInRange =%d", pParams->mode, pParams->decodeMode, pParams->cciMode, pParams->ifFreq, pParams->bandwidth, pParams->bTpsAcquire, pParams->eModeGuardAcquire, pParams->eAcquisitionMode, pParams->bTmccAcquire, pParams->tunerFreq, pParams->ePullinRange));
	
  /* Clear and set Acquisition Start Mode bits */
  p7550->pAcquireParam->AcquireStartMode &= (~THD_AcquireStartMode_Auto & ~THD_AcquireStartMode_Scan & ~THD_AcquireStartMode);  
  switch ( pParams->eAcquisitionMode )
  {
  case BTHD_ThdAcquisitionMode_eManual:
    break;
  case BTHD_ThdAcquisitionMode_eScan:
    p7550->pAcquireParam->AcquireStartMode  |= THD_AcquireStartMode_Scan;
    break;
  default:
    BDBG_WRN(("Unrecognized mode setting - defaulting to THD_AcquireStartMode_Auto"));
    /* fall through */
  case BTHD_ThdAcquisitionMode_eAuto:
    p7550->pAcquireParam->AcquireStartMode  |= THD_AcquireStartMode_Auto;
    break;
  }
 
  switch ( pParams->mode )
  {
  case BTHD_InbandMode_eIsdbt:
    p7550->pAcquireParam->Standard  = THD_Standard_ISDBT;
    break;
  default:
    BDBG_WRN(("Unrecognized mode setting - defaulting to THD_Standard_DVBT"));
    /* fall through */
  case BTHD_InbandMode_eDvbt:
    p7550->pAcquireParam->Standard  = THD_Standard_DVBT;
    break;
  }

  switch ( pParams->bandwidth )
  {
  case BTHD_Bandwidth_5Mhz:
    p7550->pAcquireParam->Bandwidth  = THD_Bandwidth_5MHz;
    break;
  case BTHD_Bandwidth_6Mhz:
    p7550->pAcquireParam->Bandwidth  = THD_Bandwidth_6MHz;
    break;
  case BTHD_Bandwidth_7Mhz:
    p7550->pAcquireParam->Bandwidth  = THD_Bandwidth_7MHz;
    break;
  default:
    BDBG_WRN(("Unrecognized bandwidth setting - defaulting to THD_Bandwidth_8MHz"));
    /* fall through */
  case BTHD_Bandwidth_8Mhz:
    p7550->pAcquireParam->Bandwidth  = THD_Bandwidth_8MHz;
    break;
  }
  p7550->pAcquireParam->RfFreq         = pParams->tunerFreq;
  p7550->pAcquireParam->CenterFreq     = pParams->ifFreq;
  switch (pParams->cciMode)
  {
  case BTHD_CCI_None:
    p7550->pAcquireParam->CoChannelMode  = THD_CoChannelMode_None;
    break;
  default:
    BDBG_WRN(("Unrecognized CoChannelMode setting - defaulting to THD_CoChannelMode_Auto"));
    /* fall through */
  case BTHD_CCI_Auto:
    p7550->pAcquireParam->CoChannelMode  = THD_CoChannelMode_Auto;
    break;
  }

  switch (pParams->ePullinRange)
  {
  case BTHD_PullInRange_eNarrow:
    p7550->pAcquireParam->CarrierRange  = THD_CarrierRange_Narrow;
    break;
  default:
    BDBG_WRN(("Unrecognized CarrierRange setting - defaulting to THD_CarrierRange_Wide"));
    /* fall through */
  case BTHD_PullInRange_eWide:
    p7550->pAcquireParam->CarrierRange  = THD_CarrierRange_Wide;
    break;
  }


  switch (pParams->eModeGuardAcquire)
  {
    
	 case BTHD_ModeGuard_eManual:
		 p7550->pAcquireParam->TransGuardMode = THD_TransGuardMode_Manual;
		switch (pParams->eTransmissionMode)
		{
			case BTHD_TransmissionMode_e4K:
				p7550->pAcquireParam->TransmissionMode  = THD_TransmissionMode_4k;
				break;
			case BTHD_TransmissionMode_e8K:
				p7550->pAcquireParam->TransmissionMode  = THD_TransmissionMode_8k;
				break;
			default:
				BDBG_WRN(("Unrecognized TransmissionMode setting - defaulting to BTHD_TransmissionMode_e2K"));
				/* fall through */
			case BTHD_TransmissionMode_e2K:
				p7550->pAcquireParam->TransmissionMode  = THD_TransmissionMode_2k;
				break;
		}

		switch (pParams->eGuardInterval)
		{
			case BTHD_GuardInterval_e1_32:
				p7550->pAcquireParam->GuardInterval  = THD_GuardInterval_1_32;
				break;
			case BTHD_GuardInterval_e1_16:
				p7550->pAcquireParam->GuardInterval  = THD_GuardInterval_1_16;
				break;
			case BTHD_GuardInterval_e1_8:
				p7550->pAcquireParam->GuardInterval  = THD_GuardInterval_1_8;
				break;
			default:
				BDBG_WRN(("Unrecognized GuardInterval setting - defaulting to THD_GuardInterval_1_4"));
				/* fall through */
			case BTHD_GuardInterval_e1_4:
				p7550->pAcquireParam->GuardInterval  = THD_GuardInterval_1_4;
				break;
		}
		break;
	 case BTHD_ModeGuard_eAutoDvbt:
		p7550->pAcquireParam->TransGuardMode  = THD_TransGuardMode_Auto_DVBT;
		break;
	 case BTHD_ModeGuard_eAutoIsdbt:
		p7550->pAcquireParam->TransGuardMode  = THD_TransGuardMode_Auto_ISDBT;
		break;
	 default:
		BDBG_WRN(("Unrecognized TransGuardMode setting - defaulting to THD_TransGuardMode_Auto"));
		/* fall through */
	 case BTHD_ModeGuard_eAuto:
		p7550->pAcquireParam->TransGuardMode  = THD_TransGuardMode_Auto;
		break;
	}


  if (p7550->pAcquireParam->Standard  == THD_Standard_DVBT)
  {
    BDBG_MSG(("BTHD_7550_P_TuneAcquire = THD_Standard_DVBT"));
    BTHD_7550_P_AppDVBTSettings(h, pParams);
  }
  else
  {
    BDBG_MSG(("BTHD_7550_P_TuneAcquire = THD_Standard_ISDBT"));
    BTHD_7550_P_AppISDBTSettings(h, pParams);
  }

  /*     Inetrnal Settings    */

  p7550->pAcquireParam->FrontEndMode         = THD_FrontEndMode_Baseband;
  p7550->pAcquireParam->ChannelEstimatorMode = THD_ChannelEstimatorMode_Auto;
  p7550->pAcquireParam->FFTWindowMode        = THD_FFTWindowMode_Auto;
  p7550->pAcquireParam->ImpulseMode          = THD_ImpulseMode_None; 
  p7550->pAcquireParam->TSMode               = THD_TSMode_Serial;
  p7550->pAcquireParam->TransGuardMaxThreshold            = 0x00050000;
  p7550->pAcquireParam->TransGuardMaxMinRatioThreshold[0] = 296;
  p7550->pAcquireParam->TransGuardMaxMinRatioThreshold[1] = 210;
  p7550->pAcquireParam->TransGuardMaxMinRatioThreshold[2] = 150;
  p7550->pAcquireParam->TransGuardMaxMinRatioThreshold[3] = 104;
  p7550->pAcquireParam->SampleFreq						= 54000000;
	/* p7550->pAcquireParam->AcquireStartMode			       |= THD_AcquireStartMode_Auto; */

  /*
  BDBG_MSG(("BTHD_7550_P_TuneAcquire:\tStandard             = %d",p7550->pAcquireParam->Standard));
  BDBG_MSG(("BTHD_7550_P_TuneAcquire:\tFrontEndMode         = %d",p7550->pAcquireParam->FrontEndMode));
  BDBG_MSG(("BTHD_7550_P_TuneAcquire:\tChannelEstimatorMode = %d",p7550->pAcquireParam->ChannelEstimatorMode));
  BDBG_MSG(("BTHD_7550_P_TuneAcquire:\tFFTWindowMode        = %d",p7550->pAcquireParam->FFTWindowMode));
  BDBG_MSG(("BTHD_7550_P_TuneAcquire:\tCoChannelMode        = %d",p7550->pAcquireParam->CoChannelMode));
  BDBG_MSG(("BTHD_7550_P_TuneAcquire:\tImpulseMode          = %d",p7550->pAcquireParam->ImpulseMode));
  BDBG_MSG(("BTHD_7550_P_TuneAcquire:\tTransGuardMode       = %d",p7550->pAcquireParam->TransGuardMode));
  BDBG_MSG(("BTHD_7550_P_TuneAcquire:\tTPSMode              = %d",p7550->pAcquireParam->TPSMode));
  BDBG_MSG(("BTHD_7550_P_TuneAcquire:\tCarrierRange         = %d",p7550->pAcquireParam->CarrierRange));
  BDBG_MSG(("BTHD_7550_P_TuneAcquire:\tTSMode               = %d",p7550->pAcquireParam->TSMode));
  BDBG_MSG(("BTHD_7550_P_TuneAcquire:\tPriorityMode         = %d",p7550->pAcquireParam->PriorityMode));
  BDBG_MSG(("BTHD_7550_P_TuneAcquire:\tTransmissionMode     = %d",p7550->pAcquireParam->TransmissionMode));
  BDBG_MSG(("BTHD_7550_P_TuneAcquire:\tGuardInterval        = %d",p7550->pAcquireParam->GuardInterval));
  BDBG_MSG(("BTHD_7550_P_TuneAcquire:\tQam                  = %d",p7550->pAcquireParam->Qam));
  BDBG_MSG(("BTHD_7550_P_TuneAcquire:\tHierarchy            = %d",p7550->pAcquireParam->Hierarchy));
  BDBG_MSG(("BTHD_7550_P_TuneAcquire:\tCodeRateHP           = %d",p7550->pAcquireParam->CodeRateHP));
  BDBG_MSG(("BTHD_7550_P_TuneAcquire:\tCodeRateLP           = %d",p7550->pAcquireParam->CodeRateLP));
  BDBG_MSG(("BTHD_7550_P_TuneAcquire:\tBandwidth            = %d",p7550->pAcquireParam->Bandwidth));
  BDBG_MSG(("BTHD_7550_P_TuneAcquire:\tCenterFreq           = %d",p7550->pAcquireParam->CenterFreq));
  BDBG_MSG(("BTHD_7550_P_TuneAcquire:\tSampleFreq           = %d",p7550->pAcquireParam->SampleFreq));
  BDBG_MSG(("BTHD_7550_P_TuneAcquire:\tRfFreq               = %d",p7550->pAcquireParam->RfFreq)); 
  */
  BTHD_CHK_RETCODE(BTHD_7550_P_AcquireThd(h));

done:
  return retCode;
}

void BTHD_7550_P_ISDBT_Softwareworkaround (BTHD_Handle h)
{

  BTHD_7550_P_Handle *p7550 = (BTHD_7550_P_Handle *)(h->pImpl);

  BDBG_MSG(("BTHD_7550_P_ISDBT_Softwareworkaround"));  

  /* Initialize memory locations between 0x000 - 0x23F to 'FF'.  Avoid overwriting
  MIPS 12-byte exception vectors at 0x000, 0x180, and 0x200. */
#if 0
  uint32_t i;

  for (i=0x00C; i< 0x180; i=i+4)
    *((volatile unsigned long *)(0xa0000000 + i)) = (unsigned long)0xffffffff;

  for (i=0x18C; i< 0x200; i=i+4)
    *((volatile unsigned long *)(0xa0000000 + i)) = (unsigned long)0xffffffff;

  for (i=0x20C; i< 0x240; i=i+4)
    *((volatile unsigned long *)(0xa0000000 + i)) = (unsigned long)0xffffffff;

#endif

  BTHD_7550_P_WriteField(p7550->hRegister, MEMC_GEN_0_ARC_0_CNTRL, WRITE_CMD_ABORT, 1);  
  BTHD_7550_P_WriteField(p7550->hRegister, MEMC_GEN_0_ARC_0_CNTRL, WRITE_CHECK, 1);
  BTHD_7550_P_WriteField(p7550->hRegister, MEMC_GEN_0_ARC_0_CNTRL, MODE, 0); /* Non-exclusive:  client could have access to outside the defined address range */

  BREG_Write32(p7550->hRegister, BCHP_MEMC_GEN_0_ARC_0_ADRS_RANGE_HIGH, 0x1); 
  BREG_Write32(p7550->hRegister, BCHP_MEMC_GEN_0_ARC_0_ADRS_RANGE_LOW, 0x0); 

  BREG_Write32(p7550->hRegister, BCHP_MEMC_GEN_0_ARC_0_WRITE_RIGHTS_0, 0xFFDFFFFF); 
  BREG_Write32(p7550->hRegister, BCHP_MEMC_GEN_0_ARC_0_WRITE_RIGHTS_1, 0xFFFFFFFF); 
  BREG_Write32(p7550->hRegister, BCHP_MEMC_GEN_0_ARC_0_WRITE_RIGHTS_2, 0xFFFFFFFF);
  BREG_Write32(p7550->hRegister, BCHP_MEMC_GEN_0_ARC_0_WRITE_RIGHTS_3, 0xFFFFFFFF);

  /* ARC 1 */
  BTHD_7550_P_WriteField(p7550->hRegister, MEMC_GEN_0_ARC_1_CNTRL, WRITE_CMD_ABORT, 1);  
  BTHD_7550_P_WriteField(p7550->hRegister, MEMC_GEN_0_ARC_1_CNTRL, WRITE_CHECK, 1);
  BTHD_7550_P_WriteField(p7550->hRegister, MEMC_GEN_0_ARC_1_CNTRL, MODE, 0); /* Non-exclusive:  client could have access to outside the defined address range */

  BREG_Write32(p7550->hRegister, BCHP_MEMC_GEN_0_ARC_1_ADRS_RANGE_HIGH, 0x31); 
  BREG_Write32(p7550->hRegister, BCHP_MEMC_GEN_0_ARC_1_ADRS_RANGE_LOW, 0x30); 

  BREG_Write32(p7550->hRegister, BCHP_MEMC_GEN_0_ARC_1_WRITE_RIGHTS_0, 0xFFDFFFFF); 
  BREG_Write32(p7550->hRegister, BCHP_MEMC_GEN_0_ARC_1_WRITE_RIGHTS_1, 0xFFFFFFFF); 
  BREG_Write32(p7550->hRegister, BCHP_MEMC_GEN_0_ARC_1_WRITE_RIGHTS_2, 0xFFFFFFFF);
  BREG_Write32(p7550->hRegister, BCHP_MEMC_GEN_0_ARC_1_WRITE_RIGHTS_3, 0xFFFFFFFF);


  /* ARC 2 */
  BTHD_7550_P_WriteField(p7550->hRegister, MEMC_GEN_0_ARC_2_CNTRL, WRITE_CMD_ABORT, 1);  
  BTHD_7550_P_WriteField(p7550->hRegister, MEMC_GEN_0_ARC_2_CNTRL, WRITE_CHECK, 1);
  BTHD_7550_P_WriteField(p7550->hRegister, MEMC_GEN_0_ARC_2_CNTRL, MODE, 0); /* Non-exclusive:  client could have access to outside the defined address range */

  BREG_Write32(p7550->hRegister, BCHP_MEMC_GEN_0_ARC_2_ADRS_RANGE_HIGH, 0x41); 
  BREG_Write32(p7550->hRegister, BCHP_MEMC_GEN_0_ARC_2_ADRS_RANGE_LOW, 0x40); 

  BREG_Write32(p7550->hRegister, BCHP_MEMC_GEN_0_ARC_2_WRITE_RIGHTS_0, 0xFFDFFFFF); 
  BREG_Write32(p7550->hRegister, BCHP_MEMC_GEN_0_ARC_2_WRITE_RIGHTS_1, 0xFFFFFFFF); 
  BREG_Write32(p7550->hRegister, BCHP_MEMC_GEN_0_ARC_2_WRITE_RIGHTS_2, 0xFFFFFFFF);
  BREG_Write32(p7550->hRegister, BCHP_MEMC_GEN_0_ARC_2_WRITE_RIGHTS_3, 0xFFFFFFFF);


}

