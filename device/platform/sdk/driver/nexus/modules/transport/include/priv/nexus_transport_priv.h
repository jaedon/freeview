/***************************************************************************
 *     (c)2007-2011 Broadcom Corporation
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
 * $brcm_Workfile: nexus_transport_priv.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 3/16/11 2:51p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/transport/7400/include/priv/nexus_transport_priv.h $
 * 
 * 1   3/16/11 2:51p erickson
 * SW7422-377: add NEXUS_Transport_GetXpt_priv
 *
 **************************************************************************/
#ifndef NEXUS_TRANSPORT_PRIV_H__
#define NEXUS_TRANSPORT_PRIV_H__

#include "bxpt.h"
#include "nexus_parser_band.h"
#include "nexus_recpump.h"
#include "nexus_playpump.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
If you get the XPT handle, you must first lock the transport module, get the handle,
use the handle, then unlock the transport module. After unlocking the transport module
you may not use the XPT handle.

This is allowed:
    NEXUS_Module_Lock(transportModule);
    {
        BXPT_Handle xpt;
        NEXUS_Transport_GetXpt_priv(&xpt);
        BXPT_Foo(xpt);
    }
    NEXUS_Module_Unlock(transportModule);

This is not allowed:
    BXPT_Handle xpt;
    NEXUS_Module_Lock(transportModule);
    NEXUS_Transport_GetXpt_priv(&xpt);
    NEXUS_Module_Unlock(transportModule);
    BXPT_Foo(xpt); <-- this is a bug

To ensure you are following these rules, we recommend that you do not cache or otherwise store
the XPT handle. Only use a local variable that goes out of scope before unlocking the module (as above).
**/
void NEXUS_Transport_GetXpt_priv(
    BXPT_Handle *pXpt /* [out] */
    );

struct NEXUS_MtsifParserBandSettings {
    unsigned index;
    NEXUS_ParserBandSettings settings;
    struct {
        bool valid;
        unsigned hwIndex;
        bool pending; /* if true, the following settings have changed */
        NEXUS_TsmfSettings settings;
    } tsmf;
};

void NEXUS_ParserBand_GetMtsifConnections_priv(
    NEXUS_FrontendConnectorHandle connector,
    struct NEXUS_MtsifParserBandSettings *pSettings,
    unsigned numEntries,
    unsigned *pNumReturned
    );

void NEXUS_ParserBand_P_MtsifErrorStatus_priv(unsigned lengthError, unsigned transportError);

bool NEXUS_TransportModule_P_IsMtsifEncrypted(void);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
