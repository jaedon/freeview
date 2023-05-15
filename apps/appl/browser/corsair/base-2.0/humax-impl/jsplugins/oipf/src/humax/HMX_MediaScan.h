#if 0
// -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*-
/*
 * Copyright (C) 1995-2010 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifdef OIPF
#ifndef __HMX_MEDIA_SCAN_
#define __HMX_MEDIA_SCAN_

#include "jsplugin.h"
#include "macros.h"
#include "ObjectInstance.h"
#include "EventTarget.h"
#include "WrapperObject.h"
#include "Collection.h"
#include "NativeHmxMediaScan.h"


namespace Ooif
{
	class HMX_MediaScan : public EventTarget
	{
		private:			
			static void * m_sScanningIdentifier;
		public:
			HMX_MediaScan();
			~HMX_MediaScan();
			int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
			int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
			static int mediaBrowse(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
			static int dirBrowse(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
			static int getVideoPlayPosition(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
			static int getVideoDuration(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
			static int getVideoThumbnailUrl(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
			static int getVideoWatched(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
			// 추후 제거할 api
			static int fileOpDo(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
			static int isFileExist(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
			static int isDirExist(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
			static int isCopying(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
			static int isDeleting(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
			static int getSpaceInfo(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
			// 
			static void onMediaContentSearch(void ** id, int size, unsigned int state);
			static void onDirectorySearch(void ** id, int size, unsigned int state);
			static const ClassType class_name = CLASS_HMX_MEDIASCAN; 
	};

	class HMX_MediaContent : public ObjectInstance, public WrapperObject<HMX_MediaContent>
	{
		private:
		public:			
			HMX_MediaContent( void * id );
			HMX_MediaContent( const HMX_MediaContent &original);
			virtual ~HMX_MediaContent();
			
			int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result );
			int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
			static void releaseNative( void * id );
			static const ClassType class_name = CLASS_HMX_MEDIACONTENT; 
	};

	class HMX_MediaScanningResultEvent : public Event
	{

	private :
		void ** contentList;
		int m_nState;
		int m_nHandleCount;
	public :
		HMX_MediaScanningResultEvent( jsplugin_obj *c,	void **handleId, int size, int state );
		~HMX_MediaScanningResultEvent(); 

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		virtual int serialize(jsplugin_value **argv);
	};

	class HMX_DirScanningResultEvent : public Event
	{

	private :
		void ** contentList;
		int m_nState;
		int m_nHandleCount;
	public :
		HMX_DirScanningResultEvent( jsplugin_obj *c,	void **handleId, int size, int state );
		~HMX_DirScanningResultEvent(); 

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		virtual int serialize(jsplugin_value **argv);
	};



	class HMX_MediaDoFile : public EventTarget
	{
		private:			
			static void * m_sDoFileIdentifier;
		public:
			HMX_MediaDoFile();
			~HMX_MediaDoFile();
			int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
			int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
			static void onFileOpComplete(unsigned int type, int nDevIndex, unsigned int state);
			static const ClassType class_name = CLASS_HMX_MEDIADOFILE; 
	};

	class HMX_FileOpResultEvent : public Event
	{

	private :
		int m_nState;	// success, partially success(fail), all fail
		int m_nType;	// Delete, Rename, Copy
	public :
		HMX_FileOpResultEvent( jsplugin_obj *c, int type, int state );
		~HMX_FileOpResultEvent(); 

		int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
		int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
		virtual int serialize(jsplugin_value **argv);
	};

	class HMX_MediaFileUtil : public EventTarget
	{
		private:			
			static void * m_sFileUtilIdentifier;
		public:
			HMX_MediaFileUtil();
			~HMX_MediaFileUtil();
			int getter(jsplugin_obj *obj, const char *name, jsplugin_value *result);
			int setter(jsplugin_obj *obj, const char *name, jsplugin_value *value);
			static int fileOpDo(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
			static int isFileExist(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
			static int isDirExist(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
			static int isCopying(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
			static int isDeleting(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
			static int getSpaceInfo(jsplugin_obj *this_obj, jsplugin_obj *function_obj, int argc, jsplugin_value *argv, jsplugin_value *result);
			static const ClassType class_name = CLASS_HMX_MEDIAFILEUTIL; 
	};



} /* namespace Ooif */

#endif // __HMX_MEDIA_SCAN_
#endif // OIPF
#endif
