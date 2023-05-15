#ifndef CFAVOURITEMANAGER_H
#define CFAVOURITEMANAGER_H

#include "oplfavouritehandle.h"

#include <list>
#include <map>
#include <hlib.h>
#include <clib.h>
#include "oplchannel.h"

typedef int TChannelId;

/************************************************************************
 * class OplFavouriteListPrivate
 ************************************************************************/

class CFavouriteList
{
public:
	CFavouriteList(bool aNew);
	virtual ~CFavouriteList();
public:
    virtual HUID getFavID() = 0;
    virtual const char* getName() = 0;
    virtual void setName(const char* aName) = 0;
    virtual std::list<OPL_Channel_t> createChannelList() = 0;
    virtual bool save(const std::list<const char*>& aCcids) = 0;
    virtual bool save(const std::list<const char*>& aCcids, const std::list<int>& aLcnList, int svcType) = 0;
public:
	int refCount();
	void registerHandle(OplFavouriteListHandle* aHandle);
	void unRegisterHandle(OplFavouriteListHandle* aHandle);
	bool isObsolete();
	void setObsolete();
	bool isUserFavourite();
	bool isNew();
	void clearNew();
private:
	bool mObsolete; // means this will be destroyed when reference cound reaches zero
	bool mNew; // means this instance created by user request, eg, user favourite
	std::list<OplFavouriteListHandle*> mHandles; // for reference handling
};

/************************************************************************
 * class OplFavouriteListPrivateDama
 ************************************************************************/

class CFavouriteListDama: public CFavouriteList
{
public:
	// contstuctor for newly created favourite list.
	CFavouriteListDama(HUID aRecordId, const char* aName);
	// constructor for existing favourite list.
	CFavouriteListDama(HUID aRecordId, const char* aName, const std::list<const CListItem_t*>& aUids);
	virtual ~CFavouriteListDama();

private: // from OplFavouriteListPrivate
    HUID getFavID();
    const char* getName();
    void setName(const char* aName);
    std::list<OPL_Channel_t> createChannelList();
    bool save(const std::list<const char*>& aCcids);
    bool save(const std::list<const char*>& aCcids,  const std::list<int>& aLcnList, int svcType);
	void print(const char* aFunc);
	void remove();
	void update();

public:
	HUID mRecordId;
	char* mName;
	bool mNameChanged; // indicate name has been changed by user
	std::list<TChannelId> mChannelInfos; // contains channel uid list
	friend class CFavouriteManager;
};

/************************************************************************
 * class OplFavouriteManagerPrivate
 ************************************************************************/
class CFavouriteManager
{
public: // for API
	static CFavouriteManager* getInstance();
	std::list<OplFavouriteListHandle*> createHandles();
	OplFavouriteListHandle* createUserFavouriteList(const char* aName);
	bool removeFavouriteList(OplFavouriteListHandle* aFavouriteListHandle);
    OplFavouriteListHandle* getFavouriteList(int aFavId);
private:
	CFavouriteManager();
	~CFavouriteManager();
public: // for internal use
	int getLCN(const int aUid);
	HUID allocUserFavId();
	CFavouriteList* getFavouriteListById(int aFavId);
	void createFavouriteList(HUID aRecordId, const char* aName, const std::list<const CListItem_t*>& aUids);
	void destroyFavouriteList(CFavouriteList* aFavouriteList);
	void updateFavouriteList(HUID aRecordId, const char* aName, const std::list<TChannelId>& aUids);
	void handleChannelRemove(HUID aRecordId, const char* aName, const std::list<TChannelId>& aUids);
	void getUserFavouriteList();
	bool reset();
	void buildWithSystemChannels();
	bool buildWithFavourite();
	bool buildWithFavourite(const std::list<int>& aLcnList, int svcType);
	HUINT32 restore(HUID aRecordId);
	bool restoreSync(HUID aRecordId = 0);
    static void restoreCb(const HCHAR* aName, HUID aUid, const void* aData, HUINT32 aSize, void* aSelf);
	void getModelID(HUINT32 *aulModelId, HUINT32 *aulActionID);
	void getDefaultLCN(HUINT32 *aulTv, HUINT32 *aulRadio);
	void print(const char* aFunc);
private:
	bool build(HxVector_t* pstSvcList);
private:
	std::list<CFavouriteList*> mCollection;
	std::map<TChannelId, CListItem_t> mAllList;
	OPL_ChannelManager_t mChannelMgr;
	bool mRestoring;
};

#endif // CFAVOURITEMANAGER_H
