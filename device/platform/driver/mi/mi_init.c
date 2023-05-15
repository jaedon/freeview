#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>

#include "nexus_base_types.h"
#include "nexus_platform.h"
#include "nexus_dma.h"

#include "berr.h"
#include "bstd_defs.h"
#include "bdbg.h"

extern int DI_MI_DECRYPTOR_SOC_Init(void);
extern int DI_MI_DECRYPTOR_SOC_Uninit(void);
int DI_MI_Init(void)
{
	(void)NEXUS_Platform_Join();
	(void)DI_MI_DECRYPTOR_SOC_Init();
	return 0;
}

void DI_MI_Uninit(void)
{
	NEXUS_Platform_Uninit();
	(void)DI_MI_DECRYPTOR_SOC_Uninit();
}


