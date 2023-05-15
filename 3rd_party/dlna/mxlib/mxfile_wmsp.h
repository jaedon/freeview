
#ifndef __MXWMSP_H__
#define __MXWMSP_H__


#ifdef __cplusplus
extern "C" 
{
#endif

#include "mxfile.h"

__MXAPI MXFILE_API_SET_PTR mxFileWmsp_checkUrl ( MXFILE_DESC *desc );

__MXAPI void mxFileWmsp_shutdown(void);
__MXAPI void mxFileWmsp_startup(void);

__MXAPI extern MX_INT32 mxFileWmsp_setOptions( MXFILE_HANDLE handle, MX_INT32 op , void * );
__MXAPI extern MX_INT32 mxFileWmsp_setDebugLevel( MX_INT32 level );

#define MXFILEWMSP_GO_IS_CONTINUEOUS_CONNECTION	0
#define MXFILEWMSP_GO_GET_CONTENT_TYPE	1
__MXAPI extern MX_INT32 mxFileWmsp_getOptions( MXFILE_HANDLE handle, MX_INT32 op , void * data);





#ifdef __cplusplus
}
#endif



#endif // __MXWMSP_H__
