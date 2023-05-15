#ifndef ___AS_APK_OUTPUT_AUDIO_H___
#define ___AS_APK_OUTPUT_AUDIO_H___

#include <as_apk_output_audio_adaptor.h>

class cApkOutputAudio
  : public Octopus::Appkit::Output::Audio_adaptor,
  public DBus::IntrospectableAdaptor,
  public DBus::ObjectAdaptor
{
public:
	static char const * const BUSNAME_APK_OUTPUT_AUDIO;
	static char const * const BUSPATH_APK_OUTPUT_AUDIO;

public:
	cApkOutputAudio(DBus::Connection &connection);

    bool SupportSpdif();
    bool GetMasterMute();
    void SetMasterMute(const bool& mute);
    uint32_t GetVolumeMax();
    uint32_t GetVolume();
    void SetVolume(const uint32_t& volume);
    uint32_t GetLipSyncDelay();
    void SetLipSyncDelay(const uint32_t& mesec);
    void SetHdmiFormat(const uint32_t& digitalAudioFormat);
    uint32_t GetHdmiFormat();
    void SetSpdifFormat(const uint32_t& digitalSpdifFormat);
    uint32_t GetSpdifFormat();
    bool GetAudioDescription();
    void SetAudioDescription(const bool& enable);
    void SetStereoSelect(const uint32_t& stereoSelect);
    uint32_t GetStereoSelect();
    void SetDualMonoSelect(const uint32_t& dualMonoSelect);
    uint32_t GetDualMonoSelect();

private:
	static void callback_Event (void);

/*
 * APK_OUTPUT_AUDIO�� Event Callback Interface�� userdata�� ���� �� �� ����. �׷��� dbus�� event�� 
   ȣ�� �� �� ���� ���� �̱��� �����ͷ� �����Ѵ�.
 */
 	static cApkOutputAudio*	pMainThis;

};

#endif
