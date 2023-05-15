/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <drmcrt.h>
#include <byteorder.h>

ENTER_PK_NAMESPACE_CODE;

/* String CRT functions */

DRM_API DRM_LONG DRM_CALL DRMCRT_wcsncmp(
    __in_ecount( count ) const DRM_WCHAR *first,
    __in_ecount( count ) const DRM_WCHAR *last,
    IN       DRM_SIZE_T  count )
{
    if (!count)
    {
        return(0);
    }

    while (--count && *first && *first == *last)
    {
        first++;
        last++;
    }

    return((DRM_LONG)(*first - *last));
}

DRM_API DRM_LONG DRM_CALL DRMCRT_wcsicmp(
    __in_z const DRM_WCHAR *first,
    __in_z const DRM_WCHAR *last )
{
    while( ( *first != g_wchNull )
        && ( DRMCRT_towlower(*first) == DRMCRT_towlower(*last) ) )
    {
        first++;
        last++;
    }

    return( (DRM_LONG)( DRMCRT_towlower(*first) - DRMCRT_towlower(*last) ) );
}

DRM_API DRM_LONG DRM_CALL DRMCRT_wcsnicmp(
    __in_ecount( count ) const DRM_WCHAR *first,
    __in_ecount( count ) const DRM_WCHAR *last,
    IN       DRM_SIZE_T  count )
{
    if( count == 0 )
    {
        return(0);
    }

    while( ( --count > 0 )
        && ( *first != g_wchNull )
        && ( DRMCRT_towlower(*first) == DRMCRT_towlower(*last) ) )
    {
        first++;
        last++;
    }

    return( (DRM_LONG)( DRMCRT_towlower(*first) - DRMCRT_towlower(*last) ) );
}

DRM_API DRM_SIZE_T DRM_CALL DRMCRT_wcslen(
    __in_z const DRM_WCHAR *wsz )
{
    const DRM_WCHAR *eos = wsz;
    while( *eos )
    {
        eos++;
    }
    return( (DRM_SIZE_T)(eos - wsz) );
}

DRM_API DRM_LONG DRM_CALL DRMCRT_strncmp (
    __in_ecount( count ) const DRM_CHAR  *first,
    __in_ecount( count ) const DRM_CHAR  *last,
    IN       DRM_SIZE_T  count)
{
    if (!count)
    {
        return(0);
    }

    while (--count && *first && *first == *last)
    {
        first++;
        last++;
    }

    return((DRM_LONG)(*first - *last));
}


DRM_API DRM_BOOL DRM_CALL DRMCRT_iswxdigit (DRM_WCHAR f_wch)
{
    DRM_WCHAR wch = NATIVE_WCHAR( f_wch );
    return ((wch >= NATIVE_WCHAR(g_wch0) && wch <= NATIVE_WCHAR(g_wch9))   /* A digit */
        ||  (wch >= NATIVE_WCHAR(g_wchA) && wch <= NATIVE_WCHAR(g_wchF))   /* Upper case hex char */
        ||  (wch >= NATIVE_WCHAR(g_wcha) && wch <= NATIVE_WCHAR(g_wchf))); /* lower case hex char */
}

DRM_API DRM_BOOL DRM_CALL DRMCRT_iswdigit( DRM_WCHAR wch )
{
    return ( NATIVE_WCHAR(wch) >= NATIVE_WCHAR(g_wch0)
          && NATIVE_WCHAR(wch) <= NATIVE_WCHAR(g_wch9) ); /* A digit */
}

DRM_API DRM_BOOL DRM_CALL DRMCRT_isdigit( DRM_CHAR ch )
{
    return ( ch >= g_ch0 && ch <= g_ch9 ); /* A digit */
}

DRM_API DRM_BOOL DRM_CALL DRMCRT_iswalpha (DRM_WCHAR wch)
{
    return ((NATIVE_WCHAR(wch) >= NATIVE_WCHAR(g_wchA) && NATIVE_WCHAR(wch) <= NATIVE_WCHAR(g_wchZ))   /* Upper case char */
         || (NATIVE_WCHAR(wch) >= NATIVE_WCHAR(g_wcha) && NATIVE_WCHAR(wch) <= NATIVE_WCHAR(g_wchz))); /* lower case char */
}

DRM_API DRM_WCHAR DRM_CALL DRMCRT_towlower (DRM_WCHAR wch)
{
    if (DRMCRT_iswalpha (wch)
    &&   NATIVE_WCHAR(wch) < NATIVE_WCHAR(g_wcha))
    {
        DRM_WCHAR wchTemp = NATIVE_WCHAR(g_wcha) - NATIVE_WCHAR(g_wchA);
        wch = NATIVE_WCHAR(wch) + wchTemp;
        wch = WCHAR_CAST( wch );
    }

    return wch;
}

DRM_API DRM_BOOL DRM_CALL DRMCRT_iswspace (DRM_WCHAR wch)
{
    return (NATIVE_WCHAR(wch) == NATIVE_WCHAR(g_wchTab)
         || NATIVE_WCHAR(wch) == NATIVE_WCHAR(g_wchLineFeed)
         || NATIVE_WCHAR(wch) == NATIVE_WCHAR(g_wchVerticalTab)
         || NATIVE_WCHAR(wch) == NATIVE_WCHAR(g_wchFormFeed)
         || NATIVE_WCHAR(wch) == NATIVE_WCHAR(g_wchCarriageReturn)
         || NATIVE_WCHAR(wch) == NATIVE_WCHAR(g_wchSpace));
}

DRM_API DRM_SIZE_T DRM_CALL DRMCRT_strlen(
    __in_z const DRM_CHAR *sz )
{
    const DRM_CHAR *eos = sz;
    while( *eos )
    {
        eos++;
    }
    return( (DRM_SIZE_T)(eos - sz) );
}

/*******************************************************************************
*memmove - Copy source buffer to destination buffer
*
*Purpose:
*       memmove() copies a source memory buffer to a destination memory buffer.
*       This routine recognize overlapping buffers to avoid propogation.
*       For cases where propogation is not a problem, memcpy() can be used.
*
*Entry:
*       void *dst = pointer to destination buffer
*       const void *src = pointer to source buffer
*       size_t count = number of bytes to copy
*
*Exit:
*       Returns a pointer to the destination buffer
*
*Exceptions:
*******************************************************************************/

DRM_API DRM_VOID * DRM_CALL DRMCRT_memmove (
    __out_bcount( count ) DRM_VOID  *dst,
    __in_bcount( count ) const DRM_VOID  *src,
    IN       DRM_SIZE_T count )
{
        DRM_VOID * ret = dst;

        if (dst <= src || (DRM_CHAR *)dst >= ((DRM_CHAR *)src + count)) {
                /*
                 * Buffers don't overlap in a way that would cause problems
                 * copy from lower addresses to higher addresses
                 */
                while (count--) {
                        *(DRM_CHAR *)dst = *(DRM_CHAR *)src;
                        dst = (DRM_CHAR *)dst + 1;
                        src = (DRM_CHAR *)src + 1;
                }
        }
        else {
                /*
                 * Overlapping Buffers
                 * copy from higher addresses to lower addresses
                 */
                dst = (DRM_CHAR *)dst + count - 1;
                src = (DRM_CHAR *)src + count - 1;

                while (count--) {
                        *(DRM_CHAR *)dst = *(DRM_CHAR *)src;
                        dst = (DRM_CHAR *)dst - 1;
                        src = (DRM_CHAR *)src - 1;
                }
        }

        return(ret);
}

DRM_API DRM_VOID DRM_CALL DRMCRT_memcpy(
    __out_bcount( count ) DRM_VOID  *dst,
    __in_bcount( count ) const DRM_VOID  *src,
    IN       DRM_SIZE_T  count )
{
    while( count > 0 )
    {
        *(DRM_BYTE*)dst = *(DRM_BYTE*)src;
        dst = (DRM_CHAR *)dst + 1;
        src = (DRM_CHAR *)src + 1;
        count--;
    }
}

DRM_API DRM_VOID DRM_CALL DRMCRT_memset(
    __out_bcount( count ) DRM_VOID  *dst,
    IN       DRM_DWORD   b,
    IN       DRM_SIZE_T  count )
{
    /*
    ** b is a DRM_DWORD to be consistent with standard memset
    ** but we only use the low order byte
    */
    DRM_BYTE by = b & 0xFF;

    DRMASSERT( b <= 0xFF );

    while( count > 0 )
    {
        *(DRM_BYTE*)dst = by;
        dst = (DRM_CHAR *)dst + 1;
        count--;
    }
}

DRM_API DRM_LONG DRM_CALL DRMCRT_memcmp(
    __in_bcount( count ) const DRM_VOID  *src1,
    __in_bcount( count ) const DRM_VOID  *src2,
    IN       DRM_SIZE_T  count )
{
    while( count > 0 )
    {
        if( *(DRM_BYTE*)src1 > *(DRM_BYTE*)src2 )
        {
            return 1;
        }
        else if( *(DRM_BYTE*)src1 < *(DRM_BYTE*)src2 )
        {
            return -1;
        }
        src1 = (DRM_CHAR *)src1 + 1;
        src2 = (DRM_CHAR *)src2 + 1;
        count--;
    }
    return 0;
}

DRM_API DRM_LONG DRM_CALL DRMCRT_abs( DRM_LONG number )
{
    return (number>=0 ? number : -number);
}

EXIT_PK_NAMESPACE_CODE;

