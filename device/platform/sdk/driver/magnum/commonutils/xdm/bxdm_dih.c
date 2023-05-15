/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxdm_dih.c $
 * $brcm_Revision: Hydra_Software_Devel/13 $
 * $brcm_Date: 7/27/12 11:12a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/commonutils/xdm/bxdm_dih.c $
 * 
 * Hydra_Software_Devel/13   7/27/12 11:12a btosi
 * SW7425-1064: tweaked debug messages
 * 
 * Hydra_Software_Devel/12   2/9/12 12:07p btosi
 * SW7425-2253: added BXDM_MFD1 debug message
 * 
 * Hydra_Software_Devel/11   2/8/12 2:19p btosi
 * SW7425-2253: added support for MFD.bChannelChange
 * 
 * Hydra_Software_Devel/10   12/14/11 1:54p btosi
 * SW7405-3984: added
 * BXDM_DisplayInterruptHandler_(Set/Get)PictureProviderMode_isr
 * 
 * Hydra_Software_Devel/9   9/8/11 1:27p btosi
 * SW7425-1064: added debug messages to help debug dual decode 3D
 * 
 * Hydra_Software_Devel/8   6/9/11 1:44p nilesh
 * SW7425-44: Set bIgnorePicture and bStallStc to TRUE (Decoder Underflow)
 * by default if no channels are started.
 * 
 * Hydra_Software_Devel/7   9/28/10 3:10p btosi
 * SW7405-4736: added back in the line I removed by mistake
 * 
 * Hydra_Software_Devel/6   9/23/10 3:09p btosi
 * SW7405-4736: add support for a XDM instance ID to help debug multi-
 * channel issues
 * 
 * Hydra_Software_Devel/5   8/10/10 1:08p btosi
 * SW7405-4736: added support for printing the channel ID before/after
 * calling the PictureProvider
 * 
 * Hydra_Software_Devel/4   4/13/10 12:18p nilesh
 * SW7125-262: Fixed memory leak in BXDM_DisplayInterruptHandler_Create()
 * when errors occur
 * 
 * Hydra_Software_Devel/3   4/13/10 12:13p nilesh
 * SW7125-263: Fixed memory leak in
 * BXDM_DisplayInterruptHandler_AddPictureProviderInterface() when errors
 * occur
 * 
 * Hydra_Software_Devel/2   3/1/10 5:10p nilesh
 * SW7405-2993: Fixed coverity warnings
 * 
 * Hydra_Software_Devel/1   2/16/10 10:50a nilesh
 * SW7405-2993: Initial XDM version
 *
 ***************************************************************************/

#include "bstd.h"
#include "bkni.h"          /* For malloc */
#include "bkni_multi.h"    /* For mutex */
#include "bmem.h"

#include "blst_slist.h"

#include "bavc.h"
#include "bxdm.h"
#include "bxdm_dih.h"

BDBG_MODULE(BXDM_DIH);
BDBG_FILE_MODULE(BXDM_DIHCTL);
BDBG_FILE_MODULE(BXDM_DIHDBG);
BDBG_FILE_MODULE(BXDM_MFD1);

typedef struct BXDM_DisplayInterruptHandler_P_PictureProviderCallback
{
   BLST_S_ENTRY(BXDM_DisplayInterruptHandler_P_PictureProviderCallback) link;

   BXDM_DisplayInterruptHandler_PictureProvider_isr fCallback;
   void *pPrivateContext;
   uint32_t uiVDCRectangleNumber;
   BXDM_DisplayInterruptHandler_PictureProviderMode ePictureProviderMode; /* SW7405-3984: enabled or disabled */
} BXDM_DisplayInterruptHandler_P_PictureProviderCallback;

typedef struct BXDM_DisplayInterruptHandler_P_PictureDataReadyHandler
{
      BXDM_DisplayInterruptHandler_PictureDataReady_isr fCallback;
      void *pPrivateContext;
      int32_t iPrivateParam;
} BXDM_DisplayInterruptHandler_P_PictureDataReadyHandler;

typedef struct BXDM_DisplayInterruptHandler_P_DisplayHandler
{
      BXDM_DisplayInterruptHandler_Display_isr fCallback;
      void *pPrivateContext;
      int32_t iPrivateParam;
} BXDM_DisplayInterruptHandler_P_DisplayHandler;

typedef struct BXDM_DisplayInterruptHandler_P_Context
{
      BAVC_MFD_Picture stMFDPicture; /* Used if no picture providers exist */
      BXDM_DisplayInterruptHandler_P_PictureDataReadyHandler stPictureDataReadyHandler;
      BXDM_DisplayInterruptHandler_P_DisplayHandler stDisplayHandler;

      BKNI_MutexHandle hMutex; /* Mutex to lock access to PictureProviderList*/

      /* Picture Provider List */
      BLST_S_HEAD(BXDM_DisplayInterruptHandler_P_PictureProviderList, BXDM_DisplayInterruptHandler_P_PictureProviderCallback) hPictureProviderList;
} BXDM_DisplayInterruptHandler_P_Context;

BERR_Code
BXDM_DisplayInterruptHandler_Create(
         BXDM_DisplayInterruptHandler_Handle *phXdmDih
         )
{
   BXDM_DisplayInterruptHandler_P_Context *pXdmDih = NULL;

   BERR_Code rc;

   BDBG_ENTER( BXDM_DisplayInterruptHandler_Create );

   BDBG_ASSERT( phXdmDih );

   /* Set the handle to NULL in case the allocation fails */
   *phXdmDih = NULL;

   pXdmDih = ( BXDM_DisplayInterruptHandler_P_Context* ) BKNI_Malloc( sizeof( BXDM_DisplayInterruptHandler_P_Context ) );
   if ( NULL == pXdmDih )
   {
      return BERR_TRACE( BERR_OUT_OF_SYSTEM_MEMORY );
   }

   /* Zero out the newly allocated context */
   BKNI_Memset( ( void * ) pXdmDih, 0, sizeof( BXDM_DisplayInterruptHandler_P_Context ) );

   rc = BKNI_CreateMutex( &(pXdmDih->hMutex) );

   if ( BERR_SUCCESS != rc ) 
   {
      BKNI_Free( pXdmDih );
      return BERR_TRACE( rc );
   }


   rc = BKNI_AcquireMutex( pXdmDih->hMutex );

   if ( BERR_SUCCESS != rc )
   {
      BKNI_Free( pXdmDih );
      return BERR_TRACE( rc );
   }

   BLST_S_INIT( &pXdmDih->hPictureProviderList );

   BKNI_ReleaseMutex( pXdmDih->hMutex );

   *phXdmDih = pXdmDih;

   BDBG_MODULE_MSG( BXDM_DIHCTL, ("Create: hDih:%08x", pXdmDih));

   BDBG_LEAVE( BXDM_DisplayInterruptHandler_Create );

   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BXDM_DisplayInterruptHandler_Destroy(
         BXDM_DisplayInterruptHandler_Handle hXdmDih
         )
{
   BXDM_DisplayInterruptHandler_P_PictureProviderCallback *pPictureProvider;

   BERR_Code rc;

   BDBG_ENTER( BXDM_DisplayInterruptHandler_Destroy );

   if ( NULL != hXdmDih )
   {
      pPictureProvider = BLST_S_FIRST( &hXdmDih->hPictureProviderList );

      rc = BKNI_AcquireMutex(  hXdmDih->hMutex );

      if ( BERR_SUCCESS != rc )
      {
         return BERR_TRACE( rc );
      }

      while ( NULL != pPictureProvider )
      {
         BLST_S_REMOVE_HEAD(
                  &hXdmDih->hPictureProviderList,
                  link);

         BKNI_Free(pPictureProvider);

         pPictureProvider = BLST_S_FIRST( &hXdmDih->hPictureProviderList );
      }

      BLST_S_INIT( &hXdmDih->hPictureProviderList );

      BKNI_ReleaseMutex( hXdmDih->hMutex );

      BKNI_DestroyMutex( hXdmDih->hMutex );

      BDBG_MODULE_MSG( BXDM_DIHCTL, ("Destroy: hDih:%08x", hXdmDih));

      BKNI_Free( hXdmDih );
   }

   BDBG_LEAVE( BXDM_DisplayInterruptHandler_Destroy );

   return BERR_TRACE( BERR_SUCCESS );
}

static BXDM_DisplayInterruptHandler_P_PictureProviderCallback* BXDM_DisplayInterruptHandler_S_FindPictureProvider(
         BXDM_DisplayInterruptHandler_Handle hXdmDih,
         const BXDM_DisplayInterruptHandler_PictureProvider_isr fCallback,
         const void *pPrivateContext
         )
{
   BXDM_DisplayInterruptHandler_P_PictureProviderCallback *pPictureProvider;

   BDBG_ENTER( BXDM_DisplayInterruptHandler_S_FindPictureProvider );

   pPictureProvider = BLST_S_FIRST( &hXdmDih->hPictureProviderList );
   while ( NULL != pPictureProvider )
   {
      if ( ( pPictureProvider->fCallback == fCallback )
           && ( pPictureProvider->pPrivateContext == pPrivateContext ) )
      {
         return pPictureProvider;
      }

      pPictureProvider = BLST_S_NEXT( pPictureProvider, link );
   }

   BDBG_LEAVE( BXDM_DisplayInterruptHandler_S_FindPictureProvider );

   return NULL;
}


BERR_Code
BXDM_DIH_AddPictureProviderInterface_GetDefaultSettings ( 
   BXDM_DisplayInterruptHandler_AddPictureProviderInterface_Settings *pstDefSettings
   )
{
   BDBG_ENTER( BXDM_DIH_AddPictureProviderInterface_GetDefaultSettings );

   BDBG_ASSERT(pstDefSettings);

   pstDefSettings->uiVDCRectangleNumber = 0;

   BDBG_LEAVE( BXDM_DIH_AddPictureProviderInterface_GetDefaultSettings );

   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BXDM_DisplayInterruptHandler_AddPictureProviderInterface(
         BXDM_DisplayInterruptHandler_Handle hXdmDih,
         BXDM_DisplayInterruptHandler_PictureProvider_isr fCallback,
         void *pPrivateContext,
         BXDM_DisplayInterruptHandler_AddPictureProviderInterface_Settings *pDefSettings
         )
{
   BXDM_DisplayInterruptHandler_P_PictureProviderCallback *pPictureProvider;

   BERR_Code rc;

   BDBG_ENTER( BXDM_DisplayInterruptHandler_AddPictureProviderInterface );

   BDBG_ASSERT( fCallback );

   pPictureProvider = BXDM_DisplayInterruptHandler_S_FindPictureProvider(
               hXdmDih,
               fCallback,
               pPrivateContext);

   if ( NULL == pPictureProvider )
   {
      pPictureProvider = ( BXDM_DisplayInterruptHandler_P_PictureProviderCallback* ) BKNI_Malloc( sizeof ( BXDM_DisplayInterruptHandler_P_PictureProviderCallback ) );
      if ( NULL == pPictureProvider )
      {
         return BERR_TRACE( BERR_OUT_OF_SYSTEM_MEMORY );
      }

      /* Zero out the newly allocated context */
      BKNI_Memset( ( void * ) pPictureProvider, 0, sizeof( BXDM_DisplayInterruptHandler_P_PictureProviderCallback) );

      pPictureProvider->fCallback = fCallback;
      pPictureProvider->pPrivateContext = pPrivateContext;
      pPictureProvider->uiVDCRectangleNumber = pDefSettings->uiVDCRectangleNumber;

      /* SW7405-3984: mark the PictureProvider as enabled. */
      pPictureProvider->ePictureProviderMode = BXDM_DisplayInterruptHandler_PictureProviderMode_eEnabled;

      /* BLST list key's in descending order, needed to negate to get in ascending order */
      pPictureProvider->uiVDCRectangleNumber = (pPictureProvider->uiVDCRectangleNumber + 1) * (-1);
      
      rc = BKNI_AcquireMutex( hXdmDih->hMutex );

      if ( BERR_SUCCESS != rc ) 
      {
         BKNI_Free( pPictureProvider );
         return BERR_TRACE( rc );
      }

      BDBG_MODULE_MSG( BXDM_DIHCTL, ("hDih:%08x pp:%08x add hXdm:%08x ch:%d",
                  hXdmDih,
                  pPictureProvider,
                  pPrivateContext,
                  pDefSettings->uiVDCRectangleNumber
                  ));

      BLST_S_DICT_ADD(
               &hXdmDih->hPictureProviderList,
               pPictureProvider,
               BXDM_DisplayInterruptHandler_P_PictureProviderCallback,
               uiVDCRectangleNumber,
               link,
               err_duplicate);

      BKNI_ReleaseMutex( hXdmDih->hMutex );

   }
   else
   {
      BDBG_ERR(("Specified Picture Provider already exists"));
      return BERR_TRACE( BERR_INVALID_PARAMETER );
   }

   BDBG_LEAVE( BXDM_DisplayInterruptHandler_AddPictureProviderInterface );

   return BERR_TRACE( BERR_SUCCESS );

err_duplicate:
   BKNI_Free( pPictureProvider );
   BKNI_ReleaseMutex( hXdmDih->hMutex );
   BDBG_ERR(("Specified Picture Provider already exists"));
   return BERR_TRACE( BERR_INVALID_PARAMETER );
}

BERR_Code
BXDM_DisplayInterruptHandler_RemovePictureProviderInterface(
         BXDM_DisplayInterruptHandler_Handle hXdmDih,
         BXDM_DisplayInterruptHandler_PictureProvider_isr fCallback,
         void *pPrivateContext
         )
{
   BXDM_DisplayInterruptHandler_P_PictureProviderCallback *pPictureProvider;

   BERR_Code rc;

   BDBG_ENTER( BXDM_DisplayInterruptHandler_RemovePictureProviderInterface );

   /* Find the picture provider */
   pPictureProvider = BXDM_DisplayInterruptHandler_S_FindPictureProvider(
            hXdmDih,
            fCallback,
            pPrivateContext);

   if ( NULL != pPictureProvider )
   {
      rc = BKNI_AcquireMutex( hXdmDih->hMutex );

      if ( BERR_SUCCESS != rc ) 
      {
         return BERR_TRACE( rc );
      }

      BDBG_MODULE_MSG( BXDM_DIHCTL, ("hDih:%08x pp:%08x remove hXdm:%08x",
                  hXdmDih,
                  pPictureProvider,
                  pPrivateContext
                  ));

      BLST_S_REMOVE(
               &hXdmDih->hPictureProviderList,
               pPictureProvider,
               BXDM_DisplayInterruptHandler_P_PictureProviderCallback,
               link);

      BKNI_ReleaseMutex( hXdmDih->hMutex );

      BKNI_Free( pPictureProvider );
   }
   else
   {
      BDBG_ERR(("Specified Picture Provider could not be found"));
      return BERR_TRACE( BERR_INVALID_PARAMETER );
   }

   BDBG_LEAVE( BXDM_DisplayInterruptHandler_RemovePictureProviderInterface );

   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BXDM_DisplayInterruptHandler_Callback_isr(
         void *pPrivateContext,
         BXDM_DisplayInterruptInfo *pstDisplayInterruptInfo
         )
{
   BXDM_DisplayInterruptHandler_Handle hXdmDih = ( BXDM_DisplayInterruptHandler_Handle ) pPrivateContext;
   BAVC_MFD_Picture *pMFDPictureList = NULL;
   BAVC_MFD_Picture *pMFDPicture = NULL;
   BXDM_DisplayInterruptHandler_P_PictureProviderCallback *pPictureProvider;

   BDBG_ENTER( BXDM_DisplayInterruptHandler_Callback_isr );

   BDBG_ASSERT( hXdmDih );

   pPictureProvider = BLST_S_FIRST( &hXdmDih->hPictureProviderList );

   /* SW7405-3085: If we only have one picture provider outputting to this display,
    * we set the interrupt count to invalid so that the picture provider
    * does not use it to synchronize interlaced output on interlaced displays.
    *
    * For non-mosaic display, this will result in the best cadence output on
    * progressive displays.
    *
    * For mosaic display, we leave the interrupt count valid to ensure all
    * picture providers synchronize to the correct target polarity when on a
    * progressive display
    */
   if ( ( NULL != pPictureProvider )
        && ( NULL == BLST_S_NEXT( pPictureProvider, link ) )
      )
   {
      pstDisplayInterruptInfo->stInterruptCount.bValid = false;
   }

   while ( NULL != pPictureProvider )
   {

      BDBG_MODULE_MSG( BXDM_DIHDBG, ("hDih:%08x pp:%08x hXdm:%08x(%d) ch:%d %d",
                  hXdmDih,
                  pPictureProvider,
                  pPictureProvider->pPrivateContext,
                  pPictureProvider->ePictureProviderMode,
                  ((pPictureProvider->uiVDCRectangleNumber * (-1)) - 1),
                  pstDisplayInterruptInfo->stInterruptCount.uiValue
                  ));

      /* SW7405-3984: only execute the callback if the PictureProvider is enabled.
       */
      if ( BXDM_DisplayInterruptHandler_PictureProviderMode_eEnabled == pPictureProvider->ePictureProviderMode )
      {
         pPictureProvider->fCallback(
               pPictureProvider->pPrivateContext,
               pstDisplayInterruptInfo,
               &pMFDPicture );

         /* Needed to negate to get in ascending order, now restore actual value */
         pMFDPicture->ulChannelId = (pPictureProvider->uiVDCRectangleNumber * (-1)) - 1;

         /* Append the returned picture list to the previous MFD Pictures */
         if ( NULL == pMFDPictureList )
         {
            pMFDPictureList = pMFDPicture;
         }
         else
         {
            BAVC_MFD_Picture *pMFDPictureLast = pMFDPictureList;

            /* Add this picture to the end of the current pMFDPictureList */
            while ( pMFDPictureLast->pNext != NULL )
            {
               pMFDPictureLast = pMFDPictureLast->pNext;
            }

            pMFDPictureLast->pNext = pMFDPicture;
            pMFDPicture->pNext = NULL;
         }
      }

      pPictureProvider = BLST_S_NEXT( pPictureProvider, link );
   }

   if ( NULL == pMFDPictureList )
   {
      char cInterrtupPolarity;
      
      hXdmDih->stMFDPicture.eInterruptPolarity = pstDisplayInterruptInfo->eInterruptPolarity;
      hXdmDih->stMFDPicture.bIgnorePicture = true;
      hXdmDih->stMFDPicture.bStallStc = true;
      hXdmDih->stMFDPicture.bMute = true;
      hXdmDih->stMFDPicture.bChannelChange = true; /* for SW7425-2253: */
      hXdmDih->stMFDPicture.pNext = NULL;

      pMFDPictureList = &hXdmDih->stMFDPicture;

      switch( pstDisplayInterruptInfo->eInterruptPolarity )
      {
         case 0:  cInterrtupPolarity = 'T';  break;
         case 1:  cInterrtupPolarity = 'B';  break;
         case 2:  cInterrtupPolarity = 'F';  break;
         default: cInterrtupPolarity = 'x';  break;
      }

      BDBG_MODULE_MSG( BXDM_MFD1, ("%c :[x.%03x] int:%c %s %s %s",
                                 ( hXdmDih->stMFDPicture.bMute ) ? 'M' : ' ',
                                 hXdmDih->stMFDPicture.ulDecodePictureId,
                                 cInterrtupPolarity,
                                 ( hXdmDih->stMFDPicture.bIgnorePicture ) ? "ip" : " ",
                                 ( hXdmDih->stMFDPicture.bStallStc ) ? "ss" : " ",
                                 ( hXdmDih->stMFDPicture.bChannelChange) ? "chg" : " "
                                 ));
   }

   /* Execute the application's Picture Data Ready callback */
   if( NULL != hXdmDih->stPictureDataReadyHandler.fCallback )
   {
      hXdmDih->stPictureDataReadyHandler.fCallback(
               hXdmDih->stPictureDataReadyHandler.pPrivateContext,
               hXdmDih->stPictureDataReadyHandler.iPrivateParam,
               pMFDPictureList
               );
   }

   /* Execute the application's Display Interrupt Callback */
   if ( NULL != hXdmDih->stDisplayHandler.fCallback )
   {
      hXdmDih->stDisplayHandler.fCallback(
               hXdmDih->stDisplayHandler.pPrivateContext,
               hXdmDih->stDisplayHandler.iPrivateParam,
               pMFDPictureList
               );
   }

   BDBG_LEAVE( BXDM_DisplayInterruptHandler_Callback_isr );

   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BXDM_DisplayInterruptHandler_InstallCallback_PictureDataReadyInterrupt(
         BXDM_DisplayInterruptHandler_Handle hXdmDih,
         BXDM_DisplayInterruptHandler_PictureDataReady_isr fCallback,
         void *pPrivateContext,
         int32_t iPrivateParam
         )
{
   BDBG_ENTER( BXDM_DisplayInterruptHandler_InstallCallback_PictureDataReadyInterrupt );

   BDBG_ASSERT( hXdmDih );

   hXdmDih->stPictureDataReadyHandler.fCallback = fCallback;
   hXdmDih->stPictureDataReadyHandler.pPrivateContext = pPrivateContext;
   hXdmDih->stPictureDataReadyHandler.iPrivateParam = iPrivateParam;

   BDBG_MODULE_MSG( BXDM_DIHCTL, ("hDih:%08x Install PDR:%08x context:%08x param:%d",
                  hXdmDih,
                  hXdmDih->stPictureDataReadyHandler.fCallback,
                  hXdmDih->stPictureDataReadyHandler.pPrivateContext,
                  hXdmDih->stPictureDataReadyHandler.iPrivateParam
                  ));

   BDBG_LEAVE( BXDM_DisplayInterruptHandler_InstallCallback_PictureDataReadyInterrupt );

   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BXDM_DisplayInterruptHandler_UnInstallCallback_PictureDataReadyInterrupt(
         BXDM_DisplayInterruptHandler_Handle hXdmDih
         )
{
   BDBG_ENTER( BXDM_DisplayInterruptHandler_UnInstallCallback_PictureDataReadyInterrupt );

   BDBG_ASSERT( hXdmDih );

   BDBG_MODULE_MSG( BXDM_DIHCTL, ("hDih:%08x UnInstall PDR:%08x context:%08x param:%d",
                  hXdmDih,
                  hXdmDih->stPictureDataReadyHandler.fCallback,
                  hXdmDih->stPictureDataReadyHandler.pPrivateContext,
                  hXdmDih->stPictureDataReadyHandler.iPrivateParam
                  ));

   hXdmDih->stPictureDataReadyHandler.fCallback = NULL;
   hXdmDih->stPictureDataReadyHandler.pPrivateContext = NULL;
   hXdmDih->stPictureDataReadyHandler.iPrivateParam = 0;

   BDBG_LEAVE( BXDM_DisplayInterruptHandler_UnInstallCallback_PictureDataReadyInterrupt );

   return BERR_TRACE( BERR_SUCCESS );
}


BERR_Code
BXDM_DisplayInterruptHandler_InstallCallback_DisplayInterrupt(
         BXDM_DisplayInterruptHandler_Handle hXdmDih,
         BXDM_DisplayInterruptHandler_Display_isr fCallback,
         void *pPrivateContext,
         int32_t iPrivateParam
         )
{
   BDBG_ENTER( BXDM_DisplayInterruptHandler_InstallCallback_DisplayInterrupt );

   BDBG_ASSERT( hXdmDih );

   hXdmDih->stDisplayHandler.fCallback = fCallback;
   hXdmDih->stDisplayHandler.pPrivateContext = pPrivateContext;
   hXdmDih->stDisplayHandler.iPrivateParam = iPrivateParam;

   BDBG_MODULE_MSG( BXDM_DIHCTL, ("hDih:%08x Install DisplayInterupt:%08x context:%08x param:%d",
                  hXdmDih,
                  hXdmDih->stDisplayHandler.fCallback,
                  hXdmDih->stDisplayHandler.pPrivateContext,
                  hXdmDih->stDisplayHandler.iPrivateParam
                  ));

   BDBG_LEAVE( BXDM_DisplayInterruptHandler_InstallCallback_DisplayInterrupt );

   return BERR_TRACE( BERR_SUCCESS );
}

BERR_Code
BXDM_DisplayInterruptHandler_UnInstallCallback_DisplayInterrupt(
         BXDM_DisplayInterruptHandler_Handle hXdmDih
         )
{
   BDBG_ENTER( BXDM_DisplayInterruptHandler_UnInstallCallback_DisplayInterrupt );

   BDBG_ASSERT( hXdmDih );

   BDBG_MODULE_MSG( BXDM_DIHCTL, ("hDih:%08x UnInstall DisplayInterupt:%08x context:%08x param:%d",
                  hXdmDih,
                  hXdmDih->stDisplayHandler.fCallback,
                  hXdmDih->stDisplayHandler.pPrivateContext,
                  hXdmDih->stDisplayHandler.iPrivateParam
                  ));

   hXdmDih->stDisplayHandler.fCallback = NULL;
   hXdmDih->stDisplayHandler.pPrivateContext = NULL;
   hXdmDih->stDisplayHandler.iPrivateParam = 0;

   BDBG_LEAVE( BXDM_DisplayInterruptHandler_UnInstallCallback_DisplayInterrupt );

   return BERR_TRACE( BERR_SUCCESS );
}

/* SW7405-3984: the Set/Get mode API's provide a way to disable
 * a PictureProvide (XDM) without unregistering it from the DIH.
 * When a PictureProvider is "disabled", the associated callback
 * will not be executed by in BXDM_DisplayInterruptHandler_Callback_isr.
 */
BERR_Code 
BXDM_DisplayInterruptHandler_SetPictureProviderMode_isr( 
   BXDM_DisplayInterruptHandler_Handle hXdmDih,
   BXDM_DisplayInterruptHandler_PictureProvider_isr fCallback,
   void *pPrivateContext,
   BXDM_DisplayInterruptHandler_PictureProviderMode eMode
   )
{
   BXDM_DisplayInterruptHandler_P_PictureProviderCallback *pPictureProvider;

   BERR_Code rc=BERR_SUCCESS;

   BDBG_ENTER( BXDM_DisplayInterruptHandler_SetPictureProviderMode_isr );

   BDBG_ASSERT( hXdmDih );

   /* Find the picture provider */
   pPictureProvider = BXDM_DisplayInterruptHandler_S_FindPictureProvider(
            hXdmDih,
            fCallback,
            pPrivateContext);

   if ( NULL != pPictureProvider )
   {
      /* Set the mode. */
      pPictureProvider->ePictureProviderMode = eMode;

      BDBG_MODULE_MSG( BXDM_DIHCTL, ("SetPictureProviderMode:: hDih:%08x pp:%08x hXdm:%08x %s",
                  hXdmDih,
                  pPictureProvider,
                  pPrivateContext,
                  ( pPictureProvider->ePictureProviderMode ) ? "enabled" : "disabled"
                  ));
   }
   else
   {
      BDBG_ERR(("BXDM_DisplayInterruptHandler_SetPictureProviderMode_isr:: Specified Picture Provider could not be found"));
      rc = BERR_INVALID_PARAMETER;
   }

   BDBG_LEAVE( BXDM_DisplayInterruptHandler_SetPictureProviderMode_isr );

   return BERR_TRACE( rc );
}

BERR_Code 
BXDM_DisplayInterruptHandler_GetPictureProviderMode_isr( 
   BXDM_DisplayInterruptHandler_Handle hXdmDih,
   BXDM_DisplayInterruptHandler_PictureProvider_isr fCallback,
   void *pPrivateContext,
   BXDM_DisplayInterruptHandler_PictureProviderMode * peMode
   )
{
   BXDM_DisplayInterruptHandler_P_PictureProviderCallback *pPictureProvider;

   BERR_Code rc=BERR_SUCCESS;

   BDBG_ENTER( BXDM_DisplayInterruptHandler_GetPictureProviderMode_isr );

   BDBG_ASSERT( hXdmDih );
   
   /* Find the picture provider */
   pPictureProvider = BXDM_DisplayInterruptHandler_S_FindPictureProvider(
            hXdmDih,
            fCallback,
            pPrivateContext);

   if ( NULL != pPictureProvider )
   {
      /* Return the current mode. */
      *peMode = pPictureProvider->ePictureProviderMode;

      BDBG_MODULE_MSG( BXDM_DIHCTL, ("GetPictureProviderMode:: hDih:%08x pp:%08x hXdm:%08x mode:%d",
                  hXdmDih,
                  pPictureProvider,
                  pPrivateContext,
                  pPictureProvider->ePictureProviderMode
                  ));
   }
   else
   {
      BDBG_ERR(("BXDM_DisplayInterruptHandler_GetPictureProviderMode_isr:: Specified Picture Provider could not be found"));
      rc = BERR_INVALID_PARAMETER;
   }

   BDBG_LEAVE( BXDM_DisplayInterruptHandler_GetPictureProviderMode_isr );

   return BERR_TRACE( rc );
}


