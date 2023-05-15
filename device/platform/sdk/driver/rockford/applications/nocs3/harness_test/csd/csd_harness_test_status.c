#include <bstd.h>
#include <stdio.h>
#include <string.h>

#include "nocs_csd_impl.h"
#include "nocs_csd.h"

#include "csd_harness_test.h"
#include "csd_harness_test_parser.h"
#include "csd_harness_test_status.h"
#include "csd_harness_test_status_private.h"
#include "csd_harness_test_utilities.h"
#include "nocs3_harness_test_utilities.h"
#include "nexus_platform.h"
#include "csd_harness_test_spv_gpv_private.h"

#define CSD_HARNESS_TEST_STATUS_DUMP_STRING(fp, id, field, value) \
	fprintf(fp, "[M%02d]-%s - %s - \n", id, field, value);\

#define CSD_HARNESS_TEST_STATUS_DUMP_NUMBER(fp, id, field, value) \
	fprintf(fp, "[M%02d]-%s - 0x%02x - \n", id, field, value);\

#define CSD_HARNESS_TEST_STATUS_DUMP_ARRAY(fp, id, field, array, size) \
	{\
		unsigned int i;\
		fprintf(fp, "[M%02d]-%s - 0x", id, field);\
		for (i=0;i<size;i++)\
			fprintf(fp, "%02x", array[i]);\
		fprintf(fp, " - \n");\
	}
#define CSD_HARNESS_TEST_GET_VERSION(field, msg)\
	status = csdGetScsPv(CSD_SCS_PV_VERSIONING_REF, NULL, CSD_HARNESS_TEST_STATUS_VERSION_SIZE, field);\
	if (status!=CSD_NO_ERROR) \
	{\
		CSD_ERR_TRACE (status, "csdGetScsPv", msg);\
		return status;\
	}\


#define CSD_HARNESS_TEST_STATUS_CHIP_MANUFACTURER					"Chipset Manufacturer"
#define CSD_HARNESS_TEST_STATUS_CHIP_NAME							"Chipset Name"
#define CSD_HARNESS_TEST_STATUS_CHIP_REVISION						"Chip Revision"
#define CSD_HARNESS_TEST_STATUS_CHIP_SW_VERSION						"CSD Software Version"
#define CSD_HARNESS_TEST_STATUS_CHIP_EXTENSION						"Chip Extension"
#define CSD_HARNESS_TEST_STATUS_NUID								"NUID"
#define CSD_HARNESS_TEST_STATUS_NUID_CN								"NUID Check Number"
#define CSD_HARNESS_TEST_STATUS_MARKET_ID							"Market Segment ID"
#define CSD_HARNESS_TEST_STATUS_BOOT_MODE							"Boot Mode"
#define CSD_HARNESS_TEST_STATUS_VERSIONING							"Versioning"
#define CSD_HARNESS_TEST_STATUS_SCS_ACTIVATION_FLAG					"SCS Activation Flag"
#define CSD_HARNESS_TEST_STATUS_STB_CA_SN							"STB CA Serial Number"
#define CSD_HARNESS_TEST_STATUS_DEBUG_IF_PL							"Debug Interface Protection Level"
#define CSD_HARNESS_TEST_STATUS_KL_DEACTIVATION_FLAG				"Key Ladder Deactivation Flag"
#define CSD_HARNESS_TEST_STATUS_RAMSCRAM_ACTIVATION_FLAG			"Ram Scrambling Activation Flag"
#define CSD_HARNESS_TEST_STATUS_FP_ACTIVATION_FLAG					"Flash Protection Activation Flag"
#define CSD_HARNESS_TEST_STATUS_DVRKEY								"DvrKey"
#define CSD_HARNESS_TEST_STATUS_CHIP_CDS_VERSION					"CSD API Version"

#define CSD_HARNESS_TEST_STATUS_VERSION_SIZE						1
typedef struct CSD_Harness_Test_Status
{
	TCsd20CharsString					chipName;
	TCsd20CharsString					softwareVersion;
	TCsd20CharsString					chipRevision;
	TCsd20CharsString					chipExtension;
	TCsd8BytesVector					nuid;
	TCsd4BytesVector					nuidCheckNumber;
	TCsd4BytesVector					marketID;
	ECsdBootMode						bootMode;
	TCsdUnsignedInt8					versioning[CSD_HARNESS_TEST_STATUS_VERSION_SIZE];
	TCsdActivationFlag					scsActivationFlag;
	TCsd4BytesVector					stbCaSn;
	TCsdDebugInterfaceAccessMode		debugIfAccessMode;
	TCsdActivationFlag	keyladderActivationFlag;
	TCsdActivationFlag	ramScramActivationFlag;
	TCsdActivationFlag	flashProtectionActivationFlag;
	unsigned char 						pvrKey[16];
	TCsd20CharsString					apiVersion;

} CSD_Harness_Test_Status;

static void csd_harness_test_status_help (void);
static NOCS3_HARNESS_TEST_STATUS_CODE csd_harness_test_status_parse_cmd_line (int argc, 
																		char* argv[], 
																		CSD_Harness_Test_STATUS_Parameters * pParams);

static TCsdStatus csd_harness_test_get_status (CSD_Harness_Test_Status * pStatus);
static void csd_harness_test_print_status (FILE * pFile, CSD_Harness_Test_Status * pStatus);


#ifdef NOCS3_DEBUG_MESSAGE
static void csd_harness_test_status_dump_paramters (CSD_Harness_Test_STATUS_Parameters * pParams)
{
	printf("CSD_Harness_Test_STATUS_Parameters is :\n");

	printf("pOutFile %p\n", (void *)pParams->pOutFile);
}
#endif						

static const char gDebugIfAccessModeString[3][16] =
{
	"OPEN",
	"PWD",
	"CLOSED"
};


static void csd_harness_test_print_status (FILE * pFile, CSD_Harness_Test_Status * pStatus)
{
	fprintf(pFile, "\n");
	fprintf(pFile, "[CSD][M] Status done successfully.\n\n");

	CSD_HARNESS_TEST_STATUS_DUMP_STRING (pFile, 1, CSD_HARNESS_TEST_STATUS_CHIP_MANUFACTURER, "Broadcom Corporation.");
	CSD_HARNESS_TEST_STATUS_DUMP_STRING (pFile, 2, CSD_HARNESS_TEST_STATUS_CHIP_NAME, pStatus->chipName);
	CSD_HARNESS_TEST_STATUS_DUMP_STRING (pFile, 3, CSD_HARNESS_TEST_STATUS_CHIP_SW_VERSION, pStatus->softwareVersion);
	CSD_HARNESS_TEST_STATUS_DUMP_STRING (pFile, 4, CSD_HARNESS_TEST_STATUS_CHIP_REVISION, pStatus->chipRevision);
	CSD_HARNESS_TEST_STATUS_DUMP_STRING (pFile, 5, CSD_HARNESS_TEST_STATUS_CHIP_EXTENSION, pStatus->chipExtension);
	CSD_HARNESS_TEST_STATUS_DUMP_ARRAY (pFile, 6, CSD_HARNESS_TEST_STATUS_NUID, pStatus->nuid, sizeof(pStatus->nuid)) ;
	CSD_HARNESS_TEST_STATUS_DUMP_ARRAY (pFile, 7, CSD_HARNESS_TEST_STATUS_NUID_CN, pStatus->nuidCheckNumber, sizeof(pStatus->nuidCheckNumber));
	CSD_HARNESS_TEST_STATUS_DUMP_ARRAY (pFile, 8, CSD_HARNESS_TEST_STATUS_MARKET_ID, pStatus->marketID, sizeof(pStatus->marketID)) ;
	CSD_HARNESS_TEST_STATUS_DUMP_STRING (pFile, 9, CSD_HARNESS_TEST_STATUS_BOOT_MODE, gHarnessTestBootModeString[pStatus->bootMode]);
	CSD_HARNESS_TEST_STATUS_DUMP_ARRAY (pFile, 10, CSD_HARNESS_TEST_STATUS_VERSIONING, pStatus->versioning, sizeof(pStatus->versioning));
	CSD_HARNESS_TEST_STATUS_DUMP_STRING (pFile, 11, CSD_HARNESS_TEST_STATUS_SCS_ACTIVATION_FLAG, \
		((pStatus->scsActivationFlag==CSD_ACTIVATION_FLAG_SET)? "SET":"NOT SET"));
	CSD_HARNESS_TEST_STATUS_DUMP_ARRAY (pFile, 12, CSD_HARNESS_TEST_STATUS_STB_CA_SN, pStatus->stbCaSn, sizeof(pStatus->stbCaSn)) ;
	CSD_HARNESS_TEST_STATUS_DUMP_STRING (pFile, 13, CSD_HARNESS_TEST_STATUS_DEBUG_IF_PL, gDebugIfAccessModeString[pStatus->debugIfAccessMode]);
	CSD_HARNESS_TEST_STATUS_DUMP_STRING (pFile, 14, CSD_HARNESS_TEST_STATUS_KL_DEACTIVATION_FLAG, \
		((pStatus->keyladderActivationFlag==CSD_ACTIVATION_FLAG_SET)? "SET":"NOT SET"));
	CSD_HARNESS_TEST_STATUS_DUMP_STRING (pFile, 15, CSD_HARNESS_TEST_STATUS_RAMSCRAM_ACTIVATION_FLAG, \
		((pStatus->ramScramActivationFlag==CSD_ACTIVATION_FLAG_SET)? "SET":"NOT SET"));
	CSD_HARNESS_TEST_STATUS_DUMP_STRING (pFile, 16, CSD_HARNESS_TEST_STATUS_FP_ACTIVATION_FLAG, \
		((pStatus->flashProtectionActivationFlag==CSD_ACTIVATION_FLAG_SET)? "SET":"NOT SET"));

	CSD_HARNESS_TEST_STATUS_DUMP_ARRAY (pFile, 17, CSD_HARNESS_TEST_STATUS_DVRKEY, pStatus->pvrKey, sizeof(pStatus->pvrKey)) ;
	CSD_HARNESS_TEST_STATUS_DUMP_STRING (pFile, 18, CSD_HARNESS_TEST_STATUS_CHIP_CDS_VERSION, pStatus->apiVersion);
	fprintf(pFile, "\n");

}


void csd_harness_test_status (int argc, char* argv[])
{
	CSD_Harness_Test_STATUS_Parameters params;
	NOCS3_HARNESS_TEST_STATUS_CODE  nocs3_status;
	NEXUS_PlatformSettings platformSettings;
 	TCsdInitParameters csdInit;
	TCsdStatus status;
	TCsdTerminateParameters csdTerminateHandle; 
	CSD_Harness_Test_Status statusInfo;

	if ( (argc <3) || (strcmp (argv[2], "help")==0 ) )
	{
		csd_harness_test_status_help();
		return;
	}

	memset (&params, 0, sizeof (CSD_Harness_Test_STATUS_Parameters));
	if ( (nocs3_status=csd_harness_test_status_parse_cmd_line (argc, argv, &params)) != NOCS3_HARNESS_TEST_STATUS_CODE_SUCCESS )
	{
		/* Parser error code */
		CSD_OTHER_ERR_TRACE ("Status command parser failed.  Please make sure the command line parameters are valid");
		goto Clean_Up;
	}

#ifdef NOCS3_DEBUG_MESSAGE
	csd_harness_test_status_dump_paramters (&params);
#endif

	/* Platform init */
    NEXUS_Platform_GetDefaultSettings(&platformSettings); 
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);

	/* csd_harness_test_scr_dump_paramters (&params);*/
	memset (&csdInit, 0, sizeof(TCsdInitParameters));
	
	status=csdInitialize(&csdInit);
	if( (status!=CSD_NO_ERROR) && (status !=CSD_ERROR_DRIVER_ALREADY_INITIALIZED) )
	{
		CSD_ERR_TRACE (status, "csdInitialize", "Operation failed");
		goto Clean_Up_1;
	}	

	memset (&statusInfo, 0, sizeof(statusInfo));

	/* Get status info */
	status = csd_harness_test_get_status (&statusInfo);
	if (status!=CSD_NO_ERROR) 
	{
		CSD_OTHER_ERR_TRACE("Operation failed.");
		goto Clean_Up;
	}	

	/* Print status info */
	csd_harness_test_print_status (((params.pOutFile!=NULL)? params.pOutFile: stdout), &statusInfo);

	/* Clean up */
Clean_Up:
	csdTerminateHandle.initParam = &csdInit; 
    csdTerminateHandle.keyHandleToRelease = NULL; 
    csdTerminate(&csdTerminateHandle);

Clean_Up_1:
	if (params.pOutFile)
	{
		fclose (params.pOutFile);
	}

    fflush(stdout);
    NEXUS_Platform_Uninit();

}



static TCsdStatus csd_harness_test_get_status (CSD_Harness_Test_Status * pStatus)
{
	TCsdStatus status;
	NEXUS_PlatformStatus nexusPlatformStatus;

	/* Get chip ID and revision */
	if ( NEXUS_Platform_GetStatus(&nexusPlatformStatus) != NEXUS_SUCCESS )
	{
		status= CSD_ERROR;
		return status;
	}
	sprintf((char *)pStatus->chipName, "BCM%x", nexusPlatformStatus.chipId);
#if 0
	sprintf((char *)pStatus->chipRevision, "%c%d", (nexusPlatformStatus.chipRevision>>4)+'A', (nexusPlatformStatus.chipRevision)&0xF );
#else
	CSD_HARNESS_TEST_GET_STATUS (csdGetChipRevision, pStatus->chipRevision, "Cannot get chip revision");
#endif

	strcpy (pStatus->softwareVersion, CSD_APP_VERSION);
	CSD_HARNESS_TEST_GET_STATUS (csdGetChipExtension, pStatus->chipExtension, "Cannot get chip extension");
	CSD_HARNESS_TEST_GET_STATUS (csdGetNuid64, pStatus->nuid, "Cannot get NUID");
	CSD_HARNESS_TEST_GET_STATUS_IGNORE_ERROR (csdGetNUIDCheckNumber, pStatus->nuidCheckNumber, "Cannot get NUID check number");
	CSD_HARNESS_TEST_GET_STATUS (csdGetMarketSegmentId, pStatus->marketID, "Cannot get NUID check number");
	CSD_HARNESS_TEST_GET_STATUS (csdGetBootMode, &pStatus->bootMode, "Cannot get Boot Mode");
	CSD_HARNESS_TEST_GET_STATUS (csdGetScsActivationFlag, &pStatus->scsActivationFlag, "Cannot get SCS Activation Flag");
	CSD_HARNESS_TEST_GET_STATUS (csdGetStbCaSn, pStatus->stbCaSn, "Cannot get STB Serial Number");
	CSD_HARNESS_TEST_GET_STATUS (csdGetDebugInterfaceProtectionLevel, &pStatus->debugIfAccessMode, "Cannot get Debug Interface Access Mode");
	CSD_HARNESS_TEST_GET_STATUS (csdGetKeyLadderDeactivationFlag, &pStatus->keyladderActivationFlag, "Cannot get Keyladder Activation Flag");
	CSD_HARNESS_TEST_GET_STATUS (csdGetScramblingActivationFlag, &pStatus->ramScramActivationFlag, "Cannot get Ram Scrambling  Activation Flag");
	CSD_HARNESS_TEST_GET_STATUS (csdGetFlashProtectionActivationFlag, &pStatus->flashProtectionActivationFlag, "Cannot get Flash Protection Activation Flag");
	CSD_HARNESS_TEST_GET_STATUS_IGNORE_ERROR (csdGetDvrKey, pStatus->pvrKey, "Cannot get PVR key");
	CSD_HARNESS_TEST_GET_STATUS (csdGetSoftwareVersion, pStatus->apiVersion, "Cannot get CSD API version");

	CSD_HARNESS_TEST_GET_VERSION (pStatus->versioning, "Cannot versioning");
	return CSD_NO_ERROR;
}




static void csd_harness_test_status_help (void)
{
	printf("\t%s\n\n", CSD_HELP_M_STATUS_INST);
	printf("\t\t%s\n\n", CSD_HELP_M_STATUS_OFOC);
}

static NOCS3_HARNESS_TEST_STATUS_CODE csd_harness_test_status_parse_cmd_line (int argc, 
																		char* argv[], 
																		CSD_Harness_Test_STATUS_Parameters * pParams)
{
	int cmdline_offset = 2;
	bool bOcParsered = false;
	bool bOfParsered = false;

	/* command parser state machine */
	do 
	{
		NOCS3_HARNESS_TEST_PARSER_STATUS_CODE status;
		bool bCommandProcessed = false;

		/* Oc */
		NOCS3_COMMAND_PARSER_PARSE_SINGLE_OPTIONAL (bOcParsered, NOCS3_HARNESS_TEST_OPTION_OC);
		
		/* Of */
		NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bOfParsered, NOCS3_HARNESS_TEST_OPTION_OF, &pParams->pOutFile, "w");

		NOCS3_COMMAND_PARSER_CHECK_IF_COMMAND_PARSED;

	} while ( cmdline_offset < argc );

	if ( (bOfParsered==true) && (bOcParsered==true)) 
	{
		if (pParams->pOutFile!=NULL)
		{
			fclose (pParams->pOutFile);
			pParams->pOutFile = NULL;
		}
		return NOCS3_HARNESS_TEST_STATUS_CODE_BAD_PARAMS;	
	}
	
	return NOCS3_HARNESS_TEST_STATUS_CODE_SUCCESS;
}

