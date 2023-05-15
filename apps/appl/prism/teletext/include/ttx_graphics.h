
/**
	@file     ttx_graphics.h
	@brief   Rendering ���� ����

	Module: Teletext
*/
#ifndef ____TTX_GRAPHICS_H_
#define ____TTX_GRAPHICS_H_
/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <teletext.h>
/*******************************************************************/
/********************      Defines         			*************************/
/*******************************************************************/
/*******************************************************************/
/********************      Typedef        		 	 *************************/
/*******************************************************************/

/*******************************************************************/
/********************      API Functions   		*************************/
/*******************************************************************/
#define ____________API_FUNCTIONS_____________
/*
 * �Ʒ��� �Լ����� �÷��� �������� �Լ���� ��� �����Ǿ�� ���ڴ��� �����Ѵ�.
 * �ܺο��� �� ��������� include���� ���ƾ� �ϸ�, �Լ� ���� ����ؼ��� �ȵȴ�.
 */


/**
 * Teletext Graphics�� ����
 *
 * @param  hDecoder	TTX_Decoder_t handle
 * @param  hDrcs	TTX_Drcs_t handle
 * @return ������ Graphics �ڵ�
 */

extern HERROR TTX_GFX_Create (ONDKWindowHandle hwindow,Handle_t hDrcs);
/**
 * Teletext Graphics�� �Ҹ�
 *
 * @param	hGraphics	TTX GFX �ڵ�
 */

extern void   TTX_GFX_Destroy (void);

/**
 * Teletext Page�� ȭ�鼭 ����� ������
 *
 * @param 	hGraphics	TTX GFX �ڵ�
 * @param 	pPage		TTX Page
 * @param 	va			�������� ����Ʈ (TTX_RenderingPage()�� ������ ...���� ������ ���ڵ�)
 * @return	ERR_TTX_OK, ERR_TTX_INVAILD_ARGUMENT, ERR_TTX_FAIL
 */
extern TTX_Result_t 	TTX_GFX_RenderingPage 	(TTX_Page_t *pPage, va_list va);


#endif

