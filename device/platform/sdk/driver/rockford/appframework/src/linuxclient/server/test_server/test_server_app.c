#include <stdio.h>
#include <stdlib.h>

#include "bstd_defs.h"
#include "berr.h"

#include "server.h"
#include "test_server.h"

void app_main(int argc, char **argv)
{
	BERR_Code err = BERR_SUCCESS;
	BEMU_Server_Handle hEmuServer;
	BEMU_Settings stSettings;
	BEMU_Server_Settings stServerSettings;

	if(argc == 1)
	{
		printf("Usage: %s [-d] port_num\n", argv[0]);
		printf("Option:\n");
		printf("       -d      turn on debug dump info\n");
	} 

	BEMU_Server_GetSettings(&stSettings, &stServerSettings);
		
	/* TODO: Fill in specific emulator settings via BEMU_Settings */

	/* Override BEMU_Server_Settings, if any */
	if (argc >= 2)
		stServerSettings.ulPort = ((unsigned short)atoi(argv[(argc==2)? 1:2]) );

	err = BEMU_Server_Open(&hEmuServer, &stSettings, &stServerSettings);
	if (err != BERR_SUCCESS)
	{
		BDBG_ERR(("Failed to open emulator server."));
	}

	err = BEMU_Server_Close(hEmuServer);
	if (err != BERR_SUCCESS)
	{
		BDBG_ERR(("Failed to close emulator server."));
	}
	
	return;
}
