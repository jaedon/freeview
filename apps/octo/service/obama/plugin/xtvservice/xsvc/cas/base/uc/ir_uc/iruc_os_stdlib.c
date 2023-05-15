/*
 * (c) 2011-2012 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

#include <octo_common.h>
#include "UniversalClient_Stdlib.h"

#define IRUC_OS_DBG_PRINTBUF_SIZE (1*512)

uc_uint32 UniversalClient_Stdlib_strlen(const uc_char *pString)
{
	HxLOG_Info("\n");
	return HxSTD_StrLen(pString);
}

/**
 * CRT strcpy function
 */
uc_char *UniversalClient_Stdlib_strcpy(uc_char *pDest, const uc_char *pSource)
{
	HxLOG_Info("\n");
	return HxSTD_StrCpy(pDest, pSource);
}

/**
 * CRT memcpy function
 */
void * UniversalClient_Stdlib_memcpy(void *pDest, const void *pSource, uc_uint32 count)
{
	HxLOG_Info("\n");
	return HxSTD_memcpy(pDest, pSource, count);
}

/**
 * CRT memset function
 */
void * UniversalClient_Stdlib_memset(void *dest, uc_uint8 c, uc_uint32 count)
{
	HxLOG_Info("\n");
	return HxSTD_memset(dest, c, count);
}

/**
 * CRT memcmp function
 */
uc_sint32 UniversalClient_Stdlib_memcmp(const uc_uint8 *pBuf1, const uc_uint8 *pBuf2, uc_uint32 len)
{
	HxLOG_Info("\n");
	return HxSTD_memcmp(pBuf1, pBuf2, len);
}

/**
 * CRT srand function
 */
void UniversalClient_Stdlib_srand(uc_uint32 seed)
{
	HxLOG_Info("\n");
	return srand(seed);
}

/**
 * CRT rand function
 */
uc_uint32 UniversalClient_Stdlib_rand(void)
{
	HxLOG_Info("\n");
	return rand();
}

/**
 * CRT printf function
 *
 * @param[in] pFormat
 *
 * \note Implementation of this requires access to varargs implementation of the core CRT method.
 */
uc_sint32 UniversalClient_Stdlib_printf(const uc_char *pFormat, ...)
{
#if defined(CONFIG_DEBUG)
	va_list args;
	HCHAR szMessage[IRUC_OS_DBG_PRINTBUF_SIZE+1] = {0, };
	HUINT32 ulCount;

	va_start(args, pFormat);
	ulCount = HxSTD_vsnprintf(szMessage, IRUC_OS_DBG_PRINTBUF_SIZE, pFormat, args);
	va_end(args);

	szMessage[ulCount-1] = '\0';

	HxLOG_Message(HxANSI_COLOR_YELLOW("%s")"\n", szMessage);

	return ulCount;
#else
	return 0;
#endif
}

/**
 * CRT sprintf function
 *
 * @param[in] buffer
 *
 * @param[in] format
 *
 * \note Implementation of this requires access to varargs implementation of the core CRT method.
 */
uc_uint32 UniversalClient_Stdlib_sprintf(uc_char *buffer, const uc_char *format, ...)
{
	va_list args;
	HUINT32 ulCount;

	va_start(args, format);
	ulCount = vsprintf(buffer, format, args);
	va_end(args);

	return ulCount;
}

int DBG_DRV_Printf(const char *format, ...)
{
	return UniversalClient_Stdlib_printf(format);
}

