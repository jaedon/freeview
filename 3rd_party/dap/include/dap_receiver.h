/**
 * @file
 * dap_receiver.h<br>
 * <br>
 * DAP_Receiverが実装すべきIFを定義する。
 */

#ifndef DAP_RECEIVER_H_
#define DAP_RECEIVER_H_
#ifdef __cplusplus
extern "C" {
#endif
/* **************************************************************************
 * 戻り値（正常系）
 * ************************************************************************** */
/**
 * @def DAP_OK
 * 正常終了<br>
 */
#define DAP_OK                                               0

/* **************************************************************************
 * 戻り値（異常系）
 * ************************************************************************** */
/* ==========================================================================
 * サービス登録・機器登録
 * ========================================================================== */
/**
 * @defgroup UI サービス登録、登録一覧取得、削除、全削除(DAP->レコーダーシステム）
 * @{
 */
/**
 * @ingroup UI
 * @def DAP_ERROR_INVALID_SERVICEID
 * 無効なサービスID<br>
 */
#define DAP_ERROR_INVALID_SERVICEID                          2
/**
 * @ingroup UI
 * @def DAP_ERROR_AUTHENTICATION_REQUIRED
 * 要機器認証（機器認証継続IDが無効）<br>
 */
#define DAP_ERROR_AUTHENTICATION_REQUIRED                   12
/**
 * @ingroup UI
 * @def DAP_ERROR_NO_SERVICES_AVAILABLE
 * 利用可能なサービスがない<br>
 */
#define DAP_ERROR_NO_SERVICES_AVAILABLE                     13
/**
 * @ingroup UI
 * @def DAP_ERROR_INVALID_PASSWORD
 * 無効なパスワード<br>
 */
#define DAP_ERROR_INVALID_PASSWORD                          14
/**
 * @ingroup UI
 * @def DAP_ERROR_INVALID_VENDOR_KEY
 * 無効なベンダーKey<br>
 */
#define DAP_ERROR_INVALID_VENDOR_KEY                        24
/**
 * @ingroup UI
 * @def DAP_ERROR_INVALID_DEVICE_SPECIFIC_ID
 * 無効な機器特定ID<br>
 */
#define DAP_ERROR_INVALID_DEVICE_SPECIFIC_ID                25
/**
 * @ingroup UI
 * @def DAP_ERROR_STOP_CONNECTING_XMPP_SERVER
 * XMPPサーバへの自動接続停止指示<br>
 */
#define DAP_ERROR_STOP_CONNECTING_XMPP_SERVER               26
/**
 * @ingroup UI
 * @def DAP_ERROR_INVALID_HASHED_VENDOR_KEY
 * 無効なハッシュされたベンダーKey<br>
 */
#define DAP_ERROR_INVALID_HASHED_VENDOR_KEY                 30
/**
 * @ingroup UI
 * @def DAP_ERROR_INVALID_CRYPT_MESSAGE
 * 無効な暗号化されたメッセージ<br>
 */
#define DAP_ERROR_INVALID_CRYPT_MESSAGE                     31
/**
 * @ingroup UI
 * @def DAP_ERROR_INVALID_MAC_ADDRESS
 * 無効なMACアドレス<br>
 */
#define DAP_ERROR_INVALID_MAC_ADDRESS                       35
/**
 * @ingroup UI
 * @def DAP_ERROR_INVALID_PRODUCT_INFO
 * 無効な製品情報<br>
 */
#define DAP_ERROR_INVALID_PRODUCT_INFO                      39
/**
 * @ingroup UI
 * @def DAP_ERROR_ALREADY_RELATED_DEVICE
 * すでに紐付けられている<br>
 */
#define DAP_ERROR_ALREADY_RELATED_DEVICE                    40
/**
 * @ingroup UI
 * @def DAP_ERROR_INTERNAL_DB_ERROR
 * DBの内部エラー<br>
 */
#define DAP_ERROR_INTERNAL_DB_ERROR                        100
/**
 * @ingroup UI
 * @def DAP_ERROR_NOT_FOUND_SERVER
 * 接続先サーバーを見つけられない<br>
 */
#define DAP_ERROR_NOT_FOUND_SERVER                        1001
/**
 * @ingroup UI
 * @def DAP_ERROR_INVALID_CHECKDIGIT
 * 不正なチェックデジット<br>
 */
#define DAP_ERROR_INVALID_CHECKDIGIT                      1002
/**
 * @ingroup UI
 * @def DAP_ERROR_PERPETUITY_DATA_ACQUISITION_FAILURE
 * 永続データ取得失敗<br>
 */
#define DAP_ERROR_PERPETUITY_DATA_ACQUISITION_FAILURE     1003
/**
 * @ingroup UI
 * @def DAP_ERROR_UNKNOWN_ERROR
 * 原因不明のエラー<br>
 */
#define DAP_ERROR_UNKNOWN_ERROR                           9999

/* **************************************************************************
 * 上限値定義
 * ************************************************************************** */
/**
 * @ingroup UI
 * @def DAP_MAX_STRING_LENGTH
 * DapServiceInfoの中、最大桁数<br>
 * サービス名、制御機器名、制御機器ニックネーム<br>
 */
#define DAP_MAX_STRING_LENGTH                                     1025
/**
 * @ingroup UI
 * @def DAP_MAX_STARTDATE_LENGTH
 * #DapServiceInfoの中、登録日時の最大値桁数<br>
 */
#define DAP_MAX_STARTDATE_LENGTH 30
/**
 * @ingroup UI
 * @def DAP_MAX_SERVICE_LIST_CNT
 * #ServiceList取得の最大数<br>
 */
#define DAP_MAX_SERVICE_LIST_CNT 10

/* **************************************************************************
 * 構造体
 * ************************************************************************** */
/* ==========================================================================
 * サービス登録・機器登録
 * ========================================================================== */
/**
 * @ingroup UI
 * @brief サービスリスト情報。
 * 全PlatformIDの現在利用可能な全てのサービスリストを表す。<br>
 */
typedef struct DapServiceInfo {
    /**
     * DAP-ID (1~2147483647の整数値)<br>
     */
    int dapId;
    /**
     * PLATFORM-ID (001～499(401-499 は予備/テスト用)の整数値)<br>
     */
    int platformID;
    /**
     * サービス名 ※文字コードはUTF-8とし、null終端すること。<br>
     * #DAP_MAX_STRING_LENGTH以上データは切り捨てる。
     */
    char serviceName[DAP_MAX_STRING_LENGTH];
    /**
     * 制御機器名 ※文字コードはUTF-8とし、null終端すること。<br>
     * #DAP_MAX_STRING_LENGTH以上データは切り捨てる。
     */
    char ctrlDevName[DAP_MAX_STRING_LENGTH];
    /**
     * 制御機器ニックネーム ※文字コードはUTF-8とし、null終端すること。<br>
     * #DAP_MAX_STRING_LENGTH以上データは切り捨てる。
     */
    char ctrlDevNickname[DAP_MAX_STRING_LENGTH];
    /**
     * 紐付け登録日時 ※文字コードはUTF-8とし、null終端すること。<br>
     */
    char startDate[DAP_MAX_STARTDATE_LENGTH];
}DapServiceInfo;

/** @} */

/* ==========================================================================
 * レコーダーシステム状態
 * ========================================================================== */
/**
 * @defgroup SystemStatusIF_SYS_DAP ネットワーク接続イベント通知(DAP->レコーダーシステム）
 * @{
 */
/**
 * @ingroup SystemStatusIF_SYS_DAP
 * @def DAP_RECORDER_EVENT_CONNECT
 * 接続を表す定数。
 */
#define DAP_RECORDER_EVENT_CONNECT                        2200
/**
 * @ingroup SystemStatusIF_SYS_DAP
 * @def DAP_RECORDER_EVENT_DISCONNECT
 * 切断を表す定数。
 *
 */
#define DAP_RECORDER_EVENT_DISCONNECT                     2201

/** @} */

/* **************************************************************************
 * 関数
 * ************************************************************************** */
/* ==========================================================================
 * サービス登録、一覧表示、削除、全削除
 * ========================================================================== */
/**
 * @defgroup UI サービス登録、全削除(DAP->レコーダーシステム）
 * @{
 */
/**
 * サービス登録を受信するコールバック。<br>
 * <br>
 * レコーダーシステムは本関数を使用し、SOAPサーバに対して被制御機器登録を行う。<br>
 *
 * @param [in] password パスワード(10進16桁(最大17桁)) null終端すること。<br>
 * 文字コードはUTF-8とする。<br>
 * @param [out] pDapServiceInfo 登録完了したサービス情報<br>
 * @return 被制御機器登録の処理結果<br>
 * <ul>
 * <li>#DAP_OK
 * <li>#DAP_ERROR_AUTHENTICATION_REQUIRED
 * <li>#DAP_ERROR_INVALID_PASSWORD
 * <li>#DAP_ERROR_INVALID_VENDOR_KEY
 * <li>#DAP_ERROR_INVALID_DEVICE_SPECIFIC_ID
 * <li>#DAP_ERROR_INVALID_HASHED_VENDOR_KEY
 * <li>#DAP_ERROR_INVALID_CRYPT_MESSAGE
 * <li>#DAP_ERROR_INVALID_MAC_ADDRESS
 * <li>#DAP_ERROR_INVALID_PRODUCT_INFO
 * <li>#DAP_ERROR_ALREADY_RELATED_DEVICE
 * <li>#DAP_ERROR_INTERNAL_DB_ERROR
 * <li>#DAP_ERROR_NOT_FOUND_SERVER
 * <li>#DAP_ERROR_INVALID_CHECKDIGIT
 * <li>#DAP_ERROR_PERPETUITY_DATA_ACQUISITION_FAILURE
 * <li>#DAP_ERROR_UNKNOWN_ERROR
 * </ul>
 */
typedef int (*SetPasswordCallback)(
        char* password,
        DapServiceInfo* pDapServiceInfo);

/**
 * サービス登録を受信するコールバック関数を設定する。<br>
 * <br>
 * @param [in] callback * サービス登録を受信するコールバック関数。
 */
void SetPasswordCallbackListener(
        SetPasswordCallback callback);

/**
 * サービス登録一覧取得を受信するコールバック。<br>
 * <br>
 * レコーダーシステムは本関数を使用し、指定されたPLATFORM-IDに登録されているサービス登録一覧を取得する。<br>
 * ※pDapServiceInfoの領域はあらかじめレコーダーシステムにて確保すること。<br>
 *
 * @param [in] platformID PLATFORM-ID<br>
 * @param [out] pDapServiceInfo サービスリスト情報<br>
 * @param [in]  length サービスリストを取得可能な最大件数<br>
 * @return サービス登録一覧取得の処理結果<br>
 * <ul>
 * <li>#DAP_OK
 * <li>#DAP_ERROR_AUTHENTICATION_REQUIRED
 * <li>#DAP_ERROR_NO_SERVICES_AVAILABLE
 * <li>#DAP_ERROR_STOP_CONNECTING_XMPP_SERVER
 * <li>#DAP_ERROR_INTERNAL_DB_ERROR
 * <li>#DAP_ERROR_NOT_FOUND_SERVER
 * <li>#DAP_ERROR_INVALID_CHECKDIGIT
 * <li>#DAP_ERROR_PERPETUITY_DATA_ACQUISITION_FAILURE
 * <li>#DAP_ERROR_UNKNOWN_ERROR
 * </ul>
 */
typedef int (*GetServiceListCallback)(
        int platformID,
        DapServiceInfo* pDapServiceInfo,
        unsigned int length);

/**
 * サービス登録一覧取得を受信するコールバック関数を設定する。<br>
 * <br>
 * @param [in] callback * サービス登録一覧取得を受信するコールバック関数。
 */
void GetServiceListCallbackListener(
        GetServiceListCallback callback);

/**
 * サービス削除を受信するコールバック。<br>
 * <br>
 * レコーダーシステムは本関数を使用し、指定されたPLATFORM-ID、DAP-IDにて登録されているサービスの削除を行う。<br>
 *
 * @param [in] platformID PLATFORM-ID<br>
 * @param [in] dapId DAP-ID<br>
 * @return サービス削除の処理結果<br>
 * <ul>
 * <li>#DAP_OK
 * <li>#DAP_ERROR_INVALID_SERVICEID
 * <li>#DAP_ERROR_AUTHENTICATION_REQUIRED
 * <li>#DAP_ERROR_NO_SERVICES_AVAILABLE
 * <li>#DAP_ERROR_STOP_CONNECTING_XMPP_SERVER
 * <li>#DAP_ERROR_INTERNAL_DB_ERROR
 * <li>#DAP_ERROR_NOT_FOUND_SERVER
 * <li>#DAP_ERROR_INVALID_CHECKDIGIT
 * <li>#DAP_ERROR_PERPETUITY_DATA_ACQUISITION_FAILURE
 * <li>#DAP_ERROR_UNKNOWN_ERROR
 * </ul>
 */
typedef int (*DeleteServiceListCallback)(
        int platformID,
        int dapId);

/**
 * サービス削除を受信するコールバック関数を設定する。<br>
 * <br>
 * @param [in] callback * 削除を受信するコールバック関数。
 */
void DeleteServiceListCallbackListener(
        DeleteServiceListCallback callback);

/**
 * サービス全削除を受信するコールバック。<br>
 * <br>
 * レコーダーシステムは本関数を使用し、指定されたPLATFORM-IDに登録されているサービスを全て削除する。<br>
 *
 * @param [in] platformID PLATFORM-ID<br>
 * @return サービス全削除の処理結果<br>
 * <ul>
 * <li>#DAP_OK
 * <li>#DAP_ERROR_AUTHENTICATION_REQUIRED
 * <li>#DAP_ERROR_NO_SERVICES_AVAILABLE
 * <li>#DAP_ERROR_STOP_CONNECTING_XMPP_SERVER
 * <li>#DAP_ERROR_INTERNAL_DB_ERROR
 * <li>#DAP_ERROR_NOT_FOUND_SERVER
 * <li>#DAP_ERROR_INVALID_CHECKDIGIT
 * <li>#DAP_ERROR_PERPETUITY_DATA_ACQUISITION_FAILURE
 * <li>#DAP_ERROR_UNKNOWN_ERROR
 * </ul>
 */
typedef int (*DeleteAllServiceListCallback)(
        int platformID);

/**
 * サービス全削除を受信するコールバック関数を設定する。<br>
 * <br>
 * @param [in] callback * サービス全削除を受信するコールバック関数。
 */
void DeleteAllServiceListCallbackListener(
        DeleteAllServiceListCallback callback);

/** @} */

/* ==========================================================================
 * ネットワーク接続イベント通知
 * ========================================================================== */
/**
 * @defgroup SystemStatusIF_SYS_DAP ネットワーク接続イベント通知(DAP->レコーダーシステム）
 * @{
 */

/**
 * レコーダーシステムのネットワーク接続イベント通知を受信するコールバック。<br>
 * <br>
 * レコーダーシステムは本関数を使用し、ネットワークへの接続、切断のイベントをDAPに行う。<br>
 *
 * @param [in] type イベント種別。
 * <ul>
 * <li>#DAP_RECORDER_EVENT_CONNECT:		接続<br>
 * ネットワーク接続可能な状態になった場合。
 * <li>#DAP_RECORDER_EVENT_DISCONNECT:	切断<br>
 * ネットワーク接続不可の状態になった場合。
 * </ul>
 */
typedef void (*NotifyRecorderConnectionEventCallback)(
        int type);

/**
 * ネットワーク接続イベント通知受信のコールバック関数を設定する。<br>
 * <br>
 * @param [in] callback ネットワーク接続イベント通知を受信するコールバック関数。
 */
void NotifyRecorderConnectionEventCallbackListener(
        NotifyRecorderConnectionEventCallback callback);

/** @} */

/* ==========================================================================
 * DAP終了処理
 * ========================================================================== */
/**
 * @defgroup Terminate DAP終了処理(DAP->レコーダーシステム）
 * @{
 */

/**
 * DAP終了を受信するコールバック。<br>
 * <br>
 * レコーダーシステムは終了する際、本関数を使用しDAPの終了処理をしなければならない。<br>
 *
 */
typedef void (*TerminateCallback)(void);

/**
 * DAP終了を受信するコールバック関数を設定する。<br>
 * <br>
 * @param [in] callback * DAP終了を受信するコールバック関数。
 */
void TerminateCallbackListener(
        TerminateCallback callback);

/** @} */

/* ==========================================================================
 * データ受信開始処理
 * ========================================================================== */
/**
 * @defgroup StartRecieveData データ受信開始処理(DAP->レコーダーシステム）
 * @{
 */

/**
 * レコーダーシステムはデータ受信を開始する。<br>
 * <br>
 * レコーダーシステムは本関数内でデータ受信用のスレッドを開始しなければならない。
 */
void StartReceiveData(void);

/** @} */
#ifdef __cplusplus
}
#endif
#endif /* DAP_RECEIVER_H_ */

