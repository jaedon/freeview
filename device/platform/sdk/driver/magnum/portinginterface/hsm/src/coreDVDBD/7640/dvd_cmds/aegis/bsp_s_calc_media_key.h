/***************************************************************************
 *     Copyright (c) 2005-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsp_s_calc_media_key.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 2/8/11 11:54a $
 ***************************************************************************/
#ifndef BSP_S_CALC_MEDIA_KEY_H__
#define BSP_S_CALC_MEDIA_KEY_H__


/* BSP_PUBLIC VOID BCMD_User_Update_Secret_Key(VOID); */
typedef enum BCMD_User_Update_Secret_Key_CmdInputField_e
{
    BCMD_UserUpdateSecretKey_CmdInputField_eKey   = (BCMD_CommonBufferFields_eParamLen + 2),                             /* byte 20 starting from 0 */ 
    BCMD_UserUpdateSecretKey_CmdInputField_eKeyID     = ( 5 << 2) + 3, 
    BCMD_UserUpdateSecretKey_CmdInputField_eReserved6 = ( 6 << 2), 
    BCMD_UserUpdateSecretKey_CmdInputField_eKeyData   = (10 << 2), 
    
	BCMD_UserUpdateSecretKey_CmdInputField_eFree      = (74 << 2), 
	BCMD_UserUpdateSecretKey_CmdInputField_eLast      = (75 << 2) 
}	BCMD_User_Update_Secret_Key_CmdInputField_e;



/*BSP_PUBLIC VOID BCMD_User_Calc_Media_Key(VOID); */
typedef enum BCMD_User_Calc_Media_Key_CmdInputField_e
{
    BCMD_UserMediaKey_CmdInputField_eKey       = (BCMD_CommonBufferFields_eParamLen + 2),                             /* byte 20 starting from 0 */ 
    BCMD_UserMediaKey_CmdInputField_eDevKeyLoc = ( 5 << 2) + 3, /*   */
    BCMD_UserMediaKey_CmdInputField_eMediaKeyID = ( 5 << 2) + 2, /*   */

    BCMD_UserMediaKey_CmdInputField_eVirtualKeyLadder = ( 6 << 2) + 3, /*  */
    BCMD_UserMediaKey_CmdInputField_eUV        = ( 7 << 2),     /*  */
    BCMD_UserMediaKey_CmdInputField_eMv        = ( 8 << 2),     /*  */
    BCMD_UserMediaKey_CmdInputField_eMvPrime   = ( 9 <<2),      /*  */
    BCMD_UserMediaKey_CmdInputField_eMediaKeyData = ( 10 <<2),   /*  */
    BCMD_UserMediaKey_CmdInputField_eKCDFlag   = ( 14 <<2) + 3, /*  */
    BCMD_UserMediaKey_CmdInputField_eKCD       = ( 15 <<2),     /*  */
    BCMD_UserMediaKey_CmdInputField_eVolumeID  = ( 19 <<2),     /*  */
    BCMD_UserMediaKey_CmdInputField_eVerificationData = ( 23 <<2), /* */


    BCMD_UserMediaKey_CmdInputField_eFree0     = ( 27 <<2),    /*  */
    BCMD_UserMediaKey_CmdInputField_eFree1     = ( 31 <<2),    /*  */
    BCMD_UserMediaKey_CmdInputField_eUnused    = ( 32 << 2)    /*  */

}	BCMD_User_Calc_Media_Key_CmdInputField_e;

/*
    #5	Encrypted Device  Key
    #6  Virtual Key Ladder
    #7	UV
    #8	M_v  (subset difference mask )
    #9	M_v ( device Key Mask )
    #10	Media Key Data
    #14	KCD
    #18	KCD Flag
    #19	Volume ID
    #23	Verification Data
*/

/*BSP_PUBLIC VOID BCMD_User_Calc_Media_Key(VOID); */
typedef enum BCMD_User_Calc_Unified_Media_Key_CmdInputField_e
{
    BCMD_UserUnifiedMediaKey_CmdInputField_eKey       = (BCMD_CommonBufferFields_eParamLen + 2),                             /* byte 20 starting from 0 */
    BCMD_UserUnifiedMediaKey_CmdInputField_eDevKeyLoc = ( 5 << 2) + 3, /*   */
    BCMD_UserUnifiedMediaKey_CmdInputField_eMediaKeyID = ( 5 << 2) + 2, /*   */
    BCMD_UserUnifiedMediaKey_CmdInputField_eVirtualKeyLadder = ( 6 << 2) + 3, /*  */
    BCMD_UserUnifiedMediaKey_CmdInputField_eUV        = ( 7 << 2),     /*  */
    BCMD_UserUnifiedMediaKey_CmdInputField_eMv        = ( 8 << 2),     /*  */
    BCMD_UserUnifiedMediaKey_CmdInputField_eMvPrime   = ( 9 <<2),      /*  */
    BCMD_UserUnifiedMediaKey_CmdInputField_eMediaKeyVariantData = ( 10 <<2),   /*  */
    BCMD_UserUnifiedMediaKey_CmdInputField_eNonce   = ( 14 <<2), /*  */
    BCMD_UserUnifiedMediaKey_CmdInputField_eVolumeID  = ( 18 <<2),     /*  */
    BCMD_UserUnifiedMediaKey_CmdInputField_eVerificationData = ( 22 <<2), /* */


    BCMD_UserUnifiedMediaKey_CmdInputField_eFree0     = ( 26 <<2),    /*  */
    BCMD_UserUnifiedMediaKey_CmdInputField_eFree1     = ( 30 <<2),    /*  */
    BCMD_UserUnifiedMediaKey_CmdInputField_eUnused    = ( 31 << 2)    /*  */

}	BCMD_User_Calc_Unified_Media_Key_CmdInputField_e;

/*
    #5	Encrypted Device  Key
    #6  Virtual Key Ladder
    #7	UV
    #8	M_v  (subset difference mask )
    #9	M_v ( device Key Mask )
    #10	Media Key Variant Data
    #14	Nonce
    #18	Volume ID
    #22	Verification Data
*/


typedef enum BCMD_UserMediaKeys_CmdOutputField_e
{
    BCMD_UserMediaKey_CmdOutputField_eStatus       = (BCMD_CommonBufferFields_eParamLen + 5),          /* byte 23 starting from 0 */
    BCMD_UserMediaKey_CmdOutputField_eMediaKeyId   = (6 << 2) +3          

}	BCMD_UserMediaKeys_CmdOutputField_e;




#endif /* BSP_S_CALC_MEDIA_KEY_H__ */


