/***************************************************************************
 *     Copyright (c) 2002-2003, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: miodevice.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 11:14p $
 *
 * Module Description: App logic for pvrgui app
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/miodevice.h $
 * 
 * 1   2/7/05 11:14p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/3   7/6/04 3:56p erickson
 * PR11771: settop api dataflow redesign
 * 
 * Irvine_BSEAVSW_Devel/2   4/16/04 10:45a erickson
 * PR9029: copy the bwin_io_handle structure to prevent reading from bad
 * memory
 * 
 * Irvine_BSEAVSW_Devel/1   2/4/04 2:11p erickson
 * PR9107: implemented new device io system
 *
 *************************************************************************/
#ifndef MIO_DEVICE_H__
#define MIO_DEVICE_H__

#include "microwidgets.h"
#include "mapplication.h"

class MIoDevice;

BEGIN_EV(MIoDevice)
	EV_METHOD(MIoDevice,Ready);
END_EV()

/**
Summary:
Provides a device-independent way to register io devices and receive callbacks.

Description:
The application can start/stop the device.
The application can trigger that the device is ready from any context (i.e. a callback 
from settop api).
MApplication will wake up its event loop, find the event, lookup the corresponding MIoDevice, 
and cause a synchronous ready event to be fired.
**/
class MIoDevice {
public:
	MIoDevice(MApplication *app);

	void start();
	void stop();

	/* Called by the application asynchronously. It will result in a
	synchronous MIoDevice Ready callback. */
	void trigger();

	/* This triggers the OnReady event. It is called by MApplication when
	an io event happens. It can also be called by an application to simulate
	an event. */
	void ready();

	MApplication *app() const {return _app;}

protected:
	MApplication *_app;

	SUPPORT_EV(MIoDevice)
	SUPPORT_EV_METHOD(Ready)
};

#endif // MIO_DEVICE_H__
