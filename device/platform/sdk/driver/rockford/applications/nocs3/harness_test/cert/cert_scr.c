/*============================================================================*\
  |* Name        : cert_scr.c
  |* Author      : Claudio Favi
  |* Email       : claudio.favi@nagra.com
  |* Version     :
  |* Copyright   : 2011 NagraVision
  |* Description : Reference code for cert_bridge application.
  |* Warning     : This code assumes a little endian 32-bit architecture.
  \*============================================================================*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <syslog.h>

#include "types.h"
#include "cert_scr.h"
#include "toollib.h"


#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_frontend.h"
#include "nexus_platform.h"
#include "nexus_parser_band.h"
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_composite_output.h"
#include "nexus_component_output.h"
#include "nexus_audio_mixer.h"
#include "nexus_audio_dac.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_output.h"
#include "nexus_spdif_output.h"
#include "nexus_keyladder.h"
#include "nexus_audio_input.h"
#include "nexus_video_window.h"
#include "nexus_message.h"
#include "nexus_memory.h"
#include "bstd.h"
#include "bkni.h"
#include "nexus_otpmsp.h"
#include "nexus_recpump.h"
#include "nexus_record.h"
#include "nexus_dma.h"
#include "bchp_sun_top_ctrl.h"
#include "nocs3x_csd.h"
#include "nocs3x_csd_impl.h"

#include "csd_harness_test.h"
#include "csd_harness_test_parser.h"
#include "csd_harness_test_utilities.h"
#include "nocs3_harness_test_utilities.h"
#include "csd_harness_test_ca_pvr.h"

#include "ca_cert_impl.h"
#include "ca_cert.h"


/* local functions */
void cert_scr_usage(char *argv[]);


/* defines */
#define CHECK_ARGV_BOUNDS(i, argc, argv, usage_function, retcode)	\
	if( (i)>=(argc)) {						\
		fprintf(stderr, "%s missing argument\n", argv[(i)-1]);	\
		usage_function(argv);					\
		return (retcode);					\
	}

#define DATA_MAXSIZE 4096


/* -------------------------------------------------------------------------- */
/* cert_scr_usage                                                             */
/* -------------------------------------------------------------------------- */
void cert_scr_usage(char *argv[]) 
{
	fprintf(stderr, 
		"cert -scr = SCR functionality. \n"
		"Usage:\n"
		"  cert -scr [-enc|-dec] [-skipUseKey] -emi EMI [-iv Value64|Value128] -videoPid VPID -audioPid APID -pcrPid PPID -if InFile|-ife -of OutFile -useParity ODD|EVEN|UNDEFINED\n"
		"\n"
		"where\n"
		"   -enc           = Perform encryption. \n"
		"   -dec           = Perform decryption. (default)\n"
		"   -skipUseKey    = Skip certUse*Key function call.\n"
		"   -emi EMI       = Encryption Mode Indicator.\n"
		"   -iv VAL        = IV value. Either 64bit or 128bit hexcoded.\n"
		"   -videoPid VPID = Sets video PID to be processed.\n"
		"   -audioPid APID = Sets audio PID to be processed.\n"
		"   -pcrPid PPID   = Sets PCR PID to be processed.\n"
		"   -if FILE       = read FILE as input.\n"
		"   -ife           = read input from frontend. Default.\n" 
		"   -of FILE       = use FILE as output.\n"
		"   -useParity PAR = use the given parity in certUse*Key function\n"
		"                    (if called: see -skipUseKey). Default: ODD\n"
		);
}

#if 0
/* -------------------------------------------------------------------------- */
/* cert_scr                                                                   */
/* -------------------------------------------------------------------------- */
int cert_scr(int argc, char *argv[])
{
	int i; 
	int stat;
	
	CERT_SCR xSCR;
	DATA_IO xIO;
	
	memset(&xSCR, 0, sizeof(CERT_SCR));
	memset(&xIO, 0, sizeof(DATA_IO));

	if( argc < 2 ) {
		cert_scr_usage(argv);
		return 100;
	}

	/* minimalistic argument checking */
	for(i=1; i<argc; i++) 
	{
		if (strcmp(argv[i], "-enc") == 0 ) 
		{
			xSCR.cmd.scr.dir = (U8)1;
		}
		else if ( strcmp(argv[i], "-dec") == 0 ) 
		{
			xSCR.cmd.scr.dir = (U8)0;
		}
		else if ( strcmp(argv[i], "-skipUseKey") == 0 ) 
		{
			xSCR.cmd.scr.skipusekey = (U8)1;
		}
		else if( strcmp(argv[i], "-emi") == 0 ) 
		{
			CHECK_ARGV_BOUNDS(i+1, argc, argv, cert_scr_usage, 1);
			long int val = strtol(argv[i+1], (char **)NULL, 0);
			if( errno == EINVAL ){
				perror("cert -scr");
				cert_scr_usage(argv);
				stat=50;
				goto clean_and_return;
			}
			xSCR.emi = (val & 0xFFFF);
			i++;
		}
		else if( strcmp(argv[i], "-iv") == 0 ) 
		{
			CHECK_ARGV_BOUNDS(i+1, argc, argv, cert_scr_usage, 2);
			int len = readHexStr(argv[i+1], xSCR.iv, sizeof(CU128));
			if(len <= 0) {
				fprintf(stderr, "\nInvalid iv: %s\n\n", argv[i+1]);
				stat=50;
				goto clean_and_return;
			}
			i++;
		}
		else if( strcmp(argv[i], "-videoPid") == 0 ) 
		{
			CHECK_ARGV_BOUNDS(i+1, argc, argv, cert_scr_usage, 3);
			long int val = strtol(argv[i+1], (char **)NULL, 0);
			if( errno == EINVAL ){
				perror("cert -scr");
				cert_scr_usage(argv);
				stat=50;
				goto clean_and_return;
			}
			xSCR.vidPid = (val & 0xFFFF);
			i++;
		}
		else if( strcmp(argv[i], "-audioPid") == 0 ) 
		{
			CHECK_ARGV_BOUNDS(i+1, argc, argv, cert_scr_usage, 4);
			long int val = strtol(argv[i+1], (char **)NULL, 0);
			if( errno == EINVAL ){
				perror("cert -scr");
				cert_scr_usage(argv);
				stat=50;
				goto clean_and_return;
			}
			xSCR.audPid = (val & 0xFFFF);
			i++;
		}
		else if( strcmp(argv[i], "-pcrPid") == 0 ) 
		{
			CHECK_ARGV_BOUNDS(i+1, argc, argv, cert_scr_usage, 5);
			long int val = strtol(argv[i+1], (char **)NULL, 0);
			if( errno == EINVAL ){
				perror("cert -scr");
				cert_scr_usage(argv);
				stat=50;
				goto clean_and_return;
			}
			xSCR.pcrPid = (val & 0xFFFF);
			i++;
		}
		else if( strcmp(argv[i], "-if") == 0 ) 
		{
			CHECK_ARGV_BOUNDS(i+1, argc, argv, cert_scr_usage, 6);
			xIO.din_fn = argv[i+1];
			xSCR.cmd.scr.swtch = 0;
			xSCR.cmd.scr.src = 1;
			i++;
		}
		else if( strcmp(argv[i], "-ife") == 0 ) 
		{
			xSCR.cmd.scr.swtch = 0;
			xSCR.cmd.scr.src = 0;
		}
		else if( strcmp(argv[i], "-of") == 0 ) 
		{
			CHECK_ARGV_BOUNDS(i+1, argc, argv, cert_scr_usage, 7);
			xIO.dout_fn = argv[i+1];
			xSCR.cmd.scr.swtch = 0;
			xSCR.cmd.scr.dest = 1;
			i++;
		}
		else if( strcmp(argv[i], "-useParity") == 0 ) 
		{
			CHECK_ARGV_BOUNDS(i+1, argc, argv, cert_scr_usage, 8);
			if( strcmp(argv[i+1], "ODD") == 0)
				xSCR.cmd.scr.parity = 0; 
			else if( strcmp(argv[i+1], "EVEN") == 0)
				xSCR.cmd.scr.parity = 1; 
			else /* UNDEFINED parity */
				xSCR.cmd.scr.parity = 3;
			i++;
		}
		else if( strcmp(argv[i], "-help") == 0 ) 
		{
			cert_scr_usage(argv);
			stat=0;
			goto clean_and_return;
		} 
		else 
		{
			fprintf(stderr, "\n%s: unknown option.\n\n", argv[i]);
			cert_scr_usage(argv);
			stat=100;
			goto clean_and_return;
		}
	}

	/* more argument checking */
	/*emi*/
	if(xSCR.emi==0x0000) {
		fprintf(stderr, "\nPlease specify a valid EMI with -emi.\n\n");
		cert_scr_usage(argv);
		stat=10;
		goto clean_and_return;
	}
	if(xSCR.vidPid==0x0000) {
		fprintf(stderr, "\nPlease specify a valid -videoPid.\n\n");
		cert_scr_usage(argv);
		stat=11;
		goto clean_and_return;
	}
	if(xSCR.audPid==0x0000) {
		fprintf(stderr, "\nPlease specify a valid -audioPid.\n\n");
		cert_scr_usage(argv);
		stat=12;
		goto clean_and_return;
	}
	if(xSCR.pcrPid==0x0000) {
		fprintf(stderr, "\nPlease specify a valid -pcrPid.\n\n");
		cert_scr_usage(argv);
		stat=13;
		goto clean_and_return;
	}

	/* debug printing options */
/* #if 1 */
	fprintf(stderr, "dir: %s\n", (xSCR.cmd.scr.dir)?"enc":"dec");
	fprintf(stderr, "certUse*Key: %s\n", (xSCR.cmd.scr.skipusekey)?"skipped":"in use");
	if(xSCR.cmd.scr.swtch) {
		fprintf(stderr, "input src: ram\n");
		fprintf(stderr, "output dest: ram\n");

	} else {
		fprintf(stderr, "input src: %s\n", (xSCR.cmd.scr.src)?"file":"frontend");
		if(xIO.din_fn!=NULL)
			fprintf(stderr, "input filename: %s\n", xIO.din_fn);
		fprintf(stderr, "output dest: %s\n", (xSCR.cmd.scr.dest)?"file":"none");
		if(xIO.dout_fn!=NULL)
			fprintf(stderr, "output filename: %s\n", xIO.dout_fn);
	}
	if ( xSCR.cmd.scr.parity == 0 )
		fprintf(stderr, "keyParity: ODD\n");
	else if ( xSCR.cmd.scr.parity == 1 )
		fprintf(stderr, "keyParity: EVEN\n");
	else 
		fprintf(stderr, "keyParity: UNDEFINED\n");
	
	fprintf(stderr, "emi: %04x\n", xSCR.emi);
	fprintf(stderr, "vidPid: %02x\n", xSCR.vidPid);
	fprintf(stderr, "audPid: %02x\n", xSCR.audPid);
	fprintf(stderr, "pcrPid: %02x\n", xSCR.pcrPid);
	display_msg_and_buf(LOG_DEBUG+1, "iv:", xSCR.iv, sizeof(CU128));
/* #endif */

	stat = cert_scr_ciph(&xSCR, &xIO);


clean_and_return:
	if(xIO.din != NULL)
		free(xIO.din);
	if(xIO.dout != NULL)
		free(xIO.dout);

	if(stat!=0) {
		fprintf(stderr, "[CERT] [EF] Error status=%d\n", stat);
	} else {
		fprintf(stdout, "[CERT] [M] scr operation completed.\n");
	}

	return stat;
}
#endif


/** -------------------------------------------------------------------------- 
 * cert_scr_ciph   
 * @brief
 *   This function shall perform scr operation on data in xpIO as described
 *   by xpSCR argument.
 * 
 * @param[in] xpSCR
 *   This structure defines which scr operation to perform.
 * 
 * @param[inout] pxIO
 *   This structure contains the input and output data. 
 *   Depending on xpSCR input data may come from:
 *       - file in which case the filename is passed in pxIO
 *       - ram in which case a preallocated buffer is passed in pxIO
 *       - frontend in which case no input information is passed in pxIO
 *   Ouput data should be directed to video output as well as written to (depending on xpSCR):
 *       - file in which case the filename is passed in pxIO
 *       - ram in which case a preallocated buffer is passed in pxIO
 *       - none in which case no output information is passed in pxIO
 *  
 *        
 * @retval 0
 *    On success
 * @retval >0
 *    On expected failure
 * @retval <0
 *    On unexpected failure
 * 
 * @note
 * Callers to this function are:
 *   -cert_scr (this file)
 *   -socSCR (soccmd.c)    
 *                                                       
 * -------------------------------------------------------------------------- */
int cert_scr_ciph(CERT_SCR *xpSCR, DATA_IO *xpIO)
{
	/* code taken from cert_scr */
	NOCS3_HARNESS_TEST_STATUS_CODE  nocs3_status;
 	TCsdInitParameters csdInit;
	TCsdStatus status;
	TCsdTerminateParameters csdTerminateHandle; 
	TCertKeyPathHandle KeyPathHandle;
	TCsdScrKeyPathHandle scrHandle;
	CSD_Harness_Test_CA_PVR_Configuration caPvrConfig, *pCaPvrConfig = &caPvrConfig;
	unsigned int ivSize;
	TCsdUnsignedInt8* dataIv;
	NEXUS_SecurityClearKey ivKey;
	TCertKeyParity scr_parity;
	TCertStatus useKeyStatus = CERT_ERROR;

	/* csd_harness_test_scr_dump_paramters (&params);*/
	memset (&csdInit, 0, sizeof(TCsdInitParameters));
	memset (&KeyPathHandle, 0, sizeof(TCertKeyPathHandle));
	memset (&caPvrConfig, 0, sizeof(CSD_Harness_Test_CA_PVR_Configuration));
	memset (&scrHandle, 0, sizeof(TCsdScrKeyPathHandle));

	status=csdInitialize(&csdInit);
	if( (status!=CSD_NO_ERROR) && (status !=CSD_ERROR_DRIVER_ALREADY_INITIALIZED) )
	{
		printf("Error: cannot initialize CSD driver due to error %d!\n", status); 
		return -1;
	}	

	KeyPathHandle.scrHandle=&scrHandle;
	KeyPathHandle.scrHandle->initHandle = &csdInit;

	/*NEXUS_Memory_Allocate(sizeof(TCsdUnsignedInt8)*16, NULL, (void *)&dataIv);
	  memset (dataIv, 0, sizeof(unsigned char)*16);*/

	switch(xpSCR->cmd.scr.parity) {
	case 0:
		scr_parity=CERT_KEY_PARITY_ODD;
		break;
	case 1:
		scr_parity=CERT_KEY_PARITY_EVEN;
		break;
	default:
		scr_parity=CERT_KEY_PARITY_UNDEFINED;
	}

	KeyPathHandle.dest = NEXUS_SecurityAlgorithmConfigDestination_eMem2Mem;
	KeyPathHandle.keySlotEngine = NEXUS_SecurityEngine_eM2m;
	KeyPathHandle.bLoadScr = true;
	if(xpSCR->cmd.scr.dir==1)
	{
		/* HERE */
		if(!xpSCR->cmd.scr.skipusekey)
			useKeyStatus = certUseEncryptionKey(
				xpSCR->emi,
				&KeyPathHandle,
				scr_parity
				);
	}
	else
	{	
		if(!xpSCR->cmd.scr.skipusekey)
			useKeyStatus = certUseDecryptionKey(
				xpSCR->emi,
				&KeyPathHandle,
				scr_parity
				);
		
	}
	if( useKeyStatus != CERT_NO_ERROR ) 
		goto cleanup_and_exit;
	printf("useKeyStatus = %d\n", useKeyStatus);
	

	dataIv = xpSCR->iv;

	/* load IV key if needed */
	if(
		(xpSCR->emi == CSD_Algorithm_AES_128_CBC_IV_0_SCTE52) ||
		(xpSCR->emi == CSD_Algorithm_AES_128_CBC_CLEAR) ||
		(xpSCR->emi == CSD_Algorithm_RAW_0x4022) ||
		(xpSCR->emi == CSD_Algorithm_RAW_0x4023) ||
		(xpSCR->emi == CSD_Algorithm_RAW_0x4024) ||
		(xpSCR->emi == CSD_Algorithm_RAW_AES_128_CBC_CLEAR) ||
		(xpSCR->emi == CSD_Algorithm_RAW_AES_128_CBC_IV_0_CLEAR) ||
		(xpSCR->emi == CSD_Algorithm_M2M_AES_128_CBC_CLEAR) ||
		(xpSCR->emi == CSD_Algorithm_M2M_AES_128_CTR_CLEAR) ||

		(xpSCR->emi == CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52) ||
		(xpSCR->emi == CSD_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR) ||
		(xpSCR->emi == CSD_Algorithm_M2M_TDES_ABA_CBC_CLEAR)
		
		)
	{
	
        	ivSize = 0;

        	if( /* (xpSCR->emi == CSD_Algorithm_AES_128_CBC_IV_0_SCTE52) || */
		    (xpSCR->emi == CSD_Algorithm_AES_128_CBC_CLEAR) ||
		    (xpSCR->emi == CSD_Algorithm_RAW_0x4022) ||
		    (xpSCR->emi == CSD_Algorithm_RAW_0x4023) ||
		    (xpSCR->emi == CSD_Algorithm_RAW_0x4024) ||
		    (xpSCR->emi == CSD_Algorithm_RAW_AES_128_CBC_CLEAR) ||
		    /* (xpSCR->emi == CSD_Algorithm_RAW_AES_128_CBC_IV_0_CLEAR) || */
		    (xpSCR->emi == CSD_Algorithm_M2M_AES_128_CBC_CLEAR) ||
		    (xpSCR->emi == CSD_Algorithm_M2M_AES_128_CTR_CLEAR) )

        	{
        		ivSize = 16;
        	}
        	else if((xpSCR->emi == CSD_Algorithm_M2M_TDES_ABA_CBC_CLEAR) /* || */
			/* (xpSCR->emi == CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52) || */
			/* (xpSCR->emi == CSD_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR) */
			)
        	{
        		ivSize = 8;
        	}   

		BKNI_Memset((void *)&ivKey, 0, sizeof(ivKey));
		NEXUS_Security_GetDefaultClearKey(&ivKey);
		ivKey.keyIVType 	= NEXUS_SecurityKeyIVType_eIV;
		ivKey.keyEntryType = NEXUS_SecurityKeyType_eIv;
   		ivKey.keySize = 16; /*xInitializationVectorSize;*/
		
		if(ivSize == 8)
			BKNI_Memcpy(&(ivKey.keyData[8]), dataIv, ivSize);
		else 
			BKNI_Memcpy(ivKey.keyData, dataIv, ivSize);
    
		/* Load IV. */
    		if (NEXUS_Security_LoadClearKey(KeyPathHandle.scrHandle->keyHandle , &ivKey) != 0) 
		{
        		printf("\nLoad IV failed \n");
			nocs3_status = NOCS3_HARNESS_TEST_STATUS_CODE_FAIL;
        		goto cleanup_and_exit;
    		}
	}

	if(xpSCR->cmd.scr.dir == 1)
		pCaPvrConfig->bEncryption = true;
	else
		pCaPvrConfig->bEncryption = false;
	
	pCaPvrConfig->bIsDsc = false;
	pCaPvrConfig->emi = xpSCR->emi;
	pCaPvrConfig->audioPID = xpSCR->audPid;
	pCaPvrConfig->videoPID = xpSCR->vidPid;
	pCaPvrConfig->pcrPID = xpSCR->pcrPid;
	pCaPvrConfig->bEcmEnabled = false;
	pCaPvrConfig->ecmPID = 0;
	pCaPvrConfig->pScrKeyHandle = KeyPathHandle.scrHandle;
	pCaPvrConfig->pDscVideoKeyHandle = NULL;
	pCaPvrConfig->pDscAudioKeyHandle = NULL;
	pCaPvrConfig->pInFile = NULL;
	pCaPvrConfig->pOutFile = NULL;
	pCaPvrConfig->pVector = NULL;
	pCaPvrConfig->pIO = xpIO;

	nocs3_status = csd_harness_test_ca_pvr (pCaPvrConfig);
	if ( nocs3_status != NOCS3_HARNESS_TEST_STATUS_CODE_SUCCESS )
	{
		/* Parser error code */
		printf ("SCR test returns error code of %08x\n", nocs3_status);
	}

	/* NEXUS_Memory_Free(dataIv); */

cleanup_and_exit:
	/* Clean up */
	csdTerminateHandle.initParam = &csdInit; 
	csdTerminateHandle.keyHandleToRelease = NULL; 
	csdTerminate(&csdTerminateHandle);

	if(useKeyStatus != CERT_NO_ERROR) 
		return -1;
	if( nocs3_status != NOCS3_HARNESS_TEST_STATUS_CODE_SUCCESS )
		return -2;

	return 0;
}

/*** end of file ***/
