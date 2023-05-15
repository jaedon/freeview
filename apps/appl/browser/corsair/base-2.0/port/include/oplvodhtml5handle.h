
#ifndef __OPLVODHTML5HANDLE_H__
#define __OPLVODHTML5HANDLE_H__

#include "oplvodhandle.h"

//#include "oplvod_html5.h"


class OplVodHtml5Handle: public OplVodHandle {
public:
	static OplVodHtml5Handle* create(const OplVideoHandle::THandleOwnerType& aHandleOwnerType);
private:
	OplVodHtml5Handle(const OplVideoHandle::THandleOwnerType& aHandleOwnerType);
public:
	~OplVodHtml5Handle();

    virtual bool probe();
	virtual bool play();
#if defined(CONFIG_TIMESTAMP_64)	
	virtual bool seek(unsigned long long position);
#else
	virtual bool seek(unsigned int position);
#endif


	virtual unsigned int getComponentsCount(eOplAVComponentType comp_type);
	virtual bool getComponent(eOplAVComponentType comp_type, int index, OplAVComponent_t *out_comp);
	virtual int getActiveComponentIndex(eOplAVComponentType comp_type);

	void setPlayerType(eOplVodPlayerType type);
	void setDuration(unsigned int duration);
	int getPumpIndex();
	bool setProbeInfo(OplVodProbeInfo *probe_info);		

	double getStartDate(void);
	unsigned int getSeekableInfo(unsigned long long **seekable_start, unsigned long long **seekable_end);

	// DRMKEY_SYNC work around
	/////////////////////////////////////////////////
	static bool m_need_key;
	
private: // from OplVod
	virtual OplVod* createVodObject(CVideoResource* aVideoResource);
	OplVodHtml5Handle& getVodHtml5();

private:
	eOplVodPlayerType m_player_type;
	unsigned int m_duration;	
	OplVodProbeInfo m_probe_info;
};

#endif // __OPLVODHTML5HANDLE_H__
