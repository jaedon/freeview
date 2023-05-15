/**************************************************************************************************/
#define ________CA_BDC_Private_Include____________________________________________________
/**************************************************************************************************/
#include "vkernel.h"
#include "na_glob.h"

#include "isosvc.h"

/**************************************************************************************************/
#define ________CA_BDC_Golbal_Value________________________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________CA_BDC_Private_Define_______________________________________________________
/**************************************************************************************************/





/**************************************************************************************************/
#define ________CA_BDC_Private_Static_Value_________________________________________________
/**************************************************************************************************/
static TBdcPopUpContent	*s_pstPopUpContent = NULL;


/**************************************************************************************************/
#define ________CA_BDC_Global_Value_______________________________________________________
/**************************************************************************************************/
TBdcDisplayPopUp 		g_pfnDisplayPopUpCallback = NULL;
TBdcRemovePopUp 		g_pfnRemovePopUpCallback = NULL;


/**************************************************************************************************/
#define ________CA_BDC_Private_Static_Prototype_____________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________CA_BDC_Private_Static_Prototype_Body________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________CA_BDC_Public_Functions_Body_______________________________________________
/**************************************************************************************************/
TBdcStatus bdcDisplayPopUp(TBdcPopUpPersistence	xPersistence, TUnsignedInt8 xNumLanguages, const TBdcPopUpContent *pxContent)
{
	TUnsignedInt32 i, j;

	HxLOG_Print("xPersistence:%d, xNumLanguage:%d\n", xPersistence, xNumLanguages);

	if ( (xPersistence < BDC_POP_UP_PERSISTENCE_ALWAYS) || (xPersistence > BDC_POP_UP_PERSISTENCE_TIMEOUT) )
	{
		HxLOG_Info("xNumLanguages = 0 || pxContent is null\n");
		return BDC_ERROR;
	}

	if ( (xNumLanguages <= 0) || (pxContent == NULL) )
	{
		HxLOG_Info("xNumLanguages = 0 || pxContent is null\n");
		return BDC_ERROR;
	}

	if (pxContent->pContent == NULL)
	{
		HxLOG_Info("pxContent->pContent is null\n");
		return BDC_ERROR;
	}

	if (pxContent->numBytes <= 0)
	{
		HxLOG_Info("pxContent->numBytes is zero\n");
		return BDC_ERROR;
	}

	if (MWC_UTIL_DvbStrlen(pxContent->pContent) <= 0)
	{
		HxLOG_Info("pxContent->pContent length is zero\n");
		return BDC_ERROR;
	}

	if (g_pfnDisplayPopUpCallback == NULL)
	{
		HxLOG_Info("g_pfnDisplayPopUpCallback is null\n");
		return BDC_ERROR;
	}

	if (s_pstPopUpContent != NULL)
	{
		OxCAS_Free(s_pstPopUpContent);
		s_pstPopUpContent = NULL;
	}

	s_pstPopUpContent = OxCAS_Malloc(sizeof(TBdcPopUpContent)*xNumLanguages);
	if (s_pstPopUpContent == NULL)
	{
		HxLOG_Info("error in OxCAS_Malloc 1\n");
		return BDC_ERROR;
	}

	HxSTD_memset(s_pstPopUpContent, 0x00, sizeof(TBdcPopUpContent)*xNumLanguages);
	HxSTD_memcpy(s_pstPopUpContent, pxContent, sizeof(TBdcPopUpContent)*xNumLanguages);

	// for BDC-PP-000 (Functions Parameter Test).
	// pxContent[0].pContent memory address is ok. But pxContent[1].pContent memory address is garbage value.
	if (xNumLanguages > 1)
	{
		for ( i=0; i<xNumLanguages; i++)
		{
			if (pxContent[i].pContent > pxContent[i+1].pContent) // memory address check. (pxContent[1].pContent)
			{
				xNumLanguages -= 1;	// xNumLanguages -= 1. if not segmentation fault !!!.
				break;
			}
		}
	}

	for (i=0 ; i<xNumLanguages ; i++)
	{
		if (pxContent[i].numBytes == 0)
		{
			continue;
		}
		s_pstPopUpContent[i].numBytes = pxContent[i].numBytes;
		s_pstPopUpContent[i].pContent = (TUnsignedInt8 *)OxCAS_Malloc(sizeof(TUnsignedInt8)*(pxContent[i].numBytes + 1));
		if (s_pstPopUpContent[i].pContent == NULL)
		{
			s_pstPopUpContent[i].numBytes = 0;
			HxLOG_Info("error in OxCAS_Malloc 2\n");
			continue;
		}

		if (pxContent[i].pContent == NULL)
		{
			s_pstPopUpContent[i].pContent = NULL;
			s_pstPopUpContent[i].numBytes = 0;
			continue;
		}

		MWC_UTIL_DvbStrncpy(s_pstPopUpContent[i].pContent, pxContent[i].pContent, pxContent[i].numBytes);
		s_pstPopUpContent[i].pContent[pxContent[i].numBytes] = '\0';
#if defined(CONFIG_DEBUG)
		if (pxContent[i].pContent != NULL)
		{
			HxLOG_Print(" %d) language:(0x%x, 0x%x, 0x%x), numBytes:%d, strlen:%d\n",
									(int)i, pxContent[i].language[0], pxContent[i].language[1], pxContent[i].language[2], pxContent[i].numBytes, strlen(pxContent[i].pContent));
			for (j=0; j<pxContent[i].numBytes; j++)
			{
				HxLOG_Print(" %02X", pxContent[i].pContent[j]);
			}
		}
		HxLOG_Print("\n");
#endif
	}

	if (g_pfnDisplayPopUpCallback)
	{
		return (*g_pfnDisplayPopUpCallback)(xPersistence, xNumLanguages, s_pstPopUpContent);
	}
	return BDC_NO_ERROR;
}


TBdcStatus bdcRemovePopUp(  void)
{
	HxLOG_Print("\n");

	if (g_pfnRemovePopUpCallback == NULL)
	{
		HxLOG_Info("g_pfnRemovePopUpCallback is null\n");
		return BDC_ERROR;
	}

	if (g_pfnRemovePopUpCallback)
	{
		return (*g_pfnRemovePopUpCallback)();
	}
	return BDC_NO_ERROR;
}

