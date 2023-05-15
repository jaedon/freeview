
#ifndef __OPLVIDEOHANDLE_H__
#define __OPLVIDEOHANDLE_H__

class CVideoResourceManager;
class CVideoResource;
class OplVideoHandle {
public:
	class IAudioPathChangeListener
	{
	public:
		virtual void audioPathChanged(bool acquired) = 0;
	};
public:
	enum THandleOwnerType {
		EHandleOwnerTypeNone = 0x00,
		EHandleOwnerTypeSystem = 0x01,
		EHandleOwnerTypeUserOipf = 0x02,
		EHandleOwnerTypeUserHbbtv = 0x04,
		EHandleOwnerTypeAll = 0x07,
	};
	enum TVideoHandleType {
		EVideoHandleTypeNone = 0x00,
		EVideoHandleTypeBroadcast = 0x01,
		EVideoHandleTypeVod = 0x02,
		EVideoHandleTypeAll = 0x03,
	};
	enum TAudioPathState {
		EAudioPathStateNone,
		EAudioPathStateReleased,
		EAudioPathStateAcquired,
	};
	enum TResourceNecessity {
		EResourceNecessityUnknown,
		EResourceNecessityNotRequired,
		EResourceNecessityRequired,
	};
	class TWindowRect
	{
	public:
		TWindowRect()
			: mX(-1), mY(-1), mWidth(-1), mHeight(-1)
		{

		}

		bool isValid()
		{
			return mX != -1 || mY != -1 || mWidth != -1 || mHeight != -1;
		}

		int mX;
		int mY;
		int mWidth;
		int mHeight;
	};

protected:
	OplVideoHandle(
		const THandleOwnerType& aHandleOwnerType,
		const TVideoHandleType& aVideoHandleType);
public:
	//static void init(OplVideoHandle* aVideoHandle);
	static void regist(OplVideoHandle* aVideoHandle);
	static void unRegist(OplVideoHandle* aVideoHandle);
	virtual ~OplVideoHandle();
	TResourceNecessity getResourceNecessity();
	void setAudioPathChangeListener(IAudioPathChangeListener* aListener);
	void requestAudioPath();
	bool hasAudioPath();
	virtual bool setVisibility(bool visible);
	bool isVisible();
	int getIndex();
private: // Used by CVideoResouceManager;
	THandleOwnerType getHandleOwnerType();
	TVideoHandleType getVideoHandleType();
	TAudioPathState getAudioPathState();
	void changeAudioPathState(const TAudioPathState& aAudioPathState);
	void videoResouceAllocated(CVideoResource* aVideoResource);
	void videoResouceDeAllocated(CVideoResource* aVideoResource, const TVideoHandleType& aVideoHandleType);
	CVideoResource* getAllocatedResource();
private: // Internal use
	void updateAudioPathState();
	void applyVisibility(bool showAlways=false);
protected:
	virtual void init();
	virtual void deInit();
	bool isResourceAllocated();
	virtual void processVideoResourceAllocated(CVideoResource* aVideoResource) = 0;
	virtual void processVideoResourceDeAllocated(CVideoResource* aVideoResource, const TVideoHandleType& aVideoHandleType) = 0;
	virtual void videoResourceRequired(const char* aReason = 0);
	virtual void videoResourceNotRequired(const char* aReason = 0);
protected:
	THandleOwnerType mHandleOwnerType;
	CVideoResource* mAllocatedResource;
private:
	TVideoHandleType mVideoHandleType;
	TResourceNecessity mResourceNecessity;
	bool mVisibility;
	CVideoResourceManager& mVideoResouceManager;
	IAudioPathChangeListener* mAudioPathChangeListener;
	friend class CVideoResourceManager;
	friend class CVideoResource;
};

#endif // __OPLVIDEOHANDLE_H__
