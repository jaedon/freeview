#include <bstd.h>
#include <stdio.h>
#include <string.h>

#include "ca_sec.h"
#include "ca_sec_impl.h"
#include "sec_harness_test.h"
#include "sec_harness_test_parser.h"
#include "sec_harness_test_r2r.h"
#include "sec_harness_test_r2r_common.h"

#include "sec_harness_test_r2r_private.h"
#include "sec_harness_test_utilities.h"
#include "nocs3_harness_test_utilities.h"
#include "nexus_memory.h"
#include "nexus_platform.h"

static void sec_harness_test_r2r_help (void);
static NOCS3_HARNESS_TEST_STATUS_CODE sec_harness_test_r2r_parse_cmd_line (int argc, 
																		char* argv[], 
																		SEC_Harness_Test_R2R_Parameters * pParams);

static char gKeyFuncName[2][64] = 
{
    "csdSetClearTextRam2RamHostKey",
    "csdSetProtectedRam2RamContentKeys"
};

static char gCryptoFuncName[2][64] = 
{
    "csdEncryptData",
    "csdDecryptData"
};

#ifdef NOCS3_DEBUG_MESSAGE
static void sec_harness_test_r2r_dump_paramters (SEC_Harness_Test_R2R_Parameters * pParams)
{
	printf("SEC_Harness_Test_R2R_Parameters is :\n");

	printf("operation: %d\n", pParams->operation);
	printf("keyType %d\n", pParams->keyType);
	printf("emi %x\n", pParams->emi);
	printf("sizeIV %d\n", pParams->sizeIV);
	printf("IV array is:\n");
	nocs3_harness_test_utility_dump_array (pParams->iv, pParams->sizeIV);
	printf("sizeData %d\n", pParams->sizeData);
	printf("Data array is:\n");
	nocs3_harness_test_utility_dump_array (pParams->data, pParams->sizeData);
	printf("bMonteCarlo %d\n", pParams->bMonteCarlo);
	printf("numRuns %d\n", pParams->numRuns);
	printf("pInFile %p\n", (void *)pParams->pInFile);

	printf("pOutFile %p\n", (void *)pParams->pOutFile);
	printf("pTvFile %p\n", (void *)pParams->pTvFile);
}
#endif

void sec_harness_test_r2r (int argc, char* argv[])
{
	SEC_Harness_Test_R2R_Parameters params;
	NOCS3_HARNESS_TEST_STATUS_CODE  nocs3_status;
	TSecStatus status;

	unsigned long ulNuidLo; 
	unsigned int i; 
	unsigned char xHostKey[16] = {

			0x7A, 0x81, 0x11, 0x34, 0x32, 0xC4, 0xCC, 0xED,
			0x2E, 0x92, 0xDA, 0x2A, 0xF1, 0x7D, 0xD9, 0x24
			
		};
	unsigned char ivCbc[16] = {0}; 
	unsigned char DataIn[1024]={0}; 
	unsigned char dataOut[1024] ={0};
	unsigned char dataOut1[1024] ={0};
	unsigned int sizeDataIn, sizeIvCbc, sizeKey; 
	NOCS3_Harness_Test_Test_Vector inputVector; 
	TUnsignedInt8 xcipheredContentKey[16] = {0};
#if 1
	TUnsignedInt8 xR2RCipheredProtectingKeys[SEC_R2R_NB_OF_PROTECTING_KEYS][16];
#endif
	unsigned int sizeCipherContentKey;
	FILE *tvfile; 
	NEXUS_PlatformSettings platformSettings;
	bool bCopyOut1 = false;
	TSecCipherSession r2rSession;
	unsigned int sizeProtectiveKey = 16;
	
	TSecFunctionTable *functionTable;
	TSecNuid64  nuid; 

	if ( (argc <3) || (strcmp (argv[2], "help")==0 ) )
	{
		sec_harness_test_r2r_help();
		return;
	}

	memset (&params, 0, sizeof (SEC_Harness_Test_R2R_Parameters));
	if ( (nocs3_status=sec_harness_test_r2r_parse_cmd_line (argc, argv, &params)) != NOCS3_HARNESS_TEST_STATUS_CODE_SUCCESS )
	{
		/* Parser error code */
		SEC_OTHER_ERR_TRACE ("R2R command parser failed.  Please make sure the command line parameters are valid");
		goto Clean_Up;
	}

#ifdef NOCS3_DEBUG_MESSAGE
	sec_harness_test_r2r_dump_paramters (&params);
#endif

	/* Platform init */
    NEXUS_Platform_GetDefaultSettings(&platformSettings); 
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
	
	functionTable = secGetFunctionTable();
	r2rSession = malloc(sizeof(TSecCipherSession));


	status = functionTable->secGetNuid64(nuid);
	if ( status!=SEC_NO_ERROR) 
	{
		SEC_ERR_TRACE (status, "secGetNuid64", "Cannot find NUID");
		goto Clean_Up;
	}		
    ulNuidLo = ((unsigned long )nuid[4])<<24 |((unsigned long )nuid[5])<<16 |((unsigned long )nuid[6])<<8 |(unsigned long )nuid[7];




	if(params.pTvFile !=NULL)
	{
		tvfile = params.pTvFile;

		nocs3_harness_test_get_test_vector_4_nuid ( nuid, 
														&inputVector,
													tvfile);
			
	
		if(inputVector.sizeHostKey) BKNI_Memcpy(xHostKey, inputVector.HostKey, inputVector.sizeHostKey);
		if(inputVector.sizeDataIn) BKNI_Memcpy(DataIn, inputVector.dataIn, inputVector.sizeDataIn); 
		if(inputVector.sizeIvCbc) BKNI_Memcpy(ivCbc, inputVector.IvCbc, inputVector.sizeIvCbc); 
		sizeIvCbc = inputVector.sizeIvCbc;

	printf("Inoput vector %d.....\n", inputVector.sizeIvCbc);
	printf("0x");
			for(i=0; i<sizeIvCbc; i++) 
				printf("%02x", ivCbc[i]); 
			printf("\n");
		sizeKey = inputVector.sizeHostKey; 
		printf("size data %d\n", inputVector.sizeDataIn);
		sizeDataIn = inputVector.sizeDataIn; 
		
		if(inputVector.sizeIvCtr &&( (params.emi==SEC_Algorithm_RAW_AES_128_CTR)||(params.emi ==SEC_Algorithm_RAW_0x4024)) )BKNI_Memcpy(ivCbc, inputVector.IvCtr, inputVector.sizeIvCbc); 
		sizeIvCbc = inputVector.sizeIvCbc;
		printf("Input ivbc	0...\n");
			
			printf("0x");
			for(i=0; i<sizeIvCbc; i++) 
				printf("%02x", ivCbc[i]); 
			printf("\n");

	#if 0
	if(inputVector.sizeCipheredContentKey){
			BKNI_Memcpy(&xcipheredContentKey[0],eCW,16);
			
		
			/*BKNI_Memcpy(&xcipheredContentKey[8], &eCW[8], 8);*/
		}
	#else
			if(inputVector.sizeCipheredContentKey)
			BKNI_Memcpy(xcipheredContentKey, inputVector.cipheredContentKey,inputVector.sizeCipheredContentKey);
	#endif		
	
		sizeCipherContentKey = inputVector.sizeCipheredContentKey; 
		if(inputVector.sizeCipheredProtectingKeyL1){
			BKNI_Memcpy(xR2RCipheredProtectingKeys[1], inputVector.cipheredProtectingKeyL1, inputVector.sizeCipheredProtectingKeyL1);
			sizeProtectiveKey = inputVector.sizeCipheredProtectingKeyL1;
		}
		
		if(inputVector.sizeCipheredProtectingKeyL2)
			BKNI_Memcpy(xR2RCipheredProtectingKeys[0], inputVector.cipheredProtectingKeyL2, inputVector.sizeCipheredProtectingKeyL2);
		}
	/*
			nocs3_harness_test_dump_test_vector(&inputVector);
	*/
		if(params.sizeData && !sizeDataIn) {
			BKNI_Memcpy(DataIn, params.data, params.sizeData); 
			sizeDataIn = params.sizeData;
			
			}
		else if(params.pInFile){
			/* read file and get input data */
			sizeDataIn = fread((void *) DataIn, sizeof(unsigned char), 1024, params.pInFile);
			printf("Reading file, size is %d\n", sizeDataIn);
		}
		if(params.sizeIV && !sizeIvCbc){
			
			BKNI_Memcpy(ivCbc, params.iv, params.sizeIV); 
			printf("Input ivbc  1...\n");
		
		printf("0x");
		for(i=0; i<sizeIvCbc; i++) 
			printf("%02x", ivCbc[i]); 
		printf("\n"); 
			sizeIvCbc = params.sizeIV;
			}
		
	memset (r2rSession, 0, sizeof(TSecCipherSession));
	if(params.operation == NOCS3_HARNESS_TEST_OPERATION_ENCRYPTION){
		status =functionTable->secOpenRam2RamEncryptSession(&r2rSession);
		if(status!=SEC_NO_ERROR){
			printf("[SEC][EX] secOpenRam2RamEncryptSession error %d\n", status);
			return;
			}
	}else{
		status = functionTable->secOpenRam2RamDecryptSession(&r2rSession);
		if(status!=SEC_NO_ERROR){
			printf("[SEC][EX] secOpenRam2RamDecryptSession error %d\n", status);
			return;
			}
			}
	if(params.keyType==NOCS3_HARNESS_TEST_KEY_SOFTWARE){ /*software key */
		if((status =functionTable->secSetClearTextKey(r2rSession, params.emi, params.parity,sizeKey, xHostKey) ) !=SEC_NO_ERROR){

			printf("[SEC][EX] secSetClearTextKey error %d\n", status);
				goto Clean_Up;
			}
		}else if(params.keyType == NOCS3_HARNESS_TEST_KEY_KEYLADDER){

		 if((status =functionTable->secSet2LevelProtectedKey(r2rSession, params.emi,params.parity, sizeCipherContentKey,xcipheredContentKey,
		 	sizeProtectiveKey, xR2RCipheredProtectingKeys[1],xR2RCipheredProtectingKeys[0] ))!=SEC_NO_ERROR){
			 printf("[SEC][EX] secSet2LevelProtectedKey error %d\n", status);
				goto Clean_Up;
		 	}
		}else if(params.keyType == NOCS3_HARNESS_TEST_KEY_CERT){

			printf("certification key...\n");
			status = functionTable->secUseCertKey(r2rSession, params.emi, params.parity);
			if(status !=SEC_NO_ERROR){
				printf("[SEC][EX] secUseCertKey error %d\n", status);
				goto Clean_Up;
			}
		}else{
			printf("[SEC][EX] Unknown key type!\n");
			return;

			}


if(sizeIvCbc)
	BKNI_Memcpy(dataOut1, ivCbc, sizeIvCbc);

	

	if(params.operation == NOCS3_HARNESS_TEST_OPERATION_ENCRYPTION)
	{
		printf("encrypt result...\n"); 
	

		if ( (params.bMonteCarlo) && (( params.emi== SEC_Algorithm_TDES_ABA_CBC_IV_0_SCTE52) ||
					(params.emi == SEC_Algorithm_RAW_AES_128_CBC_CLEAR) ||(params.emi ==SEC_Algorithm_RAW_0x4023)||
					(params.emi == SEC_Algorithm_RAW_AES_128_CBC_IV_0_CLEAR) ||
					(params.emi == SEC_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR) ||
					(params.emi == SEC_Algorithm_RAW_TDES_ABA_CBC_CLEAR)))
		{
			bCopyOut1 = true;	
		}

		for(i=0; i<params.numRuns; )
		{

			status = functionTable->secSessionEncrypt(r2rSession, DataIn, dataOut, sizeDataIn, ivCbc, sizeIvCbc /*inputVector.sizeDataIn*/); 
			if(status!=SEC_NO_ERROR){
				printf("[SEC][EX] secSessionEncrypt error %d\n", status);
			return;
				}
			if(params.bMonteCarlo&&(params.numRuns>1))
			{
				
				if(( params.emi== SEC_Algorithm_TDES_ABA_CBC_IV_0_SCTE52) ||
					(params.emi == SEC_Algorithm_RAW_AES_128_CBC_CLEAR) ||
					(params.emi == SEC_Algorithm_RAW_AES_128_CBC_IV_0_CLEAR) ||
					(params.emi == SEC_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR) ||
					(params.emi == SEC_Algorithm_RAW_TDES_ABA_CBC_CLEAR)||(params.emi ==SEC_Algorithm_RAW_0x4023)
				){
					/*printf("encrypt result. %d..\n", i); */
	
					BKNI_Memcpy(DataIn, dataOut1, sizeDataIn);
					BKNI_Memcpy(ivCbc, dataOut, sizeDataIn);
					functionTable->secSessionEncrypt(r2rSession, DataIn, dataOut1, sizeDataIn, ivCbc, sizeIvCbc /*inputVector.sizeDataIn*/); 
					BKNI_Memcpy(ivCbc, dataOut1, sizeDataIn);
					i+=2;
				}
				else 
				{
					if( ( params.emi== SEC_Algorithm_RAW_AES_128_CTR) ||(params.emi ==SEC_Algorithm_RAW_0x4024))
					{
						nocs3_harness_test_utility_increase_16bytes_counter(ivCbc, sizeDataIn>>4);
					}
					i +=1;
				}
				
				
				BKNI_Memcpy(DataIn, dataOut, sizeDataIn);
			}
			else
			{
				i++;
				#if 1
				BKNI_Memcpy(DataIn, dataOut, sizeDataIn);
				#endif
			}
		}
	}
	else if(params.operation ==NOCS3_HARNESS_TEST_OPERATION_DECRYPTION)
	{
		printf("decrypt result...\n"); 
		printf("Input data...\n");
		printf("0x");
		for(i=0; i<sizeDataIn; i++) 
			printf("%02x", DataIn[i]); 
		printf("\n"); 
		if ( (params.bMonteCarlo) && ((params.emi == SEC_Algorithm_RAW_AES_128_CBC_CLEAR) ||
					(params.emi == SEC_Algorithm_RAW_AES_128_CBC_IV_0_CLEAR) ||(params.emi ==SEC_Algorithm_RAW_0x4023)))
		{
			bCopyOut1 = true;	
		}else bCopyOut1 = false;
		
		for(i=0; i<params.numRuns; )
		{
	
			status = functionTable->secSessionDecrypt(r2rSession, DataIn, dataOut, sizeDataIn, ivCbc, sizeIvCbc /*inputVector.sizeDataIn*/); 
			if(status!=SEC_NO_ERROR){
				printf("[SEC][EX] secSessionDecrypt error %d\n", status);
				return;
				}
		
			if(params.bMonteCarlo&&(params.numRuns>1))
			{

				if(
					(params.emi == SEC_Algorithm_RAW_AES_128_CBC_IV_0_CLEAR) ||
					(params.emi == SEC_Algorithm_RAW_AES_128_CBC_CLEAR) ||(params.emi ==SEC_Algorithm_RAW_0x4023)
				)
				{
					BKNI_Memcpy(ivCbc, DataIn, sizeDataIn);
					BKNI_Memcpy(DataIn, dataOut1, sizeDataIn);
					
					status = functionTable->secSessionDecrypt(r2rSession, DataIn, dataOut1, sizeDataIn, ivCbc, sizeIvCbc /*inputVector.sizeDataIn*/); 
					if(status!=SEC_NO_ERROR){
						printf("[SEC][EX] secSessionDecrypt error %d\n", status);
						return;
						}
					BKNI_Memcpy(ivCbc, DataIn, sizeDataIn);
					i+=2;
				}
				else 
				{
					if( ( params.emi== SEC_Algorithm_RAW_AES_128_CTR) ||(params.emi ==SEC_Algorithm_RAW_0x4024))
					{
						nocs3_harness_test_utility_increase_16bytes_counter(ivCbc, sizeDataIn>>4);
					}
					i +=1;
				}

				if((params.emi == SEC_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR) || 
					(params.emi == SEC_Algorithm_RAW_TDES_ABA_CBC_CLEAR) )
				{
					BKNI_Memcpy(ivCbc, DataIn,  sizeDataIn);
				}
				
				BKNI_Memcpy(DataIn, dataOut, sizeDataIn);
				
			}else	{
				i++;
				#if 1
				if((params.emi == SEC_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR) || 
					(params.emi == SEC_Algorithm_RAW_TDES_ABA_CBC_CLEAR) )
				{
					BKNI_Memcpy(ivCbc, DataIn,  sizeDataIn);
				}
				
				BKNI_Memcpy(DataIn, dataOut, sizeDataIn);
				#endif
			}
		}

	}
	

#if 0
	if( (params.bMonteCarlo) &
					((params.emi == SEC_Algorithm_RAW_AES_128_CBC_IV_0_CLEAR)  || (params.emi == SEC_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR) ||
					(params.emi == SEC_Algorithm_RAW_AES_128_CBC_CLEAR)  || (params.emi == SEC_Algorithm_RAW_TDES_ABA_CBC_CLEAR))
		)
#else
	if ( bCopyOut1 == true )
#endif
	{
		BKNI_Memcpy(dataOut, dataOut1, sizeDataIn);
	}
			
	if(params.pOutFile!=NULL)
	{

		 fwrite(dataOut, sizeof(unsigned char), sizeDataIn,params.pOutFile);
 	
		/*fprintf(params.pOutFile, "0x");
		for(i=0; i<sizeDataIn; i++) 
		{
			fprintf(params.pOutFile, "%02x", dataOut[i]); 
		}
		fprintf(params.pOutFile, "\n"); 		
		*/
	}
	else
	{ /* -oc */
		printf("Output:\n"); 


		printf("0x");
		for(i=0; i<sizeDataIn; i++) printf("%02x", dataOut[i]); 
		printf("\n"); 		
		}

    printf ("[SEC][M] r2r test is done successfully.\n");






	   /* Clean up */
   if (params.pInFile)
   {
	   fclose (params.pInFile);
   }
   if (params.pOutFile)
   {
	   fclose (params.pOutFile);
   }
   if (params.pTvFile)
   {
	   fclose (params.pTvFile);
   }

   functionTable->secCloseSession(r2rSession);
   free(r2rSession);

   NEXUS_Memory_Free(functionTable);

   Clean_Up:

   NEXUS_Platform_Uninit();
}


static void sec_harness_test_r2r_help (void)
{
	printf("\t%s\n\n", SEC_HELP_M_R2R_INST);
	printf("\t\t%s\n\n", SEC_HELP_M_R2R_OP);
	printf("\t\t%s\n\n", NOCS3_HELP_M_KEY);
	printf("\t\t%s\n\n", NOCS3_HELP_M_EMI);
	printf("\t\t%s\n\n", NOCS3_HELP_M_IVC);
	printf("\t\t%s\n\n", SEC_HELP_M_R2R_IFIC);
	printf("\t\t%s\n\n", SEC_HELP_M_R2R_OFOC);
	printf("\t\t%s\n\n", SEC_HELP_M_R2R_MT);
	printf("\t\t%s\n\n", SEC_HELP_M_R2R_RUNS);
	printf("\t\t%s\n\n", NOCS3_HELP_M_TV);
}

static NOCS3_HARNESS_TEST_STATUS_CODE sec_harness_test_r2r_parse_cmd_line (int argc, 
																		char* argv[], 
																		SEC_Harness_Test_R2R_Parameters * pParams)
{
	bool bOperationParsed = false;
	bool bKeyParsed = false;
	bool bKeyTypeParsed = false;
	bool bEmiParsed = false;
	bool bIvcParsed = false;
	bool bOcParsered = false;
	bool bIfParsered = false;
	bool bOfParsered = false; 
	bool bIcParsered = false;
	bool bMtParsed = false;
	bool bNumRunsParsed = false;
	bool bTvParsed = false;
	bool bParityParsed = false; 
	bool bParityTypeParsed = false; 
	int cmdline_offset = 2;

	/* R2R command parser state machine */
	do 
	{
		NOCS3_HARNESS_TEST_PARSER_STATUS_CODE status;
		bool bCommandProcessed = false;

		/* Crypto Operation */
		NOCS3_HARNESS_TEST_PARSE_CRYPTO_OPERATION_REQUIRED;


		/* Key type */
		NOCS3_COMMAND_PARSER_PARSE_SINGLE_OPTIONAL(bKeyParsed, NOCS3_HARNESS_TEST_OPTION_KEY);
		SEC_HARNESS_TEST_PARSE_KEY_TYPE;

		/* EMI */
		NOCS3_COMMAND_PARSER_PARSE_NUMBER_REQUIRED (bEmiParsed, NOCS3_HARNESS_TEST_OPTION_EMI, &pParams->emi, "0x%x");

		/* IVC */
		NOCS3_COMMAND_PARSER_PARSE_ARRAY_OPTIONAL(bIvcParsed, \
												NOCS3_HARNESS_TEST_OPTION_IVC, \
												&pParams->iv[0], \
												&pParams->sizeIV, \
												NOCS3_HARNESS_TEST_MAX_IV_SIZE);

		NOCS3_COMMAND_PARSER_PARSE_SINGLE_OPTIONAL(bParityParsed, NOCS3_HARNESS_TEST_OPTION_PARITY);
			SEC_HARNESS_TEST_PARSE_PARITY_TYPE;

		/* Ic */
		NOCS3_COMMAND_PARSER_PARSE_ARRAY_OPTIONAL(bIcParsered, \
												NOCS3_HARNESS_TEST_OPTION_IC, \
												&pParams->data[0], \
												&pParams->sizeData, \
												SEC_HARNESS_TEST_R2R_MAX_DATA_SIZE);
		/* If */
		NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bIfParsered, NOCS3_HARNESS_TEST_OPTION_IF, &pParams->pInFile, "rb");

		/* Oc */
		NOCS3_COMMAND_PARSER_PARSE_SINGLE_OPTIONAL (bOcParsered, NOCS3_HARNESS_TEST_OPTION_OC);

		/* Of */
		NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bOfParsered, NOCS3_HARNESS_TEST_OPTION_OF, &pParams->pOutFile, "w");


		/* Monte Carlo */
		NOCS3_COMMAND_PARSER_PARSE_SINGLE_OPTIONAL (bMtParsed, NOCS3_HARNESS_TEST_OPTION_MT);


		/* NumRuns */
		NOCS3_COMMAND_PARSER_PARSE_NUMBER_REQUIRED (bNumRunsParsed, NOCS3_HARNESS_TEST_OPTION_RUNS, &pParams->numRuns, "0x%x");


		/* TV */
		NOCS3_COMMAND_PARSER_PARSE_FILE_REQUIRED (bTvParsed, NOCS3_HARNESS_TEST_OPTION_TV, &pParams->pTvFile, "r");


		NOCS3_COMMAND_PARSER_CHECK_IF_COMMAND_PARSED;


	} while ( cmdline_offset < argc );

	/* Make sure we got all the required field */
	if ( ( (bIfParsered==true) && (bIcParsered==true)) || \
		 ( (bOfParsered==true) && (bOcParsered==true)) || \
		 ( bOperationParsed == false) ||
		 ( bKeyTypeParsed == false ) ||
		 ( bEmiParsed == false ) ||
		 ( (bOfParsered==false) && (bOcParsered==false) ) ||
		 ( bNumRunsParsed == false ) ||
		 ( bTvParsed == false ) )
	{
		return NOCS3_HARNESS_TEST_PARSER_STATUS_CODE_BADPARAMS;
	}

	/* Further processing */
	if (bMtParsed)
	{
		pParams->bMonteCarlo = true;
	}

	return NOCS3_HARNESS_TEST_STATUS_CODE_SUCCESS;
}

