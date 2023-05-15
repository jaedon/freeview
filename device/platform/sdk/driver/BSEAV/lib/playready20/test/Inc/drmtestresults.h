/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/


#ifndef __DRMTESTRESULTS_H__
#define __DRMTESTRESULTS_H__

#include "drmresults.h"

#define DRM_RESULT_FROM_WIN32(x) ((DRM_RESULT)(x) <= 0 ? ((DRM_RESULT)(x)) : ((DRM_RESULT) (((x) & 0x0000FFFF) | (DRM_FACILITY_WIN32 << 16) | 0x80000000)))

#endif /*__DRMTESTRESULTS_H__ */

