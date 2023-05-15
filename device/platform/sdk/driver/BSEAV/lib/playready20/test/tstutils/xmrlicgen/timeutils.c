/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
#include <stdafx.h>

#include <PKTestNameSpace.h>

USING_PK_NAMESPACE
USING_PKTEST_NAMESPACE

#include <logger.h>
#include <wchar.h>

#define C_SECONDS_FROM_1601_TO_1970  DRM_UI64Mul(DRM_UI64(1164447360), DRM_UI64(10))
#define C_TICS_PER_SECOND 10000000
#define BUFFERSIZE 20

DRM_BOOL DRM_CALL tFileTimeToDWORD(    
    __in DRMFILETIME *f_pfiletime,
    __out DRM_DWORD  *f_pdwTime )
{
    DRM_UINT64 tics;

    FILETIME_TO_UI64( *f_pfiletime, tics );
    
    /* convert 100-nanosecond tics to seconds*/
    tics = DRM_UI64Div( tics, DRM_UI64(C_TICS_PER_SECOND));
    
    /* convert seconds-since-1601 to time_t (seconds since 1970)*/ 
    *f_pdwTime = DRM_UI64Low32(DRM_UI64Sub(tics,C_SECONDS_FROM_1601_TO_1970));
   
    return TRUE;
}


DRM_RESULT DRM_CALL _TextTimeToFileTime(DRM_WCHAR *pText, DRMFILETIME *pFT)
{
    DRM_RESULT    dr        = DRM_SUCCESS;
    DRMSYSTEMTIME st        = {0};
    DRM_WCHAR    *pTemp     = 0;
    DRM_WCHAR     buffer[8] = {0};

    ChkArg( pText && pFT );
    ChkArg( *pText == g_wchPound );

    pTemp = pText;
    pTemp++; // skip over #
    TST_UTL_wcsncpy(buffer, pTemp, 4);
    ChkDR( DRMCRT_WtoWORD( buffer, 4, 10, &st.wYear, NULL ) );

    pTemp += 4;
    memset(buffer, 0, 16);
    TST_UTL_wcsncpy(buffer, pTemp, 2);
    ChkDR( DRMCRT_WtoWORD( buffer, 2, 10, &st.wMonth, NULL ) );

    pTemp += 2;
    memset(buffer, 0, 16);
    TST_UTL_wcsncpy(buffer, pTemp, 2);
    ChkDR( DRMCRT_WtoWORD( buffer, 2, 10, &st.wDay, NULL ) );

    pTemp += 2;
    memset(buffer, 0, 16);

    do
    {
        if ( *pTemp != g_wchSpace )
            break;

        pTemp++;   // skip space
        TST_UTL_wcsncpy(buffer, pTemp, 2);
        ChkDR( DRMCRT_WtoWORD( buffer, 2, 10, &st.wHour, NULL ) );
        
        pTemp += 2;
        memset(buffer, 0, 16);

        if ( *pTemp != g_wchColon )
            break;

        pTemp++;   // skip colon
        TST_UTL_wcsncpy(buffer, pTemp, 2);
        ChkDR( DRMCRT_WtoWORD( buffer, 2, 10, &st.wMinute, NULL ) );
        
        pTemp += 2;
        memset(buffer, 0, 16);

        if ( *pTemp != g_wchColon )
            break;

        pTemp++;   // skip colon
        TST_UTL_wcsncpy(buffer, pTemp, 2);
        ChkDR( DRMCRT_WtoWORD( buffer, 2, 10, &st.wSecond, NULL ) );
    } while (FALSE);
    
    Oem_Clock_SystemTimeToFileTime(&st, pFT);
ErrorExit:
    return dr;
}

DRM_RESULT DRM_CALL _CalcRelativeDate(
          DRM_WCHAR       *pDest, 
    const DRM_WCHAR       *pDateOffset, 
          DRM_DWORD        destLen,
          DRMSYSTEMTIME   *pBaseSystemTime )
{
    DRM_RESULT       dr                    = DRM_SUCCESS;
    DRM_WCHAR        wszBuffer[BUFFERSIZE] = { WCHAR_CAST( '\0' ) };
    DRM_WCHAR        rgchCatBuffer[3]      = { WCHAR_CAST( '\0' ) };
    DRMSYSTEMTIME    st                    = {0};
    DRM_DWORD        offsetLen             = 0;
    DRM_DWORD        cchScratch            = 0;
    DRM_DWORD        pNext                 = 0;
    const DRM_WCHAR *pTheEnd               = 0;
    const DRM_WCHAR *pStop                 = 0;
    const DRM_WCHAR *pCur                  = 0;
    int              Year                  = 0;
    int              Month                 = 0;
    int              MonthNow              = 0;
    int              Day                   = 0;
    int              Hour                  = 0;
    int              Minute                = 0;
    int              Second                = 0;
    int              val                   = 0;
    
    ChkArg( pDest != NULL );

    pDest[0] = 0;
    if ( pBaseSystemTime != NULL )
    {
        st = *pBaseSystemTime;
    }
    else
    {
        Oem_Clock_GetSystemTime(NULL, &st);
    }

    st.wMonth--;    // make start on 0 instead of 1
    st.wDay--;      // make start on 0 instead of 1

    // MOVE DATA TO SIGNED VARIABLES TO REMOVE THE UNSIGNED MATH
    Year = (int) st.wYear;
    Month = (int) st.wMonth;
    MonthNow = Month;
    Day = (int) st.wDay;
    Hour = (int) st.wHour;
    Minute = (int) st.wMinute;
    Second = (int) st.wSecond;

    offsetLen = DRMCRT_wcslen(pDateOffset);
    pTheEnd = pDateOffset + offsetLen;  // point to the null terminator.

    pCur = pDateOffset;

    while (pCur < pTheEnd)
    {
        // pickup offsets for each field
        DRM_DWORD dwVal = 0;
        ChkDR(DRMCRT_WtoDWORD(pCur, offsetLen , 10, &dwVal, &pNext));
        pStop = pCur + pNext;
        val = (int)dwVal;
        if(pStop[0] == WCHAR_CAST('y'))
        {
            Year += val;
        }
        else if(pStop[0] == WCHAR_CAST('M'))
        {
            Month += val;
        }
        else if(pStop[0] == WCHAR_CAST('d'))
        {
            Day += val;
        }
        else if(pStop[0] == WCHAR_CAST('h'))
        {
            Hour += val;
        }
        else if(pStop[0] == WCHAR_CAST('m'))
        {
            Minute += val;
        }
        else if(pStop[0] == WCHAR_CAST('s'))
        {
            Second += val;
        }
        else
        {
            ChkDR(DRM_E_INVALIDARG);
        }
        pCur = pStop + 1;
    }

    while(Second < 0)
    {
        Minute--;
        Second+=60;
    }
    while(Second > 59)
    {
        Minute++;
        Second-=60;
    }
    while(Minute < 0)
    {
        Hour--;
        Minute+=60;
    }
    while(Minute > 59)
    {
        Hour++;
        Minute-=60;
    }
    while(Hour < 0)
    {
        Day--;
        Hour+=24;
    }
    while(Hour > 23)
    {
        Day++;
        Hour-=24;
    }
    while(Day < 0)
    {
        Month--;
        switch(MonthNow)
        {
        case 0: //January
            Day+=31;
            MonthNow = 11;
            Year--;
            Month+=12;
            break;
        case 1: //February
            Day+=31;
            MonthNow--;
            break;
        case 2: //March
            if((Year % 4 == 0 && Year % 100 != 0) || Year % 400 == 0)
            {
                Day+=29;
            }
            else
            {
                Day+=28;
            }
            MonthNow--;
            break;
        case 3: //April
            Day+=31;
            MonthNow--;            
            break;
        case 4: //May
            Day+=30;
            MonthNow--;            
            break;
        case 5: //June
            Day+=31;
            MonthNow--;            
            break;
        case 6: //July
            Day+=30;
            MonthNow--;            
            break;
        case 7: //August
            Day+=31;
            MonthNow--;            
            break;
        case 8: //September
            Day+=31;
            MonthNow--;            
            break;
        case 9: //October
            Day+=30;
            MonthNow--;            
            break;
        case 10: //November
            Day+=31;
            MonthNow--;            
            break;
        case 11: //December
            Day+=30;
            MonthNow--;            
            break;
        default:
            MonthNow = 0;
            break;
        }
    }
    while(Day > 30)
    {
        Month++;
        switch(MonthNow)
        {
        case 0: //January
            Day-=31;
            MonthNow++;
            break;
        case 1: //February
            if((Year % 4 == 0 && Year % 100 != 0) || Year % 400 == 0)
            {
                Day-=29;
            }
            else
            {
                Day-=28;
            }
            MonthNow++;
            break;
        case 2: //March
            Day-=31;
            MonthNow++;
            break;
        case 3: //April
            Day-=30;
            MonthNow++;            
            break;
        case 4: //May
            Day-=31;
            MonthNow++;            
            break;
        case 5: //June
            Day-=30;
            MonthNow++;            
            break;
        case 6: //July
            Day-=31;
            MonthNow++;            
            break;
        case 7: //August
            Day-=31;
            MonthNow++;            
            break;
        case 8: //September
            Day-=30;
            MonthNow++;            
            break;
        case 9: //October
            Day-=31;
            MonthNow++;            
            break;
        case 10: //November
            Day-=30;
            MonthNow++;            
            break;
        case 11: //December
            Day-=31;
            MonthNow = 0;            
            Year++;
            Month-=12;
            break;
        default:
            MonthNow = 0;
            break;
        }
    }
    
    while(Month > 11)
    {
        Year++;
        Month-=12;
    }
    while(Month < 0)
    {
        Year--;
        Month+=12;
    }
    
    // one last check for months with less than 31 days
    // negative days should have been handled so we just have to check for invalid positive days
    switch(Month)
    {
    case 1: //February
        if((Year % 4 == 0 && Year % 100 != 0) || Year % 400 == 0)
        {
            // leap year
            if(Day > 28)
            {
                Month++;
                Day-=29;
            }
        }
        else
        {
            if(Day > 27)
            {
                Month++;
                Day-=28;
            }
        }
        break;
    case 3: // April
    case 5: // June
    case 8: //September
    case 10: // November
        if(Day > 29)
        {
            Month++;
            Day-=30;
        }
        break;
    }
    // Months/Year adjustments were mae before last day check and we didn't adjust across year line so no month check here

    Month++;    // make start at 1
    Day++;      // make start at 1

    rgchCatBuffer[0] = g_wchPound;
    TST_UTL_wcsncat(pDest, rgchCatBuffer, destLen);
    ZEROMEM(wszBuffer, SIZEOF(DRM_WCHAR)*BUFFERSIZE);
    ChkDR( DRM_UTL_NumberToString( Year,
                                   wszBuffer,
                                   BUFFERSIZE,
                                   0, 10, &cchScratch ) );

    TST_UTL_wcsncat(pDest, wszBuffer, destLen);

#define APPEND_ZERO_EXTENDED_DATE_PART(n)                   \
    if (n < 10)                                             \
    {                                                       \
        rgchCatBuffer[0] = g_wch0;                          \
        TST_UTL_wcsncat(pDest, rgchCatBuffer, destLen);     \
    }                                                       \
    ZEROMEM(wszBuffer, SIZEOF(DRM_WCHAR)*BUFFERSIZE);       \
    ChkDR( DRM_UTL_NumberToString( n,                       \
                                   wszBuffer,               \
                                   BUFFERSIZE,              \
                                   0, 10, &cchScratch ) );  \
    TST_UTL_wcsncat(pDest, wszBuffer, destLen);     

    APPEND_ZERO_EXTENDED_DATE_PART(Month);
    APPEND_ZERO_EXTENDED_DATE_PART(Day);
    rgchCatBuffer[0] = g_wchSpace;
    TST_UTL_wcsncat(pDest, rgchCatBuffer, destLen);
    APPEND_ZERO_EXTENDED_DATE_PART(Hour);
    rgchCatBuffer[0] = g_wchColon;
    TST_UTL_wcsncat(pDest, rgchCatBuffer, destLen);
    APPEND_ZERO_EXTENDED_DATE_PART(Minute);
    rgchCatBuffer[0] = g_wchColon;
    TST_UTL_wcsncat(pDest, rgchCatBuffer, destLen);
    APPEND_ZERO_EXTENDED_DATE_PART(Second);
    rgchCatBuffer[0] = g_wchZ;
    rgchCatBuffer[1] = g_wchPound;
    TST_UTL_wcsncat(pDest, rgchCatBuffer, destLen);

ErrorExit:
    return dr;
}
