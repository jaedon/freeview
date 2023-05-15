/** **********************************************************************************************************
	@file 		backend_log.h
	
	@date		2016/02/16
	@author		SW-browser part (kcjung@humaxdigital.com)
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


#ifndef __BACKEND_LOG_H__
#define __BACKEND_LOG_H__

#include "hlib.h"
#include <uva_debug.h>

#if defined(CONFIG_CORSAIR_LOG)
#ifndef OPERA_LOG_ENABLED
# define LOG_ENTRY              HxLOG_Debug("uva backend: Entry \n")
# define LOG_INFO(fmt, ...) \
	do { \
        std::string buf;\
        buf.append("uva backend: ");\
        buf.append(fmt);\
        HxLOG_Info(buf.c_str(),##__VA_ARGS__);\
	} while (0)

# define LOG_WARN(fmt, ...) \
	do { \
        std::string buf;\
        buf.append("uva backend: ");\
        buf.append(fmt);\
        HxLOG_Warning(buf.c_str(),##__VA_ARGS__);\
	} while (0)

# define LOG_ERROR(fmt, ...) \
	do { \
        std::string buf;\
        buf.append("uva backend: ");\
        buf.append(fmt);\
        HxLOG_Error(buf.c_str(),##__VA_ARGS__);\
	} while (0)

# define LOG_CAT(module)
# define LOG_ASSERT(expr)       HxLOG_Assert(#expr)
# define LOG_EXPECT(expr)       HxLOG_Assert(#expr)
# define LOG_DATA_APPEND(ptr, n, fname, ...)  
# define LOG_DATA_WRITE(ptr, n, fname, ...) 
#else
# define LOG_ENTRY              ENTRY
# define LOG_INFO(fmt, ...)     DEBUG_INFO(fmt, ##__VA_ARGS__)
# define LOG_WARN(fmt, ...)     DEBUG_WARN(fmt, ##__VA_ARGS__)
# define LOG_ERROR(fmt, ...)    DEBUG_ERROR(fmt, ##__VA_ARGS__)
# define LOG_CAT(module)        DEBUG_CAT(#module)
# define LOG_ASSERT(expr)       DEBUG_ASSERT(#expr)
# define LOG_EXPECT(expr)       DEBUG_EXPECT(#expr)
# define LOG_DATA_APPEND(ptr, n, fname, ...)  DEBUG_DATA_APPEND(ptr, n, fname, ##__VA_ARGS__)
# define LOG_DATA_WRITE(ptr, n, fname, ...) DEBUG_DATA_WRITE(ptr, n, fname, ##__VA_ARGS__)
#endif // OPERA_LOG_ENABLED
#else
# define LOG_ENTRY              
# define LOG_INFO(fmt, ...)     
# define LOG_WARN(fmt, ...)     
# define LOG_ERROR(fmt, ...)    
# define LOG_CAT(module)        
# define LOG_ASSERT(expr)       
# define LOG_EXPECT(expr)       
# define LOG_DATA_APPEND(ptr, n, fname, ...)
# define LOG_DATA_WRITE(ptr, n, fname, ...)
#endif // CONFIG_DEBUG

#endif // __BACKEND_LOG_H__


