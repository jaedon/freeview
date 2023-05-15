#ifndef _WRAPPER_COMMON_H_
#define _WRAPPER_COMMON_H_

#include "dap_porting.h"
#include "dap_receiver.h"

#define APP_MSG_QUE_KEY 3333
#define REC_MSG_QUE_KEY 7777

/* define */
#define DAP_MAX_C_PRODUCT_INFO_LENGTH                    65      //製品情報の最大値
#define DAP_MAX_C_DEVICE_SPECIFIC_ID_LENGTH              65      //機器特定IDの最大値
#define DAP_MAX_C_AUTHENTICATEID_LENGTH                  65      //機器認証継続IDの最大値
#define MAX_DAP_C_BASE_DOMAIN                            15      //DAP ベースドメインの最大値
#define MAX_C_SERVICE_PROFILE_URN                        66      //Service Profile URNの最大値
#define MAX_C_SOAP_SERVER_URI_PORT                        7      //SOAPServerURI Portの最大値
#define MAX_C_SOAP_SERVER_URI_PATH                      256      //SOAPServerURI Pathの最大値
#define MAX_C_VENDERKEY                                  65      //VendorKeyの最大値
#define MAX_C_PRODUCT_NAME                               32      //製品名の最大値
#define MAX_C_MAC_ADDRESS                                18      //MACアドレスの最大値
#define MAX_C_RECORDER_STATUS_LENGTH                      1      //システム状態の要素の最大値

/* 信号ID */
/**********************************************************/
/* DAP -> REC                                             */
/**********************************************************/
/* Dap Porting */
/* データI/O（DAP->レコーダーシステム）*/
#define REQ_GET_SERVER_BASE_DOMAIN                  0x0001  /* ベースドメイン取得要求 *//* APP データなしサンプル */
#define REQ_GET_SERVICE_PROFILE_URN                 0x0002  /* Service Profile URN取得要求 */
#define REQ_GET_SOAP_SERVER_URI_PORT                0x0003  /* SOAPサーバアクセス先URI(Port)取得要求 */
#define REQ_GET_SOAP_SERVER_URI_PATH                0x0004  /* SOAPサーバアクセス先URI(Path)取得要求 */
#define REQ_GET_VENDER_KEY                          0x0005  /* ベンダーKey取得要求 */
#define REQ_GET_SSL_ROOT_CERTIFICATE_LENGTH         0x0006  /* SSLルート証明書長取得要求 */
#define REQ_GET_SSL_ROOT_CERTIFICATE                0x0007  /* SSLルート証明書取得要求 */
#define REQ_GET_PRODUCT_NAME                        0x0008  /* 製品名取得要求 */
#define REQ_GET_PRODUCT_INFO                        0x0009  /* 製品情報取得要求 */
#define REQ_GET_ALL_PLATFORM_ID_LENGTH              0x000A  /* 全PlatformID数取得要求 */
#define REQ_GET_ALL_PLATFORM_ID                     0x000B  /* 全PlatformID情報取得要求 */
#define REQ_ADD_PLATFORM_ID                         0x000C  /* PlatformID設定要求 */ /* APP データ有り */
#define REQ_DELETE_PLATFORM_ID                      0x000D  /* PlatformID削除要求 */
#define REQ_GET_MAC_ADDRESS                         0x000E  /* MACアドレス取得要求 */
#define REQ_GET_DEVICE_SPECIFIC_ID                  0x000F  /* 機器特定ID取得要求 */
#define REQ_SET_DEVICE_SPECIFIC_ID                  0x0010  /* 機器特定ID設定要求 */
#define REQ_GET_AUTHENTICATE_ID                     0x0011  /* 機器認証継続ID取得要求 */
#define REQ_SET_AUTHENTICATE_ID                     0x0012  /* 機器認証継続ID設定要求 */
#define REQ_GET_INITIALWAIT_TIME                    0x0013  /* 初回接続待ち時間取得要求 */
#define REQ_SET_INITIALWAIT_TIME                    0x0014  /* 初回接続待ち時間設定要求 */
#define REQ_GET_RECONNECT_WAIT_TIME                 0x0015  /* 再接続待ち時間取得要求 */
#define REQ_SET_RECONNECT_WAIT_TIME                 0x0016  /* 再接続待ち時間設定要求 */
#define REQ_GET_AUTHENTICATE_ID_EXPIRATION          0x0017  /* 機器認証継続ID有効期限取得要求 */
#define REQ_SET_AUTHENTICATE_ID_EXPIRATION          0x0018  /* 機器認証継続ID有効期限設定要求 */
/* 録画予約（DAP->レコーダーシステム）*/
#define REQ_CREATE_RECORD_SCHEDULE                  0x0019  /* 録画予約要求 */
#define REQ_GET_CONFLICT_LIST                       0x001A  /* 重複リスト取得要求 */
#define REQ_DELETE_RECORD_SCHEDULE                  0x001B  /* 録画予約削除要求 */
#define REQ_GET_RECORD_SCHEDULE_LIST_LENGTH         0x001C  /* 予約リスト検索結果数取得要求 */
#define REQ_GET_RECORD_SCHEDULE_LIST                0x001D  /* 予約リスト検索結果取得要求 */
#define REQ_UPDATE_RECORD_SCHEDULE                  0x001E  /* 録画予約更新要求 */
#define REQ_GET_TITLE_LIST_LENGTH                   0x001F  /* タイトルリスト検索結果数取得要求 */
#define REQ_GET_TITLE_LIST                          0x0020  /* タイトルリスト検索結果取得要求 */
#define REQ_DELETE_TITLE                            0x0021  /* タイトル削除要求 */
#define REQ_UPDATE_TITLE                            0x0022  /* タイトル更新要求 */
/* レコーダーシステム状態取得（DAP->レコーダーシステム）*/
#define REQ_DAP_GET_RECORDER_STATUS                 0x0023  /* レコーダーシステム要求受付状態取得要求 */
/* Dap Receiver */
/* データ受信開始処理(DAP->レコーダーシステム）*/
#define REQ_START_RECEIVE_DATA                      0x0024  /* データ受信開始要求 */
/* Dap Receiver */
/* サービス登録、全削除(DAP->レコーダーシステム）*/
#define REQ_SET_PASSWORD                            0x0025 /* サービス登録コールバック登録依頼  */
#define REQ_GET_SERVICE_LIST                        0x0026 /* サービス登録一覧取得コールバック登録依頼  */
#define REQ_DELETE_SERVICE_LIST                     0x0027 /* サービス削除コールバック登録依頼  */
#define REQ_DELETE_ALL_SERVICE_LIST                 0x0028 /* サービス全削除コールバック登録依頼  */
/* ネットワーク接続イベント通知(DAP->レコーダーシステム）*/
#define REQ_NOTIFY_RECORDER_CONNECTION_EVENT        0x0029 /* ネットワークサービスイベントコールバック登録依頼  */
/* DAP終了処理(DAP->レコーダーシステム）*/
#define REQ_TERMINATE                               0x002A /* DAP終了コールバック登録依頼 */

/**********************************************************/
/* REC -> DAP                                             */
/**********************************************************/
/* Dap Porting */
/* データI/O */
#define RES_GET_SERVER_BASE_DOMAIN                  0x0100 /* ベースドメイン取得応答 */
#define RES_GET_SERVICE_PROFILE_URN                 0x0200 /* Service Profile URN取得応答 */
#define RES_GET_SOAP_SERVER_URI_PORT                0x0300 /* SOAPサーバアクセス先URI(Port)取得応答 */
#define RES_GET_SOAP_SERVER_URI_PATH                0x0400 /* SOAPサーバアクセス先URI(Path)取得応答 */
#define RES_GET_VENDER_KEY                          0x0500 /* ベンダーKey取得応答 */
#define RES_GET_SSL_ROOT_CERTIFICATE_LENGTH         0x0600 /* SSLルート証明書長取得応答 */
#define RES_GET_SSL_ROOT_CERTIFICATE                0x0700 /* SSLルート証明書取得応答 */
#define RES_GET_PRODUCT_NAME                        0x0800 /* 製品名取得応答 */
#define RES_GET_PRODUCT_INFO                        0x0900 /* 製品情報取得応答 */
#define RES_GET_ALL_PLATFORM_ID_LENGTH              0x0A00 /* 全PlatformID数取得応答 */
#define RES_GET_ALL_PLATFORM_ID                     0x0B00 /* 全PlatformID情報取得応答 */
#define RES_ADD_PLATFORM_ID                         0x0C00 /* PlatformID設定応答 */
#define RES_DELETE_PLATFORM_ID                      0x0D00 /* PlatformID削除応答 */
#define RES_GET_MAC_ADDRESS                         0x0E00 /* MACアドレス取得応答 */
#define RES_GET_DEVICE_SPECIFIC_ID                  0x0F00 /* 機器特定ID取得応答 */
#define RES_SET_DEVICE_SPECIFIC_ID                  0x1000 /* 機器特定ID設定応答 */
#define RES_GET_AUTHENTICATE_ID                     0x1100 /* 機器認証継続ID取得応答 */
#define RES_SET_AUTHENTICATE_ID                     0x1200 /* 機器認証継続ID設定応答 */
#define RES_GET_INITIALWAIT_TIME                    0x1300 /* 初回接続待ち時間取得応答 */
#define RES_SET_INITIALWAIT_TIME                    0x1400 /* 初回接続待ち時間設定応答 */
#define RES_GET_RECONNECT_WAIT_TIME                 0x1500 /* 再接続待ち時間取得応答 */
#define RES_SET_RECONNECT_WAIT_TIME                 0x1600 /* 再接続待ち時間設定応答 */
#define RES_GET_AUTHENTICATE_ID_EXPIRATION          0x1700 /* 機器認証継続ID有効期限取得応答 */
#define RES_SET_AUTHENTICATE_ID_EXPIRATION          0x1800 /* 機器認証継続ID有効期限設定応答 */
/* 録画予約（DAP->レコーダーシステム）*/
#define RES_CREATE_RECORD_SCHEDULE                  0x1900 /* 録画予約応答 */
#define RES_GET_CONFLICT_LIST                       0x1A00 /* 重複リスト取得応答 */
#define RES_DELETE_RECORD_SCHEDULE                  0x1B00 /* 録画予約削除応答 */
#define RES_GET_RECORD_SCHEDULE_LIST_LENGTH         0x1C00 /* 予約リスト検索結果数取得応答 */
#define RES_GET_RECORD_SCHEDULE_LIST                0x1D00 /* 予約リスト検索結果取得応答 */
#define RES_UPDATE_RECORD_SCHEDULE                  0x1E00 /* 録画予約更新応答 */
#define RES_GET_TITLE_LIST_LENGTH                   0x1F00 /* タイトルリスト検索結果数取得応答 */
#define RES_GET_TITLE_LIST                          0x2000 /* タイトルリスト検索結果取得応答 */
#define RES_DELETE_TITLE                            0x2100 /* タイトル削除応答 */
#define RES_UPDATE_TITLE                            0x2200 /* タイトル更新応答 */
/* レコーダーシステム状態取得（DAP->レコーダーシステム）*/
#define RES_DAP_GET_RECORDER_STATUS                 0x2300 /* レコーダーシステム要求受付状態取得応答 */
/* Dap Receiver */
/* データ受信開始処理(DAP->レコーダーシステム）*/
#define RES_START_RECEIVE_DATA                      0x2400 /* データ受信開始応答 */
/* サービス登録、全削除(DAP->レコーダーシステム）*/
#define IND_SET_PASSWORD                            0x2500 /* サービス登録通知 */ /* 通知 データ有り */
#define IND_GET_SERVICE_LIST                        0x2600 /* サービス登録一覧取得コールバック登録依頼  */
#define IND_DELETE_SERVICE_LIST                     0x2700 /* サービス削除コールバック登録依頼  */
#define IND_DELETE_ALL_SERVICE_LIST                 0x2800 /* サービス全削除通知 */ /* 通知 データなし */
/* ネットワーク接続イベント通知(DAP->レコーダーシステム）*/
#define IND_NOTIFY_RECORDER_CONNECTION_EVENT        0x2900 /* ネットワークサービスイベント通知 */
/* DAP終了処理(DAP->レコーダーシステム）*/
#define IND_TERMINATE                               0x2A00 /* DAP終了通知 */


#define MSG_LEN_MAX  2500

#define PLATFORM_ID_ARRAY_MAX 499

/* メッセージキュー用 */
typedef struct MsgBuf
{
    long int type;
    char data[MSG_LEN_MAX];
}MsgBuf;

/* メッセージキュー信号ヘッダ */
typedef struct MsgHeadder
{
    unsigned short msg_id;
    unsigned int request_id;
    int result;
}MsgHeadder;

/* キュー信号用構造体 */
typedef struct msg_SHM_IN1                                          /* 共通 インパラ用共有メモリ1個 */
{
    int in_shm_id;
}msg_SHM_IN1;

typedef struct msg_SHM_IND                                          /* 共通 コールバック用共有メモリ1個 */
{
    int shm_id;
}msg_SHM_IND;

typedef struct msg_SHM_IN1_OUT1                                     /* 共通 インパラ、アウトパラ用共有メモリ各1個 */
{
    int in_shm_id;
    int out_shm_id;
}msg_SHM_IN1_OUT1;

typedef struct msg_REQ_IN_LENGTH                                    /* 共通 要求時のインパラが長さのみのもの用 */
{
    int length;
}msg_REQ_IN_LENGTH;

typedef struct msg_REQ_GET_SSL_ROOT_CERTIFICATE                     /* SSLルート証明書取得要求用 */
{
    int out_shm_id;
    unsigned int length;
}msg_REQ_GET_SSL_ROOT_CERTIFICATE;

typedef struct msg_REQ_ADD_PLATFORM_ID                              /*  PlatformID設定要求用 */
{
    int platformID;
}msg_REQ_ADD_PLATFORM_ID;

typedef struct msg_REQ_DELETE_PLATFORM_ID                           /* PlatformID削除要求用 */
{
    int platformID;
}msg_REQ_DELETE_PLATFORM_ID;

typedef struct msg_REQ_GET_DEVICE_SPECIFIC_ID                       /* 機器特定ID取得要求用 */
{
    int length;
    int platformID;
}msg_REQ_GET_DEVICE_SPECIFIC_ID;

typedef struct msg_REQ_SET_DEVICE_SPECIFIC_ID                       /* 機器特定ID設定要求用 */
{
    int platformID;
    char deviceSpecificID[DAP_MAX_C_DEVICE_SPECIFIC_ID_LENGTH];
}msg_REQ_SET_DEVICE_SPECIFIC_ID;

typedef struct msg_REQ_GET_AUTHENTICATE_ID                          /* 機器認証継続ID取得要求用 */
{
    int length;
    int platformID;
}msg_REQ_GET_AUTHENTICATE_ID;

typedef struct msg_REQ_SET_AUTHENTICATE_ID                          /* 機器認証継続ID設定要求用 */
{
    int platformID;
    char authenticateID[DAP_MAX_C_AUTHENTICATEID_LENGTH];
}msg_REQ_SET_AUTHENTICATE_ID;

typedef struct msg_REQ_GET_INITIALWAIT_TIME                         /* 初回接続待ち時間取得要求用 */
{
    int platformID;
}msg_REQ_GET_INITIALWAIT_TIME;

typedef struct msg_REQ_SET_INITIALWAIT_TIME                         /* 初回接続待ち時間設定要求用 */
{
    int platformID;
    int waitTime;
}msg_REQ_SET_INITIALWAIT_TIME;

typedef struct msg_REQ_GET_RECONNECT_WAIT_TIME                      /* 再接続待ち時間取得要求用 */
{
    int platformID;
}msg_REQ_GET_RECONNECT_WAIT_TIME;

typedef struct msg_REQ_SET_RECONNECT_WAIT_TIME                      /* 再接続待ち時間設定要求用 */
{
    int platformID;
    int waitTime;
}msg_REQ_SET_RECONNECT_WAIT_TIME;

typedef struct msg_REQ_GET_AUTHENTICATE_ID_EXPIRATION               /* 機器認証継続ID有効期限取得要求用 */
{
    int platformID;
    unsigned int length;
}msg_REQ_GET_AUTHENTICATE_ID_EXPIRATION;

typedef struct msg_REQ_SET_AUTHENTICATE_ID_EXPIRATION               /* 機器認証継続ID有効期限設定要求用 */
{
    int platformID;
    char expiration[DAP_MAX_DATETIME_LENGTH];
}msg_REQ_SET_AUTHENTICATE_ID_EXPIRATION;

/* 録画予約（DAP->レコーダーシステム） */
typedef struct msg_REQ_GET_CONFLICT_LIST                            /* 重複リスト取得要求用 */
{
    int in_shm_id;
    int out_shm_id;
}msg_REQ_GET_CONFLICT_LIST;

typedef struct msg_REQ_DELETE_RECORD_SCHEDULE                       /* 録画予約削除要求用 */
{
    char recordScheduleID[DAP_MAX_PROGRAMID_LENGTH];
}msg_REQ_DELETE_RECORD_SCHEDULE;

typedef struct msg_REQ_GET_RECORD_SCHEDULE_LIST_LENGTH              /* 予約リスト検索結果数取得要求用 */
{
    DapSearchInfo dapListInParamInfo;
}msg_REQ_GET_RECORD_SCHEDULE_LIST_LENGTH;

typedef struct msg_REQ_GET_RECORD_SCHEDULE_LIST                     /* 予約リスト検索結果取得要求用 */
{
    int out_shm_id_program_info;
    unsigned int length;
}msg_REQ_GET_RECORD_SCHEDULE_LIST;

typedef struct msg_REQ_GET_TITLE_LIST_LENGTH                        /* タイトルリスト検索結果数取得要求用 */
{
    DapSearchInfo dapListInParamInfo;
}msg_REQ_GET_TITLE_LIST_LENGTH;

typedef struct msg_REQ_GET_TITLE_LIST                               /* タイトルリスト検索結果取得要求用 */
{
    int out_shm_id_program_info;
    unsigned int length;
}msg_REQ_GET_TITLE_LIST;

typedef struct msg_REQ_DELETE_TITLE                                 /* タイトル削除要求用 */
{
    char title[DAP_MAX_STRING_LENGTH];
}msg_REQ_DELETE_TITLE;

/* 応答 */
typedef struct msg_RES_GET_SERVER_BASE_DOMAIN                       /* ベースドメイン取得応答用 */
{
    char base_domain[MAX_DAP_C_BASE_DOMAIN];
}msg_RES_GET_SERVER_BASE_DOMAIN;

typedef struct msg_RES_GET_SERVICE_PROFILE_URN                      /* Service Profile URN取得応答用 */
{
    char serviceProfileURN[MAX_C_SERVICE_PROFILE_URN];
}msg_RES_GET_SERVICE_PROFILE_URN;

typedef struct msg_RES_GET_SOAP_SERVER_URI_PORT                     /* SOAPサーバアクセス先URI(Port)取得応答用 */
{
    char port[MAX_C_SOAP_SERVER_URI_PORT];
}msg_RES_GET_SOAP_SERVER_URI_PORT;

typedef struct msg_RES_GET_SOAP_SERVER_URI_PATH                     /* SOAPサーバアクセス先URI(Path)取得応答用 */
{
    char path[MAX_C_SOAP_SERVER_URI_PATH];
}msg_RES_GET_SOAP_SERVER_URI_PATH;

typedef struct msg_RES_GET_VENDER_KEY                               /* ベンダーKey取得応答用 */
{
    char venderkey[MAX_C_VENDERKEY];
}msg_RES_GET_VENDER_KEY;

typedef struct msg_RES_GET_SSL_ROOT_CERTIFICATE_LENGTH              /* SSLルート証明書長取得応答用 */
{
    unsigned int length;
}msg_RES_GET_SSL_ROOT_CERTIFICATE_LENGTH;

typedef struct msg_RES_GET_SSL_ROOT_CERTIFICATE                     /* SSLルート証明書取得応答用 */
{
    int out_shm_id;
    unsigned int length;
}msg_RES_GET_SSL_ROOT_CERTIFICATE;

typedef struct msg_RES_GET_PRODUCT_NAME                             /* 製品名取得応答用 */
{
    char productName[MAX_C_PRODUCT_NAME];
}msg_RES_GET_PRODUCT_NAME;

typedef struct msg_RES_GET_PRODUCT_INFO                             /* 製品情報取得応答用 */
{
    char productInfo[DAP_MAX_C_PRODUCT_INFO_LENGTH];
}msg_RES_GET_PRODUCT_INFO;

typedef struct msg_RES_GET_ALL_PLATFORM_ID_LENGTH                   /* 全PlatformID数取得応答用 */
{
    int length;
}msg_RES_GET_ALL_PLATFORM_ID_LENGTH;

typedef struct msg_RES_GET_ALL_PLATFORM_ID                          /* 全PlatformID情報取得応答用 */
{
    unsigned short count;
    int platformID[PLATFORM_ID_ARRAY_MAX];
}msg_RES_GET_ALL_PLATFORM_ID;

typedef struct msg_RES_GET_MAC_ADDRESS                              /* MACアドレス取得応答用 */
{
    char macAddress[MAX_C_MAC_ADDRESS];
}msg_RES_GET_MAC_ADDRESS;

typedef struct msg_RES_GET_DEVICE_SPECIFIC_ID                       /* 機器特定ID取得応答用 */
{
    char deviceSpecificID[DAP_MAX_C_DEVICE_SPECIFIC_ID_LENGTH];
}msg_RES_GET_DEVICE_SPECIFIC_ID;

typedef struct msg_RES_GET_AUTHENTICATE_ID                          /* 機器認証継続ID取得応答用 */
{
    char authenticateID[DAP_MAX_C_AUTHENTICATEID_LENGTH];
}msg_RES_GET_AUTHENTICATE_ID;

typedef struct msg_RES_GET_INITIALWAIT_TIME                         /* 初回接続待ち時間取得応答用 */
{
    int waitTime;
}msg_RES_GET_INITIALWAIT_TIME;

typedef struct msg_RES_GET_RECONNECT_WAIT_TIME                      /* 再接続待ち時間取得応答用 */
{
    int waitTime;
}msg_RES_GET_RECONNECT_WAIT_TIME;

typedef struct msg_RES_GET_AUTHENTICATE_ID_EXPIRATION               /* 機器認証継続ID有効期限取得応答用 */
{
    char expiration[DAP_MAX_STARTDATE_LENGTH];
}msg_RES_GET_AUTHENTICATE_ID_EXPIRATION;

typedef struct msg_RES_GET_CONFLICT_LIST_LENGTH                     /* 重複リスト数取得応答用 */
{
    unsigned int length;
}msg_RES_GET_CONFLICT_LIST_LENGTH;

typedef struct msg_RES_GET_RECORD_SCHEDULE_LIST_LENGTH              /* 予約リスト検索結果数取得応答用 */
{
    unsigned int length;
}msg_RES_GET_RECORD_SCHEDULE_LIST_LENGTH;

typedef struct msg_RES_GET_RECORD_SCHEDULE_LIST                     /* 予約リスト検索結果取得応答用 */
{
    DapSearchResult dapSearchResult;
}msg_RES_GET_RECORD_SCHEDULE_LIST;

typedef struct msg_RES_GET_TITLE_LIST_LENGTH                        /* タイトルリスト検索結果数取得応答用 */
{
    unsigned int length;
}msg_RES_GET_TITLE_LIST_LENGTH;

typedef struct msg_RES_GET_TITLE_LIST                               /* タイトルリスト検索結果取得応答用 */
{
    DapSearchResult dapSearchResult;
}msg_RES_GET_TITLE_LIST;

typedef struct msg_RES_DAP_GET_RECORDER_STATUS                      /* レコーダーシステム要求受付状態取得応答用 */
{
    int status[MAX_C_RECORDER_STATUS_LENGTH];
    int wait_time;
}msg_RES_DAP_GET_RECORDER_STATUS;


/* 通知 */
typedef struct msg_IND_SET_PASSWORD
{
    char passwd[18];
}msg_IND_SET_PASSWORD;

typedef struct msg_IND_GET_SERVICE_LIST
{
    int platformID;
    unsigned int length;
}msg_IND_GET_SERVICE_LIST;

typedef struct msg_IND_DELETE_SERVICE_LIST
{
    int platformID;
    int dap_id;
}msg_IND_DELETE_SERVICE_LIST;

typedef struct msg_IND_DELETE_ALL_SERVICE_LIST
{
    int platformID;
}msg_IND_DELETE_ALL_SERVICE_LIST;

typedef struct msg_IND_NOTIFY_RECORDER_CONNECTION_EVENT
{
    int type;
}msg_IND_NOTIFY_RECORDER_CONNECTION_EVENT;

typedef struct shm_IND_SET_PASSWORD
{
    int flag;
    int result;
    DapServiceInfo dapOutServiceInfo;
} shm_IND_SET_PASSWORD;

typedef struct shm_IND_GET_SERVICE_LIST
{
    int flag;
    int result;
    DapServiceInfo dapOutServiceInfo[DAP_MAX_SERVICE_LIST_CNT];
} shm_IND_GET_SERVICE_LIST;

#endif
