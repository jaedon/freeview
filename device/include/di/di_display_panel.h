
/****************************************************************************
* $Workfile:  di_display_panel.h  $
* $Modtime:   Aug.21.2009 $
*
* Author:
* Description:
*
*                                 Copyright (c) 2009 HUMAX Co., Ltd.
*                                               All rights reserved.
*****************************************************************************/

#ifndef _DI_DISPLAY_PANEL_H_
#define _DI_DISPLAY_PANEL_H_

/****************************************************************************
* Includes
*****************************************************************************/

/****************************************************************************
* Definitions
*****************************************************************************/

/****************************************************************************
* Typedef
*****************************************************************************/
typedef enum
{
	DI_DISPLAY_PANEL_LC320EUH_SCA1, // LGD 32 Inch LED With T-CON
	DI_DISPLAY_PANEL_LC370EUH_SCA1, // LGD 37 Inch LED With T-CON
	DI_DISPLAY_PANEL_LC420EUH_SCA1, // LGD 42 Inch LED With T-CON
	DI_DISPLAY_PANEL_LC470EUH_SCA1, // LGD 47 Inch LED With T-CON
	DI_DISPLAY_PANEL_LC320WUH_SCA1, // LGD 32 Inch LCD With T-CON
	DI_DISPLAY_PANEL_LC420WUH_SCA1, // LGD 42 Inch LCD With T-CON
	DI_DISPLAY_PANEL_LK315D3LA57,   // Sharp 31.5 Inch LCD Without T-CON
	DI_DISPLAY_PANEL_LK420D3LA87,   // Sharp 42 Inch LCD Without T-CON
	DI_DISPLAY_PANEL_LC320WUD_SCB1, //JVC Not Used
	DI_DISPLAY_PANEL_LC370WUD_SCB1,
	DI_DISPLAY_PANEL_LC420WUD_SCB1,
	DI_DISPLAY_PANEL_NUM
} DI_DISPLAY_PANEL_TYPE_e;

#define DI_DISPLAY_PANEL_LCD_DEFAULT_PANEL DI_DISPLAY_PANEL_LK315D3LA57
#define DI_DISPLAY_PANEL_LED_DEFAULT_PANEL DI_DISPLAY_PANEL_LC320EUH_SCA1


typedef enum
{
	DI_DISPLAY_PANEL_LUMINOUS_TYPE_LCD,
	DI_DISPLAY_PANEL_LUMINOUS_TYPE_LED,
	DI_DISPLAY_PANEL_LUMINOUS_TYPE_NUM
} DI_DISPLAY_PANEL_LUMINOUS_TYPE_e;


typedef struct
{
	DI_DISPLAY_PANEL_TYPE_e			ePanelType;			//panel enum
	HUINT16							usWidth;			// horizontal pixels
	HUINT16							usHeight;			//vertical lines
	HUINT16							usPanelSizeInch;	// Panel size(inch)
	//char *							PanelName;			//panel name string
	HUINT16							usPanelLedType;    //True when type is LED
	HUINT16							usTconType;			//True when TCONLESS Panel
} DI_DISPLAY_PANEL_INFO_t;



/****************************************************************************
* Export Variables
*****************************************************************************/

/****************************************************************************
* Global function prototypes
*****************************************************************************/
DI_ERR_CODE DI_DISPLAY_PANEL_Select(DI_DISPLAY_PANEL_TYPE_e ePanelType);
DI_ERR_CODE DI_DISPLAY_PANEL_SetBacklight(HBOOL bOnOff);
DI_ERR_CODE DI_DISPLAY_PANEL_GetInfo(DI_DISPLAY_PANEL_INFO_t *ptPanelInfo);

DI_ERR_CODE DI_DISPLAY_PANEL_Off(void);

DI_ERR_CODE DI_DISPLAY_PANEL_SetVcom(HUINT16 Vcom, HBOOL Volatile);
DI_ERR_CODE DI_DISPLAY_PANEL_GetVcom(HUINT16* Vcom);

DI_ERR_CODE DI_DISPLAY_PANEL_GetPanelType(DI_DISPLAY_PANEL_LUMINOUS_TYPE_e *ptPanelType);
DI_ERR_CODE DI_DISPLAY_PANEL_GetSelectPanelInfo(DI_DISPLAY_PANEL_TYPE_e ePanelType, DI_DISPLAY_PANEL_INFO_t *ptPanelInfo);


#endif /* _DI_DISPLAY_PANEL_H_ */

