/***************************************************************************
*     (c)2004-2012 Broadcom Corporation
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
*  Except as expressly set forth in the Authorized License,
*
*  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
*  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
*  and to use this information only in connection with your use of Broadcom integrated circuit products.
*
*  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
*  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
*  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
*  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
*  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
*  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
*  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
*  USE OR PERFORMANCE OF THE SOFTWARE.
*
*  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
*  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
*  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
*  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
*  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
*  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
*  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
*  ANY LIMITED REMEDY.
*
* $brcm_Workfile: nexus_frontend_module.h $
* $brcm_Revision: 111 $
* $brcm_Date: 10/25/12 6:09p $
*
* API Description:
*   API name: Frontend Module
*    Frontend module private APIs and module data.
*
* Revision History:
*
* $brcm_Log: X:/win_views/97231/nexus/modules/frontend/common/src/nexus_frontend_module.h $
*
* 111   10/25/12 6:09p vishk
* SW3461-206: Add support to check the tuner application of parent
*  devices.
*
* 110   10/25/12 12:42p vishk
* SW3461-206: Add external fixed gain settings.
*
* 109   10/24/12 4:05p mphillip
* SW7425-4035: Merge 4538 support branch to main
*
* SW7425-4035/1   10/4/12 5:19p mphillip
* SW7425-4035: Initial 4538 support
*
* 108   10/19/12 5:48p jtna
* SW7425-4037: add NEXUS_Frontend_TransmitDebugPacket
*
* 107   10/18/12 9:07p vishk
* SW3461-206: Add front end configuration settings for external, fixed-
*  gain amp (FGA) and optional FGA-bypass
*
* 106   10/8/12 2:52p erickson
* SW7435-372: add NEXUS_Frontend_ReadSoftDecisions
*
* 105   10/8/12 1:03p erickson
* SW7425-3782: add NEXUS_FrontendConnector for frontend -> transport
*  connections
*
* 104   10/4/12 2:34p jtna
* SW7425-2752: make NEXUS_MAX_MTSIF_CONFIG dependent on
*  BMXT_MAX_NUM_PARSERS
*
* 103   10/3/12 5:00p jtna
* SW7425-3782: refactor nexus tsmf impl
*
* 102   9/5/12 6:09p jtna
* SW7425-3782: rework host and demod TSMF impl. both cases now go through
*  the host API
*
* 101   8/3/12 2:27p vishk
* SW7346-848: Merge to mainline.
*
* SW7346-848/1   7/23/12 3:32p vishk
* SW7346-848: Add support for BCM3472 BCM97346I2SFF platform.
*
* 100   8/2/12 10:34a erickson
* SW7429-227: support NEXUS_NUM_PARSER_BANDS == 0
*
* 99   7/26/12 3:09p jtna
* SW7425-3514: add frontend GetTemperature APIs
*
* 98   7/25/12 10:10a gmohile
* SW7425-3519,SW7425-3520,SW7425-3077: Add private standby function
*
* 97   7/18/12 6:24p jtna
* SW7346-928: add support for MTSIF frontend<->PB mapping with frontend
*  daisy-chain
*
* 96   7/5/12 3:42p vishk
* SW3462-12: Use 3461 api for 3462.
*
* 95   7/5/12 12:10p vishk
* SW3462-12: Enable DVB-C2 video demo by Jun 12 on BCM97241C2SFF board
*
* 94   6/26/12 3:34p jtna
* SW7425-3062: add feature to propagate host parserband settings to
*  demod. deprecate NEXUS_Frontend_Get/SetTransportSettings
*
* 93   6/21/12 5:26p mphillip
* SW7358-304: Merge tone search to main
*
* SW7358-304/1   6/20/12 6:32p mphillip
* SW7358-304: Implement tone search
*
* 92   6/20/12 4:22p erickson
* SW7420-2078: add NEXUS_Frontend_Acquire/Release
*
* 91   6/19/12 7:06p vsilyaev
* SW7420-2085: Always declare class for NEXUS_Amplifier
*
* 90   6/19/12 6:46p vsilyaev
* SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
*
* 89   6/19/12 1:27p vsilyaev
* SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
*
* 88   6/19/12 12:42p vsilyaev
* SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
*
* 87   6/8/12 3:27p jtna
* SW7425-3171: add NEXUS_Frontend_ReapplyTransportSettings
*
* 86   5/30/12 2:31p jtna
* SW7425-3018: remove old, unused DTV code
*
* 85   5/15/12 5:44p jtna
* SW7425-2752: change from *config to *pConfig
*
* 84   5/15/12 3:00p vishk
* SW7552-269, SW3128-155, SW3461-198: Add NEXUS_Frontend_WriteRegister()
*  and NEXUS_Frontend_ReadRegister().
*
* 83   5/10/12 7:42p jtna
* SW7425-2752: make new struct and functions MXT-agnostic
*
* 82   5/10/12 6:52p jtna
* SW7425-2752: rework MTSIF-frontend <-> host interface
*
* 81   5/7/12 6:14p vishk
* SW3462-12: Enable DVB-C2 video demo by Jun 12 on BCM97241C2SFF board
*
* 80   1/25/12 3:10p vishk
* SW3461-51: Implement asynchronous nexus apis for retrieval of partial
*  DVBT2 status.
*
* 3461_PARTIAL_STATUS/2   1/25/12 3:05p vishk
* SW3461-51: Implement asynchronous nexus apis for retrieval of partial
*  DVBT2 status. Cleanup of enum and api names.
*
* 3461_PARTIAL_STATUS/1   1/23/12 4:12p vishk
* SW3461-51: Implement asynchronous nexus apis for retrieval of partial
*  DVBT2 status.
*
* 79   1/18/12 12:22p vishk
* SW3128-99: Add nexus support for spectrum data retreival.
*
* 78   1/6/12 4:44p vishk
* SW7425-1786: all chip-specific frontend API's must validate against an
*  always-available chip id before dereferencing chip-specific data
*  structures
*
* 3128_SPECTRUM_DATA/2   1/18/12 10:45a vishk
* SW3128-99: Add nexus support for spectrum data retreival.
*
* 3128_SPECTRUM_DATA/1   12/22/11 4:57p vishk
* SW3128-99:Add nexus support for spectrum data retreival.
*
* 77   11/17/11 4:43p vishk
* SW3461-96: Remove GetLockStatus and replace it with GetFastStatus
*
* 76   11/1/11 5:04p mphillip
* SW7425-1437: Merge 4528 to main
*
* 75   11/1/11 3:09p vishk
* SW3461-55: Means of returning BCM3461 chip revsion and firmware
*  revision via NEXUS i/f
*
* 74   10/12/11 11:55a vishk
* SW3461-50, SW3128-16: add qam scan
*
* 73   10/11/11 6:52p vishk
* SW3128-46: add support for BCM3124 devices
*
* 72   10/7/11 2:30p vishk
* SW3128-47: Add Support for BCM3123
*
* 71   10/6/11 5:22p jtna
* SW7425-1327: add NEXUS_Frontend_Get/SetTransportSettings()
*
* 70   10/6/11 4:50p jtna
* SW7425-1189: change return type to void. make tsmfSettings per frontend
*  handle, not per device.
*
* 69   10/5/11 12:24p jtna
* SW7425-1189: add transport and frontend TSMF support
*
* 68   9/26/11 2:05p vishk
* SW3461-53: 3461 interrupt line 'stuck low' after multiple tune events.
*  Remove unsupported api.
*
* 67   9/21/11 5:43p vishk
* SW3128-16: Add scan support to 3128
*
* 66   9/7/11 5:54p vishk
* SW3461-36: add DVB T2 status API. Fix compilation flag.
*
* 65   9/7/11 5:36p vishk
* SW3461-36: add DVB T2 status API.
*
* SATIP_7425_4528/1   7/11/11 12:02p vishk
* SWSATFE-132: Add nexus support for 4528 satellite frontend for 97425
*  SATIPSW platform.
*
* 64   6/23/11 5:47p vishk
* SW7346-165: NEXUS Frontend changes for multi-tuner board are not
*  platform independent
*
* 63   6/9/11 11:27a shyi
* SWDTV-7365: Added Sharp VA1E1CD2406 tuner support to Nexus
*
* 62   6/8/11 2:14p vishk
* SW7552-35: Add 7552 Nexus frontend support. External Tuner support has
*  been removed as there will be no use case for it. Also, Scan features
*  have been disabled for now.
*
* 61   5/27/11 8:54p jtna
* SW7552-35: merge 7552 frontend work
*
* SW7552-35/1   5/26/11 7:46p jtna
* SW7552-35: add 7552
*
* 60   5/24/11 7:05p vishk
* SW7231-32: Implement the Nexus code to support the 3461 frontend chip
*
* 59   4/25/11 3:59p shyi
* SWDTV-6735: Added Nutune FK1605 tuner support
*
* 58   4/15/11 3:45p shyi
* SWDTV-6597: Added Silicon Labs Si2153 tuner support
*
* 57   3/29/11 2:59p shyi
* SWDTV-6189: Added NEXUS frontend into 35233 build
*
* 56   3/17/11 1:12p vishk
* SW7231-32: Implement the Nexus code to support the 3461 frontend chip
*
* 55   3/10/11 3:28p shyi
* SWDTV-5866: Added SEMco DTVA50CVH1019 tuner support
*
* 54   3/9/11 1:56p erickson
* SW7422-171: fix 3420
*
* 53   3/4/11 3:04p jgarrett
* SW7422-171: Adding support for 3128 daughtercard
*
* 52   3/4/11 3:43p shyi
* SWDTV-5407: Added the linkage between reading RF input power from a
*  supported tuner and passing the read value to IFD
*
* 51   1/6/11 7:31p shyi
* SW35230-2441: Made channel scanning a platform specific feature
*
* 50   12/9/10 11:02a shyi
* SW35230-2520: Added LG Innotek TDTK-H801F tuner support
*
* 49   12/1/10 8:32p shyi
* SW35230-2396: Added Nutune FA2328/31/32/33 tuner support
*
* 48   11/29/10 10:56a mward
* SW7125-729:  Add header for NEXUS_FRONTEND_3412.
*
* SW7125-729/1   11/10/10 5:39p mward
* SW7125-729:  Add header for NEXUS_FRONTEND_3412.
*
* 47   10/28/10 4:26p VISHK
* SW7420-1020: Request for non-blocking call support in Nexus frontend
*
* 46   10/21/10 6:02p VISHK
* SW7420-1020: Add asynchronous status API
*
* 45   10/21/10 6:00p VISHK
* SW7420-1020: Add asynchronous status API
*
* 44   10/21/10 5:55p mphillip
* SW7420-1020: Add asynchronous status API
*
* 43   10/14/10 11:24a shyi
* SW35230-1738: Added NXP TD1636F tuner support to NEXUS layer
* SW35230-1605: Added Sharp VA1E1ED2411 tuner support to NEXUS layer
* SW35230-1469: Added Panasonic ENGS6301D5F tuner support to NEXUS layer
* SW35230-1423: Added Panasonic ET-29 tuner support to NEXUS layer
*
* 42   8/26/10 12:00p jgarrett
* SW35230-900: Merging new tuner interfaces to main branch
*
* 41   7/26/10 10:11a mward
* SW7125-513:  Add NEXUS_FRONTEND_7125, default y, for host control of
*  7125 tuner/ds.
*
* SW7125-513/1   7/12/10 4:42p mward
* SW7125-513:  Add NEXUS_FRONTEND_7125, default y, for host control of
*  7125 tuner/ds.
*
* SW35230-900/4   8/20/10 4:43p shyi
* SW35230-900: Adding NXP TDA18273 tuner
* SW35230-1049: Adding NXP TDA18273 tuner support at NEXUS layer
*
* SW35230-900/3   8/18/10 3:37p shyi
* SW35230-900: Re-arranging build configuration and correcting function
*  header comments
*
* SW35230-900/2   8/18/10 11:24a shyi
* SW35230-900: Adding NULL tuner
*
* SW35230-900/1   8/12/10 8:24p shyi
* SW35230-900: Taking TFE tuner changes for I2C read and firmware
*  download capabilities
*
* 40   7/19/10 1:09p shyi
* SW35230-49: Merging to main branch
*
* 39   2/25/10 6:20p mward
* SW7400-2673: Implement support for BCM4500.
*
* SW7400-2673/1   2/22/10 1:45p mward
* SW7400-2673:  Support BCM4500.
*
* SW35230-49/1   2/5/10 6:42p blovre
* SW35230-49: making Nexus Frontend buildable 1'st ver. (inbranch)
*
* 38   11/6/09 5:06p jgarrett
* SW7550-48: Adding 7550 Frontend Support
*
* 37   8/20/09 4:48p erickson
* PR57770: convert FTM extension into more generic customer extension
*
* 36   7/21/09 9:01a erickson
* PR56765: add ftm state and uninit
*
* 35   7/20/09 5:11p erickson
* PR56765: enable separate NEXUS_FRONTEND_FTM_EXTENSION_INC extension
*
* 34   7/14/09 3:47p erickson
* PR56789: remove NEXUS_Frontend_SetPowerState. unimplemented and unused.
*
* 33   7/13/09 4:47p erickson
* PR56773: added NEXUS_FrontendDiseqcStatus
*
* 32   7/8/09 3:13p erickson
* PR56607: remove unused NEXUS_Frontend_P_Register function
*
* 31   7/8/09 6:53a erickson
* PR56607: add optional registerExtension operator so that callbacks can
*  be recreated with the handle that the user gets. this allows
*  NEXUS_StopCallbacks to work.
*
* 30   6/24/09 9:37a erickson
* PR53078: add support for frontend handle extensions in 73xx public API
*  implementations
*
* PR54416/2   5/21/09 6:03p jtna
* PR54416: improve naming
*
* PR54416/1   5/15/09 7:48p jtna
* PR54416: nexus support for satellite scanning
*
* 28   5/11/09 4:29p erickson
* PR53078: get time of tune or resetStatus so that elapsed time can be
*  returned with status
*
* 27   4/2/09 2:29p erickson
* PR53078: satellite api additions
*
* 26   3/11/09 7:22p vishk
* PR 46431: Add support for BCM3117, BCM3114, and BCM3112
*
* 25   2/9/09 5:04p jgarrett
* PR 50837: Merge to main branch
*
* PR50837/1   1/29/09 10:37a jgarrett
* PR 50837: Renaming 7325 to 73xx
*
* 24   1/26/09 11:29a erickson
* PR51468: global variable naming convention
*
* 23   1/23/09 3:32p jgarrett
* PR 51457: Adding frontend extension hooks for tuners and
*  frontends/demods
*
* 22   1/16/09 7:40p vishk
* PR46436: AOB PI: Add support for BCM3117 and BCM3114
*
* 21   12/22/08 9:48p agin
* PR49982: Add reset diseqc.
*
* 20   12/11/08 4:07p erickson
* PR49556: separate frontend power management into analog and digital,
*  guarantee correct refcnt
*
* 19   10/2/08 11:27a erickson
* PR40799: add optional untune function pointer
*
* 18   8/8/08 6:59p jgarrett
* PR 45171: Adding AASD hooks
*
* 17   7/22/08 7:05p anilmm
* PR44830: Add 3255 frontend
*
* 16   5/1/08 11:49a erickson
* PR42376: add prototype
*
* 15   5/1/08 11:27a jgarrett
* PR 41689: PR 41690: Adding dtt76800 and dtt75409 tuners
*
* 14   4/29/08 5:59p jrubio
* PR42336: add 7325 tuner code
*
* 13   4/21/08 1:08p jgarrett
* PR41793: PR41793: Frontend: Add Thomson tuner Dtt7045 driver to support
*  8M tuner
*
* PR41793/1   4/21/08 3:05p pinglong
* PR41793: Frontend: Add Thomson tuner Dtt7045 driver to support 8M tuner
*
* 12   4/8/08 1:53p erickson
* PR39453: add 3548 & 3556
*
* 11   3/31/08 1:13p erickson
* PR41075: add BDBG_OBJECT
*
* 10   3/14/08 1:33p jgarrett
* PR 40111: Fixing prototype
*
* 9   3/14/08 1:31p jgarrett
* PR 40111: Adding analog agc settings
*
* 8   3/14/08 12:55p jgarrett
* PR 40108: Adding NEXUS_Frontend_ScanFrequency
*
* 7   3/14/08 11:31a jgarrett
* PR 40109: Adding NEXUS_Frontend_ResetStatus
*
* 6   3/13/08 7:41p jgarrett
* PR 39985: Adding AST support
*
* 5   3/12/08 10:25a erickson
* PR39584: added BDBG_OBJECT_ASSERT's
*
* 4   3/4/08 9:48a jgarrett
* PR 39984: Adding 3510/3420 support
*
* 3   2/8/08 6:36p jgarrett
* PR 39408: Adding daughtercard support
*
* 2   2/7/08 5:36p jgarrett
* PR 39407: Converting to common close functions
*
* 1   1/18/08 2:19p jgarrett
* PR 38808: Merging to main branch
*
* Nexus_Devel/4   12/20/07 2:30p jgarrett
* PR 38019: Adding GetAnalogAudioConnector
*
* Nexus_Devel/3   11/20/07 6:52p jgarrett
* PR 37130: Unifying 3563 frontend and platform
*
* Nexus_Devel/2   10/10/07 11:29a jgarrett
* PR 35551: Adding conditional includes for frontend chips
*
* Nexus_Devel/1   10/5/07 5:52p jgarrett
* PR 35551: Adding initial version
*
***************************************************************************/
#ifndef NEXUS_FRONTEND_MODULE_H__
#define NEXUS_FRONTEND_MODULE_H__

#include "nexus_frontend_thunks.h"
#include "nexus_frontend_init.h"

#include "nexus_base.h"

#include "priv/nexus_i2c_priv.h"
#include "priv/nexus_spi_priv.h"

#include "nexus_frontend.h"
#include "nexus_tuner.h"
#include "nexus_power_management.h"
#include "priv/nexus_core.h"
#include "priv/nexus_core_audio.h"
#include "priv/nexus_core_video.h"
#include "blst_squeue.h"
#include "nexus_frontend_extension_api.h"
#include "priv/nexus_frontend_standby_priv.h"
#if NEXUS_HAS_MXT
#include "bmxt.h"
#endif

#ifdef NEXUS_FRONTEND_2940
#include "nexus_frontend_2940.h"
#endif
#ifdef NEXUS_FRONTEND_3510
#include "nexus_frontend_3510.h"
#endif
#ifdef NEXUS_FRONTEND_3520
#include "nexus_frontend_3520.h"
#endif
#ifdef NEXUS_FRONTEND_DTT760X
#include "nexus_tuner_dtt760x.h"
#endif
#ifdef NEXUS_FRONTEND_DTT7045
#include "nexus_tuner_dtt7045.h"
#endif
#ifdef NEXUS_FRONTEND_DTT76800
#include "nexus_tuner_dtt76800.h"
#endif
#ifdef NEXUS_FRONTEND_DTT75409
#include "nexus_tuner_dtt75409.h"
#endif
#ifdef NEXUS_FRONTEND_3418
#include "nexus_tuner_3418.h"
#endif
#ifdef NEXUS_FRONTEND_3420
#include "nexus_tuner_3420.h"
#endif
#ifdef NEXUS_FRONTEND_3440
#include "nexus_tuner_3440.h"
#endif
#ifdef NEXUS_FRONTEND_3412
#include "nexus_amplifier_3412.h"
#endif
#ifdef NEXUS_FRONTEND_3431
#include "nexus_amplifier_3431.h"
#endif
#ifdef NEXUS_FRONTEND_4500
#include "nexus_frontend_4500.h"
#endif
#ifdef NEXUS_FRONTEND_4501
#include "nexus_frontend_4501.h"
#endif
#ifdef NEXUS_FRONTEND_4506
#include "nexus_frontend_4506.h"
#endif
#ifdef NEXUS_FRONTEND_4528
#include "nexus_frontend_4528.h"
#endif
#ifdef NEXUS_FRONTEND_4538
#include "nexus_frontend_4538.h"
#endif
#ifdef NEXUS_FRONTEND_45216
#include "nexus_frontend_45216.h"
#endif
#ifdef NEXUS_FRONTEND_7550
#include "nexus_frontend_7550.h"
#include "nexus_tuner_7550.h"
#endif
#ifdef NEXUS_FRONTEND_7552
#include "nexus_frontend_7552.h"
#endif
#ifdef NEXUS_FRONTEND_73XX
#include "nexus_frontend_73xx.h"
#endif
#ifdef NEXUS_FRONTEND_7346
#include "nexus_frontend_7346.h"
#endif
#ifdef NEXUS_FRONTEND_3255
#include "nexus_frontend_3255.h"
#endif
#ifdef NEXUS_FRONTEND_3117
#include "nexus_frontend_31xx.h"
#endif
#ifdef NEXUS_FRONTEND_3114
#include "nexus_frontend_31xx.h"
#endif
#ifdef NEXUS_FRONTEND_3112
#include "nexus_frontend_31xx.h"
#endif
#ifdef NEXUS_FRONTEND_3109
#include "nexus_frontend_31xx.h"
#endif
#ifdef NEXUS_FRONTEND_3128
#include "nexus_frontend_3128.h"
#endif
#ifdef NEXUS_FRONTEND_3123
#include "nexus_frontend_3128.h"
#endif
#ifdef NEXUS_FRONTEND_3124
#include "nexus_frontend_3128.h"
#endif
#ifdef NEXUS_FRONTEND_3461
#include "nexus_frontend_3461.h"
#endif
#ifdef NEXUS_FRONTEND_3472
#include "nexus_frontend_3461.h"
#endif
#ifdef NEXUS_FRONTEND_3462
#include "nexus_frontend_3461.h"
#endif
#ifdef NEXUS_FRONTEND_7125
#include "nexus_frontend_7125.h"
#include "nexus_tuner_7125.h"
#endif
#ifdef NEXUS_FRONTEND_SCAN_SUPPORTED
#include "nexus_frontend_scan.h"
#endif

#ifdef NEXUS_FRONTEND_EXTENSION
/* This is for NEXUS_FrontendHandle inheritence used to integrated board-specific logic
into the frontend module. */
#include "nexus_frontend_extension.h"
#endif

#ifdef NEXUS_FRONTEND_CUSTOMER_EXTENSION
/* This is for customer's to add customer API's into the Frontend module. This is the
normal meaning of the term "extension." */
#include "nexus_frontend_customer_extension.h"

/* this function must exist in the extension to uninit the customerData hook */
extern void NEXUS_Frontend_P_UninitExtension( NEXUS_FrontendHandle handle );
#endif

#include "priv/nexus_frontend_analog_priv.h"
#include "priv/nexus_tuner_priv.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef NEXUS_MODULE_SELF
#error Cant be in two modules at the same time
#endif

#define NEXUS_MODULE_NAME frontend
#define NEXUS_MODULE_SELF g_NEXUS_frontendModule

/***************************************************************************
 * Generic Amplifier Handle
 ***************************************************************************/
#if NEXUS_AMPLIFIER_SUPPORT
#include "blna.h"
typedef struct NEXUS_Amplifier
{
    NEXUS_OBJECT(NEXUS_Amplifier);
    BLNA_Handle lnaHandle;
    NEXUS_AmplifierSettings settings;
} NEXUS_Amplifier;
#else
typedef struct NEXUS_Amplifier
{
    NEXUS_OBJECT(NEXUS_Amplifier);
} NEXUS_Amplifier;
#endif
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_Amplifier);

/***************************************************************************
 * Frontend Device structures and private api.
 ***************************************************************************/
typedef struct NEXUS_InternalGainSettings
{
    unsigned frequency;                  /* frequency */
    int16_t  loopThroughGain;            /* Internal Gain to Loop Through.  In 1/100th of a dB. */
    int16_t  daisyGain;                  /* Internal Gain to Daisy.  In 1/100th of a dB. */
    unsigned totalVariableGain;          /* Total platform variable gain ahead of the current frontend.  In 1/100th of a dB. */
    bool     isExternalFixedGainBypassed;/* This flag indicates whether the external fixed gain of the parent device is bypassed or not */
} NEXUS_InternalGainSettings;

typedef struct NEXUS_ExternalGainSettings
{
    int16_t bypassableGain; /* External fixed bypassable gain. */
    int16_t totalGain;      /* External gain total. This also includes all the fixed gains before this device. */
} NEXUS_ExternalGainSettings;

typedef struct NEXUS_GainParameters {
    unsigned frequency; /* As the gain is frequency dependent. */
    NEXUS_FrontendDeviceRfInput rfInput;
    bool accumulateTillRootDevice;
}NEXUS_GainParameters;

typedef enum NEXUS_FrontendDeviceApplication
{
  NEXUS_FrontendDeviceApplication_eUnset,
  NEXUS_FrontendDeviceApplication_eCable,
  NEXUS_FrontendDeviceApplication_eTerrestrial,
  NEXUS_FrontendDeviceApplication_eSatellite,
  NEXUS_FrontendDeviceApplication_eLast
} NEXUS_FrontendDeviceApplication;

/* common struct to cache per-frontend device transport settings.
we must cache because demod register access can be slow, and we have to make a lot of them at Tune-time */
#if NEXUS_HAS_MXT
#define NEXUS_MAX_MTSIF_CONFIG BMXT_MAX_NUM_PARSERS
#else
#define NEXUS_MAX_MTSIF_CONFIG 1
#endif
typedef struct NEXUS_FrontendDeviceMtsifConfig {
#if NEXUS_HAS_MXT
    BMXT_Handle mxt;
#endif

    struct { /* cached demod settings */
        /* for data routing */
        bool enabled;
        unsigned inputBandNum;
        unsigned virtualNum;
        NEXUS_FrontendConnectorHandle connector;

        /* other settings propagated from host PB */
        bool errorInputIgnore;
        unsigned timestampMode;
        bool dssMode;
    } demodPbSettings[NEXUS_MAX_MTSIF_CONFIG];

    unsigned numDemodIb, numDemodPb; /* actual number of demod IB/PB, known at runtime */
    NEXUS_TimerHandle timer;
    bool slave; /* slave (TX-only) in a daisy-chain */
} NEXUS_FrontendDeviceMtsifConfig;

typedef struct NEXUS_FrontendHostMtsifConfig {
#if NEXUS_NUM_PARSER_BANDS
    struct {
        unsigned demodPb; /* PB number used on demod */
        bool connected; /* if connection specified in NEXUS_ParserBandSettings has been established (via tune) */
    } hostPbSettings[NEXUS_NUM_PARSER_BANDS];
#else
    unsigned unused;
#endif
} NEXUS_FrontendHostMtsifConfig;

typedef struct NEXUS_FrontendDevice {
    BLST_D_ENTRY(NEXUS_FrontendDevice) link;
    BLST_D_HEAD(deviceChildList, NEXUS_FrontendDevice) deviceChildList;

    unsigned familyId; /* Chip's family Id. In hex, e.g. 0x3128. */
    void *pDevice;     /* Chip specific device handle. */
    struct NEXUS_FrontendDevice *parent;
    NEXUS_FrontendDeviceLinkSettings linkSettings;
    int16_t bypassableFixedGain;                 /* External fixed bypassable gain. */
    int16_t totalFixedBoardGain;                 /* Total external fixed gain. This also includes all the fixed gains(bypassable and not bypassable) before this device. */
	NEXUS_FrontendDeviceApplication application; /* Receiver mode: Terrestrial = NEXUS_TunerApplication_eTerrestrial,Cable = NEXUS_TunerApplication_eCable.
										    If set to NEXUS_TunerApplication_eUnset, and if this device uses internal lna, then a warning message will be printed out.
	                                                                   This supercedes NEXUS_FrontendQamSettings.terrestrial and NEXUS_FrontendOfdmSettings.terrestrial and a mismatch will result in a warning. */
#if defined(HUMAX_PLATFORM_BASE) && (defined(NEXUS_FRONTEND_3461)||defined(NEXUS_FRONTEND_45216))
	bool openPending; /* Set true if async device open/resume is pending */
	bool openFailed; /* Set true if async device open/resume has failed */
#endif
#if NEXUS_AMPLIFIER_SUPPORT
    NEXUS_AmplifierHandle amplifier;
#endif
    bool abortThread;
#if defined(NEXUS_FRONTEND_45216)
    /* one frontend device may have up to two mtsif configs. i.e. host only sees one device, but there are two frontend XPT blocks to control.
       the chained config is always HAB/RPC-chained, and only optionally MTSIF-chained */
    NEXUS_FrontendDeviceMtsifConfig mtsifConfig, *chainedConfig;
#endif
    NEXUS_Error (*getInternalGain)(void *handle, const NEXUS_GainParameters *params, NEXUS_InternalGainSettings *pSettings);
    NEXUS_Error (*getExternalGain)(void *handle, NEXUS_ExternalGainSettings *pSettings);
    NEXUS_Error (*setExternalGain)(void *handle, const NEXUS_ExternalGainSettings *pSettings);
    NEXUS_Error (*getAmplifierStatus)(void *handle, NEXUS_AmplifierStatus *pStatus);
    NEXUS_Error (*setAmplifierStatus)(void *handle, const NEXUS_AmplifierStatus *pStatus);

#if defined(HUMAX_PLATFORM_BASE) && (defined(NEXUS_FRONTEND_3461)||defined(NEXUS_FRONTEND_45216))
    NEXUS_Error (*getStatus)(void *handle, NEXUS_FrontendDeviceStatus *pStatus);
    void        (*close)(void * handle);
#endif

    NEXUS_Error (*standby)(void *handle, const NEXUS_StandbySettings *pSettings);
    /* NEXUS_Error (*recalibrate)(void *handle, const NEXUS_FrontendDeviceRecalibrateSettings *pSettings);*/
    void        (*getCapabilities)(void *handle, NEXUS_FrontendDeviceCapabilities *pCapabilities);
    void        (*getTunerCapabilities)(void *handle, unsigned tunerIndex, NEXUS_FrontendCapabilities *pCapabilities);
	NEXUS_Error (*getSatelliteCapabilities)(void *handle, NEXUS_FrontendSatelliteCapabilities *pCapabilities);
    NEXUS_Error (*delayedInit)(NEXUS_FrontendDeviceHandle handle);

} NEXUS_FrontendDevice;

NEXUS_Error NEXUS_Frontend_P_GetInternalGain(NEXUS_FrontendDeviceHandle handle, const NEXUS_GainParameters *params, NEXUS_InternalGainSettings *pSettings);
NEXUS_Error NEXUS_Frontend_P_GetExternalGain(NEXUS_FrontendDeviceHandle handle, NEXUS_ExternalGainSettings *pSettings);
NEXUS_Error NEXUS_Frontend_P_SetExternalGain(NEXUS_FrontendDeviceHandle handle, const NEXUS_ExternalGainSettings *pSettings);
#if NEXUS_AMPLIFIER_SUPPORT
NEXUS_Error NEXUS_Frontend_P_GetAmplifierStatus(NEXUS_FrontendDeviceHandle handle, NEXUS_AmplifierStatus *pStatus);
NEXUS_Error NEXUS_Frontend_P_SetAmplifierStatus(NEXUS_FrontendDeviceHandle handle, const NEXUS_AmplifierStatus *pStatus);
#endif
#if defined(HUMAX_PLATFORM_BASE) && (defined(NEXUS_FRONTEND_3461)||defined(NEXUS_FRONTEND_45216))
NEXUS_Error NEXUS_Frontend_P_CheckDeviceOpen(NEXUS_FrontendHandle handle);
NEXUS_Error NEXUS_FrontendDevice_P_CheckOpen(NEXUS_FrontendDeviceHandle handle);
#endif
/***************************************************************************
 * Handle for a generic frontend object
 ***************************************************************************/
typedef struct NEXUS_Frontend
{
    NEXUS_OBJECT(NEXUS_Frontend);
    BLST_SQ_ENTRY(NEXUS_Frontend) node;
    BLST_SQ_ENTRY(NEXUS_Frontend) link; /* list of all frontends maintained by Create/Destroy */
    bool acquired;
    struct NEXUS_Frontend *pParentFrontend;     /* Only set for extended frontends */
    unsigned numExtensions;
    NEXUS_FrontendConnectorHandle connector;
    void *pDeviceHandle;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DEMOD_TSMF)
	bool bEnabledTSMF;
	bool bConnectedNormal;
	int nTSMFIndex;
#endif
    NEXUS_FrontendDevice *pGenericDeviceHandle;
    struct {
        bool digital;
        bool analog;
    } power;
    NEXUS_FrontendCapabilities capabilities;
    NEXUS_FrontendUserParameters userParameters;
    void        (*close)(NEXUS_FrontendHandle handle);
    NEXUS_Error (*registerExtension)(NEXUS_FrontendHandle parentHandle, NEXUS_FrontendHandle extensionHandle);

    NEXUS_Error (*tuneAnalog)(void *handle, const NEXUS_FrontendAnalogSettings *pSettings);
    NEXUS_Error (*getAnalogStatus)(void *handle, NEXUS_FrontendAnalogStatus *pStatus);
    NEXUS_VideoInput (*getAnalogVideoConnector)(void *handle);
    NEXUS_AudioInput (*getAnalogAudioConnector)(void *handle);
    NEXUS_Error (*tuneOutOfBand)(void *handle, const NEXUS_FrontendOutOfBandSettings *pSettings);
    NEXUS_Error (*getOutOfBandStatus)(void *handle, NEXUS_FrontendOutOfBandStatus *pStatus);
    NEXUS_Error (*tuneQam)(void *handle, const NEXUS_FrontendQamSettings *pSettings);
    NEXUS_Error (*getQamStatus)(void *handle, NEXUS_FrontendQamStatus *pStatus);
    NEXUS_Error (*tuneUpstream)(void *handle, const NEXUS_FrontendUpstreamSettings *pSettings);
    NEXUS_Error (*getUpstreamStatus)(void *handle, NEXUS_FrontendUpstreamStatus *pStatus);
    NEXUS_Error (*tuneVsb)(void *handle, const NEXUS_FrontendVsbSettings *pSettings);
    NEXUS_Error (*getVsbStatus)(void *handle, NEXUS_FrontendVsbStatus *pStatus);
    NEXUS_Error (*tuneSatellite)(void *handle, const NEXUS_FrontendSatelliteSettings *pSettings);
    NEXUS_Error (*getSatelliteStatus)(void *handle, NEXUS_FrontendSatelliteStatus *pStatus);
    void        (*getDiseqcSettings)(void *handle, NEXUS_FrontendDiseqcSettings *pSettings);
    NEXUS_Error (*setDiseqcSettings)(void *handle, const NEXUS_FrontendDiseqcSettings *pSettings);
    NEXUS_Error (*getDiseqcStatus)(void *handle, NEXUS_FrontendDiseqcStatus *pStatus);
    NEXUS_Error (*sendDiseqcMessage)(void *handle, const uint8_t *pSendData, size_t sendDataSize, const NEXUS_CallbackDesc *pSendComplete);
    NEXUS_Error (*getDiseqcReply)(void *handle, NEXUS_FrontendDiseqcMessageStatus *pStatus, uint8_t *pReplyBuffer, size_t replyBufferSize, size_t *pReplyLength);
    NEXUS_Error (*sendDiseqcAcw)(void *handle, uint8_t codeWord);
    NEXUS_Error (*resetDiseqc)(void *handle, uint8_t options);
    NEXUS_Error (*tuneOfdm)(void *handle, const NEXUS_FrontendOfdmSettings *pSettings);
    NEXUS_Error (*getOfdmStatus)(void *handle, NEXUS_FrontendOfdmStatus *pStatus);
    NEXUS_Error (*getSoftDecisions)(void *handle, NEXUS_FrontendSoftDecision *pDecisions, size_t length);
    NEXUS_Error (*readSoftDecisions)(void *handle, NEXUS_FrontendSoftDecision *pDecisions, size_t length, size_t *pNumRead);
    void        (*resetStatus)(void *handle);
    #ifdef NEXUS_FRONTEND_SCAN_SUPPORTED
    NEXUS_Error (*scanFrequency)(void *handle, const NEXUS_FrontendScanSettings *pSettings, NEXUS_FrontendScanResults *pResults);
    void        (*getScanThresholds)(void *handle, NEXUS_FrontendScanThresholds *pThresholds);
    NEXUS_Error (*setScanThresholds)(void *handle, const NEXUS_FrontendScanThresholds *pThresholds);
    #endif
    void        (*getAnalogAgcSettings)(void *handle, NEXUS_FrontendAnalogAgcSettings *pSettings);
    NEXUS_Error (*setAnalogAgcSettings)(void *handle, const NEXUS_FrontendAnalogAgcSettings *pSettings);
    void        (*notifyAnalogVideoFormat)(void *handle, NEXUS_VideoFormat format);
    void        (*getAnalogVideoFormat)(void *handle, NEXUS_VideoFormat *pFormat);
    NEXUS_Error (*setAnalogAVFormat)(void *handle, NEXUS_VideoFormat format, NEXUS_IfdAudioMode audioMode, bool carriersOnly);
    void        (*setAnalogAudioInterrupt)(void *handle, NEXUS_FrontendAnalogAudioInterruptFunc function_isr, void *pFuncParam);
    void        (*untune)(void *handle); /* optional */
    NEXUS_Error (*readSatelliteConfig)(void *handle, unsigned id, void *buffer, unsigned bufferSize);
    NEXUS_Error (*writeSatelliteConfig)(void *handle, unsigned id, const void *buffer, unsigned bufferSize);
    NEXUS_Error (*satellitePeakscan)(void *handle, const NEXUS_FrontendSatellitePeakscanSettings *pSettings);
    NEXUS_Error (*getSatellitePeakscanResult)(void *handle, NEXUS_FrontendSatellitePeakscanResult *pResult);
    NEXUS_Error (*satelliteToneSearch)(void *handle, const NEXUS_FrontendSatelliteToneSearchSettings *pSettings);
    NEXUS_Error (*getSatelliteToneSearchResult)(void *handle, NEXUS_FrontendSatelliteToneSearchResult *pResult);
    NEXUS_Error (*requestQamAsyncStatus)(void *handle);
    NEXUS_Error (*getQamAsyncStatus)(void *handle, NEXUS_FrontendQamStatus *pStatus);
    NEXUS_Error (*requestOutOfBandAsyncStatus)(void *handle);
    NEXUS_Error (*getOutOfBandAsyncStatus)(void *handle, NEXUS_FrontendOutOfBandStatus *pStatus);
    NEXUS_Error (*requestOfdmAsyncStatus)(void *handle);
    NEXUS_Error (*getOfdmAsyncStatus)(void *handle, NEXUS_FrontendOfdmStatus *pStatus);
    NEXUS_Error (*getFastStatus)(void *handle, NEXUS_FrontendFastStatus *pStatus );
    NEXUS_Error (*reapplyTransportSettings)(void *handle);
    NEXUS_Error (*getQamScanStatus)(void *handle, NEXUS_FrontendQamScanStatus *pScanStatus );
    void        (*getType)(void *handle, NEXUS_FrontendType *pType);
    NEXUS_Error (*requestSpectrumData)(void *handle, const NEXUS_FrontendSpectrumSettings *settings );
    NEXUS_Error (*requestDvbt2AsyncStatus)(void *handle, NEXUS_FrontendDvbt2StatusType type);
    NEXUS_Error (*getDvbt2AsyncStatusReady)(void *handle, NEXUS_FrontendDvbt2StatusReady *pStatusReady);
    NEXUS_Error (*getDvbt2AsyncStatus)(void *handle, NEXUS_FrontendDvbt2StatusType type, NEXUS_FrontendDvbt2Status *pStatus);
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_FRONTEND_ADD)
    NEXUS_Error (*setStuffBytes)(void *handle, uint8_t *ucStuffbyte);
    NEXUS_Error (*setXportCtl)(void *handle,  bool bClkSup);
    NEXUS_Error (*setTsOutput)(void *handle,  bool bRsDisable);
#endif
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DEMOD_TSMF)
    NEXUS_Error (*reapplyTSMFSettings)(void *handle);
    NEXUS_Error (*getpathinfo)(void *handle);
#endif
    NEXUS_Error (*requestDvbc2AsyncStatus)(void *handle, NEXUS_FrontendDvbc2StatusType type);
    NEXUS_Error (*getDvbc2AsyncStatusReady)(void *handle, NEXUS_FrontendDvbc2StatusReady *pStatusReady);
    NEXUS_Error (*getDvbc2AsyncStatus)(void *handle, NEXUS_FrontendDvbc2StatusType type, NEXUS_FrontendDvbc2Status *pStatus);
    NEXUS_Error (*writeRegister)(void *handle, unsigned address, uint32_t value);
    NEXUS_Error (*readRegister)(void *handle, unsigned address, uint32_t *value);
    NEXUS_Error (*standby)(void *handle, bool enabled, const NEXUS_StandbySettings *pSettings);
    NEXUS_Error (*getTemperature)(void *handle, NEXUS_FrontendTemperature *pTemp);
    NEXUS_Error (*transmitDebugPacket)(void *handle, NEXUS_FrontendDebugPacketType type, const uint8_t *pBuffer, size_t size);

    NEXUS_Error (*getSatelliteSignalDetectStatus)(void *handle, NEXUS_FrontendSatelliteSignalDetectStatus *pStatus);
    NEXUS_Error (*getSatelliteAgcStatus)(void *handle, NEXUS_FrontendSatelliteAgcStatus *pStatus);
    NEXUS_Error (*getBertStatus)(void *handle, NEXUS_FrontendBertStatus *pStatus);
    NEXUS_Error (*getSatelliteRuntimeSettings)(void *handle, NEXUS_FrontendSatelliteRuntimeSettings *pSettings);
    NEXUS_Error (*setSatelliteRuntimeSettings)(void *handle, const NEXUS_FrontendSatelliteRuntimeSettings *pSettings);

    NEXUS_Time resetStatusTime;
    NEXUS_FrontendChipType chip;
} NEXUS_Frontend;

NEXUS_OBJECT_CLASS_DECLARE(NEXUS_Frontend);


#if !defined(NEXUS_FRONTEND_45216)
NEXUS_Error NEXUS_Frontend_P_SetMtsifConfig(NEXUS_FrontendHandle frontend, NEXUS_FrontendDeviceMtsifConfig *pConfig);
NEXUS_Error NEXUS_Frontend_P_UnsetMtsifConfig(NEXUS_FrontendHandle frontend, NEXUS_FrontendDeviceMtsifConfig *pConfig);
#else

#endif
NEXUS_Error NEXUS_Frontend_P_SetTransportConfig(NEXUS_FrontendHandle frontend, NEXUS_FrontendDeviceMtsifConfig *pConfig);
NEXUS_Error NEXUS_Frontend_P_UnsetTransportConfig(NEXUS_FrontendHandle frontend, NEXUS_FrontendDeviceMtsifConfig *pConfig);

NEXUS_Error NEXUS_Frontend_P_SetTsmfConfig(NEXUS_FrontendHandle frontend, NEXUS_FrontendDeviceMtsifConfig *pConfig);

/***************************************************************************
 * Module Instance Data
 ***************************************************************************/
/* global module handle & data */
extern NEXUS_ModuleHandle g_NEXUS_frontendModule;
extern NEXUS_FrontendModuleSettings g_NEXUS_frontendModuleSettings;

/***************************************************************************
 * Generic tuner handle
 ***************************************************************************/
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_Tuner);
typedef struct NEXUS_Tuner
{
    NEXUS_OBJECT(NEXUS_Tuner);
    void *pDeviceHandle;
    void (*close)(void *handle);
    NEXUS_Error (*setFrequency)(void *handle, NEXUS_TunerMode mode, unsigned frequency);
    unsigned ifFrequency;
    NEXUS_Error (*init)(void *handle);
    NEXUS_Error (*getStatus)(void *handle, NEXUS_TunerStatus *pStatus);
    void (*getDefaultTuneSettings)(NEXUS_TunerMode mode, NEXUS_TunerTuneSettings *pSettings);
    NEXUS_Error (*tune)(void *handle, const NEXUS_TunerTuneSettings *pSettings);
    NEXUS_Error (*getSettings)(void *handle, NEXUS_TunerSettings *pSettings);
    NEXUS_Error (*setSettings)(void *handle, const NEXUS_TunerSettings *pSettings);
    void *(*getAgcScript)(void *handle);
    void (*getAttributes)(void *handle, NEXUS_TunerAttributes *pAttributes);
    NEXUS_Error (*readPowerLevel)(void *handle, int *pPowerLevel);
} NEXUS_Tuner;

/***************************************************************************
 * Generic method to create a nexus tuner.  It will be automatically
 * destroyed when NEXUS_Tuner_Close is called.
 ***************************************************************************/
NEXUS_TunerHandle NEXUS_Tuner_P_Create(
    void *pDeviceHandle
    );

#if NEXUS_TUNER_SUPPORT
#include "btnr.h"
/***************************************************************************
 * Method to create a tuner from a BTNR handle.
 ***************************************************************************/
NEXUS_Tuner *NEXUS_Tuner_P_CreateFromBTNR(
    BTNR_Handle tnrHandle
    );
#endif

/***************************************************************************
 * Generic IFD Handle
 ***************************************************************************/
#if NEXUS_IFD_SUPPORT
#include "bifd.h"
typedef struct NEXUS_Ifd
{
    BIFD_Handle ifdHandle;
} NEXUS_Ifd;
#endif

/***************************************************************************
 * Generic Frontend Card handle
 ***************************************************************************/
#define NEXUS_MAX_FRONTEND_CHANNELS 8
#define NEXUS_MAX_FRONTEND_OOB_CHANNELS 1
#define NEXUS_MAX_FRONTEND_AUS_CHANNELS 1

BDBG_OBJECT_ID_DECLARE(NEXUS_FrontendCard);
typedef struct NEXUS_FrontendCard
{
    NEXUS_OBJECT(NEXUS_FrontendCard);
    unsigned numChannels;
    unsigned numOutOfBandChannels;
    unsigned numUpstreamChannels;
    NEXUS_FrontendHandle frontends[NEXUS_MAX_FRONTEND_CHANNELS];
    NEXUS_FrontendHandle outOfBandFrontends[NEXUS_MAX_FRONTEND_OOB_CHANNELS];
    NEXUS_FrontendHandle upstreamFrontends[NEXUS_MAX_FRONTEND_AUS_CHANNELS];
    NEXUS_AmplifierHandle amplifiers[NEXUS_MAX_FRONTEND_CHANNELS];
    NEXUS_TunerHandle tuners[NEXUS_MAX_FRONTEND_CHANNELS];
    NEXUS_IfdHandle ifds[NEXUS_MAX_FRONTEND_CHANNELS];
} NEXUS_FrontendCard;

NEXUS_OBJECT_CLASS_DECLARE(NEXUS_FrontendCard);


/***************************************************************************
 * Frontend Private Routines
 ***************************************************************************/
void NEXUS_Frontend_P_Init(void);
NEXUS_FrontendHandle NEXUS_Frontend_P_Create(void *pDeviceHandle);
void NEXUS_Frontend_P_Destroy(NEXUS_FrontendHandle handle);
void NEXUS_Ifd_P_GetDefaultSettings(NEXUS_IfdSettings *pSettings);
bool NEXUS_Frontend_P_ProbeCard(NEXUS_FrontendCard *pCard, const NEXUS_FrontendCardSettings *pSettings);
unsigned NEXUS_Frontend_P_GetDefaultQamSymbolRate(NEXUS_FrontendQamMode mode, NEXUS_FrontendQamAnnex annex);
void NEXUS_Frontend_P_SetPower(NEXUS_FrontendHandle handle, NEXUS_PowerManagementCore core, bool poweredUp);
NEXUS_FrontendHandle NEXUS_Frontend_P_GetParentHandle(NEXUS_FrontendHandle handle);

#if NEXUS_HAS_MXT
NEXUS_Error NEXUS_Frontend_P_InitMtsifConfig(NEXUS_FrontendDeviceMtsifConfig *pConfig, const BMXT_Settings *mxtSettings);
#else
NEXUS_Error NEXUS_Frontend_P_InitMtsifConfig(NEXUS_FrontendDeviceMtsifConfig *pConfig, const void *mxtSettings);
#endif
#if defined(NEXUS_FRONTEND_45216)
NEXUS_Error NEXUS_Frontend_P_SetMtsifConfig(NEXUS_FrontendHandle frontend);
#endif

#ifdef __cplusplus
}
#endif

#endif /* #ifndef NEXUS_FRONTEND_MODULE_H__ */

