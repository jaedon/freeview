/* $Header: $ */

#ifndef DLIB_PROVIDER_H__
#define DLIB_PROVIDER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <hlib.h>


typedef struct
{
	HINT32				uid;
	const HCHAR			*typeOf;
	HINT32				version;
	HCHAR				name[DxNAME_LEN];

	union {
		void *	__;
	} ex;
     HUINT32                 __CRC__;  //CHECK CRC
} DxProvider_t;



#ifdef __cplusplus
}
#endif

#endif // DLIB_PROVIDER_H__


