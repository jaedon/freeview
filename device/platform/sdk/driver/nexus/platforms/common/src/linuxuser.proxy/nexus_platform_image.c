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
* $brcm_Workfile: nexus_platform_image.c $
* $brcm_Revision: 3 $
* $brcm_Date: 5/3/11 5:09p $
*
* API Description:
*   API name: Platform (private)
*   This file containes private API to implement the B_CONFIG_IMAGE user mode part
*
* Revision History:
*
* $brcm_Log: /nexus/platforms/97400/src/linuxuser.proxy/nexus_platform_image.c $
* 
* 3   5/3/11 5:09p erickson
* SW7420-1819: repartition so NFE is a standard feature
* 
* 2   4/6/11 3:27p erickson
* SW7420-1671: don't assert on ioctl failure
*
* 1   8/21/08 3:17p katrep
* PR45527: NEXUS_CONFIG_IMAGE implementation for runtime FW download
*
*
***************************************************************************/
#include "nexus_types.h"
#include "nexus_base.h"
#include "nexus_platform.h"
#include "nexus_platform_priv_proxy.h"
#include "bimg.h"
#include "blst_list.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>
#include "nexus_driver_ioctl.h"

BDBG_MODULE(nexus_platform_image);

typedef struct BIMG_LinuxEntry {
    BLST_S_ENTRY(BIMG_LinuxEntry)  link;
    const char *name;
    const BIMG_Interface *iface; /* parent interface */
    void *context; /* context of the parent interface */
    void *image; /* opened image */
}BIMG_LinuxEntry;

BLST_S_HEAD(BIMG_InterfaceList, BIMG_LinuxEntry);

static struct {
    struct BIMG_InterfaceList active;
    bool stopped;
} b_interfaces;

NEXUS_Error Nexus_Platform_P_Image_Init(void)
{
    BLST_S_INIT(&b_interfaces.active);
    b_interfaces.stopped = false;
    return NEXUS_SUCCESS;
}

void Nexus_Platform_P_Image_Shutdown(void)
{
    BIMG_LinuxEntry *entry;

    while((entry=BLST_S_FIRST(&b_interfaces.active))!=NULL) {
        BLST_S_REMOVE_HEAD(&b_interfaces.active, link);
        BKNI_Free(entry);
    }
    return ;
}


void Nexus_Platform_P_Image_Stop(int fd, int ioctl_no)
{
    BIMG_Ioctl ctl;
    int rc;

    BKNI_Memset(&ctl, 0, sizeof(ctl));
    ctl.req.req_type =  BIMG_Ioctl_Req_Type_Exit;

    b_interfaces.stopped = true;
    rc = ioctl(fd, ioctl_no, &ctl);
    if (rc) BERR_TRACE(rc);

    return;
}

NEXUS_Error Nexus_Platform_P_Image_Interfaces_Register(const BIMG_Interface *iface, void *context, const char *id)
{
    BIMG_LinuxEntry *entry;

    entry = BKNI_Malloc(sizeof(*entry));
    if (!entry) {
        return BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
    }
    entry->name = id;
    entry->iface = iface;
    entry->context = context;
    entry->image = NULL;
    BLST_S_INSERT_HEAD(&b_interfaces.active, entry, link);
    return NEXUS_SUCCESS;
}

void Nexus_Platform_P_Image_Interfaces_Unregister(const BIMG_Interface *iface, void *context)
{
    BIMG_LinuxEntry *entry;

    for(entry=BLST_S_FIRST(&b_interfaces.active);entry;entry=BLST_S_NEXT(entry, link)) {
        if (entry->iface == iface && entry->context == context) {
            BLST_S_REMOVE(&b_interfaces.active, entry, BIMG_LinuxEntry, link);
            BKNI_Free(entry);
            return;
        }
    }
    return;
}


NEXUS_Error Nexus_Platform_P_Image_Handler(int fd, int ioctl_no)
{
    BIMG_Ioctl ctl;
    int rc;
    BERR_Code mrc;
    BIMG_LinuxEntry *entry;

    BKNI_Memset(&ctl, 0, sizeof(ctl));
    ctl.req.req_type =  BIMG_Ioctl_Req_Type_Start;

    for(;;) {
        rc = ioctl(fd, ioctl_no, &ctl);
        if (b_interfaces.stopped) {
            break;
        }
        if (rc!=0) {
            return BERR_TRACE(NEXUS_UNKNOWN);
        }
        for(entry=BLST_S_FIRST(&b_interfaces.active);entry;entry=BLST_S_NEXT(entry, link)) {
            if (!strcmp(entry->name,ctl.req.id)) {
                break;
            }
        }
        if (!entry) {
            BDBG_ERR(("unknown image ID '%s'", ctl.req.id));
            ctl.ack.result = -1;
            continue;
        }
        switch(ctl.req.req_type) {
        case BIMG_Ioctl_Req_Type_Open:
            BDBG_MSG((">>Open[%s]: %#x %#x", ctl.req.id, (unsigned)entry->context, (unsigned)ctl.req.data.open.image_id));
            mrc = entry->iface->open(entry->context, &ctl.ack.data.open.image, ctl.req.data.open.image_id);
            ctl.ack.result = mrc;
            if (mrc==BERR_SUCCESS) {
                entry->image = ctl.ack.data.open.image;
            }
            BDBG_MSG(("<<Open[%s]: %#x %#x %d %#x", ctl.req.id, (unsigned)entry->context, (unsigned)ctl.req.data.open.image_id, ctl.ack.result, (unsigned)entry->image));
            break;
        case BIMG_Ioctl_Req_Type_Next:
            BDBG_MSG((">>Next[%s]: %#x %#x %u", ctl.req.id, (unsigned)entry->image, (unsigned)ctl.req.data.next.chunk, (unsigned)ctl.req.data.next.length));
            mrc = entry->iface->next(entry->image, ctl.req.data.next.chunk, &ctl.ack.data.next.data, (uint16_t)ctl.req.data.next.length);
            ctl.ack.result = mrc;
            BDBG_MSG(("<<Next[%s]: %#x %#x %u %d %#x", ctl.req.id, (unsigned)entry->image, (unsigned)ctl.req.data.next.chunk, (unsigned)ctl.req.data.next.length, ctl.ack.result, (unsigned)ctl.ack.data.next.data ));
            break;
        case BIMG_Ioctl_Req_Type_Close:
            BDBG_MSG((">>Close[%s]: %#x", ctl.req.id, (unsigned)entry->image));
            entry->iface->close(entry->image);
            entry->image = NULL;
            BDBG_MSG(("<<Close[%s]: %#x", ctl.req.id, (unsigned)entry->image));
            ctl.ack.result = BERR_SUCCESS;
            break;
        }
    }
    return NEXUS_SUCCESS;
}

