#include <bstd.h>
#include <stdio.h>
#include <string.h>

#include "nocs_csd_impl.h"
#include "nocs_csd.h"

#include "csd_harness_test.h"
#include "csd_harness_test_parser.h"
#include "csd_harness_test_spv.h"
#include "csd_harness_test_spv_private.h"
#include "csd_harness_test_utilities.h"
#include "nocs3_harness_test_utilities.h"
#include "nexus_platform.h"


static void csd_harness_test_spv_help (void);


#ifdef NOCS3_DEBUG_MESSAGE
static void csd_harness_test_spv_dump_paramters (CSD_Harness_Test_SPV_GPV_Parameters * pParams)
{
	printf("CSD_Harness_Test_SPV_Parameters is :\n");

	printf("pvName: %x\n", pParams->pvName);
	printf("pvSize: 0x%x\n", pParams->pvSize);
	if (pParams->pvSize)
			nocs3_harness_test_utility_dump_array (pParams->pvValue, pParams->pvSize);
	printf("pvId 0x%x\n", pParams->pvId);
	printf("bootMode 0x%x\n", pParams->bootMode);
}
#endif

#define CSD_HARNESS_TEST_GPV_SET_4_BYTES_ARRAY(value, size, function, pvname) \
{\
	if (size==4)\
	{\
		status = function (value);\
		if ( status==CSD_NO_ERROR )\
		{\
			unsigned int i;\
			printf("[CSD][M] - %s - 0x", #pvname);\
			for (i=0; i<size; i++ )\
			{\
				printf("%02x",value[i]);\
			}\
			printf(" - %s() done successfully.\n", #function);\
		}\
		else\
		{\
			CSD_ERR_TRACE (status, #function, "Operation failed.\n");\
			goto Clean_Up;\
		}\
	}\
	else\
	{\
		CSD_OTHER_ERR_TRACE ("Operation failed.\n");\
		goto Clean_Up;\
	}\
}

#define CSD_HARNESS_TEST_GPV_SET_ARRAY_FOR_SIZE(array, size, function, pvname) \
{\
	status = function (array, size);\
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

#define CSD_HARNESS_TEST_GPV_SET_4_BYTES_VALUE(value, size, function, pvname) \
{\
	if (size==4)\
	{\
		unsigned int variable;\
		variable = (value[0]<<24) || (value[1]<<16) || (value[2]<<8) || (value[0]);\
		status = function (variable);\
		if ( status==CSD_NO_ERROR )\
		{\
			printf("[CSD][M] - %s - 0x%0x - %s() done successfully.\n", #pvname, variable, #function);\
		}\
		else\
		{\
			CSD_ERR_TRACE (status, #function, "Operation failed.\n");\
			goto Clean_Up;\
		}\
	}\
	else\
	{\
		CSD_OTHER_ERR_TRACE ("Operation failed.\n");\
		goto Clean_Up;\
	}\
}

#define CSD_HARNESS_TEST_GPV_SET_CONSTANT(value, function, string, pvname) \
{\
	status = function (value);\
	if ( status==CSD_NO_ERROR )\
	{\
		printf("[CSD][M] - %s - %s - %s() done successfully.\n", #pvname, string[value], #function);\
	}\
	else\
	{\
		CSD_ERR_TRACE (status, #function, "Operation failed.\n");\
		goto Clean_Up;\
	}\
}

#define CSD_HARNESS_TEST_GPV_SET_NO_VALUE(function, pvname) \
{\
	status = function ();\
	if ( status==CSD_NO_ERROR )\
	{\
		printf("[CSD][M] - %s - %s - %s() done successfully.\n", #pvname, "SET", #function);\
	}\
	else\
	{\
		CSD_ERR_TRACE (status, #function, "Operation failed.\n");\
		goto Clean_Up;\
	}\
}

#define CSD_HARNESS_TEST_SPV_SET_SCSPV(pvid, array, size, function, pvname) \
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

void csd_harness_test_spv (int argc, char* argv[])
{
	CSD_Harness_Test_SPV_GPV_Parameters params;
	NOCS3_HARNESS_TEST_STATUS_CODE  nocs3_status;
 	TCsdInitParameters csdInit;
	TCsdStatus status;
	TCsdTerminateParameters csdTerminateHandle; 
	NEXUS_PlatformSettings platformSettings;

	if ( (argc <3) || (strcmp (argv[2], NOCS3_HARNESS_TEST_OPTION_HELP)==0 ) )
	{
		csd_harness_test_spv_help();
		return;
	}

	memset (&params, 0, sizeof (CSD_Harness_Test_SPV_GPV_Parameters));
	if ( (nocs3_status=csd_harness_test_spv_gpv_parse_cmd_line (argc, argv, &params, true)) != NOCS3_HARNESS_TEST_STATUS_CODE_SUCCESS )
	{
		/* Parser error code */
		CSD_OTHER_ERR_TRACE ("SPV command parser failed.  Please make sure the command line parameters are valid");
		goto Clean_Up_1;
	}

#ifdef NOCS3_DEBUG_MESSAGE
	csd_harness_test_spv_dump_paramters (&params);
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
		CSD_HARNESS_TEST_GPV_SET_4_BYTES_ARRAY(params.pvValue, params.pvSize, csdSetMarketSegmentId, MSID);
		break;

	case NOCS3_HARNESS_TEST_PV_NAME_STBCASN:
		CSD_HARNESS_TEST_GPV_SET_4_BYTES_ARRAY(params.pvValue, params.pvSize, csdSetStbCaSn, STBCASN);
		break;

	case NOCS3_HARNESS_TEST_PV_NAME_SCSPV:
		CSD_HARNESS_TEST_SPV_SET_SCSPV(params.pvId, params.pvValue, params.pvSize, csdSetScsPv, SCSPV);
		break;

	case NOCS3_HARNESS_TEST_PV_NAME_BOOTMODE:
		{
			ECsdBootMode bootMode;
			switch (params.bootMode)
			{
			case NOCS3_HARNESS_TEST_BOOT_MODE_NAND:
				bootMode = CSD_BOOT_FROM_NAND;
				break;
			case NOCS3_HARNESS_TEST_BOOT_MODE_NOR:
				bootMode = CSD_BOOT_FROM_NOR;
				break;
			case NOCS3_HARNESS_TEST_BOOT_MODE_SPI:
				bootMode = CSD_BOOT_FROM_SPI;
				break;
			case NOCS3_HARNESS_TEST_BOOT_MODE_EMMC:
				bootMode = CSD_BOOT_FROM_EMMC;
				break;
			default:
				bootMode = CSD_BOOT_MODE_MAX;
				break;

			}
			if (bootMode != CSD_BOOT_MODE_MAX)
			{
				CSD_HARNESS_TEST_GPV_SET_CONSTANT(bootMode, csdSetBootMode, gHarnessTestBootModeString, BootMode);
			}
			else
			{
				CSD_OTHER_ERR_TRACE ("Invalid flash type specified.");
				goto Clean_Up;
			}
		}
		break;

	case NOCS3_HARNESS_TEST_PV_NAME_SCS:
		CSD_HARNESS_TEST_GPV_SET_NO_VALUE(csdEnableScs, ScsEnable);
		break;

	case NOCS3_HARNESS_TEST_PV_NAME_DIFPWD:
		CSD_HARNESS_TEST_GPV_SET_CONSTANT(CSD_DEBUG_INTERFACE_ACCESS_MODE_PASSWORD_PROTECTED, csdSelectDebugInterfaceProtectionLevel, gHarnessTestDebugInterfaceStatusString, DbgIfProtectionLevel);
		break;

	case NOCS3_HARNESS_TEST_PV_NAME_DIFCLOSE:
		CSD_HARNESS_TEST_GPV_SET_CONSTANT(CSD_DEBUG_INTERFACE_ACCESS_MODE_CLOSED, csdSelectDebugInterfaceProtectionLevel, gHarnessTestDebugInterfaceStatusString, DbgIfProtectionLevel);
		break;

	case NOCS3_HARNESS_TEST_PV_NAME_DIFOPEN:
		CSD_HARNESS_TEST_GPV_SET_CONSTANT(CSD_DEBUG_INTERFACE_ACCESS_MODE_OPEN, csdSelectDebugInterfaceProtectionLevel, gHarnessTestDebugInterfaceStatusString, DbgIfProtectionLevel);
		break;

	case NOCS3_HARNESS_TEST_PV_NAME_KLD:
		CSD_HARNESS_TEST_GPV_SET_NO_VALUE(csdDeactivateKeyLadder, KLDeactiveFlag);
		break;

	case NOCS3_HARNESS_TEST_PV_NAME_RAM:
		CSD_HARNESS_TEST_GPV_SET_NO_VALUE(csdEnableRamScrambling, RamScrambling);
		break;

	case NOCS3_HARNESS_TEST_PV_NAME_FLASH:
		CSD_HARNESS_TEST_GPV_SET_NO_VALUE(csdEnableFlashProtection, FlashProtection);
		break;

	case NOCS3_HARNESS_TEST_PV_NAME_RAMUNSCRAMBSIZE:
		CSD_HARNESS_TEST_GPV_SET_ARRAY_FOR_SIZE(params.pvValue, params.pvSize, csdSetRamUnscrambledStorageSize, RAMUNSCRAMBSIZE);
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


static void csd_harness_test_spv_help (void)
{
	printf("\t%s\n\n", CSD_HELP_M_SPV_INST);
	printf("\t\t%s\n\n", CSD_HELP_M_SPV_V);
	printf("\t\t%s\n\n", CSD_HELP_M_SPV_S);
	printf("\t\t%s\n\n", CSD_HELP_M_SPV_PVID);
}


NOCS3_HARNESS_TEST_STATUS_CODE csd_harness_test_spv_gpv_parse_cmd_line (int argc, 
																		char* argv[], 
																		CSD_Harness_Test_SPV_GPV_Parameters * pParams,
																		bool bIsSpv)
{

	int cmdline_offset = 2;
	bool bPvNameParsered = false;
	bool bPvValueParsered = false;
	bool bPvSizeParsered = false;
	bool bPvIdParsered = false;
	bool bBootModeParsered = false;

	if ( pParams==NULL )
		return NOCS3_HARNESS_TEST_STATUS_CODE_FAIL;

	pParams->pvName = NOCS3_HARNESS_TEST_PV_NAME_MAX;
	pParams->bootMode = NOCS3_HARNESS_TEST_BOOT_MODE_MAX;
	pParams->pvId = NOCS3_HARNESS_TEST_SCSPV_ID_MAX;

	/* command parser state machine */
	do 
	{
		NOCS3_HARNESS_TEST_PARSER_STATUS_CODE status;
		bool bCommandProcessed = false;

		/* Pv name */
		NOCS3_HARNESS_TEST_PARSE_PV_NAME;

		/* value */
		if (bIsSpv==true)
		{
			/* NOCS3_COMMAND_PARSER_PARSE_NUMBER_OPTIONAL (bPvValueParsered, NOCS3_HARNESS_TEST_OPTION_VALUE, &pParams->pvValue, "0x%x");*/
			NOCS3_COMMAND_PARSER_PARSE_ARRAY_OPTIONAL_2(bPvValueParsered, \
														NOCS3_HARNESS_TEST_OPTION_VALUE, \
														&pParams->pvValue, \
														&pParams->pvSize);

			NOCS3_HARNESS_TEST_PARSE_BOOT_MODE;
		}

		/* Size */
		NOCS3_COMMAND_PARSER_PARSE_NUMBER_OPTIONAL (bPvSizeParsered, NOCS3_HARNESS_TEST_OPTION_SIZE, &pParams->pvSize, "0x%x");

		/* ID value */
#if 0
		NOCS3_COMMAND_PARSER_PARSE_NUMBER_OPTIONAL (bPvIdParsered, NOCS3_HARNESS_TEST_OPTION_ID, &pParams->pvId, "0x%x");
#else
		NOCS3_COMMAND_PARSER_PARSE_SCSPV_ID_OPTIONAL;
#endif

		NOCS3_COMMAND_PARSER_CHECK_IF_COMMAND_PARSED;

	} while ( cmdline_offset < argc );

	if (bPvValueParsered)
	{
		bPvSizeParsered=true;
	}

	if ( ( pParams->pvSize !=0 ) && (pParams->pvValue==NULL) )
	{
		/* Allocate array for GPV that may require output */
		pParams->pvValue = malloc(pParams->pvSize*sizeof(unsigned char));
	}

	if (bIsSpv==true)
	{
		if  ( (bPvNameParsered==false) ||\
			( ( pParams->pvName==NOCS3_HARNESS_TEST_PV_NAME_BOOTMODE) && (pParams->bootMode==NOCS3_HARNESS_TEST_BOOT_MODE_MAX)) ||\
			( ( (pParams->pvName==NOCS3_HARNESS_TEST_PV_NAME_MSID) || (pParams->pvName==NOCS3_HARNESS_TEST_PV_NAME_STBCASN)|| \
				(pParams->pvName==NOCS3_HARNESS_TEST_PV_NAME_RAMUNSCRAMBSIZE) ) && (bPvValueParsered==false)) ||\
			( ( pParams->pvName==NOCS3_HARNESS_TEST_PV_NAME_SCSPV) && \
			((bPvValueParsered==false) || (bPvSizeParsered==false) || (bPvIdParsered==false)))		
			)
		{
			return NOCS3_HARNESS_TEST_STATUS_CODE_BAD_PARAMS;
		}
	}
	else
	{
		if  ( bPvNameParsered==false )		
		{
			return NOCS3_HARNESS_TEST_STATUS_CODE_BAD_PARAMS;
		}
	}

	return NOCS3_HARNESS_TEST_STATUS_CODE_SUCCESS;

}



