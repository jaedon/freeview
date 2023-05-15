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
* $brcm_Workfile: nexus_client_prologue.h $
* $brcm_Revision: 10 $
* $brcm_Date: 8/31/12 2:54p $
*
* API Description:
*   API name: Platform (private)
*    Common part of all kernel servers
*
* Revision History:
*
* $brcm_Log: /nexus/base/include/linuxuser/client/nexus_client_prologue.h $
* 
* 10   8/31/12 2:54p erickson
* SW7425-3418: rework user mode callbacks: filter redundant callbacks on
*  server. sync NEXUS_StopCallbacks across all callback threads.
* 
* 9   7/17/12 11:11a erickson
* SW7435-235: refactor directories to remove symbolic links
* 
* 8   9/8/11 8:49a erickson
* SW7420-1123: clean shutdown of clients, fix call to
*  NEXUS_Base_Core_Uninit, improve client BDBG_ERR's to direct user to
*  look at server console
* 
* 7   3/25/11 12:10p erickson
* SW7420-1123: server must pass heap status to avoid initial error from
*  NEXUS_Heap_GetStatus
*
* 6   2/16/11 2:17p erickson
* SW7420-1123: implement nelem/nelem_out for IPC thunk
*
* 5   2/15/11 4:00p erickson
* SW7420-1491: NEXUS_THUNK_LAYER
*
* 4   2/15/11 2:27p erickson
* SW7420-1123: refactor IPC thunk to separate in/out params
*
* 3   1/21/11 3:11p erickson
* SW7420-1123: use NEXUS_MAX_HEAPS for all client/server heap
*  communication
*
* 2   1/19/11 2:03p erickson
* SW7420-1123: socket-based usermode IPC
*
* 1   9/29/10 9:34a erickson
* SW7420-1123: add linux usermode IPC
*
***************************************************************************/

#ifndef _NEXUS_CLIENT_PROLOGUE_H_
#define _NEXUS_CLIENT_PROLOGUE_H_

#include "nexus_types.h"
#include "../server/nexus_ipc_api.h"
#include "priv/nexus_core.h"

typedef struct NEXUS_P_ClientModule *NEXUS_P_ClientModuleHandle;

NEXUS_Error NEXUS_P_Client_LockModule(NEXUS_P_ClientModuleHandle module, void **data, unsigned *size);
void NEXUS_P_Client_UnlockModule(NEXUS_P_ClientModuleHandle module);
NEXUS_Error NEXUS_P_Client_CallServer(NEXUS_P_ClientModuleHandle module, const void *in_params, unsigned in_param_size, void *out_params, unsigned out_param_mem, unsigned *p_out_param_size);

#endif /* _NEXUS_CLIENT_PROLOGUE_H_ */
