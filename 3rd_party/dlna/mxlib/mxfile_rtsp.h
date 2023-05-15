
#ifndef __MXRTSP_H__
#define __MXRTSP_H__


#ifdef __cplusplus
extern "C" 
{
#endif

#include "mxfile.h"

__MXAPI MXFILE_API_SET_PTR mxFileRtsp_checkUrl ( MXFILE_DESC *desc );

__MXAPI void mxFileRtsp_shutdown(void);
__MXAPI void mxFileRtsp_startup(void);

#define MXFILERTSP_SO_SET_USER_AGENT		1

__MXAPI extern MX_INT32 mxFileRtsp_setOptions( MXFILE_HANDLE handle, MX_INT32 op , void * );
__MXAPI extern MX_INT32 mxFileRtsp_setDebugLevel( MX_INT32 level );

#define MXFILERTSP_GO_GET_CONTENT_TYPE		1

__MXAPI extern MX_INT32 mxFileRtsp_getOptions( MXFILE_HANDLE handle, MX_INT32 op , void * data);



#ifdef __cplusplus
}
#endif



#endif // __MXRTSP_H__
