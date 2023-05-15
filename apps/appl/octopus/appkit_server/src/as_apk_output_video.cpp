#include <apk.h>


#include <vkernel.h>

#include <dbus-c++/dbus.h>
#include <as_apk_output_video.h>

char const * const cApkOutputVideo::BUSNAME_APK_OUTPUT_VIDEO = "Octopus.Appkit.Output.Video";
char const * const cApkOutputVideo::BUSPATH_APK_OUTPUT_VIDEO = "/Octopus/Appkit/Output/Video";

cApkOutputVideo * cApkOutputVideo::pMainThis	=	NULL;

void cApkOutputVideo::callback_Event (void)
{
	if (NULL == pMainThis)
		return;

	pMainThis->event ();					
}

cApkOutputVideo::cApkOutputVideo(DBus::Connection &connection)
  : DBus::ObjectAdaptor(connection, BUSPATH_APK_OUTPUT_VIDEO)
{
	if (NULL == pMainThis)
		pMainThis = this;

	/* apkit output video */
    {	
		HERROR hErr = ERR_FAIL;
		hErr = APK_OUTPUT_VIDEO_Init ();
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("APK_OUTPUT_VIDEO_Init Error : %d", hErr);
		}
				
		hErr = APK_OUTPUT_VIDEO_RegisterNotifier (cApkOutputVideo::callback_Event);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("APK_OUTPUT_VIDEO_RegisterNotifier Error : %d", hErr);
		}

		//HERROR APK_OUTPUT_VIDEO_UnregisterNotifier(void);
    }
}

bool cApkOutputVideo::SupportComposite()
{
	HERROR hErr = ERR_FAIL;

	HBOOL bSupport	=	FALSE;
		
	hErr = APK_OUTPUT_VIDEO_SupportComposite (&bSupport);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_VIDEO_SupportComposite Error : %d", hErr);
	}

	return (bool)bSupport;
}

bool cApkOutputVideo::SupportTvScart()
{
	HERROR hErr = ERR_FAIL;

	HBOOL bSupport	=	FALSE;

	hErr = APK_OUTPUT_VIDEO_SupportTvScart (&bSupport);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_VIDEO_SupportTvScart Error : %d", hErr);
	}

	return (bool)bSupport;
}

bool cApkOutputVideo::SupportVcrScart()
{
	HERROR hErr = ERR_FAIL;

	HBOOL bSupport	=	FALSE;

	hErr = APK_OUTPUT_VIDEO_SupportVcrScart (&bSupport);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_VIDEO_SupportVcrScart Error : %d", hErr);
	}

	return (bool)bSupport;
}

uint32_t cApkOutputVideo::GetSupportTvScartFormat()
{
	HERROR hErr = ERR_FAIL;

	DxScartFormat_b ulScartFormat	=	eDxSCART_FORMAT_UNKNOWN;
	
	hErr = APK_OUTPUT_VIDEO_GetSupportTvScartFormat (&ulScartFormat);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_VIDEO_GetSupportTvScartFormat Error : %d", hErr);
	}
	
	return (uint32_t)ulScartFormat;
}

uint32_t cApkOutputVideo::GetSupportVcrScartFormat()
{
	HERROR hErr = ERR_FAIL;

	DxScartFormat_b ulScartFormat	=	eDxSCART_FORMAT_UNKNOWN;

	hErr = APK_OUTPUT_VIDEO_GetSupportVcrScartFormat (&ulScartFormat);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_VIDEO_GetSupportVcrScartFormat Error : %d", hErr);
	}

	return (uint32_t)ulScartFormat;
}

void cApkOutputVideo::SetTvScartFormat(const uint32_t& scartFormat)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_OUTPUT_VIDEO_SetTvScartFormat((DxScartFormat_e)scartFormat);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_VIDEO_SetTvScartFormat Error : %d", hErr);
	}

	return ;
}

uint32_t cApkOutputVideo::GetTvScartFormat()
{
	HERROR hErr = ERR_FAIL;

	DxScartFormat_b ulScartFormat	=	eDxSCART_FORMAT_UNKNOWN;

	hErr = APK_OUTPUT_VIDEO_GetTvScartFormat (&ulScartFormat);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_VIDEO_GetTvScartFormat Error : %d", hErr);
	}

	return (uint32_t)ulScartFormat;
}

void cApkOutputVideo::SetVcrScartFormat(const uint32_t& scartFormat)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_OUTPUT_VIDEO_SetVcrScartFormat ((DxScartFormat_e)scartFormat);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_VIDEO_SetVcrScartFormat Error : %d", hErr);
	}

	return ;
}

uint32_t cApkOutputVideo::GetVcrScartFormat()
{
	HERROR hErr = ERR_FAIL;

	DxScartFormat_b ulScartFormat	=	eDxSCART_FORMAT_UNKNOWN;

	hErr = APK_OUTPUT_VIDEO_GetVcrScartFormat (&ulScartFormat);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_VIDEO_GetVcrScartFormat Error : %d", hErr);
	}

	return (uint32_t)ulScartFormat;
}

void cApkOutputVideo::SetScartPath(const uint32_t& scartPath)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_OUTPUT_VIDEO_SetScartPath ((DxScartPath_e)scartPath);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_VIDEO_SetScartPath Error : %d", hErr);
	}

	return ;
}

uint32_t cApkOutputVideo::GetScartPath()
{
	HERROR hErr = ERR_FAIL;

	DxScartPath_e eScartPath	=	eDxSCART_PATH_UNKNOWN;
	
	hErr = APK_OUTPUT_VIDEO_GetScartPath (&eScartPath);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_VIDEO_GetScartPath Error : %d", hErr);
	}

	return (uint32_t)eScartPath;
}

void cApkOutputVideo::SetResolutionSelect(const uint32_t& resolutionSelect)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_OUTPUT_VIDEO_SetResolutionSelect ((DxResolutionSelect_e)resolutionSelect);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_VIDEO_SetResolutionSelect Error : %d", hErr);
	}

	return ;
}

uint32_t cApkOutputVideo::GetResolutionSelect()
{
	HERROR hErr = ERR_FAIL;

	DxResolutionSelect_e eResolutionSelect	=	eDxRESOLUTION_SELECT_UNKNOWN;
	
	hErr = APK_OUTPUT_VIDEO_GetResolutionSelect (&eResolutionSelect);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_VIDEO_GetResolutionSelect Error : %d", hErr);
	}

	return (uint32_t)eResolutionSelect;
}

uint32_t cApkOutputVideo::GetResolutionStatus()
{
	HERROR hErr = ERR_FAIL;

	DxResolutionStatus_e eResolutionStatus	=	eDxRESOLUTION_STATUS_UNKNOWN;
		
	hErr = APK_OUTPUT_VIDEO_GetResolutionStatus (&eResolutionStatus);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_VIDEO_GetResolutionStatus Error : %d", hErr);
	}

	return (uint32_t)eResolutionStatus;
}

void cApkOutputVideo::SetTvAspectRatio(const uint32_t& tvAspectRatio)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_OUTPUT_VIDEO_SetTvAspectRatio ((DxAspectRatio_e)tvAspectRatio);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_VIDEO_SetTvAspectRatio Error : %d", hErr);
	}

	return ;
}

uint32_t cApkOutputVideo::GetTvAspectRatio()
{
	HERROR hErr = ERR_FAIL;

	DxAspectRatio_e eTvAspectRatio = eDxASPECTRATIO_UNKNOWN;
	
	hErr = APK_OUTPUT_VIDEO_GetTvAspectRatio (&eTvAspectRatio);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_VIDEO_GetTvAspectRatio Error : %d", hErr);
	}

	return (uint32_t)eTvAspectRatio;
}

uint32_t cApkOutputVideo::GetPictureRate()
{
	HERROR hErr = ERR_FAIL;

	DxPictureRate_e ePictureRate	=	eDxPICTURERATE_UNKNOWN;
	
	hErr = APK_OUTPUT_VIDEO_GetPictureRate (&ePictureRate);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_VIDEO_GetPictureRate Error : %d", hErr);
	}

	return (uint32_t)ePictureRate;
}

void cApkOutputVideo::SetDfcSelect(const uint32_t& dfcSelect)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_OUTPUT_VIDEO_SetDfcSelect ((DxDfcSelect_e)dfcSelect);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_VIDEO_SetDfcSelect Error : %d", hErr);
	}

	return ;
}

uint32_t cApkOutputVideo::GetDfcSelect()
{
	HERROR hErr = ERR_FAIL;

	DxDfcSelect_e eDfcSelect	=	eDxDFC_SELECT_UNKNOWN;
		
	hErr = APK_OUTPUT_VIDEO_GetDfcSelect (&eDfcSelect);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_VIDEO_GetDfcSelect Error : %d", hErr);
	}

	return (uint32_t)eDfcSelect;
}

uint32_t cApkOutputVideo::GetDfcStatus()
{
	HERROR hErr = ERR_FAIL;

	DxDfcStatus_b ulDfcStatus	=	eDxDFC_STATUS_FULL;
		
	hErr = APK_OUTPUT_VIDEO_GetDfcStatus (&ulDfcStatus);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_VIDEO_GetDfcStatus Error : %d", hErr);
	}

	return (uint32_t)ulDfcStatus;
}

void cApkOutputVideo::SetBackGroundColor(const uint8_t& red, const uint8_t& green, const uint8_t& blue)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_OUTPUT_VIDEO_SetBackGroundColor ((HUINT8)red, (HUINT8)green, (HUINT8)blue);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_VIDEO_SetBackGroundColor Error : %d", hErr);
	}

	return ;
}

::DBus::Struct< uint8_t, uint8_t, uint8_t > cApkOutputVideo::GetBackGroundColor()
{
	::DBus::Struct< uint8_t, uint8_t, uint8_t > reval;

	HERROR hErr = ERR_FAIL;
	HUINT8 ucRed, ucGreen, ucBlue;
		
	hErr = APK_OUTPUT_VIDEO_GetBackGroundColor (&ucRed, &ucGreen, &ucBlue);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_VIDEO_GetBackGroundColor Error : %d", hErr);
	}

	{
		reval._1	=	ucRed;
		reval._2	=	ucGreen;
		reval._3	=	ucBlue;
	}
	
	return reval;
}

void cApkOutputVideo::SetAvEnable(const bool& enable)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_OUTPUT_VIDEO_SetAvEnable ((HBOOL)enable);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_VIDEO_SetAvEnable Error : %d", hErr);
	}

	return ;
}

bool cApkOutputVideo::GetAvEnable()
{
	HERROR hErr = ERR_FAIL;

	HBOOL bEnable	=	FALSE;
	
	hErr = APK_OUTPUT_VIDEO_GetAvEnable (&bEnable);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_VIDEO_GetAvEnable Error : %d", hErr);
	}

	return (bool)bEnable;
}

uint32_t cApkOutputVideo::GetCompositeStandard()
{
	HERROR hErr = ERR_FAIL;

	DxCompositeStandard_e eCompositeStandard	=	eDxCOMPOSITE_STANDARD_UNKNOWN;
	
	hErr = APK_OUTPUT_VIDEO_GetCompositeStandard (&eCompositeStandard);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_VIDEO_GetCompositeStandard Error : %d", hErr);
	}

	return (uint32_t)eCompositeStandard;
}
