/***************************************************************************
 *     Copyright (c) 2002-2004, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: instmanager.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 7:54p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/avmanager/instmanager.h $
 * 
 * 1   2/7/05 7:54p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/2   5/28/04 12:31p erickson
 * PR11258: functions weren't returning correct values
 * 
 * Irvine_BSEAVSW_Devel/1   4/30/04 12:07p erickson
 * PR10832: rework of brutus to support single AVManager and
 * ChannelManager for system
 *
 ******************************************************************************/
#ifndef INSTANCEMANAGER_H__
#define INSTANCEMANAGER_H__

#include <bstd.h>
#include <bkni_multi.h>
#include <mlist.h>

/**
Thread-safe check in/check out manager.
Internal synchronization using BKNI_MutexHandle.
**/
class VoidManager {
public:
	VoidManager();
	virtual ~VoidManager();

	void add(void *data) {_list.add(new Wrapper(data));}
	bool remove(void *data);

	void *checkOut();
	bool checkIn(void *data);

	int active() const {return _active;}
	int total() const {return _list.total();}

protected:
	struct Wrapper {
		void *data;
		bool locked;
		Wrapper(void *d) {data=d;locked=false;}
	};
	MAutoList<Wrapper> _list;
	int _active;
	BKNI_MutexHandle _mutex;
};

/**
InstanceManager is a thin-templated class which allows you to check in/check out
resources.

It does not own the object added to it. It will not free the memory.
**/
template<class T>
class InstanceManager : public VoidManager {
public:
	void add(T *data) {VoidManager::add((void*)data);}
	bool remove(T *data) {return VoidManager::remove((void*)data);}

	T *checkOut() {return (T*)VoidManager::checkOut();}
	void checkIn(T *data) {VoidManager::checkIn((void*)data);}
};

#endif //RECORDMANAGER_H__
