#include <hlib.h>
#include <irldvt_stb_params.h>
#include <pal_output.h>
#include <pal_channel.h>
#include <ldvt.h>
#include <environment.h>


#define PrintDebug(...)

static	void	_pfnChannelCallback(HUINT32 ulTunerGroupId, HUINT32 ulRequestId, PalCh_Event_e event);


void	_initEnvironment(void)
{
	HUINT32		ulWinId;

	PrintDebug("\n============ _initEnvironment 3 =============== \n");
	IRLDVT_OUTPUT_Init();
	PAL_OUT_SetAvEnable(1);

	//	Set Audio output to PCM and Volume 0
	//PAL_OUT_SetDigitalAudioOutCodec(5);

#if defined(CONFIG_APUI_DIGITURK) // //#Bugzilla 2118
    //PAL_OUT_ChangeDigitalAudioDigiturkHDMIOutputMode(3);
#endif
	//PAL_OUT_SetVolume(0);

	//	Audio trans mode to Dolby
	//PAL_OUT_SetHdmiAudioTransCode(eDxAUDIO_CODEC_DOLBY_AC3,eDxAUDIO_CODEC_DOLBY_AC3);
	//PAL_OUT_SetSpdifAudioTransCode(eDxAUDIO_CODEC_DOLBY_AC3,eDxAUDIO_CODEC_DOLBY_AC3);

	//	Macrovision off
	PAL_OUT_SetMacrovision(0);

	//	Copyrights free
	PAL_OUT_SetScmsCopyRight(0);

#if !defined(CONFIG_PROD_IRHD5300C)
	//	HDCP off
	//edited by hmjoo
	PAL_HDMI_SetHdcp(FALSE);
#endif

#if !defined(CONFIG_PROD_IRHD5300C)
//	PAL_SCART_SetOutput(TRUE);
#endif
	//	Open Window :: how can I connect video window
	//  PAL_SCALER_OpenVideoWindow(&ulWinId);
	//  PAL_SCALER_ChageMainVideoWindow(ulWinId);

	//	Set 16:9
	PAL_SCALER_SetTvAspectRatio(eDxASPECTRATIO_16X9,TRUE);

	//	Set Resolution 1080i
	PAL_SCALER_SetResolutionSelect(eDxRESOLUTION_SELECT_1080I,TRUE);

	//	Set Channel Callback
	PAL_CH_RegisterCheckTunerCallback(_pfnChannelCallback);
	PrintDebug("\n============ _initEnvironment 3 END =============== \n");

}

void _pfnChannelCallback(HUINT32 ulTunerGroupId, HUINT32 ulRequestId, PalCh_Event_e event)
{
	//LDVT_PostMessage(LDVT_MSG_CHANNEL, ulTunerGroupId, ulRequestId, event);
}

