/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
#include <drmcommon.h>

#if DBG

#if defined(_PREFAST_)
#pragma prefast(push)
#pragma prefast(disable:22110)  /* g_pfDebugAnalyzeDR is only built in DBG builds and is intended to be a global function pointer. */
#endif /* defined(_PREFAST_) */
void (*g_pfDebugAnalyzeDR)(unsigned long) = NULL;
#if defined(_PREFAST_)
#pragma prefast(pop)
#endif /* defined(_PREFAST_) */

#endif //DBG


