/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmuxlib.h $
 * $brcm_Revision: Hydra_Software_Devel/17 $
 * $brcm_Date: 8/14/12 1:09p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/syslib/muxlib/noarch/bmuxlib.h $
 * 
 * Hydra_Software_Devel/17   8/14/12 1:09p nilesh
 * SW7425-3684: Added support for reporting mux duration
 * 
 * Hydra_Software_Devel/16   2/8/12 4:35p nilesh
 * SW7425-2221,SW7425-2349: Removed BMUXlib_FinishMode_eImmediate
 * 
 * Hydra_Software_Devel/15   1/31/12 12:03p nilesh
 * SW7425-2221: Added BMUXlib_FinishMode_eImmediate support
 * 
 * Hydra_Software_Devel/14   1/4/12 2:15p delkert
 * SW7425-998: Adjust state table to allow for auto-finish behaviour.
 *
 * Hydra_Software_Devel/13   6/29/11 11:39a delkert
 * SW7425-673: Add Finish Steps to DoMux_Status.
 *
 * Hydra_Software_Devel/12   6/7/11 11:06a delkert
 * SW7425-325: Add bBlockedOutput to DoMux_Status to allow indication to
 * application when waiting for storage.
 *
 * Hydra_Software_Devel/11   2/15/11 2:51p nilesh
 * SW7425-38, SW7425-1, SW7425-56: Changed
 * BMUXlib_Get[Video/Audio]BufferDescriptors() prototypes to be more
 * accurate
 *
 * Hydra_Software_Devel/10   2/9/11 11:22a nilesh
 * SW7425-38: Added "Finish" event to mux state transition table
 *
 * Hydra_Software_Devel/9   2/7/11 11:02a nilesh
 * SW7425-38: Removed unused mux mode setting
 *
 * Hydra_Software_Devel/8   2/4/11 10:37a nilesh
 * SW7425-38: Made finish mode a shared muxlib enum
 *
 * Hydra_Software_Devel/7   2/4/11 10:22a nilesh
 * SW7425-38: Added State Transition table
 *
 * Hydra_Software_Devel/6   2/3/11 4:40p nilesh
 * SW7425-38: Added buffer info struct to encoder interface(s)
 *
 * Hydra_Software_Devel/5   2/3/11 4:10p nilesh
 * SW7425-38: API Changes
 *
 * Hydra_Software_Devel/4   2/1/11 4:59p nilesh
 * SW7425-38: Added generic video/audio encoder interface definitions
 *
 * Hydra_Software_Devel/3   1/31/11 5:57p nilesh
 * SW7425-38: Changed DoMux() API
 *
 * Hydra_Software_Devel/2   9/22/10 4:03p nilesh
 * SW7425-38: include bavc.h
 *
 * Hydra_Software_Devel/1   9/20/10 4:43p nilesh
 * SW7425-38:
 * Added file element "bmuxlib.h".
 * SW7425-38:
 * Added file element "bmuxlib_ts.c".
 * Added file element "bmuxlib_ts.h".
 * SW7425-38: Merged muxlib to mainline
 *
 * Hydra_Software_Devel/SW7425-38/1   9/17/10 12:32p nilesh
 * SW7425-38: Initial Muxer interface definition
 *
 ***************************************************************************/

#ifndef BMUXLIB_H_
#define BMUXLIB_H_

#include "bavc.h"

#ifdef __cplusplus
extern "C" {
#if 0
}
#endif
#endif

/* For muxer proposal: These would be the common interface definition between a muxer
 * and the audio/video encoders
 */

typedef enum BMUXlib_FinishMode
{
      BMUXlib_FinishMode_ePrepareForStop,    /* Finishes muxing until all active CDBs are empty and EOS is seen. */

      BMUXlib_FinishMode_eMax
} BMUXlib_FinishMode;


/* State Transitions
+-----------------+-------------------------------------------------------------------------------+
|                 | Current State                                                                 |
|                 +------------+-----------------+------------------+------------------+----------+
| Event           | Stopped[1] | Started         | Finishing_Input  | Finishing_Output | Finished |
+-----------------+------------+-----------------+------------------+------------------+----------+
| Start           | Started    | Invalid         | Invalid          | Invalid          | Invalid  |
| Finish          | Invalid    | Finishing_Input | NOP              | NOP              | NOP      |
| Finished_Input  | Invalid    | Invalid         | Finishing_Output | Invalid          | Invalid  |
| Finished_Output | Invalid    | Invalid         | Invalid          | Finished         | Invalid  |
| Stop            | [3]        | Stopped[2]      | Stopped[2]       | Stopped[2]       | Stopped  |
+-----------------+------------+-----------------+------------------+------------------+----------+

Notes:
 [1] Stopped is the initial state
 [2] Muxer output may not be usable if Stop is called before the Finished state is reached
 [3] A stop in the stopped state simply resets everything
 Invalid Event = Error + No change in state
 NOP = do nothing (no state change)

*/

typedef enum BMUXlib_State
{
   BMUXlib_State_eStopped,          /* Muxing done/idle */
   BMUXlib_State_eStarted,          /* Actively muxing */
   BMUXlib_State_eFinishingInput,   /* Stop requested, still retrieving data from encoder(s) */
   BMUXlib_State_eFinishingOutput,  /* Stop requested, no longer retrieving data from encoder(s), finalizing output */
   BMUXlib_State_eFinished,

   BMUXlib_State_eMax
} BMUXlib_State;

typedef struct BMUXlib_DoMux_Status
{
   unsigned uiNextExecutionTime;    /* (in milliseconds) the maximum time before the muxer must be executed again to prevent underflow at the mux. */
   BMUXlib_State eState;
   bool bBlockedOutput;             /* indicates the mux is blocked waiting for output/storage to complete */
   unsigned uiFinishStepsTotal;     /* total number of steps required to complete the file */
   unsigned uiFinishStepsComplete;  /* number of steps completed so far (allows generation of %complete status) */
   unsigned uiCompletedDuration;    /* (in milliseconds) media duration completed so far */
} BMUXlib_DoMux_Status;

/***************************/
/* Video Encoder Interface */
/***************************/
typedef BERR_Code
(*BMUXlib_GetVideoBufferDescriptors)(
   void *pvContext,
   const BAVC_VideoBufferDescriptor **astDescriptors0, /* Pointer to an array of descriptors. E.g. *astDescriptorsX[0] is the first descriptor. *astDescriptorsX may be set to NULL iff uiNumDescriptorsX=0. */
   size_t *puiNumDescriptors0,
   const BAVC_VideoBufferDescriptor **astDescriptors1, /* Needed to handle FIFO wrap. Can be NULL if uiNumDescriptors1=0. */
   size_t *puiNumDescriptors1
   );

typedef BERR_Code
(*BMUXlib_ConsumeVideoBufferDescriptors)(
   void *pvContext,
   size_t uiNumDescriptors
   );

typedef BERR_Code
(*BMUXlib_GetVideoBufferStatus)(
   void *pvContext,
   BAVC_VideoBufferStatus *pBufferStatus
);

typedef struct BMUXlib_VideoEncoderInterface
{
      void *pContext;
      BMUXlib_GetVideoBufferDescriptors fGetBufferDescriptors;
      BMUXlib_ConsumeVideoBufferDescriptors fConsumeBufferDescriptors;
      BMUXlib_GetVideoBufferStatus fGetBufferStatus;

      BAVC_VideoBufferInfo stBufferInfo;
} BMUXlib_VideoEncoderInterface;

/***************************/
/* Audio Encoder Interface */
/***************************/
typedef BERR_Code
(*BMUXlib_GetAudioBufferDescriptors)(
   void *pvContext,
   const BAVC_AudioBufferDescriptor **astDescriptors0, /* Pointer to an array of descriptors. E.g. *astDescriptors0[0] is the first descriptor.  Can be NULL if uiNumDescriptors0=0. */
   size_t *puiNumDescriptors0,
   const BAVC_AudioBufferDescriptor **astDescriptors1, /* Needed to handle FIFO wrap. Can be NULL if uiNumDescriptors1=0. */
   size_t *puiNumDescriptors1
   );

typedef BERR_Code
(*BMUXlib_ConsumeAudioBufferDescriptors)(
   void *pvContext,
   size_t uiNumDescriptors
   );

typedef BERR_Code
(*BMUXlib_GetAudioBufferStatus)(
   void *pvContext,
   BAVC_AudioBufferStatus *pBufferStatus
   );

typedef struct BMUXlib_AudioEncoderInterface
{
      void *pContext;
      BMUXlib_GetAudioBufferDescriptors fGetBufferDescriptors;
      BMUXlib_ConsumeAudioBufferDescriptors fConsumeBufferDescriptors;
      BMUXlib_GetAudioBufferStatus fGetBufferStatus;

      BAVC_AudioBufferInfo stBufferInfo;
} BMUXlib_AudioEncoderInterface;

#ifdef __cplusplus
}
#endif

#endif /* BMUXLIB_H_ */
