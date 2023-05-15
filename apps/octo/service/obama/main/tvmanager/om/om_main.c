
#include	<hlib.h>
#include	<hapi.h>
#include <obama_rpcprotocol.h>

#include	<octo_common.h>
#include	<db_param.h>
#include	<bus.h>
#include	<mgr_common.h>
#include	<mgr_action.h>

#include	"om_platform.h"
#include	"om_meta_record.h"
#include	"om_meta_service.h"
#include	"om_meta_antenna.h"
#include	"om_media_play.h"
#include	"om_media_record.h"
#include	"om_output_video.h"
#include	"om_output_audio.h"
#include	"om_output_hdmi.h"
#include	"om_output_panel.h"
#include	"om_output_thermal_protection.h"
#include	"om_builder.h"
#include	"om_resource.h"
#include	"om_sysinfo_stb.h"
#include	"om_sysinfo_tuner.h"
#include	"om_scenario_pincode.h"
#include	"om_scenario.h"
#include	"om_cas_action.h"
#include	"om_exe.h"
#include	"om_swupdate.h"
#include	"om_dsmcc.h"
#if defined(CONFIG_MW_CAS) || defined(CONFIG_MW_CI)
#include	"om_cas_subcas.h"
#include 	"om_cas_mmi.h"
#include 	"om_cas_action.h"
#endif
#include	"om_drmagent.h"
#include 	"om_extdev.h"
#include 	"om_download.h"


#define	MGR_OAPI_INIT_SUBPROC(proc)	\
	do {\
		BUS_MGR_Create(#proc, APP_ACTION_PRIORITY, proc, 0, 0, 0, 0);\
	} while (0)


STATIC BUS_Result_t	proc_om_main(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	switch (message)
	{
		case	eMEVT_BUS_CREATE:
			OM_PLATFORM_Init();
			OM_META_SVC_Init();
#if defined(CONFIG_MW_MM_PVR)
			OM_META_REC_Init();
#endif
			OM_META_ANT_Init();
#if !defined(CONFIG_TEMP_OS_EMULATOR)
			OM_MEDIA_PLAY_Init();
#endif
#if defined(CONFIG_MW_MM_PVR)
			OM_MEDIA_REC_Init();
#endif
			OM_BUILDER_Init();
			OM_RSC_Init();
			OM_OUTPUT_VIDEO_Init();
			OM_OUTPUT_AUDIO_Init();
			OM_OUTPUT_HDMI_Init();
			OM_OUTPUT_PANEL_Init();
#if defined(CONFIG_MW_SYS_THERMAL_PROTECTION)
			OM_OUTPUT_THERMAL_Init();
#endif
			OM_SYSINFO_TUNER_Init();
			OM_SYSINFO_STB_Init();
			OM_EXE_Init();
			OM_SCENARIO_Init();
			OM_SCENARIO_PINCODE_Init();

#if defined(CONFIG_MW_CAS) || defined(CONFIG_MW_CI)
			OM_CAS_SUBCAS_Init();
			OM_CAS_MMI_Init();
			OM_CAS_ACTION_Init();
#endif

			OM_DRMAGENT_Init();

			OM_SWUPDATE_Init();
			OM_DSMCC_Init();
			OM_EXTDEV_Init();

			OM_DOWNLOAD_Init();
			break;

		case	eMEVT_BUS_DESTROY:
			break;

		default:
			break;
	}
	return MESSAGE_PASS;
}

HERROR OM_Init(void)
{
	BUS_Result_t	eRes;

	eRes = BUS_MGR_Create("PROC_Oapi", APP_ACTION_PRIORITY, proc_om_main, 0, 0, 0, 0);

	return (eRes != ERR_BUS_NO_ERROR) ? ERR_FAIL : ERR_OK;
}

