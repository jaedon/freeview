/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhdm_hdcp.c $
 * $brcm_Revision: Hydra_Software_Devel/103 $
 * $brcm_Date: 4/30/12 4:46p $
 *
 * Module Description:
 * This module implements HDCP functionality contained in the HDCP Spec 
 * Version 1.1 (www.digital-cp.com) for HDMI/DVI transmiters and 
 * receivers.  
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hdm/7038/bhdm_hdcp.c $
 * 
 * Hydra_Software_Devel/103   4/30/12 4:46p vle
 * SW7425-2666: Merge to mainline
 * 
 * Hydra_Software_Devel/SW7425-2666_FifoTest/3   4/19/12 10:30a rgreen
 * SW7425-2666: Update FIFO recenter algorithm.  Move valid video check to
 * HDCP State machine
 * 
 * Hydra_Software_Devel/SW7425-2666_FifoTest/2   3/27/12 3:51p rgreen
 * SW7425-2666:  Add/branch test code for FIFO debugging; remove 2nd copy
 * of test function
 * 
 * Hydra_Software_Devel/SW7425-2666_FifoTest/1   3/26/12 6:52p rgreen
 * SW7425-2666:  Add/branch test code for FIFO debugging
 * 
 * Hydra_Software_Devel/102   3/26/12 3:56p rgreen
 * SW7425-2666: Recenter FIFO if fullness is off 6 instead of 8
 * 
 * Hydra_Software_Devel/101   3/22/12 4:04p rgreen
 * SW7425-2666: Update algorithm for checking for valid video into the
 * HDMI core
 * 
 * Hydra_Software_Devel/100   1/3/12 3:27p vle
 * SW7358-203: Merged to mainline.
 * 
 * Hydra_Software_Devel/bdvd_v4.0/2   12/19/11 1:52p rbshah
 * SWBLURAY-26245:[ see Broadcom Issue Tracking JIRA for more info ]
 * 
 * Hydra_Software_Devel/99   11/14/11 2:15p rgreen
 * SW7425-1710: Update BHDM_CONFIG macro usage.  Describe specific
 * functionality vs chip process
 * 
 * Hydra_Software_Devel/98   10/23/11 12:22p rgreen
 * SW7425-1583:  Add debug warning if a Test Key Set is used for
 * authentication
 * 
 * Hydra_Software_Devel/97   10/11/11 4:50p vle
 * SW7429-5: Add support for 7429.
 * 
 * Hydra_Software_Devel/96   9/30/11 6:21p vle
 * SW7408-296: Fix coverity issue
 * 
 * Hydra_Software_Devel/95   9/2/11 4:56p vle
 * SW7403-932: Change default values of Ri4SecsAgo and Ri6SecsAgo to avoid
 * rare case where Ri event is never fired when R0 = 0x01
 * 
 * Hydra_Software_Devel/94   6/24/11 11:59a vle
 * SW7422-457: Merged bdvd updates to mainline.
 * 
 * Hydra_Software_Devel/bdvd_v3.0/bdvd_v4.0/3   6/17/11 3:23p rbshah
 * SWBLURAY-26245:[ see Broadcom Issue Tracking JIRA for more info ]
 * 
 * Hydra_Software_Devel/93   11/24/10 3:28p calvinho
 * SW7401-4468: Update HDCP version definition
 * 
 * Hydra_Software_Devel/92   11/11/10 4:07p calvinho
 * SW7401-4370: Separate BHDM_HDCP_RX_AINFO write into a separate function
 * 
 * Hydra_Software_Devel/91   9/22/10 10:42a vle
 * SW7335-837: Be sure to always validate RxBksv
 * 
 * Hydra_Software_Devel/90   4/29/10 11:56a vle
 * SW7342-110: Fix HDCP compliance test issue (test with repeater) for
 * 7340/7342 and other big endian system
 * 
 * Hydra_Software_Devel/89   4/7/10 3:54p vle
 * SW7208-20, SW7342-110: fix DRIFT_FIFO underflow/overflow errors for
 * HDCP compliance test 1A-07.
 * 
 * Hydra_Software_Devel/88   3/24/10 11:47a vle
 * SW7601-171: Add BI2C_P_SetupHdmiHwAccess to set up I2C for HDMI HDCP
 * auto Ri/Pj link integrity check feature
 * 
 * Hydra_Software_Devel/SW7601-171/2   3/10/10 6:39p vle
 * SW7400-2713: Fix coverity issue
 * 
 * Hydra_Software_Devel/SW7601-171/1   3/9/10 3:22p vle
 * SW7601-171: Add BI2C_P_SetupHdmiHwAccess to set up I2C for HDMI HDCP
 * auto Ri/Pj link integrity check feature
 * 
 * Hydra_Software_Devel/87   2/26/10 5:18p vle
 * SW7405-3016: Remove software i2c settings from HDM PI. This mode is
 * configure in I2C module
 * 
 * Hydra_Software_Devel/86   2/22/10 5:10p vle
 * SW7335-669: add BHDM_HDCP_RX_BKSV_I2C_READ_ERROR and
 * BHDM_HDCP_TX_AKSV_I2C_WRITE_ERROR errors constant.
 * 
 * Hydra_Software_Devel/85   12/21/09 4:43p vle
 * SW7405-3559: Disable Pj checking by default.
 * 
 * Hydra_Software_Devel/84   11/16/09 2:50p rgreen
 * SW7405-3409: Fix kernel mode compilation warnings
 * 
 * Hydra_Software_Devel/83   11/13/09 3:47p jrubio
 * SW7342-76: Make HMDI info functions DEBUG only
 * 
 * Hydra_Software_Devel/82   9/28/09 4:29p vle
 * SW7601-167: Merge fixes from bdvd_v3.0 branch to main branch.
 * Fix build issue for platforms without auto Ri/Pj checking feature.
 * 
 * Hydra_Software_Devel/81   9/24/09 6:04p vle
 * SW7601-167: Merge fixes from bdvd_v3.0 branch to main branch.
 * Fix build issue from an incorrect check in.
 * 
 * Hydra_Software_Devel/80   9/24/09 4:53p vle
 * SW7601-167: Merge fixes from bdvd_v3.0 branch to main branch.
 * 
 * Hydra_Software_Devel/bdvd_v3.0/3   9/8/09 3:18p rbshah
 * PR16468[DVD]:[ see HiDef-DVD bug tracking system for more info ].  Fix
 * merge issue with HDCP not coming up on AVRs. Also allow Auto HW Pj
 * checking to be enabled if so desired by the application.
 * 
 * Hydra_Software_Devel/bdvd_v3.0/2   9/2/09 3:31p rbshah
 * PR16468[DVD]:[ see HiDef-DVD bug tracking system for more info ].
 * Merged with the latest portinginterface/hdm and syslib/hdcplib files.
 * 
 * Hydra_Software_Devel/bdvd_v2.0/bdvd_v2.1/1   7/14/09 1:30p rbshah
 * PR_15413[DVD]:[ see HiDef-DVD bug tracking system for more info ].
 * Merge work from the HDMI certification branch. Also addresses PR15437
 * and PR15220 (merged from v2.0). Plus coverity PR15782.
 * 
 * Hydra_Software_Devel/bdvd_v2.0/bdvd_hdmi_cert_v2.1/2   7/9/09 9:42p rbshah
 * Don't retry R0 I2C read -- Simplay hack
 * 
 * Hydra_Software_Devel/bdvd_v2.0/bdvd_hdmi_cert_v2.1/1   7/8/09 12:18p rbshah
 * Various fixes for Auto Hardware Ri,Pj checking. I2C changes are
 * temporary. Switch from software to hardware Ri checking by default.
 * 
 * Hydra_Software_Devel/79   7/17/09 7:47a vle
 * PR56896: Fix build issue involving BHDM_HDCP_P_CheckForValidVideo
 * 
 * Hydra_Software_Devel/78   7/14/09 11:28a rgreen
 * PR56776: Prevent HDCP An Timeout; Reduce debug messages
 * 
 * Hydra_Software_Devel/77   6/24/09 5:04p vle
 * PR56341: Fix coverity issue
 * 
 * Hydra_Software_Devel/76   5/12/09 2:11a vle
 * PR 54851: Make sure to disable HDCP 1.1 only when the Pj link failures
 * occur consecutively.
 * 
 * Hydra_Software_Devel/75   5/5/09 5:14p vle
 * PR 54851: Implement a dynamic mode in the PI where HDCP 1.1 feature
 * will be disabled when a pre-define occurance of Pj Link Failures is
 * met.
 * 
 * Hydra_Software_Devel/74   3/3/09 8:23p vle
 * PR50569: Add HW Ri/Pj checking feature. Merged from bdvd branch after
 * Rajul's testing effort.
 * 
 * Hydra_Software_Devel/73   2/24/09 8:29p vle
 * PR51272: Merge to main branch
 * 
 * Hydra_Software_Devel/PR51272/1   1/30/09 3:49p vle
 * PR51272: Make sure to asset I_AUTH_REQUEST after each frame while An is
 * not ready. This will help prevent An generation timeout issue.
 *  
 * Hydra_Software_Devel/bdvd_v2.0/2   1/23/09 10:13a rbshah
 * PR_10346 [ see HiDef-DVD bug tracking system for more info ].  Checkin
 * code drop from Anthony Le for Auto Ri,Pj feature in the
 * 7601B0 (Digital Video PR50569).
 * 
 * This is disabled by default and will be turned on once it has
 * been tested and soaked.
 * 
 * Did verify the A0 build!
 * 
 * Hydra_Software_Devel/72   12/18/08 6:03p vle
 * PR48987: HDCP errors should be considered as error and warning messages
 * 
 * Hydra_Software_Devel/71   12/5/08 2:27p vle
 * PR 48987: Change variable name for better description
 * 
 * Hydra_Software_Devel/70   12/2/08 8:01p vle
 * PR48987: Merge to main branch - add run time option to enable I2C bit
 * bang mode.
 * 
 * Hydra_Software_Devel/HDMI_TX_Plugfest11_200811/1   11/13/08 7:33p vle
 * PR 48987: Add run time option to enable I2C bit bang mode.
 * 
 * Hydra_Software_Devel/69   10/22/08 6:01p rgreen
 * PR48233: Fix Coverity Issue evaluation order
 * 
 * Hydra_Software_Devel/68   10/10/08 10:38a rgreen
 * PR47781:Fix erroneous abort message for Authentications after hot plug
 * 
 * Hydra_Software_Devel/67   9/10/08 11:56a vle
 * PR 45988: ENC_EN/ENC_DIS signal can be generated by hardware on 7400E0
 * 
 * Hydra_Software_Devel/66   8/1/08 5:07p vle
 * PR 44758: fix compiler warning for kernel mode build.
 * 
 * Hydra_Software_Devel/65   2/26/08 5:01p rgreen
 * PR39995: Fix for HDCP Compliance Test 1B-03; check if HDCP has been
 * aborted due to hot plug
 * 
 * Hydra_Software_Devel/64   12/18/07 9:18p rgreen
 * PR38235: Remove code to clear RDB_AUTHENTICATED bit.  Either ENC_EN or
 * ENC_DIS should always be transmitted.  Remove redundant code to
 * SET_RDB_AUTHENTICATED bit.  Add comments
 * 
 * Hydra_Software_Devel/63   11/8/07 12:56a vle
 * PR 36796: Increase the time out for generating An values from 10ms to 6
 * frames (16ms each frames).
 * 
 * Hydra_Software_Devel/62   11/7/07 4:14p rgreen
 * PR36718: Clear I_ALWAYS_REKEY_ON_VSYNC when using HDCP 1.0
 * 
 * Hydra_Software_Devel/61   10/20/07 8:44p rgreen
 * PR36200: Fixes to support HDCP Compliance Testing.  Set Core
 * Authenticaed after An generation
 * 
 * Hydra_Software_Devel/60   10/17/07 6:25p rgreen
 * PR36200: Add fixes to support HDCP Compliance Testing
 * 
 * Hydra_Software_Devel/59   10/11/07 4:52p rgreen
 * PR36035:  Add BHDM_CONFIG_HDCP_RI_SHORT_READ to enable Fast or Short
 * I2C reads for HDMI Rx Ri' reads.  Default is standard I2C reads
 * 
 * Hydra_Software_Devel/58   10/10/07 11:05a rgreen
 * PR35874:
 * Report error if TxCore does not complete V Calculation
 * Move the reading of the V Calculation from BHDM_HDCP_WriteTxKsvFifo
 * to BHDM_HDCP_RepeaterAuthenticateLink since Repeater Authentication can
 * occur  with no Ksvs (no downstream devices)
 * 
 * Hydra_Software_Devel/57   9/6/07 5:01p rgreen
 * PR34668: add code to support HDCP 1.1 Pj Checking
 * 
 * Hydra_Software_Devel/56   5/23/07 11:31p rgreen
 * PR31560:Remove excess delay in reading HDCP R0 value from receiver
 * 
 * Hydra_Software_Devel/55   9/15/06 5:38p rgreen
 * PR24304:  Add test features for Plugfest
 * 
 * Hydra_Software_Devel/54   6/26/06 12:04p rgreen
 * PR22342: Detect three successive HDCP Pj failures prior to forcing an
 * un-Authenticated link
 * 
 * Hydra_Software_Devel/53   6/2/06 3:25p rgreen
 * PR20866: Add option to disable Pj checking; default is on
 * Add BHDM_HDCP_Get/SetOptions
 * Modify LIC return codes to report specific Ri or Pj link failure
 * 
 * Hydra_Software_Devel/52   5/1/06 2:53p rgreen
 * PR8896: Debug cleanup.  Remove second instance of r0 generated from the
 * Ri interrupt.
 * 
 * Hydra_Software_Devel/51   2/8/06 4:37p rgreen
 * PR19552: Retry i2c read due to error up to four times during HDCP LIC
 * 
 * Hydra_Software_Devel/50   2/3/06 4:36p rgreen
 * PR8896: Remove depracated functions
 * 
 * Hydra_Software_Devel/49   10/21/05 5:44p rgreen
 * PR17750:
 * Move HDCP version banner from BKsv read to when HDCP An value is
 * generated; reduces debug messages
 * Clear HDCP abort flag after loading HDCP keys
 * Allow return of  new BHDM_HDCP_AUTH_ABORTED  value
 * Add debug config flag for HDMI Rx BCaps register.
 * 
 * Hydra_Software_Devel/48   10/3/05 4:20p rgreen
 * PR17428: Force re-read of Bcaps register at all times. remove mode used
 * to prevent redundant i2c reads
 * 
 * Hydra_Software_Devel/47   7/14/05 5:13p rgreen
 * PR16287: Fix incorrectly reported HDCP Link Success when I2C Read
 * failure occurs (monitor power down).  Should be reported as a failure.
 * 
 * Hydra_Software_Devel/46   6/27/05 6:21p rgreen
 * PR15217: Add Auth Support for HDCP Repeaters with Device Count 0;
 * Use bhdm_config.h configuration option.
 * Add option for Repeater Simulation Test
 * 
 * Hydra_Software_Devel/45   4/21/05 7:42p rgreen
 * PR14622: Add HDMI API to return KSV values
 * Add functions
 * BHDM_HDCP_GetRxKsv
 * BHDM_HDCP_GetRepeaterKsvFifo
 * 
 * Hydra_Software_Devel/44   3/8/05 5:46p rgreen
 * PR9474: HDCP 1.1 Support
 * Explicitly set HDCP Version 1.0 support when reading BCaps register
 * 
 * Hydra_Software_Devel/43   3/3/05 5:21p rgreen
 * PR9474: HDCP 1.1 Support
 * BHDM_HDCP_SetVersion is now depracated;
 * HDCP Version set automatically when BCaps read
 * (See BHDM_HDCP_ReadRxBksv & BHDM_HDCP_GetRxCaps)
 * 
 * Hydra_Software_Devel/42   3/1/05 5:07p rgreen
 * PR8896:
 * Eliminate redundant I2C reads of Rx BCaps register
 * 
 * Hydra_Software_Devel/41   2/24/05 3:46p rgreen
 * PR14230: Add HDCP bond status detection to chip interface
 * 
 * Hydra_Software_Devel/40   2/23/05 10:37a rgreen
 * PR8896: Fix Pj debug message; turn on BCaps messages
 * 
 * Hydra_Software_Devel/39   2/18/05 1:34p rgreen
 * PR14181: HDCP Auth Failure after repeated hotplugs / on/off cycles
 * Increase the wait time for HDCP authentication from 10ms to 50ms.
 * Authentication should not take more than 1 field ~16ms
 * 
 * Hydra_Software_Devel/38   2/4/05 1:48p rgreen
 * PR9474: HDCP 1.1 Support; add runtime identifier for file/revision
 * 
 * Hydra_Software_Devel/37   2/4/05 1:04p rgreen
 * PR9474: HDCP 1.1 Support
 * Remove depracated   BHDM_HDCP_Version eHdcpVersion from
 * BHDM_XmitEncrypted.  Already part of hdmi handle.
 * 
 * Hydra_Software_Devel/36   2/4/05 10:36a rgreen
 * PR9474: HDCP 1.1 Support
 * Add Pj Link Integrity Check
 * 
 * Hydra_Software_Devel/35   1/3/05 12:56p rgreen
 * PR8896: HDCP Bug Fixes
 * Fix bit shifting when checking repeater depth in BStatus register
 * Add timeouts when waiting for TxRepeater calculations
 * Fix loading of SHA-1 Rx value to Tx Repeater Core for big endian
 * environments
 * Fix InitializeRepeaterAuthentication for big endian environment
 * Add debug messages for debugging big endian environment
 * 
 * Hydra_Software_Devel/34   12/9/04 2:38p rgreen
 * PR8896: Change BHDM_HDCP_AuthenticateRepeater argument
 * RepeaterAuthenticated from bool to uint8_t.
 * 
 * Clean up debug messages
 * 
 * Hydra_Software_Devel/33   12/9/04 1:25p rgreen
 * PR8896: Fix bug with HDCP Repeater Authentication;
 * Restore writing BCaps value to Tx HDCP core... not needed for
 * Rx Authentication, but needed for Repeater Authentication
 * 
 * Make sure  HDCP Spec TestAn values not generated in RandomAn Mode;
 * generate new RandomAn if TestAn generated.
 * 
 * Hydra_Software_Devel/32   11/12/04 4:35p rgreen
 * PR8896:  Fix HDCP Authentication errors in big endian environments
 * 
 * Hydra_Software_Devel/31   10/21/04 6:26p rgreen
 * PR8896: Remove HDCP Key functions used for debug purposes.
 * 
 * Hydra_Software_Devel/30   10/18/04 11:58a rgreen
 * PR9474: Add HDCP 1.1 Support
 * HDCP 1.1 not fully implemented
 * Replace private BHDM_HDCP_P_SetVersion with new API call
 * BHDM_HDCP_SetVersion
 * Remove writing BCaps value to Tx HDCP core... un-neccessary
 * BHDM_HDCP_XmitEncrypted no longer uses HDCP Version; HdcpVersion set by
 * BHDM_HDCP_SetVersion
 * argument left for legacy reasons.
 * Modified BHDM_HDCP_GetRxStatus API call (RxStatus is uint16_t not
 * uint8_t)
 * Add debug messages for HDCP Rx BCaps (cabailities)
 * Add debug messages for HDCP Rx BStatus
 * Remove BHDM_HDCP_EnableDviEESS... handled by BHDM_HDCP_SetVersion
 * 
 * Hydra_Software_Devel/29   9/24/04 6:21p rgreen
 * PR9474:  Fix HDCP KSV FIFO reading
 * 
 * Hydra_Software_Devel/28   9/23/04 8:44p rgreen
 * PR12728: fix compiler warnings
 * PR8896: Fix Pj output message
 * 
 * Hydra_Software_Devel/27   9/15/04 10:56a rgreen
 * PR8896:
 * Remove BHDM_HDCP_VerifyLink function; no longer used
 * 
 * Hydra_Software_Devel/26   8/18/04 6:52p rgreen
 * PR 12116: Add HDCP Key Loading Support
 * Warn RDB_KeyLoad function to be removed
 * 
 * Hydra_Software_Devel/25   8/18/04 6:19p rgreen
 * PR12116: Add HDCP Key Loading Support
 * Fix DEBUG messages
 * 
 * Hydra_Software_Devel/24   6/4/04 2:01p rgreen
 * PR9474: HDMI API Development / Test
 * Fix HDCP BStatus write to the HDCP core
 * 
 * Hydra_Software_Devel/23   5/13/04 4:25p rgreen
 * PR 10273: HDMI / HDCP Revocation/renewability support requirement
 * 
 * Enable Repeater authetntication support.
 * 
 * Hydra_Software_Devel/22   4/29/04 12:23p rgreen
 * PR8896: HDMI Development
 * Re-insert HDCP_VerifyLink to support B0 compilation
 * 
 * Hydra_Software_Devel/22   4/29/04 12:15p rgreen
 * PR8896: HDMI Development
 * Re-insert HDCP_VerifyLink to support B0 compilation
 * 
 * Hydra_Software_Devel/21   4/28/04 11:00a rgreen
 * PR8896: HDMI Development
 * Removed code for BHDM_HDCP_VerifyLink; use
 * BHDM_HDCP_RiLinkIntegrityCheck instead
 * Use one 8 byte I2C Write to the Rx for the HDCP An value vs 2 4 byte
 * writes
 * Delay 100ms for the Rx to complete HDCP calculations once the Aksv is
 * written
 * Reset Ri counter when authentication is cleared.
 * Remove toggling of the MUX_VSYNC bit when activating encrypted
 * transmission
 * Get Pj value for RiLinkIntegrityCheck
 * 
 * Hydra_Software_Devel/20   4/5/04 1:18p rgreen
 * PR8896: HDMI API Development/Test
 * Clear AUTH_REQUEST bit immediately after setting; reather than after An
 * value is generated.  Clear additional bits in the HDCP_CTL register in
 * the same manner.
 * 
 * Verify all Tx Core components (HDCP + Scheduler)  are authenticated
 * before setting the link as authenticated.
 * 
 * Hydra_Software_Devel/19   3/20/04 5:13p rgreen
 * PR8896: HDMI API Development/Test
 * Add timeouts for HDCP Ri and An  calculations
 * 
 * Hydra_Software_Devel/18   2/16/04 5:10p rgreen
 * PR8896: HDMI API Development/Test
 * Change BHDM_IsRxDeviceAttached to BHDM_RxDeviceAttacde.
 * return code indicates function success/failure;use function
 * argument for DeviceAttached status
 * 
 * Hydra_Software_Devel/17   2/14/04 1:50p rgreen
 * PR8896: HDMI API Development/Test
 * Modify Debug Module Name
 * Fix compilation errors/warnings
 * 
 * Hydra_Software_Devel/16   2/13/04 7:53p rgreen
 * PR8896: HDMI API Development/Test
 * Add code for HDCP Repeater Support
 * Change BHDM_HDCP_ReadReapeaterKsvFIFO to
 * BHDM_HDCP_ReadRxRepeaterKsvFIFO
 * Add BHDM_HDCP_WriteTxKsvFIFO
 * Add checkForRepeater function
 * 
 * Use BHDM_CONFIG_REPEATER_SIMULATION_TEST to generate Tx verifications values contained in
 * the HDCP 1.1 Spec
 * 
 * Hydra_Software_Devel/15   1/27/04 1:03p rgreen
 * PR8896: HDMI API Development/Test
 * Move AuthenticateRepeater function to BHDMlib; needs access to XPT
 * handle for authentication with HDCP Repeater
 * 
 * Hydra_Software_Devel/14   1/26/04 11:38a rgreen
 * PR9474: Add HDCP 1.1 functionality
 * Add BHDM_HDCP_Version enumeration for HDCP Version unused/1_0/1_1
 * 
 * Replace BHDM_HDCP_XmitEncrypted bool argument with BHDM_HDCP_Version
 * enumeration.
 * Caller can decide which version of HDCP to used based on Rx Capability
 * 
 * Add BHDM_HDCP_RiLinkIntegrityCheck to replace BHDM_HDCP_VerifyLink...
 * VerifyLink will be eventually removed
 * 
 * Add BHDM_HDCP_PjLinkIntegrityCheck stub for HDCP 1.1 functionality
 * 
 * Add toggle to AUTH_REQUEST bit for GenerateAn
 * 
 * Enable Always Rekey on VSync functionality (HDCP 1.1) to the
 * XmitEncryprpted function
 * 
 * Hydra_Software_Devel/13   1/20/04 7:04p rgreen
 * PR8896: HDMI API Development/Test
 * Add flag to BHDM_XmitEncrypted to enable HDCP 1.1 Features
 * 
 * Hydra_Software_Devel/12   1/12/04 3:25p rgreen
 * PR8896: HDMI API Development
 * Add appropriate endian macros for i2c <--> register values
 * 
 * Hydra_Software_Devel/11   12/15/03 5:08p rgreen
 * PR8896:
 * Change BHDM_IsHdmiRxAttached to BHDM_IsRxDeviceAttached...
 * Disable non implemented functions (stubs) that give compiler warnings
 * 
 * Hydra_Software_Devel/10   12/10/03 3:35p rgreen
 * PR8727:
 * Fix Compilation Warnings
 * 
 * Hydra_Software_Devel/9   12/10/03 3:32p rgreen
 * PR8727:
 * Fix Compilation Warnings
 * 
 * Hydra_Software_Devel/8   12/10/03 10:20a rgreen
 * PR8727:
 * Fix compilation warnings
 * 
 * Hydra_Software_Devel/7   11/24/03 3:04p rgreen
 * Expose BHDM_HDCP_ReadRxBksv,
 *       BHDM_HDCP_GenerateAn, and
 *       BHDM_HDCP_AuthenticateLink as public functions to be used in
 * higher level libraries/apps.
 * Add supporting functions BHDM_HDCP_WriteTxAksvToRx and
 * BHDM_HDCP_EnableSerialKeyLoad
 * for use in authenticating receivers in higher level libraries/apps.
 * 
 * Remove AuthenticateReceiver from Porting Interface
 * Use status registers to check for completion of HDCP calculations
 * 
 * Add debug function BHDM_HDCP_DEBUGTEST_RdbLoadTxKeys to support
 * testing.
 * 
 * Hydra_Software_Devel/6   11/7/03 5:33p rgreen
 * Clear I_AUTH_REQUEST bit to enable advancing Ri authentication  values.
 * 
 * Hydra_Software_Devel/5   11/5/03 4:51p rgreen
 * Add BHDM_HDCP_IsLinkAuthenticated function which can be used to check
 * authentication status of the link.
 * Add missing BDBG_ENTER / LEAVE macros for some functions
 * Turn off AN_INFLUENCE bits when generating test An values
 * Display entire generated An value in one debug message
 * Fix error writing Aksv to the Receiver
 * Configure for loading keys through register vs serial key loader
 * Fix error loading HDCP Keys
 * Add Return Codes (rc) to VerifyLink
 * Fix debug statements in AuthenticateRepeater function (not implemented
 * yet)
 * 
 * Hydra_Software_Devel/4   10/24/03 7:13p rgreen
 * Remove NOT_IMPLEMENTED return values for newly implemented functions
 * 
 * Hydra_Software_Devel/3   10/23/03 6:22p rgreen
 * Fix unreferenced variable compilation warnings
 * 
 * Hydra_Software_Devel/2   10/23/03 5:27p rgreen
 * Use KNI memory functions
 * 
 * Hydra_Software_Devel/1   10/22/03 6:19p rgreen
 * Initial Version
 * 
 ***************************************************************************/
#include "bstd.h"
#include "breg_endian.h"

#include "bhdm_config.h"
#include "bhdm.h"
#include "bhdm_priv.h"
#include "bhdm_hdcp.h"

BDBG_MODULE(BHDM_HDCP) ;

#define	BHDM_CHECK_RC( rc, func )	              \
do                                                \
{										          \
	if( (rc = BERR_TRACE(func)) != BERR_SUCCESS ) \
	{										      \
		goto done;							      \
	}										      \
} while(0)


static unsigned int BHDM_HDCP_P_NumberOfSetBits(const unsigned char *bytes, int nbytes) ;

static BERR_Code BHDM_HDCP_P_CheckRevokedKsvList(
	const uint8_t *pRevokedKsvList, 
	const uint16_t uiRevokedKsvCount, 
	uint8_t *pRxKsvList, 
	uint16_t uiRxDeviceCount, 
	bool *bRevoked) ;


#if BHDM_CONFIG_HDCP_AUTO_RI_PJ_CHECKING_SUPPORT
static BERR_Code BHDM_HDCP_P_AutoRiLinkIntegrityCheck( 
	BHDM_Handle hHDMI     /* [in] HDMI handle */
) ;

static BERR_Code BHDM_HDCP_P_AutoPjLinkIntegrityCheck(
   BHDM_Handle hHDMI              /* [in] HDMI handle */
) ;

static BERR_Code BHDM_HDCP_P_ConfigureAutoRi(
	BHDM_Handle hHDMI     /* [in] HDMI handle */
) ;

static BERR_Code BHDM_HDCP_P_ConfigureAutoPj(
	BHDM_Handle hHDMI     /* [in] HDMI handle */
) ;

static BERR_Code BHDM_HDCP_P_EnableAutoRiPjChecking (
	BHDM_Handle hHDMI,      /* [in] HDMI handle */
	uint8_t uiPjChecking
) ;

static BERR_Code BHDM_HDCP_P_DisableAutoRiPjChecking (
	BHDM_Handle hHDMI		/* [in] HDMI handle */
) ;
#endif


/******************************************************************************
unsigned int BHDM_HDCP_P_NumberOfSetBits
Summary: Get the number of Set Bits
*******************************************************************************/
static unsigned int BHDM_HDCP_P_NumberOfSetBits(const unsigned char *bytes, int nbytes)
{
	int i, j ;
	int bit ;
	int count = 0 ;
	uint8_t byteToCheck;

	count = 0 ;
	for (i = 0; i < nbytes; i++)
	{
		bit = 1 ;
		byteToCheck = bytes[i];
		for (j = 0; j < 8 ; j++)
		{
			if (bit & byteToCheck)
				count++ ;
			bit = bit << 1 ;
		}
	}
	return count ;
} /* end BHDM_HDCP_P_NumberOfSetBits */


/******************************************************************************
BERR_Code BHDM_HDCP_P_CheckRevokedKsvList
Summary: Check if retrieved Ksv(s) are on the list of revoked Ksvs
*******************************************************************************/
BERR_Code BHDM_HDCP_P_CheckRevokedKsvList(
	const uint8_t *pRevokedKsvList, 
	const uint16_t uiRevokedKsvCount, 
	uint8_t *pRxKsvList, 
	uint16_t uiRxDeviceCount,
	bool *bRevokedKsv) 
{	
	uint16_t i, j ;
	uint16_t uiRxKsvIndex ;
	uint16_t uiRevokedKsvIndex ;
	
	uint8_t k ;
	
	*bRevokedKsv = false ;
	
	/* check each retrieved KSV against the Revoked KSV List */
	for (i = 0 ; i < uiRxDeviceCount ; i++)                  /* for each RxKsv */
	{
		uiRxKsvIndex = i * BHDM_HDCP_KSV_LENGTH ;
		
		/* display each KSV to check debugging */
		BDBG_MSG(("Checking RxBksv (Device %d): %02x %02x %02x %02x %02x", 
			i, pRxKsvList[uiRxKsvIndex + 4], 
			pRxKsvList[uiRxKsvIndex + 3], pRxKsvList[uiRxKsvIndex + 2], 
			pRxKsvList[uiRxKsvIndex + 1], pRxKsvList[uiRxKsvIndex + 0]));
			
		for (j = 0 ; j < uiRevokedKsvCount; j++)        /* for each Revoked Ksv */
		{
			/* 
			* SRM messages, which contain the Revoked Keys are stored in big endian 
			* format, therefore the comparison must be reveresed for the retrieved 
			* Ksvs which are in little endian format 
			*/
				
			/* set the Revoked Ksv byte index to the end of the stored key 
			* i.e. at the LSB
			*/
			uiRevokedKsvIndex = j * BHDM_HDCP_KSV_LENGTH + BHDM_HDCP_KSV_LENGTH - 1 ;
			
			for (k = 0 ; k < BHDM_HDCP_KSV_LENGTH; k++) /* for each Ksv Byte */
			{
#if 0				
				/* debug KSV revocation */
				BDBG_MSG(("Compare Ksv LSB %d: %02x - %02x", k,
					pRxKsvList[uiRxKsvIndex + k],               /* little endian */
					pRevokedKsvList[uiRevokedKsvIndex - k])) ;  /* big endian */
#endif					
				
				if (pRxKsvList[uiRxKsvIndex + k] != pRevokedKsvList[uiRevokedKsvIndex - k])
					break ;  /* no match; go to next Revoked Key */

				/* all bytes matched */					
				if (k + 1 == BHDM_HDCP_KSV_LENGTH)
				{
					BDBG_WRN(("RxBksv appears in revoked list")) ;
					*bRevokedKsv = true ;
					goto done ;
				}
			}
		}
	}
		
done:
	return BERR_SUCCESS ;		
}


/******************************************************************************
BERR_Code BHDM_HDCP_ReadRxBksv
Summary: Read the HDCP Bksv value from the receiver
*******************************************************************************/
BERR_Code BHDM_HDCP_ReadRxBksv(
   BHDM_Handle hHDMI,              /* [in] HDMI handle */
   const uint8_t *pRevokedKsvList, /* [in] pointer to Revoked KSV List */
   const uint16_t uiRevokedKsvCount /* [in] number of KSVs in Revoked Ksv List */
)
{
	BERR_Code   rc = BERR_SUCCESS;

	uint32_t    Register ;
	uint32_t    BksvRegisterValue ;
	unsigned char   RxBksv[] = { 0x00, 0x01, 0x02, 0x03, 0x04 }; /* LSB...MSB */
	/* 
	**	FYI B1 Bksv Test Value from HDCP Specification
	**     RxBksv[] = { 0xcd, 0x1a, 0xf2, 0x1e, 0x51 }; 
	*/
	uint8_t BCaps ;
	uint8_t RxDeviceAttached ;
	bool bRevoked ;
		
	BDBG_ENTER(BHDM_HDCP_ReadRxBksv) ;


	/* make sure HDMI Cable is connected to something... */
	BHDM_CHECK_RC(rc, BHDM_RxDeviceAttached(hHDMI, &RxDeviceAttached));
	if (!RxDeviceAttached)
	{
		rc = BHDM_NO_RX_DEVICE ;
		BDBG_WRN(("No DVI/HDMI Device Found")) ;
		goto done ;
	}

	/* read the BCaps first */
	BCaps = 0 ;
	BHDM_CHECK_RC(rc, BHDM_HDCP_GetRxCaps(hHDMI, &BCaps)) ;
	
	
#if BHDM_CONFIG_REPEATER_SIMULATION_TEST	
	BCaps |= BHDM_HDCP_RxCaps_eHdcpRepeater ; 
#endif


	/* We can't access I2C if control has been handed over to HW */
	if (hHDMI->bAutoRiPjCheckingEnabled)
	{
		BKNI_Memcpy(RxBksv, hHDMI->HDCP_RxKsv, BHDM_HDCP_KSV_LENGTH) ;
	}
	else
	{
		/* try to read the RxBksv */
		rc = BREG_I2C_Read(hHDMI->hI2cRegHandle, 
			BHDM_HDCP_RX_I2C_ADDR,  BHDM_HDCP_RX_BKSV0, RxBksv, BHDM_HDCP_KSV_LENGTH ) ;	
		if (rc != BERR_SUCCESS)
		{
			BDBG_ERR(("Bksw I2C read error"));	
			rc = BHDM_HDCP_RX_BKSV_I2C_READ_ERROR;
			goto done;
		}
	}

	/* Verify RxBksv has 20 zeros & 20 ones */
	if (BHDM_HDCP_P_NumberOfSetBits(RxBksv, BHDM_HDCP_KSV_LENGTH) != 20) 
	{
		BDBG_ERR(("Valid RxBksv contain 20 1s and 20 0s")) ;
		rc = BHDM_HDCP_RX_BKSV_ERROR ;
		goto done ;
	}
	
	/* display changes in KSV for debugging */
	if (BKNI_Memcmp(RxBksv, hHDMI->HDCP_RxKsv, BHDM_HDCP_KSV_LENGTH))
	{
		BDBG_MSG(("RxBksv = %02x %02x %02x %02x %02x", 
				RxBksv[4], RxBksv[3], RxBksv[2], RxBksv[1], RxBksv[0]));
	}
	

	BKNI_Memcpy(hHDMI->HDCP_RxKsv, RxBksv, BHDM_HDCP_KSV_LENGTH) ;
	
	
	/* check the retrieved KSV against the Revoked KSV List */
	if (uiRevokedKsvCount)
	{
		BHDM_HDCP_P_CheckRevokedKsvList(
			(uint8_t *) pRevokedKsvList, uiRevokedKsvCount, RxBksv, 1, &bRevoked) ;
		
		if (bRevoked)
		{
			rc = BHDM_HDCP_RX_BKSV_REVOKED ;
			goto done ;
		}
	}
	
	/* write the 4 LSBs RxBksv to the transmitter... */
	BksvRegisterValue =
		  RxBksv[0]
		| RxBksv[1] <<  8
		| RxBksv[2] << 16
		| RxBksv[3] << 24 ;
		
	Register = BCHP_FIELD_DATA(HDMI_BKSV0, I_BKSV_31_0, BksvRegisterValue) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_BKSV0, Register) ;
#if 0	
	BDBG_MSG(("BKSV0 Register: 0x%08X", Register)) ;
#endif	


	/* 
	-- write the 1 MSB RxBksv to the transmitter... 
	-- also check if we are authenticating with a repeater
	*/
	BksvRegisterValue = RxBksv[4];
	Register = BCHP_FIELD_DATA(HDMI_BKSV1, I_BKSV_39_32, BksvRegisterValue)
	        |  BCHP_FIELD_DATA(HDMI_BKSV1, BCAPS_7_0, BCaps) ;
#if 0
	BDBG_MSG(("BKSV1 Register Value: 0x%08X", Register)) ;
#endif	
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_BKSV1, Register) ;

done:
	BDBG_LEAVE(BHDM_HDCP_ReadRxBksv) ;
	return rc ;
} /* end BHDM_HDCP_ReadRxBksv */


#if BHDM_CONFIG_DEBUG_FORCE_FIFO_ERROR
	/* Tested FIFO Recovery with RB 20120326 */
BERR_Code BHDM_HDCP_DEBUG_P_ForceValidVideoFailure(BHDM_Handle hHDMI) 
{
	BERR_Code   rc = BERR_SUCCESS;
	BREG_Handle hRegister ;
	uint32_t Register ;

	hRegister = hHDMI->hRegister ;

	Register = BREG_Read32(hRegister, BCHP_HDMI_RM_CONTROL) ;
	Register &= ~ BCHP_MASK(HDMI_RM_CONTROL, TRACKING_RANGE) ;
	Register |= BCHP_FIELD_DATA(HDMI_RM_CONTROL, TRACKING_RANGE, 1) ;
	BREG_Write32(hRegister, BCHP_HDMI_RM_CONTROL, Register) ;

#if 0
	BREG_Write32(hRegister, BCHP_HDMI_RM_SKIP_REPEAT_GAP, 0x1) ;
	BREG_Write32(hRegister, BCHP_HDMI_RM_SKIP_REPEAT_NUMBER, 0x30) ;
	
	Register = BREG_Read32(hRegister, BCHP_HDMI_RM_SKIP_REPEAT_CONTROL) ;
		Register &= ~ BCHP_MASK(HDMI_RM_SKIP_REPEAT_CONTROL, ENABLE) ;
	BREG_Write32(hRegister, BCHP_HDMI_RM_SKIP_REPEAT_CONTROL, Register) ;
		Register |= BCHP_FIELD_DATA(HDMI_RM_SKIP_REPEAT_CONTROL, ENABLE, 1) ;
	BREG_Write32(hRegister, BCHP_HDMI_RM_SKIP_REPEAT_CONTROL, Register) ;
#endif

#if 0
	BDBG_WRN(("Force FIFO Error")) ;
	Register = BREG_Read32(hRegister, BCHP_HDMI_RM_OFFSET) ;
	Register &= ~ BCHP_MASK(HDMI_RM_OFFSET, OFFSET_ONLY) ;
	{
		uint32_t offset ;
		
		offset = BCHP_GET_FIELD_DATA(Register, HDMI_RM_OFFSET, OFFSET) ;
		BDBG_WRN(("!!!!!!!!!!!!!!!!!!!!!offset %d", offset)) ;
		offset = (offset * 5000) / 5001 ;
		BDBG_WRN(("!!!!!!!!!!!!!! offset (1percent) %d", offset)) ;
		Register &= BCHP_MASK(HDMI_RM_OFFSET, OFFSET) ;
			
		Register |= BCHP_FIELD_DATA(HDMI_RM_OFFSET, OFFSET, offset) ;
		Register |= BCHP_FIELD_DATA(HDMI_RM_OFFSET, OFFSET_ONLY, 0) ;
		BREG_Write32(hRegister, BCHP_HDMI_RM_OFFSET, Register) ;
	}

	BKNI_Sleep(3) ;
	BHDM_CHECK_RC(rc, BHDM_HDCP_CheckForValidVideo(hHDMI)) ;
#endif

	if (!rc)
	{
		BDBG_WRN(("DONE DONE DONE Valid Video OK AT LINE %d", __LINE__)) ;				
	}
	
done:
	if(rc) 
	{
		BDBG_ERR(("Video NOT VALID!!!!!!!! AT LINE %d", __LINE__)) ;				
	}
	return rc ;
			
}
#endif


/******************************************************************************
BERR_Code BHDM_HDCP_GenerateAn
Summary: Generate the HDCP An value for HDCP calculations
*******************************************************************************/
BERR_Code BHDM_HDCP_GenerateAn(
	BHDM_Handle hHDMI,              /* [in] HDMI handle */
	BHDM_HDCP_AnSelect AnSelection) /* [in] HDCP An type value to use */
{
	BERR_Code   rc = BERR_SUCCESS;
	BREG_Handle hRegister ;
	uint32_t    Register ;
	uint32_t	AnMsb, AnLsb ;
	uint8_t i ;
	uint8_t timeoutFrames ;
	uint8_t AnReady ;
	uint8_t AnValue[BHDM_HDCP_AN_LENGTH] ;
	bool 	bTestAnGenerated = false ;
	bool bHdcpCapable ;

#define NUM_TEST_AN_VALUES 4
static const uint8_t ucHdcpSpecTestAnValues[NUM_TEST_AN_VALUES][BHDM_HDCP_AN_LENGTH] =
	{
		{ 0x03, 0x04, 0x07, 0x0c, 0x13, 0x1c, 0x27, 0x34},  /* A1/B1 */
		{ 0xe5, 0x0f, 0xd1, 0x3a, 0xa5, 0x62, 0x5e, 0x44},  /* A1/B2 */
		{ 0x07, 0x6e, 0xc6, 0x01, 0xbb, 0xc2, 0xbe, 0x83},  /* A2/B1 */
		{ 0x4d, 0xa7, 0x06, 0x54, 0x17, 0xf7, 0x51, 0x03}   /* A2/B2 */
	} ;
	

	BDBG_ENTER(BHDM_HDCP_GenerateAn) ;

	if (hHDMI->bHdcpAnRequest)
	{
		BDBG_ERR(("HDCP An value already requested")) ;
		return BHDM_HDCP_MULTIPLE_AN_REQUEST ;
	}

	/* new authentication attempt */
	BDBG_WRN(("HDCP Authentication Request")) ;
	BDBG_MSG(("*****************************************")) ;
	BDBG_MSG(("$brcm_Workfile: bhdm_hdcp.c $")) ;
 	BDBG_MSG(("$brcm_Revision: Hydra_Software_Devel/103 $")) ;
 	BDBG_MSG(("$brcm_Date: 4/30/12 4:46p $")) ;
	BDBG_MSG(("*****************************************")) ;

	hRegister = hHDMI->hRegister ;

#if BHDM_CONFIG_DEBUG_PJ_CHECKING
	BDBG_WRN(("!!!!! Pj Check Test... video will be solid green or magenta color !!!!! ")) ;
	BDBG_WRN(("!!!!! Ri Sequence should be consistent on every authentication  !!!!! ")) ;
	Register = BREG_Read32(hRegister,  BCHP_HDMI_CP_TST) ;
	Register &= 
		~(BCHP_MASK(HDMI_CP_TST, I_TST_FORCE_VIDEO_ALL_ONES) 
		| BCHP_MASK(HDMI_CP_TST, I_TST_FORCE_VIDEO_ALL_ZEROS)) ;

	Register |= BCHP_FIELD_DATA(HDMI_CP_TST,  I_TST_FORCE_VIDEO_ALL_ZEROS, 1) ;
		
	BREG_Write32(hRegister, BCHP_HDMI_CP_TST, Register) ;
	AnSelection = BHDM_HDCP_AnSelect_eTestA1B1An ;
#endif	

	
	bHdcpCapable = false ;
	BHDM_CHECK_RC(rc, BCHP_GetFeature(hHDMI->hChip, BCHP_Feature_eHdcpCapable, &bHdcpCapable)) ;
	if (!bHdcpCapable)
	{
		BDBG_ERR(("#############################")) ;
		BDBG_ERR(("HDCP is DISABLED on this part")) ;
		BDBG_ERR(("#############################")) ;
		rc = BERR_NOT_SUPPORTED ;
		goto done ;
	} 

	/* clear previously received values; for debug reporting */
	BKNI_Memset(hHDMI->HDCP_RxKsv, 0, BHDM_HDCP_KSV_LENGTH) ;
	hHDMI->RxBCaps = 0 ;



#if TODO
	/* The following call should be moved to the HDCP State Machine */

	/* make sure video is flowing from BVN before starting */
	BHDM_CHECK_RC(rc, BHDM_CheckForValidVideo(hHDMI)) ;

#if 0
      /* delay 1s prior in App/HDCP Auth State machine */
	BKNI_Sleep(1000) ;
#endif
#endif




#if BHDM_CONFIG_DEBUG_FORCE_FIFO_ERROR
	/* Tested FIFO Recovery w/ RB 20120326 */
	BHDM_CHECK_RC(rc, BHDM_HDCP_DEBUG_P_ForceValidVideoFailure(hHDMI)) ;
#endif	

	Register = BREG_Read32(hRegister, BCHP_HDMI_SCHEDULER_CONTROL) ;
	Register &= ~BCHP_MASK(HDMI_SCHEDULER_CONTROL, ALWAYS_VERT_KEEP_OUT) ;

	Register |= BCHP_FIELD_DATA(HDMI_SCHEDULER_CONTROL, ALWAYS_VERT_KEEP_OUT, 1) ;
	BREG_Write32(hRegister, BCHP_HDMI_SCHEDULER_CONTROL, Register) ;

	hHDMI->bHdcpAnRequest = true ;
 	do
	{
		/* set up for Test An values if requested */
		if (AnSelection != BHDM_HDCP_AnSelect_eRandomAn)  
		{
			/* for HDCP Test An values, make sure the influence bits are off */
			Register = BREG_Read32(hRegister, BCHP_HDMI_CP_CONFIG) ;
			Register &= ~BCHP_MASK(HDMI_CP_CONFIG, AN_INFLUENCE_MODE) ;
			Register |= BCHP_FIELD_DATA(HDMI_CP_CONFIG, AN_INFLUENCE_MODE, 0) ;
			BREG_Write32(hRegister, BCHP_HDMI_CP_CONFIG, Register) ;
			
			/* Set An Selection and TestAn Enable */
			Register = BREG_Read32(hRegister, BCHP_HDMI_CP_TST) ;
			Register &= 
				~( BCHP_MASK(HDMI_CP_TST, I_TST_AN_SEL_1_0) 
				 | BCHP_MASK(HDMI_CP_TST, I_TST_MODE_AN_ENABLE)) ;
			Register |= 
				  BCHP_FIELD_DATA(HDMI_CP_TST, I_TST_AN_SEL_1_0, AnSelection) 
				| BCHP_FIELD_DATA(HDMI_CP_TST, I_TST_MODE_AN_ENABLE, 1) ;
			BREG_Write32(hRegister, BCHP_HDMI_CP_TST, Register) ;
		}

		/* Check for completion of the generated HDCP An Value */
		/* should complete within 1-2 frames */
		timeoutFrames = 6 ;
		do			  
		{
			/* request the authentication values */
			/* Set AUTH_REQUEST_BIT only - all other bits must be zero */
			Register = BCHP_FIELD_DATA(HDMI_HDCP_CTL, I_AUTH_REQUEST, 1) ;
			BREG_Write32(hRegister, BCHP_HDMI_HDCP_CTL, Register) ;

			/* HDCP An value *should* be generated after one frame;  */
			/* Wait up to  3+ frames */
			/* NOTE: Video flow from BVN to HDMI must be stable when generating An value */
			BHDM_CHECK_RC(rc, BKNI_WaitForEvent(hHDMI->BHDM_EventHDCP, BHDM_HDCP_CONFIG_AN_TIMEOUT_MS)) ;

 			Register = BREG_Read32(hRegister, BCHP_HDMI_CP_STATUS) ;
			AnReady = BCHP_GET_FIELD_DATA(Register, HDMI_CP_STATUS, O_AN_READY) ;
			if (AnReady)
				break ;
	
			BDBG_WRN(("Waitng for HDCP An Value")) ;
		} while ( timeoutFrames-- ) ;
	
		if (!AnReady)
		{
			BDBG_ERR(("Timeout Error waiting for HDCP An Value")) ;

#if TODO
/* can be removed since State machine will confirm Valid Video Before starting Authentication */
			/* check for valid video */
			BHDM_CHECK_RC(rc, BHDM_CheckForValidVideo(hHDMI)) ;
#endif

			rc = BERR_TIMEOUT ;
			goto done ;
		}
		

		/* read the generated HDCP An value / write to the HDCP Rx */
		AnMsb = BREG_Read32(hRegister, BCHP_HDMI_AN1) ; /* 32 MSB */
		AnLsb = BREG_Read32(hRegister, BCHP_HDMI_AN0) ; /* 32 LSB */
		
		/* copy the AnValue to a uint8_t string for single I2C write to the Rx */
		AnValue[0] =  AnLsb        & 0xFF ;
		AnValue[1] = (AnLsb >>  8) & 0xFF ; 
		AnValue[2] = (AnLsb >> 16) & 0xFF ; 
		AnValue[3] = (AnLsb >> 24) & 0xFF ; 
		
		AnValue[4] =  AnMsb        & 0xFF ;
		AnValue[5] = (AnMsb >>  8) & 0xFF ; 
		AnValue[6] = (AnMsb >> 16) & 0xFF ; 
		AnValue[7] = (AnMsb >> 24) & 0xFF ; 
		
		/* verify the generated An value is not one of the test values if we are generating random numbers */
		if (AnSelection == BHDM_HDCP_AnSelect_eRandomAn)  /* for Test An Values */
		{
			for (i = 0 ; i < NUM_TEST_AN_VALUES ; i++)
			{
#if 0				
				BDBG_MSG(("Compare generated RandomAn to TestAn %02x%02x%02x%02x%02x%02x%02x%02x ", 
					ucHdcpSpecTestAnValues[i][7], ucHdcpSpecTestAnValues[i][6],	
					ucHdcpSpecTestAnValues[i][5], ucHdcpSpecTestAnValues[i][4],	
					ucHdcpSpecTestAnValues[i][3], ucHdcpSpecTestAnValues[i][2],	
					ucHdcpSpecTestAnValues[i][1], ucHdcpSpecTestAnValues[i][0])) ;
#endif
				if (!BKNI_Memcmp(AnValue, ucHdcpSpecTestAnValues[i], BHDM_HDCP_AN_LENGTH))
				{
					bTestAnGenerated = true ;
					break ;
				}
			}
		}
		
		BDBG_MSG(("AnValue = %02x%02x%02x%02x%02x%02x%02x%02x ", 
			AnValue[7], AnValue[6],	AnValue[5], AnValue[4],	
			AnValue[3], AnValue[2],	AnValue[1], AnValue[0])) ;
	} while ((AnSelection == BHDM_HDCP_AnSelect_eRandomAn) && (bTestAnGenerated)) ;


#if BHDM_CONFIG_HDCP_FORCE_ENC_SIGNAL
	/* 
	    For HDCP Compliance Test ...
	    make sure the RDB Auth is set so at least the ENC_DIS signal is always sent 
	    when MUX_VSYNC is enabled after authentication, ENC_EN will be transmitted
	  */
	  
	/* Set SET_RDB_AUTHENTICATED_BIT only - all other bits must be zero */
	Register = BCHP_FIELD_DATA(HDMI_HDCP_CTL, I_SET_RDB_AUTHENTICATED, 1) ;
	BREG_Write32(hRegister, BCHP_HDMI_HDCP_CTL, Register) ;	
#endif	

	/* write the generate An value to the Receiver */
        BHDM_CHECK_RC(rc, BREG_I2C_Write(hHDMI->hI2cRegHandle,
			BHDM_HDCP_RX_I2C_ADDR,	BHDM_HDCP_RX_AN0, (uint8_t *) AnValue, BHDM_HDCP_AN_LENGTH)) ;
	
done:
	hHDMI->bHdcpAnRequest = false ;	
	BDBG_LEAVE(BHDM_HDCP_GenerateAn) ;
	return rc ;
} /* end BHDM_HDCP_GenerateAn */


/******************************************************************************
BERR_Code BHDM_HDCP_WriteTxAksvToRx
Summary: Write the HDCP Aksv value to the receiver 
*******************************************************************************/
BERR_Code BHDM_HDCP_WriteTxAksvToRx(
   BHDM_Handle hHDMI,           /* [in] HDMI handle */
   const uint8_t *pTxAksv       /* [in] pointer HDCP Key Set Aksv Value */
) 
{
	static uint8_t HdcpSpecTestSet1Aksv[BHDM_HDCP_KSV_LENGTH] =
		{0x14, 0xF7, 0x61, 0x03, 0xB7} ;

	BERR_Code   rc = BERR_SUCCESS;
	uint8_t RxCaps;
	uint8_t AinfoByte;
    
	BDBG_ENTER(BHDM_HDCP_WriteTxAksvToRx) ;

	/* Verify TxAksv has 20 zeros & 20 ones */
	
	/* display KSV for debugging */
	BDBG_MSG(("TxAksv = %02x %02x %02x %02x %02x", 
			pTxAksv[4], pTxAksv[3], pTxAksv[2], pTxAksv[1], pTxAksv[0]));

	/* check for valid KSV */
	if (BHDM_HDCP_P_NumberOfSetBits((uint8_t *) pTxAksv, BHDM_HDCP_KSV_LENGTH) != 20) 
	{
		BDBG_ERR(("Valid TxAksv must contain 20 1s and 20 0s")) ;

		/* display KSV for debugging */
		BDBG_ERR(("TxAksv = %02x %02x %02x %02x %02x", 
			pTxAksv[4], pTxAksv[3], pTxAksv[2], pTxAksv[1], pTxAksv[0]));
		rc = BHDM_HDCP_TX_AKSV_ERROR ;
		goto done ;
	}

	/* make sure KSV is not from the Test Key Set*/	
	if (!BKNI_Memcmp(HdcpSpecTestSet1Aksv, pTxAksv, BHDM_HDCP_KSV_LENGTH))
	{
		BDBG_WRN(("\n\n\n")) ;
		BDBG_WRN(("******************************")) ;
		BDBG_WRN(("TxAksv = %02x %02x %02x %02x %02x", 
				pTxAksv[4], pTxAksv[3], pTxAksv[2], pTxAksv[1], pTxAksv[0]));
		BDBG_WRN(("TkAksv is part of the HDCP Spec Test Key Set")) ;
		BDBG_WRN(("Production Key Set required for use with a Production Receiver")) ;
		BDBG_WRN(("******************************")) ;
		rc = BHDM_HDCP_TX_AKSV_ERROR ;
		goto done ;
	}

	
	/* Write the AInfo value */
	BHDM_CHECK_RC(rc, BHDM_HDCP_GetRxCaps(hHDMI, &RxCaps)) ;
	
    if ((RxCaps & BHDM_HDCP_RxCaps_eHDCP_1_1_Features) 
	&& (hHDMI->HdcpOptions.PjChecking)
	&& (hHDMI->HdcpOptions.numPjFailures <= BHDM_HDCP_MAX_PJ_LINK_FAILURES_BEFORE_DISABLE_HDCP_1_1))   
	{
		AinfoByte = BHDM_HDCP_RX_ENABLE_1_1_FEATURES ;

#if BHDM_CONFIG_HDCP_AUTO_RI_PJ_CHECKING_SUPPORT
		if (!hHDMI->bAutoRiPjCheckingEnabled) {
			BHDM_CHECK_RC(rc, BREG_I2C_Write(hHDMI->hI2cRegHandle,
				BHDM_HDCP_RX_I2C_ADDR, BHDM_HDCP_RX_AINFO, (uint8_t *) &AinfoByte, 1)) ;
		}
#else
		BHDM_CHECK_RC(rc, BREG_I2C_Write(hHDMI->hI2cRegHandle,
			BHDM_HDCP_RX_I2C_ADDR, BHDM_HDCP_RX_AINFO, (uint8_t *) &AinfoByte, 1)) ;
#endif

	}   

	/* Write the TxAksv value to the HDCP Rx */
	rc = BREG_I2C_Write(hHDMI->hI2cRegHandle, BHDM_HDCP_RX_I2C_ADDR,  
		BHDM_HDCP_RX_AKSV0, (uint8_t *) pTxAksv, BHDM_HDCP_KSV_LENGTH) ;

	if (rc != BERR_SUCCESS)
	{
		BDBG_ERR(("Aksv I2C write error"));
		rc = BHDM_HDCP_TX_AKSV_I2C_WRITE_ERROR;
		goto done;
	}

	/* 
	consider the authentication request started once the TxAksv 
	is written to the Rx; clear the authenticated link bit
	*/
	hHDMI->AbortHdcpAuthRequest = 0 ;
	
	/* clear AuthenticatedLink Variable */
	hHDMI->HDCP_AuthenticatedLink = 0 ;
	

done :	
	BDBG_LEAVE(BHDM_HDCP_WriteTxAksvToRx) ;
	return rc ;
}


/******************************************************************************
BERR_Code BHDM_HDCP_EnableSerialKeyLoad(
Summary: Enable HDMI transmitter core to receive the HDCP Keys serially.
*******************************************************************************/
BERR_Code BHDM_HDCP_EnableSerialKeyLoad(
   BHDM_Handle hHDMI              /* [in] HDMI handle */
)
{
	BERR_Code   rc = BERR_SUCCESS;
	uint32_t    Register ;

	BDBG_ENTER(BHDM_HDCP_EnableSerialKeyLoad) ;

	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_CP_CONFIG) ;
	Register &= ~BCHP_MASK(HDMI_CP_CONFIG, I_ENABLE_RDB_KEY_LOAD) ;
	Register |= BCHP_FIELD_DATA(HDMI_CP_CONFIG, I_ENABLE_RDB_KEY_LOAD, 0x0) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_CP_CONFIG, Register) ;


	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_CP_CONFIG) ;
	Register &= ~BCHP_MASK(HDMI_CP_CONFIG, I_KEY_BASE_ADDRESS_9_0) ;
	Register |= BCHP_FIELD_DATA(HDMI_CP_CONFIG, I_KEY_BASE_ADDRESS_9_0, 0x80) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_CP_CONFIG, Register) ;
		
	hHDMI->AbortHdcpAuthRequest = 0 ;	
	
	BDBG_LEAVE(BHDM_HDCP_EnableSerialKeyLoad) ;
	return rc ;
}




/******************************************************************************
BERR_Code BHDM_HDCP_AuthenticateLink
Summary: Authenticate the HDCP Link; verify TxR0 and RxR0 values are equal
*******************************************************************************/
BERR_Code BHDM_HDCP_AuthenticateLink
(
	BHDM_Handle hHDMI              /* [in] HDMI handle */
)
{
	BERR_Code   rc = BERR_SUCCESS;
	uint32_t    Register ;
	uint8_t     timeoutMs ;

	uint8_t
		RdbAuthenticated ,
		SchedulerAuthenticated,
		CoreAuthenticated ;


	/* Before checking Authentication... */
	/* Reset Ri counter and previous Ri Values */
	hHDMI->HDCP_RiCount = 0 ;

	hHDMI->HDCP_Ri2SecsAgo = 0x01 ;
	hHDMI->HDCP_Ri4SecsAgo = 0x01 ;
	hHDMI->HDCP_Ri6SecsAgo = 0x01 ;

	hHDMI->HDCP_AuthenticatedLink = 0 ;

#if BHDM_CONFIG_HDCP_AUTO_RI_PJ_CHECKING_SUPPORT
	hHDMI->bAutoRiPjCheckingEnabled = false;
#endif

	if ((rc = BHDM_HDCP_RiLinkIntegrityCheck(hHDMI)) != BERR_SUCCESS) 
	{
		BDBG_ERR(("HDCP Authentication Failed"));
		goto done ;
	}


	/* SET_RDB_AUTHENTICATED must be asserted to ensure ENC_EN/ENC_DIS 
	signal is sent either via MUX_VSYNC or by the HW */
	
	/* Set SET_RDB_AUTHENTICATED_BIT only - all other bits must be zero */
	Register = BCHP_FIELD_DATA(HDMI_HDCP_CTL, I_SET_RDB_AUTHENTICATED, 1) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_HDCP_CTL, Register) ;


	/* wait up to 50ms for the core to authenticate; should authenticate within 1 field */
	timeoutMs = 5 ;
	do			  
	{
		/* make sure HDCP request is still active... hotplug will abort request  */
		if (hHDMI->AbortHdcpAuthRequest)
		{
			BDBG_WRN(("HDCP Authentication Request Aborted....")) ;
			rc = BHDM_HDCP_AUTH_ABORTED ;
			goto done ;
		}
		
		Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_CP_STATUS) ;

		RdbAuthenticated = BCHP_GET_FIELD_DATA(Register, HDMI_CP_STATUS, RDB_AUTHENTICATED) ;
		SchedulerAuthenticated = BCHP_GET_FIELD_DATA(Register, HDMI_CP_STATUS, AUTHENTICATED_OK) ;
		CoreAuthenticated = BCHP_GET_FIELD_DATA(Register, HDMI_CP_STATUS, CORE_AUTHENTICATED) ;

		if (RdbAuthenticated 
		&&  SchedulerAuthenticated
		&&  CoreAuthenticated)
			break ;
			
		BDBG_WRN(("Waiting for TxCore to Authenticate %d %d %d",
			RdbAuthenticated, SchedulerAuthenticated, CoreAuthenticated)) ;

		BKNI_Sleep(10) ;
	} while ( timeoutMs-- ) ;

	if (!RdbAuthenticated)
	{
		BDBG_WRN(("HDCP Tx RDB not authenticated")) ;
		rc = BHDM_HDCP_AUTHENTICATE_ERROR ;
		goto done ;
	}
	else if (!SchedulerAuthenticated)
	{
		BDBG_WRN(("HDCP Tx Scheduler not authenticated")) ;
		rc = BHDM_HDCP_AUTHENTICATE_ERROR ;
		goto done ;
	}
	else if (!CoreAuthenticated)
	{
		BDBG_WRN(("HDCP Tx Core not authenticated")) ;
		rc = BHDM_HDCP_AUTHENTICATE_ERROR ;
		goto done ;
	}


	hHDMI->HDCP_AuthenticatedLink = 1 ;
	BDBG_MSG(("Receiver AUTHENTICATED")) ;
	

	/* Reset the Ri values again so when Video transmission begins */
	/* HDCP doesn't think Ri is not advancing */
	hHDMI->HDCP_Ri2SecsAgo = 0x0102 ;
	hHDMI->HDCP_Ri4SecsAgo = 0x0203 ;
	hHDMI->HDCP_Ri6SecsAgo = 0x0304 ;

#if BHDM_CONFIG_HDCP_AUTO_RI_PJ_CHECKING_SUPPORT
	if (hHDMI->DeviceSettings.bEnableAutoRiPjChecking)
	{
		/* Configure/setup auto Ri */
		BHDM_HDCP_P_ConfigureAutoRi(hHDMI);

		/* Configure/setup auto Pj */
		if (hHDMI->HdcpOptions.PjChecking)
			BHDM_HDCP_P_ConfigureAutoPj(hHDMI);

                /* Cache RxBCaps and RxKSV before we switch over to auto HW checking. */
                BHDM_CHECK_RC(rc, BHDM_HDCP_GetRxCaps(hHDMI, &hHDMI->RxBCaps)) ;
                BHDM_CHECK_RC(rc, BHDM_HDCP_GetRxKsv(hHDMI, &hHDMI->HDCP_RxKsv[0])) ; 

                /* If non-repeater, switch over from software to auto HW checking now. */
                if ((hHDMI->RxBCaps & BHDM_HDCP_RxCaps_eHdcpRepeater) == 0)
                {
                        BHDM_HDCP_P_EnableAutoRiPjChecking(hHDMI, hHDMI->HdcpOptions.PjChecking) ;
                }
	}
#endif

done:	
	return rc ;
} /* end BHDM_HDCP_AuthenticateLink */




/******************************************************************************
BERR_Code BHDM_HDCP_ClearAuthentication(
Summary: Clear the Authenticated link between the Transmitter and the Receiver.
*******************************************************************************/
BERR_Code BHDM_HDCP_ClearAuthentication(
   BHDM_Handle hHDMI              /* [in] HDMI handle */
) 
{
	BERR_Code   rc = BERR_SUCCESS;
	uint32_t    Register ;

	BDBG_ENTER(BHDM_HDCP_ClearAuthentication) ;
	BDBG_ASSERT( hHDMI );

	/* Clear the MuxVsync Bit First (eliminate momentary snow at the Rx) */
	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_CP_CONFIG) ;
	Register &= ~BCHP_MASK(HDMI_CP_CONFIG, I_MUX_VSYNC) ;
	Register |= BCHP_FIELD_DATA(HDMI_CP_CONFIG, I_MUX_VSYNC, 0) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_CP_CONFIG, Register) ;

	/* Wait long enough for the current frame to complete... */
	BKNI_Sleep(20) ;  /* wait */
			

#if BHDM_CONFIG_HDCP_FORCE_ENC_SIGNAL
	/* 
	Clearing the I_CLEAR_RDB_AUTHENTICATED_BIT has been removed to fix HDCP
	Compliance Test Issue... the CLEAR_RDB_AUTHENTICATED should never asserted
	
	The Compliance Test checks to make sure either ENC_DIS or ENC_EN 
	is always transmitted.	Setting  the core as always authenticated will
	allow the MUX_VSYNC to control the ENC_DIS/ENC_EN signal
	*/

#else
	/* Set the Tx Authentication Cleared Bit */

	/* Set CLEAR_RDB_AUTHENTICATED BIT only - all other bits must be zero */
	Register = BCHP_FIELD_DATA(HDMI_HDCP_CTL, I_CLEAR_RDB_AUTHENTICATED, 1) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_HDCP_CTL, Register) ;
	
	Register = BCHP_FIELD_DATA(HDMI_HDCP_CTL, I_CLEAR_RDB_AUTHENTICATED, 0) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_HDCP_CTL, Register) ;
	
#endif			

#if BHDM_CONFIG_HDCP_AUTO_RI_PJ_CHECKING_SUPPORT
	BHDM_HDCP_P_DisableAutoRiPjChecking (hHDMI);
#endif
	/* clear AuthenticatedLink Variable */
	hHDMI->HDCP_AuthenticatedLink = 0 ;
	
	/* Reset Ri counter */
	hHDMI->HDCP_RiCount = 0 ;

	BDBG_LEAVE(BHDM_HDCP_ClearAuthentication) ;
	return rc ;
	
} /* end BHDM_HDCP_ClearAuthentication */




/******************************************************************************
BERR_Code BHDM_HDCP_XmitEncrypted
Summary:Start transmitting video encrypted with HDCP.
*******************************************************************************/
BERR_Code BHDM_HDCP_XmitEncrypted(
   BHDM_Handle hHDMI              /* [in] HDMI handle */
) 
{
	BERR_Code   rc = BERR_SUCCESS;
	uint32_t    Register ;
	uint16_t JRate ;

	BDBG_ENTER(BHDM_HDCP_XmitEncrypted) ;
	BDBG_ASSERT( hHDMI );

	if ((hHDMI->HdcpVersion == BHDM_HDCP_Version_e1_1) /* same as hHDMI->HdcpVersion == BHDM_HDCP_Version_e1_0*/ 
	||	(hHDMI->HdcpVersion == BHDM_HDCP_Version_eUnused))
	{
		BDBG_MSG(("Using HDCP 1.1 Features")) ;
		
		Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_CP_INTEGRITY_CFG) ;
		Register &= ~BCHP_MASK(HDMI_CP_INTEGRITY_CFG, I_ALWAYS_REKEY_ON_VSYNC) ;
		BREG_Write32(hHDMI->hRegister, BCHP_HDMI_CP_INTEGRITY_CFG, Register) ;
	}
	else								                
	{
		BDBG_MSG(("Using HDCP 1.1 with Optional Features")) ;
		
		/* enable ReKey on Vsync for HDCP 1.1 Receivers */ 
		Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_CP_INTEGRITY_CFG) ;
		JRate = BCHP_GET_FIELD_DATA(Register, HDMI_CP_INTEGRITY_CFG, J_RATE_7_0) ;
		BDBG_MSG(("Pj Rate: %d frames", JRate)) ;

		Register &= ~BCHP_MASK(HDMI_CP_INTEGRITY_CFG, I_ALWAYS_REKEY_ON_VSYNC) ;
		Register |= BCHP_FIELD_DATA(HDMI_CP_INTEGRITY_CFG, I_ALWAYS_REKEY_ON_VSYNC, 1) ;
		BREG_Write32(hHDMI->hRegister, BCHP_HDMI_CP_INTEGRITY_CFG, Register) ;
	}


	/* enable (toggle) the Vsync for encryption */
	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_CP_CONFIG) ;
	Register &= ~BCHP_MASK(HDMI_CP_CONFIG, I_MUX_VSYNC) ;
	
	Register |= BCHP_FIELD_DATA(HDMI_CP_CONFIG, I_MUX_VSYNC, 1) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_CP_CONFIG, Register) ;

	BDBG_LEAVE(BHDM_HDCP_XmitEncrypted) ;
	return rc ;
} /* end BHDM_HDCP_XmitEncrypted */




/******************************************************************************
BERR_Code BHDM_HDCP_XmitClear
Summary: Start transmitting video without HDCP.
*******************************************************************************/
BERR_Code BHDM_HDCP_XmitClear(
   BHDM_Handle hHDMI              /* [in] HDMI handle */
)
{
	BERR_Code   rc = BERR_SUCCESS;
	uint32_t    Register ;

	BDBG_ENTER(BHDM_HDCP_XmitClear) ;
	BDBG_ASSERT( hHDMI );

	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_CP_CONFIG) ;
	Register &= ~BCHP_MASK(HDMI_CP_CONFIG, I_MUX_VSYNC) ;
	Register |= BCHP_FIELD_DATA(HDMI_CP_CONFIG, I_MUX_VSYNC, 0) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_CP_CONFIG, Register) ;
	
	BDBG_LEAVE(BHDM_HDCP_XmitClear) ;
	return rc ;
} /* end BHDM_HDCP_XmitClear */


#if BHDM_CONFIG_HDCP_AUTO_RI_PJ_CHECKING_SUPPORT
/******************************************************************************
BERR_Code BHDM_HDCP_P_AutoRiLinkIntegrityCheck
Summary: Use hardware Ri checking to verify the protected link is still valid.
*******************************************************************************/
BERR_Code BHDM_HDCP_P_AutoRiLinkIntegrityCheck(
   BHDM_Handle hHDMI              /* [in] HDMI handle */
) 
{
	BERR_Code	rc = BERR_SUCCESS;

	uint32_t	Register ;
	uint16_t	Ri ;
#if BHDM_CONFIG_DEBUG_PJ_CHECKING	
	uint8_t  Pj ;
#endif 
	
	BDBG_ENTER(BHDM_HDCP_P_AutoRiLinkIntegrityCheck) ;
	BDBG_ASSERT( hHDMI );

	/* Request to abort authentication */
	if (hHDMI->AbortHdcpAuthRequest)
	{
		BDBG_WRN(("HDCP Authentication Request Aborted....")) ;
		rc = BHDM_HDCP_AUTH_ABORTED ;
		goto done ;
	}

	/* Ri mismatch interrupt(s) fired. Either on the 127th frame (A) or the 0th frame (B) */
	if (hHDMI->HDCP_AutoRiMismatch_A || hHDMI->HDCP_AutoRiMismatch_B)
	{
		if (hHDMI->HDCP_AutoRiMismatch_A)
		{
			Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_CP_INTEGRITY_CHK_STATUS_2) ;
			Ri = BCHP_GET_FIELD_DATA(Register, HDMI_CP_INTEGRITY_CHK_STATUS_2, O_RI_A_SOURCE) ;
			BKNI_Memcpy(&hHDMI->HDCP_TxRi, &Ri, 2) ;

			Ri = BCHP_GET_FIELD_DATA(Register, HDMI_CP_INTEGRITY_CHK_STATUS_2, O_RI_A_SINK) ;
			BKNI_Memcpy(&hHDMI->HDCP_RxRi, &Ri, 2) ;			
		
			BDBG_WRN(("HDCP Ri Failure. Sink update Ri' too early"));
			BDBG_WRN(("LIC: Frame 127th: TxRi= %04x <> RxRi= %04x		  R%d !=",
				hHDMI->HDCP_TxRi, hHDMI->HDCP_RxRi, hHDMI->HDCP_RiCount)) ;
                        hHDMI->HDCP_AutoRiMismatch_A = false;
		}

		if (hHDMI->HDCP_AutoRiMismatch_B)
		{
			Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_CP_INTEGRITY_CHK_STATUS_3) ;
			Ri = BCHP_GET_FIELD_DATA(Register, HDMI_CP_INTEGRITY_CHK_STATUS_3, O_RI_B_SOURCE) ;
			BKNI_Memcpy(&hHDMI->HDCP_TxRi, &Ri, 2) ;

			Ri = BCHP_GET_FIELD_DATA(Register, HDMI_CP_INTEGRITY_CHK_STATUS_3, O_RI_B_SINK) ;
			BKNI_Memcpy(&hHDMI->HDCP_RxRi, &Ri, 2) ;			
		
			BDBG_WRN(("HDCP Ri Failure. Sink update Ri' too late"));
			BDBG_WRN(("LIC: Frame 0th: TxRi= %04x <> RxRi= %04x 		R%d !=",
				hHDMI->HDCP_TxRi, hHDMI->HDCP_RxRi, hHDMI->HDCP_RiCount)) ;
                        hHDMI->HDCP_AutoRiMismatch_B = false;
		}			

		/* Clear/Disable the HDCP Authentication */
		BHDM_HDCP_ClearAuthentication(hHDMI) ;
					
		rc = BHDM_HDCP_LINK_RI_FAILURE ;
		goto done;
	}

	/* No Ri mismatch interrupt was fired. Update Ri values */
	else
	{
		/* Read TxRi at the 0th frame. This is the start of the next 128 frames period, thus the updated Ri value */
		Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_CP_INTEGRITY) ;
		Ri = BCHP_GET_FIELD_DATA(Register, HDMI_CP_INTEGRITY, O_RI_15_0) ;

		BKNI_Memcpy(&hHDMI->HDCP_TxRi, &Ri, 2) ;

		if (! BKNI_Memcmp(&hHDMI->HDCP_TxRi, &hHDMI->HDCP_Ri2SecsAgo, 2))	/* No Ri change from 2secs ago */
		{
			/* 
			** The TxRi has not changed, so there is no need to read the RxRi.
			** just verify the TxRi value has changed in the last 6 seconds 
			*/

			if (BKNI_Memcmp(&hHDMI->HDCP_TxRi, &hHDMI->HDCP_Ri4SecsAgo, 2)) /* Ri changed from 4secs ago */
			{
				BKNI_Memcpy(&hHDMI->HDCP_Ri4SecsAgo, &hHDMI->HDCP_Ri2SecsAgo, 2) ;
				rc = BERR_SUCCESS ;
				goto done ;
			}

			if (BKNI_Memcmp(&hHDMI->HDCP_TxRi, &hHDMI->HDCP_Ri6SecsAgo, 2)) /* Ri changed from 6secs ago */
			{
				BKNI_Memcpy(&hHDMI->HDCP_Ri6SecsAgo, &hHDMI->HDCP_Ri4SecsAgo, 2) ;
				rc = BERR_SUCCESS ;
				goto done ;
			}	  

			/* 
			** Ri hasn't changed for six seconds 
			** Clear the Authentication and return a LINK FAILURE 
			*/
			BDBG_WRN(("Ri HDCP Link Integrity (Ri) values are not changing")) ;
			BDBG_WRN(("HDCP Link will shutdown")) ;

			/* Clear/Disable the HDCP Authentication */
			BHDM_HDCP_ClearAuthentication(hHDMI) ;

			rc = BHDM_HDCP_LINK_RI_FAILURE	; 
			goto done ;
		}


		if	((hHDMI->HDCP_TxRi == hHDMI->HDCP_Ri2SecsAgo)  /* No Ri change in 2secs */ 
		&& (hHDMI->HDCP_TxRi == hHDMI->HDCP_Ri4SecsAgo)  /* No Ri change in 4secs */
		&& (hHDMI->HDCP_TxRi == hHDMI->HDCP_Ri6SecsAgo)) /* No Ri change in 6secs */
		{
			/* 
			** if Ri has not changed in 6 seconds  (3 Consecutive Ri reads)
			** the HDCP link is no longer valid
			*/
			rc = BHDM_HDCP_LINK_RI_FAILURE	; 
			goto done ;
		}

		/* Print the Ri value at the last frame of the previous 128 frames period for debug information */
		Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_CP_INTEGRITY_CHK_STATUS_2) ;
		Ri = BCHP_GET_FIELD_DATA(Register, HDMI_CP_INTEGRITY_CHK_STATUS_2, O_RI_A_SOURCE) ; 

		BDBG_MSG(("LIC: At 127th frame:  Tx/Rx Ri= %04x 		R%d",				
			Ri, hHDMI->HDCP_RiCount-1)) ; 		
		BDBG_MSG(("LIC: At 0th frame:  Tx/Rx Ri= %04x			R%d",				
			hHDMI->HDCP_TxRi, hHDMI->HDCP_RiCount)) ;

#if BHDM_CONFIG_DEBUG_PJ_CHECKING 
		/* Pj values at  Ri updates should equal the LSB of the Ri value */
		/* Note the video pixel must be zero for this test */
		Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_CP_INTEGRITY) ;
		Pj = BCHP_GET_FIELD_DATA(Register, HDMI_CP_INTEGRITY, O_PJ_7_0) ;

		BDBG_MSG((" 			  %04x", Pj)) ;
#endif
		/*	increment  the RiCount only if Ri Changed in the last 2 secs */
		if (hHDMI->HDCP_TxRi != hHDMI->HDCP_Ri2SecsAgo)   
			hHDMI->HDCP_RiCount++ ;

		/* 
		** RiNSecsAgo values are initialized  
		** in BHDM_HDCP_AuthenticateLink
		*/
		hHDMI->HDCP_Ri6SecsAgo = hHDMI->HDCP_Ri4SecsAgo ;
		hHDMI->HDCP_Ri4SecsAgo = hHDMI->HDCP_Ri2SecsAgo ;
		hHDMI->HDCP_Ri2SecsAgo = hHDMI->HDCP_TxRi ;

		/* Return Link is Valid */
		rc = BERR_SUCCESS ;
		goto done ;

	}

done:	
	BDBG_LEAVE(BHDM_HDCP_P_AutoRiLinkIntegrityCheck) ;
	return rc ;

}


/******************************************************************************
BERR_Code BHDM_HDCP_P_AutoPjLinkIntegrityCheck
Summary: Use hardware Pj checking to verify the protected link is still valid.
*******************************************************************************/
BERR_Code BHDM_HDCP_P_AutoPjLinkIntegrityCheck(
   BHDM_Handle hHDMI              /* [in] HDMI handle */
) 
{
	BERR_Code   rc = BERR_SUCCESS;
	uint32_t    Register ;
	uint8_t 	Pj ;
	
	BDBG_ENTER(BHDM_HDCP_P_AutoPjLinkIntegrityCheck) ;
	BDBG_ASSERT( hHDMI );

#if BHDM_CONFIG_HDCP_AUTO_RI_PJ_CHECKING_SUPPORT

	/* Request to abort authentication */
	if (hHDMI->AbortHdcpAuthRequest)
	{
		BDBG_WRN(("HDCP Authentication Request Aborted....")) ;
		rc = BHDM_HDCP_AUTH_ABORTED ;
		goto done ;
	}

	/* Read Tx Pj updated value */
	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_CP_INTEGRITY_CHK_STATUS_1) ;
	Pj = BCHP_GET_FIELD_DATA(Register, HDMI_CP_INTEGRITY_CHK_STATUS_1, O_PJ_SOURCE) ;
	BKNI_Memcpy(&hHDMI->HDCP_TxPj, &Pj, 1) ;

	/* Pj values mismatched */
	if (hHDMI->HDCP_AutoPjMismatch)
	{
		Pj = BCHP_GET_FIELD_DATA(Register, HDMI_CP_INTEGRITY_CHK_STATUS_1, O_PJ_SINK) ;
		BKNI_Memcpy(&hHDMI->HDCP_RxPj, &Pj, 1) ;

		BDBG_WRN(("Pj LIC: TxPj= %02x <> RxPj= %02x !=",
			hHDMI->HDCP_TxPj, hHDMI->HDCP_RxPj)) ;

		/* could be a pixel transmission error; ignore if errs < MAX MisMatch */
		if (++hHDMI->HDCP_PjMismatchCount < BHDM_HDCP_MAX_PJ_MISMATCH)
		{
			BDBG_WRN(("Pixel Transmission Error?? Pj Mis-match Count %d", 
				hHDMI->HDCP_PjMismatchCount)) ;
		}
		else   /* otherwise fail the link */
		{
			rc = BHDM_HDCP_LINK_PJ_FAILURE ;
			
			/* Clear/Disable the HDCP Authentication */
			BHDM_HDCP_ClearAuthentication(hHDMI) ;
			goto done;
		}	
	}

	
	/* Pjs match... */
	BDBG_MSG(("Pj LIC: Tx/Rx Pj: %02x", Pj)) ;
	hHDMI->HDCP_PjMismatchCount = 0 ;
	
	/* Return Link is Valid */
	rc = BERR_SUCCESS ;;
	
#endif
		
done:
	BDBG_LEAVE(BHDM_HDCP_P_AutoPjLinkIntegrityCheck) ;
	return rc ;

}
#endif

/******************************************************************************
BERR_Code BHDM_HDCP_RiLinkIntegrityCheck
Summary: Use the HDCP Ri values to verify the protected link is still valid.
*******************************************************************************/
BERR_Code BHDM_HDCP_RiLinkIntegrityCheck(
   BHDM_Handle hHDMI              /* [in] HDMI handle */
) 
{
	BERR_Code   rc = BERR_SUCCESS;

	uint32_t    Register ;
	uint16_t    Ri ;
#if BHDM_CONFIG_DEBUG_PJ_CHECKING	
	uint8_t  Pj ;
#endif 
	uint8_t RetryCount = 0;
	
	BDBG_ENTER(BHDM_HDCP_RiLinkIntegrityCheck) ;
	BDBG_ASSERT( hHDMI );

#if BHDM_CONFIG_HDCP_AUTO_RI_PJ_CHECKING_SUPPORT
	/* Enable auto Ri checking */
	if (hHDMI->DeviceSettings.bEnableAutoRiPjChecking && hHDMI->bAutoRiPjCheckingEnabled)
        {
		rc = BHDM_HDCP_P_AutoRiLinkIntegrityCheck(hHDMI);
		goto done;
	}
#endif

#if 0
	/* TE for SimplayHD Test 7-11 appears to get confused if we attempt to read R0
	 * more than once. So don't retry if this LIC is for R0.
	 *
	 * Discovered that the new Quantum Data PS-980 has an internal problem with the 
	 * 1st (short-form) access to R0 but the 2nd attempt works fine. This is a
	 * good thing to have in general, anyway.
	 */
	 
	if (hHDMI->HDCP_RiCount == 0)
			RetryCount = BHDM_HDCP_MAX_I2C_RETRY;
#endif

	/* No Auto Ri/Pj checking is disabled or not supported */
	do
	{
		RetryCount++;
		
		if (hHDMI->AbortHdcpAuthRequest)
		{
			BDBG_WRN(("HDCP Authentication Request Aborted....")) ;
			rc = BHDM_HDCP_AUTH_ABORTED ;
			goto done ;
		}

		/* Read the Tx Ri value contained in the upper 16 bits of the Register */
		/* also get the Pj value */
		Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_CP_INTEGRITY) ;
		Ri = BCHP_GET_FIELD_DATA(Register, HDMI_CP_INTEGRITY, O_RI_15_0) ;

		
		BKNI_Memcpy(&hHDMI->HDCP_TxRi, &Ri, 2) ;

		if (! BKNI_Memcmp(&hHDMI->HDCP_TxRi, &hHDMI->HDCP_Ri2SecsAgo, 2))   /* No Ri change from 2secs ago */
		{
			/* 
			** The TxRi has not changed, so there is no need to read the RxRi.
			** just verify the TxRi value has changed in the last 6 seconds 
			*/

			if (BKNI_Memcmp(&hHDMI->HDCP_TxRi, &hHDMI->HDCP_Ri4SecsAgo, 2)) /* Ri changed from 4secs ago */
			{
				BKNI_Memcpy(&hHDMI->HDCP_Ri4SecsAgo, &hHDMI->HDCP_Ri2SecsAgo, 2) ;
				rc = BERR_SUCCESS ;
				goto done ;
			}

			if (BKNI_Memcmp(&hHDMI->HDCP_TxRi, &hHDMI->HDCP_Ri6SecsAgo, 2)) /* Ri changed from 6secs ago */
			{
				BKNI_Memcpy(&hHDMI->HDCP_Ri6SecsAgo, &hHDMI->HDCP_Ri4SecsAgo, 2) ;
				rc = BERR_SUCCESS ;
				goto done ;
			}	  

			/* 
			** Ri hasn't changed for six seconds 
			** Clear the Authentication and return a LINK FAILURE 
			*/
			BDBG_WRN(("Ri HDCP Link Integrity (Ri) values are not changing")) ;
			BDBG_WRN(("HDCP Link will shutdown")) ;

			/* Clear/Disable the HDCP Authentication */
			BHDM_HDCP_ClearAuthentication(hHDMI) ;

			rc = BHDM_HDCP_LINK_RI_FAILURE	; 
			goto done ;
		}

#if BHDM_CONFIG_REPEATER_SIMULATION_TEST
		hHDMI->HDCP_RxRi = hHDMI->HDCP_TxRi ;
		BDBG_MSG(("LIC: Tx/Rx Ri= %04x  %8d",
			hHDMI->HDCP_TxRi, hHDMI->HDCP_RiCount++)) ;
		return BERR_SUCCESS ;
#endif


/* 
** All HDCP Rx are required to support Short or Fast Read Configuration
** where a read without an offset indicates the default I2C offset of the 
** device should be used.   In the case of the HDCP Rx this is offset 
** 0x08 (Ri).  Some early DVI receivers did not properly implement the 
** Short Read resulting in Authentication errors 
**
** Enable the following macro in bhdm_config.h to enable the HDCP Ri Short Read
** and eliminate compliance test reports of long Ri reads.
*/
#if BHDM_CONFIG_HDCP_RI_SHORT_READ
#if defined(HUMAX_PLATFORM_BASE)
			if(RetryCount==(BHDM_HDCP_MAX_I2C_RETRY-1))
			{
				rc = BREG_I2C_Read(hHDMI->hI2cRegHandle, 
				BHDM_HDCP_RX_I2C_ADDR,	BHDM_HDCP_RX_RI0, (uint8_t *) &hHDMI->HDCP_RxRi, 2) ;
			}
			else
#endif
                rc = BREG_I2C_ReadNoAddr(hHDMI->hI2cRegHandle, 
				BHDM_HDCP_RX_I2C_ADDR, (uint8_t *) &hHDMI->HDCP_RxRi, 2) ;
#else
                rc = BREG_I2C_Read(hHDMI->hI2cRegHandle, 
				BHDM_HDCP_RX_I2C_ADDR,	BHDM_HDCP_RX_RI0, (uint8_t *) &hHDMI->HDCP_RxRi, 2) ;
#endif

		if (rc != BERR_SUCCESS)
		{
			BDBG_ERR(("Ri LIC: RxRi I2C read failure %x", rc)) ;
			continue ;
		}
		
		/* little endian number */		
		BREG_LE16(hHDMI->HDCP_RxRi) ;


		/* if RIs match... */
		if  (hHDMI->HDCP_TxRi == hHDMI->HDCP_RxRi)
		{
			/* make sure the Ri values are changing...*/
			
			if  ((hHDMI->HDCP_TxRi == hHDMI->HDCP_Ri2SecsAgo)  /* No Ri change in 2secs */ 
			&& (hHDMI->HDCP_TxRi == hHDMI->HDCP_Ri4SecsAgo)  /* No Ri change in 4secs */
			&& (hHDMI->HDCP_TxRi == hHDMI->HDCP_Ri6SecsAgo)) /* No Ri change in 6secs */
			{
				/* 
				** if Ri has not changed in 6 seconds  (3 Consecutive Ri reads)
				** the HDCP link is no longer valid
				*/
				rc = BHDM_HDCP_LINK_RI_FAILURE	; 
				goto done ;
			}

			BDBG_MSG(("LIC: Tx/Rx Ri= %04x          R%d",
				hHDMI->HDCP_TxRi, hHDMI->HDCP_RiCount)) ;

			
#if BHDM_CONFIG_DEBUG_PJ_CHECKING	
			/* read the Integrity register a 2nd time to ensure Pj value has updated */
			/* Pj values at  Ri updates should equal the LSB of the Ri value */
			/* Note the video pixel must be zero for this test */
			Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_CP_INTEGRITY) ;
			Pj = BCHP_GET_FIELD_DATA(Register, HDMI_CP_INTEGRITY, O_PJ_7_0) ;
		
			BDBG_MSG(("               %04x", Pj)) ;
#endif
			/*  increment  the RiCount only if Ri Changed in the last 2 secs */
			if (hHDMI->HDCP_TxRi != hHDMI->HDCP_Ri2SecsAgo)   
				hHDMI->HDCP_RiCount++ ;
			
			/* 
			** RiNSecsAgo values are initialized  
			** in BHDM_HDCP_AuthenticateLink
			*/
			hHDMI->HDCP_Ri6SecsAgo = hHDMI->HDCP_Ri4SecsAgo ;
			hHDMI->HDCP_Ri4SecsAgo = hHDMI->HDCP_Ri2SecsAgo ;
			hHDMI->HDCP_Ri2SecsAgo = hHDMI->HDCP_TxRi ;
			

			/* Return Link is Valid */
			rc = BERR_SUCCESS ;
			goto done ;
		}

		/* Ri values don't match; try reading again up to 4 times */

		/* 
		** BDBG_MSG Display the different Ri Values read 
		** Add a pattern "!=" to the display message so differences can 
		** be easily found in any generated logs.
		**
		** Most of the time this will be a transition from one Ri 
		** value to the next; 
		** the next Ris read should result in equal Ri values
		*/
		BDBG_WRN(("LIC: TxRi= %04x <> RxRi= %04x         R%d !=",
			hHDMI->HDCP_TxRi, hHDMI->HDCP_RxRi, hHDMI->HDCP_RiCount)) ;

		rc = BHDM_HDCP_LINK_RI_FAILURE ;
	} while (RetryCount < BHDM_HDCP_MAX_I2C_RETRY) ;

	/* Clear/Disable the HDCP Authentication */
	BHDM_HDCP_ClearAuthentication(hHDMI) ;
	
done:	
	BDBG_LEAVE(BHDM_HDCP_RiLinkIntegrityCheck) ;
	return rc ;
}


/******************************************************************************
BERR_Code BHDM_HDCP_PjLinkIntegrityCheck
Summary: Use the HDCP Ri values to verify the protected link is still valid.
*******************************************************************************/
BERR_Code BHDM_HDCP_PjLinkIntegrityCheck(
   BHDM_Handle hHDMI              /* [in] HDMI handle */
) 
{
	BERR_Code   rc = BERR_SUCCESS;

	uint32_t    Register ;
	uint8_t 	Pj ;
	uint8_t RetryCount = 0;
	
	BDBG_ENTER(BHDM_HDCP_PjLinkIntegrityCheck) ;
	BDBG_ASSERT( hHDMI );

#if BHDM_CONFIG_HDCP_AUTO_RI_PJ_CHECKING_SUPPORT
	if (hHDMI->DeviceSettings.bEnableAutoRiPjChecking && hHDMI->bAutoRiPjCheckingEnabled)
	{
		rc = BHDM_HDCP_P_AutoPjLinkIntegrityCheck(hHDMI);
		goto done;
	}
#endif

	do
	{
		RetryCount++;

		/* Read the Tx Ri value contained in the upper 16 bits of the Register */
		/* also get the Pj value */
		Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_CP_INTEGRITY) ;
		Pj = BCHP_GET_FIELD_DATA(Register, HDMI_CP_INTEGRITY, O_PJ_7_0) ;

		/* 
		** There is no guarantee the connected Rx supports the HDCP I2C Short 
		** Read format, therefore use a standard I2C read for the Receiver
		*/
                rc = BREG_I2C_Read(hHDMI->hI2cRegHandle, 
				BHDM_HDCP_RX_I2C_ADDR, BHDM_HDCP_RX_PJ, (uint8_t *) &hHDMI->HDCP_RxPj, 1) ;
		if (rc != BERR_SUCCESS)
		{
			BDBG_ERR(("Pj LIC: Rx Pj I2C read failure %d", rc));
			continue  ;
		}

		/* Pjs match... */
		if  (hHDMI->HDCP_RxPj == Pj)
		{
			BDBG_MSG(("Pj LIC: Tx/Rx Pj: %02x", Pj)) ;
			hHDMI->HDCP_PjMismatchCount = 0 ;
			hHDMI->HdcpOptions.numPjFailures = 0;
			
			/* Return Link is Valid */
			rc = BERR_SUCCESS ;
			goto done ;
		}


		/* Pj values don't match; try reading again up to MAX_I2C_RETRY times */

		/* 
		** BDBG_MSG Display the different Pj Values read 
		** Add a pattern "!=" to the display message so differences can 
		** be easily found in any generated logs.
		**
		** Most of the time this will be a transition from one Pj 
		** value to the next; 
		** the next Pjs read should result in equal Pj values
		*/
		BDBG_WRN(("Pj LIC: TxPj= %02x <> RxPj= %02x %d !=",
			Pj, hHDMI->HDCP_RxPj, RetryCount)) ;
	} while (RetryCount < BHDM_HDCP_MAX_I2C_RETRY) ;

	/* could be a pixel transmission error; ignore if errs < MAX MisMatch */
	if (++hHDMI->HDCP_PjMismatchCount < BHDM_HDCP_MAX_PJ_MISMATCH)
	{
		BDBG_WRN(("Pixel Transmission Error?? Pj Mis-match Count %d", 
			hHDMI->HDCP_PjMismatchCount)) ;
	}
	else   /* otherwise fail the link */
	{
		rc = BHDM_HDCP_LINK_PJ_FAILURE ;
		hHDMI->HdcpOptions.numPjFailures++;
		BDBG_MSG(("%d consecutive Pj link failures", hHDMI->HdcpOptions.numPjFailures));

		/* Clear/Disable the HDCP Authentication */
		BHDM_HDCP_ClearAuthentication(hHDMI) ;
	}


done:
	BDBG_LEAVE(BHDM_HDCP_PjLinkIntegrityCheck) ;
	return rc ;
}


/******************************************************************************
BERR_Code BHDM_HDCP_GetRxCaps
Summary: Retrieve the Rx Capabilities.
*******************************************************************************/
BERR_Code BHDM_HDCP_GetRxCaps(
   BHDM_Handle hHDMI,         /* [in] HDMI handle */
   uint8_t *pRxCapsReg     /* [out] HDCP Rx Capability */
) 
{
	BERR_Code   rc = BERR_SUCCESS;

	BDBG_ENTER(BHDM_HDCP_GetRxCaps) ;
	BDBG_ASSERT( hHDMI ) ;

	/* if get RxCaps is being used to determine if the KSV FIFO is ready */
	/* return an HDCP Abort if a HP has occurred while waiting for KsvFifoRdy bit */
	if ((hHDMI->AbortHdcpAuthRequest) && (hHDMI->HDCP_AuthenticatedLink))
	{
		BDBG_WRN(("HDCP Authentication Request Aborted....")) ;
		rc = BHDM_HDCP_AUTH_ABORTED ;
		hHDMI->AbortHdcpAuthRequest = 0 ;
		goto done ;
	}

#if BHDM_CONFIG_HDCP_AUTO_RI_PJ_CHECKING_SUPPORT
	/* We can't access I2C if control has been handed over to HW */
	if (hHDMI->bAutoRiPjCheckingEnabled)
	{
		*pRxCapsReg = hHDMI->RxBCaps;
	}
	else
	{	
		BHDM_CHECK_RC(rc, BREG_I2C_Read(hHDMI->hI2cRegHandle, 
			BHDM_HDCP_RX_I2C_ADDR, BHDM_HDCP_RX_BCAPS, pRxCapsReg, 1)) ;
	}
#else
		BHDM_CHECK_RC(rc, BREG_I2C_Read(hHDMI->hI2cRegHandle, 
			BHDM_HDCP_RX_I2C_ADDR, BHDM_HDCP_RX_BCAPS, pRxCapsReg, 1)) ;
#endif


#if 0
	/* debug to force Pj values */
	/* if Rx does not support Pj auth errors will occur */
	*pRxCapsReg	|= BHDM_HDCP_RxCaps_eHDCP_1_1_Features ;
#endif

#if BHDM_CONFIG_REPEATER_SIMULATION_TEST
	*pRxCapsReg |=BHDM_HDCP_RxCaps_eHdcpRepeater ;
#endif	

	/* update settings only if RxCaps has changed */
	if (hHDMI->RxBCaps == *pRxCapsReg)
		goto done ;

#if BDBG_DEBUG_BUILD
	if ( *pRxCapsReg & BHDM_HDCP_RxCaps_eHdcpRepeater) {
		BDBG_MSG(("Repeater Bcaps (0x40): %#x", *pRxCapsReg)) ;
	} else {
		BDBG_MSG(("Receiver Bcaps (0x40): %#x", *pRxCapsReg)) ;
	}
#endif	
		
#if BHDM_CONFIG_DEBUG_HDCP_BCAPS
	BDBG_MSG(("-----------------------")) ;
	BDBG_MSG(("HDMI Rsvd(7):  %d", 
		*pRxCapsReg & BHDM_HDCP_RxCaps_eHdmiCapable ? 1 : 0)) ;
	BDBG_MSG(("Repeater(6):   %d", 
		*pRxCapsReg & BHDM_HDCP_RxCaps_eHdcpRepeater ? 1 : 0)) ;
	if (*pRxCapsReg & BHDM_HDCP_RxCaps_eHdcpRepeater)
		BDBG_MSG(("   KSV FIFO Rdy (5): %d", 
			*pRxCapsReg & BHDM_HDCP_RxCaps_eKsvFifoReady ? 1 : 0)) ;
	BDBG_MSG(("Fast I2C(4):   %d", 
		*pRxCapsReg & BHDM_HDCP_RxCaps_eI2c400KhzSupport ? 1 : 0)) ;
	BDBG_MSG(("HDCP 1.1 with Optional Features(1):   %d", 
		*pRxCapsReg & BHDM_HDCP_RxCaps_eHDCP_1_1_Features ? 1 : 0)) ;
	BDBG_MSG(("FastReauth(0): %d", 
		*pRxCapsReg & BHDM_HDCP_RxCaps_eFastReauth ? 1 : 0)) ;
#endif		

	/* If attached Rx supports HDCP 1.1 with optional features, option Pj checking desired and 
	the number of Pj link failures <= the max failures allow, enable HDCP 1.1 with optional features */
	if ((*pRxCapsReg & BHDM_HDCP_RxCaps_eHDCP_1_1_Features) 
	&& (hHDMI->HdcpOptions.PjChecking)
	&& (hHDMI->HdcpOptions.numPjFailures <= BHDM_HDCP_MAX_PJ_LINK_FAILURES_BEFORE_DISABLE_HDCP_1_1))   
	{
		/* tell the Rx the our HDMI core supports HDCP 1.1 with optional features as well */
		hHDMI->HdcpVersion = BHDM_HDCP_Version_e1_1_Optional_Features ;

		/* turn on the callbacks for Pj interrupts */
		BHDM_CHECK_RC(rc, BINT_EnableCallback( hHDMI->hCallback[MAKE_INTR_ENUM(HDCP_PJ)] )) ;

#if BHDM_CONFIG_HDCP_AUTO_RI_PJ_CHECKING_SUPPORT
		BHDM_CHECK_RC(rc, BINT_EnableCallback( hHDMI->hCallback[MAKE_INTR_ENUM(HDCP_PJ_MISMATCH)] )) ;
#endif

	}
	else
	{
		hHDMI->HdcpVersion = BHDM_HDCP_Version_e1_1 ;
		/* HDCP 1.1 only; disable the Pj interrupts */
		BHDM_CHECK_RC(rc, BINT_DisableCallback( hHDMI->hCallback[MAKE_INTR_ENUM(HDCP_PJ)] )) ;

#if BHDM_CONFIG_HDCP_AUTO_RI_PJ_CHECKING_SUPPORT
		BHDM_CHECK_RC(rc, BINT_DisableCallback( hHDMI->hCallback[MAKE_INTR_ENUM(HDCP_PJ_MISMATCH)] )) ;
#endif		
	}
	

	hHDMI->RxBCaps = *pRxCapsReg ;

done:	

	BDBG_LEAVE(BHDM_HDCP_GetRxCaps) ;
	return rc ;
} /* end BHDM_HDCP_GetRxCaps */




/******************************************************************************
BERR_Code BHDM_HDCP_GetRxStatus
Summary: Check the status of the attached receiver.
*******************************************************************************/
BERR_Code BHDM_HDCP_GetRxStatus(
   BHDM_Handle hHDMI,                /* [in] HDMI handle */
   uint16_t *pRxStatusReg          /* [out] HDCP Rx Status */
) 
{
	BERR_Code   rc = BERR_SUCCESS;
	uint16_t Status ;

	BDBG_ENTER(BHDM_HDCP_GetRxStatus) ;
	BDBG_ASSERT( hHDMI );

#if BHDM_CONFIG_HDCP_AUTO_RI_PJ_CHECKING_SUPPORT
        /* We can't access I2C if control has been handed over to HW */
        if (hHDMI->bAutoRiPjCheckingEnabled)
        {
                *pRxStatusReg = hHDMI->RxStatus;
                goto done;
        }
#endif

        BHDM_CHECK_RC(rc, BREG_I2C_Read(hHDMI->hI2cRegHandle, 
			BHDM_HDCP_RX_I2C_ADDR, BHDM_HDCP_RX_BSTATUS, (uint8_t *) &Status, 2)) ;
		
	BREG_LE16(Status) ;
		
	BDBG_MSG(("Rx BStatus (0x41): %#x", Status)) ;
	BDBG_MSG(("------------------------")) ;
	BDBG_MSG(("HDMI Mode:            %d", 
		Status & BHDM_HDCP_RxStatus_eHdmiMode ? 1 : 0)) ;
	BDBG_MSG(("MAX Cascade Exceeded: %d", 
		Status & BHDM_HDCP_RxStatus_eMaxRepeatersExceeded ? 1 : 0)) ;
	BDBG_MSG(("Max Devs Exceeded:    %d", 
		Status & BHDM_HDCP_RxStatus_eMaxDevicesExceeded ? 1 : 0)) ;

	*pRxStatusReg = Status ;

#if BHDM_CONFIG_HDCP_AUTO_RI_PJ_CHECKING_SUPPORT
	hHDMI->RxStatus =  *pRxStatusReg ;
#endif


done:	
	BDBG_LEAVE(BHDM_HDCP_GetRxStatus) ;
	return rc ;
} /* end BHDM_HDCP_GetRxStatus */



/******************************************************************************
BERR_Code BHDM_HDCP_CheckForRepeater
Summary: Check if the attached receiver is an HDCP Receiver.
*******************************************************************************/
BERR_Code BHDM_HDCP_CheckForRepeater(
   BHDM_Handle hHDMI,                /* [in] HDMI handle */
   uint8_t *uiRepeater          /* [out] HDCP Rx Status */
) 
{
	BERR_Code   rc = BERR_SUCCESS;
	uint8_t RxCaps ;

	BDBG_ENTER(BHDM_HDCP_CheckForRepeater) ;
	BDBG_ASSERT( hHDMI );

	BHDM_CHECK_RC(rc, BHDM_HDCP_GetRxCaps(hHDMI, &RxCaps)) ;
	if (RxCaps & BHDM_HDCP_RxCaps_eHdcpRepeater)
	{
		BDBG_MSG(("HDCP Repeater Found")) ;
		*uiRepeater = 1 ;
	}
	else
	{
		BDBG_MSG(("HDCP Standard Receiver Found")) ;
		*uiRepeater = 0 ;
	}
#if BHDM_CONFIG_REPEATER_SIMULATION_TEST
		*uiRepeater = 1 ;
		BDBG_WRN(("HDCP Repeater Found")) ;
#endif
done:	
	BDBG_LEAVE(BHDM_HDCP_CheckForRepeater) ;
	return rc ;
} /* end BHDM_HDCP_CheckForRepeater */


/******************************************************************************
BERR_Code BHDM_HDCP_GetRepeaterDepth
Summary: Get the depth (number of levels) of HDCP Repeaters
*******************************************************************************/
BERR_Code BHDM_HDCP_GetRepeaterDepth(
   BHDM_Handle hHDMI,      /* [in] HDMI handle */
   uint8_t *pRepeaterDepth /* [out] pointer to hold Levels of HDCP Repeaters */
) 
{
	BERR_Code   rc = BERR_SUCCESS;
	uint16_t    BStatus = 0 ;

	BDBG_ENTER(BHDM_HDCP_GetRepeaterDepth) ;
	BDBG_ASSERT( hHDMI );


	BHDM_CHECK_RC(rc, BHDM_HDCP_GetRxStatus(hHDMI, &BStatus)) ;
	
	/* shift the BStatus Register to get the Repeater Depth; HDCP Spec Table 2-4 */
	BStatus = (uint16_t) (BStatus >> 8) ;
	
	BStatus = BStatus & 0x07 ;
	*pRepeaterDepth = (uint8_t) BStatus ;
	BDBG_MSG(("Repeater Depth: %X", BStatus)) ;

done:	
	BDBG_LEAVE(BHDM_HDCP_GetRepeaterDepth) ;
	return rc ;
} /* end BHDM_HDCP_GetRepeaterDepth */




/******************************************************************************
BERR_Code BHDM_HDCP_GetRepeaterDeviceCount
Summary: Get the number of devices attached to the HDCP Repeater.
*******************************************************************************/
BERR_Code BHDM_HDCP_GetRepeaterDeviceCount(
   BHDM_Handle hHDMI,   /* [in] HDMI handle */
   uint8_t *pNumDevices /* [out] pointer to # of devices attached to the HDCP 
                           Repeater */
) 
{
	BERR_Code   rc = BERR_SUCCESS;
	uint16_t    BStatus = 0 ;

	BDBG_ENTER(BHDM_HDCP_GetRepeaterDeviceCount) ;
	BDBG_ASSERT( hHDMI );

	BHDM_CHECK_RC(rc, BHDM_HDCP_GetRxStatus(hHDMI, &BStatus)) ;

	/* AND the Device Count bits in the BStatus Register to get the Device Count */
	/* See HDCP Spec */
	BStatus = (uint8_t) (BStatus & 0x7F) ;
	BDBG_MSG(("Repeater Device Count: %X", BStatus)) ;

	/* copy the Repeater Depth Value */
	*pNumDevices = (uint8_t) BStatus ;

done :	
	BDBG_LEAVE(BHDM_HDCP_GetRepeaterDeviceCount) ;
	return rc ;
} /* end BHDM_HDCP_GetRepeaterDeviceCount */




/******************************************************************************
BERR_Code BHDM_HDCP_ReadRepeaterKsvFIFO
Summary: Read the KSV list from the attached repeater.
*******************************************************************************/
BERR_Code BHDM_HDCP_ReadRxRepeaterKsvFIFO(
   BHDM_Handle hHDMI,      /* [in] HDMI handle */
   uint8_t *pRxKsvList,    /* [out] pointer to memory to hold KSV List */
   uint16_t uiRxDeviceCount,  /* [in ] number of Ksvs in the Rx KSV List */
   const uint8_t *pRevokedKsvList,  /* [in] pointer to Revoked KSV List */
   const uint16_t uiRevokedKsvCount /* [in] number of KSVs in Revoked Ksv List */
) 
{
	BERR_Code   rc = BERR_SUCCESS;
	
	uint16_t uiBufSize ;
	bool bRevoked ;
	uint8_t iNumKsvBytes ;

#if BHDM_CONFIG_REPEATER_SIMULATION_TEST
	uint8_t TestKsvFifo[] = 
	{
		0x2e, 0x17, 0x6a, 0x79, 0x35, 
		0x0f, 0xe2, 0x71, 0x8e, 0x47, 
		0xa6, 0x97, 0x53, 0xe8, 0x74
	} ;
#endif 

	BDBG_ENTER(BHDM_HDCP_ReadRxRepeaterKsvFIFO) ;
	BDBG_ASSERT( hHDMI );

	uiBufSize = uiRxDeviceCount * BHDM_HDCP_KSV_LENGTH ;

	/* Read the KSV FIFO */
	/* NOTE: the KSV_FIFO offset in the receiver does not auto-increment when read... 
	** all values come from the same offset
	*/
		
#if BHDM_CONFIG_REPEATER_SIMULATION_TEST
	pRxKsvList = &TestKsvFifo[0] ;
#else
	BHDM_CHECK_RC(rc, BREG_I2C_Read(hHDMI->hI2cRegHandle, 
		BHDM_HDCP_RX_I2C_ADDR, BHDM_HDCP_REPEATER_KSV_FIFO, pRxKsvList, uiBufSize )) ;
#endif		


		
#if BDBG_DEBUG_BUILD
	{
		uint16_t    i ;
		BDBG_MSG(("Downstream Devices KSVs: %d", uiRxDeviceCount)) ;
			
		for (i = 0 ; i < uiRxDeviceCount; i++) 
		{
			BDBG_MSG(("KSV[%d]: = %02X %02X %02X %02X %02X ", 
				i, pRxKsvList[i*5+4], pRxKsvList[i*5+3], pRxKsvList[i*5+2], 
				pRxKsvList[i*5+1], pRxKsvList[i*5])) ;
		}
	}
#endif	

	/* keep a copy of the attached ksv list */
	/* free any previous list that may have been allocated */
	if (hHDMI->HDCP_RepeaterKsvList)
		BKNI_Free(hHDMI->HDCP_RepeaterKsvList) ;
	
		
	hHDMI->HDCP_RepeaterDeviceCount = uiRxDeviceCount ;
	iNumKsvBytes = (uint16_t) (hHDMI->HDCP_RepeaterDeviceCount * BHDM_HDCP_KSV_LENGTH) ;
	
	/* there could be no devices attached to the repeater; finished */
	if (!iNumKsvBytes)
	{
		hHDMI->HDCP_RepeaterKsvList  = (uint8_t *) 0  ;
		goto done ;
	}
	
	hHDMI->HDCP_RepeaterKsvList  = (uint8_t *) BKNI_Malloc(sizeof(uint8_t) * iNumKsvBytes) ;
	BKNI_Memcpy(hHDMI->HDCP_RepeaterKsvList, pRxKsvList, iNumKsvBytes) ;


	/* check if the Repeater's Ksv List has any revoked Ksvs */
	if (uiRevokedKsvCount)
	{
		BHDM_HDCP_P_CheckRevokedKsvList(
			(uint8_t *) pRevokedKsvList, uiRevokedKsvCount, 
			pRxKsvList, uiRxDeviceCount, &bRevoked) ;
		
		if (bRevoked)
		{
			rc = BHDM_HDCP_RX_BKSV_REVOKED ;
			goto done ;
		}
	}

done:	
	BDBG_LEAVE(BHDM_HDCP_ReadRxRepeaterKsvFIFO) ;
	return rc ;
} /* end BHDM_HDCP_ReadRepeaterKsvFIFO */



/******************************************************************************
BERR_Code BHDM_HDCP_WriteTxKsvFIFO
Summary: Write the KSV list retrieved from the attached repeater to the 
Transmitter core
*******************************************************************************/
BERR_Code BHDM_HDCP_WriteTxKsvFIFO(
	BHDM_Handle hHDMI, 
	uint8_t *KsvList, 
	uint16_t uiRxDeviceCount)
{
	BERR_Code rc = BERR_SUCCESS ;
	uint8_t     
		i, j ;
		
	uint8_t   
		timeoutMs,
		Ksv[5] ;

	uint32_t    
		Register,
		BksvRegisterValue,
		RepeaterBusy ;
		
#if BHDM_CONFIG_REPEATER_SIMULATION_TEST
	uint8_t SHA_1[] =
	{
		0x86, 0xd5, 0xcb, 0x0f,
		0xef, 0x07, 0xc1, 0xef,
		0x1d, 0x0a, 0xd7, 0xcc,
		0xda, 0x6d, 0x18, 0xb1,
		0x5e, 0xff, 0xb3, 0x1f
	} ;
	
#endif

	BDBG_ENTER(BHDM_HDCP_WriteTxKsvFIFO) ;
	BDBG_ASSERT( hHDMI );

	for (i = 0 ; i < uiRxDeviceCount; i++)
	{
		
		/* wait until the Tx Repeater core is ready for a Ksv */
		timeoutMs = 10 ;
		do
		{
			Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_CP_STATUS) ;
			RepeaterBusy = BCHP_GET_FIELD_DATA(Register, HDMI_CP_STATUS, O_REPEATER_IS_BUSY) ;
			if (!RepeaterBusy)
				break ;
				
			BKNI_Sleep(1) ;
			BDBG_WRN(("Tx Repeater Core Busy!")) ;
		} while ( timeoutMs-- ) ;


		/* copy the next Ksv */
		for (j = 0 ; j < BHDM_HDCP_KSV_LENGTH ; j++)
			Ksv[j] = KsvList[BHDM_HDCP_KSV_LENGTH * i  + j] ;

		/* write the 4 LSBs RxBksv to the transmitter... */
		BksvRegisterValue = 
			  Ksv[0]
			| Ksv[1] <<  8
			| Ksv[2] << 16
			| Ksv[3] << 24 ;
		
		BREG_Write32(hHDMI->hRegister, BCHP_HDMI_KSV_FIFO_0, BksvRegisterValue) ;
#if 0
		BDBG_MSG((" Key[%d] %X", i, BksvRegisterValue)) ;
#endif		
		
		/* write the 1 MSB RxBksv to the transmitter... */
		BksvRegisterValue = Ksv[4] ;
		BREG_Write32(hHDMI->hRegister, BCHP_HDMI_KSV_FIFO_1, BksvRegisterValue) ;
#if 0		
		BDBG_MSG((" Key[%d] MSB %X", i, BksvRegisterValue)) ;
#endif		
	}
	
	BDBG_LEAVE(BHDM_HDCP_WriteTxKsvFIFO) ;
	return rc ;
}

/******************************************************************************
BERR_Code BHDM_HDCP_IsLinkAuthenticated
Summary: Check if the current link is authenticated.
*******************************************************************************/
BERR_Code BHDM_HDCP_IsLinkAuthenticated(BHDM_Handle hHDMI)
{
	if (hHDMI->HDCP_AuthenticatedLink)
		return BERR_SUCCESS ;
	else
		return BHDM_HDCP_NO_AUTHENTICATED_LINK ;
}


#define DUMP_CP_STATUS 	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_CP_STATUS) ; \
	BDBG_MSG(("CP_STATUS: %04X", Register)) ;

/******************************************************************************
BERR_Code BHDM_HDCP_InitializeRepeaterAuthentication 
Summary: Initialize the HDCP Core to authenticate the attached repeater 
*******************************************************************************/
BERR_Code BHDM_HDCP_InitializeRepeaterAuthentication(BHDM_Handle hHDMI)
{
	BERR_Code rc = BERR_SUCCESS ;

	uint32_t Register ;
	uint16_t BStatus ;
	
	/* load the initial values to the core */
	/* BCaps was already written by BHDM_ReadRxBksv */
        BHDM_CHECK_RC(rc, BREG_I2C_Read(hHDMI->hI2cRegHandle, 
			BHDM_HDCP_RX_I2C_ADDR, BHDM_HDCP_RX_BSTATUS, (unsigned char *) &BStatus, 2)) ;

	BREG_LE16(BStatus) ;

	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_BKSV1) ;
	Register &= ~BCHP_MASK(HDMI_BKSV1, I_BSTATUS_15_0) ;

	Register |=  BCHP_FIELD_DATA(HDMI_BKSV1, I_BSTATUS_15_0, BStatus) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_BKSV1, Register) ;

#if 0
	BDBG_MSG(("Bksv1: %X", Register)) ;
#endif	

	DUMP_CP_STATUS	;

	/* initilize Repeater logic */
	Register = BCHP_MASK(HDMI_HDCP_CTL, I_INIT_REPEATER) ;
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_HDCP_CTL, Register) ;
	
	DUMP_CP_STATUS	;
done:	
	return rc ;
}


/******************************************************************************
BERR_Code BHDM_HDCP_RepeaterAuthentication 
Summary: Initialize the HDCP Core to authenticate the attached repeater 
*******************************************************************************/
BERR_Code BHDM_HDCP_RepeaterAuthenticateLink(BHDM_Handle hHDMI, uint8_t *RepeaterAuthenticated) 
{
	BERR_Code rc ;
	uint8_t   
		i,
		timeoutMs,
		RepeaterBusy,
		Ksv[5] ;
		
	
	uint32_t Register ;
	uint32_t BksvRegisterValue ;


	/* wait for core to finish V calculation */
	timeoutMs = 10 ;
	do
	{
		Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_CP_STATUS) ;
		RepeaterBusy = BCHP_GET_FIELD_DATA(Register, HDMI_CP_STATUS, O_REPEATER_IS_BUSY) ;
		if (!RepeaterBusy)
			break ;
			
		BKNI_Sleep(1) ;
		BDBG_MSG(("Waiting for TxCore Repeater V Calculation...")) ;
	} while ( timeoutMs-- ) ;

	
	if (RepeaterBusy)
	{
		BDBG_ERR(("TxCore failed Repeater V Calculation"));
		*RepeaterAuthenticated = 0 ; 
		goto done ;
	}
		
	
	/* 
	-- now read/load the results 160 byte SHA-1 Hash from the receiver... 
	-- use the Ksv to store each 32 bit part of the V value 
	*/

	for (i = 0; i < 5; i++)
	{
#if BHDM_CONFIG_REPEATER_SIMULATION_TEST
		BKNI_Memcpy(&BksvRegisterValue, (uint8_t *) &SHA_1[i*4],  4) ;
#else
                BHDM_CHECK_RC(rc, BREG_I2C_Read(hHDMI->hI2cRegHandle, 
				BHDM_HDCP_RX_I2C_ADDR,	BHDM_HDCP_REPEATER_SHA1_V_H0 + (i * 4), Ksv, 4 )) ;

		BksvRegisterValue =
		  Ksv[0]
		| (Ksv[1] <<  8)
		| (Ksv[2] << 16)
		| (Ksv[3] << 24) ;
#endif
		BDBG_MSG(("SHA-1 H%d Key = %X", i, BksvRegisterValue)) ;

		BREG_Write32(hHDMI->hRegister, BCHP_HDMI_V, BksvRegisterValue) ; 
	}
	

	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_CP_STATUS) ;
	if (Register & BCHP_MASK(HDMI_CP_STATUS, O_V_MATCH))
	{
		BDBG_MSG(("Repeater AUTHENTICATED")) ;
		*RepeaterAuthenticated = 1 ; 

#if BHDM_CONFIG_HDCP_AUTO_RI_PJ_CHECKING_SUPPORT
                if (hHDMI->DeviceSettings.bEnableAutoRiPjChecking) {
                        BHDM_HDCP_P_EnableAutoRiPjChecking(hHDMI, hHDMI->HdcpOptions.PjChecking) ;
                }
#endif
	}
	else
	{
		BDBG_ERR(("Repeater FAILED Authentication")) ;
		*RepeaterAuthenticated = 0 ; 
	}
	DUMP_CP_STATUS	;

done:	
	return BERR_SUCCESS ;
}


/******************************************************************************
BERR_Code BHDM_HDCP_GetRxKsv
Summary: Return KSV information obtained from the authentication
*******************************************************************************/
BERR_Code BHDM_HDCP_GetRxKsv(BHDM_Handle hHDMI, 
	uint8_t *AttachedDeviceKsv
)
{
	BERR_Code rc = BERR_SUCCESS ;
	uint8_t RxDeviceAttached ;

	/* make sure HDMI Cable is connected to something... */
	BHDM_CHECK_RC(rc, BHDM_RxDeviceAttached(hHDMI, &RxDeviceAttached));
	if (!RxDeviceAttached)
	{
		rc = BHDM_NO_RX_DEVICE ;
		goto done ;
	}
	
	/* make sure the Rx has been read;  */
	BHDM_CHECK_RC(rc, BHDM_HDCP_ReadRxBksv(hHDMI, (uint8_t *) 0, 0)) ; 

	/* copy the KSV of the attached device */
	BKNI_Memcpy(AttachedDeviceKsv, hHDMI->HDCP_RxKsv, BHDM_HDCP_KSV_LENGTH) ;
	
done:
	return rc ;	
}


/******************************************************************************
BERR_Code BHDM_HDCP_GetRepeaterKsvFifo
Summary: Return KSV information obtained from the authentication
*******************************************************************************/
BERR_Code BHDM_HDCP_GetRepeaterKsvFifo(BHDM_Handle hHDMI, 
	uint8_t *DownstreamKsvsBuffer, uint16_t DownstreamKsvsBufferSize 
)
{
	BERR_Code rc = BERR_SUCCESS ;
	uint16_t iNumKsvBytes ;
	uint8_t RxDeviceAttached ;

	/* make sure HDMI Cable is connected to something... */
	BHDM_CHECK_RC(rc, BHDM_RxDeviceAttached(hHDMI, &RxDeviceAttached));
	if (!RxDeviceAttached)
	{
		rc = BHDM_NO_RX_DEVICE ;
		goto done ;
	}

	/* make sure link is authenticated */	
	rc = BHDM_HDCP_IsLinkAuthenticated(hHDMI) ;
	if (rc != BERR_SUCCESS)
	{
		BDBG_WRN(("Repeater KSV FIFO available only after authentication")) ;
		return rc ;
	}
	
	/* copy the Downstream KSV list */
	if (hHDMI->HDCP_RepeaterKsvList)
	{
		iNumKsvBytes = 
			(uint16_t) hHDMI->HDCP_RepeaterDeviceCount * BHDM_HDCP_KSV_LENGTH ;
			
		if (DownstreamKsvsBufferSize < iNumKsvBytes)
		{
			BDBG_ERR(("Allocated buffer of %d bytes too small for KSV list of %d bytes", 
				DownstreamKsvsBufferSize, iNumKsvBytes)) ;
			return BERR_INVALID_PARAMETER ;
		}
		
		BKNI_Memcpy(DownstreamKsvsBuffer, hHDMI->HDCP_RepeaterKsvList, iNumKsvBytes) ;
	}
	
done:
	return rc ;	
}


#if !defined(BHDM_CONFIG_DISABLE_HDCP_AUTH_REPEATER_DEVCOUNT0)
/******************************************************************************
BERR_Code BHDM_HDCP_ForceVCalculation
Summary: Force the V Calculation for attached repeaters with device count zero 
*******************************************************************************/
BERR_Code BHDM_HDCP_ForceVCalculation(BHDM_Handle hHDMI)
{
	uint32_t Register ;
	BREG_Handle hRegister = hHDMI->hRegister ;
	
	Register = BREG_Read32(hRegister, BCHP_HDMI_HDCP_CTL) ;
	Register |= BCHP_MASK(HDMI_HDCP_CTL, I_FORCE_VCALC) ;
	BREG_Write32(hRegister, BCHP_HDMI_HDCP_CTL, Register) ;
	
	Register &= ~BCHP_MASK(HDMI_HDCP_CTL, I_FORCE_VCALC) ;
	BREG_Write32(hRegister, BCHP_HDMI_HDCP_CTL, Register) ;
	
	return BERR_SUCCESS ;
}
#endif


/******************************************************************************
BERR_Code BHDM_HDCP_GetOptions
Summary: Get current options used for HDCP Authtentication etc.
*******************************************************************************/
BERR_Code BHDM_HDCP_GetOptions(
	BHDM_Handle hHDMI, BHDM_HDCP_OPTIONS *HdcpOptions)
{
	BKNI_Memcpy(HdcpOptions, &hHDMI->HdcpOptions, sizeof(BHDM_HDCP_OPTIONS)) ;
	return BERR_SUCCESS ;
}


/******************************************************************************
BERR_Code BHDM_HDCP_SetOptions
Summary: Set options used for HDCP Authtentication etc.
*******************************************************************************/
BERR_Code BHDM_HDCP_SetOptions(
	BHDM_Handle hHDMI, BHDM_HDCP_OPTIONS *HdcpOptions)
{
	BKNI_Memcpy(&hHDMI->HdcpOptions, HdcpOptions, sizeof(BHDM_HDCP_OPTIONS)) ;
	return BERR_SUCCESS ;
}



BERR_Code BHDM_HDCP_P_DEBUG_PjForceVideo(BHDM_Handle hHDMI, uint8_t value)
{
#if BDBG_DEBUG_BUILD
	uint32_t Register ;
	
	Register = BREG_Read32(hHDMI->hRegister,  BCHP_HDMI_CP_TST) ;
	Register &= 
		~(BCHP_MASK(HDMI_CP_TST, I_TST_FORCE_VIDEO_ALL_ONES) 
		| BCHP_MASK(HDMI_CP_TST, I_TST_FORCE_VIDEO_ALL_ZEROS)) ;

	if (value)
		Register |= BCHP_FIELD_DATA(HDMI_CP_TST,  I_TST_FORCE_VIDEO_ALL_ONES, 1) ;
	else 
		Register |= BCHP_FIELD_DATA(HDMI_CP_TST,  I_TST_FORCE_VIDEO_ALL_ZEROS, 1) ;
		
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_CP_TST, Register) ;
#else
    BSTD_UNUSED(hHDMI);
    BSTD_UNUSED(value);
#endif
	return BERR_SUCCESS ;
}


BERR_Code BHDM_HDCP_P_DEBUG_PjCleanVideo(BHDM_Handle hHDMI, uint8_t value)
{
#if BDBG_DEBUG_BUILD
	uint32_t Register ;

	BSTD_UNUSED(value) ;
	
	Register = BREG_Read32(hHDMI->hRegister,  BCHP_HDMI_CP_TST) ;
	Register &= 
		~(BCHP_MASK(HDMI_CP_TST, I_TST_FORCE_VIDEO_ALL_ONES) 
		| BCHP_MASK(HDMI_CP_TST, I_TST_FORCE_VIDEO_ALL_ZEROS)) ;

	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_CP_TST, Register) ;
#else
    BSTD_UNUSED(hHDMI);
    BSTD_UNUSED(value);
#endif
	return BERR_SUCCESS ;
}


#if BHDM_CONFIG_HDCP_AUTO_RI_PJ_CHECKING_SUPPORT
/******************************************************************************
BERR_Code BHDM_HDCP_P_ConfigureAutoRi
Summary: Configure HDMI HDCP RI register for Auto (hardware) Ri Checking feature.
*******************************************************************************/
BERR_Code BHDM_HDCP_P_ConfigureAutoRi(BHDM_Handle hHDMI)
{
	uint32_t Register;
	
	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_CP_INTEGRITY_CHK_CFG_3) ;
	Register &= ~(BCHP_MASK(HDMI_CP_INTEGRITY_CHK_CFG_3, RI_IIC_REG_0_WR_EN) 
			| BCHP_MASK(HDMI_CP_INTEGRITY_CHK_CFG_3, RI_IIC_REG_0_WR_ADDR_OFFSET)
			| BCHP_MASK(HDMI_CP_INTEGRITY_CHK_CFG_3, RI_IIC_REG_0_WR_DATA)
			| BCHP_MASK(HDMI_CP_INTEGRITY_CHK_CFG_3, RI_IIC_REG_1_WR_EN)
			| BCHP_MASK(HDMI_CP_INTEGRITY_CHK_CFG_3, RI_IIC_REG_1_WR_ADDR_OFFSET)
			| BCHP_MASK(HDMI_CP_INTEGRITY_CHK_CFG_3, RI_IIC_REG_1_WR_DATA));

#if BHDM_CONFIG_HDCP_ADVANCED_HW_AUTO_RI_PJ_SUPPORT 
	Register |= BCHP_FIELD_DATA(HDMI_CP_INTEGRITY_CHK_CFG_3, RI_IIC_REG_0_WR_EN, 0x1)
			| BCHP_FIELD_DATA(HDMI_CP_INTEGRITY_CHK_CFG_3, RI_IIC_REG_0_WR_ADDR_OFFSET, 0xb)
			| BCHP_FIELD_DATA(HDMI_CP_INTEGRITY_CHK_CFG_3, RI_IIC_REG_0_WR_DATA, 0x0)
			| BCHP_FIELD_DATA(HDMI_CP_INTEGRITY_CHK_CFG_3, RI_IIC_REG_1_WR_EN, 0x1)
			| BCHP_FIELD_DATA(HDMI_CP_INTEGRITY_CHK_CFG_3, RI_IIC_REG_1_WR_ADDR_OFFSET, 0xa)
			| BCHP_FIELD_DATA(HDMI_CP_INTEGRITY_CHK_CFG_3, RI_IIC_REG_1_WR_DATA, 0xd3);
#else
	Register |= BCHP_FIELD_DATA(HDMI_CP_INTEGRITY_CHK_CFG_3, RI_IIC_REG_0_WR_EN, 0x1)
			| BCHP_FIELD_DATA(HDMI_CP_INTEGRITY_CHK_CFG_3, RI_IIC_REG_0_WR_ADDR_OFFSET, 0xb)
			| BCHP_FIELD_DATA(HDMI_CP_INTEGRITY_CHK_CFG_3, RI_IIC_REG_0_WR_DATA, 0x0)
			| BCHP_FIELD_DATA(HDMI_CP_INTEGRITY_CHK_CFG_3, RI_IIC_REG_1_WR_EN, 0x1)
			| BCHP_FIELD_DATA(HDMI_CP_INTEGRITY_CHK_CFG_3, RI_IIC_REG_1_WR_ADDR_OFFSET, 0x1)
			| BCHP_FIELD_DATA(HDMI_CP_INTEGRITY_CHK_CFG_3, RI_IIC_REG_1_WR_DATA, 0x8);
#endif
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_CP_INTEGRITY_CHK_CFG_3, Register) ;


	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_CP_INTEGRITY_CHK_CFG_4) ;
#if BHDM_CONFIG_HDCP_ADVANCED_HW_AUTO_RI_PJ_SUPPORT
	Register &= ~(BCHP_MASK(HDMI_CP_INTEGRITY_CHK_CFG_4, RI_IIC_REG_2_WR_EN) 
			| BCHP_MASK(HDMI_CP_INTEGRITY_CHK_CFG_4, RI_IIC_REG_2_WR_ADDR_OFFSET)
			| BCHP_MASK(HDMI_CP_INTEGRITY_CHK_CFG_4, RI_IIC_REG_2_WR_DATA)
			| BCHP_MASK(HDMI_CP_INTEGRITY_CHK_CFG_4, RI_IIC_REG_3_WR_EN)
			| BCHP_MASK(HDMI_CP_INTEGRITY_CHK_CFG_4, RI_IIC_REG_3_WR_ADDR_OFFSET)
                        | BCHP_MASK(HDMI_CP_INTEGRITY_CHK_CFG_4, RI_IIC_REG_3_WR_DATA));

	Register |= BCHP_FIELD_DATA(HDMI_CP_INTEGRITY_CHK_CFG_4, RI_IIC_REG_2_WR_EN, 0x1)
			| BCHP_FIELD_DATA(HDMI_CP_INTEGRITY_CHK_CFG_4, RI_IIC_REG_2_WR_ADDR_OFFSET, 0x9)
			| BCHP_FIELD_DATA(HDMI_CP_INTEGRITY_CHK_CFG_4, RI_IIC_REG_2_WR_DATA, 0x81)
			| BCHP_FIELD_DATA(HDMI_CP_INTEGRITY_CHK_CFG_4, RI_IIC_REG_3_WR_EN, 0x1)
			| BCHP_FIELD_DATA(HDMI_CP_INTEGRITY_CHK_CFG_4, RI_IIC_REG_3_WR_ADDR_OFFSET, 0x1)
			| BCHP_FIELD_DATA(HDMI_CP_INTEGRITY_CHK_CFG_4, RI_IIC_REG_3_WR_DATA, 0x8);
#else
	Register &= ~(BCHP_MASK(HDMI_CP_INTEGRITY_CHK_CFG_4, RI_IIC_REG_2_WR_EN) 
			| BCHP_MASK(HDMI_CP_INTEGRITY_CHK_CFG_4, RI_IIC_REG_2_WR_ADDR_OFFSET)
			| BCHP_MASK(HDMI_CP_INTEGRITY_CHK_CFG_4, RI_IIC_REG_2_WR_DATA)
			| BCHP_MASK(HDMI_CP_INTEGRITY_CHK_CFG_4, RI_IIC_REG_3_WR_EN));

	Register |= BCHP_FIELD_DATA(HDMI_CP_INTEGRITY_CHK_CFG_4, RI_IIC_REG_2_WR_EN, 0x1)
			| BCHP_FIELD_DATA(HDMI_CP_INTEGRITY_CHK_CFG_4, RI_IIC_REG_2_WR_ADDR_OFFSET, 0xb)
			| BCHP_FIELD_DATA(HDMI_CP_INTEGRITY_CHK_CFG_4, RI_IIC_REG_2_WR_DATA, 0x1)
			| BCHP_FIELD_DATA(HDMI_CP_INTEGRITY_CHK_CFG_4, RI_IIC_REG_3_WR_EN, 0x0);
#endif
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_CP_INTEGRITY_CHK_CFG_4, Register) ;

#if BHDM_CONFIG_HDCP_ADVANCED_HW_AUTO_RI_PJ_SUPPORT
	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_CP_INTEGRITY_CHK_CFG_8) ;
	Register &= ~(BCHP_MASK(HDMI_CP_INTEGRITY_CHK_CFG_8, RI_IIC_REG_4_WR_EN) 
			| BCHP_MASK(HDMI_CP_INTEGRITY_CHK_CFG_8, RI_IIC_REG_4_WR_ADDR_OFFSET)
			| BCHP_MASK(HDMI_CP_INTEGRITY_CHK_CFG_8, RI_IIC_REG_4_WR_DATA)
			| BCHP_MASK(HDMI_CP_INTEGRITY_CHK_CFG_8, RI_IIC_REG_5_WR_EN)
			| BCHP_MASK(HDMI_CP_INTEGRITY_CHK_CFG_8, RI_IIC_REG_5_WR_ADDR_OFFSET)
			| BCHP_MASK(HDMI_CP_INTEGRITY_CHK_CFG_8, RI_IIC_REG_5_WR_DATA));

	Register |= BCHP_FIELD_DATA(HDMI_CP_INTEGRITY_CHK_CFG_8, RI_IIC_REG_4_WR_EN, 0x1)
			| BCHP_FIELD_DATA(HDMI_CP_INTEGRITY_CHK_CFG_8, RI_IIC_REG_4_WR_ADDR_OFFSET, 0xb)
                        | BCHP_FIELD_DATA(HDMI_CP_INTEGRITY_CHK_CFG_8, RI_IIC_REG_4_WR_DATA, 0x1);
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_CP_INTEGRITY_CHK_CFG_8, Register) ;
#endif

	return BERR_SUCCESS;
}


/******************************************************************************
BERR_Code BHDM_HDCP_P_ConfigureAutoPj
Summary: Configure HDMI HDCP PJ register for Auto (hardware) Pj Checking feature.
*******************************************************************************/
BERR_Code BHDM_HDCP_P_ConfigureAutoPj(BHDM_Handle hHDMI)
{
	uint32_t Register;
	
	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_CP_INTEGRITY_CHK_CFG_5) ;
	Register &= ~(BCHP_MASK(HDMI_CP_INTEGRITY_CHK_CFG_5, PJ_IIC_REG_0_WR_EN) 
			| BCHP_MASK(HDMI_CP_INTEGRITY_CHK_CFG_5, PJ_IIC_REG_0_WR_ADDR_OFFSET)
			| BCHP_MASK(HDMI_CP_INTEGRITY_CHK_CFG_5, PJ_IIC_REG_0_WR_DATA)
			| BCHP_MASK(HDMI_CP_INTEGRITY_CHK_CFG_5, PJ_IIC_REG_1_WR_EN)
			| BCHP_MASK(HDMI_CP_INTEGRITY_CHK_CFG_5, PJ_IIC_REG_1_WR_ADDR_OFFSET)
			| BCHP_MASK(HDMI_CP_INTEGRITY_CHK_CFG_5, PJ_IIC_REG_1_WR_DATA));

#if BHDM_CONFIG_HDCP_ADVANCED_HW_AUTO_RI_PJ_SUPPORT
	Register |= BCHP_FIELD_DATA(HDMI_CP_INTEGRITY_CHK_CFG_5, PJ_IIC_REG_0_WR_EN, 0x1)
			| BCHP_FIELD_DATA(HDMI_CP_INTEGRITY_CHK_CFG_5, PJ_IIC_REG_0_WR_ADDR_OFFSET, 0xb)
			| BCHP_FIELD_DATA(HDMI_CP_INTEGRITY_CHK_CFG_5, PJ_IIC_REG_0_WR_DATA, 0x0)
			| BCHP_FIELD_DATA(HDMI_CP_INTEGRITY_CHK_CFG_5, PJ_IIC_REG_1_WR_EN, 0x1)
			| BCHP_FIELD_DATA(HDMI_CP_INTEGRITY_CHK_CFG_5, PJ_IIC_REG_1_WR_ADDR_OFFSET, 0xa)
			| BCHP_FIELD_DATA(HDMI_CP_INTEGRITY_CHK_CFG_5, PJ_IIC_REG_1_WR_DATA, 0xd3);
#else
	Register |= BCHP_FIELD_DATA(HDMI_CP_INTEGRITY_CHK_CFG_5, PJ_IIC_REG_0_WR_EN, 0x1)
			| BCHP_FIELD_DATA(HDMI_CP_INTEGRITY_CHK_CFG_5, PJ_IIC_REG_0_WR_ADDR_OFFSET, 0xb)
			| BCHP_FIELD_DATA(HDMI_CP_INTEGRITY_CHK_CFG_5, PJ_IIC_REG_0_WR_DATA, 0x0)
			| BCHP_FIELD_DATA(HDMI_CP_INTEGRITY_CHK_CFG_5, PJ_IIC_REG_1_WR_EN, 0x1)
			| BCHP_FIELD_DATA(HDMI_CP_INTEGRITY_CHK_CFG_5, PJ_IIC_REG_1_WR_ADDR_OFFSET, 0x1)
			| BCHP_FIELD_DATA(HDMI_CP_INTEGRITY_CHK_CFG_5, PJ_IIC_REG_1_WR_DATA, 0xa);
#endif
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_CP_INTEGRITY_CHK_CFG_5, Register) ;


	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_CP_INTEGRITY_CHK_CFG_6) ;
#if BHDM_CONFIG_HDCP_ADVANCED_HW_AUTO_RI_PJ_SUPPORT
	Register &= ~(BCHP_MASK(HDMI_CP_INTEGRITY_CHK_CFG_6, PJ_IIC_REG_2_WR_EN) 
			| BCHP_MASK(HDMI_CP_INTEGRITY_CHK_CFG_6, PJ_IIC_REG_2_WR_ADDR_OFFSET)
			| BCHP_MASK(HDMI_CP_INTEGRITY_CHK_CFG_6, PJ_IIC_REG_2_WR_DATA)
			| BCHP_MASK(HDMI_CP_INTEGRITY_CHK_CFG_6, PJ_IIC_REG_3_WR_EN)
			| BCHP_MASK(HDMI_CP_INTEGRITY_CHK_CFG_6, PJ_IIC_REG_3_WR_ADDR_OFFSET)
                        | BCHP_MASK(HDMI_CP_INTEGRITY_CHK_CFG_6, PJ_IIC_REG_3_WR_DATA));

	Register |= BCHP_FIELD_DATA(HDMI_CP_INTEGRITY_CHK_CFG_6, PJ_IIC_REG_2_WR_EN, 0x1)
			| BCHP_FIELD_DATA(HDMI_CP_INTEGRITY_CHK_CFG_6, PJ_IIC_REG_2_WR_ADDR_OFFSET, 0x9)
			| BCHP_FIELD_DATA(HDMI_CP_INTEGRITY_CHK_CFG_6, PJ_IIC_REG_2_WR_DATA, 0x81)
			| BCHP_FIELD_DATA(HDMI_CP_INTEGRITY_CHK_CFG_6, PJ_IIC_REG_3_WR_EN, 0x1)
			| BCHP_FIELD_DATA(HDMI_CP_INTEGRITY_CHK_CFG_6, PJ_IIC_REG_3_WR_ADDR_OFFSET, 0x1)
                        | BCHP_FIELD_DATA(HDMI_CP_INTEGRITY_CHK_CFG_6, PJ_IIC_REG_3_WR_DATA, 0xa);
#else
	Register &= ~(BCHP_MASK(HDMI_CP_INTEGRITY_CHK_CFG_6, PJ_IIC_REG_2_WR_EN) 
			| BCHP_MASK(HDMI_CP_INTEGRITY_CHK_CFG_6, PJ_IIC_REG_2_WR_ADDR_OFFSET)
			| BCHP_MASK(HDMI_CP_INTEGRITY_CHK_CFG_6, PJ_IIC_REG_2_WR_DATA)
			| BCHP_MASK(HDMI_CP_INTEGRITY_CHK_CFG_6, PJ_IIC_REG_3_WR_EN));

	Register |= BCHP_FIELD_DATA(HDMI_CP_INTEGRITY_CHK_CFG_6, PJ_IIC_REG_2_WR_EN, 0x1)
			| BCHP_FIELD_DATA(HDMI_CP_INTEGRITY_CHK_CFG_6, PJ_IIC_REG_2_WR_ADDR_OFFSET, 0xb)
			| BCHP_FIELD_DATA(HDMI_CP_INTEGRITY_CHK_CFG_6, PJ_IIC_REG_2_WR_DATA, 0x1)
			| BCHP_FIELD_DATA(HDMI_CP_INTEGRITY_CHK_CFG_6, PJ_IIC_REG_3_WR_EN, 0x0);
#endif
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_CP_INTEGRITY_CHK_CFG_6, Register) ;


#if BHDM_CONFIG_HDCP_ADVANCED_HW_AUTO_RI_PJ_SUPPORT
	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_CP_INTEGRITY_CHK_CFG_7) ;
	Register &= ~(BCHP_MASK(HDMI_CP_INTEGRITY_CHK_CFG_7, PJ_IIC_REG_4_WR_EN) 
			| BCHP_MASK(HDMI_CP_INTEGRITY_CHK_CFG_7, PJ_IIC_REG_4_WR_ADDR_OFFSET)
			| BCHP_MASK(HDMI_CP_INTEGRITY_CHK_CFG_7, PJ_IIC_REG_4_WR_DATA)
			| BCHP_MASK(HDMI_CP_INTEGRITY_CHK_CFG_7, PJ_IIC_REG_5_WR_EN)
			| BCHP_MASK(HDMI_CP_INTEGRITY_CHK_CFG_7, PJ_IIC_REG_5_WR_ADDR_OFFSET)
			| BCHP_MASK(HDMI_CP_INTEGRITY_CHK_CFG_7, PJ_IIC_REG_5_WR_DATA));

	Register |= BCHP_FIELD_DATA(HDMI_CP_INTEGRITY_CHK_CFG_7, PJ_IIC_REG_4_WR_EN, 0x1)
			| BCHP_FIELD_DATA(HDMI_CP_INTEGRITY_CHK_CFG_7, PJ_IIC_REG_4_WR_ADDR_OFFSET, 0xb)
                        | BCHP_FIELD_DATA(HDMI_CP_INTEGRITY_CHK_CFG_7, PJ_IIC_REG_4_WR_DATA, 0x1);
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_CP_INTEGRITY_CHK_CFG_7, Register) ;
#endif

	return BERR_SUCCESS;
}


/******************************************************************************
BERR_Code BHDM_HDCP_P_EnableAutoRiPjChecking
Summary: Enable Auto (hardware) Ri/Pj Checking feature
*******************************************************************************/
BERR_Code BHDM_HDCP_P_EnableAutoRiPjChecking (BHDM_Handle hHDMI, uint8_t uiPjChecking)
{
	BERR_Code rc = BERR_SUCCESS;
	uint32_t Register;
	uint8_t uiMode;

#if !(BHDM_CONFIG_HDCP_ADVANCED_HW_AUTO_RI_PJ_SUPPORT)
	uint32_t dataTransferFormat=3, cnt1=1, cnt2=2;
#endif

	/* Set up I2C HW first */
	BHDM_CHECK_RC(rc, BREG_I2C_Read(hHDMI->hI2cRegHandle, 
		BHDM_HDCP_RX_I2C_ADDR, BHDM_HDCP_RX_BSTATUS, (uint8_t *) &hHDMI->RxStatus, 2)) ;	
	BREG_LE16(hHDMI->RxStatus) ;
	
	BHDM_CHECK_RC(rc, BREG_I2C_Read(hHDMI->hI2cRegHandle, 
		BHDM_HDCP_RX_I2C_ADDR, BHDM_HDCP_RX_BCAPS, &hHDMI->RxBCaps, 1)) ;
	
#if !(BHDM_CONFIG_HDCP_ADVANCED_HW_AUTO_RI_PJ_SUPPORT)
	rc = BREG_I2C_SetupHdmiHwAccess(hHDMI->hI2cRegHandle, dataTransferFormat, cnt1, cnt2);
	if (rc != BERR_SUCCESS)
	{
		BDBG_ERR(("Error setting up I2C HW for auto Ri/Pj link integrity check"));
		goto done;
	}
#endif

	if (uiPjChecking)
		uiMode = 2;
	else
		uiMode = 1;
	
	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_CP_INTEGRITY_CHK_CFG_2) ;
	Register &= ~(BCHP_MASK(HDMI_CP_INTEGRITY_CHK_CFG_2, IIC_REG_RD_ADDR_OFFSET));
	Register |= BCHP_FIELD_DATA(HDMI_CP_INTEGRITY_CHK_CFG_2, IIC_REG_RD_ADDR_OFFSET, 0xc);
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_CP_INTEGRITY_CHK_CFG_2, Register) ;
	
	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_CP_INTEGRITY_CHK_CFG_1) ;
	Register |= BCHP_FIELD_DATA(HDMI_CP_INTEGRITY_CHK_CFG_1, CLEAR_RI_MISMATCH, 1);
	Register |= BCHP_FIELD_DATA(HDMI_CP_INTEGRITY_CHK_CFG_1, CLEAR_PJ_MISMATCH, 1);
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_CP_INTEGRITY_CHK_CFG_1, Register) ;
	Register &= ~(BCHP_MASK(HDMI_CP_INTEGRITY_CHK_CFG_1, CLEAR_RI_MISMATCH));
	Register &= ~(BCHP_MASK(HDMI_CP_INTEGRITY_CHK_CFG_1, CLEAR_PJ_MISMATCH));
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_CP_INTEGRITY_CHK_CFG_1, Register) ;

	hHDMI->HDCP_AutoRiMismatch_A = false;
	hHDMI->HDCP_AutoRiMismatch_B = false;

	Register &= ~(BCHP_MASK(HDMI_CP_INTEGRITY_CHK_CFG_1, CHECK_MODE));
	Register |= BCHP_FIELD_DATA(HDMI_CP_INTEGRITY_CHK_CFG_1, CHECK_MODE, uiMode);
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_CP_INTEGRITY_CHK_CFG_1, Register) ;

	hHDMI->bAutoRiPjCheckingEnabled = true;

done:
	return rc;
}


/******************************************************************************
BERR_Code BHDM_HDCP_P_DisableAutoRiPjChecking
Summary: Disable Auto (hardware) Ri/Pj Checking feature
*******************************************************************************/
BERR_Code BHDM_HDCP_P_DisableAutoRiPjChecking (BHDM_Handle hHDMI)
{
	uint32_t Register;
	uint8_t uiMode=0;

	Register = BREG_Read32(hHDMI->hRegister, BCHP_HDMI_CP_INTEGRITY_CHK_CFG_1) ;
	Register &= ~(BCHP_MASK(HDMI_CP_INTEGRITY_CHK_CFG_1, CHECK_MODE));
	Register |= BCHP_FIELD_DATA(HDMI_CP_INTEGRITY_CHK_CFG_1, CHECK_MODE, uiMode);
	BREG_Write32(hHDMI->hRegister, BCHP_HDMI_CP_INTEGRITY_CHK_CFG_1, Register) ;

	hHDMI->bAutoRiPjCheckingEnabled = false;
	return BERR_SUCCESS;
}

#endif




