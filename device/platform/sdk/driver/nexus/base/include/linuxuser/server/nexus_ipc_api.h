/***************************************************************************
*     (c)2004-2012 Broadcom Corporation
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
* $brcm_Workfile: nexus_ipc_api.h $
* $brcm_Revision: 7 $
* $brcm_Date: 8/31/12 3:27p $
*
* API Description:
*   API name: Platform (private)
*    Common part of all kernel servers
*
* Revision History:
*
* $brcm_Log: /nexus/base/include/linuxuser/server/nexus_ipc_api.h $
* 
* 7   8/31/12 3:27p erickson
* SW7425-3418: rename nexus_socket functions to avoid linker collision
*  with os lib
* 
* 6   8/31/12 2:54p erickson
* SW7425-3418: rework user mode callbacks: filter redundant callbacks on
*  server. sync NEXUS_StopCallbacks across all callback threads.
* 
* 5   7/30/12 8:53a erickson
* SW7435-239: Make the Nexus IPC UNIX domain socket location accessible
*  to applications running on LXC
* 
* 4   4/18/11 1:47p erickson
* SW7420-1123: use void* for intermediate device offset to avoid strict
*  aliasing warnings
* 
* 3   3/25/11 12:10p erickson
* SW7420-1123: server must pass heap status to avoid initial error from
*  NEXUS_Heap_GetStatus
*
* 2   1/19/11 2:03p erickson
* SW7420-1123: socket-based usermode IPC
*
* 1   9/29/10 9:34a erickson
* SW7420-1123: add linux usermode IPC
*
***************************************************************************/

#ifndef _NEXUS_IPC_API_H_
#define _NEXUS_IPC_API_H_

#include "nexus_platform_client.h"

#define NEXUS_IPC_ID(module, id) (id)
typedef struct NEXUS_Ipc_Header {
   unsigned version;
   unsigned client_id;
   unsigned function_id;
   int result; /* ipc system rc, not the function's rc */
} NEXUS_Ipc_Header;

/**
Intermediate physical addressed used to translate addresses between processes.
Instead of using a generic type, we use a typedef so that the auto-gen API is clear what is a translated address.
**/
typedef void *NEXUS_Ipc_DeviceAddress;

#define B_UNIX_DIR "/shared"
#define B_UNIX_MAIN_SOCKET_STR "/shared/nexus_multiprocess"
#define B_UNIX_SOCKET_FORMAT_STR "/shared/nexus_module%d"
#define B_UNIX_SCHEDULER_SOCKET_FMT_STR "/shared/nexus_scheduler%d"

struct nexus_callback_data
{
    void *interface;
    NEXUS_CallbackDesc callback;
};

/* manual IPC on main socket */
enum nexus_main_socket_message_type
{
    nexus_main_socket_message_type_disconnect,      /* client sends this for clean disconnect. no read. */
    nexus_main_socket_message_type_stop_callbacks,  /* client sends void *interface, then reads back lastStopCallbacksCount for sync. */
    nexus_main_socket_message_type_start_callbacks, /* client sends void *interface. no read. */
    nexus_main_socket_message_type_max
};

/* queue size in server and client. not required to be the same, but efficient. */
#define NEXUS_CALLBACK_QUEUE_SIZE 32

/* data passed from server to client on initialization */
struct nexus_client_init_data
{
    NEXUS_ClientConfiguration config;
    struct {
        /* client cannot call NEXUS_Heap_GetStatus to populate initial heap information */
        NEXUS_HeapHandle heap; /* must match config.heaps[] */
        unsigned offset;
        unsigned size;
        NEXUS_MemoryType memoryType;
    } heap[NEXUS_MAX_HEAPS];
};

/* nexus_socket.c */
ssize_t b_nexus_read(int fd, void *buf, size_t buf_size);
ssize_t b_nexus_write(int fd, const void *buf, size_t buf_size);
int b_nexus_socket_close(int fd);

#endif /* _NEXUS_IPC_API_H_ */
