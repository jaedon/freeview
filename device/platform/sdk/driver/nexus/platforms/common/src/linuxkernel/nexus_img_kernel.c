/***************************************************************************
 *     (c)2006-2012 Broadcom Corporation
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
 * $brcm_Workfile: nexus_img_kernel.c $
 * $brcm_Revision: 11 $
 * $brcm_Date: 7/5/12 7:44a $
 *
 * Module Description:
 * driver side of the Image Download interface
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/platforms/97400/src/linuxkernel/nexus_img_kernel.c $
 * 
 * 11   7/5/12 7:44a erickson
 * SW7425-3387: remove unused code
 * 
 * 10   9/13/11 10:51a erickson
 * SW7420-1148: add BDBG_OBJECT protection for system debug
 * 
 * 9   6/1/11 10:08a erickson
 * SW7358-46: add started event so that we can ensure proxy's IMG thread
 *  is in ioctl before NEXUS_Platform_Init is called
 * 
 * 8   5/3/11 5:09p erickson
 * SW7420-1819: repartition so NFE is a standard feature
 * 
 * 7   4/26/11 11:41a jgarrett
 * SW7425-437: Adding BDBG_OBJECT
 * 
 * 6   4/19/11 5:42p nickh
 * SW7425-365: Add 2.6.37 kernel support
 * 
 * 5   9/23/10 2:24p erickson
 * SW7420-943: refactor for driver/firmware partitioning
 * 
 * 4   9/11/09 9:56a erickson
 * SW7420-228: switch to autoconf.h
 *
 * 3   4/7/09 1:11p vsilyaev
 * PR 53916: Copy data from the user to kernel buffer in the proper
 *  context
 *
 * 2   2/6/09 10:25a erickson
 * PR51882: fix warning
 *
 * 1   8/21/08 3:17p katrep
 * PR45527: NEXUS_CONFIG_IMAGE implementation for runtime FW download
 *
 ***************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "nexus_generic_driver_impl.h"
#include "nexus_img_kernel.h"
#include "nexus_img_ioctl.h"
#include "blst_slist.h"

BDBG_MODULE(nexus_img_kernel);

BDBG_OBJECT_ID(BIMG_Driver);
typedef struct BIMG_Driver {
    BDBG_OBJECT(BIMG_Driver);
    BLST_S_ENTRY(BIMG_Driver)  link;
    const char *name;
    void *image; /* currently opened image */
}BIMG_Driver;


BLST_S_HEAD(BIMG_InterfaceList, BIMG_Driver);

BDBG_OBJECT_ID(NEXUS_ImgState);
static struct NEXUS_ImgState {
    BDBG_OBJECT(NEXUS_ImgState)
    struct BIMG_InterfaceList active;
    bool ready;
    bool stopped;
    BKNI_EventHandle started;
    BKNI_EventHandle req;
    BKNI_EventHandle ack;
    BIMG_Ioctl ioctl;
    uint8_t data[64*1024]; /* space to hold kernel copy of data */
} b_interfaces;

int
nexus_img_interfaces_init(void)
{
    BDBG_MSG(("initializing kernel image interface"));
    BDBG_OBJECT_SET(&b_interfaces, NEXUS_ImgState);
    BLST_S_INIT(&b_interfaces.active);
    BKNI_CreateEvent(&b_interfaces.started);
    BKNI_CreateEvent(&b_interfaces.req);
    BKNI_CreateEvent(&b_interfaces.ack);
    b_interfaces.ready = false;
    b_interfaces.stopped = false;
    return 0;
}

void
nexus_img_interfaces_shutdown(void)
{
    BIMG_Driver *iface;

    BDBG_OBJECT_ASSERT(&b_interfaces, NEXUS_ImgState);
    while((iface=BLST_S_FIRST(&b_interfaces.active))!=NULL) {
        BDBG_OBJECT_ASSERT(iface, BIMG_Driver);
        BDBG_MSG(("removing %#x", (unsigned)iface));
        BLST_S_REMOVE_HEAD(&b_interfaces.active, link);
        BDBG_OBJECT_DESTROY(iface, BIMG_Driver);
        BKNI_Free(iface);
    }
    /* interface shutdown called at unload time, at this time shall be no application which are sleeping in the kernel, and waiting for the event */
    BKNI_DestroyEvent(b_interfaces.req);
    BKNI_DestroyEvent(b_interfaces.ack);
    BKNI_DestroyEvent(b_interfaces.started);
    b_interfaces.ready = false;
    BDBG_OBJECT_UNSET(&b_interfaces, NEXUS_ImgState);
    return;
}

int nexus_img_wait_until_ready(void)
{
    BDBG_OBJECT_ASSERT(&b_interfaces, NEXUS_ImgState);
    /* wait until proxy's IMG thread is pending in the ioctl.
    if it times out, then we fail because NEXUS_Platform_Init would likely fail on FW loading. */
    int rc = BKNI_WaitForEvent(b_interfaces.started, 4000);
    if (rc) return BERR_TRACE(rc);
    return 0;
}

void *
Nexus_IMG_Driver_Create(const char *id)
{
    BIMG_Driver *iface;

    if (b_strlen(id)>=BIMG_ID_MAX_LEN) {
        BDBG_ERR(("length of the string '%s' exceeds %d", id, BIMG_ID_MAX_LEN));
        return NULL;
    }
    iface = BKNI_Malloc(sizeof(*iface));
    if (!iface) {
        BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        return NULL;
    }
    iface->name = id;
    BLST_S_INSERT_HEAD(&b_interfaces.active, iface, link);
    BDBG_MSG(("adding %#x(%s)", (unsigned)iface, iface->name));
    iface->image = NULL;
    BDBG_OBJECT_SET(iface, BIMG_Driver);

    return iface;
}

void
Nexus_IMG_Driver_Destroy(void *interface)
{
    BIMG_Driver *iface=interface;
    BDBG_OBJECT_ASSERT(iface, BIMG_Driver);
    BDBG_MSG(("-removing %#x(%s)", (unsigned)iface, iface->name));
    BLST_S_REMOVE(&b_interfaces.active, iface, BIMG_Driver, link);
    BDBG_MSG(("+removing %#x(%s)", (unsigned)iface, iface->name));
    BDBG_OBJECT_DESTROY(iface, BIMG_Driver);
    BKNI_Free(iface);
    return;
}


static BERR_Code
b_send_req(BIMG_Driver *iface, BIMG_Ioctl_Req_Type type)
{
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(iface, BIMG_Driver);
    BDBG_OBJECT_ASSERT(&b_interfaces, NEXUS_ImgState);

    /* check that something waits in the ioctl */
    if (!b_interfaces.ready) {
        /* nexus_img_wait_until_ready should prevent this from happening */
        return BERR_TRACE(BERR_OS_ERROR); /* ioctl is not pending */
    }
    b_interfaces.ioctl.req.req_type = type;
    b_interfaces.ioctl.ack.result = -1;
    b_strncpy(b_interfaces.ioctl.req.id, iface->name, BIMG_ID_MAX_LEN-1);
    b_interfaces.ioctl.req.id[BIMG_ID_MAX_LEN-1] = '\0';
    BDBG_MSG(("+req_type %d, id '%s'", type, iface->name));
    BKNI_ResetEvent(b_interfaces.ack); /* prevent false positives */
    BKNI_SetEvent(b_interfaces.req); /* wakeup process which waits in the  ioctl */
    BDBG_MSG(("waiting for ack"));
    rc = BKNI_WaitForEvent(b_interfaces.ack, 2000); /* wait for application to return */
    if (rc!=BERR_SUCCESS) {
            return BERR_TRACE(rc);
    }
    BDBG_MSG(("+req_type %d, id '%s' rc=%d", type, iface->name, b_interfaces.ioctl.ack.result));
    /* check return code from the user space */
    if (b_interfaces.ioctl.ack.result!=BERR_SUCCESS) {
        if ( b_interfaces.ioctl.ack.result == BERR_INVALID_PARAMETER ) {
            /* This error is common on 740x platforms.  Silently return. */
            BDBG_MSG(("Invalid Parameter Error returned.  Firmware image not available?"));
            return b_interfaces.ioctl.ack.result;
        } else {
            return BERR_TRACE(b_interfaces.ioctl.ack.result);
        }
    }

    return BERR_SUCCESS;
}

int
nexus_img_ioctl(unsigned int cmd, unsigned long arg)
{
    int result;
    BERR_Code rc;
    
    BDBG_OBJECT_ASSERT(&b_interfaces, NEXUS_ImgState);

    result = copy_from_user_small(&b_interfaces.ioctl, (void *)arg, sizeof(b_interfaces.ioctl));
    /* coverity[dead_error_condition] - it's important to do this check in case it's not a dead condition in the future. */
    if (result) {
        return BERR_TRACE(result);
    }
    b_interfaces.ready = true;
    BDBG_MSG(("<req_type:[%s] %d", b_interfaces.ioctl.req.id, b_interfaces.ioctl.req.req_type));
    switch(b_interfaces.ioctl.req.req_type) {
    case BIMG_Ioctl_Req_Type_Next:
        if(b_interfaces.ioctl.ack.result == BERR_SUCCESS) { /* copy data to the private buffer while in the right context */
            unsigned length = b_interfaces.ioctl.req.data.next.length;
            if(length <= sizeof(b_interfaces.data)) {
                result = copy_from_user_small(b_interfaces.data, b_interfaces.ioctl.ack.data.next.data, length);
                if (result == length) {
                    /* If you ask copy_from_user to copy more data than is available it will */
                    /* return the number of bytes uncopied. */
                    /* We only flag an error if uncopied bytes is how many we asked to be copied */
                    BDBG_WRN(("couldn't copy %u bytes", length));
                    b_interfaces.ioctl.ack.result = BERR_OS_ERROR;
                }
            } else {
                b_interfaces.ioctl.ack.result = BERR_TRACE(BERR_OS_ERROR);
            }
        }
        break;
    case BIMG_Ioctl_Req_Type_Start:
        b_interfaces.stopped = false;
        BKNI_SetEvent(b_interfaces.started);
        BDBG_MSG(("start req"));
        break;
    case BIMG_Ioctl_Req_Type_Exit:
        b_interfaces.stopped = true;
        BDBG_MSG(("exit req"));
        BKNI_SetEvent(b_interfaces.req); /* set an event */
        return 0; /* return right the way */
    default:
        break; /* do nothing */
    }
    BKNI_SetEvent(b_interfaces.ack); /* set an event */
    for(;;) {
        BDBG_MSG(("wating for command"));
        rc =BKNI_WaitForEvent(b_interfaces.req, 4*1000);
        if (rc==BERR_TIMEOUT) {
            BDBG_MSG(("interface timer"));
            continue;
        } else if (rc!=BERR_SUCCESS) {
            return rc;
        }
        if (b_interfaces.stopped) {
            BDBG_MSG(("stop req"));
            return -1;
        }
        b_interfaces.ready = false;
        BDBG_MSG((">req_type[%s] %d", b_interfaces.ioctl.req.id, b_interfaces.ioctl.req.req_type));
        result = copy_to_user_small((void*)arg,&b_interfaces.ioctl,sizeof(b_interfaces.ioctl));
        if (result) {
            return BERR_TRACE(result);
        }
        break;
    }
    return 0;
}

/*
* because all calls to the magnum PI shall be  synchronized, we could
* assume that all calls to the Image interface are synchronized as well.
* As result each call (open, close and next) could expects that the user application
* waits for the ioctl, if it doesn't error returned right the way.
* This should help with kernel waiting for application which went bye-bye.
*/
BERR_Code
Nexus_IMG_Driver_Open(void *context, void **image, unsigned image_id)
{
    BERR_Code rc;
    BIMG_Driver *iface = context;

    BDBG_OBJECT_ASSERT(iface, BIMG_Driver);

    BDBG_MSG(("open> context %#p'%s' id %u", iface, iface->name, image_id));
    if (iface->image!=NULL) {
        /* open shall be matched with close */
        return BERR_TRACE(BERR_OS_ERROR);
    }
    b_interfaces.ioctl.req.data.open.image_id = image_id;
    rc = b_send_req(iface, BIMG_Ioctl_Req_Type_Open);
    if (rc!=BERR_SUCCESS) {
        if ( rc == BERR_INVALID_PARAMETER ) {
            /* This error is common on 740x platforms.  Silently return. */
            BDBG_MSG(("Invalid Parameter Error returned.  Firmware image not available?"));
            return rc;
        } else {
            return BERR_TRACE(rc);
        }
    }
    iface->image = b_interfaces.ioctl.ack.data.open.image;
    BDBG_MSG(("open< context %#p'%s' id %u image %p", iface, iface->name, image_id, iface->image));
    *image = iface;

    return BERR_SUCCESS;
}

BERR_Code
Nexus_IMG_Driver_Next(void *image, unsigned chunk, const void **data, uint16_t length)
{
    BERR_Code rc;
    BIMG_Driver *iface = image;
    BDBG_OBJECT_ASSERT(iface, BIMG_Driver);

    BDBG_ASSERT(data);
    *data = NULL;
    if (iface->image==NULL) {
        return BERR_TRACE(BERR_OS_ERROR);
    }
    BDBG_MSG(("next> image %#p'%s' chunk %u length %u", iface, iface->name, chunk, (unsigned)length));
    b_interfaces.ioctl.req.data.next.chunk = chunk;
    b_interfaces.ioctl.req.data.next.length = length;
    b_interfaces.ioctl.req.data.next.image = iface->image;

    rc = b_send_req(iface, BIMG_Ioctl_Req_Type_Next);
    if (rc!=BERR_SUCCESS) {
        return BERR_TRACE(rc);
    }

    BDBG_MSG(("next< image %#p'%s' chunk %u length %u data %p", iface, iface->name, chunk, (unsigned)length, b_interfaces.ioctl.ack.data.next.data));
    /* data was already copied in the ioctl handler */
    *data = b_interfaces.data;
    return BERR_SUCCESS;
}

void
Nexus_IMG_Driver_Close(void *image)
{
    BIMG_Driver *iface = image;
    BERR_Code rc;
    BDBG_OBJECT_ASSERT(iface, BIMG_Driver);

    BDBG_MSG(("close> image %#p'%s'", iface, iface->name));
    if (iface->image==NULL) {
        return;
    }
    rc = b_send_req(iface, BIMG_Ioctl_Req_Type_Close);
    BDBG_MSG(("close< image %#p'%s'", iface, iface->name));
    iface->image = NULL;
    return;
}

