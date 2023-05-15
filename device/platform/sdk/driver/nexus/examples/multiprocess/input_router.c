/******************************************************************************
 *    (c)2008-2011 Broadcom Corporation
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
 * $brcm_Workfile: input_router.c $
 * $brcm_Revision: 4 $
 * $brcm_Date: 9/13/11 1:24p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/multiprocess/input_router.c $
 * 
 * 4   9/13/11 1:24p erickson
 * SW7420-1148: change client modes to unprotected/protected/untrusted
 * 
 * 3   8/29/11 3:55p erickson
 * SW7420-1993: add #if NEXUS_HAS_KEYPAD
 * 
 * 2   8/26/11 11:32a erickson
 * SW7420-1993: add linux USB mouse/keyboard support. add keypad. add
 *  client-based filter options.
 * 
 * 1   8/24/11 9:13a erickson
 * SW7420-1993: add input_router
 * 
 *****************************************************************************/

#include "nexus_platform.h"
#if NEXUS_HAS_INPUT_ROUTER
#include "nexus_input_router.h"
#endif
#include "nexus_ir_input.h"
#if NEXUS_HAS_KEYPAD
#include "nexus_keypad.h"
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h> /* used for input_event support */
#include <errno.h>
#include <sys/poll.h>
#include <pthread.h>

BDBG_MODULE(input_router);

#if NEXUS_HAS_INPUT_ROUTER

#define MAX_CLIENTS 10

void callback(void *context, int param)
{
    BSTD_UNUSED(param);
    BKNI_SetEvent((BKNI_EventHandle)context);
}

/* USB human interface device (HID) supports keyboard and mouse */
static int  usb_hid_init(BKNI_EventHandle event);
static void usb_hid_uninit(void);
static int  usb_hid_read(struct input_event *input);

int main(void)
{
    NEXUS_InputRouterHandle router;
    NEXUS_InputClientHandle client[MAX_CLIENTS];
    NEXUS_PlatformStartServerSettings serverSettings;
    NEXUS_Error rc;
    unsigned i;
    BKNI_EventHandle event;
    NEXUS_IrInputHandle irInput;
    NEXUS_IrInputSettings irSettings;
#if NEXUS_HAS_KEYPAD
    NEXUS_KeypadHandle keypad;
    NEXUS_KeypadSettings keypadSettings;
#endif
    NEXUS_InputRouterCode mouseCode;
    
    rc = NEXUS_Platform_Init(NULL);
    if (rc) return -1;
    
    BKNI_CreateEvent(&event);
    
    usb_hid_init(event);
    
    NEXUS_IrInput_GetDefaultSettings(&irSettings);
    irSettings.mode = NEXUS_IrInputMode_eRemoteA;
    irSettings.dataReady.callback = callback;
    irSettings.dataReady.context = event;
    irInput = NEXUS_IrInput_Open(0, &irSettings);    
    
#if NEXUS_HAS_KEYPAD
    NEXUS_Keypad_GetDefaultSettings(&keypadSettings);
    keypadSettings.dataReady.callback = callback;
    keypadSettings.dataReady.context = event;
    keypad = NEXUS_Keypad_Open(0, &keypadSettings);
#endif
    
    NEXUS_Platform_GetDefaultStartServerSettings(&serverSettings);
    serverSettings.allowUnauthenticatedClients = true;
    rc = NEXUS_Platform_StartServer(&serverSettings);
    BDBG_ASSERT(!rc);
    
    router = NEXUS_InputRouter_Create(0);
    BDBG_ASSERT(router);
    
    for (i=0;i<MAX_CLIENTS;i++) {
        /* this server grants all codes to all clients. the client can limit what it wants. */
        client[i] = NEXUS_InputRouter_CreateClient(router, i);
        BDBG_ASSERT(client[i]);
    }
    
    /* set up a persistant code for the mouse. this allows handling of incremental changes (REL_X,REL_Y). */
    NEXUS_InputRouter_GetDefaultCode(&mouseCode);
    mouseCode.deviceType = NEXUS_InputRouterDevice_ePointer;
    mouseCode.filterMask = 1<<mouseCode.deviceType;
    mouseCode.data.pointer.x = 0;
    mouseCode.data.pointer.y = 0;
    mouseCode.data.pointer.button = 0;
    
    while (1) {
        bool wait = true;
        NEXUS_InputRouterCode code;
        
        if (irInput) {
            NEXUS_IrInputEvent irEvent;
            unsigned n;
            bool overflow;
            rc = NEXUS_IrInput_GetEvents(irInput, &irEvent, 1, &n, &overflow);
            if (n == 1) {
                BDBG_MSG(("irinput code=%#x", irEvent.code));
                NEXUS_InputRouter_GetDefaultCode(&code);
                code.deviceType = NEXUS_InputRouterDevice_eIrInput;
                code.filterMask = 1<<code.deviceType;
                code.data.irInput.index = 0;
                code.data.irInput.code = irEvent.code;
                code.data.irInput.repeat = irEvent.repeat;
                NEXUS_InputRouter_SendCode(router, &code);
                wait = false;
            }
        }
        
#if NEXUS_HAS_KEYPAD
        if (keypad) {
            NEXUS_KeypadEvent keypadEvent;
            unsigned n;
            bool overflow;
            rc = NEXUS_Keypad_GetEvents(keypad, &keypadEvent, 1, &n, &overflow);
            BDBG_ASSERT(!rc);
            if (n == 1) {
                BDBG_MSG(("keypad code=%#x", keypadEvent.code));
                NEXUS_InputRouter_GetDefaultCode(&code);
                code.deviceType = NEXUS_InputRouterDevice_eKeypad;
                code.filterMask = 1<<code.deviceType;
                code.data.keypad.index = 0;
                code.data.keypad.code = keypadEvent.code;
                code.data.keypad.repeat = keypadEvent.repeat;
                NEXUS_InputRouter_SendCode(router, &code);
                wait = false;
            }
        }
#endif
        
        {
            struct input_event input;
            NEXUS_InputRouter_GetDefaultCode(&code);
            while (1) { /* loop until empty */
                rc = usb_hid_read(&input);
                if (rc) break;
                BDBG_MSG(("input_event: time=%#x type=%#x code=%#x value=%#x", input.time, input.type, input.code, input.value));
                switch (input.type) {
                case EV_REL:
                    switch (input.code) {
                    case REL_X: mouseCode.data.pointer.x += input.value; break;
                    case REL_Y: mouseCode.data.pointer.y += input.value; break;
                    }
                    NEXUS_InputRouter_SendCode(router, &mouseCode);
                    break;
                case EV_ABS:
                    switch (input.code) {
                    case ABS_X: mouseCode.data.pointer.x = input.value; break;
                    case ABS_Y: mouseCode.data.pointer.y = input.value; break;
                    }
                    NEXUS_InputRouter_SendCode(router, &mouseCode);
                    break;
                case EV_KEY:   
                    switch (input.code) {
                    case BTN_LEFT:   
                        mouseCode.data.pointer.button = input.value << 0; 
                        NEXUS_InputRouter_SendCode(router, &mouseCode);
                        break;
                    case BTN_RIGHT:  
                        mouseCode.data.pointer.button = input.value << 1; 
                        NEXUS_InputRouter_SendCode(router, &mouseCode);
                        break;
                    case BTN_MIDDLE: 
                        mouseCode.data.pointer.button = input.value << 2; 
                        NEXUS_InputRouter_SendCode(router, &mouseCode);
                        break;
                    default:
                        code.deviceType = NEXUS_InputRouterDevice_eKeyboard;
                        code.filterMask = 1<<code.deviceType;
                        code.data.keyboard.code = input.code; 
                        code.data.keyboard.down = input.value;
                        NEXUS_InputRouter_SendCode(router, &code);
                        break;
                    }
                    break;
                }
                wait = false;
            } 
        }
        
        if (wait) {
            BKNI_WaitForEvent(event, BKNI_INFINITE);
        }
    }
    
    /* must stop server before destroying clients (which could be acquired by a client) */
    NEXUS_Platform_StopServer();
    for (i=0;i<MAX_CLIENTS;i++) {
        NEXUS_InputRouter_DestroyClient(client[i]);
    }    
    NEXUS_InputRouter_Destroy(router);
    
    NEXUS_IrInput_Close(irInput);
    BKNI_DestroyEvent(event);
    NEXUS_Platform_Uninit();
    usb_hid_uninit();
    return 0;
}
#else
int main(void)
{
    printf("ERROR: you must include input_router.inc in platform_modules.inc\n");
    return -1;
}
#endif

#if NEXUS_HAS_INPUT_ROUTER
#define MAX_HID 3
static struct {
    int fd;
    const char *node;
    int major;
    int minor;
} g_hid[MAX_HID] = {
    /* a real app would make this dynamic */
    {-1, "/dev/input/event0", 13, 64},
    {-1, "/dev/input/event1", 13, 65},
    {-1, "/dev/input/event2", 13, 66}
};
static pthread_t g_hid_thread;

static void *usb_hid_thread(void *context)
{
    while (1) {
        unsigned i;
        int rc;
        struct pollfd fds[MAX_HID];
        for (i=0;i<MAX_HID;i++) {
            if (g_hid[i].fd == -1) goto done; /* exiting */
            fds[i].fd = g_hid[i].fd;
            fds[i].events = POLLIN;
        }
        rc = poll(fds, i, 1000);
        if (rc == -1) break;
        
        BKNI_SetEvent((BKNI_EventHandle)context);
    }
done:
    return NULL;
}

static int usb_hid_init(BKNI_EventHandle event)
{
    unsigned i;
    int rc;
    
    mkdir("/dev/input", 0700);
    for (i=0;i<MAX_HID;i++) {
        mknod(g_hid[i].node, 0700 | S_IFCHR, makedev(g_hid[i].major,g_hid[i].minor));
        g_hid[i].fd = open(g_hid[i].node, O_NONBLOCK|O_RDONLY);
        if (g_hid[i].fd == -1) {
            BDBG_ERR(("unable to open %s: %d", g_hid[i].node, errno)); 
            return BERR_TRACE(errno);
        }
    }
    rc = pthread_create(&g_hid_thread, NULL, usb_hid_thread, event);
    if (rc) return -1;
    
    return 0;
}

static void usb_hid_uninit(void)
{
    unsigned i;
    for (i=0;i<MAX_HID;i++) {
        if (g_hid[i].fd != -1) {
            close(g_hid[i].fd);
            g_hid[i].fd = -1;
        }
    }
    pthread_cancel(g_hid_thread);
}

static int usb_hid_read(struct input_event *input)
{
    unsigned i;
    for (i=0;i<MAX_HID;i++) {
        ssize_t n;
        n = read(g_hid[i].fd, input, sizeof(*input));
        if (!n) continue;
        if (n == -1) {
            if (errno == EWOULDBLOCK) {
                continue;
            }
            else {
                return BERR_TRACE(errno);
            }
        }
    
        if (n != sizeof(*input)) {
            return BERR_TRACE(-1);
        }
        /* got one event */
        return 0;
    }
    /* nothing */
    return -1;
}
#endif
