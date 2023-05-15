/********************************************************************
 * $Workfile:   ddi_micom.h  $
 * $Modtime:   Jan 12 2007 09:53:44  $
 *
 * Author     :
 * Description:
 *
 *                                 Copyright (c) 2007 HUMAX Co., Ltd.
 *                                               All rights reserved.
 ********************************************************************/
#ifndef	_DRV_MICOM_H_
#define	_DRV_MICOM_H_

#include "htype.h"
#include "drv_err.h"

/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/

typedef enum
{
	DRV_MICOM_SET_WAKEUPKEY_TYPE_INDIVISUAL			=0x00,
	DRV_MICOM_SET_WAKEUPKEY_TYPE_GROUP				=0x01
} DRV_MICOM_SET_WAKEUPKEY_TYPE ;

#if defined(CONFIG_MICOM_SYSTEM_PROTECTION)
typedef enum
{
	FAN_LOCKED_DETECTION,
	OVER_TEMPERATURE_DETECTION
}MICOM_SYSTEM_PROTECTION_e;
#endif

/* EU_MICOM : Wake up sources from source file - process.c line 3~8 */
typedef enum {
	DRV_MICOM_WAKEUP_BY_UNKNOWN = 0,
	DRV_MICOM_WAKEUP_BY_KEY=1,
	DRV_MICOM_WAKEUP_BY_RCU,
	DRV_MICOM_WAKEUP_BY_TIMER,
	DRV_MICOM_WAKEUP_BY_ACPOWER,
	DRV_MICOM_WAKEUP_BY_REBOOT,
	DRV_MICOM_WAKEUP_BY_RF4CE_RCU,
	DRV_MICOM_WAKEUP_BY_FAN_LOCKED,
	DRV_MICOM_WAKEUP_BY_OVER_TEMPERATURE,
	DRV_MICOM_WAKEUP_BY_WOL,
	DRV_MICOM_WAKEUP_BY_MAX=0xFF
} DRV_MICOM_WAKEUP_SRC;

#define DRV_MICOM_DEVICE CONFIG_MICOM_DEVICE_NAME

#define DRV_MICOM_BLUE_LED_ON 		0x01
#define DRV_MICOM_RED_LED_ON 		0x00
#define DRV_MICOM_LED_OFF 		0x02

#define MICOM_FRAME_CMD_GET_VER					0x01
#define MICOM_FRAME_CMD_SET_CUSTOMCODE			0x02
#define MICOM_FRAME_CMD_SET_TIME				0x03
#define MICOM_FRAME_CMD_GET_TIME				0x04
#define MICOM_FRAME_CMD_SET_WTIME				0x05
#define MICOM_FRAME_CMD_SET_STANDBY				0x06
#define MICOM_FRAME_CMD_SET_WAKEUP				0x07
#define MICOM_FRAME_CMD_GET_WAKEUPSRC			0x08
#define MICOM_FRAME_CMD_GET_POWERSTATE			0x09
#define MICOM_FRAME_CMD_POWER_REBOOT			0x0A
#define MICOM_FRAME_CMD_WATCHDOG				0x0B
#define MICOM_FRAME_CMD_SET_MICOMDBG 			0x0C
#define MICOM_FRAME_CMD_GET_WAKEUPTIME 			0x0D
#define MICOM_FRAME_CMD_SET_LED 				0x0E
#define MICOM_FRAME_CMD_DISPLAY_7SEG			0x0F
#define MICOM_FRAME_CMD_SET_ICON 				0x10
#define MICOM_FRAME_CMD_WRITE_STR 				0x12
#define MICOM_FRAME_CMD_SET_DISPLAY_TIME 		0x13
#define MICOM_FRAME_CMD_SET_PWR_SAV				0x14
#define MICOM_FRAME_CMD_SET_DIS_ANI				0x15
#define MICOM_FRAME_CMD_SET_LCD_PWR				0x16
#define MICOM_FRAME_CMD_SET_WAKEUP_KEY			0x17
#define MICOM_FRAME_CMD_GET_WAKEUP_KEY			0x18
#define MICOM_FRAME_CMD_SET_LCD_DIMMING_LEVEL	0x19
#define MICOM_FRAME_CMD_SET_LED_DIMMING_LEVEL	0x1A
#define MICOM_FRAME_CMD_GET_STRVER				0x1B
#define MICOM_FRAME_CMD_SET_WOL					0x1C
#define MICOM_FRAME_CMD_SEND_PROTECT_SYSTEM		0x1D
#define MICOM_FRAME_CMD_SET_FAST_BOOT			0x1E

#define MICOM_FRAME_CMD_SET_UNIT_TEST			0x7F
#define MICOM_FRAME_CMD_GET_READY_FOR_DOWNLOAD	0x80
#define MICOM_FRAME_CMD_SENDKEYINFO				0x81
#define MICOM_FRAME_CMD_SENDRCUINFO				0x82
#define MICOM_FRAME_CMD_SET_RCUINFO				0x83
#define MICOM_FRAME_CMD_SET_MULTI_KEYINFO		0x84
#define MICOM_FRAME_CMD_GET_TOUCH_KEYINFO		0x85
#define MICOM_FRAME_CMD_SET_TOUCH_SENSITIVITY	0x86
#define MICOM_FRAME_CMD_SET_RESCUE_RESET		0x87
#define MICOM_FRAME_CMD_GET_TOUCH_KEYSIGNAL		0x88
#define MICOM_FRAME_CMD_GET_TOUCH_KEYREFERENCE	0x89
#define MICOM_FRAME_CMD_GET_TOUCH_SENSITIVITY	0x8A

#define MICOM_FRAME_CMD_SET_RF4CE_PAIRING		0x90
#define MICOM_FRAME_CMD_GET_RF4CE_PAIRED_DEVS	0x91
#define MICOM_FRAME_CMD_GET_RF4CE_PAIRED_DEV_IEEE_ADDR	0x92
#define MICOM_FRAME_CMD_GET_RF4CE_NWK_INFO		0x93
#define MICOM_FRAME_CMD_SET_RF4CE_IEEE_ADDR		0x94
#define MICOM_FRAME_CMD_GET_RF4CE_IEEE_ADDR		0x95
#define MICOM_FRAME_CMD_SEND_RF4CE_BTN_INFO		0x96
#define MICOM_FRAME_CMD_SET_RF4CE_FACTORY_MODE	0x97
#define MICOM_FRAME_CMD_SEND_RF4CE_RSSI_VALUE	0x98
#define MICOM_FRAME_CMD_SEND_RF4CE_PAIRING_RESULT	0x99

#define MICOM_FRAME_CMD_SEND_RF4CE_ZRC_USER_DATA	0x9A
#define MICOM_FRAME_CMD_TRANS_RF4CE_ZRC_USER_DATA	0x9B

#define MICOM_FRAME_CMD_SET_RF4CE_CERTI_INOUT_MODE	0x9E /* External Certification only : CW(무변조 출력), CM (변조 출력) 및 RX mode로 변경 */
#define MICOM_FRAME_CMD_SET_RF4CE_CERTI_CHANGE_FREQ	0x9F /* External Certification only : 2.405GHz(Channel 11, 0x0B) ~ 2.480GHz(Channel 26, 0x1A) */

#define MICOM_FRAME_CMD_SET_LOADER_READY		0xA0
#define MICOM_FRAME_CMD_SET_SYSTEM_MONITOR		0xA1
#define MICOM_FRAME_CMD_SET_APP_READY			0xA2
#define MICOM_FRAME_CMD_SET_BLINKING_TIME		0xA3

#define MICOM_FRAME_CMD_SET_RF4CE_CERTI_CHANGE_POWER		0xB0 /* Add IQView for RF4CE TEST */
#define MICOM_FRAME_CMD_SET_RF4CE_CERTI_IQVIEW_RX			0xB1 /* Add IQView for RF4CE TEST */
#define MICOM_FRAME_CMD_SET_RF4CE_CERTI_IQVIEW_TX			0xB2 /* Add IQView for RF4CE TEST */
#define MICOM_FRAME_CMD_SET_RF4CE_CERTI_IQVIEW_RX_RESULT	0xB3 /* Add IQView for RF4CE TEST */

#define MICOM_FRAME_CMD_SET_WTIME_ON 			0x01
#define MICOM_FRAME_CMD_SET_WTIME_OFF 			0x00

#define MICOM_FRAME_CMD_SET_STANDBY_ON			0x00
#define MICOM_FRAME_CMD_SET_STANDBY_OFF			0x01

#define MICOM_FRAME_CMD_WATCHDOG_ON				0x01
#define MICOM_FRAME_CMD_WATCHDOG_OFF 			0x00
#define MICOM_FRAME_CMD_WATCHDOG_TIME_RATE 		0x03

#define DRV_MICOM_MAX_NUMBER			(1)
#define DRV_MICOM_MAX_LENGTH			(128)

#define DRV_MICOM_MESSAGE_BYTES		(DRV_MICOM_MAX_LENGTH+DRV_MICOM_MAX_LENGTH-1) /* Max Length x 2 -1 */

#define MICOM_FRAME_HEADER 			0x05
#define MICOM_FRAME_REC_HEADER		0x06

#define MICOM_FRAME_LENGTH_ONE		0x01
#define MICOM_FRAME_LENGTH_TWO		0x02
#define MICOM_FRAME_LENGTH_THREE	0x03
#define MICOM_FRAME_LENGTH_FOUR		0x04
#define MICOM_FRAME_LENGTH_FIVE		0x05
#define MICOM_FRAME_LENGTH_SIX		0x06
#define MICOM_FRAME_LENGTH_SEVEN	0x07
#define MICOM_FRAME_LENGTH_EIGHT	0x08
#define MICOM_FRAME_LENGTH_NINE		0x09
#define MICOM_FRAME_LENGTH_TEN		0x0A
#define MICOM_FRAME_LENGTH_ELEVEN	0x0B
#define MICOM_FRAME_LENGTH_TWELVE	0x0C

#define CHECK_NUM_ONE			(1)
#define CHECK_NUM_TWO			(2)
#define CHECK_NUM_THREE			(3)
#define CHECK_NUM_FOUR			(4)
#define CHECK_NUM_FIVE			(5)
#define CHECK_NUM_SIX			(6)
#define CHECK_NUM_SEVEN 		(7)
#define CHECK_NUM_EIGHT 		(8)
#define CHECK_NUM_NINE			(9)
#define CHECK_NUM_TEN	 		(10)
#define CHECK_NUM_ELEVEN 		(11)

#define MICOM_HEADER			(0)
#define MICOM_LENGTH			(1)
#define MICOM_COMMAND			(2)
#define MICOM_DATA_ONE			(3)
#define MICOM_DATA_TWO			(4)
#define MICOM_DATA_THREE		(5)
#define MICOM_DATA_FOUR			(6)
#define MICOM_DATA_FIVE			(7)
#define MICOM_DATA_SIX			(8)
#define MICOM_DATA_SEVEN		(9)
#define MICOM_DATA_EIGHT		(10)

#define MICOM_TIMEOUT			(2000)
#define MICOM_MESSAGE_BYTES		(16)
#define MICOM_MESSAGE_QUEUE_SIZE	(5)

#define	UNIXTIME_BYTE_MASK 		0xFF

#define MICOM_MONITOR_SEND_TIME 1 /*1 sec*/

// The below value must be same VALID_UNIXTIME in vk_clock.c.
#define VALID_UNIXTIME			(60 * 60 * 24 * 366)			// until 1971/01/02

#define MICOM_FAIL_RETRY_CNT	5
/********************************************************************
 Structure and Type Definition
 ********************************************************************/
typedef void (*pfnDRV_UCOM_RCUKey_Notify)(HUINT8 ucRcuType, HINT32 repeat, HUINT8 *pucKeyCode, HUINT8 ucLength);
typedef void (*pfnDRV_UCOM_RF4CEKey_Notify)(HINT32 repeat, HUINT8 ucKeyCode);
typedef void (*pfnDRV_UCOM_RF4CE_RSSI_Notify)(HUINT32 ulTxRSSI, HUINT32 ulRxRSSI);

#if defined(CONFIG_RF4CE_MICOM)
typedef void (*pfnDRV_UCOM_RF4CE_PAIRING_RESULT_Notify)(HUINT8 ucPairingResult);

#if defined(CONFIG_RF4CE_MOTION_TOUCH_UEI_HILCREST)
typedef void (*pfnDRV_UCOM_RF4CE_USER_DATA_INCOMING_Notify)(unsigned char ucPairingIndex, unsigned char *pucData, unsigned char ucSize);
#endif
#endif

#if defined(CONFIG_MICOM_SYSTEM_PROTECTION)
typedef void (*pfnDRV_UCOM_SYSTEM_PROTECTION_Notify)(MICOM_SYSTEM_PROTECTION_e eProtectionReason);
#endif

//must move clock module
typedef	struct {
	unsigned short	usYear;
	unsigned char		ucMonth;
	unsigned char		ucDay;
} DRV_CLOCK_DATE_T ;

typedef struct  {
	unsigned char		ucHour;
	unsigned char		ucMinute;
	unsigned char		ucSecond;
	unsigned short	usMillisecond;
} DRV_CLOCK_TIME_T ;

typedef	struct {
	DRV_CLOCK_DATE_T	stDate;
	DRV_CLOCK_TIME_T	stTime;
} DRV_CLOCK_DATE_TIME_T;

typedef struct {
	unsigned short usMjd;
	unsigned char ucHour;
	unsigned char ucMin;
	unsigned char ucSec;
}DRV_DATE_TIME;

typedef struct tagTimeType {
	unsigned char	ucHour;
	unsigned char	ucMinute;
	unsigned char	ucSecond;
	unsigned char	usMillisecond;
} TIME_T, SYS_TIME_T ;

#if defined(CONFIG_RF4CE_MICOM)
typedef enum
{
	OUTPUT_MODE_WITH_CW=0, /* Un-Modulated Output Mode */
	OUTPUT_MODE_WITH_CM,     /* Modulated Output Mode */
	INPUT_MODE			     /* Input Mode */
}MICOM_RF4CE_INOUT_MODE;
#endif

#if defined (CONFIG_MICOM) &&(CONFIG_MICOM_UPGRADE)
extern int upgradeStatus;
#endif

/********************************************************************
 DRV_MICOM Function Declaration
 ********************************************************************/
DRV_Error DRV_MICOM_Init( void );
DRV_Error DRV_MICOM_GetVersion( unsigned int *pulVersion );
DRV_Error DRV_MICOM_GetStrVersion( unsigned char *pucStrVersion );
DRV_Error DRV_MICOM_GetBootTime(HUINT16 *pusMJD, HUINT32 *pulBootTime);
DRV_Error DRV_MICOM_GetBootType(HUINT32 *pulBootType);
DRV_Error DRV_MICOM_FrontLedControl( unsigned char blue_red);
DRV_Error DRV_MICOM_SetLocalTime(unsigned int unixTime);
DRV_Error DRV_MICOM_GetLocalTime(unsigned int *punixTime);
DRV_Error DRV_MICOM_SetWakeupTime(unsigned int unixTime);
DRV_Error DRV_MICOM_RequestWakeupTime(unsigned int *punixTime, unsigned char *OnOff);
DRV_Error DRV_MICOM_DelWakeupTime( void );
DRV_Error DRV_MICOM_SetStandby( void );
DRV_Error DRV_MICOM_STBPowerOn( void );
DRV_Error DRV_MICOM_STBPowerReboot( void );
DRV_Error DRV_MICOM_RequestPowerState(unsigned char *powerState);
DRV_Error DRV_MICOM_SetWatchdog(HBOOL bOn_nOff, unsigned char ucTimeOut);
DRV_Error DRV_MICOM_DisplayTime(unsigned char on_off);
DRV_Error DRV_MICOM_GetWakeupSrc(DRV_MICOM_WAKEUP_SRC *peWakeupSrc);
DRV_Error DRV_MICOM_SetWakeOnLAN(HBOOL bOn_nOff);
DRV_Error DRV_MICOM_SetFastBoot(HBOOL bOn_nOff);
DRV_Error DRV_MICOM_SET_Icon(unsigned char position, unsigned char value);
DRV_Error DRV_MICOM_SetLedControl(HUINT32 ulLedMask, HBOOL bOn_nOff);
DRV_Error  DRV_MICOM_SetBlinkingTime(HUINT32 ulLedMask, unsigned char ucBlinkTime);
DRV_Error DRV_MICOM_WriteString(unsigned char *string, unsigned char len);
DRV_Error DRV_MICOM_SetDimmer(HBOOL bOnOff);
DRV_Error DRV_MICOM_SetPowerSavingMode(HBOOL bOn_nOff);
DRV_Error DRV_MICOM_SetDisplayAni(HBOOL bOn_nOff);
DRV_Error DRV_MICOM_SetLCDPower(HBOOL bOn_nOff);
DRV_Error DRV_MICOM_SetLcdDimmingLevel(unsigned char ucLcdDimmingLevel);
DRV_Error DRV_MICOM_SetLEDDimmingLevel(unsigned char ucLcdDimmingLevel);
DRV_Error DRV_MICOM_Set7SegDimmingLevel(HUINT8 uc7SegDimmingLevel);
DRV_Error DRV_MICOM_SetCustomCode(unsigned short usCCandCCB);
DRV_Error DRV_MICOM_SetUnitTest(unsigned char ucFuncNum, HBOOL bOn_nOff);
void DRV_MICOM_RegisterKeyEventCallback(pfnDRV_UCOM_RCUKey_Notify pfnNotify);
DRV_Error DRV_MICOM_SetWakeupRcuKey(DRV_MICOM_SET_WAKEUPKEY_TYPE nSetType, unsigned char *pucKey, unsigned char ucKeyNum);
DRV_Error DRV_MICOM_GetWakeupRcuKey(unsigned char *pucRcuKeyCode);
DRV_Error DRV_MICOM_SetRCUInfo(HBOOL bOn_nOff);
DRV_Error DRV_MICOM_GetBootTick(HUINT32 *pulCurrentTick);
DRV_Error DRV_MICOM_EnableRescueReset(HBOOL bOn_nOff);
#if defined(CONFIG_MICOM_UPGRADE)
DRV_Error DRV_MICOM_Update(HUINT8 *pMicomImage, HUINT32 binarySize);
#endif

#if defined(CONFIG_FRONT_MICOM_TOUCH)
DRV_Error  DRV_MICOM_GetTouchSensitive(unsigned char *pSensitiveData, unsigned char *pKeyNum);
DRV_Error  DRV_MICOM_GetTouchSignal(unsigned short *pSignalData, unsigned char *pKeyNum);
DRV_Error  DRV_MICOM_GetTouchReference(unsigned short *pRefrenceData, unsigned char *pKeyNum);
DRV_Error DRV_MICOM_SetTouchThreshold(unsigned char *pucThreshold, unsigned char ucTouchNum);
DRV_Error DRV_MICOM_GetTouchThreshold(unsigned char *pucThreshold, unsigned char *pKeyNum);
#endif

#ifdef CONFIG_JAPAN_SPEC
DRV_Error DRV_MICOM_SetMicomAliveTask(HBOOL bEnable);
DRV_Error DRV_MICOM_SetMicomAliveCheck(HBOOL bEnable);
#endif

#if defined(CONFIG_RF4CE_MICOM)
DRV_Error DRV_MICOM_SetPairing(unsigned char ucPairingTimeSec);
DRV_Error DRV_MICOM_GetPairedDevs(unsigned char *pucDevNum);
void DRV_MICOM_RegisterRf4ceEventCallback(pfnDRV_UCOM_RF4CEKey_Notify pfnNotify);
DRV_Error DRV_MICOM_SetRf4ceCerti_InOutMode (MICOM_RF4CE_INOUT_MODE eRF4CEInOutMode);
DRV_Error DRV_MICOM_SetRf4ceCerti_ChangeFreq (unsigned char ucChannelNumber);
void DRV_MICOM_RegisterRf4ceRssiEventCallback(pfnDRV_UCOM_RF4CE_RSSI_Notify pfnNotify);
DRV_Error DRV_MICOM_SetRf4ceFactoryMode(void);
DRV_Error DRV_MICOM_SetIEEEAddr(unsigned char *pucIEEEAddr);
DRV_Error DRV_MICOM_GetIEEEAddr(unsigned char *pucIEEEAddr);
DRV_Error DRV_MICOM_GetPairedDevIEEEAddr(HUINT32 ulIndex, unsigned char *pucValue);
void DRV_MICOM_RegisterRf4cePairingResultCallback(pfnDRV_UCOM_RF4CE_PAIRING_RESULT_Notify pfnNotify);

#if defined(CONFIG_MICOM_JP)
/* For IQView */
DRV_Error DRV_MICOM_SetRf4ceCerti_Iqview_Rx(unsigned char ucChannelNumber);
DRV_Error DRV_MICOM_SetRf4ceCerti_Iqview_Rx_Result(unsigned char *pucOkCnt, unsigned char *pucErrCnt);
DRV_Error DRV_MICOM_SetRf4ceCerti_Iqview_Tx(unsigned char ucChannelNumber);
DRV_Error DRV_MICOM_SetRf4ceCerti_ChangePower(unsigned char ucPowerLevel);
#endif

#if defined(CONFIG_RF4CE_MOTION_TOUCH_UEI_HILCREST)
void DRV_MICOM_RegisterRf4ceUserDataIncomingCallback(pfnDRV_UCOM_RF4CE_USER_DATA_INCOMING_Notify pfnNotify);
DRV_Error DRV_MICOM_TransmitZrcUserData (unsigned char ucPairingTblIndex, void *pvZrcUserData, unsigned char ucLength);
#endif
#endif

/********************************************************************
 DRV_CLOCK  Function Declaration
 ********************************************************************/
void DRV_RTC_MJD2YMD(unsigned short wMJD, DRV_CLOCK_DATE_T *pDate);
void DRV_RTC_YMD2MJD(DRV_CLOCK_DATE_T date, unsigned short *wMJD);
DRV_Error DRV_RTC_ConvertDateTimeToUnixTime(DRV_DATE_TIME dateTime, unsigned long *unixtime);
DRV_Error DRV_RTC_ConvertUnixTimeToDate(unsigned long	unixtime, DRV_CLOCK_DATE_T *pdate);
DRV_Error DRV_RTC_ConvertUnixTimeToTime(unsigned long	unixtime, DRV_CLOCK_TIME_T *ptime);

#if (CONFIG_MICOM_UPGRADE)
DRV_Error DRV_MICOM_TX_BYTE(HUINT8 ucData);
DRV_Error DRV_MICOM_RX_BYTE(HUINT8 *pucData);
DRV_Error  DRV_MICOM_GetReadyForDownload(void);
#endif

#if defined(CONFIG_MICOM_JP)
DRV_Error DRV_uMICOM_UartInit(HBOOL bOn_nOff, unsigned char ucTimeOutSec);
#endif

#endif /* _drvMicom_h_ */
