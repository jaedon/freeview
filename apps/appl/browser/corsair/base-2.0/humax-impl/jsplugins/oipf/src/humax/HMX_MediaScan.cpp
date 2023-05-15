#if 0
// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2010 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OIPF
#include "HMX_MediaScan.h"

#include "EmptyEvent.h"
#include "Collection.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsplugin.h"

#include "macros.h"

//#include "oplhmxmediascan.h"

#define	MAX_BUF_LEN 32

typedef enum
{
	eMEDIA_CONTENT_END = 0,
	eMEDIA_CONTENT_ADD
} MEDIA_CONTENT_Update_e;


using namespace Ooif;


void * HMX_MediaScan::m_sScanningIdentifier = NULL;
HMX_MediaScan::HMX_MediaScan( ) : EventTarget()
{
	ENTRY;
//	OOIF_LOG_PRINT("\n######################################\n");
//	OOIF_LOG_PRINT("\n### HMX_MediaScan() create ###\n");
//	OOIF_LOG_PRINT("\n######################################\n");
	classType = CLASS_HMX_MEDIASCAN;
}

HMX_MediaScan::~HMX_MediaScan()
{
//	OOIF_LOG_PRINT("\n######################################\n");
//	OOIF_LOG_PRINT("\n#### ~HMX_MediaScan() ########\n");
//	OOIF_LOG_PRINT("\n######################################\n");
}

int HMX_MediaScan::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;
	INTRINSIC_EVENT_GET(MediaContentSearch);
	INTRINSIC_EVENT_GET(DirectorySearch);
	GET_FUNCTION(mediaBrowse, obj, "snns", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(dirBrowse, obj, "snns", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getVideoPlayPosition, obj, "s", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getVideoDuration, obj, "s", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getVideoThumbnailUrl, obj, "s", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getVideoWatched, obj, "s", JSP_GET_VALUE_CACHE);
	// 추후 제거할 api
	GET_FUNCTION(fileOpDo, obj, "nnnsss", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(isFileExist, obj, "s", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(isDirExist, obj, "s", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(isCopying, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(isDeleting, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getSpaceInfo, obj, "s", JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int HMX_MediaScan::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

	if ( !strcmp("onMediaContentSearch",name ) )
	{
		m_sScanningIdentifier = this;
		native_HMX_MediaScanningResultListener ( (scanListener)&(HMX_MediaScan::onMediaContentSearch) );
		INTRINSIC_EVENT(MediaContentSearch);
	}
	if ( !strcmp("onDirectorySearch",name ) )
	{
		m_sScanningIdentifier = this;
		native_HMX_DirScanningResultListener ( (scanListener)&(HMX_MediaScan::onDirectorySearch) );
		INTRINSIC_EVENT(DirectorySearch);
	}

	return JSP_PUT_NOTFOUND;
}

void HMX_MediaScan::onMediaContentSearch(void **id, int size, unsigned int event)
{
	ENTRY;

	HMX_MediaScan *Scan = (HMX_MediaScan *)m_sScanningIdentifier;
	if ( NULL == Scan )
	{
		return;
	}

	jsplugin_obj *context = Scan->getHost();
	OOIF_LOG_DEBUG("debug  ", "\n[%s,%d] ################### size=%d event=%d Scan=%x context=%x#################", __FUNCTION__, __LINE__ , size, event, Scan, context);
	HMX_MediaScanningResultEvent *e;
	e = new HMX_MediaScanningResultEvent(context, id, size, event);//"AccessPointUpdated"
	Scan->dispatchEvent(e);
}

void HMX_MediaScan::onDirectorySearch(void **id, int size, unsigned int event)
{
	ENTRY;

	HMX_MediaScan *Scan = (HMX_MediaScan *)m_sScanningIdentifier;
	if ( NULL == Scan )
	{
		return;
	}

	jsplugin_obj *context = Scan->getHost();
	OOIF_LOG_DEBUG("debug  ", "\n[%s,%d] ################### size=%d event=%d Scan=%x context=%x#################", __FUNCTION__, __LINE__ , size, event, Scan, context);
	HMX_DirScanningResultEvent *e;
	e = new HMX_DirScanningResultEvent(context, id, size, event);//"AccessPointUpdated"
	Scan->dispatchEvent(e);
}

//static
int	HMX_MediaScan::mediaBrowse(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(4);
	int retval = 0;

	native_HMX_MediaBrowse(STRING_VALUE(argv[0]), NUMBER_VALUE(argv[1]), NUMBER_VALUE(argv[2]), STRING_VALUE(argv[3]), &retval );
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}

//static
int	HMX_MediaScan::dirBrowse(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(4);
	int retval = 0;

	native_HMX_DirBrowse(STRING_VALUE(argv[0]), NUMBER_VALUE(argv[1]), NUMBER_VALUE(argv[2]), STRING_VALUE(argv[3]), &retval );
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}


// 추후 제거할 api
//static
int	HMX_MediaScan::fileOpDo(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(6);
	int retval = 0;
	OOIFNumber type = NUMBER_VALUE(argv[0]);
	OOIFNumber syncMode = NUMBER_VALUE(argv[1]);
	OOIFNumber urlCount = NUMBER_VALUE(argv[2]);
	const char* szURLs = STRING_VALUE(argv[3]);
	const char* szRename = STRING_VALUE(argv[4]);
	const char* szTarget = STRING_VALUE(argv[5]);

	// type, sync_mode, url_count, urls, rename, target_dir
	native_HMX_FileOpDo(type, syncMode, urlCount, szURLs, szRename, szTarget, &retval );
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}


//static
int	HMX_MediaScan::isFileExist(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(1);
	int retval = 0;
	const char* szURLs = STRING_VALUE(argv[0]);

	native_HMX_IsFileExist(szURLs, &retval );
	OOIF_LOG_DEBUG("debug  ", "\n[%s,%d] ################### %s %d #################", __FUNCTION__, __LINE__ , szURLs, retval);
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}


//static
int	HMX_MediaScan::isDirExist(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(1);
	int retval = 0;
	const char* szURLs = STRING_VALUE(argv[0]);

	native_HMX_IsDirExist(szURLs, &retval );
	OOIF_LOG_DEBUG("debug  ", "\n[%s,%d] ################### %s %d #################", __FUNCTION__, __LINE__ , szURLs, retval);
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}


//static
int	HMX_MediaScan::isCopying(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	int retval = 0;

	native_HMX_IsCopying(&retval );
	OOIF_LOG_DEBUG("debug  ", "\n[%s,%d] ################### %d #################", __FUNCTION__, __LINE__ , retval);
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}


//static
int	HMX_MediaScan::isDeleting(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	int retval = 0;

	native_HMX_IsDeleting(&retval );
	OOIF_LOG_DEBUG("debug  ", "\n[%s,%d] ###################  %d #################", __FUNCTION__, __LINE__ , retval);
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}


//static
int	HMX_MediaScan::getSpaceInfo(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(1);
	const char *retval;
	const char* szURLs = STRING_VALUE(argv[0]);

	native_HMX_GetSpaceInfo(szURLs, &retval );
	OOIF_LOG_DEBUG("debug  ", "\n[%s,%d] ################### %s %s #################", __FUNCTION__, __LINE__ , szURLs, retval);
	RETURN_STRING(retval, JSP_CALL_VALUE);
}


//static
int	HMX_MediaScan::getVideoThumbnailUrl(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(1);
	const char *retval;
	const char* szURLs = STRING_VALUE(argv[0]);

	native_HMX_GetVideoThumbnailUrl(szURLs, &retval );
	OOIF_LOG_DEBUG("debug  ", "\n[%s,%d] ################### %s %s #################", __FUNCTION__, __LINE__ , szURLs, retval);
	RETURN_STRING(retval, JSP_CALL_VALUE);
}


//static
int	HMX_MediaScan::getVideoDuration(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(1);
	const char *retval;
	const char* szURLs = STRING_VALUE(argv[0]);

	native_HMX_GetVideoDuration(szURLs, &retval );
	OOIF_LOG_DEBUG("debug  ", "\n[%s,%d] ################### %s %s #################", __FUNCTION__, __LINE__ , szURLs, retval);
	RETURN_STRING(retval, JSP_CALL_VALUE);
}


//static
int	HMX_MediaScan::getVideoPlayPosition(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(1);
	const char *retval;
	const char* szURLs = STRING_VALUE(argv[0]);

	native_HMX_GetVideoPlayPosition(szURLs, &retval );
	OOIF_LOG_DEBUG("debug  ", "\n[%s,%d] ################### %s %s #################", __FUNCTION__, __LINE__ , szURLs, retval);
	RETURN_STRING(retval, JSP_CALL_VALUE);
}


//static
int	HMX_MediaScan::getVideoWatched(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(1);
	const char *retval;
	const char* szURLs = STRING_VALUE(argv[0]);

	native_HMX_GetVideoWatched(szURLs, &retval );
	OOIF_LOG_DEBUG("debug  ", "\n[%s,%d] ################### %s %s #################", __FUNCTION__, __LINE__ , szURLs, retval);
	RETURN_STRING(retval, JSP_CALL_VALUE);
}



/***********************************************************************************
 *  HMX_MediaContent Class
 ***********************************************************************************
 */

HMX_MediaContent::HMX_MediaContent( void *id ) : WrapperObject<HMX_MediaContent>(id)
{
	classType = CLASS_HMX_MEDIACONTENT;
}

HMX_MediaContent::HMX_MediaContent( const HMX_MediaContent &original) : WrapperObject<HMX_MediaContent>(original.identifier)
{
	classType = CLASS_HMX_MEDIACONTENT;

}

HMX_MediaContent::~HMX_MediaContent()
{
	ENTRY;
}

//static
void HMX_MediaContent::releaseNative( void *id )
{
	NATIVE_WARN_NOK(native_HMX_MediaContentReleaseHandle(id));
}



int HMX_MediaContent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	NATIVE_GET_NUMBER("fileType", native_HMX_MediaContentGetFileType(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("fullPath", native_HMX_MediaContentGetFullPath(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("hasSubitem", native_HMX_MediaContentGetHasSubitem(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("date", native_HMX_MediaContentGetDate(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("duration", native_HMX_MediaContentGetDuration(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("playPosition", native_HMX_MediaContentGetPlayPosition(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("largeThumbnailUrl", native_HMX_MediaContentGetLargeThumbnailUrl(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("thumbnailUrl", native_HMX_MediaContentGetThumbnailUrl(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("resolution", native_HMX_MediaContentGetPicureResolution(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("thumbResolution", native_HMX_MediaContentGetThumbResolution(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("pictureDate", native_HMX_MediaContentGetPictureDate(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("album", native_HMX_MediaContentGetMusicAlbum(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_STRING("artist", native_HMX_MediaContentGetMusicArtist(identifier, &RETVAL), JSP_GET_VALUE);
	NATIVE_GET_NUMBER("watched", native_HMX_MediaContentGetWatched(identifier, &RETVAL), JSP_GET_VALUE);

	return JSP_GET_NOTFOUND;
}

int HMX_MediaContent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

	return JSP_PUT_NOTFOUND;
}

/***********************************************************************************
 *  HMX_MediaScanningResultEvent Class
 ***********************************************************************************
 */
HMX_MediaScanningResultEvent::HMX_MediaScanningResultEvent( jsplugin_obj *c, void **handleId, int size, int state ) : Event( c, "MediaContentSearch"), contentList( handleId ), m_nState( state ), m_nHandleCount(size)
{
	ENTRY;
	//OOIF_LOG_PRINT("\n[%s,%d] ########################  state : %d, %x HMX_MediaScanningResultEvent #######################\n", __FUNCTION__, __LINE__, state, contentList );
}

HMX_MediaScanningResultEvent::~HMX_MediaScanningResultEvent()
{
	ENTRY;
}

int HMX_MediaScanningResultEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	OOIF_LOG_DEBUG("debug  ", "\n[%s,%d] ########################  name : %s, HMX_MediaScanningResultEvent #######################\n", __FUNCTION__, __LINE__, name );
	return JSP_GET_NOTFOUND;
}

int HMX_MediaScanningResultEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	return JSP_PUT_NOTFOUND;
}

//virtual
int HMX_MediaScanningResultEvent::serialize(jsplugin_value **argv)
{
	int count = m_nHandleCount;
	ENTRY;
	OOIF_LOG_DEBUG("debug  ", "(*) HMX_MediaScanningResultEvent() Class serialize\n");
	*argv = new jsplugin_value[1];
	jsplugin_value *result = *argv;

	if ( eMEDIA_CONTENT_END == m_nState )
	{
		return 1;
	}

	HMX_MediaContentCollection *collection = new HMX_MediaContentCollection;
	HMX_MediaContent *content = NULL;
	for ( int i = 0; i< count; i++ )
	{
		content = new HMX_MediaContent( contentList[i] );
		collection->addItem( content  );
	}

	//SET_SERIALIZED_OBJECT( new HMX_DLNAContent(m_handleId, m_metaHandleId), 0 );
	SET_SERIALIZED_OBJECT( collection, 0 );
	if ( NULL != contentList )
	{
		HLIB_STD_MemFree( contentList );
		contentList = NULL;
	}
	return 1;
}

/***********************************************************************************
 *  HMX_DirScanningResultEvent Class
 ***********************************************************************************
 */
HMX_DirScanningResultEvent::HMX_DirScanningResultEvent( jsplugin_obj *c, void **handleId, int size, int state ) : Event( c, "DirectorySearch"), contentList( handleId ), m_nState( state ), m_nHandleCount( size )
{
	ENTRY;
	//OOIF_LOG_PRINT("\n[%s,%d] ########################  state : %d, %x HMX_DirScanningResultEvent #######################\n", __FUNCTION__, __LINE__, state, content );
}

HMX_DirScanningResultEvent::~HMX_DirScanningResultEvent()
{
	ENTRY;
}

int HMX_DirScanningResultEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
//	OOIF_LOG_PRINT("\n[%s,%d] ########################  name : %s, HMX_DirScanningResultEvent #######################\n", __FUNCTION__, __LINE__, name );
	return JSP_GET_NOTFOUND;
}

int HMX_DirScanningResultEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	return JSP_PUT_NOTFOUND;
}

//virtual
int HMX_DirScanningResultEvent::serialize(jsplugin_value **argv)
{
	int count = m_nHandleCount;
	ENTRY;
	//OOIF_LOG_DEBUG("debug  ", "(*) HMX_MediaScanningResultEvent() Class serialize\n");
	*argv = new jsplugin_value[1];
	jsplugin_value *result = *argv;

	if ( eMEDIA_CONTENT_END == m_nState )
	{
		return 1;
	}

	HMX_MediaContentCollection *collection = new HMX_MediaContentCollection;
	HMX_MediaContent *content = NULL;
	for ( int i = 0; i< count; i++ )
	{
		content = new HMX_MediaContent( contentList[i] );
		collection->addItem( content  );
	}

	//SET_SERIALIZED_OBJECT( new HMX_DLNAContent(m_handleId, m_metaHandleId), 0 );
	SET_SERIALIZED_OBJECT( collection, 0 );
	if ( NULL != contentList )
	{
		HLIB_STD_MemFree( contentList );
		contentList = NULL;
	}
	return 1;
}




void * HMX_MediaDoFile::m_sDoFileIdentifier = NULL;
HMX_MediaDoFile::HMX_MediaDoFile( ) : EventTarget()
{
	ENTRY;
//	OOIF_LOG_PRINT("\n######################################\n");
//	OOIF_LOG_PRINT("\n### HMX_MediaDoFile() create ###\n");
//	OOIF_LOG_PRINT("\n######################################\n");
	classType = CLASS_HMX_MEDIADOFILE;
	m_sDoFileIdentifier = this;
	native_HMX_FileOpResultListener ( (fileOpListener)&(HMX_MediaDoFile::onFileOpComplete) );
}

HMX_MediaDoFile::~HMX_MediaDoFile()
{
//	OOIF_LOG_PRINT("\n######################################\n");
//	OOIF_LOG_PRINT("\n#### ~HMX_MediaDoFile() ########\n");
//	OOIF_LOG_PRINT("\n######################################\n");
}

int HMX_MediaDoFile::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	INTRINSIC_EVENT_GET(FileOpComplete);
	GET_FUNCTION(addEventListener, obj, "sos", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(removeEventListener, obj, "sos", JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int HMX_MediaDoFile::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

	INTRINSIC_EVENT(FileOpComplete);

	return JSP_PUT_NOTFOUND;
}

void HMX_MediaDoFile::onFileOpComplete(unsigned int type, int nDevCount, unsigned int event)
{
	ENTRY;

	HMX_MediaDoFile *DoFile = (HMX_MediaDoFile *)m_sDoFileIdentifier;
	if ( NULL == DoFile )
	{
		return;
	}

	jsplugin_obj *context = DoFile->getHost();
	OOIF_LOG_DEBUG("debug  ", "\n[%s,%d] ################### type=%d event=%d context=%x#################", __FUNCTION__, __LINE__ , type, event, context);
	HMX_FileOpResultEvent *e = NULL;
	e = new HMX_FileOpResultEvent(context, type, event);//"AccessPointUpdated"
	DoFile->dispatchEvent(e);

}


/***********************************************************************************
 *  HMX_FileOpResultEvent Class
 ***********************************************************************************
 */
HMX_FileOpResultEvent::HMX_FileOpResultEvent( jsplugin_obj *c, int type, int state ) : Event( c, "FileOpComplete"), m_nState( state ), m_nType( type )
{
	ENTRY;
	OOIF_LOG_DEBUG("debug  ", "\n[%s,%d] ########################  type : %d, state : %d, HMX_FileOpResultEvent #######################\n", __FUNCTION__, __LINE__, type, state );
}

HMX_FileOpResultEvent::~HMX_FileOpResultEvent()
{
	ENTRY;
}

int HMX_FileOpResultEvent::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	OOIF_LOG_DEBUG("debug  ", "\n[%s,%d] ########################  name : %s, HMX_FileOpResultEvent #######################\n", __FUNCTION__, __LINE__, name );
	return JSP_GET_NOTFOUND;
}

int HMX_FileOpResultEvent::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	return JSP_PUT_NOTFOUND;
}

//virtual
int HMX_FileOpResultEvent::serialize(jsplugin_value **argv)
{
	OOIF_LOG_DEBUG("debug  ", "\n[%s,%d] ########################  [serialize] HMX_FileOpResultEvent #######################\n", __FUNCTION__, __LINE__ );
	*argv = new jsplugin_value[2];
	jsplugin_value *result = *argv;
	result[0].type = JSP_TYPE_NUMBER;
	result[0].u.number = m_nType;
	result[1].type = JSP_TYPE_NUMBER;
	result[1].u.number = m_nState;

	return 2;
}



/***********************************************************************************
 *  HMX_MediaFileUtil Class
 ***********************************************************************************
 */
void * HMX_MediaFileUtil::m_sFileUtilIdentifier = NULL;
HMX_MediaFileUtil::HMX_MediaFileUtil( ) : EventTarget()
{
	ENTRY;
//	OOIF_LOG_PRINT("\n######################################\n");
//	OOIF_LOG_PRINT("\n### HMX_MediaFileUtil() create ###\n");
//	OOIF_LOG_PRINT("\n######################################\n");
	classType = CLASS_HMX_MEDIAFILEUTIL;
	m_sFileUtilIdentifier = this;
}

HMX_MediaFileUtil::~HMX_MediaFileUtil()
{
//	OOIF_LOG_PRINT("\n######################################\n");
//	OOIF_LOG_PRINT("\n#### ~HMX_MediaFileUtil() ########\n");
//	OOIF_LOG_PRINT("\n######################################\n");
}

int HMX_MediaFileUtil::getter(jsplugin_obj *obj, const char *name, jsplugin_value *result)
{
	ENTRY;

	GET_FUNCTION(fileOpDo, obj, "nnnsss", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(isFileExist, obj, "s", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(isDirExist, obj, "s", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(isCopying, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(isDeleting, obj, "", JSP_GET_VALUE_CACHE);
	GET_FUNCTION(getSpaceInfo, obj, "s", JSP_GET_VALUE_CACHE);

	return JSP_GET_NOTFOUND;
}

int HMX_MediaFileUtil::setter(jsplugin_obj *obj, const char *name, jsplugin_value *value)
{
	ENTRY;

	return JSP_PUT_NOTFOUND;
}


//static
int	HMX_MediaFileUtil::isFileExist(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(1);
	int retval = 0;
	const char* szURLs = STRING_VALUE(argv[0]);

	native_HMX_IsFileExist(szURLs, &retval );
	OOIF_LOG_DEBUG("debug  ", "\n[%s,%d] ################### %s %d #################", __FUNCTION__, __LINE__ , szURLs, retval);
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}


//static
int	HMX_MediaFileUtil::isDirExist(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(1);
	int retval = 0;
	const char* szURLs = STRING_VALUE(argv[0]);

	native_HMX_IsDirExist(szURLs, &retval );
	OOIF_LOG_DEBUG("debug  ", "\n[%s,%d] ################### %s %d #################", __FUNCTION__, __LINE__ , szURLs, retval);
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}


//static
int	HMX_MediaFileUtil::isCopying(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	int retval = 0;

	native_HMX_IsCopying(&retval );
	OOIF_LOG_DEBUG("debug  ", "\n[%s,%d] ################### %d #################", __FUNCTION__, __LINE__ , retval);
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}


//static
int	HMX_MediaFileUtil::isDeleting(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	int retval = 0;

	native_HMX_IsDeleting(&retval );
	OOIF_LOG_DEBUG("debug  ", "\n[%s,%d] ###################  %d #################", __FUNCTION__, __LINE__ , retval);
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}


//static
int	HMX_MediaFileUtil::getSpaceInfo(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(1);
	const char *retval;
	const char* szURLs = STRING_VALUE(argv[0]);

	native_HMX_GetSpaceInfo(szURLs, &retval );
	OOIF_LOG_DEBUG("debug  ", "\n[%s,%d] ################### %s %s #################", __FUNCTION__, __LINE__ , szURLs, retval);
	RETURN_STRING(retval, JSP_CALL_VALUE);
}


//static
int	HMX_MediaFileUtil::fileOpDo(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result)
{
	ENTRY;
	CHECK_ARGUMENT_COUNT(6);
	int retval = 0;
	OOIFNumber type = NUMBER_VALUE(argv[0]);
	OOIFNumber syncMode = NUMBER_VALUE(argv[1]);
	OOIFNumber urlCount = NUMBER_VALUE(argv[2]);
	const char* szURLs = STRING_VALUE(argv[3]);
	const char* szRename = STRING_VALUE(argv[4]);
	const char* szTarget = STRING_VALUE(argv[5]);

	// type, sync_mode, url_count, urls, rename, target_dir
	native_HMX_FileOpDo(type, syncMode, urlCount, szURLs, szRename, szTarget, &retval );
	RETURN_BOOLEAN(retval, JSP_CALL_VALUE);
}


#endif // OIPF
#endif
