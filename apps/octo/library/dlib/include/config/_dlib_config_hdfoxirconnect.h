/* $Header: $ */

#ifndef __DLIB_CONFIG_HDFOXIRCONNECT_H__
#define __DLIB_CONFIG_HDFOXIRCONNECT_H__

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_____INSIDE_DLIB_CONFIG_H_____)
	#error ("!!! _dlib_config_hdfoxirconnect.h shall be included only in the dlib_config.h !!!\n");
#endif

// For PVR Recordings
#undef DxPVR_THUMBNAIL_WIDTH
#undef DxPVR_THUMBNAIL_HEIGHT
#define	DxPVR_THUMBNAIL_WIDTH					140
#define	DxPVR_THUMBNAIL_HEIGHT				78

#undef EPG_HUMAX_MOUNT_PATH
#define EPG_HUMAX_MOUNT_PATH		CONFIG_PRODUCT_USERDATA_DIR"/ipepg"


#ifdef __cplusplus
}
#endif

#endif // __DLIB_CONFIG_HDFOXIRCONNECT_H__

