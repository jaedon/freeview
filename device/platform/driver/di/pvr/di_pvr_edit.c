/**
 * di_pvr_edit.c
*/

/**
 * @defgroup		DI_PVR DI_PVR_EDIT : pvr edit module
 * @author		Humax
 * @note			DI_PVR_EDIT APIs
 * @brief			Define DI_PVR_EDIT APIs
 * @file 			di_pvr_edit.c
*/

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "linden_trace.h"
#include "drv_pvr.h"
#include "vkernel.h"
#include "taskdef.h"

/*private header files*/
#include "p_rec.h"
/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#define DEBUG_MODULE	TRACE_MODULE_DI_PVR

#define UNUSED(x) (void)(x)

#define PVR_EDIT_TASK_PRIORITY 				USER_PRIORITY8
#define PVR_EDIT_TASK_STACK_SIZE			NORMAL_TASK_STACK_SIZE
#define	PVR_EDIT_EVENT_MSG_QUEUE_SIZE		10
#define PVR_EDIT_INDEX_UNIT_MAX_SIZE		64

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
typedef enum {
	P_PVR_EDIT_MSG_START = 0,
	P_PVR_EDIT_MSG_MAX,
} P_PVR_EDIT_MSG_TYPE;


/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/


/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
static HLONG s_ulPvrEditSemId = 0;
static HLONG s_ulPvrEditMsgId = 0;
static HLONG s_ulPvrEditTaskId = 0;

static DI_PVR_EDIT_SETUP_t s_pstPvrEditSetup;
static DI_PVR_Event_Callback s_pvrEditEvtCbFunc = NULL;


/*******************************************************************/
/*********************** Function Prototypes ***********************/
/*******************************************************************/
static DRV_Error P_PVR_EDIT_CreateIndexFile(DI_PVR_EDIT_SETUP_t *pstEditSetup)
{
	DRV_Error drvErr = 0;
	int err = 0;

	FILE *srcFile = NULL;
	FILE *destFile = NULL;

	char *srcIndexFileName = NULL;
	char *destIndexFileName = NULL;

	HULONG srcFileNum = 0;
	HULONG	ulIndexEntrySize = 0;
	HUINT64 ullStartFileOffset = 0;
	HUINT64 ullEndFileOffset = 0;

	HUINT64 ullTsSize = 0;
	HUINT64 ullIndexSize = 0;
	HUINT64 ullIndexCount = 0;
	HULONG ulIndexRemainSize = 0;

	HUINT8	ucIndexBuffer[PVR_EDIT_INDEX_UNIT_MAX_SIZE];
	HULONG	ulRetSize = 0;
	HULONG	ulIsTimestampEnabled = FALSE;
	HLONG   slNavEntrySize = 0;
	HUINT64 i = 0;
	HUINT64 j = 0;

	HUINT64 ullCurrentFrameOffset = 0;
	HUINT64 ullLastFrameOffset = 0;
	HUINT64 ullNewFrameOffset = 0;

	HULONG	ulDiffTimestamp = 0;
	HULONG	ulPrevTimestamp = 0;
	HULONG	ulCurrentTimestamp = 0;
	HULONG	ulNewTimestamp = 0;

	err = VK_SEM_Get(s_ulPvrEditSemId);
	if(err != VK_OK)
	{
		PrintError("[P_PVR_EDIT_CreateIndexFile] Error in VK_SEM_Get()\n\r");
		return DI_ERR_ERROR;
	}

	srcFileNum = pstEditSetup->ulSrcFileNum;
	if(srcFileNum == 0)
	{
		PrintError("[P_PVR_EDIT_CreateIndexFile] Invalid srcFileNum(%d)!\n\r", srcFileNum);
		goto __QUIT_ERR;
	}

	srcIndexFileName = DD_MEM_Alloc(DI_PVR_MAX_NAME_LEN);
	if(srcIndexFileName == NULL)
	{
		PrintError("[P_PVR_EDIT_CreateIndexFile] Error in DD_MEM_Alloc()\n\r");
		goto __QUIT_ERR;
	}

	destIndexFileName = DD_MEM_Alloc(DI_PVR_MAX_NAME_LEN);
	if(destIndexFileName == NULL)
	{
		PrintError("[P_PVR_EDIT_CreateIndexFile] Error in DD_MEM_Alloc()\n\r");
		goto __QUIT_ERR;
	}

	VK_memset((void *)destIndexFileName, 0, DI_PVR_MAX_NAME_LEN);
	VK_memcpy((void *)destIndexFileName, (void *)(pstEditSetup->aucDestFileName), VK_strlen((char *)pstEditSetup->aucDestFileName)+1 ); //include terminating 0
	VK_strncat(destIndexFileName, ".nts\0", 4); // Add fileExtension

	destFile = fopen(destIndexFileName, "wb");
	if(destFile == NULL)
	{
		PrintError("[P_PVR_EDIT_CreateIndexFile] Error Can't open file [%s]\n\r", destIndexFileName);
		goto __QUIT_ERR;
	}

	for(i=0; i<srcFileNum; i++)
	{
		VK_memset((void *)srcIndexFileName, 0, DI_PVR_MAX_NAME_LEN);
		VK_memcpy((void *)srcIndexFileName, (void *)(pstEditSetup->stSrcFileInfo[i].aucSrcFileName), VK_strlen((char *)pstEditSetup->stSrcFileInfo[i].aucSrcFileName)+1 ); //include terminating 0
		VK_strncat(srcIndexFileName, ".nts\0", 4); // Add	fileExtension

		srcFile = fopen(srcIndexFileName, "rb");
		if(srcFile == NULL)
		{
			PrintError("[P_PVR_EDIT_CreateIndexFile] Error Can't open file [%s]\n\r", srcIndexFileName);
			goto __QUIT_ERR;
		}

		ulRetSize = fread(ucIndexBuffer, 1, PVR_EDIT_INDEX_UNIT_MAX_SIZE, srcFile);
		if(ulRetSize != PVR_EDIT_INDEX_UNIT_MAX_SIZE)
		{
			PrintError("[P_PVR_EDIT_CreateIndexFile] Error fread ulRetSize(%ld)\n\r", ulRetSize);
			goto __QUIT_ERR;
		}

		drvErr = NEXUS_File_GetIndexEntrySize(ucIndexBuffer, &ulIndexEntrySize);
		if(drvErr != DRV_OK)
		{
			PrintError("[P_PVR_EDIT_CreateIndexFile] Error NEXUS_File_GetIndexEntrySize drvErr(%d)\n\r", drvErr);
			goto __QUIT_ERR;
		}

#if defined(CONFIG_LOCAL_TIMESTAMP_ENABLED_RECORD)
		ulIsTimestampEnabled = TRUE;
#endif
		fseek(srcFile, 0, SEEK_SET);

		while(!feof(srcFile))
		{
			ulRetSize = fread(ucIndexBuffer, 1, ulIndexEntrySize, srcFile);
			if(ulRetSize != ulIndexEntrySize)
			{
				PrintError("[P_PVR_EDIT_CreateIndexFile] Error fread ulRetSize(%ld)\n\r", ulRetSize);
				goto __QUIT_ERR;
			}

			drvErr = NEXUS_File_FindIndexOffset(ucIndexBuffer, pstEditSetup->stSrcFileInfo[i].ullStartOffset, ulIsTimestampEnabled);
			if(drvErr == DRV_OK)
			{
				slNavEntrySize =- ulIndexEntrySize;
				fseek(srcFile, slNavEntrySize, SEEK_CUR);
				ullStartFileOffset = ftell(srcFile);
				break;
			}
		}

		while(!feof(srcFile))
		{
			ulRetSize = fread(ucIndexBuffer, 1, ulIndexEntrySize, srcFile);
			if(ulRetSize != ulIndexEntrySize)
			{
				PrintError("[P_PVR_EDIT_CreateIndexFile] Error fread ulRetSize(%ld)\n\r", ulRetSize);
				goto __QUIT_ERR;
			}

			drvErr = NEXUS_File_FindIndexOffset(ucIndexBuffer, pstEditSetup->stSrcFileInfo[i].ullEndOffset, ulIsTimestampEnabled);
			if(drvErr == DRV_OK)
			{
				slNavEntrySize =- ulIndexEntrySize;
				fseek(srcFile, slNavEntrySize, SEEK_CUR);
				ullEndFileOffset = ftell(srcFile);
				break;
			}
		}

		ullTsSize = (pstEditSetup->stSrcFileInfo[i].ullEndOffset - pstEditSetup->stSrcFileInfo[i].ullStartOffset);
		ullIndexSize = (ullEndFileOffset-ullStartFileOffset);
		ullIndexCount = (ullIndexSize/ulIndexEntrySize);
		ulIndexRemainSize = (ullIndexSize%ulIndexEntrySize);
		if(ulIndexRemainSize !=0 )
		{
			PrintError("[P_PVR_EDIT_CreateIndexFile] Error we can't have ulIndexRemainSize(%d)\n\r", ulIndexRemainSize);
			goto __QUIT_ERR;
		}

		fseek(srcFile, ullStartFileOffset, SEEK_SET);

		for(j=0; j<ullIndexCount; j++)
		{
			ulRetSize = fread(ucIndexBuffer, 1, ulIndexEntrySize, srcFile);
			if(ulRetSize != ulIndexEntrySize)
			{
				PrintError("[P_PVR_EDIT_CreateIndexFile] fread Error!!\n\r");
				goto __QUIT_ERR;
			}

			NEXUS_File_GetFrameOffset(ucIndexBuffer, &ullCurrentFrameOffset);
			ullNewFrameOffset = (ullCurrentFrameOffset-pstEditSetup->stSrcFileInfo[i].ullStartOffset+ullLastFrameOffset);
			NEXUS_File_SetFrameOffset(ucIndexBuffer, ullNewFrameOffset);

			NEXUS_File_GetTimestamp(ucIndexBuffer, &ulCurrentTimestamp);
			if(j > 0)
			{
				ulDiffTimestamp = (ulCurrentTimestamp-ulPrevTimestamp);
			}
			ulNewTimestamp = (ulNewTimestamp+ulDiffTimestamp);
			ulPrevTimestamp = ulCurrentTimestamp;
			NEXUS_File_SetTimestamp(ucIndexBuffer, ulNewTimestamp);

			ulRetSize = fwrite(ucIndexBuffer, 1, ulIndexEntrySize, destFile);
			if(ulRetSize != ulIndexEntrySize)
			{
				PrintError("[P_PVR_EDIT_CreateIndexFile] fwrite Error!!\n\r");
				goto __QUIT_ERR;
			}
		}

		ullLastFrameOffset += ullTsSize;

		fclose(srcFile);
	}

	if(destFile != NULL) fclose(destFile);
	if(srcIndexFileName != NULL) DD_MEM_Free(srcIndexFileName);
	if(destIndexFileName != NULL) DD_MEM_Free(destIndexFileName);

	err = VK_SEM_Release(s_ulPvrEditSemId);
	if(err != VK_OK)
	{
		PrintError("[P_PVR_EDIT_CreateIndexFile] Error in VK_SEM_Release()\n\r");
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;


__QUIT_ERR:

	if(srcFile != NULL) fclose(srcFile);
	if(destFile != NULL) fclose(destFile);
	if(srcIndexFileName != NULL) DD_MEM_Free(srcIndexFileName);
	if(destIndexFileName != NULL) DD_MEM_Free(destIndexFileName);
	err = VK_SEM_Release(s_ulPvrEditSemId);
	if(err != VK_OK)
	{
		PrintError("[P_PVR_EDIT_CreateIndexFile] Error in VK_SEM_Release()\n\r");
		return DI_ERR_ERROR;
	}

	return DI_ERR_ERROR;
}


static void P_PVR_EDIT_EventTask(void *param)
{
	int err = 0;
	DRV_Error drvErr = 0;
	P_PVR_EDIT_MSG_TYPE rcvMsg;
	DI_PVR_MSG_t stCallbackMsg;

	UNUSED(param);

	while(1)
	{
		err = VK_MSG_Receive(s_ulPvrEditMsgId, &rcvMsg, sizeof(P_PVR_EDIT_MSG_TYPE));
		if(err == VK_OK)
		{
			switch(rcvMsg)
			{
				case P_PVR_EDIT_MSG_START:
					drvErr = P_PVR_EDIT_CreateIndexFile(&s_pstPvrEditSetup);
					if(drvErr != DRV_OK)
					{
						PrintError("[P_PVR_EDIT_EventTask] Error in P_PVR_EDIT_CreateIndexFile\n\r");
						stCallbackMsg.ulParam1= 200; /* edit fail */
					}
					else
					{
						stCallbackMsg.ulParam1= 100; /* finished */
					}
					if(s_pvrEditEvtCbFunc != NULL)
					{
						stCallbackMsg.ulSender = DI_PVR_EDITOR;
						stCallbackMsg.ulInstance = 0;
						stCallbackMsg.ulEvent = DI_PVR_EVENT_EDIT_STATUS_CHANGED;

						stCallbackMsg.ulParam2 = 0;
						stCallbackMsg.ulParam3 = 0;
						s_pvrEditEvtCbFunc((void *)&stCallbackMsg);
					}
					VK_MEM_Free(s_pstPvrEditSetup.stSrcFileInfo);
					break;
				default:
					PrintError("[P_PVR_EDIT_EventTask] Error Not Support Msg(%d)\n\r", rcvMsg);
					break;
			}
		}
	}
}

DRV_Error DRV_PVR_EDIT_Init(void)
{
	int err = 0;

	PrintEnter("DRV_PVR_EDIT_Init");

	err = VK_SEM_Create(&s_ulPvrEditSemId, "pvrEditSem", VK_SUSPENDTYPE_FIFO);
	if( err != VK_OK )
	{
		PrintError("[DRV_PVR_EDIT_Init] Error(0x%x) in VK_SEM_Create()\n\r", err);
		return DRV_ERR;
	}

	err = VK_MSG_Create(PVR_EDIT_EVENT_MSG_QUEUE_SIZE,
						sizeof(P_PVR_EDIT_MSG_TYPE),
						"PvrEditMsg",
						&s_ulPvrEditMsgId,
						VK_SUSPENDTYPE_FIFO);
	if(err != VK_OK)
	{
		VK_SEM_Destroy(s_ulPvrEditSemId);
		PrintError("[DRV_PVR_EDIT_Init] Error(0x%x) in VK_MSG_Create()\n\r", err);
		return DRV_ERR;
	}

	err = VK_TASK_Create(P_PVR_EDIT_EventTask,
						PVR_EDIT_TASK_PRIORITY,
						PVR_EDIT_TASK_STACK_SIZE,
						"PvrEditTask",
						NULL,
						&s_ulPvrEditTaskId,
						false);
	if(err != VK_OK)
	{
		VK_MSG_Destroy(s_ulPvrEditMsgId);
		VK_SEM_Destroy(s_ulPvrEditSemId);
		PrintError("[DRV_PVR_EDIT_Init] Error(0x%x) in VK_TASK_Create()\n\r", err);
		return DRV_ERR;
	}

	VK_TASK_Start(s_ulPvrEditTaskId);

	PrintExit("DRV_PVR_EDIT_Init");

	return DRV_OK;
}

DI_ERR_CODE DI_PVR_EDIT_CreateIndexFile(HUINT32 ulDeviceId, DI_PVR_EDIT_SETUP_t *pstEditSetup)
{
	HUINT32 i;
	int err;
	P_PVR_EDIT_MSG_TYPE sendMsg;

	UNUSED(ulDeviceId);

	s_pstPvrEditSetup.ulSrcFileNum = pstEditSetup->ulSrcFileNum;
	VK_memcpy((void *)s_pstPvrEditSetup.aucDestFileName, (void *)(pstEditSetup->aucDestFileName), DI_PVR_MAX_NAME_LEN);
	s_pstPvrEditSetup.stSrcFileInfo = (DI_PVR_EDIT_FILE_t *)VK_MEM_Alloc(sizeof(DI_PVR_EDIT_FILE_t)*s_pstPvrEditSetup.ulSrcFileNum);
	if(s_pstPvrEditSetup.stSrcFileInfo == NULL)
	{
		PrintError("[DI_PVR_EDIT_CreateIndexFile] Error in VK_MEM_Alloc()\n\r");
		return DI_ERR_ERROR;
	}

	for(i=0; i<s_pstPvrEditSetup.ulSrcFileNum; i++)
	{
		VK_memcpy((void *)s_pstPvrEditSetup.stSrcFileInfo[i].aucSrcFileName, (void *)(pstEditSetup->stSrcFileInfo[i].aucSrcFileName), DI_PVR_MAX_NAME_LEN);
		s_pstPvrEditSetup.stSrcFileInfo[i].ullStartOffset = pstEditSetup->stSrcFileInfo[i].ullStartOffset;
		s_pstPvrEditSetup.stSrcFileInfo[i].ullEndOffset = pstEditSetup->stSrcFileInfo[i].ullEndOffset;
	}

	sendMsg = P_PVR_EDIT_MSG_START;
	err = VK_MSG_SendTimeout(s_ulPvrEditMsgId, &sendMsg, sizeof(P_PVR_EDIT_MSG_TYPE), 0);
	if(err != VK_OK)
	{
		PrintError("[DI_PVR_EDIT_CreateIndexFile] Error(0x%x) in VK_MSG_SendTimeout()\n\r", err);
		return DI_ERR_ERROR;
	}

	return DI_ERR_OK;
}

DI_ERR_CODE DI_PVR_EDIT_RegisterEventCallback(HUINT32 ulDeviceId, DI_PVR_EVENT_t eEvent, DI_PVR_Event_Callback pCallbackFunc)
{
	UNUSED(ulDeviceId);

	switch(eEvent)
	{
		case DI_PVR_EVENT_EDIT_STATUS_CHANGED:
			s_pvrEditEvtCbFunc = pCallbackFunc;
			break;
		default:
			PrintError("[DI_PVR_EDIT_RegisterEventCallback] Error Not Support Event(%d)\n\r", eEvent);
			return DI_ERR_ERROR;
			break;
	}

	return DI_ERR_OK;
}

DI_ERR_CODE DRV_PVR_EDIT_CreateTsFile(DI_PVR_EDIT_SETUP_t *pstEditFile)
{
	char *srcFileName = NULL;
	char *destFileName = NULL;
	int 	srcFile = 0;
	int 	destFile = 0;
	HUINT64 ullSize = 0;
	HUINT64 ullRetSize = 0;
	HUINT8 	*ucChunkBuffer = NULL;
	HUINT32	ulChunkSize = 0;
	HUINT32	ulChunkRemainSize = 0;
	HUINT64	ullChunkCount = 0;
	HUINT32	ulRetSize = 0;
	HUINT64 i = 0;
	HUINT64 j = 0;

	destFileName = DD_MEM_Alloc(DI_PVR_MAX_NAME_LEN);
	if(destFileName == NULL)
	{
		PrintError("[DRV_PVR_EDIT_CreateTsFile] Error in DD_MEM_Alloc()\n\r");
		goto __QUIT_ERR;
	}

	VK_memset((void *)destFileName, 0, DI_PVR_MAX_NAME_LEN);
	VK_memcpy((void *)destFileName, (void *)(pstEditFile->aucDestFileName), VK_strlen((char *)pstEditFile->aucDestFileName)+1 ); //include terminating 0
#if defined(CONFIG_USE_PRIVATE_FILE_EXTENTION_HTS)
	VK_strncat(destFileName, ".hts\0", 4);
#else
	VK_strncat(destFileName, ".ts\0", 3); // Add  fileExtension
#endif

	destFile = open(destFileName, O_CREAT|O_TRUNC|O_WRONLY|O_SYNC|O_LARGEFILE, 0666);
	if(destFile == -1)
	{
		PrintError("[DRV_PVR_EDIT_CreateTsFile] Invalid destFileName = %s\n\r", destFileName);
		goto __QUIT_ERR;
	}

	srcFileName = DD_MEM_Alloc(DI_PVR_MAX_NAME_LEN);
	if(srcFileName == NULL)
	{
		PrintError("[DRV_PVR_EDIT_CreateTsFile] Error in DD_MEM_Alloc()\n\r");
		goto __QUIT_ERR;
	}

#if defined(CONFIG_LOCAL_TIMESTAMP_ENABLED_RECORD)
	ulChunkSize = (192*1024);
#else
	ulChunkSize = (188*1024);
#endif
	ucChunkBuffer = (HUINT8 *)VK_MEM_Alloc(ulChunkSize);
	if(ucChunkBuffer == NULL)
	{
		PrintError("[DRV_PVR_EDIT_CreateTsFile] VK_MEM_Alloc Error!!\n\r");
		goto __QUIT_ERR;
	}

	for(i=0; i<pstEditFile->ulSrcFileNum; i++)
	{
		VK_memset((void *)srcFileName, 0, DI_PVR_MAX_NAME_LEN);
		VK_memcpy((void *)srcFileName, (void *)(pstEditFile->stSrcFileInfo[i].aucSrcFileName), VK_strlen((char *)pstEditFile->stSrcFileInfo[i].aucSrcFileName)+1 ); //include terminating 0
#if defined(CONFIG_USE_PRIVATE_FILE_EXTENTION_HTS)
		VK_strncat(srcFileName, ".hts\0", 4);
#else
		VK_strncat(srcFileName, ".ts\0", 3); // Add  fileExtension
#endif

		srcFile = open(destFileName, O_RDONLY|O_LARGEFILE);
		if(srcFile == -1)
		{
			PrintError("[DRV_PVR_EDIT_CreateTsFile] Invalid srcFileName = %s\n\r", srcFileName);
			goto __QUIT_ERR;
		}

		ullSize = lseek(srcFile, 0, SEEK_END);

		if((pstEditFile->stSrcFileInfo[i].ullStartOffset+pstEditFile->stSrcFileInfo[i].ullEndOffset) > ullSize)
		{
			PrintError("[DRV_PVR_EDIT_CreateTsFile] Invalid ullStartOffset(0x%llX) or ullEndOffset(0x%llX)\n\r", pstEditFile->stSrcFileInfo[i].ullStartOffset, pstEditFile->stSrcFileInfo[i].ullEndOffset);
			goto __QUIT_ERR;
		}

		ullSize = (pstEditFile->stSrcFileInfo[i].ullEndOffset-pstEditFile->stSrcFileInfo[i].ullStartOffset);

		ullRetSize = lseek(srcFile, pstEditFile->stSrcFileInfo[i].ullStartOffset, SEEK_SET);
		if(ullRetSize != pstEditFile->stSrcFileInfo[i].ullStartOffset)
		{
			PrintError("[DRV_PVR_EDIT_CreateTsFile] Error, requestSize(0x%llX), returnSize(0x%llX)\n\r", pstEditFile->stSrcFileInfo[i].ullStartOffset, ullRetSize);
			goto __QUIT_ERR;
		}

		ullChunkCount = (ullSize/ulChunkSize);
		ulChunkRemainSize = (ullSize-(ullChunkCount*ulChunkSize));

		for(j=0; j<ullChunkCount; j++)
		{
			ulRetSize = read(srcFile, ucChunkBuffer, ulChunkSize);
			if(ulRetSize != ulChunkSize)
			{
				PrintError("[DRV_PVR_EDIT_CreateTsFile] fread Error!!\n\r");
				goto __QUIT_ERR;
			}

			ulRetSize = write(destFile, ucChunkBuffer, ulChunkSize);
			if(ulRetSize != ulChunkSize)
			{
				PrintError("[DRV_PVR_EDIT_CreateTsFile] fwrite Error!!\n\r");
				goto __QUIT_ERR;
			}
		}

		if(ulChunkRemainSize != 0)
		{
			ulRetSize = read(srcFile, ucChunkBuffer, ulChunkRemainSize);
			if(ulRetSize != ulChunkRemainSize)
			{
				PrintError("[DRV_PVR_EDIT_CreateTsFile] fread Error!!\n\r");
				goto __QUIT_ERR;
			}

			ulRetSize = write(destFile, ucChunkBuffer, ulChunkRemainSize);
			if(ulRetSize != ulChunkRemainSize)
			{
				PrintError("[DRV_PVR_EDIT_CreateTsFile] fwrite Error!!\n\r");
				goto __QUIT_ERR;
			}
		}

		close(srcFile);
	}

	if(destFile != -1) close(destFile);
	if(srcFileName != NULL) VK_MEM_Free(srcFileName);
	if(destFileName != NULL) VK_MEM_Free(destFileName);
	if(ucChunkBuffer != NULL) VK_MEM_Free(ucChunkBuffer);

	return DI_ERR_OK;

__QUIT_ERR:
	if(srcFile != -1) close(srcFile);
	if(destFile != -1) close(destFile);
	if(srcFileName != NULL) VK_MEM_Free(srcFileName);
	if(destFileName != NULL) VK_MEM_Free(destFileName);
	if(ucChunkBuffer != NULL) VK_MEM_Free(ucChunkBuffer);

	return DI_ERR_ERROR;
}

