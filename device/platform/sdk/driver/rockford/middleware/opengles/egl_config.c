/***************************************************************************
 *     Copyright (c) 2004-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: egl_config.c $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 7/4/09 5:12p $
 *
 * Module Description: Config entry point functions.
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/middleware/opengles/egl_config.c $
 * 
 * Hydra_Software_Devel/4   7/4/09 5:12p nissen
 * PR 56587: Removed configs that had no depth buffer.
 * 
 * Hydra_Software_Devel/3   5/8/07 1:52p nissen
 * PR 30062: Fixed config order.
 * 
 * Hydra_Software_Devel/2   7/28/04 12:20p nissen
 * PR 9132: Fixed comment header.
 * 
 ***************************************************************************/

#include "egl.h"
#include "egl_private.h"

/***************************************************************************/
static EGLint s_aiConfig[] = 
{
	0x01100565,    /* ID=1  DEPTH=16  COLOR=565  */
	0x02108888,    /* ID=4  DEPTH=16  COLOR=8888 */
	0x03104444,    /* ID=2  DEPTH=16  COLOR=4444 */
	0x04101555     /* ID=3  DEPTH=16  COLOR=1555 */
};

static EGLint s_iConfigCount = (sizeof s_aiConfig) / (sizeof (EGLint));

/***************************************************************************/
EGLConfig EGL_P_GetConfig( 
	EGLint iID )
{
	if( iID >= s_iConfigCount )
		return 0;

	return (EGLConfig) s_aiConfig[iID];
}

/***************************************************************************/
/* ENTRY POINTS                                                            */
/***************************************************************************/
GLAPI EGLBoolean APIENTRY eglGetConfigs( 
	EGLDisplay dpy, 
	EGLConfig *configs, 
	EGLint config_size, 
	EGLint *num_config )
{
	EGLint ii;

	EGL_P_Display *pDisplay = EGL_P_ValidateDisplay( dpy );
	if( pDisplay == NULL )
		return EGL_FALSE;

	if( num_config == NULL )
	{
		EGL_P_SET_ERROR(EGL_BAD_PARAMETER);
		return EGL_FALSE;
	}

	/* get configs */
	if( configs )
	{
		for( ii = 0; ii < config_size; ++ii )
			configs[ii] = (EGLConfig) s_aiConfig[ii];

		*num_config = config_size;
	}
	else
	{
		*num_config = s_iConfigCount;
	}

	return EGL_TRUE;
}

/***************************************************************************/
GLAPI EGLBoolean APIENTRY eglChooseConfig( 
	EGLDisplay dpy, 
	const EGLint *attrib_list, 
	EGLConfig *configs, 
	EGLint config_size, 
	EGLint *num_config )
{
	EGLint buffer_size = 0;
	EGLint alpha_size = 0;
	EGLint red_size = 0;
	EGLint green_size = 0;
	EGLint blue_size = 0;
	EGLint depth_size = 0;
	EGLint config_id = 0;
	EGLint iNumConfig = 0;
	EGLint ii;

	EGL_P_Display *pDisplay = EGL_P_ValidateDisplay( dpy );
	if( pDisplay == NULL )
		return EGL_FALSE;

	/* parse attribute list */
	if( attrib_list )
	{
		EGLint attribute = *attrib_list++;
		while( attribute != EGL_NONE )
		{
			switch( attribute )
			{
			case EGL_BUFFER_SIZE: buffer_size = *attrib_list++; break;
			case EGL_ALPHA_SIZE:  alpha_size  = *attrib_list++; break;
			case EGL_RED_SIZE:    red_size    = *attrib_list++; break;
			case EGL_GREEN_SIZE:  green_size  = *attrib_list++; break;
			case EGL_BLUE_SIZE:   blue_size   = *attrib_list++; break;
			case EGL_DEPTH_SIZE:  depth_size  = *attrib_list++; break;
			case EGL_CONFIG_ID:   config_id   = *attrib_list++; break;
			default: attrib_list++;
			}
			attribute = *attrib_list++;
		}
	}

	/* check if specific config id is requested */
	if( config_id )
	{
		for( ii = 0; ii < s_iConfigCount; ++ii )
		{
			if( config_id == EGL_P_CONFIG_ID(s_aiConfig[ii]) )
			{
				if( configs )
					*configs = (EGLConfig) s_aiConfig[ii];
				if( num_config )
					*num_config = 1;
				return EGL_TRUE;
			}
		}
	}

	/* find configs that match attributes */
	for( ii = 0; ii < s_iConfigCount; ++ii )
	{
		if( (buffer_size <= EGL_P_BUFFER_SIZE(s_aiConfig[ii])) && 
			(alpha_size  <= EGL_P_ALPHA_SIZE(s_aiConfig[ii])) && 
			(red_size    <= EGL_P_RED_SIZE(s_aiConfig[ii])) && 
			(green_size  <= EGL_P_GREEN_SIZE(s_aiConfig[ii])) && 
			(blue_size   <= EGL_P_BLUE_SIZE(s_aiConfig[ii])) &&
			(depth_size  <= EGL_P_DEPTH_SIZE(s_aiConfig[ii])) )
		{
			if( configs )
				configs[iNumConfig++] = (EGLConfig) s_aiConfig[ii];
			if( iNumConfig == config_size )
				break;
		}
	}

	if( num_config )
		*num_config = iNumConfig;

	return EGL_TRUE;
}

/***************************************************************************/
GLAPI EGLBoolean APIENTRY eglGetConfigAttrib( 
	EGLDisplay dpy, 
	EGLConfig config, 
	EGLint attribute, 
	EGLint *value )
{
	EGL_P_Display *pDisplay = EGL_P_ValidateDisplay( dpy );
	if( pDisplay == NULL )
		return EGL_FALSE;

	switch( attribute )
	{
	case EGL_BUFFER_SIZE:             *value = EGL_P_BUFFER_SIZE(config); break;
	case EGL_ALPHA_SIZE:              *value = EGL_P_ALPHA_SIZE(config); break;
	case EGL_RED_SIZE:                *value = EGL_P_RED_SIZE(config); break;
	case EGL_GREEN_SIZE:              *value = EGL_P_GREEN_SIZE(config); break;
	case EGL_BLUE_SIZE:               *value = EGL_P_BLUE_SIZE(config); break;
	case EGL_DEPTH_SIZE:              *value = EGL_P_DEPTH_SIZE(config); break;
	case EGL_STENCIL_SIZE:            *value = 0; break;
	case EGL_CONFIG_CAVEAT:           *value = EGL_DONT_CARE; break;
	case EGL_CONFIG_ID:               *value = EGL_P_CONFIG_ID(config); break;
	case EGL_LEVEL:                   *value = 0; break;
	case EGL_MAX_PBUFFER_WIDTH:       *value = EGL_P_RENDER_SIZE_MAX; break;
	case EGL_MAX_PBUFFER_HEIGHT:      *value = EGL_P_RENDER_SIZE_MAX; break;
	case EGL_MAX_PBUFFER_PIXELS:      *value = EGL_P_RENDER_SIZE_MAX * EGL_P_RENDER_SIZE_MAX; break;
	case EGL_NATIVE_RENDERABLE:       *value = EGL_DONT_CARE; break;
	case EGL_NATIVE_VISUAL_ID:        *value = EGL_P_CONFIG_ID(config); break;
	case EGL_NATIVE_VISUAL_TYPE:      *value = EGL_DONT_CARE; break;
	case EGL_SAMPLE_BUFFERS:          *value = 0; break;
	case EGL_SAMPLES:                 *value = 0; break;
	case EGL_SURFACE_TYPE:            *value = EGL_WINDOW_BIT; break;
	case EGL_TRANSPARENT_TYPE:        *value = EGL_NONE; break;
	case EGL_TRANSPARENT_RED_VALUE:   *value = EGL_DONT_CARE; break;
	case EGL_TRANSPARENT_GREEN_VALUE: *value = EGL_DONT_CARE; break;
	case EGL_TRANSPARENT_BLUE_VALUE:  *value = EGL_DONT_CARE; break;
	default: *value = 0;
	}

	return EGL_TRUE;
}

/* End of File */
