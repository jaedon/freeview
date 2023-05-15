/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: notification.cpp $
* $brcm_Revision: 3 $
* $brcm_Date: 2/28/12 5:03p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/notification.cpp $
* 
* 3   2/28/12 5:03p tokushig
* SW7405-5581: add header comments
* 
* 
*********************************************************************************/

#include "notification.h"
#include "brutus_os.h"

BDBG_MODULE(brutus_notification);

CNotification::CNotification(eNotification id, void * data) :
    _id(id),
    _data(data)
{
}

eNotification CNotification::getId()
{
    return _id;
}


void CNotification::setData(void * data)
{
    _data = data;
}

void * CNotification::getData()
{
    return _data;
}

CNotification::~CNotification(void)
{
}


CObserver::CObserver(NOTIFY_CALLBACK notifyCallback, CObserver * context) :
    _notifyCallback(notifyCallback),
    _context(context ? context : this)
{
}

eRet CObserver::notify(CNotification & notification)
{
    eRet ret = eRet_Ok;
    if (_notifyCallback)
    {
        _notifyCallback(_context, notification);
    }
    else
    {
        /* error: no notify callback */
        ret = eRet_NotAvailable;
    }

    return ret;
}

void CObserver::setNotifyCallback(NOTIFY_CALLBACK notifyCallback)
{
    _notifyCallback = notifyCallback;
}

void CObserver::setNotifyContext(CObserver * context)
{
    _context = context;
}

CObserver::~CObserver(void)
{
}

void CObserver::processNotification(CNotification & notification)
{
    BSTD_UNUSED(notification);
}


CSubject::CSubject(void)
{
    _mutex = B_Mutex_Create(NULL);
    BDBG_ASSERT(_mutex);

    clearAllObservers();
}

eRet CSubject::registerObserver(CObserver * pObserver, eNotification notification)
{
    eRet ret = eRet_Ok;
    MListItr <CObserverListElem> itr(&_observerList);
    CObserverListElem * ptr = NULL;

    if (eNotify_Invalid == notification)
    {
        return eRet_InvalidParameter;
    }

    /* serialized access to _observerList */
    CScopedMutex observerListMutex(_mutex);

    BDBG_ASSERT(NULL != pObserver);

    /* see if observer/notification pair already in list */
    for (ptr = itr.first(); ptr; ptr = itr.next())
    {
        CObserverListElem listElem(pObserver, notification);
        if (*ptr == listElem)
        {
            break;
        }
    }

    if (ptr)
    {
        /* duplicate observer/notification pair - do nothing */
        ret = eRet_InvalidParameter;
    }
    else
    {
        CObserverListElem * pObserverListElem = NULL;
        pObserverListElem = new CObserverListElem(pObserver, notification);
        BDBG_ASSERT(pObserverListElem);
        _observerList.add(pObserverListElem);
    }

    return ret;
}

eRet CSubject::unregisterObserver(CObserver * pObserver, eNotification notification)
{
    eRet ret = eRet_Ok;
    MListItr <CObserverListElem> itr(&_observerList);
    CObserverListElem * ptr = NULL;
    bool found = false;

    /* serialized access to _observerList */
    CScopedMutex observerListMutex(_mutex);

    BDBG_ASSERT(NULL != pObserver);

    /* assume given observer/notification not found */
    ret = eRet_InvalidParameter;

    /* search list for matching observer/notification pair */
    for (ptr = itr.first(); ptr; ptr = itr.next())
    {
        if (ptr->getObserver() == pObserver)
        {
            if ((eNotify_All == notification) || (ptr->getNotification() == notification))
            {
                ptr = _observerList.remove(ptr);
                BDBG_ASSERT(ptr);
                delete ptr;
                ret = eRet_Ok;
            }
        }
    }

    return ret;
}

eRet CSubject::notifyObservers(eNotification id, void * data)
{
    eRet ret = eRet_Ok;
    MList <CObserverListElem> foundObserverList;

    if (eNotify_Invalid == id)
    {
        return eRet_InvalidParameter;
    }

    {
        /* serialized access to _observerList */
        CScopedMutex observerListMutex(_mutex);

        /* find all observers with matching notifications in observer list and save in foundObserverList.
           this is done so that the real observer list is not mutex locked while notifying
           all the registered observers. */
        if (0 < _observerList.total())
        {
            CObserverListElem * ptr = NULL;
            MListItr <CObserverListElem> itr(&_observerList);

            for (ptr = itr.first(); ptr; ptr = itr.next())
            {
                if ((eNotify_All == ptr->getNotification()) || (id == ptr->getNotification()))
                {
                    foundObserverList.add(ptr);
                }
            }
        }
    }

    /* notify all found matching observers */
    if (0 < foundObserverList.total())
    {
        CObserverListElem * ptrFound = NULL;
        MListItr <CObserverListElem> itrFound(&foundObserverList);

        for (ptrFound = itrFound.first(); ptrFound; ptrFound = itrFound.next())
        {
            CNotification notification(id, data);

            ptrFound->getObserver()->notify(notification);
        }
    }

    return ret;
}

eRet CSubject::clearAllObservers(void)
{
    eRet ret = eRet_Ok;
    MListItr <CObserver> itr(&_observerList);
    CObserver * ptr = NULL;

    /* serialized access to _observerList */
    CScopedMutex observerListMutex(_mutex);

    /* clear all observers from list */
    if (0 < _observerList.total())
    {
        for (ptr = itr.first(); ptr; ptr = itr.next())
        {
            CObserverListElem * pElem = NULL;
            pElem = _observerList.remove(0);
            BDBG_ASSERT(pElem);
            delete pElem;
        }
    }

    return ret;
}

CSubject::~CSubject(void)
{
    clearAllObservers();

    if (_mutex)
    {
        B_Mutex_Destroy(_mutex);
        _mutex = NULL;
    }
}

