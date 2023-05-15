/* $Header: $ */

#ifndef __DLIB_CONFIG_HMS1000S_H__
#define __DLIB_CONFIG_HMS1000S_H__

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_____INSIDE_DLIB_CONFIG_H_____)
	#error ("!!! _dlib_config_hms1000s.h shall be included only in the dlib_config.h !!!\n");
#endif

#if defined(CONFIG_SUPPORT_NATIVE_APP)
#undef EPG_HUMAX_MOUNT_PATH
#define EPG_HUMAX_MOUNT_PATH		CONFIG_PRODUCT_USERDATA_DIR"/ipepg"
#endif

#ifdef __cplusplus
}
#endif


#endif // __DLIB_CONFIG_HMS1000S_H__


