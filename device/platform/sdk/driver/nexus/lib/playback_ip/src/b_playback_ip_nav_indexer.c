/***************************************************************************
 *     (c)2006-2012 Broadcom Corporation
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
 * $brcm_Workfile: b_playback_ip_nav_indexer.c $
 * $brcm_Revision: 16 $
 * $brcm_Date: 7/16/12 3:33p $
 *
 * Module Description: wrapper module for NAV indexer
 *  
 * Revision History:
 *    
 * $brcm_Log: /nexus/lib/playback_ip/src/b_playback_ip_nav_indexer.c $
 * 
 * 16   7/16/12 3:33p ssood
 * SW7425-3201: support for streaming trickmode content using playSpeed w/
 *  non-Broadcom clients (disable use of BTPs and switch to i-frame only
 *  modes)
 * 
 * 15   6/11/12 4:57p ssood
 * SW7425-3201: Add support for new DLNA PlaySpeed trickmode options in
 *  playback ip library
 * 
 * 14   1/11/12 5:54p ssood
 * SW7425-906: fix for guard violations during fast rewind trickplays
 * 
 * 13   8/9/11 5:16p mward
 * SW7125-1075:  Check return from BNAV_Player_Open().
 * 
 * 12   7/13/11 9:06a ssood
 * SW7420-1883: Android port
 * 
 * 11   6/6/11 10:34a ssood
 * SW7231-166: skip P frames at lower speeds (4x, 8x) to avoid occasional
 *  macroblocking
 * 
 * 10   6/6/11 1:35a ssood
 * SW7231-166: refine trickplay rates, account for timeSeekEnd from
 *  ip_client
 * 
 * 9   2/27/11 10:13p ssood
 * SW7425-133: add support to create index for 192 byte transport streams
 * 
 * 8   6/1/10 11:14p ssood
 * SW7420-780: need to use the 6byte entries for SCT during nav indexer
 *  creation
 * 
 * 7   4/2/10 4:23p ssood
 * SW7420-561: Fixed Coverity issues
 * 
 * 6   3/11/10 5:49p ssood
 * SW7420-561: For AVC Streams, configure BCM player to only send I Frames
 *  even at 2x
 * 
 * 5   1/23/10 11:16a ssood
 * SW7420-454: added support to extract 1st PTS from TS files, store in
 *  info file & send to clients in HTTP resp
 * 
 * 4   1/19/10 4:55p ssood
 * SW7420-454: convert verbose output to be available via msg modules
 *  flags
 * 
 * 3   1/18/10 5:00p ssood
 * SW7420-454: Add support to create index w/o using the createindex
 *  utility
 * 
 * 2   12/8/09 2:06p ssood
 * SW7420-454: disable file type determinination until player library is
 *  enhanced to provide this
 * 
 * 1   12/8/09 1:46p ssood
 * SW7420-454: Add initial support for server based trick modes (not fully
 *  DLNA compliant as DLNA requires server to send MPEG compliant streams:
 *  correct frame rate & w/ timing information)
 * 
 * 
 *************************************************************/ 
#define _FILE_OFFSET_BITS 64
#include "b_playback_ip_lib.h"
#include "b_playback_ip_priv.h"
#include "bcmplayer.h"
#include "b_playback_ip_nav_indexer.h"
#include "nexus_types.h"
#include "tsindexer.h"
#include "bcmindexer.h"
#include "bcmindexer_nav.h"

BDBG_MODULE(b_playback_ip_nav_indexer);
/*---------------------------------------------------------------
- PRIVATE DEFINITIONS AND GLOBALS
---------------------------------------------------------------*/
#ifdef BRCM_DBG
    #define PLAY_DBGMSG(x) { printf("%s:%d-",__FUNCTION__,__LINE__); printf x ; }
#else
    #define PLAY_DBGMSG(x) ((void)0)
#endif

#ifndef MIN
    #define MIN(a,b) ((a)<(b) ? (a) : (b))  /* min macro */
#endif

static long bplay_read(void *buffer, long size, long count, void *fp )
{
    return fread(buffer,size,count,fp);
}

static int bplay_bounds(BNAV_Player_Handle handle, void *filePointer, long *firstIndex, long *lastIndex)
{
    FILE * fp = filePointer;
    long size, cur_pos;
    *firstIndex = -1; 
    *lastIndex = -1;

    BSTD_UNUSED(handle);
    cur_pos = ftell( fp);
    if (cur_pos<0) {
        BDBG_ERR(("%s: ftell (1) failed to provide the offset of the current position: errno %d", __FUNCTION__, errno));
        cur_pos=0;
    }

    fseek( fp, 0 , SEEK_END);
    size = ftell(fp);
    if (size < 0) {
        BDBG_ERR(("%s: ftell (2) failed to provide the offset of the current position: errno %d", __FUNCTION__, errno));
        return 1;
    }

    fseeko( fp, cur_pos,  SEEK_SET);
    *firstIndex = 0;
    *lastIndex = (size/sizeof(BNAV_Entry)) -1;

    return 0;
}

static long bplay_tell( void *fp )
{
    return ftell(fp);
}

static int bplay_seek( void *fp, long offset, int origin )
{
    return fseek(fp,offset,origin);
}

int  nav_indexer_open(
    void **context, 
    FILE *fp,
    B_PlaybackIpPsiInfoHandle psi
    )
{
    BNAV_Player_Settings cfg;
    BNAV_DecoderFeatures features = {1,1,1,1};
    BNAV_Player_PlayMode playMode;
    long firstIndex, lastIndex;
    BNAV_Player_Handle h ;
    BNAV_Player_Position p0;
    
    BNAV_Player_GetDefaultSettings(&cfg);
    
    BDBG_MSG(("%s\n",__FUNCTION__));
    cfg.filePointer = fp;
    cfg.normalPlayBufferSize = STREAMING_BUF_SIZE;
    cfg.decoderFeatures = features;
    cfg.readCb = bplay_read;
    cfg.tellCb = bplay_tell;
    cfg.seekCb = bplay_seek;
    cfg.boundsCb = bplay_bounds;
    cfg.transportTimestampEnabled = psi->transportTimeStampEnabled;
    cfg.navVersion = (psi->videoCodec == NEXUS_VideoCodec_eH264) ? BNAV_Version_AVC: BNAV_Version2;
    cfg.skipSanityCheck = 1;
    cfg.videoPid = psi->videoPid;
    cfg.useBtpsForHostTrickModes = false;
    
	if (BNAV_Player_Open( (BNAV_Player_Handle *)context, &cfg)!=0) 
	{ 
		BDBG_ERR(("BCM Player Creation Failed\n"));
		return -ENOMEM;
	}    
    
    BDBG_MSG(("BNAV Player = %p\n",*context));
    
    BNAV_Player_GetSettings((BNAV_Player_Handle) *context, &cfg);
    playMode.playMode = eBpPlayNormal;
    playMode.playMode = eBpPlayNormalByFrames;
    playMode.playModeModifier = 1;
    /*playMode.loopMode = eBpLoopForever;*/
                /*eBpSinglePlay;*/
    playMode.loopMode = eBpSinglePlay;
    playMode.disableExtraBOptimization = 0;
    BNAV_Player_SetPlayMode((BNAV_Player_Handle) *context, &playMode);
    
    h = (BNAV_Player_Handle) *context;
    BNAV_Player_GetSettings(h, &cfg);
    BNAV_Player_DefaultGetBounds(h, cfg.filePointer, &firstIndex, &lastIndex);
    BNAV_Player_SetCurrentIndex(h, firstIndex);
    if (BNAV_Player_GetPosition(h, &p0)) {
        BDBG_WRN(("BNAV_Player_GetPosition failed ...\n"));
    }
    else
        psi->firstPts = p0.pts;
    return 0;
}

void nav_indexer_close(void *context)
{
    BNAV_Player_Handle h = (BNAV_Player_Handle) context;
    BNAV_Player_Close(h);
}

int nav_indexer_seek(void *context, unsigned long targetPts, unsigned long *duration, unsigned long *first_pts)
{
    BNAV_Player_Handle h = (BNAV_Player_Handle) context;
    BNAV_Player_Position p, p0,p1;
    BNAV_Player_Settings cfg;
#if 0
    /* see if these are needed ? */
    BNAV_Player_PlayEntry navEntry;
    BNAV_Player_PlayMode mode;
#endif
    long firstIndex, lastIndex,currentIndex;
    int rc=0;
    
    memset(&p0,0,sizeof(p0));

    BNAV_Player_GetSettings(h, &cfg);
    BNAV_Player_DefaultGetBounds(h, cfg.filePointer, &firstIndex, &lastIndex);
    BDBG_MSG(("firstIndex=%ld lastIndex=%ld",firstIndex,lastIndex));

    /* Get First PTS */
    BNAV_Player_SetCurrentIndex(h, firstIndex);
    while(1){
    
        rc = BNAV_Player_GetPosition(h, &p0);
        if(rc){
            BDBG_ERR(("BNAV_Player_GetPosition failed ...\n"));
            return rc;
        }

        BDBG_MSG(("Index=%ld FirstPTS = %lu\n",firstIndex, p0.pts));
        if(p0.pts != 0)
            break;

        if(firstIndex == lastIndex){
            BDBG_MSG(("No PTS found\n"));
            break;
        }
        firstIndex ++;

    }

    /* Get Last PTS */
    lastIndex &= ~1; /*Force to even number */
    BNAV_Player_SetCurrentIndex(h, lastIndex);
    rc = BNAV_Player_GetPosition(h, &p1);
    if(rc){
        BDBG_ERR(("BNAV_Player_GetPosition failed ...\n"));
        return rc;
    }

    printf("LastPTS = %lu\n",p1.pts);
    *duration = (p1.pts - p0.pts);
        *first_pts = p0.pts;

    if(targetPts && ((targetPts - p0.pts)>0) && ((p1.pts - targetPts)>0)){
        BDBG_MSG(("targetPts %lu, p0.pts %lu, p1.pts %lu\n", targetPts, p0.pts, p1.pts));
        currentIndex = 1.* lastIndex * (targetPts - p0.pts)/(p1.pts - p0.pts);
        currentIndex &= ~1;
    }
    else{
        BDBG_MSG(("in 2\n"));
        currentIndex = firstIndex;
    }

    /* TODO: look into current index calc above */
    BNAV_Player_SetCurrentIndex(h, currentIndex);
    rc = BNAV_Player_GetPosition(h, &p);
    if(rc){
        BDBG_ERR(("BNAV_Player_GetPosition failed ...\n"));
        return rc;
    }
    BDBG_MSG(("currentIndex=%ld, pts %lu\n",currentIndex, p.pts));
    
    return 0;
}

int nav_indexer_pts(void *context, index_entry_t *index)
{
    BNAV_Player_Handle h = (BNAV_Player_Handle) context;
    BNAV_Player_Position position;
    BNAV_Entry entry;
    BNAV_AVC_Entry avcEntry;
    int rc;
    BNAV_Player_Status status;
    BNAV_Player_Settings settings;

    rc = BNAV_Player_GetPosition(h, &position);

    if(rc){
        BDBG_ERR(("BNAV_Player_GetPosition failed ..."));
        return rc;
    }

    /* Seek to the proper offset in the stream */
    index->pts = position.pts;

    /* now get the frame type */
    BNAV_Player_GetStatus(h, &status);
    BNAV_Player_GetSettings(h, &settings);
    if (settings.navVersion == BNAV_Version_AVC) {
        rc = BNAV_Player_ReadAvcIndex(h, status.currentIndex, &avcEntry);
        if (rc) { BDBG_ERR(("BNAV_Player_ReadAvcIndex failed ...")); return -1;} 
        index->type = BNAV_get_frameType(&avcEntry);
    }
    else {
        rc = BNAV_Player_ReadIndex(h, status.currentIndex, &entry);
        if (rc) { BDBG_ERR(("BNAV_Player_ReadIndex failed ...")); return -1;} 
        index->type = BNAV_get_frameType(&entry);
    }
    return 0;
}

int nav_indexer_next(void *context, index_entry_t *index)
{
    BNAV_Player_Handle h = (BNAV_Player_Handle) context;
    BNAV_Player_PlayEntry navEntry;
    index_entry_t index2;
    
    int rc;
    memset(&navEntry, 0, sizeof(navEntry));
    rc = BNAV_Player_GetNextPlayEntry( h, &navEntry, index->pkt );
    if(rc) {
        BDBG_WRN(("BCMplayer returned nothing ...\n"));
        return -1;
    }
    /* Seek to the proper offset in the stream */
    index->offset =(((uint64_t)navEntry.startOffsetHi)<<32) | navEntry.startOffset;          
    index->size = navEntry.byteCount;
    index->insertPkt = navEntry.isInsertedPacket;

    nav_indexer_pts(context, &index2);
    index->pts = index2.pts;
    index->pcr = index2.pts;
    index->type = index2.type;
    /*printf("NAV %lld %lu\n",index->offset,index->size);*/
    if (navEntry.zeroByteCountBegin || navEntry.zeroByteCountEnd)
        BDBG_MSG(("!!!!! zeroByteCountBegin %ul, zeroByteCountEnd %ul !!!!! ", navEntry.zeroByteCountBegin, navEntry.zeroByteCountEnd));
    return 0;
}

void nav_indexer_setIndexByByteOffset(void *context, uint64_t byteOffset)
{
    BNAV_Player_Handle h = (BNAV_Player_Handle) context;
    long currentIndex;

    currentIndex = BNAV_Player_FindIndexFromOffset(h, (unsigned int)(byteOffset>>32), (unsigned int)(byteOffset));
    BDBG_MSG(("%s: NAV: byteOffset %llu, current index =%ld \n", __FUNCTION__, byteOffset, currentIndex));
    BNAV_Player_SetCurrentIndex(h, currentIndex);
}

int 
nav_indexer_setIndexByTimeOffset(void *context, double timeOffset)
{
    BNAV_Player_Handle h = (BNAV_Player_Handle) context;
    index_entry_t index;
    unsigned long pts, first_pts, duration;

    nav_indexer_pts(h, &index);
    pts = (unsigned long) index.pts + (unsigned long) (timeOffset * 45000); 
    BDBG_MSG(("%s: timeOffset %.3f, pts: 1st %lu, %lu", __FUNCTION__, timeOffset, index.pts, pts));
#if 1
    nav_indexer_seek(h, pts, &duration, &first_pts);
#else
    duration = timeOffset * 1000; /* convert to msec */
    if ( (currentIndex = BNAV_Player_FindIndexFromTimestamp(h, duration)) == -1) {
        BDBG_WRN(("Failed to find an index entry for npt %0.3f, msec %u", timeOffset, duration));
        return -1;
    }
    if (BNAV_Player_SetCurrentIndex(h, currentIndex)) {
        BDBG_ERR(("BNAV_Player_SetCurrentIndex failed to set index to %ld...", currentIndex));
        return -1;
    }
    if (BNAV_Player_GetPosition(h, &p)) {
        BDBG_ERR(("BNAV_Player_GetPosition failed ..."));
        return -1;
    }
    BDBG_MSG(("timeOffset %lld, msec %u, currentIndex=%ld, pts %lu",timeOffset, duration, currentIndex, p.pts));
#if 0
        nav_indexer_setIndexByTimeOffset: timeOffset 16.000, pts: 1st 27532821, 28252821
        timeOffset 4625196817309499392, msec 16000, currentIndex=484, pts 27897685
        setReadOffset: file offset 20632576 for entry offset 20632624 delta offset 48,
#endif
#endif
    return 0;
}

int  nav_indexer_rewind(void *context)
{
    BNAV_Player_Handle h = (BNAV_Player_Handle) context;
    BNAV_Player_PlayMode mode;
    BNAV_Player_Settings cfg;
    long firstIndex = -1, lastIndex = -1;
    int rc = -1;

    BNAV_Player_GetPlayMode(h, &mode);
    BNAV_Player_GetSettings(h, &cfg);
    if (BNAV_Player_DefaultGetBounds(h, cfg.filePointer, &firstIndex, &lastIndex) < 0) {
        BDBG_ERR(("%s: failed to get the nav file bounds, possible nav file corruption?", __FUNCTION__));
        rc = -1;
        goto out;
    }

    BDBG_WRN(("%s: Rewinding: firstIndex=%ld lastIndex=%ld",__FUNCTION__, firstIndex,lastIndex));

    if(mode.playModeModifier > 0 ) 
        rc = BNAV_Player_SetCurrentIndex(h, firstIndex);
    else
        rc = BNAV_Player_SetCurrentIndex(h, lastIndex); 
out:
    return rc;
}


bool
nav_indexer_mode(void *context, trick_mode_t *tm)
{
    BNAV_Player_Handle h = (BNAV_Player_Handle) context;

    BNAV_Player_PlayMode mode;
    BDBG_MSG(("BCMPlayer ModeChange: Speed=%d DIR=%d, modeModifier %d\n",tm->speed, tm->direction, tm->modifier));
    mode.playModeModifier = 1;
    BNAV_Player_GetPlayMode(h, &mode);

    if (tm->videoType == NEXUS_VideoCodec_eH264) {
        /* AVC Trickmodes (+ve speeds) */
        if(tm->direction>0){
            switch(tm->speed){
                case 1:
                    /* use this mode if we'r pacing to client */
                    /*TODO: Add code here to HTTP Pacing Mode */
                    /* mode.playMode = eBpPlayNormalByFrames;*/

                    /* Else, for non-pacing clients, use a more efficient block read-mode */ 
                    /* as it allows read by large fixed size blocks */
                    mode.playMode = eBpPlayNormal;
                    break;
                default:
                    mode.playMode = eBpPlayI;
                    mode.playModeModifier = tm->modifier;
                    break;
            }
        }
        else {
            /* AVC Trickmodes (-ve speeds) */
            switch(tm->speed){

            default:
                mode.playMode = eBpPlayI;
                mode.playModeModifier = -tm->modifier;
                break;

            }
        }
    }
    else {
            /* Mpeg2 Trickmodes (+ve speeds) */
            if(tm->direction> 0) {
                switch(tm->speed) {
                case 1:
                    /* use a more efficient block read-mode as it allows read by large fixed size blocks */
                    mode.playMode = eBpPlayNormal;
                    break;
#if 0
                    /* Commenting this out as we are switching to just iFrame only trickmodes */
                    /* once this is vetted out and tested, this code will be removed */
                case 2:
                    mode.playMode = eBpPlaySkipB;
                    mode.playModeModifier = 2; /* how many B Frames you want to show before skipping */
                    break;
                case 4:
                    mode.playMode = eBpPlaySkipP;
                    mode.playModeModifier = 5;  /* number of P's to be played per GOP */
                    mode.playMode = eBpPlayI;
                    mode.playModeModifier = tm->modifier;
                    break;

                case 8:
                    mode.playMode = eBpPlaySkipP;
                    mode.playModeModifier = 3;  /* number of P's to be played per GOP */
                    break;
#endif
    
                default:
                    mode.playMode = eBpPlayI;
                    mode.playModeModifier = tm->modifier;
                    break;
                }
            }
            else { 
                /* Mpeg2 Trickmodes (-ve speeds) */
                switch(tm->speed) {
#if 0
                /* 03042007: reverting back to I-frame only trick modes for rewind speeds due to macroblocking issues */
                case 1:
                    mode.playMode = eBpPlayBrcm;
                    mode.playModeModifier = tm->direction; 
                    break;
    
                case 2:
                case 4:
                    mode.playMode = eBpPlayDecoderGOPTrick;
                    mode.playModeModifier = tm->direction * ((tm->speed) * 100 + 10);
                    break;
#endif
    
                default:
                    mode.playMode = eBpPlayI;
                    mode.playModeModifier = -tm->modifier;
                    break;
                }
            }           
    }

    mode.disableExtraBOptimization = 0;
    
    if (BNAV_Player_SetPlayMode(h, &mode) < 0) {
        BDBG_WRN(("%s: Failed to set the play mode\n", __FUNCTION__));
        return false;
    }

    return true;
}

/**
* Feed from tsindexer to bcmindexer.
**/
static unsigned long 
write_to_bcmindexer(
    const void *p_readBuf, 
    unsigned long numEntries, 
    unsigned long entrySize,
    void *fp
    )
{
    BSTD_UNUSED(entrySize);
    return BNAV_Indexer_Feed((BNAV_Indexer_Handle)fp, (void*)p_readBuf, numEntries);
}

static FILE *g_mpegFile = NULL; /* global hack */
int mpegSizeCallback(BNAV_Indexer_Handle handle, unsigned long *hi, unsigned long *lo)
{
#ifdef LINUX
    off_t o = ftello(g_mpegFile);
#else
    off_t o = ftell(g_mpegFile);
#endif
    BSTD_UNUSED(handle);
    if (o == -1)
        return -1;
    *hi = o >> 32;
    *lo = o & 0xFFFFFFFF;
    BDBG_MSG(("%s: offsets: hi %u, lo %u", hi, lo));
    return 0;
}

int nav_indexer_create(
    char *mediaFileName,
    char *indexFileName,
    B_PlaybackIpPsiInfoHandle psi
    )
{
    int rc = -1;
    unsigned char *readBuf = NULL;
    FILE *fin = NULL, *fout = NULL;
    sTsIndexer *indexer = NULL;
    BNAV_Indexer_Handle bcmindexer = NULL;
    unsigned long readBytes;
    unsigned long numBytesToRead;
    BNAV_Indexer_Settings settings;
    tsindex_settings tsSettings;

    if (psi->mpegType != NEXUS_TransportType_eTs) {
        BDBG_ERR(("NAV index creation is not supported for this video codec %d container format %d", psi->videoCodec, psi->mpegType));
        return -1;
    }

    if (psi->videoPid == 0) {
        BDBG_WRN(("%s: creating index for video codec %d, container %d when video pid is 0", __FUNCTION__, psi->videoCodec, psi->mpegType));
    }

    if ((fin = fopen(mediaFileName, "rb" )) == NULL) {
        BDBG_ERR(("%s: Unable to open input file %s\n", __FUNCTION__, mediaFileName));
        goto error;
    }

    if ((fout = fopen(indexFileName, "wb" )) == NULL) {
        BDBG_ERR(("%s: Unable to open output file %s\n", __FUNCTION__, indexFileName));
        goto error;
    }
    numBytesToRead = psi->transportTimeStampEnabled ? (192*1024) : (188*1024);
    readBuf = BKNI_Malloc(numBytesToRead);
    if (!readBuf) {
        BDBG_ERR(("%s: memory allocation failure for %d bytes", __FUNCTION__, numBytesToRead));
        goto error;
    }
    BDBG_WRN(("Creating NAV index (%s) for video codec %d, pid %d, container format %d, TTS %d, frame rate %d", 
                indexFileName, psi->videoCodec, psi->videoPid, psi->mpegType, psi->transportTimeStampEnabled, (int)psi->videoFrameRate));

    BNAV_Indexer_GetDefaultSettings(&settings);
    settings.mpegSizeCallback = mpegSizeCallback;
    g_mpegFile = fin;
    settings.writeCallback = (INDEX_WRITE_CB)fwrite;
    settings.filePointer = (void *)fout;
    settings.transportTimestampEnabled = psi->transportTimeStampEnabled;
    settings.simulatedFrameRate = (int)psi->videoFrameRate;
    settings.sctVersion = BSCT_Version6wordEntry;
    if (psi->videoCodec == NEXUS_VideoCodec_eH264) {
        settings.videoFormat = BNAV_Indexer_VideoFormat_AVC;
        settings.navVersion = BNAV_Version_AVC;
    }
    else if (psi->mpegType == NEXUS_TransportType_eTs) {
        /* default settings are for MPEG2 TS */
        settings.videoFormat = BNAV_Indexer_VideoFormat_MPEG2;
    }
    BDBG_MSG(("bcmindex settings: navVersion %d, ts %d, fr %d, format %d, sct v %d nav v %d \n",
        settings.navVersion, settings.transportTimestampEnabled, settings.simulatedFrameRate, 
        settings.videoFormat, settings.sctVersion, settings.navVersion));
        
    if (BNAV_Indexer_Open(&bcmindexer, &settings)) {
        BDBG_ERR(("%s: BNAV_Indexer_Open failed for input file %s\n", __FUNCTION__, mediaFileName));
        goto error;
    }

    tsindex_settings_init(&tsSettings);
    tsSettings.pid = (unsigned short)psi->videoPid;
    if (psi->videoCodec == NEXUS_VideoCodec_eH264) {
        tsSettings.entry_size = 6;
        tsSettings.start_code_lo = 0x00;
        tsSettings.start_code_hi = 0xFF;
        tsSettings.is_avc = 1;
    }
    else {
        /* default settings are for MPEG2 TS */
        tsSettings.entry_size = 6;
    }
    tsSettings.cb = (INDEX_WRITE_CB)write_to_bcmindexer;
    tsSettings.fp = (void*)bcmindexer;
    tsSettings.ts_packet_size = psi->transportTimeStampEnabled ? 192 : 188;

    BDBG_MSG(("ts indexer settings: pid %d, entry sz %d, settings.is_avc %d, ts_packet_size %d",
        tsSettings.pid, tsSettings.entry_size, tsSettings.is_avc, tsSettings.ts_packet_size));
    indexer = tsindex_allocate_ex(&tsSettings);
    if (!indexer) {
        BDBG_ERR(("%s: tsindex_allocate_ex() failed for input file %s\n", __FUNCTION__, mediaFileName));
        goto error;
    }

    /*time1 = getms();*/
    while ((readBytes = fread(readBuf, 1, numBytesToRead, fin)) != 0) {
        tsindex_feed(indexer, readBuf, readBytes);
    }
    rc = 0;
    BDBG_WRN(("%s: Created NAV index (%s)", __FUNCTION__, indexFileName));

error:
    if (bcmindexer)
        BNAV_Indexer_Close(bcmindexer);
    if (indexer)
        tsindex_free(indexer);

    if (fin) fclose(fin);
    if (fout) fclose(fout);
    if (readBuf) BKNI_Free(readBuf);
    return rc;
}
