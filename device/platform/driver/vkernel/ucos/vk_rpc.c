/*************************************************************
 * @file		hxsocket.h
 * @date		2012/03/13
 * @author		someone
 * @brief		HLIB Socket API

 * http://www.humaxdigital.com
 *************************************************************/

#define ___HEADER________________________________________________________________________________________

#include "vkernel.h"


#define ___DEFINE________________________________________________________________________________________

#define ___TYPEDEF_______________________________________________________________________________________

#define ___STATIC_VARIABLES______________________________________________________________________________

#define ___PRIVATE_FUNCTION_PROTOTYPE____________________________________________________________________

#define	___PRIVATE_API___________________________________________________________________________________

#define	___PUBLIC_API____________________________________________________________________________________

int VK_RPC_Open(HUINT32 nRoute,
                HINT32 nProgId,
                HUINT32 *hRpc)
{
	(void)(nRoute);
	(void)(nProgId);
	(void)(hRpc);
	return VK_OK;
}

int VK_RPC_Close(HUINT32 hRpc)
{
	(void)(hRpc);
	return VK_OK;
}

int VK_RPC_RegisterCall(HUINT32 hRpc,
	                    const HCHAR *pszMethodName,
	                    const HCHAR *pszSignature,
	                    VkRPC_Func_t pfnFunction,
	                    void *pvUserData)
{
	(void)(hRpc);
	(void)(pszMethodName);
	(void)(pszSignature);
	(void)(pfnFunction);
	(void)(pvUserData);
	return VK_OK;
}

int VK_RPC_RegisterNotifier(HUINT32 hRpc,
	                        const HCHAR *pszNotifierName,
	                        void *pvData,
	                        VkRPC_Noti_t pfnNotifierCb)
{
	(void)(hRpc);
	(void)(pszNotifierName);
	(void)(pvData);
	(void)(pfnNotifierCb);
	return VK_OK;
}

int	VK_RPC_Request(HUINT32 hRpc,
	               VkRPC_Response_t pfnCallback,
	               HUINT32 nTimeout,
	               void *pvUserdata,
	               const HCHAR *pszMethodName,
	               const HCHAR *pszSignature,
	               ...)
{
	(void)(hRpc);
	(void)(pfnCallback);
	(void)(nTimeout);
	(void)(pvUserdata);
	(void)(pszMethodName);
	(void)(pszSignature);
	return VK_OK;
}

int	VK_RPC_Call(HUINT32 hRpc,
	            const HCHAR *pszMethodName,
	            const HCHAR *pszSignature,
	            ...)
{
	(void)(hRpc);
	(void)(pszMethodName);
	(void)(pszSignature);
	return VK_OK;
}

int	VK_RPC_Response(HUINT32 hRpc,
	                HUINT32 hSockClient,
	                HUINT32 nReqId,
	                const HCHAR *pszMethodName,
	                const HCHAR *pszSignature,
	                ...)
{
	(void)(hRpc);
	(void)(hSockClient);
	(void)(nReqId);
	(void)(pszMethodName);
	(void)(pszSignature);
	return VK_OK;
}

int VK_RPC_Notify(HUINT32 hRpc,
	              const HCHAR *pszNotifierName,
	              const HCHAR *pszSignature,
	              ...)
{
	(void)(hRpc);
	(void)(pszNotifierName);
	(void)(pszSignature);
	return VK_OK;

}

int VK_RPC_ProcessRpc(HUINT32 hRpc,
	                  HUINT32 nReqId,
	                  void *pvData,
	                  HUINT32 ulSize,
	                  HUINT32 hClientSock)
{
	(void)(hRpc);
	(void)(nReqId);
	(void)(pvData);
	(void)(ulSize);
	(void)(hClientSock);
	return VK_OK;
}

int VK_RPC_Init(void)
{
	return VK_OK;
}

void VK_RPC_PrintRpcInfo(void)
{
}
