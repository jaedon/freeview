/***************************************************************************
 *     Copyright (c) 2004-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bdsp_raaga_cmdresp_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/24 $
 * $brcm_Date: 11/29/12 12:02a $
*
 * Module Description:
*
* Revision History:
*
 * $brcm_Log: /magnum/basemodules/dsp/src/raaga/bdsp_raaga_cmdresp_priv.h $
 * 
 * Hydra_Software_Devel/24   11/29/12 12:02a ananthan
 * SW7425-4242: Coverity fix
 * 
 * Hydra_Software_Devel/23   11/28/12 1:27a ananthan
 * SW7425-4242: Add SCM3 to mainline.
 * 
 * Hydra_Software_Devel/22   11/26/12 1:19p ananthan
 * SW7425-4242: Add SCM3 interface and merge SCM to mainline.
 * 
 * Hydra_Software_Devel/21   11/7/12 6:11p ananthan
 * SW7425-3808: Added BDSP changes to add new Event ID to notify change in
 * dialnorm.
 * 
 * Hydra_Software_Devel/20   10/6/12 2:49a ananthan
 * SW7346-914: Adding EventId Mask for Ancillary Data Interrupt.
 * 
 * Hydra_Software_Devel/20   10/5/12 7:06p nihar
 * SW7346-914: Adding event ID mask for ancillary data interrupt.
 * 
 * Hydra_Software_Devel/19   9/18/12 6:35p ananthan
 * SW7346-914: Adding support for MPEG Ancillary data parsing.
 * 
 * Hydra_Software_Devel/18   9/11/12 1:21p gprasad
 * SW7425-3831: Matching host interface files
 * 
 * Hydra_Software_Devel/17   9/11/12 1:16p gprasad
 * SW7425-3831: Removing the #define to enable the changes for initial
 * mute fill
 * 
 * Hydra_Software_Devel/16   9/7/12 12:17p gprasad
 * SW7425-3831: Adding support for initial mute frame fill in RT transcode
 * cases
 * 
 * Hydra_Software_Devel/15   4/11/12 7:52p gprasad
 * SW7425-2863: Add an interrupt to notify when the o/p buffer gets full
 * during the encoding process
 * 
 * Hydra_Software_Devel/14   2/15/12 3:05p ashoky
 * SW7425-2308: Adding interface to put audio firmware in audio gap fill
 * mode.
 * 
 * Hydra_Software_Devel/13   2/3/12 11:55a ashoky
 * FWRAA-362: Raaga dsp sram optimization. Providing memory to swap
 * resident data.
 * 
 * Hydra_Software_Devel/12   1/20/12 3:25p gprasad
 * FWRAA-377 : Merging h264 video encoder to mainline
 * 
 * Hydra_Software_Devel/11   8/17/11 12:51p gprasad
 * FWRAA-280: Syncing basemodules and Raaga host interface files
 * 
 * Hydra_Software_Devel/10   7/28/11 2:20p gprasad
 * SW7425-468: audio transcoder generated timestamps should not restart
 * timebase before explicit stop
 * 
 * Hydra_Software_Devel/9   6/22/11 3:31p gprasad
 * SW7422-370: Code review changes in bdsp_tyes.h
 * 
 * Hydra_Software_Devel/8   6/14/11 10:05a gprasad
 * SW7422-389: Add Dolby MS11 support - Merging changes to main line
 * 
 * Hydra_Software_Devel/MS11_DEVEL_BRANCH/3   6/9/11 1:17p gprasad
 * SW7422-389: [7422] Add Dolby MS11 support
 * 
 * Hydra_Software_Devel/7   5/20/11 6:26p gprasad
 * SW7422-389: [7425] Add Dolby MS11 support
 * 
 * Hydra_Software_Devel/6   5/12/11 5:31p gprasad
 * SW7422-389: [7422] Add Dolby MS11 support
 * 
 * Hydra_Software_Devel/MS11_DEVEL_BRANCH/1   5/11/11 11:13a gprasad
 * SW7422-389: [7422] Add Dolby MS11 support
 * 
 * Hydra_Software_Devel/4   4/14/11 6:52p gprasad
 * FWRAA-279: Phase 9p0 release for raaga
 * 
 * Hydra_Software_Devel/3   4/14/11 10:56a gprasad
 * FWRAA-279: Phase 9p0 release for raaga
 * 
 * Hydra_Software_Devel/2   4/6/11 11:39a gprasad
 * SW7425-286: [7425] Checkin raaga binaries to magnum into basemodules
 * folder
 * 
 * Hydra_Software_Devel/1   4/5/11 3:51p gprasad
 * SW7425-286: [7425] Checkin raaga binaries to magnum into basemodules
 * folder
 * 
 * Hydra_Software_Devel/4   1/25/11 5:33a gautamk
 * SW7422-191: Changes in TSM config and status structure
 * 
 * Hydra_Software_Devel/3   1/18/11 4:53a gautamk
 * SW7422-191: Implementing code review comments.
 * 
 * Hydra_Software_Devel/2   1/18/11 4:23a gautamk
 * SW7125-766: Added code for review comments.
 * 
 * Hydra_Software_Devel/1   1/13/11 5:38a gautamk
 * SW7422-191:[7422]Adding DSP support for Audio decode in Basemodule  It
 * includes - DSP Booting - Open time Download :  - Start Time Audio
 * Decode algorithm download  - Message buffer interaction with DSP:  -
 * Synchronized Interrupt:  - Async interrupts:  - Config Commands for
 * Decode/TSM/IDS: - Status Buffer for Decode/TSM/IDS:  - Start AC3 Audio
 * Decode:  - Pause/Resume Command:  - Frame Advance command:  - Device
 * level interrupt
 * 
 * Hydra_Software_Devel/2   8/3/10 7:46p gprasad
 * FWRAA-133: [35230] Merging all the host interface changes from phase
 * 6p0 to phase 7p5 take 2 into main line
 * 
 * Hydra_Software_Devel/Raaga_Phase7p5tk2_Branch/2   7/26/10 8:31p gprasad
 * FWRAA-133: [35230] Merging all the host interface changes from phase
 * 6p0 to phase 7p5 take 2 into Raaga code base
 * 
 * Hydra_Software_Devel/Raaga_Phase7p5tk2_Branch/1   7/20/10 2:28p gprasad
 * FWRAA-133: [35230] Merging all the host interface changes from phase
 * 6p0 to phase 7p5 take 2 into Raaga code base
* 
 * Hydra_Software_Devel/1   4/30/10 1:10p dlimaye
 * FW35230-30: [35230] Checkin CIT files in base4 Move the CIT files from
 * label RAAGA_FW35230-29_20100419 in base3 folder to the base4 folder
 * 
 * Hydra_Software_Devel/RAAGA_EMULATION/5   3/30/10 11:39a dlimaye
* FW35230-28: [35230] Checkin CIT files for BM
* 
 * Hydra_Software_Devel/RAAGA_EMULATION/4   3/12/10 10:30a dlimaye
* FWRAA-54: [35230] Unify CIT files between magnum and Raaga vobs
* 
 * Hydra_Software_Devel/RAAGA_EMULATION/2   11/26/09 2:59p dlimaye
 * FW35230-10: [35230] Add the functionality of Vom table download and
 * make emulation release
 * Modified the CIT interface file to support download of VOM table
 * 
 * Hydra_Software_Devel/RAAGA_EMULATION/1   9/11/09 1:46p dlimaye
 * FW35230-2:[3513]Check-in the initial Raaga CIT files + FW binaries to
 * magnum
 * Checking in CIT files in the branch RAAGA_EMULATION
* 
***************************************************************************/

#ifndef _BRAAGA_CMDRESP_PRIV_H__
#define _BRAAGA_CMDRESP_PRIV_H__

#define RAAGA_SCM_SUPPORT 1 /* TODO: Vijay SCM To be removed */
/* Need to remove later */

 /* The following are the various ID's given to each command 
from host to any task on DSP */

#define BDSP_RAAGA_ASYNC_EVENT_ID_OFFSET					((uint32_t)0x0)

#define BDSP_RAAGA_INVALID_COMMAND                         ((uint32_t)-1)

#define BDSP_RAAGA_COMMAND_ID_OFFSET                       ((uint32_t)0x100)

#define BDSP_RAAGA_ACK_ID_OFFSET                           ((uint32_t)0x200)

#define BDSP_RAAGA_RESPONSE_ID_OFFSET                      ((uint32_t)0x300)

#define BDSP_RAAGA_DEFAULT_EVENTS_ENABLE_MASK				((uint32_t)0x3)

	/** This is the starting Task ID offset which can be 
		used by Host to create any	new tasks in future **/
#define BDSP_RAAGA_TASK_ID_START_OFFSET					((uint32_t)15)	

	/** This is the maximum value of taskID which can be 
		used by host in future **/		
#define BDSP_RAAGA_TASK_MAX_ID_AVAILABLE					((uint32_t)56)


/*** Following are the various Event Ids used  ***/

#define BDSP_RAAGA_DUMMY_FRAME_GENERATION_EVENT_ID  \
        ((uint32_t)((BDSP_RAAGA_ASYNC_EVENT_ID_OFFSET) )  )				/** 0x00 **/

#define BDSP_RAAGA_FRAME_REPEAT_EVENT_ID    \
        ((uint32_t)((BDSP_RAAGA_DUMMY_FRAME_GENERATION_EVENT_ID) + 1)   )	/** 0x01 **/

#define BDSP_RAAGA_FRAME_DROP_FROM_CDB_EVENT_ID \
        ((uint32_t)((BDSP_RAAGA_FRAME_REPEAT_EVENT_ID) + 1) )				/** 0x02 **/

#define BDSP_RAAGA_TSM_FAIL_EVENT_ID    \
        ((uint32_t)((BDSP_RAAGA_FRAME_DROP_FROM_CDB_EVENT_ID) + 1))		/** 0x03 **/

#define BDSP_RAAGA_CDB_ITB_UNDERFLOW_EVENT_ID   \
        ((uint32_t)((BDSP_RAAGA_TSM_FAIL_EVENT_ID) + 1) )					/** 0x04 **/

#define BDSP_RAAGA_CDB_ITB_OVERFLOW_EVENT_ID    \
        ((uint32_t)((BDSP_RAAGA_CDB_ITB_UNDERFLOW_EVENT_ID) + 1)    )		/** 0x05 **/

#define BDSP_RAAGA_CRC_ERROR_EVENT_ID       \
        ((uint32_t)((BDSP_RAAGA_CDB_ITB_OVERFLOW_EVENT_ID) + 1) )			/** 0x06 **/

#define BDSP_RAAGA_AUDIO_MODE_CHANGE_EVENT_ID   \
        ((uint32_t)((BDSP_RAAGA_CRC_ERROR_EVENT_ID) + 1)    )				/** 0x07 **/

#define BDSP_RAAGA_BIT_RATE_CHANGE_EVENT_ID \
        ((uint32_t)((BDSP_RAAGA_AUDIO_MODE_CHANGE_EVENT_ID) + 1)    )		/** 0x08 **/

#define BDSP_RAAGA_SAMPLING_RATE_CHANGE_EVENT_ID    \
        ((uint32_t)((BDSP_RAAGA_BIT_RATE_CHANGE_EVENT_ID) + 1)  )			/** 0x09 **/

#define BDSP_RAAGA_FRAME_SYNC_LOCK_EVENT_ID \
        ((uint32_t)((BDSP_RAAGA_SAMPLING_RATE_CHANGE_EVENT_ID) + 1))		/** 0x0A */

#define BDSP_RAAGA_FRAME_SYNC_LOCK_LOST_EVENT_ID    \
        ((uint32_t)((BDSP_RAAGA_FRAME_SYNC_LOCK_EVENT_ID) + 1))			/** 0x0B **/

#define BDSP_RAAGA_STC_PTS_DIFF_LOWER_THRESHOLD_EVENT_ID    \
        ((uint32_t)((BDSP_RAAGA_FRAME_SYNC_LOCK_LOST_EVENT_ID) + 1))		/** 0x0C */

#define BDSP_RAAGA_STC_PTS_DIFF_UPPER_THRESHOLD_EVENT_ID    \
    ((uint32_t)((BDSP_RAAGA_STC_PTS_DIFF_LOWER_THRESHOLD_EVENT_ID) + 1))	/** 0x0D **/

#define BDSP_RAAGA_DESIRED_CDB_BUFFER_LEVEL_EVENT_ID    \
    ((uint32_t)((BDSP_RAAGA_STC_PTS_DIFF_UPPER_THRESHOLD_EVENT_ID) + 1))	/** 0x0E **/

#define BDSP_RAAGA_FIRST_PTS_RECEIVED_FROM_ITB_EVENT_ID \
    ((uint32_t)((BDSP_RAAGA_DESIRED_CDB_BUFFER_LEVEL_EVENT_ID) + 1))		/** 0x0F **/

#define BDSP_RAAGA_PTS_ERR_EVENT_ID    \
    ((uint32_t)((BDSP_RAAGA_FIRST_PTS_RECEIVED_FROM_ITB_EVENT_ID) + 1))	/** 0x10 **/

#define BDSP_RAAGA_TSM_LOCK_EVENT_ID    \
    ((uint32_t)((BDSP_RAAGA_PTS_ERR_EVENT_ID) + 1))						/** 0x11 **/

#define BDSP_RAAGA_START_PTS_EVENT_ID    \
    ((uint32_t)((BDSP_RAAGA_TSM_LOCK_EVENT_ID) + 1))						/** 0x12 **/

#define BDSP_RAAGA_STOP_PTS_EVENT_ID    \
    ((uint32_t)((BDSP_RAAGA_START_PTS_EVENT_ID) + 1))						/** 0x13 **/

#define BDSP_RAAGA_ASTMTSM_PASS_EVENT_ID    \
    ((uint32_t)((BDSP_RAAGA_STOP_PTS_EVENT_ID) + 1))						/** 0x14 **/

#define BDSP_RAAGA_RAMP_ENABLE_EVENT_ID    \
    ((uint32_t)((BDSP_RAAGA_ASTMTSM_PASS_EVENT_ID) + 1))						/** 0x15 **/

#define BDSP_RAAGA_CDB_DATA_AVAIL_EVENT_ID    \
    ((uint32_t)((BDSP_RAAGA_RAMP_ENABLE_EVENT_ID) + 1))						/** 0x16 **/

#define BDSP_RAAGA_STREAM_INFO_AVAIL_EVENT_ID \
    ((uint32_t)((BDSP_RAAGA_CDB_DATA_AVAIL_EVENT_ID) + 1))						/** 0x17 **/

#define BDSP_RAAGA_UNLICENSED_ALGO_EVENT_ID	\
	((uint32_t)((BDSP_RAAGA_STREAM_INFO_AVAIL_EVENT_ID) + 1))					/** 0x18 **/

#define BDSP_RAAGA_ENCODER_OVERFLOW_EVENT_ID \
    ((uint32_t)((BDSP_RAAGA_UNLICENSED_ALGO_EVENT_ID) + 1))                   /** 0x19 **/

#define BDSP_RAAGA_ANCDATA_EVENT_ID \
    ((uint32_t)((BDSP_RAAGA_ENCODER_OVERFLOW_EVENT_ID) + 1))                   /** 0x1A **/	

#define BDSP_RAAGA_CHANGE_IN_DIALNORM_EVENT_ID \
    ((uint32_t)((BDSP_RAAGA_ANCDATA_EVENT_ID) + 1))                             /** 0x1B **/

#ifdef RAAGA_SCM_SUPPORT
#define BDSP_RAAGA_BSP_SCM_RESPONSE_EVENT_ID  \
    ((uint32_t)((BDSP_RAAGA_CHANGE_IN_DIALNORM_EVENT_ID) + 1))                  /** 0x1C**/

#define BDSP_RAAGA_LAST_EVENT_ID \
    ((uint32_t)BDSP_RAAGA_BSP_SCM_RESPONSE_EVENT_ID)                                                    /** 0x1C **/

#define BDSP_RAAGA_MAX_ASYNC_EVENT   \
        ((uint32_t)(BDSP_RAAGA_LAST_EVENT_ID - BDSP_RAAGA_ASYNC_EVENT_ID_OFFSET))       /** 0x1C**/
#else
#define BDSP_RAAGA_LAST_EVENT_ID \
    ((uint32_t)BDSP_RAAGA_CHANGE_IN_DIALNORM_EVENT_ID)                                      /** 0x1B **/

#define BDSP_RAAGA_MAX_ASYNC_EVENT   \
        ((uint32_t)(BDSP_RAAGA_LAST_EVENT_ID - BDSP_RAAGA_ASYNC_EVENT_ID_OFFSET))       /** 0x1B **/
#endif






/*** These are unique Command Ids used by Host ***/

#define BDSP_RAAGA_START_TASK_COMMAND_ID   \
        ((uint32_t)((BDSP_RAAGA_COMMAND_ID_OFFSET) + 1))       /** 0x101 **/

#define BDSP_RAAGA_STOP_TASK_COMMAND_ID    \
        ((uint32_t)((BDSP_RAAGA_START_TASK_COMMAND_ID) + 1))   /** 0x102 **/

#define BDSP_RAAGA_ALGO_PARAMS_CFG_COMMAND_ID  \
        ((uint32_t)((BDSP_RAAGA_STOP_TASK_COMMAND_ID) + 1))    /** 0x103 **/

#define BDSP_RAAGA_PAUSE_COMMAND_ID        \
        ((uint32_t)((BDSP_RAAGA_ALGO_PARAMS_CFG_COMMAND_ID) + 1))  /** 0x104 **/

#define BDSP_RAAGA_FRAME_ADVANCE_COMMAND_ID        \
        ((uint32_t)((BDSP_RAAGA_PAUSE_COMMAND_ID) + 1))            /** 0x105 **/

#define BDSP_RAAGA_RESUME_COMMAND_ID       \
        ((uint32_t)((BDSP_RAAGA_FRAME_ADVANCE_COMMAND_ID) + 1))    /** 0x106 **/

#define BDSP_RAAGA_EVENT_NOTIFICATION_COMMAND_ID       \
        ((uint32_t)((BDSP_RAAGA_RESUME_COMMAND_ID ) + 1))          /** 0x107 **/

#define BDSP_RAAGA_FMMPORT_RECFG_COMMAND_ID       \
        ((uint32_t)((BDSP_RAAGA_EVENT_NOTIFICATION_COMMAND_ID ) + 1))		/** 0x108 **/
		
#define BDSP_RAAGA_PING_COMMAND_ID       \
        ((uint32_t)((BDSP_RAAGA_FMMPORT_RECFG_COMMAND_ID ) + 1))    /** 0x109 **/

#define BDSP_RAAGA_GET_VOM_TABLE_COMMAND_ID       \
        ((uint32_t)((BDSP_RAAGA_PING_COMMAND_ID ) + 1))    /** 0x10A **/
		
#define BDSP_RAAGA_NUM_PIC_TO_DROP_COMMAND_ID       \
        ((uint32_t)((BDSP_RAAGA_GET_VOM_TABLE_COMMAND_ID ) + 1))	/** 0x10B **/

#define BDSP_RAAGA_RECONFIGURATION_COMMAND_ID       \
        ((uint32_t)((BDSP_RAAGA_NUM_PIC_TO_DROP_COMMAND_ID ) + 1))		/** 0x10C **/
#define BDSP_RAAGA_GET_SYSTEM_SWAP_MEMORY_COMMAND_ID       \
        ((uint32_t)((BDSP_RAAGA_RECONFIGURATION_COMMAND_ID ) + 1))      /** 0x10D **/
/* Support to handle audio gaps in NRT xcode case */
#define BDSP_RAAGA_AUDIO_GAP_FILL_COMMAND_ID       \
        ((uint32_t)((BDSP_RAAGA_GET_SYSTEM_SWAP_MEMORY_COMMAND_ID ) + 1))      /** 0x10E **/


#ifdef RAAGA_SCM_SUPPORT
#define BDSP_RAAGA_BSP_SCM_COMMAND_ID       \
        ((uint32_t)((BDSP_RAAGA_AUDIO_GAP_FILL_COMMAND_ID ) + 1))     /** 0x10F **/
#endif
/*** Following are the Ack ids for different commands ***/

#define BDSP_RAAGA_START_TASK_ACK_ID   \
        ((uint32_t)((BDSP_RAAGA_ACK_ID_OFFSET) + 1))               /** 0x201 **/

#define BDSP_RAAGA_STOP_TASK_ACK_ID        \
        ((uint32_t)((BDSP_RAAGA_START_TASK_ACK_ID) + 1))           /** 0x202 **/

#define BDSP_RAAGA_ALGO_PARAMS_CFG_COMMAND_ACK_ID            \
        ((uint32_t)((BDSP_RAAGA_STOP_TASK_ACK_ID) + 1))            /** 0x203 **/

#define BDSP_RAAGA_PAUSE_ACK_ID            \
        ((uint32_t)((BDSP_RAAGA_ALGO_PARAMS_CFG_COMMAND_ACK_ID) + 1))    /** 0x204 **/

#define BDSP_RAAGA_FRAME_ADVANCE_ACK_ID    \
        ((uint32_t)((BDSP_RAAGA_PAUSE_ACK_ID ) + 1))               /** 0x205 **/

#define BDSP_RAAGA_RESUME_ACK_ID           \
        ((uint32_t)((BDSP_RAAGA_FRAME_ADVANCE_ACK_ID) + 1))        /** 0x206 **/

#define BDSP_RAAGA_EVENT_NOTIFICATION_ACK_ID       \
        ((uint32_t)((BDSP_RAAGA_RESUME_ACK_ID ) + 1))              /** 0x207 **/

#define BDSP_RAAGA_FMMPORT_RECFG_ACK_ID       \
        ((uint32_t)((BDSP_RAAGA_EVENT_NOTIFICATION_ACK_ID ) + 1))		/** 0x208 **/
		
#define BDSP_RAAGA_PING_ACK_ID       \
        ((uint32_t)((BDSP_RAAGA_FMMPORT_RECFG_ACK_ID ) + 1))  /** 0x209 **/
		
/* Support for Video Picture Drop */
#define BDSP_RAAGA_NUM_PIC_TO_DROP_ACK_ID     \
        ((uint32_t)((BDSP_RAAGA_PING_ACK_ID ) + 1))    /** 0x20A **/

/* Support to handle audio gaps in NRT xcode case */
#define BDSP_RAAGA_AUDIO_GAP_FILL_ACK_ID       \
        ((uint32_t)((BDSP_RAAGA_NUM_PIC_TO_DROP_ACK_ID ) + 1))      /** 0x20B **/

#ifdef RAAGA_SCM_SUPPORT
#define BDSP_RAAGA_BSP_SCM_COMMAND_ACK_ID     \
		 ((uint32_t)((BDSP_RAAGA_AUDIO_GAP_FILL_ACK_ID ) + 1))    /** 0x20C **/
#endif
 /*** The following are the various Response Ids used for different commands  ***/

#define BDSP_RAAGA_START_TASK_RESPONSE_ID   \
        ((uint32_t)((BDSP_RAAGA_RESPONSE_ID_OFFSET) + 1))          /** 0x301 **/

#define BDSP_RAAGA_STOP_TASK_RESPONSE_ID        \
        ((uint32_t)((BDSP_RAAGA_START_TASK_RESPONSE_ID) + 1))           /** 0x302 **/

#define BDSP_RAAGA_ALGO_PARAMS_CFG_RESPONSE_ID            \
        ((uint32_t)((BDSP_RAAGA_STOP_TASK_RESPONSE_ID) + 1))            /** 0x303 **/

#define BDSP_RAAGA_PAUSE_RESPONSE_ID            \
        ((uint32_t)((BDSP_RAAGA_ALGO_PARAMS_CFG_RESPONSE_ID) + 1))    /** 0x304 **/

#define BDSP_RAAGA_FRAME_ADVANCE_RESPONSE_ID    \
        ((uint32_t)((BDSP_RAAGA_PAUSE_RESPONSE_ID ) + 1))               /** 0x305 **/

#define BDSP_RAAGA_RESUME_RESPONSE_ID           \
        ((uint32_t)((BDSP_RAAGA_FRAME_ADVANCE_RESPONSE_ID) + 1))        /** 0x306 **/

#define BDSP_RAAGA_EVENT_NOTIFICATION_RESPONSE_ID       \
        ((uint32_t)((BDSP_RAAGA_RESUME_RESPONSE_ID ) + 1))				/** 0x307 **/
		
#define BDSP_RAAGA_FMMPORT_RECFG_RESPONSE_ID       \
        ((uint32_t)((BDSP_RAAGA_EVENT_NOTIFICATION_RESPONSE_ID ) + 1))	/** 0x308 **/

#define BDSP_RAAGA_RECONFIGURATION_RESPONSE_ID       \
        ((uint32_t)((BDSP_RAAGA_FMMPORT_RECFG_RESPONSE_ID ) + 1))		/** 0x309 **/
#ifdef RAAGA_SCM_SUPPORT
#define BDSP_RAAGA_BSP_SCM_COMMAND_RESPONSE_ID       \
         ((uint32_t)((BDSP_RAAGA_RECONFIGURATION_RESPONSE_ID ) + 1))       /** 0x30A **/
#endif

/***************************************************************************
Summary: 
    Enum indicating Mask bit for Enabling/Disabling specific event for a task.

See Also:
****************************************************************************/

typedef enum BDSP_Raaga_P_EventIdMask
{
    BDSP_Raaga_P_EventIdMask_eNone                       = 0x00000000,
    BDSP_Raaga_P_EventIdMask_eDummyFrameGeneration		= 0x00000001, 
    BDSP_Raaga_P_EventIdMask_eFrameRepeat				= 0x00000002, 
    BDSP_Raaga_P_EventIdMask_eFrameDropFromCdb			= 0x00000004, 
    BDSP_Raaga_P_EventIdMask_eTsmFail					= 0x00000008, 
    BDSP_Raaga_P_EventIdMask_eCdbItbUnderflow			= 0x00000010, 
    BDSP_Raaga_P_EventIdMask_eCdbItbOverflow				= 0x00000020,
    BDSP_Raaga_P_EventIdMask_eCrcError					= 0x00000040, 
    BDSP_Raaga_P_EventIdMask_eAudioModeChange			= 0x00000080,
    BDSP_Raaga_P_EventIdMask_eBitRateChange				= 0x00000100, 
    BDSP_Raaga_P_EventIdMask_eSampleRateChange			= 0x00000200, 
    BDSP_Raaga_P_EventIdMask_eFrameSyncLock				= 0x00000400, 
    BDSP_Raaga_P_EventIdMask_eFrameSyncLockLost			= 0x00000800, 
    BDSP_Raaga_P_EventIdMask_eSTC_PTS_DiffLowerThreshold	= 0x00001000, 
    BDSP_Raaga_P_EventIdMask_eSTC_PTS_DiffUpperThreshold	= 0x00002000, 
    BDSP_Raaga_P_EventIdMask_eDesiredCDB_BufferLevel		= 0x00004000, 
    BDSP_Raaga_P_EventIdMask_eFirstPTS_Received			= 0x00008000, 
    BDSP_Raaga_P_EventIdMask_ePTS_error					= 0x00010000,
    BDSP_Raaga_P_EventIdMask_eTSM_Lock					= 0x00020000,
    BDSP_Raaga_P_EventIdMask_eStartOnPTS					= 0x00040000,
    BDSP_Raaga_P_EventIdMask_eStopOnPTS					= 0x00080000,
    BDSP_Raaga_P_EventIdMask_eAstmTsmPass				= 0x00100000,	
    BDSP_Raaga_P_EventIdMask_eRampEnable					= 0x00200000,		
    BDSP_Raaga_P_EventIdMask_eCDBDataAvail				= 0x00400000,
    BDSP_Raaga_P_EventIdMask_eStreamInfoAvail			= 0x00800000,
    BDSP_Raaga_P_EventIdMask_eUnlicensedAlgo				= 0x01000000,
    BDSP_Raaga_P_EventIdMask_eEncoderOverflow				= 0x02000000,
    BDSP_Raaga_P_EventIdMask_eAncData				    = 0x04000000,
    BDSP_Raaga_P_EventIdMask_eChangeInDialnorm          = 0x08000000,
#ifdef RAAGA_SCM_SUPPORT
        BDSP_Raaga_P_EventIdMask_eBspScmResponse                        = 0x10000000,
        BDSP_Raaga_P_EventIdMask_eLast,
        BDSP_Raaga_P_EventIdMask_eAll                                    = 0x1FFFFFFF,   /* Keep updating this All */
        BDSP_Raaga_P_EventIdMask_eInvalid                                       = 0x7FFFFFFF
#else
    BDSP_Raaga_P_EventIdMask_eLast,
    BDSP_Raaga_P_EventIdMask_eAll                        = 0x0FFFFFFF,   /* Keep updating this All */
    BDSP_Raaga_P_EventIdMask_eInvalid                                   = 0x7FFFFFFF
#endif
} BDSP_Raaga_P_EventIdMask;


/***************************************************************************
Summary: 
    Enum indicating the task type.

See Also:
****************************************************************************/

typedef enum BDSP_Raaga_P_TaskType
{
    BDSP_Raaga_P_TaskType_eNonRealtime = 0, 
    BDSP_Raaga_P_TaskType_eSoftRealtime,
    BDSP_Raaga_P_TaskType_eRealtime,
	
    BDSP_Raaga_P_TaskType_eLast,
    BDSP_Raaga_P_TaskType_eInvalid = 0x7FFFFFFF

} BDSP_Raaga_P_TaskType;


/***************************************************************************
Summary: 
    Enum indicating the Scheduling mode for the task.

See Also:
****************************************************************************/

typedef enum BDSP_Raaga_P_SchedulingMode
{
    BDSP_Raaga_P_SchedulingMode_eMaster = 0,
	BDSP_Raaga_P_SchedulingMode_eSlave,
	
    BDSP_Raaga_P_SchedulingMode_eLast,
    BDSP_Raaga_P_SchedulingMode_eInvalid = 0x7FFFFFFF

} BDSP_Raaga_P_SchedulingMode;



/***************************************************************************
Summary: 
    Enum indicating the type of the deadline computation function to be used
    for the task.
    
See Also:
****************************************************************************/

typedef enum BDSP_Raaga_P_DeadLineComputeFuncType
{
    BDSP_Raaga_P_DeadLineComputeFuncType_eRealtimeDecode = 0,
    BDSP_Raaga_P_DeadLineComputeFuncType_eRealtimeEncode,
    BDSP_Raaga_P_DeadLineComputeFuncType_eNonRealtimeDecode,
    BDSP_Raaga_P_DeadLineComputeFuncType_eNonRealtimeEncode,
    BDSP_Raaga_P_DeadLineComputeFuncType_ePassthrough,
#ifdef RAAGA_SCM_SUPPORT
    BDSP_Raaga_P_DeadLineComputeFuncType_eScmTask,
#endif    
    BDSP_Raaga_P_DeadLineComputeFuncType_eMax,
    BDSP_Raaga_P_DeadLineComputeFuncType_eInvalid = 0x7FFFFFFF

} BDSP_Raaga_P_DeadLineComputeFuncType;



/***************************************************************************
Summary: 
    Enum indicating the Algo type.

See Also:
****************************************************************************/

typedef enum BDSP_Raaga_P_AlgoType
{           
	BDSP_Raaga_P_AlgoType_eEncode = 10,
	BDSP_Raaga_P_AlgoType_eDecode,
	BDSP_Raaga_P_AlgoType_eTranscode,
	BDSP_Raaga_P_AlgoType_ePassThru,
	BDSP_Raaga_P_AlgoType_eVideoDecode,
	BDSP_Raaga_P_AlgoType_eVideoEncode,
#ifdef RAAGA_SCM_SUPPORT
	BDSP_Raaga_P_AlgoType_eScm,
#endif	
	BDSP_Raaga_P_AlgoType_eLast,
	BDSP_Raaga_P_AlgoType_eInvalid = 0x7FFFFFFF

} BDSP_Raaga_P_AlgoType;



/***************************************************************************
Summary: 
    Enum indicating whether Ack or Response required by Host

See Also:
****************************************************************************/

typedef enum BDSP_Raaga_P_ResponseType
{           
	BDSP_Raaga_P_ResponseType_eAckRequired = 0,
	BDSP_Raaga_P_ResponseType_eResponseRequired,
	BDSP_Raaga_P_ResponseType_eAckAndResponseBothRequired,
	BDSP_Raaga_P_ResponseType_eNone,
	BDSP_Raaga_P_ResponseType_eLast,
	BDSP_Raaga_P_ResponseType_eInvalid = 0x7FFFFFFF

} BDSP_Raaga_P_ResponseType;


/***************************************************************************
Summary: 
    Structure holding the common header for all commands between the host 
    and the DSP.
    
Description:
    This is the common header for all Commands from the host.
    
See Also:
****************************************************************************/

typedef struct BDSP_Raaga_P_CommandHeader
{   
    uint32_t    ui32CommandID;              /*  Command ID */   
    uint32_t    ui32CommandSizeInBytes;     /*  Command Size in Bytes */    
    uint32_t    ui32CommandCounter;         /*  Command Counter */
	uint32_t    ui32TaskID;                 /*  Task ID*/
	uint32_t	ui32ThreadCtrlBlkNum;
    BDSP_Raaga_P_ResponseType	eResponseType;	/*  Ack or response required */
} BDSP_Raaga_P_CommandHeader;

/***************************************************************************
Summary: 
    This structure holds all the information required by DSP for scheduling.

	 This structure is derived from the BDSP_Raaga_P_TaskParamInfo by ATM.
	 DSP uses this structure internally	
    
See Also:
****************************************************************************/

typedef union BDSP_Raaga_P_DspSchedulingBuffInfo
{
	uint32_t							ui32DramSchedulingBuffCfgAddr; /*	Handle for the Scheduling Buffer.
																			Used only for BDSP_AF_P_BufferType_eDRAM*/

	BDSP_AF_P_sDRAM_CIRCULAR_BUFFER		sRdbBasedSchedulingBuffer;	   /*	Used only for buffer of type FMM/CDB/RDB */

}BDSP_Raaga_P_DspSchedulingBuffInfo;

typedef struct BDSP_Raaga_P_NonRTTaskParamInfo
{
    uint32_t    ui32MinTimeThreshold;
    uint32_t    ui32MaxTimeSlice;
}BDSP_Raaga_P_NonRTTaskParamInfo;

/***************************************************************************
Summary: 
    This structure holds all the information required by DSP for scheduling 
	the newly created task by Host
    
See Also:
****************************************************************************/

typedef struct BDSP_Raaga_P_TaskParamInfo
{
   uint32_t							ui32SamplingFrequency;
   uint32_t							ui32FrameSize;
   uint32_t							ui32SoftRealTimeThreshold;
   uint32_t							ui32Threshold;
   uint32_t							ui32BlockTime;
   uint32_t							ui32NonRealTimeRate;

	/*  Following Circ buffer details are used for task scheduling by FW CPT, 
	also this is used by TSM for Buffer delay computation.

	PI will populates the 'sRdbBasedSchedulingBuffer' for RDB based buffers.

	Incase of DRAM intertask buffer as the scheduling buffer,
	PI will take Buffer Handle of the intertask Dram buffer and hook to ui32DramSchedulingBuffCfgAddr
	*/
   
	BDSP_AF_P_BufferType			    eBufferType;
	BDSP_Raaga_P_DspSchedulingBuffInfo   sDspSchedulingBuffInfo;

	/* 
	Feedback buffer from the Master Task to the Slave Tasks.

	Currently we support only one feedback buffer from the Master Task to all its slave tasks.

	The concept of Feedback path exists only in the Master Slave configuration and the feed back path
	becomes invalid in the single decode usage modes. PI has to make Valid/Invalid field according.

	PI allocates the Feedback buffer when a master task is started and allocates a DRAM memory of 
	type BDSP_AF_P_sSINGLE_CIRC_BUFFER to populate the circullar buffer details.  PI passes the base address 
	of this DRAM memory to FW as Handle so that FW can take care of all kinds of buffers including 
	the BDSP_AF_P_BufferType_eDRAM.	  

	PI gives the same handle for all the slave tasks of a master task..	  

	The 'ui32MasterTaskFeedbackBuffCfgAddr' is the converted address...

	*/ 
   
   uint32_t							ui32MasterTaskFeedbackBuffValid;	/* Valid =1; Invalid 0 */
   uint32_t							ui32MasterTaskFeedbackBuffCfgAddr;

   uint32_t							ui32MaxIndepDelay ;
   uint32_t							ui32IndepDelay ;
   BDSP_Raaga_P_NonRTTaskParamInfo   sNonRealTimeTaskParams;

	
   BDSP_AF_P_FmmDstFsRate			eFmmDstFsRate; /* The sinking rate of Scheduling buffer.
													  The sinking buffer should be selected as the fastest sinking buffer in the task.
													  This will ensure the jitter in 4X rate FMM buffers getting accounted for scheduling */
}BDSP_Raaga_P_TaskParamInfo;



/***************************************************************************
Summary: 
    Start task command structure.
    
Description:
    This is the command structure of the start task command to be issued by
    the host on start decode.
    
See Also:
****************************************************************************/

typedef struct BDSP_Raaga_P_StartTaskCommand
{
    BDSP_Raaga_P_AlgoType				eTaskAlgoType; /*  Algorithm type enum */
    BDSP_Raaga_P_TaskType                eTaskType;          /*  Task type:Real Time, 
                                                                        Non real time */
    BDSP_Raaga_P_SchedulingMode         eSchedulingMode;
	uint32_t							ui32MasterTaskId;
	
    BDSP_Raaga_P_DeadLineComputeFuncType eDeadlineComputationFuncType;
    uint32_t							ui32DramDeadlineConfigStructAddr; /* This is pointer to this
																			structure BDSP_Raaga_P_TaskParamInfo
																			in DRAM */
    uint32_t							ui32DramTaskConfigAddr;	/* Task config data structure address
																			from CIT module */
    uint32_t							ui32SyncQueueId;		/* Task Sync queue Id */
    uint32_t							ui32AsyncQueueId;		/* Task Async queue Id */
	BDSP_AF_P_sDRAM_BUFFER				sDramStackBuffer;		/* Stack DRAM address for stack swapping for each task */
	uint32_t							ui32EventEnableMask;

	BDSP_AF_P_EnableDisable					ePPMCorrEnable;  /* PPM correction enable or not */
    BDSP_AF_P_EnableDisable             eOpenGateAtStart;  /* Open gate at start task, do not wait for the first valid frame
                                                              to be found before starting kick starting the FMM data consumption */
} BDSP_Raaga_P_StartTaskCommand;

typedef struct BDSP_Raaga_P_StopTaskCommand
{
	BDSP_Raaga_P_TaskType	eTaskType;			/* Task Type*/
    BDSP_Raaga_P_SchedulingMode         eSchedulingMode;
	uint32_t	ui32MasterTaskId;
}BDSP_Raaga_P_StopTaskCommand;


/***************************************************************************
Summary:
    Generic configuration change command structure.
    
Description:
    Any configuration change command will have a generic structure. The
    configuration change will only have three parameters. The DRAM address
    location where the host has changed the configuration. The DRAM location
    currently being used by the DSP and the size of the configuration 
    structure.
    
See Also:
****************************************************************************/

typedef struct BDSP_Raaga_P_CfgChangeCommand
{
    uint32_t                    ui32HostConfigParamBuffAddr;
    uint32_t                    ui32DspConfigParamBuffAddr;
    uint32_t                    ui32SizeInBytes;

} BDSP_Raaga_P_CfgChangeCommand;


/***************************************************************************
Summary:
     Frame Advance command structure.
    
Description:
        
See Also:
****************************************************************************/

typedef struct BDSP_Raaga_P_FrameAdvanceCommand
{
    BDSP_TIME_45KHZ_TICKS		ui32DurationOfFrameAdv; /*  Duration for the frame 
                                                            advance */
} BDSP_Raaga_P_FrameAdvanceCommand;


/***************************************************************************
Summary:
     Event enable/disable command structure.
    
Description:
        
See Also:
****************************************************************************/

typedef struct  BDSP_Raaga_P_EventEnableDisable
{
    uint32_t ui32EnableEvent; /* Use  BDSP_Raaga_P_EventIdMask for enabling or
								 disabling specfic events by Host, We need to 
								 take care of starting 0x301 offset for event Ids */
} BDSP_Raaga_P_EventEnableDisable;


/***************************************************************************
Summary:
     Ping command structure.
    
Description:
        
See Also:
****************************************************************************/

typedef struct  BDSP_Raaga_P_PingCommand
{
    uint32_t    ui32AckQueueId; /* Ack queue Id for ping command */
} BDSP_Raaga_P_PingCommand;

/***************************************************************************
Summary:
    Fmm-Port Reconfig Command configuration change command structure.
    
Description:
		
	This structure contain two DRAM Addresses.
		a) ui32NewFMMPortBuffAdd :	Buffer that contains new FMM Buffer
									port cfg.
									This buffer is a temporary buffer 
									allocated in PI.

		b) ui32TaskFMMPortCfgBuffAddr : The task FMM port cfg addr that was
									allocated by CIT.

  
	Upon the FMM port Reconfiguration command, the FW is expected to copy
	the data in ui32NewFMMPortBuffAdd to ui32TaskFMMPortCfgBuffAddr and 
	perform the FMM port Reconfiguration.
	
	FW will send a response once the Command operations are done....
    
See Also:
****************************************************************************/

typedef struct BDSP_Raaga_P_FmmPortReconfigCommand     
{	
    uint32_t                    ui32NewFMMPortBuffAdd;
    uint32_t                    ui32TaskFMMPortCfgBuffAddr;
    uint32_t                    ui32SizeInBytes;

} BDSP_Raaga_P_FmmPortReconfigCommand;

/***************************************************************************
Summary:
    Cit Reconfiguration Command    

Description:
	This structure contain two DRAM Addresses 
		a) ui32ModifiedCitAddr	 :Buffer that contains new CIT data structure
								  This buffer is a temporary buffer 
								  allocated in PI.

		b) ui32RunningTaskCitAddr: The CIT data structure of the running task

		All these addresses should be Converted Address.

		and

		c) CIT Datastructure Size
    
See Also:
****************************************************************************/

typedef struct BDSP_Raaga_P_CitReconfigCommand
{
    uint32_t                    ui32ModifiedCitAddr;
    uint32_t                    ui32RunningTaskCitAddr;
    uint32_t                    ui32SizeInBytes;

} BDSP_Raaga_P_CitReconfigCommand;


/***************************************************************************
Summary:
     Get VOM table command structure.
    
Description:
        
See Also:
****************************************************************************/

typedef struct  BDSP_Raaga_P_GetVomTableCommand
{
    uint32_t                    ui32HostVomTableAddr;		/*Address of VOM table in the host*/
    uint32_t                    ui32NumEntries;				/*Number of valid entries in the table*/
} BDSP_Raaga_P_GetVomTableCommand;

/***************************************************************************
Summary:
     Get System swap memory address structure.

Description:

See Also:
****************************************************************************/

typedef struct  BDSP_Raaga_P_GetSystemSwapMemCommand
{
    uint32_t                    ui32SystemSwapDramMemAddr;        /* System Swap memory in Dram Address */
} BDSP_Raaga_P_GetSystemSwapMemCommand;

/***************************************************************************
Summary: 
    Number of picture to drop at decoder command structure.   

Description:

    This is the command structure of the NUM_PIC_TO_DROP command to be 
    issued by the host to video decoder to update the decoder whenever DM 
    increments it's picture drop counter.   

See Also:
****************************************************************************/
typedef struct BDSP_Raaga_P_NumPicToDropCommand
{
    uint32_t      ui32NumPicToDrop;      

} BDSP_Raaga_P_NumPicToDropCommand;

#ifdef RAAGA_SCM_SUPPORT

/***************************************************************************
Summary:
    BSP Command: Send SCM Commands to DSP

Description:

    Host sends this command structure to DSP for SCM operations.

****************************************************************************/

typedef struct BDSP_Raaga_P_SCM_CmdPayload
{
    uint32_t ui32DramCmdBufAddress;
    uint32_t ui32DramCmdBufSize;
    uint32_t ui32DramRespBufAddress;
    uint32_t ui32DramRespBufSize;

}BDSP_Raaga_P_SCM_CmdPayload;


typedef struct BDSP_Raaga_P_SCM_CmdOperation
{
    BDSP_Raaga_P_SCM_CmdPayload  sCmdBufferInfo;
}BDSP_Raaga_P_SCM_CmdOperation;
#endif

/***************************************************************************
Summary:
     Common structure for all firmware commands
        
Description: Currently, it is assumed that all the commands posted by Host will be
			 of fixed size, which is sizeof(BDSP_Raaga_P_Command) data structure. 
			 It will be easier in future if we need to skip specific commands 
			 from the queue, which can be easily achieved by moving with a 
			 fixed size offset for each command in the queue.
See Also:
****************************************************************************/

typedef struct BDSP_Raaga_P_Command
{
    BDSP_Raaga_P_CommandHeader   sCommandHeader;
    union
    {
        BDSP_Raaga_P_StartTaskCommand    	sStartTask;
        BDSP_Raaga_P_StopTaskCommand		sStopTask;
        BDSP_Raaga_P_CfgChangeCommand    	sCfgChange;
        BDSP_Raaga_P_FrameAdvanceCommand 	sFrameAdvance;
        BDSP_Raaga_P_EventEnableDisable  	sEnableDisableEvent;
        BDSP_Raaga_P_FmmPortReconfigCommand	sFmmPortReconfig;
        BDSP_Raaga_P_PingCommand         	sPing;
        BDSP_Raaga_P_GetVomTableCommand		sGetVomTable;
 		BDSP_Raaga_P_NumPicToDropCommand	sNumPicToDropCommand;
		BDSP_Raaga_P_CitReconfigCommand		sCitReconfigCommand;
		BDSP_Raaga_P_GetSystemSwapMemCommand    sSystemSwapMemCommand;
#ifdef RAAGA_SCM_SUPPORT
		BDSP_Raaga_P_SCM_CmdOperation       sScmCmd;
#endif		
    } uCommand;
} BDSP_Raaga_P_Command;


/***************************************************************************
Summary: 
    This structure holds common header for both Ack and responses sent by 
	the DSP to the host in synchronous queue. 
	Only header is sufficient for sending any specifc Acks to the Host.
    
Description:
    This is the common header for Ack and Response messages sent by DSP;
    
See Also:
****************************************************************************/
typedef struct BDSP_Raaga_P_CommonAckResponseHeader
{
    uint32_t                    ui32ResponseID;             /*  Response ID */  
    uint32_t                    ui32ResponseSizeInBytes;    /*  Response Size in Bytes */   
    uint32_t                    ui32ResponseCounter;        /*  Response Counter */ 
    uint32_t                    ui32TaskID;                 /*  Task ID */  
    uint32_t                    eStatus;                    /*  Status of completion */ 
    uint32_t					ui32ThreadCtrlBlkNum;
} BDSP_Raaga_P_CommonAckResponseHeader;

/***************************************************************************
Summary:
     Frame Advance Response structure.
    
Description:
        
See Also:
****************************************************************************/

typedef struct BDSP_Raaga_P_FrameAdvanceResponse
{
    BDSP_TIME_45KHZ_TICKS	ui32DurationOfFrmAdvCompleted;  /*  Duration for the frame 
                                                                    advance completed */
} BDSP_Raaga_P_FrameAdvanceResponse;



/***************************************************************************
Summary:
     Structure for Ack/Response message in sync queue
    
Description: Currently, it is assumed that all the sync responses posted by DSP will 
			 be of fixed size, which is sizeof(BDSP_Raaga_P_Response) data structure. 
			 It will be easier in future if Host needs to skip specific response 
			 message	in the sync queue for processing, which can be easily achieved 
			 by moving with a fixed size offset for each response in the sync queue.
        
			Only, header needs to be posted by DSP for any Ack to the Host.
See Also:
****************************************************************************/

typedef struct BDSP_Raaga_P_Response
{
    BDSP_Raaga_P_CommonAckResponseHeader     sCommonAckResponseHeader;
    union
    {
        BDSP_Raaga_P_FrameAdvanceResponse    sFrameAdvance;
    } uResponse;
} BDSP_Raaga_P_Response;


/***************************************************************************
Summary:
     Structure for async message header for any event
    
Description:
        
See Also:
****************************************************************************/

typedef struct BDSP_Raaga_P_AsynMSGHeader
{   
    uint32_t    ui32EventID;            	/* Event ID */   
    uint32_t    ui32MsgSize;           		/* Size of Message in bytes */   
    uint32_t    ui32TaskID;             	/* Task ID for which the message occurs */   
}BDSP_Raaga_P_AsynMSGHeader;

/***************************************************************************
Summary:
     Structure for getting the TSM buffer ID info from FW
Description:
        
See Also:
****************************************************************************/
typedef struct BDSP_DSPCN_P_TsmFwInfo
{
	uint32_t	uiBufId;	
}BDSP_DSPCN_P_TsmFwInfo;

/***************************************************************************
Summary:
     Structure for getting the Sample rate info from FW

	There are three parts to the payload
	        1) Base Rate : For Configuring the SRC blocks
	        2) Stream Sampling Rate : For Configuring the outputs at native
	                                  sample rate. Ex: DD convert outputs
	        3) HDMI Sampling Rate : Configuring the HDMI block
	            - Required only for RAP PI; not required for APE
Description:
        
See Also:
****************************************************************************/
typedef struct BDSP_DSPCHN_P_FsToHost
{
	uint32_t      ui32SamplingRate;   
	uint32_t      ui32ValidFlag;       /* 1: valid and 0: Invalid */
	
} BDSP_DSPCHN_P_FsToHost;

typedef struct BDSP_DSPCHN_P_FwSampleinfo
{
    BDSP_DSPCHN_P_FsToHost        sBaseSamplingRate;
    BDSP_DSPCHN_P_FsToHost        sHdmiSamplingRate;
    BDSP_DSPCHN_P_FsToHost        sStreamSamplingRate;
	
}BDSP_DSPCHN_P_FwSampleinfo;

/***************************************************************************
Summary:
     Structure for getting the Bit rate change info from FW
Description:
        
See Also:
****************************************************************************/
typedef struct BDSP_DSPCHN_P_FwBitRateChangeInfo
{
    uint32_t	ui32BitRate;    /* New Bit Rate value*/
    uint32_t 	ui32BitRateIndex;    /* This has the Bit rate index
    				as given in the standard. This value
    				is zero for audio type AAC-HE*/
}BDSP_DSPCHN_P_FwBitRateChangeInfo;

/***************************************************************************
Summary:
     Structure for getting the mode change info from FW
Description:

See Also:
****************************************************************************/
typedef struct BDSP_DSPCHN_P_FwModeChnageInfo
{
	uint32_t	ui32ModeValue;	
} BDSP_DSPCHN_P_FwModeChangeInfo;


/***************************************************************************
Summary:
     Structure for ramp enable event

Description: 
See Also:
****************************************************************************/
typedef struct BDSP_DSPCHN_RampEnableInfo
{
 uint32_t    ui32TimeDelay;   /* PI should enable the ramp after ui32TimeDelay(ms) */
 
} BDSP_DSPCHN_RampEnableInfo;

/***************************************************************************
Summary:
     Structure for unlicensed Algo event

Description: 
See Also:
****************************************************************************/
typedef struct BDSP_Raaga_P_UnsupportedAlgoInfo
{	
	uint32_t		ui32AudioAlgorithm; /* Algorithm Name */

} BDSP_Raaga_P_UnsupportedAlgoInfo;


/***************************************************************************
Summary:
     Structure for Async message events

Description: Currently, it is assumed that all the async message events posted by 
			 DSP will be of fixed size, which is sizeof(BDSP_Raaga_P_AsynEventMsg) data 
			 structure. It will be easier in future if Host needs to skip specific event
			 message from async queue for processing, which can be easily achieved by 
			 moving with a fixed size offset for each async event message in the queue.
See Also:
****************************************************************************/
/*TODO: UNify BDSP_Raaga_P_PtsInfo and BDSP_AudioTaskTsmStatus to single structure.*/
typedef struct BDSP_Raaga_P_PtsInfo
{
    uint32_t    ui32RunningPts;   /* The PTS value of the frame going 
          out the next vsync 
          MPEG/DirecTv HD - 45KHz domain
          DirecTv SD- 27MHz domain */
    int32_t           i32Pts2StcPhase;    /* Difference between current PTS 
          and the STC */
    int32_t         i32TSMUpperThreshold;      /*TSM Upper threshold*/                                       
    BDSP_PtsType  ePtsType;   /* The PTS type tag */ 

	uint32_t		ui32NumContinuousFail;		/* Fail count from TSM */


} BDSP_Raaga_P_PtsInfo;

#ifdef RAAGA_SCM_SUPPORT

/***************************************************************************
Summary:
     This structure is used for Response from DSP for SCM operations

Description:
1 - Authentication failure


****************************************************************************/

typedef enum BDSP_Raaga_P_BspScmResponseType
{
    BDSP_Raaga_P_BspScmResponseType_Algorithm_Authentication_Failure = 0x1,
    BDSP_Raaga_P_BspScmResponseType_eLast,

    BDSP_Raaga_P_BspScmResponseType_eINVALID             =   0x7FFFFFFF

} BDSP_Raaga_P_BspScmResponseType;

/***************************************************************************
Summary:
    This structure is used for Response from DSP for SCM operations

Description:

See Also: BDSP_Raaga_P_BspScmResponseType

****************************************************************************/

typedef struct BDSP_Raaga_P_BspScmResponseInfo
{
    BDSP_Raaga_P_BspScmResponseType     eBspScmResponseType;

}BDSP_Raaga_P_BspScmResponseInfo;

#endif


typedef struct BDSP_Raaga_P_AsynEventMsg
{
    BDSP_Raaga_P_AsynMSGHeader  sMsgHeader;
    union 
    {
        BDSP_Raaga_P_PtsInfo  			    sPtsInfo;
        BDSP_DSPCN_P_TsmFwInfo 			    sTsmInfo;
        BDSP_DSPCHN_P_FwSampleinfo 		    sFwSampleInfo;
        BDSP_DSPCHN_P_FwModeChangeInfo 	    sFwModeChange;
        BDSP_DSPCHN_P_FwBitRateChangeInfo	sFwBitRateChange;
        BDSP_DSPCHN_RampEnableInfo			sRampEnableInfo;
        BDSP_Raaga_P_UnsupportedAlgoInfo		sUnsupportedAlgoInfo;
#ifdef RAAGA_SCM_SUPPORT
	BDSP_Raaga_P_BspScmResponseInfo     sBspScmResponseInfo;
#endif
    }uFWMessageInfo;
}BDSP_Raaga_P_AsynEventMsg;


/* This define return the fixed size value for any command 
   This SIZEOF() macro return the value in bytes */
#define BDSP_RAAGA_P_COMMAND_SIZE_IN_BYTES					SIZEOF(BDSP_Raaga_P_Command)

/* This define return the fixed size value for any Ack */
#define BDSP_RAAGA_ACK_SIZE_IN_BYTES						SIZEOF(BDSP_Raaga_P_Response)

/* This define return the fixed size value for any Response */
#define BDSP_RAAGA_RESPONSE_SIZE_IN_BYTES					SIZEOF(BDSP_Raaga_P_Response)

/* This define return the fixed size value for any Async Event */
#define BDSP_RAAGA_ASYNC_RESPONSE_SIZE_IN_BYTES				SIZEOF(BDSP_Raaga_P_AsynEventMsg)


#endif
