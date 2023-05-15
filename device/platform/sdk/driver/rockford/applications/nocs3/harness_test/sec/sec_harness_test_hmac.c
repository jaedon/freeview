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
* $brcm_Workfile: sec_harness_test_hmac.c $
* $brcm_Revision: Hydra_Software_Devel/2 $
* $brcm_Date: 10/5/12 10:51a $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /rockford/applications/nocs3/harness_test/sec/sec_harness_test_hmac.c $
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
#include "sec_harness_test_hmac.h"
#include "sec_harness_test_hmac_private.h"
#include "sec_harness_test_utilities.h"
#include "nocs3_harness_test_utilities.h"
#include "nexus_platform.h"


static void sec_harness_test_hmac_help (void);

static NOCS3_HARNESS_TEST_STATUS_CODE sec_harness_test_hmac_parse_cmd_line (int argc, 
																		char* argv[], 
																		SEC_Harness_Test_Hmac_Parameters * pParams);

#ifdef NOCS3_DEBUG_MESSAGE
static void sec_harness_test_hmac_dump_paramters (SEC_Harness_Test_Hmac_Parameters * pParams)
{
	printf("SEC_Harness_Test_Hmac_Parameters is :\n");
	printf("Key:\n");
	nocs3_harness_test_utility_dump_array (pParams->key, SEC_HARNESS_TEST_HMAC_KEYSIZE);
	printf("pInFile: %08x\n", pParams->pInFile);
	printf("pOutFile: %08x\n", pParams->pOutFile);
}
#endif

void sec_harness_test_hmac (int argc, char* argv[])
{
	SEC_Harness_Test_Hmac_Parameters params;
	NOCS3_HARNESS_TEST_STATUS_CODE  nocs3_status;
	NEXUS_PlatformSettings platformSettings;
	TSecFunctionTable *functionTable;
	TSecHashContext shaCtx;
	TSecStatus status;
	unsigned char out_data[SEC_HARNESS_TEST_SEC_MAX_DATA_SIZE]={0};
	
	unsigned char buf[SEC_HARNESS_TEST_SEC_MAX_DATA_SIZE] = {0}; 
	size_t buf_size; 
	if ( (argc <3) || (strcmp (argv[2], NOCS3_HARNESS_TEST_OPTION_HELP)==0 ) )
	{
		sec_harness_test_hmac_help();
		return;
	}

	memset (&params, 0, sizeof (SEC_Harness_Test_Hmac_Parameters));
	if ( (nocs3_status=sec_harness_test_hmac_parse_cmd_line (argc, argv, &params)) != NOCS3_HARNESS_TEST_STATUS_CODE_SUCCESS )
	{
		/* Parser error code */
		SEC_OTHER_ERR_TRACE ("Ram command parser failed.  Please make sure the command line parameters are valid");
		return;
	}

#ifdef NOCS3_DEBUG_MESSAGE
	sec_harness_test_hmac_dump_paramters (&params);
#endif

	/* Platform init */
    NEXUS_Platform_GetDefaultSettings(&platformSettings); 
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
	functionTable = secGetFunctionTable();
	status=functionTable->secHmacSha256Init(params.key, params.sizeKey, &shaCtx);
	if(status!=SEC_NO_ERROR){
		printf("[SEC][EX] secHmacSha256Init error %d\n", status);
			return;
			}
 
	
	shaCtx->datamode = SEC_SHA_DATA_MODE_eAllin;

	do{
		buf_size =  fread (&buf[0], sizeof(unsigned char), SEC_HARNESS_TEST_SEC_MAX_DATA_SIZE, params.pInFile);
		if(buf_size  ==SEC_HARNESS_TEST_SEC_MAX_DATA_SIZE )shaCtx->datamode = SEC_SHA_DATA_MODE_eMore;
		else if(shaCtx->datamode != SEC_SHA_DATA_MODE_eAllin)shaCtx->datamode = SEC_SHA_DATA_MODE_eLast;
		
		status=functionTable->secHmacSha256Update(shaCtx, buf, buf_size); 
		if(status!=SEC_NO_ERROR){
			printf("[SEC][EX] secHmacSha256Update error %d\n", status);
					return;
					}
		status=functionTable->secHmacSha256Final(shaCtx, out_data); 
		if(status!=SEC_NO_ERROR){
			printf("[SEC][EX] secHmacSha256Final error %d\n", status);
				return;
				}
		
		if(shaCtx->datamode !=SEC_SHA_DATA_MODE_eMore)break;
		} while(1);
	if(params.pOutFile){
		fwrite((void *) out_data, sizeof(unsigned char), 32, params.pOutFile);
		fclose(params.pOutFile);
	}
    printf ("[SEC][M] Hmac test is done successfully.\n");

	/* Clean up */
	
	NEXUS_Memory_Free(functionTable);
    NEXUS_Platform_Uninit();

}


static void sec_harness_test_hmac_help (void)
{
	printf("\t%s\n\n", SEC_HELP_M_HMAC_INST);
	printf("\t%s\n\n", SEC_HELP_M_HMAC_KEY);
	printf("\t%s\n\n", SEC_HELP_M_HMAC_IF);
	printf("\t%s\n\n", SEC_HELP_M_HMAC_OF);
}

static NOCS3_HARNESS_TEST_STATUS_CODE sec_harness_test_hmac_parse_cmd_line (int argc, 
																		char* argv[], 
																		SEC_Harness_Test_Hmac_Parameters * pParams)
{

	int cmdline_offset = 2;
	bool bopTypeParsered=false;
	bool bKfParsered = false;
	bool bIfParsered = false;
	bool bOfParsered = false;
	bool bKcParsered = false; 
	do 
	{
		bool bCommandProcessed = false;
		NOCS3_HARNESS_TEST_PARSER_STATUS_CODE status;

		SEC_HARNESS_TEST_PARSE_SHA_TYPE;
			/* Ic */
		NOCS3_COMMAND_PARSER_PARSE_ARRAY_OPTIONAL(bKcParsered, \
												NOCS3_HARNESS_TEST_OPTION_KC, \
												&pParams->key[0], \
												&pParams->sizeKey, \
												SEC_HARNESS_TEST_HMAC_KEYSIZE);
		
		NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bKfParsered, NOCS3_HARNESS_TEST_OPTION_KF, &pParams->pKeyFile, "rb");
		NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bIfParsered, NOCS3_HARNESS_TEST_OPTION_IF, &pParams->pInFile, "rb");
		NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bOfParsered, NOCS3_HARNESS_TEST_OPTION_OF, &pParams->pOutFile, "wb");
		NOCS3_COMMAND_PARSER_CHECK_IF_COMMAND_PARSED
	}
	while ( cmdline_offset < argc );

#if 0
	if (pParams->pInFile!=NULL)
	{
		pParams->sizeData = fread (&pParams->data[0], sizeof(unsigned char), SEC_HARNESS_TEST_SEC_MAX_DATA_SIZE, pParams->pInFile);
		fclose (pParams->pInFile);
	}
#endif
		if (pParams->pKeyFile!=NULL)
	{
		pParams->sizeKey = fread (&pParams->key[0], sizeof(unsigned char), SEC_HARNESS_TEST_SEC_MAX_DATA_SIZE, pParams->pKeyFile);
		fclose (pParams->pKeyFile);
	}

	return NOCS3_HARNESS_TEST_STATUS_CODE_SUCCESS;

}

