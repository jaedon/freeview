/**************************************************************
 *	@file		dama.c
 *	Humax Data Manager for OCTO 2.0
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2012/03/01
 *	@author			humax
 *	@brief			HumaxDataManager for OCTO 2.0 Project
 **************************************************************/
#include "vkernel.h"

extern int		DAMA_Main(int argc, char *argv[]);

#if !defined(CONFIG_SINGLE_PROCESS)
int		main (int argc, char *argv[])
{

	VK_Init();
#if defined(CONFIG_PROD_OS_EMULATOR)
	EMU_UART_Init();
#endif
	
	DAMA_Main(argc, argv);

	for(;;)
	{
		VK_TASK_Sleep(10000);
	}
	(void)argc;
	(void)argv;
	return 0;
}
#endif

