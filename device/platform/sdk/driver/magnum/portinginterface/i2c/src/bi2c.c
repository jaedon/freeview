/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bi2c.c $
 * $brcm_Revision: Hydra_Software_Devel/211 $
 * $brcm_Date: 10/16/12 7:51a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/i2c/src/bi2c.c $
 * 
 * Hydra_Software_Devel/211   10/16/12 7:51a agin
 * SW7346-1065:  Update comments.
 * 
 * Hydra_Software_Devel/210   10/16/12 7:42a agin
 * SW7346-1065:  Missing "break" error in Rel_7346_FGDL_20120416.zip?
 * 
 * Hydra_Software_Devel/209   9/25/12 10:29a agin
 * SWNOOS-564:  Removed BI2C_MSB_FIX.  Removed
 * BI2C_SERIALIZE_SINGLE_SHARED_MASTER.  Replace reference to
 * BI2C_MAX_I2C_MSTR_CHANNELS with BI2C_MAX_I2C_CHANNELS due to
 * redundancy.
 * 
 * Hydra_Software_Devel/208   8/17/12 9:34a agin
 * SWNOOS-564:  Support i2c on 7445.
 * 
 * Hydra_Software_Devel/207   8/9/12 5:10p agin
 * SW7563-2:  Fixed i2c compile error.
 * 
 * Hydra_Software_Devel/206   8/3/12 7:56p agin
 * SWNOOS-527:  Mutex protect WriteRead.
 * 
 * Hydra_Software_Devel/205   8/3/12 2:27p erickson
 * SW7231-929: do not require unused handle in
 * BI2C_GetChannelDefaultSettings
 * 
 * Hydra_Software_Devel/204   7/23/12 4:35p randyjew
 * SW7563-2: Add 7563  support
 * 
 * Hydra_Software_Devel/203   7/3/12 6:44p agin
 * SW7425-2956:  Add BKNI_ResetEvent.
 * 
 * Hydra_Software_Devel/202   6/8/12 10:40p agin
 * SW7425-2956:  Changed error message to warning.
 * 
 * Hydra_Software_Devel/201   6/6/12 2:26p agin
 * SW7425-2956:  Merge 4 byte transfer fix and reset i2c bus fixes back to
 * mainline.
 * 
 * Hydra_Software_Devel/200   5/10/12 11:21a mward
 * SW7346-741:  Remove extra debug.  BDBG_ERR in BI2C_P_WriteCmd() back to
 * BDBG_MSG().
 * 
 * Hydra_Software_Devel/198   5/2/12 8:11p agin
 * SW7584-23:  Support 7584.
 * 
 * Hydra_Software_Devel/197   4/17/12 12:38p cnovak
 * SWNOOS-527: In BI2C_P_WriteReadNoAddr, set numSubAddrBytes to 0
 * 
 * Hydra_Software_Devel/196   4/12/12 7:34p agin
 * SWNOOS-527:  Fix subaddress length of 2 and 3 problem.  Add no sub-
 * address option for WriteRead.
 * 
 * Hydra_Software_Devel/195   4/3/12 10:09a agin
 * SWNOOS-527:  Fixed compile problem with reference software.
 * 
 * Hydra_Software_Devel/194   4/2/12 3:05p agin
 * SW7346-741:  Implement magnum changes to support NEXUS_I2c_WriteRead
 * 
 * Hydra_Software_Devel/SWNOOS-527/2   4/2/12 12:00a agin
 * SWNOOS-527:  Fixed WriteRead.
 * 
 * Hydra_Software_Devel/SWNOOS-527/1   4/1/12 8:26a agin
 * SWNOOS-527:  Support I2C WriteRead.
 * 
 * Hydra_Software_Devel/193   2/1/12 12:16a agin
 * SW7346-657:  Subaddress not handled properly for
 * BI2C_P_WriteBy4BytesCmd.
 * 
 * Hydra_Software_Devel/192   1/14/12 5:08p agin
 * SWNOOS-507:  Fixed zero byte write problem with
 * BI2C_P_WriteBy4BytesCmd.
 * 
 * Hydra_Software_Devel/191   1/12/12 4:35p agin
 * SWNOOS-507:  Back off last change.
 * 
 * Hydra_Software_Devel/190   1/12/12 2:51p agin
 * SWNOOS-507:  Make 4 byte xfer the default for chips which can support
 * it.
 * 
 * Hydra_Software_Devel/189   1/9/12 10:43a agin
 * SWNOOS-507:  Eliminate redundant functions.
 * 
 * Hydra_Software_Devel/188   1/7/12 1:42p agin
 * SWNOOS-507:  Don't require a buffer for BI2C_P_WriteBy4BytesCmd.
 * 
 * Hydra_Software_Devel/187   1/5/12 11:50a agin
 * SWNOOS-507:  Allocate memory per channel for 4 byte transfers.
 * 
 * Hydra_Software_Devel/186   1/3/12 1:15p agin
 * SWNOOS-507:  Improve i2c performance.
 * 
 * Hydra_Software_Devel/185   12/16/11 7:37p bselva
 * SW7360-6: Added appframework support for 7360 platform
 * 
 * Hydra_Software_Devel/184   12/6/11 4:24p mward
 * SW7429-7:  Add 7429 to SUPPORT_I2C_VIA_GPIO list, define the GPIO
 * parameters for it.
 * 
 * Hydra_Software_Devel/183   11/21/11 6:18p mward
 * SW7435-7: Add 7435.
 * 
 * Hydra_Software_Devel/182   11/10/11 11:09a agin
 * SWNOOS-496:  Define additional macros for AON GPIO.
 * 
 * Hydra_Software_Devel/181   11/6/11 9:07a agin
 * SWNOOS-496:  Correct i2c BSCx to BSC_Mx mapping.
 * 
 * Hydra_Software_Devel/180   10/7/11 6:49a agin
 * SW7429-7:  Add support for i2c.
 * 
 * Hydra_Software_Devel/179   9/30/11 9:47a agin
 * SWNOOS-485:  Support 7346 B0.
 * 
 * Hydra_Software_Devel/178   9/21/11 11:17a agin
 * SWNOOS-487:  Fixed problem with previous revision.
 * 
 * Hydra_Software_Devel/177   9/21/11 8:20a agin
 * SWNOOS-487:  Updated for 7344 B0.
 * 
 * Hydra_Software_Devel/176   8/29/11 1:51p jrubio
 * SW7346-470: remove 7346A0 wrap over code
 * 
 * Hydra_Software_Devel/175   8/29/11 1:48p jrubio
 * SW7346-470: add 7346 B0
 * 
 * Hydra_Software_Devel/174   8/16/11 3:10p jessem
 * SW7425-1097: Added B0 support.
 *
 * Hydra_Software_Devel/173   8/9/11 7:44p agin
 * SW7231-253:  Support 7231B0.
 *
 * Hydra_Software_Devel/172   8/8/11 1:51p mward
 * SW7125-1061:  Null pointer to indicate freed.
 *
 * Hydra_Software_Devel/171   8/8/11 12:10p mward
 * SW7125-1058: Fix FORWARD_NULL
 *
 * Hydra_Software_Devel/170   6/22/11 4:05p agin
 * SWNOOS-464:  Initialize DATA_REG_SIZE in the BSC channels.
 *
 * Hydra_Software_Devel/169   4/28/11 4:01p vanessah
 * SW7425-340: add 7425B0 appframework support
 *
 * Hydra_Software_Devel/168   4/14/11 3:17p agin
 * SW7344-44:  Add mutex protection for i2c accesses, handle EDDC
 * different.
 *
 * Hydra_Software_Devel/167   4/13/11 5:12p agin
 * SW7344-44:  Add mutex protection for i2c accesses on a channel basis.
 *
 * Hydra_Software_Devel/166   3/21/11 4:08p agin
 * SW7346-96:  Add BI2C_P_WriteSwA24,  BI2C_P_ReadSwA24,
 * BI2C_P_WriteSwA16.
 *
 * Hydra_Software_Devel/165   3/16/11 1:46p agin
 * SW7346-96:  Allow sub address bytes of 2 and 3 for BI2C_P_WriteSwCmd.
 *
 * Hydra_Software_Devel/164   3/9/11 7:41p etrudeau
 * SWBLURAY-23692: fix warning about done label
 *
 * Hydra_Software_Devel/163   3/9/11 5:31p etrudeau
 * SWBLURAY-23692: 7640 has 4 i2c channels with BSC Master C not connected
 * and BSC Master D is on AON
 *
 * Hydra_Software_Devel/162   3/2/11 5:50p agin
 * SW7346-96:  For extra gpio i2c channel, return early from
 * BI2C_CloseChannel.
 *
 * Hydra_Software_Devel/161   3/2/11 4:55p agin
 * SW7346-96:  For extra gpio i2c channel, return early from BI2C_SetClk
 * and BI2C_SetSdaDelay.
 *
 * Hydra_Software_Devel/160   2/24/11 6:51a agin
 * SW7346-96:  Fix compiler error.
 *
 * Hydra_Software_Devel/159   2/23/11 7:33p agin
 * SW7346-96:  Fix compiler error.
 *
 * Hydra_Software_Devel/158   2/21/11 3:45p hongtaoz
 * SW7346-96: fixed compile error of previous checkin; add 7425 support
 * same as 7422;
 *
 * Hydra_Software_Devel/157   2/21/11 7:35a agin
 * SW7346-96:  Add ability to use GPIO lines not tied to the BSC I2C for
 * I2C bit bang.
 *
 * Hydra_Software_Devel/156   11/17/10 7:28p agin
 * SW7231-19:  Add I2C PI support for 7231/7344/7346
 *
 * Hydra_Software_Devel/155   11/1/10 5:08p xhuang
 * SW7552-4: Add 7552 support
 *
 * Hydra_Software_Devel/154   8/27/10 5:17p tdo
 * SW7425-12: Add 7425 I2C support
 *
 * Hydra_Software_Devel/153   8/19/10 6:43a agin
 * SW7358-7:  Fix compile error for 7358.
 *
 * Hydra_Software_Devel/SW7358-7/1   8/19/10 6:09p xhuang
 * SW7358-7: fix compile error for 7358
 *
 * Hydra_Software_Devel/152   6/30/10 5:18p vanessah
 * SW7422-12:  include bchp_irq0_aon.h
 *
 * Hydra_Software_Devel/151   6/30/10 2:21p agin
 * SW7422-12:  Defined BCHP_INT_ID_iicd_irqen and BCHP_INT_ID_iice_irqen
 * locally for 7422.
 *
 * Hydra_Software_Devel/150   6/25/10 12:40p agin
 * SW7422-12:  Include bchp_aon_pin_ctrl.h for 7422.  Add support for soft
 * i2c for 7422.
 *
 * Hydra_Software_Devel/149   6/22/10 11:40a vanessah
 * SW7422-12:  To support appframework. Missing files added:
 * magnum\portinginterface\pwr rockford\appframework\src\board\97422  To
 * do list: 1. in framework_board.c, more initialization to be done.  2.
 * More registers mapping, like clock generation as well as
 * BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL etc
 *
 * Hydra_Software_Devel/148   3/24/10 11:47a vle
 * SW7601-171: Add BI2C_P_SetupHdmiHwAccess to set up I2C for HDMI HDCP
 * auto Ri/Pj link integrity check feature
 *
 * Hydra_Software_Devel/SW7601-171/1   3/9/10 3:22p vle
 * SW7601-171: Add BI2C_P_SetupHdmiHwAccess to set up I2C for HDMI HDCP
 * auto Ri/Pj link integrity check feature
 *
 * Hydra_Software_Devel/147   3/8/10 3:08p agin
 * SW7342-176:  I2c caused Oops in kernel mode.
 *
 * Hydra_Software_Devel/146   11/19/09 10:59a gmohile
 * SW7408-1 : Fix i2c build errors for 7408
 *
 * Hydra_Software_Devel/145   11/16/09 10:55a agin
 * SW7550-41:  Fix BCHP_INT_ID for general case.
 *
 * Hydra_Software_Devel/144   11/16/09 6:55a agin
 * SW7550-41:  UPG PIs are not compiling.
 *
 * Hydra_Software_Devel/143   11/11/09 2:11p agin
 * SW7400-2592:  False i2c REPEAT-START condition generated due to read-
 * modified-write manner of sw i2c implementation.
 *
 * Hydra_Software_Devel/142   11/9/09 11:27a gmohile
 * SW7408-1 : Add 7408 support
 *
 * Hydra_Software_Devel/141   11/2/09 9:42a maivu
 * SW3548-2587: Fixed warnings
 *
 * Hydra_Software_Devel/140   10/26/09 1:52p agin
 * SW7115-841: Add support for BERR_Code BI2C_P_ReadSwNoAck and
 * BI2C_P_ReadSwNoAddrNoAck.
 *
 * Hydra_Software_Devel/139   10/22/09 2:06p agin
 * SW7405-2686: Change BREG_I2C to abstract SW and HW implementation.
 *
 * Hydra_Software_Devel/138   10/14/09 2:18p mward
 * SW7125-4: Add 7125 to SUPPORT_I2C_VIA_GPIO list, define the GPIO
 * parameters for it.
 *
 * Hydra_Software_Devel/137   9/28/09 11:11a vishk
 * SW7405-3038:  Remove test for warnings.
 *
 * fixed the comment to remove build errors.
 *
 * Hydra_Software_Devel/136   9/25/09 3:56p agin
 * SW7405-3038:  Remove test for warnings.
 *
 * Hydra_Software_Devel/135   9/25/09 1:21p agin
 * SW7405-3038:  Add BI2C_SwReset().
 *
 * Hydra_Software_Devel/134   9/16/09 9:51a pntruong
 * SW7630-45: Fixed previous checked build errors and warnings.
 *
 * Hydra_Software_Devel/132   9/15/09 10:40a rpereira
 * SW7630-45: Adding 7630 support
 *
 * Hydra_Software_Devel/131   9/1/09 8:59p rpereira
 * SW7550-30: Adding 7550 support
 *
 * Hydra_Software_Devel/130   8/4/09 5:21p agin
 * PR57319: I2C bit bang mode doesn't work with 7325 for HDMI.
 *
 * Hydra_Software_Devel/129   7/29/09 12:36p jrubio
 * PR55232: add 7342/7340
 *
 * Hydra_Software_Devel/128   7/23/09 11:03a mward
 * PR 55545: Support 7125.
 *
 * Hydra_Software_Devel/127   7/20/09 10:19a agin
 * PR56687: Change I2C clock enum from Khz to Hz.
 *
 * Hydra_Software_Devel/126   5/2/09 1:13p agin
 * PR54671:  Perform a dummy gpio read following a gpio write for the sw
 * i2c implementation.
 *
 * Hydra_Software_Devel/125   4/27/09 11:16a jhaberf
 * PR53796: Updating i2c build to support BCM35130 DTV chip.
 *
 * Hydra_Software_Devel/124   4/10/09 2:42p katrep
 * PR52971: compiler warning
 *
 * Hydra_Software_Devel/123   4/9/09 6:07p rpereira
 * PR52971: added 7635 support
 *
 * Hydra_Software_Devel/122   4/9/09 8:48a agin
 * PR54035: Do not send stop condition after HW detects no-ack (hw does it
 * already).  Handle BERR_OS_ERROR also.
 *
 * Hydra_Software_Devel/121   3/9/09 5:06p jkim
 * PR50133: Remove deadcode flaged by Coverity
 *
 * Hydra_Software_Devel/120   3/5/09 11:26a agin
 * PR52310: Fix cut & paste error for modifying SDA_DELAY_SEL field.
 *
 * Hydra_Software_Devel/119   2/25/09 2:24p agin
 * PR52112: Fixed warnings.
 *
 * Hydra_Software_Devel/118   2/25/09 11:02a agin
 * PR52112: For clock stretching, add timeout.
 *
 * Hydra_Software_Devel/117   2/20/09 10:58a agin
 * PR52346: Used wrong i2c mask value for setting data transfer type
 *
 * Hydra_Software_Devel/116   2/19/09 3:28p agin
 * PR52310: Add i2c sda delay control for 7601.
 *
 * Hydra_Software_Devel/115   2/18/09 10:55a agin
 * PR52112: Don't send 0x60 and segment pointer if segment==0 for sw bit
 * bang.
 *
 * Hydra_Software_Devel/114   2/14/09 8:08a agin
 * PR52112: Handle no-ack cases by aborting transfer.
 *
 * Hydra_Software_Devel/113   2/5/09 12:04p agin
 * PR51859: I2C default settings defined BI2C_MAX_I2C_CHANNELS
 * incorrectly.
 *
 * Hydra_Software_Devel/112   1/31/09 2:56p pntruong
 * PR51629: Fixed build errors.
 *
 * Hydra_Software_Devel/111   1/31/09 1:29a jrubio
 * PR51629: add 7336 support
 *
 * Hydra_Software_Devel/110   1/12/09 1:01p maivu
 * PR 50693: Fix warning errors
 *
 * Hydra_Software_Devel/110   1/12/09 12:54p maivu
 * PR 50693: Fix warning errors
 *
 * Hydra_Software_Devel/109   12/31/08 9:20a agin
 * PR49585: Need to enable i2c support for 7325, 7335, 7413.
 *
 * Hydra_Software_Devel/108   12/31/08 9:16a agin
 * PR49585: Add sw i2c support for 7325, 7335, 7413.
 *
 * Hydra_Software_Devel/107   12/13/08 3:45p agin
 * PR42305: Support i2c via gpio for read EDDC.
 *
 * Hydra_Software_Devel/106   12/8/08 5:28p agin
 * PR49867: Fixed 7420 gpio i2c.
 *
 * Hydra_Software_Devel/105   12/2/08 4:39p agin
 * PR49585: Fixed warnings.
 *
 * Hydra_Software_Devel/104   12/2/08 4:25p kaushikb
 * PR49867: Adding support for 7420
 *
 * Hydra_Software_Devel/103   11/21/08 11:40a agin
 * PR49585: Compile error.
 *
 * Hydra_Software_Devel/102   11/21/08 10:56a agin
 * PR49585: Add sw i2c support for 7601.
 *
 * Hydra_Software_Devel/101   10/17/08 5:06p agin
 * PR42305: Fixed warnings.
 *
 * Hydra_Software_Devel/100   10/14/08 8:41p pntruong
 * PR42305: Fixed cplusplus comment build errors.
 *
 * Hydra_Software_Devel/100   10/14/08 8:37p pntruong
 * PR42305: Fixed cplusplus comment build errors.
 *
 * Hydra_Software_Devel/99   10/14/08 5:07p agin
 * PR42305: Add BI2C_P_ReadSwNoAddr and BI2C_P_ReadSwEDDC.
 *
 * Hydra_Software_Devel/98   10/14/08 4:41p jkim
 * PR46727: add sw i2c function for 7405
 *
 * Hydra_Software_Devel/97   8/28/08 9:37a agin
 * PR46159: Kernel oops from the function "BI2C_P_ReadCmd()" of BI2C
 * module.
 *
 * Hydra_Software_Devel/96   8/21/08 8:27a fbasso
 * PR 44540: added support for 7601.
 *
 * Hydra_Software_Devel/95   7/7/08 2:25p vishk
 * PR 44620: The return value for ReadCmd and WriteCmd were modified to
 * meet coverity requirements and needs to be reverted back.
 *
 * Hydra_Software_Devel/94   6/11/08 6:43p maivu
 * PR 43537: Modify correct 3548 ii2c interrupt id
 *
 * Hydra_Software_Devel/93   5/7/08 7:22p agin
 * PR42305: Add delay based on clk rate for i2c gpio.
 *
 * Hydra_Software_Devel/92   5/7/08 10:44a agin
 * PR42305: Fixed warnings.
 *
 * Hydra_Software_Devel/91   5/6/08 4:05p agin
 * PR42305: Fixed warnings.
 *
 * Hydra_Software_Devel/90   5/6/08 11:23a agin
 * PR42305: Support 7400 i2c gpio.
 *
 * Hydra_Software_Devel/89   5/5/08 6:35p agin
 * PR42305: Remove limit for number of bytes to transfer or i2c gpio.
 * Restore sun_top_ctrl register properly.
 *
 * Hydra_Software_Devel/88   5/5/08 5:05p agin
 * PR42305: Need to set gpio as output for BI2C_P_WriteSwCmd.
 *
 * Hydra_Software_Devel/87   5/5/08 2:53p agin
 * PR42305: Fixed warnings.
 *
 * Hydra_Software_Devel/86   5/4/08 7:11p agin
 * PR42305: Use reg handles generically.
 *
 * Hydra_Software_Devel/85   5/4/08 2:08a agin
 * PR42305: Support i2c via gpio.
 *
 * Hydra_Software_Devel/84   4/21/08 1:17p farshidf
 * PR41729: Add BREG_I2C_ReadNoAddrNoAck function
 *
 * Hydra_Software_Devel/83   4/10/08 2:24p vishk
 * PR 41190: Coverity Defect ID:7917 CHECKED_RETURN
 *
 * Hydra_Software_Devel/82   4/7/08 11:43a farshidf
 * PR39178: fix the i2c compile issue
 *
 * Hydra_Software_Devel/81   4/4/08 10:43a farshidf
 * PR39178: add 3548 support
 *
 * Hydra_Software_Devel/80   4/4/08 10:09a vishk
 * PR 41190: Coverity Defect ID:7917 CHECKED_RETURN,
 * PR 41189: Coverity Defect ID:7917 CHECKED_RETURN
 *
 * Hydra_Software_Devel/79   2/26/08 2:19p jkim
 * PR39964: remove compiler warning by deleteing lines checking for
 * BI2C_MAX_I2C_MSTR_CHANNELS and BI2C_MAX_I2C_CHANNELS since these
 * definitions are not used for 7405, which this function is for.
 *
 * Hydra_Software_Devel/78   1/28/08 7:10p agin
 * PR39047: Ensure that the stop condition was not previously sent out
 * before sending it.
 *
 * Hydra_Software_Devel/77   1/25/08 11:08p agin
 * PR39047: Send stop condition after I2C wait for completion failure due
 * to no-ack.
 *
 * Hydra_Software_Devel/76   11/27/07 6:02p farshidf
 * PR36894: I2c update for 7335
 *
 * Hydra_Software_Devel/75   10/18/07 4:01p jkim
 * PR29251: fix I2C problrm related to 32 byte transfer
 *
 * Hydra_Software_Devel/74   10/14/07 3:51p jkim
 * PR14344: Add 7325 support
 *
 * Hydra_Software_Devel/73   10/4/07 3:27p brianlee
 * PR35095: Another check for return code.
 *
 * Hydra_Software_Devel/72   10/4/07 3:19p brianlee
 * PR35050: Check return code after wait for completion.
 *
w * Hydra_Software_Devel/71   6/4/07 11:42a farshidf
 * PR29538: Add burst mode support
 *
 * Hydra_Software_Devel/PR29538/1   5/17/07 4:52p dyzhang
 * PR29538: Add burst mode support
 *
 * Hydra_Software_Devel/70   5/24/07 1:56p jkim
 * PR29251: fix compiler warning
 *
 * Hydra_Software_Devel/69   5/24/07 11:51a jkim
 * PR29251: Add 4byte transfer support for 7405
 *
 * Hydra_Software_Devel/68   5/2/07 10:15a jkim
 * PR30340: undo the changes
 *
 * Hydra_Software_Devel/67   5/1/07 5:51p jkim
 * PR30340: initialize the event to NULL during channelOpen
 *
 * Hydra_Software_Devel/66   4/24/07 5:29p jkim
 * PR29538: Add burst mode support
 *
 * Hydra_Software_Devel/65   4/3/07 12:46p katrep
 * PR29073:Added support for 7405
 *
 * Hydra_Software_Devel/65   3/30/07 6:59p katrep
 * PR29073:Added Support for 7405
 *
 * Hydra_Software_Devel/65   3/30/07 6:55p katrep
 * PR29073:Added support for 7405
 *
 * Hydra_Software_Devel/65   3/30/07 6:54p katrep
 * PR29073:Added support for 7405
 *
 * Hydra_Software_Devel/65   3/30/07 6:50p katrep
 * PR29073:Added 7405 support does not include the 32byte buffer support
 * for i2c
 *
 * Hydra_Software_Devel/64   2/2/07 11:19a jkim
 * PR27238: Modify to use the correct IRQ definition
 *
 * Hydra_Software_Devel/63   1/20/07 2:08p agin
 * PR27240: hChnEvent was set to NULL after it was created.
 *
 * Hydra_Software_Devel/62   1/18/07 5:22p jkim
 * PR26978: Initialize hChnEvent and hChnCallback to NULL.
 *
 * Hydra_Software_Devel/61   1/12/07 4:22p jkim
 * PR14344: Add 3563 support
 *
 * Hydra_Software_Devel/60   10/31/06 3:14p erickson
 * PR25108: add 7403 support
 *
 * Hydra_Software_Devel/59   9/18/06 2:21p agin
 * PR23800: Timeout value hardcoded in bi2c.c
 *
 * Hydra_Software_Devel/58   9/6/06 4:12p jkim
 * PR14344: add ing 7440 support
 *
 * Hydra_Software_Devel/57   8/9/06 10:46a agin
 * PR23362: Add 3563 support.
 *
 * Hydra_Software_Devel/56   6/23/06 12:11a erickson
 * PR21861: increased timeout to 20 milliseconds based on observed
 * requirements
 *
 * Hydra_Software_Devel/55   6/16/06 11:48a mward
 * PR21846: add "-mips2 -O3" gcc option to appframework
 *
 * Hydra_Software_Devel/54   6/15/06 5:14p mward
 * PR21678: Add support for 7118 chip 97118 board
 *
 * Hydra_Software_Devel/53   5/5/06 3:40p dlwin
 * PR 20343: Disable the use BERR_TRACE() for timeout errors. It generated
 * error messages when probling for optional cards.
 *
 * Hydra_Software_Devel/52   4/20/06 6:50p agin
 * PR14597: Don't need MSB fix for BX chips.
 *
 * Hydra_Software_Devel/51   2/14/06 2:58p agin
 * PR19666: 7438, not 7348.
 *
 * Hydra_Software_Devel/50   2/14/06 2:28p agin
 * PR19666: Need to create 7438 I2C porting interface.
 *
 * Hydra_Software_Devel/49   2/14/06 10:57a jgarrett
 * PR 19663: Extending minimum I2C timeout to 1s for 7400, not 1ms.
 *
 * Hydra_Software_Devel/48   1/25/06 11:52a gmullen
 * PR19220: Added INT ID for 5th I2C master.
 *
 * Hydra_Software_Devel/47   1/23/06 4:58p agin
 * PR19220: Support 5th I2C controller.
 *
 * Hydra_Software_Devel/46   1/14/06 10:13p agin
 * PR19076: Support BCM7400.
 *
 * Hydra_Software_Devel/45   12/22/05 10:37a agin
 * PR18787: Update I2C, KIR with C3 defines using new version method.
 *
 * Hydra_Software_Devel/44   9/27/05 2:13p mphillip
 * PR16012: I believe this was the intention of the conditional (fixes the
 * pre-C2/B2 workaround so it doesn't apply for C2/B2 builds)
 *
 * Hydra_Software_Devel/43   9/9/05 5:06p jkim
 * PR16945:Per conversation with Dave Lwin, change BI2C_Delete2cRegHandle
 * back to BI2C_CloseI2cRegHandle
 *
 * Hydra_Software_Devel/42   9/7/05 3:42p jkim
 * PR16945: Change BI2C_CloseI2CRegHandle to BI2C_DeleteI2CRegHandle
 *
 * Hydra_Software_Devel/41   8/1/05 5:22p vsilyaev
 * PR 16012: Added support for 7401
 *
 * Hydra_Software_Devel/40   6/24/05 5:17p agin
 * PR14597: Make MSB fix applicable to B1/C1 or earlier chips.  And don't
 * forget the 3560 chip.
 *
 * Hydra_Software_Devel/39   6/23/05 2:35p agin
 * PR14597: Make MSB fix applicable to B1/C1 or earlier chips.
 *
 * Hydra_Software_Devel/38   4/18/05 10:46a agin
 * PR14828: add read and write no ack functions.
 *
 * Hydra_Software_Devel/37   4/4/05 10:24a agin
 * PR14351: Removed conditional compile call to BI2C_P_WriteCmdNoAck from
 * BI2C_P_WriteNoAddr.
 *
 * Hydra_Software_Devel/36   4/1/05 9:36a agin
 * PR14597: fix compiler problem with comment again.
 *
 * Hydra_Software_Devel/35   3/31/05 9:13p agin
 * PR14597: make sure that no ack is properly returned for read.
 *
 * Hydra_Software_Devel/34   3/29/05 3:47p agin
 * PR14597: fix compiler problem with comment.
 *
 * Hydra_Software_Devel/33   3/29/05 10:38a agin
 * PR14597: Fix for msb I2C problem.
 *
 * Hydra_Software_Devel/32   3/22/05 9:43p dlwin
 * PR 14351: More compiler warning fixes.
 *
 * Hydra_Software_Devel/31   3/18/05 4:40p agin
 * PR14351: Fassl's changes, fixed compiler error w/ incorrect num of
 * parameters.
 *
 * Hydra_Software_Devel/30   3/16/05 4:39p dlwin
 * PR 14240: More updated for 3560
 *
 * Hydra_Software_Devel/28   3/14/05 5:51p agin
 * PR14351: Fassl's changes
 *
 * Hydra_Software_Devel/27   3/10/05 8:29a dlwin
 * PR 14240: Added support for 3560 and future chip that use this same
 * I2C core.
 *
 * Hydra_Software_Devel/26   3/3/05 2:18p agin
 * PR14318:  removed #ifdef.
 *
 * Hydra_Software_Devel/25   3/2/05 5:49p agin
 * PR14318:  Added support for I2C write for NVRAM devices.
 *
 * Hydra_Software_Devel/24   2/10/05 6:25p agin
 * PR14102:  Handle I2C zero data byte write.
 *
 * Hydra_Software_Devel/23   2/2/05 4:07p pntruong
 * PR12342: Removed Metrowerks build errors and warnings.
 *
 * Hydra_Software_Devel/22   7/12/04 1:58p brianlee
 * PR11723: Get rid of floating point calculation.
 *
 * Hydra_Software_Devel/21   7/8/04 2:23p brianlee
 * PR11845: Add I2C API for 3-byte sub address.
 *
 * Hydra_Software_Devel/20   3/26/04 5:49p brianlee
 * PR10174: Fix the handling of 10-bit address.
 *
 * Hydra_Software_Devel/19   3/26/04 4:32p brianlee
 * PR8971: Remove BDBG_ASSERT() for malloc failure.
 *
 * Hydra_Software_Devel/18   1/28/04 10:21a brianlee
 * PR9499: Fix breg_i2c.h file inclusion.
 *
 * Hydra_Software_Devel/17   1/13/04 5:02p brianlee
 * PR9268: Make write structures constant.
 *
 * Hydra_Software_Devel/16   12/29/03 3:59p marcusk
 * PR9117: Updated with changes required to support interrupt ids rather
 * than strings.
 *
 * Hydra_Software_Devel/15   12/18/03 2:16p marcusk
 * PR8985: The interrupt interface will automatically unmask the interrupt
 * register.  No need to program it directly.
 *
 * Hydra_Software_Devel/14   11/21/03 10:06a brianlee
 * Put BERR_TRACE macro around the places where error code is returned.
 *
 * Hydra_Software_Devel/13   11/6/03 9:47a brianlee
 * Added BI2C_CloseI2cRegHandle() function.
 *
 * Hydra_Software_Devel/12   11/4/03 6:47p brianlee
 * Get rid of enter/leave macros.
 *
 * Hydra_Software_Devel/11   10/27/03 5:22p brianlee
 * In EDDC read/write functions, ignore ACK during the writing of the
 * segment pointer.
 *
 * Hydra_Software_Devel/10   10/15/03 5:06p brianlee
 * Fixed a hang with no ACK occurs.
 *
 * Hydra_Software_Devel/9   10/9/03 7:04p brianlee
 * Fixed a problem with writing multiple bytes.
 *
 * Hydra_Software_Devel/8   10/3/03 9:52a brianlee
 * Fixed a bug when copying data from I2C to buffer.
 *
 * Hydra_Software_Devel/7   9/24/03 11:56a brianlee
 * Changed the names of header files.
 *
 * Hydra_Software_Devel/6   9/19/03 12:20p brianlee
 * Fixed warnings from Midas build.
 *
 * Hydra_Software_Devel/5   9/18/03 10:16a brianlee
 * Make SetClk and GetClk functions public so the caller can change the
 * clock rate after the channel is opened.
 *
 * Hydra_Software_Devel/4   9/15/03 10:25a brianlee
 * Changed TRUE/FALSE to lower case.
 *
 * Hydra_Software_Devel/3   9/12/03 5:02p brianlee
 * Add chip address to EDDC read/write functions instead of using the
 * fixed value of A0.
 *
 * Hydra_Software_Devel/2   9/11/03 5:09p brianlee
 * Don't disable callback inside ISR.
 *
 * Hydra_Software_Devel/1   9/11/03 11:43a brianlee
 * Initial version.
 *
 ***************************************************************************/
#include "bstd.h"
#include "breg_i2c_priv.h"
#include "bi2c.h"
#include "bi2c_priv.h"
#include "bchp_irq0.h"
#include "bchp_int_id_irq0.h"
#include "bchp_gio.h"
#include "bchp_sun_top_ctrl.h"

#if ((BCHP_CHIP==7344) && (BCHP_VER>=BCHP_VER_B0)) || (BCHP_CHIP==7425) || (BCHP_CHIP==7429) || (BCHP_CHIP==7640) || (BCHP_CHIP==7435) || (BCHP_CHIP==7445)
    #include "bchp_int_id_irq0_aon.h"
#endif
#if (BCHP_CHIP==7346) || (BCHP_CHIP==7425) || (BCHP_CHIP==7435) || (BCHP_CHIP==7429)
    #include "bchp_gio_aon.h"
#endif

#if (BCHP_CHIP==7231) || (BCHP_CHIP==7344) || (BCHP_CHIP==7346) || (BCHP_CHIP==7422) || \
    (BCHP_CHIP==7425) || (BCHP_CHIP==7429) || (BCHP_CHIP==7435) || (BCHP_CHIP==7358) || \
    (BCHP_CHIP==7360) || (BCHP_CHIP==7552) || (BCHP_CHIP==7563)
    #include "bchp_aon_pin_ctrl.h"
    #include "bchp_irq0_aon.h"
    #if (BCHP_CHIP==7231) || (BCHP_CHIP==7422) || (BCHP_CHIP==7358) || (BCHP_CHIP==7552) || \
        (BCHP_CHIP==7360) || (BCHP_CHIP==7563)
        #define BCHP_INT_ID_iicd_irqen       BCHP_INT_ID_CREATE(BCHP_IRQ0_AON_IRQEN, BCHP_IRQ0_AON_IRQEN_iicd_irqen_SHIFT)
        #define BCHP_INT_ID_iice_irqen       BCHP_INT_ID_CREATE(BCHP_IRQ0_AON_IRQEN, BCHP_IRQ0_AON_IRQEN_iice_irqen_SHIFT)
    #endif
#endif

#if (BCHP_CHIP==7344)
    #if (BCHP_VER<BCHP_VER_B0)
        #define BCHP_INT_ID_iicd_irqen       BCHP_INT_ID_CREATE(BCHP_IRQ0_AON_IRQEN, BCHP_IRQ0_AON_IRQEN_iicd_irqen_SHIFT)
    #endif
    #define BCHP_INT_ID_iice_irqen       BCHP_INT_ID_CREATE(BCHP_IRQ0_AON_IRQEN, BCHP_IRQ0_AON_IRQEN_iice_irqen_SHIFT)
#endif

#if (BCHP_CHIP==7346)
    #if (BCHP_VER >= BCHP_VER_B0)
        #define BCHP_IRQ0_IRQEN_iicd_irqen_SHIFT     27
        #define BCHP_IRQ0_AON_IRQEN_iice_irqen_SHIFT 27
        #define BCHP_INT_ID_iicd_irqen       BCHP_INT_ID_CREATE(BCHP_IRQ0_IRQEN, BCHP_IRQ0_IRQEN_iicd_irqen_SHIFT)
        #define BCHP_INT_ID_iice_irqen       BCHP_INT_ID_CREATE(BCHP_IRQ0_AON_IRQEN, BCHP_IRQ0_AON_IRQEN_iice_irqen_SHIFT)
    #else
        #define BCHP_INT_ID_iicd_irqen       BCHP_INT_ID_CREATE(BCHP_IRQ0_AON_IRQEN, BCHP_IRQ0_AON_IRQEN_iicd_irqen_SHIFT)
        #define BCHP_INT_ID_iice_irqen       BCHP_INT_ID_CREATE(BCHP_IRQ0_AON_IRQEN, BCHP_IRQ0_AON_IRQEN_iice_irqen_SHIFT)
    #endif
#endif

#if (BCHP_CHIP==7425)
    #if (BCHP_VER==BCHP_VER_A0 || BCHP_VER==BCHP_VER_A1)
        #define BCHP_INT_ID_iicd_irqen       BCHP_INT_ID_CREATE(BCHP_IRQ0_AON_IRQEN, BCHP_IRQ0_AON_IRQEN_iicd_irqen_SHIFT)
        #define BCHP_INT_ID_iice_irqen       BCHP_INT_ID_CREATE(BCHP_IRQ0_AON_IRQEN, BCHP_IRQ0_AON_IRQEN_iice_irqen_SHIFT)
    #endif
#endif

#include "bchp_bsca.h"
#if (BI2C_MAX_I2C_CHANNELS >= 2)
    #include "bchp_bscb.h"
#if (BI2C_MAX_I2C_CHANNELS >= 3)
    #if (BCHP_CHIP==7563) || (BCHP_CHIP==7584)
        #include "bchp_bscd.h"
    #else
        #include "bchp_bscc.h"
    #endif
#if (BI2C_MAX_I2C_CHANNELS >= 4)
    #include "bchp_bscd.h"
#if (BI2C_MAX_I2C_CHANNELS >= 5)
    #include "bchp_bsce.h"
#endif
#endif
#endif
#endif

#include "bkni_multi.h"

#define BI2C_P_ACQUIRE_MUTEX(handle) BKNI_AcquireMutex((handle)->hMutex)
#define BI2C_P_RELEASE_MUTEX(handle) BKNI_ReleaseMutex((handle)->hMutex)

#if defined(HUMAX_PLATFORM_BASE)
    #define BI2C_P_MIN_TIMEOUT_MS 1000
#else
#if (BCHP_CHIP==7400)
    #define BI2C_P_MIN_TIMEOUT_MS 1000  /* JDG - Temporary patch to avoid FPGA timeouts */
#else
    #define BI2C_P_MIN_TIMEOUT_MS 20 	/* PR 21861 - 4 millseconds observed to be required on realtime OS. */
#endif
#endif

/* Add 10% for fudge factor and round it up */
#define BI2C_P_CALCULATE_TIMEOUT(lval, clkRate) ((((lval) * 1100)/(clkRate)) + BI2C_P_MIN_TIMEOUT_MS)

BDBG_MODULE(bi2c);

#define DEV_MAGIC_ID            ((BERR_I2C_ID<<16) | 0xFACE)

#define BI2C_CHK_RETCODE( rc, func )        \
do {                                        \
    if( (rc = BERR_TRACE(func)) != BERR_SUCCESS ) \
    {                                       \
        goto done;                          \
    }                                       \
} while(0)


#define MAX_I2C_READ_REQUEST            8
#define MAX_I2C_READ_NO_ADDR_REQUEST    8
#define MAX_I2C_WRITE_REQUEST           8
#define MAX_I2C_WRITE_NO_ADDR_REQUEST   8
#define I2C_POLLING_INTERVAL            50      /* in usecs */

#define EDDC_SEGMENT_CHIP_ADDR          0x60

#if (BCHP_CHIP==7038) || (BCHP_CHIP==7325) || (BCHP_CHIP==7335) || \
    (BCHP_CHIP==7336) || (BCHP_CHIP==7346) || (BCHP_CHIP==7400) || (BCHP_CHIP==7413) || \
    (BCHP_CHIP==7420) || (BCHP_CHIP==7550) || (BCHP_CHIP==7405) || \
    (BCHP_CHIP==7601) || (BCHP_CHIP==7635) || (BCHP_CHIP==7630) || \
    (BCHP_CHIP==7125) || (BCHP_CHIP==7408) || (BCHP_CHIP==7422) || \
    (BCHP_CHIP==7425) || (BCHP_CHIP==7231) || (BCHP_CHIP==7435) || \
    (BCHP_CHIP==7429)
    #define BI2C_SUPPORT_I2C_VIA_GPIO       1
#else
    #define BI2C_SUPPORT_I2C_VIA_GPIO       0
#endif

#ifdef BCHP_BSCA_CTLHI_REG_DATA_REG_SIZE_MASK
    #define BI2C_SUPPORT_4_BYTE_XFR_MODE
#endif

#if ((BCHP_CHIP==7601) && (BCHP_VER >= BCHP_VER_B0)) || (BCHP_CHIP==7635) || (BCHP_CHIP == 7630) || \
    (BCHP_CHIP == 7420) || (BCHP_CHIP == 7125) || (BCHP_CHIP == 7340) || (BCHP_CHIP == 7342) || \
    (BCHP_CHIP == 7550) || (BCHP_CHIP == 7408) || (BCHP_CHIP == 7468) || (BCHP_CHIP == 7208) || \
    (BCHP_CHIP == 7422) || (BCHP_CHIP==7425) || (BCHP_CHIP==7435)
    #define BI2C_USES_SETUP_HDMI_HW_ACCESS
#endif

#if (BCHP_CHIP==7601) && (BCHP_VER >= BCHP_VER_B0)
    #define BI2C_USES_SET_SDA_DELAY
#endif

int BI2C_P_RegBitSet(BREG_Handle hReg, uint32_t addr, uint32_t mask, uint8_t val);
int _BI2C_P_SunTopScl(BREG_Handle hReg, int ch, uint8_t val);
int _BI2C_P_SunTopSda(BREG_Handle hReg, int ch, uint8_t val);
BERR_Code _BI2C_P_ClkSet(BREG_Handle hReg, int ch, int clk, BI2C_Clk rate);
int _BI2C_P_DataGet(BREG_Handle hReg, int ch);
BERR_Code _BI2C_P_DataSet(BREG_Handle hReg, int ch, int data, BI2C_Clk rate);
BERR_Code _BI2C_P_ByteWrite(BREG_Handle hReg, int ch, unsigned char data, BI2C_Clk rate, int *ret_ack);
BERR_Code _BI2C_P_ByteRead(BREG_Handle hReg, int ch, int generate_ack, BI2C_Clk rate, unsigned char *ret_data);

#define BI2C_P_OdenScl(val) BI2C_P_RegBitSet(hReg, i2c_gpio[ch].gio_oden_scl_reg, i2c_gpio[ch].gio_scl_mask, val)
#define BI2C_P_OdenSda(val) BI2C_P_RegBitSet(hReg, i2c_gpio[ch].gio_oden_sda_reg, i2c_gpio[ch].gio_sda_mask, val)
#define BI2C_P_IodirScl(val) BI2C_P_RegBitSet(hReg, i2c_gpio[ch].gio_iodir_scl_reg, i2c_gpio[ch].gio_scl_mask, val)
#define BI2C_P_IodirSda(val) BI2C_P_RegBitSet(hReg, i2c_gpio[ch].gio_iodir_sda_reg, i2c_gpio[ch].gio_sda_mask, val)
#define BI2C_P_DataScl(val) BI2C_P_RegBitSet(hReg, i2c_gpio[ch].gio_data_scl_reg, i2c_gpio[ch].gio_scl_mask, val)
#define BI2C_P_DataSda(val) BI2C_P_RegBitSet(hReg, i2c_gpio[ch].gio_data_sda_reg, i2c_gpio[ch].gio_sda_mask, val)

#define BI2C_P_SunTopScl(val) _BI2C_P_SunTopScl(hReg, ch, val)
#define BI2C_P_SunTopSda(val) _BI2C_P_SunTopSda(hReg, ch, val)
#define BI2C_P_ClkSet(clk) _BI2C_P_ClkSet(hReg, ch, clk, rate)
#define BI2C_P_DataGet() _BI2C_P_DataGet(hReg, ch)
#define BI2C_P_DataSet(clk) _BI2C_P_DataSet(hReg, ch, clk, rate)
#define BI2C_P_ByteWrite(data, ret_ack) _BI2C_P_ByteWrite(hReg, ch, data, rate, ret_ack)
#define BI2C_P_ByteRead(generate_ack, ret_data) _BI2C_P_ByteRead(hReg, ch, generate_ack, rate, ret_data)

/*******************************************************************************
*
*   Private Module Handles
*
*******************************************************************************/

typedef struct BI2C_P_Handle
{
    uint32_t        magicId;                    /* Used to check if structure is corrupt */
    BCHP_Handle     hChip;
    BREG_Handle     hRegister;
    BINT_Handle     hInterrupt;
    unsigned int    maxChnNo;
    BI2C_ChannelHandle hI2cChn[BI2C_MAX_I2C_CHANNELS+EXTRA_SW_I2C_CHANNELS];
} BI2C_P_Handle;

typedef struct BI2C_P_ChannelHandle
{
    uint32_t        magicId;                    /* Used to check if structure is corrupt */
    BI2C_Handle     hI2c;
    unsigned int    chnNo;
    uint32_t        coreOffset;
    BKNI_EventHandle    hChnEvent;
    BINT_CallbackHandle hChnCallback;
    BKNI_MutexHandle hMutex;                    /* Mutex handle for serialization */

    BI2C_Clk            clkRate;
    unsigned int        timeoutMs;
    bool                intMode;
    bool                noAck;
    bool                nvramAck;

    /* use burst mode R/W or not */
    bool                burstMode;

    /* use software i2c or not */
    bool                softI2c;

} BI2C_P_ChannelHandle;

/* Ack/Nack bits in I2C protocol */
#define I2C_ACK         0
#define I2C_NACK        1

/* user defined delay for I2C */
int I2c_delay_count = 200;

typedef struct i2c_gpio_params {
    unsigned long gio_scl_mask;
    unsigned long gio_sda_mask;
    unsigned long gio_data_scl_reg;
    unsigned long gio_data_sda_reg;
    unsigned long gio_iodir_scl_reg;
    unsigned long gio_iodir_sda_reg;
    unsigned long gio_oden_scl_reg;
    unsigned long gio_oden_sda_reg;
    unsigned long sun_top_scl_reg;
    unsigned long sun_top_scl_shift;
    unsigned long sun_top_scl_mask;
    unsigned long sun_top_sda_reg;
    unsigned long sun_top_sda_shift;
    unsigned long sun_top_sda_mask;
} i2c_gpio_params;

i2c_gpio_params i2c_gpio[BI2C_MAX_I2C_CHANNELS+EXTRA_SW_I2C_CHANNELS];

/*******************************************************************************
*
*   Default Module Settings
*
*******************************************************************************/
static const BI2C_Settings defI2cSettings =
{
    NULL
};

static const BI2C_ChannelSettings defI2cXChnSettings[] =
{
    {   /* Channel A */
        BI2C_Clk_eClk400Khz,
        true,
        BI2C_TimeoutBasedOnClkSpeed,
        false,
        false,
        true
    }
#if (BI2C_MAX_I2C_CHANNELS+EXTRA_SW_I2C_CHANNELS >= 2)
    ,{  /* Channel B */
        BI2C_Clk_eClk400Khz,
        true,
        BI2C_TimeoutBasedOnClkSpeed,
        false,
        false,
        true
    }
#if (BI2C_MAX_I2C_CHANNELS+EXTRA_SW_I2C_CHANNELS >= 3)
    ,{  /* Channel C */
        BI2C_Clk_eClk400Khz,
        true,
        BI2C_TimeoutBasedOnClkSpeed,
        false,
        false,
        true
    }
#if (BI2C_MAX_I2C_CHANNELS+EXTRA_SW_I2C_CHANNELS >= 4)
    ,{  /* Channel D */
        BI2C_Clk_eClk400Khz,
        true,
        BI2C_TimeoutBasedOnClkSpeed,
        false,
        false,
        true
    }
#if (BI2C_MAX_I2C_CHANNELS+EXTRA_SW_I2C_CHANNELS >= 5)
    ,{  /* Channel E */
        BI2C_Clk_eClk400Khz,
        true,
        BI2C_TimeoutBasedOnClkSpeed,
        false,
        false,
        true
    }
#endif
#endif
#endif
#endif
};

static const BINT_Id IntId[] =
{
    #if defined(BCHP_INT_ID_iica)
    BCHP_INT_ID_iica
    #elif defined(BCHP_INT_ID_iica_irqen)
    BCHP_INT_ID_iica_irqen
    #else
        #error "Not supported in chip"
    #endif

#if (BI2C_MAX_I2C_CHANNELS >= 2)
    #if defined(BCHP_INT_ID_iicb)
    ,BCHP_INT_ID_iicb
    #elif defined(BCHP_INT_ID_iicb_irqen)
    ,BCHP_INT_ID_iicb_irqen
    #else
        #error "Not supported in chip"
    #endif

#if (BI2C_MAX_I2C_CHANNELS >= 3)
    #if defined(BCHP_INT_ID_iicc)
    ,BCHP_INT_ID_iicc
    #elif defined(BCHP_INT_ID_iicc_irqen)
    ,BCHP_INT_ID_iicc_irqen
    #else
        #error "Not supported in chip"
    #endif

#if (BI2C_MAX_I2C_CHANNELS >= 4)
    #if defined(BCHP_INT_ID_iicd)
    ,BCHP_INT_ID_iicd
    #elif defined(BCHP_INT_ID_iicd_irqen)
    ,BCHP_INT_ID_iicd_irqen
    #else
        #error "Not supported in chip"
    #endif

#if (BI2C_MAX_I2C_CHANNELS >= 5)
    #if defined(BCHP_INT_ID_iice)
    ,BCHP_INT_ID_iice
    #elif defined(BCHP_INT_ID_iice_irqen)
    ,BCHP_INT_ID_iice_irqen
    #else
        #error "Not supported in chip"
    #endif
#endif
#endif
#endif
#endif
};

static const uint32_t coreOffsetVal[] =
{
    0
#if (BI2C_MAX_I2C_CHANNELS >= 2)
    , BCHP_BSCB_CHIP_ADDRESS - BCHP_BSCA_CHIP_ADDRESS
#if (BI2C_MAX_I2C_CHANNELS >= 3)
    #if (BCHP_CHIP==7563) || (BCHP_CHIP==7584)
        , BCHP_BSCD_CHIP_ADDRESS - BCHP_BSCA_CHIP_ADDRESS
    #else
        , BCHP_BSCC_CHIP_ADDRESS - BCHP_BSCA_CHIP_ADDRESS
    #endif
#if (BI2C_MAX_I2C_CHANNELS >= 4)
    , BCHP_BSCD_CHIP_ADDRESS - BCHP_BSCA_CHIP_ADDRESS
#if (BI2C_MAX_I2C_CHANNELS >= 5)
    , BCHP_BSCE_CHIP_ADDRESS - BCHP_BSCA_CHIP_ADDRESS
#endif
#endif
#endif
#endif
};

/*******************************************************************************
*
*   Public Module Functions
*
*******************************************************************************/
BERR_Code BI2C_Open(
    BI2C_Handle *pI2c,                  /* [output] Returns handle */
    BCHP_Handle hChip,                  /* Chip handle */
    BREG_Handle hRegister,              /* Register handle */
    BINT_Handle hInterrupt,             /* Interrupt handle */
    const BI2C_Settings *pDefSettings   /* Default settings */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BI2C_Handle hDev;
    unsigned int chnIdx;

    /* Sanity check on the handles we've been given. */
    BDBG_ASSERT( hChip );
    BDBG_ASSERT( hRegister );
    BDBG_ASSERT( hInterrupt );
    BSTD_UNUSED( pDefSettings );

    /* Alloc memory from the system heap */
    hDev = (BI2C_Handle) BKNI_Malloc( sizeof( BI2C_P_Handle ) );
    if( hDev == NULL )
    {
        *pI2c = NULL;
        retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        BDBG_ERR(("BI2C_Open: BKNI_malloc() failed\n"));
        goto done;
    }

    hDev->magicId   = DEV_MAGIC_ID;
    hDev->hChip     = hChip;
    hDev->hRegister = hRegister;
    hDev->hInterrupt = hInterrupt;
    hDev->maxChnNo  = BI2C_MAX_I2C_CHANNELS+EXTRA_SW_I2C_CHANNELS;
    for( chnIdx = 0; chnIdx < hDev->maxChnNo; chnIdx++ )
    {
        hDev->hI2cChn[chnIdx] = NULL;
    }

    BI2C_P_GpioInit();

    *pI2c = hDev;

done:
    return( retCode );
}

BERR_Code BI2C_Close(
    BI2C_Handle hDev                    /* Device handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );
    BKNI_Free( (void *) hDev );

    return( retCode );
}

BERR_Code BI2C_GetDefaultSettings(
    BI2C_Settings *pDefSettings,        /* [output] Returns default setting */
    BCHP_Handle hChip                   /* Chip handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;

    BSTD_UNUSED( hChip );

    *pDefSettings = defI2cSettings;

    return( retCode );
}

BERR_Code BI2C_GetTotalChannels(
    BI2C_Handle hDev,                   /* Device handle */
    unsigned int *totalChannels         /* [output] Returns total number downstream channels supported */
    )
{
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    *totalChannels = hDev->maxChnNo;

    return( retCode );
}

BERR_Code BI2C_GetChannelDefaultSettings(
    BI2C_Handle hDev,                   /* Device handle */
    unsigned int channelNo,             /* Channel number to default setting for */
    BI2C_ChannelSettings *pChnDefSettings /* [output] Returns channel default setting */
    )
{
    BERR_Code retCode = BERR_SUCCESS;

    BSTD_UNUSED(hDev);

    if( channelNo < BI2C_MAX_I2C_CHANNELS+EXTRA_SW_I2C_CHANNELS )
    {
        *pChnDefSettings = defI2cXChnSettings[channelNo];
    }
    else
    {
        retCode = BERR_TRACE (BERR_INVALID_PARAMETER);
    }

    return( retCode );
}

BERR_Code BI2C_OpenChannel(
    BI2C_Handle hDev,                   /* Device handle */
    BI2C_ChannelHandle *phChn,          /* [output] Returns channel handle */
    unsigned int channelNo,             /* Channel number to open */
    const BI2C_ChannelSettings *pChnDefSettings /* Channel default setting */
    )
{
    BERR_Code           retCode = BERR_SUCCESS;
    BI2C_ChannelHandle  hChn;
    uint32_t            lval;

    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    hChn = NULL;
    if( channelNo < hDev->maxChnNo )
    {
        if( hDev->hI2cChn[channelNo] == NULL )
        {
            /* Alloc memory from the system heap */
            hChn = (BI2C_ChannelHandle) BKNI_Malloc( sizeof( BI2C_P_ChannelHandle ) );
            if( hChn == NULL )
            {
                *phChn = NULL;
                retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
                BDBG_ERR(("BI2C_OpenChannel: BKNI_malloc() failed\n"));
                goto done;
            }

            retCode = BKNI_CreateMutex( &hChn->hMutex );
            if( retCode != BERR_SUCCESS )
            {
                BKNI_Free( hChn );
                hChn = NULL;
                goto done;
            }

            #if (EXTRA_SW_I2C_CHANNELS > 0)
            if (channelNo < BI2C_MAX_I2C_CHANNELS)
            #endif
            {
                BI2C_CHK_RETCODE( retCode, BKNI_CreateEvent( &(hChn->hChnEvent) ) );
                hChn->magicId = DEV_MAGIC_ID;
                hChn->hI2c = hDev;
                hChn->chnNo = channelNo;
                hDev->hI2cChn[channelNo] = hChn;
                hChn->nvramAck = 0;

                /*
                 * Offsets are based off of BSCA
                 */
                hChn->coreOffset = coreOffsetVal[channelNo];

                /* Program I2C clock rate */
                BI2C_SetClk (hChn, pChnDefSettings->clkRate);

                #ifdef BI2C_USES_SET_SDA_DELAY
                    BI2C_SetSdaDelay(hChn, BI2C_eSdaDelay815ns);
                #endif

                #ifdef BI2C_SUPPORT_4_BYTE_XFR_MODE
                    if (pChnDefSettings->fourByteXferMode)
                        BI2C_Set4ByteXfrMode(hChn, true);
                    else
                        BI2C_Set4ByteXfrMode(hChn, false);
                #endif

                /* Copy timeout value */
                hChn->timeoutMs = pChnDefSettings->timeoutMs;

                /* Enable/Disable burst mode for this channel */
                hChn->burstMode = pChnDefSettings->burstMode;

                /* Enable/Disable soft i2c for this channel */
                hChn->softI2c = pChnDefSettings->softI2c;

                /*
                 * Enable interrupt for this channel
                 */
                hChn->intMode = pChnDefSettings->intMode;

                /*
                 * Initialize the hChnCallback to NULL.
                 * If intMode is true, this pointer will be assigned accordingly.
                 */
                hChn->hChnCallback = NULL;

                if (hChn->intMode == true)
                {
                    /*
                     * Register and enable L2 interrupt.
                     */

                    BI2C_CHK_RETCODE( retCode, BINT_CreateCallback(
                        &(hChn->hChnCallback), hDev->hInterrupt, IntId[channelNo],
                        BI2C_P_HandleInterrupt_Isr, (void *) hChn, 0x00 ) );
                    BI2C_CHK_RETCODE( retCode, BINT_EnableCallback( hChn->hChnCallback ) );

                    /*
                     * Enable I2C interrupt in I2C Core
                     */
                    lval = BREG_Read32(hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CTL_REG));
                    lval |= BCHP_BSCA_CTL_REG_INT_EN_MASK;
                    BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CTL_REG), lval );
                }
                
                /* Make sure that the i2c bus is not held low by the controller */
                lval = BREG_Read32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_IIC_ENABLE));
                if (lval & (BCHP_BSCA_IIC_ENABLE_NO_STOP_MASK | BCHP_BSCA_IIC_ENABLE_ENABLE_MASK))
                {
                    BDBG_WRN(("BSC NO_STOP and ENABLE bits already set for channel=%d", channelNo));
                
                    /* turn off enable */
                    lval &= ~BCHP_BSCA_IIC_ENABLE_ENABLE_MASK;
                    BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_IIC_ENABLE), lval);
                
                    /* send stop */
                    lval &= ~BCHP_BSCA_IIC_ENABLE_NO_STOP_MASK;
                    lval |= BCHP_BSCA_IIC_ENABLE_ENABLE_MASK;
                    BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_IIC_ENABLE), lval);
                    BI2C_P_WaitForCompletion(hChn, 1);
                
                    /* turn off enable */
                    lval &= ~BCHP_BSCA_IIC_ENABLE_ENABLE_MASK;
                    BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_IIC_ENABLE), lval);
                }
            }
            #if (EXTRA_SW_I2C_CHANNELS > 0)
            else
            {
                /* This is for any extra I2C master channels requested for sw bit bang. */
                /* Enable/Disable soft i2c for this channel */
                hChn->magicId = DEV_MAGIC_ID;
                hChn->hI2c = hDev;
                hChn->chnNo = channelNo;
                hDev->hI2cChn[channelNo] = hChn;
                hChn->clkRate = pChnDefSettings->clkRate;
                hChn->softI2c = true;
            }
            #endif

            *phChn = hChn;
        }
        else
        {
            retCode = BERR_TRACE (BI2C_ERR_NOTAVAIL_CHN_NO);
        }
    }
    else
    {
        retCode = BERR_TRACE (BERR_INVALID_PARAMETER);
    }

done:
    if( retCode != BERR_SUCCESS )
    {
        if( hChn != NULL )
        {
            if( hChn->hChnEvent != NULL )
            {
                BKNI_DestroyEvent( hChn->hChnEvent );
            }
            BKNI_Free( hChn );
            hDev->hI2cChn[channelNo] = NULL;
            *phChn = NULL;
        }
    }
    return( retCode );
}

BERR_Code BI2C_CloseChannel(
    BI2C_ChannelHandle hChn         /* Device channel handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BI2C_Handle hDev;
    unsigned int chnNo;
    uint32_t    ctlReg;

    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hDev = hChn->hI2c;

#if EXTRA_SW_I2C_CHANNELS > 0
    if (hChn->chnNo >= BI2C_MAX_I2C_CHANNELS) {
        /* Sofware I2C channel. Nothing to do */
        goto swChannel;
    }
#endif
    /*
     * Disable interrupt for this channel
     */
    BKNI_EnterCriticalSection();
    ctlReg = BREG_Read32(hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CTL_REG));
    ctlReg &= ~BCHP_BSCA_CTL_REG_INT_EN_MASK;
    BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CTL_REG), ctlReg );
    BKNI_LeaveCriticalSection();

    if (hChn->hChnCallback != NULL)
    {
        BI2C_CHK_RETCODE( retCode, BINT_DisableCallback( hChn->hChnCallback ) );
        BI2C_CHK_RETCODE( retCode, BINT_DestroyCallback( hChn->hChnCallback ) );
    }

    if( hChn->hChnEvent != NULL )
    {
        BKNI_DestroyEvent( hChn->hChnEvent );
    }

#if EXTRA_SW_I2C_CHANNELS > 0
swChannel:
#endif
    chnNo = hChn->chnNo;
    BKNI_DestroyMutex(hChn->hMutex);
    BKNI_Free( hChn );
    hDev->hI2cChn[chnNo] = NULL;

done:
    return( retCode );
}

BERR_Code BI2C_GetDevice(
    BI2C_ChannelHandle hChn,            /* Device channel handle */
    BI2C_Handle *phDev                  /* [output] Returns Device handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    *phDev = hChn->hI2c;

    return( retCode );
}

BERR_Code BI2C_CreateI2cRegHandle(
    BI2C_ChannelHandle hChn,            /* Device channel handle */
    BREG_I2C_Handle *pI2cReg            /* [output]  */
    )
{
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    *pI2cReg = (BREG_I2C_Handle)BKNI_Malloc( sizeof(BREG_I2C_Impl) );
    if( *pI2cReg == NULL )
    {
        retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        BDBG_ERR(("BI2C_CreateI2cRegHandle: BKNI_malloc() failed"));
        goto done;
    }

    if (hChn->softI2c)
    {
        (*pI2cReg)->context                     = (void *)hChn;
        (*pI2cReg)->BREG_I2C_Write_Func         = BI2C_P_WriteSw;
        (*pI2cReg)->BREG_I2C_WriteSw_Func       = BI2C_P_WriteSw;
        (*pI2cReg)->BREG_I2C_WriteNoAck_Func    = BI2C_P_WriteNoAck;
        (*pI2cReg)->BREG_I2C_WriteA16_Func      = BI2C_P_WriteSwA16;
        (*pI2cReg)->BREG_I2C_WriteSwA16_Func    = BI2C_P_WriteSwA16;
        (*pI2cReg)->BREG_I2C_WriteA24_Func      = BI2C_P_WriteA24;
        (*pI2cReg)->BREG_I2C_WriteSwA24_Func    = BI2C_P_WriteSwA24;
        (*pI2cReg)->BREG_I2C_WriteNoAddr_Func   = BI2C_P_WriteNoAddr;
        (*pI2cReg)->BREG_I2C_WriteNoAddrNoAck_Func  = BI2C_P_WriteNoAddrNoAck;
        (*pI2cReg)->BREG_I2C_WriteNvram_Func    = BI2C_P_WriteNvram;
        (*pI2cReg)->BREG_I2C_Read_Func          = BI2C_P_ReadSw;
        (*pI2cReg)->BREG_I2C_ReadSw_Func        = BI2C_P_ReadSw;
        (*pI2cReg)->BREG_I2C_ReadNoAck_Func     = BI2C_P_ReadSwNoAck;
        (*pI2cReg)->BREG_I2C_ReadA16_Func       = BI2C_P_ReadSwA16;
        (*pI2cReg)->BREG_I2C_ReadSwA16_Func     = BI2C_P_ReadSwA16;
        (*pI2cReg)->BREG_I2C_ReadA24_Func       = BI2C_P_ReadSwA24;
        (*pI2cReg)->BREG_I2C_ReadSwA24_Func     = BI2C_P_ReadSwA24;
        (*pI2cReg)->BREG_I2C_ReadNoAddr_Func    = BI2C_P_ReadSwNoAddr;
        (*pI2cReg)->BREG_I2C_ReadSwNoAddr_Func  = BI2C_P_ReadSwNoAddr;
        (*pI2cReg)->BREG_I2C_ReadNoAddrNoAck_Func  = BI2C_P_ReadSwNoAddrNoAck;
        (*pI2cReg)->BREG_I2C_ReadEDDC_Func      = BI2C_P_ReadSwEDDC;
        (*pI2cReg)->BREG_I2C_ReadSwEDDC_Func    = BI2C_P_ReadSwEDDC;
        (*pI2cReg)->BREG_I2C_WriteEDDC_Func     = BI2C_P_WriteEDDC;
        (*pI2cReg)->BREG_I2C_SetupHdmiHwAccess_Func = BI2C_P_SetupHdmiHwAccess;
    }
    else
    {
        (*pI2cReg)->context                     = (void *)hChn;
        (*pI2cReg)->BREG_I2C_Write_Func         = BI2C_P_Write;
        (*pI2cReg)->BREG_I2C_WriteSw_Func       = BI2C_P_WriteSw;
        (*pI2cReg)->BREG_I2C_WriteNoAck_Func    = BI2C_P_WriteNoAck;
        (*pI2cReg)->BREG_I2C_WriteA16_Func      = BI2C_P_WriteA16;
        (*pI2cReg)->BREG_I2C_WriteSwA16_Func    = BI2C_P_WriteSwA16;
        (*pI2cReg)->BREG_I2C_WriteA24_Func      = BI2C_P_WriteA24;
        (*pI2cReg)->BREG_I2C_WriteSwA24_Func    = BI2C_P_WriteSwA24;
        (*pI2cReg)->BREG_I2C_WriteNoAddr_Func   = BI2C_P_WriteNoAddr;
        (*pI2cReg)->BREG_I2C_WriteNoAddrNoAck_Func  = BI2C_P_WriteNoAddrNoAck;
        (*pI2cReg)->BREG_I2C_WriteNvram_Func    = BI2C_P_WriteNvram;
        (*pI2cReg)->BREG_I2C_Read_Func          = BI2C_P_Read;
        (*pI2cReg)->BREG_I2C_ReadSw_Func        = BI2C_P_ReadSw;
        (*pI2cReg)->BREG_I2C_ReadNoAck_Func     = BI2C_P_ReadNoAck;
        (*pI2cReg)->BREG_I2C_ReadA16_Func       = BI2C_P_ReadA16;
        (*pI2cReg)->BREG_I2C_ReadSwA16_Func     = BI2C_P_ReadSwA16;
        (*pI2cReg)->BREG_I2C_ReadA24_Func       = BI2C_P_ReadA24;
        (*pI2cReg)->BREG_I2C_ReadSwA24_Func     = BI2C_P_ReadSwA24;
        (*pI2cReg)->BREG_I2C_ReadNoAddr_Func    = BI2C_P_ReadNoAddr;
        (*pI2cReg)->BREG_I2C_ReadSwNoAddr_Func  = BI2C_P_ReadSwNoAddr;
        (*pI2cReg)->BREG_I2C_ReadNoAddrNoAck_Func  = BI2C_P_ReadNoAddrNoAck;
        (*pI2cReg)->BREG_I2C_ReadEDDC_Func      = BI2C_P_ReadEDDC;
        (*pI2cReg)->BREG_I2C_ReadSwEDDC_Func    = BI2C_P_ReadSwEDDC;
        (*pI2cReg)->BREG_I2C_WriteEDDC_Func     = BI2C_P_WriteEDDC;
        (*pI2cReg)->BREG_I2C_SetupHdmiHwAccess_Func = BI2C_P_SetupHdmiHwAccess;
        (*pI2cReg)->BREG_I2C_WriteRead_Func     = BI2C_P_WriteRead;
        (*pI2cReg)->BREG_I2C_WriteReadNoAddr_Func     = BI2C_P_WriteReadNoAddr;
    }    

done:
    return( retCode );
}

BERR_Code BI2C_CloseI2cRegHandle(
    BREG_I2C_Handle     hI2cReg
    )
{
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ASSERT( hI2cReg );
    BKNI_Free( (void *) hI2cReg );

    return( retCode );
}

void BI2C_SetClk(
    BI2C_ChannelHandle  hChn,           /* Device channel handle */
    BI2C_Clk            clkRate         /* pointer to clock rate setting */
    )
{
    BI2C_Handle hDev;
    uint32_t    ctlReg;

    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hDev = hChn->hI2c;

    hChn->clkRate = clkRate;

#if EXTRA_SW_I2C_CHANNELS > 0
    if (hChn->chnNo >= BI2C_MAX_I2C_CHANNELS) {
        /* Sofware I2C channel. Nothing to do */
        return;
    }
#endif
    ctlReg = BREG_Read32(hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CTL_REG));
    ctlReg &= ~(BCHP_BSCA_CTL_REG_SCL_SEL_MASK | BCHP_BSCA_CTL_REG_DIV_CLK_MASK);
    switch (clkRate)
    {
        case BI2C_Clk_eClk100Khz:
        case BI2C_Clk_eClk400Khz:
            ctlReg |= (0x01 << BCHP_BSCA_CTL_REG_SCL_SEL_SHIFT);
            break;

        case BI2C_Clk_eClk47Khz:
        case BI2C_Clk_eClk187Khz:
            ctlReg |= (0x02 << BCHP_BSCA_CTL_REG_SCL_SEL_SHIFT);
            break;

        case BI2C_Clk_eClk50Khz:
        case BI2C_Clk_eClk200Khz:
            ctlReg |= (0x03 << BCHP_BSCA_CTL_REG_SCL_SEL_SHIFT);
            break;

        case BI2C_Clk_eClk93Khz:
        case BI2C_Clk_eClk375Khz:
            break;

    }
    if (clkRate < BI2C_Clk_eClk187Khz)
        ctlReg |= BCHP_BSCA_CTL_REG_DIV_CLK_MASK;

    BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CTL_REG), ctlReg );
}

#ifdef BI2C_USES_SET_SDA_DELAY
void BI2C_SetSdaDelay(
    BI2C_ChannelHandle  hChn,           /* Device channel handle */
    BI2C_SdaDelay       sdaDelay        /* Sda delay value */
    )
{
    BI2C_Handle hDev;
    uint32_t    ctlReg;

    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hDev = hChn->hI2c;

#if EXTRA_SW_I2C_CHANNELS > 0
    if (hChn->chnNo >= BI2C_MAX_I2C_CHANNELS) {
        /* Sofware I2C channel. Nothing to do */
        return;
    }
#endif
    ctlReg = BREG_Read32(hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CTL_REG));
    ctlReg &= ~(BCHP_BSCA_CTL_REG_reserved0_MASK | BCHP_BSCA_CTL_REG_SDA_DELAY_SEL_MASK);
    switch (sdaDelay)
    {
        /*
        case BI2C_eSdaDelay815ns:
            ctlReg |= (0x0 << BCHP_BSCA_CTL_REG_SDA_DELAY_SEL_SHIFT);
            break;
        */
        case BI2C_eSdaDelay370ns:
            ctlReg |= (0x1 << BCHP_BSCA_CTL_REG_SDA_DELAY_SEL_SHIFT);
            break;
        case BI2C_eSdaDelay482ns:
            ctlReg |= (0x2 << BCHP_BSCA_CTL_REG_SDA_DELAY_SEL_SHIFT);
            break;
        case BI2C_eSdaDelay593ns:
            ctlReg |= (0x3 << BCHP_BSCA_CTL_REG_SDA_DELAY_SEL_SHIFT);
            break;
        case BI2C_eSdaDelay704ns:
            ctlReg |= (0x4 << BCHP_BSCA_CTL_REG_SDA_DELAY_SEL_SHIFT);
            break;
        case BI2C_eSdaDelay815ns:
            ctlReg |= (0x5 << BCHP_BSCA_CTL_REG_SDA_DELAY_SEL_SHIFT);
            break;
        case BI2C_eSdaDelay926ns:
            ctlReg |= (0x6 << BCHP_BSCA_CTL_REG_SDA_DELAY_SEL_SHIFT);
            break;
        case BI2C_eSdaDelay1037ns:
            ctlReg |= (0x7 << BCHP_BSCA_CTL_REG_SDA_DELAY_SEL_SHIFT);
            break;
    }
    BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CTL_REG), ctlReg );
}
#endif

BI2C_Clk BI2C_GetClk(
    BI2C_ChannelHandle  hChn            /* Device channel handle */
)
{
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    /* TODO: how to handle the Single-Master/Multi-Channel configuration */
    return hChn->clkRate;
}

BERR_Code BI2C_SetBurstMode(
    BI2C_ChannelHandle hChn,    /* [in] I2C channel handle */
    bool bBurstMode             /* [out] Enable or Disable burst mode */
    )
{
    if (hChn == NULL)
    {
        BDBG_ERR(("Invalid I2C channel handle"));
        return BERR_INVALID_PARAMETER;
    }

    hChn->burstMode = bBurstMode;

    return BERR_SUCCESS;
}

BERR_Code BI2C_GetBurstMode(
    BI2C_ChannelHandle hChn,    /* [in] I2C channel handle */
    bool *pbBurstMode           /* [out] current burst mode? */
    )
{
    if ((hChn == NULL) || (pbBurstMode == NULL))
    {
        BDBG_ERR(("Invalid parameters\n"));
        return BERR_INVALID_PARAMETER;
    }

    *pbBurstMode = hChn->burstMode;

    return BERR_SUCCESS;
}

#ifdef BI2C_SUPPORT_4_BYTE_XFR_MODE
void BI2C_Set4ByteXfrMode(
    BI2C_ChannelHandle  hChn,           /* Device channel handle */
    bool b4ByteMode                     /* Enable or Disable 4 byte xfr mode */
    )
{
    BI2C_Handle hDev;
    uint32_t    ctlReg;

    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hDev = hChn->hI2c;

    ctlReg = BREG_Read32(hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CTLHI_REG));
    if (b4ByteMode)
    {
        ctlReg |= BCHP_BSCA_CTLHI_REG_DATA_REG_SIZE_MASK;
    }
    else
    {
        ctlReg &= ~BCHP_BSCA_CTLHI_REG_DATA_REG_SIZE_MASK;
    }

    BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CTLHI_REG), ctlReg );

}

bool BI2C_Is4ByteXfrMode(
    BI2C_ChannelHandle  hChn           /* Device channel handle */
    )
{
    BI2C_Handle hDev;
    uint32_t    ctlReg;

    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    hDev = hChn->hI2c;

    ctlReg = BREG_Read32(hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CTLHI_REG));
    if (ctlReg & BCHP_BSCA_CTLHI_REG_DATA_REG_SIZE_MASK)
    {
        return true;
    }

    return false;
}
#endif

/*******************************************************************************
*
*   Private Module Functions
*
*******************************************************************************/
BERR_Code BI2C_P_Read
(
    void *context,              /* Device channel handle */
    uint16_t chipAddr,                  /* chip address */
    uint32_t subAddr,                    /* 8-bit sub address */
    uint8_t *pData,             /* pointer to memory location to store read data  */
    size_t length                       /* number of bytes to read */
)
{
    BI2C_ChannelHandle hChn;

    if (context == NULL)
    {
        BDBG_ERR(("Invalid I2C handle\n"));
        return BERR_INVALID_PARAMETER;
    }

    hChn = (BI2C_ChannelHandle)context;

#ifdef BI2C_SUPPORT_4_BYTE_XFR_MODE
    if (BI2C_Is4ByteXfrMode(hChn) == true)
        return BI2C_P_ReadBy4BytesCmd ((BI2C_ChannelHandle)context, chipAddr, subAddr, 1, pData, length, true /*mutex*/, true /*ack*/, false /*no stop*/);
    else
#else
    if (hChn->burstMode)
        return BI2C_P_BurstReadCmd ((BI2C_ChannelHandle)context, chipAddr, subAddr, 1, pData, length);
    else
#endif
        return BI2C_P_ReadCmd ((BI2C_ChannelHandle)context, chipAddr, subAddr, 1, pData, length, true /*mutex*/, true /*ack*/, false /*no stop*/);
}

BERR_Code BI2C_P_ReadSw
(
    void *context,              /* Device channel handle */
    uint16_t chipAddr,          /* chip address */
    uint32_t subAddr,           /* 8-bit sub address */
    uint8_t *pData,             /* pointer to memory location to store read data  */
    size_t length               /* number of bytes to read */
)
{
    return BI2C_P_ReadSwCmd ((BI2C_ChannelHandle)context, chipAddr, subAddr, 1, pData, length, false /*EDDC*/, 0, true /*ack*/);
}

BERR_Code BI2C_P_ReadNoAck
(
    void *context,              /* Device channel handle */
    uint16_t chipAddr,                  /* chip address */
    uint32_t subAddr,                    /* 8-bit sub address */
    uint8_t *pData,             /* pointer to memory location to store read data  */
    size_t length                       /* number of bytes to read */
)
{
    BI2C_ChannelHandle hChn;

    if (context == NULL)
    {
        BDBG_ERR(("Invalid I2C handle\n"));
        return BERR_INVALID_PARAMETER;
    }

    hChn = (BI2C_ChannelHandle)context;

#ifdef BI2C_SUPPORT_4_BYTE_XFR_MODE
    if (BI2C_Is4ByteXfrMode(hChn) == true)
        return BI2C_P_ReadBy4BytesCmd ((BI2C_ChannelHandle)context, chipAddr, subAddr, 1, pData, length, true /*mutex*/, false /*ack*/, false /*no stop*/);
    else
#else
    if (hChn->burstMode)
        return BI2C_P_BurstReadCmdNoAck ((BI2C_ChannelHandle)context, chipAddr, subAddr, 1, pData, length);
    else
#endif    
        return BI2C_P_ReadCmd ((BI2C_ChannelHandle)context, chipAddr, subAddr, 1, pData, length, true /*mutex*/, false /*ack*/, false /*no stop*/);
}

BERR_Code BI2C_P_ReadSwNoAck
(
    void *context,              /* Device channel handle */
    uint16_t chipAddr,          /* chip address */
    uint32_t subAddr,           /* 8-bit sub address */
    uint8_t *pData,             /* pointer to memory location to store read data  */
    size_t length               /* number of bytes to read */
)
{
    if (context == NULL)
    {
        BDBG_ERR(("Invalid I2C handle\n"));
        return BERR_INVALID_PARAMETER;
    }

    return BI2C_P_ReadSwCmd ((BI2C_ChannelHandle)context, chipAddr, subAddr, 1, pData, length, false /*EDDC*/, 0, false /*ack*/);
}

BERR_Code BI2C_P_ReadA16
(
    void *context,              /* Device channel handle */
    uint16_t chipAddr,                  /* chip address */
    uint32_t subAddr,                   /* 16-bit sub address */
    uint8_t *pData,             /* pointer to memory location to store read data  */
    size_t length                       /* number of bytes to read */
)
{
    BI2C_ChannelHandle hChn;

    if (context == NULL)
    {
        BDBG_ERR(("Invalid I2C handle\n"));
        return BERR_INVALID_PARAMETER;
    }

    hChn = (BI2C_ChannelHandle)context;

#ifdef BI2C_SUPPORT_4_BYTE_XFR_MODE
    if (BI2C_Is4ByteXfrMode(hChn) == true)
        return BI2C_P_ReadBy4BytesCmd ((BI2C_ChannelHandle)context, chipAddr, subAddr, 2, pData, length, true /*mutex*/, true /*ack*/, false /*no stop*/);
    else
#else
    if (hChn->burstMode)
        return BI2C_P_BurstReadCmd ((BI2C_ChannelHandle)context, chipAddr, subAddr, 2, pData, length);
    else
#endif    
        return BI2C_P_ReadCmd ((BI2C_ChannelHandle)context, chipAddr, subAddr, 2, pData, length, true /*mutex*/, true /*ack*/, false /*no stop*/);
}

BERR_Code BI2C_P_ReadSwA16
(
    void *context,              /* Device channel handle */
    uint16_t chipAddr,                  /* chip address */
    uint32_t subAddr,                   /* 16-bit sub address */
    uint8_t *pData,             /* pointer to memory location to store read data  */
    size_t length                       /* number of bytes to read */
)
{
    BI2C_ChannelHandle hChn;

    if (context == NULL)
    {
        BDBG_ERR(("Invalid I2C handle\n"));
        return BERR_INVALID_PARAMETER;
    }

    hChn = (BI2C_ChannelHandle)context;

    return BI2C_P_ReadSwCmd ((BI2C_ChannelHandle)context, chipAddr, subAddr, 2, pData, length, false /*EDDC*/, 0, true /*ack*/);
}

BERR_Code BI2C_P_ReadA24
(
    void *context,              /* Device channel handle */
    uint16_t chipAddr,                  /* chip address */
    uint32_t subAddr,                   /* 24-bit sub address */
    uint8_t *pData,             /* pointer to memory location to store read data  */
    size_t length                       /* number of bytes to read */
)
{
    BI2C_ChannelHandle hChn;

    if (context == NULL)
    {
        BDBG_ERR(("Invalid I2C handle\n"));
        return BERR_INVALID_PARAMETER;
    }

    hChn = (BI2C_ChannelHandle)context;

#ifdef BI2C_SUPPORT_4_BYTE_XFR_MODE
    if (BI2C_Is4ByteXfrMode(hChn) == true)
        return BI2C_P_ReadBy4BytesCmd ((BI2C_ChannelHandle)context, chipAddr, subAddr, 3, pData, length, true /*mutex*/, true /*ack*/, false /*no stop*/);
    else
#else
    if (hChn->burstMode)
        return BI2C_P_BurstReadCmd ((BI2C_ChannelHandle)context, chipAddr, subAddr, 3, pData, length);
    else
#endif    
        return BI2C_P_ReadCmd ((BI2C_ChannelHandle)context, chipAddr, subAddr, 3, pData, length, true /*mutex*/, true /*ack*/, false /*no stop*/);
}

BERR_Code BI2C_P_ReadSwA24
(
    void *context,              /* Device channel handle */
    uint16_t chipAddr,                  /* chip address */
    uint32_t subAddr,                   /* 24-bit sub address */
    uint8_t *pData,             /* pointer to memory location to store read data  */
    size_t length                       /* number of bytes to read */
)
{
    BI2C_ChannelHandle hChn;

    if (context == NULL)
    {
        BDBG_ERR(("Invalid I2C handle\n"));
        return BERR_INVALID_PARAMETER;
    }

    hChn = (BI2C_ChannelHandle)context;

    return BI2C_P_ReadSwCmd ((BI2C_ChannelHandle)context, chipAddr, subAddr, 3, pData, length, false /*EDDC*/, 0, true /*ack*/);
}

BERR_Code BI2C_P_ReadNoAddr
(
    void *context,              /* Device channel handle */
    uint16_t chipAddr,                  /* chip address */
    uint8_t *pData,             /* pointer to memory location to store read data  */
    size_t length                       /* number of bytes to read */
)
{
    BI2C_ChannelHandle hChn;

    if (context == NULL)
    {
        BDBG_ERR(("Invalid I2C handle\n"));
        return BERR_INVALID_PARAMETER;
    }

    hChn = (BI2C_ChannelHandle)context;

#ifdef BI2C_SUPPORT_4_BYTE_XFR_MODE
    if (BI2C_Is4ByteXfrMode(hChn) == true)
        return BI2C_P_ReadBy4BytesCmd ((BI2C_ChannelHandle)context, chipAddr, 0, 0, pData, length, true /*mutex*/, true /*ack*/, false /*no stop*/);
    else
#else
    if (hChn->burstMode)
        return BI2C_P_BurstReadCmd ((BI2C_ChannelHandle)context, chipAddr, 0, 0, pData, length);
    else
#endif    
        return BI2C_P_ReadCmd ((BI2C_ChannelHandle)context, chipAddr, 0, 0, pData, length, true /*mutex*/, true /*ack*/, false /*no stop*/);
}

BERR_Code BI2C_P_ReadSwNoAddr
(
    void *context,              /* Device channel handle */
    uint16_t chipAddr,                  /* chip address */
    uint8_t *pData,             /* pointer to memory location to store read data  */
    size_t length                       /* number of bytes to read */
)
{
    BI2C_ChannelHandle hChn;

    if (context == NULL)
    {
        BDBG_ERR(("Invalid I2C handle\n"));
        return BERR_INVALID_PARAMETER;
    }

    hChn = (BI2C_ChannelHandle)context;

    return BI2C_P_ReadSwCmd ((BI2C_ChannelHandle)context, chipAddr, 0, 0, pData, length, false /*EDDC*/, 0, true /*ack*/);
}

BERR_Code BI2C_P_ReadNoAddrNoAck
(
    void *context,              /* Device channel handle */
    uint16_t chipAddr,                  /* chip address */
    uint8_t *pData,             /* pointer to memory location to store read data  */
    size_t length                       /* number of bytes to read */
)
{
    BI2C_ChannelHandle hChn;

    if (context == NULL)
    {
        BDBG_ERR(("Invalid I2C handle\n"));
        return BERR_INVALID_PARAMETER;
    }

    hChn = (BI2C_ChannelHandle)context;

#ifdef BI2C_SUPPORT_4_BYTE_XFR_MODE
    if (BI2C_Is4ByteXfrMode(hChn) == true)
        return BI2C_P_ReadBy4BytesCmd ((BI2C_ChannelHandle)context, chipAddr, 0, 0, pData, length, true /*mutex*/, false /*ack*/, false /*no stop*/);
    else
#else
    if (hChn->burstMode)
        return BI2C_P_BurstReadCmdNoAck ((BI2C_ChannelHandle)context, chipAddr, 0, 0, pData, length);
    else
#endif    
        return BI2C_P_ReadCmd ((BI2C_ChannelHandle)context, chipAddr, 0, 0, pData, length, true /*mutex*/, false /*ack*/, false /*no stop*/);
}

BERR_Code BI2C_P_ReadSwNoAddrNoAck
(
    void *context,              /* Device channel handle */
    uint16_t chipAddr,                  /* chip address */
    uint8_t *pData,             /* pointer to memory location to store read data  */
    size_t length                       /* number of bytes to read */
)
{
    if (context == NULL)
    {
        BDBG_ERR(("Invalid I2C handle\n"));
        return BERR_INVALID_PARAMETER;
    }

    return BI2C_P_ReadSwCmd ((BI2C_ChannelHandle)context, chipAddr, 0, 0, pData, length, false /*EDDC*/, 0, false /*ack*/);
}

BERR_Code BI2C_P_WriteRead
(
    void * context, 
    uint16_t chipAddr, 
    uint32_t subAddr, 
    const uint8_t *pWriteData, 
    size_t writeLength, 
    uint8_t *pReadData, 
    size_t readLength 
)
{
    BI2C_Handle hDev;
    BERR_Code err;

    BI2C_ChannelHandle  hChn = (BI2C_ChannelHandle)context;           /* Device channel handle */

    /* Get I2C handle from channel handle */
    hDev = hChn->hI2c;
    
    BI2C_P_ACQUIRE_MUTEX( hChn );
    
#ifdef BI2C_SUPPORT_4_BYTE_XFR_MODE
    if (BI2C_Is4ByteXfrMode((BI2C_ChannelHandle)context) == true)
    {
        err = BI2C_P_WriteBy4BytesCmd ((BI2C_ChannelHandle)context, chipAddr, subAddr, 1, pWriteData, writeLength, false /*NVRAM*/, false /*mutex*/, true /*ack*/, true /*no stop*/);
        if (err == BERR_SUCCESS)
            err = BI2C_P_ReadBy4BytesCmd ((BI2C_ChannelHandle)context, chipAddr, subAddr, 1, pReadData, readLength, false /*mutex*/, true /*ack*/, false /*no stop*/);
        else
            goto done;
    }
    else
#endif
    {
        err = BI2C_P_WriteCmd ((BI2C_ChannelHandle)context, chipAddr, subAddr, 1, pWriteData, writeLength, false /*NVRAM*/, false /*mutex*/, true /*ack*/, true /*no stop*/);
        if (err == BERR_SUCCESS)
            err = BI2C_P_ReadCmd ((BI2C_ChannelHandle)context, chipAddr, subAddr, 1, pReadData, readLength, false /*mutex*/, true /*ack*/, false /*no stop*/);
        else
            goto done;
    }
    
done:
    BI2C_P_RELEASE_MUTEX( hChn );
    return err;
}

BERR_Code BI2C_P_WriteReadNoAddr
(
    void * context, 
    uint16_t chipAddr, 
    const uint8_t *pWriteData, 
    size_t writeLength, 
    uint8_t *pReadData, 
    size_t readLength 
)
{
    BI2C_Handle hDev;
    BERR_Code err;

    BI2C_ChannelHandle  hChn = (BI2C_ChannelHandle)context;           /* Device channel handle */

    /* Get I2C handle from channel handle */
    hDev = hChn->hI2c;
    
    BI2C_P_ACQUIRE_MUTEX( hChn );
    
#ifdef BI2C_SUPPORT_4_BYTE_XFR_MODE
    if (BI2C_Is4ByteXfrMode((BI2C_ChannelHandle)context) == true)
    {
        err = BI2C_P_WriteBy4BytesCmd ((BI2C_ChannelHandle)context, chipAddr, 0, 0, pWriteData, writeLength, false /*NVRAM*/, true /*mutex*/, true /*ack*/, true /*no stop*/);
        if (err == BERR_SUCCESS)
            err =  BI2C_P_ReadBy4BytesCmd ((BI2C_ChannelHandle)context, chipAddr, 0, 0, pReadData, readLength, true /*mutex*/, true /*ack*/, false /*no stop*/);
        else
            goto done;
    }
    else
#endif
    {
        err = BI2C_P_WriteCmd ((BI2C_ChannelHandle)context, chipAddr, 0, 0, pWriteData, writeLength, false /*NVRAM*/, true /*mutex*/, true /*ack*/, true /*no stop*/);
        if (err == BERR_SUCCESS)
            err = BI2C_P_ReadCmd ((BI2C_ChannelHandle)context, chipAddr, 0, 0, pReadData, readLength, true /*mutex*/, true /*ack*/, false /*no stop*/);
        else
            goto done;
    }

done:
    BI2C_P_RELEASE_MUTEX( hChn );
    return err;
}

BERR_Code BI2C_P_Write
(
    void *context,              /* Device channel handle */
    uint16_t chipAddr,                  /* chip address */
    uint32_t subAddr,                    /* 8-bit sub address */
    const uint8_t *pData,               /* pointer to data to write */
    size_t length                       /* number of bytes to write */
)
{
#ifdef BI2C_SUPPORT_4_BYTE_XFR_MODE
    if (BI2C_Is4ByteXfrMode((BI2C_ChannelHandle)context) == true)
        return BI2C_P_WriteBy4BytesCmd ((BI2C_ChannelHandle)context, chipAddr, subAddr, 1, pData, length, false /*NVRAM*/, true /*mutex*/, true /*ack*/, false /*no stop*/);
    else
#endif    
        return BI2C_P_WriteCmd ((BI2C_ChannelHandle)context, chipAddr, subAddr, 1, pData, length, false /*NVRAM*/, true /*mutex*/, true /*ack*/, false /*no stop*/);
}

BERR_Code BI2C_P_WriteSw
(
    void *context,              /* Device channel handle */
    uint16_t chipAddr,                  /* chip address */
    uint32_t subAddr,                    /* 8-bit sub address */
    const uint8_t *pData,               /* pointer to data to write */
    size_t length                       /* number of bytes to write */
)
{
    return BI2C_P_WriteSwCmd ((BI2C_ChannelHandle)context, chipAddr, subAddr, 1, pData, length, false /*NVRAM*/);
}

BERR_Code BI2C_P_WriteNoAck
(
    void *context,              /* Device channel handle */
    uint16_t chipAddr,                  /* chip address */
    uint32_t subAddr,                    /* 8-bit sub address */
    const uint8_t *pData,               /* pointer to data to write */
    size_t length                       /* number of bytes to write */
)
{
#ifdef BI2C_SUPPORT_4_BYTE_XFR_MODE
    if (BI2C_Is4ByteXfrMode((BI2C_ChannelHandle)context) == true)
        return BI2C_P_WriteBy4BytesCmd ((BI2C_ChannelHandle)context, chipAddr, subAddr, 1, pData, length, false /*NVRAM*/, true /*mutex*/, false /*ack*/, false /*no stop*/);
    else
#endif
        return BI2C_P_WriteCmd ((BI2C_ChannelHandle)context, chipAddr, subAddr, 1, pData, length, false /*NVRAM*/, true /*mutex*/, false /*ack*/, false /*no stop*/);
}

BERR_Code BI2C_P_WriteA16
(
    void *context,              /* Device channel handle */
    uint16_t chipAddr,                  /* chip address */
    uint32_t subAddr,                   /* 16-bit sub address */
    const uint8_t *pData,               /* pointer to data to write */
    size_t length                       /* number of bytes to write */
)
{
#ifdef BI2C_SUPPORT_4_BYTE_XFR_MODE
    if (BI2C_Is4ByteXfrMode((BI2C_ChannelHandle)context) == true)
        return BI2C_P_WriteBy4BytesCmd ((BI2C_ChannelHandle)context, chipAddr, subAddr, 2, pData, length, false /*NVRAM*/, true /*mutex*/, true /*ack*/, false /*no stop*/);
    else
#endif    
        return BI2C_P_WriteCmd ((BI2C_ChannelHandle)context, chipAddr, subAddr, 2, pData, length, false /*NVRAM*/, true /*mutex*/, true /*ack*/, false /*no stop*/);
}

BERR_Code BI2C_P_WriteSwA16
(
    void *context,              /* Device channel handle */
    uint16_t chipAddr,                  /* chip address */
    uint32_t subAddr,                   /* 16-bit sub address */
    const uint8_t *pData,               /* pointer to data to write */
    size_t length                       /* number of bytes to write */
)
{
    return BI2C_P_WriteSwCmd ((BI2C_ChannelHandle)context, chipAddr, subAddr, 2, pData, length, false /*NVRAM*/);
}

BERR_Code BI2C_P_WriteA24
(
    void *context,              /* Device channel handle */
    uint16_t chipAddr,                  /* chip address */
    uint32_t subAddr,                   /* 24-bit sub address */
    const uint8_t *pData,               /* pointer to data to write */
    size_t length                       /* number of bytes to write */
)
{
#ifdef BI2C_SUPPORT_4_BYTE_XFR_MODE
    if (BI2C_Is4ByteXfrMode((BI2C_ChannelHandle)context) == true)
        return BI2C_P_WriteBy4BytesCmd ((BI2C_ChannelHandle)context, chipAddr, subAddr, 3, pData, length, false /*NVRAM*/, true /*mutex*/, true /*ack*/, false /*no stop*/);
    else
#endif
        return BI2C_P_WriteCmd ((BI2C_ChannelHandle)context, chipAddr, subAddr, 3, pData, length, false /*NVRAM*/, true /*mutex*/, true /*ack*/, false /*no stop*/);
}

BERR_Code BI2C_P_WriteSwA24
(
    void *context,              /* Device channel handle */
    uint16_t chipAddr,                  /* chip address */
    uint32_t subAddr,                   /* 24-bit sub address */
    const uint8_t *pData,               /* pointer to data to write */
    size_t length                       /* number of bytes to write */
)
{
    return BI2C_P_WriteSwCmd ((BI2C_ChannelHandle)context, chipAddr, subAddr, 3, pData, length, false /*NVRAM*/);
}

BERR_Code BI2C_P_WriteNoAddr
(
    void *context,              /* Device channel handle */
    uint16_t chipAddr,                  /* chip address */
    const uint8_t *pData,               /* pointer to data to write */
    size_t length                       /* number of bytes to write */
)
{
#ifdef BI2C_SUPPORT_4_BYTE_XFR_MODE
    if (BI2C_Is4ByteXfrMode((BI2C_ChannelHandle)context) == true)
        return BI2C_P_WriteBy4BytesCmd ((BI2C_ChannelHandle)context, chipAddr, 0, 0, pData, length, false /*NVRAM*/, true /*mutex*/, true /*ack*/, false /*no stop*/);
    else
#endif    
        return BI2C_P_WriteCmd ((BI2C_ChannelHandle)context, chipAddr, 0, 0, pData, length, false /*NVRAM*/, true /*mutex*/, false /*ack*/, false /*no stop*/);
}

BERR_Code BI2C_P_WriteNoAddrNoAck
(
    void *context,              /* Device channel handle */
    uint16_t chipAddr,                  /* chip address */
    const uint8_t *pData,               /* pointer to data to write */
    size_t length                       /* number of bytes to write */
)
{
#ifdef BI2C_SUPPORT_4_BYTE_XFR_MODE
    if (BI2C_Is4ByteXfrMode((BI2C_ChannelHandle)context) == true)
        return BI2C_P_WriteBy4BytesCmd ((BI2C_ChannelHandle)context, chipAddr, 0, 0, pData, length, false /*NVRAM*/, true /*mutex*/, false /*ack*/, false /*no stop*/);
    else
#endif    
        return BI2C_P_WriteCmd ((BI2C_ChannelHandle)context, chipAddr, 0, 0, pData, length, false /*NVRAM*/, true /*mutex*/, false /*ack*/, false /*no stop*/);
}

BERR_Code BI2C_P_WriteNvram
(
    void *context,              /* Device channel handle */
    uint16_t chipAddr,                  /* chip address */
    uint32_t subAddr,                    /* 8-bit sub address */
    const uint8_t *pData,               /* pointer to data to write */
    size_t length                       /* number of bytes to write */
)
{
#ifdef BI2C_SUPPORT_4_BYTE_XFR_MODE
    if (BI2C_Is4ByteXfrMode((BI2C_ChannelHandle)context) == true)
        return BI2C_P_WriteBy4BytesCmd ((BI2C_ChannelHandle)context, chipAddr, subAddr, 1, pData, length, true /*NVRAM*/, true /*mutex*/, true /*ack*/, false /*no stop*/);
    else
#endif    
        return BI2C_P_WriteCmd ((BI2C_ChannelHandle)context, chipAddr, subAddr, 1, pData, length, true /*NVRAM*/, true /*mutex*/, true /*ack*/, false /*no stop*/);
}

BERR_Code BI2C_P_ReadEDDC(
    void                *context,               /* Device channel handle */
    uint8_t             chipAddr,               /* chip address */
    uint8_t             segment,                /* EDDC segment */
    uint32_t            subAddr,                /* 8-bit sub address */
    uint8_t             *pData,                 /* pointer to memory location to store read data */
    size_t              length                  /* number of bytes to read */
    )
{
    BERR_Code           retCode = BERR_SUCCESS;
    BI2C_Handle         hDev;
    BI2C_ChannelHandle  hChn = (BI2C_ChannelHandle)context;
    uint32_t            lval;

    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    /* Get I2C handle from channel handle */
    hDev = hChn->hI2c;

    BI2C_P_ACQUIRE_MUTEX( hChn );

    /***********************************
     * Step 1: Write the segment pointer
     **********************************/
    /* program slave device's (id) */
    BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CHIP_ADDRESS), (uint32_t)EDDC_SEGMENT_CHIP_ADDR );

    /* write segment pointer into fifo */
    BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_DATA_IN0), (uint32_t)segment );

    /* program amount of bytes to write/read */
    BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CNT_REG), 1 );

    /* program data transfer type */
    lval = BREG_Read32(hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CTL_REG));
    lval &= ~(BCHP_BSCA_CTL_REG_reserved0_MASK | BCHP_BSCA_CTL_REG_DTF_MASK);
    lval |= BI2C_P_eWriteOnly ;
    BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CTL_REG), lval );

    /* ignore ack */
    lval = BREG_Read32(hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CTLHI_REG));
    lval |= BCHP_BSCA_CTLHI_REG_IGNORE_ACK_MASK ;
    BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CTLHI_REG), lval );

    /* start the write command */
    lval = (BCHP_BSCA_IIC_ENABLE_RESTART_MASK | BCHP_BSCA_IIC_ENABLE_NO_STOP_MASK |
            BCHP_BSCA_IIC_ENABLE_ENABLE_MASK);
    BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_IIC_ENABLE), lval );

    retCode = BI2C_P_WaitForCompletion(hChn, 1);
    if (retCode != BERR_SUCCESS)
        goto done;

    /* get rid of ignore ack */
    lval = BREG_Read32(hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CTLHI_REG));
    lval &= ~BCHP_BSCA_CTLHI_REG_IGNORE_ACK_MASK ;
    BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CTLHI_REG), lval );

    /****************************************
     * Step 2: Read the data with sub address
     ***************************************/

#ifdef BI2C_SUPPORT_4_BYTE_XFR_MODE
    if (BI2C_Is4ByteXfrMode(hChn) == true)
        retCode = BI2C_P_ReadBy4BytesCmd (hChn, chipAddr, subAddr, 1, pData, length, false /*mutex*/, true /*ack*/, false /*no stop*/);
    else
#endif
        retCode = BI2C_P_ReadCmd (hChn, chipAddr, subAddr, 1, pData, length, false /*mutex*/, true /*ack*/, false /*no stop*/);

done:
    BI2C_P_RELEASE_MUTEX( hChn );
    return( retCode );
}

BERR_Code BI2C_P_ReadSwEDDC(
    void                *context,               /* Device channel handle */
    uint8_t             chipAddr,               /* chip address */
    uint8_t             segment,                /* EDDC segment */
    uint32_t             subAddr,                /* 8-bit sub address */
    uint8_t             *pData,                 /* pointer to memory location to store read data */
    size_t              length                  /* number of bytes to read */
    )
{
    BERR_Code           retCode = BERR_SUCCESS;
    BI2C_ChannelHandle  hChn = (BI2C_ChannelHandle)context;

    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    retCode = BI2C_P_ReadSwCmd (hChn, chipAddr, subAddr, 1, pData, length, true /*EDDC*/, segment, true /*ack*/);

    return( retCode );
}

BERR_Code BI2C_P_WriteEDDC(
    void                *context,       /* Device channel handle */
    uint8_t             chipAddr,       /* chip address */
    uint8_t             segment,        /* EDDC segment */
    uint32_t             subAddr,        /* 8-bit sub address */
    const uint8_t       *pData,         /* pointer to data to write */
    size_t              length          /* number of bytes to read */
    )
{
    BERR_Code           retCode = BERR_SUCCESS;
    BI2C_Handle         hDev;
    BI2C_ChannelHandle  hChn = (BI2C_ChannelHandle)context;
    uint32_t            lval;

    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    /* Get I2C handle from channel handle */
    hDev = hChn->hI2c;

    BI2C_P_ACQUIRE_MUTEX( hChn );

    /***********************************
     * Step 1: Write the segment pointer
     **********************************/
    /* program slave device's (id) */
    BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CHIP_ADDRESS), (uint32_t)EDDC_SEGMENT_CHIP_ADDR );

    /* write segment pointer into fifo */
    BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_DATA_IN0), (uint32_t)segment );

    /* program amount of bytes to write/read */
    BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CNT_REG), 1 );

    /* program data transfer type */
    lval = BREG_Read32(hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CTL_REG));
    lval &= ~(BCHP_BSCA_CTL_REG_reserved0_MASK | BCHP_BSCA_CTL_REG_DTF_MASK);
    lval |= BI2C_P_eWriteOnly ;
    BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CTL_REG), lval );

    /* ignore ack */
    lval = BREG_Read32(hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CTLHI_REG));
    lval |= BCHP_BSCA_CTLHI_REG_IGNORE_ACK_MASK ;
    BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CTLHI_REG), lval );

    /* start the write command */
    lval = (BCHP_BSCA_IIC_ENABLE_RESTART_MASK | BCHP_BSCA_IIC_ENABLE_NO_STOP_MASK |
            BCHP_BSCA_IIC_ENABLE_ENABLE_MASK);
    BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_IIC_ENABLE), lval );

    retCode = BI2C_P_WaitForCompletion(hChn, 1);
    if (retCode != BERR_SUCCESS)
        goto done;

    /* get rid of ignore ack */
    lval = BREG_Read32(hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CTLHI_REG));
    lval &= ~BCHP_BSCA_CTLHI_REG_IGNORE_ACK_MASK ;
    BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CTLHI_REG), lval );

    /****************************************
     * Step 2: Write the data with sub address
     ***************************************/

#ifdef BI2C_SUPPORT_4_BYTE_XFR_MODE
    if (BI2C_Is4ByteXfrMode((BI2C_ChannelHandle)context) == true)
        retCode = BI2C_P_WriteBy4BytesCmd (hChn, chipAddr, subAddr, 1, pData, length, false /*NVRAM*/, false /*mutex*/, true /*ack*/, false /*no stop*/);
    else
#endif
    retCode = BI2C_P_WriteCmd (hChn, chipAddr, subAddr, 1, pData, length, false /*NVRAM*/, false /*mutex*/, true /*ack*/, false /*no stop*/);

done:
    BI2C_P_RELEASE_MUTEX( hChn );
    return( retCode );
}

#define I2C_MUX_CTRL_SCL(ch, sun_top, sgpio) \
    i2c_gpio[ch].sun_top_scl_reg = sun_top; \
    i2c_gpio[ch].sun_top_scl_shift = sun_top##_##sgpio##_SHIFT; \
    i2c_gpio[ch].sun_top_scl_mask = sun_top##_##sgpio##_MASK;

#define I2C_MUX_CTRL_SDA(ch, sun_top, sgpio) \
    i2c_gpio[ch].sun_top_sda_reg = sun_top; \
    i2c_gpio[ch].sun_top_sda_shift = sun_top##_##sgpio##_SHIFT; \
    i2c_gpio[ch].sun_top_sda_mask = sun_top##_##sgpio##_MASK;

#define I2C_GPIO(ext_or_hi) \
    { \
        int i; \
        for (i=0; i<BI2C_MAX_I2C_CHANNELS; i++) \
        { \
            i2c_gpio[i].gio_data_scl_reg = BCHP_GIO_DATA_##ext_or_hi; \
            i2c_gpio[i].gio_data_sda_reg = BCHP_GIO_DATA_##ext_or_hi; \
            i2c_gpio[i].gio_iodir_scl_reg = BCHP_GIO_IODIR_##ext_or_hi; \
            i2c_gpio[i].gio_iodir_sda_reg = BCHP_GIO_IODIR_##ext_or_hi; \
            i2c_gpio[i].gio_oden_scl_reg = BCHP_GIO_ODEN_##ext_or_hi; \
            i2c_gpio[i].gio_oden_sda_reg = BCHP_GIO_ODEN_##ext_or_hi; \
            i2c_gpio[i].gio_scl_mask = (1 << i*2); \
            i2c_gpio[i].gio_sda_mask = (1 << (i*2+1)); \
        } \
    }

#define I2C_GPIO_AON_SCL_LO(ch, gpio) \
    i2c_gpio[ch].gio_data_scl_reg = BCHP_GIO_AON_DATA_LO; \
    i2c_gpio[ch].gio_iodir_scl_reg = BCHP_GIO_AON_IODIR_LO; \
    i2c_gpio[ch].gio_oden_scl_reg = BCHP_GIO_AON_ODEN_LO; \
    i2c_gpio[ch].gio_scl_mask = (1 << gpio);

#define I2C_GPIO_AON_SDA_LO(ch, gpio) \
    i2c_gpio[ch].gio_data_sda_reg = BCHP_GIO_AON_DATA_LO; \
    i2c_gpio[ch].gio_iodir_sda_reg = BCHP_GIO_AON_IODIR_LO; \
    i2c_gpio[ch].gio_oden_sda_reg = BCHP_GIO_AON_ODEN_LO; \
    i2c_gpio[ch].gio_sda_mask = (1 << gpio);

#define I2C_GPIO_AON_SCL_EXT(ch, gpio) \
    i2c_gpio[ch].gio_data_scl_reg = BCHP_GIO_AON_DATA_EXT; \
    i2c_gpio[ch].gio_iodir_scl_reg = BCHP_GIO_AON_IODIR_EXT; \
    i2c_gpio[ch].gio_oden_scl_reg = BCHP_GIO_AON_ODEN_EXT; \
    i2c_gpio[ch].gio_scl_mask = (1 << gpio);

#define I2C_GPIO_AON_SDA_EXT(ch, gpio) \
    i2c_gpio[ch].gio_data_sda_reg = BCHP_GIO_AON_DATA_EXT; \
    i2c_gpio[ch].gio_iodir_sda_reg = BCHP_GIO_AON_IODIR_EXT; \
    i2c_gpio[ch].gio_oden_sda_reg = BCHP_GIO_AON_ODEN_EXT; \
    i2c_gpio[ch].gio_sda_mask = (1 << gpio);

#define I2C_GPIO_SCL_EXT(ch, gpio) \
    i2c_gpio[ch].gio_data_scl_reg = BCHP_GIO_DATA_EXT; \
    i2c_gpio[ch].gio_iodir_scl_reg = BCHP_GIO_IODIR_EXT; \
    i2c_gpio[ch].gio_oden_scl_reg = BCHP_GIO_ODEN_EXT; \
    i2c_gpio[ch].gio_scl_mask = (1 << gpio);

#define I2C_GPIO_SDA_EXT(ch, gpio) \
    i2c_gpio[ch].gio_data_sda_reg = BCHP_GIO_DATA_EXT; \
    i2c_gpio[ch].gio_iodir_sda_reg = BCHP_GIO_IODIR_EXT; \
    i2c_gpio[ch].gio_oden_sda_reg = BCHP_GIO_ODEN_EXT; \
    i2c_gpio[ch].gio_sda_mask = (1 << gpio);

#define I2C_GPIO_SCL_LO(ch, gpio) \
    i2c_gpio[ch].gio_data_scl_reg = BCHP_GIO_DATA_LO; \
    i2c_gpio[ch].gio_iodir_scl_reg = BCHP_GIO_IODIR_LO; \
    i2c_gpio[ch].gio_oden_scl_reg = BCHP_GIO_ODEN_LO; \
    i2c_gpio[ch].gio_scl_mask = (1 << gpio);

#define I2C_GPIO_SDA_LO(ch, gpio) \
    i2c_gpio[ch].gio_data_sda_reg = BCHP_GIO_DATA_LO; \
    i2c_gpio[ch].gio_iodir_sda_reg = BCHP_GIO_IODIR_LO; \
    i2c_gpio[ch].gio_oden_sda_reg = BCHP_GIO_ODEN_LO; \
    i2c_gpio[ch].gio_sda_mask = (1 << gpio);

/***********************************************************************func
 * Name: BI2C_P_GpioInit
 *   -
 *
 * NOTE:
 ***************************************************************************/
void BI2C_P_GpioInit(void)
{
    #if (BI2C_SUPPORT_I2C_VIA_GPIO==1)
    /*
     * Ideally, this is the only section of code which needs to be modified in
     * order to support I2C via GPIO for a particular chip.  What we are doing
     * is mapping GPIO's to an i2c channel's SDA and SCL control lines.
     * Typically, it always involves SGPIO and:
     * SGPIO 0 maps to channel 0 SCL
     * SGPIO 1 maps to channel 0 SDA
     * SGPIO 2 maps to channel 1 SCL
     * SGPIO 3 maps to channel 1 SDA
     * etc
     * etc
     *
     * The only thing that differs is where the SGPIO's are placed, in terms
     * of BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_XX, and whether the SGPIO's are in
     * BCHP_GIO_DATA_EXT or BCHP_GIO_DATA_HI.
     *
     * Instructions:
     *
     * I2C_GPIO(EXT) or I2C_GPIO(HI)
     * - Use one depending on whether the sgpio's are mapped to
     *   BCHP_GIO_DATA_EXT or BCHP_GIO_DATA_HI
     *
     * I2C_MUX_CTRL_SCL(0, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10, sgpio_00)
     * - The first parameter is the i2c channel
     * - The second parameter is which SUN_TOP_CTRL_PIN_MUX the
     *   corresponding sgpio is mapped to.
     * - The third parameter is the sgpio that is mapped to the i2c channel
     *   and SCL.
     *
     * I2C_MUX_CTRL_SDA(0, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10, sgpio_00)
     * - The first parameter is the i2c channel
     * - The second parameter is which SUN_TOP_CTRL_PIN_MUX the
     *   corresponding sgpio is mapped to.
     * - The third parameter is the sgpio that is mapped to the i2c channel
     *   and SDA.
     *
     */
    #if (BCHP_CHIP==7038)
    I2C_GPIO(EXT);
    I2C_MUX_CTRL_SCL(0, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10, sgpio_00);
    I2C_MUX_CTRL_SDA(0, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10, sgpio_01);
    I2C_MUX_CTRL_SCL(1, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11, sgpio_02);
    I2C_MUX_CTRL_SDA(1, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11, sgpio_03);
    I2C_MUX_CTRL_SCL(2, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11, sgpio_04);
    I2C_MUX_CTRL_SDA(2, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11, sgpio_05);
    I2C_MUX_CTRL_SCL(3, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11, sgpio_06);
    I2C_MUX_CTRL_SDA(3, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11, sgpio_07);
    #elif (BCHP_CHIP==7231)
        I2C_GPIO(EXT);
        #if (BCHP_VER >= BCHP_VER_B0)
            I2C_MUX_CTRL_SCL(0, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_3, sgpio_00);
            I2C_MUX_CTRL_SDA(0, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_3, sgpio_01);
            I2C_MUX_CTRL_SCL(1, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17, sgpio_02);
            I2C_MUX_CTRL_SDA(1, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18, sgpio_03);
            I2C_MUX_CTRL_SCL(2, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18, sgpio_04);
            I2C_MUX_CTRL_SDA(2, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18, sgpio_05);
            I2C_MUX_CTRL_SCL(3, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_sgpio_00);
            I2C_MUX_CTRL_SDA(3, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_01);
        #else
            I2C_MUX_CTRL_SCL(0, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17, sgpio_00);
            I2C_MUX_CTRL_SDA(0, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18, sgpio_01);
            I2C_MUX_CTRL_SCL(1, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18, sgpio_02);
            I2C_MUX_CTRL_SDA(1, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18, sgpio_03);
            I2C_MUX_CTRL_SCL(2, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18, sgpio_04);
            I2C_MUX_CTRL_SDA(2, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18, sgpio_05);
            I2C_MUX_CTRL_SCL(3, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_sgpio_00);
            I2C_MUX_CTRL_SDA(3, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_01);
        #endif
    #elif (BCHP_CHIP==7325)
    I2C_GPIO(EXT);
    I2C_MUX_CTRL_SCL(0, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11, sgpio_00);
    I2C_MUX_CTRL_SDA(0, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11, sgpio_01);
    I2C_MUX_CTRL_SCL(1, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11, sgpio_02);
    I2C_MUX_CTRL_SDA(1, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11, sgpio_03);
    I2C_MUX_CTRL_SCL(2, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11, sgpio_04);
    I2C_MUX_CTRL_SDA(2, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11, sgpio_05);
    I2C_MUX_CTRL_SCL(3, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11, sgpio_06);
    I2C_MUX_CTRL_SDA(3, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11, sgpio_07);
    #elif (BCHP_CHIP==7335) || (BCHP_CHIP==7336)
    I2C_GPIO(EXT);
    I2C_MUX_CTRL_SCL(0, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14, sgpio_00);
    I2C_MUX_CTRL_SDA(0, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14, sgpio_01);
    I2C_MUX_CTRL_SCL(1, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14, sgpio_02);
    I2C_MUX_CTRL_SDA(1, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14, sgpio_03);
    I2C_MUX_CTRL_SCL(2, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14, sgpio_04);
    I2C_MUX_CTRL_SDA(2, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14, sgpio_05);
    I2C_MUX_CTRL_SCL(3, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14, sgpio_06);
    I2C_MUX_CTRL_SDA(3, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_14, sgpio_07);
    #elif (BCHP_CHIP==7346)
    I2C_GPIO(EXT);
    I2C_MUX_CTRL_SCL(0, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16, sgpio_00);
    I2C_MUX_CTRL_SDA(0, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16, sgpio_01);
    I2C_MUX_CTRL_SCL(1, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16, sgpio_02);
    I2C_MUX_CTRL_SDA(1, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16, sgpio_03);
    I2C_MUX_CTRL_SCL(2, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16, sgpio_04);
    I2C_MUX_CTRL_SDA(2, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_17, sgpio_05);
    I2C_MUX_CTRL_SCL(3, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_00);
    I2C_MUX_CTRL_SDA(3, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_01);

    /* Extra SW I2C Master Channel */
    #if (EXTRA_SW_I2C_CHANNELS == 1)
    I2C_GPIO_AON_SCL_LO(4, 2)
    I2C_GPIO_AON_SDA_LO(4, 3)
    I2C_MUX_CTRL_SCL(4, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_002);
    I2C_MUX_CTRL_SDA(4, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0, aon_gpio_003);
    #endif

    #elif ((BCHP_CHIP==7400) || (BCHP_CHIP==7405))
    I2C_GPIO(EXT);
    I2C_MUX_CTRL_SCL(0, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13, sgpio_00);
    I2C_MUX_CTRL_SDA(0, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13, sgpio_01);
    I2C_MUX_CTRL_SCL(1, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13, sgpio_02);
    I2C_MUX_CTRL_SDA(1, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13, sgpio_03);
    I2C_MUX_CTRL_SCL(2, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13, sgpio_04);
    I2C_MUX_CTRL_SDA(2, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13, sgpio_05);
    I2C_MUX_CTRL_SCL(3, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13, sgpio_06);
    I2C_MUX_CTRL_SDA(3, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13, sgpio_07);
    #if (BCHP_CHIP==7400)
    I2C_MUX_CTRL_SCL(4, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13, sgpio_08);
    I2C_MUX_CTRL_SDA(4, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13, sgpio_09);
    #endif
    #elif (BCHP_CHIP==7413)
    I2C_GPIO(EXT);
    I2C_MUX_CTRL_SCL(0, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13, sgpio_00);
    I2C_MUX_CTRL_SDA(0, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13, sgpio_01);
    I2C_MUX_CTRL_SCL(1, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13, sgpio_02);
    I2C_MUX_CTRL_SDA(1, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13, sgpio_03);
    I2C_MUX_CTRL_SCL(2, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13, sgpio_04);
    I2C_MUX_CTRL_SDA(2, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13, sgpio_05);
    I2C_MUX_CTRL_SCL(3, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13, sgpio_06);
    I2C_MUX_CTRL_SDA(3, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13, sgpio_07);
    #elif (BCHP_CHIP==7420)
    I2C_GPIO(EXT);
    I2C_MUX_CTRL_SCL(0, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_21, sgpio_00);
    I2C_MUX_CTRL_SDA(0, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_21, sgpio_01);
    I2C_MUX_CTRL_SCL(1, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_21, sgpio_02);
    I2C_MUX_CTRL_SDA(1, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_21, sgpio_03);
    I2C_MUX_CTRL_SCL(2, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_21, sgpio_04);
    I2C_MUX_CTRL_SDA(2, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_21, sgpio_05);
    I2C_MUX_CTRL_SCL(3, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_21, sgpio_06);
    I2C_MUX_CTRL_SDA(3, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_22, sgpio_07);
    I2C_MUX_CTRL_SCL(4, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_22, sgpio_08);
    I2C_MUX_CTRL_SDA(4, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_22, sgpio_09);
    #elif (BCHP_CHIP==7422) || (BCHP_CHIP==7425)
        #if (BCHP_VER < BCHP_VER_B0)
            /*
                The BSCx mapping to BSC_Mx does not have the normal one-to-one correspondence.
                I.e.,
                    BSC_M0 is BSCD (should be BSCA)
                    BSC_M1 is BSCE (should be BSCB) 
                    BSC_M2 is BSCA (should be BSCC)
                    BSC_M3 is BSCB (should be BSCD)
                    BSC_M4 is BSCC (should be BSCE)
                Therefore , the following mapping will have to be hacked to accomodate this.
                    I2C_GPIO(EXT);
                    I2C_MUX_CTRL_SCL(0, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_sgpio_00);
                    I2C_MUX_CTRL_SDA(0, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_01);
                    I2C_MUX_CTRL_SCL(1, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_02);
                    I2C_MUX_CTRL_SDA(1, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_03);
                    I2C_MUX_CTRL_SCL(2, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18, sgpio_00);
                    I2C_MUX_CTRL_SDA(2, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_19, sgpio_01);
                    I2C_MUX_CTRL_SCL(3, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_19, sgpio_02);
                    I2C_MUX_CTRL_SDA(3, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_19, sgpio_03);
                    I2C_MUX_CTRL_SCL(4, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_19, sgpio_04);
                    I2C_MUX_CTRL_SDA(4, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_19, sgpio_05);
            */
            I2C_GPIO(EXT);
            I2C_MUX_CTRL_SCL(0, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18, sgpio_00);
            I2C_MUX_CTRL_SDA(0, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_19, sgpio_01);
            I2C_MUX_CTRL_SCL(1, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_19, sgpio_02);
            I2C_MUX_CTRL_SDA(1, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_19, sgpio_03);
            I2C_MUX_CTRL_SCL(2, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_19, sgpio_04);
            I2C_MUX_CTRL_SDA(2, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_19, sgpio_05);
            I2C_MUX_CTRL_SCL(3, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_sgpio_00);
            I2C_MUX_CTRL_SDA(3, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_01);
            I2C_MUX_CTRL_SCL(4, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_02);
            I2C_MUX_CTRL_SDA(4, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_03);
        #else
            /* I2C_GPIO(EXT); */
            I2C_GPIO_AON_SCL_EXT(0, 0); /* aon_sgpio_00 */
            I2C_GPIO_AON_SDA_EXT(0, 1); /* aon_sgpio_01 */			
            I2C_GPIO_AON_SCL_EXT(1, 2); /* aon_sgpio_02 */
            I2C_GPIO_AON_SDA_EXT(1, 3); /* aon_sgpio_03 */
            I2C_GPIO_SCL_EXT(2, 0); /* sgpio_00 */
            I2C_GPIO_SDA_EXT(2, 1); /* sgpio_01 */
            I2C_GPIO_SCL_EXT(3, 2); /* sgpio_02 */
            I2C_GPIO_SDA_EXT(3, 3); /* sgpio_03 */			
            I2C_GPIO_AON_SCL_EXT(4, 4); /* aon_sgpio_04 */
            I2C_GPIO_AON_SDA_EXT(4, 5); /* aon_sgpio_05 */
            I2C_MUX_CTRL_SCL(0, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_sgpio_00);
            I2C_MUX_CTRL_SDA(0, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_sgpio_01);
            I2C_MUX_CTRL_SCL(1, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_02);
            I2C_MUX_CTRL_SDA(1, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_03);
            I2C_MUX_CTRL_SCL(2, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18, sgpio_00);
            I2C_MUX_CTRL_SDA(2, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_19, sgpio_01);
            I2C_MUX_CTRL_SCL(3, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_19, sgpio_02);
            I2C_MUX_CTRL_SDA(3, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_19, sgpio_03);
            I2C_MUX_CTRL_SDA(4, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_05);
            I2C_MUX_CTRL_SCL(4, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_04);
        #endif
    #elif (BCHP_CHIP==7429)
    /* I2C_GPIO(EXT); */
    I2C_GPIO_AON_SCL_EXT(0, 0); /* aon_sgpio_00 */
    I2C_GPIO_AON_SDA_EXT(0, 1); /* aon_sgpio_01 */			
    I2C_GPIO_AON_SCL_EXT(1, 2); /* aon_sgpio_02 */
    I2C_GPIO_AON_SDA_EXT(1, 3); /* aon_sgpio_03 */
    I2C_GPIO_AON_SCL_EXT(2, 4); /* aon_sgpio_04 */
    I2C_GPIO_AON_SDA_EXT(2, 5); /* aon_sgpio_05 */
    I2C_GPIO_SCL_EXT(3, 0); /* sgpio_00 */
    I2C_GPIO_SDA_EXT(3, 1); /* sgpio_01 */
    I2C_GPIO_SCL_EXT(4, 2); /* sgpio_02 */
    I2C_GPIO_SDA_EXT(4, 3); /* sgpio_03 */			
    I2C_MUX_CTRL_SCL(0, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_sgpio_00);
    I2C_MUX_CTRL_SDA(0, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_01);
    I2C_MUX_CTRL_SCL(1, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_02);
    I2C_MUX_CTRL_SDA(1, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_03);
    I2C_MUX_CTRL_SCL(2, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_04);
    I2C_MUX_CTRL_SDA(2, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_05);
    I2C_MUX_CTRL_SCL(3, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18, sgpio_00);
    I2C_MUX_CTRL_SDA(3, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18, sgpio_01);
    I2C_MUX_CTRL_SCL(4, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18, sgpio_02);
    I2C_MUX_CTRL_SDA(4, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18, sgpio_03);
    #elif (BCHP_CHIP==7435)
    /* I2C_GPIO(EXT); */
    I2C_GPIO_AON_SCL_EXT(0, 0); /* aon_sgpio_00 */
    I2C_GPIO_AON_SDA_EXT(0, 1); /* aon_sgpio_01 */			
    I2C_GPIO_AON_SCL_EXT(1, 2); /* aon_sgpio_02 */
    I2C_GPIO_AON_SDA_EXT(1, 3); /* aon_sgpio_03 */
    I2C_GPIO_SCL_EXT(2, 0); /* sgpio_00 */
    I2C_GPIO_SDA_EXT(2, 1); /* sgpio_01 */
    I2C_GPIO_SCL_EXT(3, 2); /* sgpio_02 */
    I2C_GPIO_SDA_EXT(3, 3); /* sgpio_03 */			
    I2C_GPIO_AON_SCL_EXT(4, 4); /* aon_sgpio_04 */
    I2C_GPIO_AON_SDA_EXT(4, 5); /* aon_sgpio_05 */
    I2C_MUX_CTRL_SCL(0, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_sgpio_00);
    I2C_MUX_CTRL_SDA(0, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2, aon_sgpio_01);
    I2C_MUX_CTRL_SCL(1, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_02);
    I2C_MUX_CTRL_SDA(1, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_03);
    I2C_MUX_CTRL_SCL(2, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_18, sgpio_00);
    I2C_MUX_CTRL_SDA(2, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_19, sgpio_01);
    I2C_MUX_CTRL_SCL(3, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_19, sgpio_02);
    I2C_MUX_CTRL_SDA(3, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_19, sgpio_03);
    I2C_MUX_CTRL_SDA(4, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_05);
    I2C_MUX_CTRL_SCL(4, BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_3, aon_sgpio_04);
    #elif (BCHP_CHIP==7601)
    I2C_GPIO(HI);
    I2C_MUX_CTRL_SCL(0, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7, sgpio_00);
    I2C_MUX_CTRL_SDA(0, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7, sgpio_01);
    I2C_MUX_CTRL_SCL(1, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7, sgpio_02);
    I2C_MUX_CTRL_SDA(1, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7, sgpio_03);
    I2C_MUX_CTRL_SCL(2, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7, sgpio_04);
    I2C_MUX_CTRL_SDA(2, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7, sgpio_05);
    #elif (BCHP_CHIP==7635)
    I2C_GPIO(EXT);
    I2C_MUX_CTRL_SCL(0, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15, sgpio_00);
    I2C_MUX_CTRL_SDA(0, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15, sgpio_01);
    I2C_MUX_CTRL_SCL(1, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_15, sgpio_02);
    I2C_MUX_CTRL_SDA(1, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16, sgpio_03);
    I2C_MUX_CTRL_SCL(2, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16, sgpio_04);
    I2C_MUX_CTRL_SDA(2, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_16, sgpio_05);
    #elif (BCHP_CHIP==7630)
    I2C_GPIO(EXT);
    I2C_MUX_CTRL_SCL(0, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13, sgpio_00);
    I2C_MUX_CTRL_SDA(0, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13, sgpio_01);
    I2C_MUX_CTRL_SCL(1, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13, sgpio_02);
    I2C_MUX_CTRL_SDA(1, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13, sgpio_03);
    I2C_MUX_CTRL_SCL(2, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13, sgpio_04);
    I2C_MUX_CTRL_SDA(2, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_13, sgpio_05);
    #elif (BCHP_CHIP==7550)
    /* left blank for now */
    #elif (BCHP_CHIP==7125)
    I2C_GPIO(EXT);
    I2C_MUX_CTRL_SCL(0, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10, sgpio_00);
    I2C_MUX_CTRL_SDA(0, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10, sgpio_01);
    I2C_MUX_CTRL_SCL(1, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10, sgpio_02);
    I2C_MUX_CTRL_SDA(1, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10, sgpio_03);
    I2C_MUX_CTRL_SCL(2, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10, sgpio_04);
    I2C_MUX_CTRL_SDA(2, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10, sgpio_05);
    I2C_MUX_CTRL_SCL(3, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_10, sgpio_06);
    I2C_MUX_CTRL_SDA(3, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_11, sgpio_07);
    #elif (BCHP_CHIP==7408)
    I2C_GPIO(EXT);
    I2C_MUX_CTRL_SCL(0, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7, sgpio_00);
    I2C_MUX_CTRL_SDA(0, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7, sgpio_01);
    I2C_MUX_CTRL_SCL(1, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7, sgpio_02);
    I2C_MUX_CTRL_SDA(1, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7, sgpio_03);
    I2C_MUX_CTRL_SCL(2, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8, sgpio_04);
    I2C_MUX_CTRL_SDA(2, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8, sgpio_05);
    #else
    #error Unsupported chip at this time.  Need to define I2C GPIO mapping!
    #endif
    #endif
}

/***********************************************************************func
 * Name: BI2C_P_Delay
 *   -
 *
 * NOTE:  This function is used to delay the clock and data pulse to simulate
 * as best as possible the real clock and data pulse of the BSC.  It is
 * obtained through experimentation in the lab via a scope.
 ***************************************************************************/
void BI2C_P_Delay(BI2C_Clk rate)
{
    /* If you get a compile error not having this function defined. You should have this defined
        in kernel interface. */
    /* bcmKNIDelay(10); */
    switch (rate)
    {
        case BI2C_Clk_eClk47Khz:
            BKNI_Delay(7); /* 43KHz*/
            break;
        case BI2C_Clk_eClk50Khz:
            BKNI_Delay(6); /* 48KHz*/
            break;
        case BI2C_Clk_eClk93Khz:
            BKNI_Delay(2); /* 81KHz*/
            break;
        case BI2C_Clk_eClk100Khz:
        case BI2C_Clk_eClk187Khz:
        case BI2C_Clk_eClk200Khz:
        case BI2C_Clk_eClk375Khz:
        case BI2C_Clk_eClk400Khz:
        default:
            break;
    }
}

/***********************************************************************func
 * Name: BI2C_P_ClkSet
 *   - set the CLOCK bit high/low.
 *
 * NOTE:
 ***************************************************************************/
BERR_Code _BI2C_P_ClkSet(BREG_Handle hReg, int ch, int clk, BI2C_Clk rate)
{
    uint32_t data, count=0;
    uint32_t mask = i2c_gpio[ch].gio_scl_mask;
    uint32_t addr = i2c_gpio[ch].gio_data_scl_reg;
#define CLK_STRETCH_TIMEOUT 5000000 /* 5 seconds */

    /* put in delay for timing purposes */
    BI2C_P_Delay(rate);

    #ifdef RMW_METHOD
    /* read clk port */
    data = BREG_Read32(hReg, addr);

    /* set CLOCK bit high/low */
    if (clk)
        data |= mask;
    else
        data &= ~mask;

    /* write clk port with new CLOCK value */
    BREG_Write32(hReg, addr, data);
    #else
    BI2C_P_IodirScl(clk);
    #endif

    /* clock stretching */
    if (clk)
    {
        /* we have to wait until we see clock is set before we leave. */
        while (!(BREG_Read32(hReg, addr) & mask))
        {
            BKNI_Delay(1);
            if(++count > CLK_STRETCH_TIMEOUT)
            {
                BDBG_ERR(("clock stretch timer expired\n"));
                return BERR_TIMEOUT;
            }
        }
    }
    else
    {
        /* dummy read to force the write to flush out */
        data = BREG_Read32(hReg, addr);
    }

    return BERR_SUCCESS;
}

/***********************************************************************func
 * Name: BI2C_P_DataSet
 *   - set the DATA bit high/low.
 *
 * NOTE:
 ***************************************************************************/
BERR_Code _BI2C_P_DataSet(BREG_Handle hReg, int ch, int data, BI2C_Clk rate)
{
    #ifdef RMW_METHOD
    uint32_t port_data;
    uint32_t addr = i2c_gpio[ch].gio_data_sda_reg;
    uint32_t mask = i2c_gpio[ch].gio_sda_mask;
    #endif

    /* put in delay for timing purposes */
    BI2C_P_Delay(rate);

    #ifdef RMW_METHOD
    port_data = BREG_Read32(hReg, addr);
    if (data)
        BREG_Write32(hReg, addr, port_data | mask);
    else
        BREG_Write32(hReg, addr, port_data & ~mask);

    /* dummy read to force the write to flush out */
    port_data = BREG_Read32(hReg, addr);
    #else
    BI2C_P_IodirSda(data);
    #endif

    return BERR_SUCCESS;
}

/***********************************************************************func
 * Name: BI2C_P_DataGet
 *   - returns the state of the DATA bit (high/low).
 *
 * NOTE:
 ***************************************************************************/
int _BI2C_P_DataGet(BREG_Handle hReg, int ch)
{
    uint32_t port_data;

    port_data = BREG_Read32(hReg, i2c_gpio[ch].gio_data_sda_reg);

    /* return state of DATA bit */
    if (port_data & i2c_gpio[ch].gio_sda_mask)
        return 1;
    else return 0;
}

/***********************************************************************func
 * Name: _BI2C_P_ByteWrite
 *   - clocks out 1 bytes of data and returns the status
 *     of the Ack/Nack bit.
 *
 * NOTE:
 ***************************************************************************/
BERR_Code _BI2C_P_ByteWrite(BREG_Handle hReg, int ch, unsigned char data, BI2C_Clk rate, int *ret_ack)
{
    BERR_Code rc;
    unsigned char i;

    /* clock out data byte */
    for (i = 0; i < 8; i++)
    {
        BI2C_P_ClkSet(0);
        BI2C_P_DataSet(data & 0x80);
        data = data << 1;
        if ((rc = BI2C_P_ClkSet(1)) != BERR_SUCCESS) return rc;
    }

    BI2C_P_ClkSet(0);
    BI2C_P_DataSet(1);

    /* read ack bit */
    if ((rc = BI2C_P_ClkSet(1)) != BERR_SUCCESS) return rc;
    *ret_ack = BI2C_P_DataGet();
    BI2C_P_ClkSet(0);
    BI2C_P_DataSet(0);
    return BERR_SUCCESS;
}

/***********************************************************************func
 * Name: _BI2C_P_ByteRead
 *   - reads 1 bytes of data and returns the status of the
 *     Ack/Nack bit.
 *
 * NOTE:
 ***************************************************************************/
BERR_Code _BI2C_P_ByteRead(BREG_Handle hReg, int ch, int generate_ack, BI2C_Clk rate, unsigned char *ret_data)
{
    BERR_Code rc;
    unsigned char data;
    unsigned char i;

    /* initialize data to 0 */
    data = 0;

    /* clock in data byte */
    for (i = 0; i < 8; i++)
    {
        BI2C_P_ClkSet(0);
        BI2C_P_DataSet(1);
        if ((rc = BI2C_P_ClkSet(1)) != BERR_SUCCESS) return rc;
        data = (data << 1) | BI2C_P_DataGet();
    }

    BI2C_P_ClkSet(0);

    /* generate Ack/Nack */
    if (generate_ack)
        BI2C_P_DataSet(0);
    else BI2C_P_DataSet(1);

    /* clock in Ack/Nack */
    if ((rc = BI2C_P_ClkSet(1)) != BERR_SUCCESS) return rc;
    BI2C_P_ClkSet(0);
    BI2C_P_DataSet(0);

    *ret_data = data;
    return BERR_SUCCESS;
}

/***********************************************************************func
 * Name: BI2C_P_RegBitSet
 *   - set register bit, return previous value.
 *
 * NOTE:
 ***************************************************************************/
int BI2C_P_RegBitSet(BREG_Handle hReg, uint32_t addr, uint32_t mask, uint8_t val)
{
    uint32_t temp;
    temp = BREG_Read32(hReg, addr);
    if (val)
        BREG_Write32(hReg, addr, temp | mask);
    else
        BREG_Write32(hReg, addr, temp & ~mask);

    /* Dummy read to force the flush out */
    BREG_Read32(hReg, addr);
    return ((temp & mask) ? 1 : 0);
}

#if 0
/***********************************************************************func
 * Name: BI2C_P_RegFieldSet
 *   - set register field, return previous value.
 *
 * NOTE:
 ***************************************************************************/
int BI2C_P_RegFieldSet(BREG_Handle hReg, uint32_t addr, uint8_t shift, uint8_t val)
{
    uint32_t temp, masked;
    temp = BREG_Read32(hReg, addr);

    /* modifying a 3 bit field */
    masked = temp & ~(0x3 << shift);
    BREG_Write32(hReg, addr, masked | (val << shift));

    return ((temp >> shift) & 0x3);
}
#endif

/***********************************************************************func
 * Name: _BI2C_P_SunTopScl
 *   - set register field, return previous value.
 *
 * NOTE:
 ***************************************************************************/
int _BI2C_P_SunTopScl(BREG_Handle hReg, int ch, uint8_t val)
{
    uint32_t retval=0;
    uint32_t lval;
    uint32_t reg = i2c_gpio[ch].sun_top_scl_reg;
    uint32_t shift = i2c_gpio[ch].sun_top_scl_shift;
    uint32_t mask = i2c_gpio[ch].sun_top_scl_mask;
    if (ch<BI2C_MAX_I2C_CHANNELS+EXTRA_SW_I2C_CHANNELS)
    {
        lval = BREG_Read32 (hReg, reg);
        retval = (lval & mask) >> shift;
        lval &= ~mask;
        lval |= (val << shift);
        BREG_Write32 (hReg, reg, lval);
    }
    else
    {
        BDBG_ERR(("Unknown channel in BI2C_P_SunTopSda\n"));
    }
    return (retval);
}

/***********************************************************************func
 * Name: _BI2C_P_SunTopSda
 *   - set register field, return previous value.
 *
 * NOTE:
 ***************************************************************************/
int _BI2C_P_SunTopSda(BREG_Handle hReg, int ch, uint8_t val)
{
    uint32_t retval=0;
    uint32_t lval;
    uint32_t reg = i2c_gpio[ch].sun_top_sda_reg;
    uint32_t shift = i2c_gpio[ch].sun_top_sda_shift;
    uint32_t mask = i2c_gpio[ch].sun_top_sda_mask;

    if (ch<BI2C_MAX_I2C_CHANNELS+EXTRA_SW_I2C_CHANNELS)
    {
        lval = BREG_Read32 (hReg, reg);
        retval = (lval & mask) >> shift;
        lval &= ~mask;
        lval |= (val << shift);
        BREG_Write32 (hReg, reg, lval);
    }
    else
    {
        BDBG_ERR(("Unknown channel in BI2C_P_SunTopSda\n"));
    }
    return (retval);
}

/***********************************************************************func
 * Name: BI2C_P_ReadSwCmd
 *   - Read data from i2c slave device.
 *       write (1 byte of slave device register's address, subAddr)
 *   and read  (0-8 bytes of data)
 *   Using i2c Write.& Read
 *
 * NOTE: This function will be called by BI2C_P_Read, BI2C_P_ReadA16, BI2C_P_ReadNoAddr
 ***************************************************************************/
BERR_Code BI2C_P_ReadSwCmd
(
    BI2C_ChannelHandle  hChn,           /* Device channel handle */
    uint16_t        chipAddr,           /* i2c chip address.  this is unshifted */
    uint32_t        subAddr,            /* pointer to register address */
    uint8_t         numSubAddrBytes,    /* number of bytes in register address */
    uint8_t         *pData,             /* storage */
    size_t          numBytes,           /* number of bytes to read */
    bool            eddc,               /* EDDC mode */
    uint8_t         segment,            /* EDDC segment */
    bool            checkForAck         /* Check for ack? */
)
{
    #if (BI2C_SUPPORT_I2C_VIA_GPIO==1)
    uint16_t i;
    uint8_t oden_scl, oden_sda;
    uint8_t iodir_scl, iodir_sda;
    uint8_t sun_top_sda, sun_top_scl;
    int no_ack = 0;
    int ch = hChn->chnNo;
    BI2C_Handle hDev;
    BREG_Handle hReg;
    BI2C_Clk rate = hChn->clkRate;
    uint8_t lval;
    int ret_ack;
    BERR_Code rc;
    unsigned char ret_data;

    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );
    BDBG_ASSERT( numSubAddrBytes <= 3);
    BDBG_ASSERT( pData );

#define RD 0x01
#define WR 0x00

    /* Get I2C handle from channel handle */
    hDev = hChn->hI2c;
    hReg = hDev->hRegister;

    sun_top_scl = BI2C_P_SunTopScl(0);
    sun_top_sda = BI2C_P_SunTopSda(0);

    /* Set open drain output */
    oden_scl = BI2C_P_OdenScl(1);
    oden_sda = BI2C_P_OdenSda(1);

    #ifdef RMW_METHOD
    /* reset CLOCK and DATA */
    BI2C_P_ClkSet(1);
    BI2C_P_DataSet(1);

    /* Set gpio as output */
    iodir_scl = BI2C_P_IodirScl(0);
    iodir_sda = BI2C_P_IodirSda(0);
    #else
    /* Set iodir to output so we can change data */
    iodir_scl = BI2C_P_IodirScl(0);
    iodir_sda = BI2C_P_IodirSda(0);

    /* Set data low for using iodir to control output */
    BI2C_P_DataScl(0);
    BI2C_P_DataSda(0);

    /* Set iodir to input so we can start with SCL and SDA high */
    BI2C_P_IodirScl(1);
    BI2C_P_IodirSda(1);
    #endif

    /* generate start condition */
    BI2C_P_DataSet(0);

    if (eddc)
    {
        if (segment)
        {
            /* program slave device's (id) */
            /* BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CHIP_ADDRESS), (uint32_t)EDDC_SEGMENT_CHIP_ADDR ); */
            if ((rc = BI2C_P_ByteWrite((unsigned char)(EDDC_SEGMENT_CHIP_ADDR), &ret_ack)) != BERR_SUCCESS) goto no_success;
            if (ret_ack != I2C_ACK)
            {
                /*
                BDBG_ERR(("ignore no ack!\n"));
                no_ack = 1;
                */
            }

            /* write segment pointer into fifo */
            /* BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_DATA_IN0), (uint32_t)segment ); */
            if ((rc = BI2C_P_ByteWrite((unsigned char)(segment), &ret_ack)) != BERR_SUCCESS) goto no_success;
            if (ret_ack != I2C_ACK)
            {
                BDBG_ERR(("no ack!\n"));
                no_ack = 1;
                goto no_ack_detected;
            }

            /* generate repeated start condition */
            BI2C_P_DataSet(1);
            BI2C_P_ClkSet(1);
            BI2C_P_DataSet(0);
        }
    }

    if (numSubAddrBytes)
    {
        /* Send out the chip address with a write */
        if (chipAddr & 0x0380)              /* 10-bit chip address */
        {
            /* Send bits 9 and 8 */
            lval = ((chipAddr & 0x0300) >> 7) | 0xF0;
            if ((rc = BI2C_P_ByteWrite(lval | WR, &ret_ack)) != BERR_SUCCESS) goto no_success;
            if (ret_ack != I2C_ACK)
            {
                BDBG_ERR(("no ack!\n"));
                no_ack = 1;
                goto no_ack_detected;
            }
            /* Send the rest of the bits */
            lval = chipAddr & 0xff;
            if ((rc = BI2C_P_ByteWrite(lval, &ret_ack)) != BERR_SUCCESS) goto no_success;
            if (checkForAck)
            {
                if (ret_ack != I2C_ACK)
                {
                    BDBG_ERR(("no ack!\n"));
                    no_ack = 1;
                    goto no_ack_detected;
                }
            }
        }
        else
        {
            lval = (chipAddr & 0x7f) << 1;
            if ((rc = BI2C_P_ByteWrite(lval | WR, &ret_ack)) != BERR_SUCCESS) goto no_success;
            if (checkForAck)
            {
                if (ret_ack != I2C_ACK)
                {
                    BDBG_ERR(("no ack!\n"));
                    no_ack = 1;
                    goto no_ack_detected;
                }
            }
        }

        if (numSubAddrBytes==3)
        {
            uint32_t subAddr32 = subAddr;/* = *((uint32_t *)pSubAddr);*/
            lval = (uint8_t)((subAddr32 >> 16) & 0xff);
            if ((rc = BI2C_P_ByteWrite(lval, &ret_ack)) != BERR_SUCCESS) goto no_success;
            if (checkForAck)
            {
                if (ret_ack != I2C_ACK)
                {
                    BDBG_ERR(("no ack!\n"));
                    no_ack = 1;
                    goto no_ack_detected;
                }
            }
            lval = (uint8_t)((subAddr32 >> 8) & 0xff);
            if ((rc = BI2C_P_ByteWrite(lval, &ret_ack)) != BERR_SUCCESS) goto no_success;
            if (checkForAck)
            {
                if (ret_ack != I2C_ACK)
                {
                    BDBG_ERR(("no ack!\n"));
                    no_ack = 1;
                    goto no_ack_detected;
                }
            }
            lval = (uint8_t)(subAddr32 & 0xff);
            if ((rc = BI2C_P_ByteWrite(lval, &ret_ack)) != BERR_SUCCESS) goto no_success;
            if (checkForAck)
            {
                if (ret_ack != I2C_ACK)
                {
                    BDBG_ERR(("no ack!\n"));
                    no_ack = 1;
                    goto no_ack_detected;
                }
            }
        }
        else if (numSubAddrBytes==2)
        {
            uint16_t subAddr16 = subAddr;/* = *((uint16_t *)pSubAddr);*/
            lval = (uint8_t)((subAddr16 >> 8) & 0xff);
            if ((rc = BI2C_P_ByteWrite(lval, &ret_ack)) != BERR_SUCCESS) goto no_success;
            if (checkForAck)
            {
                if (ret_ack != I2C_ACK)
                {
                    BDBG_ERR(("no ack!\n"));
                    no_ack = 1;
                    goto no_ack_detected;
                }
            }
            lval = (uint8_t)(subAddr16 & 0xff);
            if ((rc = BI2C_P_ByteWrite(lval, &ret_ack)) != BERR_SUCCESS) goto no_success;
            if (checkForAck)
            {
                if (ret_ack != I2C_ACK)
                {
                    BDBG_ERR(("no ack!\n"));
                    no_ack = 1;
                    goto no_ack_detected;
                }
            }
        }
        else if (numSubAddrBytes==1)
        {
            uint8_t subAddr8 = subAddr; /* = *((uint8_t *)pSubAddr);*/
            lval = (uint8_t)(subAddr8 & 0xff);
            if ((rc = BI2C_P_ByteWrite(lval, &ret_ack)) != BERR_SUCCESS) goto no_success;
            if (checkForAck)
            {
                if (ret_ack != I2C_ACK)
                {
                    BDBG_ERR(("no ack!\n"));
                    no_ack = 1;
                    goto no_ack_detected;
                }
            }
        }

        /* generate repeated start condition */
        BI2C_P_DataSet(1);
        BI2C_P_ClkSet(1);
        BI2C_P_DataSet(0);
    }

    /* Start the read transaction.  Must send out the address again. */
    if (chipAddr & 0x0380)              /* 10-bit chip address */
    {
        /* Send bits 9 and 8.  Do not need to send the rest of the 10 bit address. */
        lval = ((chipAddr & 0x0300) >> 7) | 0xF0;
        if ((rc = BI2C_P_ByteWrite(lval | RD, &ret_ack)) != BERR_SUCCESS) goto no_success;
        if (checkForAck)
        {
            if (ret_ack != I2C_ACK)
            {
                BDBG_ERR(("no ack!\n"));
                no_ack = 1;
                goto no_ack_detected;
            }
        }
    }
    else
    {
        lval = (chipAddr & 0x7f) << 1;
        if ((rc = BI2C_P_ByteWrite(lval | RD, &ret_ack)) != BERR_SUCCESS) goto no_success;
        if (checkForAck)
        {
            if (ret_ack != I2C_ACK)
            {
                BDBG_ERR(("no ack!\n"));
                no_ack = 1;
                goto no_ack_detected;
            }
        }
    }

    for (i = 0; i < (numBytes-1); i++)
    {
        if ((rc = BI2C_P_ByteRead(1, &ret_data) != BERR_SUCCESS)) goto no_success;
        pData[i] = ret_data;
    }
    if ((rc = BI2C_P_ByteRead(0, &ret_data) != BERR_SUCCESS)) goto no_success;
    pData[i] = ret_data;

no_success:
no_ack_detected:
    /* generate stop condition */
    BI2C_P_ClkSet(1);
    BI2C_P_DataSet(1);

    /* restore iodir */
    BI2C_P_IodirScl(iodir_scl);
    BI2C_P_IodirSda(iodir_sda);

    /* restore open drain output */
    BI2C_P_OdenScl(oden_scl);
    BI2C_P_OdenSda(oden_sda);

    /* restore sun top */
    BI2C_P_SunTopScl(sun_top_scl);
    BI2C_P_SunTopSda(sun_top_sda);

    BDBG_MSG(("BI2C_P_ReadSwCmd:  ch=%d, clkRate=%d, chipAddr=0x%x, numSubAddrBytes=%d, subAddr=0x%x, numBytes=%d, pData=0x%x\n", hChn->chnNo, hChn->clkRate, chipAddr, numSubAddrBytes, numSubAddrBytes ? subAddr : 0, numBytes, *(uint8_t *)pData));

    if (no_ack) return BI2C_ERR_NO_ACK;
    else return BERR_SUCCESS;
    #else
    BSTD_UNUSED( hChn );
    BSTD_UNUSED( chipAddr );
    BSTD_UNUSED( subAddr );
    BSTD_UNUSED( numSubAddrBytes );
    BSTD_UNUSED( pData );
    BSTD_UNUSED( numBytes );
    BSTD_UNUSED( eddc );
    BSTD_UNUSED( segment);
    BSTD_UNUSED( checkForAck );

    BDBG_ERR(("BI2C_P_ReadSwCmd called, but not currently supported.  Make sure the function BI2C_P_GpioInit() is setup properly for this chip.\n"));
    return BERR_NOT_SUPPORTED;
    #endif
}

/***********************************************************************func
 * Name: BI2C_P_ReadCmd
 *   - Read data from i2c slave device.
 *       write (1 byte of slave device register's address, subAddr)
 *   and read  (0-8 bytes of data)
 *   Using i2c Write.& Read
 *
 * NOTE: This function will be called by BI2C_P_Read, BI2C_P_ReadA16, BI2C_P_ReadNoAddr
 ***************************************************************************/
BERR_Code BI2C_P_ReadCmd
(
    BI2C_ChannelHandle  hChn,           /* Device channel handle */
    uint16_t        chipAddr,           /* i2c chip address.  this is unshifted */
    uint32_t        subAddr,            /* sub address */
    uint8_t         numSubAddrBytes,    /* number of bytes in register address */
    uint8_t         *pData,             /* storage */
    size_t          numBytes,           /* number of bytes to read */
    bool            mutex,              /* protect with a mutex */
    bool            ack,                /* check for ack? */
    bool            noStop              /* no stop at the end? */
)
{
    BI2C_Handle         hDev;
    BERR_Code           retCode = BERR_SUCCESS;
    uint8_t             numWriteBytes = 0;
    uint32_t            bscRegAddr, readCmdWord, bufferIndex, lval = 0, i;
    size_t              cnt;

    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );
    BDBG_ASSERT( pData );
    
    /* Get I2C handle from channel handle */
    hDev = hChn->hI2c;

    if (mutex)
        BI2C_P_ACQUIRE_MUTEX( hChn );

    /* program slave device's (id) */
    if (chipAddr & 0x0380)              /* 10-bit chip address */
    {
        lval = ((chipAddr & 0x0300) >> 7) | 0xF0;
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CHIP_ADDRESS), lval );
        lval = (uint32_t)(chipAddr & 0xff);
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_DATA_IN0), lval );
        bscRegAddr = BCHP_BSCA_DATA_IN1;
        numWriteBytes++;
    }
    else
    {
        lval = (uint32_t) ((chipAddr & 0x7f) << 1);
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CHIP_ADDRESS), lval );
        bscRegAddr = BCHP_BSCA_DATA_IN0;
    }

    if (numSubAddrBytes)    /* read with sub addr, must issue a write */
    {
        /* program slave device's register address */
        if (numSubAddrBytes == 3)
        {
            lval = (subAddr >> 16) & 0xff;
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + bscRegAddr), lval );
            bscRegAddr += 4;
            lval = (subAddr >> 8) & 0xff;
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + bscRegAddr), lval );
            bscRegAddr += 4;
            lval = subAddr & 0xff;
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + bscRegAddr), lval );
            bscRegAddr += 4;
            numWriteBytes += 3;
        }
        else if (numSubAddrBytes == 2)
        {
            lval = (uint32_t)(subAddr >> 8);
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + bscRegAddr), lval );
            bscRegAddr += 4;
            lval = (uint32_t)(subAddr & 0xff);
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + bscRegAddr), lval );
            bscRegAddr += 4;
            numWriteBytes += 2;
        }
        else if (numSubAddrBytes == 1)
        {
            lval = (uint32_t)(subAddr & 0xff);
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + bscRegAddr), lval );
            bscRegAddr += 4;
            numWriteBytes++;
        }

        /* program amount of bytes to write/read */
        lval = (uint32_t)(numWriteBytes & 0x0f);
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CNT_REG), lval );

        /* program data transfer type */
        lval = BREG_Read32(hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CTL_REG));
        lval &= ~(BCHP_BSCA_CTL_REG_reserved0_MASK | BCHP_BSCA_CTL_REG_DTF_MASK);
        lval |= BI2C_P_eWriteOnly ;
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CTL_REG), lval );

        if (!ack)
        {
            /* Mao Neil: ignore ack */
            /* printf("Disable the acknowledge!\n\n"); */
            lval = BREG_Read32(hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CTLHI_REG));
            lval |= 0x2;
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CTLHI_REG), lval );
        }

        /* start the write command */
        lval = (BCHP_BSCA_IIC_ENABLE_RESTART_MASK | BCHP_BSCA_IIC_ENABLE_NO_STOP_MASK |
                BCHP_BSCA_IIC_ENABLE_ENABLE_MASK);
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_IIC_ENABLE), lval );

        retCode = BI2C_P_WaitForCompletion(hChn, numWriteBytes);
        if (retCode != BERR_SUCCESS)
        {
            /* We will have to quit.  But if no ack, at least send the stop condition. */
            if (retCode == BI2C_ERR_NO_ACK)
            {
                #if 0
                /* Finish the transaction and then quit */
                BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CNT_REG), 0 );
                BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_IIC_ENABLE),
                            (BCHP_BSCA_IIC_ENABLE_NO_START_MASK | BCHP_BSCA_IIC_ENABLE_ENABLE_MASK));
                rc = BI2C_P_WaitForCompletion(hChn, 1);
                if (rc != BERR_SUCCESS)
                    goto done;
                #endif
            }
            goto done;
        }
    }

    /*
     * Now perform the read portion
     */

    /* program data transfer type */
    lval = BREG_Read32(hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CTL_REG));
    lval &= ~(BCHP_BSCA_CTL_REG_reserved0_MASK | BCHP_BSCA_CTL_REG_DTF_MASK);
    lval |= BI2C_P_eReadOnly;
    BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CTL_REG), lval );

    readCmdWord = 0;
    cnt = numBytes;
    bufferIndex = 0;

    while (cnt)
    {
        if (cnt <= MAX_I2C_READ_REQUEST)
        {
            /* program amount of bytes to read */
            lval = cnt;
            cnt = 0;
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CNT_REG), lval );
            if (noStop) {
                readCmdWord |= (BCHP_BSCA_IIC_ENABLE_NO_STOP_MASK | BCHP_BSCA_IIC_ENABLE_RESTART_MASK);
            }
        }
        else
        {
            /* program amount of bytes to read */
            lval = MAX_I2C_READ_REQUEST;
            cnt -= MAX_I2C_READ_REQUEST;
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CNT_REG), lval );

            readCmdWord |= BCHP_BSCA_IIC_ENABLE_NO_STOP_MASK;

        }
        /* start the read command */
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_IIC_ENABLE),
                    (readCmdWord | BCHP_BSCA_IIC_ENABLE_ENABLE_MASK));

        retCode = BI2C_P_WaitForCompletion(hChn, lval);
        if (retCode != BERR_SUCCESS)
        {
            #if 0
            /* We will have to quit.  But if no ack, at least send the stop condition. */
            if ((retCode == BI2C_ERR_NO_ACK) && (readCmdWord & BCHP_BSCA_IIC_ENABLE_NO_STOP_MASK))
            {
                /* Finish the transaction and then quit */
                BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CNT_REG), 0 );
                BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_IIC_ENABLE),
                            (BCHP_BSCA_IIC_ENABLE_NO_START_MASK | BCHP_BSCA_IIC_ENABLE_ENABLE_MASK));
                rc = BI2C_P_WaitForCompletion(hChn, 1);
                if (rc != BERR_SUCCESS)
                    goto done;
            }
            #endif
            goto done;
        }

        /* copy data to buffer */
        for(i = 0; i < lval; i++)
        {
            pData[bufferIndex++] = (uint8_t)(BREG_Read32 (hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_DATA_OUT0 + (i * 4))));
        }

        if (cnt)
        {
            readCmdWord = BCHP_BSCA_IIC_ENABLE_NO_START_MASK;
        }
    }

done:
    if (mutex)
        BI2C_P_RELEASE_MUTEX( hChn );

    BDBG_MSG(("BI2C_P_ReadCmd:  ch=%d, clkRate=%d, chipAddr=0x%x, numSubAddrBytes=%d SubAddr=0x%x, numBytes=%d, pData=0x%x", hChn->chnNo, hChn->clkRate, chipAddr, numSubAddrBytes, numSubAddrBytes ? subAddr : 0, numBytes, *(uint8_t *)pData));
    #ifdef BI2C_DEBUG
    {
        int i;
        for (i=0; i<(int)numBytes; i++)
            BDBG_MSG(("%d:  0x%x", i, *(pData+i)));
    }
    #endif

    return retCode;
}

BERR_Code BI2C_P_BurstReadCmd
(
    BI2C_ChannelHandle  hChn,           /* Device channel handle */
    uint16_t        chipAddr,           /* i2c chip address.  this is unshifted */
    uint32_t        subAddr,            /* pointer to register address */
    uint8_t         numSubAddrBytes,    /* number of bytes in register address */
    uint8_t         *pData,             /* storage */
    size_t          numBytes            /* number of bytes to read */
)
{
    BI2C_Handle     hDev;
    BERR_Code       retCode = BERR_SUCCESS;
    uint8_t         numWriteBytes = 0;
    uint32_t        bscRegAddr, readCmdWord, bufferIndex, lval = 0, rval, i;
    size_t          cnt;

    BSTD_UNUSED( pData );
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );
    BDBG_ASSERT( numBytes <= MAX_I2C_READ_REQUEST );
    BDBG_ASSERT( pData );

    /* Get I2C handle from channel handle */
    hDev = hChn->hI2c;

    BI2C_P_ACQUIRE_MUTEX( hChn );

    /* program slave device's (id) */
    if (chipAddr & 0x0380)              /* 10-bit chip address */
    {
        lval = ((chipAddr & 0x0300) >> 7) | 0xF0;
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CHIP_ADDRESS), lval );
        lval = (uint32_t)(chipAddr & 0xff);
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_DATA_IN0), lval );
        bscRegAddr = BCHP_BSCA_DATA_IN1;
        numWriteBytes++;
    }
    else
    {
        lval = (uint32_t) ((chipAddr & 0x7f) << 1);
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CHIP_ADDRESS), lval );
        bscRegAddr = BCHP_BSCA_DATA_IN0;
    }

    /* Reset i2c enable register */
    BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_IIC_ENABLE), 0 );

    /* program data transfer type */
    lval = BREG_Read32(hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CTL_REG));
    lval &= ~(BCHP_BSCA_CTL_REG_reserved0_MASK | BCHP_BSCA_CTL_REG_DTF_MASK);
    lval |= BI2C_P_eWriteRead ;
    BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CTL_REG), lval );

    if (numSubAddrBytes)    /* read with sub addr, must fill subaddr to transmiting register */
    {
        /* program slave device's register address */
        if (numSubAddrBytes == 3)
        {
            lval = (subAddr >> 16) & 0xff;
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + bscRegAddr), lval );
            bscRegAddr += 4;
            lval = (subAddr >> 8) & 0xff;
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + bscRegAddr), lval );
            bscRegAddr += 4;
            lval = subAddr & 0xff;
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + bscRegAddr), lval );
            bscRegAddr += 4;
            numWriteBytes += 3;
        }
        else if (numSubAddrBytes == 2)
        {
            lval = (subAddr >> 8) & 0xff;
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + bscRegAddr), lval );
            bscRegAddr += 4;
            lval = subAddr & 0xff;
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + bscRegAddr), lval );
            bscRegAddr += 4;
            numWriteBytes += 2;
        }
        else if (numSubAddrBytes == 1)
        {
            lval = subAddr & 0xff;
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + bscRegAddr), lval );
            bscRegAddr += 4;
            numWriteBytes++;
        }

        /* program amount of subaddr bytes to conter register */
        lval = (uint32_t)(numWriteBytes & 0x0f);
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CNT_REG), lval );
    }

    readCmdWord = 0;
    cnt = numBytes;
    bufferIndex = 0;

    if (cnt <= MAX_I2C_READ_REQUEST)
    {
        /* program amount of bytes to read */
        lval = cnt;
        cnt = 0;
        rval = BREG_Read32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CNT_REG));
        rval &= 0x0f;
        rval |= (lval << 4);
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CNT_REG), rval );
    }

    /* start the burst read command */
    BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_IIC_ENABLE),
                BCHP_BSCA_IIC_ENABLE_ENABLE_MASK);

    retCode = BI2C_P_WaitForCompletion(hChn, lval+ (uint32_t)(numWriteBytes & 0x0f));
    if (retCode != BERR_SUCCESS)
        goto done;

    /* copy data to buffer */
    for(i = 0; i < lval; i++)
    {
        pData[bufferIndex++] = (uint8_t)(BREG_Read32 (hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_DATA_OUT0 + (i * 4))));
    }

done:
    BI2C_P_RELEASE_MUTEX( hChn);
    return retCode;
}

BERR_Code BI2C_P_BurstReadCmdNoAck
(
    BI2C_ChannelHandle  hChn,           /* Device channel handle */
    uint16_t        chipAddr,           /* i2c chip address.  this is unshifted */
    uint32_t        subAddr,            /* pointer to register address */
    uint8_t         numSubAddrBytes,    /* number of bytes in register address */
    uint8_t         *pData,             /* storage */
    size_t          numBytes            /* number of bytes to read */
)
{
    BI2C_Handle         hDev;
    BERR_Code           retCode = BERR_SUCCESS;
    uint8_t             numWriteBytes = 0;
    uint32_t            bscRegAddr, bufferIndex, lval = 0, rval, i;
    size_t              cnt;

    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );
    BDBG_ASSERT( numBytes <= MAX_I2C_READ_REQUEST );
    BDBG_ASSERT( pData );

    /* Get I2C handle from channel handle */
    hDev = hChn->hI2c;

    BI2C_P_ACQUIRE_MUTEX( hChn );

    /* program slave device's (id) */
    if (chipAddr & 0x0380)              /* 10-bit chip address */
    {
        lval = ((chipAddr & 0x0300) >> 7) | 0xF0;
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CHIP_ADDRESS), lval );
        lval = (uint32_t)(chipAddr & 0xff);
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_DATA_IN0), lval );
        bscRegAddr = BCHP_BSCA_DATA_IN1;
        numWriteBytes++;
    }
    else
    {
        lval = (uint32_t) ((chipAddr & 0x7f) << 1);
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CHIP_ADDRESS), lval );
        bscRegAddr = BCHP_BSCA_DATA_IN0;
    }

    /* Reset i2c enable register */
    BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_IIC_ENABLE), 0 );

    /* program data transfer type */
    lval = BREG_Read32(hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CTL_REG));
    lval &= ~(BCHP_BSCA_CTL_REG_reserved0_MASK | BCHP_BSCA_CTL_REG_DTF_MASK);
    lval |= BI2C_P_eWriteRead ;
    BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CTL_REG), lval );

    if (numSubAddrBytes)    /* read with sub addr, must issue a write */
    {
        /* program slave device's register address */
        if (numSubAddrBytes == 3)
        {
            lval = (subAddr >> 16) & 0xff;
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + bscRegAddr), lval );
            bscRegAddr += 4;
            lval = (subAddr >> 8) & 0xff;
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + bscRegAddr), lval );
            bscRegAddr += 4;
            lval = subAddr & 0xff;
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + bscRegAddr), lval );
            bscRegAddr += 4;
            numWriteBytes += 3;
        }
        else if (numSubAddrBytes == 2)
        {
            lval = (subAddr >> 8) & 0xff;
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + bscRegAddr), lval );
            bscRegAddr += 4;
            lval = subAddr & 0xff;
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + bscRegAddr), lval );
            bscRegAddr += 4;
            numWriteBytes += 2;
        }
        else if (numSubAddrBytes == 1)
        {
            lval = subAddr & 0xff;
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + bscRegAddr), lval );
            bscRegAddr += 4;
            numWriteBytes++;
        }

        /* program amount of subaddr bytes to conter register */
        lval = (uint32_t)(numWriteBytes & 0x0f);
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CNT_REG), lval );

        /* ignore ack */
        lval = BREG_Read32(hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CTLHI_REG));
        lval |= 0x2;
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CTLHI_REG), lval );
    }

    /*
     * Now perform the read portion
     */

    cnt = numBytes;
    bufferIndex = 0;
    {
        if (cnt <= MAX_I2C_READ_REQUEST)
        {
            /* program amount of bytes to read */
            lval = cnt;
            cnt = 0;
            rval = BREG_Read32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CNT_REG));
            rval &= 0x0f;
            rval |= (lval << 4);
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CNT_REG), rval );
        }

        /* start the burst read command */
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_IIC_ENABLE),
                    BCHP_BSCA_IIC_ENABLE_ENABLE_MASK);

        retCode = BI2C_P_WaitForCompletion(hChn, lval+ (uint32_t)(numWriteBytes & 0x0f));
        if (retCode != BERR_SUCCESS)
            goto done;

        /* copy data to buffer */
        for(i = 0; i < lval; i++)
        {
            pData[bufferIndex++] = (uint8_t)(BREG_Read32 (hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_DATA_OUT0 + (i * 4))));
        }
    }

done:
    BI2C_P_RELEASE_MUTEX( hChn );
    return retCode;
}

#ifdef BI2C_SUPPORT_4_BYTE_XFR_MODE
/***********************************************************************func
 * Name: BI2C_P_ReadBy4BytesCmd
 *   - Read data from i2c slave device.
 *       write (1 byte of slave device register's address, subAddr)
 *   and read  (0-8 bytes of data)
 *   Using i2c Write.& Read
 *
 * NOTE: This function will be called by BI2C_P_Read, BI2C_P_ReadA16, BI2C_P_ReadNoAddr
 ***************************************************************************/
BERR_Code BI2C_P_ReadBy4BytesCmd
(
    BI2C_ChannelHandle  hChn,               /* Device channel handle */
    uint16_t            chipAddr,           /* i2c chip address.  this is unshifted */
    uint32_t            subAddr,            /* sub address */
    uint8_t             numSubAddrBytes,    /* number of bytes in register address */
    uint8_t             *pData,             /* storage */
    size_t              numBytes,           /* number of bytes to read */
    bool                mutex,              /* protect with a mutex */
    bool                ack,                /* check for ack? */
    bool                noStop              /* no stop at the end? */
)
{
    BI2C_Handle         hDev;
    BERR_Code           retCode = BERR_SUCCESS;
    uint8_t             numWriteBytes = 0;
    uint8_t             maxReadBytes;
    uint8_t             numReadBytes;
    uint32_t            tempData;
    uint32_t            bscRegAddr, readCmdWord, bufferIndex, lval = 0, i;
    size_t              cnt;

    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    /* Get I2C handle from channel handle */
    hDev = hChn->hI2c;

    if (mutex)
        BI2C_P_ACQUIRE_MUTEX( hChn );

    maxReadBytes = MAX_I2C_READ_REQUEST*4;

    /* program slave device's (id) */
    if (chipAddr & 0x0380)              /* 10-bit chip address */
    {
        lval = ((chipAddr & 0x0300) >> 7) | 0xF0;
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CHIP_ADDRESS), lval );
        lval = (uint32_t)(chipAddr & 0xff);
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_DATA_IN0), lval );

        /* There are still 3 bytes we can write in BCHP_BSCA_DATA_IN0 */
        bscRegAddr = BCHP_BSCA_DATA_IN0;
        numWriteBytes++;
    }
    else
    {
        lval = (uint32_t) ((chipAddr & 0x7f) << 1);
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CHIP_ADDRESS), lval );
        bscRegAddr = BCHP_BSCA_DATA_IN0;
    }

    if (numSubAddrBytes)    /* read with sub addr, must issue a write */
    {
        /* Each BCHP_BSCA_DATA_IN register has 4 bytes */
        if (numSubAddrBytes == 3)
        {
            if (numWriteBytes == 0)
            {
                lval = (subAddr & 0xff0000) >> 16;
                lval |= (subAddr & 0x00ff00);
                lval |= ((subAddr & 0x0000ff) << 16);
            }
            else
            {
                /* byte 0 contains part of chip address. */
                lval = (uint32_t)((chipAddr & 0xff) | ((subAddr & 0xff) << 8));
                lval |= ((subAddr & 0xff0000) >> 8);
                lval |= ((subAddr & 0x00ff00) << 8);
                lval |= ((subAddr & 0x0000ff) << 16);
            }
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + bscRegAddr), lval );
            bscRegAddr += 4;
            numWriteBytes += 3;
        }
        else if (numSubAddrBytes == 2)
        {
            if (numWriteBytes == 0)
            {
                lval = ((subAddr & 0xff00) >> 8);
                lval |= ((subAddr & 0xff) << 8);
            }
            else
            {
                /* byte 0 contains part of chip address. */
                lval = (chipAddr & 0xff);
                lval |= ((subAddr & 0xff00) << 8);
                lval |= ((subAddr & 0x00ff) << 16);
            }
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + bscRegAddr), lval );
            numWriteBytes += 2;
        }
        else if (numSubAddrBytes == 1)
        {
            if (numWriteBytes == 0)
            {
                lval = subAddr & 0xff;
            }
            else
            {
                /* byte 0 contains part of chip address. */
                lval = (chipAddr & 0xff);
                lval |= ((subAddr & 0xff) << 8);
            }
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + bscRegAddr), lval );
            numWriteBytes++;
        }

        /* program amount of bytes to write/read */
        lval = (uint32_t)(numWriteBytes & 0x0f);
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CNT_REG), lval );

        /* program data transfer type */
        lval = BREG_Read32(hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CTL_REG));
        lval &= ~(BCHP_BSCA_CTL_REG_reserved0_MASK | BCHP_BSCA_CTL_REG_DTF_MASK);
        lval |= BI2C_P_eWriteOnly ;
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CTL_REG), lval );

        if (!ack)
        {
            /* Mao Neil: ignore ack */
            /* printf("Disable the acknowledge!\n\n"); */
            lval = BREG_Read32(hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CTLHI_REG));
            lval |= BCHP_BSCA_CTLHI_REG_IGNORE_ACK_MASK;
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CTLHI_REG), lval );
        }

        /* start the write command */
        lval = (BCHP_BSCA_IIC_ENABLE_RESTART_MASK | BCHP_BSCA_IIC_ENABLE_NO_STOP_MASK |
                BCHP_BSCA_IIC_ENABLE_ENABLE_MASK);
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_IIC_ENABLE), lval );

        retCode = BI2C_P_WaitForCompletion(hChn, numWriteBytes);
        if (retCode != BERR_SUCCESS)
            goto done;
    }

    /*
     * Now perform the read portion
     */

    /* program data transfer type */
    lval = BREG_Read32(hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CTL_REG));
    lval &= ~(BCHP_BSCA_CTL_REG_reserved0_MASK | BCHP_BSCA_CTL_REG_DTF_MASK);
    lval |= BI2C_P_eReadOnly;
    BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CTL_REG), lval );

    readCmdWord = 0;
    cnt = numBytes;
    bufferIndex = 0;

    while (cnt)
    {
        if (cnt <= maxReadBytes)
        {
            /* program amount of bytes to read */
            lval = cnt;
            cnt = 0;
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CNT_REG), lval );
            if (noStop) {
                readCmdWord |= (BCHP_BSCA_IIC_ENABLE_NO_STOP_MASK | BCHP_BSCA_IIC_ENABLE_RESTART_MASK);
            }
        }
        else
        {
            /* program amount of bytes to read */
            lval = maxReadBytes;
            cnt -= maxReadBytes;
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CNT_REG), lval );
            readCmdWord |= BCHP_BSCA_IIC_ENABLE_NO_STOP_MASK;
        }
        /* start the read command */
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_IIC_ENABLE),
                    (readCmdWord | BCHP_BSCA_IIC_ENABLE_ENABLE_MASK));

        retCode = BI2C_P_WaitForCompletion(hChn, lval);
        if (retCode != BERR_SUCCESS)
            goto done;

        /* copy data to buffer */
        /* Read operation is by 4 bytes */
        for(i = 0; i < ((lval-1)/4)+1; i++)
        {
            tempData = BREG_Read32 (hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_DATA_OUT0 + (i * 4)));

            if (i == ((lval-1)/4))
            {
                numReadBytes = ((lval-1)%4) + 1; /* last read may have less than 4 bytes */
            }
            else
            {
                numReadBytes = 4;
            }

            pData[bufferIndex++] = (uint8_t)(tempData & 0x000000FF);
            if ((numReadBytes == 2) || (numReadBytes == 3) || (numReadBytes == 4))
            {
                pData[bufferIndex++] = (uint8_t)((tempData & 0x0000FF00) >> 8);
            }
            if ((numReadBytes == 3) || (numReadBytes == 4))
            {
                pData[bufferIndex++] = (uint8_t)((tempData & 0x00FF0000) >> 16);
            }
            if (numReadBytes == 4)
            {
                pData[bufferIndex++] = (uint8_t)((tempData & 0xFF000000) >> 24);
            }
        }

        if (cnt)
        {
            readCmdWord = BCHP_BSCA_IIC_ENABLE_NO_START_MASK;
        }
    }

done:
    if (mutex)
        BI2C_P_RELEASE_MUTEX( hChn );

    BDBG_MSG(("BI2C_P_ReadBy4BytesCmd:  ch=%d, clkRate=%d, chipAddr=0x%x, numSubAddrBytes=%d SubAddr=0x%x, numBytes=%d, pData=0x%x", hChn->chnNo, hChn->clkRate, chipAddr, numSubAddrBytes, subAddr, numBytes, *(uint8_t *)pData));
    #ifdef BI2C_DEBUG
    {
        int i;
        for (i=0; i<(int)numBytes; i++)
            BDBG_MSG(("%d:  0x%x", i, *(pData+i)));
    }
    #endif

    return retCode;
}
#endif

/***********************************************************************func
* Name: BI2C_P_WaitForNVMToAcknowledge
 *   -
 *
 *
 ***************************************************************************/
BERR_Code BI2C_P_WaitForNVMToAcknowledge
(
    BI2C_ChannelHandle  hChn,                 /* Device channel handle */
    uint16_t                 chipAddr,            /* i2c chip address.  this is unshifted */
    uint32_t                 subAddr,       /* pointer to register address */
    uint8_t                  numSubAddrBytes,   /* number of bytes in register address */
    size_t                   numBytes            /* number of bytes to write */
   )
{
    BERR_Code   retCode   = BERR_SUCCESS;
    uint32_t        lval      = 0;
   uint32_t     timeoutMs = 0;
   uint32_t     loopCnt   = 0;
   uint8_t     myData[1] = { 0 };

   /*
    * method
    * 1) Set up timout values as appropriate
    * 2) Make a call to Read, reading some abitrary address
    * 3) If Read fails, check timeout hasn't been reached.
    * 4) If not timeout, then re-call Read.
   */

    /* Calculate the timeout value */
    numBytes++;                         /* add 1 for chip ID */
    lval      = numBytes * 4000;    /* number of clks, 4000 clks per byte worst case scenario */
    timeoutMs = BI2C_P_CALCULATE_TIMEOUT(lval, hChn->clkRate);

#ifdef CK_DEBUG
   ATE_printf( "numbytes  = %d\n\r", numBytes );
   ATE_printf( "lval      = %d\n\r", lval );
   ATE_printf( "timeoutMs = %d\n\r", timeoutMs );
#endif

   loopCnt = ((timeoutMs * 1000) / I2C_POLLING_INTERVAL) + 1;

   hChn->nvramAck = 1;

   while (1)
   {
      retCode = BI2C_P_ReadCmd( hChn, chipAddr, subAddr, numSubAddrBytes, &myData[0], 1, true /*mutex*/, true /*ack*/, false /*no stop*/);

      if ( retCode != BERR_SUCCESS )
      {
         BKNI_Delay( I2C_POLLING_INTERVAL );

         loopCnt--;

         if (loopCnt == 0)
         {
            retCode = BERR_TRACE( BERR_TIMEOUT );
            break;
         }
      }
      else
      {
         /* NVM chip responded so it is no longer in its internal
          * write cycle. Can now finish
          */
         break;
      }
   }

   hChn->nvramAck = 0;

   return retCode;
}

BERR_Code BI2C_P_WaitForDataToLeaveFIFO
(
    BI2C_ChannelHandle  hChn,                           /* Device channel handle */
    uint32_t                 numBytes                       /* number of bytes to transfer */
)
{
    BI2C_Handle hDev      = hChn->hI2c;
    BERR_Code   retCode   = BERR_SUCCESS;
    uint32_t        lval      = 0;
    uint32_t    timeoutMs = 0;
    uint32_t    loopCnt   = 0;

    /* Calculate the timeout value */

    numBytes++;                         /* add 1 for chip ID */

    lval = numBytes * 4000;         /* number of clks, 4000 clks per byte worst case scenario */

    timeoutMs = BI2C_P_CALCULATE_TIMEOUT(lval, hChn->clkRate);

    loopCnt = ((timeoutMs * 1000) / I2C_POLLING_INTERVAL) + 1;

    while (1)
    {
        lval = BREG_Read32(hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_IIC_ENABLE));

        if (lval & BCHP_BSCA_IIC_ENABLE_INTRP_MASK)
        {
         /* Ladies and gentleman, DATA HAS NOW LEFT THE FIFO!!!!
          * We can now finish
          */
            break;
        }

        if (loopCnt == 0)
        {
            retCode = BERR_TRACE (BERR_TIMEOUT);
            break;
        }

        BKNI_Delay(I2C_POLLING_INTERVAL);

        loopCnt--;
   }

   /* With respect to the NVM, we do not want to check if there has been
    * an ack to the data leaving the FIFO, because there probably wont be */

    /* stop execution */

    lval = BREG_Read32(hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_IIC_ENABLE));

    lval &= ~(BCHP_BSCA_IIC_ENABLE_ENABLE_MASK);

    BREG_Write32(hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_IIC_ENABLE), lval);

    return retCode;
}

/***********************************************************************func
 * Name: BI2C_SwReset
 *   - Perform a software reset.
 *
 ***************************************************************************/
BERR_Code BI2C_SwReset
(
    BI2C_ChannelHandle  hChn           /* Device channel handle */
)
{
    #if (BI2C_SUPPORT_I2C_VIA_GPIO == 1)
    uint16_t i;
    uint8_t oden_scl, oden_sda;
    uint8_t iodir_scl, iodir_sda;
    uint8_t sun_top_sda, sun_top_scl;
    int no_ack = 0;
    int ch = hChn->chnNo;
    BI2C_Handle hDev;
    BREG_Handle hReg;
    BI2C_Clk rate = hChn->clkRate;

    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    /* Get I2C handle from channel handle */
    hDev = hChn->hI2c;
    hReg = hDev->hRegister;

    sun_top_scl = BI2C_P_SunTopScl(0);
    sun_top_sda = BI2C_P_SunTopSda(0);

    /* Set open drain output */
    oden_scl = BI2C_P_OdenScl(1);
    oden_sda = BI2C_P_OdenSda(1);

    /* reset CLOCK and DATA */
    BI2C_P_ClkSet(1);
    BI2C_P_DataSet(1);

    /* Set gpio as output */
    iodir_scl = BI2C_P_IodirScl(0);
    iodir_sda = BI2C_P_IodirSda(0);

    /* generate start condition */
    BI2C_P_DataSet(0);
    BI2C_P_ClkSet(0);
    BI2C_P_DataSet(1);

    /* dummy clock cycles 1 through 9 */
    for (i=0; i<9; i++)
    {
        BI2C_P_ClkSet(1);
        BI2C_P_ClkSet(0);
    }

    /* start condition */
    BI2C_P_ClkSet(1);
    BI2C_P_DataSet(0);

    /* stop condition */
    BI2C_P_ClkSet(0);
    BI2C_P_ClkSet(1);
    BI2C_P_DataSet(1);
    BI2C_P_ClkSet(0);	/* Needed ????*/

    /* restore iodir */
    BI2C_P_IodirScl(iodir_scl);
    BI2C_P_IodirSda(iodir_sda);

    /* restore open drain output */
    BI2C_P_OdenScl(oden_scl);
    BI2C_P_OdenSda(oden_sda);

    /* restore sun top */
    BI2C_P_SunTopScl(sun_top_scl);
    BI2C_P_SunTopSda(sun_top_sda);

    if (no_ack) return BI2C_ERR_NO_ACK;
    else return BERR_SUCCESS;
    #else
    BSTD_UNUSED( hChn );
    BDBG_ERR(("BI2C_P_WriteSwCmd called, but not currently supported.  Make sure the function BI2C_P_GpioInit() is setup properly for this chip.\n"));
    return BERR_NOT_SUPPORTED;
    #endif
}

/***********************************************************************func
 * Name: BI2C_P_Write
 *   - Write data to an i2c slave device.
 *
 * NOTE: This function will be called by BI2C_P_Write, BI2C_P_WriteA16, BI2C_P_WriteNoAddr
 ***************************************************************************/
BERR_Code BI2C_P_WriteSwCmd
(
    BI2C_ChannelHandle  hChn,           /* Device channel handle */
    uint16_t            chipAddr,       /* i2c chip address.  this is unshifted */
    uint32_t            subAddr,        /* pointer to register address */
    uint8_t             numSubAddrBytes,    /* number of bytes in register address */
    const uint8_t       *pData,     /* storage */
    size_t              numBytes,       /* number of bytes to write */
    bool                isNvram         /* is this a nvram access? */
)
{
    #if (BI2C_SUPPORT_I2C_VIA_GPIO == 1)
    uint16_t i;
    uint8_t oden_scl, oden_sda;
    uint8_t iodir_scl, iodir_sda;
    uint8_t sun_top_sda, sun_top_scl;
    int no_ack = 0;
    int ch = hChn->chnNo;
    BI2C_Handle hDev;
    BREG_Handle hReg;
    BI2C_Clk rate = hChn->clkRate;
    int ret_ack;
    BERR_Code rc;
    uint32_t lval = 0;

    BDBG_MSG(("BI2C_P_WriteSwCmd:  ch=%d, clkRate=%d, chipAddr=0x%x, numSubAddrBytes=%d, subAddr=0x%x, numBytes=%d, pData=0x%x\n", hChn->chnNo, hChn->clkRate, chipAddr, numSubAddrBytes, subAddr, numBytes, *(uint8_t *)pData));

    BSTD_UNUSED( isNvram );
    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );
    BDBG_ASSERT( pData );

    /* Get I2C handle from channel handle */
    hDev = hChn->hI2c;
    hReg = hDev->hRegister;

    sun_top_scl = BI2C_P_SunTopScl(0);
    sun_top_sda = BI2C_P_SunTopSda(0);

    /* Set open drain output */
    oden_scl = BI2C_P_OdenScl(1);
    oden_sda = BI2C_P_OdenSda(1);

    #ifdef RMW_METHOD
    /* reset CLOCK and DATA */
    BI2C_P_ClkSet(1);
    BI2C_P_DataSet(1);

    /* Set gpio as output */
    iodir_scl = BI2C_P_IodirScl(0);
    iodir_sda = BI2C_P_IodirSda(0);
    #else
    /* Set iodir to output so we can change data */
    iodir_scl = BI2C_P_IodirScl(0);
    iodir_sda = BI2C_P_IodirSda(0);

    /* Set data low for using iodir to control output */
    BI2C_P_DataScl(0);
    BI2C_P_DataSda(0);

    /* Set iodir to input so we can start with SCL and SDA high */
    BI2C_P_IodirScl(1);
    BI2C_P_IodirSda(1);
    #endif

    /* Don't support 10 bit chip addr yet */
    if (chipAddr & 0x0380)              /* 10-bit chip address */
    {
        BDBG_ERR(("not supported yet...\n"));
        while(1);
    }

#if 0
    /* Don't support sub address bytes yet */
    if (numSubAddrBytes > 1)
    {
        BDBG_ERR(("not supported yet...\n"));
        while(1);
    }
#endif

    chipAddr = (chipAddr & 0x7f) << 1;

    /* generate start condition */
    BI2C_P_DataSet(0);

    if ((rc = BI2C_P_ByteWrite(chipAddr & 0xfe, &ret_ack)) != BERR_SUCCESS) goto no_success;
    if (ret_ack != I2C_ACK)
    {
        BDBG_ERR(("no ack!\n"));
        no_ack = 1;
        goto no_ack_detected;
    }

#if 0
    if (numSubAddrBytes==1)
    {
        if ((rc = BI2C_P_ByteWrite(*((uint8_t *)pSubAddr), &ret_ack)) != BERR_SUCCESS) goto no_success;
        if (ret_ack != I2C_ACK)
        {
            BDBG_ERR(("no ack!\n"));
            no_ack = 1;
            goto no_ack_detected;
        }
    }
#else
    if (numSubAddrBytes)    /* read with sub addr, must issue a write */
    {
        /* program slave device's register address */
        if (numSubAddrBytes == 3)
        {
            lval = (subAddr >> 16) & 0xff;
            if ((rc = BI2C_P_ByteWrite(lval, &ret_ack)) != BERR_SUCCESS) goto no_success;
            if (ret_ack != I2C_ACK)
            {
                BDBG_ERR(("no ack!\n"));
                no_ack = 1;
                goto no_ack_detected;
            }
            lval = (subAddr >> 8) & 0xff;
            if ((rc = BI2C_P_ByteWrite(lval, &ret_ack)) != BERR_SUCCESS) goto no_success;
            if (ret_ack != I2C_ACK)
            {
                BDBG_ERR(("no ack!\n"));
                no_ack = 1;
                goto no_ack_detected;
            }
            lval = subAddr & 0xff;
            if ((rc = BI2C_P_ByteWrite(lval, &ret_ack)) != BERR_SUCCESS) goto no_success;
            if (ret_ack != I2C_ACK)
            {
                BDBG_ERR(("no ack!\n"));
                no_ack = 1;
                goto no_ack_detected;
            }
        }
        else if (numSubAddrBytes == 2)
        {
            lval = (subAddr >> 8) & 0xff;
            if ((rc = BI2C_P_ByteWrite(lval, &ret_ack)) != BERR_SUCCESS) goto no_success;
            if (ret_ack != I2C_ACK)
            {
                BDBG_ERR(("no ack!\n"));
                no_ack = 1;
                goto no_ack_detected;
            }
            lval = subAddr & 0xff;
            if ((rc = BI2C_P_ByteWrite(lval, &ret_ack)) != BERR_SUCCESS) goto no_success;
            if (ret_ack != I2C_ACK)
            {
                BDBG_ERR(("no ack!\n"));
                no_ack = 1;
                goto no_ack_detected;
            }
        }
        else if (numSubAddrBytes == 1)
        {
            lval = subAddr & 0xff;
            if ((rc = BI2C_P_ByteWrite(lval, &ret_ack)) != BERR_SUCCESS) goto no_success;
            if (ret_ack != I2C_ACK)
            {
                BDBG_ERR(("no ack!\n"));
                no_ack = 1;
                goto no_ack_detected;
            }
        }
    }
#endif

    for (i = 0; i < numBytes; i++)
    {
        if ((rc = BI2C_P_ByteWrite(pData[i], &ret_ack) != BERR_SUCCESS)) goto no_success;
        if (ret_ack != I2C_ACK)
        {
            BDBG_ERR(("no ack!\n"));
            no_ack = 1;
            goto no_ack_detected;
        }
    }

no_success:
no_ack_detected:
    /* generate stop condition */
    BI2C_P_ClkSet(1);
    BI2C_P_DataSet(1);

    /* restore iodir */
    BI2C_P_IodirScl(iodir_scl);
    BI2C_P_IodirSda(iodir_sda);

    /* restore open drain output */
    BI2C_P_OdenScl(oden_scl);
    BI2C_P_OdenSda(oden_sda);

    /* restore sun top */
    BI2C_P_SunTopScl(sun_top_scl);
    BI2C_P_SunTopSda(sun_top_sda);

    if (no_ack) return BI2C_ERR_NO_ACK;
    else return BERR_SUCCESS;
    #else
    BSTD_UNUSED( hChn );
    BSTD_UNUSED( chipAddr );
    BSTD_UNUSED( subAddr );
    BSTD_UNUSED( numSubAddrBytes );
    BSTD_UNUSED( pData );
    BSTD_UNUSED( numBytes );
    BSTD_UNUSED( isNvram );
    BDBG_ERR(("BI2C_P_WriteSwCmd called, but not currently supported.  Make sure the function BI2C_P_GpioInit() is setup properly for this chip.\n"));
    return BERR_NOT_SUPPORTED;
    #endif
}

/***********************************************************************func
 * Name: BI2C_P_Write
 *   - Write data to an i2c slave device.
 *
 * NOTE: This function will be called by BI2C_P_Write, BI2C_P_WriteA16, BI2C_P_WriteNoAddr
 ***************************************************************************/
BERR_Code BI2C_P_WriteCmd
(
    BI2C_ChannelHandle  hChn,           /* Device channel handle */
    uint16_t            chipAddr,       /* i2c chip address.  this is unshifted */
    uint32_t            subAddr,      /* pointer to register address */
    uint8_t             numSubAddrBytes,    /* number of bytes in register address */
    const uint8_t       *pData,     /* storage */
    size_t              numBytes,       /* number of bytes to write */
    bool                isNvram,         /* is this a nvram access? */
    bool                mutex,           /* protect with a mutex */
    bool                ack,             /* check for ack? */
    bool                noStop           /* Do we need a no stop at the end of the transfer? */
)
{
    BI2C_Handle         hDev;
    BERR_Code           retCode = BERR_SUCCESS;
    BERR_Code           rc = BERR_SUCCESS;
    uint8_t             numWriteBytes = 0;
    uint32_t            bscRegAddr, writeCmdWord, bufferIndex, maxTxfrSize, lval = 0, i;
    size_t              cnt;

    BDBG_MSG(("BI2C_P_WriteCmd:  ch=%d, clkRate=%d, chipAddr=0x%x, numSubAddrBytes=%d SubAddr=0x%x, numBytes=%d, pData=0x%x", hChn->chnNo, hChn->clkRate, chipAddr, numSubAddrBytes, subAddr, numBytes, *(uint8_t *)pData));
    #ifdef BI2C_DEBUG
    {
        int i;
        for (i=0; i<(int)numBytes; i++)
            BDBG_MSG(("%d:  0x%x", i, *(pData+i)));
    }
    #endif

    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );
    BDBG_ASSERT( pData );

    /* Get I2C handle from channel handle */
    hDev = hChn->hI2c;

    if (mutex)
        BI2C_P_ACQUIRE_MUTEX( hChn );

    /* program slave device's (id) */
    if (chipAddr & 0x0380)              /* 10-bit chip address */
    {
        lval = ((chipAddr & 0x0300) >> 7) | 0xF0;
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CHIP_ADDRESS), lval );
        lval = (uint32_t)(chipAddr & 0xff);
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_DATA_IN0), lval );
        bscRegAddr = BCHP_BSCA_DATA_IN1;
        numWriteBytes++;
    }
    else
    {
        lval = (uint32_t) ((chipAddr & 0x7f) << 1);
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CHIP_ADDRESS), lval );
        bscRegAddr = BCHP_BSCA_DATA_IN0;
    }

    if (numSubAddrBytes)    /* read with sub addr, must issue a write */
    {
        /* program slave device's register address */
        if (numSubAddrBytes == 3)
        {
            lval = (subAddr >> 16) & 0xff;
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + bscRegAddr), lval );
            bscRegAddr += 4;
            lval = (subAddr >> 8) & 0xff;
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + bscRegAddr), lval );
            bscRegAddr += 4;
            lval = subAddr & 0xff;
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + bscRegAddr), lval );
            bscRegAddr += 4;
            numWriteBytes += 3;
        }
        else if (numSubAddrBytes == 2)
        {
            lval = (subAddr >> 8) & 0xff;
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + bscRegAddr), lval );
            bscRegAddr += 4;
            lval = subAddr & 0xff;
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + bscRegAddr), lval );
            bscRegAddr += 4;
            numWriteBytes += 2;
        }
        else if (numSubAddrBytes == 1)
        {
            lval = subAddr & 0xff;
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + bscRegAddr), lval );
            bscRegAddr += 4;
            numWriteBytes++;
        }
    }

    /* program data transfer type */
    lval = BREG_Read32(hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CTL_REG));
    lval &= ~(BCHP_BSCA_CTL_REG_reserved0_MASK | BCHP_BSCA_CTL_REG_DTF_MASK);
    lval |= BI2C_P_eWriteOnly ;
    BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CTL_REG), lval );

    maxTxfrSize = MAX_I2C_WRITE_REQUEST - numWriteBytes;
    writeCmdWord = 0;
    cnt = numBytes;
    bufferIndex = 0;

    do
    {
        if (cnt <= maxTxfrSize)
        {
            lval = cnt;
            cnt = 0;
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CNT_REG), (lval + numWriteBytes) );
            if (noStop) {
                writeCmdWord |= (BCHP_BSCA_IIC_ENABLE_NO_STOP_MASK | BCHP_BSCA_IIC_ENABLE_RESTART_MASK);
            }
        }
        else
        {
            lval = maxTxfrSize;
            cnt -= maxTxfrSize;
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CNT_REG), (lval + numWriteBytes) );

            writeCmdWord |= BCHP_BSCA_IIC_ENABLE_NO_STOP_MASK;

        }

        /* copy data from buffer */
        for(i = 0; i < lval; i++)       /* lval is number of actual user data bytes to send */
        {
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + bscRegAddr ), (uint32_t)(pData[bufferIndex++]));
            bscRegAddr += 4;
        }

        if (!ack)
        {
            /* Mao Neil: ignore ack */
            /* printf("Disable the acknowledge!\n\n"); */
            lval = BREG_Read32(hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CTLHI_REG));
            lval |= 0x2;
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CTLHI_REG), lval );
        }

        /* start the write command */
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_IIC_ENABLE),
                    (writeCmdWord | BCHP_BSCA_IIC_ENABLE_ENABLE_MASK));

        if (isNvram)
        {
            retCode = BI2C_P_WaitForDataToLeaveFIFO(hChn, (lval + numWriteBytes));

            if ( writeCmdWord & BCHP_BSCA_IIC_ENABLE_NO_STOP_MASK )
            {
                /* There is a no stop */
            }
            else
            {
                /* If the master is sending the stop command, then
                 * this signals the nvm device to begin its internal write
                 * cylce. Now would be a good time to poll for its
                 * completion
                 */

                retCode = BI2C_P_WaitForNVMToAcknowledge( hChn,
                                                                chipAddr,
                                                                subAddr,
                                                                numSubAddrBytes,
                                                                numBytes );
            }
            if (retCode != BERR_SUCCESS)
                goto done;
        }
        else
        {
            retCode = BI2C_P_WaitForCompletion(hChn, (lval + numWriteBytes));
            if (retCode != BERR_SUCCESS)
            {
                #if 0
                /* We will have to quit.  But if no ack, at least send the stop condition. */
                if (((retCode == BI2C_ERR_NO_ACK) || (retCode == BERR_OS_ERROR)) && (writeCmdWord & BCHP_BSCA_IIC_ENABLE_NO_STOP_MASK))
                {
                    /* Finish the transaction and then quit */
                    BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CNT_REG), 0 );
                    BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_IIC_ENABLE),
                                (BCHP_BSCA_IIC_ENABLE_NO_START_MASK | BCHP_BSCA_IIC_ENABLE_ENABLE_MASK));
                    rc = BI2C_P_WaitForCompletion(hChn, 1);
                    if (rc != BERR_SUCCESS)
                        goto done;
                }
                #else
                if (retCode == BERR_OS_ERROR)
                {
                    /* Finish the transaction and then quit */
                    BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CNT_REG), 0 );
                    BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_IIC_ENABLE),
                                (BCHP_BSCA_IIC_ENABLE_NO_START_MASK | BCHP_BSCA_IIC_ENABLE_ENABLE_MASK));
                    rc = BI2C_P_WaitForCompletion(hChn, 1);
                    if (rc != BERR_SUCCESS)
                        goto done;
                }
                #endif
                goto done;
            }
        }

        if (cnt)
        {
            writeCmdWord = BCHP_BSCA_IIC_ENABLE_NO_START_MASK;
            bscRegAddr = BCHP_BSCA_DATA_IN0;
            maxTxfrSize = MAX_I2C_WRITE_REQUEST;
            numWriteBytes  = 0;
        }
    }
    while (cnt);

done:
    if (mutex)
        BI2C_P_RELEASE_MUTEX( hChn );
    return retCode;
}

#ifdef BI2C_SUPPORT_4_BYTE_XFR_MODE
/***********************************************************************func
 * Name: BI2C_P_WriteBy4BytesCmd
 *   - Write data to an i2c slave device.
 *
 * NOTE: This function will be called by BI2C_P_Write, BI2C_P_WriteA16, BI2C_P_WriteNoAddr
 ***************************************************************************/
BERR_Code BI2C_P_WriteBy4BytesCmd
(
    BI2C_ChannelHandle  hChn,           /* Device channel handle */
    uint16_t            chipAddr,       /* i2c chip address.  this is unshifted */
    uint32_t            subAddr,        /* sub address */
    uint8_t             numSubAddrBytes,    /* number of bytes in register address */
    const uint8_t       *pData,         /* storage */
    size_t              numBytes,       /* number of bytes to write */
    bool                isNvram,        /* is this a nvram access? */
    bool                mutex,           /* protect with a mutex */
    bool                ack,            /* check for ack? */
    bool                noStop          /* do we need a stop at the end of the transfer */
)
{
    BI2C_Handle         hDev;
    BERR_Code           retCode = BERR_SUCCESS;
    uint8_t             numWriteBytes = 0;
    uint8_t             maxWriteBytes;
    uint32_t            bscRegAddr, writeCmdWord, bufferIndex, lval = 0;
    size_t              cnt;

    BDBG_ASSERT( hChn );
    BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

    BDBG_MSG(("BI2C_P_WriteBy4BytesCmd:  ch=%d, clkRate=%d, chipAddr=0x%x, numSubAddrBytes=%d SubAddr=0x%x, numBytes=%d, pData=0x%x", hChn->chnNo, hChn->clkRate, chipAddr, numSubAddrBytes, subAddr, numBytes, *(uint8_t *)pData));
    #ifdef BI2C_DEBUG
    {
        int i;
        for (i=0; i<(int)numBytes; i++)
            BDBG_MSG(("%d:  0x%x", i, *(pData+i)));
    }
    #endif

    /* Get I2C handle from channel handle */
    hDev = hChn->hI2c;

    if (mutex)
        BI2C_P_ACQUIRE_MUTEX( hChn );

    maxWriteBytes = MAX_I2C_WRITE_REQUEST*4;

    /* program slave device's (id) */
    if (chipAddr & 0x0380)              /* 10-bit chip address */
    {
        lval = ((chipAddr & 0x0300) >> 7) | 0xF0;
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CHIP_ADDRESS), lval );
        numWriteBytes++;
    }
    else
    {
        lval = (uint32_t) ((chipAddr & 0x7f) << 1);
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CHIP_ADDRESS), lval );
    }

    numWriteBytes += numSubAddrBytes;

    /* program data transfer type */
    lval = BREG_Read32(hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CTL_REG));
    lval &= ~(BCHP_BSCA_CTL_REG_reserved0_MASK | BCHP_BSCA_CTL_REG_DTF_MASK);
    lval |= BI2C_P_eWriteOnly ;
    BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CTL_REG), lval );

    writeCmdWord = 0;
    cnt = numBytes;
    bufferIndex = 0;
    bscRegAddr = BCHP_BSCA_DATA_IN0;
    
    do
    {
        if (cnt+numWriteBytes <= maxWriteBytes)
        {
            lval = cnt+numWriteBytes;
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CNT_REG), lval );
            if (noStop) {
                writeCmdWord |= (BCHP_BSCA_IIC_ENABLE_NO_STOP_MASK | BCHP_BSCA_IIC_ENABLE_RESTART_MASK);
            }
        }
        else
        {
            lval = maxWriteBytes;
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CNT_REG), lval );
            writeCmdWord |= BCHP_BSCA_IIC_ENABLE_NO_STOP_MASK;
        }
        
        /* copy data from buffer */
        if (lval) /* lval may be zero because of a zero byte write */
        {
            uint32_t i;
            for(i = 0; i < ((lval-1)/4)+1; i++)
            {
                uint32_t j;
                uint32_t tempData=0;
                switch (numWriteBytes)
                {
                    case 0:
                        for (j=0; (j<4) && (j<cnt); j++)
                        {
                            tempData |= (uint32_t)((pData[bufferIndex+j]<<(8*j)));
                        }
                        cnt-=j;
                        bufferIndex += 4;
                        break;
                    case 1:
                        if (chipAddr & 0x0380)              /* 10-bit chip address */
                        {
                            tempData = (uint32_t)(chipAddr & 0xff);
                        }
                        else
                        {
                            tempData = subAddr;
                        }
                        for (j=0; (j<3) && (j<cnt); j++)
                        {
                            tempData |= (uint32_t)((pData[bufferIndex+j]<<(8*(j+1))));
                        }
                        cnt-=j;
                        bufferIndex += 3;
                        break;
                    case 2:
                        if (chipAddr & 0x0380)              /* 10-bit chip address */
                        {
                            tempData = (uint32_t)(chipAddr & 0x000000ff);
                            tempData |= ((subAddr << 8)    & 0x0000ff00);
                        }
                        else
                        {
                            tempData = (subAddr >> 8)      & 0x000000ff;
                            tempData |= ((subAddr << 8)    & 0x0000ff00);
                        }
                        for (j=0; (j<2) && (j<cnt); j++)
                        {
                            tempData |= (uint32_t)((pData[bufferIndex+j]<<(8*(j+2))));
                        }
                        cnt-=j;
                        bufferIndex += 2;
                        break;
                    case 3:
                        if (chipAddr & 0x0380)              /* 10-bit chip address */
                        {
                            tempData = (uint32_t)(chipAddr & 0x000000ff);
                            tempData |= (subAddr)          & 0x0000ff00;
                            tempData |= (subAddr << 16)    & 0x00ff0000;
                        }
                        else
                        {
                            tempData = ((subAddr >> 16)    & 0x000000ff);
                            tempData |= (subAddr           & 0x0000ff00);
                            tempData |= (subAddr << 16)    & 0x00ff0000;
                        }
                        for (j=0; (j<1) && (j<cnt); j++)
                        {
                            tempData |= (uint32_t)((pData[bufferIndex+j]<<(8*(j+3))));
                        }
                        cnt-=j;
                        bufferIndex += 1;
                        break;
                    case 4:
                        if (chipAddr & 0x0380)              /* 10-bit chip address */
                        {
                            tempData = (uint32_t)(chipAddr & 0x000000ff);
                            tempData |= ((subAddr >> 8)    & 0x0000ff00);
                            tempData |= ((subAddr << 8)    & 0x00ff0000);
                            tempData |= ((subAddr << 24)   & 0xff000000);
                        }
                        break;
                    default:
                        break;
                }
                                
                numWriteBytes=0; /* done taking care of sub address */
                BREG_Write32( hDev->hRegister, (hChn->coreOffset + bscRegAddr ), tempData);
                bscRegAddr += 4;
            }
        }

        if (!ack)
        {
            /* Mao Neil: ignore ack */
            /* printf("Disable the acknowledge!\n\n"); */
            lval = BREG_Read32(hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CTLHI_REG));
            lval |= BCHP_BSCA_CTLHI_REG_IGNORE_ACK_MASK;
            BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CTLHI_REG), lval );
        }

        /* start the write command */
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_IIC_ENABLE),
                    (writeCmdWord | BCHP_BSCA_IIC_ENABLE_ENABLE_MASK));

        if (isNvram)
        {
            retCode = BI2C_P_WaitForDataToLeaveFIFO(hChn, lval);

            if ( writeCmdWord & BCHP_BSCA_IIC_ENABLE_NO_STOP_MASK )
            {
                /* There is a no stop */

            }
            else
            {
                /* If the master is sending the stop command, then
                 * this signals the nvm device to begin its internal write
                 * cylce. Now would be a good time to poll for its
                 * completion
                 */

                retCode = BI2C_P_WaitForNVMToAcknowledge( hChn,
                                                                chipAddr,
                                                                subAddr,
                                                                numSubAddrBytes,
                                                                numBytes );
             }

        }
        else
        {
            retCode = BI2C_P_WaitForCompletion(hChn, lval);
        }

        if (retCode != BERR_SUCCESS)
            goto done;

        if (cnt)
        {
            writeCmdWord = BCHP_BSCA_IIC_ENABLE_NO_START_MASK;
            bscRegAddr = BCHP_BSCA_DATA_IN0;
        }
    }
    while (cnt>0);

done:
    if (mutex)
        BI2C_P_RELEASE_MUTEX( hChn );

    return retCode;
}
#endif

BERR_Code BI2C_P_WaitForCompletion
(
    BI2C_ChannelHandle  hChn,                           /* Device channel handle */
    uint32_t            numBytes                        /* number of bytes to transfer */
)
{
    BI2C_Handle         hDev;
    BERR_Code           retCode = BERR_SUCCESS;
    uint32_t            lval, timeoutMs, loopCnt;

    hDev = hChn->hI2c;

    if (hChn->timeoutMs == BI2C_TimeoutBasedOnClkSpeed)
    {
        /* Calculate the timeout value */
        numBytes++;                         /* add 1 for chip ID */
        lval = numBytes * 9;                /* number of clks, 9 clks per byte */
        timeoutMs = BI2C_P_CALCULATE_TIMEOUT(lval, hChn->clkRate);
    }
    else
    {
        /* Use hardcoded value */
        timeoutMs = hChn->timeoutMs;
    }

    if (hChn->intMode)
    {
        /*
         * Wait for event, set by ISR
         */
        if ( (retCode = BERR_TRACE(BKNI_WaitForEvent(hChn->hChnEvent, timeoutMs)) != BERR_SUCCESS) )
        {
            BKNI_ResetEvent(hChn->hChnEvent);
            goto done;
        }
        if (hChn->noAck)
        {
            BDBG_MSG(("BI2C_P_WaitForCompletion: BI2C_ERR_NO_ACK(0x110001) at line=%d", __LINE__));
            retCode = BI2C_ERR_NO_ACK;
            goto done;
        }
    }
    else
    {
        /*
         * Polling mode
         */
        loopCnt = ((timeoutMs * 1000) / I2C_POLLING_INTERVAL) + 1;
        while (1)
        {
            lval = BREG_Read32(hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_IIC_ENABLE));
            if (lval & BCHP_BSCA_IIC_ENABLE_INTRP_MASK)
                break;

            if (loopCnt == 0)
            {
                retCode = BERR_TRACE (BERR_TIMEOUT);
                goto done;
            }
            BKNI_Delay(I2C_POLLING_INTERVAL);
            loopCnt--;
        }
    }

    lval = BREG_Read32(hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_IIC_ENABLE));
    if (lval & BCHP_BSCA_IIC_ENABLE_NO_ACK_MASK)
    {
        if ( hChn->nvramAck == 0 )
        {
            BDBG_MSG(("BI2C_P_WaitForCompletion: BI2C_ERR_NO_ACK(0x110001) at line=%d", __LINE__));
            retCode = BI2C_ERR_NO_ACK;
        }
        else
        {
            /* If we are waiting for the NVM to acknowledge, then we DO NOT
             * want to trace an error message, however, we do need to
             * tell the caller that it has still not acked.
             */
            BDBG_MSG(("BI2C_P_WaitForCompletion: BI2C_ERR_NO_ACK(0x110001) at line=%d", __LINE__));
            retCode = BI2C_ERR_NO_ACK;
        }
    }

done:
    /* stop execution */
    lval = BREG_Read32(hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_IIC_ENABLE));
    lval &= ~(BCHP_BSCA_IIC_ENABLE_ENABLE_MASK);
    BREG_Write32(hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_IIC_ENABLE), lval);


    return retCode;
}

static void BI2C_P_HandleInterrupt_Isr
(
    void *pParam1,                      /* Device channel handle */
    int parm2                           /* not used */
)
{
    BI2C_ChannelHandle  hChn;
    BI2C_Handle         hDev;
    uint32_t            lval;

    hChn = (BI2C_ChannelHandle) pParam1;
    BDBG_ASSERT( hChn );

    BSTD_UNUSED(parm2);

    hDev = hChn->hI2c;
    lval = BREG_Read32(hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_IIC_ENABLE));

    hChn->noAck = (lval & BCHP_BSCA_IIC_ENABLE_NO_ACK_MASK) ? true : false; /* save no ack status */
    lval &= ~(BCHP_BSCA_IIC_ENABLE_ENABLE_MASK);
    BREG_Write32(hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_IIC_ENABLE), lval);

    BKNI_SetEvent( hChn->hChnEvent );

    return;
}

#ifdef BI2C_USES_SETUP_HDMI_HW_ACCESS

BERR_Code BI2C_P_SetupHdmiHwAccess(
    void *context,                      /* Device channel handle */
    uint32_t dataTransferFormat,        /* Data Transfer Format */
    uint32_t cnt1,                      /* Counter 1 value */
    uint32_t cnt2                       /* Counter 2 value */
)
{
    BI2C_ChannelHandle hChn;
    BI2C_Handle hDev;
    uint32_t lval;
    BERR_Code rc = BERR_SUCCESS;

    if (context == NULL)
    {
        BDBG_ERR(("Invalid I2C handle\n"));
        rc = BERR_INVALID_PARAMETER;
        BSTD_UNUSED(dataTransferFormat);
        BSTD_UNUSED(cnt1);
        BSTD_UNUSED(cnt1);
    }
    else
    {
        hChn = (BI2C_ChannelHandle)context;
        BDBG_ASSERT( hChn->magicId == DEV_MAGIC_ID );

        /* Get I2C handle from channel handle */
        hDev = hChn->hI2c;

        lval = BREG_Read32(hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CTL_REG));
        lval &= ~BCHP_BSCA_CTL_REG_DTF_MASK;
        lval |= dataTransferFormat << BCHP_BSCA_CTL_REG_DTF_SHIFT;
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CTL_REG), lval );

        lval = BREG_Read32(hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CNT_REG));
        lval &= ~(BCHP_BSCA_CNT_REG_CNT_REG1_MASK | BCHP_BSCA_CNT_REG_CNT_REG2_MASK);
        lval |= cnt1 << BCHP_BSCA_CNT_REG_CNT_REG1_SHIFT;
        lval |= cnt2 << BCHP_BSCA_CNT_REG_CNT_REG2_SHIFT;
        BREG_Write32( hDev->hRegister, (hChn->coreOffset + BCHP_BSCA_CNT_REG), lval );
    }

    return rc;
}

#else

BERR_Code BI2C_P_SetupHdmiHwAccess(
    void *context,                      /* Device channel handle */
    uint32_t dataTransferFormat,        /* Data Transfer Format */
    uint32_t cnt1,                      /* Counter 1 value */
    uint32_t cnt2                       /* Counter 2 value */
    )
{
    BSTD_UNUSED(context);
    BSTD_UNUSED(dataTransferFormat);
    BSTD_UNUSED(cnt1);
    BSTD_UNUSED(cnt2);

    BDBG_ERR(("%s: Not Supported", __FUNCTION__));
    return BERR_NOT_SUPPORTED;
}

#endif

/* End of file */
