
/**
	@file	  port_itk_dvr.c														\n
	@brief	  itk file for Dvr porting layer							\n

	Description: ITK에서 Dvr Porting 에 사용되는 기능을 정의한 Body파일	\n
	Module: mheg	/itk								 						\n

	Copyright (c) 2008 HUMAX Co., Ltd.											\n
	All rights reserved.														\n

	[Description]

*/


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>

#include "json_parser.h"

#include "port_itk_main.h"
#include "port_itk_console.h"
#include "port_itk_dvr.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
/* debug JSON syntax */
//#define ITK_DVR_JSON_STR_DEBUG

#define ITK_DVR_CRIDTYPE_SERIES		49
#define ITK_DVR_CRIDTYPE_PROGRAMME	50

#define ITK_DVR_CRIDTYPE_SERIES_STR		"49"
#define ITK_DVR_CRIDTYPE_PROGRAMME_STR	"50"

#define ITK_DVR_JSON_PARSER_DEFAULT_DEPTH	20
#define ITK_DVR_JSON_PARSER_ALLOW_COMMENTS	1
#define ITK_DVR_JSON_PARSER_FLOAT_HANDLE	0

#define ITK_DVR_JSON_FIELD_MAX		255

#define ITK_DVR_STRING_BUFFER_SIZE	(1024*4)
#define ITK_DVR_STRING_BUFFER_NUM	10

/*
	Properties of Australia JSON schema
*/
#define ITK_DVR_JSON_KEY_DESCRIPTION_STR		"description"
#define ITK_DVR_JSON_KEY_CRIDTYPE_STR			"cridtype"
#define ITK_DVR_JSON_KEY_TITLE_STR			"title"
#define ITK_DVR_JSON_KEY_SYNOPSIS_STR			"synopsis"
#define ITK_DVR_JSON_KEY_BOOKING_STR			"booking"
#define ITK_DVR_JSON_KEY_LOCATION_STR		"location"

#define ITK_DVR_JSON_BOOKING_DATA_PREFIX		"\"description\":\"PVR bookingDetails Schema\",\"type\":\"object\",\"properties\":"

#define ITK_DVR_BOOKING_DATA_FILENAME		"booking"
#define ITK_DVR_JSON_EXT_FILENAME				"json"
#define ITK_DVR_CRID_EXT_FILENAME				"crid"

#define	ITK_DIR_BACKUP_PATH					"/var/lib/humaxtv_user"
#define ITK_DVR_TO_FLASH
#if defined(ITK_DVR_TO_FLASH)
#define ITK_DVR_BOOKING_DATA_BASE_PATH		ITK_DIR_BACKUP_PATH "/host_dvr"
#else
#define ITK_DVR_BOOKING_DATA_BASE_PATH		"/mnt/user/host_dvr"
#endif

#define ITK_DVR_FILENAME_MAX					128
#define ITK_DVR_READ_FILE_PER_BYTE			1024

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
/*
	Properties of Australia JSON schema
*/
typedef enum
{
	eMHEG_DVR_KEY_DESCRIPTION,
	eMHEG_DVR_KEY_CRIDTYPE,
	eMHEG_DVR_KEY_TITLE,
	eMHEG_DVR_KEY_SYNOPSIS,
	eMHEG_DVR_KEY_BOOKING,
	eMHEG_DVR_KEY_LOCATION,
	eMHEG_DVR_KEY_UNKNOWN
} Itk_Dvr_Booking_Key_State_t;

typedef enum
{
	eMHEG_CRID_TYPE_SERIES,
	eMHEG_CRID_TYPE_PROGRAMME
} Itk_Dvr_Crid_Type_t;

typedef struct Itk_Dvr_Booking_Info_t
{
	//HUINT16 onId;
	//HUINT16 tsId;
	//HUINT16 svcId;

	//struct Itk_Dvr_Booking_Info_t	*pPrev;
	//struct Itk_Dvr_Booking_Info_t	*pNext;
} Itk_Dvr_Booking_Info_t;

typedef struct Itk_Dvr_Booking_Handle_t
{
	char *pszCRID;
	Itk_Dvr_Crid_Type_t	eCridType;
	char *pszJSON;	/* pointer for raw data of JSON */
	HUINT32	uniqueId;	/* Handle pointer. memory번지 값으로 file생성시 unique id(file name)로 사용된다 */

	//struct Itk_Dvr_Booking_Info_t		stBookingInfo;

	//struct Itk_Dvr_Booking_Info_t	*pHeadBookingInfo;
	//struct Itk_Dvr_Booking_Info_t	*pTailBookingInfo;

	struct Itk_Dvr_Booking_Handle_t	*pPrev;
	struct Itk_Dvr_Booking_Handle_t	*pNext;
} Itk_Dvr_Booking_Handle_t;

typedef struct Itk_Dvr_Str_Buffer_t
{
	HUINT8	ucUsed;
	char		szBuffer[ITK_DVR_STRING_BUFFER_SIZE];
} Itk_Dvr_Str_Buffer_t;

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
static itk_dvr_t	*s_pstDvrObj = NULL;
static itk_dvr_event_callback_t	s_dvr_event_callback = NULL;
static void *s_dvr_event_callback_context = NULL;

static Itk_Dvr_Booking_Handle_t	*pHeadBookingHandle = NULL;
static Itk_Dvr_Booking_Handle_t	*pTailBookingHandle = NULL;

static Itk_Dvr_Str_Buffer_t		s_stStrBuffer[ITK_DVR_STRING_BUFFER_NUM];
static char	s_EmptyBuf[1];
/********************************************************************/
/********************      proto-type functions  ********************/
/********************************************************************/



/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

#define _____local_APIs________________________________________________________________
int mheg_itk_port_dvrFreeStrBuffer(const char *pBuf);

static void mheg_itk_port_dvrBookingAppendHandle(Itk_Dvr_Booking_Handle_t *pItem, Itk_Dvr_Booking_Handle_t **ppHead, Itk_Dvr_Booking_Handle_t **ppTail)
{
	if ((*ppHead) == NULL)
	{
		(*ppHead) = pItem;
		pItem->pPrev = NULL;
	}
	else
	{
		(*ppTail)->pNext = pItem;
		pItem->pPrev = (*ppTail);
	}

	(*ppTail) = pItem;
	pItem->pNext = NULL;

	HxLOG_Debug(C_CODE_F_WHITE_B_BLUE" APPENDED! pItem(%p) pItem prev(%p) pItem next(%p) pHead(%p) pHead next(%p) pTail(%p) pTail prev(%p) pTail next(%p)"C_CODE_RESET"\n",
			pItem, pItem->pPrev, pItem->pNext, *ppHead, (*ppHead)->pNext, (*ppTail), (*ppTail)->pPrev, (*ppTail)->pNext);
}

/*
	호출되어지는곳에 해당 node뿐만 아니라 STRUCT의 buffer pointer를 반드시 free해야 한다
*/
static void mheg_itk_port_dvrBookingRemoveHandle(Itk_Dvr_Booking_Handle_t *pItem, Itk_Dvr_Booking_Handle_t **ppHead, Itk_Dvr_Booking_Handle_t **ppTail)
{
	HxLOG_Debug(C_CODE_F_WHITE_B_RED" REMOVED! CRID(%s) pItem(%p)"C_CODE_RESET"\n", pItem->pszCRID, pItem);

	if (pItem->pPrev == NULL)
	{
		(*ppHead) = pItem->pNext;
	}
	else
	{
		pItem->pPrev->pNext = pItem->pNext;
	}
	if (pItem->pNext == NULL)
	{
		(*ppTail) = pItem->pPrev;
	}
	else
	{
		pItem->pNext->pPrev = pItem->pPrev;
	}
}

static Itk_Dvr_Booking_Handle_t *mheg_itk_port_dvrBookingFindHandle(const char *pCrid, Itk_Dvr_Booking_Handle_t *pHead, Itk_Dvr_Booking_Handle_t *pTail)
{
	Itk_Dvr_Booking_Handle_t *pItem = NULL;

	(void)(pTail);

	pItem = pHead;

	while (pItem != NULL)
	{
		if (strcmp(pItem->pszCRID, pCrid) == 0)
		{
			break;
		}

		pItem = pItem->pNext;
	}

	return pItem;
}

static HBOOL mheg_itk_port_dvrFileErase(const char * file)
{
	return (0 == remove(file)) ? TRUE : FALSE;
}

static HBOOL mheg_itk_port_dvrMkDir(const char * dir)
{
	char *odir, *p;

	odir = p = HLIB_STD_StrDup(dir);
	do
	{
		p = strchr(p + 1, '/');
		if (p)
			*p = '\0';

		if (access(odir, F_OK) == -1)
		{
			if (mkdir(odir, S_IRWXU | S_IRWXG | S_IRWXO) == -1)
			{
				HLIB_STD_MemFree(odir);
				return FALSE;
			}
		}

		if (p)
			*p = '/';
	} while (p);

	HLIB_STD_MemFree(odir);

	return TRUE;
}

static HBOOL mheg_itk_port_dvrIsDir(const char * dir)
{
	struct stat fs;

	return (0 == stat(dir, &fs) && S_ISDIR(fs.st_mode)) ? TRUE : FALSE;
}

/*
	CRID
	JSON
*/
static HBOOL mheg_itk_port_dvrBookingMetaFileRead(char *filename, char *data, size_t *data_size)
{
	int fd, r_size = 0;
	HUINT32 tot_size = 0;

	if ((filename == NULL) || (data == NULL) || (data_size == 0))
	{
		HxLOG_Error(" invalid parameters !\n");
		return FALSE;
	}

	fd = open(filename, O_RDONLY | O_SYNC, S_IRWXU | S_IRWXG | S_IRWXO);
	if( fd < 0 )
	{
		HxLOG_Error(" open error ! filename(%s)\n", filename);
		return FALSE;
	}

	do
	{
		r_size = read(fd, &data[tot_size], ITK_DVR_READ_FILE_PER_BYTE);
		tot_size += r_size;
	} while (r_size >= ITK_DVR_READ_FILE_PER_BYTE);

	*data_size = (size_t)tot_size;

	close(fd);

	HxLOG_Debug(" read data into file(%s) !\n", filename);

	return TRUE;
}

static HBOOL mheg_itk_port_dvrBookingFileRead(void)
{
	int fd, sz = 0, r_size = 0;
	struct stat fs;
	char filename[ITK_DVR_FILENAME_MAX];
	Itk_Dvr_Booking_Handle_t *pBookingHandle, stBookingBuffer;
	char *pCridBuf = NULL, *pJsonBuf = NULL;
	HUINT32 meta_size = 0;
	HBOOL meta_result = FALSE;

	snprintf(filename, ITK_DVR_FILENAME_MAX, "%s/%s", ITK_DVR_BOOKING_DATA_BASE_PATH, ITK_DVR_BOOKING_DATA_FILENAME);

	fd = open(filename, O_RDONLY | O_SYNC, S_IRWXU | S_IRWXG | S_IRWXO);

	if (fd < 0)
	{
		HxLOG_Error(" open error !\n");
		return FALSE;
	}

	if (fstat(fd, &fs) < 0)
	{
		close(fd);
		HxLOG_Error(" fstat error !\n");
		return FALSE;
	}

	if( (fs.st_size == 0)
		|| (fs.st_size < sizeof(Itk_Dvr_Booking_Handle_t)))
	{
		close(fd);
		HxLOG_Error(" invalid fs.st_size !\n");
		return FALSE;
	}

	sz = sizeof(Itk_Dvr_Booking_Handle_t);
	do
	{
		r_size = read(fd, (void*)&stBookingBuffer, sz);
		if (r_size != sz)
		{
			//HxLOG_Debug(" invalid read size !(%d/%d)\n", sz, r_size);
		}
		else
		{
			/* CRID */
			snprintf(filename, ITK_DVR_FILENAME_MAX, "%s/%d.%s", ITK_DVR_BOOKING_DATA_BASE_PATH, stBookingBuffer.uniqueId, ITK_DVR_CRID_EXT_FILENAME);
			/* booking list를 만드는 buffer를 임시로 사용 */
			meta_result = mheg_itk_port_dvrBookingMetaFileRead(filename, s_stStrBuffer[0].szBuffer, &meta_size);
			if (!meta_result)
			{
				continue;
			}
			else
			{
				HxLOG_Debug(" filename(%s) CridSize(%d) CridType(%d)\n", filename, meta_size, stBookingBuffer.eCridType);

				pCridBuf = HLIB_STD_MemAlloc(meta_size + 1);
				if (pCridBuf == NULL)
				{
					HxLOG_Error(" Not enogh memory!!!\n");
					close(fd);
					return FALSE;
				}
				HxSTD_StrNCpy(pCridBuf, s_stStrBuffer[0].szBuffer, meta_size);
				/* make a null terminated string */
				pCridBuf[meta_size] = '\0';
			}

			/* JSON */
			snprintf(filename, ITK_DVR_FILENAME_MAX, "%s/%d.%s", ITK_DVR_BOOKING_DATA_BASE_PATH, stBookingBuffer.uniqueId, ITK_DVR_JSON_EXT_FILENAME);
			/* booking list를 만드는 buffer를 임시로 사용 */
			meta_result = mheg_itk_port_dvrBookingMetaFileRead(filename, s_stStrBuffer[0].szBuffer, &meta_size);
			if (!meta_result)
			{
				if (pCridBuf)
				{
					HLIB_STD_MemFree(pCridBuf);
					pCridBuf = NULL;
				}
				continue;
			}
			else
			{
				pJsonBuf = HLIB_STD_MemAlloc(meta_size + 1);
				if (pJsonBuf == NULL)
				{
					HxLOG_Error(" Not enogh memory!!!\n");
					if (pCridBuf)
						HLIB_STD_MemFree(pCridBuf);

					close(fd);
					return FALSE;
				}
				HxSTD_StrNCpy(pJsonBuf, s_stStrBuffer[0].szBuffer, meta_size);
				/* make a null terminated string */
				pJsonBuf[meta_size] = '\0';
			}

			pBookingHandle = HLIB_STD_MemAlloc(sizeof(Itk_Dvr_Booking_Handle_t));
			if (pBookingHandle == NULL)
			{
				HxLOG_Error(" Not enogh memory!!!\n");
				if (pCridBuf)
					HLIB_STD_MemFree(pCridBuf);
				if (pJsonBuf)
					HLIB_STD_MemFree(pJsonBuf);

				close(fd);
				return FALSE;
			}

			/* Make the CRID buff from HEAP */
			pBookingHandle->pszCRID = pCridBuf;
			/* Make the JSON string buffer form HEAP */
			pBookingHandle->pszJSON = pJsonBuf;
			pBookingHandle->eCridType = stBookingBuffer.eCridType;
			/*
				이건 make booking당시 만들어진것만 사용한다. 그렇지 않으면 uniqueId가 매번 바뀌어서 실제 생성된 filename과 매치가 안된다
			*/
			pBookingHandle->uniqueId = stBookingBuffer.uniqueId;

			// TODO: 실제 host reservation engine과 연동하여 예약 하는 연동 필요

			/* Add the Node to the booking list */
			mheg_itk_port_dvrBookingAppendHandle(pBookingHandle, &pHeadBookingHandle, &pTailBookingHandle);
		}
	}while (r_size >= sz);

	close(fd);

	mheg_itk_port_dvrFreeStrBuffer(s_stStrBuffer[0].szBuffer);

	//HxLOG_Debug(" read data from file(%s) !\n", file);

	return TRUE;
}

/*
	CRID
	JSON
*/
static HBOOL mheg_itk_port_dvrBookingMetaFileWrite(char *filename, char *data, size_t data_size)
{
	int fd;

	if ((filename == NULL) || (data == NULL) || (data_size == 0))
	{
		HxLOG_Error(" invalid parameters !\n");
		return FALSE;
	}

	fd = open(filename, O_WRONLY | O_CREAT | /*O_TRUNC |*/ O_SYNC, S_IRWXU | S_IRWXG | S_IRWXO);
	if( fd < 0 )
	{
		HxLOG_Error(" open error ! filename(%s)\n", filename);
		return FALSE;
	}

	write(fd, data, data_size);
	close(fd);

	HxLOG_Debug(" write data into file(%s) !\n", filename);

	return TRUE;
}

static HBOOL mheg_itk_port_dvrBookingAddToFile(Itk_Dvr_Booking_Handle_t *pItem)
{
	char filename[ITK_DVR_FILENAME_MAX];
	int fd;

	if (pItem == NULL)
	{
		HxLOG_Error(" Booking data is NULL!!!\n");
		return FALSE;
	}

	if (mheg_itk_port_dvrIsDir(ITK_DVR_BOOKING_DATA_BASE_PATH) == FALSE)
	{
		if (mheg_itk_port_dvrMkDir(ITK_DVR_BOOKING_DATA_BASE_PATH) == FALSE)
		{
			HxLOG_Error(" Cannot create a directory\n");
			return FALSE;
		}
	}

	snprintf(filename, ITK_DVR_FILENAME_MAX, "%s/%s", ITK_DVR_BOOKING_DATA_BASE_PATH, ITK_DVR_BOOKING_DATA_FILENAME);

	/* open a file */
	fd = open(filename, O_WRONLY | O_CREAT | /*O_TRUNC |*/ O_SYNC, S_IRWXU | S_IRWXG | S_IRWXO);
	if( fd < 0 )
	{
		HxLOG_Error(" open error ! filename(%s)\n", filename);
		return FALSE;
	}

	/* move file pointer to end */
	if(lseek(fd, 0, SEEK_END) <0)
	{
		close(fd);
		return FALSE;
	}

	write(fd, pItem, sizeof(Itk_Dvr_Booking_Handle_t));

	/* CRID */
	snprintf(filename, ITK_DVR_FILENAME_MAX, "%s/%d.%s", ITK_DVR_BOOKING_DATA_BASE_PATH, pItem->uniqueId, ITK_DVR_CRID_EXT_FILENAME);
	mheg_itk_port_dvrBookingMetaFileWrite(filename, pItem->pszCRID, HxSTD_StrLen(pItem->pszCRID));

	/* JSON */
	snprintf(filename, ITK_DVR_FILENAME_MAX, "%s/%d.%s", ITK_DVR_BOOKING_DATA_BASE_PATH, pItem->uniqueId, ITK_DVR_JSON_EXT_FILENAME);
	mheg_itk_port_dvrBookingMetaFileWrite(filename, pItem->pszJSON, HxSTD_StrLen(pItem->pszJSON));

	close(fd);

	HxLOG_Debug(" write data into file(%s) !\n", filename);

	return TRUE;
}

/*
	cancel booking일때 검색해서 삭제하는것보다 모두를 통째로 쓴다. 추후 optimization필요
*/
static HBOOL mheg_itk_port_dvrBookingWriteAllToFile(HBOOL bWriteMeta)
{
	Itk_Dvr_Booking_Handle_t *pItem = NULL;
	char filename[ITK_DVR_FILENAME_MAX];
	int fd;

	if (pHeadBookingHandle == NULL)
	{
		HxLOG_Error(" There is no booking data\n");
		return FALSE;
	}

	if (mheg_itk_port_dvrIsDir(ITK_DVR_BOOKING_DATA_BASE_PATH) == FALSE)
	{
		if (mheg_itk_port_dvrMkDir(ITK_DVR_BOOKING_DATA_BASE_PATH) == FALSE)
		{
			HxLOG_Error(" Cannot create a directory\n");
			return FALSE;
		}
	}

	snprintf(filename, ITK_DVR_FILENAME_MAX, "%s/%s", ITK_DVR_BOOKING_DATA_BASE_PATH, ITK_DVR_BOOKING_DATA_FILENAME);

	/* delete exist booking file */
	mheg_itk_port_dvrFileErase(filename);

	/* create a file */
	fd = open(filename, O_WRONLY | O_CREAT | /*O_TRUNC |*/ O_SYNC, S_IRWXU | S_IRWXG | S_IRWXO);
	if( fd < 0 )
	{
		HxLOG_Error(" open error ! filename(%s)\n", filename);
		return FALSE;
	}

	pItem = pHeadBookingHandle;
	while (pItem != NULL)
	{
		//lseek(fd, 0, SEEK_SET);
		write(fd, pItem, sizeof(Itk_Dvr_Booking_Handle_t));
		if (bWriteMeta)
		{
			/* CRID */
			snprintf(filename, ITK_DVR_FILENAME_MAX, "%s/%d.%s", ITK_DVR_BOOKING_DATA_BASE_PATH, pItem->uniqueId, ITK_DVR_CRID_EXT_FILENAME);
			mheg_itk_port_dvrBookingMetaFileWrite(filename, pItem->pszCRID, HxSTD_StrLen(pItem->pszCRID));

			/* JSON */
			snprintf(filename, ITK_DVR_FILENAME_MAX, "%s/%d.%s", ITK_DVR_BOOKING_DATA_BASE_PATH, pItem->uniqueId, ITK_DVR_JSON_EXT_FILENAME);
			mheg_itk_port_dvrBookingMetaFileWrite(filename, pItem->pszJSON, HxSTD_StrLen(pItem->pszJSON));
		}

		pItem = pItem->pNext;
	}

	close(fd);

	HxLOG_Debug(" write data into file(%s) !\n", filename);

	return TRUE;
}

static void mheg_itk_port_dvrExecuteEventCallback(itk_dvr_event_t event)
{
	if (s_dvr_event_callback)
	{
		s_dvr_event_callback(s_dvr_event_callback_context, event);
	}
}

static itk_dvr_result_t mheg_itk_port_dvrCancelBookingByBookingId(const char *bookingId)
{
	Itk_Dvr_Booking_Handle_t *pBookingHandle;
	itk_dvr_result_t	cancel_result = ITK_DVR_RESULT_FAILED;
	char filename[ITK_DVR_FILENAME_MAX];

	/*
		Search the bookingId whether already exist or not
	*/
	pBookingHandle = mheg_itk_port_dvrBookingFindHandle(bookingId, pHeadBookingHandle, pTailBookingHandle);
	if (pBookingHandle != NULL)
	{
		HxLOG_Debug(" Found booking handle(%p) uniqueId(%d)!!!\n", pBookingHandle, pBookingHandle->uniqueId);

		/* delete the CRID file */
		snprintf(filename, ITK_DVR_FILENAME_MAX, "%s/%d.%s", ITK_DVR_BOOKING_DATA_BASE_PATH, pBookingHandle->uniqueId, ITK_DVR_CRID_EXT_FILENAME);
		mheg_itk_port_dvrFileErase(filename);
		/* delete the JSON file */
		snprintf(filename, ITK_DVR_FILENAME_MAX, "%s/%d.%s", ITK_DVR_BOOKING_DATA_BASE_PATH, pBookingHandle->uniqueId, ITK_DVR_JSON_EXT_FILENAME);
		mheg_itk_port_dvrFileErase(filename);

		mheg_itk_port_dvrBookingRemoveHandle(pBookingHandle, &pHeadBookingHandle, &pTailBookingHandle);

		/* Free the CRID buffer */
		HLIB_STD_MemFree(pBookingHandle->pszCRID);
		/* Free the JSON buffer */
		HLIB_STD_MemFree(pBookingHandle->pszJSON);
		/* Free the booking node */
		HLIB_STD_MemFree(pBookingHandle);

		/* update booking file */
		mheg_itk_port_dvrBookingWriteAllToFile(FALSE);

		cancel_result =  ITK_DVR_RESULT_REMOVED;
	}
	else
	{
		cancel_result = ITK_DVR_RESULT_NOT_FOUND;
	}

	return cancel_result;
}

extern char **environ;
void mheg_itk_port_dvrReset(void)
{
	Itk_Dvr_Booking_Handle_t *pItem = NULL;
	Itk_Dvr_Booking_Handle_t *pNextItem = NULL;
	char szCmd[ITK_DVR_FILENAME_MAX];
	pid_t pid;
	int status;

	if (pHeadBookingHandle == NULL)
	{
		HxLOG_Debug(" There is no booking data\n");
	}
	else
	{
		pItem = pHeadBookingHandle;
		while (pItem != NULL)
		{
			pNextItem = pItem->pNext;

			mheg_itk_port_dvrBookingRemoveHandle(pItem, &pHeadBookingHandle, &pTailBookingHandle);

			/* Free the CRID buffer */
			HLIB_STD_MemFree(pItem->pszCRID);
			/* Free the JSON buffer */
			HLIB_STD_MemFree(pItem->pszJSON);
			/* Free the booking node */
			HLIB_STD_MemFree(pItem);

			pItem = pNextItem;
		}
	}

	if (mheg_itk_port_dvrIsDir(ITK_DVR_BOOKING_DATA_BASE_PATH) == TRUE)
	{
		snprintf(szCmd, ITK_DVR_FILENAME_MAX, "rm -rf %s/*", ITK_DVR_BOOKING_DATA_BASE_PATH);

		pid = vfork();
		if (pid == 0)
		{
			char *argv[4];
			argv[0] = "sh";
			argv[1] = "-c";
			argv[2] = szCmd;
			argv[3] = 0;
			execve("/bin/sh", argv, environ);

			exit(0);
		}

		do
		{
			if (waitpid(pid, &status, 0) == -1)
			{
				HxLOG_Error("pid == -1, errno = %d\n", errno);
				if (errno != EINTR)
					return;
			}
			else
			{
				return;
			}
		} while(1);
	}
}

Itk_Dvr_Str_Buffer_t *mheg_itk_port_dvrGetAvailableStrBuffer(void)
{
	int i;

	for (i = 0; i < ITK_DVR_STRING_BUFFER_NUM; i++)
	{
		if (s_stStrBuffer[i].ucUsed == 0)
		{
			return &s_stStrBuffer[i];
		}
	}

	return NULL;
}

int mheg_itk_port_dvrFreeStrBuffer(const char *pBuf)
{
	int i;

	for (i = 0; i < ITK_DVR_STRING_BUFFER_NUM; i++)
	{
		if ((HUINT32)pBuf == (HUINT32)s_stStrBuffer[i].szBuffer)
		{
			HxSTD_memset(s_stStrBuffer[i].szBuffer, 0, ITK_DVR_STRING_BUFFER_SIZE);
			s_stStrBuffer[i].ucUsed = 0;

			return 0;
		}
	}

	HxLOG_Error(" Cannot find the string buffer pointer(%p)!!!\n", pBuf);

	return 1;
}

/*
	1. <booking CRID> <CRID type>
	2. space-separated list of CRIDs.
	(ex) "crid://example.com/tv1 49 crid://example.com/tv2 50 crid://example.com/tv3 49"
*/
int mheg_itk_port_dvrBookingMakeList(char *pBuf, int nBufLen)
{
	Itk_Dvr_Booking_Handle_t *pItem = NULL;
	char tempBuf[100];

	pItem = pHeadBookingHandle;

	while (pItem != NULL)
	{
		HxSTD_memset(tempBuf, 0, 100);

		snprintf(tempBuf, 100, "%s %s", pItem->pszCRID, (pItem->eCridType == eMHEG_CRID_TYPE_SERIES) ? ITK_DVR_CRIDTYPE_SERIES_STR : ITK_DVR_CRIDTYPE_PROGRAMME_STR);

		pItem = pItem->pNext;

		/* Insert Space for separation for a next CRID */
		if (pItem != NULL)
		{
			HxSTD_StrNCat(tempBuf, " ", sizeof(tempBuf) - strlen(tempBuf) - 1);
		}

		HxSTD_StrNCat(pBuf, tempBuf, nBufLen - strlen(pBuf) - 1);
	}

	return 1;
}

int mheg_itk_port_dvrBookingMakeDataToJSON(char *pBuf, const char *pJSON)
{
#if	defined(CONFIG_DEBUG)
	if(pBuf)	{	HxLOG_Assert(1);	}
	else		{	HxLOG_Assert(0);	}
	if(pJSON)	{	HxLOG_Assert(1);	}
	else		{	HxLOG_Assert(0);	}
#else
	HxLOG_Assert(pBuf);
	HxLOG_Assert(pJSON);
#endif

	snprintf(pBuf, ITK_DVR_STRING_BUFFER_SIZE, "{%s%s}", ITK_DVR_JSON_BOOKING_DATA_PREFIX, pJSON);

	return 0;
}

#if defined(ITK_DVR_JSON_STR_DEBUG)
static size_t s_Level = 0;
static const char* s_pIndention = "  ";
static int s_IsKey = 0;
static void print_indention()
{
	size_t i;

	for (i = 0; i < s_Level; ++i)
	{
		HxLOG_Debug("%s", s_pIndention);
	}
}

static int mheg_itk_port_dvrDebugJSON(int type, const JSON_value* value)
{
	switch (type)
	{
		case JSON_T_ARRAY_BEGIN:
			if (!s_IsKey) print_indention();
			s_IsKey = 0;
			HxLOG_Debug("[\n");
			++s_Level;
			break;
		case JSON_T_ARRAY_END:
			HxLOG_Assert(!s_IsKey);
			if (s_Level > 0) --s_Level;
			print_indention();
			HxLOG_Debug("]\n");
			break;
		case JSON_T_OBJECT_BEGIN:
			if (!s_IsKey) print_indention();
			s_IsKey = 0;
			HxLOG_Debug("{\n");
			++s_Level;
			break;
		case JSON_T_OBJECT_END:
			HxLOG_Assert(!s_IsKey);
			if (s_Level > 0) --s_Level;
			print_indention();
			HxLOG_Debug("}\n");
			break;
		case JSON_T_INTEGER:
			if (!s_IsKey) print_indention();
			s_IsKey = 0;
			HxLOG_Debug("integer: "JSON_PARSER_INTEGER_SPRINTF_TOKEN"\n", value->vu.integer_value);
			break;
		case JSON_T_FLOAT:
			if (!s_IsKey) print_indention();
			s_IsKey = 0;
			HxLOG_Debug("float: %f\n", value->vu.float_value); /* We wanted stringified floats */
			break;
		case JSON_T_NULL:
			if (!s_IsKey) print_indention();
			s_IsKey = 0;
			HxLOG_Debug("null\n");
			break;
		case JSON_T_TRUE:
			if (!s_IsKey) print_indention();
			s_IsKey = 0;
			HxLOG_Debug("true\n");
			break;
		case JSON_T_FALSE:
			if (!s_IsKey) print_indention();
			s_IsKey = 0;
			HxLOG_Debug("false\n");
			break;
		case JSON_T_KEY:
			s_IsKey = 1;
			print_indention();
			HxLOG_Debug("key = '%s', value = ", value->vu.str.value);
			break;
		case JSON_T_STRING:
			if (!s_IsKey) print_indention();
			s_IsKey = 0;
			HxLOG_Debug("string: '%s'\n", value->vu.str.value);
			break;
		default:
			HxLOG_Assert(0);
			HxLOG_Error(" UNKONWN JSON TYPE!!!\n");
			break;
	}

	return 1;
}
#endif

static Itk_Dvr_Booking_Key_State_t mheg_itk_port_dvrGetJSONKeyState(const char *pszKey)
{
	Itk_Dvr_Booking_Key_State_t eState = eMHEG_DVR_KEY_UNKNOWN;

	if (pszKey == NULL)
	{
		HxLOG_Error(" Key string is NULL!!!\n");
		return eState;
	}

	if (HxSTD_StrCmp(pszKey, ITK_DVR_JSON_KEY_DESCRIPTION_STR) == 0)
	{
		eState = eMHEG_DVR_KEY_DESCRIPTION;
	}
	else if (HxSTD_StrCmp(pszKey, ITK_DVR_JSON_KEY_CRIDTYPE_STR) == 0)
	{
		eState = eMHEG_DVR_KEY_CRIDTYPE;
	}
	else if (HxSTD_StrCmp(pszKey, ITK_DVR_JSON_KEY_TITLE_STR) == 0)
	{
		eState = eMHEG_DVR_KEY_TITLE;
	}
	else if (HxSTD_StrCmp(pszKey, ITK_DVR_JSON_KEY_SYNOPSIS_STR) == 0)
	{
		eState = eMHEG_DVR_KEY_SYNOPSIS;
	}
	else if (HxSTD_StrCmp(pszKey, ITK_DVR_JSON_KEY_BOOKING_STR) == 0)
	{
		eState = eMHEG_DVR_KEY_BOOKING;
	}
	else if (HxSTD_StrCmp(pszKey, ITK_DVR_JSON_KEY_LOCATION_STR) == 0)
	{
		eState = eMHEG_DVR_KEY_LOCATION;
	}

	return eState;
}

static int mheg_itk_port_dvrBookingSetData(Itk_Dvr_Booking_Handle_t *pBookingHandle, Itk_Dvr_Booking_Key_State_t eState, const char *pszStr)
{
	int result = 0;

#if	defined(CONFIG_DEBUG)
	if(pBookingHandle)	{	HxLOG_Assert(1);	}
	else		{	HxLOG_Assert(0);	}
	if(pszStr)	{	HxLOG_Assert(1);	}
	else		{	HxLOG_Assert(0);	}
#else
	HxLOG_Assert(pBookingHandle);
	HxLOG_Assert(pszStr);
#endif

	switch (eState)
	{
		case eMHEG_DVR_KEY_DESCRIPTION:
			break;
		case eMHEG_DVR_KEY_CRIDTYPE:
			if (HxSTD_StrCmp(pszStr, ITK_DVR_CRIDTYPE_SERIES_STR) == 0)
				pBookingHandle->eCridType = eMHEG_CRID_TYPE_SERIES;
			else
				pBookingHandle->eCridType = eMHEG_CRID_TYPE_PROGRAMME;
			break;
		case eMHEG_DVR_KEY_TITLE:
			break;
		case eMHEG_DVR_KEY_SYNOPSIS:
			break;
		case eMHEG_DVR_KEY_BOOKING:
			break;
		case eMHEG_DVR_KEY_LOCATION:
			break;
		default:
			result = 1;
			break;
	}

	return result;
}

static int mheg_itk_port_dvrCallbackJSON(void* ctx, int type, const JSON_value* value)
{
	static Itk_Dvr_Booking_Key_State_t eState = eMHEG_DVR_KEY_UNKNOWN;
	Itk_Dvr_Booking_Handle_t *pBookingHandle = (Itk_Dvr_Booking_Handle_t*)ctx;

#if defined(ITK_DVR_JSON_STR_DEBUG)
	mheg_itk_port_dvrDebugJSON(type, value);
#endif

#if	defined(CONFIG_DEBUG)
		if(pBookingHandle)	{	HxLOG_Assert(1);	}
		else		{	HxLOG_Assert(0);		}
#else
		HxLOG_Assert(pBookingHandle);
#endif
	switch (type)
	{
		case JSON_T_ARRAY_BEGIN:
			break;
		case JSON_T_ARRAY_END:
			break;
		case JSON_T_OBJECT_BEGIN:
			break;
		case JSON_T_OBJECT_END:
			break;
		case JSON_T_INTEGER:
			break;
		case JSON_T_FLOAT:
			break;
		case JSON_T_NULL:
			break;
		case JSON_T_TRUE:
			break;
		case JSON_T_FALSE:
			break;
		case JSON_T_KEY:
			eState = mheg_itk_port_dvrGetJSONKeyState(value->vu.str.value);
			break;
		case JSON_T_STRING:
			mheg_itk_port_dvrBookingSetData(pBookingHandle, eState, value->vu.str.value);
			break;
		default:
			HxLOG_Assert(0);
			HxLOG_Error(" UNKONWN JSON TYPE!!!\n");
			break;
	}

	return 1;
}

static int mheg_itk_port_dvrStartJSONParser(void *cb_context, const char *szJSON)
{
	JSON_config config;
	struct JSON_parser_struct* jc = NULL;
	const char *pJSONptr = szJSON;
	int count = 0, result = 0, next_char = 0;

	if (szJSON == NULL)
	{
		HxLOG_Error(" JSON string is NULL!!\n");
		return 1;
	}

	/* Should be checked what locale is used */

	init_JSON_config(&config);

	config.depth				= ITK_DVR_JSON_PARSER_DEFAULT_DEPTH;
	config.callback			= &mheg_itk_port_dvrCallbackJSON;
	config.callback_ctx			= cb_context;
	config.allow_comments		= ITK_DVR_JSON_PARSER_ALLOW_COMMENTS;
	config.handle_floats_manually = ITK_DVR_JSON_PARSER_FLOAT_HANDLE;

	jc = new_JSON_parser(&config);

	while (*pJSONptr != '\0')
	{
		next_char = ((int)*pJSONptr & 0x000000FF);	/* 우선 ASCII만 고려하자 */
		if (!JSON_parser_char(jc, next_char))
		{
			HxLOG_Error(" JSON_parser_char: syntax error, byte %d\n", count);
			result = 1;
			goto done;
		}

		pJSONptr++;
		count++;
	}

	if (!JSON_parser_done(jc))
	{
		HxLOG_Error("JSON_parser_end: syntax error\n");
		result = 1;
		goto done;
	}

done:

	delete_JSON_parser(jc);

	return result;
}

#define _____JUMP_APIs________________________________________________________________

/**
 * Sets the callback functions that the DVR can use to start or stop
 * the download of a linked carousel.
 * @param thiz The DVR interface.
 * @param start Start download function pointer.
 * @param stop The stop download function pointer.
 * @param context A context pointer to pass in the callbacks.
 */
/**********************************************************/
//1 This function may not be used for Australia profile.

itk_errcode_t mheg_itk_port_dvrSetDownloadCallbacks(struct itk_dvr_t *thiz, dvr_load_start_t start, dvr_load_stop_t stop, void *context)
{
	HxLOG_Error(C_CODE_F_WHITE_B_RED" NOT IMPLEMENTED YET!!! thiz(%p) start(%p) stop(%p) context(%p)"C_CODE_RESET"\n", thiz, start, stop, context);

	return ITKE_NOT_SUPPORTED;
}

/**
 * Removes callbacks.
 * @param thiz The DVR interface
 */
/**********************************************************/
//1 This function may not be used for Australia profile.

void mheg_itk_port_dvrRemoveDownloadCallbacks(struct itk_dvr_t *thiz)
{
	HxLOG_Error(C_CODE_F_WHITE_B_RED" NOT IMPLEMENTED YET!!! thiz(%p)"C_CODE_RESET"\n", thiz);
}


/**
 * Returns a list of all current bookingIds.
 * The returned string should persist until released by freeString
 * @param thiz The DVR interface.
 * @remark The format of the booking id list is dependant on
 * the profile. See iTK User Guide for more information.
 */
/*
	1. <booking CRID> <CRID type>
	2. space-separated list of CRIDs.
	(ex) "crid://example.com/tv1 49 crid://example.com/tv2 50 crid://example.com/tv3 49"
*/
const char *mheg_itk_port_dvrGetBookingList(struct itk_dvr_t *thiz)
{
	Itk_Dvr_Str_Buffer_t *pStrBuffer;

	if (thiz == NULL)
	{
		HxLOG_Error(" Pointer is NULL!!!\n");
		return s_EmptyBuf;
	}

	/*
		2010.03.08. NULL을 return하면 Hangup발생한다
		예약된 list가 없으면 temporary buffer를 리턴한다
	*/
	pStrBuffer = mheg_itk_port_dvrGetAvailableStrBuffer();
	if (pStrBuffer == NULL)
	{
		HxLOG_Error(" Not enough STATIC STRING BUFFER!!!\n");
		return s_EmptyBuf;
	}
	else
	{
		pStrBuffer->ucUsed = 1;

		mheg_itk_port_dvrBookingMakeList(pStrBuffer->szBuffer, ITK_DVR_STRING_BUFFER_SIZE);

		HxLOG_Debug(" thiz(%p) pBookingList(%s) pBookingList(%p)\n", thiz, pStrBuffer->szBuffer, pStrBuffer->szBuffer);

		return pStrBuffer->szBuffer;
	}
}

/**
 * Creates a new booking.
 * @param thiz The DVR interface
 * @param bookingId String identifier for booking.
 * @param bookingData String defining the bookings properties.
 * @return A profile independent result code
 * @remark The format of the booking id and data is dependant on
 * the profile. See iTK User Guide for more information.
 */
itk_dvr_result_t mheg_itk_port_dvrMakeBooking( struct itk_dvr_t *thiz, const char *bookingId, const char *bookingData /* JSON */)
{
	Itk_Dvr_Booking_Handle_t *pBookingHandle;
	char *pCridBuf = NULL, *pJsonBuf = NULL;
	int nCridBufLen = 0, nJsonBufLen = 0;
	int json_result;

	if ((thiz == NULL) || (bookingId == NULL) || (bookingData == NULL))
	{
		HxLOG_Error(" Pointer is NULL!!!\n");
		return ITK_DVR_RESULT_FAILED;
	}

	HxLOG_Debug(" thiz(%p) bookingId(%s) DataLength(%d) bookingData(%s)\n", thiz, bookingId, HxSTD_StrLen(bookingData), bookingData);

	/*
		Search the bookingId whether already exist or not
	*/
	pBookingHandle = mheg_itk_port_dvrBookingFindHandle(bookingId, pHeadBookingHandle, pTailBookingHandle);
	if (pBookingHandle != NULL)
	{
		HxLOG_Debug(" Already exists!!!\n");
		return ITK_DVR_RESULT_ALREADY_BOOKED;
	}

	pBookingHandle = HLIB_STD_MemAlloc(sizeof(Itk_Dvr_Booking_Handle_t));
	if (pBookingHandle == NULL)
	{
		HxLOG_Error(" Not enogh memory!!!\n");
		goto fail_done;
	}

	nCridBufLen = HxSTD_StrLen(bookingId) + 1;
	pCridBuf = HLIB_STD_MemAlloc(nCridBufLen);
	if (pCridBuf == NULL)
	{
		HxLOG_Error(" Not enogh memory!!!\n");
		goto fail_done;
	}
	HxSTD_memset(pCridBuf, 0, nCridBufLen);

	nJsonBufLen = HxSTD_StrLen(bookingData) + 1;
	pJsonBuf = HLIB_STD_MemAlloc(nJsonBufLen);
	if (pJsonBuf == NULL)
	{
		HxLOG_Error(" Not enogh memory!!!\n");
		goto fail_done;
	}
	HxSTD_memset(pJsonBuf, 0, nJsonBufLen);

	json_result = mheg_itk_port_dvrStartJSONParser((void*)pBookingHandle, bookingData);
	if (json_result == 0)
	{
		HxSTD_StrNCpy(pCridBuf, bookingId, nCridBufLen -1);
		pCridBuf[nCridBufLen - 1] = '\0';

		HxSTD_StrNCpy(pJsonBuf, bookingData, nJsonBufLen -1);
		pJsonBuf[nJsonBufLen - 1] = '\0';

		/* Make the CRID buff from HEAP */
		pBookingHandle->pszCRID = pCridBuf;
		/* Make the JSON string buffer form HEAP */
		pBookingHandle->pszJSON = pJsonBuf;
		/* Set the booking handle pointer as unique id. It will be used for file name to nvm. */
		pBookingHandle->uniqueId = (HUINT32)pBookingHandle;

		// TODO: 실제 host reservation engine과 연동하여 예약 하는 연동 필요

		/* Add the Node to the booking list */
		mheg_itk_port_dvrBookingAppendHandle(pBookingHandle, &pHeadBookingHandle, &pTailBookingHandle);

		mheg_itk_port_dvrBookingAddToFile(pBookingHandle);

		return ITK_DVR_RESULT_ADDED;
	}

fail_done:

	if (pBookingHandle)
		HLIB_STD_MemFree(pBookingHandle);
	if (pCridBuf)
		HLIB_STD_MemFree(pCridBuf);
	if (pJsonBuf)
		HLIB_STD_MemFree(pJsonBuf);

	return ITK_DVR_RESULT_FAILED;
}

/**
 * Returns the booking data associated with a bookingId.
 * The returned string should persist until released by freeString.
 * @param thiz The DVR interface.
 * @remark The format of the booking data string is profile specific.
 * See iTK user guide for more information.
 */
/*
[EX]
{
	"description":"PVR bookingDetails Schema",
	"type":"object",
	"properties":
	{
		"cridtype":"49",
		"title":"Travel Oz",
		"synopsis":"(G) [CC] Featuring the Great Barrier Reef.",
			"booking":[{"location":"dvb://1010.0221.0220@20100301T18:02-PT00H29M"}],
			"booking":[{"location":"dvb://1010.0221.0221@20100301T18:02-PT00H29M"}],
			"booking":[{"location":"dvb://1010.0221.0223@20100301T18:02-PT00H29M"}]
	}
}
*/
const char *mheg_itk_port_dvrGetBookingData(struct itk_dvr_t *thiz, const char *bookingId)
{
	Itk_Dvr_Str_Buffer_t *pStrBuffer;
	Itk_Dvr_Booking_Handle_t *pBookingHandle;
	int json_result;

	HxLOG_Debug(" thiz(%p) bookingId(%p)\n", thiz, bookingId);

	if ((thiz == NULL) || (bookingId == NULL))
	{
		HxLOG_Error(" Pointer is NULL!!!\n");
		return s_EmptyBuf;
	}

	/*
		Search the bookingId whether already exist or not
	*/
	pBookingHandle = mheg_itk_port_dvrBookingFindHandle(bookingId, pHeadBookingHandle, pTailBookingHandle);
	if (pBookingHandle == NULL)
	{
		HxLOG_Debug(" Cannot find the BookingId(%s)!!!\n", bookingId);
		return s_EmptyBuf;
	}

	pStrBuffer = mheg_itk_port_dvrGetAvailableStrBuffer();
	if (pStrBuffer == NULL)
	{
		HxLOG_Error(" Not enough STATIC STRING BUFFER!!!\n");
		return s_EmptyBuf;
	}

	json_result = mheg_itk_port_dvrBookingMakeDataToJSON(pStrBuffer->szBuffer, pBookingHandle->pszJSON);
	if (json_result == 0)
	{
		pStrBuffer->ucUsed = 1;

		HxLOG_Debug(" thiz(%p) JSON(%s)\n", thiz, pStrBuffer->szBuffer);

		return pStrBuffer->szBuffer;
	}
	else
	{
		HxLOG_Error(" Cannot make a JSON string for BookingData!!!\n");
		return s_EmptyBuf;
	}
}

/**
 * Cancels an existing booking.
 * @param thiz The DVR interface
 * @param bookingId String identifier for the booking.
 * @return A profile independent result code.
 * @remark The format of the booking id is dependant on
 * the profile. See iTK User Guide for more information.
 */
itk_dvr_result_t mheg_itk_port_dvrCancelBooking(struct itk_dvr_t *thiz, const char *bookingId)
{
	itk_dvr_result_t cancel_result = ITK_DVR_RESULT_FAILED;

	HxLOG_Debug(" thiz(%p) bookingId(%s)\n", thiz, bookingId);

	if ((thiz == NULL) || (bookingId == NULL))
	{
		HxLOG_Error(" Pointer is NULL!!!\n");
		return ITK_DVR_RESULT_FAILED;
	}

	cancel_result = mheg_itk_port_dvrCancelBookingByBookingId(bookingId);

	// TODO: 실제 host reservation engine과 연동하여 취소 하는 연동 필요

	return cancel_result;
}

/**
 * Returns a space seperated list of all recorded content.
 * The returned string should persist until released by freeString.
 * @param thiz The DVR interface.
 * @remark The list should be arranged in the order that content
 * was recorded with the oldest content first.
 */
/**********************************************************/
//1 This function may not be used for Australia profile.

const char *mheg_itk_port_dvrGetContentList(struct itk_dvr_t *thiz)
{
	HxLOG_Error(C_CODE_F_WHITE_B_RED" NOT IMPLEMENTED YET!!! thiz(%p)"C_CODE_RESET"\n", thiz);

	return NULL;
}

/**
 * Returns the content info associated with a contentRef.
 * The returned string should persist until released by freeString.
 * @param thiz The DVR interface.
 * @remark The format of the content info string is profile specific.
 * See iTK User Guide for more information.
 */
/**********************************************************/
//1 This function may not be used for Australia profile.

const char *mheg_itk_port_dvrGetContentInfo(struct itk_dvr_t *thiz, const char *contentRef)
{
	HxLOG_Error(C_CODE_F_WHITE_B_RED" NOT IMPLEMENTED YET!!! thiz(%p) contentRef(%p)"C_CODE_RESET"\n", thiz, contentRef);

	return NULL;
}

/**
 * Moves content to given destination.
 * @param thiz The DVR interface.
 * @param contentRef The content ref of the content to move.
 * @param dest A string identifiying the destination.
 * @remark The format of the destination string is profile specific.
 * See iTK User Guide for more information.
 * @return A profile independent result code
 */
/**********************************************************/
//1 This function may not be used for Australia profile.

itk_dvr_result_t mheg_itk_port_dvrMoveContent(struct itk_dvr_t *thiz, const char *contentRef, const char *dest)
{
	HxLOG_Error(C_CODE_F_WHITE_B_RED" NOT IMPLEMENTED YET!!!thiz(%p) contentRef(%s) dest(%s)"C_CODE_RESET"\n", thiz, contentRef, dest);

	return ITK_DVR_RESULT_FAILED;
}

/**
 * Frees a booking list, booking data, content list or content info string.
 * @param thiz The DVR interface
 * @param idDataString The string to be released.
 * @remark Strings that require freeing are those returned from getBookingList
 * getBookingData, getContentList and getContentInfo.
 */
void mheg_itk_port_dvrFreeString(struct itk_dvr_t *thiz, const char *idDataString)
{
	HxLOG_Debug(" thiz(%p), idDataString(%p)\n", thiz, idDataString);

	mheg_itk_port_dvrFreeStrBuffer(idDataString);
}

/**
 * Sets the callback to be invoked when PVR events occur, such as a change to the
 * current list of bookings.
 * @param thiz The DVR interface
 * @param callback The callback function to invoke
 * @param context A context pointer to pass to the callback.
 */
 #ifdef CONFIG_DEBUG
static HUINT8 ucCallbackNum;
#endif
void mheg_itk_port_dvrAddCallback( struct itk_dvr_t *thiz, itk_dvr_event_callback_t callback, void *context )
{
	HxLOG_Debug(" thiz(%p) callback(%p) context(%p)\n", thiz, callback, context);

	if (thiz == NULL)
	{
		HxLOG_Error(" thiz is NULL!!\n");
		return;
	}

	if (callback == NULL)
	{
		HxLOG_Error(" callback is NULL!!\n");
		return;
	}

	s_dvr_event_callback = callback;
	s_dvr_event_callback_context = context;

#ifdef CONFIG_DEBUG
	if (ucCallbackNum > 1)
	{
		HxLOG_Error(C_CODE_F_WHITE_B_RED" TOO MANY ADD CALLBACK(%d)!!!"C_CODE_RESET"\n", ucCallbackNum);
	}

	ucCallbackNum++;
#endif
}

/**
 * Removes any callback that has been set for PVR events.
 */
void mheg_itk_port_dvrRemoveEventCallback(struct itk_dvr_t *thiz)
{
	HxLOG_Debug(" thiz(%p)\n", thiz);

	if (thiz == NULL)
	{
		HxLOG_Error(" thiz is NULL!!\n");
		return;
	}

	s_dvr_event_callback = NULL;
	s_dvr_event_callback_context = NULL;

#ifdef CONFIG_DEBUG
	ucCallbackNum--;
#endif
}

#define _____PUBLIC_APIs________________________________________________________________

/*
 The list of existing bookings might change for a number of reasons,
 including when bookings are made or cancelled by MHEG-5 applications or through the receiver's native UI;
 and also when bookings have been recorded or otherwise resolved.
 Theses events are defined in
 @ itk_dvr_event_t .. and should be notified to the client via callback
 @ itk_dvr_event_callback_t ..
 Function
 @ itk_dvr_t::addEventCallback() .. is used to register the callback during initialisation.
*/
void PORT_ITK_Dvr_ExecuteEventCallback(void)
{
	// TODO: 실제 host reservation engine과 연동하여 예약 Data가 변경 되었다면 이 callback수행

	mheg_itk_port_dvrExecuteEventCallback(ITK_DVR_EV_BOOKINGS);
}

int PORT_ITK_Dvr_CancelBookingByBookingId(const char *bookingId)
{
	itk_dvr_result_t cancel_result;

	cancel_result =  mheg_itk_port_dvrCancelBookingByBookingId(bookingId);

	if (cancel_result == ITK_DVR_RESULT_REMOVED)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

void PORT_ITK_Dvr_Reset(void)
{
	/* Host reservation engine의 예약 내용이 모두 초기화 되었는지 확인 필요 */
	mheg_itk_port_dvrReset();
}

itk_dvr_t *mheg_itk_port_dvrGetObject(void)
{
	itk_dvr_t	*pDvr = NULL;

	pDvr = (itk_dvr_t*)HLIB_STD_MemAlloc(sizeof(itk_dvr_t));
	if (pDvr == NULL)
	{
		HxLOG_Error(" There is not enough memory !!!\n");
		return NULL;
	}

	pDvr->setDownloadCallbacks = mheg_itk_port_dvrSetDownloadCallbacks;
	pDvr->removeDownloadCallbacks = mheg_itk_port_dvrRemoveDownloadCallbacks;
	pDvr->getBookingList = mheg_itk_port_dvrGetBookingList;
	pDvr->makeBooking = mheg_itk_port_dvrMakeBooking;
	pDvr->getBookingData = mheg_itk_port_dvrGetBookingData;
	pDvr->cancelBooking = mheg_itk_port_dvrCancelBooking;
	pDvr->getContentList = mheg_itk_port_dvrGetContentList;
	pDvr->getContentInfo = mheg_itk_port_dvrGetContentInfo;
	pDvr->moveContent = mheg_itk_port_dvrMoveContent;
	pDvr->freeString = mheg_itk_port_dvrFreeString;
	pDvr->addEventCallback = mheg_itk_port_dvrAddCallback;
	pDvr->removeEventCallback = mheg_itk_port_dvrRemoveEventCallback;

	HxLOG_Error(C_CODE_F_WHITE_B_BLUE" DVR Object ptr(%p)!!!"C_CODE_RESET"\n", pDvr);

	return pDvr;
}

void PORT_ITK_Dvr_CreateObject(void)
{
	s_pstDvrObj = mheg_itk_port_dvrGetObject();

	mheg_itk_port_dvrBookingFileRead();

	if (s_pstDvrObj)
		ITK_registerManager(ITK_DVR, s_pstDvrObj);
}


