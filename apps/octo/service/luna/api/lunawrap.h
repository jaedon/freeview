#ifndef luna_wrap_h_
#define luna_wrap_h_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef bool
#define bool 	int
#endif
#ifndef TRUE
#define TRUE 	1
#endif
#ifndef FALSE
#define FALSE	0
#endif

typedef	enum
{
	exHWS_WindowInputEnable		= 0x00000001,
	exHWS_WindowNoSurface		= 0x00000002,
	exHWS_WindowInputAlways		= 0x00000004,
	exHWS_WindowInputFilter		= 0x00000008,
	exHWS_WindowOwnControl		= 0x00000010,

	exHWS_NONE					= 0x00000000
} HxWINDOWSTYLE_e;

typedef	enum
{
	exHRELATION_UP,
	exHRELATION_DOWN
} HxRELATION_e;

typedef enum
{
	exHINPUTRET_NONE		= 0,
	exHINPUTRET_PASS,
	exHINPUTRET_CONSUMED
} HxInputRetType_e;

typedef	enum
{
	exHSTACK_FOCUS,
	exHSTACK_WINDOW,
	exHSTACK_PANEL,
	exHSTACK_MAX
} HxSTACKTYPE_e;

typedef struct
{
	unsigned int		window_id;			//	target window
	unsigned int		request_id;			//	request id
	HxInputRetType_e	eInputReturnType;	//	input event return type;

	unsigned int		type;
	unsigned int		device_id;
	unsigned int		flags;

	int	key_code;
	int	key_id;
	int	key_symbol;
	int	modifiers;

	int	locks;
	int	button;
	int	buttons;

	int	axis;
	int	axisabs;
	int	axisrel;

	int	min;
	int	max;
} HxInputEvent_t;

extern unsigned int 	LAPI_Init(void);
extern unsigned int 	LAPI_GetWindowId(void);
extern unsigned int 	LAPI_SetWindowStyle(int nWindId, HxWINDOWSTYLE_e eWindowStyle, bool bClear);
extern unsigned int 	LAPI_SetApplicationFocusOrder(int nWindId, int nBaseWinId, HxRELATION_e eRelation);
extern unsigned int 	LAPI_SetApplicationWindowOrder(int nWindId, int nBaseWinId, HxRELATION_e eRelation);
extern bool 			LAPI_GetInputEvent(int nWindId, HxInputEvent_t *pstInputEvent, int nTimeout);
extern unsigned int 	LAPI_ReturnKeyEvent(int nWindId, HxInputRetType_e eRetType, HxInputEvent_t *pstInputEvent);

#endif

