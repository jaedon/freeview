/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_OEMTIME_C
#include <stdlib.h>
#include <time.h>

#include <drmtypes.h>
#include <drmbytemanip.h>
#include <oemcommon.h>
#include <drmerr.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM_25004, "Changing parameters to const to satisfy warning would make them not match the standard OEM interface.");
PRAGMA_WARNING_DISABLE(4293); /* Shift count too large.  Disable because we compare SIZEOF(time_t) with SIZEOF(DRM_DWORD) before performing the shift. */

#if DRM_TEST_USE_OFFSET_CLOCK
/*
** This global will allow an offset to be added to the main system's clock,
** and is used by the test code to adjust the clock
** If g_fOEMTimeOffsetIsAbsolute is TRUE, then g_lOEMTimeOffset is an
** offset from zero instead of the current time
*/
DRM_EXPORT_VAR DRM_LONG g_lOEMTimeOffset = 0;
DRM_EXPORT_VAR DRM_BOOL g_fOEMTimeOffsetIsAbsolute = FALSE;
#endif

/*
** convert a 32-bit or 64-bit time_t to a 64-bit file time
** only valid for 1970-2038 AD
*/
static void _time_t_To_UI64(
    __in time_t time_tA,
    __out DRM_UINT64 *pui64filetime )
{
    /* convert a time_t (seconds since 1970) to seconds since 1601. */
    DRM_UINT64 tics = DRM_UI64(0);

    tics = DRM_UI64Add(
        ( SIZEOF(time_t) > SIZEOF(DRM_DWORD) ) ? DRM_UI64HL( (DRM_DWORD)(time_tA >> 32), (DRM_DWORD)(time_tA & MAX_UNSIGNED_TYPE(DRM_DWORD)) ) : DRM_UI64HL( 0, (DRM_DWORD)time_tA ),
        C_SECONDS_FROM_1601_TO_1970 );

    /* convert seconds to 100-nanosecond tics. */
    *pui64filetime = DRM_UI64Mul( tics, DRM_UI64( C_TICS_PER_SECOND ) );
}

/*
** convert a 64-bit file time to a 32-bit or 64-bit time_t
** only valid for 1970-2038 AD
*/
#if DRM_TEST_USE_OFFSET_CLOCK
static void _UI64_To_time_t(
    __in DRM_UINT64 ui64filetime,
    __out time_t *ptime_tA )
{
    /* convert 100-nanosecond tics to secs. */
    DRM_UINT64 secs = DRM_UI64Div( ui64filetime, DRM_UI64( C_TICS_PER_SECOND ) );

    /* convert seconds since 1601 to a time_t (seconds since 1970). */
    secs = DRM_UI64Sub( secs, C_SECONDS_FROM_1601_TO_1970 );
    *ptime_tA = DRM_UI64Low32( secs );
}
#endif /* DRM_TEST_USE_OFFSET_CLOCK */

static DRM_VOID _struct_tm_To_SYSTEMTIME(
    __in const struct tm *ptm,
    __out DRMSYSTEMTIME *psystime )
{
    psystime->wYear         = (DRM_WORD) ( (DRM_WORD)ptm->tm_year + 1900 );
    psystime->wMonth        = (DRM_WORD) ( (DRM_WORD)ptm->tm_mon  + 1 );
    psystime->wDay          = (DRM_WORD) ptm->tm_mday;
    psystime->wHour         = (DRM_WORD) ptm->tm_hour;
    psystime->wMinute       = (DRM_WORD) ptm->tm_min;
    psystime->wSecond       = (DRM_WORD) ptm->tm_sec;
    psystime->wDayOfWeek    = (DRM_WORD) ptm->tm_wday;
    psystime->wMilliseconds = 0;
}

static DRM_VOID _SYSTEMTIME_To_struct_tm(
    __in const DRMSYSTEMTIME  *psystime,
    __out struct tm *ptm )
{
    ptm->tm_year = psystime->wYear  - 1900; /* current minus 1900 */
    ptm->tm_mon  = psystime->wMonth - 1;    /* (0-11) */
    ptm->tm_mday = psystime->wDay;          /* (1-31) */
    ptm->tm_hour = psystime->wHour;         /* (0-23) */
    ptm->tm_min  = psystime->wMinute;       /* (0-59) */
    ptm->tm_sec  = psystime->wSecond;       /* (0-59) */
    ptm->tm_wday = psystime->wDayOfWeek;    /* (0-6)  */
}

static DRM_BOOL _SystemTimeToTime_t(
    __in const DRMSYSTEMTIME *psystime,
    __out time_t *ptimet )
{
    struct tm  tmA   = {0};
    struct tm  tmB   = {0};
    time_t     timeA = 0;
    time_t     timeB = 0;

    /* internal function, no need to test ptimet == NULL */

    if( ( psystime->wYear  <  1601 ) ||
        ( psystime->wMonth == 0    || psystime->wMonth > 12 ) ||
        ( psystime->wDay   == 0    || psystime->wMonth > 31 ) ||
        ( psystime->wHour   > 23 ) ||
        ( psystime->wMinute > 59 ) ||
        ( psystime->wSecond > 59 ) )
    {
        return FALSE;
    }

    _SYSTEMTIME_To_struct_tm (psystime, &tmA);

    /* The mktime function needs some care.  The tm struct we give it is in the TZ we want but maketime feels the need
       to adjust for the current machine time zone.  To work around this we have to figure out through more calls to
       gmtime and mktime what that TZ offset is so we can later remove it from the original result of mktime */
    timeA = mktime (&tmA);
    MEMCPY( &tmB, gmtime( &timeA ), SIZEOF(tmB) );

    timeB = mktime (&tmB);

    if( timeB > timeA )
    {
        timeA -= (timeB - timeA);
    }
    else
    {
        timeA += (timeA - timeB );
    }

    *ptimet = timeA;
    return TRUE;
}


DRM_API DRM_BOOL DRM_CALL Oem_Clock_SystemTimeToFileTime(
    __in const DRMSYSTEMTIME *psystime,
    __out DRMFILETIME *pfiletime )
{
    DRM_UINT64 ui64  = DRM_UI64(0);
    time_t     timeA = 0;

    if ( !_SystemTimeToTime_t(psystime, &timeA ) )
    {
        return FALSE;
    }

    _time_t_To_UI64( timeA, &ui64 );

    ui64 = DRM_UI64Add( ui64, DRM_UI64( psystime->wMilliseconds * 10000 ) );

    UI64_TO_FILETIME( ui64, *pfiletime );

    return TRUE;
}

DRM_API DRM_BOOL DRM_CALL Oem_Clock_FileTimeToSystemTime(
    __in const DRMFILETIME *pfiletime,
    __out DRMSYSTEMTIME *psystime )
{
    DRM_UINT64 tics          = DRM_UI64(0);
    DRM_UINT64 ui64          = DRM_UI64(0);
    const struct tm *ptm     = NULL;
    time_t     timeA         = 0;
    DRM_BOOL   fOK           = FALSE;
    DRM_DWORD  cMilliseconds = 0;

    if ( pfiletime == NULL  ||  psystime == NULL )
    {
        return FALSE;
    }

    FILETIME_TO_UI64( *pfiletime, ui64 );
    FILETIME_TO_UI64( *pfiletime, tics );

    cMilliseconds  = DRM_I64ToUI32(DRM_UI2I64(DRM_UI64Mod( ui64, DRM_UI64( C_TICS_PER_SECOND ) )));
    cMilliseconds /= 10000;

    /* convert to seconds */

    tics = DRM_UI64Div( tics, DRM_UI64(C_TICS_PER_SECOND) );

    /* change base to ANSI time's */

    tics = DRM_UI64Sub( tics, C_SECONDS_FROM_1601_TO_1970);

    timeA = (time_t) DRM_I64ToUI32(DRM_UI2I64(tics));

    if ((ptm = gmtime (&timeA)) != NULL)
    {
        _struct_tm_To_SYSTEMTIME (ptm, psystime);
        psystime->wMilliseconds = (DRM_WORD) cMilliseconds;
        fOK = TRUE;
    }

    return fOK;
}

DRM_API DRM_VOID DRM_CALL Oem_Clock_GetSystemTimeAsFileTime(
    __in_opt    DRM_VOID    *pOEMContext,
    __out       DRMFILETIME *pfiletime )
{
    time_t     timeNow  = 0;
    DRM_UINT64 ui64Time = DRM_UI64(0);

    if ( pfiletime != NULL )
    {
#if DRM_TEST_USE_OFFSET_CLOCK
        if( !g_fOEMTimeOffsetIsAbsolute )
#endif /* DRM_TEST_USE_OFFSET_CLOCK */
        {
            time (&timeNow);
            _time_t_To_UI64( timeNow, &ui64Time );
        }

#if DRM_TEST_USE_OFFSET_CLOCK
        if ( g_lOEMTimeOffset >= 0 )
        {
            ui64Time = DRM_UI64Add( ui64Time, DRM_UI64Mul( DRM_UI64( C_TICS_PER_SECOND ), DRM_UI64( g_lOEMTimeOffset ) ) );
        }
        else
        {
            ui64Time = DRM_UI64Sub( ui64Time, DRM_UI64Mul( DRM_UI64( C_TICS_PER_SECOND ), DRM_UI64( -g_lOEMTimeOffset ) ) );
        }
#endif /* DRM_TEST_USE_OFFSET_CLOCK */

        UI64_TO_FILETIME( ui64Time, *pfiletime );
    }
}

DRM_API DRM_VOID DRM_CALL Oem_Clock_GetSystemTime(
    __in_opt    DRM_VOID      *pOEMContext,
    __out       DRMSYSTEMTIME *psystime )
{
    time_t     timeNow = 0;
    const struct tm *ptm     = NULL;

    if ( psystime != NULL )
    {

#if DRM_TEST_USE_OFFSET_CLOCK
        DRM_UINT64 ui64Time = DRM_UI64(0);

        if( !g_fOEMTimeOffsetIsAbsolute )
        {
#endif /* DRM_TEST_USE_OFFSET_CLOCK */

            time (&timeNow);

#if DRM_TEST_USE_OFFSET_CLOCK
            _time_t_To_UI64( timeNow, &ui64Time );
        }

        if ( g_lOEMTimeOffset >= 0 )
        {
            ui64Time = DRM_UI64Add( ui64Time, DRM_UI64Mul( DRM_UI64( C_TICS_PER_SECOND ), DRM_UI64( g_lOEMTimeOffset ) ) );
        }
        else
        {
            ui64Time = DRM_UI64Sub( ui64Time, DRM_UI64Mul( DRM_UI64( C_TICS_PER_SECOND ), DRM_UI64( -g_lOEMTimeOffset ) ) );
        }
        _UI64_To_time_t( ui64Time, &timeNow );
#endif /* DRM_TEST_USE_OFFSET_CLOCK */

        if ((ptm = gmtime (&timeNow)) != NULL)
        {
            _struct_tm_To_SYSTEMTIME (ptm, psystime);
        }
    }
}

DRM_API DRM_VOID DRM_CALL Oem_Clock_SetSystemTime(
    __in_opt       DRM_VOID      *f_pOEMContext,
    __in     const DRMSYSTEMTIME *f_lpSystemTime )
{
#if !defined( __arm ) && !defined(WINNT) && !defined(__MACINTOSH__)
#if DRM_TEST_USE_OFFSET_CLOCK
    DRMFILETIME ftCurrent;
    DRMFILETIME ftNew;
    DRM_UINT64  ui64Current = DRM_UI64(0);
    DRM_UINT64  ui64New     = DRM_UI64(0);

    Oem_Clock_GetSystemTimeAsFileTime( f_pOEMContext, &ftCurrent );

    if( DRM_SUCCEEDED(Oem_Clock_SystemTimeToFileTime( f_lpSystemTime, &ftNew) ) )
    {
        FILETIME_TO_UI64(ftCurrent, ui64Current );
        FILETIME_TO_UI64(ftNew, ui64New );

        DRMASSERT( !g_fOEMTimeOffsetIsAbsolute ); /* Not supported */
        g_lOEMTimeOffset += DRM_I64ToUI32( DRM_I64Div( DRM_I64Sub( DRM_UI2I64(ui64New), DRM_UI2I64(ui64Current) ), DRM_I64( C_TICS_PER_SECOND ) ) );
    }
#else
    struct tm *plocaltime = NULL;
    time_t     timeA      = 0;

    /* convert it to time_t structure  */
    if (f_lpSystemTime != NULL
     && _SystemTimeToTime_t(f_lpSystemTime, &timeA))
    {
        /* convert gmt time from time_t to local time in struct tm */
        plocaltime = localtime(&timeA);

        /*OEMNOTE: this function is not in ANSI C.  Setting the system time is now entirely OS-dependent and there is no
        **         standard ANSI C way to do it.  Porting kit users should replace this with a non-portable API call
        **         _setsystem() expects a local time
        */

        _setsystime (plocaltime, 0);
    }
#endif
#endif
}



/*
**  OEMNOTE:
**  This implementation of Oem_Clock_GetTickCount must be replaced with ANSI-compliant function(s) that:
**  1) Have sufficient resolution, specifically <= 20 ms for this function
**  2) DO continue to increase in value while the process is not running (e.g. while sleeping) 
*/
DRM_API DRM_DWORD DRM_CALL Oem_Clock_GetTickCount(
    __in_opt    DRM_VOID      *f_pOEMContext )
{
    /*
    ** CLOCKS_PER_SEC is an ANSI Macro that converts the units returned by clock() into seconds.
    ** clock() / ( CLOCKS_PER_SEC / 1000 ) returns the value in units of milliseconds
    ** On some platforms CLOCKS_PER_SEC can be defined < 1000, therefore we have to go through 64 bit value
    ** and do multiplication first.
    */
    DRM_UINT64 ui64Temp    = DRM_UI64(clock());
    DRM_DWORD  dwTickCount = 0;

    ui64Temp = DRM_UI64Mul( ui64Temp, DRM_UI64(1000));
    ui64Temp = DRM_UI64Div(ui64Temp, DRM_UI64(CLOCKS_PER_SEC));

    dwTickCount = DRM_UI64Low32( ui64Temp );

#if DRM_TEST_USE_OFFSET_CLOCK
    if ( f_pOEMContext != NULL )
    {
        const DRM_TEST_OEM_CONTEXT *pTestOemContext = (const DRM_TEST_OEM_CONTEXT *)f_pOEMContext;

        if ( ( pTestOemContext->fFreezeTickCount ) && ( pTestOemContext->dwTickCount > 0 ) )
        {
            dwTickCount = pTestOemContext->dwTickCount;
        }
    }
#endif

    return dwTickCount;
}


/*
**  OEMNOTE:
**  This implementation of Oem_Clock_QueryPerformanceCounter must be replaced with ANSI-compliant function(s) that:
**  1) Have sufficient resolution, specifically <= 1 ms for this function
**  2) DO continue to increase in value while the process is not running (e.g. while sleeping) 
*/
DRM_API DRM_UINT64 DRM_CALL Oem_Clock_QueryPerformanceCounter(
    __in_opt    DRM_VOID      *f_pOEMContext )
{
    return DRM_UI64( clock() );
}



DRM_API DRM_UINT64 DRM_CALL Oem_Clock_QueryPerformanceFrequency(
    __in_opt    DRM_VOID      *f_pOEMContext )
{
    return DRM_UI64( CLOCKS_PER_SEC );
}

PREFAST_POP /* Changing parameters to const to satisfy warning would make them not match the standard OEM interface. */

EXIT_PK_NAMESPACE_CODE;
