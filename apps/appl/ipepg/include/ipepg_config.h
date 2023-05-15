/* $Header: $ */

#ifndef IPEPG_CONFIG_H__
#define IPEPG_CONFIG_H__

#define	_____INSIDE_IPEPG_CONFIG_H_____

#ifdef __cplusplus
extern "C" {
#endif

#include "config/_ipepg_config_base.h"
#if defined(CONFIG_PROD_HMS1000T)
	#include "config/_ipepg_config_hms1000t.h"
#elif defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE)
	#include "config/_ipepg_config_hms1000s.h"
#elif defined(CONFIG_PROD_IR4000HD)
	#include "config/_ipepg_config_ir4000hd.h"
#elif defined(CONFIG_PROD_DIGIMAXT2)
	#include "config/_ipepg_config_digimaxt2.h"
#elif defined(CONFIG_PROD_ICORDPRO)
	#include "config/_ipepg_config_icordpro.h"
#elif defined(CONFIG_PROD_FVP4000T)
	#include "config/_ipepg_config_fvp4000t.h"
#elif defined(CONFIG_PROD_HDNEO)
	#include "config/_ipepg_config_hdneo.h"
#elif defined(CONFIG_PROD_HGS1000S)
	#include "config/_ipepg_config_hgs1000s.h"
#elif defined(CONFIG_PROD_HGS1000SIT)
	#include "config/_ipepg_config_hgs1000sit.h"
	
#endif


#undef _____INSIDE_IPEPG_CONFIG_H_____

#ifdef __cplusplus
}
#endif


#endif // IPEPG_CONFIG_H__


