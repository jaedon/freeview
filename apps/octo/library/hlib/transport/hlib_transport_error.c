/**************************************************************************************/
/**
 * @file hxtransport_error.c
 * defines APIs's body
 *
 * All possible error codes from the functions of the transport module
 *
 * @author  Jongwon Kim(jwkim@humaxdigital.com)
 * @date    2011/01/26
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
**************************************************************************************
**/

#include "_hlib_transport.h"
#include <hlib.h>

#undef	HxLOG_DOMAIN
#define HxLOG_DOMAIN	DOMAIN_TVSVC_PAL_TRANSPORT

static HxTRANSPORT_Error_e s_transport_last_error = 0;

inline HERROR HLIB_TRANSPORT_return(HINT32 nErrCode)
{
	s_transport_last_error = nErrCode;

	return nErrCode;//(nErrCode == TPE_OK) ? ERR_OK : ERR_FAIL;
}

inline HxTRANSPORT_Error_e HLIB_TRANSPORT_getlasterror(void)
{
	HxLOG_Print("[PAL_TRANSPORT][%s]: Last error code = %x.\n", __FUNCTION__,
			s_transport_last_error);

	return s_transport_last_error;
}

