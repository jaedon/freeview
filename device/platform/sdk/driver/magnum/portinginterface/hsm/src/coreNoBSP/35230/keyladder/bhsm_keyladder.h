/***************************************************************************
 *     (c)2007-2008 Broadcom Corporation
 *
 *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 *  no license (express or implied), right to use, or waiver of any kind with respect to the
 *  Software, and Broadcom expressly reserves all rights in and to the Software and all
 *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 *  Except as expressly set forth in the Authorized License,
 *
 *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 *  and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 *  USE OR PERFORMANCE OF THE SOFTWARE.
 *
 *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 *  ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: bhsm_keyladder.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 9/12/12 12:41a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hsm/src/coreNoBSP/35230/keyladder/bhsm_keyladder.h $
 * 
 * Hydra_Software_Devel/1   9/12/12 12:41a atruong
 * SWSECURITY-175: Remove symbolic links in HSM PI
 * 
 * Hydra_Software_Devel/2   2/23/11 3:21p haoboy
 * SW7550-692: changed BHSM_SolitarySelect_eSa to
 * BHSM_SolitarySelect_eCbcXorEnc0
 * 
 * Hydra_Software_Devel/1   1/21/11 10:00a haoboy
 * SW7550-666:initial version
 * 
 * Hydra_Software_Devel/7   5/25/10 6:03p qcheng
 * SW7408-82 : Add security support for 7408 A0
 * 
 * Hydra_Software_Devel/6   5/11/10 7:18p qcheng
 * SW7550-419 : 7550's BHSM_FreeCAKeySlot appears to have a wrong API
 * 
 * Hydra_Software_Devel/5   4/2/10 7:36p qcheng
 * SW7550-353: PVR encryption feature is required for 7540 production with
 * PVR function
 * 
 * Hydra_Software_Devel/4   3/22/10 7:05p qcheng
 * SW7408-82 : Add security support for 7408 A0
 * 
 * Hydra_Software_Devel/3   3/4/10 11:40a qcheng
 * SW7408-82 : Add security support for 7408 A0
 * 
 * Hydra_Software_Devel/2   1/15/10 6:26p qcheng
 * SW7550-188: Support HDMI/HDCP for 7550
 * 
 * Hydra_Software_Devel/1   12/4/09 5:43p qcheng
 * SW7550-102 : Add Security support for 7550
 *
 **************************************************************************/
#ifndef	BHSM_KEY_LADDER_H__
#define	BHSM_KEY_LADDER_H__

/* added to remove header file dependency */
#include "bhsm_priv.h"
#include "bhsm.h"
#include "bhsm_datatypes.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BHSM_MULTI2_SYS_KEY_SIZE	 32
#define BHSM_CW_IV_MAX_SIZE	 16
#define BHSM_KEYX_PROCIN_SIZE	 	16
#define BHSM_CW_PROCIN_SIZE	 	8
#define BHSM_HDMIKEY_PROCIN_SIZE		8
#define BHSM_HDMIKEY_SERIALIZER_BASE 0x80


/**
Summary:
This enum defines the supported crypto alogrithms.
**/

typedef enum BHSM_Algorithm
{
    BHSM_Algorithm_eDvb,    /* DVB will only be used by CA descrambler */
    BHSM_Algorithm_eReserved1,
    BHSM_Algorithm_eMulti2,
    BHSM_Algorithm_eDes,
    BHSM_Algorithm_e3DesAba,
    BHSM_Algorithm_e3DesAbc,
    BHSM_Algorithm_eAes,
    BHSM_Algorithm_eReserved7,
    BHSM_Algorithm_eAes192,
    BHSM_Algorithm_eTivo,

    /* The 4 algorithms below are only supported for M2M */
    BHSM_Algorithm_eC2,
    BHSM_Algorithm_eCss,
    BHSM_Algorithm_eM6Ke,
    BHSM_Algorithm_eM6,

    /* added WMDRM_PD */
    BHSM_Algorithm_eRc4,
    BHSM_Algorithm_eCbcMac,
    BHSM_Algorithm_eWMDrmPd,

    BHSM_Algorithm_eAes128G,
    BHSM_Algorithm_eHdDVD,
    BHSM_Algorithm_eBrDVD,

    /* Add new security algorithm definition before this line */
    BHSM_Algorithm_eMax
}   BHSM_Algorithm_e;


/**
Summary:
This enum defines the supported crypto alogrithm variants.
**/

typedef enum BHSM_AlgorithmVariant
{
    BHSM_AlgorithmVariant_eEcb = 0,
    BHSM_AlgorithmVariant_eXpt = 0,  /* for BHSM_Algorithm_eDvb, scramble level */
    BHSM_AlgorithmVariant_eCbc = 1,
    BHSM_AlgorithmVariant_ePes = 1,  /* for BHSM_Algorithm_eDvb, scramble level */
    BHSM_AlgorithmVariant_eCounter = 2,
    BHSM_AlgorithmVariant_eRCbc    = 3,

    /* Add new algorithm variant definition before this line */
    BHSM_AlgorithmVariant_eMax
}   BHSM_AlgorithmVariant_e;

/**
Summary:
This enum defines the supported termination modes.
For pairs of Enums, first one for ASKM; second for regular
**/
typedef enum BHSM_TerminationMode
{
    BHSM_TerminationMode_eClear                 = 0,
    BHSM_TerminationMode_eCbcResidual           = 1,
    BHSM_TerminationMode_eBlock                 = 1,
    BHSM_TerminationMode_eReserved2             = 2,
    BHSM_TerminationMode_eCipherStealing        = 2,
    BHSM_TerminationMode_eCtsCpcm               = 3,
    BHSM_TerminationMode_eCipherStealingComcast = 3,
    BHSM_TerminationMode_eReserved4             = 4,
    BHSM_TerminationMode_eReserved5             = 5,

    /* Add new termination mode definition before this line */
    BHSM_TerminationMode_eMax
}   BHSM_TerminationMode;

typedef enum BHSM_IVMode
{
    BHSM_IVMode_eRegular,
    BHSM_IVMode_eReserved1,
    BHSM_IVMode_eReserved2,

    /* Add new IV mode definition before this line */
    BHSM_IVMode_eMax
}   BHSM_IVMode;

typedef enum BHSM_SolitarySelect
{
    BHSM_SolitarySelect_eClear=0,
    /*BHSM_SolitarySelect_eSa,*/
    BHSM_SolitarySelect_eCbcXorEnc0=1,
    BHSM_SolitarySelect_eCbcXorIV=2,
    BHSM_SolitarySelect_eCbcXorShortIV=3,
    BHSM_SolitarySelect_eCbcXorEncIV=4,

    /* Add new Solitary Select definition before this line */
    BHSM_SolitarySelect_eMax

}   BHSM_SolitarySelect;

typedef enum BHSM_KeyAddr {
	BHSM_KeyAddr_eOddKeyMode1 = 0x00,
	BHSM_KeyAddr_eOddKeyMode2 = 0x01,
	BHSM_KeyAddr_eEvenKeyMode1 = 0x02,
	BHSM_KeyAddr_eEvenKeyMode2 = 0x03,
	BHSM_KeyAddr_eClearKeyMode1 = 0x04,
	BHSM_KeyAddr_eClearKeyMode2 = 0x05,
	BHSM_KeyAddr_eIva = 0x06,
	BHSM_KeyAddr_eIvb = 0x07,
	BHSM_KeyAddr_eCaKeyOdd1 = 0x08,
	BHSM_KeyAddr_eCaKeyOdd2 = 0x09,
	BHSM_KeyAddr_eCaKeyEven1 = 0x0a,
	BHSM_KeyAddr_eCaKeyEven2 = 0x0b,
	BHSM_KeyAddr_eCpKeyOdd1 = 0x0c,
	BHSM_KeyAddr_eCpKeyOdd2 = 0x0d,
	BHSM_KeyAddr_eCpKeyEven1 = 0x0e,
	BHSM_KeyAddr_eCpKeyEven2 = 0x0f,
	BHSM_KeyAddr_eM2MKeyMode = 0x00,
	BHSM_KeyAddr_eM2MIva = 0x01,
	BHSM_KeyAddr_eM2MIvb = 0x02,
	BHSM_KeyAddr_eM2MKey1 = 0x03,
	BHSM_KeyAddr_eM2MKey2 = 0x04,
	BHSM_KeyAddr_eMaxNumber = 0x10
} BHSM_KeyAddr_e;



typedef enum BHSM_M2MKeyAddr {
	BHSM_M2MKeyAddr_eKeyMode = 0x00,
	BHSM_M2MKeyAddr_eIv0 = 0x01,
	BHSM_M2MKeyAddr_eIv1 = 0x02,
	BHSM_M2MKeyAddr_eKey1 = 0x03,
	BHSM_M2MKeyAddr_eKey2 = 0x04,
	BHSM_M2MKeyAddr_eMaxNumber = 0x05
} BHSM_M2MKeyAddr_e;


/**************************************************************************************************
Summary:

Description:
this structure is a sub member in BHSM_ConfigAlgorithmIO.  All the setting will be translated into word32 
format and send to BSP configuring the key slot header area controlling bits, such as

<verbatim>
	Bit[7:0]- Alg_Sel: CA/CPD/CPS/RMX algorithm supported
	Bit[12:8]- Key_Offset: 5 bits of Key offset
	Bit[14:13]- Residue_Mode_Ctrl: 0x0=Unscrambled, 0x1=Residue block,0x2=Cipher text stealing
	Bit[13]-DVB_Scramble_Level:  0 = pes level, 1 = transport
	Bit[15]- Revered
	Bit[16]-Restr_En
	Bit[17]-Global_En 
	Bit[18]-Restr_SC_Mod
	Bit[19]-Global_SC_Mod
	Bit[20]-MSC Mode Selection for algorithms that use MSC.  0 - Select DTV_MSC0/MPEG_MSC0  
	                                                                              1 - Select DTV_MSC1/MPEG_MSC1 
	Bit[22:21]- SC value, 2 bits to be set if Global_SC_Mod or Restrict_SC_Mod is set
	
</verbatim>
<verbatim>
	Global Ctrl Bits:
	Bit[0]- Enc_SC_Polarity: 
	When Global_SC_Mod or Restr_SC_Mod for CPS is set, this bit determines the SC polarity to set to:
	For MPEG packets, Global_SC_Mod or Restr_SC_Mod = 1 will set the SC bits to 11. SC bits will be
	set to 10 otherwise.
	For non-MPEG packets, Global_SC_Mod or Restr_SC_Mod=1 is copied to the SC flag.
	Bit [2:1]- Multi2_Syskey_Sel: Multi 2 System key Select 
	Bit[3]- Atsc_Scramble_Ctrl
	Bit[4]- Atsc_Mod
	Bit[5]- Global_Drop_Packet_Ctrl: set to 1 to drop
	Bit[6]- Restr_Drop_Packet_Ctrl
	Bit[7]- Global_Region_Overwrite: when set, XPT secure modules allows all packets(restricted and 
			global) to go to global region. 
	Bit[8] CPD_Disable, set to 1 to disable CPD
	Bit[9] CPS_Disable, set to 1 to disable CPS(or Remux)
	Bit[10:31]-Reserved
</verbatim>
<verbatim>
	Bit[7:0] AlgSel, MEM2MEM, algorithm supported
	Bit[12:8]  5 bits of Key offset
	Bit[14:13] Mem2mem AES counter mode  key mode select 
	Bit[15]: reserved
	Bit[16:17] Authctrl, (decryption = 0x02, encryption = 0x01; pass = 0)
</verbatim>

See Also:
BHSM_ConfigAlgorithmIO
**************************************************************************************************/ 	
typedef struct BHSM_CryptoAlg {
	BHSM_KeyAddr_e 		eKeyOffset;

	/* Module Key Mode Setting */
	/* In: select the algorithm among BCMD_XptM2MSecCryptoAlg_e */
	BHSM_Algorithm_e		algorithm;	

	/* In:  set cipher mode for non-DVB algorithms or scrambling level for DVB, see BCMD_CipherModeSelect_e */
	BHSM_AlgorithmVariant_e			cipherDVBMode; 

	/* In:  Termination Mode for selected algorithm */
	BHSM_TerminationMode			terminationMode;

	/* In:  IV Mode for the selected algorithm */
	BHSM_IVMode					IVMode;

	/* In: Mode for solitary block processing */
	BHSM_SolitarySelect			solitaryMode;

	/* In:	MSC Mode Selection for algorithms that use MSC.  
	         0 - Select DTV_MSC0/MPEG_MSC0   - 1 - Select DTV_MSC1/MPEG_MSC1 */
	bool							bMscSel;	

	/* In: 1 - Enable scrambling/descrambling of restricted packets */
	bool							bRestrictEnable;  

	/* In: 1 - Enable scrambling/descrambling of global packets */	
	bool							bGlobalEnable;

	/* In: 1 - Modify SC for restricted packets according to SC_VALUE  */      
	bool							bRestrictScMod;  

	/* In: 1 - Modify SC for global packets according to SC_VALUE  */ 	
	bool							bGlobalScMod;    

	/* In:	SC value to replace SC field in packet when Global_SC_MOD or 
			RESTR_SC_MOD is set, LSB 2 bits are valid  */
	unsigned char					uScValue;	

}BHSM_CryptoAlg_t;


typedef struct BHSM_CaCpCryptoAlg {
	BHSM_KeyAddr_e 		eKeyOffset;

	/* Module Key Mode Setting */
	/* In: select the algorithm among BCMD_XptM2MSecCryptoAlg_e */
	BHSM_Algorithm_e		algorithm;	

	/* In:  set cipher mode for non-DVB algorithms or scrambling level for DVB, see BCMD_CipherModeSelect_e */
	BHSM_AlgorithmVariant_e			cipherDVBMode; 

	/* In:  Termination Mode for selected algorithm */
	BHSM_TerminationMode			terminationMode;

	/* In:  IV Mode for the selected algorithm */
	BHSM_IVMode					IVMode;

	/* In: Mode for solitary block processing */
	BHSM_SolitarySelect			solitaryMode;

	/* In:	MSC Mode Selection for algorithms that use MSC.  
	         0 - Select DTV_MSC0/MPEG_MSC0   - 1 - Select DTV_MSC1/MPEG_MSC1 */
	bool							bMscSel;	

	/* In: 1 - Enable scrambling/descrambling of restricted packets */
	bool							bRestrictEnable;  

	/* In: 1 - Enable scrambling/descrambling of global packets */	
	bool							bGlobalEnable;

	/* In: 1 - Modify SC for restricted packets according to SC_VALUE  */      
	bool							bRestrictScMod;  

	/* In: 1 - Modify SC for global packets according to SC_VALUE  */ 	
	bool							bGlobalScMod;    

	/* In:	SC value to replace SC field in packet when Global_SC_MOD or 
			RESTR_SC_MOD is set, LSB 2 bits are valid  */
	unsigned char					uScValue;	

	/* Global Key Mode Setting */

	/* In: Multi2 system key destination - Select 1 of 4 Multi2 system key*/	
	unsigned char					ucMulti2KeySelect;

	/* In: How SC bits are intepreted: - 0 - MPEG_SC_MODE  - 1 - ATSC_SC_MODE */	
	bool							bAtscScrambleCtrl;

	/* In: 1 - modify SC clear in ATSC (01) to SC clear in MPEG (00) */
	bool							bAtscMod;	

	/* In: 1 - drop global packets using this keyslot  */
	bool							bGlobalDropPktCtrl;

	/* In: 1 - drop restricted packets using this keyslot  */
	bool							bRestrictDropPktCtrl;

	/* In: 0 - allow only global packets to go to global region */
	bool							bGlobalRegOverwrite; 

	/* In: 1 - encrypt packets destined for RAVE in CPS, before RAVE */
	bool							bEncryptBeforeRave; 

}BHSM_CaCpCryptoAlg_t;





typedef enum BHSM_M2mAuthCtrl {
	BHSM_M2mAuthCtrl_ePassThrough = 0x00, /* no any M2M processing, just by pass*/
	BHSM_M2mAuthCtrl_eScramble= 0x01,     /* M2M scrambling/encryption*/
	BHSM_M2mAuthCtrl_eDescramble = 0x02 	/* M2M descrambling/decryption*/
} BHSM_M2mAuthCtrl_e;

typedef struct BHSM_M2MCryptoAlg {
	BHSM_KeyAddr_e 		eKeyOffset;

	/* In: M2M algorithm selection among BCMD_M2MSecCryptoAlg_e*/
	BHSM_Algorithm_e 		m2mSecAlg; 

	/* In:  set cipher mode for non-DVB algorithms or scrambling level for DVB, see BCMD_CipherModeSelect_e */
	BHSM_AlgorithmVariant_e			cipherDVBMode; 

	/* In: used in two ways. (1) if used for AES128Counter mode, this is used as counter length selection,
	    select the value among BCMD_Aes128_CounterSize_e; (2) if used for all other algorithms, this is used
	    as termination mode selection as in BHSM_ResidueMode_e.  */
	unsigned char					ucAESCounterKeyMode;  

	/* In:  IV Mode for the selected algorithm */
	BHSM_IVMode					IVMode;

	/* In: Mode for solitary block processing */
	BHSM_SolitarySelect			solitaryMode;

	/* In: M2M processing seletion, scramble/descramble/passthrough, among BHSM_M2mAuthCtrl_e */
	BHSM_M2mAuthCtrl_e			ucAuthCtrl;  

#if 0
	/* In:  Select the MSC length from RDB registers when In AES MSC Mode (alg_sel = 0x11 or 0x14),
                "0" - the msc length read from register XPT_MMSCRAM_xxx_MSC_CTRL, [7:0]  for  MPEG2 packet
                        [15:8] for reserved1 packet.
                "1", the msc length read from register XPT_ MMSCRAM_ xxx_MSC_CTRL[23:16]  for  MPEG2 packet
                       [31:24] for reserved1 packet.
	*/
	bool							bMscCtrlSel;
#endif

	/* In: conditional. 1 to select pre-appended key inside data stream; 0 to select key in regular secure key table
	          applicable to RC4, WMDRM-PD in STB, AES-G in DVD. Ignored by CBCMAC (always to use pre-appended key).
	          Ignored by all other DVD algorithms (always use external pre-appended data key and title key in key table)
	*/
	bool							bUseExtKey;		

	/* In: conditional. 1 to select pre-appended IV inside data stream; 0 to select/load IV in regular secure key table
		   applicable to DVD algorithms only
	*/
	bool							bUseExtIV;		

	bool							bDisallow_GG;		
	bool							bDisallow_GR;		
	bool							bDisallow_RG;		
	bool							bDisallow_RR;		
	bool							bEnableTimestamp;
		
}BHSM_M2MCryptoAlg_t;


typedef union BHSM_CryptoAlg_u {
	/*In: for CA/RMX key slot related settings, in BHSM_CACryptoAlg_t*/
	BHSM_CaCpCryptoAlg_t		cacpCryptAlg;	

	/* In: for M2M key slot related settings, in BHSM_M2MCryptoAlg_t */
	BHSM_M2MCryptoAlg_t		m2mCryptAlg;   
}BHSM_CryptoAlg_u;

  
typedef enum BHSM_Muti2KeyDest {
	BHSM_Muti2KeyDest_eKey0 = 0x00,
	BHSM_Muti2KeyDest_eKey1 = 0x01,
	BHSM_Muti2KeyDest_eMaxNumber = 0x02
} BHSM_Muti2KeyDest_e;



 BERR_Code BHSM_FreeCAKeySlot(
	BHSM_Handle						in_handle,
	 unsigned int	 unKeySlot 
);

BERR_Code BHSM_LocateCAKeySlotAssigned(
	BHSM_Handle						in_handle,
	unsigned int pid_channel,
	unsigned int *unKeySlot
);

BERR_Code BHSM_AllocateCAKeySlot(	
	BHSM_Handle						in_handle,
/* 	unsigned int pid_channel, */
	unsigned int *unKeySlot
);


BERR_Code BHSM_AllocateM2MKeySlot (
		BHSM_Handle					in_handle,
		unsigned int					*outp_unKeySlotNum
);

BERR_Code BHSM_FreeM2MKeySlot (
		BHSM_Handle					in_handle,
		unsigned int					in_unKeySlotNum
);


typedef struct BHSM_ConfigMulti2IO {
    unsigned char               ucMulti2RndCnt;
    unsigned char               aucSysKey[BHSM_MULTI2_SYS_KEY_SIZE];
    BHSM_Muti2KeyDest_e    ucSysKeyDest;
}BHSM_ConfigMulti2IO_t;

BERR_Code BHSM_ConfigMulti2(	
	BHSM_Handle						in_handle,
	BHSM_ConfigMulti2IO_t		*inp_configMulti2IO
);	

typedef struct BHSM_ConfigPidKeyPointerTableIO {
	unsigned int unPidChannel;
	unsigned int unKeySlotNum;
}BHSM_ConfigPidKeyPointerTableIO_t;

BERR_Code BHSM_ConfigPidKeyPointerTable (
	BHSM_Handle							in_handle,
	BHSM_ConfigPidKeyPointerTableIO_t		*inp_configPidKeyPointerTableIO
);

typedef struct BHSM_LoadCwIvIO {
	unsigned char	aucCwIv[BHSM_CW_IV_MAX_SIZE];
	unsigned int unKeySize;
	unsigned int unKeySlot;
	BHSM_KeyAddr_e eKeyAddr;
	BHSM_KeyLadderModule_e eKeyLadderModule;
	BHSM_Operation_e eOperation;	
}BHSM_LoadCwIvIO_t;


BERR_Code BHSM_LoadCwIv(
	BHSM_Handle				in_handle,
	BHSM_LoadCwIvIO_t		*inp_loadCwIvIO
);


typedef struct BHSM_ConfigAlgorithmIO {
	BHSM_CryptoAlg_u 	cryptoAlg;
	BHSM_KeyDestBlockType_e		ekeyDestBlckType;	
	unsigned int unKeySlot;
	BHSM_KeyType_e eKeyType;
}BHSM_ConfigAlgorithmIO_t;

BERR_Code BHSM_ConfigAlgorithm(
	BHSM_Handle				in_handle,
	BHSM_ConfigAlgorithmIO_t		*inp_configAlgorithmIO
);


#ifdef __cplusplus
}
#endif


#endif	/* BHSM_KEY_LADDER_H__*/

