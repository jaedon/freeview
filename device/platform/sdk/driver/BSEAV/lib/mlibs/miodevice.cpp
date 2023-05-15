/***************************************************************************
 *     Copyright (c) 2002-2003, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: miodevice.cpp $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 11:13p $
 *
 * Module Description: App logic for pvrgui app
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/miodevice.cpp $
 * 
 * 1   2/7/05 11:13p dlwin
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
#include "miodevice.h"
#include "bwin.h"
#include "mapp_priv.h"

MIoDevice::MIoDevice(MApplication *app)
{
	_app = app;
}

void MIoDevice::start()
{
	_app->add(this);
	bwin_add_io_handle(_app->_private->win_engine, (bwin_io_handle)this);
}

void MIoDevice::trigger()
{
	bwin_trigger_io(_app->_private->win_engine, (bwin_io_handle)this);
}

void MIoDevice::stop()
{
	bwin_remove_io_handle(_app->_private->win_engine, (bwin_io_handle)this);
	_app->remove(this);
}

FIRE_EV_METHOD(MIoDevice,Ready)

void MIoDevice::ready()
{
	FIRE(Ready);
}

