#ifndef DEVICE_AUDIOMIXEROUTPUT_BRCM_H
#define DEVICE_AUDIOMIXEROUTPUT_BRCM_H

//#define DEBUG_MIXOUT_DUMP


#include <nrd/IAudioMixerOutput.h>
#include <nrdbase/tr1.h>
#include "AudioMixerOutputThread.h"

namespace netflix {
namespace device {

//class MixerOutputThread;

typedef struct AudioMixerOutputBrcmResourceMap
{
	unsigned int *hPlaypumpList;
	unsigned int *hDmaList;
	unsigned int uiNumOfPlaypump;
	unsigned int uiNumOfDma;
	unsigned int uiAudioMixerHandle;
}AudioMixerOutputBrcmResourceMap;

class AudioMixerOutputBRCM : public IAudioMixerOutput
{
public:

    AudioMixerOutputBRCM();
    ~AudioMixerOutputBRCM();

    /* Netflix DPI - IAudioMixerOutput APIs */
    virtual NFErr Play();
    virtual NFErr Reset();
    virtual NFErr getBuffer(void ** buffer_ptr, const size_t requestedFrames, size_t& contiguousFrames);
    virtual NFErr commitBuffer(const size_t writtenFrames);
    virtual NFErr getBufferAvailable(size_t& availableFrames);
    virtual void  getDeviceInfo(size_t& preferredFrames, size_t& maximumFrames, bool& supportDeferredPlay);
	virtual NFErr getBufferDelay(size_t& delayFrames);
	NFErr		  pauseTransfer();
	NFErr		  resumeTransfer();
	NFErr		  detachOutput();
	NFErr		  attachOutput();
    void    	  asyncCommit();
	NFErr 		  Stop();	 /* This is for suspend callback */
private:
    void init();
	bool prepareResources();
	unsigned long getSystemTick(void);
		
	bool		bInit;
	bool		bPlaybackStarted;
    void*       m_pReservedBuffer;
    size_t		m_szReservedBuffer;
	unsigned int m_prevMixerHandle;

	shared_ptr<MixerOutputThread> mAyncCommitThread;
	std::vector<char> m_IntBuffer;
	bool		b_runningThread;
	bool		b_useIntBuffer;
	size_t		m_szNeedToWriteByte;
	Mutex m_Mutex;
	AudioMixerOutputBrcmResourceMap m_ResourceMap;
#ifdef DEBUG_MIXOUT_DUMP
    int         m_DumpIndex;
#endif    
    // 960 frames = 3840 bytes = 20ms
    // 2880 frames = 11520 bytes =  60ms
    static const size_t BRCM_FIFO_MAX_FRAMES = 960;//2880;
    static const size_t BRCM_FIFO_PREFERRED_FRAMES = 960;//960;
};

} // namespace device
} // namespace netflix

#endif // DEVICE_AUDIOMIXEROUTPUT_BRCM_H
