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
 * $brcm_Workfile: bhsm_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 2/15/12 3:33p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hsm/7425/b0/common/bhsm_priv.h $
 * 
 * Hydra_Software_Devel/2   2/15/12 3:33p atruong
 * SWSECURITY-109: HMAC-SHA command support for 40-nm platforms
 * 
 * Hydra_Software_Devel/1   10/12/11 5:31p atruong
 * SW7425-1251: HSM PI Support For BCM7425B0
 * 
 * Hydra_Software_Devel/4   6/29/11 7:08p atruong
 * SW7425-812:HSM PI API modification for new HMAC-SHA support
 * 
 * Hydra_Software_Devel/3   1/24/11 10:49a atruong
 * SW7422-26: Add HSM PI Support For 7422A0  - CC header format
 *
 ******************************************************************************/

#ifndef BHSM_PRIV_H__
#define BHSM_PRIV_H__

#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"
#include "bint.h"			/* ly 12/8, used in BHSM_P_Handle */

#include  "bmem.h"

#include "berr_ids.h"
#include "bhsm_datatypes.h"
#include "bsp_s_keycommon.h"
#include "bsp_s_commands.h"

#include "bsp_s_hw.h"


#define   BHSM_CONTIGUOUS_MEMORY_SIZE 		(256+1024*64)    /* 64 KBytes, can be adjusted later*/
#define	  BHSM_SHA_BUFFER_SIZE	(64*1024)

#ifdef __cplusplus
extern "C" {
#endif

/* external variable section (global ) */
extern bool							bHSMPIRunningFullROM;  /* defined in bhsm_keyladder.c */

/* Definitions */

/* Host Secure Module magic number used to check if opaque handle is corrupt */
#define BHSM_P_HANDLE_MAGIC_NUMBER           	0xfacedead

/* Host Secure Channel magic number used to check if opaque handle is corrupt */
#define BHSM_P_CHANNEL_HANDLE_MAGIC_NUMBER   	0xbacaface

#define BHSM_MAX_VKL							8




/* End of Definitions */ 

/* Enum Types */


typedef enum BHSM_Key2Select 
{
	BHSM_Key2Select_eRealMaskKey2  = 0x00, 
	BHSM_Key2Select_eTestMaskKey2  = 0x01,     
	BHSM_Key2Select_eFixedMaskKey2 = 0x02 	
	
} BHSM_Key2Select_e;



/* End of Enum Types */


/* Host Secure Private Data Structures */



typedef struct vklCustSubModeAssoc
{
	BCMD_VKLID_e				vkl;
	BCMD_CustomerSubMode_e		custSubMode;
	bool						bAssociated;

} vklCustSubModeAssoc_t;



/***************************************************************************
Summary:
Structure that defines Host Secure module handle.

Description:
Structure that defines Host Secure module handle.

See Also:
BHSM_Open()

****************************************************************************/  
 typedef struct BHSM_P_CommandData
{
		BCMD_cmdType_e		cmdId;  /* Command id == tag id */
		uint32_t				unContMode;
/*10/05/05,Allen.C add to support lock mutex in the command function instead of BHSM_P_CommonSubmitCommand ()*/		
		bool					bLockWaive;
		
		/* This length (in bytes) does not include header . Max is 364 bytes. Padded with zero to make it word aligned*/
		uint32_t				unInputParamLen;   
		uint32_t				unInputParamsBuf[BCMD_BUFFER_BYTE_SIZE/4];

		/* The following is for automated test */
		/* This length (in bytes) does not include header . Max is 364 bytes. Padded with zero to make it word aligned*/		
		uint32_t				unOutputParamLen;   
		uint32_t				unOutputParamsBuf[BCMD_BUFFER_BYTE_SIZE/4];

} BHSM_P_CommandData_t;


#if 0
typedef struct BHSM_P_InterruptCallback
{
	BHSM_IsrCallbackFunc	callBack;	 /*  interrupt Callback Function. */
	void 					*parm1;		/* First callback arg. */
	void					*parm2;	/* Second callback arg. */
} BHSM_P_InterruptCallback;
#endif


 typedef struct BHSM_P_CAKeySlotTypeInfo {
 	unsigned 	int		unKeySlotNum;   
	unsigned 	char		ucKeySlotStartOffset;
	
 } BHSM_P_CAKeySlotTypeInfo_t;

typedef struct BHSM_P_XPTKeySlotAlgorithm 
{
 	uint32_t		ulCAControlWordHi;
	uint32_t		ulCAControlWordLo;
 	uint32_t		ulCPDControlWordHi;
	uint32_t		ulCPDControlWordLo;
 	uint32_t		ulCPSControlWordHi;
	uint32_t		ulCPSControlWordLo;

} BHSM_P_XPTKeySlotAlgorithm_t;

 typedef struct BHSM_P_M2MKeySlotAlgorithm 
 {
	 uint32_t		 ulM2MControlWordHi;
	 uint32_t		 ulM2MControlWordLo;
 
 } BHSM_P_M2MKeySlotAlgorithm_t;


typedef struct BHSM_P_KeySlotIDData 
{
 	uint32_t		ulSTBOwnerIDSelect;
	uint32_t		ulCAVendorID;
	uint32_t		ulModuleIDKey2Select;

} BHSM_P_KeySlotIDData_t;

 typedef struct BHSM_P_CAKeySlotInfo {
 	BCMD_XptSecKeySlot_e          keySlotType;   /* ?? May not need this */
	bool                          bIsUsed;
	/* Storage for Algorithm control bits for Odd, Even, Clear key */		
	BHSM_P_XPTKeySlotAlgorithm_t  aKeySlotAlgorithm[BCMD_KeyDestEntryType_eMax];	
	BHSM_P_KeySlotIDData_t        KeySlotIDData[BCMD_KeyDestEntryType_eMax];
	
 } BHSM_P_CAKeySlotInfo_t;


 typedef struct BHSM_P_PidChnlToCAKeySlotNum {
 	BCMD_XptSecKeySlot_e		keySlotType;   /* ?? May not need this */
	unsigned int 				unKeySlotNum;
	
 } BHSM_P_PidChnlToCAKeySlotNum_t;


 typedef struct BHSM_P_M2MKeySlotInfo {

	bool						bIsUsed;
	/* Storage for Algorithm control bits for Odd, Even, Clear key -- consistent with CA */
	BHSM_P_M2MKeySlotAlgorithm_t	aKeySlotAlgorithm[BCMD_KeyDestEntryType_eMax];	
	BHSM_P_KeySlotIDData_t			KeySlotIDData[BCMD_KeyDestEntryType_eMax];
	
 } BHSM_P_M2MKeySlotInfo_t;

typedef struct BHSM_P_SHAContext_t {
	unsigned int		bufferSize;
	unsigned int		bufferSizeMax;
	unsigned char*		pBuffer;
	uint32_t            shaBufferOffset;
	bool				bMultiple;
} BHSM_P_SHAContext_t;

struct BHSM_P_Handle;             /*Leoh */

/***************************************************************************
Summary:
Structure that defines Host Secure channel handle.

Description:
Structure that defines Host Secure channel handle.

See Also:
BHSM_OpenChannel()

****************************************************************************/  
typedef struct BHSM_P_ChannelHandle
{
	uint32_t       			ulMagicNumber; /* Must be  BHSM_P_CHANNEL_HANDLE_MAGIC_NUMBER */

	struct BHSM_P_Handle *   		moduleHandle;   /* Module handle */
	
	BHSM_ChannelSettings	currentChannelSettings;   /* current channel settings */

	uint8_t        			ucChannelNumber;     /* channel number */

	bool          			bIsOpen;    /* Is channel opened */

	uint32_t	  			ulSequenceNum; /* sequence number, only use 8 bits now. Watch out for big/little endian */
	
	BKNI_MutexHandle  	mutexLock;  /* to synchronize the command */

	unsigned long			ulInCmdBufAddr;                   /* Start Address of input cmd buffer */
	unsigned long			ulOutCmdBufAddr;                   /* Start Address of output cmd buffer */

	unsigned long			ulILoadRegAddr;
	unsigned long			ulILoadVal;

	unsigned long			ulIReadyRegAddr;
	unsigned long			ulIReadyVal;	
 
	BKNI_EventHandle 		oLoadWait;
	
	uint8_t				oLoadSet;			/* for CTRL-C issue */

#if BHSM_DEBUG_POLLING  		/* a safe place, not in bhsm.c */
#define BHSM_DEBUG_POLLINGBUF_MAX (256)
	uint16_t          	pollingIntervalBuf[BHSM_DEBUG_POLLINGBUF_MAX];    /* max interval = 65535 us = 65 ms, long enough*/
	uint16_t          	pollingBufIndex;
#endif

	
} BHSM_P_ChannelHandle;

/*typedef struct BHSM_P_ChannelHandle     *BHSM_ChannelHandle;  Leoh*/


#define BHSM_HAMCSHA1_CONTEXTSWITCH_NUMBER 	3
/***************************************************************************
Summary:
Structure that defines Host Secure module handle.

Description:
Structure that defines Host Secure module handle.

See Also:
BHSM_Open()

****************************************************************************/  
 typedef struct BHSM_P_Handle
{
	uint32_t  				ulMagicNumber; /* Must be  BHSM_P_HANDLE_MAGIC_NUMBER */

	struct BHSM_P_ChannelHandle *	channelHandles[BHSM_MAX_SUPPOTED_CHANNELS];	

	BHSM_Settings 		currentSettings;   /* current settings */

	BREG_Handle			regHandle;    /* register handle */
	BCHP_Handle			chipHandle;  /* chip handle */
	BINT_Handle			interruptHandle;   /* interrupt handle */


	bool					bIsOpen;    /* Is Module opened */

	BINT_CallbackHandle	IntCallback;  /* Interrupt Callback */
	BINT_CallbackHandle	IntCallback2;  /* Interrupt Callback */	

#if 0 /* Too risky */
	BHSM_P_InterruptCallback BHSM_P_IntrCallbacks[ BHSM_IntrType_eMax ];		/* HSM interrupt handlers. */
#endif

	/* Mutex to be acquired before changing aunCAKeySlotInfo[] */
  	BKNI_MutexHandle					 caKeySlotMutexLock;

	/* If multi2 system key slot is allocated */
	bool								bMulti2SysKey;

	/* Number of Key slots per key slot type */
	BHSM_P_CAKeySlotTypeInfo_t 				aunCAKeySlotTypeInfo[BCMD_XptSecKeySlot_eTypeMax];  

	/* Total  Number of CA Key slots */
	unsigned int				unTotalCAKeySlotNum;

	/* The layout of all the key slots. Use this to find out the CA key slot type and vacant key slot. */
	BHSM_P_CAKeySlotInfo_t 				aunCAKeySlotInfo[BCMD_TOTAL_PIDCHANNELS*2];  
	
	/* 
		Given pid channel, return CA key slot number. 
		Each Pid Channel can have primary pid and secondary pid. Therefore,
		we need 2 key slot per pid channel.
		We can have multiple pid channels associated with a key slot.
		We can also have a pid channel associated with 2 key slots.
	*/
	BHSM_P_PidChnlToCAKeySlotNum_t 		aPidChannelToCAKeySlotNum[BCMD_TOTAL_PIDCHANNELS][BHSM_PidChannelType_eMax];


	/* Mutex to be acquired before changing aunM2MKeySlotInfo[] */
  	BKNI_MutexHandle					 m2mKeySlotMutexLock;

	/* Use this to find out the M2M vacant key slot. */
	BHSM_P_M2MKeySlotInfo_t 				aunM2MKeySlotInfo[BCMD_MAX_M2M_KEY_SLOT];  	
	
	bool          			bIsBusyHmacSha1[BHSM_HAMCSHA1_CONTEXTSWITCH_NUMBER];    /* Is context switch of HMAC SHA1 busy ? */
	bool					bIsBusyPKE;		 /* Is PKE engine busy ? */
	
	uint16_t				chipId;
	uint16_t				chipRev;	
	
	BMEM_Heap_Handle        hHeap;		/* 10/30/07 added for IPTV contiguous memeory support inside HSM*/
	unsigned char			*pContiguousMem;

	BHSM_P_SHAContext_t		shaContext;
} BHSM_P_Handle;


/*typedef struct BHSM_P_Handle            *BHSM_Handle;   Leoh*/



/* End of Host Secure Private Data Structures */



/* Private Function */

void BHSM_P_IntHandler_isr(
	void *inp_param1,		
	int in_param2			
);

/* Use this function to submit cmds.  This function takes care of continual modes, owner id, version number.  
ucInCmdData includes parameter length and on.  After checking output cmd, return to the caller.
Need a mutex to protect the increment seq number.  
Continual mode for diff cmds could be tricky: TBD.
Version number is from a shared aegis file ???
return error=> other than status, everything is undetermined.
 */

BERR_Code BHSM_P_SubmitCommand (
		struct BHSM_P_ChannelHandle     *	in_channelHandle	
);


BERR_Code BHSM_P_CommonSubmitCommand (
		struct BHSM_P_ChannelHandle     *	in_channelHandle,	
		BHSM_P_CommandData_t	*inoutp_commandData
);

BERR_Code BHSM_P_SubmitCommand_DMA (
		struct BHSM_P_ChannelHandle     *	in_channelHandle	
);


BERR_Code BHSM_P_CommonSubmitCommand_DMA (
		struct BHSM_P_ChannelHandle     *	in_channelHandle,	
		BHSM_P_CommandData_t	*inoutp_commandData
);


void BHSM_P_ExceptionInterruptCB_isr( 
	      void		*inp_handle,
	      void       	*inp_data 
) ;


#ifdef BHSM_AUTO_TEST
BERR_Code BHSM_P_CommonSubmitRawCommand (
		struct BHSM_P_ChannelHandle     *	in_channelHandle,	
		BHSM_P_CommandData_t	*inoutp_commandData
);
#endif

uint32_t BHSM_P_HDMIEnumToSerializerAddress (
		uint32_t		portEnum
);

/* End of Private Function */

#ifdef __cplusplus
}
#endif

#endif /* BHSM_PRIV_H__ */
