/******************************************************************************
 *    (c)2011-2012 Broadcom Corporation
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
 * $brcm_Workfile: refsw_session_simple_client.h $
 * $brcm_Revision: 7 $
 * $brcm_Date: 11/27/12 1:39p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/multiprocess/refsw_session_simple_client.h $
 * 
 * 7   11/27/12 1:39p erickson
 * SW7435-421: app IPC should call BKNI_Init
 * 
 * 6   4/20/12 10:08a erickson
 * SW7425-2882: create fully mapped heap and notify using app ipc
 * 
 * 5   1/3/12 3:51p erickson
 * SW7425-1364: allow no argc/argv, no surface client
 * 
 * 4   11/1/11 1:45p vsilyaev
 * SW7420-2114: Clear configuration
 * 
 * 3   10/27/11 2:37p erickson
 * SW7425-1364: rename to brc
 * 
 * 2   10/13/11 2:15p vsilyaev
 * SW7425-1364: Added support for simple decoder
 * 
 * 1   10/5/11 11:22a vsilyaev
 * SW7425-1364: simple function to connect to server
 * 
 *
 *****************************************************************************/
#include "bipc_client.h"
#include "brc_client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ipc_stubs_client.h"

static const bipc_interface_descriptor * const client_interfaces [] = {
    &bipc_brc_client_descriptor
};

#include "refsw_session_client.h"

static struct {
    b_refsw_client client;
    brc_client_t rca_client;
    brc_client_client_info client_info;
} simple_client_state;

static int simple_client_init(const char *appname, NEXUS_ClientAuthenticationSettings *authSettings)
{
    brc_client_client_configuration configuration;
    int rc;

    memset(&configuration, 0, sizeof(configuration));
    simple_client_state.rca_client=NULL;
    
    rc = BKNI_Init();
    if (rc) return rc;
    
    NEXUS_Platform_GetDefaultClientAuthenticationSettings(authSettings);
    if(b_client_connect(&simple_client_state.client)!=0) {
        rc = BERR_TRACE(-1);
        goto error;
    }
    snprintf(configuration.name.string, sizeof(configuration.name.string),"%s",appname?appname:"client");
    configuration.resources.screen.required = true;
#if SESSION_NEED_DECODER
    configuration.resources.audioDecoder = true;
    configuration.resources.videoDecoder = true;
#endif
    configuration.resources.screen.position.width = 720;
    configuration.resources.screen.position.height = 480;
    simple_client_state.rca_client = brc_client_create(simple_client_state.client.ipc, &configuration);
    if(!simple_client_state.rca_client) {
        rc = BERR_TRACE(-1);
        goto error;
    }

    brc_client_get_clientinfo(simple_client_state.rca_client, &simple_client_state.client_info);
    
    authSettings->certificate = simple_client_state.client_info.certificate;
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
    brc_client_destroy(simple_client_state.rca_client);
    b_client_disconnect(&simple_client_state.client);
    simple_client_state.rca_client = NULL;
    BKNI_Uninit();
    return;
}

