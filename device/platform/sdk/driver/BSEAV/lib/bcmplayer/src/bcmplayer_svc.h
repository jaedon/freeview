/***************************************************************************
 *     Copyright (c) 2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bcmplayer_svc.h $
 * $brcm_Revision: 6 $
 * $brcm_Date: 6/28/11 4:29p $
 *
 * Module Description: 
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bcmplayer/src/bcmplayer_svc.h $
 * 
 * 6   6/28/11 4:29p vsilyaev
 * SW7422-14: Changed index format to use access unit as a frame boundary
 * 
 * 5   4/15/11 3:04p vsilyaev
 * SW7422-14: Add more API into SVC player
 * 
 * 4   4/13/11 2:43p vsilyaev
 * SW7422-14: Added code to return current position
 * 
 * 3   4/12/11 7:33p vsilyaev
 * SW7422-14: Updated algorithm for locating matching frames
 * 
 * 2   3/23/11 3:00p vsilyaev
 * SW7422-14: Added facility to associate  stream id(pid) with a chunk of
 * payload
 * 
 * 1   2/11/11 12:16p vsilyaev
 * SW7422-14: Stub version of SVC player
 * 
 *
 ****************************************************************************/
#ifndef BCMPLAYER_AVC_H
#define BCMPLAYER_AVC_H

#include "bcmindexer_svc.h"

typedef struct BNAV_Player_SVC_Pid {
    unsigned short pid;
    long currentIndex;
} BNAV_Player_SVC_Pid ;

#define BNAV_PLAYER_SVC_MAX_PIDS    2

typedef struct BNAV_Player_SVC_HandleImpl {
    BNAV_Player_Handle parent;
    BNAV_Player_SVC_Pid pids[BNAV_PLAYER_SVC_MAX_PIDS];
    bool after_seek;
    long currentIndex;
    uint64_t currentOffset; /* should be a transport packet alligned */
} BNAV_Player_SVC_HandleImpl;


int BNAV_Player_SVC_Reset(BNAV_Player_SVC_HandleImpl *player, BNAV_Player_Handle parent);
int BNAV_Player_SVC_PopulateFifo(BNAV_Player_SVC_HandleImpl *player);
void BNAV_Player_SVC_SetCurrentIndex(BNAV_Player_SVC_HandleImpl *player, long index);
int BNAV_Player_SVC_GetPositionInformation(BNAV_Player_SVC_HandleImpl *player, long index, BNAV_Player_Position *position);
long BNAV_Player_SVC_FindIndexFromOffset( BNAV_Player_SVC_HandleImpl *player, uint64_t offset);
long BNAV_Player_SVC_FindIFrameFromIndex( BNAV_Player_SVC_HandleImpl *player, long index, eBpDirectionParam dir);
long BNAV_Player_SVC_FindIndexFromTimestamp( BNAV_Player_SVC_HandleImpl *player, unsigned long timestamp);
bool BNAV_Player_SVC_IndexIsBFrame(BNAV_Player_SVC_HandleImpl *player, long index);

#endif /* BCMPLAYER_AVC_H */

