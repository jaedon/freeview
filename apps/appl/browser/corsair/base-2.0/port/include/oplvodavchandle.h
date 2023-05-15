#ifndef __OPLVODAVCHANDLE_H__
#define __OPLVODAVCHANDLE_H__

#include <oplvodhandle.h>

class OplVodAvcHandle: public OplVodHandle {
public:
	static OplVodAvcHandle* create(const OplVideoHandle::THandleOwnerType& aHandleOwnerType);
private:
	OplVodAvcHandle(const OplVideoHandle::THandleOwnerType& aHandleOwnerType);
public:
	~OplVodAvcHandle();

	bool queue(const char *url);
	bool setLoop(int count);
	bool setCache(bool cache);
	bool getCache(void);
	bool cachePlay(void);
		
private: // from OplVod
	virtual OplVod* createVodObject(CVideoResource* aVideoResource);
};

#endif // __OPLVODAVCHANDLE_H__
