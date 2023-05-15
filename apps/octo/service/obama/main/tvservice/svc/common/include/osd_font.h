/****************************************************************************
*
* File Name	: mw_font.h
*
* Description	:
*
*
 * Revision History	:
*
* Date			Modification				Name
* -----------	-------------------------------------	------------------
* 2006/11/ 05	File Created.				JK Baek
*
****************************************************************************/

#ifndef	__OSD_FONT_H__
#define	__OSD_FONT_H__

#include <osd.h>
#include <isosvc.h>


typedef enum tagFontDrawProperty
{
	eFDP_None 			= 0x00000000,

	eFDP_Bold 			= 0x01000000,
	eFDP_BoldSet		= 0x01000001,
	eFDP_BoldNone		= 0x0100000F,

	eFDP_Underline 		= 0x02000000,
	eFDP_Underline1pixel= 0x02000010,
	eFDP_Underline2pixel= 0x02000020,
	eFDP_Underline3pixel= 0x02000030,
	eFDP_Underline4pixel= 0x02000040,
	eFDP_UnderlineNone	= 0x020000F0,

	eFDP_Rotate	 		= 0x04000000,
	eFDP_RotateLeft90 	= 0x04000100,
	eFDP_RotateLeft180	= 0x04000200,
	eFDP_RotateLeft270	= 0x04000300,
	eFDP_RotateNone		= 0x04000F00,

	eFDP_Italic			= 0x08000000,
	eFDP_Italic6		= 0x08001000,
	eFDP_Italic12		= 0x08002000,
	eFDP_Italic18		= 0x08003000,
	eFDP_ItalicNone		= 0x0800F000,

	eFDP_Width			= 0x10000000, /**< 수치는 퍼센트 */
	eFDP_Width50		= 0x10010000,
	eFDP_Width80		= 0x10020000,
	eFDP_Width85		= 0x10030000,
	eFDP_Width90		= 0x10040000,
	eFDP_Width95		= 0x10050000,
	eFDP_Width100		= 0x10060000,
	eFDP_Width105		= 0x10070000,
	eFDP_Width110		= 0x10080000,
	eFDP_Width115		= 0x10090000,
	eFDP_Width120		= 0x100A0000,
	eFDP_Width200		= 0x100B0000,
	eFDP_WidthNone		= 0x100F0000,

	eFDP_Height			= 0x20000000, /**< 수치는 퍼센트 */
	eFDP_Height50		= 0x20100000,
	eFDP_Height80		= 0x20200000,
	eFDP_Height85		= 0x20300000,
	eFDP_Height90		= 0x20400000,
	eFDP_Height95		= 0x20500000,
	eFDP_Height100		= 0x20600000,
	eFDP_Height105		= 0x20700000,
	eFDP_Height110		= 0x20800000,
	eFDP_Height115		= 0x20900000,
	eFDP_Height120		= 0x20A00000,
	eFDP_Height200		= 0x20B00000,
	eFDP_HeightNone		= 0x20F00000,

	eFDP_Reset			= 0x80000000,

	eFDP_Max
} FontDrawStyle_t;

void					local_fontTest(OSD_Screen_t *pixmap);


extern void		OSD_FONT_Init(void);

/**
  @remark	Initialize GWM fonts. \n
  @remark	API Type : sync \n
  @return		void \n
*/
extern void 		OSD_FONT_InitSysFont(void);

/**
  @remark	Initialize GWM Ttx fonts. \n
  @remark	API Type : sync \n
  @return		void \n
*/
extern void			OSD_FONT_InitTtxFont(void);

/**
  @remark	get gwm Font structure. Font id is the key to identify a font family instance.\n
  @remark	API Type : sync \n
  @return		OSD_Font_t *\n
*/
extern OSD_Font_t *	OSD_FONT_Get(OSD_FontId_t id);

extern OSD_Font_t	*OSD_FONT_GetFont(OSD_FontId_t id, HUINT32 size, HUINT32 percent);

/**
  @remark	release gwm Font structure.\n
  @remark	API Type : sync \n
  @return		HINT32, reference count of that font ID indicated\n
*/
extern OSD_Result_t		OSD_FONT_Release(OSD_Font_t *pOsdFont);

/**
  @remark	Set font width Ratio .\n
  @remark	API Type : sync \n
  @return		HERROR, results\n
*/
extern OSD_Result_t		OSD_FONT_SetWidthRatio(OSD_Font_t *font, HUINT32 percent);

/**
  @remark	Get font width Ratio .\n
  @remark	API Type : sync \n
  @return		HERROR, results\n
*/
extern OSD_Result_t		OSD_FONT_GetWidthRatio(OSD_Font_t *font, HUINT32 *percent);

/**
  @remark	set font size specified.\n
  @remark	API Type : sync \n
  @return		OSD_Result_t, results\n
*/
extern OSD_Result_t	OSD_FONT_SetSize(OSD_Font_t *font, HUINT32 size);

/**
  @remark	get font size specified.\n
  @remark	API Type : sync \n
  @return		HERROR, results\n
*/

extern OSD_Result_t		OSD_FONT_GetSize(OSD_Font_t *font, HUINT32 *size);
/**
  @remark	get font ascender.\n
  @remark	API Type : sync \n
  @return		HERROR, results\n
*/
extern OSD_Result_t		OSD_FONT_GetAscender(OSD_Font_t *font, HUINT32 *pAscender);

/**
  @remark	get font descender.\n
  @remark	API Type : sync \n
  @return		HERROR, results\n
*/
extern OSD_Result_t		OSD_FONT_GetDescender(OSD_Font_t *font, HUINT32 *pDescender);

/**
  @remark	get font real height (ascender+descender).\n
  @remark	API Type : sync \n
  @return		HERROR, results\n
*/
extern OSD_Result_t		OSD_FONT_GetFontHeight(OSD_Font_t *font, HUINT32 *pHeight);

extern OSD_Result_t		OSD_FONT_GetAscenderBySize(OSD_FontId_t eFontId, HINT32 nSize, HUINT32 *pAscender);
extern OSD_Result_t		OSD_FONT_GetDescenderBySize(OSD_FontId_t eFontId, HINT32 nSize, HUINT32 *pDescender);
extern OSD_Result_t		OSD_FONT_GetFontHeightBySize(OSD_FontId_t eFontId, HINT32 nSize, HUINT32 *pHeight);
extern OSD_Result_t		OSD_FONT_GetFontSizeByWidth (OSD_FontId_t eFontId, const HUINT8 *szText, HINT32 w, HUINT32 *pulSize);
/**
  @remark	draw(put) character. \n
  			각 언어별 Automata 에 의해 문자를 조합하여 출력하고 사용한 byte 수를 리턴한다.\n
  			OSD_Screen_t *pixmap 의 값이 null 인 경우에는 문자는 그리지 않고, 사용될 Byte 수 만 리턴한다.\n
  @remark	API Type : sync \n
  @return		HUINT32, the number of characters drew\n
*/
extern HINT32		OSD_FONT_PutCh(OSD_Screen_t *pixmap, OSD_Font_t *font, HINT32 x, HINT32 y
						, HUINT32 color, HUINT16 encoding, HINT32 length, HUINT8 *string);
extern HINT32		OSD_FONT_GetNumOfLineNeeded(OSD_FontId_t eFontId,
																HINT32 nSize,
																HINT32 nWidthRatio,
																HINT32 nWidth,
																eHxCP_CodePage_e eEncoding,
																HUINT32 ulStrLen,
																const CH_300468 *pSingleByteStr,
																HUINT32 *pulNumOfLine);
extern HINT32		OSD_FONT_DrawString(OSD_Screen_t *pixmap, OSD_DrawFontInfo_t *pstDrawInfo
						, OSD_StringInfo_t *pstStrInfo);
/**
  @remark	count number of characters translated UNICODE. \n
  @remark	API Type : sync \n
  @return		HUINT32, the number of characters counted\n
*/
extern HINT32	OSD_FONT_GetChNum(HUINT16 encoding, HINT32 length, HUINT8 *string, HINT32 *convCount);

/**
  @remark	draw(put) characters to specified buffer. \n
  @remark	API Type : sync \n
  @return		HUINT32, the number of characters drew\n
*/
extern HINT32		OSD_FONT_PutChToBuffer(OSD_DrawableBuf_t *dstBuffer, OSD_Font_t *font, HINT32 x, HINT32 y, HUINT32 color, HUINT16 encoding, HUINT16 length, HUINT8 *string);

/**
  @remark	estimate pixel width for specified strings. \n
  @remark	API Type : sync \n
  @return		int, the number of pixle string width\n
*/
extern HINT32		OSD_FONT_GetStringPixelWidth(OSD_Font_t *font, eHxCP_CodePage_e encoding, const HUINT8 *string, unsigned int length);

/**
  @remark	draw(put) character. \n. \n
  @remark	API Type : sync \n
  @return		int, the number of pixle string width\n
*/
extern HINT32		OSD_FONT_DrawPutChAbbr(OSD_Screen_t *pixmap, OSD_Font_t *font, HINT32 x, HINT32 y, HINT32 otRectWidth, HUINT32 color, HUINT16 encoding, HUINT32 length, HUINT8 *string);
extern HBOOL		OSD_FONT_IsEnoughSpaceForText(OSD_Font_t *pFont, HINT32 nOrgRectWidth, HUINT16 eEncoding, HUINT32 nLength, HUINT8 *pString);

extern OSD_Result_t		OSD_FONT_CountChByWidth_300468(OSD_Font_t *font,
																			HINT32 w,
																			HUINT8* pszLocaleCode,
																			HUINT32 ulLocaleLen,
																			HUINT32 ulStrLen,
																			const CH_300468 *pSingleByteStr,
																			HUINT32 *ulCount);

extern OSD_Result_t		OSD_FONT_CountChByWidth_SingleByte(OSD_Font_t *font,
																			HINT32 w,
																			eHxCP_CodePage_e encoding,
																			HUINT32 ulStrLen,
																			const CH_1BYTE *pSingleByteStr,
																			HUINT32 *ulCount);

extern OSD_Result_t		OSD_FONT_CountChByWidth_UCS2(OSD_Font_t *font,
																			HINT32 w,
																			eHxCP_CodePage_e encoding,
																			HUINT32 ulStrLen,
																			const CH_1BYTE *pSingleByteStr,
																			HUINT32 *ulCount);
extern OSD_Result_t		OSD_FONT_CountChByWidth_UTF8(OSD_Font_t *font,
																HINT32 w,
																eHxCP_CodePage_e eEncoding,
																HUINT32 ulStrLen,
																const CH_1BYTE *pSingleByteStr,
																HUINT32 *ulCount);

extern int		OSD_FONT_CalcStrWidthRatio(OSD_Font_t * font, eHxCP_CodePage_e encoding, unsigned int orgRectWidth, unsigned int ulStrLen, const HUINT8 *pSingleByteStr, HUINT32 *pWidthRatio);

// 잠시 사용 차우 삭제 또는 이동 by dhkim2
#define	FONT_BOLD				0x0001
#define	FONT_OUTLINE			0x0002
#define	FONT_ITALIC				0x0004
#define	FONT_REVERSE			0x0008
#define	FONT_UNDERLINE			0x0010

/*
   @brief Shift-JIS를 SEQ로 바꾼다. SEQ는 뭔지 몰라요. by dhkim2
 */

HERROR	OSD_FONT_DrawCaptionFont(OSD_Screen_t *pixmap, HINT32 x, HINT32 y, HINT32 xsize
										, HINT32 ysize, HINT16 jis, HINT32 attribute
										, HUINT32 color );

//  도트당 2비트를 차지하는 4계조 폰트의 출력 함수.
int		OSD_FONT_DrawBitmapFont(OSD_Screen_t  *pixmap, int x, int y, int w, int h
										, unsigned char *bitmap, int attribute
										, HUINT32 *color);

void	OSD_FONT_SetOutlineColor(HUINT32 color);

#endif	/*__OSD_FONT_H__ */

