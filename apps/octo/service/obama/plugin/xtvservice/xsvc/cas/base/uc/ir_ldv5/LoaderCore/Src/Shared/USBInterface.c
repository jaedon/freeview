/*****************************************************************************
*                  COPYRIGHT (C) 2016 Humax Co.                            *
******************************************************************************
*          File : USBInterface.c
*                                                                            *
*   Description : USB Data gatering interface                *
*****************************************************************************/


#include "LoaderCorePrivate.h"
#include "USBInterface.h"

static lc_uchar *s_pCD5 = NULL;
static lc_uint32 s_nCD5_len = 0;

lc_result LC_USB_SetCD5Data( lc_uchar * pCD5, lc_sint32 nLength )
{
    lc_result result = LC_FAILURE;

	s_pCD5 = NULL;
	s_nCD5_len = 0;

	if(pCD5 != NULL)
	{
		if( 0 < nLength )
		{
			dbgprt1("Set USB Data ok : length = %d\n", nLength );
			s_pCD5 = pCD5;
			s_nCD5_len = nLength;
			result = LC_SUCCESS;
		}
	}

   	return result;
}

lc_result
LC_USBDataRequest(
    lc_uchar *out_pData,
    lc_uint32 length,
    lc_uint32 offset )
{
    lc_result result = LC_SUCCESS;

    if (0 == length)
    {
		dbgerr0("request length is zero\n");
        result = LC_FAILURE;
    }

    if( LC_SUCCESS == result )
    {

		if( s_nCD5_len == 0 || s_pCD5 == NULL )
		{
			dbgerr2("Corruption occured, s_nCD5_len (%d), s_pCD5 [%x]\n", s_nCD5_len, s_pCD5);
			result = LC_FAILURE;
		}

		if( LC_SUCCESS == result )
		{
			if( s_nCD5_len < offset + length )
			{
				dbgerr4("s_nCD5_len (%d), offset(%d) + length(%d) = (%d)\n",
					s_nCD5_len,
					offset,
					length,
					offset + length );
				result = LC_FAILURE;
			}
		}

		if( LC_SUCCESS == result )
		{
			LC_memcpy(out_pData, &s_pCD5[offset], length);
		}
    }

    return result;

}



