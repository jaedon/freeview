/**
 * drv_led.h
*/
#ifndef __DRV_LED_H__
#define __DRV_LED_H__

/*----------------------------------------------------------------------
** Header
**--------------------------------------------------------------------*/

/*----------------------------------------------------------------------
** Definition
**--------------------------------------------------------------------*/
enum {
	LED_ID_NUM0 	= 0x00000001,	
	LED_ID_NUM1 	= 0x00000002,
	LED_ID_NUM2 	= 0x00000004,
	LED_ID_NUM3 	= 0x00000008,
	LED_ID_NUM4 	= 0x00000010,
	LED_ID_NUM5 	= 0x00000020,
	LED_ID_NUM6 	= 0x00000040,
	LED_ID_NUM7 	= 0x00000080,
	LED_ID_NUM8 	= 0x00000100,
	LED_ID_NUM9 	= 0x00000200,
	LED_ID_NUM10	= 0x00000400,
	LED_ID_NUM11	= 0x00000800,
	LED_ID_NUM12	= 0x00001000,
	LED_ID_NUM13	= 0x00002000,
	LED_ID_NUM14	= 0x00004000,
	LED_ID_NUM15	= 0x00008000,
	LED_ID_NUM16	= 0x00010000,
	LED_ID_NUM17	= 0x00020000,
	LED_ID_NUM18	= 0x00040000,
	LED_ID_NUM19	= 0x00080000,
	LED_ID_NUM20	= 0x00100000,
	LED_ID_NUM21	= 0x00200000,
	LED_ID_NUM22	= 0x00400000,
	LED_ID_NUM23	= 0x00800000,
	LED_ID_NUM24	= 0x01000000,
	LED_ID_NUM25	= 0x02000000,
	LED_ID_NUM26	= 0x04000000,
	LED_ID_NUM27	= 0x08000000,
	LED_ID_NUM28	= 0x10000000,
	LED_ID_NUM29	= 0x20000000,
	LED_ID_POWER	= 0x40000000	/* reserved */
};

enum {
	LED_FUNC_NONE = 0,
	LED_FUNC_NORMAL,
	LED_FUNC_WATCHING,
	LED_FUNC_STANDBY,
	LED_FUNC_RECORDING,
	LED_FUNC_RCU_BLINKING,
	LED_FUNC_MAX
};

enum {
	LED_RGB_R = 0x00000001,
	LED_RGB_G = 0x00000002,
	LED_RGB_B = 0x00000004
};

struct led_setting_t{
	unsigned id;
	unsigned func;
	unsigned on;
	unsigned rgb;
	unsigned dim_set;
	unsigned dim_level;
};

struct led_platform_driver_t {
	int (*set)(struct led_setting_t *pSetting);
	unsigned led_cap;
	unsigned use_dimming;
};

/*----------------------------------------------------------------------
** Global Variables
**--------------------------------------------------------------------*/

/*----------------------------------------------------------------------
** Static Variables
**--------------------------------------------------------------------*/

/*----------------------------------------------------------------------
** Function Prototype
**--------------------------------------------------------------------*/
DRV_Error DRV_LED_Init(void);
void      DRV_LED_EarlyResume(void);
DRV_Error DRV_CFG_LED_PlatformDrvInit(struct led_platform_driver_t *tLedDrv);
#endif

