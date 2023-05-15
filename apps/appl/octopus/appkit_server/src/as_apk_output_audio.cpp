#include <apk.h>


#include <vkernel.h>

#include <dbus-c++/dbus.h>
#include <as_apk_output_audio.h>

char const * const cApkOutputAudio::BUSNAME_APK_OUTPUT_AUDIO = "Octopus.Appkit.Output.Audio";
char const * const cApkOutputAudio::BUSPATH_APK_OUTPUT_AUDIO = "/Octopus/Appkit/Output/Audio";

cApkOutputAudio * cApkOutputAudio::pMainThis	=	NULL;

void cApkOutputAudio::callback_Event (void)
{
	if (NULL == pMainThis)
		return;

	pMainThis->event ();					
}

cApkOutputAudio::cApkOutputAudio(DBus::Connection &connection)
  : DBus::ObjectAdaptor(connection, BUSPATH_APK_OUTPUT_AUDIO)
{
	if (NULL == pMainThis)
		pMainThis = this;

	/* apkit output audio */
    {	
		HERROR hErr = ERR_FAIL;
		hErr = APK_OUTPUT_AUDIO_Init ();
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("APK_OUTPUT_AUDIO_Init Error : %d", hErr);
		}

		
		hErr = APK_OUTPUT_AUDIO_RegisterNotifier (cApkOutputAudio::callback_Event);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("APK_OUTPUT_AUDIO_RegisterNotifier Error : %d", hErr);
		}
		
		//HERROR APK_OUTPUT_AUDIO_UnregisterNotifier(void);
    }
}

bool cApkOutputAudio::SupportSpdif()
{
	HERROR hErr = ERR_FAIL;

	HBOOL bSupport	=	FALSE;
		
	hErr = APK_OUTPUT_AUDIO_SupportSpdif (&bSupport);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_AUDIO_SupportSpdif Error : %d", hErr);
	}

	return (bool)bSupport;
}

bool cApkOutputAudio::GetMasterMute()
{
	HERROR hErr = ERR_FAIL;

	HBOOL bMute	=	FALSE;

	hErr = APK_OUTPUT_AUDIO_GetMasterMute (&bMute);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_AUDIO_GetMasterMute Error : %d", hErr);
	}

	return (bool)bMute;
}

void cApkOutputAudio::SetMasterMute(const bool& mute)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_OUTPUT_AUDIO_SetMasterMute ((HBOOL)mute);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_AUDIO_SetMasterMute Error : %d", hErr);
	}

	return ;
}

uint32_t cApkOutputAudio::GetVolumeMax()
{
	HERROR hErr = ERR_FAIL;

	HUINT32 ulMaxVolume	=	0;
		
	hErr = APK_OUTPUT_AUDIO_GetVolumeMax (&ulMaxVolume);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_AUDIO_GetVolumeMax Error : %d", hErr);
	}

	return (uint32_t)ulMaxVolume;
}

uint32_t cApkOutputAudio::GetVolume()
{
	HERROR hErr = ERR_FAIL;

	HUINT32 ulVolume	=	0;
		
	hErr = APK_OUTPUT_AUDIO_GetVolume (&ulVolume);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_AUDIO_GetVolume Error : %d", hErr);
	}

	return (uint32_t)ulVolume;
}

void cApkOutputAudio::SetVolume(const uint32_t& volume)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_OUTPUT_AUDIO_SetVolume ((HUINT32)volume);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_AUDIO_SetVolume Error : %d", hErr);
	}

	return ;
}

uint32_t cApkOutputAudio::GetLipSyncDelay()
{
	HERROR hErr = ERR_FAIL;

	HUINT32 ulmsec	=	0;
		
	hErr = APK_OUTPUT_AUDIO_GetLipSyncDelay (&ulmsec);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_AUDIO_GetLipSyncDelay Error : %d", hErr);
	}

	return (uint32_t)ulmsec;
}

void cApkOutputAudio::SetLipSyncDelay(const uint32_t& mesec)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_OUTPUT_AUDIO_SetLipSyncDelay ((HUINT32)mesec);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_AUDIO_SetLipSyncDelay Error : %d", hErr);
	}

	return ;
}

void cApkOutputAudio::SetHdmiFormat(const uint32_t& digitalAudioFormat)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_OUTPUT_AUDIO_SetHdmiFormat ((DxDigitalAudioFormat_e)digitalAudioFormat);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_AUDIO_SetHdmiFormat Error : %d", hErr);
	}

	return ;
}

uint32_t cApkOutputAudio::GetHdmiFormat()
{
	HERROR hErr = ERR_FAIL;

	DxDigitalAudioFormat_e eDigitalAudioFormat = eDxDIGITALAUDIO_FORMAT_UNKNOWN;
		
	hErr = APK_OUTPUT_AUDIO_GetHdmiFormat (&eDigitalAudioFormat);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_AUDIO_GetHdmiFormat Error : %d", hErr);
	}

	return (uint32_t)eDigitalAudioFormat;
}

void cApkOutputAudio::SetSpdifFormat(const uint32_t& digitalSpdifFormat)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_OUTPUT_AUDIO_SetSpdifFormat ((DxDigitalAudioFormat_e)digitalSpdifFormat);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_AUDIO_SetSpdifFormat Error : %d", hErr);
	}

	return ;
}

uint32_t cApkOutputAudio::GetSpdifFormat()
{
	HERROR hErr = ERR_FAIL;

	DxDigitalAudioFormat_e eDigitalAudioFormat	=	eDxDIGITALAUDIO_FORMAT_UNKNOWN;
		
	hErr = APK_OUTPUT_AUDIO_GetSpdifFormat (&eDigitalAudioFormat);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_AUDIO_GetSpdifFormat Error : %d", hErr);
	}

	return (uint32_t)eDigitalAudioFormat;
}

bool cApkOutputAudio::GetAudioDescription()
{
	HERROR hErr = ERR_FAIL;

	HBOOL bEnable	=	FALSE;
		
	hErr = APK_OUTPUT_AUDIO_GetAudioDescription (&bEnable);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_AUDIO_GetAudioDescription Error : %d", hErr);
	}

	return (bool)bEnable;
}

void cApkOutputAudio::SetAudioDescription(const bool& enable)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_OUTPUT_AUDIO_SetAudioDescription ((HBOOL)enable);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_AUDIO_SetAudioDescription Error : %d", hErr);
	}

	return ;
}

void cApkOutputAudio::SetStereoSelect(const uint32_t& stereoSelect)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_OUTPUT_AUDIO_SetStereoSelect ((DxStereoSelect_e)stereoSelect);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_AUDIO_SetStereoSelect Error : %d", hErr);
	}

	return ;
}

uint32_t cApkOutputAudio::GetStereoSelect()
{
	HERROR hErr = ERR_FAIL;

	DxStereoSelect_e eStereoSelect = eDxSTEREO_SELECT_UNKNOWN;
		
	hErr = APK_OUTPUT_AUDIO_GetStereoSelect (&eStereoSelect);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_AUDIO_GetStereoSelect Error : %d", hErr);
	}

	return (uint32_t)eStereoSelect;
}

void cApkOutputAudio::SetDualMonoSelect(const uint32_t& dualMonoSelect)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_OUTPUT_AUDIO_SetDualMonoSelect ((DxDualMonoSelect_e)dualMonoSelect);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_AUDIO_SetDualMonoSelect Error : %d", hErr);
	}

	return ;
}

uint32_t cApkOutputAudio::GetDualMonoSelect()
{
	HERROR hErr = ERR_FAIL;

	DxDualMonoSelect_e eDualMonoSelect	=	eDxDUALMONO_SELECT_UNKNOWN;
		
	hErr = APK_OUTPUT_AUDIO_GetDualMonoSelect (&eDualMonoSelect);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_AUDIO_GetDualMonoSelect Error : %d", hErr);
	}

	return (uint32_t)eDualMonoSelect;
}
