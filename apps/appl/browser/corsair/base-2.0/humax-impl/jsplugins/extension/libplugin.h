/** **********************************************************************************************************
	@file 		libplugin.h
	
	@date		2013/11/17
	@author		ssyoo@humaxdigital.com)
	@breif		
	
	(c) 2011-2012 Humax Co., Ltd. 
	This program is produced by Humax Co., Ltd. ("Humax") and 
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee, 
	non-assignable, non-transferable and non-exclusive license to use this Software. 
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software 
	you agree to the responsibility to take all reasonable efforts to protect the any information 
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and 
	reverse engineer or extract the source code of this Software unless you have Humax's written permission 
	to do so.
	If you have no authorized license, discontinue using this Software immediately. 
	
	THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, 
	EITHER EXPRESS, IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE. 
	IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR 
	INDIRECT, SPECIAL, CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR 
	INABILITY TO USE THE SOFTWARE.
	
	This License is effective until terminated. You may terminate this License at any time 
	by destroying all copies of the Software including all documentation. This License will terminate 
	immediately without notice from Humax to you if you fail to comply with any provision of this License. 
	Upon termination, you must destroy all copies of the Software and all documentation.  
	
	The laws of the Republic of Korea will apply to any disputes arising out of or relating to 
	this Copyright Notice. All claims arising out of or relating to this Copyright Notice will be litigated
	in the Seoul Central District Court, in the Republic of Korea.
*********************************************************************************************************** */

#ifndef __LIBPLUGIN_H__
#define __LIBPLUGIN_H__

#if defined(CONFIG_DEBUG)
#define HMX_LOG_ENABLED
#define PLUGIN_DEBUG
#endif

#ifdef PLUGIN_DEBUG
#ifdef HMX_LOG_ENABLED
# define PLUGIN_Print(fmt, ...) \
	do { \
        std::string buf;\
        buf.append("extension plugin: ");\
        buf.append(fmt);\
        HxLOG_Print(buf.c_str(),##__VA_ARGS__);\
	} while (0)
# define PLUGIN_FPrint(fmt, ...) \
    do { \
        std::string buf;\
        buf.append("extension plugin: ");\
        buf.append(fmt);\
        HxLOG_Print(buf.c_str(),##__VA_ARGS__);\
    } while (0)
#else
#define PLUGIN_Print(x,...)	    printf("\033[37;44m");printf("[%-32s]", __FUNCTION__); printf (x,__VA_ARGS__);printf("\033[0m");
#define PLUGIN_FPrint(x,...)	fprintf(stderr,"\033[37;44m");fprintf(stderr,"[%-32s]", __FUNCTION__); fprintf (stderr,x,__VA_ARGS__);fprintf(stderr,"\033[0m");
#endif // HMX_LOG_ENABLED
#else
#define PLUGIN_Print(x,...)
#define PLUGIN_FPrint(x,...)
#endif // PLUGIN_DEBUG

#endif
