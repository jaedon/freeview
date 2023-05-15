/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmuxlib_fnrt.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 8/13/12 12:27p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/syslib/muxlib/noarch/bmuxlib_fnrt.h $
 * 
 * Hydra_Software_Devel/1   8/13/12 12:27p nilesh
 * SW7425-3362: Merged FNRT support
 * 
 * Hydra_Software_Devel/SW7425-3362/5   7/25/12 1:43p nilesh
 * SW7425-3362: Updated FNRTlib API to support multiple groups per input
 * type
 * 
 * Hydra_Software_Devel/SW7425-3362/4   7/3/12 5:26p nilesh
 * SW7425-3362: Updated API
 * 
 * Hydra_Software_Devel/SW7425-3362/3   7/2/12 2:50p nilesh
 * SW7425-3362: BMUXlib_FNRT_Destroy() updated to return a void type
 * 
 * Hydra_Software_Devel/SW7425-3362/2   7/2/12 11:09a nilesh
 * SW7425-3362: Updated API proposal
 * 
 * Hydra_Software_Devel/SW7425-3362/1   6/28/12 2:33p nilesh
 * SW7425-3362: Initial API proposal
 *
 ***************************************************************************/

#ifndef BMUXLIB_FNRT_H_
#define BMUXLIB_FNRT_H_

#include "bmuxlib.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************/
/* Create/Destroy */
/******************/
typedef struct BMUXlib_FNRT_P_Context *BMUXlib_FNRT_Handle;

typedef struct BMUXlib_FNRT_CreateSettings
{
      uint32_t uiSignature; /* [DO NOT MODIFY] Populated by BMUXlib_FNRT_GetDefaultCreateSettings() */

      unsigned uiMaxNumGroups; /* uiMaxNumGroups indicates the maximum number of input groups that
                                * that are expected to be added via BMUXlib_FNRT_AddVideoInputInterface
                                */
} BMUXlib_FNRT_CreateSettings;

void
BMUXlib_FNRT_GetDefaultCreateSettings(
         BMUXlib_FNRT_CreateSettings *pCreateSettings
         );

/* BMUXlib_FNRT_Create - Allocates all system/device memory required for mux operation */
BERR_Code
BMUXlib_FNRT_Create(
         BMUXlib_FNRT_Handle *phMuxFNRT,  /* [out] TSMuxer handle returned */
         const BMUXlib_FNRT_CreateSettings *pstCreateSettings
         );

/* BMUXlib_FNRT_Destroy - Frees all system/device memory allocated */
void
BMUXlib_FNRT_Destroy(
         BMUXlib_FNRT_Handle hMuxFNRT
         );

/*******************/
/* Input Interface */
/*******************/

typedef struct BMUXlib_FNRT_InputSettings
{
   uint32_t uiSignature; /* [DO NOT MODIFY] Populated by BMUXlib_FNRT_GetDefaultInputSettings() */

   unsigned uiGroupId; /* uiGroupId indicates which group this input belongs if multiple groups exist.
                        * E.g. if multiple audio programs are being encoded using FNRT in parallel,
                        * the encoders for each program would share the same uiGroupId.  The uiGroupId
                        * does NOT need to be unique across input types.  E.g. Audio and video inputs could
                        * both have uiGroupId=0.
                        */
} BMUXlib_FNRT_InputSettings;

void
BMUXlib_FNRT_GetDefaultInputSettings(
         BMUXlib_FNRT_InputSettings *pInputSettings
         );

BERR_Code
BMUXlib_FNRT_AddVideoInputInterface(
   BMUXlib_FNRT_Handle hMuxFNRT,
   const BMUXlib_VideoEncoderInterface *pstInputInterface,
   const BMUXlib_FNRT_InputSettings *pInputSettings /* OPTIONAL - If NULL, the default input settings are used */
   );

BERR_Code
BMUXlib_FNRT_AddAudioInputInterface(
   BMUXlib_FNRT_Handle hMuxFNRT,
   const BMUXlib_AudioEncoderInterface *pstInputInterface,
   const BMUXlib_FNRT_InputSettings *pInputSettings /* OPTIONAL - If NULL, the default input settings are used */
   );

#if 0
BERR_Code
BMUXlib_FNRT_RemoveVideoInputInterface(
   BMUXlib_FNRT_Handle hMuxFNRT,
   const BMUXlib_VideoEncoderInterface *pstInputInterface
   );

BERR_Code
BMUXlib_FNRT_RemoveAudioInputInterface(
   BMUXlib_FNRT_Handle hMuxFNRT,
   const BMUXlib_AudioEncoderInterface *pstInputInterface
   );
#endif

/********************/
/* Output Interface */
/********************/

typedef struct BMUXlib_FNRT_OutputSettings
{
   uint32_t uiSignature; /* [DO NOT MODIFY] Populated by BMUXlib_FNRT_GetDefaultOutputSettings() */

   unsigned uiGroupId; /* uiGroupId indicates which group's output interface to return */
} BMUXlib_FNRT_OutputSettings;

void
BMUXlib_FNRT_GetDefaultOutputSettings(
         BMUXlib_FNRT_OutputSettings *pOutputSettings
         );

BERR_Code
BMUXlib_FNRT_GetVideoOutputInterface(
   BMUXlib_FNRT_Handle hMuxFNRT,
   BMUXlib_VideoEncoderInterface *pstOutputInterface,
   const BMUXlib_FNRT_OutputSettings *pOutputSettings /* OPTIONAL - If NULL, the default output settings are used */
   );

BERR_Code
BMUXlib_FNRT_GetAudioOutputInterface(
   BMUXlib_FNRT_Handle hMuxFNRT,
   BMUXlib_AudioEncoderInterface *pstOutputInterface,
   const BMUXlib_FNRT_OutputSettings *pOutputSettings /* OPTIONAL - If NULL, the default output settings are used */
   );

#ifdef __cplusplus
}
#endif

#endif /* BMUXLIB_FNRT_H_ */
/* End of File */
