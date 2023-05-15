/***************************************************************************
*     (c)2004-2011 Broadcom Corporation
*  
*  This program is the proprietary software of Broadcom Corporation and/or its licensors,
*  and may only be used, duplicated, modified or distributed pursuant to the terms and
*  conditions of a separate, written license agreement executed between you and Broadcom
*  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
*  no license (express or implied), right to use, or waiver of any kind with respect to the
*  Software, and Broadcom expressly reserves all rights in and to the Software and all
*  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
*  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
*  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.  
*   
*  Except as expressly set forth in the Authorized License,
*   
*  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
*  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
*  and to use this information only in connection with your use of Broadcom integrated circuit products.
*   
*  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" 
*  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR 
*  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO 
*  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES 
*  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, 
*  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION 
*  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF 
*  USE OR PERFORMANCE OF THE SOFTWARE.
*  
*  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS 
*  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR 
*  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR 
*  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF 
*  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT 
*  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE 
*  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF 
*  ANY LIMITED REMEDY.
* 
* $brcm_Workfile: bape_mux_output.c $
* $brcm_Revision: Hydra_Software_Devel/55 $
* $brcm_Date: 8/30/12 5:24p $
*
* API Description:
*   API name: MuxOutput
*    Specific APIs related to Audio Transcoder Output (GenCdbItb)
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/ape/src/raaga/bape_mux_output.c $
* 
* Hydra_Software_Devel/55   8/30/12 5:24p jgarrett
* SW7425-3692: Changing maxItbDescriptors to numDescriptors based on code
* review
* 
* Hydra_Software_Devel/54   8/21/12 3:12p jgarrett
* SW7425-3692: Changed handling of default value for maxItbDescriptors
* 
* Hydra_Software_Devel/53   8/20/12 11:56a mpeteri
* SW7425-3692: merge to Hydra
* 
* Hydra_Software_Devel/SW7425-3692/1   8/16/12 5:59p mpeteri
* SW7425-3692: add field for configuring maxItbDescriptors
* 
* Hydra_Software_Devel/52   8/14/12 11:26a hongtaoz
* SW7425-624: fixed a corner case of ADTS segment CDB wraparound;
* 
* Hydra_Software_Devel/51   7/13/12 4:35p jgarrett
* SW7425-3061: Correcting CDB offset calculation for 0-length frames
* 
* Hydra_Software_Devel/49   6/26/12 11:55a jgarrett
* SW7425-3061: Breaking out of ITB read loop for NULL frames
* 
* Hydra_Software_Devel/48   6/15/12 4:44p jgarrett
* SW7435-78: Correcting metadata ITB entry address
* 
* Hydra_Software_Devel/47   6/15/12 2:30p jgarrett
* SW7425-3258: Correcting comments regarding size parameters to
* GetBufferDescriptors
* 
* Hydra_Software_Devel/46   5/21/12 6:32p jgarrett
* SW7435-194: Refactoring FRAME_END logic to match design prior to new
* ITB field
* 
* Hydra_Software_Devel/44   5/16/12 11:45a jgarrett
* SW7425-3071: Adding programmable heaps
* 
* Hydra_Software_Devel/43   5/15/12 11:39a jgarrett
* SW7425-2695: Correcting STC upper address
* 
* Hydra_Software_Devel/42   5/14/12 3:16p jgarrett
* SW7425-2695: Adding full 42-bit STC
* 
* Hydra_Software_Devel/41   5/7/12 5:44p jgarrett
* SW7435-88: Adding FRAME_END flag
* 
* Hydra_Software_Devel/40   5/3/12 5:06p jgarrett
* SW7425-2075: Merge to main branch
* 
* Hydra_Software_Devel/SW7425-2075/2   5/3/12 3:45p jgarrett
* SW7425-2075: Merging latest mainline changes
* 
* Hydra_Software_Devel/39   4/25/12 4:23p jgarrett
* SW7425-2697: Adding per-frame STC snapshot
* 
* Hydra_Software_Devel/38   4/24/12 6:18p jgarrett
* SW7346-569: Removing C bitfields for ITB entries.  Using uint32_t reads
* and RDB-style macros instead.
* 
* Hydra_Software_Devel/37   4/20/12 5:25p jgarrett
* SW7231-774: Removing assertion when consuming more descriptors than we
* have available
* 
* Hydra_Software_Devel/SW7425-2075/1   4/6/12 12:52p jgarrett
* SW7425-2075: Adding new FMT interfaces
* 
* Hydra_Software_Devel/36   2/24/12 11:49a jgarrett
* SW7425-2416: Adding sanity checks for errant mux behavior and improved
* empty CDB handling
* 
* Hydra_Software_Devel/35   2/21/12 11:12a jgarrett
* SW7425-2262: Adding empty frame support and fixing case where CDB data
* was available before ITB data
* 
* Hydra_Software_Devel/34   2/17/12 4:15p jgarrett
* SW7425-2262: Adding handling for empty frames (currently disabled)
* 
* Hydra_Software_Devel/33   2/16/12 4:41p jgarrett
* SW7425-2416: Resetting shadow read pointers on BAPE_MuxOutput_Start in
* case they are flushed
* 
* Hydra_Software_Devel/32   2/14/12 3:17p jgarrett
* SW7425-2395: Bumping max ITB descriptors to 128
* 
* Hydra_Software_Devel/31   2/10/12 1:41p jgarrett
* SW7425-1221: Adding STC Snapshot
* 
* Hydra_Software_Devel/30   2/10/12 11:03a jgarrett
* SW7425-624: Correcting ADTS segment wrap calculations
* 
* Hydra_Software_Devel/29   2/9/12 5:45p jgarrett
* SW7425-2268: Correcting CDB/ITB advancement logic with additional ITB
* entries
* 
* Hydra_Software_Devel/28   1/23/12 2:58p jgarrett
* SW7425-1478: Adding metadata support for WMA
* 
* Hydra_Software_Devel/27   12/16/11 11:34a jgarrett
* SW7425-1478: Adding BlockAlign and NumChannels to WMA metadata
* 
* Hydra_Software_Devel/26   12/9/11 4:57p jgarrett
* SW7425-1478: Copying ITB metadata into descriptor
* 
* Hydra_Software_Devel/25   12/8/11 2:05p jgarrett
* SW7425-1478: Adding WMA metadata extraction
* 
* Hydra_Software_Devel/24   11/17/11 10:34a jgarrett
* SW7429-18: Fixing STC register address
* 
* Hydra_Software_Devel/23   11/14/11 3:37p gskerl
* SW7429-18: Merging 7429 changes back to main branch.
* 
* Hydra_Software_Devel/SW7429-18/1   10/21/11 6:29p jgarrett
* SW7429-18: Initial compileable version for 7429
* 
* Hydra_Software_Devel/22   10/14/11 12:11p jgarrett
* SW7425-468: Adding hooks for NRT DSP mixer support
* 
* Hydra_Software_Devel/21   9/15/11 9:24a jgarrett
* SW7425-1285: Sending lower 32-bits of STC to DSP on systems with 42-bit
* STC support
* 
* Hydra_Software_Devel/20   7/6/11 4:55p jgarrett
* SW7358-62: Storing a local copy of the XptContextMap structure
* 
* Hydra_Software_Devel/19   6/27/11 8:27a jgarrett
* SW7425-654: Merging NRT to main line
* 
* Hydra_Software_Devel/NRT_XCODE_DEVEL/2   6/23/11 11:03a jgarrett
* SW7425-654: Changing realtimeMode setting for new API
* 
* Hydra_Software_Devel/NRT_XCODE_DEVEL/1   6/21/11 5:54p jgarrett
* SW7425-654: Adding NRT hooks
* 
* Hydra_Software_Devel/18   6/21/11 3:50p jgarrett
* SW7425-654: Adding NRT APIs to decoder and mux
* 
* Hydra_Software_Devel/17   6/17/11 4:50p jgarrett
* SW7425-741: Updated ITB parsing to drop variable entries for ASC and
* AlgoInfo.
* 
* Hydra_Software_Devel/16   5/24/11 3:18p jgarrett
* SW7425-637: Initializing metadata structure to 0
* 
* Hydra_Software_Devel/15   4/20/11 12:33p jgarrett
* SW7425-288: Flushing data cache properly on ADTS wraparound cases
* 
* Hydra_Software_Devel/14   4/18/11 10:16p jgarrett
* SW7425-288: Coverity CID 398
* 
* Hydra_Software_Devel/13   4/18/11 10:09p jgarrett
* SW7425-361: Refactoring DSP branch decisions
* 
* Hydra_Software_Devel/12   4/18/11 8:54p jgarrett
* SW7425-288: Adding ADTS parsing and metadata
* 
* Hydra_Software_Devel/11   4/17/11 1:55p jgarrett
* SW7425-288: Adding audio codec to metadata
* 
* Hydra_Software_Devel/10   4/17/11 1:26p jgarrett
* SW7425-288: Filling metadata descriptors for sample rate
* 
* Hydra_Software_Devel/9   4/16/11 2:01p jgarrett
* SW7425-288: Adding metadata interface
* 
* Hydra_Software_Devel/8   4/16/11 1:43p jgarrett
* SW7425-329: Converting buffer descriptors and frame buffer to cached
* addresses
* 
* Hydra_Software_Devel/7   4/16/11 12:15p jgarrett
* SW7425-371: Removing tab characters
* 
* Hydra_Software_Devel/6   4/15/11 9:24p jgarrett
* SW7425-88: Refactoring EOS handling to send one EOS at end of stream
* 
* Hydra_Software_Devel/5   4/14/11 3:25p jgarrett
* SW7425-360: Fixing shutdown crash if resources are leaked in the app
* 
* Hydra_Software_Devel/4   4/12/11 3:05p jgarrett
* SWDTV-6305: Eliminating strict-aliasing warnings.
* 
* Hydra_Software_Devel/3   3/28/11 3:55p srajapur
* SW7422-356 : [7425] Corrected stopped flag.
* 
* Hydra_Software_Devel/2   3/22/11 2:35p jgarrett
* SW7422-356: Fixing polarity of stopped flag
* 
* Hydra_Software_Devel/1   3/21/11 7:08p jgarrett
* SW7422-356: Adding MuxOutput
* 
***************************************************************************/

#include "bape.h"
#include "bape_priv.h"
#include "bchp_common.h"

#ifdef BCHP_XPT_PCROFFSET_REG_START
#include "bchp_xpt_pcroffset.h"
#endif

BDBG_MODULE(bape_mux_output);
BDBG_FILE_MODULE(bape_adts);

BDBG_OBJECT_ID(BAPE_MuxOutput);

typedef struct BAPE_ItbEntry
{
    uint32_t words[4];
} BAPE_ItbEntry;

typedef struct BAPE_FrameItbEntries
{
    BAPE_ItbEntry baseAddress;
    BAPE_ItbEntry ptsDts;
    BAPE_ItbEntry bitRate;
    BAPE_ItbEntry escrMetadata;
} BAPE_FrameItbEntries;

#define BAPE_ITB_ENTRY_TYPE_BASE_ADDRESS (0x20)
#define BAPE_ITB_ENTRY_TYPE_PTS_DTS      (0x21)
#define BAPE_ITB_ENTRY_TYPE_BIT_RATE     (0x60)
#define BAPE_ITB_ENTRY_TYPE_MUX_ESCR     (0x61)
#define BAPE_ITB_ENTRY_TYPE_ALGO_INFO    (0x62)
#define BAPE_ITB_ENTRY_TYPE_ASC          (0x63)
#define BAPE_ITB_ENTRY_TYPE_METADATA     (0x68)

/* Get Field Routines for ITB fields */
#define BAPE_ITB_WORD(Entry,Field) BAPE_ITB_##Entry##_##Field##_WORD
#define BAPE_ITB_MASK(Entry,Field) BAPE_ITB_##Entry##_##Field##_MASK
#define BAPE_ITB_SHIFT(Entry,Field) BAPE_ITB_##Entry##_##Field##_SHIFT

#define BAPE_ITB_GET_FIELD(Memory,Entry,Field)\
	((((Memory)->words[BAPE_ITB_WORD(Entry,Field)] & \
       BAPE_ITB_MASK(Entry,Field)) >> \
	  BAPE_ITB_SHIFT(Entry,Field)))

/* General Fields */
#define BAPE_ITB_GENERIC_ENTRY_TYPE_WORD      (0)
#define BAPE_ITB_GENERIC_ENTRY_TYPE_MASK      (0xFF000000)
#define BAPE_ITB_GENERIC_ENTRY_TYPE_SHIFT     (24)

/* Base Address Fields */
#define BAPE_ITB_BASE_ADDRESS_ERROR_WORD            (0)
#define BAPE_ITB_BASE_ADDRESS_ERROR_MASK            (0x00800000)
#define BAPE_ITB_BASE_ADDRESS_ERROR_SHIFT           (23)
#define BAPE_ITB_BASE_ADDRESS_CDB_ADDRESS_WORD      (1)
#define BAPE_ITB_BASE_ADDRESS_CDB_ADDRESS_MASK      (0xFFFFFFFF)
#define BAPE_ITB_BASE_ADDRESS_CDB_ADDRESS_SHIFT     (0)
#define BAPE_ITB_BASE_ADDRESS_FRAME_VALID_WORD    (2)
#define BAPE_ITB_BASE_ADDRESS_FRAME_VALID_MASK    (0x80000000)
#define BAPE_ITB_BASE_ADDRESS_FRAME_VALID_SHIFT   (31)
#define BAPE_ITB_BASE_ADDRESS_FRAME_LENGTH_WORD     (2)
#define BAPE_ITB_BASE_ADDRESS_FRAME_LENGTH_MASK     (0x0000FFFF)
#define BAPE_ITB_BASE_ADDRESS_FRAME_LENGTH_SHIFT    (0)

/* PTS_DTS Fields */
#define BAPE_ITB_PTS_DTS_DTS_VALID_WORD     (0)
#define BAPE_ITB_PTS_DTS_DTS_VALID_MASK     (0x00008000)
#define BAPE_ITB_PTS_DTS_DTS_VALID_SHIFT    (15)
#define BAPE_ITB_PTS_DTS_PTS_32_WORD        (0)
#define BAPE_ITB_PTS_DTS_PTS_32_MASK        (0x00000002)
#define BAPE_ITB_PTS_DTS_PTS_32_SHIFT       (1)
#define BAPE_ITB_PTS_DTS_DTS_32_WORD        (0)
#define BAPE_ITB_PTS_DTS_DTS_32_MASK        (0x00000001)
#define BAPE_ITB_PTS_DTS_DTS_32_SHIFT       (0)
#define BAPE_ITB_PTS_DTS_PTS_WORD           (1)
#define BAPE_ITB_PTS_DTS_PTS_MASK           (0xFFFFFFFF)
#define BAPE_ITB_PTS_DTS_PTS_SHIFT          (0)
#define BAPE_ITB_PTS_DTS_STC_UPPER_WORD     (2)
#define BAPE_ITB_PTS_DTS_STC_UPPER_MASK     (0xFFFFFFFF)
#define BAPE_ITB_PTS_DTS_STC_UPPER_SHIFT    (0)
#define BAPE_ITB_PTS_DTS_STC_LOWER_WORD     (3)
#define BAPE_ITB_PTS_DTS_STC_LOWER_MASK     (0xFFFFFFFF)
#define BAPE_ITB_PTS_DTS_STC_LOWER_SHIFT    (0)

/* BIT_RATE fields */
#define BAPE_ITB_BIT_RATE_SHR_WORD               (1)
#define BAPE_ITB_BIT_RATE_SHR_MASK               (0xFFFF0000)
#define BAPE_ITB_BIT_RATE_SHR_SHIFT              (16)
#define BAPE_ITB_BIT_RATE_TICKS_PER_BIT_WORD     (1)
#define BAPE_ITB_BIT_RATE_TICKS_PER_BIT_MASK     (0x0000FFFF)
#define BAPE_ITB_BIT_RATE_TICKS_PER_BIT_SHIFT    (0)
#define BAPE_ITB_BIT_RATE_SAMPLE_RATE_WORD       (2)
#define BAPE_ITB_BIT_RATE_SAMPLE_RATE_MASK       (0xFFFFFFFF)
#define BAPE_ITB_BIT_RATE_SAMPLE_RATE_SHIFT      (0)

/* ESCR_METADATA fields */
#define BAPE_ITB_ESCR_METADATA_ESCR_WORD         (1)
#define BAPE_ITB_ESCR_METADATA_ESCR_MASK         (0xFFFFFFFF)
#define BAPE_ITB_ESCR_METADATA_ESCR_SHIFT        (0)
#define BAPE_ITB_ESCR_METADATA_ORIGINAL_PTS_WORD    (2)
#define BAPE_ITB_ESCR_METADATA_ORIGINAL_PTS_MASK    (0xFFFFFFFF)
#define BAPE_ITB_ESCR_METADATA_ORIGINAL_PTS_SHIFT   (0)
#define BAPE_ITB_ESCR_METADATA_ORIGINAL_PTS_INTERPOLATED_WORD    (3)
#define BAPE_ITB_ESCR_METADATA_ORIGINAL_PTS_INTERPOLATED_MASK    (0x80000000)
#define BAPE_ITB_ESCR_METADATA_ORIGINAL_PTS_INTERPOLATED_SHIFT   (31)

/* ALGO_INFO fields */
#define BAPE_ITB_ALGO_INFO_ALGO_ID_WORD         (0)
#define BAPE_ITB_ALGO_INFO_ALGO_ID_MASK         (0x000000FF)
#define BAPE_ITB_ALGO_INFO_ALGO_ID_SHIFT        (0)
#define BAPE_ITB_ALGO_INFO_HRD_SIZE_WORD        (1)
#define BAPE_ITB_ALGO_INFO_HRD_SIZE_MASK        (0xFFFF0000)
#define BAPE_ITB_ALGO_INFO_HRD_SIZE_SHIFT       (16)
#define BAPE_ITB_ALGO_INFO_CHANNEL_CONFIG_WORD  (1)
#define BAPE_ITB_ALGO_INFO_CHANNEL_CONFIG_MASK  (0x0000FF00)
#define BAPE_ITB_ALGO_INFO_CHANNEL_CONFIG_SHIFT (8)
#define BAPE_ITB_ALGO_INFO_BITS_PER_SAMPLE_WORD  (1)
#define BAPE_ITB_ALGO_INFO_BITS_PER_SAMPLE_MASK  (0x000000FF)
#define BAPE_ITB_ALGO_INFO_BITS_PER_SAMPLE_SHIFT (0)
#define BAPE_ITB_ALGO_INFO_BIT_RATE_WORD         (2)
#define BAPE_ITB_ALGO_INFO_BIT_RATE_MASK         (0xFFFFFFFF)
#define BAPE_ITB_ALGO_INFO_BIT_RATE_SHIFT        (0)
#define BAPE_ITB_ALGO_INFO_MAX_BIT_RATE_WORD     (3)
#define BAPE_ITB_ALGO_INFO_MAX_BIT_RATE_MASK     (0xFFFFFFFF)
#define BAPE_ITB_ALGO_INFO_MAX_BIT_RATE_SHIFT    (0)

/* ASC fields */
#define BAPE_ITB_ASC_NUM_ENTRIES_WORD            (0)
#define BAPE_ITB_ASC_NUM_ENTRIES_MASK            (0x0000FF00)
#define BAPE_ITB_ASC_NUM_ENTRIES_SHIFT           (8)
#define BAPE_ITB_ASC_ENTRY_NUMBER_WORD           (0)
#define BAPE_ITB_ASC_ENTRY_NUMBER_MASK           (0x000000FF)
#define BAPE_ITB_ASC_ENTRY_NUMBER_SHIFT          (0)
#define BAPE_ITB_ASC_ASC_SIZE_WORD               (1)
#define BAPE_ITB_ASC_ASC_SIZE_MASK               (0xFFFF0000)
#define BAPE_ITB_ASC_ASC_SIZE_SHIFT              (16)
#define BAPE_ITB_ASC_ASC0_WORD                   (1)
#define BAPE_ITB_ASC_ASC0_MASK                   (0x0000FFFF)
#define BAPE_ITB_ASC_ASC0_SHIFT                  (0)
#define BAPE_ITB_ASC_ASC1_WORD                   (2)
#define BAPE_ITB_ASC_ASC1_MASK                   (0xFFFFFFFF)
#define BAPE_ITB_ASC_ASC1_SHIFT                  (0)
#define BAPE_ITB_ASC_ASC2_WORD                   (3)
#define BAPE_ITB_ASC_ASC2_MASK                   (0xFFFFFFFF)
#define BAPE_ITB_ASC_ASC2_SHIFT                  (0)

/* WMA_SPECIFIC fields */
#define BAPE_ITB_WMA_SPECIFIC_NUM_ENTRIES_WORD            (0)
#define BAPE_ITB_WMA_SPECIFIC_NUM_ENTRIES_MASK            (0x0000FF00)
#define BAPE_ITB_WMA_SPECIFIC_NUM_ENTRIES_SHIFT           (8)
#define BAPE_ITB_WMA_SPECIFIC_ENTRY_NUMBER_WORD           (0)
#define BAPE_ITB_WMA_SPECIFIC_ENTRY_NUMBER_MASK           (0x000000FF)
#define BAPE_ITB_WMA_SPECIFIC_ENTRY_NUMBER_SHIFT          (0)
#define BAPE_ITB_WMA_SPECIFIC_ENCODE_OPTION_WORD          (1)
#define BAPE_ITB_WMA_SPECIFIC_ENCODE_OPTION_MASK          (0x0000FFFF)
#define BAPE_ITB_WMA_SPECIFIC_ENCODE_OPTION_SHIFT         (0)
#define BAPE_ITB_WMA_SPECIFIC_SAMPLES_PER_FRAME_WORD      (2)
#define BAPE_ITB_WMA_SPECIFIC_SAMPLES_PER_FRAME_MASK      (0xFFFF0000)
#define BAPE_ITB_WMA_SPECIFIC_SAMPLES_PER_FRAME_SHIFT     (16)
#define BAPE_ITB_WMA_SPECIFIC_BLOCK_ALIGN_WORD            (2)
#define BAPE_ITB_WMA_SPECIFIC_BLOCK_ALIGN_MASK            (0x0000FFFF)
#define BAPE_ITB_WMA_SPECIFIC_BLOCK_ALIGN_SHIFT           (0)
#define BAPE_ITB_WMA_SPECIFIC_SUPER_BLOCK_ALIGN_WORD      (3)
#define BAPE_ITB_WMA_SPECIFIC_SUPER_BLOCK_ALIGN_MASK      (0xFFFFFFFF)
#define BAPE_ITB_WMA_SPECIFIC_SUPER_BLOCK_ALIGN_SHIFT     (0)

/* METADATA fields */
#define BAPE_ITB_METADATA_STC_UPPER_WORD            (0)
#define BAPE_ITB_METADATA_STC_UPPER_MASK            (0x000003FF)
#define BAPE_ITB_METADATA_STC_UPPER_SHIFT           (0)
#define BAPE_ITB_METADATA_STC_LOWER_WORD            (1)
#define BAPE_ITB_METADATA_STC_LOWER_MASK            (0xFFFFFFFF)
#define BAPE_ITB_METADATA_STC_LOWER_SHIFT           (0)

#define BAPE_MUXOUTPUT_MAX_ITBDESCRIPTORS 128
#define BAPE_MUXOUTPUT_MAX_METADATADESCRIPTORS 1
#define BAPE_MUXOUTPUT_MAX_ADTS_SEGMENTS 5          /* ADTS can have up to 4 data segments with a max of one CDB wrap around */
#define BAPE_MUXOUTPUT_MAX_ASC_ENTRIES 2

typedef struct BAPE_OutputDescriptorInfo
{
    uint32_t uiITBBufferShadowReadOffset; /* Points to the ITB entry that needs to be parsed next */
    uint32_t uiCDBBufferShadowReadOffset; /* Points to the CDB location that needs to be muxed next */

    BAVC_AudioBufferDescriptor *pstDescriptorsCached;
    BAVC_AudioBufferDescriptor *pstDescriptorsUncached;
    BAVC_AudioMetadataDescriptor *pstMetadataCached;
    BAVC_AudioMetadataDescriptor *pstMetadataUncached;
    uint32_t uiDescriptorWriteOffset;
    uint32_t uiDescriptorReadOffset;
    unsigned numOutstandingDescriptors;

    /* ITB Parsing Info */
    struct
    {
        BAPE_FrameItbEntries current, next;
        BAPE_ItbEntry metadata;
    } itb;

    /* ADTS Parsing Info */
    unsigned adtsSegment, adtsNumSegments;
    unsigned adtsSegmentOffset[BAPE_MUXOUTPUT_MAX_ADTS_SEGMENTS];
    unsigned adtsSegmentLength[BAPE_MUXOUTPUT_MAX_ADTS_SEGMENTS];
    unsigned adtsSegmentRawOffset[BAPE_MUXOUTPUT_MAX_ADTS_SEGMENTS];
    unsigned adtsSegmentRawLength[BAPE_MUXOUTPUT_MAX_ADTS_SEGMENTS];
} BAPE_OutputDescriptorInfo;

typedef struct BAPE_MuxOutput
{
    BDBG_OBJECT(BAPE_MuxOutput)
    BAPE_PathNode node;
    BMEM_Heap_Handle hDescriptorHeap;
    BMEM_Heap_Handle hCdbHeap;
    BMEM_Heap_Handle hItbHeap;
    bool started;
    bool sendEos;
    bool sendMetadata;
    BAPE_MuxOutputStartSettings startSettings;
    BAVC_XptContextMap contextMap;
    BAPE_MuxOutputCreateSettings createSettings;
    BAPE_Connector input;
    BDSP_TaskHandle task;
    unsigned branchId, stageId;
    void *pCdbBaseCached;
    BAPE_OutputDescriptorInfo descriptorInfo;
    BLST_S_ENTRY(BAPE_MuxOutput) deviceListNode;
} BAPE_MuxOutput;

static BERR_Code BAPE_MuxOutput_P_AllocatePathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);
static BERR_Code BAPE_MuxOutput_P_ConfigPathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);
static void BAPE_MuxOutput_P_StopPathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);
static void BAPE_MuxOutput_P_RemoveInputCallback(struct BAPE_PathNode *pNode, BAPE_PathConnector *pConnector);
static void BAPE_MuxOutput_P_ParseItb(BAPE_MuxOutputHandle hMuxOutput, BAPE_FrameItbEntries **pCurrent, BAPE_FrameItbEntries **pNext, BAVC_AudioMetadataDescriptor *pMetadataDescriptor, BAPE_ItbEntry **pMetadata);

static BERR_Code BAPE_MuxOutput_P_ParseAdtsMetadata(
    BAPE_MuxOutputHandle hMuxOutput, 
    BAVC_AudioBufferDescriptor *pFrameDescriptor, 
    BAVC_AudioMetadataDescriptor *pMetadataDescriptor
    );
extern void BAPE_MuxOutput_P_ParseAdtsSegments( 
    BAPE_MuxOutputHandle hMuxOutput, 
    const uint8_t *pBuffer,
    size_t bufferLength,
    const uint8_t *pWrapBuffer,
    size_t wrapBufferLength
    );

/***************************************************************************
Summary:
    Get default settings for a MuxOutput stage
***************************************************************************/
void BAPE_MuxOutput_GetDefaultCreateSettings(
    BAPE_MuxOutputCreateSettings *pSettings   /* [out] default settings */
    )
{
    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->numDescriptors = BAPE_MUXOUTPUT_MAX_ITBDESCRIPTORS;
}

/***************************************************************************
Summary:
    Open a MuxOutput stage
***************************************************************************/
BERR_Code BAPE_MuxOutput_Create(
    BAPE_Handle hApe,
    const BAPE_MuxOutputCreateSettings *pSettings,
    BAPE_MuxOutputHandle *pHandle
    )
{
    BAPE_MuxOutputHandle hMuxOutput;
    BERR_Code errCode;
    uint32_t offset;
    void *pAddress;
    size_t cdbLength;
    BAPE_FMT_Descriptor format;
    BAPE_FMT_Capabilities caps;

    BDBG_OBJECT_ASSERT(hApe, BAPE_Device);
    BDBG_ASSERT(NULL != pSettings);
    BDBG_ASSERT(NULL != pHandle);

    BDBG_ASSERT(NULL != pSettings->pContextMap);

    if ( pSettings->numDescriptors == 0 )
    {
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    hMuxOutput = BKNI_Malloc(sizeof(BAPE_MuxOutput));
    if ( NULL == hMuxOutput )
    {
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }
    BKNI_Memset(hMuxOutput, 0, sizeof(BAPE_MuxOutput));
    BDBG_OBJECT_SET(hMuxOutput, BAPE_MuxOutput);
    hMuxOutput->createSettings = *pSettings;
    hMuxOutput->hDescriptorHeap = pSettings->heaps.descriptor ? pSettings->heaps.descriptor : hApe->memHandle;
    hMuxOutput->hCdbHeap = pSettings->heaps.cdb ? pSettings->heaps.cdb : hApe->memHandle;
    hMuxOutput->hItbHeap = pSettings->heaps.itb ? pSettings->heaps.itb : hApe->memHandle;
    /* Technically, this has no output paths, but unless you set 1 below you get a compiler warning */
    BAPE_P_InitPathNode(&hMuxOutput->node, BAPE_PathNodeType_eMuxOutput, 0, 1, hApe, hMuxOutput);
    hMuxOutput->node.pName = "MuxOutput";

    BAPE_Connector_P_GetFormat(&hMuxOutput->node.connectors[0], &format);
    format.sampleRate = 0;
    format.source = BAPE_DataSource_eRave;
    format.type = BAPE_DataType_eRave;
    errCode = BAPE_Connector_P_SetFormat(&hMuxOutput->node.connectors[0], &format);
    if ( errCode ) { errCode = BERR_TRACE(errCode); goto err_format; }

    BAPE_PathNode_P_GetInputCapabilities(&hMuxOutput->node, &caps);
    BAPE_FMT_P_EnableSource(&caps, BAPE_DataSource_eDspBuffer);
    BAPE_FMT_P_EnableType(&caps, BAPE_DataType_eIec61937);
    BAPE_FMT_P_EnableType(&caps, BAPE_DataType_eIec61937x4);
    BAPE_FMT_P_EnableType(&caps, BAPE_DataType_eIec61937x16);
    BAPE_FMT_P_EnableType(&caps, BAPE_DataType_eCompressedRaw);
    errCode = BAPE_PathNode_P_SetInputCapabilities(&hMuxOutput->node, &caps);
    if ( errCode ) { errCode = BERR_TRACE(errCode); goto err_caps; }

    /* Save RAVE Context */
    BKNI_Memcpy(&hMuxOutput->contextMap, pSettings->pContextMap, sizeof(BAVC_XptContextMap));
    hMuxOutput->createSettings.pContextMap = &hMuxOutput->contextMap;

    /* Reset shadow pointers */
    hMuxOutput->descriptorInfo.uiCDBBufferShadowReadOffset = BREG_Read32(hMuxOutput->node.deviceHandle->regHandle, pSettings->pContextMap->CDB_Base);
    hMuxOutput->descriptorInfo.uiITBBufferShadowReadOffset = BREG_Read32(hMuxOutput->node.deviceHandle->regHandle, pSettings->pContextMap->ITB_Base);

    /* MuxOutput Specifics */
    hMuxOutput->node.allocatePathFromInput = BAPE_MuxOutput_P_AllocatePathFromInput;
    hMuxOutput->node.configPathFromInput = BAPE_MuxOutput_P_ConfigPathFromInput;
    hMuxOutput->node.stopPathFromInput = BAPE_MuxOutput_P_StopPathFromInput;
    hMuxOutput->node.removeInput = BAPE_MuxOutput_P_RemoveInputCallback;

    offset = BREG_Read32(hApe->regHandle, pSettings->pContextMap->CDB_Base);

    errCode = BMEM_Heap_ConvertOffsetToAddress(hMuxOutput->hCdbHeap, offset, &pAddress);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_cdb_offset;
    }
    errCode = BMEM_Heap_ConvertAddressToCached(hMuxOutput->hCdbHeap, pAddress, &hMuxOutput->pCdbBaseCached);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_cdb_cached;
    }
    /* Flush the CDB data from the cache prior to the HW filling it */
    cdbLength = (1+BREG_Read32(hApe->regHandle, pSettings->pContextMap->CDB_End))-offset;
    BMEM_Heap_FlushCache(hMuxOutput->hCdbHeap, hMuxOutput->pCdbBaseCached, cdbLength);

    hMuxOutput->descriptorInfo.pstDescriptorsUncached = BMEM_Heap_Alloc(hMuxOutput->hDescriptorHeap, sizeof(BAVC_AudioBufferDescriptor)*pSettings->numDescriptors);
    if ( NULL == hMuxOutput->descriptorInfo.pstDescriptorsUncached ) 
    {
        errCode = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
        goto err_alloc_descriptors;
    }
    errCode = BMEM_Heap_ConvertAddressToCached(hMuxOutput->hDescriptorHeap, hMuxOutput->descriptorInfo.pstDescriptorsUncached, &pAddress);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_cache_descriptors;
    }
    hMuxOutput->descriptorInfo.pstDescriptorsCached = pAddress;
    hMuxOutput->descriptorInfo.pstMetadataUncached = BMEM_Heap_Alloc(hMuxOutput->hDescriptorHeap, sizeof(BAVC_AudioMetadataDescriptor)*BAPE_MUXOUTPUT_MAX_METADATADESCRIPTORS);
    if ( NULL == hMuxOutput->descriptorInfo.pstMetadataUncached ) 
    {
        errCode = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
        goto err_alloc_metadata;
    }
    errCode = BMEM_Heap_ConvertAddressToCached(hMuxOutput->hDescriptorHeap, hMuxOutput->descriptorInfo.pstMetadataUncached, &pAddress);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_cache_metadata;
    }
    hMuxOutput->descriptorInfo.pstMetadataCached = pAddress;

    BLST_S_INSERT_HEAD(&hApe->muxOutputList, hMuxOutput, deviceListNode);

    *pHandle = hMuxOutput;
    return BERR_SUCCESS;

err_cache_metadata:
    BMEM_Heap_Free(hMuxOutput->hDescriptorHeap, hMuxOutput->descriptorInfo.pstMetadataUncached);
err_alloc_metadata:
err_cache_descriptors:
    BMEM_Heap_Free(hMuxOutput->hDescriptorHeap, hMuxOutput->descriptorInfo.pstDescriptorsUncached);
err_alloc_descriptors:
err_cdb_cached:    
err_cdb_offset:
err_caps:
err_format:
    BDBG_OBJECT_DESTROY(hMuxOutput, BAPE_MuxOutput);
    BKNI_Free(hMuxOutput);

    return errCode;
}


/***************************************************************************
Summary:
    Close a MuxOutput stage
    
Description:
    Input to the stage must be removed prior to closing.
***************************************************************************/
void BAPE_MuxOutput_Destroy(
    BAPE_MuxOutputHandle hMuxOutput
    )
{
    bool running;
    BDBG_OBJECT_ASSERT(hMuxOutput, BAPE_MuxOutput);
    running = (hMuxOutput->task != NULL)?true:false;
    BDBG_ASSERT(false == running);
    BDBG_ASSERT(NULL == hMuxOutput->input);
    BLST_S_REMOVE(&hMuxOutput->node.deviceHandle->muxOutputList, hMuxOutput, BAPE_MuxOutput, deviceListNode);
    BMEM_Heap_Free(hMuxOutput->hDescriptorHeap, hMuxOutput->descriptorInfo.pstMetadataUncached);
    BMEM_Heap_Free(hMuxOutput->hDescriptorHeap, hMuxOutput->descriptorInfo.pstDescriptorsUncached);
    BDBG_OBJECT_DESTROY(hMuxOutput, BAPE_MuxOutput);
    BKNI_Free(hMuxOutput);
}

/***************************************************************************
Summary:
    Get Default Start-time Settings for a MuxOutput object
***************************************************************************/
void BAPE_MuxOutput_GetDefaultStartSettings(
    BAPE_MuxOutputStartSettings *pSettings    /* [out] Settings */
    )
{
    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));    
}

static BERR_Code BAPE_MuxOutput_P_ApplyDspSettings(
    BAPE_MuxOutputHandle hMuxOutput
    )
{
    BERR_Code errCode;
    BDSP_Raaga_Audio_GenCdbItbConfigParams userConfig;

    BDBG_OBJECT_ASSERT(hMuxOutput, BAPE_MuxOutput);
    BDBG_ASSERT(NULL != hMuxOutput->task);

    errCode = BDSP_Task_GetStageSettings(hMuxOutput->task, hMuxOutput->branchId, hMuxOutput->stageId, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    userConfig.eEnableEncode = hMuxOutput->started ? BDSP_AF_P_eEnable : BDSP_AF_P_eDisable;
    userConfig.ui32EncSTCAddr = BCHP_PHYSICAL_OFFSET + BAPE_CHIP_GET_STC_ADDRESS(hMuxOutput->startSettings.stcIndex);
    #if BAPE_CHIP_HAS_42BIT_STC
    userConfig.ui32EncSTCAddrUpper = BCHP_PHYSICAL_OFFSET + BAPE_CHIP_GET_STC_UPPER_ADDRESS(hMuxOutput->startSettings.stcIndex);
    #endif
    userConfig.ui32A2PInMilliSeconds = hMuxOutput->startSettings.presentationDelay;
    userConfig.eSnapshotRequired = (NULL == hMuxOutput->startSettings.pNonRealTimeIncrement) ? BDSP_AF_P_eEnable /* RT */ : BDSP_AF_P_eDisable /* NRT */;
    userConfig.eStcItbEntryEnable = BDSP_AF_P_eEnable;  /* Always enable the per-frame STC ITB entry */

    errCode = BDSP_Task_SetStageSettings(hMuxOutput->task, hMuxOutput->branchId, hMuxOutput->stageId, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;
}

/***************************************************************************
Summary:
    Start a MuxOutput Object

Description:
    Typically, this is called prior to BAPE_Decoder_Start() in order to
    guarantee all data is captured.

See Also:
    BAPE_MuxOutput_GetDefaultStartSettings
    BAPE_MuxOutput_Stop
***************************************************************************/
BERR_Code BAPE_MuxOutput_Start(
    BAPE_MuxOutputHandle hMuxOutput,
    const BAPE_MuxOutputStartSettings *pSettings
    )
{
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(hMuxOutput, BAPE_MuxOutput);

    if ( hMuxOutput->started )
    {
        BDBG_ERR(("Already started."));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    if ( NULL == pSettings )
    {
        BAPE_MuxOutput_GetDefaultStartSettings(&hMuxOutput->startSettings);
        pSettings = &hMuxOutput->startSettings;
    }
    else
    {
        if ( pSettings->stcIndex > BAPE_CHIP_MAX_STCS )
        {
            BDBG_ERR(("STC Index %u out of range.  Supported values are 0..%u", BAPE_CHIP_MAX_STCS));
            return BERR_TRACE(BERR_INVALID_PARAMETER);
        }
        BKNI_Memcpy(&hMuxOutput->startSettings, pSettings, sizeof(BAPE_MuxOutputStartSettings));
    }

    if ( NULL != pSettings->pNonRealTimeIncrement && hMuxOutput->task )
    {
        BDBG_ERR(("To configure for Non-Realtime Transcode, BAPE_MuxOutput_Start must be called prior to starting transcoder input."));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    BDBG_MSG(("BAPE_MuxOutput_Start"));

    /* Update shadow pointers for context to the current read pointer. */
    hMuxOutput->descriptorInfo.uiCDBBufferShadowReadOffset = BREG_Read32(hMuxOutput->node.deviceHandle->regHandle, hMuxOutput->createSettings.pContextMap->CDB_Read);
    hMuxOutput->descriptorInfo.uiITBBufferShadowReadOffset = BREG_Read32(hMuxOutput->node.deviceHandle->regHandle, hMuxOutput->createSettings.pContextMap->ITB_Read);
    /* Reset Descriptor Offsets in case mux didn't consume all previous descriptors */
    hMuxOutput->descriptorInfo.uiDescriptorWriteOffset = 0;
    hMuxOutput->descriptorInfo.uiDescriptorReadOffset = 0;
    hMuxOutput->descriptorInfo.numOutstandingDescriptors = 0;

    /* Start */
    hMuxOutput->started = true;
    hMuxOutput->sendEos = false;
    hMuxOutput->sendMetadata = true;

    if ( hMuxOutput->task )
    {
        errCode = BAPE_MuxOutput_P_ApplyDspSettings(hMuxOutput);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
    }

    return BERR_SUCCESS;
}

/***************************************************************************
Summary:
    Stop a MuxOutput Object

Description:
    Typically, this is called after BAPE_Decoder_Stop() in order to
    guarantee all data is captured.

See Also:
    BAPE_MuxOutput_Start
***************************************************************************/
void BAPE_MuxOutput_Stop(
    BAPE_MuxOutputHandle hMuxOutput
    )
{
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(hMuxOutput, BAPE_MuxOutput);

    if ( !hMuxOutput->started )
    {
        return;
    }

    BDBG_MSG(("BAPE_MuxOutput_Stop"));
    hMuxOutput->started = false;
    hMuxOutput->sendEos = true;

    if ( hMuxOutput->task )
    {
        errCode = BAPE_MuxOutput_P_ApplyDspSettings(hMuxOutput);
        if ( errCode )
        {
            (void)BERR_TRACE(errCode);
            return;
        }
    }
}

BERR_Code BAPE_MuxOutput_GetBufferDescriptors(
    BAPE_MuxOutputHandle hMuxOutput,
    const BAVC_AudioBufferDescriptor **pBuffer, /* [out] pointer to BAVC_AudioBufferDescriptor structs */
    size_t *pSize, /* [out] number of BAVC_AudioBufferDescriptor elements in pBuffer */
    const BAVC_AudioBufferDescriptor **pBuffer2, /* [out] pointer to BAVC_AudioBufferDescriptor structs after wrap around */
    size_t *pSize2 /* [out] number of BAVC_AudioBufferDescriptor elements in pBuffer2 */
    )
{
    BREG_Handle hReg;
    BAPE_FrameItbEntries *pITBEntry;
    BAPE_FrameItbEntries *pITBEntryNext = NULL;
    BAPE_ItbEntry *pITBEntryMetadata = NULL;
    BAVC_AudioBufferDescriptor *pAudioDescriptor;
    uint32_t uiCDBBaseOffset;
    uint32_t uiCDBEndOffset;
    uint32_t uiCDBValidOffset;
    uint32_t uiCDBReadOffset;
    uint32_t uiCDBEndOfFrameOffset; 
    uint32_t uiNextBase=0;
    uint32_t uiTemp;
    BAPE_OutputDescriptorInfo *psOutputDescDetails;
    BAVC_AudioMetadataDescriptor stMetadata;
    BERR_Code ret = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(hMuxOutput, BAPE_MuxOutput);
    BDBG_ASSERT(NULL != pBuffer);
    BDBG_ASSERT(NULL != pBuffer2);
    BDBG_ASSERT(NULL != pSize);
    BDBG_ASSERT(NULL != pSize2);

    BDBG_ENTER(BAPE_MuxOutput_GetBufferDescriptors);

    hReg = hMuxOutput->node.deviceHandle->regHandle;

    psOutputDescDetails = &hMuxOutput->descriptorInfo;

    *pBuffer = NULL;
    *pBuffer2 = NULL;
    *pSize = 0;
    *pSize2 = 0;

    /* Read CDB Addresses */           
    uiCDBBaseOffset = BREG_Read32(hReg, hMuxOutput->createSettings.pContextMap->CDB_Base);
    uiCDBEndOffset = BREG_Read32(hReg, hMuxOutput->createSettings.pContextMap->CDB_End);           
    uiCDBValidOffset = BREG_Read32(hReg, hMuxOutput->createSettings.pContextMap->CDB_Valid);
    uiCDBReadOffset = BREG_Read32(hReg, hMuxOutput->createSettings.pContextMap->CDB_Read);

    if ( uiCDBEndOffset != 0 )
    {
        uiCDBEndOffset += 1; /* end is inclusive */
    }

    BDBG_MSG(("CDB Base/End/Shadow Read/Read/Write = %08x/%08x/%08x (%08x)/%08x",
              uiCDBBaseOffset,
              uiCDBEndOffset,
              psOutputDescDetails->uiCDBBufferShadowReadOffset,
              uiCDBReadOffset,
              uiCDBValidOffset
             ));

    while ( 1 )
    {
        /* Check for Available ITB Entries */

        BAPE_MuxOutput_P_ParseItb(hMuxOutput, &pITBEntry, &pITBEntryNext, &stMetadata, &pITBEntryMetadata);
        if ( NULL == pITBEntry )
        {
            BDBG_MSG(("No more ITB Entries"));
            break;
        }

        if ( uiCDBValidOffset == psOutputDescDetails->uiCDBBufferShadowReadOffset && \
                  BAPE_ITB_GET_FIELD(&pITBEntry->baseAddress, BASE_ADDRESS, FRAME_LENGTH) > 0 )
        {
            /* We ran out of CDB data */
            BDBG_MSG(("No more CDB Data"));
            break;
        }
        else
        {
            uint32_t uiDepthToNext;
            uint32_t uiDepthToValid;

            /* It is possible that the CDB Valid doesn't, yet, contain any of the next frame and
             * may still be in the middle of the current frame, so we need use the depth that is the
             * lesser of depth(cdb_read,cdb_next) depth(cdb_read,cdb_valid)
             */
            uiNextBase = BAPE_ITB_GET_FIELD(&pITBEntry->baseAddress, BASE_ADDRESS, FRAME_LENGTH) + 
                BAPE_ITB_GET_FIELD(&pITBEntry->baseAddress, BASE_ADDRESS, CDB_ADDRESS);
            if ( uiNextBase >= uiCDBEndOffset )
            {
                uiNextBase -= (uiCDBEndOffset-uiCDBBaseOffset);
            }
                
            if ( uiNextBase >= psOutputDescDetails->uiCDBBufferShadowReadOffset )
            {
                uiDepthToNext = BAPE_ITB_GET_FIELD(&pITBEntryNext->baseAddress, BASE_ADDRESS, CDB_ADDRESS) - \
                                psOutputDescDetails->uiCDBBufferShadowReadOffset;
            }
            else
            {
                uiDepthToNext = uiCDBEndOffset - \
                                psOutputDescDetails->uiCDBBufferShadowReadOffset;
                uiDepthToNext += uiNextBase - uiCDBBaseOffset;
            }

            if ( uiCDBValidOffset >= \
                 psOutputDescDetails->uiCDBBufferShadowReadOffset )
            {
                uiDepthToValid = uiCDBValidOffset - \
                                 psOutputDescDetails->uiCDBBufferShadowReadOffset;
            }
            else
            {
                uiDepthToValid = uiCDBEndOffset - \
                                 psOutputDescDetails->uiCDBBufferShadowReadOffset;
                uiDepthToValid += uiCDBValidOffset - uiCDBBaseOffset;
            }

            if ( uiDepthToValid < uiDepthToNext )
            {
                uiCDBEndOfFrameOffset = uiCDBValidOffset;
            }
            else
            {
                uiCDBEndOfFrameOffset = uiNextBase;
            }
        }

        /* Get Audio Descriptor for this ITB entry */
        uiTemp = (psOutputDescDetails->uiDescriptorWriteOffset + 1) % hMuxOutput->createSettings.numDescriptors;
        if ( uiTemp == psOutputDescDetails->uiDescriptorReadOffset )
        {
            BDBG_MSG(("Out of descriptors"));
            break;
        }
        pAudioDescriptor = &psOutputDescDetails->pstDescriptorsCached[psOutputDescDetails->uiDescriptorWriteOffset];
        psOutputDescDetails->uiDescriptorWriteOffset = uiTemp;

        BKNI_Memset(pAudioDescriptor, 0, sizeof(BAVC_AudioBufferDescriptor));

        if ( uiCDBEndOfFrameOffset >= psOutputDescDetails->uiCDBBufferShadowReadOffset )
        {
            pAudioDescriptor->stCommon.uiLength = uiCDBEndOfFrameOffset - \
                                                  psOutputDescDetails->uiCDBBufferShadowReadOffset;
        }
        else
        {
            /* CDB Wrap occurs, so we need to split this picture into two descriptors.  We handle the first one here. */
            pAudioDescriptor->stCommon.uiLength = uiCDBEndOffset - \
                                                  psOutputDescDetails->uiCDBBufferShadowReadOffset;
        }

        /* Populate other fields iff this descriptor contains the beginning of the frame */
        if ( psOutputDescDetails->uiCDBBufferShadowReadOffset == \
             BAPE_ITB_GET_FIELD(&pITBEntry->baseAddress, BASE_ADDRESS, CDB_ADDRESS) )
        {

            /* We're muxing the beginning of this frame */
            pAudioDescriptor->stCommon.uiFlags |= BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_FRAME_START;

            /* Determine if the original PTS is coded or interpolated */
            if ( BAPE_ITB_GET_FIELD(&pITBEntry->escrMetadata, ESCR_METADATA, ORIGINAL_PTS_INTERPOLATED) == 0 )
            {   
                /* TODO: Is this the correct logic?  This could always be interpolated with frame size changes */
                pAudioDescriptor->stCommon.uiFlags |= BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_ORIGINALPTS_VALID;
            }
            pAudioDescriptor->stCommon.uiOriginalPTS = BAPE_ITB_GET_FIELD(&pITBEntry->escrMetadata, ESCR_METADATA, ORIGINAL_PTS);

            pAudioDescriptor->stCommon.uiFlags |= BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_PTS_VALID;
            pAudioDescriptor->stCommon.uiPTS = BAPE_ITB_GET_FIELD(&pITBEntry->ptsDts, PTS_DTS, PTS_32);
            pAudioDescriptor->stCommon.uiPTS <<= 32;
            pAudioDescriptor->stCommon.uiPTS |= BAPE_ITB_GET_FIELD(&pITBEntry->ptsDts, PTS_DTS, PTS);

            pAudioDescriptor->stCommon.uiFlags |= BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_ESCR_VALID;
            pAudioDescriptor->stCommon.uiESCR = BAPE_ITB_GET_FIELD(&pITBEntry->escrMetadata, ESCR_METADATA, ESCR);

            pAudioDescriptor->stCommon.uiFlags |= BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_TICKSPERBIT_VALID;
            pAudioDescriptor->stCommon.uiTicksPerBit = BAPE_ITB_GET_FIELD(&pITBEntry->bitRate, BIT_RATE, TICKS_PER_BIT);

            pAudioDescriptor->stCommon.uiFlags |= BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_SHR_VALID;
            pAudioDescriptor->stCommon.iSHR = BAPE_ITB_GET_FIELD(&pITBEntry->bitRate, BIT_RATE, SHR);

            if ( 0 == BAPE_ITB_GET_FIELD(&pITBEntry->baseAddress, BASE_ADDRESS, FRAME_VALID) )
            {
                pAudioDescriptor->stCommon.uiFlags |= BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_EMPTY_FRAME;
                BDBG_MSG(("Empty frame received in mux (len %u)", BAPE_ITB_GET_FIELD(&pITBEntry->baseAddress, BASE_ADDRESS, FRAME_LENGTH)));
            }

            /* Parse Metadata Descriptor if found */
            if ( NULL != pITBEntryMetadata )
            {
                pAudioDescriptor->stCommon.uiFlags |= BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_STCSNAPSHOT_VALID;
                pAudioDescriptor->stCommon.uiSTCSnapshot = BAPE_ITB_GET_FIELD(pITBEntryMetadata, METADATA, STC_UPPER);
                pAudioDescriptor->stCommon.uiSTCSnapshot <<= 32;
                pAudioDescriptor->stCommon.uiSTCSnapshot |= BAPE_ITB_GET_FIELD(pITBEntryMetadata, METADATA, STC_LOWER);
                if ( hMuxOutput->startSettings.pNonRealTimeIncrement ) 
                {
                    /* In NRT mode, we use 45kHz not 27Mhz.  Normalize to 27MHz. [27000/45 = 600 so there is no rounding error below] */
                    pAudioDescriptor->stCommon.uiSTCSnapshot = pAudioDescriptor->stCommon.uiSTCSnapshot * (27000/45);
                }
            }
        }

        /* Normalize the offset to 0 */
        pAudioDescriptor->stCommon.uiOffset = \
                                              psOutputDescDetails->uiCDBBufferShadowReadOffset - uiCDBBaseOffset;
        
        /* Invalidate this frame from the cache prior to the host accessing it */
        BMEM_Heap_FlushCache(hMuxOutput->hCdbHeap, 
                             (char *)hMuxOutput->pCdbBaseCached + pAudioDescriptor->stCommon.uiOffset, 
                             pAudioDescriptor->stCommon.uiLength);

        /* Set RAW data offset and length equal to frame offset and length.  When ADTS is parsed, add actual raw offset/length here */
        pAudioDescriptor->uiRawDataOffset = pAudioDescriptor->stCommon.uiOffset;
        pAudioDescriptor->uiRawDataLength = pAudioDescriptor->stCommon.uiLength;

        switch ( BAPE_FMT_P_GetAudioCompressionStd(&hMuxOutput->input->format) )
        {
        case BAVC_AudioCompressionStd_eAacAdts:
        case BAVC_AudioCompressionStd_eAacPlusAdts:
            /* If Start Of Frame */
            if ( pAudioDescriptor->stCommon.uiFlags & BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_FRAME_START )
            {
                uint8_t *pWrapBase;
                size_t wrapLength;
                if ( uiCDBEndOfFrameOffset > psOutputDescDetails->uiCDBBufferShadowReadOffset ) 
                {                    
                    pWrapBase = NULL;
                    wrapLength = 0;
                }
                else
                {
                    pWrapBase = hMuxOutput->pCdbBaseCached;
                    wrapLength = uiCDBEndOfFrameOffset - uiCDBBaseOffset;
                    /* Make sure any wraparound data is also invalidated from the cache prior to accessing it. */
                    BMEM_Heap_FlushCache(hMuxOutput->hCdbHeap, pWrapBase, wrapLength);
                }
                /* This function can not fail.  It will simply create a single segment equal to the
                   entire frame if a parse error occurs */
                BAPE_MuxOutput_P_ParseAdtsSegments(hMuxOutput, 
                                                   (uint8_t *)hMuxOutput->pCdbBaseCached+pAudioDescriptor->stCommon.uiOffset,
                                                   pAudioDescriptor->stCommon.uiLength,
                                                   (uint8_t *)pWrapBase,
                                                   wrapLength);
            }
            else
            {
                BDBG_MSG(("Not Parsing"));
            }
            /* Populate this segment's data */
            {
                unsigned segment = hMuxOutput->descriptorInfo.adtsSegment;
                if ( segment >= hMuxOutput->descriptorInfo.adtsNumSegments)
                {
                    BDBG_ERR(("ADTS segment parsing error"));
                    BKNI_Sleep(1000);   /* Let some debug data flush */
                    BDBG_ASSERT(segment < hMuxOutput->descriptorInfo.adtsNumSegments);
                }
                pAudioDescriptor->stCommon.uiOffset = hMuxOutput->descriptorInfo.adtsSegmentOffset[segment];
                pAudioDescriptor->stCommon.uiLength = hMuxOutput->descriptorInfo.adtsSegmentLength[segment];
                pAudioDescriptor->uiRawDataOffset = hMuxOutput->descriptorInfo.adtsSegmentRawOffset[segment];
                pAudioDescriptor->uiRawDataLength = hMuxOutput->descriptorInfo.adtsSegmentRawLength[segment];
                hMuxOutput->descriptorInfo.adtsSegment = segment+1;
            }
            break;
        default:
            break;
        }

        /* Advance read pointer appropriately */
        BDBG_MSG(("Advance CDB shadow read from %#x to %#x (len %#x)",psOutputDescDetails->uiCDBBufferShadowReadOffset,
                  psOutputDescDetails->uiCDBBufferShadowReadOffset+pAudioDescriptor->stCommon.uiLength,
                  pAudioDescriptor->stCommon.uiLength));
        psOutputDescDetails->uiCDBBufferShadowReadOffset += \
                                                            pAudioDescriptor->stCommon.uiLength;
        if ( psOutputDescDetails->uiCDBBufferShadowReadOffset >= \
             uiCDBEndOffset )
        {
            psOutputDescDetails->uiCDBBufferShadowReadOffset -= \
                                                                ( uiCDBEndOffset - uiCDBBaseOffset );
        }

        if ( psOutputDescDetails->uiCDBBufferShadowReadOffset == uiNextBase )
        {
            pAudioDescriptor->stCommon.uiFlags |= BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_FRAME_END;
        }

        BDBG_MSG(("Audio Descriptor Base/Length = %08x/%08x Shadow Read %08x",
                  pAudioDescriptor->stCommon.uiOffset,
                  pAudioDescriptor->stCommon.uiLength,
                  psOutputDescDetails->uiCDBBufferShadowReadOffset
                 ));

        /* If we need to send metadata, send it on the first frame */
        if ( hMuxOutput->sendMetadata && (pAudioDescriptor->stCommon.uiFlags & BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_FRAME_START) )
        {
            BAVC_AudioMetadataDescriptor *pMetadataDescriptor = &psOutputDescDetails->pstMetadataCached[0];
            BAVC_AudioBufferDescriptor *pFrameDescriptor;

            /* This is the first frame so we should always have another descriptor available. Assert for sanity. */
            uiTemp = (psOutputDescDetails->uiDescriptorWriteOffset + 1) % hMuxOutput->createSettings.numDescriptors;
            BDBG_ASSERT(uiTemp != psOutputDescDetails->uiDescriptorReadOffset);
            pFrameDescriptor = &psOutputDescDetails->pstDescriptorsCached[psOutputDescDetails->uiDescriptorWriteOffset];
            psOutputDescDetails->uiDescriptorWriteOffset = uiTemp;

            /* The metadata descriptor must come before the first frame.  Swap them. */
            *pFrameDescriptor = *pAudioDescriptor;   /* Copy frame descriptor contents into second descriptor - we're about to overwrite the old one */

            BKNI_Memset(pAudioDescriptor, 0, sizeof(BAVC_AudioBufferDescriptor));
            /* SW7425-75: For Phase 2.0, only a single metadata descriptor will be sent at the beginning */
            /* TODO: Update this in Phase 3.0 to send a metadata descriptor on each RAP or when the metadata changes */
            pAudioDescriptor->stCommon.uiOffset = 0;
            pAudioDescriptor->stCommon.uiLength = sizeof( BAVC_AudioMetadataDescriptor );
            pAudioDescriptor->stCommon.uiFlags |= BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_METADATA;
            pAudioDescriptor->uiDataUnitType = BAVC_AudioMetadataType_eCommon;
            
            /* Populate metadata */
            BKNI_Memset(pMetadataDescriptor, 0, sizeof(*pMetadataDescriptor));

            if ( stMetadata.uiMetadataFlags & BAVC_AUDIOMETADATADESCRIPTOR_FLAGS_BITRATE_VALID )
            {
                pMetadataDescriptor->uiMetadataFlags |= BAVC_AUDIOMETADATADESCRIPTOR_FLAGS_BITRATE_VALID;
                pMetadataDescriptor->stBitrate = stMetadata.stBitrate;
            }
            
            /* Set Sample Rate */
            pMetadataDescriptor->uiMetadataFlags |= BAVC_AUDIOMETADATADESCRIPTOR_FLAGS_SAMPLING_FREQUENCY_VALID;
            pMetadataDescriptor->stSamplingFrequency.uiSamplingFrequency = hMuxOutput->input->format.sampleRate;
            
            /* Set STC Snapshot */
            pMetadataDescriptor->uiMetadataFlags |= BAVC_AUDIOMETADATADESCRIPTOR_FLAGS_TIMING_VALID;
            pMetadataDescriptor->stTiming = stMetadata.stTiming;

            /* Set Protocol */
            BDBG_MSG(("Setting protocol to %u", BAPE_FMT_P_GetAudioCompressionStd(&hMuxOutput->input->format)));
            pMetadataDescriptor->eProtocol = BAPE_FMT_P_GetAudioCompressionStd(&hMuxOutput->input->format);
            switch ( pMetadataDescriptor->eProtocol )
            {
            case BAVC_AudioCompressionStd_eAacAdts:
            case BAVC_AudioCompressionStd_eAacPlusAdts:
                /* ASSUMPTION: This is the first frame and does not need to handle CDB Wrap or partial frames.
                   That would make life much more difficult since metadata comes first, and the DSP should be doing this anyway... */
                ret = BAPE_MuxOutput_P_ParseAdtsMetadata(hMuxOutput, pFrameDescriptor, pMetadataDescriptor);
                if ( ret == BERR_SUCCESS )
                {
                    pMetadataDescriptor->uiMetadataFlags |= BAVC_AUDIOMETADATADESCRIPTOR_FLAGS_PROTOCOL_DATA_VALID;
                }
                else
                {
                    BDBG_WRN(("Unable to parse ADTS metadata."));
                    ret = BERR_SUCCESS;
                }
                break;
            case BAVC_AudioCompressionStd_eWmaStd:
                if ( stMetadata.uiMetadataFlags & BAVC_AUDIOMETADATADESCRIPTOR_FLAGS_PROTOCOL_DATA_VALID )
                {
                    BAPE_EncoderCodecSettings encoderCodecSettings;                    

                    pMetadataDescriptor->uiMetadataFlags |= BAVC_AUDIOMETADATADESCRIPTOR_FLAGS_PROTOCOL_DATA_VALID;
                    pMetadataDescriptor->uProtocolData = stMetadata.uProtocolData;  /* Metadata was parsed above */

                    BAPE_Encoder_GetCodecSettings(hMuxOutput->input->pParent->pHandle, BAVC_AudioCompressionStd_eWmaStd, &encoderCodecSettings);
                    pMetadataDescriptor->uProtocolData.stWmaStd.uiNumChannels = (encoderCodecSettings.codecSettings.wmaStd.channelMode==BAPE_ChannelMode_e1_0)?1:2;
                }
                else
                {
                    BDBG_WRN(("WMA Metadata not available"));
                }
                break;
            default:
                break;
            }
            BDBG_MSG(("Sending Metadata for codec %s (%u) - Flags %#x", BAPE_P_GetCodecName(pMetadataDescriptor->eProtocol), pMetadataDescriptor->eProtocol, pMetadataDescriptor->uiMetadataFlags));

            hMuxOutput->sendMetadata = false;
        }
    }

    if ( hMuxOutput->sendEos )
    {
        BDBG_MSG(("EOS Required"));
        /* Get a descriptor for EOS */
        uiTemp = (psOutputDescDetails->uiDescriptorWriteOffset + 1) % hMuxOutput->createSettings.numDescriptors;
        if ( uiTemp == psOutputDescDetails->uiDescriptorReadOffset )
        {
            BDBG_MSG(("Out of descriptors, can't send EOS"));
        }
        else
        {
            pAudioDescriptor = &psOutputDescDetails->pstDescriptorsCached[psOutputDescDetails->uiDescriptorWriteOffset];
            psOutputDescDetails->uiDescriptorWriteOffset = uiTemp;

            BKNI_Memset(pAudioDescriptor, 0, sizeof(BAVC_AudioBufferDescriptor));

            /* Decoder is stopped and we have run out of data. Fill the EOS entry in Audio descriptor */
            pAudioDescriptor->stCommon.uiFlags = BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_EOS;
            BDBG_MSG(("EOS %#x", pAudioDescriptor));
            hMuxOutput->sendEos = false;
        }
    }

    /* Assign array(s) and count(s) */
    if ( psOutputDescDetails->uiDescriptorWriteOffset >= \
         psOutputDescDetails->uiDescriptorReadOffset )
    {
        *pBuffer = &psOutputDescDetails->pstDescriptorsCached[psOutputDescDetails->uiDescriptorReadOffset];
        *pSize = psOutputDescDetails->uiDescriptorWriteOffset - psOutputDescDetails->uiDescriptorReadOffset;

        *pBuffer2 = NULL;
        *pSize2 = 0;         
    }
    else
    {
        *pBuffer = &psOutputDescDetails->pstDescriptorsCached[psOutputDescDetails->uiDescriptorReadOffset];
        *pSize = hMuxOutput->createSettings.numDescriptors - psOutputDescDetails->uiDescriptorReadOffset;

        *pBuffer2 = &psOutputDescDetails->pstDescriptorsCached[0];
        *pSize2 = psOutputDescDetails->uiDescriptorWriteOffset;
    }

    BDBG_MSG(("pSize = %d",(*pSize)));
    for ( uiTemp=0;uiTemp < (*pSize);uiTemp++ )
        BDBG_MSG(("astDescriptors0[%d] = 0x%x (flags %#x, len %u)",uiTemp,&psOutputDescDetails->pstDescriptorsCached[psOutputDescDetails->uiDescriptorReadOffset+uiTemp],
                  psOutputDescDetails->pstDescriptorsCached[psOutputDescDetails->uiDescriptorReadOffset+uiTemp].stCommon.uiFlags,
                  psOutputDescDetails->pstDescriptorsCached[psOutputDescDetails->uiDescriptorReadOffset+uiTemp].stCommon.uiLength));

    BDBG_MSG(("pSize2 = %d",(*pSize2)));
    for ( uiTemp=0;uiTemp < (*pSize2);uiTemp++ )
        BDBG_MSG(("astDescriptors1[%d] = 0x%x (flags %#x, len %u)",uiTemp,&psOutputDescDetails->pstDescriptorsCached[uiTemp],
                  psOutputDescDetails->pstDescriptorsCached[uiTemp].stCommon.uiFlags,
                  psOutputDescDetails->pstDescriptorsCached[uiTemp].stCommon.uiLength));

    psOutputDescDetails->numOutstandingDescriptors = (*pSize) + (*pSize2);
    BDBG_MSG(("Returning %u descriptors", psOutputDescDetails->numOutstandingDescriptors));

    BDBG_LEAVE(BAPE_MuxOutput_GetBufferDescriptors);

    return ret;
}

BERR_Code BAPE_MuxOutput_ConsumeBufferDescriptors(
    BAPE_MuxOutputHandle hMuxOutput,
    unsigned numBufferDescriptors /* must be <= pSize+pSize2 returned by last BAPE_MuxOutput_GetBufferDescriptors call. */
    )
{
    BREG_Handle hReg;
    BERR_Code   ret = BERR_SUCCESS;
    uint32_t uiCDBReadOffset;
    uint32_t uiCDBEndOffset;
    uint32_t uiCDBBaseOffset;
    uint32_t uiCDBValidOffset;  
    uint32_t uiCDBDepth;
    uint32_t uiCDBIncrement=0;
    BAPE_OutputDescriptorInfo  *psOutputDescDetails;

    BDBG_OBJECT_ASSERT(hMuxOutput, BAPE_MuxOutput);
    BDBG_ASSERT(numBufferDescriptors > 0);  

    BDBG_ENTER(BAPE_MuxOutput_ConsumeBufferDescriptors);

    BDBG_MSG(("BAPE_MuxOutput_ConsumeBufferDescriptors: uiNumBufferDescriptors = %d",numBufferDescriptors));
    hReg = hMuxOutput->node.deviceHandle->regHandle;
    psOutputDescDetails = &hMuxOutput->descriptorInfo;

    /* If a watchdog or stop/start event came while the mux is running, we may receive a request to drop more descriptors than we have.
       Warn and drop the ones that are actually outstanding */
    if ( numBufferDescriptors > psOutputDescDetails->numOutstandingDescriptors )
    {
        if ( 0 == psOutputDescDetails->numOutstandingDescriptors )
        {
            return BERR_SUCCESS;
        }
        else
        {
            BDBG_WRN(("Request to consume %u descriptors, but %u are available.  Consuming %u.", 
                      numBufferDescriptors, psOutputDescDetails->numOutstandingDescriptors));
            numBufferDescriptors = psOutputDescDetails->numOutstandingDescriptors;
        }
    }
    psOutputDescDetails->numOutstandingDescriptors -= numBufferDescriptors;

    /* Read CDB Addresses */           
    uiCDBBaseOffset = BREG_Read32(hReg, hMuxOutput->createSettings.pContextMap->CDB_Base);
    uiCDBEndOffset = BREG_Read32(hReg, hMuxOutput->createSettings.pContextMap->CDB_End);           
    uiCDBValidOffset = BREG_Read32(hReg, hMuxOutput->createSettings.pContextMap->CDB_Valid);
    uiCDBReadOffset = BREG_Read32(hReg, hMuxOutput->createSettings.pContextMap->CDB_Read);
    uiCDBEndOffset++;

    if ( uiCDBValidOffset == uiCDBReadOffset )
    {
        uiCDBDepth = 0;
    }
    else if ( uiCDBValidOffset > uiCDBReadOffset )
    {
        uiCDBDepth = uiCDBValidOffset - uiCDBReadOffset;
    }
    else
    {
        uiCDBDepth = uiCDBEndOffset - uiCDBReadOffset;
        uiCDBDepth += uiCDBValidOffset - uiCDBBaseOffset;
    }

    while ( numBufferDescriptors )
    {
        if ( 0 == (psOutputDescDetails->pstDescriptorsCached[psOutputDescDetails->uiDescriptorReadOffset].stCommon.uiFlags & (BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_EOS|BAVC_COMPRESSEDBUFFERDESCRIPTOR_FLAGS_METADATA)) )
        {
            /* Move CDB Read Offset */
            uiCDBIncrement += psOutputDescDetails->pstDescriptorsCached[psOutputDescDetails->uiDescriptorReadOffset].stCommon.uiLength;
        }

        /* Move Descriptor Read Offset */
        psOutputDescDetails->uiDescriptorReadOffset++;
        psOutputDescDetails->uiDescriptorReadOffset %= hMuxOutput->createSettings.numDescriptors;

        numBufferDescriptors--;
    }

    if ( uiCDBIncrement > uiCDBDepth )
    {
        BDBG_ERR(("Attempting to consume %u bytes from CDB when only %u are present.", uiCDBIncrement, uiCDBDepth));
        BDBG_ASSERT(uiCDBDepth >= uiCDBIncrement);
    }
    else
    {
        BDBG_MSG(("Consume %u of %u bytes from CDB", uiCDBIncrement, uiCDBDepth));
    }

    uiCDBReadOffset += uiCDBIncrement;
    if ( uiCDBReadOffset >= uiCDBEndOffset )
    {
        uiCDBReadOffset -= ( uiCDBEndOffset - uiCDBBaseOffset );
    }

    BDBG_MSG(("BRAP_UpdateBufferDescriptors :uiDescriptorReadOffset = %d",\
              psOutputDescDetails->uiDescriptorReadOffset));

    /* Update Actual ITB/CDB Read Pointers */
    BREG_Write32(hReg, hMuxOutput->createSettings.pContextMap->CDB_Read, uiCDBReadOffset);
    /* No need to compute the ITB amount - just consume all previously used ITB entries in one shot. */
    BREG_Write32(hReg, hMuxOutput->createSettings.pContextMap->ITB_Read, psOutputDescDetails->uiITBBufferShadowReadOffset);

    BDBG_LEAVE (BAPE_MuxOutput_ConsumeBufferDescriptors);
    return ret;    
}

BERR_Code BAPE_MuxOutput_GetBufferStatus(
    BAPE_MuxOutputHandle hMuxOutput,
    BAVC_AudioBufferStatus *pBufferStatus    /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(hMuxOutput, BAPE_MuxOutput);
    BDBG_ASSERT(NULL != pBufferStatus);

    pBufferStatus->stCommon.pFrameBufferBaseAddress = hMuxOutput->pCdbBaseCached;
    pBufferStatus->stCommon.pMetadataBufferBaseAddress = hMuxOutput->descriptorInfo.pstMetadataCached;

    return BERR_SUCCESS;
}

void BAPE_MuxOutput_GetConnector(
    BAPE_MuxOutputHandle hMuxOutput,
    BAPE_Connector *pConnector
    )
{
    BDBG_OBJECT_ASSERT(hMuxOutput, BAPE_MuxOutput);
    BDBG_ASSERT(NULL != pConnector);
    *pConnector = &hMuxOutput->node.connectors[0];
}

BERR_Code BAPE_MuxOutput_AddInput(
    BAPE_MuxOutputHandle hMuxOutput,
    BAPE_Connector input
    )
{
    BERR_Code errCode;
    BDBG_OBJECT_ASSERT(hMuxOutput, BAPE_MuxOutput);
    BDBG_OBJECT_ASSERT(input, BAPE_PathConnector);
    if ( NULL != hMuxOutput->input )
    {
        BDBG_ERR(("Can not have more than one input"));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    errCode = BAPE_PathNode_P_AddInput(&hMuxOutput->node, input);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    hMuxOutput->input = input;
    return BERR_SUCCESS;
}

BERR_Code BAPE_MuxOutput_RemoveInput(
    BAPE_MuxOutputHandle hMuxOutput,
    BAPE_Connector input
    )
{
    BERR_Code errCode;
    BDBG_OBJECT_ASSERT(hMuxOutput, BAPE_MuxOutput);
    BDBG_OBJECT_ASSERT(input, BAPE_PathConnector);
    if ( input != hMuxOutput->input )
    {
        BDBG_ERR(("Input %s %s (%#x) is not connected", input->pParent->pName, input->pName, input));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    errCode = BAPE_PathNode_P_RemoveInput(&hMuxOutput->node, input);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    hMuxOutput->input = NULL;
    return BERR_SUCCESS;
}

BERR_Code BAPE_MuxOutput_RemoveAllInputs(
    BAPE_MuxOutputHandle hMuxOutput
    )
{
    BDBG_OBJECT_ASSERT(hMuxOutput, BAPE_MuxOutput);
    if ( hMuxOutput->input )
    {
        return BAPE_MuxOutput_RemoveInput(hMuxOutput, hMuxOutput->input);
    }
    return BERR_SUCCESS;
}

static BERR_Code BAPE_MuxOutput_P_AllocatePathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection)
{
    BERR_Code errCode;
    BAPE_MuxOutputHandle hMuxOutput;
    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);
    BDBG_ASSERT(NULL != pConnection->pSource->pTaskCreateSettings);
    hMuxOutput = pNode->pHandle;
    BDBG_OBJECT_ASSERT(hMuxOutput, BAPE_MuxOutput);
    /* Add stage to CIT and propagate task settings */
    errCode = BAPE_DSP_P_AddProcessingStage(pConnection->pSource->pTaskCreateSettings, pConnection->pSource->branchId, pConnection->pSource->stageId, 
                                            BDSP_AF_P_DistinctOpType_eCdb, 
                                            BDSP_AudioProcessing_eGenCdbItb,
                                            false,
                                            &hMuxOutput->branchId, &hMuxOutput->stageId);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    /* Add RAVE output buffer */
    errCode = BAPE_DSP_P_AddRaveOutputBuffer(pConnection->pSource->pTaskCreateSettings->pBranchInfo[hMuxOutput->branchId],
                                             hMuxOutput->stageId, hMuxOutput->createSettings.pContextMap);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    /* If configuring for NRT, setup STC increment linkage */
    if ( hMuxOutput->startSettings.pNonRealTimeIncrement )
    {
        pConnection->pSource->pTaskCreateSettings->stcIncrementConfig.enableStcTrigger = true;
        pConnection->pSource->pTaskCreateSettings->stcIncrementConfig.stcIncHiAddr = hMuxOutput->startSettings.pNonRealTimeIncrement->StcIncHi + BCHP_PHYSICAL_OFFSET;
        pConnection->pSource->pTaskCreateSettings->stcIncrementConfig.stcIncLowAddr = hMuxOutput->startSettings.pNonRealTimeIncrement->StcIncLo + BCHP_PHYSICAL_OFFSET;
        pConnection->pSource->pTaskCreateSettings->stcIncrementConfig.stcIncTrigAddr = hMuxOutput->startSettings.pNonRealTimeIncrement->IncTrigger + BCHP_PHYSICAL_OFFSET;
        #ifdef BCHP_XPT_PCROFFSET_STC0_INC_TRIG
        pConnection->pSource->pTaskCreateSettings->stcIncrementConfig.triggerBit = BCHP_XPT_PCROFFSET_STC0_INC_TRIG_SOFT_INC_TRIG_SHIFT;
        #endif
        /* TODO: This shouldn't be mutually exclusive with Master/Slave */
        pConnection->pSource->pTaskCreateSettings->realtimeMode = BDSP_TaskRealtimeMode_eNonRealTime;
    }

    return BERR_SUCCESS;
}

static BERR_Code BAPE_MuxOutput_P_ConfigPathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection)
{
    BERR_Code errCode;
    BAPE_MuxOutputHandle hMuxOutput;
    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);
    BDBG_ASSERT(NULL != pConnection->pSource->task);
    /* Need to apply settings to DSP and propagate task hMuxOutput */
    hMuxOutput = pNode->pHandle;
    BDBG_OBJECT_ASSERT(hMuxOutput, BAPE_MuxOutput);
    hMuxOutput->task = pConnection->pSource->task;
    errCode = BAPE_MuxOutput_P_ApplyDspSettings(hMuxOutput);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    return BERR_SUCCESS;
}

static void BAPE_MuxOutput_P_StopPathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection)
{
    /* Invalidate task handle */
    BAPE_MuxOutputHandle hMuxOutput;
    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BSTD_UNUSED(pConnection);
    hMuxOutput = pNode->pHandle;
    BDBG_OBJECT_ASSERT(hMuxOutput, BAPE_MuxOutput);
    hMuxOutput->task = NULL;
    BDBG_MSG(("Requesting EOS on Task Stop"));
    hMuxOutput->sendEos = true;
}

BERR_Code BAPE_MuxOutput_GetDelayStatus(
    BAPE_MuxOutputHandle hMuxOutput,
    BAVC_AudioCompressionStd codec,
    BAPE_MuxOutputDelayStatus *pStatus    /* [out] */
    )
{
    unsigned frameTime;
    BDBG_OBJECT_ASSERT(hMuxOutput, BAPE_MuxOutput);
    BDBG_ASSERT(NULL != pStatus);
    pStatus->endToEndDelay = 0;
    if ( NULL == hMuxOutput->input )
    {
        BDBG_ERR(("Not connected to any input.  Can not determine delay status."));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    /* Determine codec delay */
    switch (codec)
    {
    /* End to End delay (Time difference between when First PCM sample comes to Encode buffer
       To the time when reference decoder can start decoding) is 2*FrameTime. 32kHz is chosen to 
       take care of worst case delay */
    case BAVC_AudioCompressionStd_eMpegL1:
    case BAVC_AudioCompressionStd_eMpegL2:
    case BAVC_AudioCompressionStd_eMpegL3:
        frameTime = 1152/32;
        break;
    case BAVC_AudioCompressionStd_eAc3:
    case BAVC_AudioCompressionStd_eAc3Plus:
    case BAVC_AudioCompressionStd_eAc3Lossless:
        frameTime = 1536/32;
        break;
    case BAVC_AudioCompressionStd_eAacAdts:
    case BAVC_AudioCompressionStd_eAacLoas:
    case BAVC_AudioCompressionStd_eAacPlusAdts:
    case BAVC_AudioCompressionStd_eAacPlusLoas:
    default:
        frameTime = 2048/32;
        break;
    }
    if ( hMuxOutput->input->pParent->type == BAPE_PathNodeType_eDecoder )
    {
        /* Decoder input */
        pStatus->endToEndDelay = frameTime;
    }
    else
    {
        /* Encoder requires an extra frame of buffering */
        pStatus->endToEndDelay = frameTime * 2;
    }

    return BERR_SUCCESS;
}

static void BAPE_MuxOutput_P_RemoveInputCallback(struct BAPE_PathNode *pNode, BAPE_PathConnector *pConnector)
{
    (void)BAPE_MuxOutput_RemoveInput(pNode->pHandle, pConnector);
}

/* Get type of next entry */
static uint8_t BAPE_MuxOutput_P_GetNextItbEntryType(BAPE_MuxOutputHandle hMuxOutput, uint32_t readOffset)
{
    void *pSource;
    BAPE_ItbEntry *pEntry;

    BMEM_Heap_ConvertOffsetToAddress(hMuxOutput->hItbHeap, readOffset, &pSource);
    
    pEntry = pSource;
    return BAPE_ITB_GET_FIELD(pEntry,GENERIC,ENTRY_TYPE);
}

/* Read and hMuxOutput wraparound */
static size_t BAPE_MuxOutput_P_ReadItb(BAPE_MuxOutputHandle hMuxOutput, uint32_t baseOffset, uint32_t readOffset, uint32_t endOffset, uint32_t depth, uint8_t *pDest, size_t length)
{
    void *pSource;

    if ( length <= depth )
    {
        BMEM_Heap_ConvertOffsetToAddress(hMuxOutput->hItbHeap, readOffset, &pSource);
        if ( readOffset + length > endOffset )
        {
            size_t preWrapAmount = endOffset-readOffset;
            size_t wrapAmount = length - preWrapAmount;
            /* Wraparound */
            BKNI_Memcpy(pDest, pSource, preWrapAmount);
            BMEM_Heap_ConvertOffsetToAddress(hMuxOutput->hItbHeap, baseOffset, &pSource);
            BKNI_Memcpy(pDest+preWrapAmount, pSource, wrapAmount);
        }
        else
        {
            /* No Wrap */
            BKNI_Memcpy(pDest, pSource, length);
        }
        
        return length;
    }
    else
    {
        return 0;
    }
}

static void BAPE_MuxOutput_P_ParseItb(BAPE_MuxOutputHandle hMuxOutput, BAPE_FrameItbEntries **pCurrent, BAPE_FrameItbEntries **pNext, BAVC_AudioMetadataDescriptor *pMetadataDescriptor, BAPE_ItbEntry **pMetadata)    
{
    BREG_Handle hReg;
    uint32_t uiITBBaseOffset;
    uint32_t uiITBEndOffset;
    uint32_t uiITBValidOffset;
    uint32_t uiITBReadOffset;
    uint32_t uiITBDepth;
    uint32_t uiShadowReadOffset;
    uint32_t uiNextEntryOffset;
    size_t uiAmountRead;
    uint8_t entryType;
    BAPE_OutputDescriptorInfo *psOutputDescDetails;
    BAPE_FrameItbEntries *pITBEntry, *pITBEntryNext;
    BAPE_ItbEntry *pITBEntryMetadata;

    hReg = hMuxOutput->node.deviceHandle->regHandle;
    psOutputDescDetails = &hMuxOutput->descriptorInfo;

    /* Invalidate pointers */
    *pCurrent = NULL;
    *pNext = NULL;
    *pMetadata = NULL;

    /* Setup Metadata */
    BKNI_Memset(pMetadataDescriptor, 0, sizeof(*pMetadataDescriptor));

    /* Read ITB Addresses */
    uiITBBaseOffset = BREG_Read32(hReg, hMuxOutput->createSettings.pContextMap->ITB_Base);
    uiITBEndOffset = BREG_Read32(hReg, hMuxOutput->createSettings.pContextMap->ITB_End);
    uiITBValidOffset= BREG_Read32(hReg, hMuxOutput->createSettings.pContextMap->ITB_Valid);
    uiITBReadOffset= BREG_Read32(hReg, hMuxOutput->createSettings.pContextMap->ITB_Read);

    if ( uiITBEndOffset != 0 )
    {
        uiITBEndOffset += 1; /* end is inclusive */
    }

    BDBG_MSG(("ITB Base/End/Shadow Read/Valid = %08x/%08x/%08x (%08x)/%08x",\
              uiITBBaseOffset,
              uiITBEndOffset,
              psOutputDescDetails->uiITBBufferShadowReadOffset,
              uiITBReadOffset,
              uiITBValidOffset
             ));


    uiShadowReadOffset = psOutputDescDetails->uiITBBufferShadowReadOffset;
    for ( ;; )
    {
        pITBEntry = NULL;
        pITBEntryNext = NULL;
        pITBEntryMetadata = NULL;

        if ( uiITBValidOffset >= uiShadowReadOffset )
        {
            uiITBDepth = uiITBValidOffset - \
                         uiShadowReadOffset;
        }
        else
        {
            uiITBDepth = uiITBEndOffset - uiShadowReadOffset;
            uiITBDepth += uiITBValidOffset - uiITBBaseOffset;
        }

        BDBG_MSG(("ITB Depth: %d bytes (Valid: %08x, Shadow Read: %08x)",
                  uiITBDepth,
                  uiITBValidOffset,
                  uiShadowReadOffset
                 ));

        /* Don't attempt to read the next entry if it's invaild */
        if ( uiITBDepth < 16 )
        {
            BDBG_MSG(("No ITB entries available"));
            return;
        }

        /* Check for odd ITB entries and drop them */
        entryType = BAPE_MuxOutput_P_GetNextItbEntryType(hMuxOutput, uiShadowReadOffset);
        if ( entryType != BAPE_ITB_ENTRY_TYPE_BASE_ADDRESS )
        {
            /* this should never happen. We should be in sync looking for this entry drops should happen looking for the next one */
            BDBG_WRN(("Dropping ITB Entry type 0x%02x looking for first", entryType));
            uiShadowReadOffset += 16;
            if ( uiShadowReadOffset >= uiITBEndOffset )
            {
                uiShadowReadOffset -= (uiITBEndOffset-uiITBBaseOffset);
            }
            continue;
        }

        uiAmountRead = BAPE_MuxOutput_P_ReadItb(hMuxOutput, uiITBBaseOffset, uiShadowReadOffset, uiITBEndOffset, uiITBDepth, 
                                                (uint8_t *)&psOutputDescDetails->itb.current, sizeof(BAPE_FrameItbEntries));
        if ( 0 == uiAmountRead )
        {
            /* We ran out of ITB entries */
            BDBG_MSG(("No more ITB Entries"));
            psOutputDescDetails->uiITBBufferShadowReadOffset = uiShadowReadOffset;
            return;
        }
    
        uiITBDepth -= uiAmountRead;
        pITBEntry = &psOutputDescDetails->itb.current;

        /* Check other fields for sanity */            
        BDBG_ASSERT(BAPE_ITB_GET_FIELD(&pITBEntry->ptsDts, GENERIC, ENTRY_TYPE) == BAPE_ITB_ENTRY_TYPE_PTS_DTS);
        BDBG_ASSERT(BAPE_ITB_GET_FIELD(&pITBEntry->bitRate, GENERIC, ENTRY_TYPE) == BAPE_ITB_ENTRY_TYPE_BIT_RATE);
        BDBG_ASSERT(BAPE_ITB_GET_FIELD(&pITBEntry->escrMetadata, GENERIC, ENTRY_TYPE) == BAPE_ITB_ENTRY_TYPE_MUX_ESCR);
    
        BDBG_MSG(("*** ITB Dump (entry size = %d bytes)***", sizeof(BAPE_FrameItbEntries)));
        BDBG_MSG(("\n error = %1x\n valid = %1x cdb_address = %08x\n length = %04x\n pts[32] = %1x\n \
            \n pts[31:0] = %08x\n stc[31:0] = %08x\n ticks_per_bit = %04x\n \
            \n shr = %4d\n samplerate = %08x\n ui32ESCR = %08x\n ui32OriginalPTS = %8x\n",
                  BAPE_ITB_GET_FIELD(&pITBEntry->baseAddress, BASE_ADDRESS, ERROR),
                  BAPE_ITB_GET_FIELD(&pITBEntry->baseAddress, BASE_ADDRESS, FRAME_VALID),
                  BAPE_ITB_GET_FIELD(&pITBEntry->baseAddress, BASE_ADDRESS, CDB_ADDRESS),
                  BAPE_ITB_GET_FIELD(&pITBEntry->baseAddress, BASE_ADDRESS, FRAME_LENGTH),
                  BAPE_ITB_GET_FIELD(&pITBEntry->ptsDts, PTS_DTS, PTS_32),
                  BAPE_ITB_GET_FIELD(&pITBEntry->ptsDts, PTS_DTS, PTS),
                  BAPE_ITB_GET_FIELD(&pITBEntry->ptsDts, PTS_DTS, STC_LOWER),
                  BAPE_ITB_GET_FIELD(&pITBEntry->bitRate, BIT_RATE, TICKS_PER_BIT),
                  BAPE_ITB_GET_FIELD(&pITBEntry->bitRate, BIT_RATE, SHR),
                  BAPE_ITB_GET_FIELD(&pITBEntry->bitRate, BIT_RATE, SAMPLE_RATE),
                  BAPE_ITB_GET_FIELD(&pITBEntry->escrMetadata, ESCR_METADATA, ESCR),
                  BAPE_ITB_GET_FIELD(&pITBEntry->escrMetadata, ESCR_METADATA, ORIGINAL_PTS)
                 ));        
    
        uiNextEntryOffset = uiShadowReadOffset + sizeof(BAPE_FrameItbEntries);
        if ( uiNextEntryOffset >= uiITBEndOffset )
        {
            uiNextEntryOffset -= (uiITBEndOffset-uiITBBaseOffset);
        }

        pMetadataDescriptor->uiMetadataFlags |= BAVC_AUDIOMETADATADESCRIPTOR_FLAGS_TIMING_VALID;
        pMetadataDescriptor->stTiming.uiSTCSnapshot = BAPE_ITB_GET_FIELD(&pITBEntry->ptsDts, PTS_DTS, STC_UPPER);
        pMetadataDescriptor->stTiming.uiSTCSnapshot <<= 32;
        pMetadataDescriptor->stTiming.uiSTCSnapshot |= BAPE_ITB_GET_FIELD(&pITBEntry->ptsDts, PTS_DTS, STC_LOWER);

        while ( uiITBDepth >= sizeof(BAPE_FrameItbEntries) && NULL == pITBEntryNext )
        {
            /* Check for odd ITB entries and drop them */
            entryType = BAPE_MuxOutput_P_GetNextItbEntryType(hMuxOutput, uiNextEntryOffset);
            if ( entryType != BAPE_ITB_ENTRY_TYPE_BASE_ADDRESS )
            {
                switch ( entryType )
                {
                case BAPE_ITB_ENTRY_TYPE_ASC:
                    /* Parse WMA Algo-specific info */
                    if ( BAPE_FMT_P_GetAudioCompressionStd(&hMuxOutput->input->format) == BAVC_AudioCompressionStd_eWmaStd )
                    {
                        BAPE_ItbEntry wmaEntry;
                        uiAmountRead = BAPE_MuxOutput_P_ReadItb(hMuxOutput, uiITBBaseOffset, uiNextEntryOffset, uiITBEndOffset, uiITBDepth, 
                                                                (uint8_t *)&wmaEntry, sizeof(wmaEntry));
                        if ( uiAmountRead != sizeof(wmaEntry) )
                        {
                            BDBG_MSG(("WMA entry unavailable"));
                            return;
                        }
                        pMetadataDescriptor->uProtocolData.stWmaStd.uiSamplesPerBlock = BAPE_ITB_GET_FIELD(&wmaEntry, WMA_SPECIFIC, SAMPLES_PER_FRAME);
                        pMetadataDescriptor->uProtocolData.stWmaStd.uiEncodeOptions = BAPE_ITB_GET_FIELD(&wmaEntry, WMA_SPECIFIC, ENCODE_OPTION);
                        pMetadataDescriptor->uProtocolData.stWmaStd.uiSuperBlockAlign = BAPE_ITB_GET_FIELD(&wmaEntry, WMA_SPECIFIC, SUPER_BLOCK_ALIGN);
                        pMetadataDescriptor->uProtocolData.stWmaStd.uiBlockAlign = BAPE_ITB_GET_FIELD(&wmaEntry, WMA_SPECIFIC, BLOCK_ALIGN);
                        pMetadataDescriptor->uiMetadataFlags |= BAVC_AUDIOMETADATADESCRIPTOR_FLAGS_PROTOCOL_DATA_VALID;
                        BDBG_MSG(("WMA Protocol Data - Samples Per Block %u SuperBlockAlign %u Options %#x", 
                                  pMetadataDescriptor->uProtocolData.stWmaStd.uiSamplesPerBlock, 
                                  pMetadataDescriptor->uProtocolData.stWmaStd.uiSuperBlockAlign, 
                                  pMetadataDescriptor->uProtocolData.stWmaStd.uiEncodeOptions));
                    }
                    /* TODO: Parse AAC */
                    break;
                case BAPE_ITB_ENTRY_TYPE_ALGO_INFO:
                    {
                        BAPE_ItbEntry algoInfo;
                        uiAmountRead = BAPE_MuxOutput_P_ReadItb(hMuxOutput, uiITBBaseOffset, uiNextEntryOffset, uiITBEndOffset, uiITBDepth, 
                                                                (uint8_t *)&algoInfo, sizeof(algoInfo));
                        if ( uiAmountRead != sizeof(algoInfo) )
                        {
                            BDBG_MSG(("ALGO_INFO entry unavailable"));
                            return;
                        }
                        pMetadataDescriptor->uiMetadataFlags |= BAVC_AUDIOMETADATADESCRIPTOR_FLAGS_BITRATE_VALID;
                        pMetadataDescriptor->stBitrate.uiMax = BAPE_ITB_GET_FIELD(&algoInfo, ALGO_INFO, MAX_BIT_RATE);
                    }
                    break;
                case BAPE_ITB_ENTRY_TYPE_METADATA:                     
                    uiAmountRead = BAPE_MuxOutput_P_ReadItb(hMuxOutput, uiITBBaseOffset, uiNextEntryOffset, uiITBEndOffset, uiITBDepth, 
                                                            (uint8_t *)&psOutputDescDetails->itb.metadata, sizeof(BAPE_ItbEntry));
                    if ( uiAmountRead != sizeof(BAPE_ItbEntry) )
                    {
                        BDBG_MSG(("METADATA entry unavailable"));
                        return;
                    }
                    pITBEntryMetadata = &(psOutputDescDetails->itb.metadata);                   
                    break;
                default:
                    BDBG_WRN(("Dropping ITB Entry type 0x%02x looking for next", entryType));
                    #if 0
                    BDBG_WRN(("uiNextEntryOffset 0x%08x base 0x%08x end 0x%08x shadow 0x%08x valid 0x%08x",uiNextEntryOffset,uiITBBaseOffset,uiITBEndOffset,uiShadowReadOffset,uiITBValidOffset));
                    #endif
                    break;
                }
                uiNextEntryOffset += 16;
                if ( uiNextEntryOffset >= uiITBEndOffset )
                {
                    uiNextEntryOffset -= (uiITBEndOffset-uiITBBaseOffset);
                }
                uiITBDepth -= 16;
                continue;
            }
            
            /* Found a base address entry.  Read the next entry. */
            uiAmountRead = BAPE_MuxOutput_P_ReadItb(hMuxOutput, uiITBBaseOffset, uiNextEntryOffset, uiITBEndOffset, uiITBDepth, 
                                                    (uint8_t *)&psOutputDescDetails->itb.next, sizeof(BAPE_FrameItbEntries));
            if ( 0 == uiAmountRead )
            {
                /* We ran out of ITB entries */
                BDBG_MSG(("Next ITB entry unavailable"));
                return;
            }

            uiITBDepth -= uiAmountRead;
            pITBEntryNext = &psOutputDescDetails->itb.next;
        }

        /* Figure out how much CDB data we have for the current Frame */
        if ( NULL != pITBEntryNext )
        {    
            /* Goto next frame's ITB Entry */
            if ( BAPE_ITB_GET_FIELD(&pITBEntryNext->baseAddress, BASE_ADDRESS, CDB_ADDRESS) == 
                 psOutputDescDetails->uiCDBBufferShadowReadOffset )
            {
                /* We have a next entry, and we've finished with the
                 * current entry, so move to the next entry
                 */                
                uiShadowReadOffset = uiNextEntryOffset;
                psOutputDescDetails->uiITBBufferShadowReadOffset = uiNextEntryOffset;
                if ( BAPE_ITB_GET_FIELD(&pITBEntry->baseAddress, BASE_ADDRESS, FRAME_LENGTH) == 0 )
                {
                    *pCurrent = pITBEntry;
                    *pNext = pITBEntryNext;
                    *pMetadata = pITBEntryMetadata;
                    return;
                }
                BDBG_MSG(("Goto Next Entry"));
                continue;
            }
    
            *pCurrent = pITBEntry;
            *pNext = pITBEntryNext;
            *pMetadata = pITBEntryMetadata;
            return;
        }
        else
        {
            /* We ran out of ITB entries */
            BDBG_MSG(("Next ITB entry unavailable"));
            return;
        }
    }
}


/**
Advance both bit and index by one through the byte array
**/
#define BADTS_ADVANCE(abit, aindex, asize) \
	do {if (abit) {(abit)--;} else {(abit)=7;if(++(aindex)>=asize) {goto err_eof;}} } while (0)

#define BADTS_GET_BITS(stream, val, len, bit, index, size) \
	do { unsigned i_tmp; (val)=0; for ( i_tmp=0; i_tmp < (len); i_tmp++) { (val) <<= 1; (val) |= ((((uint32_t)stream[(index)])>>(bit))&0x1); BADTS_ADVANCE(bit, index, size);} } while (0)

#define BADTS_SET_BITS(stream, val, len, bit, index, size) \
	do { unsigned i_tmp; for ( i_tmp=(len); i_tmp > 0; i_tmp-- ) { (stream[(index)] |= ((((val)>>(i_tmp-1))&0x1)<<(bit))); BADTS_ADVANCE(bit, index, size); } } while (0)

static BERR_Code BAPE_MuxOutput_P_ParseAdtsMetadata(
    BAPE_MuxOutputHandle hMuxOutput, 
    BAVC_AudioBufferDescriptor *pFrameDescriptor, 
    BAVC_AudioMetadataDescriptor *pMetadataDescriptor
    )
{
    /* CDB Data is assumed to be a bytestream */
    unsigned bit=7;
    unsigned index=0;
    unsigned size=pFrameDescriptor->stCommon.uiLength;
    uint32_t value;
    uint32_t adtsId;
    uint32_t adtsLayer;
    uint32_t adtsObject;
    uint32_t adtsRateIndex;
    uint32_t adtsChanCfg;
    uint8_t *pBytestream = (uint8_t *)hMuxOutput->pCdbBaseCached + pFrameDescriptor->stCommon.uiOffset;
    
    BADTS_GET_BITS(pBytestream, value, 12, bit, index, size);

    /* Check syncword */
    if ( value != 0xfff )
    {
        BDBG_MSG(("Bad ADTS Syncword - expected 0xfff got 0x%x [offset=%u]", value, pFrameDescriptor->stCommon.uiOffset));
        goto err_parse;
    }
    /* Get ID */
    BADTS_GET_BITS(pBytestream, adtsId, 1, bit, index, size);
    BDBG_MSG(("ADTS ID: %u", adtsId));
    /* Get Layer */
    BADTS_GET_BITS(pBytestream, adtsLayer, 2, bit, index, size);
    BDBG_MSG(("ADTS Layer: %u", adtsLayer));
    /* Skip Protection */
    BADTS_ADVANCE(bit, index, size);
    /* Get Object Type */
    BADTS_GET_BITS(pBytestream, adtsObject, 2, bit, index, size);
    BDBG_MSG(("ADTS ObjectType: %u", adtsObject));
    /* Get Sample Rate Index */
    BADTS_GET_BITS(pBytestream, adtsRateIndex, 4, bit, index, size);
    BDBG_MSG(("ADTS SR Index: %u", adtsRateIndex));
    /* Skip Private Bit */
    BADTS_ADVANCE(bit, index, size);
    /* Get Channel Config */
    BADTS_GET_BITS(pBytestream, adtsChanCfg, 3, bit, index, size);
    BDBG_MSG(("ADTS Channel Config: %u", adtsChanCfg));

    /* That's enough to get us what we need.  Convert Object Type to value for ASConfig. */
    adtsObject += 1;

    /* Zero out Target Bytestream */
    bit=7;
    index=0;
    pBytestream = pMetadataDescriptor->uProtocolData.stAac.auiASC;
    size = BAVC_AUDIO_SPECIFIC_CONFIG_MAX_LENGTH;
    BKNI_Memset(pBytestream, 0, size);
    /* Write out payload */
    BADTS_SET_BITS(pBytestream, adtsObject, 5, bit, index, size);   /* audioObjectType:5 */
    BADTS_SET_BITS(pBytestream, adtsRateIndex, 4, bit, index, size);   /* samplingFrequency:4 */
    BADTS_SET_BITS(pBytestream, adtsChanCfg, 4, bit, index, size);  /* channelConfiguration:4 */
    /* TODO: SBR? Doesn't seem supported in ADTS (ADTS Object Type is 2 bits, max val 3 + 1 = 4 and SBR is 5) */
    /* Begin GASpecificConfig */
    BADTS_ADVANCE(bit, index, size);    /* frameLenthFlag:1 */
    BADTS_ADVANCE(bit, index, size);    /* dependsOnCoreCoder:1 */
    BADTS_ADVANCE(bit, index, size);    /* extensionFlag:1 */
    if ( adtsChanCfg == 0 )
    {
        /* Program Config element is required.  We don't support this. */
        BDBG_WRN(("PCE Not supported for ADTS."));
        goto err_parse;
    }
    /* Audio Object Types 6 and 20 are not supported, no layerNr. */
    /* No extension flag, so no other data */
    pMetadataDescriptor->uProtocolData.stAac.uiASCLengthBits = (8*index) + (7-bit);
    pMetadataDescriptor->uProtocolData.stAac.uiASCLengthBytes = (bit==7)?index:index+1;
#if 0
    {
        unsigned i;
        for ( i = 0; i < pMetadataDescriptor->uProtocolData.stAac.uiASCLengthBytes; i++ )
        {
            BDBG_WRN(("ASC[%u] = 0x%02x", i, pMetadataDescriptor->uProtocolData.stAac.auiASC[i]));
        }
    }
#endif
    return BERR_SUCCESS;
err_eof:
    BDBG_WRN(("ADTS EOF"));
err_parse:
    return BERR_INVALID_PARAMETER;        
}

/**
Advance both bit and index by one through the byte array
**/
#define BADTS_ADVANCE_WRAP(astream1, astream2, abit, aindex, asize, asize2, awrap) \
	do {if (abit) {(abit)--;} else {(abit)=7;if(++(aindex)>=asize) {if ( (asize2) > 0 && ((astream2) != NULL) ) { (asize)=(asize2);(astream1)=(astream2);(asize2)=0;(astream2)=NULL; (aindex)=0; (awrap)=true;} else { goto err_eof;}}} } while (0)

#define BADTS_SKIP_BITS_WRAP(stream1, stream2, len, bit, index, size, size2, wrap) \
	do { unsigned i_tmp; for ( i_tmp=0; i_tmp < (len); i_tmp++) { BADTS_ADVANCE_WRAP(stream1, stream2, bit, index, size, size2, wrap);} } while (0)

#define BADTS_GET_BITS_WRAP(stream1, stream2, val, len, bit, index, size, size2, wrap) \
	do { unsigned i_tmp; (val)=0; for ( i_tmp=0; i_tmp < (len); i_tmp++) { (val) <<= 1; (val) |= ((((uint32_t)stream1[(index)])>>(bit))&0x1); BADTS_ADVANCE_WRAP(stream1, stream2, bit, index, size, size2, wrap);} } while (0)

#define BADTS_HANDLE_HEADER_WRAP() \
do {\
if ( wrapped )\
{\
    /* Wrapped in header, no actual data */\
    hMuxOutput->descriptorInfo.adtsSegmentOffset[0] = segmentOffset;\
    hMuxOutput->descriptorInfo.adtsSegmentLength[0] = segmentLength;\
    hMuxOutput->descriptorInfo.adtsSegmentRawOffset[0] = segmentOffset;\
    hMuxOutput->descriptorInfo.adtsSegmentRawLength[0] = 0;\
    segmentLength = s1;\
    segmentOffset = (pBuf1-pBase);\
    segment=1;\
    wrapped = false;\
}} while (0)

void BAPE_MuxOutput_P_ParseAdtsSegments( 
    BAPE_MuxOutputHandle hMuxOutput, 
    const uint8_t *pBuffer,
    size_t bufferLength,
    const uint8_t *pWrapBuffer,
    size_t wrapBufferLength
    )
{
    const uint8_t *pBase = hMuxOutput->pCdbBaseCached;
    bool wrapped=false;
    bool protection;
    size_t s1=bufferLength,s2=wrapBufferLength;
    const uint8_t *pBuf1=pBuffer, *pBuf2=pWrapBuffer;
    unsigned bit=7;
    unsigned index=0;
    unsigned frameLength, numBlocks;
    unsigned i;
    unsigned segment, blockLength;
    unsigned segmentOffset, segmentLength;
    unsigned rawOffset, rawLength;
    uint32_t val;
    /* Initialize default segment info */
    hMuxOutput->descriptorInfo.adtsSegment = 0;
    hMuxOutput->descriptorInfo.adtsNumSegments = 0;
    segment = 0;
    segmentOffset = (pBuf1 - pBase);    /* Segment starts at beginning of first buffer */
    segmentLength = s1;                 /* Segment length is until wraparound by default */
    /* Parse header */
    /* adts_fixed_header */
    BADTS_GET_BITS_WRAP(pBuf1, pBuf2, val, 12, bit, index, s1, s2, wrapped);    /* syncword:12 */
    BADTS_HANDLE_HEADER_WRAP();
    if ( val != 0xfff )
    {
        BDBG_MODULE_MSG(bape_adts,("ADTS syncword mismatch expected 0xfff got 0x%03x", val));
        goto err_parse;
    }
    BADTS_ADVANCE_WRAP(pBuf1, pBuf2, bit, index, s1, s2, wrapped);              /* ID:1 */
    BADTS_HANDLE_HEADER_WRAP();
    BADTS_ADVANCE_WRAP(pBuf1, pBuf2, bit, index, s1, s2, wrapped);              /* layer:2 */
    BADTS_HANDLE_HEADER_WRAP();
    BADTS_ADVANCE_WRAP(pBuf1, pBuf2, bit, index, s1, s2, wrapped);              
    BADTS_HANDLE_HEADER_WRAP();
    BADTS_GET_BITS_WRAP(pBuf1, pBuf2, val, 1, bit, index, s1, s2, wrapped);     /* protection_absent:1 */
    BADTS_HANDLE_HEADER_WRAP();
    protection = (val)?false:true;  /* Bit indicates absence of protection */
    BADTS_SKIP_BITS_WRAP(pBuf1, pBuf2, 2, bit, index, s1, s2, wrapped);         /* profile_ObjectType:2 */
    BADTS_HANDLE_HEADER_WRAP();
    BADTS_SKIP_BITS_WRAP(pBuf1, pBuf2, 4, bit, index, s1, s2, wrapped);         /* sampling_frequency_index:4 */
    BADTS_HANDLE_HEADER_WRAP();
    BADTS_ADVANCE_WRAP(pBuf1, pBuf2, bit, index, s1, s2, wrapped);              /* private_bit:1 */
    BADTS_HANDLE_HEADER_WRAP();
    BADTS_SKIP_BITS_WRAP(pBuf1, pBuf2, 3, bit, index, s1, s2, wrapped);         /* channel_configuration:3 */
    BADTS_HANDLE_HEADER_WRAP();
    BADTS_ADVANCE_WRAP(pBuf1, pBuf2, bit, index, s1, s2, wrapped);              /* original_copy:1 */
    BADTS_HANDLE_HEADER_WRAP();
    BADTS_ADVANCE_WRAP(pBuf1, pBuf2, bit, index, s1, s2, wrapped);              /* home:1 */
    BADTS_HANDLE_HEADER_WRAP();
    /* adts_variable_header */
    BADTS_ADVANCE_WRAP(pBuf1, pBuf2, bit, index, s1, s2, wrapped);              /* copyright_identification_bit:1 */
    BADTS_HANDLE_HEADER_WRAP();
    BADTS_ADVANCE_WRAP(pBuf1, pBuf2, bit, index, s1, s2, wrapped);              /* copyright_identification_start:1 */
    BADTS_HANDLE_HEADER_WRAP();
    BADTS_GET_BITS_WRAP(pBuf1, pBuf2, val, 13, bit, index, s1, s2, wrapped);    /* aac_frame_length:13 */
    BADTS_HANDLE_HEADER_WRAP();
    frameLength = val;  /* In Bytes */
    if ( frameLength != bufferLength + wrapBufferLength ) 
    { 
        BDBG_MODULE_MSG(bape_adts,("Frame Length error.  Expected %u got %u", bufferLength+wrapBufferLength, frameLength));
        goto err_parse;
    }
    BADTS_SKIP_BITS_WRAP(pBuf1, pBuf2, 11, bit, index, s1, s2, wrapped);        /* aac_buffer_fullness:11 */
    BADTS_HANDLE_HEADER_WRAP();
    BADTS_GET_BITS_WRAP(pBuf1, pBuf2, val, 2, bit, index, s1, s2, wrapped);     /* number_of_raw_data_blocks_in_frame:2 */
    BADTS_HANDLE_HEADER_WRAP();
    numBlocks = val;
    BDBG_ASSERT(bit == 7);  /* We should be byte aligned now.  If not something has gone horribly wrong. */
    /* handle protection prior to data if required */
    if ( protection )
    {
        if ( numBlocks > 0 )
        {
            /* adts_header_error_check */
            for ( i = 1; i <= numBlocks; i++ ) 
            { 
                BADTS_SKIP_BITS_WRAP(pBuf1, pBuf2, 16, bit, index, s1, s2, wrapped);    /* raw_data_block_position(i):16 */
                BADTS_HANDLE_HEADER_WRAP();
            }
        }
        BADTS_SKIP_BITS_WRAP(pBuf1, pBuf2, 16, bit, index, s1, s2, wrapped);            /* crc_check:16 */
        BADTS_HANDLE_HEADER_WRAP();
    }
    
    numBlocks++;    /* Original numBlocks was inclusive.  That doesn't make sense for dividing. */
    if ( segment == 1 )
    {
        /* We have wrapped in the header */
        blockLength = frameLength - index - hMuxOutput->descriptorInfo.adtsSegmentLength[0];  /* Subtract header bytes pre and post-wrap */
    }
    else
    {
        blockLength = frameLength - index;  /* Subtract header bytes */
    }
    if ( (blockLength % numBlocks) != 0 )
    {
        BDBG_MODULE_MSG(bape_adts,("Raw Frame length not a multiple of number of blocks.  Fail."));
        goto err_parse;
    }
    blockLength /= numBlocks;
    if ( protection )
    {
        blockLength -= 2;   /* Remove 2-byte CRC that comes after each access unit */
    }

    /* First data segment will start here. */    
    rawOffset = (pBuf1-pBase)+index;
    rawLength = blockLength;

    /* Actual first segment starts wherever the header parsing started and contains the header data also */
    segmentOffset = pBuf1-pBase;
    if ( segment == 1 )
    {
        /* Wrapped in header */
        segmentLength = frameLength-hMuxOutput->descriptorInfo.adtsSegmentLength[0];
    }
    else
    {
        segmentLength = frameLength;
    }
    BDBG_MODULE_MSG(bape_adts,("S1 %u Segment Len %u Frame Len %u Raw Len %u nblk %u protection %u index %u", s1, segmentLength, frameLength, rawLength, numBlocks, protection, index));

    /* Iterate through number of blocks (it's not inclusive anymore) */
    for ( i = 0; i < numBlocks; )
    {
        /* Find segment boundary */
        hMuxOutput->descriptorInfo.adtsSegmentOffset[segment] = segmentOffset;
        hMuxOutput->descriptorInfo.adtsSegmentRawOffset[segment] = rawOffset;
        if ( segmentLength >= s1 )
        {
            /* Truncate Segment */
             hMuxOutput->descriptorInfo.adtsSegmentLength[segment] = s1;
             segmentLength -= s1;
             /* Determine if raw payload wrapped (it might wrap in segment CRC region instead) */
             if ( rawLength + (rawOffset-segmentOffset) > s1 )
             {
                 hMuxOutput->descriptorInfo.adtsSegmentRawLength[segment] = s1-(rawOffset-segmentOffset);
                 rawLength -= hMuxOutput->descriptorInfo.adtsSegmentRawLength[segment];
             }
             else
             {
                 hMuxOutput->descriptorInfo.adtsSegmentRawLength[segment] = rawLength;
                 rawLength = 0;
             }             
             /* Wrap pointers */
             s1 = s2;
             pBuf1 = pBuf2;
             s2 = 0;
             pBuf2 = NULL;
             /* Start new segment */
             segmentOffset = pBuf1 - pBase;
             rawOffset = segmentOffset;
        }
        else
        {
            /* No Wrap.  Use full segment */
            hMuxOutput->descriptorInfo.adtsSegmentLength[segment] = segmentLength;
            hMuxOutput->descriptorInfo.adtsSegmentOffset[segment] = segmentOffset;
            hMuxOutput->descriptorInfo.adtsSegmentRawOffset[segment] = rawOffset;
            hMuxOutput->descriptorInfo.adtsSegmentRawLength[segment] = rawLength;
            s1 -= segmentLength;
            pBuf1 += segmentLength;
            segmentOffset += segmentLength;
            rawOffset = segmentOffset;     /* The next RAW block starts with the next segment */
            segmentLength = 0;
            rawLength = 0;
        }

        /* Determine if we need to update lengths */
        if ( 0 == segmentLength )
        {
            rawLength = blockLength;
            segmentLength = (protection)?rawLength + 2:rawLength;
            i++;    /* We consumed a data block */
        }
        
        segment++;
    }
    BDBG_ASSERT(segment <= BAPE_MUXOUTPUT_MAX_ADTS_SEGMENTS);
    BDBG_MODULE_MSG(bape_adts,("ADTS Frame has %u segments", segment));
    hMuxOutput->descriptorInfo.adtsNumSegments = segment;
#if 1
    for ( segment = 0; segment < hMuxOutput->descriptorInfo.adtsNumSegments; segment++ )
    {
        BDBG_MODULE_MSG(bape_adts,("Segment %u length %u", segment, hMuxOutput->descriptorInfo.adtsSegmentLength[segment]));
    }
#endif
    return; /* Success */
err_parse:
err_eof:
    hMuxOutput->descriptorInfo.adtsSegment = 0;
    hMuxOutput->descriptorInfo.adtsNumSegments = 1;
    hMuxOutput->descriptorInfo.adtsSegmentOffset[0] = pBuffer - pBase;
    hMuxOutput->descriptorInfo.adtsSegmentLength[0] = bufferLength;
    hMuxOutput->descriptorInfo.adtsSegmentRawOffset[0] = pBuffer - pBase;
    hMuxOutput->descriptorInfo.adtsSegmentRawLength[0] = bufferLength;
    if ( pWrapBuffer != NULL )
    {
        hMuxOutput->descriptorInfo.adtsNumSegments = 2;
        hMuxOutput->descriptorInfo.adtsSegmentOffset[1] = pWrapBuffer - pBase;
        hMuxOutput->descriptorInfo.adtsSegmentLength[1] = wrapBufferLength;
        hMuxOutput->descriptorInfo.adtsSegmentRawOffset[1] = pWrapBuffer - pBase;
        hMuxOutput->descriptorInfo.adtsSegmentRawLength[1] = wrapBufferLength;
    }    
}

bool BAPE_MuxOutput_P_IsRunning(BAPE_MuxOutputHandle hMuxOutput)
{
    BDBG_OBJECT_ASSERT(hMuxOutput, BAPE_MuxOutput);
    return hMuxOutput->started;
}

