/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bscd_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 10/12/12 10:27a $
 *
 * Module Description: This file contains Broadcom smart card Porting 
 *                     Interface private functions.  
 *                    			                    
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/scd/src/core65nm/bscd_priv.c $
 * 
 * Hydra_Software_Devel/5   10/12/12 10:27a qxie
 * SW7360-34:Smartcard:fix work wait time setting
 * 
 * Hydra_Software_Devel/4   10/12/12 10:25a qxie
 * SW7360-34:Smartcard:fix work wait time setting
 * 
 * Hydra_Software_Devel/3   10/8/12 11:12a qxie
 * SW7360-34:fix wait time setting in activation
 * 
 * Hydra_Software_Devel/2   10/3/12 4:18p qxie
 * SW7360-34:Smartcard: fixed incorrect timer set up during reset
 * 
 * Hydra_Software_Devel/1   9/7/12 11:03a qxie
 * SW7429-283:Smartcard: remove symlink in PI code
 * 
 * Hydra_Software_Devel/73   5/24/11 1:43p qxie
 * SW7346-128:Need to add support for the smartcard controller chips
 * TDA803x
 * 
 * Hydra_Software_Devel/72   4/26/11 1:36p cdisc
 * SWDTV-6344: 35233 support
 * 
 * Hydra_Software_Devel/71   2/25/11 4:51p qxie
 * SW7405-5086:Timeout from BSCD_Channel_P_T0ReadData()
 * 
 * Hydra_Software_Devel/70   11/2/10 10:50a qxie
 * SW7335-809:Smartcard:Write succeeds with no smartcard
 * 
 * Hydra_Software_Devel/69   10/28/10 10:29a qxie
 * SW7325-653: add support to timer unit milliseconds in activating
 * 
 * Hydra_Software_Devel/68   10/27/10 6:19p qxie
 * SW7325-653: add support to timer unit milliseconds in activating
 * 
 * Hydra_Software_Devel/67   9/22/10 10:16a qxie
 * SW7335-809:Smartcard:Write succeeds with no smartcard
 * 
 * Hydra_Software_Devel/66   5/28/10 4:20p qxie
 * SW7325-753:Smartcard: Irdeto Smartcard settings is not set correctly
 * 
 * Hydra_Software_Devel/65   5/17/10 10:22a qxie
 * SW7405-4135:when edc is enabled, cwi is cleared during transmit/read.
 * Fix this by add or Proto_CMD value
 * 
 * Hydra_Software_Devel/64   5/11/10 11:33a cdisc
 * SW35230-52: for 35230 bringup
 * 
 * Hydra_Software_Devel/63   4/26/10 10:52a mward
 * SW7125-358:SCD support needed for 7125c0
 * 
 * Hydra_Software_Devel/62   3/31/10 6:08p qxie
 * SW7405-4135:when edc is enabled, cwi is cleared during transmit. Fix
 * this by add or Proto_CMD value
 * 
 * Hydra_Software_Devel/61   1/7/10 4:57p qxie
 * SW7550-167:add default f, d, settings in ARIB table
 * 
 * Hydra_Software_Devel/60   12/7/09 10:09p randyjew
 * SW7468-6:Add 7468 support
 * 
 * Hydra_Software_Devel/60   12/7/09 10:08p randyjew
 * SW7468-6:Add 7468 support
 * 
 * Hydra_Software_Devel/59   12/2/09 6:16p qxie
 * SW7550-96:Smartcard: Add support to 7550
 * 
 * Hydra_Software_Devel/58   11/19/09 4:55p qxie
 * SW7125-113:Smartcard:add support to 7125
 * 
 * Hydra_Software_Devel/57   11/12/09 4:47p qxie
 * SW7325-653: Smartcard: DirecTV smartcard requires longer delay for no-
 * responsive card
 * 
 * Hydra_Software_Devel/56   8/27/09 1:54p qxie
 * SW7125-18:add support for 7125A0 in SCD
 * 
 * Hydra_Software_Devel/55   8/10/09 5:50p qxie
 * PR57564: Smartcard: add support to 7340/7342
 * 
 * Hydra_Software_Devel/54   7/15/09 12:49p qxie
 * PR54713: Smartcard: Add suport to Arib FD value
 * 
 * Hydra_Software_Devel/53   7/13/09 3:20p qxie
 * PR56770: Smartcard: move BSCD_Channel_P_DumpRegisters to public PI,
 * changed name to BSCD_Channel_DumpRegisters
 * 
 * Hydra_Software_Devel/52   7/7/09 11:51a qxie
 * PR54713: Smartcard: Add suport to Arib FD value
 * 
 * Hydra_Software_Devel/51   7/7/09 11:45a qxie
 * PR54713: Smartcard: Add suport to Arib FD value
 * 
 * Hydra_Software_Devel/50   4/29/09 12:09p qxie
 * PR54713: Smartcard: EDC setting not correct;
 * 
 * Hydra_Software_Devel/49   4/7/09 4:53p qxie
 * PR47804: add atr receive time to channel settings
 * 
 * Hydra_Software_Devel/48   4/7/09 11:15a qxie
 * PR47804: add atr receive time to channel settings
 * 
 * Hydra_Software_Devel/47   2/18/09 6:03p qxie
 * PR47804: add new clk divider sc_div=16
 * 
 * Hydra_Software_Devel/46   2/18/09 5:59p qxie
 * PR47804: add new clk divider sc_div=16
 * 
 * Hydra_Software_Devel/45   2/18/09 5:09p qxie
 * PR47804: add new clk divider sc_div=16
 * 
 * Hydra_Software_Devel/44   1/19/09 11:07a qxie
 * PR48383: BSCD - VCC, CLK and RESET lines are raised in the wrong order
 * 
 * Hydra_Software_Devel/43   1/8/09 2:36p qxie
 * PR48563: Smartcard: NDS scr3 card fails at new tests(flow control
 * * enable)- undo check in, the issue is on NDS card itself.
 * 
 * Hydra_Software_Devel/42   12/15/08 3:11p qxie
 * PR48563: Smartcard: NDS scr3 card fails at new tests(flow control
 * * enable)- undo check in, the issue is on NDS card itself.
 * 
 * Hydra_Software_Devel/41   12/8/08 8:54p qxie
 * PR47804: add smartcard support to 7420
 * 
 * Hydra_Software_Devel/40   10/30/08 5:32p qxie
 * PR48563: Smartcard: NDS scr3 card fails at new tests(flow control
 * enable)
 * 
 * Hydra_Software_Devel/39   5/14/08 2:49p qxie
 * PR42538:Possible System froze - oops from Smartcard
 * 
 * Hydra_Software_Devel/38   4/28/08 12:02p qxie
 * PR42234: Smartcard: add EMV2000 support under 36M external clock
 * 
 * Hydra_Software_Devel/37   3/14/08 3:21p qxie
 * PR40605: Smartcard: add 36M clock support for Nagra certification
 * 
 * Hydra_Software_Devel/36   1/11/08 12:10p qxie
 * PR38191: SmartCard presence detection
 * 
 * Hydra_Software_Devel/35   10/22/07 3:31p qxie
 * PR36354: Smartcard: bscd_priv.c err status was overided in
 * BSCD_Channel_P_Activating, remove the check condition in disable
 * interrupt
 * 
 * Hydra_Software_Devel/34   10/19/07 1:02p qxie
 * PR35976:Coverity Defect ID:3755 CHECKED_RETURN bscd_priv.c
 * Product=97401ln
 * 
 * Hydra_Software_Devel/33   4/27/07 2:40p qxie
 * PR30269:Smartcard: Add support to NDS no flow control card
 * 
 * Hydra_Software_Devel/32   10/25/06 11:54a qxie
 * PR 25171: Smartcard: remove printf statement in bscd_priv.c
 * 
 * Hydra_Software_Devel/31   10/25/06 11:48a qxie
 * PR 25171: Smartcard: remove printf statement in bscd_priv.c
 * 
 * Hydra_Software_Devel/30   10/20/06 2:49p qxie
 * PR23810: Smartcard: Add support to 7118(add T=14 support)
 * 
 * Hydra_Software_Devel/29   9/28/06 10:28a qxie
 * PR 24606:SmartCard driver should return data even on a partial read
 * 
 * Hydra_Software_Devel/28   4/12/06 10:28a btan
 * PR 20789: Fixed a race condition that BSCD_Channel_P_WaitForTimerEvent
 * may wait for 5 seconds if there is no smartcard in the slot and we
 * perform ResetIFD.
 * 
 * Hydra_Software_Devel/27   3/8/06 6:29p qcheng
 * PR20091: Fix bug in Irdeto T=14 -- Parity Bit( Stop Bit) need always be
 * high.
 * 
 * Hydra_Software_Devel/26   12/15/05 2:38p qcheng
 * PR 16586: Added support for 7401
 * 
 * Hydra_Software_Devel/25   9/21/05 3:44p qcheng
 * PR 17263:Exit BKNI_WaitForEvent() when card is removed
 * 
 * Hydra_Software_Devel/24   6/15/05 3:55p btan
 * PR 10898: Added support EMV2000 version 3.0.4 tests
 * 
 * Hydra_Software_Devel/23   5/27/05 10:04p btan
 * PR 15680: Added smartcard support for 3560.
 * 
 * Hydra_Software_Devel/22   4/9/05 2:47p btan
 * PR 14698: Removed unused parameter warnings.
 * 
 * Hydra_Software_Devel/21   3/16/05 12:07p btan
 * PR 14493: Dont't include bchp_7038.h.
 * 
 * Hydra_Software_Devel/20   1/7/05 8:06p btan
 * PR 13681: Do not disable WWT in BSCD_Channel_Receive. Remove GP timer
 * in BSCD_Channel_P_T0ReadData. Need to retest all other standards.
 * 
 * Hydra_Software_Devel/19   12/29/04 10:04a btan
 * PR 13681: Fixed 2 bugs 1) milli seconds to ETU conversion. and
 * 2) if sc_prescale is channelSetting is 0, we should use the setting in
 * the table.
 * 
 * Hydra_Software_Devel/19   12/27/04 8:41p btan
 * PR 13681: Fixed 2 bugs 1) milli seconds to ETU conversion. and
 * 2) if sc_prescale is channelSetting is 0, we should use the setting in
 * the table.
 * 
 * Hydra_Software_Devel/19   12/27/04 8:39p btan
 * PR 13681: Fixed 2 bugs 1) milli seconds to ETU conversion. and
 * 2) if sc_prescale is channelSetting is 0, we should use the setting in
 * the table.
 * 
 * Hydra_Software_Devel/18   12/1/04 11:47a btan
 * PR 13420: BSCD_Channel_P_T0ReadData does not handle timeout correctly.
 * 
 * Hydra_Software_Devel/17   11/5/04 5:55p btan
 * PR 13185: Add smartcard tests in Brutus.
 * 
 * Hydra_Software_Devel/16   10/5/04 1:22p btan
 * PR 12924: Fixed an issue of calling  2 consecutive
 * LeaveCriticalSections in a row.
 * 
 * Hydra_Software_Devel/15   9/21/04 11:15a btan
 * PR 12728: Fixed warnings caused by compiler option '-pdantic'.
 * 
 * Hydra_Software_Devel/14   9/17/04 4:33p btan
 * PR 12504: support customized callback functions and pass an event type.
 * 
 * Hydra_Software_Devel/13   9/9/04 12:49p btan
 * PR 12504, PR12607:support customized callback functions. Smartcard: T=0
 * protocol takes too long to time out and time out happens after all
 * bytes are received.
 * 
 * Hydra_Software_Devel/12   8/4/04 10:24a btan
 * PR 12164: Fixed smartcard compilation warning
 * 
 * Hydra_Software_Devel/11   7/29/04 4:48p btan
 * PR 12114: Added NDS DSS support on 7038
 * 
 * Hydra_Software_Devel/10   7/20/04 2:12p btan
 * PR 10898: Pass EMV 2000 tests with either event interrupt or WWT to
 * detect CWT +4
 * 
 * Hydra_Software_Devel/9   6/8/04 3:16p btan
 * PR 10898: Added support for EMV 2000.
 * 
 * Hydra_Software_Devel/8   5/17/04 4:54p btan
 * PR 10898: Added support for 7038 B0.
 * 
 * Hydra_Software_Devel/7   12/31/03 11:05a btan
 * PR 8080: Fixed AUTO_CLK set before SC_CLK_EN
 * 
 * Hydra_Software_Devel/6   12/1/03 5:56p btan
 * Fixes a debug message.
 * 
 * Hydra_Software_Devel/5   11/26/03 5:01p btan
 * Fixes for EMV T=0
 * 
 * Hydra_Software_Devel/3   11/18/03 5:08p btan
 * Added support for Irdeto transmission and receiving.
 * 
 * Hydra_Software_Devel/2   10/28/03 5:03p btan
 * Fixed the bugs after the first round test.
 * 
 * Hydra_Software_Devel/1   10/13/03 12:20p btan
 * Initial Creation.
 *
 ***************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"
#if(BCHP_CHIP!=7125) ||((BCHP_CHIP==7125)&& (BCHP_VER>=BCHP_VER_C0))
#include "bchp_sca.h"
#else
#include "bchp_scb.h"
#endif

#include "bscd.h"
#include "bscd_priv.h"
#include "bscd_emvpriv.h"


#if (BCHP_CHIP==35230) ||(BCHP_CHIP==7420) || (BCHP_CHIP==7340) || (BCHP_CHIP==7342) ||(BCHP_CHIP==7550) || (BCHP_CHIP==7468)  || ((BCHP_CHIP==7125)&& (BCHP_VER>=BCHP_VER_C0))
#define BCHP_SCA_SC_CLK_CMD_clk_en_MASK BCHP_SCA_SC_CLK_CMD_1_clk_en_MASK
#define BCHP_SCA_SC_CLK_CMD_bauddiv_MASK BCHP_SCA_SC_CLK_CMD_1_bauddiv_MASK
#elif (BCHP_CHIP==35233)
#define BCHP_SCA_SC_CLK_CMD_clk_en_MASK BCHP_SCA_SC_CLK_CMD_1_clk_en_MASK
#define BCHP_SCA_SC_CLK_CMD_bauddiv_MASK BCHP_SCA_SC_CLK_CMD_1_bauddiv0_MASK

#endif
#if 1
#undef BDBG_ENTER
#undef BDBG_LEAVE
#define BDBG_ENTER(x) 
#define BDBG_LEAVE(x)
#endif


#define  BSCD_INTERRUPT_DEBUG  1

BDBG_MODULE(BSCD);


/* Population count of 1's in a byte */
static const unsigned char BSCD_P_Popcount[] = {
 0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
};

static const BSCD_P_DFSmartCardStruct BSCD_P_DF_EMV[10][14] = {


#if defined(BSCD_NAGRA_36M_CLK)
	/* There is an issue with old Integri that we have to go with
      lower frequency 
   */
    /* D = 0 */
   {{0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 
    {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 
    {0,0,0,0}, {0,0,0,0}},

    /* D = 1 */
   {{0x02,0x17,0x1F,0x06}, {0x02,0x17,0x1F,0x06}, {0x02,0x23,0x1F,0x06}, 
    {0x02,0x2F,0x1F,0x06}, {0x02,0x47,0x1F,0x06}, {0x02,0x5F,0x1F,0x06}, 
    {0x02,0x77,0x1F,0x06}, {0x00,0x00,0x00,0x00}, {0x00,0x00,0x00,0x00},
    {0x02,0x1F,0x20,0x06}, {0x02,0x2F,0x20,0x06}, {0x02,0x3F,0x20,0x06}, 
    {0x02,0x5F,0x20,0x06}, {0x02,0x7F,0x20,0x06}},

    /* D = 2 */
    {{0x02,0x0B,0x1F,0x08}, {0x02,0x0B,0x1F,0x08}, {0x02,0x11,0x1F,0x08}, 
    {0x02,0x17,0x1F,0x08}, {0x02,0x23,0x1F,0x08}, {0x02,0x2F,0x1F,0x08}, 
    {0x02,0x3B,0x1F,0x08}, {0x00,0x00,0x00,0x00}, {0x00,0x00,0x00,0x00},
    {0x02,0x0F,0x20,0x08}, {0x02,0x17,0x20,0x08}, {0x02,0x1F,0x20,0x08}, 
    {0x02,0x2F,0x20,0x08}, {0x02,0x3F,0x20,0x08}},

    /* D = 3 */
   {{0x03,0x08,0x1F,0x06}, {0x03,0x08,0x1F,0x06}, {0x06,0x17,0x1F,0x03}, 
    {0x03,0x11,0x1F,0x06}, {0x06,0x35,0x1F,0x03}, {0x03,0x23,0x1F,0x06}, 
    {0x06,0x59,0x1F,0x03}, {0x00,0x00,0x00,0x00}, {0x00,0x00,0x00,0x00},
    {0x03,0x0B,0x20,0x06}, {0x03,0x11,0x20,0x06}, {0x03,0x17,0x20,0x06}, 
    {0x03,0x23,0x20,0x06}, {0x03,0x2F,0x20,0x06}},

    /* D = 4 */
   {{0x02,0x02,0x1F,0x06}, {0x02,0x02,0x1F,0x06}, {0x04,0x08,0x1F,0x03}, 
    {0x02,0x05,0x1F,0x06},  {0x02,0x08,0x1F,0x06}, {0x02,0x0B,0x1F,0x06}, 
    {0x02,0x0E,0x1F,0x06}, {0x00,0x00,0x00,0x00}, {0x00,0x00,0x00,0x00},
    {0x02,0x03,0x20,0x06}, {0x02,0x05,0x20,0x06}, {0x02,0x07,0x20,0x06}, 
    {0x02,0x0B,0x20,0x06}, {0x02,0x0F,0x20,0x06}},

    /* D = 5 */
   {{0x04,0x02,0x1F,0x03}, {0x04,0x02,0x1F,0x03}, {0x08,0x08,0x1F,0x02}, 
    {0x02,0x02,0x1F,0x06}, {0x04,0x08,0x1F,0x03}, {0x02,0x05,0x1F,0x06}, 
    {0x04,0x0E,0x1F,0x03}, {0x00,0x00,0x00,0x00}, {0x00,0x00,0x00,0x00},
    {0x02,0x01,0x20,0x06}, {0x02,0x02,0x20,0x06}, {0x02,0x03,0x20,0x06}, 
    {0x02,0x05,0x20,0x06}, {0x02,0x07,0x20,0x06}},

    /* D = 6 */
   {{0x08,0x02,0x1F,0x02}, {0x08,0x02,0x1F,0x02}, {0x10,0x08,0x1F,0x01}, 
    {0x04,0x02,0x1F,0x03}, {0x08,0x08,0x1F,0x02}, {0x04,0x05,0x1F,0x03}, 
    {0x08,0x0E,0x1F,0x02}, {0x00,0x00,0x00,0x00}, {0x00,0x00,0x00,0x00},
    {0x04,0x01,0x20,0x03}, {0x04,0x02,0x20,0x03}, {0x02,0x01,0x20,0x06}, 
    {0x02,0x02,0x20,0x06}, {0x02,0x03,0x20,0x06}},

    /* D = 7 */
   {{0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 
    {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 
    {0,0,0,0}, {0,0,0,0}},

    /* D = 8 */
   {{0x02,0x01,0x1F,0x06}, {0x02,0x01,0x1F,0x06}, {0x02,0x02,0x1F,0x06}, 
    {0x02,0x03,0x1F,0x06}, {0x02,0x05,0x1F,0x06}, {0x02,0x07,0x1F,0x06},
    {0x02,0x09,0x1F,0x06}, {0x00,0x00,0x00,0x00}, {0x00,0x00,0x00,0x00},
    {0x03,0x03,0x20,0x04}, {0x02,0x03,0x20,0x06}, {0x03,0x07,0x20,0x04},
    {0x02,0x07,0x20,0x06}, {0x03,0x0F,0x20,0x04}},

    /* D = 9 */
   {{0x05,0x02,0x1F,0x04}, {0x05,0x02,0x1F,0x04}, {0x0A,0x08,0x1F,0x02}, 
    {0x05,0x05,0x1F,0x04}, {0x05,0x08,0x1F,0x04}, {0x05,0x0B,0x1F,0x04}, 
    {0x02,0x05,0x1F,0x08}, {0x00,0x00,0x00,0x00}, {0x00,0x00,0x00,0x00},
    {0x05,0x03,0x20,0x04}, {0x05,0x05,0x20,0x04}, {0x05,0x07,0x20,0x04}, 
    {0x05,0x0B,0x20,0x04}, {0x05,0x0F,0x20,0x04}}
#else /*27Mhz*/
   /* There is an issue with old Integri that we have to go with
      lower frequency 
   */
    /* D = 0 */
   {{0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 
    {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 
    {0,0,0,0}, {0,0,0,0}},

    /* D = 1 */
   {{0x01,0x0B,0x1F,0x08}, {0x02,0x17,0x1F,0x08}, {0x01,0x11,0x1F,0x08}, 
    {0x01,0x17,0x1F,0x08}, {0x01,0x23,0x1F,0x08}, {0x01,0x2F,0x1F,0x08}, 
    {0x01,0x3B,0x1F,0x08}, {0x00,0x00,0x00,0x00}, {0x00,0x00,0x00,0x00},
    {0x01,0x0F,0x20,0x08}, {0x01,0x17,0x20,0x08}, {0x01,0x1F,0x20,0x08}, 
    {0x01,0x2F,0x20,0x08}, {0x01,0x3F,0x20,0x08}},

    /* D = 2 */
   {{0x01,0x05,0x1F,0x08}, {0x02,0x0B,0x1F,0x08}, {0x01,0x08,0x1F,0x08}, 
    {0x01,0x0B,0x1F,0x08}, {0x01,0x11,0x1F,0x08}, {0x01,0x17,0x1F,0x08}, 
    {0x01,0x1D,0x1F,0x08}, {0x00,0x00,0x00,0x00}, {0x00,0x00,0x00,0x00},
    {0x01,0x07,0x20,0x08}, {0x01,0x0B,0x20,0x08}, {0x01,0x0F,0x20,0x08}, 
    {0x01,0x17,0x20,0x08}, {0x01,0x1F,0x20,0x08}},

    /* D = 3 */
   {{0x01,0x02,0x1F,0x08}, {0x02,0x05,0x1F,0x08}, {0x02,0x08,0x1F,0x08}, 
    {0x01,0x05,0x1F,0x08}, {0x01,0x08,0x1F,0x08}, {0x01,0x0B,0x1F,0x08},
    {0x01,0x0E,0x1F,0x08}, {0x00,0x00,0x00,0x00}, {0x00,0x00,0x00,0x00},
    {0x01,0x03,0x20,0x08}, {0x01,0x05,0x20,0x08}, {0x01,0x07,0x20,0x08}, 
    {0x01,0x0B,0x20,0x08}, {0x01,0x0F,0x20,0x08}},

    /* D = 4 */
   {{0x02,0x02,0x1F,0x08}, {0x02,0x02,0x1F,0x08}, {0x04,0x08,0x1F,0x04}, 
    {0x01,0x02,0x1F,0x08}, {0x02,0x08,0x1F,0x08}, {0x01,0x05,0x1F,0x08}, 
    {0x02,0x0E,0x1F,0x08}, {0x00,0x00,0x00,0x00}, {0x00,0x00,0x00,0x00},
    {0x01,0x01,0x20,0x08}, {0x01,0x02,0x20,0x08}, {0x01,0x03,0x20,0x08}, 
    {0x01,0x05,0x20,0x08}, {0x01,0x07,0x20,0x08}},

    /* D = 5 */
   {{0x04,0x02,0x1F,0x04}, {0x04,0x02,0x1F,0x04}, {0x08,0x08,0x1F,0x02}, 
    {0x02,0x02,0x1F,0x08}, {0x04,0x08,0x1F,0x04}, {0x01,0x02,0x1F,0x08}, 
    {0x04,0x0E,0x1F,0x04}, {0x00,0x00,0x00,0x00}, {0x00,0x00,0x00,0x00},
    {0x02,0x01,0x20,0x08}, {0x02,0x02,0x20,0x08}, {0x01,0x01,0x20,0x08}, 
    {0x01,0x02,0x20,0x08}, {0x01,0x03,0x20,0x08}},

    /* D = 6 */
   {{0x08,0x02,0x1F,0x02}, {0x08,0x02,0x1F,0x02}, {0x10,0x08,0x1F,0x01}, 
    {0x04,0x02,0x1F,0x04}, {0x08,0x08,0x1F,0x02}, {0x02,0x02,0x1F,0x08}, 
    {0x08,0x0E,0x1F,0x02}, {0x00,0x00,0x00,0x00}, {0x00,0x00,0x00,0x00},
    {0x04,0x01,0x20,0x04}, {0x04,0x02,0x20,0x04}, {0x02,0x01,0x20,0x08}, 
    {0x02,0x02,0x20,0x08}, {0x01,0x01,0x20,0x08}},

    /* D = 7 */
   {{0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 
    {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 
    {0,0,0,0}, {0,0,0,0}},

    /* D = 8 */
   {{0x02,0x01,0x1F,0x08}, {0x02,0x01,0x1F,0x08}, {0x02,0x02,0x1F,0x08}, 
    {0x02,0x03,0x1F,0x08}, {0x02,0x05,0x1F,0x08}, {0x02,0x07,0x1F,0x08},
    {0x02,0x09,0x1F,0x08}, {0x00,0x00,0x00,0x00}, {0x00,0x00,0x00,0x00},
    {0x03,0x03,0x20,0x08}, {0x02,0x03,0x20,0x08}, {0x03,0x07,0x20,0x08},
    {0x02,0x07,0x20,0x08}, {0x03,0x0F,0x20,0x08}},

    /* D = 9 */
   {{0x05,0x02,0x1F,0x04}, {0x05,0x02,0x1F,0x04}, {0x0A,0x08,0x1F,0x02}, 
    {0x05,0x05,0x1F,0x04}, {0x05,0x08,0x1F,0x04}, {0x05,0x0B,0x1F,0x04}, 
    {0x02,0x05,0x1F,0x08}, {0x00,0x00,0x00,0x00}, {0x00,0x00,0x00,0x00},
    {0x05,0x03,0x20,0x04}, {0x05,0x05,0x20,0x04}, {0x05,0x07,0x20,0x04}, 
    {0x05,0x0B,0x20,0x04}, {0x05,0x0F,0x20,0x04}}
#endif
};

static const BSCD_P_DFSmartCardStruct BSCD_P_DF[10][14] = {
#if defined(BSCD_DSS_ICAM)

    /* D = 0 */
   {{0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 
    {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 
    {0,0,0,0}, {0,0,0,0}},

    /* D = 1 */
   {{0x01,0x0B,0x1F,0x08}, {0x01,0x0B,0x1F,0x08}, {0x01,0x11,0x1F,0x08}, 
    {0x01,0x17,0x1F,0x04}, {0x01,0x23,0x1F,0x08}, {0x01,0x2F,0x1F,0x02}, 
    {0x01,0x3B,0x1F,0x08}, {0x00,0x00,0x00,0x00}, {0x00,0x00,0x00,0x00},
    {0x01,0x0F,0x20,0x08}, {0x01,0x17,0x20,0x08}, {0x01,0x1F,0x20,0x08}, 
    {0x01,0x2F,0x20,0x08}, {0x01,0x3F,0x20,0x08}},

    /* D = 2 */
   {{0x01,0x05,0x1F,0x08}, {0x01,0x05,0x1F,0x08}, {0x01,0x08,0x1F,0x08}, 
    {0x01,0x0B,0x1F,0x04}, {0x01,0x11,0x1F,0x08}, {0x01,0x17,0x1F,0x02}, 
    {0x01,0x1D,0x1F,0x08}, {0x00,0x00,0x00,0x00}, {0x00,0x00,0x00,0x00},
    {0x01,0x07,0x20,0x08}, {0x01,0x0B,0x20,0x08}, {0x01,0x0F,0x20,0x08}, 
    {0x01,0x17,0x20,0x08}, {0x01,0x1F,0x20,0x08}},

    /* D = 3 */
   {{0x01,0x02,0x1F,0x08}, {0x01,0x02,0x1F,0x08}, {0x02,0x08,0x1F,0x08}, 
    {0x01,0x05,0x1F,0x04}, {0x01,0x08,0x1F,0x08}, {0x01,0x0B,0x1F,0x02},
    {0x01,0x0E,0x1F,0x08}, {0x00,0x00,0x00,0x00}, {0x00,0x00,0x00,0x00},
    {0x01,0x03,0x20,0x08}, {0x01,0x05,0x20,0x08}, {0x01,0x07,0x20,0x08}, 
    {0x01,0x0B,0x20,0x08}, {0x01,0x0F,0x20,0x08}},

    /* D = 4 */
   {{0x02,0x02,0x1F,0x04}, {0x02,0x02,0x1F,0x04}, {0x04,0x08,0x1F,0x04}, 
    {0x01,0x02,0x1F,0x04}, {0x02,0x08,0x1F,0x08}, {0x01,0x05,0x1F,0x02}, 
    {0x02,0x0E,0x1F,0x08}, {0x00,0x00,0x00,0x00}, {0x00,0x00,0x00,0x00},
    {0x01,0x01,0x20,0x08}, {0x01,0x02,0x20,0x08}, {0x01,0x03,0x20,0x08}, 
    {0x01,0x05,0x20,0x08}, {0x01,0x07,0x20,0x08}},

    /* D = 5 */
   {{0x04,0x02,0x1F,0x02}, {0x04,0x02,0x1F,0x02}, {0x08,0x08,0x1F,0x02}, 
    {0x02,0x02,0x1F,0x02}, {0x04,0x08,0x1F,0x04}, {0x01,0x02,0x1F,0x02}, 
    {0x04,0x0E,0x1F,0x04}, {0x00,0x00,0x00,0x00}, {0x00,0x00,0x00,0x00},
    {0x02,0x01,0x20,0x08}, {0x02,0x02,0x20,0x08}, {0x01,0x01,0x20,0x08}, 
    {0x01,0x02,0x20,0x08}, {0x01,0x03,0x20,0x08}},

    /* D = 6 */
   {{0x08,0x02,0x1F,0x02}, {0x08,0x02,0x1F,0x02}, {0x10,0x08,0x1F,0x01}, 
    {0x04,0x02,0x1F,0x04}, {0x08,0x08,0x1F,0x02}, {0x02,0x02,0x1F,0x08}, 
    {0x08,0x0E,0x1F,0x02}, {0x00,0x00,0x00,0x00}, {0x00,0x00,0x00,0x00},
    {0x04,0x01,0x20,0x04}, {0x04,0x02,0x20,0x04}, {0x02,0x01,0x20,0x08}, 
    {0x02,0x02,0x20,0x08}, {0x01,0x01,0x20,0x08}},

    /* D = 7 */
#if(BCHP_CHIP==7420)|| (BCHP_CHIP==7340) || (BCHP_CHIP==7342)|| (BCHP_CHIP==7125)
   {{0x10,0x02,0x1F,0x01}, {0x10,0x02,0x1F,0x01}, {0x00,0x00,0x00,0x00}, 
    {0x08,0x02,0x1F,0x01}, {0x10,0x08,0x1F,0x01}, {0x04,0x02,0x1F,0x02}, 
    {0x10,0x0E,0x1F,0x01}, {0x00,0x00,0x00,0x00}, {0x00,0x00,0x00,0x00}, 
    {0x08,0x01,0x20,0x01}, {0x08,0x02,0x20,0x01}, {0x04,0x01,0x20,0x02}, 
    {0x04,0x02,0x20,0x02}, {0x02,0x01,0x20,0x04}},
#else
   {{0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 
    {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 
    {0,0,0,0}, {0,0,0,0}},
#endif

    /* D = 8 */
   {{0x02,0x01,0x1F,0x08}, {0x02,0x01,0x1F,0x08}, {0x02,0x02,0x1F,0x08}, 
    {0x02,0x03,0x1F,0x08}, {0x02,0x05,0x1F,0x08}, {0x02,0x07,0x1F,0x08},
    {0x02,0x09,0x1F,0x08}, {0x00,0x00,0x00,0x00}, {0x00,0x00,0x00,0x00},
    {0x03,0x03,0x20,0x08}, {0x02,0x03,0x20,0x08}, {0x03,0x07,0x20,0x08},
    {0x02,0x07,0x20,0x08}, {0x03,0x0F,0x20,0x08}},

    /* D = 9 */
   {{0x05,0x02,0x1F,0x04}, {0x05,0x02,0x1F,0x04}, {0x0A,0x08,0x1F,0x02}, 
    {0x05,0x05,0x1F,0x04}, {0x05,0x08,0x1F,0x04}, {0x05,0x0B,0x1F,0x04}, 
    {0x02,0x05,0x1F,0x08}, {0x00,0x00,0x00,0x00}, {0x00,0x00,0x00,0x00},
    {0x05,0x03,0x20,0x04}, {0x05,0x05,0x20,0x04}, {0x05,0x07,0x20,0x04}, 
    {0x05,0x0B,0x20,0x04}, {0x05,0x0F,0x20,0x04}}
#elif defined(BSCD_NAGRA_36M_CLK)
/* Note: QX: only Test (D, F) = (5, 9)(6, 9)(6, A) */

/* D = 0 */
   {{0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 
    {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 
    {0,0,0,0}, {0,0,0,0}},

    /* D = 1 */
   {{0x01,0x0B,0x1F,0x08}, {0x01,0x0B,0x1F,0x08}, {0x01,0x11,0x1F,0x08}, 
    {0x01,0x17,0x1F,0x08}, {0x01,0x23,0x1F,0x08}, {0x01,0x2F,0x1F,0x08}, 
    {0x01,0x3B,0x1F,0x08}, {0x00,0x00,0x00,0x00}, {0x00,0x00,0x00,0x00},
    {0x01,0x0F,0x20,0x08}, {0x01,0x17,0x20,0x08}, {0x01,0x1F,0x20,0x08}, 
    {0x01,0x2F,0x20,0x08}, {0x01,0x3F,0x20,0x08}},

    /* D = 2 */
   {{0x01,0x05,0x1F,0x08}, {0x01,0x05,0x1F,0x08}, {0x01,0x08,0x1F,0x08}, 
    {0x01,0x0B,0x1F,0x04}, {0x01,0x11,0x1F,0x08}, {0x01,0x17,0x1F,0x08}, 
    {0x01,0x1D,0x1F,0x08}, {0x00,0x00,0x00,0x00}, {0x00,0x00,0x00,0x00},
    {0x01,0x07,0x20,0x08}, {0x01,0x0B,0x20,0x08}, {0x01,0x0F,0x20,0x08}, 
    {0x01,0x17,0x20,0x08}, {0x01,0x1F,0x20,0x08}},

    /* D = 3 */
   {{0x01,0x02,0x1F,0x08}, {0x01,0x02,0x1F,0x08}, {0x02,0x08,0x1F,0x04}, 
    {0x01,0x05,0x1F,0x08}, {0x01,0x08,0x1F,0x08}, {0x01,0x0B,0x1F,0x08},
    {0x01,0x0E,0x1F,0x08}, {0x00,0x00,0x00,0x00}, {0x00,0x00,0x00,0x00},
    {0x01,0x03,0x20,0x08}, {0x01,0x05,0x20,0x08}, {0x01,0x07,0x20,0x08}, 
    {0x01,0x0B,0x20,0x08}, {0x01,0x0F,0x20,0x08}},

    /* D = 4 */
   {{0x02,0x02,0x1F,0x04}, {0x02,0x02,0x1F,0x04}, {0x04,0x08,0x1F,0x02}, 
    {0x01,0x02,0x1F,0x08}, {0x02,0x08,0x1F,0x04}, {0x01,0x05,0x1F,0x08}, 
    {0x02,0x0E,0x1F,0x04}, {0x00,0x00,0x00,0x00}, {0x00,0x00,0x00,0x00},
    {0x01,0x01,0x20,0x08}, {0x01,0x02,0x20,0x08}, {0x01,0x03,0x20,0x08}, 
    {0x01,0x05,0x20,0x08}, {0x01,0x07,0x20,0x08}},

    /* D = 5 */
   {{0x04,0x02,0x1F,0x02}, {0x04,0x02,0x1F,0x02}, {0x08,0x08,0x1F,0x01}, 
    {0x02,0x02,0x1F,0x04}, {0x04,0x08,0x1F,0x02}, {0x01,0x02,0x1F,0x08}, 
    {0x04,0x0E,0x1F,0x02}, {0x00,0x00,0x00,0x00}, {0x00,0x00,0x00,0x00},
    {0x02,0x01,0x20,0x04}, {0x02,0x02,0x20,0x04}, {0x01,0x01,0x20,0x08}, 
    {0x01,0x02,0x20,0x08}, {0x01,0x03,0x20,0x08}},

    /* D = 6 */
   {{0x08,0x02,0x1F,0x01}, {0x08,0x02,0x1F,0x01}, {0x10,0x08,0x1F,0x01}, 
    {0x04,0x02,0x1F,0x02}, {0x08,0x08,0x1F,0x01}, {0x04,0x05,0x1F,0x02}, 
    {0x08,0x0E,0x1F,0x01}, {0x00,0x00,0x00,0x00}, {0x00,0x00,0x00,0x00},
    {0x04,0x01,0x20,0x02}, {0x04,0x02,0x20,0x02}, {0x02,0x01,0x20,0x04}, 
    {0x02,0x02,0x20,0x04}, {0x01,0x01,0x20,0x08}},

    /* D = 7 */
#if(BCHP_CHIP==7420) || (BCHP_CHIP==7340) || (BCHP_CHIP==7342)|| (BCHP_CHIP==7125)
   {{0x10,0x02,0x1F,0x01}, {0x10,0x02,0x1F,0x01}, {0x00,0x00,0x00,0x00}, 
    {0x08,0x02,0x1F,0x01}, {0x10,0x08,0x1F,0x01}, {0x04,0x02,0x1F,0x02}, 
    {0x10,0x0E,0x1F,0x01}, {0x00,0x00,0x00,0x00}, {0x00,0x00,0x00,0x00}, 
    {0x08,0x01,0x20,0x01}, {0x08,0x02,0x20,0x01}, {0x04,0x01,0x20,0x02}, 
    {0x04,0x02,0x20,0x02}, {0x02,0x01,0x20,0x04}},
#else
  {{0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 
    {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 
    {0,0,0,0}, {0,0,0,0}},
#endif
    /* D = 8 */
   {{0x02,0x01,0x1F,0x04}, {0x02,0x01,0x1F,0x04}, {0x02,0x02,0x1F,0x04}, 
    {0x02,0x03,0x1F,0x04}, {0x02,0x05,0x1F,0x04}, {0x02,0x07,0x1F,0x04},
    {0x02,0x09,0x1F,0x04}, {0x00,0x00,0x00,0x00}, {0x00,0x00,0x00,0x00},
    {0x03,0x03,0x20,0x03}, {0x02,0x03,0x20,0x04}, {0x03,0x07,0x20,0x03},
    {0x02,0x07,0x20,0x04}, {0x03,0x0F,0x20,0x03}},

    /* D = 9 */
   {{0x05,0x02,0x1F,0x02}, {0x05,0x02,0x1F,0x02}, {0x0A,0x08,0x1F,0x01}, 
    {0x05,0x05,0x1F,0x02}, {0x05,0x08,0x1F,0x02}, {0x05,0x0B,0x1F,0x02}, 
    {0x02,0x05,0x1F,0x04}, {0x00,0x00,0x00,0x00}, {0x00,0x00,0x00,0x00},
    {0x05,0x03,0x20,0x02}, {0x05,0x05,0x20,0x02}, {0x05,0x07,0x20,0x02}, 
    {0x05,0x0B,0x20,0x02}, {0x05,0x0F,0x20,0x02}}
#elif defined(BSCD_ARIB_24M_CLK)
	    /* D = 0 */
   {{0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 
    {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 
    {0,0,0,0}, {0,0,0,0}},
        /* D = 1 */
   {{0,0,0,0}, {0x01,0x0B,0x1F,0x06}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 
    {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 
    {0,0,0,0}, {0,0,0,0}},
        /* D = 2 */
   {{0,0,0,0}, {0x01,0x05,0x1F,0x06}, {0,0,0,0}, {0x01,0x0B,0x1F,0x06}, {0,0,0,0}, {0,0,0,0}, 
    {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 
    {0,0,0,0}, {0,0,0,0}},
        /* D = 3 */
   {{0,0,0,0}, {0x01,0x02,0x1F,0x03}, {0,0,0,0}, {0x01,0x05,0x1F,0x03}, {0,0,0,0}, {0,0,0,0}, 
    {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 
    {0,0,0,0}, {0,0,0,0}},
        /* D = 4 */
   {{0,0,0,0}, {0x02,0x02,0x1F,0x03}, {0,0,0,0}, {0x01,0x02,0x1F,0x06}, {0,0,0,0}, {0,0,0,0}, 
    {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 
    {0,0,0,0}, {0,0,0,0}},
        /* D = 5 */
   {{0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 
    {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 
    {0,0,0,0}, {0,0,0,0}},
        /* D = 6 */
   {{0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 
    {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 
    {0,0,0,0}, {0,0,0,0}},
        /* D = 7 */
   {{0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 
    {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 
    {0,0,0,0}, {0,0,0,0}},
        /* D = 8 */
   {{0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 
    {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 
    {0,0,0,0}, {0,0,0,0}},
        /* D = 9 */
   {{0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 
    {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 
    {0,0,0,0}, {0,0,0,0}},
#else
	
    /* D = 0 */
   {{0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 
    {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, 
    {0,0,0,0}, {0,0,0,0}},

    /* D = 1 */
   {{0x01,0x0B,0x1F,0x06}, {0x01,0x0B,0x1F,0x06}, {0x01,0x11,0x1F,0x06}, 
    {0x01,0x17,0x1F,0x06}, {0x01,0x23,0x1F,0x06}, {0x01,0x2F,0x1F,0x06}, 
    {0x01,0x3B,0x1F,0x06}, {0x00,0x00,0x00,0x00}, {0x00,0x00,0x00,0x00},
    {0x01,0x0F,0x20,0x06}, {0x01,0x17,0x20,0x06}, {0x01,0x1F,0x20,0x06}, 
    {0x01,0x2F,0x20,0x06}, {0x01,0x3F,0x20,0x06}},

    /* D = 2 */
   {{0x01,0x05,0x1F,0x06}, {0x01,0x05,0x1F,0x06}, {0x01,0x08,0x1F,0x06}, 
    {0x01,0x0B,0x1F,0x06}, {0x01,0x11,0x1F,0x06}, {0x01,0x17,0x1F,0x06}, 
    {0x01,0x1D,0x1F,0x06}, {0x00,0x00,0x00,0x00}, {0x00,0x00,0x00,0x00},
    {0x01,0x07,0x20,0x06}, {0x01,0x0B,0x20,0x06}, {0x01,0x0F,0x20,0x06}, 
    {0x01,0x17,0x20,0x06}, {0x01,0x1F,0x20,0x06}},

    /* D = 3 */
   {{0x01,0x02,0x1F,0x06}, {0x01,0x02,0x1F,0x06}, {0x02,0x08,0x1F,0x03}, 
    {0x01,0x05,0x1F,0x06}, {0x01,0x08,0x1F,0x06}, {0x01,0x0B,0x1F,0x06},
    {0x01,0x0E,0x1F,0x06}, {0x00,0x00,0x00,0x00}, {0x00,0x00,0x00,0x00},
    {0x01,0x03,0x20,0x06}, {0x01,0x05,0x20,0x06}, {0x01,0x07,0x20,0x06}, 
    {0x01,0x0B,0x20,0x06}, {0x01,0x0F,0x20,0x06}},

    /* D = 4 */
   {{0x02,0x02,0x1F,0x03}, {0x02,0x02,0x1F,0x03}, {0x04,0x08,0x1F,0x02}, 
    {0x01,0x02,0x1F,0x06}, {0x02,0x08,0x1F,0x03}, {0x01,0x05,0x1F,0x06}, 
    {0x02,0x0E,0x1F,0x03}, {0x00,0x00,0x00,0x00}, {0x00,0x00,0x00,0x00},
    {0x01,0x01,0x20,0x06}, {0x01,0x02,0x20,0x06}, {0x01,0x03,0x20,0x06}, 
    {0x01,0x05,0x20,0x06}, {0x01,0x07,0x20,0x06}},

    /* D = 5 */
   {{0x04,0x02,0x1F,0x02}, {0x04,0x02,0x1F,0x02}, {0x08,0x08,0x1F,0x01}, 
    {0x02,0x02,0x1F,0x03}, {0x04,0x08,0x1F,0x02}, {0x01,0x02,0x1F,0x06}, 
    {0x04,0x0E,0x1F,0x02}, {0x00,0x00,0x00,0x00}, {0x00,0x00,0x00,0x00},
    {0x02,0x01,0x20,0x03}, {0x02,0x02,0x20,0x03}, {0x01,0x01,0x20,0x06}, 
    {0x01,0x02,0x20,0x06}, {0x01,0x03,0x20,0x06}},

    /* D = 6 */
   {{0x08,0x02,0x1F,0x01}, {0x08,0x02,0x1F,0x01}, {0x10,0x08,0x1F,0x01}, 
    {0x04,0x02,0x1F,0x01}, {0x08,0x08,0x1F,0x01}, {0x02,0x02,0x1F,0x03}, 
    {0x08,0x0E,0x1F,0x01}, {0x00,0x00,0x00,0x00}, {0x00,0x00,0x00,0x00},
    {0x04,0x01,0x20,0x02}, {0x04,0x02,0x20,0x02}, {0x02,0x01,0x20,0x03}, 
    {0x02,0x02,0x20,0x03}, {0x01,0x01,0x20,0x06}},

    /* D = 7 */
   {{0x10,0x02,0x1F,0x01}, {0x10,0x02,0x1F,0x01}, {0x00,0x00,0x00,0x00}, 
    {0x08,0x02,0x1F,0x01}, {0x10,0x08,0x1F,0x01}, {0x04,0x02,0x1F,0x02}, 
    {0x10,0x0E,0x1F,0x01}, {0x00,0x00,0x00,0x00}, {0x00,0x00,0x00,0x00}, 
    {0x08,0x01,0x20,0x01}, {0x08,0x02,0x20,0x01}, {0x04,0x01,0x20,0x02}, 
    {0x04,0x02,0x20,0x02}, {0x02,0x01,0x20,0x04}},

    /* D = 8 */
   {{0x02,0x01,0x1F,0x03}, {0x02,0x01,0x1F,0x03}, {0x02,0x02,0x1F,0x03}, 
    {0x02,0x03,0x1F,0x03}, {0x02,0x05,0x1F,0x03}, {0x02,0x07,0x1F,0x03},
    {0x02,0x09,0x1F,0x03}, {0x00,0x00,0x00,0x00}, {0x00,0x00,0x00,0x00},
    {0x03,0x03,0x20,0x02}, {0x02,0x03,0x20,0x03}, {0x03,0x07,0x20,0x02},
    {0x02,0x07,0x20,0x03}, {0x03,0x0F,0x20,0x02}},

    /* D = 9 */
   {{0x05,0x02,0x1F,0x02}, {0x05,0x02,0x1F,0x02}, {0x0A,0x08,0x1F,0x01}, 
    {0x05,0x05,0x1F,0x02}, {0x05,0x08,0x1F,0x02}, {0x05,0x0B,0x1F,0x02}, 
    {0x02,0x05,0x1F,0x03}, {0x00,0x00,0x00,0x00}, {0x00,0x00,0x00,0x00},
    {0x05,0x03,0x20,0x02}, {0x05,0x05,0x20,0x02}, {0x05,0x07,0x20,0x02}, 
    {0x05,0x0B,0x20,0x02}, {0x05,0x0F,0x20,0x02}}
#endif
};




static const unsigned int BSCD_P_aunFFactor[14] = {372, 372, 558, 744, 1116, 1488, 1860, 
                                 -1,  -1, 512, 768, 1024, 1536, 2048};


static const unsigned char BSCD_P_aucDFactor[10] = {-1, 1, 2, 4, 8, 16, 32, -1, 12, 20};

static const unsigned char BSCD_P_aucSCClkDiv[16] = {0x00, 0x10, 0x20, 0x30, 0x40, 0x00, 0x00, 0x50, 
                                 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70}; 


unsigned char BSCD_P_GetClkDiv(
	BSCD_ChannelHandle	in_channelHandle, 
      unsigned char in_ucDFactor,
      unsigned char in_ucFFactor
)
{
	if ((in_channelHandle->currentChannelSettings.scStandard != BSCD_Standard_eEMV1996) && 
				(in_channelHandle->currentChannelSettings.scStandard != BSCD_Standard_eEMV2000) )
	{
	
	if (in_channelHandle->currentChannelSettings.scStandard != BSCD_Standard_eIrdeto)
   return BSCD_P_DF[in_ucDFactor][in_ucFFactor].sc_clkdiv;
	else return 4;
	}else{
	
	return BSCD_P_DF_EMV[in_ucDFactor][in_ucFFactor].sc_clkdiv;
		}

}


unsigned char BSCD_P_GetETUClkDiv(
	BSCD_ChannelHandle	in_channelHandle, 
      unsigned char in_ucDFactor,
      unsigned char in_ucFFactor
)
{
	if ((in_channelHandle->currentChannelSettings.scStandard != BSCD_Standard_eEMV1996) && 
				(in_channelHandle->currentChannelSettings.scStandard != BSCD_Standard_eEMV2000) )
	{
		if (in_channelHandle->currentChannelSettings.scStandard != BSCD_Standard_eIrdeto)
   return BSCD_P_DF[in_ucDFactor][in_ucFFactor].sc_etuclkdiv;
		else return 1;
	}else
	return BSCD_P_DF_EMV[in_ucDFactor][in_ucFFactor].sc_etuclkdiv;
   
}

unsigned char BSCD_P_GetISOBaudRateAdjustor(
      unsigned char in_ucDFactor
)
{
   return  BSCD_P_aucDFactor[in_ucDFactor];
}

unsigned int BSCD_P_GetISOClockRateConversionFactor(
      unsigned char in_ucFFactor
)
{
   return  BSCD_P_aunFFactor[in_ucFFactor];
}

unsigned char BSCD_P_MapScClkDivToMaskValue(
      unsigned char in_ucClkDiv
)
{
   return  BSCD_P_aucSCClkDiv[in_ucClkDiv-1];
}

unsigned char BSCD_P_GetPrescale(
	BSCD_ChannelHandle	in_channelHandle, 
      unsigned char in_ucDFactor,
      unsigned char in_ucFFactor
)
{

if ((in_channelHandle->currentChannelSettings.scStandard != BSCD_Standard_eEMV1996) && 
			(in_channelHandle->currentChannelSettings.scStandard != BSCD_Standard_eEMV2000) )
{
	if (in_channelHandle->currentChannelSettings.scStandard != BSCD_Standard_eIrdeto)
   return BSCD_P_DF[in_ucDFactor][in_ucFFactor].sc_prescale;
	else return 0x4A;
}else
return BSCD_P_DF_EMV[in_ucDFactor][in_ucFFactor].sc_prescale;
}

unsigned char BSCD_P_GetBaudDiv(
      BSCD_ChannelHandle	in_channelHandle, 
      unsigned char in_ucDFactor,
      unsigned char in_ucFFactor
)
{
if ((in_channelHandle->currentChannelSettings.scStandard != BSCD_Standard_eEMV1996) && 
			(in_channelHandle->currentChannelSettings.scStandard != BSCD_Standard_eEMV2000) )
{
		if (in_channelHandle->currentChannelSettings.scStandard != BSCD_Standard_eIrdeto)
   return BSCD_P_DF[in_ucDFactor][in_ucFFactor].sc_bauddiv;
		else return 32;
}else
return BSCD_P_DF_EMV[in_ucDFactor][in_ucFFactor].sc_bauddiv;
}


/* BSYT???: Change this function name to Adjust WWT */
BERR_Code BSCD_P_AdjustWWT(
      		BSCD_ChannelHandle	in_channelHandle, 
		unsigned char 			in_ucFFactor, 
		unsigned char 			in_ucDFactor,
		unsigned char			in_ucWorkWaitTImeInteger
)
{
	BERR_Code errCode = BERR_SUCCESS;
	unsigned char         ucBaudRateAdjustor;

	BDBG_ENTER(BSCD_P_AdjustWWT);
   
	BDBG_MSG(("in_ucDFactor = %d\n", in_ucDFactor));
	BDBG_MSG(("in_ucFFactor = %d\n", in_ucFFactor));

	BDBG_MSG(("baudrate = %lu\n", in_channelHandle->currentChannelSettings.currentBaudRate));

	BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, 
		(in_channelHandle->currentChannelSettings.currentBaudRate == 0) );

	BDBG_MSG (("etu in us= %d\n", 
	   	1000000/in_channelHandle->currentChannelSettings.currentBaudRate));

	ucBaudRateAdjustor = BSCD_P_GetISOBaudRateAdjustor(in_ucDFactor);

/*EMV2000*/
	if (in_channelHandle->currentChannelSettings.scStandard == BSCD_Standard_eEMV2000)
		in_channelHandle->currentChannelSettings.workWaitTime.ulValue = 
			BSCD_ISO_WORK_WAIT_TIME_DEFAULT_FACTOR * ucBaudRateAdjustor * 
			in_ucWorkWaitTImeInteger + ucBaudRateAdjustor * 
			BSCD_DEFAULT_EXTRA_WORK_WAITING_TIME_EMV2000 + 
			BSCD_EMV2000_WORK_WAIT_TIME_DELTA;		
	else
		in_channelHandle->currentChannelSettings.workWaitTime.ulValue = 
			BSCD_ISO_WORK_WAIT_TIME_DEFAULT_FACTOR * ucBaudRateAdjustor * 
			in_ucWorkWaitTImeInteger;
				
	in_channelHandle->currentChannelSettings.workWaitTime.unit = BSCD_TimerUnit_eETU;

	BSTD_UNUSED(in_ucFFactor);

BSCD_P_DONE_LABEL:
	
	BDBG_LEAVE(BSCD_P_AdjustWWT);
	return( errCode );
}


/* This modify registers */
BERR_Code BSCD_P_FDAdjust(
		BSCD_ChannelHandle	in_channelHandle, 
		unsigned char 			in_ucFFactor, 
		unsigned char			in_ucDFactor
)
{
	BERR_Code errCode = BERR_SUCCESS;
	uint32_t ulClkCmd=0;

	BDBG_ENTER(BSCD_P_FDAdjust);
	
	/* Set BCM to adjust the clock and bit rate  */
	in_channelHandle->currentChannelSettings.unPrescale = 
				BSCD_P_GetPrescale(in_channelHandle, in_ucDFactor, in_ucFFactor) *
				in_channelHandle->currentChannelSettings.ucExternalClockDivisor + 
			(in_channelHandle->currentChannelSettings.ucExternalClockDivisor - 1);

   	BREG_Write32( 
		in_channelHandle->moduleHandle->regHandle, 
		(in_channelHandle->ulRegStartAddr + BSCD_P_PRESCALE),
		in_channelHandle->currentChannelSettings.unPrescale);		
	BDBG_MSG(("SC_Prescale = 0x%x\n", in_channelHandle->currentChannelSettings.unPrescale));

	in_channelHandle->currentChannelSettings.ucBaudDiv = 
			BSCD_P_GetBaudDiv(in_channelHandle, in_ucDFactor, in_ucFFactor);


	BDBG_MSG(("sc_bauddiv = 0x%2x\n", in_channelHandle->currentChannelSettings.ucBaudDiv));			
	
	BDBG_MSG(("orig ucClkCmd = 0x%x\n", BREG_Read32( 
		in_channelHandle->moduleHandle->regHandle, 
		(in_channelHandle->ulRegStartAddr + BSCD_P_CLK_CMD)) ));	
	
	in_channelHandle->currentChannelSettings.ucScClkDiv = 
		BSCD_P_GetClkDiv(in_channelHandle, in_ucDFactor, in_ucFFactor) ;
	
	if (in_channelHandle->currentChannelSettings.ucBaudDiv == BSCD_DEFAULT_BAUD_DIV) {
		if(in_channelHandle->currentChannelSettings.ucScClkDiv<16)
		ulClkCmd = BCHP_SCA_SC_CLK_CMD_clk_en_MASK | 
				BSCD_P_MapScClkDivToMaskValue(in_channelHandle->currentChannelSettings.ucScClkDiv ) | 
				((in_channelHandle->currentChannelSettings.ucEtuClkDiv - 1) << 1)  ;
		else{
			#if(BCHP_CHIP==7420) || (BCHP_CHIP==7340) || (BCHP_CHIP==7342)|| (BCHP_CHIP==7125)
			ulClkCmd = BCHP_SCA_SC_CLK_CMD_clk_en_MASK | 
				BSCD_P_MapScClkDivToMaskValue(1 ) | 
				((in_channelHandle->currentChannelSettings.ucEtuClkDiv - 1) << 1)  ;
				BREG_Write32( 
				in_channelHandle->moduleHandle->regHandle, 
				(in_channelHandle->ulRegStartAddr + BSCD_P_CLK_CMD_2),
				1);
			#else
				BDBG_ERR(("Clock divider %d not supported", in_channelHandle->currentChannelSettings.ucScClkDiv)); 
			#endif
		}
	}
	else {
		if(in_channelHandle->currentChannelSettings.ucScClkDiv<16)
		ulClkCmd = BCHP_SCA_SC_CLK_CMD_clk_en_MASK | 
				BSCD_P_MapScClkDivToMaskValue(in_channelHandle->currentChannelSettings.ucScClkDiv ) | 
				((in_channelHandle->currentChannelSettings.ucEtuClkDiv - 1) << 1)  | 
				BCHP_SCA_SC_CLK_CMD_bauddiv_MASK;		
		else{
			#if(BCHP_CHIP==7420) || (BCHP_CHIP==7340) || (BCHP_CHIP==7342)|| (BCHP_CHIP==7125)
			ulClkCmd = BCHP_SCA_SC_CLK_CMD_clk_en_MASK | 
				BSCD_P_MapScClkDivToMaskValue(1 ) | 
			((in_channelHandle->currentChannelSettings.ucEtuClkDiv - 1) << 1)  | 
				BCHP_SCA_SC_CLK_CMD_bauddiv_MASK;		
				BREG_Write32( 
				in_channelHandle->moduleHandle->regHandle, 
				(in_channelHandle->ulRegStartAddr + BSCD_P_CLK_CMD_2),
				1);
			#else
				BDBG_ERR(("Clock divider %d not supported", in_channelHandle->currentChannelSettings.ucScClkDiv)); 
			#endif
		}
	}

   	BREG_Write32( 
		in_channelHandle->moduleHandle->regHandle, 
		(in_channelHandle->ulRegStartAddr + BSCD_P_CLK_CMD),
		ulClkCmd);
	BDBG_MSG(("New SC_CLK_CMD = 0x%x\n", ulClkCmd));

	BDBG_LEAVE(BSCD_P_FDAdjust);
	return( errCode );   

}


/* Default ISR Callback Functions */
void BSCD_Channel_P_CardInsertCB_isr( 
      BSCD_ChannelHandle	in_channelHandle, 
      void       				*inp_data 
) 
{
	BDBG_MSG(("default  BSCD_Channel_P_CardInsertCB_isr \n"));
	BSTD_UNUSED(inp_data);		
	if (in_channelHandle->bIsOpen == true) {
		BKNI_SetEvent( in_channelHandle->channelWaitEvent.cardWait);
	}
}

void  BSCD_Channel_P_CardRemoveCB_isr( 
      BSCD_ChannelHandle	in_channelHandle, 
      void       				*inp_data  
) 
{
	BDBG_MSG(("default  BSCD_Channel_P_CardRemoveCB_isr \n"));
	BSTD_UNUSED(inp_data);		
	if (in_channelHandle->bIsOpen == true) {
		BKNI_SetEvent( in_channelHandle->channelWaitEvent.cardWait);
		/* 09/20/05,	Allen.C,  set rcv event and let BKNI_WaitForEvent() exit after card is removed*/	
		BKNI_SetEvent( in_channelHandle->channelWaitEvent.rcvWait);
		BKNI_SetEvent( in_channelHandle->channelWaitEvent.tdoneWait);
		BKNI_SetEvent( in_channelHandle->channelWaitEvent.timerWait);
	}
}


void BSCD_Channel_P_RcvCB_isr( 
      BSCD_ChannelHandle	in_channelHandle, 
      void       				*inp_data  
) 
{
	BSTD_UNUSED(inp_data);	
	if (in_channelHandle->bIsOpen == true) {
		BKNI_SetEvent( in_channelHandle->channelWaitEvent.rcvWait);
	}
}

void BSCD_Channel_P_ATRCB_isr( 
      BSCD_ChannelHandle	in_channelHandle, 
      void       				*inp_data  
) 
{
	BSTD_UNUSED(inp_data);	
	if (in_channelHandle->bIsOpen == true) {
		BKNI_SetEvent( in_channelHandle->channelWaitEvent.atrStart);
	}
}

void BSCD_Channel_P_WaitCB_isr( 
      BSCD_ChannelHandle	in_channelHandle, 
      void       *inp_data 
) 
{
	BSTD_UNUSED(inp_data);	
	if (in_channelHandle->bIsOpen == true) {
		BKNI_SetEvent( in_channelHandle->channelWaitEvent.rcvWait);
		BKNI_SetEvent( in_channelHandle->channelWaitEvent.tdoneWait);
	}   
	
}

void BSCD_Channel_P_RetryCB_isr( 
      BSCD_ChannelHandle	in_channelHandle, 
      void       *inp_data 
) 
{
	BSTD_UNUSED(inp_data);	
	if (in_channelHandle->bIsOpen == true) {
		BKNI_SetEvent( in_channelHandle->channelWaitEvent.tdoneWait);				
		BKNI_SetEvent( in_channelHandle->channelWaitEvent.rcvWait);
	}
}

void BSCD_Channel_P_TimerCB_isr( 
      BSCD_ChannelHandle	in_channelHandle,
      void       *inp_data 
) 
{
	BDBG_ENTER(BSCD_Channel_P_TimerCB_isr);
	BSTD_UNUSED(inp_data);		
	if (in_channelHandle->bIsOpen == true) {
		BKNI_SetEvent( in_channelHandle->channelWaitEvent.atrStart);
		BKNI_SetEvent( in_channelHandle->channelWaitEvent.rcvWait);
		BKNI_SetEvent( in_channelHandle->channelWaitEvent.timerWait);
	}  
	BDBG_LEAVE(BSCD_Channel_P_TimerCB_isr);	
 }

void BSCD_Channel_P_RParityCB_isr( 
      BSCD_ChannelHandle	in_channelHandle,
      void       *inp_data 
) 
{
	BSTD_UNUSED(inp_data);	
	if (in_channelHandle->bIsOpen == true) {
		BKNI_SetEvent( in_channelHandle->channelWaitEvent.rcvWait);
	}  
}

void BSCD_Channel_P_TParityCB_isr(
      BSCD_ChannelHandle	in_channelHandle,
      void       *inp_data 
)
{
	BSTD_UNUSED(inp_data);	
	if (in_channelHandle->bIsOpen == true) {
		BKNI_SetEvent( in_channelHandle->channelWaitEvent.tdoneWait);				
	}	
}

void BSCD_Channel_P_CWTCB_isr( 
      BSCD_ChannelHandle	in_channelHandle,
      void       *inp_data 
) 
{
	BSTD_UNUSED(inp_data);	
	if (in_channelHandle->bIsOpen == true) {
		BKNI_SetEvent( in_channelHandle->channelWaitEvent.rcvWait);
	} 
}

void BSCD_Channel_P_BGTCB_isr( 
      BSCD_ChannelHandle	in_channelHandle,
      void       *inp_data 
) 
{
	BSTD_UNUSED(inp_data);	
	if (in_channelHandle->bIsOpen == true) {
		BKNI_SetEvent( in_channelHandle->channelWaitEvent.tdoneWait);				
		BKNI_SetEvent( in_channelHandle->channelWaitEvent.rcvWait);
	}	
}

void BSCD_Channel_P_RLenCB_isr( 
      BSCD_ChannelHandle	in_channelHandle,
      void       *inp_data 
) 
{
	BSTD_UNUSED(inp_data);	
	if (in_channelHandle->bIsOpen == true) {		
		BKNI_SetEvent( in_channelHandle->channelWaitEvent.rcvWait);
	}
}

void BSCD_Channel_P_RReadyCB_isr( 
      BSCD_ChannelHandle	in_channelHandle,
      void       *inp_data 
) 
{
	BSTD_UNUSED(inp_data);	
	if (in_channelHandle->bIsOpen == true) {		
		BKNI_SetEvent( in_channelHandle->channelWaitEvent.rcvWait);
	}
}

void BSCD_Channel_P_TDoneCB_isr( 
      BSCD_ChannelHandle	in_channelHandle,
      void       *inp_data 
) 
{
	BSTD_UNUSED(inp_data);	
	if (in_channelHandle->bIsOpen == true) {
		BKNI_SetEvent( in_channelHandle->channelWaitEvent.tdoneWait);				
	}
}

#ifdef BSCD_EMV2000_CWT_PLUS_4_EVENT_INTR
void BSCD_Channel_P_Event1CB_isr( 
      BSCD_ChannelHandle	in_channelHandle,
      void       *inp_data 
) 
{
	BSTD_UNUSED(inp_data);	
	if (in_channelHandle->bIsOpen == true) {
		BKNI_SetEvent( in_channelHandle->channelWaitEvent.rcvWait);		
		BKNI_SetEvent( in_channelHandle->channelWaitEvent.event1Wait);	
	}
}
#endif

void BSCD_Channel_P_Event2CB_isr( 
      BSCD_ChannelHandle	in_channelHandle,
      void       *inp_data 
) 
{
	BSTD_UNUSED(inp_data);	
	if (in_channelHandle->bIsOpen == true) {
		BKNI_SetEvent( in_channelHandle->channelWaitEvent.event2Wait);	
		BKNI_SetEvent( in_channelHandle->channelWaitEvent.rcvWait);
	}
}



BERR_Code BSCD_Channel_P_WaitForCardInsertion(
		BSCD_ChannelHandle	in_channelHandle
)
{
	BERR_Code errCode = BERR_SUCCESS;
	uint32_t ulStatus1;

	BDBG_ENTER(BSCD_Channel_P_WaitForCardInsertion);

	BDBG_MSG(("Ready to receive card insertion pres_intr interrupt")); 


	BKNI_EnterCriticalSection();
	ulStatus1 = in_channelHandle->ulStatus1 & BCHP_SCA_SC_STATUS_1_card_pres_MASK;
	BKNI_LeaveCriticalSection();
		
	do {		

		if ( ulStatus1 != BCHP_SCA_SC_STATUS_1_card_pres_MASK) {
			
			BSCD_P_CHECK_ERR_CODE_FUNC(errCode, 
				BKNI_WaitForEvent( in_channelHandle->channelWaitEvent.cardWait, BKNI_INFINITE));
					
		}

		BKNI_EnterCriticalSection();
		ulStatus1 = in_channelHandle->ulStatus1 & BCHP_SCA_SC_STATUS_1_card_pres_MASK;
		BKNI_LeaveCriticalSection();		

	} while  (ulStatus1 != BCHP_SCA_SC_STATUS_1_card_pres_MASK);

	BDBG_MSG(("Received card insertion pres_intr interrupt")); 

	if ( ulStatus1 == BCHP_SCA_SC_STATUS_1_card_pres_MASK) {
		in_channelHandle->channelStatus.bCardPresent = true;
		in_channelHandle->ulIntrStatus1 &= ~BCHP_SCA_SC_INTR_STAT_1_pres_intr_MASK;
		BDBG_MSG(("Smart Card Inserted"));
	}
	
BSCD_P_DONE_LABEL:

	BDBG_LEAVE(BSCD_Channel_P_WaitForCardInsertion);	
	return errCode;
}

BERR_Code BSCD_Channel_P_WaitForCardRemove(
		BSCD_ChannelHandle	in_channelHandle
)
{
	BERR_Code errCode = BERR_SUCCESS;
	uint32_t ulStatus1;

	BDBG_ENTER(BSCD_Channel_P_WaitForCardRemove);

	BDBG_MSG(("Ready to receive card removal pres_intr interrupt")); 


	BKNI_EnterCriticalSection();
	ulStatus1 = in_channelHandle->ulStatus1 & BCHP_SCA_SC_STATUS_1_card_pres_MASK;
	BKNI_LeaveCriticalSection();
		
	do {		

		if ( ulStatus1 == BCHP_SCA_SC_STATUS_1_card_pres_MASK) {
			
			BSCD_P_CHECK_ERR_CODE_FUNC(errCode, 
				BKNI_WaitForEvent( in_channelHandle->channelWaitEvent.cardWait, BKNI_INFINITE));
					
		}

		BKNI_EnterCriticalSection();
		ulStatus1 = in_channelHandle->ulStatus1 & BCHP_SCA_SC_STATUS_1_card_pres_MASK;
		BKNI_LeaveCriticalSection();		

	} while  (ulStatus1 == BCHP_SCA_SC_STATUS_1_card_pres_MASK);

	BDBG_MSG(("Received card removal pres_intr interrupt")); 

	if ( ulStatus1 != BCHP_SCA_SC_STATUS_1_card_pres_MASK) {
		in_channelHandle->channelStatus.bCardPresent = false;
		in_channelHandle->ulIntrStatus1 &= ~BCHP_SCA_SC_INTR_STAT_1_pres_intr_MASK;
		BDBG_MSG(("Smart Card Removed"));
	}
	
BSCD_P_DONE_LABEL:

	BDBG_LEAVE(BSCD_Channel_P_WaitForCardRemove);	
	return errCode;
}


BERR_Code BSCD_Channel_P_WaitForTimerEvent(
		BSCD_ChannelHandle	in_channelHandle
)
{
	BERR_Code errCode = BERR_SUCCESS;
	uint32_t ulIntrStatus1;

	BDBG_ENTER(BSCD_Channel_P_WaitForTimerEvent);

	BDBG_MSG(("Ready to receive scard_timer_wait interrupt\n")); 

			
		
	do {		
	BKNI_EnterCriticalSection();
	ulIntrStatus1 = in_channelHandle->ulIntrStatus1;
	BKNI_LeaveCriticalSection();

	BDBG_MSG(("ulIntrStatus1 = 0x%x\n", ulIntrStatus1)); 	
	BDBG_MSG(("in_channelHandle->ulIntrStatus1 = 0x%x\n", in_channelHandle->ulIntrStatus1)); 		
		/*09/20/05,Allen.C, check if the Card is removed  */	
		if ( (in_channelHandle-> bIsCardRemoved == true) &&
			(( ulIntrStatus1 & BCHP_SCA_SC_INTR_STAT_1_pres_intr_MASK ) == BCHP_SCA_SC_INTR_STAT_1_pres_intr_MASK)) {
			BKNI_EnterCriticalSection();    
			in_channelHandle->ulIntrStatus1 &= ~BCHP_SCA_SC_INTR_STAT_1_pres_intr_MASK;               
			in_channelHandle->channelStatus.ulStatus1  |= BSCD_RESET_CHANNEL_REQUIRED;
			in_channelHandle-> bIsCardRemoved = false ;
			BKNI_LeaveCriticalSection();
			BDBG_ERR(("ScardDeviceTimerEvent: SC_CARD_REMOVED error \n"));
	 		errCode = BSCD_STATUS_FAILED ;
		         goto BSCD_P_DONE_LABEL;
		}
		else if (( ulIntrStatus1 & BCHP_SCA_SC_INTR_STAT_1_timer_intr_MASK) != BCHP_SCA_SC_INTR_STAT_1_timer_intr_MASK) {
			
			BSCD_P_CHECK_ERR_CODE_FUNC(errCode, 
				BKNI_WaitForEvent( in_channelHandle->channelWaitEvent.timerWait, 
							in_channelHandle->currentChannelSettings.timeOut.ulValue));
					
		}

		

	} while  ((ulIntrStatus1 & BCHP_SCA_SC_INTR_STAT_1_timer_intr_MASK )!= 
					BCHP_SCA_SC_INTR_STAT_1_timer_intr_MASK);

	BKNI_EnterCriticalSection();
	in_channelHandle->ulIntrStatus1  &= ~BCHP_SCA_SC_INTR_STAT_1_timer_intr_MASK;
	BKNI_LeaveCriticalSection();	
	BDBG_MSG(("scard_timer_wait interrupt received\n")); 
	
BSCD_P_DONE_LABEL:

	BDBG_LEAVE(BSCD_Channel_P_WaitForTimerEvent);	
	return errCode;
}

BERR_Code BSCD_Channel_P_WaitForATRStart(
		BSCD_ChannelHandle	in_channelHandle
)
{
	BERR_Code errCode = BERR_SUCCESS;
	uint32_t ulIntrStatus1, ulIntrStatus2;

	BDBG_ENTER(BSCD_Channel_P_WaitForATRStart);

	BDBG_MSG(("Ready to receive scard_atrStart interrupt, ucSlot = %d\n", in_channelHandle->ucChannelNumber)); 

	BKNI_EnterCriticalSection();
	ulIntrStatus1 = in_channelHandle->ulIntrStatus1;
	ulIntrStatus2 = in_channelHandle->ulIntrStatus2 & BCHP_SCA_SC_INTR_STAT_2_atrs_intr_MASK;
	BKNI_LeaveCriticalSection();	

	BDBG_MSG(("ulIntrStatus1 = 0x%x, ulIntrStatus2 = 0x%xn", ulIntrStatus1, ulIntrStatus2)); 	
	BDBG_MSG(("in_channelHandle->ulIntrStatus1 = 0x%x\n", in_channelHandle->ulIntrStatus1)); 		
		
		
	do {		

		if ( (( ulIntrStatus1 & BCHP_SCA_SC_INTR_STAT_1_timer_intr_MASK) == BCHP_SCA_SC_INTR_STAT_1_timer_intr_MASK) &&
		      (ulIntrStatus2 == BCHP_SCA_SC_INTR_STAT_2_atrs_intr_MASK) ) {
		         goto BSCD_P_SUCCESS_LABEL;					
		}
		else if (( ulIntrStatus1 & BCHP_SCA_SC_INTR_STAT_1_timer_intr_MASK)  == BCHP_SCA_SC_INTR_STAT_1_timer_intr_MASK) {
			in_channelHandle->ulIntrStatus1 &= ~BCHP_SCA_SC_INTR_STAT_1_timer_intr_MASK;
			 BDBG_ERR(("ScardDeviceWaitForATRStart: SC_TIMER_INTR error \n"));
			 errCode = BSCD_STATUS_TIME_OUT;
			 goto BSCD_P_DONE_LABEL;
			
		}
		/*04/11/06,Allen.C, check if the Card is removed  */	
		else if ( (in_channelHandle-> bIsCardRemoved == true) &&
			(( ulIntrStatus1 & BCHP_SCA_SC_INTR_STAT_1_pres_intr_MASK ) == BCHP_SCA_SC_INTR_STAT_1_pres_intr_MASK)) {
			BKNI_EnterCriticalSection();    
			in_channelHandle->ulIntrStatus1 &= ~BCHP_SCA_SC_INTR_STAT_1_pres_intr_MASK;               
			in_channelHandle->channelStatus.ulStatus1  |= BSCD_RESET_CHANNEL_REQUIRED;
			in_channelHandle-> bIsCardRemoved = false ;
			BKNI_LeaveCriticalSection();
			BDBG_ERR(("ScardDeviceWaitForTDone: SC_CARD_REMOVED error \n"));
	 		errCode = BSCD_STATUS_FAILED ;
		         goto BSCD_P_DONE_LABEL;
		}		
		else if (ulIntrStatus2 != BCHP_SCA_SC_INTR_STAT_2_atrs_intr_MASK) {

			
			if ( (errCode = BERR_TRACE(BKNI_WaitForEvent( in_channelHandle->channelWaitEvent.atrStart, 
						in_channelHandle->currentChannelSettings.timeOut.ulValue))) != BERR_SUCCESS ) {
				in_channelHandle->channelStatus.ulStatus1 |= BSCD_RX_TIMEOUT; 
				errCode = BSCD_STATUS_TIME_OUT;			
				goto BSCD_P_DONE_LABEL;							
			}
					
		}

		BKNI_EnterCriticalSection();
		ulIntrStatus1 = in_channelHandle->ulIntrStatus1;
		ulIntrStatus2 = in_channelHandle->ulIntrStatus2 & BCHP_SCA_SC_INTR_STAT_2_atrs_intr_MASK;
		BKNI_LeaveCriticalSection();
	
	}while  (ulIntrStatus2 != BCHP_SCA_SC_INTR_STAT_2_atrs_intr_MASK);


BSCD_P_SUCCESS_LABEL:

	BKNI_EnterCriticalSection();	
	in_channelHandle->ulIntrStatus2  &= ~BCHP_SCA_SC_INTR_STAT_2_atrs_intr_MASK;
	BKNI_LeaveCriticalSection();	
	BDBG_MSG(("scard_atrStart interrupt received\n")); 
	
BSCD_P_DONE_LABEL:

	BDBG_LEAVE(BSCD_Channel_P_WaitForATRStart);	
	return errCode;
}


BERR_Code BSCD_Channel_P_WaitForTDone(
		BSCD_ChannelHandle	in_channelHandle
)
{
	BERR_Code errCode = BERR_SUCCESS;
	uint32_t ulIntrStatus1, ulIntrStatus2;

	BDBG_ENTER(BSCD_Channel_P_WaitForTDone);

	BDBG_MSG(("Ready to receive scard_tDone interrupt, ucSlot = %d\n", in_channelHandle->ucChannelNumber)); 

	BKNI_EnterCriticalSection();
	ulIntrStatus1 = in_channelHandle->ulIntrStatus1 ;
	ulIntrStatus2 = in_channelHandle->ulIntrStatus2;
	BKNI_LeaveCriticalSection();	
		
	do {		
 		if ( (in_channelHandle->currentChannelSettings.eProtocolType == BSCD_AsyncProtocolType_e1) &&
         				((ulIntrStatus1 & BCHP_SCA_SC_INTR_STAT_1_bgt_intr_MASK) ==
         					BCHP_SCA_SC_INTR_STAT_1_bgt_intr_MASK) ) {
			BKNI_EnterCriticalSection();         				
        		 in_channelHandle->ulIntrStatus1  &= ~BCHP_SCA_SC_INTR_STAT_1_bgt_intr_MASK;
			BKNI_LeaveCriticalSection();				 
         		BDBG_ERR(("ScardDeviceWaitForTDone: SC_BGT_INTR error \n"));
			 errCode = BSCD_STATUS_FAILED;
		         goto BSCD_P_DONE_LABEL;
      		}

		else if ( (in_channelHandle->currentChannelSettings.eProtocolType == BSCD_AsyncProtocolType_e0) &&
				(( ulIntrStatus1 & BCHP_SCA_SC_INTR_STAT_1_retry_intr_MASK) == 
				BCHP_SCA_SC_INTR_STAT_1_retry_intr_MASK) ) {
				
			BKNI_EnterCriticalSection();         				
        		in_channelHandle->ulIntrStatus1  &= ~BCHP_SCA_SC_INTR_STAT_1_retry_intr_MASK;
			BKNI_LeaveCriticalSection();			
			BDBG_ERR(("ScardDeviceWaitForTDone: RETRY_INTR error \n"));

			in_channelHandle->channelStatus.ulStatus1 |= BSCD_TX_PARITY;
			 errCode = BSCD_STATUS_FAILED;
		         goto BSCD_P_DONE_LABEL;
		}
		else if (( ulIntrStatus2 & BCHP_SCA_SC_INTR_STAT_2_wait_intr_MASK) == BCHP_SCA_SC_INTR_STAT_2_wait_intr_MASK) {
			BKNI_EnterCriticalSection();   
			in_channelHandle->ulIntrStatus2 &= ~BCHP_SCA_SC_INTR_STAT_2_wait_intr_MASK;
			BKNI_LeaveCriticalSection();	
			in_channelHandle->channelStatus.ulStatus1  |= BSCD_TX_TIMEOUT; 
			BDBG_ERR(("ScardDeviceWaitForTDone: SC_WAIT_INTR error \n"));
			 errCode = BSCD_STATUS_FAILED;
		         goto BSCD_P_DONE_LABEL;
		}
		/*09/20/05,Allen.C, check if the Card is removed  */	
		else if ( (in_channelHandle-> bIsCardRemoved == true) &&
			(( ulIntrStatus1 & BCHP_SCA_SC_INTR_STAT_1_pres_intr_MASK ) == BCHP_SCA_SC_INTR_STAT_1_pres_intr_MASK)) {
			BKNI_EnterCriticalSection();    
			in_channelHandle->ulIntrStatus1 &= ~BCHP_SCA_SC_INTR_STAT_1_pres_intr_MASK;               
			in_channelHandle->channelStatus.ulStatus1  |= BSCD_RESET_CHANNEL_REQUIRED;
			in_channelHandle-> bIsCardRemoved = false ;
			BKNI_LeaveCriticalSection();
			BDBG_ERR(("ScardDeviceWaitForTDone: SC_CARD_REMOVED error \n"));
	 		errCode = BSCD_STATUS_FAILED ;
		         goto BSCD_P_DONE_LABEL;
		}
		else if (( ulIntrStatus1 & BCHP_SCA_SC_INTR_STAT_1_tdone_intr_MASK) != BCHP_SCA_SC_INTR_STAT_1_tdone_intr_MASK) {
			if ( (errCode = BERR_TRACE(BKNI_WaitForEvent( in_channelHandle->channelWaitEvent.tdoneWait, 
						in_channelHandle->currentChannelSettings.timeOut.ulValue))) != BERR_SUCCESS ) {
				BKNI_EnterCriticalSection();
				in_channelHandle->channelStatus.ulStatus1 |= BSCD_TX_TIMEOUT; 
				BKNI_LeaveCriticalSection();				
				errCode = BSCD_STATUS_TIME_OUT;			
				goto BSCD_P_DONE_LABEL;							
			}			
		}

		BKNI_EnterCriticalSection();
		ulIntrStatus1 = in_channelHandle->ulIntrStatus1;
		ulIntrStatus2 = in_channelHandle->ulIntrStatus2;
		BKNI_LeaveCriticalSection();
	
	}while  ((ulIntrStatus1 & BCHP_SCA_SC_INTR_STAT_1_tdone_intr_MASK )!= 
					BCHP_SCA_SC_INTR_STAT_1_tdone_intr_MASK);

	BKNI_EnterCriticalSection();	
	in_channelHandle->ulIntrStatus1  &= ~BCHP_SCA_SC_INTR_STAT_1_tdone_intr_MASK;
	in_channelHandle->ulStatus1 &= ~BCHP_SCA_SC_STATUS_1_tdone_MASK;
	BKNI_LeaveCriticalSection();	
	BDBG_MSG(("tdone_intr interrupt received\n")); 

	
BSCD_P_DONE_LABEL:

	BDBG_LEAVE(BSCD_Channel_P_WaitForTDone);	
	return errCode;
}


BERR_Code BSCD_Channel_P_WaitForRcv(
		BSCD_ChannelHandle	in_channelHandle
)
{
	BERR_Code errCode = BERR_SUCCESS;
	uint32_t ulIntrStatus1, ulIntrStatus2, ulStatus2;

	BDBG_ENTER(BSCD_Channel_P_WaitForRcv);

	/* BDBG_MSG(("Ready to receive rcv interrupt\n"));  */

	BKNI_EnterCriticalSection();
	ulIntrStatus1 = in_channelHandle->ulIntrStatus1 ;
	ulIntrStatus2 = in_channelHandle->ulIntrStatus2;
	ulStatus2 =  in_channelHandle->ulStatus2;
	BKNI_LeaveCriticalSection();	
		
	do {		

		
		if ((ulIntrStatus1 & BCHP_SCA_SC_INTR_STAT_1_timer_intr_MASK) ==  BCHP_SCA_SC_INTR_STAT_1_timer_intr_MASK ) {
			BKNI_EnterCriticalSection();         				
     			in_channelHandle->channelStatus.ulStatus1  |= BSCD_RX_TIMEOUT; 
			BKNI_LeaveCriticalSection();	
			/* This could be a good error if the caller specify a length larger than that of  the actual one. */
         		BDBG_MSG(("ScardDeviceWaitForRcv: SC_TIMER_INTR error \n"));
			 errCode = BSCD_STATUS_TIME_OUT;
		         goto BSCD_P_DONE_LABEL;
      		}

		/* BSYT???:  Obsolete ??? */
		else if ( (in_channelHandle->currentChannelSettings.eProtocolType == BSCD_AsyncProtocolType_e1) &&
			      ((ulIntrStatus1 & BCHP_SCA_SC_INTR_STAT_1_bgt_intr_MASK) == BCHP_SCA_SC_INTR_STAT_1_bgt_intr_MASK) ) {
			BKNI_EnterCriticalSection();         				
     			in_channelHandle->ulIntrStatus1  &= ~BCHP_SCA_SC_INTR_STAT_1_bgt_intr_MASK; 
			BKNI_LeaveCriticalSection();			
			BDBG_ERR(("ScardDeviceWaitForRcv: SC_BGT_INTR error \n"));
			errCode = BSCD_STATUS_READ_FAILED;
		         goto BSCD_P_DONE_LABEL;
		}		
		else if ( ( ulIntrStatus2 & BCHP_SCA_SC_INTR_STAT_2_event2_intr_MASK) == BCHP_SCA_SC_INTR_STAT_2_event2_intr_MASK)  {
					
			BKNI_EnterCriticalSection();  
			in_channelHandle->ulIntrStatus2 &= ~BCHP_SCA_SC_INTR_STAT_2_event2_intr_MASK;  
			in_channelHandle->channelStatus.ulStatus2  |= BSCD_RX_TIMEOUT;			
			BKNI_LeaveCriticalSection();			    
			BDBG_ERR(("ScardDeviceWaitForRcv: SC_EVENT2_INTR error \n"));
			errCode = BSCD_STATUS_FAILED;
		         goto BSCD_P_DONE_LABEL;				 
		}		
		else if ((in_channelHandle->currentChannelSettings.eProtocolType == BSCD_AsyncProtocolType_e0 ) &&
			(in_channelHandle->currentChannelSettings.scStandard != BSCD_Standard_eIrdeto) &&
			   (( ulIntrStatus1 & BCHP_SCA_SC_INTR_STAT_1_retry_intr_MASK) == BCHP_SCA_SC_INTR_STAT_1_retry_intr_MASK)) {
			BKNI_EnterCriticalSection();    			
			in_channelHandle->ulIntrStatus1 &= ~BCHP_SCA_SC_INTR_STAT_1_retry_intr_MASK;         
			BKNI_LeaveCriticalSection();			
			BDBG_ERR(("ScardDeviceWaitForRcv: SC_RETRY_INTR error \n"));
			in_channelHandle->channelStatus.ulStatus1 |= BSCD_RX_PARITY;
			errCode = BSCD_STATUS_PARITY_EDC_ERR;
			goto BSCD_P_DONE_LABEL;
		}

		else if ( (in_channelHandle->currentChannelSettings.eProtocolType == BSCD_AsyncProtocolType_e1) &&
			((ulIntrStatus2 & BCHP_SCA_SC_INTR_STAT_2_rlen_intr_MASK) == BCHP_SCA_SC_INTR_STAT_2_rlen_intr_MASK) ) {
			BKNI_EnterCriticalSection();    			
			in_channelHandle->ulIntrStatus2 &= ~BCHP_SCA_SC_INTR_STAT_2_rlen_intr_MASK;    
			BKNI_LeaveCriticalSection();				
			BDBG_ERR(("ScardDeviceWaitForRcv: SC_RLEN_INTR error \n"));
			errCode = BSCD_STATUS_READ_FAILED;
			goto BSCD_P_DONE_LABEL;
		}
		else if (( ulIntrStatus2 & BCHP_SCA_SC_INTR_STAT_2_wait_intr_MASK) == BCHP_SCA_SC_INTR_STAT_2_wait_intr_MASK) {
			BKNI_EnterCriticalSection();    
			in_channelHandle->ulIntrStatus2 &= ~BCHP_SCA_SC_INTR_STAT_2_wait_intr_MASK;               
			in_channelHandle->channelStatus.ulStatus1  |= BSCD_RX_TIMEOUT;
			BKNI_LeaveCriticalSection();
			BDBG_ERR(("ScardDeviceWaitForRcv: SC_WAIT_INTR error \n"));
	 		errCode = BSCD_STATUS_TIME_OUT;
		         goto BSCD_P_DONE_LABEL;
		}
		else if (( ulIntrStatus2 & BCHP_SCA_SC_INTR_STAT_2_cwt_intr_MASK) == BCHP_SCA_SC_INTR_STAT_2_cwt_intr_MASK) {
			BKNI_EnterCriticalSection();  
			in_channelHandle->ulIntrStatus2 &= ~BCHP_SCA_SC_INTR_STAT_2_cwt_intr_MASK;               
			in_channelHandle->channelStatus.ulStatus1  |= BSCD_RX_TIMEOUT;
			BKNI_LeaveCriticalSection();
			BDBG_ERR(("ScardDeviceWaitForRcv: SC_CWT_INTR error \n"));
	 		errCode = BSCD_STATUS_TIME_OUT;
		         goto BSCD_P_DONE_LABEL;
		}
		else if (( ulStatus2 & BCHP_SCA_SC_STATUS_2_roverflow_MASK) == BCHP_SCA_SC_STATUS_2_roverflow_MASK) {
			BKNI_EnterCriticalSection(); 
			in_channelHandle->ulStatus2 &= ~BCHP_SCA_SC_STATUS_2_roverflow_MASK;
			BKNI_LeaveCriticalSection();
			BDBG_ERR(("ScardDeviceWaitForRcv: SC_ROVERFLOW error \n"));
			errCode = BSCD_STATUS_READ_FAILED;
		         goto BSCD_P_DONE_LABEL;
		}
		/*09/20/05,Allen.C, check if the Card is removed  */	
		else if ( (in_channelHandle-> bIsCardRemoved == true) &&
			(( ulIntrStatus1 & BCHP_SCA_SC_INTR_STAT_1_pres_intr_MASK ) == BCHP_SCA_SC_INTR_STAT_1_pres_intr_MASK)) {
			BKNI_EnterCriticalSection();    
			in_channelHandle->ulIntrStatus1 &= ~BCHP_SCA_SC_INTR_STAT_1_pres_intr_MASK;               
			in_channelHandle->channelStatus.ulStatus1  |= BSCD_RESET_CHANNEL_REQUIRED;
			in_channelHandle-> bIsCardRemoved = false ;
			BKNI_LeaveCriticalSection();
			BDBG_ERR(("ScardDeviceWaitForRcv: SC_CARD_REMOVED error \n"));
	 		errCode = BSCD_STATUS_FAILED ;
		         goto BSCD_P_DONE_LABEL;
		}
		else if (( ulIntrStatus2 & BCHP_SCA_SC_INTR_STAT_2_rcv_intr_MASK) != BCHP_SCA_SC_INTR_STAT_2_rcv_intr_MASK) {
			if ( (errCode = BERR_TRACE(BKNI_WaitForEvent( in_channelHandle->channelWaitEvent.rcvWait, 
						in_channelHandle->currentChannelSettings.timeOut.ulValue))) != BERR_SUCCESS ) {
				BKNI_EnterCriticalSection();						
				in_channelHandle->channelStatus.ulStatus1 |= BSCD_RX_TIMEOUT; 
				BKNI_LeaveCriticalSection();				
				errCode = BSCD_STATUS_TIME_OUT;			
				goto BSCD_P_DONE_LABEL;							
			}			
		}

		BKNI_EnterCriticalSection();
		ulIntrStatus1 = in_channelHandle->ulIntrStatus1;
		ulIntrStatus2 = in_channelHandle->ulIntrStatus2;
		ulStatus2 =  in_channelHandle->ulStatus2;		
		BKNI_LeaveCriticalSection();
	
	}while  ((ulIntrStatus2 & BCHP_SCA_SC_INTR_STAT_2_rcv_intr_MASK ) != 
					BCHP_SCA_SC_INTR_STAT_2_rcv_intr_MASK);
	

	BKNI_EnterCriticalSection();	
	in_channelHandle->ulIntrStatus2  &= ~BCHP_SCA_SC_INTR_STAT_2_rcv_intr_MASK;
	in_channelHandle->ulStatus2 |= BCHP_SCA_SC_STATUS_2_rempty_MASK;
	BKNI_LeaveCriticalSection();	
	/* BDBG_MSG(("rcv interrupt received\n"));  */
	
BSCD_P_DONE_LABEL:

	BDBG_LEAVE(BSCD_Channel_P_WaitForRcv);	
	/* BDBG_MSG(("LeaveWaitForRcv errCode = 0x%x \n", errCode));	 */
	return errCode;
}

BERR_Code BSCD_Channel_P_WaitForRReady(
		BSCD_ChannelHandle	in_channelHandle
)
{
	BERR_Code errCode = BERR_SUCCESS;
	uint32_t ulIntrStatus1, ulIntrStatus2, ulStatus2;
#ifdef BSCD_EMV2000_CWT_PLUS_4_EVENT_INTR
	uint32_t ulVal;
#endif	 


	BDBG_ENTER(BSCD_Channel_P_WaitForRReady);

	BDBG_MSG(("Ready to receive rready interrupt\n")); 

	BKNI_EnterCriticalSection();
	ulIntrStatus1 = in_channelHandle->ulIntrStatus1 ;
	ulIntrStatus2 = in_channelHandle->ulIntrStatus2;
	ulStatus2 =  in_channelHandle->ulStatus2;
	BKNI_LeaveCriticalSection();	
	

		
	do {		


		BDBG_MSG(("ulIntrStatus1 = 0x%x, ulIntrStatus2 = 0x%x, ulStatus2 = 0x%x \n", 
				ulIntrStatus1, ulIntrStatus2, ulStatus2)); 			

		if ((ulIntrStatus1 & BCHP_SCA_SC_INTR_STAT_1_bgt_intr_MASK) == BCHP_SCA_SC_INTR_STAT_1_bgt_intr_MASK) {
			BKNI_EnterCriticalSection();         				
			in_channelHandle->ulIntrStatus1  &= ~BCHP_SCA_SC_INTR_STAT_1_bgt_intr_MASK; 
			BKNI_LeaveCriticalSection();  
			BDBG_ERR(("ScardDeviceWaitForRReady: SC_BGT_INTR error \n"));
			errCode = BSCD_STATUS_FAILED;
			goto BSCD_P_DONE_LABEL;
		}


#if 0  /* Do not need this since T=1 parity and rlen check will be perform when we read in each byte */		
		else if ( (in_channelHandle->currentChannelSettings.eProtocolType != BSCD_AsyncProtocolType_e1) &&
				(( ulIntrStatus1 & BCHP_SCA_SC_INTR_STAT_1_retry_intr_MASK) == BCHP_SCA_SC_INTR_STAT_1_retry_intr_MASK) ) {
			BKNI_EnterCriticalSection();    			
			in_channelHandle->ulIntrStatus1 &= ~BCHP_SCA_SC_INTR_STAT_1_retry_intr_MASK;         
			BKNI_LeaveCriticalSection();	    
			/* For T = 1, retry interrupt is not required while reading a block from the ICC */
			BDBG_ERR(("ScardDeviceWaitForRReady: SC_RETRY_INTR error \n"));
			in_channelHandle->channelStatus.ulStatus1 |= BSCD_RX_PARITY;
			errCode = BSCD_STATUS_FAILED;
			goto BSCD_P_DONE_LABEL;
		}
		else if ( (in_channelHandle->currentChannelSettings.eProtocolType != BSCD_AsyncProtocolType_e1) &&
				((ulIntrStatus2 & BCHP_SCA_SC_INTR_STAT_2_rpar_intr_MASK) == BCHP_SCA_SC_INTR_STAT_2_rpar_intr_MASK) ) {
			/* For T = 1, parity interrupt is not required while reading a block from the ICC */
			BKNI_EnterCriticalSection();
			in_channelHandle->ulIntrStatus2 &= ~BCHP_SCA_SC_INTR_STAT_2_rpar_intr_MASK; 
			BKNI_LeaveCriticalSection();	
			BDBG_ERR(("ScardDeviceWaitForRReady: SC_RPAR_INTR error \n"));
			errCode = BSCD_STATUS_FAILED;
			goto BSCD_P_DONE_LABEL;
		}

		else if ( (in_channelHandle->currentChannelSettings.eProtocolType != BSCD_AsyncProtocolType_e1) &&
				((ulStatus2 & BCHP_SCA_SC_STATUS_2_edc_err_MASK) == BCHP_SCA_SC_STATUS_2_edc_err_MASK) ) {
			/* For T = 1, EDC interrupt is not required while reading a block from the ICC */    
			BKNI_EnterCriticalSection();			
			in_channelHandle->ulStatus2&= ~BCHP_SCA_SC_STATUS_2_edc_err_MASK;
			BKNI_LeaveCriticalSection();				
			BDBG_ERR(("ScardDeviceWaitForRReady: SC_EDC_ERR error \n"));
			errCode = BSCD_STATUS_FAILED;
			goto BSCD_P_DONE_LABEL;
		}
		
#endif		  

			
#ifdef BSCD_EMV2000_CWT_PLUS_4
		else if ( (( ulIntrStatus2 & BCHP_SCA_SC_INTR_STAT_2_cwt_intr_MASK) == BCHP_SCA_SC_INTR_STAT_2_cwt_intr_MASK) && 
			(( ulIntrStatus2 & BCHP_SCA_SC_INTR_STAT_2_rready_intr_MASK) != BCHP_SCA_SC_INTR_STAT_2_rready_intr_MASK) &&
			(in_channelHandle->currentChannelSettings.scStandard != BSCD_Standard_eEMV2000)) {				
			BKNI_EnterCriticalSection();  
			in_channelHandle->ulIntrStatus2 &= ~BCHP_SCA_SC_INTR_STAT_2_cwt_intr_MASK;  
			in_channelHandle->channelStatus.ulStatus1  |= BSCD_RX_TIMEOUT;			
			BKNI_LeaveCriticalSection();			    
			BDBG_ERR(("ScardDeviceWaitForRReady: SC_CWT_INTR error \n"));
			errCode = BSCD_STATUS_FAILED;
		         goto BSCD_P_DONE_LABEL;				 
		}	
#elif defined(BSCD_EMV2000_CWT_PLUS_4_EVENT_INTR)			
		else if ( (( ulIntrStatus1 & BCHP_SCA_SC_INTR_STAT_1_event1_intr_MASK) == BCHP_SCA_SC_INTR_STAT_1_event1_intr_MASK) && 
			(( ulIntrStatus2 & BCHP_SCA_SC_INTR_STAT_2_rready_intr_MASK) != BCHP_SCA_SC_INTR_STAT_2_rready_intr_MASK) ) {				
					
			BKNI_EnterCriticalSection();  
			in_channelHandle->ulIntrStatus1 &= ~BCHP_SCA_SC_INTR_STAT_1_event1_intr_MASK;  
			in_channelHandle->channelStatus.ulStatus1  |= BSCD_RX_TIMEOUT;			
			BKNI_LeaveCriticalSection();			    
			BDBG_ERR(("ScardDeviceWaitForRReady: SC_EVENT1_INTR error \n"));
			errCode = BSCD_STATUS_FAILED;
		         goto BSCD_P_DONE_LABEL;				 
		}		
#else
		else if ( (( ulIntrStatus2 & BCHP_SCA_SC_INTR_STAT_2_cwt_intr_MASK) == BCHP_SCA_SC_INTR_STAT_2_cwt_intr_MASK) && 
			(( ulIntrStatus2 & BCHP_SCA_SC_INTR_STAT_2_rready_intr_MASK) != BCHP_SCA_SC_INTR_STAT_2_rready_intr_MASK) ) {				
			BKNI_EnterCriticalSection();  
			in_channelHandle->ulIntrStatus2 &= ~BCHP_SCA_SC_INTR_STAT_2_cwt_intr_MASK;  
			in_channelHandle->channelStatus.ulStatus1  |= BSCD_RX_TIMEOUT;			
			BKNI_LeaveCriticalSection();			    
			BDBG_ERR(("ScardDeviceWaitForRReady: SC_CWT_INTR error \n"));
			errCode = BSCD_STATUS_FAILED;
		         goto BSCD_P_DONE_LABEL;				 
		}
#endif		

		else if ( (in_channelHandle->currentChannelSettings.eProtocolType == BSCD_AsyncProtocolType_e1) &&
			((ulIntrStatus2 & BCHP_SCA_SC_INTR_STAT_2_rlen_intr_MASK) == BCHP_SCA_SC_INTR_STAT_2_rlen_intr_MASK) ) {
			BKNI_EnterCriticalSection();    			
			in_channelHandle->ulIntrStatus2 &= ~BCHP_SCA_SC_INTR_STAT_2_rlen_intr_MASK;    
			BKNI_LeaveCriticalSection();				
			BDBG_ERR(("ScardDeviceWaitForRReady: SC_RLEN_INTR error \n"));
			errCode = BSCD_STATUS_FAILED;
			goto BSCD_P_DONE_LABEL;
		}

		else if (( ulIntrStatus2 & BCHP_SCA_SC_INTR_STAT_2_wait_intr_MASK) == BCHP_SCA_SC_INTR_STAT_2_wait_intr_MASK) {
			BKNI_EnterCriticalSection();    
			in_channelHandle->ulIntrStatus2 &= ~BCHP_SCA_SC_INTR_STAT_2_wait_intr_MASK;               
			in_channelHandle->channelStatus.ulStatus1  |= BSCD_RX_TIMEOUT;
			BKNI_LeaveCriticalSection();
			BDBG_ERR(("ScardDeviceWaitForRReady: SC_WAIT_INTR error \n"));
	 		errCode = BSCD_STATUS_TIME_OUT;
		         goto BSCD_P_DONE_LABEL;
		}

		else if (( ulStatus2 & BCHP_SCA_SC_STATUS_2_roverflow_MASK) == BCHP_SCA_SC_STATUS_2_roverflow_MASK) {
			BKNI_EnterCriticalSection();    
			in_channelHandle->ulStatus2 &= ~BCHP_SCA_SC_STATUS_2_roverflow_MASK;               
			BKNI_LeaveCriticalSection();			
			BDBG_ERR(("ScardDeviceWaitForRReady: SC_ROVERFLOW error \n"));
			errCode = BSCD_STATUS_FAILED;
		         goto BSCD_P_DONE_LABEL;
		}
		/*09/20/05,Allen.C, check if the Card is removed  */	
		else if ( (in_channelHandle-> bIsCardRemoved == true) &&
			(( ulIntrStatus1 & BCHP_SCA_SC_INTR_STAT_1_pres_intr_MASK ) == BCHP_SCA_SC_INTR_STAT_1_pres_intr_MASK)) {
			BKNI_EnterCriticalSection();    
			in_channelHandle->ulIntrStatus1 &= ~BCHP_SCA_SC_INTR_STAT_1_pres_intr_MASK;               
			in_channelHandle->channelStatus.ulStatus1  |= BSCD_RESET_CHANNEL_REQUIRED;
			in_channelHandle-> bIsCardRemoved = false ;
			BKNI_LeaveCriticalSection();
			BDBG_ERR(("ScardDeviceWaitForRReady: SC_CARD_REMOVED error \n"));
	 		errCode = BSCD_STATUS_FAILED ;
		         goto BSCD_P_DONE_LABEL;
		}

		else if (( ulIntrStatus2 & BCHP_SCA_SC_INTR_STAT_2_rready_intr_MASK) != BCHP_SCA_SC_INTR_STAT_2_rready_intr_MASK) {
			if ( (errCode = BERR_TRACE(BKNI_WaitForEvent( in_channelHandle->channelWaitEvent.rcvWait, 
						in_channelHandle->currentChannelSettings.timeOut.ulValue))) != BERR_SUCCESS ) {
				
				BKNI_EnterCriticalSection();						
				in_channelHandle->channelStatus.ulStatus1 |= BSCD_RX_TIMEOUT; 
				BKNI_LeaveCriticalSection();	
				BDBG_ERR(("ScardDeviceWaitForRReady: BKNI_WaitForEvent timeout error %d\n", in_channelHandle->currentChannelSettings.timeOut.ulValue));
				errCode = BSCD_STATUS_TIME_OUT;				
				goto BSCD_P_DONE_LABEL;							
			}			
		}

		BKNI_EnterCriticalSection();
		ulIntrStatus1 = in_channelHandle->ulIntrStatus1;
		ulIntrStatus2 = in_channelHandle->ulIntrStatus2;
		ulStatus2 =  in_channelHandle->ulStatus2;		
		BKNI_LeaveCriticalSection();
	
	}while  (( ulIntrStatus2 & BCHP_SCA_SC_INTR_STAT_2_rready_intr_MASK) != 
	BCHP_SCA_SC_INTR_STAT_2_rready_intr_MASK); 

	BKNI_EnterCriticalSection();	
	in_channelHandle->ulIntrStatus2  &= ~BCHP_SCA_SC_INTR_STAT_2_rready_intr_MASK;
	in_channelHandle->ulStatus2  &= ~BCHP_SCA_SC_STATUS_2_rready_MASK;
	BKNI_LeaveCriticalSection();	
	BDBG_MSG(("rready interrupt received\n")); 

	
BSCD_P_DONE_LABEL:
	
#ifdef BSCD_EMV2000_CWT_PLUS_4_EVENT_INTR
			/* Disable event1 */
			ulVal = BREG_Read32( 
				in_channelHandle->moduleHandle->regHandle, 
				(in_channelHandle->ulRegStartAddr + BSCD_P_EVENT1_CMD_4));
			ulVal &= ~(BCHP_SCA_SC_EVENT1_CMD_4_event_en_MASK);
			BREG_Write32( 
				in_channelHandle->moduleHandle->regHandle, 
				(in_channelHandle->ulRegStartAddr + BSCD_P_EVENT1_CMD_4), 
				 ulVal); 
#endif

	BDBG_LEAVE(BSCD_Channel_P_WaitForRReady);	
	BDBG_MSG(("BSCD_Channel_P_WaitForRReady errCode = 0x%x\n", errCode));

#ifdef BSCD_EMV2000_CWT_PLUS_4
	in_channelHandle->bIsReceive = false;
#endif

	return errCode;
}

BERR_Code BSCD_Channel_P_Activating(
		BSCD_ChannelHandle	in_channelHandle
)
{

	BERR_Code		errCode = BERR_SUCCESS;
	uint32_t        ulIFCmdVal, ulVal;
	BSCD_Timer 		timer = {BSCD_TimerType_eGPTimer, {BSCD_GPTimerMode_eIMMEDIATE}, true, true};	
	BSCD_TimerValue timeValue= {BSCD_MAX_RESET_IN_CLK_CYCLES, BSCD_TimerUnit_eCLK};

	BSCD_Timer 		wwtTimer = {BSCD_TimerType_eWaitTimer, {BSCD_GPTimerMode_eIMMEDIATE}, true, true};	
	BSCD_TimerValue    wwtTimeValue= {BSCD_MAX_ETU_PER_ATR_BYTE_EMV2000, BSCD_TimerUnit_eETU};

	
	unsigned char   i;
	uint32_t        ulTimerCntVal1, ulTimerCntVal2;
	uint32_t        ulTimerCntVal;
	uint32_t        ulPrevTimerCntVal = 0;

	BDBG_ENTER(BSCD_Channel_P_Activating);
	BDBG_ASSERT( in_channelHandle );

	BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, 
		(in_channelHandle == NULL ) );
	
	BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, 
		(in_channelHandle->ulMagicNumber != BSCD_P_CHANNEL_HANDLE_MAGIC_NUMBER ) );

	/* Set SC_VCC low = CMDVCC low which in turn starts the 
 		VCC signal rising in the Phillips chip */
	ulIFCmdVal =  BREG_Read32( 
		in_channelHandle->moduleHandle->regHandle, 
		(in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1)) ;	

	/* Use Auto Deactivation instead of TDA8004 */
        if(in_channelHandle->currentChannelSettings.bConnectDirectly == true){
			if(in_channelHandle->currentChannelSettings.bDirectVccInverted == true)
		ulIFCmdVal &= ~(BCHP_SCA_SC_IF_CMD_1_vcc_MASK);
			else
          ulIFCmdVal |= BCHP_SCA_SC_IF_CMD_1_vcc_MASK;
	}
	else {
		ulIFCmdVal &= ~(BCHP_SCA_SC_IF_CMD_1_vcc_MASK);
	}
        if (in_channelHandle->currentChannelSettings.bAutoDeactiveReq == true) {      
          ulIFCmdVal |= BCHP_SCA_SC_IF_CMD_1_auto_vcc_MASK; 
        }
	
	BREG_Write32( 
		in_channelHandle->moduleHandle->regHandle, 
		(in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1), 
		 ulIFCmdVal);

if(in_channelHandle->currentChannelSettings.bConnectDirectly == true){	
	if(in_channelHandle->currentChannelSettings.bDirectRstInverted == true)                 
  ulIFCmdVal |= BCHP_SCA_SC_IF_CMD_1_rst_MASK;
	else	/* Set SC_RST low = RSTIN low */
	ulIFCmdVal &= ~BCHP_SCA_SC_IF_CMD_1_rst_MASK;


}
else{

	/* Set SC_RST low = RSTIN low */
	ulIFCmdVal &= ~BCHP_SCA_SC_IF_CMD_1_rst_MASK;
}
        ulIFCmdVal &= ~BCHP_SCA_SC_IF_CMD_1_auto_rst_MASK; 
	BREG_Write32( 
		in_channelHandle->moduleHandle->regHandle, 
		(in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1), 
		 ulIFCmdVal);

	BDBG_MSG(("Activating: SC_RST low \n"));
	
	/* wait for 42,000 clk cycles. */	
	for (i=0; i<in_channelHandle->currentChannelSettings.ucExternalClockDivisor; i++) {

		timer.bIsTimerInterruptEnable = true;
		timer.bIsTimerEnable = true;
		timeValue.ulValue = in_channelHandle->currentChannelSettings.eResetCycles;
		BSCD_P_CHECK_ERR_CODE_FUNC(errCode, 
			BSCD_Channel_ConfigTimer(in_channelHandle, &timer, &timeValue));	

		BSCD_P_CHECK_ERR_CODE_FUNC(errCode, 
			BSCD_Channel_P_WaitForTimerEvent(in_channelHandle));	

		/* Disable timer */
		timer.bIsTimerInterruptEnable = false;
		timer.bIsTimerEnable = false;
		BSCD_P_CHECK_ERR_CODE_FUNC(errCode, 
			BSCD_Channel_EnableDisableTimer_isr(in_channelHandle, &timer));	

	}
 
	/*****************************************************
	**
	**  Set all required registers before we receive ATR 
	**
	******************************************************/
	/* Set this to 0 temporarily during ATR session.  For EMV,
		we will set it back in BSCD_Channel_P_EMVATRReceiveAndDecode.
		For the rest, the application should set it back */
	BREG_Write32( 
		in_channelHandle->moduleHandle->regHandle, 
		(in_channelHandle->ulRegStartAddr + BSCD_P_UART_CMD_2), 
		 0);	

	/* Enable 2 interrupts with callback */
	BSCD_P_CHECK_ERR_CODE_FUNC(errCode, 	
 				BSCD_Channel_EnableIntrCallback_isr (
				in_channelHandle, BSCD_IntType_eATRInt,
		                BSCD_Channel_P_ATRCB_isr));

	BSCD_P_CHECK_ERR_CODE_FUNC(errCode, 
				BSCD_Channel_EnableIntrCallback_isr (
				in_channelHandle, BSCD_IntType_eRcvInt,
		                BSCD_Channel_P_RcvCB_isr));	

	/* Enable WWT to ensure the max interval between 2 consecutive ATR chars of 10080 ETU */
	BDBG_MSG(("Activating: Set WWT timer \n"));
	if (in_channelHandle->currentChannelSettings.scStandard == BSCD_Standard_eEMV2000) 
		wwtTimeValue.ulValue = BSCD_MAX_ETU_PER_ATR_BYTE_EMV2000;	
	else /* EMV 96 or the rest */
		wwtTimeValue.ulValue = BSCD_MAX_ETU_PER_ATR_BYTE;		
	wwtTimer.timerMode.eWaitTimerMode = BSCD_WaitTimerMode_eWorkWaitTime;
	BSCD_P_CHECK_ERR_CODE_FUNC(errCode, 
			BSCD_Channel_ConfigTimer(in_channelHandle, &wwtTimer, &wwtTimeValue));				
	

	/* Set BCM to get ATR packet.       */
	ulVal =  BREG_Read32( 
		in_channelHandle->moduleHandle->regHandle, 
		(in_channelHandle->ulRegStartAddr + BSCD_P_UART_CMD_1)) ;	

	ulVal |= (BCHP_SCA_SC_UART_CMD_1_get_atr_MASK | BCHP_SCA_SC_UART_CMD_1_io_en_MASK);

	BREG_Write32( 
		in_channelHandle->moduleHandle->regHandle, 
		(in_channelHandle->ulRegStartAddr + BSCD_P_UART_CMD_1), 
		 ulVal);

	/* Set RST */
if(in_channelHandle->currentChannelSettings.bConnectDirectly == true){	
	if(in_channelHandle->currentChannelSettings.bDirectRstInverted == true)             
		ulIFCmdVal &= ~BCHP_SCA_SC_IF_CMD_1_rst_MASK;
	else
		ulIFCmdVal |= BCHP_SCA_SC_IF_CMD_1_rst_MASK;

	}
	else {
		ulIFCmdVal |= BCHP_SCA_SC_IF_CMD_1_rst_MASK;
	}
if(in_channelHandle->currentChannelSettings.bAutoDeactiveReq == true){
        ulIFCmdVal |= BCHP_SCA_SC_IF_CMD_1_auto_rst_MASK; 
}
	BREG_Write32( 
		in_channelHandle->moduleHandle->regHandle, 
		(in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1), 
		 ulIFCmdVal);	

	/* wait for 40,000 clk cycles for EMV96 and 42000 for EMV2000 */
	for (i=0; i<in_channelHandle->currentChannelSettings.ucExternalClockDivisor ; i++)  {

		/* Set Timer */
		timer.bIsTimerInterruptEnable = true;
		timer.bIsTimerEnable = true;
		timer.eTimerType = BSCD_TimerType_eGPTimer;
		timer.timerMode.eGPTimerMode = BSCD_GPTimerMode_eIMMEDIATE;

		if (in_channelHandle->currentChannelSettings.scStandard == BSCD_Standard_eEMV2000){
			timeValue.ulValue = BSCD_EMV2000_MAX_ATR_START_IN_CLK_CYCLES + BSCD_ATR_START_BIT_DELAY_IN_CLK_CYCLES;
		timeValue.unit  = BSCD_TimerUnit_eCLK;
		}
		else{
 			if(in_channelHandle->currentChannelSettings.ATRRecvTimeInteger.unit == BSCD_TimerUnit_eMilliSec){
				timeValue.unit	=	BSCD_TimerUnit_eETU;
				timeValue.ulValue = in_channelHandle->currentChannelSettings.ATRRecvTimeInteger.ulValue*1000/372+
					BSCD_ATR_START_BIT_DELAY_IN_CLK_CYCLES/372;
			}else{
				timeValue.unit  =	in_channelHandle->currentChannelSettings.ATRRecvTimeInteger.unit;
			
				timeValue.ulValue = in_channelHandle->currentChannelSettings.ATRRecvTimeInteger.ulValue + 
				BSCD_ATR_START_BIT_DELAY_IN_CLK_CYCLES/((timeValue.unit ==BSCD_TimerUnit_eCLK)? 1:372 );
					}
		
		}
	
		BDBG_MSG(("Activating: Set GP timer \n"));		
		BSCD_P_CHECK_ERR_CODE_FUNC(errCode, 
			BSCD_Channel_ConfigTimer(in_channelHandle, &timer, &timeValue));	

		if ((errCode = BSCD_Channel_P_WaitForATRStart(in_channelHandle)) != BERR_SUCCESS) {
		
			/* Disable timer */
			timer.bIsTimerInterruptEnable = false;
			timer.bIsTimerEnable = false;
		
			BSCD_Channel_EnableDisableTimer_isr(in_channelHandle, &timer);	
	
			if (errCode == BSCD_STATUS_TIME_OUT) {

				if (i == (in_channelHandle->currentChannelSettings.ucExternalClockDivisor -1)) {

					/* if this is the last loop and we still timeout, major error */
					/* Need to return deactivate for EMV2000 test 1719 xy=30 */
					BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_DEACTIVATE, true); 
				}
				else {
				/* If this is not the last loop, continue */
					if (in_channelHandle->currentChannelSettings.scStandard == BSCD_Standard_eEMV2000)
					ulPrevTimerCntVal += BSCD_MAX_ATR_START_IN_CLK_CYCLES;
					else
						ulPrevTimerCntVal += in_channelHandle->currentChannelSettings.ATRRecvTimeInteger.ulValue;
					continue;
				}
			}
			else {
				/* If the error is not scTimeOut, major error */
				BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, true); 

			}

		}


		/* Disable timer */
		timer.bIsTimerInterruptEnable = false;
		timer.bIsTimerEnable = false;
		BSCD_P_CHECK_ERR_CODE_FUNC(errCode, 
			BSCD_Channel_EnableDisableTimer_isr(in_channelHandle, &timer));		
		BDBG_MSG(("Activating: Disable GP timer \n"));
		
		/* Read timer counter, the ATR shall be received after 400 clock cycles */
		ulTimerCntVal2 =  BREG_Read32( 
				in_channelHandle->moduleHandle->regHandle, 
				(in_channelHandle->ulRegStartAddr + BSCD_P_TIMER_CNT_2)) ;	
		ulTimerCntVal1 =  BREG_Read32( 
				in_channelHandle->moduleHandle->regHandle, 
				(in_channelHandle->ulRegStartAddr + BSCD_P_TIMER_CNT_1)) ;

		ulTimerCntVal =  ((( ulTimerCntVal2) << 8) | ulTimerCntVal1) + ulPrevTimerCntVal;

#ifdef BSCD_DSS_ICAM
		if ((ulTimerCntVal < (BSCD_MIN_ATR_START_IN_CLK_CYCLES*in_channelHandle->currentChannelSettings.ucScClkDiv/((in_channelHandle->currentChannelSettings.unPrescale+1)*in_channelHandle->currentChannelSettings.ucBaudDiv) * in_channelHandle->currentChannelSettings.ucExternalClockDivisor) ) ||
			(ulTimerCntVal > timeValue.ulValue) ) 
#else
		if ((ulTimerCntVal < (uint32_t)(BSCD_MIN_ATR_START_IN_CLK_CYCLES * in_channelHandle->currentChannelSettings.ucExternalClockDivisor) ) ||
			(ulTimerCntVal > timeValue.ulValue) ) 
#endif			
		{

			BDBG_MSG(("PreATRREceive: ulTimerCmdVal = %lu\n", ulTimerCntVal));
			/* Need to return deactivate for EMV2000 test 1719 xy=30 */
			BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_DEACTIVATE, true); 
		}

		/* 
			Enable WWT to ensure all ATR bytes are received within certain time
		*/
		if((in_channelHandle->currentChannelSettings.scStandard == BSCD_Standard_eEMV2000)|| (in_channelHandle->currentChannelSettings.scStandard == BSCD_Standard_eEMV1996))
		{
		if (in_channelHandle->currentChannelSettings.scStandard == BSCD_Standard_eEMV2000) 
				timeValue.ulValue = BSCD_MAX_EMV_ETU_FOR_ALL_ATR_BYTES_EMV2000;	
		else /* EMV 96 or the rest */
				timeValue.ulValue = BSCD_MAX_EMV_ETU_FOR_ALL_ATR_BYTES;	
			timeValue.unit = BSCD_TimerUnit_eETU;
			timer.bIsTimerInterruptEnable = true;
			timer.bIsTimerEnable = true;
		BSCD_P_CHECK_ERR_CODE_FUNC(errCode, 
				BSCD_Channel_ConfigTimer(in_channelHandle, &timer, &timeValue));
			}
		BDBG_MSG(("ulTimerCntVal = %lu, MIN_ATR_START_IN_CLK_CYCLES = %d\n", ulTimerCntVal, BSCD_MIN_ATR_START_IN_CLK_CYCLES));

		if (errCode == BERR_SUCCESS) {   	  
			break;
		}
	}


BSCD_P_DONE_LABEL:
	
	BDBG_LEAVE(BSCD_Channel_P_Activating);
	return( errCode );
}




void BSCD_P_HexDump(
      char          *inp_cTitle, 
      unsigned char *inp_ucBuf, 
      unsigned int  in_unLen
)
{
	size_t   i;

	BDBG_MSG(("\n%s (%u bytes):", inp_cTitle, in_unLen));

	for(i=0; i<in_unLen; i++) {

		if(!(i%20)) {
			BDBG_MSG(("\n"));
		}

		BDBG_MSG(("%02X  ",*(inp_ucBuf+i)));
	}

	BDBG_MSG(("\n"));

	BSTD_UNUSED(inp_cTitle);
	BSTD_UNUSED(inp_ucBuf);
	
}





BERR_Code BSCD_Channel_P_T0ReadData(
		BSCD_ChannelHandle       in_channelHandle,
		uint8_t                  *outp_ucRcvData,
		unsigned long                 *outp_ulNumRcvBytes,
		unsigned long                 in_ulMaxReadBytes
)
{
	BERR_Code 		errCode = BERR_SUCCESS;
	uint32_t                	ulLen = 0;
	uint32_t		unIntrEn2,	ulStatus2;
#ifndef BSCD_DSS_ICAM	
	BSCD_Timer 		timer = {BSCD_TimerType_eWaitTimer, {BSCD_GPTimerMode_eIMMEDIATE}, true, true};	
	BSCD_TimerValue    timeValue= {BSCD_DEFAULT_WORK_WAITING_TIME, BSCD_TimerUnit_eETU};
#endif	

	BDBG_ENTER(BSCD_Channel_P_T0ReadData);

	*outp_ulNumRcvBytes = 0;
	
	BKNI_EnterCriticalSection();
	in_channelHandle->ulStatus2 = ulStatus2 = BREG_Read32( 
			in_channelHandle->moduleHandle->regHandle, 
			(in_channelHandle->ulRegStartAddr + BSCD_P_STATUS_2));	
	BKNI_LeaveCriticalSection();	

	BDBG_MSG(("in_ulMaxReadBytes = %d\n", in_ulMaxReadBytes));

	while (ulLen < in_ulMaxReadBytes ) {

#ifndef BSCD_DSS_ICAM
		/* 
			This is a backup time out for non EMV standard. 
			Just in case, we do not read all the byte in one shot but 
			WWT was disable in BSCD_Channel_Receive
		*/
		if ((in_channelHandle->currentChannelSettings.scStandard != BSCD_Standard_eEMV1996) && 
			(in_channelHandle->currentChannelSettings.scStandard != BSCD_Standard_eEMV2000) ) {	
			unIntrEn2 = BREG_Read32( 
					in_channelHandle->moduleHandle->regHandle, 
					(in_channelHandle->ulRegStartAddr + BSCD_P_INTR_EN_2));	
			if(!(unIntrEn2 &BCHP_SCA_SC_INTR_EN_2_wait_ien_MASK)){
			timeValue.ulValue = in_channelHandle->currentChannelSettings.workWaitTime.ulValue;
			BSCD_P_CHECK_ERR_CODE_FUNC2(errCode, BSCD_STATUS_READ_FAILED,
					BSCD_Channel_ConfigTimer(in_channelHandle, &timer, &timeValue));	
		}
		}
#endif

	        BDBG_MSG(("\nSmartCardReadCmd: After SmartCardSetGPTimer\n"));

		BKNI_EnterCriticalSection();
		ulStatus2 = in_channelHandle->ulStatus2;
		BKNI_LeaveCriticalSection();
			
	 	if ( (( ulStatus2 & BCHP_SCA_SC_STATUS_2_rempty_MASK) == BCHP_SCA_SC_STATUS_2_rempty_MASK)  &&
	  		 	((errCode = BSCD_Channel_P_WaitForRcv(in_channelHandle)) != 
	  		 										BERR_SUCCESS) ) {

			errCode = BERR_TRACE(errCode);
			BDBG_MSG (("After  BSCD_Channel_P_WaitForRcv in BSCD_Channel_P_T0ReadData errCode = 0x%x\n", 
						errCode));
#ifndef BSCD_DSS_ICAM
			/* Disable timer */
			if ((in_channelHandle->currentChannelSettings.scStandard != BSCD_Standard_eEMV1996) && 
				(in_channelHandle->currentChannelSettings.scStandard != BSCD_Standard_eEMV2000) ) {
				timer.bIsTimerInterruptEnable = false;
				timer.bIsTimerEnable = false;
				BSCD_Channel_EnableDisableTimer_isr(in_channelHandle, &timer);	
			}
#endif			

		         if (errCode == BSCD_STATUS_PARITY_EDC_ERR ) {
				;   /* No op in software , hardware will retry */
			}
			else if (errCode == BSCD_STATUS_TIME_OUT)
				break;
			else {						
				return BSCD_STATUS_READ_FAILED;
			}
		}

		/* BDBG_MSG(("\nSmartCardReadCmd: After ScardDeviceWaitForRcv\n")); */

#if 0 /*ndef BSCD_DSS_ICAM*/
		/* Disable timer */
		if ((in_channelHandle->currentChannelSettings.scStandard != BSCD_Standard_eEMV1996) && 
			(in_channelHandle->currentChannelSettings.scStandard != BSCD_Standard_eEMV2000) ) {
			timer.bIsTimerInterruptEnable = false;
			timer.bIsTimerEnable = false;
			BSCD_P_CHECK_ERR_CODE_FUNC2(errCode, BSCD_STATUS_READ_FAILED,
				BSCD_Channel_EnableDisableTimer_isr(in_channelHandle, &timer));	   
		}
#endif		

		while (ulLen < in_ulMaxReadBytes ) {
			BDBG_MSG (("In  ulLen < in_ulMaxReadBytes\n")); 
			if ((errCode = BSCD_Channel_P_ByteRead(in_channelHandle, &outp_ucRcvData[ulLen])) == BERR_SUCCESS) {

				in_channelHandle->channelStatus.ulStatus1 &= ~BSCD_RX_PARITY; 
				if ((outp_ucRcvData[ulLen] == 0x60) && 
				     (in_channelHandle->currentChannelSettings.bNullFilter == true) ) {
					BDBG_MSG (("Ignore 0x60 == %2X ", outp_ucRcvData[ulLen])); 
					continue;
					
				}
				else {
					BDBG_MSG (("%2X ", outp_ucRcvData[ulLen])); 
					ulLen++;
				}
				
			}

			else if (errCode == BSCD_STATUS_PARITY_EDC_ERR) {
				BDBG_MSG (("errCode == BSCD_STATUS_PARITY_EDC_ERR\n")); 
				continue;
			}
			else {
				break;
			}
		}
	}

#ifndef BSCD_DSS_ICAM	
BSCD_P_DONE_LABEL:
#endif
	
	/* 09/28/2006 QX: return data even it's a partial read 
	if (errCode != BERR_SUCCESS) 
		ulLen = 0;
	*/	
	*outp_ulNumRcvBytes = ulLen;		
	
	BDBG_LEAVE(BSCD_Channel_P_T0ReadData);	
	/* BDBG_MSG (("Leave BSCD_Channel_P_T0ReadData errCode = 0x%x\n", errCode));  */
	return errCode;
}


BERR_Code BSCD_Channel_P_ByteRead(
		BSCD_ChannelHandle	in_channelHandle,
		unsigned char *outp_ucData		
)
{
	BERR_Code errCode = BERR_SUCCESS;
	uint32_t ulStatus2;

	BDBG_ENTER(BSCD_Channel_P_ByteRead);

	BKNI_EnterCriticalSection();
	in_channelHandle->ulStatus2 =  ulStatus2 = BREG_Read32( 
			in_channelHandle->moduleHandle->regHandle, 
			(in_channelHandle->ulRegStartAddr + BSCD_P_STATUS_2));	
	BKNI_LeaveCriticalSection();

	if (( ulStatus2 & BCHP_SCA_SC_STATUS_2_rempty_MASK) != BCHP_SCA_SC_STATUS_2_rempty_MASK) {

		*outp_ucData = (unsigned char) BREG_Read32( 
			in_channelHandle->moduleHandle->regHandle, 
			(in_channelHandle->ulRegStartAddr + BSCD_P_RECEIVE));	

		BKNI_EnterCriticalSection();
		in_channelHandle->ulStatus2 =  ulStatus2 = BREG_Read32( 
			in_channelHandle->moduleHandle->regHandle, 
			(in_channelHandle->ulRegStartAddr + BSCD_P_STATUS_2));	
		BKNI_LeaveCriticalSection();

		
	      if ( (( ulStatus2 & BCHP_SCA_SC_STATUS_2_rpar_err_MASK) == BCHP_SCA_SC_STATUS_2_rpar_err_MASK) &&
		      (in_channelHandle->currentChannelSettings.scStandard  != BSCD_Standard_eIrdeto)) {
			BDBG_MSG(("Receive a parity error byte\n"));
			BKNI_EnterCriticalSection();
			in_channelHandle->channelStatus.ulStatus1 |= BSCD_RX_PARITY; 
			BKNI_LeaveCriticalSection();			 
			return BSCD_STATUS_PARITY_EDC_ERR;
	      }

	} 
	else        
		return (BSCD_STATUS_FAILED);   

	BDBG_LEAVE(BSCD_Channel_P_ByteRead);	
	return errCode;
}


BERR_Code BSCD_Channel_P_T1ReadData(
		BSCD_ChannelHandle       in_channelHandle,
		uint8_t                  *outp_ucRcvData,
		unsigned long               *outp_ulNumRcvBytes,
		unsigned long        in_ulMaxReadBytes 
)
{
	BERR_Code errCode = BERR_SUCCESS;
	uint32_t ulVal, ulLen1, ulLen2;
	uint32_t          ulLen = 0, i;	
	BSCD_Timer 		timer = {BSCD_TimerType_eWaitTimer, {BSCD_GPTimerMode_eIMMEDIATE}, false, false};		

	BDBG_ENTER(BSCD_Channel_P_T1ReadData);

	BSTD_UNUSED(in_ulMaxReadBytes);
	*outp_ulNumRcvBytes	 = 0;
	
	ulVal =  BREG_Read32( 
			in_channelHandle->moduleHandle->regHandle, 
			(in_channelHandle->ulRegStartAddr + BSCD_P_PROTO_CMD));

	if (in_channelHandle->currentChannelSettings.scStandard == BSCD_Standard_eES) {
		/* application computes its own LRC or CRC and appends it as the last byte */
		ulVal |= BCHP_SCA_SC_PROTO_CMD_tbuf_rst_MASK;
	}
	else {

        if(in_channelHandle->currentChannelSettings.edcSetting.bIsEnabled==false)
            ulVal |= BCHP_SCA_SC_PROTO_CMD_tbuf_rst_MASK;
        else{
			ulVal |=  BCHP_SCA_SC_PROTO_CMD_edc_en_MASK;
	
		if (in_channelHandle->currentChannelSettings.edcSetting.edcEncode == BSCD_EDCEncode_eLRC ) {
			ulVal &=  ~BCHP_SCA_SC_PROTO_CMD_crc_lrc_MASK;
		}
		else if (in_channelHandle->currentChannelSettings.edcSetting.edcEncode == BSCD_EDCEncode_eCRC) {
			ulVal |=  BCHP_SCA_SC_PROTO_CMD_crc_lrc_MASK;		
		}		
	
		}
	}

	BREG_Write32( 
			in_channelHandle->moduleHandle->regHandle, 
			(in_channelHandle->ulRegStartAddr + BSCD_P_PROTO_CMD),
			ulVal);

	if ((errCode = BSCD_Channel_P_WaitForRReady(in_channelHandle)) != 
	  		 												BERR_SUCCESS)  {
		/* If parity error, continue reading all the bytes */
		errCode = BERR_TRACE(errCode);
		return BSCD_STATUS_NO_SC_RESPONSE;
	}

	/* Disable block wait timer */
	timer.eTimerType = BSCD_TimerType_eWaitTimer;							
	timer.timerMode.eWaitTimerMode = BSCD_WaitTimerMode_eBlockWaitTime;	
	BSCD_P_CHECK_ERR_CODE_FUNC2(errCode, BSCD_STATUS_READ_FAILED,
		BSCD_Channel_EnableDisableTimer_isr(in_channelHandle, &timer));	

	/* Disable cwt since we already receive all the bytes */
	ulVal =  BREG_Read32( 
			in_channelHandle->moduleHandle->regHandle, 
			(in_channelHandle->ulRegStartAddr + BSCD_P_TIMER_CMD));
	
	ulVal &= ~BCHP_SCA_SC_TIMER_CMD_cwt_en_MASK;

	BREG_Write32( 
			in_channelHandle->moduleHandle->regHandle, 
			(in_channelHandle->ulRegStartAddr + BSCD_P_TIMER_CMD),
			ulVal);

	/* Clear cwt_intr so that it won't show up next time */
	BKNI_EnterCriticalSection();
#ifdef BSCD_EMV2000_CWT_PLUS_4_EVENT_INTR
	in_channelHandle->ulIntrStatus1 &= ~BCHP_SCA_SC_INTR_STAT_1_event1_intr_MASK; 
#else
	in_channelHandle->ulIntrStatus2 &= ~BCHP_SCA_SC_INTR_STAT_2_cwt_intr_MASK;
#endif
	BKNI_LeaveCriticalSection();

	ulLen1 =  BREG_Read32( 
			in_channelHandle->moduleHandle->regHandle, 
			(in_channelHandle->ulRegStartAddr + BSCD_P_RLEN_1));
	ulLen2 =  BREG_Read32( 
			in_channelHandle->moduleHandle->regHandle, 
			(in_channelHandle->ulRegStartAddr + BSCD_P_RLEN_2));	

	/* RLEN_9_BIT_MASK = 0x01ff */
	ulLen = ((((unsigned short) ulLen2) << 8) | ulLen1) & BSCD_RLEN_9_BIT_MASK;
	BDBG_MSG(("SmartCardBlockRead: rlen = %d\n", ulLen));

	if (ulLen) {

		for (i = 0; i < ulLen; i++) {

			outp_ucRcvData[i] =  (uint8_t) BREG_Read32( 
						in_channelHandle->moduleHandle->regHandle, 
						(in_channelHandle->ulRegStartAddr + BSCD_P_RECEIVE));
			ulVal =  BREG_Read32( 
						in_channelHandle->moduleHandle->regHandle, 
						(in_channelHandle->ulRegStartAddr + BSCD_P_STATUS_2));

			if ((ulVal & BCHP_SCA_SC_STATUS_2_rpar_err_MASK) == BCHP_SCA_SC_STATUS_2_rpar_err_MASK) {
				BDBG_MSG(("SmartCardBlockRead: parity error\n"));
				errCode = BSCD_STATUS_PARITY_EDC_ERR;
			}
			else if ((ulVal & BCHP_SCA_SC_STATUS_2_edc_err_MASK) == BCHP_SCA_SC_STATUS_2_edc_err_MASK) {
				BDBG_MSG(("SmartCardBlockRead: EDC error\n"));
				errCode = BSCD_STATUS_PARITY_EDC_ERR;
			}

			if ((i % 16) == 0) {
				BDBG_MSG(("\n"));
				/* Need to this to fix the warning if BDBG_MSG is defined to nothing */
				;
			}

			BDBG_MSG(("%02x ", outp_ucRcvData[i]));

		}
	}

BSCD_P_DONE_LABEL:

	if (errCode != BERR_SUCCESS) 
		ulLen = 0;
	
	*outp_ulNumRcvBytes = ulLen;	
	
	BDBG_LEAVE(BSCD_Channel_P_T1ReadData);	
	return errCode;
}


BERR_Code BSCD_Channel_P_ReceiveAndDecode(
		BSCD_ChannelHandle	in_channelHandle
)
{

	BERR_Code		errCode = BERR_SUCCESS;

	BDBG_ENTER(BSCD_Channel_P_ReceiveAndDecode);
	
	if (in_channelHandle->currentChannelSettings.resetCardAction == BSCD_ResetCardAction_eNoAction) {
		BDBG_MSG(("In BSCD_Channel_P_ReceiveAndDecode BSCD_ResetCardAction_eNoAction\n"));
		return BERR_SUCCESS;
	}
	else if (in_channelHandle->currentChannelSettings.resetCardAction == BSCD_ResetCardAction_eReceiveAndDecode) {
		BDBG_MSG(("In BSCD_Channel_P_ReceiveAndDecode BSCD_ResetCardAction_eReceiveAndDecode standard = %d\n",
			in_channelHandle->currentChannelSettings.scStandard));
		switch (in_channelHandle->currentChannelSettings.scStandard) {
			case BSCD_Standard_eEMV1996:		
			case BSCD_Standard_eEMV2000:				
				if ( (errCode = BSCD_Channel_P_EMVATRReceiveAndDecode(in_channelHandle)) != BERR_SUCCESS) {
					errCode = BERR_TRACE(errCode);
					goto BSCD_P_DONE_LABEL;
				}				
				break;
			default:
				BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, true);
		}
		
	}


BSCD_P_DONE_LABEL:
	
	BDBG_LEAVE(BSCD_Channel_P_ReceiveAndDecode);
	return( errCode );
}


void BSCD_Channel_P_IntHandler_isr(
	void *inp_param1,		/* Device channel handle */
	int in_param2				/* reserved */
)
{
	uint32_t   unStaReg1 = 0, unStaReg2 = 0, unProtoCmdReg = 0;
	uint32_t    unIntrEn1 = 0, unIntrEn2 = 0, unIntrStaReg1 = 0, unIntrStaReg2 = 0;
	uint32_t   unIFCmdVal, unPrevStaReg1, ulVal;
	BSCD_IntrType event;
	BERR_Code errCode = BERR_SUCCESS;
	BSCD_Timer 		timer = {BSCD_TimerType_eGPTimer, {BSCD_GPTimerMode_eIMMEDIATE}, true, true};	
	int 		i;	
	BSCD_ChannelHandle 	channelHandle = (BSCD_ChannelHandle) inp_param1;	

#ifdef BSCD_EMV2000_CWT_PLUS_4
	BSCD_Timer 		cwtTimer = {BSCD_TimerType_eWaitTimer, {BSCD_WaitTimerMode_eWorkWaitTime}, true, true};	
	BSCD_TimerValue    cwtTimeValue= {16, BSCD_TimerUnit_eETU};	
#endif	

	BDBG_ENTER(BSCD_IntHandler_isr);

	BSTD_UNUSED(in_param2);
	
	BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,  (channelHandle == NULL) );	
	
	BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, 
		(channelHandle->ulMagicNumber != BSCD_P_CHANNEL_HANDLE_MAGIC_NUMBER ) );

	BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, 
		(channelHandle->bIsOpen ==  false) );	


	/* Read Smartcard Interrupt Status & Mask Register */
	unProtoCmdReg = BREG_Read32( 
		channelHandle->moduleHandle->regHandle, 
		(channelHandle->ulRegStartAddr + BSCD_P_PROTO_CMD));	
	
	unStaReg1 = BREG_Read32( 
		channelHandle->moduleHandle->regHandle, 
		(channelHandle->ulRegStartAddr + BSCD_P_STATUS_1));	

	unStaReg2 = BREG_Read32( 
		channelHandle->moduleHandle->regHandle, 
		(channelHandle->ulRegStartAddr + BSCD_P_STATUS_2));		

	unIntrEn1 = BREG_Read32( 
		channelHandle->moduleHandle->regHandle, 
		(channelHandle->ulRegStartAddr + BSCD_P_INTR_EN_1));	

	unIntrStaReg1 = BREG_Read32( 
		channelHandle->moduleHandle->regHandle, 
		(channelHandle->ulRegStartAddr + BSCD_P_INTR_STAT_1));		

	unIntrEn2 = BREG_Read32( 
		channelHandle->moduleHandle->regHandle, 
		(channelHandle->ulRegStartAddr + BSCD_P_INTR_EN_2));	

	unIntrStaReg2 = BREG_Read32( 
		channelHandle->moduleHandle->regHandle, 
		(channelHandle->ulRegStartAddr + BSCD_P_INTR_STAT_2));	

#ifdef BSCD_INTERRUPT_DEBUG
#if 1
	BDBG_MSG(("unIntrEn1 = %2x, unIntrStaReg1 = %2x, ucSlot = %d\n", 
		unIntrEn1, unIntrStaReg1, channelHandle->ucChannelNumber));
	BDBG_MSG(("unIntrEn2 = %2x, unIntrStaReg2 = %2x\n", unIntrEn2, unIntrStaReg2));
	BDBG_MSG(("unStaReg1 = %2x, unStaReg2= %2x\n", unStaReg1, unStaReg2));
#endif	
#endif

	channelHandle->ulStatus1  = unStaReg1;

	channelHandle->ulStatus2  = unStaReg2;

	/* Process interrupt */

	if ( (unIntrEn1 & BCHP_SCA_SC_INTR_EN_1_pres_ien_MASK) && (unIntrStaReg1 & BCHP_SCA_SC_INTR_STAT_1_pres_intr_MASK) ) {

		/* Disable pres intr to debounce the card pres */
		BSCD_P_CHECK_ERR_CODE_FUNC(errCode, 	
 				BSCD_Channel_DisableIntrCallback_isr (
				channelHandle, BSCD_IntType_eCardInsertInt));		

		/* Store status_1 to determine if hardware failure */
		unPrevStaReg1 = BREG_Read32( 
			channelHandle->moduleHandle->regHandle, 
			(channelHandle->ulRegStartAddr + BSCD_P_STATUS_1));	
#ifdef BSCD_INTERRUPT_DEBUG			
		BDBG_MSG(("unPrevStaReg1 = %2x\n", unPrevStaReg1));
#endif

if(channelHandle->currentChannelSettings.bAutoDeactiveReq == false){                
		/* In case this is an emergency deactivation, we have to set 
		   IF_CMD_1[VCC]=1 to detect card pres again. */
		unIFCmdVal = BREG_Read32( 
			channelHandle->moduleHandle->regHandle, 
			(channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1));	
#ifdef BSCD_INTERRUPT_DEBUG	
		BDBG_MSG(("1 BCM_SC_IF_CMD_1 = %2x\n", unIFCmdVal));
#endif
                if(channelHandle->currentChannelSettings.bConnectDirectly == true){	
					if(channelHandle->currentChannelSettings.bDirectVccInverted == true)
		unIFCmdVal |= BCHP_SCA_SC_IF_CMD_1_vcc_MASK;
					else
                  unIFCmdVal &= ~BCHP_SCA_SC_IF_CMD_1_vcc_MASK;
		}else{
		unIFCmdVal |= BCHP_SCA_SC_IF_CMD_1_vcc_MASK;
                }
                unIFCmdVal &= ~BCHP_SCA_SC_IF_CMD_1_auto_vcc_MASK;
		BREG_Write32( 
			channelHandle->moduleHandle->regHandle, 
			(channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1), 
			 unIFCmdVal);		

		/* TDA8004 suggests we to wait until debounce stabilizes.  NDS suggests to
			sleep for 10 milli seconds.  This may hold the system for 10ms but it is
			okay since the system should not continue without the card. */
		/* All customers should use TDA8024 now */
		/* BKNI_Delay(10000);*/
}

		unStaReg1 = BREG_Read32( 
			channelHandle->moduleHandle->regHandle, 
			(channelHandle->ulRegStartAddr + BSCD_P_STATUS_1));

#ifdef BSCD_INTERRUPT_DEBUG			
		BDBG_MSG(("PRES_INTR , IntrEn1 = %x, IntrStaReg1 = %x, unStaReg1 = %x, ucSlot = %d\n", 
					unIntrEn1, unIntrStaReg1, unStaReg1, channelHandle->ucChannelNumber));
#endif

		/* According TDA 8004 Application note, this is how to determine card presence, card removal and hardware failure. */
		if ( (unStaReg1 & BCHP_SCA_SC_STATUS_1_card_pres_MASK) &&  (!(unPrevStaReg1 & BCHP_SCA_SC_STATUS_1_card_pres_MASK)) ) {
#ifdef BSCD_INTERRUPT_DEBUG				
			BDBG_MSG(("hardware failure\n"));
#endif
			channelHandle->channelStatus.bCardPresent = true;
			channelHandle->channelStatus.ulStatus1 |= BSCD_HARDWARE_FAILURE | BSCD_RESET_CHANNEL_REQUIRED;
		}
		else if ( (unStaReg1 & BCHP_SCA_SC_STATUS_1_card_pres_MASK) &&  (unPrevStaReg1 & BCHP_SCA_SC_STATUS_1_card_pres_MASK) ) {
#ifdef BSCD_INTERRUPT_DEBUG				
			BDBG_MSG(("Insert\n"));
#endif
			channelHandle->channelStatus.bCardPresent = true;			
			channelHandle-> bIsCardRemoved = false ;
		}
		else if (!(unStaReg1 & BCHP_SCA_SC_STATUS_1_card_pres_MASK) && !(unPrevStaReg1 & BCHP_SCA_SC_STATUS_1_card_pres_MASK)) {
#ifdef BSCD_INTERRUPT_DEBUG				
			BDBG_MSG(("Remove\n"));
#endif

			/* Disable all interrupt but pres_intr to support auto-deactivation. 
			     Auto Deactvation will cause a parity_intr and retry_intr to loop forever
			*/
			BREG_Write32( 
				channelHandle->moduleHandle->regHandle, 
				(channelHandle->ulRegStartAddr + BSCD_P_INTR_EN_1), 
				0);		

			BREG_Write32( 
				channelHandle->moduleHandle->regHandle, 
				(channelHandle->ulRegStartAddr + BSCD_P_INTR_EN_2), 
				0);	
	
			/* 09/20/05,Allen.C, remember Card was removed */
			channelHandle->bIsCardRemoved = true;
			
			channelHandle->channelStatus.bCardPresent = false;	
			channelHandle->channelStatus.ulStatus1 |= BSCD_RESET_CHANNEL_REQUIRED;
		}

		channelHandle->ulIntrStatus1 = unIntrStaReg1;
		channelHandle->ulStatus1  = unStaReg1;

		if (channelHandle->channelStatus.bCardPresent == true) {

			event = BSCD_IntType_eCardInsertInt;
			for (i=0; i< BSCD_MAX_NUM_CALLBACK_FUNC; i++)  {
				if (channelHandle->callBack.cardInsertIsrCBFunc[i] != NULL) 
					(*(channelHandle->callBack.cardInsertIsrCBFunc[i])) (channelHandle, &event);
			}
	
#ifdef BSCD_INTERRUPT_DEBUG
			BDBG_MSG(("SC %d Insertion Interrupt\n", channelHandle->ucChannelNumber));

#endif

		}

		else {
			event = BSCD_IntType_eCardRemoveInt;
			for (i=0; i< BSCD_MAX_NUM_CALLBACK_FUNC; i++)  {
				if (channelHandle->callBack.cardRemoveIsrCBFunc[i] != NULL)
					(*(channelHandle->callBack.cardRemoveIsrCBFunc[i])) (channelHandle, &event);
			}

#ifdef BSCD_INTERRUPT_DEBUG
			BDBG_MSG(("SC %d Removal Interrupt\n", channelHandle->ucChannelNumber));

#endif

		}

		/* re-enable pres intr */
		BSCD_P_CHECK_ERR_CODE_FUNC(errCode, 	
 				BSCD_Channel_EnableIntrCallback_isr (
				channelHandle, BSCD_IntType_eCardInsertInt,
		               BSCD_Channel_P_CardInsertCB_isr));

	}

	if ( (unIntrEn1 & BCHP_SCA_SC_INTR_EN_1_tpar_ien_MASK) && (unIntrStaReg1 & BCHP_SCA_SC_INTR_STAT_1_tpar_intr_MASK) ) {
#ifdef BSCD_INTERRUPT_DEBUG		
		BDBG_MSG(("TPAR_INTR , IntrEn1 = %x, IntrStaReg1 = %x, ucSlot = %d\n", 
				unIntrEn1, unIntrStaReg1, channelHandle->ucChannelNumber));
#endif

		channelHandle->channelStatus.ulStatus1 |= BSCD_TX_PARITY;

		channelHandle->ulIntrStatus1 |= unIntrStaReg1;
		channelHandle->ulIntrStatus2 |= unIntrStaReg2;

		event = BSCD_IntType_eTParityInt; 
		for (i=0; i< BSCD_MAX_NUM_CALLBACK_FUNC; i++)  {
			if (channelHandle->callBack.tParityIsrCBFunc[i] != NULL)
				(*(channelHandle->callBack.tParityIsrCBFunc[i])) (channelHandle, &event);
		}
	}

	if ( (unIntrEn1 & BCHP_SCA_SC_INTR_EN_1_timer_ien_MASK) && (unIntrStaReg1 & BCHP_SCA_SC_INTR_STAT_1_timer_intr_MASK) ) {
#ifdef BSCD_INTERRUPT_DEBUG	
		BDBG_MSG(("TIMER_INTR , IntrEn1 = %x, IntrStaReg1 = %x, ucSlot = %d\n", 
						unIntrEn1, unIntrStaReg1, channelHandle->ucChannelNumber)); 
#endif
	
		timer.bIsTimerInterruptEnable = false;
		timer.bIsTimerEnable = false;
		BSCD_P_CHECK_ERR_CODE_FUNC(errCode, 
			BSCD_Channel_EnableDisableTimer_isr(channelHandle, &timer));	

		channelHandle->ulIntrStatus1 |= unIntrStaReg1;
		channelHandle->ulIntrStatus2 |= unIntrStaReg2;

		/* We need to signal different events to take care of different scenarioes */
		event = BSCD_IntType_eTimerInt;
		for (i=0; i< BSCD_MAX_NUM_CALLBACK_FUNC; i++)  {		
			if (channelHandle->callBack.timerIsrCBFunc[i] != NULL)
				(*(channelHandle->callBack.timerIsrCBFunc[i])) (channelHandle, &event);
		}
	}

	if ( (unIntrEn1 & BCHP_SCA_SC_INTR_EN_1_bgt_ien7_MASK) && (unIntrStaReg1 & BCHP_SCA_SC_INTR_STAT_1_bgt_intr_MASK) ) {
#ifdef BSCD_INTERRUPT_DEBUG
		BDBG_MSG(("BGT_INTR , IntrEn1 = %x, IntrStaReg1 = %x, ucSlot = %d\n", 
			unIntrEn1, unIntrStaReg1, channelHandle->ucChannelNumber));
#endif

		channelHandle->ulIntrStatus1 |= unIntrStaReg1;
		channelHandle->ulIntrStatus2 |= unIntrStaReg2;

		/* We need to signal different events to take care of different scenarioes */
		event = BSCD_IntType_eBGTInt;		
		for (i=0; i< BSCD_MAX_NUM_CALLBACK_FUNC; i++)  {		
			if (channelHandle->callBack.bgtIsrCBFunc[i] != NULL)
				(*(channelHandle->callBack.bgtIsrCBFunc[i])) (channelHandle, &event);
		}
	}

	if ( (unIntrEn1 & BCHP_SCA_SC_INTR_EN_1_tdone_ien_MASK) && (unIntrStaReg1 & BCHP_SCA_SC_INTR_STAT_1_tdone_intr_MASK) ) {
#ifdef BSCD_INTERRUPT_DEBUG
		BDBG_MSG(("TDONE_INTR ,IntrEn1 = %x, IntrStaReg1 = %x, ucSlot = %d\n", 
					unIntrEn1, unIntrStaReg1, channelHandle->ucChannelNumber));
#endif

		channelHandle->ulIntrStatus1 |= unIntrStaReg1;
		channelHandle->ulIntrStatus2 |= unIntrStaReg2;

		/* We need to signal different events to take care of different scenarioes */
		event = BSCD_IntType_eTDoneInt;		
		for (i=0; i< BSCD_MAX_NUM_CALLBACK_FUNC; i++)  {		
			if (channelHandle->callBack.tDoneIsrCBFunc[i] != NULL)
				(*(channelHandle->callBack.tDoneIsrCBFunc[i])) (channelHandle, &event);
		}
	}

	if ( (unIntrEn1 & BCHP_SCA_SC_INTR_EN_1_retry_ien_MASK) && (unIntrStaReg1 & BCHP_SCA_SC_INTR_STAT_1_retry_intr_MASK) ) {
#ifdef BSCD_INTERRUPT_DEBUG
		BDBG_MSG(("RETRY_INTR IntrEn1 = %x, IntrStaReg1 = %x, ucSlot = %d\n",
						unIntrEn1, unIntrStaReg1, channelHandle->ucChannelNumber));
#endif

		/* If parity tx or rx retrial failes, we should reset uart and NOT to continue tx any more data */
		ulVal =  BREG_Read32( 
			channelHandle->moduleHandle->regHandle, 
			(channelHandle->ulRegStartAddr + BSCD_P_UART_CMD_1)) ;
		ulVal |= (BCHP_SCA_SC_UART_CMD_1_uart_rst_MASK);
		BREG_Write32( 
			channelHandle->moduleHandle->regHandle, 
			(channelHandle->ulRegStartAddr + BSCD_P_UART_CMD_1), 
			 ulVal);  

		channelHandle->ulIntrStatus1 |= unIntrStaReg1;
		channelHandle->ulIntrStatus2 |= unIntrStaReg2;

		/* We need to signal different events to take care of different scenarioes */
		event = BSCD_IntType_eRetryInt;
		for (i=0; i< BSCD_MAX_NUM_CALLBACK_FUNC; i++)  {
			if (channelHandle->callBack.retryIsrCBFunc[i] != NULL)
				(*(channelHandle->callBack.retryIsrCBFunc[i])) (channelHandle, &event);
		}
	}

	if ( (unIntrEn1 & BCHP_SCA_SC_INTR_EN_1_tempty_ien_MASK) && (unIntrStaReg1 & BCHP_SCA_SC_INTR_STAT_1_tempty_intr_MASK) ) {
#ifdef BSCD_INTERRUPT_DEBUG
		BDBG_MSG(("TEMPTY_INTR ,IntrEn1 = %x, IntrStaReg1 = %x, ucSlot = %d\n", 
						unIntrEn1, unIntrStaReg1, channelHandle->ucChannelNumber)); 
#endif

		channelHandle->ulIntrStatus1 |= unIntrStaReg1;
		channelHandle->ulIntrStatus2 |= unIntrStaReg2;

		/* Currently we do not need this.  No signal needed */
		event = BSCD_IntType_eTEmptyInt;
		for (i=0; i< BSCD_MAX_NUM_CALLBACK_FUNC; i++)  {		
			if (channelHandle->callBack.tEmptyIsrCBFunc[i] != NULL)
				(*(channelHandle->callBack.tEmptyIsrCBFunc[i])) (channelHandle, &event);
		}
	}

	if ( (unIntrEn2 & BCHP_SCA_SC_INTR_EN_2_rpar_ien_MASK) && (unIntrStaReg2 & BCHP_SCA_SC_INTR_STAT_2_rpar_intr_MASK) ) {
#ifdef BSCD_INTERRUPT_DEBUG	
		BDBG_MSG(("RPAR_INTR , IntrEn2 = %x, IntrStaReg2 = %x, ucSlot = %d\n",
						unIntrEn2, unIntrStaReg2, channelHandle->ucChannelNumber));
#endif

		channelHandle->channelStatus.ulStatus2 |= BSCD_RX_PARITY;
		channelHandle->ulIntrStatus1 |= unIntrStaReg1;
		channelHandle->ulIntrStatus2 |= unIntrStaReg2;

		event = BSCD_IntType_eRParityInt;
		for (i=0; i< BSCD_MAX_NUM_CALLBACK_FUNC; i++)  {	
			if (channelHandle->callBack.rParityIsrCBFunc[i] != NULL)
				(*(channelHandle->callBack.rParityIsrCBFunc[i])) (channelHandle, &event);
		}

	}

	if ( (unIntrEn2 & BCHP_SCA_SC_INTR_EN_2_atrs_ien_MASK) && (unIntrStaReg2 & BCHP_SCA_SC_INTR_STAT_2_atrs_intr_MASK) ) {
		BDBG_MSG(("ATRS_INTR  unIntrEn2 = %x, IntrStaReg2 = %x, ucSlot = %d\n", 
			unIntrEn2, unIntrStaReg2, channelHandle->ucChannelNumber));

		/* 
			We need this interrupt to measure the period of time we received leading edge of
			the start bit of the first ATR byte.  As soon as we receive this interrupt, we should
			stop the timer so that we could get more accurate timing
		*/
		
		timer.bIsTimerInterruptEnable = false;
		timer.bIsTimerEnable = false;
		BSCD_P_CHECK_ERR_CODE_FUNC(errCode, 
			BSCD_Channel_EnableDisableTimer_isr(channelHandle, &timer));			
			
		channelHandle->ulIntrStatus1 |= unIntrStaReg1;
		channelHandle->ulIntrStatus2 |= unIntrStaReg2;

		event = BSCD_IntType_eATRInt;
		for (i=0; i< BSCD_MAX_NUM_CALLBACK_FUNC; i++)  {	
			if (channelHandle->callBack.atrIsrCBFunc[i] != NULL)
				(*(channelHandle->callBack.atrIsrCBFunc[i])) (channelHandle, &event);
		}
	}

	if ( (unIntrEn2 & BCHP_SCA_SC_INTR_EN_2_cwt_ien_MASK) && (unIntrStaReg2 & BCHP_SCA_SC_INTR_STAT_2_cwt_intr_MASK) ) {
#ifdef BSCD_INTERRUPT_DEBUG
		BDBG_MSG(("CWT_INTR IntrEn2 = %x, IntrStaReg2 = %x, ucSlot = %d\n",
						unIntrEn2, unIntrStaReg2, channelHandle->ucChannelNumber));
#endif

		/* If cwt_intr comes in after rready_intr, it is considered normal */
		channelHandle->ulIntrStatus1 |= unIntrStaReg1;
		channelHandle->ulIntrStatus2 |= unIntrStaReg2;

		event = BSCD_IntType_eCWTInt;
		for (i=0; i< BSCD_MAX_NUM_CALLBACK_FUNC; i++)  {	
			if (channelHandle->callBack.cwtIsrCBFunc[i] != NULL)
				(*(channelHandle->callBack.cwtIsrCBFunc[i])) (channelHandle, &event);
		}
	}

	if ( (unIntrEn2 & BCHP_SCA_SC_INTR_EN_2_rlen_ien_MASK) && (unIntrStaReg2 & BCHP_SCA_SC_INTR_STAT_2_rlen_intr_MASK) ) {
#ifdef BSCD_INTERRUPT_DEBUG	
		BDBG_MSG(("RLEN_INTR , IntrEn2 = %x, IntrStaReg2 = %x, ucSlot = %d\n", 
						unIntrEn2, unIntrStaReg2, channelHandle->ucChannelNumber));
#endif

		channelHandle->ulIntrStatus1 |= unIntrStaReg1;
		channelHandle->ulIntrStatus2 |= unIntrStaReg2;

		event = BSCD_IntType_eRLenInt;
		for (i=0; i< BSCD_MAX_NUM_CALLBACK_FUNC; i++)  {	
			if (channelHandle->callBack.rLenIsrCBFunc[i] != NULL) 
				(*(channelHandle->callBack.rLenIsrCBFunc[i])) (channelHandle, &event);
		}
	}

	if ( (unIntrEn2 & BCHP_SCA_SC_INTR_EN_2_wait_ien_MASK) && (unIntrStaReg2 & BCHP_SCA_SC_INTR_STAT_2_wait_intr_MASK) ) {
#ifdef BSCD_INTERRUPT_DEBUG
		BDBG_MSG(("WAIT_INTR , IntrEn2 = %x, IntrStaReg2 = %x, ucSlot = %d\n", 
					unIntrEn2, unIntrStaReg2, channelHandle->ucChannelNumber));
#endif

		channelHandle->ulIntrStatus1 |= unIntrStaReg1;
		channelHandle->ulIntrStatus2 |= unIntrStaReg2;

		/* We need to signal different events to take care of different scenarioes */
		event = BSCD_IntType_eWaitInt;
		for (i=0; i< BSCD_MAX_NUM_CALLBACK_FUNC; i++)  {			
			if (channelHandle->callBack.waitIsrCBFunc[i] != NULL)
				(*(channelHandle->callBack.waitIsrCBFunc[i])) (channelHandle, &event);
		}
	}

	if ( (unIntrEn2 & BCHP_SCA_SC_INTR_EN_2_rcv_ien_MASK) && (unIntrStaReg2 & BCHP_SCA_SC_INTR_STAT_2_rcv_intr_MASK) ) {
#ifdef BSCD_INTERRUPT_DEBUG		
	BDBG_MSG(("RCV_INTR IntrEn2 = %x, IntrStaReg2 = %x, ucSlot = %d\n", 
					unIntrEn2, unIntrStaReg2, channelHandle->ucChannelNumber));
#endif

/* Enable RCV_INTR only in T=1, EMV 2000 to resolve CWT+4 issue */
#ifdef BSCD_EMV2000_CWT_PLUS_4
		if ( (channelHandle->currentChannelSettings.scStandard == BSCD_Standard_eEMV2000) &&
		  	(channelHandle->currentChannelSettings.eProtocolType == BSCD_AsyncProtocolType_e1) &&
		  	(channelHandle->bIsReceive == true))  {

			/* Disable BWT timer */
			cwtTimer.bIsTimerInterruptEnable = false;
			cwtTimer.bIsTimerEnable = false;
			BSCD_P_CHECK_ERR_CODE_FUNC2(errCode, BSCD_STATUS_READ_FAILED,
				BSCD_Channel_EnableDisableTimer_isr(channelHandle, &cwtTimer));

			/* Enable WWT in lieu of CWT */
			cwtTimer.bIsTimerInterruptEnable = true;
			cwtTimer.bIsTimerEnable = true;			
			if (channelHandle->currentChannelSettings.ulCharacterWaitTimeInteger != 0) 
				cwtTimeValue.ulValue = (2<<(channelHandle->currentChannelSettings.ulCharacterWaitTimeInteger-1)) 
					+ 15 + BSCD_CHARACTER_WAIT_TIME_GRACE_PERIOD;
			BSCD_P_CHECK_ERR_CODE_FUNC2(errCode, BSCD_STATUS_READ_FAILED,
					BSCD_Channel_ConfigTimer(channelHandle, &cwtTimer, &cwtTimeValue));
			
			BDBG_MSG(("RCV_INTR  cwt = %d\n", cwtTimeValue.ulValue));	
			
			channelHandle->ulStatus2 |= unStaReg2;	
			unIntrStaReg2 &= ~BCHP_SCA_SC_INTR_STAT_2_rcv_intr_MASK;
										
		}	
				
#endif			
/* Enable RCV_INTR only in T=1, EMV 2000 to resolve CWT+4 issue */	

		channelHandle->ulIntrStatus1 |= unIntrStaReg1;
		channelHandle->ulIntrStatus2 |= unIntrStaReg2;		

		event = BSCD_IntType_eRcvInt;
		for (i=0; i< BSCD_MAX_NUM_CALLBACK_FUNC; i++)  {	
			if (channelHandle->callBack.rcvIsrCBFunc[i] != NULL)
				(*(channelHandle->callBack.rcvIsrCBFunc[i])) (channelHandle, &event);
		}
	}

	if ( (unIntrEn2 & BCHP_SCA_SC_INTR_EN_2_rready_ien_MASK) && (unIntrStaReg2 & BCHP_SCA_SC_INTR_STAT_2_rready_intr_MASK) ) {
#ifdef BSCD_INTERRUPT_DEBUG		
		BDBG_MSG(("RREADY_INTR IntrEn2 = %x, IntrStaReg2 = %x, ucSlot = %d\n",
						unIntrEn2, unIntrStaReg2, channelHandle->ucChannelNumber));
#endif

#ifdef BSCD_EMV2000_CWT_PLUS_4
		if ( (channelHandle->currentChannelSettings.scStandard == BSCD_Standard_eEMV2000) &&
		  	(channelHandle->currentChannelSettings.eProtocolType == BSCD_AsyncProtocolType_e1)) {		
			/* Disable WWT timer, which is used as CWT + 4  */
			cwtTimer.bIsTimerInterruptEnable = false;
			cwtTimer.bIsTimerEnable = false;
			BSCD_P_CHECK_ERR_CODE_FUNC2(errCode, BSCD_STATUS_READ_FAILED,
				BSCD_Channel_EnableDisableTimer_isr(channelHandle, &cwtTimer));
			BDBG_MSG(("RREADY_INTR  cwt disable\n"));
		}
#endif

		channelHandle->ulStatus2 |= unStaReg2;	
		channelHandle->ulIntrStatus1 |= unIntrStaReg1;
		channelHandle->ulIntrStatus2 |= unIntrStaReg2;

		event = BSCD_IntType_eRReadyInt;
		for (i=0; i< BSCD_MAX_NUM_CALLBACK_FUNC; i++)  {	
			if (channelHandle->callBack.rReadyIsrCBFunc[i] != NULL)
				(*(channelHandle->callBack.rReadyIsrCBFunc[i])) (channelHandle,  &event);
		}
	}

	if ( (unProtoCmdReg & BCHP_SCA_SC_PROTO_CMD_edc_en_MASK) && (unStaReg2 & BCHP_SCA_SC_STATUS_2_edc_err_MASK) ) {
#ifdef BSCD_INTERRUPT_DEBUG		
		BDBG_MSG(("EDC_ERR , ProtoCmdReg = %x, unStaReg2 = %x, ucSlot = %d\n", 
						unProtoCmdReg, unStaReg2, channelHandle->ucChannelNumber));
#endif

		channelHandle->channelStatus.ulStatus1 |= BSCD_TX_PARITY;
		channelHandle->ulIntrStatus1 |= unIntrStaReg1;
		channelHandle->ulIntrStatus2 |= unIntrStaReg2;

		event = BSCD_IntType_eEDCInt;
		for (i=0; i< BSCD_MAX_NUM_CALLBACK_FUNC; i++)  {			
			if (channelHandle->callBack.edcIsrCBFunc[i] != NULL) 
				(*(channelHandle->callBack.edcIsrCBFunc[i])) (channelHandle, &event);
		}
	}

#ifdef BSCD_EMV2000_CWT_PLUS_4_EVENT_INTR
	if ( (unIntrEn1 & BCHP_SCA_SC_INTR_EN_1_event1_ien_MASK) && (unIntrStaReg1 & BCHP_SCA_SC_INTR_STAT_1_event1_intr_MASK) ) {
#ifdef BSCD_INTERRUPT_DEBUG
		BDBG_MSG(("EVENT1_INTR IntrEn1 = %x, IntrStaReg1 = %x, ucSlot = %d\n",
						unIntrEn1, unIntrStaReg1, channelHandle->ucChannelNumber));
#endif

		/* If cwt_intr comes in after rready_intr, it is considered normal */
		channelHandle->ulIntrStatus1 |= unIntrStaReg1;
		channelHandle->ulIntrStatus2 |= unIntrStaReg2;

		event = BSCD_IntType_eEvent1Int;
		for (i=0; i< BSCD_MAX_NUM_CALLBACK_FUNC; i++)  {	
			if (channelHandle->callBack.event1IsrCBFunc[i] != NULL)
				(*(channelHandle->callBack.event1IsrCBFunc[i])) (channelHandle, &event);
		}
	}

	if ( (unIntrEn2 & BCHP_SCA_SC_INTR_EN_2_event2_ien_MASK) && (unIntrStaReg2 & BCHP_SCA_SC_INTR_STAT_2_event2_intr_MASK) ) {
#ifdef BSCD_INTERRUPT_DEBUG
		BDBG_MSG(("EVENT2_INTR IntrEn2 = %x, IntrStaReg2 = %x, ucSlot = %d\n",
						unIntrEn2, unIntrStaReg2, channelHandle->ucChannelNumber));
#endif

		/* If cwt_intr comes in after rready_intr, it is considered normal */
		channelHandle->ulIntrStatus1 |= unIntrStaReg1;
		channelHandle->ulIntrStatus2 |= unIntrStaReg2;

		event = BSCD_IntType_eEvent2Int;
		for (i=0; i< BSCD_MAX_NUM_CALLBACK_FUNC; i++)  {	
			if (channelHandle->callBack.event2IsrCBFunc[i] != NULL)
				(*(channelHandle->callBack.event2IsrCBFunc[i])) (channelHandle, &event);
		}
	}

#endif	

BSCD_P_DONE_LABEL:

	BDBG_LEAVE(BSCD_IntHandler_isr);
	return;   
}



/* For T=0 and T=1 only */
BERR_Code BSCD_Channel_P_T0T1Transmit(		
		BSCD_ChannelHandle          in_channelHandle,
		uint8_t                     *inp_ucXmitData,
		unsigned long                    in_ulNumXmitBytes
)
{
	BERR_Code errCode = BERR_SUCCESS;
	uint32_t         ulVal; 
	unsigned int          i;
	BSCD_Timer 		timer = {BSCD_TimerType_eGPTimer, {BSCD_GPTimerMode_eIMMEDIATE}, true, true};	
	BSCD_TimerValue    timeValue= {BSCD_MIN_DELAY_BEFORE_TZERO_SEND, BSCD_TimerUnit_eETU};	

	BDBG_ENTER(BSCD_Channel_P_T0T1Transmit);
	BDBG_ASSERT( in_channelHandle );

	BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, 
		(in_channelHandle->ulMagicNumber != BSCD_P_CHANNEL_HANDLE_MAGIC_NUMBER ) );

	BSCD_P_CHECK_ERR_CODE_CONDITION(
		 errCode,BSCD_STATUS_SEND_FAILED,(in_channelHandle->bIsCardRemoved == true));

	/* BSCD_P_HexDump("Send",inp_ucXmitData, in_ulNumXmitBytes); */

	BKNI_EnterCriticalSection();
	in_channelHandle->ulIntrStatus1 &=
		~BCHP_SCA_SC_INTR_STAT_1_tpar_intr_MASK & 
		~BCHP_SCA_SC_INTR_STAT_1_timer_intr_MASK & 
		~BCHP_SCA_SC_INTR_STAT_1_bgt_intr_MASK & 
		~BCHP_SCA_SC_INTR_STAT_1_tdone_intr_MASK & 
		~BCHP_SCA_SC_INTR_STAT_1_retry_intr_MASK & 
		~BCHP_SCA_SC_INTR_STAT_1_tempty_intr_MASK
#ifdef BSCD_EMV2000_CWT_PLUS_4_EVENT_INTR
		 & ~BCHP_SCA_SC_INTR_STAT_1_event1_intr_MASK
#endif		 
		; 
	
	in_channelHandle->ulIntrStatus2 &=
		~BCHP_SCA_SC_INTR_STAT_2_rpar_intr_MASK & 
		~BCHP_SCA_SC_INTR_STAT_2_cwt_intr_MASK & 
		~BCHP_SCA_SC_INTR_STAT_2_rlen_intr_MASK & 
		~BCHP_SCA_SC_INTR_STAT_2_wait_intr_MASK & 
		~BCHP_SCA_SC_INTR_STAT_2_rcv_intr_MASK &
		~BCHP_SCA_SC_INTR_STAT_2_rready_intr_MASK &
		~BCHP_SCA_SC_INTR_STAT_2_event2_intr_MASK ;
	BKNI_LeaveCriticalSection();


	
	/* Reset the Transmit and Receive buffer */
	ulVal =  BCHP_SCA_SC_PROTO_CMD_tbuf_rst_MASK | BCHP_SCA_SC_PROTO_CMD_rbuf_rst_MASK |
		BREG_Read32( 
		in_channelHandle->moduleHandle->regHandle, 
		(in_channelHandle->ulRegStartAddr + BSCD_P_PROTO_CMD)) ;	
	BREG_Write32( 
		in_channelHandle->moduleHandle->regHandle, 
		(in_channelHandle->ulRegStartAddr + BSCD_P_PROTO_CMD), 
		 ulVal);   

	/* 
	  Enable cwt here for only T=1. We will disable cwt in 
	  SmartCardTOneReceive() after we receive RREADY_INTR
	*/
	if (in_channelHandle->currentChannelSettings.eProtocolType == BSCD_AsyncProtocolType_e1) {

		/* Clear the possible previous cwt_intr */
		in_channelHandle->ulIntrStatus2 &= ~BCHP_SCA_SC_INTR_STAT_2_cwt_intr_MASK;
		
		ulVal =  BCHP_SCA_SC_TIMER_CMD_cwt_en_MASK |
				BREG_Read32( 
				in_channelHandle->moduleHandle->regHandle, 
				(in_channelHandle->ulRegStartAddr + BSCD_P_TIMER_CMD)) ;	
		BREG_Write32( 
			in_channelHandle->moduleHandle->regHandle, 
			(in_channelHandle->ulRegStartAddr + BSCD_P_TIMER_CMD), 
			 ulVal);  

#ifdef BSCD_EMV2000_CWT_PLUS_4_EVENT_INTR

		/* Clear the possible previous event1 intr */
		in_channelHandle->ulIntrStatus1 &= ~BCHP_SCA_SC_INTR_STAT_1_event1_intr_MASK; 

		if  (in_channelHandle->currentChannelSettings.scStandard == BSCD_Standard_eEMV2000) {
			/* 4 ETU after CWT */
			BREG_Write32( 
				in_channelHandle->moduleHandle->regHandle, 
				(in_channelHandle->ulRegStartAddr + BSCD_P_EVENT1_CMP), 
				 5); 

			/* start event src */
			BREG_Write32( 
				in_channelHandle->moduleHandle->regHandle, 
				(in_channelHandle->ulRegStartAddr + BSCD_P_EVENT1_CMD_3), 
				 BSCD_P_CWT_INTR_EVENT_SRC); 	

			/* increment event src */
			BREG_Write32( 
				in_channelHandle->moduleHandle->regHandle, 
				(in_channelHandle->ulRegStartAddr + BSCD_P_EVENT1_CMD_2), 
				 BSCD_P_RX_ETU_TICK_EVENT_SRC); 			

			/* reset event src */
			BREG_Write32( 
				in_channelHandle->moduleHandle->regHandle, 
				(in_channelHandle->ulRegStartAddr + BSCD_P_EVENT1_CMD_1), 
				 BSCD_P_RX_START_BIT_EVENT_SRC); 

			/* event_en, intr_mode, run_after_reset and run_after_compare*/
			ulVal = BCHP_SCA_SC_EVENT1_CMD_4_event_en_MASK | 
					BCHP_SCA_SC_EVENT1_CMD_4_intr_after_compare_MASK |
					BCHP_SCA_SC_EVENT1_CMD_4_run_after_reset_MASK;
			
			ulVal &= ~(BCHP_SCA_SC_EVENT1_CMD_4_intr_after_reset_MASK |
						BCHP_SCA_SC_EVENT1_CMD_4_run_after_compare_MASK);
			BREG_Write32( 
				in_channelHandle->moduleHandle->regHandle, 
				(in_channelHandle->ulRegStartAddr + BSCD_P_EVENT1_CMD_4), 
				 ulVal); 		
			
		}
#endif		
		
	}
	
    
	/*
	  For EMV T=0 only, the minimum interval btw the leading 
	  edges of the start bits of 2 consecutive characters sent 
	  in opposite directions shall be 16.  For EMV and ISO T=1, 
	  the minimum interval btw the leading edges of the start bits of 2 
	  consecutive characters sent in opposite directions shall be 22.
	*/
      
	if (in_channelHandle->currentChannelSettings.eProtocolType == BSCD_AsyncProtocolType_e0) {

	
		BSCD_P_CHECK_ERR_CODE_FUNC(errCode, 
			BSCD_Channel_ConfigTimer(in_channelHandle, &timer, &timeValue));	
	
	} 

	else {

		/* Set Timer */
		/* 	timer.bIsTimerInterruptEnable = true;
		     	timer.bIsTimerEnable = true;
		     	timer.eTimerType = BSCD_TimerType_eGPTimer;
		     	timer.timerMode.eGPTimerMode = BSCD_GPTimerMode_eIMMEDIATE;
			timeValue.ulValue = BSCD_BLOCK_GUARD_TIME;
			timeValue.unit  = BSCD_TimerUnit_eETU;
		*/
		
		BSCD_P_CHECK_ERR_CODE_FUNC(errCode, 
			BSCD_Channel_ConfigTimer(in_channelHandle, &timer, &timeValue));	

	}
	
	BSCD_P_CHECK_ERR_CODE_FUNC(errCode, 
			BSCD_Channel_P_WaitForTimerEvent(in_channelHandle));	
   

   	/* Disable timer */
	timer.bIsTimerInterruptEnable = false;
	timer.bIsTimerEnable = false;
	BSCD_P_CHECK_ERR_CODE_FUNC(errCode, 
		BSCD_Channel_EnableDisableTimer_isr(in_channelHandle, &timer));	


	/* For T=1, we have to check the Block wait time */
	/* For T=0, we have to check the Work Wait Time.  */
	/* BSYT Issue: RC0 WWT timer could only check the interval 
	  btw the leading edge of 2 consecutive characters sent 
	  by the ICC.  We will use GP timer to check the interval 
	  btw the leading edge of characters in opposite directions 
	*/
	
	if (in_channelHandle->currentChannelSettings.eProtocolType == BSCD_AsyncProtocolType_e0) {

			/* Restore the original WWT */
			timer.bIsTimerInterruptEnable = true;
			timer.bIsTimerEnable = true;
			timer.eTimerType = BSCD_TimerType_eWaitTimer;
			timer.timerMode.eWaitTimerMode = BSCD_WaitTimerMode_eWorkWaitTime;			
			timeValue.ulValue = in_channelHandle->currentChannelSettings.workWaitTime.ulValue;		
			BSCD_P_CHECK_ERR_CODE_FUNC(errCode, 
					BSCD_Channel_ConfigTimer(in_channelHandle, &timer, &timeValue));	

	}

	else if (in_channelHandle->currentChannelSettings.eProtocolType == BSCD_AsyncProtocolType_e1) {
			timer.bIsTimerInterruptEnable = true;
			timer.bIsTimerEnable = true;
			timer.eTimerType = BSCD_TimerType_eWaitTimer;			
			timer.timerMode.eWaitTimerMode = BSCD_WaitTimerMode_eBlockWaitTime;	

			if (in_channelHandle->currentChannelSettings.blockWaitTimeExt.ulValue == 0)
				timeValue.ulValue = in_channelHandle->currentChannelSettings.blockWaitTime.ulValue ;
			else
				timeValue.ulValue = in_channelHandle->currentChannelSettings.blockWaitTimeExt.ulValue;
			BSCD_P_CHECK_ERR_CODE_FUNC(errCode, 
					BSCD_Channel_ConfigTimer(in_channelHandle, &timer, &timeValue));			
	}

	/*
	* Fill BCM FIFO with the request message.
	*/
	for (i = 0; i < in_ulNumXmitBytes; i++) {

		BREG_Write32( 
			in_channelHandle->moduleHandle->regHandle, 
			(in_channelHandle->ulRegStartAddr + BSCD_P_TRANSMIT), 
			 (uint32_t) inp_ucXmitData[i]);  

		BDBG_MSG(("%02x ", inp_ucXmitData[i]));
	}
	BDBG_MSG(("\n"));

	/* Enable EDC */	 
	ulVal = BREG_Read32( 
			in_channelHandle->moduleHandle->regHandle, 
			(in_channelHandle->ulRegStartAddr + BSCD_P_PROTO_CMD));

	if (in_channelHandle->currentChannelSettings.eProtocolType == BSCD_AsyncProtocolType_e0)  {

		ulVal |= BCHP_SCA_SC_PROTO_CMD_rbuf_rst_MASK;
	}	
	else {
		if (in_channelHandle->currentChannelSettings.scStandard == BSCD_Standard_eES) {

			/*
			 application computes its own LRC or CRC and appends it as the last byte 
			*/
			ulVal |= BCHP_SCA_SC_PROTO_CMD_rbuf_rst_MASK;
		}
		else {

            if(in_channelHandle->currentChannelSettings.edcSetting.bIsEnabled==false)
                ulVal |= BCHP_SCA_SC_PROTO_CMD_rbuf_rst_MASK;
            else{

			ulVal |=  BCHP_SCA_SC_PROTO_CMD_edc_en_MASK;
	
			if (in_channelHandle->currentChannelSettings.edcSetting.edcEncode == BSCD_EDCEncode_eLRC ) {
			ulVal &= ~BCHP_SCA_SC_PROTO_CMD_crc_lrc_MASK;
		}
			else if (in_channelHandle->currentChannelSettings.edcSetting.edcEncode == BSCD_EDCEncode_eCRC) {
				ulVal |=  BCHP_SCA_SC_PROTO_CMD_crc_lrc_MASK;		
			}		
            	}
			}
	}

	BREG_Write32( 
			in_channelHandle->moduleHandle->regHandle, 
			(in_channelHandle->ulRegStartAddr + BSCD_P_PROTO_CMD), 
			ulVal);  


	/* Set flow cmd */
	ulVal = BREG_Read32( 
			in_channelHandle->moduleHandle->regHandle, 
			(in_channelHandle->ulRegStartAddr + BSCD_P_FLOW_CMD));	

	if (in_channelHandle->currentChannelSettings.eProtocolType == BSCD_AsyncProtocolType_e0)  {
		/* 
		Only NDS support smartcard flow control. We MUST NOT set SC_FLOW_EN to 1 for 
		other standards. 
		*/
		if (in_channelHandle->currentChannelSettings.scStandard == BSCD_Standard_eNDS) {
			ulVal |= (BCHP_SCA_SC_FLOW_CMD_flow_en_MASK) ;
		}
		else {
			ulVal &= ~BCHP_SCA_SC_FLOW_CMD_flow_en_MASK;
		}
	}
	else {
		/* No flow control for T=1 protocol or T=14. */
		ulVal &= ~BCHP_SCA_SC_FLOW_CMD_flow_en_MASK;
	}
	BREG_Write32( 
			in_channelHandle->moduleHandle->regHandle, 
			(in_channelHandle->ulRegStartAddr + BSCD_P_FLOW_CMD), 
			ulVal);  


	/* Ready to transmit */
	ulVal = BREG_Read32( 
			in_channelHandle->moduleHandle->regHandle, 
			(in_channelHandle->ulRegStartAddr + BSCD_P_UART_CMD_1));	

	/* Always set auto receive */
	ulVal = BCHP_SCA_SC_UART_CMD_1_t_r_MASK | BCHP_SCA_SC_UART_CMD_1_xmit_go_MASK | 
			BCHP_SCA_SC_UART_CMD_1_io_en_MASK |BCHP_SCA_SC_UART_CMD_1_auto_rcv_MASK ;
         
	BREG_Write32( 
			in_channelHandle->moduleHandle->regHandle, 
			(in_channelHandle->ulRegStartAddr + BSCD_P_UART_CMD_1), 
			ulVal);


#ifdef BSCD_EMV2000_CWT_PLUS_4
	in_channelHandle->bIsReceive = true;
#endif

	/*
	* Wait until the BCM sent all the data.
	*/
	BSCD_P_CHECK_ERR_CODE_FUNC(errCode, 
			BSCD_Channel_P_WaitForTDone(in_channelHandle));

if (in_channelHandle->currentChannelSettings.scStandard == BSCD_Standard_eNDS){

	ulVal = BREG_Read32( 
			in_channelHandle->moduleHandle->regHandle, 
			(in_channelHandle->ulRegStartAddr + BSCD_P_FLOW_CMD));	
	ulVal &= ~BCHP_SCA_SC_FLOW_CMD_flow_en_MASK;
	BREG_Write32( 
			in_channelHandle->moduleHandle->regHandle, 
			(in_channelHandle->ulRegStartAddr + BSCD_P_FLOW_CMD), 
			ulVal);  
}

BSCD_P_DONE_LABEL:
	
	BDBG_LEAVE(BSCD_Channel_P_T0T1Transmit);
	return( errCode );
}


/* For T=14 only */
#if (BCHP_CHIP==7118)     /* 7118 has support for T=14 */
BERR_Code BSCD_Channel_P_T14IrdetoTransmit(		
		BSCD_ChannelHandle          in_channelHandle,
		uint8_t                     *inp_ucXmitData,
		unsigned long                    in_ulNumXmitBytes
)
{
	BERR_Code errCode = BERR_SUCCESS;
	uint32_t         ulVal; 
	unsigned int          i;
	BSCD_Timer 		timer = {BSCD_TimerType_eGPTimer, {BSCD_GPTimerMode_eIMMEDIATE}, true, true};	
	BSCD_TimerValue    timeValue= {BSCD_T14_IRDETO_MIN_DELAY_RX2TX, BSCD_TimerUnit_eCLK};	

	BDBG_ENTER(BSCD_Channel_P_T14IrdetoTransmit);
	BDBG_ASSERT( in_channelHandle );

	BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, 
		(in_channelHandle->ulMagicNumber != BSCD_P_CHANNEL_HANDLE_MAGIC_NUMBER ) );

	BSCD_P_CHECK_ERR_CODE_CONDITION(
		 errCode,BSCD_STATUS_SEND_FAILED,(in_channelHandle->bIsCardRemoved == true));

	/* BSCD_P_HexDump("Send",inp_ucXmitData, in_ulNumXmitBytes); */

	BKNI_EnterCriticalSection();
	in_channelHandle->ulIntrStatus1 &=
		~BCHP_SCA_SC_INTR_STAT_1_tpar_intr_MASK & 
		~BCHP_SCA_SC_INTR_STAT_1_timer_intr_MASK & 
		~BCHP_SCA_SC_INTR_STAT_1_bgt_intr_MASK & 
		~BCHP_SCA_SC_INTR_STAT_1_tdone_intr_MASK & 
		~BCHP_SCA_SC_INTR_STAT_1_retry_intr_MASK & 
		~BCHP_SCA_SC_INTR_STAT_1_tempty_intr_MASK;
	
	in_channelHandle->ulIntrStatus2 &=
		~BCHP_SCA_SC_INTR_STAT_2_rpar_intr_MASK & 
		~BCHP_SCA_SC_INTR_STAT_2_cwt_intr_MASK & 
		~BCHP_SCA_SC_INTR_STAT_2_rlen_intr_MASK & 
		~BCHP_SCA_SC_INTR_STAT_2_wait_intr_MASK & 
		~BCHP_SCA_SC_INTR_STAT_2_rcv_intr_MASK &
		~BCHP_SCA_SC_INTR_STAT_2_rready_intr_MASK;
	BKNI_LeaveCriticalSection();


	
	/* Reset the Transmit and Receive buffer */
	ulVal =  BCHP_SCA_SC_PROTO_CMD_tbuf_rst_MASK | BCHP_SCA_SC_PROTO_CMD_rbuf_rst_MASK |
		BREG_Read32( 
		in_channelHandle->moduleHandle->regHandle, 
		(in_channelHandle->ulRegStartAddr + BSCD_P_PROTO_CMD)) ;	
	BREG_Write32( 
		in_channelHandle->moduleHandle->regHandle, 
		(in_channelHandle->ulRegStartAddr + BSCD_P_PROTO_CMD), 
		 ulVal);   

	/* Need to wait for minimum of 1250 from last RX to this TX */
	BSCD_P_CHECK_ERR_CODE_FUNC(errCode, 
			BSCD_Channel_ConfigTimer(in_channelHandle, &timer, &timeValue));	

	
	BSCD_P_CHECK_ERR_CODE_FUNC(errCode, 
			BSCD_Channel_P_WaitForTimerEvent(in_channelHandle));	
   
   	/* Disable timer */
	timer.bIsTimerInterruptEnable = false;
	timer.bIsTimerEnable = false;
	BSCD_P_CHECK_ERR_CODE_FUNC(errCode, 
		BSCD_Channel_EnableDisableTimer_isr(in_channelHandle, &timer));	


	/* Enable EDC */	 
	ulVal = BREG_Read32( 
			in_channelHandle->moduleHandle->regHandle, 
			(in_channelHandle->ulRegStartAddr + BSCD_P_PROTO_CMD));
	
	ulVal |= BCHP_SCA_SC_PROTO_CMD_rbuf_rst_MASK;

	BREG_Write32( 
			in_channelHandle->moduleHandle->regHandle, 
			(in_channelHandle->ulRegStartAddr + BSCD_P_PROTO_CMD), 
			ulVal);  


	/* Set tguard to be 0*/
	/*ulVal = BREG_Read32( 
			in_channelHandle->moduleHandle->regHandle, 
			(in_channelHandle->ulRegStartAddr + BSCD_P_TGUARD));
	
	if(ulVal !=1) ulVal = 0;
	BREG_Write32( 
			in_channelHandle->moduleHandle->regHandle, 
			(in_channelHandle->ulRegStartAddr + BSCD_P_TGUARD), 
			ulVal);*/  
	/* Set flow cmd */
	ulVal = BREG_Read32( 
			in_channelHandle->moduleHandle->regHandle, 
			(in_channelHandle->ulRegStartAddr + BSCD_P_FLOW_CMD));	

	/* No flow control for T=1 protocol or T=14. */
	ulVal &= ~BCHP_SCA_SC_FLOW_CMD_flow_en_MASK;
	ulVal |= 0x00000060;                       /* 	SC_FLOW_CMD[t10etu]=1 , SC_FLOW_CMD[r10etu] */

	BREG_Write32( 
			in_channelHandle->moduleHandle->regHandle, 
			(in_channelHandle->ulRegStartAddr + BSCD_P_FLOW_CMD), 
			ulVal);  

	/* Set uart cmd */
	ulVal = BREG_Read32( 
			in_channelHandle->moduleHandle->regHandle, 
			(in_channelHandle->ulRegStartAddr + BSCD_P_UART_CMD_1));	
	
	ulVal |= 0x00000008;  /* SC_UART_CMD[dispar]=1  */
	
	BREG_Write32( 
			in_channelHandle->moduleHandle->regHandle, 
			(in_channelHandle->ulRegStartAddr + BSCD_P_UART_CMD_1), 
			ulVal);  
	
/*
	* Fill BCM FIFO with the request message.
	*/
	for (i = 0; i < in_ulNumXmitBytes; i++) 
   {

		BREG_Write32( 
			in_channelHandle->moduleHandle->regHandle, 
			(in_channelHandle->ulRegStartAddr + BSCD_P_TRANSMIT), 
			 (uint32_t) inp_ucXmitData[i]);  

		BDBG_MSG(("%02x ", inp_ucXmitData[i]));
		
		/* Ready to transmit */
		ulVal = BREG_Read32( 
				in_channelHandle->moduleHandle->regHandle, 
				(in_channelHandle->ulRegStartAddr + BSCD_P_UART_CMD_1));	

		/* Always set auto receive */
      /* "xmit_go" will be set only when all the Bytes will be in the FIFO */
		ulVal = BCHP_SCA_SC_UART_CMD_1_t_r_MASK | BCHP_SCA_SC_UART_CMD_1_io_en_MASK ;

  		
		if (i == (in_ulNumXmitBytes - 1)) 
			ulVal |= BCHP_SCA_SC_UART_CMD_1_auto_rcv_MASK;  /* Last TX byte, ready to receive */
		/* printf("ul val %x \n", ulVal); */
		BREG_Write32( 
				in_channelHandle->moduleHandle->regHandle, 
				(in_channelHandle->ulRegStartAddr + BSCD_P_UART_CMD_1), 
				ulVal);
		
	}



   /* Now that the FIFO is filled we can set the "xmit_go" bit to start the transmission */
	ulVal = BREG_Read32( 
			in_channelHandle->moduleHandle->regHandle, 
			(in_channelHandle->ulRegStartAddr + BSCD_P_UART_CMD_1));	

   ulVal |= BCHP_SCA_SC_UART_CMD_1_xmit_go_MASK;

	BREG_Write32( 
			in_channelHandle->moduleHandle->regHandle, 
			(in_channelHandle->ulRegStartAddr + BSCD_P_UART_CMD_1), 
			ulVal);

	/*
	* Wait until the BCM sent all the data.
	*/
	BSCD_P_CHECK_ERR_CODE_FUNC(errCode, 
			BSCD_Channel_P_WaitForTDone(in_channelHandle));		
	/*ulVal |= BCHP_SCA_SC_UART_CMD_1_auto_rcv_MASK; */ /* TX done, ready to receive */
	/*	BREG_Write32( 
			in_channelHandle->moduleHandle->regHandle, 
			(in_channelHandle->ulRegStartAddr + BSCD_P_UART_CMD_1), 
			ulVal);*/
	
	BDBG_MSG(("\n"));

BSCD_P_DONE_LABEL:
	
	BDBG_LEAVE(BSCD_Channel_P_T14IrdetoTransmit);
	return( errCode );
}
#else

BERR_Code BSCD_Channel_P_T14IrdetoTransmit(		
		BSCD_ChannelHandle          in_channelHandle,
		uint8_t                     *inp_ucXmitData,
		unsigned long                    in_ulNumXmitBytes
)
{
	BERR_Code errCode = BERR_SUCCESS;
	uint32_t         ulVal; 
	unsigned int          i;
	BSCD_Timer 		timer = {BSCD_TimerType_eGPTimer, {BSCD_GPTimerMode_eIMMEDIATE}, true, true};	
	BSCD_TimerValue    timeValue= {BSCD_T14_IRDETO_MIN_DELAY_RX2TX, BSCD_TimerUnit_eCLK};	

	BDBG_ENTER(BSCD_Channel_P_T14IrdetoTransmit);
	BDBG_ASSERT( in_channelHandle );

	BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, 
		(in_channelHandle->ulMagicNumber != BSCD_P_CHANNEL_HANDLE_MAGIC_NUMBER ) );

	BSCD_P_CHECK_ERR_CODE_CONDITION(
		 errCode,BSCD_STATUS_SEND_FAILED,(in_channelHandle->bIsCardRemoved == true));

	/* BSCD_P_HexDump("Send",inp_ucXmitData, in_ulNumXmitBytes); */
	BKNI_EnterCriticalSection();
	in_channelHandle->ulIntrStatus1 &=
		~BCHP_SCA_SC_INTR_STAT_1_tpar_intr_MASK & 
		~BCHP_SCA_SC_INTR_STAT_1_timer_intr_MASK & 
		~BCHP_SCA_SC_INTR_STAT_1_bgt_intr_MASK & 
		~BCHP_SCA_SC_INTR_STAT_1_tdone_intr_MASK & 
		~BCHP_SCA_SC_INTR_STAT_1_retry_intr_MASK & 
		~BCHP_SCA_SC_INTR_STAT_1_tempty_intr_MASK;
	
	in_channelHandle->ulIntrStatus2 &=
		~BCHP_SCA_SC_INTR_STAT_2_rpar_intr_MASK & 
		~BCHP_SCA_SC_INTR_STAT_2_cwt_intr_MASK & 
		~BCHP_SCA_SC_INTR_STAT_2_rlen_intr_MASK & 
		~BCHP_SCA_SC_INTR_STAT_2_wait_intr_MASK & 
		~BCHP_SCA_SC_INTR_STAT_2_rcv_intr_MASK &
		~BCHP_SCA_SC_INTR_STAT_2_rready_intr_MASK;
	BKNI_LeaveCriticalSection();

	
	/* Reset the Transmit and Receive buffer */
	ulVal =  BCHP_SCA_SC_PROTO_CMD_tbuf_rst_MASK | BCHP_SCA_SC_PROTO_CMD_rbuf_rst_MASK |
		BREG_Read32( 
		in_channelHandle->moduleHandle->regHandle, 
		(in_channelHandle->ulRegStartAddr + BSCD_P_PROTO_CMD)) ;	
	BREG_Write32( 
		in_channelHandle->moduleHandle->regHandle, 
		(in_channelHandle->ulRegStartAddr + BSCD_P_PROTO_CMD), 
		 ulVal);   

	/* Need to wait for minimum of 1250 from last RX to this TX */
	BSCD_P_CHECK_ERR_CODE_FUNC(errCode, 
			BSCD_Channel_ConfigTimer(in_channelHandle, &timer, &timeValue));	

	
	BSCD_P_CHECK_ERR_CODE_FUNC(errCode, 
			BSCD_Channel_P_WaitForTimerEvent(in_channelHandle));	
   
   	/* Disable timer */
	timer.bIsTimerInterruptEnable = false;
	timer.bIsTimerEnable = false;
	BSCD_P_CHECK_ERR_CODE_FUNC(errCode, 
		BSCD_Channel_EnableDisableTimer_isr(in_channelHandle, &timer));	


	/* Enable EDC */	 
	ulVal = BREG_Read32( 
			in_channelHandle->moduleHandle->regHandle, 
			(in_channelHandle->ulRegStartAddr + BSCD_P_PROTO_CMD));
	
	ulVal |= BCHP_SCA_SC_PROTO_CMD_rbuf_rst_MASK;

	BREG_Write32( 
			in_channelHandle->moduleHandle->regHandle, 
			(in_channelHandle->ulRegStartAddr + BSCD_P_PROTO_CMD), 
			ulVal);  


	/* Set flow cmd */
	ulVal = BREG_Read32( 
			in_channelHandle->moduleHandle->regHandle, 
			(in_channelHandle->ulRegStartAddr + BSCD_P_FLOW_CMD));	

	/* No flow control for T=1 protocol or T=14. */
	ulVal &= ~BCHP_SCA_SC_FLOW_CMD_flow_en_MASK;

	BREG_Write32( 
			in_channelHandle->moduleHandle->regHandle, 
			(in_channelHandle->ulRegStartAddr + BSCD_P_FLOW_CMD), 
			ulVal);  



	   
	/*
	* Fill BCM FIFO with the request message.
	*/
	for (i = 0; i < in_ulNumXmitBytes; i++) {

		BREG_Write32( 
			in_channelHandle->moduleHandle->regHandle, 
			(in_channelHandle->ulRegStartAddr + BSCD_P_TRANSMIT), 
			 (uint32_t) inp_ucXmitData[i]);  

		BDBG_MSG(("%02x ", inp_ucXmitData[i]));

		/* Ready to transmit */
		ulVal = BREG_Read32( 
				in_channelHandle->moduleHandle->regHandle, 
				(in_channelHandle->ulRegStartAddr + BSCD_P_UART_CMD_1));	

		/* Always set auto receive */
		ulVal = BCHP_SCA_SC_UART_CMD_1_t_r_MASK | BCHP_SCA_SC_UART_CMD_1_xmit_go_MASK | 
				BCHP_SCA_SC_UART_CMD_1_io_en_MASK ;

		/*if (BSCD_P_Popcount[inp_ucXmitData[i]] % 2 == 1) {
			BDBG_MSG(("flip the parity \n"));
			ulVal |= BCHP_SCA_SC_UART_CMD_1_inv_par_MASK;
			}*/
		if (BSCD_P_Popcount[inp_ucXmitData[i]] % 2 == 0) 
      {
   		BDBG_MSG(("Even number of one => Parity = 0 => flip the parity \n"));
			
			ulVal |= BCHP_SCA_SC_UART_CMD_1_inv_par_MASK;
		}
		else 
      {
         BDBG_MSG(("Odd number of one => Parity = 1 => Do not flip the parity \n"));   
			
			ulVal &= ~BCHP_SCA_SC_UART_CMD_1_inv_par_MASK;			
		}
		if (i == (in_ulNumXmitBytes - 1)) 
			ulVal |= BCHP_SCA_SC_UART_CMD_1_auto_rcv_MASK;  /* Last TX byte, ready to receive */
		
		BREG_Write32( 
				in_channelHandle->moduleHandle->regHandle, 
				(in_channelHandle->ulRegStartAddr + BSCD_P_UART_CMD_1), 
				ulVal);

		/*
		* Wait until the BCM sent all the data.
		*/
		BSCD_P_CHECK_ERR_CODE_FUNC(errCode, 
				BSCD_Channel_P_WaitForTDone(in_channelHandle));		

	}
	BDBG_MSG(("\n"));

BSCD_P_DONE_LABEL:
	
	BDBG_LEAVE(BSCD_Channel_P_T14IrdetoTransmit);
	return( errCode );
}
#endif

#if 0 /* A582_HAWK */

/*
   OLO 15/03/2006. See Bug #44565 for more information
   
   The function "BSCD_Channel_P_T14IrdetoTransmit" implemented by Broadcom is not efficient.
   It sends one Byte at a time and thus it is very sensitive to context switch.
   This new implementation makes a better use of the Transmit FIFO.
*/

/* For T=14 only */
BERR_Code BSCD_Channel_P_T14IrdetoTransmit(		
		BSCD_ChannelHandle          in_channelHandle,
		uint8_t                     *inp_ucXmitData,
		unsigned long                    in_ulNumXmitBytes
)
{
	BERR_Code errCode = BERR_SUCCESS;
	uint32_t         ulVal; 
	unsigned int          i;
	BSCD_Timer 		timer = {BSCD_TimerType_eGPTimer, {BSCD_GPTimerMode_eIMMEDIATE}, true, true};	
	BSCD_TimerValue    timeValue= {BSCD_T14_IRDETO_MIN_DELAY_RX2TX, BSCD_TimerUnit_eCLK};	

	BDBG_ENTER(BSCD_Channel_P_T14IrdetoTransmit);
	BDBG_ASSERT( in_channelHandle );

	BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, 
		(in_channelHandle->ulMagicNumber != BSCD_P_CHANNEL_HANDLE_MAGIC_NUMBER ) );

	/* BSCD_P_HexDump("Send",inp_ucXmitData, in_ulNumXmitBytes); */

	BKNI_EnterCriticalSection();
	in_channelHandle->ulIntrStatus1 &=
		~BCHP_SCA_SC_INTR_STAT_1_tpar_intr_MASK & 
		~BCHP_SCA_SC_INTR_STAT_1_timer_intr_MASK & 
		~BCHP_SCA_SC_INTR_STAT_1_bgt_intr_MASK & 
		~BCHP_SCA_SC_INTR_STAT_1_tdone_intr_MASK & 
		~BCHP_SCA_SC_INTR_STAT_1_retry_intr_MASK & 
		~BCHP_SCA_SC_INTR_STAT_1_tempty_intr_MASK;
	
	in_channelHandle->ulIntrStatus2 &=
		~BCHP_SCA_SC_INTR_STAT_2_rpar_intr_MASK & 
		~BCHP_SCA_SC_INTR_STAT_2_cwt_intr_MASK & 
		~BCHP_SCA_SC_INTR_STAT_2_rlen_intr_MASK & 
		~BCHP_SCA_SC_INTR_STAT_2_wait_intr_MASK & 
		~BCHP_SCA_SC_INTR_STAT_2_rcv_intr_MASK &
		~BCHP_SCA_SC_INTR_STAT_2_rready_intr_MASK;
	BKNI_LeaveCriticalSection();


	
	/* Reset the Transmit and Receive buffer */
	ulVal =  BCHP_SCA_SC_PROTO_CMD_tbuf_rst_MASK | BCHP_SCA_SC_PROTO_CMD_rbuf_rst_MASK |
		BREG_Read32( 
		in_channelHandle->moduleHandle->regHandle, 
		(in_channelHandle->ulRegStartAddr + BSCD_P_PROTO_CMD)) ;	
	BREG_Write32( 
		in_channelHandle->moduleHandle->regHandle, 
		(in_channelHandle->ulRegStartAddr + BSCD_P_PROTO_CMD), 
		 ulVal);   

	/* Need to wait for minimum of 1250 from last RX to this TX */
	BSCD_P_CHECK_ERR_CODE_FUNC(errCode, 
			BSCD_Channel_ConfigTimer(in_channelHandle, &timer, &timeValue));	

	
	BSCD_P_CHECK_ERR_CODE_FUNC(errCode, 
			BSCD_Channel_P_WaitForTimerEvent(in_channelHandle));	
   
   	/* Disable timer */
	timer.bIsTimerInterruptEnable = false;
	timer.bIsTimerEnable = false;
	BSCD_P_CHECK_ERR_CODE_FUNC(errCode, 
		BSCD_Channel_EnableDisableTimer_isr(in_channelHandle, &timer));	


	/* Enable EDC */	 
	ulVal = BREG_Read32( 
			in_channelHandle->moduleHandle->regHandle, 
			(in_channelHandle->ulRegStartAddr + BSCD_P_PROTO_CMD));
	
	ulVal |= BCHP_SCA_SC_PROTO_CMD_rbuf_rst_MASK;

	BREG_Write32( 
			in_channelHandle->moduleHandle->regHandle, 
			(in_channelHandle->ulRegStartAddr + BSCD_P_PROTO_CMD), 
			ulVal);  


	/* Set flow cmd */
	ulVal = BREG_Read32( 
			in_channelHandle->moduleHandle->regHandle, 
			(in_channelHandle->ulRegStartAddr + BSCD_P_FLOW_CMD));	

	/* No flow control for T=1 protocol or T=14. */
	ulVal &= ~BCHP_SCA_SC_FLOW_CMD_flow_en_MASK;

	BREG_Write32( 
			in_channelHandle->moduleHandle->regHandle, 
			(in_channelHandle->ulRegStartAddr + BSCD_P_FLOW_CMD), 
			ulVal);  



	   
	/*
	* Fill BCM FIFO with the request message.
	*/
	for (i = 0; i < in_ulNumXmitBytes; i++) 
   {

		BREG_Write32( 
			in_channelHandle->moduleHandle->regHandle, 
			(in_channelHandle->ulRegStartAddr + BSCD_P_TRANSMIT), 
			 (uint32_t) inp_ucXmitData[i]);  

		BDBG_MSG(("%02x ", inp_ucXmitData[i]));

		/* Ready to transmit */
		ulVal = BREG_Read32( 
				in_channelHandle->moduleHandle->regHandle, 
				(in_channelHandle->ulRegStartAddr + BSCD_P_UART_CMD_1));	

		/* Always set auto receive */
      /* "xmit_go" will be set only when all the Bytes will be in the FIFO */
		ulVal = BCHP_SCA_SC_UART_CMD_1_t_r_MASK | BCHP_SCA_SC_UART_CMD_1_io_en_MASK ;

  		/* 
  		   See Bug #44564
		   For T=14 parity should be disabled but this is not possible with 7038 hardware
		   So this code is a workaround which ensure that Parity is always equal to '1' 
		*/
		if (BSCD_P_Popcount[inp_ucXmitData[i]] % 2 == 0) 
      {
   		BDBG_MSG(("Even number of one => Parity = 0 => flip the parity \n"));

			ulVal |= BCHP_SCA_SC_UART_CMD_1_inv_par_MASK;
		}
		else 
      {
         BDBG_MSG(("Odd number of one => Parity = 1 => Do not flip the parity \n"));   
			
			ulVal &= ~BCHP_SCA_SC_UART_CMD_1_inv_par_MASK;			
		}

		if (i == (in_ulNumXmitBytes - 1)) 
			ulVal |= BCHP_SCA_SC_UART_CMD_1_auto_rcv_MASK;  /* Last TX byte, ready to receive */
		/*printf("ul val %x \n", ulVal);*/
		BREG_Write32( 
				in_channelHandle->moduleHandle->regHandle, 
				(in_channelHandle->ulRegStartAddr + BSCD_P_UART_CMD_1), 
				ulVal);
	}
	BDBG_MSG(("\n"));


   /* Now that the FIFO is filled we can set the "xmit_go" bit to start the transmission */
	ulVal = BREG_Read32( 
			in_channelHandle->moduleHandle->regHandle, 
			(in_channelHandle->ulRegStartAddr + BSCD_P_UART_CMD_1));	

   ulVal |= BCHP_SCA_SC_UART_CMD_1_xmit_go_MASK;
		
		BREG_Write32( 
				in_channelHandle->moduleHandle->regHandle, 
				(in_channelHandle->ulRegStartAddr + BSCD_P_UART_CMD_1), 
				ulVal);

		/*
		* Wait until the BCM sent all the data.
		*/
		BSCD_P_CHECK_ERR_CODE_FUNC(errCode, 
				BSCD_Channel_P_WaitForTDone(in_channelHandle));		

   BDBG_MSG(("Transmission done"));


BSCD_P_DONE_LABEL:
	
	BDBG_LEAVE(BSCD_Channel_P_T14IrdetoTransmit);
	return( errCode );
}
#endif /* A582_HAWK */



BERR_Code BSCD_Channel_P_EnableInterrupts_isr( 
		BSCD_ChannelHandle	in_channelHandle
)
{
	BERR_Code errCode = BERR_SUCCESS;

	BDBG_ENTER(BSCD_Channel_P_EnableInterrupts_isr);	

	if (in_channelHandle->bIsOpen == true) {
		/* Update BSCD_P_INTR_EN_1 and BSCD_P_INTR_EN_2 */
		if  ((in_channelHandle->currentChannelSettings.eProtocolType == BSCD_AsyncProtocolType_e0 ) &&
			(in_channelHandle->currentChannelSettings.scStandard != BSCD_Standard_eIrdeto))	{

			/* Enable parity error re-transmission only in T=0 */
			BSCD_P_CHECK_ERR_CODE_FUNC(errCode, 
						BSCD_Channel_EnableIntrCallback_isr (
							in_channelHandle, BSCD_IntType_eRetryInt,
				               		BSCD_Channel_P_RetryCB_isr));

			/* Enable RCV_INTR only in T=0 */
			BSCD_P_CHECK_ERR_CODE_FUNC(errCode, 
						BSCD_Channel_EnableIntrCallback_isr (
							in_channelHandle, BSCD_IntType_eRcvInt,
				               		BSCD_Channel_P_RcvCB_isr));

			/* Enable RPAR_INTR only in T=0 */
			BSCD_P_CHECK_ERR_CODE_FUNC(errCode, 
						BSCD_Channel_EnableIntrCallback_isr (
							in_channelHandle, BSCD_IntType_eRParityInt,
				               		BSCD_Channel_P_RParityCB_isr));

			/* Enable TPAR_INTR only in T=0 */
			BSCD_P_CHECK_ERR_CODE_FUNC(errCode, 
						BSCD_Channel_EnableIntrCallback_isr (
							in_channelHandle, BSCD_IntType_eTParityInt,
				               		BSCD_Channel_P_TParityCB_isr));
			
					
		}


		else if (in_channelHandle->currentChannelSettings.eProtocolType == BSCD_AsyncProtocolType_e1 ) {  /* T=1 protocol */

			/* Enable cwt only in T=1 */
#ifdef BSCD_EMV2000_CWT_PLUS_4
			if  ( (in_channelHandle->currentChannelSettings.scStandard != BSCD_Standard_eEMV2000) ||
			  	(in_channelHandle->currentChannelSettings.eProtocolType != BSCD_AsyncProtocolType_e1)) {		  	
#endif			
				BSCD_P_CHECK_ERR_CODE_FUNC(errCode, 
							BSCD_Channel_EnableIntrCallback_isr (
								in_channelHandle, BSCD_IntType_eCWTInt,
					               		BSCD_Channel_P_CWTCB_isr));

#ifdef BSCD_EMV2000_CWT_PLUS_4
			}
#endif


#ifdef BSCD_EMV2000_CWT_PLUS_4_EVENT_INTR
			/* Enable BGT only in T=1 */
			if  (in_channelHandle->currentChannelSettings.scStandard == BSCD_Standard_eEMV2000) {
				BSCD_P_CHECK_ERR_CODE_FUNC(errCode, 
						BSCD_Channel_EnableIntrCallback_isr (
							in_channelHandle, BSCD_IntType_eEvent1Int,
				               		BSCD_Channel_P_Event1CB_isr));
			}

#endif

			/* Enable BGT only in T=1 */
			BSCD_P_CHECK_ERR_CODE_FUNC(errCode, 
						BSCD_Channel_EnableIntrCallback_isr (
							in_channelHandle, BSCD_IntType_eBGTInt,
				               		BSCD_Channel_P_BGTCB_isr));

			/* Enable rlen only in T=1 */
			BSCD_P_CHECK_ERR_CODE_FUNC(errCode, 
						BSCD_Channel_EnableIntrCallback_isr (
							in_channelHandle, BSCD_IntType_eRLenInt,
				               		BSCD_Channel_P_RLenCB_isr));

			/* Enable rreadyonly in T=1 */
			BSCD_P_CHECK_ERR_CODE_FUNC(errCode, 
						BSCD_Channel_EnableIntrCallback_isr (
							in_channelHandle, BSCD_IntType_eRReadyInt,
				               		BSCD_Channel_P_RReadyCB_isr));

			/* set block guard time for T=1 only */
			/* Update the BSCD_P_BGT */   	
			BREG_Write32( 
				in_channelHandle->moduleHandle->regHandle, 
				(in_channelHandle->ulRegStartAddr + BSCD_P_BGT), 
				BCHP_SCA_SC_BGT_r2t_MASK| in_channelHandle->currentChannelSettings.blockGuardTime.ulValue);

/* Enable RCV_INTR only in T=1, EMV 2000 to resolve CWT+4 issue */
#ifdef BSCD_EMV2000_CWT_PLUS_4
			if ( (in_channelHandle->currentChannelSettings.scStandard == BSCD_Standard_eEMV2000) &&
		  		(in_channelHandle->currentChannelSettings.eProtocolType == BSCD_AsyncProtocolType_e1)) {
				
				BSCD_P_CHECK_ERR_CODE_FUNC(errCode, 
						BSCD_Channel_EnableIntrCallback_isr (
							in_channelHandle, BSCD_IntType_eRcvInt,
				               		NULL));								
			}		
#endif			
/* Enable RCV_INTR only in T=1, EMV 2000 to resolve CWT+4 issue */				
		

		}
		else if (in_channelHandle->currentChannelSettings.scStandard ==  BSCD_Standard_eIrdeto) {  /* T=14 Irdeto  protocol */
			/* Enable RCV_INTR only in T=0 */
			BSCD_P_CHECK_ERR_CODE_FUNC(errCode, 
						BSCD_Channel_EnableIntrCallback_isr (
							in_channelHandle, BSCD_IntType_eRcvInt,
				               		BSCD_Channel_P_RcvCB_isr));
		}

		/* Keep the card insertion and removal interrrupt */
		BSCD_P_CHECK_ERR_CODE_FUNC(errCode, 
						BSCD_Channel_EnableIntrCallback_isr (
							in_channelHandle, BSCD_IntType_eCardInsertInt,
				               		BSCD_Channel_P_CardInsertCB_isr));
				
		BSCD_P_CHECK_ERR_CODE_FUNC(errCode, 
						BSCD_Channel_EnableIntrCallback_isr (
							in_channelHandle, BSCD_IntType_eCardRemoveInt,
				               		BSCD_Channel_P_CardRemoveCB_isr));
		
		/* Enable tdone for T=0 and  T=1 */
		BSCD_P_CHECK_ERR_CODE_FUNC(errCode, 
						BSCD_Channel_EnableIntrCallback_isr (
							in_channelHandle, BSCD_IntType_eTDoneInt,
				               		BSCD_Channel_P_TDoneCB_isr));

	}

BSCD_P_DONE_LABEL:
	
	BDBG_LEAVE(BSCD_Channel_P_EnableInterrupts_isr);
	return( errCode );
	
}


BERR_Code BSCD_Channel_P_SetStandard(
		BSCD_ChannelHandle	in_channelHandle,
		const BSCD_ChannelSettings	*inp_sSettings
)
{

	BERR_Code		errCode = BERR_SUCCESS;

	BDBG_ENTER(BSCD_Channel_P_SetStandard);
	
	/* Asynchronous Protocol Types. */
	BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, 
		((inp_sSettings->eProtocolType <= BSCD_AsyncProtocolType_eUnknown)  || 
		(inp_sSettings->eProtocolType > BSCD_AsyncProtocolType_e14_IRDETO)) );	
	switch(in_channelHandle->currentChannelSettings.scStandard) {
		case BSCD_Standard_eNDS:  		/* NDS. T=0 with flow control. */
		case BSCD_Standard_eNDS_NO_FLOWCRTL: /* NDS. T=0 without flow control. */
			BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, 							  
							(inp_sSettings->eProtocolType != BSCD_AsyncProtocolType_e0 ));
			in_channelHandle->currentChannelSettings.eProtocolType = inp_sSettings->eProtocolType;
			break;
			
		case BSCD_Standard_eISO:      		/* ISO 7816. T_0 or T=1*/
			BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, 
							( (inp_sSettings->eProtocolType != BSCD_AsyncProtocolType_e0 ) &&  
							 (inp_sSettings->eProtocolType != BSCD_AsyncProtocolType_e1 )));
			in_channelHandle->currentChannelSettings.eProtocolType = inp_sSettings->eProtocolType;		
			break;
						
		case BSCD_Standard_eEMV1996:  		/* EMV. T=0 or T=1 */
		case BSCD_Standard_eEMV2000:  		/* EMV. T=0 or T=1 */			
			BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, 
							((inp_sSettings->eProtocolType != BSCD_AsyncProtocolType_e0 ) &&  
							 (inp_sSettings->eProtocolType != BSCD_AsyncProtocolType_e1 )));
			in_channelHandle->currentChannelSettings.eProtocolType = inp_sSettings->eProtocolType;				
			break;
						
		case BSCD_Standard_eARIB:		/* ARIB. T=1 */
			BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, 
							( (inp_sSettings->eProtocolType != BSCD_AsyncProtocolType_e0 ) &&  
							 (inp_sSettings->eProtocolType != BSCD_AsyncProtocolType_e1 )));
			in_channelHandle->currentChannelSettings.eProtocolType = inp_sSettings->eProtocolType;				
			break;
						
		case BSCD_Standard_eIrdeto:		/* Irdeto. T=14.  Need Major software workarouond to support this */
			BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, 
							( (inp_sSettings->eProtocolType != BSCD_AsyncProtocolType_e0 ) &&  
							 (inp_sSettings->eProtocolType != BSCD_AsyncProtocolType_e14_IRDETO )));
			in_channelHandle->currentChannelSettings.eProtocolType = inp_sSettings->eProtocolType;				
			break;
			
			
		case BSCD_Standard_eES:             	/* ES, T=1.  Obsolete. Use ISO */
			BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, 
							( (inp_sSettings->eProtocolType != BSCD_AsyncProtocolType_e0 ) &&  
							 (inp_sSettings->eProtocolType != BSCD_AsyncProtocolType_e1 )));
			in_channelHandle->currentChannelSettings.eProtocolType = inp_sSettings->eProtocolType;				
			break;

		case BSCD_Standard_eMT:             	/* MT, T=0.  Obsolete. Use ISO */
			BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, 
							((inp_sSettings->eProtocolType != BSCD_AsyncProtocolType_e0 ) ));
			in_channelHandle->currentChannelSettings.eProtocolType = inp_sSettings->eProtocolType;				
			break;

		case BSCD_Standard_eConax:             	/* Conax, T=0.  Obsolete. Use ISO */
			BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, 
							((inp_sSettings->eProtocolType != BSCD_AsyncProtocolType_e0 ) ));
			in_channelHandle->currentChannelSettings.eProtocolType = inp_sSettings->eProtocolType;				
			break;			
			
		default:
			BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,  true);									
			break;
						
	}
	BDBG_MSG(("eProtocolType = %d", in_channelHandle->currentChannelSettings.eProtocolType));	


BSCD_P_DONE_LABEL:
	
	BDBG_LEAVE(BSCD_Channel_P_SetStandard);
	return( errCode );
}


BERR_Code BSCD_Channel_P_SetFreq(
		BSCD_ChannelHandle	in_channelHandle,
		const BSCD_ChannelSettings	*inp_sSettings
)
{

	BERR_Code		errCode = BERR_SUCCESS;

	BDBG_ENTER(BSCD_Channel_P_SetFreq);

	/* Set F, 	Clock Rate Conversion Factor */														
	if (inp_sSettings->ucFFactor == 0) {
		in_channelHandle->currentChannelSettings.ucFFactor = BSCD_DEFAULT_F;
	}
	else if ((inp_sSettings->ucFFactor >= 1 ) && (inp_sSettings->ucFFactor <= 13)){
		in_channelHandle->currentChannelSettings.ucFFactor = inp_sSettings->ucFFactor;
	}
	else {
		BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, true);
	}
	BDBG_MSG(("ucFFactor = %d", in_channelHandle->currentChannelSettings.ucFFactor));	

	/* Set D, 	Baud Rate Adjustor */
	if (inp_sSettings->ucDFactor == 0) {
		in_channelHandle->currentChannelSettings.ucDFactor = BSCD_DEFAULT_D;
	}	
	if ((inp_sSettings->ucDFactor >= 1 ) && (inp_sSettings->ucDFactor <= 9)) {
		BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, 
				(BSCD_P_GetISOBaudRateAdjustor(inp_sSettings->ucDFactor) ==  ((unsigned char ) -1) ) );
		in_channelHandle->currentChannelSettings.ucDFactor = inp_sSettings->ucDFactor;
	}
	else {
		BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, true);
	}
	BDBG_MSG(("ucDFactor = %d", in_channelHandle->currentChannelSettings.ucDFactor));		

	/* Set ETU Clock Divider */    
	if (inp_sSettings->ucEtuClkDiv == 0 ) {
		in_channelHandle->currentChannelSettings.ucEtuClkDiv = 
				BSCD_P_GetETUClkDiv(in_channelHandle, in_channelHandle->currentChannelSettings.ucDFactor,
									  in_channelHandle->currentChannelSettings.ucFFactor);
	}
	else if ( (inp_sSettings->ucEtuClkDiv == 1) || (inp_sSettings->ucEtuClkDiv == 2) ||
			(inp_sSettings->ucEtuClkDiv == 3) || (inp_sSettings->ucEtuClkDiv == 4) ||
			(inp_sSettings->ucEtuClkDiv == 5) || (inp_sSettings->ucEtuClkDiv == 6) ||
			(inp_sSettings->ucEtuClkDiv == 7) || (inp_sSettings->ucEtuClkDiv == 8)  ) {
		
		in_channelHandle->currentChannelSettings.ucEtuClkDiv = inp_sSettings->ucEtuClkDiv;
	}
	else {
		BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, true);				
	}
	BDBG_MSG(("ucEtuClkDiv = %d", in_channelHandle->currentChannelSettings.ucEtuClkDiv));		
	
	/* Set SC Clock Divider */  
	if (inp_sSettings->ucScClkDiv == 0 ) {
		in_channelHandle->currentChannelSettings.ucScClkDiv = 
						BSCD_P_GetClkDiv(in_channelHandle, in_channelHandle->currentChannelSettings.ucDFactor,
										  in_channelHandle->currentChannelSettings.ucFFactor) ;
	}
	else if ( (inp_sSettings->ucScClkDiv == 1) || (inp_sSettings->ucScClkDiv == 2) ||
			(inp_sSettings->ucScClkDiv == 3) || (inp_sSettings->ucScClkDiv == 4) ||
			(inp_sSettings->ucScClkDiv == 5) || (inp_sSettings->ucScClkDiv == 8) ||
			(inp_sSettings->ucScClkDiv == 10) || (inp_sSettings->ucScClkDiv == 16)  ) { 

		in_channelHandle->currentChannelSettings.ucScClkDiv = inp_sSettings->ucScClkDiv;
	}
	else {
		BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, true);			
	}
	BDBG_MSG(("ucScClkDiv = %d", in_channelHandle->currentChannelSettings.ucScClkDiv));	

	/* Set external Clock Divisor.  For TDA only 1, 2,4,8 are valid value. */  
	if (inp_sSettings->ucExternalClockDivisor == 0 ) {
		in_channelHandle->currentChannelSettings.ucExternalClockDivisor = BSCD_DEFAULT_EXTERNAL_CLOCK_DIVISOR;
	}					
	else {
		in_channelHandle->currentChannelSettings.ucExternalClockDivisor = inp_sSettings->ucExternalClockDivisor;		
	}
	BDBG_MSG(("ucExternalClockDivisor = %d", in_channelHandle->currentChannelSettings.ucExternalClockDivisor));	

	/* Set Prescale */  
	if (inp_sSettings->unPrescale == 0 ) {
		in_channelHandle->currentChannelSettings.unPrescale = 
		                   BSCD_P_GetPrescale(in_channelHandle, in_channelHandle->currentChannelSettings.ucDFactor,
                                              in_channelHandle->currentChannelSettings.ucFFactor) *
			in_channelHandle->currentChannelSettings.ucExternalClockDivisor + 
			(in_channelHandle->currentChannelSettings.ucExternalClockDivisor - 1);
	}					
	else if ( (inp_sSettings->unPrescale <= BSCD_MAX_PRESCALE)) { 
		in_channelHandle->currentChannelSettings.unPrescale = inp_sSettings->unPrescale *
			in_channelHandle->currentChannelSettings.ucExternalClockDivisor + 
			(in_channelHandle->currentChannelSettings.ucExternalClockDivisor - 1);
	}
	else {
		BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, true);			
	}
	BDBG_MSG(("unPrescale = %d", in_channelHandle->currentChannelSettings.unPrescale));
	
	/* Set baud divisor */
	if (inp_sSettings->ucBaudDiv == 0 ) {
		
		in_channelHandle->currentChannelSettings.ucBaudDiv = BSCD_P_GetBaudDiv(in_channelHandle, in_channelHandle->currentChannelSettings.ucDFactor,
		in_channelHandle->currentChannelSettings.ucFFactor);
		
	}					
	else if ( (inp_sSettings->ucBaudDiv == 31) || (inp_sSettings->ucBaudDiv == 32) ||(inp_sSettings->ucBaudDiv == 25) ) {
		
		in_channelHandle->currentChannelSettings.ucBaudDiv = inp_sSettings->ucBaudDiv;
	}
	else {
		BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, true);				
	}
	BDBG_MSG(("ucBaudDiv = %d", in_channelHandle->currentChannelSettings.ucBaudDiv));
	
	/* Set ICC CLK Freq */  	
	in_channelHandle->currentChannelSettings.currentICCClkFreq = 
			in_channelHandle->moduleHandle->currentSettings.moduleClkFreq.ulClkFreq  /
					in_channelHandle->currentChannelSettings.ucScClkDiv /
					in_channelHandle->currentChannelSettings.ucEtuClkDiv/
					in_channelHandle->currentChannelSettings.ucExternalClockDivisor;	
	BDBG_MSG(("currentICCClkFreq = %d", in_channelHandle->currentChannelSettings.currentICCClkFreq));

	in_channelHandle->currentChannelSettings.currentBaudRate =
			in_channelHandle->moduleHandle->currentSettings.moduleClkFreq.ulClkFreq /
					in_channelHandle->currentChannelSettings.ucEtuClkDiv/
					(in_channelHandle->currentChannelSettings.unPrescale+1)/
					in_channelHandle->currentChannelSettings.ucBaudDiv;	
	BDBG_MSG(("currentBaudRate = %d", in_channelHandle->currentChannelSettings.currentBaudRate));	

	if (in_channelHandle->currentChannelSettings.scStandard != BSCD_Standard_eIrdeto) {
		BDBG_MSG(("ISO currentBaudRate = %ld", in_channelHandle->currentChannelSettings.currentICCClkFreq *
			BSCD_P_GetISOBaudRateAdjustor(in_channelHandle->currentChannelSettings.ucDFactor) /
			BSCD_P_GetISOClockRateConversionFactor(in_channelHandle->currentChannelSettings.ucFFactor) ));

		BDBG_MSG(("ISOBaudRateAdjustor = %d", 
			BSCD_P_GetISOBaudRateAdjustor(in_channelHandle->currentChannelSettings.ucDFactor ) ));

		BDBG_MSG(("ISOClockRateConversionFactor = %d",
			BSCD_P_GetISOClockRateConversionFactor(in_channelHandle->currentChannelSettings.ucFFactor) ));

		
		/* If the final ISO baudrate is not equal to the final BRCM baudrate, there is a potential mismatch */
		BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, 
			(in_channelHandle->currentChannelSettings.currentBaudRate != 
			(in_channelHandle->currentChannelSettings.currentICCClkFreq *
			BSCD_P_GetISOBaudRateAdjustor(in_channelHandle->currentChannelSettings.ucDFactor) /
			BSCD_P_GetISOClockRateConversionFactor(in_channelHandle->currentChannelSettings.ucFFactor)) ));

	}
	else {
		/* For T=14 Irdeto */
		BDBG_MSG(("ISO currentBaudRate = %d", in_channelHandle->currentChannelSettings.currentICCClkFreq /
			BSCD_T14_IRDETO_CONSTANT_CLOCK_RATE_CONV_FACTOR) );
		
		/* If the final ISO baudrate is not equal to the final BRCM baudrate, there is a potential mismatch */
#ifndef A582_HAWK /* Temporary(?) workaround to get T=14 card to work */
		BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, 
			(in_channelHandle->currentChannelSettings.currentBaudRate != 
			(in_channelHandle->currentChannelSettings.currentICCClkFreq  /
			BSCD_T14_IRDETO_CONSTANT_CLOCK_RATE_CONV_FACTOR) ));		
#endif         
	}
	
BSCD_P_DONE_LABEL:
	
	BDBG_LEAVE(BSCD_Channel_P_SetFreq);
	return( errCode );
}


BERR_Code BSCD_Channel_P_SetWaitTime(
		BSCD_ChannelHandle	in_channelHandle,
		const BSCD_ChannelSettings	*inp_sSettings
)
{

	BERR_Code		errCode = BERR_SUCCESS;

	BDBG_ENTER(BSCD_Channel_P_SetWaitTime);
	
	/* Set work waiting time */
	if (inp_sSettings->workWaitTime.ulValue == 0) {
		
		BSCD_P_CHECK_ERR_CODE_FUNC(errCode,
					   BSCD_P_AdjustWWT(in_channelHandle, in_channelHandle->currentChannelSettings.ucFFactor, 
					   in_channelHandle->currentChannelSettings.ucDFactor,
							   BSCD_ISO_DEFAULT_WORK_WAIT_TIME_INTEGER));
		
	}
	else {
		switch (inp_sSettings->workWaitTime.unit) {
			case BSCD_TimerUnit_eETU:
				in_channelHandle->currentChannelSettings.workWaitTime.ulValue =  inp_sSettings->workWaitTime.ulValue ;					
				break;
			case BSCD_TimerUnit_eCLK:
				in_channelHandle->currentChannelSettings.workWaitTime.ulValue =  
					inp_sSettings->workWaitTime.ulValue*in_channelHandle->currentChannelSettings.currentBaudRate/
					in_channelHandle->currentChannelSettings.currentICCClkFreq;								
				break;
			case BSCD_TimerUnit_eMilliSec:
				in_channelHandle->currentChannelSettings.workWaitTime.ulValue =  
					inp_sSettings->workWaitTime.ulValue*in_channelHandle->currentChannelSettings.currentBaudRate/1000;
				in_channelHandle->currentChannelSettings.workWaitTime.unit = BSCD_TimerUnit_eETU;

				break;
			default:
				BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, true);
				break;
		}
		in_channelHandle->currentChannelSettings.workWaitTime.unit =  BSCD_TimerUnit_eETU; 					
	}
	BDBG_MSG(("workWaitTime.ulValue in ETU = %d", in_channelHandle->currentChannelSettings.workWaitTime.ulValue));
	BDBG_MSG(("workWaitTime.unit = %d", in_channelHandle->currentChannelSettings.workWaitTime.unit));

	/* Set block Wait time */
	if (inp_sSettings->blockWaitTime.ulValue == 0) {
		in_channelHandle->currentChannelSettings.blockWaitTime.ulValue =  BSCD_DEFAULT_BLOCK_WAITING_TIME ;		
		in_channelHandle->currentChannelSettings.blockWaitTime.unit = BSCD_TimerUnit_eETU;
	}
	else {
		switch (inp_sSettings->blockWaitTime.unit) {
			case BSCD_TimerUnit_eETU:
				in_channelHandle->currentChannelSettings.blockWaitTime.ulValue =  inp_sSettings->blockWaitTime.ulValue ;					
				break;
			case BSCD_TimerUnit_eCLK:
				in_channelHandle->currentChannelSettings.blockWaitTime.ulValue =  
					inp_sSettings->blockWaitTime.ulValue*in_channelHandle->currentChannelSettings.currentBaudRate/
					in_channelHandle->currentChannelSettings.currentICCClkFreq;								
				break;
			case BSCD_TimerUnit_eMilliSec:
				in_channelHandle->currentChannelSettings.blockWaitTime.ulValue =  
					inp_sSettings->blockWaitTime.ulValue*in_channelHandle->currentChannelSettings.currentBaudRate/
					1000;
				break;
			default:
				BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, true);
				break;
		}
		in_channelHandle->currentChannelSettings.blockWaitTime.unit =  BSCD_TimerUnit_eETU; 					
	}
	BDBG_MSG(("blockWaitTime.ulValue in ETU = %d", in_channelHandle->currentChannelSettings.blockWaitTime.ulValue));
	BDBG_MSG(("blockWaitTime.unit = %d", in_channelHandle->currentChannelSettings.blockWaitTime.unit));

	/* Set Character Waiting Time Integer */
	BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, 
				(inp_sSettings->ulCharacterWaitTimeInteger > BSCD_MAX_CHARACTER_WAIT_TIME_INTEGER));
	in_channelHandle->currentChannelSettings.ulCharacterWaitTimeInteger =  inp_sSettings->ulCharacterWaitTimeInteger ;		
	BDBG_MSG(("ulCharacterWaitTimeInteger = %d", in_channelHandle->currentChannelSettings.ulCharacterWaitTimeInteger));

	

BSCD_P_DONE_LABEL:
	
	BDBG_LEAVE(BSCD_Channel_P_SetWaitTime);
	return( errCode );
}
	

BERR_Code BSCD_Channel_P_SetGuardTime(
		BSCD_ChannelHandle	in_channelHandle,
		const BSCD_ChannelSettings	*inp_sSettings
)
{

	BERR_Code		errCode = BERR_SUCCESS;

	BDBG_ENTER(BSCD_Channel_P_SetGuardTime);
	
	/* Set Extra Guard Time  */
	switch (inp_sSettings->extraGuardTime.unit) {
		case BSCD_TimerUnit_eETU:
			in_channelHandle->currentChannelSettings.extraGuardTime.ulValue =  inp_sSettings->extraGuardTime.ulValue ;						
			break;
		case BSCD_TimerUnit_eCLK:
			in_channelHandle->currentChannelSettings.extraGuardTime.ulValue =  
				inp_sSettings->extraGuardTime.ulValue*in_channelHandle->currentChannelSettings.currentBaudRate/
				in_channelHandle->currentChannelSettings.currentICCClkFreq;								
			break;
		case BSCD_TimerUnit_eMilliSec:
			in_channelHandle->currentChannelSettings.extraGuardTime.ulValue =  
				inp_sSettings->extraGuardTime.ulValue*in_channelHandle->currentChannelSettings.currentBaudRate/
				1000;
			break;
		default:
			BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, true);
			break;
	}
	in_channelHandle->currentChannelSettings.extraGuardTime.unit =  BSCD_TimerUnit_eETU; 					
	
	BDBG_MSG(("extraGuardTime.ulValue in ETU = %d", in_channelHandle->currentChannelSettings.extraGuardTime.ulValue));
	BDBG_MSG(("extraGuardTime.unit = %d", in_channelHandle->currentChannelSettings.extraGuardTime.unit));

	/* Set block Guard time */
	if (inp_sSettings->blockGuardTime.ulValue == 0) {
		in_channelHandle->currentChannelSettings.blockGuardTime.ulValue =  BSCD_DEFAULT_BLOCK_GUARD_TIME ;		
		in_channelHandle->currentChannelSettings.blockGuardTime.unit = BSCD_TimerUnit_eETU;
	}
	else {
		switch (inp_sSettings->blockGuardTime.unit) {
			case BSCD_TimerUnit_eETU:
				in_channelHandle->currentChannelSettings.blockGuardTime.ulValue =  inp_sSettings->blockGuardTime.ulValue ;					
				break;
			case BSCD_TimerUnit_eCLK:
				in_channelHandle->currentChannelSettings.blockGuardTime.ulValue =  
					inp_sSettings->blockGuardTime.ulValue*in_channelHandle->currentChannelSettings.currentBaudRate/
					in_channelHandle->currentChannelSettings.currentICCClkFreq;								
				break;
			case BSCD_TimerUnit_eMilliSec:
				in_channelHandle->currentChannelSettings.blockGuardTime.ulValue =  
					inp_sSettings->blockGuardTime.ulValue*in_channelHandle->currentChannelSettings.currentBaudRate/
					1000;
				break;
			default:
				BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, true);
				break;
		}
		in_channelHandle->currentChannelSettings.blockGuardTime.unit =  BSCD_TimerUnit_eETU; 					
	}
	BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, 
			((in_channelHandle->currentChannelSettings.blockGuardTime.ulValue > BSCD_MAX_BLOCK_GUARD_TIME) ||
			(in_channelHandle->currentChannelSettings.blockGuardTime.ulValue < BSCD_MIN_BLOCK_GUARD_TIME)) );	
	BDBG_MSG(("blockGuardTime.ulValue in ETU = %d", in_channelHandle->currentChannelSettings.blockGuardTime.ulValue));
	BDBG_MSG(("blockGuardTime.unit = %d", in_channelHandle->currentChannelSettings.blockGuardTime.unit));
	

BSCD_P_DONE_LABEL:
	
	BDBG_LEAVE(BSCD_Channel_P_SetGuardTime);
	return( errCode );
}

BERR_Code BSCD_Channel_P_SetTransactionTimeout(
		BSCD_ChannelHandle	in_channelHandle,
		const BSCD_ChannelSettings	*inp_sSettings
)
{

	BERR_Code		errCode = BERR_SUCCESS;

	BDBG_ENTER(BSCD_Channel_P_SetTransactionTimeout);
	
	if (inp_sSettings->timeOut.ulValue == 0) {
		in_channelHandle->currentChannelSettings.timeOut.ulValue =  BSCD_DEFAULT_TIME_OUT ;		
		in_channelHandle->currentChannelSettings.timeOut.unit = BSCD_TimerUnit_eMilliSec;
	}
	else {
		switch (inp_sSettings->timeOut.unit) {
			case BSCD_TimerUnit_eETU:
				in_channelHandle->currentChannelSettings.timeOut.ulValue =  inp_sSettings->timeOut.ulValue * 1000000 /
					in_channelHandle->currentChannelSettings.currentBaudRate;
				break;
			case BSCD_TimerUnit_eCLK:
				in_channelHandle->currentChannelSettings.timeOut.ulValue =  
					inp_sSettings->timeOut.ulValue * 1000000 /
					in_channelHandle->currentChannelSettings.currentICCClkFreq ;								
				break;
			case BSCD_TimerUnit_eMilliSec:
				in_channelHandle->currentChannelSettings.timeOut.ulValue =  inp_sSettings->timeOut.ulValue ;		
				break;
			default:
				BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, true);
				break;
		}
		in_channelHandle->currentChannelSettings.timeOut.unit =  BSCD_TimerUnit_eMilliSec; 					
	}
	BDBG_MSG(("timeOut.ulValue in milliseconds = %d", in_channelHandle->currentChannelSettings.timeOut.ulValue));
	BDBG_MSG(("timeOut.unit = %d", in_channelHandle->currentChannelSettings.timeOut.unit));


BSCD_P_DONE_LABEL:
	
	BDBG_LEAVE(BSCD_Channel_P_SetTransactionTimeout);
	return( errCode );
}

BERR_Code BSCD_Channel_P_SetEdcParity(
		BSCD_ChannelHandle	in_channelHandle,
		const BSCD_ChannelSettings	*inp_sSettings
)
{

	BERR_Code		errCode = BERR_SUCCESS;

	BDBG_ENTER(BSCD_Channel_P_SetEdcParity);
	
	/* Set Number of transmit parity retries */
	BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, 
				(inp_sSettings->ucTxRetries > BSCD_MAX_TX_PARITY_RETRIES));
	in_channelHandle->currentChannelSettings.ucTxRetries =  inp_sSettings->ucTxRetries ;		
	BDBG_MSG(("ucTxRetries = %d", in_channelHandle->currentChannelSettings.ucTxRetries));

	/* Set Number of receive parity retries */
	BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, 
				(inp_sSettings->ucRxRetries > BSCD_MAX_TX_PARITY_RETRIES));
	in_channelHandle->currentChannelSettings.ucRxRetries =  inp_sSettings->ucRxRetries ;		
	BDBG_MSG(("ucRxRetries = %d", in_channelHandle->currentChannelSettings.ucRxRetries));

	/* Set EDC encoding */
	in_channelHandle->currentChannelSettings.edcSetting.bIsEnabled =  inp_sSettings->edcSetting.bIsEnabled; 
	in_channelHandle->currentChannelSettings.edcSetting.edcEncode =  inp_sSettings->edcSetting.edcEncode;

	BDBG_MSG(("edcSetting.bIsEnabled = %d", in_channelHandle->currentChannelSettings.edcSetting.bIsEnabled ));
	BDBG_MSG(("edcSetting.edcEncode = %d", in_channelHandle->currentChannelSettings.edcSetting.edcEncode));


BSCD_P_DONE_LABEL:
	
	BDBG_LEAVE(BSCD_Channel_P_SetEdcParity);
	return( errCode );
}


#ifdef BSCD_MEMDEBUG
char BSCD_Channel_P_gDebugBuffer[BSCD_Channel_P_MAX_COUNT][BSCD_Channel_P_MAX_SIZE];
int  BSCD_Channel_P_gCount = 0;
int  BSCD_Channel_P_gbLoop = 0;

void BSCD_Channel_P_StoreInMemory(
      char *inp_format, 
      ...
) 
{  
   va_list arg_pointer;

   va_start(arg_pointer, inp_format);
   vsprintf(BSCD_Channel_P_gDebugBuffer[BSCD_Channel_P_gCount], inp_format, arg_pointer);
   va_end(arg_pointer);
   if (++BSCD_Channel_P_gCount >= BSCD_Channel_P_MAX_COUNT) {
      BSCD_Channel_P_gCount = 0;
      BSCD_Channel_P_gbLoop = 1;
   } 
}

void BSCD_Channel_P_PrintDebug( 
      void
)
{
   unsigned int  i;
   if (BSCD_Channel_P_gbLoop == 0) {
      for (i=0; i< BSCD_Channel_P_gCount; i++) {
         DBGMSG(BSCD_Channel_P_gDebugBuffer[i]);
      }
   }
   else {
      for (i=BSCD_Channel_P_gCount; i< BSCD_Channel_P_MAX_COUNT; i++) {
         DBGMSG(BSCD_Channel_P_gDebugBuffer[i]);
      }
      for (i=0; i< BSCD_Channel_P_gCount; i++) {
         DBGMSG(BSCD_Channel_P_gDebugBuffer[i]);
      }
   }
}
#endif


