/***************************************************************************
*     (c)2004-2011 Broadcom Corporation
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
* $brcm_Workfile: nexus_client_body.h $
* $brcm_Revision: 3 $
* $brcm_Date: 2/16/11 2:17p $
*
* API Description:
*   API name: Platform (private)
*    Common part of all kernel servers
*
* Revision History:
*
* $brcm_Log: /nexus/base/include/linuxuser/client/nexus_client_body.h $
* 
* 3   2/16/11 2:17p erickson
* SW7420-1123: implement nelem/nelem_out for IPC thunk
*
* 2   1/19/11 2:03p erickson
* SW7420-1123: socket-based usermode IPC
*
* 1   9/29/10 9:34a erickson
* SW7420-1123: add linux usermode IPC
*
*
***************************************************************************/


#if 0
#define NEXUS_IPC_ENTER(name) BDBG_WRN((">>%s", #name))
#define NEXUS_IPC_LEAVE(name) BDBG_WRN(("<<%s", #name))
#else
#define NEXUS_CLIENT_ENTER(f)
#define NEXUS_CLIENT_LEAVE(f)
#endif

static NEXUS_P_ClientModuleHandle nexus_client_module_state;

NEXUS_Error
nexus_client_module_init(NEXUS_P_ClientModuleHandle module)
{
    /* save to global storage so client thunk can get it */
    nexus_client_module_state = module;
    return 0;
}

void
nexus_client_module_uninit(void)
{
    nexus_client_module_state = NULL;
}

