#include "OplAudioControl.h"
#include "hlib.h"
#include "apk.h"

#define WARNING_IF_APK_ERROR(x) \
    do{\
        if(ERR_OK != (x))\
            HxLOG_Error("APK API is failed. \n"); \
    }while(0)

#ifdef __cplusplus
extern "C" {
#endif
    int MI_AudioPCM_Init(void);
    int MI_AudioPCM_Open(void);
    int MI_AudioPCM_Close(void);
    int MI_AudioPCM_Destroy(void);
    int MI_AudioPCM_SetResource(unsigned int ulAudioMixerHandle);
    int MI_AudioPCM_AddInputToMixer(void);
    int MI_AudioPCM_DetachOutput(void);
    int MI_AudioPCM_AttachOutput(void);
    int MI_AudioPCM_PauseWriteData(void);
    int MI_AudioPCM_ResumeWriteData(void);

    int MI_AudioPCM_Start(void);
    int MI_AudioPCM_Stop(void);
    int MI_AudioPCM_WriteData(void *pBuf, unsigned int size);
    int MI_AudioPCM_GetAudioPCMStatus(void);

    int MI_AudioPCM_GetMuteStatus(bool *pbMute);
    int MI_AudioPCM_SetOutputMute(bool bOutputMute);
#ifdef __cplusplus
}
#endif

using namespace std;

#define ENTRY std::cout << "\x1b[45m\x1b[34m| " << __PRETTY_FUNCTION__ << "\x1b[0m" <<  std::endl;


/****************************************************************************************
 * OplAudioPlayer class
 ***************************************************************************************/

OplAudioPlayer::OplAudioPlayer(const char *playerName)
    :m_name(playerName),
    m_ePlayState(OPLVOD_PLAY_STATE_STOPPED),
    m_pfListener(NULL),
    m_player(NULL)
{
    ENTRY;
}

OplAudioPlayer::~OplAudioPlayer()
{
    ENTRY;
    int result = 0;
    result = MI_AudioPCM_Stop();
    if(result <0)
    {
        HxLOG_Error(" ## Webaudio ## OplAudioPlayer::~OplAudioPlayer-----MI_AudioPCM_Stop fail--------------\n");
    }
}


bool OplAudioPlayer::play()
{
    ENTRY;
    int result = MI_AudioPCM_Start();
    if(result <0)
    {
        HxLOG_Error(" ## Webaudio ## OplAudioPlayer::play-------MI_WebAudio_Start result(%d)------------\n", result);
        return false;
    }

    setPlayState(OPLVOD_PLAY_STATE_PLAYING);
    return true;
}

bool OplAudioPlayer::stop()
{
    ENTRY;
    MI_AudioPCM_Stop();
    return true;
}

bool OplAudioPlayer::pause()
{
    ENTRY;

    int result = MI_AudioPCM_PauseWriteData();
    if(result < 0)
    {
        HxLOG_Error(" ## Webaudio ## OplAudioPlayer::pause-------MI_AudioPCM_PauseWriteData fail------------\n");
        return false;
    }

    return true;
}
bool OplAudioPlayer::resume()
{
    ENTRY;

    int result = MI_AudioPCM_ResumeWriteData();
    if(result <0)
    {
        HxLOG_Error(" ## Webaudio ## OplAudioPlayer::connectOutput-------MI_AudioPCM_ResumeWriteData fail------------\n");
        return false;
    }

    return true;
}

bool OplAudioPlayer::push(void *data, unsigned int size)
{
    //ENTRY;
    return MI_AudioPCM_WriteData((void *)data, size);
}

void OplAudioPlayer::setListener(void *player, OplAudioPlayerListener_t listener)
{
    ENTRY;

    m_pfListener = listener;
    m_player = player;
}

std::string &OplAudioPlayer::getName()
{
    return m_name;
}

void OplAudioPlayer::setName(std::string name)
{
    ENTRY;
    m_name = name;
}

void OplAudioPlayer::setPlayState(eOplVideoOnDemandPlayState ePlayState)
{
    ENTRY;
    m_ePlayState = ePlayState;
}

eOplVideoOnDemandPlayState OplAudioPlayer::getPlayState()
{
    ENTRY;
    return m_ePlayState;
}

void OplAudioPlayer::eventCallback(eOplVideoOnDemandEventType eEventType)
{
    ENTRY;
    m_pfListener(m_player, this, eEventType);
}

/****************************************************************************************
 * OplAudioControl class
 ***************************************************************************************/
std::list<OplAudioPlayer *> OplAudioControl::s_audio_players;
unsigned int OplAudioControl::m_ulViewId;
OplAudioControl::eAudioConnectionStatus OplAudioControl::m_connect_status;

static void oplaudioplayer_proc(unsigned int ulViewId,
        unsigned int ulSessionId,
        OxMediaPlay_MediaType_e eType,
        OxMediaPlay_Event_e eEvent,
        OxMediaPlay_NotifierData_t *pstNotifyData,
        void *pvUserData);
static void oplaudioplayer_audio_output_proc(void);
static HBOOL oplaudioplayer_eventFilter (HUINT32 ulViewId, OxMediaPlay_MediaType_e eType, OxMediaPlay_Event_e eEvent, HBOOL *updated);


bool OplAudioControl::init(unsigned int ulAudioMixerHandle, unsigned int view_id)
{
    ENTRY;
	bool bMasterMute = false;

    WARNING_IF_APK_ERROR(APK_MEDIA_PLAY_RegisterNotifier(view_id, oplaudioplayer_proc, oplaudioplayer_eventFilter, NULL));
    WARNING_IF_APK_ERROR(APK_OUTPUT_AUDIO_RegisterNotifier(oplaudioplayer_audio_output_proc));


    m_ulViewId = view_id;
    int nRet = 0;

    if(ulAudioMixerHandle == 0)
    {
        HxLOG_Error("ulAudioMixerHandle is 0\n");
        return false;
    }

    nRet = MI_AudioPCM_Init();
    if(nRet < 0)
    {
        if(nRet == -2)
        {
            HxLOG_Error("AudioPCM is already initialized!, nRet = %d\n", nRet);
            if ( s_audio_players.empty() )
            {
                MI_AudioPCM_AttachOutput();
            }
        }
        else
        {
            HxLOG_Error("Failed to initialize AudioPCM, nRet = %d\n", nRet);
            return false;
        }
    }
    else
    {
        nRet = MI_AudioPCM_SetResource(ulAudioMixerHandle);
        if(nRet < 0)
        {
            HxLOG_Error("Failed to set AudioPCM resource, nRet = %d\n", nRet);
            return false;
        }

        nRet = MI_AudioPCM_Open();
        if(nRet < 0)
        {
            HxLOG_Error("Failed to open AudioPCM, nRet = %d\n", nRet);
            return false;
        }

        nRet = MI_AudioPCM_AddInputToMixer();
        if(nRet < 0)
        {
            HxLOG_Error("Failed to connect AudioPlayback input\n");
            return false;
        }
    }

    WARNING_IF_APK_ERROR(APK_OUTPUT_AUDIO_GetMasterMute((HBOOL *)&bMasterMute));
    nRet = MI_AudioPCM_SetOutputMute(bMasterMute);
    if(nRet < 0)
    {
        HxLOG_Error("Failed to set mute mode, nRet = %d\n", __func__, __LINE__, nRet);
        return false;
    }

    return true;
}

bool OplAudioControl::deinit()
{
    ENTRY;

    int result = 0;

    WARNING_IF_APK_ERROR(APK_MEDIA_PLAY_UnregisterNotifier(m_ulViewId, oplaudioplayer_proc, NULL));
    WARNING_IF_APK_ERROR(APK_OUTPUT_AUDIO_UnregisterNotifier());


    result = MI_AudioPCM_Stop();
    if(result <0)
    {
        HxLOG_Error(" ## Webaudio ## OplAudioPlayer::~OplAudioPlayer-----MI_AudioPCM_Stop fail--------------\n");
        return false;
    }
    else
    {
        MI_AudioPCM_Close();
        MI_AudioPCM_Destroy();
    }

    return true;
}

OplAudioPlayer* OplAudioControl::createPlayer(const char *name)
{
    ENTRY;
    OplAudioPlayer *player = new OplAudioPlayer(name);
    s_audio_players.push_back(player);
    return player;
}

void OplAudioControl::destroyPlayer(OplAudioPlayer *player)
{
    ENTRY;

    s_audio_players.remove(player);

    if ( s_audio_players.empty() )
        deinit();
    delete player;
}

bool OplAudioControl::suspendAll()
{
    ENTRY;
    MI_AudioPCM_PauseWriteData();
    return true;
}

bool OplAudioControl::resumeAll()
{
    ENTRY;
    MI_AudioPCM_ResumeWriteData();
    return true;
}

OplAudioControl::eAudioControlStatus OplAudioControl::getStatus()
{
    ENTRY;

    int ret = MI_AudioPCM_GetAudioPCMStatus();
    switch(ret)
    {
        case 0: return AudioControl_Initialized;
        case 1: return AudioControl_Started;
        case 2: return AudioControl_Stopped;
        case 3: return AudioControl_Closed;
        case 4: return AudioControl_Destroyed;
    }

    return AudioControl_NotInitialized;
}


OplAudioPlayer *OplAudioControl::getAudioPlayer(const char *name)
{
    //ENTRY;
    if ( s_audio_players.empty() ) return NULL;

    for ( list<OplAudioPlayer*>::iterator it=s_audio_players.begin(); it!=s_audio_players.end(); it++ )
    {
        OplAudioPlayer *player= static_cast <OplAudioPlayer*> (*it);
        if ( !(player->getName()).compare(name))
            return player;
    }

    return NULL;
}

bool OplAudioControl::connectOutput()
{
    ENTRY;

    if (  m_connect_status == AudioConnection_Attached )
        return true;

    int result = MI_AudioPCM_AttachOutput();
    if(result < 0)
        HxLOG_Error(" ## Webaudio ## OplAudioPlayer::connectOutput-------MI_AudioPCM_AttachOutput fail---but resumewrite---------\n");

    m_connect_status = AudioConnection_Attached;

    return true;
}

bool OplAudioControl::disconnectOutput()
{
    ENTRY;

    int result = 0;
    result = MI_AudioPCM_DetachOutput();
    if(result <0)
        HxLOG_Error(" ## Webaudio ## OplAudioPlayer::disconnectOutput-----MI_AudioPCM_DetachOutput fail--------------\n");

    m_connect_status = AudioConnection_Detached;

    return true;
}



static void oplaudioplayer_proc(unsigned int ulViewId,
        unsigned int ulSessionId,
        OxMediaPlay_MediaType_e eType,
        OxMediaPlay_Event_e eEvent,
        OxMediaPlay_NotifierData_t *pstNotifyData,
        void *pvUserData)
{
    switch(eType)
    {
        case eOxMP_MEDIATYPE_MEDIAPLAY:
            {
                switch(eEvent)
                {
                    case eOxMP_EVENT_STOPPED:
#if 0
                        OplAudioPlayer *player = OplAudioControl::getAudioPlayer("webaudio");
                        player->setPlayState(OPLVOD_PLAY_STATE_STOPPED);
                        MI_AudioPCM_AttachOutput();
#endif
                        break;
                    case eOxMP_EVENT_PLAYING:
                        break;

                    default:
                        break;
                }
                break;
            }

        default:
            break;
    }

    return;
}

static void oplaudioplayer_audio_output_proc(void)
{
	int nRet = 0;
	bool bMasterMute = false;

	WARNING_IF_APK_ERROR(APK_OUTPUT_AUDIO_GetMasterMute((HBOOL *)&bMasterMute));

	nRet = MI_AudioPCM_SetOutputMute(bMasterMute);

	if(nRet < 0)
		HxLOG_Error("\033[1;31m @@@@@ [%s:%d] Failed to set mute mode, nRet = %d\033[0m\n", __func__, __LINE__, nRet);
}

static HBOOL oplaudioplayer_eventFilter (HUINT32 ulViewId, OxMediaPlay_MediaType_e eType, OxMediaPlay_Event_e eEvent, HBOOL *updated)
{
    *updated = FALSE;

    if (eOxMP_MEDIATYPE_MEDIAPLAY == eType)
        *updated = TRUE;

    return TRUE;
}
