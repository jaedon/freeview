/**************************************************************
*	@file	roma.c
*	http://www.humaxdigital.com
*	@author			humax
*	@brief			DLNA / WOON
**************************************************************/

/*
* ⓒ 2011-2012 Humax Co., Ltd.
* This program is produced by Humax Co., Ltd. ("Humax") and
* the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
* non-assignable, non-transferable and non-exclusive license to use this Software.
* You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
* you agree to the responsibility to take all reasonable efforts to protect the any information
* you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
* reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
* If you have no authorized license, discontinue using this Software immediately.
*
* THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
* IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
* IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/

#define	________________Header_Files_______________________________
#include "homma_interop.h"

#include <vkernel.h>
#include <hlib.h>
#include <oapi.h>
#include <hapi.h>
#include <uapi.h>
#include "hom_int.h"
#include "homma_bus.h"
#include "homma_server.h"
#include "homma_system.h"

#if defined(CONFIG_HOMMA_DMS) || defined(CONFIG_HOMMA_SATIP)
#include "pl_streaming.h"
#endif

#if defined(CONFIG_HOMMA_USERCP)
#include "pl_usercp.h"
#endif

#if 0 //defined(CONFIG_LXC_ONLY) || defined(CONFIG_CHROOT_AND_LXC)		// TODO: 관련 API를 HAPI에 생성후 삭제 예정
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/resource.h>
#endif

#if defined(CONFIG_HOMMA_CDI)
#include "di_crypt.h" /* for di_crypt.so shared mode */
#endif

#if defined(CONFIG_HUMAX_CRASHREPORT)
#include "hcrlib.h"
#endif

#define ________________Static_Variables___________________________
#if defined(CONFIG_HOMMA_DMS)
extern void		HOMMA_CMD_Register_DMS(void);
#endif
#if defined(CONFIG_HOMMA_DMP)
extern void		HOMMA_CMD_Register_DMP(void);
#endif
#if defined(CONFIG_HOMMA_DMR)
extern void		HOMMA_CMD_Register_DMR(void);
#endif
#if defined(CONFIG_HOMMA_DIAL)
extern void 	HOMMA_CMD_Register_DIAL(void);
#endif
#if defined(CONFIG_HOMMA_WOON)
extern void		HOMMA_CMD_Register_WOONS(void);
#endif
#if defined(CONFIG_HOMMA_SATIP)
extern void		HOMMA_CMD_Register_SATIP(void);
#endif
#if defined(CONFIG_HOMMA_AIRPLAY)
extern void		HOMMA_CMD_Register_AIRPLAY(void);
#endif
#if defined(CONFIG_HOMMA_RC)
extern void		HOMMA_CMD_Register_RC(void);
#endif

#define ________________Internal_Functions_________________________
HINT32   __homma_cmdtool_init(void)
{
#if defined(CONFIG_HOMMA_DMP)
	HOMMA_CMD_Register_DMP();
#endif
#if defined(CONFIG_HOMMA_DMS)
	HOMMA_CMD_Register_DMS();
#endif
#if defined(CONFIG_HOMMA_DMR)
	HOMMA_CMD_Register_DMR();
#endif
#if defined(CONFIG_HOMMA_DIAL)
	HOMMA_CMD_Register_DIAL();
#endif
#if defined(CONFIG_HOMMA_WOON)
	HOMMA_CMD_Register_WOONS();
#endif
#if defined(CONFIG_HOMMA_SATIP)
	HOMMA_CMD_Register_SATIP();
#endif
#if defined(CONFIG_HOMMA_AIRPLAY)
	HOMMA_CMD_Register_AIRPLAY();
#endif
#if defined(CONFIG_HOMMA_DMS) || defined(CONFIG_HOMMA_SATIP)
	HOMMA_CMDRegister_Streaming_Init();
#endif
#if defined(CONFIG_HOMMA_RC)
	HOMMA_CMD_Register_RC();
#endif
#if defined(CONFIG_HOMMA_USERCP)
	HOMMA_CMD_Register_USERCP();
#endif

	return 0;
}

// check how homma was terminated
#define HOMMA_CRASH_CHECK_PATH "mnt/hd2/homma.crashed"


#ifdef SUPPORT_DIAL_EXTENTION
static HCHAR s_portal_url[4096] = {0,}; // url/url_portal.txt or default portal url
#endif	// SUPPORT_DIAL_EXTENTION


static	void	__homma_debug_conf( void )
{
#ifdef CONFIG_DEBUG
	HLIB_DBG_Init();
	__homma_cmdtool_init();
#endif
}

void	__homma_run(HINT32 argc, HCHAR **argv)
{
#if defined(CONFIG_HOMMA_CDI)
	DI_ERR_CODE diErr = 0;
#endif
#ifndef HOMMA_USE_APPKIT_DIRECTLY
	HOMMA_INTEROP_Init();
#endif

	HOMMA_BUS_Init();
	HOMMA_SYSTEM_Run();

#if defined(CONFIG_HOMMA_CDI) /* for nexus_platform_join */
	/*
	 * For using the di_crypt.so (nexus client shared lib), check configs:
	 * - device/product_config/defconfigs/hms1000sph2/rev0.3/platform_defconfig
	 *   CONFIG_DI_MODULE_SHARED_LIB=y
	 *   CONFIG_DI_MODULE_CRYPT_SHARED_LIB=y
	 *
	 * - apps/make/product_configs/hms1000sph2_defconfig"
	 *   CONFIG_HOMMA_CDI=y
	 */
	diErr = DI_CRYPT_Init();
	if (diErr != DI_ERR_OK)
	{
		HxLOG_Print("Error> Can't initialize the DI crypto\n");
		/* exit(-1);*/
	}
#endif

#ifdef CONFIG_HOMMA_USERCP
	/* UserCP Init */
	if(PL_USERCP_Init() != ERR_OK)
	{
		HxLOG_Error("PL_USERCP_Init() fail\n");
	}
#endif

	HOM_DMS_Run();
	HOM_DMP_Run();
	HOM_DMR_Run();


	HOM_WOONSERVER_Run();
	HOM_DIAL_Run();
#if defined(CONFIG_HOMMA_AIRPLAY)
	HOM_AIRPLAY_Run();
#endif
	HOM_SATIP_Run();
#if defined(CONFIG_HOMMA_RC)
	HOM_RC_Run();
#endif
	HOMMA_SERVER_Run();
}

#define ________________Main_Function______________________________

#ifdef HOMMA_USE_APPKIT_DIRECTLY
static void	_apk_homma_task(void *arg)
{
	while (TRUE)
	{
		if (HOMMA_INTEROP_EVENT_Dispatch() == 0)
			HLIB_STD_TaskSleep(100);
	}
}
#endif

int		homma_main(int argc, char *argv[])
{
#ifdef HOMMA_USE_APPKIT_DIRECTLY
	unsigned long	ulTaskId;
#endif

	HxLOG_Print(HxANSI_COLOR_RED("[HOMMA] Process Start!!!!\n"));
	VK_Init();

	HLIB_EXTRA_DropRootPrivileges("homma");

	signal(SIGPIPE, SIG_IGN);

	__homma_debug_conf();

	__homma_run(argc, argv);

	HAPI_SendReadySignal(HLIB_STD_GetPID());

#ifdef HOMMA_USE_APPKIT_DIRECTLY
	VK_TASK_Create(_apk_homma_task, 40, SIZE_16K, "hommaApk", NULL, (unsigned long *)&ulTaskId, 0);
	VK_TASK_Start(ulTaskId);
#endif

	return 0;
}

static void s_signal_hanlder(int signo) {

	HxFILE_t * fp = HLIB_FILE_Open(HOMMA_CRASH_CHECK_PATH, "wb");
	char temp[10] = {0,};
	HxSTD_snprintf(temp, sizeof(temp), "%d", signo);
	if (fp) {
		HLIB_FILE_Write(fp, temp, strlen(temp), 1);
		HLIB_FILE_Close(fp);
	}
}

static void s_siginfo_handler(int signo, siginfo_t *sig_info, void *context) {
	s_signal_hanlder(signo);
}

static void s_setAppSignal(void) {
	signal(SIGSEGV, s_signal_hanlder);
	signal(SIGINT, s_signal_hanlder);
	signal(SIGKILL, s_signal_hanlder);
	signal(SIGQUIT, s_signal_hanlder);
	signal(SIGHUP, s_signal_hanlder);
	signal(SIGILL, s_signal_hanlder);
	signal(SIGBUS, s_signal_hanlder);
	signal(SIGABRT, s_signal_hanlder);
	signal(SIGFPE, s_signal_hanlder);
}

static unsigned char s_check_previously_crashed(void) {

	// If previous termination was by crash, there should be HOMMA_CRASH_CHECK_PATH
	HxFILE_t * fp = HLIB_FILE_Open(HOMMA_CRASH_CHECK_PATH, "rb");
	if (fp) {
		HLIB_FILE_Close(fp);
		return 1;
	}
	return 0;
}

unsigned char HOMMA_CheckPreviouslyCrashed(void) {
	return s_check_previously_crashed();
}

void HOMMA_RemoveCrashCheckerFile(void) {
	HLIB_FILE_Delete(HOMMA_CRASH_CHECK_PATH);
}



#ifdef SUPPORT_DIAL_EXTENTION

/**
 * url_portal.txt or default portal url
 */
static void __get_portal_url( char * pszUrl, unsigned int ulLen) {

#define _PORTAL_URL_FILE	"/url/url_portal.txt"

	char * pTempUrl = NULL;
	HxSTD_MemSet(pszUrl,0,ulLen);
	pTempUrl = (char *)UAPI_FM_GetDefaultURL((HCHAR *)CONFIG_APPPORTAL_SERVER_URL, (HCHAR *)_PORTAL_URL_FILE);
	if(pTempUrl) HxSTD_StrNCpy(pszUrl, pTempUrl, ulLen);
	pszUrl[ulLen-1] = '\0';

#undef _PORTAL_URL_FILE
}

/**
 *
 */
void HOMMA_GetPortalURL( char * pszUrl, unsigned int ulLen) {

	int size = 0;

	//if (!strncmp(s_portal_url, CONFIG_APPPORTAL_SERVER_URL, sizeof(s_portal_url))) {
	__get_portal_url(s_portal_url, sizeof(s_portal_url));
	//}

	size = sizeof(s_portal_url);
	HxSTD_StrNCpy(pszUrl, s_portal_url, ulLen > size ? size : ulLen);
}

#endif	// SUPPORT_DIAL_EXTENTION


int		main(int argc, char *argv[])
{

	// crash handler
#if defined(CONFIG_HUMAX_CRASHREPORT)
	HCR_Init(s_siginfo_handler);
	signal(SIGINT, s_signal_hanlder);
	signal(SIGKILL, s_signal_hanlder);
	signal(SIGQUIT, s_signal_hanlder);
	signal(SIGABRT, s_signal_hanlder);
#else
	s_setAppSignal();
#endif


#ifdef SUPPORT_DIAL_EXTENTION
	__get_portal_url(s_portal_url, sizeof(s_portal_url));
#endif


#if	defined(CONFIG_SUPPORT_HOMMA)
	homma_main(argc, argv);
#endif

	while (TRUE)
		HLIB_STD_TaskSleep(100000);

	NOT_USED_PARAM(argc);
	NOT_USED_PARAM(argv);
	HxLOG_Print(HxANSI_COLOR_RED("[HOMMA] Process End!!!!\n"));

	return 0;
}
#define _____END_OF_FILE_________

