/***************************************************************************
 *     (c)2010-2012 Broadcom Corporation
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
 * $brcm_Workfile: nexus_file.c $
 * $brcm_Revision: 9 $
 * $brcm_Date: 8/28/12 3:24p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/file/src/nexus_file.c $
 * 
 * 9   8/28/12 3:24p erickson
 * SW7346-994: don't rely on bcmplayer BNAV_Version detection because it's
 *  not possible for file fifos with AVC
 * 
 * 8   5/3/12 6:09p vsilyaev
 * SW7425-2821: Added function to allow run-time control of directIo mode
 * 
 * 7   2/8/12 11:58a mphillip
 * SW7335-1295: BNAV_Player_Open closes itself on failure, so don't close
 *  it in error-handling
 * 
 * 6   4/7/11 6:13p vsilyaev
 * SW7425-232: Updated file mux module
 * 
 * 5   4/6/11 6:53p vsilyaev
 * SW7425-232: Removed MuxFileIo API to avoid naming clashes
 * 
 * 4   4/6/11 4:45p vsilyaev
 * SW7425-232: Added MuxFile interface
 * 
 * 3   2/25/11 12:47p erickson
 * SW7125-818: use BNAV_GetEntrySize
 *
 * 2   11/11/10 5:17p vsilyaev
 * SW7405-4995: Use BKNI_Memcmp to preinitialize structure used in
 *  GetDefaultSettings
 *
 * 1   9/22/10 6:57p vsilyaev
 * SW7405-4419: Added functions for file editing
 *
 *
 **************************************************************************/
#include "nexus_file_module.h"
#include "bcmplayer.h"
#include "bfile_util.h"
#include "nexus_file_muxio.h"
#include "nexus_file_posix.h"

#if defined(HUMAX_PLATFORM_BASE)
#include <stdio.h>
#include "bcmindexer_nav.h"
#endif

BDBG_MODULE(nexus_file);

#if defined(HUMAX_PLATFORM_BASE)
#define create64(HI,LO) ((((uint64_t)(HI))<<32)|(unsigned long)(LO))
#define getLo64(X) (unsigned long)((X)&0xFFFFFFFF)
#define getHi64(X) (unsigned long)((X)>>32)
#endif

typedef struct b_file_bp_info {
    unsigned index_entrysize;
    NEXUS_FilePlayHandle play_file;
} b_file_bp_info;

static long
bp_read(void *buffer, long size, long count, void *fp )
{
    bfile_io_read_t f = ((b_file_bp_info *)fp)->play_file->file.index;

    return (long)f->read(f, buffer, (size_t)(count*size));
}

static long
bp_tell( void *fp )
{
    bfile_io_read_t f = ((b_file_bp_info *)fp)->play_file->file.index;
    return (long)f->seek(f, 0, SEEK_CUR);
}

static int
bp_seek( void *fp, long offset, int origin )
{
    bfile_io_read_t f = ((b_file_bp_info *)fp)->play_file->file.index;
    off_t rc;
    rc = f->seek(f, (off_t)offset, origin);
    if ( rc == (off_t)-1) {
        return -1;
    }
    return 0;
}

static int
bp_bounds(BNAV_Player_Handle handle, void *fp, long *firstIndex, long *lastIndex)
{
    bfile_io_read_t f = ((b_file_bp_info *)fp)->play_file->file.index;
    off_t first, last;
    unsigned index_entrysize =((b_file_bp_info *)fp)->index_entrysize;

    f->bounds(f, &first, &last);

    BSTD_UNUSED(handle);
    *firstIndex = first/index_entrysize;
    *lastIndex = (last-1)/index_entrysize;

    return 0;
}

void
NEXUS_P_FilePlay_ConvertPosition(unsigned index_entrysize, const BNAV_Player_Position *nav_position, NEXUS_FilePosition *position)
{
   position->indexOffset = nav_position->index * index_entrysize;
   position->timestamp = nav_position->timestamp;
   position->mpegFileOffset = ((nav_position)->offsetLo|(((uint64_t)(nav_position)->offsetHi)<<32));
   return;
}

int
NEXUS_P_ReadFirstNavEntry(bfile_io_read_t f, BNAV_Entry *pNavEntry)
{
    off_t rc;

    rc = f->seek(f, 0, SEEK_SET);
    if ( rc == (off_t)-1) {
        return -1;
    }

    rc = f->read(f, pNavEntry, sizeof(*pNavEntry));
    if (rc != sizeof(*pNavEntry)) {
        return BERR_TRACE(-1);
    }
    
    return 0;
}

static NEXUS_Error
NEXUS_P_FilePlay_OpenBcmPlayer(BNAV_Player_Handle *bcm_player, b_file_bp_info *bp_info, NEXUS_FilePlayHandle file)
{
    NEXUS_Error rc;
    BNAV_Player_Settings cfg;
    BNAV_Entry navEntry;
    static const BNAV_DecoderFeatures features = {1,1,1,1};

    if(file->file.index==NULL) { rc = BERR_TRACE(NEXUS_NOT_SUPPORTED); goto err_args; }

    BNAV_Player_GetDefaultSettings(&cfg);
    rc = NEXUS_P_ReadFirstNavEntry(file->file.index, &navEntry);
    if (!rc) {
        cfg.navVersion = BNAV_get_version(&navEntry);
    }
    bp_info->index_entrysize = BNAV_GetEntrySize(cfg.navVersion);
    bp_info->play_file = file;
    cfg.videoPid = 0x1FFF; /* since BNAV_Player doesn't like 0 */
    cfg.filePointer = bp_info;
    cfg.decoderFeatures = features;
    cfg.readCb = bp_read;
    cfg.tellCb = bp_tell;
    cfg.seekCb   = bp_seek;
    cfg.boundsCb = bp_bounds;
    cfg.transportTimestampEnabled = false;
    cfg.firstIndex = -1; /* fifo file may have firstIndex > 0 */
    if (BNAV_Player_Open(bcm_player, &cfg)!=0) { rc=BERR_TRACE(NEXUS_NOT_SUPPORTED); goto err_player; }
    return NEXUS_SUCCESS;

err_player:
err_args:
    return rc;
}


NEXUS_Error
NEXUS_FilePlay_GetBounds(NEXUS_FilePlayHandle file, NEXUS_FilePosition *pFirst,  NEXUS_FilePosition *pLast)
{
    b_file_bp_info bp_info;
    int i_rc;
    NEXUS_Error rc;
    BNAV_Player_Handle bcm_player;
    long firstIndex, lastIndex;
    BNAV_Player_Position first, last;

    BDBG_ASSERT(file);
    BDBG_ASSERT(pFirst);
    BDBG_ASSERT(pLast);

    rc = NEXUS_P_FilePlay_OpenBcmPlayer(&bcm_player, &bp_info, file);
    if(rc!=NEXUS_SUCCESS) {goto err_player;}

    i_rc = bp_bounds(bcm_player, &bp_info, &firstIndex, &lastIndex);
    if(i_rc!=0) { rc=BERR_TRACE(NEXUS_NOT_SUPPORTED); goto err_bounds;}
    i_rc = BNAV_Player_GetPositionInformation(bcm_player, firstIndex, &first);
    if(i_rc!=0) { rc=BERR_TRACE(NEXUS_NOT_SUPPORTED); goto err_first;}
    i_rc = BNAV_Player_GetPositionInformation(bcm_player, lastIndex, &last);
    if(i_rc!=0) { rc=BERR_TRACE(NEXUS_NOT_SUPPORTED); goto err_last;}
    NEXUS_P_FilePlay_ConvertPosition(bp_info.index_entrysize, &first, pFirst);
    NEXUS_P_FilePlay_ConvertPosition(bp_info.index_entrysize, &last, pLast);
    BNAV_Player_Close(bcm_player);
    return NEXUS_SUCCESS;

err_last:
err_first:
err_bounds:
    BNAV_Player_Close(bcm_player);
err_player:
    return rc;
}

NEXUS_Error
NEXUS_FilePlay_GetLocation( NEXUS_FilePlayHandle file, unsigned long timestamp, NEXUS_FilePosition *pPosition )
{
    b_file_bp_info bp_info;
    int i_rc;
    NEXUS_Error rc;
    BNAV_Player_Handle bcm_player;
    long i_frame_index;
    BNAV_Player_Position i_frame_position;
    long index;

    rc = NEXUS_P_FilePlay_OpenBcmPlayer(&bcm_player, &bp_info, file);
    if(rc!=NEXUS_SUCCESS) {goto err_player;}

    index = BNAV_Player_FindIndexFromTimestamp(bcm_player, timestamp);
    if(index==-1)  {rc=BERR_TRACE(NEXUS_NOT_SUPPORTED);goto err_find_index;}

    i_frame_index = BNAV_Player_FindIFrameFromIndex(bcm_player, index, eBpForward);
    if (i_frame_index == -1) { rc=BERR_TRACE(NEXUS_NOT_SUPPORTED);goto err_find_iframe;}

    i_rc = BNAV_Player_GetPositionInformation(bcm_player, i_frame_index, &i_frame_position);
    if(i_rc!=0) { rc=BERR_TRACE(NEXUS_NOT_SUPPORTED); goto err_position_iframe;}

    NEXUS_P_FilePlay_ConvertPosition(bp_info.index_entrysize, &i_frame_position, pPosition);
    BNAV_Player_Close(bcm_player);
    return NEXUS_SUCCESS;

err_position_iframe:
err_find_iframe:
err_find_index:
    BNAV_Player_Close(bcm_player);
err_player:
    return rc;
}

void
NEXUS_FilePlayOffset_GetDefaultSettings(NEXUS_FilePlayOffset_Settings *settings)
{
    BKNI_Memset(settings, 0, sizeof(*settings));
    settings->dataOffset = 0;
    settings->indexOffset = 0;
    return;
}

typedef struct NEXUS_FilePlayOffset {
    struct NEXUS_FilePlay  parent; /* must be first */
    NEXUS_FilePlayHandle  original;
} NEXUS_FilePlayOffset;

static void
NEXUS_P_FilePlayOffset_Close(struct NEXUS_FilePlay *file_)
{
    NEXUS_FilePlayOffset *file = (NEXUS_FilePlayOffset *)file_;
    bfile_read_offset_detach(file->parent.file.data);
    if(file->parent.file.index) {
        bfile_read_offset_detach(file->parent.file.index);
    }
    BKNI_Free(file);
    return;
}

NEXUS_FilePlayHandle
NEXUS_FilePlayOffset_Open( NEXUS_FilePlayHandle file, const NEXUS_FilePlayOffset_Settings *settings)
{
    NEXUS_FilePlayOffset *fileOffset;

    BDBG_ASSERT(file);
    BDBG_ASSERT(settings);
    BDBG_ASSERT(file->file.data);
    fileOffset = BKNI_Malloc(sizeof(*fileOffset));
    if(fileOffset==NULL) {(void)BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);goto err_alloc;}
    fileOffset->parent.file.index = NULL;
    fileOffset->parent.file.data = bfile_read_offset_attach(file->file.data, -settings->dataOffset);
    if(fileOffset->parent.file.data==NULL) {(void)BERR_TRACE(NEXUS_NOT_SUPPORTED);goto err_data;}
    if(file->file.index) {
        fileOffset->parent.file.index = bfile_read_offset_attach(file->file.index, -settings->indexOffset);
        if(fileOffset->parent.file.index==NULL) {(void)BERR_TRACE(NEXUS_NOT_SUPPORTED);goto err_index;}
    }
    fileOffset->parent.file.close = NEXUS_P_FilePlayOffset_Close;
    fileOffset->original = file;
    return &fileOffset->parent;

err_index:
    bfile_read_offset_detach(file->file.data);
err_data:
    BKNI_Free(fileOffset);
err_alloc:
    return NULL;
}

void NEXUS_MuxFile_Close(NEXUS_MuxFileHandle file)
{
    file->close(&file->mux);
    return;
}

NEXUS_MuxFileHandle NEXUS_MuxFile_OpenPosix( const char *fileName)
{
    struct bfile_io_mux_posix *file;
    NEXUS_Error rc;

    file = BKNI_Malloc(sizeof(*file));
    if(!file) {rc=BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);goto err_alloc;}

    rc = b_file_io_mux_posix_open(file, fileName, 0, false); /* no DIRECT_IO */
    if(rc!=NEXUS_SUCCESS) {rc=BERR_TRACE(rc);goto err_open;}

    return &file->self;

err_open:
    BKNI_Free(file);
err_alloc:
    return NULL;
}

void NEXUS_FileRecord_GetDefaultOpenSettings(NEXUS_FileRecordOpenSettings *pSettings)
{
    BDBG_ASSERT(pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->data.directIo = true;
    return;
}

void NEXUS_FilePlay_GetDefaultOpenSettings(NEXUS_FilePlayOpenSettings *pSettings)
{
    BDBG_ASSERT(pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->data.directIo = true;
    return;
}

#if defined(HUMAX_PLATFORM_BASE)
NEXUS_Error
NEXUS_FilePlay_ConvertFileOffsetFromIndexFileToTime(
	const char *pszIndexFilePath, 
	unsigned long ulFileOffsetHi, /* [out] */
	unsigned long ulFileOffsetLow, /* [out] */
	unsigned long *pulResultTimeMs,  /* [out] */
	unsigned long ulIsTimestampEnabled)
{
	BNAV_Player_Handle pstBcmPlyaerHandle;
	BNAV_Player_Settings stBcmPlayerSettings;
	BNAV_Player_Position tPlayerPosition;
	BNAV_Entry stEntryData;
	BNAV_Version eNavVersion;
	unsigned long ulPacketSize;
	long lIndex, lIFrameIndex;
	FILE *fpIndexFile = NULL;
	int nResult;

	if((pszIndexFilePath == NULL) || (pulResultTimeMs == NULL))
	{
		BDBG_ERR(("%s : Invalid parameter : pszIndexFilePath=%p, pulResultTimeMs=%p",
			__FUNCTION__, pszIndexFilePath, pulResultTimeMs));
		return NEXUS_INVALID_PARAMETER;
	}

	fpIndexFile = fopen(pszIndexFilePath, "rw");

	/* get nav version */
	nResult = fread((void*)&stEntryData, sizeof(stEntryData), 1, fpIndexFile);
	if(nResult < 0)
	{	
		BDBG_ERR(("%s : index file read fail (ret=%d)", __FUNCTION__, nResult));
		fclose(fpIndexFile);
		return NEXUS_UNKNOWN;
	}

	eNavVersion = (BNAV_Version)BNAV_get_version(&stEntryData);

	/* open nav player */
	BNAV_Player_GetDefaultSettings(&stBcmPlayerSettings);
	stBcmPlayerSettings.readCb = (BP_READ_CB)fread;
	stBcmPlayerSettings.seekCb = (BP_SEEK_CB)fseek;
	stBcmPlayerSettings.tellCb = (BP_TELL_CB)ftell;
	stBcmPlayerSettings.filePointer = fpIndexFile;
	stBcmPlayerSettings.videoPid = 1; /* doesn't matter */
	stBcmPlayerSettings.firstIndex = -1; /* initial value */
	stBcmPlayerSettings.lastIndex = -1; /* initial value */
	stBcmPlayerSettings.navVersion = eNavVersion;

	if(ulIsTimestampEnabled == true)
	{
		stBcmPlayerSettings.transportTimestampEnabled = true;
		ulPacketSize = 192;
	}
	else
	{
		stBcmPlayerSettings.transportTimestampEnabled = false;
		ulPacketSize = 188;
	}

	nResult = BNAV_Player_Open(&pstBcmPlyaerHandle, &stBcmPlayerSettings);
	if(nResult < 0)
	{	
		BDBG_ERR(("%s : BNAV_Player_Open fail (ret=%d)", __FUNCTION__, nResult));
		fclose(fpIndexFile);
		return NEXUS_UNKNOWN;
	}

	lIndex = BNAV_Player_FindIndexFromOffset(pstBcmPlyaerHandle, ulFileOffsetHi, ulFileOffsetLow);
	if(lIndex < 0)
	{
		BDBG_ERR(("%s : BNAV_Player_FindIndexFromOffset is fail (lIndex=%d)", __FUNCTION__, lIndex));
		BNAV_Player_Close(pstBcmPlyaerHandle);
		fclose(fpIndexFile);
		return NEXUS_UNKNOWN;
	}

	/* Search for the nearest random access point (i.e. I frame) in the opposite direction.
	This is not required for the decoder; it will simply discard data until it gets to a random access point.
	It's required for the user experience; the user doesn't want to lose content on a trick mode transition because decoding
	must start at a random access point. */

	lIFrameIndex = BNAV_Player_FindIFrameFromIndex(pstBcmPlyaerHandle, lIndex, eBpReverse);

	if(lIFrameIndex == -1) 
	{
		lIFrameIndex = BNAV_Player_FindIFrameFromIndex(pstBcmPlyaerHandle, lIndex, eBpForward);
	}
	
	if(lIFrameIndex != -1) 
	{
		lIndex = lIFrameIndex;
	}

	nResult = BNAV_Player_GetPositionInformation(pstBcmPlyaerHandle, lIndex, &tPlayerPosition);
	if(nResult < 0)
	{
		BDBG_ERR(("%s : GetPositionInformation fail (nResult=%d)", __FUNCTION__, nResult));
		BNAV_Player_Close(pstBcmPlyaerHandle);
		fclose(fpIndexFile);
		return NEXUS_UNKNOWN;
	}

	*pulResultTimeMs = tPlayerPosition.ulTimestamp;

	BNAV_Player_Close(pstBcmPlyaerHandle);
	fclose(fpIndexFile);

	return NEXUS_SUCCESS;


}

NEXUS_Error
NEXUS_FilePlay_ConvertTimeToFileOffsetFromIndexFile(
	const char *pszIndexFilePath, 
	unsigned long ulRequestTimeMs,
	unsigned long *pulResultFileOffsetHi, /* [out] */
	unsigned long *pulResultFileOffsetLow, /* [out] */
	unsigned long *pulResultTimeMs,  /* [out] */
	unsigned long ulIsTimestampEnabled)
{
	BNAV_Player_Handle pstBcmPlyaerHandle;
	BNAV_Player_Settings stBcmPlayerSettings;
	BNAV_Player_Position tPlayerPosition;
	BNAV_Entry stEntryData;
	BNAV_Version eNavVersion;
	unsigned long ulPacketSize;
	long lIndex, lIFrameIndex;
	uint64_t ullFileOffset;
	FILE *fpIndexFile = NULL;
	int nResult;

	if((pszIndexFilePath == NULL) || (pulResultFileOffsetHi == NULL) || (pulResultFileOffsetLow == NULL) || (pulResultTimeMs == NULL))
	{
		BDBG_ERR(("%s : Invalid parameter : pszIndexFilePath=%p, pulResultFileOffsetHi=%p, pulResultFileOffsetLow=%p, pulResultTimeMs=%p",
			__FUNCTION__, pszIndexFilePath, pulResultFileOffsetHi, pulResultFileOffsetLow, pulResultTimeMs));
		return NEXUS_INVALID_PARAMETER;
	}
	
	/* index file open */
	fpIndexFile = fopen(pszIndexFilePath, "rb");
	if(fpIndexFile == NULL)
	{
		BDBG_ERR(("%s : %s file open fail", __FUNCTION__, pszIndexFilePath));
		return NEXUS_UNKNOWN;
	}

	/* get nav version */
	nResult = fread((void*)&stEntryData, sizeof(stEntryData), 1, fpIndexFile);
	if(nResult < 0)
	{	
		BDBG_ERR(("%s : index file read fail (ret=%d)", __FUNCTION__, nResult));
		fclose(fpIndexFile);
		return NEXUS_UNKNOWN;
	}

	eNavVersion = (BNAV_Version)BNAV_get_version(&stEntryData);

	/* open nav player */
	BNAV_Player_GetDefaultSettings(&stBcmPlayerSettings);
	stBcmPlayerSettings.readCb = (BP_READ_CB)fread;
	stBcmPlayerSettings.seekCb = (BP_SEEK_CB)fseek;
	stBcmPlayerSettings.tellCb = (BP_TELL_CB)ftell;
	stBcmPlayerSettings.filePointer = fpIndexFile;
	stBcmPlayerSettings.videoPid = 1; /* doesn't matter */
	stBcmPlayerSettings.firstIndex = -1; /* initial value */
	stBcmPlayerSettings.lastIndex = -1; /* initial value */
	stBcmPlayerSettings.navVersion = eNavVersion;

	if(ulIsTimestampEnabled == true)
	{
		stBcmPlayerSettings.transportTimestampEnabled = true;
		ulPacketSize = 192;
	}
	else
	{
		stBcmPlayerSettings.transportTimestampEnabled = false;
		ulPacketSize = 188;
	}

	nResult = BNAV_Player_Open(&pstBcmPlyaerHandle, &stBcmPlayerSettings);
	if(nResult < 0)
	{	
		BDBG_ERR(("%s : BNAV_Player_Open fail (ret=%d)", __FUNCTION__, nResult));
		fclose(fpIndexFile);
		return NEXUS_UNKNOWN;
	}

	lIndex = BNAV_Player_FindIndexFromTimestamp(pstBcmPlyaerHandle, ulRequestTimeMs);
	if(lIndex < 0)
	{
		BDBG_ERR(("%s : FindIndexFromTimestamp is fail (lIndex=%d)", __FUNCTION__, lIndex));
		BNAV_Player_Close(pstBcmPlyaerHandle);
		fclose(fpIndexFile);
		return NEXUS_UNKNOWN;
	}

	/* Search for the nearest random access point (i.e. I frame) in the opposite direction.
	This is not required for the decoder; it will simply discard data until it gets to a random access point.
	It's required for the user experience; the user doesn't want to lose content on a trick mode transition because decoding
	must start at a random access point. */

	lIFrameIndex = BNAV_Player_FindIFrameFromIndex(pstBcmPlyaerHandle, lIndex, eBpReverse);

	if(lIFrameIndex == -1) 
	{
		lIFrameIndex = BNAV_Player_FindIFrameFromIndex(pstBcmPlyaerHandle, lIndex, eBpForward);
	}
	
	if(lIFrameIndex != -1) 
	{
		lIndex = lIFrameIndex;
	}

	nResult = BNAV_Player_GetPositionInformation(pstBcmPlyaerHandle, lIndex, &tPlayerPosition);
	if(nResult < 0)
	{
		BDBG_ERR(("%s : GetPositionInformation fail (nResult=%d)", __FUNCTION__, nResult));
		BNAV_Player_Close(pstBcmPlyaerHandle);
		fclose(fpIndexFile);
		return NEXUS_UNKNOWN;
	}

	ullFileOffset = create64(tPlayerPosition.offsetHi, tPlayerPosition.offsetLo);
	/* Back up to the TS-packet aligned sequence header. */
	ullFileOffset = ullFileOffset - (uint64_t)(tPlayerPosition.ulSeqHdrStartOffset);
	/* we need to maintain data file offset aligned by packet size (188 or 192) */
	ullFileOffset = ullFileOffset - (ullFileOffset % ulPacketSize);

	*pulResultFileOffsetHi = getHi64(ullFileOffset);
	*pulResultFileOffsetLow = getLo64(ullFileOffset);
	*pulResultTimeMs = tPlayerPosition.ulTimestamp;

	BNAV_Player_Close(pstBcmPlyaerHandle);
	fclose(fpIndexFile);

	return NEXUS_SUCCESS;
}


NEXUS_Error
NEXUS_FilePlay_GetDurationFromIndexFile(
	const char *pszIndexFilePath, 
	unsigned long *pulDurationMs /* [out] */)
{
	BNAV_Player_Handle pstBcmPlyaerHandle;
	BNAV_Player_Settings stBcmPlayerSettings;
	BNAV_Player_Position tPlayerPosition;
	BNAV_Entry stEntryData;
	BNAV_Version eNavVersion;
	long lFirstIndex, lLastIndex;
	unsigned long ulDurationMs = 0;
	unsigned long ulFirstTimestamp, ulLastTimestamp;
	FILE *fpIndexFile = NULL;
	int nResult;

	if((pszIndexFilePath == NULL) || (pulDurationMs == NULL) )
	{
		BDBG_ERR(("%s : Invalid parameter : pszIndexFilePath=%p, pulDurationMs=%p", 
									__FUNCTION__, pszIndexFilePath, pulDurationMs));
		return NEXUS_INVALID_PARAMETER;
	}
	
	/* index file open */
	fpIndexFile = fopen(pszIndexFilePath, "rb");
	if(fpIndexFile == NULL)
	{
		BDBG_ERR(("%s : %s file open fail", __FUNCTION__, pszIndexFilePath));
		return NEXUS_UNKNOWN;
	}

	/* get nav version */
	nResult = fread((void*)&stEntryData, sizeof(stEntryData), 1, fpIndexFile);
	if(nResult < 0)
	{	
		BDBG_ERR(("%s : index file read fail (ret=%d)", __FUNCTION__, nResult));
		fclose(fpIndexFile);
		return NEXUS_UNKNOWN;
	}

	eNavVersion = (BNAV_Version)BNAV_get_version(&stEntryData);

	/* open nav player */
	BNAV_Player_GetDefaultSettings(&stBcmPlayerSettings);
	stBcmPlayerSettings.readCb = (BP_READ_CB)fread;
	stBcmPlayerSettings.seekCb = (BP_SEEK_CB)fseek;
	stBcmPlayerSettings.tellCb = (BP_TELL_CB)ftell;
	stBcmPlayerSettings.filePointer = fpIndexFile;
	stBcmPlayerSettings.videoPid = 1; /* doesn't matter */
	stBcmPlayerSettings.firstIndex = -1; /* initial value */
	stBcmPlayerSettings.lastIndex = -1; /* initial value */
	stBcmPlayerSettings.navVersion = eNavVersion;

	nResult = BNAV_Player_Open(&pstBcmPlyaerHandle, &stBcmPlayerSettings);
	if(nResult < 0)
	{	
		BDBG_ERR(("%s : BNAV_Player_Open fail (ret=%d)", __FUNCTION__, nResult));
		fclose(fpIndexFile);
		return NEXUS_UNKNOWN;
	}

	nResult = BNAV_Player_DefaultGetBounds(pstBcmPlyaerHandle, fpIndexFile, &lFirstIndex, &lLastIndex);
	if(nResult < 0)
	{
		BDBG_ERR(("%s : DefaultGetBounds fail (nResult=%d)", __FUNCTION__, nResult));
		BNAV_Player_Close(pstBcmPlyaerHandle);
		fclose(fpIndexFile);
		return NEXUS_UNKNOWN;
	}

	/* get time stamp value of first index */
	nResult = BNAV_Player_GetPositionInformation(pstBcmPlyaerHandle, lFirstIndex, &tPlayerPosition);
	if(nResult < 0)
	{
		BDBG_ERR(("%s : GetPositionInformation fail (nResult=%d)", __FUNCTION__, nResult));
		BNAV_Player_Close(pstBcmPlyaerHandle);
		fclose(fpIndexFile);
		return NEXUS_UNKNOWN;
	}

	ulFirstTimestamp = tPlayerPosition.ulTimestamp;

	/* get time stamp value of last index */
	nResult = BNAV_Player_GetPositionInformation(pstBcmPlyaerHandle, lLastIndex, &tPlayerPosition);
	if(nResult < 0)
	{
		BDBG_ERR(("%s : GetPositionInformation fail (nResult=%d)", __FUNCTION__, nResult));
		BNAV_Player_Close(pstBcmPlyaerHandle);
		fclose(fpIndexFile);
		return NEXUS_UNKNOWN;
	}

	ulLastTimestamp = tPlayerPosition.ulTimestamp;

	if(ulLastTimestamp > ulFirstTimestamp)
	{
		ulDurationMs = ulLastTimestamp - ulFirstTimestamp;
	}
	else
	{
		BDBG_ERR(("%s : Invalid timestamp value (ulFirstTimestamp=%d, ulLastTimestamp=%d)", __FUNCTION__, ulFirstTimestamp, ulLastTimestamp));
		BNAV_Player_Close(pstBcmPlyaerHandle);
		fclose(fpIndexFile);
		return NEXUS_UNKNOWN;
	}

	*pulDurationMs = ulDurationMs;
	
	BNAV_Player_Close(pstBcmPlyaerHandle);
	fclose(fpIndexFile);

	return NEXUS_SUCCESS;
}

NEXUS_Error
NEXUS_File_GetIndexEntrySize(void *pucEntryBuffer, unsigned long *ulEntrySize)
{
	BNAV_Version eNavVersion;
	unsigned long	ulRetSize = 0;
	uint64_t	ullFileOffset = 0;	

	eNavVersion = (BNAV_Version)BNAV_get_version((BNAV_Entry *)pucEntryBuffer);
	if(eNavVersion == BNAV_Version_AVC)
	{
		*ulEntrySize = sizeof(BNAV_AVC_Entry);
	}
	else
	{
		*ulEntrySize = sizeof(BNAV_Entry);
	}

	return NEXUS_SUCCESS;
}

NEXUS_Error
NEXUS_File_FindIndexOffset(void *pucEntryBuffer, uint64_t ullTsOffset, unsigned long ulIsTimestampEnabled)
{
	unsigned long ulFrameHiOffset;
	unsigned long ulFrameLowOffset;
	unsigned long ulSeqHdrStartOffset;
	uint64_t ullFrameOffset;
	unsigned long ulPacketSize;
	
	if(ulIsTimestampEnabled == true)
	{
		ulPacketSize = 192;
	}
	else
	{
		ulPacketSize = 188;
	}

	ulSeqHdrStartOffset = BNAV_get_seqHdrStartOffset((BNAV_Entry *)pucEntryBuffer);
	ulFrameHiOffset = BNAV_get_frameOffsetHi((BNAV_Entry *)pucEntryBuffer);
	ulFrameLowOffset = BNAV_get_frameOffsetLo((BNAV_Entry *)pucEntryBuffer);
	ullFrameOffset = create64(ulFrameHiOffset,ulFrameLowOffset);
	ullFrameOffset = (ullFrameOffset - (uint64_t)ulSeqHdrStartOffset);
	ullFrameOffset = (ullFrameOffset - (ullFrameOffset % ulPacketSize));
	
	if(ullTsOffset == ullFrameOffset)
	{
		return NEXUS_SUCCESS;
	}
	
	return NEXUS_UNKNOWN;
}

void 
NEXUS_File_GetFrameOffset(void *pucEntryBuffer, uint64_t *ullFrameOffset)
{
	unsigned long ulFrameHiOffset;
	unsigned long ulFrameLowOffset;

	ulFrameHiOffset = BNAV_get_frameOffsetHi((BNAV_Entry *)pucEntryBuffer);
	ulFrameLowOffset = BNAV_get_frameOffsetLo((BNAV_Entry *)pucEntryBuffer);	
	*ullFrameOffset = create64(ulFrameHiOffset, ulFrameLowOffset);
}

void
NEXUS_File_SetFrameOffset(void *pucEntryBuffer, uint64_t ullFrameOffset)
{
	BNAV_set_frameOffsetLo((BNAV_Entry *)pucEntryBuffer, ullFrameOffset & 0xFFFFFFFF);
	BNAV_set_frameOffsetHi((BNAV_Entry *)pucEntryBuffer, ullFrameOffset >> 32);
}

void 
NEXUS_File_GetTimestamp(void *pucEntryBuffer, unsigned long *ulTimestamp)
{
	*ulTimestamp = BNAV_get_timestamp((BNAV_Entry *)pucEntryBuffer);
}

void 
NEXUS_File_SetTimestamp(void *pucEntryBuffer, unsigned long ulTimestamp)
{
	BNAV_set_timestamp((BNAV_Entry *)pucEntryBuffer, ulTimestamp);
}
/*#endif*/
#else
NEXUS_Error
NEXUS_FilePlay_ConvertFileOffsetFromIndexFileToTime(
	const char *pszIndexFilePath, 
	unsigned long ulFileOffsetHi, /* [out] */
	unsigned long ulFileOffsetLow, /* [out] */
	unsigned long *pulResultTimeMs,  /* [out] */
	unsigned long ulIsTimestampEnabled)
{
	BSTD_UNUSED(pszIndexFilePath);
	BSTD_UNUSED(ulFileOffsetHi);
	BSTD_UNUSED(ulFileOffsetLow);
	BSTD_UNUSED(pulResultTimeMs);
	BSTD_UNUSED(ulIsTimestampEnabled);
	return NEXUS_SUCCESS;
}

NEXUS_Error
NEXUS_FilePlay_ConvertTimeToFileOffsetFromIndexFile(
	const char *pszIndexFilePath, 
	unsigned long ulRequestTimeMs,
	unsigned long *pulResultFileOffsetHi, /* [out] */
	unsigned long *pulResultFileOffsetLow, /* [out] */
	unsigned long *pulResultTimeMs,  /* [out] */
	unsigned long ulIsTimestampEnabled)
{
	BSTD_UNUSED(pszIndexFilePath);
	BSTD_UNUSED(ulRequestTimeMs);
	BSTD_UNUSED(pulResultFileOffsetHi);
	BSTD_UNUSED(pulResultFileOffsetLow);
	BSTD_UNUSED(pulResultTimeMs);
	BSTD_UNUSED(ulIsTimestampEnabled);
	return NEXUS_SUCCESS;
}


NEXUS_Error
NEXUS_FilePlay_GetDurationFromIndexFile(
	const char *pszIndexFilePath, 
	unsigned long *pulDurationMs /* [out] */)
{
	BSTD_UNUSED(pszIndexFilePath);
	BSTD_UNUSED(pulDurationMs);
	return NEXUS_SUCCESS;
}

NEXUS_Error 
NEXUS_File_GetIndexEntrySize(void *pucEntryBuffer, unsigned long *ulEntrySize)
{
	BSTD_UNUSED(pucEntryBuffer);
	BSTD_UNUSED(ulEntrySize);
	return NEXUS_SUCCESS;
}

NEXUS_Error 
NEXUS_File_FindIndexOffset(void *pucEntryBuffer, uint64_t ullTsOffset, unsigned long ulIsTimestampEnabled)
{
	BSTD_UNUSED(pucEntryBuffer);
	BSTD_UNUSED(ullTsOffset);
	BSTD_UNUSED(ulIsTimestampEnabled);
	return NEXUS_SUCCESS;
}

void 
NEXUS_File_GetFrameOffset(void *pucEntryBuffer, uint64_t *ullFrameOffset)
{
	BSTD_UNUSED(pucEntryBuffer);
	BSTD_UNUSED(ullFrameOffset);
}

void
NEXUS_File_SetFrameOffset(void *pucEntryBuffer, uint64_t ullFrameOffset)
{
	BSTD_UNUSED(pucEntryBuffer);
	BSTD_UNUSED(ullFrameOffset);	
}

void 
NEXUS_File_GetTimestamp(void *pucEntryBuffer, unsigned long *ulTimestamp)
{
	BSTD_UNUSED(pucEntryBuffer);
	BSTD_UNUSED(ulTimestamp);	
}

void 
NEXUS_File_SetTimestamp(void *pucEntryBuffer, unsigned long ulTimestamp)
{
	BSTD_UNUSED(pucEntryBuffer);
	BSTD_UNUSED(ulTimestamp);	
}
/*#endif*/
#endif

