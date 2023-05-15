
#ifndef __MXMMS_H__
#define __MXMMS_H__


#ifdef __cplusplus
extern "C" 
{
#endif

#include "mxfile.h"

__MXAPI MXFILE_API_SET_PTR mxFileMms_checkUrl ( MXFILE_DESC *desc );



__MXAPI extern MX_INT32 mxFileMms_setOptions( MXFILE_HANDLE handle, MX_INT32 op , void * );
__MXAPI extern MX_INT32 mxFileMms_setDebugLevel( MX_INT32 level );

#define MXFILEWMMS_GO_GET_URL			1
__MXAPI extern MX_INT32 mxFileMms_getOptions( MXFILE_HANDLE handle, MX_INT32 op , void * data);

__MXAPI extern void mxFileMms_shutdown(void);
__MXAPI extern void mxFileMms_startup(void);


#ifdef __cplusplus
}
#endif



#endif // __MXWMSP_H__
