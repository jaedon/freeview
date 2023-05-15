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
 * $brcm_Workfile: nexus_file_mux_module.h $
 * $brcm_Revision: 9 $
 * $brcm_Date: 8/31/12 1:43p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/file_mux/src/nexus_file_mux_module.h $
 * 
 * 9   8/31/12 1:43p hongtaoz
 * SW7425-3425: merged FNRT to mainline;
 * 
 * FNRT_Demo/3   8/13/12 12:05p hongtaoz
 * SW7425-3684: added mux media duration status API;
 * 
 * FNRT_Demo/2   7/25/12 8:06p hongtaoz
 * SW7425-3425, SW7425-3362: map FNRT inputs to output by track number;
 *  removed FNRT enable setting;
 * 
 * FNRT_Demo/1   7/6/12 5:40p hongtaoz
 * SW7425-3425: add FNRT support to file mux module;
 * 
 * 8   8/16/12 3:28p vsilyaev
 * SW7425-3684: Added duration of muxed portion of stream/file
 * 
 * 7   11/30/11 1:37p vsilyaev
 * SW7425-1790: Added support for S/G file mux I/O
 * 
 * SW7425-324/1   11/21/11 1:01p vsilyaev
 * SW7425-1790: Added support for S/G file mux I/O
 * 
 * 6   6/15/11 2:36p vsilyaev
 * SW7425-713: Added code to handle mux blocking on I/O resources
 * 
 * 5   6/14/11 4:39p vsilyaev
 * SW7425-713: Bumped size of the queue size to 64
 * 
 * 4   6/8/11 12:03p vsilyaev
 * SW7425-447: Conditionally compile ASF support
 * 
 * 3   5/16/11 8:11p vsilyaev
 * SW7425-482: Added support for the ASF file MUX
 * 
 * 2   4/7/11 3:23p vsilyaev
 * SW7425-232: Updated MP4 mux support
 * 
 *
 **************************************************************************/
#ifndef NEXUS_FILE_MUX_MODULE_H__
#define NEXUS_FILE_MUX_MODULE_H__

#include "nexus_base.h"
#include "nexus_file_mux_thunks.h"
#include "nexus_file_mux.h"
#include "nexus_file_mux_init.h"
#include "priv/nexus_playpump_priv.h"
#include "priv/nexus_tsmux_priv.h"

#include "priv/nexus_core.h"
#include "bmuxlib_file_mp4.h"
#include "bmuxlib_fnrt.h"
#if B_HAS_ASF
#include "bmuxlib_file_asf.h"
#endif

#include "bfifo.h"
#include "blst_list.h"

#ifdef __cplusplus
extern "C" {
#endif

BDBG_OBJECT_ID_DECLARE(NEXUS_FileMux);

#define NEXUS_P_FILEMUX_QUEUE_SIZE  64
typedef struct NEXUS_P_FileMux_File {
    BDBG_OBJECT(NEXUS_P_FileMux_File)
    NEXUS_MuxFileHandle file;
    NEXUS_FileMuxHandle mux;
    unsigned completed;
    ssize_t lastResult;
    bool ioError;
    bool ioInProgress;
    bool ioCanceled;
    BFIFO_HEAD(NEXUS_P_FileMux_Fifo, const BMUXlib_StorageDescriptor *) fifo;
    const BMUXlib_StorageDescriptor *descriptors[NEXUS_P_FILEMUX_QUEUE_SIZE];
} NEXUS_P_FileMux_File;

typedef struct NEXUS_P_FileMux_TempFile {
    NEXUS_P_FileMux_File file;
    BLST_D_ENTRY(NEXUS_P_FileMux_TempFile) link;
    bool destroyed; /* if file was destroyed while I/O is in progress  it's marked as destroyed */
} NEXUS_P_FileMux_TempFile;

typedef struct NEXUS_P_FileMux_TempStorage {
    BDBG_OBJECT(NEXUS_P_FileMux_TempStorage)
    NEXUS_FileMuxHandle mux;
    BLST_D_HEAD(NEXUS_P_FileMux_TempFiles, NEXUS_P_FileMux_TempFile) files;
} NEXUS_P_FileMux_TempStorage;

typedef struct NEXUS_P_FileMux_Fnrt {
    BMUXlib_FNRT_Handle hFnrt;
    unsigned numVideoTracks;
    unsigned numAudioTracks;

    struct {
        unsigned output; /* output group ID: one group per output interface */
        BMUXlib_VideoEncoderInterface interface;/* input interface */
    } videoInputs[NEXUS_MAX_MUX_PIDS];
    struct {
        unsigned output; /* output group ID: one group per output interface */
        BMUXlib_AudioEncoderInterface interface;/* input interface */
    } audioInputs[NEXUS_MAX_MUX_PIDS];

    struct {
        unsigned track; /* track ID */
        BMUXlib_VideoEncoderInterface interface; /* output interface */
    } videoOutputs[NEXUS_MAX_MUX_PIDS];
    struct {
        unsigned track;
        BMUXlib_AudioEncoderInterface interface;
    } audioOutputs[NEXUS_MAX_MUX_PIDS];
} NEXUS_P_FileMux_Fnrt;

#define NEXUS_FILE_MUX_P_MAX_DESCRIPTORS  16
struct NEXUS_FileMux {
    BDBG_OBJECT(NEXUS_FileMux)
    NEXUS_TimerHandle muxTimer;
    NEXUS_FileMuxCreateSettings createSettings;
    NEXUS_FileMuxStartSettings startSettings;
    NEXUS_TaskCallbackHandle finishedCallback;
    NEXUS_P_FileMux_TempStorage tempStorage;
    NEXUS_P_FileMux_File outputFile;
    NEXUS_P_FileMux_Fnrt fnrt;
    struct {
        BMUXlib_File_MP4_Handle mux;
        bool active;
    } mp4;
#if B_HAS_ASF
    struct {
        BMUXlib_File_ASF_Handle mux;
        bool active;
    } asf;
#endif
    bool stopping;
    bool started;
    bool ioWaitingThreshold;
    unsigned duration;
};

typedef struct NEXUS_FileMux_P_State {
    NEXUS_ModuleHandle module;
    NEXUS_FileMuxModuleSettings config;
} NEXUS_FileMux_P_State;

extern NEXUS_FileMux_P_State g_NEXUS_FileMux_P_State;
#define NEXUS_MODULE_NAME file_mux
#define NEXUS_MODULE_SELF g_NEXUS_FileMux_P_State.module


#ifdef __cplusplus
}
#endif


#endif /* NEXUS_FILE_MUX_MODULE_H__ */

