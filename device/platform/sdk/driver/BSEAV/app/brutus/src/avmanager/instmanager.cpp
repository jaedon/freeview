/***************************************************************************
 *     Copyright (c) 2002-2004, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: instmanager.cpp $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 7:53p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/avmanager/instmanager.cpp $
 * 
 * 1   2/7/05 7:53p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/1   4/30/04 12:06p erickson
 * PR10832: rework of brutus to support single AVManager and
 * ChannelManager for system
 *
 ******************************************************************************/
#include "instmanager.h"

VoidManager::VoidManager()
{
	_active = 0;
	BKNI_CreateMutex(&_mutex);
}

VoidManager::~VoidManager()
{
	BKNI_DestroyMutex(_mutex);
}

void *VoidManager::checkOut()
{
	Wrapper *w;
	BKNI_AcquireMutex(_mutex);
	for (w = _list.first(); w; w = _list.next()) {
		if (!w->locked) {
			w->locked = true;
			_active++;
			break;
		}
	}
	BKNI_ReleaseMutex(_mutex);
	return w?w->data:NULL;
}

bool VoidManager::checkIn(void *data)
{
	bool rc = false;
	BKNI_AcquireMutex(_mutex);
	for (Wrapper *w = _list.first(); w; w = _list.next()) {
		if (w->data == data) {
			if (w->locked) {
				w->locked = false;
				_active--;
				rc = true;
				break;
			}
		}
	}
	BKNI_ReleaseMutex(_mutex);
	return rc;
}

bool VoidManager::remove(void *data)
{
	bool rc = false;
	BKNI_AcquireMutex(_mutex);
	for (Wrapper *w = _list.first(); w; w = _list.next()) {
		if (w->data == data) {
			/* Don't care about whether it's checked out. Assume the caller
			knows what they want. */
			if (w->locked)
				_active--;
			_list.remove();
			delete w;
			rc = true;
			break;
		}
	}
	BKNI_ReleaseMutex(_mutex);
	return rc;
}
