/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmuxlib_output.h $
 * $brcm_Revision: Hydra_Software_Devel/7 $
 * $brcm_Date: 1/9/12 3:45p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/syslib/muxlib/noarch/bmuxlib_output.h $
 * 
 * Hydra_Software_Devel/7   1/9/12 3:45p delkert
 * SW7425-2121: change create settings so interfaces are copied, not
 * pointers. Modify create setting defaults and error checking to match.
 *
 * Hydra_Software_Devel/6   11/30/11 4:14p nilesh
 * SW7425-324: Added scatter/gather file I/O support
 *
 * Hydra_Software_Devel/5   11/28/11 11:02a delkert
 * SW7425-1494: Fix up AddNewDescriptor() and SetCurrentOffset() to return
 * error if invalid eOffsetFrom specified.  Add appropriate assertions
 * for handle.
 *
 * Hydra_Software_Devel/4   11/9/11 4:16p delkert
 * SW7425-1713: Add "offset from" field to descriptor.
 * Added OutputID to uniquely identify output when multiple outputs used
 * Change GetCurrentOffset() to return uint64_t
 * Add GetEndOffset() and SetCurrentOffset() APIs
 * Add Usage debug
 * Change Read/Write/Queued offsets to "index"
 *
 * Hydra_Software_Devel/3   9/1/11 12:03p nilesh
 * SW7425-720: Added BMUXlib_Output_GetCurrentOffset()
 *
 * Hydra_Software_Devel/2   8/12/11 10:16a nilesh
 * SW7425-936: Added IsDescriptorPendingCompletion and
 * IsDescriptorPendingQueue calls
 *
 * Hydra_Software_Devel/1   5/11/11 11:56a nilesh
 * SW7425-447: ASF Mux
 *
 ***************************************************************************/

#ifndef BMUXLIB_OUTPUT_H_
#define BMUXLIB_OUTPUT_H_

#include "bmuxlib_file.h"

#ifdef __cplusplus
extern "C" {
#if 0
}
#endif
#endif

typedef enum
{
   BMUXlib_Output_OffsetReference_eStart = 0,   /* absolute offset (default) */
   BMUXlib_Output_OffsetReference_eEnd,         /* location relative to the end, i.e. append data to the end */
   BMUXlib_Output_OffsetReference_eCurrent      /* location to access is relative to current location */
} BMUXlib_Output_OffsetReference;

/* The following is the descriptors provided to the output module by the mux */
typedef struct
{
   bool bWriteOperation;            /* false = read operation */
   void *pBufferAddress;            /* pointer to the data to write, or buffer to read into */
   /* NOTE: a zero length *is* supported and must NOT alter the storage (a NOP) */
   size_t uiLength;                 /* length in Bytes of the data to read/write  */
   uint64_t uiOffset;               /* the offset within the output stream to read from/write to. */
   BMUXlib_Output_OffsetReference eOffsetFrom;  /* position offset is relative to */
} BMUXlib_Output_StorageDescriptor;

typedef struct BMUXlib_Output_Context *BMUXlib_Output_Handle;

typedef union BMUXlib_Output_Descriptor
{
      BMUXlib_Output_StorageDescriptor stStorage;
} BMUXlib_Output_Descriptor;

typedef void (*BMUXlib_Output_CallbackFunction)( void *pPrivateData, const BMUXlib_Output_Descriptor *pOutputDescriptor );

typedef struct BMUXlib_Output_CompletedCallbackInfo
{
      void *pCallbackData;
      BMUXlib_Output_CallbackFunction pCallback;
} BMUXlib_Output_CompletedCallbackInfo;

typedef struct BMUXlib_Output_CreateSettings
{
      uint32_t uiSignature;

      size_t uiCount;
      BMUXlib_StorageObjectInterface stStorage;
      uint32_t uiOutputID;          /* used to uniquely identify this output when multiple outputs are used */
} BMUXlib_Output_CreateSettings;

/****************************
*   P R O T O T Y P E S     *
****************************/
void
BMUXlib_Output_GetDefaultCreateSettings(
         BMUXlib_Output_CreateSettings *pstSettings
         );

BERR_Code
BMUXlib_Output_Create(
         BMUXlib_Output_Handle *phOutput,
         const BMUXlib_Output_CreateSettings *pstSettings
         );

BERR_Code
BMUXlib_Output_Destroy(
         BMUXlib_Output_Handle hOutput
         );

bool
BMUXlib_Output_IsSpaceAvailable(
         BMUXlib_Output_Handle hOutput
         );

bool
BMUXlib_Output_IsDescriptorPendingCompletion(
         BMUXlib_Output_Handle hOutput
         );

bool
BMUXlib_Output_IsDescriptorPendingQueue(
         BMUXlib_Output_Handle hOutput
         );

BERR_Code
BMUXlib_Output_AddNewDescriptor(
         BMUXlib_Output_Handle hOutput,
         BMUXlib_Output_Descriptor *pstDescriptor,
         BMUXlib_Output_CompletedCallbackInfo *pstCompletedCallbackInfo
         );

BERR_Code
BMUXlib_Output_ProcessNewDescriptors(
         BMUXlib_Output_Handle hOutput
         );

BERR_Code
BMUXlib_Output_ProcessCompletedDescriptors(
         BMUXlib_Output_Handle hOutput
         );

uint64_t
BMUXlib_Output_GetCurrentOffset(
         BMUXlib_Output_Handle hOutput
         );

uint64_t
BMUXlib_Output_GetEndOffset(
         BMUXlib_Output_Handle hOutput
         );

BERR_Code
BMUXlib_Output_SetCurrentOffset(
         BMUXlib_Output_Handle hOutput,
         uint64_t uiOffset,
         BMUXlib_Output_OffsetReference eOffsetFrom
         );

#ifdef __cplusplus
}
#endif

#endif /* BMUXLIB_OUTPUT_H_ */

/*****************************************************************************
* EOF
******************************************************************************/
