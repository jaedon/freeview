/***************************************************************************
 *     Copyright (c) 2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bcmplayer_impl.h $
 * $brcm_Revision: 4 $
 * $brcm_Date: 4/15/11 3:04p $
 *
 * Module Description: 
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bcmplayer/src/bcmplayer_impl.h $
 * 
 * 4   4/15/11 3:04p vsilyaev
 * SW7422-14: Add more API into SVC player
 * 
 * 3   4/13/11 2:43p vsilyaev
 * SW7422-14: Added code to return current position
 * 
 * 2   2/16/11 5:41p vsilyaev
 * SW7422-14: Updated handling frame-at-a-time mode
 * 
 * 1   2/11/11 12:15p vsilyaev
 * SW7422-14: Added hooks for SVC player
 * 
 *
 ****************************************************************************/
#ifndef BCMPLAYER_IMPL_H
#define BCMPLAYER_IMPL_H



/* const BNAV_PktFifoEntry *BNAV_Player_ExtractFifoEntry(BNAV_Player_Handle handle); */
int BNAV_Player_AddData(BNAV_Player_Handle handle, uint64_t offset, unsigned long size, unsigned long pts,
    int btpMode, int display, int skip, unsigned pid);
int BNAV_Player_AddSimpleData(BNAV_Player_Handle handle, uint64_t offset, unsigned long size);
int BNAV_Player_AddStartCode(BNAV_Player_Handle handle, uint8_t startcode);


/*---------------------------------------------------------------
- PRIVATE FUNCTIONS
---------------------------------------------------------------*/
typedef struct BNAV_Player_State {
    eBpPlayModeParam    playMode;
    eBpDirectionParam   playDir;
    eBpLoopModeParam    loopMode;
    unsigned            advanceCount;  
    unsigned long       normalPlayBufferSize;
    unsigned short      videoPid;            /*  Video pid for the stream */
    unsigned short      enhancementVideoPid; /*  Enhancement Video pid for the stream */
    uint8_t             timestampOffset;    
    uint8_t             packetSize;    
} BNAV_Player_State;

BNAV_Entry *BNAV_Player_ReadEntry(BNAV_Player_Handle handle, long index);
void BNAV_Player_P_GetState(BNAV_Player_Handle handle, BNAV_Player_State *state);
void BNAV_Player_P_AddToPTSCache(BNAV_Player_Handle handle, uint32_t pts, long index);
long BNAV_Player_P_SearchIndexByTimestamp(BNAV_Player_Handle handle, unsigned long timestamp, void *context, int (*read_timestamp)(void *, long, unsigned long *));
long BNAV_Player_P_FindIndexForOffset(BNAV_Player_Handle handle, uint64_t offset, void *context, int (*read_offset)(void *, long, uint64_t *));
long BNAV_Player_P_GetFirstIndex(BNAV_Player_Handle handle);

#endif /* BCMPLAYER_IMPL_H */

