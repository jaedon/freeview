
	
#if !defined(__PUDA_PUSH_CONTROLLER_H__)
#define __PUDA_PUSH_CONTROLLER_H__


#ifdef __cplusplus
extern "C"{
#endif


#define MAXCONNECTIONS  3					// 커넥션 개수 


typedef void *mxDlnaPush_Token;
typedef void *mxDlnaPush_Session;
//typedef void *ILibThreadPool;

typedef void (*mxDlnaPush_pushFileResponse)(mxDlnaPush_Session session, int errorCode, void* sessionTag);
typedef void (*mxDlnaPush_transferStatus)(mxDlnaPush_Session session, int startTransfer,int finishTransfer, MX_INT64 bytesTransfered,MX_INT64 totalBytesToTransfer, void* sessionTag);

void mxDlnaPush_init();
void mxDlnaPush_destroy();

mxDlnaPush_Token mxDlnaPush_pushObject(void *tag);
void mxDlnaPush_destroyPushObject(mxDlnaPush_Token token);
void* mxDlnaPush_getTag(mxDlnaPush_Token token);
void mxDlnaPush_setTag(mxDlnaPush_Token token, void* tag);

mxDlnaPush_Session mxDlnaPush_pushFile(mxDlnaPush_Token token,
//									   struct AVRendererConnection* rendererConnection,
										void *rendererConnection,	// 변경 
									   char* filePath, 
									   //char* mimeType, char* protocolInfo,
									   mxDlnaPush_pushFileResponse callback,
									   mxDlnaPush_transferStatus updateTransferCallback,
									   void* sessionTag);

mxDlnaPush_Session mxDlnaPush_pushFileInputMimeNProtocolInfo(mxDlnaPush_Token token,
									   void *rendererConnection,	// 변경 
									   char* filePath, 
									   char* mimeType,
									   char* protocolInfo,
									   mxDlnaPush_pushFileResponse callback,
									   mxDlnaPush_transferStatus updateTransferCallback,
									   void* sessionTag);



mxDlnaPush_Token mxDlnaPush_getPushControllerToken(mxDlnaPush_Session session);
void mxDlnaPush_updateTransferStatus(mxDlnaPush_Session session);
	
#ifdef __cplusplus
}
#endif	
	
#endif /* __PUDA_PUSH_CONTROLLER_H__ */
