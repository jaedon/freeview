/***************************************************************************
 *     Copyright (c) 2005-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsp_s_segmentTableConv.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 2/8/11 11:55a $
 ***************************************************************************/

#ifndef BSP_S_SEGMENT_TABLE_CONV_H__  
#define BSP_S_SEGMENT_TABLE_CONV_H__

enum BCMD_User_SegTableConvert_CmdInputField_e
{
    BCMD_UserSegTableConvert_CmdInputField_eFlag    =  ( 5 << 2) + 3, 
    BCMD_UserSegTableConvert_CmdInputField_eNumKeys =  ( 6 << 2) + 3, 
    BCMD_UserSegTableConvert_CmdInputField_eKeyID   =  ( 7 << 2) + 3, 
    
	BCMD_UserSegTableConvert_CmdInputField_eIV             = (  8 << 2) + 0,
    BCMD_UserSegTableConvert_CmdInputField_eValidationData = ( 12 << 2) + 0,
    BCMD_UserSegTableConvert_CmdInputField_eSegKeyInfoData = ( 16 << 2) + 0,
    BCMD_UserSegTableConvert_CmdInputField_eEnd            = ( 96 << 2) + 0, 

    BCMD_UserSegTableConvert_CmdInputField_eCmdBufferLimit = ( 96 << 2) + 0, 
};

enum BCMD_User_SegTableConvert_CmdOutputField_e
{
    BCMD_UserSegTableConvert_CmdOutputField_eStatus   = ( 5 << 2) + 3, 
    BCMD_UserSegTableConvert_CmdOutputField_eFlag     = ( 6 << 2) + 3, 
    BCMD_UserSegTableConvert_CmdOutputField_eNumKeys  = ( 7 << 2) + 3, 
    
	BCMD_UserSegTableConvert_CmdOutputField_eEncryptedIV    = (  8 << 2) + 0,
    BCMD_UserSegTableConvert_CmdOutputField_eValidationData = ( 12 << 2) + 0,
    BCMD_UserSegTableConvert_CmdOutputField_eSegKeyInfoData   = ( 16 << 2) + 0,
    BCMD_UserSegTableConvert_CmdOutputField_eEnd            = ( 96 << 2) + 0, 

    BCMD_UserSegTableConvert_CmdOutputField_eCmdBufferLimit = ( 96 << 2) + 0, 
};

#endif /* BSP_S_SEGMENT_TABLE_CONV_H__ */

