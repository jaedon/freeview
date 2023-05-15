/*************************************************************************************************************
	File            : pl_dmp_uplaod.c
	author          : IPM tipmsw1@humaxdigital.com
	comment         :
	date            :
	attention       :

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. (Humax) and
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
	non-assignable, non-transferable and non-exclusive license to use this Software.
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
	you agree to the responsibility to take all reasonable efforts to protect the any information
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
	reverse engineer or extract the source code of this Software unless you have Humax's written permission
	to do so.
	If you have no authorized license, discontinue using this Software immediately.

	THE SOFTWARE IS PROVIDED ¡°AS IS¡± AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
	EITHER EXPRESS, IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
	IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR
	INDIRECT, SPECIAL, CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR
	INABILITY TO USE THE SOFTWARE.

	This License is effective until terminated. You may terminate this License at any time
	by destroying all copies of the Software including all documentation. This License will terminate
	immediately without notice from Humax to you if you fail to comply with any provision of this License.
	Upon termination, you must destroy all copies of the Software and all documentation.

	The laws of the Republic of Korea will apply to any disputes arising out of or relating to
	this Copyright Notice. All claims arising out of or relating to this Copyright Notice will be litigated
	in the Seoul Central District Court, in the Republic of Korea.
*************************************************************************************************************/

#define	________________Header_Files_______________________________

#include <hlib.h>

#include "mxdlna.h"
#include "mxres.h"
#include "mxfile.h"
#include "mxdlna_mscp.h"
#include "mxmedia_define.h"
#include <sys/time.h>

#include "vkernel.h"
#include "common.h"
#include "pl_common.h"
#include "pl_dmp.h"

#define ________________Private_Mecro_and_Defines__________________
#define	__MODULE__	"PL:DMP:UPLOAD"

#define UPLOAD_ASSERT(x)	\
    if ( x == NULL )	\
    {\
	    HxLOG_Error("[ %s:%d ] : the param is NULL.\n", __MODULE__, __LINE__);\
	    return ERR_FAIL;\
    }\

#define	PL_DMP_SNPRINTF(x, v)			HxSTD_snprintf(x, sizeof(x), "%s", v)

#define UPLOAD_CHECK_OBJECT(x) \
	if(__find_obj((upload_obj_t*)obj) == NULL )\
	{\
	    HxLOG_Error("[ %s:%d ] : the object is not existed.\n", __MODULE__, __LINE__);\
	    return ERR_FAIL;\
	}\


#define MAX_OBJECT_COUNT 	10
#define UPLOAD_TASK_NEME   "upload_task"
#define UPLOAD_SEMA_NEME   "upload_sema"

#define ________________Internal_Typedef_and_Structure_____________
typedef void (*upload_callback)( HINT32 nService, HINT32 p1,  HINT32 p2, HINT32 p3);

typedef struct
{
	MXFILE_HANDLE		handle;
	MXFILE_DESC			desc;
	HUINT32				ip;
	HCHAR				*url;
} upload_dst_t;

typedef struct
{
	HCHAR				*path;
	HUINT32				fd;
	HINT64				size;
} upload_dub_t;

typedef struct
{
	Handle_t			hRingBuf;
} upload_rec_t;

typedef enum
{
	UPLOAD_IDLE = 0,
	UPLOAD_START,
	UPLOAD_STOP,
	UPLOAD_END,
	UPLOAD_ERROR,
} upload_state_e;

typedef enum
{
	Upload_Error_None = 0,
	Upload_Error_Read_Source = 901,
	Upload_Error_Send_SinkDms = 902,
	Upload_Error_CancelByUser = 903,
	Upload_Error_AkeCommit = 904,
	Upload_Error_PostResult = 905, /*  http post value is 200 under */
} upload_err_reason_e;

typedef struct
{
	HBOOL				ake;

	HULONG				read_size;
	HUINT64				write_total;

	HINT32				err_cnt;
	HBOOL				read_err;
	HBOOL				write_err;

	upload_state_e		state;
	upload_err_reason_e	reason;
} upload_status_t;

typedef enum
{
	UPLOAD_DUBBING,
	UPLOAD_RECORDING,
} upload_type_e;

typedef struct
{
	upload_type_e		type;

	/*  dms info */
	upload_dst_t		dst;

	/*  src info */
	union
	{
		upload_dub_t 	file;
		upload_rec_t 	rec;
	} src;

	HCHAR				*buf;

	/*  upload status */
	upload_status_t		status;
} upload_obj_t;

typedef struct
{
	HINT32  dtcp_port;
	HINT32  buf_size;

	upload_callback		cb;
	upload_obj_t		*obj[MAX_OBJECT_COUNT];
	HUINT32				obj_cnt;

	HxList_t			*obj_list;

	HULONG  task;
	HULONG  lock;

	HBOOL	is_init;
} upload_context_t;

#define ________________Private_Static_Variables___________________
static upload_context_t	st_upload_context;

#define ________________Internal_APIs_Prototype____________________
extern HINT32 ILibGetLocalIPAddressList(HINT32** pp_int);
extern void ILibParseUri(HCHAR* URI, HCHAR** IP, unsigned short* Port, HCHAR** Path);
extern HCHAR *ILibIPAddress_ToDottedQuad(HINT32 ip);

#define ________________Internal_Functions_________________________
static upload_context_t* __context(void)
{
	return &st_upload_context;
}

static upload_obj_t*	__new_obj(void)
{
	upload_obj_t	*p = HLIB_STD_MemCalloc(sizeof(upload_obj_t));

	if(p)  	return p;

	HxLOG_Debug("[%s:%d] not enough memcpy. \n", __FUNCTION__, __LINE__);

	return NULL;
}

static upload_obj_t * __find_obj(upload_obj_t *obj)
{
	upload_context_t	*ctx = __context();
	upload_obj_t		*p = NULL;
	HxList_t			*list = NULL;

	list = HLIB_LIST_Find(HLIB_LIST_First(ctx->obj_list), obj);
	p = HLIB_LIST_Data(list);

	if(p)  	return p;

	HxLOG_Debug("[%s:%d] object is not existed. \n", __FUNCTION__, __LINE__);

	return NULL;
}

static HUINT32	__get_obj_count(void)
{
	upload_context_t	*ctx = __context();
	return HLIB_LIST_Length(HLIB_LIST_First(ctx->obj_list));
}

static inline void  __lock(void)
{
    VK_SEM_Get(st_upload_context.lock);
}

static inline void __unlock(void)
{
    VK_SEM_Release(st_upload_context.lock);
}

static void __print_time(void)
{
    UNIXTIME        utCurrentTime = 0;
    HxDATETIME_t    stDate;
    HERROR          hErr = ERR_OK;
    hErr = VK_CLOCK_GetTime((HULONG*)&utCurrentTime);
    if(hErr != VK_OK)
    {
        HxLOG_Info("VK_CLOCK_GetTime returned err (%d)  [%s : %d]\n",hErr, __FUNCTION__,__LINE__);
    }
    else
    {
        hErr = HLIB_DATETIME_ConvertUnixTimeToDateTime(utCurrentTime, &stDate);
        if(hErr != ERR_OK)
        {
            HxLOG_Info("HLIB_DATETIME_ConvertUnixTimeToDateTime returned err (%d)  [%s : %d]\n",hErr, __FUNCTION__,__LINE__);
        }
        else
        {
            HxLOG_Error("Current Time : %d / %d / %d \t %d : %d : %d\n", \
                    stDate.stDate.usYear, stDate.stDate.ucMonth, stDate.stDate.ucDay, stDate.stTime.ucHour, stDate.stTime.ucMinute, stDate.stTime.ucSecond);
        }
    }
}

static void __show_obj_status(upload_obj_t *p)
{
    HINT32              nPercent = 0;

	switch(p->type)
	{
		case UPLOAD_DUBBING:
			{
				nPercent = (HINT32)((p->status.write_total * 100)/p->src.file.size);
				__print_time();
    			HxLOG_Print("[%s:%d] <%d> %d %%  Total write :  %lld MB  / FileSize :  %lld MB\n" ,__MODULE__, __LINE__,
           				(HUINT32)p, nPercent, p->status.write_total/(1024*1024), p->src.file.size/(1024*1024));
			}
			break;

		case UPLOAD_RECORDING:
			{

			}
			break;
	}
}

/* protocol info 에서 3번째 field string 을 얻어온다. */
static HCHAR *__get_third_field( HCHAR *s )
{
    if ( s )
    {
        HINT32 count = 1;
        HCHAR *p;

        p = strchr( s, ':' );
        while ( p && count > 0 )
        {
            p ++;
            count --;
            p = strchr( p, ':' );
        }

        if ( count == 0 && p )
        {
            HCHAR *n;
            HCHAR *ret = (HCHAR *)HLIB_STD_MemAlloc( HxSTD_StrLen(p) + 1 ) ;
            p ++;
            n = strchr( p , ':' );

            if ( n )
            {
                HxSTD_StrNCpy( ret , p , (n-p) );
                ret[ n-p ] = '\0';
            }
            else
                HxSTD_StrNCpy( ret , p, HxSTD_StrLen(p));

            return ret;
        }
    }

    return "";
}

/* protocol info 에서 4번째 field string 을 얻어온다. */
static HCHAR *__get_fourth_field( HCHAR * s )
{
    if ( s )
    {
        HINT32 count = 2;
        HCHAR *p;

        p = strchr( s, ':' );
        while ( p && count > 0 )
        {
            p ++;
            count --;
            p = strchr( p, ':' );
        }

        if ( count == 0 && p )
            return p + 1;
    }

    return "";
}

/* 접속하는 url의 IP 를 참조하여 같은 subnet의 자신의 IP를 찾는다. */
static HCHAR *__getLocalIPFromRemoteIP( HCHAR *url )
{
	HCHAR *ret = (HCHAR *)mxUtil_getStaticBuffer( 128 );
	HCHAR *ip = NULL ;

	HCHAR *path = NULL;
	HUINT16 port;
	HINT32 *list = NULL;
	HINT32 ipaddr;

	HINT32 target = 0;
	HINT32 target_best = -1;

	HINT32 i;
	HINT32 ipcount = ILibGetLocalIPAddressList( &list );

	ILibParseUri(url, &ip , &port , &path );

	ipaddr = inet_addr( ip );

	for ( i = 0 ; i < ipcount ; i ++ )
	{
		HINT32 j;
		HCHAR *a = (HCHAR*)&ipaddr;
		HCHAR *b = (HCHAR*)&list[i];
		HINT32 score = 0;

		for ( j = 0 ; j < 3 ; j ++ )
			if ( a[j] == b[j] ) score ++;

		if ( score >  target_best )
		{
			target_best = score;
			target = i;
		}
	}

	HxSTD_StrNCpy( ret , ILibIPAddress_ToDottedQuad( list[target] ), 127 );

	if(NULL == ip || NULL == path || NULL == list)
	{
		HxLOG_Error("Check null pointer: ip = 0x%x, path = 0x%x, list = 0x%x\n", ip, path, list);
	}

	if(ip)		MX_FREE( ip );
	if(path)	MX_FREE( path );
	if(list)	MX_FREE( list );

	return ret;
}

static HERROR   __make_header(HCHAR *protocol, HCHAR *url, HCHAR **ppHeader)
{
	upload_context_t 	*ctx = __context();
    HCHAR *header = (char*)mxUtil_getStaticBuffer(1024);

    header[0] = '\0';

    HxSTD_snprintf( header + HxSTD_StrLen(header ) , 1024, "USER-AGENT: UPnP/1.0 DLNADOC/1.50\r\n"
                                        "Connection: Keep-Alive\r\n"
                                        "Cache-Control: no-cache\r\n"
                                        );

    HxSTD_snprintf( header + HxSTD_StrLen(header ) , 1024, "contentFeatures.dlna.org: %s\r\n" , __get_fourth_field( protocol) );

    HxSTD_snprintf( header + HxSTD_StrLen(header ) , 1024,
        "Content-Type: application/x-dtcp1;DTCP1HOST=%s;DTCP1PORT=%d;CONTENTFORMAT=\"%s\"\r\n",
        __getLocalIPFromRemoteIP( url ),
        ctx->dtcp_port,
        __get_third_field(protocol));

    *ppHeader = header;

    return ERR_OK;
}

static void __task(void *p)
{
	upload_context_t 	*ctx = __context();
	upload_obj_t 		*obj = NULL;

	HERROR              err = 0;
	HUINT32             ring_read_size=0;
	MX_UINT32           write_size=0;

	HxList_t 			*list = NULL;

	while (1)
	{
		//__lock();

		if(__get_obj_count() == 0)
		{
			HLIB_STD_TaskSleep(1000);
			continue;
		}

		if(list == NULL)
			list = ctx->obj_list;
		else
			obj = HLIB_LIST_Data(list);

		/*  todo - object status check  */

		if(obj->status.state == UPLOAD_STOP)
		{
			/*  todo - stop process  */
			if(obj->type == UPLOAD_DUBBING)
			{
				HxLOG_Print("[%s:%d] Dubbing Cancel. !! \n", __MODULE__, __LINE__);
				obj->status.reason = Upload_Error_CancelByUser;
				/*  stop event send  */
			}
			else
			{
				HxLOG_Print("[%s:%d] DLNA Record Stop Complete. !!  \n", __MODULE__, __LINE__);
				obj->status.state  = UPLOAD_END;
				/*  stop event send  */
			}
			continue;
		}

		if(obj->status.write_err == FALSE)
		{
			switch(obj->type)
			{
				case UPLOAD_DUBBING:
					//err = PAL_FS_Secure_Read(obj->dst.file.fd, obj->buf, ctx->buf_size, &obj->status.read_size);
					break;
				case UPLOAD_RECORDING:
					{
#if 0
						/* recording */
						if((obj->dst.rec.hRingBuf != HANDLE_NULL) && (obj->dst.rec.hRingBuf != 0))
						{
							PalRingBuf_Status_t stStatus = {0,};
							err = PAL_RINGBUF_GetBufferingStatus(obj->dst.rec.hRingBuf, &stStatus);
							if(err != ERR_OK)
							{
								HxLOG_Critical("[%s:%d] %d READ ERROR !! ring buffer is not existed. \n", __MODULE__, __LINE__, (HUINT32)obj);
								obj->statuc.err_cnt = 0xFF;
							}
							else
							{
								if(stStatus.bOverflowed == TRUE)
								{
									HxLOG_Critical("[%s:%d] ERROR !! ring buffer is overflowed !!. \n", __MODULE__, __LINE__);
									pObj->eLastErrorCode = Upload_Complete_Error_BufferOverflow;
									continue;
								}

								if(stStatus.ulUnreadSize > 0)
								{
									HxLOG_Print("[%s:%d] UnreadSize ring buffer size :: %d \n", __FUNCTION__, __LINE__, stStatus.ulUnreadSize);
									err = PAL_RINGBUF_ReadData(obj->hRingBuf, ctx->nBufferSize, obj->buf, (HUINT32 *)&ring_read_size);
									if(err == ERR_OK)
										obj->status.read_size = ring_read_size;
								}
							}
						}
						else
						{
							HxLOG_Critical("[%s:%d] READ ERROR !! invalid ring buffer handle. \n", __MODULE__, __LINE__);
							err = ERR_FAIL;
						}
#endif
					}
					break;
			}

			if(err != ERR_OK)
			{
				HxLOG_Debug("[%s:%d] read error occurred during uploading. so will be retry.  \n", __MODULE__, __LINE__);
				obj->status.read_err = TRUE;
				obj->status.err_cnt++;

				HLIB_STD_TaskSleep(100);

				if(obj->status.err_cnt > 15)
				{
					HxLOG_Debug("[%s:%d] retry time(%d). so will be object(%d) cleanup.  \n", __MODULE__, __LINE__, obj->status.err_cnt, (HUINT32)obj);
					//pObj->eLastErrorCode = Upload_Complete_Error_Read_Source;
				}
				else
					continue;
			}
			else
			{
				obj->status.read_err = FALSE;
				obj->status.err_cnt = 0;
			}
		}
		else
			HxLOG_Print("[%s:%d] read is skiped. because of write error. !! \n", __MODULE__, __LINE__);

		/* object write */
		if(obj->status.read_size > 0 )
		{
			if ( mxFile_writeFile(obj->dst.handle, obj->buf, obj->status.read_size, &write_size ) != MXFILE_OK )
			{
				HxLOG_Error("[%s:%d] write error occurred during uploading. so will be write retry.  \n", __MODULE__, __LINE__);

				obj->status.write_err = TRUE;
				obj->status.err_cnt++;

				HLIB_STD_TaskSleep(300);

				if(obj->status.err_cnt > 15)
				{
					HxLOG_Critical("[%s:%d] retry time(%d). so will be object(%d) cleanup.  \n", __MODULE__, __LINE__, obj->status.err_cnt, (HUINT32)obj);
					obj->status.reason = Upload_Error_Send_SinkDms;
				}

				write_size = 0;
				continue;
			}
			else
			{
				obj->status.write_err 	= FALSE;
				obj->status.err_cnt		= 0;
				obj->status.write_total += write_size;

				write_size = 0;
				obj->status.read_size = 0;

				if(obj->type == UPLOAD_DUBBING)
				{
					if(obj->status.write_total == obj->src.file.size/* || PAL_FS_Secure_IsEOF(pObj->f) */)
					{
						HxLOG_Print("[%s:%d] Write Complete. !!\n", __MODULE__, __LINE__);
						obj->status.state  = UPLOAD_END;
					}
				}
				/*  recording is not upload complete timing. just stop */
			}

			__show_obj_status(obj);
		}

		list = list->next;
	}
}

#define ________________Public_Functions___________________________

HERROR	dmp_upload_init(upload_callback callback)
{
	upload_context_t 	*ctx = __context();

	UPLOAD_ASSERT(callback);

	HxSTD_MemSet(&ctx, 0x00, sizeof(upload_context_t));

	ctx->cb = callback;
	ctx->dtcp_port	= 9998;
	ctx->buf_size	= 256 * 1024;

	if(VK_TASK_Create(__task, VK_TASK_PRIORITY_MW_REF, SIZE_64K, UPLOAD_TASK_NEME, NULL,
                    &ctx->task, 0))
    {
        HxLOG_Error( "[ %s:%d ] cann't create upload command tesk. [%d]\n", __MODULE__, __LINE__);
        return ERR_FAIL;
    }

    if(VK_SEM_Create(&ctx->lock, UPLOAD_SEMA_NEME, VK_SUSPENDTYPE_PRIORITY))
    {
        HxLOG_Error( "[ %s:%d ] cann't create upload sender semaphone. [%d]\n", __MODULE__, __LINE__);
        return ERR_FAIL;
    }


    if(VK_TASK_Start(ctx->task))
    {
        HxLOG_Error( "[ %s:%d ] cann't start upload command task. [%d]\n", __MODULE__, __LINE__);
        return ERR_FAIL;
    }

    ctx->is_init = TRUE;

	return ERR_OK;
}

HERROR	dmp_upload_deinit(void)
{

	return ERR_OK;
}


void *dmp_upload_dub_add(HCHAR *url, HCHAR *file)
{
	upload_context_t 	*ctx = __context();
	upload_obj_t 		*p = __new_obj();

	p->type = UPLOAD_DUBBING;
	p->dst.url = HLIB_STD_StrDup(url);
	p->src.file.path = HLIB_STD_StrDup(file);

	ctx->obj_list = HLIB_LIST_Append(ctx->obj_list, (void *)p);

	return (void *)p;
}

void *dmp_upload_rec_add(HCHAR *url, Handle_t handle)
{
	upload_context_t 	*ctx = __context();
	upload_obj_t 		*p = __new_obj();

	p->type = UPLOAD_RECORDING;
	p->dst.url = HLIB_STD_StrDup(url);
	p->src.rec.hRingBuf = handle;

	ctx->obj_list = HLIB_LIST_Append(ctx->obj_list, (void *)p);

	return (void *)p;
}

HERROR	dmp_upload_del(void *obj)
{
	upload_obj_t 	*p = (upload_obj_t*) obj;

	UPLOAD_ASSERT(p);
	UPLOAD_CHECK_OBJECT(p);



	return ERR_OK;
}

HERROR	dmp_upload_open(void *obj, HCHAR *protocol)
{
	HCHAR				*header = NULL;
	MX_UINT32           write_size = 0;

	upload_obj_t 		*p = (upload_obj_t*) obj;
	upload_context_t 	*ctx = __context();

	UPLOAD_ASSERT(p);
	UPLOAD_CHECK_OBJECT(p);

	p->buf = (HCHAR*)HLIB_STD_MemCalloc( ctx->buf_size);
	if ( p->buf == NULL )
	{
		HxLOG_Debug( "[ %s:%d ] NOT enough memory. \n", __MODULE__, __LINE__ );
		goto err;
	}

	HxSTD_MemSet ( &p->dst.desc , 0 , sizeof( MXFILE_DESC ) );
	p->dst.desc.url = p->dst.url;
	p->dst.desc.option |= MXFILEHTTP_DESC_OPTION_WRITE;
	p->dst.desc.option |= MXFILEHTTP_DESC_OPTION_DTCP;

#if 0
	switch(p->type)
	{
		case UPLOAD_DUBBING:
			{
				if( PAL_FS_Secure_Open(pObj->pPath, PAL_FS_MODE_READ, &pObj->f) != ERR_OK)
				{
					HxLOG_Error( "[ %s:%d ] source file(%s)  open failed. \n" ,__MODULE__, __LINE__, pObj->pPath);
					goto error;
				}

				if(PAL_FS_Secure_GetFileSize(pObj->f, &pObj->filesize) != ERR_OK)
				{
					HxLOG_Error( "[ %s:%d ] fail getting file size(%s). \n" ,__MODULE__, __LINE__, pObj->pPath);
					ErrCode = Upload_Start_Error_FileOpen;
					goto error;
				}

				*(MX_INT64*)( p->dst.desc.user_data + 8 ) = p->src.file.size;
				*(HUINT8 *)( p->dst.desc.user_data + 16 ) = dtcpEmiMove ;

			}
			break;

		case UPLOAD_RECORDING:
			{
				*(MX_INT64*)( p->dst.desc.user_data + 8 ) = 0;
				*(HUINT8 *)( p->dst.desc.user_data + 16 ) = dtcpEmiCopyOneGenFormatNonCog;
			}
			break;
	}
#endif

	if(__make_header(protocol, p->dst.url, &header) != ERR_OK)
	{
		HxLOG_Error( "[ %s:%d ] cann't make header. \n", __MODULE__, __LINE__);
		goto err;
	}

	if ( mxFile_openFile( &p->dst.desc , &p->dst.handle ) != MXFILE_OK )
	{
		HxLOG_Error( "[ %s:%d ] destnation file(%s) open failed. \n" ,__MODULE__, __LINE__, p->dst.url);
		goto err;
	}

	//mxFileHttp_setOptions( p->dst.handle, MXFILEHTTP_SO_SET_CUSTOM_HEADER , header );
	if(  mxFile_writeFile(p->dst.handle, p->buf, 0 , &write_size ))
	{
		HxLOG_Error( "[ %s:%d ] failed send start packet. (%u)  \n", __MODULE__, __LINE__, (HUINT32)write_size );
		goto err;
	}

	//mxFileHttp_getOptions( p->dst.handle, MXFILEHTTP_SO_GET_DTCP_IP , &p->dst.ip );
	//if ( mxDtcp_WaitAKE( p->dst.ip ) < 0 )
	{
		HxLOG_Error( "[ %s:%d ] AKE Wait fail.\n", __MODULE__, __LINE__ );
		goto err;
	}

	p->status.ake = TRUE;

	Sleep(500);

	if(ctx->cb)
		ctx->cb((HINT32)0, (HINT32)p, ERR_OK, 0);

err:

	return ERR_FAIL;
}

HERROR	dmp_upload_close(void *obj)
{
    HINT32  result = -1;
    MX_UINT32 write_size=0;

	upload_obj_t 		*p = (upload_obj_t*) obj;
	upload_context_t 	*ctx = __context();

	UPLOAD_ASSERT(p);
	UPLOAD_CHECK_OBJECT(p);

    if(p->status.state != UPLOAD_END)
        HxLOG_Print( "[ %s:%d ] Upload is not Completed. Error is occurred. Cann't process end paceket \n" , __MODULE__, __LINE__);

    mxFile_writeFile(p->dst.handle, p->buf, 0 , &write_size );

    /* 상대방에서 HTTP 200 OK 또는 HTTP 201 Created 의 메세지를 받기 위함
     result 값이 200 이상이면 성공임 */
    //if ( mxFileHttp_getOptions( p->dst.handle, MXFILEHTTP_SO_GET_POST_RESULT, &result) == MXFILE_OK )
    {
        HxLOG_Print( "[ %s:%d ] Post Result: %d\n" , __MODULE__, __LINE__, result);
    }

	switch(p->type)
	{
		case UPLOAD_DUBBING:
			{
				if(result >= 200 )
				{
					p->status.ake = FALSE;
					//if ( mxDtcp_WaitAKECommit( pObj->remote_ip ) < 0 )
					{
						HxLOG_Error( "[ %s:%d ] file is sent. but, dtcp  Failed to commit move. (%d)\n" , __MODULE__, __LINE__, result);
						p->status.reason = Upload_Error_AkeCommit;
						goto term;
					}

					HxLOG_Print( "[ %s:%d ] file is sent. dtcp Move commit. (%d) \n", __MODULE__, __LINE__, result );
				}
				else
				{
					p->status.reason = Upload_Error_PostResult;
					goto term;
				}
			}
			break;

		case UPLOAD_RECORDING:
			{
				if(result != -1)
					goto term;
				else
				{
					p->status.reason = Upload_Error_AkeCommit;
					goto term;
				}
			}
			break;
	}

term:

	switch(p->type)
	{
		case UPLOAD_DUBBING:
	//		if(p->src.file.fd) PAL_FS_Secure_Close(p->src.file.fd);
			break;
		case UPLOAD_RECORDING:
			if(p->src.rec.hRingBuf) p->src.rec.hRingBuf = 0;
			break;
	}

	if(p->dst.handle) mxFile_closeFile(p->dst.handle);
	//if(p->status.ake)  mxDtcp_WaitStopAKE( p->dst.ip );
	if(p->buf) HLIB_STD_MemFree(p->buf);

	if(ctx->cb)
	{
#if 0
		if(error != ERR_OK || p->status.state != UPLOAD_END)
		{
			HxLOG_Print("[%s:%d] Upload is completed.. ERROR(%d) \n", __MODULE__, __LINE__, pObj->eLastErrorCode);
			ctx->cb((HINT32)ePAL_DMP_ServiceEvent_Upload_Complete, pObj->pUdn, (HINT32)pObj, ERR_FAIL, pObj->eLastErrorCode);
		}
		else
		{
			HxLOG_Print("[%s:%d] Upload is completed.. OK\n", __MODULE__, __LINE__);
			ctx->cb((HINT32)ePAL_DMP_ServiceEvent_Upload_Complete, pObj->pUdn,  (HINT32)pObj, ERR_OK, pObj->content_id);
		}
#endif
	}


	return ERR_OK;
}


HERROR	dmp_upload_start(void *obj)
{
	upload_obj_t 	*p = (upload_obj_t*) obj;

	UPLOAD_ASSERT(obj);
	UPLOAD_CHECK_OBJECT(obj);

	__lock();

	p->status.state = UPLOAD_START;

	__unlock();

	return ERR_OK;
}

HERROR	dmp_upload_stop(void *obj)
{
	upload_obj_t 	*p = (upload_obj_t*) obj;

	UPLOAD_ASSERT(obj);
	UPLOAD_CHECK_OBJECT(obj);

	__lock();

	p->status.state = UPLOAD_STOP;

	__unlock();

	return ERR_OK;
}

HERROR	dmp_upload_get_status(void *obj, upload_status_t *status)
{
	upload_obj_t 	*p = (upload_obj_t*) obj;

	UPLOAD_ASSERT(obj);
	UPLOAD_CHECK_OBJECT(obj);

	__lock();

	memcpy(status, &(p->status), sizeof(upload_status_t));

	__unlock();

	return ERR_OK;
}
/* end od file */
