#include <nrddpi/AudioMixerSoftware.h>
#include "AudioMixerOutputBrcm.h"
#include "NativeBridge.h"

#include <nrd/AppLog.h>
#include <nrd/NrdApplication.h>

//for debug
#include <time.h>
#include <sys/time.h>


using namespace netflix;
using namespace netflix::device;

extern "C"
{
    typedef struct __audio_playback_if_proto
    {
    	int (*init)(void);
    	int (*open)(void);
    	int (*close)(void);
    	int (*destroy)(void);
    	int (*set_resource)(unsigned int ulAudioMixerHandle);
    	int (*add_input_to_mixer)(void);
    	int (*start)(void);
    	int (*stop)(void);
    	int (*write_data)(void *pBuf, unsigned int unInjSize);
    	int (*pause_write_data)(void);
    	int (*resume_write_data)(void);	
    	int (*get_buffer)(void **pBuf, size_t *pSize);
    	int (*read_complete)(size_t szWritten);
    	int (*get_remaining)(void);
    	int (*detach_output)(void);
    	int (*attach_output)(void);
    	int (*set_output_mute)(bool bOutputMute);
    	int (*get_mute_state)(bool *pbMute);
    	void (*set_mute_state)(bool bMute);
    } MI_AUDIOPLAYBACK_IF_PROTO;

    extern MI_AUDIOPLAYBACK_IF_PROTO MI_AudioPlayback_NetflixAudioIF;
};

#define USE_AUDIOMIXER_ALTERNATIVE_STOP

#define HUMAX_IFACE (MI_AudioPlayback_NetflixAudioIF)

/* Func. for converting unit provided by AudioMixerSoftware */
#define FRM_TO_MS(x)		AudioMixerSoftware::framesToMs(x)
#define FRM_TO_BYTE(x)		AudioMixerSoftware::framesToBytes(x)
#define FRM_TO_SAMPLE(x)	AudioMixerSoftware::framesToSamples(x)
#define SAMPLE_TO_FRM(x)	AudioMixerSoftware::samplesToFrames(x)
#define BYTE_TO_SAMPLE(x)	AudioMixerSoftware::bytesToSamples(x)
#define BYTE_TO_FRM(x)		AudioMixerSoftware::samplesToFrames( AudioMixerSoftware::bytesToSamples(x) )

/* Fixed setting */
#define SAMPLE_RATE		48000
#define BIT_PER_SAMPLE	16
#define IS_SIGNED		1
#define IS_STEREO		1

#if 0
#define LOGFILE_PATH "/media/drive1/log.txt"

#define RELEASE_PRINT(...) \
    {\
        FILE *fp = (FILE *)0x0;\
        fp = fopen(LOGFILE_PATH, "a");\
        if(fp)\
        {\
            fprintf(fp, __VA_ARGS__);\
            fclose(fp);\
        }\
    }

//Timestamp print macro
#define TIMESTAMP_PRINT \
	{\
		struct timeval timestamp; \
		gettimeofday(&timestamp, 0); \
		time_t curtime = timestamp.tv_sec; \
		struct tm *convTime = localtime(&curtime); \
		RELEASE_PRINT("%02d:%02d:%02d:%03d : ", convTime->tm_hour, convTime->tm_min, convTime->tm_sec, (int)timestamp.tv_usec/1000); \
	}

#else

#define RELEASE_PRINT(...)
#define TIMESTAMP_PRINT

#endif


AudioMixerOutputBRCM::AudioMixerOutputBRCM()
: b_runningThread(false), b_useIntBuffer(false), bPlaybackStarted(false), bInit(false), m_szNeedToWriteByte(0), m_Mutex(UNTRACKED_MUTEX, "AudioMixerOutputMutex")
{
#ifdef DEBUG_MIXOUT_DUMP
  m_DumpIndex = 0;
#endif

	init();
}

AudioMixerOutputBRCM::~AudioMixerOutputBRCM()
{
    if(bInit == true)
    {
        bool bMute;
        
        (void)HUMAX_IFACE.stop();
    	if( NativeBridge::esManager().getSystemMuteStatus(&bMute) != NativeBridge_ERR_OK)
    	{
    		Log::error(TRACE_DPI, "Can not get system mute status");
    	}
    	else
    	{
    		(void) HUMAX_IFACE.set_output_mute(bMute);
            Log::error(TRACE_DPI, "Mute setting to %d\n", bMute);
    	}    
        
        (void)HUMAX_IFACE.close();
        (void)HUMAX_IFACE.destroy();
    }
    bPlaybackStarted = false;
    mAyncCommitThread.reset();
}

void AudioMixerOutputBRCM::asyncCommit()
{
    int szTotalWrite = m_szNeedToWriteByte;
    size_t szBuffer = 0; 
    void *pDstBuffer = NULL;
    char *pSrcBuffer = (char *)m_pReservedBuffer;

    if( !m_pReservedBuffer )
    {
        Log::error(TRACE_DPI, "MixerOutput - AyncCommit : Playback handle is null or reserved buffer does not exist!");
        goto  EXIT_ASYNC;
    }

    m_Mutex.lock();
	do
    {
        if( HUMAX_IFACE.get_buffer( &pDstBuffer, &szBuffer) < 0)
        {
            Log::error(TRACE_DPI, "MixerOutput - AyncCommit : Get buffer failed\n");
            m_Mutex.unlock();
            goto EXIT_ASYNC;
        }
        //Log::error(TRACE_DPI, "AsyncCommit : Get buffer returns %d bytes",szBuffer);
        if(szBuffer)
        {
            int szToWrite = szTotalWrite > szBuffer ? szBuffer : szTotalWrite;
            //Log::error(TRACE_DPI, "AsyncCommit : Buffer writing ... From offset %d, size %d", m_szNeedToWriteByte - szTotalWrite, szToWrite);
            memcpy(pDstBuffer, &pSrcBuffer[m_szNeedToWriteByte - szTotalWrite], szToWrite);
            if( HUMAX_IFACE.read_complete(szToWrite) < 0)
            {
                Log::error(TRACE_DPI, "MixerOutput - AyncCommit : Write failed\n");
                m_Mutex.unlock();
                goto EXIT_ASYNC;
            }
            szTotalWrite -= szToWrite;
        }        
    } while(szTotalWrite > 0);
    m_Mutex.unlock();

    //Thread::Sleep( (FRM_TO_MS(BYTE_TO_FRM(m_szNeedToWriteByte)) - 25 ) /* roughly calling time */ );

EXIT_ASYNC:
    b_runningThread = false;
    b_useIntBuffer = false;
	m_pReservedBuffer = NULL;
	m_szReservedBuffer = 0;
    //Log::error(TRACE_DPI, "AyncCommit : Complete");
    return;
}

NFErr AudioMixerOutputBRCM::pauseTransfer()
{
    if(bInit == true)
    {
        (void)HUMAX_IFACE.pause_write_data();
    }

    return NFErr_OK;
}

NFErr AudioMixerOutputBRCM::resumeTransfer()
{
    bool bMute = false;
    
    if(bInit == true)
    {
        (void)HUMAX_IFACE.resume_write_data();
        (void)prepareResources();
        
    	if( NativeBridge::esManager().getSystemMuteStatus(&bMute) != NativeBridge_ERR_OK)
    	{
    		Log::error(TRACE_DPI, "Can not get system mute status");
    	}
    	else
    	{
    		(void) HUMAX_IFACE.set_output_mute(bMute);
    	}
        
        (void)HUMAX_IFACE.start();
    }
    
    return NFErr_OK;    
}

NFErr AudioMixerOutputBRCM::detachOutput()
{    
    if(bInit == true)
    {
    while(b_runningThread)
    {
        Thread::Sleep(5);
    }
    (void)HUMAX_IFACE.detach_output();
    }

    return NFErr_OK;
}

NFErr AudioMixerOutputBRCM::attachOutput()
{  
    if(bInit == true)
    {
    while(b_runningThread)
    {
        Thread::Sleep(5);
    }
    (void)HUMAX_IFACE.attach_output();
    }
    return NFErr_OK;
}

NFErr AudioMixerOutputBRCM::Play()
{
    init();
	bool bMute = false;

	if( NativeBridge::esManager().getSystemMuteStatus(&bMute) != NativeBridge_ERR_OK)
	{
		Log::error(TRACE_DPI, "Can not get system mute status");
	}
	else
	{
		(void) HUMAX_IFACE.set_output_mute(bMute);
	}

    if(bPlaybackStarted == false)
	{
        (void) HUMAX_IFACE.start();
        bPlaybackStarted = true;
    }

    TIMESTAMP_PRINT;
    RELEASE_PRINT("Mixer Play started\n");

	return NFErr_OK;
}

NFErr AudioMixerOutputBRCM::Stop()
{
    if(bInit == true)
    {
#ifdef USE_AUDIOMIXER_ALTERNATIVE_STOP
        bool bMute;
        
        if(bPlaybackStarted == true)
        {
            (void)HUMAX_IFACE.stop();
            bPlaybackStarted = false;
        }
    	if( NativeBridge::esManager().getSystemMuteStatus(&bMute) != NativeBridge_ERR_OK)
    	{
    		Log::error(TRACE_DPI, "Can not get system mute status");
    	}
    	else
    	{
    		(void) HUMAX_IFACE.set_output_mute(bMute);
            Log::error(TRACE_DPI, "Mute setting to %d\n", bMute);
    	}
        (void)HUMAX_IFACE.close();
        (void)HUMAX_IFACE.destroy();
        m_prevMixerHandle = 0;

		bInit = false;
#else
        /* Playback will be automatically stopped by LiveTune. so we need to set flag to false */
        bPlaybackStarted = false;
#endif        
    }
    mAyncCommitThread.reset();
    //bInit = false;
    
	return NFErr_OK;
}

NFErr AudioMixerOutputBRCM::Reset()
{
	int nQueuedSize = 0;
	const int nMaxThresholdPrint = 30;
	int nThresholdPrint = 0;

	/* Waiting for AudioPlayback buffer is flat */
	do
	{
        nQueuedSize = HUMAX_IFACE.get_remaining();
		if(nQueuedSize == 0)
		{
			//Log::error(TRACE_DPI, "Queue is flat");
			break;
		}
		else if (nQueuedSize > 0)
		{
			nThresholdPrint++;
			if(nThresholdPrint > nMaxThresholdPrint)
			{
				//Log::error(TRACE_DPI, "Waiting for queue is flat (Remaining : %d)", nQueuedSize);
			}
			Thread::Sleep(1);
			continue;
		}
		else
		{
			Log::error(TRACE_DPI, "MI_AudioPlayback_GetQueuedBytes returns negative! Stop immediately");
			break;
		}
	}
	while(true);

    m_Mutex.lock();
	if(m_pReservedBuffer)
	{
		m_pReservedBuffer = NULL;
		m_szReservedBuffer = 0;
	}   
    m_Mutex.unlock();

	Log::error(TRACE_DPI, "Reset complete");

    TIMESTAMP_PRINT;
    RELEASE_PRINT("Reset finished (%d bytes)\n", m_szNeedToWriteByte);   
#ifdef DEBUG_MIXOUT_DUMP
	m_DumpIndex++;
#endif

	return NFErr_OK;
}

void AudioMixerOutputBRCM::getDeviceInfo(size_t& preferredFrames, size_t& maximumFrames, bool& supportDeferredPlay)
{
	maximumFrames = BRCM_FIFO_MAX_FRAMES;
	preferredFrames = BRCM_FIFO_PREFERRED_FRAMES;
	supportDeferredPlay = false;
}

NFErr AudioMixerOutputBRCM::getBufferAvailable(size_t& availableFrames)
{
	init();

    if (b_useIntBuffer == false)
    {
        m_Mutex.lock();
	    // If device buffer is not enough, use Int buffer
	    m_IntBuffer.resize( FRM_TO_BYTE(BRCM_FIFO_PREFERRED_FRAMES) );
	    m_pReservedBuffer = &m_IntBuffer[0];
	    m_szReservedBuffer = FRM_TO_BYTE(BRCM_FIFO_PREFERRED_FRAMES);
	    availableFrames = BRCM_FIFO_PREFERRED_FRAMES;
	    b_useIntBuffer = true;
	    //Log::error(TRACE_DPI, "Set InternalBuffer (%d frames)", availableFrames);
        m_Mutex.unlock();
    }
    else
    {
        // If device buffer is not enough and buffer is in use, 
      	availableFrames = 0;
        //Log::error(TRACE_DPI, "No Buffer available", availableFrames);
    }  
	return NFErr_OK;
}

NFErr AudioMixerOutputBRCM::getBuffer(void ** buffer_ptr, const size_t requestFrames, size_t& contiguousFrames)
{
	init();

	bool bMute = false;

	if( NativeBridge::esManager().getSystemMuteStatus(&bMute) != NativeBridge_ERR_OK)
	{
		Log::error(TRACE_DPI, "Can not get system mute status");
	}
	else
	{
		(void) HUMAX_IFACE.set_output_mute(bMute);
	}

	*buffer_ptr = m_pReservedBuffer;
	contiguousFrames = BYTE_TO_FRM((int)m_szReservedBuffer);
	
    TIMESTAMP_PRINT;
    RELEASE_PRINT("Get buffer (%d frames)\n", contiguousFrames);

	//Log::error(TRACE_DPI, "Getbuffer returns buffer with size (%d, to frames : %d (Requested : %d))", m_szReservedBuffer, contiguousFrames, requestFrames); 
	
	return NFErr_OK;
}

NFErr AudioMixerOutputBRCM::getBufferDelay(size_t& delayFrames)
{
	while(b_runningThread)
	{
		Thread::Sleep(1);
	}
	delayFrames = BYTE_TO_FRM(HUMAX_IFACE.get_remaining());

	return NFErr_OK;
}

NFErr AudioMixerOutputBRCM::commitBuffer(const size_t framesWritten)
{
#ifdef DEBUG_MIXOUT_DUMP
	char filename[256];
	snprintf(filename, 256, "/mnt/hd2/%s%d.dump", "amixout-", m_DumpIndex);
	FILE* fdump = fopen(filename, "a");
	fwrite(m_pReservedBuffer, FRM_TO_BYTE(framesWritten), 1, fdump);
	fclose(fdump);	
#endif

    if(b_runningThread == false)
    {
        //create aync thread
        //Log::error(TRACE_DPI, "Commit using internal buffer");
        m_szNeedToWriteByte = FRM_TO_BYTE(framesWritten);
        b_runningThread = true;
        mAyncCommitThread.reset(new MixerOutputThread(*this, &AudioMixerOutputBRCM::asyncCommit));
    }
    else
    {
        Log::error(TRACE_DPI, "Commit failed.  useIntBuffer(%d), runningThread(%d)", b_useIntBuffer, b_runningThread);
        return NFErr_Bad;
    }
    
	return NFErr_OK;
}

void AudioMixerOutputBRCM::init()
{
	bool bMute = false;

    if(!bInit)
    {
        (void) HUMAX_IFACE.init();
        (void) HUMAX_IFACE.open();
        if(prepareResources() == false)
		{
        	(void) HUMAX_IFACE.close();
        	(void) HUMAX_IFACE.destroy();
			return;
		}

        if( NativeBridge::esManager().getSystemMuteStatus(&bMute) != NativeBridge_ERR_OK)
        {
	        Log::error(TRACE_DPI, "Can not get system mute status");
	    }
	    else
		{
			(void) HUMAX_IFACE.set_output_mute(bMute);
		}
		
		bInit = true;
        Log::error(TRACE_DPI, "Mixer initializaed");

        TIMESTAMP_PRINT;
        RELEASE_PRINT("Mixer initialized\n");
    }

	return;
}

bool AudioMixerOutputBRCM::prepareResources()
{
    if( NativeBridge::esManager().getResourceMap((void *)&m_ResourceMap) != NativeBridge_ERR_OK )
    {
        Log::error(TRACE_DPI, "Can not get resource map");
        return false;           
    }
    if(m_prevMixerHandle != m_ResourceMap.uiAudioMixerHandle)
    {
        (void) HUMAX_IFACE.set_resource(m_ResourceMap.uiAudioMixerHandle);
        if( 0 != HUMAX_IFACE.add_input_to_mixer() )
	{
		Log::error(TRACE_DPI, "Can not set mixer to output.\n");
		(void) HUMAX_IFACE.set_resource(0);
		m_prevMixerHandle = 0;
		return false;
	}
        Log::error(TRACE_DPI, "Mixer Handle(%X -> %X) is set to audio playback as output\n", m_prevMixerHandle, m_ResourceMap.uiAudioMixerHandle);
        m_prevMixerHandle = m_ResourceMap.uiAudioMixerHandle;
    }
    else
    {
        Log::error(TRACE_DPI, "Mixer handle is the same as previous. Skip to set resource\n");
    }
    return true;
}
