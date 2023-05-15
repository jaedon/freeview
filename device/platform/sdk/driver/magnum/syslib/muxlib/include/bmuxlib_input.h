/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmuxlib_input.h $
 * $brcm_Revision: Hydra_Software_Devel/18 $
 * $brcm_Date: 8/14/12 3:37p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/syslib/muxlib/noarch/bmuxlib_input.h $
 * 
 * Hydra_Software_Devel/18   8/14/12 3:37p nilesh
 * SW7425-3684: Improved accuracy of reported duration
 * 
 * Hydra_Software_Devel/17   8/14/12 1:09p nilesh
 * SW7425-3684: Added support for reporting mux duration
 * 
 * Hydra_Software_Devel/16   8/13/12 12:28p nilesh
 * SW7425-3362: Merged FNRT support
 * 
 * Hydra_Software_Devel/SW7425-3362/2   7/26/12 1:40p delkert
 * SW7425-3370: Fix up descriptor dump to write input descriptor base
 * address (common descriptor no longer contains a base address)
 *
 * Hydra_Software_Devel/SW7425-3362/1   7/5/12 1:24p delkert
 * SW7425-3370: Add macros for manipulating the new descriptor
 * fields/flags
 *
 * Hydra_Software_Devel/15   5/29/12 1:49p delkert
 * SW7425-2567: Change PeekAtnextDescriptor and GetNextDescriptor APIs to
 * return bool. Make GetNextInput a void return. Add relevant assertions
 * for lack of descriptors.
 *
 * Hydra_Software_Devel/14   5/11/12 10:10a nilesh
 * SW7435-78: Added FRAME_END support
 *
 * Hydra_Software_Devel/13   2/1/12 4:23p nilesh
 * SW7425-2315: Added support for
 * BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_EMPTY_FRAME
 *
 * Hydra_Software_Devel/12   1/25/12 1:48p delkert
 * SW7425-2217: Add BMUXLIB_INPUT_DESCRIPTOR_IS_OPTS_VALID macro
 *
 * Hydra_Software_Devel/11   1/18/12 11:06a delkert
 * SW7425-1378: Add macro for IS_DATA_UNIT_START
 *
 * Hydra_Software_Devel/10   1/9/12 3:45p delkert
 * SW7425-2121: change create settings so interfaces are copied, not
 * pointers. Modify create setting defaults and error checking to match.
 *
 * Hydra_Software_Devel/9   12/20/11 1:46p nilesh
 * SW7425-1869: Merged BMUXlib_InputGroup support
 *
 * Hydra_Software_Devel/SW7425-1869/2   12/14/11 2:54p delkert
 * SW7425-1494: Fix bug in ActivateInput() when input not found in table
 *
 * Hydra_Software_Devel/SW7425-1869/1   12/9/11 8:31a delkert
 * SW7425-1869: Fix bug in DTS_VALID macro. Add metadata pointer to
 * InputCreateSettings for use by mux. Add InputGroup API. Improve error
 * checking. Modify GetNextInput to be part of InputGroup and simplify
 * sorting method. Move comparison functions from ASF and TS to make them
 * generic.
 *
 * Hydra_Software_Devel/8   11/9/11 10:46a nilesh
 * SW7425-1707: Changed uiInstance/uiOutputInstance to uiMuxId
 *
 * Hydra_Software_Devel/7   11/9/11 9:58a nilesh
 * SW7425-1707: Added uiOutputInstance to facilitate debug logging for
 * multiple transcode
 *
 * Hydra_Software_Devel/6   10/6/11 4:57p nilesh
 * SW7425-721: Added more accessor macros
 *
 * Hydra_Software_Devel/5   10/5/11 4:44p nilesh
 * SW7425-721: Added input selection logic
 *
 * Hydra_Software_Devel/4   9/16/11 11:08a nilesh
 * SW7425-1281: Added input descriptor dump support
 *
 * Hydra_Software_Devel/3   5/17/11 3:41p nilesh
 * SW7425-447: Added macros to get raw audio buffer offset/length
 *
 * Hydra_Software_Devel/2   5/16/11 3:26p nilesh
 * SW7425-447: Added eBurstMode setting.  Added frame size computation.
 *
 * Hydra_Software_Devel/1   5/11/11 11:56a nilesh
 * SW7425-447: ASF Mux
 *
 ***************************************************************************/

#ifndef BMUXLIB_INPUT_H_
#define BMUXLIB_INPUT_H_

#include "bmuxlib.h"

#ifdef __cplusplus
extern "C" {
#if 0
}
#endif
#endif

/*
** General descriptor accessor macros
*/
#define BMUXLIB_INPUT_DESCRIPTOR_TYPE(x)           ((x)->eType)
#define BMUXLIB_INPUT_DESCRIPTOR_BASE_ADDRESS(x)   ((x)->pBaseAddress)
#define BMUXLIB_INPUT_DESCRIPTOR_FRAMESIZE(x)      ((x)->uiFrameSize)

#define BMUXLIB_INPUT_DESCRIPTOR_FLAGS(x)          ((x)->descriptor.pstCommon->uiFlags)
#define BMUXLIB_INPUT_DESCRIPTOR_OFFSET(x)         ((x)->descriptor.pstCommon->uiOffset)
#define BMUXLIB_INPUT_DESCRIPTOR_LENGTH(x)         ((x)->descriptor.pstCommon->uiLength)
#define BMUXLIB_INPUT_DESCRIPTOR_ORIGINAL_PTS(x)   ((x)->descriptor.pstCommon->uiOriginalPTS)
#define BMUXLIB_INPUT_DESCRIPTOR_PTS(x)            ((x)->descriptor.pstCommon->uiPTS)
#define BMUXLIB_INPUT_DESCRIPTOR_ESCR(x)           ((x)->descriptor.pstCommon->uiESCR)
#define BMUXLIB_INPUT_DESCRIPTOR_TICKS_PER_BIT(x)  ((x)->descriptor.pstCommon->uiTicksPerBit)
#define BMUXLIB_INPUT_DESCRIPTOR_SHR(x)            ((x)->descriptor.pstCommon->iSHR)
#define BMUXLIB_INPUT_DESCRIPTOR_STC_SNAPSHOT(x)   ((x)->descriptor.pstCommon->uiSTCSnapshot)

#define BMUXLIB_INPUT_DESCRIPTOR_VIRTUAL_ADDRESS(x) ((void*)(((unsigned) (x)->pBaseAddress) + BMUXLIB_INPUT_DESCRIPTOR_OFFSET(x)))

#define BMUXLIB_INPUT_DESCRIPTOR_IS_ESCR_VALID(x)  (0 != (((x)->descriptor.pstCommon->uiFlags) & BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_ESCR_VALID))
#define BMUXLIB_INPUT_DESCRIPTOR_IS_PTS_VALID(x)   (0 != (((x)->descriptor.pstCommon->uiFlags) & BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_PTS_VALID))
#define BMUXLIB_INPUT_DESCRIPTOR_IS_TICKS_PER_BIT_VALID(x)  \
         (0 != (((x)->descriptor.pstCommon->uiFlags) & BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_TICKSPERBIT_VALID))
#define BMUXLIB_INPUT_DESCRIPTOR_IS_SHR_VALID(x)   (0 != (((x)->descriptor.pstCommon->uiFlags) & BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_SHR_VALID))
#define BMUXLIB_INPUT_DESCRIPTOR_IS_OPTS_VALID(x)  (0 != (((x)->descriptor.pstCommon->uiFlags) & BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_ORIGINALPTS_VALID))
#define BMUXLIB_INPUT_DESCRIPTOR_IS_STC_SNAPSHOT_VALID(x)   \
         (0 != (((x)->descriptor.pstCommon->uiFlags) & BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_STCSNAPSHOT_VALID))

#define BMUXLIB_INPUT_DESCRIPTOR_IS_FRAMESTART(x)  (0 != (((x)->descriptor.pstCommon->uiFlags) & BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_FRAME_START))
#define BMUXLIB_INPUT_DESCRIPTOR_IS_EOS(x)         (0 != (((x)->descriptor.pstCommon->uiFlags) & BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_EOS))
#define BMUXLIB_INPUT_DESCRIPTOR_IS_EMPTYFRAME(x)  (0 != (((x)->descriptor.pstCommon->uiFlags) & BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_EMPTY_FRAME))
#define BMUXLIB_INPUT_DESCRIPTOR_IS_FRAMEEND(x)    (0 != (((x)->descriptor.pstCommon->uiFlags) & BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_FRAME_END))
#define BMUXLIB_INPUT_DESCRIPTOR_IS_EOC(x)         (0 != (((x)->descriptor.pstCommon->uiFlags) & BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_EOC))

#define BMUXLIB_INPUT_DESCRIPTOR_IS_METADATA(x)    (0 != (((x)->descriptor.pstCommon->uiFlags) & BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_METADATA))

#define BMUXLIB_INPUT_DESCRIPTOR_IS_DTS_VALID(x) \
 ( (BMUXlib_Input_Type_eVideo == (x)->eType) ? \
         (0 != (((x)->descriptor.pstVideo->uiVideoFlags) & BAVC_VIDEOBUFFERDESCRIPTOR_FLAGS_DTS_VALID)) \
         : BMUXLIB_INPUT_DESCRIPTOR_IS_PTS_VALID(x) )

#define BMUXLIB_INPUT_DESCRIPTOR_IS_KEYFRAME(x)    \
 ( (BMUXlib_Input_Type_eVideo == (x)->eType) ? \
          (0 != (((x)->descriptor.pstVideo->uiVideoFlags) & BAVC_VIDEOBUFFERDESCRIPTOR_FLAGS_RAP)) \
          : true )   /* audio frames are always RAP points */

#define BMUXLIB_INPUT_DESCRIPTOR_DTS(x) \
  ( (BMUXlib_Input_Type_eVideo == (x)->eType) ? \
         ((x)->descriptor.pstVideo->uiDTS) \
         : (x)->descriptor.pstCommon->uiPTS )

#define BMUXLIB_INPUT_DESCRIPTOR_VIDEO_IS_DATA_UNIT_START(x)   \
        (0 != (((x)->descriptor.pstVideo->uiVideoFlags) & BAVC_VIDEOBUFFERDESCRIPTOR_FLAGS_DATA_UNIT_START))

#define BMUXLIB_INPUT_DESCRIPTOR_VIDEO_FLAGS(x)          ((x)->descriptor.pstVideo->uiVideoFlags)
#define BMUXLIB_INPUT_DESCRIPTOR_VIDEO_DTS(x)            ((x)->descriptor.pstVideo->uiDTS)
#define BMUXLIB_INPUT_DESCRIPTOR_VIDEO_DATA_UNIT_TYPE(x) ((x)->descriptor.pstVideo->uiDataUnitType)

#define BMUXLIB_INPUT_DESCRIPTOR_AUDIO_RAWOFFSET(x)      ((x)->descriptor.pstAudio->uiRawDataOffset)
#define BMUXLIB_INPUT_DESCRIPTOR_AUDIO_RAWLENGTH(x)      ((x)->descriptor.pstAudio->uiRawDataLength)
#define BMUXLIB_INPUT_DESCRIPTOR_AUDIO_DATA_UNIT_TYPE(x) ((x)->descriptor.pstAudio->uiDataUnitType)

/*
** Input Types
*/
typedef enum BMUXlib_Input_Type
{
   BMUXlib_Input_Type_eVideo,
   BMUXlib_Input_Type_eAudio,

   BMUXlib_Input_Type_eMax
} BMUXlib_Input_Type;

typedef struct BMUXlib_Input_Descriptor
{
      BMUXlib_Input_Type eType;

      union
      {
            const BAVC_CompressedBufferDescriptor *pstCommon;
            const BAVC_VideoBufferDescriptor *pstVideo;
            const BAVC_AudioBufferDescriptor *pstAudio;
      } descriptor;

      const void *pBaseAddress;
      size_t uiFrameSize;                 /* Only valid if BMUXlib_Input_CreateSettings.eBurstMode == BMUXlib_Input_BurstMode_eFrame */
} BMUXlib_Input_Descriptor;

typedef enum BMUXlib_Input_BurstMode
{
   BMUXlib_Input_BurstMode_eDescriptor,   /* Descriptors will be returned as soon as they are available.  Useful for low delay modes */
   BMUXlib_Input_BurstMode_eFrame,        /* Descriptors will be returned only when the entire frame is available.  Useful for optimizing output and/or modes where frame size is needed */

   BMUXlib_Input_BurstMode_eMax
} BMUXlib_Input_BurstMode;

typedef struct BMUXlib_Input_Status
{
   uint64_t uiInitialDTS;
   bool bInitialDTSValid;
   uint64_t uiCurrentDTS;
} BMUXlib_Input_Status;


typedef struct BMUXlib_Input_CreateSettings
{
   uint32_t uiSignature;                  /* [DO NOT MODIFY] Populated by BMUXlib_Input_GetDefaultCreateSettings() */

   BMUXlib_Input_Type eType;
   union
   {
      BMUXlib_VideoEncoderInterface stVideo;
      BMUXlib_AudioEncoderInterface stAudio;
   } interface;


   BMUXlib_Input_BurstMode eBurstMode;

   void *pMetadata;                       /* reference to usage-dependent metadata associated with this input */

   /* For debug: */
   unsigned uiMuxId;                      /* indicates the ID of the mux this input is associated with.
                                             E.g. for dual transcode, each mux "output" should have a different instance */
   unsigned uiTypeInstance;               /* indicates which input instance for this type that is accociated with the mux instance.
                                             E.g. for single transcode with multiple audio, each audio input should have a different type instance */
} BMUXlib_Input_CreateSettings;

typedef struct BMUXlib_Input_P_Context * BMUXlib_Input_Handle;

/*
** Input Group Types
*/
/* the following enum defines which of the two descriptors being compared
   is desired for input selection.
   The criteria for which input to select is determined by the implementation
   of the selector function.
   e.g. if the input with the lowest DTS is desired for selection, then the
   selector function would return "SelectA" if A.DTS < B.DTS
*/
typedef enum
{
   BMUXlib_InputGroup_SelectionResult_eSelectA, /* argument A is the desired entry */
   BMUXlib_InputGroup_SelectionResult_eSelectB  /* argument B is the desired entry */
} BMUXlib_InputGroup_SelectionResult;


/* function to choose between two input descriptors based on some
   implementation-dependent criteria */
typedef BMUXlib_InputGroup_SelectionResult (*BMUXlib_InputGroup_DescriptorSelector)(
   BMUXlib_Input_Descriptor *pstDescriptorA,
   BMUXlib_Input_Descriptor *pstDescriptorB
   );

typedef struct BMUXlib_InputGroup_CreateSettings
{
   uint32_t uiSignature;                  /* [DO NOT MODIFY] Populated by BMUXlib_InputGroup_GetDefaultCreateSettings() */

   uint32_t uiInputCount;                 /* number of inputs to add to the group */
   BMUXlib_Input_Handle *pInputTable;     /* table of handles for the inputs in the group */
} BMUXlib_InputGroup_CreateSettings;

typedef struct
{
   uint32_t uiSignature;                              /* [DO NOT MODIFY] Populated by BMUXlib_InputGroup_GetDefaultSettings() */

   bool bWaitForAllInputs;                            /* if set, input selection waits until all inputs have descriptors */
   BMUXlib_InputGroup_DescriptorSelector fSelector;   /* selector function used by GetNextInput() */
} BMUXlib_InputGroup_Settings;

typedef struct
{
   uint32_t uiNumActiveInputs;
   unsigned uiDuration; /* in milliseconds */
} BMUXlib_InputGroup_Status;

typedef struct BMUXlib_InputGroup_P_Context * BMUXlib_InputGroup_Handle;

/****************************
*   P R O T O T Y P E S     *
****************************/
/***************
   Input API
****************/
void
BMUXlib_Input_GetDefaultCreateSettings(
         BMUXlib_Input_CreateSettings *pstSettings
         );

BERR_Code
BMUXlib_Input_Create(
         BMUXlib_Input_Handle *phInput,
         const BMUXlib_Input_CreateSettings *pstSettings
         );

BERR_Code
BMUXlib_Input_GetCreateSettings(
         BMUXlib_Input_Handle hInput,
         BMUXlib_Input_CreateSettings *pstSettings
         );

BERR_Code
BMUXlib_Input_Destroy(
         BMUXlib_Input_Handle hInput
         );

BERR_Code
BMUXlib_Input_ProcessNewDescriptors(
         BMUXlib_Input_Handle hInput
         );

bool
BMUXlib_Input_IsDescriptorAvailable(
         BMUXlib_Input_Handle hInput
         );

bool
BMUXlib_Input_PeekAtNextDescriptor(
         BMUXlib_Input_Handle hInput,
         BMUXlib_Input_Descriptor *pstDescriptor
         );

bool
BMUXlib_Input_GetNextDescriptor(
         BMUXlib_Input_Handle hInput,
         BMUXlib_Input_Descriptor *pstDescriptor
         );

BERR_Code
BMUXlib_Input_ConsumeDescriptors(
         BMUXlib_Input_Handle hInput,
         size_t uiCount
         );

BERR_Code
BMUXlib_Input_GetStatus(
         BMUXlib_Input_Handle hInput,
         BMUXlib_Input_Status *pstStatus
         );


/**********************
   Input Group API
**********************/
void
BMUXlib_InputGroup_GetDefaultCreateSettings(
         BMUXlib_InputGroup_CreateSettings *pstSettings);

BERR_Code
BMUXlib_InputGroup_Create(
         BMUXlib_InputGroup_Handle *phInputGroup,
         const BMUXlib_InputGroup_CreateSettings *pstSettings
         );

void
BMUXlib_InputGroup_Destroy(
         BMUXlib_InputGroup_Handle hInputGroup
         );

void
BMUXlib_InputGroup_GetDefaultSettings(
         BMUXlib_InputGroup_Settings *pstSettings
         );

BERR_Code
BMUXlib_InputGroup_SetSettings(
         BMUXlib_InputGroup_Handle hInputGroup,
         const BMUXlib_InputGroup_Settings *pstSettings
         );

BERR_Code
BMUXlib_InputGroup_GetSettings(
         BMUXlib_InputGroup_Handle hInputGroup,
         BMUXlib_InputGroup_Settings *pstSettings
         );

BERR_Code
BMUXlib_InputGroup_ActivateInput(
         BMUXlib_InputGroup_Handle hInputGroup,
         BMUXlib_Input_Handle hInput,
         bool bActive
         );

BERR_Code
BMUXlib_InputGroup_GetStatus(
         BMUXlib_InputGroup_Handle hInputGroup,
         BMUXlib_InputGroup_Status *pstStatus
         );

/* select the next input to process based upon criteria defined by the
   supplied selector function.  The inputs are ranked based on the result
   of the selection between the available descriptor for the inputs, in order
   of decreasing "selection", and the handle of the input at the head of
   the list is returned (the item that was selected the most)
   Sets *phInput to NULL if no input ready to select (i.e. no descriptors available)
*/
void
BMUXlib_InputGroup_GetNextInput(
         BMUXlib_InputGroup_Handle hInputGroup,
         BMUXlib_Input_Handle *phInput
         );

/* pre-defined selection routines */
BMUXlib_InputGroup_SelectionResult
BMUXlib_InputGroup_DescriptorSelectLowestDTS (
         BMUXlib_Input_Descriptor *pstDescriptorA,
         BMUXlib_Input_Descriptor *pstDescriptorB
         );

BMUXlib_InputGroup_SelectionResult
BMUXlib_InputGroup_DescriptorSelectLowestESCR (
         BMUXlib_Input_Descriptor *pstDescriptorA,
         BMUXlib_Input_Descriptor *pstDescriptorB
         );

#ifdef __cplusplus
}
#endif

#endif /* BMUXLIB_INPUT_H_ */

/*****************************************************************************
* EOF
******************************************************************************/
