/*************************************************************************************************************
	File            : define.h
	author          : IPM tipmsw1@humaxdigital.com
	comment         :
	date            :
	attention       :

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. (Humax) and
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
	non-assignable, non-transferable and non-exclusive license to use this Software.
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
	you agree to the responsibility to take all reasonable efforts to protect the any information
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
	reverse engineer or extract the source code of this Software unless you have Humax's written permission
	to do so.
	If you have no authorized license, discontinue using this Software immediately.

	THE SOFTWARE IS PROVIDED ¡°AS IS¡± AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
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
*************************************************************************************************************/

#ifndef	__DEF_H__
#define	__DEF_H__

#include <hlib.h>

#define HOM_MEMCOPY(d,s,n)			HxSTD_MemCopy(d,s,n)
#define	HOM_MEMALLOC(x)				HLIB_STD_MemAlloc(x)
#define	HOM_MEMSET(a,b,c)			HxSTD_MemSet(a,b,c)
#define	HOM_MEMFREE(x)				{if(x) {	HLIB_STD_MemFree(x); 	x = NULL; 	}}

#define	HOM_STRSTR					HxSTD_StrStr
#define HOM_STRLEN					HxSTD_StrLen
#define HOM_STRNCMP					HxSTD_StrNCmp
#define HOM_STRCHAR					HxSTD_StrChar
#define HOM_SNPRINTF				HxSTD_snprintf
#define HOM_STRTOINT32				HLIB_STD_StrToINT32
#define HOM_STRNCPY_SAFE			HLIB_STD_StrNCpySafe

#define HOM_FUNC_ENTER 				HxLOG_Debug("(++)Enter!!\n")
#define HOM_FUNC_LEAVE				HxLOG_Debug("(--)Leave!!\n")

#define HOM_MEMZERO(target,size) 	HxSTD_MemSet((void *)(target), 0, (size_t)(size))

#define HOM_ASSERT(x)  		\
	    if ( x == NULL )    \
    	{\
		    HxLOG_Error("[ %s:%d ] : the param is NULL.\n", __FUNCTION__, __LINE__);\
		    return ERR_FAIL;\
	    }\

#define HOM_JSON_DEBUG(x)	\
		{\
     		HxLOG_Debug("[ %s:%d ] : json Param  - %s\n", __FUNCTION__, __LINE__, HLIB_JSON_Encode((const HxJSON_t)x, NULL, 0));\
		}

#endif	 /* __DEF_H__ */
/* end od file */

