#include <bstd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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

#include "types.h"
#include "tlvlib.h"
#include "proclib.h"
#include "toollib.h"
#include "cert_bridge.h"
#include "ca_cert_impl.h"
#include "ca_cert.h"

#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h> /* or <netinet/in.h> */
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <syslog.h>

void usage(char *argv[]);

NEXUS_MessageHandle msg;
NEXUS_DisplayHandle display;
NEXUS_DisplaySettings displaySettings;
NEXUS_VideoWindowHandle window;
NEXUS_VideoDecoderHandle vdecode;
NEXUS_AudioDecoderHandle pcmDecoder, compressedDecoder;
NEXUS_PlatformConfiguration *pNexusConfig;
NEXUS_DmaHandle dma = NULL;
NEXUS_StcChannelHandle stcChannel;



bool use_sw_keys_flag = true;
int gServerSock; 
int g_infd; /* input file descriptor for "cert -bridge -n"  */
int g_outfd; /* output file descriptor for "cert -bridge -n"*/
int g_numchld; /* number of childre spawn in netmode */

FILE* inputFile; /* This is used for "cert -bridge -f" */
FILE* outputFile;/* This is used for "cert -bridge -f" */

#define INBUFSIZE  (1<<16) 

#define OUTBUFSIZE (1<<16)


void copyString(char* csvInput, char* pString, unsigned int size)
{
	/* Clear the csvInput and copy new string. */
	memset(csvInput, 0, sizeof(char)*8);
	strncpy(csvInput, pString, size);	
	return;
}

void parseCertCsvFile(
	FILE* csvFile,
	CERT_Csv* certData
	)
{
	char String[512], *pString=String;
	unsigned int i, j;
	char csvInput[8];
	unsigned int base;
	unsigned long temp;

	unsigned int controlWordIndex=0;
	
	/* Read in a line from the csv. */
	while((fscanf (csvFile, "%s", pString))!=EOF )
	{
		/* Calculate the string length of the index. */
		for(i=0;;i++)
		{
			if(strncmp(&(pString[i]), ",", 1) ==0)
				break;
		}
		
		/* Clear the csvInput and copy new string. */
		copyString(csvInput, pString, i);

		certData[controlWordIndex].cwIndex = strtoul(csvInput,NULL,16);

		printf("index = %x\n", certData[controlWordIndex].cwIndex);

		i++;
		base = i;

		/* Calculate the string length of the number of commands. */
		for(;;i++)
			if(strncmp(&(pString[i]), ",", 1) ==0)
				break;

		copyString(csvInput, pString+base, i-base);

		certData[controlWordIndex].numOfCommands = strtoul(csvInput,NULL,16);
		printf("cmds = %x\n", certData[controlWordIndex].numOfCommands);
		
		i++;
		base = i;

		for(j=0;j<(certData[controlWordIndex].numOfCommands);j++)
		{
			/* Copy the input data. */
			for(i=0;i<8;i++)
			{
				copyString(csvInput, &(pString[base+(i*8)]), 8);
				temp = strtoul (csvInput,NULL,16);

				certData[controlWordIndex].cert_cmd[j].inputData[(i*4)+3] = (unsigned char)((temp>>24) & 0xFF);
				certData[controlWordIndex].cert_cmd[j].inputData[(i*4)+2] = (unsigned char)((temp>>16) & 0xFF);
				certData[controlWordIndex].cert_cmd[j].inputData[(i*4)+1] = (unsigned char)((temp>>8) & 0xFF);
				certData[controlWordIndex].cert_cmd[j].inputData[i*4] = (unsigned char)(temp & 0xFF);
			}


			printf("%08x\n%08x\n%08x\n%08x\n%08x\n%08x\n%08x\n%08x\n", 
			       (uint32_t)certData[controlWordIndex].cert_cmd[j].inputData[0], 
			       (uint32_t)certData[controlWordIndex].cert_cmd[j].inputData[4], 
			       (uint32_t)certData[controlWordIndex].cert_cmd[j].inputData[8], 
			       (uint32_t)certData[controlWordIndex].cert_cmd[j].inputData[12], 
			       (uint32_t)certData[controlWordIndex].cert_cmd[j].inputData[16], 
			       (uint32_t)certData[controlWordIndex].cert_cmd[j].inputData[20], 
			       (uint32_t)certData[controlWordIndex].cert_cmd[j].inputData[24], 
			       (uint32_t)certData[controlWordIndex].cert_cmd[j].inputData[28]);

			/* Copy the command op code. */
			copyString(csvInput, &(pString[base+(i*8)+1]), 8);
			temp = strtoul (csvInput,NULL,16);

			certData[controlWordIndex].cert_cmd[j].opcodes[3] = (unsigned char)((temp>>24) & 0xFF);
			certData[controlWordIndex].cert_cmd[j].opcodes[2] = (unsigned char)((temp>>16) & 0xFF);
			certData[controlWordIndex].cert_cmd[j].opcodes[1] = (unsigned char)((temp>>8) & 0xFF);
			certData[controlWordIndex].cert_cmd[j].opcodes[0] = (unsigned char)(temp & 0xFF);

			printf("%08x\n", *(uint32_t*)certData[controlWordIndex].cert_cmd[j].opcodes);

			base+=74;

			certData[controlWordIndex].cert_cmd[j].timeout = CERT_TIMEOUT_DEFAULT;
		}
		controlWordIndex++;
	}
	
}

void R2R_byteSwap(
	unsigned char *word
	)
{
	unsigned char temp;
    
	temp = word[0];
	word[0] = word[3];
	word[3] = temp;
    
	temp = word[1];
	word[1] = word[2];
	word[2] = temp;
    
	return;
}


TCsdStatus R2R_CryptoOperation(
	NEXUS_KeySlotHandle keyHandle,
  	const TCsdUnsignedInt8* pxInputData, 
	TCsdUnsignedInt8* pxOutputData, 
	TCsdSize xDataSize,
	NEXUS_DmaDataFormat dmaFormat
	)
{
	/* NEXUS_DmaHandle dma; */
	NEXUS_DmaJobSettings jobSettings;
	NEXUS_DmaJobHandle dmaJob;
	NEXUS_DmaJobBlockSettings blockSettings;
	NEXUS_DmaJobStatus jobStatus;
	NEXUS_Error errCode;
	unsigned char *input_data;
	unsigned char *output_data;
#if(CFG_BIG_ENDIAN==1)
	TCsdSize i;
#endif

	/* Need to allocate memory for M2M input and output buffers. */
	NEXUS_Memory_Allocate(sizeof(unsigned char) * xDataSize, NULL, (void *)&input_data);
	NEXUS_Memory_Allocate(sizeof(unsigned char) * xDataSize, NULL, (void *)&output_data);

	/* Copy the input data to M2M input buffer. */
	BKNI_Memcpy((void *)input_data, (void *)pxInputData, xDataSize );

#if(CFG_BIG_ENDIAN==1)
	for(i=0;i<xDataSize;i+=4)
		R2R_byteSwap(&(input_data[i]));
#endif

	/* Open DMA handle */
	/* dma = NEXUS_Dma_Open(0, NULL); */

	NEXUS_DmaJob_GetDefaultSettings(&jobSettings);
	jobSettings.numBlocks = 1;
	jobSettings.keySlot = keyHandle;
	jobSettings.dataFormat = dmaFormat;
	jobSettings.completionCallback.callback = NULL;

	dmaJob = NEXUS_DmaJob_Create(dma, &jobSettings);

	NEXUS_DmaJob_GetDefaultBlockSettings(&blockSettings);
	blockSettings.pSrcAddr = input_data;  
	blockSettings.pDestAddr = output_data; 
	blockSettings.blockSize = xDataSize;
	blockSettings.resetCrypto = true;
	blockSettings.scatterGatherCryptoStart = true;
	blockSettings.scatterGatherCryptoEnd = true;
	blockSettings.cached = true;

	errCode = NEXUS_DmaJob_ProcessBlocks(dmaJob, &blockSettings, 1);

	/* Wait for M2M Encryption to complete. */
	for(;;)
	{
		NEXUS_DmaJob_GetStatus(dmaJob, &jobStatus);

		if(jobStatus.currentState == NEXUS_DmaJobState_eComplete )
		{
			break;
		}
		BKNI_Delay(1);
	}

#if(CFG_BIG_ENDIAN==1)
	for(i=0;i<xDataSize;i+=4)
		R2R_byteSwap(&(output_data[i]));
#endif

	/* Copy the xCSCData to M2M input buffer. */
	BKNI_Memcpy((void *)pxOutputData, (void *)output_data, xDataSize );

	/* Done.  Close DMA. */
	NEXUS_DmaJob_Destroy(dmaJob);
	
	/* NEXUS_Dma_Close(dma); */

	NEXUS_Memory_Free(output_data);
	NEXUS_Memory_Free(input_data);

    	return CSD_NO_ERROR;
	

}


void cert_usage(void) {
	fprintf(stderr,
		"To get mode information of a command type \"cert -COMMAND -h\n"
		"-bridge = The bridge functionality is used to exchange commands to\n"
		"	the CERT IP.\n"
		"-r2r = RAM2RAM functionality\n"
		"-dsc = Transport Stream Functionality\n"
		"-scr = Stream Crypto Functionality\n"
		"-reset = Perfrom the hard reset of the CERT IP\n"
		"-v = Versoin of the CERT App.\n"
		);
}


void bridge_usage(char *argv[]) {
	fprintf(stderr,
		"CERT Bridge v %x\n"
		"Usage:\n"
		"  %s -brdige -f [-i filein] [-o fileout] [-l]\n"
		"or\n"
		"  %s bridge -n [-p bindport] [-b bindaddr] [-l]\n"
		"\n"
		"where\n", CERT_BRIDGE_VERSION, argv[0], argv[0]
		);
	fprintf(stderr,
		"   -f   use file mode\n"
		"   -n   use network mode (default)\n"
		" Global options:\n"
		"   -l   enable logging.\n"
		" File mode options:\n"
		"   -i   set input filename. Defaults reading from stdin.\n"
		"   -o   set output filename. Defaults writing to stdout.\n"
		" Network mode options:\n"
		"   -p   set bind port. root permission are required for ports < 1024. (default 10101)\n"
		"   -b   set bind address. The address should be in the standard numbers-and-dots notation.\n"			
		);
}

void r2r_usage(void) {
	fprintf(stderr,
		"-r2r = This defines the RAM2RAM functionality.\n"
		"	-enc|dec = This defines the operation performed for RAM2RAM\n"
		"	functionality.  If -enc, is an encryption operation and -dec is a\n"
		"	decryption operation.\n\n"
		"	-emi EMI = Defines what is the format (Algorithm, mode, IV) of\n"
		"	the input stream.\n\n"
		);
	fprintf(stderr,
		"	-iv Value64|Value128 = IV value.  Optional Parameter.\n\n"
		"	-if InFile|-ic Value64|Value128 = The value to be used as an input\n"
		"	to RAM2RAM is taken form file (-if) or from command line (-ic).\n\n"
		"	-of OutFile|-oc = The path and name of the output file (-of) or\n"
		"	computed value printed on stdout (-oc).\n\n"
		"	-numbruns # = How many times the input data must be sourced.\n"
		);
	
	return;
}

void reset_usage(void) {
	fprintf(stderr,
		"-reset = This defines the reset functionality.\n"
		"	-t = This defines the minimum amount of time in seconds the command\n"
		"	should wait before it executes the reset operation.\n"
		);
	
	return;
}


void dsc_usage(void) {
	fprintf(stderr,
		"-dsc = This defines the Descrambling functionality.\n"
		"	-emi EMI = Defines what is the format (Algorithm, mode, and IV) of\n"
		"	the input stream.\n\n"
		"	-if InFile|-ife = Defines what is the input stream.  If -if, then the input\n"
		"	comes from the file (InFile is a path to an MPEG TS file).  If -ife, then\n"
		"	the input stream comes from the STB frontend.\n\n"
		);
	fprintf(stderr,
			
		"	-of OutFile = If this option is present, the descrambled output is sent\n"
		"	into OutFile as well as to the video output.  If not specified, the descrambled\n"
		"	stream only goes to the video output.\n\n"
		"	-videoPid VPID = Sets video PID to be processed.\n\n"
		"	-audioPid APID = Sets audio PID to be processed.\n\n"
		);
	fprintf(stderr,
		"	-pcrPid PPID = Sets PCR PID to be processed.\n\n"
		"	-ecmPid EPID = Sets ECM PID to be processed.  Optional if the stream is\n"
		"	not scrambled or if there are not ECMs in the stream.\n\n"
		"	-pecm TRUE|FALSE = Whether to process the ECMs or not.  If set to FALSE\n"
		"	do not perform the ECM processing, instead the key is only set once at\n"
		"	startup with a certUseDecryptionKey.\n\n"
		);
	fprintf(stderr,
			
		"	-useParity ODD|EVEN = Only used when -pecm is set to FALSE; indicates\n"
		"	what key parity should be used on the certUseDecryptionKey call.\n\n"
		"	-ecm2cert ecm2certcmds.csv = Indicates the path and filename of the \n"
		"	.CSV files which contains the required CERT commands for each\n"
		"	control_word_?_index to output the corresponding CS from the CERT\n"
		);
	
	return;
}

void scr_usage(void) {
	fprintf(stderr,
		"-scr = This defines the Stream Crypto functionality.\n"
		"	-enc|-dec = This defines operation performed for Stream Crypto\n"
		"	functionality.  If -enc is an encryption (scrambling) operation and\n"
		"	-dec is a decryption (descrambling) operation.\n\n"
		);
	fprintf(stderr,
			
		"	-emi EMI = Defines what is the format (Algorithm, mode, and IV)\n"
		"	of the input stream.\n\n"
		"	-iv IV = Initialization Vector value.  Optional according to EMI.\n\n"
		"	-videoPid VPID = Sets video PID to be processed.\n\n"
		"	-audioPid APID = Sets audio PID to be processed.\n\n"
		);
	fprintf(stderr,
		"	-pcrPid PPID = Sets PCR PID to be processed.\n\n"
		"	-if InFile|-ife = Defines what is the input stream.  If -if then the \n"
		"	input comes from the file (InFile is a path to an MPEG TS file). If\n"
		"	-ife then the input stream comes from the STB front end.\n\n"
		);
	fprintf(stderr,
			
		"	-of OutFile = The path and name of the output file.\n\n"
		"	-useParity ODD|EVEN|UNDEFINED = indicates what key parity\n"
		"	should be used on the certUseDecryptionKey or certUseEncryptionKey\n"
		"	calls.  UNDEFINED is used with EMIs that define en/decryption at raw\n"
		"	level and where parity has no meaning.\n\n"
		);
	
	return;
}


int cert_bridge(
	int filemode,
	int netmode,
	char *infile,
	char *outfile,
	int bind_port,
	int log_enabled
	)
{
	unsigned char inBuf[INBUFSIZE]; 		/* input buffer to store the data from file or socket. */
	unsigned char outBuf[OUTBUFSIZE]; 		/* output buffer to store the data to a file or write to the socket. */
	size_t nb; 							/* number of bytes to be processed in inBuf */
	size_t nbseq; 						/* number of bytes of current tlv sequence */
	int ret;
	struct in_addr bind_addr;
	struct sockaddr_in sSockAddrIn;
	int sClientSock;
	/* accept connection */
	struct sockaddr sClientAddr;
	socklen_t iLength;

	bind_addr.s_addr = INADDR_ANY;	
	g_numchld = 0;

	if( (filemode ^ netmode) == 0) 
	{
		fprintf(stderr, "Please select either file mode (-f) or network mode (-n).\n");
		return 1;
	}

	openlog(NULL, LOG_PID | LOG_CONS, LOG_LOCAL0); /* tell the logger to log PID */
	if(log_enabled) 
		setlogmask(LOG_UPTO(LOG_DEBUG)); /* set log level to debug */
	else
		setlogmask(LOG_UPTO(LOG_INFO)); /* set log level to info */
	
	syslog(LOG_INFO, "cert_bridge v%d.%d.%04d starting up.", (CERT_BRIDGE_VERSION >>24)&0xFF,
	       (CERT_BRIDGE_VERSION >>16)&0xFF,
	       (CERT_BRIDGE_VERSION >>0)&0xFFFF);

	if(filemode) 
	{
		syslog(LOG_DEBUG, "Running in file mode.");  	


		if(infile == NULL)
			inputFile = stdin;
		else
		{
			if((inputFile = fopen(infile, "rb")) == NULL)
				return -1;			

			syslog(LOG_DEBUG, "infile set to: %s", infile);			
			
		}

		if(outfile == NULL)
			outputFile = stdout;
		else
		{
			if((outputFile = fopen(outfile, "w+b")) == NULL)
				return -1;			

			syslog(LOG_DEBUG, "output file set to: %s", outfile);			
			
		}
			
		/* This code will request a new socket.  Once it has the socket, it will set the port number and address to listen on.
		   These parameters are then binded with the socket and now the server listens over the socket to wiat for incoming
		   data. */
	}
	else 
	{ /* network mode (default) */
		syslog(LOG_DEBUG, "Running in network mode.");
		
		/* Server requests to create a new socket to communicate over. */ 
		if( (gServerSock = socket(AF_INET, SOCK_STREAM, 0)) == -1 ) 
		{
			perror("Unable to create server socket");
			return -2;
		}
				
		/* setup connexion option */
		/* Clear the structure defining the socket. */
		bzero(&sSockAddrIn, sizeof(sSockAddrIn));

		sSockAddrIn.sin_family = AF_INET;
		sSockAddrIn.sin_port = htons(bind_port);
		sSockAddrIn.sin_addr.s_addr = INADDR_ANY;

		/* bind the allocated socket to a port number and address. */
		if (bind(gServerSock, (struct sockaddr *)&sSockAddrIn, sizeof(sSockAddrIn)) == -1) 
		{
			perror("Unable to bind socket...");
			return -3;
		}
		
		/* listen on socket */
		if (listen(gServerSock, 5) == -1) 
		{
			perror("Unable to listen on socket.");
			return -4;
		}
		
		syslog(LOG_DEBUG, "Socket listening on port %d.", bind_port);
		
	}

net_accept:
	if(netmode) 
	{

		iLength = sizeof(sClientAddr);
#if 0
		while(1)
		{
#endif		
			syslog(LOG_INFO, "Accepting connections on port %d...\n", bind_port );
	
			/* Causes the server to block until a client connects to the server. */
			sClientSock = accept(gServerSock, &sClientAddr, &iLength);
			if( sClientSock == -1) 
			{
				perror("Error accepting client");
				return -6;
			}

			/* Log the address of the client that connected. */
			syslog(LOG_INFO, "Accepted connection from %s\n", 
			       inet_ntoa(((struct sockaddr_in *)&sClientAddr)->sin_addr ));
#if 0
			pid = fork();
			if(pid < 0) 
			{
				perror("Fork error");
				return -7;
			} 
			else if (pid == 0) 
			{ /* child */
				g_numchld=0; /* reset chip */
				break;
			} 
			else 
			{ /* parent */
				g_numchld++;
				syslog(LOG_DEBUG, "Spawned child process with pid: %d\n", pid);
				for(i=0; i < g_numchld; i++) 
				{ /* clean up after all potential children death.  */
					if( waitpid(-1, NULL, WNOHANG) > 0 ) 
					{
						g_numchld--;
					}
				}
			}
		}
#endif			
		
		g_infd = sClientSock;
		g_outfd = sClientSock;		

		nb = 0;
		
		while(1)
		{
			/* read in data from socket or file */
			ret = read(g_infd, inBuf+nb, INBUFSIZE-nb);
			if( ret == 0 ) 
			{ /* EOD */
				break;
			} 
			else if ( ret < 0 ) 
			{
				syslog(LOG_INFO, "Read on fd failed: %s\n", strerror(errno));
				break;
			}
			
			nb += ret;

			/* printf("The number of bytes read from the socket = 0x%x\n", nb); */
			
			/*check if a valid tlv sequence (including end tlv) is present */
			while( (ret = tlvChkFirstSeq(inBuf, nb, NULL)) > 0) 
			{
				nbseq = (size_t)ret;

				/* process sequence here */
				syslog(LOG_DEBUG, "Valid TLV sequence read.");
				display_buffer(inBuf, nbseq);
				ret = procBuf(inBuf, nbseq, outBuf, OUTBUFSIZE);
				display_buffer(outBuf, ret);
				ret = write(g_outfd, outBuf, ret);
				if(ret < 0) 
				{
					perror("write");
					break;
				}
				/* end process sequence */
				
				/* move the remaing of the buffer at the beginning.  */
				memmove(inBuf, inBuf+nbseq, nb-nbseq); /* overlapping regions! */
				nb -= nbseq;
			} 
		
			if ( nb >= INBUFSIZE) 
			{ /* in buffer full  */
				syslog(LOG_WARNING, "Input buffer is full and no valid TLV sequence was read.\nFlushing...\n");
				nb = 0;
			}
			
		}

		syslog(LOG_INFO, "EOF. Exiting.\n");

		close(g_infd);
		close(g_outfd);

	}
	else
	{
		nb = 0;
		
		while(1)
		{
			/* read in data from socket or file */
			ret = fread(inBuf+nb, sizeof(unsigned char), INBUFSIZE-nb, inputFile);
			if( ret == 0 ) 
			{ /* EOD */
				break;
			} 
			else if ( ret < 0 ) 
			{
				syslog(LOG_INFO, "Read on fd failed: %s\n", strerror(errno));
				break;
			}
			
			nb += ret;

			printf("The number of bytes read from the socket = 0x%x\n", nb);
			
			/*check if a valid tlv sequence (including end tlv) is present */
			while( (ret = tlvChkFirstSeq(inBuf, nb, NULL)) > 0) 
			{
				nbseq = (size_t)ret;

				/* process sequence here */
				syslog(LOG_DEBUG, "Valid TLV sequence read.");
				display_buffer(inBuf, nbseq);
				ret = procBuf(inBuf, nbseq, outBuf, OUTBUFSIZE);
				display_buffer(outBuf, ret);
				ret = fwrite(outBuf, sizeof (unsigned char), ret, outputFile);
				if(ret < 0) 
				{
					perror("write");
					break;
				}
				/* end process sequence */
				
				/* move the remaing of the buffer at the beginning.  */
				memmove(inBuf, inBuf+nbseq, nb-nbseq); /* overlapping regions! */
				nb -= nbseq;
			} 
		
			if ( nb >= INBUFSIZE) 
			{ /* in buffer full  */
				syslog(LOG_WARNING, "Input buffer is full and no valid TLV sequence was read.\nFlushing...\n");
				nb = 0;
			}
			
		}

		syslog(LOG_INFO, "EOF. Exiting.\n");

		fclose(inputFile);
		fclose(outputFile);		
	}
	
	if(netmode) {
		goto net_accept;
	}

	return 0;

}

void cert_reset(
	uint32_t time
	)
{
	uint32_t reg_data;

	/* Enable the SW master reset. */
	NEXUS_Platform_ReadRegister( BCHP_SUN_TOP_CTRL_RESET_SOURCE_ENABLE, &reg_data );
	reg_data |= BCHP_SUN_TOP_CTRL_RESET_SOURCE_ENABLE_sw_master_reset_enable_MASK;
	NEXUS_Platform_WriteRegister( BCHP_SUN_TOP_CTRL_RESET_SOURCE_ENABLE, reg_data );

	/* Wait for "time" seconds before the reset is applied. */
	BKNI_Sleep(time*1000);

	/* Apply a SW master reset.  The whole chip is reset. */
	NEXUS_Platform_ReadRegister( BCHP_SUN_TOP_CTRL_SW_MASTER_RESET, &reg_data );
	reg_data |= BCHP_SUN_TOP_CTRL_SW_MASTER_RESET_chip_master_reset_MASK;
	NEXUS_Platform_WriteRegister( BCHP_SUN_TOP_CTRL_SW_MASTER_RESET, reg_data );
	
	return;
}

int cert_r2r(
	bool perform_encryption,
	uint32_t emi,
	uint32_t iv_value0,
	uint32_t iv_value1,
	uint32_t iv_value2,
	uint32_t iv_value3,
	bool input_from_file,
	char* infile,
	uint32_t input_data0,
	uint32_t input_data1,
	uint32_t input_data2,
	uint32_t input_data3,
	char *outfile,
	bool output_console,
	uint32_t number_of_runs
	)
{

	uint32_t i;
	TCsdUnsignedInt8 *dataIn;
	TCsdUnsignedInt8 *dataOut;
	TCsdUnsignedInt8* dataIv;
	TCertKeyPathHandle *pxKeyPathHandle;
	TCsdR2RKeyPathHandle* pxR2rHandle;
	TCsdInitParameters csdInit;
	TCsdTerminateParameters csdTerminateHandle; 
	TCsdStatus status;
	FILE* tempFile;
	uint32_t numread;
	uint32_t ivSize;

	/* Initialize the CSD driver. */	
	memset (&csdInit, 0, sizeof(TCsdInitParameters));

	status=csdInitialize(&csdInit);
	
	if( (status!=CSD_NO_ERROR) && (status !=CSD_ERROR_DRIVER_ALREADY_INITIALIZED) )
	{
		printf("Error: cannot initialize CSD driver due to error %d!\n", status); 
		return -1;
	}

	/* Setup the RAM2RAM Handle. */	
	NEXUS_Memory_Allocate(sizeof(TCertKeyPathHandle), NULL, (void *)&pxKeyPathHandle);
	NEXUS_Memory_Allocate(sizeof(TCertKeyPathHandle), NULL, (void *)&pxR2rHandle);

	pxKeyPathHandle->r2rHandle = pxR2rHandle;
	pxKeyPathHandle->r2rHandle->initHandle = &csdInit;

	/* Allocate memory for the input and output buffers. */
	NEXUS_Memory_Allocate(sizeof(unsigned char)*256, NULL, (void *)&dataIn);
	NEXUS_Memory_Allocate(sizeof(unsigned char)*256, NULL, (void *)&dataOut);
	NEXUS_Memory_Allocate(sizeof(unsigned char)*16, NULL, (void *)&dataIv);

	ivSize = 0;

	if((emi == CSD_Algorithm_M2M_AES_128_CBC_CLEAR) ||
	   (emi == CSD_Algorithm_M2M_AES_128_CTR_CLEAR))
	{
		*(uint32_t*)&dataIv[0]  = htonl(iv_value0);
		*(uint32_t*)&dataIv[4]  = htonl(iv_value1);
		*(uint32_t*)&dataIv[8]  = htonl(iv_value2);
		*(uint32_t*)&dataIv[12] = htonl(iv_value3);

		ivSize = 16;
	}
	else if(emi == CSD_Algorithm_M2M_TDES_ABA_CBC_CLEAR)
	{
		*(uint32_t*)&dataIv[0] = htonl(iv_value2);
		*(uint32_t*)&dataIv[4] = htonl(iv_value3);

		ivSize = 8;
	}

	/* Copy the right amount of data (8 vs 16 bytes) depending on the EMI. */
	if((emi == CSD_Algorithm_M2M_TDES_ABA_ECB_CLEAR) ||
	   (emi == CSD_Algorithm_M2M_TDES_ABA_CBC_CLEAR) )
	{
		/* Get the input data either from the input file or from the console input. */		
		if(input_from_file == false)
		{
			*(uint32_t*)&dataIn[0] = input_data2;
			*(uint32_t*)&dataIn[4] = input_data3;

			numread = 8;
		}
		else
		{
			if( (tempFile  = fopen( infile, "r" )) == NULL )
			{
				perror(infile);
				return -2;
			}
			numread = fread( dataIn, sizeof( unsigned char ), 256, tempFile );

			fclose(tempFile);
			
			if((numread % 8) != 0)
				return -3;
		}
			
	}
	else
	{	
		/* Get the input data either from the input file or from the console input. */		
		if(input_from_file == false)
		{
			*(uint32_t*)&dataIn[0] = input_data0;
			*(uint32_t*)&dataIn[4] = input_data1;
			*(uint32_t*)&dataIn[8] = input_data2;
			*(uint32_t*)&dataIn[12] = input_data3;	
			
			numread = 16;
			
		}
		else
		{		
			if( (tempFile  = fopen( infile, "r" )) == NULL )
			{
				perror(infile);
				return -4;
			}

			numread = fread( dataIn, sizeof( unsigned char ), 256, tempFile );

			fclose(tempFile);
			
			if((numread % 16) !=0)
				return -5;
		}
			
	
	}
	pxKeyPathHandle->r2rHandle->keyHandle = NULL;
	pxKeyPathHandle->dest = NEXUS_SecurityAlgorithmConfigDestination_eMem2Mem;
	pxKeyPathHandle->keySlotEngine = NEXUS_SecurityEngine_eM2m;
	
	if(perform_encryption)
	{
		certUseEncryptionKey(
			emi,
			pxKeyPathHandle,
			CERT_KEY_PARITY_UNDEFINED
			);
	}
	else
	{
		certUseDecryptionKey(
			emi,
			pxKeyPathHandle,
			CERT_KEY_PARITY_UNDEFINED
			);
	}
	
	for(i=0;i<number_of_runs; i++)
	{
		if(perform_encryption)
		{
			csdEncryptData(
				emi,
				pxKeyPathHandle->r2rHandle,
				dataIv,
				ivSize,
				dataIn,
				dataOut,
				numread
	        		);			
		}
		else
		{
		
			csdDecryptData(
				emi,
				pxKeyPathHandle->r2rHandle,
				dataIv,
				ivSize,
				dataIn,
				dataOut,
				numread
	        		);
		}		
		BKNI_Memcpy(dataIn, dataOut, sizeof(TCsdUnsignedInt8)*numread);
	}

	if(output_console == false)
	{
		if( (tempFile  = fopen( outfile, "w+" )) == NULL )
		{
			perror(outfile);
			return -6;
		}

		fwrite( dataOut, sizeof( unsigned char ), numread, tempFile );

		fclose(tempFile);
		
	}
	else
	{
		printf("Output = ");

		for(i=0;i<numread;i++)
			printf("%02x", dataOut[i]);
	}	

	NEXUS_Memory_Free(dataOut);
	NEXUS_Memory_Free(dataIn); 
	NEXUS_Memory_Free(dataIv);
	NEXUS_Memory_Free(pxR2rHandle);
	NEXUS_Memory_Free(pxKeyPathHandle);

	csdTerminateHandle.initParam = &csdInit; 
	csdTerminateHandle.keyHandleToRelease= NULL; 
	csdTerminate(&csdTerminateHandle);
	
	return 0;	
}

int cert_dsc(
	uint32_t emi,
	char *infile,
	char *outfile,
	bool front_end_input,
	bool output_to_file,
	uint32_t videoPid,
	uint32_t audioPid,
	uint32_t pcrPid,
	uint32_t ecmPid,
	bool process_ecms,
	bool use_odd_parity,
	char* csv_file
	)
{
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

	if(front_end_input==false)
	{
		if( (pInFile  = fopen( infile, "r" )) == NULL )
		{
			printf( "The file %s was not opened\n", infile);
			printf( "ERROR: Could not create temporary storage.\n");
			return -3;
		}
	}

	if(output_to_file == true)
	{
        	if( (pOutFile  = fopen( outfile, "w+" )) == NULL )
        	{
        		printf( "The file %s was not opened\n", outfile);
        		printf( "ERROR: Could not create temporary storage.\n");
        		return -4;
        	}    
	}
	if(process_ecms == true) {
		if( (pCsvFile = fopen( csv_file, "r" )) == NULL ) {
			perror(csv_file);
			return -5;
		}
	}

	

	keyPathHandle.dscVideoHandle = &dscVideoHandle;
	keyPathHandle.dscAudioHandle = &dscAudioHandle;

	keyPathHandle.dscVideoHandle->initHandle = &csdInitVideo;
	keyPathHandle.dscAudioHandle->initHandle = &csdInitAudio;
    


	pCaPvrConfig->bIsDsc = true;
	pCaPvrConfig->bEncryption = false;
	pCaPvrConfig->emi = emi;
	pCaPvrConfig->audioPID = audioPid;
	pCaPvrConfig->videoPID = videoPid;
	pCaPvrConfig->pcrPID = pcrPid;
	pCaPvrConfig->bEcmEnabled = process_ecms;
	pCaPvrConfig->ecmPID = ecmPid;	
	pCaPvrConfig->pDscKeyHandle= &keyPathHandle;	   
	pCaPvrConfig->pInFile = pInFile;
	pCaPvrConfig->pOutFile = pOutFile;
	pCaPvrConfig->pVector = pVector;
	pCaPvrConfig->sizeIV = 0;
	pCaPvrConfig->use_odd_parity=use_odd_parity;



	nocs3_status = csd_harness_test_ca_pvr (pCaPvrConfig);
	if ( nocs3_status != NOCS3_HARNESS_TEST_STATUS_CODE_SUCCESS )
	{
		/* Parser error code */
		printf ("SCR test returns error code of %08x\n", nocs3_status);
	}

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

	return 0;
}

int cert_scr(
	bool perform_encryption,
	uint32_t emi,	
	char* infile,
	bool front_end_input,
	char* outfile,
	uint32_t iv_value0,
	uint32_t iv_value1,
	uint32_t iv_value2,
	uint32_t iv_value3,
	uint32_t videoPid,
	uint32_t audioPid,
	uint32_t pcrPid,
	TCertKeyParity scr_parity
	)
{
	NOCS3_HARNESS_TEST_STATUS_CODE  nocs3_status;
 	TCsdInitParameters csdInit;
	TCsdStatus status;
	TCsdTerminateParameters csdTerminateHandle; 
	TCertKeyPathHandle KeyPathHandle;
	TCsdScrKeyPathHandle scrHandle;
	CSD_Harness_Test_CA_PVR_Configuration caPvrConfig, *pCaPvrConfig = &caPvrConfig;
	unsigned int ivSize;
	FILE* pInFile=NULL;
	FILE* pOutFile=NULL;
	TCsdUnsignedInt8* dataIv;
	NEXUS_SecurityClearKey ivKey;

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

	NEXUS_Memory_Allocate(sizeof(TCsdUnsignedInt8)*16, NULL, (void *)&dataIv);
    	memset (dataIv, 0, sizeof(unsigned char)*16);

	KeyPathHandle.dest = NEXUS_SecurityAlgorithmConfigDestination_eMem2Mem;
	KeyPathHandle.keySlotEngine = NEXUS_SecurityEngine_eM2m;
	KeyPathHandle.bLoadScr = true;
	if(perform_encryption)
	{
		certUseEncryptionKey(
			emi,
			&KeyPathHandle,
			scr_parity
			);
	}
	else
	{	
		certUseDecryptionKey(
			emi,
			&KeyPathHandle,
			scr_parity
			);
		
	}


	/* load IV key if needed */
	if(
		(emi == CSD_Algorithm_AES_128_CBC_IV_0_SCTE52) ||
		(emi == CSD_Algorithm_AES_128_CBC_CLEAR) ||
		(emi == CSD_Algorithm_RAW_0x4022) ||
		(emi == CSD_Algorithm_RAW_0x4023) ||
		(emi == CSD_Algorithm_RAW_0x4024) ||
		(emi == CSD_Algorithm_RAW_AES_128_CBC_CLEAR) ||
		(emi == CSD_Algorithm_RAW_AES_128_CBC_IV_0_CLEAR) ||
		(emi == CSD_Algorithm_M2M_AES_128_CBC_CLEAR) ||
		(emi == CSD_Algorithm_M2M_AES_128_CTR_CLEAR) ||

		(emi == CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52) ||
		(emi == CSD_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR) ||
		(emi == CSD_Algorithm_M2M_TDES_ABA_CBC_CLEAR)
		
		)
	{
	
        	ivSize = 0;

        	if( (emi == CSD_Algorithm_AES_128_CBC_IV_0_SCTE52) ||
		    (emi == CSD_Algorithm_AES_128_CBC_CLEAR) ||
		    (emi == CSD_Algorithm_RAW_0x4022) ||
		    (emi == CSD_Algorithm_RAW_0x4023) ||
		    (emi == CSD_Algorithm_RAW_0x4024) ||
		    (emi == CSD_Algorithm_RAW_AES_128_CBC_CLEAR) ||
		    (emi == CSD_Algorithm_RAW_AES_128_CBC_IV_0_CLEAR) ||
		    (emi == CSD_Algorithm_M2M_AES_128_CBC_CLEAR) ||
		    (emi == CSD_Algorithm_M2M_AES_128_CTR_CLEAR) )

        	{
			/*R2R_byteSwap(&iv_value0);*/
        		*(uint32_t*)&dataIv[0]  = htonl(iv_value0);
        		*(uint32_t*)&dataIv[4]  = htonl(iv_value1);
        		*(uint32_t*)&dataIv[8]  = htonl(iv_value2);
        		*(uint32_t*)&dataIv[12] = htonl(iv_value3);

        		ivSize = 16;
        	}
        	else if((emi == CSD_Algorithm_M2M_TDES_ABA_CBC_CLEAR) ||
			(emi == CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52) ||
			(emi == CSD_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR))
        	{
        		*(uint32_t*)&dataIv[0] = htonl(iv_value2);
			*(uint32_t*)&dataIv[4] = htonl(iv_value3);

        		ivSize = 8;
        	}   

		BKNI_Memset((void *)&ivKey, 0, sizeof(ivKey));
		NEXUS_Security_GetDefaultClearKey(&ivKey);
		ivKey.keyIVType 	= NEXUS_SecurityKeyIVType_eIV;
		ivKey.keyEntryType = NEXUS_SecurityKeyType_eIv;
   		ivKey.keySize = 16; /*xInitializationVectorSize;*/
		
		if(ivSize == 8){
			BKNI_Memcpy(&(ivKey.keyData[8]), dataIv, ivSize);
		}
		else
			BKNI_Memcpy(ivKey.keyData, dataIv, ivSize);
    
		/* Load IV. */
    		if (NEXUS_Security_LoadClearKey(KeyPathHandle.scrHandle->keyHandle , &ivKey) != 0) 
		{
        		printf("\nLoad IV failed \n");
        		return -2;
    		}
	}

	if(front_end_input==false)
	{
		if( (pInFile  = fopen( infile, "r" )) == NULL )
		{
			perror(infile);
			return -2;
		}
	}

	if( (pOutFile  = fopen( outfile, "w+" )) == NULL )
	{
		perror(outfile);
		return -3;
	}

	
	if(perform_encryption == true)
		pCaPvrConfig->bEncryption = true;
	else
		pCaPvrConfig->bEncryption = false;
	
	pCaPvrConfig->bIsDsc = false;
	pCaPvrConfig->emi = emi;
	pCaPvrConfig->audioPID = audioPid;
	pCaPvrConfig->videoPID = videoPid;
	pCaPvrConfig->pcrPID = pcrPid;
	pCaPvrConfig->bEcmEnabled = false;
	pCaPvrConfig->ecmPID = 0;
	pCaPvrConfig->pScrKeyHandle = KeyPathHandle.scrHandle;
	pCaPvrConfig->pDscVideoKeyHandle = NULL;
	pCaPvrConfig->pDscAudioKeyHandle = NULL;
	pCaPvrConfig->pInFile = pInFile;
	pCaPvrConfig->pOutFile = pOutFile;
	pCaPvrConfig->pVector = NULL;

	nocs3_status = csd_harness_test_ca_pvr (pCaPvrConfig);
	if ( nocs3_status != NOCS3_HARNESS_TEST_STATUS_CODE_SUCCESS )
	{
		/* Parser error code */
		printf ("SCR test returns error code of %08x\n", nocs3_status);
	}

	NEXUS_Memory_Free(dataIv);


	/* Clean up */
	csdTerminateHandle.initParam = &csdInit; 
	csdTerminateHandle.keyHandleToRelease = NULL; 
	csdTerminate(&csdTerminateHandle);

	if (pInFile)
	{
		fclose (pInFile);
	}
	if (pOutFile)
	{
		fclose (pOutFile);
	}
	
	return 0;
}

int cert_convert_to_64_128(
	char* iv_str, 
	uint32_t* iv_value0,
	uint32_t* iv_value1,
	uint32_t* iv_value2,
	uint32_t* iv_value3
	)
{
	char iv_str0[9];
	char iv_str1[9];
	char iv_str2[9];
	char iv_str3[9];

	strcpy(iv_str0, "00000000");
	strcpy(iv_str1, "00000000");
	strcpy(iv_str2, "00000000");
	strcpy(iv_str3, "00000000");
	
	if(((strncmp("0x", iv_str, 2) == 0) || 
	    (strncmp("0X", iv_str, 2) == 0)) &&
	   (strlen(iv_str) == 34))
	{
		strncpy(iv_str0, &(iv_str[2]), 8);
		strncpy(iv_str1, &(iv_str[10]), 8);
		strncpy(iv_str2, &(iv_str[18]), 8);
		strncpy(iv_str3, &(iv_str[26]), 8);
	}
	else if (strlen(iv_str) == 32)
	{
		strncpy(iv_str0, &(iv_str[0]), 8);
		strncpy(iv_str1, &(iv_str[8]), 8);
		strncpy(iv_str2, &(iv_str[16]), 8);
		strncpy(iv_str3, &(iv_str[24]), 8);
	}
	else if(((strncmp("0x", iv_str, 2) == 0) || 
		 (strncmp("0X", iv_str, 2) == 0)) &&
		(strlen(iv_str) == 18))
	{
		strncpy(iv_str2, &(iv_str[2]), 8);
		strncpy(iv_str3, &(iv_str[10]), 8);
	}
	else if (strlen(iv_str) == 16)
	{
		strncpy(iv_str2, &(iv_str[0]), 8);
		strncpy(iv_str3, &(iv_str[8]), 8);
	}
	
	else
	{
		printf("ERROR: IV is invalid length.  Pass in a 128-bit value.\n");
		return 1;
	}
	
	*iv_value0= strtoul (iv_str0,NULL,16);
	*iv_value1= strtoul (iv_str1,NULL,16);
	*iv_value2= strtoul (iv_str2,NULL,16);
	*iv_value3= strtoul (iv_str3,NULL,16);
	
	printf("value = 0x%08x%08x%08x%08x\n", *iv_value0, *iv_value1, *iv_value2, *iv_value3);

	return 0;
	
}

int main(int argc, char* argv[])
{    	
	int copt;
	int filemode = 0;
	int netmode = 0;
	char *infile = NULL;
	char *outfile = NULL;
	int log_enabled = 0;
	int bind_port = 10101;
	int i;

	uint32_t iv_value0;
	uint32_t iv_value1;
	uint32_t iv_value2;
	uint32_t iv_value3;
	
	bool perform_encryption;
	uint32_t emi;
	uint32_t time;
	uint32_t input_data0;
	uint32_t input_data1;
	uint32_t input_data2;
	uint32_t input_data3;
	bool output_console;
	uint32_t number_of_runs;
	bool front_end_input=false;
	bool output_to_file=false;
	bool input_from_file=false;
	uint32_t videoPid=0;
	uint32_t audioPid=0;
	uint32_t pcrPid=0;
	uint32_t ecmPid=0;
	bool process_ecms=false;
	bool use_odd_parity=false;
	char* csv_file;
	TCertKeyParity scr_parity;
	NEXUS_PlatformSettings platformSettings;
	NEXUS_PlatformConfiguration platformConfig;
	NEXUS_DmaSettings dmaSettings;
	NEXUS_StcChannelSettings stcSettings;

	int main_status=-1;

	/* Platform init */
    	NEXUS_Platform_GetDefaultSettings(&platformSettings); 
    	platformSettings.openFrontend = false;
    	NEXUS_Platform_Init(&platformSettings);

	pNexusConfig = &platformConfig;
	NEXUS_Platform_GetConfiguration(pNexusConfig);


	/* Bring up video display and outputs */
	NEXUS_Display_GetDefaultSettings(&displaySettings);
	display = NEXUS_Display_Open(0, &displaySettings);
	window = NEXUS_VideoWindow_Open(display, 0);


#if NEXUS_NUM_COMPONENT_OUTPUTS
	NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(pNexusConfig->outputs.component[0]));
#endif
#if NEXUS_NUM_SCART_INPUTS
	NEXUS_Display_AddOutput(display, NEXUS_ScartInput_GetVideoOutputConnector(pNexusConfig->inputs.scart[0]));
#if NEXUS_NUM_SCART_INPUTS>1
	NEXUS_Display_AddOutput(display, NEXUS_ScartInput_GetVideoOutputConnector(pNexusConfig->inputs.scart[1]));
#endif
#endif
#if NEXUS_NUM_COMPOSITE_OUTPUTS
	if ( pNexusConfig->outputs.composite[0] ) {
		NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(pNexusConfig->outputs.composite[0]));
	}
#endif

	/* bring up decoder and connect to display */
	vdecode = NEXUS_VideoDecoder_Open(0, NULL); /* take default capabilities */
	NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(vdecode));

	/* Bring up audio decoders and outputs */
	pcmDecoder = NEXUS_AudioDecoder_Open(0, NULL);
	compressedDecoder = NEXUS_AudioDecoder_Open(1, NULL);
	NEXUS_AudioOutput_AddInput(
		NEXUS_AudioDac_GetConnector(pNexusConfig->outputs.audioDacs[0]),
		NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo));


	NEXUS_Dma_GetDefaultSettings (&dmaSettings);
#if(CFG_BIG_ENDIAN==1)
	dmaSettings.endianMode = NEXUS_DmaEndianMode_eBig; 
	dmaSettings.swapMode = NEXUS_DmaSwapMode_eNone;     
#endif

	dma = NEXUS_Dma_Open(0, &dmaSettings);
	if ( dma==NULL )
	{
		printf("Can't open DMA");
		main_status=-2;
		goto Exit_main;
	}

	/* Open the StcChannel  */
	NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
	stcSettings.timebase = NEXUS_Timebase_e0;
	stcSettings.mode = NEXUS_StcChannelMode_eAuto; 
	stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);



	printf("done with init\n");

	/* If the user request help, print out help page. */
	if(strcmp(argv[1], "-help") == 0x0)
	{
		cert_usage();
	}	

	/* Return the version of the CERT application. */
	else if(strcmp(argv[1], "-v") == 0x0)
	{
		fprintf(stderr, "The current version fo the CERT app is: %x\n", CERTAPI_VERSION_INT);
	}

	/* Perform the bridge functionality to the CERT IP. */
	else if(strcmp(argv[1], "-bridge") == 0x0)
	{	

		while((copt = getopt(argc, argv, "lfi:o:np:b:h")) != -1) 
		{
			switch (copt)
			{
			case 'l':
				log_enabled = 1;
				break;
			case 'f':
				filemode = 1;
				break;
			case 'n':
				netmode = 1;
				break;
			case 'i':
				infile = optarg;
				break;
			case 'o':
				outfile = optarg;
				break;
			case 'p':
				bind_port = atoi(optarg);
				if(bind_port <= 0) 
				{
					fprintf(stderr, "Invalid bind port.\n");
				}
				break;					
					
			case 'b':
#if 0					
				if( inet_aton( optarg, &bind_addr) == 0 ) 
				{
					
					fprintf(stderr, "Invalid bind address\n");
					return;
				}
#endif						
				break;				
					
			case '?':
				if (isprint(optopt))
					fprintf(stderr, "Unknown option `-%c'.\n", optopt);
				else
					fprintf(stderr, "Unknown option character `0x%x'.\n", optopt);
				bridge_usage(argv);
				goto Exit_main;
			case 'h':
			default:					
				bridge_usage(argv);
				goto Exit_main;
			}
		}

		main_status = cert_bridge(filemode,
					  netmode,
					  infile,
					  outfile,
					  bind_port,
					  log_enabled);

	}

	/* Perform RAM2RAM operation. */
	else if(strcmp(argv[1], "-r2r") == 0x0)
	{			
		/* Parse through the arguments. */
		for (i = 2; i < argc; i++)
		{
			if (strcmp(argv[i], "-enc") == 0)
			{
				perform_encryption = true;
			}
			if (strcmp(argv[i], "-dec") == 0)
			{
				perform_encryption = false;
			}
			if (strcmp(argv[i], "-emi") == 0)
			{
				emi = strtoul (argv[i+1],NULL,16);
			}
			if (strcmp(argv[i], "-iv") == 0)
			{
				if(cert_convert_to_64_128(argv[i+1], &iv_value0, &iv_value1, &iv_value2, &iv_value3))
					goto Exit_main;
			}
			if (strcmp(argv[i], "-if") == 0)
			{
				infile=argv[i+1];				
				input_from_file = true;
			}
			if (strcmp(argv[i], "-ic") == 0)
			{
				if(cert_convert_to_64_128(argv[i+1], &input_data0, &input_data1, &input_data2, &input_data3))
					goto Exit_main;
				input_from_file = false;

			}
			if (strcmp(argv[i], "-of") == 0)
			{
				outfile=argv[i+1];
				output_console = false;
				
			}
			if (strcmp(argv[i], "-oc") == 0)
			{
				output_console = true;
			}
			if (strcmp(argv[i], "-numbruns") == 0)
			{
				number_of_runs= strtoul (argv[i+1],NULL,16);
			}
			if (strcmp(argv[i], "-help") == 0)
			{
				r2r_usage();
				goto Exit_main;
			}
			
			
		}
		
		main_status = cert_r2r(perform_encryption,
				       emi,
				       iv_value0,
				       iv_value1,
				       iv_value2,
				       iv_value3,
				       input_from_file,
				       infile,
				       input_data0,
				       input_data1,
				       input_data2,
				       input_data3,
				       outfile,
				       output_console,
				       number_of_runs );
		
	}
	else if(strcmp(argv[1], "-dsc") == 0x0)
	{
		/* Parse through the arguments. */
		for (i = 2; i < argc; i++)
		{
			if (strcmp(argv[i], "-emi") == 0)
			{
				emi = strtoul (argv[i+1],NULL,16);
				i++;
			} else if (strcmp(argv[i], "-if") == 0)
			{
				infile=argv[i+1];
				i++;
			} else
			if (strcmp(argv[i], "-ife") == 0)
			{
				front_end_input = true;
			} else
			if (strcmp(argv[i], "-of") == 0)
			{
				outfile=argv[i+1];
				output_to_file = true;
				i++;
			} else
			if (strcmp(argv[i], "-videoPid") == 0)
			{
				videoPid = strtoul (argv[i+1],NULL,16);
				i++;
			} else
			if (strcmp(argv[i], "-audioPid") == 0)
			{
				audioPid = strtoul (argv[i+1],NULL,16);
				i++;
			} else
			if (strcmp(argv[i], "-pcrPid") == 0)
			{
				pcrPid = strtoul (argv[i+1],NULL,16);
				i++;
			} else
			if (strcmp(argv[i], "-ecmPid") == 0)
			{
				ecmPid = strtoul (argv[i+1],NULL,16);
				i++;
			} else
			if (strcmp(argv[i], "-pecm") == 0)
			{
				if(strcmp(argv[i+1], "FALSE") == 0)
					process_ecms = false;
				else
					process_ecms = true;
				i++;
			} else
			if (strcmp(argv[i], "-useParity") == 0)
			{
				if(strcmp(argv[i+1], "ODD") == 0)
					use_odd_parity = true;
				else
					use_odd_parity = false;
				i++;
			} else 
			if (strcmp(argv[i], "-ecm2cert") == 0)
			{
				csv_file=argv[i+1];
				i++;
			} else
			if (strcmp(argv[i], "-help") == 0)
			{
				dsc_usage();
				goto Exit_main;
			} else {
				printf("bad argument: %s\n", argv[i]);
				main_status=100;
				goto Exit_main;
			}
			
			
		}
		main_status = cert_dsc(emi,
				       infile,
				       outfile,
				       front_end_input,
				       output_to_file,
				       videoPid,
				       audioPid,
				       pcrPid,
				       ecmPid,
				       process_ecms,
				       use_odd_parity,
				       csv_file	);
	}
	else if(strcmp(argv[1], "-scr") == 0x0)
	{
		/* Parse through the arguments. */
		for (i = 2; i < argc; i++)
		{
			if (strcmp(argv[i], "-enc") == 0)
			{
				perform_encryption = true;
			} else
			if (strcmp(argv[i], "-dec") == 0)
			{			
				perform_encryption = false;
			} else
			if (strcmp(argv[i], "-emi") == 0)
			{			
				emi = strtoul (argv[i+1],NULL,16);
				printf("emi = %x\n", emi);
				i++;
			} else
			if (strcmp(argv[i], "-if") == 0)
			{			
				infile=argv[i+1];
				i++;
			} else
			if (strcmp(argv[i], "-ife") == 0)
			{			
				front_end_input = true;
			} else
			if (strcmp(argv[i], "-of") == 0)
			{			
				outfile=argv[i+1];
				i++;
			} else
			if (strcmp(argv[i], "-iv") == 0)
			{
				if(cert_convert_to_64_128(argv[i+1], &iv_value0, &iv_value1, &iv_value2, &iv_value3)) {
					main_status = 10;
					goto Exit_main;
				}
				i++;
			} else
			
			if (strcmp(argv[i], "-videoPid") == 0)
			{			
				videoPid = strtoul (argv[i+1],NULL,16);
				i++;
			} else
			if (strcmp(argv[i], "-audioPid") == 0)
			{			
				audioPid = strtoul (argv[i+1],NULL,16);
				i++;
			} else
			if (strcmp(argv[i], "-pcrPid") == 0)
			{			
				pcrPid = strtoul (argv[i+1],NULL,16);
				i++;
			} else
			if (strcmp(argv[i], "-useParity") == 0)
			{			
				if(strcmp(argv[i+1], "ODD") == 0)
					scr_parity=CERT_KEY_PARITY_ODD;
				else if(strcmp(argv[i+1], "EVEN") == 0)
					scr_parity=CERT_KEY_PARITY_EVEN;
				else if(strcmp(argv[i+1], "UNDEFINED") == 0)
					scr_parity=CERT_KEY_PARITY_UNDEFINED;
				else
					scr_parity=CERT_KEY_PARITY_UNDEFINED;
				i++;
			} else			
			if (strcmp(argv[i], "-help") == 0)
			{
				scr_usage();
				goto Exit_main;
			} else {
				printf("bad argument: %s\n", argv[i]);
				main_status=100;
				goto Exit_main;
			}
			
			
		}

		main_status = cert_scr(	perform_encryption,
					emi,
					infile	,
					front_end_input,
					outfile,
					iv_value0,
					iv_value1,
					iv_value2,
					iv_value3,
					videoPid,
					audioPid,
					pcrPid,
					scr_parity );
	
	}
	else if(strcmp(argv[1], "-reset") == 0x0)
	{
		/* Parse through the arguments. */
		for (i = 2; i < argc; i++)
		{
			if (strcmp(argv[i], "-t") == 0)
			{
				time = strtoul (argv[i+1],NULL,16);
				i++;
			} else
			if (strcmp(argv[i], "-help") == 0)
			{
				reset_usage();
				goto Exit_main;
			} else {
				printf("bad argument: %s\n", argv[i]);
				main_status=100;
				goto Exit_main;
			}
		}
	
		cert_reset(time);
	}
	else
	{
		cert_usage();
		main_status = 0;
	}	

Exit_main:
	printf("cert main exiting... cleanup\n");
	NEXUS_VideoDecoder_Stop(vdecode);
	NEXUS_VideoWindow_RemoveInput(window, NEXUS_VideoDecoder_GetConnector(vdecode));
	NEXUS_VideoInput_Shutdown(NEXUS_VideoDecoder_GetConnector(vdecode));
	NEXUS_VideoDecoder_Close(vdecode);
	NEXUS_VideoWindow_Close(window);
	NEXUS_Display_Close(display);


	NEXUS_AudioDecoder_Stop(compressedDecoder);
	NEXUS_AudioDecoder_Stop(pcmDecoder);
	NEXUS_AudioOutput_RemoveAllInputs(NEXUS_AudioDac_GetConnector(pNexusConfig->outputs.audioDacs[0]));
	NEXUS_AudioOutput_RemoveAllInputs(NEXUS_SpdifOutput_GetConnector(pNexusConfig->outputs.spdif[0]));
	NEXUS_AudioInput_Shutdown(NEXUS_AudioDecoder_GetConnector(pcmDecoder, NEXUS_AudioDecoderConnectorType_eStereo));
	NEXUS_AudioDecoder_Close(compressedDecoder);
	NEXUS_AudioDecoder_Close(pcmDecoder);

	NEXUS_Dma_Close(dma);
	NEXUS_StcChannel_Close(stcChannel);

    	NEXUS_Platform_Uninit();



	return main_status;
}

