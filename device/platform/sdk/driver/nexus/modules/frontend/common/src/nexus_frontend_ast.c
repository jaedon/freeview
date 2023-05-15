/******************************************************************************
 *    (c)2008-2012 Broadcom Corporation
 *
 * This program is the proprietary software of Broadcom Corporation and/or its licensors,
 * and may only be used, duplicated, modified or distributed pursuant to the terms and
 * conditions of a separate, written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 * no license (express or implied), right to use, or waiver of any kind with respect to the
 * Software, and Broadcom expressly reserves all rights in and to the Software and all
 * intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 * NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 * secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 * AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 * WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 * THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 * USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 * LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 * EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 * ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: nexus_frontend_ast.c $
 * $brcm_Revision: 91 $
 * $brcm_Date: 10/24/12 3:50p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/frontend/common/src/nexus_frontend_ast.c $
 *
 * 91   10/24/12 3:50p mphillip
 * SW7425-4035: Merge 4538 support branch to main
 *
 * SW7425-4035/1   10/4/12 5:19p mphillip
 * SW7425-4035: Initial 4538 support
 *
 * 90   9/11/12 2:24p mphillip
 * SWSATFE-214: Update nexus 73xx frontend code to accommodate the
 *  reorganization
 *
 * 89   7/31/12 3:47p mphillip
 * SW7358-172: Enable tone burst/tone mode support for 7358 (and other
 *  7346 frontends)
 *
 * 88   7/30/12 5:41p mphillip
 * SW7346-951: Merge DSS/Bert updates to main
 *
 * CSP555846/1   7/30/12 1:21p cnovak
 * CSP555846: Correct BER configuration for DSS mode.
 *
 * 87   7/17/12 5:03p mphillip
 * SW7346-702: Restore internal LNA after powerup
 *
 * 86   6/21/12 5:26p mphillip
 * SW7358-304: Merge tone search to main
 *
 * SW7358-304/2   6/21/12 5:13p mphillip
 * SW7358-304: Place tone search min ratio numerator/denominator in a
 *  minRatio struct in tone search settings
 *
 * SW7358-304/1   6/20/12 6:32p mphillip
 * SW7358-304: Implement tone search
 *
 * 85   6/5/12 2:56p jtna
 * SW7425-2752: rework satellite frontend MTSIF connection
 *
 * 84   5/15/12 3:35p mphillip
 * SW3461-210: Switch Read/WriteSatelliteRegister to the new, general
 *  interface
 *
 * 83   5/15/12 3:10p mphillip
 * SW7358-277: Merge BERT clarifications to main
 *
 * 82   3/13/12 4:51p mphillip
 * SW7346-702: Guard power management code with ifdefs
 *
 * 81   2/13/12 2:12p mphillip
 * SW7346-680: Change NEXUS_Frontend_P_Ast_PeakscanEventHandler to exit
 *  the scan rather than assert when out of bounds
 *
 * 80   2/9/12 9:59a erickson
 * SW7425-1786: verify chip id before typecasting to NEXUS_AstDevice
 *
 * 79   1/17/12 3:47p mphillip
 * SW7346-618: Add control of BAST transport settings
 *
 * 78   12/5/11 3:09p mphillip
 * SW7346-580: Add NEXUS_Frontend_GetFastStatus support for BAST-based
 *  frontends
 *
 * 77   11/11/11 3:50p mphillip
 * SW7346-516: Merge to main
 *
 * SW7346-516/1   11/8/11 5:18p mphillip
 * SW7346-516: Allow diseqc to be powered down separately
 *
 * 76   7/11/11 12:36p vishk
 * SWSATFE-132: Add nexus support for 4528 satellite frontend for 97425
 *  SATIPSW platform.
 *
 * 75   6/23/11 7:47p vishk
 * SW7422-272: Add nexus gpio support for 4506 tuners. Bug fix.
 *
 * 74   6/23/11 3:11p vishk
 * SW7346-165: NEXUS Frontend changes for multi-tuner board are not
 *  platform independent
 *
 * 73   6/13/11 5:09p jrubio
 * SW7346-182: fix network spec pointer
 *
 * 72   6/9/11 7:33p randyjew
 * SW7346-230: Fix compile warnings
 *
 * 71   5/16/11 9:11a jrubio
 * SW7346-182: make use of handle not device
 *
 * 70   5/13/11 11:07a jrubio
 * SW7346-182: adding network spec for g3
 *
 * 69   2/22/11 5:12p jrubio
 * SW7346-17: fix peak scan functions for 7346/7344
 *
 * 68   1/20/11 10:54a xhuang
 * SW7358-24: Add support for 7358
 *
 * 67   1/17/11 11:16a jrubio
 * SW7344-9: fix compile warning
 *
 * 66   12/8/10 9:48a jrubio
 * SW7344-9: add G3 Support
 *
 * 65   11/16/10 2:20p erickson
 * SW7405-4968: add NEXUS_FrontendSatelliteSettings.networkSpec
 *
 * 64   10/5/10 3:40p jgarrett
 * SW7335-873: Adding searchRange option
 *
 * 63   10/1/10 4:57p mward
 * SWSATFE-80: fixed DISEQC_CTL2 bit definition for tone burst B (4501),
 *  updated DISEQC_CTL2 bit definitions (4506).
 *
 * 62   9/1/10 4:11p erickson
 * SW7325-800: add NEXUS_FrontendCapabilities.diseqc and use for internal
 *  configuration as wel
 *
 * 61   4/16/10 10:24a erickson
 * SW3556-1092: added frontend master I2C (MI2c) interface for 4501/4506
 *
 * 60   11/10/09 5:25p jrubio
 * SW7342-52: Make sure that AST Power calls are covered by
 *  NEXUS_POWER_MANAGEMENT Flag
 *
 * 59   10/8/09 5:27p erickson
 * SW7400-2562: fix DISEQC_CTL2_EXP_REPLY_DISABLE logic. don't overwrite
 *  previous values.
 *
 * 58   10/5/09 11:52a erickson
 * SW7400-2555: fix NEXUS_Frontend_P_Ast_SetDiseqcSettings WriteConfig
 *  buffer for 4501 & 4506
 *
 * 57   9/23/09 1:27p erickson
 * SW7335-561: create FTM task callback per AST core
 *
 * 56   9/16/09 12:08p erickson
 * SW7405-2980: support more variables in NEXUS_Frontend_SetDiseqcSettings
 *  on 4501/4506
 *
 * 55   8/20/09 12:59p erickson
 * PR57239: fix NEXUS_Frontend_P_Ast_GetChannel to handle extension
 *  handles
 *
 * 54   8/20/09 12:57p erickson
 * PR57471: convert to NEXUS_FrontendSatelliteStatusError enum
 *
 * 53   8/12/09 3:07p erickson
 * PR57471: added NEXUS_FrontendSatelliteStatus.statusAvailable
 *
 * 52   8/12/09 10:38a erickson
 * PR57598: added NEXUS_FrontendDiseqcStatus.sendStatus
 *
 * 51   8/3/09 9:24a erickson
 * PR57239: fix non-AST platforms
 *
 * 50   7/31/09 3:40p erickson
 * PR57239: move NEXUS_Frontend_P_GetAstDevice and related functions into
 *  nexus_frontend_ast.c
 *
 * 49   7/31/09 2:22p erickson
 * PR57239: don't propogate BAST_GetChannelStatus failure. just report
 *  status that it's not locked.
 *
 * 48   7/30/09 5:05p erickson
 * PR57175: sync up NEXUS_FrontendDiseqcMessageStatus with AST PI.
 *
 * 47   7/30/09 3:59p jtna
 * PR42924: remove unneeded code
 *
 * 46   7/21/09 3:21p erickson
 * PR56834: deprecated NEXUS_FrontendSatelliteStatus.lnaStatus for all sat
 *  demods except 4501
 *
 * 45   7/16/09 2:27p erickson
 * PR56773: reorder BAST_GetDiseqcVoltage cal
 *
 * 44   7/16/09 9:32a erickson
 * PR56832: fix NEXUS_FrontendSatelliteStatus.tunerLocked
 *
 * 43   7/14/09 11:12a erickson
 * PR56791: fix 4501 and 4506
 *
 * 42   7/14/09 10:40a erickson
 * PR56791: added NEXUS_FrontendDiseqcSettings.replyDisabled
 *
 * 41   7/13/09 4:47p erickson
 * PR56773: added NEXUS_FrontendDiseqcStatus
 *
 * 40   7/13/09 4:15p erickson
 * PR56772: force use of NEXUS_FrontendSatelliteInversion_eScan
 *
 * 39   7/13/09 3:48p erickson
 * PR56509: add NEXUS_FrontendDiseqcSettings.lnbEnabled
 *
 * 38   7/8/09 3:13p erickson
 * PR56607: remove unused NEXUS_Frontend_P_Register function
 *
 * 37   7/8/09 6:53a erickson
 * PR56607: add optional registerExtension operator so that callbacks can
 *  be recreated with the handle that the user gets. this allows
 *  NEXUS_StopCallbacks to work.
 *
 * 36   6/30/09 6:53p jtna
 * PR56368: support NEXUS_MAX_FRONTENDS < num AST channels
 *
 * 35   6/24/09 9:31a erickson
 * PR53078: added NEXUS_FrontendSatelliteStatus.agf
 *
 * 34   6/16/09 3:54p jgarrett
 * PR 55888: Adding envelope mode
 *
 * 33   6/9/09 2:25p erickson
 * PR55699: move NEXUS_AstDevice definition to private header file to
 *  enable AST extension
 *
 * 32   6/4/09 10:29a jtna
 * PR54416: nexus support for satellite scanning
 *
 * 31   5/26/09 12:16p erickson
 * PR55403: added NEXUS_FrontendDiseqcToneBurst
 *
 * 30   5/15/09 12:01p erickson
 * PR54843: added untune
 *
 * 29   5/11/09 4:29p erickson
 * PR53078: get time of tune or resetStatus so that elapsed time can be
 *  returned with status
 *
 * 28   4/3/09 4:46p erickson
 * PR53078: fix logic for adding standard acqSettings.acq_ctl settings
 *
 * 27   4/2/09 2:29p erickson
 * PR53078: satellite api additions
 *
 * 26   3/16/09 5:37p jrubio
 * PR50837: Get rid of more WRN's.
 *
 * 25   3/5/09 5:51p jrubio
 * PR50837: make BDBG_WRN msg's BDBG_MSG
 *
 * 24   2/11/09 12:24p jrubio
 * PR50837: make sure AST lock is a debug msg not a wrn
 *
 * 23   2/9/09 5:04p jgarrett
 * PR 50837: Merge to main branch
 *
 * PR50837/1   1/29/09 10:38a jgarrett
 * PR 50837: Removing board specifics
 *
 * 22   1/8/09 11:33a katrep
 * PR48985: Fixed compiler warning 7405 build
 *
 * 21   1/5/09 6:54p agin
 * PR48985: Correct the code rates for turbo.
 *
 * 20   12/22/08 9:48p agin
 * PR49982: Add reset diseqc.
 *
 * 19   12/18/08 9:35p agin
 * PR49983: Incorrect channel status reported for power, BER, SNR for
 *  97335 AST.
 *
 * 18   11/15/08 7:12a agin
 * PR48985: Support LDPC and Turbo at the same time.
 *
 * 17   11/12/08 6:36p agin
 * PR48907: Need to support Turbo 8PSK for 97325/97335.
 *
 * 16   10/21/08 10:51a agin
 * PR47371: Add DVB-S2 (8PSK) Support.
 *
 * 15   8/22/08 4:27p jrubio
 * PR42336: fix Frequency for 7335
 *
 * 14   8/5/08 10:45a jrubio
 * PR42336: take out "//" from comments
 *
 * 13   8/1/08 5:38p erickson
 * PR45361: c89 fixes
 *
 * 12   7/11/08 5:06p jtna
 * PR42924: Add channel change measurements functionality
 *
 * 11   7/2/08 9:20a erickson
 * PR42336: fix non-73xx
 *
 * 10   7/1/08 5:33p agin
 * PR42336: add external tuner support
 *
 * 9   6/19/08 6:20p jrubio
 *  PR42593: fix 7325 statement for diseq
 *
 * 8   6/16/08 5:42p jrubio
 * PR42593: Make sure that we do not block 4056/4501 diseqc calls
 *
 * 7   6/3/08 10:35a agin
 * PR42336: Temporarily comment out untested code for 7325.
 *
 * 6   6/3/08 8:38a agin
 * PR42336: Control frontend filters based on freq for 7335.  Level to
 *  bcm73xxast_diags.c, \33.
 *
 * 5   6/1/08 10:42a agin
 * PR42336: Fixed diseqc for 7335.
 *
 * 4   5/29/08 10:04a jrubio
 * PR42593: make sure 7335 diseq setting set are allowed for the first two
 *  channels
 *
 * 3   5/22/08 5:54p jrubio
 * PR42593: add multiple tuner support for 7325/7335 -external tuner does
 *  not control the diseqc
 *
 * 2   4/2/08 11:31a erickson
 * PR40198: fix DEBUG=n warning
 *
 * 1   3/14/08 9:47a jgarrett
 * PR 39985: adding ast
 *
 *****************************************************************************/
#include "nexus_frontend_module.h"
#include "nexus_frontend_ast.h"
#include "nexus_i2c.h"
#include "priv/nexus_i2c_priv.h"
#include "breg_i2c_priv.h"
#include "bi2c.h" /* status error codes */

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_FRONTEND_ADD)
#if defined(CONFIG_PLATFORM_DB4506)
#include "bchp_4506.h"
#include "bast_priv.h"

#define	SDS0_PKTP_MASK			0x000000FF
#define	SDS0_PKTP_ENABLE		0x00000077
#define	SDS0_PKTP_DISABLE		0x00000080
#define	SDS1_PKTP_MASK			0x007F8000
#define	SDS1_PKTP_ENABLE		0x003B8000
#define	SDS1_PKTP_DISABLE		0x00400000

#else
#include "bast_g3.h"
#endif
#endif

BDBG_MODULE(nexus_frontend_ast);
BDBG_OBJECT_ID(NEXUS_AstDevice);

BTRC_MODULE_DECLARE(ChnChange_Tune);
BTRC_MODULE_DECLARE(ChnChange_TuneLock);

#if defined NEXUS_FRONTEND_73XX
#define PEAK_SCAN_SYM_RATE_MIN      BAST_G2_CONFIG_PEAK_SCAN_SYM_RATE_MIN
#define LEN_PEAK_SCAN_SYM_RATE_MIN  BAST_G2_CONFIG_LEN_PEAK_SCAN_SYM_RATE_MIN
#define PEAK_SCAN_SYM_RATE_MAX      BAST_G2_CONFIG_PEAK_SCAN_SYM_RATE_MAX
#define LEN_PEAK_SCAN_SYM_RATE_MAX  BAST_G2_CONFIG_LEN_PEAK_SCAN_SYM_RATE_MAX
#endif

/***************************************************************************
Frontend Callback Prototypes
***************************************************************************/
NEXUS_Error NEXUS_Frontend_P_Ast_TuneSatellite(void *handle, const NEXUS_FrontendSatelliteSettings *pSettings);
void NEXUS_Frontend_P_Ast_Untune(void *handle);

static NEXUS_Error NEXUS_Frontend_P_Ast_GetSatelliteStatus(void *handle, NEXUS_FrontendSatelliteStatus *pStatus);
static void NEXUS_Frontend_P_Ast_GetDiseqcSettings(void *handle, NEXUS_FrontendDiseqcSettings *pSettings);
static NEXUS_Error NEXUS_Frontend_P_Ast_SetDiseqcSettings(void *handle, const NEXUS_FrontendDiseqcSettings *pSettings);
static NEXUS_Error NEXUS_Frontend_P_Ast_GetDiseqcStatus( void *handle, NEXUS_FrontendDiseqcStatus *pStatus );
static NEXUS_Error NEXUS_Frontend_P_Ast_SendDiseqcMessage(void *handle, const uint8_t *pSendData, size_t sendDataSize, const NEXUS_CallbackDesc *pSendComplete);
static NEXUS_Error NEXUS_Frontend_P_Ast_GetDiseqcReply(void *handle, NEXUS_FrontendDiseqcMessageStatus *pStatus, uint8_t *pReplyBuffer, size_t pReplyBufferSize, size_t *pReplyLength);
static NEXUS_Error NEXUS_Frontend_P_Ast_ResetDiseqc(void *handle, uint8_t options);
static NEXUS_Error NEXUS_Frontend_P_Ast_SendDiseqcAcw(void *handle, uint8_t codeWord);
static void NEXUS_Frontend_P_Ast_ResetStatus(void *handle);
static void NEXUS_Frontend_P_Ast_Close(NEXUS_FrontendHandle handle);
static NEXUS_Error NEXUS_Frontend_P_Ast_GetSoftDecisions(void *handle, NEXUS_FrontendSoftDecision *pDecisions, size_t length);
static void NEXUS_Frontend_P_Ast_LockEventHandler(void *pParam);
static void NEXUS_Frontend_P_Ast_DiseqcEventHandler(void *pParam);
static void NEXUS_Frontend_P_Ast_PeakscanEventHandler(void *pParam);
static NEXUS_Error NEXUS_Frontend_P_Ast_ReadSatelliteConfig( void *handle, unsigned id, void *buffer, unsigned bufferSize );
static NEXUS_Error NEXUS_Frontend_P_Ast_WriteSatelliteConfig( void *handle, unsigned id, const void *buffer, unsigned bufferSize );
static NEXUS_Error NEXUS_Frontend_P_Ast_SatellitePeakscan( void *handle, const NEXUS_FrontendSatellitePeakscanSettings *pSettings );
static NEXUS_Error NEXUS_Frontend_P_Ast_GetSatellitePeakscanResult( void *handle, NEXUS_FrontendSatellitePeakscanResult *pResult );
static NEXUS_Error NEXUS_Frontend_P_Ast_SatelliteToneSearch( void *handle, const NEXUS_FrontendSatelliteToneSearchSettings *pSettings );
static NEXUS_Error NEXUS_Frontend_P_Ast_GetSatelliteToneSearchResult( void *handle, NEXUS_FrontendSatelliteToneSearchResult *pResult );
static NEXUS_Error NEXUS_Frontend_P_Ast_RegisterExtension(NEXUS_FrontendHandle parentHandle, NEXUS_FrontendHandle extensionHandle);
static BERR_Code NEXUS_Frontend_P_Ast_I2cReadNoAddr(void * context, uint16_t chipAddr, uint8_t *pData, size_t length);
static BERR_Code NEXUS_Frontend_P_Ast_I2cWriteNoAddr(void * context, uint16_t chipAddr, const uint8_t *pData, size_t length);
static NEXUS_Error NEXUS_Frontend_P_Ast_GetFastStatus(void *handle, NEXUS_FrontendFastStatus *pStatus );
static NEXUS_Error NEXUS_Frontend_P_Ast_ReadRegister(void *handle, unsigned address, uint32_t *pValue);
static NEXUS_Error NEXUS_Frontend_P_Ast_WriteRegister(void *handle, unsigned address, uint32_t value);

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_FRONTEND_ADD)
static NEXUS_Error NEXUS_Frontend_P_Ast_SetStuffBytes(void *handle, uint8_t *ucStuffByte);
static NEXUS_Error NEXUS_Frontend_P_Ast_SetXportCtl(void *handle, bool bClkSup);
static NEXUS_Error NEXUS_Frontend_P_Ast_SetTsOutput(void *handle,  bool bEnableOutput);
#else
#endif

#ifdef TEST_NETWORK_TUNER
NEXUS_AstDevice *pAstDevice[3]; /* This allows us to test the network tuner */
#endif

/***************************************************************************
NEXUS -> PI Conversion Routines
***************************************************************************/
typedef struct NEXUS_AstModeEntry
{
    const NEXUS_FrontendSatelliteCodeRate *pCodeRate;
    BAST_Mode mode;
} NEXUS_AstModeEntry;

static const NEXUS_FrontendSatelliteCodeRate
    g_cr_scan = {0,0,0},
    g_cr_1_4 = {1,4,0},
    g_cr_1_2 = {1,2,0},
    g_cr_2_3 = {2,3,0},
    g_cr_3_4 = {3,4,0},
    g_cr_5_6 = {5,6,0},
    g_cr_6_7 = {6,7,0},
    g_cr_7_8 = {7,8,0},
    g_cr_5_11 = {5,11,0},
    g_cr_3_5 = {3,5,0},
    g_cr_4_5 = {4,5,0},
    g_cr_9_10 = {9,10,0},
    g_cr_8_9 = {8,9,0};

static const struct NEXUS_AstModeEntry
g_sds_dvb_modes[] = {
    {&g_cr_1_2, BAST_Mode_eDvb_1_2},
    {&g_cr_2_3, BAST_Mode_eDvb_2_3},
    {&g_cr_3_4, BAST_Mode_eDvb_3_4},
    {&g_cr_5_6, BAST_Mode_eDvb_5_6},
    {&g_cr_7_8, BAST_Mode_eDvb_7_8},
    {NULL, BAST_Mode_eDvb_scan}
},
g_sds_dss_modes[] = {
    {&g_cr_1_2, BAST_Mode_eDss_1_2},
    {&g_cr_2_3, BAST_Mode_eDss_2_3},
    {&g_cr_6_7, BAST_Mode_eDss_6_7},
    {NULL, BAST_Mode_eDss_scan}
},
g_sds_dcii_modes[] = {
    {&g_cr_1_2, BAST_Mode_eDcii_1_2},
    {&g_cr_2_3, BAST_Mode_eDcii_2_3},
    {&g_cr_3_5, BAST_Mode_eDcii_3_5},
    {&g_cr_3_4, BAST_Mode_eDcii_3_4},
    {&g_cr_4_5, BAST_Mode_eDcii_4_5},
    {&g_cr_5_6, BAST_Mode_eDcii_5_6},
    {&g_cr_7_8, BAST_Mode_eDcii_7_8},
    {&g_cr_5_11, BAST_Mode_eDcii_5_11},
    {NULL, BAST_Mode_eDcii_scan}
},
g_sds_qpsk_turbo_modes[] = {
    {&g_cr_1_2, BAST_Mode_eTurbo_Qpsk_1_2},
    {&g_cr_2_3, BAST_Mode_eTurbo_Qpsk_2_3},
    {&g_cr_3_4, BAST_Mode_eTurbo_Qpsk_3_4},
    {&g_cr_5_6, BAST_Mode_eTurbo_Qpsk_5_6},
    {&g_cr_7_8, BAST_Mode_eTurbo_Qpsk_7_8},
    {NULL, BAST_Mode_eTurbo_scan}
},
g_sds_8psk_turbo_modes[] = {
    {&g_cr_2_3, BAST_Mode_eTurbo_8psk_2_3},
    {&g_cr_3_4, BAST_Mode_eTurbo_8psk_3_4},
    {&g_cr_4_5, BAST_Mode_eTurbo_8psk_4_5},
    {&g_cr_5_6, BAST_Mode_eTurbo_8psk_5_6},
    {&g_cr_8_9, BAST_Mode_eTurbo_8psk_8_9},
    {NULL, BAST_Mode_eTurbo_scan}
},
g_sds_turbo_modes[] = {
    {&g_cr_1_2, BAST_Mode_eTurbo_Qpsk_1_2},
    {&g_cr_2_3, BAST_Mode_eTurbo_Qpsk_2_3},
    {&g_cr_3_4, BAST_Mode_eTurbo_Qpsk_3_4},
    {&g_cr_5_6, BAST_Mode_eTurbo_Qpsk_5_6},
    {&g_cr_7_8, BAST_Mode_eTurbo_Qpsk_7_8},
    {&g_cr_2_3, BAST_Mode_eTurbo_8psk_2_3},
    {&g_cr_3_4, BAST_Mode_eTurbo_8psk_3_4},
    {&g_cr_4_5, BAST_Mode_eTurbo_8psk_4_5},
    {&g_cr_5_6, BAST_Mode_eTurbo_8psk_5_6},
    {&g_cr_8_9, BAST_Mode_eTurbo_8psk_8_9},
    {NULL, BAST_Mode_eTurbo_scan}
},
g_sds_8psk_ldpc_modes[] = {
    {&g_cr_3_5, BAST_Mode_eLdpc_8psk_3_5},
    {&g_cr_2_3, BAST_Mode_eLdpc_8psk_2_3},
    {&g_cr_3_4, BAST_Mode_eLdpc_8psk_3_4},
    {&g_cr_5_6, BAST_Mode_eLdpc_8psk_5_6},
    {&g_cr_8_9, BAST_Mode_eLdpc_8psk_8_9},
    {&g_cr_9_10, BAST_Mode_eLdpc_8psk_9_10},
    {NULL, BAST_Mode_eLdpc_scan}
},
g_sds_qpsk_ldpc_modes[] = {
    {&g_cr_1_2, BAST_Mode_eLdpc_Qpsk_1_2},
    {&g_cr_3_5, BAST_Mode_eLdpc_Qpsk_3_5},
    {&g_cr_2_3, BAST_Mode_eLdpc_Qpsk_2_3},
    {&g_cr_3_4, BAST_Mode_eLdpc_Qpsk_3_4},
    {&g_cr_4_5, BAST_Mode_eLdpc_Qpsk_4_5},
    {&g_cr_5_6, BAST_Mode_eLdpc_Qpsk_5_6},
    {&g_cr_8_9, BAST_Mode_eLdpc_Qpsk_8_9},
    {&g_cr_9_10, BAST_Mode_eLdpc_Qpsk_9_10},
    {NULL, BAST_Mode_eLdpc_scan}
},
g_sds_ldpc_modes[] = {
    {&g_cr_1_2, BAST_Mode_eLdpc_Qpsk_1_2},
    {&g_cr_3_5, BAST_Mode_eLdpc_Qpsk_3_5},
    {&g_cr_2_3, BAST_Mode_eLdpc_Qpsk_2_3},
    {&g_cr_3_4, BAST_Mode_eLdpc_Qpsk_3_4},
    {&g_cr_4_5, BAST_Mode_eLdpc_Qpsk_4_5},
    {&g_cr_5_6, BAST_Mode_eLdpc_Qpsk_5_6},
    {&g_cr_8_9, BAST_Mode_eLdpc_Qpsk_8_9},
    {&g_cr_9_10, BAST_Mode_eLdpc_Qpsk_9_10},
    {&g_cr_3_5, BAST_Mode_eLdpc_8psk_3_5},
    {&g_cr_2_3, BAST_Mode_eLdpc_8psk_2_3},
    {&g_cr_3_4, BAST_Mode_eLdpc_8psk_3_4},
    {&g_cr_5_6, BAST_Mode_eLdpc_8psk_5_6},
    {&g_cr_8_9, BAST_Mode_eLdpc_8psk_8_9},
    {&g_cr_9_10, BAST_Mode_eLdpc_8psk_9_10},
    {NULL, BAST_Mode_eLdpc_scan}
},
g_blind_acquisition_mode[] = {
    {NULL, BAST_Mode_eBlindScan}
};

static BAST_Mode NEXUS_Frontend_P_Ast_GetMode(const struct NEXUS_AstModeEntry *pModes, const NEXUS_FrontendSatelliteCodeRate *pCodeRate)
{
    /* NOTE: there are not non-zero bitsPerSymbol values in the look up tables. if someone specifies a non-zero bitsPerSymbol,
    this algorithm will result in a scan. */
    for ( ;; pModes++ )
    {
        if ( NULL == pModes->pCodeRate )
        {
            BDBG_MSG(("Use scan for coderate %d:%d:%d", pCodeRate->numerator, pCodeRate->denominator, pCodeRate->bitsPerSymbol));
            return pModes->mode;
        }
        if ( pModes->pCodeRate->numerator == pCodeRate->numerator &&
             pModes->pCodeRate->denominator == pCodeRate->denominator &&
             pModes->pCodeRate->bitsPerSymbol == pCodeRate->bitsPerSymbol )
        {
            return pModes->mode;
        }
    }
}

void NEXUS_Frontend_P_Ast_GetDefaultSettings( NEXUS_FrontendAstSettings *pSettings )
{
    unsigned i;
    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));

    /* by default, everything is supported. specific chips should turn off what they don't support. */
    pSettings->capabilities.satellite = true;
    pSettings->capabilities.diseqc = true;
    for (i = 0;i < NEXUS_FrontendSatelliteMode_eMax; i++)
    {
        pSettings->capabilities.satelliteModes[i] = true;
    }
}

NEXUS_FrontendHandle NEXUS_Frontend_P_Ast_Create( const NEXUS_FrontendAstSettings *pSettings )
{
    BERR_Code errCode;
    NEXUS_AstDevice *pDevice;

    BDBG_ASSERT(NULL != pSettings);
    BDBG_ASSERT(NULL != pSettings->astHandle);
    BDBG_ASSERT(NULL != pSettings->astChannel);

    pDevice = BKNI_Malloc(sizeof(NEXUS_AstDevice));
    if ( NULL == pDevice )
    {
        errCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        return NULL;
    }
    BKNI_Memset(pDevice, 0, sizeof(*pDevice));

    pDevice->settings = *pSettings;     /* Save Settings */
    pDevice->astHandle = pSettings->astHandle;
    pDevice->astChannel = pSettings->astChannel;
    pDevice->astChip = pSettings->astChip;

    errCode = BAST_GetLockStateChangeEventHandle(pDevice->astChannel, &pDevice->lockEvent);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_lock_event;
    }

    if (pSettings->capabilities.diseqc)
    {
        (void)BAST_ResetDiseqc(pDevice->astChannel, 0);
    }

    pDevice->lockEventCallback = NEXUS_RegisterEvent(pDevice->lockEvent,
                                                     NEXUS_Frontend_P_Ast_LockEventHandler,
                                                     pDevice);
    if ( NULL == pDevice->lockEventCallback )
    {
        errCode = BERR_TRACE(BERR_OS_ERROR);
        goto err_lock_event_callback;
    }

    #if TEST_NETWORK_TUNER
    pAstDevice[pDevice->settings.channelIndex]=pDevice;
    #endif

    /* reset diseqc with options 0. Currently this paramater is not used by the AST PI */
    if (pSettings->capabilities.diseqc)
    {
        errCode = BAST_ResetDiseqc(pDevice->astChannel, 0);
        if ( errCode )
        {
            errCode = BERR_TRACE(errCode);
            goto err_reset_diseqc;
        }


        errCode = BAST_GetDiseqcEventHandle(pDevice->astChannel, &pDevice->diseqcEvent);
        if ( errCode )
        {
            errCode = BERR_TRACE(errCode);
            goto err_diseqc_event;
        }

        pDevice->diseqcEventCallback = NEXUS_RegisterEvent(pDevice->diseqcEvent,
                                                           NEXUS_Frontend_P_Ast_DiseqcEventHandler,
                                                           pDevice);
        if ( NULL == pDevice->diseqcEventCallback )
        {
            errCode = BERR_TRACE(errCode);
            goto err_diseqc_event_callback;
        }
        BKNI_Memset(&pDevice->diseqcSettings,0,sizeof(pDevice->diseqcSettings));
        pDevice->diseqcSettings.enabled = true;
    }

    errCode = BAST_GetPeakScanEventHandle(pDevice->astChannel, &pDevice->peakscanEvent);
    if ( errCode == BERR_NOT_SUPPORTED) {
        pDevice->peakscanEvent = NULL; /* used to determine whether peakscan is available */
    }
    else {
        if ( errCode )
        {
            errCode = BERR_TRACE(errCode);
            goto err_peak_scan_event;
        }

        pDevice->peakscanEventCallback = NEXUS_RegisterEvent(pDevice->peakscanEvent,
                                                             NEXUS_Frontend_P_Ast_PeakscanEventHandler,
                                                             pDevice);
        if ( NULL == pDevice->peakscanEventCallback )
        {
            errCode = BERR_TRACE(BERR_OS_ERROR);
            goto err_peak_scan_event_callback;
        }
    }

    /* Create Frontend Handle */
    pDevice->frontendHandle = NEXUS_Frontend_P_Create(pDevice);
    if ( NULL == pDevice->frontendHandle )
    {
        errCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        goto err_frontend_handle;
    }
    pDevice->frontendHandle->chip.id = pSettings->astChip;

    /* Set Capabilities */
    BDBG_ASSERT(pSettings->capabilities.satellite);
    pDevice->frontendHandle->capabilities = pSettings->capabilities;
    pDevice->capabilities = pSettings->capabilities;

    /* Install Hooks */
    pDevice->frontendHandle->close = NEXUS_Frontend_P_Ast_Close;
    pDevice->frontendHandle->untune = NEXUS_Frontend_P_Ast_Untune;
    pDevice->frontendHandle->getDiseqcReply = NEXUS_Frontend_P_Ast_GetDiseqcReply;
    pDevice->frontendHandle->getDiseqcSettings = NEXUS_Frontend_P_Ast_GetDiseqcSettings;
    pDevice->frontendHandle->sendDiseqcAcw = NEXUS_Frontend_P_Ast_SendDiseqcAcw;
    pDevice->frontendHandle->sendDiseqcMessage = NEXUS_Frontend_P_Ast_SendDiseqcMessage;
    pDevice->frontendHandle->setDiseqcSettings = NEXUS_Frontend_P_Ast_SetDiseqcSettings;
    pDevice->frontendHandle->getDiseqcStatus = NEXUS_Frontend_P_Ast_GetDiseqcStatus;
    pDevice->frontendHandle->resetDiseqc = NEXUS_Frontend_P_Ast_ResetDiseqc;
    pDevice->frontendHandle->getSatelliteStatus = NEXUS_Frontend_P_Ast_GetSatelliteStatus;
    pDevice->frontendHandle->getSoftDecisions = NEXUS_Frontend_P_Ast_GetSoftDecisions;
    pDevice->frontendHandle->tuneSatellite = NEXUS_Frontend_P_Ast_TuneSatellite;
    pDevice->frontendHandle->resetStatus = NEXUS_Frontend_P_Ast_ResetStatus;
    pDevice->frontendHandle->readSatelliteConfig = NEXUS_Frontend_P_Ast_ReadSatelliteConfig;
    pDevice->frontendHandle->writeSatelliteConfig = NEXUS_Frontend_P_Ast_WriteSatelliteConfig;
    pDevice->frontendHandle->satellitePeakscan = NEXUS_Frontend_P_Ast_SatellitePeakscan;
    pDevice->frontendHandle->getSatellitePeakscanResult = NEXUS_Frontend_P_Ast_GetSatellitePeakscanResult;
    pDevice->frontendHandle->satelliteToneSearch = NEXUS_Frontend_P_Ast_SatelliteToneSearch;
    pDevice->frontendHandle->getSatelliteToneSearchResult = NEXUS_Frontend_P_Ast_GetSatelliteToneSearchResult;
    pDevice->frontendHandle->registerExtension = NEXUS_Frontend_P_Ast_RegisterExtension;
    pDevice->frontendHandle->getFastStatus = NEXUS_Frontend_P_Ast_GetFastStatus;
    pDevice->frontendHandle->readRegister = NEXUS_Frontend_P_Ast_ReadRegister;
    pDevice->frontendHandle->writeRegister = NEXUS_Frontend_P_Ast_WriteRegister;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_FRONTEND_ADD)
    pDevice->frontendHandle->setStuffBytes = NEXUS_Frontend_P_Ast_SetStuffBytes;
    pDevice->frontendHandle->setXportCtl = NEXUS_Frontend_P_Ast_SetXportCtl;
    pDevice->frontendHandle->setTsOutput = NEXUS_Frontend_P_Ast_SetTsOutput;
#else
#endif

    /* Set up I2C handle */
    {
        NEXUS_I2cCallbacks i2cCallbacks;
        NEXUS_I2c_InitCallbacks(&i2cCallbacks);
        i2cCallbacks.readNoAddr = NEXUS_Frontend_P_Ast_I2cReadNoAddr;
        i2cCallbacks.writeNoAddr = NEXUS_Frontend_P_Ast_I2cWriteNoAddr;
        pDevice->deviceI2cHandle = NEXUS_I2c_CreateHandle(NEXUS_MODULE_SELF, pDevice, &i2cCallbacks);
        if (!pDevice->deviceI2cHandle ) {
            errCode = BERR_TRACE(BERR_NOT_SUPPORTED);
            goto err_i2c;
        }
    }

    if (pSettings->capabilities.diseqc)
    {
         /* These must be created after the frontend handle has been created */
         pDevice->diseqcAppCallback = NEXUS_TaskCallback_Create(pDevice->frontendHandle, NULL);
         if ( NULL == pDevice->diseqcAppCallback )
         {
             errCode = BERR_TRACE(BERR_OS_ERROR);
             goto err_diseqc_app_callback;
         }
     }

    if (pDevice->peakscanEvent) {
        pDevice->peakscanAppCallback = NEXUS_TaskCallback_Create(pDevice->frontendHandle, NULL);
        if ( NULL == pDevice->peakscanAppCallback )
        {
            errCode = BERR_TRACE(BERR_OS_ERROR);
            goto err_peak_scan_app_callback;
        }
    }

    pDevice->lockAppCallback = NEXUS_TaskCallback_Create(pDevice->frontendHandle, NULL);
    if ( NULL == pDevice->lockAppCallback )
    {
        errCode = BERR_TRACE(BERR_OS_ERROR);
        goto err_lock_app_callback;
    }

    pDevice->ftmCallback = NEXUS_TaskCallback_Create(pDevice->frontendHandle, NULL);
    if ( NULL == pDevice->ftmCallback )
    {
        errCode = BERR_TRACE(BERR_OS_ERROR);
        goto err_lock_app_callback;
    }

    /* Success */
    BDBG_OBJECT_SET(pDevice, NEXUS_AstDevice);
    return pDevice->frontendHandle;

err_lock_app_callback:
    if (pDevice->peakscanEvent) {
        NEXUS_TaskCallback_Destroy(pDevice->peakscanAppCallback);
    }
err_peak_scan_app_callback:
    NEXUS_TaskCallback_Destroy(pDevice->diseqcAppCallback);
err_diseqc_app_callback:
    NEXUS_I2c_DestroyHandle(pDevice->deviceI2cHandle);
err_i2c:
    NEXUS_Frontend_P_Destroy(pDevice->frontendHandle);
err_frontend_handle:
    if (pDevice->peakscanEvent) {
        NEXUS_UnregisterEvent(pDevice->peakscanEventCallback);
    }
err_peak_scan_event_callback:
err_peak_scan_event:
    NEXUS_UnregisterEvent(pDevice->diseqcEventCallback);
err_diseqc_event_callback:
err_diseqc_event:
err_reset_diseqc:
    NEXUS_UnregisterEvent(pDevice->lockEventCallback);
err_lock_event_callback:
err_lock_event:
    BKNI_Free(pDevice);

    return NULL;
}

static void NEXUS_Frontend_P_Ast_Close(NEXUS_FrontendHandle handle)
{
    NEXUS_AstDevice *pDevice;

    BDBG_OBJECT_ASSERT(handle, NEXUS_Frontend);
    pDevice = handle->pDeviceHandle;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_AstDevice);

    /* Cleanup Channel */
    if (pDevice->peakscanEvent) {
        NEXUS_UnregisterEvent(pDevice->peakscanEventCallback);
        NEXUS_TaskCallback_Destroy(pDevice->peakscanAppCallback);
    }
    NEXUS_UnregisterEvent(pDevice->lockEventCallback);
    NEXUS_TaskCallback_Destroy(pDevice->lockAppCallback);
    NEXUS_TaskCallback_Destroy(pDevice->ftmCallback);

    if (pDevice->capabilities.diseqc)
    {
        NEXUS_UnregisterEvent(pDevice->diseqcEventCallback);
        NEXUS_TaskCallback_Destroy(pDevice->diseqcAppCallback);
    }

    NEXUS_I2c_DestroyHandle(pDevice->deviceI2cHandle);
    NEXUS_Frontend_P_Destroy(pDevice->frontendHandle);

    /* Call post-close callback */
    if ( pDevice->settings.closeFunction )
    {
        pDevice->settings.closeFunction(handle, pDevice->settings.pCloseParam);
    }

    BKNI_Memset(pDevice, 0, sizeof(*pDevice));
    BKNI_Free(pDevice);
}

NEXUS_Error NEXUS_Frontend_P_Ast_SetDevices( NEXUS_FrontendHandle handle, const NEXUS_FrontendAstDevices *pDevices )
{
    NEXUS_AstDevice *pDevice;

    BDBG_OBJECT_ASSERT(handle, NEXUS_Frontend);
    BDBG_ASSERT(NULL != pDevices);

    pDevice = handle->pDeviceHandle;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_AstDevice);

    pDevice->settings.devices = *pDevices;

    return BERR_SUCCESS;
}

NEXUS_Error NEXUS_Frontend_P_Ast_TuneSatellite( void *handle, const NEXUS_FrontendSatelliteSettings *pSettings )
{
    BERR_Code errCode;
    NEXUS_AstDevice *pDevice = handle;
    BAST_AcqSettings acqSettings;
	uint8_t networkSpec;

    BDBG_OBJECT_ASSERT(pDevice, NEXUS_AstDevice);
    BDBG_ASSERT(NULL != pSettings);

    BKNI_Memset(&acqSettings, 0, sizeof(acqSettings));

    /* BAST_ACQSETTINGS_DEFAULT was applied in NEXUS_Frontend_GetDefaultSatelliteSettings */

    if ( pSettings->pnData )
        acqSettings.acq_ctl |= BAST_ACQSETTINGS_PN;
    if ( pSettings->prbs15 )
        acqSettings.acq_ctl |= BAST_ACQSETTINGS_PRBS15;
    if ( pSettings->nyquist20 )
        acqSettings.acq_ctl |= BAST_ACQSETTINGS_NYQUIST_20;
    if ( pSettings->bertEnable )
        acqSettings.acq_ctl |= BAST_ACQSETTINGS_BERT_ENABLE;
    if ( pSettings->pnDataInvert )
        acqSettings.acq_ctl |= BAST_ACQSETTINGS_PN_INVERT;
    if ( pSettings->bertResyncDisable )
        acqSettings.acq_ctl |= BAST_ACQSETTINGS_BERT_RESYNC_DISABLE;
    if ( pSettings->reacquireDisable )
        acqSettings.acq_ctl |= BAST_ACQSETTINGS_REACQ_DISABLE;
    if ( pSettings->dciiSplit )
        acqSettings.acq_ctl |= BAST_ACQSETTINGS_DCII_SPLIT;
    if ( pSettings->dciiSplitQ )
        acqSettings.acq_ctl |= BAST_ACQSETTINGS_DCII_SPLIT_Q;
    if ( pSettings->oQpsk )
        acqSettings.acq_ctl |= BAST_ACQSETTINGS_OQPSK;
    if ( pSettings->rsDisable )
        acqSettings.acq_ctl |= BAST_ACQSETTINGS_RS_DISABLE;
    if ( pSettings->ldpcPilot )
        acqSettings.acq_ctl |= BAST_ACQSETTINGS_LDPC_PILOT;
    if ( pSettings->ldpcPilotPll )
        acqSettings.acq_ctl |= BAST_ACQSETTINGS_LDPC_PILOT_PLL;
    if ( pSettings->ldpcPilotScan )
        acqSettings.acq_ctl |= BAST_ACQSETTINGS_LDPC_PILOT_SCAN;
    if ( pSettings->tunerTestMode )
        acqSettings.acq_ctl |= BAST_ACQSETTINGS_TUNER_TEST_MODE;
    if ( pSettings->toneSearchMode )
        acqSettings.acq_ctl |= BAST_ACQSETTINGS_TONE_SEARCH_MODE;
    if ( pSettings->signalDetectMode )
        acqSettings.acq_ctl |= BAST_ACQSETTINGS_SIGNAL_DETECT_MODE;
    if ( pSettings->bypassTune )
        acqSettings.acq_ctl |= BAST_ACQSETTINGS_BYPASS_TUNE;
    if ( pSettings->bypassAcquire )
        acqSettings.acq_ctl |= BAST_ACQSETTINGS_BYPASS_ACQUIRE;

    /* AST engineers prefer we always set this to scan. */
    if (pSettings->spectralInversion != NEXUS_FrontendSatelliteInversion_eScan) {
        BDBG_MSG(("forcing NEXUS_FrontendSatelliteInversion_eScan mode")); /* don't WRN */
    }
    acqSettings.acq_ctl |= BAST_ACQSETTINGS_SPINV_IQ_SCAN;

    /* This is added for backward compatibility */
    switch ( pSettings->mode )
    {
    case NEXUS_FrontendSatelliteMode_eQpskLdpc:
    case NEXUS_FrontendSatelliteMode_e8pskLdpc:
    case NEXUS_FrontendSatelliteMode_eLdpc:
        acqSettings.acq_ctl |= (BAST_ACQSETTINGS_NYQUIST_20 | BAST_ACQSETTINGS_SPINV_IQ_SCAN);
        break;
    default:
        break;
    }

    switch ( pSettings->mode )
    {
    case NEXUS_FrontendSatelliteMode_eDvb:
        BDBG_MSG(("Tune DVB"));
        acqSettings.mode = NEXUS_Frontend_P_Ast_GetMode(g_sds_dvb_modes, &pSettings->codeRate);
        break;
    case NEXUS_FrontendSatelliteMode_eDss:
        BDBG_MSG(("Tune DSS"));
        acqSettings.mode = NEXUS_Frontend_P_Ast_GetMode(g_sds_dss_modes, &pSettings->codeRate);
        break;
    case NEXUS_FrontendSatelliteMode_eDcii:
        BDBG_MSG(("Tune DCII"));
        acqSettings.mode = NEXUS_Frontend_P_Ast_GetMode(g_sds_dcii_modes, &pSettings->codeRate);
        break;
    case NEXUS_FrontendSatelliteMode_eQpskLdpc:
        BDBG_MSG(("Tune QPSK LDPC"));
        acqSettings.mode = NEXUS_Frontend_P_Ast_GetMode(g_sds_qpsk_ldpc_modes, &pSettings->codeRate);
        break;
    case NEXUS_FrontendSatelliteMode_e8pskLdpc:
        BDBG_MSG(("Tune 8PSK LDPC"));
        acqSettings.mode = NEXUS_Frontend_P_Ast_GetMode(g_sds_8psk_ldpc_modes, &pSettings->codeRate);
        break;
    case NEXUS_FrontendSatelliteMode_eLdpc:
        BDBG_MSG(("Tune LDCP"));
        acqSettings.mode = NEXUS_Frontend_P_Ast_GetMode(g_sds_ldpc_modes, &pSettings->codeRate);
        break;
    case NEXUS_FrontendSatelliteMode_eQpskTurbo:
        BDBG_MSG(("Tune Turbo QPSK"));
        acqSettings.mode = NEXUS_Frontend_P_Ast_GetMode(g_sds_qpsk_turbo_modes, &pSettings->codeRate);
        break;
    case NEXUS_FrontendSatelliteMode_e8pskTurbo:
        BDBG_MSG(("Tune Turbo 8PSK"));
        acqSettings.mode = NEXUS_Frontend_P_Ast_GetMode(g_sds_8psk_turbo_modes, &pSettings->codeRate);
        break;
    case NEXUS_FrontendSatelliteMode_eTurbo:
        BDBG_MSG(("Tune Turbo"));
        acqSettings.mode = NEXUS_Frontend_P_Ast_GetMode(g_sds_turbo_modes, &pSettings->codeRate);
        break;
    case NEXUS_FrontendSatelliteMode_eBlindAcquisition:
        BDBG_MSG(("Blind acquisition"));
        acqSettings.mode = NEXUS_Frontend_P_Ast_GetMode(g_blind_acquisition_mode, &pSettings->codeRate);
        break;
    default:
        /* turbo not supported */
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    BDBG_MSG(("acqSettings.acq_ctl %x", acqSettings.acq_ctl));
    acqSettings.symbolRate = pSettings->symbolRate;
    BDBG_MSG(("Freq %u, Symbol Rate %u", pSettings->frequency, acqSettings.symbolRate));
    acqSettings.carrierFreqOffset = pSettings->carrierOffset;

    NEXUS_TaskCallback_Set(pDevice->lockAppCallback, &pSettings->lockCallback);

#ifdef NEXUS_POWER_MANAGEMENT
    errCode = BAST_PowerUp(pDevice->astChannel, BAST_CORE_ALL);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    /* Restore the internal LNA (if configured to do so)
     * On some front-ends, this is required in order to be able to tune after the SDS core has been through a power cycle */
    if ( pDevice->settings.restoreInternalLnaFunction )
    {
        pDevice->settings.restoreInternalLnaFunction(handle, pDevice->settings.pRestoreParam);
    }
#endif

    /* Support external tuner if present */
    if ( pDevice->settings.devices.tuner )
    {
        errCode = NEXUS_Tuner_SetFrequency(pDevice->settings.devices.tuner, NEXUS_TunerMode_eDigital, pSettings->frequency);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
    }

    if (pSettings->networkSpec != pDevice->lastSettings.networkSpec) {
        networkSpec = (uint8_t)pSettings->networkSpec;
        BAST_SetNetworkSpec(pDevice->astHandle, networkSpec);
    }

    #ifdef BCM73XX_CONFIG_LEN_EXT_TUNER_IF_OFFSET
    /* Set IF offset for external tuners */
    {
        uint32_t val = (uint32_t)pSettings->ifOffset;
        uint8_t buf[4];

        buf[0] = (val >> 24) & 0xFF;
        buf[1] = (val >> 16) & 0xFF;
        buf[2] = (val >> 8) & 0xFF;
        buf[3] = (val & 0xFF);
        BAST_WriteConfig(pDevice->astChannel, BCM73XX_CONFIG_EXT_TUNER_IF_OFFSET, buf, BCM73XX_CONFIG_LEN_EXT_TUNER_IF_OFFSET);
    }
    #endif

    /* Warning, this overrides the search range for all channels of this AST device.  */
    errCode = BAST_SetSearchRange(pDevice->astHandle, pSettings->searchRange);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    /* If BERT mode is enabled OR if it has changed since the last time */
    if (pSettings->bertEnable || (pDevice->lastSettings.bertEnable != pSettings->bertEnable)) {
        /* BERT mode has changed */
        BAST_OutputTransportSettings xptSettings;
        BAST_GetOutputTransportSettings(pDevice->astChannel, &xptSettings);
        if ((pSettings->bertEnable) && (pSettings->mode != NEXUS_FrontendSatelliteMode_eDss)) {
            xptSettings.bXbert = true;
        } else {
            xptSettings.bXbert = false;
        }
        BAST_SetOutputTransportSettings(pDevice->astChannel, &xptSettings);
    }

    pDevice->lastSettings = *pSettings; /* save after all config, but before the acquire. */

    errCode = BAST_TuneAcquire(pDevice->astChannel, pSettings->frequency, &acqSettings);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    BTRC_TRACE(ChnChange_Tune, STOP);
    BTRC_TRACE(ChnChange_TuneLock, START);

    return BERR_SUCCESS;
}

void NEXUS_Frontend_P_Ast_Untune(void *handle)
{
    NEXUS_Error errCode;
    NEXUS_AstDevice *pDevice = handle;

    BDBG_MSG(("untune"));
#ifdef NEXUS_POWER_MANAGEMENT
    errCode = BAST_PowerDown(pDevice->astChannel, BAST_CORE_ALL);
    if (errCode) {errCode = BERR_TRACE(errCode);}
#else
    BSTD_UNUSED(pDevice);
    BSTD_UNUSED(errCode);
#endif
}

static NEXUS_Error NEXUS_Frontend_P_Ast_GetSatelliteStatus( void *handle, NEXUS_FrontendSatelliteStatus *pStatus )
{
    NEXUS_Error errCode;
    BAST_ChannelStatus astStatus;
    NEXUS_AstDevice *pDevice = handle;
    NEXUS_Time now;

    BDBG_OBJECT_ASSERT(pDevice, NEXUS_AstDevice);
    BDBG_ASSERT(NULL != pStatus);

    BKNI_Memset(pStatus, 0, sizeof(*pStatus));

    errCode = BAST_GetApVersion(pDevice->astHandle,
        &pStatus->version.chipId,
        &pStatus->version.chipVersion,
        &pStatus->version.bondingOption,
        &pStatus->version.apMicrocodeVersion,
        &pStatus->version.hostConfigurationVersion);
    if (errCode) {
        if (errCode == BI2C_ERR_NO_ACK) {
            pStatus->statusError = NEXUS_FrontendSatelliteStatusError_eI2cNoAck;
            return 0;
        }
        else if (errCode == BAST_ERR_HAB_TIMEOUT) {
            pStatus->statusError = NEXUS_FrontendSatelliteStatusError_eHabTimeout;
            return 0;
        }
        else {
            /* unknown error. status function should fail. */
            return BERR_TRACE(errCode);
        }
    }

    errCode = BAST_GetChannelStatus(pDevice->astChannel, &astStatus);
    if ( errCode ) {
        /* if not locked, BAST_GetChannelStatus will fail. this is not an error. the status structure should just report not locked and no status available. */
        pStatus->statusError = NEXUS_FrontendSatelliteStatusError_eGetChannelStatusError;
        return 0;
    }

    NEXUS_Time_Get(&now);
    pStatus->timeElapsed = NEXUS_Time_Diff(&now, &pDevice->frontendHandle->resetStatusTime);

    pStatus->settings = pDevice->lastSettings; /* return last settings used in NEXUS_Frontend_TuneSatellite */
    pStatus->channel = pDevice->settings.channelIndex;
    pStatus->sampleClock = astStatus.sample_clock;
    pStatus->carrierError = astStatus.carrierError;
    pStatus->outputBitRate = astStatus.outputBitrate;
    pStatus->symbolRate = astStatus.symbolRate;
    pStatus->symbolRateError = astStatus.symbolRateError;
    pStatus->snrEstimate = astStatus.snrEstimate * 100/256;  /* convert to 1/100 units */
    pStatus->tunerLocked = astStatus.bTunerLocked;
    pStatus->demodLocked = astStatus.bDemodLocked;
    pStatus->bertLocked = astStatus.bBertLocked;
    pStatus->mpegErrors = astStatus.mpegErrors;
    pStatus->mpegCount = astStatus.mpegCount;
    pStatus->berErrorCount = astStatus.berErrors;
    pStatus->ifAgc = astStatus.IFagc;
    pStatus->rfAgc = astStatus.RFagc;
    pStatus->agf = astStatus.agf;
    pStatus->frequency = astStatus.tunerFreq;

#if NEXUS_FRONTEND_4501
    {
    BAST_LnaStatus lnaStatus;
    BAST_GetLnaStatus(pDevice->settings.astHandle, &lnaStatus);
    pStatus->lnaStatus.intConfig = lnaStatus.int_config;
    pStatus->lnaStatus.extConfig = lnaStatus.ext_config;
    pStatus->lnaStatus.version = lnaStatus.version;
    pStatus->lnaStatus.agc0 = lnaStatus.agc0;
    pStatus->lnaStatus.agc1 = lnaStatus.agc1;
    }
#endif

    if (BAST_MODE_IS_LEGACY_QPSK(astStatus.mode))
    {
        pStatus->fecCorrected = astStatus.modeStatus.legacy.rsCorrCount;
        pStatus->fecUncorrected = astStatus.modeStatus.legacy.rsUncorrCount;
        pStatus->preViterbiErrorCount = astStatus.modeStatus.legacy.preVitErrCount;
        switch ( astStatus.modeStatus.legacy.spinv )
        {
        default:
            pStatus->spectralInversion = NEXUS_FrontendSatelliteInversion_eScan;
            break;
        case BAST_Spinv_eNormal:
            pStatus->spectralInversion = NEXUS_FrontendSatelliteInversion_eNormal;
            break;
        case BAST_Spinv_eIinv:
            pStatus->spectralInversion = NEXUS_FrontendSatelliteInversion_eI;
            break;
        case BAST_Spinv_eQinv:
            pStatus->spectralInversion = NEXUS_FrontendSatelliteInversion_eQ;
            break;
        }
        switch ( astStatus.modeStatus.legacy.phase )
        {
        default:
        case BAST_PhaseRotation_e0:
            pStatus->fecPhase = 0;
            break;
        case BAST_PhaseRotation_e90:
            pStatus->fecPhase = 90;
            break;
        case BAST_PhaseRotation_e180:
            pStatus->fecPhase = 180;
            break;
        case BAST_PhaseRotation_e270:
            pStatus->fecPhase = 270;
            break;
        }
    }
    else if ( BAST_MODE_IS_LDPC(astStatus.mode) )
    {
        pStatus->fecCorrected = astStatus.modeStatus.ldpc.corrBlocks;
        pStatus->fecUncorrected = astStatus.modeStatus.ldpc.badBlocks;
        pStatus->fecClean = astStatus.modeStatus.ldpc.totalBlocks -
            astStatus.modeStatus.ldpc.corrBlocks -
            astStatus.modeStatus.ldpc.badBlocks;
    }
    else if (BAST_MODE_IS_TURBO(astStatus.mode))
    {
        pStatus->fecCorrected = astStatus.modeStatus.turbo.corrBlocks;
        pStatus->fecUncorrected = astStatus.modeStatus.turbo.badBlocks;
        pStatus->fecClean = astStatus.modeStatus.turbo.totalBlocks -
            astStatus.modeStatus.turbo.corrBlocks -
            astStatus.modeStatus.turbo.badBlocks;
   }

    /* Check mode to get coderate */
    switch ( astStatus.mode )
    {
    case BAST_Mode_eDvb_scan:
    case BAST_Mode_eDss_scan:
    case BAST_Mode_eDcii_scan:
    case BAST_Mode_eLdpc_scan:
    case BAST_Mode_eTurbo_scan:
        pStatus->codeRate = g_cr_scan;
        break;

    case BAST_Mode_eDvb_1_2:
    case BAST_Mode_eDss_1_2:
    case BAST_Mode_eDcii_1_2:
    case BAST_Mode_eLdpc_Qpsk_1_2:
    case BAST_Mode_eTurbo_Qpsk_1_2:
        pStatus->codeRate = g_cr_1_2;
        break;

    case BAST_Mode_eDvb_2_3:
    case BAST_Mode_eDss_2_3:
    case BAST_Mode_eDcii_2_3:
    case BAST_Mode_eLdpc_Qpsk_2_3:
    case BAST_Mode_eLdpc_8psk_2_3:
    case BAST_Mode_eTurbo_Qpsk_2_3:
    case BAST_Mode_eTurbo_8psk_2_3:
        pStatus->codeRate = g_cr_2_3;
        break;

    case BAST_Mode_eDvb_3_4:
    case BAST_Mode_eDcii_3_4:
    case BAST_Mode_eLdpc_Qpsk_3_4:
    case BAST_Mode_eLdpc_8psk_3_4:
    case BAST_Mode_eTurbo_Qpsk_3_4:
    case BAST_Mode_eTurbo_8psk_3_4:
        pStatus->codeRate = g_cr_3_4;
        break;

    case BAST_Mode_eDcii_3_5:
    case BAST_Mode_eLdpc_Qpsk_3_5:
    case BAST_Mode_eLdpc_8psk_3_5:
        pStatus->codeRate = g_cr_3_5;
        break;

    case BAST_Mode_eDcii_4_5:
    case BAST_Mode_eLdpc_Qpsk_4_5:
    case BAST_Mode_eTurbo_8psk_4_5:
        pStatus->codeRate = g_cr_4_5;
        break;

    case BAST_Mode_eDvb_5_6:
    case BAST_Mode_eDcii_5_6:
    case BAST_Mode_eLdpc_Qpsk_5_6:
    case BAST_Mode_eLdpc_8psk_5_6:
    case BAST_Mode_eTurbo_Qpsk_5_6:
    case BAST_Mode_eTurbo_8psk_5_6:
        pStatus->codeRate = g_cr_5_6;
        break;

    case BAST_Mode_eDss_6_7:
        pStatus->codeRate = g_cr_6_7;
        break;

    case BAST_Mode_eDvb_7_8:
    case BAST_Mode_eDcii_7_8:
    case BAST_Mode_eTurbo_Qpsk_7_8:
        pStatus->codeRate = g_cr_7_8;
        break;

    case BAST_Mode_eDcii_5_11:
        pStatus->codeRate = g_cr_5_11;
        break;

    case BAST_Mode_eLdpc_Qpsk_8_9:
    case BAST_Mode_eLdpc_8psk_8_9:
    case BAST_Mode_eTurbo_8psk_8_9:
        pStatus->codeRate = g_cr_8_9;
        break;

    case BAST_Mode_eLdpc_Qpsk_9_10:
    case BAST_Mode_eLdpc_8psk_9_10:
        pStatus->codeRate = g_cr_9_10;
        break;

    default:
        break;
    }

    switch ( astStatus.mode )
    {
    default:
    case BAST_Mode_eDvb_scan:
    case BAST_Mode_eDvb_1_2:
    case BAST_Mode_eDvb_2_3:
    case BAST_Mode_eDvb_3_4:
    case BAST_Mode_eDvb_5_6:
    case BAST_Mode_eDvb_7_8:
        pStatus->mode = NEXUS_FrontendSatelliteMode_eDvb;
        break;

    case BAST_Mode_eDss_scan:
    case BAST_Mode_eDss_1_2:
    case BAST_Mode_eDss_2_3:
    case BAST_Mode_eDss_6_7:
        pStatus->mode = NEXUS_FrontendSatelliteMode_eDss;
        break;

    case BAST_Mode_eDcii_scan:
    case BAST_Mode_eDcii_1_2:
    case BAST_Mode_eDcii_2_3:
    case BAST_Mode_eDcii_3_4:
    case BAST_Mode_eDcii_3_5:
    case BAST_Mode_eDcii_4_5:
    case BAST_Mode_eDcii_5_6:
    case BAST_Mode_eDcii_7_8:
    case BAST_Mode_eDcii_5_11:
        pStatus->mode = NEXUS_FrontendSatelliteMode_eDcii;
        break;

    case BAST_Mode_eLdpc_scan:
    case BAST_Mode_eLdpc_Qpsk_1_2:
    case BAST_Mode_eLdpc_Qpsk_2_3:
    case BAST_Mode_eLdpc_Qpsk_3_4:
    case BAST_Mode_eLdpc_Qpsk_3_5:
    case BAST_Mode_eLdpc_Qpsk_4_5:
    case BAST_Mode_eLdpc_Qpsk_5_6:
    case BAST_Mode_eLdpc_Qpsk_9_10:
    case BAST_Mode_eLdpc_Qpsk_8_9:
        pStatus->mode = NEXUS_FrontendSatelliteMode_eQpskLdpc;
        break;

    case BAST_Mode_eLdpc_8psk_2_3:
    case BAST_Mode_eLdpc_8psk_3_4:
    case BAST_Mode_eLdpc_8psk_3_5:
    case BAST_Mode_eLdpc_8psk_5_6:
    case BAST_Mode_eLdpc_8psk_8_9:
    case BAST_Mode_eLdpc_8psk_9_10:
        pStatus->mode = NEXUS_FrontendSatelliteMode_e8pskLdpc;
        break;

    case BAST_Mode_eTurbo_scan:
    case BAST_Mode_eTurbo_Qpsk_1_2:
    case BAST_Mode_eTurbo_Qpsk_2_3:
    case BAST_Mode_eTurbo_Qpsk_3_4:
    case BAST_Mode_eTurbo_Qpsk_5_6:
    case BAST_Mode_eTurbo_Qpsk_7_8:
        pStatus->mode = NEXUS_FrontendSatelliteMode_eTurboQpsk;
        break;

    case BAST_Mode_eTurbo_8psk_2_3:
    case BAST_Mode_eTurbo_8psk_3_4:
    case BAST_Mode_eTurbo_8psk_4_5:
    case BAST_Mode_eTurbo_8psk_5_6:
    case BAST_Mode_eTurbo_8psk_8_9:
        pStatus->mode = NEXUS_FrontendSatelliteMode_eTurbo8psk;
        break;
    }

    return BERR_SUCCESS;
}

static void NEXUS_Frontend_P_Ast_GetDiseqcSettings( void *handle, NEXUS_FrontendDiseqcSettings *pSettings )
{
    NEXUS_AstDevice *pDevice = handle;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_AstDevice);

    #ifndef TEST_NETWORK_TUNER
    if (pDevice->capabilities.diseqc)
    #endif
    {
        BDBG_ASSERT(NULL != pSettings);
        *pSettings = pDevice->diseqcSettings;
    }
}

static NEXUS_Error NEXUS_Frontend_P_Ast_SetDiseqcSettings( void *handle, const NEXUS_FrontendDiseqcSettings *pSettings )
{
    NEXUS_Error errCode;
    NEXUS_AstDevice *pDevice = handle;
    uint8_t buf[2];
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_FRONTEND_ADD)
	BAST_DiseqcSettings stAstDiseqcSettings;
#endif
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_AstDevice);
    BDBG_ASSERT(NULL != pSettings);


#ifdef NEXUS_POWER_MANAGEMENT
    /* Ensure diseqc is powered on before reading from it.*/
    BAST_PowerUp(pDevice->astChannel, BAST_CORE_DISEQC);
#endif

    if(pDevice->astChip == 4506){
#if defined NEXUS_FRONTEND_4506
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_FRONTEND_ADD)
		/* BCM4506_CONFIG_DISEQC_PRETX_DELAY changes the delay defined below :
		    PreTxDelay, between DiSEqC Msg and TB, after TB */
		buf[0] = pSettings->ucPreTxDelay;
		errCode = BAST_WriteConfig(pDevice->astChannel, BCM4506_CONFIG_DISEQC_PRETX_DELAY, buf, BCM4506_CONFIG_LEN_DISEQC_PRETX_DELAY);
		if (errCode) return BERR_TRACE(errCode);
#endif

        errCode = BAST_ReadConfig(pDevice->astChannel, BCM4506_CONFIG_DISEQC_CTL2, buf, BCM4506_CONFIG_LEN_DISEQC_CTL2);
        if (errCode) return BERR_TRACE(errCode);

        buf[0] &= ~(BCM4506_DISEQC_CTL2_TB_ENABLE | BCM4506_DISEQC_CTL2_TB_B | BCM4506_DISEQC_CTL2_ENVELOPE | BCM4506_DISEQC_CTL2_EXP_REPLY_DISABLE);

        if (pSettings->toneBurst == NEXUS_FrontendDiseqcToneBurst_eUnmodulated) {
            buf[0] |= BCM4506_DISEQC_CTL2_TB_ENABLE;
        }
        else if (pSettings->toneBurst == NEXUS_FrontendDiseqcToneBurst_eNominal) {
            buf[0] |= BCM4506_DISEQC_CTL2_TB_ENABLE | BCM4506_DISEQC_CTL2_TB_B;
        }

        if ( pSettings->toneMode == NEXUS_FrontendDiseqcToneMode_eEnvelope ) {
            buf[0] |= BCM4506_DISEQC_CTL2_ENVELOPE;
        }

        if (pSettings->replyDisabled) {
            buf[0] |= BCM4506_DISEQC_CTL2_EXP_REPLY_DISABLE;
        }

        errCode = BAST_WriteConfig(pDevice->astChannel, BCM4506_CONFIG_DISEQC_CTL2, buf, BCM4506_CONFIG_LEN_DISEQC_CTL2);
        if (errCode) return BERR_TRACE(errCode);
#endif
    }
    else if(pDevice->astChip == 4501){
#if defined NEXUS_FRONTEND_4501
        errCode = BAST_ReadConfig(pDevice->astChannel, BCM4501_CONFIG_DISEQC_CTL2, buf, BCM4501_CONFIG_LEN_DISEQC_CTL2);
        if (errCode) return BERR_TRACE(errCode);

        buf[0] &= ~(BCM4501_DISEQC_CTL2_TB_ENABLE | BCM4501_DISEQC_CTL2_TB_B | BCM4501_DISEQC_CTL2_ENVELOPE | BCM4501_DISEQC_CTL2_EXP_REPLY_DISABLE);

        if (pSettings->toneBurst == NEXUS_FrontendDiseqcToneBurst_eUnmodulated) {
            buf[0] |= BCM4501_DISEQC_CTL2_TB_ENABLE;
        }
        else if (pSettings->toneBurst == NEXUS_FrontendDiseqcToneBurst_eNominal) {
            buf[0] |= BCM4501_DISEQC_CTL2_TB_ENABLE | BCM4501_DISEQC_CTL2_TB_B;
        }

        if ( pSettings->toneMode == NEXUS_FrontendDiseqcToneMode_eEnvelope ) {
            buf[0] |= BCM4501_DISEQC_CTL2_ENVELOPE;
        }

        if (pSettings->replyDisabled) {
            buf[0] |= BCM4501_DISEQC_CTL2_EXP_REPLY_DISABLE;
        }

        errCode = BAST_WriteConfig(pDevice->astChannel, BCM4501_CONFIG_DISEQC_CTL2, buf, BCM4501_CONFIG_LEN_DISEQC_CTL2);
        if (errCode) return BERR_TRACE(errCode);
#endif
    }
    else if(pDevice->astChip == 7346) {
#if defined(NEXUS_FRONTEND_7346)
        BAST_DiseqcSettings diseqcSettings;
        BSTD_UNUSED(buf);
        errCode = BAST_GetDiseqcSettings(pDevice->astChannel, &diseqcSettings);
        if (errCode) return BERR_TRACE(errCode);

        diseqcSettings.bEnableToneburst = ((pSettings->toneBurst == NEXUS_FrontendDiseqcToneBurst_eUnmodulated) || (pSettings->toneBurst == NEXUS_FrontendDiseqcToneBurst_eNominal));
        diseqcSettings.bToneburstB = (pSettings->toneBurst == NEXUS_FrontendDiseqcToneBurst_eNominal);
        diseqcSettings.bEnvelope = (pSettings->toneMode == NEXUS_FrontendDiseqcToneMode_eEnvelope);
        diseqcSettings.bExpectReply = !(pSettings->replyDisabled);

        errCode = BAST_SetDiseqcSettings(pDevice->astChannel, &diseqcSettings);
        if (errCode) return BERR_TRACE(errCode);
#endif
    }
    else {
#if defined(BCM73XX_CONFIG_DISEQC_CTL)
        errCode = BAST_ReadConfig(pDevice->astChannel, BCM73XX_CONFIG_DISEQC_CTL, buf, BCM73XX_CONFIG_LEN_DISEQC_CTL);
        if (errCode) return BERR_TRACE(errCode);

        buf[1] &= ~(BCM73XX_DISEQC_CTL_TONEBURST_ENABLE | BCM73XX_DISEQC_CTL_TONEBURST_B | BCM73XX_DISEQC_CTL_ENVELOPE | BCM73XX_DISEQC_CTL_EXP_REPLY_DISABLE);

        if (pSettings->toneBurst == NEXUS_FrontendDiseqcToneBurst_eUnmodulated) {
            buf[1] |= BCM73XX_DISEQC_CTL_TONEBURST_ENABLE;
        }
        else if (pSettings->toneBurst == NEXUS_FrontendDiseqcToneBurst_eNominal) {
            buf[1] |= BCM73XX_DISEQC_CTL_TONEBURST_ENABLE | BCM73XX_DISEQC_CTL_TONEBURST_B;
        }

        if ( pSettings->toneMode == NEXUS_FrontendDiseqcToneMode_eEnvelope ) {
            buf[1] |= BCM73XX_DISEQC_CTL_ENVELOPE;
        }

        if (pSettings->replyDisabled) {
            buf[1] |= BCM73XX_DISEQC_CTL_EXP_REPLY_DISABLE;
        }

        errCode = BAST_WriteConfig(pDevice->astChannel, BCM73XX_CONFIG_DISEQC_CTL, buf, BCM73XX_CONFIG_LEN_DISEQC_CTL);
        if (errCode) return BERR_TRACE(errCode);

        /*After setting DISEQC_CTL, reset is needed to complete setting*/
        if ((pSettings->toneBurst != pDevice->diseqcSettings.toneBurst) ||
            (pSettings->toneMode!= pDevice->diseqcSettings.toneMode))
        {
            errCode = BAST_ResetDiseqc(pDevice->astChannel, 0);
            if ( errCode )
            {
                return BERR_TRACE(errCode);
            }
        }
#else
    BSTD_UNUSED(buf);
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_FRONTEND_ADD)
		errCode = BAST_GetDiseqcSettings(pDevice->astChannel, &stAstDiseqcSettings);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }

		stAstDiseqcSettings.bToneAlign = true;

        if (pSettings->toneBurst == NEXUS_FrontendDiseqcToneBurst_eUnmodulated)
		{
            stAstDiseqcSettings.bEnableToneburst = true;
			stAstDiseqcSettings.bToneburstB = false;
        }
        else if (pSettings->toneBurst == NEXUS_FrontendDiseqcToneBurst_eNominal)
		{
            stAstDiseqcSettings.bEnableToneburst = true;
			stAstDiseqcSettings.bToneburstB = true;
        }
		else
		{
			stAstDiseqcSettings.bEnableToneburst = false;
		}

        if ( pSettings->toneMode == NEXUS_FrontendDiseqcToneMode_eEnvelope )
		{
            stAstDiseqcSettings.bEnvelope = true;
        }

        if (pSettings->replyDisabled)
		{
            stAstDiseqcSettings.bExpectReply = false;
			stAstDiseqcSettings.bDisableRxOnly = false;
        }

		errCode = BAST_SetDiseqcSettings(pDevice->astChannel, &stAstDiseqcSettings);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
#endif
#endif
    }

    #ifndef TEST_NETWORK_TUNER
    if (pDevice->capabilities.diseqc)
    #endif
    {
        #ifdef TEST_NETWORK_TUNER
        if (pDevice->settings.channelIndex==NEXUS_P_MAX_AST_CHANNELS-1)
        {
            /* For testing external tuner easily.  If we are SDS1 for 7325 or SDS2 for 7335, control diseqc from SDS0 or SDS1, respectively. */
            pDevice = pAstDevice[NEXUS_P_MAX_AST_CHANNELS-2];
        }
        #endif

        errCode = BAST_SetDiseqcTone(pDevice->astChannel, pSettings->toneEnabled);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
        if((pDevice->astChip != 4501) && (pDevice->astChip != 4506)){
            errCode = BAST_EnableDiseqcLnb(pDevice->astChannel, pSettings->lnbEnabled);
            if ( errCode )
            {
                return BERR_TRACE(errCode);
            }
        }

        errCode = BAST_SetDiseqcVoltage(pDevice->astChannel, (pSettings->voltage==NEXUS_FrontendDiseqcVoltage_e18v)?true:false);
        if ( errCode )
        {
            /* Restore original setting */
            (void) BAST_SetDiseqcTone(pDevice->astChannel, pDevice->diseqcSettings.toneEnabled);
            return BERR_TRACE(errCode);
        }

        pDevice->diseqcSettings = *pSettings;
    }

#ifdef NEXUS_POWER_MANAGEMENT
    if (!pDevice->diseqcSettings.enabled) {
        BAST_PowerDown(pDevice->astChannel, BAST_CORE_DISEQC);
    }
#endif

    return BERR_SUCCESS;
}

static NEXUS_Error NEXUS_Frontend_P_Ast_GetDiseqcStatus( void *handle, NEXUS_FrontendDiseqcStatus *pStatus )
{
    NEXUS_AstDevice *pDevice = handle;
    BERR_Code rc;
    uint8_t temp;
    BAST_DiseqcStatus diseqcStatus;

    BDBG_OBJECT_ASSERT(pDevice, NEXUS_AstDevice);
    BKNI_Memset(pStatus, 0, sizeof(*pStatus));

#if !defined NEXUS_FRONTEND_4501
    /* HW requires that BAST_GetDiseqcVoltage be called before BAST_GetDiseqcTone */
    rc = BAST_GetDiseqcVoltage(pDevice->astChannel, &temp);
    if (rc) return BERR_TRACE(rc);
    pStatus->voltage = temp;
#else
    BSTD_UNUSED(temp);
#endif

    rc = BAST_GetDiseqcTone(pDevice->astChannel, &pStatus->toneEnabled);
    if (rc) return BERR_TRACE(rc);

    BDBG_CASSERT((NEXUS_FrontendDiseqcMessageStatus)BAST_DiseqcSendStatus_eBusy == NEXUS_FrontendDiseqcMessageStatus_eBusy);
    rc = BAST_GetDiseqcStatus(pDevice->astChannel, &diseqcStatus);
    if (rc) return BERR_TRACE(rc);
    pStatus->sendStatus = diseqcStatus.status;

    return 0;
}

static NEXUS_Error NEXUS_Frontend_P_Ast_SendDiseqcMessage( void *handle, const uint8_t *pSendData,
    size_t sendDataSize, const NEXUS_CallbackDesc *pSendComplete )
{
    NEXUS_AstDevice *pDevice = handle;
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(pDevice, NEXUS_AstDevice);
    BDBG_ASSERT(NULL != pSendData);
    BDBG_ASSERT(sendDataSize > 0);

    if (pDevice->capabilities.diseqc)
    {
        NEXUS_TaskCallback_Set(pDevice->diseqcAppCallback, pSendComplete);
        errCode = BAST_SendDiseqcCommand(pDevice->astChannel, pSendData, sendDataSize);
        if ( errCode )
        {
            NEXUS_TaskCallback_Set(pDevice->diseqcAppCallback, NULL);
            return BERR_TRACE(errCode);
        }

        return BERR_SUCCESS;
    }
    else
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

static NEXUS_Error NEXUS_Frontend_P_Ast_ResetDiseqc( void *handle, const uint8_t options )
{
    NEXUS_AstDevice *pDevice = handle;
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(pDevice, NEXUS_AstDevice);

    if (pDevice->capabilities.diseqc)
    {
        errCode = BAST_ResetDiseqc(pDevice->astChannel, options);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }

        return BERR_SUCCESS;
    }
    else
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

static NEXUS_Error NEXUS_Frontend_P_Ast_GetDiseqcReply( void *handle, NEXUS_FrontendDiseqcMessageStatus *pStatus, uint8_t *pReplyBuffer,
    size_t replyBufferSize, size_t *pReplyLength )
{
    NEXUS_AstDevice *pDevice = handle;
    BERR_Code errCode;
    BAST_DiseqcStatus status;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_AstDevice);

    BDBG_ASSERT(NULL != pStatus);
    BDBG_ASSERT(NULL != pReplyBuffer);
    BDBG_ASSERT(replyBufferSize > 0);
    BDBG_ASSERT(NULL != pReplyLength);

    NEXUS_TaskCallback_Set(pDevice->diseqcAppCallback, NULL);   /* just to be safe */

    BDBG_CASSERT((NEXUS_FrontendDiseqcMessageStatus)BAST_DiseqcSendStatus_eBusy == NEXUS_FrontendDiseqcMessageStatus_eBusy);
    errCode = BAST_GetDiseqcStatus(pDevice->astChannel, &status);
    *pStatus = status.status;

    if (errCode || status.status != BAST_DiseqcSendStatus_eSuccess) {
        return BERR_TRACE(errCode);
    }

    if (!status.bReplyExpected) {
        status.nReplyBytes = 0; /* force it */
    }

    *pReplyLength = replyBufferSize<status.nReplyBytes?replyBufferSize:status.nReplyBytes;
    if (*pReplyLength) {
        BKNI_Memcpy(pReplyBuffer, status.replyBuffer, *pReplyLength);
    }

    return BERR_SUCCESS;
}

static NEXUS_Error NEXUS_Frontend_P_Ast_SendDiseqcAcw( void *handle, uint8_t codeWord )
{
    NEXUS_Error errCode;
    NEXUS_AstDevice *pDevice = handle;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_AstDevice);

    if (pDevice->capabilities.diseqc)
    {
        errCode = BAST_SendACW(pDevice->astChannel, codeWord);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
    }
    else
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }


    return BERR_SUCCESS;
}

static NEXUS_Error NEXUS_Frontend_P_Ast_GetSoftDecisions(void *handle, NEXUS_FrontendSoftDecision *pDecisions, size_t length)
{
#define TOTAL_AST_SOFTDECISIONS 15  /* What an odd number... */
    int j;
    size_t i;
    NEXUS_Error errCode;
    NEXUS_AstDevice *pDevice = handle;
    int16_t d_i[TOTAL_AST_SOFTDECISIONS], d_q[TOTAL_AST_SOFTDECISIONS];

    BDBG_OBJECT_ASSERT(pDevice, NEXUS_AstDevice);

    for( i=0; i<length; i += TOTAL_AST_SOFTDECISIONS )
    {
        errCode = BAST_GetSoftDecisionBuf(pDevice->astChannel, d_i, d_q);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
        for ( j=0; j<TOTAL_AST_SOFTDECISIONS && i+j<length; j++ )
        {
            pDecisions[i+j].i = d_i[j] * 256 * 2;
            pDecisions[i+j].q = d_q[j] * 256 * 2;
        }
    }

    return BERR_SUCCESS;
}

static NEXUS_Error NEXUS_Frontend_P_Ast_ReadSatelliteConfig( void *handle, unsigned id, void *buffer, unsigned bufferSize )
{
    NEXUS_AstDevice *pDevice = handle;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_AstDevice);
    return BAST_ReadConfig(pDevice->astChannel, id, buffer, bufferSize);
}

static NEXUS_Error NEXUS_Frontend_P_Ast_WriteSatelliteConfig( void *handle, unsigned id, const void *buffer, unsigned bufferSize )
{
    NEXUS_AstDevice *pDevice = handle;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_AstDevice);
    return BAST_WriteConfig(pDevice->astChannel, id, (void*)buffer, bufferSize);
}

static NEXUS_Error NEXUS_Frontend_P_Ast_SatellitePeakscan( void *handle, const NEXUS_FrontendSatellitePeakscanSettings *pSettings )
{
#if defined NEXUS_FRONTEND_73XX || defined NEXUS_FRONTEND_4506 || defined NEXUS_FRONTEND_7346
    NEXUS_AstDevice *pDevice = handle;
    NEXUS_SatellitePeakscanStatus *psStatus = &pDevice->peakscanStatus;
    BERR_Code errCode;
    uint8_t buf[8];

    BDBG_OBJECT_ASSERT(pDevice, NEXUS_AstDevice);

    if (!pDevice->peakscanEvent) {
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }

    if (pSettings->frequency - pSettings->frequencyRange < NEXUS_SATELLITE_PEAKSCAN_MIN_FREQ ||
        pSettings->frequency + pSettings->frequencyRange > NEXUS_SATELLITE_PEAKSCAN_MAX_FREQ) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    NEXUS_TaskCallback_Set(pDevice->peakscanAppCallback, &pSettings->peakscanCallback);

#if BCHP_CHIP ==7342 || BCHP_CHIP==7340 || BCHP_CHIP==7335 || BCHP_CHIP==7325
    /* set BLIND_SCAN_SYM_RATE_MIN configuration parameter */
    buf[0] = (uint8_t)((pSettings->minSymbolRate >> 24) & 0xFF);
    buf[1] = (uint8_t)((pSettings->minSymbolRate >> 16) & 0xFF);
    buf[2] = (uint8_t)((pSettings->minSymbolRate >> 8) & 0xFF);
    buf[3] = (uint8_t)(pSettings->minSymbolRate & 0xFF);
    errCode = BAST_WriteConfig(pDevice->astChannel, PEAK_SCAN_SYM_RATE_MIN, buf, LEN_PEAK_SCAN_SYM_RATE_MIN);
    if (errCode != BERR_SUCCESS) {
        BDBG_ERR(("BAST_WriteConfig(BLIND_SCAN_SYM_RATE_MIN) error %#x. Peak scan (blind scan) not initiated", errCode));
        return BERR_TRACE(errCode);
    }

    /* set BLIND_SCAN_SYM_RATE_MAX configuration parameter */
    buf[0] = (uint8_t)((pSettings->maxSymbolRate >> 24) & 0xFF);
    buf[1] = (uint8_t)((pSettings->maxSymbolRate >> 16) & 0xFF);
    buf[2] = (uint8_t)((pSettings->maxSymbolRate >> 8) & 0xFF);
    buf[3] = (uint8_t)(pSettings->maxSymbolRate & 0xFF);
    errCode = BAST_WriteConfig(pDevice->astChannel, PEAK_SCAN_SYM_RATE_MAX, buf, LEN_PEAK_SCAN_SYM_RATE_MAX);
    if (errCode != BERR_SUCCESS) {
        BDBG_ERR(("BAST_WriteConfig(BLIND_SCAN_SYM_RATE_MAX) error %#x. Peak scan (blind scan) not initiated", errCode));
        return BERR_TRACE(errCode);
    }
#else
  buf[0]=0;
  errCode = BAST_SetPeakScanSymbolRateRange(pDevice->astChannel,  pSettings->minSymbolRate, pSettings->maxSymbolRate );
  if (errCode != BERR_SUCCESS) {
      BDBG_ERR(("BAST_WriteConfig(BLIND_SCAN_SYM_RATE_MAX) error %#x. Peak scan (blind scan) not initiated", errCode));
      return BERR_TRACE(errCode);
  }
#endif

    /* setup status variables */
    psStatus->curFreq = pSettings->frequency - pSettings->frequencyRange;
    psStatus->endFreq = pSettings->frequency + pSettings->frequencyRange;
    psStatus->stepFreq = pSettings->frequencyStep;
    psStatus->symRateCount = 0;
    psStatus->lastSymRate = 0;
    psStatus->maxPeakPower = 0;
    psStatus->maxPeakFreq = 0;
    psStatus->scanFinished = false;
    psStatus->singleScan = (pSettings->frequencyRange==0);

    if (psStatus->singleScan) {
        psStatus->endFreq = psStatus->curFreq+1;
    }

    pDevice->toneSearch = false;
    errCode = BAST_PeakScan(pDevice->astChannel, psStatus->curFreq);
    if (errCode != BERR_SUCCESS) {
        BDBG_ERR(("BAST_PeakScan() error %#x. Peak scan not initiated", errCode));
        return BERR_TRACE(errCode);
    }

    BDBG_MSG(("Peak scan (blind scan) started at %uHz", psStatus->curFreq));

    /* the state machine is driven by pDevice->peakscanEventCallback and NEXUS_Frontend_P_Ast_PeakscanEventHandler */
    return NEXUS_SUCCESS;

#else /* defined NEXUS_FRONTEND_73XX || defined NEXUS_FRONTEND_4506 */
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);
    return NEXUS_NOT_SUPPORTED;
#endif
}

static NEXUS_Error NEXUS_Frontend_P_Ast_GetSatellitePeakscanResult( void *handle, NEXUS_FrontendSatellitePeakscanResult *pResult )
{
    NEXUS_AstDevice *pDevice = handle;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_AstDevice);

    if (pDevice->peakscanEvent) {
        if (pDevice->peakscanStatus.scanFinished) {
            pResult->peakFrequency = pDevice->peakscanStatus.maxPeakFreq;
            pResult->symbolRate = pDevice->peakscanStatus.lastSymRate;
            pResult->lastFrequency= pDevice->peakscanStatus.curFreq;
            pResult->peakPower = pDevice->peakscanStatus.maxPeakPower;
            return NEXUS_SUCCESS;
        }
        else {
            return BERR_TRACE(NEXUS_NOT_INITIALIZED);
        }
    }
    else {
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }
}

static NEXUS_Error NEXUS_Frontend_P_Ast_SatelliteToneSearch( void *handle, const NEXUS_FrontendSatelliteToneSearchSettings *pSettings )
{
#if defined NEXUS_FRONTEND_73XX || defined NEXUS_FRONTEND_4506 || defined NEXUS_FRONTEND_7346
    NEXUS_AstDevice *pDevice = handle;
    NEXUS_SatellitePeakscanStatus *psStatus = &pDevice->peakscanStatus;
    BERR_Code errCode;
    uint8_t buf[8];

    BDBG_OBJECT_ASSERT(pDevice, NEXUS_AstDevice);

    if (!pDevice->peakscanEvent) {
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }

    NEXUS_TaskCallback_Set(pDevice->peakscanAppCallback, &pSettings->completionCallback);

    buf[0]=0;
    errCode = BAST_SetPeakScanSymbolRateRange(pDevice->astChannel, 0, 0); /* tone search uses 0,0 */
    if (errCode != BERR_SUCCESS) {
        BDBG_ERR(("BAST_WriteConfig() error %#x. Peak scan (tone search) not initiated", errCode));
        return BERR_TRACE(errCode);
    }

    /* setup status variables */
    psStatus->curFreq = pSettings->frequency - pSettings->frequencyRange;
    psStatus->endFreq = pSettings->frequency + pSettings->frequencyRange;
    psStatus->symRateCount = 0;
    psStatus->lastSymRate = 0;
    psStatus->maxPeakPower = 0;
    psStatus->minPeakPower = 0xFFFFFFFF;
    psStatus->maxPeakFreq = 0;
    psStatus->scanFinished = false;
    psStatus->minRatio.numerator = pSettings->minRatio.numerator;
    psStatus->minRatio.denominator = pSettings->minRatio.denominator;
    psStatus->singleScan = (pSettings->frequencyRange==0);

    {
        /* calculate the frequency step */
        BAST_ChannelStatus status;
        uint32_t fs;
        uint64_t p, q;
        BERR_Code rc;
        rc = BAST_GetChannelStatus(pDevice->astChannel, &status);
#define DFT_SIZE (512)
        fs = status.sample_clock;
        p = ((uint64_t)fs) * ((uint64_t)512);
        q = p / ((uint64_t)DFT_SIZE * (uint64_t)64);
        psStatus->binsize = ((int32_t)q)>>1; /* binsize is scaled 2^8 */
#undef DFT_SIZE
        psStatus->stepFreq = psStatus->binsize;
    }

    if (psStatus->singleScan) {
        psStatus->endFreq = psStatus->curFreq+1;
    }

    pDevice->toneSearch = true;
    errCode = BAST_PeakScan(pDevice->astChannel, psStatus->curFreq);
    if (errCode != BERR_SUCCESS) {
        BDBG_ERR(("BAST_PeakScan() error %#x. Peak scan (tone search) not initiated", errCode));
        return BERR_TRACE(errCode);
    }

    BDBG_MSG(("Peak scan (tone search) started at %uHz", psStatus->curFreq));

    /* the state machine is driven by pDevice->peakscanEventCallback and NEXUS_Frontend_P_Ast_PeakscanEventHandler */
    return NEXUS_SUCCESS;
#else /* defined NEXUS_FRONTEND_73XX || defined NEXUS_FRONTEND_4506 */
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);
    return NEXUS_NOT_SUPPORTED;
#endif
}

static NEXUS_Error NEXUS_Frontend_P_Ast_GetSatelliteToneSearchResult( void *handle, NEXUS_FrontendSatelliteToneSearchResult *pResult )
{
    NEXUS_AstDevice *pDevice = handle;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_AstDevice);

    if (pDevice->peakscanEvent) {
        if (pDevice->peakscanStatus.scanFinished) {
            pResult->peakFrequency = pDevice->peakscanStatus.maxPeakFreq;
            pResult->lastFrequency= pDevice->peakscanStatus.curFreq;
            pResult->peakPower = pDevice->peakscanStatus.maxPeakPower;
            pResult->frequencyStep = pDevice->peakscanStatus.stepFreq;
            return NEXUS_SUCCESS;
        }
        else {
            return BERR_TRACE(NEXUS_NOT_INITIALIZED);
        }
    }
    else {
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }
}

static void NEXUS_Frontend_P_Ast_ResetStatus( void *handle )
{
    NEXUS_AstDevice *pDevice = handle;
    BAST_ResetStatus(pDevice->astChannel);
}

BAST_ChannelHandle NEXUS_Frontend_P_Ast_GetChannel( NEXUS_FrontendHandle handle )
{
    NEXUS_AstDevice *pDevice;

    BDBG_OBJECT_ASSERT(handle, NEXUS_Frontend);
    if (handle->pParentFrontend) {
        handle = handle->pParentFrontend;
        BDBG_OBJECT_ASSERT(handle, NEXUS_Frontend);
    }
    pDevice = handle->pDeviceHandle;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_AstDevice);

    return pDevice->astChannel;
}

static void NEXUS_Frontend_P_Ast_LockEventHandler(void *pParam)
{
    NEXUS_AstDevice *pDevice = pParam;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_AstDevice);
    BDBG_MSG(("AST Lock Event"));

    BTRC_TRACE(ChnChange_TuneLock, STOP);
    NEXUS_TaskCallback_Fire(pDevice->lockAppCallback);
}

static void NEXUS_Frontend_P_Ast_DiseqcEventHandler(void *pParam)
{
    NEXUS_AstDevice *pDevice = pParam;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_AstDevice);
    BDBG_MSG(("AST Diseqc Event"));

    NEXUS_TaskCallback_Fire(pDevice->diseqcAppCallback);
}

static void NEXUS_Frontend_P_Ast_PeakscanEventHandler(void *pParam)
{
    NEXUS_AstDevice *pDevice = pParam;
    BAST_PeakScanStatus astStatus;
    NEXUS_SatellitePeakscanStatus *psStatus = &pDevice->peakscanStatus;
    BERR_Code errCode;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_AstDevice);

    if (psStatus->curFreq > psStatus->endFreq) {
        BDBG_WRN(("curFreq: %d > endFreq: %d, exiting scan",psStatus->curFreq,psStatus->endFreq));
        goto done;
    }

    if (pDevice->toneSearch) {

        /* get results of tone search scan */
        errCode = BAST_GetPeakScanStatus(pDevice->astChannel, &astStatus);
        if (errCode != BERR_SUCCESS) {
            BDBG_ERR(("BAST_GetPeakScanStatus() (tone search) error %#x", errCode));
            errCode = BERR_TRACE(errCode);
            goto tone_step;
        }

        if (astStatus.status != 0) {
            BDBG_ERR(("BAST_GetPeakScanStatus() (tone search) scan status error %u", astStatus.status));
            goto tone_step;
        }

        BDBG_MSG(("%d Hz: Fb=%d, power=%#x, step=%#d, min=%#x, max=%#x", astStatus.tunerFreq, astStatus.out, astStatus.peakPower, psStatus->stepFreq, psStatus->minPeakPower, psStatus->maxPeakPower));

        if (astStatus.peakPower > psStatus->maxPeakPower) { /* track min and max over the frequency range */
            psStatus->maxPeakPower = astStatus.peakPower;
            psStatus->maxPeakFreq = astStatus.tunerFreq;
            psStatus->maxPeakIndex = astStatus.out;
        }
        if (astStatus.peakPower < psStatus->minPeakPower) {
            psStatus->minPeakPower = astStatus.peakPower;
        }
        if (pDevice->peakscanStatus.singleScan) {
            psStatus->maxPeakIndex = astStatus.out;
            goto tone_done;
        }

tone_step:
        psStatus->curFreq += psStatus->stepFreq;

        if (psStatus->curFreq <= psStatus->endFreq) {
            errCode = BAST_PeakScan(pDevice->astChannel, psStatus->curFreq);
            if (errCode != BERR_SUCCESS) {
                BDBG_ERR(("BAST_PeakScan() (tone search) error %#x. Peak scan terminated", errCode));
                errCode = BERR_TRACE(errCode);
                goto done;
            }
            else {
                return;
            }
        }
        else {
#if 0
            if (psStatus->symRateCount > 0) {
                BDBG_WRN(("Potential signal found at %d Hz (%d sym/sec), but reached end of scan range",
                    psStatus->maxPeakFreq, psStatus->lastSymRate));
            }
            else {
                BDBG_WRN(("No signal found using peak scan"));
                psStatus->maxPeakFreq = 0;
                psStatus->lastSymRate = 0;
            }
#endif
            psStatus->curFreq -= psStatus->stepFreq;
        }
tone_done:
        /* calculate ratio */
        {
            uint64_t ratio, target_ratio;
#define TONE_SEARCH_RATIO_SCALE (1000)
            uint64_t max_pow = ((uint64_t)psStatus->maxPeakPower)*TONE_SEARCH_RATIO_SCALE;
            uint64_t min_pow = ((uint64_t)psStatus->minPeakPower);
            target_ratio = (((uint64_t)psStatus->minRatio.numerator)*TONE_SEARCH_RATIO_SCALE) / ((uint64_t)psStatus->minRatio.denominator);
            ratio = max_pow / min_pow;

            BDBG_ERR(("target_ratio: %lld, min=%#llx, max=%#llx min=%#lx, max=%#lx", target_ratio, min_pow, max_pow, psStatus->minPeakPower, psStatus->maxPeakPower));
            if (max_pow > (min_pow * target_ratio)) {
                uint32_t binsize; /* 2^8 scale */
                uint64_t p, q;

                binsize = psStatus->binsize;
                p = ((uint64_t)psStatus->maxPeakIndex) * ((uint64_t)binsize);
                q = p / (uint64_t)256;
                psStatus->maxPeakFreq += (uint32_t)q;
            } else {
                psStatus->maxPeakFreq = 0;
                psStatus->lastSymRate = 0;
            }
        }

    } else {

        /* get results of symbol rate scan */
        errCode = BAST_GetPeakScanStatus(pDevice->astChannel, &astStatus);
        if (errCode != BERR_SUCCESS) {
            BDBG_ERR(("BAST_GetPeakScanStatus() (blind scan) error %#x", errCode));
            errCode = BERR_TRACE(errCode);
            goto blind_step;
        }

        if (astStatus.status != 0) {
            BDBG_ERR(("BAST_GetPeakScanStatus() (blind scan) scan status error %u", astStatus.status));
            goto blind_step;
        }

        BDBG_MSG(("%d Hz: Fb=%d, power=%#x", astStatus.tunerFreq, astStatus.out, astStatus.peakPower));

        if (astStatus.out == psStatus->lastSymRate) { /* same symbol rate as last scan */
            psStatus->symRateCount++;
            if (psStatus->maxPeakPower < astStatus.peakPower) {
                psStatus->maxPeakPower = astStatus.peakPower;
                psStatus->maxPeakFreq = astStatus.tunerFreq;
            }
            if (pDevice->peakscanStatus.singleScan) {
                psStatus->lastSymRate = astStatus.out;
                goto done;
            }
        }
        else {
            if (psStatus->symRateCount > 0) {
                BDBG_MSG(("Potential signal found at %d Hz (%d sym/sec)", psStatus->maxPeakFreq, psStatus->lastSymRate));
                goto done;
            }
            else {
                /* save new symbol rate */
                psStatus->symRateCount = 0;
                psStatus->lastSymRate = astStatus.out;
                psStatus->maxPeakPower = astStatus.peakPower;
                psStatus->maxPeakFreq = astStatus.tunerFreq;
                if (pDevice->peakscanStatus.singleScan) {
                    goto done;
                }
            }
        }

blind_step:
        psStatus->curFreq += psStatus->stepFreq;

        if (psStatus->curFreq < psStatus->endFreq) {
            errCode = BAST_PeakScan(pDevice->astChannel, psStatus->curFreq);
            if (errCode != BERR_SUCCESS) {
                BDBG_ERR(("BAST_PeakScan() (blind scan) error %#x. Peak scan terminated", errCode));
                errCode = BERR_TRACE(errCode);
                goto done;
            }
            else {
                return;
            }
        }
        else {
            if (psStatus->symRateCount > 0) {
                BDBG_WRN(("Potential signal found at %d Hz (%d sym/sec), but reached end of scan range",
                    psStatus->maxPeakFreq, psStatus->lastSymRate));
            }
            else {
                BDBG_WRN(("No signal found using peak scan"));
                psStatus->maxPeakFreq = 0;
                psStatus->lastSymRate = 0;
            }
            psStatus->curFreq -= psStatus->stepFreq;
            goto done;
        }
    }

done:
    psStatus->scanFinished = true;
    NEXUS_TaskCallback_Fire(pDevice->peakscanAppCallback);

    return;
}

static NEXUS_Error NEXUS_Frontend_P_Ast_RegisterExtension(NEXUS_FrontendHandle parentHandle, NEXUS_FrontendHandle extensionHandle)
{
    NEXUS_AstDevice *pDevice;

    BDBG_OBJECT_ASSERT(parentHandle, NEXUS_Frontend);
    pDevice = parentHandle->pDeviceHandle;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_AstDevice);

    if (extensionHandle == NULL) {
        extensionHandle = parentHandle;
    }
    else {
        BDBG_OBJECT_ASSERT(extensionHandle, NEXUS_Frontend);
    }

    /* recreate callbacks with the extension handle. this allows NEXUS_StopCallbacks to work. */
    if (pDevice->diseqcAppCallback) {
        NEXUS_TaskCallback_Destroy(pDevice->diseqcAppCallback);
        pDevice->diseqcAppCallback = NEXUS_TaskCallback_Create(extensionHandle, NULL);
        if ( NULL == pDevice->diseqcAppCallback ) {
            return BERR_TRACE(BERR_OS_ERROR);
        }
    }
    if (pDevice->peakscanAppCallback) {
        NEXUS_TaskCallback_Destroy(pDevice->peakscanAppCallback);
        pDevice->peakscanAppCallback = NEXUS_TaskCallback_Create(extensionHandle, NULL);
        if ( NULL == pDevice->peakscanAppCallback ) {
            return BERR_TRACE(BERR_OS_ERROR);
        }
    }
    if (pDevice->lockAppCallback) {
        NEXUS_TaskCallback_Destroy(pDevice->lockAppCallback);
        pDevice->lockAppCallback = NEXUS_TaskCallback_Create(extensionHandle, NULL);
        if ( NULL == pDevice->lockAppCallback ) {
            return BERR_TRACE(BERR_OS_ERROR);
        }
    }
    if (pDevice->ftmCallback) {
        NEXUS_TaskCallback_Destroy(pDevice->ftmCallback);
        pDevice->ftmCallback = NEXUS_TaskCallback_Create(extensionHandle, NULL);
        if ( NULL == pDevice->ftmCallback ) {
            return BERR_TRACE(BERR_OS_ERROR);
        }
    }

    return 0;
}

/**
The following public API functions are implemented for AST only.
This means that a combo AST/SDS system will no longer work.
**/

NEXUS_AstDevice *NEXUS_Frontend_P_GetAstDeviceByChip(NEXUS_FrontendHandle handle, unsigned chipId)
{
    NEXUS_AstDevice *pDevice;

    BDBG_OBJECT_ASSERT(handle, NEXUS_Frontend);
    if (handle->pParentFrontend) {
        handle = handle->pParentFrontend;
        BDBG_OBJECT_ASSERT(handle, NEXUS_Frontend);
    }
    if (handle->chip.id == chipId) {
        pDevice = handle->pDeviceHandle;
        BDBG_OBJECT_ASSERT(pDevice, NEXUS_AstDevice);
        return pDevice;
    }
    return NULL;
}

static NEXUS_Error NEXUS_Frontend_P_Ast_GetFastStatus(void *handle, NEXUS_FrontendFastStatus *pStatus )
{
    NEXUS_AstDevice *pDevice = handle;
    bool isLocked = false;
    BERR_Code rc;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_AstDevice);
    BDBG_ASSERT(pStatus);
    pStatus->lockStatus = NEXUS_FrontendLockStatus_eUnknown;
    rc = BAST_GetLockStatus(pDevice->astChannel, &isLocked);
    if (rc) {
        return BERR_TRACE(NEXUS_NOT_AVAILABLE);
    }
    /* AST only returns locked/unlocked, there is currently no "no signal" status. */
    pStatus->lockStatus = isLocked ? NEXUS_FrontendLockStatus_eLocked : NEXUS_FrontendLockStatus_eUnlocked;
    return NEXUS_SUCCESS;
}

static NEXUS_Error NEXUS_Frontend_P_Ast_ReadRegister(void *handle, unsigned address, uint32_t *pValue)
{
    NEXUS_AstDevice *astDevice = handle;
    BDBG_OBJECT_ASSERT(astDevice, NEXUS_AstDevice);
    if (astDevice) {
        return BAST_ReadRegister(astDevice->astChannel, address, pValue);
    }
    else {
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }
}

static NEXUS_Error NEXUS_Frontend_P_Ast_WriteRegister(void *handle, unsigned address, uint32_t value)
{
    NEXUS_AstDevice *astDevice = handle;
    BDBG_OBJECT_ASSERT(astDevice, NEXUS_AstDevice);
    if (astDevice) {
        return BAST_WriteRegister(astDevice->astChannel, address, &value);
    }
    else {
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }
}

/***************************************************************************
 * Module callback functions for I2C -- I2C Auto-synchronizes these.
 ***************************************************************************/
static BERR_Code NEXUS_Frontend_P_Ast_I2cReadNoAddr(void * context, uint16_t chipAddr, uint8_t *pData, size_t length)
{
    NEXUS_AstDevice *pDevice = context;
    uint8_t dummy;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_AstDevice);
    return BAST_ReadMi2c(pDevice->astChannel, chipAddr<<1, &dummy, 0, pData, length);
}

static BERR_Code NEXUS_Frontend_P_Ast_I2cWriteNoAddr(void * context, uint16_t chipAddr, const uint8_t *pData, size_t length)
{
    NEXUS_AstDevice *pDevice = context;
    BDBG_OBJECT_ASSERT(pDevice, NEXUS_AstDevice);
    return BAST_WriteMi2c(pDevice->astChannel, chipAddr<<1, (uint8_t *)((unsigned long)pData), length);
}

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_FRONTEND_ADD)
NEXUS_Error NEXUS_Frontend_P_Ast_SetStuffBytes(
    void *handle,
    uint8_t *ucStuffByte
)
{
	BERR_Code etBErrorCode = BERR_SUCCESS;
    	NEXUS_AstDevice *pDevice = handle;

	BDBG_OBJECT_ASSERT(pDevice, NEXUS_AstDevice);

	BDBG_MSG(( "NEXUS_Frontend_P_Ast_SetStuffBytes : [0x%x] \n", *ucStuffByte));

#if defined(CONFIG_PLATFORM_DB4506)
	etBErrorCode = BAST_WriteConfig(pDevice->astChannel,
									BCM4506_CONFIG_STUFF_BYTES ,
									ucStuffByte,
									BCM4506_CONFIG_LEN_STUFF_BYTES);
#else
	etBErrorCode = BAST_WriteConfig(pDevice->astChannel,
									BAST_G3_CONFIG_STUFF_BYTES ,
									ucStuffByte,
									BAST_G3_CONFIG_LEN_STUFF_BYTES);
#endif

	if(etBErrorCode != BERR_SUCCESS)
	{
		BDBG_ERR(( "NEXUS_Frontend_P_Ast_SetStuffBytes : Error Code(0x%x) \n", etBErrorCode));
        	return BERR_TRACE(BERR_NOT_SUPPORTED);
	}

	return BERR_SUCCESS;
}


NEXUS_Error NEXUS_Frontend_P_Ast_SetXportCtl(
    void *handle,
    bool bClkSup
)
{
	BERR_Code etBErrorCode = BERR_SUCCESS;
    	NEXUS_AstDevice *pDevice = handle;
	BAST_OutputTransportSettings stXportSettings;

	BDBG_OBJECT_ASSERT(pDevice, NEXUS_AstDevice);

	etBErrorCode = BAST_GetOutputTransportSettings(pDevice->astChannel, &stXportSettings);
	if(etBErrorCode != BERR_SUCCESS)
	{
		BDBG_ERR(( "NEXUS_Frontend_P_Ast_SetXportCtl : Error Code(0x%x) \n", etBErrorCode));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
	}

	stXportSettings.bClkSup = bClkSup;

	etBErrorCode = BAST_SetOutputTransportSettings(pDevice->astChannel, &stXportSettings);
	if(etBErrorCode != BERR_SUCCESS)
	{
		BDBG_ERR(( "NEXUS_Frontend_P_Ast_SetXportCtl : Error Code(0x%x) \n", etBErrorCode));
        	return BERR_TRACE(BERR_NOT_SUPPORTED);
	}

	return BERR_SUCCESS;
}

NEXUS_Error NEXUS_Frontend_P_Ast_SetTsOutput(
    void *handle,
    bool bEnableOutput
)
{
	BERR_Code etBErrorCode = BERR_SUCCESS;
    	NEXUS_AstDevice *pDevice = handle;

#if defined(CONFIG_PLATFORM_DB4506)
	uint32_t	ulReadBuf = 0;
	uint32_t	ulWriteBuf = 0;
#endif

	BDBG_OBJECT_ASSERT(pDevice, NEXUS_AstDevice);

#if defined(CONFIG_PLATFORM_DB4506)
	if(bEnableOutput == false)  /*Disable TS output */
	{
		BDBG_MSG(( "Disable TS output ! \n"));
		etBErrorCode = BAST_ReadRegister(pDevice->astChannel,BCM4506_TM_SDS_XPT_DRIVE_CTRL,&ulReadBuf);

		if (pDevice->astChannel->channel == 0)
			ulWriteBuf = (ulReadBuf & ~SDS0_PKTP_MASK) | SDS0_PKTP_DISABLE;
		else
			ulWriteBuf = (ulReadBuf & ~SDS1_PKTP_MASK) | SDS1_PKTP_DISABLE;

		etBErrorCode = BAST_WriteRegister(pDevice->astChannel,BCM4506_TM_SDS_XPT_DRIVE_CTRL,&ulWriteBuf);
	}
	else
	{
		BDBG_MSG(( "Enable TS output ! \n"));

		etBErrorCode = BAST_ReadRegister(pDevice->astChannel,BCM4506_TM_SDS_XPT_DRIVE_CTRL,&ulReadBuf);

		if (pDevice->astChannel->channel == 0)
			ulWriteBuf = (ulReadBuf & ~SDS0_PKTP_MASK) | SDS0_PKTP_ENABLE;
		else
			ulWriteBuf = (ulReadBuf & ~SDS1_PKTP_MASK) | SDS1_PKTP_ENABLE;

		etBErrorCode = BAST_WriteRegister(pDevice->astChannel,BCM4506_TM_SDS_XPT_DRIVE_CTRL,&ulWriteBuf);
	}
#else
	if(bEnableOutput == false)  /*Disable TS output */
	{
		BDBG_MSG(( "Disable TS output ! \n"));
		etBErrorCode = BAST_g3_P_SdsDisableOif(pDevice->astChannel);
	}
	else
	{
		BDBG_MSG(( "Enable TS output ! \n"));
	}
#endif

	if(etBErrorCode != BERR_SUCCESS)
	{
		BDBG_ERR(( "NEXUS_Frontend_P_Ast_SetTsOutput : Error Code(0x%x) \n", etBErrorCode));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
	}

	return BERR_SUCCESS;
}
#else
NEXUS_Error NEXUS_Frontend_P_Ast_SetStuffBytes(
    void *handle,
    uint8_t *ucStuffByte
)
{
	return BERR_SUCCESS;
}

NEXUS_Error NEXUS_Frontend_P_Ast_SetXportCtl(
    void *handle,
    bool bClkSup
)
{
	return BERR_SUCCESS;
}

NEXUS_Error NEXUS_Frontend_P_Ast_SetTsOutput(
    void *handle,
    bool bEnableOutput
)
{
	return BERR_SUCCESS;
}
#endif

