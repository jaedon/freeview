#ifndef __VIDEORESOUCEMANAGER_H__
#define __VIDEORESOUCEMANAGER_H__

#include <list>
#include "oplvideohandle.h"

class OplVideoHandle;
class OplVideoBroadcastHandle;
class OplVodHandle;
class CVideoResource;
class CVideoResourceManager
{
public:
	enum TEventType
	{
		EEventTypeNone,
		EEventTypeOnVideoHandleCreate,
		EEventTypeOnVideoHandleDestroyed,
		EEventTypeOnVideoHandleShown,
		EEventTypeOnVideoHandleHidden,
		EEventTypeOnVideoResourceRequired,
		EEventTypeOnVideoResourceNotRequired
	};
	enum TVisibilityCondition
	{
		EVisibilityConditionDontCare,
		EVisibilityConditionNotVisible,
		EVisibilityConditionVisible,
	};
	enum TVideoResourceAllocType
	{
		EVideoResourceAllocTypeNone,
		EVideoResourceAllocTypeMain,
		EVideoResourceAllocTypeMainIfAvailable,
		EVideoResourceAllocTypeSub
	};
	enum TVideoResourceDeAllocType
	{
		EVideoResourceDeAllocTypeNone,
		EVideoResourceDeAllocTypeDeAllocSelf,
		EVideoResourceDeAllocTypeRecoverPrevBroadcast,
		EVideoResourceDeAllocTypeRecoverPrevCreated
	};
	class TPolicy
	{
	public:
		TPolicy(
			const TEventType& aEventType,
			const int& aHandleOwnerType,
			const OplVideoHandle::TVideoHandleType& aVideoHandleType)
			: mEventType(aEventType),
			  mVideoHandleOwnerType(aHandleOwnerType),
			  mVideoHandleType(aVideoHandleType),
			  mVisibilityCondition(EVisibilityConditionDontCare),
			  mResourceNecessity(OplVideoHandle::EResourceNecessityUnknown)
		{
		}
		TPolicy(
			const TEventType& aEventType,
			const int& aHandleOwnerType,
			const OplVideoHandle::TVideoHandleType& aVideoHandleType,
			const TVisibilityCondition& aVisibilityCondition,
			const OplVideoHandle::TResourceNecessity& aResourceNecessity)
			: mEventType(aEventType),
			  mVideoHandleOwnerType(aHandleOwnerType),
			  mVideoHandleType(aVideoHandleType),
			  mVisibilityCondition(aVisibilityCondition),
			  mResourceNecessity(aResourceNecessity)
		{
		}
		TEventType mEventType;
		int mVideoHandleOwnerType;
		OplVideoHandle::TVideoHandleType mVideoHandleType;
		TVisibilityCondition mVisibilityCondition;
		OplVideoHandle::TResourceNecessity mResourceNecessity;
	};
	class TAllocPolicy: public TPolicy
	{
	public:
		TAllocPolicy(
			const TEventType& aEventType,
			const int& aHandleOwnerType,
			const OplVideoHandle::TVideoHandleType& aVideoHandleType,
			const TVideoResourceAllocType& aAllocType)
			: TPolicy(aEventType, aHandleOwnerType, aVideoHandleType),
			  mAllocType(aAllocType)
		{
		}
		TVideoResourceAllocType mAllocType;
	};
	class TDeAllocPolicy: public TPolicy
	{
	public:
		TDeAllocPolicy(
			const TEventType& aEventType,
			const int& aHandleOwnerType,
			const OplVideoHandle::TVideoHandleType& aVideoHandleType,
			const TVisibilityCondition& aVisibilityCondition,
			const OplVideoHandle::TResourceNecessity& aResourceNecessity,
			const TVideoResourceDeAllocType& aDeAllocType)
			: TPolicy(aEventType, aHandleOwnerType, aVideoHandleType, aVisibilityCondition, aResourceNecessity),
			  mDeAllocType(aDeAllocType)
		{
		}
		TVideoResourceDeAllocType mDeAllocType;
	};
private:
	CVideoResourceManager();
public:
	static CVideoResourceManager* getInstance();
	static bool startLiveTV(void);
	static bool stopLiveTV(void);
	static bool recoverLiveTVIfPossible(void);
	static bool isPlayingMedia(void);
	
	virtual ~CVideoResourceManager();
	void registerVideoHandle(OplVideoHandle* aVideoHandle);
	void unRegisterVideoHandle(OplVideoHandle* aVideoHandle);
	bool isRegistered(OplVideoHandle* aVideoHandle);
	void videoHandleShown(OplVideoHandle* aVideoHandle);
	void videoHandleHidden(OplVideoHandle* aVideoHandle);
	void videoResourceRequired(OplVideoHandle* aVideoHandle, const char* aReason);
	void videoResourceNotRequired(OplVideoHandle* aVideoHandle, const char* aReason);
	void requestAudioPath(OplVideoHandle* aVideoHandle);
	OplVideoHandle* getMainVideoHandle();
	OplVideoBroadcastHandle* getMainVideoBroadcastHandle();
	OplVodHandle* getMainVodHandle();
	int findIndex(OplVideoHandle* aVideoHandle);
	bool controllLiveTV(bool on_off);
	OplVideoBroadcastHandle* getSystemVideoBroadcastHandle();
		
private:
	void setMainView(CVideoResource* aVideoResouce);
	void updateMainView(int aResouceId = 0);
	void processAllocPolicy(const TEventType& aEventType, OplVideoHandle* aVideoHandle);
	void allocate(CVideoResource* aVideoResource, OplVideoHandle* aVideoHandle);
	CVideoResource* getMain();
	CVideoResource* getMainIfAvailable();
	CVideoResource* getSub();
	void processDeAllocPolicy(const TEventType& aEventType, OplVideoHandle* aVideoHandle);
	CVideoResource* deAllocate(OplVideoHandle* aVideoHandle);
	CVideoResource* findAllocatedResourceByHandle(OplVideoHandle* aVideoHandle);
	void logVideoHandle(OplVideoHandle* aVideoHandle);
	bool hasHandle(OplVideoHandle* aVideoHandle,
		   	const OplVideoHandle::TVideoHandleType, const OplVideoHandle::THandleOwnerType);
    bool _recoverLiveTVIfPossible(void);
    bool _isPlayingMedia(void);
    
private:
	std::list<CVideoResource*> mVideoResouces;
	std::list<OplVideoHandle*> mVideoHandleList;
	std::list<OplVideoHandle*> mAllocHistoryMain;
	std::list<OplVideoHandle*> mAllocHistoryPiP;
	OplVideoBroadcastHandle* mMainVBHandle;
	bool m_live_recovery_enabled;
};

#endif // __VIDEORESOUCEMANAGER_H__
