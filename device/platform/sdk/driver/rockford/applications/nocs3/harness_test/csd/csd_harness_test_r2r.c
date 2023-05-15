#include <bstd.h>
#include <stdio.h>
#include <string.h>

#include "nocs_csd_impl.h"
#include "nocs_csd.h"

#include "csd_harness_test.h"
#include "csd_harness_test_parser.h"
#include "csd_harness_test_r2r.h"
#include "csd_harness_test_r2r_private.h"
#include "csd_harness_test_utilities.h"
#include "nocs3_harness_test_utilities.h"
#include "nexus_memory.h"
#include "nexus_platform.h"

static void csd_harness_test_r2r_help (void);
static NOCS3_HARNESS_TEST_STATUS_CODE csd_harness_test_r2r_parse_cmd_line (int argc, 
																		char* argv[], 
																		CSD_Harness_Test_R2R_Parameters * pParams);

#if (CSDAPI_VERSION_MEDIUM>1)
static NOCS3_HARNESS_TEST_STATUS_CODE csd_harness_test_r2r_map_emi_to_algorithm (TCsdUnsignedInt16 xEmi,\
																				 TCsdR2RAlgorithm *pAlgorithm, \
																				 TCsdR2RCryptoOperationMode *pMode)
{
	NOCS3_HARNESS_TEST_STATUS_CODE nocs_status = NOCS3_HARNESS_TEST_STATUS_CODE_SUCCESS;
	switch (xEmi)
	{
	  case CSD_Algorithm_AES_128_ECB_CLEAR:
	  case CSD_Algorithm_RAW_AES_128_ECB_CLEAR:
			*pAlgorithm = CSD_R2R_ALGORITHM_AES_128;
			*pMode = CSD_R2R_CRYPTO_OPERATION_MODE_ECB;
			break;

	  case CSD_Algorithm_AES_128_CBC_CLEAR:
	  case CSD_Algorithm_RAW_AES_128_CBC_IV_0_CLEAR:
	  case CSD_Algorithm_RAW_AES_128_CBC_CLEAR:
	  case CSD_Algorithm_RAW_0x4022:
	  case CSD_Algorithm_RAW_0x4023:
			*pAlgorithm = CSD_R2R_ALGORITHM_AES_128;
			*pMode = CSD_R2R_CRYPTO_OPERATION_MODE_CBC;
			break;

	  case CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52:
	  case CSD_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR:
	  case CSD_Algorithm_RAW_TDES_ABA_CBC_CLEAR:
			*pAlgorithm = CSD_R2R_ALGORITHM_TDES_K1K2K1;
			*pMode = CSD_R2R_CRYPTO_OPERATION_MODE_CBC;
			break;
			
	  case CSD_Algorithm_TDES_ABA_ECB_CLEAR:
	  case CSD_Algorithm_RAW_TDES_ABA_ECB_CLEAR:
			*pAlgorithm = CSD_R2R_ALGORITHM_TDES_K1K2K1;
			*pMode = CSD_R2R_CRYPTO_OPERATION_MODE_ECB;
			break;

	  case CSD_Algorithm_RAW_AES_128_COUNTER_CLEAR:
	  case CSD_Algorithm_RAW_0x4024:
			*pAlgorithm = CSD_R2R_ALGORITHM_AES_128;
			*pMode = CSD_R2R_CRYPTO_OPERATION_MODE_CTR;
			break;

	  default:
			nocs_status = NOCS3_HARNESS_TEST_STATUS_CODE_BAD_PARAMS;
			break;

		
	}

	return nocs_status;
}

#endif /* (CSDAPI_VERSION_MEDIUM>1) */


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
static void csd_harness_test_r2r_dump_paramters (CSD_Harness_Test_R2R_Parameters * pParams)
{
	printf("CSD_Harness_Test_R2R_Parameters is :\n");

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


void csd_harness_test_r2r (int argc, char* argv[])
{
	CSD_Harness_Test_R2R_Parameters params;
	NOCS3_HARNESS_TEST_STATUS_CODE  nocs3_status;
	TCsdStatus status;
	TCsdInitParameters csdInit;
	TCsdTerminateParameters csdTerminateHandle; 
	TCsd8BytesVector  nuid; 
	TCsdR2RKeyPathHandle      xR2RKeyPathHandle,  *pxR2RKeyPathHandle = &xR2RKeyPathHandle;
	unsigned long ulNuidLo; 
	unsigned int i; 
	unsigned char xHostKey[16] = {
			/*0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 
			0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0x01*/
		/*	0x8E, 0x1C,  0x09, 0x83, 0x8D, 0x9E, 0x6E, 0xD2, 0xA1, 0xED, 0x0D, 0xD4, 0x6D, 0xD4, 0x5C, 0x03*/
			/*0x80,  0x76, 0x28, 0xD9, 0xFD, 0x99, 0xA6, 0xB5,
			0x70, 0x0C, 0xA1, 0x49, 0x00, 0xC0, 0xD8, 0x65,

			*/
			
			0x7A, 0x81, 0x11, 0x34, 0x32, 0xC4, 0xCC, 0xED,
			0x2E, 0x92, 0xDA, 0x2A, 0xF1, 0x7D, 0xD9, 0x24
			
		};
	unsigned char ivCbc[16] = {0}; 
	unsigned char DataIn[16]={0}; 
	unsigned char dataOut[16] ={0};
	unsigned char dataOut1[16] ={0};
	unsigned int sizeDataIn, sizeIvCbc, sizeKey; 
	NOCS3_Harness_Test_Test_Vector inputVector; 
	TCsdUnsignedInt8 xcipheredContentKey[16] = {0};
	TCsdUnsignedInt8 xR2RCipheredProtectingKeys[CSD_R2R_NB_OF_PROTECTING_KEYS][16];
	unsigned int sizeCipherContentKey;
	FILE *tvfile; 
	NEXUS_PlatformSettings platformSettings;
	bool bCopyOut1 = false;
#if (CSDAPI_VERSION_MEDIUM>1)
	TCsdR2RAlgorithm xAlgorithm;
	TCsdR2RCryptoOperationMode xMode;
#endif

	if ( (argc <3) || (strcmp (argv[2], "help")==0 ) )
	{
		csd_harness_test_r2r_help();
		return;
	}

	memset (&params, 0, sizeof (CSD_Harness_Test_R2R_Parameters));
	if ( (nocs3_status=csd_harness_test_r2r_parse_cmd_line (argc, argv, &params)) != NOCS3_HARNESS_TEST_STATUS_CODE_SUCCESS )
	{
		/* Parser error code */
		CSD_OTHER_ERR_TRACE ("R2R command parser failed.  Please make sure the command line parameters are valid");
		goto Clean_Up;
	}

#ifdef NOCS3_DEBUG_MESSAGE
	csd_harness_test_r2r_dump_paramters (&params);
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


	status = csdGetNuid64(nuid);
	if ( status!=CSD_NO_ERROR) 
	{
		CSD_ERR_TRACE (status, "csdGetNuid64", "Cannot find NUID");
		goto Clean_Up;
	}		
    ulNuidLo = ((unsigned long )nuid[4])<<24 |((unsigned long )nuid[5])<<16 |((unsigned long )nuid[6])<<8 |(unsigned long )nuid[7];

#if (CSDAPI_VERSION_MEDIUM>1)
	nocs3_status = csd_harness_test_r2r_map_emi_to_algorithm (params.emi, &xAlgorithm, &xMode);
	if ( nocs3_status != NOCS3_HARNESS_TEST_STATUS_CODE_SUCCESS )
	{
		CSD_OTHER_ERR_TRACE ("Cannot map emi to NOCS1.x algorithm");
		goto Clean_Up;
	}		
#endif

	if(params.sizeData== 0 && params.pInFile == NULL)
	{
		if(params.pTvFile !=NULL) 
			tvfile = params.pTvFile;

		nocs3_harness_test_get_test_vector_4_nuid ( nuid, 
															&inputVector,
														tvfile);
				
	#ifdef NOCS3_DEBUG_MESSAGE
		nocs3_harness_test_dump_test_vector (&inputVector);
	#endif
		
		if(inputVector.sizeHostKey) 
			BKNI_Memcpy(xHostKey, inputVector.HostKey, inputVector.sizeHostKey);
		if(inputVector.sizeDataIn) 
			BKNI_Memcpy(DataIn, inputVector.dataIn, inputVector.sizeDataIn); 
		if ( (params.emi==CSD_Algorithm_RAW_AES_128_COUNTER_CLEAR) || (params.emi==CSD_Algorithm_RAW_0x4024) )
		{
			if(inputVector.sizeIvCtr) 
				BKNI_Memcpy(ivCbc, inputVector.IvCtr, inputVector.sizeIvCtr); 
			sizeIvCbc = inputVector.sizeIvCtr;
		}
		else
		{
			if(inputVector.sizeIvCbc) 
				BKNI_Memcpy(ivCbc, inputVector.IvCbc, inputVector.sizeIvCbc); 
			sizeIvCbc = inputVector.sizeIvCbc;
		}
		sizeKey = inputVector.sizeHostKey; 
		sizeDataIn = inputVector.sizeDataIn; 
	#if 0
		if(inputVector.sizeCipheredContentKey)
		{
				BKNI_Memcpy(&xcipheredContentKey[0],eCW,16);
				
			
				/*BKNI_Memcpy(&xcipheredContentKey[8], &eCW[8], 8);*/
		}
	#else
		if(inputVector.sizeCipheredContentKey)
			BKNI_Memcpy(xcipheredContentKey, inputVector.cipheredContentKey,inputVector.sizeCipheredContentKey);
	#endif		
		
		sizeCipherContentKey = inputVector.sizeCipheredContentKey; 
		if(inputVector.sizeCipheredProtectingKeyL1)
			BKNI_Memcpy(xR2RCipheredProtectingKeys[1], inputVector.cipheredProtectingKeyL1, inputVector.sizeCipheredProtectingKeyL1);
			
		if(inputVector.sizeCipheredProtectingKeyL2)
			BKNI_Memcpy(xR2RCipheredProtectingKeys[0], inputVector.cipheredProtectingKeyL2, inputVector.sizeCipheredProtectingKeyL2);
		/*
				nocs3_harness_test_dump_test_vector(&inputVector);
		*/
	}
	else
	{
		if(params.sizeData) {
			BKNI_Memcpy(DataIn, params.data, params.sizeData); 
			sizeDataIn = params.sizeData;
			
			}
		else if(params.pInFile){
			/* read file and get input data */
		}
		if(params.sizeIV){
			
			BKNI_Memcpy(ivCbc, params.iv, params.sizeIV); 
			
			sizeIvCbc = params.sizeIV;
			}
		
	}
	memset (pxR2RKeyPathHandle, 0, sizeof(TCsdR2RKeyPathHandle));
	pxR2RKeyPathHandle->initHandle = &csdInit;
	pxR2RKeyPathHandle->keyHandle = NULL;
#if (CSDAPI_VERSION_MEDIUM<2)
	pxR2RKeyPathHandle->bEncrypt = (params.operation==NOCS3_HARNESS_TEST_OPERATION_ENCRYPTION)? true:false; 
	if(params.keyType==NOCS3_HARNESS_TEST_KEY_SOFTWARE)
	{ 
		/*software key */
		if((status =csdSetClearTextRam2RamHostKey(params.emi,pxR2RKeyPathHandle, xHostKey, sizeKey) ) !=CSD_NO_ERROR)
		{

				CSD_ERR_TRACE (status, "csdSetClearTextRam2RamHostKey", "Cannot set R2R host key");
				goto Clean_Up;
			
		}
		
	}
	else if(params.keyType == NOCS3_HARNESS_TEST_KEY_KEYLADDER)
	{

		 if((status =csdSetProtectedRam2RamContentKeys(params.emi,pxR2RKeyPathHandle,xR2RCipheredProtectingKeys,xcipheredContentKey, sizeCipherContentKey))!=CSD_NO_ERROR)
		 {
				CSD_ERR_TRACE (status, "csdSetProtectedRam2RamContentKeys", "Cannot set R2R host key");
				goto Clean_Up;
		 }
	}
#endif

	BKNI_Memcpy(dataOut1, ivCbc, sizeIvCbc);

	if(params.operation == NOCS3_HARNESS_TEST_OPERATION_ENCRYPTION)
	{
		printf("encrypt result...\n"); 
		printf("Input data...\n");
		
		printf("0x");
		for(i=0; i<sizeDataIn; i++) 
			printf("%02x", DataIn[i]); 
		printf("\n"); 

		if ( (params.bMonteCarlo) && (( params.emi== CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52) ||
					(params.emi == CSD_Algorithm_RAW_AES_128_CBC_CLEAR) ||
					(params.emi == CSD_Algorithm_RAW_0x4022) ||
					(params.emi == CSD_Algorithm_RAW_0x4023) ||
					(params.emi == CSD_Algorithm_RAW_AES_128_CBC_IV_0_CLEAR) ||
					(params.emi == CSD_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR) ||
					(params.emi == CSD_Algorithm_RAW_TDES_ABA_CBC_CLEAR)))
		{
			bCopyOut1 = true;	
		}

		for(i=0; i<params.numRuns; )
		{
#if (CSDAPI_VERSION_MEDIUM<2)
			csdEncryptData(params.emi, pxR2RKeyPathHandle, ivCbc, sizeIvCbc, DataIn, dataOut, sizeDataIn/*inputVector.sizeDataIn*/); 
#else
			/* NOT efficient to put if statement in a loop.  But OK for testing */
			if(params.keyType==NOCS3_HARNESS_TEST_KEY_SOFTWARE)
			{
				csdEncryptDataWithClearTextHostKey(
						xAlgorithm,
						xMode,
						xHostKey,
						sizeKey,
						ivCbc,
						sizeIvCbc,
						true,
						pxR2RKeyPathHandle,
						DataIn,
						dataOut,
						sizeDataIn)	;		
			}
			else if(params.keyType == NOCS3_HARNESS_TEST_KEY_KEYLADDER)
			{
				csdEncryptDataWithSecretContentKey(
						xAlgorithm,
						xMode,
						xR2RCipheredProtectingKeys,
						xcipheredContentKey,
						sizeCipherContentKey,
						ivCbc,
						sizeIvCbc,
						true,
						pxR2RKeyPathHandle,
						DataIn,
						dataOut,
						sizeDataIn);		
			}

#endif
			if(params.bMonteCarlo)
			{
				
				if(( params.emi== CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52) ||
					(params.emi == CSD_Algorithm_RAW_AES_128_CBC_CLEAR) ||
					(params.emi == CSD_Algorithm_RAW_0x4022) ||
					(params.emi == CSD_Algorithm_RAW_0x4023) ||
					(params.emi == CSD_Algorithm_RAW_AES_128_CBC_IV_0_CLEAR) ||
					(params.emi == CSD_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR) ||
					(params.emi == CSD_Algorithm_RAW_TDES_ABA_CBC_CLEAR)
				)
				{
					/*printf("encrypt result. %d..\n", i); */
	
					BKNI_Memcpy(DataIn, dataOut1, sizeDataIn);
					BKNI_Memcpy(ivCbc, dataOut, sizeDataIn);
#if (CSDAPI_VERSION_MEDIUM<2)
					csdEncryptData(params.emi, pxR2RKeyPathHandle, ivCbc, sizeIvCbc, DataIn, dataOut1, sizeDataIn/*inputVector.sizeDataIn*/); 
#else
					/* NOT efficient to put if statement in a loop.  But OK for testing */
					if(params.keyType==NOCS3_HARNESS_TEST_KEY_SOFTWARE)
					{
						csdEncryptDataWithClearTextHostKey(
								xAlgorithm,
								xMode,
								xHostKey,
								sizeKey,
								ivCbc,
								sizeIvCbc,
								true,
								pxR2RKeyPathHandle,
								DataIn,
								dataOut1,
								sizeDataIn)	;		
					}
					else if(params.keyType == NOCS3_HARNESS_TEST_KEY_KEYLADDER)
					{
						csdEncryptDataWithSecretContentKey(
								xAlgorithm,
								xMode,
								xR2RCipheredProtectingKeys,
								xcipheredContentKey,
								sizeCipherContentKey,
								ivCbc,
								sizeIvCbc,
								true,
								pxR2RKeyPathHandle,
								DataIn,
								dataOut1,
								sizeDataIn);		
					}
#endif
					BKNI_Memcpy(ivCbc, dataOut1, sizeDataIn);
					i+=2;
				}
				else 
				{
					if (( params.emi== CSD_Algorithm_RAW_AES_128_COUNTER_CLEAR) || ( params.emi== CSD_Algorithm_RAW_0x4024) )
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
				BKNI_Memcpy(DataIn, dataOut, sizeDataIn);
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

		if ( (params.bMonteCarlo) && ((params.emi == CSD_Algorithm_RAW_AES_128_CBC_CLEAR) ||\
					(params.emi == CSD_Algorithm_RAW_0x4022) ||\
					(params.emi == CSD_Algorithm_RAW_0x4023) ||\
					(params.emi == CSD_Algorithm_RAW_AES_128_CBC_IV_0_CLEAR) ))
		{
			bCopyOut1 = true;	
		}

		for(i=0; i<params.numRuns; )
		{
#if (CSDAPI_VERSION_MEDIUM<2)
			csdDecryptData(params.emi, pxR2RKeyPathHandle, ivCbc, sizeIvCbc, DataIn, dataOut, sizeDataIn/*inputVector.sizeDataIn*/);
#else
			/* NOT efficient to put if statement in a loop.  But OK for testing */
			if(params.keyType==NOCS3_HARNESS_TEST_KEY_SOFTWARE)
			{
				csdDecryptDataWithClearTextHostKey(
						xAlgorithm,
						xMode,
						xHostKey,
						sizeKey,
						ivCbc,
						sizeIvCbc,
						true,
						pxR2RKeyPathHandle,
						DataIn,
						dataOut,
						sizeDataIn)	;		
			}
			else if(params.keyType == NOCS3_HARNESS_TEST_KEY_KEYLADDER)
			{
				csdDecryptDataWithSecretContentKey(
						xAlgorithm,
						xMode,
						xR2RCipheredProtectingKeys,
						xcipheredContentKey,
						sizeCipherContentKey,
						ivCbc,
						sizeIvCbc,
						true,
						pxR2RKeyPathHandle,
						DataIn,
						dataOut,
						sizeDataIn
				);		
			}
#endif
			if(params.bMonteCarlo)
			{
				if(
					(params.emi == CSD_Algorithm_RAW_AES_128_CBC_IV_0_CLEAR) ||\
					(params.emi == CSD_Algorithm_RAW_0x4022) ||\
					(params.emi == CSD_Algorithm_RAW_0x4023) ||\
					(params.emi == CSD_Algorithm_RAW_AES_128_CBC_CLEAR) 
				)
				{
					BKNI_Memcpy(ivCbc, DataIn, sizeDataIn);
					BKNI_Memcpy(DataIn, dataOut1, sizeDataIn);
					
#if (CSDAPI_VERSION_MEDIUM<2)
					csdDecryptData(params.emi, pxR2RKeyPathHandle, ivCbc, sizeIvCbc, DataIn, dataOut1, sizeDataIn/*inputVector.sizeDataIn*/); 
#else
					/* NOT efficient to put if statement in a loop.  But OK for testing */
					if(params.keyType==NOCS3_HARNESS_TEST_KEY_SOFTWARE)
					{
						csdDecryptDataWithClearTextHostKey(
								xAlgorithm,
								xMode,
								xHostKey,
								sizeKey,
								ivCbc,
								sizeIvCbc,
								true,
								pxR2RKeyPathHandle,
								DataIn,
								dataOut1,
								sizeDataIn)	;		
					}
					else if(params.keyType == NOCS3_HARNESS_TEST_KEY_KEYLADDER)
					{
						csdDecryptDataWithSecretContentKey(
								xAlgorithm,
								xMode,
								xR2RCipheredProtectingKeys,
								xcipheredContentKey,
								sizeCipherContentKey,
								ivCbc,
								sizeIvCbc,
								true,
								pxR2RKeyPathHandle,
								DataIn,
								dataOut1,
								sizeDataIn
						);		
					}
#endif
					BKNI_Memcpy(ivCbc, DataIn, sizeDataIn);
					i+=2;
				}
				else 
				{
					if ( (params.emi==CSD_Algorithm_RAW_AES_128_COUNTER_CLEAR) || (params.emi==CSD_Algorithm_RAW_0x4024) )
					{
						nocs3_harness_test_utility_increase_16bytes_counter(ivCbc, sizeDataIn>>4);
					}
					i +=1;
				}

				if((params.emi == CSD_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR) || 
					(params.emi == CSD_Algorithm_RAW_TDES_ABA_CBC_CLEAR) )
				{
					BKNI_Memcpy(ivCbc, DataIn,  sizeDataIn);
				}
				
				BKNI_Memcpy(DataIn, dataOut, sizeDataIn);
				
			}
			else
			{
				i++;
				if((params.emi == CSD_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR) || 
					(params.emi == CSD_Algorithm_RAW_TDES_ABA_CBC_CLEAR) )
				{
					BKNI_Memcpy(ivCbc, DataIn,  sizeDataIn);
				}
				
				BKNI_Memcpy(DataIn, dataOut, sizeDataIn);
				
			}
		}
	}
	

	if ( bCopyOut1 == true )
	{
		BKNI_Memcpy(dataOut, dataOut1, sizeDataIn);
	}
			
	if(params.pOutFile!=NULL)
	{
		/* fwrite(dataOut, sizeof(unsigned char), sizeof(dataOut),params.pOutFile);*/
		fprintf(params.pOutFile, "0x");
		for(i=0; i<sizeDataIn; i++) 
		{
			fprintf(params.pOutFile, "%02x", dataOut[i]); 
		}
		fprintf(params.pOutFile, "\n"); 		
		
	}
	else
	{ /* -oc */
		printf("Output:\n"); 


		printf("0x");
		for(i=0; i<sizeDataIn; i++) printf("%02x", dataOut[i]); 
		printf("\n"); 		
		}

    printf ("[CSD][M] %s() and %s() done successfully.\n", gKeyFuncName[params.keyType], gCryptoFuncName[params.operation]);
 Clean_Up:
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

	csdTerminateHandle.initParam = &csdInit; 
	csdTerminateHandle.keyHandleToRelease = NULL; 
	csdTerminate(&csdTerminateHandle);
 Clean_Up_1:
    fflush(stdout);

   NEXUS_Platform_Uninit();
}


static void csd_harness_test_r2r_help (void)
{
	printf("\t%s\n\n", CSD_HELP_M_R2R_INST);
	printf("\t\t%s\n\n", CSD_HELP_M_R2R_OP);
	printf("\t\t%s\n\n", CSD_HELP_M_KEY);
	printf("\t\t%s\n\n", CSD_HELP_M_EMI);
	printf("\t\t%s\n\n", CSD_HELP_M_IVC);
	printf("\t\t%s\n\n", CSD_HELP_M_R2R_IFIC);
	printf("\t\t%s\n\n", CSD_HELP_M_R2R_OFOC);
	printf("\t\t%s\n\n", CSD_HELP_M_R2R_MT);
	printf("\t\t%s\n\n", CSD_HELP_M_R2R_RUNS);
	printf("\t\t%s\n\n", CSD_HELP_M_TV);
}

static NOCS3_HARNESS_TEST_STATUS_CODE csd_harness_test_r2r_parse_cmd_line (int argc, 
																		char* argv[], 
																		CSD_Harness_Test_R2R_Parameters * pParams)
{
	bool bOperationParsed = false;
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
	int cmdline_offset = 2;

	/* R2R command parser state machine */
	do 
	{
		NOCS3_HARNESS_TEST_PARSER_STATUS_CODE status;
		bool bCommandProcessed = false;

		/* Crypto Operation */
		NOCS3_HARNESS_TEST_PARSE_CRYPTO_OPERATION_REQUIRED;


		/* Key type */
		NOCS3_HARNESS_TEST_PARSE_KEY_TYPE_REQUIRED;

		/* EMI */
		NOCS3_COMMAND_PARSER_PARSE_NUMBER_REQUIRED (bEmiParsed, NOCS3_HARNESS_TEST_OPTION_EMI, &pParams->emi, "0x%x");

		/* IVC */
		NOCS3_COMMAND_PARSER_PARSE_ARRAY_OPTIONAL(bIvcParsed, \
												NOCS3_HARNESS_TEST_OPTION_IVC, \
												&pParams->iv[0], \
												&pParams->sizeIV, \
												NOCS3_HARNESS_TEST_MAX_IV_SIZE);

		/* Ic */
		NOCS3_COMMAND_PARSER_PARSE_ARRAY_OPTIONAL(bIcParsered, \
												NOCS3_HARNESS_TEST_OPTION_IC, \
												&pParams->data[0], \
												&pParams->sizeData, \
												CSD_HARNESS_TEST_R2R_MAX_DATA_SIZE);
		/* If */
		NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bIfParsered, NOCS3_HARNESS_TEST_OPTION_IF, &pParams->pInFile, "rb");

		/* Oc */
		NOCS3_COMMAND_PARSER_PARSE_SINGLE_OPTIONAL (bOcParsered, NOCS3_HARNESS_TEST_OPTION_OC);

		/* Of */
		NOCS3_COMMAND_PARSER_PARSE_FILE_OPTIONAL (bOfParsered, NOCS3_HARNESS_TEST_OPTION_OF, &pParams->pOutFile, "w");


		/* Monte Carlo */
		NOCS3_COMMAND_PARSER_PARSE_SINGLE_OPTIONAL (bMtParsed, NOCS3_HARNESS_TEST_OPTION_MT);


		/* NumRuns */
		NOCS3_COMMAND_PARSER_PARSE_NUMBER_REQUIRED (bNumRunsParsed, NOCS3_HARNESS_TEST_OPTION_RUNS, &pParams->numRuns, "%d");


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

