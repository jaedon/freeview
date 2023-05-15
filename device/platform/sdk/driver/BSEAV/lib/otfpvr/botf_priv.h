/***************************************************************************
 *	   Copyright (c) 2007-2009, Broadcom Corporation
 *	   All Rights Reserved
 *	   Confidential Property of Broadcom Corporation
 *
 *	THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *	AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *	EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: botf_priv.h $
 * $brcm_Revision: 6 $
 * $brcm_Date: 7/10/09 5:27p $
 *
 * Module Description:
 *      OTF private header file
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/otfpvr/botf_priv.h $
 * 
 * 6   7/10/09 5:27p vsilyaev
 * PR 56736: Added use decoder status to help detect livelock in the OTF
 * engine
 * 
 * 5   7/8/09 11:46a vsilyaev
 * PR 55989: Added handling of special EOS marker
 * 
 * 4   3/31/09 7:29p vsilyaev
 * PR 50887: OTFPVR support for the Nexus video decoder
 * 
 * 3   2/4/09 4:53p vsilyaev
 * PR 50887: Unwrapped main processing thread, removed dependencies on XVD
 * and RAVE handles
 * 
 * 2   2/4/09 2:32p vsilyaev
 * PR 50887: Removed globals and unneded functions. Demoted functions to
 * static. Rearrange structure members to allow compaction. Used const
 * modifier where appropriate.
 * 
 **************************************************************************/

#ifndef BOTF_PRIV_H_
#define BOTF_PRIV_H_

#include "botf_mem.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 * @file
 */

/*****************************************************************************
* BOTF_BufferPtrs
*****************************************************************************/
typedef struct BOTF_ParserPtrs
{
    botf_mem_t     mem; /* interface to convert between virtual and physicall addresses */
    const uint8_t *CdbStartPtr;           /* CDB Start pointer */
    const uint8_t *CdbReadPtr;            /* Current CDB read pointer */
    const uint8_t *CdbValidPtr;           /* Current CDB write pointer */
    const uint8_t *CdbEndPtr;             /* Current CDB End pointer */
    const uint8_t *CdbWrapAroundPtr;      /* Last CDB wraparound pointer */
    uint8_t *ItbStartPtr;                 /* ITB Start pointer */
    const uint8_t *ItbReadPtr;            /* Current ITB Read pointer */
    uint8_t *ItbValidPtr;                 /* Current ITB Write pointer */
    const uint8_t *ItbEndPtr;             /* Current ITB End pointer */
    const uint8_t *ItbWrapAroundPtr;      /* Last ITB wraparound pointer */
} BOTF_ParserPtrs;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#include "botf_scv_parser.h"
#include "botf_gop_manager.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/*****************************************************************************
* BOTF_FeederState
*****************************************************************************/
typedef enum  BOTF_FeederState {
        BOTF_FeederState_eRunning,
        BOTF_FeederState_ePaused, /* no parsing */
        BOTF_FeederState_eStopped,
        BOTF_FeederState_eDraining,
        BOTF_FeederState_eVbvInvalid
} BOTF_FeederState;

/*****************************************************************************
* BOTF_PlayPumpState
*****************************************************************************/
typedef struct BOTF_PlayPumpState
{
        bool                  OtfParser;   /* true if we parse MPEG stream and use secondary feed for decoder */
        BOTF_FeederMode       FeederMode;
        bpvr_gop_player_mode  PlayerMode;
} BOTF_PlayPumpState;

/*****************************************************************************
* BOTF_ParserState
*****************************************************************************/
typedef struct BOTF_ParserState
{
        BOTF_FeederState        State;
        BOTF_PlayPumpState		FeederState; /* current state of the feeder */
        const uint8_t           *ParserScvReadPtr;        
} BOTF_ParserState;

BDBG_OBJECT_ID_DECLARE(BOTF);


typedef struct BOTF_Data
{
    BREG_Handle              hBReg;
    BMEM_Handle              hBMem;
    BOTF_FeederState         State;

    botf_scv_parser          ScvParser;
    bpvr_gop_manager         GopManager;
    bpvr_gop_player          GopPlayer;    
    BOTF_ParserState         ParserState;
    unsigned int             DataEndTime;
    unsigned int             DecoderStuckTime;
    bool		             bOtfStop;
    bool                     bOtfTaskDone;
    bool					 bStarted;
    bool                     bUsePtsAsTag;
    BOTF_Config		         Settings;

    uint32_t                 InputParserCDBSize;  /* CDB size for input parser */
    uint32_t                 InputParserITBSize;  /* ITB size for input parser */
    uint32_t                 OPParserITBSize;     /* ITB size for output parser */
    uint32_t                 OPParserITBAlign;    /* ITB alignment in bits for output parser */
    BAVC_XptContextMap       IpParserRegMap;      /* Input Parser register map */
    BAVC_XptContextMap       OpParserRegMap;      /* Output Parser register map */
    BOTF_ParserPtrs          IPParserPtrs;        /* Local copy of latest IP parser buffer pointers*/
    BOTF_ParserPtrs          OPParserPtrs;        /* Local copy of latest OP parser buffer pointers*/    
    struct {
        uint32_t CdbStart; 
        uint32_t ItbStart;
    } OpParserRegSave;     /* Local copy of OP Parser pointers when OTF was opened */
    BOTF_VideoDecoderStatus  prevVideoDecoderStatus;
    BKNI_EventHandle         DataEndEvent;
    botf_mem    mem;

    BDBG_OBJECT(BOTF)
} BOTF_Data ;


void BOTF_P_FrameCountCallBack(void *hOtf, uint32_t chunk_no, uint32_t frame_cnt);
void BOTF_P_DetectedEof(void *hOtf);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BOTF_PRIV_H_ */

