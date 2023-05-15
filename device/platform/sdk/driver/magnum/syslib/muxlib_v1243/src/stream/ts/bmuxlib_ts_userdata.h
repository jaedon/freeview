/***************************************************************************
 *     Copyright (c) 2003-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmuxlib_ts_userdata.h $
 * $brcm_Revision: Hydra_Software_Devel/10 $
 * $brcm_Date: 4/22/13 5:58p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/syslib/muxlib/src/stream/ts/bmuxlib_ts_userdata.h $
 * 
 * Hydra_Software_Devel/10   4/22/13 5:58p delkert
 * SW7425-4709: Add continuity count checking on userdata packets. Skip
 * Packet parsing if packet was already parsed but was outside selection
 * window.
 *
 * Hydra_Software_Devel/9   4/5/13 5:19p delkert
 * SW7425-4643: Remove obsolete code. Move system data pre-Q code to its
 * own routine so it can be called after A/V input processing. Keep track
 * of found A/V/PCR PIDs. Add A/V/PCR PID conflict checking on system
 * data and userdata. Disabled userdata input if PID conflict detected.
 *
 * Hydra_Software_Devel/8   4/5/12 9:21a delkert
 * SW7425-2567: Add TEST_MODE build option to dump CSV file of userdata
 * timing information
 *
 * Hydra_Software_Devel/7   3/27/12 3:14p delkert
 * SW7425-2598: Fix up timing values. Turn off debug workaround
 *
 * Hydra_Software_Devel/6   3/27/12 8:31a delkert
 * SW7425-2598: Refactor packet parser to better handle packets that do
 * not conform to ETSI EN 300 472
 *
 * Hydra_Software_Devel/5   2/28/12 11:32a delkert
 * SW7425-2217: Improve detection and handling of non-conforming packets.
 * Improve debug. Cleanup.
 *
 * Hydra_Software_Devel/4   2/22/12 12:56p delkert
 * SW7425-2217: Redefine data structures to better separate settings from
 * each userdata PID. Add *temporary* workaround to allow streams with
 * bad PTS to pass thru (for testing purposes only).
 *
 * Hydra_Software_Devel/3   2/20/12 2:59p delkert
 * SW7425-2217: Add support for PES without PTS and schedule them based on
 * space availability
 *
 * Hydra_Software_Devel/2   2/17/12 4:28p delkert
 * SW7425-2217: Modify scheduler to process one PES per input until all
 * ready PES are scheduled, and to remember the last-scheduled input.
 *
 * Hydra_Software_Devel/1   2/16/12 4:30p nilesh
 * SW7425-2217: Merged TS VBI Userdata insertion support
 *
 * Hydra_Software_Devel/SW7425-2217/2   2/16/12 3:38p delkert
 * SW7425-2217: Fix buffer wrap problem.  deltaPTS calc should be
 * deltaDTS. Output warnings if data dropped.
 *
 * Hydra_Software_Devel/SW7425-2217/1   2/14/12 10:56a delkert
 * SW7425-2217: Move userdata code to separate file
 *
 ***************************************************************************/

#ifndef BMUXLIB_TS_USERDATA_H_
#define BMUXLIB_TS_USERDATA_H_

#ifdef __cplusplus
extern "C" {
#endif

/* enable the following to allow processing of streams that have badly
   encoded PTS values - testing purposes only */
/*#define TS_UD_BAD_PTS_WORKAROUND*/

/* enable this to always show dropped data warnings */
/*#define TS_UD_ALWAYS_SHOW_DROPPED_PACKETS*/

#ifdef BMUXLIB_TS_P_TEST_MODE
#include <stdio.h>
#endif

/*************************/
/*   Constants           */
/*************************/
/* scaling factor for converting PTS (90kHz) to ESCR (27MHz) */
#define BMUXLIB_TS_SCALE_PTS_TO_ESCR            300

/* 50ms window in 45kHz (PTS) units */
#define BMUXLIB_TS_USERDATA_DEFAULT_WINDOW      (50 * 45)

/* max number of segments a single userdata PES can be split into:
   (header prior to PTS, PTS, DTS(if present), data after PTS)
   Packets that do not contain a PES will use only one segment */
#define BMUXLIB_TS_USERDATA_MAX_SEGMENTS        4

/* Userdata requires approx 6 TS packets per video frame
   (max of 1128 bytes per frame for North American VBI, as per
    ANSI_SCTE 127, Section 8.1)
   or 8 TS packets per video frame (max of 1504 bytes per frame,
   for ETSI Teletext, as per ETSI EN 300 472, Section 5)
   Thus, we assume worst case requirement of 8 TS packets per
   video frame, per userdata input.
   (here assume createSettings.uiPESHeadersCount represents max video frames
   so total count can be determined using:
   BMUXLIB_TS_USERDATA_MAX_PKTS_PER_VID * BMUXLIB_TS_MAX_USERDATAPIDS * createSettings.uiPESHeadersCount)
*/
#define BMUXLIB_TS_USERDATA_MAX_PKTS_PER_VID    8

/*
 * TS Packet constants
 */
#define BMUXLIB_TS_PACKET_SYNC                  0x47
#define BMUXLIB_TS_PAYLOAD_LENGTH               184
#define BMUXLIB_TS_HEADER_LENGTH                4
#define BMUXLIB_TS_PACKET_LENGTH                (BMUXLIB_TS_HEADER_LENGTH + BMUXLIB_TS_PAYLOAD_LENGTH)

/*
 * PES Packet constants
 */

/* A PTS or DTS entry in a PES header requires 5 bytes */
#define BMUXLIB_PES_PTS_LENGTH                  5

/* number of bytes in PES header up to and including the length */
#define BMUXLIB_PES_HEADER_LENGTH               6

/* length of PES optional fields flags and header_data_length */
#define BMUXLIB_PES_HEADER_FLAGS_LENGTH         3

/* header length up to and including DTS (if present)
   This is the minimum number of bytes we need to process the PES packet s*/
#define BMUXLIB_PES_MIN_HEADER_LENGTH           (BMUXLIB_PES_HEADER_LENGTH + BMUXLIB_PES_HEADER_FLAGS_LENGTH + (BMUXLIB_PES_PTS_LENGTH * 2))

/* ISO/IEC-13818-1 Private Stream 2 */
#define BMUXLIB_PES_TYPE_PSM                 0xBC
#define BMUXLIB_PES_TYPE_PADDING             0xBE
#define BMUXLIB_PES_TYPE_PRIVATE_2           0xBF
#define BMUXLIB_PES_TYPE_ECM                 0xF0
#define BMUXLIB_PES_TYPE_EMM                 0xF1
#define BMUXLIB_PES_TYPE_DSMCC               0xF2
#define BMUXLIB_PES_TYPE_H222_TYPE_E         0xF8
#define BMUXLIB_PES_TYPE_PSD                 0xFF

/*
 * Field test/access macros
 */
#define BMUXLIB_TS_GET_PID(ts)   \
            (((uint16_t)(ts[1] & 0x1F) << 8) | ts[2])

#define BMUXLIB_TS_GET_CC(ts)    \
            ((uint8_t)(ts[3] & 0xF))

#define BMUXLIB_TS_IS_PAYLOAD_START(ts) \
            (0 != ((ts)[1] & 0x40))

#define BMUXLIB_TS_IS_ADAPT_PRESENT(ts) \
            (0 != ((ts)[3] & 0x20))

#define BMUXLIB_TS_IS_PAYLOAD_PRESENT(ts) \
            (0 != ((ts)[3] & 0x10))

#define BMUXLIB_TS_GET_ADAPT_LEN(ts)   (ts)[4]

#define BMUXLIB_TS_IS_DISCONTINUITY(ts)   \
            (0 != ((ts)[5] & 0x80))

#define BMUXLIB_PES_IS_PTS_PRESENT(pes) \
            (0 != ((pes)[7] & 0x80))

#define BMUXLIB_PES_IS_DTS_PRESENT(pes) \
            (0 != ((pes)[7] & 0x40))

/* determine if the PES has the optional
   fields after the length (e.g. PTS/DTS etc)
   (i.e. this is not a private stream, padding
    stream, etc) */
#define BMUXLIB_PES_HAS_OPTIONAL_FIELDS(id) \
   (((id) != BMUXLIB_PES_TYPE_PSM) && ((id) != BMUXLIB_PES_TYPE_PADDING) && ((id) != BMUXLIB_PES_TYPE_PRIVATE_2)  \
     && ((id) != BMUXLIB_PES_TYPE_ECM) && ((id) != BMUXLIB_PES_TYPE_EMM) && ((id) != BMUXLIB_PES_TYPE_DSMCC)   \
     && ((id) != BMUXLIB_PES_TYPE_H222_TYPE_E) && ((id) != BMUXLIB_PES_TYPE_PSD))

#define BMUXLIB_PES_GET_STREAM_ID(pes) (pes)[3]

#define BMUXLIB_PES_GET_LENGTH(pes)  \
            (((uint16_t)((pes)[4]) << 8) | (pes)[5])

#define BMUXLIB_PES_GET_HDR_DATA_LEN(pes) (pes)[8]

#define BMUXLIB_PES_GET_PTS(pes)  \
            ((((uint64_t)((pes)[9] & 0x0e)) << 29) | \
            (((uint64_t)(pes)[10]) << 22) | \
            (((uint64_t)((pes)[11] & 0xFE)) << 14) | \
            (((uint64_t)(pes)[12]) << 7) | \
            (((uint64_t)((pes)[13] & 0xFE)) >> 1))

#define BMUXLIB_PES_GET_DTS(pes) \
            ((((uint64_t)((pes)[14] & 0x0e)) << 29) | \
            (((uint64_t)(pes)[15]) << 22) | \
            (((uint64_t)((pes)[16] & 0xFE)) << 14) | \
            (((uint64_t)(pes)[17]) << 7) | \
            (((uint64_t)((pes)[18] & 0xFE)) >> 1))

/* macro for writing a PTS or DTS field into a PTS entry */
#define BMUXLIB_TS_USERDATA_SET_PTS_DTS(buffer, pts)   \
            (buffer)[4] = (((pts) << 1) & 0xFE) | 0x01; \
            (buffer)[3] = (((pts) >> 7) & 0xFF); \
            (buffer)[2] = (((pts) >> 14) & 0xFE) | 0x01; \
            (buffer)[1] = (((pts) >> 22) & 0xFF); \
            (buffer)[0] = (((pts) >> 29) & 0x0E) | 0x21

/*************************/
/*   Definitions         */
/*************************/

/* specific to companion video - applies to all userdata inputs that refer to this companion video */
typedef struct BMUXlib_TS_P_UserdataVideoInfo
{
   uint16_t uiPID;                  /* PID of the companion video input */
   bool bStartup;                   /* true = skip initial blank frames */
   bool bPrevDTSValid;
   uint32_t uiTargetPTS45kHz;       /* target PTS in the userdata domain for the current companion video frame */
   uint32_t uiTimingAdjust45kHz;    /* timing adjustment to convert from source timing to encoder timing */
   uint32_t uiPrevDTS45kHz;
   uint32_t uiDeltaDTS45kHz;
   bool bSelectionWindowSet;        /* dPTS has been determined, so selection window can be set to desired value */
   uint32_t uiSelectionWindow;      /* the current selection window for determining which userdata PES to process with the current frame */
   uint32_t uiOrigPTS;              /* for debug: original video PTS for cross-reference */
} BMUXlib_TS_P_UserdataVideoInfo;

typedef struct BMUXlib_TS_P_UserdataReleaseQEntry
{
   uint32_t uiSequenceID;              /* identifies the correct release order */
   size_t uiDataLength;                /* number of bytes to consume */
   struct BMUXlib_TS_P_UserdataReleaseQEntry *pNext;
} BMUXlib_TS_P_UserdataReleaseQEntry;

typedef struct
{
   BMUXlib_TS_P_UserdataReleaseQEntry *pHead;
   BMUXlib_TS_P_UserdataReleaseQEntry *pTail;
   uint32_t uiSequenceID;              /* the current sequence number of the outgoing segment */
   uint32_t uiExpectedSeqID;           /* next expected sequence number to be freed */
} BMUXlib_TS_P_UserdataReleaseQ;

typedef struct
{
   uint32_t uiBytesProcessed;          /* num bytes processed in this TS packet - for locating PES header */
   bool bParsed;                       /* indicates this packet has already been parsed */
   bool bPayloadStart;                 /* PES payload starts in this packet */
   bool bAdaptationPresent;
   bool bPayloadPresent;
   bool bPTSPresent;
   bool bDTSPresent;
   uint32_t uiPTS45kHz;
   uint32_t uiDTS45kHz;
   uint16_t uiPID;
   uint8_t uiCC;
} BMUXlib_TS_P_UserdataPacketInfo;

/* userdata information specific to a particular userdata input */
typedef struct BMUXlib_TS_P_UserdataInfo
{
   bool bDisabled;                  /* indicates the input is disabled due to PID conflict */
   bool bFirstPESSeen;              /* if the first PES header has not been seen, we need to drop payloads */
   bool bDropPES;                   /* drop the current PES packet - flag is sticky until next PUSI seen; it will drop any packet with a payload until PUSI */
   bool bUnwrapInUse;               /* true if the unwrap buffer is being used - for error checking */
   bool bPIDValid;                  /* the PID for this input has been determined */
   bool bPrevCCValid;               /* continuity count value has been initialized */
   uint8_t *pCurrentPacket;         /* location of current TS Packet (either in unwrap buffer, or from userdata input) */
   uint8_t *pUnwrap;                /* location of storage for unwrapping a TS packet */
   uint32_t uiPendingBytes;         /* number of bytes pending for this userdata input */
   uint32_t uiIndex;                /* index of this userdata input */
   BMUXlib_TS_P_UserdataReleaseQ stReleaseQ;
   BMUXlib_TS_P_UserdataVideoInfo *pCompanionVideo;
   BMUXlib_TS_P_UserdataPacketInfo stPacketInfo;   /* info about the current packet being parsed */
   uint16_t uiPID;                  /* The PID detected corresponding to this input */
   uint8_t uiPrevCC;                /* the previous continuity count value seen for this input */
#ifndef TS_UD_ALWAYS_SHOW_DROPPED_PACKETS
   bool bPacketAccepted;            /* for debug: indicates PES has been processed, so warnings are not due to startup conditions */
#endif
#ifdef TS_UD_BAD_PTS_WORKAROUND
   /* FIXME: The following is a hack for working around badly encoded PTS - for testing only */
   uint32_t uiTimingOffset45kHz;    /* NOTE: this should be zero for properly encoded streams */
#endif
} BMUXlib_TS_P_UserdataInfo;

/* the user data (TS) packets to be queued to transport */
typedef struct
{
   bool bESCRValid;                    /* indicates whether this Packet has a valid ESCR or not
                                          (packets belonging to PES without PTS, or packets without
                                          a payload, have no ESCR) */
   uint32_t uiESCR;                    /* the scheduling time for this Packet */
   uint32_t uiNumSegments;             /* Total number of data segments that make up this PES */
   struct stSegment
   {
      uint8_t *pData;
      uint32_t uiLength;
      BMUXlib_TS_P_DataType eDataType; /* the type of the data pointed to by pData (may be either userdata, local storage or PTS entry) */
                                       /* Note: Source is always "Userdata" to ensure bytes get freed from userdata input */
   } aSegments[BMUXLIB_TS_USERDATA_MAX_SEGMENTS];
} BMUXlib_TS_P_UserdataPending;

/* PTS entry for storing the modified PTS value for output to transport
   (since we are not permitted to modify the incoming userdata in-place) */
typedef struct
{
   uint8_t aPTS[BMUXLIB_PES_PTS_LENGTH];
} BMUXlib_TS_P_UserdataPTSEntry;

/* general status that applies to all userdata */
typedef struct BMUXlib_TS_P_UserdataStatus
{
   uint32_t uiNumInputs;
   uint32_t uiCurrentScheduledInput;   /* specifies the current userdata input that is due to be scheduled */
   uint32_t uiCurrentVideoPID;
#ifdef BMUXLIB_TS_P_TEST_MODE
   FILE *hUserdataCSV;
   bool bUserdataCSVOpened;
#endif
} BMUXlib_TS_P_UserdataStatus;

/* information about the incoming data for this userdata input */
typedef struct
{
   uint8_t *pData0, *pData1;
   uint32_t uiBytes0, uiBytes1, uiBytesTotal;
} BMUXlib_TS_P_UserdataInput;

/*************************/
/*  Private Userdata API */
/*************************/

BERR_Code   BMUXlib_TS_P_UserdataInit(BMUXlib_TS_Handle hMuxTS);

BERR_Code   BMUXlib_TS_P_Userdata_ProcessInputs(BMUXlib_TS_Handle hMuxTS);

void        BMUXlib_TS_P_Userdata_AddToReleaseQ(BMUXlib_TS_Handle hMuxTS, uint32_t uiUserdataIndex, uint32_t uiLength);

BERR_Code   BMUXlib_TS_P_Userdata_ProcessReleaseQueues(BMUXlib_TS_Handle hMuxTS);

void        BMUXlib_TS_P_Userdata_SchedulePackets(BMUXlib_TS_Handle hMuxTS);

bool        BMUXlib_TS_P_Userdata_FindTargetPTS(BMUXlib_TS_Handle hMuxTS, BMUXlib_TS_P_InputMetaData *pInput, BMUXlib_Input_Descriptor *pstDescriptor);

#ifdef __cplusplus
}
#endif

#endif /* BMUXLIB_TS_USERDATA_H_ */
