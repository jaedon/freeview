/* $Header: $ */

#ifndef DLIB_BOUQUET_H__
#define DLIB_BOUQUET_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <hlib.h>

#if defined(CONFIG_DB_SVC_SVCBOUQUET_MULTILANG_NAME)
typedef struct
{
	HUINT8				szLangCode[DxMAX_BOUQ_LANG_LEN];		// iso639
	HUINT8				szBouqName[DxMAX_BOUQ_NAME_LEN];
}DxMultiBouqName_t;
#endif

typedef struct
{
	HINT32				uid;
	const HCHAR			*typeOf;
	HINT32				version;
	HINT32				id;
	HCHAR				name[DxNAME_LEN];

	union {
#if defined(CONFIG_DB_SVC_SVCBOUQUET_MULTILANG_NAME)
		DxMultiBouqName_t	astMultiBouqName[DxMAX_MULTI_BOUQ_NAME_NUM];
#else
		void *	__;
#endif
	} ex;
    HUINT32                 __CRC__;  //CHECK CRC
} DxBouquet_t;

#ifdef __cplusplus
}
#endif

#endif // DLIB_BOUQUET_H__


