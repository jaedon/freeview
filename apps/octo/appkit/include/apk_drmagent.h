#ifndef __APK_DRMAGENT_H__
#define __APK_DRMAGENT_H__


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <oapi_drmagent.h>


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/



/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef void (*APK_DRMAGENT_MessageResultNotifier_t) (const HCHAR *pzResultMsg, HUINT16 usMsgId, HUINT32 ulResultCode);
typedef void (*APK_DRMAGENT_StatusChangeNotifier_t) (const HCHAR *pzDrmSystemId);


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/

void	APK_DRMAGENT_Init(void);
void	APK_DRMAGENT_SendDrmMessage(const HCHAR *pzMsgType, const HCHAR *pzMsg, const HCHAR *pzDrmSystemId, HUINT16 usMsgId);
void	APK_DRMAGENT_SendDrmSystemStatusRequest(const HCHAR *pzDrmSystemId);



HERROR	APK_DRMAGENT_RegisterDrmMessageNotifier(APK_DRMAGENT_MessageResultNotifier_t fnNotifier);
HERROR	APK_DRMAGENT_RegisterDrmStatusNotifier(APK_DRMAGENT_StatusChangeNotifier_t fnNotifier);


#endif // __APK_DRMAGENT_H__
