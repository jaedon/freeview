/******************************************************************************
 *    (c)2008-2013 Broadcom Corporation
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
 *****************************************************************************/
#ifndef PLATFORM_IPC_SESSION_SIMPLE_CLIENT_H
#define PLATFORM_IPC_SESSION_SIMPLE_CLIENT_H

#include "bipc_client.h"
#include "platform_ipc_client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ipc_stubs_dfb_client.h"

static const bipc_interface_descriptor * const client_interfaces [] = {
    &bipc_brc_client_descriptor
};

#include "platform_ipc_session_client.h"

static struct {
    b_dfb_client client;
    brc_client_t rca_client;
} simple_client_state;

static int simple_client_init(const char *appname)
{
    brc_client_client_configuration configuration;
    int rc;

    memset(&configuration, 0, sizeof(configuration));
    simple_client_state.rca_client=NULL;

    rc = BKNI_Init();
    if (rc) return rc;

    if(b_client_connect(&simple_client_state.client)!=0) {
        rc = BERR_TRACE(-1);
        goto error;
    }
    snprintf(configuration.name.string, sizeof(configuration.name.string),"%s",appname?appname:"client");
    simple_client_state.rca_client = brc_client_create_client(simple_client_state.client.ipc, &configuration);
    if(!simple_client_state.rca_client) {
        rc = BERR_TRACE(-1);
        goto error;
    }
    return 0;

error:
    BKNI_Uninit();
    return rc;
}

static void simple_client_shutdown(void)
{
    if(!simple_client_state.rca_client) {
        return;
    }
    brc_client_destroy_client(simple_client_state.rca_client);
    b_client_disconnect(&simple_client_state.client);
    simple_client_state.rca_client = NULL;
    BKNI_Uninit();
    return;
}
#endif /* PLATFORM_IPC_SESSION_SIMPLE_CLIENT_H */
