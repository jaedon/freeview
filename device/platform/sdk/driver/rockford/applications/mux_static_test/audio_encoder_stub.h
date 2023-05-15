/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: audio_encoder_stub.h $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 5/25/11 1:44p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/applications/mux_static_test/audio_encoder_stub.h $
 * 
 * Hydra_Software_Devel/3   5/25/11 1:44p delkert
 * SW7425-634: Add API for stop encode, and insert EOS descriptor into
 * output stream when done.
 *
 * Hydra_Software_Devel/2   4/6/11 12:58p delkert
 * SW7425-73: Add initial audio encoder stub for audio static test
 *
 * Hydra_Software_Devel/1   9/28/10 4:53p nilesh
 * SW7425-38: Added audio encoder stub
 *
 ***************************************************************************/

#ifndef AUDIO_ENCODER_STUB_H_
#define AUDIO_ENCODER_STUB_H_

#ifdef __cplusplus
extern "C" {
#if 0
}
#endif
#endif

/* Audio Encoder Stub */
typedef struct AudioEncoderStubContext* AudioEncoderHandle;

BERR_Code
app_GetAudioBufferDescriptors(
   AudioEncoderHandle hAudioEncoder,
   const BAVC_AudioBufferDescriptor *astDescriptors0[], /* Can be NULL if uiNumDescriptors0=0. */
   size_t *puiNumDescriptors0,
   const BAVC_AudioBufferDescriptor *astDescriptors1[], /* Needed to handle FIFO wrap. Can be NULL if uiNumDescriptors1=0. */
   size_t *puiNumDescriptors1
   );

BERR_Code
app_ConsumeAudioBufferDescriptors(
   AudioEncoderHandle hAudioEncoder,
   size_t uiNumDescriptors
   );

BERR_Code
app_GetAudioBufferStatus(
   AudioEncoderHandle hAudioEncoder,
   BAVC_AudioBufferStatus *pBufferStatus
   );

typedef struct AudioEncoderSettings
{
      BMEM_Handle hMem;
} AudioEncoderSettings;

BERR_Code
app_OpenAudioEncoder(
         AudioEncoderHandle *phAudioEncoder,
         const AudioEncoderSettings* pstSettings
         );

BERR_Code
app_CloseAudioEncoder(
         AudioEncoderHandle hAudioEncoder
         );

BERR_Code
app_StopAudioEncoder(
         AudioEncoderHandle hAudioEncoder
         );

BERR_Code
app_AudioEncoderIncrementTime(
         AudioEncoderHandle hAudiooEncoder,
         unsigned uiMilliseconds
         );

#ifdef __cplusplus
}
#endif

#endif /* AUDIO_ENCODER_STUB_H_ */
