#include "bchp_sun_top_ctrl.h"
#include "bchp_nand.h"

#include "nexus_platform.h"
#include "nexus_dma.h"
#include "nexus_types.h"
#include "nexus_memory.h"
#include "nexus_security_datatypes.h"
#include "nexus_security.h"
#include "nexus_core_security.h"     /* to pick up definitions for NEXUS_KeySlotHandle */
#include "nexus_security_types.h"
#include "priv/nexus_security_priv.h"
#include "nexus_otpmsp.h"
#include "nexus_keyladder.h"
#include "nocs3x_csd_impl.h"
#include "nocs3x_csd.h"
#include "bdbg.h"
#include "bint.h"
#include "bstd.h"
#include "bkni.h"
#include "ca_cert.h"
#include "ca_cert_impl.h"
#include "bchp_bsp_pkl.h"

#include <syslog.h>

#define CERT_DATA_IN ( BCHP_BSP_PKL_PKLi_ARRAY_BASE) /* PKL Register Address 0 */
#define CERT_DATA_OUT (CERT_DATA_IN+32)
#define CERT_STATUS_AKL (CERT_DATA_IN+64)
#define CERT_COMMAND (CERT_DATA_IN+68)
#define CERT_INTERRUPT (CERT_DATA_IN+72)

/* stubs */
void swap_bytes(unsigned char *input);

TCertStatus certInit(void);

TCertFunctionTable certTable;
TCertResourceHandle ResourceHandle;

TCsdStatus certFreeHandleNode(
	TCsdInitParameters*  pxInitParameters, 
	NEXUS_KeySlotHandle handle)
{
	handleNode curNode, prevNode;

	/* always start at root */
	curNode  = pxInitParameters->rootHandleNode;
	prevNode = pxInitParameters->rootHandleNode;

	/* Search through all nodes for the key slot handle we want to free. */
	while (curNode)
	{
		if (handle == curNode->handle)
			break;
		prevNode = curNode;
		curNode  = curNode->nextNode;
	}

	if (curNode)
	{
		/* found the node to release */
		NEXUS_Security_FreeKeySlot(handle);

		/* remove the node from the link */
		prevNode->nextNode = curNode->nextNode;
		if (curNode == pxInitParameters->rootHandleNode)
			pxInitParameters->rootHandleNode = curNode->nextNode;

		pxInitParameters->handleNodeCount -= 1;

		/* release the node storage */
		NEXUS_Memory_Free((void *)curNode);
	}
	else
		/* The linked list does not contain the requested handle, return error. */
		return CSD_ERROR_INVALID_PARAMETERS;

	return CSD_NO_ERROR;
	
}

static void certInsertHandleNode(
	TCsdInitParameters*  	pxInitParameters, 
	NEXUS_KeySlotHandle	handle
	)
{
	handleNode node;

	/* allocate the memory for the node storage */
	NEXUS_Memory_Allocate(sizeof(struct keyHandleNode), NULL, (void *)&node);
	node->handle = handle;

	/* always insert to the front */
	node->nextNode = pxInitParameters->rootHandleNode;
	pxInitParameters->rootHandleNode   = node;
	pxInitParameters->handleNodeCount += 1;

	return;
	
}

TCertStatus certInit(
	void
	)
{
	TCertStatus status = CERT_ERROR;
       static bool firstTime=true;
	
	if(firstTime) 
	{
		/* Init the PKL */	
		NEXUS_Security_PklInit();
		firstTime=false;

		if(ResourceHandle != NULL)
			BKNI_Free(ResourceHandle);	
		
		if ((ResourceHandle = 
					(TCertResourceHandle)BKNI_Malloc( sizeof( SCertResourceHandle))) 
					== NULL) 
		{
			/* wrap initially detected error code */
			status = CERT_ERROR_BAD_HANDLE;
			goto Exit_certInit;
		}
		BKNI_CreateMutex( &(ResourceHandle->mutexLock));
	}
	
	status = CERT_NO_ERROR;
	
Exit_certInit:
	return status;
}


TCertStatus certLock(
	TCertResourceHandle *pxResourceHandle
	)
{	

	certInit();

	BKNI_AcquireMutex(ResourceHandle->mutexLock);

	*pxResourceHandle = ResourceHandle;

	return CERT_NO_ERROR;
}

TCertStatus certUnlock(
        TCertResourceHandle  xResourceHandle
	)
{
	BKNI_ReleaseMutex(xResourceHandle->mutexLock);

	return CERT_NO_ERROR;
}

TCertStatus certExchange(
	TCertResourceHandle  	xResourceHandle,
	size_t               		xNumOfCommands,
  	const TCertCommand* 	pxCommands,
	size_t*             		pxNumOfProcessedCommands
	)
{
	uint32_t cmd_complete;
	uint32_t data;
	TCertStatus status;
	unsigned int j, i = 0;
	syslog(LOG_DEBUG, "%s:%d\n", __FUNCTION__, __LINE__);
	status = CERT_ERROR;
	
	/* process each command in the buffer. */
	for(i=0;i<xNumOfCommands;i++)
	{
		/* write the input data. */
		for(j=0;j<32;j+=4)
		{
#if CFG_BIG_ENDIAN == 0
			data = (*(uint32_t*)&(pxCommands[i].inputData[j])<<24) & 0xFF000000;
			data |= (*(uint32_t*)&(pxCommands[i].inputData[j+1])<<16) & 0x00FF0000;
			data |= (*(uint32_t*)&(pxCommands[i].inputData[j+2])<<8) & 0x0000FF00;
			data |= (*(uint32_t*)&(pxCommands[i].inputData[j+3])) & 0x000000FF;
#else
			data = *(uint32_t*)&(pxCommands[i].inputData[j]);
#endif
	
			NEXUS_Platform_WriteRegister( CERT_DATA_IN+j, data );
		}	

#if CFG_BIG_ENDIAN == 0
		data = ((uint32_t)(pxCommands[i].opcodes[0])<<24) & 0xFF000000;
		data |= ((uint32_t)pxCommands[i].opcodes[1]<<16) & 0x00FF0000;
		data |= ((uint32_t)pxCommands[i].opcodes[2]<<8) & 0x0000FF00;
		data |= ((uint32_t)pxCommands[i].opcodes[3]) & 0x000000FF;
#else
		data = (uint32_t)(pxCommands[i].opcodes[0]);
#endif		
	
		/* write the opcdoe to the CERT. */
		NEXUS_Platform_WriteRegister( CERT_COMMAND, data);

		syslog(LOG_DEBUG, "opcodes = 0x%x\n",  data);
		
		NEXUS_Platform_ReadRegister( CERT_COMMAND, &cmd_complete );

		syslog(LOG_DEBUG, "cmd_complete = 0x%x\n",  cmd_complete);
#if POLL
		/* wait for command to complete. */
		do
		{
			NEXUS_Platform_ReadRegister( CERT_COMMAND, &cmd_complete );
		}while((cmd_complete&0x1) == 0x1);
#else
		if((pxCommands->timeout) == CERT_TIMEOUT_DEFAULT)
			BKNI_Sleep( CERT_DEFAULT_TIMEOUT_VALUE);
		else
			BKNI_Sleep( CERT_OTP_CMD_TIMEOUT_VALUE);
		
		NEXUS_Platform_ReadRegister( CERT_COMMAND, &cmd_complete );
		if((cmd_complete&0x1) == 0x1)
		{
			syslog(LOG_ERR, "ERROR: Command did not complete in time!!!\n");
			status = CERT_ERROR_TIMEOUT;
			goto Exit_TCertExchange;
		}
		else
			syslog(LOG_DEBUG, "Command completed\n");
#endif

		/* clear the interrupt. */
		NEXUS_Platform_ReadRegister( CERT_INTERRUPT, &cmd_complete );
		NEXUS_Platform_WriteRegister( CERT_INTERRUPT, cmd_complete );
		
		/* read the status of the command. */
		NEXUS_Platform_ReadRegister( CERT_STATUS_AKL, (uint32_t *)&(pxCommands[i].status[0]) );

		swap_bytes((unsigned char *)&(pxCommands[i].status[0]));	

		/* read the output data. */
		for(j=0;j<32;j+=4)
		{
			NEXUS_Platform_ReadRegister( CERT_DATA_OUT+j, (uint32_t*)&(pxCommands[i].outputData[j]));

			swap_bytes((unsigned char *)&(pxCommands[i].outputData[j]));	
		}
#if 0	/* HUMAX_PLATFORM_BASE */ 
	/* In spec, LSB '1' in status[0] is current, but not correct.
	* check '1' in stats[0], working fine so patched.
	*/
		if( (*(TCertUInt32*)&(pxCommands[i].status[0]) & 0x1)== 0x1)
#else
		if(pxCommands[i].status[3] & 0x1)
#endif
		{
#if 1	/* HUMAX_PLATFORM_BASE */
			i++;
#endif
			status = CERT_ERROR;
			goto Exit_TCertExchange;
		}
	}		

	status = CERT_NO_ERROR;
	
Exit_TCertExchange:	
	
	*pxNumOfProcessedCommands = i;
	return status;
}

#if CFG_BIG_ENDIAN == 0
void swap_bytes(unsigned char *input)
{
	unsigned char temp;

	temp = input[0];
	input[0] = input[3];
	input[3] = temp;
	
	temp = input[2];
	input[2] = input[1];
	input[1] = temp;

	return;
}
#else
void swap_bytes(unsigned char *input)
{
	return;
}
#endif
			
TCertStatus certOperation(	
	unsigned char*	command,
	unsigned int 		cert_cmd_maxcycles,
	unsigned int 		din_size,
	unsigned char* 	cert_data_in,
	unsigned int 		dout_size,
	unsigned char* 	cert_data_out,
	unsigned char* 	cert_status,
	unsigned int* 		cmd_cycles
	)
{
	uint32_t cmd_complete;
	uint32_t data;
	TCertStatus status;
	unsigned int j;
	/* unsigned char temp; */
	syslog(LOG_DEBUG, "%s:%d\n", __FUNCTION__, __LINE__);
	status = CERT_ERROR;

	syslog(LOG_DEBUG, "cert cycles = 0x%x\n", cert_cmd_maxcycles);

	syslog(LOG_DEBUG, "Writing to input registers\n");

	/* write the input data. */
	for(j=0;j<din_size;j+=4)
	{
#if CFG_BIG_ENDIAN == 0
		data = (*(uint32_t*)(cert_data_in+j)<<24) & 0xFF000000;
		data |= (*(uint32_t*)(cert_data_in+j+1)<<16) & 0x00FF0000;
		data |= (*(uint32_t*)(cert_data_in+j+2)<<8) & 0x0000FF00;
		data |= (*(uint32_t*)(cert_data_in+j+3)) & 0x000000FF;
#else
		data = *(uint32_t*)(cert_data_in+j);
#endif

		NEXUS_Platform_WriteRegister( CERT_DATA_IN+j, data);

		syslog(LOG_DEBUG, "Addr 0x%08x = 0x%08x\n", CERT_DATA_IN+j, data);
	}		

	syslog(LOG_DEBUG, "Writing to command register\n");
	syslog(LOG_DEBUG, "command[0..3]=%02x %02x %02x %02x\n", *(char*)(command), 
																													*(char*)(command+1), 
																													*(char*)(command+2), 
																													*(char*)(command+3));

	data = (*(char*)(command)<<24) & 0xFF000000;
	data |= (*(char*)(command+1)<<16) & 0x00FF0000;
	data |= (*(char*)(command+2)<<8) & 0x0000FF00;
	data |= (*(char*)(command+3)) & 0x000000FF;

	syslog(LOG_DEBUG, "Addr 0x%08x = 0x%08x\n", CERT_COMMAND, data);

	
	/* write the opcdoe to the CERT. */
	NEXUS_Platform_WriteRegister( CERT_COMMAND, data);

		
	NEXUS_Platform_ReadRegister( CERT_COMMAND, &cmd_complete );

	/* wait for command to complete. */
	/* Perfrom a sleep to wait for the AKL to respond.  The sleep time depends on the 
	   input.  The cmd_cycles can take on 2 values.
	   0: wait a normal time for the command to complete.
	   1: wait a long time (for OTP commands) to wait for the command to complete.
	   If the command does not finish on time */
#define POLL 1
#if POLL
	do
	{
		NEXUS_Platform_ReadRegister( CERT_COMMAND, &cmd_complete );
	}while((cmd_complete&0x1) == 0x1);
	
#else
	if(cert_cmd_maxcycles == CERT_TIMEOUT_DEFAULT)
		BKNI_Sleep( CERT_DEFAULT_TIMEOUT_VALUE);
	else
		BKNI_Sleep( CERT_OTP_CMD_TIMEOUT_VALUE);
	
	NEXUS_Platform_ReadRegister( CERT_COMMAND, &cmd_complete );
	if((cmd_complete&0x1) == 0x1)
	{
		syslog(LOG_ERR, "ERROR: Command did not complete in time!!!\n");
		status = CERT_ERROR_TIMEOUT;
		goto Exit_certOperation;
	}
	else
		syslog(LOG_DEBUG, "Command completed\n");
#endif

	/* clear the interrupt. */
	NEXUS_Platform_ReadRegister( CERT_INTERRUPT, &cmd_complete );

	NEXUS_Platform_WriteRegister( CERT_INTERRUPT, (cmd_complete & 0xFF) );
		
	/* read the status of the command. */
	NEXUS_Platform_ReadRegister( CERT_STATUS_AKL, (uint32_t*)(cert_status));

	swap_bytes(cert_status);

	syslog(LOG_DEBUG, "Reading Status register\n");
	syslog(LOG_DEBUG, "CERT_STATUS_AKL = 0x%02x%02x%02x%02x\n", cert_status[0],
																															cert_status[1],
																															cert_status[2],
																															cert_status[3]);
		
	/* read the output data. */
	for(j=0;j<dout_size;j+=4)
	{
		NEXUS_Platform_ReadRegister( CERT_DATA_OUT+j, (uint32_t*)(cert_data_out+j));

		swap_bytes(cert_data_out+j);

		syslog(LOG_DEBUG, "CERT_DATA_OUT = 0x%02x%02x%02x%02x\n", *(cert_data_out+j),
																															*(cert_data_out+j+1),
																															*(cert_data_out+j+2),
																															*(cert_data_out+j+3));
	}

	cmd_cycles = 0; /* not supported */
	
	status = CERT_NO_ERROR;

Exit_certOperation:
	
	return status;
}

TCertStatus certUseDecryptionKey(
	TCertUInt16           	xEmi,
	TCertKeyPathHandle*  	pxKeyPathHandle,
	TCertKeyParity        	xKeyParity
	)
{
	NEXUS_KeySlotHandle keyHandle;
	NEXUS_SecurityAlgorithmSettings	NexusConfig;
	NEXUS_SecurityKeySlotSettings	keySlotSettings;
	NEXUS_SecurityEncryptedControlWord	CW; 
	BERR_Code errCode=CERT_ERROR;
	bool bConfigureSCR=false;
	TCertStatus status = CERT_ERROR;

	memset(&NexusConfig, 0, sizeof(NEXUS_SecurityAlgorithmSettings));
	memset(&keySlotSettings, 0, sizeof(NEXUS_SecurityKeySlotSettings));
	memset(&CW, 0, sizeof(NEXUS_SecurityEncryptedControlWord));

	keyHandle = NULL;

    	NEXUS_Security_GetDefaultAlgorithmSettings(&NexusConfig);
	NexusConfig.dest = pxKeyPathHandle->dest; 

	NexusConfig.operation = NEXUS_SecurityOperation_eDecrypt;

	if(xKeyParity==CERT_KEY_PARITY_EVEN)
		NexusConfig.keyDestEntryType = NEXUS_SecurityKeyType_eEven; 		
	else if(xKeyParity==CERT_KEY_PARITY_ODD)
		NexusConfig.keyDestEntryType = NEXUS_SecurityKeyType_eOdd; 
	else
		NexusConfig.keyDestEntryType = NEXUS_SecurityKeyType_eOddAndEven; 


	if(pxKeyPathHandle->keySlotEngine == NEXUS_SecurityEngine_eCa)
	{

		NexusConfig.keyDestEntryType = NEXUS_SecurityKeyType_eOddAndEven; 

		/* it's an allocated keyslot? */
		keyHandle = NEXUS_Security_LocateCaKeySlotAssigned(pxKeyPathHandle->dscVideoHandle->initHandle->pidchannel);
        		
		if (!keyHandle)
		{	/* if not, try to allocate a new one*/
			NEXUS_Security_GetDefaultKeySlotSettings(&keySlotSettings);
			keySlotSettings.keySlotEngine = pxKeyPathHandle->keySlotEngine; 
			keyHandle = NEXUS_Security_AllocateKeySlot(&keySlotSettings);

			if( keyHandle == NULL) 
			{ 
				status = CERT_ERROR;
				goto CERT_DEC_P_DONE;
			}
			else
			{
				/* Set key handle for both Audio and video. */
				certInsertHandleNode(pxKeyPathHandle->dscVideoHandle->initHandle, keyHandle);
				certInsertHandleNode(pxKeyPathHandle->dscAudioHandle->initHandle, keyHandle);
			}

			/* Connect key handle to the video PID. */
			errCode = NEXUS_Security_AddPidChannelToKeySlot(keyHandle, 
									pxKeyPathHandle->dscVideoHandle->initHandle->pidchannel);

			if (errCode != 0) 
			{
				syslog(LOG_ERR, "NEXUS_Security_AddPidChannelToKeySlot video errCode: %x\n", errCode );	
				status = CERT_ERROR;
				goto CERT_DEC_P_DONE;
			}
        			
			/* Connect key handle to the audio PID. */
			errCode = NEXUS_Security_AddPidChannelToKeySlot(keyHandle, 
									pxKeyPathHandle->dscAudioHandle->initHandle->pidchannel);

			if (errCode != 0) 
			{
				syslog(LOG_ERR, "NEXUS_Security_AddPidChannelToKeySlot audio errCode: %x\n", errCode );	
				status = CERT_ERROR;
				goto CERT_DEC_P_DONE;
			}
		}
	}
	else if(pxKeyPathHandle->keySlotEngine == NEXUS_SecurityEngine_eM2m)
	{
		/* For mem2mem only ODD key are allowed overrides xKeyParity */
		NexusConfig.keyDestEntryType = NEXUS_SecurityKeyType_eOdd;

		if(pxKeyPathHandle->bLoadScr == false)
		{
		 	NEXUS_Security_GetDefaultKeySlotSettings(&keySlotSettings);
	    		keySlotSettings.keySlotEngine = pxKeyPathHandle->keySlotEngine; 
	        	keyHandle = NEXUS_Security_AllocateKeySlot(&keySlotSettings);

			pxKeyPathHandle->r2rHandle->keyHandle = keyHandle;

			if( keyHandle == NULL) 
			{ 
				status = CERT_ERROR;
				goto CERT_DEC_P_DONE;
			}
			else
			{
				certInsertHandleNode(pxKeyPathHandle->r2rHandle->initHandle, keyHandle);
			}
		}
		else
		{		
		 	NEXUS_Security_GetDefaultKeySlotSettings(&keySlotSettings);
	    		keySlotSettings.keySlotEngine = pxKeyPathHandle->keySlotEngine; 
	        	keyHandle = NEXUS_Security_AllocateKeySlot(&keySlotSettings);

			pxKeyPathHandle->scrHandle->keyHandle = keyHandle;

			if( keyHandle == NULL) 
			{ 
				status = CERT_ERROR;
				goto CERT_DEC_P_DONE;
			}
			else
			{
				bConfigureSCR=true;
				pxKeyPathHandle->scrHandle->dataFormat = NEXUS_DmaDataFormat_eMpeg;
				certInsertHandleNode(pxKeyPathHandle->scrHandle->initHandle, keyHandle);
			}			
		}
	}


	switch(xEmi)
	{
		case CSD_Algorithm_ASA: /* 0x0010 */
		case 0x0011:
		NexusConfig.algorithm = NEXUS_SecurityAlgorithm_eAsa;
		NexusConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eXpt;
		NexusConfig.terminationMode = NEXUS_SecurityTerminationMode_eClear;
		NexusConfig.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
		NexusConfig.ivMode = NEXUS_SecurityIVMode_eRegular;
		NexusConfig.dvbScramLevel = NEXUS_SecurityDvbScrambleLevel_eTs;
		CW.keySize = 16;
		break;
			
		case CSD_Algorithm_DVB_CSA2: /* 0x0000 */
		NexusConfig.algorithm = NEXUS_SecurityAlgorithm_eDvb;
		NexusConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eXpt;
		NexusConfig.terminationMode = NEXUS_SecurityTerminationMode_eClear;
		NexusConfig.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
		NexusConfig.ivMode = NEXUS_SecurityIVMode_eRegular;
		NexusConfig.dvbScramLevel = NEXUS_SecurityDvbScrambleLevel_eTs;
		CW.keySize = 8;
		break;
		
		case CSD_Algorithm_DVB_CSA3: /* 0x0001 */
		NexusConfig.algorithm = NEXUS_SecurityAlgorithm_eDvbCsa3;
		NexusConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eXpt;
		NexusConfig.terminationMode = NEXUS_SecurityTerminationMode_eClear;
		NexusConfig.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
		NexusConfig.ivMode = NEXUS_SecurityIVMode_eRegular;
		NexusConfig.dvbScramLevel = NEXUS_SecurityDvbScrambleLevel_eTs;
		CW.keySize = 16;
		break;            
		
		case CSD_Algorithm_AES_128_CBC_IV_0_SCTE52: /* 0x0020 */
		NexusConfig.algorithm = NEXUS_SecurityAlgorithm_eAes;
		NexusConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;
		NexusConfig.terminationMode = NEXUS_SecurityTerminationMode_eCbcResidual;
		NexusConfig.solitarySelect = NEXUS_SecuritySolitarySelect_eCbcXorIV; 
		NexusConfig.ivMode = NEXUS_SecurityIVMode_eRegular;
		CW.keySize = 16;
		
		if(bConfigureSCR==true) 
		{
			pxKeyPathHandle->scrHandle->ivSize = 16;
		}
			
		break;
		
		case CSD_Algorithm_RAW_AES_128_ECB_CLEAR:  /* 0x4021 */
		NexusConfig.algorithm = NEXUS_SecurityAlgorithm_eAes;
		NexusConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
		NexusConfig.terminationMode = NEXUS_SecurityTerminationMode_eClear;
		NexusConfig.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
		NexusConfig.ivMode = NEXUS_SecurityIVMode_eRegular;
		if(bConfigureSCR==true) 
		{
			pxKeyPathHandle->scrHandle->dataFormat = NEXUS_DmaDataFormat_eBlock;
		}
		CW.keySize = 16;
		break;

		case CSD_Algorithm_AES_128_ECB_CLEAR: /* 0x0021 */
		case CSD_Algorithm_M2M_AES_128_ECB_CLEAR: /* 0x4025 */
		NexusConfig.algorithm = NEXUS_SecurityAlgorithm_eAes;
		NexusConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
		NexusConfig.terminationMode = NEXUS_SecurityTerminationMode_eClear;
		NexusConfig.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
		NexusConfig.ivMode = NEXUS_SecurityIVMode_eRegular;
		CW.keySize = 16;
			
		break;
		
		case CSD_Algorithm_RAW_AES_128_CBC_IV_0_CLEAR: /* 0x4020 */
		NexusConfig.algorithm = NEXUS_SecurityAlgorithm_eAes;
		NexusConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;
		NexusConfig.terminationMode = NEXUS_SecurityTerminationMode_eClear;
		NexusConfig.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
		NexusConfig.ivMode = NEXUS_SecurityIVMode_eRegular;
		if(bConfigureSCR==true) 
		{
			pxKeyPathHandle->scrHandle->dataFormat = NEXUS_DmaDataFormat_eBlock;
			pxKeyPathHandle->scrHandle->ivSize = 16;
		}
		CW.keySize = 16;
			
		break;
		
		case CSD_Algorithm_AES_128_CBC_CLEAR: /* 0x0022 */
		case CSD_Algorithm_M2M_AES_128_CBC_CLEAR: /* 0x4026 */
		NexusConfig.algorithm = NEXUS_SecurityAlgorithm_eAes;
		NexusConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;
		NexusConfig.terminationMode = NEXUS_SecurityTerminationMode_eClear;
		NexusConfig.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
		NexusConfig.ivMode = NEXUS_SecurityIVMode_eRegular;
		CW.keySize = 16;
		if(bConfigureSCR==true) 
		{
			pxKeyPathHandle->scrHandle->ivSize = 16;
		}
		break;
		
		case CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52: /* 0x0040 */
		NexusConfig.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;
		NexusConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;
		NexusConfig.terminationMode = NEXUS_SecurityTerminationMode_eCbcResidual;
		NexusConfig.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;  /* CFV this is not as specified */
		NexusConfig.ivMode = NEXUS_SecurityIVMode_eRegular;
		CW.keySize = 16;
		if(bConfigureSCR==true) 
		{
			pxKeyPathHandle->scrHandle->ivSize = 8;
		}
		break;
		
		case CSD_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR: /* 0x4040 */
		case CSD_Algorithm_RAW_TDES_ABA_CBC_CLEAR: /* 0x4043 */
		NexusConfig.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;
		NexusConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;
		NexusConfig.terminationMode = NEXUS_SecurityTerminationMode_eClear;
		NexusConfig.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
		NexusConfig.ivMode = NEXUS_SecurityIVMode_eRegular;
		CW.keySize = 16;
		if(bConfigureSCR==true) 
		{
			pxKeyPathHandle->scrHandle->dataFormat = NEXUS_DmaDataFormat_eBlock;
			pxKeyPathHandle->scrHandle->ivSize = 8;
		}
		break;

		case CSD_Algorithm_TDES_ABA_ECB_CLEAR: /* 0x0041 */
		NexusConfig.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;
		NexusConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
		NexusConfig.terminationMode = NEXUS_SecurityTerminationMode_eClear;
		NexusConfig.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
		NexusConfig.ivMode = NEXUS_SecurityIVMode_eRegular;
		CW.keySize = 16;
		break;
		
		case CSD_Algorithm_RAW_TDES_ABA_ECB_CLEAR: /* 0x4041 */
		case CSD_Algorithm_M2M_TDES_ABA_ECB_CLEAR: /* 0x4042 */
		NexusConfig.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;
		NexusConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
		NexusConfig.terminationMode = NEXUS_SecurityTerminationMode_eClear;
		NexusConfig.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
		NexusConfig.ivMode = NEXUS_SecurityIVMode_eRegular;
		CW.keySize = 16;
		if(bConfigureSCR==true) 
		{
			pxKeyPathHandle->scrHandle->dataFormat = NEXUS_DmaDataFormat_eBlock;
		}
		break;
		
		default:
		syslog(LOG_ERR, "Unknown EMI: %04x\n", xEmi);
		status = CERT_ERROR_BAD_EMI;
		goto CERT_DEC_P_DONE;
	}

	NexusConfig.caVendorID=0x97ed;
	NexusConfig.otpId = NEXUS_SecurityOtpId_eOtpVal;
	NexusConfig.testKey2Select = false;
	NexusConfig.key2Select = NEXUS_SecurityKey2Select_eFixedKey;

	errCode = NEXUS_Security_ConfigAlgorithm(keyHandle, &NexusConfig);

	if (errCode != 0) 
	{
		syslog(LOG_ERR, "NEXUS_Security_ConfigAlgorithm(odd) errCode: %x\n", errCode );	
		status = CERT_ERROR_BAD_USAGE;
		goto CERT_DEC_P_DONE;
	}

	if ( xKeyParity==CERT_KEY_PARITY_ODD)		
		CW.keyEntryType = NEXUS_SecurityKeyType_eOdd; 
	else if(xKeyParity == CERT_KEY_PARITY_EVEN)
		CW.keyEntryType = NEXUS_SecurityKeyType_eEven; 
	else
		CW.keyEntryType = NEXUS_SecurityKeyType_eOddAndEven; 

	CW.rootKeySwap = false;
	CW.custSubMode = NEXUS_SecurityCustomerSubMode_eReserved21;
	CW.bASKMMode = true;
	CW.keyLadderSelect = NEXUS_SecurityKeyLadderSelect_ePKL;
	CW.bkeyGenBlocked = true;
	CW.keyGenCmdID = NEXUS_SecurityKeyGenCmdID_eKeyGen;
	CW.keyladderID = NEXUS_SecurityKeyladderID_eA; 
	CW.keyGenCmdID   = NEXUS_SecurityKeyGenCmdID_eKeyGen; 
	CW.keyladderType = NEXUS_SecurityKeyladderType_e3Des; 
	CW.rootKeySrc = NEXUS_SecurityRootKeySrc_eOtpKeyA; 
	CW.keyDestIVType = NEXUS_SecurityKeyIVType_eNoIV;
	CW.swizzleType = NEXUS_SecuritySwizzleType_eNone; 
	CW.bASKMMode = true;
	CW.keyMode = NEXUS_SecurityKeyMode_eRegular;
	CW.operation = NEXUS_SecurityOperation_eDecrypt; 
	CW.bSwapAESKey = false;
	CW.bRouteKey = true;
	CW.keySize = 16;
	CW.virtualKeyLadderID = 0x0;
	CW.sc01Polarity[NEXUS_SecurityPacketType_eGlobal] = 0x0;
	CW.sc01Polarity[NEXUS_SecurityPacketType_eRestricted] = 0x0;

	memset(CW.keyData, 0, sizeof(unsigned char) * 16 );

	errCode = NEXUS_Security_GenerateControlWord(keyHandle , &CW);
	
	if (errCode != 0) 
	{
		syslog(LOG_ERR, "NEXUS_Security_GenerateControlWord errCode: 0x%x\n", errCode );
		status = CERT_ERROR_BAD_USAGE;
		goto CERT_DEC_P_DONE;
	}

	/* load IV key if needed */
	if ( (xEmi == CSD_Algorithm_AES_128_CBC_IV_0_SCTE52) ) /* 0x0020 */
	{	
		NEXUS_SecurityClearKey ivKey;

		BKNI_Memset((void *)&ivKey, 0, sizeof(ivKey));
		NEXUS_Security_GetDefaultClearKey(&ivKey);
		ivKey.keyIVType = NEXUS_SecurityKeyIVType_eIV;
		ivKey.keyEntryType = NEXUS_SecurityKeyType_eIv;
		ivKey.keySize = 16; /*xInitializationVectorSize;*/
    
		/* Load IV. */
		errCode = NEXUS_Security_LoadClearKey(keyHandle , &ivKey);
		if (errCode != 0) 
		{
			syslog(LOG_ERR, "\nLoad IV failed \n");
			status = CERT_ERROR;
			goto CERT_DEC_P_DONE;
		}
	}

	status = CERT_NO_ERROR;
	
CERT_DEC_P_DONE: 
	
	return status;
}

TCertStatus certUseEncryptionKey(
	TCertUInt16           	xEmi,
	TCertKeyPathHandle*  	pxKeyPathHandle,
	TCertKeyParity        	xKeyParity
	)
{
	NEXUS_KeySlotHandle keyHandle;
	NEXUS_SecurityAlgorithmSettings	NexusConfig;
	NEXUS_SecurityKeySlotSettings	keySlotSettings;
	NEXUS_SecurityEncryptedControlWord	CW; 
	BERR_Code errCode=CERT_ERROR;
	bool bConfigureSCR=false;
	TCertStatus status = CERT_ERROR;

	memset(&NexusConfig, 0, sizeof(NEXUS_SecurityAlgorithmSettings));
	memset(&keySlotSettings, 0, sizeof(NEXUS_SecurityKeySlotSettings));
	memset(&CW, 0, sizeof(NEXUS_SecurityEncryptedControlWord));

	/* it's an allocated keyslot? */
	keyHandle = NULL;

	NEXUS_Security_GetDefaultAlgorithmSettings(&NexusConfig);
	NexusConfig.dest = pxKeyPathHandle->dest; 

	NexusConfig.operation = NEXUS_SecurityOperation_eEncrypt;
	
	if(xKeyParity==CERT_KEY_PARITY_EVEN)
		NexusConfig.keyDestEntryType = NEXUS_SecurityKeyType_eEven; 		
	else if(xKeyParity==CERT_KEY_PARITY_ODD)
		NexusConfig.keyDestEntryType = NEXUS_SecurityKeyType_eOdd; 
	else
		NexusConfig.keyDestEntryType = NEXUS_SecurityKeyType_eOddAndEven; 

	if(pxKeyPathHandle->keySlotEngine == NEXUS_SecurityEngine_eCa)
	{
		/* it's an allocated keyslot? */
		keyHandle = NEXUS_Security_LocateCaKeySlotAssigned(pxKeyPathHandle->dscVideoHandle->initHandle->pidchannel);
        		
		if (!keyHandle)
		{	/* if not, try to allocate a new one*/
			NEXUS_Security_GetDefaultKeySlotSettings(&keySlotSettings);
			keySlotSettings.keySlotEngine = pxKeyPathHandle->keySlotEngine; 
			keyHandle = NEXUS_Security_AllocateKeySlot(&keySlotSettings);

			if( keyHandle == NULL) 
			{ 
				status = CERT_ERROR;
				goto CERT_ENC_P_DONE;
			}
			else
			{
				/* Set key handle for both Audio and video. */
				certInsertHandleNode(pxKeyPathHandle->dscVideoHandle->initHandle, keyHandle);
				certInsertHandleNode(pxKeyPathHandle->dscAudioHandle->initHandle, keyHandle);
			}

			/* Connect key handle to the video PID. */
			errCode = NEXUS_Security_AddPidChannelToKeySlot(keyHandle, 
										pxKeyPathHandle->dscVideoHandle->initHandle->pidchannel);

			if (errCode != 0) 
			{
				syslog(LOG_ERR, "NEXUS_Security_AddPidChannelToKeySlot errCode: %x\n", errCode );	
				status = CERT_ERROR;
				goto CERT_ENC_P_DONE;
			}
        			
			/* Connect key handle to the audio PID. */
			errCode = NEXUS_Security_AddPidChannelToKeySlot(keyHandle, 
									pxKeyPathHandle->dscAudioHandle->initHandle->pidchannel);

			if (errCode != 0) 
			{
				syslog(LOG_ERR, "NEXUS_Security_AddPidChannelToKeySlot errCode: %x\n", errCode );	
				status = CERT_ERROR;
				goto CERT_ENC_P_DONE;
			}
		}
	}
	else if(pxKeyPathHandle->keySlotEngine == NEXUS_SecurityEngine_eM2m)
	{
		/* For mem2mem only ODD key are allowed overrides xKeyParity */
		NexusConfig.keyDestEntryType = NEXUS_SecurityKeyType_eOdd;

		if(pxKeyPathHandle->bLoadScr == false)
		{

			NEXUS_Security_GetDefaultKeySlotSettings(&keySlotSettings);
			keySlotSettings.keySlotEngine = pxKeyPathHandle->keySlotEngine; 
			keyHandle = NEXUS_Security_AllocateKeySlot(&keySlotSettings);

			pxKeyPathHandle->r2rHandle->keyHandle = keyHandle;

			if( keyHandle == NULL) 
			{ 
				status = CERT_ERROR;
				goto CERT_ENC_P_DONE;
			}
			else
			{
				certInsertHandleNode(pxKeyPathHandle->r2rHandle->initHandle, keyHandle);
			}
		}
		else
		{
			NEXUS_Security_GetDefaultKeySlotSettings(&keySlotSettings);
	    		keySlotSettings.keySlotEngine = pxKeyPathHandle->keySlotEngine; 
	        	keyHandle = NEXUS_Security_AllocateKeySlot(&keySlotSettings);

			pxKeyPathHandle->scrHandle->keyHandle = keyHandle;

			if( keyHandle == NULL) 
			{ 
				status = CERT_ERROR;
				goto CERT_ENC_P_DONE;
			}
			else
			{
				bConfigureSCR=true;
				pxKeyPathHandle->scrHandle->dataFormat = NEXUS_DmaDataFormat_eMpeg;
				certInsertHandleNode(pxKeyPathHandle->scrHandle->initHandle, keyHandle);
			}
		}
	}
	

	switch(xEmi)
	{
	case CSD_Algorithm_ASA: /* 0x0010 */
	case 0x0011:
		NexusConfig.algorithm = NEXUS_SecurityAlgorithm_eAsa;
		NexusConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eXpt;
		NexusConfig.terminationMode = NEXUS_SecurityTerminationMode_eClear;
		NexusConfig.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
		NexusConfig.ivMode = NEXUS_SecurityIVMode_eRegular;
		NexusConfig.dvbScramLevel = NEXUS_SecurityDvbScrambleLevel_eTs;
		CW.keySize = 16;
		break;
		
	case CSD_Algorithm_DVB_CSA2: /* 0x0000 */
		NexusConfig.algorithm = NEXUS_SecurityAlgorithm_eDvb;
		NexusConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eXpt;
		NexusConfig.terminationMode = NEXUS_SecurityTerminationMode_eClear;
		NexusConfig.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
		NexusConfig.ivMode = NEXUS_SecurityIVMode_eRegular;
		NexusConfig.dvbScramLevel = NEXUS_SecurityDvbScrambleLevel_eTs;
		CW.keySize = 8;
		break;
	case CSD_Algorithm_DVB_CSA3: /* 0x0001 */
		NexusConfig.algorithm = NEXUS_SecurityAlgorithm_eDvbCsa3;
		NexusConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eXpt;
		NexusConfig.terminationMode = NEXUS_SecurityTerminationMode_eClear;
		NexusConfig.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
		NexusConfig.ivMode = NEXUS_SecurityIVMode_eRegular;
		NexusConfig.dvbScramLevel = NEXUS_SecurityDvbScrambleLevel_eTs;
		CW.keySize = 16;
		break;            
	case CSD_Algorithm_AES_128_CBC_IV_0_SCTE52: /* 0x0020 */
		NexusConfig.algorithm = NEXUS_SecurityAlgorithm_eAes;
		NexusConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;
		NexusConfig.terminationMode = NEXUS_SecurityTerminationMode_eCbcResidual;
		NexusConfig.solitarySelect = NEXUS_SecuritySolitarySelect_eCbcXorIV; /* was Clear */
		NexusConfig.ivMode = NEXUS_SecurityIVMode_eRegular;
		CW.keySize = 16;
		if(bConfigureSCR==true) 
		{
			pxKeyPathHandle->scrHandle->ivSize = 16;
		}
			
		break;
	case CSD_Algorithm_RAW_AES_128_ECB_CLEAR: /* 0x4021 */
		NexusConfig.algorithm = NEXUS_SecurityAlgorithm_eAes;
		NexusConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
		NexusConfig.terminationMode = NEXUS_SecurityTerminationMode_eClear;
		NexusConfig.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
		NexusConfig.ivMode = NEXUS_SecurityIVMode_eRegular;
		if(bConfigureSCR==true) 
		{
			pxKeyPathHandle->scrHandle->dataFormat = NEXUS_DmaDataFormat_eBlock;
		}
		CW.keySize = 16;
		break;

	case CSD_Algorithm_AES_128_ECB_CLEAR: /* 0x0021 */
	case CSD_Algorithm_M2M_AES_128_ECB_CLEAR: /* 0x4025 */
		NexusConfig.algorithm = NEXUS_SecurityAlgorithm_eAes;
		NexusConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
		NexusConfig.terminationMode = NEXUS_SecurityTerminationMode_eClear;
		NexusConfig.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
		NexusConfig.ivMode = NEXUS_SecurityIVMode_eRegular;
		CW.keySize = 16;
			
		break;
	case CSD_Algorithm_RAW_AES_128_CBC_IV_0_CLEAR: /* 0x4020 */
		NexusConfig.algorithm = NEXUS_SecurityAlgorithm_eAes;
		NexusConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;
		NexusConfig.terminationMode = NEXUS_SecurityTerminationMode_eClear;
		NexusConfig.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
		NexusConfig.ivMode = NEXUS_SecurityIVMode_eRegular;
		if(bConfigureSCR==true) 
		{
			pxKeyPathHandle->scrHandle->dataFormat = NEXUS_DmaDataFormat_eBlock;
			pxKeyPathHandle->scrHandle->ivSize = 16;
		}
		CW.keySize = 16;
			
		break;
	case CSD_Algorithm_AES_128_CBC_CLEAR: /* 0x0022 */
	case CSD_Algorithm_M2M_AES_128_CBC_CLEAR: /* 0x4026 */
		NexusConfig.algorithm = NEXUS_SecurityAlgorithm_eAes;
		NexusConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;
		NexusConfig.terminationMode = NEXUS_SecurityTerminationMode_eClear;
		NexusConfig.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
		NexusConfig.ivMode = NEXUS_SecurityIVMode_eRegular;
		CW.keySize = 16;
		if(bConfigureSCR==true) 
		{
			pxKeyPathHandle->scrHandle->ivSize = 16;
		}
		break;
	case CSD_Algorithm_TDES_ABA_CBC_IV_0_SCTE52: /* 0x0040 */
		NexusConfig.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;
		NexusConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;
		NexusConfig.terminationMode = NEXUS_SecurityTerminationMode_eCbcResidual;
		NexusConfig.solitarySelect = NEXUS_SecuritySolitarySelect_eClear; /* CFV this is not as specified */
		NexusConfig.ivMode = NEXUS_SecurityIVMode_eRegular;
		CW.keySize = 16;
		if(bConfigureSCR==true) 
		{
			pxKeyPathHandle->scrHandle->ivSize = 8;
		}
		break;
	case CSD_Algorithm_RAW_TDES_ABA_CBC_IV_0_CLEAR: /* 0x4040 */
	case CSD_Algorithm_RAW_TDES_ABA_CBC_CLEAR: /* 0x4043 */
		NexusConfig.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;
		NexusConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eCbc;
		NexusConfig.terminationMode = NEXUS_SecurityTerminationMode_eClear;
		NexusConfig.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
		NexusConfig.ivMode = NEXUS_SecurityIVMode_eRegular;
		CW.keySize = 16;
		if(bConfigureSCR==true) 
		{
			pxKeyPathHandle->scrHandle->dataFormat = NEXUS_DmaDataFormat_eBlock;
			pxKeyPathHandle->scrHandle->ivSize = 8;
		}
		break;
	case CSD_Algorithm_TDES_ABA_ECB_CLEAR: /* 0x0041 */
		NexusConfig.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;
		NexusConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
		NexusConfig.terminationMode = NEXUS_SecurityTerminationMode_eClear;
		NexusConfig.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
		NexusConfig.ivMode = NEXUS_SecurityIVMode_eRegular;
		CW.keySize = 16;
			
		break;
	case CSD_Algorithm_RAW_TDES_ABA_ECB_CLEAR: /* 0x4041 */
	case CSD_Algorithm_M2M_TDES_ABA_ECB_CLEAR: /* 0x4042 */
		NexusConfig.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;
		NexusConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
		NexusConfig.terminationMode = NEXUS_SecurityTerminationMode_eClear;
		NexusConfig.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
		NexusConfig.ivMode = NEXUS_SecurityIVMode_eRegular;
		CW.keySize = 16;
		if(bConfigureSCR==true) 
		{
			pxKeyPathHandle->scrHandle->dataFormat = NEXUS_DmaDataFormat_eBlock;
		}
		break;
	default:
		syslog(LOG_ERR, "Unknown EMI: %04x\n", xEmi);
		status = CERT_ERROR_BAD_EMI;	
		goto CERT_ENC_P_DONE;
	}

	NexusConfig.caVendorID=0x97ed;
	NexusConfig.otpId = NEXUS_SecurityOtpId_eOtpVal;
	NexusConfig.testKey2Select = false;
	NexusConfig.key2Select = NEXUS_SecurityKey2Select_eFixedKey;
	
	errCode = NEXUS_Security_ConfigAlgorithm(keyHandle, &NexusConfig);

	if (errCode != 0) 
	{
		syslog(LOG_ERR, "NEXUS_Security_ConfigAlgorithm(odd) errCode: %x\n", errCode );
		status = CERT_ERROR_BAD_USAGE;
		goto CERT_ENC_P_DONE;
	}

	if ( xKeyParity==CERT_KEY_PARITY_ODD)		
		CW.keyEntryType = NEXUS_SecurityKeyType_eOdd; 
	else if(xKeyParity == CERT_KEY_PARITY_EVEN)
		CW.keyEntryType = NEXUS_SecurityKeyType_eEven; 
	else
		CW.keyEntryType = NEXUS_SecurityKeyType_eOddAndEven; 

	CW.rootKeySwap = false;
	CW.custSubMode = NEXUS_SecurityCustomerSubMode_eReserved21;
	CW.bASKMMode = true;
	CW.keyLadderSelect = NEXUS_SecurityKeyLadderSelect_ePKL;
	CW.bkeyGenBlocked = true;
	CW.keyGenCmdID = NEXUS_SecurityKeyGenCmdID_eKeyGen;
	CW.keyladderID = NEXUS_SecurityKeyladderID_eA; 
	CW.keyGenCmdID   = NEXUS_SecurityKeyGenCmdID_eKeyGen; 
	CW.keyladderType = NEXUS_SecurityKeyladderType_e3Des; 
	CW.rootKeySrc = NEXUS_SecurityRootKeySrc_eOtpKeyA; 
/*	CW.keyEntryType = NEXUS_SecurityKeyType_eOdd; */
	CW.keyDestIVType = NEXUS_SecurityKeyIVType_eNoIV;
	CW.swizzleType = NEXUS_SecuritySwizzleType_eNone; 
	CW.bASKMMode = true;
	CW.keyMode = NEXUS_SecurityKeyMode_eRegular;
	CW.operation = NEXUS_SecurityOperation_eDecrypt; /* CFV Decrypt in certUseEncryptionKey ??? */

	CW.bSwapAESKey = false;
	CW.bRouteKey = true;
	CW.keySize = 16;
	CW.virtualKeyLadderID = 0x0;
	CW.sc01Polarity[NEXUS_SecurityPacketType_eGlobal] = 0x0;
	CW.sc01Polarity[NEXUS_SecurityPacketType_eRestricted] = 0x0;
	
	memset(CW.keyData, 0, sizeof(unsigned char) * 16 );

	errCode = NEXUS_Security_GenerateControlWord(keyHandle , &CW);
	
	if (errCode != 0) 
	{
		syslog(LOG_ERR, "NEXUS_Security_GenerateControlWord errCode: 0x%x\n", errCode );
		status = CERT_ERROR_BAD_USAGE;
		goto CERT_ENC_P_DONE;
	}			

	status = CERT_NO_ERROR;
	
CERT_ENC_P_DONE: 	

	return status;
	
}
#if 0 /* HUMAX_PLATFORM_BASE */ /* Call from Application, current not needed */
TCertFunctionTable* certGetFunctionTable(
	void
	)
{
	certTable.certApiVersion = CERTAPI_VERSION_INT;
	certTable.certExchange = &certExchange;
	certTable.certLock = &certLock;
	certTable.certUnlock = &certUnlock;	
	certTable.certUseDecryptionKey = &certUseEncryptionKey;
	certTable.certUseEncryptionKey = &certUseDecryptionKey;	

	return &certTable;
}
#endif
