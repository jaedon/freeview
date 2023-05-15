
#ifndef __OPLVODJABSHANDLE_H__
#define __OPLVODJABSHANDLE_H__

#include <oplvodhandle.h>
#include "oplvodjlabshandle_def.h"

class OplVodJlabs;
class OplVodJlabsHandle: public OplVodHandle {
public:
	static OplVodJlabsHandle* create(const OplVideoHandle::THandleOwnerType& aHandleOwnerType);
private:
    OplVodJlabsHandle(const OplVideoHandle::THandleOwnerType& aHandleOwnerType);
public:
    ~OplVodJlabsHandle();

    bool getProgramme(OplVodProgramme_t *out_prog);
    bool getBmlStartFlag();
    bool getBmlEndParam(int *out_cause, char *out_param);
    bool showVideoMessage(int type);
    bool hideVideoMessage(int type);
    bool startBML();
    bool endBML();
    bool setBMLKeyControlMode(bool permit, int *ret_val);

    unsigned int getComponentsCount(eOplAVComponentType comp_type);
    bool getComponent(eOplAVComponentType comp_type, int index, OplAVComponent_t *out_comp);
    bool getActiveComponent(eOplAVComponentType comp_type, OplAVComponent_t *out_comp);
    bool setActiveComponent(eOplAVComponentType comp_type, int pid);

private: // from OplVodHandle
    OplVod* createVodObject(CVideoResource* aVideoResource);

    /* Video Component */
    virtual unsigned int getVideoComponentCount();
    virtual bool getVideoComponent(int index, OplAVComponent_t *out_comp);
    virtual	bool getActiveVideoComponent(OplAVComponent_t *out_comp);
    virtual bool setActiveVideoComponent(int pid);

    /* Audio Component */
    virtual	unsigned int getAudioComponentCount();
    virtual bool getAudioComponent(int index, OplAVComponent_t *out_comp);
    virtual	bool getActiveAudioComponent(OplAVComponent_t *out_comp);
    virtual	bool setActiveAudioComponent(int pid);

    /* Subtitle Component */
    virtual unsigned int getSubtitleComponentCount();
    virtual bool getSubtitleComponent(int index, OplAVComponent_t *out_comp);
    virtual bool getActiveSubtitleComponent(OplAVComponent_t *out_comp);
    virtual bool setActiveSubtitleComponent(int pid);

    /* Superimpose Component */
    virtual unsigned int getSuperimposeComponentCount();
    virtual bool getSuperimposeComponent(int index, OplAVComponent_t *out_comp);

    /* Data Component */
    virtual unsigned int getDataComponentCount();
    virtual bool getDataComponent(int index, OplAVComponent_t *out_comp);
private:
	OplVodJlabs& getVodJLabs();
};

#endif // __OPLVODJABSHANDLE_H__
