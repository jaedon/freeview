/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
#include <stdlib.h>
#include <time.h>
#include <drmcommon.h>

#include <sys/time.h>
#include <pthread.h>

ENTER_PK_NAMESPACE_CODE;

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

#ifdef PRDY_ROBUSTNESS
wchar_t scdFilename[] =L"/mnt/persistent/scd.bin";

#define SECURE_CLK_DELTA_SIZE_BYTES     (16)

static time_t stored_delta = 0;
static time_t stored_lkgst = 0;
static pthread_mutex_t semOemTime = PTHREAD_MUTEX_INITIALIZER;

/* The format of buf is as follow:
 *      buf[0-3] = clock delta
 *      buf[8-11] = clock lkgst (global system time)
 */
#define SCD_FILE_FETCH_DELTA(buf, delta, lkgst)         \
{                                                       \
    (*delta) = ((time_t)(buf[0]))<<24    & 0xFF000000;  \
    (*delta) |= ((time_t)(buf[1]))<<16   & 0x00FF0000;  \
    (*delta) |= ((time_t)(buf[2]))<<8    & 0x0000FF00;  \
    (*delta) |= ((time_t)(buf[3]))       & 0x000000FF;  \
                                                        \
    /*if 0xFFFFFFFF was decrypted, this means the delta was 0 */  \
    if( (*delta) == 0xFFFFFFFF) (*delta) = 0;           \
                                                        \
    (*lkgst) =  ((time_t)(buf[8]))<<24   & 0xFF000000;  \
    (*lkgst) |= ((time_t)(buf[9]))<<16   & 0x00FF0000;  \
    (*lkgst) |= ((time_t)(buf[10]))<<8   & 0x0000FF00;  \
    (*lkgst) |= ((time_t)(buf[11]))      & 0x000000FF;  \
}


DRM_RESULT DRM_API DRM_CALL OEM_TimeInit(
    __in_opt    DRM_VOID      *pOEMContext
)
{

    DRM_BOOL scd_found      = FALSE;
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_BYTE *buf  = NULL;
    time_t current_systime    = 0;

    if(pthread_mutex_lock(&semOemTime) != 0){
        printf("%s: Cannot take semaphore.\n", __FUNCTION__);
        ChkDR(DRM_E_FAIL);
    }        

    //Initialize delta and 'last-known good secure time' to '0'
    stored_delta = 0;
    stored_lkgst = 0;

    /******************************************************************************
      Call 'time' so that we can get the 'current_systime'
    ******************************************************************************/
    time (&current_systime);    

    buf = Oem_MemHeapAlloc(SECURE_CLK_DELTA_SIZE_BYTES);
    ChkMem(buf);

    BKNI_Memset(buf, 0,SECURE_CLK_DELTA_SIZE_BYTES);

    /*  Detect presence of scd.bin */
    scd_found = FileExists(scdFilename);

    /************************************************************************************
        If scd.bin exists, read it, fetch stored delta
    *************************************************************************************/
    if( scd_found == TRUE )
    {

        ChkDR(_ReadAndDecryptFromFile (pOEMContext, DRM_DIT_PLAYREADY_ID, buf, SECURE_CLK_DELTA_SIZE_BYTES, scdFilename));

        SCD_FILE_FETCH_DELTA(buf, &stored_delta, &stored_lkgst);

        if( current_systime >= stored_lkgst) 
        {
            // **************************************************************************************
            // Reset delta to '0' (0xFFFFFFFF) and store current time as 'last-known good secure time
            // **************************************************************************************
            buf[0] = 0xFF;
            buf[1] = 0xFF;
            buf[2] = 0xFF;
            buf[3] = 0xFF;
            
            stored_lkgst = current_systime;
        }
        else
        {         
            // *************************************************************************
            // Update delta
            // *************************************************************************
            stored_delta = stored_lkgst - current_systime;
            
            buf[0] = (unsigned char)((stored_delta & 0xFF000000) >> 24);
            buf[1] = (unsigned char)((stored_delta & 0x00FF0000) >> 16);
            buf[2] = (unsigned char)((stored_delta & 0x0000FF00) >> 8);
            buf[3] = (unsigned char)((stored_delta & 0x000000FF) );
        }
        
        // Set the 'last-known good secure time' to either:
        //      a) what it was
        //      b) current system time
        buf[8] = (unsigned char) ((stored_lkgst & 0xFF000000) >> 24);
        buf[9] = (unsigned char) ((stored_lkgst & 0x00FF0000) >> 16);
        buf[10] = (unsigned char)((stored_lkgst & 0x0000FF00) >> 8);
        buf[11] = (unsigned char)((stored_lkgst & 0x000000FF) );

        /*
        printf("\n%s - buf[0:7] = %02x %02x %02x %02x %02x %02x %02x %02x (scd found)\n", __FUNCTION__, 
                     buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
        printf("%s - buf[8:15] = %02x %02x %02x %02x %02x %02x %02x %02x (scd found)\n\n", __FUNCTION__, 
                     buf[8], buf[9], buf[10], buf[11], buf[12], buf[13], buf[14], buf[15]);
        */

    }//end of if scd.bin found
    else //( scd_found == false )
    {
        /***********************************************************************************************
         If no scd.bin exists delta should be '0' (0xFFFFFFFF) and LKGST should be current system time
        ************************************************************************************************/
        buf[0] = 0xFF;
        buf[1] = 0xFF;
        buf[2] = 0xFF;
        buf[3] = 0xFF;

        //Set the 'last-known good secure time'
        buf[8] = (unsigned char) ((current_systime & 0xFF000000) >> 24);
        buf[9] = (unsigned char) ((current_systime & 0x00FF0000) >> 16);
        buf[10] = (unsigned char)((current_systime & 0x0000FF00) >> 8);
        buf[11] = (unsigned char)((current_systime & 0x000000FF) );

        /*
        printf("\n%s - buf[0:7] = %02x %02x %02x %02x %02x %02x %02x %02x (scd not found)\n", __FUNCTION__, 
                     buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
        printf("%s - buf[8:15] = %02x %02x %02x %02x %02x %02x %02x %02x (scd not found)\n\n", __FUNCTION__, 
                     buf[8], buf[9], buf[10], buf[11], buf[12], buf[13], buf[14], buf[15]);
        */
    }

    ChkDR(_EncryptAndWriteToFile (pOEMContext, DRM_DIT_PLAYREADY_ID, buf, SECURE_CLK_DELTA_SIZE_BYTES, scdFilename));

ErrorExit:

    if(buf != NULL){
        Oem_MemHeapFree(buf);
    }

    pthread_mutex_unlock(&semOemTime);

    return dr;
}

DRM_RESULT DRM_API DRM_CALL OEM_TimeUnInit(
    __in_opt    DRM_VOID      *pOEMContext
)
{
    DRM_BOOL scd_found   = FALSE;

    DRM_RESULT dr    = DRM_SUCCESS;
    DRM_BYTE  *buf   = NULL;

    time_t current_systime    = 0;

    if(pthread_mutex_lock(&semOemTime) != 0){
        printf("%s: Cannot take semaphore.\n", __FUNCTION__);
        ChkDR(DRM_E_FAIL);
    }        

    /******************************************************************************
      Call 'time' so that we can get the 'current_systime'
    ******************************************************************************/
    time (&current_systime);    

    buf = Oem_MemHeapAlloc(SECURE_CLK_DELTA_SIZE_BYTES);
    ChkMem(buf);

    BKNI_Memset(buf, 0,SECURE_CLK_DELTA_SIZE_BYTES);

    /*  Detect presence of scd.bin */
    scd_found = FileExists(scdFilename);

    /************************************************************************************
        If scd.bin exists, read it, fetch stored delta
    *************************************************************************************/
    if( scd_found == TRUE )
    {
        ChkDR(_ReadAndDecryptFromFile (pOEMContext, DRM_DIT_PLAYREADY_ID, buf, SECURE_CLK_DELTA_SIZE_BYTES, scdFilename));

        SCD_FILE_FETCH_DELTA(buf, &stored_delta, &stored_lkgst);

        //Delta stays the same
        if(stored_delta == 0){
            stored_delta = 0xFFFFFFFF;
        }

        buf[0] = (unsigned char)((stored_delta & 0xFF000000) >> 24);
        buf[1] = (unsigned char)((stored_delta & 0x00FF0000) >> 16);
        buf[2] = (unsigned char)((stored_delta & 0x0000FF00) >> 8);
        buf[3] = (unsigned char)((stored_delta & 0x000000FF) );
        
        // Set the 'last-known good secure time' to 'current_systime  + delta'
        stored_lkgst = current_systime + stored_delta;
        buf[8] = (unsigned char) ((stored_lkgst & 0xFF000000) >> 24);
        buf[9] = (unsigned char) ((stored_lkgst & 0x00FF0000) >> 16);
        buf[10] = (unsigned char)((stored_lkgst & 0x0000FF00) >> 8);
        buf[11] = (unsigned char)((stored_lkgst & 0x000000FF) );

        /*
        printf("\n%s - buf[0:7] = %02x %02x %02x %02x %02x %02x %02x %02x (scd found)\n", __FUNCTION__, 
                     buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
        printf("%s - buf[8:15] = %02x %02x %02x %02x %02x %02x %02x %02x (scd found)\n\n", __FUNCTION__, 
                     buf[8], buf[9], buf[10], buf[11], buf[12], buf[13], buf[14], buf[15]);
        */

        ChkDR(_EncryptAndWriteToFile (pOEMContext, DRM_DIT_PLAYREADY_ID, buf, SECURE_CLK_DELTA_SIZE_BYTES, scdFilename));

    }//end of if scd.bin found


ErrorExit:
    if(buf != NULL){
        Oem_MemHeapFree(buf);
    }

    pthread_mutex_unlock(&semOemTime);

    return dr;
}
#endif
/*
** convert a 32-bit time_t to a 64-bit file time
** only valid for 1970-2038 AD
*/
static void _time_t_To_UI64(
    __in time_t time_tA,
    __out DRM_UINT64 *pui64filetime )
{
    /* convert a time_t (seconds since 1970) to seconds since 1601. */
    DRM_UINT64 tics = DRM_UI64Add( DRM_UI64( time_tA ), C_SECONDS_FROM_1601_TO_1970 );

    /* convert seconds to 100-nanosecond tics. */
    *pui64filetime = DRM_UI64Mul( tics, DRM_UI64( C_TICS_PER_SECOND ) );
}

/*
** convert a 64-bit file time to a 32-bit time_t
** only valid for 1970-2038 AD
*/
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

static DRM_VOID _struct_tm_To_SYSTEMTIME(
    __in struct tm *ptm,
    __out DRMSYSTEMTIME *psystime )
{
    psystime->wYear         = (DRM_WORD) ptm->tm_year + 1900;
    psystime->wMonth        = (DRM_WORD) ptm->tm_mon  + 1;
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
    struct tm *ptm           = NULL;
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

#ifndef PRDY_ROBUSTNESS
DRM_API DRM_VOID DRM_CALL Oem_Clock_GetSystemTime(
    __in_opt    DRM_VOID      *pOEMContext,
    __out       DRMSYSTEMTIME *psystime )
{
    time_t     timeNow = 0;
    struct tm *ptm     = NULL;

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
    __in_opt    DRM_VOID      *pOEMContext,
    __in        DRMSYSTEMTIME *lpSystemTime )
{
#if !defined( __TMS320C55X__ ) && !defined( __arm ) && !defined(WINNT) && !defined(__MACINTOSH__)
#if DRM_TEST_USE_OFFSET_CLOCK
    DRMFILETIME ftCurrent;
    DRMFILETIME ftNew;
    DRM_UINT64  ui64Current = DRM_UI64(0);
    DRM_UINT64  ui64New     = DRM_UI64(0);

    Oem_Clock_GetSystemTimeAsFileTime( pOEMContext, &ftCurrent );

    if( DRM_SUCCEEDED(Oem_Clock_SystemTimeToFileTime( lpSystemTime, &ftNew) ) )
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
    if (lpSystemTime != NULL
     && _SystemTimeToTime_t(lpSystemTime, &timeA))
    {
        /* convert gmt time from time_t to local time in struct tm */
#if 0
        plocaltime = localtime(&timeA);

        /*OEMNOTE: this function is not in ANSI C.  Setting the system time is now entirely OS-dependent and there is no
        **         standard ANSI C way to do it.  Porting kit users should replace this with a non-portable API call
        **         _setsystem() expects a local time
        */

        _setsystime (plocaltime, 0);
#else
#if 1
		settimeofday(&timeA, NULL);
#else
		struct timeval garfield;
		time_t         new_time;

		plocaltime = localtime(&timeA);
		new_time=mktime(plocaltime);

		garfield.tv_sec =new_time;
		garfield.tv_usec=0;
		
		settimeofday(&garfield,NULL);

#endif	
#endif
    }
#endif
#endif
}

#else
DRM_API DRM_VOID DRM_CALL Oem_Clock_GetSystemTime(
    __in_opt    DRM_VOID      *pOEMContext,
    __out       DRMSYSTEMTIME *psystime )
{
    time_t     timeNow = 0;
    struct tm *ptm = NULL;

    if ( psystime != NULL )
    {
        time (&timeNow);
        //printf("%s - timeNow = '%lu' stored_delta = '%lu'\n", __FUNCTION__, timeNow, stored_delta);

        /******************************************************************************
         Add the stored delta to 'timeNow' if this is not an initial state case
        ******************************************************************************/
        timeNow += stored_delta;
        //printf("%s - timeNow + delta = '%lu'   (counter = %d)\n", __FUNCTION__, timeNow, counter);

        if ((ptm = gmtime (&timeNow)) != NULL)
        {
            _struct_tm_To_SYSTEMTIME (ptm, psystime);
        }
    }
}

DRM_API DRM_VOID DRM_CALL Oem_Clock_SetSystemTime(
    __in_opt    DRM_VOID      *pOEMContext,
    __in        DRMSYSTEMTIME *lpSystemTime )
{
    struct tm *plocaltime = NULL;
    time_t     timeA      = 0;
    struct timeval tv;
    struct timezone tz;
    DRM_BOOL scd_found      = FALSE;

    DRM_RESULT dr    = DRM_SUCCESS;
    DRM_BYTE  *buf   = NULL;
    time_t current_delta    = 0;
    time_t current_systime  = 0;

    if(pthread_mutex_lock(&semOemTime) != 0){
        printf("%s: Cannot take semaphore.\n", __FUNCTION__);
        ChkDR(DRM_E_FAIL);
    }        

    /* convert it to time_t structure  */
    if (lpSystemTime != NULL  && _SystemTimeToTime_t(lpSystemTime, &timeA))
    {
        /******************************************************************************
            1.  We still have to call 'time' so that we can compute the 'current_delta'
                regardless if no scd.bin (ok bkp) exists.
        ******************************************************************************/
        time (&current_systime);

        // Time to set is before current system time
        if(timeA < current_systime){
            printf("%s - negative delta, error.\n", __FUNCTION__);
            ChkDR(DRM_E_FAIL);
        }

        current_delta = timeA - current_systime;
        //printf("%s - Current delta = timeA(%lu) - current_systime(%lu) = '%lu'.\n", __FUNCTION__, timeA,  current_systime, current_delta);
        
        buf = Oem_MemHeapAlloc(SECURE_CLK_DELTA_SIZE_BYTES);
        ChkMem(buf);

        BKNI_Memset(buf, 0x00, SECURE_CLK_DELTA_SIZE_BYTES);

        /******************************************************************************
            2. Detect presence of scd.bin and open a FILE *
        ******************************************************************************/
        scd_found = FileExists(scdFilename);

        /************************************************************************************
            3.  If scd.bin exists, read it, verify integrity and fetch stored delta
        *************************************************************************************/
        if( scd_found == TRUE )
        {

            ChkDR(_ReadAndDecryptFromFile (pOEMContext, DRM_DIT_PLAYREADY_ID, buf, SECURE_CLK_DELTA_SIZE_BYTES, scdFilename));

            SCD_FILE_FETCH_DELTA(buf, &stored_delta, &stored_lkgst);
            
            //After obtaining time_t value from scd.bin, only update delta if different
            if(current_delta != stored_delta)
            {
                printf("%s - 'current_delta(%lu) != stored_delta (%lu)', updating file ...\n", __FUNCTION__, current_delta, stored_delta);

                if( current_delta == 0)
                {
                    buf[0] = 0xFF;
                    buf[1] = 0xFF;
                    buf[2] = 0xFF;
                    buf[3] = 0xFF;
                }
                else
                {         
                    buf[0] = (unsigned char)((current_delta & 0xFF000000) >> 24);
                    buf[1] = (unsigned char)((current_delta & 0x00FF0000) >> 16);
                    buf[2] = (unsigned char)((current_delta & 0x0000FF00) >> 8);
                    buf[3] = (unsigned char)((current_delta & 0x000000FF) );
                }

                 // *****************************
                 // update the stored_delta
                 //******************************
                 stored_delta = current_delta;


                // Set the LKGST to 'current_systime + current_delta'
                stored_lkgst = current_systime + current_delta;
                buf[8] = (unsigned char)((stored_lkgst & 0xFF000000) >> 24);
                buf[9] = (unsigned char)((stored_lkgst & 0x00FF0000) >> 16);
                buf[10] = (unsigned char)((stored_lkgst & 0x0000FF00) >> 8);
                buf[11] = (unsigned char)((stored_lkgst & 0x000000FF) );
    
                /*
                printf("\n%s - buf[0:7] = %02x %02x %02x %02x %02x %02x %02x %02x (scd found)\n", __FUNCTION__, 
                             buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
                printf("%s - buf[8:15] = %02x %02x %02x %02x %02x %02x %02x %02x (scd found)\n\n", __FUNCTION__, 
                             buf[8], buf[9], buf[10], buf[11], buf[12], buf[13], buf[14], buf[15]);
                */

                ChkDR(_EncryptAndWriteToFile (pOEMContext, DRM_DIT_PLAYREADY_ID, buf, SECURE_CLK_DELTA_SIZE_BYTES, scdFilename));
            }
        }//end of if scd.bin found
    }//end of  if (psystime != NULL  && _SystemTimeToTime_t(psystime, &timeA))

ErrorExit:
    if(buf != NULL){
        Oem_MemHeapFree(buf);
    }
    pthread_mutex_unlock(&semOemTime);

    return;
}
#endif

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
        DRM_TEST_OEM_CONTEXT *pTestOemContext = (DRM_TEST_OEM_CONTEXT *)f_pOEMContext;

        if ( ( pTestOemContext->fFreezeTickCount ) && ( pTestOemContext->dwTickCount > 0 ) )
        {
            dwTickCount = pTestOemContext->dwTickCount;
        }
    }
#endif

    return dwTickCount;
}


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


EXIT_PK_NAMESPACE_CODE;
