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
 * $brcm_Workfile: bhsm_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 2/23/11 3:03p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hsm/7550/b0/common/bhsm_priv.h $
 * 
 * Hydra_Software_Devel/2   2/23/11 3:03p haoboy
 * SW7550-692: added AES support for key ladder operation
 * 
 * Hydra_Software_Devel/1   1/21/11 9:50a haoboy
 * SW7550-666:initial version
 * 
 * Hydra_Software_Devel/6   5/25/10 6:26p qcheng
 * SW7408-82 : Add security support for 7408 A0
 * 
 * Hydra_Software_Devel/5   4/2/10 7:30p qcheng
 * SW7550-353: PVR encryption feature is required for 7540 production with
 * PVR function
 * 
 * Hydra_Software_Devel/4   3/4/10 11:36a qcheng
 * SW7408-82 : Add security support for 7408 A0
 * 
 * Hydra_Software_Devel/3   1/15/10 6:27p qcheng
 * SW7550-188: Support HDMI/HDCP for 7550
 * 
 * Hydra_Software_Devel/2   12/30/09 1:49a rnayak
 * SW7550-102: 7550 HSM files were not properly added to source control.
 * Adding them again
 * 
 * Hydra_Software_Devel/1   12/4/09 5:26p qcheng
 * SW7550-102 : Add Security support for 7550
 *
 **************************************************************************/
#ifndef BHSM_PRIV_H__
#define BHSM_PRIV_H__

#include "berr_ids.h"
#include "bhsm_datatypes.h"
#include "bhsm.h"
#include "bkni_multi.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Definitions */

/* Host Secure Module magic number used to check if opaque handle is corrupt */
#define BHSM_P_HANDLE_MAGIC_NUMBER           0xfacedead


/* End of Definitions */ 

/* Enum Types */

/* End of Enum Types */



/***************************************************************************
Summary:
Structure that defines Host Secure module handle.

Description:
Structure that defines Host Secure module handle.

See Also:
BHSM_Open()

****************************************************************************/  
  typedef struct BHSM_P_CaCpKeySlotAlgorithm {
 	uint32_t		ulGlobalControlBits; 	
 	uint32_t		ulCAControlBits;
 	uint32_t		ulCPDControlBits;
 	uint32_t		ulCPSControlBits;  
 } BHSM_P_CaCpKeySlotAlgorithm;

 typedef struct BHSM_P_CaCpKeySlotInfo {
	bool						bIsUsed;
	/* Storage for Algorithm control bits for Odd, Even, Clear key */		
	BHSM_P_CaCpKeySlotAlgorithm	aKeySlotAlgorithm[BHSM_KeyType_eMaxNumber] ;	
 } BHSM_P_CaCpKeySlotInfo;

   typedef struct BHSM_P_M2MKeySlotAlgorithm {
 	uint32_t		ulGlobalControlBits; 	
 	uint32_t		ulM2MControlBits;
 } BHSM_P_M2MKeySlotAlgorithm;

 typedef struct BHSM_P_M2MKeySlotInfo {
	bool						bIsUsed;
	/* Storage for Algorithm control bits */		
	BHSM_P_M2MKeySlotAlgorithm	aKeySlotAlgorithm ;		
 } BHSM_P_M2MKeySlotInfo;


 typedef struct BHSM_P_Handle
{
	uint32_t  		ulMagicNumber; /* Must be  BHSM_P_HANDLE_MAGIC_NUMBER */
	BHSM_Settings 		currentSettings;   /* current settings */
	BHSM_CustMode_e		eCustMode;
	BREG_Handle		regHandle;    /* register handle */
	BCHP_Handle		chipHandle;  /* chip handle */
	bool			bIsOpen;    /* Is Module opened */	
	uint16_t		chipId;
	uint16_t		chipRev;	
  	BKNI_MutexHandle	caCpKeySlotMutexLock; /* Mutex to be acquired before changing aCaCpKeySlotInfo[] */
	BHSM_P_CaCpKeySlotInfo	aCaCpKeySlotInfo[BHSM_MaxCaCpKeySlot] ;	
	unsigned int 		aunPidChannelToCaCpKeySlotNum[BHSM_MaxPidChannelNum];
	BKNI_MutexHandle	m2mKeySlotMutexLock; /* Mutex to be acquired before changing aM2MKeySlotInfo[] */
	BHSM_P_M2MKeySlotInfo	aM2MKeySlotInfo[BHSM_MaxM2MKeySlot];
} BHSM_P_Handle;

typedef enum BHSM_KeyDestBlockType {
	BHSM_KeyDestBlockType_eCA = 0x00,
	BHSM_KeyDestBlockType_eCPS = 0x01,
	BHSM_KeyDestBlockType_eCPD = 0x02,
	BHSM_KeyDestBlockType_eM2M = 0x03,
	BHSM_KeyDestBlockType_eMaxNumber = 0x04
} BHSM_KeyDestBlockType_e;

typedef enum BHSM_KeyLadderModule {
	BHSM_KeyLadderModule_eSecurity = 0x00,
	BHSM_KeyLadderModule_eMmscram = 0x01,
	BHSM_KeyLadderModule_eMaxNumber = 0x02
} BHSM_KeyLadderModule_e;

typedef struct BHSM_P_ProcessKeyIO {
	BHSM_KeyGenProcess_e eKeyProcess;
	unsigned int	unCustKey;
	unsigned int	unKeyVar;
	uint32_t 	procInLow;
	uint32_t 	procInHigh;
	BHSM_Operation_e eOperation;		
	unsigned int 	unKeyDest;
	uint32_t 	procOutLow;
	uint32_t 	procOutHigh;
	BHSM_KeyLadderModule_e eKeyLadderModule;
	uint32_t 	procInAesLow;
	uint32_t 	procInAesHigh;
}BHSM_P_ProcessKeyIO_t;

BERR_Code BHSM_P_ProcessKey(
	struct BHSM_P_Handle		*in_handle,
	BHSM_P_ProcessKeyIO_t		*inp_processKeyIO
);

BERR_Code BHSM_P_ProcessKeyAes(
	struct BHSM_P_Handle		*in_handle,
	BHSM_P_ProcessKeyIO_t		*inp_processKeyIO
);


BERR_Code BHSM_P_ReqCustMode(
	struct BHSM_P_Handle     *in_handle,
	BHSM_CustMode_e	mode
);

void BHSM_P_EnableSecurity(
	struct BHSM_P_Handle		*in_handle
	);

void BHSM_P_DisableSecurity(
	struct BHSM_P_Handle		*in_handle
	);



#ifdef __cplusplus
}
#endif

#endif /* BHSM_PRIV_H__ */

