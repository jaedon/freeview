/* $Header: $ */

#ifndef DLIB_GROUP_H__
#define DLIB_GROUP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <hlib.h>

#define	DxGROUP_LIST_BUF_LEN					64

#ifdef	CONFIG_OP_FREESAT
typedef struct
{
	HUINT8	ucGroupType;
	HUINT8	ucNondestructiveTuneFlag;
	HUINT8	ucReturnChannelAccessFlag;
	HUINT8	szIso639LangCode[DxFSAT_LANG_CODE_LEN+1];
	HUINT8	ucG2ExtensionFlag;
	HUINT8	ucDefaultVisableFlag;
	HUINT8	ucAdultChannelsGroupFlag;
	HUINT8	ucIpChannelsGroupFlag;
	HUINT8	ucCaChannelsGroupFlag;
}DxFreesatGroup_t;
#endif

typedef struct
{
	HINT32				uid;
	const HCHAR			*typeOf;
	HINT32				version;
	HINT32				id;
	HCHAR				name[DxNAME_LEN];

	union {
#ifdef	CONFIG_OP_FREESAT
		DxFreesatGroup_t	freesat;
#else
		void *	__;
#endif
	} ex;
    HUINT32                 __CRC__;  //CHECK CRC
} DxGroup_t;


#ifdef __cplusplus
}
#endif

#endif // DLIB_EPG_H__


