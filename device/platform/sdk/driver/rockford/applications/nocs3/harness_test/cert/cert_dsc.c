/*============================================================================*\
|* Name        : cert_dsc.c
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
#include "cert_dsc.h"
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
void cert_dsc_usage(char *argv[]);


/* defines */
#define CHECK_ARGV_BOUNDS(i, argc, argv, usage_function, retcode) \
if( (i)>=(argc)) { \
	fprintf(stderr, "%s missing argument\n", argv[(i)-1]); \
	usage_function(argv); \
	return (retcode); \
}

#define DATA_MAXSIZE 4096

#if 0
/* -------------------------------------------------------------------------- */
/* cert_dsc_usage                                                             */
/* -------------------------------------------------------------------------- */
void cert_dsc_usage(char *argv[]) 
{
	fprintf(stderr, 
			"cert -dsc = DSC functionality. \n"
			"Usage:\n"
			"  cert -dsc [-skipUseKey] -emi EMI -videoPid VPID -audioPid APID -pcrPid PPID -ecmPid EPID -pecm TRUE|FALSE -if InFile|-ife [-of OutFile] -useParity ODD|EVEN -ecm2cert ecm2certcmds.csv\n"
			"\n"
			"where\n"
			"   -skipUseKey      = Skip certUse*Key function call.\n"
			"   -emi EMI         = Encryption Mode Indicator.\n"
			"   -videoPid VPID   = Sets video PID to be processed.\n"
			"   -audioPid APID   = Sets audio PID to be processed.\n"
			"   -pcrPid PPID     = Sets PCR PID to be processed.\n"
			"   -ecmPid EPID     = Set ECM PID to be processed. Optional if the stream is not\n"
			"                      scrambled or if there are no ECMs in the stream.\n"
			"   -pecm TRUE|FALSE = Whether to process the ECMs or not. If set to FALSE do not\n"
			"                      perform the ECM processing, instead the key is only set once\n"
			"                      at startup with a certUseDecryptionKey.\n"
			"   -if FILE         = read FILE as input.\n"
			"   -ife             = read input from frontend. Default.\n" 
			"   -of FILE         = output goes to FILE as well as to the video output.\n"
			"   -useParity PAR   = Only used when pecm is set to FALSE; indicates what key\n"
			"                      parity should be used on the certUseDecryptionKey call.\n"
			"   -ecm2cert csv    = Indicates the path and filename of the .CSV files\n"
			"                      which contains the required CERT commands for\n"
			"                      each control_word_?_index to output the \n"
			"                      corresponding CW from the CERT.\n"
	);
}


/* -------------------------------------------------------------------------- */
/* cert_dsc                                                                   */
/* -------------------------------------------------------------------------- */
int cert_dsc(int argc, char *argv[])
{
	int i; 
	int stat;
	
	char *e2c_fn=NULL;
	CERT_DSC xDSC;
	DATA_IO xIO;
	
	memset(&xDSC, 0, sizeof(CERT_DSC));
	memset(&xIO, 0, sizeof(DATA_IO));

	if( argc < 2 ) {
		cert_dsc_usage(argv);
		return 100;
	}

	/* minimalistic argument checking */
	for(i=1; i<argc; i++) 
	{
		if( strcmp(argv[i], "-emi") == 0 ) 
		{
			CHECK_ARGV_BOUNDS(i+1, argc, argv, cert_dsc_usage, 1);
			long int val = strtol(argv[i+1], (char **)NULL, 0);
			if( errno == EINVAL ){
				perror("cert -dsc");
				cert_dsc_usage(argv);
				stat=50;
				goto clean_and_return;
			}
			xDSC.emi = (val & 0xFFFF);
			i++;
		}
		else if ( strcmp(argv[i], "-skipUseKey") == 0 ) 
		{
			xDSC.cmd.dsc.skipusekey = (U8)1;
		}
		else if( strcmp(argv[i], "-videoPid") == 0 ) 
		{
			CHECK_ARGV_BOUNDS(i+1, argc, argv, cert_dsc_usage, 3);
			long int val = strtol(argv[i+1], (char **)NULL, 0);
			if( errno == EINVAL ){
				perror("cert -dsc");
				cert_dsc_usage(argv);
				stat=50;
				goto clean_and_return;
			}
			xDSC.vidPid = (val & 0xFFFF);
			i++;
		}
		else if( strcmp(argv[i], "-audioPid") == 0 ) 
		{
			CHECK_ARGV_BOUNDS(i+1, argc, argv, cert_dsc_usage, 4);
			long int val = strtol(argv[i+1], (char **)NULL, 0);
			if( errno == EINVAL ){
				perror("cert -dsc");
				cert_dsc_usage(argv);
				stat=50;
				goto clean_and_return;
			}
			xDSC.audPid = (val & 0xFFFF);
			i++;
		}
		else if( strcmp(argv[i], "-pcrPid") == 0 ) 
		{
			CHECK_ARGV_BOUNDS(i+1, argc, argv, cert_dsc_usage, 5);
			long int val = strtol(argv[i+1], (char **)NULL, 0);
			if( errno == EINVAL ){
				perror("cert -dsc");
				cert_dsc_usage(argv);
				stat=50;
				goto clean_and_return;
			}
			xDSC.pcrPid = (val & 0xFFFF);
			i++;
		}
		else if( strcmp(argv[i], "-ecmPid") == 0 ) 
		{
			CHECK_ARGV_BOUNDS(i+1, argc, argv, cert_dsc_usage, 5);
			long int val = strtol(argv[i+1], (char **)NULL, 0);
			if( errno == EINVAL ){
				perror("cert -dsc");
				cert_dsc_usage(argv);
				stat=50;
				goto clean_and_return;
			}
			xDSC.ecmPid = (val & 0xFFFF);
			i++;
		}
		else if( strcmp(argv[i], "-pecm") == 0 ) 
		{
			CHECK_ARGV_BOUNDS(i+1, argc, argv, cert_dsc_usage, 6);
			if( strcmp(argv[i+1], "TRUE") == 0)
				xDSC.cmd.dsc.pecm = 1;
			else 
				xDSC.cmd.dsc.pecm = 0;
			i++;
		}
		else if( strcmp(argv[i], "-if") == 0 ) 
		{
			CHECK_ARGV_BOUNDS(i+1, argc, argv, cert_dsc_usage, 6);
			xIO.din_fn = argv[i+1];
			xDSC.cmd.dsc.swtch = 0;
			xDSC.cmd.dsc.src = 1;
			i++;
		}
		else if( strcmp(argv[i], "-ife") == 0 ) 
		{
			xDSC.cmd.dsc.swtch = 0;
			xDSC.cmd.dsc.src = 0;
		}
		else if( strcmp(argv[i], "-of") == 0 ) 
		{
			CHECK_ARGV_BOUNDS(i+1, argc, argv, cert_dsc_usage, 7);
			xIO.dout_fn = argv[i+1];
			xDSC.cmd.dsc.swtch = 0;
			xDSC.cmd.dsc.dest = 1;
			i++;
		}
		else if( strcmp(argv[i], "-useParity") == 0 ) 
		{
			CHECK_ARGV_BOUNDS(i+1, argc, argv, cert_dsc_usage, 8);
			if( strcmp(argv[i+1], "ODD") == 0)
				xDSC.cmd.dsc.parity = 0; 
			else
				xDSC.cmd.dsc.parity = 1; 
			i++;
		}
		else if( strcmp(argv[i], "-ecm2cert") == 0 ) 
		{
			CHECK_ARGV_BOUNDS(i+1, argc, argv, cert_dsc_usage, 6);
			e2c_fn = argv[i+1];
			i++;
		}
		else if( strcmp(argv[i], "-help") == 0 ) 
		{
			cert_dsc_usage(argv);
			stat=0;
			goto clean_and_return;
		} 
		else 
		{
			fprintf(stderr, "\n%s: unknown option.\n\n", argv[i]);
			cert_dsc_usage(argv);
			stat=100;
			goto clean_and_return;
		}
	}

	/* missing arguments check */
	if(xDSC.emi==0x0000) {
		fprintf(stderr, "\nPlease specify a valid EMI with -emi.\n\n");
		cert_dsc_usage(argv);
		stat=10;
		goto clean_and_return;
	}
	if(xDSC.vidPid==0x0000) {
		fprintf(stderr, "\nPlease specify a valid -videoPid.\n\n");
		cert_dsc_usage(argv);
		stat=11;
		goto clean_and_return;
	}
	if(xDSC.audPid==0x0000) {
		fprintf(stderr, "\nPlease specify a valid -audioPid.\n\n");
		cert_dsc_usage(argv);
		stat=12;
		goto clean_and_return;
	}
	if(xDSC.pcrPid==0x0000) {
		fprintf(stderr, "\nPlease specify a valid -pcrPid.\n\n");
		cert_dsc_usage(argv);
		stat=13;
		goto clean_and_return;
	}
	if(xDSC.ecmPid==0x0000 && xDSC.cmd.dsc.pecm==1) {
		fprintf(stderr, "\nPlease specify a valid -ecmPid.\n\n");
		cert_dsc_usage(argv);
		stat=13;
		goto clean_and_return;
	}
	if(e2c_fn==NULL && xDSC.cmd.dsc.pecm==1) {
		fprintf(stderr, "\nPlease specify -ecm2cert.\n\n");
		cert_dsc_usage(argv);
		stat=13;
		goto clean_and_return;
	}


	/* debug printing options */
/* #if 1 */
	fprintf(stderr, "dir: %s\n", (xDSC.cmd.dsc.dir)?"enc":"dec");
	fprintf(stderr, "certUse*Key: %s\n", (xDSC.cmd.dsc.skipusekey)?"skipped":"in use");
	if(xDSC.cmd.dsc.swtch) {
		fprintf(stderr, "input src: ram\n");
		fprintf(stderr, "output dest: ram\n");

	} else {
		fprintf(stderr, "input src: %s\n", (xDSC.cmd.dsc.src)?"file":"frontend");
		if(xIO.din_fn!=NULL)
			fprintf(stderr, "input filename: %s\n", xIO.din_fn);
		fprintf(stderr, "output dest: %s\n", (xDSC.cmd.dsc.dest)?"file":"none");
		if(xIO.dout_fn!=NULL)
			fprintf(stderr, "output filename: %s\n", xIO.dout_fn);
	}
	if ( xDSC.cmd.dsc.parity == 0 )
		fprintf(stderr, "keyParity: ODD\n");
	else 
		fprintf(stderr, "keyParity: EVEN\n");
	fprintf(stderr, "emi: %04x\n", xDSC.emi);
	fprintf(stderr, "vidPid: %02x\n", xDSC.vidPid);
	fprintf(stderr, "audPid: %02x\n", xDSC.audPid);
	fprintf(stderr, "pcrPid: %02x\n", xDSC.pcrPid);
	fprintf(stderr, "ecmPid: %02x\n", xDSC.ecmPid);
	fprintf(stderr, "ecm processing: %s\n", (xDSC.cmd.dsc.pecm)?"enabled":"disabled");
	if(e2c_fn!=NULL)
		fprintf(stderr, "ecm2cert csv file: %s\n", e2c_fn);
/* #endif */	

	stat = cert_dsc_ciph(&xDSC, &xIO, e2c_fn);


clean_and_return:
	if(xIO.din != NULL)
		free(xIO.din);
	if(xIO.dout != NULL)
		free(xIO.dout);

	if(stat!=0) {
		fprintf(stderr, "[CERT] [EF] Error status=%d\n", stat);
	} else {
		fprintf(stdout, "[CERT] [M] dsc operation completed.\n");
	}

	return stat;
}
#endif

/* -------------------------------------------------------------------------- 
 * cert_dsc_ciph      
 * @brief
 *   This function shall perform dsc operation on data in xpIO as described
 *   by xpDSC argument.
 * 
 * @param[in] xpDSC
 *   This structure defines which dsc operation to perform.
 * 
 * @param[inout] pxIO
 *   This structure contains the input and output data. 
 *   Depending on xpDSC input data may come from:
 *       - file in which case the filename is passed in pxIO
 *       - ram in which case a preallocated buffer is passed in pxIO
 *       - frontend in which case no input information is passed in pxIO
 *   Ouput data should be directed to video output as well as written to (depending on xpDSC):
 *       - file in which case the filename is passed in pxIO
 *       - ram in which case a preallocated buffer is passed in pxIO
 *       - none in which case no output information is passed in pxIO
 *  
 * @param[in] ecm2cert_csv_filename 
 *   When ECMs processing is enabled (see xpDSC.cmd) use this file to read the necessary commands to send to
 *   CERT for each ecm.
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
 *   -cert_dsc (this file) *if enabled*
 *   -socDSC (soccmd.c)                                                                                                                   
 * -------------------------------------------------------------------------- */
int cert_dsc_ciph(CERT_DSC *xpDSC, DATA_IO *xpIO, const char *ecm2cert_csv_filename)
{
	/* code taken from cert_dsc in cert_harness_test_main.c */
	NOCS3_HARNESS_TEST_STATUS_CODE  nocs3_status;
	NOCS3_Harness_Test_Test_Vector test_vector, * pVector = &test_vector;
 	TCsdInitParameters csdInitVideo;
 	TCsdInitParameters csdInitAudio;
	TCsdStatus status;
	TCsdTerminateParameters csdTerminateHandle; 
	TCertKeyPathHandle keyPathHandle;
	TCsdDscKeyPathHandle					dscAudioHandle;
	TCsdDscKeyPathHandle					dscVideoHandle;    
	FILE* pInFile=NULL;
	FILE* pOutFile=NULL;
	FILE* pCsvFile=NULL;
       
	CSD_Harness_Test_CA_PVR_Configuration caPvrConfig, *pCaPvrConfig = &caPvrConfig;

       
	memset (&caPvrConfig, 0, sizeof(CSD_Harness_Test_CA_PVR_Configuration));
	memset (&csdInitVideo, 0, sizeof(TCsdInitParameters));
	memset (&csdInitAudio, 0, sizeof(TCsdInitParameters));
	memset (&dscAudioHandle, 0, sizeof(TCsdDscKeyPathHandle));
	memset (&dscVideoHandle, 0, sizeof(TCsdDscKeyPathHandle));
	memset (&keyPathHandle, 0, sizeof(TCertKeyPathHandle));
    
	
	status=csdInitialize(&csdInitVideo);
	if( (status!=CSD_NO_ERROR) && (status !=CSD_ERROR_DRIVER_ALREADY_INITIALIZED) )
	{
		printf("Error: cannot initialize CSD driver for video due to error %d!\n", status); 
		return -1;
	}		
	status=csdInitialize(&csdInitAudio);
	if( (status!=CSD_NO_ERROR) && (status !=CSD_ERROR_DRIVER_ALREADY_INITIALIZED) )
	{
		printf("Error: cannot initialize CSD driver for Audio due to error %d!\n", status); 
		return -2;
	}	

	if( (xpDSC->cmd.dsc.swtch==0) && 
	    (xpDSC->cmd.dsc.src==1) ) /* source is a file */
	{
		if( (pInFile  = fopen( xpIO->din_fn, "r" )) == NULL )
		{
			printf( "The file %s was not opened\n", xpIO->din_fn);
			printf( "ERROR: Could not create temporary storage.\n");
			return -3;
		}
	}

	if( (xpDSC->cmd.dsc.swtch==0) && 
	    (xpDSC->cmd.dsc.dest==1) ) /* destination is a file */
	{
        	if( (pOutFile  = fopen( xpIO->dout_fn, "w+" )) == NULL )
        	{
        		printf( "The file %s was not opened\n", xpIO->dout_fn);
        		printf( "ERROR: Could not create temporary storage.\n");
        		return -4;
        	}    
	}

	/* if(xpDSC.cmd.dsc.pecm == 1) { */
	/* 	if( (pCsvFile = fopen( csv_file, "r" )) == NULL ) { */
	/* 		perror(csv_file); */
	/* 		return -5; */
	/* 	} */
	/* } */

	

	keyPathHandle.dscVideoHandle = &dscVideoHandle;
	keyPathHandle.dscAudioHandle = &dscAudioHandle;

	keyPathHandle.dscVideoHandle->initHandle = &csdInitVideo;
	keyPathHandle.dscAudioHandle->initHandle = &csdInitAudio;
    


	pCaPvrConfig->bIsDsc = true;
	pCaPvrConfig->bEncryption = false;
	pCaPvrConfig->emi = xpDSC->emi;
	pCaPvrConfig->audioPID = xpDSC->audPid;
	pCaPvrConfig->videoPID = xpDSC->vidPid;
	pCaPvrConfig->pcrPID = xpDSC->pcrPid;
	pCaPvrConfig->bEcmEnabled = xpDSC->cmd.dsc.pecm;
	pCaPvrConfig->ecmPID = xpDSC->ecmPid;
	pCaPvrConfig->pDscKeyHandle= &keyPathHandle;	   
	pCaPvrConfig->pInFile = pInFile;
	pCaPvrConfig->pOutFile = pOutFile;
	pCaPvrConfig->pVector = pVector;
	pCaPvrConfig->sizeIV = 0;
	pCaPvrConfig->use_odd_parity=(xpDSC->cmd.dsc.parity==0)?true:false;
	pCaPvrConfig->pIO = xpIO;

	nocs3_status = csd_harness_test_ca_pvr (pCaPvrConfig);

	

	/* Clean up */
  	csdTerminateHandle.initParam = &csdInitVideo; 
	csdTerminateHandle.keyHandleToRelease = NULL; 
	csdTerminate(&csdTerminateHandle);

  	/* csdTerminateHandle.initParam = &csdInitAudio;  */
	/* csdTerminateHandle.keyHandleToRelease = NULL;  */
	/* csdTerminate(&csdTerminateHandle); */

	if (pInFile)
	{
		fclose (pInFile);
	}
	if (pOutFile)
	{
		fclose (pOutFile);
	}
	if (pCsvFile)
	{
		fclose (pCsvFile);
	}

	if ( nocs3_status != NOCS3_HARNESS_TEST_STATUS_CODE_SUCCESS )
	{
		/* Parser error code */
		printf ("DSC test returns error code of %08x\n", nocs3_status);
		return -1;
	}
	
	return 0;

}


/*** end of file ***/
