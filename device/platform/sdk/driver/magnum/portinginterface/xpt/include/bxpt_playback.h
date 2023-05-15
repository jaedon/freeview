/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxpt_playback.h $
 * $brcm_Revision: Hydra_Software_Devel/13 $
 * $brcm_Date: 10/1/12 11:19a $
 *
 * Porting interface code for the PVR record and playback sections of the
 * data transport core. 
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xpt/include/bxpt_playback.h $
 * 
 * Hydra_Software_Devel/13   10/1/12 11:19a gmullen
 * SW7445-17: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7445-17/1   10/1/12 10:00a gmullen
 * SW7445-17: Created core28nm file tree
 * 
 * Hydra_Software_Devel/12   8/8/12 9:06a gmullen
 * SW7425-3701: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7425-3701/1   8/7/12 5:26p gmullen
 * SW7425-3701: Added BXPT_Playback__IsAvailable(). Fail if either PB 13
 * or 14 is requested on 7425 and 7346
 * 
 * Hydra_Software_Devel/11   8/2/12 4:31p gmullen
 * SW7425-3617: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7425-3617/1   8/1/12 4:04p gmullen
 * SW7425-3617: PI will access device RAM through cached addresses
 * 
 * Hydra_Software_Devel/10   6/18/12 10:53a gmullen
 * SW7425-2914: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7425-2914/1   6/11/12 10:59a gmullen
 * SW7425-2914: Moved and updated files to new home
 * 
 * Hydra_Software_Devel/9   5/3/12 10:10a ahulse
 * SW7425-1886: merge to mainline
 * 
 * Hydra_Software_Devel/SW7425-1886_2/1   5/1/12 4:58p ahulse
 * SW7425-1886: refactor to enable cc error with new xpt
 * 
 * Hydra_Software_Devel/8   6/3/11 4:34p gmullen
 * SW7425-653: Merged changes to Hydra branch
 * 
 * Hydra_Software_Devel/SW7425-653/1   6/2/11 10:35a gmullen
 * SW7425-653: Added non-realtime transcoding support
 * 
 * Hydra_Software_Devel/7   5/23/11 1:26p gmullen
 * SW7231-170: Removed incorrect sync mode define
 * 
 * Hydra_Software_Devel/6   5/16/11 4:58p gmullen
 * SW7408-284: Added support for jitter adjust in PCR hw to PI
 * 
 * Hydra_Software_Devel/5   3/7/11 3:09p gmullen
 * SW7425-153: Merged to mainline
 * 
 * Hydra_Software_Devel/SW7425-153/1   3/7/11 3:07p gmullen
 * SW7425-153: Fixed blockout calc and disabled usage of spare bandwidth
 * 
 * Hydra_Software_Devel/4   12/16/10 3:06p gmullen
 * SW7425-15: Added PES-based pacing
 * 
 * Hydra_Software_Devel/3   12/1/10 8:01a gmullen
 * SW7425-15: Updated header and stub calls for TS mux
 * 
 * Hydra_Software_Devel/2   11/30/10 6:09p gmullen
 * SW7425-15: Added TS muxing support
 * 
 * Hydra_Software_Devel/1   10/25/10 2:09p gmullen
 * SW7425-15: Moved srcs to base2 folder
 * 
 * Hydra_Software_Devel/1   10/8/10 2:57p gmullen
 * SW7425-15: Added header files
 * 
 * Hydra_Software_Devel/38   4/30/10 5:33p gmullen
 * SW7630-81: Added OutOfSync interrupt enum
 * 
 * Hydra_Software_Devel/37   12/16/09 3:26p gmullen
 * SW7325-665: Added support for 32-bit timestamp mode
 * 
 * Hydra_Software_Devel/36   7/31/09 2:44p gmullen
 * PR54331: Added 35130 to XPT support.
 * 
 * Hydra_Software_Devel/35   7/14/09 10:22a piyushg
 * PR56771: Add XPT PI code for 7342.
 * 
 * Hydra_Software_Devel/34   6/2/09 10:29a gmullen
 * PR51821: Merged workaround to mainline.
 * 
 * Hydra_Software_Devel/33   5/28/09 2:11p gmullen
 * PR55549: Added support for PCR-based pacing.
 * 
 * Hydra_Software_Devel/32   4/16/09 3:39p gmullen
 * PR54222: Merged 7002-specific changes to mainline. Created symlinks for
 * 7002 files back to 7400.
 * 
 * Hydra_Software_Devel/31   4/7/09 5:25p piyushg
 * PR52986: Add support for 7635
 * Added directory element "7635".
 * 
 * Hydra_Software_Devel/30   2/18/09 10:49a piyushg
 * PR52189: Added hooks for Get/Set PACING_OFFSET_ADJ_DIS bit.
 * 
 * Hydra_Software_Devel/29   1/27/09 1:10p gmullen
 * PR51625: Added 7336 support
 * 
 * Hydra_Software_Devel/28   12/9/08 4:42p gmullen
 * PR47755: Fixed compilation error in packetizer code.
 * 
 * Hydra_Software_Devel/27   11/12/08 3:10p gmullen
 * PR48835: BXPT_Playback_SetChannelPacketSettings now takes a struct.
 * 
 * Hydra_Software_Devel/26   10/10/08 6:18p gmullen
 * PR47232: Updated comments for UsePcrTimeBase and WhichPcrToUse struct
 * members.
 * 
 * Hydra_Software_Devel/25   10/9/08 11:35a gmullen
 * PR47745: Added BXPT_Playback_GetPacketizerDefaults().
 * 
 * Hydra_Software_Devel/24   9/17/08 5:02p gmullen
 * PR47065: Added PCR-based pacing support,
 * 
 * Hydra_Software_Devel/23   7/16/08 3:44p gmullen
 * PR37867: Merged playback mux code to Hydra_Software_Devel
 * 
 * Hydra_Software_Devel/22   4/10/08 3:36p gmullen
 * PR38954: Updated number of playbacks, added HDDVI support to PCR API,
 * and added packetizer support.
 * 
 * Hydra_Software_Devel/21   12/4/07 6:23p mward
 * PR37908: Use 7401 DVD LPCM support for 7118.
 * 
 * Hydra_Software_Devel/SanJose_CDI_Devel/2   12/17/07 6:41p shuang
 * PR37867:Merge Jethead patch in order to support DirecTV AM21 project
 * which ATSC data will input through USB interface.
 * Merge Magnum Phase 7.0.
 * 
 * Hydra_Software_Devel/20   11/30/07 11:49a gmullen
 * PR37062: Changed LPCM support to DVD Mode.
 * 
 * Hydra_Software_Devel/19   11/28/07 9:02a gmullen
 * PR37062: Added support for LPCM on the 7401.
 * 
 * Hydra_Software_Devel/18   11/8/07 10:11a gmullen
 * PR36800: Added defines to fix enum mispellings.
 * 
 * Hydra_Software_Devel/17   9/7/07 6:53p katrep
 * PR27642: Use the new stream filtering using spid for 7405. utilize the
 * new hw features.
 * 
 * Hydra_Software_Devel/16   8/31/07 3:50p gmullen
 * PR34504: Added substream filtering.
 * 
 * Hydra_Software_Devel/15   8/21/07 5:21p gmullen
 * PR34222: Added DisableTimestampParityCheck bool to
 * BXPT_Playback_ChannelSettings struct.
 * 
 * Hydra_Software_Devel/14   8/1/07 3:38p gmullen
 * PR33710: pepSpecial arbiter settings for 7440. Also support playback
 * data direct to RAVE.
 * 
 * Hydra_Software_Devel/13   7/13/07 4:09p piyushg
 * PR32218: PACING_START bit is reset whenever
 * TS_RANGE_ERROR interrupt occurs.
 * 
 * Hydra_Software_Devel/12   4/25/07 4:22p gmullen
 * PR29688: Added AcceptNulls and AcceptAdapt00 support to parser config
 * 
 * Hydra_Software_Devel/11   3/15/07 7:23p katrep
 * PR28320: Fixed the previous version.
 *
 * Hydra_Software_Devel/10   3/15/07 5:12p katrep
 * PR28320: Expose PsMode and PackHdrConfig for all the chips which
 * support it.
 * 
 * Hydra_Software_Devel/9   11/22/06 4:58p gmullen
 * PR26109: Updated for 7403.
 * 
 * Hydra_Software_Devel/8   10/2/06 11:07a gmullen
 * PR24504: Added BXPT_Playback_GetIntId().
 * 
 * Hydra_Software_Devel/7   9/18/06 3:01p gmullen
 * PR15309: Fixed compiler warnings in BXPT_Playback_PackHdr_Config enum
 * 
 * Hydra_Software_Devel/6   9/18/06 12:12p katrep
 * PR23114: Disable context enable for PES as context pt is used.
 * 
 * Hydra_Software_Devel/5   9/15/06 4:28p katrep
 * PR23114: Added PsMode to Channel Settings ,updated SyncMode comments.
 * 
 * Hydra_Software_Devel/4   8/17/06 6:01p katrep
 * PR23114: Added Support for 7440 chip
 * 
 * Hydra_Software_Devel/3   7/20/06 5:30p katrep
 * PR22365: Added better mangement of XC buffer client BW for playback.XC
 * buf for PB is given max BW & BW throtle is done at PB engine.Spare BW
 * enabllag is used to provide more BW for PB if available.
 * 
 * Hydra_Software_Devel/2   4/19/06 5:37p gmullen
 * PR21119: Added BXPT_Playback_DisablePacketizers()
 * 
 * Hydra_Software_Devel/4   10/20/05 11:14a gmullen
 * PR15309: Added support for PES and ES playback.
 * 
 * Hydra_Software_Devel/3   10/17/05 10:37a gmullen
 * PR15309: Added ES support and AllPass mode for PB parsers.
 * 
 * Hydra_Software_Devel/2   9/21/05 2:17p gmullen
 * PR15309: Added support for AAC HE and AC3+ audio, fixed bug in playback
 * PI, modified RAVE PI to support channel change and reset.
 * 
 * Hydra_Software_Devel/1   5/13/05 1:27p gmullen
 * PR15309: First draft of XPT PI.
 * 
 ***************************************************************************/

/*= Module Overview *********************************************************
Overview

***************************************************************************/

#ifndef BXPT_PLAYBACK_H__
#define BXPT_PLAYBACK_H__

#include "bxpt.h"

#if BXPT_HAS_MULTICHANNEL_PLAYBACK
#else
    #include "bchp_xpt_pb0.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
Summary:
Handle for accessing the playback API via a channel. Users should not directly
access the contents of the structure.
****************************************************************************/
typedef struct BXPT_P_PbHandle *BXPT_Playback_Handle;

/***************************************************************************
Summary:
This defines the maximum rate the playback channels can run at, in bits 
per second.
****************************************************************************/
#define BXPT_MAX_PLAYBACK_RATE  ( 108000000 )

/***************************************************************************
Summary:
This defines the minimum bit rate  bandwidth alloacted to the  playback
channels.Playback channels can run at, at bit rate  greater than this 
based on available bandwidth.
****************************************************************************/
#define BXPT_MIN_PLAYBACK_RATE   ( 10000 ) 

/***************************************************************************
Summary:
This defines the different packet synchronizations that are supported. These
are used as values for BXPT_Playback_ChannelSettings.SyncMode ( used when the
channel is opened or BXPT_Playback_SetChannelSettings() is called ). 

If transport packets are to be played back, the caller should also set   
BXPT_Playback_ChannelSettings.PacketLength appropriately.  
****************************************************************************/
#define BXPT_PB_SYNC_MPEG           ( 0x0 )

#define BXPT_PB_SYNC_PES            ( 0x3 )
#define BXPT_PB_SYNC_BYPASS         ( 0x4 ) 
#define BXPT_PB_SYNC_MPEG_BLIND     ( 0x5 )

#if BXPT_HAS_FULL_PID_PARSER
#define BXPT_INT_ID_PBP_CONTINUITY_ERROR(X)  BCHP_INT_ID_CREATE(BCHP_XPT_FULL_PID_PARSER_PBP_PCC_INTR_STATUS_REG, (X))
#endif

/***************************************************************************
Summary:
Enumeration of different timestamp types a channel will support. 
 
Discussion: 
There are three formats for supported for pacing timestamps. The full timestamp 
is 32 bits. For each of these 3 formats, the timestamp mode can be either 
Mod-300 or binary. The parity and user bits can generated by the RAVE during 
recording. The timeetamp formats are:

1) 32-bit timestamp 
2) 4-bit Parity + 28-bit timestamp (lower 28 bits of the 32 bit timestamps are 
    used and upper 4 bits are replaced with 4-bit parity)
3) 2-bit User field + 30-bit timestamp (lower 30 bits of the 32 bit timestamps 
    are used and upper 2 bits are replaced with programmable User field)

To configure playback, in BXPT_Playback_ChannelSettings the TimestampMode 
(XPT_PBn_CTRL4.TIMESTAMP_MODE) and Use32BitTimestamps (XPT_PBn_CTRL2.TIMESTAMP_MODE_32BIT) 
fields are supposed to be programmed for the timestamp format of the stamps used 
in the stream. 

It is possible to re-stamp the timestamps in the playback parser, changing 
their format, or between Mod-300 and binary. To enable restamping, 
in BXPT_Playback_ParserConfig set the ForceRestamping field to true and 
TsMode (XPT_PBn_PARSER_CTRL1.PARSER_TIMESTAMP_MODE) as appropriate.
****************************************************************************/
typedef enum BXPT_TimestampMode
{
    BXPT_TimestampMode_e28_4P_Mod300 = 0,   /* 28 bit timestamp, 4 bit parity, modulo 300 */
    BXPT_TimestampMode_e30_2U_Mod300 = 2,   /* 30 bit timestamp, 2 bit user field, modulo 300 */
    BXPT_TimestampMode_e30_2U_Binary = 3    /* 30 bit timestamp, 2 bit user field, binary. */           
}
BXPT_TimestampMode;

/***************************************************************************
Summary:
Supports starting and stopping of playback pacing logic. 
****************************************************************************/
typedef enum BXPT_PacingControl
{
    BXPT_PacingControl_eStart,      /* Enable and arm pacing. */
    BXPT_PacingControl_eStop        /* Disable pacing. */
}
BXPT_PacingControl;

/***************************************************************************
Summary:
Enums for playback channel interrupts. Useful for places where the RDB 
macros are too cumbersome. 
****************************************************************************/
typedef enum BXPT_Playback_Int
{
#if BXPT_HAS_MULTICHANNEL_PLAYBACK
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
    BXPT_PbInt_eDone,           
    BXPT_PbInt_eTsRangeErr,
    BXPT_PbInt_eOutOfSync,
    BXPT_PbInt_eTeiError
#else
    BXPT_PbInt_eDone
#endif
#else
    BXPT_PbInt_eDone = BCHP_XPT_PB0_INTR_DONE_INT_SHIFT,
    BXPT_PbInt_eTsRangeErr = BCHP_XPT_PB0_INTR_TS_RANGE_ERROR_SHIFT,
    BXPT_PbInt_eOutOfSync = BCHP_XPT_PB0_INTR_SE_OUT_OF_SYNC_INT_SHIFT,
    BXPT_PbInt_eTeiError = BCHP_XPT_PB0_INTR_PARSER_TRANSPORT_ERROR_SHIFT,
    BXPT_PbInt_eLengthError = BCHP_XPT_PB0_INTR_PARSER_LENGTH_ERROR_SHIFT
#endif 
}
BXPT_Playback_Int;


/***************************************************************************
Summary:
Configuration for the PES stream packetizer inside the playback channel.
****************************************************************************/
#if BXPT_HAS_PID_CHANNEL_PES_FILTERING
	
/* PR 36800: The PacetizerMode enums below are mispelt. */
#define BXPT_Playback_PacketizerMode   						  BXPT_Playback_PacetizerMode
#define BXPT_Playback_PacketizerMode_Es               		  BXPT_Playback_PacetizerMode_Es               
#define BXPT_Playback_PacketizerMode_Pes_MapAll       		  BXPT_Playback_PacetizerMode_Pes_MapAll       
#define BXPT_Playback_PacketizerMode_Pes_Sid          		  BXPT_Playback_PacetizerMode_Pes_Sid          
#define BXPT_Playback_PacketizerMode_Pes_SidRange     		  BXPT_Playback_PacetizerMode_Pes_SidRange     
#define BXPT_Playback_PacketizerMode_Pes_SidExtension 		  BXPT_Playback_PacetizerMode_Pes_SidExtension 
#define BXPT_Playback_PacketizerMode_Pes_SidSubSid     		  BXPT_Playback_PacetizerMode_Pes_SidSubSid    

typedef enum
{
    BXPT_Playback_PacetizerMode_Es,               /* ES, playback data */
    BXPT_Playback_PacetizerMode_Pes_MapAll,       /* PES,map all the stream IDs to this context */
    BXPT_Playback_PacetizerMode_Pes_Sid,          /* PES,map streams based on single stream ID */
    BXPT_Playback_PacetizerMode_Pes_SidRange,     /* PES,map streams based on stream ID range */ 
    BXPT_Playback_PacetizerMode_Pes_SidExtension, /* PES,map streams based on stream ID and stream ID extension */ 
    BXPT_Playback_PacetizerMode_Pes_SidSubSid     /* PES,maps streams based on stream ID and stream sun stream ID */

}BXPT_Playback_PacetizerMode;

typedef struct BXPT_Playback_PacketizeConfig
{
    unsigned Pid;           /* PID value inserted into MPED header during packetization. */
    unsigned ChannelNum;    /* PID channel number the packets will be mapped to. */

    BXPT_Playback_PacetizerMode PacketizerMode;
    union                            
    {
        unsigned char StreamId;
        struct
        {
            unsigned char Hi;
            unsigned char Lo;
        }StreamIdRange;

        struct
        {
            unsigned char Id;
            unsigned char Extension;
        }StreamIdAndExtension;

        struct
        {
            unsigned char Id;
            unsigned char SubStreamId;
        }StreamIdAndSubStreamId;
    }FilterConfig;                              /* program the filter values based on the PacketizerMode */

}
BXPT_Playback_PacketizeConfig;

#else

typedef struct BXPT_Playback_PacketizeConfig
{
    unsigned Pid;           /* PID value inserted into MPED header during packetization. */
    unsigned ChannelNum;    /* PID channel number the packets will be mapped to. */
    
    /* Set to true if the playback data is ES level. For PES level data, this should be false */
    bool IsEs;              
        
    /* 
    ** If true, map all PES stream IDs to this context. If false, map only the 
    ** streams matching PesStreamId below. 
    */
    bool MapAll;            
    unsigned PesStreamId;

#if ( BCHP_CHIP == 7401 ) || ( BCHP_CHIP == 7118 )
	/* Set to true if playing back DVD streams */
	bool IsDvd;
#endif
}
BXPT_Playback_PacketizeConfig;

#endif /* ( BCHP_CHIP == 7440 ) */

/***************************************************************************
Summary:
Settings for a playback parser band. The playback parsers have certain
functions that are not available to the input band parsers.
****************************************************************************/
typedef struct BXPT_Playback_ParserConfig
{
    bool ErrorInputIgnore;      /* PID parser ignores the error input signal and TEI bit */
#if BXPT_HAS_FULL_PID_PARSER
	/* The below params were moved to the full parser and mesg filters */
#else	
    bool ContCountIgnore;       /* PID parser does not check continuity of enabled PID channels */
    bool SaveShortPsiMsg;       /* Save short (total length less than 7 bytes) PSI messages */
    bool SaveLongPsiMsg;        /* Save long (total length of 4097 or 4098 bytes) PSI messages */
    bool PsfCrcDis;             /* Whether or not CRC checking is performed for short form private sections */
    BXPT_PsiMesgModModes PsiMod;    /* Type of mods to be done to PSI messages transfered into the DMA buffers. */
    BXPT_ParserTimestampMode TsMode; /* Controls the type of timestamp generated as packets pass through the parser band. */
#endif

    bool ForceRestamping;       /* Force new timestamps onto packets */
    
    /* 
    ** Map all PIDs in the playback stream to PID channel X, where X is
    ** 16 + the playback channel number. For example, if the parser for 
    ** playback 1 is put into all pass, the PIDs are mapped to channel 17 (16 + 1 ).
    */
    bool AllPass;               

    bool AcceptNulls;       /* NULL packets are not discarded if true */
    bool AcceptAdapt00;     /* Packets with an adaptation field of 00 are accepted if true */

    /* 
    ** true if timebase is locked to a DPCR module. false if the free running 
    ** 27 MHz clock is to be used. WhichPcrToUse specifies the DPCR module to
    ** use. Note that a DPCR module can be driven by sources other than PCRs in the
    ** stream: it can lock to 656 input, I2S, or even be controlled directly by the
    ** host MIPS writing to the certain registers. To other modules in the chip, the
    ** output of the DPCR is just a timebase, a 27 MHz clock. Better names should have
    ** been chosen for these two members. 
    */
    bool UsePcrTimeBase;    
    unsigned int WhichPcrToUse; /* Which DPCR module, if UsePcrTimeBase == true */
}
BXPT_Playback_ParserConfig;

typedef enum BXPT_Playback_PS_Mode
{
    BXPT_Playback_PS_Mode_MPEG2 = 0,
    BXPT_Playback_PS_Mode_MPEG1 = 1
}
BXPT_Playback_PS_Mode;

typedef enum BXPT_Playback_PackHdr_Config 
{
    BXPT_Playback_PackHdr_Payload_Insert = 0,  /* pack header is inserted in separate 
                                                  packets payload field and packets is mapped 
                                                  to the pid */
    BXPT_Playback_PackHdr_Adaptation_Insert,   /* not supported on all the chips */
    BXPT_Playback_PackHdr_Drop
}
BXPT_Playback_PackHdr_Config;

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
#if BXPT_HAS_MULTICHANNEL_PLAYBACK
typedef struct BXPT_P_GpcInfo *BXPT_Playback_PacingCounter;
#endif
#endif

/***************************************************************************
Summary:
Settings for the Playback channels. These values can be passed in when the
channel is opened, and will be used as the new default settings.
****************************************************************************/
typedef struct BXPT_Playback_ChannelSettings
{
    /* 
    ** Length of the transport packet. NOTE: When doing an ES or PES playback
    ** the PacketLength must be set to 0xB8. 
    */
    unsigned int PacketLength;  

    /* 
    ** Packet sync mode to be used during playback. This value must be one
    ** of the BXPT_PB_SYNC_* defines (above). Behavior is undefined otherwise.
    ** NOTE: When doing an ES,MPEG TS playback,use BXPT_PB_SYNC_MPEG_BLIND,
      and for PES,PS playback SyncMode must be BXPT_PB_SYNC_PES
    */
    unsigned int SyncMode;

    /* 
    ** true if timebase is locked to a PCR module. false if the free running 
    ** 27 MHz clock is to be used. WhichPcrToUse specifies the PCR block to
    ** lock to.
    */
    bool UsePcrTimeBase;    
    unsigned int WhichPcrToUse; /* Which PCR module, if UsePcrTimeBase == true */

    bool TimestampEn;                   /* Prepend timestamps to each packet */
    BXPT_TimestampMode TimestampMode;   /* If enabled, this is the type of timestamps to prepend. */

    /* Disable parity checking for pacing timestamps. Normally set to false (ie. do parity checking) */
    bool DisableTimestampParityCheck;

#if BXPT_HAS_32BIT_PB_TIMESTAMPS
    /* 
    ** Enables the playback pacing and packetizer modules to use 32 bit timestamps instead of 28 bits. 
    ** The 4 bit parity checksum or 2 bit user field parsing gets disabled if this bit is set. 
    */
    bool Use32BitTimestamps;	
#endif

    /* 
    ** The playback channels read data in 4-byte chunks. Within that chunk,
    ** data can be arranged in two formats (demonstrated here using the 
    ** 4 byte transport header) :
    ** 1) Big endian - sync byte, upper PID byte, lower PID byte, CC/SC byte in that order
    ** 2) Little endian - CC/SC byte, lower PID byte, upper PID byte, sync byte in that order
    */
    bool SwapBytes;
    
    /* Select between MPEG1/MPEG2 Program stream modes */
    BXPT_Playback_PS_Mode PsMode;

    /* For Program Streams,the 14 byte pack header can be placed in TS or dropped all together.
       The adaptation field,can be optionally placed in the adaptation field of the TS packet. */ 
    BXPT_Playback_PackHdr_Config PackHdrConfig; 
    
    /* Set this TRUE if you need to reset the PACING_START bit when TS_RANGE_ERROR interrupt ocures*/
    bool ResetPacing;

    /* 
    ** Route playback data directly to the RAVE, bypassing the XC buffer. Note that the data
    ** will go only to the RAVE; message buffer and remux capability will be lost. By default,
    ** this is false.
    */
    bool RaveOutputOnly;

#if BXPT_HAS_PCR_PACING
    /* 
    ** Select PCR-based pacing. PCRs in the stream will be used, rather than the 4-byte timestamps
    ** prepending during recording. Note that the 4-byte timestamps aren't needed for PCR-based
    ** pacing.  
    */
    bool PcrBasedPacing;		/* true if PCR-based pacing is requested, false to use legacy 4-byte timestamps */
    uint16_t PcrPacingPid;		/* which PID in the stream carries the PCRs */
#endif	
    bool PacingOffsetAdjustDisable; /* 0 = Normal: adjust the pacing timestamps with the reference offset
                                       1 = Do not adjust the pacing timestamps with the reference offset */

    /* 
    ** This code enforces the workaround for PR 51821. When using this workaround, applications must either 
    ** use bpvrlib_feed.c version 23 (or later), or change their descriptor management code to match. It is 
    ** NOT backward compatible.
    */

#if BXPT_HAS_TSMUX
	/* Support for transport stream muxing, needed for the transcoding feature in some chips. */		

	/*
	** Pacing is done using timestamps stored in the playback descriptor. Use the NextPacketPacingTimestamp
	** member of the BXPT_PvrDescriptor8 struct. Use8WordDesc (below) must be true to use this feature. 
	** 
	** NOTE: PesBasedPacing and PcrBasedPacing (above) are mutually exclusive. If both bools are true, 
	** PES pacing will be used. 
	*/
	bool PesBasedPacing;	
	
	/* If true, hw will expect all descriptors to use the 8-word format. Value is false by default */
	bool Use8WordDesc;		
#endif	

	bool AlwaysResumeFromLastDescriptor;

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
#if BXPT_HAS_MULTICHANNEL_PLAYBACK
    BXPT_Playback_PacingCounter PacingCounter;  /* Default value is NULL (no counter allocated). */
#endif
#endif
}
BXPT_Playback_ChannelSettings;

/***************************************************************************
Summary: 
Status bits for the playback channel. 
****************************************************************************/
typedef struct BXPT_Playback_ChannelStatus
{
    bool OutOfSync;     /* true if channel is out of sync */
    bool Finished;      /* true if channel has processed last descriptor in the chain. */
    bool Busy;          /* true if channel is active. */
    bool Run;           /* true if a playback session has been started. */
}
BXPT_Playback_ChannelStatus;

/***************************************************************************
Summary:
Layout of a PVR descriptor. The order of the members matches the order the
hardware will expect them in when the descriptor is read from memory. Thus,
the descriptor's contents can be accessed by creating a pointer to this type
of struct and setting that pointer to the address of the real descriptor
in memory (after being memory mapped, of course).
****************************************************************************/
typedef struct BXPT_PvrDescriptor
{
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
#if BXPT_HAS_MULTICHANNEL_PLAYBACK    
    uint32_t    BufferStartAddrHi;  /* upper 32-bits of (Pointer to) the buffer associated with this descriptor. */
    uint32_t    BufferStartAddr;    /* lower 32-bit of (Pointer to) the buffer associated with this descriptor. */
    uint32_t    NextDescAddr;       /* (Pointer to) the next descriptor in the chain. */
    uint32_t    BufferLength;       /* Length (in bytes) of the buffer. */
    uint32_t    Flags;              /* Channel-specific flags. */
    uint32_t    words5_11[7];
#else
    uint32_t    BufferStartAddr;    /* (Pointer to) the buffer associated with this descriptor. */
    uint32_t    BufferLength;       /* Length (in bytes) of the buffer. */
    uint32_t    Flags;              /* Channel-specific flags. */
    uint32_t    NextDescAddr;       /* (Pointer to) the next descriptor in the chain. */

    /* Support for TS muxing via playback hardware. Ignored by hardware on non-TS Mux platforms. */
    #if BXPT_HAS_TSMUX
        uint32_t    Reserved0;
        uint32_t    MuxingFlags;	         /* Random Access Indicator, Next Packet Pacing Timestamp Valid, and Pkt2Pkt Timestamp valid. */
        uint32_t    NextPacketPacingTimestamp;  
        uint32_t    Pkt2PktPacingTimestampDelta;
    #endif
#endif
#else
    uint32_t    BufferStartAddr;    /* (Pointer to) the buffer associated with this descriptor. */
    uint32_t    BufferLength;       /* Length (in bytes) of the buffer. */
    uint32_t    Flags;              /* Channel-specific flags. */
    uint32_t    NextDescAddr;       /* (Pointer to) the next descriptor in the chain. */
#endif
} 
BXPT_PvrDescriptor;

#if BXPT_HAS_TSMUX

/***************************************************************************
Summary:
Layout of a 8 word PVR descriptor. The first 4 words are identical to the 
legacy 4-word layout. The additional words are fields for TS mux 
generation.
****************************************************************************/
typedef struct BXPT_PvrDescriptor8
{
	BXPT_PvrDescriptor Desc;

    /* Support for TS muxing via playback hardware. Use BXPT_Tsmux_CreateDesc to set these extended fields.*/
    uint32_t    Reserved0;
    uint32_t    MuxingFlags;	         /* Random Access Indicator, Next Packet Pacing Timestamp Valid, and Pkt2Pkt Timestamp valid. */
    uint32_t    NextPacketPacingTimestamp;  
    uint32_t    Pkt2PktPacingTimestampDelta;
} 
BXPT_PvrDescriptor8;

#endif  /* BXPT_HAS_TSMUX */

#ifdef ENABLE_PLAYBACK_MUX
typedef struct BXPT_Playback_ChannelPacketSettings
{
    /* 
    ** Length of the transport packet. NOTE: When doing an ES or PES playback
    ** the PacketLength must be set to 0xB8. 
    */
    unsigned int PacketLength;  

    /* 
    ** Packet sync mode to be used during playback. This value must be one
    ** of the BXPT_PB_SYNC_* defines (above). Behavior is undefined otherwise.
    ** NOTE: When doing an ES,MPEG TS playback,use BXPT_PB_SYNC_MPEG_BLIND,
      and for PES,PS playback SyncMode must be BXPT_PB_SYNC_PES
    */
    unsigned int SyncMode;

	/* Prepend timestamps to each packet */
    bool TimestampEn;

    /* Disable parity checking for pacing timestamps. Normally set to false (ie. do parity checking) */
    bool DisableTimestampParityCheck;
}
BXPT_Playback_ChannelPacketSettings;
#endif /* ENABLE_PLAYBACK_MUX */

/***************************************************************************
Summary:
Return the number of playback channels.

Description:
For the given transport core, return the number of playback channels that is
supported. 

Returns:
    BERR_SUCCESS                - Retrieved address from hardware.
    BERR_INVALID_PARAMETER      - Bad input parameter
****************************************************************************/
BERR_Code BXPT_Playback_GetTotalChannels(
    BXPT_Handle hXpt,           /* [in] Handle for this transport */
    unsigned int *TotalChannels     /* [out] The number of playback channels. */
    );  

/***************************************************************************
Summary:
Return the playback channel default settings.

Description:
Each playback channel has a set of default configuration values. This 
function retrieves these values and places them in a structure that can be
used to change the defaults when the playback channel is opened.

Returns:
    BERR_SUCCESS                - Retrieved playback defaults.
    BERR_INVALID_PARAMETER      - Bad input parameter
****************************************************************************/
BERR_Code BXPT_Playback_GetChannelDefaultSettings(
    BXPT_Handle hXpt,           /* [in] Handle for this transport */
    unsigned int ChannelNo,         /* [in] Which channel to get defaults from. */
    BXPT_Playback_ChannelSettings *ChannelSettings /* [out] The defaults */
    );  

/***************************************************************************
Summary:
Open a given playback channel.

Description:
Reset the given playback channel, configure it using the given defaults, then
return a handle to access that channel.

Returns:
    BERR_SUCCESS                - Playback channel opened.
    BERR_INVALID_PARAMETER      - Bad input parameter
****************************************************************************/
BERR_Code BXPT_Playback_OpenChannel(
    BXPT_Handle hXpt,                           /* [in] Handle for this transport */
    BXPT_Playback_Handle *PlaybackHandle,   /* [out] Handle for opened record channel */
    unsigned int ChannelNo,                         /* [in] Which channel to open. */
    BXPT_Playback_ChannelSettings *ChannelSettings /* [in] The defaults to use */
    );  

/***************************************************************************
Summary:
Close a given playback channel.

Description:
Shutdown a given playback channel, disable any interrupts from it. 

Returns:
    void
****************************************************************************/
void BXPT_Playback_CloseChannel(
    BXPT_Playback_Handle PlaybackHandle /* [in] Handle for the channel to close*/
    );  

/***************************************************************************
Summary:
Write out the given channel settings to the playback core.

Description:
The given channel settings are loaded into the playback core. If the 
'WhichPcrToUse' fields is out of range, the default 27MHz internal clock will
be used. 

Returns:
    BERR_SUCCESS                - New settings are now in effect.
    BERR_INVALID_PARAMETER      - Bad input parameter
****************************************************************************/
BERR_Code BXPT_Playback_SetChannelSettings(
    BXPT_Playback_Handle hPb,       /* [in] Handle for the playback channel. */
    const BXPT_Playback_ChannelSettings *ChannelSettings /* [in] New settings to use */
    );

/***************************************************************************
Summary:
Get the current channel settings.

Description:
The current channel settings are read from the playback core and returned in
the ChannelSettings structure.

Returns:
    BERR_SUCCESS                - Retrieved settings from hardware.
    BERR_INVALID_PARAMETER      - Bad input parameter
****************************************************************************/
BERR_Code BXPT_Playback_GetChannelSettings(
    BXPT_Playback_Handle hPb,       /* [in] Handle for the playback channel. */
    BXPT_Playback_ChannelSettings *ChannelSettings /* [out] The current settings  */
    );

#ifdef ENABLE_PLAYBACK_MUX
/***************************************************************************
Summary:
Set the current channel packet settings.

Description:
The specified playback's packet length and sync type are updated by the
provided values.

Returns:
    BERR_SUCCESS                - Retrieved settings from hardware.
    BERR_INVALID_PARAMETER      - Bad input parameter
****************************************************************************/
BERR_Code BXPT_Playback_SetChannelPacketSettings(
	BXPT_Playback_Handle hPb,								   /* [in] Handle for the playback channel. */
    const BXPT_Playback_ChannelPacketSettings *ChannelSettings /* [in] New settings to use */
	);
#endif /*ENABLE_PLAYBACK_MUX*/

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
#if FOO    
/***************************************************************************
Summary:
Set default values in a BXPT_PvrDescriptorAdv descriptor.
****************************************************************************/
void BXPT_Playback_PvrDescriptorAdvDefault(
    BXPT_PvrDescriptorAdv *desc
    );
#endif

#if BXPT_HAS_MULTICHANNEL_PLAYBACK
/***************************************************************************
Summary:
Return the address of the last last descriptor that has been completely 
consumed by hardware. If no descriptors have been consumed, NULL is returned.

Returns:
    BERR_SUCCESS                - Retrieved address from hardware.
    BERR_INVALID_PARAMETER      - Bad input parameter
****************************************************************************/
BERR_Code BXPT_Playback_GetLastCompletedDescriptorAddress(
    BXPT_Playback_Handle PlaybackHandle,        /* [in] Handle for the playback channel */
    BXPT_PvrDescriptor **LastCompletedDesc      /* [in] Address of the last completed descriptor. */
    );
#endif
#endif      
/***************************************************************************
Summary:
Get the current channel status.

Description:
Retrieve the current status bits for the given channel. Values are returned
through the structure pointer passed in.

Returns:
    BERR_SUCCESS                - Retrieved status bits from hardware.
    BERR_INVALID_PARAMETER      - Bad input parameter
****************************************************************************/
BERR_Code BXPT_Playback_GetChannelStatus(
    BXPT_Playback_Handle hPb,           /* [in] Handle for the playback channel. */
    BXPT_Playback_ChannelStatus *Status /* [out] Channel status. */
    );
      
/***************************************************************************
Summary:
Return the address of the next byte to written in a Record descriptor buffer. 

Description:
For a given Playback channel, get the address of the next data access to the 
currently used buffer. The access is the next 32 word that will be read. 

Returns:
    BERR_SUCCESS                - Retrieved address from hardware.
    BERR_INVALID_PARAMETER      - Bad input parameter
****************************************************************************/
BERR_Code BXPT_Playback_GetCurrentBufferAddress(
    BXPT_Playback_Handle PlaybackHandle,    /* [in] Handle for the playback channel */
    uint32_t *Address                       /* [out] The address read from hardware. */
    );  

/***************************************************************************
Summary:
Return the address of the current playback descriptor.

Description:
For a given Playback channel, get the address of the descriptor that is 
currently being used. 

Returns:
    BERR_SUCCESS                - Retrieved address from hardware.
    BERR_INVALID_PARAMETER      - Bad input parameter
****************************************************************************/
BERR_Code BXPT_Playback_GetCurrentDescriptorAddress(
    BXPT_Playback_Handle PlaybackHandle,    /* [in] Handle for the playback channel */
    BXPT_PvrDescriptor **LastDesc       /* [in] Address of the current descriptor. */
    );

/***************************************************************************
Summary:
Create a playback linked-list descriptor. 

Description:
Initialize the contents of a playback descriptor. The caller passes in the
starting address of the buffer that the descriptor will point to, along with
the length of that buffer.  

The caller specifies if an interrupt should be generated when the
hardware has finished processing the contents of the descriptor's
buffer. The caller also specifies if the sync extraction engine should
enter the re-sync state before processing the contents of the buffer. 

The memory for the descriptor must be allocated by the caller, and must 
start on a 16-byte boundary. 

Since descriptors are usually used in chains, the function allows the 
descriptor's NextDescAddr to be initialized along with the rest of the 
fields. This next descriptor does not have to be initialized itself. The
address must meet the 16-byte boundary requirement.

Returns:
    BERR_SUCCESS                - Descriptor initialized successfully.
    BERR_INVALID_PARAMETER      - Bad input parameter
****************************************************************************/
BERR_Code BXPT_Playback_CreateDesc( 
    BXPT_Handle hXpt,                       /* [in] Handle for this transport */
    BXPT_PvrDescriptor * const Desc,        /* [in] Descriptor to initialize */  
    uint8_t *Buffer,                        /* [in] Data buffer. */
    uint32_t BufferLength,                  /* [in] Size of buffer (in bytes). */
    bool IntEnable,                         /* [in] Interrupt when done? */
    bool ReSync,                            /* [in] Re-sync extractor engine? */
    BXPT_PvrDescriptor * const NextDesc     /* [in] Next descriptor, or NULL */
    );

#ifdef ENABLE_PLAYBACK_MUX
/***************************************************************************
Summary:
Update a playback linked-list descriptor's buffer information. 

Description:
Updates the buffer information of a playback descriptor. The caller passes in the
starting address of the buffer that the descriptor will point to, along with
the length of that buffer.  

Returns:
    Nothing.
****************************************************************************/
void BXPT_Playback_SetDescBuf( 
	BXPT_Handle hXpt, 	   					/* [in] Handle for this transport */
	BXPT_PvrDescriptor * const Desc,		/* [in] Descriptor to initialize */  
	uint8_t *Buffer, 						/* [in] Data buffer. */
	uint32_t BufferLength 					/* [in] Size of buffer (in bytes). */
	);
#endif /*ENABLE_PLAYBACK_MUX*/

/***************************************************************************
Summary:
Mark the descriptor as the last one on a linked list.

Description:
For the given descriptor, set the flag to indicate that this is the last
descriptor on a linked-list chain. This function can be used with 4 supported
types of descriptors: playback, record, startcode detect, and packet sub. 

Returns:
    void 
****************************************************************************/
void BXPT_SetLastDescriptorFlag(
    BXPT_Handle hXpt,                       /* [in] Handle for this transport */
    BXPT_PvrDescriptor * const Desc     /* [in] Descriptor to initialize */  
    );

/***************************************************************************
Summary:
Add a descriptor to a playback linked-list. 

Description:
Add a descriptor chain to an existing (but possibly empty) chain used by a
playback channel. If the existing chain is empty, the chain being added simply
becomes the current chain. 

The FirstDesc argument is a pointer to the descriptor at the start of the 
chain being added. The LastDesc argument passed in is a pointer to the end 
of that chain (or NULL if this is the only descriptor being added).

Returns:
    BERR_SUCCESS                - Descriptor added successfully.
    BERR_INVALID_PARAMETER      - Bad input parameter
****************************************************************************/
BERR_Code BXPT_Playback_AddDescriptors( 
    BXPT_Playback_Handle PlaybackHandle,    /* [in] Handle for the playback channel */
    BXPT_PvrDescriptor *LastDesc,   /* [in] Last descriptor in new chain */
    BXPT_PvrDescriptor *FirstDesc   /* [in] First descriptor in new chain */
    );

/***************************************************************************
Summary:
Start a playback channel. 

Description:
Enable playback channel processing. If no buffers have been added to the 
playback channel, the hardware is not actually started until the first buffer 
is added. 

The PID parser associated with this playback channel will be enabled 
automatically by this call.

Note: the caller should install an interrupt handler prior to calling this
function. That handler will be called when a descriptor's data has been 
processed if the IntEnable bit in the descriptor was set.

Returns:
    BERR_SUCCESS                        - Playback channel started successfully.
    BERR_INVALID_PARAMETER              - Bad input parameter
    BXPT_ERR_CHANNEL_ALREADY_RUNNING    - Channel already running.
****************************************************************************/
BERR_Code BXPT_Playback_StartChannel( 
    BXPT_Playback_Handle PlaybackHandle /* [in] Handle for the playback channel */
    );

/***************************************************************************
Summary:
Stop a playback channel. 

Description:
Stops a playback channel. The interrupts for the given channel are disabled. 

Returns:
    BERR_SUCCESS                        - Playback channel stopped successfully.
    BERR_INVALID_PARAMETER              - Bad input parameter
    BXPT_ERR_CHANNEL_ALREADY_STOPPED    - Channel already stopped.
****************************************************************************/
BERR_Code BXPT_Playback_StopChannel( 
    BXPT_Playback_Handle PlaybackHandle     /* [in] Handle for the playback channel */
    );

/***************************************************************************
Summary:
Pause a playback channel. 

Description:                                                         
Assert the software (or CPU) pauses to a playback channel. The playback 
engine will stop when this function is called. Playback can be resumed by 
calling BXPT_Playback_Resume().

Returns:
    BERR_SUCCESS                - Playback channel paused successfully.
    BERR_INVALID_PARAMETER      - Bad input parameter

See Also:
BXPT_Playback_Resume
****************************************************************************/
BERR_Code BXPT_Playback_Pause( 
    BXPT_Playback_Handle PlaybackHandle     /* [in] Handle for the playback channel */
    );

/***************************************************************************
Summary:
Resume a playback session. 

Description:                                                         
Resume, or un-pause, a playback channel. If the channel was not paused by
a call to BXPT_Playback_Pause(), this function will have no effect.

Returns:
    BERR_SUCCESS                - Playback channel resumed successfully.
    BERR_INVALID_PARAMETER      - Bad input parameter

See Also:
BXPT_Playback_Pause
****************************************************************************/
BERR_Code BXPT_Playback_Resume( 
    BXPT_Playback_Handle PlaybackHandle     /* [in] Handle for the playback channel */
    );

/***************************************************************************
Summary:
Set the bitrate for a playback channel.

Description:
Set the playback rate for a given channel. This is the maximum rate the data
will be played back at. The actual rate may be lower if the channel is 
restrained by the pause signals or by the pacing logic. Any rate up to 81 
Mbps is supported.

The caller must specify the rate in bytes/sec. For example, to set a rate
of 27 Mbps, the BitRate argument must be 27000000 .

Returns:
    BERR_SUCCESS                - Playback bitrate set.
    BERR_INVALID_PARAMETER      - Bad input parameter
****************************************************************************/
BERR_Code BXPT_Playback_SetBitRate( 
    BXPT_Playback_Handle PlaybackHandle,    /* [in] Handle for the playback channel */
    uint32_t BitRate                        /* [in] Rate, in bits per second. */
    );

/***************************************************************************
Summary:
Determine if this descriptor is on the head of a playback linked list.

Description:
Determine if the current descriptor being processed by the record channel is 
the first on the channel's chain (which means this descriptor is still being 
used). If the playback channel is still 'busy', the size of the 
descriptor's buffer is also returned. 

Returns:
    BERR_SUCCESS                - Record channel flushed successfully.
    BERR_INVALID_PARAMETER      - Bad input parameter
****************************************************************************/
BERR_Code BXPT_Playback_CheckHeadDescriptor( 
    BXPT_Playback_Handle PlaybackHandle,    /* [in] Handle for the playback channel */
    BXPT_PvrDescriptor *Desc,       /* [in] Descriptor to check. */
    bool *InUse,                    /* [out] Is descriptor in use? */
    uint32_t *BufferSize            /* [out] Size of the buffer (in bytes). */
    );

/***************************************************************************
Summary:
Return the user bits of the last timestamp seen by the playback engine. 

Description:
In some configurations, the upper 2 bits of the packet's recorded timestamp
may be user-programmable. This function will return the upper 2 bits of the
last timestamp seen by the playback engine, shifted down to align with bit 
0.  

Returns:
    BERR_SUCCESS                - Retrieved address from hardware.
    BERR_INVALID_PARAMETER      - Bad input parameter
****************************************************************************/
BERR_Code BXPT_Playback_GetTimestampUserBits(
    BXPT_Playback_Handle PlaybackHandle,    /* [in] Handle for the playback channel */
    unsigned int *Bits                          /* [out] The user bits read from hardware. */
    );  

/***************************************************************************
Summary:
Controls pacing in the playback channel. 

Description:
Start, or stop, pacing in the given playback channel. When started, the pacing
logic will capture the next playback timestamp and use it to control the 
playback rate. When stopped, pacing will no longer regulate the playback rate.

Returns:
    BERR_SUCCESS                - New pacing mode has been set.
    BERR_INVALID_PARAMETER      - Bad input parameter
****************************************************************************/
BERR_Code BXPT_Playback_ConfigPacing(
    BXPT_Playback_Handle PlaybackHandle,    /* [in] Handle for the playback channel */
    BXPT_PacingControl Mode                 /* [in] New mode for pacing. */
    );

/***************************************************************************
Summary:
Set the maximum allowable timestamp error used during playback pacing.
    
Description:
The pacing logic checks the delta between successive packets, and will 
immediately release a packet that is either too early or too late. This
functions sets the maximum delta used in the comparison.

When Mod-300 timestamps are used, MaxTsError is expressed in 90kHz ticks.
For binary timestamps, MaxTsError is 52734 Hz ticks (or 27MHz/512). 

Note that the value set here will be used only by the playback channel
associated with the handle. This is different behavior from previous 
chips, where the same error bound was used by all channels. 

Returns:
    BERR_SUCCESS                - New error bound is set.
    BERR_INVALID_PARAMETER      - Bad input parameter
****************************************************************************/

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
#define BXPT_MAX_TS_ERROR   ((1 << 23) - 1)
#endif

BERR_Code BXPT_Playback_SetPacingErrorBound(
    BXPT_Playback_Handle hPb,       /* [in] Handle for the playback channel */
    unsigned long MaxTsError        /* [in] Maximum timestamp error. */
    );

/***************************************************************************
Summary:
Get default settings for the packetizer struct. 
    
Description:
Get default settings for the packetizer struct. 

Returns:
	void
****************************************************************************/
void BXPT_Playback_GetPacketizerDefaults(
    const BXPT_Playback_PacketizeConfig *Cfg   /* [in] Config to initialize */
    );

/***************************************************************************
Summary:
Configure PES to MPEG packetization.
    
Description:
This function allows the playback PES data to be packetized and mapped to
one of several contexts, for use by the RAVE engine. The data can be packetized 
into MPEG packets. The PID, channel number, and stream ID that is packetized 
are all programmable through this call. 
 
If this function is not called, or is called with Enable == false, packetizing
will not be done. PES data will be mapped as-is to context 0. 

Returns:
    BERR_SUCCESS                - New error bound is set.
    BERR_INVALID_PARAMETER      - Bad input parameter
****************************************************************************/
BERR_Code BXPT_Playback_PacketizeStream(
    BXPT_Playback_Handle hPb,                   /* [in] Handle for the playback channel */
    unsigned Context,                           /* [in] Which context to map the packets to. */
    const BXPT_Playback_PacketizeConfig *Cfg,   /* [in] Configuration for this context */
    bool Enable                                 /* [in] Start or stop packetization. */
    );

/***************************************************************************
Summary:
Retrieves the current playback parser configuration.

Description:
Read the current settings for the given playback parser band from the chip. 
Settings are returned in the structure that is passed in. See the structure 
for the complete list of supported settings.

Returns:
    BERR_SUCCESS                - Copied the parser config.
    BERR_INVALID_PARAMETER      - Bad input parameter
 
See Also:
BXPT_Playback_SetParserConfig
****************************************************************************/
BERR_Code BXPT_Playback_GetParserConfig(
    BXPT_Playback_Handle hPb,                   /* [in] Handle for the playback channel */
    BXPT_Playback_ParserConfig *ParserConfig    /* [out] The current settings */
    );

/***************************************************************************
Summary:
Sets the current parser band configuration.

Description:
Writes the given parser band configuration to the device. The caller supplies
a structure containing the new parser band settings. The new settings take
effect as soon as the function returns.

Returns:
    BERR_SUCCESS                - Transfered the parser config to the chip.
    BERR_INVALID_PARAMETER      - Bad input parameter
 
See Also:
BXPT_Playback_GetParserConfig
****************************************************************************/
BERR_Code BXPT_Playback_SetParserConfig(
    BXPT_Playback_Handle hPb,                   /* [in] Handle for the playback channel */
    const BXPT_Playback_ParserConfig *ParserConfig  /* [in] The new settings */
    );

/***************************************************************************
Summary:
Disables all packetizers.

Description:
Stops all stream packetizers associated with the given playback channel. 
This is the equivilent of calling BXPT_Playback_PacketizeStream() with 
the Enable argument equal to 'false' for each packetizer.

Returns:
    BERR_SUCCESS                - Packetizers disabled.
    BERR_INVALID_PARAMETER      - Bad input parameter
 
See Also:
BXPT_Playback_PacketizeStream
****************************************************************************/
BERR_Code BXPT_Playback_DisablePacketizers(
    BXPT_Playback_Handle hPb                    /* [in] Handle for the playback channel */
    );

/***************************************************************************
Summary:
Return the interrupt ID for a given playback interrupt. 

Description:
In some cases, the RDB macros used to define a playback interrupt are cumbersome
to use. This API allows a more general abstraction: given a playback channel
handle and an enum for a generic PB interrupt, the channel-specific BINT_Id 
is returned.

Returns:
    BINT_Id - Interrupt ID for the given playback channel.
****************************************************************************/
BINT_Id BXPT_Playback_GetIntId(
    BXPT_Playback_Handle hPb,                   /* [in] Handle for the playback channel */
    BXPT_Playback_Int PbInt
    );      

/***************************************************************************
Summary:
Indicate if a given playback channel is available. 

Description:
Returns true if the playback channel is not open at the time, false otherwise.

Returns:
    bool - true if available, false if not.
****************************************************************************/
bool BXPT_Playback_IsAvailable(
    BXPT_Handle hXpt,                           /* [in] Handle for this transport */
    unsigned int ChannelNo                      /* [in] Which channel to check. */
    );      

#if BXPT_HAS_TSMUX

/***************************************************************************
Summary:
Settings for a playback parser band. The playback parsers have certain
functions that are not available to the input band parsers.
****************************************************************************/
typedef struct BXPT_Playback_MuxingInfo
{
	unsigned uiPacingCounter;    
}
BXPT_Playback_MuxingInfo;

/***************************************************************************
Summary:
Return transport muxing status. 

Description:
Some additional status fields are available on chips that support the TS 
muxing function.

Returns:
	void
****************************************************************************/
void BXPT_Playback_GetMuxingInfo(
    BXPT_Playback_Handle hPb,                   /* [in] Handle for the playback channel */
    BXPT_Playback_MuxingInfo *Info
    );      

/***************************************************************************
Summary:
Set the pacing speed multiplier. 
 
Description:
The pacing counters can be incremented by using the steps defined by this API
on every timebase toggle. The steps are: 1x, 2x, 4x, 8x, 16x, 32x, 64x, 
and 128x. The 'Speed' argument below is the multiplier from the steps given 
above. 
 
Returns:
    BERR_SUCCESS                - Packetizers disabled.
    BERR_INVALID_PARAMETER      - Bad input parameter
****************************************************************************/
BERR_Code BXPT_Playback_P_SetPacingSpeed( 
    BXPT_Playback_Handle hPb,                   /* [in] Handle for the playback channel */
    unsigned Speed
    );

/***************************************************************************
Summary:
Set the pacing count value. 
 
Description:
Give the pacing logic an initial value. This will force the value to be 
loaded by each of the playback channels referenced by the same TsMux object. 
 
Returns: 
    void 
****************************************************************************/
void BXPT_Playback_P_SetPacingCount(
    BXPT_Playback_Handle hPb,                   /* [in] Handle for the playback channel */
    unsigned PacingLoadMap,
    unsigned PacingCount
    );

#endif

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
#if BXPT_HAS_MULTICHANNEL_PLAYBACK
/***************************************************************************
Description:
Allocate a pacing counter from the pool. The allocated counter 
is given to the playback channel through the 
BXPT_Playback_ChannelSettings.PacingCounter structure member. 

This is needed only if pacing will be used on the playback 
channel. Calls to BXPT_Playback_ConfigPacing() will fail if the
channel has not been given a counter beforehand.
****************************************************************************/
BXPT_Playback_PacingCounter BXPT_Playback_AllocPacingCounter( 
    BXPT_Handle hXpt           /* [in] Handle for this transport */
    );

/***************************************************************************
Description:
Release a previously allocated pacing counter. The caller's  
storage location for the handle will be NULL'd.
****************************************************************************/
void BXPT_Playback_FreePacingCounter( 
    BXPT_Playback_PacingCounter PacingCounter
    );
#endif

/***************************************************************************
Description:

Variables to control the features in the multichannel descriptors. 
****************************************************************************/
typedef struct BXPT_PvrDescriptorFlags 
{
    BAVC_TsMux_DescConfig muxFlags;
    bool PushPartialPacket;
    bool PauseAtDescEnd;
} 
BXPT_PvrDescriptorFlags;

/***************************************************************************
Description:
Initialize the flags structure with default values. 
****************************************************************************/
void BXPT_Playback_InitDescriptorFlags( 
    BXPT_PvrDescriptorFlags *flags
    );

/***************************************************************************
Description:
Set the bitfields in the descriptor using the flag values passed in. 
***************************************************************************/
void BXPT_Playback_SetDescriptorFlags(
    BXPT_Playback_Handle hPb,   /* [in] Handle for the playback channel */
    BXPT_PvrDescriptor *desc, 
    const BXPT_PvrDescriptorFlags *flags
    );
#endif

/*
** These functions are called internally. 
** Users should NOT uses these functions directly.
*/

uint32_t BXPT_Playback_P_ReadReg( BXPT_Playback_Handle PlaybackHandle, uint32_t Pb0RegAddr );
void BXPT_Playback_P_WriteReg( BXPT_Playback_Handle PlaybackHandle, uint32_t Pb0RegAddr, uint32_t RegVal );
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
unsigned BXPT_Playback_P_GetBandId( BXPT_Playback_Handle hPb );
void BXPT_Playback_P_SetBandId( BXPT_Playback_Handle hPb, unsigned NewBandId );
#endif

#if BXPT_HAS_MULTICHANNEL_PLAYBACK
void BXPT_Playback_P_Init(
	BXPT_Handle hXpt 	   					/* [in] Handle for this transport */
    );

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
BERR_Code BXPT_Playback_P_LoadPacingCounter(
    BXPT_Playback_Handle hPb,
    unsigned long pacingCount
    );

BERR_Code BXPT_Playback_P_LoadStcMuxDelayDiff(
    BXPT_Playback_Handle hPb
    );
#endif
#endif

#ifdef __cplusplus
}
#endif

#endif /* #ifndef BXPT_PLAYBACK_H__ */

/* end of file */


