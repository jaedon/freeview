#include <stdio.h>
#include <hlib.h>
#include <vkernel.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <limits.h>

#include <dbus-c++/dbus.h>

#include <oapi_media_play.h>
#include <oapi_media_rec.h>

#include <dbus_util_cvt.h>

#include <as_apk_media_airplay.h>
#include <as_apk_util_builder.h>
#include <as_apk_cas_action.h>
#include <as_apk_cas_ci.h>
#include <as_apk_cas_irdeto.h>
#include <as_apk_cas_mmi.h>
#include <as_apk_cas_nagra.h>
#include <as_apk_util_datastreamer.h>
#include <as_apk_util_db.h>
#include <as_apk_dlna_util.h>
#include <as_apk_util_drmagent.h>
#include <as_apk_util_dsmcc.h>
#include <as_apk_util_exe.h>
#include <as_apk_util_extdev.h>
#include <as_apk_filemgr_base.h>
#include <as_apk_filemgr_browse.h>
#include <as_apk_filemgr_group.h>
#include <as_apk_filemgr_ftpserver.h>
#include <as_apk_filemgr_sambaclient.h>
#include <as_apk_filemgr_sambaserver.h>
#include <as_apk_filemgr_storage.h>
#include <as_apk_media_play.h>
#include <as_apk_media_record.h>
#include <as_apk_meta_antenna.h>
#include <as_apk_meta_query.h>
#include <as_apk_meta_record.h>
#include <as_apk_meta_schedule.h>
#include <as_apk_meta_service.h>
#include <as_apk_util_network.h>
#include <as_apk_output_audio.h>
#include <as_apk_output_hdmi.h>
#include <as_apk_output_panel.h>
#include <as_apk_output_video.h>
#include <as_apk_util_power.h>
#include <as_apk_scenario_base.h>
#include <as_apk_scenario_pincode.h>
#include <as_apk_util_swupdate.h>
#include <as_apk_sysinfo_stb.h>
#include <as_apk_sysinfo_tuner.h>


#include <as_apk_module_submodule.h>


DBus::BusDispatcher g_Dispatcher;

void system_signal(int sig)
{
	g_Dispatcher.leave();
}

int main (int argc, char *argv[])
{
	VK_Init();

	/* system */
	{
		signal(SIGTERM, system_signal);
		signal(SIGINT, system_signal);
	}

	/* dbus */
	{
		DBus::default_dispatcher = &g_Dispatcher;
		
		DBus::Connection conn = DBus::Connection::SystemBus();

		/* appkit */
		conn.request_name(cApkMediaAirplay::BUSNAME_APK_MEDIA_AIRPLAY);
		conn.request_name(cApkUtilBuilder::BUSNAME_APK_UTIL_BUILDER);
		conn.request_name(cApkCasAction::BUSNAME_APK_CAS_ACTION);
		conn.request_name(cApkCasCi::BUSNAME_APK_CAS_CI);
		conn.request_name(cApkCasIrdeto::BUSNAME_APK_CAS_IRDETO);
		conn.request_name(cApkCasMmi::BUSNAME_APK_CAS_MMI);
		conn.request_name(cApkCasNagra::BUSNAME_APK_CAS_NAGRA);
		conn.request_name(cApkUtilDatastreamer::BUSNAME_APK_UTIL_DATASTREAMER);
		conn.request_name(cApkUtilDb::BUSNAME_APK_UTIL_DB);
		conn.request_name(cApkDlnaUtil::BUSNAME_APK_DLNA_UTIL);
		conn.request_name(cApkUtilDrmagent::BUSNAME_APK_UTIL_DRMAGENT);
		conn.request_name(cApkUtilDsmcc::BUSNAME_APK_UTIL_DSMCC);
		conn.request_name(cApkUtilExe::BUSNAME_APK_UTIL_EXE);
		conn.request_name(cApkUtilExtdev::BUSNAME_APK_UTIL_EXTDEV);
		conn.request_name(cApkFilemgrBase::BUSNAME_APK_FILEMGR_BASE);
		conn.request_name(cApkFilemgrBrowse::BUSNAME_APK_FILEMGR_BROWSE);
		conn.request_name(cApkFilemgrGroup::BUSNAME_APK_FILEMGR_GROUP);
		conn.request_name(cApkFilemgrFtpserver::BUSNAME_APK_FILEMGR_FTPSERVER);
		conn.request_name(cApkFilemgrSambaclient::BUSNAME_APK_FILEMGR_SAMBACLIENT);
		conn.request_name(cApkFilemgrSambaserver::BUSNAME_APK_FILEMGR_SAMBASERVER);
		conn.request_name(cApkFilemgrStorage::BUSNAME_APK_FILEMGR_STORAGE);
		conn.request_name(cApkMediaPlay::BUSNAME_APK_MEDIA_PLAY);
		conn.request_name(cApkMediaRecord::BUSNAME_APK_MEDIA_RECORD);
		conn.request_name(cApkMetaAntenna::BUSNAME_APK_META_ANTENNA);
		conn.request_name(cApkMetaQuery::BUSNAME_APK_META_QUERY);
		conn.request_name(cApkMetaRecord::BUSNAME_APK_META_RECORD);
		conn.request_name(cApkMetaSchedule::BUSNAME_APK_META_SCHEDULE);
		conn.request_name(cApkMetaService::BUSNAME_APK_META_SERVICE);
		conn.request_name(cApkUtilNetwork::BUSNAME_APK_UTIL_NETWORK);
		conn.request_name(cApkOutputAudio::BUSNAME_APK_OUTPUT_AUDIO);
		conn.request_name(cApkOutputHdmi::BUSNAME_APK_OUTPUT_HDMI);
		conn.request_name(cApkOutputPanel::BUSNAME_APK_OUTPUT_PANEL);
		conn.request_name(cApkOutputVideo::BUSNAME_APK_OUTPUT_VIDEO);
		conn.request_name(cApkUtilPower::BUSNAME_APK_UTIL_POWER);
		conn.request_name(cApkScenarioBase::BUSNAME_APK_SCENARIO_BASE);
		conn.request_name(cApkScenarioPincode::BUSNAME_APK_SCENARIO_PINCODE);
		conn.request_name(cApkUtilSwupdate::BUSNAME_APK_UTIL_SWUPDATE);
		conn.request_name(cApkSysinfoStb::BUSNAME_APK_SYSINFO_STB);
		conn.request_name(cApkSysinfoTuner::BUSNAME_APK_SYSINFO_TUNER);
		
		cApkMediaAirplay	 apkMediaAirplay (conn);	
		cApkUtilBuilder 	 apkUtilBuilder (conn);	
		cApkCasAction		 apkCasAction (conn);	
		cApkCasCi			 apkCasCi (conn);	
		cApkCasIrdeto		 apkCasIrdeto (conn);	
		cApkCasMmi			 apkCasMmi (conn);	
		cApkCasNagra		 apkCasNagra (conn);	
		cApkUtilDatastreamer apkUtilDatastreamer (conn);	
		cApkUtilDb			 apkUtilDb (conn);	
		cApkDlnaUtil		 apkDlnaUtil (conn);	
		cApkUtilDrmagent	 apkUtilDrmagent (conn);	
		cApkUtilDsmcc		 apkUtilDsmcc (conn);	
		cApkUtilExe 		 apkUtilExe (conn);	
		cApkUtilExtdev		 apkUtilExtdev (conn);	
		cApkFilemgrBase 	 apkFilemgrBase (conn);	
		cApkFilemgrBrowse	 apkFilemgrBrowse (conn);	
		cApkFilemgrGroup	 apkFilemgrGroup (conn);	
		cApkFilemgrFtpserver apkFilemgrFtpserver (conn);	
		cApkFilemgrSambaclient apkFilemgrSambaclient (conn);	
		cApkFilemgrSambaserver apkFilemgrSambaserver (conn);	
		cApkFilemgrStorage 	 apkFilemgrStorage (conn);	
		cApkMediaPlay		 apkMediaPlay (conn);	
		cApkMediaRecord 	 apkMediaRecord (conn);	
		cApkMetaAntenna 	 apkMetaAntenna (conn);	
		cApkMetaQuery		 apkMetaQuery (conn);	
		cApkMetaRecord		 apkMetaRecord (conn);	
		cApkMetaSchedule	 apkMetaSchedule (conn);	
		cApkMetaService 	 apkMetaService (conn);	
		cApkUtilNetwork 	 apkUtilNetwork (conn);	
		cApkOutputAudio 	 apkOutputAudio (conn);	
		cApkOutputHdmi		 apkOutputHdmi (conn);	
		cApkOutputPanel 	 apkOutputPanel (conn);	
		cApkOutputVideo 	 apkOutputVideo (conn);	
		cApkUtilPower		 apkUtilPower (conn);	
		cApkScenarioBase	 apkScenarioBase (conn);	
		cApkScenarioPincode  apkScenarioPincode (conn);	
		cApkUtilSwupdate	 apkUtilSwupdate (conn);	
		cApkSysinfoStb		 apkSysinfoStb (conn);
		cApkSysinfoTuner	 apkSysinfoTuner (conn);
			
	
		g_Dispatcher.enter();
	}

	return 0;
}

