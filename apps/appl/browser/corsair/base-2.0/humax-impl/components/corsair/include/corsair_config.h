/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; -*-
 *
 * Copyright (C) 1995-2012 Opera Software AS.  All rights reserved.
 * May not be redistributed without prior written permission.
 *
 */

#ifndef __CORSAIR_CONFIG_H_
#define __CORSAIR_CONFIG_H_

#include "autoconf.h"

// Default screen size used when creating surfaces and
#define SCREEN_WIDTH (CONFIG_MWC_OSD_HORIZONTAL)
#define SCREEN_HEIGHT (CONFIG_MWC_OSD_VERTICAL)

// Limit the time for running Opera to 10MS.
#define MAX_OP_RUN_SLICE_TIME 10000

// Limit the amount of screen updates to no less than 30 MS /
// update. Otherwise we might be flooded during page loads, causing
// lots of extra blits without any new content to draw.
#define SCREEN_UPDATE_DELAY 30000

#define DEFAULT_TV_PORTAL_URI			"http://sqe.humaxtvportal.com"
#define DEFAULT_APPLICATION_DIR			"/usr/browser/webapps/"
#define DEFAULT_APPLICATION_PATH		DEFAULT_APPLICATION_DIR"default.html"
#define DEFAULT_APPLICATION_URI			"file://"DEFAULT_APPLICATION_PATH
#define USB_INSTALLED_APPLICATION_PATH	"/mainapp/default.html"
#define USER_INSTALLED_APPLICATION_PATH	"/mnt/hd1/apps/default.html"
#define USER_INSTALLED_APPLICATION_URI	"file://"USER_INSTALLED_APPLICATION_PATH

#define DEFAULT_ERRORPAGE_OF_CORSAIR_PATH	"/usr/browser/opera_dir/error/error.htm"
#define DEFAULT_ERRORPAGE_OF_CORSAIR_URI	"file://"DEFAULT_ERRORPAGE_OF_CORSAIR_PATH

/* the default path for corsair runtime env files */
#define DEFAULT_DRAGONFLY_NFSROOT_PATH	"/dragonfly.txt"
#define DEFAULT_DRAGONFLY_USB_PATH		"/opera/dragonfly.txt"
#define DEFAULT_PORTAL_URL_PATH			"/usr/browser/opera_dir/url/url.txt"
#define DEFAULT_USB_PORTAL_PATH			"/url/url_hbbtv.txt"
#define DEFAULT_USB_BR_URL_PATH			"/url/url_hbbtv_br.txt"
#define DEFAULT_CERT_FILE_PATH			"/usr/browser/opera_dir/certs/"
#define DEFAULT_CERT_CONFIG_FILE_PATH	"/var/lib/humaxtv/browser/cert.xml"

#define HEAP_LIMIT_SIZE					(1024*1024*250)
#define ALLOCATED_LIMIT_SIZE			(1024*1024*230)

#define CORSAIR_APPID_BASE				0x10000000
#define CORSAIR_OIPF_APPID				CORSAIR_APPID_BASE+1
#define CORSAIR_WEB_APPID				CORSAIR_APPID_BASE+2
#define CORSAIR_HBBTV_APPID				CORSAIR_APPID_BASE+3

#ifdef USE_DIRECTFBGL
#define MULTI_EGL_CONTEXT
//#define SINGLE_EGL_CONTEXT
#endif
/** Humax Internal compile options, please set usually the default value off.*/
#ifdef CONFIG_OP_JCOM
#define REMOVE_HBBTV_INPUT
#endif

#ifdef CONFIG_DEBUG
#define DSMCC_MANAGING_WITH_APPID /** to trace dsmcc working sequence */
//#define NOTIFICATION_DEBUG
//#define WINDOW_TRACE /** this is to trace gogi_window management. */
#endif
//#define ENABLE_ORIGINAL_CORSAIR

#define ENABLE_OPERA_TVBROWSER
#ifdef ENABLE_OPERA_TVSTORE
#define OPERA_TVSTORE_EXTENTION 			"Model/Humax-"
#define OPERA_TVSTORE_TESTDEVICE 			"TestingDevice"
#define RUNTIME_ENVFILE_TVSTORE_TESTPAGE	"/opera/otvs_testdevice.txt"
#endif
#ifdef CONFIG_PROD_FVP4000T
#define OPERA_FVC_EXTENTION 			"FVC/1.0"
#endif

#define ENABLE_OPERA_HBBTV

//#define ENABLE_FPS_DISPLAYER

#ifdef CONFIG_PROD_FVP4000T
#define ENABLE_ABORT_IF_OUT_OF_MEMORY
#endif

//#define
#endif //__CORSAIR_CONFIG_H_
