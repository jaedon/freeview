/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxdm_pp_frd.c $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 9/11/12 1:02p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/commonutils/xdm/bxdm_pp_frd.c $
 * 
 * Hydra_Software_Devel/4   9/11/12 1:02p btosi
 * SW7405-4736: (Enhance XDM debug capabilities) standardized the instance
 * Id in the debug messages
 * 
 * Hydra_Software_Devel/3   6/18/12 9:19a btosi
 * SW7425-3177: tweaked sFrameRateLUT, added a comment
 * 
 * Hydra_Software_Devel/2   6/13/12 11:21a btosi
 * SW7425-3177: added support frame rates e10,e15,e20.
 * 
 * Hydra_Software_Devel/1   2/16/10 10:51a nilesh
 * SW7405-2993: Initial XDM version
 *
 ***************************************************************************/

#include "bstd.h"
#include "bdbg.h"                /* Dbglib */

#include "bxdm_pp.h"
#include "bxdm_pp_priv.h"
#include "bxdm_pp_frd.h"


BDBG_MODULE(BXDM_PPFRD);

const char BXDM_PictureProvider_P_DISPMGR_FRD_NODE[]="DMFRD:\t""$brcm_Revision: Hydra_Software_Devel/4 $";

static void BXDM_PPFRD_S_AddDeltaPTS(
   BXDM_PictureProvider_Handle hXdmPP,
   const BXDM_PPFP_P_DataType *pstDeltaPTS
   )
{

   if ( hXdmPP->stDMState.stDecode.stFRDStats.uiDeltaPTSCount < BXDM_PPFRD_P_MAX_DELTAPTS_SAMPLES )
   {
      /* Increment the PTS count */
      hXdmPP->stDMState.stDecode.stFRDStats.uiDeltaPTSCount++;
   }
   else
   {
      /* Subtract old entry from the running sum */
      BXDM_PPFP_P_FixPtSub(
         &hXdmPP->stDMState.stDecode.stFRDStats.stDeltaPTSRunningSum,
         &hXdmPP->stDMState.stDecode.stFRDStats.astDeltaPTS[hXdmPP->stDMState.stDecode.stFRDStats.uiDeltaPTSIndex],
         &hXdmPP->stDMState.stDecode.stFRDStats.stDeltaPTSRunningSum
         );
   }

   BXVD_DBG_MSG(hXdmPP, ("%x:[%02x.xxx] BXDM_PPFRD_S_AddDeltaPTS(%d.%d)[%d]", 
                              hXdmPP->stDMState.stDecode.stDebug.uiVsyncCount,
                              BXDM_PPDBG_FORMAT_INSTANCE_ID( hXdmPP ),
                              pstDeltaPTS->uiWhole,
                              pstDeltaPTS->uiFractional,
                              hXdmPP->stDMState.stDecode.stFRDStats.uiDeltaPTSCount));

   /* Replace the old entry with the new entry */
   hXdmPP->stDMState.stDecode.stFRDStats.astDeltaPTS[hXdmPP->stDMState.stDecode.stFRDStats.uiDeltaPTSIndex] = *pstDeltaPTS;

   /* Update running sum */
   BXDM_PPFP_P_FixPtAdd(
      &hXdmPP->stDMState.stDecode.stFRDStats.stDeltaPTSRunningSum,
      &hXdmPP->stDMState.stDecode.stFRDStats.astDeltaPTS[hXdmPP->stDMState.stDecode.stFRDStats.uiDeltaPTSIndex],
      &hXdmPP->stDMState.stDecode.stFRDStats.stDeltaPTSRunningSum);

   /* Increment PTS index */
   hXdmPP->stDMState.stDecode.stFRDStats.uiDeltaPTSIndex++;
   if ( hXdmPP->stDMState.stDecode.stFRDStats.uiDeltaPTSIndex >= BXDM_PPFRD_P_MAX_DELTAPTS_SAMPLES )
   {
      hXdmPP->stDMState.stDecode.stFRDStats.uiDeltaPTSIndex = 0;
   }
}

static void BXDM_PPFRD_S_AddNumElements(
   BXDM_PictureProvider_Handle hXdmPP,
   const uint32_t uiNumElements
   )
{
   if ( hXdmPP->stDMState.stDecode.stFRDStats.uiNumPicturesCount < BXDM_PPFRD_P_MAX_NUMELEMENTS_SAMPLES )
   {
      /* Increment the PTS count */
      hXdmPP->stDMState.stDecode.stFRDStats.uiNumPicturesCount++;
   }
   else
   {
      /* Subtract old entry from the running sum */
      hXdmPP->stDMState.stDecode.stFRDStats.uiNumElementsRunningSum -= hXdmPP->stDMState.stDecode.stFRDStats.auiNumElements[hXdmPP->stDMState.stDecode.stFRDStats.uiNumElementsIndex];
   }

   BXVD_DBG_MSG(hXdmPP, ("%x:[%02x.xxx] BXDM_PPFRD_S_AddNumElements(%d)[%d]", 
                              hXdmPP->stDMState.stDecode.stDebug.uiVsyncCount,
                              BXDM_PPDBG_FORMAT_INSTANCE_ID( hXdmPP ),
                              uiNumElements,
                              hXdmPP->stDMState.stDecode.stFRDStats.uiNumPicturesCount));

   /* Replace the old entry with the new entry */
   hXdmPP->stDMState.stDecode.stFRDStats.auiNumElements[hXdmPP->stDMState.stDecode.stFRDStats.uiNumElementsIndex] = uiNumElements;

   /* Update running sum */
   hXdmPP->stDMState.stDecode.stFRDStats.uiNumElementsRunningSum += hXdmPP->stDMState.stDecode.stFRDStats.auiNumElements[hXdmPP->stDMState.stDecode.stFRDStats.uiNumElementsIndex];

   /* Increment PTS index */
   hXdmPP->stDMState.stDecode.stFRDStats.uiNumElementsIndex++;
   if ( hXdmPP->stDMState.stDecode.stFRDStats.uiNumElementsIndex >= BXDM_PPFRD_P_MAX_NUMELEMENTS_SAMPLES )
   {
      hXdmPP->stDMState.stDecode.stFRDStats.uiNumElementsIndex = 0;
   }
}

#define BXDM_PPFRD_P_DISCONTINUITY_DELTA_PTS_THRESHOLD 45000

BERR_Code BXDM_PPFRD_P_AddPTS(
   BXDM_PictureProvider_Handle hXdmPP,
   uint32_t uiPTS,
   bool bPTSValid,
   uint32_t uiNumElements
   )
{
   BXDM_PPFP_P_DataType stDeltaPTS;
   uint32_t i;

   BXVD_DBG_MSG(hXdmPP, ("%x:[%02x.xxx] BXDM_PPFRD_P_AddPTS(%d, %d)", 
                              hXdmPP->stDMState.stDecode.stDebug.uiVsyncCount,
                              BXDM_PPDBG_FORMAT_INSTANCE_ID( hXdmPP ),
                              uiPTS,
                              bPTSValid));

   if ( true == bPTSValid )
   {
      /* We have a valid PTS */
      if ( true == hXdmPP->stDMState.stDecode.stFRDStats.bLastPTSValid )
      {
         /* We have another valid PTS, so we can compute and insert
          * the delta PTS value(s) into the dPTS queue */
         if ( uiPTS > hXdmPP->stDMState.stDecode.stFRDStats.uiLastPTS )
         {
            stDeltaPTS.uiWhole = uiPTS - hXdmPP->stDMState.stDecode.stFRDStats.uiLastPTS;
            stDeltaPTS.uiFractional = 0;
            BXDM_PPFP_P_FixPtDiv(&stDeltaPTS,
                                 hXdmPP->stDMState.stDecode.stFRDStats.uiPicturesSinceLastValidPTS,
                                 &stDeltaPTS);

            /* Handle PTS discontinuity scenario */
            if ( stDeltaPTS.uiWhole < BXDM_PPFRD_P_DISCONTINUITY_DELTA_PTS_THRESHOLD )
            {
               for (i = 0; i < hXdmPP->stDMState.stDecode.stFRDStats.uiPicturesSinceLastValidPTS; i++)
               {
                  /* Add new deltaPTS to queue.  We add one deltaPTS entry
                   * per picture. */
                  BXDM_PPFRD_S_AddDeltaPTS(
                     hXdmPP,
                     &stDeltaPTS);
               }
            }
            else
            {
               BXVD_DBG_MSG(hXdmPP, ("%x:[%02x.xxx] Warning, PTS discontinuity detected (dPTS=%d).  Ignoring deltaPTS during discontinuity.", 
                                          hXdmPP->stDMState.stDecode.stDebug.uiVsyncCount,
                                          BXDM_PPDBG_FORMAT_INSTANCE_ID( hXdmPP ),
                                          stDeltaPTS.uiWhole));
            }
         }
         else
         {
            BXVD_DBG_MSG(hXdmPP, ("%x:[%02x.xxx] Warning, PTS wrap/discontinuity detected (%08x -> %08x).  Ignoring deltaPTS during wrap/discontinuity.", 
                                          hXdmPP->stDMState.stDecode.stDebug.uiVsyncCount,
                                          BXDM_PPDBG_FORMAT_INSTANCE_ID( hXdmPP ),
                                          hXdmPP->stDMState.stDecode.stFRDStats.uiLastPTS,
                                          uiPTS ));
         }
      }

      hXdmPP->stDMState.stDecode.stFRDStats.uiLastPTS = uiPTS;
      hXdmPP->stDMState.stDecode.stFRDStats.bLastPTSValid = true;
      hXdmPP->stDMState.stDecode.stFRDStats.uiPicturesSinceLastValidPTS = uiNumElements;
   }
   else
   {
      hXdmPP->stDMState.stDecode.stFRDStats.uiPicturesSinceLastValidPTS += uiNumElements;
   }

   /* Add new deltaPTS to queue.  We add one deltaPTS entry
    * per picture. */
   BXDM_PPFRD_S_AddNumElements(
      hXdmPP,
      uiNumElements);

   return BERR_SUCCESS;
}

typedef struct BXDM_PPFRD_P_DeltaPtsToFrameRateMap
{
   BXDM_PPFP_P_DataType stDeltaPTS;
   BAVC_FrameRateCode eFrameRate;
} BXDM_PPFRD_P_DeltaPtsToFrameRateMap;

/* The table below is used to map the average delta PTS to a frame rate.
 * The range for a given frame rate is:
 * - low end:  45000/(frame_rate) - 45 (1 millisecond)
 * - high end: the preceding frame's rates low end
 *
 * The exceptions to this are the non-whole number rates; 7.493, 14.985, 23.976,
 * 29.97, 59.95.  In these cases, the low end is just a few ticks less 
 * than 45000/(frame_rate)
 */
static const BXDM_PPFRD_P_DeltaPtsToFrameRateMap sFrameRateLUT[] =
{
   { { 6051, 1 }, BAVC_FrameRateCode_eUnknown },   /* 6051 = (45000/7.493)    + 45     */
   { { 5961, 1 }, BAVC_FrameRateCode_e7_493 },     /* 5961 = (45000/7.493)    - 45     */
   { { 4455, 1 }, BAVC_FrameRateCode_e10 },        /* 4455 = (45000/10)       - 45     */
   { { 3002, 1 }, BAVC_FrameRateCode_e14_985 },    /* 3002 = (45000/14.958)   - 1      */
   { { 2955, 1 }, BAVC_FrameRateCode_e15 },        /* 2955 = (45000/15)       - 45     */
   { { 2205, 1 }, BAVC_FrameRateCode_e20 },        /* 2205 = (45000/20)       - 45     */
   { { 1875, 1 }, BAVC_FrameRateCode_e23_976 },    /* 1875 = (45000/23.976)   - ~2     */
   { { 1830, 1 }, BAVC_FrameRateCode_e24 },        /* 1830 = (45000/24)       - 45     */
   { { 1755, 1 }, BAVC_FrameRateCode_e25 },        /* 1755 = (45000/25)       - 45     */
   { { 1500, 1 }, BAVC_FrameRateCode_e29_97 },     /* 1500 = (45000/29.97)    - ~1.5   */
   { { 1455, 1 }, BAVC_FrameRateCode_e30 },        /* 1455 = (45000/30)       - 45     */
   { {  855, 1 }, BAVC_FrameRateCode_e50 },        /*  855 = (45000/50)       - 45     */
   { {  750, 1 }, BAVC_FrameRateCode_e59_94 },     /*  750 = (45000/59.94)    - ~1     */
   { {  705, 1 }, BAVC_FrameRateCode_e60 },        /*  705 = (45000/60)       - 45     */
   { {    0, 0 }, BAVC_FrameRateCode_eUnknown }
};

static const uint32_t sFrameRateStabiltyLUT[BXDM_PictureProvider_P_MAX_FRAMERATE] =
{
   30, /* BAVC_FrameRateCode_eUnknown */
   24, /* BAVC_FrameRateCode_e23_976 */
   24, /* BAVC_FrameRateCode_e24 */
   25, /* BAVC_FrameRateCode_e25 */
   30, /* BAVC_FrameRateCode_e29_97 */
   30, /* BAVC_FrameRateCode_e30 */
   50, /* BAVC_FrameRateCode_e50 */
   60, /* BAVC_FrameRateCode_e59_94 */
   60, /* BAVC_FrameRateCode_e60 */
   15, /* BAVC_FrameRateCode_e14_985 */
   8,  /* BAVC_FrameRateCode_e7_493 */
   10, /* BAVC_FrameRateCode_e10 */
   15, /* BAVC_FrameRateCode_e15 */
   20  /* BAVC_FrameRateCode_e20 */
};

BERR_Code BXDM_PPFRD_P_GetFrameRate(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_ClockRate eClockRate,
   BXDM_PictureProvider_FrameRateDetectionMode eFrameRateDetectionMode,
   BAVC_FrameRateCode *peFrameRate
   )
{
   BXDM_PPFP_P_DataType stAverageDeltaPTS;
   uint32_t uiFrameRateIndex = 0;

   *peFrameRate = BAVC_FrameRateCode_eUnknown;
   stAverageDeltaPTS.uiWhole = 0;
   stAverageDeltaPTS.uiFractional = 0;

   if ( BXDM_PictureProvider_FrameRateDetectionMode_eOff != eFrameRateDetectionMode )
   {
      /* Calculate the average deltaPTS */
      if ( hXdmPP->stDMState.stDecode.stFRDStats.uiDeltaPTSCount > 0 )
      {
         if ( BXDM_PictureProvider_P_ClockRate_eDirecTV == eClockRate )
         {
            /* Scale down by 600 if we have a DirecTV clock rate */
            BXDM_PPFP_P_FixPtDiv(
               &hXdmPP->stDMState.stDecode.stFRDStats.stDeltaPTSRunningSum,
               hXdmPP->stDMState.stDecode.stFRDStats.uiDeltaPTSCount*600,
               &stAverageDeltaPTS
               );
         }
         else
         {
            BXDM_PPFP_P_FixPtDiv(
               &hXdmPP->stDMState.stDecode.stFRDStats.stDeltaPTSRunningSum,
               hXdmPP->stDMState.stDecode.stFRDStats.uiDeltaPTSCount,
               &stAverageDeltaPTS
               );
         }

         /* The average delta PTS we've computed so far is the avg dPTS
          * per _field_.  We need to convert it to the avg dPTS per
          * _frame_.  We assume that 2 interlaced fields equals 1 frame.
          * So, we calculate the following:
          *
          * (avg dPTS)/picture = (avg dPTS/field)*(floor(avg fields/picture))
          *
          * This equation accounts for 3:2 pulldown content which has 2.5
          * fields/picture.
          */
         BXDM_PPFP_P_FixPtMult(
            &stAverageDeltaPTS,
            hXdmPP->stDMState.stDecode.stFRDStats.uiNumElementsRunningSum/hXdmPP->stDMState.stDecode.stFRDStats.uiNumPicturesCount,
            &stAverageDeltaPTS);

         /* Map the average deltaPTS to a known frame rate */
         while ( ( stAverageDeltaPTS.uiWhole < sFrameRateLUT[uiFrameRateIndex].stDeltaPTS.uiWhole )
                 || ( ( stAverageDeltaPTS.uiWhole == sFrameRateLUT[uiFrameRateIndex].stDeltaPTS.uiWhole )
                      && ( stAverageDeltaPTS.uiFractional < sFrameRateLUT[uiFrameRateIndex].stDeltaPTS.uiFractional ) ) )
         {
            uiFrameRateIndex++;
         }

         /* Determine which frame rate type to return */
         switch ( eFrameRateDetectionMode )
         {
            case BXDM_PictureProvider_FrameRateDetectionMode_eStable:
            {
               /* Keep track of how many pictures the frame rate is stable for */
               if (sFrameRateLUT[uiFrameRateIndex].eFrameRate == hXdmPP->stDMState.stDecode.stFRDStats.eLastCalculatedFrameRate)
               {
                  hXdmPP->stDMState.stDecode.stFRDStats.uiNumPicturesCalculatedFrameRateWasStable++;
               }
               else
               {
                  hXdmPP->stDMState.stDecode.stFRDStats.uiNumPicturesCalculatedFrameRateWasStable = 0;
               }
               hXdmPP->stDMState.stDecode.stFRDStats.eLastCalculatedFrameRate = sFrameRateLUT[uiFrameRateIndex].eFrameRate;

               if ( hXdmPP->stDMState.stDecode.stFRDStats.uiNumPicturesCalculatedFrameRateWasStable >= sFrameRateStabiltyLUT[sFrameRateLUT[uiFrameRateIndex].eFrameRate] )
               {
                  hXdmPP->stDMState.stDecode.stFRDStats.eLastReportedStableFrameRate = sFrameRateLUT[uiFrameRateIndex].eFrameRate;
               }

               *peFrameRate = hXdmPP->stDMState.stDecode.stFRDStats.eLastReportedStableFrameRate;
            }
            break;

            case BXDM_PictureProvider_FrameRateDetectionMode_eFast:
               *peFrameRate = sFrameRateLUT[uiFrameRateIndex].eFrameRate;
               break;

            default:
               *peFrameRate = BAVC_FrameRateCode_eUnknown;
         }
      }
      else
      {
         /* We don't have enough data to compute a frame rate */
         *peFrameRate = BAVC_FrameRateCode_eUnknown;
      }
   }

   BXVD_DBG_MSG(hXdmPP, ("%x:[%02x.xxx] BXDM_PPFRD_P_GetFrameRate((%d.%d)/%d = %d.%d --> %d [%d])", 
                              hXdmPP->stDMState.stDecode.stDebug.uiVsyncCount,
                              BXDM_PPDBG_FORMAT_INSTANCE_ID( hXdmPP ),
                              hXdmPP->stDMState.stDecode.stFRDStats.stDeltaPTSRunningSum.uiWhole,
                              hXdmPP->stDMState.stDecode.stFRDStats.stDeltaPTSRunningSum.uiFractional,
                              hXdmPP->stDMState.stDecode.stFRDStats.uiDeltaPTSCount,
                              stAverageDeltaPTS.uiWhole,
                              stAverageDeltaPTS.uiFractional,
                              *peFrameRate,
                              eFrameRateDetectionMode
                        ));

   return BERR_SUCCESS;
}
