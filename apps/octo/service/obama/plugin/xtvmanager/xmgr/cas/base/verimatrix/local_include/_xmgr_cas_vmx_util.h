#ifndef	__xmgr_cas_VmxUTIL_H__
#define	__xmgr_cas_VmxUTIL_H__


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

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
HCHAR*		xmgr_cas_VmxUtil_EncodeString(HCHAR *string);
HERROR 		xmgr_cas_VmxUtil_PostMessageSessionEventToOM(DxCAS_MmiSessionType_e eSessionType, BUS_Callback_t ulUiHandle, Handle_t hSession, HUINT32 ulSlotNumber, HINT32 eEvent);
void 		xmgr_cas_VmxUtil_ConvertLanguageCode(HUINT8 *pszMessage);
HCHAR* 		xmgr_cas_VmxUtil_GetIptvHybridVersionString(void);
HCHAR* 		xmgr_cas_VmxUtil_GetWebVersionString(void);
HCHAR* 		xmgr_cas_VmxUtil_GetGatewayVersionString(void);


#endif /* !__xmgr_cas_VmxUTIL_H__ */

/* end of file */
