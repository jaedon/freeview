/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#if !defined(__arm)
#include <wchar.h>
#endif

#include <drmcrt.h>

#if !TARGET_LITTLE_ENDIAN
#error On a Big Endian platform for wide strings, PlayReady PK only supports Little Endian UTF16 while the CRT will expect Big Endian UTF16.  DRM_SUPPORT_CRT must be set.
#endif

#if DRM_BUILD_PROFILE == DRM_BUILD_PROFILE_WP8 || DRM_BUILD_PROFILE == DRM_BUILD_PROFILE_XBOX || DRM_BUILD_PROFILE == DRM_BUILD_PROFILE_IOS
#ifdef DRMCRT_memcpy 
#undef DRMCRT_memcpy
#endif
#ifdef DRMCRT_memset 
#undef DRMCRT_memset
#endif
#ifdef DRMCRT_memcmp 
#undef DRMCRT_memcmp
#endif
#endif /* DRM_BUILD_PROFILE == DRM_BUILD_PROFILE_WP8 || DRM_BUILD_PROFILE == DRM_BUILD_PROFILE_XBOX || DRM_BUILD_PROFILE == DRM_BUILD_PROFILE_IOS */

ENTER_PK_NAMESPACE_CODE;

DRM_API DRM_LONG DRM_CALL DRMCRT_wcsncmp(
    __in_ecount( count ) const DRM_WCHAR  *first,
    __in_ecount( count ) const DRM_WCHAR  *last,
    __in                       DRM_SIZE_T  count )
{
    return wcsncmp( (const DRM_WCHAR_NATIVE *)first, (const DRM_WCHAR_NATIVE *)last, count );
}

DRM_API DRM_LONG DRM_CALL DRMCRT_wcsicmp(
    __in_z const DRM_WCHAR *first,
    __in_z const DRM_WCHAR *last )
{
    return _wcsicmp( (const DRM_WCHAR_NATIVE *)first, (const DRM_WCHAR_NATIVE *)last );
}

DRM_API DRM_LONG DRM_CALL DRMCRT_wcsnicmp(
    __in_ecount( count ) const DRM_WCHAR  *first,
    __in_ecount( count ) const DRM_WCHAR  *last,
    __in                       DRM_SIZE_T  count )
{
    return _wcsnicmp( (const DRM_WCHAR_NATIVE *)first, (const DRM_WCHAR_NATIVE *)last, count );
}

DRM_API DRM_SIZE_T DRM_CALL DRMCRT_wcslen(
    __in_z const DRM_WCHAR *wsz )
{
    return (DRM_DWORD)wcslen( (const DRM_WCHAR_NATIVE *)wsz  ) ;
}

DRM_API DRM_LONG DRM_CALL DRMCRT_strncmp(
    __in_ecount( count ) const DRM_CHAR   *first,
    __in_ecount( count ) const DRM_CHAR   *last,
    __in                       DRM_SIZE_T  count )
{
    return strncmp( first, last, count );
}

DRM_API DRM_BOOL DRM_CALL DRMCRT_iswxdigit (DRM_WCHAR f_wch)
{
    return iswxdigit( NATIVE_WCHAR(f_wch) );
}

DRM_API DRM_BOOL DRM_CALL DRMCRT_iswdigit( DRM_WCHAR wch )
{
    return iswdigit( NATIVE_WCHAR(wch) );
}

DRM_API DRM_BOOL DRM_CALL DRMCRT_isdigit( DRM_CHAR ch )
{
    return isdigit( (unsigned char)ch );
}

DRM_API DRM_BOOL DRM_CALL DRMCRT_iswalpha (DRM_WCHAR wch)
{
    return iswalpha( NATIVE_WCHAR(wch) );
}

DRM_API DRM_WCHAR DRM_CALL DRMCRT_towlower (DRM_WCHAR wch)
{
    return towlower( NATIVE_WCHAR(wch) );
}

DRM_API DRM_BOOL DRM_CALL DRMCRT_iswspace (DRM_WCHAR wch)
{
    return iswspace( NATIVE_WCHAR(wch) );
}

DRM_API DRM_SIZE_T DRM_CALL DRMCRT_strlen(
    __in_z const DRM_CHAR *sz )
{
    return (DRM_DWORD)strlen( sz );
}

/*
** Microsoft Internal Policy related change. See constdata.c for details.
*/
#if DRM_OBFUS_INSIDE_VBN
    extern "C" void* VBN_memmove(void* dst, const void* src, DRM_SIZE_T count);
    extern "C" DRM_LONG VBN_memcmp(const void* src1, const void* src2, DRM_SIZE_T count);
    extern "C" void* VBN_memset(void* dst, DRM_LONG val, DRM_SIZE_T count);
    extern "C" void* VBN_memcpy(void* dst, const void* src, DRM_SIZE_T count);
#endif /* DRM_OBFUS_INSIDE_VBN */

DRM_API DRM_VOID * DRM_CALL DRMCRT_memmove(
    __out_bcount( count )       DRM_VOID  *dst,
    __in_bcount( count )  const DRM_VOID  *src,
    __in                        DRM_SIZE_T count )
{
#if DRM_OBFUS_INSIDE_VBN
    return VBN_memmove( dst, src, count );
#else /* DRM_OBFUS_INSIDE_VBN */
    return memmove( dst, src, count );
#endif /* DRM_OBFUS_INSIDE_VBN */
}

DRM_API DRM_VOID DRM_CALL DRMCRT_memcpy(
    __out_bcount( count )       DRM_VOID   *dst,
    __in_bcount( count )  const DRM_VOID   *src,
    __in                        DRM_SIZE_T  count )
{
#if DRM_OBFUS_INSIDE_VBN
    VBN_memcpy( dst, src, count );
#else /* DRM_OBFUS_INSIDE_VBN */
#if DRM_BUILD_PROFILE == DRM_BUILD_PROFILE_PC && !__MACINTOSH__
    memcpy_s( dst, count, src, count );
#else  /* DRM_BUILD_PROFILE == DRM_BUILD_PROFILE_PC && !__MACINTOSH__ */
    memcpy( dst, src, count );
#endif /* DRM_BUILD_PROFILE == DRM_BUILD_PROFILE_PC && !__MACINTOSH__ */
#endif /* DRM_OBFUS_INSIDE_VBN */
}

DRM_API DRM_VOID DRM_CALL DRMCRT_memset(
    __out_bcount( count ) DRM_VOID   *dst,
    __in                  DRM_DWORD   b,
    __in                  DRM_SIZE_T  count )
{
#if DRM_OBFUS_INSIDE_VBN
    VBN_memset( dst, b, count );
#else /* DRM_OBFUS_INSIDE_VBN */
    memset( dst, (int)b, count );
#endif /* DRM_OBFUS_INSIDE_VBN */
}

DRM_API DRM_LONG DRM_CALL DRMCRT_memcmp(
    __in_bcount( count ) const DRM_VOID   *src1,
    __in_bcount( count ) const DRM_VOID   *src2,
    __in                       DRM_SIZE_T  count )
{
#if DRM_OBFUS_INSIDE_VBN
    return VBN_memcmp( src1, src2, count );
#else /* DRM_OBFUS_INSIDE_VBN */
    return memcmp( src1, src2, count );
#endif /* DRM_OBFUS_INSIDE_VBN */
}

DRM_API DRM_LONG DRM_CALL DRMCRT_abs( DRM_LONG number )
{
    return abs( (DRM_LONG)number );
}

EXIT_PK_NAMESPACE_CODE;

