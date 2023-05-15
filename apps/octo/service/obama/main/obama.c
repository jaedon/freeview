#include <stdio.h>
#include <stdlib.h>

#include <vkernel.h>
#include <di_init.h>
#include <cmdtool.h>
#include <hapi.h>
#include <napi.h>

#if defined(CONFIG_DROP_ROOT_PRIVILEGES)//1 To Do 관련 API를 HAPI에 생성후 삭제 예정
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/resource.h>

//#include <secapi.h>
#endif

#if (defined(CONFIG_DEBUG) & defined(OS_LINUX))
#include <sys/resource.h>
#endif

#if defined(CONFIG_HUMAX_CRASHREPORT) && !defined(CONFIG_DEBUG)
#include "hcrlib.h"
#endif

extern void	OBAMA_Init(void);

#if defined(CONFIG_ZAPPING_TIME_CHECK)
STATIC int obama_cbForTimeLog (const HCHAR *format, ...)
{
	va_list			va;

	va_start(va, format);
	HLIB_CMD_FileLogVa(CONFIG_ZAPPING_TIME_CHECK_LOGFILE, format, &va);
	va_end(va);

	return 0;
}
#endif

int	obama_main(int argc, char *argv[])
{
#if defined(CONFIG_HUMAX_CRASHREPORT)
	HCR_Init(NULL);
#endif

//	HSMSIG_Main();

#if defined(CONFIG_MW_CAS_NAGRA)
	HLIB_EXTRA_ReadSpecialMemory(HxEXTRA_MEM_NaPd);
#endif

	//	Drop Root Previleges --> if CONFIG_DROP_ROOT_PRIVILEGES is ON,
	HLIB_EXTRA_DropRootPrivileges("obama");

	HAPI_Init();

#if defined(CONFIG_SUPPORT_NIMA)
	NAPI_Init();
#endif

	OBAMA_Init();

	HAPI_SendReadySignal(HLIB_STD_GetPID());

	return 0;
}
