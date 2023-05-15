/***************************************************************************
*     (c)2008-2011 Broadcom Corporation
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
* $brcm_Workfile: nexus_proxy_body.h $
* $brcm_Revision: 8 $
* $brcm_Date: 5/31/11 4:36p $
*
* API Description:
*   API name: Platform (private)
*    Common part of all userspace proxies
*
* Revision History:
*
* $brcm_Log: /nexus/base/include/linuxuser/proxy/nexus_proxy_body.h $
* 
* 8   5/31/11 4:36p erickson
* SW7420-1907: make NEXUS_IOCTL_MODULE_INIT failure message more generic.
*  may have nothing to do with version number.
* 
* 7   4/8/11 11:04a erickson
* SW7420-1671: improve BDBG_ERR
* 
* 6   9/23/10 2:09p erickson
* SW7420-943: improve debug
*
* 5   7/31/08 5:19p vsilyaev
* PR 45137: Initial support for multi application mode
*
* 4   7/24/08 6:09p vsilyaev
* PR 45137: Modified INIT ioctl to pass application type
*
* 3   1/25/08 2:35p vsilyaev
* PR 38682: Added handling of 'attr' to the function arguments
*
* 2   1/24/08 4:32p vsilyaev
* PR 38682: Improved kernel/proxy environment
*
*
***************************************************************************/


#define  NEXUS_PROXY_ENTER(f)
#define  NEXUS_PROXY_LEAVE(f)

static struct {
    int fd;
} nexus_proxy_module_state;

NEXUS_Error
nexus_proxy_module_init(int fd)
{
    int rc;
    PROXY_NEXUS_ModuleInit init;

    init.version = NEXUS_PROXY_MODULE_VERSION;
    rc = ioctl(fd, NEXUS_IOCTL_MODULE_INIT, &init);
    if(rc!=0) {
        /* the immediate failure is being unable to verify the version. but this could be caused by 
        a variety of problems, so return a vanilla message. the specific cause will be issued by the driver. */
        BDBG_ERR(("unable to init driver"));
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    nexus_proxy_module_state.fd = fd;
    return NEXUS_SUCCESS;
}

void
nexus_proxy_module_uninit(void)
{
    nexus_proxy_module_state.fd = -1;
    return;
}

