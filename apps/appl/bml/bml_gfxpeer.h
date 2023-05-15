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

#define SLIM_E_DTV_GENERIC            -1 /* == eDtv_GenericError. ��ʃG��? */
#define SLIM_E_DTV_NOTSUPPORTED       -2 /* �T??�g�O */
#define SLIM_E_DTV_ILLEGALPARAMETER   -3 /* �p����??��� */
#define SLIM_E_DTV_INBUSY             -4 /* �b�� */
#define SLIM_E_DTV_TIMEDOUT           -5 /* ?�C?�A�E�g */
#define SLIM_E_DTV_NODIALTONE         -6 /* ?�C�A���g?�������o����Ȃ� */
#define SLIM_E_DTV_NOCARRIER          -7 /* �L�����A�����o����Ȃ� */
#define SLIM_E_DTV_CUTOFF             -8 /* �����ؒf���ꂽ */
#define SLIM_E_DTV_NOTREADY           -9 /* �܂�?��s�� */
#define SLIM_E_DTV_NOSUCHFILE         -10 /* �t?�C����������Ȃ� */
#define SLIM_E_DTV_NOSUCHSOUND        -11 /* �w�肳�ꂽ������������Ȃ� */
#define SLIM_E_DTV_UNRESOLVED         -12 /* �����ł��Ȃ��z�X�g�� */
#define SLIM_E_DTV_WOULDBLOCK         -13 /* �x������Ă��� */
#define SLIM_E_DTV_UNKNOWNTYPE        -14 /* �T??�g���Ă��Ȃ�? */
#define SLIM_E_DTV_MEMORYERROR        -15 /* ���炩�̃������G��? */
#define SLIM_E_DTV_IOERROR            -16 /* ���炩��I/O�G��? */
#define SLIM_E_DTV_NOTFOUND           -17 /* �Ώۂ�������Ȃ����� */
#define SLIM_E_DTV_INVALIDKEY         -18 /* IC�J?�h�������ȂǃL?��������Ȃ� */
#define SLIM_E_DTV_TOOMUCHRESOURCE    -19 /* �d�l��̐����l���z���� */
#define SLIM_E_DTV_MODEMOCCUPIED      -20 /* ���f?�g�p�� */
#define SLIM_E_DTV_INVALIDTYPE        -21 /* ?���قȂ� */
#define SLIM_E_DTV_ABORTBYEVENT       -22 /* �O�������݂ɂ�钆�f */
/* -23 */
#define SLIM_E_DTV_SHORTAGE           -24 /* �s�� */
#define SLIM_E_DTV_AUTOCONNECTFAILED  -25 /* �����ڑ����̎��s */
#define SLIM_E_DTV_DNSFAILED          -26 /* DNS���O�ϊ����̎��s */
#define SLIM_E_DTV_TLSFAILED          -27 /* TLS�������̎��s */
#define SLIM_E_DTV_ALREADYCONNECTED   -28 /* �ڑ����m���ς݂ł��� */
#define SLIM_E_DTV_AUTOCONNECTED      -29 /* �����ڑ����ꂽ�Ăł��� */
#define SLIM_E_DTV_NOTCONNECTED       -30 /* �Ă��m�����Ă��Ȃ� */
#define SLIM_E_DTV_OUTOFAREA          -31 /* �l�b�g��?�N�T?�r�X�̌��O */
#define SLIM_E_DTV_NOMEMORY           -32 /* �̈�s�� */
#define SLIM_E_DTV_USERCANCEL         -33 /* ��?�U�ɂ��L�����Z�� */
#define SLIM_E_DTV_UNMATCHED          -34 /* ���ƎҎ��ʏ��̕s��v */
#define SLIM_E_DTV_IOSLEEP            -35 /* �x������Ă��� (�X��?�v��?) */
#define SLIM_E_DTV_TOOMUCHCOMPONENT   -36 /* �R��??�l���g���������� */
#define SLIM_E_DTV_SERVICEABORTED     -40 /* sendTextMail, sendMIMEMail�p */
#define SLIM_E_DTV_UNINITIALIZED      -41

#define SLIM_E_DTV_UNEXPECTED         -9999 /* ���̑�?�����Ȃ��G��? */



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
* �׷��ȿ� ���� �ʱ�ȭ�� ����.
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
* �׷��� ���� ó���� ��.
*
*******************************************************************************/
extern void dtvGraphic_FinalizePeer(void);


/*******************************************************************************
* function	dtvGraphic_GetCapsPeer
* input		None
* output	out_caps : �Ӽ��� ������ ����ü
* return	SLIM_E_DTV_OK
*			SLIM_E_DTV_GENERIC
*
* description
*
* �Ӽ� ���� (align)
*
*******************************************************************************/
extern slim_int dtvGraphic_GetCapsPeer(TDTVGraphicCaps *out_caps);


/*******************************************************************************
* function	dtvGraphic_SetViewModePeer
* input		in_mode : Į�� ���
*			in_width : screen�� ��
*			in_height : screen�� ����
*			in_ratio : aspect ratio
* output	None
* return	SLIM_E_DTV_OK
*			SLIM_E_DTV_GENERIC
*			SLIM_E_DTV_ILLEGALPARAMETER
*
* description
*
*	<�ػ� ����>
*	- FULL VIDEO : video�� �ػ󵵸� ���� ��, �� �Լ��� ȣ���� �ػ󵵿� ������ �־�� �ȵ�.
*	- PIG VIDEO : BML �ػ󵵰� �����. BML�� ���� PIG ������ dtvVideoStream_UpdatePeer()���� ��.
* 	- NONE VIDEO : video�� �����Ƿ� BML �ػ󵵰� �����.
*
*******************************************************************************/
extern slim_int dtvGraphic_SetViewModePeer(slim_int in_mode,
                           slim_int in_width,
                           slim_int in_height,
                           slim_int in_ratio);


/*******************************************************************************
* function	dtvGraphic_SetCLUTPeer
* input		in_table : aYCbCr Į�� ����Ʈ
*			in_start : ������ index ���� ��ġ
*			in_end : ������ index ���� ��ġ
* output	None
* return	SLIM_E_DTV_OK
*			SLIM_E_DTV_GENERIC
*			SLIM_E_DTV_ILLEGALPARAMETER
*
* description
*
* BML Plane�� YCbCr �����̶�� NetFront�κ��� �Ѿ�� in_table ���� �ٷ� �����ϸ� ������ ���� RGB
* �����̶�� conversion ó���� �ʿ���. (YCbCr2RGB)
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
* input		in_x : ��ȭ ���� x ��ǥ
*			in_y : ��ȭ ���� y ��ǥ
*			in_text : ��ȭ�� ���ڿ� ������ (EUC-JP)
*			in_len : ���ڿ� ����
*			in_color : ���ڻ� (4���� ��Ʈ)
* 			in_font_family : ��Ʈ ���� (�ձ� ���, �� ���, ���� �ձ� ���)
* 			in_font_size : ��Ʈ ũ��
*			in_letter_spacing : �ڰ�
* output	None
* return	SLIM_E_DTV_OK
*			SLIM_E_DTV_GENERIC
*			SLIM_E_DTV_ILLEGALPARAMETER
*
* description
*
* browserApp���� BML stop�� �޽����� ���� NetFront�� �����ϹǷ� sync ������ ���� �� �ִ�.
* browserApp���� BML stop�� s_nGraphicSem�� take �����ν� ��� drawing �Ǵ� ���� ���´�.
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
* input		in_rect : ä�� ����
*			in_color : ä�� �� �ε��� ��
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
* input		in_rect : �����ϰ� ä�� ����
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
* input		in_rect : ������Ʈ�� ����
* output	None
* return	SLIM_E_DTV_OK
*			SLIM_E_DTV_GENERIC
*			SLIM_E_DTV_ILLEGALPARAMETER
*
* description
*
* ���� ȭ�鿡 �׸� BML Plane �׷��� �����͸� ���� OSD�� ������.
*
*******************************************************************************/
extern slim_int	dtvGraphic_ExposeRectPeer(ONDK_Rect_t* in_rect);


/*******************************************************************************
* function	dtvGraphic_BitBltPeer
* input		in_x : destination x ��ǥ
*			in_y : destination y ��ǥ
* 			in_rect : source ��ǥ
*			in_pixmap : source ������
*			in_pixmap_width : source �������� ��
*			in_pixmap_height : source �������� ����
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
* input		in_status : video ���¸� ����
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
* input		in_plane : ��� �÷���
*			in_rect : �κ��÷�����ǥ
*			in_num : �κ��÷��� ��(in_rect �迭 ��)
* output	None
* return	SLIM_E_DTV_OK
*			SLIM_E_DTV_GENERIC
*
* description
*
* VIDEO + in_num (0) : ��ȭ�� ��ȭ
* STILL + in_num (0) : ��ȭ�� ����ȭ
* STILL + in_num (1) : PIG
* VIDEO + in_num (n) : ��ȭȭ�鿡 n ������ rectangle(����ȭ��)�� ����.
*
*******************************************************************************/
extern slim_int	dtvSwitchingPlane_SetPeer(slim_int in_plane,
				                          ONDK_Rect_t* in_rect,
				                          slim_int in_num);

/*******************************************************************************
* function	dtvStillPlane_BitBltPeer
* input	 	in_dest_x : destination x ��ǥ
*			in_dest_y : destination y ��ǥ
*			in_rect : source �ȼ������� ��ǥ
*			in_pixmap : �ȼ�������
* 			in_pixmap_width : �ȼ������� ��
*			in_pixmap_height : �ȼ������� ����
*			in_rowbytes : 1���� ����Ʈ ��
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
* input	 	in_dest_x : destination x ��ǥ(ex. 960x540 ����)
*			in_dest_y : destination y ��ǥ(ex. 960x540 ����)
*			in_rect : source �ȼ������� ��ǥ	(ex. 1920x1080 ����)
*			in_pixmap : �ȼ�������		(1920x1080�� ������)
* 			in_pixmap_width : �ȼ������� ��	(1920 - �����δ� ���� ��)
*			in_pixmap_height : �ȼ������� ����(1080 - �����δ� ���� �� )
*			in_rowbytes : 1���� ����Ʈ ��	(1920*4 - 1920�ȼ� * 32bpp)
* output	None
* return	SLIM_E_DTV_OK
*			SLIM_E_DTV_GENERIC
*			SLIM_E_DTV_ILLEGALPARAMETER
*
* description :   BML �� Still Plane �� 1920x1080 �϶�,  
*			HUMAX 5-Plane ������ �ش��ϴ� 960x540 ���� 
*			Copy �ϱ� ���� �Լ� �Դϴ�. 
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
* input	 	in_rect : �� ä�� ��ǥ
*			in_color : ��
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
* input	 	in_status : ��ȭ ǥ�� ����
*			in_rect : ��ȭ����
*			in_component : component_tag
*			in_mime : ��ȭ Ÿ��
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
* input	 	in_data : ���� ���� ������ ������
*��			in_data_len : ���� ���� ������ ����
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
* input	 	in_data : 8���� ���� ��ȣ ������
*			in_data_len : 8���� ���� ��ȣ ������ ����
*			in_width : 8���� ���ں�ȣ�� ǥ���ϴ� ���� (��)
*			in_height : 8���� ���ں�ȣ�� ǥ���ϴ� ������ ����
* output	None
* return	>= 0 8���� ���� ��ȣ�� ��ũ����.
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
* input	 	in_id : ��ũ����
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
* input	 	in_x : x��ǥ
*			in_y : y��ǥ
*			in_clip_rect : Ŭ���� ����
*			in_id : descriptor
* output	None
* return	SLIM_E_DTV_OK
*			SLIM_E_DTV_GENERIC
*			SLIM_E_DTV_ILLEGALPARAMETER
*
* description
*
* drawJIS8() �� ACCESS�κ��� ���� �ҽ� ��.
*
*******************************************************************************/
extern slim_int	dtv8Text_DrawPeer(slim_int in_x, slim_int in_y,
                  ONDK_Rect_t* in_clip_rect,
                  slim_int in_id);



#endif	//__BML_GFXPEER_H__

