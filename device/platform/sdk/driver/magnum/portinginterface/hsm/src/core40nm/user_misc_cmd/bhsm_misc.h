/***************************************************************************
 *     (c)2007-2011 Broadcom Corporation
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
 * $brcm_Workfile: bhsm_misc.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 2/15/12 1:03p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hsm/7425/b0/user_misc_cmd/bhsm_misc.h $
 * 
 * Hydra_Software_Devel/2   2/15/12 1:03p atruong
 * SWSECURITY-109: HMAC-SHA command support for 40-nm platforms
 * 
 * Hydra_Software_Devel/1   10/12/11 8:36p atruong
 * SW7425-1251: HSM PI Support For BCM7425B0
 * 
 * Hydra_Software_Devel/1   1/25/11 6:42p atruong
 * SW7422-26: Add HSM PI Support For 7422A0  - CC header format
 *
 ******************************************************************************/


#ifndef BHSM_MISC_H__
#define BHSM_MISC_H__



/* added to remove header file dependency chain*/
#include "bsp_s_misc.h"

#include "bhsm.h"


#ifdef __cplusplus
extern "C" {
#endif


/* Module Specific Functions */



/**************************************************************************************************
Summary:

Description:
Structure that defines BHSM_SetMiscBitsIO members

See Also:
BHSM_SetMiscBits
**************************************************************************************************/
typedef struct BHSM_SetMiscBitsIO {

	/* In: SubCommand ID for BCMD_SetMiscBits command. */
	BCMD_SetMiscBitsSubCmd_e	setMiscBitsSubCmd;
	
	/****  Rave Ctrl SubCmd ****/
	/* In: value must be '1'.  Enable RAVE0 IMEM to be written by host b(0) */
	bool 						bEnableWriteIMem;
	
	/* In: value must be '1'. Enable RAVE0 IMEM to be read by host      b(1)*/
	bool 						bEnableReadIMem;
	
	/* In: value must be '1'. Enable RAVE0 DMEM to be read by host     b(2)*/
	bool 						bEnableReadDMem;

	/* In: Bypass encryption engine for RAVE                                    b(3)*/
	bool						bRAVEEncryptionBypass;
	
	/* In: When set, this will remove the RAVE from reset.  RAVE is placed in reset
	whenever code is written to RAVE IMEM.                                     b(4) */
    bool                   		bDisableClear;

	/**** SubCommand 0x01   ****/
	/* In:   b(0)*/
    bool                 		bDMAXPTPacketBypassEnable;

	/* In:   b(1)*/
	bool               			bNMAPacketBypassEnable;

	/* In:   b(2) */
	bool               			bDMAM2MPacketBypassEnable;
	
	/**** M2M Endian Swap SubCommand ****/
	/* In:   b(0) Swap each 32-bit word byte order of the input data */	
	bool               			bCBCMACDataInSwapEnable;
	
	/* In:   b(1) Swap each 32-bit word byte order of the output data */
	bool               			bCBCMACDataOutSwapEnable;
	
	/* In:   b(2) Swap each 32-bit word byte order of the key */
	bool               			bCBCMACKeySwapEnable;
	
	/**** ForceSC SubCommand ****/
	/* In:   b(0..7)   Band Select - 0 : IB  1 : PB */	
	bool               			bBandSelect_PB;

	/* In:   b(8..15) Band number ( IB[0...15] or PB[0...31] */
	unsigned char				bandNumber;

	/* In:   b(0)      Control bits SC_FORCE_EN_ALL */
	bool						bSCForceEnAll;

	/* In:   b(1)      Control Bits SC_FORCE_EN_NZ */
	bool						bSCForceEnNZ;

	/* In:   b(2, 3)  Control Bits SC_FORCE_EN_VALUE */
	unsigned char 				SCForceEnValue;

	/* Out: 0 for success, otherwise failed. */
	uint32_t					unStatus;

	/* Out: For Dump XPT/M2M Status SubCmd. */	
	uint32_t 					unM2MSecurityErrStatus;

	/* Out: For Dump XPT/M2M Status SubCmd. */	
	uint32_t					unXPTSecurityErrStatus;

} BHSM_SetMiscBitsIO_t;


/**************************************************************************************************
Summary:
This function is used to remove RAVE from reset.

Description:
This function is used to remove the RAVE from reset.  RAVE is placed in reset whenever the Host
CPU writes to RAVE IMEM.  When RAVE is removed from reset, it will begin executing code from
IMEM.  Video/Audio will not be descrambled until the RAVE is out of reset.

Calling Context:
This function is called after RAVE firmware has been loaded into IMEM and descrambling of 
video/audio is ready to begin.

Performance and Timing:
This is a synchronous/blocking function that will not return until it completes.

Input:
in_handle  - BHSM_Handle, Host Secure module handle.
inoutp_setMiscBitsIO  - BHSM_SetMiscBitsIO_t.
			
Output:
inoutp_setMiscBitsIO  - BHSM_SetMiscBitsIO_t.

Returns:
BERR_SUCCESS - success
BHSM_STATUS_FAILED - failure

See Also:
N/A
**************************************************************************************************/
BERR_Code   BHSM_SetMiscBits (
		BHSM_Handle					in_handle,
		BHSM_SetMiscBitsIO_t		*inoutp_setMiscBitsIO
);


typedef enum BCMD_PCIArchType_e
{
	BCMD_PCIArchType_eNonPCIWin,
	BCMD_PCIArchType_ePCIWin,
	BCMD_PCIArchType_ePCIeWin,

	BCMD_PCIArchType_eMax
}	BCMD_PCIArchType_e;

typedef enum BCMD_ArchSelect_e
{
	BCMD_ArchSelect_eSel00,
	BCMD_ArchSelect_eSel01,
	BCMD_ArchSelect_eSel02,
	BCMD_ArchSelect_eSel03,

	BCMD_ArchSelect_eMax

}	BCMD_ArchSelect_e;

typedef enum BCMD_DRAMSelect_e
{
	BCMD_DRAMSelect_eSel00,
	BCMD_DRAMSelect_eSel01,

	BCMD_DRAMSelect_eMax

}	BCMD_DRAMSelect_e;



/**************************************************************************************************
Summary:

Description:
Structure that defines BHSM_SetArchIO members

See Also:
BHSM_SetArch
**************************************************************************************************/
typedef struct BHSM_SetArchIO {

	/* In: defines the start address for the secure region. */
	uint32_t			unLowerRangeAddress;
	
	/* In: defines the end address (inclusive) for the secure region. */	
	uint32_t			unUpperRangeAddress;

	/* In: defines the arch Select */	
	BCMD_ArchSelect_e	ArchSel;

	/*In: defines the PCI Arch type */
	BCMD_PCIArchType_e	PCIArch;

	/*In: defines DRAM Select */
	BCMD_DRAMSelect_e	DRAMSel;	

	/* Out: 0 for success, otherwise failed. */
	uint32_t			unStatus;
} BHSM_SetArchIO_t;

/**************************************************************************************************
Summary:
This function configures the address range of secure region. It is currently designed for future 
enhanced features.

Description:
The host DRAM is partitioned into Secure Region and Global Region. This function specifies 
the address range of the secure region.  Everything outside the specified region would be  
the global region.   

Calling Context:
It shall be called after the heap is allocated for secure region and specific buffer pointers are
initialized.

Performance and Timing:
This is a synchronous/blocking function that will not return until it completes.

Input:
in_handle  - BHSM_Handle, Host Secure module handle.
inoutp_setArchIO  -  BHSM_SetArchIO_t.
			
Output:
inoutp_setArchIO  - BHSM_SetArchIO_t.

Returns:
BERR_SUCCESS - success
BHSM_STATUS_FAILED - failure

See Also:
N/A
**************************************************************************************************/
BERR_Code   BHSM_SetArch (
		BHSM_Handle			in_handle,
		BHSM_SetArchIO_t	*inoutp_setArchIO
);




#define	BHSM_OTP_ID_LEN			8		/* OTP ID is 8 bytes long */

typedef enum BHSM_OtpIdRead_e
{
	BHSM_OtpIdRead_eOTPIdA = 0,
	BHSM_OtpIdRead_eOTPIdB = 1,
	BHSM_OtpIdRead_eOTPIdC = 2,
	BHSM_OtpIdRead_eOTPIdD = 3,
	BHSM_OtpIdRead_eOTPIdE = 4,
	BHSM_OtpIdRead_eOTPIdF = 5,
	BHSM_OtpIdRead_eMax

}	BHSM_OtpIdRead_e;

/**************************************************************************************************
Summary:

Description:
Structure that defines BHSM_ReadOTPIdIO members

See Also:
BHSM_ReadOTPId()
**************************************************************************************************/

typedef struct BHSM_ReadOTPIdIO {

	/* In:   Which OTP ID to read */
	BHSM_OtpIdRead_e		OtpId;
	/* Out: 0 for success, otherwise failed. */
	uint32_t				unStatus;
	/* Out: if usStatus is successful, this is the OTP ID, as requested */
	unsigned char			aucOTPId[BHSM_OTP_ID_LEN];
	/* Out: actual number of bytes returned for OTP ID */
	uint32_t				unIdSize;

}	BHSM_ReadOTPIdIO_t;


BERR_Code	BHSM_ReadOTPId(
		BHSM_Handle			in_handle,
		BHSM_ReadOTPIdIO_t	*inoutp_readOTPIdIO
);


#define MAX_AVD_SVD_RANGE				8
#define BHSM_HMACSHA256_SIGNATURE_SIZE	32

typedef enum BCMD_AVDType_e
{
    BCMD_AVDType_eSVD  = 0,
    BCMD_AVDType_eAVD  = 1,
	BCMD_AVDType_eVICE = 2,

	BCMD_AVDType_eMax
} 	BCMD_AVDType_e;


/**************************************************************************************************
Summary:

Description:
Structure that defines BHSM_SetVichRegParIO members

See Also:
BHSM_SetVichRegPar
**************************************************************************************************/
typedef struct BHSM_SetVichRegParIO {

	/* In: key from which virtual key ladder to invalidate */
	BCMD_VKLID_e		virtualKeyLadderID;

	/* In: key from which key layer of the above virtual key ladder to invalidate */
	BCMD_KeyRamBuf_e	keyLayer;

	/* In: VDEC ID -  0 : SVD   1 : AVD   2 : VICE */
	BCMD_AVDType_e		VDECId;

	/* In: Number of ranges  - max 8 for AVD/SVD  5 for VICE */
	unsigned char		nRanges;
	
	/* In: start address (Low, Inclusive) */
	uint32_t			unRangeLo[MAX_AVD_SVD_RANGE];
	
	/* In: end address (High, Inclusive)  */
	uint32_t			unRangeHi[MAX_AVD_SVD_RANGE];
	
	/* In: Signature for the entire command */
	unsigned char		aucSignature[BHSM_HMACSHA256_SIGNATURE_SIZE];

	/* Out: 0 for success, otherwise failed */
	uint32_t			unStatus;

} BHSM_SetVichRegParIO_t;


/**************************************************************************************************
Summary:
This function is used to protect certain AVD registers.

Description:
This function is used to protect AVD registers.  Once the AVD registers are protected, the
Host can no longer write to those registers.  It is assumed that the Host initially configures
these AVD registers and calls this command with authenticated code.

Calling Context:
This function is called after the Host CPU configures the AVD registers to the desired values.

Performance and Timing:
This is a synchronous/blocking function that will not return until it completes.

Input:
in_handle  - BHSM_Handle, Host Secure module handle.
inoutp_setVichIO  - BHSM_SetVichRegParIO_t.
							
Output:
inoutp_setVichIO  - BHSM_SetVichRegParIO_t.				

Returns:
BERR_SUCCESS - success
BHSM_STATUS_FAILED - failure

See Also:
N/A
**************************************************************************************************/
BERR_Code  BHSM_SetVichRegPar (
		BHSM_Handle					in_handle,
		BHSM_SetVichRegParIO_t		*inoutp_setVichRegParIO
);





/**************************************************************************************************
Summary:

Description:
Structure that defines BHSM_StartAVDIO members

See Also:
BHSM_StartAVD
**************************************************************************************************/
typedef struct BHSM_StartAVDIO {

	/* In: key from which virtual key ladder to use for key generation */
	BCMD_VKLID_e		virtualKeyLadderID;

	/* In: key from which key layer of the above virtual key ladder to use for key generation */
	BCMD_KeyRamBuf_e	keyLayer;

	/*In: Which AVD to restart */
	BCMD_AVDType_e		avdID;

	/*In: Number of AVD Register/Address pairs to use */
	uint32_t			numAVDReg;

	/* In: address to program */
	uint32_t			avdRegAddr[MAX_AVD_SVD_RANGE];
	
	/* In: value to program  */
	uint32_t			avdRegVal[MAX_AVD_SVD_RANGE];

	/* In: Signature for the entire command */
	unsigned char		aucSignature[BHSM_HMACSHA256_SIGNATURE_SIZE];

	/* Out: 0 for success, otherwise failed */
	uint32_t			unStatus;

} BHSM_StartAVDIO_t;


/**************************************************************************************************
Summary:

Description:

Calling Context:

Performance and Timing:
This is a synchronous/blocking function that will not return until it completes.

Input:
in_handle  - BHSM_Handle, Host Secure module handle.
inoutp_startAVDIO  - BHSM_StartAVDIO_t.
							
Output:
inoutp_startAVDIO  - BHSM_StartAVDIO_t.

Returns:
BERR_SUCCESS - success
BHSM_STATUS_FAILED - failure

See Also:
N/A
**************************************************************************************************/
BERR_Code  BHSM_StartAVD (
		BHSM_Handle				in_handle,
		BHSM_StartAVDIO_t		*inoutp_startAVDIO
);





/* End of Module Specific Functions */
#if BHSM_SECURE_PI_SUPPORT_MISC
#include   "bhsm_misc_enc.h"
#endif


#ifdef __cplusplus
}
#endif

#endif /* BHSM_MISC_H__ */







