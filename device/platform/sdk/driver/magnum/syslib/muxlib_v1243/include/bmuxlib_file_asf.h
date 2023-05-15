/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmuxlib_file_asf.h $
 * $brcm_Revision: Hydra_Software_Devel/7 $
 * $brcm_Date: 11/15/11 4:47p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/syslib/muxlib/7425/file/asf/bmuxlib_file_asf.h $
 * 
 * Hydra_Software_Devel/7   11/15/11 4:47p delkert
 * SWCHECKFRAME-114: Move accessor macros to private header, and rename
 * them to include _P_
 *
 * Hydra_Software_Devel/6   11/15/11 3:20p delkert
 * SWCHECKFRAME-114: Add accessor macros for mux state for use in testing.
 * Modify the mux to use these macros instead of directly accessing the
 * state.
 *
 * Hydra_Software_Devel/5   11/9/11 10:46a nilesh
 * SW7425-1707: Changed uiInstance/uiOutputInstance to uiMuxId
 *
 * Hydra_Software_Devel/4   11/9/11 10:01a nilesh
 * SW7425-1707: Added support for setting
 * BMUXlib_Input_CreateSettings.uiOutputInstance
 *
 * Hydra_Software_Devel/3   10/10/11 8:40a delkert
 * SW7425-1405: Add parameter checks for output interface.  Verify stream
 * numbers are not already used.
 * Add error checking on audio protocol. Stop() should return "success" if
 * executed from stopped state.
 * Set output handle to NULL after Destroy(). Add check for no output
 * object when executing DoMux() - can happen
 * if called in the initial stopped state.
 *
 * Hydra_Software_Devel/2   10/6/11 2:25p delkert
 * SW7425-1405: Moved Max Packet Length to the public API.
 *
 * Hydra_Software_Devel/1   5/11/11 11:56a nilesh
 * SW7425-447: ASF Mux
 *
 ***************************************************************************/

/* ASF File Structure
 * + Header Object
 *    + File Properties Object (1)
 *    + Header Extension Object (1)
 *    + Stream Properties Object (1+) - One per stream
 *    + Content Description Object (0-1)
 *    + Script Command Object (0-1)
 *    + Marker Object (0-1)
 * + Data Object
 * + Index Object
 *    + Simple Index Object (0+) - One per video stream
 *    + Index Object (0-1)
 *    + Media Object Index Object (0-1)
 *    + Timecode Index Object (0-1)
 */
#ifndef BMUXLIB_FILE_ASF_H_
#define BMUXLIB_FILE_ASF_H_

/* Includes */
#include "bmuxlib.h"
#include "bmuxlib_file.h"


#ifdef __cplusplus
extern "C" {
#if 0
}
#endif
#endif

/****************************
*      D E F I N E S        *
****************************/

#define BMUXLIB_FILE_ASF_MAX_VIDEO_INPUTS             1
#define BMUXLIB_FILE_ASF_MAX_AUDIO_INPUTS             1
#define BMUXLIB_FILE_ASF_MAX_PACKET_LENGTH            64*1024
#define BMUXLIB_FILE_ASF_FILE_ID_LENGTH               16

/****************************
*        T Y P E S          *
****************************/

typedef struct
{
      uint32_t uiSignature;     /* [DO NOT MODIFY] Populated by BMUXlib_File_ASF_GetDefaultCreateSettings() */

      uint32_t uiPacketLength;  /* Size of ASF packets.
                                 * A/V data payload(s) will be packetized according to this size.
                                 * Smaller sizes typically mean lower latency but worse size efficiency
                                 * */

      unsigned uiMuxId;         /* For debug: indicates the ID of this muxlib for multiple transcode systems */
} BMUXlib_File_ASF_CreateSettings;

typedef struct BMUXlib_File_ASF_VideoInput
{
      BMUXlib_VideoEncoderInterface stInterface;
      /* NOTE: for ASF compliance, Stream Numbers are range limited to 7 bits in the range 1-127. */
      uint8_t uiStreamNumber;   /* the stream number to use for this source (must NOT be zero) */
} BMUXlib_File_ASF_VideoInput;

typedef struct BMUXlib_File_ASF_AudioInput
{
      BMUXlib_AudioEncoderInterface stInterface;
      /* NOTE: for ASF compliance, Stream Numbers are range limited to 7 bits in the range 1-127. */
      uint8_t uiStreamNumber;   /* the stream number to use for this source (must NOT be zero) */
} BMUXlib_File_ASF_AudioInput;

typedef struct BMUXlib_File_ASF_StartSettings
{
      uint32_t uiSignature;     /* [DO NOT MODIFY] Populated by BMUXlib_File_ASF_GetDefaultStartSettings() */

      /*
         Input/Source Interface(s):
      */
      /* NOTE: Currently this mux only supports maximum of ONE video and ONE audio input */
      BMUXlib_File_ASF_VideoInput stVideoInputs[BMUXLIB_FILE_ASF_MAX_VIDEO_INPUTS];
      unsigned int uiNumVideoInputs;         /* currently, this can be 0 or 1 */

      BMUXlib_File_ASF_AudioInput stAudioInputs[BMUXLIB_FILE_ASF_MAX_AUDIO_INPUTS];
      unsigned int uiNumAudioInputs;         /* currently, this can be 0 or 1 */

      /*
         Output Interface:
      */
      /* NOTE: The following is currently not used - it is only present for nexus compatibility */
      BMUXlib_StorageSystemInterface stStorage; /* storage system used to obtain temporary files. OPTIONAL iff bBroadcastOnly=true */
      BMUXlib_StorageObjectInterface stOutput;  /* final destination output "file" */

      /* ASF-specific settings */
      uint8_t auiFileId[BMUXLIB_FILE_ASF_FILE_ID_LENGTH];     /* 128-bit Globally Unique Identifier (GUID) for this file */

      bool bBroadcastOnly;      /* TRUE if ASF will only be used for broadcast.
                                 *    ASF Muxer will assume output is being streams, so will NOT perform random file I/O.
                                 * FALSE if ASF file will be persistent
                                 *    ASF Muxer will post-process the file at Finish
                                 */

      /* The following are only used if bBroadcastOnly = false */
      /* TODO: Creation Date */
} BMUXlib_File_ASF_StartSettings;

typedef struct BMUXlib_File_ASF_FinishSettings
{
      uint32_t uiSignature;     /* [DO NOT MODIFY] Populated by BMUXlib_File_ASF_GetDefaultStartSettings() */

      BMUXlib_FinishMode eFinishMode;
} BMUXlib_File_ASF_FinishSettings;

/* Handle to ASF File Mux instance */
typedef struct BMUXlib_File_ASF_P_Context *BMUXlib_File_ASF_Handle;

/****************************
*   P R O T O T Y P E S     *
****************************/

/*
   Function:
      BMUXlib_File_ASF_GetDefaultCreateSettings

   Description:
      Return the default settings for the Create() API in the
      location pointed to by pCreateSettings.

   Returns:
      None
*/
void BMUXlib_File_ASF_GetDefaultCreateSettings(
            BMUXlib_File_ASF_CreateSettings  *pCreateSettings);/* [out] location to write the default create settings */

/*
   Function:
      BMUXlib_File_ASF_Create

   Description:
      Create a new "instance" of the ASF file-based software mux, using the
      settings indicated by the structure pointed to by pCreateSettings.
      Handle to the created instance returned in the location pointed to by phASFMux.
      This location will be set to NULL upon failure.
      Mux will be placed in the initial state of "stopped"

   Returns:
      BERR_SUCCESS               - Mux instance successfully created
      BERR_OUT_OF_SYSTEM_MEMORY  - no memory to allocate internal storage
      BERR_INVALID_PARAMETER     - bad Create Setting
*/
BERR_Code BMUXlib_File_ASF_Create(
            BMUXlib_File_ASF_Handle          *phASFMux,        /* [out] location to write the created ASF Mux Handle */
      const BMUXlib_File_ASF_CreateSettings  *pCreateSettings);/* [in] configuration settings */

/*
   Function:
      BMUXlib_File_ASF_Destroy

   Description:
      Deallocates heap memory utilized by this mux instance, then frees the
      instance.  The caller should set hASFMux to NULL after this API returns.

   Returns:
      None
*/
void BMUXlib_File_ASF_Destroy(
            BMUXlib_File_ASF_Handle          hASFMux);         /* [in] Handle of the mux to destroy */

/*
   Function:
      BMUXlib_File_ASF_GetDefaultStartSettings

   Description:
      Returns the default settings for the Start() API in the location
      indicated by pStartSettings.

   Returns:
      None
*/
void BMUXlib_File_ASF_GetDefaultStartSettings(
            BMUXlib_File_ASF_StartSettings  *pStartSettings);  /* [out] location to write the default start settings */

/*
   Function:
      BMUXlib_File_ASF_Start

   Description:
      Start the mux ready for processing of incoming data from the encoders
      using the configuration parameters specified in pStartSettings.  This
      call will transition the mux to the "started" state.

      Start settings define the configuration of the mux prior to starting the
      muxing process, and do not change thereafter.
      They also provide information about the encoder that is connected to the
      specific input channel, the output destination, etc.

   Returns:
      BERR_SUCCESS            - mux successfully started
      BERR_INVALID_PARAMETER  - bad start setting
      BERR_ALREADY_STARTED??
*/
BERR_Code BMUXlib_File_ASF_Start(
            BMUXlib_File_ASF_Handle          hASFMux,          /* [in] Handle of the mux */
      const BMUXlib_File_ASF_StartSettings  *pStartSettings);  /* [in] Settings to use for muxing */

/*
   Function:
      BMUXlib_File_ASF_GetDefaultFinishSettings

   Description:
      Returns the default settings for the Finish() API in the location
      indicated by pFinishSettings.

   Returns:
      None
*/
void BMUXlib_File_ASF_GetDefaultFinishSettings(
            BMUXlib_File_ASF_FinishSettings *pFinishSettings); /* [out] location to write default finish settings */

/*
   Function:
      BMUXlib_File_ASF_Finish

   Description:
      Request the Mux to finish the muxing process.
      If the finish mode is "prepare for stop", then this call will transition the mux to the
      "finishing_input" state and DoMux will continue to process remaining data and will
      subsequently transition to the "finishing_output" state.

      If the BMUXlib_File_ASF_StartSettings.bBroadcastOnly = false, then the file will also be
      finalized (i.e. writes the index and updates headers as needed)

      Finally the state will transition to the "finished" state when done.
      This is the normal mode of behavior.

      NOTE: The contents of the final output are NOT valid until the mux reaches the
      "finished" state.

   Returns:
      BERR_SUCCESS            - mux "finishing"
      BERR_INVALID_PARAMETER  - bad finish setting
      BERR_ALREADY_FINISHED??? / BAD_STATE
*/
BERR_Code BMUXlib_File_ASF_Finish(
            BMUXlib_File_ASF_Handle          hASFMux,          /* [in] handle of the mux */
      const BMUXlib_File_ASF_FinishSettings *pFinishSettings); /* [in] Settings to use to finish the mux */
/*
   Function:
      BMUXlib_File_ASF_Stop

   Description:
      Stop the mux, and return the internal state to default values
      (effectively reset everything).

      For a clean stop, this should only be called after the
      "finished" event occurs after calling BMUXlib_File_ASF_Finish()

      This function may need to be called without BMUXlib_File_ASF_Finish()
      in cases where an abrupt stop is needed.
      If this is done, then DoMux will immediately halt muxing, and
      any remaining data will be discarded (mux will move directly to "stopped" state.
      Under these conditions, the ASF will not be completed, and may be invalid.
      Immediate Stop implies hardware not available (external storage ejected, for example)
      or some other condition that needs the stop to be performed without delay.

   Returns:
      BERR_SUCCESS            - mux transitioned to "stopped" successfully
      can this fail? ALREADY_STOPPED?

*/
BERR_Code BMUXlib_File_ASF_Stop(
            BMUXlib_File_ASF_Handle          hASFMux);         /* [in] Handle of the mux */

/*
   Function:
      BMUXlib_File_ASF_DoMux

   Description:
      Main processing routine for performing the muxing operation.

      DoMux must not block - once an I/O transaction is scheduled, if there is nothing that
      can be done until the I/O completes then this must return and the domux will not be
      scheduled again until at least one I/O operation is complete (the application can monitor
      the status of the storage system passed into the mux to determine if I/O is waiting
      or not for any of the I/O streams opened for this mux)

      Returns the current state of the muxing process in the location pointed to by pStatus

      The muxing is complete when in the "finished" or "stopped" state.  The output is not
      valid unless the "finished" state is reached (i.e. transition into "stopped" means
      muxing is done, but the output is INVALID; i.e. abnormal termination).

      Note that the encoder can be "unhitched" from the mux once the "finishing_output" state
      is reached (no more input data to process) - this signifies an EOS on all input streams
      has been seen.

   Returns:
      BERR_SUCCESS
      BERR_BAD_STATE
      Other failure modes?
*/
BERR_Code BMUXlib_File_ASF_DoMux(
            BMUXlib_File_ASF_Handle          hASFMux,          /* [in] Handle of the mux */
            BMUXlib_DoMux_Status            *pStatus);         /* [out] location to write mux status */

#ifdef __cplusplus
}
#endif

#endif /* BMUXLIB_FILE_ASF_H_ */
