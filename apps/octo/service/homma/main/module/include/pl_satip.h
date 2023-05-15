/**
 *************************************************************
 *	@file		pl_satip.h
 *	HumaxDataManager internal header for other applications.
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2013/04/12
 *	@author			humax
 *	@brief			pl_satip external header
 *************************************************************
 */

#ifndef	__PL_SATIP_H__
#define	__PL_SATIP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <hlib.h>
#include "common.h"

#define ________________Public_Interfaces__________________________
HERROR	PL_SATIP_Start( const HCHAR *pszFriendlyName );

HERROR	PL_SATIP_Stop( void );

HERROR	PL_SATIP_SetFriendlyName( const HCHAR *pszFriendlyName);

HERROR	PL_SATIP_ChangeBootID(void);

#ifdef __cplusplus
};
#endif // __cplusplus

#endif // __PL_SATIP_H__

/* end od file */

