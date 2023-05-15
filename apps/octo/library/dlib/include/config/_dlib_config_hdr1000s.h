/* $Header: $ */

#ifndef __DLIB_CONFIG_HDR1000S_H__
#define __DLIB_CONFIG_HDR1000s_H__

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_____INSIDE_DLIB_CONFIG_H_____)
	#error ("!!! _dlib_config_hdr1000s.h shall be included only in the dlib_config.h !!!\n");
#endif

#undef	DxMEDIA_CERT_PATH
#undef	DxMEDIA_KEY_PATH
#define	DxMEDIA_CERT_PATH	"/tmp/FKP/Freesat_Unique_Device_Certificate1.txt"
#define	DxMEDIA_KEY_PATH	"/tmp/FKP/Freesat_Unique_Device_Key1.txt"

#ifdef __cplusplus
}
#endif


/*		DB
 */
#undef DxMAX_GUIDANCE_INFO_LEN
#define DxMAX_GUIDANCE_INFO_LEN			255
/*		DB End		*/


/*		EPG
 */

// for CONFIG_OP_FREESAT_GEN2_ADDFILTER
#undef DxEPG_EXT_TEXT_LEN
#define DxEPG_EXT_TEXT_LEN								(256)	/* Size per one text */

// for CONFIG_OP_FREESAT_GEN2_ADDFILTER
#undef DxEPG_EXT_INFO_MAX
#undef DxEPG_EXT_ITEM_MAX
#define DxEPG_EXT_INFO_MAX								(15)
#define DxEPG_EXT_ITEM_MAX								(15) // Need Check

#undef DxEPG_CONTENTS_MAX
#define DxEPG_CONTENTS_MAX									2

// only for CONFIG_OP_FREESAT_GEN2
#define DxEPG_ONLINE_AVAIL_INFO_MAX						(1)
#define DxEPG_SHORT_EVENT_PROMO_INFO_MAX					(1)
#define DxEPG_MEDIA_LOCATOR_INFO_MAX						(10)
/*		EPG End		*/



#endif // __DLIB_CONFIG_HDR1000S_H__


