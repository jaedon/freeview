/***************************************************************************
 *     Copyright (c) 2005-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsp_s_keycommon.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 4/1/09 3:04p $
 * Broadcom Security Processor Code
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hsm/3548/b0/cust_generic/aegis/bsp_s_keycommon.h $
 * 
 * Hydra_Software_Devel/2   4/1/09 3:04p yili
 * PR53745:Fix header file to remove nexus hack
 * 
 * Hydra_Software_Devel/1   10/3/08 1:27a atruong
 * PR46656: HSM PI Support for Rev B0
 * 
 * Aegis_Firmware_Devel/6   3/17/08 2:32p kostas
 * PR1039: Added support for new HW ciphers
 * 
 * Aegis_Firmware_Devel/5   2/22/08 6:07p kostas
 * PR994: Block SW key to Clear key slot for non-SVP mode
 * 
 * Aegis_Firmware_Devel/4   3/30/07 4:08p sshekhar
 * PR 413: naming convention change for HDMI0 blocktype.
 * 
 * Aegis_Firmware_Devel/3   9/19/06 1:26p sshekhar
 * PR 413: Initial codebase for 3563A0
 * 
 * Aegis_Firmware_Devel/2   9/8/06 10:29a sshekhar
 * PR 394: second HDMI port changes
 * 
 * Aegis_Firmware_Devel/27   5/21/06 10:22p hzhu
 * PR189: Added a flag to determine if second key slot is needed for a pid
 * channel to avoid false error status when ACL disallows slot B for SVP
 * related customer modes.
 * 
 * Aegis_Firmware_Devel/26   3/16/06 3:20p hzhu
 * PR276: took out customer mode defines for AES-ECB and AES-counter mode
 * since it is supported in generic mode now.
 * 
 * Aegis_Firmware_Devel/25   2/22/06 10:35p hzhu
 * PR276:new mem2mem algorithm change
 * 
 * Aegis_Firmware_Devel/24   2/15/06 10:38a hzhu
 * PR276:added new mem2mem algorithm enums for 7118a0/7401b0/7440a0
 * 
 * Aegis_Firmware_Devel/23   1/27/06 6:08p hzhu
 * PR268:Moved key mode parameter from config_alg to key routing commands
 * 
 * Aegis_Firmware_Devel/22   1/6/06 1:51a btan
 * PR 255: Update shared files to sync up both Aegis and Host sides.
 * 
 * Aegis_Firmware_Devel/21   12/21/05 8:10p hzhu
 * PR244:Added new parameter for procout command
 * 
 * Aegis_Firmware_Devel/20   12/14/05 1:40p hzhu
 * PR229:Added one command field for host to control max errors allowed
 * for OTP C programming before using redundant row.
 * 
 * Aegis_Firmware_Devel/19   11/23/05 3:41p hzhu
 * PR222: slot type name changes and change p144 to p199
 * 
 * Aegis_Firmware_Devel/18   10/21/05 8:49a hzhu
 * PR208:Add one Key algorithm AesEcbMsc for Directv only
 * 
 * Aegis_Firmware_Devel/17   10/10/05 1:51p grasche
 * PR 205: replace BSP_DSS with BSP_DIRECTV
 * 
 * Aegis_Firmware_Devel/16   10/10/05 12:20p hzhu
 * PR187: comment change for 7400 key slot definition
 * 
 * Aegis_Firmware_Devel/15   9/23/05 9:06a hzhu
 * PR187:PR189: 7400 key slot definition change/Pid vector change/added
 * two more data words for keygen command
 * 
 * Aegis_Firmware_Devel/14   9/19/05 6:59p btan
 * PR 185: Updated the shared file for host compilation
 * 
 * Aegis_Firmware_Devel/13   8/22/05 2:56p btan
 * PR 120: Made changes based on E* code review
 * 
 * Aegis_Firmware_Devel/12   8/19/05 5:05p hzhu
 * PR110:Added new key slot types
 * 
 * Aegis_Firmware_Devel/11   8/12/05 3:02p dbayani
 * PR23: Replaced icam3 with nsk.
 * 
 * Aegis_Firmware_Devel/10   8/12/05 2:48p hzhu
 * PR89: keyladder/key loading code review clean up - adding more comments
 * 
 * Aegis_Firmware_Devel/9   8/8/05 1:38p hzhu
 * PR89:key ladder/key loading code review clean up
 * 
 * Aegis_Firmware_Devel/8   7/26/05 6:41p hzhu
 * PR81:Fixed multi command bug
 * 
 * Aegis_Firmware_Devel/7   7/21/05 8:44a hzhu
 * PR58:Recheck in the new enum for key slot type.
 * 
 * Aegis_Firmware_Devel/6   7/20/05 6:20p hzhu
 * PR58: added a new key slot type
 * 
 * Aegis_Firmware_Devel/5   7/12/05 7:11a hzhu
 * PR44: fixed key ladder related issues found by Bryant
 * 
 * Aegis_Firmware_Devel/4   7/10/05 11:03a hzhu
 * PR42:E* code review change to hide Ntdecryptor/NtEncryptor
 * 
 * Aegis_Firmware_Devel/3   6/13/05 8:49p hzhu
 * PR13830: naming convention changes
 * 
 * Aegis_Firmware_Devel/2   6/13/05 1:23p hzhu
 * PR13830: Move one define over
 * 
 * Aegis_Firmware_Devel/1   5/31/05 5:40p hzhu
 * PR13830: New key related shared file for host and aegis
 * 
 * 
 ***************************************************************************/
#ifndef BSP_S_KEYCOMMON_H__
#define BSP_S_KEYCOMMON_H__

/* This header file contains common enums for BSP and host*/

/* Five key slot types supported */
typedef enum BCMD_XptSecKeySlot_e
{                                        /* 7401A0      7400*/
    BCMD_XptSecKeySlot_eType0,      /* 6 entries   10 entries*/
    BCMD_XptSecKeySlot_eType1,     /* 8 entries   12 entries*/
    BCMD_XptSecKeySlot_eType2,     /* 10 entries 14 entries */
    BCMD_XptSecKeySlot_eType3,  /* 12 entries 16 entries*/
    BCMD_XptSecKeySlot_eType4,  /* 12 entries 16 entries */
    BCMD_XptSecKeySlot_eType5,  /* 16 entries 20 entries */
    BCMD_XptSecKeySlot_eType6,   /* 22 entries 26 entries */
    BCMD_XptSecKeySlot_eTypeMax
}BCMD_XptSecKeySlot_e;

/* key ladder root key src enum used for key ladder
root key generation */
typedef enum BCMD_RootKeySrc_e
{
    BCMD_RootKeySrc_eCusKey  = 0,
    BCMD_RootKeySrc_eOTPKeya = 1,
    BCMD_RootKeySrc_eOTPKeyb = 2,
    BCMD_RootKeySrc_eOTPKeyc = 3,
    BCMD_RootKeySrc_eReserved0  = 4,
    BCMD_RootKeySrc_eReserved1 = 5,
    BCMD_RootKeySrc_eReserved2 = 6,
    BCMD_RootKeySrc_eInvalidKey = 7	
}BCMD_RootKeySrc_e;

/* key ladder root key destination enum used for key ladder
root key generation */
typedef enum BCMD_GenRootKeyDest_e
{
    BCMD_GenRootKeyDest_eReserved0 = 0,
    BCMD_GenRootKeyDest_eReserved1 = 1,
    BCMD_GenRootKeyDest_eReserved2 = 2,
    BCMD_GenRootKeyDest_eReserved3    = 3,	
    /* The following enums are for 
    access control matrix only -
    not for host interface: Not used in digitaltv code*/
    BCMD_AccessControl_eKey3Ladder1 = 4,
    BCMD_AccessControl_eKey3Ladder2 = 5,
    BCMD_AccessControl_eKey3Ladder3 = 6,
    BCMD_GenRootKeyDest_Invalid
}BCMD_GenRootKeyDest_e;

/* key ladder key source/destination enum used for key ladder
 non-root key generation from key3 - key5*/
typedef enum BCMD_KeyRamBuf_e
{
    BCMD_KeyRamBuf_eFirstRam    = 0, /* First 192 bit user key ram*/
    BCMD_KeyRamBuf_eSecondRam   = 1, /* Second 192 bit user key ram*/
    BCMD_KeyRamBuf_eThirdRam    = 2, /* third 192 bit user key ram*/

    BCMD_KeyRamBuf_eKey3KeyLadder1 = 3, /* key3 of key ladder1 - CP*/
    BCMD_KeyRamBuf_eKey4KeyLadder1 = 4, /* key4 of key ladder1 - CP*/
    BCMD_KeyRamBuf_eKey5KeyLadder1 = 5, /* key5 of key ladder1 - CP*/

    BCMD_KeyRamBuf_eKey3KeyLadder2 = 6, /* key3 of key ladder2 - CA*/
    BCMD_KeyRamBuf_eKey4KeyLadder2 = 7, /* key4 of key ladder2 - CA*/
    BCMD_KeyRamBuf_eKey5KeyLadder2 = 8, /* key5 of key ladder2 - CA*/

    BCMD_KeyRamBuf_eKey3KeyLadder3 = 9,  /* key3 of key ladder3 */
    BCMD_KeyRamBuf_eKey4KeyLadder3 = 10, /* key4 of key ladder3 */
    BCMD_KeyRamBuf_eKey5KeyLadder3 = 11, /* key5 of key ladder3 */
    BCMD_KeyRamBuf_eReserved0 = 12,
    BCMD_KeyRamBuf_eReserved1 = 13, 
    BCMD_KeyRamBuf_eReserved2  = 14,       
    BCMD_KeyRamBuf_eReserved3 = 15,
    BCMD_KeyRamBuf_eReserved4 = 16,
    BCMD_KeyRamBuf_eFirstRam2048  = 17,   /* First 2048bit user ram*/
    BCMD_KeyRamBuf_eSecondRam2048 = 18,   /* Second 2048bit user ram*/
    BCMD_KeyRamBuf_eThirdRam2048  = 19,   /* Third 2048bit user ram*/

    BCMD_KeyRamBuf_eFirstRNRam1024 = 20,  /* First 1024 bit user ram*/
    BCMD_KeyRamBuf_eSecondRNRam1024= 21,  /* Second 1024 bit user ram*/
    BCMD_KeyRamBuf_eInvalid

}BCMD_KeyRamBuf_e;

/* swizzle type supported*/
typedef enum BCMD_SwizzleType_e
{
    BCMD_SwizzleType_eNoSwizzle,
    BCMD_SwizzleType_eSwizzle1,
    BCMD_SwizzleType_eSwizzle0,
    BCMD_SwizzleType_eInvalid
}BCMD_SwizzleType_e;


/* key destination block types*/
typedef enum BCMD_KeyDestBlockType_e
{
    BCMD_KeyDestBlockType_eCPDescrambler = 0, /* for use of CP descrambler */
    BCMD_KeyDestBlockType_eReserved0 = 0,     
    BCMD_KeyDestBlockType_eCA = 1,			  /* for CA */ 
    BCMD_KeyDestBlockType_eCPScrambler = 2,
    BCMD_KeyDestBlockType_eReserved1 = 2,     
    BCMD_KeyDestBlockType_eRmx = 3, /* Rmx or other use of CP scrambler */ 
    BCMD_KeyDestBlockType_eMem2Mem = 4,     /* for CP */
    BCMD_KeyDestBlockType_eHdmi = 5,        /* HDMI 0 */
    BCMD_KeyDestBlockType_eHdmi0 = 5,        /* HDMI 0 */
    BCMD_KeyDestBlockType_eHdmi1 = 6,        /* HDMI 1 */
    BCMD_KeyDestBlockType_eInvalid = 7
} BCMD_KeyDestBlockType_e;

/* key destination entry types*/
typedef enum BCMD_KeyDestEntryType_e
{
    BCMD_KeyDestEntryType_eOddKey   =  0,
    BCMD_KeyDestEntryType_eEvenKey  =  1,
    BCMD_KeyDestEntryType_eClearKey =  2,  
    BCMD_KeyDestEntryType_eReserved0 =  2,     
    BCMD_KeyDestEntryType_eIV       =  3,
    BCMD_KeyDestEntryType_eMaxNumber
} BCMD_KeyDestEntryType_e;

/* All the algorithms supported in XPT Security blocks*/
typedef enum BCMD_XptSecCryptoAlg_e	 
{  
	
    BCMD_XptSecCryptoAlg_eDvb = 0,      
    BCMD_XptSecCryptoAlg_eReserved0  =1,    
    BCMD_XptSecCryptoAlg_eMulti2Ecb = 2,
    BCMD_XptSecCryptoAlg_eMulti2Cbc = 3,
    BCMD_XptSecCryptoAlg_eDesEcb =    4,
    BCMD_XptSecCryptoAlg_eDesCbc =    5,
    BCMD_XptSecCryptoAlg_e3DesAbaEcb= 6,
    BCMD_XptSecCryptoAlg_e3DesAbaCbc= 7,
    BCMD_XptSecCryptoAlg_eAesCbc    = 8,

    BCMD_XptSecCryptoAlg_eReserved1 = 9,
    BCMD_XptSecCryptoAlg_eReserved2 = 10,
    BCMD_XptSecCryptoAlg_eReserved3 = 11,
    BCMD_XptSecCryptoAlg_eAesEcb =12,
    BCMD_XptSecCryptoAlg_eReserved12 =12,
    BCMD_XptSecCryptoAlg_eReserved5 = 13,    
    BCMD_XptSecCryptoAlg_eReserved6 = 14,
    BCMD_XptSecCryptoAlg_eMaxNumber = 15
}BCMD_XptSecCryptoAlg_e;

/* All the Supporting mem2mem algorithms */
typedef enum BCMD_M2MSecCryptoAlg_e	 
{
    BCMD_M2MSecCryptoAlg_e3DesAbaEcb = 0,
    BCMD_M2MSecCryptoAlg_e3DesAbcEcb = 1,
    BCMD_M2MSecCryptoAlg_eDesEcb     = 2,
    BCMD_M2MSecCryptoAlg_e3DesAbaCbc = 3, 
    BCMD_M2MSecCryptoAlg_eDesCbc     = 4,   
    BCMD_M2MSecCryptoAlg_eReserved0 = 5,    
    /* BCMD_M2MSecCryptoAlg_eC2Ecb = 6, */
    BCMD_M2MSecCryptoAlg_eReserved6 = 6,
    /* BCMD_M2MSecCryptoAlg_eC2Cbc = 7, */
    BCMD_M2MSecCryptoAlg_eReserved7 = 7,
    /* BCMD_M2MSecCryptoAlg_eCss   = 8, */
    BCMD_M2MSecCryptoAlg_eReserved8   = 8,
    BCMD_M2MSecCryptoAlg_eM6KE  = 9,
    BCMD_M2MSecCryptoAlg_eM6Ecb = 10,
    BCMD_M2MSecCryptoAlg_eM6Cbc = 11,
    BCMD_M2MSecCryptoAlg_eAes128Ecb = 12,
    BCMD_M2MSecCryptoAlg_eAes192Ecb = 13,
    BCMD_M2MSecCryptoAlg_eAes128Cbc = 14,
    BCMD_M2MSecCryptoAlg_eAesCounter = 15,
    BCMD_M2MSecCryptoAlg_e3DesAbcCbc = 16,
    BCMD_M2MSecCryptoAlg_eReserved3 = 17,
    BCMD_M2MSecCryptoAlg_eAes192Cbc = 18,
    BCMD_M2MSecCryptoAlg_eAes192Counter = 19,
    BCMD_M2MSecCryptoAlg_eReserved5 = 20,
    BCMD_M2MSecCryptoAlg_eRc4 = 21,
    BCMD_M2MSecCryptoAlg_eCbcMac = 22,
    BCMD_M2MSecCryptoAlg_eWMDrmPd = 23,
    BCMD_M2MSecCryptoAlg_eAes128G = 24,
    BCMD_M2MSecCryptoAlg_eHdDVD =25,
    BCMD_M2MSecCryptoAlg_eBrDVD =26,
    BCMD_M2MSecCryptoAlg_eMaxNumber = 27
}BCMD_M2MSecCryptoAlg_e;

/* key size enum */
typedef enum BCMD_KeySize_e
{
    BCMD_KeySize_e64,
    BCMD_KeySize_e128,
    BCMD_KeySize_e192,
    BCMD_KeySize_eInvalid
		
}BCMD_KeySize_e;
/* key size union used for user key and key ladder keys*/
typedef union BCMD_KeySize_u
{
    BCMD_KeySize_e eKeySize;
    unsigned short usKeySize; /* number of bits used only for 2048 bit user key*/
}BCMD_KeySize_u;

/* key ladder type enum*/
typedef enum BCMD_KeyLadderType_e
{
    BCMD_KeyLadderType_e1DES = 0, 
    BCMD_KeyLadderType_e3DESABA = 1,
    /* BCMD_KeyLadderType_eAES128, */
    BCMD_KeyLadderType_eReserved2 = 2, 
    BCMD_KeyLadderType_eInvalid
}BCMD_KeyLadderType_e;
/* key ladder operation enum*/
typedef enum BCMD_KeyLadderOp_e
{  
    BCMD_KeyLadderOp_eDecrypt,
    BCMD_KeyLadderOp_eEncrypt
}BCMD_KeyLadderOp_e;

typedef union BCMD_CryptoAlg_u
{
    BCMD_XptSecCryptoAlg_e eXptSecAlg;
    BCMD_M2MSecCryptoAlg_e eM2MSecAlg;
}BCMD_CryptoAlg_u;

/* All the key mode that Aegis supports - 
this is algorithm dependent*/
typedef enum BCMD_KeyMode_e
{
    BCMD_KeyMode_eRegular = 0,
    BCMD_KeyMode_eDes56 = 1,
    BCMD_KeyMode_eReserved0 =2,
    BCMD_KeyMode_eReserved1 =3,
    BCMD_KeyMode_eDvbConformance = 4,
    BCMD_KeyMode_eInvalid
}BCMD_KeyMode_e;

/* enum for key invalidation command*/
typedef enum BCMD_InvalidateKey_Flag_e
{ 
    BCMD_InvalidateKey_Flag_eSrcKeyOnly,
    BCMD_InvalidateKey_Flag_eDestKeyOnly,
    BCMD_InvalidateKey_Flag_eBoth
} BCMD_InvalidateKey_Flag_e;

/* command interface enums */
/* Output buffer table for all the following commands*/
/* common output header bytes(first 5 words - see bsp_s_commands.h)
|--------------|------------|------------|------------------------------------| 
|  31..24      | 23..16     | 15..8      | 7..0                               | 
|--------------|------------|------------|------------------------------------| 
|  unused      | unused     | unused     | BCMD_CommonBufferFields_eVerNum    | 
|--------------|------------|------------|------------------------------------| 
|  unused      | unused     | unused     | BCMD_CommonBufferFields_eOwnerId   | 
|--------------|------------|------------|------------------------------------| 
|  unused      | unused     | unused     | BCMD_CommonBufferFields_eContMode  | 
|--------------|------------|------------|------------------------------------| 
|  unused      | unused     | BCMD_CommonBufferFields_eParamLen               | 
|--------------|------------|------------|------------------------------------| 
|  unused      | unused     | unused     | BCMD_CommonBufferFields_eStatus    | 
|--------------|------------|------------|------------------------------------| 
|  unused      | unused     | unused     | BCMD_DATA_OUTBUF                   | 
|--------------|------------|------------|------------------------------------| 
*/
#define BCMD_DATA_OUTBUF        (6 << 2)  /* output data start from */
/* Command buffer information for BCMD_KeyLoad_InitKeySlot*/ 
/*
|--------------|-------------------|-------------------|-----------------------| 
|  31..24      | 23..16            | 15..8             | 7..0                  | 
|--------------|-------------------|-------------------|-----------------------| 
|  unused      | unused            | unused            | BCMD_KeySlot_InCmdInit| 
|--------------|-------------------|-------------------|-----------------------| 
*/
#define BCMD_KeySlot_InCmdInit  (5 << 2) + 3         /* 8 bit index*/

/* Command buffer information for BCMD_KeyLoad_ConfigKeyPointerTable for 7401A0 */
/*
|--------------|------------|------------|------------------------------------| 
|  31..24      | 23..16     | 15..8      | 7..0                               | 
|--------------|------------|------------|------------------------------------| 
|  SetSlotB    | unused     | unused     | BCMD_KeyPointer_InCmdCfg_ePidChan  | 
|--------------|------------|------------|------------------------------------| 
|  unused      | unused     | unused     | BCMD_KeyPointer_InCmdCfg_eSlotType | 
|--------------|------------|------------|------------------------------------| 
|  unused      | unused     | unused     | BCMD_KeyPointer_InCmdCfg_eSlotNumber| 
|--------------|------------|------------|------------------------------------| 
*/
/* Additional command buffers for 7400A0 and beyond */
/*
|--------------|------------|------------|------------------------------------| 
|  31..24      | 23..16     | 15..8      | 7..0                               | 
|--------------|------------|------------|------------------------------------| 
|  unused      | unused     | unused     | BCMD_KeyPointer_InCmdCfg_eSlotTypeB | 
|--------------|------------|------------|------------------------------------| 
|  unused      | unused     | unused     | BCMD_KeyPointer_InCmdCfg_eSlotNumberB| 
|--------------|------------|------------|------------------------------------| 
|  unused      | unused     | unused     | BCMD_KeyPointer_InCmdCfg_eKeyPointerSel| 
|--------------|------------|------------|------------------------------------| 
*/

enum BCMD_KeyPointer_InCmdCfg_e
{
    BCMD_KeyPointer_InCmdCfg_ePidChan = (5 << 2),   /* Bit31 is for slot B, LSbyte is for pidchan*/
    BCMD_KeyPointer_InCmdCfg_eSlotType = (6 << 2) + 3,  /* 8 bit index*/
    BCMD_KeyPointer_InCmdCfg_eSlotNumber =(7 << 2) + 3,  /* 8 bit index*/    
    BCMD_KeyPointer_InCmdCfg_eSlotTypeB = (8 << 2) + 3,  /* 8 bit index*/
    BCMD_KeyPointer_InCmdCfg_eSlotNumberB =(9 << 2) + 3,  /* 8 bit index*/
    BCMD_KeyPointer_InCmdCfg_eKeyPointerSel = (10 << 2) + 3 /* 8 bit index*/
};
/* Command buffer information for BCMD_KeyLoad_ConfigAlg for 7401A0 */
/*
|--------------|------------|------------|------------------------------------| 
|  31..24      | 23..16     | 15..8      | 7..0                               | 
|--------------|------------|------------|------------------------------------| 
|  unused      | unused     | unused     | BCMD_Alg_InCmdCfg_eKeySrc          | 
|--------------|------------|------------|------------------------------------| 
|  unused      | unused     | unused     | BCMD_Alg_InCmdCfg_eBlkType         | 
|--------------|------------|------------|------------------------------------| 
|  unused      | unused     | unused     | BCMD_Alg_InCmdCfg_eEntryType       | 
|--------------|------------|------------|------------------------------------| 
|  unused      | unused     | unused     | BCMD_Alg_InCmdCfg_eSlotType        | 
|--------------|------------|------------|------------------------------------| 
|  unused      | unused     | unused     | BCMD_Alg_InCmdCfg_eSlotNumber      | 
|--------------|------------|------------|------------------------------------| 
|                  BCMD_Alg_InCmdCfg_eOperaModeWord0                          | 
|--------------|------------|------------|------------------------------------| 
|                  BCMD_Alg_InCmdCfg_eOperaModeWord1                          | 
|--------------|------------|------------|------------------------------------| 
|  unused      | unused     | unused     |BCMD_Alg_InCmdCfg_eGlobalXptSecureCtrl| 
|--------------|------------|------------|------------------------------------| 
|  unused      | unused     | unused     | BCMD_Alg_InCmdCfg_eKeyMode         | 
|--------------|------------|------------|------------------------------------| 
*/

/* Command buffer information for BCMD_KeyLoad_ConfigAlg for 7400A0*/
/*
|--------------|------------|------------|------------------------------------| 
|  31..24      | 23..16     | 15..8      | 7..0                               | 
|--------------|------------|------------|------------------------------------| 
|  unused      | unused     | unused     | BCMD_Alg_InCmdCfg_eKeySrc          | 
|--------------|------------|------------|------------------------------------| 
|  unused      | unused     | unused     | BCMD_Alg_InCmdCfg_eBlkType         | 
|--------------|------------|------------|------------------------------------| 
|  unused      | unused     | unused     | BCMD_Alg_InCmdCfg_eEntryType       | 
|--------------|------------|------------|------------------------------------| 
|  unused      | unused     | unused     | BCMD_Alg_InCmdCfg_eSlotType        | 
|--------------|------------|------------|------------------------------------| 
|  unused      | unused     | unused     | BCMD_Alg_InCmdCfg_eSlotNumber      | 
|--------------|------------|------------|------------------------------------| 
|                  BCMD_Alg_InCmdCfg_eOperaModeWord0                          | 
|--------------|------------|------------|------------------------------------| 
|                  BCMD_Alg_InCmdCfg_eOperaModeWord1                          | 
|--------------|------------|------------|------------------------------------| 
|                  BCMD_Alg_InCmdCfg_eOperaModeWord2                          | 
|--------------|------------|------------|------------------------------------| 
|                  BCMD_Alg_InCmdCfg_eOperaModeWord3                          |
|--------------|------------|------------|------------------------------------| 
|  unused      | unused     | unused     | BCMD_Alg_InCmdCfg_eKeyMode         | 
|--------------|------------|------------|------------------------------------| 
*/

enum BCMD_Alg_InCmdCfg_e
{
    BCMD_Alg_InCmdCfg_eKeySrc = (5 << 2) + 3,               /* 8 bit index*/
    BCMD_Alg_InCmdCfg_eBlkType = (6 << 2) + 3,              /* 8 bit index*/
    BCMD_Alg_InCmdCfg_eEntryType = (7 << 2) + 3,            /* 8 bit index*/
    BCMD_Alg_InCmdCfg_eSlotType = (8 << 2) + 3,             /* 8 bit index*/
    BCMD_Alg_InCmdCfg_eSlotNumber = (9 << 2) + 3,           /* 8 bit index*/
    BCMD_Alg_InCmdCfg_eOperaModeWord0 = (10 << 2),            /* 32 bit index*/
    BCMD_Alg_InCmdCfg_eOperaModeWord1 = (11 << 2),            /* 32 bit index*/ 
    BCMD_Alg_InCmdCfg_eOperaModeWord2 = (12 << 2),            /* 32 bit index*/
    BCMD_Alg_InCmdCfg_eOperaModeWord3 = (13 << 2)            /* 32 bit index*/   
    /*
    ,
    BCMD_Alg_InCmdCfg_eKeyMode = (14 << 2) + 3               
    */
};

/* Command buffer information for BCMD_KeyLoad_ConfigMulti2 */
/*
|--------------|------------|------------|------------------------------------| 
|  31..24      | 23..16     | 15..8      | 7..0                               | 
|--------------|------------|------------|------------------------------------| 
|                      BCMD_Multi2_InCmdCfg_eRoundCount                       | 
|--------------|------------|------------|------------------------------------| 
|                      BCMD_Multi2_InCmdCfg_eSystemKeys (8-32 bits word)      | 
|--------------|------------|------------|------------------------------------| 
|  unused      | unused     | unused     | BCMD_Multi2_InCmdCfg_eWhichSysKey  | 
|--------------|------------|------------|------------------------------------| 
*/
enum BCMD_InCmdCfgMulti2_e
{
    BCMD_Multi2_InCmdCfg_eRoundCount = (5 << 2),       /* 32 bit index*/
    BCMD_Multi2_InCmdCfg_eSystemKeys = (6 << 2) ,       /* 32 bit index - 256 bit */
    BCMD_Multi2_InCmdCfg_eWhichSysKey = (14 << 2) + 3    /* 8 bit index*/
};

/* common fields for root key generation for BCMD_KeyLoad_GenRootKey
and BCMD_KeyLoad_GenerateRouteKey*/
/*
|--------------|------------|---------|----------------------------------------| 
|  31..24      | 23..16     | 15..8   | 7..0                                   | 
|--------------|------------|---------|----------------------------------------| 
|  unused      | unused     | unused  | BCMD_CommonGenKey_InCmd_eKeyLadderType | 
|--------------|------------|---------|----------------------------------------| 
|  unused      | unused     | unused  | BCMD_CommonGenKey_InCmd_eRootKeySrc    | 
|--------------|------------|---------|----------------------------------------| 
|  unused      | unused     | unused  | BCMD_CommonGenKey_InCmd_eSwizzle1IndexSel| 
|--------------|------------|---------|----------------------------------------| 
|  unused      | unused     | unused  | BCMD_CommonGenKey_InCmd_eSwizzleType   | 
|--------------|------------|---------|----------------------------------------| 
|  unused      | unused     | unused  | BCMD_CommonGenKey_InCmd_eCusKeySelL    | 
|--------------|------------|---------|----------------------------------------| 
|  unused      | unused     | unused  | BCMD_CommonGenKey_InCmd_eKeyVarL       | 
|--------------|------------|---------|----------------------------------------| 
|  unused      | unused     | unused  | BCMD_CommonGenKey_InCmd_eCusKeySelH    | 
|--------------|------------|---------|----------------------------------------| 
|  unused      | unused     | unused  | BCMD_CommonGenKey_InCmd_eKeyVarH       | 
|--------------|------------|---------|----------------------------------------| 
*/
enum BCMD_CommonGenRootKey_e
{
    BCMD_CommonGenKey_InCmd_eKeyLadderType = (5 << 2) + 3,	   /* 8 bit index*/ 
    BCMD_CommonGenKey_InCmd_eRootKeySrc = (6 << 2) + 3,        /* 8 bit index*/
    BCMD_CommonGenKey_InCmd_eSwizzle1IndexSel = (7 << 2) + 3,  /* 8 bit index*/
    BCMD_CommonGenKey_InCmd_eSwizzleType = (8 << 2) + 3,       /* 8 bit index*/
    BCMD_CommonGenKey_InCmd_eCusKeySelL = (9 << 2) + 3,        /* 8 bit index*/
    BCMD_CommonGenKey_InCmd_eKeyVarL = (10 << 2) + 3,          /* 8 bit index*/
    BCMD_CommonGenKey_InCmd_eCusKeySelH = (11 << 2) + 3,       /* 8 bit index*/
    BCMD_CommonGenKey_InCmd_eKeyVarH = (12 << 2) + 3          /* 8 bit index*/
};

/* BCMD_KeyLoad_GenerateRouteKey input buffer enum */
/*
|--------------|------------|------------|------------------------------------| 
|  31..24      | 23..16     | 15..8      | 7..0                               | 
|--------------|------------|------------|------------------------------------| 
|  unused      | unused     | unused     | BCMD_GenKey_InCmd_eKeySrc          | 
|--------------|------------|------------|------------------------------------| 
|  unused      | unused     | unused     | BCMD_GenKey_InCmd_eKeyBuffer       | 
|--------------|------------|------------|------------------------------------| 
|  unused      | unused     | unused     | BCMD_GenKey_InCmd_eKeyLadderOpera  | 
|--------------|------------|------------|------------------------------------| 
|           BCMD_GenKey_InCmd_eProcIn (6 - 32 bits words )                    | 
|--------------|------------|------------|------------------------------------| 
|  unused      | unused     | unused     | BCMD_GenKey_InCmd_eKeySize         | 
|--------------|------------|------------|------------------------------------| 
|  unused      | unused     | unused     | BCMD_GenKey_InCmd_eRouteKeyFlag    | 
|--------------|------------|------------|------------------------------------| 
|  unused      | unused     | unused     | BCMD_GenKey_InCmd_eBlkType         | 
|--------------|------------|------------|------------------------------------| 
|  unused      | unused     | unused     | BCMD_GenKey_InCmd_eEntryType       | 
|--------------|------------|------------|------------------------------------| 
|  unused      | unused     | unused     | BCMD_GenKey_InCmd_eKeySlotType     | 
|--------------|------------|------------|------------------------------------|  
|  unused      | unused     | unused     | BCMD_GenKey_InCmd_eKeySlotNumber   | 
|--------------|------------|------------|------------------------------------| 
*/
enum BCMD_GenKey_InCmd_e
{
    BCMD_GenKey_InCmd_eKeySrc = (13 << 2) + 3,		            /* 8 bit index*/
    BCMD_GenKey_InCmd_eKeyBuffer = (14 << 2) + 3,                /* 8 bit index*/
    BCMD_GenKey_InCmd_eKeyLadderOpera = (15 << 2) + 3,           /* 8 bit index*/
    BCMD_GenKey_InCmd_eProcIn = (16 << 2),                       /* 32 bit index*/
    BCMD_GenKey_InCmd_eKeySize = (24 << 2) + 3,                 /* 8 bit index*/
    BCMD_GenKey_InCmd_eRouteKeyFlag = (25 << 2) + 3,            /* 8 bit index*/
    BCMD_GenKey_InCmd_eBlkType = (26 << 2) + 3,                 /* 8 bit index*/
    BCMD_GenKey_InCmd_eEntryType = (27 << 2) + 3,               /* 8 bit index*/
    BCMD_GenKey_InCmd_eKeySlotType =(28 << 2) + 3,              /* 8 bit index*/
    BCMD_GenKey_InCmd_eKeySlotNumber =(29 << 2) + 3,             /* 8 bit index*/
    BCMD_GenKey_InCmd_eKeyMode = (30 << 2) + 3                  /* 8 bit index*/
};
/* BCMD_KeyLoad_GenRootKey input buffer enum */
/*
|--------------|------------|------------|------------------------------------| 
|  31..24      | 23..16     | 15..8      | 7..0                               | 
|--------------|------------|------------|------------------------------------| 
|        BCMD_GenRootKey_InCmd_eWrappedKey (4 - 32 bit words)                 | 
|--------------|------------|------------|------------------------------------| 
|        BCMD_GenRootKey_InCmd_eDestAndDigest(4 - 32 bit words)               | 
|--------------|------------|------------|------------------------------------| 
|  unused      | unused     | unused     | BCMD_GenRootKey_InCmd_eTDesOpera   | 
|--------------|------------|------------|------------------------------------| 
|  unused      | unused     | unused     | BCMD_GenRootKey_InCmd_eMaxOTPErr   | 
|--------------|------------|------------|------------------------------------| 
*/
enum BCMD_GenRootKey_InCmd_e
{
    BCMD_GenRootKey_InCmd_eWrappedKey = (13 << 2),     /* 128 bits here*/
    BCMD_GenRootKey_InCmd_eDestAndDigest = (17 << 2) , /* 32 bit dest + 96 bits digest*/	  
    BCMD_GenRootKey_InCmd_eTDesOpera = (21 << 2) + 3,  /* 8 bit index*/
    BCMD_GenRootKey_InCmd_eMaxOTPErr = (22 << 2) + 3    /* 8 bit - only allow 0 or 1 */
   };

/* Command buffer information for BCMD_KeyLoad_RouteUserKey */
/*
|--------------|------------|------------|------------------------------------| 
|  31..24      | 23..16     | 15..8      | 7..0                               | 
|--------------|------------|------------|------------------------------------| 
|  unused      | unused     | unused     | BCMD_LoadUseKey_InCmd_eKeyBuffer   | 
|--------------|------------|------------|------------------------------------| 
|  unused      | unused     | unused     | BCMD_LoadUseKey_InCmd_eKeyLength   | 
|--------------|------------|------------|------------------------------------| 
|           BCMD_LoadUseKey_InCmd_eKeyData (64 - 32 bit words)                | 
|--------------|------------|------------|------------------------------------| 
|  unused      | unused     | unused     | BCMD_LoadUseKey_InCmd_eRouteKeyFlag| 
|--------------|------------|------------|------------------------------------| 
|  unused      | unused     | unused     | BCMD_LoadUseKey_InCmd_eBlkType     | 
|--------------|------------|------------|------------------------------------| 
|  unused      | unused     | unused     | BCMD_LoadUseKey_InCmd_eEntryType   | 
|--------------|------------|------------|------------------------------------| 
|  unused      | unused     | unused     | BCMD_LoadUseKey_InCmd_eKeySlotType | 
|--------------|------------|------------|------------------------------------| 
|  unused      | unused     | unused     | BCMD_LoadUseKey_InCmd_eKeySlotNumber| 
|--------------|------------|------------|------------------------------------| 
*/
enum BCMD_LoadUseKey_InCmd_e
{
    BCMD_LoadUseKey_InCmd_eKeyBuffer = (5 << 2) + 3,      /* 8 bit index*/
    BCMD_LoadUseKey_InCmd_eKeyLength = (6 << 2) + 2,      /* 16 bit index*/
    BCMD_LoadUseKey_InCmd_eKeyData = (7 << 2) ,           /* 32 bits max 2048 bits - 64 words*/	
    BCMD_LoadUseKey_InCmd_eRouteKeyFlag = (71 << 2) + 3,  /* 8 bit index*/
    BCMD_LoadUseKey_InCmd_eBlkType = (72 << 2) + 3,       /* 8 bit index*/
    BCMD_LoadUseKey_InCmd_eEntryType = (73 << 2) + 3,     /* 8 bit index*/
    BCMD_LoadUseKey_InCmd_eKeySlotType =(74 << 2) + 3,    /* 8 bit index*/
    BCMD_LoadUseKey_InCmd_eKeySlotNumber =(75 << 2) + 3,  /* 8 bit index*/
    BCMD_LoadUseKey_InCmd_eKeyMode = (76 << 2) + 3       /* 8 bit index*/
   };
/* command buffer enums for BCMD_KeyLadder_InvalidateKey*/
/*
|--------------|------------|---------|----------------------------------------| 
|  31..24      | 23..16     | 15..8   | 7..0                                   | 
|--------------|------------|---------|----------------------------------------| 
|  unused      | unused     | unused  | BCMD_InvalidateKey_InCmd_eKeyFlag      | 
|--------------|------------|---------|----------------------------------------| 
|  unused      | unused     | unused  | BCMD_InvalidateKey_InCmd_eKeySrc       | 
|--------------|------------|---------|----------------------------------------| 
|  unused      | unused     | unused  | BCMD_InvalidateKey_InCmd_eBlkType      | 
|--------------|------------|---------|----------------------------------------| 
|  unused      | unused     | unused  | BCMD_InvalidateKey_InCmd_eEntryType    | 
|--------------|------------|---------|----------------------------------------| 
|  unused      | unused     | unused  | BCMD_InvalidateKey_InCmd_eKeySlotType  | 
|--------------|------------|---------|----------------------------------------| 
|  unused      | unused     | unused  | BCMD_InvalidateKey_InCmd_eKeySlotNumber| 
|--------------|------------|---------|----------------------------------------| 
*/
enum BCMD_InvalidateKey_InCmd_e
{
    BCMD_InvalidateKey_InCmd_eKeyFlag = (5 << 2) + 3,        /* 8 bit index*/
    BCMD_InvalidateKey_InCmd_eKeySrc = (6 << 2) + 3,         /* 8 bit index*/
    BCMD_InvalidateKey_InCmd_eBlkType = (7 << 2) + 3,        /* 8 bit index*/
    BCMD_InvalidateKey_InCmd_eEntryType = (8 << 2) + 3,      /* 8 bit index*/
    BCMD_InvalidateKey_InCmd_eKeySlotType = (9 << 2) + 3,    /* 8 bit index*/
    BCMD_InvalidateKey_InCmd_eKeySlotNumber = (10 << 2) + 3  /* 8 bit index*/
};
/* command buffer defines for BCMD_GetProcOut*/
/*
|--------------|------------|---------|----------------------------------------| 
|  31..24      | 23..16     | 15..8   | 7..0                                   | 
|--------------|------------|---------|----------------------------------------| 
|  unused      | unused     | unused  |  BCMD_ProcOut_InCmd_Keysrc             | 
|--------------|------------|---------|----------------------------------------|
|              BCMD_ProcOut_InCmd_ProcIn 4 32 bits words                       | 
|--------------|------------|---------|----------------------------------------|  
*/
enum BCMD_ProcOut_InCmd_e
{
    BCMD_ProcOut_InCmd_eKeySrc = (5 << 2)+ 3,        /* 8 bit index */
    BCMD_ProcOut_InCmd_eProcIn = 6 << 2             /* 4 - 32 bit words*/ 
};

#endif

