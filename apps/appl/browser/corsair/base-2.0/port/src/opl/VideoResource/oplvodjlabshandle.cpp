
#include "oplvodjlabshandle.h"
#include "cvideoresource.h"
#include "oplvod_jlabs.h"

OplVodJlabsHandle* OplVodJlabsHandle::create(const OplVideoHandle::THandleOwnerType& aHandleOwnerType)
{
	OplVodJlabsHandle* ret = new OplVodJlabsHandle(aHandleOwnerType);
	OplVideoHandle::regist(ret);
	return ret;
}

OplVodJlabsHandle::OplVodJlabsHandle(
		const OplVideoHandle::THandleOwnerType& aHandleOwnerType)
	: OplVodHandle(aHandleOwnerType)
{
}

OplVodJlabsHandle::~OplVodJlabsHandle()
{
}

OplVodJlabs& OplVodJlabsHandle::getVodJLabs()
{
	return reinterpret_cast<OplVodJlabs&>(getVodObject());
}

OplVod* OplVodJlabsHandle::createVodObject(CVideoResource* aVideoResource)
{
	return new OplVodJlabs(aVideoResource->getVideoResourceId());
}

bool OplVodJlabsHandle::getProgramme(OplVodProgramme_t *out_prog)
{
	bool ret(false);
	if (hasVodObject())
	{
		ret = getVodJLabs().getProgramme(out_prog);
	}
	return ret;
}

bool OplVodJlabsHandle::getBmlStartFlag()
{
	bool ret(false);
	if (hasVodObject())
	{
		ret = getVodJLabs().getBmlStartFlag();
	}
	return ret;
}

bool OplVodJlabsHandle::getBmlEndParam(int *out_cause, char *out_param)
{
	bool ret(false);
	if (hasVodObject())
	{
		ret = getVodJLabs().getBmlEndParam(out_cause, out_param);
	}
	return ret;
}

bool OplVodJlabsHandle::showVideoMessage(int type)
{
	bool ret(false);
	if (hasVodObject())
	{
		ret = getVodJLabs().showVideoMessage(type);
	}
	return ret;
}

bool OplVodJlabsHandle::hideVideoMessage(int type)
{
	bool ret(false);
	if (hasVodObject())
	{
		ret = getVodJLabs().hideVideoMessage(type);
	}
	return ret;
}

bool OplVodJlabsHandle::startBML()
{
	bool ret(false);
	if (hasVodObject())
	{
		ret = getVodJLabs().startBML();
	}
	return ret;
}

bool OplVodJlabsHandle::endBML()
{
	bool ret(false);
	if (hasVodObject())
	{
		ret = getVodJLabs().endBML();
	}
	return ret;
}

bool OplVodJlabsHandle::setBMLKeyControlMode(bool permit, int *ret_val)
{
	bool ret(false);
	if (hasVodObject())
	{
		ret = getVodJLabs().setBMLKeyControlMode(permit, ret_val);
	}
	return ret;
}

unsigned int OplVodJlabsHandle::getComponentsCount(eOplAVComponentType comp_type)
{
	unsigned int ret = false;
	if (hasVodObject())
	{
		ret = getVodJLabs().getComponentsCount(comp_type);
	}
	return ret;
}

bool OplVodJlabsHandle::getComponent(eOplAVComponentType comp_type, int index, OplAVComponent_t *out_comp)
{
	bool ret(false);
	if (hasVodObject())
	{
		ret = getVodJLabs().getComponent(comp_type, index, out_comp);
	}
	return ret;
}

bool OplVodJlabsHandle::getActiveComponent(eOplAVComponentType comp_type, OplAVComponent_t *out_comp)
{
	bool ret(false);
	if (hasVodObject())
	{
		ret = getVodJLabs().getActiveComponent(comp_type, out_comp);
	}
	return ret;
}

bool OplVodJlabsHandle::setActiveComponent(eOplAVComponentType comp_type, int pid)
{
	bool ret(false);
	if (hasVodObject())
	{
		ret = getVodJLabs().setActiveComponent(comp_type, pid);
	}
	return ret;
}

/* Video Component */
unsigned int OplVodJlabsHandle::getVideoComponentCount()
{
	unsigned int ret = false;
	if (hasVodObject())
	{
		ret = getVodJLabs().getVideoComponentCount();
	}
	return ret;
}

bool OplVodJlabsHandle::getVideoComponent(int index, OplAVComponent_t *out_comp)
{
	bool ret(false);
	if (hasVodObject())
	{
		ret = getVodJLabs().getVideoComponent(index, out_comp);
	}
	return ret;
}

bool OplVodJlabsHandle::getActiveVideoComponent(OplAVComponent_t *out_comp)
{
	bool ret(false);
	if (hasVodObject())
	{
		ret = getVodJLabs().getActiveVideoComponent(out_comp);
	}
	return ret;
}

bool OplVodJlabsHandle::setActiveVideoComponent(int pid)
{
	bool ret(false);
	if (hasVodObject())
	{
		ret = getVodJLabs().setActiveVideoComponent(pid);
	}
	return ret;
}

/* Audio Component */
unsigned int OplVodJlabsHandle::getAudioComponentCount()
{
	unsigned int ret = false;
	if (hasVodObject())
	{
		ret = getVodJLabs().getAudioComponentCount();
	}
	return ret;
}

bool OplVodJlabsHandle::getAudioComponent(int index, OplAVComponent_t *out_comp)
{
	bool ret(false);
	if (hasVodObject())
	{
		ret = getVodJLabs().getAudioComponent(index, out_comp);
	}
	return ret;
}

bool OplVodJlabsHandle::getActiveAudioComponent(OplAVComponent_t *out_comp)
{
	bool ret(false);
	if (hasVodObject())
	{
		ret = getVodJLabs().getActiveAudioComponent(out_comp);
	}
	return ret;
}

bool OplVodJlabsHandle::setActiveAudioComponent(int pid)
{
	bool ret(false);
	if (hasVodObject())
	{
		ret = getVodJLabs().setActiveAudioComponent(pid);
	}
	return ret;
}

/* Subtitle Component */
unsigned int OplVodJlabsHandle::getSubtitleComponentCount()
{
	unsigned int ret(0);
	if (hasVodObject())
	{
		ret = getVodJLabs().getSubtitleComponentCount();
	}
	return ret;
}

bool OplVodJlabsHandle::getSubtitleComponent(int index, OplAVComponent_t *out_comp)
{
	bool ret(false);
	if (hasVodObject())
	{
		ret = getVodJLabs().getSubtitleComponent(index, out_comp);
	}
	return ret;
}

bool OplVodJlabsHandle::getActiveSubtitleComponent(OplAVComponent_t *out_comp)
{
	bool ret(false);
	if (hasVodObject())
	{
		ret = getVodJLabs().getActiveSubtitleComponent(out_comp);
	}
	return ret;
}

bool OplVodJlabsHandle::setActiveSubtitleComponent(int pid)
{
	bool ret(false);
	if (hasVodObject())
	{
		ret = getVodJLabs().setActiveSubtitleComponent(pid);
	}
	return ret;
}

/* Superimpose Component */
unsigned int OplVodJlabsHandle::getSuperimposeComponentCount()
{
	unsigned int ret(0);
	if (hasVodObject())
	{
		ret = getVodJLabs().getSuperimposeComponentCount();
	}
	return ret;
}

bool OplVodJlabsHandle::getSuperimposeComponent(int index, OplAVComponent_t *out_comp)
{
	bool ret(false);
	if (hasVodObject())
	{
		ret = getVodJLabs().getSuperimposeComponent(index, out_comp);
	}
	return ret;
}

/* Data Component */
unsigned int OplVodJlabsHandle::getDataComponentCount()
{
	unsigned int ret(0);
	if (hasVodObject())
	{
		ret = getVodJLabs().getDataComponentCount();
	}
	return ret;
}

bool OplVodJlabsHandle::getDataComponent(int index, OplAVComponent_t *out_comp)
{
	bool ret(false);
	if (hasVodObject())
	{
		ret = getVodJLabs().getDataComponent(index, out_comp);
	}
	return ret;
}
