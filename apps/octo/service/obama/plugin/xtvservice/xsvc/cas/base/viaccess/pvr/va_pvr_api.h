/*
	Viaccess PVR API
		Driver API Manual ACS 3.0 - Doc. Ref. 2217, p.123
*/

#ifndef VA_PVR_API_H
#define VA_PVR_API_H

#define VA_PVR_INVALID_ECM_OFFSET			0xFFFFFFFF

typedef enum {
	eVaPvrFileStatus_Close,
	eVaPvrFileStatus_FileClose,		// file close �ϴ� ��.. close ���� �ܰ�
	eVaPvrFileStatus_Open,
	eVaPvrFileStatus_FileOpen,		// file open �ϴ� ��,, open ���� �ܰ�
} VA_PVR_FILE_STATUS_t;

// �Ϲ� rec�� ���� �ٷ� file�� ����.
// tsr�� ���� memory�� linked list�� �����Ѵ�.. (��ð��� �ϵ� 2�ð��� �����ϸ�ȴ�... circular array ����Ͽ� ������ ecm�� �����..)
typedef struct {
	VA_PVR_FILE_STATUS_t eStatus;
	HUINT32	ulAcsId;
	HUINT32	ulContentsId;
	HUINT32 ulTimestamp;
	HBOOL	bIsTimeshift;
	Handle_t hVaActionPb;
	HUINT32	ulFileHandle;
	HUINT32 ulSemId;
	HBOOL bBlocked;		// descramble�� �ȵǴ� ������ �������� �ʴ´�.
	HBOOL bRecord;		// ���� record ��
	HINT32 nTimeOffset;
} VA_PVR_FILE_t;

typedef struct {
	VA_PVR_FILE_t *pFile;
	HUINT16 usPid;
} VA_ECM_CHANNEL_t;

typedef struct {
	VA_ECM_CHANNEL_t *pEcmChannel;
	HUINT32 ulEcmLength;
	HUINT8 aucEcm[kVA_PVR_ECM_MAX_SIZE];
	HUINT32 ulTimestamp;
} VA_ECM_DATA_t;

void VA_PVR_Init();
void VA_PVR_Exit();

void VA_PVR_SendEcmByPid(HUINT32 ulAcsId, HUINT16 usPid);
void VA_PVR_SetTimestamp(Handle_t hAction, HUINT32 ulCurrentTime);
void VA_PVR_SetRecordInfo(HUINT32 ulAcsId, HUINT32 ulStbStreamHandle);
void VA_PVR_SetPlaybackInfo(HUINT32 ulAcsId);

VA_PVR_FILE_t *VA_PVR_GetFileInfo(HUINT32 ulAcsId);
VA_PVR_FILE_t *VA_PVR_GetPbFileInfoByVaActionHandle(Handle_t hVaAction);
VA_PVR_FILE_t *VA_PVR_GetPbFileInfo(Handle_t hVaAction, HUINT32 ulContentsId, HBOOL bCreate);

void VA_PVR_OpenEcmFileRetry(VA_PVR_FILE_t *pFile, HUINT32 ulRetryCnt);
void VA_PVR_OpenEcmFile(HUINT32 ulAcsId, HBOOL bIsTimeshift);
void VA_PVR_CloseEcmFile(HUINT32 ulAcsId);

#endif //VA_PVR_API_H


