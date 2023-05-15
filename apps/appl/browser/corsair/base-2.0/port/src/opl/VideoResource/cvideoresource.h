#ifndef __CVIDEORESOURCE_H__
#define __CVIDEORESOURCE_H__
#include <set>
#include <oplvod.h>
#include <oplvideobroadcasthandle.h>

class OplVideoHandle;
class OplVideoBroadcast;
class CVideoResourceManager;
class CVideoResource
{
private: // Used by CVideoResouceManager
	static CVideoResource* create(const int& aVideoResourceId);
	CVideoResource(const int& aVideoResourceId);
	virtual ~CVideoResource();
	void initialize(const int& aVideoResourceId);
	bool setMain(bool aMain);
	bool isAllocated();
	OplVideoHandle* getVideoHandle();
	void setVideoHandle(OplVideoHandle* aVideoHandle);
	OplVideoHandle* releaseVideoHandle();
	bool isAllocatedHandleIdentical(OplVideoHandle* aVideoHandle);
	bool releaseVideoHandleIfIdentical(OplVideoHandle* aVideoHandle);
	void setUsing(bool aUsing);
public:
	bool isMain();
	int getVideoResourceId();
	bool isBeingUsed();
	OplVideoBroadcast* getVideoBroadcastResource();
	void addVideoBroadcastListener(IVideoBroadcastListener* aListener);
	void removeVideoBroadcastListener(IVideoBroadcastListener* aListener);
	static void CbLive(void *aSelf, eOplBroadcastEventType aEventType);
	static void CbTsr(void *aSelf, eOplBroadcastEventType aEventType);
	static void CbCas(void *aSelf, eOplBroadcastEventType aEventType);
	static void CbRec(void *aSelf, eOplBroadcastEventType aEventType);
	static void CbMail(void *aSelf, eOplBroadcastEventType aEventType);
	static void CbBml(void *aSelf, eOplBroadcastEventType aEventType);
	static void CbMheg(void *aSelf, eOplBroadcastEventType aEventType);
private:
	void deliverVideoBroadcastEvent(const IVideoBroadcastListener::TEventType& aEventType, const eOplBroadcastEventType& aOplEventType);
private:
	int mVideoResourceId;
	OplVideoHandle* mVideoHandle;
	bool mMain;
	bool mResourceUsing;
	OplVideoBroadcast* mVideoBroadcast;
	std::set<IVideoBroadcastListener*> mVideoBroadcastListeners;
	friend class CVideoResourceManager;
};

#endif // __CVIDEORESOURCE_H__
