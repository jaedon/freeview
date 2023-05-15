/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include "DebugTypes.h"
#include "TeeDebug.h"
#include "TeeLog.h"

#define nfSS_MAX_FILE_SIZE	8092

// internal buffer for holding manuf secure store in memory
//static uint8_t	_saMemBuf[nfSS_MAX_FILE_SIZE];

/***************************************************************************
 * @function teeDebugPrint
 *
 * @brief formatted debug print, like printf
 *
 * @param[in] FormatStrPtr - pointer to format string
 * @param[in] va_list - list of format arguments (like printf)
 *
 * @return   void
 *
 **************************************************************************/
void teeDebugPrint( const char *FormatStrPtr, ...)
{
    va_list ArgList;

    va_start(ArgList, FormatStrPtr);
    teeVLog(Tee_Trace, FormatStrPtr, ArgList);
}/* end teeDebugPrint */

/***************************************************************************
 * @function teePrintHexDump
 *
 * @brief formatted hex dump to stdout
 *
 * @param[in] BufferPtr - pointer to buffer
 * @param[in] BufferLength - length of buffer
 *
 * @return   void
 *
 **************************************************************************/
void teePrintHexDump( uint8_t *BufferPtr, uint32_t BufferLength )
{
    uint32_t    i;
    uint32_t    j;
#define BYTES_PER_LINE 16 // bytes dumped per line
    char        DisplayedBytes[BYTES_PER_LINE + 1]; // output line
    uint16_t    Offset = 0;
    uint32_t    OffsetMax = (1 << (sizeof(Offset) * 8)) - 1;

    // null-terminate output line
    DisplayedBytes[BYTES_PER_LINE] = '\0';

    // limit buffer size (offset is uint16_t)
    if ( BufferLength > OffsetMax )
        BufferLength = OffsetMax;

    // dump it. dump it all.
    while ( BufferLength > 0 )
    {
        // print data offset
        printf( "%04x: ", Offset );
        Offset += BYTES_PER_LINE;

        // print BYTES_PER_LINE hex bytes
        for ( i = 0; i < BYTES_PER_LINE; ++i )
        {
            // if out of data, print ## instead...
            if ( i == BufferLength )
            {
                /* partial line - print ## for rest of line */
                for ( j = BYTES_PER_LINE - 1; j >= BufferLength; --j )
                {
                    printf("## ");
                    DisplayedBytes[j] = '#';
                }

               /* print DisplayedBytes + newline */
                printf( " >%s<\n", DisplayedBytes );
                return;
            }

            // print current byte in hex
            printf( "%02x ", BufferPtr[i] );

            // save display byte value until eol
            if ( isprint( BufferPtr[i] ) )
                DisplayedBytes[i] = BufferPtr[i];
            else
               DisplayedBytes[i] = '.';

        } /* end for ( i = 0; i < BYTES_PER_LINE; ++i ) */

        /* print display byte values + newline */
        printf( " >%s<\n", DisplayedBytes );

        /* update pointer and buffer length */
        BufferPtr += BYTES_PER_LINE;
        BufferLength -= BYTES_PER_LINE;

    }/* end while */

} /* end teePrintHexDump */

// ENDIANESS functions
/***************************************************************************
 * @function teeNtohl
 *
 * @brief non-libc ntohl
 *
 * @param[in] NetLong - long in network byte order
 *
 * @returns   uint32_t size
 *
 **************************************************************************/
uint32_t teeNtohl(const uint32_t NetLong)
{
    return (ntohl(NetLong));
}/* end teeNtohl */

/***************************************************************************
 * @function teeHtonl
 *
 * @brief non-libc htonl
 *
 * @param[in] HostLong - long in host byte order
 *
 * @returns   value in host order
 *
 **************************************************************************/
uint32_t teeHtonl(const uint32_t HostLong)
{
    return (htonl(HostLong));
}/* end teeHtonl */

/***************************************************************************
 * @function teeNtohs
 *
 * @brief non-libc ntohs
 *
 * @param[in] NetShort - short in network byte order
 *
 * @returns   value in network byte order
 *
 **************************************************************************/
uint16_t teeNtohs(const uint16_t NetShort)
{
    return (ntohs(NetShort));
}/* end teeNtohs */

/***************************************************************************
 * @function teeHtons
 *
 * @brief non-libc htons
 *
 * @param[in] HostShort - short in host byte order
 *
 * @returns   value in host order
 *
 **************************************************************************/
uint16_t teeHtons(const uint32_t HostShort)
{
    return (htons(HostShort));
}/* end teeHtons */

/***************************************************************************
 * @function teeMemcmp
 *
 * @brief non-libc memcmp
 *
 * @param[in] Ptr1 - pointer to first location
 * @param[in] Ptr2 - pointer to second location
 * @param[in] Length - length to compare
 *
 * @return   -1 if *Ptr1 < *Ptr2, 0 if equal, 1 if *Ptr1 > *Ptr2
 *
 **************************************************************************/
int teeMemcmp( const void *Ptr1, const void *Ptr2, uint32_t Length )
{
    // NRD prototype has libc...
    return memcmp( Ptr1, Ptr2, (size_t)Length );
}/* end teeMemcmp */


/***************************************************************************
 * @function teeMemcpy
 *
 * @brief non-libc memcpy
 *
 * @param[in] DstPtr - pointer to destination
 * @param[in] SrcPtr - pointer to source
 * @param[in] Length - length to copy
 *
 * @returns   DstPtr
 *
 **************************************d************************************/
void *teeMemcpy( void *DstPtr, const void *SrcPtr, uint32_t Length )
{
    // NRD prototype has libc...
    return memcpy( DstPtr, SrcPtr, (size_t)Length );

}/* end teeMemcpy */

/***************************************************************************
 * @function teeBzero
 *
 * @brief non-libc bzero
 *
 * @param[in] DstPtr - pointer to destination
 * @param[in] Length - length to zeroize
 *
 * @returns  void
 *
 **************************************************************************/
void teeBzero( void *DstPtr, uint32_t Length )
{
    // NRD prototype has libc...
    return bzero( DstPtr, (size_t)Length );

}/* end teeBzero */


/***************************************************************************
 * @function teeMemset
 *
 * @brief non-libc memset
 *
 * @param[in] DstPtr - pointer to destination
 * @param[in] Value - value to set
 * @param[in] Length - length
 *
 * @returns   DstPtr
 *
 **************************************************************************/
void *teeMemset( void *DstPtr, int Value, uint32_t Length )
{
    // NRD prototype has libc...
    return memset( DstPtr, Value, (size_t)Length );

}/* end teeMemset */

/***************************************************************************
 * @function teeMemAlloc
 *
 * @brief non-libc malloc
 *
 * @param[in] Size - number of bytes to allocate
 *
 * @returns   pointer to allocated memory if successful, NULL otherwise
 *
 **************************************************************************/
uint8_t *teeMemAlloc( uint32_t Size )
{
    // Just use malloc for now...
    return (uint8_t *) malloc( Size );

}/* end teeMemAlloc */

/***************************************************************************
 * @function teeMemFree
 *
 * @brief non-libc free
 *
 * @param[in] MemPtrPtr - pointer to pointer to memory you want freed
 *
 * @returns   void
 *
 **************************************************************************/
void teeMemFree( uint8_t **MemPtrPtr )
{
    // log error and return
    if ( ( MemPtrPtr == NULL ) || ( *MemPtrPtr == NULL ) )
    {
        MEMDBG("%s: invalid pointer, Ptr: %p, *Ptr: %p)\n", __FUNCTION__, MemPtrPtr,
                MemPtrPtr ? *MemPtrPtr : NULL );
        return;
    }

    // Just use free for now...
    (void) free( *MemPtrPtr );

    // clear pointer
    *MemPtrPtr = NULL;

    return;

}/* end teeMemAlloc */

/***************************************************************************
 * @function teeMemRealloc
 *
 * @brief non-libc realloc
 *
 * @param[in] ptr  - pointer which needs reallocate
 * @param[in] size - number of bytes to allocate
 *
 * @returns   pointer to allocated memory if successful, NULL otherwise
 *
 **************************************************************************/
uint8_t *teeMemRealloc( uint8_t * ptr, uint32_t size )
{
    return ((uint8_t*) realloc (ptr, size));
}/* end teeMemRealloc */

/***************************************************************************
 * @function teeStrlen
 *
 * @brief non-libc strlen
 *
 * @param[in] str - pointer to string
 *
 * @returns   uint32_t size
 *
 **************************************************************************/
int teeStrlen (const char* str)
{
    return ((int) strlen(str));
}/* end teeStrlen */

