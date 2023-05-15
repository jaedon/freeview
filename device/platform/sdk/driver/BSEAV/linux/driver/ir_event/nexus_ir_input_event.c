/******************************************************************************
 *    (c)2010 Broadcom Corporation
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
 * $brcm_Workfile: nexus_ir_input_event.c $
 * $brcm_Revision: 5 $
 * $brcm_Date: 3/13/12 10:21a $
 *
 * Module Description: Kernel module that joins an initialized NEXUS instance,
 * registers as a Linux input device and sets up an IR input callback to translate 
 * Broadcom IR events into Linux input events.
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/linux/driver/ir_event/nexus_ir_input_event.c $
 * 
 * 5   3/13/12 10:21a ttrammel
 * SWANDROID-38: Add support for Broadcom silver remote (nec protocol).
 * 
 * 4   5/2/11 3:23p ttrammel
 * SW7425-434: Android support for 2.6.37.
 * 
 * 3   3/30/11 8:43p alexpan
 * SW7420-1708: Fix IR no response problem
 * 
 * 2   12/9/10 4:43p ttrammel
 * SW7420-1243: Add debounce and key down/up events to initial driver.
 * 
 *****************************************************************************/
#include <linux/version.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37)
#include <generated/autoconf.h>
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 30)) && (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 37))
#include <linux/autoconf.h>
#else
#include <linux/config.h>
#endif
#include <linux/module.h>
#include <linux/input.h>
#include <linux/pci.h>

#include "nexus_platform.h"
#include "nexus_ir_input.h"

static int remote_a = 0;
module_param(remote_a, int, S_IRUGO);

#define DEBOUNCE_TIME (200)
static struct {
	int ir_inputev[2];
	int inputev;
} nexus_ir_input_inputkeys[] = {
    /* silver    black      key */
	{ {0x4d,     0x12},        KEY_ESC },
	{ {0x1f,     0x01},        KEY_1 },
	{ {0x5e,     0x02},        KEY_2 },
	{ {0x5f,     0x03},        KEY_3 },
	{ {0x1b,     0x04},        KEY_4 },
	{ {0x5a,     0x05},        KEY_5 },
	{ {0x5b,     0x06},        KEY_6 },
	{ {0x17,     0x07},        KEY_7 },
	{ {0x56,     0x08},        KEY_8 },
	{ {0x57,     0x09},        KEY_9 },
	{ {0x52,     0x00},        KEY_0 },
	{ {0x08,     0x11},        KEY_ENTER },  /* Enter key */
	{ {0x4e,     0x34},        KEY_UP },
	{ {-1,       0x3a},        KEY_PAGEUP },
	{ {0x0b,     0x36},        KEY_LEFT },
	{ {0x49,     0x37},        KEY_RIGHT },
	{ {0x0c,     0x35},        KEY_DOWN },
	{ {-1,       0x38},        KEY_F5 },
	{ {-1,       0x3b},        KEY_PAGEDOWN },
	{ {0x4c,     0x0e},        KEY_VOLUMEDOWN },
	{ {0x48,     0x0d},        KEY_VOLUMEUP },
	{ {0x0a,     0x0a},        KEY_POWER },
	{ {0x4f,     0x19},        KEY_MENU },
	{ {0x01,     0x0f},        KEY_MUTE },
    { {0x50,     0x26},        KEY_RED },
    { {0x10,     0x25},        KEY_GREEN },
    { {0x11,     0x23},        KEY_YELLOW },
    { {0x51,     -1},          KEY_BLUE },
	{ {0,        0},           0 },
};

static struct nexus_ir_input_event_device {
	struct input_dev *input_key_dev;
	NEXUS_IrInputHandle irHandle;
	NEXUS_IrInputSettings irSettings;
} nexus_ir_input_event_device;

void nexus_ir_input_event_irCallback(void *pParam, int iParam)
{
    size_t numEvents = 1;
    NEXUS_Error rc = 0;
    bool overflow;
    static int last_event_time = 0;
	int i,delta=0;
    NEXUS_IrInputHandle irHandle = *(NEXUS_IrInputHandle *)pParam;
    BSTD_UNUSED(iParam);
    while (numEvents && !rc) {
        NEXUS_IrInputEvent irEvent;
        rc = NEXUS_IrInput_GetEvents(irHandle,&irEvent,1,&numEvents,&overflow);
        if (numEvents)
		{
            irEvent.code >>= (remote_a) ? 0 : 16;
			irEvent.code &= 0xff;
			/*printk("irCallback: rc: %d, code: %08x, repeat: %s\n", rc, irEvent.code, irEvent.repeat ? "true" : "false");*/
			if(last_event_time)
			{
				delta = jiffies - last_event_time;
			}
			last_event_time = jiffies;
			if(delta>0 && delta<DEBOUNCE_TIME) continue;
			for (i = 0; nexus_ir_input_inputkeys[i].inputev; i++)
				if(irEvent.code==nexus_ir_input_inputkeys[i].ir_inputev[remote_a])
				{
					/*printk("irCallback: key matched input event generated as %x\n",nexus_ir_input_inputkeys[i].inputev);*/
					input_report_key(nexus_ir_input_event_device.input_key_dev, nexus_ir_input_inputkeys[i].inputev, 1);
					input_sync(nexus_ir_input_event_device.input_key_dev);
					input_report_key(nexus_ir_input_event_device.input_key_dev, nexus_ir_input_inputkeys[i].inputev, 0);
					input_sync(nexus_ir_input_event_device.input_key_dev);

					break;
				}
		}
    }
}

static int __devinit nexus_ir_input_event_create_input_devices()
{
	struct input_dev *key_dev;
	int i;
	int error;

	nexus_ir_input_event_device.input_key_dev = key_dev = input_allocate_device();
	if (!key_dev) return -ENOMEM;

	key_dev->name = "nexus ir input";
	key_dev->id.bustype = BUS_HOST;
	key_dev->id.vendor = PCI_VENDOR_ID_BROADCOM;
	key_dev->dev.parent = NULL;

	/* Initialize the Input Drivers: special keys */
	key_dev->evbit[0] = BIT_MASK(EV_KEY);
	for (i = 0; nexus_ir_input_inputkeys[i].inputev; i++)
			set_bit(nexus_ir_input_inputkeys[i].inputev, key_dev->keybit);

	error = input_register_device(key_dev);
	if (error) goto err_free_keydev;

	return 0;

err_free_keydev:
	input_free_device(key_dev);
	nexus_ir_input_event_device.input_key_dev = NULL;
	return error;
}

#ifdef B_REFSW_FIRMWARE
MODULE_LICENSE("GPL");
#else
MODULE_LICENSE("Proprietary");
#endif

static int __init my_init (void)
{
	int error;

    printk("Configuring for ir device %s remote\n",(remote_a) ? "RemoteA(black)" : "Nec(silver)");
    NEXUS_IrInput_GetDefaultSettings(&nexus_ir_input_event_device.irSettings);
    nexus_ir_input_event_device.irSettings.mode = (remote_a) ? NEXUS_IrInputMode_eRemoteA : NEXUS_IrInputMode_eCirNec;
    nexus_ir_input_event_device.irSettings.dataReady.callback = nexus_ir_input_event_irCallback;
    nexus_ir_input_event_device.irSettings.dataReady.context = &nexus_ir_input_event_device.irHandle;
    nexus_ir_input_event_device.irHandle = NEXUS_IrInput_Open(0, &nexus_ir_input_event_device.irSettings);
	if(!nexus_ir_input_event_device.irHandle) return -ENODEV;

    error = nexus_ir_input_event_create_input_devices();
	return error;
}

static void __exit my_exit (void)
{
	input_unregister_device(nexus_ir_input_event_device.input_key_dev);
	input_free_device(nexus_ir_input_event_device.input_key_dev);
	nexus_ir_input_event_device.input_key_dev = NULL;
    NEXUS_IrInput_Close(nexus_ir_input_event_device.irHandle);
}


module_init (my_init);
module_exit (my_exit);

