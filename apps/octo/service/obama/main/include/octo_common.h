/****************************************************************************
*
* File Name	: octo_common.h
*
* Description	: system wide definition
*
*
* Revision History	:
*
* Date			Modification				Name
* -----------	-------------------------------------	------------------
* 2007/12/20		initial revision				JK Baek
* 2008/01/09		revised for octo			JK Baek
* 2008/02/05		delete primitive types		JK Baek
*
****************************************************************************/

#ifndef _OCTOCOMMON_H_
#define _OCTOCOMMON_H_

/** Core Arcitecture Team comment (Recommendation)

	Octo에서는 AP, MW에서는 octo_common.h만 include하면 기본 primitive는 전부 포함하도록 하는 것으로 하기 위함.
	Module을 이용할 경우 Module의 primitive를 추가하여 include 하며, local header는 다른 module에서 include할 수 없다.
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include <htype.h>
#include <vkernel.h>
#include <hlib.h>
#include <obama_memconfig.h>
#include <octo_config.h>
#include <stb_params.h>
#include <key_params.h>

// 일본향의 jlabs test 에서만 적용할 내용을 위한 config
// #define CONFIG_JLABS_TEST

#ifndef __func__
#define  __func__ 	__FUNCTION__
#endif

/* 각 모듈별로 디버그 on/off 용 define을 여기다 해주세요. */
/* define this(CONFIG_DEBUG) on configuration */

#define OCTO_NULL_PRINT		while (0) ((int (*)(char *, ...)) 0)
#define	OCTO_SHELL_NAME			(HINT8*)"OCTO_CMD" // defined in ox_protoco.h

#if defined(CONFIG_DEBUG)
/* UNIXTIME으로 나온 시간을 그때 그때 debug로 찍을 수 있음 */
#define		PRINT_DATETIME(mod, x)	\
{	\
	HxDATETIME_t	stDT;	\
	HLIB_DATETIME_ConvertUnixTimeToDateTime(x, &stDT);	\
	fprintf(stderr, "PRINT_DATETIME %s:", (mod));	\
	fprintf(stderr, " (%d_%d.%d, %02d:%02d.%02d \n",	\
				stDT.stDate.usYear, stDT.stDate.ucMonth, stDT.stDate.ucDay,		\
				stDT.stTime.ucHour, stDT.stTime.ucMinute, stDT.stTime.ucSecond);	\
}

#define		PRINT_STR(mod, x)	\
{	\
	int i = 0;	\
	fprintf(stderr, "PRINT_STR %s [%s]: ", (mod), x);	\
	for(i = 0; i<strlen(x); i++)	\
	{	\
		fprintf(stderr, "%02x.", x[i]);	\
	}	\
	fprintf(stderr, " [");	\
	for(i = 0; i<strlen(x); i++)	\
	{	\
		fprintf(stderr, "%c.", x[i]);	\
	}	\
	fprintf(stderr, "]\n");	\
}
#else
#define		PRINT_DATETIME(mod, x)
#define		PRINT_STR(mod, x)
#endif


#define OBAMA_EVENT_MANAGER_START 			(0x10000000)
#define OBAMA_EVENT_SERVICE_START 			(0x20000000)
#define OBAMA_EVENT_OM_START 				(0x30000000)
#define OBAMA_EVENT_EVNETEND 				(0xF0000000) // 1~F

#define eMEVT_BUS_START			(OBAMA_EVENT_MANAGER_START | 0x00100000)
#define eMEVT_MGR_START			(OBAMA_EVENT_MANAGER_START | 0x00200000)
#define eMEVT_XMGR_START			(OBAMA_EVENT_MANAGER_START | 0x00300000)
#define eMEVT_MANAGER_END 			(OBAMA_EVENT_MANAGER_START | 0x0FF00000) // 101~1FF


// svc_common.h 를 생성하고 옮긴다.
#define eSEVT_CH_START				(OBAMA_EVENT_SERVICE_START | 0x00100000)
#define eSEVT_SI_START				(OBAMA_EVENT_SERVICE_START | 0x00200000)
#define eSEVT_AV_START				(OBAMA_EVENT_SERVICE_START | 0x00300000)
#define eSEVT_CAS_START				(OBAMA_EVENT_SERVICE_START | 0x00400000)
#define eSEVT_SBTL_START				(OBAMA_EVENT_SERVICE_START | 0x00500000)
#define eSEVT_TTX_START				(OBAMA_EVENT_SERVICE_START | 0x00600000)
#define eSEVT_DB_START				(OBAMA_EVENT_SERVICE_START | 0x00700000)
#define eSEVT_DOWN_START			(OBAMA_EVENT_SERVICE_START | 0x00800000)
#define eSEVT_OUT_START				(OBAMA_EVENT_SERVICE_START | 0x00900000)
#define eSEVT_SWUP_START			(OBAMA_EVENT_SERVICE_START | 0x00A00000)
#define eSEVT_EPG_START				(OBAMA_EVENT_SERVICE_START | 0x00B00000)
#define eSEVT_M_SAMBACLIENT_START	(OBAMA_EVENT_SERVICE_START | 0x00C00000)
#define eSEVT_MEDIA_START			(OBAMA_EVENT_SERVICE_START | 0x00D00000)
#define eSEVT_CEC_START				(OBAMA_EVENT_SERVICE_START | 0x00E00000)
#define eSEVT_LOGCAPTURE_START		(OBAMA_EVENT_SERVICE_START | 0x00F00000)
#define eSEVT_TRANSCODER_START		(OBAMA_EVENT_SERVICE_START | 0x01000000)
#define eSEVT_REC_START				(OBAMA_EVENT_SERVICE_START | 0x01100000)
#define eSEVT_PB_START				(OBAMA_EVENT_SERVICE_START | 0x01200000)
#define eSEVT_META_START				(OBAMA_EVENT_SERVICE_START | 0x01300000)
#define eSEVT_SERVICE_END				(OBAMA_EVENT_SERVICE_START | 0x0FF00000) // 201~2FF


#define eOEVT_OM_START					(OBAMA_EVENT_OM_START | 0x00100000)
#define eOEVT_OM_END					(OBAMA_EVENT_OM_START | 0x0FF00000) // 301~3FF




/* Type definition */

#define	SIZE_1K		0x00000400
#define	SIZE_2K		0x00000800
#define	SIZE_4K		0x00001000
#define	SIZE_8K		0x00002000
#define	SIZE_16K 	0x00004000
#define	SIZE_32K 	0x00008000
#define	SIZE_64K	0x00010000
#define	SIZE_128K	0x00020000
#define	SIZE_256K	0x00040000
#define	SIZE_300K	0x0004B000
#define	SIZE_400K	0x00064000
#define	SIZE_512K	0x00080000

/* Task와 task queue size는 여기서 모두 관리한다. 각 task를 생성할때는
이 값을 이용하라. 그래야 중앙 관리 및 조정이 된다. */
/* These are legacy level of VxWoks tasks, so define them again for Linux kernel */
/* Especially that of GWM task should be tunned for instant operation */

/*********************************************************
				OCTO system task priority
**********************************************************/

#define DB_SVC_TASK_PRIORITY				VK_TASK_PRIORITY_MW_REF
#define BACKGROUND_TASK_PRIORITY			VK_TASK_PRIORITY_MW_REF
#define GWM_TASK_PRIORITY					VK_TASK_PRIORITY_MW_REF
#define GWM_TIMERTASK_PRIORITY				VK_TASK_PRIORITY_MW_REF
#define AP_EXPLOREMETALIST_TASK_PRIORITY	VK_TASK_PRIORITY_MW_REF
#define OAPI_CTRL_TASK_PRIORITY			VK_TASK_PRIORITY_MW_REF

#define SVC_CH_TASK_PRIORITY					VK_TASK_PRIORITY_MW_REF
#define MW_SI_TASK_PRIORITY					VK_TASK_PRIORITY_MW_REF
#define MW_CAS_TASK_PRIORITY 				VK_TASK_PRIORITY_MW_REF
#define MW_AV_TASK_PRIORITY 				VK_TASK_PRIORITY_MW_REF
#define MW_AVCTRL_TASK_PRIORITY 			VK_TASK_PRIORITY_MW_REF
#define MW_DMP_TASK_PRIORITY 				VK_TASK_PRIORITY_MW_REF
#define MW_EPG_TASK_PRIORITY				VK_TASK_PRIORITY_MW_REF
#define MW_SBTL_TASK_PRIORITY 				VK_TASK_PRIORITY_MW_REF
#define MW_TLTXT_TASK_PRIORITY              VK_TASK_PRIORITY_MW_REF
#define MW_PVR_DEPOT_TASK_PRIORITY			VK_TASK_PRIORITY_MW_REF
#define MW_PVR_RECORDER_TASK_PRIORITY		VK_TASK_PRIORITY_MW_REF
#define MW_PVR_PLAYER_TASK_PRIORITY			VK_TASK_PRIORITY_MW_REF
#define MW_CAS_CIMGR_TASK_PRIORITY			VK_TASK_PRIORITY_MW_REF
#define MW_MHEG_TASK_PRIORITY				VK_TASK_PRIORITY_MW_REF
#define MW_MHP_TASK_PRIORITY				VK_TASK_PRIORITY_MW_REF
#define MW_MHEG_RCV_TASK_PRIORITY			VK_TASK_PRIORITY_MW_REF
#define MW_MHEG_SI_MONITOR_TASK_PRIORITY		VK_TASK_PRIORITY_MW_REF
#define MW_RSV_TASK_PRIORITY				VK_TASK_PRIORITY_MW_REF
#define MW_DOWN_TASK_PRIORITY				VK_TASK_PRIORITY_MW_REF

#define	MW_SWUP_INSTALL_TASK_PRIORITY		VK_TASK_PRIORITY_MW_REF

#define MW_PANEL_TASK_PRIORITY				VK_TASK_PRIORITY_MW_REF
#if defined(CONFIG_MW_MM_MEDIA_SBTL_FILE)
#define MW_MDSUB_TASK_PRIORITY				VK_TASK_PRIORITY_MW_REF
#endif
#if defined(CONFIG_MW_MM_MEDIA_XSUB)
#define MW_XSUB_TASK_PRIORITY				VK_TASK_PRIORITY_MW_REF
#endif
#if defined(CONFIG_MW_CEC)
#define MW_CEC_TASK_PRIORITY				VK_TASK_PRIORITY_MW_REF
#endif
#if defined(CONFIG_MW_COMM)
#define MW_COMM_TASK_PRIORITY				VK_TASK_PRIORITY_MW_REF
#endif
#define MW_OUT_TASK_PRIORITY 				VK_TASK_PRIORITY_MW_REF

#define MW_BROWSER_TASK_PRIORITY			VK_TASK_PRIORITY_MW_REF /**< for bml by dhkim2 */

#define MW_RFVOD_TASK_PRIORITY			VK_TASK_PRIORITY_MW_REF

#define PAL_DMX_SND_TASK_PRIORITY			(VK_TASK_PRIORITY_MW_REF+1)
#define PAL_DMX_RCV_TASK_PRIORITY			(VK_TASK_PRIORITY_MW_REF+1)
#define PAL_SBTL_TASK_PRIORITY              (VK_TASK_PRIORITY_MW_REF+1)
#define PAL_TLTXT_TASK_PRIORITY				(VK_TASK_PRIORITY_MW_REF+1)

#define PAL_DMP_TASK_PRIORITY 				VK_TASK_PRIORITY_MW_REF
#define PAL_TRANSC_TASK_PRIORITY 		VK_TASK_PRIORITY_MW_REF

#ifdef CONFIG_MW_CI
#define PAL_CI_ITC_TIMER_TASK_PRIORITY		(VK_TASK_PRIORITY_MW_REF+1)
#define PAL_CI_ITC_LAYER_TASK_PRIORITY		(VK_TASK_PRIORITY_MW_REF+1)
#endif
#ifdef CONFIG_MW_CI_PLUS
#define PAL_CI_CC_MAIN_TASK_PRIORITY		(VK_TASK_PRIORITY_MW_REF+2) // CC 타스크의 경우 CI+ 캠과의 통신에서 엄밀한 time limit 가 적용되므로 다른 MW 타스크보다 priority 가 높아야 한다.
#define PAL_CI_CC_SUB_TASK_PRIORITY			(VK_TASK_PRIORITY_MW_REF+1)
	#ifdef CONFIG_MW_CI_PLUS_V_1_3
	#define PAL_CI_LSC_TASK_PRIORITY			(VK_TASK_PRIORITY_MW_REF+1)
	#define PAL_CI_OP_TASK_PRIORITY			(VK_TASK_PRIORITY_MW_REF+1)
	#endif
#endif

#ifdef CONFIG_MW_CAS_NUM_SC_SLOT
#define PAL_SC_TASK_PRIORITY				VK_TASK_PRIORITY_MW_REF
#endif

#define PAL_DSMCC_TASK_PRIORITY				(VK_TASK_PRIORITY_MW_REF+1)
#define PAL_SCSI_MON_TASK_PRIORITY			(VK_TASK_PRIORITY_DI_REF)
#define	PAL_PWM_CTL_TASK_PRIORITY			(VK_TASK_PRIORITY_MW_REF+1)
#define PAL_TV_VBI_TASK_PRIORITY			(VK_TASK_PRIORITY_MW_REF+1)
#define PAL_COMM_TASK_PRIORITY				(VK_TASK_PRIORITY_MW_REF - 2)
#define UTIL_TASKUTIL_TASK_PRIORITY			(VK_TASK_PRIORITY_MW_REF - 2 + 15)

#define PAL_DICALLBACK_TASK_PRIORITY		(VK_TASK_PRIORITY_MW_REF+5)

#ifdef CONFIG_DEBUG
#define OCTO_COMMAND_TASK_PRIORITY			(VK_TASK_PRIORITY_MW_REF+2)		/* TODO: redefine */
#endif

#define	OXSERVER_LISTENER_TASK_PRIORITY		(VK_TASK_PRIORITY_MW_REF)

/*********************************************************
				OCTO task stack size
**********************************************************/
#define	OXSERVER_LISTENER_STACK_SIZE		SIZE_32K

#define DB_SVC_TASK_STACK_SIZE				SIZE_64K
#define GWM_TASK_STACK_SIZE					SIZE_256K

#define SVC_FS_TASK_STACK_SIZE				SIZE_64K
#define SVC_CH_TASK_STACK_SIZE				SIZE_64K
#define MW_SI_TASK_STACK_SIZE				SIZE_64K
#define MW_CAS_TASK_STACK_SIZE 				SIZE_64K
#define MW_AV_TASK_STACK_SIZE 				SIZE_64K
#define MW_DMP_TASK_STACK_SIZE 				SIZE_64K
#define MW_EPG_TASK_STACK_SIZE				SIZE_64K
#define MW_EPGLOAD_TASK_STACK_SIZE			SIZE_32K
#define MW_PVR_STACK_SIZE					SIZE_64K
#define MW_CAS_CIMGR_TASK_STACK_SIZE		SIZE_64K
#define MW_SBTL_TASK_STACK_SIZE				SIZE_64K
#define MW_MHEG_RCV_TASK_STACK_SIZE			SIZE_64K
#define MW_MHEG_SI_TASK_STACK_SIZE			SIZE_32K
#define MW_RSV_TASK_STACK_SIZE				SIZE_64K
#define MW_DOWN_TASK_STACK_SIZE				SIZE_64K

#define	MW_SWUP_INSTALL_TASK_SIZE			SIZE_32K

#define MW_PANEL_TASK_STACK_SIZE			SIZE_64K
#if defined(CONFIG_MW_CEC)
#define MW_CEC_TASK_STACK_SIZE				SIZE_8K
#endif
#if defined(CONFIG_MW_COMM)
#define MW_COMM_TASK_STACK_SIZE				SIZE_32K
#endif

#define PAL_DMX_SND_TASK_STACK_SIZE			SIZE_64K
#define PAL_DMX_RCV_TASK_STACK_SIZE			SIZE_64K
#define PAL_SBTL_TASK_STACK_SIZE			SIZE_64K
#define PAL_TLTXT_TASK_STACK_SIZE			SIZE_64K

#define PAL_DMP_TASK_STACK_SIZE 			SIZE_64K

#ifdef CONFIG_MW_CI
#define PAL_CI_ITC_TIMER_TASK_STACK_SIZE	SIZE_64K
#define PAL_CI_ITC_LAYER_TASK_STACK_SIZE	SIZE_1K
#endif
#ifdef CONFIG_MW_CI_PLUS
#define PAL_CI_CC_TASK_STACK_SIZE			SIZE_64K
	#ifdef CONFIG_MW_CI_PLUS_V_1_3
	#define PAL_CI_LSC_TASK_STACK_SIZE		SIZE_64K
	#define PAL_CI_OP_TASK_STACK_SIZE		SIZE_64K
	#endif
#endif

#ifdef CONFIG_MW_CAS_NUM_SC_SLOT
#define PAL_SC_TASK_STACK_SIZE				SIZE_64K
#endif

#define PAL_DSMCC_TASK_STACK_SIZE			SIZE_64K
#define PAL_PWM_CTL_TASK_STACK_SIZE			SIZE_64K
#define PAL_TV_VBI_TASK_STACK_SIZE			SIZE_64K
#if defined(CONFIG_MW_COMM)
#define PAL_COMM_TASK_STACK_SIZE			SIZE_32K
#endif
#define PAL_TRANSC_TASK_STACK_SIZE		SIZE_32K
#define PAL_HDMI_TASK_STACK_SIZE		SIZE_32K
#define PAL_VIDEO_TASK_STACK_SIZE		SIZE_32K
#define PAL_AUDIO_TASK_STACK_SIZE		SIZE_32K

#define UTIL_TASKUTIL_TASK_STACK_SIZE		SIZE_64K

/*********************************************************
				OCTO task message queue size
**********************************************************/
#define OCTO_MSGQ_LEVEL01				32
#define OCTO_MSGQ_LEVEL02				64
#define OCTO_MSGQ_LEVEL03				128
#define OCTO_MSGQ_LEVEL04				256
#define OCTO_MSGQ_LEVEL05				512
#define OCTO_MSGQ_LEVEL06				1024


#define DB_SVC_MSGQ_SIZE				OCTO_MSGQ_LEVEL01
#define GWM_MSGQ_SIZE					OCTO_MSGQ_LEVEL05			//	OAPI Wakeup Msg로 인해 추가 Queue사이즈 확보.

#define	MW_DATASTREAMER_MSGQ_SIZE		OCTO_MSGQ_LEVEL04
#define SVC_CH_MSGQ_SIZE					OCTO_MSGQ_LEVEL02
#define MW_SI_MSGQ_SIZE					OCTO_MSGQ_LEVEL05
#define MW_CAS_MSGQ_SIZE 				OCTO_MSGQ_LEVEL02
#define MW_AV_MSGQ_SIZE 				OCTO_MSGQ_LEVEL02
#define MW_AVCTRL_MSGQ_SIZE 			OCTO_MSGQ_LEVEL02
#define	MW_DMP_MSG_SIZE					OCTO_MSGQ_LEVEL03
#define MW_EPG_MSGQ_SIZE				OCTO_MSGQ_LEVEL05
#define MW_PVR_MSGQ_SIZE				OCTO_MSGQ_LEVEL03
#define MW_CAS_CIMGR_MSGQ_SIZE			OCTO_MSGQ_LEVEL02
#define MW_SBTL_MSGQ_SIZE				OCTO_MSGQ_LEVEL02
#define MW_EPG_LOAD_MSGQ_SIZE			OCTO_MSGQ_LEVEL02

#define MW_TLTXT_MSGQ_SIZE				OCTO_MSGQ_LEVEL04
#define MW_MHEG_MSGQ_SIZE				OCTO_MSGQ_LEVEL03
#define MW_MHEG_RCV_MSGQ_SIZE			OCTO_MSGQ_LEVEL05
#define MW_RSV_MSGQ_SIZE				OCTO_MSGQ_LEVEL01
#define MW_DOWN_MSG_SIZE				OCTO_MSGQ_LEVEL04
#if defined(CONFIG_MW_MM_MEDIA_SBTL_FILE)
#define MW_MDSUB_MSGQ_SIZE				OCTO_MSGQ_LEVEL02
#endif
#if defined(CONFIG_MW_MM_MEDIA_XSUB)
#define MW_XSUB_MSGQ_SIZE				OCTO_MSGQ_LEVEL02
#endif
#define	MW_SYS_MSGQ_SIZE				OCTO_MSGQ_LEVEL02
#if defined(CONFIG_MW_CEC)
#define MW_CEC_MSGQ_SIZE				OCTO_MSGQ_LEVEL02
#endif
#if defined(CONFIG_MW_COMM)
#define MW_COMM_MSGQ_SIZE				OCTO_MSGQ_LEVEL01
#endif
#define MW_OUT_MSGQ_SIZE 				OCTO_MSGQ_LEVEL02

#define PAL_DMX_SND_MSGQ_SIZE			OCTO_MSGQ_LEVEL05
#define PAL_DMX_RCV_MSGQ_SIZE			OCTO_MSGQ_LEVEL05
#define PAL_SBTL_MSGQ_SIZE				OCTO_MSGQ_LEVEL01
#define PAL_TTX_MSGQ_SIZE				OCTO_MSGQ_LEVEL05
#if defined(CONFIG_MW_COMM)
#define PAL_COMM_MSGQ_SIZE				OCTO_MSGQ_LEVEL02
#endif
#define	PAL_TRANSC_MSG_SIZE			OCTO_MSGQ_LEVEL02
#define	PAL_DMP_MSG_SIZE				OCTO_MSGQ_LEVEL03


#ifdef CONFIG_MW_CI
#define PAL_CI_ITC_MSG_Q_SIZE			OCTO_MSGQ_LEVEL02
#endif
#ifdef CONFIG_MW_CI_PLUS
#define PAL_CI_CC_MSG_Q_SIZE			OCTO_MSGQ_LEVEL02
	#ifdef CONFIG_MW_CI_PLUS_V_1_3
	#define PAL_CI_LSC_MSG_Q_SIZE		OCTO_MSGQ_LEVEL01
	#define PAL_CI_OP_MSG_Q_SIZE		OCTO_MSGQ_LEVEL01
	#endif
#endif

#ifdef CONFIG_MW_CAS_NUM_SC_SLOT
#define PAL_SC_MSGQ_SIZE				OCTO_MSGQ_LEVEL02
#endif

#define PAL_DSMCC_MSGQ_SIZE				OCTO_MSGQ_LEVEL06
#define PAL_TV_VIB_MSGQ_SIZE			OCTO_MSGQ_LEVEL05

#define UTIL_TASKUTIL_MSGQ_SIZE			OCTO_MSGQ_LEVEL06

#define PAL_FS_USB_MSGQ_SIZE				OCTO_MSGQ_LEVEL04

#ifndef VK_Print

#ifdef CONFIG_DEBUG
#if defined(WIN32)
#define VK_Print(_fmt, ...)				EMU_UART_Print(_fmt, ## __VA_ARGS__)
#else
#define VK_Print(_fmt, ...)				HxLOG_Print(_fmt, ## __VA_ARGS__)
#endif
#else
#define VK_Print(_fmt, ...)				((void)0)
#endif

#endif


#define NOT_USED_PARAM(x)				(void)(x)
#define VALUE_TO_STR(x)					#x

#define	OBAMA_DIR_DATA					CONFIG_PRODUCT_DATA_DIR
#define	OBAMA_DIR_BACKUPDATA			CONFIG_PRODUCT_BACKUPDATA_DIR
#define	OBAMA_DIR_USERDATA				CONFIG_PRODUCT_USERDATA_DIR


/* ANSI Color Codes */
#ifndef C_CODE_RESET
#if !defined(WIN32) && !defined(_ICC)
	#define C_CODE_RESET				"\033[0m"
	#define C_CODE_F_BLACK				"\033[30m"		// Fg:Black
	#define C_CODE_F_RED				"\033[31m"		// Fg:Red
	#define C_CODE_F_GREEN				"\033[32m"		// Fg:Green
	#define C_CODE_F_YELLOW				"\033[33m"		// Fg:Yellow
	#define C_CODE_F_BLUE				"\033[34m"		// Fg:Blue
	#define C_CODE_F_PURPLE				"\033[35m"		// Fg:Purple
	#define C_CODE_F_CYAN				"\033[36m"		// Fg:Cyan
	#define C_CODE_F_YELLOW_B_BLACK		"\033[33;40m"	// Fg:Yellow, Bg:Black
	#define C_CODE_F_WHITE_B_RED		"\033[37;41m"	// Fg:White, Bg:Red
	#define C_CODE_F_BLACK_B_GREEN		"\033[30;42m"	// Fg:Black, Bg:Green
	#define C_CODE_F_BLACK_B_YELLOW		"\033[30;43m"	// Fg:black, Bg:Yellow
	#define C_CODE_F_WHITE_B_BLUE		"\033[37;44m"	// Fg:White, Bg:Blue
	#define C_CODE_F_WHITE_B_PURPLE		"\033[37;45m"	// Fg:White, Bg:Purple
	#define C_CODE_F_BLACK_B_CYAN		"\033[30;46m"	// Fg:Black, Bg:Cyan
	#define C_CODE_F_RED_B_BLACK		"\033[31;40m"	// Fg:Red, Bg:Black
#else
	#define C_CODE_RESET                ""
	#define C_CODE_F_BLACK              ""
	#define C_CODE_F_RED                ""
	#define C_CODE_F_GREEN              ""
	#define C_CODE_F_YELLOW             ""
	#define C_CODE_F_BLUE               ""
	#define C_CODE_F_PURPLE             ""
	#define C_CODE_F_CYAN               ""
	#define C_CODE_F_YELLOW_B_BLACK     ""
	#define C_CODE_F_WHITE_B_RED        ""
	#define C_CODE_F_BLACK_B_GREEN      ""
	#define C_CODE_F_BLACK_B_YELLOW     ""
	#define C_CODE_F_WHITE_B_BLUE       ""
	#define C_CODE_F_WHITE_B_PURPLE     ""
	#define C_CODE_F_BLACK_B_CYAN       ""
	#define C_CODE_F_RED_B_BLACK		""
#endif
#endif

#define OSD_SCREEN_OFFSET_X 		0
#define OSD_SCREEN_OFFSET_Y 		0
#define OSD_SCREEN_WIDTH			CONFIG_MWC_OSD_HORIZONTAL
#define OSD_SCREEN_HEIGHT			CONFIG_MWC_OSD_VERTICAL

/* SVC API synchronizing mode */
typedef enum
{
	eSyncMode,
	eAsyncMode
} ApiSyncMode_t;

//#define USE_RLIB

typedef HINT32 (* PostMsgToMgrCb_t)(HINT32		nMsg,		/**< [in] AP로 전송하는 event message */
									Handle_t	hAction,	/**< [in] Action handle, if needed */
									HINT32		nParam1,	/**< [in] Message parameter 1, if needed */
									HINT32		nParam2,	/**< [in] Message parameter 2, if needed */
									HINT32		nParam3		/**< [in] Message parameter 3, if needed */
									);
typedef HINT32 (* PostDataToMgrCb_t)(HINT32		 nMsg,			/**< [in] AP로 전송하는 event message */
									Handle_t	 hAction,	/**< [in] Action handle, if needed */
									HINT32		 nParam1,	/**< [in] Message parameter 1, if needed */
									HINT32		 nParam2,	/**< [in] Message parameter 2, if needed */
									void		*pvData,	/**< [in] Message Data Pointer, if needed */
									HUINT32		 ulSize);	/**< [in] Message Data Size, if needed */

#endif /* _OCTOCOMMON_H_ */

