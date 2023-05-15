
#ifndef	__OTL_CONV_COMMON_H__
#define	__OTL_CONV_COMMON_H__


#include <htype.h>
#include <stb_params.h>

#ifdef  __cplusplus
extern "C" {
#endif

/**
 * SVC_SatType_t
 * <-> string
 */
SVC_SatType_t 		OTL_CONV_SatString2SVC_SatType (const HCHAR *name);
const HCHAR *		OTL_CONV_SVC_SatType2String (SVC_SatType_t type);

#ifdef  __cplusplus
}
#endif

#endif /* __OTL_CONV_COMMON_H__ */

