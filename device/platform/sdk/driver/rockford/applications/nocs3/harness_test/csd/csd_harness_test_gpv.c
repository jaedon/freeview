
#include <bstd.h>
#include <stdio.h>
#include <string.h>

#include "nocs_csd_impl.h"
#include "nocs_csd.h"

#include "csd_harness_test.h"
#include "csd_harness_test_parser.h"
#include "csd_harness_test_gpv.h"
#include "csd_harness_test_gpv_private.h"
#include "csd_harness_test_utilities.h"
#include "nocs3_harness_test_utilities.h"
#include "nexus_platform.h"


static void csd_harness_test_gpv_help (void);


#ifdef NOCS3_DEBUG_MESSAGE
static void csd_harness_test_gpv_dump_paramters (CSD_Harness_Test_SPV_GPV_Parameters * pParams)
{
	printf("CSD_Harness_Test_SPV_Parameters is :\n");

	printf("pvName: %x\n", pParams->pvName);
	printf("pvSize: 0x%x\n", pParams->pvSize);
	printf("pvId 0x%x\n", pParams->pvId);
	printf("bootMode 0x%x\n", pParams->bootMode);
}
#endif

#define CSD_HARNESS_TEST_GPV_GET_ARRAY(type, function, pvname) \
{\
	type variable;\
	status = function (variable);\
	if ( status==CSD_NO_ERROR )\
	{\
		unsigned int i;\
		printf("[CSD][M] - %s - 0x", #pvname);\
		for (i=0; i<sizeof(variable); i++ )\
		{\
			printf("%02x",variable[i]);\
		}\
		printf(" - %s()  done successfully.\n", #function);\
	}\
	else\
	{\
		CSD_ERR_TRACE (status, #function, "Operation failed.\n");\
	}\
}

#define CSD_HARNESS_TEST_GPV_GET_ARRAY_FOR_SIZE(type, size, function, pvname) \
{\
	type * variable;\
	if ( size !=0 )\
	{\
		variable = (type *)malloc(size*sizeof(type));\
	}\
	else\
	{\
		variable = NULL;\
	}\
	if (variable != NULL)\
	{\
		status = function (variable, size);\
		if ( status==CSD_NO_ERROR )\
		{\
			unsigned int i;\
			printf("[CSD][M] - %s - 0x", #pvname);\
			for (i=0; i<size; i++ )\
			{\
				printf("%02x",variable[i]);\
			}\
			printf(" - %s() done successfully.\n", #function);\
		}\
		else\
		{\
			CSD_ERR_TRACE (status, #function, "Operation failed.\n");\
		}\
		free (variable);\
	}\
	else\
	{\
		CSD_OTHER_ERR_TRACE ("Operation failed.\n");\
	}\
}

#define CSD_HARNESS_TEST_GPV_GET_VALUE(type, function, string, pvname) \
{\
	type variable;\
	status = function (&variable);\
	if ( status==CSD_NO_ERROR )\
	{\
	printf("[CSD][M] - %s - %s - %s() done successfully.\n", #pvname, string[variable], #function);\
	}\
	else\
	{\
		CSD_ERR_TRACE (status, #function, "Operation failed.\n");\
	}\
}

#define CSD_HARNESS_TEST_GPV_GET_SCSPV(pvid, array, size, function, pvname) \
{\
	status = function (pvid, NULL, size, array);\
	if ( status==CSD_NO_ERROR )\
	{\
		unsigned int i;\
		printf("[CSD][M] - %s - 0x", #pvname);\
		for (i=0; i<size; i++ )\
		{\
			printf("%02x",array[i]);\
		}\
		printf(" - %s() done successfully.\n", #function);\
	}\
	else\
	{\
		CSD_ERR_TRACE (status, #function, "Operation failed.\n");\
		goto Clean_Up;\
	}\
}


void csd_harness_test_gpv (int argc, char* argv[])
{
	CSD_Harness_Test_SPV_GPV_Parameters params;
	NOCS3_HARNESS_TEST_STATUS_CODE  nocs3_status;
 	TCsdInitParameters csdInit;
	TCsdStatus status;
	TCsdTerminateParameters csdTerminateHandle; 
	NEXUS_PlatformSettings platformSettings;

	if ( (argc <3) || (strcmp (argv[2], NOCS3_HARNESS_TEST_OPTION_HELP)==0 ) )
	{
		csd_harness_test_gpv_help();
		return;
	}

	memset (&params, 0, sizeof (CSD_Harness_Test_SPV_GPV_Parameters));
	if ( (nocs3_status=csd_harness_test_spv_gpv_parse_cmd_line (argc, argv, &params, false)) != NOCS3_HARNESS_TEST_STATUS_CODE_SUCCESS )
	{
		/* Parser error code */
		CSD_OTHER_ERR_TRACE ("GPV command parser failed.  Please make sure the command line parameters are valid");
		goto Clean_Up_1;
	}

#ifdef NOCS3_DEBUG_MESSAGE
	csd_harness_test_gpv_dump_paramters (&params);
#endif

	/* Platform init */
    NEXUS_Platform_GetDefaultSettings(&platformSettings); 
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);

	memset (&csdInit, 0, sizeof(TCsdInitParameters));
	status=csdInitialize(&csdInit);
	if( (status!=CSD_NO_ERROR) && (status !=CSD_ERROR_DRIVER_ALREADY_INITIALIZED) )
	{
		CSD_ERR_TRACE (status, "csdInitialize", "Operation failed");
		goto Clean_Up_1;
	}	

	switch (params.pvName)
	{
	case NOCS3_HARNESS_TEST_PV_NAME_MSID:
		CSD_HARNESS_TEST_GPV_GET_ARRAY (TCsd4BytesVector, csdGetMarketSegmentId, MSID);
		break;

	case NOCS3_HARNESS_TEST_PV_NAME_STBCASN:
		CSD_HARNESS_TEST_GPV_GET_ARRAY (TCsd4BytesVector, csdGetStbCaSn, STBCASN);
		break;

	case NOCS3_HARNESS_TEST_PV_NAME_SCSPV:
		CSD_HARNESS_TEST_GPV_GET_SCSPV(params.pvId, params.pvValue, params.pvSize, csdGetScsPv, SCSPV);
		break;

	case NOCS3_HARNESS_TEST_PV_NAME_BOOTMODE:
		CSD_HARNESS_TEST_GPV_GET_VALUE (TCsdBootMode, csdGetBootMode, gHarnessTestBootModeString, BootMode);
		break;

	case NOCS3_HARNESS_TEST_PV_NAME_SCS:
		CSD_HARNESS_TEST_GPV_GET_VALUE (TCsdActivationFlag, csdGetScsActivationFlag, gHarnessTestActivationFlagString, ScsEnable);
		break;

	case NOCS3_HARNESS_TEST_PV_NAME_DIF:
		CSD_HARNESS_TEST_GPV_GET_VALUE (TCsdDebugInterfaceAccessMode, csdGetDebugInterfaceProtectionLevel, gHarnessTestDebugInterfaceStatusString, DbgIfProtectionLevel);
		break;

	case NOCS3_HARNESS_TEST_PV_NAME_KLD:
		CSD_HARNESS_TEST_GPV_GET_VALUE (TCsdActivationFlag, csdGetKeyLadderDeactivationFlag, gHarnessTestActivationFlagString, KLDeactiveFlag);
		break;

	case NOCS3_HARNESS_TEST_PV_NAME_RAM:
		CSD_HARNESS_TEST_GPV_GET_VALUE (TCsdActivationFlag, csdGetScramblingActivationFlag, gHarnessTestActivationFlagString, RamScrambling);
		break;

	case NOCS3_HARNESS_TEST_PV_NAME_FLASH:
		CSD_HARNESS_TEST_GPV_GET_VALUE (TCsdActivationFlag, csdGetFlashProtectionActivationFlag, gHarnessTestActivationFlagString, FlashProtection);
		break;

	case NOCS3_HARNESS_TEST_PV_NAME_RAMUNSCRAMBSIZE:
		CSD_HARNESS_TEST_GPV_GET_ARRAY_FOR_SIZE(TCsdUnsignedInt8, params.pvSize, csdGetRamUnscrambledStorageSize, RAMUNSCRAMBSIZE);
		break;

	default:
		break;

	}

Clean_Up:
	/* Clean up */
    csdTerminateHandle.initParam = &csdInit; 
    csdTerminateHandle.keyHandleToRelease = NULL; 
    csdTerminate(&csdTerminateHandle);
	
Clean_Up_1:
	if ( params.pvValue)
	{
		free (params.pvValue);
	}

   fflush(stdout);
   NEXUS_Platform_Uninit();

}


static void csd_harness_test_gpv_help (void)
{
	printf("\t%s\n\n", CSD_HELP_M_GPV_INST);
	printf("\t\t%s\n\n", CSD_HELP_M_GPV_S);
	printf("\t\t%s\n\n", CSD_HELP_M_GPV_PVID);
}
