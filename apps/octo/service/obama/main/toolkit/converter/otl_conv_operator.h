
#ifndef	__OTL_CONV_OPERATOR_H__
#define	__OTL_CONV_OPERATOR_H__

#include <hlib.h>
#include <octo_common.h>

#ifdef  __cplusplus
extern "C" {
#endif

extern DxOperator_e	OTL_CONV_SvcOperator2SysOperator(DxOPTYPE_e eSvcOpType);
extern DxOPTYPE_e	OTL_CONV_SysOperator2SvcOperator(DxOperator_e eSysOperator);
extern const HCHAR*	OTL_CONV_Operator2String(DxOperator_e eOpType);
#ifdef  __cplusplus
}
#endif

#endif /* __OTL_CONV_OPERATOR_H__ */

