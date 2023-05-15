/*****************************************************************************
**
**  Name: mt_spuravoid.c
**
**  Copyright 2006-2008 Microtune, Inc. All Rights Reserved
**
**  This source code file contains confidential information and/or trade
**  secrets of Microtune, Inc. or its affiliates and is subject to the
**  terms of your confidentiality agreement with Microtune, Inc. or one of
**  its affiliates, as applicable.
**
*****************************************************************************/

/*****************************************************************************
**
**  Name: mt_spuravoid.c
**
**  Description:    Microtune spur avoidance software module.
**                  Supports Microtune tuner drivers.
**
**  CVS ID:         $Id: mt_spuravoid.c,v 1.4 2008/11/05 13:46:19 software Exp $
**  CVS Source:     $Source: /export/vol0/cvsroot/software/tuners/MT2063/mt_spuravoid.c,v $
**
**  Revision History:
**
**   SCR      Date      Author  Description
**  -------------------------------------------------------------------------
**   082   03-25-2005    JWS    Original multi-tuner support - requires
**                              MTxxxx_CNT declarations
**   096   04-06-2005    DAD    Ver 1.11: Fix divide by 0 error if maxH==0.
**   094   04-06-2005    JWS    Ver 1.11 Added uceil and ufloor to get rid
**                              of compiler warnings
**   N/A   04-07-2005    DAD    Ver 1.13: Merged single- and multi-tuner spur
**                              avoidance into a single module.
**   103   01-31-2005    DAD    Ver 1.14: In MT_AddExclZone(), if the range
**                              (f_min, f_max) < 0, ignore the entry.
**   115   03-23-2007    DAD    Fix declaration of spur due to truncation
**                              errors.
**   117   03-29-2007    RSK    Ver 1.15: Re-wrote to match search order from
**                              tuner DLL.
**   137   06-18-2007    DAD    Ver 1.16: Fix possible divide-by-0 error for
**                              multi-tuners that have
**                              (delta IF1) > (f_out-f_outbw/2).
**   147   07-27-2007    RSK    Ver 1.17: Corrected calculation (-) to (+)
**                              Added logic to force f_Center within 1/2 f_Step.
**   177 S 02-26-2008    RSK    Ver 1.18: Corrected calculation using LO1 > MAX/2
**                              Type casts added to preserve correct sign.
**   195 I 06-17-2008    RSK    Ver 1.19: Refactoring avoidance of DECT
**                              frequencies into MT_ResetExclZones().
**   N/A I 06-20-2008    RSK    Ver 1.21: New VERSION number for ver checking.
**   199   08-06-2008    JWS    Ver 1.22: Added 0x1x1 spur frequency calculations
**                              and indicate success of AddExclZone operation.
**   200   08-07-2008    JWS    Ver 1.23: Moved definition of DECT avoid constants
**                              so users could access them more easily.
**
*****************************************************************************/
#include "mt_spuravoid.h"
#include <stdlib.h>
#include <assert.h>  /*  debug purposes  */

#if !defined(MT_TUNER_CNT)
#error MT_TUNER_CNT is not defined (see mt_userdef.h)
#endif

#if MT_TUNER_CNT == 0
#error MT_TUNER_CNT must be updated in mt_userdef.h
#endif

/*  Version of this module                         */
#define VERSION 10203            /*  Version 01.23 */


/*  Implement ceiling, floor functions.  */
#define ceil(n, d) (((n) < 0) ? (-((-(n))/(d))) : (n)/(d) + ((n)%(d) != 0))
#define uceil(n, d) ((n)/(d) + ((n)%(d) != 0))
#define floor(n, d) (((n) < 0) ? (-((-(n))/(d))) - ((n)%(d) != 0) : (n)/(d))
#define ufloor(n, d) ((n)/(d))

/*  Macros to check for DECT exclusion options */
#define MT_EXCLUDE_US_DECT_FREQUENCIES(s) (((s) & MT_AVOID_US_DECT) != 0)
#define MT_EXCLUDE_EURO_DECT_FREQUENCIES(s) (((s) & MT_AVOID_EURO_DECT) != 0)


struct MT_FIFZone_t
{
    SData_t         min_;
    SData_t         max_;
};

#if MT_TUNER_CNT > 1
static MT_AvoidSpursData_t* TunerList[MT_TUNER_CNT];
static UData_t              TunerCount = 0;
#endif

UData_t MT_RegisterTuner(MT_AvoidSpursData_t* pAS_Info)
{
#if MT_TUNER_CNT == 1
    pAS_Info->nAS_Algorithm = 1;
    return MT_OK;
#else
    UData_t index;

    pAS_Info->nAS_Algorithm = 2;

    /*
    **  Check to see if tuner is already registered
    */
    for (index = 0; index < TunerCount; index++)
    {
        if (TunerList[index] == pAS_Info)
        {
            return MT_OK;  /* Already here - no problem  */
        }
    }

    /*
    ** Add tuner to list - if there is room.
    */
    if (TunerCount < MT_TUNER_CNT)
    {
        TunerList[TunerCount] = pAS_Info;
        TunerCount++;
        return MT_OK;
    }
    else
        return MT_TUNER_CNT_ERR;
#endif
}


void MT_UnRegisterTuner(MT_AvoidSpursData_t* pAS_Info)
{
#if MT_TUNER_CNT == 1
    pAS_Info;
#else

    UData_t index;

    for (index = 0; index < TunerCount; index++)
    {
        if (TunerList[index] == pAS_Info)
        {
            TunerList[index] = TunerList[--TunerCount];
        }
    }
#endif
}


/*
**  Reset all exclusion zones.
**  Add zones to protect the PLL FracN regions near zero
**
**   195 I 06-17-2008    RSK    Ver 1.19: Refactoring avoidance of DECT
**                              frequencies into MT_ResetExclZones().
*/
void MT_ResetExclZones(MT_AvoidSpursData_t* pAS_Info)
{
    UData_t center;
#if MT_TUNER_CNT > 1
    UData_t index;
    MT_AvoidSpursData_t* adj;
#endif

    pAS_Info->nZones = 0;                           /*  this clears the used list  */
    pAS_Info->usedZones = NULL;                     /*  reset ptr                  */
    pAS_Info->freeZones = NULL;                     /*  reset ptr                  */

    center = pAS_Info->f_ref * ((pAS_Info->f_if1_Center - pAS_Info->f_if1_bw/2 + pAS_Info->f_in) / pAS_Info->f_ref) - pAS_Info->f_in;
    while (center < pAS_Info->f_if1_Center + pAS_Info->f_if1_bw/2 + pAS_Info->f_LO1_FracN_Avoid)
    {
        /*  Exclude LO1 FracN  */
        MT_AddExclZone(pAS_Info, center-pAS_Info->f_LO1_FracN_Avoid, center-1);
        MT_AddExclZone(pAS_Info, center+1, center+pAS_Info->f_LO1_FracN_Avoid);
        center += pAS_Info->f_ref;
    }

    center = pAS_Info->f_ref * ((pAS_Info->f_if1_Center - pAS_Info->f_if1_bw/2 - pAS_Info->f_out) / pAS_Info->f_ref) + pAS_Info->f_out;
    while (center < pAS_Info->f_if1_Center + pAS_Info->f_if1_bw/2 + pAS_Info->f_LO2_FracN_Avoid)
    {
        /*  Exclude LO2 FracN  */
        MT_AddExclZone(pAS_Info, center-pAS_Info->f_LO2_FracN_Avoid, center-1);
        MT_AddExclZone(pAS_Info, center+1, center+pAS_Info->f_LO2_FracN_Avoid);
        center += pAS_Info->f_ref;
    }

    if( MT_EXCLUDE_US_DECT_FREQUENCIES(pAS_Info->avoidDECT) )
    {
        /*  Exclude LO1 values that conflict with DECT channels */
        MT_AddExclZone(pAS_Info, 1920836000 - pAS_Info->f_in, 1922236000 - pAS_Info->f_in);  /* Ctr = 1921.536 */
        MT_AddExclZone(pAS_Info, 1922564000 - pAS_Info->f_in, 1923964000 - pAS_Info->f_in);  /* Ctr = 1923.264 */
        MT_AddExclZone(pAS_Info, 1924292000 - pAS_Info->f_in, 1925692000 - pAS_Info->f_in);  /* Ctr = 1924.992 */
        MT_AddExclZone(pAS_Info, 1926020000 - pAS_Info->f_in, 1927420000 - pAS_Info->f_in);  /* Ctr = 1926.720 */
        MT_AddExclZone(pAS_Info, 1927748000 - pAS_Info->f_in, 1929148000 - pAS_Info->f_in);  /* Ctr = 1928.448 */
    }

    if( MT_EXCLUDE_EURO_DECT_FREQUENCIES(pAS_Info->avoidDECT) )
    {
        MT_AddExclZone(pAS_Info, 1896644000 - pAS_Info->f_in, 1898044000 - pAS_Info->f_in);  /* Ctr = 1897.344 */
        MT_AddExclZone(pAS_Info, 1894916000 - pAS_Info->f_in, 1896316000 - pAS_Info->f_in);  /* Ctr = 1895.616 */
        MT_AddExclZone(pAS_Info, 1893188000 - pAS_Info->f_in, 1894588000 - pAS_Info->f_in);  /* Ctr = 1893.888 */
        MT_AddExclZone(pAS_Info, 1891460000 - pAS_Info->f_in, 1892860000 - pAS_Info->f_in);  /* Ctr = 1892.16  */
        MT_AddExclZone(pAS_Info, 1889732000 - pAS_Info->f_in, 1891132000 - pAS_Info->f_in);  /* Ctr = 1890.432 */
        MT_AddExclZone(pAS_Info, 1888004000 - pAS_Info->f_in, 1889404000 - pAS_Info->f_in);  /* Ctr = 1888.704 */
        MT_AddExclZone(pAS_Info, 1886276000 - pAS_Info->f_in, 1887676000 - pAS_Info->f_in);  /* Ctr = 1886.976 */
        MT_AddExclZone(pAS_Info, 1884548000 - pAS_Info->f_in, 1885948000 - pAS_Info->f_in);  /* Ctr = 1885.248 */
        MT_AddExclZone(pAS_Info, 1882820000 - pAS_Info->f_in, 1884220000 - pAS_Info->f_in);  /* Ctr = 1883.52  */
        MT_AddExclZone(pAS_Info, 1881092000 - pAS_Info->f_in, 1882492000 - pAS_Info->f_in);  /* Ctr = 1881.792 */
    }

#if MT_TUNER_CNT > 1
    /*
    ** Iterate through all adjacent tuners and exclude frequencies related to them
    */
    for (index = 0; index < TunerCount; ++index)
    {
        adj = TunerList[index];
        if (pAS_Info == adj)    /* skip over our own data, don't process it */
            continue;

        /*
        **  Add 1st IF exclusion zone covering adjacent tuner's LO2
        **  at "adjfLO2 + f_out" +/- m_MinLOSpacing
        */
        if (adj->f_LO2 != 0)
            MT_AddExclZone(pAS_Info,
                           (adj->f_LO2 + pAS_Info->f_out) - pAS_Info->f_min_LO_Separation,
                           (adj->f_LO2 + pAS_Info->f_out) + pAS_Info->f_min_LO_Separation );

        /*
        **  Add 1st IF exclusion zone covering adjacent tuner's LO1
        **  at "adjfLO1 - f_in" +/- m_MinLOSpacing
        */
        if (adj->f_LO1 != 0)
            MT_AddExclZone(pAS_Info,
                           (adj->f_LO1 - pAS_Info->f_in) - pAS_Info->f_min_LO_Separation,
                           (adj->f_LO1 - pAS_Info->f_in) + pAS_Info->f_min_LO_Separation );
    }
#endif
}


static struct MT_ExclZone_t* InsertNode(MT_AvoidSpursData_t* pAS_Info,
                                  struct MT_ExclZone_t* pPrevNode)
{
    struct MT_ExclZone_t* pNode;
    /*  Check for a node in the free list  */
    if (pAS_Info->freeZones != NULL)
    {
        /*  Use one from the free list  */
        pNode = pAS_Info->freeZones;
        pAS_Info->freeZones = pNode->next_;
    }
    else
    {
        /*  Grab a node from the array  */
        pNode = &pAS_Info->MT_ExclZones[pAS_Info->nZones];
    }

    if (pPrevNode != NULL)
    {
        pNode->next_ = pPrevNode->next_;
        pPrevNode->next_ = pNode;
    }
    else    /*  insert at the beginning of the list  */
    {
        pNode->next_ = pAS_Info->usedZones;
        pAS_Info->usedZones = pNode;
    }

    pAS_Info->nZones++;
    return pNode;
}


static struct MT_ExclZone_t* RemoveNode(MT_AvoidSpursData_t* pAS_Info,
                                  struct MT_ExclZone_t* pPrevNode,
                                  struct MT_ExclZone_t* pNodeToRemove)
{
    struct MT_ExclZone_t* pNext = pNodeToRemove->next_;

    /*  Make previous node point to the subsequent node  */
    if (pPrevNode != NULL)
        pPrevNode->next_ = pNext;

    /*  Add pNodeToRemove to the beginning of the freeZones  */
    pNodeToRemove->next_ = pAS_Info->freeZones;
    pAS_Info->freeZones = pNodeToRemove;

    /*  Decrement node count  */
    pAS_Info->nZones--;

    return pNext;
}


/*****************************************************************************
**
**  Name: MT_AddExclZone
**
**  Description:    Add (and merge) an exclusion zone into the list.
**                  If the range (f_min, f_max) is totally outside the
**                  1st IF BW, ignore the entry.
**                  If the range (f_min, f_max) is negative, ignore the entry.
**
**  Revision History:
**
**   SCR      Date      Author  Description
**  -------------------------------------------------------------------------
**   103   01-31-2005    DAD    Ver 1.14: In MT_AddExclZone(), if the range
**                              (f_min, f_max) < 0, ignore the entry.
**   199   08-06-2008    JWS    Ver 1.22: Indicate success of addition operation.
**
*****************************************************************************/
UData_t MT_AddExclZone(MT_AvoidSpursData_t* pAS_Info,
                       UData_t f_min,
                       UData_t f_max)
{
    UData_t status = 1;  /* Assume addition is possible */

    /*  Check to see if this overlaps the 1st IF filter  */
    if ((f_max > (pAS_Info->f_if1_Center - (pAS_Info->f_if1_bw / 2)))
        && (f_min < (pAS_Info->f_if1_Center + (pAS_Info->f_if1_bw / 2)))
        && (f_min < f_max))
    {
        /*
        **                1           2          3        4         5          6
        **
        **   New entry:  |---|    |--|        |--|       |-|      |---|         |--|
        **                     or          or        or       or        or
        **   Existing:  |--|          |--|      |--|    |---|      |-|      |--|
        */
        struct MT_ExclZone_t* pNode = pAS_Info->usedZones;
        struct MT_ExclZone_t* pPrev = NULL;
        struct MT_ExclZone_t* pNext = NULL;

        /*  Check for our place in the list  */
        while ((pNode != NULL) && (pNode->max_ < f_min))
        {
            pPrev = pNode;
            pNode = pNode->next_;
        }

        if ((pNode != NULL) && (pNode->min_ < f_max))
        {
            /*  Combine me with pNode  */
            if (f_min < pNode->min_)
                pNode->min_ = f_min;
            if (f_max > pNode->max_)
                pNode->max_ = f_max;
        }
        else
        {
            pNode = InsertNode(pAS_Info, pPrev);
            pNode->min_ = f_min;
            pNode->max_ = f_max;
        }

        /*  Look for merging possibilities  */
        pNext = pNode->next_;
        while ((pNext != NULL) && (pNext->min_ < pNode->max_))
        {
            if (pNext->max_ > pNode->max_)
                pNode->max_ = pNext->max_;
            pNext = RemoveNode(pAS_Info, pNode, pNext);  /*  Remove pNext, return ptr to pNext->next  */
        }
    }
    else
        status = 0;  /* Did not add the zone - caller determines whether this is a problem */

    return status;
}


/*****************************************************************************
**
**  Name: MT_ChooseFirstIF
**
**  Description:    Choose the best available 1st IF
**                  If f_Desired is not excluded, choose that first.
**                  Otherwise, return the value closest to f_Center that is
**                  not excluded
**
**  Revision History:
**
**   SCR      Date      Author  Description
**  -------------------------------------------------------------------------
**   117   03-29-2007    RSK    Ver 1.15: Re-wrote to match search order from
**                              tuner DLL.
**   147   07-27-2007    RSK    Ver 1.17: Corrected calculation (-) to (+)
**                              Added logic to force f_Center within 1/2 f_Step.
**
*****************************************************************************/
UData_t MT_ChooseFirstIF(MT_AvoidSpursData_t* pAS_Info)
{
    /*
    ** Update "f_Desired" to be the nearest "combinational-multiple" of "f_LO1_Step".
    ** The resulting number, F_LO1 must be a multiple of f_LO1_Step.  And F_LO1 is the arithmetic sum
    ** of f_in + f_Center.  Neither f_in, nor f_Center must be a multiple of f_LO1_Step.
    ** However, the sum must be.
    */
    const UData_t f_Desired = pAS_Info->f_LO1_Step * ((pAS_Info->f_if1_Request + pAS_Info->f_in + pAS_Info->f_LO1_Step/2) / pAS_Info->f_LO1_Step) - pAS_Info->f_in;
    const UData_t f_Step = (pAS_Info->f_LO1_Step > pAS_Info->f_LO2_Step) ? pAS_Info->f_LO1_Step : pAS_Info->f_LO2_Step;
    UData_t f_Center;

    SData_t i;
    SData_t j = 0;
    UData_t bDesiredExcluded = 0;
    UData_t bZeroExcluded = 0;
    SData_t tmpMin, tmpMax;
    SData_t bestDiff;
    struct MT_ExclZone_t* pNode = pAS_Info->usedZones;
    struct MT_FIFZone_t zones[MAX_ZONES];

    if (pAS_Info->nZones == 0)
        return f_Desired;

    /*  f_Center needs to be an integer multiple of f_Step away from f_Desired */
    if (pAS_Info->f_if1_Center > f_Desired)
        f_Center = f_Desired + f_Step * ((pAS_Info->f_if1_Center - f_Desired + f_Step/2) / f_Step);
    else
        f_Center = f_Desired - f_Step * ((f_Desired - pAS_Info->f_if1_Center + f_Step/2) / f_Step);

    assert(abs((SData_t) f_Center - (SData_t) pAS_Info->f_if1_Center) <= (SData_t) (f_Step/2));

    /*  Take MT_ExclZones, center around f_Center and change the resolution to f_Step  */
    while (pNode != NULL)
    {
        /*  floor function  */
        tmpMin = floor((SData_t) (pNode->min_ - f_Center), (SData_t) f_Step);

        /*  ceil function  */
        tmpMax = ceil((SData_t) (pNode->max_ - f_Center), (SData_t) f_Step);

        if ((pNode->min_ < f_Desired) && (pNode->max_ > f_Desired))
            bDesiredExcluded = 1;

        if ((tmpMin < 0) && (tmpMax > 0))
            bZeroExcluded = 1;

        /*  See if this zone overlaps the previous  */
        if ((j>0) && (tmpMin < zones[j-1].max_))
            zones[j-1].max_ = tmpMax;
        else
        {
            /*  Add new zone  */
            assert(j<MAX_ZONES);
            zones[j].min_ = tmpMin;
            zones[j].max_ = tmpMax;
            j++;
        }
        pNode = pNode->next_;
    }

    /*
    **  If the desired is okay, return with it
    */
    if (bDesiredExcluded == 0)
        return f_Desired;

    /*
    **  If the desired is excluded and the center is okay, return with it
    */
    if (bZeroExcluded == 0)
        return f_Center;

    /*  Find the value closest to 0 (f_Center)  */
    bestDiff = zones[0].min_;
    for (i=0; i<j; i++)
    {
        if (abs(zones[i].min_) < abs(bestDiff)) bestDiff = zones[i].min_;
        if (abs(zones[i].max_) < abs(bestDiff)) bestDiff = zones[i].max_;
    }


    if (bestDiff < 0)
        return f_Center - ((UData_t) (-bestDiff) * f_Step);

    return f_Center + (bestDiff * f_Step);
}


/****************************************************************************
**
**  Name: gcd
**
**  Description:    Uses Euclid's algorithm
**
**  Parameters:     u, v     - unsigned values whose GCD is desired.
**
**  Global:         None
**
**  Returns:        greatest common divisor of u and v, if either value
**                  is 0, the other value is returned as the result.
**
**  Dependencies:   None.
**
**  Revision History:
**
**   SCR      Date      Author  Description
**  -------------------------------------------------------------------------
**   N/A   06-01-2004    JWS    Original
**   N/A   08-03-2004    DAD    Changed to Euclid's since it can handle
**                              unsigned numbers.
**
****************************************************************************/
static UData_t gcd(UData_t u, UData_t v)
{
    UData_t r;

    while (v != 0)
    {
        r = u % v;
        u = v;
        v = r;
    }

    return u;
}

/****************************************************************************
**
**  Name: umax
**
**  Description:    Implements a simple maximum function for unsigned numbers.
**                  Implemented as a function rather than a macro to avoid
**                  multiple evaluation of the calling parameters.
**
**  Parameters:     a, b     - Values to be compared
**
**  Global:         None
**
**  Returns:        larger of the input values.
**
**  Dependencies:   None.
**
**  Revision History:
**
**   SCR      Date      Author  Description
**  -------------------------------------------------------------------------
**   N/A   06-02-2004    JWS    Original
**
****************************************************************************/
static UData_t umax(UData_t a, UData_t b)
{
    return (a >= b) ? a : b;
}

#if MT_TUNER_CNT > 1
static SData_t RoundAwayFromZero(SData_t n, SData_t d)
{
    return (n<0) ? floor(n, d) : ceil(n, d);
}

/****************************************************************************
**
**  Name: IsSpurInAdjTunerBand
**
**  Description:    Checks to see if a spur will be present within the IF's
**                  bandwidth or near the zero IF.
**                  (fIFOut +/- fIFBW/2, -fIFOut +/- fIFBW/2)
**                                  and
**                  (0 +/- fZIFBW/2)
**
**                    ma   mb               me   mf               mc   md
**                  <--+-+-+-----------------+-+-+-----------------+-+-+-->
**                     |   ^                   0                   ^   |
**                     ^   b=-fIFOut+fIFBW/2      -b=+fIFOut-fIFBW/2   ^
**                     a=-fIFOut-fIFBW/2              -a=+fIFOut+fIFBW/2
**
**                  Note that some equations are doubled to prevent round-off
**                  problems when calculating fIFBW/2
**
**                  The spur frequencies are computed as:
**
**                     fSpur = n * f1 - m * f2 - fOffset
**
**  Parameters:     f1      - The 1st local oscillator (LO) frequency
**                            of the tuner whose output we are examining
**                  f2      - The 1st local oscillator (LO) frequency
**                            of the adjacent tuner
**                  fOffset - The 2nd local oscillator of the tuner whose
**                            output we are examining
**                  fIFOut  - Output IF center frequency
**                  fIFBW   - Output IF Bandwidth
**                  nMaxH   - max # of LO harmonics to search
**                  fp      - If spur, positive distance to spur-free band edge (returned)
**                  fm      - If spur, negative distance to spur-free band edge (returned)
**
**  Returns:        1 if an LO spur would be present, otherwise 0.
**
**  Dependencies:   None.
**
**  Revision History:
**
**   SCR      Date      Author  Description
**  -------------------------------------------------------------------------
**   N/A   01-21-2005    JWS    Original, adapted from MT_DoubleConversion.
**   115   03-23-2007    DAD    Fix declaration of spur due to truncation
**                              errors.
**   137   06-18-2007    DAD    Ver 1.16: Fix possible divide-by-0 error for
**                              multi-tuners that have
**                              (delta IF1) > (f_out-f_outbw/2).
**   177 S 02-26-2008    RSK    Ver 1.18: Corrected calculation using LO1 > MAX/2
**                              Type casts added to preserve correct sign.
**   199   08-06-2008    JWS    Ver 1.22: Added 0x1x1 spur frequency calculations.
**
****************************************************************************/
static UData_t IsSpurInAdjTunerBand(UData_t bIsMyOutput,
                                    UData_t f1,
                                    UData_t f2,
                                    UData_t fOffset,
                                    UData_t fIFOut,
                                    UData_t fIFBW,
                                    UData_t fZIFBW,
                                    UData_t nMaxH,
                                    UData_t *fp,
                                    UData_t *fm)
{
    UData_t bSpurFound = 0;

    const UData_t fHalf_IFBW = fIFBW / 2;
    const UData_t fHalf_ZIFBW = fZIFBW / 2;

    /* Calculate a scale factor for all frequencies, so that our
       calculations all stay within 31 bits */
    const UData_t f_Scale = ((f1 + (fOffset + fIFOut + fHalf_IFBW) / nMaxH) / (MAX_UDATA/2 / nMaxH)) + 1;

    /*
    **  After this scaling, _f1, _f2, and _f3 are guaranteed to fit into
    **  signed data types (smaller than MAX_UDATA/2)
    */
    const SData_t _f1 = (SData_t) (     f1 / f_Scale);
    const SData_t _f2 = (SData_t) (     f2 / f_Scale);
    const SData_t _f3 = (SData_t) (fOffset / f_Scale);

    const SData_t c = (SData_t) (fIFOut - fHalf_IFBW) / (SData_t) f_Scale;
    const SData_t d = (SData_t) ((fIFOut + fHalf_IFBW) / f_Scale);
    const SData_t f = (SData_t) (fHalf_ZIFBW / f_Scale);

    SData_t  ma, mb, mc, md, me, mf;

    SData_t  fp_ = 0;
    SData_t  fm_ = 0;
    SData_t  n;


    /*
    **  If the other tuner does not have an LO frequency defined,
    **  assume that we cannot interfere with it
    */
    if (f2 == 0)
        return 0;


    /* Check out all multiples of f1 from -nMaxH to +nMaxH */
    for (n = -(SData_t)nMaxH; n <= (SData_t)nMaxH; ++n)
    {
        const SData_t nf1 = n*_f1;
        md = floor(_f3 + d - nf1, _f2);

        /* If # f2 harmonics > nMaxH, then no spurs present */
        if (md <= -(SData_t) nMaxH )
            break;

        ma = floor(_f3 - d - nf1, _f2);
        if ((ma == md) || (ma >= (SData_t) (nMaxH)))
            continue;

        mc = floor(_f3 + c - nf1, _f2);
        if (mc != md)
        {
            const SData_t m = md;
            const SData_t fspur = (nf1 + m*_f2 - _f3);
            const SData_t den = (bIsMyOutput ? n - 1 : n);
            if (den == 0)
            {
                fp_ = (d - fspur)* f_Scale;
                fm_ = (fspur - c)* f_Scale;
            }
            else
            {
                fp_ = (SData_t) RoundAwayFromZero((d - fspur)* f_Scale, den);
                fm_ = (SData_t) RoundAwayFromZero((fspur - c)* f_Scale, den);
            }
            if (((UData_t)abs(fm_) >= f_Scale) && ((UData_t)abs(fp_) >= f_Scale))
            {
                bSpurFound = 1;
                break;
            }
        }

        /* Location of Zero-IF-spur to be checked */
        mf = floor(_f3 + f - nf1, _f2);
        me = floor(_f3 - f - nf1, _f2);
        if (me != mf)
        {
            const SData_t m = mf;
            const SData_t fspur = (nf1 + m*_f2 - _f3);
            const SData_t den = (bIsMyOutput ? n - 1 : n);
            if (den == 0)
            {
                fp_ = (d - fspur)* f_Scale;
                fm_ = (fspur - c)* f_Scale;
            }
            else
            {
                fp_ = (SData_t) RoundAwayFromZero((f - fspur)* f_Scale, den);
                fm_ = (SData_t) RoundAwayFromZero((fspur + f)* f_Scale, den);
            }
            if (((UData_t)abs(fm_) >= f_Scale) && ((UData_t)abs(fp_) >= f_Scale))
            {
                bSpurFound = 1;
                break;
            }
        }

        mb = floor(_f3 - c - nf1, _f2);
        if (ma != mb)
        {
            const SData_t m = mb;
            const SData_t fspur = (nf1 + m*_f2 - _f3);
            const SData_t den = (bIsMyOutput ? n - 1 : n);
            if (den == 0)
            {
                fp_ = (d - fspur)* f_Scale;
                fm_ = (fspur - c)* f_Scale;
            }
            else
            {
                fp_ = (SData_t) RoundAwayFromZero((-c - fspur)* f_Scale, den);
                fm_ = (SData_t) RoundAwayFromZero((fspur +d)* f_Scale, den);
            }
            if (((UData_t)abs(fm_) >= f_Scale) && ((UData_t)abs(fp_) >= f_Scale))
            {
                bSpurFound = 1;
                break;
            }
        }
    }

    /*
    **  Verify that fm & fp are both positive
    **  Add one to ensure next 1st IF choice is not right on the edge
    */
    if (fp_ < 0)
    {
        *fp = -fm_ + 1;
        *fm = -fp_ + 1;
    }
    else if (fp_ > 0)
    {
        *fp = fp_ + 1;
        *fm = fm_ + 1;
    }
    else
    {
        *fp = 1;
        *fm = abs(fm_) + 1;
    }

    return bSpurFound;
}
#endif

/****************************************************************************
**
**  Name: IsSpurInBand
**
**  Description:    Checks to see if a spur will be present within the IF's
**                  bandwidth. (fIFOut +/- fIFBW, -fIFOut +/- fIFBW)
**
**                    ma   mb                                     mc   md
**                  <--+-+-+-------------------+-------------------+-+-+-->
**                     |   ^                   0                   ^   |
**                     ^   b=-fIFOut+fIFBW/2      -b=+fIFOut-fIFBW/2   ^
**                     a=-fIFOut-fIFBW/2              -a=+fIFOut+fIFBW/2
**
**                  Note that some equations are doubled to prevent round-off
**                  problems when calculating fIFBW/2
**
**  Parameters:     pAS_Info - Avoid Spurs information block
**                  fm       - If spur, amount f_IF1 has to move negative
**                  fp       - If spur, amount f_IF1 has to move positive
**
**  Global:         None
**
**  Returns:        1 if an LO spur would be present, otherwise 0.
**
**  Dependencies:   None.
**
**  Revision History:
**
**   SCR      Date      Author  Description
**  -------------------------------------------------------------------------
**   N/A   11-28-2002    DAD    Implemented algorithm from applied patent
**
****************************************************************************/
static UData_t IsSpurInBand(MT_AvoidSpursData_t* pAS_Info,
                            UData_t* fm,
                            UData_t* fp)
{
    /*
    **  Calculate LO frequency settings.
    */
    UData_t n, n0;
    const UData_t f_LO1 = pAS_Info->f_LO1;
    const UData_t f_LO2 = pAS_Info->f_LO2;
    const UData_t d = pAS_Info->f_out + pAS_Info->f_out_bw/2;
    const UData_t c = d - pAS_Info->f_out_bw;
    const UData_t f = pAS_Info->f_zif_bw/2;
    const UData_t f_Scale = (f_LO1 / (MAX_UDATA/2 / pAS_Info->maxH1)) + 1;
    SData_t f_nsLO1, f_nsLO2;
    SData_t f_Spur;
    UData_t ma, mb, mc, md, me, mf;
    UData_t lo_gcd, gd_Scale, gc_Scale, gf_Scale, hgds, hgfs, hgcs;
#if MT_TUNER_CNT > 1
    UData_t index;

    MT_AvoidSpursData_t *adj;
#endif
    *fm = 0;

    /*
    ** For each edge (d, c & f), calculate a scale, based on the gcd
    ** of f_LO1, f_LO2 and the edge value.  Use the larger of this
    ** gcd-based scale factor or f_Scale.
    */
    lo_gcd = gcd(f_LO1, f_LO2);
    gd_Scale = umax((UData_t) gcd(lo_gcd, d), f_Scale);
    hgds = gd_Scale/2;
    gc_Scale = umax((UData_t) gcd(lo_gcd, c), f_Scale);
    hgcs = gc_Scale/2;
    gf_Scale = umax((UData_t) gcd(lo_gcd, f), f_Scale);
    hgfs = gf_Scale/2;

    n0 = uceil(f_LO2 - d, f_LO1 - f_LO2);

    /*  Check out all multiples of LO1 from n0 to m_maxLOSpurHarmonic  */
    for (n=n0; n<=pAS_Info->maxH1; ++n)
    {
        md = (n*((f_LO1+hgds)/gd_Scale) - ((d+hgds)/gd_Scale)) / ((f_LO2+hgds)/gd_Scale);

        /*  If # fLO2 harmonics > m_maxLOSpurHarmonic, then no spurs present  */
        if (md >= pAS_Info->maxH1)
            break;

        ma = (n*((f_LO1+hgds)/gd_Scale) + ((d+hgds)/gd_Scale)) / ((f_LO2+hgds)/gd_Scale);

        /*  If no spurs between +/- (f_out + f_IFBW/2), then try next harmonic  */
        if (md == ma)
            continue;

        mc = (n*((f_LO1+hgcs)/gc_Scale) - ((c+hgcs)/gc_Scale)) / ((f_LO2+hgcs)/gc_Scale);
        if (mc != md)
        {
            f_nsLO1 = (SData_t) (n*(f_LO1/gc_Scale));
            f_nsLO2 = (SData_t) (mc*(f_LO2/gc_Scale));
            f_Spur = (gc_Scale * (f_nsLO1 - f_nsLO2)) + n*(f_LO1 % gc_Scale) - mc*(f_LO2 % gc_Scale);

            *fp = ((f_Spur - (SData_t) c) / (mc - n)) + 1;
            *fm = (((SData_t) d - f_Spur) / (mc - n)) + 1;
            return 1;
        }

        /*  Location of Zero-IF-spur to be checked  */
        me = (n*((f_LO1+hgfs)/gf_Scale) + ((f+hgfs)/gf_Scale)) / ((f_LO2+hgfs)/gf_Scale);
        mf = (n*((f_LO1+hgfs)/gf_Scale) - ((f+hgfs)/gf_Scale)) / ((f_LO2+hgfs)/gf_Scale);
        if (me != mf)
        {
            f_nsLO1 = n*(f_LO1/gf_Scale);
            f_nsLO2 = me*(f_LO2/gf_Scale);
            f_Spur = (gf_Scale * (f_nsLO1 - f_nsLO2)) + n*(f_LO1 % gf_Scale) - me*(f_LO2 % gf_Scale);

            *fp = ((f_Spur + (SData_t) f) / (me - n)) + 1;
            *fm = (((SData_t) f - f_Spur) / (me - n)) + 1;
            return 1;
        }

        mb = (n*((f_LO1+hgcs)/gc_Scale) + ((c+hgcs)/gc_Scale)) / ((f_LO2+hgcs)/gc_Scale);
        if (ma != mb)
        {
            f_nsLO1 = n*(f_LO1/gc_Scale);
            f_nsLO2 = ma*(f_LO2/gc_Scale);
            f_Spur = (gc_Scale * (f_nsLO1 - f_nsLO2)) + n*(f_LO1 % gc_Scale) - ma*(f_LO2 % gc_Scale);

            *fp = (((SData_t) d + f_Spur) / (ma - n)) + 1;
            *fm = (-(f_Spur + (SData_t) c) / (ma - n)) + 1;
            return 1;
        }
    }

#if MT_TUNER_CNT > 1
    /*  If no spur found, see if there are more tuners on the same board  */
    for (index = 0; index < TunerCount; ++index)
    {
        adj = TunerList[index];
        if (pAS_Info == adj)    /* skip over our own data, don't process it */
            continue;

        /*  Look for LO-related spurs from the adjacent tuner generated into my IF output  */
        if (IsSpurInAdjTunerBand(1,                   /*  check my IF output                     */
                                 pAS_Info->f_LO1,     /*  my fLO1                                */
                                 adj->f_LO1,          /*  the other tuner's fLO1                 */
                                 pAS_Info->f_LO2,     /*  my fLO2                                */
                                 pAS_Info->f_out,     /*  my fOut                                */
                                 pAS_Info->f_out_bw,  /*  my output IF bandwidth                 */
                                 pAS_Info->f_zif_bw,  /*  my Zero-IF bandwidth                   */
                                 pAS_Info->maxH2,
                                 fp,                  /*  minimum amount to move LO's positive   */
                                 fm))                 /*  miminum amount to move LO's negative   */
            return 1;
        /*  Look for LO-related spurs from my tuner generated into the adjacent tuner's IF output  */
        if (IsSpurInAdjTunerBand(0,                   /*  check his IF output                    */
                                 pAS_Info->f_LO1,     /*  my fLO1                                */
                                 adj->f_LO1,          /*  the other tuner's fLO1                 */
                                 adj->f_LO2,          /*  the other tuner's fLO2                 */
                                 adj->f_out,          /*  the other tuner's fOut                 */
                                 adj->f_out_bw,       /*  the other tuner's output IF bandwidth  */
                                 pAS_Info->f_zif_bw,  /*  the other tuner's Zero-IF bandwidth    */
                                 adj->maxH2,
                                 fp,                  /*  minimum amount to move LO's positive   */
                                 fm))                 /*  miminum amount to move LO's negative   */
            return 1;
    }
#endif
    /*  No spurs found  */
    return 0;
}


/*****************************************************************************
**
**  Name: MT_AvoidSpurs
**
**  Description:    Main entry point to avoid spurs.
**                  Checks for existing spurs in present LO1, LO2 freqs
**                  and if present, chooses spur-free LO1, LO2 combination
**                  that tunes the same input/output frequencies.
**
**  Revision History:
**
**   SCR      Date      Author  Description
**  -------------------------------------------------------------------------
**   096   04-06-2005    DAD    Ver 1.11: Fix divide by 0 error if maxH==0.
**
*****************************************************************************/
UData_t MT_AvoidSpurs(Handle_t h,
                      MT_AvoidSpursData_t* pAS_Info)
{
    UData_t status = MT_OK;
    UData_t fm, fp;                     /*  restricted range on LO's        */
    pAS_Info->bSpurAvoided = 0;
    pAS_Info->nSpursFound = 0;
    h=h;                          /*  warning expected: code has no effect    */

    if (pAS_Info->maxH1 == 0)
        return MT_OK;

    /*
    **  Avoid LO Generated Spurs
    **
    **  Make sure that have no LO-related spurs within the IF output
    **  bandwidth.
    **
    **  If there is an LO spur in this band, start at the current IF1 frequency
    **  and work out until we find a spur-free frequency or run up against the
    **  1st IF SAW band edge.  Use temporary copies of fLO1 and fLO2 so that they
    **  will be unchanged if a spur-free setting is not found.
    */
    pAS_Info->bSpurPresent = IsSpurInBand(pAS_Info, &fm, &fp);
    if (pAS_Info->bSpurPresent)
    {
        UData_t zfIF1 = pAS_Info->f_LO1 - pAS_Info->f_in; /*  current attempt at a 1st IF  */
        UData_t zfLO1 = pAS_Info->f_LO1;         /*  current attempt at an LO1 freq  */
        UData_t zfLO2 = pAS_Info->f_LO2;         /*  current attempt at an LO2 freq  */
        UData_t delta_IF1;
        UData_t new_IF1;
        UData_t added;                           /*  success of Excl Zone addition  */

        /*
        **  Spur was found, attempt to find a spur-free 1st IF
        */
        do
        {
            pAS_Info->nSpursFound++;

            /*  Raise f_IF1_upper, if needed  */
            added = MT_AddExclZone(pAS_Info, zfIF1 - fm, zfIF1 + fp);

            /*
            **  If Exclusion Zone could NOT be added, then we'll just keep picking
            **  the same bad point, so exit this loop and report that there is a
            **  spur present in the output
            */
            if (!added)
                break;

            /*  Choose next IF1 that is closest to f_IF1_CENTER              */
            new_IF1 = MT_ChooseFirstIF(pAS_Info);

            if (new_IF1 > zfIF1)
            {
                pAS_Info->f_LO1 += (new_IF1 - zfIF1);
                pAS_Info->f_LO2 += (new_IF1 - zfIF1);
            }
            else
            {
                pAS_Info->f_LO1 -= (zfIF1 - new_IF1);
                pAS_Info->f_LO2 -= (zfIF1 - new_IF1);
            }
            zfIF1 = new_IF1;

            if (zfIF1 > pAS_Info->f_if1_Center)
                delta_IF1 = zfIF1 - pAS_Info->f_if1_Center;
            else
                delta_IF1 = pAS_Info->f_if1_Center - zfIF1;
        }
        /*
        **  Continue while the new 1st IF is still within the 1st IF bandwidth
        **  and there is a spur in the band (again)
        */
        while ((2*delta_IF1 + pAS_Info->f_out_bw <= pAS_Info->f_if1_bw) &&
              (pAS_Info->bSpurPresent = IsSpurInBand(pAS_Info, &fm, &fp)));

        /*
        ** Use the LO-spur free values found.  If the search went all the way to
        ** the 1st IF band edge and always found spurs, just leave the original
        ** choice.  It's as "good" as any other.
        */
        if (pAS_Info->bSpurPresent == 1)
        {
            status |= MT_SPUR_PRESENT;
            pAS_Info->f_LO1 = zfLO1;
            pAS_Info->f_LO2 = zfLO2;
        }
        else
            pAS_Info->bSpurAvoided = 1;
    }

    status |= ((pAS_Info->nSpursFound << MT_SPUR_SHIFT) & MT_SPUR_CNT_MASK);

    return (status);
}


UData_t MT_AvoidSpursVersion(void)
{
    return (VERSION);
}

