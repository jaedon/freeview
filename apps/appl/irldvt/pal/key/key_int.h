
#define MAX_RCU_KEY_NUM				0xFF

/* sec단위. Standby key 입력후 이 시간 내로 timer 재 설정을 하지 않으면 micom이 main board를 AC off 한다. */
#define MICOM_POWER_KEY_WATCHDOG	2		/* sec */



extern KEY_NotifyCallback_t	g_pfnKeyCallback;
extern HBOOL				g_bDisableKey;

extern HUINT32 	INT_FrontKey_GetFrontKeyCode(HUINT32 ulDiKeyCode);
extern HBOOL 	INT_FrontKey_IsReleaseKeyAllowed(HUINT32 ucMwCode);
extern HBOOL 	INT_FrontKey_IsRepeatKeyAllowed(HUINT32 ucMwCode);
extern void		INT_PalKey_SetWakeUpKeyToDi(void);
extern HINT32 	INT_PalKey_DICallback(DI_KEY_DEVICE eDevice, HUINT32 ulDiKeyCode, DI_KEY_TYPE eKeyType);
extern void		INT_PalKey_MultiCustomCode_Callback(HUINT8 msgType, HUINT8 mode);
#if defined(CONFIG_MW_SYS_HOTKEY)
extern HUINT32	INT_FrontKey_GetFrontMultiKeyCode(HUINT32 ulDiKeyCode, HUINT32 aulKey[], HUINT32 ulSize);
#endif

