/***************************************************************************
 *     (c)2011-2012 Broadcom Corporation
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
   3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 *  ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: nexus_input_router_impl.h $
 * $brcm_Revision: 3 $
 * $brcm_Date: 6/19/12 12:43p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/input_router/src/nexus_input_router_impl.h $
 * 
 * 3   6/19/12 12:43p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 2   8/31/11 3:56p erickson
 * SW7420-1993: fix global
 * 
 * 1   8/24/11 9:14a erickson
 * SW7420-1993: add input_router
 * 
 **************************************************************************/
#ifndef NEXUS_INPUT_ROUTER_IMPL_H__
#define NEXUS_INPUT_ROUTER_IMPL_H__

#include "nexus_input_router_module.h"

#ifdef __cplusplus
#error
#endif

struct NEXUS_InputRouter
{
    NEXUS_OBJECT(NEXUS_InputRouter);
    BLST_S_ENTRY(NEXUS_InputRouter) link; /* for g_nexus_inputrouter_list */
    BLST_S_HEAD(clientlist, NEXUS_InputClient) clients;
    unsigned server_id;
};

struct NEXUS_InputClient
{
    NEXUS_OBJECT(NEXUS_InputClient);
    BLST_S_ENTRY(NEXUS_InputClient) link; /* for router->clients */
    NEXUS_InputRouterHandle router;
    unsigned client_id;
    bool acquired;
    NEXUS_TaskCallbackHandle codeAvailableCallback;
    NEXUS_InputRouterClientSettings serverSettings;
    NEXUS_InputClientSettings settings;
    struct {
        NEXUS_InputRouterCode *data; /* size is serverSettings.fifoSize */
        unsigned rptr, wptr;
    } fifo;
};

extern BLST_S_HEAD(nexus_inputrouter_list, NEXUS_InputRouter) g_nexus_inputrouter_list;

#endif
