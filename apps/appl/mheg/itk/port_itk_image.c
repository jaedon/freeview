/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>

#include <apk.h>

#include "port_itk_main.h"
#include "port_itk_display.h"
#include "port_itk_image.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      external Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/********************************************************************/
/********************      proto-type functions  ********************/
/********************************************************************/



/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
/**
 * refer to itk_display.h - itk_image_t
 *
 * @author swkim (2011-08-09)
 *
 * @param[in] thiz The image
 */
void mheg_itk_port_imageRelease(struct itk_image_t *thiz)
{
	HUINT32				ulViewId;
	ITK_ImageHandle_t	*image;

	image = (ITK_ImageHandle_t *)thiz;
	if (thiz == NULL)
	{
		HxLOG_Error(" input parameter is invalid NULL\n");
		return;
	}

	PORT_ITK_GetMastViewId(&ulViewId);

	HxLOG_Debug(C_CODE_F_RED"image release - ucImageEv (%d), content (%p), content_size (%d)"C_CODE_RESET"\n", image->ucImageEv, image->content, image->content_size);

	if (image->ucImageEv != eITK_DISPLAY_EVENT_STOP)
	{
		HUINT8 plane = (HUINT8)((image->target_plane==ITK_DISPLAY_PLANE_VIDEO) ? MHEG_DISPLAY_PLANE_STILL2 : MHEG_DISPLAY_PLANE_STILL1);
		//stop image
		PORT_ITK_PostMsg(eMHEG_DISPLAY_IMAGE_RELEASE, ulViewId, 0, (HINT32)plane, 0, 0);

		port_itk_display_SetStillPlaneStatus(plane, FALSE);
		image->ucImageEv = eITK_DISPLAY_EVENT_STOP;
	}

	if (image->content != NULL)
	{
		HLIB_STD_MemFree(image->content);
		image->content = NULL;
	}
	HLIB_STD_MemFree(image);
}

/**
 * refer to itk_display.h - itk_image_t
 *
 * @author swkim (2011-08-09)
 *
 * @param[in] thiz The image
 */
void mheg_itk_port_imageShow(struct itk_image_t *thiz)
{
	HUINT32		ulViewId;
	ITK_ImageHandle_t *image = NULL;

	image = (ITK_ImageHandle_t *)thiz;
	if (image == NULL)
	{
		HxLOG_Error(" input parameter is invalid NULL\n");
		return;
	}

	if (PORT_ITK_GetMhegInternalRunningStatus() == FALSE)
	{
		HxLOG_Error("Redkey Engine should not be call this function - ignore (%s)\n", __FUNCTION__);
		return;
	}

	PORT_ITK_GetMastViewId(&ulViewId);
	HxLOG_Debug(C_CODE_F_GREEN"image show - ucImageEv (%d), content (%p), content_size (%d), format (%d)"C_CODE_RESET"\n", image->ucImageEv, image->content, image->content_size, image->format);

	if (image->ucImageEv == eITK_DISPLAY_EVENT_STOP)
	{
		if ((image->content == NULL) || (image->content_size == 0))
		{
			HxLOG_Error(" image content or content_size is invalid.\n");
			return;
		}
		else
		{
			HUINT8 plane = (HUINT8)((image->target_plane==ITK_DISPLAY_PLANE_VIDEO) ? MHEG_DISPLAY_PLANE_STILL2 : MHEG_DISPLAY_PLANE_STILL1);

			//play image
			// TODO: check memory leak.
			PORT_ITK_PostMsg(eMHEG_DISPLAY_IMAGE_SHOW, ulViewId, (HINT32)image, plane, FALSE, 0);

			port_itk_display_SetStillPlaneStatus(plane, TRUE);
			image->ucImageEv = eITK_DISPLAY_EVENT_PLAY;
		}
	}
}

/**
 *
 *
 * @author swkim (2011-08-09)
 *
 * @param[in] thiz The image
 */
void mheg_itk_port_imageHide(struct itk_image_t *thiz)
{
	HUINT32		ulViewId;
	ITK_ImageHandle_t *image = NULL;

	image = (ITK_ImageHandle_t *)thiz;
	if (thiz == NULL)
	{
		HxLOG_Error(" input parameter is invalid NULL\n");
		return;
	}

	PORT_ITK_GetMastViewId(&ulViewId);

	HxLOG_Debug("image hide - ucImageEv (%d), content (%p), content_size (%d)\n", image->ucImageEv, image->content, image->content_size);

	if (image->ucImageEv != eITK_DISPLAY_EVENT_STOP)
	{
		HUINT8 plane = (HUINT8)((image->target_plane==ITK_DISPLAY_PLANE_VIDEO) ? MHEG_DISPLAY_PLANE_STILL2 : MHEG_DISPLAY_PLANE_STILL1);
		//stop image
		PORT_ITK_PostMsg(eMHEG_DISPLAY_IMAGE_HIDE, ulViewId, 0, plane, 0, 0);

		port_itk_display_SetStillPlaneStatus(plane, FALSE);
		image->ucImageEv = eITK_DISPLAY_EVENT_STOP;
	}
}

#define _____PUBLIC_APIs________________________________________________________________
/**
 * Returns an object that can be used to present an I-frame image on the
 * video or still planes.
 *
 * @see image
 *
 * @warning The parameter content is transitory and can not be assumed to be
 * valid on return from this call. An implementation that wishes to keep this
 * source data will need to make a local copy of the data.
 *
 * @param[in] thiz The display
 * @param[in] targetPlane The plane this image is to be displayed on
 * @param[in] content A pointer to a buffer that holds the content to present
 * @param[in] contentSize The nmber of bytes of content in the buffer
 *
 * @return NULL if the image can not be created
 * @return An image for the content
 */
itk_image_t *port_itk_image_NewImage(struct itk_display_t *thiz,
									   itk_image_format_t format, itk_display_plane_t target_plane,
									   const void *content, size_t content_size)
{
	ITK_ImageHandle_t *image = NULL;

	if (thiz == NULL)
	{
		HxLOG_Error(" input parameter is invalid NULL\n");
		return (itk_image_t *)NULL;
	}

	if (content == NULL || content_size == 0)
	{
		HxLOG_Error(" content or content_size is NULL\n");
		return (itk_image_t *)NULL;
	}

	image = (ITK_ImageHandle_t *)HLIB_STD_MemAlloc(sizeof(ITK_ImageHandle_t));
	if (image == NULL)
	{
	/**
	 * failed to allocate image.
	 */
		HxLOG_Error(" image is NULL\n");
		return (itk_image_t *)NULL;
	}

	HxSTD_memset(image, 0, sizeof(ITK_ImageHandle_t));

	/**
	 * 1. link image handling functions.
	 * 2. target_pland
	 * 3. content
	 */
	image->vtbl.release	= mheg_itk_port_imageRelease;
	image->vtbl.show	= mheg_itk_port_imageShow;
	image->vtbl.hide	= mheg_itk_port_imageHide;	// check point
	image->target_plane = target_plane;					// <-- check point
	image->content = (void *)HLIB_STD_MemAlloc(content_size);
	image->format = format;

	/**
	 * failed to allocate image contnt.
	 */
	if (image->content == NULL)
	{
		HxLOG_Error(" image->content allocation failed.\n");
		goto _host_display_err;
	}
	HxSTD_MemCopy(image->content, content, content_size);

	/**
	 * 4. content_size
	 * 5. image event (무조건 STOP으로 설정)
	 */
	image->content_size = content_size;
	image->ucImageEv = eITK_DISPLAY_EVENT_STOP;

	HxLOG_Info(" image created.\n\t\t target_plane (%s), content_size (%d), imageev (%d) image type(%d)\n",
		((image->target_plane == ITK_DISPLAY_PLANE_VIDEO) ? "VIDEO" : "Image"),
		image->content_size, image->ucImageEv, format);

	return (itk_image_t *)image;

_host_display_err:
	if (image->content != NULL)
	{
		HLIB_STD_MemFree(image->content);
		image->content = NULL;
	}

	HLIB_STD_MemFree(image);
	image = NULL;

	HxLOG_Error(" failed to create an image.\n");
	return (itk_image_t *)NULL;
}


