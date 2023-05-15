/***************************************************************************
*     (c)2008-2011 Broadcom Corporation
*  
*  This program is the proprietary software of Broadcom Corporation and/or its licensors,
*  and may only be used, duplicated, modified or distributed pursuant to the terms and
*  conditions of a separate, written license agreement executed between you and Broadcom
*  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
*  no license (express or implied), right to use, or waiver of any kind with respect to the
*  Software, and Broadcom expressly reserves all rights in and to the Software and all
*  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
*  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
*  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.  
*   
*  Except as expressly set forth in the Authorized License,
*   
*  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
*  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
*  and to use this information only in connection with your use of Broadcom integrated circuit products.
*   
*  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" 
*  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR 
*  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO 
*  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES 
*  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, 
*  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION 
*  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF 
*  USE OR PERFORMANCE OF THE SOFTWARE.
*  
*  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS 
*  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR 
*  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR 
*  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF 
*  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT 
*  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE 
*  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF 
*  ANY LIMITED REMEDY.
* 
* $brcm_Workfile: sec_harness_test_sha.c $
* $brcm_Revision: Hydra_Software_Devel/2 $
* $brcm_Date: 10/5/12 10:51a $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /rockford/applications/nocs3/harness_test/sec/sec_harness_test_sha.c $
* 
* Hydra_Software_Devel/2   10/5/12 10:51a qxie
* SW7346-555:Nocs3 integration
* 
* 
**************************************************************************/
#include <bstd.h>
#include <stdio.h>
#include <string.h>

#include "ca_sec.h"
#include "ca_sec_impl.h"


#include "sec_harness_test.h"
#include "sec_harness_test_sha.h"
#include "sec_harness_test_sha_private.h"
#include "sec_harness_test_utilities.h"
#include "nocs3_harness_test_utilities.h"
#include "nexus_platform.h"
#include "nexus_hmac_sha_cmd.h"


static void sec_harness_test_sha_help (void);

static NOCS3_HARNESS_TEST_STATUS_CODE sec_harness_test_sha_parse_cmd_line (int argc, 
																		char* argv[], 
																		SEC_Harness_Test_Sha_Parameters * pParams);

#ifdef NOCS3_DEBUG_MESSAGE
static void sec_harness_test_sha_dump_paramters (SEC_Harness_Test_Sha_Parameters * pParams)
{
	printf("SEC_Harness_Test_Sha_Parameters is :\n");
	printf("operation: %08x\n", pParams->operation);
	printf("pInFile: %08x\n", pParams->pInFile);
	printf("pOutFile: %08x\n", pParams->pOutFile);
}
#endif

void sec_harness_test_sha (int argc, char* argv[])
{
	SEC_Harness_Test_Sha_Parameters params;
	NOCS3_HARNESS_TEST_STATUS_CODE  nocs3_status;
	NEXUS_PlatformSettings platformSettings;
	TSecFunctionTable *functionTable;
	TSecHashContext shaCtx;
	unsigned char out_data[SEC_HARNESS_TEST_SEC_MAX_DATA_SIZE]={0};
	TSecStatus status;
	size_t buffer_size; 
	unsigned int out_size = 0;
	if ( (argc <3) || (strcmp (argv[2], NOCS3_HARNESS_TEST_OPTION_HELP)==0 ) )
	{
		sec_harness_test_sha_help();
		return;
	}

	memset (&params, 0, sizeof (SEC_Harness_Test_Sha_Parameters));
	if ( (nocs3_status=sec_harness_test_sha_parse_cmd_line (argc, argv, &params)) != NOCS3_HARNESS_TEST_STATUS_CODE_SUCCESS )
	{
		/* Parser error code */
		SEC_OTHER_ERR_TRACE ("Ram command parser failed.  Please make sure the command line parameters are valid");
		return;
	}

#ifdef NOCS3_DEBUG_MESSAGE
	sec_harness_test_sha_dump_paramters (&params);
#endif
	if(params.pInFile == NULL) {
		
		printf("[SEC][EX] Input file size is 0\n");
	}
	/* Platform init */
    NEXUS_Platform_GetDefaultSettings(&platformSettings); 
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
	functionTable = secGetFunctionTable();

	if(params.operation == SEC_HARNESS_TEST_SHA_OPERATION_SHA1){
		status = functionTable->secSha1Init(&shaCtx);
		if(status!=SEC_NO_ERROR){
			printf("[SEC][EX] secSha1Init error %d\n", status);
			return;
		}
		#if 0
		status = functionTable->secSha1Update(shaCtx, params.data, params.sizeData); 
		if(status!=SEC_NO_ERROR){
			printf("[SEC][EX] secSha1Update error %d\n", status);
			return;
		}

		
		status = functionTable->secSha1Final(shaCtx, out_data); 
		if(status!=SEC_NO_ERROR){
			printf("[SEC][EX] secSha1Final error %d\n", status);
			return;
		}

		if(params.pOutFile){
		fwrite((void *) out_data, sizeof(unsigned char), 20, params.pOutFile);
		fclose(params.pOutFile);
		}
		
#endif
	}else if (params.operation == SEC_HARNESS_TEST_SHA_OPERATION_SHA256){
		status = functionTable->secSha256Init(&shaCtx);
		if(status!=SEC_NO_ERROR){
			printf("[SEC][EX] secSha256Init error %d\n", status);
			return;
		}
#if 0
		status = functionTable->secSha256Update(shaCtx, params.data, params.sizeData); 
		if(status!=SEC_NO_ERROR){
			printf("[SEC][EX] secSha256Update error %d\n", status);
			return;
		}

		status = functionTable->secSha256Final(shaCtx, out_data); 
		if(status!=SEC_NO_ERROR){
			printf("[SEC][EX] secSha256Final error %d\n", status);
			return;
		}
		if(params.pOutFile){
			fwrite((void *) out_data, sizeof(unsigned char), 32, params.pOutFile);
			fclose(params.pOutFile);
		}
		
#endif
	}
	else if (params.operation == SEC_HARNESS_TEST_SHA_OPERATION_SHA384){
			status = functionTable->secSha384Init(&shaCtx);
			if(status!=SEC_NO_ERROR){
				printf("[SEC][EX] secSha384Init error %d\n", status);
				return;
			}
#if 0
			status = functionTable->secSha384Update(shaCtx, params.data, params.sizeData); 
			if(status!=SEC_NO_ERROR){
				printf("[SEC][EX] secSha384Update error %d\n", status);
				return;
			}
			status = functionTable->secSha384Final(shaCtx, out_data); 
			if(status!=SEC_NO_ERROR){
				printf("[SEC][EX] secSha384Final error %d\n", status);
				return;
			}
			if(params.pOutFile){
				fwrite((void *) out_data, sizeof(unsigned char), 48, params.pOutFile);
				fclose(params.pOutFile);
			}
			
#endif	
		}


	shaCtx->datamode = SEC_SHA_DATA_MODE_eAllin;


	do{
		
		params.sizeData = fread (&params.data[0], sizeof(unsigned char), SEC_HARNESS_TEST_SEC_MAX_DATA_SIZE, params.pInFile);
		if( params.sizeData ==SEC_HARNESS_TEST_SEC_MAX_DATA_SIZE )shaCtx->datamode = SEC_SHA_DATA_MODE_eMore;
		else if(shaCtx->datamode != SEC_SHA_DATA_MODE_eAllin)shaCtx->datamode = SEC_SHA_DATA_MODE_eLast;

	if(params.operation == SEC_HARNESS_TEST_SHA_OPERATION_SHA1){
		status = functionTable->secSha1Update(shaCtx, params.data, params.sizeData); 
		if(status!=SEC_NO_ERROR){
			printf("[SEC][EX] secSha1Update error %d\n", status);
			return;
		}

		
		status = functionTable->secSha1Final(shaCtx, out_data); 
		if(status!=SEC_NO_ERROR){
			printf("[SEC][EX] secSha1Final error %d\n", status);
			return;
		}
		out_size = 20;

	}else if (params.operation == SEC_HARNESS_TEST_SHA_OPERATION_SHA256){

		status = functionTable->secSha256Update(shaCtx, params.data, params.sizeData); 
		
		if(status!=SEC_NO_ERROR){
			printf("[SEC][EX] secSha256Update error %d\n", status);
			return;
		}
		
		status = functionTable->secSha256Final(shaCtx, out_data); 
		if(status!=SEC_NO_ERROR){
			printf("[SEC][EX] secSha256Final error %d\n", status);
			return;
		}
		out_size = 32;

	}
	else if (params.operation == SEC_HARNESS_TEST_SHA_OPERATION_SHA384){
			status = functionTable->secSha384Update(shaCtx, params.data, params.sizeData); 
			if(status!=SEC_NO_ERROR){
				printf("[SEC][EX] secSha384Update error %d\n", status);
				return;
			}
			 
		}
	if(params.sizeData < SEC_HARNESS_TEST_SEC_MAX_DATA_SIZE)break;

		}while(1);
	if (params.operation == SEC_HARNESS_TEST_SHA_OPERATION_SHA384){

	status = functionTable->secSha384Final(shaCtx, out_data); 
				if(status!=SEC_NO_ERROR){
					printf("[SEC][EX] secSha384Final error %d\n", status);
					return;
				}
				
				out_size = 48; 
		}
	if(params.pOutFile){
		fwrite((void *) out_data, sizeof(unsigned char), out_size, params.pOutFile);
		fclose(params.pOutFile);
	}

    printf ("[SEC][M] Sha test is done successfully.\n");

	
	/* Clean up */
	
	NEXUS_Memory_Free(functionTable);
    NEXUS_Platform_Uninit();

}


static void sec_harness_test_sha_help (void)
{
	printf("\t%s\n\n", SEC_HELP_M_SHA_INST);
	printf("\t%s\n\n", SEC_HELP_M_SHA_IF);
	printf("\t%s\n\n", SEC_HELP_M_SHA_OF);
}

static NOCS3_HARNESS_TEST_STATUS_CODE sec_harness_test_sha_parse_cmd_line (int argc, 
																		char* argv[], 
																		SEC_Harness_Test_Sha_Parameters * pParams)
{
	int cmdline_offset = 2;
	bool bopTypeParsered=false;
	bool bIfParsered = false;
	bool bOfParsered = false;
	
	do 
	{
		bool bCommandProcessed = false;
		NOCS3_HARNESS_TEST_PARSER_STATUS_CODE status;

		SEC_HARNESS_TEST_PARSE_SHA_TYPE;
		
		NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bIfParsered, NOCS3_HARNESS_TEST_OPTION_IF, &pParams->pInFile, "rb");
		NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bOfParsered, NOCS3_HARNESS_TEST_OPTION_OF, &pParams->pOutFile, "wb");
		NOCS3_COMMAND_PARSER_CHECK_IF_COMMAND_PARSED
	}
	while ( cmdline_offset < argc );

	
	return NOCS3_HARNESS_TEST_STATUS_CODE_SUCCESS;

}

