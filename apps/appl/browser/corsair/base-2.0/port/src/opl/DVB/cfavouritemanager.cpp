#include <dapi.h>
#include <dlib.h>
#include <algorithm>

#include "cfavouritemanager.h"
#include "oplchannel.h"


/************************************************************************
 * Internal utilities
 ************************************************************************/

static HINT32	compareLogicalNumber(const void *arg1, const void *arg2)
{
	//STEP;
	const CListItem_t *lhs = *(const CListItem_t **)arg1;
	const CListItem_t *rhs = *(const CListItem_t **)arg2;

	return (lhs->nLogicalNo - rhs->nLogicalNo);
}

static HINT32	compareUid(const void *arg1, const void *arg2)
{
	//STEP;
	const CListItem_t *lhs = *(const CListItem_t **)arg1;
	const CListItem_t *rhs = *(const CListItem_t **)arg2;

	return (lhs->uid - rhs->uid);
}

static bool compareById(CFavouriteList* aA, CFavouriteList* aB)
{
	return aA->getFavID() < aB->getFavID();
}

static void	commitRecord(HUINT32 uid, const HCHAR *pszGroupName, HxVector_t *pstTargetList)
{
    //STEP;
    HINT32	ssize, dsize;
    CList_t	*pstCList;
    DxGroupList_t	*groupList;

    pstCList = CLIB_Serialize(pstTargetList, &ssize);
    dsize = sizeof(DxGroupList_t) + ssize;

    groupList = (DxGroupList_t*)HLIB_STD_MemAlloc(dsize);
    groupList->nSize = dsize - sizeof(DxGroupList_t);
    HxSTD_MemCopy(groupList->pucData, pstCList, ssize);
    HxSTD_StrNCpy(groupList->szGroupName, pszGroupName, (DxNAME_LEN-1));

    DAPI_Set(DxNAMEOF(DxGroupList_t), uid, (const void*)groupList, dsize);
	HxLOG_Warning("[FAV] DAMA WRITE: uid=[%d], name=[%s], size [%d]\n", uid, groupList->szGroupName, HLIB_VECTOR_Length(pstTargetList));
    HLIB_STD_MemFree(groupList);
	HLIB_STD_MemFree(pstCList);
}

static std::list<TChannelId> makeUidList(const std::list<const char*>& aCcids)
{
	//STEP;
	std::list<TChannelId> ret;
	std::list<const char*>::const_iterator i(aCcids.begin());
	while(i!=aCcids.end())
	{
		const char* ccid = (*i++);
		int uid = OPL_Channel_CCID2UniqueID(ccid);
		HxLOG_Assert(uid);
		ret.push_back(uid);
	}
	return ret;
}

static bool isEqual(const std::list<TChannelId>& a, const std::list<TChannelId>& b)
{
	//STEP;
	if (a.size() != b.size()) {
		return false;
	}
	return std::equal(a.begin(), a.end(), b.begin());
}

static std::list<TChannelId> getRemovedList(const std::list<TChannelId>& original, const std::list<TChannelId>& target)
{
	//STEP;
	std::list<TChannelId> removed;
	std::list<TChannelId>::const_iterator i(original.begin());
	while (i != original.end()) {
		int uid = (*i++);
		if (std::find(target.begin(), target.end(), uid) == target.end()) {
			removed.push_back(uid);
		}
	}
	return removed;
}

/************************************************************************
 * class CFavouriteList
 ************************************************************************/

CFavouriteList::CFavouriteList(bool aNew)
	: mObsolete(false), mNew(aNew)
{
	//STEP;
}

CFavouriteList::~CFavouriteList()
{
	//STEP;
	HxLOG_Assert(mHandles.size() == 0);
}

int CFavouriteList::refCount()
{
	return mHandles.size();
}

void CFavouriteList::registerHandle(OplFavouriteListHandle* aHandle)
{
    STEP;
    mHandles.push_back(aHandle);
}

void CFavouriteList::unRegisterHandle(OplFavouriteListHandle* aHandle)
{
    STEP;
    mHandles.remove(aHandle);
    if (mObsolete && mHandles.size()  == 0)
    {
        CFavouriteManager* favMgr = CFavouriteManager::getInstance();
        favMgr->destroyFavouriteList(this);
    }
}

bool CFavouriteList::isObsolete()
{
	//STEP;
	return mObsolete;
}

void CFavouriteList::setObsolete()
{
	STEP;
	mObsolete = true;
}

bool CFavouriteList::isUserFavourite()
{
	return EFavouriteIdUserStart <= getFavID();
}

bool CFavouriteList::isNew()
{
	return mNew;
}

void CFavouriteList::clearNew()
{
	mNew = false;
}

/************************************************************************
 * class CFavouriteListDama
 ************************************************************************/

CFavouriteListDama::CFavouriteListDama(HUID aRecordId, const char* aName)
	: CFavouriteList(true), mRecordId(aRecordId), mName(NULL), mNameChanged(false)
{
	mName = HLIB_STD_StrDup(aName);
	print("from User");
}

CFavouriteListDama::CFavouriteListDama(HUID aRecordId, const char* aName, const std::list<const CListItem_t*>& aChannelInfos)
	: CFavouriteList(false), mRecordId(aRecordId), mName(NULL), mNameChanged(false)
{
	STEP;
	mName = HLIB_STD_StrDup(aName);
    std::list<const CListItem_t*>::const_iterator i(aChannelInfos.begin());
    while (i != aChannelInfos.end())
    {
        const CListItem_t* item = (*i++);
        mChannelInfos.push_back(item->uid);
    }
	print("from DB");
}

CFavouriteListDama::~CFavouriteListDama()
{
	//STEP;
	HxLOG_Warning("CFavouriteListDama destroyed\n");
	HLIB_STD_MemFree(mName);
}

HUID CFavouriteListDama::getFavID()
{
	//STEP;
	return mRecordId;
}

const char* CFavouriteListDama::getName()
{
	//STEP;
	return mName;
}

void CFavouriteListDama::setName(const char* aName)
{
	//STEP;
	if (mName) {
		HLIB_STD_MemFree(mName);
	}
	mName = HLIB_STD_StrDup(aName);
	mNameChanged = true;
}

std::list<OPL_Channel_t> CFavouriteListDama::createChannelList()
{
	//STEP;
	std::list<OPL_Channel_t> ret;
	std::list<TChannelId>::const_iterator i(mChannelInfos.begin());
	while (i != mChannelInfos.end()) {
        OPL_Channel_t channel = OPL_Channel_FindChannelByUID(NULL, (*i++));
        if (channel == NULL)
            continue;
        else
            ret.push_back(OPL_Channel_Clone(channel));
	}
	return ret;
}

bool CFavouriteListDama::save(const std::list<const char*>& aCcids)
{
    STEP;

	HxLOG_Warning("[FAV] CFavouriteList.save: uid=[%lld], name=[%s], size [%d]\n", mRecordId, mName, mChannelInfos.size());

	bool ret = true;

	std::list<TChannelId> target = makeUidList(aCcids);
	bool channelListChanged = !isEqual(target, mChannelInfos);

	// Channel removal from All, TV, Radio means removal from system channel list
	switch (mRecordId)
	{
	// for system favourite
	case EFavouriteIdTv:
	case EFavouriteIdRadio:
		if (channelListChanged)
		{
			CFavouriteManager* favMgr = CFavouriteManager::getInstance();
			std::list<TChannelId> removedList = getRemovedList(mChannelInfos, target);
			if (removedList.size()) {
				HxLOG_Warning("[FAV] CFavouriteList.save: system channel removal\n");
				favMgr->handleChannelRemove(mRecordId, mName, removedList);
			}
			HxLOG_Warning("[FAV] CFavouriteList.save: favourite list will be regenarated\n");
			mChannelInfos = target;
			favMgr->buildWithFavourite();
		}
		break;

	// we do not allow these to be edited
	case EFavouriteIdAll:
	case EFavouriteIdHdTv:
	case EFavouriteIdFta:
	case EFavouriteIdCas:
#if defined(CONFIG_OP_MIDDLE_EAST) || defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE)	// Support Alphabet Group List
	case EFavouriteIdAG_ALL:
	case EFavouriteIdAG_A:
	case EFavouriteIdAG_B:
	case EFavouriteIdAG_C:
	case EFavouriteIdAG_D:
	case EFavouriteIdAG_E:
	case EFavouriteIdAG_F:
	case EFavouriteIdAG_G:
	case EFavouriteIdAG_H:
	case EFavouriteIdAG_I:
	case EFavouriteIdAG_J:
	case EFavouriteIdAG_K:
	case EFavouriteIdAG_L:
	case EFavouriteIdAG_M:
	case EFavouriteIdAG_N:
	case EFavouriteIdAG_O:
	case EFavouriteIdAG_P:
	case EFavouriteIdAG_Q:
	case EFavouriteIdAG_R:
	case EFavouriteIdAG_S:
	case EFavouriteIdAG_T:
	case EFavouriteIdAG_U:
	case EFavouriteIdAG_V:
	case EFavouriteIdAG_W:
	case EFavouriteIdAG_X:
	case EFavouriteIdAG_Y:
	case EFavouriteIdAG_Z:
	case EFavouriteIdAG_Digit:
	case EFavouriteIdAG_Etc:
#endif
#ifdef CONFIG_DEBUG
		HxLOG_Assert(true);
#else
		ret = false;
#endif
		break;

	// for user favourite
	default:
		if (isNew() || mNameChanged || channelListChanged)
		{
			HxLOG_Warning("[FAV] CFavouriteList.save: udpate started\n");
			mChannelInfos = target;
			update();
		}
	}

	return ret;
}

bool CFavouriteListDama::save(const std::list<const char*>& aCcids, const std::list<int>& aLcnList, int svcType)
{
    STEP;

	HxLOG_Warning("[FAV] CFavouriteList.save: uid=[%lld], name=[%s], size [%d]\n", mRecordId, mName, mChannelInfos.size());

	bool ret = true;

	std::list<TChannelId> target = makeUidList(aCcids);
	bool channelListChanged = !isEqual(target, mChannelInfos);

	// Channel removal from All, TV, Radio means removal from system channel list
	switch (mRecordId)
	{
	// for system favourite
	case EFavouriteIdTv:
	case EFavouriteIdRadio:
		if (channelListChanged)
		{
			CFavouriteManager* favMgr = CFavouriteManager::getInstance();
			std::list<TChannelId> removedList = getRemovedList(mChannelInfos, target);
			if (removedList.size()) {
				HxLOG_Warning("[FAV] CFavouriteList.save: system channel removal\n");
				favMgr->handleChannelRemove(mRecordId, mName, removedList);
			}
			HxLOG_Warning("[FAV] CFavouriteList.save: favourite list will be regenarated\n");
			mChannelInfos = target;
			favMgr->buildWithFavourite(aLcnList, svcType);
		}
		break;

	// we do not allow these to be edited
	case EFavouriteIdAll:
	case EFavouriteIdHdTv:
	case EFavouriteIdFta:
	case EFavouriteIdCas:
#ifdef CONFIG_DEBUG
		HxLOG_Assert(true);
#else
		ret = false;
#endif
		break;

	// for user favourite
	default:
		if (isNew() || mNameChanged || channelListChanged)
		{
			HxLOG_Warning("[FAV] CFavouriteList.save: udpate started\n");
			mChannelInfos = target;
			update();
		}
	}

	return ret;
}

void CFavouriteListDama::update()
{
	CFavouriteManager::getInstance()->updateFavouriteList(mRecordId, mName, mChannelInfos);
}

void CFavouriteListDama::print(const char* aFunc)
{
	HxLOG_Info("[FAV] %s: uid=[%lld]\tname=[%s]\tsize=[%d]\tref=[%d]\tobsolete=%d,new=%d\n", aFunc, mRecordId, getName(), mChannelInfos.size(), refCount(),isObsolete(), isNew());
	/*
	std::list<TChannelId>::const_iterator i(mChannelInfos.begin());
	while (i!=mChannelInfos.end())
	{
		HxLOG_Warning("[FAV]   uid=%d\n", (*i++));
	}
	*/
}

void CFavouriteListDama::remove()
{
	setObsolete();
	DAPI_Reset(DxNAMEOF(DxGroupList_t), getFavID());
	print("remove");
}

/************************************************************************
 * class CFavouriteManager
 ************************************************************************/

static CFavouriteManager* gFavouriteMgr(NULL);

CFavouriteManager::CFavouriteManager()
	: mChannelMgr(NULL), mRestoring(false)
{
	//STEP;
    mChannelMgr = OPL_Channel_GetManager();
    restoreSync();
}

CFavouriteManager::~CFavouriteManager()
{

	//STEP;

}

CFavouriteManager* CFavouriteManager::getInstance()
{
	//STEP;
	if (gFavouriteMgr == NULL)
	{
		gFavouriteMgr = new CFavouriteManager();
	}
	return gFavouriteMgr;
}

/////////////////////////////////////////////////////////////////////////
// API

std::list<OplFavouriteListHandle*> CFavouriteManager::createHandles()
{
	//STEP;
	std::list<OplFavouriteListHandle*> ret;
    std::list<CFavouriteList*>::iterator i(mCollection.begin());
	while (i != mCollection.end()) {
        CFavouriteList* favList = (*i++);
		if (!favList->isObsolete()) {
            ret.push_back(new OplFavouriteListHandle(favList));
		}
	}
	HxLOG_Warning("[FAV] CFavouriteManager::createHandles %d\n", ret.size());
	return ret;
}

OplFavouriteListHandle* CFavouriteManager::createUserFavouriteList(const char* aName)
{
	//STEP;
	CFavouriteList* item = new CFavouriteListDama(allocUserFavId(), aName);
	mCollection.push_back(item);
	OplFavouriteListHandle* ret = new OplFavouriteListHandle(item);
	return ret;
}

bool CFavouriteManager::removeFavouriteList(OplFavouriteListHandle* aFavouriteListHandle)
{
	//STEP;
	CFavouriteListDama* favList = reinterpret_cast<CFavouriteListDama*>(aFavouriteListHandle->p);
	favList->remove();
	return true;
}

OplFavouriteListHandle* CFavouriteManager::getFavouriteList(int aFavID)
{
	//STEP;
	CFavouriteList* item = getFavouriteListById(aFavID);
	HxLOG_Warning("[FAV] CFavouriteManager::getFavouriteList %d\n", aFavID);
	if (item)
	{
		return new OplFavouriteListHandle(item);
	}
	return NULL;
}

/////////////////////////////////////////////////////////////////////////

// Internal API for OplFavouriteListPrivate
int CFavouriteManager::getLCN(const int aUid)
{
	//STEP;
	std::map<TChannelId, CListItem_t>::iterator item = mAllList.find(aUid);
	CListItem_t listItem = item->second;
	return listItem.nLogicalNo;
}

HUID CFavouriteManager::allocUserFavId()
{
	// First sort CFavouriteList by favourite id.
	mCollection.sort(compareById);

	// Find lowest available user favourite id
	std::list<CFavouriteList*>::const_iterator i(mCollection.begin());
	HUID newUserFavId = EFavouriteIdUserStart;
	while(i != mCollection.end()) {
		CFavouriteList* item = (*i++);
		if(!item->isObsolete() && item->isUserFavourite())
		{
			if (item->getFavID() == newUserFavId)
			{
				newUserFavId++;
			}
			else
			{
				break;
			}
		}
	}
	return newUserFavId;
}

CFavouriteList* CFavouriteManager::getFavouriteListById(int aFavId)
{
	std::list<CFavouriteList*>::const_iterator i(mCollection.begin());
	CFavouriteList* ret = NULL;
	while(i != mCollection.end()) {
		CFavouriteList* item = (*i++);
		if(!item->isObsolete() && item->getFavID() == aFavId) {
			ret = item;
			break;
		}
	}
	return ret;
}

void CFavouriteManager::createFavouriteList(HUID aRecordId, const char* aName, const std::list<const CListItem_t*>& aListItem)
{
	//STEP;

	// Create CFavouriteList
	CFavouriteListDama* favList = new CFavouriteListDama(aRecordId, aName, aListItem);

	// Make previouse CFavouriteList instance obsolete
	std::list<CFavouriteList*>::iterator i(mCollection.begin());
    while(i != mCollection.end()) {
		CFavouriteList* item = (*i);
		if(item->getFavID() == aRecordId)
        {
			if (item->refCount())
			{
				item->setObsolete();
				item->clearNew();
				++i;
			}
			else
			{
				mCollection.erase(i++);
				delete item;
			}
        }
		else
		{
			++i;
		}
    }

	mCollection.push_back(favList);

	// Create mapping for channel id <-> LCN
	if (aRecordId == EFavouriteIdAll) {
		mAllList.clear();
		std::list<const CListItem_t*>::const_iterator i(aListItem.begin());
		while (i != aListItem.end()) {
			const CListItem_t* item = (*i++);
			mAllList.insert(std::map<TChannelId, CListItem_t>::value_type((int)item->uid, *item));
		}
	}
}

void CFavouriteManager::destroyFavouriteList(CFavouriteList* aFavouriteList)
{
	STEP;
	mCollection.remove(aFavouriteList);
	delete aFavouriteList;
	print("destroyFavouriteList");
}

void CFavouriteManager::updateFavouriteList(HUID aRecordId, const char* aName, const std::list<TChannelId>& aUids)
{
	STEP;
	HxVector_t* channelInfoList = HLIB_VECTOR_NewEasy(aUids.size(), NULL, NULL);
	std::list<TChannelId>::const_iterator i(aUids.begin());
	while (i != aUids.end()) {
		int uid = (*i++);
		HLIB_VECTOR_Add(channelInfoList, &(mAllList[uid]));
   	}
	commitRecord(aRecordId, aName, channelInfoList);

	restoreSync(aRecordId);

    HLIB_VECTOR_Delete(channelInfoList);
}

void CFavouriteManager::handleChannelRemove(HUID aRecordId, const char* aName, const std::list<TChannelId>& aUids)
{
	HUINT32 ulModelId = EModelId_None, ulActionID = CLIB_OP_NULL;
    	STEP;

	getModelID(&ulModelId, &ulActionID);

	// Remove channel info from all favourite list
	std::list<CFavouriteList*>::const_iterator i(mCollection.begin());
	std::list<CFavouriteList*> userFavs;
	while(i != mCollection.end())
	{
		CFavouriteListDama* item = (CFavouriteListDama*)(*i++);
		std::list<TChannelId>::const_iterator j(aUids.begin());
		while(j != aUids.end())
		{
			item->mChannelInfos.remove(*j++);
		}
		if (item->isUserFavourite() && !item->isObsolete())
		{
			userFavs.push_back(item);
		}
	}

	std::list<CFavouriteList*>::const_iterator j(userFavs.begin());
	while (j != userFavs.end())
	{
		CFavouriteListDama* item = (CFavouriteListDama*)(*j++);
		item->update();
	}

	// Remove system channel
	std::list<TChannelId>::const_iterator k(aUids.begin());
    	int index=0;
	while (k != aUids.end())
	{
		int uid = (*k++);
        	OPL_Channel_t channel = OPL_Channel_FindChannelByUID(mChannelMgr, uid);
		HxLOG_Assert(channel);

		if (ulModelId	== EModelId_AUS || ulModelId	== EModelId_ITA_DTT)
			OPL_Channel_SetRemovedFlag(channel, OPL_TRUE);
		else
      		OPL_Channel_RemoveChannel(mChannelMgr, channel);
        	index++;
    }
}

/////////////////////////////////////////////////////////////////////////
// Internal


bool CFavouriteManager::reset()
{
	HxLOG_Warning("[FAV] DAMA: RESET\n");
	DERROR ret;

	std::list<CFavouriteList*>::iterator i(mCollection.begin());
	while(i != mCollection.end()) {
		CFavouriteListDama* f = reinterpret_cast<CFavouriteListDama*>(*i);
		f->remove();
		if (f->refCount() == 0)
		{
			delete f;
			mCollection.erase(i++);
		}
		else
		{
			++i;
		}
	}

	ret = DAPI_ResetTable(DxNAMEOF(DxGroupList_t));
	HxLOG_Assert(ret == DERR_OK);
	ret = DAPI_Sync(DxNAMEOF(DxGroupList_t));
	HxLOG_Assert(ret == DERR_OK);
	ret = DAPI_ResetTable(DxNAMEOF(DxGroupList_t));
	HxLOG_Assert(ret == DERR_OK);
	ret = DAPI_Sync(DxNAMEOF(DxGroupList_t));
	HxLOG_Assert(ret == DERR_OK);

	return true;
}

static HINT32 favMgr_CompUids(const void *pvData1, const void *pvData2)
{
	DxService_t	*pstData1 = *(DxService_t**)pvData1;
	DxService_t	*pstData2 = *(DxService_t**)pvData2;

	return pstData1->uid - pstData2->uid;
}

static HINT32 favMgr_CompLcnNumbers(const void *pvData1, const void *pvData2)
{
	DxService_t	*pstData1 = *(DxService_t**)pvData1;
	DxService_t	*pstData2 = *(DxService_t**)pvData2;

	return pstData1->lcn - pstData2->lcn;
}

void CFavouriteManager::buildWithSystemChannels()
{
    HUINT32 ulModelId = EModelId_None, ulActionID = CLIB_OP_NULL;

    getModelID(&ulModelId, &ulActionID);

    // Retrieve system channel list
    OPL_Channel_t* channelList = NULL;
    int count = OPL_Channel_GetChannelList(mChannelMgr, &channelList);

    HxLOG_Warning("[FAV] buildWithSystemChannels %d\n", count);
    if ( count <=  0 || channelList == NULL )
    {
        return;
    }
    
    // Make channel list without lcnFlag
    HxVector_t* pstSvcList = HLIB_VECTOR_NewEasy(count, HLIB_STD_MemFree_CB, NULL);
    if ( pstSvcList != NULL )
    {
    	for (int i=0; i<count; i++) 
    	{
        	DxService_t* service = (DxService_t*)HLIB_STD_MemDup(channelList[i], sizeof(DxService_t));
        	if ((ulModelId == EModelId_HMS) && (service->lcn >= 100))
        	{
        		service->lcnFlag = false;
        	}
        	else if ((ulModelId == EModelId_AUS) && (service->lcn >= 350 && service->lcn <= 399))
        	{
        		service->lcnFlag = false;
        	}
        	else if ((ulModelId == EModelId_ZIGGO) && (service->lcn >= 1000))
        	{
        		service->lcnFlag = false;
        	}
        	else if ((ulModelId == EModelId_UK_DTT) && (service->lcn >= 800))
        	{
        		service->lcnFlag = false;
        	}
        	else if ((ulModelId == EModelId_ITA_DTT) && (service->lcn >= 850 && service->lcn <= 999))
        	{
        		service->lcnFlag = false;
        	}

        	HLIB_VECTOR_Add(pstSvcList, service);
        	OPL_Channel_Delete(channelList[i]);
        }
        
    	HLIB_VECTOR_Sort(pstSvcList, favMgr_CompLcnNumbers);
    	build(pstSvcList);
    	HLIB_VECTOR_Delete(pstSvcList);
    }
    // Clean up
    if ( channelList != NULL )
        HLIB_STD_MemFree(channelList);
}

bool CFavouriteManager::buildWithFavourite()
{
	HUINT32 ulModelId = EModelId_None, ulActionID = CLIB_OP_NULL;

	getModelID(&ulModelId, &ulActionID);

	// Make channel list from favourite list
	CFavouriteListDama* favTv = (CFavouriteListDama*)getFavouriteListById(EFavouriteIdTv);
	CFavouriteListDama* favRadio = (CFavouriteListDama*)getFavouriteListById(EFavouriteIdRadio);
	std::list<TChannelId> allChannel = favTv->mChannelInfos;
	allChannel.merge(favRadio->mChannelInfos);

	// Make channel list without lcnFlag
	HxVector_t* pstSvcList = HLIB_VECTOR_NewEasy(allChannel.size(), HLIB_STD_MemFree_CB, NULL);
	std::list<TChannelId>::const_iterator i(allChannel.begin());
	while (i != allChannel.end())
	{
		TChannelId channeId = (*i++);
		OPL_Channel_t oplChannel = OPL_Channel_FindChannelByUID(mChannelMgr, channeId);
		HxLOG_Assert(oplChannel);
		DxService_t* service = (DxService_t*)HLIB_STD_MemDup(oplChannel, sizeof(DxService_t));

		if ((ulModelId == EModelId_HMS) && (service->lcn >= 100))
		{
			service->lcnFlag = false;
		}
		else if ((ulModelId == EModelId_AUS) && (service->lcn >= 350 && service->lcn <= 399))
		{
			service->lcnFlag = false;
		}
		else if ((ulModelId == EModelId_ZIGGO) && (service->lcn >= 1000))
		{
			service->lcnFlag = false;
		}
		else if ((ulModelId == EModelId_UK_DTT) && (service->lcn >= 800))
		{
			service->lcnFlag = false;
		}
		else if ((ulModelId == EModelId_ITA_DTT) && (service->lcn >= 850 && service->lcn <= 999))
		{
			service->lcnFlag = false;
		}

		HLIB_VECTOR_Add(pstSvcList, service);
	}

#if 0 // It makes a move issue on medu edit channel.
	//	Reference list is sorted with service uid.
	HLIB_VECTOR_Sort(pstSvcList, favMgr_CompUids);
#endif

	// Build new channel
	build(pstSvcList);

	// Clean up
	HLIB_VECTOR_Delete(pstSvcList);

	return true;
}

bool CFavouriteManager::buildWithFavourite(const std::list<int>& aLcnList, int svcType)
{
	#define UNDEFINED_UID 0xFFFF

	// Make channel list from favourite list
	CFavouriteListDama* favTv = (CFavouriteListDama*)getFavouriteListById(EFavouriteIdTv);
	CFavouriteListDama* favRadio = (CFavouriteListDama*)getFavouriteListById(EFavouriteIdRadio);
	std::list<TChannelId> allChannel;
	int oldLcn, newLcn;

	allChannel = favTv->mChannelInfos;
	allChannel.merge(favRadio->mChannelInfos);

	// Make channel list without lcnFlag
	HxVector_t* pstSvcList = HLIB_VECTOR_NewEasy(allChannel.size(), HLIB_STD_MemFree_CB, NULL);
	std::list<TChannelId>::const_iterator i(allChannel.begin());
	std::list<int>::const_iterator j(aLcnList.begin());
	while (i != allChannel.end())
	{
		TChannelId channeId = (*i++);
		OPL_Channel_t oplChannel = OPL_Channel_FindChannelByUID(mChannelMgr, channeId);
		HxLOG_Assert(oplChannel);
		DxService_t* service = (DxService_t*)HLIB_STD_MemDup(oplChannel, sizeof(DxService_t));
		oldLcn = service->lcn;
		/* 동일 service type의 변경된 service no를 할당 한다. */
		if (svcType == service->svcType)
		{
			newLcn = oldLcn;
			if (j != aLcnList.end())
				newLcn = (*j++);

			if ( (oldLcn != newLcn) && (newLcn < 100) )
			{
				printf ("\n\t will be changed the service no from %d to %d and the name is %s\n", oldLcn, newLcn, service->name);

				service->lcn = newLcn;
				service->lcnFlag = TRUE;
				service->svcSection = eDxSvcSection_PreProgrammed;
				if (oldLcn >= 100)
				{
					// add
					service->uid = UNDEFINED_UID;
					OPL_Channel_AddChannel(mChannelMgr, (OPL_Channel_t)service);
					printf ("\n\t will be added the service no %d and the name is %s, uid = 0x%x\n", service->lcn, service->name, service->uid);
				}
				else
				{
					// move
					printf ("\n\t will be moved the service no %d and the name is %s\n", ((DxService_t*)oplChannel)->lcn, ((DxService_t*)oplChannel)->name);
					OPL_Channel_SetMajorChannel(oplChannel, newLcn);
				}
			}
		}

		if((service->lcn >= 100) && (service->svcSection != eDxSvcSection_Lcn))
			service->lcnFlag = FALSE;
		else
			service->lcnFlag = TRUE;
		HLIB_VECTOR_Add(pstSvcList, service);
	}

	// Build new channel
	build(pstSvcList);

	// Clean up
	HLIB_VECTOR_Delete(pstSvcList);
	return TRUE;
}

bool CFavouriteManager::build(HxVector_t* pstSvcList)
{
    HUINT32	ulModelId = EModelId_None, ulActionID = CLIB_OP_NULL;
    int tvLen = 0, radioLen = 0;

    STEP;
    mChannelMgr = NULL;

    getModelID(&ulModelId, &ulActionID);

    HxVector_t* pstTVList = CLIB_GenerateList(pstSvcList, NULL, CLIB_TYPE_TV, ulActionID, 0, TRUE);
    commitRecord(EFavouriteIdTv, "TV", pstTVList);

    HxVector_t* pstRadioList = CLIB_GenerateList(pstSvcList, NULL, CLIB_TYPE_RADIO, ulActionID, 0, TRUE);
    commitRecord(EFavouriteIdRadio, "RADIO", pstRadioList);

    HxVector_t* pstHDList    = CLIB_GenerateList(pstSvcList, pstTVList, CLIB_TYPE_HD, ulActionID, 0, TRUE);
    commitRecord(EFavouriteIdHdTv, "HDTV", pstHDList);

    if (pstTVList)
        tvLen = HLIB_VECTOR_Length(pstTVList);
    if (pstRadioList)
        radioLen = HLIB_VECTOR_Length(pstRadioList);

    HxVector_t* pstFullList = HLIB_VECTOR_NewEasy(tvLen+radioLen, NULL, NULL);
    for (HINT32 i = 0; i < tvLen; i++) {
    	HLIB_VECTOR_Add(pstFullList, (void*)HLIB_VECTOR_ItemAt(pstTVList, i));
    }
    for (HINT32 i = 0; i < radioLen; i++) {
    	HLIB_VECTOR_Add(pstFullList, (void*)HLIB_VECTOR_ItemAt(pstRadioList, i));
    }
    HLIB_VECTOR_Sort(pstFullList, compareLogicalNumber);
    commitRecord(EFavouriteIdAll, "ALL", pstFullList);

    HxVector_t* pstFreeList  = CLIB_GenerateList(pstSvcList, pstFullList, CLIB_TYPE_CAS, ulActionID, eDxCAS_TYPE_FTA, TRUE);
    commitRecord(EFavouriteIdFta, "FREE", pstFreeList);

    HxVector_t* pstCasList   = CLIB_GenerateList(pstSvcList, pstFullList, CLIB_TYPE_CAS, ulActionID, 0xFFFE, TRUE);
    commitRecord(EFavouriteIdCas, "PAY", pstCasList);

#if defined(CONFIG_OP_MIDDLE_EAST) || defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE)	// Support Alphabet Group List
    HxVector_t* pstAlpabetAllList   = CLIB_GenerateList(pstSvcList, pstFullList, CLIB_TYPE_ALPABET, ulActionID, eDxAlphabetGroup_AlphabetAll, TRUE);
    commitRecord(EFavouriteIdAG_ALL, "AGALL", pstAlpabetAllList);

    HxVector_t* pstAlpabetAList   = CLIB_GenerateList(pstSvcList, pstFullList, CLIB_TYPE_ALPABET, ulActionID, eDxAlphabetGroup_AlphabetA, TRUE);
    commitRecord(EFavouriteIdAG_A, "AGA", pstAlpabetAList);

    HxVector_t* pstAlpabetBList   = CLIB_GenerateList(pstSvcList, pstFullList, CLIB_TYPE_ALPABET, ulActionID, eDxAlphabetGroup_AlphabetB, TRUE);
    commitRecord(EFavouriteIdAG_B, "AGB", pstAlpabetBList);

    HxVector_t* pstAlpabetCList   = CLIB_GenerateList(pstSvcList, pstFullList, CLIB_TYPE_ALPABET, ulActionID, eDxAlphabetGroup_AlphabetC, TRUE);
    commitRecord(EFavouriteIdAG_C, "AGC", pstAlpabetCList);

    HxVector_t* pstAlpabetDList   = CLIB_GenerateList(pstSvcList, pstFullList, CLIB_TYPE_ALPABET, ulActionID, eDxAlphabetGroup_AlphabetD, TRUE);
    commitRecord(EFavouriteIdAG_D, "AGD", pstAlpabetDList);

    HxVector_t* pstAlpabetEList   = CLIB_GenerateList(pstSvcList, pstFullList, CLIB_TYPE_ALPABET, ulActionID, eDxAlphabetGroup_AlphabetE, TRUE);
    commitRecord(EFavouriteIdAG_E, "AGE", pstAlpabetEList);

    HxVector_t* pstAlpabetFList   = CLIB_GenerateList(pstSvcList, pstFullList, CLIB_TYPE_ALPABET, ulActionID, eDxAlphabetGroup_AlphabetF, TRUE);
    commitRecord(EFavouriteIdAG_F, "AGF", pstAlpabetFList);

    HxVector_t* pstAlpabetGList   = CLIB_GenerateList(pstSvcList, pstFullList, CLIB_TYPE_ALPABET, ulActionID, eDxAlphabetGroup_AlphabetG, TRUE);
    commitRecord(EFavouriteIdAG_G, "AGG", pstAlpabetGList);

    HxVector_t* pstAlpabetHList   = CLIB_GenerateList(pstSvcList, pstFullList, CLIB_TYPE_ALPABET, ulActionID, eDxAlphabetGroup_AlphabetH, TRUE);
    commitRecord(EFavouriteIdAG_H, "AGH", pstAlpabetHList);

    HxVector_t* pstAlpabetIList   = CLIB_GenerateList(pstSvcList, pstFullList, CLIB_TYPE_ALPABET, ulActionID, eDxAlphabetGroup_AlphabetI, TRUE);
    commitRecord(EFavouriteIdAG_I, "AGI", pstAlpabetIList);

    HxVector_t* pstAlpabetJList   = CLIB_GenerateList(pstSvcList, pstFullList, CLIB_TYPE_ALPABET, ulActionID, eDxAlphabetGroup_AlphabetJ, TRUE);
    commitRecord(EFavouriteIdAG_J, "AGJ", pstAlpabetJList);

    HxVector_t* pstAlpabetKList   = CLIB_GenerateList(pstSvcList, pstFullList, CLIB_TYPE_ALPABET, ulActionID, eDxAlphabetGroup_AlphabetK, TRUE);
    commitRecord(EFavouriteIdAG_K, "AGK", pstAlpabetKList);

    HxVector_t* pstAlpabetLList   = CLIB_GenerateList(pstSvcList, pstFullList, CLIB_TYPE_ALPABET, ulActionID, eDxAlphabetGroup_AlphabetL, TRUE);
    commitRecord(EFavouriteIdAG_L, "AGL", pstAlpabetLList);

    HxVector_t* pstAlpabetMList   = CLIB_GenerateList(pstSvcList, pstFullList, CLIB_TYPE_ALPABET, ulActionID, eDxAlphabetGroup_AlphabetM, TRUE);
    commitRecord(EFavouriteIdAG_M, "AGM", pstAlpabetMList);

    HxVector_t* pstAlpabetNList   = CLIB_GenerateList(pstSvcList, pstFullList, CLIB_TYPE_ALPABET, ulActionID, eDxAlphabetGroup_AlphabetN, TRUE);
    commitRecord(EFavouriteIdAG_N, "AGN", pstAlpabetNList);

    HxVector_t* pstAlpabetOList   = CLIB_GenerateList(pstSvcList, pstFullList, CLIB_TYPE_ALPABET, ulActionID, eDxAlphabetGroup_AlphabetO, TRUE);
    commitRecord(EFavouriteIdAG_O, "AGO", pstAlpabetOList);

    HxVector_t* pstAlpabetPList   = CLIB_GenerateList(pstSvcList, pstFullList, CLIB_TYPE_ALPABET, ulActionID, eDxAlphabetGroup_AlphabetP, TRUE);
    commitRecord(EFavouriteIdAG_P, "AGP", pstAlpabetPList);

    HxVector_t* pstAlpabetQList   = CLIB_GenerateList(pstSvcList, pstFullList, CLIB_TYPE_ALPABET, ulActionID, eDxAlphabetGroup_AlphabetQ, TRUE);
    commitRecord(EFavouriteIdAG_Q, "AGQ", pstAlpabetQList);

    HxVector_t* pstAlpabetRList   = CLIB_GenerateList(pstSvcList, pstFullList, CLIB_TYPE_ALPABET, ulActionID, eDxAlphabetGroup_AlphabetR, TRUE);
    commitRecord(EFavouriteIdAG_R, "AGR", pstAlpabetRList);

    HxVector_t* pstAlpabetSList   = CLIB_GenerateList(pstSvcList, pstFullList, CLIB_TYPE_ALPABET, ulActionID, eDxAlphabetGroup_AlphabetS, TRUE);
    commitRecord(EFavouriteIdAG_S, "AGS", pstAlpabetSList);

    HxVector_t* pstAlpabetTList   = CLIB_GenerateList(pstSvcList, pstFullList, CLIB_TYPE_ALPABET, ulActionID, eDxAlphabetGroup_AlphabetT, TRUE);
    commitRecord(EFavouriteIdAG_T, "AGT", pstAlpabetTList);

    HxVector_t* pstAlpabetUList   = CLIB_GenerateList(pstSvcList, pstFullList, CLIB_TYPE_ALPABET, ulActionID, eDxAlphabetGroup_AlphabetU, TRUE);
    commitRecord(EFavouriteIdAG_U, "AGU", pstAlpabetUList);

    HxVector_t* pstAlpabetVList   = CLIB_GenerateList(pstSvcList, pstFullList, CLIB_TYPE_ALPABET, ulActionID, eDxAlphabetGroup_AlphabetV, TRUE);
    commitRecord(EFavouriteIdAG_V, "AGV", pstAlpabetVList);

    HxVector_t* pstAlpabetWList   = CLIB_GenerateList(pstSvcList, pstFullList, CLIB_TYPE_ALPABET, ulActionID, eDxAlphabetGroup_AlphabetW, TRUE);
    commitRecord(EFavouriteIdAG_W, "AGW", pstAlpabetWList);

    HxVector_t* pstAlpabetXList   = CLIB_GenerateList(pstSvcList, pstFullList, CLIB_TYPE_ALPABET, ulActionID, eDxAlphabetGroup_AlphabetX, TRUE);
    commitRecord(EFavouriteIdAG_X, "AGX", pstAlpabetXList);

    HxVector_t* pstAlpabetYList   = CLIB_GenerateList(pstSvcList, pstFullList, CLIB_TYPE_ALPABET, ulActionID, eDxAlphabetGroup_AlphabetY, TRUE);
    commitRecord(EFavouriteIdAG_Y, "AGY", pstAlpabetYList);

    HxVector_t* pstAlpabetZList   = CLIB_GenerateList(pstSvcList, pstFullList, CLIB_TYPE_ALPABET, ulActionID, eDxAlphabetGroup_AlphabetZ, TRUE);
    commitRecord(EFavouriteIdAG_Z, "AGZ", pstAlpabetZList);

    HxVector_t* pstAlpabetDigitList   = CLIB_GenerateList(pstSvcList, pstFullList, CLIB_TYPE_ALPABET, ulActionID, eDxAlphabetGroup_AlphabetDigit, TRUE);
    commitRecord(EFavouriteIdAG_Digit, "AGDIGIT", pstAlpabetDigitList);

    HxVector_t* pstAlpabetEtcList   = CLIB_GenerateList(pstSvcList, pstFullList, CLIB_TYPE_ALPABET, ulActionID, eDxAlphabetGroup_AlphabetEtc, TRUE);
    commitRecord(EFavouriteIdAG_Etc, "AGETC", pstAlpabetEtcList);

    CLIB_RemoveItemList(pstAlpabetAllList);
    CLIB_RemoveItemList(pstAlpabetAList);
    CLIB_RemoveItemList(pstAlpabetBList);
    CLIB_RemoveItemList(pstAlpabetCList);
    CLIB_RemoveItemList(pstAlpabetDList);
    CLIB_RemoveItemList(pstAlpabetEList);
    CLIB_RemoveItemList(pstAlpabetFList);
    CLIB_RemoveItemList(pstAlpabetGList);
    CLIB_RemoveItemList(pstAlpabetHList);
    CLIB_RemoveItemList(pstAlpabetIList);
    CLIB_RemoveItemList(pstAlpabetJList);
    CLIB_RemoveItemList(pstAlpabetKList);
    CLIB_RemoveItemList(pstAlpabetLList);
    CLIB_RemoveItemList(pstAlpabetMList);
    CLIB_RemoveItemList(pstAlpabetNList);
    CLIB_RemoveItemList(pstAlpabetOList);
    CLIB_RemoveItemList(pstAlpabetPList);
    CLIB_RemoveItemList(pstAlpabetQList);
    CLIB_RemoveItemList(pstAlpabetRList);
    CLIB_RemoveItemList(pstAlpabetSList);
    CLIB_RemoveItemList(pstAlpabetTList);
    CLIB_RemoveItemList(pstAlpabetUList);
    CLIB_RemoveItemList(pstAlpabetVList);
    CLIB_RemoveItemList(pstAlpabetWList);
    CLIB_RemoveItemList(pstAlpabetXList);
    CLIB_RemoveItemList(pstAlpabetYList);
    CLIB_RemoveItemList(pstAlpabetZList);
    CLIB_RemoveItemList(pstAlpabetDigitList);
    CLIB_RemoveItemList(pstAlpabetEtcList);
#endif

    CLIB_RemoveItemList(pstTVList);
    CLIB_RemoveItemList(pstRadioList);
    CLIB_RemoveItemList(pstHDList);
    CLIB_RemoveItemList(pstFreeList);
    CLIB_RemoveItemList(pstCasList);
    HLIB_VECTOR_Delete(pstFullList);
    restoreSync();
    return true;
}

HUINT32 CFavouriteManager::restore(HUID aRecordId)
{
	STEP;
	HxLOG_Assert(!mRestoring);
	mRestoring = true;
	if (aRecordId)
	{
		return DAPI_Get(DxNAMEOF(DxGroupList_t), aRecordId, CFavouriteManager::restoreCb, this);
	}
	else
	{
		return DAPI_GetAll(DxNAMEOF(DxGroupList_t), CFavouriteManager::restoreCb, this);
	}
}

bool CFavouriteManager::restoreSync(HUID aRecordId)
{
	STEP;
	HUINT32 reqId = restore(aRecordId);
    DAPI_Wait(120000, reqId);
    DAPI_Sync(DxNAMEOF(DxGroupList_t));
	return true;
}

void CFavouriteManager::restoreCb(const HCHAR* aName, HUID aUid, const void* aData, HUINT32 aSize, void* aSelf)
{
    //STEP;
    DAxIterator_t	iter;
    DAPI_InitIterator(&iter, aData, aSize, 0);
    CFavouriteManager* favMgr = static_cast<CFavouriteManager*>(aSelf);
    while (DAPI_NextIterator(&iter))
    {
    	DxGroupList_t* groupInfo = (DxGroupList_t*)iter.data;
    	std::list<const CListItem_t*> channelList;
        if ( groupInfo )
        {
    		HxVector_t* groupList = CLIB_DeSerialize((CList_t*)groupInfo->pucData);
    		for (HINT32 i = 0; i < HLIB_VECTOR_Length(groupList); i++)
    		{
    			CListItem_t* listItem = (CListItem_t*)HLIB_VECTOR_ItemAt(groupList, i);
    			channelList.push_back(listItem);
    		}
            favMgr->createFavouriteList(iter.uid, groupInfo->szGroupName, channelList);
            HLIB_VECTOR_Delete(groupList);
        }
    }
    favMgr->mRestoring = false;
    favMgr->mCollection.sort(compareById);
    favMgr->print("restoreCb");
}

void CFavouriteManager::print(const char* aFunc)
{
	HxLOG_Warning("[FAV] %s: count=%d\n", aFunc, mCollection.size());
	std::list<CFavouriteList*>::const_iterator i(mCollection.begin());
	while(i != mCollection.end()) {
		CFavouriteListDama* item = (CFavouriteListDama*)(*i++);
		item->print(aFunc);
	}
}

void CFavouriteManager:: getModelID(HUINT32 *aulModelId, HUINT32 *aulActionID)
{
	HUINT32	ulModelId = EModelId_None, ulActionID = CLIB_OP_NULL;

	STEP;
#if defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE)
	ulModelId = EModelId_HMS;
	ulActionID = CLIB_OP_NULL;
#elif defined(CONFIG_PROD_HMS1000T)
	ulModelId = EModelId_AUS;
	ulActionID = CLIB_OP_AUSDTT;
#elif defined(CONFIG_OP_MIDDLE_EAST)
	ulModelId = EModelId_ME;
	ulActionID = CLIB_OP_ME;
#elif defined(CONFIG_OP_ERTELECOM)
	ulModelId = EModelId_ERTELECOM;
	ulActionID = CLIB_OP_ERTELECOM;
#elif defined(CONFIG_OP_SES)
	ulModelId = EModelId_SES;
	ulActionID = CLIB_OP_SES;
#elif defined(CONFIG_OP_UK_DTT)
	ulModelId = EModelId_UK_DTT;
	ulActionID = CLIB_OP_UK_DTT;
#elif defined(CONFIG_OP_ZIGGO)
	ulModelId = EModelId_ZIGGO;
	ulActionID = CLIB_OP_ZIGGO;
#elif defined(CONFIG_OP_UK_DTT)
	ulModelId = EModelId_UK_DTT;
	ulActionID = CLIB_OP_UK_DTT;
#elif defined(CONFIG_OP_ITALY_DBOOK)
	ulModelId = EModelId_ITA_DTT;
	ulActionID = CLIB_OP_ITA_DTT;
#elif defined(CONFIG_OP_TELENOR)
	ulModelId = EModelId_TELENOR;
	ulActionID = CLIB_OP_TELENOR;
#elif defined(CONFIG_OP_TDC)
	ulModelId = EModelId_TDC;
	ulActionID = CLIB_OP_TDC;
#endif

	*aulModelId = ulModelId;
	*aulActionID = ulActionID;
}

void CFavouriteManager::getDefaultLCN(HUINT32 *aulTv, HUINT32 *aulRadio)
{
	HUINT32 ulModelId = EModelId_None, ulActionID = CLIB_OP_NULL;

	STEP;
	getModelID(&ulModelId, &ulActionID);
	switch (ulModelId)
	{
		case EModelId_HMS:
			*aulTv = 100;
			*aulRadio = 100;
			break;
		case EModelId_ME:
			*aulTv = 1;
			*aulRadio = 1;
			break;
		case EModelId_AUS:
			*aulTv = 350;
			*aulRadio = 350;
			break;
		case EModelId_ERTELECOM:
			*aulTv = 2000;
			*aulRadio = 2000;
			break;
		case EModelId_SES:
			*aulTv = 100;
			*aulRadio = 100;
			break;
		case EModelId_ZIGGO:
			*aulTv = 1000;
			*aulRadio = 1000;
			break;
		case EModelId_UK_DTT:
			*aulTv = 800;
			*aulRadio = 800;
			break;
		case EModelId_ITA_DTT:
			*aulTv = 850;
			*aulRadio = 850;
			break;
		case EModelId_TELENOR:
			*aulTv = 1;
			*aulRadio = 1;
			break;
		case EModelId_TDC:
			*aulTv = 1;
			*aulRadio = 1;
			break;
		case EModelId_None:
		default:
			*aulTv = 1;
			*aulRadio = 5000;
			break;
	}
}


