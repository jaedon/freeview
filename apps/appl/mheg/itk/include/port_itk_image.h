#ifndef _PORT_ITK_IMAGE_H_
#define _PORT_ITK_IMAGE_H_

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>

#include <port_itk_main.h>
#include "itk_display.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
/** @brief Display Image handle type */
typedef struct ITK_ImageHandle_t
{
	itk_image_t vtbl;
	itk_display_plane_t target_plane;
	void *content;
	size_t content_size;
	itk_image_format_t format;
	//HUINT8 ucImageId;
	HUINT8 ucImageEv;
} ITK_ImageHandle_t;

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
extern itk_image_t *port_itk_image_NewImage(struct itk_display_t *thiz,
									   itk_image_format_t format, itk_display_plane_t target_plane,
									   const void *content, size_t content_size);

#endif

