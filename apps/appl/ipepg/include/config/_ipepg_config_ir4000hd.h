/* $Header: $ */

#ifndef __IPEPG_CONFIG_IR4000HD_H__
#define __IPEPG_CONFIG_IR4000HD_H__

#undef IPEPG_TOTAL_TIME
#undef EPG_TRIPLEID_FILE
#undef HUMAX_UPLOADFILE_PATH
#undef RP_UPLOAD_FILE

//#define	IPEPG_TEST_DIFFERENT_TIME											// 현재시간 변경 test 진행.

#define IPEPG_MAX_IMAGE_SIZE					(300)

#define IPEPG_TOTAL_TIME						(24*7)				// 1주치를 받는다. 			24시간 * 7 시간

#define EPG_TRIPLEID_FILE 						CONFIG_PRODUCT_USERDATA_DIR"/ipepg/tripleid.xml"
#define HUMAX_UPLOADFILE_PATH					CONFIG_PRODUCT_USERDATA_DIR"/ipepg/stb_status.xml"
#define	RP_UPLOAD_FILE							CONFIG_PRODUCT_USERDATA_DIR"/ipepg/tmp_status.xml"


#endif // __IPEPG_CONFIG_IR4000HD_H__


