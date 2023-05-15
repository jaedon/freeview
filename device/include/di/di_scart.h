/**
 * di_scart.h
*/

/**
 * @defgroup		DI_SCART DI_SCART : DI_SCART module
 * @author		Chung Dong Chul
 * @note			DI_SCART APIs
 * @brief			Define DI_SCART APIs
 * @file 			di_scart.h
*/

#ifndef __DI_SCART_H__
#define __DI_SCART_H__

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
#include "htype.h"
#include "di_err.h"


/*******************************************************************/
/************************* Type Definition *************************/
/*******************************************************************/

/******************************************************************************/
/***************************** IDTV API - START *******************************/
/******************************************************************************/
typedef enum
{	
	DI_SCART_TYPE_STB = 0,
	DI_SCART_TYPE_TV,
	DI_SCART_TYPE_MAX
} DI_SCART_TYPE;

typedef enum
{
	DI_SCART_ID_1 = 0,
	DI_SCART_ID_2,
	DI_SCART_ID_MAX
} DI_SCART_ID; 
/******************************************************************************/
/***************************** IDTV API - END *********************************/
/******************************************************************************/


typedef enum 
{
	/*
	 * Aspect Ratio Setting 
	 *		Related variables:
	 *					DI_SCART_SETTING.stNormalMode.eTvAspectRatioOutput
	 *					DI_SCART_SETTING.stNormalMode.eVcrAspectRatioOutput
	 *					DI_SCART_SETTING.stNormalMode.eTVAspectRatioInput
	 */
	DI_SCART_NO_SIGNAL=0,		
	DI_SCART_AR_4_3,
	DI_SCART_AR_16_9,

	/*
	 * Video Output Type Setting
	 *		Related variables:
	 *					DI_SCART_SETTING.stNormalMode.eTvVideoOutput
	 *					DI_SCART_SETTING.stNormalMode.eVcrVideoOutput
	 *   				DI_SCART_SETTING.stNormalMode.eTvVideoInput
	 */
	DI_SCART_OUTPUT_CVBS,
	DI_SCART_OUTPUT_RGB,
	DI_SCART_OUTPUT_SVIDEO,

	/*
	 * Bypass Type Setting
	 *		Related variables:
	 *					DI_SCART_SETTING.eMode
	 */
	DI_SCART_MODE_NORMAL,		// Path : STB->TV & STB->VCR
	DI_SCART_MODE_BYPASS,		// Path : VCR->TV 
	DI_SCART_MODE_HW_BYPASS,	// Path : VCR<->TV (bi-directional), Auto Detection is enabled.
	DI_SCART_MODE_HW_BYPASS_NO_DET,	// Path : VCR<->TV (bi-directional)

	DI_SCART_MAX
} DI_SCART_SETTING_VALUE;

typedef enum
{
	DS_EVENT_VCR_AR_CHANGE,							// STB
	DS_EVENT_VCR_PWR_CHANGE,						// STB
	DS_EVENT_TV_CONTROL_CHANGE,						// TV
	DS_EVENT_MAX
} DI_SCART_EVENT;

typedef struct
{
	/*
	 * pin 8 (Slow Blanking) : 4_3, 16_9, no_signal
	 */
	DI_SCART_SETTING_VALUE eTvAspectRatioOutput;	
	DI_SCART_SETTING_VALUE eVcrAspectRatioOutput; 	
	DI_SCART_SETTING_VALUE eTvAspectRatioInput;		// TV
	/* 
	 * pin 16 (FastBlankingPin) : CVBS, RGB, S-Video
	 */
	DI_SCART_SETTING_VALUE eTvVideoOutput; 			// STB
	DI_SCART_SETTING_VALUE eVcrVideoOutput;			// STB
	DI_SCART_SETTING_VALUE eTvVideoInput;				// TV
} DI_SCART_NORMAL_MODE_SETTINGS;

typedef struct
{
	DI_SCART_SETTING_VALUE eMode;						// STB
	DI_SCART_NORMAL_MODE_SETTINGS stNormalMode;		// STB(Output) & TV(Input)
} DI_SCART_SETTINGS;

typedef void (*DI_SCART_CALLBACK)(void*);

/******************************************************************************/
/***************************** IDTV API - START *******************************/
/******************************************************************************/
typedef struct
{
    DI_SCART_ID eScartId;
    DI_SCART_SETTINGS stSettings;
} DI_SCART_EVENT_PARAM;

typedef struct
{
    DI_SCART_TYPE eScartType;   
    HUINT32 ulScartNum;       
} DI_SCART_CAPA; 
/******************************************************************************/
/***************************** IDTV API - END *********************************/
/******************************************************************************/

/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
DI_ERR_CODE DI_SCART_SetSettings(DI_SCART_SETTINGS* pSettings);
void DI_SCART_GetSettings(DI_SCART_SETTINGS* pSettings);
DI_ERR_CODE DI_SCART_RegisterEventNotify(DI_SCART_EVENT eScartEvent, DI_SCART_CALLBACK pfScartEventCallback);


/******************************************************************************/
/***************************** IDTV API - START *******************************/
/******************************************************************************/
DI_ERR_CODE DI_SCART_GetCapability (DI_SCART_CAPA *pScartCapa);
DI_ERR_CODE DI_SCART_SetSettingsTv(DI_SCART_ID eScartId, DI_SCART_SETTINGS* pSettings);
DI_ERR_CODE DI_SCART_GetSettingsTv(DI_SCART_ID eScartId, DI_SCART_SETTINGS* pSettings);
/******************************************************************************/
/***************************** IDTV API - END *********************************/
/******************************************************************************/

#endif /* !__DI_SCART_H__ */ 
