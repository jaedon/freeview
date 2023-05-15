/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMCHKDEBUG_C
#include <drmerr.h>
#include <drmlastinclude.h>

#if DBG

#if defined(_PREFAST_)
#pragma prefast(push)
#pragma prefast(disable:22110)  /* g_pfDebugAnalyzeDR is only built in DBG builds and is intended to be a global function pointer. */
#endif /* defined(_PREFAST_) */

ENTER_PK_NAMESPACE_CODE;

void (*g_pfDebugAnalyzeDR)(unsigned long) = NULL;

EXIT_PK_NAMESPACE_CODE;

#if defined(_PREFAST_)
#pragma prefast(pop)
#endif /* defined(_PREFAST_) */

#endif //DBG


