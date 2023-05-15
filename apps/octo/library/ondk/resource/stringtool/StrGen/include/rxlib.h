/*************************************************************
 * @file		hlib.h
 * @date		2011/01/17
 * @author	hwang ji yeon(jyhwang@humaxdigital.com)
 * @brief		HUMAX common library header.
 *
 * http://www.humaxdigital.com
 *************************************************************/

#ifndef _RXLIB_H_
#define _RXLIB_H_

#ifdef __cplusplus
extern "C" {
#endif


/* include generic files */
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#include 	<ctype.h>
#include 	<stddef.h>


#if defined(OS_LINUX)
#include	<getopt.h>
#endif


#define RxLOG_Assert
#define	RxLOG_Print
#define	RxLOG_Error
#define	RxLOG_Critical
#define	RxLOG_Warning
#define	RxLOG_Message
#define	RxLOG_Info
#define	RxLOG_Debug

/* include di files */

#include 	<rxtype.h>
#include	<rxstdlib.h>



#include <rxcodepage.h>
#include <rxfile.h>
#include <rxdir.h>
#include <rxvector.h>
#include <rxlist.h>
#include <rxhash.h>
#include <rxlang.h>


typedef 	HINT64			HOffset_t;
typedef 	HUINT64			HSize_t;

#if defined(CONFIG_DEBUG)
#define		Rx_Trace
#define		Rx_Debug					printf
#define		Rx_Print					printf
#define		Rx_Error					printf
#define		Rx_GetCh					getch
#define		Rx_NULL_PRINT				while (0) ((int (*)(char *, ...)) 0)
#else
#define		Rx_Trace
#define		Rx_Debug
#define		Rx_Print					printf
#define		Rx_Error					printf
#define		Rx_GetCh
#define		Rx_NULL_PRINT				while (0) ((int (*)(char *, ...)) 0)
#endif /* defined(CONFIG_DEBUG) */



#ifdef __cplusplus
}
#endif


#endif /* _RXLIB_H_ */

