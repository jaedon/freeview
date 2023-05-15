/**************************************************************************************/
/**
 * @file pblgraphickit.c
 *
 * The media interfaces
 *
 **************************************************************************************
 **/


#ifndef	__BPLGRAPHIC_H__
#define	__BPLGRAPHIC_H__

#include "bpltype.h"

/******************************************************************
	Define
******************************************************************/

/******************************************************************
	Enum
 ******************************************************************/

/******************************************************************
	Typedef
******************************************************************/
typedef void BplGraphic_t;

/******************************************************************
	Interfaces	
******************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Create the graphic instance
 *
 * @return the handle of graphic instance on success, and NULL on error
 */
BplGraphic_t	*BPL_Graphic_Create(int nWidth, int nHeight);

/**
 * Destroy the graphic instance.
 *
 * @param[in] hGraphic the handle of graphic instance
 *
 * @return none
 */
void BPL_Graphic_Destroy(BplGraphic_t *hGraphic);

/**
 * Get the memory address of the framebuffer.
 *
 * @return the memory address of the framebuffer if your platform supports a framebuffer.
 */
char *BPL_Graphic_GetFrameBuffer(BplGraphic_t *hGraphic);

/**
 * Get the width size of the screen.
 * 
 * @param[in] hGraphic the handle of graphic instance
 * @param[out] pnWidth the width of the screen
 *
 * @return BPL_STATUS_OK on success, and other status on error.
 */
BPLStatus BPL_Graphic_GetWidth(BplGraphic_t *hGraphic, int *pnWidth);

/**
 * Get the height size of the screen.
 *
 * @param[in] hGraphic the handle of graphic instance
 * @param[out] pnHeight the height of the screen
 *
 * @return BPL_STATUS_OK on success, and other status on error.
 */
BPLStatus BPL_Graphic_GetHeight(BplGraphic_t *hGraphic, int *pnHeight);

/**
 * This function is called when it has drawn something to the screen.
 *
 * @param[in] x the start position of the abscissa
 * @param[in] y the start position of the ordinate
 * @param[in] width the width of the updated area
 * @param[in] height the height of the updated area
 *
 * @return BPL_STATUS_OK on success, and other status on error.
 */
BPLStatus BPL_Graphic_UpdateScreen(BplGraphic_t *hGraphic, int x, int y,
							int width, int height);

#ifdef __cplusplus
};

/******************************************************************
	class BplGraphic
******************************************************************/
class BplGraphic {
public:
	BplGraphic() : m_graphic(NULL) {}
	~BplGraphic() { destroy(); }

	bool create(int nWidth, int nHeight)
	{
		m_graphic = BPL_Graphic_Create(nWidth, nHeight);
		return m_graphic != NULL;
	}

	void destroy()
	{
		if ( !m_graphic ) return;
		BPL_Graphic_Destroy(m_graphic);
		m_graphic = NULL;
	}

	HCHAR *getFrameBuffer()
	{ return BPL_Graphic_GetFrameBuffer(m_graphic); }

	int getWidth()
	{
		int nWidth = 0;
		BPL_Graphic_GetWidth(m_graphic, &nWidth);
		return nWidth;
	}

	int getHeight()
	{
		int nHeight = 0;
		BPL_Graphic_GetHeight(m_graphic, &nHeight);
		return nHeight;
	}

	bool updateScreen(int x, int y, int width, int height)
	{ return BPL_Graphic_UpdateScreen(m_graphic, x, y, width, height) != BPL_STATUS_OK; }

private:
	BplGraphic_t		*m_graphic;	

};


#endif

#endif


