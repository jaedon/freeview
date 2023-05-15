
#include <bstd.h>
#include <stdio.h>
#include <string.h>

#include "ca_sec.h"

#include "sec_harness_test.h"
#include "sec_harness_test_lchipid.h"
#include "sec_harness_test_lchipid_private.h"
#include "sec_harness_test_utilities.h"
#include "nocs3_harness_test_utilities.h"
#include "nexus_platform.h"


static void sec_harness_test_lchipid_help (void);



void sec_harness_test_lchipid (int argc, char* argv[])
{
	NEXUS_PlatformSettings platformSettings;
	TSecFunctionTable *functionTable;
	TSecNuid64 pxNuid={0};
	TChar chipRev[2];
	
	unsigned int i;
	if ( argc >2 )
	{
		sec_harness_test_lchipid_help();
		return;
	}


	/* Platform init */
    NEXUS_Platform_GetDefaultSettings(&platformSettings); 
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
	functionTable = secGetFunctionTable();
	if(functionTable->secGetNuid((TSecNuid *)pxNuid)){
		printf("[SEC][EX]Get NUID error!\n"); 
		return;
	}
	#if 0
	printf("NUID = 0x"); 
	for(i=0; i<4; i++)printf("%02x", pxNuid[i]);
	printf("\n"); 
	#endif
	if(functionTable->secGetChipsetRevision((const TChar **)&chipRev)){
		printf("[SEC][EX]Get Chip REvision error!\n"); 
		return;
	}

	printf("[SEC][M] TSecGetNuid64, TSecGetChipsetRevision and TSecGetChipsetExtension done successfully\n"); 
	printf("[SEC][M] - NUID\t\t\t - 0x"); 
	for(i=0; i<4; i++)printf("%02x", pxNuid[i]);
	printf(" -\n"); 
	printf("[SEC][M] - Chip Revision\t - %c%c -\n", chipRev[0], chipRev[1]);
	/* Clean up */
	NEXUS_Memory_Free(functionTable);
    NEXUS_Platform_Uninit();

}


static void sec_harness_test_lchipid_help (void)
{
	printf("\t%s\n\n", SEC_HELP_M_LCHIPID_INST);
}


