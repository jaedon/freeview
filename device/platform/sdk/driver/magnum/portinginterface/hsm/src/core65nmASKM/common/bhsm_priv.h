/***************************************************************************
 *     Copyright (c) 2005-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhsm_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 8/5/09 7:32a $
 *
 * Module Description: This file contains Broadcom Host Secure Module Porting 
 *                     Interface private data structures, enums, 
 *                     definitions and functions prototypes.           
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hsm/7420/b0/common/bhsm_priv.h $
 * 
 * Hydra_Software_Devel/1   8/5/09 7:32a atruong
 * PR55169: HSM PI Support for 7420B0
 * 
 * Hydra_Software_Devel/1   6/29/09 5:38p atruong
 * PR55169: HSM PI Support for 7420A1
 * 
 * Hydra_Software_Devel/16   4/11/08 6:23p atruong
 * PR40242: Adjust size for array aKeySlotAlgorithm
 * 
 * Hydra_Software_Devel/15   4/11/08 2:18p atruong
 * PR40242: aKeySlotAlgorithm array size increased
 * 
 * Hydra_Software_Devel/14   11/3/07 9:18a lyang
 * PR 35987: HSM to support BSECK, MS IPTV commands, SHA1 udpate
 * 
 * Hydra_Software_Devel/13   10/30/07 5:28p lyang
 * PR 35987: HSM to support BSECK, MS IPTV commands, SHA1 udpate
 * 
 * Hydra_Software_Devel/12   4/5/07 11:51a lyang
 * PR 28969: for HPC20 project, no bkni_delay(), Can not get XPT_PB0 TS
 * data sent to remux module on 7401 C1/C0 (B0 works fine)
 * 
 * Hydra_Software_Devel/11   3/1/07 10:46p lyang
 * PR 27378: HSM keyloading during DMA action,m2m hw patch
 * 
 * Hydra_Software_Devel/10   3/1/07 10:09p lyang
 * PR 27378: HSM keyloading during DMA action,m2m hw patch
 * 
 * Hydra_Software_Devel/9   2/28/07 7:01p lyang
 * PR 26218: HSM: Need to add support for 7400 B0
 * 
 * Hydra_Software_Devel/8   12/11/06 11:38p lyang
 * PR 26458: HSM, OTP DataSection implementation needed
 * 
 * Hydra_Software_Devel/7   10/27/06 3:22p lyang
 * PR 23412: hsm pi include files should be re-factored so they are not
 * order dependent. PR 25133:HSM header files should include dependencies
 * 
 * Hydra_Software_Devel/6   10/26/06 4:20p lyang
 * PR 23412: hsm pi include files should be re-factored so they are not
 * order dependent. PR 25133:HSM header files should include dependencies
 * 
 * Hydra_Software_Devel/5   9/26/06 3:09p btan
 * PR 23906: Converted BCHP_REV to BCHP_VER.
 * 
 * Hydra_Software_Devel/4   8/4/06 12:17a btan
 * PR22187: Sync with 7401C0 BSP header files.
 * 
 * Hydra_Software_Devel/3   5/8/06 8:56p btan
 * PR 21183: HSM needs to compile for 7401 B0
 * 
 * Hydra_Software_Devel/2   4/26/06 11:06a btan
 * PR 20446: HSM needs to compile for 7400 A0
 * 
 * Hydra_Software_Devel/2   3/27/06 7:54p btan
 * PR 20436: Sync up with Aegis 7_0 release
 * 
 * Hydra_Software_Devel/1   1/20/06 5:15p btan
 * PR 19140: Do not encrypt generic mode files.
 * 
 * Hydra_Software_Devel/4   1/17/06 9:31p btan
 * PR 19140: Do not encrypt generic mode files.
 *
 ***************************************************************************/
#ifndef BHSM_PRIV_H__
#define BHSM_PRIV_H__

#include "berr_ids.h"
/*#include "bhsm.h"   ly*/
#include "bhsm_datatypes.h"
#include "bsp_s_keycommon.h"
#include "bsp_s_commands.h"
#include "bsp_s_otp.h"
#if (BCHP_CHIP==7401) &&  (BCHP_VER == BCHP_VER_A0)
#include "bsp_s_hw_7401.h"
#else
#include "bsp_s_hw.h"
#endif
#include "bkni.h"
#include "bkni_multi.h"
#include "bint.h"			/* ly 12/8, used in BHSM_P_Handle */

#if  (BHSM_IPTV ==1)	
#include  "bmem.h"
#define  BHSM_CONTIGUOUS_MEMORY_SIZE 		(256+1024*64)    /* 64 KBytes, can be adjusted later*/
#endif


/*#include "bhsm_keyladder.h"   ly  L258 to use this, moved to bsp_s_hw.h */

/* 2/20/2007, this is only a patch to help hw, may have performance impact on key loading
	7401 B0, C0, C1 only +  M2M only  +   for all customer modes
*/
#if (BCHP_CHIP==7401) &&  (BCHP_VER >= BCHP_VER_B0)&& (BCHP_VER <= BCHP_VER_C1)
#include  "bchp_memc_0.h"				/* other STB chips, may use bchp_memc_1.h etc.  */

/* !!!!!!!!! at most, only one can be set to true. never both to true. both can be false to turn off completely !!!!!*/
#define    BHSM_PATCH_MEMC_DMA		1		/* workaround in a special submit command only */
#define    BHSM_PATCH_MEMC_DMA_PI	0		/* workaround in the individual PI only */
/* !!!!!!!!! at most, only one can be set to true. never both to true. both can be false to turn off completely !!!!!*/


#define    BHSM_PATCH_MEMC_DMA_WAITING    (500)        /* in units of 2us, please adjust this in the experiment*/
#define    BHSM_PATCH_MEMC_DMA_REG  	  (BCHP_MEMC_0_CLIENT_INFO_44)  /* if other reg, please adjust this also*/
#define    BHSM_PATCH_MEMC_DMA_BOMASK    (0x001FFF3F)      /* block out value mask, change according to RDB*/


#if (BCHP_CHIP==7401) &&  (BCHP_VER == BCHP_VER_C1)   /* for HPC20 project only, 4/5/07 */
#define BKNI_ACQUIREMUTEX_LOCK_TRUST    (1)      /*  1 to use BKNI_Sleep()/non-cpu-blocking, 0 to use BKNI_Delay()/cpu-blocking */
#endif

#else
#define     BHSM_PATCH_MEMC_DMA		0
#define     BHSM_PATCH_MEMC_DMA_PI	0
#endif


#ifdef __cplusplus
extern "C" {
#endif

/* Definitions */

/* Host Secure Module magic number used to check if opaque handle is corrupt */
#define BHSM_P_HANDLE_MAGIC_NUMBER           0xfacedead

/* Host Secure Channel magic number used to check if opaque handle is corrupt */
#define BHSM_P_CHANNEL_HANDLE_MAGIC_NUMBER   0xbacaface



/* End of Definitions */ 

/* Enum Types */

/* End of Enum Types */


/* Host Secure Private Data Structures */

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

  typedef struct BHSM_P_KeySlotAlgorithm {
 	uint32_t		ulGlobalControlBits; 	
 	uint32_t		ulCAControlBits;
 	uint32_t		ulCPDControlBits;
 	uint32_t		ulCPSControlBits;   /* Remux */
	uint32_t		ulM2MControlWordHi;
	uint32_t		ulM2MControlWordLo;

 } BHSM_P_KeySlotAlgorithm_t;

 typedef struct BHSM_P_KeySlotIDData {
 	uint32_t		ulSTBOwnerIDSelect;
	uint32_t		ulCAVendorID;
	uint32_t		ulModuleIDTestKeySelect;

 } BHSM_P_KeySlotIDData_t;

 typedef struct BHSM_P_CAKeySlotInfo {
 	BCMD_XptSecKeySlot_e		keySlotType;   /* ?? May not need this */
	bool						bIsUsed;
	/* Storage for Algorithm control bits for Odd, Even, Clear key */		
	BHSM_P_KeySlotAlgorithm_t	aKeySlotAlgorithm[BCMD_KeyDestEntryType_eMax + 1] ;	
	BHSM_P_KeySlotIDData_t		KeySlotIDData[BCMD_KeyDestEntryType_eMax + 1];
	
 } BHSM_P_CAKeySlotInfo_t;


 typedef struct BHSM_P_PidChnlToCAKeySlotNum {
 	BCMD_XptSecKeySlot_e		keySlotType;   /* ?? May not need this */
	unsigned int 				unKeySlotNum;
	
 } BHSM_P_PidChnlToCAKeySlotNum_t;


 typedef struct BHSM_P_M2MKeySlotInfo {

	bool						bIsUsed;
	/* Storage for Algorithm control bits for Odd, Even, Clear key -- consistent with CA */
	BHSM_P_KeySlotAlgorithm_t	aKeySlotAlgorithm[BCMD_KeyDestEntryType_eMax + 1] ;	
	BHSM_P_KeySlotIDData_t		KeySlotIDData[BCMD_KeyDestEntryType_eMax + 1];
	
 } BHSM_P_M2MKeySlotInfo_t;


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

#if  (BHSM_IPTV ==1)	
	BMEM_Heap_Handle	         hHeap;		/* 10/30/07 added for IPTV contiguous memeory support inside HSM*/
	unsigned char			*pContiguousMem;
#endif

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

/* End of Private Function */

#ifdef __cplusplus
}
#endif

#endif /* BHSM_PRIV_H__ */
