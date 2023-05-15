
/**
	@file     ttx_graphics.h
	@brief   Rendering 관련 진합

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
 * 아래의 함수들은 플랫폼 종속적인 함수들로 모두 구현되어야 디코더가 동작한다.
 * 외부에선 이 헤더파일을 include하지 말아야 하며, 함수 또한 사용해서는 안된다.
 */


/**
 * Teletext Graphics를 생성
 *
 * @param  hDecoder	TTX_Decoder_t handle
 * @param  hDrcs	TTX_Drcs_t handle
 * @return 생성된 Graphics 핸들
 */

extern HERROR TTX_GFX_Create (ONDKWindowHandle hwindow,Handle_t hDrcs);
/**
 * Teletext Graphics를 소멸
 *
 * @param	hGraphics	TTX GFX 핸들
 */

extern void   TTX_GFX_Destroy (void);

/**
 * Teletext Page를 화면서 실재로 렌더링
 *
 * @param 	hGraphics	TTX GFX 핸들
 * @param 	pPage		TTX Page
 * @param 	va			가변인자 리스트 (TTX_RenderingPage()의 마지막 ...으로 들어오는 인자들)
 * @return	ERR_TTX_OK, ERR_TTX_INVAILD_ARGUMENT, ERR_TTX_FAIL
 */
extern TTX_Result_t 	TTX_GFX_RenderingPage 	(TTX_Page_t *pPage, va_list va);


#endif

