


#ifndef __MXUSER_H__
#define __MXUSER_H__


#ifdef __cplusplus
extern "C"
{
#endif

#include "mxfile.h"

typedef MX_UINT32 (*user_open) ( const char *pathname, MX_UINT32 flags, MX_UINT32 *filedes, const char *clientIP);
typedef MX_INT32 (*user_read) (MX_UINT32 filedes, MX_UINT8 *buf, MX_UINT32 size, MX_UINT32 *sizep);
typedef void (*user_close)(MX_UINT32 filedes);
typedef MX_INT64 (*user_seek) (MX_UINT32 filedes, MX_INT64 offset, MX_UINT32 whence);
typedef MX_INT32 (*user_tell) (MX_UINT32 filedes, MX_UINT64 *poffset);
typedef MX_INT32 (*user_get_filesize)(MX_UINT32 filedes, MX_INT64 *pulSize);
typedef MX_INT32 (*user_is_eof) (MX_UINT32 filedes);
typedef MX_INT32 (*user_is_userfile) (const char *pathname , MX_INT32 *isUserFile );

__MXAPI MXFILE_API_SET_PTR mxFileUser_checkUser ( MXFILE_DESC *desc );

__MXAPI void mxFileUser_shutdown(void);
__MXAPI void mxFileUser_startup(void);

__MXAPI MX_INT32 mxFileUser_SetApi( user_is_userfile, user_open, user_close, user_read, user_seek,
	user_tell, user_get_filesize, user_is_eof );


#ifdef __cplusplus
}
#endif



#endif // __MXUSER_H__
