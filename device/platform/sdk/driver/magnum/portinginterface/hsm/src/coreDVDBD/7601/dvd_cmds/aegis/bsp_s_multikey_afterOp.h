/***************************************************************************
 *     Copyright (c) 2005-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * Module Description: This file implements calculation of media key from 
 *          a device key (encrypted by BRCM key) - AACS spec for HD-DVD / Blue-Ray 
 *
 * Author : Ravindra Jejurikar (rjejuri@broadcom.com)
 * 
 ***************************************************************************/

#ifndef BSP_S_MULTI_KEY_AFTER_OP_H__
#define BSP_S_MULTI_KEY_AFTER_OP_H__

enum BCMD_User_Load_Key_After_Op_CmdInputField_e
{
    BCMD_UserLoadKeyAfterOp_CmdInputField_eSomeThing = (BCMD_CommonBufferFields_eParamLen + 2),                             /* byte 20 starting from 0 */ 
    BCMD_UserLoadKeyAfterOp_CmdInputField_eNumOfKeys = ( 5 << 2) + 3, /*  */
    BCMD_UserLoadKeyAfterOp_CmdInputField_eKeySize   = ( 6 << 2) + 3, /*  */
    BCMD_UserLoadKeyAfterOp_CmdInputField_eBlkType   = ( 7 << 2) + 3, /*  */

    BCMD_UserLoadKeyAfterOp_CmdInputField_eOpType    = ( 8 << 2) + 3, /*  */
    BCMD_UserLoadKeyAfterOp_CmdInputField_eOpDetail0 = ( 9 << 2) ,    /*  */
    BCMD_UserLoadKeyAfterOp_CmdInputField_eOpDetail1 = ( 10 << 2) ,   /*  */
    BCMD_UserLoadKeyAfterOp_CmdInputField_eOpDetail2 = ( 11 << 2) ,   /*  */
    BCMD_UserLoadKeyAfterOp_CmdInputField_eOpDetail3 = ( 12 << 2) ,   /*  */
    
	BCMD_UserLoadKeyAfterOp_CmdInputField_eReserved13 = (13 << 2) + 3,/*  */
	BCMD_UserLoadKeyAfterOp_CmdInputField_eReserved14 = (14 << 2) + 3,/*  */

    BCMD_UserLoadKeyAfterOp_CmdInputField_eKeySlotBase = (15 << 2),   /*  */

	BCMD_UserLoadKeyAfterOp_CmdInputField_eOpData0 =     (19 << 2),   /*  */
	BCMD_UserLoadKeyAfterOp_CmdInputField_eOpData1 =     (23 << 2),   /*  */

    BCMD_UserLoadKeyAfterOp_CmdInputField_eKeyDataBase = (27 << 2),   /*  */
	BCMD_UserLoadKeyAfterOp_CmdInputField_eFree        = (91 << 2),   /*  */

	BCMD_UserLoadKeyAfterOp_CmdInputField_eDummyLast   = (92 << 2)    /*  */
};

enum BCMD_UserLoadKeyAfterOp_OperationType_e   
{
   BCMD_UserLoadKeyAfterOp_OperationType_eNOP =0,
   BCMD_UserLoadKeyAfterOp_OperationType_eAES_128D =1,  
   BCMD_UserLoadKeyAfterOp_OperationType_eAES_ECBD_XOR_Key =2,
   BCMD_UserLoadKeyAfterOp_OperationType_eAES_ECBD_XOR_DataOut =3,
   BCMD_UserLoadKeyAfterOp_OperationType_eAES_ECBD_XOR_Key_DataOut =4,

   BCMD_UserLoadKeyAfterOp_OperationType_eINVALID
};

enum BCMD_UserLoadKeyAfterOp_Op_AES128D_e   
{
   BCMD_UserLoadKeyAfterOp_Op_AES128D_eSecretKeyID = 
	                       BCMD_UserLoadKeyAfterOp_CmdInputField_eOpDetail0 + 3,

   BCMD_UserLoadKeyAfterOp_Op_AES128D_eParam1 = 
	                       BCMD_UserLoadKeyAfterOp_CmdInputField_eOpDetail1 + 0,

   BCMD_UserLoadKeyAfterOp_Op_AES128D_eParam2 = 
	                       BCMD_UserLoadKeyAfterOp_CmdInputField_eOpDetail2 + 0,

   BCMD_UserLoadKeyAfterOp_Op_AES128D_eParam3 = 
	                       BCMD_UserLoadKeyAfterOp_CmdInputField_eOpDetail3 + 0,


   BCMD_UserLoadKeyAfterOp_Op_AES128D_eXorWithKey = 
	                       BCMD_UserLoadKeyAfterOp_CmdInputField_eOpData0 + 0,

   BCMD_UserLoadKeyAfterOp_Op_AES128D_eXorWithDataOut = 
	                       BCMD_UserLoadKeyAfterOp_CmdInputField_eOpData1 + 0,

   BCMD_UserLoadKeyAfterOp_Op_AES128D_eInvalid

};


#endif /*  */

