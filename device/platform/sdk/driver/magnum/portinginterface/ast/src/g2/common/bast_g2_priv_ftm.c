/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bast_g2_priv_ftm.c $
 * $brcm_Revision: Hydra_Software_Devel/75 $
 * $brcm_Date: 9/6/12 6:15p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ast/src/g2/common/bast_g2_priv_ftm.c $
 * 
 * Hydra_Software_Devel/75   9/6/12 6:15p ronchan
 * SWSATFE-214: rename 73xx to g2
 * 
 * Hydra_Software_Devel/74   8/14/12 9:59a ronchan
 * SWSATFE-213: exit critical section if error condition occurs
 * 
 * Hydra_Software_Devel/73   7/10/12 10:26a ronchan
 * SWSATFE-206: copy crc byte when holding nack error message
 * 
 * Hydra_Software_Devel/72   5/29/12 3:25p ronchan
 * SWSATFE-199: additional preamble byte per DirecTV request
 * 
 * Hydra_Software_Devel/71   4/5/12 4:57p ronchan
 * SWSATFE-22: insert length byte when forwarding bad crc packet
 * 
 * Hydra_Software_Devel/70   4/3/12 2:40p ronchan
 * HW7335-383: added verification for timer interrupts
 * 
 * Hydra_Software_Devel/69   11/9/11 3:20p ronchan
 * SWSATFE-75: disable coef memory auto increment after write
 * 
 * Hydra_Software_Devel/68   6/11/11 5:08p ronchan
 * SWSATFE-125: pad tx fifo if empty during transmit
 * 
 * Hydra_Software_Devel/67   8/9/10 2:56p ronchan
 * SWSATFE-70: wait additional 150ms for reg response if request
 * transmitted just before timing out
 * 
 * Hydra_Software_Devel/66   8/5/10 3:12p ronchan
 * SWSATFE-70: Wait until the last transmit completes before reporting reg
 * time out
 * 
 * Hydra_Software_Devel/65   6/17/10 9:57a ronchan
 * SWSATFE-63: enable SWM option bits for No-OS and Soap
 * 
 * Hydra_Software_Devel/64   6/1/10 2:49p ronchan
 * SWSATFE-60: added preliminary sliding window settings but disabled it
 * 
 * Hydra_Software_Devel/63   3/19/10 4:26p ronchan
 * SW7325-699: implement for TX watchdog timer
 * 
 * Hydra_Software_Devel/62   3/9/10 4:17p ronchan
 * SW7325-699: reset ftm after hard reset
 * 
 * Hydra_Software_Devel/61   1/21/10 11:12a ronchan
 * SWSATFE-22: updated error handling during registration and xtune
 * 
 * Hydra_Software_Devel/60   12/9/09 5:53p ronchan
 * SWSATFE-22: discard nack crc broadcast if not registering
 * 
 * Hydra_Software_Devel/59   12/9/09 3:32p ronchan
 * SWSATFE-22: drop hard reset request if FTM UP packet received
 * 
 * Hydra_Software_Devel/58   12/9/09 2:47p ronchan
 * SWSATFE-22: added preamble detect option bit
 * 
 * Hydra_Software_Devel/57   12/9/09 11:04a ronchan
 * SWSATFE-18: clear start TX flag only on registration timeout
 * 
 * Hydra_Software_Devel/56   11/23/09 5:16p ronchan
 * SWSATFE-18: clear all TX busy flags on terminate registration
 * 
 * Hydra_Software_Devel/55   11/23/09 4:52p ronchan
 * SWSATFE-19: suppress bad packet message if no pending packet
 * 
 * Hydra_Software_Devel/54   11/23/09 4:32p ronchan
 * SWSATFE-18: clear start TX flag on terminate registration
 * 
 * Hydra_Software_Devel/53   11/23/09 2:10p ronchan
 * SWSATFE-14: filter unexpected xtune responses
 * 
 * Hydra_Software_Devel/52   11/23/09 10:51a ronchan
 * SWSATFE-14: hold rx buffer until read by host
 * 
 * Hydra_Software_Devel/51   10/16/09 5:37p ronchan
 * HW7335-383: updated hand-off interface settings
 * 
 * Hydra_Software_Devel/50   10/13/09 5:01p ronchan
 * HW7335-383: disable fskphy for loopback
 * 
 * Hydra_Software_Devel/49   9/24/09 4:30p ronchan
 * HW7335-383: use diff osc only for 7335
 * 
 * Hydra_Software_Devel/48   9/24/09 11:24a ronchan
 * HW7335-383: fixed compile warning
 * 
 * Hydra_Software_Devel/47   9/22/09 4:56p ronchan
 * HW7335-383: verify ftm interrupts on initialization
 * 
 * Hydra_Software_Devel/46   8/6/09 3:02p ronchan
 * PR 55860: clear ftm interrupts after disabling
 * 
 * Hydra_Software_Devel/45   8/6/09 2:51p ronchan
 * PR 54943: return busy error if SWM hard reset pending
 * 
 * Hydra_Software_Devel/44   8/4/09 3:57p ronchan
 * PR 57350: prevent rx bit mask truncation when passed as bool
 * 
 * Hydra_Software_Devel/43   6/2/09 4:52p ronchan
 * PR 55650: implement recommended FTM peak validation settings
 * 
 * Hydra_Software_Devel/42   5/28/09 11:18a ronchan
 * PR 55538: softened assertion to stop HW assist during interrupt
 * 
 * Hydra_Software_Devel/41   4/27/09 3:16p ronchan
 * PR 54581: added flag to indicate if FTM core is powered down
 * 
 * Hydra_Software_Devel/40   4/13/09 2:11p ronchan
 * PR 54006: renamed EnableInterrupt to EnableInterrupt_isr
 * 
 * Hydra_Software_Devel/39   4/10/09 3:24p ronchan
 * PR 54006: created DoWhenHwAssistIdle_isr() for ISR context
 * 
 * Hydra_Software_Devel/38   3/9/09 2:04p ronchan
 * PR 51177: power down FSK PHY only if non-analog diseqc or all diseqcs
 * powered down
 * 
 * Hydra_Software_Devel/37   2/18/09 2:47p ronchan
 * PR 52262: added debug function traces to main api calls
 * 
 * Hydra_Software_Devel/36   2/17/09 4:10p ronchan
 * PR 37769: removed function to determine if ftm is busy
 * 
 * Hydra_Software_Devel/35   2/13/09 10:40a ronchan
 * PR 52103: re-added non-polled mode per Thomson request
 * 
 * Hydra_Software_Devel/34   2/12/09 11:36a ronchan
 * PR 52055: remove poll processing logic
 * 
 * Hydra_Software_Devel/33   2/12/09 11:21a ronchan
 * PR 52054: remove non-polled mode and send raw mode
 * 
 * Hydra_Software_Devel/32   2/12/09 10:48a ronchan
 * PR 52049: added rx bit mask filter and poll filter
 * 
 * Hydra_Software_Devel/31   1/28/09 3:06p ronchan
 * PR 51675: power down unused FSKPHY cores
 * 
 * Hydra_Software_Devel/30   1/18/09 2:27p ronchan
 * PR 37881: fixed comments
 * 
 * Hydra_Software_Devel/28   12/13/08 2:07p ronchan
 * PR 50348: reworked ftm reset for interrupt and non-interrupt context
 * 
 * Hydra_Software_Devel/27   11/21/08 4:51p ronchan
 * PR 49603: changed EnableFtmInterrupts() to be interrupt safe
 * 
 * Hydra_Software_Devel/26   11/18/08 9:07a ronchan
 * PR 37881: configure tx level for fsk channel 1
 * 
 * Hydra_Software_Devel/25   11/12/08 2:30p ronchan
 * PR 37769: disable hw assist, disable receiver, reset state when
 * powering down ftm
 * 
 * Hydra_Software_Devel/24   10/10/08 10:32a ronchan
 * PR 37769: always enable DAC for TX channel
 * 
 * Hydra_Software_Devel/23   9/29/08 3:05p ronchan
 * PR 37769: added function to determine if ftm is busy
 * 
 * Hydra_Software_Devel/22   9/22/08 2:51p ronchan
 * PR 37769: added new fsk channel configurations
 * 
 * Hydra_Software_Devel/21   9/10/08 5:20p ronchan
 * PR 37769: protect hard reset from requests to send packet
 * 
 * Hydra_Software_Devel/20   8/19/08 10:54a ronchan
 * PR 37769: return error message when xtune request times out
 * 
 * Hydra_Software_Devel/19   8/5/08 4:44p ronchan
 * PR 37881: removed '//' style comments
 * 
 * Hydra_Software_Devel/18   7/8/08 3:52p ronchan
 * PR 37881: added config parameters for tx power & channel select
 * 
 * Hydra_Software_Devel/17   6/26/08 2:39p ronchan
 * PR 37769: enable auto rx disable during tx
 * 
 * Hydra_Software_Devel/16   6/20/08 4:52p ronchan
 * PR 37769: auto insert crc for ping response
 * 
 * Hydra_Software_Devel/14   6/6/08 4:27p ronchan
 * PR 37769: modified xtune retransmit
 * 
 * Hydra_Software_Devel/13   5/29/08 4:17p ronchan
 * PR 37769: removed unused scripts
 * 
 * Hydra_Software_Devel/12   5/27/08 3:31p ronchan
 * PR 37769: added options to compile out powerdown functions
 * 
 * Hydra_Software_Devel/11   5/16/08 8:27a ronchan
 * PR 37769: updated ftm network error handling
 * 
 * Hydra_Software_Devel/10   5/12/08 6:09p ronchan
 * PR 37769: turn off crc insertion by default, fixed extended version
 * response, fixed crc check
 * 
 * Hydra_Software_Devel/9   4/29/08 4:34p ronchan
 * PR 37769: implemented ftm power down, power up
 * 
 * Hydra_Software_Devel/8   4/28/08 2:44p ronchan
 * PR 37881: check return code when enabling or disabling interrupts
 * 
 * Hydra_Software_Devel/7   2/19/08 6:30p jrubio
 * PR38507: fix compile false warnings
 * 
 * Hydra_Software_Devel/6   1/22/08 3:45p ronchan
 * PR 37769: added non-polled mode and send raw mode
 * 
 * Hydra_Software_Devel/5   1/18/08 2:28p ronchan
 * PR 37769: fixed bug when rx bit mask change, changed callback functions
 * to callback_isr, added extended version command, added insert crc
 * option, fixed promiscuous mode
 * 
 * Hydra_Software_Devel/4   12/10/07 9:36a ronchan
 * PR 37769: fixed bad crc handling
 * 
 * Hydra_Software_Devel/2   11/30/07 9:34a ronchan
 * PR 37769: removed uart status check when manually transmitting
 * registration request
 * 
 * Hydra_Software_Devel/1   11/21/07 11:28a ronchan
 * PR 37769: initial version
 *
 ***************************************************************************/
 
#ifndef BAST_ENABLE_SKIT_FSK
 
#include "bstd.h"
#include "bast.h"
#include "bast_priv.h"
#include "bast_g2_priv.h"
#include "bast_g2_priv_ftm.h"

BDBG_MODULE(BAST_g2_priv_ftm);


/* private functions prototypes used by public ftm functions */
bool BAST_g2_Ftm_P_IsHwAssistStopped(BAST_Handle);
BERR_Code BAST_g2_Ftm_P_InitFtm(BAST_Handle);
BERR_Code BAST_g2_Ftm_P_ConfigureFsk(BAST_Handle, bool, bool);
BERR_Code BAST_g2_Ftm_P_VerifyInterrupts(BAST_Handle);
BERR_Code BAST_g2_Ftm_P_ResetUart(BAST_Handle h);
BERR_Code BAST_g2_Ftm_P_EnableHwAssist(BAST_Handle, bool);
BERR_Code BAST_g2_Ftm_P_DoWhenHwAssistIdle(BAST_Handle, BAST_FTM_ISR);
BERR_Code BAST_g2_Ftm_P_DoWhenHwAssistIdle_isr(BAST_Handle, BAST_FTM_ISR);
BERR_Code BAST_g2_Ftm_P_EnableFtmInterrupts(BAST_Handle, bool);
BERR_Code BAST_g2_Ftm_P_InitDevice(BAST_Handle);
BERR_Code BAST_g2_Ftm_P_InterruptOnPreamble(BAST_Handle);
BERR_Code BAST_g2_Ftm_P_InterruptOnNetworkActivity(BAST_Handle);
BERR_Code BAST_g2_Ftm_P_SetExcludeList(BAST_Handle, BAST_Ftm_Exclude, bool);
BERR_Code BAST_g2_Ftm_P_EnableReceiver(BAST_Handle, bool);
BERR_Code BAST_g2_Ftm_P_ProcessLocalCommand(BAST_Handle, uint8_t*, uint8_t);
BERR_Code BAST_g2_Ftm_P_SendLocalMessage(BAST_Handle, uint8_t*, uint8_t);
BERR_Code BAST_g2_Ftm_P_SendErrorMessage(BAST_Handle, uint8_t);
BERR_Code BAST_g2_Ftm_P_SendPacket(BAST_Handle, uint8_t*, uint8_t);
BERR_Code BAST_g2_Ftm_P_CheckCrc(BAST_Handle, uint8_t*, uint8_t, bool*);
BERR_Code BAST_g2_Ftm_P_GetRxBitMask(BAST_Handle);


static const uint32_t script_ftm_shutdown[] = 
{
   BAST_SCRIPT_WRITE(BCHP_FTM_PHY_FIRQ_STS, 0xFFFFFFFF), /* disable FTM interrupts */
   BAST_SCRIPT_AND_OR(BCHP_FTM_PHY_CORR_CTL, 0xFFFFFFFE, 0x00000000),   /* disable correlator */
   BAST_SCRIPT_AND_OR(BCHP_FTM_PHY_CTL, 0xFFFFFFFE, 0x00000000),        /* disable timer, disable rcvr */
   BAST_SCRIPT_EXIT
};


BAST_g2_Ftm_P_InterruptCbTable BAST_Ftm_Interrupts[] =
{
   BAST_INT_CB_DEF(BCHP_INT_ID_FTM_UART, BAST_Ftm_IntID_eUart, false)
   BAST_INT_CB_DEF(BCHP_INT_ID_FTM_UART_TX, BAST_Ftm_IntID_eUartTx, false)
   BAST_INT_CB_DEF(BCHP_INT_ID_FTM_UART_END, BAST_Ftm_IntID_eUartEnd, false)
   BAST_INT_CB_DEF(BCHP_INT_ID_FTM_UART_ERR, BAST_Ftm_IntID_eUartErr, false)
   BAST_INT_CB_DEF(BCHP_INT_ID_FTM_RSSI_RISE, BAST_Ftm_IntID_eRssiRise, false)
   BAST_INT_CB_DEF(BCHP_INT_ID_FTM_RSSI_FALL, BAST_Ftm_IntID_eRssiFall, false)
   BAST_INT_CB_DEF(BCHP_INT_ID_FTM_CORR_STATE, BAST_Ftm_IntID_eCorrState, false)
   BAST_INT_CB_DEF(BCHP_INT_ID_FTM_CORR_BYTE, BAST_Ftm_IntID_eCorrByte, false)
   BAST_INT_CB_DEF(BCHP_INT_ID_FTM_CORR_TIMEOUT, BAST_Ftm_IntID_eCorrTimeout, false)
   BAST_INT_CB_DEF(BCHP_INT_ID_FTM_CORR_AGC, BAST_Ftm_IntID_eCorrAgc, false)
   BAST_INT_CB_DEF(BCHP_INT_ID_FTM_CORR_PREAMBLE, BAST_Ftm_IntID_eCorrPreamble, false)
   BAST_INT_CB_DEF(BCHP_INT_ID_FTM_TIMER1, BAST_Ftm_IntID_eTimer1, false)
   BAST_INT_CB_DEF(BCHP_INT_ID_FTM_TIMER2, BAST_Ftm_IntID_eTimer2, false)
   BAST_INT_CB_DEF(BCHP_INT_ID_FTM_TIMER3, BAST_Ftm_IntID_eTimer3, false)
   BAST_INT_CB_DEF(BCHP_INT_ID_FTM_ASSIST_IDLE, BAST_Ftm_IntID_eAssistIdle, false)
   BAST_INT_CB_DEF(BCHP_INT_ID_FTM_RX_PKT_RDY, BAST_Ftm_IntID_eRxPktRdy, true)
   BAST_INT_CB_DEF(BCHP_INT_ID_FTM_ASSIST_LVL1, BAST_Ftm_IntID_eAssistLvl1, false)
   BAST_INT_CB_DEF(BCHP_INT_ID_FTM_ASSIST_LVL2, BAST_Ftm_IntID_eAssistLvl2, false)
   BAST_INT_CB_DEF(BCHP_INT_ID_FTM_ASSIST_LVL3, BAST_Ftm_IntID_eAssistLvl3, false)
   BAST_INT_CB_DEF(BCHP_INT_ID_FTM_ASSIST_LVL4, BAST_Ftm_IntID_eAssistLvl4, false)
   BAST_INT_CB_DEF(BCHP_INT_ID_FTM_HB_PKT_SENT, BAST_Ftm_IntID_eHbPktSent, false)
   BAST_INT_CB_DEF(BCHP_INT_ID_FTM_SP_PKT_SENT, BAST_Ftm_IntID_eSpPktSent, true)
   BAST_INT_CB_DEF(BCHP_INT_ID_FTM_ASSIST_ERR, BAST_Ftm_IntID_eAssistErr, false)
   BAST_INT_CB_DEF(BCHP_INT_ID_FTM_RX_PKT_RDY_CNT, BAST_Ftm_IntID_eRxPktRdyCnt, false)
   BAST_INT_CB_DEF(BCHP_INT_ID_FTM_FPKT_CNT, BAST_Ftm_IntID_eFPktCnt, false)
   BAST_INT_CB_DEF(BCHP_INT_ID_FTM_FBAD_CRC_CNT, BAST_Ftm_IntID_eFBadCrcCnt, true)
   BAST_INT_CB_DEF(BCHP_INT_ID_FTM_FRX_MSK_NM_CNT, BAST_Ftm_IntID_eFRxMskNmCnt, false)
   BAST_INT_CB_DEF(BCHP_INT_ID_FTM_FXCLUDE_CNT, BAST_Ftm_IntID_eFXcludeCnt, false)
   BAST_INT_CB_DEF(BCHP_INT_ID_FTM_RX_BIT_MASK_CHG, BAST_Ftm_IntID_eRxBitMaskChg, true)
   BAST_INT_CB_DEF(BCHP_INT_ID_FTM_SER_NUM_CHG, BAST_Ftm_IntID_eSerNumChg, true)
   BAST_INT_CB_DEF(BCHP_INT_ID_FTM_RX_TYPE1_CNT, BAST_Ftm_IntID_eRxType1Cnt, false)
   BAST_INT_CB_DEF(BCHP_INT_ID_FTM_RX_TYPE2_CNT, BAST_Ftm_IntID_eRxType2Cnt, false)
};


/******************************************************************************
 BAST_g2_P_ResetFtm() - Non-ISR context
******************************************************************************/ 
BERR_Code BAST_g2_P_ResetFtm(BAST_Handle h)
{
   BAST_g2_P_Handle *hDev = (BAST_g2_P_Handle *)(h->pImpl);
   BERR_Code retCode = BERR_SUCCESS;
   
   BDBG_ENTER(BAST_g2_P_ResetFtm);
   
   if (hDev->bFtmPoweredDown)
      return BAST_ERR_POWERED_DOWN;
   
   /* hw assist must be off */
   if (!BAST_g2_Ftm_P_IsHwAssistStopped(h))
   {
      /* attempt to disable hw assist */
      retCode = BAST_g2_Ftm_P_EnableHwAssist(h, false);
      if (retCode != BERR_SUCCESS)
      {
         /* wait for hw assist to be idle if disable failed */
         retCode = BAST_g2_Ftm_P_DoWhenHwAssistIdle(h, BAST_g2_Ftm_P_InitFtm);
         return retCode;
      }
   }
   
   /* turn off ftm interrupts */
   BAST_g2_Ftm_P_EnableFtmInterrupts(h, false);
   
   BKNI_EnterCriticalSection();
   retCode = BAST_g2_Ftm_P_InitFtm(h);
   BKNI_LeaveCriticalSection();
   
   /* turn on ftm interrupts */
   BAST_g2_Ftm_P_EnableFtmInterrupts(h, true);
   
   BDBG_LEAVE(BAST_g2_P_ResetFtm);
   return retCode;
}


/******************************************************************************
 BERR_Code BAST_g2_P_ReadFtm() - Non-ISR context
******************************************************************************/
BERR_Code BAST_g2_P_ReadFtm(
   BAST_Handle h,  /* [in] BAST handle */
   uint8_t *pBuf,  /* [out] data read */
   uint8_t *n      /* [out] length of data read */
)
{
   BAST_g2_P_Handle *hDev = (BAST_g2_P_Handle *)(h->pImpl);
   BAST_g2_P_FtmDevice *hFtm = (BAST_g2_P_FtmDevice *) &(((BAST_g2_P_Handle *)(h->pImpl))->hFtmDev);
   BERR_Code retCode = BERR_SUCCESS;
   *n = 0;
   
   BDBG_ENTER(BAST_g2_P_ReadFtm);
   
   if (hDev->bFtmPoweredDown)
      return BAST_ERR_POWERED_DOWN;
   
   if ((hFtm->buf_status & (BAST_FTM_BUF_ERR_NOT_EMPTY | BAST_FTM_BUF_HOST_NOT_EMPTY | BAST_FTM_BUF_RX_NOT_EMPTY)) == 0)
      return BERR_SUCCESS;
   
   /* disable interrupts */
   BAST_CHK_RETCODE(BAST_g2_Ftm_P_EnableFtmInterrupts(h, false));
   
   /* check err buffer then host buffer then rx buffer */
   if (hFtm->buf_status & BAST_FTM_BUF_ERR_NOT_EMPTY)
   {
      *n = (hFtm->err_buffer[0] & 0x7F) + 1;
      BKNI_Memcpy(pBuf, hFtm->err_buffer, *n);
      hFtm->err_buffer[0] = 0;
      hFtm->buf_status &= ~BAST_FTM_BUF_ERR_NOT_EMPTY;
   }
   else if (hFtm->buf_status & BAST_FTM_BUF_HOST_NOT_EMPTY)
   {
      *n = (hFtm->host_buffer[0] & 0x7F) + 1;
      BKNI_Memcpy(pBuf, hFtm->host_buffer, *n);
      hFtm->host_buffer[0] = 0;
      hFtm->buf_status &= ~BAST_FTM_BUF_HOST_NOT_EMPTY;
   }
   else if (hFtm->buf_status & BAST_FTM_BUF_RX_NOT_EMPTY)
   {
      *n = (hFtm->rx_buffer[0] & 0x7F) + 1;
      BKNI_Memcpy(pBuf, hFtm->rx_buffer, *n);
      hFtm->rx_buffer[0] = 0;
      hFtm->buf_status &= ~BAST_FTM_BUF_RX_NOT_EMPTY;
   }
   
   if (hFtm->buf_status & (BAST_FTM_BUF_ERR_NOT_EMPTY | BAST_FTM_BUF_HOST_NOT_EMPTY | BAST_FTM_BUF_RX_NOT_EMPTY))
      BKNI_SetEvent(hDev->hFtmEvent);
   
   done:
   BAST_g2_Ftm_P_EnableFtmInterrupts(h, true);
   BDBG_LEAVE(BAST_g2_P_ReadFtm);
   return retCode;
}


/******************************************************************************
 BERR_Code BAST_g2_P_WriteFtm() - Non-ISR context
******************************************************************************/
BERR_Code BAST_g2_P_WriteFtm(
   BAST_Handle h,  /* [in] BAST handle */
   uint8_t *pBuf,  /* [in] data to write */
   uint8_t n       /* [in] length of data to write */
)
{
   BAST_g2_P_Handle *hDev = (BAST_g2_P_Handle *)(h->pImpl);
   BAST_g2_P_FtmDevice *hFtm = (BAST_g2_P_FtmDevice *) &(((BAST_g2_P_Handle *)(h->pImpl))->hFtmDev);
   BERR_Code retCode = BERR_SUCCESS;
   bool bValidCrc = true;
   
   BDBG_ENTER(BAST_g2_P_WriteFtm);
   
   if (hDev->bFtmPoweredDown)
      return BAST_ERR_POWERED_DOWN;
   
   /* disable interrupts */
   BAST_CHK_RETCODE(BAST_g2_Ftm_P_EnableFtmInterrupts(h, false));

#if !defined(BAST_FTM_DEBUG) && !defined(NOOS) && !defined(SOAP)
   /* Per DirecTV, dont allow debug options in production code */
   if (n == 4)
   {
      if ((pBuf[0] == 0x83) && (pBuf[1] == 0x04))
      {
         /* this is the "Set Options" local message */
         /* dont allow option bits 0, 1, 3, 4, 9, 14 */
         if ((pBuf[2] & 0x42) || (pBuf[3] & 0x1B))
         {
            BDBG_ERR(("specified SWM option bit(s) are not allowed in production code"));
            return BERR_NOT_SUPPORTED;
         }
      }
      else if ((pBuf[0] == 0x83) && (pBuf[1] == 0x02))
      {
         /* this is the "Set Rx Bit Mask" local message */
         BDBG_ERR(("Set Rx Bit Mask SWM command not allowed in production code"));
         return BERR_NOT_SUPPORTED;
      }
   }
#endif

   if (pBuf[0] & 0x80)
   {
      /* this is a local command */
      retCode = BAST_g2_Ftm_P_ProcessLocalCommand(h, pBuf, n);
   }
   else
   {
      /* this is a network packet */
      if (hFtm->buf_status & BAST_FTM_BUF_TX_NOT_EMPTY)
      {
         hFtm->err_status |= BAST_FTM_ERR_TX_PKT_DROPPED;
         retCode = BAST_g2_Ftm_P_SendErrorMessage(h, BAST_FTM_MSG_ERR_BUSY);
         goto done;
      }
      
      /* check for invalid length */
      if (((pBuf[0] + 1) != n) || (n > 65))
         goto bad_packet;
      
      /* NOT IMPLEMENTED: validate outgoing packet if necessary */
      if (hFtm->options & BAST_FTM_OPTION_VALIDATE_OUT_PKTS)
      {
         /* should not use hw crc check since hw assist not idle! */
         /* BAST_CHK_RETCODE(BAST_g2_Ftm_P_CheckCrc(h, &pBuf[1], n - 1, &bValidCrc)); */
         
         if (!bValidCrc)
         {
            bad_packet:
            hFtm->buf_status &= ~BAST_FTM_BUF_TX_NOT_EMPTY;
            retCode = BAST_g2_Ftm_P_SendErrorMessage(h, BAST_FTM_MSG_ERR_BAD_PKT_SENT);
            goto done;
         }
      }
      
      /* send network packet */
      retCode = BAST_g2_Ftm_P_SendPacket(h, pBuf, n);
   }
   
   done:
   BAST_g2_Ftm_P_EnableFtmInterrupts(h, true);
   BDBG_LEAVE(BAST_g2_P_WriteFtm);
   return retCode;
}


/******************************************************************************
 BAST_g2_P_PowerDownFtm() - Non-ISR context
******************************************************************************/
BERR_Code BAST_g2_P_PowerDownFtm(
   BAST_Handle h  /* [in] BAST handle */
)
{
#ifndef BAST_EXCLUDE_POWERDOWN
   BAST_g2_P_Handle *hDev = (BAST_g2_P_Handle *)(h->pImpl);
   BERR_Code retCode = BERR_SUCCESS;
   BAST_g2_P_ChannelHandle *hChn0 = (BAST_g2_P_ChannelHandle *)(h->pChannels[0]->pImpl);
   BAST_g2_P_ChannelHandle *hChn1 = (BAST_g2_P_ChannelHandle *)(h->pChannels[1]->pImpl);
   uint32_t mb;
   
   BDBG_ASSERT(hChn0);
   BDBG_ASSERT(hChn1);
   BDBG_ENTER(BAST_g2_P_PowerDownFtm);
   
   /* disable hw assist, disable receiver, reset state */
   BAST_CHK_RETCODE(BAST_g2_Ftm_P_EnableHwAssist(h, false));
   BAST_CHK_RETCODE(BAST_g2_Ftm_P_EnableReceiver(h, false));
   BAST_g2_Ftm_P_InitDevice(h);
   
   hDev->bFtmPoweredDown = true;
   
   /* power down ftm phy for non-analog diseqc mode */
   BAST_FTM_READ(FTM_PHY_ANA_MISC, &mb);
   if (h->settings.networkSpec != BAST_NetworkSpec_eEcho)
   {
      mb |= 0x2081;  /* power down phy, stop 108MHz clock */
   }
   else
   {
      /* power down ftm phy if all diseqcs powered down */
      if ((h->totalChannels == 2) && (hChn0->coresPoweredDown & BAST_CORE_DISEQC))
         mb |= 0x2081;
      else if ((hChn0->coresPoweredDown & BAST_CORE_DISEQC) && (hChn1->coresPoweredDown & BAST_CORE_DISEQC))
         mb |= 0x2081;
   }
   BAST_FTM_WRITE(FTM_PHY_ANA_MISC, mb);
   
   done:
   BDBG_LEAVE(BAST_g2_P_PowerDownFtm);
   return retCode;
#else
   return BERR_NOT_SUPPORTED;
#endif
}


/******************************************************************************
 BAST_g2_P_PowerUpFtm() - Non-ISR context
******************************************************************************/
BERR_Code BAST_g2_P_PowerUpFtm(
   BAST_Handle h  /* [in] BAST handle */
)
{
#ifndef BAST_EXCLUDE_POWERDOWN
   BAST_g2_P_Handle *hDev = (BAST_g2_P_Handle *)(h->pImpl);
   BERR_Code retCode = BERR_SUCCESS;
   uint32_t mb;
   
   BDBG_ENTER(BAST_g2_P_PowerUpFtm);
   
   hDev->bFtmPoweredDown = false;
   
   BAST_FTM_READ(FTM_PHY_ANA_MISC, &mb);
   mb &= ~0x2080;  /* power up phy first */
   BAST_FTM_WRITE(FTM_PHY_ANA_MISC, mb);
   mb &= ~0x01;    /* start 108MHz clock */
   BAST_FTM_WRITE(FTM_PHY_ANA_MISC, mb);
   
   BAST_g2_Ftm_P_InitDevice(h);
   
   BDBG_LEAVE(BAST_g2_P_PowerUpFtm);
   return retCode;
#else
   return BERR_NOT_SUPPORTED;
#endif
}


/******************************************************************************
 BAST_g2_P_GetFtmEventHandle()
******************************************************************************/
BERR_Code BAST_g2_P_GetFtmEventHandle(BAST_Handle h, BKNI_EventHandle *hFtmEvent)
{
   BERR_Code retCode = BERR_SUCCESS;
   BDBG_ENTER(BAST_g2_P_GetFtmEventHandle);
  
   *hFtmEvent = ((BAST_g2_P_Handle *)(h->pImpl))->hFtmEvent;
   
   BDBG_LEAVE(BAST_g2_P_GetFtmEventHandle);
   return retCode;
}


/******************************************************************************
 BAST_g2_Ftm_P_InitFtm() - ISR context
******************************************************************************/ 
BERR_Code BAST_g2_Ftm_P_InitFtm(
   BAST_Handle h    /* [in] BAST channel handle */
)
{
   BAST_g2_P_Handle *hDev = (BAST_g2_P_Handle *)(h->pImpl);
   BAST_g2_P_FtmDevice *hFtm = (BAST_g2_P_FtmDevice *) &(((BAST_g2_P_Handle *)(h->pImpl))->hFtmDev);
   BERR_Code retCode = BERR_SUCCESS;
   uint8_t buf[2];
   uint32_t i;
   
   static uint32_t phy_coef[] =
   {
      0xfffe0011, 0x00200022, 0x0016fffe, 0xffe1ffcc, 0xffcaffe1, 0x000d003f, 0x0060005c, 0x002affd8, 0xff84ff56,
      0xff6fffd1, 0x006000e1, 0x011500D3, 0x0022ff3d, 0xfe85fe57, 0xfee4000d, 0x01680260, 0x02700165, 0xff87fd85,
      0xfc44fc7a, 0xfe580159, 0x045d0606, 0x054401dd, 0xfcb0f799, 0xf4f0f6bf, 0xfe010a1d, 0x18e92727, 0x316f352d
   };
   
   BDBG_ASSERT(BAST_g2_Ftm_P_IsHwAssistStopped(h));
   
   /* reset ftm uart, tx, rx */
   BAST_FTM_WRITE(FTM_PHY_RST, 0x000001C0);
   
   /* initialize ftm dev struct */
   BAST_g2_Ftm_P_InitDevice(h);
   
   BAST_FTM_WRITE(FTM_PHY_FIRQ_STS, 0xFFFFFFFF);    /* clear all interrupts */
   BAST_FTM_WRITE(FTM_PHY_ASSIST_CTL, 0x00000001);  /* stop HW assist */

   BAST_FTM_WRITE(FTM_PHY_ADDR, 0x80000000);        /* write coef memory w/ auto increment */
   for(i = 0; i < 27; i++)                          /* write next 27 values to FTM_PHY_COEF */
      BAST_FTM_WRITE(FTM_PHY_COEF, phy_coef[i]);
   BAST_FTM_WRITE(FTM_PHY_ADDR, 0x00000000);        /* disable auto increment */

   BAST_FTM_WRITE(FTM_PHY_CORR_PREAMBLE, 0x0055550D);
   BAST_FTM_WRITE(FTM_PHY_CORR_THRES, 0x00970550);
   BAST_FTM_WRITE(FTM_PHY_CORR_PEAK_QUAL, 0x00180A8C);
   BAST_FTM_WRITE(FTM_PHY_CORR_CTL, 0x0043920C);      /* corr idle after 67 bytes, corr disabled, 3.9 msec agc window */
   BAST_FTM_WRITE(FTM_PHY_CORR_TIMEOUT, 0x00000000);
   BAST_FTM_WRITE(FTM_PHY_CTL, 0x00000001);           /* don't invert data; auto rx disable on */
   BAST_FTM_WRITE(FTM_PHY_TX_CTL, 0x0AC60000);        /* 256us end delay, increase tx pre-carrier to 5.1ms */
   BAST_FTM_WRITE(FTM_UART_ASSIST_BRR, 0x00020001);   /* disable auto crc, reset HB, sp pkt, store buffers */
   BAST_FTM_WRITE(FTM_PHY_ASSIST_CNT2, 0x00000001);   /*  bad crc down counter = 1 */
   BAST_FTM_WRITE(FTM_UART_ASSIST_SERNMGNTR, 0x00000000);   /* initialize FTM serial number, monitor only my polls for ftm s/n */
   BAST_FTM_WRITE(FTM_UART_ASSIST_PCTL2R, 0x000094C6);      /* initialize exclude list */
   BAST_FTM_WRITE(FTM_UART_ASSIST_PCTL1R, 0x80000004);      /* initialize rx_bit_mask */
   BAST_FTM_WRITE(FTM_UART_ASSIST_CMDTRR, 0x94C694C6);      /* 7-bit checking on offer and poll cmd ids */
   BAST_FTM_WRITE(FTM_UART_ASSIST_RXMGNT2R, 0x00048003);    /* set MAX_NO_POLL to 4 */
   
#if 0
#if (BCHP_CHIP==7342) || (BCHP_CHIP==7340)
   /* sliding window settings */
   #if 0
   BAST_FTM_WRITE(FTM_PHY_CORR2_CTL, 0x00000001);     /* enable preamble sliding window detection */
   BAST_FTM_WRITE(FTM_PHY_CORR2_THRES, 0x43012011);   /* [8:0] pream0_thres = 0x11; [20:12] pream2_thres=0x12 */
   BAST_FTM_WRITE(FTM_PHY_CORR_IRQ, 0x18000009);      /* [31:24] pream_timeout [15:0] timeout=9, [16] return_idle_timeout=0 */
   #endif
   
   /* common settings */
   BAST_FTM_WRITE(FTM_PHY_CORR_PREAMBLE, 0x0055550D);    /* preamble */
   BAST_FTM_WRITE(FTM_PHY_CORR_PEAK_QUAL, 0x000B0510);   /* sample_length=11, clock_length=1296 (108*(sample_length+1)) */ 
   BAST_FTM_WRITE(FTM_PHY_CORR_IRQ, 0x00000000);         /* interrupt on byte  */
   #if 1
   /* version one - preamble sliding window detection with original correlator */
   BAST_FTM_WRITE(FTM_PHY_CORR2_CTL, 0x00000001);     /* enable preamble sliding window detection */
   BAST_FTM_WRITE(FTM_PHY_CORR2_THRES, 0x4300C00D);   /* [31:28] pream2_vld_time, [27:24] pream0_vld_time, [8:0] pream0_thres; [20:12] pream2_thres */
   BAST_FTM_WRITE(FTM_PHY_CORR2_DATA, 0x00000000);    /* [21:20] peak_high_2_peak_low ratio */
   #else
   /* version two,  preamble sliding window detection with enhanced correlator */
   BAST_FTM_WRITE(FTM_PHY_CORR2_CTL, 0x00000031);     /* enable preamble sliding window detection and enhanced correlator */
   BAST_FTM_WRITE(FTM_PHY_CORR2_THRES, 0x4300C00D);   /* [31:28] pream2_vld_time, [27:24] pream0_vld_time, [8:0] pream0_thres; [20:12] pream2_thres */
   BAST_FTM_WRITE(FTM_PHY_CORR2_DATA, 0x003044D0);    /* [21:20] peak_high_2_peak_low ratio  50% */
   #endif
   BAST_FTM_WRITE(FTM_PHY_CORR_TIMEOUT, 0x17000009);  /* [31:24] pream_timeout [15:0] timeout=9, [16] return_idle_timeout=0 */
   BAST_FTM_WRITE(FTM_PHY_CORR_THRES, 0x004E0300);    /* frame_thres=190, start_thres=0x0300, good for -60dBm attanuation */
   BAST_FTM_WRITE(FTM_PHY_CORR_CTL, 0x0043F70D);      /* packet_length=67, [12:8] frame_length=4.9ms, dc offset on */
#endif
#endif
   
   /* reset fsk uart */
   BAST_g2_Ftm_P_ResetUart(h);
   
#if (BCHP_CHIP==7335)
   /* check ftm interrupts */
   retCode = BAST_g2_Ftm_P_VerifyInterrupts(h);
   if (retCode != BERR_SUCCESS)
   {
      BDBG_MSG(("FTM interrupt out of sync!(1)"));
      BAST_g2_Ftm_P_ConfigureFsk(h, true, true);   /* swap clocks */
      
      /* check ftm interrupts again */
      retCode = BAST_g2_Ftm_P_VerifyInterrupts(h);
      if (retCode != BERR_SUCCESS)
      {
         BDBG_MSG(("FTM interrupt out of sync!(2)"));
         BAST_g2_Ftm_P_ConfigureFsk(h, true, false);   /* select default clock */
         
         /* check ftm interrupts one more time */
         retCode = BAST_g2_Ftm_P_VerifyInterrupts(h);
         if (retCode != BERR_SUCCESS)
         {
            BDBG_MSG(("FTM interrupt out of sync!(3)"));
            BAST_g2_Ftm_P_ConfigureFsk(h, false, false);   /* unswap clocks */
            
            retCode = BAST_g2_Ftm_P_VerifyInterrupts(h);
            if (retCode != BERR_SUCCESS)
            {
               hFtm->err_status |= 0x80;
               return BERR_UNKNOWN;
            }
         }
      }
   }
#endif

   /* turn on receiver */
   BAST_CHK_RETCODE(BAST_g2_Ftm_P_InterruptOnPreamble(h));
   BAST_CHK_RETCODE(BAST_g2_Ftm_P_InterruptOnNetworkActivity(h));
   BAST_CHK_RETCODE(BAST_g2_Ftm_P_EnableHwAssist(h, true));
   BAST_CHK_RETCODE(BAST_g2_Ftm_P_EnableReceiver(h, true));
   
   /* send the reset response only for local reset command */
   if (hDev->bFtmLocalReset)
   {
      buf[0] = 0x81;
      buf[1] = BAST_FTM_MSG_RESET_UC;
      BAST_CHK_RETCODE(BAST_g2_Ftm_P_SendLocalMessage(h, buf, 2));
      hDev->bFtmLocalReset = false;
   }
   
   /* clear all interrupts */
   BAST_FTM_WRITE(FTM_PHY_FIRQ_STS, 0xFFFFFFFF);
   for (i = 0; i < 32; i++)
   {
      retCode = BINT_ClearCallback_isr(hFtm->hCallback[i]);
      BDBG_ASSERT(retCode == BERR_SUCCESS);
   }
   
   done:
   return retCode;
}


/******************************************************************************
 BAST_g2_Ftm_P_InitDevice()
******************************************************************************/
BERR_Code BAST_g2_Ftm_P_InitDevice(BAST_Handle h)
{
   BAST_g2_P_Handle *hDev = (BAST_g2_P_Handle *)(h->pImpl);
   BAST_g2_P_FtmDevice *hFtm = (BAST_g2_P_FtmDevice *) &(((BAST_g2_P_Handle *)(h->pImpl))->hFtmDev);
   BERR_Code retCode = BERR_SUCCESS;
   uint32_t mb;

   /* init ftm parameters */
   hFtm->idle_funct = NULL;
   hFtm->options = 0x0000;
   hFtm->rx_bit_mask = 0x8000;
   hFtm->sp_packet_src_addr = 0;
   
   /* reset statuses */
   hFtm->status = BAST_FTM_STATUS_RESET;
   hFtm->buf_status = BAST_FTM_BUF_ALL_EMPTY;
   hFtm->err_status = BAST_FTM_ERR_NONE;
   
#if (BCHP_CHIP==7335)
   BAST_g2_Ftm_P_ConfigureFsk(h, false, true);   /* use diff osc for ftm clock */
#else
   BAST_g2_Ftm_P_ConfigureFsk(h, false, false);
#endif
   
   /* set up ftm tx level */
   if(hDev->ftmChannelSelect > BAST_FskChannelConfig_eCh0Tx_Ch1Rx)
   {
      BAST_FTM_READ(FTM_PHY_ANA1_0, &mb);
      mb &= ~0x3F;
      mb |= hDev->ftmTxPower & 0x3F;
      BAST_FTM_WRITE(FTM_PHY_ANA1_0, mb);
   }
   else
   {
      BAST_FTM_READ(FTM_PHY_ANA0_0, &mb);
      mb &= ~0x3F;
      mb |= hDev->ftmTxPower & 0x3F;
      BAST_FTM_WRITE(FTM_PHY_ANA0_0, mb);
   }
   
   return retCode;
}


/******************************************************************************
 BAST_g2_Ftm_P_ConfigureFsk() - ISR context
******************************************************************************/ 
BERR_Code BAST_g2_Ftm_P_ConfigureFsk(BAST_Handle h, bool bClkSwap, bool bAltClk)
{
   BAST_g2_P_Handle *hDev = (BAST_g2_P_Handle *)(h->pImpl);
   uint32_t mb;
   
   /* set up fsk channels */
   if (!bClkSwap)
   {
      switch(hDev->ftmChannelSelect)
      {
         case BAST_FskChannelConfig_eCh0Tx_Ch0Rx:
            mb = 0x02060;  /* ch1_fskphy_pwrdn | ch0_fskphy_dac_en | ch0_tx_out_edge */
            break;
         case BAST_FskChannelConfig_eCh0Tx_Ch1Rx:
            mb = 0x80060;  /* ch1_ftm_rx_sel | ch0_fskphy_dac_en | ch0_tx_out_edge */
            break;
         case BAST_FskChannelConfig_eCh1Tx_Ch0Rx:
            mb = 0x11C00;  /* ch0_clk_swp | ch1_fskphy_dac_en | ch1_tx_out_edge | ch1_ftm_tx_sel */
            break;
         case BAST_FskChannelConfig_eCh1Tx_Ch1Rx:
            mb = 0x91C80;  /* ch1_ftm_rx_sel | ch0_clk_swp | ch1_fskphy_dac_en | ch1_tx_out_edge | ch1_ftm_tx_sel | ch0_fskphy_pwrdn */
            break;
         default:
            mb = 0x00060;  /* use ch0 tx ch0 rx as default */
      }
   }
   else
   {
      switch(hDev->ftmChannelSelect)
      {
         case BAST_FskChannelConfig_eCh0Tx_Ch0Rx:
            mb = 0x10060;  /* ch0_clk_swp | ch0_fskphy_dac_en | ch0_tx_out_edge */
            break;
         case BAST_FskChannelConfig_eCh0Tx_Ch1Rx:
            mb = 0x90060;  /* ch1_ftm_rx_sel | ch0_clk_swp | ch0_fskphy_dac_en | ch0_tx_out_edge */
            break;
         case BAST_FskChannelConfig_eCh1Tx_Ch0Rx:
            mb = 0x41C00;  /* ch1_clk_swp | ch1_fskphy_dac_en | ch1_tx_out_edge | ch1_ftm_tx_sel */
            break;
         case BAST_FskChannelConfig_eCh1Tx_Ch1Rx:
            mb = 0xC1C00;  /* ch1_ftm_rx_sel | ch1_clk_swp | ch1_fskphy_dac_en | ch1_tx_out_edge | ch1_ftm_tx_sel */
            BAST_FTM_WRITE(FTM_PHY_CTL, 0x00000003);
            break;
         default:
            mb = 0x10060;
      }
   }
   if (bAltClk)
      mb |= 0x4100;  /* use alternate clock source */
      
   BAST_FTM_WRITE(FTM_PHY_ANA_MISC, mb);
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g2_Ftm_P_VerifyInterrupts() - ISR context
******************************************************************************/ 
BERR_Code BAST_g2_Ftm_P_VerifyInterrupts(BAST_Handle h)
{
   BAST_g2_P_FtmDevice *hFtm = (BAST_g2_P_FtmDevice *) &(((BAST_g2_P_Handle *)(h->pImpl))->hFtmDev);
   BERR_Code retCode = BERR_SUCCESS;
   uint32_t i, mb, mb2, tx_ctl, phy_ctl, ana_misc;
   
   BDBG_ASSERT(BAST_g2_Ftm_P_IsHwAssistStopped(h));
   
   /* disable fskphy for loopback */
   BAST_FTM_READ(FTM_PHY_ANA_MISC, &ana_misc);
   mb = (ana_misc | 0x80010);    /* ch1_ftm_rx_sel | ch0_diseqc_sel */
   mb &= ~0x1440;                /* ch1_fskphy_dac_en=0 | ch1_ftm_tx_sel=0 | ch0_fsk_phy_dac_en=0 */
   BAST_FTM_WRITE(FTM_PHY_ANA_MISC, mb);
   
   /* set rx input to loopback and auto rx disable off */
   BAST_FTM_READ(FTM_PHY_CTL, &phy_ctl);
   mb = (phy_ctl | 0x40C);
   BAST_FTM_WRITE(FTM_PHY_CTL, mb);
   
   /* increase tx post-carrier to 16 bit time */
   BAST_FTM_WRITE(FTM_PHY_TX_CTL, 0x0FC60000);
   
   /* set trace countdown timers to 1 */
   BAST_FTM_WRITE(FTM_PHY_ASSIST_CNT1, 0x10000);   /* trace rx packet count */
   BAST_FTM_WRITE(FTM_PHY_ASSIST_CNT3, 0x0101);    /* trace offer count */
   BAST_FTM_READ(FTM_UART_ASSIST_CMDTRR, &mb);
   mb |= 0x1414;     /* track manual offer */
   BAST_FTM_WRITE(FTM_UART_ASSIST_CMDTRR, mb);
   
   /* set ftm timers to 1us */
   BAST_FTM_WRITE(FTM_PHY_TIMER1, 1);
   BAST_FTM_WRITE(FTM_PHY_TIMER2, 1);
   BAST_FTM_WRITE(FTM_PHY_TIMER3, 1);
   
   /* clear all interrupts */
   BAST_FTM_WRITE(FTM_PHY_FIRQ_STS, 0xFFFFFFFF);
   for (i = 0; i < 32; i++)
   {
      retCode = BINT_ClearCallback_isr(hFtm->hCallback[i]);
      BDBG_ASSERT(retCode == BERR_SUCCESS);
   }
   
   /* enable timers */
   BAST_FTM_READ(FTM_PHY_CTL, &mb);
   mb |= 0x001C0000;
   BAST_FTM_WRITE(FTM_PHY_CTL, mb);
   
   /* remove offers from exclude list and enable hw assist */
   BAST_g2_Ftm_P_SetExcludeList(h, BAST_Ftm_Exclude_eOffers, false);
   BAST_CHK_RETCODE(BAST_g2_Ftm_P_EnableReceiver(h, true));
   BAST_CHK_RETCODE(BAST_g2_Ftm_P_EnableHwAssist(h, true));

   /* disable tx state machine */
   BAST_FTM_READ(FTM_PHY_TX_CTL, &tx_ctl);
   tx_ctl &= 0xFFFFFFFE;
   BAST_FTM_WRITE(FTM_PHY_TX_CTL, tx_ctl);
   
   /* reset the tx fifo */
   mb = 0x05;
   BAST_FTM_WRITE(FTM_UART_FCR, mb);
   
   /* start the transmit */
   tx_ctl &= 0xF0FF00FF;
   tx_ctl |= 0x0A000000;
   tx_ctl |= ((3 + 7) << 8);   /* set packet size for preamble + offer */
   tx_ctl |= 0x00000001;
   BAST_FTM_WRITE(FTM_PHY_TX_CTL, tx_ctl);

   /* send out preamble and test offer packet */
   mb = 0x55;
   BAST_FTM_WRITE(FTM_UART_THR, mb);
   BAST_FTM_WRITE(FTM_UART_THR, mb);
   mb = 0x0D;
   BAST_FTM_WRITE(FTM_UART_THR, mb);
   mb = 0x14;
   BAST_FTM_WRITE(FTM_UART_THR, mb);
   mb = 0x00;
   BAST_FTM_WRITE(FTM_UART_THR, mb);
   mb = 0x0F;
   BAST_FTM_WRITE(FTM_UART_THR, mb);
   mb = 0x02;
   BAST_FTM_WRITE(FTM_UART_THR, mb);
   mb = 0x01;
   BAST_FTM_WRITE(FTM_UART_THR, mb);
   mb = 0x0F;
   BAST_FTM_WRITE(FTM_UART_THR, mb);
   mb = 0xA7;
   BAST_FTM_WRITE(FTM_UART_THR, mb);
   
   /* wait for uart end */
   while(1)
   {
      BAST_FTM_READ(FTM_PHY_IRQ_STS, &mb);
      if (mb & 0x4) break;
   }
   
   /* wait to disable hw assist */
   while(1)
   {
      retCode = BAST_g2_Ftm_P_EnableHwAssist(h, false);
      if (retCode == BERR_SUCCESS) break;
   }
   
   /* disable tx state machine */
   BAST_FTM_READ(FTM_PHY_TX_CTL, &mb);
   mb &= 0xFFFFFFFE;
   BAST_FTM_WRITE(FTM_PHY_TX_CTL, mb);

   /* disable receiver */
   BAST_g2_Ftm_P_EnableReceiver(h, false);
   
   /* set rx input back and post-carrier back to default */
   BAST_FTM_WRITE(FTM_PHY_CTL, phy_ctl);
   BAST_FTM_WRITE(FTM_PHY_TX_CTL, 0x0AC60000);
   BAST_g2_Ftm_P_SetExcludeList(h, BAST_Ftm_Exclude_eOffers, true);
   
   /* compare interrupt registers */
   BAST_FTM_READ(FTM_PHY_IRQ_STS, &mb);
   BAST_FTM_READ(FTM_INTR2_CPU_STATUS, &mb2);
   if (mb != mb2)
      return BERR_UNKNOWN;
   
   /* restore settings */
   BAST_FTM_WRITE(FTM_PHY_ANA_MISC, ana_misc);
   
   done:
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g2_Ftm_P_ResetUart() - Non-ISR context
******************************************************************************/
BERR_Code BAST_g2_Ftm_P_ResetUart(BAST_Handle h)
{
   /* reset uart */
   BAST_FTM_WRITE(FTM_UART_FCR, 0x07);
   BAST_FTM_WRITE(FTM_UART_MCR, 0x20);
   BAST_FTM_WRITE(FTM_UART_LCR, 0x80);
   BAST_FTM_WRITE(FTM_UART_DLL, 0xAD);
   BAST_FTM_WRITE(FTM_UART_LCR, 0x00);
   BAST_FTM_WRITE(FTM_UART_IER, 0x82);    /* enable TX fifo empty interrupt, write IER: [7] PTIME, [1] ETBEI [0] ERBFI */
   BAST_FTM_WRITE(FTM_UART_LCR, 0x03);
   BAST_FTM_WRITE(FTM_UART_ASSIST_DBGR, 0x003B0210); /* prevent packet overlap by increasing bit time delay to 15 bits */
   
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g2_Ftm_P_EnableFtmInterrupts() - Non-ISR context
******************************************************************************/
BERR_Code BAST_g2_Ftm_P_EnableFtmInterrupts(BAST_Handle h, bool bEnable)
{
   BAST_g2_P_FtmDevice *hFtm = (BAST_g2_P_FtmDevice *) &(((BAST_g2_P_Handle *)(h->pImpl))->hFtmDev);
   BERR_Code retCode = BERR_SUCCESS;
   uint8_t i;
   
   if (bEnable)
   {
      for (i = 0; i < 32 ; i++)
      {
         /* clear callback */
         retCode = BINT_ClearCallback(hFtm->hCallback[i]);
         BDBG_ASSERT(retCode == BERR_SUCCESS);
            
         if (BAST_Ftm_Interrupts[i].enable)
         {
            /* enable the interrupt */
            retCode = BINT_EnableCallback(hFtm->hCallback[i]);
            BDBG_ASSERT(retCode == BERR_SUCCESS);
         }
         else
         {
            /* disable the interrupt */
            retCode = BINT_DisableCallback(hFtm->hCallback[i]);
            BDBG_ASSERT(retCode == BERR_SUCCESS);
            retCode = BINT_ClearCallback(hFtm->hCallback[i]);
            BDBG_ASSERT(retCode == BERR_SUCCESS);
         }
      }
   }
   else
   {
      for(i = 0; i < 32 ; i++)
      {
         /* disable and clear the interrupt */
         retCode = BINT_DisableCallback(hFtm->hCallback[i]);
         BDBG_ASSERT(retCode == BERR_SUCCESS);
         retCode = BINT_ClearCallback(hFtm->hCallback[i]);
         BDBG_ASSERT(retCode == BERR_SUCCESS);
      }
   }
   
   return retCode;
}


/******************************************************************************
 BAST_g2_Ftm_P_EnableInterrupt_isr() - ISR context
******************************************************************************/
BERR_Code BAST_g2_Ftm_P_EnableInterrupt_isr(BAST_Handle h, BAST_Ftm_IntID intID, bool bEnable)
{
   BAST_g2_P_FtmDevice *hFtm = (BAST_g2_P_FtmDevice *) &(((BAST_g2_P_Handle *)(h->pImpl))->hFtmDev);
   BERR_Code retCode = BERR_SUCCESS;
   
   /* clear callback */
   retCode = BINT_ClearCallback_isr(hFtm->hCallback[intID]);
   BDBG_ASSERT(retCode == BERR_SUCCESS);
   
   if (bEnable)
   {
      BAST_Ftm_Interrupts[intID].enable = true;
      retCode = BINT_EnableCallback_isr(hFtm->hCallback[intID]);
      BDBG_ASSERT(retCode == BERR_SUCCESS);
   }
   else
   {
      BAST_Ftm_Interrupts[intID].enable = false;
      retCode = BINT_DisableCallback_isr(hFtm->hCallback[intID]);
      BDBG_ASSERT(retCode == BERR_SUCCESS);
   }
   
   return retCode;
}


/******************************************************************************
 BAST_g2_Ftm_P_EnableReceiver()
******************************************************************************/
BERR_Code BAST_g2_Ftm_P_EnableReceiver(BAST_Handle h, bool bEnable)
{
   BERR_Code retCode = BERR_SUCCESS;
   uint32_t mb, old;
   
   BAST_FTM_READ(FTM_PHY_CORR_CTL, &old);
   mb = old;
   if (bEnable)
      mb |= 0x01;
   else
      mb &= ~0x01;
   
   /* check if value changed */
   if (old ^ mb)
      BAST_FTM_WRITE(FTM_PHY_CORR_CTL, mb);
   
   return retCode;
}


/******************************************************************************
 BAST_g2_Ftm_P_EnableHwAssist()
******************************************************************************/
BERR_Code BAST_g2_Ftm_P_EnableHwAssist(BAST_Handle h, bool bEnable)
{
   BERR_Code retCode = BERR_SUCCESS;
   uint32_t mb, old, sts;
   
   BAST_FTM_READ(FTM_PHY_ASSIST_CTL, &old);
   mb = old;
   
   if (bEnable)
      mb &= ~0x01;
   else
      mb |= 0x01;
   
   /* check if value changed */
   if (old ^ mb)
      BAST_FTM_WRITE(FTM_PHY_ASSIST_CTL, mb);
   
   if (!bEnable)
   {
      /* check hw assist status if disabling */
      BAST_FTM_READ(FTM_PHY_ASSIST_STS, &sts);
      if ((sts & 1) == 0)
      {
         BDBG_MSG(("Unable to stop HW assist\n"));
         retCode = BERR_UNKNOWN;
      }
   }
   
   return retCode;
}


/******************************************************************************
 BAST_g2_Ftm_P_DoWhenHwAssistIdle_isr() - ISR context
******************************************************************************/
BERR_Code BAST_g2_Ftm_P_DoWhenHwAssistIdle_isr(BAST_Handle h, BAST_FTM_ISR idle_funct)
{
   BAST_g2_P_FtmDevice *hFtm = (BAST_g2_P_FtmDevice *) &(((BAST_g2_P_Handle *)(h->pImpl))->hFtmDev);
   BERR_Code retCode = BERR_SUCCESS;
   uint32_t ctl, sts;
   
   hFtm->idle_funct = NULL;
   
   /* check if hw assist totally stopped */
   BAST_FTM_READ(FTM_PHY_ASSIST_STS, &sts);
   if (sts & 0x100)
   {
      hw_assist_is_idle:
      retCode = idle_funct(h);
   }
   else
   {
      /* request hw assist to stop */
      BAST_FTM_READ(FTM_PHY_ASSIST_CTL, &ctl);
      ctl |= 0x01;
      BAST_FTM_WRITE(FTM_PHY_ASSIST_CTL, ctl);
      
      /* check if hw assist totally stopped */
      BAST_FTM_READ(FTM_PHY_ASSIST_STS, &sts);
      if (sts & 0x100)
         goto hw_assist_is_idle;
      else
         hFtm->idle_funct = idle_funct;
      
      /* enable hw assist idle interrupt */
      BAST_g2_Ftm_P_EnableInterrupt_isr(h, BAST_Ftm_IntID_eAssistIdle, true);
   }
   
   return retCode;
}


/******************************************************************************
 BAST_g2_Ftm_P_DoWhenHwAssistIdle() - Non-ISR context
******************************************************************************/
BERR_Code BAST_g2_Ftm_P_DoWhenHwAssistIdle(BAST_Handle h, BAST_FTM_ISR idle_funct)
{
   BERR_Code retCode;
   
   BKNI_EnterCriticalSection();
   retCode = BAST_g2_Ftm_P_DoWhenHwAssistIdle_isr(h, idle_funct);
   BKNI_LeaveCriticalSection();
   
   return retCode;
}


/******************************************************************************
BAST_g2_Ftm_P_IsHwAssistStopped()
******************************************************************************/
bool BAST_g2_Ftm_P_IsHwAssistStopped(BAST_Handle h)
{
   uint32_t mb;
   
   BAST_FTM_READ(FTM_PHY_ASSIST_STS, &mb);
   if (mb & 0x100)
      return true;
   
   return false;
}


/******************************************************************************
BAST_g2_Ftm_P_InterruptOnPreamble() - ISR context
******************************************************************************/
BERR_Code BAST_g2_Ftm_P_InterruptOnPreamble(BAST_Handle h)
{
   BERR_Code retCode = BERR_SUCCESS;
   
   /* clear and enable the interrupt */
   retCode = BAST_g2_Ftm_P_EnableInterrupt_isr(h, BAST_Ftm_IntID_eAssistLvl1, true);
   return retCode;
}


/******************************************************************************
 BAST_g2_Ftm_P_InterruptOnNetworkActivity() - ISR context
******************************************************************************/
BERR_Code BAST_g2_Ftm_P_InterruptOnNetworkActivity(BAST_Handle h)
{
   BERR_Code retCode = BERR_SUCCESS;
   
   /* clear and enable the interrupt */
   retCode = BAST_g2_Ftm_P_EnableInterrupt_isr(h, BAST_Ftm_IntID_eAssistLvl2, true);
   return retCode;
}


/******************************************************************************
 BAST_g2_Ftm_P_SetExcludeList() - hw assist assumed to be off
******************************************************************************/
BERR_Code BAST_g2_Ftm_P_SetExcludeList(BAST_Handle h, BAST_Ftm_Exclude excludePacket, bool bFilterOn)
{
   BERR_Code retCode = BERR_SUCCESS;
   uint32_t mb;
   
   BDBG_ASSERT(BAST_g2_Ftm_P_IsHwAssistStopped(h));
   
   BAST_FTM_READ(FTM_UART_ASSIST_PCTL2R, &mb);
   if (excludePacket == BAST_Ftm_Exclude_ePolls)
   {
      if (bFilterOn)
         mb |= 0x80;
      else
         mb &= ~0x80;
   }
   else if (excludePacket == BAST_Ftm_Exclude_eOffers)
   {
      if (bFilterOn)
         mb |= 0x8000;
      else
         mb &= ~0x8000;
   }
   BAST_FTM_WRITE(FTM_UART_ASSIST_PCTL2R, mb);
   
   return retCode;
}


/******************************************************************************
 BAST_g2_Ftm_P_StopTimer() - ISR context
******************************************************************************/
BERR_Code BAST_g2_Ftm_P_StopTimer(BAST_Handle h, BAST_Ftm_TimerSelect t)
{
   BAST_g2_P_FtmDevice *hFtm = (BAST_g2_P_FtmDevice *) &(((BAST_g2_P_Handle *)(h->pImpl))->hFtmDev);
   BERR_Code retCode = BERR_SUCCESS;
   uint32_t mb, mask;
   BAST_Ftm_IntID Timer_IntID;

   switch (t)
   {
      case BAST_Ftm_TimerSelect_eRx:
         mask = (1 << 20);
         Timer_IntID = BAST_Ftm_IntID_eTimer1;
         break;
      
      case BAST_Ftm_TimerSelect_eTx:
         mask = (1 << 19);
         Timer_IntID = BAST_Ftm_IntID_eTimer2;
         break;
      
      case BAST_Ftm_TimerSelect_eReg:
         mask = (1 << 18);
         Timer_IntID = BAST_Ftm_IntID_eTimer3;
         break;
      
      default:
         BDBG_MSG(("BAST_g2_Ftm_P_StopTimer() - should not get here!\n"));
         return BERR_UNKNOWN;
   }

   /* stop timer control */
   BAST_FTM_READ(FTM_PHY_CTL, &mb);
   mb &= ~mask;
   BAST_FTM_WRITE(FTM_PHY_CTL, mb);
   
   /* clear and disable the interrupt */
   BAST_Ftm_Interrupts[Timer_IntID].enable = false;
   retCode = BINT_ClearCallback_isr(hFtm->hCallback[Timer_IntID]);
   BDBG_ASSERT(retCode == BERR_SUCCESS);
   BINT_DisableCallback_isr(hFtm->hCallback[Timer_IntID]);
   BDBG_ASSERT(retCode == BERR_SUCCESS);

   return retCode;
}


/******************************************************************************
 BAST_g2_Ftm_P_StartTimer() - ISR context
******************************************************************************/
BERR_Code BAST_g2_Ftm_P_StartTimer(BAST_Handle h, BAST_Ftm_TimerSelect t, uint32_t count)
{
   BAST_g2_P_FtmDevice *hFtm = (BAST_g2_P_FtmDevice *) &(((BAST_g2_P_Handle *)(h->pImpl))->hFtmDev);
   BERR_Code retCode = BERR_SUCCESS;
   uint32_t mask, mb;
   BAST_Ftm_IntID Timer_IntID;

   /* stop timer */
   BAST_CHK_RETCODE(BAST_g2_Ftm_P_StopTimer(h, t));
      
   switch (t)
   {
      case BAST_Ftm_TimerSelect_eRx:
         mask = (1 << 20);
         Timer_IntID = BAST_Ftm_IntID_eTimer1;
         BAST_FTM_WRITE(FTM_PHY_TIMER1, count);  /* set requested time out */
         break;
      
      case BAST_Ftm_TimerSelect_eTx:
         mask = (1 << 19);
         Timer_IntID = BAST_Ftm_IntID_eTimer2;
         BAST_FTM_WRITE(FTM_PHY_TIMER2, count);  /* set requested time out */
         break;
      
      case BAST_Ftm_TimerSelect_eReg:
         mask = (1 << 18);
         Timer_IntID = BAST_Ftm_IntID_eTimer3;
         BAST_FTM_WRITE(FTM_PHY_TIMER3, count);  /* set requested time out */
         break;
      
      default:
         BDBG_MSG(("BAST_g2_Ftm_P_StartTimer() - should not get here!\n"));
         return BERR_UNKNOWN;
   }
   
   /* clear and enable the selected timer interrupt */
   BAST_Ftm_Interrupts[Timer_IntID].enable = true;
   retCode = BINT_ClearCallback_isr(hFtm->hCallback[Timer_IntID]);
   BDBG_ASSERT(retCode == BERR_SUCCESS);
   retCode = BINT_EnableCallback_isr(hFtm->hCallback[Timer_IntID]);
   BDBG_ASSERT(retCode == BERR_SUCCESS);
   
   /* enable selected timer */
   BAST_FTM_READ(FTM_PHY_CTL, &mb);
   mb |= mask;
   BAST_FTM_WRITE(FTM_PHY_CTL, mb);
   
   done:
   return retCode;
}

/******************************************************************************
 BAST_g2_Ftm_P_HoldErrorMessage() - holds an error message until timeout
******************************************************************************/
BERR_Code BAST_g2_Ftm_P_HoldErrorMessage(BAST_Handle h, uint8_t *pBuf, uint8_t len)
{
   BAST_g2_P_FtmDevice *hFtm = (BAST_g2_P_FtmDevice *) &(((BAST_g2_P_Handle *)(h->pImpl))->hFtmDev);
   
   /* forward latest error message on timeout */
   BKNI_Memcpy(hFtm->err_buffer, pBuf, len);
   hFtm->buf_status |= BAST_FTM_BUF_ERR_NOT_EMPTY;
   
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g2_Ftm_P_SendLocalMessage() - prepares a local message for the application
******************************************************************************/
BERR_Code BAST_g2_Ftm_P_SendLocalMessage(BAST_Handle h, uint8_t *pBuf, uint8_t len)
{
   BAST_g2_P_Handle *hDev = (BAST_g2_P_Handle *)(h->pImpl);
   BAST_g2_P_FtmDevice *hFtm = (BAST_g2_P_FtmDevice *) &(((BAST_g2_P_Handle *)(h->pImpl))->hFtmDev);
   
   if (hFtm->buf_status & BAST_FTM_BUF_HOST_NOT_EMPTY)
   {
      BDBG_MSG(("BAST_g2_Ftm_P_SendLocalMessage(): host buffer full\n"));
      hFtm->err_status |= BAST_FTM_ERR_HOST_MSG_DROPPED;
   }
   else
   {
      BKNI_Memcpy(hFtm->host_buffer, pBuf, len);
      hFtm->buf_status |= BAST_FTM_BUF_HOST_NOT_EMPTY;
      BKNI_SetEvent(hDev->hFtmEvent);
   }
   
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g2_Ftm_P_SendErrorMessage()
******************************************************************************/
BERR_Code BAST_g2_Ftm_P_SendErrorMessage(BAST_Handle h, uint8_t err)
{
   uint8_t buf[2];

   buf[0] = 0x81;
   buf[1] = err;
   
   return BAST_g2_Ftm_P_SendLocalMessage(h, buf, 2);
}


/******************************************************************************
 BAST_g2_Ftm_P_SendHardReset() - hw assist assumed to be off - ISR context
******************************************************************************/
BERR_Code BAST_g2_Ftm_P_SendHardReset(BAST_Handle h)
{
   BAST_g2_P_FtmDevice *hFtm = (BAST_g2_P_FtmDevice *) &(((BAST_g2_P_Handle *)(h->pImpl))->hFtmDev);
   BERR_Code retCode = BERR_SUCCESS;
   
   BDBG_ASSERT(BAST_g2_Ftm_P_IsHwAssistStopped(h));
   
   /* indicate hard reset in progress */
   hFtm->status |= BAST_FTM_STATUS_HARD_RESET;
   
   /* set to interrupt on network traffic detected */
   BAST_CHK_RETCODE(BAST_g2_Ftm_P_InterruptOnPreamble(h));
   
   /* remove reg offers from exclude list */
   BAST_g2_Ftm_P_SetExcludeList(h, BAST_Ftm_Exclude_eOffers, false);
   
   /* start timer, enable receiver, enable hw assist */
   BAST_CHK_RETCODE(BAST_g2_Ftm_P_StartTimer(h, BAST_Ftm_TimerSelect_eReg, BAST_FTM_TIMEOUT_HARD_RESET));
   BAST_CHK_RETCODE(BAST_g2_Ftm_P_EnableReceiver(h, true));
   BAST_CHK_RETCODE(BAST_g2_Ftm_P_EnableHwAssist(h, true));
   
   done:
   return retCode;
}


/******************************************************************************
BAST_g2_Ftm_P_ResetAutoTxBuffer()
******************************************************************************/
BERR_Code BAST_g2_Ftm_P_ResetAutoTxBuffer(BAST_Handle h)
{
   BAST_g2_P_FtmDevice *hFtm = (BAST_g2_P_FtmDevice *) &(((BAST_g2_P_Handle *)(h->pImpl))->hFtmDev);
   BERR_Code retCode = BERR_SUCCESS;
   uint32_t mb;
 
   /* reset special packet buffer */
   BAST_FTM_READ(FTM_UART_ASSIST_BRR, &mb);
   mb |= 0x00010000;
   BAST_FTM_WRITE(FTM_UART_ASSIST_BRR, mb);
   hFtm->sp_packet_src_addr = 0;
   
   return retCode; 
}


/******************************************************************************
 BAST_g2_Ftm_P_CheckCrc()
******************************************************************************/
BERR_Code BAST_g2_Ftm_P_CheckCrc(BAST_Handle h, uint8_t *pBuf, uint8_t len, bool *bValid)
{
   BERR_Code retCode = BERR_SUCCESS;
   uint32_t mb, i, data_crc_idx = 0;
   
   BDBG_ASSERT(BAST_g2_Ftm_P_IsHwAssistStopped(h));

   *bValid = false;
   for (i = 0; i < len; i++)
   {
      if ((i == 0) || (i == 5))
      {
         /* clear the tx crc */
         mb = 0x03;
         BAST_FTM_WRITE(FTM_PHY_UART_CRC_CTL, mb);
      }
      else if (i == 3)
      {
         /* find position of data crc */
         data_crc_idx = pBuf[3] + 4;
      }
      else if ((i == 4) || (i == data_crc_idx))
      {
         /* insert correct CRC */
         BAST_FTM_READ(FTM_PHY_UART_CRC_TX, &mb);
         if ((uint8_t)(mb & 0xFF) != pBuf[i])
         {
            *bValid = false;
            goto done;
         }
      }
      
      mb = pBuf[i];
      BAST_FTM_WRITE(FTM_UART_THR, mb);
   }
   
   *bValid = true;
   
   done:
   return retCode;
}


/******************************************************************************
 BAST_g2_Ftm_P_CheckOutgoingPacket() - hw assist assumed to be off - ISR context
******************************************************************************/
BERR_Code BAST_g2_Ftm_P_CheckOutgoingPacket(BAST_Handle h)
{
   BAST_g2_P_FtmDevice *hFtm = (BAST_g2_P_FtmDevice *) &(((BAST_g2_P_Handle *)(h->pImpl))->hFtmDev);
   BERR_Code retCode = BERR_SUCCESS;
   uint32_t mb;
   
   BDBG_ASSERT(BAST_g2_Ftm_P_IsHwAssistStopped(h));

   /* check if there is pending packet */
   if (hFtm->buf_status & BAST_FTM_BUF_TX_NOT_EMPTY)
   {
      /* check if src address still in rx bit mask */
      if ((hFtm->rx_bit_mask & (1 << (hFtm->tx_buffer[2] & 0x0F))) == 0)
      {
         /* clear statuses and send error message */
         hFtm->buf_status &= ~BAST_FTM_BUF_TX_NOT_EMPTY;
         hFtm->status &= ~(BAST_FTM_STATUS_REG_PENDING | BAST_FTM_STATUS_POLL_PENDING);
         BAST_CHK_RETCODE(BAST_g2_Ftm_P_SendErrorMessage(h, BAST_FTM_MSG_ERR_REREGISTER));
      }
   }
   else
   {
      /* check for pending packet in sp pkt buffer */
      BAST_FTM_READ(FTM_UART_ASSIST_BLVR, &mb);
      if (mb &= 0x7F00)
      {
         /* check if src address of sp pkt is still in rx bit mask */
         if ((hFtm->rx_bit_mask & (1 << (hFtm->sp_packet_src_addr & 0x0F))) == 0)
         {
            /* cancel pending tx packet since it is no longer in rx bit mask */
            BAST_CHK_RETCODE(BAST_g2_Ftm_P_ResetAutoTxBuffer(h));
            
            /* stop waiting for poll */
            BAST_CHK_RETCODE(BAST_g2_Ftm_P_StopTimer(h, BAST_Ftm_TimerSelect_eRx));
            
            /* clear statuses and send error message */
            hFtm->sp_packet_src_addr = 0;
            hFtm->buf_status &= ~BAST_FTM_BUF_TX_NOT_EMPTY;
            hFtm->status &= ~(BAST_FTM_STATUS_REG_PENDING | BAST_FTM_STATUS_POLL_PENDING);
            BAST_CHK_RETCODE(BAST_g2_Ftm_P_SendErrorMessage(h, BAST_FTM_MSG_ERR_REREGISTER));
         }
      }
   }
   
   /* forward rx bit mask to host if modified */
   if (hFtm->rx_packet_cmd != BAST_FTM_PKT_FTM_UP)
      retCode = BAST_g2_Ftm_P_GetRxBitMask(h);

   done:
   return retCode;
}


/******************************************************************************
 BAST_g2_Ftm_P_UpdateHeartBeat() - hw assist assumed to be off
******************************************************************************/
BERR_Code BAST_g2_Ftm_P_UpdateHeartBeat(BAST_Handle h, bool bValid)
{
   BAST_g2_P_FtmDevice *hFtm = (BAST_g2_P_FtmDevice *) &(((BAST_g2_P_Handle *)(h->pImpl))->hFtmDev);
   BERR_Code retCode = BERR_SUCCESS;
   uint32_t mb;
   
   BDBG_ASSERT(BAST_g2_Ftm_P_IsHwAssistStopped(h));
   
   /* reset auto heartbeat buffer */
   BAST_FTM_READ(FTM_UART_ASSIST_BRR, &mb);
   mb |= 0x00000100;
   BAST_FTM_WRITE(FTM_UART_ASSIST_BRR, mb);
   
   if (bValid)
   {
      /* preamble */
      mb = 0x55;
      BAST_FTM_WRITE(FTM_UART_ASSIST_HBBR, mb);
      BAST_FTM_WRITE(FTM_UART_ASSIST_HBBR, mb);
      mb = 0x0D;
      BAST_FTM_WRITE(FTM_UART_ASSIST_HBBR, mb);
      
      mb = BAST_FTM_PKT_HEARTBEAT;     /* cmd byte */   
      BAST_FTM_WRITE(FTM_UART_ASSIST_HBBR, mb);
      mb = 0x00;                       /* dest addr */
      BAST_FTM_WRITE(FTM_UART_ASSIST_HBBR, mb);
      mb = 0x07;                       /* payload length */
      BAST_FTM_WRITE(FTM_UART_ASSIST_HBBR, mb);
      
      /* receiver id */
      mb = (hFtm->rid >> 24) & 0xFF;
      BAST_FTM_WRITE(FTM_UART_ASSIST_HBBR, mb);
      mb = (hFtm->rid >> 16) & 0xFF;   
      BAST_FTM_WRITE(FTM_UART_ASSIST_HBBR, mb);
      mb = (hFtm->rid >> 8) & 0xFF;
      BAST_FTM_WRITE(FTM_UART_ASSIST_HBBR, mb);
      mb = hFtm->rid & 0xFF;
      BAST_FTM_WRITE(FTM_UART_ASSIST_HBBR, mb);
      
      /* rx bit mask */
      mb = (hFtm->rx_bit_mask >> 8) & 0xFF;
      BAST_FTM_WRITE(FTM_UART_ASSIST_HBBR, mb);
      mb = (hFtm->rx_bit_mask & 0xFF);
      BAST_FTM_WRITE(FTM_UART_ASSIST_HBBR, mb);
   }

   return retCode;
}


/******************************************************************************
 BAST_g2_Ftm_P_GetRxBitMask()
******************************************************************************/
BERR_Code BAST_g2_Ftm_P_GetRxBitMask(BAST_Handle h)
{
   BERR_Code retCode = BERR_SUCCESS;
   uint8_t buf[4];
   uint32_t mb;
   
   /* read rx bit mask */
   BAST_FTM_READ(FTM_PHY_ASSIST_STS, &mb);
   mb = (mb >> 16) & 0x0000FFFF;
   
   /* build local response */
   buf[0] = 0x83;
   buf[1] = BAST_FTM_MSG_GET_RX_BIT_MASK;
   buf[2] = (mb >> 8) & 0xFF;
   buf[3] = (mb & 0xFF);
   retCode = BAST_g2_Ftm_P_SendLocalMessage(h, buf, 4);
   
   return retCode;   
}


/******************************************************************************
 BAST_g2_Ftm_P_SetRxBitMask() - hw assist assumed to be off - ISR context
******************************************************************************/
BERR_Code BAST_g2_Ftm_P_SetRxBitMask(BAST_Handle h)
{
   BAST_g2_P_FtmDevice *hFtm = (BAST_g2_P_FtmDevice *) &(((BAST_g2_P_Handle *)(h->pImpl))->hFtmDev);
   BERR_Code retCode = BERR_SUCCESS;
   uint32_t mb;
   bool bEnableRcvr;
   
   BDBG_ASSERT(BAST_g2_Ftm_P_IsHwAssistStopped(h));
   
   /* enable receiver if tuner registered */
   bEnableRcvr = (hFtm->rx_bit_mask & 0x7FFC) ? true : false;
   
   /* set new rx bit mask */
   BAST_FTM_READ(FTM_UART_ASSIST_PCTL1R, &mb);
   mb &= 0x0000FFFF;
   mb |= (hFtm->rx_bit_mask << 16);
   BAST_FTM_WRITE(FTM_UART_ASSIST_PCTL1R, mb);
   
   /* check pending tx packet's src_addr */
   BAST_CHK_RETCODE(BAST_g2_Ftm_P_CheckOutgoingPacket(h));
   BAST_CHK_RETCODE(BAST_g2_Ftm_P_UpdateHeartBeat(h, (hFtm->rx_bit_mask & 0x7FFC) ? true : false));
   /* BAST_CHK_RETCODE(BAST_g2_Ftm_P_EnableReceiver(h, bEnableRcvr)); */
   
   done:
   return retCode;   
}


/******************************************************************************
 BAST_g2_Ftm_P_SetOptions() - hw assist assumed to be off - ISR context
******************************************************************************/
BERR_Code BAST_g2_Ftm_P_SetOptions(BAST_Handle h)
{
   BAST_g2_P_FtmDevice *hFtm = (BAST_g2_P_FtmDevice *) &(((BAST_g2_P_Handle *)(h->pImpl))->hFtmDev);
   BERR_Code retCode = BERR_SUCCESS;
   uint32_t mb, mb2;
   
   BDBG_ASSERT(BAST_g2_Ftm_P_IsHwAssistStopped(h));
   
   /* handle carrier on option immediately */
   BAST_FTM_READ(FTM_PHY_TX_CTL, &mb);
   hFtm->options = hFtm->req_options;
   if (hFtm->options & BAST_FTM_OPTION_CARRIER_ON)
   {
      if ((mb & 0x02) == 0)
      {
         mb |= 0x02;  /* enable manual tx */
         BAST_FTM_WRITE(FTM_PHY_TX_CTL, mb);
         BAST_FTM_READ(FTM_PHY_TX_MAN, &mb);
         mb |= 0x11;  /* turn on carrier */
         BAST_FTM_WRITE(FTM_PHY_TX_MAN, mb);
      }
   }
   else
   {
      if (mb & 0x02)
      {
         BAST_FTM_READ(FTM_PHY_TX_MAN, &mb2);
         mb2 &= ~0x11;  /* turn off carrier */
         BAST_FTM_WRITE(FTM_PHY_TX_MAN, mb2);
         mb &= ~0x02;  /* disable manual tx */
         BAST_FTM_WRITE(FTM_PHY_TX_CTL, mb);
      }
   }
   
   if ((hFtm->options & BAST_FTM_OPTION_PREAMBLE_DETECTED) == 0)
   {
      /* wait for preamble detection */
      BAST_CHK_RETCODE(BAST_g2_Ftm_P_InterruptOnPreamble(h));
   }
   
   if ((hFtm->options & BAST_FTM_OPTION_NETWORK_ACTIVITY) == 0)
   {
      /* wait for network activity */
      BAST_CHK_RETCODE(BAST_g2_Ftm_P_InterruptOnNetworkActivity(h));
   }
   
   if (hFtm->options & BAST_FTM_OPTION_INSERT_CRC)
   {
      /* enable crc auto insertion */
      BAST_FTM_READ(FTM_UART_ASSIST_BRR, &mb);
      mb &= ~0x00020000;
      BAST_FTM_WRITE(FTM_UART_ASSIST_BRR, mb);
   }
   else
   {
      /* disable crc auto insertion */
      BAST_FTM_READ(FTM_UART_ASSIST_BRR, &mb);
      mb |= 0x00020000;
      BAST_FTM_WRITE(FTM_UART_ASSIST_BRR, mb);
   }
   
   /* handle promiscuous mode and non-polled mode */
   if (hFtm->options & (BAST_FTM_OPTION_PROMISCUOUS | BAST_FTM_OPTION_NON_POLLED))
   {
      /* lower filter level */
      BAST_FTM_READ(FTM_UART_ASSIST_PCTL1R, &mb);
      mb &= ~0x07;
      mb |= 0x02;    /* level 2 filtering: preamble + good crc */
      BAST_FTM_WRITE(FTM_UART_ASSIST_PCTL1R, mb);
   }
   else
   {
      /* default filter level */
      BAST_FTM_READ(FTM_UART_ASSIST_PCTL1R, &mb);
      mb &= ~0x07;
      mb |= 0x04;    /* level 4 filtering: preamble + good crc + rxbitmask + exclude list */
      BAST_FTM_WRITE(FTM_UART_ASSIST_PCTL1R, mb);
   }
   
   /* re-enable hw assist */
   retCode = BAST_g2_Ftm_P_EnableHwAssist(h, true);
   
   done:
   return retCode;
}


/******************************************************************************
 BAST_g2_Ftm_P_PrepareHardReset() - hw assist is assumed to be off
******************************************************************************/
BERR_Code BAST_g2_Ftm_P_PrepareHardReset(BAST_Handle h)
{
   BAST_g2_P_FtmDevice *hFtm = (BAST_g2_P_FtmDevice *) &(((BAST_g2_P_Handle *)(h->pImpl))->hFtmDev);
   uint32_t i;
   
   hFtm->tx_buffer[0] = 34; /* length */
   for (i = 0; i < 11; i++)
      hFtm->tx_buffer[1+i] = 0x55;
   for (i = 0; i < 11; i++)
      hFtm->tx_buffer[12+i] = 0xF0;
   for (i = 0; i < 11; i++)
      hFtm->tx_buffer[23+i] = 0x8C;
   hFtm->tx_buffer[34] = 0xFF;
   
   hFtm->buf_status |= BAST_FTM_BUF_TX_NOT_EMPTY;
   hFtm->status |= BAST_FTM_STATUS_START_TX;
   
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g2_Ftm_P_ManualTransmit() - hw assist assumed to be off - ISR context
******************************************************************************/
BERR_Code BAST_g2_Ftm_P_ManualTransmit(BAST_Handle h)
{
   BAST_g2_P_FtmDevice *hFtm = (BAST_g2_P_FtmDevice *) &(((BAST_g2_P_Handle *)(h->pImpl))->hFtmDev);
   BERR_Code retCode = BERR_SUCCESS;
   bool bHardReset;
   uint32_t i, mb, tx_ctl, data_crc_idx;
   
   BDBG_ASSERT(BAST_g2_Ftm_P_IsHwAssistStopped(h));
   
   /* identify hard reset */
   bHardReset = (hFtm->tx_buffer[1] == 0x55) && (hFtm->tx_buffer[2] == 0x55) && (hFtm->tx_buffer[3] == 0x55);
   
   /* disable the correlator while transmitting */
   BAST_CHK_RETCODE(BAST_g2_Ftm_P_EnableReceiver(h, false));
   
   /* disable tx state machine */
   BAST_FTM_READ(FTM_PHY_TX_CTL, &tx_ctl);
   tx_ctl &= ~0x00000001;
   BAST_FTM_WRITE(FTM_PHY_TX_CTL, tx_ctl);
   
   /* reset uart tx fifo */
   BAST_FTM_WRITE(FTM_UART_SRR, 0x04);
   
   /* set ftm statuses */
   hFtm->status |= BAST_FTM_STATUS_TX_IN_PROGRESS;
   hFtm->status &= ~BAST_FTM_STATUS_START_TX;
   
   if ((hFtm->status & BAST_FTM_STATUS_REG_PENDING) == 0)
      hFtm->buf_status &= ~BAST_FTM_BUF_TX_NOT_EMPTY;
   
   /* enable uart end interrupt */
   BAST_g2_Ftm_P_EnableInterrupt_isr(h, BAST_Ftm_IntID_eUartEnd, true);

   /* send out the preamble if necessary */
   if (!bHardReset)
   {
      /* transmit extra preamble byte if destination is swm */
      if (hFtm->tx_buffer[3] == 0)
      {
         mb = 0xAA;
         BAST_FTM_WRITE(FTM_UART_THR, mb);
      }
      mb = 0x55;
      BAST_FTM_WRITE(FTM_UART_THR, mb);
      BAST_FTM_WRITE(FTM_UART_THR, mb);
      mb = 0x0D;
      BAST_FTM_WRITE(FTM_UART_THR, mb);
   }
   
   /* calculate location of data crc */
   data_crc_idx = hFtm->tx_buffer[4] + 4;
   
   /* first byte is the length of the packet to be transmitted*/
   for (i = 0; i < hFtm->tx_buffer[0]; i++)
   {
      if (!bHardReset && ((hFtm->options & BAST_FTM_OPTION_INSERT_CRC) || (hFtm->tx_buffer[1] == BAST_FTM_PKT_HEARTBEAT)))
      {
         if ((i == 0) || (i == 5))
         {
            /* clear the tx crc */
            mb = 0x03;
            BAST_FTM_WRITE(FTM_PHY_UART_CRC_CTL, mb);
         }
         else if ((i == 4) || (i == data_crc_idx))
         {
            /* insert correct CRC */
            BAST_FTM_READ(FTM_PHY_UART_CRC_TX, &mb); 
            hFtm->tx_buffer[1+i] = (uint8_t)(mb & 0x000000FF);
         }
      }
      
      /* add the transmit data byte to the tx fifo */
      mb = hFtm->tx_buffer[1+i] & 0x000000FF;
      BAST_FTM_WRITE(FTM_UART_THR, mb); 
   }
   
   /* add extra bytes to fifo */
   BAST_FTM_WRITE(FTM_UART_THR, 0xFF);
   BAST_FTM_WRITE(FTM_UART_THR, 0xFF);
   BAST_FTM_WRITE(FTM_UART_THR, 0xFF);
   
   /* specify packet size */
   tx_ctl &= ~0x0000FF00;
   if (bHardReset)
      tx_ctl |= (hFtm->tx_buffer[0] << 8);   /* set packet size for hard reset, without preamble */
   else if (hFtm->tx_buffer[3] == 0)
      tx_ctl |= ((hFtm->tx_buffer[0] + 4) << 8);   /* set packet size to include extended preamble */
   else
      tx_ctl |= ((hFtm->tx_buffer[0] + 3) << 8);   /* set packet size to include preamble */
   BAST_FTM_WRITE(FTM_PHY_TX_CTL, tx_ctl);

   /* enable tx fifo empty interrupt */
   BAST_g2_Ftm_P_EnableInterrupt_isr(h, BAST_Ftm_IntID_eUart, true);
   
   /* start the transmit */
   tx_ctl |= 0x00000001;
   BAST_FTM_WRITE(FTM_PHY_TX_CTL, tx_ctl);
   
   retCode = BAST_g2_Ftm_P_StartTimer(h, BAST_Ftm_TimerSelect_eTx, 24000);  /* 17.2ms for max packet + 5.1ms precarrier */
   
   done:
   return retCode;
}


/******************************************************************************
 BAST_g2_Ftm_P_UartFifoEmpty() - ISR context
******************************************************************************/
BERR_Code BAST_g2_Ftm_P_UartFifoEmpty(BAST_Handle h)
{
   uint8_t i;
   
   for (i = 0; i < 64; i++)
   {
      /* stuff extra bytes in fifo */
      BAST_FTM_WRITE(FTM_UART_THR, 0xFF);
   }
   
   /* re-enable tx fifo empty interrupt */
   BAST_g2_Ftm_P_EnableInterrupt_isr(h, BAST_Ftm_IntID_eUart, true);
   
   return BERR_SUCCESS;
}


/******************************************************************************
 BAST_g2_Ftm_P_AutoTransmit() - hw assist assumed to be off
******************************************************************************/
BERR_Code BAST_g2_Ftm_P_AutoTransmit(BAST_Handle h)
{
   BAST_g2_P_FtmDevice *hFtm = (BAST_g2_P_FtmDevice *) &(((BAST_g2_P_Handle *)(h->pImpl))->hFtmDev);
   BERR_Code retCode = BERR_SUCCESS;
   uint32_t i, mb;
   
   BDBG_ASSERT(BAST_g2_Ftm_P_IsHwAssistStopped(h));
   
   if (hFtm->buf_status & BAST_FTM_BUF_TX_NOT_EMPTY)
   {
      /* return if sp pkt buffer not empty */
      BAST_FTM_READ(FTM_UART_ASSIST_BLVR, &mb);
      if ((mb >> 8) & 0x7F)
         return retCode;
      
      /* invalidate the special packet buffer */
      BAST_CHK_RETCODE(BAST_g2_Ftm_P_ResetAutoTxBuffer(h));
      hFtm->sp_packet_src_addr = hFtm->tx_buffer[2];
      
      /* preamble */
      mb = 0x55;
      BAST_FTM_WRITE(FTM_UART_ASSIST_SPBR, mb);   
      BAST_FTM_WRITE(FTM_UART_ASSIST_SPBR, mb); 
      mb = 0x0D;  
      BAST_FTM_WRITE(FTM_UART_ASSIST_SPBR, mb);
      
      /* load header except crc */
      for (i = 1; i < 5; i++)
      {
         mb = hFtm->tx_buffer[i]; 
         BAST_FTM_WRITE(FTM_UART_ASSIST_SPBR, mb);
      }
      
      /* user crc if auto crc disabled */
      if ((hFtm->options & BAST_FTM_OPTION_INSERT_CRC) == 0)
      {
         mb = hFtm->tx_buffer[5];
         BAST_FTM_WRITE(FTM_UART_ASSIST_SPBR, mb);
      }
      
      /* load payload except crc */
      for (i = 6; (int)i < (5 + hFtm->tx_buffer[4]); i++)
      {
         mb = hFtm->tx_buffer[i];
         BAST_FTM_WRITE(FTM_UART_ASSIST_SPBR, mb);
      }
      
      /* user crc if auto crc disabled */
      if ((hFtm->options & BAST_FTM_OPTION_INSERT_CRC) == 0)
      {
         mb = hFtm->tx_buffer[5 + hFtm->tx_buffer[4]];
         BAST_FTM_WRITE(FTM_UART_ASSIST_SPBR, mb);
      }
      
      /* cancel manual transmission */
      hFtm->status &= ~BAST_FTM_STATUS_START_TX;
      
      if ((hFtm->status & BAST_FTM_STATUS_REG_PENDING) == 0)
         hFtm->buf_status &= ~BAST_FTM_BUF_TX_NOT_EMPTY; 
   }
   
   done:
   return retCode;   
}


/******************************************************************************
 BAST_g2_Ftm_P_RegRequestTransmit() - hw assist assumed to be off
******************************************************************************/
BERR_Code BAST_g2_Ftm_P_RegRequestTransmit(BAST_Handle h)
{
   BAST_g2_P_FtmDevice *hFtm = (BAST_g2_P_FtmDevice *) &(((BAST_g2_P_Handle *)(h->pImpl))->hFtmDev);
   BERR_Code retCode = BERR_SUCCESS;
   
   BDBG_ASSERT(BAST_g2_Ftm_P_IsHwAssistStopped(h));
   
   /* remove offers from exclude list if sending out reg request */
   BAST_CHK_RETCODE(BAST_g2_Ftm_P_SetExcludeList(h, BAST_Ftm_Exclude_eOffers, false));
   
   /* send reg request automatically on poll if registered */
   if (hFtm->rx_bit_mask & 0x7FFC)
      retCode = BAST_g2_Ftm_P_AutoTransmit(h);
   
   done:
   return retCode;
}


/******************************************************************************
 BAST_g2_Ftm_P_SendPacket() - Non-ISR context
******************************************************************************/
BERR_Code BAST_g2_Ftm_P_SendPacket(BAST_Handle h, uint8_t *pBuf, uint8_t len)
{
   BAST_g2_P_FtmDevice *hFtm = (BAST_g2_P_FtmDevice *) &(((BAST_g2_P_Handle *)(h->pImpl))->hFtmDev);
   BERR_Code retCode = BERR_SUCCESS;
   bool bCommit = false;
   
   /* check for busy conditions */
   if ((hFtm->status & BAST_FTM_STATUS_TX_BUSY) || (hFtm->buf_status & BAST_FTM_BUF_TX_NOT_EMPTY))
   {
      retCode = BAST_g2_Ftm_P_SendErrorMessage(h, BAST_FTM_MSG_ERR_BUSY);
      goto done;
   }
   
   if (hFtm->options & BAST_FTM_OPTION_NON_POLLED)
   {
      bCommit = true;
      hFtm->status |= BAST_FTM_STATUS_START_TX;
   }
   else
   {
      /* check if packet originates from a registered tuner */
      if (hFtm->rx_bit_mask & (1 << (pBuf[2] & 0x0F)))
      {
         /* handle registration requests differently */
         if (pBuf[1] == BAST_FTM_PKT_REG_REQUEST)
         {
            if ((hFtm->status & BAST_FTM_STATUS_REG_PENDING) == 0)
            {
               retCode = BAST_g2_Ftm_P_EnableReceiver(h, true);
               
               /* get receiver id */
               hFtm->rid = (pBuf[6] << 24) & 0xFF000000;
               hFtm->rid |= (pBuf[7] << 16) & 0x00FF0000;
               hFtm->rid |= (pBuf[8] << 8) & 0x0000FF00;
               hFtm->rid |= pBuf[9] & 0x000000FF;
               
               /* reset registration parameters */
               hFtm->num_offers = 0;
               hFtm->network_activity = 0;
               hFtm->reg_time_left = BAST_FTM_TIMEOUT_REG;
               
               /* setup timer to wait for registration offer */
               BKNI_EnterCriticalSection();
               hFtm->status |= BAST_FTM_STATUS_REG_REQ;
               retCode = BAST_g2_Ftm_P_StartTimer(h, BAST_Ftm_TimerSelect_eReg, BAST_FTM_TIMEOUT_REG);
               BKNI_LeaveCriticalSection();
               
               if (retCode == BERR_SUCCESS)
                  bCommit = true;
            }
            else
            {
               hFtm->buf_status &= ~BAST_FTM_BUF_TX_NOT_EMPTY;
               retCode = BAST_g2_Ftm_P_SendErrorMessage(h, BAST_FTM_MSG_ERR_BUSY);
            }
         }
         else
         {
            /* flag retransmit for xtune */
            if (pBuf[1] == BAST_FTM_PKT_XTUNE)
            {
               hFtm->status |= BAST_FTM_STATUS_XTUNE_RESP;
               
               /* flag network xtune request */
               if (pBuf[2] == 0x01)
                  hFtm->status |= BAST_FTM_STATUS_NETWORK_XTUNE_RESP;
            }
            
            /* setup timer to wait for poll */
            BKNI_EnterCriticalSection();
            hFtm->status |= BAST_FTM_STATUS_POLL_PENDING;
            retCode = BAST_g2_Ftm_P_StartTimer(h, BAST_Ftm_TimerSelect_eRx, BAST_FTM_TIMEOUT_RX);
            BKNI_LeaveCriticalSection();
            
            if (retCode == BERR_SUCCESS)
               bCommit = true;
         }
      }
      else
      {
         BDBG_MSG(("BAST_g2_Ftm_P_SendPacket - src address not in bit mask!\n"));
         hFtm->buf_status &= ~BAST_FTM_BUF_TX_NOT_EMPTY;
         retCode = BAST_g2_Ftm_P_SendErrorMessage(h, BAST_FTM_MSG_ERR_REREGISTER);
      }
   }
   
   if (bCommit)
   {
      /* protect tx buffer */
      hFtm->buf_status |= BAST_FTM_BUF_TX_NOT_EMPTY;
      BKNI_Memcpy(hFtm->tx_buffer, pBuf, len);
      
      if (hFtm->status & BAST_FTM_STATUS_START_TX)
      {
         /* transmit without poll for non-polled mode */
         BAST_CHK_RETCODE(BAST_g2_Ftm_P_DoWhenHwAssistIdle(h, BAST_g2_Ftm_P_ManualTransmit));
      }
      else
      {
         if (hFtm->status & BAST_FTM_STATUS_REG_REQ)
         {
            /* remove offer filter and use auto transmission only if registered i.e. receiving polls */
            BAST_CHK_RETCODE(BAST_g2_Ftm_P_DoWhenHwAssistIdle(h, BAST_g2_Ftm_P_RegRequestTransmit));
         }
         else
         {
            /* for all other network packets */
            BAST_CHK_RETCODE(BAST_g2_Ftm_P_DoWhenHwAssistIdle(h, BAST_g2_Ftm_P_AutoTransmit));
         }
         
         /* enable hw assist for auto packet transmission */
         retCode = BAST_g2_Ftm_P_EnableHwAssist(h, true);
      }
   }
   
   done:
   return retCode;
}


/******************************************************************************
 BAST_g2_Ftm_P_TransmitDone() - ISR context
******************************************************************************/
BERR_Code BAST_g2_Ftm_P_TransmitDone(BAST_Handle h)
{
   BAST_g2_P_FtmDevice *hFtm = (BAST_g2_P_FtmDevice *) &(((BAST_g2_P_Handle *)(h->pImpl))->hFtmDev);
   BERR_Code retCode = BERR_SUCCESS;
   uint32_t mb;
   
   BAST_CHK_RETCODE(BAST_g2_Ftm_P_StopTimer(h, BAST_Ftm_TimerSelect_eTx));
   
   /* disable tx fifo empty interrupt */
   BAST_g2_Ftm_P_EnableInterrupt_isr(h, BAST_Ftm_IntID_eUart, false);
   
   if (hFtm->status & BAST_FTM_STATUS_XTUNE_RESP)
   {
      /* prepare xtune request retransmit */
      hFtm->buf_status |= BAST_FTM_BUF_TX_NOT_EMPTY;
      retCode = BAST_g2_Ftm_P_AutoTransmit(h);
   }
   else
   {
      /* stop waiting for poll once packet transmitted */
      BAST_CHK_RETCODE(BAST_g2_Ftm_P_StopTimer(h, BAST_Ftm_TimerSelect_eRx));
   }
   
   /* clear transmit in progress status */
   hFtm->status &= ~BAST_FTM_STATUS_TX_IN_PROGRESS;
   
   /* check if sending hard reset */
   if (hFtm->status & BAST_FTM_STATUS_HARD_RESET)
      hFtm->status &= ~BAST_FTM_STATUS_HARD_RESET;
   
   /* disable tx state machine */
   BAST_FTM_READ(FTM_PHY_TX_CTL, &mb);
   mb &= ~0x00000001;
   BAST_FTM_WRITE(FTM_PHY_TX_CTL, mb);
   
   /* reset tx fifo */
   BAST_FTM_WRITE(FTM_UART_SRR, 0x04);
   
   /* wait additional 150ms for reg response if req transmitted just before timing out */
   if (hFtm->status & BAST_FTM_STATUS_REG_TIMEOUT)
   {
      hFtm->status &= ~BAST_FTM_STATUS_REG_TIMEOUT;
      
      hFtm->status |= BAST_FTM_STATUS_REG_RESP;
      BAST_CHK_RETCODE(BAST_g2_Ftm_P_StartTimer(h, BAST_Ftm_TimerSelect_eReg, 150000));
   }

   /* enable receiver since expecting response */
   BAST_g2_Ftm_P_EnableReceiver(h, true);
   
   done:
   return retCode;
}


/******************************************************************************
 BAST_g2_Ftm_P_NetworkPreambleDetected() - lvl1 isr - hw assist assumed to be  off - ISR context
******************************************************************************/
BERR_Code BAST_g2_Ftm_P_NetworkPreambleDetected(BAST_Handle h)
{
   BAST_g2_P_FtmDevice *hFtm = (BAST_g2_P_FtmDevice *) &(((BAST_g2_P_Handle *)(h->pImpl))->hFtmDev);
   BERR_Code retCode = BERR_SUCCESS;

   BDBG_ASSERT(BAST_g2_Ftm_P_IsHwAssistStopped(h));
      
   if (hFtm->status & BAST_FTM_STATUS_REG_WAIT)
   {
      hFtm->network_activity = 1;
      
      /* wait for next reg offer */
      hFtm->status &= ~BAST_FTM_STATUS_REG_WAIT;
      hFtm->status |= BAST_FTM_STATUS_REG_REQ;
      retCode = BAST_g2_Ftm_P_StartTimer(h, BAST_Ftm_TimerSelect_eReg, hFtm->reg_time_left);
      
      /* remove reg offers from exclude list */
      BAST_g2_Ftm_P_SetExcludeList(h, BAST_Ftm_Exclude_eOffers, false);
   }
   else if (hFtm->status & BAST_FTM_STATUS_HARD_RESET)
   {
      /* set to interrupt again on network traffic detected */
      BAST_CHK_RETCODE(BAST_g2_Ftm_P_InterruptOnPreamble(h));
      retCode = BAST_g2_Ftm_P_StartTimer(h, BAST_Ftm_TimerSelect_eReg, BAST_FTM_TIMEOUT_HARD_RESET);
   }
   
   done:
   return retCode;  
}


/******************************************************************************
 BAST_g2_Ftm_P_GetRandomWaitTime()
******************************************************************************/
BERR_Code BAST_g2_Ftm_P_GetRandomWaitTime(BAST_Handle h, uint8_t modulus, uint32_t *pTimeout)
{
   BERR_Code retCode = BERR_SUCCESS;
   uint32_t mb;

   /* read random value from linear feedback shift register */
   BAST_FTM_READ(FTM_PHY_LFSR, &mb);
   *pTimeout = (mb & modulus & 0xFF) * 10000;
    
   return retCode;
}


/******************************************************************************
 BAST_g2_Ftm_P_TerminateRegistration() - hw assist assumed to be off - ISR context
******************************************************************************/
BERR_Code BAST_g2_Ftm_P_TerminateRegistration(BAST_Handle h)
{
   BAST_g2_P_FtmDevice *hFtm = (BAST_g2_P_FtmDevice *) &(((BAST_g2_P_Handle *)(h->pImpl))->hFtmDev);
   BERR_Code retCode = BERR_SUCCESS;
   
   BDBG_ASSERT(BAST_g2_Ftm_P_IsHwAssistStopped(h));
   
   /* clear statuses */
   hFtm->buf_status &= ~BAST_FTM_BUF_TX_NOT_EMPTY;
   hFtm->status &= ~BAST_FTM_STATUS_REG_PENDING;
   
   /* terminate registration timer */
   retCode = BAST_g2_Ftm_P_StopTimer(h, BAST_Ftm_TimerSelect_eReg);
   
   /* re-enable offer exclusion */
   BAST_g2_Ftm_P_SetExcludeList(h, BAST_Ftm_Exclude_eOffers, true);
   
   return retCode;
}


/******************************************************************************
 BAST_g2_Ftm_P_RegistrationTimeout() - hw assist assumed to be off - ISR context
******************************************************************************/
BERR_Code BAST_g2_Ftm_P_RegistrationTimeout(BAST_Handle h)
{
   BAST_g2_P_Handle *hDev = (BAST_g2_P_Handle *)(h->pImpl);
   BAST_g2_P_FtmDevice *hFtm = (BAST_g2_P_FtmDevice *) &(((BAST_g2_P_Handle *)(h->pImpl))->hFtmDev);
   BERR_Code retCode = BERR_SUCCESS;
   
   BDBG_ASSERT(BAST_g2_Ftm_P_IsHwAssistStopped(h));
   
   /* terminate registration & invalidate special packet buffer */
   BAST_CHK_RETCODE(BAST_g2_Ftm_P_TerminateRegistration(h));
   BAST_CHK_RETCODE(BAST_g2_Ftm_P_ResetAutoTxBuffer(h));
   
   /* cancel manual tx */
   hFtm->status &= ~BAST_FTM_STATUS_START_TX;
   
   /* delay registration error if tx in progress */
   if (hFtm->status & BAST_FTM_STATUS_TX_IN_PROGRESS)
   {
      hFtm->status |= BAST_FTM_STATUS_REG_TIMEOUT;
   }
   else
   {
      /* send error message */
      if (hFtm->buf_status & BAST_FTM_BUF_ERR_NOT_EMPTY)
         BKNI_SetEvent(hDev->hFtmEvent);
      else
         retCode = BAST_g2_Ftm_P_SendErrorMessage(h, (hFtm->options & BAST_FTM_OPTION_NETWORK_ACTIVITY) ? BAST_FTM_MSG_ERR_NOT_AVAILABLE : BAST_FTM_MSG_ERR_NOT_DETECTED);
   }
   
   done:
   return retCode;
}


/******************************************************************************
 BAST_g2_Ftm_P_CancelXtuneRetransmit() - ISR context
******************************************************************************/
BERR_Code BAST_g2_Ftm_P_CancelXtuneRetransmit(BAST_Handle h)
{
   BAST_g2_P_FtmDevice *hFtm = (BAST_g2_P_FtmDevice *) &(((BAST_g2_P_Handle *)(h->pImpl))->hFtmDev);
   BERR_Code retCode = BERR_SUCCESS;
   
   /* cancel retransmit of xtune request */
   hFtm->status &= ~(BAST_FTM_STATUS_XTUNE_RESP | BAST_FTM_STATUS_NETWORK_XTUNE_RESP);
   hFtm->buf_status &= ~BAST_FTM_BUF_TX_NOT_EMPTY;
   BAST_CHK_RETCODE(BAST_g2_Ftm_P_ResetAutoTxBuffer(h));
   
   /* stop timer waiting for xtune request */
   retCode = BAST_g2_Ftm_P_StopTimer(h, BAST_Ftm_TimerSelect_eRx);  
   
   done:
   return retCode;
}


/******************************************************************************
 BAST_g2_Ftm_P_ProcessLocalCommand() - Non-ISR context
******************************************************************************/
BERR_Code BAST_g2_Ftm_P_ProcessLocalCommand(BAST_Handle h, uint8_t *pBuf, uint8_t len)
{
   BAST_g2_P_Handle *hDev = (BAST_g2_P_Handle *)(h->pImpl);
   BAST_g2_P_FtmDevice *hFtm = (BAST_g2_P_FtmDevice *) &(((BAST_g2_P_Handle *)(h->pImpl))->hFtmDev);
   BERR_Code retCode = BERR_SUCCESS;
   uint8_t buf[12], sb = 0;
   
   BSTD_UNUSED(len);
   
   /* decode local command */
   switch (pBuf[1])
   {
      case BAST_FTM_MSG_RESET_UC:
         if (pBuf[0] == 0x82)
         {
            hDev->bFtmLocalReset = true;
            retCode = BAST_g2_Ftm_P_DoWhenHwAssistIdle(h, BAST_g2_Ftm_P_InitFtm);
         }
         else
            goto invalid_msg;
         break;
         
      case BAST_FTM_MSG_RESET_FTM:
         if (pBuf[0] == 0x81)
         {
            if ((hFtm->status & (BAST_FTM_STATUS_HARD_RESET | BAST_FTM_STATUS_REG_PENDING)) || (hFtm->status & BAST_FTM_STATUS_POLL_PENDING))
            {
               /* reject hard reset request if pending hard reset */
               retCode = BAST_g2_Ftm_P_SendErrorMessage(h, BAST_FTM_MSG_ERR_BUSY);
            }
            else
               retCode = BAST_g2_Ftm_P_DoWhenHwAssistIdle(h, BAST_g2_Ftm_P_SendHardReset);
         }
         else
            goto invalid_msg;
         break;
         
      case BAST_FTM_MSG_GET_RX_BIT_MASK:       
         if (pBuf[0] == 0x81)
            retCode = BAST_g2_Ftm_P_GetRxBitMask(h);
         else
            goto invalid_msg;
         break;
         
      case BAST_FTM_MSG_SET_RX_BIT_MASK:
         if (pBuf[0] == 0x83)
         {
            hFtm->rx_bit_mask = (pBuf[2] << 8) | pBuf[3];   
            retCode = BAST_g2_Ftm_P_DoWhenHwAssistIdle(h, BAST_g2_Ftm_P_SetRxBitMask);
         }
         else
            goto invalid_msg;
         break;
         
      case BAST_FTM_MSG_GET_OPTIONS:
         if (pBuf[0] == 0x81)
         {
            buf[0] = 0x83;
            buf[1] = BAST_FTM_MSG_GET_OPTIONS;
            buf[2] = (hFtm->options >> 8) & 0xFF;
            buf[3] = (hFtm->options & 0xFF);
            retCode = BAST_g2_Ftm_P_SendLocalMessage(h, buf, 4);
         }
         else
            goto invalid_msg;
         break;
         
      case BAST_FTM_MSG_SET_OPTIONS:
         if (pBuf[0] == 0x83)
         {
            /* filter options that are not implemented */
            hFtm->req_options = (pBuf[2] << 8) | pBuf[3];
            hFtm->req_options &= BAST_FTM_OPTIONS_IMPLEMENTED;          
            retCode = BAST_g2_Ftm_P_DoWhenHwAssistIdle(h, BAST_g2_Ftm_P_SetOptions);
         }
         else
            goto invalid_msg;
         break;
         
      case BAST_FTM_MSG_GET_VERSION:
      case BAST_FTM_MSG_GET_EXT_VERSION:
         if (hFtm->err_status & 0x80)
            sb = 0xFF;
         if (pBuf[0] == 0x81)
         {
            buf[0] = 0x8A;
            buf[1] = pBuf[1]; /* response command is the same as request */
            buf[2] = 0x01;    /* current firmware image index */
            buf[3] = BAST_FTM_MAJOR_VERSION;
            buf[4] = BAST_FTM_MINOR_VERSION;
            buf[5] = BAST_FTM_BUILD_NUM >> 8;
            buf[6] = BAST_FTM_BUILD_NUM & 0xFF;
            buf[7] = sb;
            buf[8] = sb;
            buf[9] = sb;
            buf[10] = sb;
            retCode = BAST_g2_Ftm_P_SendLocalMessage(h, buf, 11);
         }
         else
            goto invalid_msg;
         break;

      default:
         invalid_msg:
         retCode = BAST_g2_Ftm_P_SendErrorMessage(h, BAST_FTM_MSG_ERR_INVALID_MSG);
         break;
   }
   
   return retCode;
}


/******************************************************************************
 BAST_g2_Ftm_P_ProcessRegOfferPacket() - hw assist assumed to be off - ISR context
******************************************************************************/
BERR_Code BAST_g2_Ftm_P_ProcessRegOfferPacket(BAST_Handle h)
{
   BAST_g2_P_FtmDevice *hFtm = (BAST_g2_P_FtmDevice *) &(((BAST_g2_P_Handle *)(h->pImpl))->hFtmDev);
   BERR_Code retCode = BERR_SUCCESS;
   uint32_t timeout, mb;
   
   BDBG_ASSERT(BAST_g2_Ftm_P_IsHwAssistStopped(h));

   if ((hFtm->options & (BAST_FTM_OPTION_PROMISCUOUS | BAST_FTM_OPTION_NON_POLLED)) == 0)
   {
      hFtm->bForwardPacket = false;
      
      if (hFtm->status & BAST_FTM_STATUS_REG_PENDING)
      {
         hFtm->num_offers++;
         hFtm->status &= ~(BAST_FTM_STATUS_REG_RESP | BAST_FTM_STATUS_REG_WAIT);
         hFtm->status |= BAST_FTM_STATUS_REG_REQ;
      }
      else
      {
         /* re-enable offer exclusion if no registration pending */
         BAST_g2_Ftm_P_SetExcludeList(h, BAST_Ftm_Exclude_eOffers, true);
      }
   }
   
   /* hard reset takes precedence */
   if (hFtm->status & BAST_FTM_STATUS_HARD_RESET)
   {
      /* disable level1 irq */
      BAST_g2_Ftm_P_EnableInterrupt_isr(h, BAST_Ftm_IntID_eAssistLvl1, false);
      
      BAST_CHK_RETCODE(BAST_g2_Ftm_P_StopTimer(h, BAST_Ftm_TimerSelect_eReg));     
      BAST_CHK_RETCODE(BAST_g2_Ftm_P_PrepareHardReset(h));
   }
   else if ((hFtm->status & BAST_FTM_STATUS_REG_REQ) &&
            (((hFtm->rx_bit_mask & 0x7FFC) == 0) || (hFtm->num_offers > 2)))
   {
      /* begin manual transmission only if not registered or auto transmission failed after several polling cycles */
      hFtm->network_activity = 0;
      
      /* check offer modulus */
      if (hFtm->rx_buffer[6])
      {
         /* modulo != 0 */
         hFtm->status &= ~BAST_FTM_STATUS_REG_REQ;
         BAST_CHK_RETCODE(BAST_g2_Ftm_P_GetRandomWaitTime(h, hFtm->rx_buffer[6], &timeout));
         
         BDBG_MSG(("got reg offer: mod=%d, wait=%d\n", hFtm->rx_buffer[6], timeout));
         
         if (timeout > 0)
         {
            /* set to interrupt on network traffic detected */
            BAST_CHK_RETCODE(BAST_g2_Ftm_P_InterruptOnPreamble(h));
            
            /* check how much time is left for registration */
            BAST_FTM_READ(FTM_PHY_TIMER3, &mb);
            hFtm->reg_time_left = mb;
            /*BDBG_MSG(("reg_time_left=%d", hFtm->reg_time_left));*/
            if (hFtm->reg_time_left > timeout)
               hFtm->reg_time_left -= timeout;
            else
            {
               /* no more time left for registration */
               BAST_CHK_RETCODE(BAST_g2_Ftm_P_RegistrationTimeout(h));
               return retCode;
            }
            
            /* setup timer to wait for random time for no traffic */
            hFtm->status |= BAST_FTM_STATUS_REG_WAIT;
            retCode = BAST_g2_Ftm_P_StartTimer(h, BAST_Ftm_TimerSelect_eReg, timeout);
         }
         else
         {
            /* invalidate the special packet buffer */
            BAST_CHK_RETCODE(BAST_g2_Ftm_P_ResetAutoTxBuffer(h));
            
            /* transmit the reg req now and expect reg resp */
            hFtm->status |= (BAST_FTM_STATUS_REG_RESP | BAST_FTM_STATUS_START_TX);
         }
      }
      else
      {
         /* cancel registration */
         retCode = BAST_g2_Ftm_P_TerminateRegistration(h);
         
         /* construct a "no more addresses" pkt to host */
         BDBG_MSG(("no more addresses\n"));
         hFtm->rx_buffer[0] = 5;
         hFtm->rx_buffer[1] = BAST_FTM_PKT_NO_MORE_ADDRESSES;
         hFtm->rx_buffer[2] = 0x00;
         hFtm->rx_buffer[3] = 0x0F;
         hFtm->rx_buffer[4] = 0x00;
         hFtm->rx_buffer[5] = 0xEB;
         hFtm->bForwardPacket = true;
      }
   }
   
   done:
   return retCode;
}


/******************************************************************************
 BAST_g2_Ftm_P_ProcessRegRespPacket() - hw assist assumed to be off
******************************************************************************/
BERR_Code BAST_g2_Ftm_P_ProcessRegRespPacket(BAST_Handle h)
{
   BAST_g2_P_FtmDevice *hFtm = (BAST_g2_P_FtmDevice *) &(((BAST_g2_P_Handle *)(h->pImpl))->hFtmDev);
   BERR_Code retCode = BERR_SUCCESS;
   uint32_t rid, idx, mb;
   
   BDBG_ASSERT(BAST_g2_Ftm_P_IsHwAssistStopped(h));
   
   /* get receiver id */
   rid = (hFtm->rx_buffer[7] << 24) | (hFtm->rx_buffer[8] << 16) | (hFtm->rx_buffer[9] << 8) | (hFtm->rx_buffer[10]);
   
   if (rid == hFtm->rid)
   {
      /* forward reg response to host */
      hFtm->bForwardPacket = true;
      
      /* clear statuses and stop reg timer */
      BAST_CHK_RETCODE(BAST_g2_Ftm_P_TerminateRegistration(h));
      
      /* update rx bit mask */
      idx = hFtm->rx_buffer[6] & 0x0F;
      hFtm->rx_bit_mask |= (1 << idx);
      
      /* manually set rx bit mask in hardware since no need to check outgoing packet */
      BAST_FTM_READ(FTM_UART_ASSIST_PCTL1R, &mb);
      mb &= 0x0000FFFF;
      mb |= (hFtm->rx_bit_mask << 16);
      BAST_FTM_WRITE(FTM_UART_ASSIST_PCTL1R, mb);
      
      /* update auto heartbeat packet */
      retCode = BAST_g2_Ftm_P_UpdateHeartBeat(h, (hFtm->rx_bit_mask & 0x7FFC) ? true : false);
   }
   else
   {
      BDBG_MSG(("unexpected RID in reg resp (0x%08X)\n", rid));
      hFtm->bForwardPacket = false;
   }

   done:
   return retCode;
}


/******************************************************************************
 BAST_g2_Ftm_P_ProcessFtmUpPacket() - hw assist assumed to be off - ISR context
******************************************************************************/
BERR_Code BAST_g2_Ftm_P_ProcessFtmUpPacket(BAST_Handle h)
{
   BAST_g2_P_FtmDevice *hFtm = (BAST_g2_P_FtmDevice *) &(((BAST_g2_P_Handle *)(h->pImpl))->hFtmDev);
   BERR_Code retCode = BERR_SUCCESS;
   
   BDBG_ASSERT(BAST_g2_Ftm_P_IsHwAssistStopped(h));
   BDBG_MSG(("got ftm up pkt\n"));
   
   /* cancel pending packets after successful hard reset */
   if (hFtm->status & BAST_FTM_STATUS_REG_PENDING)
   {
      BAST_CHK_RETCODE(BAST_g2_Ftm_P_RegistrationTimeout(h));
   }
   if (hFtm->status & BAST_FTM_STATUS_XTUNE_RESP)
   {
      /* cancel retransmit of xtune request */
      BAST_CHK_RETCODE(BAST_g2_Ftm_P_CancelXtuneRetransmit(h));
      BAST_CHK_RETCODE(BAST_g2_Ftm_P_SendErrorMessage(h, (hFtm->options & BAST_FTM_OPTION_NETWORK_ACTIVITY) ? BAST_FTM_MSG_ERR_NOT_AVAILABLE : BAST_FTM_MSG_ERR_NOT_DETECTED));
   }
   if (hFtm->status & BAST_FTM_STATUS_HARD_RESET)
   {
      /* re-enable offer exclusion */
      BAST_CHK_RETCODE(BAST_g2_Ftm_P_SetExcludeList(h, BAST_Ftm_Exclude_eOffers, true));
      
      hFtm->status &= ~BAST_FTM_STATUS_HARD_RESET;
      BAST_CHK_RETCODE(BAST_g2_Ftm_P_StopTimer(h, BAST_Ftm_TimerSelect_eReg));
   }
   
   /* reset ftm */
   BAST_CHK_RETCODE(BAST_g2_Ftm_P_InitFtm(h));
   
   done:
   return retCode;
}


/******************************************************************************
 BAST_g2_Ftm_P_ProcessPingPacket() - ISR context
******************************************************************************/
BERR_Code BAST_g2_Ftm_P_ProcessPingPacket(BAST_Handle h)
{
   BAST_g2_P_FtmDevice *hFtm = (BAST_g2_P_FtmDevice *) &(((BAST_g2_P_Handle *)(h->pImpl))->hFtmDev);
   BERR_Code retCode = BERR_SUCCESS;
   uint32_t i, mb;
   
   if ((hFtm->rx_packet_cmd & BAST_FTM_PKT_RESP_FLAG) == 0)  /* check for response flag = 0 */
   {
      if ((hFtm->options & (BAST_FTM_OPTION_PROMISCUOUS | BAST_FTM_OPTION_NON_POLLED)) == 0)
         hFtm->bForwardPacket = false;
      
      /* check if special packet buffer empty */
      BAST_FTM_READ(FTM_UART_ASSIST_BLVR, &mb);
      if (((mb >> 8) & 0x7F) == 0)
      {
         /* sp pkt buffer in hardware is empty */
         BAST_CHK_RETCODE(BAST_g2_Ftm_P_ResetAutoTxBuffer(h));
         
         /* preamble */
         mb = 0x55;
         BAST_FTM_WRITE(FTM_UART_ASSIST_SPBR, mb);
         BAST_FTM_WRITE(FTM_UART_ASSIST_SPBR, mb);
         mb = 0x0D;
         BAST_FTM_WRITE(FTM_UART_ASSIST_SPBR, mb);
         
         /* header except crc */
         mb = hFtm->rx_buffer[1] | BAST_FTM_PKT_RESP_FLAG;  /* set response flag */
         BAST_FTM_WRITE(FTM_UART_ASSIST_SPBR, mb);
         mb = hFtm->rx_buffer[3];         /* switch source to destination address */
         BAST_FTM_WRITE(FTM_UART_ASSIST_SPBR, mb);
         mb = hFtm->rx_buffer[2];         /* switch destination to source address */
         BAST_FTM_WRITE(FTM_UART_ASSIST_SPBR, mb);
         mb = hFtm->rx_buffer[4];         /* payload length */
         BAST_FTM_WRITE(FTM_UART_ASSIST_SPBR, mb);
         
         /* temporarily enable crc auto insertion to send ping response */
         if ((hFtm->options & BAST_FTM_OPTION_INSERT_CRC) == 0)
         {
            hFtm->status |= BAST_FTM_STATUS_PING_RESP;
            BAST_FTM_READ(FTM_UART_ASSIST_BRR, &mb);
            mb &= ~0x00020000;
            BAST_FTM_WRITE(FTM_UART_ASSIST_BRR, mb);
         }
         
         /* payload except crc */
         for (i = 6; (int)i < (5 +  hFtm->rx_buffer[4]); i++)
         {
            mb = hFtm->rx_buffer[i];
            BAST_FTM_WRITE(FTM_UART_ASSIST_SPBR, mb);
         }
         
         /* setup timer to wait for poll for transmission */
         retCode = BAST_g2_Ftm_P_StartTimer(h, BAST_Ftm_TimerSelect_eRx, BAST_FTM_TIMEOUT_RX);
      }
      else if ((hFtm->buf_status & BAST_FTM_BUF_TX_NOT_EMPTY) == 0)
      {
         BKNI_Memcpy(hFtm->tx_buffer, hFtm->rx_buffer, hFtm->rx_buffer[0] + 1);
         hFtm->tx_buffer[1] |= BAST_FTM_PKT_RESP_FLAG;   /* set response flag */
         hFtm->tx_buffer[2] = hFtm->rx_buffer[3];        /* switch source to destination address */
         hFtm->tx_buffer[3] = hFtm->rx_buffer[2];        /* switch destination to source address */
         hFtm->buf_status |= BAST_FTM_BUF_TX_NOT_EMPTY;
         
         /* temporarily enable crc auto insertion to send ping response */
         if ((hFtm->options & BAST_FTM_OPTION_INSERT_CRC) == 0)
         {
            hFtm->status |= BAST_FTM_STATUS_PING_RESP;
            BAST_FTM_READ(FTM_UART_ASSIST_BRR, &mb);
            mb &= ~0x00020000;
            BAST_FTM_WRITE(FTM_UART_ASSIST_BRR, mb);
         }
      }
      else
      {
         /* cannot send now because tx buf currently has a pending packet */
         BAST_g2_Ftm_P_SendErrorMessage(h, BAST_FTM_MSG_ERR_BUSY);
      }
   }
   
   done:
   return retCode;
}


/******************************************************************************
 BAST_g2_Ftm_P_ProcessBadCrcNack()
******************************************************************************/
BERR_Code BAST_g2_Ftm_P_ProcessBadCrcNack(BAST_Handle h)
{
   BAST_g2_P_FtmDevice *hFtm = (BAST_g2_P_FtmDevice *) &(((BAST_g2_P_Handle *)(h->pImpl))->hFtmDev);
   BERR_Code retCode = BERR_SUCCESS;
   
   /* note broadcast error implies registration request sent */
   if (hFtm->rx_packet_dest_addr == 0x0F)
   {
      if (hFtm->status & BAST_FTM_STATUS_REG_PENDING)
      {
         /* ignore nak crc broadcast during registration and resend reg request - FTM spec 0.26 */
         hFtm->bForwardPacket = false;
         
         /* hold nak crc broadcast until reg timeout - FTM spec 0.27 */
         retCode = BAST_g2_Ftm_P_HoldErrorMessage(h, hFtm->rx_buffer, hFtm->rx_packet_data_len + 6);   /* include length byte */
      }
      else
      {
         /* ignore nak crc broadcast if not registering - FTM spec 0.27 */
         hFtm->bForwardPacket = false;
      }
   }
   else if (hFtm->status & BAST_FTM_STATUS_XTUNE_RESP)
   {
      /* ignore bad crc during xtune retry - FTM spec 0.26 */
      hFtm->bForwardPacket = false;
      
      /* hold nak crc broadcast until xtune timeout - FTM spec 0.27 */
      retCode = BAST_g2_Ftm_P_HoldErrorMessage(h, hFtm->rx_buffer, hFtm->rx_packet_data_len + 6);   /* include length byte */
   }
   else
   {
      /* forward packet to host, abort outgoing packet on error by default */
      hFtm->bForwardPacket = true;
   }
   
   return retCode;
}


/******************************************************************************
 BAST_g2_Ftm_P_ProcessHwFailNack() - ISR context
******************************************************************************/
BERR_Code BAST_g2_Ftm_P_ProcessHwFailNack(BAST_Handle h)
{
   BAST_g2_P_FtmDevice *hFtm = (BAST_g2_P_FtmDevice *) &(((BAST_g2_P_Handle *)(h->pImpl))->hFtmDev);
   BERR_Code retCode = BERR_SUCCESS;
   
   /* cancel pending xtune request and forward error to host - FTM spec 0.26 */
   if (hFtm->status & BAST_FTM_STATUS_XTUNE_RESP)
      retCode = BAST_g2_Ftm_P_CancelXtuneRetransmit(h);
   
   /* forward packet to host, abort outgoing packet on error */
   hFtm->bForwardPacket = true;
   
   return retCode;
}


/******************************************************************************
 BAST_g2_Ftm_P_ProcessBusyNack()
******************************************************************************/
BERR_Code BAST_g2_Ftm_P_ProcessBusyNack(BAST_Handle h)
{
   BAST_g2_P_FtmDevice *hFtm = (BAST_g2_P_FtmDevice *) &(((BAST_g2_P_Handle *)(h->pImpl))->hFtmDev);
   BERR_Code retCode = BERR_SUCCESS;
   
   if (((hFtm->status & BAST_FTM_STATUS_REG_PENDING) == 0) && ((hFtm->status & BAST_FTM_STATUS_POLL_PENDING) == 0))
   {
      /* ignore busy message if no pending packets */
      hFtm->bForwardPacket = false;
   }
   
   return retCode;
}


/******************************************************************************
 BAST_g2_Ftm_P_ProcessMyPacket() - hw assist assumed to be off - ISR context
******************************************************************************/
BERR_Code BAST_g2_Ftm_P_ProcessMyPacket(BAST_Handle h)
{
   BAST_g2_P_Handle *hDev = (BAST_g2_P_Handle *)(h->pImpl);
   BAST_g2_P_FtmDevice *hFtm = (BAST_g2_P_FtmDevice *) &(((BAST_g2_P_Handle *)(h->pImpl))->hFtmDev);
   BERR_Code retCode = BERR_SUCCESS;
   uint32_t mb, packet_len;
   uint8_t i, *pBuf;
   bool bPacketSentLast;
   
   BDBG_ASSERT(BAST_g2_Ftm_P_IsHwAssistStopped(h));
   
   /* forward packet to host by default */
   hFtm->bForwardPacket = true;
   
   /* get packet length */
   BAST_FTM_READ(FTM_UART_ASSIST_BLVR, &mb);
   packet_len = (mb >> 16) & 0x7F;
   
   /* read packet header */
   BAST_FTM_READ(FTM_UART_ASSIST_STBR, &mb);
   hFtm->rx_packet_cmd = mb & 0xFF;
   BAST_FTM_READ(FTM_UART_ASSIST_STBR, &mb);
   hFtm->rx_packet_src_addr = mb & 0xFF;
   BAST_FTM_READ(FTM_UART_ASSIST_STBR, &mb);
   hFtm->rx_packet_dest_addr = mb & 0xFF;
   BAST_FTM_READ(FTM_UART_ASSIST_STBR, &mb);
   hFtm->rx_packet_data_len = mb & 0xFF;
   BAST_FTM_READ(FTM_UART_ASSIST_STBR, &mb);
   hFtm->rx_packet_header_crc = mb & 0xFF;
   
   /* verify packet length */
   if ((int)packet_len != (5 +  hFtm->rx_packet_data_len))
   {
      BDBG_MSG(("BAST_g2_Ftm_P_ProcessMyPacket(): incorrect packet_len!"));
      return retCode;
   }
   
   /* abort processing if receive buffer not empty */
   if (hFtm->buf_status & BAST_FTM_BUF_RX_NOT_EMPTY)
   {
      BDBG_MSG(("BAST_g2_Ftm_P_ProcessMyPacket(): rx buffer full!"));
      hFtm->err_status |= BAST_FTM_ERR_RX_PKT_DROPPED;
      return retCode;
   }

   /* additional software filters when not in promiscuous mode */
   if ((hFtm->options & (BAST_FTM_OPTION_PROMISCUOUS | BAST_FTM_OPTION_NON_POLLED)) == 0)
   {
      /* ignore reg response broadcast if not registering */
      if ((hFtm->rx_packet_cmd == BAST_FTM_PKT_REG_RESPONSE) && ((hFtm->status & BAST_FTM_STATUS_REG_PENDING) == 0))
         return retCode;
         
      /* polls and heartbeats should never be forwarded */
      if ((hFtm->rx_packet_cmd == BAST_FTM_PKT_POLL) || (hFtm->rx_packet_cmd == BAST_FTM_PKT_HEARTBEAT))
         return retCode;
      
      /* filter out packets not in rx bit mask */
      if ((hFtm->rx_bit_mask & (1 << (hFtm->rx_packet_dest_addr & 0x0F))) == 0)
         return retCode;
      
      if (hFtm->rx_packet_cmd == BAST_FTM_PKT_XTUNE_RESP)
      {
         /* ignore xtune response if not expected */
         if ((hFtm->status & BAST_FTM_STATUS_XTUNE_RESP) == 0)
            return retCode;
         
         /* ignore network xtune response if not expected */
         if (((hFtm->status & BAST_FTM_STATUS_NETWORK_XTUNE_RESP) == 0) && (hFtm->rx_packet_dest_addr == 0x01))
            return retCode;
      }
   }
   
   /* copy the header into rx buffer */
   pBuf = hFtm->rx_buffer;
   *pBuf++ = 5 + hFtm->rx_packet_data_len;
   *pBuf++ = hFtm->rx_packet_cmd;
   *pBuf++ = hFtm->rx_packet_src_addr;
   *pBuf++ = hFtm->rx_packet_dest_addr;
   *pBuf++ = hFtm->rx_packet_data_len;
   *pBuf++ = hFtm->rx_packet_header_crc;
   
   /* copy the data into rx_buffer */
   for (i = 0; i < hFtm->rx_packet_data_len; i++)
   {
      BAST_FTM_READ(FTM_UART_ASSIST_STBR, &mb);
      *pBuf++ = mb & 0xFF;
   }
   
   /* ftm error processing */
   if ((hFtm->rx_packet_cmd & 0x30) == 0x30)
   {
      BDBG_MSG(("FTM error: %02X(%02X|%02X|%02X)\n", packet_len, hFtm->rx_packet_cmd, hFtm->rx_packet_src_addr, hFtm->rx_packet_dest_addr));
      
      switch (hFtm->rx_packet_cmd & 0x3F)
      {
         case BAST_FTM_PKT_NACK_BAD_CRC:
            retCode = BAST_g2_Ftm_P_ProcessBadCrcNack(h);
            break;
         
         case BAST_FTM_PKT_NACK_HW_FAIL:
            retCode = BAST_g2_Ftm_P_ProcessHwFailNack(h);
            break;
         
         case BAST_FTM_PKT_NACK_BUSY:
            retCode = BAST_g2_Ftm_P_ProcessBusyNack(h);
            break;
         
         default:
            /* forward packet to host on default error */
            hFtm->bForwardPacket = true;
      }
      
      /* check destination address for broadcast */
      if (hFtm->rx_packet_dest_addr == 0x0F)
      {
         BDBG_MSG(("broadcast error\n"));
         BAST_FTM_READ(FTM_PHY_ASSIST_STS, &mb);
         bPacketSentLast = (mb & 0x0200) >> 9;
         
         /* ignore broadcast error if we did not send packet */
         if (!bPacketSentLast)
            hFtm->bForwardPacket = false;
      }
      
      if (hFtm->bForwardPacket)
      {
         /* At this point, we have one of these conditions:
                              1. this is not a broadcast packet
                              2. this is a broadcast packet and we transmitted last
                        Therefore, forward to host and cancel any pending packets */
         if ((hFtm->status & BAST_FTM_STATUS_POLL_PENDING) || (hFtm->status & BAST_FTM_STATUS_REG_PENDING))
         {
            /* abort pending for poll and stop poll timer */
            hFtm->status &= ~BAST_FTM_STATUS_POLL_PENDING;
            BAST_CHK_RETCODE(BAST_g2_Ftm_P_StopTimer(h, BAST_Ftm_TimerSelect_eRx));
            
            /* cancel pending packet for transmission */
            hFtm->buf_status &= ~BAST_FTM_BUF_TX_NOT_EMPTY;
            BAST_CHK_RETCODE(BAST_g2_Ftm_P_ResetAutoTxBuffer(h));
            
            /* clear statuses and expire timers */
            if (hFtm->status & BAST_FTM_STATUS_HARD_RESET)
            {
               /* re-enable offer exclusion */
               BAST_CHK_RETCODE(BAST_g2_Ftm_P_SetExcludeList(h, BAST_Ftm_Exclude_eOffers, true));
               
               hFtm->status &= ~BAST_FTM_STATUS_HARD_RESET;
               BAST_CHK_RETCODE(BAST_g2_Ftm_P_StopTimer(h, BAST_Ftm_TimerSelect_eReg));
               BDBG_MSG(("cancel pending hard reset\n"));
            }
            if (hFtm->status & BAST_FTM_STATUS_REG_PENDING)
            {
               BAST_CHK_RETCODE(BAST_g2_Ftm_P_TerminateRegistration(h));
               BDBG_MSG(("cancel pending registration\n"));
            }
         }
      }
   }
   else
   {
      switch (hFtm->rx_packet_cmd)
      {
         case BAST_FTM_PKT_REG_OFFER:
            retCode = BAST_g2_Ftm_P_ProcessRegOfferPacket(h);
            break;
         
         case BAST_FTM_PKT_REG_RESPONSE:
            retCode = BAST_g2_Ftm_P_ProcessRegRespPacket(h);
            break;
         
         case BAST_FTM_PKT_FTM_UP:
            retCode = BAST_g2_Ftm_P_ProcessFtmUpPacket(h);
            break;
         
         case BAST_FTM_PKT_PING:
            retCode = BAST_g2_Ftm_P_ProcessPingPacket(h);
            break;
         
         case BAST_FTM_PKT_XTUNE_RESP:
            retCode = BAST_g2_Ftm_P_CancelXtuneRetransmit(h);
      }
   }
   
   /* forward all packets when in promiscuous mode */
   if (hFtm->options & BAST_FTM_OPTION_PROMISCUOUS)
      hFtm->bForwardPacket = true;

   /* notify host if no error and if packet is to be forwarded */
   if (!retCode && hFtm->bForwardPacket)
   {
      /* clear pending errors */
      hFtm->buf_status &= ~BAST_FTM_BUF_ERR_NOT_EMPTY;
      
      hFtm->buf_status |= BAST_FTM_BUF_RX_NOT_EMPTY;
      BKNI_SetEvent(hDev->hFtmEvent);
   }

   done:
   return retCode;
}


/******************************************************************************
 BAST_g2_Ftm_P_BadRxCrc() - ISR context
******************************************************************************/
BERR_Code BAST_g2_Ftm_P_BadRxCrc(BAST_Handle h)
{
   BAST_g2_P_Handle *hDev = (BAST_g2_P_Handle *)(h->pImpl);
   BAST_g2_P_FtmDevice *hFtm = (BAST_g2_P_FtmDevice *) &(((BAST_g2_P_Handle *)(h->pImpl))->hFtmDev);
   BERR_Code retCode = BERR_SUCCESS;
   uint32_t mb, bad_packet_len;
   uint8_t i, *pBuf;
   
   bool bPacketSentLast = false;
   uint8_t buf[2] = {0x81, BAST_FTM_MSG_ERR_BAD_PKT_RCVD};
   
   /* check if packet just transmitted */
   BAST_FTM_READ(FTM_PHY_ASSIST_STS, &mb);
   bPacketSentLast = (mb & 0x0200) >> 9;
   if (!bPacketSentLast)
      return retCode;
   
   /* check if rx buffer available for bad packet */
   if ((hFtm->buf_status & BAST_FTM_BUF_RX_NOT_EMPTY) == 0)
   {
      /* get the number of bytes in the rx fifo */
      BAST_FTM_READ(FTM_UART_ASSIST_BLVR, &mb);
      bad_packet_len = (mb >> 16) & 0x7F;
      
      /* copy the data into rx_buffer */
      pBuf = hFtm->rx_buffer;
      *pBuf++ = bad_packet_len;
      
      for (i = 0; i < bad_packet_len; i++)
      {
         BAST_FTM_READ(FTM_UART_ASSIST_STBR, &mb);
         *pBuf++ = mb & 0xFF;
      }
      
      /* disregard bad poll packet */
      if ((bad_packet_len == 0x0B) && (hFtm->rx_buffer[1] == BAST_FTM_PKT_POLL))
         return retCode;
      
      /* disregard bad offer packet */
      if ((bad_packet_len == 0x07) && (hFtm->rx_buffer[1] == BAST_FTM_PKT_REG_OFFER))
         return retCode;
      
      /* forward bad packet to host if necessary */
      if (hFtm->options & (BAST_FTM_OPTION_FORWARD_BAD_PKTS | BAST_FTM_OPTION_PROMISCUOUS))
      {
         hFtm->buf_status |= BAST_FTM_BUF_RX_NOT_EMPTY;
         BKNI_SetEvent(hDev->hFtmEvent);
      }
   }
   
   /* store bad crc during registration or pending xtune */
   if ((hFtm->status & BAST_FTM_STATUS_REG_PENDING) || (hFtm->status & BAST_FTM_STATUS_XTUNE_RESP))
      retCode = BAST_g2_Ftm_P_HoldErrorMessage(h, buf, 2);
   else
      retCode = BAST_g2_Ftm_P_SendErrorMessage(h, BAST_FTM_MSG_ERR_BAD_PKT_RCVD);
   
   return retCode;      
}


/******************************************************************************
 BAST_g2_Ftm_P_TimerRxIrq() - ISR context
******************************************************************************/
BERR_Code BAST_g2_Ftm_P_TimerRxIrq(BAST_Handle h)
{
   BAST_g2_P_Handle *hDev = (BAST_g2_P_Handle *)(h->pImpl);
   BAST_g2_P_FtmDevice *hFtm = (BAST_g2_P_FtmDevice *) &(((BAST_g2_P_Handle *)(h->pImpl))->hFtmDev);
   BERR_Code retCode = BERR_SUCCESS;
   
   BDBG_MSG(("rx timeout\n"));
   
   /* stop rx timer */
   BAST_CHK_RETCODE(BAST_g2_Ftm_P_StopTimer(h, BAST_Ftm_TimerSelect_eRx));
   hFtm->buf_status &= ~BAST_FTM_BUF_TX_NOT_EMPTY;

   /* xtune response time out */
   if (hFtm->status & BAST_FTM_STATUS_XTUNE_RESP)
   {
      /* cancel retransmit of xtune request */
      hFtm->status &= ~(BAST_FTM_STATUS_XTUNE_RESP | BAST_FTM_STATUS_NETWORK_XTUNE_RESP);
   }

   /* clear statuses and send error message */
   hFtm->status &= ~BAST_FTM_STATUS_POLL_PENDING;
   if (hFtm->buf_status & BAST_FTM_BUF_ERR_NOT_EMPTY)
      BKNI_SetEvent(hDev->hFtmEvent);
   else
   {
      BAST_CHK_RETCODE(BAST_g2_Ftm_P_SendErrorMessage(h, 
         (hFtm->options & BAST_FTM_OPTION_NETWORK_ACTIVITY) ? BAST_FTM_MSG_ERR_NOT_AVAILABLE : BAST_FTM_MSG_ERR_NOT_DETECTED));
   }

   /* invalidate the special packet buffer */
   retCode = BAST_g2_Ftm_P_DoWhenHwAssistIdle_isr(h, BAST_g2_Ftm_P_ResetAutoTxBuffer);

   done:
   return retCode;
}


/******************************************************************************
 BAST_g2_Ftm_P_TimerTxIrq() - ISR context
******************************************************************************/
BERR_Code BAST_g2_Ftm_P_TimerTxIrq(BAST_Handle h)
{
   BAST_g2_P_Handle *hDev = (BAST_g2_P_Handle *)(h->pImpl);
   BAST_g2_P_FtmDevice *hFtm = (BAST_g2_P_FtmDevice *) &(((BAST_g2_P_Handle *)(h->pImpl))->hFtmDev);
   BERR_Code retCode = BERR_SUCCESS;
   uint32_t mb;
   
   BAST_CHK_RETCODE(BAST_g2_Ftm_P_StopTimer(h, BAST_Ftm_TimerSelect_eTx));
   BAST_g2_Ftm_P_EnableInterrupt_isr(h, BAST_Ftm_IntID_eUartEnd, false);
   
   /* disable tx state machine */
   BAST_FTM_READ(FTM_PHY_TX_CTL, &mb);
   mb &= 0xFFFFFFFE;
   BAST_FTM_WRITE(FTM_PHY_TX_CTL, mb);
   
   /* monitor TX state machine with TP_OUT[9:8] */
   BAST_FTM_WRITE(FTM_PHY_TP_CTL, 0x003000E0);
   BAST_FTM_READ(FTM_PHY_TP_OUT, &mb);
   
   /* check if TX is idle */
   if ((mb & 0x300) != 0)
   {
      /* reset tx fifo and uart */
      BAST_FTM_WRITE(FTM_UART_SRR, 0x04);
      BAST_FTM_WRITE(FTM_PHY_RST, 0x00000100);

      /* re-init hw assist */
      BAST_FTM_WRITE(FTM_UART_ASSIST_BRR, 0x00020001);   /* disable auto crc, reset HB, sp pkt, store buffers */
      BAST_FTM_WRITE(FTM_PHY_ASSIST_CNT2, 0x00000001);   /*  bad crc down counter = 1 */
      BAST_FTM_WRITE(FTM_UART_ASSIST_SERNMGNTR, 0x00000000);   /* initialize FTM serial number, monitor only my polls for ftm s/n */
      BAST_FTM_WRITE(FTM_UART_ASSIST_PCTL2R, 0x000094C6);      /* initialize exclude list */
      BAST_FTM_WRITE(FTM_UART_ASSIST_PCTL1R, 0x80000004);      /* initialize rx_bit_mask */
      BAST_FTM_WRITE(FTM_UART_ASSIST_CMDTRR, 0x94C694C6);      /* 7-bit checking on offer and poll cmd ids */
      BAST_FTM_WRITE(FTM_UART_ASSIST_RXMGNT2R, 0x00048003);    /* set MAX_NO_POLL to 4 */
      
      /* re-init fsk uart */
      BAST_g2_Ftm_P_ResetUart(h);
   }
   
   /* clear transmit in progress status */
   hFtm->status &= ~BAST_FTM_STATUS_TX_IN_PROGRESS;
   
   /* check if sending hard reset */
   if (hFtm->status & BAST_FTM_STATUS_HARD_RESET)
      hFtm->status &= ~BAST_FTM_STATUS_HARD_RESET;
   
   /* send error if registration timed out */
   if (hFtm->status & BAST_FTM_STATUS_REG_TIMEOUT)
   {
      hFtm->status &= ~BAST_FTM_STATUS_REG_TIMEOUT;
      
      if (hFtm->buf_status & BAST_FTM_BUF_ERR_NOT_EMPTY)
         BKNI_SetEvent(hDev->hFtmEvent);
      else
         retCode = BAST_g2_Ftm_P_SendErrorMessage(h, (hFtm->options & BAST_FTM_OPTION_NETWORK_ACTIVITY) ? BAST_FTM_MSG_ERR_NOT_AVAILABLE : BAST_FTM_MSG_ERR_NOT_DETECTED);
   }

   done:
   return retCode;
}


/******************************************************************************
 BAST_g2_Ftm_P_TimerRegIrq() - ISR context
******************************************************************************/
BERR_Code BAST_g2_Ftm_P_TimerRegIrq(BAST_Handle h)
{
   BAST_g2_P_FtmDevice *hFtm = (BAST_g2_P_FtmDevice *) &(((BAST_g2_P_Handle *)(h->pImpl))->hFtmDev);
   BERR_Code retCode = BERR_SUCCESS;
   uint32_t mb;
   
   /* stop registration timer */
   BAST_CHK_RETCODE(BAST_g2_Ftm_P_StopTimer(h, BAST_Ftm_TimerSelect_eReg));

   if (hFtm->status & BAST_FTM_STATUS_HARD_RESET)
   {
      /* disable network activity detect interrupt */
      BAST_g2_Ftm_P_EnableInterrupt_isr(h, BAST_Ftm_IntID_eAssistLvl1, false);
      
      /* send hard reset if no network activity */
      BDBG_MSG(("BAST_g2_Ftm_P_TimerRegIrq(): send hard reset\n"));
      retCode = BAST_g2_Ftm_P_DoWhenHwAssistIdle_isr(h, BAST_g2_Ftm_P_PrepareHardReset);
   }
   else if (hFtm->status & (BAST_FTM_STATUS_REG_REQ | BAST_FTM_STATUS_REG_RESP))
   {
      /* didn't get reg offer or reg resp */
      BDBG_MSG(("BAST_g2_Ftm_P_TimerRegIrq(): reg timeout\n"));
      
      /* terminate registration and invalidate sp pkt buffer */
      registration_timeout:
      retCode = BAST_g2_Ftm_P_DoWhenHwAssistIdle_isr(h, BAST_g2_Ftm_P_RegistrationTimeout);
   }
   else if (hFtm->status & BAST_FTM_STATUS_REG_WAIT)
   {
      if (hFtm->reg_time_left == 0)
         goto registration_timeout;
      
      if (hFtm->network_activity)
         goto wait_for_no_network_activity;
      
      /* check for non-zero byte count */
      BAST_FTM_READ(FTM_PHY_CORR_BYTE_CNT, &mb);
      if ((mb >> 24) & 0x7F)
         goto wait_for_no_network_activity;
      
      /* no network activity detected, so now we can send the reg request, disable network activity detect interrupt */
      BAST_g2_Ftm_P_EnableInterrupt_isr(h, BAST_Ftm_IntID_eAssistLvl1, false);
      
      /* check hw assist status */
      BAST_FTM_READ(FTM_PHY_ASSIST_STS, &mb);
      if (mb & 0x01)
      {
         /* request hw assist to stop */
         retCode = BAST_g2_Ftm_P_EnableHwAssist(h, false);
         if (retCode != BERR_SUCCESS)
         {
            /* revert stop hw assist request to wait for next offer */
            BAST_g2_Ftm_P_EnableHwAssist(h, true);
            goto wait_for_no_network_activity;
         }
         
         /* disable correlator */
         BAST_CHK_RETCODE(BAST_g2_Ftm_P_EnableReceiver(h, false));
      }
      else
      {
         /* hw assist is on so wait for next offer */
         wait_for_no_network_activity:
         retCode = BAST_g2_Ftm_P_DoWhenHwAssistIdle_isr(h, BAST_g2_Ftm_P_NetworkPreambleDetected);
         return retCode;
      }
      
      /* hw assist should be stopped here */
      BDBG_ASSERT(BAST_g2_Ftm_P_IsHwAssistStopped(h));
      
      /* invalidate the special packet buffer to cancel automatic transmission */
      BAST_CHK_RETCODE(BAST_g2_Ftm_P_ResetAutoTxBuffer(h));
      
      hFtm->status &= ~BAST_FTM_STATUS_REG_WAIT;
      hFtm->status |= (BAST_FTM_STATUS_REG_RESP | BAST_FTM_STATUS_START_TX);
      
      /* set up timer to wait for reg resp */
      BAST_CHK_RETCODE(BAST_g2_Ftm_P_StartTimer(h, BAST_Ftm_TimerSelect_eReg, hFtm->reg_time_left));
      
      /* issue manual transmission of the reg req */
      retCode = BAST_g2_Ftm_P_ManualTransmit(h);
   }
   else
   {
      BDBG_MSG(("BAST_g2_Ftm_P_TimerRegIrq(): should not get here!\n"));
      retCode = BERR_UNKNOWN;
   }

   done:
   return retCode;
}


/******************************************************************************
 BAST_g2_Ftm_P_HandleInterrupt_isr() - ISR context
******************************************************************************/
void BAST_g2_Ftm_P_HandleInterrupt_isr(void *p, int param)
{
   BAST_Handle h = (BAST_Handle)p;
   BAST_g2_P_FtmDevice *hFtm = (BAST_g2_P_FtmDevice *) &(((BAST_g2_P_Handle *)(h->pImpl))->hFtmDev);
   BERR_Code retCode = BERR_SUCCESS;
   BAST_FTM_ISR funct;
   uint32_t mb;
   
   /* clear the interrupt */
   retCode = BINT_ClearCallback_isr(hFtm->hCallback[param]);
   BDBG_ASSERT(retCode == BERR_SUCCESS);
   
#if (BCHP_CHIP==7335)
   /* verify interrupt */
   BAST_FTM_READ(FTM_PHY_IRQ_STS, &mb);
   if (mb & (1 << param))
   {
      /* clear interrupt */
      BAST_FTM_WRITE(FTM_PHY_IRQ_STS, 1 << param);
   }
   else
   {
      BDBG_ERR(("Dropped extraneous interrupt!"));
      return;
   }
#endif
   
   /* disable hw assist if hw assist interrupt */
   if (param >= BAST_Ftm_IntID_eAssistIdle)
   {
      retCode = BAST_g2_Ftm_P_EnableHwAssist(h, false);
      if (retCode != BERR_SUCCESS)
      {
         BDBG_ERR(("Failed to stop HW assist!"));
         return;
      }
   }
   
   switch (param)
	{
      case BAST_Ftm_IntID_eUartEnd:
         BDBG_MSG(("uart end!"));
         BAST_g2_Ftm_P_EnableInterrupt_isr(h, param, false);  /* disable interrupt */
         
         retCode = BAST_g2_Ftm_P_TransmitDone(h);
         break;
      
      case BAST_Ftm_IntID_eUart:
         BDBG_MSG(("uart fifo empty!"));
         BAST_g2_Ftm_P_EnableInterrupt_isr(h, param, false);  /* disable interrupt */
         retCode = BAST_g2_Ftm_P_UartFifoEmpty(h);
         break;
         
      case BAST_Ftm_IntID_eTimer1:
         BDBG_MSG(("timer1!"));
         retCode = BAST_g2_Ftm_P_TimerRxIrq(h);
         break;
      
      case BAST_Ftm_IntID_eTimer2:
         BDBG_MSG(("timer2!"));
         retCode = BAST_g2_Ftm_P_TimerTxIrq(h);
         break;
      
      case BAST_Ftm_IntID_eTimer3:
         BDBG_MSG(("timer3!"));
         retCode = BAST_g2_Ftm_P_TimerRegIrq(h);
         break;
         
      case BAST_Ftm_IntID_eAssistIdle:
         BDBG_MSG(("hw assist idle!"));
         BAST_g2_Ftm_P_EnableInterrupt_isr(h, param, false);  /* disable interrupt */
         if (hFtm->idle_funct)
         {
            funct = hFtm->idle_funct;
            hFtm->idle_funct = NULL;
            retCode = funct(h);
         }
         break;
         
      case BAST_Ftm_IntID_eRxPktRdy:
         BDBG_MSG(("rx packet ready!"));
         retCode = BAST_g2_Ftm_P_ProcessMyPacket(h);
         break;
         
      case BAST_Ftm_IntID_eAssistLvl1:
         BDBG_MSG(("level1!"));
         BAST_g2_Ftm_P_EnableInterrupt_isr(h, param, false);  /* disable interrupt */
         retCode = BAST_g2_Ftm_P_NetworkPreambleDetected(h);
         hFtm->options |= BAST_FTM_OPTION_PREAMBLE_DETECTED;
         break;
         
      case BAST_Ftm_IntID_eAssistLvl2:
         BDBG_MSG(("level2!"));
         BAST_g2_Ftm_P_EnableInterrupt_isr(h, param, false);  /* disable interrupt */
         hFtm->options |= BAST_FTM_OPTION_NETWORK_ACTIVITY;
         break;
         
      case BAST_Ftm_IntID_eSpPktSent:
         BDBG_MSG(("sp pkt sent!"));
         hFtm->status &= ~BAST_FTM_STATUS_POLL_PENDING;
         BAST_CHK_RETCODE(BAST_g2_Ftm_P_TransmitDone(h));
            
         /* check for any other pending messages to load to sp pkt buffer */
         if ((hFtm->buf_status & BAST_FTM_BUF_TX_NOT_EMPTY) && ((hFtm->status & BAST_FTM_STATUS_REG_PENDING) == 0))
            retCode = BAST_g2_Ftm_P_AutoTransmit(h);
         
         /* disable crc auto insertion after sending ping response */
         if (hFtm->status & BAST_FTM_STATUS_PING_RESP)
         {
            hFtm->status &= ~BAST_FTM_STATUS_PING_RESP;
            BAST_FTM_READ(FTM_UART_ASSIST_BRR, &mb);
            mb |= 0x00020000;
            BAST_FTM_WRITE(FTM_UART_ASSIST_BRR, mb);
         }
         break;
         
      case BAST_Ftm_IntID_eFBadCrcCnt:
         BDBG_MSG(("bad crc!\n"));
         
         /* skip packet processing */
         retCode = BINT_ClearCallback_isr(hFtm->hCallback[BAST_Ftm_IntID_eRxPktRdy]);
         retCode = BAST_g2_Ftm_P_BadRxCrc(h);
         break;
         
      case BAST_Ftm_IntID_eRxBitMaskChg:
         BDBG_MSG(("rx bit mask chg!\n"));
         BAST_FTM_READ(FTM_PHY_ASSIST_STS, &mb);
         hFtm->rx_bit_mask = (mb >> 16) & 0xFFFF;
         retCode = BAST_g2_Ftm_P_SetRxBitMask(h);
         break;
         
      case BAST_Ftm_IntID_eSerNumChg:
         BDBG_MSG(("ftm sernum chg!"));
         
         /* skip packet processing */
         retCode = BINT_ClearCallback_isr(hFtm->hCallback[BAST_Ftm_IntID_eRxPktRdy]);
   
         /* reset statuses and rx bit mask */
         hFtm->status = BAST_FTM_STATUS_RESET;
         hFtm->buf_status &= ~BAST_FTM_BUF_TX_NOT_EMPTY;
         hFtm->rx_bit_mask = 0x8000;
         BAST_CHK_RETCODE(BAST_g2_Ftm_P_SetRxBitMask(h));
         
         /* clear ftm serial number */
         BAST_FTM_WRITE(FTM_UART_ASSIST_SERNR, 0);
         
         /* let hw assist pick up new serial number without interrupt */  
         BAST_FTM_READ(FTM_UART_ASSIST_SERNMGNTR, &mb);
         mb &= ~0x01;
         BAST_FTM_WRITE(FTM_UART_ASSIST_SERNMGNTR, mb);
         break;
         
      default:
         /* disable this unhandled interrupt */
         BINT_DisableCallback_isr(hFtm->hCallback[param]);
         BDBG_ASSERT(retCode == BERR_SUCCESS);
         BDBG_MSG(("Unhandled FTM Interrupt ID=0x%x !", param));
	}

   done:
   if (retCode == BERR_SUCCESS)
   {
      /* start transmission if necessary, otherwise re-enable hw assist if no pending tx */
      if (hFtm->status & BAST_FTM_STATUS_START_TX)
         retCode = BAST_g2_Ftm_P_DoWhenHwAssistIdle_isr(h, BAST_g2_Ftm_P_ManualTransmit);
      else if ((hFtm->status & BAST_FTM_STATUS_TX_IN_PROGRESS) == 0)
         retCode = BAST_g2_Ftm_P_EnableHwAssist(h, true);
   }
}

#endif
