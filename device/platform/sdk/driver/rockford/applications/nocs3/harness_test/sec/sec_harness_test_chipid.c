
#include <bstd.h>
#include <stdio.h>
#include <string.h>

#include "ca_sec.h"

#include "sec_harness_test.h"
#include "sec_harness_test_chipid.h"
#include "sec_harness_test_chipid_private.h"
#include "sec_harness_test_utilities.h"
#include "nocs3_harness_test_utilities.h"
#include "nexus_platform.h"


static void sec_harness_test_chipid_help (void);



void sec_harness_test_chipid (int argc, char* argv[])
{
	NEXUS_PlatformSettings platformSettings;
	TSecFunctionTable *functionTable;
	TSecNuid64 pxNuid={0};
	TChar chipRev[2];
	TChar chipExt[20]={0};
	unsigned int i;
	TSecStatus status;
	if ( argc >2 )
	{
		sec_harness_test_chipid_help();
		return;
	}


	/* Platform init */
    NEXUS_Platform_GetDefaultSettings(&platformSettings); 
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
	functionTable = secGetFunctionTable();
	if(functionTable->secGetNuid64((TSecNuid64 *)pxNuid)){
		printf("[SEC][EX]Get NUID error!\n"); 
		return;
	}
	printf("NUID = 0x"); 
	for(i=0; i<8; i++)printf("%02x", pxNuid[i]);
	printf("\n"); 
	status = functionTable->secGetChipsetRevision((const TChar **)&chipRev);
	if(status !=SEC_NO_ERROR){
		printf("[SEC][EX]Get Chip Revision error %d!\n", status); 
		return;
	}
	status = functionTable->secGetChipsetExtension((const TChar **)chipExt);
	if(status !=SEC_NO_ERROR){
		printf("[SEC][EX]Get Chip Extention error %d!\n", status); 
		return;
	}
	printf("[SEC][M] TSecGetNuid64, TSecGetChipsetRevision and TSecGetChipsetExtension done successfully\n"); 
	printf("[SEC][M] - NUID\t\t\t - 0x"); 
	for(i=0; i<8; i++)printf("%02x", pxNuid[i]);
	printf(" -\n"); 
	printf("[SEC][M] - Chip Revision\t - %c%c -\n", chipRev[0], chipRev[1]);
	printf("[SEC][M] - Chip Extension\t - %s -\n", chipExt);
	/* Clean up */
	NEXUS_Memory_Free(functionTable);
    NEXUS_Platform_Uninit();

}


static void sec_harness_test_chipid_help (void)
{
	printf("\t%s\n\n", SEC_HELP_M_CHIPID_INST);
}


