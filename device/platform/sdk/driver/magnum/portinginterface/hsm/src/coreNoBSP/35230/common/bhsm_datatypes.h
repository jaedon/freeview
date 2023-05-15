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
 * $brcm_Workfile: bhsm_datatypes.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 9/11/12 11:16p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hsm/src/coreNoBSP/35230/common/bhsm_datatypes.h $
 * 
 * Hydra_Software_Devel/1   9/11/12 11:16p atruong
 * SWSECURITY-175: Remove symbolic links in HSM PI
 * 
 * Hydra_Software_Devel/2   2/23/11 2:48p haoboy
 * SW7550-692: new mode support
 * 
 * Hydra_Software_Devel/1   1/21/11 9:51a haoboy
 * SW7550-666:initial version
 * 
 * Hydra_Software_Devel/7   5/25/10 6:26p qcheng
 * SW7408-82 : Add security support for 7408 A0
 * 
 * Hydra_Software_Devel/6   5/11/10 7:19p qcheng
 * SW7550-419 : 7550's BHSM_FreeCAKeySlot appears to have a wrong API
 * 
 * Hydra_Software_Devel/5   5/4/10 4:22p qcheng
 * SW7550-102 : Add Security support for 7550
 * 
 * Hydra_Software_Devel/4   4/2/10 7:27p qcheng
 * SW7550-353: PVR encryption feature is required for 7540 production with
 * PVR function
 * 
 * Hydra_Software_Devel/3   3/4/10 11:27a qcheng
 * SW7408-82 : Add security support for 7408 A0
 * 
 * Hydra_Software_Devel/2   12/30/09 1:49a rnayak
 * SW7550-102: 7550 HSM files were not properly added to source control.
 * Adding them again
 * 
 * Hydra_Software_Devel/1   12/4/09 5:25p qcheng
 * SW7550-102 : Add Security support for 7550
 *
 **************************************************************************/
#ifndef BHSM_DATATYPES_H__
#define BHSM_DATATYPES_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Definitions */
#define BHSM_P_CHECK_ERR_CODE_FUNC( errCode, function )		\
	if( (errCode = (function)) != BERR_SUCCESS )	\
	{							\
		errCode = BERR_TRACE(errCode);	\
		goto BHSM_P_DONE_LABEL;	\
	}									

#define BHSM_P_CHECK_ERR_CODE_FUNC2( errCode, errCodeValue, function )			\
	if ( ( errCode = (function)) != BERR_SUCCESS ) \
	{										\
		errCode = BERR_TRACE(errCodeValue);	\
		goto BHSM_P_DONE_LABEL;							\
	}

	
#define BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, errCodeValue, condition )			\
	if( (condition) ) \
	{ \
		errCode = BERR_TRACE((errCodeValue));		\
		goto BHSM_P_DONE_LABEL;							\
	}	

#define check(x)	\
{	if ( x != 0 ) {	\
		BDBG_MSG(("keyladder failed at %s:%d\n", __FILE__, __LINE__));	\
		return BHSM_STATUS_FAILED ;	\
	} \
} 


/* Host Secure module status codes */
#define BHSM_STATUS_FAILED				BERR_MAKE_CODE(BERR_ICM_ID, 1)  /* Return code for general failure. */ 
#define BHSM_STATUS_TIME_OUT			BERR_MAKE_CODE(BERR_ICM_ID, 2)  
#define BHSM_STATUS_PARM_LEN_ERR		BERR_MAKE_CODE(BERR_ICM_ID, 3) 
#define BHSM_STATUS_OWNER_ID_ERR		BERR_MAKE_CODE(BERR_ICM_ID, 7) 
#define BHSM_STATUS_INPUT_PARM_ERR	BERR_MAKE_CODE(BERR_ICM_ID, 4) 
#define BHSM_STATUS_HW_BUSY_ERR			BERR_MAKE_CODE(BERR_ICM_ID, 5) 
#define BHSM_STATUS_VERSION_ERR			BERR_MAKE_CODE(BERR_ICM_ID, 6) 
#define BHSM_STATUS_NO_HW_SUPPORT		BERR_MAKE_CODE(BERR_ICM_ID, 7) 

#define BHSM_MaxCaCpKeySlot 			(24)
#define BHSM_MaxM2MKeySlot	 	(8)
#define BHSM_MaxPidChannelNum 		(128)
#define BHSM_MAX_CUST_KEY			(64)
#define BHSM_CA_KEYSLOT_STEPSIZE		(0x10)
#define BHSM_M2M_KEYSLOT_STEPSIZE		(0x8)

#define BHSM_SLOT_NUM_INIT_VAL		(-1)

#define BHSM_KEY_POINTOR_INIT_VAL	(0)


/* End of Definitions */


/* Enum Types */
/*
destination of key ladder output for current operation.
proc_dest == 00000 -> generate key2l from otp
proc_dest == 00001 -> generate key2h from otp
proc_dest == 00010 -> generate key2l from cus_key
proc_dest == 00011 -> generate key2h from cus_key
proc_dest == 00100 -> generate key3l from proc_in
proc_dest == 00101 -> generate key3h from proc_in
proc_dest == 00110 -> generate key4l from proc_in
proc_dest == 00111 -> generate key4h from proc_in
proc_dest == 01000 -> CW (to key_table) from key3
proc_dest == 01001 -> CW (to key_table) from key4
proc_dest == 01010 -> CW (to key_table) direct SW
proc_dest == 01011 -> generate keyal from key3l (Nonce)
proc_dest == 01100 -> generate keyah from key3h (Nonce)
proc_dest == 01101 -> cus_mode
proc_dest == 01110 -> copy otp_flash_key_aes to key_table
proc_dest == 01111 -> copy otp_bulk_key_aes to key_table
proc_dest == 10000 -> reset pksm
proc_dest == 10001 -> key3 output to pksm
proc_dest == 10010 -> pksm output to CW
proc_dest == 10011 -> pksm step
proc_dest == 10100 -> hdmi key from key3
proc_dest == 10101 -> challenge-response
proc_dest == 10110 -> copy OTP to key2
proc_dest == 10111 -> decrypt nonce to proc_out

*/

typedef enum BHSM_KeyGenProcess	 
{  
    BHSM_KeyGenProcess_eKey2lFromOTP = 0x00,  
    BHSM_KeyGenProcess_eKey2hFromOTP = 0x01, 
    BHSM_KeyGenProcess_eKey2lFromCusKey = 0x02,
    BHSM_KeyGenProcess_eKey2hFromCusKey = 0x03,
    BHSM_KeyGenProcess_eKey3lFromProcIn = 0x04,
    BHSM_KeyGenProcess_eKey3hFromProcIn = 0x05,
    BHSM_KeyGenProcess_eKey4lFromProcIn = 0x06,   
    BHSM_KeyGenProcess_eKey4hFromProcIn = 0x07,
    BHSM_KeyGenProcess_eCwUsingKey3 = 0x08,
    BHSM_KeyGenProcess_eCwUsingKey4  = 0x09,    
    BHSM_KeyGenProcess_eCwDirectSW  = 0x0a,
    BHSM_KeyGenProcess_eCwFromKey3l  = 0x0b,
    BHSM_KeyGenProcess_eCwFromKey3h  = 0x0c,
    BHSM_KeyGenProcess_eCusMode = 0x0d,
    BHSM_KeyGenProcess_eOtpFlashKeyAesToKeyTable = 0x0e,
    BHSM_KeyGenProcess_eOtpBulkKeyAesToKeyTable = 0x0f,    
    BHSM_KeyGenProcess_eResetPksm  = 0x10,
    BHSM_KeyGenProcess_eKey3ToPksm  = 0x11,
    BHSM_KeyGenProcess_ePksmToCw  = 0x12,
    BHSM_KeyGenProcess_ePksmStep  = 0x13,
    BHSM_KeyGenProcess_eHdmiKeyFromKey3  = 0x14,
    BHSM_KeyGenProcess_eChallengeResponse  = 0x15,
    BHSM_KeyGenProcess_eOtpToKey2  = 0x16,
    BHSM_KeyGenProcess_eDeNonceToProcOut  = 0x17,   
    BHSM_KeyGenProcess_eMaxNumber = 0x18
} BHSM_KeyGenProcess_e;


typedef enum BHSM_Operation {
	BHSM_Operation_eCP = 0x00,
	BHSM_Operation_eCA = 0x01,
	BHSM_Operation_eMaxNumber = 0x02
} BHSM_Operation_e;


typedef enum BHSM_CustMode {
/*
forA0
	BHSM_CustMode_eDVS042 = 0x01,
	BHSM_CustMode_eDESCTS = 0x02,
	BHSM_CustMode_eDVBCSA = 0x04,
	BHSM_CustMode_eGeneric = 0x08
*/
/*for B0*/
	BHSM_CustMode_eDVS042 = 0x10,
	BHSM_CustMode_eDESCTS = 0x20,
	BHSM_CustMode_eDVBCSA = 0x40,
	BHSM_CustMode_eGeneric= 0x80,
        BHSM_CustMode_eGeneric_AESKL=0x08,
        BHSM_CustMode_eGeneric_AESECB=0x04
} BHSM_CustMode_e;

typedef enum BHSM_KeyType {
	BHSM_KeyType_eOddKey = 0x00,
	BHSM_KeyType_eEvenKey = 0x01,
	BHSM_KeyType_eClearKey = 0x02,
	BHSM_KeyType_eMaxNumber = 0x03
} BHSM_KeyType_e;


/* End of Enum Types */




/***************************************************************************
Summary:
Required default settings structure for Host Secure module.

Description:
The default setting structure defines the default configure of
HSM.  Since BHSM has multiple channels, it also has 
default settings for a  channel.

See Also:
BHSM_ChannelSettings, BHSM_Open()

****************************************************************************/   
typedef struct BHSM_Setting 
{	
/* ToDo: Added all settiing here */
	BHSM_CustMode_e		eCustMode;
}BHSM_Settings;


#ifdef __cplusplus
}
#endif


#endif  /* BHSM_DATATYPES_H__ */


