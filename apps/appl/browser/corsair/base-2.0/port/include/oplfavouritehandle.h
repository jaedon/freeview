#ifndef OPLFAVOURITEHANDLE_H
#define OPLFAVOURITEHANDLE_H

#include <hlib.h>
#include <list>

typedef void *	OPL_Channel_t;
class CFavouriteList;

enum TFavouriteId
{
	EFavouriteIdNone = 0,
	EFavouriteIdAll,
	EFavouriteIdTv,
	EFavouriteIdRadio,
	EFavouriteIdHdTv,
	EFavouriteIdFta,
	EFavouriteIdCas,
#if defined(CONFIG_OP_MIDDLE_EAST) || defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE)	// Support Alphabet Group List
	EFavouriteIdAG_ALL,
	EFavouriteIdAG_A,
	EFavouriteIdAG_B,
	EFavouriteIdAG_C,
	EFavouriteIdAG_D,
	EFavouriteIdAG_E,
	EFavouriteIdAG_F,
	EFavouriteIdAG_G,
	EFavouriteIdAG_H,
	EFavouriteIdAG_I,
	EFavouriteIdAG_J,
	EFavouriteIdAG_K,
	EFavouriteIdAG_L,
	EFavouriteIdAG_M,
	EFavouriteIdAG_N,
	EFavouriteIdAG_O,
	EFavouriteIdAG_P,
	EFavouriteIdAG_Q,
	EFavouriteIdAG_R,
	EFavouriteIdAG_S,
	EFavouriteIdAG_T,
	EFavouriteIdAG_U,
	EFavouriteIdAG_V,
	EFavouriteIdAG_W,
	EFavouriteIdAG_X,
	EFavouriteIdAG_Y,
	EFavouriteIdAG_Z,
	EFavouriteIdAG_Digit,
	EFavouriteIdAG_Etc,
#endif
	EFavouriteIdUserStart= 100
};

enum TModelId
{
	EModelId_None = 0,
	EModelId_HMS,
	EModelId_ME,
	EModelId_AUS,
	EModelId_ERTELECOM,
	EModelId_SES,
	EModelId_UK_DTT,
	EModelId_ZIGGO,
	EModelId_ITA_DTT,
	EModelId_TELENOR,
	EModelId_TDC
};

class OplFavouriteListHandle
{
private:
    OplFavouriteListHandle(CFavouriteList* p);
public:
	virtual ~OplFavouriteListHandle();
	const char* getFavID();
	const char* getName();
    	void setName(const char* aName);
	std::list<OPL_Channel_t> createChannelList();
	bool save(const std::list<const char*>& aCcids);
	bool save(const std::list<const char*>& aCcids, const std::list<int>& aLcnList, int svcType);
private:
    CFavouriteList* p;
	char m_fav_id[32];
	friend class CFavouriteManager;
};

class OplFavouriteManager
{
public:
    static std::list<OplFavouriteListHandle*> createHandles();
    static bool removeFavouriteList(OplFavouriteListHandle* aFavouriteListHandle);
    static OplFavouriteListHandle* getFavouriteList(int aFavID);
    static void refreshFavouriteList();
    static void reloadFavouriteList();
    static OplFavouriteListHandle* createFavouriteList(const char* aName);
    static int getLCN(const int aUid);
};

#endif // OPLFAVOURITEHANDLE_H
