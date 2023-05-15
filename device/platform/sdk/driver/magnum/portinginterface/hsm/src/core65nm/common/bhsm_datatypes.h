/***************************************************************************
 *     Copyright (c) 2005-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhsm_datatypes.h $
 * $brcm_Revision: Hydra_Software_Devel/38 $
 * $brcm_Date: 7/27/09 11:06a $ 
 *
 * Module Description: This file contains common Broadcom host secure module data 
 *                     structures, enums and definitions that are
 *                     recommended to be used in different layers. 
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hsm/7400/a0/common/bhsm_datatypes.h $
 * 
 * Hydra_Software_Devel/38   7/27/09 11:06a atruong
 * PR56432: Bug fix - Merging code for 3556C0
 *
 * Hydra_Software_Devel/37   7/25/09 4:12p atruong
 * PR56432: HSM PI Support for Secure RSA/Access cmds and FastCycling
 *
 * Hydra_Software_Devel/36   7/14/09 4:26p atruong
 * PR56536: HSM PI Support for 3556C0
 * 
 * Hydra_Software_Devel/35   4/14/09 7:39a atruong
 * PR54104: HSM PI Support for 35130A0
 * 
 * Hydra_Software_Devel/34   1/30/09 4:58p atruong
 * PR51632: 7336A0 Support
 * 
 * Hydra_Software_Devel/33   1/30/09 4:50p atruong
 * PR51632: Support for 7336A0
 * 
 * Hydra_Software_Devel/32   1/26/09 12:36p atruong
 * PR51446: HSM PI Support for BCM3548B2
 * 
 * Hydra_Software_Devel/31   1/23/09 5:29p atruong
 * PR51446: HSM PI Support for BCM3548B2
 * 
 * Hydra_Software_Devel/30   1/22/09 1:44p atruong
 * PR51441: HSM PI Support for 7420A1
 * 
 * Hydra_Software_Devel/29   12/19/08 4:46p atruong
 * PR50482: HSM PI Support for BCM97413A0 (7405C0)
 * 
 * Hydra_Software_Devel/28   12/5/08 8:33p atruong
 * PR50045: HSM PI Support for BCM7420 A0
 * 
 * Hydra_Software_Devel/27   12/4/08 9:21a atruong
 * PR49638: HSM PI Support for 3548B1 and 3558B1
 * 
 * Hydra_Software_Devel/26   10/3/08 1:14a atruong
 * PR46656: HSM PI Support for Rev B0
 * 
 * Hydra_Software_Devel/25   7/17/08 4:22p atruong
 * PR44849: BCM97335B0 Bring Up
 * 
 * Hydra_Software_Devel/24   5/20/08 11:34a atruong
 * PR42828: HSM PI Support for 7325B0
 * 
 * Hydra_Software_Devel/23   4/9/08 10:48a atruong
 * PR38948: HSM PI support for BCM3548
 * PR39464: HSM PI support for BCM3556
 * 
 * Hydra_Software_Devel/22   3/21/08 11:52a farshidf
 * PR40819: Add A1 support
 * 
 * Hydra_Software_Devel/21   2/28/08 10:46a qxie
 * PR39971: add support for 7400 E0 to portinginterface/hsm
 * 
 * Hydra_Software_Devel/20   2/13/08 2:40p qxie
 * PR38615: Add HSM support for 7118 C0
 * 
 * Hydra_Software_Devel/19   1/22/08 6:20p qxie
 * PR38887: HSM: add support to 7405B038887: HSM: add support to 7405B0
 * 
 * Hydra_Software_Devel/18   12/17/07 6:26p qxie
 * PR37432:add support to 7400 d1
 * 
 * Hydra_Software_Devel/17   11/3/07 9:18a lyang
 * PR 35987: HSM to support BSECK, MS IPTV commands, SHA1 udpate
 * 
 * Hydra_Software_Devel/16   10/30/07 5:22p lyang
 * PR 35987: HSM to support BSECK, MS IPTV commands, SHA1 udpate
 * 
 * Hydra_Software_Devel/15   10/15/07 3:40p lyang
 * PR 35021: HSM bringup on 7325, part of 7335 added also
 * 
 * Hydra_Software_Devel/14   9/13/07 10:23a lyang
 * PR 34837: Add HSM support for 7405 A1
 * 
 * Hydra_Software_Devel/13   8/2/07 11:08a lyang
 * PR 33325: add 7118B0 support and link
 * 
 * Hydra_Software_Devel/12   6/26/07 11:56a lyang
 * PR 32424: add 7400C0 support and link
 * 
 * Hydra_Software_Devel/11   6/7/07 12:07p lyang
 * PR 27711: Add 7401C3 7400B0, 7405A0 support to HSM
 * 
 * Hydra_Software_Devel/10   4/19/07 3:37p lyang
 * PR 29703: Add HSM support for 7403 A1, 7118A1, Program secret update
 * 
 * Hydra_Software_Devel/9   4/10/07 10:07a lyang
 * PR 28969: for HPC20 project, no bkni_delay(), Can not get XPT_PB0 TS
 * data sent to remux module on 7401 C1/C0 (B0 works fine)
 * 
 * Hydra_Software_Devel/8   2/22/07 7:00p lyang
 * PR 26218: HSM: Need to add support for 7400 B0
 * 
 * Hydra_Software_Devel/7   2/9/07 3:58p lyang
 * PR 27777: HSM add IRDY waiting loop, add polling support 27393
 * 
 * Hydra_Software_Devel/6   12/13/06 11:06a lyang
 * PR 24864: HSM: Add HSM support for 7403, PR 25689: add 7403 support to
 * HSM/SCD
 * 
 * Hydra_Software_Devel/5   9/26/06 3:07p btan
 * PR 23906: Converted BCHP_REV to BCHP_VER.
 * 
 * Hydra_Software_Devel/4   9/8/06 4:57p dbayani
 * PR 24170: Updated HSM PI to support 7401 C0.
 * 
 * Hydra_Software_Devel/3   5/8/06 8:56p btan
 * PR 21183: HSM needs to compile for 7401 B0
 * 
 * Hydra_Software_Devel/2   4/26/06 11:06a btan
 * PR 20446: HSM needs to compile for 7400 A0
 * 
 * Hydra_Software_Devel/1   2/28/06 8:21p btan
 * PR 19931: Added HSM support for 7400A0
 * 
 * Hydra_Software_Devel/1   1/20/06 5:14p btan
 * PR 19140: Do not encrypt generic mode files.
 * 
 * Hydra_Software_Devel/5   1/17/06 9:32p btan
 * PR 19140: Do not encrypt generic mode files.
 *
 ***************************************************************************/
#ifndef BHSM_DATATYPES_H__
#define BHSM_DATATYPES_H__

#include "bsp_s_version_number.h"

#if  (BHSM_IPTV ==1)	
#include  "bmem.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define 	BHSM_HOST_MIPS
#define   HSM_PI_FUNC 

/*
#undef BDBG_MSG
#define BDBG_MSG(x)   printf x
*/


#define BHSM_DEBUG_POLLING			(0)             /* to turn off */



/***************************************************************************
Summary:
Required default settings structure for Host Secure channel.

Description:
The default setting structure defines the default configure of
Host Secure channel when the interface is open.  Since ICM 
could support multiple Host Secure Bands, system may have
more than one default channel settings that each channel may have
different default channel settings.

See Also:
BHSM_GetDefaultSettings, BHSM_OpenChannel.

****************************************************************************/
typedef struct BHSM_ChannelSetting   
{
/* ToDo: Added all settiing here */

	unsigned char  ucUnknown;
	
} BHSM_ChannelSettings;


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
	
/* Definitions */

/* Host Secure module status codes */
#define BHSM_STATUS_FAILED				BERR_MAKE_CODE(BERR_ICM_ID, 1)  /* Return code for general failure. */ 
#define BHSM_STATUS_TIME_OUT			BERR_MAKE_CODE(BERR_ICM_ID, 2)  
#define BHSM_STATUS_PARM_LEN_ERR		BERR_MAKE_CODE(BERR_ICM_ID, 3) 
#define BHSM_STATUS_OWNER_ID_ERR		BERR_MAKE_CODE(BERR_ICM_ID, 7) 
#define BHSM_STATUS_INPUT_PARM_ERR	BERR_MAKE_CODE(BERR_ICM_ID, 4) 
#define BHSM_STATUS_HW_BUSY_ERR			BERR_MAKE_CODE(BERR_ICM_ID, 5) 
#define BHSM_STATUS_VERSION_ERR			BERR_MAKE_CODE(BERR_ICM_ID, 6) 

/* error is set after a loop of waiting is over, IRDY is still not ready*/
#define BHSM_STATUS_IRDY_ERR			BERR_MAKE_CODE(BERR_ICM_ID, 8)    

/* for a multistep HSM command, like BHSM_ResetKeySlotCtrlBits(), if failed in the middle ( not an atomic processing)  */
#define BHSM_STATUS_FAILED_FIRST	BERR_MAKE_CODE(BERR_ICM_ID, 9)          /* keyslot still ok to reuse*/
#define BHSM_STATUS_FAILED_REST		BERR_MAKE_CODE(BERR_ICM_ID, 10)        /* keyslot partially modified, no reuse, suggest to free*/


#define BHSM_STATUS_TESTDATA_DIFF_BENCHDATA		BERR_MAKE_CODE(BERR_ICM_ID, 11)   

#define BHSM_STATUS_MEMORY_HEAP_ERR				BERR_MAKE_CODE(BERR_ICM_ID, 12)      
#define BHSM_STATUS_CONTIGUOUS_MEMORY_ERR		BERR_MAKE_CODE(BERR_ICM_ID, 13)      
#define BHSM_STATUS_MEMORY_PHYCOVERTING_ERR		BERR_MAKE_CODE(BERR_ICM_ID, 14)      

#define BHSM_STATUS_PKE_IN_PROGRESS				BERR_MAKE_CODE(BERR_ICM_ID, 0xA4)

/**
Maximum number of supported Aegis command Interfaces in BHSM module.
**/
#define BHSM_MAX_SUPPOTED_CHANNELS    BHSM_HwModule_eMax


/*
Maximum size key data.
*/
#define BHSM_MAX_KEY_DATA_LEN	24




  
#if (BCHP_CHIP==7401) && (BCHP_VER == BCHP_VER_A0)
	#define BSP_S_CHIP_NUMBER				BSP_S_CHIP_NUMBER_7401  
	#define BSP_S_CHIP_VERSION				BSP_S_A0_CHIP_VERSION
	#define BSP_S_ROM_NUMBER				BSP_S_ROM_NUMBER_0
	#define BSP_MAJOR_VERSION			0x0
	#define BSP_MINOR_VERSION			0x0
	#define BCM7401A0
 
#elif (BCHP_CHIP==7401) && (BCHP_VER == BCHP_VER_B0)
	#define BSP_S_CHIP_NUMBER				BSP_S_CHIP_NUMBER_7401  
	#define BSP_S_CHIP_VERSION				BSP_S_B0_CHIP_VERSION
	#define BSP_S_ROM_NUMBER				BSP_S_ROM_NUMBER_0
	#define BSP_MAJOR_VERSION			0x0
	#define BSP_MINOR_VERSION			0x0
	#define BCM7401B0

#elif (BCHP_CHIP==7401) && (BCHP_VER == BCHP_VER_C0)
	#define BSP_S_CHIP_NUMBER				BSP_S_CHIP_NUMBER_7401  
	#define BSP_S_CHIP_VERSION				BSP_S_C0_CHIP_VERSION
	#define BSP_S_ROM_NUMBER				BSP_S_ROM_NUMBER_0
	#define BSP_MAJOR_VERSION			0x0
	#define BSP_MINOR_VERSION			0x0
	#define BCM7401C0

#elif (BCHP_CHIP==7401) && (BCHP_VER == BCHP_VER_C1)
	#define BSP_S_CHIP_NUMBER				BSP_S_CHIP_NUMBER_7401  
	#define BSP_S_CHIP_VERSION				BSP_S_C1_CHIP_VERSION
	#define BSP_S_ROM_NUMBER				BSP_S_ROM_NUMBER_0
	#define BSP_MAJOR_VERSION			0x0
	#define BSP_MINOR_VERSION			0x0
	#define BCM7401C1	
	
#elif (BCHP_CHIP==7401) && (BCHP_VER == BCHP_VER_C2)
	#define BSP_S_CHIP_NUMBER				BSP_S_CHIP_NUMBER_7401  
	#define BSP_S_CHIP_VERSION				BSP_S_C2_CHIP_VERSION
	#define BSP_S_ROM_NUMBER				BSP_S_ROM_NUMBER_0
	#define BSP_MAJOR_VERSION			0x0
	#define BSP_MINOR_VERSION			0x0
	#define BCM7401C2	

#elif (BCHP_CHIP==7401) && (BCHP_VER == BCHP_VER_C3)
	#define BSP_S_CHIP_NUMBER				BSP_S_CHIP_NUMBER_7401  
	#define BSP_S_CHIP_VERSION				BSP_S_C3_CHIP_VERSION
	#define BSP_S_ROM_NUMBER				BSP_S_ROM_NUMBER_0
	#define BSP_MAJOR_VERSION			0x0
	#define BSP_MINOR_VERSION			0x0
	#define BCM7401C3	
		 
#elif (BCHP_CHIP==7400) && (BCHP_VER == BCHP_VER_A0) 
	#define BSP_S_CHIP_NUMBER				BSP_S_CHIP_NUMBER_7400  
	#define BSP_S_CHIP_VERSION				BSP_S_A0_CHIP_VERSION
	#define BSP_S_ROM_NUMBER				BSP_S_ROM_NUMBER_0
	#define BSP_MAJOR_VERSION			0x0
	#define BSP_MINOR_VERSION			0x0
	#define BCM7400A0
 
#elif (BCHP_CHIP==7400)&& (BCHP_VER == BCHP_VER_B0) 
	#define BSP_S_CHIP_NUMBER				BSP_S_CHIP_NUMBER_7400  
	#define BSP_S_CHIP_VERSION				BSP_S_B0_CHIP_VERSION
	#define BSP_S_ROM_NUMBER				BSP_S_ROM_NUMBER_1
	#define BSP_MAJOR_VERSION			0x0
	#define BSP_MINOR_VERSION			0x0
	#define BCM7400B0	

#elif (BCHP_CHIP==7400)&& (BCHP_VER == BCHP_VER_C0) 
	#define BSP_S_CHIP_NUMBER				BSP_S_CHIP_NUMBER_7400  
	#define BSP_S_CHIP_VERSION				BSP_S_C0_CHIP_VERSION
	#define BSP_S_ROM_NUMBER				BSP_S_ROM_NUMBER_2
	#define BSP_MAJOR_VERSION			0x0
	#define BSP_MINOR_VERSION			0x0
	#define BCM7400C0	

#elif (BCHP_CHIP==7400)&& (BCHP_VER == BCHP_VER_D0) 
	#define BSP_S_CHIP_NUMBER				BSP_S_CHIP_NUMBER_7400  
	#define BSP_S_CHIP_VERSION				BSP_S_D0_CHIP_VERSION
	#define BSP_S_ROM_NUMBER				BSP_S_ROM_NUMBER_3
	#define BSP_MAJOR_VERSION			0x0
	#define BSP_MINOR_VERSION			0x0
	#define BCM7400D0	

#elif (BCHP_CHIP==7400)&& (BCHP_VER == BCHP_VER_D1) 
	#define BSP_S_CHIP_NUMBER				BSP_S_CHIP_NUMBER_7400  
	#define BSP_S_CHIP_VERSION				BSP_S_D1_CHIP_VERSION
	#define BSP_S_ROM_NUMBER				BSP_S_ROM_NUMBER_3
	#define BSP_MAJOR_VERSION			0x0
	#define BSP_MINOR_VERSION			0x0
	#define BCM7400D1	

#elif (BCHP_CHIP==7400)&& (BCHP_VER == BCHP_VER_E0) 
	#define BSP_S_CHIP_NUMBER				BSP_S_CHIP_NUMBER_7400  
	#define BSP_S_CHIP_VERSION				BSP_S_E0_CHIP_VERSION
	#define BSP_S_ROM_NUMBER				BSP_S_ROM_NUMBER_3
	#define BSP_MAJOR_VERSION			0x0
	#define BSP_MINOR_VERSION			0x0
	#define BCM7400E0	

#elif (BCHP_CHIP==7440) && (BCHP_VER == BCHP_VER_A0) 
	#define BSP_S_CHIP_NUMBER				BSP_S_CHIP_NUMBER_7440  
	#define BSP_S_CHIP_VERSION				BSP_S_A0_CHIP_VERSION
	#define BSP_S_ROM_NUMBER				BSP_S_ROM_NUMBER_0
	#define BSP_MAJOR_VERSION			0x0
	#define BSP_MINOR_VERSION			0x0
	#define BCM7440A0
	 
#elif (BCHP_CHIP==7118)&& (BCHP_VER == BCHP_VER_A0)
	#define BSP_S_CHIP_NUMBER				BSP_S_CHIP_NUMBER_7118  
	#define BSP_S_CHIP_VERSION				BSP_S_A0_CHIP_VERSION
	#define BSP_S_ROM_NUMBER				BSP_S_ROM_NUMBER_0
	#define BSP_MAJOR_VERSION			0x0
	#define BSP_MINOR_VERSION			0x0
	#define BCM7118A0
	
#elif (BCHP_CHIP==7118)&& (BCHP_VER == BCHP_VER_A1)
	#define BSP_S_CHIP_NUMBER				BSP_S_CHIP_NUMBER_7118  
	#define BSP_S_CHIP_VERSION				BSP_S_A1_CHIP_VERSION
	#define BSP_S_ROM_NUMBER				BSP_S_ROM_NUMBER_0
	#define BSP_MAJOR_VERSION			0x0
	#define BSP_MINOR_VERSION			0x0
	#define BCM7118A1

#elif (BCHP_CHIP==7118)&& (BCHP_VER == BCHP_VER_B0)
	#define BSP_S_CHIP_NUMBER				BSP_S_CHIP_NUMBER_7118  
	#define BSP_S_CHIP_VERSION				BSP_S_B0_CHIP_VERSION
	#define BSP_S_ROM_NUMBER				BSP_S_ROM_NUMBER_1
	#define BSP_MAJOR_VERSION			0x0
	#define BSP_MINOR_VERSION			0x0
	#define BCM7118B0

#elif (BCHP_CHIP==7118)&& (BCHP_VER == BCHP_VER_C0)
	#define BSP_S_CHIP_NUMBER				BSP_S_CHIP_NUMBER_7118  
	#define BSP_S_CHIP_VERSION				BSP_S_C0_CHIP_VERSION
	#define BSP_S_ROM_NUMBER				BSP_S_ROM_NUMBER_1
	#define BSP_MAJOR_VERSION			0x0
	#define BSP_MINOR_VERSION			0x0
	#define BCM7118C0

#elif (BCHP_CHIP==7403) && (BCHP_VER == BCHP_VER_A0)
	#define BSP_S_CHIP_NUMBER				BSP_S_CHIP_NUMBER_7403  
	#define BSP_S_CHIP_VERSION				BSP_S_A0_CHIP_VERSION
	#define BSP_S_ROM_NUMBER				BSP_S_ROM_NUMBER_0
	#define BSP_MAJOR_VERSION			0x0
	#define BSP_MINOR_VERSION			0x0
	#define BCM7403A0	/* BCM7401C1	  */

#elif (BCHP_CHIP==7403) && (BCHP_VER == BCHP_VER_A1)
	#define BSP_S_CHIP_NUMBER				BSP_S_CHIP_NUMBER_7403  
	#define BSP_S_CHIP_VERSION				BSP_S_A1_CHIP_VERSION
	#define BSP_S_ROM_NUMBER				BSP_S_ROM_NUMBER_0
	#define BSP_MAJOR_VERSION			0x0
	#define BSP_MINOR_VERSION			0x0
	#define BCM7403A1	

#elif (BCHP_CHIP==7405) && (BCHP_VER == BCHP_VER_A0)
	#define BSP_S_CHIP_NUMBER				BSP_S_CHIP_NUMBER_7405  
	#define BSP_S_CHIP_VERSION				BSP_S_A0_CHIP_VERSION
	#define BSP_S_ROM_NUMBER				BSP_S_ROM_NUMBER_0
	#define BSP_MAJOR_VERSION			0x0
	#define BSP_MINOR_VERSION			0x0
	#define BCM7405A0	

#elif (BCHP_CHIP==7405) && (BCHP_VER == BCHP_VER_A1)
	#define BSP_S_CHIP_NUMBER				BSP_S_CHIP_NUMBER_7405  
	#define BSP_S_CHIP_VERSION				BSP_S_A1_CHIP_VERSION
	#define BSP_S_ROM_NUMBER				BSP_S_ROM_NUMBER_0
	#define BSP_MAJOR_VERSION			0x0
	#define BSP_MINOR_VERSION			0x0
	#define BCM7405A1	

#elif (BCHP_CHIP==7405) && (BCHP_VER == BCHP_VER_B0)
	#define BSP_S_CHIP_NUMBER				BSP_S_CHIP_NUMBER_7405  
	#define BSP_S_CHIP_VERSION				BSP_S_B0_CHIP_VERSION
	#define BSP_S_ROM_NUMBER				BSP_S_ROM_NUMBER_0
	#define BSP_MAJOR_VERSION			0x0
	#define BSP_MINOR_VERSION			0x0
	#define BCM7405B0	

#elif (BCHP_CHIP==7405) && (BCHP_VER == BCHP_VER_C0)
	#define BSP_S_CHIP_NUMBER				BSP_S_CHIP_NUMBER_7405  
	#define BSP_S_CHIP_VERSION				BSP_S_C0_CHIP_VERSION
	#define BSP_S_ROM_NUMBER				BSP_S_ROM_NUMBER_0
	#define BSP_MAJOR_VERSION			0x0
	#define BSP_MINOR_VERSION			0x0
	#define BCM7405C0	

#elif (BCHP_CHIP==7325)&& (BCHP_VER == BCHP_VER_A0)
	#define BSP_S_CHIP_NUMBER				BSP_S_CHIP_NUMBER_7325  
	#define BSP_S_CHIP_VERSION				BSP_S_A0_CHIP_VERSION
	#define BSP_S_ROM_NUMBER				BSP_S_ROM_NUMBER_0
	#define BSP_MAJOR_VERSION			0x0
	#define BSP_MINOR_VERSION			0x0
	#define BCM7325A0

#elif (BCHP_CHIP==7325)&& (BCHP_VER == BCHP_VER_B0)
	#define BSP_S_CHIP_NUMBER				BSP_S_CHIP_NUMBER_7325  
	#define BSP_S_CHIP_VERSION				BSP_S_B0_CHIP_VERSION
	#define BSP_S_ROM_NUMBER				BSP_S_ROM_NUMBER_0
	#define BSP_MAJOR_VERSION			0x0
	#define BSP_MINOR_VERSION			0x0
	#define BCM7325B0

#elif (BCHP_CHIP==7335)&& ((BCHP_VER == BCHP_VER_A0) || (BCHP_VER == BCHP_VER_A1))
	#define BSP_S_CHIP_NUMBER				BSP_S_CHIP_NUMBER_7335  
	#define BSP_S_CHIP_VERSION				BSP_S_A0_CHIP_VERSION
	#define BSP_S_ROM_NUMBER				BSP_S_ROM_NUMBER_0
	#define BSP_MAJOR_VERSION			0x0
	#define BSP_MINOR_VERSION			0x0
	#define BCM7335A0

#elif (BCHP_CHIP==7335)&& (BCHP_VER == BCHP_VER_B0)
        #define BSP_S_CHIP_NUMBER                               BSP_S_CHIP_NUMBER_7335
        #define BSP_S_CHIP_VERSION                              BSP_S_B0_CHIP_VERSION
        #define BSP_S_ROM_NUMBER                                BSP_S_ROM_NUMBER_0
        #define BSP_MAJOR_VERSION                       0x0
        #define BSP_MINOR_VERSION                       0x0
        #define BCM7335B0
	
#elif (BCHP_CHIP==7336)&& (BCHP_VER == BCHP_VER_A0)
        #define BSP_S_CHIP_NUMBER                               BSP_S_CHIP_NUMBER_7336
        #define BSP_S_CHIP_VERSION                              BSP_S_A0_CHIP_VERSION
        #define BSP_S_ROM_NUMBER                                BSP_S_ROM_NUMBER_0
        #define BSP_MAJOR_VERSION                       0x0
        #define BSP_MINOR_VERSION                       0x0
        #define BCM7336A0
	
#elif (BCHP_CHIP==7420)&& (BCHP_VER == BCHP_VER_A0)
        #define BSP_S_CHIP_NUMBER                               BSP_S_CHIP_NUMBER_7420
        #define BSP_S_CHIP_VERSION                              BSP_S_A0_CHIP_VERSION
        #define BSP_S_ROM_NUMBER                                BSP_S_ROM_NUMBER_0
        #define BSP_MAJOR_VERSION                       0x0
        #define BSP_MINOR_VERSION                       0x0
        #define BCM7420A0

#elif (BCHP_CHIP==7420)&& (BCHP_VER == BCHP_VER_A1)
        #define BSP_S_CHIP_NUMBER                               BSP_S_CHIP_NUMBER_7420
        #define BSP_S_CHIP_VERSION                              BSP_S_A1_CHIP_VERSION
        #define BSP_S_ROM_NUMBER                                BSP_S_ROM_NUMBER_0
        #define BSP_MAJOR_VERSION                       0x0
        #define BSP_MINOR_VERSION                       0x0
        #define BCM7420A1

#elif (BCHP_CHIP==3563)&& (BCHP_VER == BCHP_VER_A0)
        #define BSP_S_CHIP_NUMBER                               BSP_S_CHIP_NUMBER_3563
        #define BSP_S_CHIP_VERSION                              BSP_S_A0_CHIP_VERSION
        #define BSP_S_ROM_NUMBER                                BSP_S_ROM_NUMBER_0
        #define BSP_MAJOR_VERSION                       0x0
        #define BSP_MINOR_VERSION                       0x0
        #define BCM3563A0

#elif (BCHP_CHIP==3563)&& (BCHP_VER == BCHP_VER_C0)
        #define BSP_S_CHIP_NUMBER                               BSP_S_CHIP_NUMBER_3563
        #define BSP_S_CHIP_VERSION                              BSP_S_C0_CHIP_VERSION
        #define BSP_S_ROM_NUMBER                                BSP_S_ROM_NUMBER_0
        #define BSP_MAJOR_VERSION                       0x0
        #define BSP_MINOR_VERSION                       0x0
        #define BCM3563C0

#elif (BCHP_CHIP==3563)&& (BCHP_VER == BCHP_VER_D0)
        #define BSP_S_CHIP_NUMBER                               BSP_S_CHIP_NUMBER_3563
        #define BSP_S_CHIP_VERSION                              BSP_S_D0_CHIP_VERSION
        #define BSP_S_ROM_NUMBER                                BSP_S_ROM_NUMBER_0
        #define BSP_MAJOR_VERSION                       0x0
        #define BSP_MINOR_VERSION                       0x0
        #define BCM3563D0

#elif (BCHP_CHIP==3563)&& (BCHP_VER == BCHP_VER_A1)
        #define BSP_S_CHIP_NUMBER                               BSP_S_CHIP_NUMBER_3563
        #define BSP_S_CHIP_VERSION                              BSP_S_A1_CHIP_VERSION
        #define BSP_S_ROM_NUMBER                                BSP_S_ROM_NUMBER_0
        #define BSP_MAJOR_VERSION                       0x0
        #define BSP_MINOR_VERSION                       0x0
        #define BCM3563A1

#elif (BCHP_CHIP==3548)&& (BCHP_VER == BCHP_VER_A0)
        #define BSP_S_CHIP_NUMBER                               BSP_S_CHIP_NUMBER_3548
        #define BSP_S_CHIP_VERSION                              BSP_S_A0_CHIP_VERSION
        #define BSP_S_ROM_NUMBER                                BSP_S_ROM_NUMBER_0
        #define BSP_MAJOR_VERSION                       0x0
        #define BSP_MINOR_VERSION                       0x0
        #define BCM3548A0

#elif (BCHP_CHIP==3548)&& (BCHP_VER == BCHP_VER_B0)
        #define BSP_S_CHIP_NUMBER                               BSP_S_CHIP_NUMBER_3548
        #define BSP_S_CHIP_VERSION                              BSP_S_B0_CHIP_VERSION
        #define BSP_S_ROM_NUMBER                                BSP_S_ROM_NUMBER_0
        #define BSP_MAJOR_VERSION                       0x0
        #define BSP_MINOR_VERSION                       0x0
        #define BCM3548B0

#elif (BCHP_CHIP==3548)&& (BCHP_VER == BCHP_VER_B1)
        #define BSP_S_CHIP_NUMBER                               BSP_S_CHIP_NUMBER_3548
        #define BSP_S_CHIP_VERSION                              BSP_S_B1_CHIP_VERSION
        #define BSP_S_ROM_NUMBER                                BSP_S_ROM_NUMBER_0
        #define BSP_MAJOR_VERSION                       0x0
        #define BSP_MINOR_VERSION                       0x0
        #define BCM3548B1

#elif (BCHP_CHIP==3548)&& (BCHP_VER == BCHP_VER_B2)
        #define BSP_S_CHIP_NUMBER                               BSP_S_CHIP_NUMBER_3548
        #define BSP_S_CHIP_VERSION                              BSP_S_B2_CHIP_VERSION
        #define BSP_S_ROM_NUMBER                                BSP_S_ROM_NUMBER_0
        #define BSP_MAJOR_VERSION                       0x0
        #define BSP_MINOR_VERSION                       0x0
        #define BCM3548B2

#elif (BCHP_CHIP==3556)&& (BCHP_VER == BCHP_VER_A0)
        #define BSP_S_CHIP_NUMBER                               BSP_S_CHIP_NUMBER_3556
        #define BSP_S_CHIP_VERSION                              BSP_S_A0_CHIP_VERSION
        #define BSP_S_ROM_NUMBER                                BSP_S_ROM_NUMBER_0
        #define BSP_MAJOR_VERSION                       0x0
        #define BSP_MINOR_VERSION                       0x0
        #define BCM3556A0

#elif (BCHP_CHIP==3556)&& (BCHP_VER == BCHP_VER_B0)
        #define BSP_S_CHIP_NUMBER                               BSP_S_CHIP_NUMBER_3556
        #define BSP_S_CHIP_VERSION                              BSP_S_B0_CHIP_VERSION
        #define BSP_S_ROM_NUMBER                                BSP_S_ROM_NUMBER_0
        #define BSP_MAJOR_VERSION                       0x0
        #define BSP_MINOR_VERSION                       0x0
        #define BCM3556B0

#elif (BCHP_CHIP==3556)&& (BCHP_VER == BCHP_VER_B1)
        #define BSP_S_CHIP_NUMBER                               BSP_S_CHIP_NUMBER_3556
        #define BSP_S_CHIP_VERSION                              BSP_S_B1_CHIP_VERSION
        #define BSP_S_ROM_NUMBER                                BSP_S_ROM_NUMBER_0
        #define BSP_MAJOR_VERSION                       0x0
        #define BSP_MINOR_VERSION                       0x0
        #define BCM3556B1

#elif (BCHP_CHIP==3556)&& (BCHP_VER == BCHP_VER_B2)
        #define BSP_S_CHIP_NUMBER                               BSP_S_CHIP_NUMBER_3556
        #define BSP_S_CHIP_VERSION                              BSP_S_B2_CHIP_VERSION
        #define BSP_S_ROM_NUMBER                                BSP_S_ROM_NUMBER_0
        #define BSP_MAJOR_VERSION                       0x0
        #define BSP_MINOR_VERSION                       0x0
        #define BCM3556B2

#elif (BCHP_CHIP==3556)&& (BCHP_VER == BCHP_VER_C0)
        #define BSP_S_CHIP_NUMBER                               BSP_S_CHIP_NUMBER_3556
        #define BSP_S_CHIP_VERSION                              BSP_S_C0_CHIP_VERSION
        #define BSP_S_ROM_NUMBER                                BSP_S_ROM_NUMBER_0
        #define BSP_MAJOR_VERSION                       0x0
        #define BSP_MINOR_VERSION                       0x0
        #define BCM3556C0
		
#elif (BCHP_CHIP==35130)&& (BCHP_VER == BCHP_VER_A0)
        #define BSP_S_CHIP_NUMBER                               BSP_S_CHIP_NUMBER_35130
        #define BSP_S_CHIP_VERSION                              BSP_S_A0_CHIP_VERSION
        #define BSP_S_ROM_NUMBER                                BSP_S_ROM_NUMBER_0
        #define BSP_MAJOR_VERSION                       0x0
        #define BSP_MINOR_VERSION                       0x0
        #define BCM35130A0


#else
#error "Does not have a version number"
#endif


#define BSP_S_DRV_VERSION \
		(BSP_S_CHIP_NUMBER << BSP_S_CHIP_NUMBER_SHIFT) | \
		(BSP_S_CHIP_VERSION << BSP_S_CHIP_VERSION_SHIFT) | \
		(BSP_MAJOR_VERSION << BSP_S_MAJOR_VERSION_SHIFT)  | \
		(BSP_MINOR_VERSION << BSP_S_MINOR_VERSION_SHIFT) 

#define BSP_S_CHECK_VERSION_MASK	0xFFFF0000


#define BHSM_SLOT_NUM_INIT_VAL		0xFFFF

/* End of Definitions */



/* Enum Types */

/***************************************************************************
Summary:
This enum represents all the HASM Interrupt Types.

Description:
This enumeration defines all the HASM Interrupt Types. 

See Also:


****************************************************************************/
typedef enum  BHSM_IntrType {
      BHSM_IntrType_eOLoad1,     /* Output Load1 interrupt */
      BHSM_IntrType_eOLoad2,     /* Output Load2 interrupt */
      BHSM_IntrType_eReserved0,  /* Other Exception Interrupt */
      BHSM_IntrType_eException,  /* Other Exception Interrupt */

      BHSM_IntrType_eMax
  
} BHSM_IntrType;

/***************************************************************************
Summary:
This enum is to identify the Host Secure Hardware Module.

Description:
This enumeration defines the supported Host Secure Hardware Module.

See Also:

****************************************************************************/
typedef enum BHSM_HwModule {
   BHSM_HwModule_eCmdInterface1,    /* Cmd Interface 1 for Cancel commands */
   BHSM_HwModule_eCmdInterface2,   /* Cmd Interface 2 for all operation commands */
   BHSM_HwModule_eMax    /* Maximum number of Hardware Modules */   
} BHSM_HwModule;

/* End of Enum Types */


/*
This defines the function prototype that is used for Aegis Command callbacks.
*/
typedef void (*BHSM_IsrCallbackFunc)( void * inp_handle, void * inp_data);

/*  each bit is for an enum value, as flag, they can be ORed
	BHSM_CTRLS_POLLINGORISR | BHSM_CTRLS_TIMEOUT is good

      first LSB byte is to identify field start-location in bits
      2nd LSB byte is to identify timeout, channel number.
 */ 
typedef enum  BHSM_SpecialCtrol_Flags{
				BHSM_CTRLS_POLLINGORISR=1,   /* start at bit0, */				
				BHSM_CTRLS_TIMEOUT=2,     /* bit1 Timeout change*/
				/* BHSM_CTRLS_CHANNELNUM=4   bit2 for channel number, defined yet not used at all for 3563*/
				/* please add the new flags here, 1 bit per field*/					

				BHSM_CTRLS_MAX
				
} BHSM_SpecialCtrol_Flag_e;

/* this is dependent on actual BHSM_SpecialCtrol_Flag_e content, 2 flags = 0x3, 3 flags=0x7, 4 flags = 0xF ...*/
#define BHSM_SPECIALCTROL_FLAGS      		(0x00000003)       

#define BHSM_SPECIALCTROL_TIMEOUT_MAX   (120000)      /* in ms , i.e. 120s*/
#define BHSM_SPECIALCTROL_TIMEOUT_MIN   (1)      		/* min is 1ms */

#define BHSM_CTRL_SET_POLLING		(1)
#define BHSM_CTRL_SET_ISR		(0)

/***************************************************************************
Summary:
New settings structure for Host Secure module.

Description:
The new setting structure defines the updated configure of BHSM module which has a default one,

See Also:
BHSM_Settings, BHSM_Open()

****************************************************************************/   
typedef struct BHSM_NewSettings{					 
             /* In: mandatory. select what BHSM controls to update, one or multipe. 
                      control bits shall not be overlapped. 1 to update the corresponding control. 0 for no update of it.*/
		BHSM_SpecialCtrol_Flag_e   whichControl;
			 
  	      /* In: conditional on 'whichControl', what value is set for this control field. Bit0 is for ISR or 
  	               Polling selection.     Other bits RFU.*/			 
	  	unsigned long 				ctrlValue;

	      /* In: conditional on 'whichControl', new timeout value for polling or wairt-for-ISR-from-BSP */
	  	unsigned long				timeoutMs;


	      /* In: conditional on 'whichControl', the max number of Aegis Cmd Interfaces supported 
	  	unsigned long				maxChannelNum;                                      */


	      /*  if any new settings to be added/updated, please append here */
} BHSM_NewSettings_t;	  	

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

	unsigned char	ucMaxChannels;   /* maximum Aegis Cmd Interfaces supported */
	unsigned long ulTimeOutInMilSecs;

	BHSM_IsrCallbackFunc exceptionCBfunc; /* callback function for exception callback  */	

	unsigned long			uSpecialControl;  /*  bit0 = 1/BSP-polling, 0/BSP-ISR; other bits reserved for future use*/

#if  (BHSM_IPTV ==1)	
	BMEM_Heap_Handle	hHeap;		/* newly added for IPTV contiguous memeory support inside HSM*/
#endif

}BHSM_Settings;




#ifdef __cplusplus
}
#endif

#endif /* BHSM_DATATYPES_H__ */
	
