/**************************************************************
 *	@file		oapi.h
 *	This module for OCTO 2.0's Open API Interface
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2011/09/07
 *	@author			humax
 *	@brief			OCTO Open API Interface
 **************************************************************/

#ifndef	__OAPI_H__
#define	__OAPI_H__

#ifdef	__cplusplus
extern "C" {
#endif

#include <hlib.h>


// module apis
#include <oapi_platform.h>
#include <oapi_media_play.h>
#include <oapi_media_rec.h>
#include <oapi_builder.h>
#include <oapi_output_video.h>
#include <oapi_output_audio.h>
#include <oapi_output_hdmi.h>
#include <oapi_output_panel.h>
#include <oapi_output_thermal_protection.h>
#include <oapi_sysinfo_tuner.h>
#include <oapi_sysinfo_stb.h>
#include <oapi_meta_service.h>
#include <oapi_meta_antenna.h>
#include <oapi_meta_record.h>
#include <oapi_scenario.h>
#include <oapi_scenario_pincode.h>
#include <oapi_filemgr_sambaclient.h>
#include <oapi_cas_nagra.h>
#include <oapi_cas_irdeto.h>
#include <oapi_cas_verimatrix.h>
#include <oapi_exe.h>
#include <oapi_cas_mmi.h>
#include <oapi_cas_ci.h>
#include <oapi_cas_action.h>
#include <oapi_swupdate.h>
#include <oapi_dsmcc.h>
#include <oapi_drmagent.h>
#include <oapi_rpc.h>
#include <oapi_extdev.h>
#include <oapi_datastreamer.h>
#include <oapi_download.h>
#include <oapi_resource.h>

// add more module here!

#ifdef	__cplusplus
}
#endif

#endif

