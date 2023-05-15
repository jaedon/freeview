#ifndef	__XMGR_CAS_VMX_INT_H__
#define	__XMGR_CAS_VMX_INT_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <bus.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/



/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef enum
{
	eVmxCasUiType_FingerPrinting,
	eVmxCasUiType_Popup,
	eVmxCasUiType_SystemMessage,
	eVmxCasUiType_Pincode,
	eVmxCasUiType_Max
} XmgrCas_VmxUiType_e;

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
BUS_Result_t	xproc_cas_VmxMain (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xproc_cas_VmxUiFingerPrinting (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xproc_cas_VmxUiPopup (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xproc_cas_VmxUiErrorMsg(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t	xproc_cas_VmxUiPinDialog (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
HERROR xmgr_cas_Vmx_SetOverrideRatingLevel(Handle_t hAction, HUINT32 nActionId);
#if defined (CONFIG_DEBUG)
HUINT8 *xmgr_cas_VmxGetMessageName(HINT32 message);
#endif


HERROR xmgr_cas_Vmx_SetRatingLevel(void);
HERROR xmgr_cas_Vmx_GetRatingLevel(HUINT8 *pucRating);
HERROR xmgr_cas_Vmx_SetOverrideRatingLevel(Handle_t hAction, HUINT32 nActionId);
HBOOL xmgr_cas_Vmx_IsWaitParentalRatingState(MgrPg_Info_t *pstPgInfo);
HERROR xmgr_cas_Vmx_ConvertRatingBaseToVmx(HUINT8 ucBaseRating, HUINT8 *pucVmxRating);
HERROR xmgr_cas_Vmx_ConvertRatingVmxToBase(HUINT8 ucVmxRating, HUINT8 *pucBaseRating);
HERROR xmgr_cas_Vmx_GetVmxIptvRatingLevel(HUINT32 p2, HUINT8 *pRating);

#endif /* !__XMGR_CAS_VMX_INT_H__ */

