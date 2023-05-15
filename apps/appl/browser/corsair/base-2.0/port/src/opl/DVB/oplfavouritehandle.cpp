
#include "oplfavouritehandle.h"
#include "cfavouritemanager.h"


/************************************************************************
 * class OplFavouriteListHandle
 ************************************************************************/

OplFavouriteListHandle::OplFavouriteListHandle(CFavouriteList* p)
	: p(p)
{
	p->registerHandle(this);
}

OplFavouriteListHandle::~OplFavouriteListHandle()
{
	p->unRegisterHandle(this);
}

const char* OplFavouriteListHandle::getFavID()
{
    snprintf(m_fav_id, sizeof(m_fav_id)-1, "%d", (int)p->getFavID());
	return (const char*) m_fav_id;
}

void OplFavouriteListHandle::setName(const char* aName)
{
    p->setName(aName);
}

const char* OplFavouriteListHandle::getName()
{
	return p->getName();
}

std::list<OPL_Channel_t> OplFavouriteListHandle::createChannelList()
{
    return p->createChannelList();
}

bool OplFavouriteListHandle::save(const std::list<const char*>& aCcids)
{
	return p->save(aCcids);
}

bool OplFavouriteListHandle::save(const std::list<const char*>& aCcids, const std::list<int>& aLcnList, int svcType)
{
	return p->save(aCcids, aLcnList, svcType);
}


/************************************************************************
 * class OplFavouriteManager
 ************************************************************************/

std::list<OplFavouriteListHandle*> OplFavouriteManager::createHandles()
{
	CFavouriteManager* p = CFavouriteManager::getInstance();
	return p->createHandles();
}

bool OplFavouriteManager::removeFavouriteList(OplFavouriteListHandle* aFavouriteListHandle)
{
	CFavouriteManager* p = CFavouriteManager::getInstance();
	return p->removeFavouriteList(aFavouriteListHandle);
}

OplFavouriteListHandle* OplFavouriteManager::getFavouriteList(int aFavID)
{
    CFavouriteManager* p = CFavouriteManager::getInstance();
    return p->getFavouriteList(aFavID);
}

OplFavouriteListHandle* OplFavouriteManager::createFavouriteList(const char* aName)
{
    CFavouriteManager* p = CFavouriteManager::getInstance();
    return p->createUserFavouriteList(aName);
}
