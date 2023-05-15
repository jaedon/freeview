/******************************************************************************
 *    (c)2011 Broadcom Corporation
 *
 * This program is the proprietary software of Broadcom Corporation and/or its licensors,
 * and may only be used, duplicated, modified or distributed pursuant to the terms and
 * conditions of a separate, written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 * no license (express or implied), right to use, or waiver of any kind with respect to the
 * Software, and Broadcom expressly reserves all rights in and to the Software and all
 * intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 * NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 * secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 * AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 * WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 * THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 * USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 * LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 * EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 * ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: bipc_server.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 10/4/11 5:44p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/lib/ipc/bipc_server.h $
 * 
 * 1   10/4/11 5:44p vsilyaev
 * SW7425-1364: Reference applicaion IPC and reference server
 * 
 *****************************************************************************/
#ifndef BIPC_SERVER_H__
#define BIPC_SERVER_H__

#include "bipc.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct bipc_server_descriptor {
    bipc_interface_descriptor interface;
    unsigned constructor_entry;
    unsigned destructor_entry;
    int (*process)(void **instance, unsigned entry, void *buf, size_t recv_size, size_t *send_offset, size_t *send_size);
} bipc_server_descriptor;

typedef struct bipc_server_create_settings {
    unsigned max_instances;
    const bipc_server_descriptor * const *interfaces;
    size_t interface_count;
} bipc_server_create_settings;

typedef struct bipc_server_client_create_settings {
    int recv_fd;
    int send_fd;
    void *ipc_context;
} bipc_server_client_create_settings;

typedef struct bipc_server_client *bipc_server_client_t;

void bipc_server_get_default_create_settings(bipc_server_create_settings *create_settings);
void bipc_server_get_default_client_create_settings(bipc_server_client_create_settings *create_settings);

bipc_t bipc_server_create(const bipc_server_create_settings *create_settings);
bipc_server_client_t bipc_server_client_create(bipc_t ipc, bipc_server_client_create_settings *create_settings);
void bipc_server_client_destroy(bipc_t ipc, bipc_server_client_t client);

int bipc_server_client_process(bipc_t ipc, bipc_server_client_t client);
void bipc_server_destroy(bipc_t ipc);

#ifdef __cplusplus
}
#endif

#endif /* BIPC_SERVER)H__ */

