#include <signal.h>

#include <hlib.h>
#include <octo_common.h>

#include <linkedlist.h>
#include <otl.h>

#include <db_common.h>
#include <db_svc.h>
#include <db_param.h>

#include <svc_cas.h>
#include <linkedlist.h>
#include <treeutil.h>
#include <util_task.h>
#include <pal_power.h>
#include <pal_pipe.h>
#include <pal_key.h>
#include <pal_hdmi.h>
#include <pal_video.h>
#include <pal_audio.h>
#include <pal_sef.h>
#include <pal_channel.h>
#include <pal_pes.h>
#include <pal_dsc.h>
#include <pal_sc.h>
#ifdef CONFIG_MW_MM_MEDIA
//#include <pal_media20.h>
#endif
#include <pal_demux.h>
#include <pal_sirawpool.h>
#include <pal_scart.h>
#include <pal_scaler.h>
#include <pal_output.h>
#if defined(CONFIG_MW_MM_PVR)
#include <pal_rec.h>
#include <pal_pb.h>
#endif

#if defined(CONFIG_MW_CI)
#include <pal_ci.h>
#endif
#include <pal_ringbuf.h>
#if defined(CONFIG_MW_MM_TRANSCODING)
#include <pal_transcoder.h>
#include <svc_transcoder.h>
#endif

#include <svc_sys.h>
#include <svc_pipe.h>
#include <svc_av.h>
#include <svc_resources.h>
#include <svc_ch.h>
#include <svc_epg.h>

#ifdef CONFIG_PROD_DEVICE_FILE_SYSTEM
#include <svc_fs.h>
#endif
#ifdef CONFIG_MW_MULTIMEDIA
#include <svc_rec.h>
#include <svc_pb.h>
#include <svc_meta.h>
#endif
#include <svc_si.h>
#ifdef CONFIG_MW_DOWNLOAD
#include <svc_download.h>
#endif
#if defined(CONFIG_MW_TELETEXT)
#include <svc_ttx.h>
#endif
#if defined(CONFIG_MW_SUBTITLE)
#include <svc_sbtl.h>
#endif
#if defined(CONFIG_MW_COMM)
#include <svc_comm.h>
#endif
#ifdef CONFIG_MW_MM_MEDIA
#if defined(CONFIG_SUPPORT_IMMA)
#include <svc_mplayercso.h>
#else
#include <svc_mplayer.h>
#endif
#endif
#include <svc_output.h>
#include <mwc_codepage.h>

#include <pal_datastream.h>

#ifdef CONFIG_MW_CI_PLUS
//#include <ci_plus.h>     	// for CI_MES_SetLicenseDownCbRegState()
#endif // CONFIG_MW_CI_PLUS

#if defined (CONFIG_MW_CEC)
#include <svc_cec.h>
#endif

#include <bus.h>
#include <mgr_common.h>

#include <svc_swup.h>

#if defined(CONFIG_MW_LOGCAPTURE)
#include <svc_logcapture.h>
#endif

#if defined(CONFIG_MW_CEC)
#include <pal_cec.h>
#endif

#if defined(CONFIG_DEBUG)
#include <cmdtool.h>
#endif
#if defined(CONFIG_MW_CAS_IRDETO_LOADER_DVT) && !defined(CONFIG_MW_CAS_IRDETO_LOADER_DVT_V5)
#include <ldvt.h>
#endif

#include <oapi_manager.h>

#define DAPI_SETTIMEOUT
// #define DAPI_ASYNCSET

/// Belows are temporary..
#if 1
void	OCTO_WEBSOCKET_Init(void);
void	OxAPI_MGR_Init(void);

extern BUS_Result_t		PROC_Init(HINT32 message, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3);
#endif

#ifdef	CONFIG_DEBUG
extern void CMD_OBAMA_Init(void);
#endif


#define ___INTERNAL_FUNCTION______________________

STATIC HINT32 obama_DbNotifyMessage(HINT32 nMsg, Handle_t hAct, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	BUS_Result_t	hRes = BUS_PostMessage(NULL, nMsg, hAct, nParam1, nParam2, nParam3);

	return (hRes != ERR_BUS_NO_ERROR) ? ERR_FAIL : ERR_OK;
}

STATIC HINT32 obama_NotifyMessage(HINT32 nMsg, Handle_t hAct, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	BUS_Result_t	hRes = BUS_PostMessage(NULL, nMsg, hAct, nParam1, nParam2, nParam3);

	return (hRes != ERR_BUS_NO_ERROR) ? ERR_FAIL : ERR_OK;
}

STATIC HINT32 obama_NotifyData(HINT32 nMsg, Handle_t hAct, HINT32 nParam1, HINT32 nParam2, void *pvData, HUINT32 ulSize)
{
	BUS_Result_t	hRes = BUS_PostData(NULL, nMsg, hAct, nParam1, nParam2, pvData, ulSize);

	return (hRes != ERR_BUS_NO_ERROR) ? ERR_FAIL : ERR_OK;
}

STATIC void obama_Init_HDD_FORMAT_JIGSW(void)
{
#ifdef CONFIG_MW_MM_HDD_FORMAT_JIGSW
	// PAL
	PAL_PWR_Init();
	PAL_KEY_Init();

#if defined(CONFIG_MW_OUTPUT_HDMI)
	PAL_HDMI_Init();
#endif

#if defined(CONFIG_MW_CEC)
	PAL_CEC_Init();
#endif
	PAL_PIPE_Init();
	PAL_VIDEO_Init();
	PAL_AUDIO_Init();
	PAL_SEF_Init();
	PAL_CH_Init();
	PAL_PES_Init();
	PAL_DMX_Init();
	PAL_SIRAWPOOL_Init();

#if (defined(CONFIG_MW_CAS) || defined(CONFIG_MW_CI_PLUS))
	PAL_DSC_Init();
#endif
	PAL_SCALER_Init();

#if defined(CONFIG_MW_CAS) && (CONFIG_MW_CAS_NUM_SC_SLOT > 0)
	PAL_SC_Init();
#endif

#if defined(CONFIG_MW_CI)
	PAL_CI_Init();
#endif

	PAL_SCART_Init();
	PAL_RINGBUF_Init();
	PAL_OUT_Init();

#if defined(CONFIG_MW_MM_TRANSCODING)
	PAL_TRANSC_Init();
#endif

#if defined(CONFIG_MW_CAS_IRDETO_LOADER_DVT)
	SVC_SYS_Init(obama_NotifyMessage);
	SVC_OUT_Init(obama_NotifyMessage);
	SVC_SYS_DisplayPanelString(2, "DVT Test Appl", 1);
	LDVT_InitAppl();
	return;
#endif

	// HxDateTime Init
	HLIB_DATETIME_Init();

#if defined(CONFIG_MW_MM_PVR)
	PAL_REC_Init();
	PAL_PB_Init();
#endif

/*****************
*	SVC_XXX_Init
******************/

	SVC_SYS_Init(obama_NotifyMessage);


#if defined(CONFIG_PROD_DEVICE_FILE_SYSTEM)
	SVC_FS_Init();
#endif

#if defined(CONFIG_MW_MM_PVR)
	SVC_REC_Init(obama_NotifyMessage);
	SVC_PB_Init( obama_NotifyMessage);
#endif

#endif
}

STATIC void obama_Init_Normal(void)
{
/////////////////////////////////////////////////////////
	UTIL_LINKEDLIST_InitItemPool();
	UTIL_TREEUTIL_InitItemPool();
	UTIL_TASK_Init();
	// This is only for OCTO server mode.
	// Because application's main will be call this function.

	HLIB_LOG_Init(TRUE);
	HLIB_CP_Init();

	// HxLib Init
	HLIB_JSON_Init (eHLIB_JSON_Conf_Minimum, NULL, NULL);

	// Set DAPI sendValue:
#if defined(DAPI_SETTIMEOUT)
	(void)DAPI_Option(eDAPI_OPTION_SndBufSize, (512 * 1000), NULL);
	(void)DAPI_Option(eDAPI_OPTION_SndTimeout, (4 * 1000 * 1000), NULL);
#endif
#if defined(DAPI_ASYNCSET)
	(void)DAPI_Option(eDAPI_OPTION_AsyncSend, 256, NULL);
#endif

	OTL_Init();

	// flash interface를 사용하는 경우, RAMDISK를 이용하여, 로딩하기 때문에, DB보다 먼저 초기화되어야 한다.
	// file system을 사용하는 경우 DB Init 이후에, 초기화 되어도 상관없음.
	OTL_RAMDISK_Init();
	PAL_DSTREAM_Init();

	BUS_Init();

	DB_Init(obama_DbNotifyMessage);
/////////////////////////////////////////////////////////

	// PAL
	PAL_PWR_Init();
	PAL_KEY_Init();

#if defined(CONFIG_MW_OUTPUT_HDMI)
	PAL_HDMI_Init();
#endif

#if defined(CONFIG_MW_CEC)
	PAL_CEC_Init();
#endif
	PAL_PIPE_Init();
	PAL_VIDEO_Init();
	PAL_AUDIO_Init();
	PAL_SEF_Init();
	PAL_CH_Init();
	PAL_PES_Init();
	PAL_DMX_Init();
	PAL_SIRAWPOOL_Init();

#if (defined(CONFIG_MW_CAS) || defined(CONFIG_MW_CI_PLUS))
	PAL_DSC_Init();
#endif
	PAL_SCALER_Init();

#if defined(CONFIG_MW_CAS) && (CONFIG_MW_CAS_NUM_SC_SLOT > 0)
	PAL_SC_Init();
#endif

#if defined(CONFIG_MW_CI)
	PAL_CI_Init();
#endif

	PAL_SCART_Init();
	PAL_RINGBUF_Init();
	PAL_OUT_Init();

#if defined(CONFIG_MW_MM_TRANSCODING)
	PAL_TRANSC_Init();
#endif

#if defined(CONFIG_MW_CAS_IRDETO_LOADER_DVT)
	SVC_SYS_Init(obama_NotifyMessage);
	SVC_OUT_Init(obama_NotifyMessage);
	SVC_SYS_DisplayPanelString(2, "DVT Test Appl", 1);
	LDVT_InitAppl();
	return;
#endif

	// HxDateTime Init
	HLIB_DATETIME_Init();

#if defined(CONFIG_MW_MM_PVR)
	PAL_REC_Init();
	PAL_PB_Init();
#endif

/*****************
*	SVC_XXX_Init
******************/

	SVC_SYS_Init(obama_NotifyMessage);

/////////////////////////////////////////////////////////
	SVC_PIPE_Init();

	SVC_OUT_Init(obama_NotifyMessage);

	SVC_AV_Init(obama_NotifyMessage);

	SVC_CH_Init(obama_NotifyMessage);

	SVC_CAS_Init(obama_NotifyMessage, obama_NotifyData); // CI+ Applicaion MMI 동작으로 인해 MW_ITK_Init() 직전에 호출되는 것이 바람직하다. but...

#if defined(CONFIG_3RD_MHEG) && defined(CONFIG_3RD_MHEG_USE_SNT_ENGINE)
	//PORT_ITK_RegisterNotifyCallback(obama_NotifyMessage, obama_NotifyData);
	//PORT_ITK_Init();
#endif

	SVC_EPG_Init(obama_NotifyMessage);
/////////////////////////////////////////////////////////

#if defined(CONFIG_PROD_DEVICE_FILE_SYSTEM)
	SVC_FS_Init();
#endif

#if defined(CONFIG_MW_MM_PVR)
	SVC_REC_Init(obama_NotifyMessage);
	SVC_PB_Init( obama_NotifyMessage);
#endif

/////////////////////////////////////////////////////////
	SVC_SI_Init((void *)obama_NotifyMessage, obama_NotifyData);

#if defined(CONFIG_MW_DOWNLOAD)
	SVC_DOWNLOAD_Init(obama_NotifyMessage, obama_NotifyData);
#endif

#if defined(CONFIG_MW_SUBTITLE)
	SVC_SBTL_Init(obama_NotifyMessage);
#endif

#if defined(CONFIG_MW_TELETEXT)
	SVC_TTX_Init(obama_NotifyMessage);
#endif

#if defined(CONFIG_MW_MM_TRANSCODING)
	SVC_TRANSCODER_Init(obama_NotifyMessage);
#endif

#if defined(CONFIG_MW_MM_MEDIA)
	#if defined(CONFIG_SUPPORT_IMMA)
		/* Init MW CSO(Calypso) Media */
		SVC_CSO_Init(obama_NotifyMessage);
	#else
		/* Init MW MP(Media 1.0/2.0) Media */
		{
			SvcMp_Config_t config;
			SVC_MP_GetDefaultConfig(&config);
			SVC_MP_Init(&config, obama_NotifyMessage);
		}
	#endif
#endif
/////////////////////////////////////////////////////////
}

STATIC void obama_Init_Common(void)
{
#if defined (CONFIG_MW_CEC)
	{
		HUINT8	szModelName[CEC_MAX_OPERAND_LEN];

		SVC_CEC_Init(obama_NotifyMessage);
		if (ERR_OK == SVC_SYS_GetModelName((HUINT8 *)szModelName, CEC_MAX_OPERAND_LEN, TRUE))
		{
#if defined(CONFIG_PROD_FVP4000T)
{
			HUINT32 ulSystemId = 0;

			if (ERR_OK == SVC_SYS_GetSystemId(&ulSystemId))
			{
				if (0x90107d44 == ulSystemId)	// System ID of FVP-5000T : 9010.7D44
				{
					// To support both FVP-4000T and FVP-5000T
					HxSTD_snprintf(szModelName, CEC_MAX_OPERAND_LEN, "%s", "FVP-5000T");
				}
			}
}
#endif
			SVC_CEC_SetModelName((HCHAR *)szModelName);
		}
	}
#endif

	HLIB_CRYPT_Init();

	/*
		Original BUS_Init() position is here. It is moved in front of DB_Init.
		Because BUS_PostMessage was used before BUS_Init().
		This comments would be removed after more test. 2015-10-23
	*/
	//BUS_Init();

	SVC_SWUPDATE_Init(obama_NotifyMessage, obama_NotifyData);

#if defined(CONFIG_MW_LOGCAPTURE)
	SVC_LogCapture_Init();
#endif

	BUS_Start (PROC_Init);

#if defined(CONFIG_DEBUG)
	CMD_OBAMA_Init(); /* this should be done after VK_Init() because it uses vk */
#endif

#if defined(CONFIG_MW_MULTIMEDIA) && defined(CONFIG_MW_MM_PVR)
	SVC_META_Init(obama_NotifyMessage);
#endif
}

#define ___EXTERNAL_FUNCTION______________________

void OBAMA_Init(void)
{
#if !defined(WIN32)
	//	Ignore BROKEN PIPE signal.
	signal(SIGPIPE, SIG_IGN);
#endif

#ifdef CONFIG_MW_MM_HDD_FORMAT_JIGSW
	obama_Init_HDD_FORMAT_JIGSW();
#else
	obama_Init_Normal();
#endif

	obama_Init_Common();
	/* end of init */
}

/* end of file */
