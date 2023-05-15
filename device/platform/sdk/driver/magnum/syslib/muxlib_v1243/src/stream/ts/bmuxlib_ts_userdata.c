/***************************************************************************
 *     Copyright (c) 2010-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmuxlib_ts_userdata.c $
 * $brcm_Revision: Hydra_Software_Devel/23 $
 * $brcm_Date: 4/22/13 5:58p $
 *
 * [File Description:]
 * Private Functions to provide VBI Userdata pass-thru
 *
 * Userdata processing:
 * Read the current video frame's OPTS and DTS
 * Determine an adjustment to convert DTS to userdata domain (i.e. OPTS domain)
 * Scan the incoming userdata stream to find a matching userdata packet
 * Adjust the matching userdata packet using the same adjustment to convert to video stream domain
 * Schedule PES packets to fit them in with PCR and System Data
 *    => calculate an ESCR for the outgoing packet(s)
 *    => process PCR
 *    => while space for packets
 *          - process userdata
 *          - process system data
 *
 * [Revision History:]
 * $brcm_Log: /magnum/syslib/muxlib/src/stream/ts/bmuxlib_ts_userdata.c $
 * 
 * Hydra_Software_Devel/23   4/22/13 5:58p delkert
 * SW7425-4709: Add continuity count checking on userdata packets. Skip
 * Packet parsing if packet was already parsed but was outside selection
 * window.
 *
 * Hydra_Software_Devel/22   4/5/13 5:19p delkert
 * SW7425-4643: Remove obsolete code. Move system data pre-Q code to its
 * own routine so it can be called after A/V input processing. Keep track
 * of found A/V/PCR PIDs. Add A/V/PCR PID conflict checking on system
 * data and userdata. Disabled userdata input if PID conflict detected.
 *
 * Hydra_Software_Devel/21   3/12/13 6:36p nilesh
 * SW7435-680: Merge to mainline
 *
 * Hydra_Software_Devel/SW7435-680/1   3/11/13 3:07p delkert
 * SW7435-680: Increase MSP count to allocate more TS Packets. Add
 * warnings if insufficient system data bitrate. Roll initial STC
 * snapshot forward to just prior to first media ESCR. Enable late ESCR
 * warning.
 *
 * Hydra_Software_Devel/20   3/6/13 9:49a nilesh
 * SW7425-659: Merge MTU BPP support
 *
 * Hydra_Software_Devel/SW7425-659/1   2/25/13 3:36p nilesh
 * SW7425-649: Unify insertion of metadata and desc pending lists
 *
 * Hydra_Software_Devel/19   7/3/12 6:07p delkert
 * SW7425-3415: Fix userdata startup to allow for funky timing due to B-
 * frames (only set timing adjustment on RAP)
 *
 * Hydra_Software_Devel/18   6/27/12 11:10a nilesh
 * SW7425-3076: Merge to mainline
 *
 * Hydra_Software_Devel/SW7425-3076/3   6/20/12 1:17p nilesh
 * SW7425-3076: Rebase from mainline
 *
 * Hydra_Software_Devel/17   6/15/12 1:07p delkert
 * SW7425-3214: Cleanup debug output to make it easier to correlate with
 * testing results.
 *
 * Hydra_Software_Devel/SW7425-3076/2   6/20/12 11:59a nilesh
 * SW7425-3076: Use separate heaps for system and device memory
 *
 * Hydra_Software_Devel/SW7425-3076/1   6/7/12 4:45p nilesh
 * SW7425-3076: Accumulate contiguous dropped packets
 *
 * Hydra_Software_Devel/16   4/11/12 3:47p delkert
 * SW7425-2567: Fix unused var compiler warning in release build
 *
 * Hydra_Software_Devel/15   4/5/12 9:21a delkert
 * SW7425-2567: Add TEST_MODE build option to dump CSV file of userdata
 * timing information
 *
 * Hydra_Software_Devel/14   3/27/12 3:14p delkert
 * SW7425-2598: Fix up timing values. Turn off debug workaround
 *
 * Hydra_Software_Devel/13   3/27/12 8:31a delkert
 * SW7425-2598: Refactor packet parser to better handle packets that do
 * not conform to ETSI EN 300 472
 *
 * Hydra_Software_Devel/12   3/5/12 2:43p delkert
 * SW7425-1112: Change all packet format compliance warnings to message
 * level output.
 *
 * Hydra_Software_Devel/11   3/5/12 9:37a delkert
 * SW7425-1112: Revise workaround to round num packets up to allow for
 * partial packets.
 *
 * Hydra_Software_Devel/10   3/5/12 9:19a delkert
 * SW7425-1112: Add workaround to deal with PES length less than one TS
 * packet.
 *
 * Hydra_Software_Devel/9   3/2/12 4:14p delkert
 * SW7425-1188: Add pacing of system data based on supplied timestamp
 * delta.
 *
 * Hydra_Software_Devel/8   3/2/12 8:13a delkert
 * SW7425-2271: Add dummy userdata API Get() call as a workaround for
 * message_pid2buf paired API requirement
 *
 * Hydra_Software_Devel/7   2/28/12 11:32a delkert
 * SW7425-2217: Improve detection and handling of non-conforming packets.
 * Improve debug. Cleanup.
 *
 * Hydra_Software_Devel/6   2/22/12 12:56p delkert
 * SW7425-2217: Redefine data structures to better separate settings from
 * each userdata PID. Add *temporary* workaround to allow streams with
 * bad PTS to pass thru (for testing purposes only).
 *
 * Hydra_Software_Devel/5   2/20/12 2:59p delkert
 * SW7425-2217: Add support for PES without PTS and schedule them based on
 * space availability
 *
 * Hydra_Software_Devel/4   2/17/12 4:28p delkert
 * SW7425-2217: Modify scheduler to process one PES per input until all
 * ready PES are scheduled, and to remember the last-scheduled input.
 *
 * Hydra_Software_Devel/3   2/17/12 1:19p delkert
 * SW7425-2217: Modify ESCR calc to correctly deal with mod33 wrap in PTS.
 *
 * Hydra_Software_Devel/2   2/17/12 10:03a delkert
 * SW7425-2217: Fix error in segment length calc.
 *
 * Hydra_Software_Devel/1   2/16/12 4:30p nilesh
 * SW7425-2217: Merged TS VBI Userdata insertion support
 *
 * Hydra_Software_Devel/SW7425-2217/4   2/16/12 3:38p delkert
 * SW7425-2217: Fix buffer wrap problem.  deltaPTS calc should be
 * deltaDTS. Output warnings if data dropped.
 *
 * Hydra_Software_Devel/SW7425-2217/3   2/15/12 11:42a delkert
 * SW7425-2217: Fix bug when freeing release Q free list.
 *
 * Hydra_Software_Devel/SW7425-2217/2   2/14/12 4:12p delkert
 * SW7425-2217: Only process userdata if there are active userdata inputs.
 *
 * Hydra_Software_Devel/SW7425-2217/1   2/14/12 10:56a delkert
 * SW7425-2217: Move userdata code to separate file
 *
 ***************************************************************************/

#include "bstd.h" /* also includes berr, bdbg, etc */
#include "bkni.h"

#include "bmuxlib_ts.h"
#include "bmuxlib_ts_priv.h"

BDBG_MODULE(BMUXLIB_TS_USERDATA);
BDBG_FILE_MODULE(BMUXLIB_TS_UD_PES);   /* PES parsing debug */
BDBG_FILE_MODULE(BMUXLIB_TS_UD_RQ);    /* release Q debug */
BDBG_FILE_MODULE(BMUXLIB_TS_UD_SCHED); /* scheduler debug */

/**************************
    Static Definitions
**************************/

static void SkipInputBytes(BMUXlib_TS_P_UserdataInput *pInputData, uint32_t uiByteCount);
static bool ParsePacket(BMUXlib_TS_P_UserdataInfo *pUserdataInfo);
static void QueuePacket(BMUXlib_TS_Handle hMuxTS, BMUXlib_TS_P_UserdataInfo *pUserdataInfo);
static void DropPackets(BMUXlib_TS_Handle hMuxTS, BMUXlib_TS_P_UserdataInfo *pUserdataInfo, unsigned uiNumPackets);

#ifdef TS_UD_BAD_PTS_WORKAROUND
int32_t AdjustTimingDiff(BMUXlib_TS_P_UserdataInfo *pUserdataInfo, int32_t iTimingDiff45kHz);
#endif

/*************************
*      Private API       *
*************************/

BERR_Code BMUXlib_TS_P_UserdataInit(BMUXlib_TS_Handle hMuxTS)
{
   uint32_t uiUserdataIndex;
   uint32_t uiVideoIndex;
   BMUXlib_TS_StartSettings *pstStartSettings = &hMuxTS->status.stStartSettings;

   /* verify supplied interfaces if userdata PIDs are active...*/
   for (uiUserdataIndex = 0; (uiUserdataIndex < pstStartSettings->uiNumValidUserdataPIDs) && (uiUserdataIndex < BMUXLIB_TS_MAX_USERDATA_PIDS); uiUserdataIndex++)
   {
      if ((NULL == pstStartSettings->userdata[uiUserdataIndex].stUserDataInterface.fGetUserDataBuffer)
         || (NULL == pstStartSettings->userdata[uiUserdataIndex].stUserDataInterface.fConsumeUserDataBuffer))
         return BERR_TRACE(BERR_INVALID_PARAMETER);
      hMuxTS->status.stUserdataStatus.uiNumInputs++;
   }

   /* initialize each companion video info struct */
   for (uiVideoIndex = 0; (uiVideoIndex < BMUXLIB_TS_MAX_VIDEO_PIDS) && (uiVideoIndex < pstStartSettings->uiNumValidVideoPIDs); uiVideoIndex++)
   {
      BMUXlib_TS_P_UserdataVideoInfo *pVideoInfo = &hMuxTS->status.stUserdataVideoInfo[uiVideoIndex];

      BKNI_Memset(pVideoInfo, 0, sizeof(BMUXlib_TS_P_UserdataVideoInfo));

      pVideoInfo->uiPID = pstStartSettings->video[uiVideoIndex].uiPID;
      pVideoInfo->bStartup = true;
   }

   /* for each userdata input, locate the corresponding companion video input and link them */
   for (uiUserdataIndex = 0; uiUserdataIndex < hMuxTS->status.stUserdataStatus.uiNumInputs; uiUserdataIndex++)
   {
      uint32_t uiInputIndex;
      BMUXlib_TS_P_UserdataInfo *pUserdataInfo = &hMuxTS->status.stUserdataInfo[uiUserdataIndex];

      /* NOTE: currently, we only support one video input - thus, the companion video PID must
         always be the PID of the first, and only, video input */
      /* if we support multiple video in the future, the companion video PID will come from the userdata start settings */
      uint16_t uiCompanionVideoPID = pstStartSettings->video[0].uiPID; /* pstStartSettings->userdata[uiUserdataIndex].companion_video_PID */

      BKNI_Memset(pUserdataInfo, 0, sizeof(BMUXlib_TS_P_UserdataInfo));
      pUserdataInfo->uiIndex = uiUserdataIndex;
      pUserdataInfo->pUnwrap = (uint8_t *)&hMuxTS->astUserdataUnwrap[uiUserdataIndex];

      /* find userdata companion video PID in input metadata array */
      for (uiInputIndex = 0; uiInputIndex < hMuxTS->status.uiNumInputs; uiInputIndex++)
      {
         BMUXlib_TS_P_InputMetaData *pInput = &hMuxTS->status.stInputMetaData[uiInputIndex];
         /* if this is the companion video for this userdata input, then link to it */
         if (uiCompanionVideoPID == pInput->uiPID)
         {
            pUserdataInfo->pCompanionVideo = pInput->pstUserdata;
            break;
         }
      }
   }
   return BERR_SUCCESS;
}

/*
   Determine the target PTS for the userdata that may be associated with this
   video input

   Returns flag indicating where userdata should be processed for this descriptor
   or not.
*/
bool BMUXlib_TS_P_Userdata_FindTargetPTS(BMUXlib_TS_Handle hMuxTS, BMUXlib_TS_P_InputMetaData *pInput, BMUXlib_Input_Descriptor *pstDescriptor)
{
   BMUXlib_TS_P_UserdataStatus *pStatus = &hMuxTS->status.stUserdataStatus;
   uint32_t uiOPTS = BMUXLIB_INPUT_DESCRIPTOR_ORIGINAL_PTS(pstDescriptor);
   uint32_t uiPTS45kHz = BMUXLIB_INPUT_DESCRIPTOR_PTS(pstDescriptor) >> 1;
   uint32_t uiDTS45kHz = (BMUXLIB_INPUT_DESCRIPTOR_IS_DTS_VALID(pstDescriptor))?
            (BMUXLIB_INPUT_DESCRIPTOR_DTS(pstDescriptor) >> 1):uiPTS45kHz;
   bool bProcessUserdata = false;
   BMUXlib_TS_P_UserdataVideoInfo *pVideoInfo = pInput->pstUserdata;

   BDBG_ENTER(BMUXlib_TS_P_Userdata_FindTargetPTS);

   if (NULL == pVideoInfo)
   {
      /* this input has no companion video - do not process any userdata */
      BDBG_LEAVE(BMUXlib_TS_P_Userdata_FindTargetPTS);
      return bProcessUserdata;
   }

   if ((BMUXlib_Input_Type_eVideo == BMUXLIB_INPUT_DESCRIPTOR_TYPE(pstDescriptor))
      && (0 != pStatus->uiNumInputs))
   {
      /* if this descriptor is from a video input, find the target userdata PTS
         associated with this frame */
      BDBG_ASSERT(BMUXLIB_INPUT_DESCRIPTOR_IS_PTS_VALID(pstDescriptor));
      BDBG_ASSERT(BMUXLIB_INPUT_DESCRIPTOR_IS_FRAMESTART(pstDescriptor));

      pStatus->uiCurrentVideoPID = pInput->uiPID;

      /* determine video dPTS for determining selection window */
      if (!pVideoInfo->bSelectionWindowSet)
      {
         if (pVideoInfo->bPrevDTSValid)
         {
            uint32_t uiDeltaDTS = uiDTS45kHz - pVideoInfo->uiPrevDTS45kHz;
#if defined(HUMAX_PLATFORM_BASE)    
            if(uiDeltaDTS >= 0xFFFFFF)
            {
               pVideoInfo->bPrevDTSValid = false;
               BDBG_LEAVE(BMUXlib_TS_P_Userdata_FindTargetPTS);
               return bProcessUserdata;
            }
#else
            BDBG_ASSERT(uiDeltaDTS < 0xFFFFFF); /* if this not the case, then we got an invalid DTS! */
#endif
            /* NOTE: this value should be constant - if it changes we can assume that a new metadata descriptor will occur and we can use that to reset
               the delta DTS calc - typically the blank frames will account for the dDTS calc), so the dDTS will be known before the first
               userdata is processed */

            /* FIXME: verify deltaDTS against framerate code received in metadata */
            pVideoInfo->uiDeltaDTS45kHz = uiDeltaDTS;
            BDBG_MODULE_MSG(BMUXLIB_TS_UD_PES,("dDTS (45khz) found: %x", uiDeltaDTS));
            /* NOTE: 3 dPTS used to allow for 60->24 conversion, in which up to 3 source "frames" could need to be captured per output frame */
            pVideoInfo->uiSelectionWindow = 3 * uiDeltaDTS;
            pVideoInfo->bSelectionWindowSet = true;
         }
         else
         {
            pVideoInfo->uiPrevDTS45kHz = uiDTS45kHz;
            pVideoInfo->bPrevDTSValid = true;
            pVideoInfo->uiSelectionWindow = BMUXLIB_TS_USERDATA_DEFAULT_WINDOW;
         }
      }

      if (0 == uiOPTS && true == pVideoInfo->bStartup)
      {
         /* ignore blank frames upon startup */
         bProcessUserdata = false;
      }
      else
      {
         /* NOTE: due to B-frame reordering we cannot calculate the timing adjustment on
            B-frames.  However, this code has no indication of what is a B-frame.
            Therefore, only calculate timing on a RAP thus guaranteeing consistent timing
            in the presence of B-frames.  Note also that due to startup conditions where
            we get a GOP containing blank frames, we cannot sample the FIRST RAP. */
         if (true == pVideoInfo->bStartup)
         {
            if (BMUXLIB_INPUT_DESCRIPTOR_IS_KEYFRAME(pstDescriptor))
            {
               /* determine the initial mapping between the source timing and the encoded video timing */
               pVideoInfo->uiTimingAdjust45kHz = uiOPTS - uiPTS45kHz;
               pVideoInfo->uiOrigPTS = uiOPTS;  /* save the current OPTS */
               BDBG_MODULE_MSG(BMUXLIB_TS_UD_PES,("Initial timing adjustment = %u", pVideoInfo->uiTimingAdjust45kHz));
               bProcessUserdata = true;
               pVideoInfo->bStartup = false;
            }
            else
               bProcessUserdata = false;
         }
         else
         {
            /* only calculate a new timing adjustment if the OPTS has changed */
            if ((uiOPTS != pVideoInfo->uiOrigPTS) && BMUXLIB_INPUT_DESCRIPTOR_IS_KEYFRAME(pstDescriptor))
            {
               /* determine the new mapping between the source timing and the encoded video timing */
               pVideoInfo->uiTimingAdjust45kHz = uiOPTS - uiPTS45kHz;
               pVideoInfo->uiOrigPTS = uiOPTS;  /* save the current OPTS */
               BDBG_MODULE_MSG(BMUXLIB_TS_UD_PES,("New timing adjustment = %u", pVideoInfo->uiTimingAdjust45kHz));
            }
            bProcessUserdata = true;
         }
         if (bProcessUserdata)
         {
            /* find the target PTS for this video frame in the source time domain */
            /* FIXME: This should really adjust for the reordering delay to compensate for the diff between PTS/DTS
               (since we use DTS to determine the userdata target PTS) */
            pVideoInfo->uiTargetPTS45kHz = uiDTS45kHz + pVideoInfo->uiTimingAdjust45kHz /*+ pVideoInfo->uiDeltaDTS45kHz*/;
            BDBG_MODULE_MSG(BMUXLIB_TS_UD_PES,("Target PTS (45kHz) = %u, Video PTS (45kHz) = %u", pVideoInfo->uiTargetPTS45kHz, uiPTS45kHz));
         }
      } /* end: not blank frames */
   } /* end: descriptor is video && userdata is active */

   BDBG_LEAVE(BMUXlib_TS_P_Userdata_FindTargetPTS);
   return bProcessUserdata;
}

/*
   Process Userdata PES packets from incoming TS packets

   Generate a pending list of outgoing data to be scheduled for sending to transport

   This code assumes that we are not interleaving userdata in video frames
   (only between video frames)

   NOTE: if a PES packet has no valid PTS, then the data is queued and scheduled on a space-availability
   basis.

   Scenarios:
   1) Userdata is contiguous and arrives well in advance of video data (normal case due to data pipeline)
   2) Userdata is contiguous but arrives late - we must discard the userdata (need a warning for this)
   3) Userdata has discontinuity and new timebase is in past relative to old timebase
      => need to be careful that we don't throw out any data from the new timebase thinking it is OLD!
   4) Userdata has discontinuity and new timebase is in future relative to old timebase
      => processing will wait until a new userdata PES comes into the selection window

   discontinuity handling -
      if processing packets and they're old, but no packet selected, keep all packets (could be new timebase).
      if packets old, and new packet selected, dump all packets up to selected one (all are obsolete)
      if packets are way in future and no packet selected, keep them (could be new timebase)
      if packets are way in future and new packets selected, dump future packets - outside window.

   NOTE: it is assumed that video stream from encoder will NEVER have a discontinuity.  The only obvious
   change will be sudden change in adjustment factor (OPTS will be discontinuous, but PTS/DTS will not)
*/
BERR_Code BMUXlib_TS_P_Userdata_ProcessInputs(BMUXlib_TS_Handle hMuxTS)
{
   BERR_Code rc = BERR_SUCCESS;
   BMUXlib_TS_P_UserdataStatus *pStatus = &hMuxTS->status.stUserdataStatus;
   uint32_t uiUserdataIndex;

   BDBG_ENTER(BMUXlib_TS_P_Userdata_ProcessInputs);

   /* process incoming userdata */
   for (uiUserdataIndex = 0; (uiUserdataIndex < pStatus->uiNumInputs) && (BERR_SUCCESS == rc); uiUserdataIndex++)
   {
      unsigned uiDroppedCount = 0;

      BMUXlib_TS_UserDataInterface *pInterface = &hMuxTS->status.stStartSettings.userdata[uiUserdataIndex].stUserDataInterface;
      BMUXlib_TS_P_UserdataInput stInputData;
      BMUXlib_TS_P_UserdataInfo *pUserdataInfo = &hMuxTS->status.stUserdataInfo[uiUserdataIndex];
      BMUXlib_TS_P_UserdataPacketInfo *pPacketInfo = &pUserdataInfo->stPacketInfo;
      BMUXlib_TS_P_UserdataVideoInfo *pCompanionVideo = pUserdataInfo->pCompanionVideo;

      /* currently, ensure that the userdata is ONLY processed for the first video PID - if this
         assertion fails, we need to add support for multiple video streams (companion video PIDs) to userdata start settings */
      BDBG_ASSERT(pStatus->uiCurrentVideoPID == pCompanionVideo->uiPID);

      if (pStatus->uiCurrentVideoPID != pCompanionVideo->uiPID)
         continue;   /* skip this userdata input - does not match companion video */

      if (NULL == pInterface->fGetUserDataBuffer)
      {
         BDBG_ERR(("Userdata Input[%d]: Interface not accessible ... skipping", uiUserdataIndex));
         continue;   /* skip this userdata input */
      }

      /* NOTE: initial data from this interface should always start on a TS boundary - no need to do sync search */
      if (BERR_SUCCESS != (pInterface->fGetUserDataBuffer)(pInterface->pContext, (const void **)&stInputData.pData0, &stInputData.uiBytes0, (const void **)&stInputData.pData1, &stInputData.uiBytes1))
      {
         BDBG_ERR(("Userdata Input[%d]: Unable to obtain userdata from input ... skipping", uiUserdataIndex));
         continue;   /* skip this input */
      }

      stInputData.uiBytesTotal = stInputData.uiBytes0 + stInputData.uiBytes1;
      /* skip over bytes pending (those already processed) */
      SkipInputBytes(&stInputData, pUserdataInfo->uiPendingBytes);

      /* if this input is disabled, add all bytes to release Q (i.e. ignore all data) and continue to next input */
      if (pUserdataInfo->bDisabled)
      {
         BMUXlib_TS_P_Userdata_AddToReleaseQ(hMuxTS, pUserdataInfo->uiIndex, stInputData.uiBytesTotal);
         continue;
      }

      /* process TS packets one-at-a-time extracting PES headers as necessary ... */
      /* ensure we have at least one TS packet available, and we have resources to process the userdata
         (we require one free entry for the current packet, and one free spot on the pending list
          to send that packet to transport. We can also require up to two entries in the PTS list) */
      while ((stInputData.uiBytesTotal >= BMUXLIB_TS_PACKET_LENGTH)
             && (false == BMUXlib_List_IsEmpty(hMuxTS->hUserdataPTSFreeList))
             && (false == BMUXlib_List_IsEmpty(hMuxTS->hUserdataFreeList))
             && (false == BMUXlib_List_IsFull(hMuxTS->hUserdataPendingList[uiUserdataIndex])))
      {
         uint32_t uiBytesToEnd = stInputData.uiBytes0;
         bool bDropPacket = false;

         /* make sure we have enough bytes to process the packet (i.e. packet does not split buffer boundary)
            this necessary since we don't know how much data we need to parse to locate the PES header */
         if (uiBytesToEnd < BMUXLIB_TS_PACKET_LENGTH)
         {
            /* the unwrap buffer should not be still in use by the time we get around to using it again
               (would imply buffer overflow) */
            BDBG_ASSERT(!pUserdataInfo->bUnwrapInUse);
            /* unwrap buffer into local storage.*/
            BKNI_Memcpy(pUserdataInfo->pUnwrap, stInputData.pData0, uiBytesToEnd);
            BKNI_Memcpy(pUserdataInfo->pUnwrap+uiBytesToEnd, stInputData.pData1, BMUXLIB_TS_PACKET_LENGTH-uiBytesToEnd);
            pUserdataInfo->pCurrentPacket = pUserdataInfo->pUnwrap;
         }
         else
            pUserdataInfo->pCurrentPacket = stInputData.pData0;

         if (!pPacketInfo->bParsed)
            bDropPacket = ParsePacket(pUserdataInfo);
         /* SW7425-4643: If packetInfo->uiPID is same as PCR PID, error! (we do not support PCR on same
            PID as any other data due to limitations with continuity counter)
            Also ensure that no userdata packet uses the same PID as audio or video */
         /* only verify the PID for this input ONCE */
         if (pUserdataInfo->bPIDValid)
         {
            /* check for PID change */
            if (pUserdataInfo->uiPID != pPacketInfo->uiPID)
               /* For now, just report an error */
               BDBG_ERR(("Userdata[%d]: Unexpected PID change detected (%x->%x)", uiUserdataIndex, pUserdataInfo->uiPID, pPacketInfo->uiPID));
         }
         else
         {
            /* check for PID conflict with A/V/PCR PIDs */
            if (hMuxTS->status.aFoundPIDs[pPacketInfo->uiPID])
            {
               /* PID conflict detected ... */
               if (pPacketInfo->uiPID == hMuxTS->status.stStartSettings.stPCRData.uiPID)
                  BDBG_ERR(("Userdata[%d]: Mux does not support PCR on same PID as Userdata: %x (use a separate PID for PCR)", uiUserdataIndex, pPacketInfo->uiPID));
               else
                  BDBG_ERR(("Userdata[%d]: PID %x is aleady in use by Audio or Video input", uiUserdataIndex, pPacketInfo->uiPID));
               /* disable this input */
               pUserdataInfo->bDisabled = true;
               /* drop everything related to this PID */
               bDropPacket = true;
               pUserdataInfo->bDropPES = true;
               break;
            }
            pUserdataInfo->bPIDValid = true;
            pUserdataInfo->uiPID = pPacketInfo->uiPID;
            hMuxTS->status.aFoundPIDs[pPacketInfo->uiPID] = true;
         }
         if (!bDropPacket)
         {
            if (pPacketInfo->bDTSPresent)
            {
               /* verify we have sufficient resources to process the DTS as well (we require 2 free PTS entries) */
               uint32_t uiEntriesAvail;
               BMUXlib_List_GetNumFree(hMuxTS->hUserdataPTSFreeList, &uiEntriesAvail);
               if (uiEntriesAvail < 2)
                  break;
            }
            if (pPacketInfo->bPTSPresent)
            {
               /* NOTE: The following will properly handle wrapped timing since this calc is mod 32-bits
                        (this essentially calculates the distance between Target PTS and Userdata PTS modulo 32-bits
                         it expects userdata PTS to be "ahead of" the Target PTS, and if this is not the case, then
                         a "negative" distance will result) */
               int32_t iTimingDiff45kHz = pPacketInfo->uiPTS45kHz - pCompanionVideo->uiTargetPTS45kHz;

#ifdef TS_UD_BAD_PTS_WORKAROUND
               iTimingDiff45kHz = AdjustTimingDiff(pUserdataInfo, iTimingDiff45kHz);
#endif

               BDBG_MODULE_MSG(BMUXLIB_TS_UD_PES, ("[%d]: vOPTS: %u, Target PTS: %u, UD PTS: %u (timing diff = %d)",
                     uiUserdataIndex, pCompanionVideo->uiOrigPTS, pCompanionVideo->uiTargetPTS45kHz,
                     pPacketInfo->uiPTS45kHz, iTimingDiff45kHz));

               /* is this PTS within the window for the target PTS? */
               /* FIXME: make this an adjustable window - user supplies an adjustment to slide the window up or down */
               if (iTimingDiff45kHz > 0 && (uint32_t)iTimingDiff45kHz <= pCompanionVideo->uiSelectionWindow)
               {
                  BDBG_ASSERT(bDropPacket == false);

                  /* Adjust the timing of this packet, ready for queuing to transport */
                  if (pPacketInfo->bPTSPresent)
                  {
                     uint32_t uiNewPTS45kHz = pPacketInfo->uiPTS45kHz - pCompanionVideo->uiTimingAdjust45kHz;
                     BDBG_MODULE_MSG(BMUXLIB_TS_UD_PES, ("[%d]: Accepting PES, New PTS = %u (orig PTS = %u)",
                        uiUserdataIndex, uiNewPTS45kHz, pPacketInfo->uiPTS45kHz));
#ifdef BMUXLIB_TS_P_TEST_MODE
                     {
                        FILE *hCSVFile = hMuxTS->status.stUserdataStatus.hUserdataCSV;
                        /* dump CSV containing: PID, UDindex, bPTSpresent, bDTSpresent, OPTS, ODTS, New PTS, New DTS */
                        if (!hMuxTS->status.stUserdataStatus.bUserdataCSVOpened)
                        {
                           char fname[256];
                           sprintf(fname, "BMUXlib_TS_Userdata_%2.2d.csv", hMuxTS->stCreateSettings.uiMuxId);
                           hCSVFile = fopen(fname, "w");
                           if (NULL == hCSVFile)
                              BDBG_ERR(("Error Creating Userdata Dump File (%s)", fname));
                           hMuxTS->status.stUserdataStatus.hUserdataCSV = hCSVFile;
                           hMuxTS->status.stUserdataStatus.bUserdataCSVOpened = true;
                           if (hCSVFile != NULL)
                              fprintf(hCSVFile, "index,pid,opts_45khz,new_pts_45khz,dts_present,odts_45khz,new_dts_45khz\n");
                        }
                        if (hCSVFile != NULL)
                           fprintf(hCSVFile, "%u,%u,%u,%u,%u,%u,%u\n", pUserdataInfo->uiIndex, pPacketInfo->uiPID,
                              pPacketInfo->uiPTS45kHz, uiNewPTS45kHz, pPacketInfo->bDTSPresent,
                              pPacketInfo->uiDTS45kHz, (pPacketInfo->bDTSPresent)?
                                 pPacketInfo->uiDTS45kHz - pCompanionVideo->uiTimingAdjust45kHz:0);
                     }
#endif
                     pPacketInfo->uiPTS45kHz = uiNewPTS45kHz;
                  }
                  if (pPacketInfo->bDTSPresent)
                  {
                     pPacketInfo->uiDTS45kHz -= pCompanionVideo->uiTimingAdjust45kHz;
                  }

               }
               /* else, userdata PES is outside the selection window ... */
               else if(iTimingDiff45kHz <= 0)
               {
                  /* skip this PES (old data - late PTS) */
                  /* FIXME: if we see data outside (before) window, and valid userdata has been selected, then stop processing */
                  /* FIXME: this should not dump data _unless_ a valid PES was selected - all data could correspond
                     to an earlier timebase from a future video frame (after discontinuity) */
#ifndef TS_UD_ALWAYS_SHOW_DROPPED_PACKETS
                  if (pUserdataInfo->bPacketAccepted)
#endif
                     BDBG_WRN(("[%d]: Dropping PES - Late PTS", uiUserdataIndex));
                  pUserdataInfo->bDropPES = true;  /* Drop subsequent packets belonging to this PES */
                  bDropPacket = true;
               }
               else
               {
                  /* future PES found (early PTS) ... waiting */
                  BDBG_MODULE_MSG(BMUXLIB_TS_UD_PES, ("[%d] Future PES found ... waiting", uiUserdataIndex));
                  pPacketInfo->bParsed = true;
                  break;
               }
               /* FIXME: if ALL the data processed is old data, we do nothing (it could be discontinuous from a new timebase)
                  - only discard data if an acceptable packet is subsequently found, if not, do nothing */
            }
            /* if a packet does not have a PTS, it is passed thru transparently */
         } /* end: if not drop packet */

         if (bDropPacket)
         {
            /* drop only occurs for packets that are part of a partial PES at startup
               OR if packets belong to a PES that has missed the window (late packets)
               OR for a disabled input (if the input has a bad PID) */
            uiDroppedCount++;
         }
         else
         {
            DropPackets(hMuxTS, pUserdataInfo, uiDroppedCount);
            uiDroppedCount = 0;

#ifndef TS_UD_ALWAYS_SHOW_DROPPED_PACKETS
            pUserdataInfo->bPacketAccepted = true;
#endif
            QueuePacket(hMuxTS, pUserdataInfo);
         }

         /* SW7425-4709: update CC count */
         pUserdataInfo->uiPrevCC = pPacketInfo->uiCC;
         pUserdataInfo->bPrevCCValid = true;
         /* account for this packet ... */
         SkipInputBytes(&stInputData, BMUXLIB_TS_PACKET_LENGTH);
         pUserdataInfo->uiPendingBytes += BMUXLIB_TS_PACKET_LENGTH;
         pPacketInfo->bParsed = false;
      } /* end: while packets to process & resources available */

      /* FIXME: if we get to the end of the buffer and did not select userdata, then data is all old or all future
         => for now, assert this since this should not happen */

      DropPackets(hMuxTS, pUserdataInfo, uiDroppedCount);
   } /* end: for each userdata input */

   BDBG_LEAVE(BMUXlib_TS_P_Userdata_ProcessInputs);
   return rc;
}

void BMUXlib_TS_P_Userdata_AddToReleaseQ(BMUXlib_TS_Handle hMuxTS, uint32_t uiUserdataIndex, uint32_t uiLength)
{
   BMUXlib_TS_P_UserdataInfo *pInfo = &hMuxTS->status.stUserdataInfo[uiUserdataIndex];
   BMUXlib_TS_P_UserdataReleaseQ *pReleaseQ = &pInfo->stReleaseQ;
   BMUXlib_TS_P_UserdataReleaseQEntry *pEntry;

   BDBG_ENTER(BMUXlib_TS_P_Userdata_AddToReleaseQ);

   /* fetch a new entry from the free list */
   BDBG_ASSERT(hMuxTS->pUserdataReleaseQFreeList != NULL);   /* check free list is not empty! */
   pEntry = hMuxTS->pUserdataReleaseQFreeList;
   hMuxTS->pUserdataReleaseQFreeList = pEntry->pNext;
   /* ... and add it to the release Q for this userdata input */
   pEntry->uiDataLength = uiLength;
   pEntry->uiSequenceID = pReleaseQ->uiSequenceID;
   pEntry->pNext = NULL;

   if (NULL == pReleaseQ->pHead)
      /* queue is empty, so this is the one and only node */
      pReleaseQ->pTail = pReleaseQ->pHead = pEntry;
   else
   {
      pReleaseQ->pTail->pNext = pEntry;
      pReleaseQ->pTail = pEntry;
   }
   pReleaseQ->uiSequenceID++;

   BDBG_LEAVE(BMUXlib_TS_P_Userdata_AddToReleaseQ);
}

BERR_Code BMUXlib_TS_P_Userdata_ProcessReleaseQueues(BMUXlib_TS_Handle hMuxTS)
{
   BERR_Code rc = BERR_SUCCESS;
   BMUXlib_TS_P_UserdataStatus *pStatus = &hMuxTS->status.stUserdataStatus;
   uint32_t uiUserdataIndex;

   BDBG_ENTER(BMUXlib_TS_P_Userdata_ProcessReleaseQueues);

   for (uiUserdataIndex = 0; (uiUserdataIndex < pStatus->uiNumInputs) && (BERR_SUCCESS == rc); uiUserdataIndex++)
   {
      BMUXlib_TS_P_UserdataInfo *pInfo = &hMuxTS->status.stUserdataInfo[uiUserdataIndex];
      BMUXlib_TS_P_UserdataReleaseQ *pReleaseQ = &pInfo->stReleaseQ;
      uint32_t uiTotalBytesToFree = 0;
      bool bDone = false;

      if (NULL == pReleaseQ->pHead)
         continue;         /* skip userdata release Q's that are emtpy */

      BDBG_MODULE_MSG(BMUXLIB_TS_UD_RQ, ("Userdata %d: Looking for sequence %d", uiUserdataIndex, pReleaseQ->uiExpectedSeqID));
      while (!bDone)
      {
         BMUXlib_TS_P_UserdataReleaseQEntry *pEntry = pReleaseQ->pHead;
         BMUXlib_TS_P_UserdataReleaseQEntry **ppPrevNext = &pReleaseQ->pHead;

         /* traverse the list to find an entry that matches the expected sequence count
            => this is the entry that can be freed */
         while((pEntry != NULL) && (pEntry->uiSequenceID != pReleaseQ->uiExpectedSeqID))
         {
            BDBG_MODULE_MSG(BMUXLIB_TS_UD_RQ, ("Current SeqCount: %d", pEntry->uiSequenceID));
            ppPrevNext = &pEntry->pNext;
            pEntry = pEntry->pNext;
         }
         if (pEntry != NULL)
         {
            /* descriptor to be freed found */
            BDBG_MODULE_MSG(BMUXLIB_TS_UD_RQ, ("Freeing %d bytes from Input %d, seq: %d",
               pEntry->uiDataLength, uiUserdataIndex, pReleaseQ->uiExpectedSeqID));

            uiTotalBytesToFree += pEntry->uiDataLength;
            pReleaseQ->uiExpectedSeqID++;

            /* unlink the segment and return it to the free list */
            *ppPrevNext = pEntry->pNext;
            pEntry->pNext = hMuxTS->pUserdataReleaseQFreeList;
            hMuxTS->pUserdataReleaseQFreeList = pEntry;
         }
         else
            /* descriptor for current read pointer has not been completed yet, so we wait */
            bDone = true;
      } /* end: while not done */
      if (0 != uiTotalBytesToFree)
      {
         BMUXlib_TS_UserDataInterface *pInterface = &hMuxTS->status.stStartSettings.userdata[uiUserdataIndex].stUserDataInterface;

         BDBG_MODULE_MSG(BMUXLIB_TS_UD_RQ, ("Freeing %d bytes from input %d", uiTotalBytesToFree, uiUserdataIndex));

         rc = BERR_TRACE(pInterface->fConsumeUserDataBuffer(pInterface->pContext, uiTotalBytesToFree));
         /* the following checks for corruption of release Q or pending bytes */
         BDBG_ASSERT(pInfo->uiPendingBytes >= uiTotalBytesToFree);
         pInfo->uiPendingBytes -= uiTotalBytesToFree;
         {
            /* SW7425-2271: Dummy Get() call to work around API pairing requirement in nexus API */
            BMUXlib_TS_P_UserdataInput stInputData;
            (pInterface->fGetUserDataBuffer)(pInterface->pContext, (const void **)&stInputData.pData0, &stInputData.uiBytes0, (const void **)&stInputData.pData1, &stInputData.uiBytes1);
         }
      } /* end: segments to free */
   } /* end: for each active userdata release Q */

   BDBG_LEAVE(BMUXlib_TS_P_Userdata_ProcessReleaseQueues);
   return rc;
}

/*
   this will schedule all necessary userdata packets until either all are inserted,
   there are no more to be scheduled for this time, or we run out of available space

   Scheduling is performed in a one-PES-at-a-time round-robin fashion, per input.
   Scheduling always begins with the input that was ready for processing the last time
   (to avoid favoring any input)
*/
void BMUXlib_TS_P_Userdata_SchedulePackets(BMUXlib_TS_Handle hMuxTS)
{
   BMUXlib_TS_P_UserdataStatus *pStatus = &hMuxTS->status.stUserdataStatus;
   uint32_t uiActiveInputsCount = pStatus->uiNumInputs;
   uint32_t uiUserdataIndex = pStatus->uiCurrentScheduledInput;
   uint32_t uiTSPacketsMuxed = 0;
   uint32_t uiNextESCR = hMuxTS->status.stPCRInfo.uiNextESCR & 0xFFFFFFFF;
   uint32_t uiSpaceAvailable = hMuxTS->status.stSystemDataInfo.uiPacketsUntilNextPCR;
   uint32_t uiTransportChannelIndex = hMuxTS->status.stStartSettings.stPCRData.uiTransportChannelIndex;
   bool aDoneFlags[BMUXLIB_TS_MAX_USERDATA_PIDS];

   BDBG_ENTER(BMUXlib_TS_P_Userdata_SchedulePackets);
   BKNI_Memset(aDoneFlags, 0, sizeof(aDoneFlags)); /* nothing is done yet */

   BDBG_MODULE_MSG(BMUXLIB_TS_UD_SCHED, ("Current PCR ESCR: %u, Next = %u, Packets Available = %u", (uint32_t)(hMuxTS->status.stPCRInfo.uiESCR & 0xFFFFFFFF), uiNextESCR,
      uiSpaceAvailable));

   while (0 != uiActiveInputsCount)
   {
      uint32_t i;

      /* process each input in turn, starting with the one that was ready to be processed last time */
      for (i = 0; i < pStatus->uiNumInputs; i++)
      {
         if (!aDoneFlags[uiUserdataIndex])
         {
            BMUXlib_List_Handle hPending = hMuxTS->hUserdataPendingList[uiUserdataIndex];
            if ((0 == hMuxTS->status.stSystemDataInfo.uiPacketsUntilNextPCR) &&
               (false == BMUXlib_List_IsEmpty(hPending)))
               BDBG_WRN(("Insufficient System Data bitrate to insert Userdata packets"));
            if (false == BMUXlib_List_IsEmpty(hPending) && 0 != uiSpaceAvailable)
            {
               /* there is userdata waiting for this input ... */
               BMUXlib_TS_P_UserdataPending *pUserdata;
               uint32_t uiSpaceInPendingList;
               uint32_t uiNumAvailableDescriptors;

               /* look at the pending entry ...*/
               BMUXlib_List_GetHead(hPending, (void **)&pUserdata);

               BDBG_ASSERT(0 != pUserdata->uiNumSegments);

               BMUXlib_List_GetNumFree(hMuxTS->hTransportDescriptorPendingList[uiTransportChannelIndex], &uiSpaceInPendingList);
               BMUXlib_List_GetNumEntries(hMuxTS->hTransportDescriptorFreeList, &uiNumAvailableDescriptors);

               /* ensure there is space for this packet ... */
               if ((pUserdata->uiNumSegments <= uiSpaceInPendingList)
                   && (pUserdata->uiNumSegments <= uiNumAvailableDescriptors))
               {
                  uint32_t uiNumSegments = pUserdata->uiNumSegments;
                  bool bInsertPacket = false;

                  /* determine if its time to insert this packet ... */
                  if (pUserdata->bESCRValid)
                  {
                     /* check if userdata's ESCR is <= next PCR ESCR, if so, we can fit this userdata packet in */
                     if ((int32_t)(uiNextESCR - pUserdata->uiESCR) >= 0)
                     {
                        /* ESCR is valid ... Userdata is ready for scheduling ...*/
                        BDBG_MODULE_MSG(BMUXLIB_TS_UD_SCHED, ("[%d]: ESCR %u ready", uiUserdataIndex, pUserdata->uiESCR));
                        bInsertPacket = true;
                     }
                     else
                     {
                        /* ESCR out of range ... this input is done */
                        BDBG_MODULE_MSG(BMUXLIB_TS_UD_SCHED, ("[%d]: ESCR = %u waiting", uiUserdataIndex, pUserdata->uiESCR));
                        bInsertPacket = false;
                     }
                  }
                  else
                  {
                     /* no valid ESCR for this packet, so data cannot be scheduled based on time
                        - insert on a space-availability basis */
                     BDBG_MODULE_MSG(BMUXLIB_TS_UD_SCHED, ("[%d]: Untimed Packet ready", uiUserdataIndex));
                     /* FIXME: need to monitor for possible queue overflow if there is no space available - this
                        will push back on processing, and processing will push back on userdata input causing overflow
                        if we have no space, and if pending queue has less than 5% availability, we could drop the packet */
                     bInsertPacket = true;
                  }

                  if (bInsertPacket)
                  {
                     /* schedule this packet ... */
                     BERR_Code rc;
                     uint32_t uiSegment;
                     /* remove the pending userdata packet  ... */
                     BMUXlib_List_Remove(hPending, (void **)&pUserdata);

                     /* add each segment to transport */
                     for (uiSegment = 0; uiSegment < uiNumSegments; uiSegment++)
                     {
                        BMUXlib_TS_TransportDescriptor *pDesc = NULL;
                        BMUXlib_TS_P_TransportDescriptorMetaData *pMetaDesc = NULL;

                        BDBG_MODULE_MSG(BMUXLIB_TS_UD_SCHED, ("Adding Segment[%d]: %d bytes from %p", uiSegment, pUserdata->aSegments[uiSegment].uiLength, pUserdata->aSegments[uiSegment].pData));

                        /* fetch a free descriptor and meta-descriptor */
                        rc = BERR_TRACE(BMUXlib_List_Remove(hMuxTS->hTransportDescriptorFreeList, (void **)&pDesc));
                        /* this should be successful, since we check for available descriptors up-front */
                        BDBG_ASSERT(BERR_SUCCESS == rc);

                        rc = BERR_TRACE( BMUXlib_List_Remove(hMuxTS->hTransportDescriptorMetaDataFreeList, (void **)&pMetaDesc));
                        /* this should be successful, since we check for available descriptors up-front */
                        BDBG_ASSERT(BERR_SUCCESS == rc);

                        /* Populate Transport Meta Data */
                        BKNI_Memset(pMetaDesc, 0, sizeof(BMUXlib_TS_P_TransportDescriptorMetaData));
                        pMetaDesc->eDataType = pUserdata->aSegments[uiSegment].eDataType;
                        /* the following ensures that no matter where the data is from when passed to the transport,
                           the corresponding number of bytes are released/consumed from the relevant userdata input */
                        pMetaDesc->eSourceType = BMUXlib_TS_P_SourceType_eUserdata;
                        pMetaDesc->uiInputIndex = uiUserdataIndex;

                        /* populate transport descriptor */
                        BKNI_Memset(pDesc, 0, sizeof(BMUXlib_TS_TransportDescriptor));

                        /* NOTE: userdata packets are scheduled to hardware without ESCR to allow hardware to insert them as required */
                        /* Set Packet 2 Packet Timestamp Delta */
                        pDesc->stTsMuxDescriptorConfig.bPacket2PacketTimestampDeltaValid = true;
                        pDesc->stTsMuxDescriptorConfig.uiPacket2PacketTimestampDelta = hMuxTS->status.stSystemDataInfo.uiPacket2PacketTimestampDelta;

                        /* Set Buffer Info */
                        pDesc->pBufferAddress = (void *) pUserdata->aSegments[uiSegment].pData;
                        pDesc->uiBufferLength = pUserdata->aSegments[uiSegment].uiLength;

                        rc = BERR_TRACE(BMUXlib_TS_P_AddTransportDescriptor(
                           hMuxTS,
                           uiTransportChannelIndex,
                           pDesc,
                           pMetaDesc));
                        BDBG_ASSERT(BERR_SUCCESS == rc);
                     } /* end: for each userdata PES segment */

                     /* return the userdata entry to the free list (there should always be room for it!)*/
                     rc = BERR_TRACE(BMUXlib_List_Add(hMuxTS->hUserdataFreeList, (void *)pUserdata));
                     BDBG_ASSERT(BERR_SUCCESS == rc);

                     uiSpaceAvailable--;
                     uiTSPacketsMuxed++;
                  }
                  else
                  {
                     /* No packet to insert, so this input is done ... */
                     aDoneFlags[uiUserdataIndex] = true;
                     uiActiveInputsCount--;
                  } /*end: Insert packet */
               }
               else
               {
                  /* no more space to process packet ... no more inputs can be processed
                     (come back to this input again next time) */
                  uiActiveInputsCount = 0;
                  break;
               } /* end: if space for this PES */
            }
            else
            {
               /* pending list is empty ... this input is done */
               aDoneFlags[uiUserdataIndex] = true;
               uiActiveInputsCount--;
            } /* end: if userdata waiting to be processed */
         } /* end: if input not done */

         /* go to next input to be scheduled */
         if (++uiUserdataIndex == pStatus->uiNumInputs)
            uiUserdataIndex = 0;
      } /* end: for each active input */

   } /* end: while inputs are active */

   BDBG_MODULE_MSG(BMUXLIB_TS_UD_SCHED, ("Total bytes written = %d", uiTSPacketsMuxed * BMUXLIB_TS_PACKET_LENGTH));
   /* save the current input ready for scheduling so we can come back to it next time */
   pStatus->uiCurrentScheduledInput = uiUserdataIndex;
   /* adjust system data timing to account for the userdata packets written ... */
   hMuxTS->status.stSystemDataInfo.uiESCR += (hMuxTS->status.stSystemDataInfo.uiPacket2PacketTimestampDelta * uiTSPacketsMuxed);
   /* update space available for use by system data */
   hMuxTS->status.stSystemDataInfo.uiPacketsUntilNextPCR = uiSpaceAvailable;

   BDBG_LEAVE(BMUXlib_TS_P_Userdata_SchedulePackets);
}

/*************************
     Static functions
*************************/
/*
   Skip the specified number of bytes in the input data buffer, and adjust the
   total number of bytes available
   NOTE: if all of the first segment in a wrapped-buffer scenario are skipped
   then the second segment is returned as the first (i.e. pData1 will be NULL
   and uiBytes1 will be 0)
*/
static void SkipInputBytes(BMUXlib_TS_P_UserdataInput *pInputData, uint32_t uiByteCount)
{
   /* catch a request to skip more bytes than available - indicates data corruption */
   BDBG_ASSERT(uiByteCount <= (pInputData->uiBytes0 + pInputData->uiBytes1));

   if (pInputData->uiBytes0 > uiByteCount)
   {
      pInputData->uiBytes0 -= uiByteCount;
      pInputData->pData0 += uiByteCount;
   }
   else
   {
      uint32_t uiAdjustBytes = uiByteCount - pInputData->uiBytes0;
      /* here, uiBytes1 must NOT be zero UNLESS uiAdjustBytes is zero (all bytes in
         buffer are consumed)  */
      BDBG_ASSERT(0 != pInputData->uiBytes1 || 0 == uiAdjustBytes);

      /* all data in first fragment has been processed, so return the second fragment as the first (and only) fragment ... */
      pInputData->uiBytes0 = pInputData->uiBytes1 - uiAdjustBytes;
      pInputData->pData0 = pInputData->pData1 + uiAdjustBytes;
      pInputData->uiBytes1 = 0;
      pInputData->pData1 = NULL;
   }
   pInputData->uiBytesTotal = pInputData->uiBytes0 + pInputData->uiBytes1;
}

/* parse the TS packet and determine if this has a PES header in it,
   and if so, whether PTS/DTS are present
   NOTE: This parser does not require ETSI EN 300 472 compliance
   => if a packet can be parsed, it will be parsed thru
   (it does not verify stream ID, header data length, etc).
   This parser's goal is to locate the PTS (and DTS if it exists)
   to enable retiming the packet. */
static bool ParsePacket(BMUXlib_TS_P_UserdataInfo *pUserdataInfo)
{
   BMUXlib_TS_P_UserdataPacketInfo *pPacketInfo = &pUserdataInfo->stPacketInfo;
   uint8_t *pTSPacket = pUserdataInfo->pCurrentPacket;
   bool bDropPacket = false;
   bool bDiscFlag = false;

   /* we must always be aligned to packet boundary */
   BDBG_ASSERT(pTSPacket[0] == BMUXLIB_TS_PACKET_SYNC);

   /* clear the packet info - nothing found yet!
      NOTE: this also resets bytes_processed to zero */
   BKNI_Memset(pPacketInfo, 0, sizeof(BMUXlib_TS_P_UserdataPacketInfo));

   /* read the necessary TS header bytes ...*/
   pPacketInfo->uiPID = BMUXLIB_TS_GET_PID(pTSPacket);
   pPacketInfo->bPayloadStart = BMUXLIB_TS_IS_PAYLOAD_START(pTSPacket);
   pPacketInfo->bAdaptationPresent = BMUXLIB_TS_IS_ADAPT_PRESENT(pTSPacket);
   pPacketInfo->bPayloadPresent = BMUXLIB_TS_IS_PAYLOAD_PRESENT(pTSPacket);
   pPacketInfo->uiCC = BMUXLIB_TS_GET_CC(pTSPacket);

   /* skip over the TS header */
   pPacketInfo->uiBytesProcessed += BMUXLIB_TS_HEADER_LENGTH;

   if (pPacketInfo->bAdaptationPresent)
   {
      /* process the adaptation field, to determine its length
         (we don't care about what's in it) */
      uint8_t uiAdaptationLength = BMUXLIB_TS_GET_ADAPT_LEN(pTSPacket);
      /* skip over the adaptation field */
      pPacketInfo->uiBytesProcessed += (uiAdaptationLength+1);
      if (uiAdaptationLength > 0)
         /* read disc flag for use with CC checking */
         bDiscFlag = BMUXLIB_TS_IS_DISCONTINUITY(pTSPacket);
   }
   /* SW7425-4709: Check CC for validity
      only check CC if at least one packet seen, and the packet has a payload */
   if (pUserdataInfo->bPrevCCValid && pPacketInfo->bPayloadPresent)
   {
      /* verify continuity count is incrementing properly */
      uint8_t uiExpectedCC = (pUserdataInfo->uiPrevCC+1) & 0xF;
      if (!bDiscFlag && (pPacketInfo->uiCC != uiExpectedCC))
         BDBG_WRN(("Userdata[%d]: Continuity Count Error (expecting: %x, got %x)", pUserdataInfo->uiIndex, uiExpectedCC, pPacketInfo->uiCC));
   }

   if (pPacketInfo->bPayloadPresent)
   {
      /* this TS packet has a payload ... */
#if BDBG_DEBUG_BUILD
      uint8_t uiPayloadLen = BMUXLIB_TS_PACKET_LENGTH - pPacketInfo->uiBytesProcessed;
#endif
      if (pPacketInfo->bPayloadStart)
      {
         /* This payload is the start of the PES ... process the PES header */
         uint8_t *pPES = pTSPacket + pPacketInfo->uiBytesProcessed;
         uint8_t uiStreamID;
         pUserdataInfo->bDropPES = false;    /* we do not want to drop this PES, unless determined otherwise */
         pUserdataInfo->bFirstPESSeen = true;

         /* NOTE: can't process the PES header if number of bytes remaining
            in the packet (after adaptation header) is less than the MIN header size (since the
            PTS/DTS needs to be intact to allow it to be modified;
            => we currently dont support a split PTS.
            Generally the only reason for the adaptation header is to pad a short packet,
            so it is likely that the entire PES header is present in the payload;
            generally, it has been seen that padding of long PES is typically done
            in the last PES using an adaptation header) */
         BDBG_ASSERT(uiPayloadLen >= BMUXLIB_PES_MIN_HEADER_LENGTH);

         uiStreamID = BMUXLIB_PES_GET_STREAM_ID(pPES);
         /* NOTE: it is not necessary to know the PES length - payload packets are
            passed thru transparently until the next PES header is detected
            If the whole PES is not available in the buffer, then for PES with PTS,
            it would show up in the packet selection (we would continually drop
            late packets).  For PES without PTS, the partial packets would end up
            going out at the same rate it arrived. */

         pPacketInfo->uiBytesProcessed += BMUXLIB_PES_HEADER_LENGTH;
         if (BMUXLIB_PES_HAS_OPTIONAL_FIELDS(uiStreamID))
         {
            /* process the PES flags and optional fields */
            pPacketInfo->bPTSPresent = BMUXLIB_PES_IS_PTS_PRESENT(pPES);
            pPacketInfo->bDTSPresent = BMUXLIB_PES_IS_DTS_PRESENT(pPES);
            pPacketInfo->uiBytesProcessed += BMUXLIB_PES_HEADER_FLAGS_LENGTH;

            /* NOTE: it is not necessary to know how long the header is - it is only
               used to determine the size/location of the payload, which we do not modify */
            if (!pPacketInfo->bPTSPresent && pPacketInfo->bDTSPresent)
            {
               /* should not happen - treat as if PTS only */
               BDBG_WRN(("Userdata[%d]: PES has invalid PTS/DTS flags: 01 (using DTS as PTS)", pUserdataInfo->uiIndex));
               pPacketInfo->bPTSPresent = true;
               pPacketInfo->bDTSPresent = false;
            }
            if (pPacketInfo->bPTSPresent)
               pPacketInfo->uiPTS45kHz = BMUXLIB_PES_GET_PTS(pPES) >> 1;

            if (pPacketInfo->bDTSPresent)
               /* in theory, this should not occur, but if it does we adjust the DTS also */
               pPacketInfo->uiDTS45kHz = BMUXLIB_PES_GET_DTS(pPES) >> 1;
         }
         /* else, private syntax, so pass thru as-is */
      } /* end: if payload start */
      else
      {
         /* this is a PES payload packet */
         if (pUserdataInfo->bDropPES) bDropPacket = true;   /* drop this packet of the PES */
         /* NOTE: the following is done to align to first full PES header to prevent "leakage" of PES
            that may be prior to the first accepted PES, if the PES has a PTS (since old data
            may be discarded due to startup conditions) */
         if (!pUserdataInfo->bFirstPESSeen)
         {
#ifdef TS_UD_ALWAYS_SHOW_DROPPED_PACKETS
            BDBG_WRN(("Userdata[%d]: Skipping TS Packet ... no PES found", pUserdataInfo->uiIndex));
#endif
            /* ignore payloads until we see PES start */
            bDropPacket = true;
         }
      }
   } /* end: if payload */
   /* else, packets without payload will pass straight-thru */
   return bDropPacket;
}

#ifdef TS_UD_BAD_PTS_WORKAROUND
/* The following is a Workaround for badly encoded userdata that has a PTS timebase that
   is not close to the companion video timebase
   (for example, streams have been seen that are missing the high bit on the PTS).
   NOTE: It is extremely unlikely the output generated by this is playable - the timing
   delay due to initial buffering is not known so retimed PTS values will be skewed upon
   output! */
int32_t AdjustTimingDiff(BMUXlib_TS_P_UserdataInfo *pUserdataInfo, int32_t iTimingDiff45kHz)
{
   int32_t iAdjustedTimingDiff = 0;

   iAdjustedTimingDiff = iTimingDiff45kHz-pUserdataInfo->uiTimingOffset45kHz;

   if ((iAdjustedTimingDiff > (10*45000)) || (iAdjustedTimingDiff < -(10*45000)))
   {
      /* check if this is startup condition and no offset is in place ... */
      if (!pUserdataInfo->bPacketAccepted &&(0 == pUserdataInfo->uiTimingOffset45kHz))
      {
         /* shift the timing diff to put it just outside the window */
         if (iAdjustedTimingDiff > (10*45000))
         {
            /* if current diff is way in the future, then shift it to be slightly in the future */
            pUserdataInfo->uiTimingOffset45kHz = iTimingDiff45kHz + 56000;
         }
         else
         {
            /* if current diff is way in the past, then shift to be slighty in the past */
            pUserdataInfo->uiTimingOffset45kHz = iTimingDiff45kHz - 56000;
         }
         BDBG_WRN(("[%d]: Adjusting timing to compensate for skewed timebase - using offset: %x",
            pUserdataInfo->uiIndex, pUserdataInfo->uiTimingOffset45kHz));
      }
      else
      {
         /* not startup... */
         /* check to see if we need to turn it off again due to discontinuity */
         if (0 != pUserdataInfo->uiTimingOffset45kHz)
         {
            BDBG_WRN(("[%d]: Removing timing offset", pUserdataInfo->uiIndex));
            pUserdataInfo->uiTimingOffset45kHz = 0;
         }
      }
   }
   iTimingDiff45kHz -= pUserdataInfo->uiTimingOffset45kHz;
   return iTimingDiff45kHz;
}
#endif /* TS_UD_BAD_PTS_WORKAROUND */

/* put the specified packet in the pending queue, breaking it into the necessary pieces to
   allow updated PTS and DTS as required.
*/
static void QueuePacket(BMUXlib_TS_Handle hMuxTS, BMUXlib_TS_P_UserdataInfo *pUserdataInfo)
{
   BERR_Code rc;
   BMUXlib_TS_P_UserdataPacketInfo *pPacketInfo = &pUserdataInfo->stPacketInfo;
   uint8_t *pTSPacket = pUserdataInfo->pCurrentPacket;
   bool bLocal = (pTSPacket == pUserdataInfo->pUnwrap);
   BMUXlib_TS_P_DataType eDataType = (bLocal)?BMUXlib_TS_P_DataType_eUserdataLocal:BMUXlib_TS_P_DataType_eCDB;
   BMUXlib_TS_P_UserdataPending *pEntry;

   /* queue the packet for transport (pTSPacket points to the start of the packet) */
   /* NOTE: here, bytes processed indicates the bytes to the location of the PTS (if any)
      (allows calc of bytes in each segment) */

   if (bLocal)
      /* the following is cleared when the packet is freed - for error checking */
      pUserdataInfo->bUnwrapInUse = true;

   rc = BMUXlib_List_Remove(hMuxTS->hUserdataFreeList, (void **)&pEntry);
   /* this should always be successful, since we check for available entries */
   BDBG_ASSERT(BERR_SUCCESS == rc);
   /* clear the entry (ESCR invalid, num segments = 0) */
   BKNI_Memset(pEntry, 0, sizeof(BMUXlib_TS_P_UserdataPending));

   if (pPacketInfo->bPTSPresent)
   {
      uint8_t *pPTS = NULL;
      uint8_t *pDTS = NULL;
      uint32_t uiESCR;
      uint32_t uiAdjustedPTS45kHz = pPacketInfo->uiPTS45kHz;
      uint8_t uiInitialLength = 0;     /* length of the data up to and including PTS/DTS */
      uint64_t uiNewPTS = pPacketInfo->uiPTS45kHz << 1;
      BMUXlib_TS_P_UserdataVideoInfo *pCompanionVideo = pUserdataInfo->pCompanionVideo;



      /* calculate the ESCR for this PES */
      /* ESCR => New PTS for the userdata - 2 dPTS */
      if (pCompanionVideo->bSelectionWindowSet)
      {
         uiAdjustedPTS45kHz -= (2 * pCompanionVideo->uiDeltaDTS45kHz);
      }
      else
      {
         /* use an approximate value of 50ms
            (this is unlikely to happen) */
         uiAdjustedPTS45kHz -= (50 * 45);
      }
      /* when pacing the packets to transport, the ESCR is set only for the
         first packet that has a PES header => all others would not have an ESCR.
         All subsequent packets would then be delayed by the PES header packet
         when the packets are scheduled */
      uiESCR = ((uiAdjustedPTS45kHz << 1) * BMUXLIB_TS_SCALE_PTS_TO_ESCR) & 0xFFFFFFFF;
      BDBG_MODULE_MSG(BMUXLIB_TS_UD_PES, ("[%d]: Packet with PTS (%u), PES ESCR: %u ",
            pUserdataInfo->uiIndex, pPacketInfo->uiPTS45kHz, uiESCR));

      pEntry->bESCRValid = true;
      pEntry->uiESCR = uiESCR;

      /* First segment is packet from start up to beginning of PTS */
      pEntry->aSegments[pEntry->uiNumSegments].pData = pTSPacket;
      pEntry->aSegments[pEntry->uiNumSegments].uiLength = pPacketInfo->uiBytesProcessed;
      uiInitialLength += pPacketInfo->uiBytesProcessed;
      pEntry->aSegments[pEntry->uiNumSegments].eDataType = eDataType;
      pEntry->uiNumSegments++;

      /* obtain a PTS entry for this packet */
      rc = BMUXlib_List_Remove(hMuxTS->hUserdataPTSFreeList, (void **)&pPTS);
      /* we know this must succeed since we checked for available entries before entering the loop */
      BDBG_ASSERT(BERR_SUCCESS == rc);

      /* write updated PTS ...*/
      BMUXLIB_TS_USERDATA_SET_PTS_DTS(pPTS, uiNewPTS);
      /* Second segment is the PTS */
      pEntry->aSegments[pEntry->uiNumSegments].pData = pPTS;
      pEntry->aSegments[pEntry->uiNumSegments].uiLength = BMUXLIB_PES_PTS_LENGTH;
      uiInitialLength += BMUXLIB_PES_PTS_LENGTH;
      pEntry->aSegments[pEntry->uiNumSegments].eDataType = BMUXlib_TS_P_DataType_eUserdataPTS;
      pEntry->uiNumSegments++;

      if (pPacketInfo->bDTSPresent)
      {
         uint64_t uiNewDTS = pPacketInfo->uiDTS45kHz << 1;
         /* obtain a PTS entry for the DTS */
         rc = BMUXlib_List_Remove(hMuxTS->hUserdataPTSFreeList, (void **)&pDTS);
         /* we know this must succeed since we checked for available entries before entering the loop */
         BDBG_ASSERT(BERR_SUCCESS == rc);
         /* write updated DTS ...*/
         BMUXLIB_TS_USERDATA_SET_PTS_DTS(pDTS, uiNewDTS);
         pEntry->aSegments[pEntry->uiNumSegments].pData = pDTS;
         pEntry->aSegments[pEntry->uiNumSegments].uiLength = BMUXLIB_PES_PTS_LENGTH;
         uiInitialLength += BMUXLIB_PES_PTS_LENGTH;
         pEntry->aSegments[pEntry->uiNumSegments].eDataType = BMUXlib_TS_P_DataType_eUserdataPTS;
         pEntry->uiNumSegments++;
      }

      /* Last chunk is the data after the PTS/DTS */
      BDBG_ASSERT(uiInitialLength <= BMUXLIB_TS_PACKET_LENGTH);
      pEntry->aSegments[pEntry->uiNumSegments].pData = pTSPacket + uiInitialLength;
      pEntry->aSegments[pEntry->uiNumSegments].uiLength = BMUXLIB_TS_PACKET_LENGTH - uiInitialLength;
      pEntry->aSegments[pEntry->uiNumSegments].eDataType = eDataType;
      pEntry->uiNumSegments++;
   }
   else
   {
      /* there is no PTS in this packet, so that packet goes thru as-is
         (insert it without a valid ESCR and let the scheduler insert it
         on a space-availability basis */
      BDBG_MODULE_MSG(BMUXLIB_TS_UD_PES, ("[%d]: Packet without PTS", pUserdataInfo->uiIndex));
      pEntry->aSegments[0].pData = pTSPacket;
      pEntry->aSegments[0].uiLength = BMUXLIB_TS_PACKET_LENGTH;
      pEntry->aSegments[0].eDataType = eDataType;
      pEntry->uiNumSegments = 1;
   }

   /* add the entry to the pending queue for this packet */
   rc = BMUXlib_List_Add(hMuxTS->hUserdataPendingList[pUserdataInfo->uiIndex], pEntry);
   /* we know there is room for the entry ... */
   BDBG_ASSERT(BERR_SUCCESS == rc);
}

/* Drop/Ignore the packet
   Typically, this will only occur when:
   - dropping partial PES packets prior the the first PES header (startup condition)
   - dropping late PES packets that missed their selection window
*/
static void DropPackets(BMUXlib_TS_Handle hMuxTS, BMUXlib_TS_P_UserdataInfo *pUserdataInfo, unsigned uiNumPackets)
{
   unsigned uiBytesToDrop = BMUXLIB_TS_PACKET_LENGTH*uiNumPackets;
   /* skip this TS Packet; not required */
   if ( 0 != uiBytesToDrop )
   {
      BMUXlib_TS_P_Userdata_AddToReleaseQ(hMuxTS, pUserdataInfo->uiIndex, uiBytesToDrop);
   }
}

/*****************************************************************************
* EOF
******************************************************************************/
