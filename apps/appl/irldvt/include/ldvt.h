/*
 * (c) 2011-2012 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/
/****************************************************************************
	Global header for Loader Test Application.
****************************************************************************/
#include <autoconf.h>

#include <htype.h>



/****************************************************************************
	Definitions
****************************************************************************/
#define	LDVT_MSGQ_SIZE			256
#define	LDVT_MSGQ_NAME			"LDVTMsgQ"
#define	LDVT_TASK_PRIORITY		VK_TASK_PRIORITY_MW_REF
#define	LDVT_TASK_STACK_SIZE	65536
#define	LDVT_TASK_NAME			"LDVTMAINTASK"
#define	LDVT_KEYTASK_NAME		"LDVTKEYTASK"
#define	INVALID_COUNT			0xFFFF
#define	MIN_PART_NUM			1

#if defined(CONFIG_PROD_IRHD5100C) || defined(CONFIG_PROD_IHDR5200C) || defined(CONFIG_PROD_IRHD5300C) || defined(CONFIG_PROD_HDH100S) || defined(CONFIG_PROD_IR2020HD) || \
         defined(CONFIG_PROD_HM9502HD) || defined(CONFIG_PROD_IRHD5100S) || defined(CONFIG_PROD_IRHD5100CTVT) || defined(CONFIG_PROD_ICORDHDPLUSME) || defined(CONFIG_PROD_IRHD5200S) || defined(CONFIG_PROD_ICORDHDPLUSMEADM)
#define	MAX_PART_NUM		240

#elif defined(CONFIG_PROD_IR2000SD)
#define	MAX_PART_NUM		108

#elif defined(CONFIG_PROD_VTCHD06) || defined(CONFIG_PROD_UDFOXHD) || defined(CONFIG_PROD_HDFREE) || defined(CONFIG_PROD_IR3025HD) || defined(CONFIG_PROD_IR3035HD)
#define	MAX_PART_NUM		146	/* FLASH_TOTAL_PARTITION_COUNT */

#elif defined(CONFIG_PROD_IR3020HD) || defined(CONFIG_PROD_IR3000HD) || defined(CONFIG_PROD_IR3100HD) || defined(CONFIG_PROD_IR3030HD) ||\
	  defined(CONFIG_PROD_HM9504HD) || defined(CONFIG_PROD_HDR1003S)
#define	MAX_PART_NUM		82	/* FLASH_TOTAL_PARTITION_COUNT */

#elif defined(CONFIG_PROD_HMS1000S)
#define	MAX_PART_NUM		82	/* FLASH_TOTAL_PARTITION_COUNT */

#elif defined(CONFIG_PROD_ICORDPRO)
#define	MAX_PART_NUM		82	/* FLASH_TOTAL_PARTITION_COUNT */

#elif defined(CONFIG_PROD_IR4000HD)
#define	MAX_PART_NUM		168	/* FLASH_TOTAL_PARTITION_COUNT */
#else

#error "max partition not defined."
#endif

enum
{
	LDVT_MSG_KEYUP 	 = 0x1000,
	LDVT_MSG_KEYDOWN = 0x1001,
	LDVT_MSG_CHANNEL = 0x1002,

};

enum
{
	LDVT_ARROWUP 	= 0x1003,
	LDVT_ARROWDOWN 	= 0x1004,
	LDVT_ARROWLEFT 	= 0x1005,
	LDVT_ARROWRIGHT	= 0x1006

};

enum
{
	LDVT_KEY_0 = 0x1007,
	LDVT_KEY_1 = 0x1008,
	LDVT_KEY_2 = 0x1009,
	LDVT_KEY_3 = 0x100A,
	LDVT_KEY_4 = 0x100B,
	LDVT_KEY_5 = 0x100C,
	LDVT_KEY_6 = 0x100D,
	LDVT_KEY_7 = 0x100E,
	LDVT_KEY_8 = 0x100F,
	LDVT_KEY_9 = 0x1010,
	LDVT_KEY_10 = 0x1011,
	LDVT_KEY_11 = 0x1012
};

enum
{
	LDVT_KEY_OK = 0x1019,
	LDVT_KEY_BACK = 0x1020,
	LDVT_KEY_EXIT = 0x1021,
	//LDVT_KEY_BACK = 0x1022,
};

enum
{
	LDVT_KEY_RED = 0x1030,
	LDVT_KEY_GREEN = 0x1031,
	LDVT_KEY_YELLOW = 0x1032,
	LDVT_KEY_BLUE = 0x1033,
};



#define LDVT_VKEY_OK				DIKS_ENTER
#define LDVT_VKEY_BACK				DIKS_BACK
#define LDVT_VKEY_UP				DIKS_CURSOR_UP
#define LDVT_VKEY_DOWN				DIKS_CURSOR_DOWN
#define LDVT_VKEY_LEFT				DIKS_CURSOR_LEFT
#define LDVT_VKEY_RIGHT				DIKS_CURSOR_RIGHT
#define LDVT_VKEY_1					DIKS_1
#define LDVT_VKEY_2					DIKS_2
#define LDVT_VKEY_3					DIKS_3
#define LDVT_VKEY_4					DIKS_4
#define LDVT_VKEY_5					DIKS_5
#define LDVT_VKEY_6					DIKS_6
#define LDVT_VKEY_7					DIKS_7
#define LDVT_VKEY_8					DIKS_8
#define LDVT_VKEY_9					DIKS_9
#define LDVT_VKEY_0					DIKS_0
#define LDVT_VKEY_11				DIKS_CUSTOM11
#define LDVT_VKEY_12				DIKS_CUSTOM12
#define LDVT_VKEY_POWER				DIKS_POWER
#define LDVT_VKEY_EXIT				DIKS_EXIT
#define LDVT_VKEY_BLUE    			DIKS_BLUE
#define LDVT_VKEY_RED    			DIKS_RED
#define LDVT_VKEY_YELLOW   			DIKS_YELLOW
#define LDVT_VKEY_GREEN    			DIKS_GREEN
/****************************************************************************
	Structures.
****************************************************************************/
typedef	struct tagDateType
{
	HUINT16	usYear;
	HUINT8	ucMonth;
	HUINT8	ucDay;
} DATE_T, SYS_DATE_T ;

typedef struct tagTimeType
{
	HUINT8	ucHour;
	HUINT8	ucMinute;
	HUINT8	ucSecond;
	HUINT16	usMillisecond;
} TIME_T, SYS_TIME_T ;

typedef	struct
{
	HUINT32		ulMsgId;
	HUINT32		ulArg1, ulArg2, ulArg3;
}	LMSG;

typedef	void(*ProcessItemFunc)(void *pvMenuInfo, void *pvItemInfo, HUINT32 ulMsg, HUINT32 ulArg1, HUINT32 ulArg2, HUINT32 ulArg3);
typedef	void(*DrawItemFunc)(void *pvMenuInfo, void *pvItemInfo, HINT32 nIdx);
typedef	void(*ProcessFunc)(void *pvVoid, HUINT32 ulMsg, HUINT32 ulArg1, HUINT32 ulArg2, HUINT32 ulArg3);
typedef	void(*DrawFunc)(void *pvVoid);
typedef	void(*InitMenu)(void *pvVoid);

typedef	struct tagItem_t
{
	HUINT32		nItemId;
	HUINT32		nTargetMenuId;
	HUINT32		nValue;

	HUINT8		*pszName;

	ProcessItemFunc	pfnItemProc;
	DrawItemFunc	pfnDrawProc;
} Item_t;

typedef	struct tagMenuItem_t
{
	HUINT32		nMenuId;
	HUINT32		nPrevMenuId;
	HUINT8		*pszTitle;
	HUINT8		*pszTail1, *pszTail2, *pszTail3;

	InitMenu	pfnInitMenu;
	ProcessFunc	pfnMenuProcessFunc;
	DrawFunc	pfnMenuDrawFunc;

	HUINT32		nItemNum;
	HUINT32		nCurSelItemIndex;
	HUINT32		aulItems[32];

	Item_t		*pstItems[32];
} MenuItem_t;

/****************************************************************************
	Functions.
****************************************************************************/
extern	void	LDVT_WINDOW_Create(void);
extern	void	LDVT_WINDOW_UpdateScreen(HINT32 x1, HINT32 y1, HINT32 x2, HINT32 y2);
extern	void	LDVT_WINDOW_SetPixel(HINT32 x, HINT32 y, HUINT32 col);
extern	void	LDVT_WINDOW_FillRect(HINT32 x, HINT32 y, HINT32 w, HINT32 h, HUINT32 col);
extern	void	LDVT_WINDOW_DrawString(HINT32 x, HINT32 y, HINT32 w, HINT32 h, HINT32 fs, HUINT8 *pszString, HUINT32 col, HINT32 eAlign);
extern	void	LDVT_WINDOW_GetScreenSize(HUINT32 *pulWidth, HUINT32 *pulHeight);
//extern	void	WRAP_PLANE_GetFullSize(MRect *pRect);

extern	void	DVTAPP_InitAppl(void);
extern	void	DVTAPP_ProcessMessage(HUINT32 ulMsg, HUINT32 ulArg1, HUINT32 ulArg2, HUINT32 ulArg3);
extern	void	DVTAPP_RedrawScreen(void);
extern  void	LDVT_InitAppl(void);
extern  HINT32 xsvc_cas_IrLoaderInit(void);


//extern  HERROR SVC_OUT_SetOsdLayerOutput(OsdZOrder_t		ulZOrder,
//							DxAspectRatio_e		eOsdAspectRatio,
//							DxDfcSelect_e 			eDfc,
//							PIG_Rect_t				stPigRect,
//							DxAspectRatio_e 	ePigAspectRatio
//							);

#ifdef	CONFIG_MW_CH_SATELLITE
#include <irldvt_stb_params.h>
//#include <dlib_tuneparam.h>
#include <di_channel_s.h>
//#include <irldvt_type.h>

/************************************************
		Tuner, Demudulator Parameters
*************************************************/
CH_SAT_Polarization_e	_dvtApp_sat_ConvPAL2DI_Polarization(DxSat_Polarization_e polarization);
CH_SAT_CodeRate_e		_dvtApp_sat_ConvPAL2DI_CodeRate(DxSat_CodeRate_e codeRate);
CH_SAT_LnbVoltage_e		_dvtApp_sat_ConvPAL2DI_LnbVoltage(DxSAT_LnbVoltage_e lnbVoltage);
CH_SAT_DiseqcInput_e	_dvtApp_sat_ConvPAL2DI_DiseqcInput(DxSAT_DiseqcInput_e diseqcInput);
CH_SAT_ToneBurst_e		_dvtApp_sat_ConvPAL2DI_ToneBurst(DxSAT_ToneBurst_e toneBurst);
CH_SAT_DiseqcVer_e		_dvtApp_sat_ConvPAL2DI_DiseqcVersion(DxSAT_DiseqcVer_e diseqcVer);
CH_SAT_TransportSpec_e	_dvtApp_sat_ConvPAL2DI_TransSpec(DxSat_TransportSpec_e transSpec);
CH_SAT_Modulation_e		_dvtApp_sat_ConvPAL2DI_Modulation(DxSat_PskModulation_e pskModulation);
CH_SAT_Pilot_e			_dvtApp_sat_ConvPAL2DI_Pilot(DxSat_Pilot_e pilot);
DxSat_Polarization_e		_dvtApp_sat_ConvDI2PAL_Polarization(CH_SAT_Polarization_e polarization);
DxSat_CodeRate_e			_dvtApp_sat_ConvDI2PAL_CodeRate(CH_SAT_CodeRate_e codeRate);
DxSAT_LnbVoltage_e		_dvtApp_sat_ConvDI2PAL_LnbVoltage(CH_SAT_LnbVoltage_e lnbVoltage);
DxSAT_DiseqcInput_e		_dvtApp_sat_ConvDI2PAL_DiseqcInput(CH_SAT_DiseqcInput_e diseqcInput);
DxSAT_ToneBurst_e			_dvtApp_sat_ConvDI2PAL_ToneBurst(CH_SAT_ToneBurst_e toneBurst);
DxSAT_DiseqcVer_e			_dvtApp_sat_ConvDI2PAL_DiseqcVersion(CH_SAT_DiseqcVer_e diseqcVer);
DxSat_TransportSpec_e		_dvtApp_sat_ConvDI2PAL_TransSpec(CH_SAT_TransportSpec_e transSpec);
DxSat_PskModulation_e		_dvtApp_sat_ConvDI2PAL_Modulation(CH_SAT_Modulation_e pskModulation);
DxSat_Pilot_e				_dvtApp_sat_ConvDI2PAL_Pilot(CH_SAT_Pilot_e pilot);
#if defined(CONFIG_OP_DIGITURK)
HUINT32					_dvtApp_sat_Conv_LnbType2LnbFreq(HUINT8 LnbType);
HUINT8					_dvtApp_sat_Conv_LnbFreq2LnbType(HUINT32 ulLnbFreq);
#endif
#endif
