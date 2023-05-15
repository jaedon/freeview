/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bscd.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 10/17/12 12:23p $
 *
 * Module Description: This file contains Broadcom smart card (OS/platform
 *                     independent) porting interface public functions that
 *                     can support multiple Broadcom smart card interfaces.
 *                     Since smart card module supports multiple independent
 *                     and identical interfaces/channels, most of the functions
 *                     required a channel handle (BSCD_ChannelHandle),
 *                     instead of module handle (BSCD_Handle).
 *                     This module can support the board with or without
 *                     TDA8004 chip.
 *
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/scd/src/core65nm/bscd.c $
 * 
 * Hydra_Software_Devel/2   10/17/12 12:23p qxie
 * SW7346-1066: 1. BSCD_Channel_ResetCard is calling
 * BSCD_Channel_SetVccLevel without checking the channelHandle-
 * >currentChannelSettings.setVcc boolean 2. disbale flow control in set
 * parameter
 * 
 * Hydra_Software_Devel/1   9/7/12 11:04a qxie
 * SW7429-283:Smartcard: remove symlink in PI code
 * 
 * Hydra_Software_Devel/139   6/6/12 1:12p qxie
 * SW7425-3109:add two more settings from 7425
 * 
 * Hydra_Software_Devel/138   3/21/12 11:40a randyjew
 * SW7468-401: Fix atomic access warning
 * 
 * Hydra_Software_Devel/137   8/16/11 4:46p dlwin
 * SW7346-461: Update source to address coverity:35203 issue, update other
 * functions too.
 * 
 * Hydra_Software_Devel/136   8/16/11 4:37p dlwin
 * SW7346-461: Update source to address coverity:35203 issue.
 * 
 * Hydra_Software_Devel/135   8/15/11 5:15p gmohile
 * SW7420-2034 : Acquire smart card resource only if internal 27MHz clock
 * is not used
 * 
 * Hydra_Software_Devel/134   7/21/11 11:25a cdisc
 * SWDTV-8010: add support for 35233 B0 in BSCD_Open
 * 
 * Hydra_Software_Devel/133   5/24/11 11:54a qxie
 * SW7346-218:Need to add support for the smartcard controller chips
 * TDA803x
 * 
 * Hydra_Software_Devel/132   4/26/11 1:36p cdisc
 * SWDTV-6344: 35233 support
 * 
 * Hydra_Software_Devel/131   3/28/11 4:57p VISHK
 * SW7400-2603: NEXUS Smartcard module needs to add the ability to execute
 * a callback for clock/PLL setup.  SetClockCb is removed from
 * BSCD_Settings as it is redundant. Instead BSCD_ClockFreq.bIsUsingOsc
 * can be used to notify the PI that an external oscillator is being
 * used.
 * 
 * Hydra_Software_Devel/130   3/4/11 12:34p qxie
 * SWDTV-5781: Add SCD in 35230
 * 
 * Hydra_Software_Devel/129   2/24/11 4:52p jtna
 * SW7420-1500: add BCHP_PWR-based power management to SCD and TMN
 * 
 * Hydra_Software_Devel/128   12/15/10 10:33a qxie
 * SW7550-638:Use proper clock updation mechanism before updating clock in
 * bscd.c
 * 
 * Hydra_Software_Devel/127   11/30/10 2:51p qxie
 * SW7125-18:change BREG_WRITE32 to atomicupdate for CLK register
 * 
 * Hydra_Software_Devel/126   11/22/10 4:17p qxie
 * SW7400-261:for PR34810, permantly remove set IF_CMD_1 to 0 in
 * deactivation.
 * 
 * Hydra_Software_Devel/125   11/18/10 1:59p qxie
 * SW7550-522:PowerICC function semantics reversed.
 * 
 * Hydra_Software_Devel/124   9/7/10 9:58a qxie
 * SW7550-522:fix pin mux setting bug for SC1
 * 
 * Hydra_Software_Devel/123   8/18/10 9:52a qxie
 * SW7550-522:Add support slot 1 in 7550
 * 
 * Hydra_Software_Devel/122   7/20/10 10:19a qxie
 * SW7335-805:Smartcard: 24M clock is not set correctly
 * 
 * Hydra_Software_Devel/121   5/25/10 11:42a qxie
 * SW7400-261:change incorrect vcc type
 * 
 * Hydra_Software_Devel/120   5/11/10 11:32a cdisc
 * SW35230-52: for 35230 bringup
 * 
 * Hydra_Software_Devel/119   4/26/10 10:52a qxie
 * SW7125-358:SCD support needed for 7125c0
 * 
 * Hydra_Software_Devel/118   4/23/10 5:50p qxie
 * SW7125-358:SCD support needed for 7125c0
 * 
 * Hydra_Software_Devel/117   4/20/10 3:54p qxie
 * SW7125-358: SCD support needed for 7125c0
 * 
 * Hydra_Software_Devel/116   4/9/10 6:37p qxie
 * SW3548-2878:Smartcard: Add support to external clk
 * 
 * Hydra_Software_Devel/115   3/12/10 1:19a qxie
 * SW7125-18:af pin muxing for UPG_SC0
 * 
 * Hydra_Software_Devel/114   3/9/10 4:12p qxie
 * SW7400-2714:change to use atomicUpdate32(), fix issue at Coverity
 * Defect ID:19385
 * 
 * Hydra_Software_Devel/113   3/3/10 5:23p qxie
 * SW7468-47: add support to 7468, fix external clock set up for 7468
 * 
 * Hydra_Software_Devel/112   1/26/10 10:14p pntruong
 * SW7400-261: Fixed typo.
 *
 * Hydra_Software_Devel/111   1/26/10 3:12p VISHK
 * SW7400-261: The NEXU Smartcard API needs to add support for two new
 * smartcard parameters setPinmuxCb and setVccCb
 *
 * Hydra_Software_Devel/110   1/26/10 11:35a VISHK
 * SW7400-2551: The NEXUS Smartcard module needs to add support for power
 * up/down of the smartcard inteface.
 *
 * Hydra_Software_Devel/109   1/7/10 4:55p qxie
 * SW7550-167:fixed incorrect bauddividor changes
 *
 * Hydra_Software_Devel/108   12/31/09 5:17p qxie
 * SW7125-18:7125A0/B0 smartcard: fixed incorrect external clock source
 * settings
 *
 * Hydra_Software_Devel/107   12/11/09 10:54a qxie
 * SW7468-47: add support to 7468
 *
 * Hydra_Software_Devel/106   12/10/09 10:42p qxie
 * SW7468-47: add support to 7468
 *
 * Hydra_Software_Devel/105   12/10/09 3:09p qxie
 * SW7468-47:Smartcard: Add smartcard support to 7468
 *
 * Hydra_Software_Devel/104   12/9/09 5:43p qxie
 * SW7400-2608:add channel number as argument to channel callback
 *
 * Hydra_Software_Devel/103   12/7/09 10:07p randyjew
 * SW7468-6:Add 7468 support
 *
 * Hydra_Software_Devel/102   12/2/09 6:16p qxie
 * SW7550-96:Smartcard: Add support to 7550
 *
 * Hydra_Software_Devel/101   11/30/09 4:43p qxie
 * SW7325-657:Smartcard: Fix incorrect setting of bauddiv 25
 *
 * Hydra_Software_Devel/100   11/23/09 4:13p qxie
 * SW7125-113:Fix a pin muxing bug in 7125 for slot 1
 *
 * Hydra_Software_Devel/99   11/19/09 4:54p qxie
 * SW7125-113:Smartcard:add support to 7125
 *
 * Hydra_Software_Devel/98   11/17/09 3:07p qxie
 * SW7401-4341:Smartcard: Create event after interrupt enable could cause
 * system crash
 *
 * Hydra_Software_Devel/97   11/12/09 12:00p qxie
 * SW7400-2608:BSCD_Channel_Open  should not call
 * BSCD_Channel_SetVccLevel.
 *
 * Hydra_Software_Devel/96   10/22/09 10:50a qxie
 * SW7340-64:Smartcard: Add pin muxing setting to channel open
 *
 * Hydra_Software_Devel/95   9/17/09 6:44p qxie
 * SW7340-39:Smartcard: Add support to 7340
 *
 * Hydra_Software_Devel/94   9/16/09 6:34p qxie
 * SW7340-39:Smartcard: Add support to 7340
 *
 * Hydra_Software_Devel/93   8/27/09 1:54p qxie
 * SW7125-18: add support for 7125A0 in SCD
 *
 * Hydra_Software_Devel/92   8/10/09 6:56p qxie
 * PR57564: Smartcard: add support to 7340/7342
 *
 * Hydra_Software_Devel/91   8/10/09 5:50p qxie
 * PR57564: Smartcard: add support to 7340/7342
 *
 * Hydra_Software_Devel/90   7/13/09 3:19p qxie
 * PR56770: Smartcard: move BSCD_Channel_P_DumpRegisters to public PI,
 * changed name to BSCD_Channel_DumpRegisters
 *
 * Hydra_Software_Devel/89   6/29/09 3:32p qxie
 * PR55414: Smartcard: card pres default value should be true
 *
 * Hydra_Software_Devel/88   6/16/09 1:32p qxie
 * PR55414: Smartcard: support ext clk routed internally for 7335
 *
 * Hydra_Software_Devel/87   6/12/09 1:40p qxie
 * PR55972: Smartcard:add tda8024 connectivity to channel settings
 *
 * Hydra_Software_Devel/86   6/2/09 5:20p qxie
 * PR55414: Smartcard: support ext clk routed internally
 *
 * Hydra_Software_Devel/85   5/22/09 2:51p qxie
 * PR55414: Smartcard: support ext clk routed internally
 *
 * Hydra_Software_Devel/84   4/29/09 12:10p qxie
 * PR54713: Smartcard: EDC setting not correct; change macro define
 * ECHO_PROPRIETARY_CODE to BSCD_NO_TDA8024
 *
 * Hydra_Software_Devel/83   4/7/09 4:53p qxie
 * PR47804: add atr receive time to channel settings
 *
 * Hydra_Software_Devel/82   4/7/09 11:14a qxie
 * PR47804: add atr receive time to channel settings
 *
 * Hydra_Software_Devel/81   2/18/09 5:09p qxie
 * PR47804: add external smartcard clk to 7420
 *
 * Hydra_Software_Devel/80   1/19/09 11:07a qxie
 * PR48383: BSCD - VCC, CLK and RESET lines are raised in the wrong order
 *
 * Hydra_Software_Devel/79   12/9/08 5:40p btan
 * PR47804: Temporarily removed the external smartcard clock support for
 * 7420
 *
 * Hydra_Software_Devel/78   12/8/08 8:50p qxie
 * PR47804: add smartcard support to 7420
 *
 * Hydra_Software_Devel/77   11/25/08 4:53p qxie
 * PR47804:Add Smart Card PI for 7420
 *
 * Hydra_Software_Devel/76   10/21/08 2:43p qxie
 * PR48045:Coverity Defect ID:12291 UNREACHABLE bscd.c Product=97401linux
 *
 * Hydra_Software_Devel/75   8/28/08 4:32p qxie
 * PR36893:add support to 7335
 *
 * Hydra_Software_Devel/74   7/3/08 4:11p qxie
 * PR44585: BSCD build on 7401 Cx and above is broken with BSCD_ALT_SC_1
 * defined
 *
 * Hydra_Software_Devel/73   5/30/08 1:31p qxie
 * PR39198: add support to 3548
 *
 * Hydra_Software_Devel/72   5/30/08 12:13p qxie
 * PR39198: add support to 3548
 *
 * Hydra_Software_Devel/71   5/6/08 3:16p qxie
 * PR36893:add support to 7335
 *
 * Hydra_Software_Devel/70   5/1/08 5:37p qxie
 * PR36893:add support to 7335, (support slot 1)
 *
 * Hydra_Software_Devel/69   4/28/08 12:04p qxie
 * PR42234: Smartcard: add EMV2000 support under 36M external clock
 *
 * Hydra_Software_Devel/68   4/11/08 3:36p qxie
 * PR36893:add support to 7335, (support A1)
 *
 * Hydra_Software_Devel/67   4/7/08 4:10p farshidf
 * PR39491: add 3556 support
 *
 * Hydra_Software_Devel/66   4/2/08 5:42p qxie
 * PR40493:KLOCWORK: bscd.c , bug in bscd_GetChannel
 *
 * Hydra_Software_Devel/65   3/14/08 3:25p qxie
 * PR40605: Smartcard: add 36M clock support for Nagra certification
 *
 * Hydra_Software_Devel/64   3/4/08 4:01p qxie
 * PR38165: SUNDRY: add smartcard controller to 3546
 *
 * Hydra_Software_Devel/63   2/14/08 10:42a qxie
 * PR39198: Add SCD PI to 3548
 *
 * Hydra_Software_Devel/63   2/14/08 10:39a qxie
 * PR39198: Add SCD PI to 3548
 *
 * Hydra_Software_Devel/63   2/14/08 10:31a qxie
 * PR39198: Add SCD PI to 3548
 *
 * Hydra_Software_Devel/63   2/14/08 10:31a qxie
 * PR39198: Add SCD PI to 3548
 *
 * Hydra_Software_Devel/62   2/13/08 2:29p qxie
 * PR39198:add SCD PI support to 3548
 *
 * Hydra_Software_Devel/61   1/15/08 6:47p qxie
 * PR38739: warning about missing initializer in bscd.c
 *
 * Hydra_Software_Devel/60   1/11/08 4:09p qxie
 * PR35015: adding 7325 suport, add external clock support
 *
 * Hydra_Software_Devel/59   1/11/08 12:08p qxie
 * PR38191: SmartCard presence detection
 *
 * Hydra_Software_Devel/58   1/2/08 5:39p qxie
 * PR27141:Need an API to select smartcard clock source in bscd.c, add
 * call back function for set clock
 *
 * Hydra_Software_Devel/57   12/14/07 1:37p qxie
 * PR35015: adding 7325 suport, change pin muxing value for slot 0
 *
 * Hydra_Software_Devel/56   12/6/07 3:57p qxie
 * PR37984: Smartcard: 7403 Slot 2 doesn't work, modify pin muxing of
 * slot 2
 *
 * Hydra_Software_Devel/55   12/3/07 7:27p qxie
 * PR36893: Smartcard: add support to 7335
 *
 * Hydra_Software_Devel/54   10/31/07 2:02p jrubio
 * PR35015: adding 7325 suport
 *
 * Hydra_Software_Devel/53   9/11/07 5:01p qxie
 * PR 34810:Smartcard: incorrect voltage transition on VCC
 *
 * Hydra_Software_Devel/52   8/21/07 6:04p qxie
 * PR34224: Smartcard: 5 seconds reset for no-responsive card problem:fix
 * bug in channel_reset function
 *
 * Hydra_Software_Devel/51   8/9/07 11:30a qxie
 * PR33872: Smartcard: Add support to smartcard to 7405( add generate 24
 * and 36.864 M external clock from audio Pll1)
 *
 * Hydra_Software_Devel/50   8/8/07 6:19p qxie
 * PR33872: Smartcard: Add support to smartcard(minor fixed on external
 * clock setting)
 *
 * Hydra_Software_Devel/49   8/8/07 5:53p qxie
 * PR33872: Smartcard: Add support to smartcard
 *
 * Hydra_Software_Devel/48   7/26/07 10:44a qxie
 * PR33502: Smartcard: fixed a bug in bscd_channel_open, remove double
 * reset for DSS_ICAM
 *
 * Hydra_Software_Devel/47   7/20/07 3:04p qxie
 * PR33269: Smartcard: change 3/5V control for 7400B0
 *
 * Hydra_Software_Devel/46   5/16/07 5:55p qxie
 * PR26227: smartcard:  add support for 7400 B0, fix code to use
 * smartcard_pll generate external clk for smartcard
 *
 * Hydra_Software_Devel/45   4/13/07 10:25a qxie
 * PR 26227: Add smartcard support to 7400b0
 *
 * Hydra_Software_Devel/44   2/21/07 1:53p qxie
 * PR26227: smartcard:  add support for 7400 B0
 *
 * Hydra_Software_Devel/43   2/20/07 3:14p qxie
 * PR26013:Smartcard: Add smartcard support for 3563A0
 *
 * Hydra_Software_Devel/42   12/1/06 5:50p qxie
 * PR26296: Smartcard:bscd_channel crashed when enter critical
 * section:fixed
 *
 * Hydra_Software_Devel/41   11/29/06 8:14p qxie
 * PR26013: Smartcard:  Add smartcard support for 3563A0, fix a missing =
 * in line 427
 *
 * Hydra_Software_Devel/40   11/29/06 5:02p qxie
 * PR26013: Smartcard:  Add smartcard support for 3563A0
 *
 * Hydra_Software_Devel/39   11/29/06 3:59p qxie
 * PR25689: add 7403 support to SCD
 *
 * Hydra_Software_Devel/38   10/25/06 6:35p qxie
 * PR  24062: Smartcard: add support to 7401 C0
 *
 * Hydra_Software_Devel/37   10/20/06 2:41p qxie
 * PR23810: Smartcard: Add support to 7118(change pin muxing)
 *
 * Hydra_Software_Devel/36   10/20/06 2:08p qxie
 * PR25058:Smartcard: compiler warning in bscd.c
 *
 * Hydra_Software_Devel/35   9/25/06 2:01p qxie
 * PR24526: fix warning message during compile
 *
 * Hydra_Software_Devel/34   9/15/06 1:34p qxie
 * PR 24260: DirecTV P5 smartcard support for warm reset does not work,
 * change reset code
 *
 * Hydra_Software_Devel/33   8/28/06 8:53a jrubio
 * PR23918: fixed compile problem in "#if else" statement
 *
 * Hydra_Software_Devel/32   8/25/06 4:42p qxie
 * PR 23918: Smartcard: GPIO pin change for 3V/5V control for 97398 board
 *
 * Hydra_Software_Devel/31   8/1/06 5:41p qxie
 * PR23162: fixed a warning message in BSCD_Channel_SetVccLevel
 *
 * Hydra_Software_Devel/30   7/6/06 2:16p qxie
 * PR22497:add support to BCM7401 B0
 *
 * Hydra_Software_Devel/29   6/21/06 10:13a jkim
 * PR14344: Add support for B0
 *
 * Hydra_Software_Devel/29   6/21/06 10:02a jkim
 * PR14344: Add support for B0
 *
 * Hydra_Software_Devel/28   4/27/06 2:31p qcheng
 * PR21265: Add 24MHZ external clock support
 *
 * Hydra_Software_Devel/27   4/21/06 4:07p qcheng
 * PR21101: fix compiler warnings
 *
 * Hydra_Software_Devel/26   3/23/06 1:56p qcheng
 * PR19076: Support BCM7400
 *
 * Hydra_Software_Devel/25   3/22/06 10:23a qcheng
 * PR19076: Support BCM7400.
 *
 * Hydra_Software_Devel/24   3/17/06 9:39a qcheng
 * PR20272:  fix a bug in BSCD_Channel_SetParameters()
 *
 * Hydra_Software_Devel/23   12/15/05 2:37p qcheng
 * PR 16586: Added support for 7401
 *
 * Hydra_Software_Devel/22   9/21/05 3:44p qcheng
 * PR 17263:Exit BKNI_WaitForEvent() when card is removed
 *
 * Hydra_Software_Devel/21   8/5/05 7:26p qcheng
 * PR 16586: Added support for 7401
 *
 * Hydra_Software_Devel/20   5/27/05 10:05p btan
 * PR 15680: Added smartcard support for 3560.
 *
 * Hydra_Software_Devel/19   5/18/05 12:55a agin
 * PR14720: B2, C1, C2 compilation support.
 *
 * Hydra_Software_Devel/18   3/16/05 12:08p btan
 * PR 14493: Dont't include bchp_7038.h.
 *
 * Hydra_Software_Devel/17   1/7/05 8:06p btan
 * PR 13681: Do not disable WWT in BSCD_Channel_Receive. Remove GP timer
 * in BSCD_Channel_P_T0ReadData. Need to retest all other standards.
 *
 * Hydra_Software_Devel/16   12/29/04 7:28p btan
 * PR 13700 : To be able to build for C0.
 *
 * Hydra_Software_Devel/15   9/21/04 10:42a btan
 * PR 12728: Fixed warnings caused by comipler option "-pdantic" .
 *
 * Hydra_Software_Devel/14   9/9/04 12:37p btan
 * PR 12504, PR12607:support customized callback functions. Smartcard: T=0
 * protocol takes too long to time out and time out happens after all
 * bytes are received.
 *
 * Hydra_Software_Devel/13   7/29/04 4:49p btan
 * PR 12079: Removed  warning.
 *
 * Hydra_Software_Devel/12   7/20/04 2:11p btan
 * PR 10898: Pass EMV 2000 tests with either event interrupt or WWT to
 * detect CWT +4
 *
 * Hydra_Software_Devel/11   5/26/04 8:46a dlwin
 * PR 11189: Merging to Main Branch
 *
 * Hydra_Software_Devel/Refsw_Devel_7038_B0/2   5/17/04 4:53p btan
 * PR 10898: Added support for 7038 B0.
 *
 * Hydra_Software_Devel/Refsw_Devel_7038_B0/1   4/29/04 5:38p btan
 * PR 10898:Added support for 7038 B0
 *
 * Hydra_Software_Devel/10   12/31/03 3:37p btan
 * PR 9168: Fixed compilation warnings.
 *
 * Hydra_Software_Devel/9   12/31/03 11:05a btan
 * PR 8080: Fixed auto_clk set before sc_clk_en
 *
 * Hydra_Software_Devel/8   12/29/03 3:59p marcusk
 * PR9117: Updated with changes required to support interrupt ids rather
 * than strings.
 *
 * Hydra_Software_Devel/7   12/1/03 5:55p btan
 * Added BSCD_Channel_ResetBlockWaitTimer.
 *
 * Hydra_Software_Devel/6   11/26/03 5:00p btan
 * Fixes for EMV T=0
 *
 * Hydra_Software_Devel/4   11/18/03 5:09p btan
 * Added pin muxing for smartcard interfaces.
 *
 * Hydra_Software_Devel/3   10/28/03 5:03p btan
 * Fixed the bugs after the first round test.
 *
 * Hydra_Software_Devel/2   10/15/03 1:40p btan
 * Supported external clock crystal.
 *
 * Hydra_Software_Devel/1   10/13/03 12:22p btan
 * Initial Creation
 *
 ***************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "bchp_pm.h"
#if(BCHP_CHIP!=7125) ||((BCHP_CHIP==7125)&& (BCHP_VER>=BCHP_VER_C0))
#include "bchp_sca.h"
#endif
#if (BCHP_CHIP==7038) ||(BCHP_CHIP==7438) || (BCHP_CHIP==7400) || (BCHP_CHIP==7401)  ||(BCHP_CHIP == 7118) || (BCHP_CHIP==7403)|| (BCHP_CHIP==7405)|| (BCHP_CHIP==7325) || (BCHP_CHIP==7335) || (BCHP_CHIP==7420) ||(BCHP_CHIP==7340) ||(BCHP_CHIP==7342)||(BCHP_CHIP==7125)||(BCHP_CHIP==7550)||(BCHP_CHIP==35230) ||(BCHP_CHIP==35233)

#include "bchp_scb.h"
#endif
#if ((BCHP_CHIP==7038) || (BCHP_CHIP==7438) ||(BCHP_CHIP==7400))
#include "bchp_scc.h"
#endif
#include "bchp_scirq0.h"
#include "bchp_sun_top_ctrl.h"
#if (BCHP_CHIP==7400) || (BCHP_CHIP==7401)|| (BCHP_CHIP==7118) || (BCHP_CHIP==7403)|| (BCHP_CHIP==7405)|| (BCHP_CHIP==7325) || (BCHP_CHIP==7335)|| (BCHP_CHIP==7468)
#include "bchp_aud_fmm_op_ctrl.h"
#include "bchp_aud_fmm_pll0.h"
#include "bchp_aud_fmm_pll1.h"
#if (BCHP_CHIP == 7405)|| (BCHP_CHIP==7325) || (BCHP_CHIP==7335)||(BCHP_CHIP==7468)
#include "bchp_vcxo_0_rm.h"
#include "bchp_vcxo_ctl_misc.h"
#endif
#if (BCHP_CHIP==7335) || (BCHP_CHIP==7468)
#include "bchp_clk.h"
#endif

#endif
#if (BCHP_CHIP==7400) &&(BCHP_VER >= BCHP_VER_B0)
#include "bchp_smartcard_pll.h"
#include "bchp_vcxo_ctl_misc.h"
#endif

#if(BCHP_CHIP==3548)
#include "bchp_vcxo_0_rm.h"
#include "bchp_vcxo_ctl_misc.h"

#include "bchp_clkgen.h"
#endif

#include "bscd.h"
#include "bscd_priv.h"
#include "bint.h"
#include "bchp_int_id_scirq0.h"
#include "bchp_gio.h"
#if (BCHP_CHIP==7420)||(BCHP_CHIP==7340) ||(BCHP_CHIP==7342)||(BCHP_CHIP==7125)
#include "bchp_smartcard_pll.h"
#if (BCHP_CHIP==7420)||(BCHP_CHIP==7342)
#include "bchp_clk.h"
#else
#include "bchp_clkgen.h"
#endif

#endif
#if (BCHP_CHIP==7420)||(BCHP_CHIP==7340) ||(BCHP_CHIP==7342)||(BCHP_CHIP==7125)||(BCHP_CHIP==7468)
#include "bchp_vcxo_ctl_misc.h"
#endif

#if (BCHP_CHIP ==7550)
#include "bchp_clk_sc.h"
#include "bchp_vcxo_ctl_config_fsm.h"
#endif

#if (BCHP_CHIP==35233)||(BCHP_CHIP==35230)||(BCHP_CHIP==7420)||(BCHP_CHIP==7340) ||(BCHP_CHIP==7342)||(BCHP_CHIP==7550)||(BCHP_CHIP==7468)||((BCHP_CHIP==7125)&& (BCHP_VER>=BCHP_VER_C0))
#define BCHP_SCA_SC_CLK_CMD_clk_en_MASK BCHP_SCA_SC_CLK_CMD_1_clk_en_MASK
#endif

#ifdef BCHP_PWR_SUPPORT
#include "bchp_pwr.h"
#endif

#if 1
#undef BDBG_ENTER
#undef BDBG_LEAVE
#define BDBG_ENTER(x)
#define BDBG_LEAVE(x)
#endif

BDBG_MODULE(BSCD);


/*******************************************************************************
*	Default Module and Channel Settings.  Note that we could only modify
*	Module settings during BSCD_Open.
*******************************************************************************/
static const BSCD_Settings BSCD_defScdSettings =
{
	/* This attribute indicates the source of clock and the value */
	{BSCD_ClockFreqSrc_eInternalClock, 27000000, false, false},

	/* maximum SCD channels supported */
    BSCD_MAX_SUPPOTED_CHANNELS,
    NULL,
};

static const BSCD_ChannelSettings BSCD_defScdChannelSettings =
{
		/* Smart Card Standard */
		BSCD_Standard_eNDS,

		/* Asynchronous Protocol Types. */
		BSCD_AsyncProtocolType_e0,

		/* This read-only attribute specifies the default
			source clock frequency in Hz. */
		BSCD_INTERNAL_CLOCK_FREQ,

		/* ICC CLK frequency in Hz which is
			source freq / SC_CLK_CMD[etu_clkdiv] / SC_CLK_CMD[sc_clkdiv] */
		BSCD_INTERNAL_CLOCK_FREQ/BSCD_DEFAULT_ETU_CLKDIV/BSCD_DEFAULT_SC_CLKDIV,

		/* ETU in microseconds which is source freq / SC_CLK_CMD[etu_clkdiv] */
			/* (SC_PRESCALE * external_clock_div + (external_clock_div - 1))  */
		BSCD_INTERNAL_CLOCK_FREQ/BSCD_DEFAULT_ETU_CLKDIV/(BSCD_DEFAULT_PRESCALE+1)/BSCD_DEFAULT_BAUD_DIV,

		/* This read-only attribute specifies the maximum IFSD.
			Should be 264. */
		BSCD_MAX_TX_SIZE,

		/* This attribute indicates the current IFSD */
		BSCD_DEFAULT_EMV_INFORMATION_FIELD_SIZE,

		/* Clock Rate Conversion Factor,
			F in 1,2,3,4,5,6,9, 10, 11, 12 or 13.
			Default is 1. */
		BSCD_DEFAULT_F,

		/* Baud Rate Adjustment Factor,
			D in 1,2,3,4,5,6,8 or 9.
			Default is 1. */
		BSCD_DEFAULT_D,

		/* 	ETU Clock Divider in
			SC_CLK_CMD register. Valid value is
			from 1 to 8. Default is 6. */
		BSCD_DEFAULT_ETU_CLKDIV,

		/* 	SC Clock Divider in
			SC_CLK_CMD register. Valid value is
			1,2,3,4,5,8,10,16. Default is 1. */
		BSCD_DEFAULT_SC_CLKDIV,

		/* Prescale Value */
		BSCD_DEFAULT_PRESCALE,

		/* external clock divisor */
		BSCD_DEFAULT_EXTERNAL_CLOCK_DIVISOR,

		/* Baud Divisor	*/
		BSCD_DEFAULT_BAUD_DIV,

		/* Number of transmit parity retries per character in
			SC_UART_CMD_2 register.	Default is 4 and max is 6.
			7 indicates infinite retries */
		BSCD_DEFAULT_TX_PARITY_RETRIES,

		/* Number of receive parity retries per character in
			SC_UART_CMD_2 register. Default is 4 and max is 6.
			7 indicates infinite retries */
		BSCD_DEFAULT_RX_PARITY_RETRIES,

		/* work waiting time in SC_TIME_CMD register. Other than EMV
			standard, only valid if current protocol is T=0. */
		{BSCD_DEFAULT_WORK_WAITING_TIME,   BSCD_TimerUnit_eETU},

		/* block Wait time in SC_TIME_CMD register. Only valid if
			current protocol is T=1. */
		{BSCD_DEFAULT_BLOCK_WAITING_TIME,   BSCD_TimerUnit_eETU},

		/* Extra Guard Time in SC_TGUARD register. */
		{BSCD_DEFAULT_EXTRA_GUARD_TIME,   BSCD_TimerUnit_eETU},

		/*  block Guard time in SC_BGT register.Other than EMV
			standard, only valid if current protocol is T=1.  */
		{BSCD_DEFAULT_BLOCK_GUARD_TIME,   BSCD_TimerUnit_eETU},

		/* character Wait time in SC_PROTO_CMD register. Only valid
			if current protocol is T=1. */
		BSCD_DEFAULT_CHARACTER_WAIT_TIME_INTEGER,

		/* EDC encoding. Only valid if current protocol is T=1. */
		{BSCD_EDCEncode_eLRC,   false},

		/* arbitrary Time Out value for any synchronous transaction. */
		{BSCD_DEFAULT_TIME_OUT,   BSCD_TimerUnit_eMilliSec},

		/* Specify if we need auto deactivation sequence */
		false,

		/* True if we receive 0x60 in T=0, we will ignore it.  Otherwise, we treat 0x60 as a valid data byte */
		false,

		/* Debounce info for IF_CMD_2 */
		{BSCD_ScPresMode_eMask, true, BSCD_DEFAULT_DB_WIDTH},

		BSCD_ResetCardAction_eReceiveAndDecode,   /* Specify if we want the driver to read, decode and program registers */

		{0,   BSCD_TimerUnit_eETU} , /* block wait time extension */
		true  /* pres is low */		,
		{BSCD_MAX_ATR_START_IN_CLK_CYCLES, BSCD_TimerUnit_eCLK},
		false  , /* connect to TDA8024 */
		true,   /* pin setting */
		true,   /* VCC setting */
		BSCD_VccLevel_e5V  ,     /* 5 Volts */
                 BSCD_ClockFreq_e27MHZ,
                 BSCD_MAX_RESET_IN_CLK_CYCLES,
                true,
                false
};




/*******************************************************************************
*	Public Module Functions
*******************************************************************************/
BERR_Code BSCD_GetDefaultSettings(
		BSCD_Settings	*outp_sSettings,
		BCHP_Handle		in_chipHandle
)
{
	BERR_Code errCode = BERR_SUCCESS;

	BDBG_ENTER(BSCD_GetDefaultSettings);
	BDBG_ASSERT( in_chipHandle );

	*outp_sSettings = BSCD_defScdSettings;

	BSTD_UNUSED(in_chipHandle);

	BDBG_LEAVE(BSCD_GetDefaultSettings);
	return( errCode );
}


BERR_Code BSCD_Open(
		BSCD_Handle			*outp_handle,
		BREG_Handle			in_regHandle,
		BCHP_Handle			in_chipHandle,
	      	BINT_Handle			in_interruptHandle,
		const BSCD_Settings	*inp_sSettings
)
{
	BERR_Code errCode = BERR_SUCCESS;
 	BSCD_Handle moduleHandle;
	unsigned int channelNum;
	uint32_t ulVal = 0;

	BDBG_ENTER(BSCD_Open);
	BDBG_ASSERT( in_chipHandle );
	BDBG_ASSERT( in_regHandle );
	BDBG_ASSERT( in_interruptHandle );

	*outp_handle = NULL;

	/* on many chips, ulVal is actually used, but having an unconditional BSTD_UNUSED makes it easier to maintain this code. */
	BSTD_UNUSED(ulVal);

	/* Alloc memory from the system heap */
	if ((moduleHandle =
				(BSCD_Handle) BKNI_Malloc( sizeof( BSCD_P_Handle)))
				== NULL) {
		/* wrap initially detected error code */
		errCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
		goto BSCD_P_DONE_LABEL;
	}

	BKNI_Memset(moduleHandle, 0, sizeof( BSCD_P_Handle ));

	moduleHandle->ulMagicNumber = BSCD_P_HANDLE_MAGIC_NUMBER;
	moduleHandle->chipHandle = in_chipHandle;
	moduleHandle->regHandle = in_regHandle;
	moduleHandle->interruptHandle = in_interruptHandle;

	if (inp_sSettings == NULL)
		moduleHandle->currentSettings = BSCD_defScdSettings;
	else {
		moduleHandle->currentSettings = *inp_sSettings;

	}

	/* Get the chip information for Bcm7038 */
	BCHP_GetChipInfo( moduleHandle->chipHandle, &moduleHandle->chipId, &moduleHandle->chipRev );
	BDBG_MSG(( "Chip Information for Bcm7038\n" ));
	BDBG_MSG(( "chipId=%d, chipRev=%d\n", moduleHandle->chipId, moduleHandle->chipRev ));
	if(!moduleHandle->currentSettings.moduleClkFreq.bIsUsingOsc){
	/* Set ICC CLK Freq */
	if ((moduleHandle->currentSettings.moduleClkFreq.FreqSrc == BSCD_ClockFreqSrc_eInternalClock) ||
		(moduleHandle->currentSettings.moduleClkFreq.FreqSrc == BSCD_ClockFreqSrc_eUnknown) ) {
		moduleHandle->currentSettings.moduleClkFreq.FreqSrc = BSCD_ClockFreqSrc_eInternalClock;
#if(BCHP_CHIP==7550)
		if(moduleHandle->currentSettings.moduleClkFreq.ulClkFreq != 27000000 ){
                uint32_t ulCurrCfg;

				ulVal = BREG_Read32 (moduleHandle->regHandle, BCHP_CLK_SC_SC_SEL);
				ulVal &= ~(BCHP_CLK_SC_SC_SEL_SC0_27_MASK |BCHP_CLK_SC_SC_SEL_SC1_27_MASK   );

				BREG_Write32 (moduleHandle->regHandle, BCHP_CLK_SC_SC_SEL, ulVal );

                ulVal = BREG_Read32(moduleHandle->regHandle, BCHP_VCXO_CTL_CONFIG_FSM_PLL_CURR_CFG_3A);
                ulCurrCfg = BCHP_GET_FIELD_DATA(ulVal, VCXO_CTL_CONFIG_FSM_PLL_CURR_CFG_3A, en_reg_ctrl_cfg_3);

                if (ulCurrCfg) {
    				ulVal = BREG_Read32 (moduleHandle->regHandle, BCHP_VCXO_CTL_CONFIG_FSM_PLL_CURR_CFG_3B);
    				ulVal &= ~( BCHP_VCXO_CTL_CONFIG_FSM_PLL_CURR_CFG_3B_m6div_MASK   );
    				if(moduleHandle->currentSettings.moduleClkFreq.ulClkFreq == 24000000)
    					ulVal |= 0x36<<BCHP_VCXO_CTL_CONFIG_FSM_PLL_CURR_CFG_3B_m6div_SHIFT;
    				else if(moduleHandle->currentSettings.moduleClkFreq.ulClkFreq == 36000000)
    					ulVal |= 0x24<<BCHP_VCXO_CTL_CONFIG_FSM_PLL_CURR_CFG_3B_m6div_SHIFT;
    				else {
    					BDBG_WRN(("Frequency %d is not supported\n",moduleHandle->currentSettings.moduleClkFreq.ulClkFreq  ));
    				}
    				BREG_Write32 (moduleHandle->regHandle,BCHP_VCXO_CTL_CONFIG_FSM_PLL_CURR_CFG_3B, ulVal);
                }
                else {
				ulVal = BREG_Read32 (moduleHandle->regHandle, BCHP_VCXO_CTL_CONFIG_FSM_PLL_NEXT_CFG_3B);
				ulVal &= ~( BCHP_VCXO_CTL_CONFIG_FSM_PLL_NEXT_CFG_3B_m6div_MASK   );
				if(moduleHandle->currentSettings.moduleClkFreq.ulClkFreq == 24000000)
					ulVal |= 0x36<<BCHP_VCXO_CTL_CONFIG_FSM_PLL_NEXT_CFG_3B_m6div_SHIFT;
				else if(moduleHandle->currentSettings.moduleClkFreq.ulClkFreq == 36000000)
					ulVal |= 0x24<<BCHP_VCXO_CTL_CONFIG_FSM_PLL_NEXT_CFG_3B_m6div_SHIFT;
				else {
					BDBG_WRN(("Frequency %d is not supported\n",moduleHandle->currentSettings.moduleClkFreq.ulClkFreq  ));
				}
				BREG_Write32 (moduleHandle->regHandle,BCHP_VCXO_CTL_CONFIG_FSM_PLL_NEXT_CFG_3B, ulVal);
				ulVal = BREG_Read32 (moduleHandle->regHandle, BCHP_VCXO_CTL_CONFIG_FSM_PLL_NEXT_CFG_3A);
				ulVal &= ~( BCHP_VCXO_CTL_CONFIG_FSM_PLL_NEXT_CFG_3A_en_reg_ctrl_cfg_3_MASK   );
				BREG_Write32 (moduleHandle->regHandle,BCHP_VCXO_CTL_CONFIG_FSM_PLL_NEXT_CFG_3A, ulVal|1<<BCHP_VCXO_CTL_CONFIG_FSM_PLL_NEXT_CFG_3A_en_reg_ctrl_cfg_3_SHIFT );
				ulVal = BREG_Read32 (moduleHandle->regHandle, BCHP_VCXO_CTL_CONFIG_FSM_PLL_UPDATE);
				BREG_Write32 (moduleHandle->regHandle,BCHP_VCXO_CTL_CONFIG_FSM_PLL_UPDATE,
					ulVal|1<<BCHP_VCXO_CTL_CONFIG_FSM_PLL_UPDATE_update_SHIFT);
		}
		}

#else
		moduleHandle->currentSettings.moduleClkFreq.ulClkFreq = BSCD_INTERNAL_CLOCK_FREQ;
#endif
              /*04/27/2006 Qiang Xie clear external clock mask */
		/* check whether 7038 and other platforms have BCHP_PM_CONFIG_sc_ext_clk_sel bit */
#if (BCHP_CHIP==7420)  ||(BCHP_CHIP==7342)
/*
 ulVal = BREG_Read32 (moduleHandle->regHandle,
					BCHP_CLK_MISC);
			ulVal &= ~(BCHP_CLK_MISC_SMARTCARD_PLL_REFERENCE_CLK_SEL_MASK );*/
			BREG_AtomicUpdate32 (moduleHandle->regHandle, BCHP_CLK_MISC,BCHP_CLK_MISC_SMARTCARD_PLL_REFERENCE_CLK_SEL_MASK,
				ulVal |(2<<BCHP_CLK_MISC_SMARTCARD_PLL_REFERENCE_CLK_SEL_SHIFT));
#elif (BCHP_CHIP==7125)
/*
	 ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_CLKGEN_SMARTCARD_CLOCK_CTRL);
	 ulVal &= ~(BCHP_CLKGEN_SMARTCARD_CLOCK_CTRL_SMARTCARD_PLL_REFERENCE_CLK_SEL_MASK );
*/
	 BREG_AtomicUpdate32 (moduleHandle->regHandle, BCHP_CLKGEN_SMARTCARD_CLOCK_CTRL,
	 BCHP_CLKGEN_SMARTCARD_CLOCK_CTRL_SMARTCARD_PLL_REFERENCE_CLK_SEL_MASK, ulVal);
#elif (BCHP_CHIP==7468)

	BREG_AtomicUpdate32(moduleHandle->regHandle,BCHP_CLK_MISC,BCHP_CLK_MISC_SMARTCARD_CLOCK_0_SOURCE_SEL_MASK,
					2<<BCHP_CLK_MISC_SMARTCARD_CLOCK_0_SOURCE_SEL_SHIFT);


#endif
#if ((BCHP_CHIP==7401) ||( BCHP_CHIP == 7118) || (BCHP_CHIP==7403 ) || (BCHP_CHIP==3563 )|| (BCHP_CHIP==7405 )|| (BCHP_CHIP==7325) || (BCHP_CHIP==7335) )

		BREG_Write32 (moduleHandle->regHandle, BCHP_PM_CONFIG,
			(BREG_Read32 (moduleHandle->regHandle,
				BCHP_PM_CONFIG)&(~BCHP_PM_CONFIG_sc_ext_clk_sel_MASK)));
#endif
	}
	else if ( (moduleHandle->currentSettings.moduleClkFreq.FreqSrc > BSCD_ClockFreqSrc_eExternalClock) ||
		  ((moduleHandle->currentSettings.moduleClkFreq.FreqSrc == BSCD_ClockFreqSrc_eExternalClock) &&
		  (moduleHandle->currentSettings.moduleClkFreq.ulClkFreq == 0) )  ) {
		BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, true);
	}
	else if ((moduleHandle->currentSettings.moduleClkFreq.FreqSrc == BSCD_ClockFreqSrc_eExternalClock) &&
		  (moduleHandle->currentSettings.moduleClkFreq.ulClkFreq != 0) ) {

/* Need to program this to get External Clock:
PM_CONFIG - PERIPHERAL MODULE CONFIGURATION REGISTER
Address Offset = 32'h0040_013c
Physical Address = 32'h1040_013c
Verilog Macro Address = `PM_REG_START + `PM_CONFIG
Reset Value = 32'h0000_0000
Access = RW (32-bit only)

06 sc_ext_clk_sel 0: Internal 27 MHz is used for smart card clocks.
1: External clock at SC_EXT_CLK pin is used for smart card clocks. The minimum frequency for smart card external clock is 18.4 MHz; the maximum is 40.0 MHz.
Reset value is 0x0.

*/

#if (BCHP_CHIP==7038 || BCHP_CHIP==7438 || BCHP_CHIP == 3563 || BCHP_CHIP==3546 || BCHP_CHIP==3556)

		BREG_Write32 (moduleHandle->regHandle, BCHP_PM_CONFIG,
			(BREG_Read32 (moduleHandle->regHandle,
				BCHP_PM_CONFIG) | BCHP_PM_CONFIG_sc_ext_clk_sel_MASK));
#if 0
	#if ((BCHP_CHIP==3548) || (BCHP_CHIP==3556))
	      #if (BCHP_VER < BCHP_VER_B0)
				ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5);
				ulVal &= ~BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_5, gpio_29);
				ulVal |= BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_5, gpio_29, 2);

				BREG_Write32 (moduleHandle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5, ulVal);
		#else
				ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6);
				ulVal &= ~BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_29);
				ulVal |= BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_6, gpio_29, 2);

				BREG_Write32 (moduleHandle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6, ulVal);
		 #endif
		#endif
#endif
		BDBG_MSG(("**** External clock ****\n"));
#elif (BCHP_CHIP == 3548) /* 3548 audio clock can be routed to sc internally */
	BREG_Write32 (moduleHandle->regHandle, BCHP_CLKGEN_GEN_CTRL, 1) ; /* use audio 1 as output */ 
	BREG_Write32 (moduleHandle->regHandle, BCHP_VCXO_0_RM_CONTROL , 0x228) ; 
	ulVal = BREG_Read32 (moduleHandle->regHandle, BCHP_VCXO_CTL_MISC_VC0_CTRL);
	ulVal &= ~BCHP_VCXO_CTL_MISC_VC0_CTRL_DRESET_MASK  ;	
	BREG_Write32 (moduleHandle->regHandle,BCHP_VCXO_CTL_MISC_VC0_CTRL, ulVal);

	ulVal = BREG_Read32 (moduleHandle->regHandle, BCHP_VCXO_CTL_MISC_AC1_CONTROL);
	ulVal &= ~BCHP_VCXO_CTL_MISC_AC1_CONTROL_REFERENCE_SELECT_MASK ;
	BREG_Write32 (moduleHandle->regHandle,BCHP_VCXO_CTL_MISC_AC1_CONTROL, ulVal);

	ulVal = BREG_Read32 (moduleHandle->regHandle, BCHP_VCXO_CTL_MISC_AC1_MACRO);
	ulVal &= ~BCHP_VCXO_CTL_MISC_AC1_MACRO_MACRO_SELECT_MASK  ;
	BREG_Write32 (moduleHandle->regHandle,BCHP_VCXO_CTL_MISC_AC1_MACRO, 
		ulVal|BCHP_VCXO_CTL_MISC_AC1_MACRO_MACRO_SELECT_User       );

	ulVal = BREG_Read32 (moduleHandle->regHandle, BCHP_VCXO_CTL_MISC_AC1_USER_DIV1);
	ulVal &= ~BCHP_VCXO_CTL_MISC_AC1_USER_DIV1_M1DIV_MASK       ;
	BREG_Write32 (moduleHandle->regHandle,BCHP_VCXO_CTL_MISC_AC1_USER_DIV1, 
		ulVal|(0x30<< BCHP_VCXO_CTL_MISC_AC1_USER_DIV1_M1DIV_SHIFT  )       );

	ulVal = BREG_Read32 (moduleHandle->regHandle, BCHP_VCXO_CTL_MISC_AC1_USER_DIV2);
	ulVal &= ~BCHP_VCXO_CTL_MISC_AC1_USER_DIV2_NDIV_INT_MASK       ;
	BREG_Write32 (moduleHandle->regHandle,BCHP_VCXO_CTL_MISC_AC1_USER_DIV2, 
		ulVal|(0x32<< BCHP_VCXO_CTL_MISC_AC1_USER_DIV2_NDIV_INT_SHIFT )       );

	ulVal = BREG_Read32 (moduleHandle->regHandle, BCHP_VCXO_CTL_MISC_AC1_CONTROL);
	ulVal &= ~BCHP_VCXO_CTL_MISC_AC1_CONTROL_USER_UPDATE_DIVIDERS_MASK   ;
	BREG_Write32 (moduleHandle->regHandle,BCHP_VCXO_CTL_MISC_AC1_CONTROL, ulVal|
		(BCHP_VCXO_CTL_MISC_AC1_CONTROL_USER_UPDATE_DIVIDERS_Update_now
		<< BCHP_VCXO_CTL_MISC_AC1_CONTROL_USER_UPDATE_DIVIDERS_SHIFT  ));
BDBG_MSG(("**** External clock ****\n"));

#elif (BCHP_CHIP==7400)
#if (BCHP_VER < BCHP_VER_B0)
		/* Configure  GPIO_99 Pinmux = sc_clk_out */
		ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12);

		ulVal &= ~BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_099);
		ulVal |= BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_099, 3);

		BREG_Write32 (moduleHandle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12, ulVal);

		/* Switch to use external clock in Smart Card Block */
		BREG_Write32 (moduleHandle->regHandle, BCHP_PM_CONFIG,
				(BREG_Read32 (moduleHandle->regHandle, BCHP_PM_CONFIG) |
				BCHP_PM_CONFIG_sc_ext_clk_sel_MASK));


		/* Configure  sc_ext_clk pinmux = sc_ext_clk */
		ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12);

		ulVal &= ~BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_100);
		ulVal |= BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_100, 1);

		BREG_Write32 (moduleHandle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12, ulVal );

#else
		/* Configure  GPIO_99 Pinmux = sc_clk_out */
		ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4);

		ulVal &= ~BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_4, gpio_019);
		ulVal |= BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_4, gpio_019, 3);

		BREG_Write32 (moduleHandle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4, ulVal);


		/* Switch to use external clock in Smart Card Block */
		BREG_Write32 (moduleHandle->regHandle, BCHP_PM_CONFIG,
				(BREG_Read32 (moduleHandle->regHandle, BCHP_PM_CONFIG) |
				BCHP_PM_CONFIG_sc_ext_clk_sel_MASK));

#if 0
		/* Configure  sc_ext_clk pinmux = sc_ext_clk */
		ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12);

		ulVal &= ~BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_103);
		ulVal |= BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_103, 1);

		BREG_Write32 (moduleHandle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12, ulVal );
#endif

		/* Configure smartcard_pll */
		/*Set PPL  mode */
		ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_SMARTCARD_PLL_MACRO );

		ulVal &= ~BCHP_SMARTCARD_PLL_MACRO_MACRO_SELECT_MASK;

		if(moduleHandle->currentSettings.moduleClkFreq.ulClkFreq ==24000000){
			BREG_Write32 (moduleHandle->regHandle, BCHP_SMARTCARD_PLL_MACRO,
			(ulVal |  BCHP_SMARTCARD_PLL_MACRO_MACRO_SELECT_freq_24MHz   ));
              }
		else{
                     /*04/26/2006 Qiang Xie */
			/*external clock is 36.894MHZ*/
			BREG_Write32 (moduleHandle->regHandle, BCHP_SMARTCARD_PLL_MACRO,
			(ulVal |  BCHP_SMARTCARD_PLL_MACRO_MACRO_SELECT_freq_36p864MHz    ));
		}

		ulVal = BREG_Read32 (moduleHandle->regHandle, BCHP_SMARTCARD_PLL_CONTROL);
		/* choose VCX0 reference clock */
		ulVal &=~BCHP_SMARTCARD_PLL_CONTROL_REFERENCE_SELECT_MASK;
		ulVal |=BCHP_SMARTCARD_PLL_CONTROL_USER_UPDATE_DIVIDERS_MASK;
		/* Enable PLL parameter update */
		BREG_Write32 (moduleHandle->regHandle, BCHP_SMARTCARD_PLL_CONTROL,  ulVal);


#endif
		BDBG_MSG(("**** External clock ****\n"));
#elif (BCHP_CHIP==7401 || BCHP_CHIP == 7118 || BCHP_CHIP==7403  || BCHP_CHIP==7405 )|| (BCHP_CHIP==7325) || (BCHP_CHIP==7335) ||(BCHP_CHIP==7468)
		/*Program AUD_FMM_PLL1 */
		/*Select AUD_FMM_PLL1 Channel 2 */
	if(moduleHandle->currentSettings.moduleClkFreq.bIsUsingOsc == false){
		#if (BCHP_CHIP==7468)
	 	ulVal = BREG_Read32 (moduleHandle->regHandle,  
					BCHP_CLK_MISC);
		ulVal &= ~(BCHP_CLK_MISC_SMARTCARD_CLOCK_0_SOURCE_SEL_MASK ); 
		BREG_Write32 (moduleHandle->regHandle,BCHP_CLK_MISC, 
				ulVal|1<<BCHP_CLK_MISC_SMARTCARD_CLOCK_0_SOURCE_SEL_SHIFT);
		#endif
		
		ulVal = BREG_Read32 (moduleHandle->regHandle,
		(BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_EXTi_ARRAY_BASE + 1 * BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_EXTi_ARRAY_ELEMENT_SIZE /8 ));

		ulVal &= ~(BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_EXTi_PLLCLKSEL_MASK );
		#if(BCHP_CHIP==7325)
		BREG_Write32 (moduleHandle->regHandle,
			(BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_EXTi_ARRAY_BASE + 0 * BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_EXTi_ARRAY_ELEMENT_SIZE /8 ),
			(ulVal |(BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_EXTi_PLLCLKSEL_PLL1_ch1
					<< BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_EXTi_PLLCLKSEL_SHIFT ) ));
		#else
		#if(BCHP_CHIP==7335)
		if(moduleHandle->currentSettings.moduleClkFreq.bIsRoutedInternal == false){
			BREG_Write32 (moduleHandle->regHandle,
						(BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_EXTi_ARRAY_BASE + 1 * BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_EXTi_ARRAY_ELEMENT_SIZE /8 ),
						(ulVal |(BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_EXTi_PLLCLKSEL_PLL1_ch2
								<< BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_EXTi_PLLCLKSEL_SHIFT ) ));
		}else{
			BREG_Write32 (moduleHandle->regHandle,
						(BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_EXTi_ARRAY_BASE + 0 * BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_EXTi_ARRAY_ELEMENT_SIZE /8 ),
						(ulVal |(BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_EXTi_PLLCLKSEL_PLL0_ch3
								<< BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_EXTi_PLLCLKSEL_SHIFT ) ));
		}
		#else
		BREG_Write32 (moduleHandle->regHandle,
			(BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_EXTi_ARRAY_BASE + 1 * BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_EXTi_ARRAY_ELEMENT_SIZE /8 ),
			(ulVal |(BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_EXTi_PLLCLKSEL_PLL1_ch2
					<< BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_EXTi_PLLCLKSEL_SHIFT ) ));
		#endif
		#endif
		/*Set PPL1 to user mode */
		#if(BCHP_CHIP==7335)
		if(moduleHandle->currentSettings.moduleClkFreq.bIsRoutedInternal == false){
		ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_AUD_FMM_PLL1_MACRO );
			ulVal &= ~BCHP_AUD_FMM_PLL0_MACRO_MACRO_SELECT_MASK;
			BREG_Write32 (moduleHandle->regHandle, BCHP_AUD_FMM_PLL1_MACRO,
				(ulVal |( BCHP_AUD_FMM_PLL0_MACRO_MACRO_SELECT_User
					<< BCHP_AUD_FMM_PLL0_MACRO_MACRO_SELECT_SHIFT ) ));

			/* Enable PLL parameter update */
			BREG_Write32 (moduleHandle->regHandle, BCHP_AUD_FMM_PLL1_CONTROL,
				(BREG_Read32 (moduleHandle->regHandle, BCHP_AUD_FMM_PLL1_CONTROL) |
				BCHP_AUD_FMM_PLL0_CONTROL_USER_UPDATE_DIVIDERS_MASK));
		}else{
			ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_AUD_FMM_PLL0_MACRO );
		ulVal &= ~BCHP_AUD_FMM_PLL0_MACRO_MACRO_SELECT_MASK;
			BREG_Write32 (moduleHandle->regHandle, BCHP_AUD_FMM_PLL0_MACRO,
				(ulVal |( BCHP_AUD_FMM_PLL0_MACRO_MACRO_SELECT_User
					<< BCHP_AUD_FMM_PLL0_MACRO_MACRO_SELECT_SHIFT ) ));

			/* Enable PLL parameter update */
			BREG_Write32 (moduleHandle->regHandle, BCHP_AUD_FMM_PLL0_CONTROL,
				(BREG_Read32 (moduleHandle->regHandle, BCHP_AUD_FMM_PLL1_CONTROL) |
				BCHP_AUD_FMM_PLL0_CONTROL_USER_UPDATE_DIVIDERS_MASK));
		}
		#else
		ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_AUD_FMM_PLL1_MACRO );
		ulVal &= ~BCHP_AUD_FMM_PLL0_MACRO_MACRO_SELECT_MASK;
		BREG_Write32 (moduleHandle->regHandle, BCHP_AUD_FMM_PLL1_MACRO,
			(ulVal |( BCHP_AUD_FMM_PLL0_MACRO_MACRO_SELECT_User
					<< BCHP_AUD_FMM_PLL0_MACRO_MACRO_SELECT_SHIFT ) ));

		/* Enable PLL parameter update */
		BREG_Write32 (moduleHandle->regHandle, BCHP_AUD_FMM_PLL1_CONTROL,
			(BREG_Read32 (moduleHandle->regHandle, BCHP_AUD_FMM_PLL1_CONTROL) |
			BCHP_AUD_FMM_PLL0_CONTROL_USER_UPDATE_DIVIDERS_MASK));
		#endif
	      if(moduleHandle->currentSettings.moduleClkFreq.ulClkFreq ==24000000){
			/*04/26/2006 Qiang Xie  */
			/*external clock is 24MHZ */

			/*Set PLL Divider Parameters to generate 24.000 MHZ */
	#if (BCHP_CHIP==7401 || BCHP_CHIP == 7118 || BCHP_CHIP==7403	)
			ulVal =    (0x00000001 << BCHP_AUD_FMM_PLL0_USER_DIV1_USER_P1_PLL1_SHIFT) |
					(0x00000001 << BCHP_AUD_FMM_PLL0_USER_DIV1_USER_N2_PLL1_SHIFT) |
					(0x00000010 << BCHP_AUD_FMM_PLL0_USER_DIV1_USER_N1_PLL1_SHIFT) |
					(0x0000001e << BCHP_AUD_FMM_PLL0_USER_DIV1_USER_M1_PLL1_SHIFT) ;
			BREG_Write32 (moduleHandle->regHandle, BCHP_AUD_FMM_PLL1_USER_DIV1, ulVal);

			ulVal =    (0x00000001 << BCHP_AUD_FMM_PLL0_USER_DIV2_USER_P1_PLL2_SHIFT) |
					(0x00000003 << BCHP_AUD_FMM_PLL0_USER_DIV2_USER_N2_PLL2_SHIFT) |
					(0x00000019 << BCHP_AUD_FMM_PLL0_USER_DIV2_USER_N1_PLL2_SHIFT ) ;
			BREG_Write32 (moduleHandle->regHandle, BCHP_AUD_FMM_PLL1_USER_DIV2, ulVal);

			ulVal =    (0x00000005 << BCHP_AUD_FMM_PLL0_USER_DIV3_USER_M2_CH2_SHIFT) |
					(0x00000009 << BCHP_AUD_FMM_PLL0_USER_DIV3_USER_M1_CH2_SHIFT) |
					(0x00000001 << BCHP_AUD_FMM_PLL0_USER_DIV3_USER_M2_CH3_SHIFT) |
					(0x00000019 << BCHP_AUD_FMM_PLL0_USER_DIV3_USER_M1_CH3_SHIFT);
			BREG_Write32 (moduleHandle->regHandle, BCHP_AUD_FMM_PLL1_USER_DIV3, ulVal);

			ulVal =    (0x00000001 << BCHP_AUD_FMM_PLL0_USER_DIV4_USER_VCORNG_PLL1_SHIFT) |
					(0x00000001 << BCHP_AUD_FMM_PLL0_USER_DIV4_USER_VCORNG_PLL2_SHIFT ) ;
			BREG_Write32 (moduleHandle->regHandle, BCHP_AUD_FMM_PLL1_USER_DIV4, ulVal);

		#elif (BCHP_CHIP==7405 )|| (BCHP_CHIP==7325) ||  (BCHP_CHIP==7468)
				#if  (BCHP_CHIP==7325) || (BCHP_CHIP==7468) 
					ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_AUD_FMM_PLL1_USER_DIV1);
					ulVal &= ~BCHP_AUD_FMM_PLL0_USER_DIV1_M1DIV_MASK;
					ulVal |= 24;
					BREG_Write32 (moduleHandle->regHandle, BCHP_AUD_FMM_PLL1_USER_DIV1, ulVal);
					ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_AUD_FMM_PLL1_USER_DIV2);
					ulVal &= ~BCHP_AUD_FMM_PLL0_USER_DIV2_NDIV_INT_MASK    ;
					ulVal |= 50;
				#else
				ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_AUD_FMM_PLL1_USER_DIV1);
				ulVal &= ~BCHP_AUD_FMM_PLL0_USER_DIV1_M1DIV_MASK;
				#if (BCHP_VER < BCHP_VER_B0)
				ulVal |= 72;
				#else
				ulVal |= 36;
				#endif

				BREG_Write32 (moduleHandle->regHandle, BCHP_AUD_FMM_PLL1_USER_DIV1, ulVal);
				ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_AUD_FMM_PLL1_USER_DIV2);
				ulVal &= ~BCHP_AUD_FMM_PLL0_USER_DIV2_NDIV_INT_MASK    ;
			        #if (BCHP_VER < BCHP_VER_B0)
				ulVal |= 125;
				#else
				ulVal |= 75;
				#endif
				#endif
				ulVal = ulVal |
					(1 << BCHP_AUD_FMM_PLL0_USER_DIV2_VCORNG_SHIFT)|
					(15<<BCHP_AUD_FMM_PLL0_USER_DIV2_CTL_BITS_37_32_SHIFT);
				BREG_Write32 (moduleHandle->regHandle, BCHP_AUD_FMM_PLL1_USER_DIV2, ulVal);
				BREG_Write32 (moduleHandle->regHandle, BCHP_AUD_FMM_PLL1_USER_CTLBUS_LO, 0x380005c0 );
		#elif (BCHP_CHIP==7335)
		if(moduleHandle->currentSettings.moduleClkFreq.bIsRoutedInternal == false){
			ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_AUD_FMM_PLL1_USER_DIV1);
			ulVal &= ~BCHP_AUD_FMM_PLL0_USER_DIV1_M1DIV_MASK;
			ulVal |= 24;
			BREG_Write32 (moduleHandle->regHandle, BCHP_AUD_FMM_PLL1_USER_DIV1, ulVal);
			ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_AUD_FMM_PLL1_USER_DIV2);
			ulVal &= ~BCHP_AUD_FMM_PLL0_USER_DIV2_NDIV_INT_MASK    ;
			ulVal |= 50;
			ulVal = ulVal |
				(1 << BCHP_AUD_FMM_PLL0_USER_DIV2_VCORNG_SHIFT)|
				(15<<BCHP_AUD_FMM_PLL0_USER_DIV2_CTL_BITS_37_32_SHIFT);
			BREG_Write32 (moduleHandle->regHandle, BCHP_AUD_FMM_PLL1_USER_DIV2, ulVal);
			BREG_Write32 (moduleHandle->regHandle, BCHP_AUD_FMM_PLL1_USER_CTLBUS_LO, 0x380005c0 );
			}
		else{
			ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_AUD_FMM_PLL0_USER_DIV1);
			ulVal &= ~BCHP_AUD_FMM_PLL0_USER_DIV1_M1DIV_MASK;
			ulVal |= 24;
			BREG_Write32 (moduleHandle->regHandle, BCHP_AUD_FMM_PLL0_USER_DIV1, ulVal);
			ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_AUD_FMM_PLL0_USER_DIV2);
			ulVal &= ~BCHP_AUD_FMM_PLL0_USER_DIV2_NDIV_INT_MASK    ;
			ulVal |= 50;
			ulVal = ulVal |
					(1 << BCHP_AUD_FMM_PLL0_USER_DIV2_VCORNG_SHIFT)|
					(15<<BCHP_AUD_FMM_PLL0_USER_DIV2_CTL_BITS_37_32_SHIFT);
				BREG_Write32 (moduleHandle->regHandle, BCHP_AUD_FMM_PLL0_USER_DIV2, ulVal);
				
				BREG_Write32 (moduleHandle->regHandle, BCHP_AUD_FMM_PLL0_USER_CTLBUS_LO, 0x380005c0 );
		}
		#endif
              }else if(moduleHandle->currentSettings.moduleClkFreq.ulClkFreq ==36000000){
			#if (BCHP_CHIP==7401 || BCHP_CHIP == 7118 || BCHP_CHIP==7403	)
			ulVal =    (0x00000001 << BCHP_AUD_FMM_PLL0_USER_DIV1_USER_P1_PLL1_SHIFT) |
					(0x00000001 << BCHP_AUD_FMM_PLL0_USER_DIV1_USER_N2_PLL1_SHIFT) |
					(0x00000020 << BCHP_AUD_FMM_PLL0_USER_DIV1_USER_N1_PLL1_SHIFT) |
					(0x0000001e << BCHP_AUD_FMM_PLL0_USER_DIV1_USER_M1_PLL1_SHIFT) ;
			BREG_Write32 (moduleHandle->regHandle, BCHP_AUD_FMM_PLL1_USER_DIV1, ulVal);

			ulVal =    (0x00000002 << BCHP_AUD_FMM_PLL0_USER_DIV2_USER_P1_PLL2_SHIFT) |
					(0x00000003 << BCHP_AUD_FMM_PLL0_USER_DIV2_USER_N2_PLL2_SHIFT) |
					(0x00000019 << BCHP_AUD_FMM_PLL0_USER_DIV2_USER_N1_PLL2_SHIFT ) ;
			BREG_Write32 (moduleHandle->regHandle, BCHP_AUD_FMM_PLL1_USER_DIV2, ulVal);

			ulVal =    (0x00000005 << BCHP_AUD_FMM_PLL0_USER_DIV3_USER_M2_CH2_SHIFT) |
					(0x00000006<< BCHP_AUD_FMM_PLL0_USER_DIV3_USER_M1_CH2_SHIFT) |
					(0x00000001 << BCHP_AUD_FMM_PLL0_USER_DIV3_USER_M2_CH3_SHIFT) |
					(0x00000019 << BCHP_AUD_FMM_PLL0_USER_DIV3_USER_M1_CH3_SHIFT);
			BREG_Write32 (moduleHandle->regHandle, BCHP_AUD_FMM_PLL1_USER_DIV3, ulVal);

			ulVal =    (0x00000001 << BCHP_AUD_FMM_PLL0_USER_DIV4_USER_VCORNG_PLL1_SHIFT) |
					(0x00000001 << BCHP_AUD_FMM_PLL0_USER_DIV4_USER_VCORNG_PLL2_SHIFT ) ;
			BREG_Write32 (moduleHandle->regHandle, BCHP_AUD_FMM_PLL1_USER_DIV4, ulVal);
			#elif(	BCHP_CHIP==7405 )

				ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_AUD_FMM_PLL1_USER_DIV1);
				ulVal &= ~BCHP_AUD_FMM_PLL0_USER_DIV1_M1DIV_MASK;
				#if (BCHP_VER < BCHP_VER_B0)
				ulVal |= 48;
				#else
				ulVal |= 24;
				#endif
				BREG_Write32 (moduleHandle->regHandle, BCHP_AUD_FMM_PLL1_USER_DIV1, ulVal);

				ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_AUD_FMM_PLL1_USER_DIV2);
				ulVal &= ~BCHP_AUD_FMM_PLL0_USER_DIV2_NDIV_INT_MASK    ;
				#if (BCHP_VER < BCHP_VER_B0)
				ulVal |= 125;
				#else
				ulVal |= 75;
				#endif
				ulVal = ulVal |
					(1 << BCHP_AUD_FMM_PLL0_USER_DIV2_VCORNG_SHIFT)|
					(15<<BCHP_AUD_FMM_PLL0_USER_DIV2_CTL_BITS_37_32_SHIFT);
				BREG_Write32 (moduleHandle->regHandle, BCHP_AUD_FMM_PLL1_USER_DIV2, ulVal);
				BREG_Write32 (moduleHandle->regHandle, BCHP_AUD_FMM_PLL1_USER_CTLBUS_LO, 0x380005c0 );
		#elif (BCHP_CHIP==7325)
			ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_AUD_FMM_PLL1_USER_DIV1);
			ulVal &= ~BCHP_AUD_FMM_PLL0_USER_DIV1_M1DIV_MASK;
			ulVal |= 16;
			BREG_Write32 (moduleHandle->regHandle, BCHP_AUD_FMM_PLL1_USER_DIV1, ulVal);
			ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_AUD_FMM_PLL1_USER_DIV2);
			ulVal &= ~BCHP_AUD_FMM_PLL0_USER_DIV2_NDIV_INT_MASK    ;
			ulVal |= 50;
			ulVal = ulVal |
				(0 << BCHP_AUD_FMM_PLL0_USER_DIV2_VCORNG_SHIFT)|
				(15<<BCHP_AUD_FMM_PLL0_USER_DIV2_CTL_BITS_37_32_SHIFT);
			BREG_Write32 (moduleHandle->regHandle, BCHP_AUD_FMM_PLL1_USER_DIV2, ulVal);
		#elif (BCHP_CHIP==7335)
		if(moduleHandle->currentSettings.moduleClkFreq.bIsRoutedInternal == false){
			ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_AUD_FMM_PLL1_USER_DIV1);
			ulVal &= ~BCHP_AUD_FMM_PLL0_USER_DIV1_M1DIV_MASK;
			ulVal |= 16;
			BREG_Write32 (moduleHandle->regHandle, BCHP_AUD_FMM_PLL1_USER_DIV1, ulVal);
			ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_AUD_FMM_PLL1_USER_DIV2);
			ulVal &= ~BCHP_AUD_FMM_PLL0_USER_DIV2_NDIV_INT_MASK    ;
			ulVal |= 50;
			ulVal = ulVal |
				(0 << BCHP_AUD_FMM_PLL0_USER_DIV2_VCORNG_SHIFT)|
				(15<<BCHP_AUD_FMM_PLL0_USER_DIV2_CTL_BITS_37_32_SHIFT);
			BREG_Write32 (moduleHandle->regHandle, BCHP_AUD_FMM_PLL1_USER_DIV2, ulVal);}
		else{
			ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_AUD_FMM_PLL0_USER_DIV1);
			ulVal &= ~BCHP_AUD_FMM_PLL0_USER_DIV1_M1DIV_MASK;
			ulVal |= 16;
			BREG_Write32 (moduleHandle->regHandle, BCHP_AUD_FMM_PLL0_USER_DIV1, ulVal);
			ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_AUD_FMM_PLL0_USER_DIV2);
			ulVal &= ~BCHP_AUD_FMM_PLL0_USER_DIV2_NDIV_INT_MASK    ;
			ulVal |= 50;
			ulVal = ulVal |
					(0 << BCHP_AUD_FMM_PLL0_USER_DIV2_VCORNG_SHIFT)|
					(15<<BCHP_AUD_FMM_PLL0_USER_DIV2_CTL_BITS_37_32_SHIFT);
				BREG_Write32 (moduleHandle->regHandle, BCHP_AUD_FMM_PLL0_USER_DIV2, ulVal);
		}
		#elif (BCHP_CHIP==7468)
			ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_AUD_FMM_PLL1_USER_DIV1);
			ulVal &= ~BCHP_AUD_FMM_PLL0_USER_DIV1_M1DIV_MASK;
			ulVal |= 24;
			BREG_Write32 (moduleHandle->regHandle, BCHP_AUD_FMM_PLL1_USER_DIV1, ulVal);
			ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_AUD_FMM_PLL1_USER_DIV2);
			ulVal &= ~BCHP_AUD_FMM_PLL0_USER_DIV2_NDIV_INT_MASK    ;
			ulVal |= 75;
			ulVal = ulVal |
				(0 << BCHP_AUD_FMM_PLL0_USER_DIV2_VCORNG_SHIFT)|
				(15<<BCHP_AUD_FMM_PLL0_USER_DIV2_CTL_BITS_37_32_SHIFT); 
			BREG_Write32 (moduleHandle->regHandle, BCHP_AUD_FMM_PLL1_USER_DIV2, ulVal);
		#endif
		}
		else if(moduleHandle->currentSettings.moduleClkFreq.ulClkFreq ==36864000){
                     /*04/26/2006 Qiang Xie */
			/*external clock is 36.894MHZ*/

		/*Set PLL Divider Parameters to generate 36.864 MHZ */
		#if (BCHP_CHIP==7401 || BCHP_CHIP == 7118 || BCHP_CHIP==7403	)
			ulVal =    (0x00000001 << BCHP_AUD_FMM_PLL0_USER_DIV1_USER_P1_PLL1_SHIFT) |
					(0x00000001 << BCHP_AUD_FMM_PLL0_USER_DIV1_USER_N2_PLL1_SHIFT) |
					(0x00000020 << BCHP_AUD_FMM_PLL0_USER_DIV1_USER_N1_PLL1_SHIFT) |
					(0x0000000f << BCHP_AUD_FMM_PLL0_USER_DIV1_USER_M1_PLL1_SHIFT) ;
			BREG_Write32 (moduleHandle->regHandle, BCHP_AUD_FMM_PLL1_USER_DIV1, ulVal);

			ulVal =    (0x00000003 << BCHP_AUD_FMM_PLL0_USER_DIV2_USER_P1_PLL2_SHIFT) |
					(0x00000001 << BCHP_AUD_FMM_PLL0_USER_DIV2_USER_N2_PLL2_SHIFT) |
					(0x00000030 << BCHP_AUD_FMM_PLL0_USER_DIV2_USER_N1_PLL2_SHIFT) |
					(0x00000001 << BCHP_AUD_FMM_PLL0_USER_DIV2_USER_M2_CH1_SHIFT) |
					(0x00000019 << BCHP_AUD_FMM_PLL0_USER_DIV2_USER_M1_CH1_SHIFT ) ;
			BREG_Write32 (moduleHandle->regHandle, BCHP_AUD_FMM_PLL1_USER_DIV2, ulVal);

			ulVal =    (0x00000001 << BCHP_AUD_FMM_PLL0_USER_DIV3_USER_M2_CH2_SHIFT) |
					(0x00000019 << BCHP_AUD_FMM_PLL0_USER_DIV3_USER_M1_CH2_SHIFT) |
					(0x00000001 << BCHP_AUD_FMM_PLL0_USER_DIV3_USER_M2_CH3_SHIFT) |
					(0x00000019 << BCHP_AUD_FMM_PLL0_USER_DIV3_USER_M1_CH3_SHIFT);
			BREG_Write32 (moduleHandle->regHandle, BCHP_AUD_FMM_PLL1_USER_DIV3, ulVal);

			ulVal =    (0x00000001 << BCHP_AUD_FMM_PLL0_USER_DIV4_USER_VCORNG_PLL1_SHIFT) |
					(0x00000001 << BCHP_AUD_FMM_PLL0_USER_DIV4_USER_VCORNG_PLL2_SHIFT ) ;
			BREG_Write32 (moduleHandle->regHandle, BCHP_AUD_FMM_PLL1_USER_DIV4, ulVal);
		#elif (BCHP_CHIP==7405 )
			ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_AUD_FMM_PLL1_USER_DIV1);
			ulVal &= ~BCHP_AUD_FMM_PLL0_USER_DIV1_M1DIV_MASK;
			#if (BCHP_VER < BCHP_VER_B0)
			ulVal |= 24;
			#else
			ulVal |= 20;
			#endif
			BREG_Write32 (moduleHandle->regHandle, BCHP_AUD_FMM_PLL1_USER_DIV1, ulVal);
			ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_AUD_FMM_PLL1_USER_DIV2);
			ulVal &= ~BCHP_AUD_FMM_PLL0_USER_DIV2_NDIV_INT_MASK    ;
			ulVal |= 64;
			ulVal = ulVal |
				(0 << BCHP_AUD_FMM_PLL0_USER_DIV2_VCORNG_SHIFT)|
				(15<<BCHP_AUD_FMM_PLL0_USER_DIV2_CTL_BITS_37_32_SHIFT);
			BREG_Write32 (moduleHandle->regHandle, BCHP_AUD_FMM_PLL1_USER_DIV2, ulVal);
			BREG_Write32 (moduleHandle->regHandle, BCHP_AUD_FMM_PLL1_USER_CTLBUS_LO, 0x200005c0 );
		#elif (BCHP_CHIP==7325)
			ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_AUD_FMM_PLL1_USER_DIV1);
			ulVal &= ~BCHP_AUD_FMM_PLL0_USER_DIV1_M1DIV_MASK;
			ulVal |= 20;
			BREG_Write32 (moduleHandle->regHandle, BCHP_AUD_FMM_PLL1_USER_DIV1, ulVal);
			ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_AUD_FMM_PLL1_USER_DIV2);
			ulVal &= ~BCHP_AUD_FMM_PLL0_USER_DIV2_NDIV_INT_MASK    ;
			ulVal |= 64;
			ulVal = ulVal |
				(0 << BCHP_AUD_FMM_PLL0_USER_DIV2_VCORNG_SHIFT)|
				(15<<BCHP_AUD_FMM_PLL0_USER_DIV2_CTL_BITS_37_32_SHIFT);
			BREG_Write32 (moduleHandle->regHandle, BCHP_AUD_FMM_PLL1_USER_DIV2, ulVal);
			#elif (BCHP_CHIP==7335)
			if(moduleHandle->currentSettings.moduleClkFreq.bIsRoutedInternal == false){
			ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_AUD_FMM_PLL1_USER_DIV1);
			ulVal &= ~BCHP_AUD_FMM_PLL0_USER_DIV1_M1DIV_MASK;
			ulVal |= 20;
			BREG_Write32 (moduleHandle->regHandle, BCHP_AUD_FMM_PLL1_USER_DIV1, ulVal);
			ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_AUD_FMM_PLL1_USER_DIV2);
			ulVal &= ~BCHP_AUD_FMM_PLL0_USER_DIV2_NDIV_INT_MASK    ;
			ulVal |= 64;
			ulVal = ulVal |
				(0 << BCHP_AUD_FMM_PLL0_USER_DIV2_VCORNG_SHIFT)|
				(15<<BCHP_AUD_FMM_PLL0_USER_DIV2_CTL_BITS_37_32_SHIFT);
			BREG_Write32 (moduleHandle->regHandle, BCHP_AUD_FMM_PLL1_USER_DIV2, ulVal);
			}else{
				ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_AUD_FMM_PLL0_USER_DIV1);
				ulVal &= ~BCHP_AUD_FMM_PLL0_USER_DIV1_M1DIV_MASK;
				ulVal |= 20;
				BREG_Write32 (moduleHandle->regHandle, BCHP_AUD_FMM_PLL0_USER_DIV1, ulVal);
				ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_AUD_FMM_PLL0_USER_DIV2);
				ulVal &= ~BCHP_AUD_FMM_PLL0_USER_DIV2_NDIV_INT_MASK    ;
				ulVal |= 64;
				ulVal = ulVal |
						(0 << BCHP_AUD_FMM_PLL0_USER_DIV2_VCORNG_SHIFT)|
						(15<<BCHP_AUD_FMM_PLL0_USER_DIV2_CTL_BITS_37_32_SHIFT);
				BREG_Write32 (moduleHandle->regHandle, BCHP_AUD_FMM_PLL0_USER_DIV2, ulVal);

			}
			#elif (BCHP_CHIP==7468) 	
			ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_AUD_FMM_PLL1_USER_DIV1);
			ulVal &= ~BCHP_AUD_FMM_PLL0_USER_DIV1_M1DIV_MASK;
			ulVal |= 20;
			BREG_Write32 (moduleHandle->regHandle, BCHP_AUD_FMM_PLL1_USER_DIV1, ulVal);
			ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_AUD_FMM_PLL1_USER_DIV2);
			ulVal &= ~BCHP_AUD_FMM_PLL0_USER_DIV2_NDIV_INT_MASK    ;
			ulVal |= 64;
			ulVal = ulVal |
				(0 << BCHP_AUD_FMM_PLL0_USER_DIV2_VCORNG_SHIFT)|
				(15<<BCHP_AUD_FMM_PLL0_USER_DIV2_CTL_BITS_37_32_SHIFT); 
			BREG_Write32 (moduleHandle->regHandle, BCHP_AUD_FMM_PLL1_USER_DIV2, ulVal);
		#endif
		}else{
			BDBG_WRN((" Ext Clock freq %d is not supported, please use custom setting.\n",moduleHandle->currentSettings.moduleClkFreq.ulClkFreq ));
		}
                /* 06/14/06, Q. Xie, 7401B0 change PIN_MUX register */
#if ((BCHP_VER >= BCHP_VER_A0) && (BCHP_VER < BCHP_VER_B0) &&(BCHP_CHIP==7401))
                /* Configure  GPIO_38 Pinmux = aud_fs_clk_1 */
		ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8);

		ulVal &= ~BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_38_MASK;

		BREG_Write32 (moduleHandle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8,
			(ulVal |( 1 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_38_SHIFT )));

		/* Switch to use external clock in Smart Card Block */
		BREG_Write32 (moduleHandle->regHandle, BCHP_PM_CONFIG,
				(BREG_Read32 (moduleHandle->regHandle, BCHP_PM_CONFIG) |
				BCHP_PM_CONFIG_sc_ext_clk_sel_MASK));
		}
		/* Configure  sc_ext_clk pinmux = sc_ext_clk */
		ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4);

		ulVal &= ~BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_sc_ext_clk_MASK;

		BREG_Write32 (moduleHandle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4, ulVal );

#elif ((BCHP_VER >= BCHP_VER_B0) &&(BCHP_CHIP==7401))
		/* Configure  GPIO_38 Pinmux = aud_fs_clk_1 */
		ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10);

		ulVal &= ~BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_38_MASK;

		BREG_Write32 (moduleHandle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10,
			(ulVal |( 1 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_38_SHIFT )));

		/* Switch to use external clock in Smart Card Block */
		BREG_Write32 (moduleHandle->regHandle, BCHP_PM_CONFIG,
				(BREG_Read32 (moduleHandle->regHandle, BCHP_PM_CONFIG) |
				BCHP_PM_CONFIG_sc_ext_clk_sel_MASK));
		}
		/* Configure  sc_ext_clk pinmux = sc_ext_clk */
		ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6);

		ulVal &= ~BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_sc_ext_clk_MASK;

		BREG_Write32 (moduleHandle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6, ulVal );

#elif(BCHP_CHIP== 7118)
		/* Configure  GPIO_26 Pinmux = aud_fs_clk_1 */
   	ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8);

		ulVal &= ~BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_26_MASK;

		BREG_Write32 (moduleHandle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8,
			(ulVal |( 2 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_26_SHIFT )));

		/* Switch to use external clock in Smart Card Block */
		BREG_Write32 (moduleHandle->regHandle, BCHP_PM_CONFIG,
				(BREG_Read32 (moduleHandle->regHandle, BCHP_PM_CONFIG) |
				BCHP_PM_CONFIG_sc_ext_clk_sel_MASK));
		}
		/* Configure  sc_ext_clk pinmux = sc_ext_clk */

		ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12);

		ulVal &= ~BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_66_MASK;

		BREG_Write32 (moduleHandle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12,
			(ulVal |( 1 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_66_SHIFT )));
#elif(BCHP_CHIP==7403)
              /* Configure  GPIO_38 Pinmux = aud_fs_clk_1 */
		ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9);

		ulVal &= ~BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_38_MASK;

		BREG_Write32 (moduleHandle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9,
			(ulVal |( 1 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_38_SHIFT )));

		/* Switch to use external clock in Smart Card Block */
		BREG_Write32 (moduleHandle->regHandle, BCHP_PM_CONFIG,
				(BREG_Read32 (moduleHandle->regHandle, BCHP_PM_CONFIG) |
				BCHP_PM_CONFIG_sc_ext_clk_sel_MASK));
}
		/* Configure  sc_ext_clk pinmux = sc_ext_clk */
		ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6);

		ulVal &= ~BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_sc_ext_clk_MASK;

		BREG_Write32 (moduleHandle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6, ulVal );
#elif(BCHP_CHIP==7405) || (BCHP_CHIP==7325) || (BCHP_CHIP==7335) || (BCHP_CHIP==7468)

		/* setup to use 13.864 ref lock */
		ulVal = BREG_Read32 (moduleHandle->regHandle, BCHP_VCXO_0_RM_RATE_RATIO);
		ulVal &= ~BCHP_VCXO_0_RM_RATE_RATIO_DENOMINATOR_MASK;
		ulVal |= 32<<BCHP_VCXO_0_RM_RATE_RATIO_DENOMINATOR_SHIFT;
		BREG_Write32 (moduleHandle->regHandle, BCHP_VCXO_0_RM_RATE_RATIO,ulVal);

		ulVal = BREG_Read32 (moduleHandle->regHandle, BCHP_VCXO_0_RM_SAMPLE_INC);
		ulVal &=~BCHP_VCXO_0_RM_SAMPLE_INC_SAMPLE_INC_MASK;
		ulVal |= 3<<BCHP_VCXO_0_RM_SAMPLE_INC_SAMPLE_INC_SHIFT;
		ulVal &=~BCHP_VCXO_0_RM_SAMPLE_INC_NUMERATOR_MASK;
		ulVal |= 29<<BCHP_VCXO_0_RM_SAMPLE_INC_NUMERATOR_SHIFT;
		BREG_Write32 (moduleHandle->regHandle, BCHP_VCXO_0_RM_SAMPLE_INC,ulVal);

		ulVal = BREG_Read32 (moduleHandle->regHandle, BCHP_VCXO_CTL_MISC_VC0_CTRL);
		ulVal &= ~BCHP_VCXO_CTL_MISC_VC0_CTRL_DRESET_MASK;
		BREG_Write32 (moduleHandle->regHandle, BCHP_VCXO_CTL_MISC_VC0_CTRL,ulVal);
        #if(BCHP_CHIP==7335)
			if(moduleHandle->currentSettings.moduleClkFreq.bIsRoutedInternal == false){
				ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_AUD_FMM_PLL1_CONTROL);
				ulVal &= ~BCHP_AUD_FMM_PLL0_CONTROL_REFERENCE_SELECT_MASK;
				BREG_Write32 (moduleHandle->regHandle, BCHP_AUD_FMM_PLL1_CONTROL, ulVal);
			}else{
				ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_AUD_FMM_PLL0_CONTROL);
				ulVal &= ~BCHP_AUD_FMM_PLL0_CONTROL_REFERENCE_SELECT_MASK;
				BREG_Write32 (moduleHandle->regHandle, BCHP_AUD_FMM_PLL0_CONTROL, ulVal);
			}
		#else
		ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_AUD_FMM_PLL1_CONTROL);
		ulVal &= ~BCHP_AUD_FMM_PLL0_CONTROL_REFERENCE_SELECT_MASK;
		BREG_Write32 (moduleHandle->regHandle, BCHP_AUD_FMM_PLL1_CONTROL, ulVal);
		#endif
			/* Switch to use external clock in Smart Card Block */
		#if(BCHP_CHIP!=7468)
		ulVal = BREG_Read32 (moduleHandle->regHandle, BCHP_PM_CONFIG);
		ulVal |=BCHP_PM_CONFIG_sc_ext_clk_sel_MASK;
		BREG_Write32 (moduleHandle->regHandle, BCHP_PM_CONFIG,ulVal);
		#endif
		#if (BCHP_CHIP==7405)
              /* Configure  GPIO_019 Pinmux = sc_clk_out */
		ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4);

		ulVal &= ~BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_021_MASK;
		ulVal |=5 <<BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_021_SHIFT;
		BREG_Write32 (moduleHandle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4, ulVal);
		}
		/* Configure  sc_ext_clk pinmux = sc_ext_clk */
		ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12);

		ulVal &= ~BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_103_MASK;
		ulVal |=1 <<BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_103_SHIFT;
		BREG_Write32 (moduleHandle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12, ulVal );
		#elif (BCHP_CHIP==7335)
		if(moduleHandle->currentSettings.moduleClkFreq.bIsRoutedInternal == false){
		/* Configure  GPIO_019 Pinmux = sc_clk_out */
		ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8);
		ulVal &= ~BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_050_MASK;
		ulVal |=1 <<BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_050_SHIFT;
		BREG_Write32 (moduleHandle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8, ulVal);
		/* Configure  sc_ext_clk pinmux = sc_ext_clk */
		ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8);
		ulVal &= ~BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_051_MASK;
		ulVal |=1 <<BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_051_SHIFT;
		BREG_Write32 (moduleHandle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8, ulVal );

		}
		else{
			ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_CLK_MISC);
			ulVal &= ~ BCHP_CLK_MISC_SC_AUD0_PLL_SEL_MASK ;
            ulVal |=1 <<BCHP_CLK_MISC_SC_AUD0_PLL_SEL_SHIFT ;
			BREG_Write32 (moduleHandle->regHandle, BCHP_CLK_MISC, ulVal);
			}
		}

		#elif (BCHP_CHIP==7325)

		if(moduleHandle->currentSettings.moduleClkFreq.bIsRoutedInternal == false){
		/* Configure  GPIO_019 Pinmux = sc_clk_out */
		/*need to confir which pin is sc_clk_out */
		ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4);
		ulVal &= ~BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_25_MASK;
		ulVal |=1 <<BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_25_SHIFT;
		BREG_Write32 (moduleHandle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4, ulVal);
			/* Configure  sc_ext_clk pinmux = sc_ext_clk */
			ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_3);

			ulVal &= ~BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_3_gpio_14_MASK;
			ulVal |=2 <<BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_3_gpio_14_SHIFT;
			BREG_Write32 (moduleHandle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_3, ulVal );

				}else{
				ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_SUN_TOP_CTRL_GENERAL_CTRL_1);
				ulVal &= ~BCHP_SUN_TOP_CTRL_GENERAL_CTRL_1_smartcard_clk_pll_sel_MASK;
				ulVal |=1 <<BCHP_SUN_TOP_CTRL_GENERAL_CTRL_1_smartcard_clk_pll_sel_SHIFT;
				BREG_Write32 (moduleHandle->regHandle, BCHP_SUN_TOP_CTRL_GENERAL_CTRL_1, ulVal);

				}
		}
		#elif (BCHP_CHIP == 7468)
		BDBG_MSG(("7468: don't need to set pin muxing\n"));}
		#else

			}
			BDBG_WRN(("***Unknown chip!***\n"));
		#endif
#else
	BSTD_UNUSED(ulVal);
#endif
#elif (BCHP_CHIP == 7420) ||(BCHP_CHIP==7340) ||(BCHP_CHIP==7342)

	 if(moduleHandle->currentSettings.moduleClkFreq.ulClkFreq ==36000000){
		BREG_AtomicUpdate32 (moduleHandle->regHandle,  BCHP_SMARTCARD_PLL_MACRO,BCHP_SMARTCARD_PLL_MACRO_MACRO_SELECT_MASK,
			BCHP_SMARTCARD_PLL_MACRO_MACRO_SELECT_freq_36MHz );
	 }else  if(moduleHandle->currentSettings.moduleClkFreq.ulClkFreq ==24000000){
		BREG_AtomicUpdate32 (moduleHandle->regHandle,  BCHP_SMARTCARD_PLL_MACRO,BCHP_SMARTCARD_PLL_MACRO_MACRO_SELECT_MASK,
			BCHP_SMARTCARD_PLL_MACRO_MACRO_SELECT_freq_24MHz );
	 }else  {   /* 36.864Mhz */

	#if(BCHP_CHIP!=7340)
	/*
	 ulVal = BREG_Read32 (moduleHandle->regHandle,
					BCHP_CLK_MISC);
			ulVal &= ~(BCHP_CLK_MISC_SMARTCARD_PLL_REFERENCE_CLK_SEL_MASK );*/
			BREG_AtomicUpdate32 (moduleHandle->regHandle, BCHP_CLK_MISC,BCHP_CLK_MISC_SMARTCARD_PLL_REFERENCE_CLK_SEL_MASK ,
				ulVal |(1<<BCHP_CLK_MISC_SMARTCARD_PLL_REFERENCE_CLK_SEL_SHIFT));

	#elif (BCHP_CHIP == 7340)
	/*
	 ulVal = BREG_Read32 (moduleHandle->regHandle,
					BCHP_CLKGEN_SMARTCARD_CLOCK_CTRL);
			ulVal &= ~(BCHP_CLKGEN_SMARTCARD_CLOCK_CTRL_SMARTCARD_PLL_REFERENCE_CLK_SEL_MASK );*/
			BREG_AtomicUpdate32 (moduleHandle->regHandle, BCHP_CLKGEN_SMARTCARD_CLOCK_CTRL,BCHP_CLKGEN_SMARTCARD_CLOCK_CTRL_SMARTCARD_PLL_REFERENCE_CLK_SEL_MASK ,
				ulVal |(1<<BCHP_CLKGEN_SMARTCARD_CLOCK_CTRL_SMARTCARD_PLL_REFERENCE_CLK_SEL_SHIFT));
	#endif
			BREG_AtomicUpdate32 (moduleHandle->regHandle,  BCHP_SMARTCARD_PLL_MACRO,BCHP_SMARTCARD_PLL_MACRO_MACRO_SELECT_MASK,
			BCHP_SMARTCARD_PLL_MACRO_MACRO_SELECT_freq_36p864MHz );
	 	}
#elif (BCHP_CHIP==7125)
	 if(moduleHandle->currentSettings.moduleClkFreq.ulClkFreq ==36000000){
		BREG_AtomicUpdate32 (moduleHandle->regHandle,  BCHP_SMARTCARD_PLL_SC_MACRO,BCHP_SMARTCARD_PLL_SC_MACRO_MACRO_SELECT_MASK,
			BCHP_SMARTCARD_PLL_SC_MACRO_MACRO_SELECT_freq_36MHz );
	 }else  if(moduleHandle->currentSettings.moduleClkFreq.ulClkFreq ==24000000){
		BREG_AtomicUpdate32 (moduleHandle->regHandle,  BCHP_SMARTCARD_PLL_SC_MACRO,BCHP_SMARTCARD_PLL_SC_MACRO_MACRO_SELECT_MASK,
			BCHP_SMARTCARD_PLL_SC_MACRO_MACRO_SELECT_freq_24MHz );
	 }else  {   /* 36.864Mhz */
	 /*
	 ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_CLKGEN_SMARTCARD_CLOCK_CTRL);
	 ulVal &= ~(BCHP_CLKGEN_SMARTCARD_CLOCK_CTRL_SMARTCARD_PLL_REFERENCE_CLK_SEL_MASK );*/
	 BREG_AtomicUpdate32 (moduleHandle->regHandle, BCHP_CLKGEN_SMARTCARD_CLOCK_CTRL,BCHP_CLKGEN_SMARTCARD_CLOCK_CTRL_SMARTCARD_PLL_REFERENCE_CLK_SEL_MASK ,
			 ulVal|(1<<BCHP_CLKGEN_SMARTCARD_CLOCK_CTRL_SMARTCARD_PLL_REFERENCE_CLK_SEL_SHIFT));

	BREG_AtomicUpdate32 (moduleHandle->regHandle,  BCHP_SMARTCARD_PLL_SC_MACRO,BCHP_SMARTCARD_PLL_SC_MACRO_MACRO_SELECT_MASK,
	BCHP_SMARTCARD_PLL_SC_MACRO_MACRO_SELECT_freq_36p864MHz );
	 	}
#else
		BDBG_MSG(("**** External clock ****\n"));
#endif

	}
	}else{
		BDBG_WRN((" External oscillator is being used and should be configured by the user.\n"));
	}


	/***************************************************************
		Warning:  Note that we have to modify the board to use GPIO and connect it
				 to pin 3 (3V/5V) of TDA chip and run this function.
				 Make sure to disconnect your QAM or 	QPSK connection before
				 calling this function or your smartcard will be damaged.
	***************************************************************/

#if ((BCHP_CHIP==7038) &&  (BCHP_VER >= BCHP_VER_B0) && (BCHP_VER < BCHP_VER_C0 ))
		ulVal = BREG_Read32 (moduleHandle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8);
		ulVal &= ~(7<< BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_44_SHIFT);
		BREG_Write32 (moduleHandle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8,
			ulVal );

		/* GIO_IODIR_HI[GPIO_44] */
		ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_GIO_IODIR_HI);
		ulVal &=  ~(0x1000);
		BREG_Write32 (moduleHandle->regHandle, BCHP_GIO_IODIR_HI, ulVal );
#endif

#if ((BCHP_CHIP==7038) && ( BCHP_VER >= BCHP_VER_C0) && (BCHP_VER < BCHP_VER_D0 )) || (BCHP_CHIP==7438)
		ulVal = BREG_Read32 (moduleHandle->regHandle, 	BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10);
		ulVal &= ~(7<< BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_58_SHIFT);
		BREG_Write32 (moduleHandle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10,ulVal );

		/* GIO_IODIR_HI[GPIO_58] */
		ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_GIO_IODIR_HI);
		ulVal &=  ~(0x04000000);
		BREG_Write32 (moduleHandle->regHandle, BCHP_GIO_IODIR_HI, ulVal );
#endif

#if ((BCHP_CHIP==7400 ))
		if(BCHP_VER < BCHP_VER_B0){
		/*Set Pinmux for the GPIO used to control 5v/~3V in TDA8024 */
		ulVal = BREG_Read32 (moduleHandle->regHandle, 	BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9);
		ulVal &= ~(7<< BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_073_SHIFT );
		BREG_Write32 (moduleHandle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9,ulVal );

		/* Set IO Direction for the GPIO used to control 5v/~3V in TDA8024 */
		/* GIO_IODIR_EXT[GPIO_73] */
		ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_GIO_IODIR_EXT);
		ulVal &=  ~(0x00080000);
		BREG_Write32 (moduleHandle->regHandle, BCHP_GIO_IODIR_EXT, ulVal );
		}else{
			/* John Xie, 07/21/2007, there are two options, GPIO45 and GPIO 111 */
			/* In this example, I use GPIO 111 only */

			ulVal = BREG_Read32 (moduleHandle->regHandle, 	BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13);
			ulVal &= ~(3<< BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13_gpio_111_SHIFT );
			BREG_Write32 (moduleHandle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13,ulVal );
			/* using GPIO45 */
#if 0
			ulVal = BREG_Read32 (moduleHandle->regHandle, 	BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7);
			ulVal &= ~(4<< BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_045_SHIFT );
			BREG_Write32 (moduleHandle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7,ulVal );
#endif
		}
#endif

#if ((BCHP_CHIP==7125 ))
			/*Set Pinmux for the GPIO used to control 5v/~3V in TDA8024 */
			ulVal = BREG_Read32 (moduleHandle->regHandle,	BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16);	
			ulVal &= ~(15<< BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16_gpio_58_SHIFT );	
			BREG_Write32 (moduleHandle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16,ulVal );
	
			/* Set IO Direction for the GPIO used to control 5v/~3V in TDA8024 */
			/* GIO_IODIR_EXT[GPIO_58] */	
			ulVal = BREG_Read32 (moduleHandle->regHandle,  BCHP_GIO_IODIR_HI); 
			ulVal &=  ~(1<<(58-32));	
			BREG_Write32 (moduleHandle->regHandle, BCHP_GIO_IODIR_HI, ulVal ); 
#endif	

	/*
		If inp_sSettings->maxChannels == 0, set it to
		BSCD_MAX_SUPPOTED_CHANNELS
	*/
	if (moduleHandle->currentSettings.ucMaxChannels == 0)
		moduleHandle->currentSettings.ucMaxChannels =
							BSCD_MAX_SUPPOTED_CHANNELS;

	for( channelNum = 0;
		channelNum < moduleHandle->currentSettings.ucMaxChannels;
		channelNum++ )
	{
		moduleHandle->channelHandles[channelNum] = NULL;
	}

	*outp_handle = moduleHandle;

	BKNI_EnterCriticalSection();
	moduleHandle->bIsOpen = true;
	BKNI_LeaveCriticalSection();

BSCD_P_DONE_LABEL:

	BDBG_LEAVE(BSCD_Open);
	return( errCode );

}


BERR_Code BSCD_Close(
		BSCD_Handle inout_handle
)
{
	BERR_Code errCode = BERR_SUCCESS;
       /* 12/02/2006 QX: add this to avoid crash during BKNI_Free */
	 void *pTemp;


	BDBG_ENTER(BSCD_Close);
	BDBG_ASSERT( inout_handle );

	BKNI_EnterCriticalSection();
	BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, (inout_handle ==  NULL) );
	BKNI_LeaveCriticalSection();

	BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
		(inout_handle->ulMagicNumber != BSCD_P_HANDLE_MAGIC_NUMBER ) );

	BKNI_EnterCriticalSection();
	BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
		(inout_handle->bIsOpen ==  false) );
	BKNI_LeaveCriticalSection();

BSCD_P_DONE_LABEL:
	BKNI_EnterCriticalSection();
	inout_handle->bIsOpen = false;
	BKNI_LeaveCriticalSection();

 /* 12/02/2006 QX: add this to avoid crash during BKNI_Free */
	BKNI_EnterCriticalSection();
	pTemp = inout_handle;
	inout_handle = NULL;
	BKNI_LeaveCriticalSection();

	BKNI_Free(pTemp);


	BDBG_LEAVE(BSCD_Close);
	return( errCode );
}


BERR_Code BSCD_GetTotalChannels(
		BSCD_Handle		in_handle,
		unsigned char		*outp_ucTotalChannels
)
{
	BERR_Code errCode = BERR_SUCCESS;

	BDBG_ENTER(BSCD_GetTotalChannels);
	BDBG_ASSERT( in_handle );

	BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
		(in_handle->ulMagicNumber != BSCD_P_HANDLE_MAGIC_NUMBER ) );

	*outp_ucTotalChannels = in_handle->currentSettings.ucMaxChannels;

BSCD_P_DONE_LABEL:

	BDBG_LEAVE(BSCD_GetTotalChannels);
	return( errCode );
}


BERR_Code BSCD_GetChannelDefaultSettings(
		BSCD_Handle				in_handle,
		unsigned int			in_channelNo,
		BSCD_ChannelSettings	*outp_sSettings
)
{
	BERR_Code errCode = BERR_SUCCESS;


	BDBG_ENTER(BSCD_GetChannelDefaultSettings);
	BDBG_ASSERT( in_handle );

	BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
		(in_handle->ulMagicNumber != BSCD_P_HANDLE_MAGIC_NUMBER ) );

	BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BERR_INVALID_PARAMETER,
		(in_channelNo >= in_handle->currentSettings.ucMaxChannels) );

	*outp_sSettings = BSCD_defScdChannelSettings;

BSCD_P_DONE_LABEL:

	BDBG_LEAVE(BSCD_GetChannelDefaultSettings);
	return( errCode );
}


BERR_Code BSCD_Channel_Open(
		BSCD_Handle					in_handle,
		BSCD_ChannelHandle			*outp_channelHandle,
		unsigned int				in_channelNo,
		const BSCD_ChannelSettings	*inp_channelDefSettings
)
{
	BERR_Code errCode = BERR_SUCCESS;
 	BSCD_ChannelHandle channelHandle = NULL;
	uint32_t 			ulVal = 0;
#if (BCHP_CHIP == 3548) || (BCHP_CHIP==3556)|| (BCHP_CHIP==35230)|| (BCHP_CHIP==35233)
	BINT_Id 			intId = BCHP_INT_ID_CREATE(BCHP_SCIRQ0_SCIRQEN, BCHP_SCIRQ0_SCIRQEN_sca_irqen_SHIFT);

#else
	BINT_Id 			intId = BCHP_INT_ID_sca_irqen;
#endif
#ifdef BCHP_PWR_RESOURCE_SMARTCARD0
    BCHP_PWR_ResourceId resourceId = BCHP_PWR_RESOURCE_SMARTCARD0;
#ifdef BCHP_PWR_RESOURCE_SMARTCARD1
    if(in_channelNo == 1) {
	resourceId = BCHP_PWR_RESOURCE_SMARTCARD1;
    }
#endif
#endif

	BDBG_ENTER(BSCD_Channel_Open);
	BDBG_ASSERT( in_handle );
	BSTD_UNUSED(ulVal);

	BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
		(in_handle->ulMagicNumber != BSCD_P_HANDLE_MAGIC_NUMBER ) );

	BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BERR_INVALID_PARAMETER,
		(in_channelNo >= in_handle->currentSettings.ucMaxChannels) );

	BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
		(in_handle->bIsOpen ==  false) );

	/* channel handle must be NULL.  */
	BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
		(in_handle->channelHandles[in_channelNo]  != NULL) );

	*outp_channelHandle = NULL;

	/* Alloc memory from the system heap */
	if ((channelHandle =
			(BSCD_ChannelHandle) BKNI_Malloc(sizeof(BSCD_P_ChannelHandle)))
			== NULL) {
		/* wrap initially detected error code */
		errCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
		goto BSCD_P_DONE_LABEL;
	}

	BKNI_Memset(channelHandle, 0, sizeof( BSCD_P_ChannelHandle ));

	channelHandle->ulMagicNumber = BSCD_P_CHANNEL_HANDLE_MAGIC_NUMBER;
	channelHandle->moduleHandle = in_handle;

	channelHandle->ucChannelNumber = in_channelNo;

#ifdef BCHP_PWR_RESOURCE_SMARTCARD0
	/* acquire power */
	BCHP_PWR_AcquireResource(channelHandle->moduleHandle->chipHandle, resourceId);
#endif

#ifdef BSCD_EMV2000_CWT_PLUS_4
	channelHandle->bIsReceive = false;
#endif
	switch (in_channelNo) {
#if(BCHP_CHIP!=7125) ||((BCHP_CHIP==7125)&& (BCHP_VER>=BCHP_VER_C0))
		case 0:
			channelHandle->ulRegStartAddr = BCHP_SCA_SC_UART_CMD_1;
#if(BCHP_CHIP == 7420)||(BCHP_CHIP==7340) ||(BCHP_CHIP==7342)|| (BCHP_CHIP==7125)||(BCHP_CHIP==7468)
			 if ((in_handle->currentSettings.moduleClkFreq.FreqSrc == BSCD_ClockFreqSrc_eExternalClock) &&
		  (in_handle->currentSettings.moduleClkFreq.ulClkFreq != 0) ) {
/*
				ulVal = BREG_Read32 (in_handle->regHandle, BCHP_VCXO_CTL_MISC_VC0_CTRL);
ulVal &= ~(BCHP_VCXO_CTL_MISC_VC0_CTRL_ARESET_MASK   );*/
				BREG_AtomicUpdate32 (in_handle->regHandle, BCHP_VCXO_CTL_MISC_VC0_CTRL, BCHP_VCXO_CTL_MISC_VC0_CTRL_ARESET_MASK , ulVal );
#if (BCHP_CHIP == 7420)
/*
				ulVal = BREG_Read32 (in_handle->regHandle,  BCHP_VCXO_CTL_MISC_VC1_CTRL);
ulVal &= ~(BCHP_VCXO_CTL_MISC_VC1_CTRL_ARESET_MASK   );*/
				BREG_AtomicUpdate32 (in_handle->regHandle, BCHP_VCXO_CTL_MISC_VC1_CTRL, BCHP_VCXO_CTL_MISC_VC1_CTRL_ARESET_MASK, ulVal );
#elif(BCHP_CHIP == 7340) || (BCHP_CHIP == 7342)|| (BCHP_CHIP==7125)||(BCHP_CHIP==7468)
/*
				ulVal = BREG_Read32 (in_handle->regHandle,  BCHP_VCXO_CTL_MISC_VC0_CTRL);
ulVal &= ~(BCHP_VCXO_CTL_MISC_VC0_CTRL_ARESET_MASK   );*/
				BREG_AtomicUpdate32 (in_handle->regHandle, BCHP_VCXO_CTL_MISC_VC0_CTRL,BCHP_VCXO_CTL_MISC_VC0_CTRL_ARESET_MASK,  ulVal );

#endif
#if (BCHP_CHIP!=7340)&&(BCHP_CHIP!=7125)
/*
				ulVal = BREG_Read32 (in_handle->regHandle,  BCHP_CLK_MISC);
			ulVal &= ~(BCHP_CLK_MISC_SMARTCARD_CLOCK_0_SOURCE_SEL_MASK );*/
				BREG_AtomicUpdate32 (in_handle->regHandle, BCHP_CLK_MISC,BCHP_CLK_MISC_SMARTCARD_CLOCK_0_SOURCE_SEL_MASK,
					ulVal |(1<<BCHP_CLK_MISC_SMARTCARD_CLOCK_0_SOURCE_SEL_SHIFT));
			 }
			 else{  /* internal 27Mhz clock */
			 	/*
			 	ulVal = BREG_Read32 (in_handle->regHandle,  BCHP_CLK_MISC);
			ulVal &= ~(BCHP_CLK_MISC_SMARTCARD_CLOCK_0_SOURCE_SEL_MASK );*/
				BREG_AtomicUpdate32 (in_handle->regHandle, BCHP_CLK_MISC, BCHP_CLK_MISC_SMARTCARD_CLOCK_0_SOURCE_SEL_MASK, 
					ulVal |(2<<BCHP_CLK_MISC_SMARTCARD_CLOCK_0_SOURCE_SEL_SHIFT));

#elif (BCHP_CHIP == 7340)
/*
				ulVal = BREG_Read32 (in_handle->regHandle,  BCHP_CLKGEN_SMARTCARD_CLOCK_CTRL);
				ulVal &= ~(BCHP_CLKGEN_SMARTCARD_CLOCK_CTRL_SMARTCARD_CLOCK_1_SOURCE_SEL_MASK );*/
				BREG_AtomicUpdate32 (in_handle->regHandle, BCHP_CLKGEN_SMARTCARD_CLOCK_CTRL,BCHP_CLKGEN_SMARTCARD_CLOCK_CTRL_SMARTCARD_CLOCK_1_SOURCE_SEL_MASK,
					ulVal |(2<<BCHP_CLKGEN_SMARTCARD_CLOCK_CTRL_SMARTCARD_CLOCK_1_SOURCE_SEL_SHIFT));
			 }
			 else{  /* internal 27Mhz clock */
/*
			 	ulVal = BREG_Read32 (in_handle->regHandle,  BCHP_CLKGEN_SMARTCARD_CLOCK_CTRL);
				ulVal &= ~(BCHP_CLKGEN_SMARTCARD_CLOCK_CTRL_SMARTCARD_CLOCK_1_SOURCE_SEL_MASK );
*/
				BREG_AtomicUpdate32 (in_handle->regHandle, BCHP_CLKGEN_SMARTCARD_CLOCK_CTRL,
					BCHP_CLKGEN_SMARTCARD_CLOCK_CTRL_SMARTCARD_CLOCK_1_SOURCE_SEL_MASK,
					ulVal |(0<<BCHP_CLKGEN_SMARTCARD_CLOCK_CTRL_SMARTCARD_CLOCK_1_SOURCE_SEL_SHIFT));
#elif (BCHP_CHIP==7125)
	
			ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_CLKGEN_SMARTCARD_CLOCK_CTRL);
			ulVal &= ~(BCHP_CLKGEN_SMARTCARD_CLOCK_CTRL_SMARTCARD_CLOCK_1_SOURCE_SEL_MASK );
			BREG_AtomicUpdate32 (in_handle->regHandle, BCHP_CLKGEN_SMARTCARD_CLOCK_CTRL,BCHP_CLKGEN_SMARTCARD_CLOCK_CTRL_SMARTCARD_CLOCK_2_SOURCE_SEL_MASK, 
				ulVal |(2<<BCHP_CLKGEN_SMARTCARD_CLOCK_CTRL_SMARTCARD_CLOCK_1_SOURCE_SEL_SHIFT));
			 }
			 else{  /* internal 27Mhz clock */

			 	ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_CLKGEN_SMARTCARD_CLOCK_CTRL);
			ulVal &= ~(BCHP_CLKGEN_SMARTCARD_CLOCK_CTRL_SMARTCARD_CLOCK_1_SOURCE_SEL_MASK );
			BREG_AtomicUpdate32 (in_handle->regHandle, BCHP_CLKGEN_SMARTCARD_CLOCK_CTRL,BCHP_CLKGEN_SMARTCARD_CLOCK_CTRL_SMARTCARD_CLOCK_2_SOURCE_SEL_MASK, 
				ulVal |(0<<BCHP_CLKGEN_SMARTCARD_CLOCK_CTRL_SMARTCARD_CLOCK_1_SOURCE_SEL_SHIFT));
			 

#endif
}

#elif (BCHP_CHIP==7550)
			 if ((in_handle->currentSettings.moduleClkFreq.FreqSrc == BSCD_ClockFreqSrc_eInternalClock) &&
					   (in_handle->currentSettings.moduleClkFreq.ulClkFreq ==27000000) )
			{
				ulVal = BREG_Read32 (in_handle->regHandle, BCHP_CLK_SC_SC_SEL);
				ulVal &= ~(BCHP_CLK_SC_SC_SEL_SC0_27_MASK );

				BREG_Write32 (in_handle->regHandle, BCHP_CLK_SC_SC_SEL, ulVal |(1<<BCHP_CLK_SC_SC_SEL_SC0_27_SHIFT));
			}

#endif
			break;
#endif
	#if (BCHP_CHIP==7038 || (BCHP_CHIP==7438) ||BCHP_CHIP==7400 || BCHP_CHIP==7401 ||BCHP_CHIP ==7118 ||BCHP_CHIP ==7403||BCHP_CHIP ==7405||BCHP_CHIP==7325 ||BCHP_CHIP==7335||BCHP_CHIP==7420) ||(BCHP_CHIP==7340) ||(BCHP_CHIP==7342) || (BCHP_CHIP==7125)|| (BCHP_CHIP==7550)
		case 1:
			channelHandle->ulRegStartAddr = BCHP_SCB_SC_UART_CMD_1;
#if (BCHP_CHIP==7550)
			if ((in_handle->currentSettings.moduleClkFreq.FreqSrc == BSCD_ClockFreqSrc_eInternalClock) &&
		  		(in_handle->currentSettings.moduleClkFreq.ulClkFreq ==27000000) )
		  	{
				ulVal = BREG_Read32 (in_handle->regHandle, BCHP_CLK_SC_SC_SEL);
				ulVal &= ~(BCHP_CLK_SC_SC_SEL_SC1_27_MASK );

				BREG_Write32 (in_handle->regHandle, BCHP_CLK_SC_SC_SEL, ulVal |(1<<BCHP_CLK_SC_SC_SEL_SC1_27_SHIFT));
				 }
#endif
			 if ((in_handle->currentSettings.moduleClkFreq.FreqSrc == BSCD_ClockFreqSrc_eExternalClock) &&
		  (in_handle->currentSettings.moduleClkFreq.ulClkFreq != 0) ) {

#if(BCHP_CHIP == 7420) ||(BCHP_CHIP==7342)
			ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_CLK_MISC);
			ulVal &= ~(BCHP_CLK_MISC_SMARTCARD_CLOCK_1_SOURCE_SEL_MASK );
			BREG_Write32 (in_handle->regHandle, BCHP_CLK_MISC,
				ulVal |(1<<BCHP_CLK_MISC_SMARTCARD_CLOCK_1_SOURCE_SEL_SHIFT));
			 }
			 else{  /* internal 27Mhz clock */

			 	ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_CLK_MISC);
			ulVal &= ~(BCHP_CLK_MISC_SMARTCARD_CLOCK_1_SOURCE_SEL_MASK );
			BREG_Write32 (in_handle->regHandle, BCHP_CLK_MISC,
				ulVal |(2<<BCHP_CLK_MISC_SMARTCARD_CLOCK_1_SOURCE_SEL_SHIFT));

#elif (BCHP_CHIP == 7340)
				ulVal = BREG_Read32 (in_handle->regHandle,  BCHP_CLKGEN_SMARTCARD_CLOCK_CTRL);
				ulVal &= ~(BCHP_CLKGEN_SMARTCARD_CLOCK_CTRL_SMARTCARD_CLOCK_2_SOURCE_SEL_MASK );
				BREG_Write32 (in_handle->regHandle, BCHP_CLKGEN_SMARTCARD_CLOCK_CTRL, ulVal |(2<<BCHP_CLKGEN_SMARTCARD_CLOCK_CTRL_SMARTCARD_CLOCK_2_SOURCE_SEL_SHIFT));
#elif (BCHP_CHIP==7125)
	 if ((in_handle->currentSettings.moduleClkFreq.FreqSrc == BSCD_ClockFreqSrc_eExternalClock) &&
		  (in_handle->currentSettings.moduleClkFreq.ulClkFreq != 0) ) {

			ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_CLKGEN_SMARTCARD_CLOCK_CTRL);
			ulVal &= ~(BCHP_CLKGEN_SMARTCARD_CLOCK_CTRL_SMARTCARD_CLOCK_2_SOURCE_SEL_MASK );

			BREG_AtomicUpdate32 (in_handle->regHandle,  BCHP_CLKGEN_SMARTCARD_CLOCK_CTRL,
				BCHP_CLKGEN_SMARTCARD_CLOCK_CTRL_SMARTCARD_CLOCK_2_SOURCE_SEL_MASK, 
			    ulVal |(2<<BCHP_CLKGEN_SMARTCARD_CLOCK_CTRL_SMARTCARD_CLOCK_2_SOURCE_SEL_SHIFT));
			 }
			 else{  /* internal 27Mhz clock */

			 	ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_CLKGEN_SMARTCARD_CLOCK_CTRL);
			ulVal &= ~(BCHP_CLKGEN_SMARTCARD_CLOCK_CTRL_SMARTCARD_CLOCK_2_SOURCE_SEL_MASK );
			BREG_AtomicUpdate32 (in_handle->regHandle,  BCHP_CLKGEN_SMARTCARD_CLOCK_CTRL,
				BCHP_CLKGEN_SMARTCARD_CLOCK_CTRL_SMARTCARD_CLOCK_2_SOURCE_SEL_MASK, 
			    ulVal |(0<<BCHP_CLKGEN_SMARTCARD_CLOCK_CTRL_SMARTCARD_CLOCK_2_SOURCE_SEL_SHIFT));
			 }
#endif
			 	}
			break;
	#endif
	#if (BCHP_CHIP==7038 || (BCHP_CHIP==7438) || BCHP_CHIP == 7400)
		case 2:
			channelHandle->ulRegStartAddr = BCHP_SCC_SC_UART_CMD_1;
			break;
	#endif
		default:
			channelHandle->ulRegStartAddr = BCHP_SCA_SC_UART_CMD_1;
			break;
	}
	BKNI_EnterCriticalSection();
	BREG_Write32(
		channelHandle->moduleHandle->regHandle,
		(channelHandle->ulRegStartAddr + BSCD_P_INTR_EN_1),
		 0);

	BREG_Write32(
		channelHandle->moduleHandle->regHandle,
		(channelHandle->ulRegStartAddr + BSCD_P_INTR_EN_2),
		 0);
	channelHandle->ulStatus1 = 0x00;
	channelHandle->ulStatus2 = 0x00;
	channelHandle->ulIntrStatus1 = 0x00;
	channelHandle->ulIntrStatus2 = 0x00;
	BKNI_LeaveCriticalSection();

	/*  Enable smartcard interrupt   */
	switch (in_channelNo) {
#if (BCHP_CHIP!=7125)||((BCHP_CHIP==7125)&& (BCHP_VER>=BCHP_VER_C0))
		case 0:
			#if (BCHP_CHIP == 3548) || (BCHP_CHIP==3556)|| (BCHP_CHIP==35230) || (BCHP_CHIP==35233)
				intId = BCHP_INT_ID_CREATE(BCHP_SCIRQ0_SCIRQEN, BCHP_SCIRQ0_SCIRQEN_sca_irqen_SHIFT);
			#else
				intId = BCHP_INT_ID_sca_irqen;
			#endif
			BREG_Write32( in_handle->regHandle, BCHP_SCIRQ0_SCIRQEN,
					(BCHP_SCIRQ0_SCIRQEN_sca_irqen_MASK |
					  BREG_Read32(in_handle->regHandle,  BCHP_SCIRQ0_SCIRQEN)) );
			break;
#endif
#if (BCHP_CHIP==7038 || (BCHP_CHIP==7438) ||BCHP_CHIP==7400 || BCHP_CHIP==7401 ||BCHP_CHIP ==7118 ||BCHP_CHIP ==7403||BCHP_CHIP ==7405||BCHP_CHIP==7325 ||BCHP_CHIP==7335||BCHP_CHIP==7420) ||(BCHP_CHIP==7340) ||(BCHP_CHIP==7342)|| (BCHP_CHIP==7125)|| (BCHP_CHIP==7550)  /* 7038 and 7401 have 2nd smartcard */
		case 1:
			intId = BCHP_INT_ID_scb_irqen;
			BREG_Write32(  in_handle->regHandle, BCHP_SCIRQ0_SCIRQEN,
					(BCHP_SCIRQ0_SCIRQEN_scb_irqen_MASK |
					  BREG_Read32(in_handle->regHandle,  BCHP_SCIRQ0_SCIRQEN)) );
			break;
#endif
#if (BCHP_CHIP==7038 || (BCHP_CHIP==7438) || BCHP_CHIP == 7400)  /* 7038,7400 has 3rd smartcard */
			case 2:
				intId = BCHP_INT_ID_scc_irqen;
				BREG_Write32(  in_handle->regHandle,  BCHP_SCIRQ0_SCIRQEN,
						(BCHP_SCIRQ0_SCIRQEN_scc_irqen_MASK |
						  BREG_Read32(in_handle->regHandle,  BCHP_SCIRQ0_SCIRQEN)) );
			break;
#endif


	}


	BSCD_P_CHECK_ERR_CODE_FUNC( errCode, BINT_CreateCallback(
			&(channelHandle->channelIntCallback), in_handle->interruptHandle, intId,
			BSCD_Channel_P_IntHandler_isr, (void *) channelHandle, 0x00 ) );
	BSCD_P_CHECK_ERR_CODE_FUNC( errCode, BINT_EnableCallback(channelHandle->channelIntCallback) );


	BDBG_MSG(("in_channelNo = %d\n", in_channelNo));
	BDBG_MSG(("channelHandle->ulRegStartAddr = 0x%x\n", channelHandle->ulRegStartAddr));

	if (inp_channelDefSettings != NULL)
		BSCD_Channel_SetParameters(
			channelHandle,
			inp_channelDefSettings);

	else {
		BSCD_Channel_SetParameters(
			channelHandle,
			&BSCD_defScdChannelSettings);
	}
		/* set pin muxing */
	if( channelHandle->currentChannelSettings.setPinmux){

		switch (in_channelNo) {
#if (BCHP_CHIP!=7125)||((BCHP_CHIP==7125)&& (BCHP_VER>=BCHP_VER_C0))
			case 0:

			/* Pin muxing */

#if ((BCHP_CHIP==7038) &&  (BCHP_VER >= BCHP_VER_B0) && (BCHP_VER < BCHP_VER_C0) )

			/* make sure you are using B0 header file bchp_sun_top_ctrl.h */
			ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4);
			ulVal &= ~(BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_sc_vcc_0_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_sc_pres_0_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_sc_rst_0_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_sc_clk_0_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_sc_io_0_MASK);
			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4,
				ulVal );
			BDBG_MSG(( "7038 Bx, need to switch io pin\n" ));


#elif ((BCHP_CHIP==7038) &&  (BCHP_VER >= BCHP_VER_C0) && (BCHP_VER < BCHP_VER_D0)) || (BCHP_CHIP==7438)
			/* make sure you are using C0 header file bchp_sun_top_ctrl.h */
			ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4);
			ulVal &= ~(BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_sc_vcc_0_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_sc_pres_0_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_sc_rst_0_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_sc_clk_0_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_sc_io_0_MASK);
			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4,
				ulVal );
			BDBG_MSG(( "7038 C0, need to switch io pin\n" ));
#endif

#if( BCHP_CHIP == 3560 )
			/* make sure you are using 3560 header file bchp_sun_top_ctrl.h */
			ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6);
			ulVal &= ~(BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_61_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_60_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_59_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_58_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_57_MASK );
			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6,
				(ulVal |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_61_SHIFT) |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_60_SHIFT) |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_59_SHIFT) |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_58_SHIFT) |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_57_SHIFT ) ));
			BDBG_MSG(( "3560 , need to switch io pin\n" ));
#endif

#if( BCHP_CHIP == 3563 )
			/* make sure you are using 3560 header file bchp_sun_top_ctrl.h */
			ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11);
			ulVal &= ~(BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_61_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_60_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_59_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_58_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_57_MASK );
			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11,
				(ulVal |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_61_SHIFT) |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_60_SHIFT) |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_59_SHIFT) |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_58_SHIFT) |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_57_SHIFT ) ));
			BDBG_MSG(( "3563 , need to switch io pin\n" ));
#endif

#if( (BCHP_CHIP == 3548 ) || (BCHP_CHIP==3556) )
			/* make sure you are using 3560 header file bchp_sun_top_ctrl.h */
			ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5);
			ulVal &= ~(BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5_gpio_28_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5_gpio_27_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5_gpio_26_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5_gpio_25_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5_gpio_24_MASK );
			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5,
				(ulVal |
					(0x00000003 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5_gpio_28_SHIFT) |
					(0x00000003 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5_gpio_27_SHIFT) |
					(0x00000003 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5_gpio_26_SHIFT) |
					(0x00000003 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5_gpio_25_SHIFT) |
					(0x00000003 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5_gpio_24_SHIFT ) ));
			BDBG_MSG(( "3548 , need to switch io pin\n" ));
#endif

#if( BCHP_CHIP == 3546 )
			/* make sure you are using 3560 header file bchp_sun_top_ctrl.h */
			ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4);
			ulVal &= ~(BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_38_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_36_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_34_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_33_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_31_MASK );
			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4,
				(ulVal |
					(0x00000002 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_38_SHIFT) |
					(0x00000002 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_36_SHIFT) |
					(0x00000002 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_34_SHIFT) |
					(0x00000002 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_33_SHIFT) |
					(0x00000002 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_31_SHIFT ) ));
			BDBG_MSG(( "3546 , need to switch io pin\n" ));
#endif

#if( BCHP_CHIP == 7400)
#if ((BCHP_VER >= BCHP_VER_A0) && (BCHP_VER < BCHP_VER_B0) )
			/* make sure you are using 7400 header file bchp_sun_top_ctrl.h */
			ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8);
			ulVal &= ~(BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_064) |  /*sc_vcc_0*/
					BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_063) |  /*sc_pres_0*/
					BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_062) |  /*sc_rst_0*/
					BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_061) |  /*sc_clk_0*/
					BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_060));  /*sc_io_0*/
			ulVal |= BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_064, 1) |  /*sc_vcc_0*/
					BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_063, 1) |  /*sc_pres_0*/
					BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_062, 1) |  /*sc_rst_0*/
					BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_061, 1) |  /*sc_clk_0*/
					BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_060, 1);  /*sc_io_0*/
			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8, ulVal );

#else
			/* make sure you are using 7400 header file bchp_sun_top_ctrl.h */
			ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12);
			ulVal &= ~(BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_108) |  /*sc_vcc_0*/
					BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_107) |  /*sc_pres_0*/
					BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_106) |  /*sc_rst_0*/
					BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_105) |  /*sc_clk_0*/
					BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_104));  /*sc_io_0*/
			ulVal |= BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_108, 1) |  /*sc_vcc_0*/
					BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_107, 1) |  /*sc_pres_0*/
					BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_106, 1) |  /*sc_rst_0*/
					BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_105, 1) |  /*sc_clk_0*/
					BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_104, 1);  /*sc_io_0*/
			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12, ulVal );
#endif
			BDBG_MSG(( "7400 , need to switch io pin\n" ));
#endif

#if( BCHP_CHIP == 7401)

#if ((BCHP_VER >= BCHP_VER_A0) && (BCHP_VER < BCHP_VER_B0) )
			/* make sure you are using 7401 header file bchp_sun_top_ctrl.h */
			ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4);
			ulVal &= ~(BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_sc_vcc_0_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_sc_pres_0_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_sc_rst_0_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_sc_clk_0_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_sc_io_0_MASK );
			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4, ulVal );
			BDBG_MSG(( "7401 , need to switch io pin\n" ));
#elif ((BCHP_VER >= BCHP_VER_B0)  )
			/* make sure you are using 7401 header file bchp_sun_top_ctrl.h */
			ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6);
			ulVal &= ~(BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_sc_vcc_0_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_sc_pres_0_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_sc_rst_0_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_sc_clk_0_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_sc_io_0_MASK );
			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6, ulVal );
			BDBG_MSG(( "7401 , need to switch io pin\n" ));
#endif
#endif

#if( BCHP_CHIP == 7118)
			/* make sure you are using 7118 header file bchp_sun_top_ctrl.h */
		ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_2);
			ulVal &= ~(BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_2, sc_vcc) |  /*sc_vcc_0*/
					BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_2, sc_pres) |  /*sc_pres_0*/
					BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_2, sc_rst) |  /*sc_rst_0*/
					BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_2, sc_clk_out) |  /*sc_clk_0*/
					BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_2, sc_io));  /*sc_io_0*/
			ulVal |= BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_2, sc_vcc, 0) |  /*sc_vcc_0*/
					BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_2, sc_pres, 0) |  /*sc_pres_0*/
					BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_2, sc_rst, 0) |  /*sc_rst_0*/
					BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_2, sc_clk_out, 0) |  /*sc_clk_0*/
					BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_2, sc_io, 0);  /*sc_io_0*/

			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_2, ulVal );

			BDBG_MSG(( "7118 , need to switch io pin\n" ));

#endif
#if( BCHP_CHIP == 7403)
			ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6);
			ulVal &= ~BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_sc_vcc_0_MASK ;
			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6, ulVal );
			ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5);
			ulVal &= ~(BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5_sc_pres_0_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5_sc_rst_0_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5_sc_clk_0_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5_sc_io_0_MASK );
			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5, ulVal );
#endif
#if( BCHP_CHIP == 7405)
			ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12);
			ulVal &= ~(	BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_108_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_107_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_106_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_105_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_104_MASK );
			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12,
				(ulVal |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_108_SHIFT) |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_107_SHIFT) |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_106_SHIFT) |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_105_SHIFT )|
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_104_SHIFT )));

			BDBG_MSG(( "7405 , need to switch io pin\n" ));

#endif
#if( BCHP_CHIP == 7335)
#if 1    /* (BCHP_VER >= BCHP_VER_A1) */ /*remove A0 support under Sean's request */
			ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6);
			ulVal &= ~(	BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_029_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_028_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_027_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_026_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_025_MASK );
			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6,
				(ulVal |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_029_SHIFT) |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_028_SHIFT) |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_027_SHIFT) |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_026_SHIFT )|
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_025_SHIFT )));
#else
			ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4);
			ulVal &= ~(	BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_006_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_005_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_004_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_003_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_002_MASK );
			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4,
				(ulVal |
					(0x00000003 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_006_SHIFT) |
					(0x00000003 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_005_SHIFT) |
					(0x00000003 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_004_SHIFT) |
					(0x00000003 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_003_SHIFT )|
					(0x00000003 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_002_SHIFT )));
#endif

			BDBG_MSG(( "7335 , need to switch io pin\n" ));

#endif
#if( BCHP_CHIP == 7325)
			ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7);
			ulVal &= ~(	BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_57_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_56_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_55_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_54_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_53_MASK );
			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7,
				(ulVal |
					(0x00000004 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_57_SHIFT) |
					(0x00000004 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_56_SHIFT) |
					(0x00000004 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_55_SHIFT) |
					(0x00000004 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_54_SHIFT )|
					(0x00000004 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_53_SHIFT )));

			BDBG_MSG(( "7325 , need to switch io pin\n" ));

#endif
#if( BCHP_CHIP == 7420)
			ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17);
			ulVal &= ~(	BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_085_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_084_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_083_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_082_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_081_MASK );
			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17,
				(ulVal |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_085_SHIFT) |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_084_SHIFT) |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_083_SHIFT) |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_082_SHIFT )|
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_081_SHIFT )));

			BDBG_MSG(( "7420 , need to switch io pin\n" ));

#endif
#if( BCHP_CHIP == 7340)
			ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11);
			ulVal &= ~(	BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_57_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_56_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_55_MASK );
			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11,
				(ulVal |
					(0x00000004 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_57_SHIFT) |
					(0x00000004 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_56_SHIFT) |
					(0x00000004 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_55_SHIFT )));
			ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10);
			ulVal &= ~(	BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_54_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_53_MASK );
			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10,
				(ulVal |
					(0x00000004 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_54_SHIFT) |
					(0x00000004 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_53_SHIFT )));
			BDBG_MSG(( "7340 , need to switch io pin\n" ));

#endif
#if( BCHP_CHIP == 7342)
			ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8);
			ulVal &= ~(	BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_006_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_005_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_004_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_003_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_002_MASK );
			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8,
				(ulVal |
					(0x00000003 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_006_SHIFT) |
					(0x00000003 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_005_SHIFT) |
					(0x00000003 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_004_SHIFT) |
					(0x00000003 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_003_SHIFT )|
					(0x00000003 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_002_SHIFT )));

			BDBG_MSG(( "7342 , need to switch io pin\n" ));
#endif
#if( BCHP_CHIP == 7550)
			ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10);
			ulVal &= ~(	BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_72_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_71_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_70_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_69_MASK );
			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10,
				(ulVal |
					(0x00000002 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_72_SHIFT) |
					(0x00000002 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_71_SHIFT) |
					(0x00000002 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_70_SHIFT) |
					(0x00000002 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10_gpio_69_SHIFT )));
			ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11);
			ulVal &= ~(	BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_73_MASK );
			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11,
				(ulVal |
					(0x00000002 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_73_SHIFT)));
			BDBG_MSG(( "7550 , need to switch io pin\n" ));
#endif

#if( BCHP_CHIP == 7468)
			ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7);
			ulVal &= ~(	BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_40_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_39_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_38_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_37_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_36_MASK );
			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7,
				(ulVal |
					(0x00000002 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_40_SHIFT) |
					(0x00000002 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_39_SHIFT) |
					(0x00000002 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_38_SHIFT) |
					(0x00000002 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_37_SHIFT) |
					(0x00000002 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_36_SHIFT )));

			BDBG_MSG(( "7468 , need to switch io pin\n" ));
#endif


#if( BCHP_CHIP == 7125)
			ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7);
			ulVal &= ~(	BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_sc0_vcc_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_sc0_pres_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_sc0_rst_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_sc0_clk_out_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_sc0_io_MASK );
			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7,
				(ulVal |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_sc0_vcc_SHIFT) |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_sc0_pres_SHIFT) |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_sc0_rst_SHIFT) |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_sc0_clk_out_SHIFT) |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_sc0_io_SHIFT )));
			BDBG_MSG(( "7125 , need to switch io pin\n" ));

#endif
#if( BCHP_CHIP == 35230)
						ulVal = BREG_Read32 (in_handle->regHandle,
								BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13);
						ulVal &= ~( BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13_tdatd_5_MASK |
									BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13_tdatd_4_MASK |
									BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13_tdatd_3_MASK |
									BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13_tdatd_2_MASK |
									BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13_tdatd_1_MASK );
						BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13,
							(ulVal |
								(0x00000005 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13_tdatd_5_SHIFT) |
								(0x00000005 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13_tdatd_4_SHIFT) |
								(0x00000005 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13_tdatd_3_SHIFT) |
								(0x00000005 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13_tdatd_2_SHIFT) |
								(0x00000005 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13_tdatd_1_SHIFT )));
						BDBG_MSG(( "3523x , need to switch io pin\n" ));
			
#endif

#if ( BCHP_CHIP == 35233)
#if( BCHP_VER == BCHP_VER_A0)
						ulVal = BREG_Read32 (in_handle->regHandle,
								BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9);
						ulVal &= ~( BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_pkt6_data5_MASK |
									BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_pkt6_data4_MASK |
									BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_pkt6_data3_MASK |
									BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_pkt6_data2_MASK |
									BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_pkt6_data1_MASK );
						BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13,
							(ulVal |
								(0x00000004 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_pkt6_data5_SHIFT) |
								(0x00000004 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_pkt6_data4_SHIFT) |
								(0x00000004 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_pkt6_data3_SHIFT) |
								(0x00000004 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_pkt6_data2_SHIFT) |
								(0x00000004 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_pkt6_data1_SHIFT )));
						BDBG_MSG(( "35233 A0, need to switch io pin\n" ));
#else /* end of 35233 A0 code start of >= B0 */
						ulVal = BREG_Read32 (in_handle->regHandle,
								BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9);
						ulVal &= ~( BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_pkt6_data5_MASK |
									BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_pkt6_data4_MASK |
									BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_pkt6_data3_MASK |
									BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_pkt6_data2_MASK );
						BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9,
							(ulVal |
								(0x00000003 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_pkt6_data5_SHIFT) |
								(0x00000003 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_pkt6_data4_SHIFT) |
								(0x00000003 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_pkt6_data3_SHIFT) |
								(0x00000003 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_pkt6_data2_SHIFT) ));


						ulVal = BREG_Read32 (in_handle->regHandle,
								BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8);
						ulVal &= ~( BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_pkt6_data1_MASK );
						BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8,
							(ulVal | (0x00000003 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_pkt6_data1_SHIFT )));

						BDBG_MSG(( "35233 B0, need to switch io pin\n" ));
#endif /* end of 35233 B0 code */
#endif

			break;
#endif
#if (BCHP_CHIP==7038 || (BCHP_CHIP==7438) ||BCHP_CHIP==7400 || BCHP_CHIP==7401 ||BCHP_CHIP ==7118 ||BCHP_CHIP ==7403||BCHP_CHIP ==7405||BCHP_CHIP==7325 ||BCHP_CHIP==7335||BCHP_CHIP==7420) ||(BCHP_CHIP==7340) ||(BCHP_CHIP==7342)|| (BCHP_CHIP==7125)|| (BCHP_CHIP==7550)  /* 7038 and 7401 have 2nd smartcard */
		case 1:
			intId = BCHP_INT_ID_scb_irqen;
			BREG_Write32(  in_handle->regHandle, BCHP_SCIRQ0_SCIRQEN,
					(BCHP_SCIRQ0_SCIRQEN_scb_irqen_MASK |
					  BREG_Read32(in_handle->regHandle,  BCHP_SCIRQ0_SCIRQEN)) );

			/* Pin muxing */
#if ((BCHP_CHIP==7038) &&  (BCHP_VER >= BCHP_VER_B0) && (BCHP_VER < BCHP_VER_C0))
			/* make sure you are using B0 header file bchp_sun_top_ctrl.h */
			ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4);
			ulVal &= ~(BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_sc_vcc_1_MASK |
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_sc_pres_1_MASK |
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_sc_rst_1_MASK |
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_sc_clk_1_MASK |
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_sc_io_1_MASK);
			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4,
				ulVal );
			BDBG_MSG(( "7038 Bx, need to switch io pin\n" ));


#elif ((BCHP_CHIP==7038) && (BCHP_VER >= BCHP_VER_C0) && (BCHP_VER < BCHP_VER_D0) ) || (BCHP_CHIP==7438)
			/* make sure you are using C0 header file bchp_sun_top_ctrl.h */
			ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4);
			ulVal &= ~(BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_sc_clk_1_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_sc_io_1_MASK) ;
			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4,
				ulVal );

			ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5);
			ulVal &= ~(BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5_sc_rst_1_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5_sc_pres_1_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5_sc_vcc_1_MASK
						 );
			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5,
				ulVal );

			BDBG_MSG(( "7038 C0, need to switch io pin\n" ));
#endif

#if( BCHP_CHIP == 7400)
#if ((BCHP_VER >= BCHP_VER_A0) && (BCHP_VER < BCHP_VER_B0) )
			/* make sure you are using 7400 header file bchp_sun_top_ctrl.h */
			ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12);

			ulVal &= ~(BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_105) |  /*sc_vcc_1*/
					BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_104) |  /*sc_pres_1*/
					BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_103) |  /*sc_rst_1*/
					BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_102) |  /*sc_clk_1*/
					BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_101));  /*sc_io_1*/
			ulVal |= BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_105, 1) |  /*sc_vcc_1*/
					BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_104, 1) |  /*sc_pres_1*/
					BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_103, 1) |  /*sc_rst_1*/
					BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_102, 1) |  /*sc_clk_1*/
					BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_12, gpio_101, 1);  /*sc_io_1*/

			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12, ulVal);


#else
           /* make sure you are using 7400 header file bchp_sun_top_ctrl.h */
			ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10);

			ulVal &= ~(BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_085) |  /*sc_vcc_1*/
					BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_084) |  /*sc_pres_1*/
					BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_083) |  /*sc_rst_1*/
					BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_082) |  /*sc_clk_1*/
					BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_081));  /*sc_io_1*/
			ulVal |= BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_085, 1) |  /*sc_vcc_1*/
					BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_084, 1) |  /*sc_pres_1*/
					BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_083, 1) |  /*sc_rst_1*/
					BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_082, 1) |  /*sc_clk_1*/
					BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_10, gpio_081, 1);  /*sc_io_1*/

			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10, ulVal);
#endif
			BDBG_MSG(( "7400 , need to switch io pin\n" ));
#endif

#if( BCHP_CHIP == 7401)
#ifdef BSCD_ALT_SC_1 	/*There are two groups of pins to contol 2nd SmartCard interface in 7401 */
			#if ((BCHP_VER >= BCHP_VER_A0) && (BCHP_VER < BCHP_VER_B0) )
	/* make sure you are using 7401 header file bchp_sun_top_ctrl.h */
			ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7);
			ulVal &= ~(BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_35_MASK |			/*alt_sc_vcc_1*/
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_29_MASK |		/*alt_sc_pres_1 */
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_33_MASK );		/*alt_sc_clk_1*/
			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7,
				(ulVal |
					(0x00000002 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_35_SHIFT) |
					(0x00000002 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_29_SHIFT) |
					(0x00000002 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_33_SHIFT ) ));

			ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9);
			ulVal &= ~(	BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_52_MASK |		/*alt_sc_rst_1*/
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_54_MASK );		/*alt_sc_io_1*/
			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9,
				(ulVal |
					(0x00000002 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_52_SHIFT) |
					(0x00000002 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_54_SHIFT ) ));
	#elif ((BCHP_VER >= BCHP_VER_B0)  )
		ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9);
		ulVal &= ~(BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_35_MASK |			/*alt_sc_vcc_1*/
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_29_MASK |		/*alt_sc_pres_1 */
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_33_MASK );		/*alt_sc_clk_1*/
		BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9,
				(ulVal |
					(0x00000002 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_35_SHIFT) |
					(0x00000002 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_29_SHIFT) |
					(0x00000002 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_33_SHIFT ) ));
		ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11);
		ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11);
		ulVal &= ~(	BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_52_MASK |		/*alt_sc_rst_1*/
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_54_MASK );
		BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11,
				(ulVal |
					(0x00000002 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_52_SHIFT) |
					(0x00000002 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_54_SHIFT ) ));
	#endif

#if ((BCHP_VER >= BCHP_VER_A0) && (BCHP_VER < BCHP_VER_B0) )
			/* make sure you are using 7401 header file bchp_sun_top_ctrl.h */
			ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4);
			ulVal &= ~(	BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_01_MASK |		/*sc_pres_1*/
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_03_MASK );		/*sc_vcc_1*/
			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4,
				(ulVal |
					(0x00000002 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_01_SHIFT) |
					(0x00000002 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_03_SHIFT ) ));

			ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5);
			ulVal &= ~(BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5_gpio_04_MASK |			/*sc_clk_1*/
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5_gpio_07_MASK |		/*sc_io_1 */
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5_gpio_08_MASK );		/*sc_rst_1*/
			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5,
				(ulVal |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5_gpio_04_SHIFT) |
					(0x00000002 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5_gpio_07_SHIFT) |
					(0x00000002 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5_gpio_08_SHIFT ) ));
#elif ((BCHP_VER >= BCHP_VER_B0)  )
			/* make sure you are using 7401 header file bchp_sun_top_ctrl.h */
			ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6);
			ulVal &= ~(	BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_04_MASK |		/*sc_clk_1*/
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_03_MASK |		/*sc_vcc_1*/
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_01_MASK );		/*sc_pres_1*/
			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6,
				(ulVal |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_04_SHIFT) |
					(0x00000002 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_03_SHIFT )|
					(0x00000002 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_01_SHIFT )));

			ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7);
			ulVal &= ~(
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_07_MASK |		/*sc_io_1 */
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_08_MASK );		/*sc_rst_1*/
			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7,
				(ulVal |
					(0x00000002 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_07_SHIFT) |
					(0x00000002 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_08_SHIFT ) ));
#endif
#endif
			BDBG_MSG(( "7401 , need to switch io pin\n" ));

#endif
#if( BCHP_CHIP == 7403)
			/* make sure you are using 7403 header file bchp_sun_top_ctrl.h */
			ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6);
			ulVal &= ~(	BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_08_MASK |		/*sc_rst_1*/
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_07_MASK |            /* sc_io_1 */
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_04_MASK |            /* sc_clk_1 */
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_03_MASK |		/*sc_vcc_1*/
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_01_MASK );		/*sc_pres_1*/
			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6,
				(ulVal |
					(0x00000002 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_08_SHIFT) |
					(0x00000002 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_07_SHIFT) |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_04_SHIFT) |
					(0x00000002 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_03_SHIFT )|
					(0x00000002 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_6_gpio_01_SHIFT )));
			BDBG_MSG(( "7403 , need to switch io pin\n" ));

#endif

#if( BCHP_CHIP == 7118)
			/* make sure you are using 7400 header file bchp_sun_top_ctrl.h */
			ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8);

			ulVal &= ~(BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_29) |  /*sc_rst2*/
					BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_28) |  /*sc_clk2*/
					BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_27) );  /*sc_io2*/

			ulVal |= BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_29, 4) |  /*sc_rst2*/
					BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_28, 4) |  /*sc_clk2*/
					BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_27, 4);   /*sc_io2*/

			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8, ulVal);

			ulVal &= ~(BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_31) |  /*sc_vcc2*/
					BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_30)) ;  /*sc_pres2*/

			ulVal |= BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_31, 4) |  /*sc_vcc2*/
					BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_30, 4) ;  /*sc_pres2*/


			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8, ulVal);

			BDBG_MSG(( "7118 , need to switch io pin\n" ));

#endif

#if( BCHP_CHIP == 7405)
			ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7);
			ulVal &= ~(	BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_046_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_047_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_048_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_049_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_050_MASK );
			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7,
				(ulVal |
					(0x00000004 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_046_SHIFT) |
					(0x00000004 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_047_SHIFT) |
					(0x00000004 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_048_SHIFT) |
					(0x00000004 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_049_SHIFT )|
					(0x00000004 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7_gpio_050_SHIFT )));

			BDBG_MSG(( "7405 , need to switch io pin\n" ));

#endif
#if( BCHP_CHIP == 7335)
			ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8);
			ulVal &= ~(	BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_046_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_045_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_044_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_043_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_042_MASK );
			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8,
				(ulVal |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_046_SHIFT) |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_045_SHIFT) |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_044_SHIFT) |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_043_SHIFT )|
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_042_SHIFT )));

			BDBG_MSG(( "7335 , need to switch io pin\n" ));

#endif
#if( BCHP_CHIP == 7325)
			ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8);
			ulVal &= ~(	BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_65_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_64_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_63_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_62_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_61_MASK );
			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8,
				(ulVal |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_65_SHIFT) |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_64_SHIFT) |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_63_SHIFT) |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_62_SHIFT )|
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_61_SHIFT )));

			BDBG_MSG(( "7355 , need to switch io pin\n" ));

#endif
#if( BCHP_CHIP == 7420)

#if 0
/* another option */
			ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13);
			ulVal &= ~(	BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13_gpio_048_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13_gpio_049_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13_gpio_050_MASK );
			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13,
				(ulVal |
					(0x00000004 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13_gpio_048_SHIFT) |
					(0x00000004 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13_gpio_049_SHIFT )|
					(0x00000004 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13_gpio_050_SHIFT )));

			ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12);
			ulVal &= ~(	BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_047_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_046_MASK );
			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12,
				(ulVal |
					(0x00000004 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_047_SHIFT) |
					(0x00000004 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_046_SHIFT )));
#endif

			ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17);
			ulVal &= ~(	BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_085_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_084_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_083_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_082_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_081_MASK );
			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17,
				(ulVal |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_085_SHIFT) |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_084_SHIFT) |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_083_SHIFT) |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_082_SHIFT )|
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17_gpio_081_SHIFT )));

			BDBG_MSG(( "7420 , need to switch io pin\n" ));
#endif
#if( BCHP_CHIP == 7340)
			ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12);
			ulVal &= ~(	BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_65_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_64_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_63_MASK );
			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12,
				(ulVal |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_65_SHIFT) |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_64_SHIFT) |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_gpio_63_SHIFT )));
			ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11);
			ulVal &= ~(	BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_62_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_61_MASK );
			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11,
				(ulVal |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_62_SHIFT) |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_gpio_61_SHIFT )));
			BDBG_MSG(( "7340 , need to switch io pin\n" ));

#endif
#if( BCHP_CHIP == 7342)
			ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9);
			ulVal &= ~(	BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_014_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_013_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_012_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_011_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_010_MASK );
			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9,
				(ulVal |
					(0x00000003 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_014_SHIFT) |
					(0x00000003 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_013_SHIFT) |
					(0x00000003 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_012_SHIFT) |
					(0x00000003 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_011_SHIFT )|
					(0x00000003 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_010_SHIFT )));

			BDBG_MSG(( "7342 , need to switch io pin\n" ));
#endif
#if( BCHP_CHIP == 7125)
	ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14);
	ulVal &= ~( BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14_gpio_46_MASK |
							BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14_gpio_45_MASK |
							BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14_gpio_44_MASK );
	BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14,
				(ulVal |
					(0x00000003 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14_gpio_46_SHIFT) |
					(0x00000003 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14_gpio_45_SHIFT) |
					(0x00000003 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14_gpio_44_SHIFT )));
	ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15);
	ulVal &= ~BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15_gpio_56_MASK ;
	BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15,
					(ulVal | 0x00000003 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15_gpio_56_SHIFT ));
	ulVal = BREG_Read32 (in_handle->regHandle,
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16);
		ulVal &= ~BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16_gpio_57_MASK ;
		BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16,
						(ulVal | 0x00000003 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16_gpio_57_SHIFT ));

	BDBG_MSG(( "7125 , need to switch io pin\n" ));
#endif
#if( BCHP_CHIP == 7550)
						ulVal = BREG_Read32 (in_handle->regHandle,
								BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8);
						ulVal &= ~( BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_56_MASK |
									BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_55_MASK |
									BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_54_MASK );
						BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8,
							(ulVal |
								(0x00000002 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_56_SHIFT) |
								(0x00000002 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_55_SHIFT) |
								(0x00000002 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8_gpio_54_SHIFT )));
						ulVal = BREG_Read32 (in_handle->regHandle,
								BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9);
						ulVal &= ~( BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_58_MASK |
									BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_57_MASK);
						BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9,
							(ulVal |
								(0x00000002 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_58_SHIFT)|
								(0x00000002 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9_gpio_57_SHIFT)));
						BDBG_MSG(( "7550 , need to switch io pin\n" ));
#endif
#if( BCHP_CHIP == 35230)
						ulVal = BREG_Read32 (in_handle->regHandle,
								BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12);
						ulVal &= ~( BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_tdata_5_MASK |
									BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_tdata_4_MASK |
									BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_tdata_3_MASK  );
						BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12,
							(ulVal |
								(0x00000004 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_tdat2_5_SHIFT) |
								(0x00000004 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_tdat2_4_SHIFT) |
								(0x00000004 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_tdat2_3_SHIFT )));
						ulVal = BREG_Read32 (in_handle->regHandle,
								BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11);
						ulVal &= ~( BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_tdata_2_MASK |
									BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_tdata_1_MASK  );
						BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11,
							(ulVal |
								(0x00000004 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_tdat2_2_SHIFT) |
								(0x00000004 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_tdat2_1_SHIFT )));
						
						BDBG_MSG(( "35230 , need to switch io pin\n" ));
						
#endif

#if( BCHP_CHIP == 35233)
						ulVal = BREG_Read32 (in_handle->regHandle,
								BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12);
						ulVal &= ~( BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_spi3_out_MASK |
									BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_spi3_cs0_MASK |
									BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_spi3_in_MASK  );
						BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12,
							(ulVal |
								(0x00000004 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_spi3_out_SHIFT) |
								(0x00000004 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_spi3_cs0_SHIFT) |
								(0x00000004 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_12_spi3_in_SHIFT )));
						ulVal = BREG_Read32 (in_handle->regHandle,
								BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11);
						ulVal &= ~( BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_spi4_clk_MASK |
									BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_spi4_out_MASK  );
						BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11,
							(ulVal |
								(0x00000004 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_spi4_clk_SHIFT) |
								(0x00000004 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11_spi4_out_SHIFT )));
						
						BDBG_MSG(( "35233 , need to switch io pin\n" ));
						
#endif
			break;
#endif  /* 7038 & 7401 have 2nd smartcard */


#if (BCHP_CHIP==7038 || (BCHP_CHIP==7438) || BCHP_CHIP == 7400)  /* 7038,7400 has 3rd smartcard */
		case 2:
			intId = BCHP_INT_ID_scc_irqen;
			BREG_Write32(  in_handle->regHandle,  BCHP_SCIRQ0_SCIRQEN,
					(BCHP_SCIRQ0_SCIRQEN_scc_irqen_MASK |
					  BREG_Read32(in_handle->regHandle,  BCHP_SCIRQ0_SCIRQEN)) );

			/* Pin muxing */
#if (BCHP_CHIP == 7400)
#if ((BCHP_VER >= BCHP_VER_A0) && (BCHP_VER < BCHP_VER_B0) )
			ulVal = BREG_Read32 (in_handle->regHandle,BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_2);
			ulVal &= ~(BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_2, gpio_001) |
					BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_2, gpio_000));
			ulVal |= 	BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_2, gpio_001, 2) |
					BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_2, gpio_000, 2);
			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_2, ulVal );

			ulVal = BREG_Read32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_3);
			ulVal &= ~(BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_3, gpio_004) |
					BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_3, gpio_003) |
					BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_3, gpio_002));
			ulVal |= 	BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_3, gpio_004, 2) |
					BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_3, gpio_003, 2) |
					BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_3, gpio_002, 2);
			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_3, ulVal );

#else
			 /* make sure you are using 7400 header file bchp_sun_top_ctrl.h */
			ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7);

			ulVal &= ~(BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_050) |  /*sc_vcc_2*/
					BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_049) |  /*sc_pres_2*/
					BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_048) |  /*sc_rst_2*/
					BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_047) |  /*sc_clk_2*/
					BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_046));  /*sc_io_2*/
			ulVal |= BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_050, 4) |  /*sc_vcc_2*/
					BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_049, 4) |  /*sc_pres_1*/
					BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_048, 4) |  /*sc_rst_2*/
					BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_047, 4) |  /*sc_clk_2*/
					BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_046, 4);  /*sc_io_2*/

			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7, ulVal);
#endif
		     BDBG_MSG(( "7400, need to switch io pin\n" ));
#endif

			/* Pin muxing */
#if ((BCHP_CHIP==7038) &&  (BCHP_VER >= BCHP_VER_B0) && (BCHP_VER < BCHP_VER_C0))
			/* make sure you are using B0 header file bchp_sun_top_ctrl.h */
			ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4);
			ulVal &= ~( BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_00_MASK);
			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4,
				( ulVal |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4_gpio_00_SHIFT) ));

			ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5);
			ulVal &= ~(BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5_gpio_04_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5_gpio_03_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5_gpio_02_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5_gpio_01_MASK );
			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5,
				(ulVal |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5_gpio_04_SHIFT) |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5_gpio_03_SHIFT) |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5_gpio_02_SHIFT) |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5_gpio_01_SHIFT)));
			BDBG_MSG(( "7038 Bx, need to switch io pin\n" ));

#elif ((BCHP_CHIP==7038) && (BCHP_VER >= BCHP_VER_C0) && (BCHP_VER < BCHP_VER_D0) ) || (BCHP_CHIP==7438)
			/* make sure you are using C0 header file bchp_sun_top_ctrl.h */
			ulVal = BREG_Read32 (in_handle->regHandle,
					BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5);
			ulVal &= ~(BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5_gpio_04_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5_gpio_03_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5_gpio_02_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5_gpio_01_MASK |
						BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5_gpio_00_MASK );
			BREG_Write32 (in_handle->regHandle, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5,
				(ulVal |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5_gpio_04_SHIFT) |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5_gpio_03_SHIFT) |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5_gpio_02_SHIFT) |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5_gpio_01_SHIFT) |
					(0x00000001 << BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_5_gpio_00_SHIFT ) ));
			BDBG_MSG(( "7038 C0, need to switch io pin\n" ));
#else
	BSTD_UNUSED(ulVal);

#endif

			break;

#endif  /* #if (BCHP_CHIP==7038 || BCHP_CHIP == 7400) , 7038,7400 has 3rd smartcard */

		default:
			errCode = BERR_INVALID_PARAMETER;
			goto BSCD_P_DONE_LABEL;
	}
	}



	BSCD_P_CHECK_ERR_CODE_FUNC(errCode,
		BKNI_CreateEvent( &(channelHandle->channelWaitEvent.cardWait)));
	BSCD_P_CHECK_ERR_CODE_FUNC(errCode,
		BKNI_CreateEvent( &(channelHandle->channelWaitEvent.tdoneWait)));
	BSCD_P_CHECK_ERR_CODE_FUNC(errCode,
		BKNI_CreateEvent( &(channelHandle->channelWaitEvent.rcvWait)));
	BSCD_P_CHECK_ERR_CODE_FUNC(errCode,
		BKNI_CreateEvent( &(channelHandle->channelWaitEvent.atrStart)));
	BSCD_P_CHECK_ERR_CODE_FUNC(errCode,
		BKNI_CreateEvent( &(channelHandle->channelWaitEvent.timerWait)));

#ifdef BSCD_EMV2000_CWT_PLUS_4_EVENT_INTR
	BSCD_P_CHECK_ERR_CODE_FUNC(errCode,
		BKNI_CreateEvent( &(channelHandle->channelWaitEvent.event1Wait)));
#endif

	BSCD_P_CHECK_ERR_CODE_FUNC(errCode,
		BKNI_CreateEvent( &(channelHandle->channelWaitEvent.event2Wait)));

	BKNI_EnterCriticalSection();
	BSCD_Channel_P_EnableInterrupts_isr(channelHandle);   /* Todo:  Only enable intr for ATR */
	BKNI_LeaveCriticalSection();

	/*Set VCC level to inp_channelDefSettings->vcc*/
	if(  channelHandle->currentChannelSettings.setVcc){
		BSCD_Channel_SetVccLevel( channelHandle,channelHandle->currentChannelSettings.vcc);
	}

	in_handle->channelHandles[in_channelNo] = channelHandle;

	*outp_channelHandle = channelHandle;
	BKNI_EnterCriticalSection();
	channelHandle->bIsOpen = true;
	BKNI_LeaveCriticalSection();


	ulVal = BREG_Read32( 
		
	in_handle->regHandle, 
		(channelHandle->ulRegStartAddr + BSCD_P_STATUS_1));	

     if(ulVal&BCHP_SCA_SC_STATUS_1_card_pres_MASK)
	 	channelHandle->channelStatus.bCardPresent = true;
BSCD_P_DONE_LABEL:
	if( errCode != BERR_SUCCESS )
	{
		if( channelHandle != NULL )
		{
			if (channelHandle->channelWaitEvent.cardWait != NULL)
				BKNI_DestroyEvent( channelHandle->channelWaitEvent.cardWait );
			if (channelHandle->channelWaitEvent.tdoneWait != NULL)
				BKNI_DestroyEvent( channelHandle->channelWaitEvent.tdoneWait );
			if (channelHandle->channelWaitEvent.rcvWait != NULL)
				BKNI_DestroyEvent( channelHandle->channelWaitEvent.rcvWait );
			if (channelHandle->channelWaitEvent.atrStart != NULL)
				BKNI_DestroyEvent( channelHandle->channelWaitEvent.atrStart );
			if (channelHandle->channelWaitEvent.timerWait != NULL)
				BKNI_DestroyEvent( channelHandle->channelWaitEvent.timerWait );
#ifdef BSCD_EMV2000_CWT_PLUS_4_EVENT_INTR
			if (channelHandle->channelWaitEvent.atrStart != NULL)
				BKNI_DestroyEvent( channelHandle->channelWaitEvent.event1Wait );
#endif
			if (channelHandle->channelWaitEvent.timerWait != NULL)
				BKNI_DestroyEvent( channelHandle->channelWaitEvent.event2Wait );

#ifdef BCHP_PWR_RESOURCE_SMARTCARD0
			    /* a failed open releases power */
	    BCHP_PWR_ReleaseResource(channelHandle->moduleHandle->chipHandle, resourceId);
#endif

			BKNI_Free( channelHandle );

		}
	}

	BDBG_LEAVE(BSCD_Channel_Open);
	return( errCode );
}

BERR_Code BSCD_Channel_Close(
		BSCD_ChannelHandle inout_channelHandle
)
{
	BERR_Code errCode = BERR_SUCCESS;
	BSCD_Handle moduleHandle;
	uint32_t ulValue = 0;
       /* 12/02/2006 QX: add this to avoid crash during BKNI_Free */
	 void *pTemp;
#ifdef BCHP_PWR_RESOURCE_SMARTCARD0
    BCHP_PWR_ResourceId resourceId = BCHP_PWR_RESOURCE_SMARTCARD0;
#ifdef BCHP_PWR_RESOURCE_SMARTCARD1
    if(inout_channelHandle->ucChannelNumber == 1) {
	resourceId = BCHP_PWR_RESOURCE_SMARTCARD1;
    }
#endif
#endif

	BDBG_ENTER(BSCD_Channel_Close);
	BDBG_ASSERT( inout_channelHandle );

	BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
		(inout_channelHandle->ulMagicNumber != BSCD_P_CHANNEL_HANDLE_MAGIC_NUMBER ) );

	BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
		(inout_channelHandle->bIsOpen ==  false) );


BSCD_P_DONE_LABEL:

	inout_channelHandle->bIsOpen = false;

	ulValue = BREG_Read32(inout_channelHandle->moduleHandle->regHandle,  BCHP_SCIRQ0_SCIRQEN);
	/*  Disable smartcard interrupt   */
	switch (inout_channelHandle->ucChannelNumber) {
		case 0:
			ulValue &= ~BCHP_SCIRQ0_SCIRQEN_sca_irqen_MASK;
			break;

#if (BCHP_CHIP==7038 || BCHP_CHIP==7438 || BCHP_CHIP==7400 || BCHP_CHIP==7401 )  /* 3560 only has 1 smartcard */
		case 1:
			ulValue &= ~BCHP_SCIRQ0_SCIRQEN_scb_irqen_MASK;
			break;
#endif

#if (BCHP_CHIP==7038 || BCHP_CHIP==7438 )  /* 3560 only has 1 smartcard */
		case 2:
			ulValue &= ~BCHP_SCIRQ0_SCIRQEN_scc_irqen_MASK;
			break;
#endif

		default:
			errCode = BERR_INVALID_PARAMETER;
			break;
	}


	BREG_Write32(inout_channelHandle->moduleHandle->regHandle,  BCHP_SCIRQ0_SCIRQEN,
					ulValue);
	BSCD_P_CHECK_ERR_CODE_FUNC( errCode, BINT_DisableCallback( inout_channelHandle->channelIntCallback));
	BSCD_P_CHECK_ERR_CODE_FUNC( errCode, BINT_DestroyCallback( inout_channelHandle->channelIntCallback ) );

	BKNI_DestroyEvent( inout_channelHandle->channelWaitEvent.cardWait );
	BKNI_DestroyEvent( inout_channelHandle->channelWaitEvent.tdoneWait );
	BKNI_DestroyEvent( inout_channelHandle->channelWaitEvent.rcvWait );
	BKNI_DestroyEvent( inout_channelHandle->channelWaitEvent.atrStart );
	BKNI_DestroyEvent( inout_channelHandle->channelWaitEvent.timerWait );
#ifdef BSCD_EMV2000_CWT_PLUS_4_EVENT_INTR
	BKNI_DestroyEvent( inout_channelHandle->channelWaitEvent.event1Wait );
#endif
	BKNI_DestroyEvent( inout_channelHandle->channelWaitEvent.event2Wait );

	BSCD_Channel_Deactivate(inout_channelHandle);

	moduleHandle = inout_channelHandle->moduleHandle;
	moduleHandle->channelHandles[inout_channelHandle->ucChannelNumber] = NULL;
	
#ifdef BCHP_PWR_RESOURCE_SMARTCARD
	if (moduleHandle->currentSettings.moduleClkFreq.FreqSrc == BSCD_ClockFreqSrc_eExternalClock) {
	    /* release power */
	    BCHP_PWR_ReleaseResource(inout_channelHandle->moduleHandle->chipHandle, BCHP_PWR_RESOURCE_SMARTCARD);
	}
#endif

	 /* 12/02/2006 QX: add this to avoid crash during BKNI_Free */
	BKNI_EnterCriticalSection();
	pTemp = inout_channelHandle;
	inout_channelHandle = NULL;
	BKNI_LeaveCriticalSection();
	BKNI_Free(pTemp);

	BDBG_LEAVE(BSCD_Channel_Close);
	return( errCode );
}

BERR_Code BSCD_Channel_GetDevice(
		BSCD_ChannelHandle	in_channelHandle,
		BSCD_Handle			*outp_handle
)
{
	BERR_Code errCode = BERR_SUCCESS;

	BDBG_ENTER(BSCD_Channel_GetDevice);
	BDBG_ASSERT( in_channelHandle );

	BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
		(in_channelHandle->moduleHandle->bIsOpen ==  false) );

	*outp_handle = NULL;

	BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
		(in_channelHandle->ulMagicNumber != BSCD_P_CHANNEL_HANDLE_MAGIC_NUMBER ) );

	*outp_handle = in_channelHandle->moduleHandle;

BSCD_P_DONE_LABEL:
	BDBG_LEAVE(BSCD_Channel_GetDevice);
	return( errCode );
}


BERR_Code BSCD_GetChannel(
		BSCD_Handle			in_handle,
		unsigned int		in_channelNo,
		BSCD_ChannelHandle	*outp_channelHandle
)
{
	BERR_Code errCode = BERR_SUCCESS;
 	BSCD_ChannelHandle channelHandle = NULL;

	BDBG_ENTER(BSCD_GetChannel);
	BDBG_ASSERT( in_handle );

	*outp_channelHandle = NULL;
	BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
		(in_handle->ulMagicNumber != BSCD_P_HANDLE_MAGIC_NUMBER ) );

	BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BERR_INVALID_PARAMETER,
		(in_channelNo >= in_handle->currentSettings.ucMaxChannels) );

	channelHandle = in_handle->channelHandles[in_channelNo];

	BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
		(channelHandle == NULL ) );

	BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
		(channelHandle->ulMagicNumber != BSCD_P_CHANNEL_HANDLE_MAGIC_NUMBER ) );


	BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
		(channelHandle->bIsOpen ==  false) );

	*outp_channelHandle = channelHandle;

BSCD_P_DONE_LABEL:

	BDBG_LEAVE(BSCD_GetChannel);
	return( errCode );
}


BERR_Code BSCD_Channel_DetectCard(
		BSCD_ChannelHandle	in_channelHandle,
		BSCD_CardPresent	in_eCardPresent
)
{
	BERR_Code errCode = BERR_SUCCESS;

	BDBG_ENTER(BSCD_Channel_DetectCard);
	BDBG_ASSERT( in_channelHandle );

	BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
		(in_channelHandle->ulMagicNumber != BSCD_P_CHANNEL_HANDLE_MAGIC_NUMBER ) );

	BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
		(in_channelHandle->bIsOpen ==  false) );

	switch (in_eCardPresent) {
		case BSCD_CardPresent_eInserted:
		{
			BKNI_EnterCriticalSection();

			if ( in_channelHandle->ulStatus1 & BCHP_SCA_SC_STATUS_1_card_pres_MASK) {
				in_channelHandle->channelStatus.bCardPresent = true;
				BKNI_LeaveCriticalSection();
				goto BSCD_P_DONE_LABEL;
			}
			else {
				BDBG_MSG(("SmartCard Not Present"));
				BDBG_MSG(("Please insert the SmartCard"));
			}
			BKNI_LeaveCriticalSection();


		   	BSCD_P_CHECK_ERR_CODE_FUNC(errCode,
		   		BSCD_Channel_P_WaitForCardInsertion(in_channelHandle));
		}
		break;

		case BSCD_CardPresent_eRemoved:
		{
			BKNI_EnterCriticalSection();

			if ( !(in_channelHandle->ulStatus1 & BCHP_SCA_SC_STATUS_1_card_pres_MASK)) {
				in_channelHandle->channelStatus.bCardPresent = false;
				BKNI_LeaveCriticalSection();
				goto BSCD_P_DONE_LABEL;
			}
			else {
				BDBG_MSG(("SmartCard Present"));
				BDBG_MSG(("Please remove the SmartCard"));
			}
			BKNI_LeaveCriticalSection();

		   	BSCD_P_CHECK_ERR_CODE_FUNC(errCode,
		   		BSCD_Channel_P_WaitForCardRemove(in_channelHandle));
		}
		break;
	}

BSCD_P_DONE_LABEL:

	BDBG_LEAVE(BSCD_Channel_DetectCard);
	return( errCode );

}



BERR_Code BSCD_Channel_SetParameters(
		BSCD_ChannelHandle			in_channelHandle,
		const BSCD_ChannelSettings	*inp_sSettings
)
{
	BERR_Code errCode = BERR_SUCCESS;
	uint32_t		ulValue = 0;

	BDBG_ENTER(BSCD_Channel_SetParameters);
	BDBG_ASSERT( in_channelHandle );

	BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
		(in_channelHandle->ulMagicNumber != BSCD_P_CHANNEL_HANDLE_MAGIC_NUMBER ) );


    in_channelHandle->currentChannelSettings.bIsPresHigh = inp_sSettings->bIsPresHigh;
	 in_channelHandle->currentChannelSettings.setPinmux = inp_sSettings->setPinmux;
	 in_channelHandle->currentChannelSettings.setVcc = inp_sSettings->setVcc;
	 in_channelHandle->currentChannelSettings.vcc = inp_sSettings->vcc;
     in_channelHandle->currentChannelSettings.srcClkFreqInHz = inp_sSettings->srcClkFreqInHz;
	 in_channelHandle->currentChannelSettings.eResetCycles =inp_sSettings->eResetCycles;

     in_channelHandle->currentChannelSettings.bDirectRstInverted =inp_sSettings->bDirectRstInverted;
     in_channelHandle->currentChannelSettings.bDirectVccInverted =inp_sSettings->bDirectVccInverted;


 BDBG_MSG(("bIsPresHigh = %d", in_channelHandle->currentChannelSettings.bIsPresHigh));

    if(in_channelHandle->currentChannelSettings.bIsPresHigh)
    {
        ulValue =  BCHP_SCA_SC_IF_CMD_1_pres_pol_MASK | BREG_Read32(
            in_channelHandle->moduleHandle->regHandle,
            (in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1)) ;
        BREG_Write32(
            in_channelHandle->moduleHandle->regHandle,
            (in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1),
             ulValue);
    }


	/*  Smart Card Standard */
	BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
        ((inp_sSettings->scStandard <= BSCD_Standard_eUnknown)  || (inp_sSettings->scStandard >= BSCD_Standard_eMax)) );
	in_channelHandle->currentChannelSettings.scStandard = 	inp_sSettings->scStandard;
	BDBG_MSG(("scStandard = %d", in_channelHandle->currentChannelSettings.scStandard));

	BSCD_P_CHECK_ERR_CODE_FUNC(errCode, BSCD_Channel_P_SetStandard(
		in_channelHandle, inp_sSettings));

	BSCD_P_CHECK_ERR_CODE_FUNC(errCode, BSCD_Channel_P_SetFreq(
		in_channelHandle, inp_sSettings));

	/* Set maximum IFSD */
	in_channelHandle->currentChannelSettings.unMaxIFSD =  BSCD_MAX_TX_SIZE ;
	BDBG_MSG(("unMaxIFSD = %d", in_channelHandle->currentChannelSettings.unMaxIFSD));

	/* Set current IFSD */
	BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
				(inp_sSettings->unCurrentIFSD > BSCD_MAX_TX_SIZE));
	if (inp_sSettings->unMaxIFSD == 0) {
		in_channelHandle->currentChannelSettings.unCurrentIFSD =  BSCD_MAX_TX_SIZE ;
	}
	else {
		in_channelHandle->currentChannelSettings.unCurrentIFSD =  inp_sSettings->unCurrentIFSD ;
	}
	BDBG_MSG(("unCurrentIFSD = %d", in_channelHandle->currentChannelSettings.unCurrentIFSD));

	BSCD_P_CHECK_ERR_CODE_FUNC(errCode, BSCD_Channel_P_SetEdcParity(
		in_channelHandle, inp_sSettings));

	BSCD_P_CHECK_ERR_CODE_FUNC(errCode, BSCD_Channel_P_SetWaitTime(
		in_channelHandle, inp_sSettings));

	BSCD_P_CHECK_ERR_CODE_FUNC(errCode, BSCD_Channel_P_SetGuardTime(
		in_channelHandle, inp_sSettings));

	/* Set transaction time out */
	BSCD_P_CHECK_ERR_CODE_FUNC(errCode, BSCD_Channel_P_SetTransactionTimeout(
		in_channelHandle, inp_sSettings));

	/* auto deactivation sequence */
	in_channelHandle->currentChannelSettings.bAutoDeactiveReq =  inp_sSettings->bAutoDeactiveReq;
	BDBG_MSG(("bAutoDeactiveReq = %d", in_channelHandle->currentChannelSettings.bAutoDeactiveReq));

	/* nullFilter */
	in_channelHandle->currentChannelSettings.bNullFilter =  inp_sSettings->bNullFilter;
	BDBG_MSG(("bNullFilter = %d", in_channelHandle->currentChannelSettings.bNullFilter));

    /* connectDirectly */
	in_channelHandle->currentChannelSettings.bConnectDirectly =  inp_sSettings->bConnectDirectly;
	BDBG_MSG(("bConnectDirectly = %d", in_channelHandle->currentChannelSettings.bConnectDirectly));


	/* debounce info */
	BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
				(inp_sSettings->scPresDbInfo.ucDbWidth > BSCD_MAX_DB_WIDTH ));
	in_channelHandle->currentChannelSettings.scPresDbInfo =  inp_sSettings->scPresDbInfo;
	BDBG_MSG(("scPresDbInfo.bIsEnabled = %d", in_channelHandle->currentChannelSettings.scPresDbInfo.bIsEnabled));
	BDBG_MSG(("scPresDbInfo.ucDbWidth = %d", in_channelHandle->currentChannelSettings.scPresDbInfo.ucDbWidth));
	BDBG_MSG(("scPresDbInfo.scPresMode = %d", in_channelHandle->currentChannelSettings.scPresDbInfo.scPresMode));

	/* Specify if we want the driver to read, decode and program registers */
	in_channelHandle->currentChannelSettings.resetCardAction = inp_sSettings->resetCardAction;
	BDBG_MSG(("resetCardAction = %d", in_channelHandle->currentChannelSettings.resetCardAction));


	in_channelHandle->currentChannelSettings.ATRRecvTimeInteger =  inp_sSettings->ATRRecvTimeInteger;

	/* Update the BSCD_P_PRESCALE */
	ulValue = BREG_Read32(
		in_channelHandle->moduleHandle->regHandle,
		(in_channelHandle->ulRegStartAddr + BSCD_P_PRESCALE));
	BDBG_MSG(("orig BSCD_P_PRESCALE = 0x%x\n", ulValue));



	BREG_Write32(
		in_channelHandle->moduleHandle->regHandle,
		(in_channelHandle->ulRegStartAddr + BSCD_P_PRESCALE),
		in_channelHandle->currentChannelSettings.unPrescale);
	BDBG_MSG(("New BSCD_P_PRESCALE = 0x%x\n", in_channelHandle->currentChannelSettings.unPrescale));

#if 1
	/* Don't enable clock here since auto_clk need to be set first in ResetIFD before
	     clock enabling for auto_deactivation */
	ulValue = BREG_Read32(
		in_channelHandle->moduleHandle->regHandle,
		(in_channelHandle->ulRegStartAddr + BSCD_P_CLK_CMD));
	BDBG_MSG(("orig ucClkCmd = 0x%x\n",  ulValue));

	/* If enabled before, change the the value.  Otherwise leave it intact. */
	ulValue = ulValue & BCHP_SCA_SC_CLK_CMD_clk_en_MASK;
	if (ulValue == BCHP_SCA_SC_CLK_CMD_clk_en_MASK) {

		ulValue = ulValue | (BSCD_P_MapScClkDivToMaskValue(in_channelHandle->currentChannelSettings.ucScClkDiv))  |
				((in_channelHandle->currentChannelSettings.ucEtuClkDiv - 1) << 1)  |
				((in_channelHandle->currentChannelSettings.ucBaudDiv == 31) ? 0 : 1);

	   	BREG_Write32(
			in_channelHandle->moduleHandle->regHandle,
			(in_channelHandle->ulRegStartAddr + BSCD_P_CLK_CMD),
			ulValue);
		if(in_channelHandle->currentChannelSettings.ucBaudDiv == 25){
				ulValue = BREG_Read32(
					in_channelHandle->moduleHandle->regHandle,
					(in_channelHandle->ulRegStartAddr + BSCD_P_FLOW_CMD));
				ulValue = 0x80 |ulValue;
				BREG_Write32(
					in_channelHandle->moduleHandle->regHandle,
					(in_channelHandle->ulRegStartAddr + BSCD_P_FLOW_CMD),
					ulValue);
		}
		BDBG_MSG(("New SC_CLK_CMD = 0x%x\n", ulValue));
	}
#endif

	BDBG_MSG(("address  = 0x%x\n", in_channelHandle->ulRegStartAddr));
	BDBG_MSG(("BSCD_P_UART_CMD_2 address  = 0x%x\n", (in_channelHandle->ulRegStartAddr + BSCD_P_UART_CMD_2)));
	/* Update the BSCD_P_UART_CMD_2 */
	ulValue = BREG_Read32(
		in_channelHandle->moduleHandle->regHandle,
		(in_channelHandle->ulRegStartAddr + BSCD_P_UART_CMD_2));
	BDBG_MSG(("orig BSCD_P_UART_CMD_2 = 0x%x\n", 	ulValue));

	ulValue  &=  (BCHP_SCA_SC_UART_CMD_2_convention_MASK);

	if (inp_sSettings->eProtocolType == BSCD_AsyncProtocolType_e0 ) {

		ulValue |= (in_channelHandle->currentChannelSettings.ucRxRetries << BCHP_SCA_SC_UART_CMD_2_rpar_retry_SHIFT) |
				(in_channelHandle->currentChannelSettings.ucTxRetries);
	}
	else if ( (inp_sSettings->eProtocolType == BSCD_AsyncProtocolType_e1 )  ||
			(inp_sSettings->eProtocolType == BSCD_AsyncProtocolType_e14_IRDETO ) ) {
		/* No OP */ ;
	}
   	BREG_Write32(
		in_channelHandle->moduleHandle->regHandle,
		(in_channelHandle->ulRegStartAddr + BSCD_P_UART_CMD_2),
		ulValue);

	BDBG_MSG(("BSCD_P_UART_CMD_2 = 0x%x\n", 	ulValue));

	/* Update the BSCD_P_PROTO_CMD */
	ulValue =  BREG_Read32(
		in_channelHandle->moduleHandle->regHandle,
		(in_channelHandle->ulRegStartAddr + BSCD_P_PROTO_CMD));
	if ((inp_sSettings->eProtocolType == BSCD_AsyncProtocolType_e1 ) &&
			(in_channelHandle->currentChannelSettings.edcSetting.bIsEnabled))  {
		ulValue =  BCHP_SCA_SC_PROTO_CMD_edc_en_MASK;

		if (in_channelHandle->currentChannelSettings.edcSetting.edcEncode == BSCD_EDCEncode_eLRC ) {
			ulValue &=  ~BCHP_SCA_SC_PROTO_CMD_crc_lrc_MASK;
		}
		else if (in_channelHandle->currentChannelSettings.edcSetting.edcEncode == BSCD_EDCEncode_eCRC) {
			ulValue |=  BCHP_SCA_SC_PROTO_CMD_crc_lrc_MASK;
		}
	}
	else {
		ulValue &=  ~BCHP_SCA_SC_PROTO_CMD_edc_en_MASK;
	}

	ulValue |= in_channelHandle->currentChannelSettings.ulCharacterWaitTimeInteger;

   	BREG_Write32(
		in_channelHandle->moduleHandle->regHandle,
		(in_channelHandle->ulRegStartAddr + BSCD_P_PROTO_CMD),
		ulValue);

	/* Update the BSCD_P_FLOW_CMD */
	ulValue = 0;
	/* flow control in enabled in TX/RV function, do not enable here */
	/*
	if (in_channelHandle->currentChannelSettings.scStandard == BSCD_Standard_eNDS) {
		ulValue =  BCHP_SCA_SC_FLOW_CMD_flow_en_MASK;
	}
	else {
		ulValue &=  ~BCHP_SCA_SC_FLOW_CMD_flow_en_MASK;
	}

*/
   	BREG_Write32(
		in_channelHandle->moduleHandle->regHandle,
		(in_channelHandle->ulRegStartAddr + BSCD_P_FLOW_CMD),
		ulValue);


	/* Update the BSCD_P_IF_CMD_2 */
	ulValue = 0;
	if (in_channelHandle->currentChannelSettings.scPresDbInfo.bIsEnabled == true) {
		ulValue =  BCHP_SCA_SC_IF_CMD_2_db_en_MASK;
	}
	else {
		ulValue &=  ~BCHP_SCA_SC_IF_CMD_2_db_en_MASK;
	}

	if (in_channelHandle->currentChannelSettings.scPresDbInfo.scPresMode == BSCD_ScPresMode_eMask) {
		ulValue |= BCHP_SCA_SC_IF_CMD_2_db_mask_MASK;
	}
	else if (in_channelHandle->currentChannelSettings.scPresDbInfo.scPresMode == BSCD_ScPresMode_eDebounce) {
		ulValue &= ~BCHP_SCA_SC_IF_CMD_2_db_mask_MASK;
	}

	ulValue |= in_channelHandle->currentChannelSettings.scPresDbInfo.ucDbWidth;

   	BREG_Write32(
		in_channelHandle->moduleHandle->regHandle,
		(in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_2),
		ulValue);

	/* Update the BSCD_P_TGUARD */
	BREG_Write32(
		in_channelHandle->moduleHandle->regHandle,
		(in_channelHandle->ulRegStartAddr + BSCD_P_TGUARD),
		in_channelHandle->currentChannelSettings.extraGuardTime.ulValue);

	if( inp_sSettings->setVcc){
		BSCD_Channel_SetVccLevel( in_channelHandle,inp_sSettings->vcc);
	}

BSCD_P_DONE_LABEL:

	BDBG_LEAVE(BSCD_Channel_SetParameters);
	return( errCode );
}


BERR_Code BSCD_Channel_GetParameters(
		BSCD_ChannelHandle	in_channelHandle,
		BSCD_ChannelSettings	*outp_sSettings
)
{
	BERR_Code errCode = BERR_SUCCESS;

	BDBG_ENTER(BSCD_Channel_GetParameters);
	BDBG_ASSERT( in_channelHandle );

	BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
		(in_channelHandle->ulMagicNumber != BSCD_P_CHANNEL_HANDLE_MAGIC_NUMBER ) );

	*outp_sSettings = in_channelHandle->currentChannelSettings;

BSCD_P_DONE_LABEL:

	BDBG_LEAVE(BSCD_Channel_GetParameters);
	return( errCode );
}

char	BSCD_Channel_GetChannelNumber(
		BSCD_ChannelHandle	in_channelHandle
)
{
	BERR_Code errCode = BERR_SUCCESS;

	BDBG_ENTER(BSCD_Channel_GetChannelNumber);
	BDBG_ASSERT( in_channelHandle );

	BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
		(in_channelHandle->ulMagicNumber != BSCD_P_CHANNEL_HANDLE_MAGIC_NUMBER ) );

BSCD_P_DONE_LABEL:

	BDBG_LEAVE(BSCD_Channel_GetChannelNumber);
	if (errCode == BERR_SUCCESS)
		return( in_channelHandle->ucChannelNumber );
	else
		return  -1;
}


BERR_Code BSCD_Channel_Deactivate(
		BSCD_ChannelHandle          in_channelHandle
)
{
	BERR_Code errCode = BERR_SUCCESS;
	uint32_t	ulValue;

	BDBG_ENTER(BSCD_Channel_Deactivate);
	BDBG_ASSERT( in_channelHandle );

	BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
		(in_channelHandle->ulMagicNumber != BSCD_P_CHANNEL_HANDLE_MAGIC_NUMBER ) );

	BDBG_MSG(("In BSCD_Channel_Deactivate\n"));
	/* Disable all interrupts */
	BREG_Write32(
		in_channelHandle->moduleHandle->regHandle,
		(in_channelHandle->ulRegStartAddr + BSCD_P_INTR_EN_1), 0);

	BREG_Write32(
		in_channelHandle->moduleHandle->regHandle,
		(in_channelHandle->ulRegStartAddr + BSCD_P_INTR_EN_2), 0);


        if(in_channelHandle->currentChannelSettings.bConnectDirectly == true){
          /* Turn off VCC */
          ulValue =  BREG_Read32(
              in_channelHandle->moduleHandle->regHandle,
              (in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1)) ;
	  if(in_channelHandle->currentChannelSettings.bDirectVccInverted == true)
     	ulValue |= BCHP_SCA_SC_IF_CMD_1_vcc_MASK; 
 	  else
          ulValue &= ~BCHP_SCA_SC_IF_CMD_1_vcc_MASK; 
        ulValue |= BCHP_SCA_SC_IF_CMD_1_io_MASK;
    }else{
	/* Turn off VCC */
	ulValue =  BCHP_SCA_SC_IF_CMD_1_vcc_MASK | BREG_Read32(
		in_channelHandle->moduleHandle->regHandle,
		(in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1)) ;
        }
        ulValue &= ~BCHP_SCA_SC_IF_CMD_1_auto_vcc_MASK;
	BREG_Write32(
		in_channelHandle->moduleHandle->regHandle,
		(in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1),
		 ulValue);


        if(in_channelHandle->currentChannelSettings.bConnectDirectly == true){
          /* Set RST = 0.     */
		     if(in_channelHandle->currentChannelSettings.bDirectRstInverted ==true)
          ulValue =  BCHP_SCA_SC_IF_CMD_1_rst_MASK | BREG_Read32(
              in_channelHandle->moduleHandle->regHandle,
              (in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1)) ;
			 else 
			 	ulValue =  (~BCHP_SCA_SC_IF_CMD_1_rst_MASK) & BREG_Read32(
                in_channelHandle->moduleHandle->regHandle,
                 (in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1)) ;
        }
        else
         
        {
	/* Set RST = 0.     */
	ulValue =  (~BCHP_SCA_SC_IF_CMD_1_rst_MASK) & BREG_Read32(
		in_channelHandle->moduleHandle->regHandle,
		(in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1)) ;
        }
       
        ulValue &= ~BCHP_SCA_SC_IF_CMD_1_auto_rst_MASK; 
	BREG_Write32(
		in_channelHandle->moduleHandle->regHandle,
		(in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1),
		 ulValue);

	/* Set CLK = 0.      */
	BREG_Write32(
		in_channelHandle->moduleHandle->regHandle,
		(in_channelHandle->ulRegStartAddr + BSCD_P_CLK_CMD),
		 0);
#if 0
    /* Set IO = 0.     This will cause IO line in an unknown state in wamr reset, comment out temparily */
	ulValue =  (BCHP_SCA_SC_IF_CMD_1_io_MASK) | BREG_Read32(
		in_channelHandle->moduleHandle->regHandle,
		(in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1)) ;
	BREG_Write32(
		in_channelHandle->moduleHandle->regHandle,
		(in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1),
		 ulValue);
#endif 
	/* Reset Tx & Rx buffers.   */
	BREG_Write32(
		in_channelHandle->moduleHandle->regHandle,
		(in_channelHandle->ulRegStartAddr + BSCD_P_UART_CMD_1),
		 ~BCHP_SCA_SC_UART_CMD_1_io_en_MASK );

	BREG_Write32(
		in_channelHandle->moduleHandle->regHandle,
		(in_channelHandle->ulRegStartAddr + BSCD_P_PROTO_CMD),
		 BCHP_SCA_SC_PROTO_CMD_rbuf_rst_MASK | BCHP_SCA_SC_PROTO_CMD_tbuf_rst_MASK);

BSCD_P_DONE_LABEL:

	BDBG_LEAVE(BSCD_Channel_Deactivate);
	return( errCode );
}


BERR_Code BSCD_Channel_ResetIFD(
		BSCD_ChannelHandle	in_channelHandle,
		BSCD_ResetType		in_resetType

)
{
	BERR_Code errCode = BERR_SUCCESS;
	uint32_t	ulIFCmdVal = 0, ulValue;
	BSCD_Timer 		timer = {BSCD_TimerType_eGPTimer, {BSCD_GPTimerMode_eIMMEDIATE}, true, true};
	BSCD_TimerValue    timeValue= {2, BSCD_TimerUnit_eETU};


	BDBG_ENTER(BSCD_Channel_ResetIFD);
	BDBG_ASSERT( in_channelHandle );

	BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
		(in_channelHandle->ulMagicNumber != BSCD_P_CHANNEL_HANDLE_MAGIC_NUMBER ) );

	/* Reset all status */
	in_channelHandle->ulStatus1 = 0;
	in_channelHandle->ulStatus2 = 0;
	in_channelHandle->ulIntrStatus1= 0;
	in_channelHandle->ulIntrStatus2= 0;

	in_channelHandle->channelStatus.ulStatus1 = 0;

      if (in_resetType == BSCD_ResetType_eCold) {

	in_channelHandle->channelStatus.bCardPresent = false;

	/* 09/20/05,Allen.C, reset bIsCardRemoved after card removed and reinitialize*/
	in_channelHandle->bIsCardRemoved = false;

       }
	/* Reset some critical registers */
	BREG_Write32(
		in_channelHandle->moduleHandle->regHandle,
		(in_channelHandle->ulRegStartAddr + BSCD_P_TIMER_CMD),
		 0);

	BREG_Write32(
		in_channelHandle->moduleHandle->regHandle,
		(in_channelHandle->ulRegStartAddr + BSCD_P_INTR_EN_1),
		 0);

	BREG_Write32(
		in_channelHandle->moduleHandle->regHandle,
		(in_channelHandle->ulRegStartAddr + BSCD_P_INTR_EN_2),
		 0);

	BREG_Write32(
		in_channelHandle->moduleHandle->regHandle,
		(in_channelHandle->ulRegStartAddr + BSCD_P_UART_CMD_1),
		 0);

	BREG_Write32(
		in_channelHandle->moduleHandle->regHandle,
		(in_channelHandle->ulRegStartAddr + BSCD_P_UART_CMD_2),
		 0);

	/* Set up debounce filter */
	if (in_channelHandle->currentChannelSettings.scPresDbInfo.bIsEnabled == true) {

		ulValue = BCHP_SCA_SC_IF_CMD_2_db_en_MASK;

		if (in_channelHandle->currentChannelSettings.scPresDbInfo.scPresMode == BSCD_ScPresMode_eMask) {
			ulValue |= BCHP_SCA_SC_IF_CMD_2_db_mask_MASK;
		}

		ulValue |= in_channelHandle->currentChannelSettings.scPresDbInfo.ucDbWidth;

		BREG_Write32(
			in_channelHandle->moduleHandle->regHandle,
			(in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_2), ulValue);
	}
	else {
		BREG_Write32(
			in_channelHandle->moduleHandle->regHandle,
			(in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_2), 0);
	}

	BDBG_MSG(("Inside Reset: Before Cold Reset ulIFCmdVal = 0x%x\n", ulIFCmdVal));
	/* Cold Reset or Warm Reset */
	if (in_resetType == BSCD_ResetType_eCold) {
		BDBG_MSG(("Cold Reset\n"));
		in_channelHandle->resetType = BSCD_ResetType_eCold;  /* Cold Reset */

   

                if(in_channelHandle->currentChannelSettings.bConnectDirectly == true){
#ifdef BSCD_DSS_ICAM
			if(in_channelHandle->currentChannelSettings.bDirectRstInverted ==true)

        	  	ulIFCmdVal = 0; 
			else
                  ulIFCmdVal = BCHP_SCA_SC_IF_CMD_1_rst_MASK; /*VCC L, RST H*/

			if(in_channelHandle->currentChannelSettings.bDirectVccInverted ==true)

        	  	ulIFCmdVal |= BCHP_SCA_SC_IF_CMD_1_vcc_MASK; 
			else
          		ulIFCmdVal &= ~BCHP_SCA_SC_IF_CMD_1_vcc_MASK; /*VCC L, RST H*/
#else
 		if(in_channelHandle->currentChannelSettings.bDirectRstInverted ==true)               
                  ulIFCmdVal = BCHP_SCA_SC_IF_CMD_1_rst_MASK; /*VCC L, RST L*/
		else
                  ulIFCmdVal = 0; /*VCC L, RST L*/
		if(in_channelHandle->currentChannelSettings.bDirectVccInverted ==true)

        	  	ulIFCmdVal |= BCHP_SCA_SC_IF_CMD_1_vcc_MASK; 
			else
          		ulIFCmdVal &= ~BCHP_SCA_SC_IF_CMD_1_vcc_MASK; /*VCC L, RST L*/
        ulIFCmdVal |= BCHP_SCA_SC_IF_CMD_1_io_MASK;
#endif

                }
                else{
#ifdef BSCD_DSS_ICAM
	 	ulIFCmdVal = BCHP_SCA_SC_IF_CMD_1_vcc_MASK | BCHP_SCA_SC_IF_CMD_1_rst_MASK;
#else
	 	ulIFCmdVal = BCHP_SCA_SC_IF_CMD_1_vcc_MASK;
#endif
        }
		BREG_Write32(
			in_channelHandle->moduleHandle->regHandle,
			(in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1),
			 ulIFCmdVal);
    /* waiting for TDA8024 to fully deactivate*/ 
    if(in_channelHandle->currentChannelSettings.bConnectDirectly == false) BKNI_Delay(20); 

		 /*Disable CLK*/
	BREG_Write32(
	in_channelHandle->moduleHandle->regHandle,
	(in_channelHandle->ulRegStartAddr + BSCD_P_CLK_CMD),
	 0);

	if(in_channelHandle->currentChannelSettings.bIsPresHigh){
        BDBG_MSG(("Change Presence Polarity\n"));

	 	ulIFCmdVal |= BCHP_SCA_SC_IF_CMD_1_pres_pol_MASK;
		BREG_Write32(
			in_channelHandle->moduleHandle->regHandle,
			(in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1),
			 ulIFCmdVal);
            BDBG_MSG(("Finished Change Presence Polarity\n"));
    	}


   	}
 	else {
		BDBG_MSG(("Warm Reset\n"));
		in_channelHandle->resetType = BSCD_ResetType_eWarm;  /* Warm Reset */

		ulIFCmdVal = BREG_Read32(
			in_channelHandle->moduleHandle->regHandle,
			(in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1));
 	}
	BDBG_MSG(("Inside Reset: After Cold Reset ulIFCmdVal = 0x%x\n", ulIFCmdVal));

	/* Use Auto Deactivation instead of TDA8004 */
	if (in_channelHandle->currentChannelSettings.bAutoDeactiveReq == true) {

		BDBG_MSG(("Inside Reset: Before auto clk  BSCD_P_CLK_CMD = 0x%x\n",
				BREG_Read32(
				in_channelHandle->moduleHandle->regHandle,
				(in_channelHandle->ulRegStartAddr + BSCD_P_CLK_CMD))));

		ulIFCmdVal |= BCHP_SCA_SC_IF_CMD_1_auto_clk_MASK;
		BREG_Write32(
			in_channelHandle->moduleHandle->regHandle,
			(in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1),
			 ulIFCmdVal);
	}


if(in_channelHandle->currentChannelSettings.bConnectDirectly == true){
    /* wait for voltage on Vcc to completely low */
        BKNI_Sleep(BSCD_RESET_WAIT_TIME);

    ulIFCmdVal = BREG_Read32( in_channelHandle->moduleHandle->regHandle, (in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1));
    /* Pull high Vcc, and start activation sequence */
	if(in_channelHandle->currentChannelSettings.bDirectVccInverted ==true)
    ulIFCmdVal &= ~BCHP_SCA_SC_IF_CMD_1_vcc_MASK;
	else
    ulIFCmdVal |= BCHP_SCA_SC_IF_CMD_1_vcc_MASK;

   #if 0
 ulIFCmdVal &= ~BCHP_SCA_SC_IF_CMD_1_io_MASK;
#endif
    if(in_channelHandle->currentChannelSettings.bAutoDeactiveReq == true){
      ulIFCmdVal |= BCHP_SCA_SC_IF_CMD_1_auto_vcc_MASK;
    }
    BREG_Write32( in_channelHandle->moduleHandle->regHandle, (in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1), ulIFCmdVal);

    /* From Vcc goes high, TDA8024 expects 50-220 us for CLK start */
    BKNI_Delay(100);
    ulIFCmdVal &= ~BCHP_SCA_SC_IF_CMD_1_io_MASK;
    BREG_Write32( in_channelHandle->moduleHandle->regHandle, (in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1), ulIFCmdVal);

}


	/* Set Clk cmd */
	ulValue = BCHP_SCA_SC_CLK_CMD_clk_en_MASK |
				(BSCD_P_MapScClkDivToMaskValue(in_channelHandle->currentChannelSettings.ucScClkDiv))  |
				((in_channelHandle->currentChannelSettings.ucEtuClkDiv - 1) << 1)  |
				((in_channelHandle->currentChannelSettings.ucBaudDiv == 31) ? 0 : 1);

	BDBG_MSG(("Reset: BCM_SC_CLK_CMD = 0x%lx\n", ulValue));

	BREG_Write32(
		in_channelHandle->moduleHandle->regHandle,
		(in_channelHandle->ulRegStartAddr + BSCD_P_CLK_CMD),
		 ulValue);

	BREG_Write32(
		in_channelHandle->moduleHandle->regHandle,
		(in_channelHandle->ulRegStartAddr + BSCD_P_PRESCALE),
		 in_channelHandle->currentChannelSettings.unPrescale);
	BDBG_MSG(("Reset: BSCD_P_PRESCALE = 0x%lx\n", in_channelHandle->currentChannelSettings.unPrescale));

	/* Use Auto Deactivation instead of TDA8004 */
	if (in_channelHandle->currentChannelSettings.bAutoDeactiveReq == true) {

		BDBG_MSG(("Inside Reset: Before auto io ulIFCmdVal = 0x%x\n", ulIFCmdVal));
		ulIFCmdVal |= BCHP_SCA_SC_IF_CMD_1_auto_io_MASK;
		BREG_Write32(
			in_channelHandle->moduleHandle->regHandle,
			(in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1),
			 ulIFCmdVal);
		BDBG_MSG(("Inside Reset: after auto io ulIFCmdVal = 0x%x\n", ulIFCmdVal));
	}

#if 0 /*ndef BSCD_DSS_ICAM*/
	if(in_channelHandle->currentChannelSettings.bConnectDirectly == false){

	ulIFCmdVal |= BCHP_SCA_SC_IF_CMD_1_rst_MASK;
	BREG_Write32(
			in_channelHandle->moduleHandle->regHandle,
			(in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1),
			 ulIFCmdVal);
		}
#endif


	ulValue = 0;
	BREG_Write32(
			in_channelHandle->moduleHandle->regHandle,
			(in_channelHandle->ulRegStartAddr + BSCD_P_UART_CMD_1),
			 ulValue);

	BDBG_MSG(("Inside Reset: Before SmartCardEnableInt\n"));

	/* Enable 2 interrupts with callback */
	BSCD_P_CHECK_ERR_CODE_FUNC(errCode,
 				BSCD_Channel_EnableIntrCallback_isr (
				in_channelHandle, BSCD_IntType_eCardInsertInt,
		               BSCD_Channel_P_CardInsertCB_isr));

	BSCD_P_CHECK_ERR_CODE_FUNC(errCode,
				BSCD_Channel_EnableIntrCallback_isr (
				in_channelHandle, BSCD_IntType_eCardRemoveInt,
		               BSCD_Channel_P_CardRemoveCB_isr));

	BREG_Write32(
		in_channelHandle->moduleHandle->regHandle,
		(in_channelHandle->ulRegStartAddr + BSCD_P_UART_CMD_1),
		BCHP_SCA_SC_UART_CMD_1_uart_rst_MASK);

	/******************************************************************
	**
	** UART Reset should be set within 1 ETU (however, we are generous
	** to give it 2 etus.
	**
	*****************************************************************/
	BSCD_P_CHECK_ERR_CODE_FUNC(errCode,
		BSCD_Channel_ConfigTimer(in_channelHandle, &timer, &timeValue));

	BSCD_P_CHECK_ERR_CODE_FUNC(errCode,
		BSCD_Channel_P_WaitForTimerEvent(in_channelHandle));

	/* Disable timer */
	timer.bIsTimerInterruptEnable = false;
	timer.bIsTimerEnable = false;
	BSCD_P_CHECK_ERR_CODE_FUNC(errCode,
		BSCD_Channel_EnableDisableTimer_isr(in_channelHandle, &timer));

	ulValue = BREG_Read32(
		in_channelHandle->moduleHandle->regHandle,
		(in_channelHandle->ulRegStartAddr + BSCD_P_UART_CMD_1));

	/* If equal to zero, then UART reset has gone low, so return success */
	if ((ulValue & BCHP_SCA_SC_UART_CMD_1_uart_rst_MASK) == 0) {
		BDBG_MSG(("Reset Success\n"));

		/*
		**   INITIAL_CWI_SC_PROTO_CMD = 0x0f is required so that
		**   CWI does not remain equal to zero, which causes an
		**   erroneous timeout, the CWI is set correctly in the
		**   SmartCardEMVATRDecode procedure
		*/
		BREG_Write32(
			in_channelHandle->moduleHandle->regHandle,
			(in_channelHandle->ulRegStartAddr + BSCD_P_PROTO_CMD),
			 BCHP_SCA_SC_PROTO_CMD_tbuf_rst_MASK | BCHP_SCA_SC_PROTO_CMD_rbuf_rst_MASK);
	}


BSCD_P_DONE_LABEL:

	BDBG_LEAVE(BSCD_Channel_ResetIFD);
	return( errCode );
}


BERR_Code BSCD_Channel_PowerICC(
		BSCD_ChannelHandle          in_channelHandle,
		BSCD_PowerICC               in_iccAction
)
{
	BERR_Code errCode = BERR_SUCCESS;
	uint32_t ulValue;

	BDBG_ENTER(BSCD_Channel_PowerICC);
	BDBG_ASSERT( in_channelHandle );

	BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
		(in_channelHandle->ulMagicNumber != BSCD_P_CHANNEL_HANDLE_MAGIC_NUMBER ) );

	if(in_channelHandle->currentChannelSettings.bConnectDirectly == false){

	switch (in_iccAction) {
			case BSCD_PowerICC_ePowerDown:
				ulValue =  BCHP_SCA_SC_IF_CMD_1_vcc_MASK | BREG_Read32(
				in_channelHandle->moduleHandle->regHandle,
				(in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1)) ;
				ulValue &= ~BCHP_SCA_SC_IF_CMD_1_rst_MASK;
				BREG_Write32(
					in_channelHandle->moduleHandle->regHandle,
					(in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1),
				 	ulValue);
                /* QX delay to let TDA finished */
                BKNI_Delay(20);
			break;
		case BSCD_PowerICC_ePowerUp:
			ulValue =  BREG_Read32(
				in_channelHandle->moduleHandle->regHandle,
				(in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1)) ;
			BREG_Write32(
				in_channelHandle->moduleHandle->regHandle,
				(in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1),
				 (ulValue & ~BCHP_SCA_SC_IF_CMD_1_vcc_MASK));
			break;

		default:
			BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, true);
			break;
		}
		}else{
		switch (in_iccAction) {
		case BSCD_PowerICC_ePowerUp:
			ulValue =  BCHP_SCA_SC_IF_CMD_1_vcc_MASK | BREG_Read32(
				in_channelHandle->moduleHandle->regHandle,
				(in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1)) ;
    if(in_channelHandle->currentChannelSettings.bAutoDeactiveReq == true){
      ulValue |= BCHP_SCA_SC_IF_CMD_1_auto_vcc_MASK;
    }
			BREG_Write32(
				in_channelHandle->moduleHandle->regHandle,
				(in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1),
				 ulValue);
			break;
		case BSCD_PowerICC_ePowerDown:
			ulValue =  BREG_Read32(
				in_channelHandle->moduleHandle->regHandle,
				(in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1)) ;
                        ulValue &= ~BCHP_SCA_SC_IF_CMD_1_auto_vcc_MASK;                          
			BREG_Write32(
				in_channelHandle->moduleHandle->regHandle,
				(in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1),
				 (ulValue & ~BCHP_SCA_SC_IF_CMD_1_vcc_MASK));
			break;

		default:
			BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, true);
			break;
	}

		}


BSCD_P_DONE_LABEL:

	BDBG_LEAVE(BSCD_Channel_ResetIFD);
	return( errCode );
}

BERR_Code BSCD_Channel_SetVccLevel(
		BSCD_ChannelHandle          in_channelHandle,
		BSCD_VccLevel               	in_vccLevel
)
{
	BERR_Code errCode = BERR_SUCCESS;
	uint32_t ulValue = 0;

	BSTD_UNUSED(ulValue);
	BDBG_ENTER(BSCD_Channel_SetVccLevel);
	BDBG_ASSERT( in_channelHandle );

	BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
		(in_channelHandle->ulMagicNumber != BSCD_P_CHANNEL_HANDLE_MAGIC_NUMBER ) );


	/***************************************************************
		Warning:  Note that we have to modify the board to use VPP pin of NDS
				ICAM smartcard and connect it to pin 3 (3V/5V) of TDA chip
				and run this function.  Make sure to disconnect your QAM or
				QPSK connection before calling this function or your smartcard
				will be damaged.
	***************************************************************/
	BDBG_MSG(("BSCD_Channel_SetVccLevel: in_vccLevel = 0x%x\n", in_vccLevel));

	BKNI_EnterCriticalSection();

#if (BCHP_CHIP==7038 || BCHP_CHIP==7438 )
#if ((BCHP_CHIP==7038) &&  (BCHP_VER >= BCHP_VER_B0) && (BCHP_VER < BCHP_VER_C0 ))
	switch (in_vccLevel) {
		case BSCD_VccLevel_e3V:
			/* *(uint32_t*) (0x10400724) &= ~(0x1000); GIO_DATA_HI[GPIO_44] */
			ulValue = BREG_Read32 (in_channelHandle->moduleHandle->regHandle,  BCHP_GIO_DATA_HI);
			ulValue &=  ~(0x1000);
			BREG_Write32 (in_channelHandle->moduleHandle->regHandle, BCHP_GIO_DATA_HI, ulValue );

			break;

		case BSCD_VccLevel_e5V:
			/* *(uint32_t*) (0x10400724) |= 0x1000; GIO_DATA_HI[GPIO_44] */
			ulValue = BREG_Read32 (in_channelHandle->moduleHandle->regHandle,  BCHP_GIO_DATA_HI);
			ulValue |=  0x1000;
			BREG_Write32 (in_channelHandle->moduleHandle->regHandle, BCHP_GIO_DATA_HI, ulValue );
			break;

		default:
			errCode = BERR_TRACE(BSCD_STATUS_FAILED);
			BDBG_ERR(("BSCD_Channel_SetVccLevel: Do not support VCC Level switch = 0x%x, \n", in_vccLevel));
			goto BSCD_P_DONE_LABEL;
	}
#endif

#if ((BCHP_CHIP==7038) && ( BCHP_VER >= BCHP_VER_C0) && (BCHP_VER < BCHP_VER_D0 )) || (BCHP_CHIP==7438)


	switch (in_vccLevel) {
		case BSCD_VccLevel_e3V:

			ulValue = BREG_Read32 (in_channelHandle->moduleHandle->regHandle,  BCHP_GIO_DATA_HI);
		       ulValue &=  ~(0x04000000);
		       BREG_Write32 (in_channelHandle->moduleHandle->regHandle, BCHP_GIO_DATA_HI, ulValue );
			break;

		case BSCD_VccLevel_e5V:
			 ulValue = BREG_Read32 (in_channelHandle->moduleHandle->regHandle,  BCHP_GIO_DATA_HI);
			ulValue |=  0x04000000;
			BREG_Write32 (in_channelHandle->moduleHandle->regHandle, BCHP_GIO_DATA_HI, ulValue );
			break;

		default:
			errCode = BERR_TRACE(BSCD_STATUS_FAILED);
			BDBG_ERR(("BSCD_Channel_SetVccLevel: Do not support VCC Level switch = 0x%x, \n", in_vccLevel));
			goto BSCD_P_DONE_LABEL;
	}

#endif

#elif (BCHP_CHIP==7400)
	switch (in_vccLevel) {
		case BSCD_VccLevel_e3V:
			/* *(uint32_t*) (10400744) &= ~(0x80000); GIO_DATA_EXT[GPIO_73] */
			ulValue = BREG_Read32 (in_channelHandle->moduleHandle->regHandle,  BCHP_GIO_DATA_EXT);
			ulValue &=  ~(0x80000);
			BREG_Write32 (in_channelHandle->moduleHandle->regHandle, BCHP_GIO_DATA_EXT, ulValue );

			break;

		case BSCD_VccLevel_e5V:
			/* *(uint32_t*) (10400744) |= 0x80000; GIO_DATA_EXT[GPIO_73] */
			ulValue = BREG_Read32 (in_channelHandle->moduleHandle->regHandle,  BCHP_GIO_DATA_EXT);
			ulValue |=  0x80000;
			BREG_Write32 (in_channelHandle->moduleHandle->regHandle, BCHP_GIO_DATA_EXT, ulValue );
			break;

		default:
			errCode = BERR_TRACE(BSCD_STATUS_FAILED);
			BDBG_ERR(("BSCD_Channel_SetVccLevel: Do not support VCC Level switch = 0x%x, \n", in_vccLevel));
			goto BSCD_P_DONE_LABEL;
	}



#elif (BCHP_CHIP==7125)	

	switch (in_vccLevel) {
		case BSCD_VccLevel_e3V:
			ulValue = BREG_Read32 (in_channelHandle->moduleHandle->regHandle,  BCHP_GIO_DATA_HI);	
			ulValue &=	~(1<<(58-32)); 
			BREG_Write32 (in_channelHandle->moduleHandle->regHandle, BCHP_GIO_DATA_HI, ulValue );				
			
			break;
			
		case BSCD_VccLevel_e5V:
			ulValue = BREG_Read32 (in_channelHandle->moduleHandle->regHandle,  BCHP_GIO_DATA_HI);	
			ulValue |=	(1<<(58-32));	
			BREG_Write32 (in_channelHandle->moduleHandle->regHandle, BCHP_GIO_DATA_HI, ulValue );				
			break;

		default:
			errCode = BERR_TRACE(BSCD_STATUS_FAILED);
			BDBG_ERR(("BSCD_Channel_SetVccLevel: Do not support VCC Level switch = 0x%x, \n", in_vccLevel));
			goto BSCD_P_DONE_LABEL;
	}


    
#else
	BSTD_UNUSED(in_vccLevel);
	BSTD_UNUSED(ulValue);
#endif



BSCD_P_DONE_LABEL:
	BKNI_LeaveCriticalSection();
	BDBG_LEAVE(BSCD_Channel_SetVccLevel);
	return( errCode );

}

BERR_Code BSCD_Channel_InsCardHwReset(
		BSCD_ChannelHandle          in_channelHandle,
		bool 			              in_enableHwRst
)
{
	BERR_Code errCode = BERR_SUCCESS;
	uint32_t ulValue = 0;

	BDBG_ENTER(BSCD_Channel_InsCardHwReset);
	BDBG_ASSERT( in_channelHandle );

	BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
		(in_channelHandle->ulMagicNumber != BSCD_P_CHANNEL_HANDLE_MAGIC_NUMBER ) );

#if   !defined(LINUX) && ((BCHP_CHIP==7400) )
	ulValue = BREG_Read32 (in_channelHandle->moduleHandle->regHandle,  BCHP_SUN_TOP_CTRL_RESET_CTRL);

		switch (in_channelHandle->ucChannelNumber) {
			case 0:
				if (  in_enableHwRst ) /*Enable Inser Card Hardware Reset*/
					ulValue |= ( 1 << BCHP_SUN_TOP_CTRL_RESET_CTRL_sc_insert_reset_en_SHIFT );
				else
					ulValue &= ~( 1 << BCHP_SUN_TOP_CTRL_RESET_CTRL_sc_insert_reset_en_SHIFT );

				break;

			case 1:
				if (  in_enableHwRst ) /*Enable Inser Card Hardware Reset*/
					ulValue |= ( 2 << BCHP_SUN_TOP_CTRL_RESET_CTRL_sc_insert_reset_en_SHIFT );
				else
					ulValue &= ~( 2 << BCHP_SUN_TOP_CTRL_RESET_CTRL_sc_insert_reset_en_SHIFT );

				break;

			case 2:
				if (  in_enableHwRst ) /*Enable Inser Card Hardware Reset*/
					ulValue |= ( 4 << BCHP_SUN_TOP_CTRL_RESET_CTRL_sc_insert_reset_en_SHIFT );
				else
					ulValue &= ~( 4 << BCHP_SUN_TOP_CTRL_RESET_CTRL_sc_insert_reset_en_SHIFT );

				break;

			default:
				return -1;
		}

	BREG_Write32 (in_channelHandle->moduleHandle->regHandle, BCHP_SUN_TOP_CTRL_RESET_CTRL, ulValue);

#elif   !defined(LINUX) && (BCHP_CHIP==7401)
	ulValue = BREG_Read32 (in_channelHandle->moduleHandle->regHandle,  BCHP_SUN_TOP_CTRL_RESET_CTRL);

		switch (in_channelHandle->ucChannelNumber) {
			case 0:
				if (  in_enableHwRst ) /*Enable Inser Card Hardware Reset*/
					ulValue |= ( 1 << BCHP_SUN_TOP_CTRL_RESET_CTRL_sc_insert_reset_en_SHIFT );
				else
					ulValue &= ~( 1 << BCHP_SUN_TOP_CTRL_RESET_CTRL_sc_insert_reset_en_SHIFT );

				break;

			case 1:
				if (  in_enableHwRst ) /*Enable Inser Card Hardware Reset*/
					ulValue |= ( 2 << BCHP_SUN_TOP_CTRL_RESET_CTRL_sc_insert_reset_en_SHIFT );
				else
					ulValue &= ~( 2 << BCHP_SUN_TOP_CTRL_RESET_CTRL_sc_insert_reset_en_SHIFT );

				break;

			default:
				return -1;
		}

	BREG_Write32 (in_channelHandle->moduleHandle->regHandle, BCHP_SUN_TOP_CTRL_RESET_CTRL, ulValue);
/*#elif !defined(LINUX) &&  (defined(BCM97038) || defined(BCM7038)) */
#elif !defined(LINUX) &&   ((BCHP_CHIP==7038) || (BCHP_CHIP==7438) )

	ulValue = BREG_Read32 (in_channelHandle->moduleHandle->regHandle,  BCHP_SUN_TOP_CTRL_RESET_CTRL);

	switch (in_channelHandle->ucChannelNumber) {
		case 2:
			if (  in_enableHwRst ) /*Enable Inser Card Hardware Reset*/
				ulValue |= BCHP_SUN_TOP_CTRL_RESET_CTRL_sc2_insert_reset_en_MASK;
			else
				ulValue &= ~(BCHP_SUN_TOP_CTRL_RESET_CTRL_sc2_insert_reset_en_MASK);
			break;

		case 1:
			if (  in_enableHwRst ) /*Enable Inser Card Hardware Reset*/
				ulValue |= BCHP_SUN_TOP_CTRL_RESET_CTRL_sc1_insert_reset_en_MASK;
			else
				ulValue &= ~(BCHP_SUN_TOP_CTRL_RESET_CTRL_sc1_insert_reset_en_MASK);
			break;

		case 0:
			if (  in_enableHwRst ) /*Enable Inser Card Hardware Reset*/
				ulValue |= BCHP_SUN_TOP_CTRL_RESET_CTRL_sc0_insert_reset_en_MASK;
			else
				ulValue &= ~(BCHP_SUN_TOP_CTRL_RESET_CTRL_sc0_insert_reset_en_MASK);
			break;

		default:
			return -1;
	}

	BREG_Write32 (in_channelHandle->moduleHandle->regHandle, BCHP_SUN_TOP_CTRL_RESET_CTRL, ulValue);

#elif !defined(LINUX) &&  (BCHP_CHIP==3560)

	ulValue = BREG_Read32 (in_channelHandle->moduleHandle->regHandle,  BCHP_SUN_TOP_CTRL_RESET_CTRL);

	switch (in_channelHandle->ucChannelNumber) {
		case 0:
			if (  in_enableHwRst ) /*Enable Inser Card Hardware Reset*/
				ulValue |= BCHP_SUN_TOP_CTRL_RESET_CTRL_sc0_insert_reset_en_MASK;
			else
				ulValue &= ~(BCHP_SUN_TOP_CTRL_RESET_CTRL_sc0_insert_reset_en_MASK);
			break;

		default:
			return -1;
	}

	BREG_Write32 (in_channelHandle->moduleHandle->regHandle, BCHP_SUN_TOP_CTRL_RESET_CTRL, ulValue);
#else
	BSTD_UNUSED( ulValue );
	BSTD_UNUSED( in_enableHwRst);

#endif


BSCD_P_DONE_LABEL:

	BDBG_LEAVE(BSCD_Channel_InsCardHwReset);
	return( errCode );

}

BERR_Code BSCD_Channel_ResetCard(
		BSCD_ChannelHandle          in_channelHandle,
		BSCD_ResetCardAction               in_iccAction
)
{
	BERR_Code errCode = BERR_SUCCESS;

	BDBG_ENTER(BSCD_Channel_ResetCard);
	BDBG_ASSERT( in_channelHandle );

	BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
		(in_channelHandle->ulMagicNumber != BSCD_P_CHANNEL_HANDLE_MAGIC_NUMBER ) );

    if( in_channelHandle->currentChannelSettings.setVcc){

		if(BSCD_Channel_SetVccLevel(in_channelHandle,
				in_channelHandle->currentChannelSettings.vcc)){
				return BERR_UNKNOWN; 
				};
    	}
	switch (in_iccAction) {
		case BSCD_ResetCardAction_eNoAction:
			if (  (errCode = BSCD_Channel_P_Activating(in_channelHandle)) != BERR_SUCCESS) {
			       errCode = BERR_TRACE(errCode);
				goto BSCD_P_DONE_LABEL;
			}
			break;
		case BSCD_ResetCardAction_eReceiveAndDecode:
			BDBG_MSG(("BSCD_ResetCardAction_eReceiveAndDecode \n"));
			if (  ((errCode = BSCD_Channel_P_Activating(in_channelHandle)) != BERR_SUCCESS) ||
			       ((errCode = BSCD_Channel_P_ReceiveAndDecode(in_channelHandle)) != BERR_SUCCESS) ) {
			       errCode = BERR_TRACE(errCode);
				goto BSCD_P_DONE_LABEL;
			}
			break;

		default:
			BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, true);
			break;
	}


BSCD_P_DONE_LABEL:

	BDBG_LEAVE(BSCD_Channel_ResetCard);
	BDBG_MSG(("Leave ResetCard erroCode = 0x%x\n", errCode));
	return( errCode );
}


BERR_Code BSCD_Channel_GetStatus(
		BSCD_ChannelHandle          in_channelHandle,
		BSCD_Status                 *outp_status
)
{
	BERR_Code errCode = BERR_SUCCESS;

	BDBG_ENTER(BSCD_Channel_GetStatus);
	BDBG_ASSERT( in_channelHandle );

	BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
		(in_channelHandle->ulMagicNumber != BSCD_P_CHANNEL_HANDLE_MAGIC_NUMBER ) );

	*outp_status = in_channelHandle->channelStatus;

BSCD_P_DONE_LABEL:

	BDBG_LEAVE(BSCD_Channel_GetStatus);
	return( errCode );
}

BERR_Code BSCD_Channel_Transmit(
		BSCD_ChannelHandle          in_channelHandle,
		uint8_t                     *inp_ucXmitData,
		unsigned long                    in_ulNumXmitBytes
)
{
	if (in_channelHandle->currentChannelSettings.scStandard == BSCD_Standard_eIrdeto) {
		return (BSCD_Channel_P_T14IrdetoTransmit(
				in_channelHandle,
				inp_ucXmitData,
				in_ulNumXmitBytes));
	}
	else {
		return (BSCD_Channel_P_T0T1Transmit(
				in_channelHandle,
				inp_ucXmitData,
				in_ulNumXmitBytes));
	}
}


BERR_Code BSCD_Channel_Receive(
		BSCD_ChannelHandle       in_channelHandle,
		uint8_t                  		*outp_ucRcvData,
		unsigned long                  *outp_ulNumRcvBytes,
		unsigned long                	 in_ulMaxReadBytes
)
{
	BERR_Code 		errCode = BSCD_STATUS_READ_SUCCESS;
#ifndef BSCD_DSS_ICAM
	BSCD_Timer 		timer = {BSCD_TimerType_eWaitTimer, {BSCD_WaitTimerMode_eWorkWaitTime}, false, false};
#endif

	BDBG_ENTER(BSCD_Channel_Receive);
	BDBG_ASSERT( in_channelHandle );

	BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
		(in_channelHandle->ulMagicNumber != BSCD_P_CHANNEL_HANDLE_MAGIC_NUMBER ) );

	*outp_ulNumRcvBytes = 0;

	/* Coverity: 35203 */
	if(outp_ucRcvData == NULL) {
		BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, true);
	}

	if ((in_channelHandle->currentChannelSettings.eProtocolType == BSCD_AsyncProtocolType_e0) ||
		(in_channelHandle->currentChannelSettings.eProtocolType == BSCD_AsyncProtocolType_e14_IRDETO) ) {

		BSCD_P_CHECK_ERR_CODE_FUNC(errCode,
					BSCD_Channel_P_T0ReadData(in_channelHandle, outp_ucRcvData, outp_ulNumRcvBytes,
							in_ulMaxReadBytes));

		/*
			The Work Wait Timer is enabled in BSCD_Channel_P_T0T1Transmit. We cannot disable
			it in BSCD_Channel_P_T0ReadData since BSCD_Channel_P_T0ReadData is also used
			by reading ATR, which is one byte at a time.
		*/

#ifndef BSCD_DSS_ICAM 	/* BSYT leave this WWT enabled. We only disable WWT in transmit. */
		/*
			I assume all standards, other than EMV, will read all the bytes in BSCD_Channel_P_T0ReadData,
			therefore we couold safely disable the WWT here.  EMV only read 1 bytes at a time, therefore
			we have to disable WWT in the application
		*/
		if ((in_channelHandle->currentChannelSettings.scStandard != BSCD_Standard_eEMV1996) &&
			(in_channelHandle->currentChannelSettings.scStandard != BSCD_Standard_eEMV2000) )
		BSCD_P_CHECK_ERR_CODE_FUNC2(errCode, BSCD_STATUS_READ_FAILED,
			BSCD_Channel_EnableDisableTimer_isr(in_channelHandle, &timer));
#endif


	}/* BSCD_AsyncProtocolType_e0 */

	else { /* BSCD_AsyncProtocolType_e1 */

		BSCD_P_CHECK_ERR_CODE_FUNC(errCode,
					BSCD_Channel_P_T1ReadData(in_channelHandle, outp_ucRcvData, outp_ulNumRcvBytes,
							in_ulMaxReadBytes));

	} /* BSCD_AsyncProtocolType_e1 */

	if (*outp_ulNumRcvBytes > 0) {

		/* Ignore the ReadTimeOut error returned by SmartCardByteRead */
		/* BDBG_MSG (("success in SmartCardReadCmd\n")); */
	}

	else {
		BDBG_MSG (("No Response detected...deactivating, scerr = %02x\n",errCode));
		BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, true);
	}



BSCD_P_DONE_LABEL:

	BDBG_LEAVE(BSCD_Channel_Receive);
	/* BDBG_MSG(("Leave BSCD_Channel_Receive = 0x%x\n", errCode)); */
	return( errCode );
}

BERR_Code BSCD_Channel_ReceiveATR(
		BSCD_ChannelHandle       in_channelHandle,
		uint8_t                  		*outp_ucRcvData,
		unsigned long                  *outp_ulNumRcvBytes,
		unsigned long                	 in_ulMaxReadBytes
)
{
	BERR_Code 		errCode = BSCD_STATUS_READ_SUCCESS;

	BDBG_ENTER(BSCD_Channel_ReceiveATR);
	BDBG_ASSERT( in_channelHandle );

	BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
		(in_channelHandle->ulMagicNumber != BSCD_P_CHANNEL_HANDLE_MAGIC_NUMBER ) );

	*outp_ulNumRcvBytes = 0;

	if(outp_ucRcvData == NULL) {
		BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, true);
	}

	BSCD_P_CHECK_ERR_CODE_FUNC(errCode,
				BSCD_Channel_P_T0ReadData(in_channelHandle, outp_ucRcvData, outp_ulNumRcvBytes,
						in_ulMaxReadBytes));

	if (*outp_ulNumRcvBytes > 0) {

		/*
		For T=0, we depend on timeout to
		identify that there is no more byte to be received
		*/

		/* Ignore the ReadTimeOut error returned by SmartCardByteRead */
		/* BDBG_MSG (("success in SmartCardReadCmd\n")); */
	}

	else {
		BDBG_MSG (("No Response detected...deactivating, scerr = %02x\n",errCode));
		BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, true);
	}



BSCD_P_DONE_LABEL:

	BDBG_LEAVE(BSCD_Channel_ReceiveATR);
	/* BDBG_MSG(("Leave BSCD_Channel_ReceiveATR = 0x%x\n", errCode)); */
	return( errCode );
}

BERR_Code BSCD_Channel_ConfigTimer(
		BSCD_ChannelHandle          in_channelHandle,
		BSCD_Timer 		            *inp_timer,
		BSCD_TimerValue             *inp_unCount

)
{
	BERR_Code errCode = BERR_SUCCESS;
	uint32_t			ulTimerCmdVal, ulTimerCmpVal;

	BDBG_ENTER(BSCD_Channel_ConfigTimer);
	BDBG_ASSERT( in_channelHandle );

	BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
		(in_channelHandle->ulMagicNumber != BSCD_P_CHANNEL_HANDLE_MAGIC_NUMBER ) );

	if (inp_timer->eTimerType == BSCD_TimerType_eGPTimer) {

		/* Always disbale timer first before we change timer_cmd */
		ulTimerCmdVal = BREG_Read32(
					in_channelHandle->moduleHandle->regHandle,
					(in_channelHandle->ulRegStartAddr + BSCD_P_TIMER_CMD));

		ulTimerCmdVal &= (~BCHP_SCA_SC_TIMER_CMD_timer_en_MASK);

		BREG_Write32(
					in_channelHandle->moduleHandle->regHandle,
					(in_channelHandle->ulRegStartAddr + BSCD_P_TIMER_CMD),
					ulTimerCmdVal);

		BKNI_EnterCriticalSection();
		in_channelHandle->ulIntrStatus1  &= ~BCHP_SCA_SC_INTR_STAT_1_timer_intr_MASK;
		BKNI_LeaveCriticalSection();

		/* Set timer_cmp registers */

		ulTimerCmpVal = ((inp_unCount->ulValue & 0xFF00) >> 8);
		BREG_Write32(
					in_channelHandle->moduleHandle->regHandle,
					(in_channelHandle->ulRegStartAddr + BSCD_P_TIMER_CMP_2),
					ulTimerCmpVal);

		ulTimerCmpVal = inp_unCount->ulValue & 0x00FF;
		BREG_Write32(
					in_channelHandle->moduleHandle->regHandle,
					(in_channelHandle->ulRegStartAddr + BSCD_P_TIMER_CMP_1),
					ulTimerCmpVal);


		/* Set the timer unit and mode */
		if ( inp_unCount->unit == BSCD_TimerUnit_eCLK) {
			ulTimerCmdVal |= BCHP_SCA_SC_TIMER_CMD_timer_src_MASK;
		}
		else if (inp_unCount->unit  == BSCD_TimerUnit_eETU) {
			ulTimerCmdVal &= (~BCHP_SCA_SC_TIMER_CMD_timer_src_MASK);
		}
		else {
			BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, true);
		}

		if (inp_timer->timerMode.eGPTimerMode == BSCD_GPTimerMode_eNEXT_START_BIT ) {
			ulTimerCmdVal |= BCHP_SCA_SC_TIMER_CMD_timer_mode_MASK;
		}
		else {  /* BSCD_GPTimerMode_eIMMEDIATE */
			ulTimerCmdVal &= (~BCHP_SCA_SC_TIMER_CMD_timer_mode_MASK);
		}

		/* Check if we need to invoke an interrupt when the time expires */
		if (inp_timer->bIsTimerInterruptEnable == true) {  /* inp_timer->bIsTimerInterruptEnable == true && BSCD_TimerType_eGPTimer */
			BSCD_P_CHECK_ERR_CODE_FUNC(errCode,
					BSCD_Channel_EnableIntrCallback_isr (
						in_channelHandle, BSCD_IntType_eTimerInt,
			               		BSCD_Channel_P_TimerCB_isr));
		}
		else { /* inp_timer->bIsTimerInterruptEnable == false && BSCD_TimerType_eGPTimer */
			BSCD_P_CHECK_ERR_CODE_FUNC(errCode,
					BSCD_Channel_DisableIntrCallback_isr (
						in_channelHandle, BSCD_IntType_eTimerInt));
		}

	 	if (inp_timer->bIsTimerEnable == true) {
			ulTimerCmdVal	 |= BCHP_SCA_SC_TIMER_CMD_timer_en_MASK;
	 	} /* inp_timer->bIsTimerEnable == true && BSCD_TimerType_eGPTimer */

		else { /* inp_timer->bIsTimerEnable == false && BSCD_TimerType_eGPTimer */
			ulTimerCmdVal	 &= ~BCHP_SCA_SC_TIMER_CMD_timer_en_MASK;

		}

	}  /* if (inp_timer->eTimerType == BSCD_TimerType_eGPTimer) */

	else {  /* BSCD_TimerType_eWaitTimer */

		/* Always disable timer first before we change timer_cmd */
		ulTimerCmdVal = BREG_Read32(
					in_channelHandle->moduleHandle->regHandle,
					(in_channelHandle->ulRegStartAddr + BSCD_P_TIMER_CMD));

		ulTimerCmdVal &= (~BCHP_SCA_SC_TIMER_CMD_wait_en_MASK);

		BREG_Write32(
					in_channelHandle->moduleHandle->regHandle,
					(in_channelHandle->ulRegStartAddr + BSCD_P_TIMER_CMD),
					ulTimerCmdVal);

		BKNI_EnterCriticalSection();
		in_channelHandle->ulIntrStatus2  &= ~BCHP_SCA_SC_INTR_STAT_2_wait_intr_MASK;
		BKNI_LeaveCriticalSection();

		/* Set sc_wait registers */
		ulTimerCmpVal = ((inp_unCount->ulValue  & 0xFF0000) >> 16);
		BREG_Write32(
					in_channelHandle->moduleHandle->regHandle,
					(in_channelHandle->ulRegStartAddr + BSCD_P_WAIT_3),
					ulTimerCmpVal);

		ulTimerCmpVal = ((inp_unCount->ulValue & 0x00FF00) >> 8);
		BREG_Write32(
					in_channelHandle->moduleHandle->regHandle,
					(in_channelHandle->ulRegStartAddr + BSCD_P_WAIT_2),
					ulTimerCmpVal);

		ulTimerCmpVal = (inp_unCount->ulValue & 0x0000FF);
		BREG_Write32(
					in_channelHandle->moduleHandle->regHandle,
					(in_channelHandle->ulRegStartAddr + BSCD_P_WAIT_1),
					ulTimerCmpVal);

		/* Check if we need to invoke an interrupt when the time expires */
		if (inp_timer->bIsTimerInterruptEnable == true) {
			BSCD_P_CHECK_ERR_CODE_FUNC(errCode,
					BSCD_Channel_EnableIntrCallback_isr (
						in_channelHandle, BSCD_IntType_eWaitInt,
			               		BSCD_Channel_P_WaitCB_isr));
		}
		else {
			BSCD_P_CHECK_ERR_CODE_FUNC(errCode,
					BSCD_Channel_DisableIntrCallback_isr (
						in_channelHandle, BSCD_IntType_eWaitInt));
		}

	 	if (inp_timer->bIsTimerEnable == true) {

			/* Set the wait mode */
			if (inp_timer->eTimerType == BSCD_TimerType_eWaitTimer) {
				if (inp_timer->timerMode.eWaitTimerMode == BSCD_WaitTimerMode_eBlockWaitTime) {
					ulTimerCmdVal |= BCHP_SCA_SC_TIMER_CMD_wait_mode_MASK;
				}
				else { /* BSCD_WaitTimerMode_eWorkWaitTime */
					ulTimerCmdVal &= ~ BCHP_SCA_SC_TIMER_CMD_wait_mode_MASK;
				}

				ulTimerCmdVal |= BCHP_SCA_SC_TIMER_CMD_wait_en_MASK;
			}

	 	}/* BSCD_TimerType_eWaitTimer && inp_timer->bIsTimerEnable == true */

		else { /* inp_timer->bIsTimerEnable == false && BSCD_TimerType_eWaitTimer */
    			ulTimerCmdVal &= ~BCHP_SCA_SC_TIMER_CMD_wait_en_MASK;
		}


	}  /* else  BSCD_TimerType_eWaitTimer */
	BREG_Write32(
					in_channelHandle->moduleHandle->regHandle,
					(in_channelHandle->ulRegStartAddr + BSCD_P_TIMER_CMD),
					ulTimerCmdVal);

	BDBG_MSG (("*** BSCD_Channel_ConfigTimer: Timer cmd = 0x%08x\n", ulTimerCmdVal));

BSCD_P_DONE_LABEL:

	BDBG_LEAVE(BSCD_Channel_ConfigTimer);
	return( errCode );
}

BERR_Code BSCD_Channel_EnableDisableTimer_isr(
		BSCD_ChannelHandle   in_channelHandle,
		BSCD_Timer                  *inp_timer
)
{
	BERR_Code errCode = BERR_SUCCESS;
	uint32_t         ulTimerCmdVal;

	BDBG_ENTER(BSCD_Channel_EnableDisableTimer_isr);
	BDBG_ASSERT( in_channelHandle );

	BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,
		(in_channelHandle->ulMagicNumber != BSCD_P_CHANNEL_HANDLE_MAGIC_NUMBER ) );


	ulTimerCmdVal = BREG_Read32(
					in_channelHandle->moduleHandle->regHandle,
					(in_channelHandle->ulRegStartAddr + BSCD_P_TIMER_CMD));

	if (inp_timer->eTimerType == BSCD_TimerType_eGPTimer) {

		in_channelHandle->ulIntrStatus1  &= ~BCHP_SCA_SC_INTR_STAT_1_timer_intr_MASK;

		/* Check if we need to invoke an interrupt when the time expires */
		if (inp_timer->bIsTimerInterruptEnable == true) {  /* inp_timer->bIsTimerInterruptEnable == true && BSCD_TimerType_eGPTimer */
			BSCD_P_CHECK_ERR_CODE_FUNC(errCode,
					BSCD_Channel_EnableIntrCallback_isr (
						in_channelHandle, BSCD_IntType_eTimerInt,
			               		BSCD_Channel_P_TimerCB_isr));
		}
		else { /* inp_timer->bIsTimerInterruptEnable == false && BSCD_TimerType_eGPTimer */
			BSCD_P_CHECK_ERR_CODE_FUNC(errCode,
					BSCD_Channel_DisableIntrCallback_isr (
						in_channelHandle, BSCD_IntType_eTimerInt));
		}

	 	if (inp_timer->bIsTimerEnable == true) {
			ulTimerCmdVal	 |= BCHP_SCA_SC_TIMER_CMD_timer_en_MASK;
	 	} /* inp_timer->bIsTimerEnable == true && BSCD_TimerType_eGPTimer */

		else { /* inp_timer->bIsTimerEnable == false && BSCD_TimerType_eGPTimer */
			ulTimerCmdVal	 &= ~BCHP_SCA_SC_TIMER_CMD_timer_en_MASK;
		}


	}  /* if (inp_timer->eTimerType == BSCD_TimerType_eGPTimer) */

	else {  /* BSCD_TimerType_eWaitTimer */

        in_channelHandle->ulIntrStatus2  &= ~BCHP_SCA_SC_INTR_STAT_2_wait_intr_MASK;

		/* Check if we need to invoke an interrupt when the time expires */
		if (inp_timer->bIsTimerInterruptEnable == true) {
			BSCD_P_CHECK_ERR_CODE_FUNC(errCode,
					BSCD_Channel_EnableIntrCallback_isr (
						in_channelHandle, BSCD_IntType_eWaitInt,
			               		BSCD_Channel_P_WaitCB_isr));
		}
		else {
			BSCD_P_CHECK_ERR_CODE_FUNC(errCode,
					BSCD_Channel_DisableIntrCallback_isr (
						in_channelHandle, BSCD_IntType_eWaitInt));
		}

	 	if (inp_timer->bIsTimerEnable == true) {

			/* Set the wait mode */
			if (inp_timer->eTimerType == BSCD_TimerType_eWaitTimer) {
				if (inp_timer->timerMode.eWaitTimerMode == BSCD_WaitTimerMode_eBlockWaitTime) {
					ulTimerCmdVal |= BCHP_SCA_SC_TIMER_CMD_wait_mode_MASK;
				}
				else { /* BSCD_WaitTimerMode_eWorkWaitTime */
					ulTimerCmdVal &= ~ BCHP_SCA_SC_TIMER_CMD_wait_mode_MASK;
				}

				ulTimerCmdVal |= BCHP_SCA_SC_TIMER_CMD_wait_en_MASK;
			}

	 	}/* BSCD_TimerType_eWaitTimer && inp_timer->bIsTimerEnable == true */

		else { /* inp_timer->bIsTimerEnable == false && BSCD_TimerType_eWaitTimer */
    			ulTimerCmdVal &= ~BCHP_SCA_SC_TIMER_CMD_wait_en_MASK;
		}


	}  /* else  BSCD_TimerType_eWaitTimer */

	BREG_Write32(
					in_channelHandle->moduleHandle->regHandle,
					(in_channelHandle->ulRegStartAddr + BSCD_P_TIMER_CMD),
					ulTimerCmdVal);

	BDBG_MSG (("*** BSCD_Channel_EnableDisableTimer_isr: Timer cmd = 0x%08x\n", ulTimerCmdVal));

BSCD_P_DONE_LABEL:

	BDBG_LEAVE(BSCD_Channel_EnableDisableTimer_isr);
	return( errCode );
}

BERR_Code BSCD_Channel_EnableIntrCallback_isr(
	BSCD_ChannelHandle	in_channelHandle,
	BSCD_IntrType		in_eIntType,
	BSCD_IsrCallbackFunc in_callback
)
{
	uint32_t  ulVal;
	unsigned int  unReg = BSCD_P_INTR_EN_1, i;
	BERR_Code errCode = BERR_SUCCESS;

	if ( (in_eIntType == BSCD_IntType_eTParityInt)    ||
	    (in_eIntType == BSCD_IntType_eTimerInt)      ||
	    (in_eIntType == BSCD_IntType_eCardInsertInt) ||
	    (in_eIntType == BSCD_IntType_eCardRemoveInt) ||
	    (in_eIntType == BSCD_IntType_eBGTInt)        ||
	    (in_eIntType == BSCD_IntType_eTDoneInt)      ||
	    (in_eIntType == BSCD_IntType_eRetryInt)      ||
	    (in_eIntType == BSCD_IntType_eTEmptyInt) ||
	    (in_eIntType == BSCD_IntType_eEvent1Int)) {
		unReg = BSCD_P_INTR_EN_1;
		/* BDBG_MSG(("BSCD_P_INTR_EN_1: ")); */
	}
	else if ( (in_eIntType == BSCD_IntType_eRParityInt) ||
	    (in_eIntType == BSCD_IntType_eATRInt)          ||
	    (in_eIntType == BSCD_IntType_eCWTInt)          ||
	    (in_eIntType == BSCD_IntType_eRLenInt)         ||
	    (in_eIntType == BSCD_IntType_eWaitInt)         ||
	    (in_eIntType == BSCD_IntType_eRcvInt)          ||
	    (in_eIntType == BSCD_IntType_eRReadyInt) ||
	    (in_eIntType == BSCD_IntType_eEvent2Int)) {
		unReg = BSCD_P_INTR_EN_2;
		/* BDBG_MSG(("BSCD_P_INTR_EN_2: ")); */
	}
	else if (in_eIntType == BSCD_IntType_eEDCInt) {
		unReg = BSCD_P_PROTO_CMD;
	}
	else {
		BDBG_ERR(("Interrupt not supported, in_eIntType = %d\n", in_eIntType));
		BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,  true);
	}

	ulVal = BREG_Read32(
			in_channelHandle->moduleHandle->regHandle,
			(in_channelHandle->ulRegStartAddr + unReg));

	/* BDBG_MSG(("ulVal = 0x%x", ulVal)); */

	switch (in_eIntType) {

		case BSCD_IntType_eTParityInt:
			for (i=0; i< BSCD_MAX_NUM_CALLBACK_FUNC; i++)  {
				if (in_channelHandle->callBack.tParityIsrCBFunc[i] == NULL) {
					in_channelHandle->callBack.tParityIsrCBFunc[i] = in_callback;
					break;
				}
				else if ((in_channelHandle->callBack.tParityIsrCBFunc[i] != NULL) &&
					(in_channelHandle->callBack.tParityIsrCBFunc[i] == in_callback) ) {
					break;
				}
			}
			ulVal |=  BCHP_SCA_SC_INTR_STAT_1_tpar_intr_MASK;
			break;

		case BSCD_IntType_eTimerInt:
			for (i=0; i< BSCD_MAX_NUM_CALLBACK_FUNC; i++)  {
				if (in_channelHandle->callBack.timerIsrCBFunc[i] == NULL) {
					in_channelHandle->callBack.timerIsrCBFunc[i] = in_callback;
					break;
				}
				else if ((in_channelHandle->callBack.timerIsrCBFunc[i] != NULL) &&
					(in_channelHandle->callBack.timerIsrCBFunc[i] == in_callback) ) {
					break;
				}
			}
			ulVal |=  BCHP_SCA_SC_INTR_STAT_1_timer_intr_MASK;
			break;

		case BSCD_IntType_eCardInsertInt:
			for (i=0; i< BSCD_MAX_NUM_CALLBACK_FUNC; i++)  {
				if (in_channelHandle->callBack.cardInsertIsrCBFunc[i] == NULL) {
					in_channelHandle->callBack.cardInsertIsrCBFunc[i] = in_callback;
					BDBG_MSG(("new BSCD_IntType_eCardInsertInt  callback "));
					break;
				}
				else if ((in_channelHandle->callBack.cardInsertIsrCBFunc[i] != NULL) &&
					(in_channelHandle->callBack.cardInsertIsrCBFunc[i] == in_callback) ) {
					BDBG_MSG(("BSCD_IntType_eCardInsertInt same callback "));
					break;
				}
			}
			ulVal |=  BCHP_SCA_SC_INTR_STAT_1_pres_intr_MASK;
			break;

		case BSCD_IntType_eCardRemoveInt:
			for (i=0; i< BSCD_MAX_NUM_CALLBACK_FUNC; i++)  {
				if (in_channelHandle->callBack.cardRemoveIsrCBFunc[i] == NULL) {
					in_channelHandle->callBack.cardRemoveIsrCBFunc[i] = in_callback;
					BDBG_MSG(("new BSCD_IntType_eCardRemoveInt  callback "));
					break;
				}
				else if ((in_channelHandle->callBack.cardRemoveIsrCBFunc[i] != NULL) &&
					(in_channelHandle->callBack.cardRemoveIsrCBFunc[i] == in_callback) ) {
					BDBG_MSG(("BSCD_IntType_eCardRemoveInt same callback "));
					break;
				}
			}
			ulVal |=  BCHP_SCA_SC_INTR_STAT_1_pres_intr_MASK;
			break;

		case BSCD_IntType_eBGTInt:
			for (i=0; i< BSCD_MAX_NUM_CALLBACK_FUNC; i++)  {
				if (in_channelHandle->callBack.bgtIsrCBFunc[i] == NULL) {
					in_channelHandle->callBack.bgtIsrCBFunc[i] = in_callback;
					break;
				}
				else if ((in_channelHandle->callBack.bgtIsrCBFunc[i] != NULL) &&
					(in_channelHandle->callBack.bgtIsrCBFunc[i] == in_callback) ) {
					break;
				}
			}
			ulVal |=  BCHP_SCA_SC_INTR_STAT_1_bgt_intr_MASK;
			break;

		case BSCD_IntType_eTDoneInt:
			for (i=0; i< BSCD_MAX_NUM_CALLBACK_FUNC; i++)  {
				if (in_channelHandle->callBack.tDoneIsrCBFunc[i] == NULL) {
					in_channelHandle->callBack.tDoneIsrCBFunc[i] = in_callback;
					break;
				}
				else if ((in_channelHandle->callBack.tDoneIsrCBFunc[i] != NULL) &&
					(in_channelHandle->callBack.tDoneIsrCBFunc[i] == in_callback) ) {
					break;
				}
			}
			ulVal |=  BCHP_SCA_SC_INTR_STAT_1_tdone_intr_MASK;
			break;

		case BSCD_IntType_eRetryInt:
			for (i=0; i< BSCD_MAX_NUM_CALLBACK_FUNC; i++)  {
				if (in_channelHandle->callBack.retryIsrCBFunc[i] == NULL) {
					in_channelHandle->callBack.retryIsrCBFunc[i] = in_callback;
					break;
				}
				else if ((in_channelHandle->callBack.retryIsrCBFunc[i] != NULL) &&
					(in_channelHandle->callBack.retryIsrCBFunc[i] == in_callback) ) {
					break;
				}
			}
			ulVal |=  BCHP_SCA_SC_INTR_STAT_1_retry_intr_MASK;
			break;

		case BSCD_IntType_eTEmptyInt:
			for (i=0; i< BSCD_MAX_NUM_CALLBACK_FUNC; i++)  {
				if (in_channelHandle->callBack.tEmptyIsrCBFunc[i] == NULL) {
					in_channelHandle->callBack.tEmptyIsrCBFunc[i] = in_callback;
					break;
				}
				else if ((in_channelHandle->callBack.tEmptyIsrCBFunc[i] != NULL) &&
					(in_channelHandle->callBack.tEmptyIsrCBFunc[i] == in_callback) ) {
					break;
				}
			}
			ulVal |=  BCHP_SCA_SC_INTR_STAT_1_tempty_intr_MASK;
			break;

		case BSCD_IntType_eRParityInt:
			for (i=0; i< BSCD_MAX_NUM_CALLBACK_FUNC; i++)  {
				if (in_channelHandle->callBack.rParityIsrCBFunc[i] == NULL) {
					in_channelHandle->callBack.rParityIsrCBFunc[i] = in_callback;
					break;
				}
				else if ((in_channelHandle->callBack.rParityIsrCBFunc[i] != NULL) &&
					(in_channelHandle->callBack.rParityIsrCBFunc[i] == in_callback) ) {
					break;
				}
			}
			ulVal |=  BCHP_SCA_SC_INTR_STAT_2_rpar_intr_MASK;
			break;

		case BSCD_IntType_eATRInt:
			for (i=0; i< BSCD_MAX_NUM_CALLBACK_FUNC; i++)  {
				if (in_channelHandle->callBack.atrIsrCBFunc[i] == NULL) {
					in_channelHandle->callBack.atrIsrCBFunc[i] = in_callback;
					break;
				}
				else if ((in_channelHandle->callBack.atrIsrCBFunc[i] != NULL) &&
					(in_channelHandle->callBack.atrIsrCBFunc[i] == in_callback) ) {
					break;
				}
			}
			ulVal |=  BCHP_SCA_SC_INTR_STAT_2_atrs_intr_MASK;
			break;

		case BSCD_IntType_eCWTInt:
			for (i=0; i< BSCD_MAX_NUM_CALLBACK_FUNC; i++)  {
				if (in_channelHandle->callBack.cwtIsrCBFunc[i] == NULL) {
					in_channelHandle->callBack.cwtIsrCBFunc[i] = in_callback;
					break;
				}
				else if ((in_channelHandle->callBack.cwtIsrCBFunc[i] != NULL) &&
					(in_channelHandle->callBack.cwtIsrCBFunc[i] == in_callback) ) {
					break;
				}
			}
			ulVal |=  BCHP_SCA_SC_INTR_STAT_2_cwt_intr_MASK;
			break;

		case BSCD_IntType_eRLenInt:
			for (i=0; i< BSCD_MAX_NUM_CALLBACK_FUNC; i++)  {
				if (in_channelHandle->callBack.rLenIsrCBFunc[i] == NULL) {
					in_channelHandle->callBack.rLenIsrCBFunc[i] = in_callback;
					break;
				}
				else if ((in_channelHandle->callBack.rLenIsrCBFunc[i] != NULL) &&
					(in_channelHandle->callBack.rLenIsrCBFunc[i] == in_callback) ) {
					break;
				}
			}
			ulVal |=  BCHP_SCA_SC_INTR_STAT_2_rlen_intr_MASK;
			break;

		case BSCD_IntType_eWaitInt:
			for (i=0; i< BSCD_MAX_NUM_CALLBACK_FUNC; i++)  {
				if (in_channelHandle->callBack.waitIsrCBFunc[i] == NULL) {
					in_channelHandle->callBack.waitIsrCBFunc[i] = in_callback;
					break;
				}
				else if ((in_channelHandle->callBack.waitIsrCBFunc[i] != NULL) &&
					(in_channelHandle->callBack.waitIsrCBFunc[i] == in_callback) ) {
					break;
				}
			}
			ulVal |=  BCHP_SCA_SC_INTR_STAT_2_wait_intr_MASK;
			break;

		case BSCD_IntType_eRcvInt:
			for (i=0; i< BSCD_MAX_NUM_CALLBACK_FUNC; i++)  {
				if (in_channelHandle->callBack.rcvIsrCBFunc[i] == NULL) {
					in_channelHandle->callBack.rcvIsrCBFunc[i] = in_callback;
					break;
				}
				else if ((in_channelHandle->callBack.rcvIsrCBFunc[i] != NULL) &&
					(in_channelHandle->callBack.rcvIsrCBFunc[i] == in_callback) ) {
					break;
				}
			}
			ulVal |=  BCHP_SCA_SC_INTR_STAT_2_rcv_intr_MASK;
			break;

		case BSCD_IntType_eRReadyInt:
			for (i=0; i< BSCD_MAX_NUM_CALLBACK_FUNC; i++)  {
				if (in_channelHandle->callBack.rReadyIsrCBFunc[i] == NULL) {
					in_channelHandle->callBack.rReadyIsrCBFunc[i] = in_callback;
					break;
				}
				else if ((in_channelHandle->callBack.rReadyIsrCBFunc[i] != NULL) &&
					(in_channelHandle->callBack.rReadyIsrCBFunc[i] == in_callback) ) {
					break;
				}
			}
			ulVal |=  BCHP_SCA_SC_INTR_STAT_2_rready_intr_MASK;
			break;

		case BSCD_IntType_eEDCInt:
			if (in_channelHandle->currentChannelSettings.eProtocolType == BSCD_AsyncProtocolType_e0 ) {
				for (i=0; i< BSCD_MAX_NUM_CALLBACK_FUNC; i++)  {
					if (in_channelHandle->callBack.edcIsrCBFunc[i] == NULL) {
						in_channelHandle->callBack.edcIsrCBFunc[i] = in_callback;
						break;
					}
					else if ((in_channelHandle->callBack.edcIsrCBFunc[i] != NULL) &&
						(in_channelHandle->callBack.edcIsrCBFunc[i] == in_callback) ) {
						break;
					}
				}
				ulVal |=  BCHP_SCA_SC_PROTO_CMD_edc_en_MASK;
			}
			break;

		case BSCD_IntType_eEvent1Int:
			for (i=0; i< BSCD_MAX_NUM_CALLBACK_FUNC; i++)  {
				if (in_channelHandle->callBack.event1IsrCBFunc[i] == NULL) {
					in_channelHandle->callBack.event1IsrCBFunc[i] = in_callback;
					break;
				}
				else if ((in_channelHandle->callBack.event1IsrCBFunc[i] != NULL) &&
					(in_channelHandle->callBack.event1IsrCBFunc[i] == in_callback) ) {
					break;
				}
			}
			ulVal |=  BCHP_SCA_SC_INTR_STAT_1_event1_intr_MASK;
			break;

		case BSCD_IntType_eEvent2Int:
			for (i=0; i< BSCD_MAX_NUM_CALLBACK_FUNC; i++)  {
				if (in_channelHandle->callBack.event2IsrCBFunc[i] == NULL) {
					in_channelHandle->callBack.event2IsrCBFunc[i] = in_callback;
					break;
				}
				else if ((in_channelHandle->callBack.event2IsrCBFunc[i] != NULL) &&
					(in_channelHandle->callBack.event2IsrCBFunc[i] == in_callback) ) {
					break;
				}
			}
			ulVal |=  BCHP_SCA_SC_INTR_STAT_2_event2_intr_MASK;
			break;

		default:
			BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, true);
	}

	BREG_Write32(
			in_channelHandle->moduleHandle->regHandle,
			(in_channelHandle->ulRegStartAddr + unReg),
			 ulVal);


	/*BDBG_MSG((", final ulVal = 0x%x\n ", ulVal)); */

BSCD_P_DONE_LABEL:

	BDBG_LEAVE(BSCD_Channel_EnableIntrCallback_isr);
	return( errCode );

}

BERR_Code BSCD_Channel_DisableIntrCallback_isr(
	BSCD_ChannelHandle	in_channelHandle,
	BSCD_IntrType    in_eIntType
)
{
	uint32_t ulVal;
	unsigned int  unReg = BSCD_P_INTR_EN_1;
	BERR_Code errCode = BERR_SUCCESS;

	if ( (in_eIntType == BSCD_IntType_eTParityInt)    ||
		(in_eIntType == BSCD_IntType_eTimerInt)      ||
		(in_eIntType == BSCD_IntType_eCardInsertInt) ||
		(in_eIntType == BSCD_IntType_eCardRemoveInt) ||
		(in_eIntType == BSCD_IntType_eBGTInt)        ||
		(in_eIntType == BSCD_IntType_eTDoneInt)      ||
		(in_eIntType == BSCD_IntType_eRetryInt)      ||
		(in_eIntType == BSCD_IntType_eTEmptyInt) ||
	    	(in_eIntType == BSCD_IntType_eEvent1Int)) {
		unReg = BSCD_P_INTR_EN_1;
		/* BDBG_MSG(("BSCD_P_INTR_EN_1: "));		 */
	}
	else if ( (in_eIntType == BSCD_IntType_eRParityInt) ||
		(in_eIntType == BSCD_IntType_eATRInt)          ||
		(in_eIntType == BSCD_IntType_eCWTInt)          ||
		(in_eIntType == BSCD_IntType_eRLenInt)         ||
		(in_eIntType == BSCD_IntType_eWaitInt)         ||
		(in_eIntType == BSCD_IntType_eRcvInt)          ||
		(in_eIntType == BSCD_IntType_eRReadyInt) ||
	   	 (in_eIntType == BSCD_IntType_eEvent2Int)) {
		unReg = BSCD_P_INTR_EN_2;
		/* BDBG_MSG(("BSCD_P_INTR_EN_2: "));		 */
	}
	else if (in_eIntType == BSCD_IntType_eEDCInt) {
		unReg = BSCD_P_PROTO_CMD;
	}
	else {
		BDBG_MSG(("Interrupt not supported\n"));
		BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED,  true);
	}

	ulVal = BREG_Read32(
		in_channelHandle->moduleHandle->regHandle,
		(in_channelHandle->ulRegStartAddr + unReg));

	/* BDBG_MSG(("ulVal = 0x%x", ulVal)); */

	switch (in_eIntType) {


		case BSCD_IntType_eTParityInt:
			 ulVal &= ~BCHP_SCA_SC_INTR_STAT_1_tpar_intr_MASK;
			 break;

		case BSCD_IntType_eTimerInt:
			ulVal &= ~BCHP_SCA_SC_INTR_STAT_1_timer_intr_MASK;
			break;

		case BSCD_IntType_eCardInsertInt:
			ulVal &= ~BCHP_SCA_SC_INTR_STAT_1_pres_intr_MASK;
			break;

		case BSCD_IntType_eCardRemoveInt:
			ulVal &= ~BCHP_SCA_SC_INTR_STAT_1_pres_intr_MASK;
			break;

		case BSCD_IntType_eBGTInt:
			ulVal &= ~BCHP_SCA_SC_INTR_STAT_1_bgt_intr_MASK;
			break;

		case BSCD_IntType_eTDoneInt:
			ulVal &= ~BCHP_SCA_SC_INTR_STAT_1_tdone_intr_MASK;
			break;

		case BSCD_IntType_eRetryInt:
			ulVal &= ~BCHP_SCA_SC_INTR_STAT_1_retry_intr_MASK;
			break;

		case BSCD_IntType_eTEmptyInt:
			ulVal &= ~BCHP_SCA_SC_INTR_STAT_1_tempty_intr_MASK;
			break;

		case BSCD_IntType_eRParityInt:
			ulVal &= ~BCHP_SCA_SC_INTR_STAT_2_rpar_intr_MASK;
			break;

		case BSCD_IntType_eATRInt:
			ulVal &= ~BCHP_SCA_SC_INTR_STAT_2_atrs_intr_MASK;
			break;

		case BSCD_IntType_eCWTInt:
			ulVal &= ~BCHP_SCA_SC_INTR_STAT_2_cwt_intr_MASK;
			break;

		case BSCD_IntType_eRLenInt:
			ulVal &= ~BCHP_SCA_SC_INTR_STAT_2_rlen_intr_MASK;
			break;

		case BSCD_IntType_eWaitInt:
			ulVal &= ~BCHP_SCA_SC_INTR_STAT_2_wait_intr_MASK;
			break;

		case BSCD_IntType_eRcvInt:
			ulVal &= ~BCHP_SCA_SC_INTR_STAT_2_rcv_intr_MASK;
			break;

		case BSCD_IntType_eRReadyInt:
			ulVal &= ~BCHP_SCA_SC_INTR_STAT_2_rready_intr_MASK;
			break;

		case BSCD_IntType_eEDCInt:
			ulVal &= ~BCHP_SCA_SC_PROTO_CMD_edc_en_MASK;
			break;

		case BSCD_IntType_eEvent1Int:
			ulVal &= ~BCHP_SCA_SC_INTR_STAT_1_event1_intr_MASK;
			break;

		case BSCD_IntType_eEvent2Int:
			ulVal &=  ~BCHP_SCA_SC_INTR_STAT_2_event2_intr_MASK;
			break;

		default:
			BSCD_P_CHECK_ERR_CODE_CONDITION( errCode, BSCD_STATUS_FAILED, true);
	}

	BREG_Write32(
			in_channelHandle->moduleHandle->regHandle,
			(in_channelHandle->ulRegStartAddr + unReg),
			 ulVal);

	/* BDBG_MSG((", final ulVal = 0x%x\n ", ulVal)); */
BSCD_P_DONE_LABEL:

	BDBG_LEAVE(BSCD_Channel_DisableIntrCallback_isr);
	return( errCode );
}

BERR_Code BSCD_Channel_EnableInterrupts(
	BSCD_ChannelHandle	in_channelHandle
)
{
	BERR_Code errCode = BERR_SUCCESS;

	BDBG_ENTER(BSCD_Channel_EnableInterrupts);
	BDBG_ASSERT( in_channelHandle );

	BKNI_EnterCriticalSection();
	if ( (errCode = BSCD_Channel_P_EnableInterrupts_isr(in_channelHandle)) != BERR_SUCCESS) {
		errCode = BERR_TRACE(errCode);
		goto BSCD_P_DONE_LABEL;
	}
	BKNI_LeaveCriticalSection();

BSCD_P_DONE_LABEL:

	BDBG_LEAVE(BSCD_Channel_EnableInterrupts);
	return( errCode );
}

BERR_Code BSCD_Channel_ResetBlockWaitTimer(
		BSCD_ChannelHandle          in_channelHandle
)
{
	BERR_Code errCode = BERR_SUCCESS;
	BSCD_Timer 		timer = {BSCD_TimerType_eWaitTimer, {BSCD_GPTimerMode_eIMMEDIATE},
							false, true};
	BSCD_TimerValue    timeValue= {BSCD_DEFAULT_BLOCK_WAITING_TIME, BSCD_TimerUnit_eETU};

	BDBG_ENTER(BSCD_Channel_ResetBlockWaitTimer);
	BDBG_ASSERT( in_channelHandle );

	/* Need this for MetroWerks */
	timer.eTimerType = BSCD_TimerType_eWaitTimer;
	timer.timerMode.eWaitTimerMode = BSCD_WaitTimerMode_eBlockWaitTime;

	timeValue.ulValue = in_channelHandle->currentChannelSettings.blockWaitTime.ulValue ;
	BSCD_P_CHECK_ERR_CODE_FUNC(errCode,
			BSCD_Channel_ConfigTimer(in_channelHandle, &timer, &timeValue));

	in_channelHandle->currentChannelSettings.blockWaitTimeExt.ulValue = 0;

BSCD_P_DONE_LABEL:

	BDBG_LEAVE(BSCD_Channel_ResetBlockWaitTimer);
	return( errCode );
}


BERR_Code BSCD_Channel_SetBlockWaitTimeExt(
		BSCD_ChannelHandle          in_channelHandle,
		uint32_t 				    in_ulBlockWaitTimeExtInETU
)
{
	BERR_Code errCode = BERR_SUCCESS;

	BDBG_ENTER(BSCD_Channel_SetBlockWaitTimeExt);
	BDBG_ASSERT( in_channelHandle );

	in_channelHandle->currentChannelSettings.blockWaitTimeExt.ulValue = in_ulBlockWaitTimeExtInETU;

	BDBG_LEAVE(BSCD_Channel_SetBlockWaitTimeExt);
	return( errCode );
}

void BSCD_Channel_DumpRegisters(
		BSCD_ChannelHandle       in_channelHandle
)
{
	uint32_t ulVal;

	ulVal = BREG_Read32(
			in_channelHandle->moduleHandle->regHandle,
			(in_channelHandle->ulRegStartAddr + BSCD_P_UART_CMD_1));
	BDBG_MSG(("UART_CMD_1 = %02x \n",ulVal));

	ulVal = BREG_Read32(
			in_channelHandle->moduleHandle->regHandle,
			(in_channelHandle->ulRegStartAddr + BSCD_P_UART_CMD_2));
	BDBG_MSG(("UART_CMD_2 = %02x \n",ulVal));

	ulVal = BREG_Read32(
			in_channelHandle->moduleHandle->regHandle,
			(in_channelHandle->ulRegStartAddr + BSCD_P_PROTO_CMD));
	BDBG_MSG(("PROTO = %02x \n",ulVal));

	ulVal = BREG_Read32(
			in_channelHandle->moduleHandle->regHandle,
			(in_channelHandle->ulRegStartAddr + BSCD_P_FLOW_CMD));
	BDBG_MSG(("FLOW_CMD = %02x \n",ulVal));

	ulVal = BREG_Read32(
			in_channelHandle->moduleHandle->regHandle,
			(in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_1));
	BDBG_MSG(("IF_CMD_1 = %02x \n",ulVal));

	ulVal = BREG_Read32(
			in_channelHandle->moduleHandle->regHandle,
			(in_channelHandle->ulRegStartAddr + BSCD_P_IF_CMD_2));
	BDBG_MSG(("IF_CMD_2 = %02x \n",ulVal));

	ulVal = BREG_Read32(
			in_channelHandle->moduleHandle->regHandle,
			(in_channelHandle->ulRegStartAddr + BSCD_P_INTR_STAT_1));
	BDBG_MSG(("INTR_STAT_1 = %02x \n",ulVal));

	ulVal = BREG_Read32(
			in_channelHandle->moduleHandle->regHandle,
			(in_channelHandle->ulRegStartAddr + BSCD_P_INTR_STAT_2));
	BDBG_MSG(("INTR_STAT_2 = %02x \n",ulVal));

	ulVal = BREG_Read32(
			in_channelHandle->moduleHandle->regHandle,
			(in_channelHandle->ulRegStartAddr + BSCD_P_INTR_EN_1));
	BDBG_MSG(("INTR_EN_1 = %02x \n",ulVal));

	ulVal = BREG_Read32(
			in_channelHandle->moduleHandle->regHandle,
			(in_channelHandle->ulRegStartAddr + BSCD_P_INTR_EN_2));
	BDBG_MSG(("INTR_EN_2 = %02x \n",ulVal));

	ulVal = BREG_Read32(
			in_channelHandle->moduleHandle->regHandle,
			(in_channelHandle->ulRegStartAddr + BSCD_P_CLK_CMD));
	BDBG_MSG(("CLK_CMD = %02x \n",ulVal));

	ulVal = BREG_Read32(
			in_channelHandle->moduleHandle->regHandle,
			(in_channelHandle->ulRegStartAddr + BSCD_P_PRESCALE));
	BDBG_MSG(("PRESCALE = %02x \n",ulVal));

	ulVal = BREG_Read32(
			in_channelHandle->moduleHandle->regHandle,
			(in_channelHandle->ulRegStartAddr + BSCD_P_TIMER_CMD));
	BDBG_MSG(("TIMER_CMD = %02x \n",ulVal));

	ulVal = BREG_Read32(
			in_channelHandle->moduleHandle->regHandle,
			(in_channelHandle->ulRegStartAddr + BSCD_P_BGT));
	BDBG_MSG(("BGT = %02x \n",ulVal));

	ulVal = BREG_Read32(
			in_channelHandle->moduleHandle->regHandle,
			(in_channelHandle->ulRegStartAddr + BSCD_P_TIMER_CNT_1));
	BDBG_MSG(("TIMER_CNT_1 = %02x \n",ulVal));

	ulVal = BREG_Read32(
			in_channelHandle->moduleHandle->regHandle,
			(in_channelHandle->ulRegStartAddr + BSCD_P_TIMER_CNT_2));
	BDBG_MSG(("TIMER_CNT_2 = %02x \n",ulVal));

	ulVal = BREG_Read32(
			in_channelHandle->moduleHandle->regHandle,
			(in_channelHandle->ulRegStartAddr + BSCD_P_TIMER_CMP_1));
	BDBG_MSG(("TIMER_CMP_1 = %02x \n",ulVal));

	ulVal = BREG_Read32(
			in_channelHandle->moduleHandle->regHandle,
			(in_channelHandle->ulRegStartAddr + BSCD_P_TIMER_CMP_2));
	BDBG_MSG(("TIMER_CMP_2 = %02x \n",ulVal));

	ulVal = BREG_Read32(
			in_channelHandle->moduleHandle->regHandle,
			(in_channelHandle->ulRegStartAddr + BSCD_P_WAIT_1));
	BDBG_MSG(("SC_WAIT_1 = %02x \n",ulVal));

	ulVal = BREG_Read32(
			in_channelHandle->moduleHandle->regHandle,
			(in_channelHandle->ulRegStartAddr + BSCD_P_WAIT_2));
	BDBG_MSG(("SC_WAIT_2 = %02x \n",ulVal));

	ulVal = BREG_Read32(
			in_channelHandle->moduleHandle->regHandle,
			(in_channelHandle->ulRegStartAddr + BSCD_P_WAIT_3));
	BDBG_MSG(("SC_WAIT_3 = %02x \n",ulVal));

	ulVal = BREG_Read32(
			in_channelHandle->moduleHandle->regHandle,
			(in_channelHandle->ulRegStartAddr + BSCD_P_TGUARD));
	BDBG_MSG(("TGUARD = %02x \n",ulVal));

#ifdef BSCD_EMV2000_CWT_PLUS_4_EVENT_INTR
	ulVal = BREG_Read32(
			in_channelHandle->moduleHandle->regHandle,
			(in_channelHandle->ulRegStartAddr + BSCD_P_EVENT1_CMD_1));
	BDBG_MSG(("BSCD_P_EVENT1_CMD_1 = %02x \n",ulVal));

	ulVal = BREG_Read32(
			in_channelHandle->moduleHandle->regHandle,
			(in_channelHandle->ulRegStartAddr + BSCD_P_EVENT1_CMD_2));
	BDBG_MSG(("BSCD_P_EVENT1_CMD_2 = %02x \n",ulVal));

	ulVal = BREG_Read32(
			in_channelHandle->moduleHandle->regHandle,
			(in_channelHandle->ulRegStartAddr + BSCD_P_EVENT1_CMD_3));
	BDBG_MSG(("BSCD_P_EVENT1_CMD_3 = %02x \n",ulVal));

	ulVal = BREG_Read32(
			in_channelHandle->moduleHandle->regHandle,
			(in_channelHandle->ulRegStartAddr + BSCD_P_EVENT1_CMD_4));
	BDBG_MSG(("BSCD_P_EVENT1_CMD_4 = %02x \n",ulVal));

	ulVal = BREG_Read32(
			in_channelHandle->moduleHandle->regHandle,
			(in_channelHandle->ulRegStartAddr + BSCD_P_EVENT1_CMP));
	BDBG_MSG(("BSCD_P_EVENT1_CMP = %02x \n",ulVal));

	ulVal = BREG_Read32(
			in_channelHandle->moduleHandle->regHandle,
			(in_channelHandle->ulRegStartAddr + BSCD_P_EVENT2_CMD_1));
	BDBG_MSG(("BSCD_P_EVENT2_CMD_1 = %02x \n",ulVal));

	ulVal = BREG_Read32(
			in_channelHandle->moduleHandle->regHandle,
			(in_channelHandle->ulRegStartAddr + BSCD_P_EVENT2_CMD_2));
	BDBG_MSG(("BSCD_P_EVENT2_CMD_2 = %02x \n",ulVal));

	ulVal = BREG_Read32(
			in_channelHandle->moduleHandle->regHandle,
			(in_channelHandle->ulRegStartAddr + BSCD_P_EVENT2_CMD_3));
	BDBG_MSG(("BSCD_P_EVENT2_CMD_3 = %02x \n",ulVal));

	ulVal = BREG_Read32(
			in_channelHandle->moduleHandle->regHandle,
			(in_channelHandle->ulRegStartAddr + BSCD_P_EVENT2_CMD_4));
	BDBG_MSG(("BSCD_P_EVENT2_CMD_4 = %02x \n",ulVal));

	ulVal = BREG_Read32(
			in_channelHandle->moduleHandle->regHandle,
			(in_channelHandle->ulRegStartAddr + BSCD_P_EVENT2_CMP));
	BDBG_MSG(("BSCD_P_EVENT2_CMP = %02x \n",ulVal));

#endif
	/*
	bcmDeviceRead(inp_device, eSize8, BCM_SC_TRANSMIT, (void *)&ulVal);
	BDBG_MSG(("TRANSMIT = %02x \n",ulVal));
	bcmDeviceRead(inp_device, eSize8, BCM_SC_RECEIVE, (void *)&ulVal);
	BDBG_MSG(("RECEIVE = %02x \n",ulVal));


	bcmDeviceRead(inp_device, eSize8, BCM_SC_TLEN_1, (void *)&ulVal);
	BDBG_MSG(("TLEN_1 = %02x \n",ulVal));
	bcmDeviceRead(inp_device, eSize8, BCM_SC_TLEN_2, (void *)&ulVal);
	BDBG_MSG(("TLEN_2 = %02x \n",ulVal));
	bcmDeviceRead(inp_device, eSize8, BCM_SC_RLEN_1, (void *)&ulVal);
	BDBG_MSG(("RLEN_1 = %02x \n",ulVal));
	bcmDeviceRead(inp_device, eSize8, BCM_SC_RLEN_2, (void *)&ulVal);
	BDBG_MSG(("RLEN_2 = %02x \n",ulVal));

	*/
}


