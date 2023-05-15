/* $Header: $ */

#ifndef DLIB_CONFIG_H__
#define DLIB_CONFIG_H__

#define	_____INSIDE_DLIB_CONFIG_H_____

#ifdef __cplusplus
extern "C" {
#endif

#include "config/_dlib_config_base.h"
#if defined(CONFIG_PROD_HDR1000S)
	#include "config/_dlib_config_hdr1000s.h"
#elif defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE)
	#include "config/_dlib_config_hms1000s.h"
#elif defined(CONFIG_PROD_IR4000HD)
	#include "config/_dlib_config_ir4000hd.h"
#elif defined(CONFIG_PROD_ICORDPRO)
	#include "config/_dlib_config_icordpro.h"
#elif defined(CONFIG_PROD_HDNEO)
	#include "config/_dlib_config_hdneo.h"
#elif defined(CONFIG_PROD_HGS1000S)
	#include "config/_dlib_config_hgs1000s.h"
#elif defined(CONFIG_PROD_HD9000I)
	#include "config/_dlib_config_hd9000i.h"
#elif defined(CONFIG_PROD_HMS1000T)
	#include "config/_dlib_config_hms1000t.h"
#elif defined(CONFIG_PROD_IHDR6100C)
	#include "config/_dlib_config_ihdr6100c.h"
#elif defined(CONFIG_PROD_FVP4000T)
	#include "config/_dlib_config_fvp4000t.h"
#elif defined(CONFIG_PROD_HMX4KBBC)
	#include "config/_dlib_config_hmx4kbbc.h"
#elif defined(CONFIG_PROD_DIGIMAXT2)
	#include "config/_dlib_config_digimaxt2.h"
#elif defined(CONFIG_PROD_HDFOXIRCONNECT)
	#include "config/_dlib_config_hdfoxirconnect.h"
#elif defined(CONFIG_PROD_YSR2000)
	#include "config/_dlib_config_ysr2000.h"
#elif defined(CONFIG_PROD_YS1000)
	#include "config/_dlib_config_ys1000.h"
#elif defined(CONFIG_PROD_CDIG2000C)
	#include "config/_dlib_config_cdig2000c.h"
#elif defined(CONFIG_PROD_ROCKYPOM) || defined(CONFIG_PROD_ROCKYWPOM)
	#include "config/_dlib_config_rockypom.h"
#elif defined(CONFIG_PROD_HA7100S)
	#include "config/_dlib_config_ha7100s.h"
#elif defined(CONFIG_PROD_M1)
	#include "config/_dlib_config_m1.h"
#else
	#error ("config/_dlib_config_[modelname/opname].h Shall be included!!!\n")
#endif

#undef _____INSIDE_DLIB_CONFIG_H_____

#ifdef __cplusplus
}
#endif


#endif // DLIB_CONFIG_H__


