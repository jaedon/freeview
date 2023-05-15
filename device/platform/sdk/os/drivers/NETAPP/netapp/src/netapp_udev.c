/***************************************************************************
 *    (c)2010-2011 Broadcom Corporation
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
 * $brcm_Workfile: netapp_udev.c $
 * $brcm_Revision: 1 $
 * $brcm_Date: 12/9/11 4:44p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /AppLibs/broadcom/netapp/src/netapp_udev.c $
 * 
 * 1   12/9/11 4:44p steven
 * SW7231-513: USB Hotplug Support in NetApp
 * 
 * SW7231-513/1   12/9/11 3:36p steven
 * SW7231-513: Finished working hotplug detection
 *
 *
 ***************************************************************************/
/**
 * @brief NetApp Private USB Hotplug Support
 *
 * @author Steven Hartley (steven@broadcom.com)
 */
#include "netapp_priv.h"
#include "netapp_udev.h"
#include "libudev.h"


#define NETAPP_UDEV_CHECK(api, tRetCode)                                        \
{                                                                               \
    if (api != 0)                                                               \
    {                                                                           \
        NETAPP_ERR(("%s(): [%s] failed!", __FUNCTION__, #api));                 \
        tRetCode = NETAPP_FAILURE;                                              \
        goto err_out;                                                           \
    }                                                                           \
    tRetCode = NETAPP_SUCCESS;                                                  \
}


#define NETAPP_UDEV_COPY(dest, source)                                          \
{                                                                               \
    if (source != NULL)                                                         \
    {                                                                           \
        strncpy(dest, source, sizeof(dest)/sizeof(dest[0])-1);                  \
        dest[sizeof(dest)/sizeof(dest[0])-1] = '\0';                            \
    }                                                                           \
}


struct sNETAPP_UDEV
{
    struct udev         *udev;
    uint32_t            ulBkgrndTaskId;
    bool                bStopBkgrndTask;
    struct udev_monitor *pUdevMonitor;
};

static void NetAppUdev_P_BackgroundTask(
    void              *pParam);


NETAPP_RETCODE NetAppUdevPrivateInit(
    NETAPP_UDEV_HANDLE       *phNetAppUdev)
{

    NETAPP_RETCODE      tRetCode    = NETAPP_SUCCESS;
    NETAPP_UDEV_HANDLE  hNetAppUdev = NULL;
    struct udev_enumerate *enumerate;
    struct udev_list_entry *devices, *dev_list_entry;
    struct udev_device *dev;

    NETAPP_MSG(("%s() Entered!", __FUNCTION__));
    if (phNetAppUdev == NULL)
    {
        NETAPP_ERR(("%s() -- Invalid parameter!", __FUNCTION__));
        return NETAPP_INVALID_PARAMETER;
    }

    hNetAppUdev = (NETAPP_UDEV_HANDLE)NetAppOSMemAlloc(sizeof(struct sNETAPP_UDEV));
    if (hNetAppUdev == NULL)
    {
        NETAPP_ERR(("%s() -- MemAlloc error!", __FUNCTION__));
        tRetCode = NETAPP_OUT_OF_MEMORY;
        goto err_out;
    }

    memset(hNetAppUdev, 0, sizeof(struct sNETAPP_UDEV));

    /* Create the udev object */
    hNetAppUdev->udev = udev_new();

    if (hNetAppUdev->udev == NULL)
    {
        printf("Can't create udev\n");
        goto err_out;
    }

    /* Set up a monitor to monitor hidraw devices */
    hNetAppUdev->pUdevMonitor = udev_monitor_new_from_netlink(hNetAppUdev->udev, "kernel");
    if (hNetAppUdev->pUdevMonitor == 0)
    {
        NETAPP_ERR(("%s() Cannot create udev monitor\n", __FUNCTION__));
        tRetCode = NETAPP_FAILURE;
        goto err_out;
    }

    NETAPP_UDEV_CHECK(udev_monitor_filter_add_match_subsystem_devtype(hNetAppUdev->pUdevMonitor, "usb", NULL), tRetCode);
    NETAPP_UDEV_CHECK(udev_monitor_filter_add_match_subsystem_devtype(hNetAppUdev->pUdevMonitor, "input", NULL), tRetCode);
    NETAPP_UDEV_CHECK(udev_monitor_enable_receiving(hNetAppUdev->pUdevMonitor), tRetCode);

    hNetAppUdev->ulBkgrndTaskId = NetAppOSTaskSpawn("NetAppUdev_P_BackgroundTask", NETAPP_OS_PRIORITY_LOW,
           256*1024, NetAppUdev_P_BackgroundTask, hNetAppUdev);

    if (hNetAppUdev->ulBkgrndTaskId == 0)
    {
        NETAPP_ERR(("%s() Cannot create BackgroundTask\n", __FUNCTION__));
        tRetCode = NETAPP_FAILURE;
        goto err_out;
    }

    *phNetAppUdev = hNetAppUdev;

err_out:
    if ( (tRetCode != NETAPP_SUCCESS) && (hNetAppUdev != NULL) )
    {
        if (hNetAppUdev->ulBkgrndTaskId != 0)
        {
            hNetAppUdev->bStopBkgrndTask = true;
            NetAppOSTaskJoin(hNetAppUdev->ulBkgrndTaskId);
            NetAppOSTaskDelete(hNetAppUdev->ulBkgrndTaskId);
            hNetAppUdev->ulBkgrndTaskId = 0;
        }

        if (hNetAppUdev->udev != 0)
        {
            udev_unref(hNetAppUdev->udev);
            hNetAppUdev->udev = 0;
        }

        NetAppOSMemFree(hNetAppUdev);
    }
    return tRetCode;
}

NETAPP_RETCODE NetAppUdevPrivateDeinit(
    NETAPP_UDEV_HANDLE       hNetAppUdev)
{
    NETAPP_PRIV_HANDLE_CHECK(hNetAppUdev);

    if (hNetAppUdev->ulBkgrndTaskId != 0)
    {
        hNetAppUdev->bStopBkgrndTask = true;
        NetAppOSTaskJoin(hNetAppUdev->ulBkgrndTaskId);
        NetAppOSTaskDelete(hNetAppUdev->ulBkgrndTaskId);
        hNetAppUdev->ulBkgrndTaskId = 0;
    }

    udev_unref(hNetAppUdev->udev);
    NetAppOSMemFree(hNetAppUdev);
    hNetAppUdev = NULL;

    return NETAPP_SUCCESS;
}


/*******************************************************************************
 *                     Static NetApp UDEV Private Functions
 ******************************************************************************/
static void NetAppUdev_P_BackgroundTask(
    void              *pParam)
{
    NETAPP_UDEV_HANDLE  hNetAppUdev = (NETAPP_UDEV_HANDLE)pParam;
    int fd;

    if (hNetAppUdev == NULL)
    {
        NETAPP_ERR(("%s(): Invalid Parameter!", __FUNCTION__));
        return;
    }

    //udev_monitor_filter_add_match_subsystem_devtype(mon, "hidraw", NULL);
    //udev_monitor_filter_add_match_subsystem_devtype(pUdevMonitor, "input", NULL);

    if ((fd = udev_monitor_get_fd(hNetAppUdev->pUdevMonitor)) <= 0)
    {
        NETAPP_ERR(("%s(): Failure to get the monitor FD, exiting", __FUNCTION__));
        goto err_out;
    }

    while (!hNetAppUdev->bStopBkgrndTask)
    {
        fd_set              fds;
        struct timeval      tv;
        int                 ret;
        struct udev_device  *dev;

        FD_ZERO(&fds);
        FD_SET(fd, &fds);
        tv.tv_sec = 0;
        tv.tv_usec = 250*1000;

        ret = select(fd+1, &fds, NULL, NULL, &tv);

        /* Check if our file descriptor has received data. */
        if (ret > 0 && FD_ISSET(fd, &fds))
        {
            //NETAPP_MSG(("%s(): Received something from udev!", __FUNCTION__));

            NetAppOSTaskDelayMsec(20);
            /* Make the call to receive the device.
               select() ensured that this will not block. */
            dev = udev_monitor_receive_device(hNetAppUdev->pUdevMonitor);
            if (dev != NULL)
            {
                NETAPP_HOTPLUG_DEVICE_INFO tInfo;

                memset(&tInfo, 0, sizeof(NETAPP_HOTPLUG_DEVICE_INFO));
                const char *pAction     = udev_device_get_action(dev);
                const char *pSubSystem  = udev_device_get_subsystem(dev);
                const char *pSysName    = udev_device_get_sysname(dev);
                const char *pDevType    = udev_device_get_devtype(dev);

                /* Filter on the Action */
                if (strcmp(pAction, "add") == 0)
                {
                    tInfo.tAction = NETAPP_HOTPLUG_ADD;
                }
                else if (strcmp(pAction, "remove") == 0)
                {
                    tInfo.tAction = NETAPP_HOTPLUG_REMOVE;
                }
                else
                {
                    NETAPP_MSG(("%s(): Ignoring action %s", __FUNCTION__, pAction));
                    udev_device_unref(dev);
                    continue;
                }

                /* Filter on SubSystem */
                if ((strcmp(pSubSystem, "input") == 0) && (strncmp(pSysName, "input", strlen("input")) != 0) )
                {
                    tInfo.tType = NETAPP_HOTPLUG_DEVICE_USB_INPUT;
                }
                else if ( (strcmp(pSubSystem, "usb") == 0) && (pSysName != NULL) && (strcmp(pSysName, "btusb0") == 0) )
                {
                    tInfo.tType = NETAPP_HOTPLUG_DEVICE_BLUETOOTH;
                }

                else if ( (strcmp(pSubSystem, "usb") == 0) && (strcmp(pDevType, "usb_device") == 0) )
                {
                    tInfo.tType = NETAPP_HOTPLUG_DEVICE_USB;
                    NETAPP_UDEV_COPY(tInfo.cManufacturer,   udev_device_get_sysattr_value(dev, "manufacturer")); //!< Manufacturer Name
                    NETAPP_UDEV_COPY(tInfo.cProduct,        udev_device_get_sysattr_value(dev, "product"));      //!< Product Name
                    NETAPP_UDEV_COPY(tInfo.cSerialNumber,   udev_device_get_sysattr_value(dev, "serial"));     //!< Serial information
                }
                else
                {
                    NETAPP_MSG(("%s(): Ignoring SubSystem %s", __FUNCTION__, pSubSystem));
                    udev_device_unref(dev);
                    continue;
                }

                if (udev_device_get_sysattr_value(dev, "idVendor") != NULL)
                {
                    tInfo.usVendorID = atoi(udev_device_get_sysattr_value(dev, "idVendor"));
                }
                if (udev_device_get_sysattr_value(dev, "idProduct") != NULL)
                {
                    tInfo.usProductID = atoi(udev_device_get_sysattr_value(dev, "idProduct"));
                }

                NETAPP_UDEV_COPY(tInfo.cSysName,        pSysName);
                NETAPP_UDEV_COPY(tInfo.cSysPath,        udev_device_get_syspath(dev));

                NETAPP_MSG(("************************************"));
                NETAPP_MSG(("%s(): USB Hotplug Notification:", __FUNCTION__));
                NETAPP_MSG(("Action:        %s", (tInfo.tAction == NETAPP_HOTPLUG_ADD) ? "ADD" : "REMOVE"));
                NETAPP_MSG(("cDevType:      %s", tInfo.cDevType));
                if (tInfo.tType == NETAPP_HOTPLUG_DEVICE_USB)
                {
                    NETAPP_MSG(("usVendorID:    %04X", tInfo.usVendorID));
                    NETAPP_MSG(("usProductID:   %04X", tInfo.usProductID));
                    NETAPP_MSG(("cManufacturer: %s", tInfo.cManufacturer));
                    NETAPP_MSG(("cProduct:      %s", tInfo.cProduct));
                    NETAPP_MSG(("SerialNumber:  %s", tInfo.cSerialNumber));
                    NETAPP_UDEV_COPY(tInfo.cDevType,        pDevType)
                }
                NETAPP_MSG(("SysPath:       %s", tInfo.cSysPath));
                NETAPP_MSG(("SysName:       %s", tInfo.cSysName));
                NETAPP_MSG(("DevPath:       %s", udev_device_get_devpath(dev)));
                NETAPP_MSG(("pSubSystem:    %s", pSubSystem));
                NETAPP_MSG(("SysNum:        %s", udev_device_get_sysnum(dev)));
                NETAPP_MSG(("Driver:        %s", udev_device_get_driver(dev)));
                NETAPP_MSG(("************************************"));

                NETAPP_DO_CALLBACK(NETAPP_CB_HOTPLUG, &tInfo, sizeof(tInfo), 0, NETAPP_SUCCESS, NETAPP_IFACE_MAX);

                udev_device_unref(dev);
            }
        }
    }

err_out:
    NETAPP_MSG(("%s() Leaving", __FUNCTION__));
    NetAppOSTaskExit();
    return;
}

#if 0
    /* Create a list of the devices in the 'hidraw' subsystem. */
    enumerate = udev_enumerate_new(hNetAppUdev->udev);
    udev_enumerate_add_match_subsystem(enumerate, "input");
    udev_enumerate_scan_devices(enumerate);
    devices = udev_enumerate_get_list_entry(enumerate);
    /* For each item enumerated, print out its information.
       udev_list_entry_foreach is a macro which expands to
       a loop. The loop will be executed for each member in
       devices, setting dev_list_entry to a list entry
       which contains the device's path in /sys. */
    udev_list_entry_foreach(dev_list_entry, devices) {
        const char *path;

        /* Get the filename of the /sys entry for the device
           and create a udev_device object (dev) representing it */
        path = udev_list_entry_get_name(dev_list_entry);
        dev = udev_device_new_from_syspath(hNetAppUdev->udev, path);

        /* usb_device_get_devnode() returns the path to the device node
           itself in /dev. */
        printf("Device Node Path: %s\n", udev_device_get_devnode(dev));

        /* The device pointed to by dev contains information about
           the hidraw device. In order to get information about the
           USB device, get the parent device with the
           subsystem/devtype pair of "usb"/"usb_device". This will
           be several levels up the tree, but the function will find
           it.*/
        dev = udev_device_get_parent_with_subsystem_devtype(
               dev,
               "usb",
               "usb_device");
        if (!dev) {
            printf("Unable to find parent usb device.");
            goto err_out;
        }

        /* From here, we can call get_sysattr_value() for each file
           in the device's /sys entry. The strings passed into these
           functions (idProduct, idVendor, serial, etc.) correspond
           directly to the files in the directory which represents
           the USB device. Note that USB strings are Unicode, UCS2
           encoded, but the strings returned from
           udev_device_get_sysattr_value() are UTF-8 encoded. */
        printf("  VID/PID: %s %s\n",
                udev_device_get_sysattr_value(dev,"idVendor"),
                udev_device_get_sysattr_value(dev, "idProduct"));
        printf("  %s\n  %s\n",
                udev_device_get_sysattr_value(dev,"manufacturer"),
                udev_device_get_sysattr_value(dev,"product"));
        printf("  serial: %s\n",
                 udev_device_get_sysattr_value(dev, "serial"));
        udev_device_unref(dev);
    }
    /* Free the enumerator object */
    udev_enumerate_unref(enumerate);
#endif
