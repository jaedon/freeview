#include <apk.h>


#include <vkernel.h>

#include <dbus-c++/dbus.h>
#include <as_apk_output_panel.h>

char const * const cApkOutputPanel::BUSNAME_APK_OUTPUT_PANEL = "Octopus.Appkit.Output.Panel";
char const * const cApkOutputPanel::BUSPATH_APK_OUTPUT_PANEL = "/Octopus/Appkit/Output/Panel";

cApkOutputPanel::cApkOutputPanel(DBus::Connection &connection)
  : DBus::ObjectAdaptor(connection, BUSPATH_APK_OUTPUT_PANEL)
{
	/* apkit output panel */
    {	
		HERROR hErr = ERR_FAIL;
		hErr = APK_OUTPUT_PANEL_Init ();
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("APK_OUTPUT_PANEL_Init Error : %d", hErr);
		}

		//HERROR APK_OUTPUT_PANEL_DeInit(void);
    }
}

void cApkOutputPanel::SetAppPriority(const int32_t& priority)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_OUTPUT_PANEL_SetAppPriority ((HINT32)priority);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_PANEL_SetAppPriority Error : %d", hErr);
	}

	return ;
}

void cApkOutputPanel::SetLed(const uint32_t& ledId, const uint32_t& ledStatus)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_OUTPUT_PANEL_SetLed ((DxLedId_e)ledId, (DxLedStatus_e)ledStatus);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_PANEL_SetLed Error : %d", hErr);
	}
	
	return ;
}

void cApkOutputPanel::SetLedAuto()
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_OUTPUT_PANEL_SetLedAuto ();
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_PANEL_SetLedAuto Error : %d", hErr);
	}

	return ;
}

void cApkOutputPanel::SetLedCategory(const uint32_t& category)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_OUTPUT_PANEL_SetLedCategory ((DxPanelCategory_e)category);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_PANEL_SetLedCategory Error : %d", hErr);
	}

	return ;
}

void cApkOutputPanel::ClearLed()
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_OUTPUT_PANEL_ClearLed ();
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_PANEL_ClearLed Error : %d", hErr);
	}

	return ;
}

void cApkOutputPanel::SetPanel(const int32_t& priority, const std::string& text, const uint32_t& panelAlign)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_OUTPUT_PANEL_SetPanel ((HINT32)priority, (const HCHAR *)&text[0], (DxPanelAlign_e)panelAlign);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_PANEL_SetPanel Error : %d", hErr);
	}

	return ;
}

void cApkOutputPanel::SetPanelAuto(const int32_t& priority)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_OUTPUT_PANEL_SetPanelAuto ((HINT32)priority);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_PANEL_SetPanelAuto Error : %d", hErr);
	}

	return ;
}

void cApkOutputPanel::SetPanelDimLevel(const int32_t& priority, const int32_t& dimLevel)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_OUTPUT_PANEL_SetPanelDimLevel ((HINT32)priority, (HINT32)dimLevel);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_PANEL_SetPanelDimLevel Error : %d", hErr);
	}

	return ;
}

void cApkOutputPanel::SetPanelAnimation(const int32_t& priority)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_OUTPUT_PANEL_SetPanelAnimation ((HINT32)priority);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_OUTPUT_PANEL_SetPanelAnimation Error : %d", hErr);
	}

	return ;
}

