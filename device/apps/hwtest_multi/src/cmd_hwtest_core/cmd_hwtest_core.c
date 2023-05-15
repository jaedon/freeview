/**
 * cmd_hwtest.c
*/

#include "di_init.h"

int g_iDdrTestSize;

int main(void)
{
	DI_PLATFORM_Init();

	while(1)
	{
		HI_VK_TASK_Sleep(1000000);
	}

	return 0;
}
