#include <stdio.h>
#include <stdlib.h>
#include "vkernel.h"
#include "di_init.h"

extern int	obama_main(int argc, char *argv[]);

int	main(int argc, char *argv[])
{
#ifdef	CONFIG_DEBUG
	HLIB_DBG_Init();
#endif

#if defined(CONFIG_ZAPPING_TIME_CHECK)
//	obama_cbForTimeLog("Obama Main Started... (%s)", "start");
#endif

#if defined(CONFIG_PROD_OS_EMULATOR)
	VK_Init();
#endif

	DI_Init();

#if	defined(CONFIG_DEBUG) || defined(CONFIG_DEBUG_RELEASE_LOG)
	#if defined(CONFIG_DEVICE_DEBUG)
			CMD_Init();
			CMD_DI_Init();
	#endif
#endif

	obama_main(argc, argv);

	/* Wait forever */
	while(1)
	{
		VK_TASK_Sleep(360000);
	}

	exit(EXIT_SUCCESS);
}

