#ifndef __APK_DB_H__
#define __APK_DB_H__

HERROR	APK_DB_Init(void);
HERROR	APK_DB_Remove(const HCHAR *pszKey);
HERROR	APK_DB_SetInt(const HCHAR *pszKey, HINT32 nValue);
HERROR	APK_DB_GetInt(const HCHAR *pszKey, HINT32 *pnValue);
HERROR	APK_DB_SetStr(const HCHAR *pszKey, const HCHAR *pszString);
HERROR	APK_DB_GetStr(const HCHAR *pszKey, HCHAR *pszString, HINT32 nMax);

#endif // __APK_DB_H__


