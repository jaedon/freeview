/***************************************************************************
 *     Copyright (c) 2005-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsp_s_calc_media_key.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 8/25/08 10:21p $
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hsm/7601/a0/dvd_cmds/aegis/bsp_s_calc_media_key.h $
 * 
 * Hydra_Software_Devel/2   8/25/08 10:21p atruong
 * PR45631: BCM7601A0 Bring up
 * 
 * Hydra_Software_Devel/1   8/12/08 5:59p atruong
 * PR45631: HSM PI support for BCM7601 platform
 * 
 * 1   1/23/08 2:31p aratim
 * PR3: Ported from 7440
 * 
 * Aegis_Firmware_Devel/5   4/5/07 7:27p rjejuri
 * PR 522 : Update Device key usage in MKB
 * 
 * Aegis_Firmware_Devel/4   2/15/07 5:28p rjejuri
 * PR 522 : Media Key Km_1 (added)
 * 
 * Aegis_Firmware_Devel/3   11/6/06 11:56a rjejuri
 * PR 388: change comments to C style
 * 
 * Aegis_Firmware_Devel/2   11/5/06 7:25p rjejuri
 * PR 388: new commands for 7440
 * 
 * Aegis_Firmware_Devel/1   10/30/06 3:16p rjejuri
 * PR 388: new commands to 7440A1
 * 
 ***************************************************************************/
#ifndef BSP_S_CALC_MEDIA_KEY_H__
#define BSP_S_CALC_MEDIA_KEY_H__


/* BSP_PUBLIC VOID BCMD_User_Update_Secret_Key(VOID); */
enum BCMD_User_Update_Secret_Key_CmdInputField_e
{
    BCMD_UserUpdateSecretKey_CmdInputField_eKey   = (BCMD_CommonBufferFields_eParamLen + 2),                             /* byte 20 starting from 0 */ 
    BCMD_UserUpdateSecretKey_CmdInputField_eKeyID     = ( 5 << 2) + 3, 
    BCMD_UserUpdateSecretKey_CmdInputField_eReserved6 = ( 6 << 2), 
    BCMD_UserUpdateSecretKey_CmdInputField_eKeyData   = (10 << 2), 
    
	BCMD_UserUpdateSecretKey_CmdInputField_eFree      = (74 << 2), 
	BCMD_UserUpdateSecretKey_CmdInputField_eLast      = (75 << 2) 
};



/*BSP_PUBLIC VOID BCMD_User_Calc_Media_Key(VOID); */
enum BCMD_User_Calc_Media_Key_CmdInputField_e
{
    BCMD_UserMediaKey_CmdInputField_eKey       = (BCMD_CommonBufferFields_eParamLen + 2),                             /* byte 20 starting from 0 */ 
    BCMD_UserMediaKey_CmdInputField_eDevKeyLoc = ( 5 << 2) + 3, /*   */
    BCMD_UserMediaKey_CmdInputField_eMediaKeyID = ( 5 << 2) + 2, /*   */
    BCMD_UserMediaKey_CmdInputField_eUV        = ( 6 << 2),     /*  */
    BCMD_UserMediaKey_CmdInputField_eMv        = ( 7 << 2),     /*  */
    BCMD_UserMediaKey_CmdInputField_eMvPrime   = ( 8 <<2),      /*  */
    BCMD_UserMediaKey_CmdInputField_eMediaKeyData = ( 9 <<2),   /*  */
    BCMD_UserMediaKey_CmdInputField_eKCDFlag   = ( 13 <<2) + 3, /*  */
    BCMD_UserMediaKey_CmdInputField_eKCD       = ( 14 <<2),     /*  */
    BCMD_UserMediaKey_CmdInputField_eVolumeID  = ( 18 <<2),     /*  */
    BCMD_UserMediaKey_CmdInputField_eVerificationData = ( 22 <<2), /* */


    BCMD_UserMediaKey_CmdInputField_eFree0     = ( 26 <<2),    /*  */
    BCMD_UserMediaKey_CmdInputField_eFree1     = ( 30 <<2),    /*  */
    BCMD_UserMediaKey_CmdInputField_eUnused    = ( 31 << 2)    /*  */

  };

/*
    #5	Encrypted Device  Key
    #6	UV
    #7	M_v  (subset difference mask )
    #8	M_v ( device Key Mask )
    #9	Media Key Data
    #13	KCD
    #17	KCD Flag
    #18	Volume ID
    #22	Verification Data
*/

enum BCMD_UserMediaKeys_CmdOutputField_e
{
    BCMD_UserMediaKey_CmdOutputField_eStatus       = (BCMD_CommonBufferFields_eParamLen + 5),          /* byte 23 starting from 0 */
    BCMD_UserMediaKey_CmdOutputField_eMediaKeyId   = (6 << 2) +3          

};



#if 0
/*BSP_PUBLIC VOID BCMD_User_Load_Secret_Key(VOID); */
enum BCMD_User_Load_Secret_Key_CmdInputField_e
{
    BCMD_UserLoadSecretKey_CmdInputField_eSomeThing = (BCMD_CommonBufferFields_eParamLen + 2),                             /* byte 20 starting from 0 */ 
    BCMD_UserLoadSecretKey_CmdInputField_eKeyID     = ( 5 << 2) + 3, /*  */
    BCMD_UserLoadSecretKey_CmdInputField_eBlkType   = ( 6 << 2) + 3, /*   */
    BCMD_UserLoadSecretKey_CmdInputField_eKeySlotNumber = ( 7 << 2) + 3 /*  */

};
#endif

#endif /* BSP_S_CALC_MEDIA_KEY_H__ */


