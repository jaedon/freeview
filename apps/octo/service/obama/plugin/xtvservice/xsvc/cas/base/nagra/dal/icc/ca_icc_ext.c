/**************************************************************************************************/
#define ________CA_ICC_EXT_Private_Include_________________________________________________
/**************************************************************************************************/
#include "vkernel.h"
#include "na_glob.h"


/**************************************************************************************************/
#define ________CA_ICC_EXT_Golbal_Value___________________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________CA_ICC_EXT_Private_Define__________________________________________________
/**************************************************************************************************/





/**************************************************************************************************/
#define ________CA_ICC_EXT_Private_Type_Define____________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________CA_ICC_EXT_Private_Static_Value____________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________CA_ICC_EXT_Global_Value___________________________________________________
/**************************************************************************************************/
extern TUnsignedInt32			g_ulNumOfSlot;
extern ScMgrIntance_t			*g_pstScMgrInstance;
extern TIccEventNotification	g_pfnNaIccNotiCallback;


/**************************************************************************************************/
#define ________CA_ICC_EXT_Private_Static_Prototype_________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________CA_ICC_EXT_Private_Static_Prototype_Body____________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________CA_ICC_EXT_Public_Functions_Body___________________________________________
/**************************************************************************************************/
HERROR CA_NA_IccGetCardStatus(ScStatusItem_t *pstScStatus)
{
	TUnsignedInt32 i;

	if (pstScStatus == NULL)
	{
		HxLOG_Info("target is null...\n");
		return ERR_FAIL;
	}

	for (i=0; i<g_ulNumOfSlot; i++)
	{
		pstScStatus[i].ucSlotId = (TUnsignedInt8)g_pstScMgrInstance[i].ulSessionId;
		pstScStatus[i].eCardStatus = eSc_Out;

		if ( (g_pstScMgrInstance[i].eInstState != eSc_Extracted) && (g_pstScMgrInstance[i].eCardStatus == eSc_In) )
			pstScStatus[i].eCardStatus = eSc_In;

		if ( (g_pstScMgrInstance[i].eInstState != eSc_Extracted) && (g_pstScMgrInstance[i].eCardStatus == eSc_Mute) )
			pstScStatus[i].eCardStatus = eSc_Mute;

		if ( (g_pstScMgrInstance[i].eInstState != eSc_Extracted) && (g_pstScMgrInstance[i].eCardStatus == eSc_Invalid) )
			pstScStatus[i].eCardStatus = eSc_Invalid;
	}
	return ERR_OK;
}


HERROR CA_NA_IccGetNumberOfSlots(TUnsignedInt8 *pucSlots)
{
	if (pucSlots == NULL)
	{
		HxLOG_Info("target is null...\n");
		return ERR_FAIL;
	}

	*pucSlots = (TUnsignedInt8)g_ulNumOfSlot;
	return ERR_OK;
}


HERROR CA_NA_IccGetRegistrationStatus(void)
{
	return ((g_pfnNaIccNotiCallback != NULL) ? ERR_OK : ERR_FAIL);
}


