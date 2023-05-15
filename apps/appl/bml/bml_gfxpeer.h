/*
   @file aribcc_draw.c
   @brief MW ARIB CLOSE CAPTION AND SUPER  drawing
   @section copyright	Copyright(c) 2010, Humax Co., Ltd. All rights reserved.
   @section module MW/ARIBCC
   @author ccash@humaxdigital.com
   @remark
*/

#ifndef	__BML_GFXPEER_H__
#define	__BML_GFXPEER_H__

#include <stdio.h>
//#include <util_png.h>
#include "directfb.h"
#include "ondk.h"



#define 	JAPANESE_FONT_PATH 	"/usr/fonts/JP-ARIB-JGTR01B.ttf"

/**
 * Error
 */
#define SLIM_E_DTV_OK                  0 /* == eDtv_Success */

#define SLIM_E_DTV_GENERIC            -1 /* == eDtv_GenericError. 덇붗긄깋? */
#define SLIM_E_DTV_NOTSUPPORTED       -2 /* 긖??긣둖 */
#define SLIM_E_DTV_ILLEGALPARAMETER   -3 /* 긬깋긽??뚮귟 */
#define SLIM_E_DTV_INBUSY             -4 /* 쁞뭷 */
#define SLIM_E_DTV_TIMEDOUT           -5 /* ?귽?귺긂긣 */
#define SLIM_E_DTV_NODIALTONE         -6 /* ?귽귺깑긣?깛궕뙚뢯궠귢궶궋 */
#define SLIM_E_DTV_NOCARRIER          -7 /* 긌긿깏귺궕뙚뢯궠귢궶궋 */
#define SLIM_E_DTV_CUTOFF             -8 /* 떗맕먛뭚궠귢궫 */
#define SLIM_E_DTV_NOTREADY           -9 /* 귏궬?띿븉됀 */
#define SLIM_E_DTV_NOSUCHFILE         -10 /* 긲?귽깑궕뙥궰궔귞궶궋 */
#define SLIM_E_DTV_NOSUCHSOUND        -11 /* 럚믦궠귢궫돶맳궕뙥궰궔귞궶궋 */
#define SLIM_E_DTV_UNRESOLVED         -12 /* 됶뙂궳궖궶궋긼긚긣뼹 */
#define SLIM_E_DTV_WOULDBLOCK         -13 /* 뭯뎱궠귢궲궋귡 */
#define SLIM_E_DTV_UNKNOWNTYPE        -14 /* 긖??긣궢궲궋궶궋? */
#define SLIM_E_DTV_MEMORYERROR        -15 /* 돺귞궔궻긽긾깏긄깋? */
#define SLIM_E_DTV_IOERROR            -16 /* 돺귞궔궻I/O긄깋? */
#define SLIM_E_DTV_NOTFOUND           -17 /* 뫮뤭궕뙥궰궔귞궶궔궯궫 */
#define SLIM_E_DTV_INVALIDKEY         -18 /* IC긇?긤궕뼰궋궶궵긌?궕뙥궰궔귞궶궋 */
#define SLIM_E_DTV_TOOMUCHRESOURCE    -19 /* 럅뾩뤵궻맕뙽뭠귩뎭궑궫 */
#define SLIM_E_DTV_MODEMOCCUPIED      -20 /* 긾긢?럊뾭뭷 */
#define SLIM_E_DTV_INVALIDTYPE        -21 /* ?궕댶궶귡 */
#define SLIM_E_DTV_ABORTBYEVENT       -22 /* 둖븫뒆뜛귒궸귝귡뭷뭚 */
/* -23 */
#define SLIM_E_DTV_SHORTAGE           -24 /* 븉뫉 */
#define SLIM_E_DTV_AUTOCONNECTFAILED  -25 /* 렔벍먝뫏렄궻렪봲 */
#define SLIM_E_DTV_DNSFAILED          -26 /* DNS뼹멟빾듂렄궻렪봲 */
#define SLIM_E_DTV_TLSFAILED          -27 /* TLS룉뿚렄궻렪봲 */
#define SLIM_E_DTV_ALREADYCONNECTED   -28 /* 먝뫏궕둴뿧띙귒궳궇귡 */
#define SLIM_E_DTV_AUTOCONNECTED      -29 /* 렔벍먝뫏궠귢궫뚁궳궇귡 */
#define SLIM_E_DTV_NOTCONNECTED       -30 /* 뚁궕둴뿧궢궲궋궶궋 */
#define SLIM_E_DTV_OUTOFAREA          -31 /* 긨긞긣깗?긏긖?긮긚궻뙒둖 */
#define SLIM_E_DTV_NOMEMORY           -32 /* 쀌덃븉뫉 */
#define SLIM_E_DTV_USERCANCEL         -33 /* 깇?긗궸귝귡긌긿깛긜깑 */
#define SLIM_E_DTV_UNMATCHED          -34 /* 럷떾롌렞빶륃뺪궻븉덇뭭 */
#define SLIM_E_DTV_IOSLEEP            -35 /* 뭯뎱궠귢궲궋귡 (긚깏?긵됀?) */
#define SLIM_E_DTV_TOOMUCHCOMPONENT   -36 /* 긓깛??긨깛긣궕뫝궥궗귡 */
#define SLIM_E_DTV_SERVICEABORTED     -40 /* sendTextMail, sendMIMEMail뾭 */
#define SLIM_E_DTV_UNINITIALIZED      -41

#define SLIM_E_DTV_UNEXPECTED         -9999 /* 궩궻뫜?딖궢궶궋긄깋? */



/* Event manager ------------------------------------------------------------*/

typedef char			slim_tiny;
typedef short int		slim_short;
typedef int			slim_int;
typedef unsigned int		slim_bool;
typedef HUINT8	slim_byte;
typedef HUINT8	slim_char;
typedef unsigned short	slim_half;
typedef unsigned int		slim_word;
typedef double			slim_double;
typedef int			slim_opaque;
typedef int			slim_fixed_point;

typedef signed char		slim_schar;



typedef struct TDTVGraphicCaps {
    slim_int fPixmapAlign;
    slim_int fBitmapAlign;
    slim_int fYUVImageAlign;
} TDTVGraphicCaps;


#define ___PEER_APIs___

extern	int 	g_nSkipUpdate;


/*******************************************************************************
* function	dtvGraphic_InitializePeer
* input		None
* output	None
* return	SLIM_E_DTV_OK Succeeded
* 			SLIM_E_DTV_GENERIC Not succeeded
*			SLIM_E_DTV_NOTSUPPORTED
*
* description
*
* 그래픽에 관한 초기화를 행함.
*
*******************************************************************************/
extern slim_int dtvGraphic_InitializePeer(void);


/*******************************************************************************
* function	dtvGraphic_InitializePeer
* input		None
* output	None
* return	None
*
* description
*
* 그래픽 종료 처리를 함.
*
*******************************************************************************/
extern void dtvGraphic_FinalizePeer(void);


/*******************************************************************************
* function	dtvGraphic_GetCapsPeer
* input		None
* output	out_caps : 속성을 대입할 구조체
* return	SLIM_E_DTV_OK
*			SLIM_E_DTV_GENERIC
*
* description
*
* 속성 설정 (align)
*
*******************************************************************************/
extern slim_int dtvGraphic_GetCapsPeer(TDTVGraphicCaps *out_caps);


/*******************************************************************************
* function	dtvGraphic_SetViewModePeer
* input		in_mode : 칼라 모드
*			in_width : screen의 폭
*			in_height : screen의 높이
*			in_ratio : aspect ratio
* output	None
* return	SLIM_E_DTV_OK
*			SLIM_E_DTV_GENERIC
*			SLIM_E_DTV_ILLEGALPARAMETER
*
* description
*
*	<해상도 적용>
*	- FULL VIDEO : video의 해상도를 적용 함, 이 함수의 호출이 해상도에 영향을 주어서는 안됨.
*	- PIG VIDEO : BML 해상도가 적용됨. BML에 의한 PIG 적용은 dtvVideoStream_UpdatePeer()에서 함.
* 	- NONE VIDEO : video가 없으므로 BML 해상도가 적용됨.
*
*******************************************************************************/
extern slim_int dtvGraphic_SetViewModePeer(slim_int in_mode,
                           slim_int in_width,
                           slim_int in_height,
                           slim_int in_ratio);


/*******************************************************************************
* function	dtvGraphic_SetCLUTPeer
* input		in_table : aYCbCr 칼라 빨레트
*			in_start : 설정할 index 개시 위치
*			in_end : 설정할 index 종료 위치
* output	None
* return	SLIM_E_DTV_OK
*			SLIM_E_DTV_GENERIC
*			SLIM_E_DTV_ILLEGALPARAMETER
*
* description
*
* BML Plane이 YCbCr 포멧이라면 NetFront로부터 넘어온 in_table 값을 바로 적용하면 되지만 만약 RGB
* 포멧이라면 conversion 처리가 필요함. (YCbCr2RGB)
*
*******************************************************************************/
extern slim_int dtvGraphic_SetCLUTPeer(slim_word* in_table,
                       slim_int in_start,
                       slim_int in_end);


/*******************************************************************************
* function	dtvGraphic_ExposeCLUTPeer
* input		None
* output	None
* return	None
*
* description
*
*******************************************************************************/
extern void	dtvGraphic_ExposeCLUTPeer(void);


/*******************************************************************************
* function	dtvGraphic_DrawTextPeer
* input		in_x : 묘화 개시 x 좌표
*			in_y : 묘화 개시 y 좌표
*			in_text : 묘화할 문자열 포인터 (EUC-JP)
*			in_len : 문자열 길이
*			in_color : 문자색 (4계조 폰트)
* 			in_font_family : 폰트 종류 (둥근 고딕, 각 고딕, 굵은 둥근 고딕)
* 			in_font_size : 폰트 크기
*			in_letter_spacing : 자간
* output	None
* return	SLIM_E_DTV_OK
*			SLIM_E_DTV_GENERIC
*			SLIM_E_DTV_ILLEGALPARAMETER
*
* description
*
* browserApp에서 BML stop을 메시지를 통해 NetFront에 통지하므로 sync 문제가 있을 수 있다.
* browserApp에서 BML stop시 s_nGraphicSem를 take 함으로써 계속 drawing 되는 것을 막는다.
*
*******************************************************************************/
extern slim_int	dtvGraphic_DrawTextPeer(slim_int in_x,
                        slim_int in_y,
                        slim_char* in_text,
                        slim_int in_len,
                        ONDK_Color_t* in_colors,
                        slim_int in_font_family,
                        slim_int in_font_size,
                        slim_int in_font_style,
                        ONDK_Rect_t* in_clip_rect,
                        slim_int in_letter_spacing);


/*******************************************************************************
* function	dtvGraphic_FillRectPeer
* input		in_rect : 채울 영역
*			in_color : 채울 색 인덱스 값
* output	None
* return	SLIM_E_DTV_OK
*			SLIM_E_DTV_GENERIC
*
* description
*
*******************************************************************************/
extern slim_int	dtvGraphic_FillRectPeer(ONDK_Rect_t* in_rect,
                        				HUINT8 in_color);


/*******************************************************************************
* function	dtvGraphic_FillTransparenMRectPeer
* input		in_rect : 투명하게 채울 영역
* output	None
* return	SLIM_E_DTV_OK
*			SLIM_E_DTV_GENERIC
*			SLIM_E_DTV_ILLEGALPARAMETER
*
* description
*
*******************************************************************************/
extern slim_int	dtvGraphic_FillTransparenMRectPeer(ONDK_Rect_t* in_rect);


/*******************************************************************************
* function	dtvGraphic_ExposeRectPeer
* input		in_rect : 업데이트할 영역
* output	None
* return	SLIM_E_DTV_OK
*			SLIM_E_DTV_GENERIC
*			SLIM_E_DTV_ILLEGALPARAMETER
*
* description
*
* 가상 화면에 그린 BML Plane 그래픽 데이터를 실제 OSD에 적용함.
*
*******************************************************************************/
extern slim_int	dtvGraphic_ExposeRectPeer(ONDK_Rect_t* in_rect);


/*******************************************************************************
* function	dtvGraphic_BitBltPeer
* input		in_x : destination x 좌표
*			in_y : destination y 좌표
* 			in_rect : source 좌표
*			in_pixmap : source 데이터
*			in_pixmap_width : source 데이터의 폭
*			in_pixmap_height : source 데이터의 높이
* output	None
* return	SLIM_E_DTV_OK
*			SLIM_E_DTV_GENERIC
*			SLIM_E_DTV_ILLEGALPARAMETER
*
* description
*
*******************************************************************************/
extern slim_int	dtvGraphic_BitBltPeer(slim_int in_x,
                      slim_int in_y,
                      ONDK_Rect_t* in_rect,
                      slim_byte* in_pixmap,
                      slim_int in_pixmap_width,
                      slim_int in_pixmap_height);

/*******************************************************************************
* function	dtvGraphic_NotifyVideoStatusPeer
* input		in_status : video 상태를 통지
* output	None
* return	None
*
* description
*
*******************************************************************************/
extern void	dtvGraphic_NotifyVideoStatusPeer(slim_int in_status);


/*******************************************************************************
* function	dtvGraphic_BeginUpdatePeer
* input		None
* output	None
* return	None
*
* description
*
*******************************************************************************/
extern void	dtvGraphic_BeginUpdatePeer(void);


/*******************************************************************************
* function	dtvGraphic_EndUpdatePeer
* input		None
* output	None
* return	None
*
* description
*
*******************************************************************************/
extern void	dtvGraphic_EndUpdatePeer(void);

/*******************************************************************************
* function	dtvSwitchingPlane_SetPeer
* input		in_plane : 배경 플레인
*			in_rect : 부분플레인좌표
*			in_num : 부분플레인 수(in_rect 배열 수)
* output	None
* return	SLIM_E_DTV_OK
*			SLIM_E_DTV_GENERIC
*
* description
*
* VIDEO + in_num (0) : 전화면 동화
* STILL + in_num (0) : 전화면 정지화
* STILL + in_num (1) : PIG
* VIDEO + in_num (n) : 동화화면에 n 개수의 rectangle(정지화면)이 생김.
*
*******************************************************************************/
extern slim_int	dtvSwitchingPlane_SetPeer(slim_int in_plane,
				                          ONDK_Rect_t* in_rect,
				                          slim_int in_num);

/*******************************************************************************
* function	dtvStillPlane_BitBltPeer
* input	 	in_dest_x : destination x 좌표
*			in_dest_y : destination y 좌표
*			in_rect : source 픽셀데이터 좌표
*			in_pixmap : 픽셀데이터
* 			in_pixmap_width : 픽셀데이터 폭
*			in_pixmap_height : 픽셀데이터 높이
*			in_rowbytes : 1행의 바이트 수
* output	None
* return	SLIM_E_DTV_OK
*			SLIM_E_DTV_GENERIC
*			SLIM_E_DTV_ILLEGALPARAMETER
*
* description
*
*******************************************************************************/
extern slim_int	dtvStillPlane_BitBltPeer(slim_int in_dest_x, slim_int in_dest_y,
                         ONDK_Rect_t* in_rect,
                         slim_byte *in_pixelmap,	/* RGB */
                         slim_int in_pixelmap_width,
                         slim_int in_pixelmap_height,
                         slim_int in_rowBytes);

/*******************************************************************************
* function	dtvStillPlane_BitQuaterBltPeer
* input	 	in_dest_x : destination x 좌표(ex. 960x540 기준)
*			in_dest_y : destination y 좌표(ex. 960x540 기준)
*			in_rect : source 픽셀데이터 좌표	(ex. 1920x1080 기준)
*			in_pixmap : 픽셀데이터		(1920x1080의 포인터)
* 			in_pixmap_width : 픽셀데이터 폭	(1920 - 실제로는 무시 됨)
*			in_pixmap_height : 픽셀데이터 높이(1080 - 실제로는 무시 됨 )
*			in_rowbytes : 1행의 바이트 수	(1920*4 - 1920픽셀 * 32bpp)
* output	None
* return	SLIM_E_DTV_OK
*			SLIM_E_DTV_GENERIC
*			SLIM_E_DTV_ILLEGALPARAMETER
*
* description :   BML 의 Still Plane 이 1920x1080 일때,  
*			HUMAX 5-Plane 구조에 해당하는 960x540 으로 
*			Copy 하기 위한 함수 입니다. 
*
*******************************************************************************/
slim_int	dtvStillPlane_BitQuaterBltPeer(slim_int in_dest_x, slim_int in_dest_y,
				ONDK_Rect_t* in_rect,
				slim_byte *in_pixelmap,    /* RGB */
				slim_int in_pixelmap_width,
				slim_int in_pixelmap_height,
				slim_int in_rowBytes);

/*******************************************************************************
* function	dtvStillPlane_FillRectPeer
* input	 	in_rect : 색 채울 좌표
*			in_color : 색
* output	None
* return	SLIM_E_DTV_OK
*			SLIM_E_DTV_GENERIC
*			SLIM_E_DTV_ILLEGALPARAMETER
*
* description
*
*******************************************************************************/
extern slim_int	dtvStillPlane_FillRectPeer(ONDK_Rect_t* in_rect,
                         	  slim_word in_color);	/*ARGB*/


/*******************************************************************************
* function	dtvVideoStream_UpdatePeer
* input	 	in_status : 동화 표시 여부
*			in_rect : 동화영역
*			in_component : component_tag
*			in_mime : 동화 타입
* output	None
* return	SLIM_E_DTV_OK
*			SLIM_E_DTV_GENERIC
*			SLIM_E_DTV_ILLEGALPARAMETER
*
* description
*
*******************************************************************************/
extern slim_int	dtvVideoStream_UpdatePeer(slim_int in_status,
											ONDK_Rect_t* in_rect,
                        					slim_int in_component,
                        					slim_int in_mime);


/*******************************************************************************
* function	dtvDRCS_LoadPeer
* input	 	in_data : 외자 정의 데이터 포인터
*ㅂ			in_data_len : 외자 정의 데이터 길이
* output	None
* return	SLIM_E_DTV_OK
*			SLIM_E_DTV_GENERIC
*			SLIM_E_DTV_ILLEGALPARAMETER
*
* description
*
*******************************************************************************/
extern slim_int	dtvDRCS_LoadPeer(slim_byte *in_data,
                 					slim_word in_data_len);


/*******************************************************************************
* function	dtvDRCS_UnloadPeer
* input	 	None
* output	None
* return	SLIM_E_DTV_OK
*			SLIM_E_DTV_GENERIC
*			SLIM_E_DTV_ILLEGALPARAMETER
*
* description
*
*******************************************************************************/
extern slim_int	dtvDRCS_UnloadPeer(void);


/*******************************************************************************
* function	dtv8Text_CreatePeer
* input	 	in_data : 8단위 문자 부호 데이터
*			in_data_len : 8단위 문자 부호 데이터 길이
*			in_width : 8단위 문자부호를 표시하는 영역 (폭)
*			in_height : 8단위 문자부호를 표시하는 영역의 높이
* output	None
* return	>= 0 8단위 문자 부호의 디스크립터.
*			SLIM_E_DTV_GENERIC
*			SLIM_E_DTV_ILLEGALPARAMETER
*
* description
*
*******************************************************************************/
extern slim_int	dtv8Text_CreatePeer(slim_byte *in_data, slim_int in_data_len,
                    slim_int in_width, slim_int in_height);


/*******************************************************************************
* function	dtv8Text_DisposePeer
* input	 	in_id : 디스크립터
* output	None
* return	SLIM_E_DTV_OK
*			SLIM_E_DTV_GENERIC
*			SLIM_E_DTV_ILLEGALPARAMETER
*
* description
*
*******************************************************************************/
extern slim_int	dtv8Text_DisposePeer(slim_int in_id);


/*******************************************************************************
* function	dtv8Text_DrawPeer
* input	 	in_x : x좌표
*			in_y : y좌표
*			in_clip_rect : 클리핑 영역
*			in_id : descriptor
* output	None
* return	SLIM_E_DTV_OK
*			SLIM_E_DTV_GENERIC
*			SLIM_E_DTV_ILLEGALPARAMETER
*
* description
*
* drawJIS8() 은 ACCESS로부터 받은 소스 임.
*
*******************************************************************************/
extern slim_int	dtv8Text_DrawPeer(slim_int in_x, slim_int in_y,
                  ONDK_Rect_t* in_clip_rect,
                  slim_int in_id);



#endif	//__BML_GFXPEER_H__

