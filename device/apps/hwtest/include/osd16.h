#ifndef _OSD16_H
#define _OSD16_H

#define PAL_OFFSET		(0)

#define NTSC_X_SPAN	720
#define NTSC_Y_SPAN 480

#define PAL_M_X_SPAN		NTSC_X_SPAN
#define PAL_M_Y_SPAN		NTSC_Y_SPAN

#define PAL_X_SPAN	720
#define PAL_Y_SPAN	576

#define PAL_N_X_SPAN		PAL_X_SPAN
#define PAL_N_Y_SPAN		PAL_Y_SPAN

// COLOR SYSTEM
#define	COLOR_SYSTEM_TRANS			0x0000	// TRANS Color
#define	COLOR_SYSTEM_BLACK			0xFB00	//Black

// COLOR TEXT
#define	COLOR_TEXT_BLACK				0xF111
#define	COLOR_TEXT_DARKGREY			0xF333
#define	COLOR_TEXT_GREY				0xF445
#define	COLOR_TEXT_MGREY				0xFAAA	// Help Text
#define	COLOR_TEXT_LIGHTGREY			0xFBBB
#define	COLOR_TEXT_WHITE				0xFDDD
#define	COLOR_TEXT_FRESHGREY			0xF677	// Disable Text
#define	COLOR_TEXT_BLUEGREY			0xF89A	// Disable Text
#define	COLOR_TEXT_LIGHTRED			0xFD9A	// Color Key
#define	COLOR_TEXT_REDGREY			0xF877	// Caendar - Sunday
#define	COLOR_TEXT_RED					0xFF11	// Play Bar
#define	COLOR_TEXT_DARKRED			0xF300	// Color Key
#define	COLOR_TEXT_LIGHTGREEN			0xFAB6	// Color Key
#define	COLOR_TEXT_GREEN				0xF6C0	// Play Bar
#define	COLOR_TEXT_DARKGREEN			0xF030	// Color Key
#define	COLOR_TEXT_LIGHTYELLOW		0xFCB5	// Color Key
#define	COLOR_TEXT_BROWN				0xFA50	// Cursor Text
#define	COLOR_TEXT_DARKBROWN			0xF420	// Cursor Text
#define	COLOR_TEXT_LIGHTBLUE			0xF9BE	// Color Key
#define	COLOR_TEXT_BLUE				0xF039	// Color Key
#define	COLOR_TEXT_BLUE2				0xF03C	// Media New recording
#define	COLOR_TEXT_LIGHTSKYBLUE		0xFADF	// i-Plate, Infor Popup
#define	COLOR_TEXT_SKYBLUE			0xF4BE	// Play Bar
#define	COLOR_TEXT_REDVIOLET			0xF306	// Strength
#define	COLOR_TEXT_BLUEVIOLET			0xF238	// Event Text
#define	COLOR_TEXT_DARKBLUE			0xF003	// EPG Cell Text
#define	COLOR_TEXT_COBALTBLUE			0xF006	// Menu Title Text
#define	COLOR_TEXT_YELLOW_T			0xFDC7	// Menu Title Text
#define	COLOR_TEXT_POPUP_T			0xF68B	// Blue Popup Title Text
#define	COLOR_TEXT_VIOLET				0xF336	// Media List - HDD Text
#define	COLOR_TEXT_PINK				0xFFDE	// EPG - Data
#define	COLOR_TEXT_LIGHTBLUEGRAY		0xFCDE	// EPG - Time
#define	COLOR_TEXT_LEMONYELLOW		0xFFEA	// Loader Text
#define	COLOR_TEXT_RED2				0xFF00


// COLOR_WINDOW
#define	COLOR_WIN_BG1					0xFBBC	// Menu, Surfer
#define	COLOR_WIN_BG2					0xF667	// 2 Depth BG
#define	COLOR_WIN_BG3					0xF334	// EPG Title, Bottom
#define	COLOR_WIN_BG4					0xF122	// Wizard Title
#define	COLOR_WIN_BG5					0xF233	// i-Plate, LiySync

#define	COLOR_WIN_ICON_BG1			0xF667	// 1 Depth
#define	COLOR_WIN_ICON_BG2			0xF9AA	// 2 Depth

#define	COLOR_WIN_LINE1				0xFCDD
#define	COLOR_WIN_LINE2				0xFBBB
#define	COLOR_WIN_LINE3				0xF9AA

#define	COLOR_WIN_FRAME1				0xF334	// 1 Depth
#define	COLOR_WIN_FRAME2				0xF233	// 2 Depth

#define	COLOR_WIN_GREYCELL1			0xF899	// EPG Cell
#define	COLOR_WIN_GREYCELL2			0xF9AA	// EPG Cell

#define	COLOR_WIN_GREYCELL_LINE1		0xFBBB	// EPG Cell
#define	COLOR_WIN_GREYCELL_LINE2		0xF899	// EPG Cell
#define	COLOR_WIN_GREYCELL_LINE3		0xF788	// EPG Cell

#define	COLOR_WIN_POPUP_RED1			0xF734
#define	COLOR_WIN_POPUP_RED2			0xF724
#define	COLOR_WIN_POPUP_RED3			0xF512

#define	COLOR_WIN_POPUP_BLUE1			0xF558
#define	COLOR_WIN_POPUP_BLUE2			0xF346
#define	COLOR_WIN_POPUP_BLUE3			0xF235
#define	COLOR_WIN_POPUP_BLUE_BG		0xFABC

#define	COLOR_WIN_LIST_FRAME1			0xF344
#define	COLOR_WIN_LIST1				0xF678
#define	COLOR_WIN_LIST2				0xF567
#define	COLOR_WIN_LIST3				0xF556

#define	COLOR_WIN_P_LINE1				0xFCCD	// PopUp Window
#define	COLOR_WIN_P_LINE2				0xFAAB
#define	COLOR_WIN_P_LINE3				0xF89A
#define	COLOR_WIN_BOX_BG				0xF89B	// EPG Find Box


// COLOR_CURSOR_CELL
#define	COLOR_CN_FRAME1				0xF970	// Cursor OUT LINE
#define	COLOR_CN_FRAME2				0xFDA0	// Cursor OUT LINE
#define	COLOR_CH_FRAME1				0xF775
#define	COLOR_CH_FRAME2				0xFA96
#define	COLOR_CS_FRAME1				0xF789
#define	COLOR_CS_FRAME2				0xF9AB
#define	COLOR_CBTN_FRAME				0xF530
#define	COLOR_C2LINE					0xFCB6	// 2Line cursor Áß¾Ó
#define	COLOR_C2LINE_S					0xFABB	// 2Line cursor Áß¾Ó
#define	COLOR_C2LINE_H					0xF986	// 2Line cursor Áß¾Ó
#define	COLOR_C_BG						0xF788	// EPG Cell BG

#define	COLOR_C_LINE1					0xFBBB	// EPG CH Cell Line
#define	COLOR_C_LINE2					0xF899
#define	COLOR_C_LINE3					0xF788
#define	COLOR_C_LINE4					0xFBCC	// EPG Program Cell Line
#define	COLOR_C_LINE5					0xF8AB
#define	COLOR_C_LINE6					0xF78A
#define	COLOR_C_TIME					0xF578	// EPG Time Bar
#define	COLOR_C_BLUE_BG1				0xF346	// AV Blocking Blue
#define	COLOR_C_BLUE_BG2				0xF235
#define	COLOR_C_BLUE_BG3				0xF224
#define	COLOR_C_BLUE_BG4				0xF124


// COLOR_BUTTON
#define	COLOR_BTN_KEY_ST1				0xF669	// Keyboard Standard Button
#define	COLOR_BTN_KEY_ST2				0xF457
#define	COLOR_BTN_KEY_ST3				0xF558

#define	COLOR_BTN_KEY_EX1				0xF569	// Keyboard Extrended Button
#define	COLOR_BTN_KEY_EX2				0xF458
#define	COLOR_BTN_KEY_EX3				0xF458

#define	COLOR_BTN_KEY_NU1				0xF568	// keyboard Numeric Button
#define	COLOR_BTN_KEY_NU2				0xF457
#define	COLOR_BTN_KEY_NU3				0xF457

#define	COLOR_BTN_KEY_DI1				0xF557	// keyboard disable Button
#define	COLOR_BTN_KEY_DI2				0xF346
#define	COLOR_BTN_KEY_DI3				0xF446

#define	COLOR_BTN_KEY_BTN1			0xF457	// keyboard Special Button
#define	COLOR_BTN_KEY_BTN2			0xF236
#define	COLOR_BTN_KEY_BTN3			0xF235
#define	COLOR_BTN_KEY_BTN4			0xF124

#define	COLOR_BTN_POP_NO_F			0xF56A	// Popup Normal Button
#define	COLOR_BTN_POP_NO				0xF68B	// Popup Normal Button
#define	COLOR_BTN_POP_DI				0xF789	// Popup Normal Button


// Progress Bar
#define	COLOR_PBAR_MOVE_CURSOR		0xF000	// Moving Cursor
#define	COLOR_PBAR_CUR_CURSOR1		0xFDA2	// Current Cursor
#define	COLOR_PBAR_CUR_CURSOR2		0xFA62	// Current Cursor
#define	COLOR_PBAR_LINE				0xF566
#define	COLOR_PBAR_TIMEBOX			0xF222
#define	COLOR_PBAR_PIG1				0xFCDD	// PIG Frame(EPG)
#define	COLOR_PBAR_HDD					0xF448	// Bar Frame
#define	COLOR_PBAR_VCELL				0xF75A
#define	COLOR_PBAR_REDVIOLET			0xF609	// Media New Recording

#define	COLOR_PBAR_REC1				0xFF9A	// Record Progress Bar
#define	COLOR_PBAR_REC2				0xFE89
#define	COLOR_PBAR_REC3				0xFE78
#define	COLOR_PBAR_REC4				0xFD77
#define	COLOR_PBAR_REC5				0xFD66
#define	COLOR_PBAR_REC6				0xFC56
#define	COLOR_PBAR_REC7				0xFC45
#define	COLOR_PBAR_REC8				0xFB45

#define	COLOR_PBAR_TSR1				0xFACF
#define	COLOR_PBAR_TSR2				0xF9CE
#define	COLOR_PBAR_TSR3				0xF8BE
#define	COLOR_PBAR_TSR4				0xF7AE
#define	COLOR_PBAR_TSR5				0xF69D
#define	COLOR_PBAR_TSR6				0xF58D
#define	COLOR_PBAR_TSR7				0xF48C
#define	COLOR_PBAR_TSR8				0xF47B

#define	COLOR_PBAR_PLAYBACK1			0xFBDA
#define	COLOR_PBAR_PLAYBACK2			0xFAC9
#define	COLOR_PBAR_PLAYBACK3			0xFAC8
#define	COLOR_PBAR_PLAYBACK4			0xF9B7
#define	COLOR_PBAR_PLAYBACK5			0xF7A6
#define	COLOR_PBAR_PLAYBACK6			0xF6A5
#define	COLOR_PBAR_PLAYBACK7			0xF694
#define	COLOR_PBAR_PLAYBACK8			0xF593

#define	COLOR_PBAR_EDIT1				0xF8A7
#define	COLOR_PBAR_EDIT2				0xF797
#define	COLOR_PBAR_EDIT3				0xF695
#define	COLOR_PBAR_EDIT4				0xF584
#define	COLOR_PBAR_EDIT5				0xF473
#define	COLOR_PBAR_EDIT6				0xF371
#define	COLOR_PBAR_EDIT7				0xF260
#define	COLOR_PBAR_EDIT8				0xF360

// Message window
#define COLOR_MSG_WIN_0			0xF560
#define COLOR_MSG_WIN_1			0xF780;
#define COLOR_MSG_WIN_2			0xF9a0;
#define COLOR_MSG_WIN_3			0xFab0;
#define COLOR_MSG_WIN_4			0xFbc0;

#define COLOR_ALPHA_FULL			0xF000
#define COLOR_ALPHA_1_4				0x4000

#define COLOR_MASK_COLOR			0x0FFF
#define COLOR_MASK_ALPHA			0xF000

#define ARGB_VAL(r,g,b)   ( COLOR_ALPHA_FULL | ((r&0xf)<<8) | ((g&0xf)<<4) | ((b&0xf)) )

typedef enum eOSD_COLOR {
	OSD_COLOR_BACKGROUND1 = ARGB_VAL(1, 2, 4),
	OSD_COLOR_BACKGROUND2 = ARGB_VAL(1, 1, 4),
	OSD_COLOR_BOX = ARGB_VAL(1, 2, 4),
	
	OSD_COLOR_BOX_LINE  = ARGB_VAL(3, 5, 7),
	OSD_COLOR_BOX_LINE1 = ARGB_VAL(0, 0, 0),
	OSD_COLOR_BOX_LINE2 = ARGB_VAL(1, 1, 3),
	OSD_COLOR_BOX_LINE3 = ARGB_VAL(4, 5, 6),
	OSD_COLOR_BOX_LINE4 = ARGB_VAL(6, 6, 8),
	
	OSD_COLOR_SIGNAL_ORANGE  = ARGB_VAL(12,10,4),
	OSD_COLOR_SIGNAL_GRAY    = ARGB_VAL(6,7,7),
	OSD_COLOR_SIGNAL_LINE    = ARGB_VAL(1,1,2),

	OSD_COLOR_TEXT_ORANGE = ARGB_VAL(14,8,0) ,
	OSD_COLOR_TEXT_GRAY = ARGB_VAL(9,9,9),
	OSD_COLOR_TEXT_ERROR = ARGB_VAL(11,9,1),	
	OSD_COLOR_TEXT_BLUE = ARGB_VAL(4,5,8),

	OSD_COLOR_3RD_DARK = ARGB_VAL(0, 0, 1),
	OSD_COLOR_3RD_FILL = ARGB_VAL(1, 2, 3),
	OSD_COLOR_3RD_LIGHT = ARGB_VAL(2, 3, 4),
	OSD_COLOR_BAR_FILL  = ARGB_VAL(5,5,5)
} OSD_COLOR;

typedef enum
{
	LEFT_ALIGNED,
	CENTER_ALIGNED,
	RIGHT_ALIGNED
}ALIGN_MODE;

typedef unsigned short OSD_COLOR_VAL_t;
typedef unsigned short *OSD_BUFFER_ADDR_t;

void LD_OSD_LoaderDebugText(unsigned char* message);

#endif

