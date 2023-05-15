


#ifndef __MXHTTP_H__
#define __MXHTTP_H__


#ifdef __cplusplus
extern "C" 
{
#endif

#include "mxfile.h"


__MXAPI MXFILE_API_SET_PTR mxFileHttp_checkUrl ( MXFILE_DESC *desc );

__MXAPI void mxFileHttp_shutdown(void);
__MXAPI void mxFileHttp_startup(void);



#define MXFILEHTTP_SO_SET_CUSTOM_HEADER		0
#define MXFILEHTTP_SO_SET_HTTP_REFERER		1
#define MXFILEHTTP_SO_SET_USER_AGENT		2
#define MXFILEHTTP_SO_SET_TIMEOUT			3


typedef void (*CALLBACK_LOAD_COMPLETE)( MXFILE_HANDLE );

#define MXFILEHTTP_SO_SET_CALLBACK_LOAD_COMPLETE	10


// #ifdef DYNAMIC_CACHE
#define MXFILEHTTP_SO_SET_SLOTSIZE_KB			100
#define MXFILEHTTP_SO_SET_SLOTCOUNT				101
#define MXFILEHTTP_SO_SET_RANGE_TO_LINEAR_KB	102
//#endif


extern MX_INT32 mxFileHttp_setOptions( MXFILE_HANDLE handle, MX_INT32 op , void * );
extern MX_INT32 mxFileHttp_setDebugLevel( MX_INT32 level );


#define	MXFILEHTTP_GO_GET_URL			1
#define MXFILEHTTP_GO_GET_CONTENT_TYPE	2

#define MXFILEHTTP_GO_GET_GET_PROGRESS	10
#define MXFILEHTTP_SO_GET_POST_RESULT	20


#define MXFILEHTTP_SO_GET_DTCP_IP		100

__MXAPI 
extern MX_INT32 mxFileHttp_getOptions( MXFILE_HANDLE handle, MX_INT32 op , void * data);

#ifdef __cplusplus
}
#endif



#endif // __MXHTTP_H__
