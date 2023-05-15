
#include "oplvodhtml5handle.h"
#include "cvideoresource.h"
#include "oplvod_html5.h"


// DRMKEY_SYNC work around
/////////////////////////////////////////////////
bool OplVodHtml5Handle::m_need_key = false;

OplVodHtml5Handle* OplVodHtml5Handle::create(const OplVideoHandle::THandleOwnerType& aHandleOwnerType)
{
	OplVodHtml5Handle* ret = new OplVodHtml5Handle(aHandleOwnerType);
	OplVideoHandle::regist(ret);
	return ret;
}

OplVodHtml5Handle::OplVodHtml5Handle(
		const OplVideoHandle::THandleOwnerType& aHandleOwnerType)
	: OplVodHandle(aHandleOwnerType)
{
	memset(&m_probe_info, 0x0, sizeof(m_probe_info));
	m_duration = 0;
}

OplVodHtml5Handle::~OplVodHtml5Handle()
{

}

OplVodHtml5Handle& OplVodHtml5Handle::getVodHtml5()
{
	return reinterpret_cast<OplVodHtml5Handle&>(getVodObject());
}

bool OplVodHtml5Handle::probe()
{
	bool ret(false);
	videoResourceRequired("probe");
	if (mOplVod)
	{
		applyCachedValue();
		mOplVod->setPlayerType(m_player_type);
		ret = mOplVod->probe();
	}
	return ret;
}

bool OplVodHtml5Handle::play()
{
	bool ret = false;
	videoResourceRequired("play");
	HxLOG_Debug("[VRM] VOD[%d].play\n", getIndex());
	if ( mOplVod )
	{
		HxLOG_Debug("[VRM] VOD[%d].play applyCachedValue\n", getIndex());
		applyCachedValue();
		if ( m_player_type == OPLVOD_PLAYER_TYPE_MSE )
		{
			mOplVod->setPlayerType(m_player_type);
		}

		ret = mOplVod->play(&m_probe_info);

		if ( m_duration != 0 )
		{
			OplVodHtmlFive *player = dynamic_cast<OplVodHtmlFive *>(mOplVod);
			if ( player ) player->setDuration(m_duration);		
		}
		
		HxLOG_Debug("[VRM] VOD[%d].play = %d\n", getIndex(), ret);
	}
	return ret;
}

#if defined(CONFIG_TIMESTAMP_64)
bool OplVodHtml5Handle::seek(unsigned long long position)
{
	bool ret = false;
	videoResourceRequired("seek");
	if (mOplVod)
	{
		applyCachedValue();
		ret = mOplVod->seek(position);
	}
	return ret;
}
#else
bool OplVodHtml5Handle::seek(unsigned int position)
{
	bool ret = false;
	videoResourceRequired("seek");
	if (mOplVod)
	{
		applyCachedValue();
		ret = mOplVod->seek(position);
	}
	return ret;
}
#endif

void OplVodHtml5Handle::setPlayerType(eOplVodPlayerType type)
{
	m_player_type = type;
}

void OplVodHtml5Handle::setDuration(unsigned int duration)
{
	if ( mOplVod )
	{
		OplVodHtmlFive *player = dynamic_cast<OplVodHtmlFive *>(mOplVod);
		if ( !player ) return;

		if ( hasVodObject() )
			player->setDuration(duration);
		else
			m_duration = duration;	
	}
	else
	{
		m_duration = duration;
	}
}

int OplVodHtml5Handle::getPumpIndex()
{
	int ret = 0;

	if ( mOplVod )
	{
		OplVodHtmlFive *player = dynamic_cast<OplVodHtmlFive *>(mOplVod);
		if ( !player ) return 0;

		if ( hasVodObject() )
		{
			ret = player->getPumpIndex();
		}
	}

	return ret;
}

bool OplVodHtml5Handle::setProbeInfo(OplVodProbeInfo *probe_info)
{
	bool ret = false;

	m_probe_info.audio_pid = probe_info->audio_pid;
	m_probe_info.video_pid = probe_info->video_pid;
	m_probe_info.audio_codec = probe_info->audio_codec;
	m_probe_info.video_codec = probe_info->video_codec;
	
	return ret;	
}

unsigned int OplVodHtml5Handle::getComponentsCount(eOplAVComponentType comp_type)
{
	unsigned int ret(0);
	if (mOplVod)
	{
		OplVodHtmlFive *player = dynamic_cast<OplVodHtmlFive *>(mOplVod);
		if( player )
			ret = player->getComponentsCount(comp_type);
	}
	return ret;
}

bool OplVodHtml5Handle::getComponent(eOplAVComponentType comp_type, int index, OplAVComponent_t *out_comp)
{
	bool ret = false;
	if (mOplVod)
	{
		OplVodHtmlFive *player = dynamic_cast<OplVodHtmlFive *>(mOplVod);
		if( player )
			ret = player->getComponent(comp_type, index, out_comp);
	}
	return ret;
}

int OplVodHtml5Handle::getActiveComponentIndex(eOplAVComponentType comp_type)
{
	int index = -1;
	if (mOplVod)
	{
		OplVodHtmlFive *player = dynamic_cast<OplVodHtmlFive *>(mOplVod);
		if( player ) 
			index = player->getActiveComponentIndex(comp_type);
	}
	return index;
}

double OplVodHtml5Handle::getStartDate(void)
{
//	bool ret = false;
	double start_date = 0;
	if (mOplVod)
	{
		OplVodHtmlFive *player = dynamic_cast<OplVodHtmlFive *>(mOplVod);
		if( player )
			start_date = player->getStartDate();
	}

	return start_date;
}

unsigned int OplVodHtml5Handle::getSeekableInfo(unsigned long long **seekable_start, unsigned long long **seekable_end)
{
	unsigned int ret(0);
	if (mOplVod)
	{
		OplVodHtmlFive *player = dynamic_cast<OplVodHtmlFive *>(mOplVod);
		if( player )
			ret = player->getSeekableInfo(seekable_start, seekable_end);
	}
	return ret;
}


OplVod* OplVodHtml5Handle::createVodObject(CVideoResource* aVideoResource)
{
	return new OplVodHtmlFive(aVideoResource->getVideoResourceId());
}
