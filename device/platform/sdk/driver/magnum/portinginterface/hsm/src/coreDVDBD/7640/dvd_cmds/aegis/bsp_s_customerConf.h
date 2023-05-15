/***************************************************************************
 *     Copyright (c) 2005-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsp_s_customerConf.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 2/8/11 11:54a $
 ***************************************************************************/

#ifndef BSP_S_CUSTOMER_CONF_H__  
#define BSP_S_CUSTOMER_CONF_H__

enum BCMD_User_CustConfidential_CmdInputField_e
{
    BCMD_UserCustConfidential_CmdInputField_eIV      =  ( 5 << 2) + 0, 
    BCMD_UserCustConfidential_CmdInputField_eNonce_1 =  ( 9 << 2) + 0, 
    BCMD_UserCustConfidential_CmdInputField_eNonce_2 =  ( 13 << 2) + 0, 
    BCMD_UserCustConfidential_CmdInputField_eKMU1    =  (17 << 2) + 0,
    BCMD_UserCustConfidential_CmdInputField_eKMU2    =  (21 << 2) + 0,
    BCMD_UserCustConfidential_CmdInputField_eC_UID   =  ( 25 << 2) + 0, 
    BCMD_UserCustConfidential_CmdInputField_eV_UID   =  ( 29 << 2) + 0,
    BCMD_UserCustConfidential_CmdInputField_eAuth_Key_in = ( 33 << 2) + 0,
    BCMD_UserCustConfidential_CmdInputField_eEnd     =  ( 37 << 2) + 0, 
};

enum BCMD_User_CustConfidential_CmdOutputField_e
{
    BCMD_UserCustConfidential_CmdOutputField_eStatus   =  (  5 << 2) + 3, 
    BCMD_UserCustConfidential_CmdOutputField_eAuthKey2 =  (  6 << 2) + 0, 
    BCMD_UserCustConfidential_CmdOutputField_eEnd      =  ( 11 << 2) + 0 
};


/***************************************************************************/
/*                     Next Command - Verify Sign                          */
/***************************************************************************/

enum BCMD_User_CustVerifySign_CmdInputField_e
{
    BCMD_UserCustVerifySign_CmdInputField_eDigest  =  ( 5 << 2) + 0, 
    BCMD_UserCustVerifySign_CmdInputField_eSign    =  ( 9 << 2) + 0, 
    BCMD_UserCustVerifySign_CmdInputField_eEnd     =  ( 13 << 2) + 0, 
};

enum BCMD_User_CustVerifySign_CmdOutputField_e
{
    BCMD_UserCustVerifySign_CmdOutputField_eStatus =  ( 5 << 2) + 3, 
    BCMD_UserCustVerifySign_CmdOutputField_eEnd    =  ( 6 << 2) + 0 
};

#endif /* BSP_S_CUSTOMER_CONF_H__ */

