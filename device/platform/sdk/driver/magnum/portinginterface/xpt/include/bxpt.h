/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxpt.h $
 * $brcm_Revision: Hydra_Software_Devel/64 $
 * $brcm_Date: 10/2/12 9:48a $
 *
 * Porting interface code for the data transport core. 
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xpt/include/bxpt.h $
 * 
 * Hydra_Software_Devel/64   10/2/12 9:48a gmullen
 * SW7445-17: Removed unmatched elif
 * 
 * Hydra_Software_Devel/63   10/1/12 1:57p gmullen
 * SW7445-17: Added 7445 support
 * 
 * Hydra_Software_Devel/62   6/18/12 10:45a gmullen
 * SW7425-2914: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7425-2914/3   6/11/12 1:23p gmullen
 * SW7425-2914: Fixed more 7405 compilation errors
 * 
 * Hydra_Software_Devel/SW7425-2914/1   6/11/12 10:59a gmullen
 * SW7425-2914: Moved and updated files to new home
 * 
 * Hydra_Software_Devel/61   5/8/12 10:15a gmullen
 * SW7429-115: Merged to Hydra
 * 
 * Hydra_Software_Devel/60   5/7/12 4:07p gmullen
 * SW7425-2906: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7425-2906/1   4/26/12 5:38p gmullen
 * SW7425-2906: Support adjusting record timestamps with PCR information
 * 
 * Hydra_Software_Devel/59   5/2/12 10:23a gmullen
 * SW7584-19: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7584-19/1   5/2/12 9:01a gmullen
 * SW7584-19: Added support for 7584
 * 
 * Hydra_Software_Devel/SW7429-115/2   5/2/12 4:00p gmullen
 * SW7429-115: Updated list of chips that have the bug
 * 
 * Hydra_Software_Devel/SW7429-115/1   4/6/12 2:04p gmullen
 * SW7429-115: Implemented workaround
 * 
 * Hydra_Software_Devel/58   4/2/12 2:17p gmullen
 * SW7435-81: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7435-81/1   4/2/12 1:38p gmullen
 * SW7435-81: Shared DRAM for unused XC and RS buffers
 * 
 * Hydra_Software_Devel/57   3/30/12 11:06a gmullen
 * SW7435-81: Bug in TRIG_MODE selection
 * 
 * Hydra_Software_Devel/56   3/8/12 3:41p gmullen
 * SW7425-2497: Merged to Hydra
 * 
 * Hydra_Software_Devel/55   3/8/12 1:42p gmullen
 * SW7435-53: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7435-53/1   3/8/12 11:17a gmullen
 * SW7435-53: Fixed hole in timebase mapping
 * 
 * Hydra_Software_Devel/54   3/7/12 2:41p gmullen
 * SW7325-879: Added BXPT_HAS_PACKETSUB_FORCED_INSERTION around code
 * 
 * Hydra_Software_Devel/SW7425-2497/2   3/8/12 3:19p gmullen
 * SW7425-2497: Added BXPT_HAS_PIPELINE_ERROR_REPORTING around code
 * 
 * Hydra_Software_Devel/SW7425-2497/1   3/5/12 4:30p gmullen
 * SW7425-2497: Checks for errors in data pipeline
 * 
 * Hydra_Software_Devel/53   2/29/12 10:22a gmullen
 * SW7346-718: Increased BXPT_NUM_PCRS
 * 
 * Hydra_Software_Devel/52   2/3/12 12:02p jtna
 * SW7425-2248: add BXPT_P_ParserClients.ToMpodRs
 * 
 * Hydra_Software_Devel/51   1/12/12 2:40p gmullen
 * SW7425-2138: Fixed incorrect parser band index
 * 
 * Hydra_Software_Devel/50   1/11/12 10:32a gmullen
 * SW7425-2138: Added BXPT_HAS_PB_PACKETSUB
 * 
 * Hydra_Software_Devel/49   1/2/12 3:12p gmullen
 * SW7360-1: Added support
 * 
 * Hydra_Software_Devel/48   1/2/12 1:47p gmullen
 * SW7435-10: Merge to Hydra
 * 
 * Hydra_Software_Devel/SW7435-10/3   12/20/11 5:04p mward
 * SW7435-10:  Merge from main.
 * 
 * Hydra_Software_Devel/47   12/20/11 4:27p gmullen
 * SW7425-1868: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7425-1868/1   12/20/11 4:24p gmullen
 * SW7425-1868: Added filter offset support
 * 
 * Hydra_Software_Devel/46   12/20/11 1:29p gmullen
 * SW7358-176: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7358-176/1   12/20/11 1:05p gmullen
 * SW7358-175: Added per-client control of XC and RS buffer alloc
 * 
 * Hydra_Software_Devel/SW7435-10/2   12/2/11 3:37p mward
 * SW7435-10:  Merge from main.
 * 
 * Hydra_Software_Devel/45   12/2/11 12:13p jtna
 * SW7425-1863: refactor xpt standby power management
 * 
 * Hydra_Software_Devel/44   11/30/11 2:59p gmullen
 * SW7425-1871: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7425-1871/1   11/30/11 2:52p gmullen
 * SW7425-1871: Added BXPT_NUM_MESSAGE_CAPABLE_PID_CHANNELS
 * 
 * Hydra_Software_Devel/43   11/29/11 4:47p gmullen
 * SW7429-3: Fixed incorrect number of TPIT blocks
 * 
 * Hydra_Software_Devel/SW7435-10/1   10/31/11 4:01p mward
 * SW7435-10:  Initial 7435 support.  Adapted from 7425 to allow
 * appframework to compile.
 * 
 * Hydra_Software_Devel/42   10/28/11 4:15p jtna
 * SW7425-1486: remove BXPT_GetMesgBufferErrorStatus() in favor of
 * BXPT_Mesg_GetErrorStatus()
 * 
 * Hydra_Software_Devel/41   10/28/11 4:03p jtna
 * SW7425-1486: added BXPT_Mesg_GetErrorStatus()
 * 
 * Hydra_Software_Devel/40   10/19/11 8:06p gmullen
 * SW7425-1491: Removed API.
 * 
 * Hydra_Software_Devel/39   10/19/11 5:27p gmullen
 * SW7425-1491: Added BXPT_P_RxMode()
 * 
 * Hydra_Software_Devel/38   10/12/11 5:26p gmullen
 * SW7425-1487: Inverted RxClockPol for certain parts
 * 
 * Hydra_Software_Devel/37   10/11/11 3:41p gmullen
 * SW7425-1383: Changed allPass macros
 * 
 * Hydra_Software_Devel/36   10/10/11 10:49a gmullen
 * SW7429-3: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7429-3/1   10/10/11 10:43a gmullen
 * SW7429-3: Added support
 * 
 * Hydra_Software_Devel/35   10/6/11 4:47p gmullen
 * SW7425-1384: Merged change to Hydra
 * 
 * Hydra_Software_Devel/SW7425-1384/1   10/6/11 2:19p gmullen
 * SW7425-1384: Added define for playback all-pass starting PID channel.
 * 
 * Hydra_Software_Devel/34   10/5/11 5:43p gmullen
 * SW7425-1183: Added BXPT_HAS_WAKEUP_PKT_SUPPORT to flag parts with this
 * feature
 * 
 * Hydra_Software_Devel/33   10/5/11 4:25p gmullen
 * SW7346-502: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7346-502/1   10/5/11 4:03p gmullen
 * SW7346-502: Added AcceptAdapt00 support
 * 
 * Hydra_Software_Devel/32   9/30/11 3:10p gmullen
 * SW7425-1183: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7425-1183/1   8/30/11 9:05a gmullen
 * SW7425-1183: Acquire the CLK216 resource when entering standby
 * 
 * Hydra_Software_Devel/31   9/28/11 11:47a gmullen
 * SW7425-1323: Cached RAVE settings in DRAM to reduce overhead
 * 
 * Hydra_Software_Devel/30   9/21/11 10:17a gmullen
 * SW7425-1323: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7425-1323/1   9/21/11 10:02a gmullen
 * SW7425-1323: Potential workaround.
 * 
 * Hydra_Software_Devel/29   9/8/11 1:18p gmullen
 * SW7552-99: Merged support to Hydra
 * 
 * Hydra_Software_Devel/SW7552-99/1   9/8/11 12:51p gmullen
 * SW7552-99: Added B0 support
 * 
 * Hydra_Software_Devel/28   9/2/11 1:41p gmullen
 * SW7425-690: Merged support to Hydra
 * 
 * Hydra_Software_Devel/SW7425-690/1   9/2/11 1:38p gmullen
 * SW7425-690: Added B0 defines
 * 
 * Hydra_Software_Devel/27   8/29/11 5:09p gmullen
 * SW7344-147: Merged support to Hydra
 * 
 * Hydra_Software_Devel/SW7344-147/1   8/29/11 5:08p gmullen
 * SW7344-147: Added B0 support
 * 
 * Hydra_Software_Devel/26   8/29/11 3:34p gmullen
 * SW7346-419: Merged changes to Hydra
 * 
 * Hydra_Software_Devel/SW7346-419/1   8/29/11 3:20p gmullen
 * SW7346-419: Added B0 support
 * 
 * Hydra_Software_Devel/25   8/22/11 5:45p gmullen
 * SW7231-319: Merged to Hydra branch
 * 
 * Hydra_Software_Devel/SW7231-319/1   8/16/11 2:15p gmullen
 * SW7231-319: Initial support for B0
 * 
 * Hydra_Software_Devel/24   8/10/11 4:32p gmullen
 * SW7346-414: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7346-414/1   8/9/11 11:00a gmullen
 * SW7346-414: Mark all 40nm chips as having AVS.
 * 
 * Hydra_Software_Devel/23   7/15/11 9:05a gmullen
 * SW7346-308: Merged fix to Hydra
 * 
 * Hydra_Software_Devel/SW7346-308/1   7/14/11 4:58p gmullen
 * SW7346-308: Added BXPT_RemoveFilterFromGroupAndBuffer() and
 * BXPT_Mesg_GetPidChannelFromBufferNum(), editted comments in bxpt.h
 * 
 * Hydra_Software_Devel/22   5/12/11 4:59p gmullen
 * SW7231-128: Merged to mainline
 * 
 * Hydra_Software_Devel/SW7231-128/1   5/10/11 1:40p gmohile
 * SW7231-128 : Add power management support
 * Hydra_Software_Devel/21   5/3/11 3:38p gmullen
 * SW7425-494: Default MTSIF RX clock polarity to negative edge
 * 
 * Hydra_Software_Devel/20   4/18/11 1:31p gmullen
 * SW7425-313: Added PID parser mapping to other 40nm chips
 * 
 * Hydra_Software_Devel/19   4/15/11 4:49p gmullen
 * SW7425-313: Merged to mainline
 * 
 * Hydra_Software_Devel/SW7425-313/1   4/13/11 2:05p gmullen
 * SW7425-313: Added parser remapping support
 * 
 * Hydra_Software_Devel/18   3/30/11 11:14a gmullen
 * SWDTV-6265: Free-running timebase select was incorrect
 * 
 * Hydra_Software_Devel/17   3/23/11 5:12p gmullen
 * SW7346-124: Do not allocate XC buffers for unused remux blocks
 * 
 * Hydra_Software_Devel/16   3/22/11 4:03p gmullen
 * SW7425-223: BXPT_NUM_STCS incorrect for 7422
 * 
 * Hydra_Software_Devel/15   3/8/11 9:58a gmullen
 * SW7425-156: Fixed incorrect blockout calculations
 * 
 * Hydra_Software_Devel/14   2/14/11 4:44p gmullen
 * SW7420-750: Changed BXPT_P_IS_PB macro to avoid issue with CodeWarrior
 * 
 * Hydra_Software_Devel/13   1/7/11 3:19p gmullen
 * SWBLURAY-23703: Added PI support
 * 
 * Hydra_Software_Devel/12   12/3/10 5:22p gmullen
 * SW7422-87: Added per-PID channel R-pipe support for mesg filters
 * 
 * Hydra_Software_Devel/11   12/3/10 3:20p gmullen
 * SW7425-15: Allow setting the initial CC for generation
 * 
 * Hydra_Software_Devel/10   11/30/10 6:09p gmullen
 * SW7425-15: Added TS muxing support
 * 
 * Hydra_Software_Devel/9   11/18/10 5:21p gmullen
 * SW7344-2: Initial checkin
 * 
 * Hydra_Software_Devel/8   11/18/10 5:13p gmullen
 * SW7346-3: Initial checkin
 * 
 * Hydra_Software_Devel/7   11/18/10 4:58p gmullen
 * SW7231-6: Initial checkin
 * 
 * Hydra_Software_Devel/6   11/1/10 10:47a gmullen
 * SW7552-3: Added support for the 7552
 * 
 * Hydra_Software_Devel/5   10/28/10 3:30p gmullen
 * SW7358-12: Updated PI for the 7358
 * 
 * Hydra_Software_Devel/4   10/28/10 2:08p gmullen
 * SW7422-20: Checkin ported files
 * 
 * Hydra_Software_Devel/3   10/27/10 5:24p gmullen
 * SW7422-20: Added macros to generate Int IDs for CC, TEI, and Parser
 * Length errors
 * 
 * Hydra_Software_Devel/2   10/25/10 5:32p gmullen
 * SW7422-20: Changed _P_MAX defines to _NUM
 * 
 * Hydra_Software_Devel/1   10/25/10 2:09p gmullen
 * SW7425-15: Moved srcs to base2 folder
 * 
 * Hydra_Software_Devel/2   10/25/10 9:11a gmullen
 * SW7425-15: Updated headers and added stub versions of C files
 * 
 * Hydra_Software_Devel/1   10/8/10 2:57p gmullen
 * SW7425-15: Added header files
 * 
 * Hydra_Software_Devel/95   9/13/10 3:05p gmullen
 * SW7125-632: Added BXPT_HAS_AVS to identify which chips where RAVE does
 * not automatically index startcode 0x00
 * 
 * Hydra_Software_Devel/94   8/27/10 3:46p gmullen
 * SW35230-1172: Merged to mainline
 * 
 * Hydra_Software_Devel/SW35230-1172/1   8/27/10 2:16p gmullen
 * SW35230-1172: Added BXPT_CheckBufferWithWrap()
 * 
 * Hydra_Software_Devel/93   7/29/10 1:59p gmullen
 * SW7420-904: Added RMX_NULL_PKT_THRESHOLD
 * 
 * Hydra_Software_Devel/92   7/28/10 6:21p gmullen
 * SWBLURAY-21763: Fixed compilation bug for 7630B0
 * 
 * Hydra_Software_Devel/91   7/19/10 4:21p piyushg
 * SW7400-2859: Change the MAX TPIT to 6 for 7400B0 and later.
 * 
 * Hydra_Software_Devel/90   7/6/10 11:40a gmullen
 * SW7630-81: Fixed compilation bug for 7125
 * 
 * Hydra_Software_Devel/89   7/4/10 5:33p gmullen
 * SW7630-81: Added support for QUICK
 * 
 * Hydra_Software_Devel/88   4/30/10 5:23p gmullen
 * SW7630-81: Created branch
 * 
 * Hydra_Software_Devel/87   4/2/10 5:11p gmullen
 * SW7405-4165: Fixed inconsistent API descriptions
 * 
 * Hydra_Software_Devel/86   3/25/10 3:53p gmullen
 * SW35230-127: Updated code for requirements in this JIRA
 * 
 * Hydra_Software_Devel/85   3/1/10 3:05p gmullen
 * SW7550-16: Fixed incorrect defines for number of PCR offsets and STCs
 * 
 * Hydra_Software_Devel/84   1/25/10 11:54a gmullen
 * SW7408-64: Added DirecTV support
 * 
 * Hydra_Software_Devel/83   1/8/10 5:45p gmullen
 * SW7550-179: Define for number of TPIT indexers was incorrect
 * 
 * Hydra_Software_Devel/82   12/17/09 9:34a gmullen
 * SW7630-62: Merged fix to mainline
 * 
 * Hydra_Software_Devel/SW7630-62/1   12/17/09 9:13a gmullen
 * SW7630-62: Buffer enums were incorrect
 * 
 * Hydra_Software_Devel/81   12/16/09 3:26p gmullen
 * SW7325-665: Added support for 32-bit timestamp mode
 * 
 * Hydra_Software_Devel/80   12/9/09 2:46p gmullen
 * SW7405-3331: Merged to mainline
 * 
 * Hydra_Software_Devel/79   11/23/09 5:14p gmullen
 * SW7468-13: Updated PI and unit test
 * 
 * Hydra_Software_Devel/78   11/17/09 3:07p gmullen
 * SW7408-9: Finished adding 7408 support to XPT
 * 
 * Hydra_Software_Devel/77   10/26/09 3:09p gmullen
 * SW7468-13: Added XPT support
 * 
 * Hydra_Software_Devel/76   10/22/09 9:30a gmullen
 * SW7630-46: Added DVD change for BXPT_Playback_CheckHeadDescriptor
 * 
 * Hydra_Software_Devel/75   9/24/09 5:27p gmullen
 * SWDEPRECATED-3717: Added BXPT_GetDefaultPsiSettings() and
 * BXPT_GetDefaultPidChannelRecordSettings()
 * 
 * Hydra_Software_Devel/74   9/16/09 2:57p piyushg
 * SW3548-2479: Added new API BXPT_GetBuffer_isr which directly calls
 * BXPT_UpdateReadOffset_isr() to avoid entering into critical section.
 * 
 * Hydra_Software_Devel/73   9/14/09 10:15a gmullen
 * SW35230-2: Added case for 35230
 * 
 * Hydra_Software_Devel/72   9/9/09 8:05a piyushg
 * SW7630-30: Add 7630 XPT PI support.
 * Added directory element "7630".
 * 
 * Hydra_Software_Devel/71   9/2/09 2:06p piyushg
 * SW7325-582: Added API in bxpt_mesg.c to disable the pid channel
 * to message buffer association that is created after calling the
 * ConfigPid2BufferMap.
 * 
 * Hydra_Software_Devel/70   8/31/09 3:23p gmullen
 * SW7342-32:Added BXPT_SetPidChannelSwitchedISR()
 * 
 * Hydra_Software_Devel/69   8/31/09 2:39p gmullen
 * SW7342-34: Reduced number of playbacks to 2 for A0.
 * 
 * Hydra_Software_Devel/68   8/19/09 12:14p piyushg
 * PR56771: Add support for 7342. Hence the previous usage
 * of constant PARSER_REG_STEPSIZE does not work for this chip.
 * Added new function to calculate offset.
 * 
 * Hydra_Software_Devel/67   8/11/09 11:13a piyushg
 * PR55216: Fix compilation error.
 * 
 * Hydra_Software_Devel/66   8/11/09 10:38a piyushg
 * PR55216: Added initial 7340 XPT support.
 * 
 * Hydra_Software_Devel/65   8/7/09 2:03p gmullen
 * PR57524: Enabled scrambling control support in RAVE, to match new RDB
 * header.
 * 
 * Hydra_Software_Devel/64   8/5/09 4:51p piyushg
 * PR55545: Add 7125 XPT PI support
 * Added file element "bxpt_rave_ihex.c".
 * Added file element "bxpt_rave_ihex.h".
 * 
 * Hydra_Software_Devel/63   7/31/09 5:39p gmullen
 * PR54331: Fixed compilation errors in 7118.
 * 
 * Hydra_Software_Devel/62   7/31/09 3:20p piyushg
 * PR56771: Add support for 7342.
 * 
 * Hydra_Software_Devel/61   7/31/09 2:44p gmullen
 * PR54331: Added 35130 to XPT support.
 * 
 * Hydra_Software_Devel/60   7/29/09 11:38a gmullen
 * PR57174: Added 7420B0 support.
 * 
 * Hydra_Software_Devel/59   7/17/09 1:49p gmullen
 * PR52986: Message buffer support wasn't enabled.
 * 
 * Hydra_Software_Devel/58   7/14/09 10:20a piyushg
 * PR56771: Add XPT PI code for 7342.
 * 
 * Hydra_Software_Devel/57   6/18/09 8:51a gmullen
 * PR56110: Removed PID remapping feature from remux code. Not supported
 * on this part.
 * 
 * Hydra_Software_Devel/56   6/17/09 6:46p gmullen
 * PR56110: Added support.xpt/7550/uif_image/a0
 * 
 * Hydra_Software_Devel/55   6/8/09 6:04p gmullen
 * PR55700: Added BXPT_IsDataPresent().
 * 
 * Hydra_Software_Devel/54   6/8/09 3:12p gmullen
 * PR55814: Updated comments and removed redundant definitions in
 * bxpt_mesg.h .
 * 
 * Hydra_Software_Devel/53   5/28/09 2:11p gmullen
 * PR55549: Added support for PCR-based pacing.
 * 
 * Hydra_Software_Devel/52   4/29/09 1:40p gmullen
 * PR54619: Fixed compilation bug for 7335.
 * 
 * Hydra_Software_Devel/51   4/28/09 6:41p gmullen
 * PR54619: Changed conditional compile checks for GetScramblingCtrl API.
 * 
 * Hydra_Software_Devel/50   4/19/09 9:35a gmullen
 * PR53662: Added hook to determine if XPT can be powered down.
 * 
 * Hydra_Software_Devel/49   4/7/09 5:25p piyushg
 * PR52986: Add support for 7635
 * Added directory element "7635".
 * 
 * Hydra_Software_Devel/48   3/26/09 7:22p piyushg
 * PR53632: Changed the MAX_PID_PARSER for 7420 to 6 from 7.
 * This was causing GISB timout error on initialization.
 * 
 * Hydra_Software_Devel/47   1/27/09 1:09p gmullen
 * PR51625: Added 7336 support
 * 
 * Hydra_Software_Devel/46   1/5/09 10:03a gmullen
 * PR50811: Added _isr version of BXPT_UpdateReadOffset().
 * 
 * Hydra_Software_Devel/45   12/15/08 2:36p gmullen
 * PR48908: Removed power management code from XPT PI.
 * 
 * Hydra_Software_Devel/44   11/26/08 4:14p gmullen
 * PR47755: Added support for 7420.
 * 
 * Hydra_Software_Devel/43   11/6/08 11:24a gmullen
 * PR46544: Added public PI call to incr/decr power reference count.
 * 
 * Hydra_Software_Devel/42   10/30/08 1:14p gmullen
 * PR46544: Fixed spelling error in define.
 * 
 * Hydra_Software_Devel/41   10/29/08 9:20a gmullen
 * PR46544: Fixed compile error: code was built for unsupported chips.
 * 
 * Hydra_Software_Devel/40   10/28/08 6:18p gmullen
 * PR46544: Default power management to ON. Added compile-time support to
 * disable power management.
 * 
 * Hydra_Software_Devel/39   10/28/08 4:58p gmullen
 * PR46544: Added power management support, default to disabled for now.
 * 
 * Hydra_Software_Devel/38   8/18/08 3:58p gmullen
 * PR45362: Number of playback channels that support muxing should be
 * programmable.
 * 
 * Hydra_Software_Devel/37   8/8/08 5:47p gmullen
 * PR45362: Merged SanJose_CDI_Devel changes to Hydra_Software_Devel
 * 
 * Hydra_Software_Devel/SanJose_CDI_Devel/4   8/4/08 11:10a bhsu
 * PR45362 : Fixed Kernel oops for AM21.
 * 
 * Hydra_Software_Devel/36   7/16/08 3:43p gmullen
 * PR37867: Merged playback mux code to Hydra_Software_Devel
 * 
 * Hydra_Software_Devel/35   6/26/08 6:30p gmullen
 * PR43907: Added comments to description of BXPT_ParserAllPassMode
 * 
 * Hydra_Software_Devel/34   6/18/08 4:52p piyushg
 * PR39234:Fix compilation error.
 * 
 * Hydra_Software_Devel/33   6/18/08 4:11p piyushg
 * PR39234: Change some comments and API names for setting up
 * multiple message buffers for same PID channel.
 * 
 * Hydra_Software_Devel/32   6/16/08 11:55a piyushg
 * PR39234: Change some comments and API names for setting up
 * multiple message buffers for same PID channel.
 * 
 * Hydra_Software_Devel/31   4/10/08 3:36p gmullen
 * PR38954: Updated number of playbacks, added HDDVI support to PCR API,
 * and added packetizer support.
 * 
 * Hydra_Software_Devel/30   3/26/08 11:23a gmullen
 * PR38954: Added 3548 support to XPT PI.
 * 
 * Hydra_Software_Devel/29   3/17/08 1:03p gmullen
 * PR40675: Added support to route all mesg filter data on R-pipe.
 * 
 * Hydra_Software_Devel/28   2/28/08 3:56p piyushg
 * PR38891: Change the BXPT_P_MAX_PID_CHANNELS from 255 to 256.
 * 
 * Hydra_Software_Devel/27   11/28/07 10:42a gmullen
 * PR36900: Added 7335 support
 * 
 * Hydra_Software_Devel/26   10/19/07 4:52p gmullen
 * PR35018: Updated for 7325/
 * 
 * Hydra_Software_Devel/25   9/28/07 11:16a gmullen
 * PR35427: Added BXPT_GetDefaultParserConfig() and
 * BXPT_GetDefaultInputBandConfig()
 * 
 * Hydra_Software_Devel/24   9/13/07 1:36p gmullen
 * PR32868: Changes to support bmsglib
 * 
 * Hydra_Software_Devel/23   7/18/07 3:35p gmullen
 * PR29691: Incorporated workaround. See PR for details.
 * 
 * Hydra_Software_Devel/22   5/17/07 2:48p katrep
 * PR29057: Added BXPT_GetMesgBufferErrorStatus api
 * 
 * Hydra_Software_Devel/21   5/17/07 2:17p gmullen
 * PR30877: Added support for dedicated playback heap handle.
 * 
 * Hydra_Software_Devel/20   2/14/07 1:09p gmullen
 * PR27642: Added support for 7405.
 * 
 * Hydra_Software_Devel/19   2/6/07 7:05p gmullen
 * PR26217: Added B0 support.
 * 
 * Hydra_Software_Devel/18   12/5/06 3:00p gmullen
 * PR26325: Added support for CAP filtering.
 * 
 * Hydra_Software_Devel/17   12/5/06 2:04p gmullen
 * PR26278: Allow user to configure byte alignment of data in message
 * buffer.
 * 
 * Hydra_Software_Devel/16   11/17/06 5:34p gmullen
 * PR26034: Fixed BXPT_Remux_AllocPidMap().
 * 
 * Hydra_Software_Devel/15   11/15/06 3:22p gmullen
 * PR15309: Added BXPT_P_EnablePidChannel.
 * 
 * Hydra_Software_Devel/14   11/2/06 3:05p katrep
 * PR25278: Added PidChannelNum argument to BXPT_AddFilterToGroup API.
 * 
 * Hydra_Software_Devel/13   11/2/06 2:42p gmullen
 * PR25402: Added handle for secure heap
 * 
 * Hydra_Software_Devel/12   8/17/06 6:01p katrep
 * PR23114: Added Support for 7440 chip
 * 
 * Hydra_Software_Devel/11   8/10/06 2:51p gmullen
 * PR23189: Added defines for 3563.
 * 
 * Hydra_Software_Devel/10   8/4/06 5:16p katrep
 * PR23114: Added support for 7440
 * 
 * Hydra_Software_Devel/9   8/2/06 3:56p gmullen
 * PR23137: Added support for playback data to PCR block.
 * 
 * Hydra_Software_Devel/8   7/21/06 12:17p katrep
 * PR21525: Added support for 128,16 byte filters across 4 banks or
 * 64,32byte filters across 2 banks.
 * 
 * Hydra_Software_Devel/7   7/17/06 5:19p gmullen
 * PR20624: Added code for 7118.
 * 
 * Hydra_Software_Devel/6   7/13/06 11:37a katrep
 * PR20316: Added support to set/unset PSI setting per pid channel basis
 * as it's supported by new HW arch.PR20315:Also added support for 32
 * byte filters.Default filter size is 16 bytes.32 Bytes filters can be
 * enabled at compile time by -DBXPT_FILTER_32
 * 
 * Hydra_Software_Devel/5   7/12/06 5:54p gmullen
 * PR21042: XC and RS buffers can be customized to reduce total memory
 * needs.
 * 
 * Hydra_Software_Devel/4   6/5/06 5:54p gmullen
 * PR18998: Added support for control of ACCEPT_ADAPT_00 in parser regs.
 * 
 * Hydra_Software_Devel/3   4/12/06 10:34a gmullen
 * PR20631: Added compilation checks for 7401 B0
 * 
 * Hydra_Software_Devel/2   2/1/06 10:19a gmullen
 * PR18998: Fixed overflow issue in RAVE ITB/CDB, added support for PB
 * channels.
 * 
 * Hydra_Software_Devel/1   1/16/06 2:08p gmullen
 * PR18998: Initial checkin for 7400
 * 
 *
 ***************************************************************************/

/*= Module Overview *********************************************************

Overview

***************************************************************************/

#ifndef BXPT_H__
#define BXPT_H__

#include "breg_mem.h"
#include "bint.h"
#include "bmem.h"
#include "bavc.h"
#include "bchp.h"
#include "berr_ids.h"
#include "bchp_xpt_fe.h"

#include "bxpt_capabilities.h"

#if BXPT_HAS_FULL_PID_PARSER
#include "bchp_xpt_full_pid_parser.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if BXPT_HAS_MULTICHANNEL_PLAYBACK
    /* MCPB has it's own mapping. */
    #define BXPT_PB_ALLPASS_PID_CHANNEL_START	128
    #define BXPT_PB_PARSER_BAND_BASE            128
#else
    /* SW7425-1384: The PID channels that playback uses for all-pass mode have changed. So has the
       playback's parser band number, as used in the PID table. */
    #if BCHP_XPT_FE_PID_TABLE_i_PLAYBACK_BAND_PARSER_PID_CHANNEL_INPUT_SELECT_MASK == 0x003f0000
    	#define BXPT_PB_ALLPASS_PID_CHANNEL_START	64
        #define BXPT_PB_PARSER_BAND_BASE            64
    #elif BCHP_XPT_FE_PID_TABLE_i_PLAYBACK_BAND_PARSER_PID_CHANNEL_INPUT_SELECT_MASK == 0x000f0000
    	#define BXPT_PB_ALLPASS_PID_CHANNEL_START	16
    	#define BXPT_PB_PARSER_BAND_BASE	        16
    #else
    	#error "INTERNAL ERROR"
    #endif
#endif

/* Use a separate define, in case this defacto mapping is changed on a future chip. */
#ifdef BXPT_NUM_MESG_BUFFERS
    #define BXPT_NUM_MESSAGE_CAPABLE_PID_CHANNELS BXPT_NUM_MESG_BUFFERS
#endif
#ifdef BXPT_P_MAX_MESG_BUFFERS
    #define BXPT_NUM_MESSAGE_CAPABLE_PID_CHANNELS BXPT_P_MAX_MESG_BUFFERS
#endif

#define BXPT_GET_PLAYBACK_ALLPASS_CHNL( PbNum ) (PbNum + BXPT_PB_ALLPASS_PID_CHANNEL_START)
#define BXPT_GET_IB_PARSER_ALLPASS_CHNL( ParserNum )    (ParserNum)

/***************************************************************************
Summary:
The handle for the transport module. Users should not directly access the 
contents.
****************************************************************************/
typedef struct BXPT_P_TransportData *BXPT_Handle;

/***************************************************************************
Summary:
Macros to generate BINT IDs for some of the errors reported by the parser 
bands. 
****************************************************************************/
#if BXPT_HAS_FULL_PID_PARSER
	#define BXPT_INT_ID_PARSER_LENGTH_ERROR(X)      BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS0_REG, (X))
	#define BXPT_INT_ID_PARSER_TRANSPORT_ERROR(X)   BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS0_REG, (X + 16))
	#define BXPT_INT_ID_PARSER_CONTINUITY_ERROR(X)  BCHP_INT_ID_CREATE(BCHP_XPT_FULL_PID_PARSER_IBP_PCC_INTR_STATUS_REG, (X))
#else
	#define BXPT_INT_ID_PARSER_LENGTH_ERROR(X)      BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS_REG, (X + BCHP_XPT_FE_INTR_STATUS_REG_PARSER0_LENGTH_ERROR_SHIFT))
	#define BXPT_INT_ID_PARSER_TRANSPORT_ERROR(X)   BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS_REG, (X + BCHP_XPT_FE_INTR_STATUS_REG_PARSER0_TRANSPORT_ERROR_SHIFT))
	#define BXPT_INT_ID_PARSER_CONTINUITY_ERROR(X)  BCHP_INT_ID_CREATE(BCHP_XPT_FE_INTR_STATUS_REG, (X + BCHP_XPT_FE_INTR_STATUS_REG_PARSER0_CONTINUITY_ERROR_SHIFT))
#endif

/***************************************************************************
Summary:
Transport specific error codes.
****************************************************************************/
#define BXPT_ERR_NO_AVAILABLE_RESOURCES     BERR_MAKE_CODE(BERR_XPT_ID, 0)
#define BXPT_ERR_CHANNEL_ALREADY_RUNNING    BERR_MAKE_CODE(BERR_XPT_ID, 1)
#define BXPT_ERR_CHANNEL_ALREADY_STOPPED    BERR_MAKE_CODE(BERR_XPT_ID, 2)
#define BXPT_ERR_DEVICE_BUSY                BERR_MAKE_CODE(BERR_XPT_ID, 3)
#define BXPT_ERR_MESG_BUFFER_NOT_CONFIGURED BERR_MAKE_CODE(BERR_XPT_ID, 4)
#define BXPT_ERR_DUPLICATED_PID             BERR_MAKE_CODE(BERR_XPT_ID, 5)
#define BXPT_ERR_PID_ALREADY_ALLOCATED      BERR_MAKE_CODE(BERR_XPT_ID, 6)
#define BXPT_ERR_QUEUE_FULL                 BERR_MAKE_CODE(BERR_XPT_ID, 7)
#define BXPT_ERR_RESOURCE_NOT_FOUND         BERR_MAKE_CODE(BERR_XPT_ID, 8)
#define BXPT_ERR_DATA_PIPELINE				BERR_MAKE_CODE(BERR_XPT_ID, 9)

/***************************************************************************
Summary:
Number of bytes in the default PSI filter.
****************************************************************************/
#ifdef  BXPT_FILTER_32
#define BXPT_FILTER_SIZE    32
#else
#define BXPT_FILTER_SIZE    16
#endif

/***************************************************************************
Summary:
Macro to specify that a PID channel should get data from a Playback parser,
rather than a Front End parser. For example, to select Playback parser 1 
as the source of data, use BXPT_PB_PARSER( 1 ) as the Band argument to 
BXPT_ConfigurePidChannel().
****************************************************************************/
#define BXPT_PB_PARSER( x )     ( ( x ) | 0x8000 )

/***************************************************************************
Summary:
Enumeration of all the data sources. Some functions that use this enum may
not support all members. 
****************************************************************************/
typedef enum BXPT_DataSource
{
    BXPT_DataSource_eInputBand,         /* External input source. */
    BXPT_DataSource_eRemuxFeedback,     /* Feedback path from remux block. */
    BXPT_DataSource_ePlayback           /* Data from playback channels */
}
BXPT_DataSource;


/***************************************************************************
Summary:
Enumeration of the types of PID parsers supported. Note that the types have
different features and are not interchangable in some cases.
****************************************************************************/
typedef enum BXPT_ParserType
{
    BXPT_ParserType_eIb,    /* Parsers fed from input bands. */
    BXPT_ParserType_ePb     /* Parsers fed from playback channels. */
}
BXPT_ParserType;

/***************************************************************************
Summary:
The PSI messages transfered into the DMA message buffers may be modified by 
the hardware to make further parsing by software easier. 3 modifications are 
supported. They are:

Replace CRC bytes with a sentinel - The CRC bytes are overwritten with a
fixed sentinel value of 0x98B93846. 

Replace CRC bytes with match vector - The CRC bytes are replaced with a 32-bit
bitmap that indicates which filter(s) associated with the PID channel the 
message matched. For instance, if message matched filter 1 and 3, then bits
1 and 3 of the vector are set, and all others are 0.

Append match vector to message - Append the filter match vector to the 
message, following the CRC bytes. The CRC isn't overwritten. This append is 
done before any word-align padding for that message

These settings affect all PID channels associated with the given PID parser.
****************************************************************************/
typedef enum BXPT_PsiMesgModModes
{
    BXPT_PsiMesgModModes_eNoMod = 0,                    /* Do not modify messages. */
    BXPT_PsiMesgModModes_eReplaceWithSentinel = 1,      /* Replace CRC bytes with Sentinel. */
    BXPT_PsiMesgModModes_eReplaceWithMatchVector = 2,   /* Replace CRC bytes with filter match vector */
    BXPT_PsiMesgModModes_eAppendMatchVector = 3     /* Replace CRC bytes with filter match vector */
}
BXPT_PsiMesgModModes;

/***************************************************************************
Summary:
Controls the type of timestamp generated as packets pass through this parser band.
****************************************************************************/
typedef enum BXPT_ParserTimestampMode
{
    BXPT_ParserTimestampMode_eAutoSelect = 0,   /* Use Mod-300 timestamp if the transport stream is MPEG; use binary timestamp otherwise. */
    BXPT_ParserTimestampMode_eMod300 = 2,   /* Use Mod-300 timestamp. */
    BXPT_ParserTimestampMode_eBinary = 3    /* Use binary timestamp. */
}
BXPT_ParserTimestampMode;

/***************************************************************************
Summary:
Defines the polarity values for input band related parameters. 
****************************************************************************/
typedef enum BXPT_Polarity
{
    BXPT_Polarity_eActiveHigh,
    BXPT_Polarity_eActiveLow
}
BXPT_Polarity;

/***************************************************************************
Summary: 
Config for the MTSIF interface.  
****************************************************************************/
typedef struct BXPT_MtsifConfig
{
    /* Width of the MTSIF bus. Allowable values are 1, 2, 4, and 8 bits */
    unsigned RxInterfaceWidth;   
     
    /* 1  = rising edge, 0 = falling edge. NOTE: On some parts, due to a
       hardware bug, these values might not match those given in the register
       docs. The portinginterface will handle that. */
    unsigned RxClockPolarity;   

    bool Enable;    /* Interface enabled or not. */
}
BXPT_MtsifConfig;

#if (BXPT_HAS_FIXED_RSBUF_CONFIG || BXPT_HAS_FIXED_XCBUF_CONFIG)
/***************************************************************************
Summary: 
Maximum bitrates for each input band parser and each playback parser. These are 
rates expected *after* PID parsing. Value is specified in Mbps.  

The valid range for PID parser data rates is from BXPT_MIN_PARSER_RATE up to
BXPT_MAX_PARSER_RATE, inclusive. A value of 0 has special meaning: this indicates 
that the PID parser is not going to be used. 
****************************************************************************/

/* In Mbps */
#define BXPT_MIN_PARSER_RATE	(10000)
#define BXPT_MAX_PARSER_RATE	(108000000)

typedef struct BXPT_P_ParserClients
{
       /* If true, send this parser's data to ... */
       bool ToRave;         /* ... the RAVE */
       bool ToMsg;                /* ... the message filters */
    
       #if BXPT_HAS_REMUX
       bool ToRmx[ BXPT_NUM_REMULTIPLEXORS ];   /* ... the remuxes. */
       #endif

       bool ToMpodRs;   /* ... the MPOD RS buffers, thence to the MPOD */
}
BXPT_P_ParserClients;

typedef struct BXPT_BandWidthConfig
{
    #if BXPT_HAS_IB_PID_PARSERS
    unsigned MaxInputRate[ BXPT_NUM_PID_PARSERS ];
    BXPT_P_ParserClients IbParserClients[ BXPT_NUM_PID_PARSERS ];
    #endif

    #if BXPT_HAS_PLAYBACK_PARSERS
    unsigned MaxPlaybackRate[ BXPT_NUM_PLAYBACKS ];
    BXPT_P_ParserClients PlaybackParserClients[ BXPT_NUM_PLAYBACKS ];
    #endif

    #if BXPT_HAS_REMUX
    /* Set to 'false' if the app does not expect to use the remux blocks. Default setting is 'true' */
    bool RemuxUsed[ BXPT_NUM_REMULTIPLEXORS ];
    #endif
}
BXPT_BandWidthConfig;

/***************************************************************************
Summary:
Default settings and/or actions to used during BXPT_Open(). 
****************************************************************************/
typedef struct BXPT_DefaultSettings
{
    #if BXPT_HAS_FIXED_RSBUF_CONFIG || BXPT_HAS_FIXED_XCBUF_CONFIG 
    BXPT_BandWidthConfig BandwidthConfig;
    #endif

    /* Handle for secure memory allocations. This is initialized to NULL */
    BMEM_Handle hRHeap;

    /* 
    ** Handle for playback memory mapping. In some cases, the playback 
    ** descriptors/buffers are taken from a heap other than the system
    ** heap that XPT was opened with. In that case, the handle for that
    ** heap should be set here. This is initialized to NULL. If it is not
    ** overridden, the heap used at BXPT_Open will be used.
    */
    BMEM_Handle hPbHeap;

	/* 
	** When true, data sent to the mesg filters will go via the R-pipe. When
	** false, data is sent on the G-pipe. Default is the G-pipe.
	*/		 
	bool MesgDataOnRPipe;

    #if BXPT_HAS_MTSIF
    /* Settings for the MTSIF RX block(s). The receivers default to disabled. */
    BXPT_MtsifConfig MtsifConfig[ BXPT_NUM_MTSIF ];
    #endif
}
BXPT_DefaultSettings;

#else
/***************************************************************************
Summary:
Sizes for the Rate Smoothing (Rs) and Transport Client (Xc)
DRAM buffers. Sizes are specified in kilobytes. Note that setting
a buffer size to 0 will disable usage of that particular data
path through the transport core. For example, setting 
IbParserRsSize[ 1 ] = 0 will effectively disable input parser
band 1. Likewise, setting RaveXcCfg.PbParserXcSize[ 0 ] = 0
will prohibit playback parser 0 data from reaching the RAVE. 
****************************************************************************/
typedef struct BXPT_DramBufferCfg
{
#if BXPT_HAS_IB_PID_PARSERS             
    /* 
    ** Size of the input parser's rate-smoothing buffer.
    ** InputParserRsSize[ 0 ] specifies the size for input parser 0's
    ** buffer.
    */
    unsigned IbParserRsSize[ BXPT_P_MAX_PID_PARSERS ];
#endif

    /* 
    ** Size of the playback parser's rate-smoothing buffer. 
    ** PlaybackParserRsSize[ 0 ] specifies the size for playback 
    ** parser 0's buffer. 
    */
    unsigned PbParserRsSize[ BXPT_P_MAX_PLAYBACKS ];

    /*
    ** Size of the transport client buffers (XC buf), for the RAVE
    ** and message filters. Each of these clients has one or more
    ** XC buffers, one for each source of data the client can use.
    */
    struct 
    {
#if BXPT_HAS_IB_PID_PARSERS             
        unsigned IbParserXcSize[ BXPT_P_MAX_PID_PARSERS ];
#endif
        unsigned PbParserXcSize[ BXPT_P_MAX_PLAYBACKS ];
    }
    RaveXcCfg, MesgBufXcCfg;

#if BXPT_HAS_REMUX
    /* 
    ** Size of the transport client buffers for the remux channels. 
    */
    struct 
    {
        unsigned BandAXcSize;   /* Remux band A buffer size */
        unsigned BandBXcSize;   /* Remux band B buffer size */
    }
    RemuxXcCfg[ BXPT_P_MAX_REMULTIPLEXORS ];

#endif
}
BXPT_DramBufferCfg;

/***************************************************************************
Summary:
Default settings and/or actions to used during BXPT_Open(). 
****************************************************************************/
typedef struct BXPT_DefaultSettings
{
    /* RS and XC buffer configuration. */
    BXPT_DramBufferCfg DramBuffers; 

#if ( BCHP_CHIP == 3563 )
    /* Enable MPOD output on the parallel output. Otherwise, Remux is output */
    bool EnableMpodOut;
#endif

    /* Handle for secure memory allocations. This is initialized to NULL */
    BMEM_Handle hRHeap;

    /* 
    ** Handle for playback memory mapping. In some cases, the playback 
    ** descriptors/buffers are taken from a heap other than the system
    ** heap that XPT was opened with. In that case, the handle for that
    ** heap should be set here. This is initialized to NULL. If it is not
    ** overridden, the heap used at BXPT_Open will be used.
    */
    BMEM_Handle hPbHeap;

	/* 
	** When true, data sent to the mesg filters will go via the R-pipe. When
	** false, data is sent on the G-pipe. Default is the G-pipe.
	*/		 
	bool MesgDataOnRPipe;
}
BXPT_DefaultSettings;
#endif

/***************************************************************************
Summary:
The Capability structure contains handy stuff describing what is supported
by this core.
****************************************************************************/
typedef struct BXPT_Capability
{
    unsigned int MaxPlaybacks;          /* Number of playback blocks we support. */
    unsigned int MaxPidChannels;        /* Number of PID channels we support. */
    unsigned int MaxPidParsers;         /* Number of PID parsers we support. */
    unsigned int MaxInputBands;         /* Max number of input bands we support. */
    unsigned int MaxTpitPids;           /* Max number of TPIT PIDs each record can support. */
    unsigned int MaxFilterBanks;        /* Max number of filter banks */
    unsigned int MaxFiltersPerBank;     /* Number of filters in each bank. */
    unsigned int MaxPcrs;               /* Max number of Pcr blocks */
    unsigned int MaxPacketSubs;         /* Max number of packet subsitution channels. */

    unsigned int MaxRaveContexts;       /* Max number of RAVE contexts (AV plus record). */
}
BXPT_Capability;

#if BXPT_HAS_IB_PID_PARSERS             

/***************************************************************************
Summary:
Settings for a parser band. 
****************************************************************************/
typedef struct BXPT_ParserConfig
{
#if BXPT_HAS_FULL_PID_PARSER
    bool ErrorInputIgnore;      /* PID parser ignores the error input signal and TEI bit */
    BXPT_ParserTimestampMode TsMode; /* Controls the type of timestamp generated as packets pass through the parser band. */
    bool AcceptNulls;       /* NULL packets are not discarded if true */
	bool AcceptAdapt00;	/* Packets with an Adaptation Field of 00b will not be dropped. */
#else
    bool ErrorInputIgnore;      /* PID parser ignores the error input signal and TEI bit */
    bool ContCountIgnore;       /* PID parser does not check continuity of enabled PID channels */
    bool SaveShortPsiMsg;       /* Save short (total length less than 7 bytes) PSI messages */
    bool SaveLongPsiMsg;        /* Save long (total length of 4097 or 4098 bytes) PSI messages */
    bool PsfCrcDis;             /* Whether or not CRC checking is performed for short form private sections */
    BXPT_PsiMesgModModes PsiMod;    /* Type of mods to be done to PSI messages transfered into the DMA buffers. */
    BXPT_ParserTimestampMode TsMode; /* Controls the type of timestamp generated as packets pass through the parser band. */

    /* 
    ** true if timebase is locked to a PCR module. false if the free running 
    ** 27 MHz clock is to be used. WhichPcrToUse specifies the PCR block to
    ** lock to.
    */
    bool UsePcrTimeBase;    
    unsigned int WhichPcrToUse; /* Which PCR module, if UsePcrTimeBase == true */

    bool AcceptNulls;       /* NULL packets are not discarded if true */
    bool AcceptAdapt00;     /* Packets with an adaptation field of 00 are accepted if true */
#endif
}
BXPT_ParserConfig;

/***************************************************************************
Summary:
Parser Settings for a per pid channel. New chip architecture supports these
setting for each pid channnel. This structure is to be used with the new 
archicture to configure the pasrer settings for each pid channel.  
****************************************************************************/
typedef struct BXPT_PidPsiConfig
{
    bool SaveShortPsiMsg;       
    bool SaveLongPsiMsg;        
    bool PsfCrcDis;             
    BXPT_PsiMesgModModes PsiMsgMod;

#if BXPT_HAS_FULL_PID_PARSER
    bool ContCountIgnore;       
    bool AcceptAdapt00;     /* Packets with an adaptation field of 00 are accepted if true */
#endif
}
BXPT_PidPsiConfig;

/***************************************************************************
Summary:
Settings for a input band. 
****************************************************************************/
typedef struct BXPT_InputBandConfig
{
    BXPT_Polarity ClockPolSel;  /* Defines which edge of the input band clock is used */         
    BXPT_Polarity SyncPolSel;   /* Input band sync signal active high/low select */           
    BXPT_Polarity DataPolSel;   /* Input band data signal active high/low select */           
    BXPT_Polarity ValidPolSel;  /* Input band valid signal active high/low select */           
    BXPT_Polarity ErrorPolSel;  /* Input band error signal active high/low select */           
    bool EnableErrorInput;      /* Use the error input signal */
    bool SyncDetectEn;          /* Selects whether the circuit performs local sync detection */   
    bool UseSyncAsValid;        /* Sync input is used as valid; internal sync detection must be enabled by the user */   
    bool ForceValid;            /* Use input valid signal or ignore it. */           
    bool LsbFirst;              /* Controls whether each byte is received MSB or LSB first */   
    bool ParallelInputSel;      /* Select parallel data input for this port instead of serial. Not available on all input bands. */        
    unsigned int IbPktLength;   /* Transport packet length value used for this input band. */   
}
BXPT_InputBandConfig;

#endif

/***************************************************************************
Summary:
Defines the message buffer sizes that are supported. 
****************************************************************************/
typedef enum BXPT_MessageBufferSize
{
    BXPT_MessageBufferSize_e1kB,
    BXPT_MessageBufferSize_e2kB,
    BXPT_MessageBufferSize_e4kB,
    BXPT_MessageBufferSize_e8kB,
    BXPT_MessageBufferSize_e16kB,
    BXPT_MessageBufferSize_e32kB,
    BXPT_MessageBufferSize_e64kB,
    BXPT_MessageBufferSize_e128kB,
    BXPT_MessageBufferSize_e256kB,
    BXPT_MessageBufferSize_e512kB
}
BXPT_MessageBufferSize;

/***************************************************************************
Summary:
A PSI/SI message filter. The fields of the structure are written to or read
from the hardware filters by the BXPT_GetFilter() and BXPT_SetFilter()
calls.

The arrays are read or written using contents of the first byte as the most 
significant bits of the filter, and subsequent bytes containing subsequen
bits. I.e. Mask[0] contains mask bits 96 through 103, mask[1] contains bits 
64 through 95, etc.
****************************************************************************/
typedef struct BXPT_Filter
{
    uint8_t Mask[ BXPT_FILTER_SIZE ];           
    uint8_t Coeficient[ BXPT_FILTER_SIZE ];
    uint8_t Exclusion[ BXPT_FILTER_SIZE ];
} 
BXPT_Filter;

/***************************************************************************
Summary:
Data needed to configure a channel for PSI message filtering. Caller 
supplies the PID and band that carry the packets to be filtered. The filter
enable masks are a bit map, with 1 bit for each filter in the transport 
core. To enable filtering by filter 2, set bit 2 in the appropriate mask
member ( this is done automatically by BXPT_AddFilterToGroup() ).
****************************************************************************/
typedef struct BXPT_PsiMessageSettings
{
    unsigned int Pid;                   /* Which PID carries the PSI messages. */
    unsigned int Band;                  /* The band that carries the PID. */
    bool CrcDisable;                    /* Disable CRC checks for this PID channel. */
    
    unsigned int Bank;                  /* Which filter bank has the filters */
    uint32_t FilterEnableMask;      /* Enable bits for filters 0 through 31 */

    /*
    ** Set to true to have data byte-aligned in the message buffer. If false,
    ** the data is word aligned, with bytes of value 0x55 inserted as padding.
    ** This is done for software compatability with older chips that didn't
    ** support byte-alignment.
    */
    bool ByteAlign;       
    
    /* 
    ** Use R-pipe data. This will affect ALL message buffers being fed by 
    ** the PID channel is buffer is using. Hardware forces that condition. 
    **  
    ** In summary, R pipe data will be used if either UseRPipe is true or 
    ** if MesgDataOnRPipe was true when BXPT_Open() was called.
    */
    bool UseRPipe;

    unsigned StartingOffset;    /* Offset into the message section where the filter comparison will start. 
                                Default is 0, max is 255. */
    bool SkipByte2;         /* Skip the 3rd byte (offset 2) when doing the filter comparison. Default is false. */
} 
BXPT_PsiMessageSettings;

/***************************************************************************
Summary:
Defines the types of single PID channel recoding this transport supports. 
****************************************************************************/
typedef enum BXPT_SingleChannelRecordType
{
    BXPT_SingleChannelRecordType_ePes,              /* Recording PES level data. */
    BXPT_SingleChannelRecordType_ePacketSaveAll,    /* Recoding entire transport packets. */
    BXPT_SingleChannelRecordType_ePesSaveAll
}
BXPT_SingleChannelRecordType;

/***************************************************************************
Summary:
Data needed to configure a single PID channel recording. 
****************************************************************************/
typedef struct BXPT_PidChannelRecordSettings
{                                               
    unsigned int Pid;                               /* Which PID to record. */ 
    unsigned int Band;                              /* The band the PID is on. */
    BXPT_SingleChannelRecordType RecordType;    /* Type of recording to do. */

    /* 
    ** For PES SAVE ALL recording, the message-ready interrupt can be configured
    ** to trigger on one (and only one) of the following conditions. 
    **
    **  SaveAllCountType = 0 - Interrupt after the number of transport packets
    ** received equals SaveAllCount (below).
    **
    **  SaveAllCountType = 1 - Interrupt after the number of 32-bit words
    ** received equals SaveAllCount (below).
    */
    unsigned int SaveAllCountType;              
    
    /* 
    ** Number of parsed transport packets, or 32-bit words, that need to written
    ** to DRAM in order to generate a message-ready interrupt. 
    */
    unsigned int SaveAllCount;                  
    /*
    ** Set to true to have data byte-aligned in the message buffer. If false,
    ** the data is word aligned, with bytes of value 0x55 inserted as padding.
    ** This is done for software compatability with older chips that didn't
    ** support byte-alignment.
    */
    bool ByteAlign;                
     
    /* 
    ** Use R-pipe data. This will affect ALL message buffers being fed by 
    ** the PID channel is buffer is using. Hardware forces that condition. 
    **  
    ** In summary, R pipe data will be used if either UseRPipe is true or 
    ** if MesgDataOnRPipe was true when BXPT_Open() was called.
    */
    bool UseRPipe;
} 
BXPT_PidChannelRecordSettings;


#if BXPT_HAS_FULL_PID_PARSER
/***************************************************************************
Summary:
Control the Continuity Count checking, configurable on a per-PID channel basis. 
****************************************************************************/
typedef struct BXPT_PidChannel_CC_Config
{
	bool Primary_CC_CheckEnable;		/* CC handling for the Primary PID (PID Table). */
	bool Secondary_CC_CheckEnable;		/* CC handling for the Secondary PID (SPID Table). */
	bool Generate_CC_Enable;			/* Generate and insert CC values for TS streams generated by the playback packetizer */
    unsigned Initial_CC_Value;          /* Starting value for inserted continuity counts. Used when Generate_CC_Enable == true. */
}
BXPT_PidChannel_CC_Config;
#endif

#if BXPT_HAS_PARSER_REMAPPING
/***************************************************************************
Summary: 
In XPT cores that support this feature, a physical input band  parser or 
playback parser may be "remapped" to another "virtual" input band or 
playback parser. Packets seen by a phyiscal parser are presented to the rest 
of XPT as if they came from the virtual parser. 
 
For example, if front-end parser 2 is remapped to playback parser 4, then 
all packets seen by front-end parser 2 will be treated by the rest of XPT 
as if they came from playback parser 4. This affects how blocks like the 
PID table are configured. 
 
At BXPT_Open(), all parser mappings default to themselves. I.e, front-end 
parser 0 is mapped to front-end parser 0, and so forth.  
 
All API calls that take a parser band number as an input would use the 
virtual band number, with two exceptions: the indexes into the FrontEnd 
and Playback arrays used by BXPT_GetParserMapping() and 
BXPT_SetParserMapping() will always refer to physical parsers.
****************************************************************************/
typedef struct BXPT_BandMap
{
    unsigned VirtualParserBandNum;
    bool VirtualParserIsPlayback;
}
BXPT_BandMap;

/* Physical to virtual parser band mapping. */
typedef struct BXPT_ParserBandMapping
{
    /* The indexes into these arrays represent physical parser bands. */
    BXPT_BandMap FrontEnd[ BXPT_NUM_PID_PARSERS ];
    BXPT_BandMap Playback[ BXPT_NUM_PLAYBACKS ];
}
BXPT_ParserBandMapping;

/***************************************************************************
Summary:
Get the current parser band mapping. 
 
Description:
Retrieve the current physical to virtual parser band mapping. See the comments 
for the BXPT_ParserBandMapping structure for more information.
 
Returns:
    void 
****************************************************************************/
void BXPT_GetParserMapping(
    BXPT_Handle hXpt,           /* [in] Handle for the Transport. */ 
    BXPT_ParserBandMapping *ParserMap
    );

/***************************************************************************
Summary:
Set the current parser band mapping. 
 
Description:
Set the current physical to virtual parser band mapping. See the comments 
for the BXPT_ParserBandMapping structure for more information.
 
Returns:
    void 
****************************************************************************/
BERR_Code BXPT_SetParserMapping(
    BXPT_Handle hXpt,           /* [in] Handle for the Transport. */ 
    const BXPT_ParserBandMapping *ParserMap
    );
#endif

/***************************************************************************
Summary:
Return the default settings for the transport core. 

Description:
The transport core has certain default settings which the user can override
when the device is opened. This function returns those defaults in a structure
that can be changed, and then passed to BXPT_Open().

Returns:
    BERR_SUCCESS                - Device opened.
    BERR_INVALID_PARAMETER      - Bad input parameter
 
See Also:
BXPT_Open
****************************************************************************/
BERR_Code BXPT_GetDefaultSettings(
    BXPT_DefaultSettings *Defaults, /* [out] Defaults to use during init.*/
    BCHP_Handle hChip               /* [in] Handle to used chip. */
    );

/***************************************************************************
Summary:
Opens the transport module. 

Description:
The transport core is reset and initialized. Dynamic resources needed by the 
transport module are allocated. A handle to use when configuring the core is 
created and returned. The caller may specify certain default and/or 
initialization actions by setting the appropriate members of the default 
settings parameter.

Returns:
    BERR_SUCCESS                - Device opened.
    BERR_OUT_OF_DEVICE_MEMORY   - Memory failure (should not happen)
    BERR_INVALID_PARAMETER      - Bad input parameter
 
See Also:
BXPT_GetDefaultSettings, BXPT_Close
****************************************************************************/
BERR_Code BXPT_Open(
    BXPT_Handle *hXpt,                      /* [out] Transport handle. */
    BCHP_Handle hChip,                      /* [in] Handle to used chip. */
    BREG_Handle hRegister,                  /* [in] Handle to access regiters. */
    BMEM_Handle hMemory,                    /* [in] Handle to memory heap to use. */
    BINT_Handle hInt,                       /* [in] Handle to interrupt interface to use. */
    const BXPT_DefaultSettings *Defaults    /* [in] Defaults to use during init.*/
    );

/***************************************************************************
Summary:
Close the transport module.

Description:
Release any memory and other system resources allocated by the data transport 
core. Reset the transport core to insure that all interrupt sources are
disabled.

Returns:
    void                

See Also:
BXPT_Open
****************************************************************************/
void BXPT_Close(
    BXPT_Handle hXpt        /* [in] Handle for the Transport to be closed. */ 
    );

/***************************************************************************
Summary:
Standby settings for XPT
****************************************************************************/
typedef struct BXPT_StandbySettings 
{
#if BXPT_HAS_WAKEUP_PKT_SUPPORT
    /* 
    ** Enable handling of "wakeup" packets from the demod. See bxpt_wakeup.h 
    ** for a full description and example code. Enabling this feature will 
    ** keep the CLK216 to XPT running during standby mode. 
    */
    bool UseWakeupPacket; /* Defaults to false. */
#endif
    /* XPT can enter standby in S3 or S2 mode.
       in S2 standby, only SRAM-based registers are saved/restored. 
       in S3 standby, all registers are saved/restored. */
    bool S3Standby; /* Defaults to false */
} 
BXPT_StandbySettings;

/***************************************************************************
Summary:
Get default standby settings

Description:
Return default standby settings for BXPT_Standby

Returns:
    void

See Also:
BXPT_Standby
****************************************************************************/
void BXPT_GetDefaultStandbySettings(
    BXPT_StandbySettings *pSettings
    );

/***************************************************************************
Summary:
Enter standby mode

Description:
Enter standby with XPT. This allows you to enter a minimal power state without
calling BXPT_Close. Standby is not possible if XPT is currently in use.
This means that upper-level SW must disable parsers and close all playback,
packetsub and remux channels.

If standby is not supported on this platform, calling this function has no effect.

No BXPT_* calls are allowed until standby mode is exitted by calling 
BXPT_Resume.

Returns:
    BERR_SUCCESS - Standby entered successfully
    BERR_UNKNOWN - Could not enter standby
    
See Also:
BXPT_Resume
****************************************************************************/
BERR_Code BXPT_Standby(
    BXPT_Handle hXpt,
    BXPT_StandbySettings *pSettings
    );

/***************************************************************************
Summary:
Exit standby mode

Description:
Exit standby with XPT. This re-enables XPT clocks and upper-level SW is  
free to make BXPT_* calls again.

Returns:
    BERR_SUCCESS - Standby exitted successfully
    BERR_UNKNOWN - Currently not in standby mode 

See Also:
BXPT_Standby
****************************************************************************/
BERR_Code BXPT_Resume(
    BXPT_Handle hXpt
    );

/***************************************************************************
Summary:
Get the capability params for the transport core.

Description:
The transport core has a certain resources, such as PID channels, that can
vary in number from one version of the chip to the next. This function returns
the quantity of each such resource in the device reference by the transport
handle. See the structure definition for the complete list of info that is 
returned.

Returns:
    void
****************************************************************************/
void BXPT_GetCapability(
    BXPT_Handle hXpt,           /* [in] Which transport to get data. */
    BXPT_Capability *Capability /* [out] Where to put the capability data. */
    );

#if BXPT_HAS_IB_PID_PARSERS             
/***************************************************************************
Summary:
Retrieves the current parser band configuration.

Description:
Read the current settings for the given parser band from the chip. Settings 
are returned in the structure that is passed in. See the structure for the
complete list of supported settings.

Returns:
    BERR_SUCCESS                - Copied the parser config.
    BERR_INVALID_PARAMETER      - Bad input parameter
 
See Also:
BXPT_SetParserConfig
****************************************************************************/
BERR_Code BXPT_GetParserConfig(
    BXPT_Handle hXpt,               /* [in] Handle for the transport to access. */
    unsigned int ParserNum,             /* [in] Which parser band to access. */
    BXPT_ParserConfig *ParserConfig /* [out] The current settings */
    );

/***************************************************************************
Summary:
Sets the current parser band configuration.

Description:
Writes the given parser band configuration to the device. The caller supplies
a structure containing the new parser band settings. The new settings take
effect as soon as the function returns.

Returns:
    BERR_SUCCESS                - Transfered the parser config to the chip.
    BERR_INVALID_PARAMETER      - Bad input parameter
 
See Also:
BXPT_GetParserConfig
****************************************************************************/
BERR_Code BXPT_SetParserConfig(
    BXPT_Handle hXpt,                       /* [in] Handle for the transport to access. */
    unsigned int ParserNum,             /* [in] Which parser band to access. */
    const BXPT_ParserConfig *ParserConfig   /* [in] The new settings */
    );

#if BXPT_HAS_STATIC_PID2BUF				

/***************************************************************************
Summary:
Override the PSI settings for the Pid channel or Reset the per pid channel PSI
settings to the paser band settings  

Description:
Writes the given PSI configuration to the pid channel or reset the PSI configuration 
to the paser band setting .Each parser band has its own PSI settings set by 
BXPT_SetParserConfig API. On the new chip architectures each pid channel can have its 
own PSI settings. This API lets the user change the PSI settings on per pid channel 
basis. User should un-override the setting when different PSI settings are no
longer desired 

Returns:
    BERR_SUCCESS                - Transfered the parser config to the chip.
    BERR_INVALID_PARAMETER      - Bad input parameter
 
See Also:
BXPT_GetParserConfig,BXPT_GetPidChannelPsiSettings
****************************************************************************/
BERR_Code BXPT_SetPidChannelPsiSettings(
        BXPT_Handle hXpt,                  /* [In] Handle for this transport */
        unsigned int PidChannelNum,        /* [In] The pid channel to configure. */
        bool OverrideParserSettings,       /* [In] If set true the PSI settings for 
                                                   this pid channel will be changed to Config
                                                   else restored to the paser band to which 
                                                   this pid channel is allocated */
        const BXPT_PidPsiConfig *Config    /* [In] Message mode setting for pid channel. */
        );  

/***************************************************************************
Summary:
Retrieves the current PSI setting for the given pid channel.

Description:
Read the current settings for the given pid channel chip. Settings 
are returned in the structure that is passed in. See the structure for the
complete list of supported settings.

Returns:
    BERR_SUCCESS                - Copied the parser config.
    BERR_INVALID_PARAMETER      - Bad input parameter
 
See Also:
BXPT_SetPidChannelPsiSettings
****************************************************************************/
BERR_Code BXPT_GetPidChannelPsiSettings(
        BXPT_Handle hXpt,                  /* [In] Handle for this transport */
        unsigned int PidChannelNum,        /* [In] The pid channel to configure. */
        bool *OverrideParserSettings,       /* [Out] true if the pid channel is overriden */
        BXPT_PidPsiConfig *Config    /* [Out] Message mode setting for pid channel. */
        );

/***************************************************************************
Summary:
Sets up a region of memory to be used as a PID channel buffer. 

Description:
Setup a region of memory to be used as a PID channel's message buffer. The 
caller specifies the size (in bytes) of the memory region. The caller may 
allocate the memory to be used; in that case, a pointer to the user-allocated 
region would be passed in through the CpuAddr param. If CpuAddr is NULL, 
this function will do the allocation internally, using BufferSize as the 
size to allocate. User-allocated buffers must be 1024-byte aligned.

Note that the if the caller allocates memory, the caller is also responsible 
for freeing it. Memory allocated internally by this call will be freed when the 
transport handle is closed, or when it is called again with a different 
BufferSize and a NULL CpuAddr. 

Also, if the caller allocates memory, the size of the area must match one of
the sizes defined by the BXPT_MessageBufferSize enumeration. These are the 
only buffer sizes supported by the underlying hardware. User allocated memory
must be obtained through the BMEM memory manager, preferably by using 
BMEM_AllocAligned().

Returns:
    BERR_SUCCESS                - Message buffer configured.
    BERR_INVALID_PARAMETER      - Bad input parameter
***************************************************************************/
BERR_Code BXPT_SetPidChannelBuffer( 
    BXPT_Handle hXpt,                   /* [in] Handle for this transport */
    unsigned int PidChannelNum,             /* [in] Which PID channel buffer we want. */
    void *CpuAddr,                  /* [in] Caller allocated memory, or NULL. */
    BXPT_MessageBufferSize BufferSize   /* [in] Size, in bytes, of the buffer. */
    );

/***************************************************************************
Summary:
Add a filter to a group of filters.

Description:
Add a given filter to the group of filters maintained in the Settings 
structure. All filters in the group will be used to filter messages carried
on the PID channel the group is associated with. Pid channel must be allocated 
and configured before calling this function

Returns:
    BERR_SUCCESS                - Filter successfully added to the group.
    BERR_INVALID_PARAMETER      - Bad input parameter

See Also:
BXPT_AllocPSIFilter, BXPT_GetFilter, BXPT_SetFilter, 
BXPT_RemoveFilterFromGroup
***************************************************************************/
BERR_Code BXPT_AddFilterToGroup(
    BXPT_Handle hXpt,                   /* [in] Handle for this transport */
    unsigned int PidChannelNum,         /* [in] Pid channel number */
    unsigned int FilterNum,             /* [in] Which filter to add. */
    BXPT_PsiMessageSettings *Settings   /* [in,out] Filter group to add to. */
    );
	
/***************************************************************************
Summary:
Configure for recording a PID channel.

Description:
Configure for recording a single PID channel, using the message buffers. 
The PID, parser band, and type of recording to do are specified in the 
Settings structure. This function will enable the PID channel, but will
not install an interrupt handler for the message interrupt. Installing
the interrupt should be done before calling this function.

The caller must allocate a PID channel before using the function. It should
NOT be called if the PID channel is already enabled.

BXPT_SetPidChannelBuffer() must have been called at some point prior to 
calling this function, to insure that the message buffer for this channel
has been initialized. 

Returns:
    BERR_SUCCESS                - PID channel record configured.
    BERR_INVALID_PARAMETER      - Bad input parameter
    BXPT_ERR_MESG_BUFFER_NOT_CONFIGURED - Message buffer not initialized.

See Also:
BXPT_AllocPidChannel, BXPT_StopPidChannelRecord, BXPT_SetPidChannelBuffer
***************************************************************************/
BERR_Code BXPT_StartPidChannelRecord( 
    BXPT_Handle hXpt,                               /* [in] Handle for this transport */
    unsigned int PidChannelNum,                         /* [in] Which PID channel. */
    const BXPT_PidChannelRecordSettings *Settings   /* [in] Record settings. */
    );

/***************************************************************************
Summary:
Disable recording of a PID channel.

Description:
Stops the recording of packet or PES data on the given PID channel. The PID
channel is disabled. The interrupt handler, if there was one installed, will
not be disabled; thus, any data still in the hardware may trigger an interrupt
after this function returns.

NOTE: This function may sleep for up to 200 microseconds, in order to flush 
any remaining data from the hardware's internal buffers.

Returns:
    BERR_SUCCESS                - PID channel record stopped.
    BERR_INVALID_PARAMETER      - Bad input parameter

See Also:
BXPT_StartPidChannelRecord
***************************************************************************/
BERR_Code BXPT_StopPidChannelRecord( 
    BXPT_Handle hXpt,                           /* [in] Handle for this transport */
    unsigned int PidChannelNum                      /* [in] Which PID channel. */
    );
	
/***************************************************************************
Summary:
Configure for capturing PSI messages.

Description:
Associate one or more PSI message filters with a given PID and parser band.
This function will enable the PID channel, but will not install an 
interrupt handler for the message interrupt. The interrupt handler should be
installed before calling this function.

The caller must allocate a PID channel before using the function. It should
NOT be called if the PID channel is already enabled.

BXPT_SetPidChannelBuffer() must have been called at some point prior to 
calling this function, to insure that the message buffer for this channel
has been initialized. 

Returns:
    BERR_SUCCESS                - PSI filtering is setup.
    BERR_INVALID_PARAMETER      - Bad input parameter
    BXPT_ERR_MESG_BUFFER_NOT_CONFIGURED - Message buffer not initialized.

See Also:
BXPT_AllocPSIFilter, BXPT_GetFilter, BXPT_SetFilter, BXPT_AddFilterToGroup,
BXPT_StopPsiMessageCapture, BXPT_SetPidChannelBuffer
***************************************************************************/
BERR_Code BXPT_StartPsiMessageCapture( 
    BXPT_Handle hXpt,                           /* [in] Handle for this transport */
    unsigned int PidChannelNum,                     /* [in] Which PID channel. */
    const BXPT_PsiMessageSettings *Settings     /* [in] PID, band, and filters to use. */
    );

/***************************************************************************
Summary:
Stop capturing PSI messages.

Description:
Disable the PID channel being used to capture PSI messages. Flush any data
still in the chip. If the interrupts for the message buffer are still 
enabled, flushing the data may trigger another interrupt.

NOTE: This function may sleep for up to 200 microseconds, in order to flush 
any remaining data from the hardware's internal buffers.

Returns:
    BERR_SUCCESS                - PSI filtering is stopped.
    BERR_INVALID_PARAMETER      - Bad input parameter

See Also:
BXPT_StartPsiMessageCapture
***************************************************************************/
BERR_Code BXPT_StopPsiMessageCapture( 
    BXPT_Handle hXpt,                           /* [in] Handle for this transport */
    unsigned int PidChannelNum                  /* [in] Which PID channel. */
    );

/***************************************************************************
Summary:
Get the current message buffer error status.

Description:
Get the error status of all the message buffers on per pid channel basis. 
This API should be used with L2 error interrupt to identify the type of error a
nd message bufffer which has triggered the error interrupt

Returns:
    BERR_SUCCESS                    - 
    BERR_INVALID_PARAMETER          - Bad buffer range
See Also:
***************************************************************************/

typedef enum BXPT_PidChannelRange
{
    BXPT_PidChannelRange_00_31=0,
    BXPT_PidChannelRange_32_63,
    BXPT_PidChannelRange_64_95,
    BXPT_PidChannelRange_96_127
}
BXPT_PidChannelRange;

BERR_Code BXPT_GetMesgBufferErrorStatus(
    BXPT_Handle hXpt,                       /* [in]  Handle for this transport */
    BXPT_PidChannelRange PidChannelRange,   /* [in]  Range of pid channel numbers */
    unsigned int *ErrorStatus               /* [out] Bitwise error status,single bit 
                                                     is dedicated to each pid channel
                                                    indicated by the rage parameter above   */
    );
#endif

/***************************************************************************
Summary:
Retrieves the default input band configuration.

Description:
Read the default settings for the given input band from the chip. Settings 
are returned in the structure that is passed in. See the structure for the
complete list of supported settings.

Returns:
    BERR_SUCCESS                - Default config returned.
    BERR_INVALID_PARAMETER      - Bad input parameter
 
See Also:
BXPT_SetInputBandConfig
****************************************************************************/
BERR_Code BXPT_GetDefaultInputBandConfig(
	BXPT_Handle hXpt,						/* [in] Handle for the transport to access. */
	unsigned int BandNum,						/* [in] Which input band to access. */
	BXPT_InputBandConfig *InputBandConfig	/* [out] The current settings */
	);

/***************************************************************************
Summary:
Retrieves the current input band configuration.

Description:
Read the current settings for the given input band from the chip. Settings 
are returned in the structure that is passed in. See the structure for the
complete list of supported settings.

Returns:
    BERR_SUCCESS                - Retrieved the input band config.
    BERR_INVALID_PARAMETER      - Bad input parameter
 
See Also:
BXPT_SetInputBandConfig
****************************************************************************/
BERR_Code BXPT_GetInputBandConfig(
    BXPT_Handle hXpt,                       /* [in] Handle for the transport to access. */
    unsigned int BandNum,                       /* [in] Which input band to access. */
    BXPT_InputBandConfig *InputBandConfig   /* [out] The current settings */
    );

/***************************************************************************
Summary:
Sets the current input band configuration.

Description:
Writes the given input band configuration to the device. The caller supplies
a structure containing the new input band settings. The new settings take
effect as soon as the function returns.

Returns:
    BERR_SUCCESS                - Transfered the input config to the chip.
    BERR_INVALID_PARAMETER      - Bad input parameter
 
See Also:
BXPT_GetInputBandConfig
****************************************************************************/
BERR_Code BXPT_SetInputBandConfig(
    BXPT_Handle hXpt,                           /* [in] Handle for the transport to access. */
    unsigned int BandNum,                       /* [in] Which input band to access. */
    const BXPT_InputBandConfig *InputBandConfig /* [in] The new settings */
    );

/***************************************************************************
Summary:
Configure parser band to pass ALL PIDS except NULLs.

Description:
Enable the parser band's all-pass mode. If enabled, the parser will ONLY 
filter out NULL PIDs. All other PIDs in the stream will forwarded on a 
single PID channel. That single PID channel fixed for each parser band:
parser 0 will forward packets on PID channel 0, parser 1 will forward on
PID channel 1, etc. 

Returns:
    BERR_SUCCESS                - All-Pass mode changed per AllPass argument.
    BERR_INVALID_PARAMETER      - Bad input parameter
****************************************************************************/
BERR_Code BXPT_ParserAllPassMode( 
    BXPT_Handle hXpt,   /* [in] Handle for the transport to access. */
    unsigned int BandNum,                       /* [in] Which input band to access. */
    bool AllPass        /* [in] All-pass enabled if true, or not if false. */
    );

/***************************************************************************
Summary:
Set a parser band's data source.

Description:
Select the data source for a given parser band. The allowable data sources
are listed in the ParserDataSource enumeration. There may be (and probably 
will be) more than 1 instance of each ParserDataSource; the WhichSource 
parameter selects which instance is to be used. 

Returns:
    BERR_SUCCESS                - Data source has been set in hardware.
    BERR_INVALID_PARAMETER      - Bad input parameter

See Also:
BXPT_GetParserDataSource
***************************************************************************/
BERR_Code BXPT_SetParserDataSource( 
    BXPT_Handle hXpt,               /* [in] Handle for this transport */
    unsigned int ParserBand,            /* [in] Which parser to configure */
    BXPT_DataSource DataSource,     /* [in] The data source. */
    unsigned int WhichSource            /* [in] Which instance of the source */
    );      

/***************************************************************************
Summary:
Get a parser band's data source.

Description:
Return the data source for a given parser band. The allowable data sources
are listed in the ParserDataSource enumeration. There may be (and probably 
will be) more than 1 instance of each ParserDataSource; the WhichSource 
parameter denotes which instance is currently being used. 

Returns:
    BERR_SUCCESS                - Data source retrieved from hardware.
    BERR_INVALID_PARAMETER      - Bad input parameter

See Also:
BXPT_SetParserDataSource
***************************************************************************/
BERR_Code BXPT_GetParserDataSource( 
    BXPT_Handle hXpt,               /* [in] Handle for this transport */
    unsigned int ParserBand,            /* [in] Which parser to configure */
    BXPT_DataSource *DataSource,    /* [out] The data source. */
    unsigned int *WhichSource           /* [out] Which instance of the source */
    );      

/***************************************************************************
Summary:
Enable or disable a parser band.

Description:
Start or stop PID parsing for the given parser band. When enabled, the parser
band will remove all PIDs it receives that are not assigned to a PID channel.
When disabled, no all PIDs will be removed. 

Returns:
    BERR_SUCCESS                - Data source retrieved from hardware.
    BERR_INVALID_PARAMETER      - Bad input parameter
***************************************************************************/
BERR_Code BXPT_SetParserEnable( 
    BXPT_Handle hXpt,               /* [in] Handle for this transport */
    unsigned int ParserBand,            /* [in] Which parser to configure */
    bool Enable                 /* [in] Parser enabled if true, disabled if false. */
    );

#endif      

/***************************************************************************
Summary:
Enable the given PID channel. This will not allocate a PID channel.

Description:
Enable a PID channel. Sets the enable bit in the given PID channel's 
register. This function will NOT allocate a PID channel.

Returns:
    BERR_SUCCESS                - PID channel enabled.
    BERR_INVALID_PARAMETER      - Bad input parameter
***************************************************************************/
BERR_Code BXPT_EnablePidChannel( 
    BXPT_Handle hXpt,               /* [in] Handle for this transport */
    unsigned int PidChannelNum          /* [in] Which PID channel. */
    );

/***************************************************************************
Summary:
Disable the given PID channel.

Description:
Clears the enable bit in the given PID channel's register. This call will
not deallocate the channel; it simply turns the channel off.

Returns:
    BERR_SUCCESS                - PID channel disabled.
    BERR_INVALID_PARAMETER      - Bad input parameter

See Also:
BXPT_EnablePidChannel
***************************************************************************/
BERR_Code BXPT_DisablePidChannel( 
    BXPT_Handle hXpt,               /* [in] Handle for this transport */
    unsigned int PidChannelNum          /* [in] Which PID channel. */
    );                    

/***************************************************************************
Summary:
Allocate a new PID channel.

Description:
Allocate an unused PID channel, and return that channel's number. This is 
usefull for cases where the caller needs a PID channel, but hasn't 
determined which PID or parser band that channel will be assigned to. 

The caller specifies if the channel must have a message buffer associated
with it. In some architectures, PID channels exist that don't have message
buffer support. Channels used solely for audio or video decoding are one
example.
 
Returns:
    BERR_SUCCESS                - PID channel allocated.
    BERR_INVALID_PARAMETER      - Bad input parameter

See Also:
BXPT_FreePidChannel, BXPT_FreeAllPidChannels
***************************************************************************/
BERR_Code BXPT_AllocPidChannel( 
    BXPT_Handle hXpt,           /* [in] Handle for this transport */
    bool NeedMessageBuffer,     /* [in] Is a message buffer required? */
    unsigned int *PidChannelNum     /* [out] The allocated channel number. */
    );

/***************************************************************************
Summary:
Set the PID and parser band for a given PID channel. 

Description:
Configure the PID and parser band for the given PID channel. It is not 
necessary to allocate the PID channel first, although it is recommended that
this be done. The PID channel is not enabled; see BXPT_EnablePidChannel().

Note: For 74xx builds, the hardware supports two different classes of parsers:
Input Band and Playback Channel. The two are entirely separate and can't be
used interchangably. To select a playback parser, the Band argument must
be wrapped is wrapped in a macro that indicates the argument refers to a
playback parser, like so:

BXPT_ConfigurePidChannel( hXpt, PidChannelNum, Pid, BXPT_PB_PARSER( BandNum ) );

If the BXPT_PB_PARSER() macro is not used, BandNum is taken to refer to an
Input Band parser. 

Returns:
    BERR_SUCCESS                - PID channel configured.
    BERR_INVALID_PARAMETER      - Bad input parameter

See Also:
BXPT_EnablePidChannel, BXPT_SetPidChnlConfig
***************************************************************************/
BERR_Code BXPT_ConfigurePidChannel( 
    BXPT_Handle hXpt,           /* [in] Handle for this transport */
    unsigned int PidChannelNum,     /* [in] Which channel to configure. */
    unsigned int Pid,               /* [in] PID to use. */
    unsigned int Band               /* [in] The parser band to use. */
    );

#ifdef ENABLE_PLAYBACK_MUX
/***************************************************************************
Summary:
Sets a PID channel to the switched state.

Description:
This function turns on or off PID channel switching on the specified PID
channel.  This is used to provide a multiplexed input on top of a playback
parser (specifically the last playback parser).

Returns:
    BERR_SUCCESS                - PID channel configured.
    BERR_INVALID_PARAMETER      - Bad input parameter

See Also:
BXPT_SwitchPidChannel
***************************************************************************/
BERR_Code BXPT_SetPidChannelSwitched(
	BXPT_Handle hXpt, 	   				/* [in] Handle for this transport */
	unsigned int PidChannelNum,			/* [in] Which PID channel. */
	bool IsSwitched						/* [in] Switch state. */
	);

/***************************************************************************
Summary:
Sets a PID channel to the switched state. Callable from an interrupt context.

Description:
Same as BXPT_SetPidChannelSwitched() but callable from an ISR. 

Returns:
    BERR_SUCCESS                - PID channel configured.
    BERR_INVALID_PARAMETER      - Bad input parameter

See Also:
BXPT_SwitchPidChannel
***************************************************************************/
BERR_Code BXPT_SetPidChannelSwitched_isr(
	BXPT_Handle hXpt, 	   				/* [in] Handle for this transport */
	unsigned int PidChannelNum,			/* [in] Which PID channel. */
	bool IsSwitched						/* [in] Switch state. */
	);

/***************************************************************************
Summary:
Enables/disables a switched PID channel.  Never to be used within an ISR.

Description:
This function is used to enable or disable a switched PID channel.  Switched
PID channels are used to provide a multiplexed input on top of a playback
parser (specifically the last playback parser).

Returns:
    TRUE						- PID channel was enabled/disabled
    FALSE						- PID channel was unable to be enabled/disabled

See Also:
BXPT_SetPidChannelSwitched, BXPT_SwitchPidChannelISR
***************************************************************************/
bool BXPT_SwitchPidChannel(
	BXPT_Handle hXpt, 	   				/* [in] Handle for this transport */
	unsigned int PidChannelNum,			/* [in] Which PID channel. */
	bool EnableIt						/* [in] Enabled state. */
	);

/***************************************************************************
Summary:
Enables/disables a switched PID channel.  To be used only within an ISR.

Description:
This function is used to enable or disable a switched PID channel.  Switched
PID channels are used to provide a multiplexed input on top of a playback
parser (specifically the last playback parser).

Returns:
    TRUE						- PID channel was enabled/disabled
    FALSE						- PID channel was unable to be enabled/disabled

See Also:
BXPT_SetPidChannelSwitched, BXPT_SwitchPidChannel
***************************************************************************/
bool BXPT_SwitchPidChannelISR(
	BXPT_Handle hXpt, 	   				/* [in] Handle for this transport */
	unsigned int PidChannelNum,			/* [in] Which PID channel. */
	bool EnableIt						/* [in] Enabled state. */
	);

/***************************************************************************
Summary:
Set the number of playback blocks to be used for playback muxing.

Description:
This function is used to specify how many hardware playback blocks are
to be used for time-multiplexing playback of transport streams. Of the
available HW playback blocks, the last "Num" blocks will be used for muxing.
This function must only be called once and called during system initialization.

Returns:
    BERR_SUCCESS						- Playback blocks are reserved for muxing.
    BERR_INVALID_PARAMETER				- Bad input parameter

***************************************************************************/
BERR_Code BXPT_SetNumPlaybackMux(
	BXPT_Handle hXpt, 	   				/* [in] Handle for this transport */
	unsigned int Num           			/* [in] Number of playback mux blocks */
	);

#endif /*ENABLE_PLAYBACK_MUX*/
  
/***************************************************************************
Summary:
Free the given PID channel.

Description:                                  
An allocated PID channel is returned to the pool of available channel. The 
PID channel must have been previously allocated by a call to 
BXPT_AllocPidChannel(). 
 
Returns:
    BERR_SUCCESS                - PID channel freed.
    BERR_INVALID_PARAMETER      - Bad input parameter

See Also:
BXPT_EnablePidChannel, BXPT_AllocPidChannel
***************************************************************************/
BERR_Code BXPT_FreePidChannel( 
    BXPT_Handle hXpt,           /* [in] Handle for this transport */
    unsigned int PidChannelNum      /* [in] PID channel to free up. */
    );      

/***************************************************************************
Summary:
Free all allocated PID channels.

Description:
Release ALL the PID channels associated with the given data transport core.
Each channel is released. The PID channels must have been previously 
allocated by a call to BXPT_AllocPidChannel(). 
 
Returns:
    void.

See Also:
BXPT_EnablePidChannel, BXPT_AllocPidChannel
***************************************************************************/
void BXPT_FreeAllPidChannels( 
    BXPT_Handle hXpt            /* [in] Handle for this transport */
    );

/***************************************************************************
Summary:
Request allocation of a specific PID channel.

Description:
In some cases, the user may want to use a specific PID channel. There is no
way to guarentee which channel BXPT_AllocPidChannel() will return, so this
function has been added to allow users to 'allocate' a specific PID channel.
If the request succeedes, the PidChannelNum can be used exactly like a 
PID channel allocated by BXPT_AllocPidChannel(). NOTE: when the user nolonger
needs the requested PID channel, BXPT_FreePidChannel() should be called to 
tell the porting interface that channel is available for alloc. 

Returns:
    BERR_SUCCESS                    - Read offset has been updated.
    BERR_INVALID_PARAMETER          - Bad PidChannelNum
    BXPT_ERR_PID_ALREADY_ALLOCATED  - The channel has already been allocated.

See Also:
BXPT_AllocPidChannel(), BXPT_FreePidChannel() 
***************************************************************************/
BERR_Code BXPT_RequestPidChannel(
    BXPT_Handle hXpt,       /* [in] Handle for this transport */
    unsigned int PidChannelNum  /* [out] The channel number the user wants. */ 
    );

#if BXPT_HAS_MESG_BUFFERS

/***************************************************************************
Summary:
Allocate a PSI message filter.

Description:
Allocate a 16-byte PSI filter, from the requested filter bank. The first unused 
filter is returned. The filters used for a given PID channel must all come
from the same bank. Note that only banks 0 and 1 can be used. 

Returns:
    BERR_SUCCESS                    - Filter allocated.
    BXPT_ERR_NO_AVAILABLE_RESOURCES - No filters available

See Also:
BXPT_FreePSIFilter
***************************************************************************/
BERR_Code BXPT_AllocPSIFilter( 
    BXPT_Handle hXpt,           /* [in] Handle for this transport */
    unsigned int Bank,          /* [in] Which bank to allocate from. */
    unsigned int *FilterNum     /* [out] Number for the allocated filter. */
    );

/***************************************************************************
Summary:
Free a PSI message filter.

Description:
Frees a previously allocated filter. The filter is marked as available for 
allocation.
 
Returns:
    BERR_SUCCESS                - Filter freed.
    BERR_INVALID_PARAMETER      - Bad input parameter

See Also:
BXPT_AllocPSIFilter
***************************************************************************/
BERR_Code BXPT_FreePSIFilter( 
    BXPT_Handle hXpt,           /* [in] Handle for this transport */
    unsigned int Bank,          /* [in] Which bank to free the filter from. */
    unsigned int FilterNum      /* [in] Which filter to free up. */
    );

/***************************************************************************
Summary:
Return the filtering arrays for a PSI filter.

Description:
Retrieve the coefficient, mask, and exclusion byte arrays for a given 
filter. The data is copied into a structure, which was passed in by pointer.
 
Returns:
    BERR_SUCCESS                - PID channel configured.
    BERR_INVALID_PARAMETER      - Bad input parameter.

See Also:
BXPT_AllocPSIFilter, BXPT_SetFilter
***************************************************************************/
BERR_Code BXPT_GetFilter( 
    BXPT_Handle hXpt,           /* [in] Handle for this transport */
    unsigned int Bank,              /* [in] Which bank the filter belongs to. */
    unsigned int FilterNum,         /* [in] Which filter to get data from. */
    BXPT_Filter *Filter         /* [out] Filter data */
    );

/***************************************************************************
Summary:
Load the filtering arrays for a PSI filter.

Description:
Load the coefficient, mask, and exclusion byte arrays for a given filter, 
using data passed in by the caller. 
 
Returns:
    BERR_SUCCESS                - PID channel configured.
    BERR_INVALID_PARAMETER      - Bad input parameter

See Also:
BXPT_AllocPSIFilter, BXPT_GetFilter
***************************************************************************/
BERR_Code BXPT_SetFilter( 
    BXPT_Handle hXpt,           /* [in] Handle for this transport */
    unsigned int Bank,              /* [in] Which bank the filter belongs to. */
    unsigned int FilterNum,         /* [in] Which filter to get data from. */
    const BXPT_Filter *Filter   /* [in] Filter data to be loaded */
    );

/***************************************************************************
Summary:
Change a filter's coefficeint byte.

Description:
Change a single coefficient byte in a given filter. Caller specifies which
byte by giving an offset into the filter array; this is the same offset
the byte had in the BXPT_Filter arrays used to initialize the filter.

Returns:
    BERR_SUCCESS                - Coeffecient byte changed.
    BERR_INVALID_PARAMETER      - Bad input parameter

See Also:
BXPT_AllocPSIFilter, BXPT_GetFilter, BXPT_SetFilter
***************************************************************************/
BERR_Code BXPT_ChangeFilterCoefficientByte( 
    BXPT_Handle hXpt,       /* [in] Handle for this transport */
    unsigned int Bank,          /* [in] Which bank the filter belongs to. */
    unsigned int FilterNum,     /* [in] Which filter to change. */
    unsigned int ByteOffset,        /* [in] Which byte in the array to change */
    uint8_t FilterByte      /* [in] New filter byte to be written. */
    );

/***************************************************************************
Summary:
Change a filter's mask byte.

Description:
Change a single mask byte in a given filter. Caller specifies which
byte by giving an offset into the filter array; this is the same offset
the byte had in the BXPT_Filter arrays used to initialize the filter.

Returns:
    BERR_SUCCESS                - Mask byte changed.
    BERR_INVALID_PARAMETER      - Bad input parameter

See Also:
BXPT_AllocPSIFilter, BXPT_GetFilter, BXPT_SetFilter
***************************************************************************/
BERR_Code BXPT_ChangeFilterMaskByte( 
    BXPT_Handle hXpt,       /* [in] Handle for this transport */
    unsigned int Bank,          /* [in] Which bank the filter belongs to. */
    unsigned int FilterNum,     /* [in] Which filter to change. */
    unsigned int ByteOffset,        /* [in] Which byte in the array to change */
    uint8_t MaskByte        /* [in] New mask byte to be written. */
    );

/***************************************************************************
Summary:
Change a filter's exclusion byte.

Description:
Change a single exclusion byte in a given filter. Caller specifies which
byte by giving an offset into the filter array; this is the same offset
the byte had in the BXPT_Filter arrays used to initialize the filter.

Returns:
    BERR_SUCCESS                - Exclusion byte changed.
    BERR_INVALID_PARAMETER      - Bad input parameter

See Also:
BXPT_AllocPSIFilter, BXPT_GetFilter, BXPT_SetFilter
***************************************************************************/
BERR_Code BXPT_ChangeFilterExclusionByte( 
    BXPT_Handle hXpt,       /* [in] Handle for this transport */
    unsigned int Bank,          /* [in] Which bank the filter belongs to. */
    unsigned int FilterNum,     /* [in] Which filter to change. */
    unsigned int ByteOffset,        /* [in] Which byte in the array to change */
    uint8_t ExclusionByte       /* [in] New exclusion byte to be written. */
    );

/* 
   NOTE: The BXPT_RemoveFilterFromGroup() API NO LONGER MAKES SENSE WITH THE
   PID2BUF HARDWARE. USERS SHOULD CALL BXPT_RemoveFilterFromGroupAndBuffer
   instead.
*/
/***************************************************************************
Summary:
Remove a filter from a group.

Description:
Remove a given filter from the group of filters maintained in the Settings 
structure. The filter will no longer be applied to the PID that the group
is associated with.

Returns:
    BERR_SUCCESS                - Filter successfully removed to the group.
    BERR_INVALID_PARAMETER      - Bad input parameter

See Also:
BXPT_AllocPSIFilter, BXPT_GetFilter, BXPT_SetFilter, BXPT_AddFilterToGroup
***************************************************************************/
BERR_Code BXPT_RemoveFilterFromGroup(
    BXPT_Handle hXpt,               /* [in] Handle for this transport */
    unsigned int FilterNum,                 /* [in] Which filter to remove. */
    BXPT_PsiMessageSettings *Settings   /* [in,out] Filter group to add to. */
    );

/***************************************************************************
Summary:
Remove a filter from a group.

Description:
Remove a given filter from the group of filters maintained in the Settings 
structure. The filter will no longer be applied to the PID that the group
is associated with.

Returns:
    BERR_SUCCESS                - Filter successfully removed to the group.
    BERR_INVALID_PARAMETER      - Bad input parameter

See Also:
BXPT_AllocPSIFilter, BXPT_GetFilter, BXPT_SetFilter, BXPT_AddFilterToGroup
***************************************************************************/
BERR_Code BXPT_RemoveFilterFromGroupAndBuffer(
    BXPT_Handle hXpt,               /* [in] Handle for this transport */
    unsigned int FilterNum,                 /* [in] Which filter to remove. */
    unsigned int BufferNum,                 /* [in] Which message buffer is using this filter. */
    BXPT_PsiMessageSettings *Settings   /* [in] Filter group to add to. */
    );

/***************************************************************************
Summary:
Get default values for the PSI Settings structure. 

Description:
Full in the given BXPT_PsiMessageSettings struct with default values.

Returns:
    void

See Also:
BXPT_AllocPSIFilter, BXPT_GetFilter, BXPT_SetFilter, BXPT_AddFilterToGroup,
BXPT_StopPsiMessageCapture, BXPT_SetPidChannelBuffer
***************************************************************************/
void BXPT_GetDefaultPsiSettings( 
    BXPT_PsiMessageSettings *Settings     /* [out] PSI defaults */
    );

/***************************************************************************
Summary:
Check for new data in a message buffer.

Description:
Check for new messages in the buffer associated with the given PID channel.
If there are messages, the address of the buffer is returned and the
'MessageSize' and 'MoreDataAvailable' params are updated as described below.
Note that their could be more than one new message in the buffer. 

The 'MessageSize' and 'MoreDataAvailable' params are used together to 
give the number of bytes that the messages occupy. Since the hardware buffer
is circular, the messages may have wrapped around the end of the buffer to 
the beginning. The 'MoreDataAvailable' param will be set to true if the 
wrap-around condition has occurred. 

Thus, if 'MoreDataAvailable' is false, the value of 'MessageSize' is the
total size of the messages. The user should read the data from 'BufferAddr',
then call BXPT_UpdateReadOffset() to signal that the data has been read.

If 'MoreDataAvailable' is true, the messages have wrapped. In this case,
'MessageSize' is the number of bytes from the start of the messages to the
end of the buffer. After reading the data and calling BXPT_UpdateReadOffset(),
the user should call BXPT_CheckBuffer() again to retrieve the rest of the 
message bytes. 

Returns:
    BERR_SUCCESS                - No errors found during the check.
    BERR_INVALID_PARAMETER      - Bad input parameter

See Also:
BXPT_UpdateReadOffset, BXPT_AllocPSIFilter, BXPT_GetFilter, BXPT_SetFilter,
BXPT_AddFilterToGroup, BXPT_SetupPidChannelRecord, 
BXPT_SetupChannelForPsiMessages
***************************************************************************/
BERR_Code BXPT_CheckBuffer( 
    BXPT_Handle hXpt,           /* [in] Handle for this transport */
    unsigned int MesgBuffNum,     /* [in] Message buffer to check. This is same as PidChNum in case of 1-to-1 mapping*/
    uint8_t **BufferAddr,       /* [out] Address of the buffer. */
    size_t *MessageSize,        /* [out] Total size of new messages. */
    bool *MoreDataAvailable     /* [out] TRUE if the buffer wrapped. */
    );

/***************************************************************************
Summary:
Check for new data in a message buffer. Account for any the possibilty that 
the data wrapped around the end of the buffer.  

Description:
Check for new messages in the buffer associated with the given PID channel. 
Two conditions could occur: the data did not wrap around, or it did. If the 
data did not wrap, the address of the buffer is returned in "BufferAddr" and 
the 'MessageSize' is set to the number of bytes in the message. "WrapBufferAddr" 
will be NULL and "MessageSizeAfterWrap" will be 0. 
 
If the data did wrap, "BufferAddr" and the 'MessageSize' will reflect the data 
before the wrap around, while "WrapBufferAddr" will be non-NULL and 
"MessageSizeAfterWrap" will indicate the number of bytes after the wrap. 

Returns:
    BERR_SUCCESS                - No errors found during the check.
    BERR_INVALID_PARAMETER      - Bad input parameter

See Also:
BXPT_UpdateReadOffset, BXPT_AllocPSIFilter, BXPT_GetFilter, BXPT_SetFilter,
BXPT_AddFilterToGroup, BXPT_SetupPidChannelRecord, 
BXPT_SetupChannelForPsiMessages
***************************************************************************/
BERR_Code BXPT_CheckBufferWithWrap( 
    BXPT_Handle hXpt,           /* [in] Handle for this transport */
    unsigned int MesgBufferNum, /* [in] buffer to check.*/
    uint8_t **BufferAddr,       /* [out] Address of the buffer. */
    size_t *MessageSizeBeforeWrap,        /* [out] Total size of new messages before the buffer wrapped. */
    uint8_t **WrapBufferAddr,    /* [out] Address of the wraparound data. 0 if no wrap */
    size_t *MessageSizeAfterWrap    /* Total size of message starting at the base. NULL if no wrap */
    );

/***************************************************************************
Summary:
Check for new data in a message buffer, and copy any new data into the user
supplied storage location.

Description:
Check for new messages in the given message buffer.
If new messages are available, they will be copied into the user supplied
storage area, up to the number of bytes passed in through the BufferSize
parameter. On return, BufferSize will indicate how many bytes where actually
copied in. The user-supplied storage area should be the same size as the 
message buffer, to insure that no data is lost due to insufficient storage.

This call supports the use of duplicated PID channels. In that case, some
PSI data in the message buffers may be corrupted. If the buffer contains 
PSI data, the data will be validated by checking the length field of each 
PSI message. If any corrupted messages are seen, the message buffer will 
be discarded and this call will not copy any data into the user-supplied 
storage. 

If duplicated PIDs are used, this call should be used instead of
BXPT_CheckBuffer() and BXPT_UpdateReadOffset(). This call can also be used 
for any cases where BXPT_CheckBuffer() and BXPT_UpdateReadOffset() are
currently used.

Returns:
    BERR_SUCCESS                - No errors found during the check.
    BERR_INVALID_PARAMETER      - Bad input parameter

See Also:
BXPT_AllocPSIFilter, BXPT_GetFilter, BXPT_SetFilter, BXPT_AddFilterToGroup,
BXPT_SetupPidChannelRecord, BXPT_SetupChannelForPsiMessages
***************************************************************************/
BERR_Code BXPT_GetBuffer( 
    BXPT_Handle hXpt,           /* [in] Handle for this transport */
    unsigned int MesgBuffNum,     /* [in] Message buffer to check. This is same as PidChNum in case of 1-to-1 mapping*/
    uint8_t *BufferAddr,        /* [out] Address of the buffer. */
    size_t *BufferSize          /* [in,out] Size of message buffer (on input), size of new messages (on output). */
    );

/***************************************************************************
Summary:
Check for new data in a message buffer, and copy any new data into the user
supplied storage location.

Description:
Check BXPT_GetBuffer() for more description. Use this API only in case if you want to 
avoid  BXPT_UpdateReadOffset() entering into critical section.
***************************************************************************/
BERR_Code BXPT_GetBuffer_isr(
    BXPT_Handle hXpt,           /* [in] Handle for this transport */
    unsigned int MesgBuffNum,     /* [in] Message buffer to check. This is same as PidChNum in case of 1-to-1 mapping*/
    uint8_t *BufferAddr,        /* [out] Address of the buffer. */
    size_t *BufferSize          /* [in,out] Size of message buffer (on input), size of new messages (on output). */
    );

/***************************************************************************
Summary:
Update a message buffer's read offset.

Description:
Update the read pointer of the given message buffer. Doing so tells the 
transport hardware that the buffer space 'behind' the read pointer is 
available for new messages. This action clears the interrupt asserted when
new data has been transfered into the buffer.

Returns:
    BERR_SUCCESS                - Read offset has been updated.
    BERR_INVALID_PARAMETER      - Bad input parameter

See Also:
BXPT_AllocPSIFilter, BXPT_GetFilter, BXPT_SetFilter, BXPT_AddFilterToGroup,
BXPT_SetupPidChannelRecord, BXPT_SetupChannelForPsiMessages
***************************************************************************/
BERR_Code BXPT_UpdateReadOffset( 
    BXPT_Handle hXpt,           /* [in] Handle for this transport */
    unsigned int MesgBuffNum,     /* [in] Message buffer to check. This is same as PidChNum in case of 1-to-1 mapping*/
    size_t BytesRead            /* [in] Number of bytes read. */
    );


/***************************************************************************
Summary:
Update a message buffer's read offset, ISR version.

Description:
Use this call when running in an interrupt context.

Returns:
    BERR_SUCCESS                - Read offset has been updated.
    BERR_INVALID_PARAMETER      - Bad input parameter

See Also:
BXPT_AllocPSIFilter, BXPT_GetFilter, BXPT_SetFilter, BXPT_AddFilterToGroup,
BXPT_SetupPidChannelRecord, BXPT_SetupChannelForPsiMessages
***************************************************************************/
BERR_Code BXPT_UpdateReadOffset_isr( 
    BXPT_Handle hXpt,           /* [in] Handle for this transport */
    unsigned int MesgBuffNum,     /* [in] Message buffer to check. This is same as PidChNum in case of 1-to-1 mapping*/
    size_t BytesRead            /* [in] Number of bytes read. */
    );

/***************************************************************************
Summary:
Pause or resume a PES recording session.

Description:
Pause or resume a PES recording that uses the message buffers. This call, 
unlike BXPT_StopPidChannelRecord(), does not disable the PID channel when the
record is stopped. If other blocks in the transport core are using this PID
channel, their data will not be interrupted by this call. 

When the PES record is resumed, the associated message buffer will be flushed,
removing any old stale data.

Returns:
    BERR_SUCCESS                - Recording pause (or resume) successful.
    BERR_INVALID_PARAMETER      - Bad input parameter

See Also:
BXPT_StartPidChannelRecord(), BXPT_StopPidChannelRecord() 
***************************************************************************/
BERR_Code BXPT_PausePesRecord(
    BXPT_Handle hXpt,                       /* [in] Handle for this transport */
    unsigned int MesgBuffNum,     /* [in] Message buffer to check. This is same as PidChNum in case of 1-to-1 mapping*/
    bool Pause                                /* [in] Enable or disable pause */
    );

/***************************************************************************
Summary:
Pause or resume a PSI capture session.

Description:
Pause or resume a PSI capture. This call, unlike BXPT_StopPidChannelRecord(), 
does not disable the PID channel when the capture is stopped. If other blocks 
in the transport core are using this PID channel, their data will not be 
interrupted.

When the PSI capture is resumed, the associated message buffer will be flushed,
removing any old stale data.

Returns:
    BERR_SUCCESS                - PSI capture pause (or resume) successful.
    BERR_INVALID_PARAMETER      - Bad input parameter

See Also:
BXPT_StartPidChannelRecord(), BXPT_StopPidChannelRecord() 
***************************************************************************/
BERR_Code BXPT_PausePsiCapture(
    BXPT_Handle hXpt,                       /* [in] Handle for this transport */
    unsigned int MesgBuffNum,     /* [in] Message buffer to check. This is same as PidChNum in case of 1-to-1 mapping*/
    bool Pause                                /* [in] Enable or disable pause */
    );

/***************************************************************************
Summary:
Get the current message buffer depth.

Description:
Compute the total number of unread bytes in the given message buffer, 
including any data that wrapped around the end of the buffer. 

Returns:
    BERR_SUCCESS                    - Depth returned in *BufferDepth.
    BERR_INVALID_PARAMETER          - Bad PidChannelNum

See Also:
BXPT_AllocPidChannel(), BXPT_FreePidChannel() 
***************************************************************************/
BERR_Code BXPT_GetMesgBufferDepth(
    BXPT_Handle hXpt,				/* [in] Handle for this transport instance */
    unsigned int PidChannelNum,			/* [in] Which PID channel's buffer to query */
    size_t *BufferDepth				/* [out] Unread byte count */
    );

/*************************************************************************** 
Summary:
Get the current message buffer error status.

Description:
Get the error status of all the message buffers. 
This API should be used with L2 error interrupt to identify the type of error 
and message bufffer which has triggered the error interrupt

Returns:
    BERR_SUCCESS                    - 
    BERR_INVALID_PARAMETER          - Bad message buffer number
See Also:
***************************************************************************/
BERR_Code BXPT_Mesg_GetErrorStatus(
    BXPT_Handle hXpt,         /* [in]  Handle for this transport */
    unsigned MesgBufferNum,   /* [in]  Which buffer to get the status of */
    bool *ErrorStatus         /* [out] Error status for the message buffer */ 
);

/***************************************************************************
Summary:
Determine which PID channel is feeding data to a given message buffer. 
 
Description:
Look up the which PID channel is mapped to the given message buffer. If such 
a channel is found, return it in *PidChannelNum. Otherwise, set *PidChannelNum 
to an invalid PID table index and BXPT_ERR_RESOURCE_NOT_FOUND is returned. 
 
Returns:
    BERR_SUCCESS                    - PidChannelNum 
    BERR_INVALID_PARAMETER          - Bad MesgBufferNum
    BXPT_ERR_RESOURCE_NOT_FOUND     - MesgBuffer is not mapped to a PID channel
 
See Also:
***************************************************************************/
BERR_Code BXPT_Mesg_GetPidChannelFromBufferNum(
    BXPT_Handle hXpt,                               /* [in] Handle for this transport */
    unsigned int MesgBufferNum,                         /* [in] Which Buffer Number. */
    unsigned int *PidChannelNum                         /* [out] Which PID channel. */
    );

#endif

/***************************************************************************
Summary:
Get a PID channel's configuration.

Description:
Retrieve the PID and parser band that a PID channel has been configured for.
Also indicate if the parser band is a playback parser, as opposed to input
band parser.	

Returns:
    BERR_SUCCESS                     	
	BERR_INVALID_PARAMETER - Invalid PidChannelNum passed in.
See Also:
***************************************************************************/
BERR_Code BXPT_GetPidChannelConfig(
	BXPT_Handle hXpt, 	   		    /* [in] Handle for this transport */
	unsigned int PidChannelNum, 	/* [in] Which channel to get config for. */
	unsigned int *Pid, 				/* [out] The PID its using. */
	unsigned int *Band,				/* [out] The parser band the channel is mapped to. */
    bool *IsPlayback                /* [out] true if band is a playback parser, false if input */
    );

#if BXPT_HAS_FULL_PID_PARSER
/***************************************************************************
Summary:
Get a PID channel's Continuity Count checking configuration.

Description: 
Continuity Count handling can be controlled on a per-PID channel basis. Get 
the current settings for the given PID channel. See the BXPT_PidChannel_CC_Config 
structure definition for further details.  

Returns:
    BERR_SUCCESS                     	
    BERR_INVALID_PARAMETER - Invalid PidChannelNum passed in.
 
See Also: 
BXPT_SetPidChannel_CC_Config 
***************************************************************************/
BERR_Code BXPT_GetPidChannel_CC_Config(
	BXPT_Handle hXpt, 	   		    /* [in] Handle for this transport */
	unsigned int PidChannelNum, 	/* [in] Which channel to get config for. */
	BXPT_PidChannel_CC_Config *Cfg
    );

/***************************************************************************
Summary:
Set a PID channel's Continuity Count checking configuration.

Description: 
Continuity Count handling can be controlled on a per-PID channel basis. Give 
new settings for the given PID channel. See the BXPT_PidChannel_CC_Config 
structure definition for further details.  

Returns:
    BERR_SUCCESS                     	
    BERR_INVALID_PARAMETER - Invalid PidChannelNum passed in.
 
See Also: 
BXPT_GetPidChannel_CC_Config 
***************************************************************************/
BERR_Code BXPT_SetPidChannel_CC_Config(
	BXPT_Handle hXpt, 	   		    /* [in] Handle for this transport */
	unsigned int PidChannelNum, 	/* [in] Which channel to get config for. */
	const BXPT_PidChannel_CC_Config *Cfg
    );
#endif

#if BXPT_HAS_PID2BUF_MAPPING
/***************************************************************************
Summary:
Sets up the message module for PID to multiple buffer mapping. 

Description:
If you need to use the PID to multiple buffer mapping API's, then set SetPid2Buff=true.
Default is false, i.e original 1 to 1 mapping. 

NOTE: This call is not needed on the 7422 and later devices. 
***************************************************************************/
void BXPT_Mesg_SetPid2Buff(
        BXPT_Handle hXpt,	/* [in] Handle for this transport instance */
        bool SetPid2Buff	/* [in] Enable (if true) or disable (if false) */
    );

/***************************************************************************
Summary:
Use only if you need PID to multiple buffer mapping capability.
Retrieves the current PSI setting for the given pid channel.

Description:
Read the current settings for the given pid channel chip. Settings 
are returned in the structure that is passed in. See the structure for the
complete list of supported settings.

Returns:
    BERR_SUCCESS                - Copied the parser config.
    BERR_INVALID_PARAMETER      - Bad input parameter
 
See Also:
BXPT_SetPidChannelPsiSettings
****************************************************************************/
BERR_Code BXPT_Mesg_GetPidChannelPsiSettings(
        BXPT_Handle hXpt,                  /* [In] Handle for this transport */
        unsigned int PidChannelNum,        /* [In] The pid channel to configure. */
        unsigned int MesgBufferNum,        /* [In] The pid channel to configure. */
        bool *OverrideParserSettings,       /* [Out] true if these settings are overriding the parser versions */
        BXPT_PidPsiConfig *Config    /* [Out] Message mode setting for pid channel. */
        );

/***************************************************************************
Summary:
Use only if you need PID to multiple buffer mapping capability.
Override the PSI settings for the Pid channel or Reset the per pid channel PSI
settings to the paser band settings  

Description:
Writes the given PSI configuration to the pid channel or reset the PSI configuration 
to the paser band setting .Each parser band has its own PSI settings set by 
BXPT_SetParserConfig API. On the new chip architectures each pid channel can have its 
own PSI settings. This API lets the user change the PSI settings on per pid channel 
basis. User should un-override the setting when different PSI settings are no
longer desired 

Returns:
    BERR_SUCCESS                - Transfered the parser config to the chip.
    BERR_INVALID_PARAMETER      - Bad input parameter
 
See Also:
BXPT_GetParserConfig,BXPT_GetPidChannelPsiSettings
****************************************************************************/
BERR_Code BXPT_Mesg_SetPidChannelPsiSettings(
        BXPT_Handle hXpt,                  /* [In] Handle for this transport */
        unsigned int PidChannelNum,        /* [In] The pid channel to configure. */
        unsigned int MesgBufferNum,        /* [In] The Mesg Buffer NUmber to configure. */
        bool OverrideParserSettings,       /* [In] If set true the PSI settings for 
                                                   this pid channel will be changed to Config
                                                   else restored to the paser band to which 
                                                   this pid channel is allocated */
        const BXPT_PidPsiConfig *Config    /* [In] Message mode setting for pid channel. */
        );



/***************************************************************************
Summary:
This function is Added as a substitiute for BXPT_SetPidChannelBuffer to support 
PID to multiple buffer mapping capabilities. User has to specify the Message Buffer Number along 
with the PidChannelNumber.
Sets up a region of memory to be used as a PID channel buffer. 

Description:
Setup a region of memory to be used as a PID channel's message buffer. The 
caller specifies the size (in bytes) of the memory region. The caller may 
allocate the memory to be used; in that case, a pointer to the user-allocated 
region would be passed in through the CpuAddr param. If CpuAddr is NULL, 
this function will do the allocation internally, using BufferSize as the 
size to allocate. User-allocated buffers must be 1024-byte aligned.

Note that the if the caller allocates memory, the caller is also responsible 
for freeing it. Memory allocated internally by this call will be freed when the 
transport handle is closed, or when it is called again with a different 
BufferSize and a NULL CpuAddr. 

Also, if the caller allocates memory, the size of the area must match one of
the sizes defined by the BXPT_MessageBufferSize enumeration. These are the 
only buffer sizes supported by the underlying hardware. User allocated memory
must be obtained through the BMEM memory manager, preferably by using 
BMEM_AllocAligned().

Returns:
    BERR_SUCCESS                - Message buffer configured.
    BERR_INVALID_PARAMETER      - Bad input parameter
***************************************************************************/
BERR_Code BXPT_Mesg_SetPidChannelBuffer( 
    BXPT_Handle hXpt,                   /* [in] Handle for this transport */
    unsigned int PidChannelNum,             /* [in] Which PID channel buffer we want. */
    unsigned int MesgBufferNum,             /* [in] Which Buffer number we want . */
    void *CpuAddr,                  /* [in] Caller allocated memory, or NULL. */
    BXPT_MessageBufferSize BufferSize   /* [in] Size, in bytes, of the buffer. */
    );


/***************************************************************************
Summary:
Use this API only when you need PID to multiple buffer mapping capabilities.
Pass Message Buffer Number along with the PidChannelNumber.
Add a filter to a group of filters. 

Description:
Add a given filter to the group of filters maintained in the Settings 
structure. All filters in the group will be used to filter messages carried
on the PID channel the group is associated with.Pid channel must be allocated 
and configured before calling this function.

Returns:
    BERR_SUCCESS                - Filter successfully added to the group.
    BERR_INVALID_PARAMETER      - Bad input parameter

See Also:
BXPT_AllocPSIFilter, BXPT_GetFilter, BXPT_SetFilter, 
BXPT_RemoveFilterFromGroup
***************************************************************************/
BERR_Code BXPT_Mesg_AddFilterToGroup(
    BXPT_Handle hXpt,                   /* [in] Handle for this transport */
    unsigned int PidChannelNum,         /* [in] Pid channel number */
    unsigned int MesgBufferNum,         /* [in] Message Buffer number */
    unsigned int FilterNum,             /* [in] Which filter to add. */
    BXPT_PsiMessageSettings *Settings   /* [in,out] Filter group to add to. */
    );


/***************************************************************************
Summary:
Use only if you need PID to multiple buffer mapping capability.
Configure for recording a PID channel.

Description:
Configure for recording a single PID channel, using the message buffers. 
The PID, parser band, and type of recording to do are specified in the 
Settings structure. This function will enable the PID channel, but will
not install an interrupt handler for the message interrupt. Installing
the interrupt should be done before calling this function.

The caller must allocate a PID channel before using the function. It should
NOT be called if the PID channel is already enabled.

BXPT_SetPidChannelBuffer() must have been called at some point prior to 
calling this function, to insure that the message buffer for this channel
has been initialized. 

Returns:
    BERR_SUCCESS                - PID channel record configured.
    BERR_INVALID_PARAMETER      - Bad input parameter
    BXPT_ERR_MESG_BUFFER_NOT_CONFIGURED - Message buffer not initialized.

***************************************************************************/
BERR_Code BXPT_Mesg_StartPidChannelRecord( 
    BXPT_Handle hXpt,                               /* [in] Handle for this transport */
    unsigned int PidChannelNum,                         /* [in] Which PID channel. */
    unsigned int MesgBufferNum,                         /* [in] Which Buffer Number. */
    const BXPT_PidChannelRecordSettings *Settings   /* [in] Record settings. */
    );


/***************************************************************************
Summary:
Use Only when you need PID to multiple buffer mapping capabilty 
Disable recording of a PID channel.

Description:
Stops the recording of packet or PES data on the given PID channel. The PID
channel is disabled. The interrupt handler, if there was one installed, will
not be disabled; thus, any data still in the hardware may trigger an interrupt
after this function returns.

NOTE: This function may sleep for up to 200 microseconds, in order to flush 
any remaining data from the hardware's internal buffers.

Returns:
    BERR_SUCCESS                - PID channel record stopped.
    BERR_INVALID_PARAMETER      - Bad input parameter

***************************************************************************/
BERR_Code BXPT_Mesg_StopPidChannelRecord( 
    BXPT_Handle hXpt,                               /* [in] Handle for this transport */
    unsigned int PidChannelNum,                      /* [in] Which PID channel. */
    unsigned int MesgBufferNum                      /* [in] Which Buffer Number. */
    );


/***************************************************************************
Summary:
Use only when you need PID to multiple buffer mapping capability.
Configure for capturing PSI messages.

Description:
Associate one or more PSI message filters with a given PID and parser band.
This function will enable the PID channel, but will not install an 
interrupt handler for the message interrupt. The interrupt handler should be
installed before calling this function.

The caller must allocate a PID channel before using the function. It should
NOT be called if the PID channel is already enabled.

BXPT_SetPidChannelBuffer() must have been called at some point prior to 
calling this function, to insure that the message buffer for this channel
has been initialized. 

Returns:
    BERR_SUCCESS                - PSI filtering is setup.
    BERR_INVALID_PARAMETER      - Bad input parameter
    BXPT_ERR_MESG_BUFFER_NOT_CONFIGURED - Message buffer not initialized.

***************************************************************************/
BERR_Code BXPT_Mesg_StartPsiMessageCapture( 
    BXPT_Handle hXpt,                           /* [in] Handle for this transport */
    unsigned int PidChannelNum,                     /* [in] Which PID channel. */
    unsigned int MesgBufferNum,                     /* [in] Which Message Buffer. */
    const BXPT_PsiMessageSettings *Settings     /* [in] PID, band, and filters to use. */
    );


/***************************************************************************
Summary:
Use only when you need PID to multiple buffer mapping capability.
Stop capturing PSI messages.

Description:
Disable the PID channel being used to capture PSI messages. Flush any data
still in the chip. If the interrupts for the message buffer are still 
enabled, flushing the data may trigger another interrupt.

NOTE: This function may sleep for up to 200 microseconds, in order to flush 
any remaining data from the hardware's internal buffers.

Returns:
    BERR_SUCCESS                - PSI filtering is stopped.
    BERR_INVALID_PARAMETER      - Bad input parameter

***************************************************************************/
BERR_Code BXPT_Mesg_StopPsiMessageCapture( 
    BXPT_Handle hXpt,                           /* [in] Handle for this transport */
    unsigned int PidChannelNum,                  /* [in] Which PID channel. */
    unsigned int MesgBufferNum                  /* [in] Which PID channel. */
    );



/***************************************************************************
Summary:
 API in bxpt_mesg.c to disable the pid channel to message buffer association 
 that is created after calling the ConfigPid2BufferMap

Description:
 API in bxpt_mesg.c to disable the pid channel to message buffer association 
 that is created after calling the ConfigPid2BufferMap

Returns:
    BERR_SUCCESS                - PSI filtering is stopped.
    BERR_INVALID_PARAMETER      - Bad input parameter

***************************************************************************/
BERR_Code BXPT_Mesg_ClearPidChannelBuffer( 
    BXPT_Handle hXpt,                   /* [in] Handle for this transport */
    unsigned int PidChannelNum,             /* [in] Which PID channel buffer we want. */
    unsigned int MesgBufferNum             /* [in] Which Buffer number we want . */
	);
	
#endif

#if BXPT_HAS_RSBUF
/***************************************************************************
Summary:
Determine if there is data present on the parser band used by the given
PID channel.

Description:
IsDataPresent will be true if there is data on the parser band used by
the given PID channel, false otherwise. Note that if data is present, it
may not be the particular PID that the channel is configured for.

Returns:
    BERR_SUCCESS                - Got data present status from hw.
    BERR_INVALID_PARAMETER      - Bad input parameter

***************************************************************************/
BERR_Code BXPT_IsDataPresent(
	BXPT_Handle hXpt, 	   		    /* [in] Handle for this transport */
	unsigned int PidChannelNum, 	/* [in] Which channel to get status for. */
    bool *IsDataPresent			/* [out] true if data is present */
    );
#endif

#if BXPT_HAS_PIPELINE_ERROR_REPORTING
/***************************************************************************
Summary:
Check for RS and XC buffer overflows. If any are found, print a user-friendly
message through BDBG_ERR().

Returns:
    BERR_SUCCESS - 				No error detected.
	BERR_NOT_SUPPORTED -		Not supported on the current chip. 
	BXPT_ERR_DATA_PIPELINE -	Error was detected
***************************************************************************/
BERR_Code BXPT_CheckPipelineErrors(
	BXPT_Handle xpt
    );
#endif

/*
** These functions are called internally. 
** Users should NOT uses these functions directly.
*/

/* Indentify PB parser bands by special bitfields. Also clear those bitfields */
#define BXPT_P_IS_PB( x )               (( x & 0x8000 ) != 0)
#define BXPT_P_CLEAR_PB_FLAG( x )       ( x &= 0x7FFF )
#define BXPT_P_GET_PB_BAND_NUM( x )     ( x & 0x7FFF )

BERR_Code BXPT_P_ApplyParserPsiSettings(
    BXPT_Handle hXpt,                     
    unsigned int ParserNum,
    bool IsPbParser             
    );

BERR_Code BXPT_P_SetPidChannelDestination( 
    BXPT_Handle hXpt, 
    unsigned int PidChannelNum, 
    unsigned Destination,
    bool EnableIt 
    );

BERR_Code BXPT_P_ClearAllPidChannelDestinations( 
    BXPT_Handle hXpt,
    unsigned int PidChannelNum
    );

BERR_Code BXPT_P_DisableFilter( 
    BXPT_Handle hXpt, 
    unsigned int FilterNum, 
    unsigned int PidChannelNum 
    );

BERR_Code BXPT_P_PauseFilters( 
    BXPT_Handle hXpt, 
    unsigned int PidChannelNum, 
    unsigned FilteringOp, 
    bool Pause 
    );

BERR_Code BXPT_P_EnablePidChannel(
    BXPT_Handle hXpt,               
    unsigned int PidChannelNum          
    );

bool BXPT_P_CanPowerDown( 
	BXPT_Handle hXpt
	);

BERR_Code BXPT_P_AllocSharedXcRsBuffer(
	BXPT_Handle hXpt
	);

int BXPT_P_GetParserRegOffset(
	int parserIndex
	);

#if BXPT_HAS_MTSIF
bool BXPT_IsMtsifDecryptionEnabled(
    BXPT_Handle hXpt,
    unsigned channelNo
    );
#endif

#ifdef __cplusplus
}
#endif

#endif /* #ifndef BXPT_H__ */

/* end of file */


