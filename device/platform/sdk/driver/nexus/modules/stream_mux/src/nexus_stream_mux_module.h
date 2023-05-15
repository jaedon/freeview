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
 * $brcm_Workfile: nexus_stream_mux_module.h $
 * $brcm_Revision: 12 $
 * $brcm_Date: 9/6/12 9:49a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/stream_mux/src/nexus_stream_mux_module.h $
 * 
 * 12   9/6/12 9:49a hongtaoz
 * SW7425-3425: added FNRT support to stream mux;
 * 
 * 11   8/16/12 3:28p vsilyaev
 * SW7425-3684: Added duration of muxed portion of stream/file
 * 
 * 10   7/24/12 1:40p vsilyaev
 * SW7425-3540: Use static allocation for large data structures
 * 
 * 9   7/24/12 1:02p vsilyaev
 * SW7425-3540: Use static allocation for large data structures
 * 
 * 8   6/19/12 12:43p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 7   11/29/11 4:28p vsilyaev
 * SW7425-1853: Don't place BMUXlib_TS_StartSettings on stack
 * 
 * 6   2/10/11 1:08p vsilyaev
 * SW7425-39: Added code to notify application about 'finish' event
 * 
 * 5   12/28/10 5:45p vsilyaev
 * SW7425-39: Open Audio/Video PidChannel inside the stream mux module
 * 
 * 4   12/23/10 3:25p vsilyaev
 * SW7425-39: Start/stop playpump inside mux
 * 
 * 3   12/22/10 6:48p vsilyaev
 * SW7425-39: Updated MUX module
 * 
 * 2   12/16/10 7:42p vsilyaev
 * SW7425-39: Added video/transport interfaces
 * 
 * 1   12/9/10 8:08p vsilyaev
 * SW7425-39: Mux module
 * 
 *
 **************************************************************************/
#ifndef NEXUS_STREAM_MUX_MODULE_H__
#define NEXUS_STREAM_MUX_MODULE_H__

#include "nexus_base.h"
#include "nexus_stream_mux_thunks.h"
#include "nexus_stream_mux.h"
#include "nexus_stream_mux_init.h"
#include "priv/nexus_playpump_priv.h"
#include "priv/nexus_tsmux_priv.h"

#include "priv/nexus_core.h"
#include "bmuxlib_ts.h"
#include "bmuxlib_fnrt.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NEXUS_P_StreamMux_Fnrt {
    BMUXlib_FNRT_Handle hFnrt;
    unsigned numVideoPids;
    unsigned numAudioPids;

    struct {
        unsigned output; /* output group ID: one group per output interface */
        BMUXlib_VideoEncoderInterface interface;/* input interface */
    } videoInputs[NEXUS_MAX_MUX_PIDS];
    struct {
        unsigned output; /* output group ID: one group per output interface */
        BMUXlib_AudioEncoderInterface interface;/* input interface */
    } audioInputs[NEXUS_MAX_MUX_PIDS];

    struct {
        unsigned pid; /* PID */
        unsigned pesId; /* PES PID */
        NEXUS_PlaypumpHandle playpump;
        BMUXlib_VideoEncoderInterface interface; /* output interface */
    } videoOutputs[NEXUS_MAX_MUX_PIDS];
    struct {
        unsigned pid; /* PID */
        unsigned pesId; /* PES PID */
        NEXUS_PlaypumpHandle playpump;
        BMUXlib_AudioEncoderInterface interface;
    } audioOutputs[NEXUS_MAX_MUX_PIDS];
} NEXUS_P_StreamMux_Fnrt;

#define NEXUS_STREAM_MUX_P_MAX_DESCRIPTORS  16
struct NEXUS_StreamMux {
    NEXUS_OBJECT(NEXUS_StreamMux);
    BMUXlib_TS_Handle mux;
    NEXUS_TimerHandle muxTimer;
    NEXUS_TsMuxHandle tsMux;
    NEXUS_P_StreamMux_Fnrt fnrt;
    NEXUS_StreamMuxCreateSettings createSettings;
    NEXUS_StreamMuxStartSettings startSettings;
    BMUXlib_TS_StartSettings muxStartSettings;
    NEXUS_TaskCallbackHandle finishedCallback;
    bool started;
    unsigned completedDuration;
};
NEXUS_OBJECT_CLASS_DECLARE(NEXUS_StreamMux);

typedef struct NEXUS_StreamMux_P_State {
    NEXUS_ModuleHandle module;
    NEXUS_StreamMuxModuleSettings config;
    BPVRlib_Feed_ExtendedOffsetEntry entries[NEXUS_STREAM_MUX_P_MAX_DESCRIPTORS];
    /* instead of allocating large data structures on the stack, they are allocated  in the static data (it's safe since module is serialized)
       data is kept in union with entry per function and we use 'ccokie' to verify that union was not stoled */
    union {
        struct {
            void (*cookie)(NEXUS_StreamMuxStartSettings *);
            BMUXlib_TS_StartSettings muxStartSettings;
        } NEXUS_StreamMux_GetDefaultStartSettings;
        struct {
            void (*cookie)(NEXUS_StreamMuxConfiguration *);
            BMUXlib_TS_StartSettings startSettings;
            BMUXlib_TS_MuxSettings muxSettings;
        } NEXUS_StreamMux_GetDefaultConfiguration;
        struct {
            void (*cookie)(const NEXUS_StreamMuxConfiguration *, NEXUS_StreamMuxMemoryConfiguration *);
            BMUXlib_TS_MuxConfig muxConfig;
            BMUXlib_TS_MemoryConfig memoryConfig;
        } NEXUS_StreamMux_GetMemoryConfiguration;
    } functionData;
} NEXUS_StreamMux_P_State;

extern NEXUS_StreamMux_P_State g_NEXUS_StreamMux_P_State;
#define NEXUS_MODULE_NAME stream_mux
#define NEXUS_MODULE_SELF g_NEXUS_StreamMux_P_State.module


#ifdef __cplusplus
}
#endif


#endif /* NEXUS_STREAM_MUX_MODULE_H__ */

