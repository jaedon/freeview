/* $Header: $ */

#ifndef __DLIB_CONFIG_HGS1000S_H__
#define __DLIB_CONFIG_HGS1000S_H__

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_____INSIDE_DLIB_CONFIG_H_____)
	#error ("!!! _dlib_config_icordpro.h shall be included only in the dlib_config.h !!!\n");
#endif

#undef DxBOUQUET_LIST_BUF_LEN
#define DxBOUQUET_LIST_BUF_LEN					128

#undef EPG_HUMAX_MOUNT_PATH
#define EPG_HUMAX_MOUNT_PATH		CONFIG_PRODUCT_USERDATA_DIR"/ipepg"

#ifdef __cplusplus
}
#endif


#endif // __DLIB_CONFIG_ICORDPRO_H__


