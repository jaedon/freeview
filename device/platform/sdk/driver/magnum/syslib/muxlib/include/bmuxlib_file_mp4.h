/***************************************************************************
 *     Copyright (c) 2010-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmuxlib_file_mp4.h $
 * $brcm_Revision: Hydra_Software_Devel/9 $
 * $brcm_Date: 1/19/12 4:25p $
 *
 ****************************************************************************
 * [File Description:]
 * API for MP4 software mux
 *
 * This mux will produce an MP4 file that is in interchange format with
 * the moov either up-front (progressive-download compatible) or with the
 * moov at the end (if reduced run-time-overhead is required).
 *
 * Notes:
 * Creation of the progressive-download compatible
 * content may incur a large overhead during the finalization of the file
 * during the transition from finishing_input state to finished state.
 *
 * There are no mux settings for MP4 that can change during the muxing process.
 *
 * The "file" based mux has no concept of real time vs non-real-time.
 * The requirement is that processing of I/O must be done fast enough to
 * ensure that encoder output does not overflow - regardless of whether
 * encoder is running RT or AFAP/NRT
 *
 * Thus the "file"-based mux output has no timing restrictions and as such
 * scheduling of the muxing process (DoMux) is based on I/O completion on
 * the output (including any temporary storage)
 *
 * For MP4, a "sample" is typically a video frame or an audio frame.
 *
 ****************************************************************************
 * [Revision History:]
 * $brcm_Log: /magnum/syslib/muxlib/7425/file/mp4/bmuxlib_file_mp4.h $
 * 
 * Hydra_Software_Devel/9   1/19/12 4:25p delkert
 * SW7425-1378: Refactor to use common muxlib input module. Remove support
 * for stream trucation for offset correction (edit list always used).
 * Remove stall timeout. Remove workarounds for SW7425-357 and SW7425-
 * 624.
 *
 * Hydra_Software_Devel/8   11/14/11 4:08p delkert
 * SW7425-1379: Refactor to use common output module:
 * Add Release Qs & callback data to support out-of-order "completion".
 * Remove uiNumActiveInputs (use interface to ecide if input is acive).
 * Split box processing so that only one descriptor output at-a-time.
 * Add cleanup if any output fails creation.
 * App "blocked" status is now dependent only on storage.
 *
 * Hydra_Software_Devel/7   11/9/11 11:11a nilesh
 * SW7425-1707: Added BMUXlib_File_MP4_CreateSettings.uiMuxId
 *
 * Hydra_Software_Devel/6   9/14/11 11:39a delkert
 * SW7425-1282: Add input stall timeout to start setting.
 *
 * Hydra_Software_Devel/5   6/1/11 5:09p delkert
 * SW7425-377: Add relocation buffer for mdat relocation/progressive
 * download support
 *
 * Hydra_Software_Devel/4   5/20/11 1:38p delkert
 * SW7425-491: Add start settings flag to select edit list vs stream
 * truncation for lip-sync adjustment
 *
 * Hydra_Software_Devel/3   5/4/11 1:50p delkert
 * SW7425-300: Fix assert if Box Buffer size set less than peak usage.
 * Minor cleanup of Box Buffer debug. Minor cleanup of API descriptions.
 *
 * Hydra_Software_Devel/2   4/6/11 9:11p delkert
 * SW7425-73: Modifications to match Storage API changes
 *
 * Hydra_Software_Devel/1   4/4/11 12:53p delkert
 * SW7425-73: Merged mp4_mux_devel to mainline
 *
 * Hydra_Software_Devel/mp4_mux_devel/9   3/30/11 5:48p delkert
 * SW7425-73: remove size of offset stack from create settings (this is
 * not user-controllable, and is dependent on box nesting)
 *
 * Hydra_Software_Devel/mp4_mux_devel/8   3/8/11 5:54p delkert
 * SW7425-73: Add storage for size fields (updates to boxes, or for NALU).
 * Unify OutputMP4Headers() and FinalizeOutput() into ProcessBoxes() to
 * allow sharing of states. Add states for moov processing.  Add initial
 * moov and mvhd creation.
 *
 * Hydra_Software_Devel/mp4_mux_devel/7   3/7/11 4:11p delkert
 * SW7425-73: Add stack for managing starting offsets of nested boxes.
 * Fix up error codes, and error handling
 *
 * Hydra_Software_Devel/mp4_mux_devel/6   2/25/11 8:34p delkert
 * SW7425-73: Remove unused start settings.  Fix up start state for header
 * processing, and exit condition if not complete.
 *
 * Hydra_Software_Devel/mp4_mux_devel/5   2/22/11 7:50p delkert
 * SW7425-73: Add metadescriptors for managing output descriptors.
 * Initial box processing to enable box creation using mux_static_test
 *
 * Hydra_Software_Devel/mp4_mux_devel/4   2/15/11 5:04p delkert
 * SW7425-73: Change API to make use of new muxlib storage interface
 *
 * Hydra_Software_Devel/mp4_mux_devel/3   2/14/11 11:06a delkert
 * SW7425-73: externally-supplied track ID needs to be 16 bits for MP4
 * compliance
 *
 * Hydra_Software_Devel/mp4_mux_devel/2   2/13/11 9:10p delkert
 * SW7425-73: Add initial API functionality, stubs and prototypes, and get
 * it to compile
 *
 * Hydra_Software_Devel/mp4_mux_devel/1   2/8/11 3:26p delkert
 * SW7425-73: Initial Version of MP4 File Mux API
 *
 ***************************************************************************/

#ifndef BMUXLIB_FILE_MP4_H__
#define BMUXLIB_FILE_MP4_H__

/* Includes */
#include "bmuxlib.h"
#include "bmuxlib_file.h"
#include "bmuxlib_output.h"

#ifdef __cplusplus
extern "C" {
#endif

/* TODO:
  [ ] Suspend/Resume APIs and operation
  NOTE: The suspend API must also cause the mux to correctly suspend if in the process of completing a "stop"
*/

/****************************
*      D E F I N E S        *
****************************/

/* signifies an unknown duration for uiExpectedDurationMs in StartSettings */
#define BMUXLIB_FILE_MP4_EXPECTED_DURATION_UNKNOWN    0

#define BMUXLIB_FILE_MP4_MAX_VIDEO_INPUTS             1
#define BMUXLIB_FILE_MP4_MAX_AUDIO_INPUTS             1

/****************************
*        T Y P E S          *
****************************/

typedef struct
{
   uint32_t uiSignature;                     /* [DO NOT MODIFY] Populated by BMUXlib_File_MP4_GetDefaultCreateSettings() */

   size_t uiNumOutputStorageDescriptors;     /* the number of descriptors for passing data to storage for non-metadata outputs */
   size_t uiNumMetadataEntriesCached;        /* the number of "samples" stored in memory before being flushed to external storage */
   size_t uiBoxHeapSizeBytes;                /* space for allocating temporary storage for boxes created during moov creation */
   size_t uiNumSizeEntries;                  /* the number of entries in the size storage */
   size_t uiRelocationBufferSizeBytes;       /* space for relocating the mdat if progressive download support is required */

   unsigned uiMuxId;                         /* For debug: indicates the ID of this muxlib for multiple transcode systems */
} BMUXlib_File_MP4_CreateSettings;

typedef struct
{
   BMUXlib_VideoEncoderInterface stInterface;
   /* NOTE: for MP4 compliance, Track IDs are range limited to 16 bits specifying the ES ID of the input */
   uint16_t uiTrackID;                       /* the track ID to use for this source (must NOT be zero) */
} BMUXlib_File_MP4_VideoInput;

typedef struct
{
   BMUXlib_AudioEncoderInterface stInterface;
   /* NOTE: for MP4 compliance, Track IDs are range limited to 16 bits specifying the ES ID of the input */
   uint16_t uiTrackID;                       /* the track ID to use for this source (must NOT be zero) */
} BMUXlib_File_MP4_AudioInput;

typedef struct
{
   uint32_t uiSignature;                     /* [DO NOT MODIFY] Populated by BMUXlib_File_MP4_GetDefaultStartSettings() */

   /*
      Input/Source Interface(s):
   */
   /* NOTE: Currently this mux only supports maximum of ONE video and ONE audio input */
   BMUXlib_File_MP4_VideoInput stVideoInputs[BMUXLIB_FILE_MP4_MAX_VIDEO_INPUTS];
   unsigned int uiNumVideoInputs;            /* currently, this can be 0 or 1 */

   BMUXlib_File_MP4_AudioInput stAudioInputs[BMUXLIB_FILE_MP4_MAX_AUDIO_INPUTS];
   unsigned int uiNumAudioInputs;            /* currently, this can be 0 or 1 */

   /*
      Output Interface:
   */
   BMUXlib_StorageSystemInterface stStorage; /* storage system used to obtain temporary files */
   BMUXlib_StorageObjectInterface stOutput;  /* final destination output "file" */

   /*
      General
   */
   uint32_t uiExpectedDurationMs;            /* hint from app re: duration of transcode */

   /*
      MP4-specific settings
   */
   bool bProgressiveDownloadCompatible;      /* ensure output is compatible with progressive download
                                                (moov is at the start of the file) */
   uint32_t uiCreateTimeUTC;                 /* Seconds since Midnight Jan 1st 1904 - obtained from OS - 32 bits enough for dates to 2040 */
} BMUXlib_File_MP4_StartSettings;

typedef struct
{
   uint32_t uiSignature;                     /* [DO NOT MODIFY] Populated by BMUXlib_File_MP4_GetDefaultStartSettings() */

   BMUXlib_FinishMode eFinishMode;           /* mode of finish (e.g. "finalize file" or "immediate stop", etc) */
} BMUXlib_File_MP4_FinishSettings;

/* Handle to MP4 File Mux instance */
typedef struct BMUXlib_File_MP4_P_Context *BMUXlib_File_MP4_Handle;

/****************************
*   P R O T O T Y P E S     *
****************************/

/*
   Function:
      BMUXlib_File_MP4_GetDefaultCreateSettings

   Description:
      Return the default settings for the Create() API in the
      location pointed to by pCreateSettings.

   Returns:
      None
*/
void BMUXlib_File_MP4_GetDefaultCreateSettings(
            BMUXlib_File_MP4_CreateSettings  *pCreateSettings);/* [out] location to write the default create settings */

/*
   Function:
      BMUXlib_File_MP4_Create

   Description:
      Create a new "instance" of the MP4 file-based software mux, using the
      settings indicated by the structure pointed to by pCreateSettings.

      Handle to the created instance returned in the location pointed to by phMP4Mux.
      This location will be set to NULL upon failure.

      Mux will be placed in the initial state of "stopped"

   Returns:
      BERR_SUCCESS               - Mux instance successfully created
      BERR_OUT_OF_SYSTEM_MEMORY  - no memory to allocate internal storage
      BERR_INVALID_PARAMETER     - bad Create Setting (minimum space requirements not met)
*/
BERR_Code BMUXlib_File_MP4_Create(
            BMUXlib_File_MP4_Handle          *phMP4Mux,        /* [out] location to write the created MP4 Mux Handle */
      const BMUXlib_File_MP4_CreateSettings  *pCreateSettings);/* [in] configuration settings */

/*
   Function:
      BMUXlib_File_MP4_Destroy

   Description:
      Deallocates heap memory utilized by this mux instance, then frees the
      instance.  The caller should set hMP4Mux to NULL after this API returns.

   Returns:
      None
*/
void BMUXlib_File_MP4_Destroy(
            BMUXlib_File_MP4_Handle          hMP4Mux);         /* [in] Handle of the mux to destroy */

/*
   Function:
      BMUXlib_File_MP4_GetDefaultStartSettings

   Description:
      Returns the default settings for the Start() API in the location
      indicated by pStartSettings.

   Returns:
      None
*/
void BMUXlib_File_MP4_GetDefaultStartSettings(
            BMUXlib_File_MP4_StartSettings  *pStartSettings);  /* [out] location to write the default start settings */

/*
   Function:
      BMUXlib_File_MP4_Start

   Description:
      Start the mux ready for processing of incoming data from the encoders
      using the configuration parameters specified in pStartSettings.  This
      call will transition the mux to the "started" state.

      Start settings define the configuration of the mux prior to starting the
      muxing process, and do not change thereafter.
      They also provide information about the encoder that is connected to the
      specific input channel, the output destination, etc.

   Returns:
      BERR_SUCCESS               - mux successfully started
      BERR_INVALID_PARAMETER     - bad start setting (currently no settings will generate this)
      BERR_NOT_SUPPORTED         - Start() invoked from invalid state (must be invoked
                                   from "stopped" state).
*/
BERR_Code BMUXlib_File_MP4_Start(
            BMUXlib_File_MP4_Handle          hMP4Mux,          /* [in] Handle of the mux */
      const BMUXlib_File_MP4_StartSettings  *pStartSettings);  /* [in] Settings to use for muxing */

/*
   Function:
      BMUXlib_File_MP4_GetDefaultFinishSettings

   Description:
      Returns the default settings for the Finish() API in the location
      indicated by pFinishSettings.

   Returns:
      None
*/
void BMUXlib_File_MP4_GetDefaultFinishSettings(
            BMUXlib_File_MP4_FinishSettings *pFinishSettings); /* [out] location to write default finish settings */

/*
   Function:
      BMUXlib_File_MP4_Finish

   Description:
      Request the Mux to finish the muxing process.
      If the finish mode is "prepare for stop", then this call will transition the mux to the
      "finishing_input" state and DoMux will continue to process remaining data and will
      subsequently transition to the "finishing_output" state when it finalizes the file
      (i.e. writes the moov) and finally will transition to the "finished" state when done.
      This is the normal mode of behaviour.

      NOTE: The contents of the final output are NOT valid until the mux reaches the
      "finished" state.

   Returns:
      BERR_SUCCESS            - mux "finishing"
      BERR_INVALID_PARAMETER  - bad finish setting
      BERR_NOT_SUPPORTED      - finish called in invalid state
*/
BERR_Code BMUXlib_File_MP4_Finish(
            BMUXlib_File_MP4_Handle          hMP4Mux,          /* [in] handle of the mux */
      const BMUXlib_File_MP4_FinishSettings *pFinishSettings); /* [in] Settings to use to finish the mux */

/*
   Function:
      BMUXlib_File_MP4_Stop

   Description:
      Stop the mux, and return the internal state to default values
      (effectively reset everything).  This can be called when the mux is in
      any state.

      For a clean stop, this should only be called after the
      "finished" event occurs after calling BMUXlib_File_MP4_Finish()

      This function may need to be called without BMUXlib_File_MP4_Finish()
      in cases where an abrupt stop is needed.
      If this is done, then DoMux will immediately halt muxing, and
      any remaining data will be discarded (mux will move directly to "stopped" state.
      Under these conditions, the MP4 will not be completed, and will be invalid.
      Immediate Stop implies hardware not available (external storage ejected, for example)
      or some other condition that needs the stop to be performed without delay.

   Returns:
      BERR_SUCCESS   - always successful
*/
BERR_Code BMUXlib_File_MP4_Stop(
            BMUXlib_File_MP4_Handle          hMP4Mux);         /* [in] Handle of the mux */


/*
   Events:          |  Mux states:
                    |     Stopped        Started           Finishing_Input   Finishing_Output     Finished
======================================================================================================
   Start            |     Started        Invalid           Invalid           Invalid              Invalid
   Finish           |     Invalid        Finishing_Input   Invalid           Invalid              Invalid
   Input_Done       |     Invalid        Invalid           Finishing_Output  Invalid              Invalid
   Output_Done      |     Invalid        Invalid           Invalid           Finished             Invalid
   Stop             |     (1)            Stopped           Stopped           Stopped              Stopped

   Invalid Event = Error + No change in state
   (1) A stop in the stopped state simply resets everything
*/

/*
   Function:
      BMUXlib_File_MP4_DoMux

   Description:
      Main processing routine for performing the muxing operation.

      DoMux must not block - once an I/O transaction is scheduled, if there is nothing that
      can be done until the I/O completes then this must return and the domux will not be
      scheduled again until at least one I/O operation is complete (the application can monitor
      the status of the storage system passed into the mux via the hStorage handle to determine
      if I/O is waiting or not for any of the I/O streams opened for this mux)

      Returns the current state of the muxing process in the location pointed to by pStatus

      The muxing is complete when in the "finished" or "stopped" state.  The output is not
      valid unless the "finished" state is reached.
      Note that the encoder can be "unhitched" from the mux once the "finishing_output" state
      is reached (no more input data to process) - this signifies an EOS on all input streams
      has been seen.

      It is expected that mux will be rescheduled once output is not busy (if mux quit due to
      output) or after the indicated execution time (if mux quit due to lack of input data).

   Returns:
      BERR_SUCCESS            - mux run successfully
      BERR_NOT_SUPPORTED      - bad mux state detected
*/
BERR_Code BMUXlib_File_MP4_DoMux(
            BMUXlib_File_MP4_Handle          hMP4Mux,          /* [in] Handle of the mux */
            BMUXlib_DoMux_Status            *pStatus);         /* [out] location to write mux status */

#ifdef __cplusplus
}
#endif

#endif /* BMUXLIB_FILE_MP4_H__ */

/*****************************************************************************
* EOF
******************************************************************************/
