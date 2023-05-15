#ifndef ___AS_APK_MEDIA_PLAY_H___
#define ___AS_APK_MEDIA_PLAY_H___

#include <as_apk_media_play_adaptor.h>

class cApkMediaPlay
  : public Octopus::Appkit::Media::Play_adaptor,
  public DBus::IntrospectableAdaptor,
  public DBus::ObjectAdaptor
{
public:
	static char const * const BUSNAME_APK_MEDIA_PLAY;
	static char const * const BUSPATH_APK_MEDIA_PLAY;

public:
	cApkMediaPlay(DBus::Connection &connection);

	uint32_t GetViewNumber();
	void SetMainViewId(const uint32_t& viewId);
	uint32_t GetMainViewId();
	uint32_t GetPlayType(const uint32_t& viewId);
	bool GetNeedRadioBg(const uint32_t& viewId);
	uint32_t GetSessionId(const uint32_t& viewId);
	uint32_t GetRequestId(const uint32_t& viewId, const uint32_t& playType);
	uint32_t GetStreamAspectRatio(const uint32_t& viewId);
	::DBus::Struct< int32_t, int32_t > GetMhegDisplayPoint(const uint32_t& viewId, const int32_t& refWidth, const int32_t& refHeight, const int32_t& videoPointX, const int32_t& videoPointY);
	void StartAudioClip(const uint32_t& requestId, const std::vector< uint32_t >& buffer, const uint32_t& bufferBytes, const uint32_t& audioCodec, const uint32_t& loopCount);
	void PauseAudioClip(const uint32_t& requestId);
	void ResumeAudioClip(const uint32_t& requestId);
	void StopAudioClip(const uint32_t& requestId);
	void SetVideoFreeze(const uint32_t& viewId, const bool& freeze);
	void SetAudioFreeze(const uint32_t& viewId, const bool& freeze);
	void SetComponentIndex(const uint32_t& viewId, const uint32_t& compType, const int32_t& compIndex);
	int32_t GetComponentNum(const uint32_t& viewId, const uint32_t& compType);
	virtual ::DBus::Struct< int32_t, int32_t, uint8_t, int32_t, int32_t, int32_t, int32_t, std::string > GetSubtitleComponent(const uint32_t& viewId, const int32_t& compIndex);
	int32_t GetComponentIndex(const uint32_t& viewId, const uint32_t& compType);
    uint32_t StartLive(const uint32_t& viewId, const ::DBus::Struct< int32_t, int32_t, int32_t, int32_t, int32_t >& startInfo);
    uint32_t StartPvrPb(const uint32_t& viewId, const ::DBus::Struct< int32_t, int32_t >& startInfo);
    uint32_t StartTsrPb(const uint32_t& viewId, const ::DBus::Struct< int32_t, int32_t, int32_t, int32_t >& startInfo);
    uint32_t StartMedia(const uint32_t& viewId, const ::DBus::Struct< int32_t, int32_t, int32_t, int32_t, int32_t, std::string >& startInfo);
	void Stop(const uint32_t& viewId);
	void SetPlaySpeed(const uint32_t& viewId, const int32_t& speed);
	void SetPlayPosition(const uint32_t& viewId, const uint32_t& position);
	int32_t GetPlaySpeed(const uint32_t& viewId);
	uint32_t GetPlayPosition(const uint32_t& viewId);
	uint32_t GetPlayState(const uint32_t& viewId);
	uint32_t GetPlayError(const uint32_t& viewId);
	uint32_t GetBufferedTime(const uint32_t& viewId);
	uint32_t GetDurationTime(const uint32_t& viewId);
	uint32_t GetTsrStartTime(const uint32_t& viewId);
	std::vector< int32_t > GetSupportedSpeeds(const uint32_t& viewId, const uint32_t& maxNumSpeed);
	uint32_t GetTrickRestrictMode(const uint32_t& viewId);
	::DBus::Struct< int32_t, int32_t, int32_t, int32_t > GetVideoSize(const uint32_t& viewId);
	void SetVideoSize(const uint32_t& viewId, const int32_t& startX, const int32_t& startY, const int32_t& width, const int32_t& height);
	void ResetVideoSize(const uint32_t& viewId);
	void SetPigRect(const uint32_t& viewId, const uint32_t& zOrder, const int32_t& isX, const int32_t& isY, const int32_t& iW, const int32_t& iH, const int32_t& irW, const int32_t& irH, const int32_t& osX, const int32_t& osY, const int32_t& oW, const int32_t& oH, const int32_t& orW, const int32_t& orH, const uint32_t& osdAspectRatio, const uint32_t& dfc, const uint32_t& pigAspectRatio);
	void SetTSREnable(const bool& enable);
	void SetSubtitleEnable(const uint32_t& viewId, const bool& enable);
	void SetVideoHide(const uint32_t& viewId, const bool& hide);
	bool GetTSREnable();
	bool GetSubtitleEnable(const uint32_t& viewId);
	std::vector< uint8_t > GetEventInfo(const uint32_t& viewId);
	void SaveThumbnail(const uint32_t& viewId, const std::string& filename, const uint32_t& width, const uint32_t& height);
	uint32_t GetMajorChannel(const uint32_t& viewId);
	bool CheckPlayChangable(const uint32_t& viewId, const uint32_t& playType, const std::vector< uint8_t >& startInfo);
	uint32_t GetViewState(const uint32_t& viewId);
	uint32_t GetLockState(const uint32_t& viewId);
	void MemorizeMediaState(const uint32_t& viewId, const bool& stopMedia);
	void RestoreMediaState(const uint32_t& viewId);
	uint32_t GetThumbnail(const uint32_t& viewId, const uint32_t& pos);
    std::string GetURI(const uint32_t& viewId);
    void Probe(const uint32_t& viewId, const std::string& uri);
    void SetSubtitleSyncTime(const uint32_t& viewId, const uint32_t& subtitleSyncTime);
    void SetSubtitleFontSize(const uint32_t& viewId, const uint32_t& subtitleFontSize);
    void SetSubtitleTextPosition(const uint32_t& viewId, const uint32_t& subtitleTextPosition);
    uint32_t GetSubtitleSyncTime(const uint32_t& viewId);
    uint32_t GetSubtitleFontSize(const uint32_t& viewId);
    uint32_t GetSubtitleTextPosition(const uint32_t& viewId);
    ::DBus::Struct< uint32_t, uint32_t, uint32_t, uint32_t, uint32_t > GetSubtitleStatus(const uint32_t& viewId);
    void SetBufferingScenario(const uint32_t& viewId, const bool& isAuto, const int32_t& threshold);
    ::DBus::Struct< uint32_t, std::vector< uint32_t >, std::vector< uint32_t > > GetBufferingChunkInfo(const uint32_t& viewId);
    uint32_t GetBufferingRemainedTime(const uint32_t& viewId);
    uint32_t GetBufferingPercent(const uint32_t& viewId);
    uint32_t GetBufferState(const uint32_t& viewId);
    void SetDrmLicenseData(const uint32_t& viewId, const std::vector< uint8_t >& aData);
    void SetBrowserUserAgentData(const uint32_t& viewId, const std::vector< ::DBus::Variant >& aUAData);
    ::DBus::Struct< std::vector< uint32_t >, std::vector< uint32_t >, uint32_t, uint32_t > GetResourceMap();
    uint32_t GetPumpIndex(const uint32_t& viewId);
    void SetDuration(const uint32_t& viewId, const int64_t& duration);
    void SetPlayerType(const uint32_t& viewId, const uint32_t& playerType);
    void SetPlayReadyInfo(const uint32_t& viewId, const std::string& laURL, const std::vector< uint8_t >& customData, const bool& controlFlag);
    std::vector< uint8_t > GetVmxIdentifier();
    int32_t GetVmxStatus(const uint32_t& viewId);
    int32_t GetVmxError(const uint32_t& viewId);
    void SetVmxCas(const uint32_t& viewId, const std::string& VCASUrl, const std::string& vendorName);
    void UnsetVmxCas(const uint32_t& viewId);

private:
	static void callback_Event (HUINT32 ulViewId,
								HUINT32 ulSessionId,
								OxMediaPlay_MediaType_e eType,
								OxMediaPlay_Event_e eEvent,
								OxMediaPlay_NotifierData_t *pstNotifyData,
								void *pvUserData
								);

};

#endif
