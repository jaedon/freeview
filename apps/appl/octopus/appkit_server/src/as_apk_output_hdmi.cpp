#include <apk.h>


#include <vkernel.h>

#include <dbus-c++/dbus.h>
#include <as_apk_output_hdmi.h>

char const * const cApkOutputHdmi::BUSNAME_APK_OUTPUT_HDMI = "Octopus.Appkit.Output.Hdmi";
char const * const cApkOutputHdmi::BUSPATH_APK_OUTPUT_HDMI = "/Octopus/Appkit/Output/Hdmi";

cApkOutputHdmi * cApkOutputHdmi::pMainThis	=	NULL;

void cApkOutputHdmi::callback_Event (void)
{
	if (NULL == pMainThis)
		return;

	pMainThis->event ();					
}

cApkOutputHdmi::cApkOutputHdmi(DBus::Connection &connection)
  : DBus::ObjectAdaptor(connection, BUSPATH_APK_OUTPUT_HDMI)
{
	if (NULL == pMainThis)
		pMainThis = this;

	/* apkit output hdmi */
    {	
		HERROR hErr = ERR_FAIL;
		hErr = APK_OUTPUT_HDMI_Init ();
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("APK_OUTPUT_HDMI_Init Error : %d", hErr);
		}

		hErr = APK_OUTPUT_HDMI_RegisterNotifier (cApkOutputHdmi::callback_Event);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("APK_OUTPUT_HDMI_RegisterNotifierError : %d", hErr);
		}

		//HERROR APK_OUTPUT_HDMI_UnregisterNotifier(void);
    }
}

bool cApkOutputHdmi::SupportCec()
{
	HERROR hErr = ERR_FAIL;

	HBOOL bSupport	=	FALSE;
		
	hErr = APK_OUTPUT_HDMI_SupportCec (&bSupport);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_HDMI_SupportCec : %d", hErr);
	}

	return (bool)bSupport;
}

uint32_t cApkOutputHdmi::GetHdcpStatus()
{
	HERROR hErr = ERR_FAIL;
	
	DxHdcpStatus_e eHdcpStatus	=	eDxHDCPSTATUS_OFF;

	hErr = APK_OUTPUT_HDMI_GetHdcpStatus (&eHdcpStatus);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_HDMI_GetHdcpStatus : %d", hErr);
	}

	return (uint32_t)eHdcpStatus;
}

void cApkOutputHdmi::Set3DSelect(const uint32_t& hdmi3DSelect)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_OUTPUT_HDMI_Set3DSelect ((DxHdmi3DSelect_e)hdmi3DSelect);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_HDMI_Set3DSelect : %d", hErr);
	}

	return ;
}

uint32_t cApkOutputHdmi::Get3DSelect()
{
	HERROR hErr = ERR_FAIL;

	DxHdmi3DSelect_e eHdmi3DSelect = eDxHDMI_3DSELECT_OFF;
		
	hErr = APK_OUTPUT_HDMI_Get3DSelect (&eHdmi3DSelect);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_HDMI_Get3DSelect : %d", hErr);
	}

	return (uint32_t)eHdmi3DSelect;
}

uint32_t cApkOutputHdmi::GetEdid3DFormatCapability()
{
	HERROR hErr = ERR_FAIL;

	DxVideo3DFormat_b ul3DFormatCapability	=	eDxVIDEO_3DFORMAT_NONE;
		
	hErr = APK_OUTPUT_HDMI_GetEdid3DFormatCapability (&ul3DFormatCapability);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_HDMI_GetEdid3DFormatCapability : %d", hErr);
	}

	return (uint32_t)ul3DFormatCapability;
}

uint32_t cApkOutputHdmi::Get3DStatus()
{
	HERROR hErr = ERR_FAIL;

	DxVideo3DFormat_e eHdmi3DStatus	=	eDxVIDEO_3DFORMAT_NONE;
		
	hErr = APK_OUTPUT_HDMI_Get3DStatus (&eHdmi3DStatus);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_HDMI_Get3DStatus : %d", hErr);
	}

	return (uint32_t)eHdmi3DStatus;
}

uint32_t cApkOutputHdmi::GetEdidResolutionCapability()
{
	HERROR hErr = ERR_FAIL;

	DxResolutionStatus_b ulResolutionCapability	=	eDxRESOLUTION_STATUS_UNKNOWN;
		
	hErr = APK_OUTPUT_HDMI_GetEdidResolutionCapability (&ulResolutionCapability);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_HDMI_GetEdidResolutionCapability : %d", hErr);
	}

	return (uint32_t)ulResolutionCapability;
}

void cApkOutputHdmi::SetCecEnable(const bool& support)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_OUTPUT_HDMI_SetCecEnable ((HBOOL)support);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_HDMI_SetCecEnable : %d", hErr);
	}

	return ;
}

bool cApkOutputHdmi::GetCecEnable()
{
	HERROR hErr = ERR_FAIL;

	HBOOL bEnable = FALSE;
	
	hErr = APK_OUTPUT_HDMI_GetCecEnable (&bEnable);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_HDMI_GetCecEnable : %d", hErr);
	}

	return (bool)bEnable;
}
