/***************************************************************************
 *     Copyright (c) 2005-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsp_s_calc_sequence_key.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 2/8/11 11:54a $
 ***************************************************************************/
#ifndef BSP_S_CALC_SEQUENCE_KEY_H__
#define BSP_S_CALC_SEQUENCE_KEY_H__

typedef enum BCMD_User_Calc_Sequence_Key_CmdInputField_e
{
    BCMD_UserSequenceKey_CmdInputField_eKeySrc      = (BCMD_CommonBufferFields_eParamLen + 2),                             /* byte 20 starting from 0 */ 
    BCMD_UserSequenceKey_CmdInputField_eDecryptKey =  ( 5 << 2) + 3, /*   */

    BCMD_UserSequenceKey_CmdInputField_eMediaKeyId  = ( 5 <<2) + 2,     /*  */
    BCMD_UserSequenceKey_CmdInputField_eKeyEnum     = ( 5 <<2) + 3,     /*  */
    BCMD_UserSequenceKey_CmdInputField_eKeyLadderOp = ( 6 <<2) + 3,     /*  */
    BCMD_UserSequenceKey_CmdInputField_eKeyLadderType = (7 <<2)+ 3,    /*  */
    BCMD_UserSequenceKey_CmdInputField_eNonce        = ( 8 <<2),     /* */

    BCMD_UserSequenceKey_CmdInputField_eSequenceKeyPtr = (12 <<2),   /*  */
    BCMD_UserSequenceKey_CmdInputField_eSequenceKeyDataPtr= ( 13 <<2),    /*  */
    BCMD_UserSequenceKey_CmdInputField_eSKBRecordPtr= ( 14 << 2),     /*  */
    BCMD_UserSequenceKey_CmdInputField_eNumCondCVDR = ( 15 << 2),     /*  */
    BCMD_UserSequenceKey_CmdInputField_eVolumeId    = ( 16 << 2),     /*  */
    BCMD_UserSequenceKey_CmdInputField_eKvvu_Id     = ( 20 << 2) + 3 ,     /*  */
    BCMD_UserSequenceKey_CmdInputField_Free         = ( 21 << 2)     /*  */

}   BCMD_User_Calc_Sequence_Key_CmdInputField_e;

typedef enum BCMD_UserSequenceKeys_CmdOutputField_e
{
    BCMD_UserSequenceKey_CmdOutputField_eStatus  = (BCMD_CommonBufferFields_eParamLen + 5),          /* byte 23 starting from 0 */

    BCMD_UserSequenceKey_CmdOutputField_VariantNumber = (0x06 << 2)+2,       /* */   
    BCMD_UserSequenceKey_CmdOutputField_Free  = (0x07 << 2)          /* */
}   BCMD_UserSequenceKeys_CmdOutputField_e;


#endif /* BSP_S_CALC_SEQUENCE_KEY_H__ */


