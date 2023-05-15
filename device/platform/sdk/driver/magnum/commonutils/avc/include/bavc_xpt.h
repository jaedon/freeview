/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bavc_xpt.h $
 * $brcm_Revision: Hydra_Software_Devel/20 $
 * $brcm_Date: 10/12/11 6:47p $
 *
 * Module Description:
 *   XPT related defines,removed from bavc.h and included in this file
 * Revision History:
 *
 * $brcm_Log: /magnum/commonutils/avc/7425/bavc_xpt.h $
 * 
 * Hydra_Software_Devel/20   10/12/11 6:47p jtna
 * SW7422-452: add BAVC_ItbEsType_eVp6Video
 * 
 * Hydra_Software_Devel/19   6/7/11 2:27p gmullen
 * SW7425-653: Return STC INC HI and LO registers
 * 
 * Hydra_Software_Devel/18   2/14/11 1:18p gmullen
 * SW7420-1459: Force CDB and ITB alignment per decoder requirements
 * 
 * Hydra_Software_Devel/17   12/7/10 2:17p gmullen
 * SW7425-15: Updated BAVC_TsMux_DescConfig
 * 
 * Hydra_Software_Devel/16   12/1/10 7:57a gmullen
 * SW7425-15: Added TS muxing playback descriptor
 * 
 * Hydra_Software_Devel/15   10/13/09 11:40a gmullen
 * SW7405-3199: Added audio descriptor support for other codecs.
 * 
 * Hydra_Software_Devel/14   9/30/09 9:34a gmullen
 * SW3548-2188: Added AMR support
 * 
 * Hydra_Software_Devel/13   6/10/08 8:31a gmullen
 * PR43467: Added DRA enum.
 * 
 * Hydra_Software_Devel/12   5/19/08 8:18a gmullen
 * PR42623: Changed audio descriptor enum to note MPEG audio support.
 * 
 * Hydra_Software_Devel/11   5/12/08 12:30p gmullen
 * PR42623: Added audio descriptor suppor
 * 
 * Hydra_Software_Devel/10   3/25/08 2:20p vsilyaev
 * PR 39818: Added MKV stream type
 * 
 * Hydra_Software_Devel/9   1/24/08 11:33a gmullen
 * PR38907: Added CDB and ITB END registers to BAVC_XptContextMap
 * 
 * Hydra_Software_Devel/8   1/24/08 11:28a katrep
 * PR38429: Added ITB types for AVS video and Audio
 * 
 * Hydra_Software_Devel/7   11/29/07 3:36p gmullen
 * PR37062: Removed BAVC_ItbEsType_eDvdMpeg2Video.
 * 
 * Hydra_Software_Devel/6   11/28/07 4:57p gmullen
 * PR37062: Added BAVC_ItbEsType_eDvdMpeg2Video for 7401 and 7118 only.
 * 
 * Hydra_Software_Devel/5   10/31/07 4:44p piyushg
 * PR32599: Add new ITB entry type enums for 7440 graphics and HLI format.
 * Add new API BXPT_Rave_AddBppChannel to add multiple pid in the same
 * context without overwriting the previous SCD values.
 * 
 * Hydra_Software_Devel/4   5/30/07 5:50p shyam
 * PR 31696 : Remove redundancies in the BAVC_StreamType enum
 * 
 * Hydra_Software_Devel/3   3/26/07 4:11p katrep
 * PR29067: Added BAVC_ItbEsType_eMlpAudio itb type
 * 
 * Hydra_Software_Devel/2   2/13/07 6:26p shyam
 * PR 27542 : Move some transcoder types into AVC
 * 
 * Hydra_Software_Devel/1   2/8/07 4:57p katrep
 * PR25430: Moved the xpt related defines from bavc.h to bavc_xpt.h
 * 
 *
 ***************************************************************************/

#ifndef BAVC_XPT_H__
#define BAVC_XPT_H__

/***************************************************************************
Summary:
An enumeration describing the transport (xpt) outputs that feed data to the
audio and video decoders.

Description:
Data flows between the transport (xpt) and the audio/video decoders in bands.
The xpt, audio, and video blocks must be told which bands are carrying data.
This enumeration is used to specify those bands.

Note:
Not all BAVC_XptOutputId values may exist for all chips and platforms.

See Also:
BAVC_XptOutput
***************************************************************************/
typedef enum BAVC_XptOutputId
{
    BAVC_XptOutputId_eParserBand0 = 0,   /* band 0 */
    BAVC_XptOutputId_eParserBand1,       /* band 1 */
    BAVC_XptOutputId_eParserBand2,       /* band 2 */
    BAVC_XptOutputId_eParserBand3,       /* band 3 */
    BAVC_XptOutputId_eParserBand4,       /* band 4 */

    BAVC_XptOutputId_eParserBand5,       /* band 5 */
    BAVC_XptOutputId_eParserBand6,       /* band 6 */
    BAVC_XptOutputId_eParserBand7,       /* band 7 */
    BAVC_XptOutputId_eParserBand8,       /* band 8 */

    BAVC_XptOutputId_ePlayback0 = 100,   /* playback 0 */
    BAVC_XptOutputId_ePlayback1,         /* playback 1 */
    BAVC_XptOutputId_ePlayback2,         /* playback 2 */
    BAVC_XptOutputId_ePlayback3          /* playback 3 */
} BAVC_XptOutputId;

/***************************************************************************
Summary:
A structure that is used to communicate the data path between the transport
and audio/video decoders.

Description:
Data can be routed from the transport to the audio/video deocers in the
following ways:

 1. Broadcast source through parser band
 2. Directly from playback channel
 3. Playback channel routed through parser band

During playback operation the audio/video decoders must be able to throttle
the data flow to prevent buffer overflows.  Therefore, the A/V decoders must
be aware of this routing in order to properly configure themselves.

In order to handle all of the above routes two source values are required.
This structure is used as the standard method for passing these two values
to routines.

Example:

// Broadcast decode from parser band 0:

BAVC_XptOutput xptOutput;
xptOutput.eXptOutputId = BAVC_XptOutputId_eParserBand0;
xptOutput.eXptSourceId = BAVC_XptOutputId_eParserBand0;


// Playback directly from playback channel 0:

BAVC_XptOutput xptOutput;
xptOutput.eXptOutputId = BAVC_XptOutputId_ePlayback0;
xptOutput.eXptSourceId = BAVC_XptOutputId_ePlayback0;


// Playback from playback channel 0 through pid parser band 2:

BAVC_XptOutput xptOutput;
xptOutput.eXptOutputId = BAVC_XptOutputId_eParserBand2;
xptOutput.eXptSourceId = BAVC_XptOutputId_ePlayback0;


See Also:
BAVC_XptOutputId
***************************************************************************/
typedef struct BAVC_XptOutput
{
    BAVC_XptOutputId   eXptOutputId;     /* input band which is used to
                                           send data to audio or video
                                           modules */
    BAVC_XptOutputId   eXptSourceId;     /* the source id that is sourcing
                                           eXptOutputId  */
} BAVC_XptOutput;

/***************************************************************************
Summary:
A structure that is used to communicate the data path between the transport
and audio/video decoders.

Description:
The data transport will deliver the coded elementary stream data into DRAM
buffers, whose locations and sizes can be changed by software. This structure
provides a means of communicating the location of the buffers from the
transport to the decoders.
***************************************************************************/
typedef struct BAVC_XptContextMap
{
    uint32_t CDB_Read;      /* Address of the coded data buffer READ register */
    uint32_t CDB_Base;      /* Address of the coded data buffer BASE register */
    uint32_t CDB_Wrap;      /* Address of the coded data buffer WRAPAROUND register */
    uint32_t CDB_Valid;     /* Address of the coded data buffer VALID register */
    uint32_t CDB_End;     	/* Address of the coded data buffer END register */

    uint32_t ITB_Read;      /* Address of the index table buffer READ register */
    uint32_t ITB_Base;      /* Address of the index table buffer BASE register */
    uint32_t ITB_Wrap;      /* Address of the index table buffer WRAPAROUND register */
    uint32_t ITB_Valid;     /* Address of the index table buffer VALID register */
    uint32_t ITB_End;     	/* Address of the index table buffer END register */

    unsigned ContextIdx;        /* Which RAVE context contains the pointers given herein */
    uint32_t PictureCounter;    /* Address of picture counter reg, 0 if no counter is allocated */
    uint32_t PicIncDecCtrl;     /* Picture Counter Increment/Decrement/Reset Control Register */
}
BAVC_XptContextMap;

/***************************************************************************
Summary:
    Defines supported MPEG streaming, packetization or container formats

Description:
    This enum is used to set strean format in audio and video decoders. 

See Also:
****************************************************************************/
typedef enum BAVC_StreamType
{
    BAVC_StreamType_eTsMpeg,        /* MPEG Transport Stream*/
    BAVC_StreamType_eDssEs,         /* DSS(DirecTV Transport) ES Stream */
    BAVC_StreamType_ePes,           /* PES Stream */
    BAVC_StreamType_eEs,            /* ES Stream */
    BAVC_StreamType_eBes,           /* BES Stream format */
    BAVC_StreamType_eDssPes ,       /* DSS(DirecTV Transport) PES (Video HD) Stream */
    BAVC_StreamType_ePS,            /* Program Stream */
    BAVC_StreamType_eCms,           /* Compressed multi-stream */
    BAVC_StreamType_eTsBDRay,       /* Blue Ray Transport Stream*/
    BAVC_StreamType_eMpeg1System,   /* MPEG-1 system stream */
    BAVC_StreamType_eAVI,           /* AVI format */
    BAVC_StreamType_eMPEG4,         /* MP4 (MPEG-4 Part 12 & 14) container format */
    BAVC_StreamType_eMKV            /* matroska container format */
}BAVC_StreamType;

/***************************************************************************
Summary:
    Defines the supported Index Table types.

Description:
    This enum is used to set the type of elementary stream that index table
    entries will be built for. Hardware cannot guess this value.

See Also:
****************************************************************************/
typedef enum BAVC_ItbEsType
{
    BAVC_ItbEsType_eMpeg2Video = 0,
    BAVC_ItbEsType_eAvcVideo = 1,
    BAVC_ItbEsType_eMpegAudio = 2,
    BAVC_ItbEsType_eAacAudio,
    BAVC_ItbEsType_eAc3gAudio,
    BAVC_ItbEsType_eDtsAudio,
    BAVC_ItbEsType_eLpcmAudio,
    BAVC_ItbEsType_eVc1Video,
    BAVC_ItbEsType_eAacHe,
    BAVC_ItbEsType_eAc3Plus,
    BAVC_ItbEsType_eWma,
    BAVC_ItbEsType_eH263,
    BAVC_ItbEsType_eVC1SimpleMain,
    BAVC_ItbEsType_eMpeg4Part2,
    BAVC_ItbEsType_eMpeg1Video,
    BAVC_ItbEsType_eOTFVideo = 0x0f,
    BAVC_ItbEsType_eVp6Video,
    BAVC_ItbEsType_eMpegAudioLayer3,
    BAVC_ItbEsType_eMpegAudio2_5,
    BAVC_ItbEsType_eMlpAudio,
    BAVC_ItbEsType_DVD_Subpicture,
    BAVC_ItbEsType_DVD_HLI,
    BAVC_ItbEsType_eAvsVideo,
    BAVC_ItbEsType_eAvsAudio,
    BAVC_ItbEsType_eMpegAudioWithDescriptor,
    BAVC_ItbEsType_eDra,
    BAVC_ItbEsType_eAmr,
    BAVC_ItbEsType_eAudioDescriptor     /* Generic AD support for all audio codecs. Replaces BAVC_ItbEsType_eMpegAudioWithDescriptor */
}       
BAVC_ItbEsType;

/***************************************************************************
Summary:
Transport (XPT) stream IDs.

Description:
ID for the individual streams in the packet multiplex output from the
transport block. See BAVC_GetXptId() for more details.

See Also:
BAVC_GetXptId
****************************************************************************/
typedef int BAVC_XptId;

/***************************************************************************
Summary:
Map a XPT band to a A/V bus ID.

Description:
There is a bus between the data transport and the audio and video decoders.
This bus multiplexes data from all XPT bands into a single packet stream.
Associated with each packet is an ID. The audio and video decoders use this
ID field to differentiate the muxed streams. This function provides a mapping
between the XPT bands and these ID fields.

Returns:
Stream ID for the given XPT band.

See Also:
****************************************************************************/
BAVC_XptId BAVC_GetXptId(
    BAVC_XptOutputId outputId  /* [in] Transport (xpt) band to map. */
    );


/***************************************************************************
Summary:
Defines DRAM buffer requirements.

Description:
This structure may be used to convey DRAM buffer requirements between
software modules.

The Alignment member gives the required byte alignment, expressed as the
power of 2. For example, to require aligment on a 1kB boundary, Alignment
should be set to 10 (pow( 2, 10 ) = 1024).
****************************************************************************/
typedef struct BAVC_BufferConfig
{
    size_t Length;          /* Buffer length, expressed in bytes. */

    /* Buffer aligment, in bytes, expressed as a power of 2. For audio and video
    ** decoders, this value can be overriden by the BXPT rave portinginterface. 
    ** See BXPT_Rave_AllocContext() in bxpt_rave.h 
    */ 
    unsigned Alignment;     

    bool LittleEndian;      /* Buffer endianess. Little endian if true, Big endian if false. */
}
BAVC_BufferConfig;

/***************************************************************************
Summary:
Defines DRAM buffer requirements between the transport RAVE and the
audio/video decoders.

Description:
The
****************************************************************************/
typedef struct BAVC_CdbItbConfig
{
    BAVC_BufferConfig Cdb, Itb;
    bool UsePictureCounter;
}
BAVC_CdbItbConfig;

/***************************************************************************
Summary:
Define the video modes that have unique ITB/CDB size requirements.
Description:

Todo:  Move the actual definition of the strucutre here from the decoder
       private h file.  Need to resolve sharing of this structure
       between the code bases.
****************************************************************************/
#define BAVC_Vmode_Max (10)

/***************************************************************************
Summary:
Aggregates the definitions of DRAM buffer requirements between the transport
RAVE and the video decoder for each of the video modes.
audio/video decoders.

Description:
****************************************************************************/
typedef struct BAVC_ContextSizes
{
    uint32_t        a;
    BAVC_CdbItbConfig modes[BAVC_Vmode_Max];
    uint32_t        b;
} BAVC_ContextSizes;

/***************************************************************************
Summary: 
Values for various flags and muxing parameters in the playback descriptor.  
****************************************************************************/
typedef struct BAVC_TsMux_DescConfig
{
    unsigned uiNextPacketPacingTimestamp;
    unsigned uiPacket2PacketTimestampDelta;
    bool bNextPacketPacingTimestampValid;
    bool bPacket2PacketTimestampDeltaValid;
    bool bRandomAccessIndication;
}
BAVC_TsMux_DescConfig;

/***************************************************************************
Summary: 
Address of STC increment registers used by the audio fw during non-realtime 
transcoding.  
***************************************************************************/
typedef struct BAVC_StcSoftIncRegisters
{
    /* See the RDB for the bitfield layouts */
    uint32_t StcIncLo;      /* BCHP_XPT_PCROFFSET_STC0_INC_LO */
    uint32_t StcIncHi;      /* BCHP_XPT_PCROFFSET_STC0_INC_HI */
    uint32_t IncTrigger;    /* BCHP_XPT_PCROFFSET_STC0_INC_TRIG */
}
BAVC_Xpt_StcSoftIncRegisters;
#endif
