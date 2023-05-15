/***************************************************************************
 *     Copyright (c) 2005-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * Module Description: This file implements multiple-key decryption command 
 *          required in HD-DVD / Blue-Ray 
 *
 * Author : Ravindra Jejurikar (rjejuri@broadcom.com)
 * 
 ***************************************************************************/

#ifndef BSP_S_MULTI_KEY_H__
#define BSP_S_MULTI_KEY_H__


typedef enum BCMD_UserMultipleKeys_CmdInputField_e
{

/* What is this??? */
    BCMD_UserMultipleKeys_CmdInputField_eKey         = (BCMD_CommonBufferFields_eParamLen + 2),                             /* byte 20 starting from 0 */ 
    BCMD_UserMultipleKeys_CmdInputField_eKeyID0      = (5 << 2) + 3,   /* word 5 starting from 0 */
    BCMD_UserMultipleKeys_CmdInputField_eKeyID1      = (6 << 2) + 3,   /* word 5 starting from 0 */
    BCMD_UserMultipleKeys_CmdInputField_eTitleKey0   = (7 << 2),   /* word 5 starting from 0 */
    BCMD_UserMultipleKeys_CmdInputField_eTitleKey1   = (11 << 2),   /* word 9 */

    BCMD_UserMultipleKeys_CmdInputField_eAESVariant  = ((15 <<2)+3), /*  */
    BCMD_UserMultipleKeys_CmdInputField_eNumContentKeys = ((16 <<2)+3), /*  */
    BCMD_UserMultipleKeys_CmdInputField_eTitleKeyMap =   ((17 << 2) + 2),/*  */

/* WHy 16 key slot map base??? */
	BCMD_UserMultipleKeys_CmdInputField_eKeySlotMapBase  = (18 << 2), /* byte 56 starting from 0 */

    BCMD_UserMultipleKeys_CmdInputField_eContentKey0 = ( 22 <<2),   /*  */
    BCMD_UserMultipleKeys_CmdInputField_eContentKey1 = ( 26 <<2),   /*  */
    BCMD_UserMultipleKeys_CmdInputField_eContentKey2 = ( 30 <<2),   /*  */
    BCMD_UserMultipleKeys_CmdInputField_eContentKey3 = ( 34 <<2),   /*  */
    BCMD_UserMultipleKeys_CmdInputField_eContentKey4 = ( 38 <<2),   /*  */
    BCMD_UserMultipleKeys_CmdInputField_eContentKey5 = ( 42 <<2),   /*  */
    BCMD_UserMultipleKeys_CmdInputField_eContentKey6 = ( 52 <<2),   /*  */
    BCMD_UserMultipleKeys_CmdInputField_eContentKey7 = ( 56 <<2),   /*  */

    BCMD_UserMultipleKeys_CmdInputField_eUnused      =  ( 60 << 2)   /* byte 56 starting from 0 */


  }BCMD_UserMultipleKeys_CmdInputField_e;

typedef enum BCMD_UserMultipleKeys_e
{
   BCMD_UserMultipleKeys_NumberTitleKeys   = 2, /* 128 bit = 16 bytes */

   BCMD_UserMultipleKeys_TitleKeySize   = 16, /* 128 bit = 16 bytes */
   BCMD_UserMultipleKeys_ContentKeySize = 16, /* 128 bit = 16 bytes */
   BCMD_UserMultipleKeys_KeySlotSize = 1,     /* 8 bit = 1 bytes */

   BCMD_UserMultipleKeys_AESVariante_AES_G = 0,
   BCMD_UserMultipleKeys_AESVariante_AES_Ex = 1,

   BCMD_UserMultipleKeys_DummyTerminator   = 0 /*  */
} BCMD_UserMultipleKeys_e;

/*
5	Title Key 0
9	Title Key 1 
13	Key Binding BitMap
14	Content Key 0
18	Content Key 1
22	Content Key 2
26	Content Key 3
30	Content Key 4
34	Content Key 5
38	Content Key 6
42	Content Key 7
46	AES-G or AES-Ex
47	Key Slot Map
*/


typedef enum BCMD_UserMultipleKeys_CmdOutputField_e
{
    BCMD_UserMultipleKeys_CmdOutputField_eStatus       = (BCMD_CommonBufferFields_eParamLen + 5),          /* byte 23 starting from 0 */

/* Why so many output parameter??? */
    BCMD_UserMultipleKeys_CmdOutputField_TK0       = (0x10 << 2),          
    BCMD_UserMultipleKeys_CmdOutputField_TK1       = (0x14 << 2),          
    BCMD_UserMultipleKeys_CmdOutputField_Decrypt_TK0       = (0x18 << 2),          
    BCMD_UserMultipleKeys_CmdOutputField_Decrypt_TK1       = (0x1c << 2),          
    BCMD_UserMultipleKeys_CmdOutputField_keyMap       = (0x20 << 2),          
    BCMD_UserMultipleKeys_CmdOutputField_ContentKey0  = (0x22 << 2),          
    BCMD_UserMultipleKeys_CmdOutputField_ContentKey0_AES_D  = (0x26 << 2),          
    BCMD_UserMultipleKeys_CmdOutputField_ContentKey0_AES_G  = (0x2a << 2),          
    BCMD_UserMultipleKeys_CmdOutputField_AES_Variant        = ((0x2e << 2)+3), 
    BCMD_UserMultipleKeys_CmdOutputField_FinalDummy        = (0x32 << 2)
} BCMD_UserMultipleKeys_CmdOutputField_e;




/*
10	Title Key 0
14	Title Key 1 
18	Decrypt Title Key 0
1c	Decrypt Title Key 1
20  Key binding map 
22  Content Key 0
26  AES-D content Key 
2a  AES-G content key


22	Content Key 2
26	Content Key 3
30	Content Key 4
34	Content Key 5
38	Content Key 6
42	Content Key 7
46	AES-G or AES-Ex
47	Key Slot Map
*/



typedef enum BCMD_User_Load_Key_After_Op_CmdInputField
{
    BCMD_UserLoadKeyAfterOp_CmdInputField_eSomeThing = (BCMD_CommonBufferFields_eParamLen + 2),                             /* byte 20 starting from 0 */ 
    BCMD_UserLoadKeyAfterOp_CmdInputField_eNumOfKeys = ( 5 << 2) + 3, /*  */
    BCMD_UserLoadKeyAfterOp_CmdInputField_eKeySize   = ( 6 << 2) + 3, /*  */
    BCMD_UserLoadKeyAfterOp_CmdInputField_eBlkType   = ( 7 << 2) + 3, /*   */

    BCMD_UserLoadKeyAfterOp_CmdInputField_eOpType    = ( 8 << 2) + 3, /*   */
    BCMD_UserLoadKeyAfterOp_CmdInputField_eOpDetail0 = ( 9 << 2) , /*   */
    BCMD_UserLoadKeyAfterOp_CmdInputField_eOpDetail1 = ( 10 << 2) , /*   */
    BCMD_UserLoadKeyAfterOp_CmdInputField_eOpDetail2 = ( 11 << 2) , /*   */
    BCMD_UserLoadKeyAfterOp_CmdInputField_eOpDetail3 = ( 12 << 2) , /*   */

    BCMD_UserLoadKeyAfterOp_CmdInputField_eCustSubMode = (13 << 2) + 3,
	BCMD_UserLoadKeyAfterOp_CmdInputField_eReserved14 = (14 << 2) + 3, /*   */

    BCMD_UserLoadKeyAfterOp_CmdInputField_eKeySlotBase = (15 << 2), /*  */


	BCMD_UserLoadKeyAfterOp_CmdInputField_eOperation_Data0 =  (19 << 2), /*   */
	
	BCMD_UserLoadKeyAfterOp_CmdInputField_eOperation_Data1 =  (23 << 2), /*   */

    BCMD_UserLoadKeyAfterOp_CmdInputField_eKeyDataBase = (27 << 2), /*  */
	BCMD_UserLoadKeyAfterOp_CmdInputField_eFree        = (91 << 2), /*   */

	BCMD_UserLoadKeyAfterOp_CmdInputField_eDummyLast   = (92 << 2) /*   */
}BCMD_User_Load_Key_After_Op_CmdInputField_e;

typedef enum BCMD_UserLoadKeyAfterOp_OperationType
{
   BCMD_UserLoadKeyAfterOp_OperationType_eNOP =0,
   BCMD_UserLoadKeyAfterOp_OperationType_eAES_128D =1,
   BCMD_UserLoadKeyAfterOp_OperationType_eINVALID = 2,
   BCMD_UserLoadKeyAfterOp_OperationType_eAES_ECBD_XOR_DataOut = 3
}BCMD_UserLoadKeyAfterOp_OperationType_e;

typedef enum BCMD_UserLoadKeyAfterOp_Op_AES128D   
{
   BCMD_UserLoadKeyAfterOp_Op_AES128D_eSecretKeyID = 
	                      BCMD_UserLoadKeyAfterOp_CmdInputField_eOpDetail0 + 3,

   BCMD_UserLoadKeyAfterOp_Op_AES128D_eParam1 = 
	                      BCMD_UserLoadKeyAfterOp_CmdInputField_eOpDetail1 + 0,

   BCMD_UserLoadKeyAfterOp_Op_AES128D_eParam2 = 
	                      BCMD_UserLoadKeyAfterOp_CmdInputField_eOpDetail2 + 0,

   BCMD_UserLoadKeyAfterOp_Op_AES128D_eParam3 = 
	                      BCMD_UserLoadKeyAfterOp_CmdInputField_eOpDetail3 + 0,

   BCMD_UserLoadKeyAfterOp_Op_AES128D_eInvalid
}BCMD_UserLoadKeyAfterOp_Op_AES128D_e;


typedef enum BCMD_User_Load_Secret_Key_CmdInputField
{
    BCMD_UserLoadSecretKey_CmdInputField_eStart = BCMD_HEADER_BYTE_SIZE,
		
    BCMD_UserLoadSecretKey_CmdInputField_eKeyID = ( 5 << 2) + 3, /*  */
    BCMD_UserLoadSecretKey_CmdInputField_eBlkType = (6<<2)+3,     

    BCMD_UserLoadSecretKey_CmdInputField_eCustSubMode = (7<<2) + 3,  /*  */
    BCMD_UserLoadSecretKey_CmdInputField_eSlotNum=(8<<2)+3,            
    BCMD_UserLoadSecretKey_CmdInputField_eEnd   = (10 << 2) /*   */
}BCMD_User_Load_Secret_Key_CmdInputField_e;


#endif /* BSP_S_MULTI_KEY_H__ */


