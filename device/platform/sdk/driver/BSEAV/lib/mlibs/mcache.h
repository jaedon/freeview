/***************************************************************************
 *     Copyright (c) 2001-2006, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mcache.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 8/28/06 10:48a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/mcache.h $
 * 
 * 2   8/28/06 10:48a tokushig
 * PR22347: remove mutex protection - not needed with single threaded
 * brutus picture viewer
 * 
 * 1   8/25/06 1:26p tokushig
 * PR22347: in non-skinned brutus, images w/o embedded thumbnails will
 * display full image as thumb.  if using a skin, small images w/o
 * embedded thumbnails will display full image as thumb but large images
 * w/o embedded thumbnails will display a placeholder icon instead
 * (images are currently considered large if they have more than 640 *
 * 480 = 307200pixels.)  thumbnail please wait popup was replaced by
 * sequential drawing of thumbs which provides better feedback to users.
 * implemented image caching on thumbs (default: last 30) and for full
 * screen image view (default: last 3).
 * 
 * SanDiego_Brutus_Skin/2   8/23/06 5:11p tokushig
 * fixed error check for mutex initialization
 * 
 * SanDiego_Brutus_Skin/1   8/17/06 5:00p tokushig
 * initial creation
 * 
 ****************************************************************************/
#ifndef MCACHE_H
#define MCACHE_H

#include <stdlib.h>
#include "time.h"
#include "mlist.h"
#include "mstring.h"


template <class T>
class MCache
{
public:
    MCache()
    {
    };
    virtual ~MCache()
    {
        clear(true);
    };
    
    void      setMaxElements(unsigned long maxElem = 0);

    bool      add(const char * name, T * data);
    T       * get(const char * name);
    T       * reserve(const char * name);
    void      reserveAll();
    T       * unreserve(const char * name);
    void      unreserveAll();
    bool      clear(bool force = false);
    void      print();

protected:
    bool      push(const char * name, T * data);
    T       * pop();
    T       * find(const char * name);
    T       * remove(const char *name); 
    T       * remove();

protected:
    struct Node 
    {
        MString   _name;
        T       * _data;
        bool      _reserved;
        time_t    _age;
        Node(const char * n, T * d, time_t a = 0) 
        { 
            _name = n; 
            _data = d; 
            _age = a; 
            _reserved = false; 
        }
        ~Node() 
        { 
            if (_data) 
                delete _data; 
        }
    };

    //used to sort list of nodes based on age
    static int my_sort_cmp(Node * n1, Node * n2)
    {
        int retval = n1->_age - n2->_age;
        if (retval > 0)
        {
            //MVoidList::sort() is stupid...
            retval = 1;
        }

        return retval;
    };
    
    unsigned long    _maxElements; //_maxElements == 0 means unlimited
    MList<Node>      _list;
};

template <class T>
void MCache<T>::setMaxElements(unsigned long maxElem)
{
    _maxElements = maxElem;
}

template <class T>
bool MCache<T>::add(const char * name, T * data) 
{
    bool bRet = false;

    push(name, data);
    return bRet;
}

template <class T>
T * MCache<T>::get(const char *name) 
{
    T * data = NULL;
    data = find(name);
    return data;
}

//if returns non-NULL then considered a cache "hit"
//will move element to end on "hit" to keep in cache longer 
template <class T>
T * MCache<T>::reserve(const char * name)
{
    T * data = NULL;
    for (Node *n = (Node*)_list.first(); n; n = (Node*)_list.next())
    {
        if (n->_name == name)
        {
            //cache hit! keep in cache longer
            n->_reserved = true;
            n->_age = time(NULL);
            _list.sort(MCache::my_sort_cmp);
            data = n->_data;
            break;
        }
    }

    return data;
}

template <class T>
void MCache<T>::reserveAll()
{
    for (Node *n = (Node*)_list.first(); n; n = (Node*)_list.next())
    {
        n->_reserved = true;
    }
}

template <class T>
T * MCache<T>::unreserve(const char * name)
{
    T * data = NULL;
    for (Node *n = (Node*)_list.first(); n; n = (Node*)_list.next())
    {
        if (n->_name == name)
        {
            n->_reserved = false;
            data = n->_data;
            break;
        }
    }

    return data;
}

template <class T>
void MCache<T>::unreserveAll()
{
    for (Node *n = (Node*)_list.first(); n; n = (Node*)_list.next())
    {
        n->_reserved = false;
    }
}


template <class T>
bool MCache<T>::clear(bool force)
{
    bool   bRet = true;
    Node * n    = NULL;

    while (n = (Node*)_list.first())
    {
        //fprintf(stderr, "attempting to removing element:%s\n", n->_name.s());
        if (force)
        {
            n->_reserved = false;
        }

        if (n->_reserved) 
        {
            fprintf(stderr, "reserved item cannot clear(): %s (stopping clear)\n", n->_name.s());
            //reserved element exists!  
            bRet = false;
            break;
        }
        else
        {
            T * data = remove();
            if (data)
            {
                //fprintf(stderr, "deleting element data:%p\n", data);
                delete data;
                data = NULL;
            }
        }
    }
    return bRet;
}

template <class T>
void MCache<T>::print() 
{
    fprintf(stderr, "total elements:%d\n", _list.total());

    for (Node *n = (Node*)_list.first(); n; n = (Node*)_list.next())
    {
        fprintf(stderr,"name:%s reserved:%d\n", n->_name.s(), n->_reserved);
    }
}

//returns false if duplicate entry is reserved and cannot be replaced.
template <class T>
bool MCache<T>::push(const char * name, T * data)
{
    bool bRet = false;

    if (!name)
    {
        fprintf(stderr, "push() failed - invalid name\n");
    }

    if (!data)
    {
        fprintf(stderr, "push() failed - invalid data\n");
    }

    if (find(name))
    {
        //remove existing element with same name
        //fprintf(stderr, "push(): removing duplicate:%s\n", name);
        T * dataOld = remove(name);
        if (dataOld)
        {
            delete dataOld;
            dataOld = NULL;
        }
        else
        {
            //node to be deleted is currently reserved
            return bRet;
        }
    }

    if ((unsigned long)_list.total() >= _maxElements)
    {
        //fprintf(stderr,"push(): removing element to make room\n");
        T * dataOld = pop();
        if (dataOld)
        {
            delete dataOld;
            dataOld = NULL;
        }
        else
        {
            //all nodes are currently reserved
            fprintf(stderr,"push(): all elements reserved - cannot remove\n");
            return bRet;
        }
    }

    _list.add(new Node(name, data, time(NULL)));
    bRet = true;

    return bRet;
}

template <class T>
T * MCache<T>::pop()
{
    T    * data = NULL;

    for (Node *n = (Node*)_list.first(); n; n = (Node*)_list.next())
    {
        if (n->_reserved == false)
        {
            //fprintf(stderr,"pop(): popping:%s\n", n->_name.s());
            data = remove();
            break;
        }
        else
        {
            fprintf(stderr,"pop(): cannot pop:%s because it is reserved\n", n->_name.s());
        }
    }

    return data;
}

template <class T>
T * MCache<T>::find(const char *name) 
{
    T * data = NULL;
    for (Node *n = (Node*)_list.first(); n; n = (Node*)_list.next())
    {
        if (n->_name == name)
        {
            data = n->_data;
            break;
        }
    }

    return data;
}

// returns data from removed node, NULL otherwise.
// NULL return value will result if given node is reserved.
template <class T>
T * MCache<T>::remove(const char *name) 
{
    T * data = NULL;

    for (Node *n = (Node*)_list.first(); n; n = (Node*)_list.next())
    {
        if (n->_name == name)
        {
            data = remove();
            return data;
        }
    }

    return data;
}

template <class T>
T * MCache<T>::remove() 
{
    T * data = NULL;

    Node *n = (Node*)_list.current();
    
    if ((n->_reserved == false) && (n->_data))
    {
        Node *nDeleted = (Node*)_list.remove();
        data = nDeleted->_data;
        nDeleted->_data = NULL;
        delete nDeleted;
        nDeleted = NULL;
    }

    return data;
}
#endif
