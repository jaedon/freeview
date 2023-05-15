#ifndef __UTIL_MD5_H__
#define __UTIL_MD5_H__

#include <stdlib.h>
#include "htype.h"
#include "vkernel.h"

struct md5_s
{
    HUINT64 i_bits;      /* Total written bits */
    HUINT32 p_digest[4]; /* The MD5 digest */
    HUINT32 p_data[16];  /* Buffer to cache non-aligned writes */
};

void InitMD5( struct md5_s *p_md5 );
void AddMD5( struct md5_s *p_md5, const void *p_src, size_t i_len );
void EndMD5( struct md5_s *p_md5 );

static inline char * psz_md5_hash( struct md5_s *p_md5_s )
{
	int iIndex;

    char *pcTemp = (char *) DD_MEM_Alloc ( 33 ); /* md5 string is 32 bytes + NULL character */
    if( pcTemp == NULL )
	{
		return NULL;
    }

    for ( iIndex = 0; iIndex < 4; iIndex++ )
    {
        VK_snprintf( &pcTemp[8*iIndex], 8,"%02x%02x%02x%02x",
            p_md5_s->p_digest[iIndex] & 0xff,
            ( p_md5_s->p_digest[iIndex] >> 8 ) & 0xff,
            ( p_md5_s->p_digest[iIndex] >> 16 ) & 0xff,
            p_md5_s->p_digest[iIndex] >> 24
        );
    }
    pcTemp[32] = '\0';

    return pcTemp;
}

#endif /* !__UTIL_MD5_H__ */
