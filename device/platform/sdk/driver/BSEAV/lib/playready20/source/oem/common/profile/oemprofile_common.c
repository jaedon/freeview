/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <oemprofile.h>

ENTER_PK_NAMESPACE_CODE;

#if DRM_SUPPORT_PROFILING

/* Default global context: check detail level, do not check feature set 
/* Future enhancement: read default values of flags from a global configuration file */
/* so the flags can be set at runtime. */
static PERF_GLOBAL_CONTEXT g_PerfContext = {
    /* Scope stack */
    {0}, 
    
    /* Point to top of the scope stack */
    1,
    
    /* Logging detail level*/
    3,
    
    /* Logging feature set mask */
    0,
    
    /* Next PER_AGG_SCOPE_CONTEXT slot number to be assigned */
    0,
    
    /* File handle for normal scope samples */
    OEM_INVALID_HANDLE_VALUE,
    
    /* File handle for aggregated scope samples */
    OEM_INVALID_HANDLE_VALUE,
    
    /* Buffer to cache normal scope samples to minimize I/O */
    {0},
    
    /* Next cache buffer (sample) slot to be assigned */
    0,
    
    /* Global aggregated scope context */
    {0},
    
    /* Flag indicating whether the profiling machanism is enable or not */
    FALSE,
    
    /* Flag to prevent recursive profiling */
    FALSE,

    /*Setting the time spent in writing profiling logs to 0 initially*/
    0

#if PROFILE_USER_DATA
    , 
    
    /* File handle for user data logging */
    OEM_INVALID_HANDLE_VALUE,
    
    /* Buffer to cache user data to minimize I/O */
    {0},
    
    /* Next cache buffer (user data) slot to be assigned */
     0
#endif

#if PROFILE_STACK_SIZE
    ,

    /*File handle for logging stack utilization data*/
    OEM_INVALID_HANDLE_VALUE,

    /*Max stack size*/
    0,

    /*Stack Base*/
    0,

    /*Scope ID of the PK API leading to max stack usage*/
    0,

    /*Max stack usage over all the PK APIs involved in a specific scenario*/
    0
#endif

};

/**********************************************************************
** Function:    Oem_Profiling_GetProfileContext
** Synopsis:    Returns pointer to global profiling object. 
** Arguments:   None 
** Returns:     pointer to global profiling object. 
**              This function should always return pointer to valid object.
** Notes:       Common for most OEM platforms.
**              Requires special implementation on Symbian since 
**              global variables are not available.
*******************************************************************/
DRM_API PERF_GLOBAL_CONTEXT * DRM_CALL Oem_Profiling_GetProfileContext(DRM_VOID)
{ 
    return &g_PerfContext;
}

#endif  /* DRM_SUPPORT_PROFILING */

EXIT_PK_NAMESPACE_CODE;

