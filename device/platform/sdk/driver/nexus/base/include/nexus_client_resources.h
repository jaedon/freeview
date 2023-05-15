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
 * $brcm_Workfile: nexus_client_resources.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 8/2/12 3:07p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/base/include/nexus_client_resources.h $
 * 
 * 2   8/2/12 3:07p erickson
 * SW7425-2734: disable NEXUS_CLIENT_RESOURCES if NEXUS_SERVER_SUPPORT=n
 * 
 * 1   8/1/12 4:03p erickson
 * SW7425-2734: allow server to limit resources for untrusted clients
 * 
 ************************************************************/
#ifndef B_NEXUS_CLIENT_RESOURCES_H__
#define B_NEXUS_CLIENT_RESOURCES_H__

/* 
untrusted client resource limits
*/

/* set NEXUS_CLIENT_RESOURCES_DISABLED to 1 to turn off these limits */
#if !NEXUS_SERVER_SUPPORT
#define NEXUS_CLIENT_RESOURCES_DISABLED 1
#endif

/*
acquire/release macros:
    NAME must be a member of NEXUS_ClientResources
    supported values of TYPE are Count, IdList
    ID is a don't care for TYPE Count. 
    NEXUS_ANY_ID is not supported; use Count instead.
*/
#if NEXUS_CLIENT_RESOURCES_DISABLED
#define NEXUS_CLIENT_RESOURCES_ACQUIRE(NAME,TYPE,ID) (0)
#define NEXUS_CLIENT_RESOURCES_RELEASE(NAME,TYPE,ID) do { } while (0)
#else
#define NEXUS_CLIENT_RESOURCES_ACQUIRE(NAME,TYPE,ID) nexus_client_resources_##TYPE##_acquire(#NAME,offsetof(NEXUS_ClientResources,NAME),ID)
#define NEXUS_CLIENT_RESOURCES_RELEASE(NAME,TYPE,ID) nexus_client_resources_##TYPE##_release(#NAME,offsetof(NEXUS_ClientResources,NAME),ID)

int nexus_client_resources_Count_acquire(const char *name, unsigned offset, unsigned id);
void nexus_client_resources_Count_release(const char *name, unsigned offset, unsigned id);
int nexus_client_resources_IdList_acquire(const char *name, unsigned offset, unsigned id);
void nexus_client_resources_IdList_release(const char *name, unsigned offset, unsigned id);
#endif

#endif /* B_NEXUS_CLIENT_RESOURCES_H__ */
