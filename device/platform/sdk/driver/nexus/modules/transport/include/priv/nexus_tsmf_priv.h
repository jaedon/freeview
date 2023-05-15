/***************************************************************************
 *     (c)2007-2012 Broadcom Corporation
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
 * $brcm_Workfile: nexus_tsmf_priv.h $
 * $brcm_Revision: 3 $
 * $brcm_Date: 10/8/12 4:29p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/transport/include/priv/nexus_tsmf_priv.h $
 * 
 * 3   10/8/12 4:29p jtna
 * SW7425-3782: use NEXUS_FrontendConnector for TSMF
 * 
 * 2   10/3/12 5:02p jtna
 * SW7425-3782: refactor nexus tsmf impl
 * 
 * 1   9/5/12 6:07p jtna
 * SW7425-3782: rework host and demod TSMF impl. both cases now go through
 *  the host API
 * 
 **************************************************************************/
#ifndef NEXUS_TSMF_PRIV_H__
#define NEXUS_TSMF_PRIV_H__

#include "nexus_tsmf.h"

#ifdef __cplusplus
extern "C"
{
#endif


#if NEXUS_HAS_TSMF

unsigned NEXUS_Tsmf_GetNumHandles_priv(void);
NEXUS_TsmfHandle NEXUS_Tsmf_GetHandle_priv(
    unsigned num,
    unsigned *hwIndex /* [out] */
    );

bool NEXUS_Tsmf_TestFrontendInput_priv(NEXUS_TsmfHandle tsmf, NEXUS_FrontendConnectorHandle frontend);
NEXUS_Error NEXUS_Tsmf_SetOutput_priv(void *parserBandHandle);

bool NEXUS_Tsmf_GetPending_priv(NEXUS_TsmfHandle tsmf);
void NEXUS_Tsmf_SetPending_priv(NEXUS_TsmfHandle tsmf, bool pending);

#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
