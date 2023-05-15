/***************************************************************************
*     Copyright (c) 2006-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brap_cap.c $
* $brcm_Revision: Hydra_Software_Devel/27 $
* $brcm_Date: 4/11/12 1:21p $
*
* Module Description:
*	This file contains the implementations of the APIs for a capture channel, 
*   which are exposed to the upper layer by the Raptor Audio PI. This file 
*   is part of Audio Manager Module. Audio Manager module is the top level 
*   module of the RAP PI, which interfaces with the caller and manages the 
*   internal channel objects to get RAP PI produce the desired result.
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/rap/base3/brap_cap.c $
* 
* Hydra_Software_Devel/27   4/11/12 1:21p purush
* SW7340-297: [7340]Moved path structure memory allocation from start
* channel to open channel. Porting the changes to Mainline.
* 
* Hydra_Software_Devel/26   4/20/11 3:42p sgadara
* SW7420-1756: [7420] Implementing code review comments --> Change the
* Pointer validity check function to a macro and name it aptly
* 
* Hydra_Software_Devel/25   12/15/10 5:14a gautamk
* SW7420-1202: [7420] Merging PM changes for RAP
* 
* Hydra_Software_Devel/SW7420-1202_t2/1   11/19/10 3:33p jtna
* SW7420-1202: [7420] disable/destroy all callbacks on Close
* 
* Hydra_Software_Devel/24   7/22/10 5:57p gautamk
* SW7400-2848: [7405] Adding support for on the fly input sample rate
* programming for PB channel.
* 
* Hydra_Software_Devel/23   6/22/10 6:11p gautamk
* SW7335-774: [7335] Moving the frequent dynamic Memory allocations of
* structures to BRAP_Open and store it in hRap. Using it from hRap
* whenever required.
* 
* Hydra_Software_Devel/22   6/16/10 2:20p gautamk
* SW7405-3063: [7405] Fixing compilation warning with DEBUG=n option
* 
* Hydra_Software_Devel/21   12/15/09 11:43a gautamk
* SW7405-3063: [7405] Checking Malloc against NULL, Because 0xDEADDATA
* and 0xDATADEAD can be valid address.
* 
* Hydra_Software_Devel/20   9/3/09 7:10p sgadara
* SW7405-2928: [7405] Correcting the register that has to be programmed
* to clear the fullmark interrupt.
* 
* Hydra_Software_Devel/19   9/2/09 8:22p sgadara
* SW7405-2928: [7405] Adding support for  Multichannel capture.
* --> Changing the interface as to use the old APIs
* 
* Hydra_Software_Devel/18   8/27/09 2:43p gautamk
* SW7405-2928: [7405] Adding support Multichannel capture.
* 
* Hydra_Software_Devel/17   8/25/09 1:06p gautamk
* SW7405-2928: [7405] Adding support Multichannel capture.
* 
* Hydra_Software_Devel/16   6/12/09 3:45p gautamk
* PR53302: [7405] Merging cleanup code to mainline
* 
* Hydra_Software_Devel/15   6/9/09 12:33p gautamk
* PR55572:[7405] Before accessing any pointer location ccheck if it is
* 0XDA7ADEAD or 0xDEADDA7A in addition to NULL.
* 
* Hydra_Software_Devel/14   2/23/09 11:54a gautamk
* PR52383: [7405] Correcting the call sequence of Enable/Disabel Callback
* of Int module as expected by this module
* 
* Hydra_Software_Devel/13   1/27/09 3:35p speter
* PR 50495: [3548,3556] Merging the customer branch to main line
* 
* Hydra_Software_Devel/12   12/10/08 10:58p gautamk
* PR49232:[7405] Adding support for Allocating Rbuf/SrcCh and Paths at
* OpenChannel time.
* 
* Hydra_Software_Devel/11   12/10/08 6:51a sgadara
* PR 44771: [3548,3556] Writing to status clear registers after the data
* in the ringbuffer is flushed. This is required to handle the overflow
* case in ringbuffer capture
* 
* Hydra_Software_Devel/RAP_354856_PH_4_1/2   1/21/09 4:00p speter
* PR 50581: [3548,3556] Fixing the watchdog recovery for capture
* channels.
* 
* Hydra_Software_Devel/RAP_354856_PH_4_1/1   12/15/08 12:10a sgadara
* PR 44771: [3548,3556] Writing to status clear registers after the data
* in the ringbuffer is flushed. This is required to handle the overflow
* case in ringbuffer capture
* 
* Hydra_Software_Devel/10   12/4/08 9:41a sgadara
* PR 44771: [3548,3556] Writing to status clear registers after the
* ringbuffer pointers are updated. This is required to handle the
* overflow case in ringbuffer capture where if status registers are not
* cleared, the interrupts trigger continously.
* 
* Hydra_Software_Devel/9   10/29/08 3:24a speter
* PR 44771: [3548,3556] Adding interrupt for Ring Buffer Overflow and
* cleaning up base3 code to remove 3563 and 7440 specific code
* 
* Hydra_Software_Devel/8   10/21/08 4:06a speter
* PR 47961: [3548,3556] Removing redundant APIs for RF Audio
* 
* Hydra_Software_Devel/7   9/22/08 10:39p sushmit
* PR 43454:[7405] Fixing CAP Watchdog Recovery
* 
* Hydra_Software_Devel/6   9/19/08 8:49p sgadara
* PR 44770: [3548,3556] Implementing the Destination based Interrupts for
* 'capture to ringbuffer' feature.
* 
* Hydra_Software_Devel/5   9/9/08 1:13p sushmit
* PR 46643:[7405] Fixing Close time seg fault.
* 
* Hydra_Software_Devel/4   9/5/08 4:30p sushmit
* PR 46588:[7405]Removing Path Alloc/Free during channel open/close time
* for PB & CAP.
* 
* Hydra_Software_Devel/3   8/18/08 9:09a speter
* PR45731: PR 45731: [3548,3556] Using toggle bits to determine if
* capture buffer is empty or full.
* 
* Hydra_Software_Devel/2   7/28/08 9:25a speter
* PR 45171: [3548,3556] Merging from Phase 2.0 release to main line of
* development (Hydra_Software_Devel)
* 
* Hydra_Software_Devel/RAP_354856_PH_2_0/1   7/24/08 10:19a speter
* PR 45032: [3548,3556] Add DSP support to capture channel (I2S In and
* ADC In)
* 
* Hydra_Software_Devel/1   7/21/08 2:19p speter
* PR 44984: [3548,3556,7405] Adding files from base2 which have new
* Architecture specific changes and cannot be merged back in base2
* 
* Hydra_Software_Devel/Rap_NewFwArch/9   7/3/08 11:34p gautamk
* PR42605: [7405] fixing coverity issues for 7405
* 
* Hydra_Software_Devel/Rap_NewFwArch/8   6/11/08 7:32a speter
* PR38950: [3548,3556] Changing generic use of BTSC as RF Audio
* 
* Hydra_Software_Devel/Rap_NewFwArch/7   5/29/08 10:04p gautamk
* PR41726: [7405] doing Memset after Malloc
* 
* Hydra_Software_Devel/Rap_NewFwArch/6   5/28/08 3:00a speter
* PR 39442: [3548,3556] Adding the Nicam and Pal A2 support
* 
* Hydra_Software_Devel/Rap_NewFwArch/5   5/28/08 2:44a speter
* PR 39442: [3548,3556] Adding the Nicam and Pal A2 support
* 
* Hydra_Software_Devel/Rap_NewFwArch/4   5/5/08 11:24a speter
* PR 38950: [3548] Making 3548_FAMILY macro and using it for both 3548
* and 3556 cases.
* 
* Hydra_Software_Devel/Rap_NewFwArch/3   4/28/08 4:35p speter
* PR 38950: [3548] Fixing compilation for 3548/3556.
* 
* Hydra_Software_Devel/Rap_NewFwArch/2   4/2/08 11:40a sushmit
* PR 34648: [7405] Moving capture to interrupts
* 
* Hydra_Software_Devel/Rap_NewFwArch/1   3/3/08 2:51p gautamk
* PR34648: [7405] Fixing coverity defects.
* 
* Hydra_Software_Devel/34   1/23/08 10:50a speter
* PR 28188: [3563] Fixing the Capture to File feature for I2S In
* 
* Hydra_Software_Devel/33   11/27/07 1:25p sushmit
* PR 36877: [7335] Getting rap_test compiled
* 
* Hydra_Software_Devel/32   10/26/07 12:28p sushmit
* PR 36003: [7405] Fixing compilation error due to last checkin
* 
* Hydra_Software_Devel/31   10/25/07 1:39a gautamk
* PR36409: [3563] Fixing Coverity issue for 3563 RAP Code base
* 
* Hydra_Software_Devel/30   10/22/07 12:19p sushmit
* PR 36047: [7405] Freeing Path Memory in ChannelClose
* 
* Hydra_Software_Devel/29   10/13/07 8:10p sushmit
* PR36047:[7405] Channel handle is freed two time, cause the kernel
* crash.
* 
* Hydra_Software_Devel/28   10/11/07 4:51p speter
* PR 28188: [3563] Modifications to support capturing to file in SPDIF Rx
* mode
* 
* Hydra_Software_Devel/27   10/11/07 4:30p sushmit
* PR 36003: [7405] Fixing Memory Leaks
* 
* Hydra_Software_Devel/26   9/12/07 4:10p speter
* PR 34823: [3563] The stack size of BRAP_CAP_P_ChannelOpen is reduced
* 
* Hydra_Software_Devel/25   7/13/07 6:26p nitinb
* PR 31453: Fixed compilation warnings on 7440.
* 
* Hydra_Software_Devel/24   7/9/07 2:13p sushmit
* PR 31453: Watchdog support for 3563
* 
* Hydra_Software_Devel/23   7/3/07 7:01p bmishra
* PR 23951: Merging the changes required for Autotrap
* 
* Hydra_Software_Devel/22   6/13/07 7:27p nitinb
* PR 32116: Provide an API to scale the input signal to BTSC decoder
* 
* Hydra_Software_Devel/21   5/28/07 1:33p bhanus
* PR 30069 : Fixed Coverity Issues.
* 
* Hydra_Software_Devel/20   5/23/07 5:51p nitinb
* PR 31498: Merge RAP PI development from phase 1.7 branch to main branch
* 
* Hydra_Software_Devel/RAP_3563_PH_1_7/1   5/15/07 4:47p nitinb
* PR 30747: Return errors from RAP APIs that are not supported when
* channel is not started
* 
* Hydra_Software_Devel/19   4/18/07 5:13p bhanus
* PR 28561 : Modified MixerSRCLayout for making it generic to be used by
* all paths.
* - Added code for sequential path implementation.
* 
* Hydra_Software_Devel/18   4/16/07 9:47a kagrawal
* PR 28561: Reduced channel handle size by changing sPath[] to *pPath[]
* 
* Hydra_Software_Devel/17   4/13/07 6:33p nitinb
* PR 25346: Added more BTSC code
* 
* Hydra_Software_Devel/16   4/10/07 2:57p bhanus
* PR 29381: Fixed the compilation break for 7440 introduced due to last
* check-in.
* 
* Hydra_Software_Devel/15   4/10/07 11:47a nitinb
* PR 29381: Malloc/Free large size structures instead of allocating on
* stack to prevent stack overflow
* 
* Hydra_Software_Devel/14   4/9/07 8:14p sushmit
* PR 29166:
* Volume Change: Left & Right volume level stored in hOp instead of
* hMixer. SetVolume added during Op_start.
* Mute Status Change: Mute status stored properly in hOp. SetMute added
* during Op_Start.
* Set/Get BTSC Mode: Includes open & run time BTSC mode & config changes.
* Added BtscParams in Inputconfig & CapportParams.
* Tone Control changes: Modified so that these can be changed even before
* start.
* 
* Hydra_Software_Devel/13   3/30/07 1:45p nitinb
* PR 28191: Added API to get BTSC envelope frequency amplitude
* 
* Hydra_Software_Devel/12   3/29/07 4:06p speter
* PR 28713: Adding the path information for DSP resource allocation
* 
* Hydra_Software_Devel/11   3/14/07 3:50p kagrawal
* PR 28561: Moved channel handle malloc from BRAP_XYZ_P_OpenChannel() to
* the common BRAP_OpenChannel()
* 
* Hydra_Software_Devel/10   2/9/07 6:19p bhanus
* PR 25607 : Fixed Compilation warnings
* 
* Hydra_Software_Devel/9   2/9/07 4:42p kagrawal
* PR 25607: Removed deprecated eAudioSource from BRAP_ChannelSettings
* structure
* 
* Hydra_Software_Devel/8   2/5/07 8:57p nitinb
* PR 25607 : Fixed 3563 compilation error
* 
* Hydra_Software_Devel/7   2/4/07 5:12p bhanus
* PR 25607 : Fixed compilation warnings.
* 
* Hydra_Software_Devel/6   12/29/06 1:48a nitinb
* PR 26766: Fixed compilation errors
* 
* Hydra_Software_Devel/5   12/29/06 1:08a nitinb
* PR 26766: 1. (3563-specific) Updated function BRAP_CAP_P_ChannelOpen to
*   allocate DSP/DSp-context for a capture channel.
* 2. (3563-specific) Updated function BRAP_CAP_P_ChannelClose to free
*   DSP/DSP-context resources
* 3. Updated BTSC related functions to call corresponding BTSC private
*   functions
* 
* Hydra_Software_Devel/4   11/27/06 12:13p sushmit
* PR 25607: Added code for CAP, XCD & ChSubTypeNone
* 
* Hydra_Software_Devel/3   11/9/06 6:41p bhanus
* PR 24717 : Replaced BTMR_TimerHandle with BTMR_Handle to fix
* compilation warning.
* 
* Hydra_Software_Devel/2   10/6/06 7:56p kagrawal
* PR 24717: Merged latest from the 7440 devel branch
* 
* Hydra_Software_Devel/7440_Devel_Branch/3   9/1/06 11:33a bhanus
* PR 20654: Fixed errors.
* 
* Hydra_Software_Devel/7440_Devel_Branch/2   8/31/06 4:14p bhanus
* PR 20654 : Fixed Compilation errors.
* 
* Hydra_Software_Devel/7440_Devel_Branch/1   8/31/06 2:09p bhanus
* PR 20654: Added API BRAP_CAP_GetBufInfo, BRAP_CAP_UpdateBufUsg,
* BRAP_CAP_P_PrepBufInfo_isr etc.
* 
* Hydra_Software_Devel/1   8/28/06 4:33p speter
* PR 20654: Adding capture related files

**************************************************************************/
#include "brap.h"
#include "brap_priv.h"	

BDBG_MODULE(rap_cap);		/* Register software module with debug interface */

static BERR_Code BRAP_PrepRingBufInfo_isr(
    BRAP_DestinationHandle          hDstHandle,
	unsigned int 		            uiRBufId,
	BRAP_RingBufInfo	            *pRBufInfo
);

static BERR_Code BRAP_WriteRingBufUsg_isr(
    BRAP_DestinationHandle          hDstHandle,
	unsigned int 		            uiRBufId,
	unsigned int		            uiSzUpdt
);

/***************************************************************************
Summary:
	Private API used to open a capture channel.
Description:
	It is used to instantiate a capture channel. It allocates channel handle 
	and resource required for the channel if any.
Returns:
	BERR_SUCCESS on success else error
See Also:
	BRAP_CAP_P_ChannelClose
****************************************************************************/
BERR_Code BRAP_CAP_P_ChannelOpen( 
    BRAP_Handle                     hRap,		    /* [in] Raptor Audio Device 
                                                       handle*/
    BRAP_ChannelHandle              hRapCh,		    /* [in] Raptor Capture 
                                                       Channel handle */
    const BRAP_ChannelSettings      *pChnSettings   /* [in] Channel settings*/ 
	)
{
	BERR_Code               err = BERR_SUCCESS;
	unsigned int            uiChannelNo = 0;
    bool                    bWdgRecovery = BRAP_P_GetWatchdogRecoveryFlag(hRap);
    unsigned int            uiPth = 0, i=0;

    BDBG_ENTER(BRAP_CAP_P_ChannelOpen);
    
	/* Validate input params */
	BDBG_ASSERT(hRap);
	BDBG_ASSERT(hRapCh);

	if(false == bWdgRecovery)
	{
    	/* Malloc large structures */
        /* pChnSettings is valid only when not in watchdog recovery */
        BDBG_ASSERT(pChnSettings);
        BDBG_MSG(("BRAP_CAP_P_ChannelOpen():"
                  "hRap=0x%x," 
                  "\n\t pChnSettings->eChType = %d" 
                  "\n\t pChnSettings->eChSubType = %d",
                  hRap, pChnSettings->eChType, pChnSettings->eChSubType));

        /* Validate if this channel can be added to hRap */
        for(uiChannelNo=0; uiChannelNo<BRAP_RM_P_MAX_CAP_CHANNELS; uiChannelNo++)
        {
        	if(!(BRAP_P_IsPointerValid((void *)hRap->hRapCapCh[uiChannelNo])))
        	{
        		break;
        	}
        }

        if(BRAP_RM_P_MAX_CAP_CHANNELS == uiChannelNo)
        {
        	BDBG_ERR(("Max number of Cap channels(%d) already exist", uiChannelNo));
        	err = BERR_TRACE(BERR_NOT_SUPPORTED);
        	goto end_open;
        }
        hRapCh->uiChannelNo = uiChannelNo;

#if (!((BRAP_7405_FAMILY == 1)||(BRAP_3548_FAMILY == 1)))
        /* Find a free pPath index to save granted resources */
        for(uiPth = 0; uiPth < BRAP_P_MAX_PATHS_IN_A_CHAN; uiPth++)
        {
            if(!(BRAP_P_IsPointerValid((void *)hRapCh->pPath[uiPth])))
                break;
        }
        if(uiPth >= BRAP_P_MAX_PATHS_IN_A_CHAN)
        {
            BDBG_ERR(("No free pPath available"));
			err = BERR_TRACE(err); goto error;
        }
        else
        {            
            BDBG_ASSERT(0 == uiPth); /* For Debug */
#ifdef BRAP_OPEN_TIME_PATH_ALLOCATION
            err = BRAP_P_MallocPath(hRapCh,uiPth,&(hRapCh->pPath[uiPth]));
#else
            err = BRAP_P_MallocPath(&(hRapCh->pPath[uiPth]));
#endif            
            if(BERR_SUCCESS != err)
            {
    			err = BERR_TRACE(err); goto error;
            }            
            for (i=0;i<BRAP_P_MAX_OUT_PARALLEL_FMM_PATH;i++)
            {
                BRAP_P_SET_USG_PATH(hRapCh->pPath[uiPth], 
                                BRAP_P_UsgPath_eDecodePcm,
                                BRAP_INVALID_VALUE,
                                uiPth,
                                BRAP_INVALID_VALUE,i);
            }
        }
#else
        BSTD_UNUSED(i);
        BSTD_UNUSED(uiPth);
#endif        

    }

    BSTD_UNUSED(pChnSettings);

#if (BRAP_3548_FAMILY == 1) || (BRAP_7405_FAMILY == 1)
	/* Intialise Raptor interrupt handling */
	err = BRAP_P_InterruptInstall (hRapCh);
	if(err != BERR_SUCCESS)
	{
		err = BERR_TRACE(err);
		BDBG_ERR(("InstallInterrupt()failed for RAP CAP Channel handle 0x%x", 
                hRapCh));
		if(true == bWdgRecovery)
		{
            goto end_open;
        }
		else
		{
    		goto end_open;
        }
    }
#endif       

    /* No errors, so clean exit */
  	goto end_open;
#if (!((BRAP_7405_FAMILY == 1)||(BRAP_3548_FAMILY == 1)))
error:
#endif
    /* TODO: */
end_open:
	
    /* only if channel has been successfully opened, save the handle */
    if((BERR_SUCCESS == err)&&(false == bWdgRecovery))
    {
        hRapCh->eState = BRAP_P_State_eOpened; /* Opened successfully */
        hRap->hRapCapCh[uiChannelNo] = hRapCh ;    
    }

	BDBG_LEAVE(BRAP_CAP_P_ChannelOpen);
	return err;
}

/***************************************************************************
Summary:
	API used to close a capture channel.
Description:
	It closes the instance of a capture channel operation. It frees the 
	channel handle and resources associated with it if any.
Returns:
	BERR_SUCCESS on success else error
See Also:
	BRAP_CAP_P_ChannelOpen
****************************************************************************/
BERR_Code BRAP_CAP_P_ChannelClose( 
	BRAP_ChannelHandle 	hRapCh	/* [in] The RAP Channel handle */
	)
{
    BERR_Code	    ret = BERR_SUCCESS;
    unsigned int    uiGrpId = 0;
    int             i = 0;
    unsigned int    uiChannelNo = 0;
    BRAP_Handle 	hRap = NULL;

	BDBG_ENTER(BRAP_CAP_P_CloseChannel);

	/* Validate input parameters. */
	BDBG_ASSERT(hRapCh);

    hRap = hRapCh->hRap;


#if (BRAP_3548_FAMILY == 1) || (BRAP_7405_FAMILY == 1)
	/* Mask interrupts and uninstall callbacks */
	BRAP_P_InterruptUnInstall(hRapCh);
#endif       

	/* We have associtated channel pairs and we need to check each of them to 
	   see if the channel exits there. If so we need to remove the channel from 
	   there */

	/* Find the group */
	for(uiGrpId=0; uiGrpId < BRAP_MAX_ASSOCIATED_GROUPS; uiGrpId++)
	{
        
        for(i=0; i<BRAP_MAX_CAP_CHAN_IN_ASSOCIATED_GRP; i++)
		{   
			if(hRap->sAssociatedCh[uiGrpId].hCapCh[i] == hRapCh)
			{
				hRap->sAssociatedCh[uiGrpId].hCapCh[i] = NULL;
				break;
			}
		}
        if(i != BRAP_MAX_CAP_CHAN_IN_ASSOCIATED_GRP)
    	break;
       
	}

  	if( hRapCh->pPath[0] )
	{
    	/* Call Resource manager to release these resources */    
    	ret = BRAP_RM_P_FreeResources(hRapCh->hRap->hRm, &(hRapCh->pPath[0]->sRsrcGrnt),true); /* TODO for all paths */
        if(ret != BERR_SUCCESS)    
        {        
            BDBG_ERR(("BRAP_CAP_P_CloseChannel: call to BRAP_RM_P_FreeResources()" 
                " failed. Ignoring error!!!!!"));        
            ret = BERR_TRACE (ret);    
        }	
        BDBG_MSG(("Resources Freed."));

        /* Free path memory */
        BRAP_P_FreePath(&(hRapCh->pPath[0]));   
    }

    /* Mark the place for the current channel handle to 'invalid' inside RAP 
    handle */
    for(uiChannelNo=0; uiChannelNo<BRAP_RM_P_MAX_CAP_CHANNELS; uiChannelNo++)
    {
        if(hRapCh == hRap->hRapCapCh[uiChannelNo])
        {
            break;
        }
    }	
    hRapCh->hRap->hRapCapCh[hRapCh->uiChannelNo] = NULL;

	/* Free the channel handle */
	/* BKNI_Free(hRapCh);*/ 

	BDBG_LEAVE(BRAP_CAP_P_CloseChannel);
	return(ret);
}

/***************************************************************************
Summary:   
   Gets the destination ring buffers's information.
Description:
   This API returns the destination ring buffers's information.
   Note: BRAP_PcmBufInfo has left and right ring buffer info structure. Each 
      info structure has both pWritePtr and pReadPtr elements. This routine 
      returns pWritePtr = NULL and pReadPtr = valid address (if any) as an
      application requires only a readPtr to copy captured PCM data   from 
      ring buffer to some local memory.
Returns:
   BERR_SUCCESS
See Also:
   BRAP_UpdateRingBufUsg
****************************************************************************/
BERR_Code BRAP_GetRingBufInfo(
   BRAP_DestinationHandle          hDstHandle, /* [in] Destination Handle */
   BRAP_PcmBufInfo                 *psPcmBufInfo,  /* [Out] Associated Pcm buffer 
                                                    information */
    BRAP_OutputChannelPair          eChPair
)
{
    BERR_Code ret = BERR_SUCCESS;

    BDBG_ENTER(BRAP_GetRingBufInfo);

    BKNI_EnterCriticalSection();
    BRAP_GetRingBufInfo_isr(hDstHandle, psPcmBufInfo, eChPair);
    BKNI_LeaveCriticalSection();

    BDBG_LEAVE(BRAP_GetRingBufInfo);
    return ret;
}

/***************************************************************************
Summary:   Gets the destination ring buffers's information.
Description:
   This API returns the destination ring buffers's information. This is an 
   ISR version of BRAP_GetRingBufInfo(). 
Returns:
   BERR_SUCCESS
See Also:
****************************************************************************/
BERR_Code BRAP_GetRingBufInfo_isr(
   BRAP_DestinationHandle          hDstHandle, /* [in] Destination Handle */
   BRAP_PcmBufInfo                 *psPcmBufInfo,  /* [Out] Associated Pcm buffer 
                                                               information */
    BRAP_OutputChannelPair          eChPair
)
{
	BERR_Code ret = BERR_SUCCESS;
	unsigned int uiLtRBufId = BRAP_RM_P_INVALID_INDEX;
	unsigned int uiRtRBufId = BRAP_RM_P_INVALID_INDEX;
		
	BDBG_ENTER(BRAP_GetRingBufInfo_isr);

	/* Validate input params */
	BDBG_ASSERT(hDstHandle);
	BDBG_ASSERT(psPcmBufInfo);

    /* Pre-fill with invalid values */
    psPcmBufInfo->sRtBufInfo.pBasePtr = NULL;
    psPcmBufInfo->sRtBufInfo.pEndPtr = NULL;
    psPcmBufInfo->sRtBufInfo.pReadPtr = NULL;
    psPcmBufInfo->sRtBufInfo.pWritePtr = NULL;
    psPcmBufInfo->sRtBufInfo.uiContiguousSize = 0;
    psPcmBufInfo->sRtBufInfo.uiTotalSize = 0;

    psPcmBufInfo->sLtOrSnglBufInfo.pBasePtr = NULL;
    psPcmBufInfo->sLtOrSnglBufInfo.pEndPtr = NULL;
    psPcmBufInfo->sLtOrSnglBufInfo.pReadPtr = NULL;
    psPcmBufInfo->sLtOrSnglBufInfo.pWritePtr = NULL;
    psPcmBufInfo->sLtOrSnglBufInfo.uiContiguousSize = 0;
    psPcmBufInfo->sLtOrSnglBufInfo.uiTotalSize = 0;    

	BDBG_MSG(("BRAP_GetRingBufInfo_isr: hDstHandle=%d psPcmBufInfo=%d,psPcmBufInfo->eChPair =%d",hDstHandle, psPcmBufInfo,eChPair));
	
	/* Get the valid rbuf ids */
	uiLtRBufId = hDstHandle->sExtDstDetails.uDstDetails.sRBufDetails.uiRBufId[eChPair*2];
	uiRtRBufId = hDstHandle->sExtDstDetails.uDstDetails.sRBufDetails.uiRBufId[((eChPair)*2)+1];

	BDBG_MSG(("uiLtRBufId > %d, uiRtRBufId > %d", uiLtRBufId, uiRtRBufId));

	if(uiLtRBufId != (unsigned int)BRAP_RM_P_INVALID_INDEX)
	{
		ret = BRAP_PrepRingBufInfo_isr(hDstHandle, uiLtRBufId, &(psPcmBufInfo->sLtOrSnglBufInfo));
		if(ret != BERR_SUCCESS)
		{
            BDBG_ERR(("BRAP_GetRingBufInfo_isr: Couldn't get left ring buffer[id - %d]info",
				uiLtRBufId));			
            return BERR_TRACE(ret);
		}
	}

	if(uiRtRBufId != (unsigned int)BRAP_RM_P_INVALID_INDEX)
	{
		ret = BRAP_PrepRingBufInfo_isr(hDstHandle, uiRtRBufId, &(psPcmBufInfo->sRtBufInfo));
		if(ret != BERR_SUCCESS)
		{
			BDBG_ERR(("BRAP_GetRingBufInfo_isr: Couldn't get right ring buffer[id - %d]info",
				uiRtRBufId));			
            return BERR_TRACE(ret);
		}
	}

	BDBG_MSG(("BRAP_GetRingBufInfo_isr: OutParam:\n"
			"\tsLtOrSnglBufInfo(B=%p E=%p W=%p R=%p C=%d T=%d)\n"
			"\tsRtBufInfo(B=%p E=%p W=%p R=%p C=%d T=%d",
			psPcmBufInfo->sLtOrSnglBufInfo.pBasePtr,
			psPcmBufInfo->sLtOrSnglBufInfo.pEndPtr,
			psPcmBufInfo->sLtOrSnglBufInfo.pWritePtr,
			psPcmBufInfo->sLtOrSnglBufInfo.pReadPtr,
			psPcmBufInfo->sLtOrSnglBufInfo.uiContiguousSize,
			psPcmBufInfo->sLtOrSnglBufInfo.uiTotalSize,
			psPcmBufInfo->sRtBufInfo.pBasePtr,
			psPcmBufInfo->sRtBufInfo.pEndPtr,
			psPcmBufInfo->sRtBufInfo.pWritePtr,
			psPcmBufInfo->sRtBufInfo.pReadPtr,
			psPcmBufInfo->sRtBufInfo.uiContiguousSize,
			psPcmBufInfo->sRtBufInfo.uiTotalSize));
			
	BDBG_LEAVE(BRAP_GetRingBufInfo_isr);
	return ret;
}

/***************************************************************************
Summary:   
   Updates the read pointers for the destination ring buffers as per the 
   ring buffer usage provided.
Description:
   After reading PCM data from the ring buffer, an application can use this 
   routine to update the read pointer associated to that ring buffer. 
Returns:
   BERR_SUCCESS
See Also:
   BRAP_GetRingBufInfo
****************************************************************************/
BERR_Code BRAP_UpdateRingBufUsg( 
   BRAP_DestinationHandle          hDstHandle, /* [in] Destination Handle */
    const BRAP_PcmBufSzUpdt   *psPcmBufSzUpdt,   /* [in] Ring buffer read 
                                                information */
    BRAP_OutputChannelPair          eChPair
)
{
    BERR_Code ret = BERR_SUCCESS;

    BDBG_ENTER(BRAP_UpdateRingBufUsg);

    BKNI_EnterCriticalSection();
    ret = BRAP_UpdateRingBufUsg_isr(hDstHandle, psPcmBufSzUpdt, eChPair);
    BKNI_LeaveCriticalSection();

    BDBG_LEAVE(BRAP_UpdateRingBufUsg);
    return ret;
}

/***************************************************************************
Summary:   
   Updates the read pointers for the destination ring buffers as per the 
   ring buffer usage provided.
Description:
   After reading PCM data from the ring buffer, an application can use this 
   routine to update the read pointer associated to that ring buffer. 
   This is an ISR version of BRAP_UpdateRingBufUsg().
Returns:
   BERR_SUCCESS
See Also:
   BRAP_GetRingBufInfo
****************************************************************************/
BERR_Code BRAP_UpdateRingBufUsg_isr( 
   BRAP_DestinationHandle          hDstHandle, /* [in] Destination Handle */
    const BRAP_PcmBufSzUpdt        *psPcmBufSzUpdt,   /* [in] Ring buffer usage 
                                                        information */
    BRAP_OutputChannelPair          eChPair
)
{
    BERR_Code ret = BERR_SUCCESS;

	unsigned int uiLtRBufId = BRAP_RM_P_INVALID_INDEX;
	unsigned int uiRtRBufId = BRAP_RM_P_INVALID_INDEX;
        uint32_t ui32IntStatusTemp=0;
		
	BDBG_ENTER(BRAP_UpdateRingBufUsg_isr);

	/* Validate input params */
	BDBG_ASSERT(hDstHandle);
	BDBG_ASSERT(psPcmBufSzUpdt);

	BDBG_MSG(("BRAP_CAP_UpdateBufUsg_isr: hDstHandle=%d"
			  	"\tpsPcmBufSzUpdt->uiLtOrSnglBufSzUpdt=%d"
			  	"\tpsPcmBufSzUpdt->uiRtBufSzUpdt=%d",hDstHandle,
			  	psPcmBufSzUpdt->uiLtOrSnglBufSzUpdt,
			  	psPcmBufSzUpdt->uiRtBufSzUpdt));
			  
	/* Get the valid rbuf ids */
	uiLtRBufId = hDstHandle->sExtDstDetails.uDstDetails.sRBufDetails.uiRBufId[eChPair*2];
	uiRtRBufId = hDstHandle->sExtDstDetails.uDstDetails.sRBufDetails.uiRBufId[((eChPair)*2)+1];
    
	BDBG_MSG(("uiLtRBufId > %d, uiRtRBufId > %d", uiLtRBufId, uiRtRBufId));

	if(uiLtRBufId != (unsigned int)BRAP_RM_P_INVALID_INDEX)
	{
		ret = BRAP_WriteRingBufUsg_isr(hDstHandle, uiLtRBufId, psPcmBufSzUpdt->uiLtOrSnglBufSzUpdt);
		if(ret != BERR_SUCCESS)
		{
			BDBG_ERR(("BRAP_CAP_UpdateBufUsg_isr: Couldn't update left ring buffer[0] usage"));			
			return BERR_TRACE(ret);	
		}
	}

	if(uiRtRBufId != (unsigned int)BRAP_RM_P_INVALID_INDEX)
	{
		ret = BRAP_WriteRingBufUsg_isr(hDstHandle,uiRtRBufId, psPcmBufSzUpdt->uiRtBufSzUpdt);
		if(ret != BERR_SUCCESS)
		{
			BDBG_ERR(("BRAP_CAP_UpdateBufUsg_isr: Couldn't update right ring buffer[1] usage"));			
			return BERR_TRACE(ret);	
		}
	}

#if (BRAP_P_WATERMARK_WORKAROUND == 0)
#if (BRAP_P_EDGE_TRIG_INTRPT == 0)
	ret = BRAP_P_UnmaskDestinationInterrupt_isr(hDstHandle, BRAP_Interrupt_eFmmDstRbufFullMark);
	if(ret != BERR_SUCCESS)
	{
		BDBG_ERR(("BRAP_CAP_UpdateBufUsg_isr: Couldn't unmask eFmmDstRbufFullMark interrupt")); 		
		return BERR_TRACE(ret); 
	}
#else

            ui32IntStatusTemp = 1 << (8 + (hDstHandle->sExtDstDetails.uDstDetails.sRBufDetails.uiDstChId[eChPair]));
                                        /*since for Dstn Rbuf it is bits 8-11 and 0-7bits are for source Rbuf*/
	BRAP_Write32_isr (hDstHandle->hAssociation->hRap->hRegister, BCHP_AUD_FMM_BF_ESR2_H_STATUS_CLEAR, ui32IntStatusTemp);    

    BRAP_P_ReArmFullMarkInterrupt_isr(hDstHandle);
#endif    
#endif

	BDBG_LEAVE(BRAP_UpdateRingBufUsg_isr);
	return ret;
}

static BERR_Code BRAP_PrepRingBufInfo_isr(
    BRAP_DestinationHandle          hDstHandle,
	unsigned int 			        uiRBufId,
	BRAP_RingBufInfo		        *pRBufInfo
	)
{
	BERR_Code err = BERR_SUCCESS;

	BRAP_RBUF_P_Handle hRBuf;
	uint32_t ui32RdAddr = 0;
	uint32_t ui32WrAddr = 0;
	uint32_t ui32RegVal = 0;
	uint32_t ui32BaseAddr = 0;
	unsigned int uiRBufSize = 0;
	unsigned int uiFrameSize = 0;
    unsigned int uiGap = 256;
    uint32_t ui32WrapBit;
    bool bSameWrapBit;

	BDBG_ENTER(BRAP_PrepRingBufInfo_isr);

	/* Get the ring buffer handle */
	hRBuf = hDstHandle->hAssociation->hRap->hFmm[0]->hRBuf[uiRBufId];
    pRBufInfo->uiRBufId = uiRBufId;
	if(!(BRAP_P_IsPointerValid((void *)hRBuf)))
	{
		BDBG_ERR(("BRAP_CAP_P_PrepBufInfo_isr: hRBuf[%d] is NULL", uiRBufId));
		return BERR_TRACE(BERR_NOT_INITIALIZED);
	}

	/* Get the base address */	
	ui32BaseAddr = BRAP_Read32_isr(
					hRBuf->hRegister,  
                	(BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_BASEADDR + hRBuf->ui32Offset));

	/* Get current write address */	
	ui32RegVal = BRAP_Read32_isr(
					hRBuf->hRegister,  
                	(BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_WRADDR + hRBuf->ui32Offset));

    ui32WrAddr = BCHP_GET_FIELD_DATA(ui32RegVal,
				AUD_FMM_BF_CTRL_RINGBUF_0_WRADDR,
				RINGBUF_WRADDR);	

    ui32WrapBit = BCHP_GET_FIELD_DATA(ui32RegVal,
                                      AUD_FMM_BF_CTRL_RINGBUF_0_WRADDR,
                                      RINGBUF_WRADDR_WRAP);	

	/* Get current read address */	
	ui32RegVal = BRAP_Read32_isr(
					hRBuf->hRegister,  
                	(BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR + hRBuf->ui32Offset));

    ui32RdAddr = BCHP_GET_FIELD_DATA(ui32RegVal,
				AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR,
				RINGBUF_RDADDR);	

    ui32WrapBit ^= BCHP_GET_FIELD_DATA(ui32RegVal,
                                      AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR,
                                      RINGBUF_RDADDR_WRAP);

    /* This will be one if the bits don't match */
    bSameWrapBit = (ui32WrapBit)?false:true;

	/* Prepare return buf info structure */
	uiRBufSize = hRBuf->sSettings.sExtSettings.uiSize;

	/* Each Ring buffer is logically divided into 4 frames. This is also the max 
   	   contiguous ring buffer size in bytes written with PCM data that is informed 
   	   to the user */
	BRAP_RBUF_P_GetFrameSize(hRBuf->sSettings.sExtSettings.uiSize, &uiFrameSize);
	
	pRBufInfo->pBasePtr = (hRBuf->sSettings.sExtSettings.pBufferStart);
	pRBufInfo->pEndPtr = (void *)((unsigned int)pRBufInfo->pBasePtr + uiRBufSize);

    err = BMEM_ConvertOffsetToAddress(hRBuf->hHeap, ui32RdAddr, (void **)&(pRBufInfo->pReadPtr));
	BDBG_ASSERT(err == BERR_SUCCESS);
    
	BDBG_MSG(("Offset (%p) -> Address (%p)",ui32RdAddr,pRBufInfo->pReadPtr));

    /* If the write pointer is behind read pointer - or the buffer is completely full */
	if(ui32WrAddr < ui32RdAddr || (ui32WrAddr == ui32RdAddr && bSameWrapBit == false))
	{
		/* tot used size = rbuf size - (rdPtr - wrPtr) */ 
		pRBufInfo->uiTotalSize = uiRBufSize - (ui32RdAddr - ui32WrAddr);

		/* cont used size = endPtr - rdPtr */
		pRBufInfo->uiContiguousSize = (ui32BaseAddr + uiRBufSize) - ui32RdAddr; 	 

		/* Clip cont used size if more than tot free size */
		if(pRBufInfo->uiContiguousSize > pRBufInfo->uiTotalSize)
			pRBufInfo->uiContiguousSize = pRBufInfo->uiTotalSize;
 	}
	else
	{
		/* cont used size = tot used size = wrPtr - rdPtr */ 
		pRBufInfo->uiTotalSize = ui32WrAddr - ui32RdAddr;
		pRBufInfo->uiContiguousSize = pRBufInfo->uiTotalSize;
	}

	BDBG_MSG(("Before FRAME logic W=%p R=%p T=%p C=%p F=%p",ui32WrAddr, ui32RdAddr, 
				pRBufInfo->uiTotalSize, pRBufInfo->uiContiguousSize, uiFrameSize));

	/* Return contiguous size keeping some gap between R-W ptrs */
   	if(pRBufInfo->uiTotalSize == pRBufInfo->uiContiguousSize)
    {
    	if(pRBufInfo->uiContiguousSize > uiGap)
    	{
    		pRBufInfo->uiContiguousSize -= uiGap;
    	}
    	else
    	{
        	pRBufInfo->uiContiguousSize = 0;
        }    
    }
    else if(pRBufInfo->uiTotalSize > pRBufInfo->uiContiguousSize)
    {
        /* The complete contiguous size can be returned to the app */
    }        
    else
    {
        /* This case should never occur */
        BDBG_ERR(("T(0x%x) < C(0x%x)",pRBufInfo->uiTotalSize,pRBufInfo->uiContiguousSize));
        BDBG_ASSERT(0);
    }

	BDBG_MSG(("W=%p R=%p T=%p C=%p F=%p",ui32WrAddr, ui32RdAddr, 
				pRBufInfo->uiTotalSize, pRBufInfo->uiContiguousSize, uiFrameSize));
	BDBG_LEAVE(BRAP_PrepRingBufInfo_isr);
	return BERR_SUCCESS;
}

static BERR_Code BRAP_WriteRingBufUsg_isr(
    BRAP_DestinationHandle          hDstHandle,
	unsigned int 		            uiRBufId,
	unsigned int		            uiSzUpdt
	)
{
	BRAP_RBUF_P_Handle hRBuf;
	uint32_t ui32RdAddr = 0;
	uint32_t ui32RegVal = 0;
	uint32_t ui32BaseAddr = 0;	
	unsigned int uiRBufSize = 0;
	uint32_t ui32NewRdAddr = 0;
	uint32_t ui32WrapAroundBit = 0;
	
	BDBG_ENTER(BRAP_WriteRingBufUsg_isr);
	BDBG_MSG(("BRAP_CAP_P_WriteBufUsg_isr: hDstHandle=%d, uiRBufId=%d, uiSzUpdt=%d",
		hDstHandle, uiRBufId, uiSzUpdt));

	/* Get the ring buffer handle */
	hRBuf = hDstHandle->hAssociation->hRap->hFmm[0]->hRBuf[uiRBufId];
	if(!(BRAP_P_IsPointerValid((void *)hRBuf)))
	{
		BDBG_ERR(("BRAP_CAP_P_WriteBufUsg_isr: hRBuf[%d] is NULL", uiRBufId));
		return BERR_TRACE(BERR_NOT_INITIALIZED);
	}

	/* Get the base address */	
	ui32BaseAddr = BRAP_Read32_isr(
					hRBuf->hRegister,  
                	(BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_BASEADDR + hRBuf->ui32Offset));

	/* Get current read address */	
	ui32RegVal = BRAP_Read32_isr(
					hRBuf->hRegister,  
                	(BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR + hRBuf->ui32Offset));

    ui32RdAddr = BCHP_GET_FIELD_DATA(ui32RegVal,
						AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR,
						RINGBUF_RDADDR);	

	ui32WrapAroundBit = BCHP_GET_FIELD_DATA(ui32RegVal,
						AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR,
						RINGBUF_RDADDR_WRAP);
	uiRBufSize = hRBuf->sSettings.sExtSettings.uiSize;

	BDBG_MSG(("uiSzUpdt(0x%x) ui32RdAddr(0x%x) ui32BaseAddr(0x%x) uiRBufSize(0x%x)",
			uiSzUpdt , ui32RdAddr, ui32BaseAddr, uiRBufSize));

	/* Check if the uiSzUpdt is within the ring buffer */
	if(uiSzUpdt + ui32RdAddr > ui32BaseAddr + uiRBufSize)
	{
		BDBG_ERR(("BRAP_CAP_P_WriteBufUsg_isr: uiReadSize exceeds ring buffer boundary"));
		BDBG_ERR(("uiSzUpdt(0x%x) + ui32RdAddr(0x%x) > ui32BaseAddr(0x%x) + uiRBufSize(0x%x)",
			uiSzUpdt , ui32RdAddr, ui32BaseAddr, uiRBufSize));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}
	else if(uiSzUpdt + ui32RdAddr == ui32BaseAddr + uiRBufSize)
	{ 
		/* wrap around read ptr to base ptr */
		ui32NewRdAddr = ui32BaseAddr;
		
		BDBG_MSG(("Before WrapAroundBit = 0x%x",ui32WrapAroundBit));
		ui32WrapAroundBit ^= 1;
		BDBG_MSG(("After WrapAroundBit = 0x%x",ui32WrapAroundBit));
	}
	else
	{
		ui32NewRdAddr = ui32RdAddr + uiSzUpdt;
	}
		
	/* Write back the updated read pointer */
	ui32RegVal = (BCHP_FIELD_DATA (
                        AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR, 
                        RINGBUF_RDADDR, 
                        ui32NewRdAddr));

	ui32RegVal |= (BCHP_FIELD_DATA (
                        AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR, 
                        RINGBUF_RDADDR_WRAP,
                        ui32WrapAroundBit));

    BRAP_Write32_isr (hRBuf->hRegister,  
                  BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR + hRBuf->ui32Offset, 
                  ui32RegVal);

	BDBG_MSG(("ui32RdAddr = %p,  uiSzUpdt = %p,  newRdAddr = %p",
		ui32RdAddr, uiSzUpdt, ui32RegVal));
	BDBG_LEAVE(BRAP_WriteRingBufUsg_isr);
	return BERR_SUCCESS;
}

/***************************************************************************
Summary:   
   Flushes the Ring Buffer.
Description:
   This API is required to be called by the application after it has received
   an Overflow interrupt. This call makes the Read pointer equal the write pointer
   and hence flushes the data out
Returns:
    Returns error on failure or BERR_SUCCESS on success
See Also:
   BRAP_FlushRingBuffer_isr
****************************************************************************/
BERR_Code BRAP_FlushRingBuffer(
   BRAP_DestinationHandle          hDstHandle /* [in] Destination Handle */
)
{
	BERR_Code ret = BERR_SUCCESS;
    BDBG_ENTER(BRAP_FlushRingBuffer);

	BDBG_ASSERT(hDstHandle);	
	BKNI_EnterCriticalSection();
	ret = BRAP_FlushRingBuffer_isr(hDstHandle);
	BKNI_LeaveCriticalSection();

	BDBG_LEAVE(BRAP_FlushRingBuffer);
	return ret;
}

/***************************************************************************
Summary:   
   Flushes the Ring Buffer - ISR version
Description:
   This API is required to be called by the application after it has received
   an Overflow interrupt. This call makes the Read pointer equal the write pointer
   and hence flushes the data out
Returns:
    Returns error on failure or BERR_SUCCESS on success
See Also:
   BRAP_FlushRingBuffer
****************************************************************************/
BERR_Code BRAP_FlushRingBuffer_isr(
   BRAP_DestinationHandle          hDstHandle /* [in] Destination Handle */
)
{
	BERR_Code ret = BERR_SUCCESS;
	unsigned int uiLtRBufId = BRAP_RM_P_INVALID_INDEX;
	unsigned int uiRtRBufId = BRAP_RM_P_INVALID_INDEX;
    uint32_t ui32RegVal = 0;
    unsigned int uichannelpair = 0;
    BRAP_RBUF_P_Handle hRBuf = NULL;    
		
	BDBG_ENTER(BRAP_FlushRingBuffer_isr);

	/* Validate input params */
	BDBG_ASSERT(hDstHandle);
			  
	/* Get the valid rbuf ids */
	uiLtRBufId = hDstHandle->sExtDstDetails.uDstDetails.sRBufDetails.uiRBufId[BRAP_OutputChannel_eLeft];
	uiRtRBufId = hDstHandle->sExtDstDetails.uDstDetails.sRBufDetails.uiRBufId[BRAP_OutputChannel_eRight];
    
	BDBG_MSG(("uiLtRBufId = %d, uiRtRBufId = %d", uiLtRBufId, uiRtRBufId));



	if(uiLtRBufId != (unsigned int)BRAP_RM_P_INVALID_INDEX)
	{
	    hRBuf = hDstHandle->hAssociation->hRap->hFmm[0]->hRBuf[uiLtRBufId];
    	/* Get current Write address */	
    	ui32RegVal = BRAP_Read32_isr (
    					hRBuf->hRegister,  
                    	BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_WRADDR + hRBuf->ui32Offset
                    	);	

        /* Program Read as Write */
    	BRAP_Write32_isr (
    		hRBuf->hRegister,  
        	BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR + hRBuf->ui32Offset,
        	ui32RegVal
        	);        
    }

	if(uiRtRBufId != (unsigned int)BRAP_RM_P_INVALID_INDEX)
	{
	    hRBuf = hDstHandle->hAssociation->hRap->hFmm[0]->hRBuf[uiRtRBufId];
    	/* Get current Write address */	
    	ui32RegVal = BRAP_Read32_isr (
    					hRBuf->hRegister,  
                    	BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_WRADDR + hRBuf->ui32Offset
                    	);	

        /* Program Read as Write */
    	BRAP_Write32_isr (
    		hRBuf->hRegister,  
        	BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR + hRBuf->ui32Offset,
        	ui32RegVal
        	); 
	}

    ui32RegVal = 0;
    for (uichannelpair = 0; uichannelpair < BRAP_RM_P_MAX_OP_CHANNEL_PAIRS; uichannelpair++)
    {
        if (hDstHandle->sExtDstDetails.uDstDetails.sRBufDetails.uiDstChId[uichannelpair] != (unsigned int)BRAP_RM_P_INVALID_INDEX)
        {
            ui32RegVal = 1 << (8 + (hDstHandle->sExtDstDetails.uDstDetails.sRBufDetails.uiDstChId[uichannelpair]));
                                        /*since for Dstn Rbuf it is bits 8-11 and 0-7bits are for source Rbuf*/
            break;
        }
    }
    BRAP_Write32_isr (hDstHandle->hAssociation->hRap->hRegister, BCHP_AUD_FMM_BF_ESR1_H_STATUS_CLEAR, ui32RegVal);    

    BRAP_P_ReArmFullMarkInterrupt_isr(hDstHandle);

	BDBG_LEAVE(BRAP_FlushRingBuffer_isr);
	return ret;
}

/***************************************************************************
Summary:   
   Informs the PI about the completion of reading of data from ring buffers.
Description:
   This API is required to be called by the application after it has completed
   reading of PCM data from ring buffers. This call is mandatory to ensure that
   it gets further interrupts in future.
Returns:
    Returns error on failure or BERR_SUCCESS on success
See Also:
   BRAP_RingBufferReadDone_isr
****************************************************************************/
BERR_Code BRAP_RingBufferReadDone(
   BRAP_DestinationHandle          hDstHandle /* [in] Destination Handle */
)
{
	BERR_Code ret = BERR_SUCCESS;
    BDBG_ENTER(BRAP_RingBufferReadDone);
	BDBG_ASSERT(hDstHandle);	
    
	BKNI_EnterCriticalSection();
	ret = BRAP_RingBufferReadDone_isr(hDstHandle);
	BKNI_LeaveCriticalSection();

	BDBG_LEAVE(BRAP_RingBufferReadDone);
	return ret;
}

/***************************************************************************
Summary:   
   Informs the PI about the completion of reading of data from ring buffers.
Description:
   This API is required to be called by the application after it has completed
   reading of PCM data from ring buffers. This call is mandatory to ensure that
   it gets further interrupts in future. This API is used within the ISR context
Returns:
    Returns error on failure or BERR_SUCCESS on success
See Also:
   BRAP_RingBufferReadDone
****************************************************************************/
BERR_Code BRAP_RingBufferReadDone_isr(
   BRAP_DestinationHandle          hDstHandle /* [in] Destination Handle */
)
{
	BERR_Code ret = BERR_SUCCESS;
#if ( BRAP_P_WATERMARK_WORKAROUND == 1 )
	BRAP_PcmBufInfo sPcmBufInfo;

	uint32_t	ui32WaterMarkinBytes =0;
	uint32_t	ui32InitalTimerValue=0;
	uint32_t	ui32Diff=0;
	BAVC_AudioSamplingRate 	eSamplingRate=BAVC_AudioSamplingRate_eUnknown;
	uint32_t	ui32IpBitsPerSample;
    unsigned int uiSR;
	uint32_t	ui32TotalFullBytes=0;
    BRAP_RBUF_P_Handle hRBuf = NULL;
    int i=0;

	BDBG_ENTER(BRAP_CAP_BufferReadDone_isr);
	BDBG_MSG(("Entering BRAP_CAP_BufferWriteDone_isr"));

	/* 1. Validate input parameters. */
	BDBG_ASSERT(hDstHandle);
    
#ifdef AUTOTRAP
    ret = BTMR_StopTimer_isr ( hRapCh->hTimer1 );
#else
	ret = BTMR_StopTimer_isr ( hRapCh->hTimer );
#endif /* AUTOTRAP */

	if ( ret != BERR_SUCCESS )
	{
		BDBG_ERR(("BRAP_CAP_BufferWriteDone_isr() Failed to stop timer"));
		return BERR_TRACE(ret);
	}

	ret = BRAP_CAP_GetBufInfo_isr(hRapCh, &sPcmBufInfo);
	if ( ret != BERR_SUCCESS )
	{
		BDBG_ERR(("BRAP_CAP_BufferWriteDone_isr() Failed to ger buffer info"));
		return BERR_TRACE(ret);
	}

    /* Get the ring buffer handle */
/*	hRBuf = hRapCh->pPath[BRAP_P_UsgPath_eDecodePcm].hRBuf[sPcmBufInfo.sLtOrSnglBufInfo.uiRBufId];*/
	hRBuf = hRapCh->hRap->hFmm[0]->hRBuf[sPcmBufInfo.sLtOrSnglBufInfo.uiRBufId];
    
	ui32WaterMarkinBytes = ( hRBuf->sSettings.sExtSettings.uiWaterMark *
						            hRBuf->sSettings.sExtSettings.uiSize )/100;
	ui32TotalFullBytes = sPcmBufInfo.sLtOrSnglBufInfo.uiTotalSize;

	BDBG_MSG(("Before StartTimer"));
	if ( ui32TotalFullBytes >= ui32WaterMarkinBytes )
	{
#ifdef AUTOTRAP
        ret=BTMR_StartTimer_isr(hRapCh->hTimer1, 1000);
#else
		ret=BTMR_StartTimer_isr(hRapCh->hTimer, 1000);
#endif /* AUTOTRAP */

        	if ( ret != BERR_SUCCESS )
    		{
    			BDBG_ERR(("BRAP_CAP_BufferWriteDone_isr() Failed to get buffer info"));
    			return BERR_TRACE(ret);
    		}
       	goto end_writedone;
	}
	else
	{

		ui32Diff = ( ui32WaterMarkinBytes - ui32TotalFullBytes );
        
        for (i=0; i<BRAP_P_MAX_PATHS_IN_A_CHAN; i++)
            if (hRapCh->pPath[i]->sCapPort[BRAP_OutputChannelPair_eLR][0].hCapPort != NULL)
                break;          
            
            if(i >= BRAP_P_MAX_PATHS_IN_A_CHAN)
            {
                BDBG_ERR(("i = %d is greater than BRAP_P_MAX_PATHS_IN_A_CHAN =%d",i,BRAP_P_MAX_PATHS_IN_A_CHAN));
                BDBG_ASSERT(0);
                return BERR_TRACE(BERR_INVALID_PARAMETER);
            }
            
		ui32IpBitsPerSample = hRapCh->pPath[i]->sCapPort[BRAP_OutputChannelPair_eLR][0].hCapPort->sParams.eInputBitsPerSample;

		eSamplingRate = hRapCh->eInputSamplingRate;

		ret = BRAP_P_ConvertSR(eSamplingRate, &uiSR);
		if(ret != BERR_SUCCESS)
		{
			return BERR_TRACE(ret);
		}


		BDBG_MSG(("ui32Diff= %d, eSamplingRate= %d,ui32IpBitsPerSample = %d, ui32InitalTimerValue=%d"
                  "ui32WaterMarkinBytes=%d, ui32TotalFullBytes= %d",
                  ui32Diff,eSamplingRate,ui32IpBitsPerSample,ui32InitalTimerValue,
                  ui32WaterMarkinBytes,ui32TotalFullBytes));
        
	}
	BDBG_MSG(("After StartTimer"));

	if ( ui32InitalTimerValue == 0 )
	{
		ui32InitalTimerValue = 1000;
	}

#ifdef AUTOTRAP
    ret = BTMR_StartTimer_isr ( hRapCh->hTimer1, ui32InitalTimerValue );
#else
	ret = BTMR_StartTimer_isr ( hRapCh->hTimer, ui32InitalTimerValue );
#endif /* AUTOTRAP */

	if ( ret != BERR_SUCCESS )
	{
		BDBG_ERR(("BRAP_CAP_BufferWriteDone_isr() Failed to start timer"));
		return BERR_TRACE(ret);
	}
	BDBG_MSG(("After BTMR_StartTimer"));

end_writedone:
	BDBG_LEAVE(BRAP_CAP_BufferReadDone_isr);
#else
    BSTD_UNUSED(hDstHandle);

#endif /* BRAP_P_WATERMARK_WORKAROUND */
	return ret;
}

#if ( (BRAP_3548_FAMILY == 1) )
/******************************************************************************
Summary:
	Gets status for a RF Audio decoder for BTSC or Eiaj standards.
Description:
	This function reads the status registers for Rf Audio broadcasting mode
	and processor error conditions.
	
Returns:
    BERR_SUCCESS                - if successful 
    BERR_INVALID_PARAMETER      - Bad input parameter
See Also: 
*******************************************************************************/
BERR_Code BRAP_GetRfAudioStatus (
	BRAP_Handle hRap,	                    /* [in] The RAP handle */
	BRAP_RfAudioStatus  *psStatus			/* [out] Status of the channel */
	)
{
    BERR_Code err = BERR_SUCCESS;
    BRAP_CAPPORT_P_Handle hCapPort;
    
	BDBG_ENTER( BRAP_GetRfAudioStatus );
    BDBG_ASSERT( hRap );
    BDBG_ASSERT( psStatus );

    hCapPort = hRap->hFmm[0]->hCapPort[BRAP_CapInputPort_eRfAudio];
    if(!(BRAP_P_IsPointerValid((void *)hCapPort)))
    {
    	BDBG_ERR((" RAP channel using RF Audio port is not in start state "));
    	return BERR_TRACE( BERR_NOT_SUPPORTED );
    }
	
    err = BRAP_RFAUDIO_P_GetStatus ( hCapPort, psStatus );
    if ( BERR_SUCCESS!=err)
    {
        return BERR_TRACE( err );
    }

    BDBG_LEAVE( BRAP_GetRfAudioStatus );
	return err;
}
	
/******************************************************************************
Summary:
	Gets RF Audio envelope frequency amplitude
Description:
	This function returns the RF Audio envelope frequency amplitude
	
Returns:
    BERR_SUCCESS                - if successful 
    BERR_INVALID_PARAMETER      - Bad input parameter
See Also: 
*******************************************************************************/
BERR_Code BRAP_GetRfAudioEFAMPLStatus (
	BRAP_Handle hRap,	        /* [in] The RAP handle */
	uint32_t  *puReg			/* [out] Status of the channel 
								    Bits[15:0] amplitude of the stereo signal modulated 
								    on the AM carrier
								    Bits[31:16] amplitude of the dual mono signal 
								    modulated on the AM carrier*/
	)
{
    BERR_Code err = BERR_SUCCESS;
    BRAP_CAPPORT_P_Handle hCapPort;
    
	BDBG_ENTER( BRAP_GetRfAudioEFAMPLStatus );
    BDBG_ASSERT( hRap );
    BDBG_ASSERT( puReg );

    hCapPort = hRap->hFmm[0]->hCapPort[BRAP_CapInputPort_eRfAudio];
    if(!(BRAP_P_IsPointerValid((void *)hCapPort)))
    {
    	BDBG_ERR((" RAP channel using RF Audio port is not in start state "));
    	return BERR_TRACE( BERR_NOT_SUPPORTED );
    }
	
    err = BRAP_RFAUDIO_P_GetEnvelopeFreqAmpl ( hCapPort, puReg );
    if (BERR_SUCCESS != err)
    {
        return BERR_TRACE( err );
    }

    BDBG_LEAVE( BRAP_GetRfAudioEFAMPLStatus );
	return err;
}


/******************************************************************************
Summary:
	Set RF Audio BTSC_CTRL.BASIC_CTRL1.JMPCOND_RESV3.
Description:
	This function sets BTSC_CTRL.BASIC_CTRL1.JMPCOND_RESV3
	WARNING: This API will be replaced by new API which will be more specific to the functionality.
Returns:
    BERR_SUCCESS                - if successful 
    BERR_INVALID_PARAMETER      - Bad input parameter 
See Also:
*******************************************************************************/
BERR_Code BRAP_SetRfAudioFirmwareReg(
	BRAP_Handle hRap,	            /* [in] The RAP handle */
	uint8_t uRegNum,				/* [in] Reg number 1, 2, 3, 4 */
	bool bValue	                    /* [in] 0 -  1 */
	)
{
    BERR_Code err = BERR_SUCCESS;
    BRAP_CAPPORT_P_Handle hCapPort;
    
    BDBG_ENTER( BRAP_SetRfAudioFirmwareReg );
    BDBG_ASSERT( hRap );

    hCapPort = hRap->hFmm[0]->hCapPort[BRAP_CapInputPort_eRfAudio];
    if(!(BRAP_P_IsPointerValid((void *)hCapPort)))
    {
    	BDBG_ERR((" RAP channel using RF Audio port is not in start state "));
    	return BERR_TRACE( BERR_NOT_SUPPORTED );
    }
	
    err = BRAP_RFAUDIO_P_SetFirmwareReg( hCapPort, uRegNum, bValue );
    if ( BERR_SUCCESS!=err)
    {
        return BERR_TRACE( err );
    }

    BDBG_LEAVE( BRAP_SetRfAudioFirmwareReg );
    return err;
}

/******************************************************************************
Summary:
	Set RF Audio BTSC_CTRL.BASIC_CTRL2.VAR_OUTSCALING.
Description:
	This function sets BTSC_CTRL.BASIC_CTRL2.VAR_OUTSCALING
Returns:
    BERR_SUCCESS                - if successful 
    BERR_INVALID_PARAMETER      - Bad input parameter 
See Also:
*******************************************************************************/
BERR_Code BRAP_ScaleRfAudioOutput(
	BRAP_Handle hRap,	            /* [in] The RAP handle */
	uint32_t ui32Reg			    /* [in] Input gain scale */
	)
{
    BERR_Code err = BERR_SUCCESS;
    BRAP_CAPPORT_P_Handle hCapPort;
    
    BDBG_ENTER( BRAP_ScaleRfAudioOutput );
    BDBG_ASSERT( hRap );

    hCapPort = hRap->hFmm[0]->hCapPort[BRAP_CapInputPort_eRfAudio];
    if(!(BRAP_P_IsPointerValid((void *)hCapPort)))
    {
    	BDBG_ERR((" RAP channel using RF Audio port is not in start state "));
    	return BERR_TRACE( BERR_NOT_SUPPORTED );
    }
	
	
    err = BRAP_RFAUDIO_P_ScaleOutput( hCapPort, ui32Reg );
    if ( BERR_SUCCESS!=err)
    {
        return BERR_TRACE( err );
    }

    BDBG_LEAVE( BRAP_ScaleRfAudioOutput );
    return err;
}

/******************************************************************************
Summary:
	Scale input signal to Rf Audio decoder
	
Description:
	This function scales the signal at RF Audio digital decoder's input.
	
Returns:
    BERR_SUCCESS                - if successful 
    BERR_INVALID_PARAMETER      - Bad input parameter 
See Also:
*******************************************************************************/
BERR_Code BRAP_ScaleRfAudioInput(
	BRAP_Handle hRap,	           			 /* [in] The RAP handle */
	uint32_t ui32InputScaleValue		     /* [in] Input gain scale */
	)
{
    BERR_Code err = BERR_SUCCESS;
    BRAP_CAPPORT_P_Handle hCapPort;
    
    BDBG_ENTER( BRAP_ScaleRfAudioInput );
    BDBG_ASSERT( hRap );

    hCapPort = hRap->hFmm[0]->hCapPort[BRAP_CapInputPort_eRfAudio];
    if(!(BRAP_P_IsPointerValid((void *)hCapPort)))
    {
    	BDBG_ERR((" RAP channel using RF Audio port is not in start state "));
    	return BERR_TRACE( BERR_NOT_SUPPORTED );
    }
	
	
    err = BRAP_RFAUDIO_P_ScaleInput( hCapPort, ui32InputScaleValue );
    if ( BERR_SUCCESS!=err)
    {
        return BERR_TRACE( err );
    }

    BDBG_LEAVE( BRAP_ScaleRfAudioInput );
    return err;
}

#endif /* #if ( BCHP_CHIP == 3548 ) */


/**************************************************************************
Summary:
    Private function that gets the current audio params for a capture channel.
**************************************************************************/
BERR_Code
BRAP_CAP_P_GetCurrentAudioParams (
	BRAP_ChannelHandle	    hRapCh,         /* [in] Capture channel handle */
	BRAP_ChannelParams	    *pAudioParams  /* [out] Current channel params */
	)
{
	BERR_Code                   err = BERR_SUCCESS;
    BRAP_OutputChannelPair      eChP = BRAP_OutputChannelPair_eMax;
    BRAP_SRCCH_P_Params         sSrcChParams;
    unsigned int                uiPth = 0;
    bool                        bFound=false;
#if ((BRAP_3548_FAMILY == 1) )
    BRAP_DSPCHN_P_AudioParams	*pDspChParams;
    BRAP_DSPCHN_Handle          hDspCh = NULL;
#endif

	BDBG_ENTER(BRAP_CAP_P_GetCurrentAudioParams);
	BDBG_ASSERT(hRapCh);
	BDBG_ASSERT(pAudioParams);


    if(hRapCh->eChannelType != BRAP_ChannelType_ePcmCapture)
    {
        BDBG_ERR(("BRAP_CAP_P_GetCurrentAudioParams: ChType(%d) is not Capture"
            " ChType(%d)", hRapCh->eChannelType, BRAP_ChannelType_ePcmCapture));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    
    BKNI_Memset(pAudioParams, 0, sizeof(BRAP_ChannelParams));
    
	/* Get parameters that are available in channel handle */
    pAudioParams->eAudioSource      = hRapCh->eAudioSource;
    BRAP_P_ConvertSrToEnum(hRapCh->uiInputSamplingRate,&(pAudioParams->eInputSR));    
    pAudioParams->bInputLfePresent  = hRapCh->bInputLfeOn;
    pAudioParams->eInputAudMode     = hRapCh->eInputAudMode;
    pAudioParams->sMixingCoeff      = hRapCh->sMixingCoeff;

#if ((BRAP_3548_FAMILY == 1) )
	/* Get DSP channel parameters */
    for(uiPth = 0; uiPth < BRAP_P_MAX_PATHS_IN_A_CHAN; uiPth++)
    {
    	if(!(BRAP_P_IsPointerValid((void *)hRapCh->pPath[uiPth] )))
    	{
		continue;
    	}
        if(((hRapCh->pPath[uiPth]->eUsgPath == BRAP_P_UsgPath_eDecodePcm)||
           (hRapCh->pPath[uiPth]->eUsgPath == BRAP_P_UsgPath_eDecodeCompress)) &&
           (BRAP_P_IsPointerValid((void *)hRapCh->pPath[uiPth]->hDspCh)))
        {
            hDspCh = hRapCh->pPath[uiPth]->hDspCh;
            break;
        }
    }
    if(!(BRAP_P_IsPointerValid((void *)hDspCh)))
    {
        BDBG_ERR(("BRAP_DEC_P_GetCurrentAudioParams: hDspCh not found"));
		return BERR_TRACE(BERR_NOT_INITIALIZED);
    }

	/* Malloc large local structures */
	pDspChParams = hRapCh->hRap->sOpenTimeMallocs.pDspChParams;
	if( NULL==pDspChParams)
	{
		err =  BERR_TRACE( BERR_OUT_OF_SYSTEM_MEMORY );
            goto error;
	}
    BKNI_Memset(pDspChParams ,0,sizeof( BRAP_DSPCHN_P_AudioParams ));   

	err = BRAP_DSPCHN_P_GetCurrentAudioParams(hDspCh, pDspChParams);
	if(err != BERR_SUCCESS)
    {
        BDBG_ERR(("BRAP_DEC_P_GetCurrentAudioParams: "
            " BRAP_DSPCHN_P_GetCurrentAudioParams returned err(%d)", err));
        err =  BERR_TRACE(err);
        goto error;        
    }
    
	pAudioParams->sDspChParams  = pDspChParams->sExtAudioParams;
	pAudioParams->eTimebase     = pDspChParams->eTimebase;
    pAudioParams->bPlayback     = pDspChParams->bPlayback;
#else    
    pAudioParams->eTimebase = BAVC_Timebase_e0;	
#endif    

    /* Get SrcCh Params */
    for(uiPth = 0; uiPth < BRAP_P_MAX_PATHS_IN_A_CHAN; uiPth++)
    {
        if(!(BRAP_P_IsPointerValid((void *)hRapCh->pPath[uiPth])))
        {
            continue;
        }
        bFound = false;
        for(eChP=0; eChP < BRAP_OutputChannelPair_eMax; eChP++)
        {
            if(!(BRAP_P_IsPointerValid((void *)hRapCh->pPath[uiPth]->sSrcCh[eChP].hSrcCh)))
            {
                continue;
            }

            err = BRAP_SRCCH_P_GetCurrentParams(
                                    hRapCh->pPath[uiPth]->sSrcCh[eChP].hSrcCh,
                                    &sSrcChParams);
            if(err != BERR_SUCCESS)
            {
		        err =  BERR_TRACE( err );
                goto error;
            }
            /* We got one SrcCh, break from the loop */
            bFound = true;
            break;
        }/* for eChP */
        if(true == bFound)
            break;
    }
/*    if(BRAP_OutputChannelPair_eMax == eChP)*/
    if(false == bFound)
    {
		BDBG_ERR(("BRAP_CAP_P_GetCurrentAudioParams: hSrcCh not found"));
        err =  BERR_TRACE( BERR_NOT_INITIALIZED );
        goto error;
    }

    pAudioParams->eInputBitsPerSample = sSrcChParams.eInputBitsPerSample;
    pAudioParams->eBufDataMode          = sSrcChParams.eBufDataMode; 

#if (BRAP_UNSIGNED_PCM_SUPPORTED ==1 )
    pAudioParams->bIsPcmSigned = sSrcChParams.bIsSigned;
#endif
     
    pAudioParams->eCapMode      = hRapCh->eCapMode;
    pAudioParams->eCapInputPort = hRapCh->eCapInputPort;
    pAudioParams->eInputChPair  = BRAP_OutputChannelPair_eLR;

    BKNI_Memset(&(pAudioParams->sXptContextMap), BRAP_INVALID_VALUE, 
                sizeof(BAVC_XptContextMap));

error:   

	BDBG_LEAVE( BRAP_CAP_P_GetCurrentAudioParams );
	return err;
}

/* End Of File */	

