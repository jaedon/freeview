
#include "oplvodavchandle.h"
#include "cvideoresource.h"
#include "oplvod_avc.h"

OplVodAvcHandle* OplVodAvcHandle::create(const OplVideoHandle::THandleOwnerType& aHandleOwnerType)
{
	OplVodAvcHandle* ret = new OplVodAvcHandle(aHandleOwnerType);
	OplVideoHandle::regist(ret);
	return ret;
}

OplVodAvcHandle::OplVodAvcHandle(
		const OplVideoHandle::THandleOwnerType& aHandleOwnerType)
	: OplVodHandle(aHandleOwnerType)
{

}

OplVodAvcHandle::~OplVodAvcHandle()
{

}

OplVod* OplVodAvcHandle::createVodObject(CVideoResource* aVideoResource)
{
	return new OplVodAVControl(aVideoResource->getVideoResourceId());
}

bool OplVodAvcHandle::queue(const char *url)
{
	bool ret = false;

	OplVodAVControl *player = dynamic_cast<OplVodAVControl *>(mOplVod);
	
	if (player)
	{
		ret = player->queue(url);
	}
	return ret;
}

bool OplVodAvcHandle::setLoop(int count)
{
	bool ret = false;

	OplVodAVControl *player = dynamic_cast<OplVodAVControl *>(mOplVod);
	
	if (player)
	{
		ret = player->setLoop(count);
	}
	return ret;
}

bool OplVodAvcHandle::setCache(bool cache)
{
	bool ret = false;

	OplVodAVControl *player = dynamic_cast<OplVodAVControl *>(mOplVod);
	
	if (player)
	{
		ret = player->setCache(cache);
	}

	return ret;
}

bool OplVodAvcHandle::getCache()
{
	bool ret = false;

	OplVodAVControl *player = dynamic_cast<OplVodAVControl *>(mOplVod);
	
	if (player)
	{
		ret = player->getCache();
	}

	return ret;
}

bool OplVodAvcHandle::cachePlay()
{
	bool ret = false;

	OplVodAVControl *player = dynamic_cast<OplVodAVControl *>(mOplVod);
	
	if (player)
	{
		ret = player->cachePlay();
	}

	return ret;
}

