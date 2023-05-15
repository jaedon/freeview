
#include <bstd.h>
#include <stdio.h>
#include <string.h>

#include "ca_sec.h"

#include "sec_harness_test.h"
#include "sec_harness_test_rsa.h"
#include "sec_harness_test_rsa_private.h"
#include "sec_harness_test_utilities.h"
#include "nocs3_harness_test_utilities.h"
#include "nexus_platform.h"

static void sec_harness_test_rsa_help (void);

static NOCS3_HARNESS_TEST_STATUS_CODE sec_harness_test_rsa_parse_cmd_line (int argc, 
																		char* argv[], 
																		SEC_Harness_Test_Rsa_Parameters * pParams);

#ifdef NOCS3_DEBUG_MESSAGE
static void sec_harness_test_rsa_dump_paramters (SEC_Harness_Test_Rsa_Parameters * pParams)
{
	printf("SEC_Harness_Test_Rsa_Parameters is :\n");
}
#endif

void sec_harness_test_rsa (int argc, char* argv[])
{
	SEC_Harness_Test_Rsa_Parameters params;
	NOCS3_HARNESS_TEST_STATUS_CODE  nocs3_status;
	NEXUS_PlatformSettings platformSettings;
	unsigned long e;/* = {3, 17, 65537};*/
 	unsigned char n[1024] = {0}; 
	unsigned char p[1024] = {0}; 
	unsigned char q[1024] = {0}; 
	unsigned char dq[1024] = {0}; 
	unsigned char dp[1024] = {0};
	unsigned char qinv[1024] = {0};
	unsigned char out_data[SEC_HARNESS_TEST_SEC_MAX_DATA_SIZE]={0};
	TSecFunctionTable *functionTable;
	unsigned int dataSize; 
	unsigned int padSize;
	TSecStatus status;
	if ( (argc <3) || (strcmp (argv[2], NOCS3_HARNESS_TEST_OPTION_HELP)==0 ) )
	{
		sec_harness_test_rsa_help();
		return;
	}

	memset (&params, 0, sizeof (SEC_Harness_Test_Rsa_Parameters));
	if ( (nocs3_status=sec_harness_test_rsa_parse_cmd_line (argc, argv, &params)) != NOCS3_HARNESS_TEST_STATUS_CODE_SUCCESS )
	{
		/* Parser error code */
		SEC_OTHER_ERR_TRACE ("Ram command parser failed.  Please make sure the command line parameters are valid");
		return;
	}

#ifdef NOCS3_DEBUG_MESSAGE
	sec_harness_test_rsa_dump_paramters (&params);
#endif

	/* Platform init */
    NEXUS_Platform_GetDefaultSettings(&platformSettings); 
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
	functionTable = secGetFunctionTable();
	e = params.expE;

	if(params.keyGen){
		status = functionTable->secRsaGenerateKey(params.keySize, e, n,p,q, dp, dq, qinv);
		if(status!=SEC_NO_ERROR){
			printf("[SEC][EX] secRsaGenerateKey error %d\n", status);
			return;
		}

		if ( params.pModFile)
			fwrite((void *) n, sizeof(unsigned char), params.keySize, params.pModFile);

		if ( params.pPFile)
			fwrite((void *) p, sizeof(unsigned char), params.keySize/2, params.pPFile);

		if ( params.pQFile)
			fwrite((void *) q, sizeof(unsigned char), params.keySize/2, params.pQFile);

		if ( params.pDPFile)
			fwrite((void *) dp, sizeof(unsigned char), params.keySize/2, params.pDPFile);

		if ( params.pDQFile)
			fwrite((void *) dq, sizeof(unsigned char), params.keySize/2, params.pDQFile);

		if ( params.pQinvFile)
			fwrite((void *) qinv, sizeof(unsigned char), params.keySize/2, params.pQinvFile);
	}

	if(params.rsaCmp){
		if(params.pPFile)
			fread((void *) p, sizeof(unsigned char), params.keySize/2, params.pPFile);
		else{
			
			printf("[SEC][EX]P file is missing\n");
			return;
			}
		if(params.pQFile)fread((void *) q, sizeof(unsigned char), params.keySize/2, params.pQFile);
		else{
			
			printf("[SEC][EX]Q file is missing\n");
			return;
		}
		status = functionTable->secRsaComputeCrtParams(params.keySize,params.expE, p, q, dp, dq, qinv);
		if(status!=SEC_NO_ERROR){
				printf("[SEC][EX] secRsaComputeCrtParams error %d\n", status);
				return;
		}
		
		if ( params.pDPFile)
			fwrite((void *) dp, sizeof(unsigned char), params.keySize/2, params.pDPFile);

		if ( params.pDQFile)
			fwrite((void *) dq, sizeof(unsigned char), params.keySize/2, params.pDQFile);

		if ( params.pQinvFile)
			fwrite((void *) qinv, sizeof(unsigned char), params.keySize/2, params.pQinvFile);
	}

	switch(params.padding){
			case SEC_RSA_PKCS1_V1_5_PADDING:
				padSize = 11;
				break;
			case SEC_RSA_OAEP_SHA_1_PADDING:
			padSize = 42;
				break;
			case SEC_RSA_OAEP_SHA_256_PADDING:
			/*qx??*/
			padSize = 66;
				break;
			case SEC_RSA_NO_PADDING:
				padSize = 0;
				break;
			case SEC_RSA_OAEP_SHA_384_PADDING:
				padSize = 98;
				break;
			default:
				printf("[SEC][EX] unknow padding type!\n"); 
				return;
				break;
	}
		
	if(params.pModFile){
		fread((void *) n, sizeof(unsigned char), params.keySize, params.pModFile);
		fclose(params.pModFile);
	}
	if(params.bPublic){
		if(params.operation ==NOCS3_HARNESS_TEST_OPERATION_ENCRYPTION){
			status = functionTable->secRsaPublicEncrypt(params.data, params.sizeData, out_data, params.expE, n, params.keySize, params.padding);
			if(status!=SEC_NO_ERROR){
				printf("[SEC][EX] secRsaPublicEncrypt error %d\n", status);
				return;
			}
dataSize = params.keySize;	
	}
		
		if(params.operation ==NOCS3_HARNESS_TEST_OPERATION_DECRYPTION){
			status = functionTable->secRsaPublicDecrypt(params.data, out_data,&(params.sizeData),  params.expE, n, params.keySize, params.padding);
			if(status!=SEC_NO_ERROR){
				printf("[SEC][EX] secRsaPublicDecrypt error %d\n", status);
				return;
			}

			dataSize = params.keySize-padSize;
		}
		
		if(params.pOutFile){
				fwrite((void *) out_data, sizeof(unsigned char), dataSize, params.pOutFile);
				fclose(params.pOutFile);
		}
	}
	
	if(params.bPrivate){
		if(params.pPFile){
			fread((void *) p, sizeof(unsigned char), params.keySize/2, params.pPFile);
			fclose(params.pPFile);
		}
		if(params.pQFile){
			fread((void *) q, sizeof(unsigned char), params.keySize/2, params.pQFile);
			fclose(params.pQFile);
		}
		if(params.pDPFile){
			fread((void *) dp, sizeof(unsigned char), params.keySize/2, params.pDPFile);
			fclose(params.pDPFile);
		}
		if(params.pDQFile){
			fread((void *) dq, sizeof(unsigned char), params.keySize/2, params.pDQFile);
			fclose(params.pDQFile);
			}
		if(params.pQinvFile){
			fread((void *) qinv, sizeof(unsigned char), params.keySize/2, params.pQinvFile);
			fclose(params.pQinvFile);
			}
		if(params.operation ==NOCS3_HARNESS_TEST_OPERATION_ENCRYPTION){
			status = functionTable->secRsaPrivateEncrypt(params.data, params.sizeData, out_data, params.expE, n, p, q, dp, dq, qinv, params.keySize, params.padding);
			
			if(status!=SEC_NO_ERROR){
				printf("[SEC][EX] secRsaPrivateEncrypt error %d\n", status);
				return;
			}

			dataSize = params.sizeData+padSize;
		}
		
		if(params.operation ==NOCS3_HARNESS_TEST_OPERATION_DECRYPTION){
		/*	printf("RSA public decrypt\n");*/
/*BKNI_Delay(1);*/
	status = functionTable->secRsaPrivateDecrypt(params.data,  out_data, &(params.sizeData), params.expE, n,p, q, dp, dq, qinv, params.keySize, params.padding);
			
			if(status!=SEC_NO_ERROR){
				printf("[SEC][EX] secRsaPrivateDecrypt error %d\n", status);
				return;
			}
	
			dataSize = params.keySize-padSize;

		}
		
		if(params.pOutFile){

				fwrite((void *) out_data, sizeof(unsigned char), dataSize, params.pOutFile);
				fclose(params.pOutFile);
		}
	}
	
  printf ("[SEC][M] RSA test is done successfully.\n");
	if(params.pExpDFile)
	fclose(params.pExpDFile);

	NEXUS_Memory_Free(functionTable);
  NEXUS_Platform_Uninit();
}


static void sec_harness_test_rsa_help (void)
{
	printf("\t%s\n\n", SEC_HELP_M_RSA_INST);
	printf("\t%s\n\n", SEC_HELP_M_RSA_GEN_PUB_PVT);
	printf("\t%s\n\n", SEC_HELP_M_ENC_DEC);
	printf("\t%s\n\n", SEC_HELP_M_RSA_KEYSIZE);
	printf("\t%s\n\n", SEC_HELP_M_RSA_EXPE);
	printf("\t%s\n\n", SEC_HELP_M_RSA_EXPD);
	printf("\t%s\n\n", SEC_HELP_M_RSA_MODN);
	printf("\t%s\n\n", SEC_HELP_M_RSA_PAD);
	printf("\t%s\n\n", SEC_HELP_M_RSA_IF);
	printf("\t%s\n\n", SEC_HELP_M_RSA_OF);
}

static NOCS3_HARNESS_TEST_STATUS_CODE sec_harness_test_rsa_parse_cmd_line (int argc, 
																		char* argv[], 
																		SEC_Harness_Test_Rsa_Parameters * pParams)
{
#if 1
	int cmdline_offset = 2;
	bool bOperationParsed = false;
	bool bKeyGenParsed = false;
	bool bRsaCmpParsed = false; 
	bool bPubOp = false;
	bool bPvtOp = false;
	bool bPad = false;
	bool bexpFileParsed = false;
	bool bKeySizeParsed = false;
	bool bExpEParsed = false;
	bool bmodNFileParsed = false; 
	bool bIfParsered = false;
	bool bOfParsered = false;
	bool bpadTypeParsered = false;
	bool bpFileParsed =false;
	bool bqFileParsed = false;
	bool bDpFileParsed = false;
	bool bDqFileParsed = false;
	bool bQinvFileParsed = false;
	FILE * pInFile = NULL;


	/* command parser state machine */
	do 
	{
		bool bCommandProcessed = false;
		NOCS3_HARNESS_TEST_PARSER_STATUS_CODE status;

		NOCS3_HARNESS_TEST_PARSE_CRYPTO_OPERATION_REQUIRED;
		
		NOCS3_COMMAND_PARSER_PARSE_SINGLE_OPTIONAL(bKeyGenParsed, NOCS3_HARNESS_TEST_OPTION_KEYGEN);
		NOCS3_COMMAND_PARSER_PARSE_SINGLE_OPTIONAL(bRsaCmpParsed, NOCS3_HARNESS_TEST_OPTION_RSACMP);

		NOCS3_COMMAND_PARSER_PARSE_SINGLE_OPTIONAL(bPubOp, NOCS3_HARNESS_TEST_OPTION_PUB);
		NOCS3_COMMAND_PARSER_PARSE_SINGLE_OPTIONAL(bPvtOp, NOCS3_HARNESS_TEST_OPTION_PVT);
		
		NOCS3_COMMAND_PARSER_PARSE_NUMBER_REQUIRED (bKeySizeParsed, NOCS3_HARNESS_TEST_OPTION_KEYSIZE, &pParams->keySize, "0x%x");

		NOCS3_COMMAND_PARSER_PARSE_NUMBER_REQUIRED (bExpEParsed, NOCS3_HARNESS_TEST_OPTION_EXPE, &pParams->expE, "0x%x");
		NOCS3_COMMAND_PARSER_PARSE_SINGLE_OPTIONAL(bPad, NOCS3_HARNESS_TEST_OPTION_PAD);
		
		if(bPad) {
			SEC_HARNESS_TEST_PARSE_PAD_TYPE;

		}

    
		/* If */
		#if 1
		if(bKeyGenParsed){
			NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bexpFileParsed, NOCS3_HARNESS_TEST_OPTION_EXPD, &pParams->pExpDFile, "wb");
			NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bmodNFileParsed, NOCS3_HARNESS_TEST_OPTION_MODN, &pParams->pModFile, "wb");
			
			NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bpFileParsed, NOCS3_HARNESS_TEST_OPTION_XP, &pParams->pPFile, "wb");
			NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bqFileParsed, NOCS3_HARNESS_TEST_OPTION_XQ, &pParams->pQFile, "wb");
			
			NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bDpFileParsed, NOCS3_HARNESS_TEST_OPTION_XDP, &pParams->pDPFile, "wb");
			NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bDqFileParsed, NOCS3_HARNESS_TEST_OPTION_XDQ, &pParams->pDQFile, "wb");
			
			NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bQinvFileParsed, NOCS3_HARNESS_TEST_OPTION_XQINV, &pParams->pQinvFile, "wb");
		}else if(bRsaCmpParsed){
			
			NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bmodNFileParsed, NOCS3_HARNESS_TEST_OPTION_MODN, &pParams->pModFile, "wb");
			
			NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bpFileParsed, NOCS3_HARNESS_TEST_OPTION_XP, &pParams->pPFile, "rb");
			NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bqFileParsed, NOCS3_HARNESS_TEST_OPTION_XQ, &pParams->pQFile, "rb");
			NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bDpFileParsed, NOCS3_HARNESS_TEST_OPTION_XDP, &pParams->pDPFile, "wb");
			NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bDqFileParsed, NOCS3_HARNESS_TEST_OPTION_XDQ, &pParams->pDQFile, "wb");
			NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bQinvFileParsed, NOCS3_HARNESS_TEST_OPTION_XQINV, &pParams->pQinvFile, "wb");
			}
		else{
			NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bexpFileParsed, NOCS3_HARNESS_TEST_OPTION_EXPD, &pParams->pExpDFile, "rb");
			

			NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bmodNFileParsed, NOCS3_HARNESS_TEST_OPTION_MODN, &pParams->pModFile, "rb");

			
			NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bpFileParsed, NOCS3_HARNESS_TEST_OPTION_XP, &pParams->pPFile, "rb");

			NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bqFileParsed, NOCS3_HARNESS_TEST_OPTION_XQ, &pParams->pQFile, "rb");
			
			NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bDpFileParsed, NOCS3_HARNESS_TEST_OPTION_XDP, &pParams->pDPFile, "rb");

			NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bDqFileParsed, NOCS3_HARNESS_TEST_OPTION_XDQ, &pParams->pDQFile, "rb");
			
			NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bQinvFileParsed, NOCS3_HARNESS_TEST_OPTION_XQINV, &pParams->pQinvFile, "rb");

		}
		#endif

		
		NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bIfParsered, NOCS3_HARNESS_TEST_OPTION_IF, &pInFile, "rb");
#if 1
		NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bOfParsered, NOCS3_HARNESS_TEST_OPTION_OF, &pParams->pOutFile, "wb");
#endif
		NOCS3_COMMAND_PARSER_CHECK_IF_COMMAND_PARSED;

	} while ( cmdline_offset < argc );

	if((bKeyGenParsed == false) &&(bPubOp == false) &&(bPvtOp == false)&&(bRsaCmpParsed == false)){
		return NOCS3_HARNESS_TEST_PARSER_STATUS_CODE_BADPARAMS;

	}
	
	if (pInFile!=NULL)
	{
		pParams->sizeData = fread (&pParams->data[0], sizeof(unsigned char), SEC_HARNESS_TEST_SEC_MAX_DATA_SIZE, pInFile);
		fclose (pInFile);
	}
	if(bKeyGenParsed == true){
		pParams->keyGen = true;
	}
	if(bPubOp == true){
		pParams->bPublic = true;
	}

	if(bPvtOp== true){
		pParams->bPrivate= true;
	}
	if(bRsaCmpParsed== true){
		pParams->rsaCmp = true;
	}


#endif

	return NOCS3_HARNESS_TEST_STATUS_CODE_SUCCESS;

}

