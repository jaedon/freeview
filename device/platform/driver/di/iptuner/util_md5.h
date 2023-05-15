#ifndef __UTIL_MD5_H__
#define __UTIL_MD5_H__

#include <stdlib.h>
#include "htype.h"
#include "vkernel.h"

struct md5_s
{
    HUINT64 i_bits;
    HUINT32 uiDigest[4];
    HUINT32 p_data[16];
};

void HUMAX_InitMD5( struct md5_s *p_md5 );
void HUMAX_AddMD5( struct md5_s *p_md5, const void *p_src, size_t i_len );
void HUMAX_EndMD5( struct md5_s *p_md5 );

/**
 * Returns a char representation of the md5 hash, as shown by UNIX md5 or
 * md5sum tools.
 */
static inline char * psz_md5_hash( struct md5_s *pstMd5 )
{
	int i=0;
    char *psz = NULL;

	psz = (char *) DD_MEM_Alloc ( 33 ); /* md5 string is 32 bytes + NULL character */
    if( !psz )
	{
		return NULL;
    }

    for ( i = 0; i < 4; i++ )
    {
        (void) VK_snprintf( &psz[8*i],8,"%02x%02x%02x%02x",
            pstMd5->uiDigest[i] & 0xff,
            ( pstMd5->uiDigest[i] >> 8 ) & 0xff,
            ( pstMd5->uiDigest[i] >> 16 ) & 0xff,
            pstMd5->uiDigest[i] >> 24
        );
    }
    psz[32] = '\0';

    return psz;
}

#endif /* !__UTIL_MD5_H__ */
