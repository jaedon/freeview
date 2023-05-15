
#ifndef	__OBAMA_RPCPROTOCOL_H__
#define	__OBAMA_RPCPROTOCOL_H__


#include <hlib.h>

#ifdef  __cplusplus
extern "C" {
#endif


HINT32		OBAMA_RpcProtocol_ReadAndDispatch (HINT32 nCount);
HERROR		OBAMA_RpcProtocol_GetRpcHandle (HINT32 *pnRpcHandle);


#ifdef  __cplusplus
}
#endif

#endif /* __OBAMA_RPCPROTOCOL_H__ */

