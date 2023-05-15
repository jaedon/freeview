/*******************************************************************************
* File name : drv_spi.h
* Author : D.K Lee
* description :
*
* Copyright (c)  by Humax Co., Ltd. Since 2007.
* All right reserved
*******************************************************************************/
 
#ifndef _DRV_STLED_H_
#define _DRV_STLED_H_

/*******************************************************************************
* Headers
*******************************************************************************/
#include "drv_gpio.h"

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/
#define FRONTPANEL_MESSAGE_BYTES		(4)
#define FRONTPANEL_MESSAGE_QUEUE_SIZE	(8)
	
	/* Constants ----------------------------------------------------------------*/
#define FRONT_STB GPIO_ID_SPI_M_SS0  /* pio1[4] : FRONT_STROBE */
#define FRONT_CLK GPIO_ID_SPI_M_CLK  /* pio1[5] : FRONT_CLK */
#define FRONT_DO GPIO_ID_SPI_M_MOSI  /* pio1[6] : FRONT_DIO */
#define FRONT_DI GPIO_ID_SPI_M_MISO  /* pio1[6] : FRONT_DIO */
	
	/* Number */
#define NUMBER_OF_SEGMENTS 4
#define NUMBER_OF_LEDS     2
#define NUMBER_OF_KEYS     2
	
	/* Pages */
#define STLED136S_7SEGMENTS_MEMORY_PAGE          0x00
#define STLED136S_LEDS_DISPLAT_PAGE              0x01
#define STLED136S_7SEGMENTS_BRIGHTNESS_PAGE      0x02
#define STLED136S_LEDS_BRIGHTNESS_PAGE           0x03
	
	/* Offsets : 7Segment Display Memory */
#define STLED136S_7SEGMENT0_DISPLAY_MEM          0x00
#define STLED136S_7SEGMENT1_DISPLAY_MEM          0x01
#define STLED136S_7SEGMENT2_DISPLAY_MEM          0x02
#define STLED136S_7SEGMENT3_DISPLAY_MEM          0x03
#define STLED136S_7SEGMENT4_DISPLAY_MEM          0x03
#define STLED136S_7SEGMENT5_DISPLAY_MEM          0x03
	
	/* Offsets : 7Segment Brightness Setting */
#define STLED136S_7SEGMENT0_BRIGHTNESS           0x00
#define STLED136S_7SEGMENT1_BRIGHTNESS           0x01
#define STLED136S_7SEGMENT2_BRIGHTNESS           0x02
#define STLED136S_7SEGMENT3_BRIGHTNESS           0x03
	
#define SET_FIXED_WRITE_CMD(Page,Offset)        (((Page & 0x3)<<3 | (Offset & 0x07)) | 0x20)
#define SET_VARIBLE_WRITE_CMD(Page,Offset)      (((Page & 0x3)<<3 | (Offset & 0x07)) & ~0x20)
#define SET_SUPPORTED_DIGITS(x)                 ((x)-1)
	
#define STDLED135S_FIXED_BRIGHTNESS_MODE         0x18
#define STDLED135S_VARIABLE_BRIGHTNESS_MODE      0x00
	
	/* Command : Turn On/Off */
#define STLED136S_CMD_TURN_ON_DISPLAY            0x0D
#define STLED136S_CMD_TURN_OFF_DISPLAY           0x0E
	
	/* Command : LED Display */
#define STLED136S_CMD_DISPLAY_LED                0x28  /* write, fixed addr, page:1 addr:0 */
	
	/* Command : Read Key */
#define STLED136S_CMD_READ_KEY1                  0x49  /* read, increase addr, page:1 addr:1 */
#define STLED136S_CMD_READ_KEY2                  0x6a  /* read, fixed addr, page:1 addr:2 */
	
	/* Data */
#define STLED136S_MAX_BRIGHTNESS_VAL             0x07
#define STLED136S_BRIGHTNESS_MASK                0x07
#define STLED136S_NOM_BRIGHTNESS_VAL             0x02


/*******************************************************************************
* Global variables and structures
*******************************************************************************/


/*******************************************************************************
* External variables and functions
*******************************************************************************/
DRV_Error DRV_STLED_WriteString(unsigned char *string, unsigned char len, int isTime);
DRV_Error DRV_STLED_Brightness(int value);
DRV_Error DRV_STLED_DisplayLED (unsigned char *ucData);
DRV_Error DRV_STLED_7SegmentOnOff (bool OnOff);
DRV_Error DRV_STLED_7SegmentTest(int Onoff);
DRV_Error DRV_STLED_Init(void);

/*******************************************************************************
* Static variables and structures
*******************************************************************************/


/*******************************************************************************
* Static function prototypes
*******************************************************************************/


#endif /* _DRV_STLED_H_ */


/* end of file */

