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
 * $brcm_Workfile: nexus_file_mux.c $
 * $brcm_Revision: 26 $
 * $brcm_Date: 9/18/12 3:30p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/file_mux/src/nexus_file_mux.c $
 * 
 * 26   9/18/12 3:30p hongtaoz
 * SW7425-3425: always clear fnrt context at stop; check number of PID
 *  channels for muxlib startSettings to prevent array overrun;
 * 
 * 25   8/31/12 1:46p hongtaoz
 * SW7425-3425: merged FNRT to mainline;
 * 
 * FNRT_Demo/7   8/13/12 12:04p hongtaoz
 * SW7425-3684: added mux media duration status API;
 * 
 * FNRT_Demo/6   7/27/12 5:02p hongtaoz
 * SW7425-3425: removed dynamic Add/Remove API for now;
 * 
 * FNRT_Demo/5   7/26/12 11:39p hongtaoz
 * SW7425-3425: fixed more typo;
 * 
 * FNRT_Demo/4   7/26/12 7:33p hongtaoz
 * SW7425-3425: fixed typo;
 * 
 * FNRT_Demo/3   7/25/12 8:06p hongtaoz
 * SW7425-3425, SW7425-3362: map FNRT inputs to output by track number;
 *  removed FNRT enable setting;
 * 
 * FNRT_Demo/2   7/25/12 1:44p nilesh
 * SW7425-3362: Updated FNRTlib API to support multiple groups per input
 *  type
 * 
 * FNRT_Demo/1   7/6/12 5:40p hongtaoz
 * SW7425-3425: add FNRT support to file mux module;
 * 
 * 24   8/16/12 3:28p vsilyaev
 * SW7425-3684: Added duration of muxed portion of stream/file
 * 
 * 23   7/5/12 2:28p vsilyaev
 * SW7425-3393: Fixed typo
 * 
 * 22   4/16/12 1:28p vsilyaev
 * SW7435-107: Resolved false positive
 * 
 * 21   1/31/12 1:26p vsilyaev
 * SW7425-2301: Added option to coltrol size of mp4 relocation buffer
 * 
 * 20   12/14/11 4:52p jgarrett
 * SW7425-1950: Consolidating audio codec conversion routines into core
 *  module
 * 
 * 19   12/13/11 11:36a vsilyaev
 * SW7425-1950:Undo previous commit
 * 
 * 17   12/12/11 3:50p hongtaoz
 * SW7425-1950: incorrect audio codec to magnum conversion;
 * 
 * 16   11/30/11 1:37p vsilyaev
 * SW7425-1790: Added support for S/G file mux I/O
 * 
 * SW7425-324/4   11/29/11 5:07p nilesh
 * SW7425-1790: Fixed logic getting next BMUXlib_StorageDescriptor
 * 
 * SW7425-324/3   11/29/11 1:52p nilesh
 * SW7425-324: Fixed typo using wrong loop variable
 * 
 * SW7425-324/2   11/28/11 3:45p nilesh
 * SW7425-324: Updated to compile with latest BMUXlib Storage API
 * 
 * SW7425-324/1   11/21/11 1:01p vsilyaev
 * SW7425-1790: Added support for S/G file mux I/O
 * 
 * 15   9/2/11 3:02p vsilyaev
 * SW7425-713: User proper way to test for the depth of I/O FIFO's
 * 
 * 14   6/17/11 11:00a vsilyaev
 * SW7425-713: Fixed typo
 * 
 * 13   6/17/11 10:57a vsilyaev
 * SW7425-713: Moved I/O wake-up into a better spot
 * 
 * 12   6/15/11 2:36p vsilyaev
 * SW7425-713: Added code to handle mux blocking on I/O resources
 * 
 * 11   6/8/11 12:03p vsilyaev
 * SW7425-447: Conditionally compile ASF support
 * 
 * 10   5/23/11 1:50p vsilyaev
 * SW7425-447: Merged fixes for ASF Support
 * 
 * SW7425-447/2   5/23/11 4:33p nilesh
 * SW7425-447: Reverted NEXUS_FileMux_GetDefaultCreateSettings() prototype
 * 
 * SW7425-447/1   5/18/11 3:16p nilesh
 * SW7425-447: Fixed ASF Support
 * 
 * 9   5/16/11 8:11p vsilyaev
 * SW7425-482: Added support for the ASF file MUX
 * 
 * 8   4/19/11 5:04p jgarrett
 * SW7425-288: Merge to main branch
 * 
 * SW7425-288/3   4/19/11 2:31a nilesh
 * SW7425-288: Propagate audio metdata buffer address
 * 
 * SW7425-288/2   4/19/11 1:47a nilesh
 * SW7425-288: Set audio codec
 * 
 * SW7425-288/1   4/19/11 1:18a nilesh
 * SW7425-288: Fixed audio encoder setting for MP4 mux
 * 
 * 7   4/17/11 1:46p hongtaoz
 * SW7425-232: use BDBG_FILE_MODULE to demux debug messages in file mux
 *  module;
 * 
 * 6   4/15/11 7:42p vsilyaev
 * SW7425-232: Added video codec type
 * 
 * 5   4/14/11 11:14a vsilyaev
 * SW7425-75: Propagate pMetadataBufferBaseAddress through from nexus
 *  video encoder to the muxes
 * 
 * 4   4/8/11 1:04p vsilyaev
 * SW7425-232: Updated to the latest mux code
 * 
 * 3   4/7/11 6:13p vsilyaev
 * SW7425-232: Updated file mux module
 * 
 * 2   4/7/11 3:23p vsilyaev
 * SW7425-232: Updated MP4 mux support
 * 
 *
 **************************************************************************/
#include "nexus_file_mux_module.h"
#include "nexus_video_encoder_output.h"
#include "nexus_file_muxio.h"

BDBG_MODULE(nexus_file_mux);
BDBG_FILE_MODULE(nexus_file_mux_io);
BDBG_FILE_MODULE(nexus_file_mux_video);
BDBG_FILE_MODULE(nexus_file_mux_audio);
BDBG_FILE_MODULE(nexus_file_mux_timer);

BDBG_OBJECT_ID(NEXUS_FileMux);

#define BDBG_MSG_TRACE(x)   /* BDBG_MSG(x) */
#define BDBG_MSG_IO(x)      /* BDBG_MSG(x) */
#define NEXUS_P_FILEMUX_QUEUE_THRESHOLD (NEXUS_P_FILEMUX_QUEUE_SIZE/4)

NEXUS_FileMux_P_State g_NEXUS_FileMux_P_State;
static void NEXUS_FileMux_P_DoMux(NEXUS_FileMuxHandle mux);

void 
NEXUS_FileMuxModule_GetDefaultSettings( NEXUS_FileMuxModuleSettings *pSettings)
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    return;
}

NEXUS_ModuleHandle 
NEXUS_FileMuxModule_Init( const NEXUS_FileMuxModuleSettings *pSettings)
{
    NEXUS_Error rc;
    NEXUS_ModuleSettings moduleSettings;

    BSTD_UNUSED(pSettings);

    BDBG_ASSERT(g_NEXUS_FileMux_P_State.module==NULL);
    BKNI_Memset(&g_NEXUS_FileMux_P_State, 0, sizeof(g_NEXUS_FileMux_P_State));
    NEXUS_Module_GetDefaultSettings(&moduleSettings);
    moduleSettings.priority = NEXUS_ModulePriority_eHigh;
    g_NEXUS_FileMux_P_State.module = NEXUS_Module_Create("file_mux", &moduleSettings);
    if(g_NEXUS_FileMux_P_State.module == NULL) { rc = BERR_TRACE(BERR_OS_ERROR); goto error; }

    return g_NEXUS_FileMux_P_State.module;

error:
    return NULL;
}

void 
NEXUS_FileMuxModule_Uninit(void)
{
    if(g_NEXUS_FileMux_P_State.module==NULL) {return;}

    NEXUS_Module_Destroy(g_NEXUS_FileMux_P_State.module);
    g_NEXUS_FileMux_P_State.module = NULL;
    return;
}

static void NEXUS_P_FileMux_IoComplete(void *cntx, ssize_t size);

BDBG_OBJECT_ID(NEXUS_P_FileMux_File);

static void NEXUS_P_FileMux_File_Attach(NEXUS_P_FileMux_File *muxFile, NEXUS_FileMuxHandle mux, NEXUS_MuxFileHandle file)
{
    BDBG_OBJECT_INIT(muxFile, NEXUS_P_FileMux_File);
    muxFile->mux = mux;
    muxFile->file = file;
    muxFile->completed = 0;
    muxFile->lastResult = 0;
    muxFile->ioError = false;
    muxFile->ioInProgress = false;
    muxFile->ioCanceled = false;
    BFIFO_INIT(&muxFile->fifo, muxFile->descriptors, NEXUS_P_FILEMUX_QUEUE_SIZE);
    return;
}

static void NEXUS_P_FileMux_File_Detach(NEXUS_P_FileMux_File *muxFile)
{
    BDBG_OBJECT_DESTROY(muxFile, NEXUS_P_FileMux_File);
}

static void NEXUS_P_FileMux_Dequeue(NEXUS_P_FileMux_File *muxFile)
{
    const BMUXlib_StorageDescriptor *desc;

    if(BFIFO_READ_LEFT(&muxFile->fifo)==0) { (void)BERR_TRACE(NEXUS_INVALID_PARAMETER);return;}
    desc = *BFIFO_READ(&muxFile->fifo);
    if(muxFile->ioCanceled) { 
        BDBG_MSG(("%#x:%#x canceled io", (unsigned)muxFile->mux, (unsigned)muxFile));
        return; 
    }
    BDBG_ASSERT(!muxFile->ioInProgress);
    muxFile->ioInProgress = true;
    BDBG_MODULE_MSG(nexus_file_mux_io, ("%#x:%#x processing request %#x:%s %u %u:%u -> %#x[%u]", (unsigned)muxFile->mux, (unsigned)muxFile, (unsigned)desc, desc->bWriteOperation?"write":"read", (unsigned)desc->uiOffset, (unsigned)desc->iov[0].uiLength, (unsigned)desc->iov[0].pBufferAddress, desc->uiVectorCount));
    if(desc->bWriteOperation) {
        NEXUS_ASSERT_FIELD(NEXUS_FileMuxIoWriteAtom, base, BMUXlib_StorageBuffer, pBufferAddress);
        NEXUS_ASSERT_FIELD(NEXUS_FileMuxIoWriteAtom, len, BMUXlib_StorageBuffer, uiLength);
        NEXUS_ASSERT_STRUCTURE(NEXUS_FileMuxIoWriteAtom, BMUXlib_StorageBuffer);
        NEXUS_File_AsyncMuxWrite(&muxFile->file->mux, desc->uiOffset, (NEXUS_FileMuxIoWriteAtom *)desc->iov, desc->uiVectorCount, NEXUS_MODULE_SELF, NEXUS_P_FileMux_IoComplete, muxFile);
    } else {
        NEXUS_ASSERT_FIELD(NEXUS_FileMuxIoWriteAtom, base, BMUXlib_StorageBuffer, pBufferAddress);
        NEXUS_ASSERT_FIELD(NEXUS_FileMuxIoWriteAtom, len, BMUXlib_StorageBuffer, uiLength);
        NEXUS_ASSERT_STRUCTURE(NEXUS_FileMuxIoWriteAtom, BMUXlib_StorageBuffer);
        NEXUS_File_AsyncMuxRead(&muxFile->file->mux, desc->uiOffset, (NEXUS_FileMuxIoReadAtom *)desc->iov, desc->uiVectorCount, NEXUS_MODULE_SELF, NEXUS_P_FileMux_IoComplete, muxFile);
    }
    return;
}

static void NEXUS_P_FileMux_IoComplete(void *cntx, ssize_t size)
{
    NEXUS_P_FileMux_File *muxFile = cntx;
    const BMUXlib_StorageDescriptor *desc;
    NEXUS_FileMuxHandle mux;
    size_t expectedSize;
    unsigned i;

    BDBG_OBJECT_ASSERT(muxFile, NEXUS_P_FileMux_File);
    BDBG_ASSERT(muxFile->ioInProgress);
    NEXUS_ASSERT_MODULE();
    mux = muxFile->mux;
    if(mux->ioWaitingThreshold) {
        if(BFIFO_WRITE_PEEK(&muxFile->fifo)>=NEXUS_P_FILEMUX_QUEUE_THRESHOLD) {
            mux->ioWaitingThreshold = false;
            if(!mux->stopping) {
                NEXUS_FileMux_P_DoMux(mux);
            }
        }
    }
    muxFile->ioInProgress = false;
    if(muxFile->ioCanceled) {
        BDBG_MSG(("I/O:%#x canceled", (unsigned)muxFile));
        return;
    }
    desc = *BFIFO_READ(&muxFile->fifo);
    BDBG_MODULE_MSG(nexus_file_mux_io, ("%#x:%#x completed request %#x:%s %u:%u (%u)-> %#x:%d", (unsigned)muxFile->mux, (unsigned)muxFile, (unsigned)desc, desc->bWriteOperation?"write":"read", (unsigned)desc->uiOffset, (unsigned)desc->iov[0].uiLength, (unsigned)desc->uiVectorCount, (unsigned)desc->iov[0].pBufferAddress, size));
    if(BFIFO_READ_LEFT(&muxFile->fifo)==0) { (void)BERR_TRACE(NEXUS_UNKNOWN); return;}
    muxFile->lastResult = size;
    for(expectedSize=0,i=0;i<desc->uiVectorCount;i++) {
        expectedSize += desc->iov[i].uiLength;
    }
    if(size<0 || expectedSize != (size_t)size) { 
        BDBG_WRN(("I/O %#x result %d expected %u", (unsigned)muxFile, size, expectedSize));
        (void)BERR_TRACE(NEXUS_OS_ERROR);return;
    }
    muxFile->completed++;
    BFIFO_READ_COMMIT(&muxFile->fifo, 1);
    if(BFIFO_READ_LEFT(&muxFile->fifo)) {
        NEXUS_P_FileMux_Dequeue(muxFile);
    }
    return;
}

static BERR_Code NEXUS_P_FileMux_AddStorageDescriptors(void *pStorageInterfaceContext, const BMUXlib_StorageDescriptor *pDescriptors, size_t uiNumDescriptors, size_t *puiQueuedCount)
{
    NEXUS_P_FileMux_File *muxFile = pStorageInterfaceContext;
    unsigned i;
    unsigned j;
    bool idle;

    BDBG_OBJECT_ASSERT(muxFile, NEXUS_P_FileMux_File);
    BDBG_ASSERT(pDescriptors);
    BDBG_ASSERT(puiQueuedCount);
    idle = BFIFO_READ_LEFT(&muxFile->fifo)==0;
    BDBG_MODULE_MSG(nexus_file_mux_io, ("%#x:%#x > add descriptors %u:%u %s", (unsigned)muxFile->mux, (unsigned)muxFile, uiNumDescriptors, BFIFO_WRITE_PEEK(&muxFile->fifo), idle?"idle":""));
    for(i=0;i<uiNumDescriptors;i++) {
        const BMUXlib_StorageDescriptor **desc = BFIFO_WRITE(&muxFile->fifo);
        if(BFIFO_WRITE_LEFT(&muxFile->fifo)==0) {
            break;
        }
        *desc = pDescriptors;
        BFIFO_WRITE_COMMIT(&muxFile->fifo, 1);
        for(j=0;j<pDescriptors->uiVectorCount;j++) {
            BDBG_MODULE_MSG(nexus_file_mux_io, ("%#x:%#x queing request %u:%u %#x:%s:%u (%u:%u) %u -> %#x", (unsigned)muxFile->mux, (unsigned)muxFile, uiNumDescriptors, i, (unsigned)pDescriptors, pDescriptors->bWriteOperation?"write":"read", (unsigned)pDescriptors->uiOffset, pDescriptors->uiVectorCount, j, (unsigned)pDescriptors->iov[j].uiLength, (unsigned)pDescriptors->iov[j].pBufferAddress));
            BDBG_ASSERT(pDescriptors->iov[j].pBufferAddress!=0);
        }
        pDescriptors = BMUXLIB_STORAGE_GET_NEXT_DESC(pDescriptors);
    }
    *puiQueuedCount = i;
    BDBG_MODULE_MSG(nexus_file_mux_io, ("%#x:%#x < add descriptors %u:%u -> %u", (unsigned)muxFile->mux, (unsigned)muxFile, uiNumDescriptors, BFIFO_WRITE_PEEK(&muxFile->fifo), i));
    if(idle && i>0) {
        NEXUS_P_FileMux_Dequeue(muxFile);
    }
    return BERR_SUCCESS;
}

static BERR_Code NEXUS_P_FileMux_GetCompletedStorageDescriptors(void *pStorageInterfaceContext, size_t *puiCompletedCount)
{
    NEXUS_P_FileMux_File *muxFile = pStorageInterfaceContext;
    BDBG_OBJECT_ASSERT(muxFile, NEXUS_P_FileMux_File);
    BDBG_ASSERT(puiCompletedCount);
    BDBG_MODULE_MSG(nexus_file_mux_io, ("%#x:%#x completed %u", (unsigned)muxFile->mux, (unsigned)muxFile, muxFile->completed));
    if(muxFile->ioError) {return BERR_TRACE(BERR_OS_ERROR);}
    *puiCompletedCount = muxFile->completed;
    muxFile->completed = 0;
    return BERR_SUCCESS;
}

BDBG_OBJECT_ID(NEXUS_P_FileMux_TempStorage);

static void NEXUS_P_FileMux_TempStorage_Init(NEXUS_P_FileMux_TempStorage *tempStorage, NEXUS_FileMuxHandle mux)
{
    BDBG_OBJECT_INIT(tempStorage, NEXUS_P_FileMux_TempStorage);
    tempStorage->mux = mux;
    BLST_D_INIT(&tempStorage->files);
    return;
}

static void NEXUS_P_FileMux_GetDefaultStorageSettings(BMUXlib_StorageSettings *pStorageSettings)
{
    BDBG_ASSERT(pStorageSettings);
    BKNI_Memset(pStorageSettings, 0, sizeof(*pStorageSettings));
    return;
}

static BERR_Code NEXUS_P_FileMux_CreateStorage(void *pStorageContext, BMUXlib_StorageObjectInterface *pStorageInterface, const BMUXlib_StorageSettings *pStorageSettings)
{
    NEXUS_P_FileMux_TempFile *tempFile;
    NEXUS_P_FileMux_TempStorage *tempStorage = pStorageContext;
    NEXUS_MuxFileHandle file;
    BERR_Code rc;
    char fname[128];

    BDBG_OBJECT_ASSERT(tempStorage, NEXUS_P_FileMux_TempStorage);
    BDBG_ASSERT(pStorageSettings);
    BDBG_ASSERT(pStorageInterface);
    BSTD_UNUSED(pStorageSettings);

    tempFile = BKNI_Malloc(sizeof(*tempFile));
    if(!tempFile) {rc=BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);goto err_alloc;}
    BKNI_Snprintf(fname, sizeof(fname),"%s/mux%#x.tmp", tempStorage->mux->startSettings.tempDir, (unsigned)tempFile);
    BDBG_MSG(("using temprorary file %s", fname));
    file = NEXUS_MuxFile_OpenPosix(fname);
    if(!file) {rc=BERR_TRACE(NEXUS_OS_ERROR);goto err_open;}
    BDBG_MSG(("%#x: opening file %s:%#x(%#x)", (unsigned)tempStorage->mux, fname, tempFile, file)); 
    unlink(fname);
    NEXUS_P_FileMux_File_Attach(&tempFile->file, tempStorage->mux, file);
    tempFile->destroyed = false;
    BLST_D_INSERT_HEAD(&tempStorage->files, tempFile, link);

    pStorageInterface->pContext = &tempFile->file;
    pStorageInterface->pfAddDescriptors = NEXUS_P_FileMux_AddStorageDescriptors;
    pStorageInterface->pfGetCompleteDescriptors = NEXUS_P_FileMux_GetCompletedStorageDescriptors;

    return BERR_SUCCESS;
err_open:
    BKNI_Free(tempFile);
err_alloc:
    return rc;
}

static void NEXUS_P_TempFile_Destroy(NEXUS_P_FileMux_TempStorage *tempStorage,NEXUS_P_FileMux_TempFile *tempFile)
{
    BLST_D_REMOVE(&tempStorage->files, tempFile, link);
    NEXUS_MuxFile_Close(tempFile->file.file);
    NEXUS_P_FileMux_File_Detach(&tempFile->file);
    BKNI_Free(tempFile);
    return;
}

static void NEXIS_P_FileMux_DestroyStorage(void *pStorageContext, void *pStorageObjectContext)
{
    NEXUS_P_FileMux_TempFile *tempFile = pStorageObjectContext;
    NEXUS_P_FileMux_TempStorage *tempStorage = pStorageContext;

    BDBG_OBJECT_ASSERT(tempStorage, NEXUS_P_FileMux_TempStorage);
    BDBG_OBJECT_ASSERT(&tempFile->file, NEXUS_P_FileMux_File);
    if(!tempFile->file.ioInProgress) {
        NEXUS_P_TempFile_Destroy(tempStorage, tempFile);
    } else {
       tempFile->file.ioCanceled = true;
    }
    return;
}

static void NEXUS_P_TempStorage_CancelIo(NEXUS_P_FileMux_TempStorage *tempStorage)
{
    NEXUS_P_FileMux_TempFile *tempFile;

    for(tempFile=BLST_D_FIRST(&tempStorage->files);tempFile;tempFile=BLST_D_NEXT(tempFile, link)) {
        tempFile->file.ioCanceled = true;
    }
    return;
}

static bool NEXUS_P_TempStorage_IsInProgress(NEXUS_P_FileMux_TempStorage *tempStorage)
{
    NEXUS_P_FileMux_TempFile *tempFile;

    for(tempFile=BLST_D_FIRST(&tempStorage->files);tempFile;tempFile=BLST_D_NEXT(tempFile, link)) {
        if(tempFile->file.ioInProgress) {
            return true;
        }
    }
    return false;
}

static void NEXUS_P_TempStorage_Clean(NEXUS_P_FileMux_TempStorage *tempStorage)
{
    NEXUS_P_FileMux_TempFile *tempFile;
    /* coverity[use_after_free] */
    while(NULL!=(tempFile=BLST_D_FIRST(&tempStorage->files))) {
        if(!tempFile->destroyed) {
            BDBG_WRN(("destroying leaked file %#x", (unsigned)tempFile));
        }
        NEXUS_P_TempFile_Destroy(tempStorage, tempFile);
    }
    return;
}

static void
NEXUS_P_FileMux_GetDefaultCreateSettings_MP4( NEXUS_FileMuxCreateSettings *pSettings )
{
    BMUXlib_File_MP4_CreateSettings muxCreateSettings;
    BDBG_ASSERT(pSettings);

    BMUXlib_File_MP4_GetDefaultCreateSettings(&muxCreateSettings);
    pSettings->storageDescriptors = muxCreateSettings.uiNumOutputStorageDescriptors;
    pSettings->mp4.supported = true;
    pSettings->mp4.metadataCache = muxCreateSettings.uiNumMetadataEntriesCached;
    pSettings->mp4.heapSize = muxCreateSettings.uiBoxHeapSizeBytes;
    pSettings->mp4.sizeEntriesCache = muxCreateSettings.uiNumSizeEntries;
    pSettings->mp4.relocationBuffer = muxCreateSettings.uiRelocationBufferSizeBytes;
    return;
}

static void
NEXUS_P_FileMux_GetDefaultCreateSettings_ASF( NEXUS_FileMuxCreateSettings *pSettings )
{
#if B_HAS_ASF
    BMUXlib_File_ASF_CreateSettings muxCreateSettings;
    BDBG_ASSERT(pSettings);

    BMUXlib_File_ASF_GetDefaultCreateSettings(&muxCreateSettings);
    pSettings->asf.supported = true;
    pSettings->asf.packetLength = muxCreateSettings.uiPacketLength;
#endif
    BSTD_UNUSED(pSettings);
    return;
}

void
NEXUS_FileMux_GetDefaultCreateSettings( NEXUS_FileMuxCreateSettings *pSettings )
{
    BDBG_ASSERT(pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));

    NEXUS_P_FileMux_GetDefaultCreateSettings_MP4(pSettings);
    NEXUS_P_FileMux_GetDefaultCreateSettings_ASF(pSettings);

    return;
}

static NEXUS_Error 
NEXUS_P_FileMux_Create_MP4(NEXUS_FileMuxHandle mux, const NEXUS_FileMuxCreateSettings *pSettings)
{
    BERR_Code rc;
    BMUXlib_File_MP4_CreateSettings muxCreateSettings;

    BMUXlib_File_MP4_GetDefaultCreateSettings(&muxCreateSettings);
    muxCreateSettings.uiNumOutputStorageDescriptors = pSettings->storageDescriptors;
    muxCreateSettings.uiNumMetadataEntriesCached = pSettings->mp4.metadataCache;
    muxCreateSettings.uiBoxHeapSizeBytes = pSettings->mp4.heapSize;
    muxCreateSettings.uiNumSizeEntries = pSettings->mp4.sizeEntriesCache;
    muxCreateSettings.uiRelocationBufferSizeBytes = pSettings->mp4.relocationBuffer;

    rc = BMUXlib_File_MP4_Create(&mux->mp4.mux, &muxCreateSettings);
    if(rc!=BERR_SUCCESS) {return BERR_TRACE(rc);}

    return NEXUS_SUCCESS;
}

static void
NEXUS_P_FileMux_Destroy_MP4(NEXUS_FileMuxHandle mux)
{
    if(mux->mp4.mux) {
        BMUXlib_File_MP4_Destroy(mux->mp4.mux);
        mux->mp4.mux = NULL;
        mux->mp4.active = false;
    }
    return;
}

static NEXUS_Error 
NEXUS_P_FileMux_Create_ASF(NEXUS_FileMuxHandle mux, const NEXUS_FileMuxCreateSettings *pSettings)
{
#if B_HAS_ASF
    BERR_Code rc;
    BMUXlib_File_ASF_CreateSettings muxCreateSettings;

    BMUXlib_File_ASF_GetDefaultCreateSettings(&muxCreateSettings);
    muxCreateSettings.uiPacketLength = pSettings->asf.packetLength;

    rc = BMUXlib_File_ASF_Create(&mux->asf.mux, &muxCreateSettings);
    if(rc!=BERR_SUCCESS) {return BERR_TRACE(rc);}

    return NEXUS_SUCCESS;
#else
    BSTD_UNUSED(mux);
    BSTD_UNUSED(pSettings);
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
#endif
}

static void
NEXUS_P_FileMux_Destroy_ASF(NEXUS_FileMuxHandle mux)
{
#if B_HAS_ASF
    if(mux->asf.mux) {
        BMUXlib_File_ASF_Destroy(mux->asf.mux);
        mux->asf.mux = NULL;
        mux->asf.active = false;
    }
#endif
    BSTD_UNUSED(mux);
    return;
}


NEXUS_FileMuxHandle 
NEXUS_FileMux_Create( const NEXUS_FileMuxCreateSettings *pSettings )
{
    NEXUS_FileMuxHandle  mux;
    NEXUS_FileMuxCreateSettings createSettings;
    NEXUS_Error rc;

    if(pSettings==NULL) {
        NEXUS_FileMux_GetDefaultCreateSettings(&createSettings);
        pSettings = &createSettings;
    }
    mux = BKNI_Malloc(sizeof(*mux));
    if(mux==NULL) {rc=BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);goto error;}
    BDBG_OBJECT_INIT(mux, NEXUS_FileMux);
    BKNI_Memset(&mux->fnrt, 0, sizeof(NEXUS_P_FileMux_Fnrt));
    mux->createSettings = *pSettings;
    mux->muxTimer = NULL;
    mux->started = false;
    mux->ioWaitingThreshold = false;
    mux->stopping = false;
    mux->mp4.mux = NULL;
    mux->mp4.active = false;
#if B_HAS_ASF
    mux->asf.mux = NULL;
    mux->asf.active = false;
#endif
    NEXUS_P_FileMux_TempStorage_Init(&mux->tempStorage, mux);

    mux->finishedCallback = NEXUS_TaskCallback_Create(mux, NULL);
    if(!mux->finishedCallback) {rc=BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);goto error;}
    NEXUS_TaskCallback_Set(mux->finishedCallback, &pSettings->finished);

    if(pSettings->mp4.supported) {
        rc = NEXUS_P_FileMux_Create_MP4(mux, pSettings);
        if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);goto error_mp4;}
    }

    if(pSettings->asf.supported) {
        rc = NEXUS_P_FileMux_Create_ASF(mux, pSettings);
        if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);goto error_asf;}
    }

    return mux;

error_asf:
    NEXUS_P_FileMux_Destroy_MP4(mux);
error_mp4:
    BKNI_Free(mux);
error:
    return NULL;
}

void 
NEXUS_FileMux_Destroy(NEXUS_FileMuxHandle mux)
{
    BDBG_OBJECT_ASSERT(mux, NEXUS_FileMux);
    if(mux->started) {
        NEXUS_FileMux_Stop(mux);
    }
    NEXUS_TaskCallback_Destroy(mux->finishedCallback);
    NEXUS_P_FileMux_Destroy_MP4(mux);
    NEXUS_P_FileMux_Destroy_ASF(mux);
    BDBG_OBJECT_DESTROY(mux, NEXUS_FileMux);
    BKNI_Free(mux);
    return;
}

static void
NEXUS_P_FileMux_GetDefaultStartSettings_MP4( NEXUS_FileMuxStartSettings *pSettings)
{
    BMUXlib_File_MP4_StartSettings  startSettings;
    BMUXlib_File_MP4_GetDefaultStartSettings(&startSettings);
    pSettings->expectedDuration = startSettings.uiExpectedDurationMs;
    pSettings->streamSettings.mp4.progressiveDownloadCompatible = startSettings.bProgressiveDownloadCompatible;
    pSettings->streamSettings.mp4.createTime = startSettings.uiCreateTimeUTC;
    return;
}

static void
NEXUS_P_FileMux_GetDefaultStartSettings_ASF( NEXUS_FileMuxStartSettings *pSettings)
{
#if B_HAS_ASF
    BMUXlib_File_ASF_StartSettings  startSettings;

    BMUXlib_File_ASF_GetDefaultStartSettings(&startSettings);
    BDBG_CASSERT(sizeof(pSettings->streamSettings.asf.fileId)==sizeof(startSettings.auiFileId));
    BKNI_Memcpy(pSettings->streamSettings.asf.fileId, startSettings.auiFileId, sizeof(pSettings->streamSettings.asf.fileId));
    pSettings->streamSettings.asf.broadcastOnly = startSettings.bBroadcastOnly;
#endif
    BSTD_UNUSED(pSettings);
    return;
}

void 
NEXUS_FileMux_GetDefaultStartSettings(NEXUS_FileMuxStartSettings *pSettings, NEXUS_TransportType streamType)
{
    BDBG_ASSERT(pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->streamType = streamType;
    switch(streamType) {
    case NEXUS_TransportType_eMp4:
        NEXUS_P_FileMux_GetDefaultStartSettings_MP4(pSettings);
        break;
    case NEXUS_TransportType_eAsf:
        NEXUS_P_FileMux_GetDefaultStartSettings_ASF(pSettings);
        break;
    default:
        BDBG_WRN(("streamType:%u is not supported", (unsigned)streamType));
        break;
    }
    return;
}

static BERR_Code
NEXUS_FileMux_P_GetVideoBufferDescriptors( void *context, const BAVC_VideoBufferDescriptor *descriptors0[], size_t *numDescriptors0, const BAVC_VideoBufferDescriptor *descriptors1[], size_t *numDescriptors1) 
{
    BERR_Code rc;
    NEXUS_ASSERT_MODULE();
    NEXUS_ASSERT_STRUCTURE(NEXUS_VideoEncoderDescriptor, BAVC_VideoBufferDescriptor);
    rc = NEXUS_VideoEncoder_GetBuffer(context, (const NEXUS_VideoEncoderDescriptor**)descriptors0, numDescriptors0, (const NEXUS_VideoEncoderDescriptor**)descriptors1, numDescriptors1);
    BDBG_MODULE_MSG(nexus_file_mux_video, ("GetVideoBufferDescriptors:%#x numDescriptors:%u:%u", (unsigned)context, *numDescriptors0, *numDescriptors1));
    return BERR_TRACE(rc);
}

static BERR_Code
NEXUS_FileMux_P_ConsumeVideoBufferDescriptors( void *context, size_t numDescriptors)
{
    BERR_Code rc;

    NEXUS_ASSERT_MODULE();
    rc = NEXUS_VideoEncoder_ReadComplete(context, numDescriptors);
    BDBG_MODULE_MSG(nexus_file_mux_video, ("ConsumeVideoBufferDescriptors:%#x numDescriptors:%u:%u", (unsigned)context, numDescriptors));
    return BERR_TRACE(rc);
}

static BERR_Code
NEXUS_FileMux_P_GetVideoBufferStatus( void *context, BAVC_VideoBufferStatus *status)
{
    NEXUS_VideoEncoderStatus encoderStatus;

    NEXUS_ASSERT_MODULE();
    BDBG_ASSERT(status);
    NEXUS_VideoEncoder_GetStatus(context, &encoderStatus);
    BKNI_Memset(status,0,sizeof(*status));
    status->stCommon.pFrameBufferBaseAddress = encoderStatus.bufferBase;
    status->stCommon.pMetadataBufferBaseAddress = encoderStatus.metadataBufferBase;
    return BERR_SUCCESS;
}

static BERR_Code
NEXUS_FileMux_P_GetAudioBufferDescriptors(
   void *pvContext,
   const BAVC_AudioBufferDescriptor *astDescriptors0[], /* Can be NULL if uiNumDescriptors0=0. */
   size_t *puiNumDescriptors0,
   const BAVC_AudioBufferDescriptor *astDescriptors1[], /* Needed to handle FIFO wrap. Can be NULL if uiNumDescriptors1=0. */
   size_t *puiNumDescriptors1
   )
{
    BERR_Code rc;
    NEXUS_ASSERT_MODULE();
    NEXUS_ASSERT_STRUCTURE(NEXUS_AudioMuxOutputFrame, BAVC_AudioBufferDescriptor);
    rc = NEXUS_AudioMuxOutput_GetBuffer(pvContext, (const NEXUS_AudioMuxOutputFrame**)astDescriptors0, puiNumDescriptors0, (const NEXUS_AudioMuxOutputFrame**)astDescriptors1, puiNumDescriptors1);
    BDBG_MODULE_MSG(nexus_file_mux_audio, ("GetAudioBufferDescriptors:%#x numDescriptors:%u:%u", (unsigned)pvContext, *puiNumDescriptors0, *puiNumDescriptors1));
    return BERR_TRACE(rc);
    
}

static BERR_Code
NEXUS_FileMux_P_ConsumeAudioBufferDescriptors(
   void *pvContext,
   size_t uiNumDescriptors
   )
{
    BERR_Code rc;

    NEXUS_ASSERT_MODULE();
    rc = NEXUS_AudioMuxOutput_ReadComplete(pvContext, uiNumDescriptors);
    return BERR_TRACE(rc);
}

static BERR_Code
NEXUS_FileMux_P_GetAudioBufferStatus(
   void *pvContext,
   BAVC_AudioBufferStatus *pBufferStatus
   )
{
    NEXUS_AudioMuxOutputStatus encoderStatus;
    NEXUS_ASSERT_MODULE();
    BDBG_ASSERT(pBufferStatus);
    NEXUS_AudioMuxOutput_GetStatus(pvContext, &encoderStatus);
    pBufferStatus->stCommon.pFrameBufferBaseAddress = encoderStatus.pBufferBase;
    pBufferStatus->stCommon.pMetadataBufferBaseAddress = encoderStatus.pMetadataBufferBase;
    return BERR_SUCCESS;
}


static void
NEXUS_FileMux_P_MuxTimer(void *context)
{
    NEXUS_FileMuxHandle mux=context;

    BDBG_OBJECT_ASSERT(mux, NEXUS_FileMux);

    mux->muxTimer = NULL;
    NEXUS_FileMux_P_DoMux(mux);
    return;
}

static void
NEXUS_FileMux_P_DoMux(NEXUS_FileMuxHandle mux)
{
    BERR_Code rc;
    BMUXlib_DoMux_Status muxStatus;


    if(mux->stopping) {
        return;
    }
    switch(mux->startSettings.streamType) {
    case NEXUS_TransportType_eMp4:
        if(!mux->mp4.active) { rc=BERR_TRACE(NEXUS_UNKNOWN); goto error; }
        rc = BMUXlib_File_MP4_DoMux(mux->mp4.mux, &muxStatus);
        if(rc!=BERR_SUCCESS) { rc=BERR_TRACE(rc); goto error; }
        break;
#if B_HAS_ASF
    case NEXUS_TransportType_eAsf:
        if(!mux->asf.active) { rc=BERR_TRACE(NEXUS_UNKNOWN); goto error; }
        rc = BMUXlib_File_ASF_DoMux(mux->asf.mux, &muxStatus);
        if(rc!=BERR_SUCCESS) { rc=BERR_TRACE(rc); goto error; }
        break;
#endif
    default:
        rc=BERR_TRACE(NEXUS_UNKNOWN); goto error;
    }
    mux->duration = muxStatus.uiCompletedDuration;
    BDBG_MODULE_MSG(nexus_file_mux_timer, ("MuxTimer:%#x nextExecutionTime:%u state:%u %s duration=%u(ms)", (unsigned)mux, muxStatus.uiNextExecutionTime, (unsigned)muxStatus.eState,muxStatus.bBlockedOutput?"BlockedOutput":"", muxStatus.uiCompletedDuration));
    if(muxStatus.eState!=BMUXlib_State_eFinished) {
        if(muxStatus.bBlockedOutput) {
            unsigned queued = BFIFO_READ_PEEK(&mux->outputFile.fifo);

            if(queued==0) {
                const NEXUS_P_FileMux_TempFile *tempFile;
                for(tempFile=BLST_D_FIRST(&mux->tempStorage.files);tempFile;tempFile=BLST_D_NEXT(tempFile, link)) {
                    queued += BFIFO_READ_PEEK(&tempFile->file.fifo);
                }
            }

            if(queued>0) {
                mux->ioWaitingThreshold = true;
            } else {
                BDBG_WRN(("NEXUS_FileMux_P_MuxTimer:%#lx BlockedOutput set without pending I/O transactions",(unsigned)mux));
            }
        }
        if(mux->muxTimer==NULL) {
            unsigned nextExecutionTime = muxStatus.uiNextExecutionTime;
            /* called from the timer  context */
            if(nextExecutionTime<30) {nextExecutionTime=30;}
            mux->muxTimer = NEXUS_ScheduleTimer(nextExecutionTime, NEXUS_FileMux_P_MuxTimer, mux);
            if(mux->muxTimer==NULL) {rc=BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);goto error; }
        }
    } else {
        BDBG_MODULE_MSG(nexus_file_mux_timer, ("MuxTimer:%#x finished", (unsigned)mux));
        NEXUS_TaskCallback_Fire(mux->finishedCallback);
    }
    return;
error:
    return;
}

static NEXUS_Error 
NEXUS_P_FileMux_Start_MP4( NEXUS_FileMuxHandle mux, const NEXUS_FileMuxStartSettings *pSettings, NEXUS_MuxFileHandle file)
{
    BMUXlib_File_MP4_StartSettings  startSettings;
    BERR_Code rc;
    unsigned i,j;

    BSTD_UNUSED(file);

    BDBG_ASSERT(pSettings->streamType==NEXUS_TransportType_eMp4);
    if(mux->mp4.mux==NULL) {rc=BERR_TRACE(NEXUS_NOT_SUPPORTED);goto error;}

    BMUXlib_File_MP4_GetDefaultStartSettings(&startSettings);
    startSettings.uiExpectedDurationMs = pSettings->expectedDuration;
    startSettings.bProgressiveDownloadCompatible = pSettings->streamSettings.mp4.progressiveDownloadCompatible;
    startSettings.uiCreateTimeUTC = pSettings->streamSettings.mp4.createTime;
    startSettings.stOutput.pContext = &mux->outputFile;
    startSettings.stOutput.pfAddDescriptors = NEXUS_P_FileMux_AddStorageDescriptors;
    startSettings.stOutput.pfGetCompleteDescriptors = NEXUS_P_FileMux_GetCompletedStorageDescriptors;
    startSettings.stStorage.pContext = &mux->tempStorage;
    startSettings.stStorage.pfGetDefaultStorageSettings = NEXUS_P_FileMux_GetDefaultStorageSettings;
    startSettings.stStorage.pfCreateStorageObject = NEXUS_P_FileMux_CreateStorage;
    startSettings.stStorage.pfDestroyStorageObject = NEXIS_P_FileMux_DestroyStorage;
    /* map each encoder to an FNRT interface if enabled and connect FNRT to mux */
    if(mux->fnrt.hFnrt) {
        for(i=0; i < mux->fnrt.numVideoTracks; i++) {
            startSettings.stVideoInputs[i].uiTrackID   = mux->fnrt.videoOutputs[i].track;
            startSettings.stVideoInputs[i].stInterface = mux->fnrt.videoOutputs[i].interface;
        }
        startSettings.uiNumVideoInputs = mux->fnrt.numVideoTracks;

        for(i=0; i < mux->fnrt.numAudioTracks; i++) {
            startSettings.stAudioInputs[i].uiTrackID   = mux->fnrt.audioOutputs[i].track;
            startSettings.stAudioInputs[i].stInterface = mux->fnrt.audioOutputs[i].interface;
        }
        startSettings.uiNumAudioInputs = mux->fnrt.numAudioTracks;
    } else {
        for(i=j=0;i<NEXUS_MAX_MUX_PIDS;i++) {
            if(pSettings->video[i].encoder==NULL) {
                break;
            }
            startSettings.stVideoInputs[j].uiTrackID = pSettings->video[i].track;
            startSettings.stVideoInputs[j].stInterface.stBufferInfo.eProtocol = NEXUS_P_VideoCodec_ToMagnum(pSettings->video[i].codec, NEXUS_TransportType_eEs);
            startSettings.stVideoInputs[j].stInterface.pContext = pSettings->video[i].encoder;
            startSettings.stVideoInputs[j].stInterface.fGetBufferDescriptors = NEXUS_FileMux_P_GetVideoBufferDescriptors;
            startSettings.stVideoInputs[j].stInterface.fConsumeBufferDescriptors = NEXUS_FileMux_P_ConsumeVideoBufferDescriptors;
            startSettings.stVideoInputs[j].stInterface.fGetBufferStatus = NEXUS_FileMux_P_GetVideoBufferStatus;
            j++;
        }
        startSettings.uiNumVideoInputs = j;
        for(j=i=0;i<NEXUS_MAX_MUX_PIDS;i++) {
            if(pSettings->audio[i].muxOutput==NULL) {
                break;
            }
            startSettings.stAudioInputs[j].uiTrackID = pSettings->audio[i].track;
            startSettings.stAudioInputs[j].stInterface.stBufferInfo.eProtocol = NEXUS_P_AudioCodec_ToMagnum(pSettings->audio[i].codec);
            startSettings.stAudioInputs[j].stInterface.pContext = pSettings->audio[i].muxOutput;
            startSettings.stAudioInputs[j].stInterface.fGetBufferDescriptors = NEXUS_FileMux_P_GetAudioBufferDescriptors;
            startSettings.stAudioInputs[j].stInterface.fConsumeBufferDescriptors = NEXUS_FileMux_P_ConsumeAudioBufferDescriptors;
            startSettings.stAudioInputs[j].stInterface.fGetBufferStatus = NEXUS_FileMux_P_GetAudioBufferStatus;
            j++;
        }
        startSettings.uiNumAudioInputs = j;
    }
 
    rc = BMUXlib_File_MP4_Start(mux->mp4.mux, &startSettings);
    if (rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);goto error;}
    mux->mp4.active = true;

    return NEXUS_SUCCESS;
error:
    return rc;
}

static NEXUS_Error 
NEXUS_P_FileMux_Start_ASF( NEXUS_FileMuxHandle mux, const NEXUS_FileMuxStartSettings *pSettings, NEXUS_MuxFileHandle file)
{
#if B_HAS_ASF
    BMUXlib_File_ASF_StartSettings  startSettings;
    BERR_Code rc;
    unsigned i,j;

    BSTD_UNUSED(file);

    BDBG_ASSERT(pSettings->streamType==NEXUS_TransportType_eAsf);
    if(mux->asf.mux==NULL) {rc=BERR_TRACE(NEXUS_NOT_SUPPORTED);goto error;}

    BMUXlib_File_ASF_GetDefaultStartSettings(&startSettings);
    BKNI_Memcpy(startSettings.auiFileId, pSettings->streamSettings.asf.fileId,  sizeof(startSettings.auiFileId));
    startSettings.bBroadcastOnly = pSettings->streamSettings.asf.broadcastOnly;
    startSettings.stOutput.pContext = &mux->outputFile;
    startSettings.stOutput.pfAddDescriptors = NEXUS_P_FileMux_AddStorageDescriptors;
    startSettings.stOutput.pfGetCompleteDescriptors = NEXUS_P_FileMux_GetCompletedStorageDescriptors;
    startSettings.stStorage.pContext = &mux->tempStorage;
    startSettings.stStorage.pfGetDefaultStorageSettings = NEXUS_P_FileMux_GetDefaultStorageSettings;
    startSettings.stStorage.pfCreateStorageObject = NEXUS_P_FileMux_CreateStorage;
    startSettings.stStorage.pfDestroyStorageObject = NEXIS_P_FileMux_DestroyStorage;
    if(mux->fnrt.hFnrt) {
        for(i=0; i < mux->fnrt.numVideoTracks; i++) {
            if(mux->fnrt.videoOutputs[i].track > 127) {return BERR_TRACE(NEXUS_INVALID_PARAMETER);}
            startSettings.stVideoInputs[i].uiStreamNumber   = mux->fnrt.videoOutputs[i].track;
            startSettings.stVideoInputs[i].stInterface = mux->fnrt.videoOutputs[i].interface;
        }
        startSettings.uiNumVideoInputs = mux->fnrt.numVideoTracks;

        for(i=0; i < mux->fnrt.numAudioTracks; i++) {
            if(mux->fnrt.audioOutputs[i].track > 127) {return BERR_TRACE(NEXUS_INVALID_PARAMETER);}
            startSettings.stAudioInputs[i].uiStreamNumber   = mux->fnrt.audioOutputs[i].track;
            startSettings.stAudioInputs[i].stInterface = mux->fnrt.audioOutputs[i].interface;
        }
        startSettings.uiNumAudioInputs = mux->fnrt.numAudioTracks;
    } else {
        for(i=j=0;i<NEXUS_MAX_MUX_PIDS;i++) {
            if(pSettings->video[i].encoder==NULL) {
                break;
            }
            if(pSettings->video[i].track > 127) {return BERR_TRACE(NEXUS_INVALID_PARAMETER);}
            startSettings.stVideoInputs[j].uiStreamNumber  = pSettings->video[i].track;
            startSettings.stVideoInputs[j].stInterface.stBufferInfo.eProtocol = NEXUS_P_VideoCodec_ToMagnum(pSettings->video[i].codec, NEXUS_TransportType_eEs);
            startSettings.stVideoInputs[j].stInterface.pContext = pSettings->video[i].encoder;
            startSettings.stVideoInputs[j].stInterface.fGetBufferDescriptors = NEXUS_FileMux_P_GetVideoBufferDescriptors;
            startSettings.stVideoInputs[j].stInterface.fConsumeBufferDescriptors = NEXUS_FileMux_P_ConsumeVideoBufferDescriptors;
            startSettings.stVideoInputs[j].stInterface.fGetBufferStatus = NEXUS_FileMux_P_GetVideoBufferStatus;
            j++;
        }
        startSettings.uiNumVideoInputs = j;
        for(j=i=0;i<NEXUS_MAX_MUX_PIDS;i++) {
            if(pSettings->audio[i].muxOutput==NULL) {
                break;
            }
            if(pSettings->audio[i].track > 127) {return BERR_TRACE(NEXUS_INVALID_PARAMETER);}
            startSettings.stAudioInputs[j].uiStreamNumber = pSettings->audio[i].track;
            startSettings.stAudioInputs[j].stInterface.stBufferInfo.eProtocol = NEXUS_P_AudioCodec_ToMagnum(pSettings->audio[i].codec);
            startSettings.stAudioInputs[j].stInterface.pContext = pSettings->audio[i].muxOutput;
            startSettings.stAudioInputs[j].stInterface.fGetBufferDescriptors = NEXUS_FileMux_P_GetAudioBufferDescriptors;
            startSettings.stAudioInputs[j].stInterface.fConsumeBufferDescriptors = NEXUS_FileMux_P_ConsumeAudioBufferDescriptors;
            startSettings.stAudioInputs[j].stInterface.fGetBufferStatus = NEXUS_FileMux_P_GetAudioBufferStatus;
            j++;
        }
        startSettings.uiNumAudioInputs = j;
    }
 
    rc = BMUXlib_File_ASF_Start(mux->asf.mux, &startSettings);
    if (rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);goto error;}
    mux->asf.active = true;

    return NEXUS_SUCCESS;
error:
    return rc;
#else /* B_HAS_ASF */
    BSTD_UNUSED(mux);
    BSTD_UNUSED(pSettings);
    BSTD_UNUSED(file);
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
#endif
}



NEXUS_Error 
NEXUS_FileMux_Start( NEXUS_FileMuxHandle mux, const NEXUS_FileMuxStartSettings *pSettings, NEXUS_MuxFileHandle file)
{
    unsigned i, j, k;
    BERR_Code rc;
    BDBG_OBJECT_ASSERT(mux, NEXUS_FileMux);
    BDBG_ASSERT(pSettings);
    BDBG_ASSERT(file);

    mux->stopping = false;
    mux->ioWaitingThreshold = false;
    mux->startSettings = *pSettings;

    /* 1) search if FNRTlib is needed to group inputs to shared FNRT video output interface */
    for(i=j=k=0; i < NEXUS_MAX_MUX_PIDS; i++) {
        if(pSettings->video[i].encoder==NULL) {
            continue;
        }
        /* to find matched track ID to share the same fnrt output interface */
        for(k=0; k<=j; k++) {
            if(pSettings->video[i].track == mux->fnrt.videoOutputs[k].track) {
                mux->fnrt.videoInputs[i].output = k;/* input[i] -> output[k] */
                if(mux->fnrt.hFnrt == NULL) {/* create FNRTlib if trackID matches */
                    rc = BMUXlib_FNRT_Create(&mux->fnrt.hFnrt,NULL);
                    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);goto error;}
                }
                break;
            }
        }
        if(k > j) {/* no match needs to create a new fnrt interface */
            mux->fnrt.videoInputs[i].output = j;/* input[i] -> output[j] */
            mux->fnrt.videoOutputs[j].track = pSettings->video[i].track;
            j++;
        }
    }
    if(j>BMUXLIB_FILE_MP4_MAX_VIDEO_INPUTS) { rc = BERR_TRACE(NEXUS_NOT_SUPPORTED); goto error;}
    mux->fnrt.numVideoTracks = j;

    /* 2) search if FNRTlib is needed to group inputs to shared FNRT audio output interface */
    for(i=j=k=0; i < NEXUS_MAX_MUX_PIDS; i++) {
        if(pSettings->audio[i].muxOutput==NULL) {
            continue;
        }
        /* to find matched track ID to share the same fnrt output interface */
        for(k=0; k<=j; k++) {
            if(pSettings->audio[i].track == mux->fnrt.audioOutputs[k].track) {
                mux->fnrt.audioInputs[i].output = k;/* input[i] -> output[k] */
                if(mux->fnrt.hFnrt == NULL) {/* create FNRTlib if trackID matches */
                    rc = BMUXlib_FNRT_Create(&mux->fnrt.hFnrt,NULL);
                    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);goto error;}
                }
                break;
            }
        }
        if(k > j) {/* no match needs to create a new fnrt interface */
            mux->fnrt.audioInputs[i].output = j;/* input[i] -> output[j] */
            mux->fnrt.audioOutputs[j].track = pSettings->audio[i].track;
            j++;
        }
    }
    if(j>BMUXLIB_FILE_MP4_MAX_AUDIO_INPUTS) { rc = BERR_TRACE(NEXUS_NOT_SUPPORTED); goto error;}
    mux->fnrt.numAudioTracks = j;

    /* 3) map each encoder to an FNRT interface if enabled and connect FNRT to mux */
    if(mux->fnrt.hFnrt) {
        BMUXlib_FNRT_InputSettings inputSetting;
        BMUXlib_FNRT_OutputSettings outputSetting;

        /* attach video encoders to an FNRT interface */
        BMUXlib_FNRT_GetDefaultInputSettings(&inputSetting);
        for(i=0; i < NEXUS_MAX_MUX_PIDS; i++) {
            if(pSettings->video[i].encoder==NULL) {
                continue;
            }
            mux->fnrt.videoInputs[i].interface.pContext = pSettings->video[i].encoder;
            mux->fnrt.videoInputs[i].interface.fGetBufferDescriptors = NEXUS_FileMux_P_GetVideoBufferDescriptors;
            mux->fnrt.videoInputs[i].interface.fConsumeBufferDescriptors = NEXUS_FileMux_P_ConsumeVideoBufferDescriptors;
            mux->fnrt.videoInputs[i].interface.fGetBufferStatus = NEXUS_FileMux_P_GetVideoBufferStatus;
            mux->fnrt.videoInputs[i].interface.stBufferInfo.eProtocol = NEXUS_P_VideoCodec_ToMagnum(pSettings->video[i].codec, NEXUS_TransportType_eEs);
            inputSetting.uiGroupId = mux->fnrt.videoInputs[i].output;/* input -> output group mapping */
            BMUXlib_FNRT_AddVideoInputInterface(mux->fnrt.hFnrt, &mux->fnrt.videoInputs[i].interface, &inputSetting);
        }
        BMUXlib_FNRT_GetDefaultOutputSettings(&outputSetting);
        for(i=0; i < mux->fnrt.numVideoTracks; i++) {
            outputSetting.uiGroupId = i;
            BMUXlib_FNRT_GetVideoOutputInterface(mux->fnrt.hFnrt,&mux->fnrt.videoOutputs[i].interface, &outputSetting);
        }

        /* attach audio encoders to an FNRT interface */
        for(i=0; i < NEXUS_MAX_MUX_PIDS; i++) {
            if(pSettings->audio[i].muxOutput==NULL) {
                continue;
            }
            mux->fnrt.audioInputs[i].interface.pContext = pSettings->audio[i].muxOutput;
            mux->fnrt.audioInputs[i].interface.fGetBufferDescriptors = NEXUS_FileMux_P_GetAudioBufferDescriptors;
            mux->fnrt.audioInputs[i].interface.fConsumeBufferDescriptors = NEXUS_FileMux_P_ConsumeAudioBufferDescriptors;
            mux->fnrt.audioInputs[i].interface.fGetBufferStatus = NEXUS_FileMux_P_GetAudioBufferStatus;
            mux->fnrt.audioInputs[i].interface.stBufferInfo.eProtocol = NEXUS_P_AudioCodec_ToMagnum(pSettings->audio[0].codec);
            inputSetting.uiGroupId = mux->fnrt.audioInputs[i].output;/* input -> output group mapping */
            BMUXlib_FNRT_AddAudioInputInterface(mux->fnrt.hFnrt, &mux->fnrt.audioInputs[i].interface, &inputSetting);
        }
        for(i=0; i < mux->fnrt.numAudioTracks; i++) {
            outputSetting.uiGroupId = i;
            BMUXlib_FNRT_GetAudioOutputInterface(mux->fnrt.hFnrt,&mux->fnrt.audioOutputs[i].interface, &outputSetting);
        }
    }

    NEXUS_P_FileMux_File_Attach(&mux->outputFile, mux, file);
    switch(pSettings->streamType) {
    case NEXUS_TransportType_eMp4:
        rc = NEXUS_P_FileMux_Start_MP4(mux, pSettings, file);
        if(rc!=NEXUS_SUCCESS) {rc=BERR_TRACE(rc);goto error;}
        break;
    case NEXUS_TransportType_eAsf:
        rc = NEXUS_P_FileMux_Start_ASF(mux, pSettings, file);
        if(rc!=NEXUS_SUCCESS) {rc=BERR_TRACE(rc);goto error;}
        break;
    default:
        rc = BERR_TRACE(NEXUS_NOT_SUPPORTED); goto error;
    }
    mux->muxTimer = NEXUS_ScheduleTimer(0, NEXUS_FileMux_P_MuxTimer, mux);
    if(mux->muxTimer==NULL) {rc=BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);goto error; }

    mux->started = true;
    mux->duration = 0;

    return NEXUS_SUCCESS;

error:
    return rc;
}

void 
NEXUS_FileMux_Finish(NEXUS_FileMuxHandle mux)
{
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(mux, NEXUS_FileMux);
    switch(mux->startSettings.streamType) {
    case NEXUS_TransportType_eMp4:
        if(mux->mp4.active) {
            BMUXlib_File_MP4_FinishSettings muxFinishSettings;
            BMUXlib_File_MP4_GetDefaultFinishSettings(&muxFinishSettings);
            rc = BMUXlib_File_MP4_Finish(mux->mp4.mux, &muxFinishSettings);
            if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);goto error;}
        }
        break;
#if B_HAS_ASF
    case NEXUS_TransportType_eAsf:
        if(mux->asf.active) {
            BMUXlib_File_ASF_FinishSettings muxFinishSettings;
            BMUXlib_File_ASF_GetDefaultFinishSettings(&muxFinishSettings);
            rc = BMUXlib_File_ASF_Finish(mux->asf.mux, &muxFinishSettings);
            if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);goto error;}
        }
        break;
#endif
    default:
        break;
    }

error:
    return;
}

void 
NEXUS_FileMux_Stop(NEXUS_FileMuxHandle mux)
{
    BERR_Code rc;
    unsigned time;

    BDBG_OBJECT_ASSERT(mux, NEXUS_FileMux);
    if(!mux->started) {
        return;
    }
    mux->stopping = true;
    switch(mux->startSettings.streamType) {
    case NEXUS_TransportType_eMp4:
        if(mux->mp4.active) {
            rc = BMUXlib_File_MP4_Stop(mux->mp4.mux);
            if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);goto error;}
        }
        break;
#if B_HAS_ASF
    case NEXUS_TransportType_eAsf:
        if(mux->asf.active) {
            rc = BMUXlib_File_ASF_Stop(mux->asf.mux);
            if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);goto error;}
        }
        break;
#endif
    default:
        break;
    }
    if(mux->muxTimer) {
        NEXUS_CancelTimer(mux->muxTimer);
        mux->muxTimer = NULL;
    }
    for(time=0;mux->outputFile.ioInProgress || NEXUS_P_TempStorage_IsInProgress(&mux->tempStorage);) {
        NEXUS_P_TempStorage_CancelIo(&mux->tempStorage);
        mux->outputFile.ioCanceled = true;
        if(time<1000) {
            BDBG_ERR(("there are some I/O outstanding and they can't be canceled"));
            break;
        } else {
            BDBG_WRN(("there are some I/O outstanding wait for them to get completed .. %u", time));
        }
        NEXUS_UnlockModule();
        BKNI_Sleep(30);
        NEXUS_LockModule();
        time+=30;
    }

    mux->started = false;
    NEXUS_P_FileMux_File_Detach(&mux->outputFile);
    NEXUS_P_TempStorage_Clean(&mux->tempStorage);

    /* detach FNRT */
    if(mux->fnrt.hFnrt) {
        BMUXlib_FNRT_Destroy(mux->fnrt.hFnrt);
    }
    BKNI_Memset(&mux->fnrt, 0, sizeof(NEXUS_P_FileMux_Fnrt));
    return;

error:
    return;
}

NEXUS_Error 
NEXUS_FileMux_GetStatus( NEXUS_FileMuxHandle mux, NEXUS_FileMuxStatus *pStatus )
{
    BDBG_OBJECT_ASSERT(mux, NEXUS_FileMux);
    BDBG_ASSERT(pStatus);

    BKNI_Memset(pStatus, 0, sizeof(*pStatus));
    pStatus->duration = mux->duration;

    return NEXUS_SUCCESS;
}

