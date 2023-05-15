/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:       $Workfile:   di_iptuner_config.h  $
 * Version:         $Revision:   1.0  $
 * Original Author: SungYoung Jeon
 * Current Author:  $Author: junsy@humaxdigital.com $
 * Date:            $Date:  May 20 03:08:21 KST 2009 $
 * File Description:	   IPTUNER Configurations
 * Module:
 * Remarks:
 */

/*******************************************************************/
/* Copyright (c) 2009 HUMAX Co., Ltd.                              */
/* All rights reserved.                                            */
/*******************************************************************/
#ifndef __DI_IPTUNER_CONFIG_H__
#define __DI_IPTUNER_CONFIG_H__

/******************************
 * DEBUG Option               *
 ******************************/
//#define IPTUNER_ALL_DEBUG
#if defined(CONFIG_HDD_SMART) && !defined(CONFIG_HDD_DETACHABLE)
#define USE_HDD_SMART
#endif

/******************************
 * MAX URL Length option
 ******************************/
#ifdef CONFIG_DI_MAX_URL_LENGTH
#define IPTUNER_CONST_URL_LENGTH CONFIG_DI_MAX_URL_LENGTH
#else
#ifdef DI_IPTUNER_URL_LENGTH
#define IPTUNER_CONST_URL_LENGTH	DI_IPTUNER_URL_LENGTH
#else
#error "CONFIG_DI_MAX_URL_LENGTH is not defined . Please check the config file"
#endif
#endif

/******************************
 * Real-Time Http Options
 * 		default real-time
 ******************************/
/* HDD가 있는지 없는지를 통해 RAM BUFFER를 사용할 것 인지 판단함 */
#if defined(USE_HDD_SMART) && !defined(USE_ESATA)
#define STREAM_BUFFERING 			0
#define REAL_TIME_MULTISOCKET_SUPPORT 0
#define REAL_TIME_INDEXBUFFER_SUPPORT 0
#define REAL_TIME_CHUNKBUFFER_SUPPORT 0
#else
#if USE_IPTUNER_STREAM_BUFFERING
#define STREAM_BUFFERING 			1
#define REAL_TIME_MULTISOCKET_SUPPORT 0
#define REAL_TIME_INDEXBUFFER_SUPPORT 0
#define REAL_TIME_CHUNKBUFFER_SUPPORT 0
#else
#define STREAM_BUFFERING 			0
#define REAL_TIME_MULTISOCKET_SUPPORT 1
#define REAL_TIME_INDEXBUFFER_SUPPORT 1
#define REAL_TIME_CHUNKBUFFER_SUPPORT 1
#endif
#endif 

#ifdef CONFIG_MEDIA_SSTR
#define STREAM_BUFFERING_SSTR 1
/** Seconds for Buffering */
#ifndef STREAM_SSTR_BUFFERING_SECS 
#define SSTR_CONFIG_BUFFERING_SECS (4)
#else
#define SSTR_CONFIG_BUFFERING_SECS STREAM_SSTR_BUFFERING_SECS
#endif
/** Maximum BufferSize */
#ifndef STREAM_SSTR_MAX_BUFFER
#define SSTR_CONFIG_MAX_BUFFER (5*1024*1024) 
#else
#define SSTR_CONFIG_MAX_BUFFER (STREAM_SSTR_MAX_BUFFER * 1024 * 1024)
#endif
/** Minimum BufferSize */
#ifndef STREAM_SSTR_MIN_BUFFER 
#define SSTR_CONFIG_MIN_BUFFER (2*1024*1024) 
#else
#define SSTR_CONFIG_MIN_BUFFER (STREAM_SSTR_MIN_BUFFER * 1024 * 1024) 
#endif
/* endif CONFIG_MEDIA_SSTR */
#else
#define STREAM_BUFFERING_SSTR 0
#endif

#define STREAM_BUFFERING_HLS 1
/** Seconds for Buffering */
#ifndef STREAM_HLS_BUFFERING_SEGMENTS
#define HLS_CONFIG_BUFFERING_SEGMENTS (2)
#else
#define HLS_CONFIG_BUFFERING_SEGMENTS STREAM_HLS_BUFFERING_SEGMENTS
#endif
/** Maximum BufferSize */
#ifndef STREAM_HLS_MAX_BUFFER
#define HLS_CONFIG_MAX_BUFFER (10*1024*1024) 
#else
#define HLS_CONFIG_MAX_BUFFER (STREAM_HLS_MAX_BUFFER * 1024 * 1024)
#endif
/** Minimum BufferSize */
#ifndef STREAM_HLS_MIN_BUFFER 
#define HLS_CONFIG_MIN_BUFFER (2*1024*1024) 
#else
#define HLS_CONFIG_MIN_BUFFER (STREAM_HLS_MIN_BUFFER * 1024 * 1024) 
#endif

#if STREAM_BUFFERING
#define STREAM_BUFFERING_SECS 		USE_IPTUNER_STREAM_BUFFERING
#define PREBUFFERING_BUFFER_SIZE 				(USE_IPTUNER_STREAM_BUFFERING*1024*1024)		/*  3Mbytes */
#if defined(USE_PREBUFFERING_ONOFF_THRESHOLD)
#define PREBUFFERING_ONOFF_THRESHOLD           (USE_PREBUFFERING_ONOFF_THRESHOLD*1024*1024)
#else
#define PREBUFFERING_ONOFF_THRESHOLD				(15*1024*1024)		/* 15Mbytes */
#endif
#else
#define PREBUFFERING_BUFFER_SIZE 				(3*1024*1024)		/*  3Mbytes */
#define PREBUFFERING_ONOFF_THRESHOLD           (15*1024*1024)		/* 15Mbytes */
#endif

#define __USE_VFIO__

#define SUPPORT_COOKIE 1

/******************************
 * Download Tuner Options
 * 		MAPPED_DNP
 ******************************/
#define ENABLE_MAPPED_DNP

/******************************
 * OPEN SSL Enable Option               *
 ******************************/
#if defined(CONFIG_IPTUNER_OPENSSL)
#define USE_IPTUNER_OPENSSL
#define USE_IPTUNER_OPENSSL_CA_PATH	"/usr/browser/res/for_iptuner/rootcert_1k.pem"
#endif

#ifdef USE_IPTUNER_OPENSSL
#ifndef ENABLE_IPTUNER_OPENSSL
#define ENABLE_IPTUNER_OPENSSL
#endif
#define USING_SSL_USING_PASSWORD
#endif

#if defined(ENABLE_IPTUNER_OPENSSL)
#define SUPPORT_TLS_V1_PROTOCOL
#endif
//#define SUPPORT_IPV6_TRANSPORT
#define ICY_PROTOCOL
#define SUPPORT_TITLE
#endif /* !  __DI_IPTUNER_CONFIG_H__ */
