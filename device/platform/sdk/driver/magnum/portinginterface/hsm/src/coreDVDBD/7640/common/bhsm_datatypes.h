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
 * $brcm_Workfile: bhsm_datatypes.h $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 9/7/11 8:01a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hsm/7640/a0/common/bhsm_datatypes.h $
 * 
 * Hydra_Software_Devel/3   9/7/11 8:01a cdisc
 * SWBLURAY-27243: updating for 7640 B0
 * 
 * Hydra_Software_Devel/2   2/8/11 12:36p cdisc
 * SWBLURAY-23691: adding 7640 specific mods
 * 
 * Hydra_Software_Devel/1   2/8/11 9:07a cdisc
 * SWBLURAY-23691: branching files
 * 
 * Hydra_Software_Devel/5   1/28/11 12:48p atruong
 * SW7425-23: Add HSM PI Support For 97425
 * 
 * Hydra_Software_Devel/4   1/24/11 10:49a atruong
 * SW7422-26: Add HSM PI Support For 7422A0  - CC header format
 *
 ******************************************************************************/

#ifndef BHSM_DATATYPES_H__
#define BHSM_DATATYPES_H__

#include "bsp_s_version_number.h"
	
#include  "bmem.h"

#ifdef __cplusplus
extern "C" {
#endif

#define 	BHSM_HOST_MIPS
#define   HSM_PI_FUNC 


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
#define BHSM_STATUS_VKL_ASSOCIATION_ERR			BERR_MAKE_CODE(BERR_ICM_ID, 15)
#define BHSM_STATUS_UNCONFIGURED_KEYSLOT_ERR	BERR_MAKE_CODE(BERR_ICM_ID, 16)
#define BHSM_STATUS_PKE_IN_PROGRESS				BERR_MAKE_CODE(BERR_ICM_ID, 0xA4)


/**
Maximum number of supported Aegis command Interfaces in BHSM module.
**/
#define BHSM_MAX_SUPPOTED_CHANNELS    BHSM_HwModule_eMax


/*
Maximum size key data.
*/
#define BHSM_MAX_KEY_DATA_LEN	24

#if (BCHP_CHIP == 7422) && (BCHP_VER == BCHP_VER_A0)
        #define BSP_S_CHIP_NUMBER                               BSP_S_CHIP_NUMBER_7422
        #define BSP_S_CHIP_VERSION                              BSP_S_A0_CHIP_VERSION
        #define BSP_S_ROM_NUMBER                                BSP_S_ROM_NUMBER_0
        #define BSP_MAJOR_VERSION                       0x0
        #define BSP_MINOR_VERSION                       0x0
        #define BCM7422A0
#elif (BCHP_CHIP == 7425) && (BCHP_VER == BCHP_VER_A0)
        #define BSP_S_CHIP_NUMBER                               BSP_S_CHIP_NUMBER_7425
        #define BSP_S_CHIP_VERSION                              BSP_S_A0_CHIP_VERSION
        #define BSP_S_ROM_NUMBER                                BSP_S_ROM_NUMBER_0
        #define BSP_MAJOR_VERSION                       0x0
        #define BSP_MINOR_VERSION                       0x0
        #define BCM7425A0
#elif (BCHP_CHIP == 7344) && (BCHP_VER == BCHP_VER_A0)
        #define BSP_S_CHIP_NUMBER                               BSP_S_CHIP_NUMBER_7344
        #define BSP_S_CHIP_VERSION                              BSP_S_A0_CHIP_VERSION
        #define BSP_S_ROM_NUMBER                                BSP_S_ROM_NUMBER_0
        #define BSP_MAJOR_VERSION                       0x0
        #define BSP_MINOR_VERSION                       0x0
        #define BCM7344A0
#elif (BCHP_CHIP == 7346) && (BCHP_VER == BCHP_VER_A0)
        #define BSP_S_CHIP_NUMBER                               BSP_S_CHIP_NUMBER_7346
        #define BSP_S_CHIP_VERSION                              BSP_S_A0_CHIP_VERSION
        #define BSP_S_ROM_NUMBER                                BSP_S_ROM_NUMBER_0
        #define BSP_MAJOR_VERSION                       0x0
        #define BSP_MINOR_VERSION                       0x0
        #define BCM7346A0
#elif (BCHP_CHIP == 7231) && (BCHP_VER == BCHP_VER_A0)
        #define BSP_S_CHIP_NUMBER                               BSP_S_CHIP_NUMBER_7231
        #define BSP_S_CHIP_VERSION                              BSP_S_A0_CHIP_VERSION
        #define BSP_S_ROM_NUMBER                                BSP_S_ROM_NUMBER_0
        #define BSP_MAJOR_VERSION                       0x0
        #define BSP_MINOR_VERSION                       0x0
        #define BCM7231A0
#elif (BCHP_CHIP == 7640) && (BCHP_VER == BCHP_VER_A0)
        #define BSP_S_CHIP_NUMBER                               BSP_S_CHIP_NUMBER_7640
        #define BSP_S_CHIP_VERSION                              BSP_S_A0_CHIP_VERSION
        #define BSP_S_ROM_NUMBER                                BSP_S_ROM_NUMBER_0
        #define BSP_MAJOR_VERSION                       0x0
        #define BSP_MINOR_VERSION                       0x0
        #define BCM7640A0
#elif (BCHP_CHIP == 7640) && (BCHP_VER == BCHP_VER_B0)
        #define BSP_S_CHIP_NUMBER                               BSP_S_CHIP_NUMBER_7640
        #define BSP_S_CHIP_VERSION                              BSP_S_B0_CHIP_VERSION
        #define BSP_S_ROM_NUMBER                                BSP_S_ROM_NUMBER_0
        #define BSP_MAJOR_VERSION                       0x0
        #define BSP_MINOR_VERSION                       0x0
        #define BCM7640B0
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

	BMEM_Heap_Handle	hHeap;		/* newly added for IPTV contiguous memeory support inside HSM*/

}BHSM_Settings;




#ifdef __cplusplus
}
#endif

#endif /* BHSM_DATATYPES_H__ */
	