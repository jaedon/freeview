/**
 * @file
 * dap_porting.h<br>
 * <br>
 * レコーダーシステムが実装すべきIFを定義する。
 */

#include <time.h>
#include <stdbool.h>

#ifndef _DAP_PORTING_H_
#define _DAP_PORTING_H_
#ifdef __cplusplus
extern "C" {
#endif

/*
 * [[データ型]]
 */
#define VOID		void
#define BYTE		char
#define BOOLEAN		bool
#define CHAR		char
#define SHORT		short
#define INT			int
#define LONG		long
#define FLOAT		float
#define DOUBLE		double

#define FALSE	0
#define TRUE	1

/* **************************************************************************
 * 上限値定義
 * ************************************************************************** */
/**
 * @ingroup RecordScheduleIF
 * @def DAP_MAX_SEARCH_LENGTH
 * #DapSearchInfoの中、検索条件の最大値<br>
 */
#define DAP_MAX_SEARCH_LENGTH                               1052
/**
 * @ingroup RecordScheduleIF
 * @def DAP_MAX_STRING_LENGTH
 * #DapProgramInfoの中、以下データの最大値<br>
 * 番組タイトル名、SI情報ID、SI情報種別、録画モード、ジャンルID、重複コード、メディア残量アラートコード<br>
 */
#define DAP_MAX_STRING_LENGTH                                     1025
/**
 * @ingroup RecordScheduleIF
 * #DapProgramInfoの中、番組情報IDの最大値<br>
 * DAP_MAX_PROGRAMID_LENGTH<br>
 */
#define DAP_MAX_PROGRAMID_LENGTH                             260
/**
 * @ingroup RecordScheduleIF
 * @def DAP_MAX_FILTER_LENGTH
 * #DapSearchInfoの中、フィルターの最大値<br>
 */
#define DAP_MAX_FILTER_LENGTH                                240
/**
 * @ingroup RecordScheduleIF
 * @def DAP_MAX_SORT_LENGTH
 * #DapSearchInfoの中、ソート条件の最大値<br>
 */
#define DAP_MAX_SORT_LENGTH                                   24
/**
 * @ingroup RecordScheduleIF
 * @def DAP_MAX_CHANNELID_LENGTH
 * #DapProgramInfoの中、チャンネルIDの最大値<br>
 */
#define DAP_MAX_CHANNELID_LENGTH                              20
/**
 * @ingroup RecordScheduleIF
 * @def DAP_MAX_TZD_LENGTH
 * #DapProgramInfoの中、録画開始日時のタイムゾーンの最大値<br>
 */
#define DAP_MAX_TZD_LENGTH                               8
/**
 * @ingroup RecordScheduleIF
 * @def DAP_MAX_CONDITIONID_LENGTH
 * #DapProgramInfoの中、録画設定条件コードの最大値<br>
 */
#define DAP_MAX_CONDITIONID_LENGTH                               8

/**
 * @ingroup RecordScheduleIF
 * @def DAP_MAX_DATETIME_LENGTH
 * #DapProgramInfoの中、録画開始日時の最大値桁数<br>
 */
#define DAP_MAX_DATETIME_LENGTH 30

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
 * 録画予約
 * ========================================================================== */
/**
 * @ingroup RecordScheduleIF
 * @def DAP_ERROR_INVALID_VALUE
 * チャンネルが特定できない、または録画できない番組が指定された場合等のエラー<br>
 */
#define DAP_ERROR_INVALID_VALUE                            803
/**
 * @ingroup RecordScheduleIF
 * @def DAP_ERROR_NO_SUCH_RECORDSCHEDULEID
 * 該当する録画予約IDが存在しない場合のエラー
 */
#define DAP_ERROR_NO_SUCH_RECORDSCHEDULEID                 804
/**
 * @ingroup RecordScheduleIF
 * @def DAP_ERROR_RECORDSCHEDULE_ACTIVELY_RECCORDING
 * 対象の録画予約が現在録画中の為に支障をきたす場合のエラー
 */
#define DAP_ERROR_RECORDSCHEDULE_ACTIVELY_RECCORDING       805
/**
 * @ingroup RecordScheduleIF
 * @def DAP_ERROR_UNSUPPORTED_OR_INVALID_SORT_CRITERIA
 * 検索条件のソート条件指定に不備がある場合のエラー
 */
#define DAP_ERROR_UNSUPPORTED_OR_INVALID_SORT_CRITERIA     809
/**
 * @ingroup RecordScheduleIF
 * @def DAP_ERROR_CANNOT_PROCCES_REQUEST
 * 被制御機器がLowCpuMode状態等で処理できない場合のエラー
 */
#define DAP_ERROR_CANNOT_PROCCES_REQUEST                   820
/**
 * @ingroup RecordScheduleIF
 * @def DAP_ERROR_CONFLICT
 * 投入した予約が被制御機器側の既存の予約と重複している場合のエラー
 */
#define DAP_ERROR_CONFLICT                                 830
/**
 * @ingroup RecordScheduleIF
 * @def DAP_ERROR_PROTECTED_CONTENTS
 * 視聴制限、コピーコントロール関連に触れた場合のエラー
 */
#define DAP_ERROR_PROTECTED_CONTENTS                       831
/**
 * @ingroup RecordScheduleIF
 * @def DAP_ERROR_TOO_MANY_RECORD_SCHEDULES
 * 予約数の上限値を超えるため予約投入できない場合のエラー
 */
#define DAP_ERROR_TOO_MANY_RECORD_SCHEDULES                836
/**
 * @ingroup RecordScheduleIF
 * @def DAP_ERROR_DIRECCT_RECORDING
 * 被制御機器本体でのダイレクト録画の影響で支障がでる場合のエラー
 */
#define DAP_ERROR_DIRECCT_RECORDING                        841
/**
 * @ingroup RecordScheduleIF
 * @def DAP_ERROR_CANNOT_DELETE_SPECIFIC_RECORED_SCHEDULE
 * 対象の録画予約が削除禁止となっている場合のエラー
 */
#define DAP_ERROR_CANNOT_DELETE_SPECIFIC_RECORED_SCHEDULE  844
/**
 * @ingroup RecordScheduleIF
 * @def DAP_ERROR_SYSTEM_STARTING_UP
 * 被制御機器が予約受付可能な状態まで起動してない場合のエラー
 */
#define DAP_ERROR_SYSTEM_STARTING_UP                       845
/**
 * @ingroup RecordScheduleIF
 * @def DAP_ERROR_INVALID_VALUE_EDIT_RECORDSCHEDULE
 * 指定したプロパティに不備がある場合のエラー
 */
#define DAP_ERROR_INVALID_VALUE_EDIT_RECORDSCHEDULE        853
/**
 * @ingroup RecordScheduleIF
 * @def DAP_ERROR_INVALID_VALUE_EDIT_TITLE
 * タイトル修正に際し、不正な値が指定された場合のエラー
 */
#define DAP_ERROR_INVALID_VALUE_EDIT_TITLE                 873
/**
 * @ingroup RecordScheduleIF
 * @def DAP_ERROR_NO_SUCH_TITLEID
 * 該当するタイトルリスト取得で取得したタイトルIDが存在しない場合のエラー
 */
#define DAP_ERROR_NO_SUCH_TITLEID                          874
/**
 * @ingroup RecordScheduleIF
 * @def DAP_ERROR_TITLE_ACTIVELY_RECORDING
 * 対象のタイトルが録画中の場合のエラー
 */
#define DAP_ERROR_TITLE_ACTIVELY_RECORDING                 875
/**
 * @ingroup RecordScheduleIF
 * @def DAP_ERROR_TITLE_ACTIVELY_PLAYING
 * 対象のタイトルが再生中の場合のエラー
 */
#define DAP_ERROR_TITLE_ACTIVELY_PLAYING                   876
/**
 * @ingroup RecordScheduleIF
 * @def DAP_ERROR_TITLE_PROTECTED_BY_USER
 *対象のタイトルがプロテクトされている場合のエラー
 */
#define DAP_ERROR_TITLE_PROTECTED_BY_USER                  877
/**
 * @ingroup RecordScheduleIF
 * @def DAP_ERROR_CANNOT_DELETE_OR_EDIT_SPECIFIC_TITLE
 * 以下のエラー以外の理由でタイトル修正/削除できない場合のエラー
 * <ul>
 * <li>#DAP_ERROR_INVALID_VALUE_EDIT_TITLE
 * <li>#DAP_ERROR_NO_SUCH_TITLEID
 * <li>#DAP_ERROR_TITLE_ACTIVELY_RECORDING
 * <li>#DAP_ERROR_TITLE_ACTIVELY_PLAYING
 * <li>#DAP_ERROR_TITLE_PROTECTED_BY_USER
 * </ul>
 */
#define DAP_ERROR_CANNOT_DELETE_OR_EDIT_SPECIFIC_TITLE     878
/**
 * @ingroup RecordScheduleIF
 * @def DAP_ERROR_OTHER_ERROR_RECORD_SCHEDULE
 * 予約投入/重複リスト取得/予約削除/予約リスト取得/予約修正で発生した原因不明のエラー
 */
#define DAP_ERROR_OTHER_ERROR_RECORD_SCHEDULE              860
/**
 * @ingroup RecordScheduleIF
 * @def DAP_ERROR_OTHER_ERROR_ABOUT_TITLE
 * タイトルリスト取得/タイトル削除/タイトル修正で発生した原因不明のエラー
 */
#define DAP_ERROR_OTHER_ERROR_ABOUT_TITLE                  861

/* ==========================================================================
 * レコーダーシステム状態
 * ========================================================================== */
/**
 * @ingroup RecorderStatusIF_DAP_SYS
 * @def DAP_RECORDER_STATUS_RECEIVING
 * 受け付け可能状態を表す定数。<br>
 * レコーダーシステムが、DAPからの要求を受け付け可能な状態を表す。<br>
 * 例）電源ON状態、かつ起動が完了している状態<br>
 */
#define DAP_RECORDER_STATUS_RECEIVING                     2000
/**
 * @ingroup RecorderStatusIF_DAP_SYS
 * @def DAP_RECORDER_STATUS_PREPARING
 * 準備中状態を表す定数。<br>
 * レコーダーシステムが、DAPからの要求をすぐには受け付けできず、<br>
 * 一定時間経過すると受け付け可能な状態を表す。<br>
 * 例）起動中、スリープ状態、スタンバイ状態、省電力モード等<br>
 */
#define DAP_RECORDER_STATUS_PREPARING                     2001
/**
 * @ingroup RecorderStatusIF_DAP_SYS
 * @def DAP_RECORDER_STATUS_UNAVAILABLE
 * 一時利用不可状態を表す定数。<br>
 * レコーダーシステムが、DAPからの要求を受け付けできない状態を表す。<br>
 * 何らかの原因で要求が受け付けられず、かつ受け付け可能状態になるまでの時間が不明な状態である。<br>
 * 例）低CPUモード、メーカー固有の処理をしておりDAPからの要求が受け付けれない等<br>
 */
#define DAP_RECORDER_STATUS_UNAVAILABLE                   2002

/* **************************************************************************
 * 構造体
 * ************************************************************************** */
/* ==========================================================================
 * 録画予約
 * ========================================================================== */
/**
 * @ingroup RecordScheduleIF
 * @brief 番組情報。
 */
typedef struct DapProgramInfo {
    /**
     * 予約投入が成功した際に被制御機器側で発行される被制御機器内で一意のIDで、
     * 番組情報のインデックスの役割を担う。録画予約IDと同等。<br>
     * ※文字コードはUTF-8とし、null終端すること。<br>
     */
    char programID[DAP_MAX_PROGRAMID_LENGTH];
    /**
     * 番組タイトル名<br>
     * #DAP_MAX_STRING_LENGTH以上データは切り捨てる。
     * ※文字コードはUTF-8とし、null終端すること。<br>
     */
    char title[DAP_MAX_STRING_LENGTH];
    /**
     * 録画開始日時(YYYY-MM-DDTHH:MM:SS)<br>
     * #DAP_MAX_DATETIME_LENGTH以上データは切り捨てる。<br>
     */
    char schStartDateTime[DAP_MAX_DATETIME_LENGTH];
    /**
     * 録画開始日時のタイムゾーン (タイムゾーン指定子(+HHMM/-HHMM))<br>
     * 登録開始日時を指定する場合は、タイムゾーンを必ず指定すること。<br>
     * 指定しない場合は、null終端のみとする。<br>
     * ※文字コードはUTF-8とし、null終端すること。<br>
     */
    char schTzd[DAP_MAX_TZD_LENGTH];
    /**
     * 録画時間。<br>
     * 録画予約における録画時間、またはタイトル(録画済番組)の長さを表す。
     * (0~359999999(99999時間59分59秒)の秒数)<br>
     * 指定しない場合は-1とする。<br>
     */
    INT schDuration;
    /**
     * 録画設定条件コード。<br>
     * 録画予約の設定に関わる、毎系録画条件や被制御機器側機能に特化した録画条件を表す。<br>
     * ・1 - 通常の1回予約<br>
     * ・d - 毎日予約<br>
     * ・w1~w7 - 毎週月曜予約~毎週日曜予約<br>
     * ・w15 - 月~金予約<br>
     * ・w16 - 月~土予約<br>
     * ・4桁の半角英数字 - ベンダーにて別途定義可<br>
     * ・上記以外の1~3桁の半角英数字 - 必要に応じて定義<br>
     * ※文字コードはUTF-8とし、null終端すること。<br>
     */
    char schConditionID[DAP_MAX_CONDITIONID_LENGTH];
    /**
     * チャンネルID (4桁16進(接頭辞0x 、0フィルあり、Case Insensitive))<br>
     * ※文字コードはUTF-8とし、null終端すること。<br>
     */
    char schChannelID[DAP_MAX_CHANNELID_LENGTH];
    /**
     * チャンネル放送種別コード<br>
     * ・1 - 地上波アナログ<br>
     * ・2 - 地上波デジタル<br>
     * ・3 - BS<br>
     * ・4 - CS(br>
     * ・100 - 外部入力<br>
     * ・101 - 録画機器内部コンテンツ<br>
     * ・1000~ - ベンダーにて別途定義可<br>
     * ・0 - 使用しない<br>
     * ・5~999 - 必要に応じて定義<br>
     */
    unsigned int schChannelBroadType;
    /**
     * チャンネル情報種別<br>
     * ・1 - Gガイドチャンネル<br>
     * ・2 - ServiceID<br>
     * ・999 - 受信チャンネル/外部入力/録画機器内部コンテンツ<br>
     * ・0 - 使用しない<br>
     */
    unsigned int schChannelType;
    /**
     * チャンネル情報枝番 (1桁の10進数(0~9))<br>
     * (チャンネル放送種別コードが「2」でチャンネル情報種別が「999」のみ利用可能)<br>
     */
    unsigned int schChannelBranchNum;
    /**
     * SI情報ID<br>
     * ・SI_PROGRAMIDの場合→NWID,TransportStreamID,SID,ProgramID<br>
     * ・SI_SERIESIDの場合→NWID,TransportStreamID,SID,SeriesID<br>
     * ・VendorDefinedの場合→ベンダーにて別途定義可<br>
     * #DAP_MAX_STRING_LENGTH以上データは切り捨てる。
     * ※文字コードはUTF-8とし、null終端すること。<br>
     */
    char desiredMatchingID[DAP_MAX_STRING_LENGTH];
    /**
     * SI情報種別。
     * (SI_PROGRAMID, SI_SERIESID, その他ベンダーにて別途定義可)<br>
     * #DAP_MAX_STRING_LENGTH以上データは切り捨てる。
     * ※文字コードはUTF-8とし、null終端すること。<br>
     */
    char desiredMatchingtype[DAP_MAX_STRING_LENGTH];
    /**
     * 録画モード。
     * 録画予約時に指定する録画モード、またはタイトル(録画済番組)の録画モードを表す。<br>
     * 被制御機器固有に取り扱う録画モードを、各ベンダーにてコード体系などを別途定義する。<br>
     * #DAP_MAX_STRING_LENGTH以上データは切り捨てる。
     * ※文字コードはUTF-8とし、null終端すること。<br>
     */
    char desiredQualityMode[DAP_MAX_STRING_LENGTH];
    /**
     * ジャンルID (ベンダーにて別途定義)<br>
     * #DAP_MAX_STRING_LENGTH以上データは切り捨てる。
     * ※文字コードはUTF-8とし、null終端すること。<br>
     */
    char genreID[DAP_MAX_STRING_LENGTH];
    /**
     * ジャンルID種別コード<br>
     * ・1 - Gガイドチャンネルのジャンル<br>
     * ・2 - 地上波デジタル放送のジャンル<br>
     * ・3 - BS、CS 110度デジタル放送のジャンル<br>
     * ・4 - CS 110度放送の拡張ジャンル<br>
     * ・0 - 使用しない<br>
     */
    unsigned int genreType;
    /**
     * 予約投入優先フラグ。
     * 録画予約投入や録画予約修正の際に、該当の予約が他の予約と重複する場合、
     * エラーを返すか、予約を成功させるかを決めるフラグ。<br>
     * ・1 - True<br>
     * ・0 - False<br>
     * ・-1 - 未使用
     */
    INT priorityFlag;
    /**
     * 重複コード。
     * 予約リスト取得の際などに、該当の予約の重複の有無、もしくは録画の優先順位を表す。<br>
     * ・0 - 重複なしの予約。<br>
     * ・1 - 録画優先順位が1位の重複予約。<br>
     * ・2 - 録画優先順位が2位の重複予約。または優先順位が不明の重複予約。<br>
     * ・3以上の整数 - 数値が示す録画優先順位の重複予約。<br>
     * ・1桁の半角英字 - 必要に応じて定義<br>
     * #DAP_MAX_STRING_LENGTH以上データは切り捨てる。
     * ※文字コードはUTF-8とし、null終端すること。<br>
     */
    char conflictID[DAP_MAX_STRING_LENGTH];
    /**
     * メディア残量アラートコード。
     * 録画予約投入や録画予約リスト取得の際に、
     * 該当の予約か他の予約がメディアの残量不足の為に録画できない恐れがあることを、各ケース別にコード化して表す。(ベンダーにて別途定義)<br>
     * #DAP_MAX_STRING_LENGTH以上データは切り捨てる。
     * ※文字コードはUTF-8とし、null終端すること。<br>
     */
    char mediaRemainAlertID[DAP_MAX_STRING_LENGTH];
    /**
     * 予約投入元コード。
     * 録画予約やタイトル（録画済番組）に属する情報として、どのように予約投入されたかを表す。<br>
     * ・1000 - 自動録画予約全般<br>
     * ・1100 - 自動録画予約/本体予約全般(おまかせまる録など)<br>
     * ・1200 - 自動録画予約/リモート予約全般(リモート側から予約番組を自動投入するケース)<br>
     * ・2000 - ユーザー録画予約全般<br>
     * ・2100 - ユーザー録画予約/本体予約全般<br>
     * ・2110 - ユーザー録画予約/本体予約/ダイレクトREC(本体録画ボタン操作による録画)<br>
     * ・2120 - ユーザー録画予約/本体予約/タイマーREC(本体でのEPG操作や日時指定操作による録画)<br>
     * ・2190 - ユーザー録画予約/本体予約/その他<br>
     * ・2200 - ユーザー録画予約/リモート予約全般<br>
     * ・2210 - ユーザー録画予約/リモート予約/インターネットからの予約<br>
     * ・2220 - ユーザー録画予約/リモート予約/ホームネットからの予約<br>
     * ・2290 - ユーザー録画予約/リモート予約/その他<br>
     * ・9900 - その他<br>
     * ・上記以外の0000~9999 - 必要に応じて定義<br>
     */
    INT reservationCreatorID;
    /**
     * プロテクト状態フラグ。
     * タイトル（録画済番組）の削除操作に対するプロテクト状態を表す。
     * 但し、タイトル修正操作については、プロテクトのON/OFF状態と関係しない。<br>
     * ・1 - True<br>
     * ・0 - False<br>
     * ・-1 - 未使用
     */
    INT titleProtectFlag;
    /**
     * 視聴状態フラグ。
     * タイトル（録画済番組）が既に視聴されたものか否かの状態を表す。<br>
     * ・1 - True<br>
     * ・0 - False<br>
     * ・-1 - 未使用
     */
    INT titleNewFlag;
    /**
     * 録画中フラグ。
     * 録画予約番組やタイトルが、現在録画中であるかどうかの状態を表す。<br>
     * ・1 - True<br>
     * ・0 - False<br>
     * ・-1 - 未使用
     */
    INT recordingFlag;
} DapProgramInfo;

/**
 * @ingroup RecordScheduleIF
 * @brief リスト取得の取得条件。
 * 予約リスト取得/タイトルリスト取得でのみ使用。<br>
 */
typedef struct DapSearchInfo {
    /**
     * 検索条件。
     * 検索条件が不要な場合は指定しない（null終端のみ）。<br>
     * ※文字コードはUTF-8とし、null終端すること。<br>
     */
    char searchCriteria[DAP_MAX_SEARCH_LENGTH];
    /**
     * フィルター。
     * 取得するプロパティを限定しない場合や被制御機器側がフィルター機能に対応していない場合は、
     * 「*」(該当プロパティ全部を取得)を指定。(必須)<br>
     * ※文字コードはUTF-8とし、null終端すること。<br>
     */
    char filter[DAP_MAX_FILTER_LENGTH];
    /**
     * 検索開始オフセット値 (0~4294967295の整数値)
     * Index(0オリジン)による検索開始位置の指定。(必須)<br>
     */
    unsigned int startingIndex;
    /**
     * 取得要求件数 (0~4294967295の整数値)
     * 取得する件数(制限値)を指定、全件の場合は0を指定。(必須)
     */
    unsigned int requestedCount;
    /**
     * ソート条件。
     * ソート条件が不要な場合は指定しない（null終端のみ）。<br>
     * ※文字コードはUTF-8とし、null終端すること。<br>
     */
    char sortCriteria[DAP_MAX_SORT_LENGTH];
} DapSearchInfo;

/**
 * @ingroup RecordScheduleIF
 * @brief リスト取得の取得結果。
 * 予約リスト取得/タイトルリスト取得でのみ使用。<br>
 */
typedef struct DapSearchResult {
    /**
     * 取得件数 (0~4294967295の整数値)<br>
     */
    unsigned int numberReturned;
    /**
     * 検索条件に該当する全件数 (0~4294967295の整数値)<br>
     */
    unsigned int totalMatches;
} DapSearchResult;
/* **************************************************************************
 * 関数
 * ************************************************************************** */
/* ==========================================================================
 * データI/O
 * ========================================================================== */
/**
 * @defgroup DataIO データI/O（DAP->レコーダーシステム）
 * @{
 */
/**
 * 機器埋め込み情報のベースドメインを取得しアウトパラメータとして返す。<br>
 * @param [out] baseDomain ベースドメイン
 * 最大15文字(データ14文字＋"\0") ※null終端すること。<br>
 * 文字コードはUTF-8とする。<br>
 * @param [in] length 確保した領域の長さ
 * @return ベースドメイン取得の処理結果<br>
 * <ul>
 * <li>#TRUE
 * <li>#FALSE
 * </ul>
 */
BOOLEAN DapGetServerBaseDomain(
        char* baseDomain,
        unsigned int length);
/**
 * 機器埋め込み情報のService Profile URNを取得しアウトパラメータとして返す。<br>
 * @param [out] serviceProfileURN Service Profile URN
 * 最大66文字(データ65文字＋"\0") ※null終端すること。<br>
 * 文字コードはUTF-8とする。<br>
 * @param [in] length 確保した領域の長さ
 * @return Service Profile URN取得の処理結果<br>
 * <ul>
 * <li>#TRUE
 * <li>#FALSE
 * </ul>
 */
BOOLEAN DapGetServiceProfileURN(
        char* serviceProfileURN,
        unsigned int length);
/**
 * 機器埋め込み情報のSOAPサーバアクセス先URI(Port)を取得しアウトパラメータとして返す。<br>
 * @param [out] port SOAPサーバアクセス先URI(Port)
 * 最大5文字(データ4文字＋"\0") ※null終端すること。<br>
 * 文字コードはUTF-8とする。<br>
 * @param [in] length 確保した領域の長さ
 * @return SOAPサーバアクセス先URI(Port)取得の処理結果<br>
 * <ul>
 * <li>#TRUE
 * <li>#FALSE
 * </ul>
 */
BOOLEAN DapGetSOAPServerURIPort(
        char* port,
        unsigned int length);
/**
 * 機器埋め込み情報のSOAPサーバアクセス先URI(Path)を取得しアウトパラメータとして返す。<br>
 * @param [out] path SOAPサーバアクセス先URI(Path)
 * 十分な領域を確保すること。 ※null終端すること。<br>
 * 文字コードはUTF-8とする。<br>
 * @param [in] length 確保した領域の長さ
 * @return SOAPサーバアクセス先URI(Port)取得の処理結果<br>
 * <ul>
 * <li>#TRUE
 * <li>#FALSE
 * </ul>
 */
BOOLEAN DapGetSOAPServerURIPath(
        char* path,
        unsigned int length);
/**
 * 機器埋め込み情報のベンダーKeyを取得しアウトパラメータとして返す。<br>
 * @param [out] venderkey ベンダーKey
 * 64バイト<br>
 * @param [in] length 確保した領域の長さ
 * @return ベンダーKey取得の処理結果<br>
 * <ul>
 * <li>#TRUE
 * <li>#FALSE
 * </ul>
 */
BOOLEAN DapGetVenderKey(
        BYTE* venderkey,
        unsigned int length);
/**
 * 機器埋め込み情報のSSLルート証明書の長さを取得する。<br>
 * SSLルート証明書取得の前に呼び出す必要がある。※取得するデータの領域を予め確保するため。<br>
 * @param [out] length SSLルート証明書の長さ<br>
 * @return SSLルート証明書長取得の処理結果<br>
 * <ul>
 * <li>#TRUE
 * <li>#FALSE
 * </ul>
 */
BOOLEAN DapGetSSLRootCertificateLength(
        unsigned int* length);
/**
 * 機器埋め込み情報のSSLルート証明書を取得しアウトパラメータとして返す。<br>
 * DAPは、SSLルート証明書の長さを予め取得し必要な領域を確保する。
 * @param [out] sslRootCertificate SSLルート証明書<br>
 * @param [in] length SSLルート証明書長取得で取得した長さ<br>
 * @return SSLルート証明書取得の処理結果<br>
 * <ul>
 * <li>#TRUE
 * <li>#FALSE
 * </ul>
 */
BOOLEAN DapGetSSLRootCertificate(
        BYTE* sslRootCertificate,
        unsigned int length);
/**
 * 機器埋め込み情報の製品名を取得しアウトパラメータとして返す。<br>
 * 製品名長は32文字以内(半角英数のみ)(NULL不可)。<br>
 * @param [out] productName 製品名 ※null終端すること。<br>
 * 文字コードはUTF-8とする。<br>
 * @param [in] length 確保した領域の長さ
 * @return 製品名取得の処理結果<br>
 * <ul>
 * <li>#TRUE
 * <li>#FALSE
 * </ul>
 */
BOOLEAN DapGetProductName(
        char* productName,
        unsigned int length);
/**
 * 製品情報を取得しアウトパラメータとして返す。<br>
 * 十分な領域を確保すること。<br>
 * @param [out] productInfo 製品情報 ※null終端すること。<br>
 * 文字コードはUTF-8とする。<br>
 * @param [in] length 確保した領域の長さ
 * @return 製品情報取得の処理結果<br>
 * <ul>
 * <li>#TRUE
 * <li>#FALSE
 * </ul>
 */
BOOLEAN DapGetProductInfo(
        char* productInfo,
        unsigned int length);
/**
 * 全PlatformIDの個数を取得する。<br>
 * 全PlatformID取得の前に呼び出す必要がある。※取得するデータの領域を予め確保するため。<br>
 * @param [out] length 全PlatformIDの個数<br>
 * @return 全PlatformIDの個数取得の処理結果<br>
 * <ul>
 * <li>#TRUE
 * <li>#FALSE
 * </ul>
 */
BOOLEAN DapGetAllPlatformIDLength(
        unsigned int* length);
/**
 * 全PlatformID情報を取得しアウトパラメータとして返す。<br>
 * DAPは、全PlatformIDの個数を予め取得し必要な領域を確保する。
 * インパラメータとして、領域を確保した個数を指定する。
 * @param [out] pPlatformID 全PlatformID(最大499件)<br>
 * @param [in] length 全PlatformIDの個数取得で取得した個数<br>
 * @return 全PlatformID取得の処理結果<br>
 * <ul>
 * <li>#TRUE
 * <li>#FALSE
 * </ul>
 */
BOOLEAN DapGetAllPlatformID(
        INT* pPlatformID,
        unsigned int length);
/**
 * PlatformIDをセットする。<br>
 * 既にPlatformIDが存在する場合は、#TRUEを返し何も処理しない。
 * @param [in] platformID PlatformID<br>
 * @return PlatformIDセットの処理結果<br>
 * <ul>
 * <li>#TRUE
 * <li>#FALSE
 * </ul>
 */
BOOLEAN DapAddPlatformID(
        INT platformID);
/**
 * PlatformIDを削除する。レコーダーシステムはPlatformIDに紐付いた以下のデータを全て削除すること。<br>
 * 機器特定ID/機器認証継続ID/初回接続待ち時間/再接続待ち時間/機器認証継続ID有効期限<br>
 * @param [in] platformID PlatformID<br>
 * @return PlatformID削除の処理結果<br>
 * <ul>
 * <li>#TRUE
 * <li>#FALSE
 * </ul>
 */
BOOLEAN DapDeletePlatformID(
        INT platformID);
/**
 * MACアドレスを取得しアウトパラメータとして返す。<br>
 * オクテット表現で%02X:%02X:%02X:%02X:%02X:%02Xの形式とする。例）00:11:22:AA:BB:CC<br>
 * @param [out] macAddress MACアドレス ※null終端すること。<br>
 * 文字コードはUTF-8とする。<br>
 * @param [in] length 確保した領域の長さ
 * @return MACアドレス取得の処理結果<br>
 * <ul>
 * <li>#TRUE
 * <li>#FALSE
 * </ul>
 */
BOOLEAN DapGetMACAddress(
        char* macAddress,
        unsigned int length);
/**
 * 機器特定IDを取得しアウトパラメータとして返す。<br>
 * @param [in] platformID
 * @param [out] deviceSpecificID 機器特定ID(フォーマットは32バイトUUID) ※null終端すること。<br>
 * 文字コードはUTF-8とする。<br>
 * @param [in] length 確保した領域の長さ
 * @return 機器特定ID取得の処理結果<br>
 * <ul>
 * <li>#TRUE
 * <li>#FALSE
 * </ul>
 */
BOOLEAN DapGetDeviceSpecificID(
        INT platformID,
        char* deviceSpecificID,
        unsigned int length);
/**
 * 機器特定IDをセットする。<br>
 * @param [in] platformID
 * @param [in] deviceSpecificID 機器特定ID(フォーマットは32バイトUUID) ※null終端すること。<br>
 * 文字コードはUTF-8とする。<br>
 * @return 機器特定IDセットの処理結果<br>
 * <ul>
 * <li>#TRUE
 * <li>#FALSE
 * </ul>
 */
BOOLEAN DapSetDeviceSpecificID(
        INT platformID,
        char* deviceSpecificID);
/**
 * 機器認証継続IDを取得しアウトパラメータとして返す。<br>
 * @param [in] platformID
 * @param [out] authenticateID 機器認証継続ID
 * (最大33バイト(データ32バイト＋"\0") ※null終端すること。<br>
 * 文字コードはUTF-8とする。<br>
 * @param [in] length 確保した領域の長さ
 * @return 機器認証継続ID取得の処理結果<br>
 * <ul>
 * <li>#TRUE
 * <li>#FALSE
 * </ul>
 */
BOOLEAN DapGetAuthenticateID(
        INT platformID,
        char* authenticateID,
        unsigned int length);
/**
 * 機器認証継続IDをセットする。<br>
 * @param [in] platformID
 * @param [in] authenticateID 機器認証継続ID
 * (最大33バイト(データ32バイト＋"\0") ※null終端すること。<br>
 * 文字コードはUTF-8とする。<br>
 * @return 機器認証継続IDセットの処理結果<br>
 * <ul>
 * <li>#TRUE
 * <li>#FALSE
 * </ul>
 */
BOOLEAN DapSetAuthenticateID(
        INT platformID,
        char* authenticateID);
/**
 * 初回接続待ち時間を取得しアウトパラメータとして返す。<br>
 * @param [in] platformID
 * @param [out] waitTime 初回接続待ち時間(秒)<br>
 * @return 初回接続待ち時間取得の処理結果<br>
 * <ul>
 * <li>#TRUE
 * <li>#FALSE
 * </ul>
 */
BOOLEAN DapGetInitialWaitTime(
        INT platformID,
        INT* waitTime);
/**
 * 初回接続待ち時間をセットする。<br>
 * @param [in] platformID
 * @param [in] waitTime 初回接続待ち時間(秒)<br>
 * @return 初回接続待ち時間セットの処理結果<br>
 * <ul>
 * <li>#TRUE
 * <li>#FALSE
 * </ul>
 */
BOOLEAN DapSetInitialWaitTime(
        INT platformID,
        INT waitTime);
/**
 * 再接続待ち時間を取得しアウトパラメータとして返す。<br>
 * @param [in] platformID
 * @param [out] waitTime 初回接続待ち時間(秒)<br>
 * @return 再接続待ち時間取得の処理結果<br>
 * <ul>
 * <li>#TRUE
 * <li>#FALSE
 * </ul>
 */
BOOLEAN DapGetReconnectWaitTime(
        INT platformID,
        INT* waitTime);
/**
 * 再接続待ち時間をセットする。<br>
 * @param [in] platformID
 * @param [in] waitTime 初回接続待ち時間(秒)<br>
 * @return 再接続待ち時間セットの処理結果<br>
 * <ul>
 * <li>#TRUE
 * <li>#FALSE
 * </ul>
 */
BOOLEAN DapSetReconnectWaitTime(
        INT platformID,
        INT waitTime);
/**
 * 機器認証継続ID有効期限を取得しアウトパラメータとして返す。<br>
 * @param [in] platformID
 * @param [out] expiration 機器認証継続ID有効期限
 * (最大30バイト(データ29バイト＋"\0") ※null終端すること。<br>
 * 文字コードはUTF-8とする。<br>
 * @param [in] length 確保した領域の長さ
 * @return 機器認証継続ID有効期限取得の処理結果<br>
 * <ul>
 * <li>#TRUE
 * <li>#FALSE
 * </ul>
 */
BOOLEAN DapGetAuthenticateIDExpiration(
        INT platformID,
        char* expiration,
        unsigned int length);
/**
 * 機器認証継続ID有効期限をセットする。<br>
 * @param [in] platformID
 * @param [in] expiration 機器認証継続ID有効期限<br>
 * (最大30バイト(データ29バイト＋"\0") ※null終端すること。<br>
 * @return 機器認証継続ID有効期限セットの処理結果<br>
 * <ul>
 * <li>#TRUE
 * <li>#FALSE
 * </ul>
 */
BOOLEAN DapSetAuthenticateIDExpiration(
        INT platformID,
        char* expiration);
/** @} */
/* ==========================================================================
 * 録画予約
 * ========================================================================== */
/**
 * @defgroup RecordScheduleIF 録画予約（DAP->レコーダーシステム）
 * @{
 */
/**
 * 録画予約の投入を行う。<br>
 * @param [in] pDapInProgramInfo 予約に必要な番組情報(IDの値は指定しないこと)<br>
 * @param [out] pDapOutProgramInfo 予約投入結果の番組情報<br>
 * @return 予約投入の処理結果<br>
 * <ul>
 * <li>#DAP_OK
 * <li>#DAP_ERROR_INVALID_VALUE
 * <li>#DAP_ERROR_CANNOT_PROCCES_REQUEST
 * <li>#DAP_ERROR_CONFLICT
 * <li>#DAP_ERROR_PROTECTED_CONTENTS
 * <li>#DAP_ERROR_TOO_MANY_RECORD_SCHEDULES
 * <li>#DAP_ERROR_DIRECCT_RECORDING
 * <li>#DAP_ERROR_SYSTEM_STARTING_UP
 * <li>#DAP_ERROR_OTHER_ERROR_RECORD_SCHEDULE
 * </ul>
 */
INT DapCreateRecordSchedule(
        DapProgramInfo* pDapInProgramInfo,
        DapProgramInfo* pDapOutProgramInfo);
/**
 * 重複リストを取得する。
 * 重複リストの個数取得で指定した番組と重複する番組リストをID付で返す。<br>
 * DAPは、重複リストの個数を予め取得し必要な領域を確保する。<br>
 * インパラメータとして、領域を確保した個数を指定する。<br>
 * @param [out] pDapProgramInfo 指定した番組に対して録画予約の番組情報
 * (0個、1個、複数個の場合がある)<br>
 * @param [in] length 重複リストの個数取得で取得した個数<br>
 * @return 重複リスト取得の処理結果<br>
 * <ul>
 * <li>#DAP_OK
 * <li>#DAP_ERROR_CANNOT_PROCCES_REQUEST
 * <li>#DAP_ERROR_SYSTEM_STARTING_UP
 * <li>#DAP_ERROR_OTHER_ERROR_RECORD_SCHEDULE
 * </ul>
 */
INT DapGetConflictList(
        DapProgramInfo* pDapProgramInfo,
        DapProgramInfo** pDapOutPrgInfo,
        unsigned int* length);

/**
 * 録画予約IDにて指定した予約を削除する。<br>
 * @param [in] recordScheduleID 予約投入/予約リスト取得で取得した録画予約ID<br>
 * @return 予約削除の処理結果<br>
 * <ul>
 * <li>#DAP_OK
 * <li>#DAP_ERROR_INVALID_VALUE
 * <li>#DAP_ERROR_NO_SUCH_RECORDSCHEDULEID
 * <li>#DAP_ERROR_RECORDSCHEDULE_ACTIVELY_RECCORDING
 * <li>#DAP_ERROR_CANNOT_PROCCES_REQUEST
 * <li>#DAP_ERROR_CANNOT_DELETE_SPECIFIC_RECORED_SCHEDULE
 * <li>#DAP_ERROR_SYSTEM_STARTING_UP
 * <li>#DAP_ERROR_OTHER_ERROR_RECORD_SCHEDULE
 * </ul>
 */
INT DapDeleteRecordSchedule(
        char* recordScheduleID);
/**
 * 検索条件に該当する予約リストの個数を取得する。<br>
 * 予約リスト取得の前に呼び出す必要がある。※取得するデータの領域を予め確保するため。<br>
 * @param [in] pDapListInParamInfo 予約リストの取得に必要な情報(検索条件)<br>
 * @param [out] length 予約リストの個数<br>
 * @return 予約リストの個数取得の処理結果<br>
 * <ul>
 * <li>#DAP_OK
 * <li>#DAP_ERROR_NO_SUCH_RECORDSCHEDULEID
 * <li>#DAP_ERROR_UNSUPPORTED_OR_INVALID_SORT_CRITERIA
 * <li>#DAP_ERROR_CANNOT_PROCCES_REQUEST
 * <li>#DAP_ERROR_SYSTEM_STARTING_UP
 * <li>#DAP_ERROR_OTHER_ERROR_RECORD_SCHEDULE
 * </ul>
 */
INT DapGetRecordScheduleListLength(
        DapSearchInfo* pDapListInParamInfo,
        unsigned int* length);
/**
 * 検索条件に該当する予約リストを取得する。<br>
 * DAPは、予約リストの個数を予め取得し必要な領域を確保する。<br>
 * インパラメータとして、領域を確保した個数を指定する。<br>
 * @param [out] pDapProgramInfo 予約リスト取得で取得した番組情報
 * (0個、1個、複数個の場合がある)<br>
 * @param [in] length 予約リストの個数取得で取得した個数<br>
 * @param [out] pDapSearchResult 取得件数、検索条件に該当する全件数、アップデートID<br>
 * @return 予約リスト取得の処理結果<br>
 * <ul>
 * <li>#DAP_OK
 * <li>#DAP_ERROR_CANNOT_PROCCES_REQUEST
 * <li>#DAP_ERROR_SYSTEM_STARTING_UP
 * <li>#DAP_ERROR_OTHER_ERROR_RECORD_SCHEDULE
 * </ul>
 */
INT DapGetRecordScheduleList(
        DapProgramInfo* pDapProgramInfo,
        unsigned int length,
        DapSearchResult* pDapSearchResult);
/**
 * 録画予約を番組情報にて修正する。指定された内容のみが上書き更新される。
 * @param [in] pDapInProgramInfo 修正内容を指定する番組情報<br>
 * @param [out] pDapOutProgramInfo 更新される番組情報<br>
 * @return 予約修正の処理結果<br>
 * <ul>
 * <li>#DAP_OK
 * <li>#DAP_ERROR_INVALID_VALUE
 * <li>#DAP_ERROR_NO_SUCH_RECORDSCHEDULEID
 * <li>#DAP_ERROR_RECORDSCHEDULE_ACTIVELY_RECCORDING
 * <li>#DAP_ERROR_CANNOT_PROCCES_REQUEST
 * <li>#DAP_ERROR_CONFLICT
 * <li>#DAP_ERROR_PROTECTED_CONTENTS
 * <li>#DAP_ERROR_SYSTEM_STARTING_UP
 * <li>#DAP_ERROR_INVALID_VALUE_EDIT_RECORDSCHEDULE
 * <li>#DAP_ERROR_OTHER_ERROR_RECORD_SCHEDULE
 * </ul>
 */
INT DapUpdateRecordSchedule(
        DapProgramInfo* pDapInProgramInfo,
        DapProgramInfo* pDapOutProgramInfo);
/**
 * 検索条件に該当するタイトルリストの個数を取得する。<br>
 * タイトルリスト取得の前に呼び出す必要がある。※取得するデータの領域を予め確保するため。<br>
 * @param [in] pDapListInParamInfo リスト取得に必要な情報(検索条件)<br>
 * @param [out] length タイトルリストの個数<br>
 * @return タイトルリストの個数取得の処理結果<br>
 * <ul>
 * <li>#DAP_OK
 * <li>#DAP_ERROR_UNSUPPORTED_OR_INVALID_SORT_CRITERIA
 * <li>#DAP_ERROR_CANNOT_PROCCES_REQUEST
 * <li>#DAP_ERROR_SYSTEM_STARTING_UP
 * <li>#DAP_ERROR_OTHER_ERROR_ABOUT_TITLE
 * <li>#DAP_ERROR_NO_SUCH_TITLEID
 * </ul>
 */
INT DapGetTitleListLength(
        DapSearchInfo* pDapListInParamInfo,
        unsigned int* length);
/**
 * 検索条件に該当するタイトルリストを取得する。<br>
 * DAPは、タイトルリストの個数を予め取得し必要な領域を確保する。
 * インパラメータとして、領域を確保した個数を指定する。
 * @param [out] pDapProgramInfo タイトルリスト取得で取得した番組情報
 * (0個、1個、複数個の場合がある)<br>
 * @param [in] length タイトルリストの個数取得で取得した個数<br>
 * @param [out] pDapSearchResult 取得件数、検索条件に該当する全件数、アップデートID<br>
 * @return タイトルリスト取得の処理結果<br>
 * <ul>
 * <li>#DAP_OK
 * <li>#DAP_ERROR_CANNOT_PROCCES_REQUEST
 * <li>#DAP_ERROR_SYSTEM_STARTING_UP
 * <li>#DAP_ERROR_OTHER_ERROR_ABOUT_TITLE
 * <li>#DAP_ERROR_NO_SUCH_TITLEID
 * </ul>
 */
INT DapGetTitleList(
        DapProgramInfo* pDapProgramInfo,
        unsigned int length,
        DapSearchResult* pDapSearchResult);
/**
 * 録画予約IDにて指定したタイトルを削除する。<br>
 * @param [in] titleID タイトルリスト取得で取得した録画予約ID<br>
 * @return タイトル削除の処理結果<br>
 * <ul>
 * <li>#DAP_OK
 * <li>#DAP_ERROR_CANNOT_PROCCES_REQUEST
 * <li>#DAP_ERROR_SYSTEM_STARTING_UP
 * <li>#DAP_ERROR_OTHER_ERROR_ABOUT_TITLE
 * <li>#DAP_ERROR_NO_SUCH_TITLEID
 * <li>#DAP_ERROR_TITLE_ACTIVELY_RECORDING
 * <li>#DAP_ERROR_TITLE_ACTIVELY_PLAYING
 * <li>#DAP_ERROR_TITLE_PROTECTED_BY_USER
 * <li>#DAP_ERROR_CANNOT_DELETE_OR_EDIT_SPECIFIC_TITLE
 * </ul>
 */
INT DapDeleteTitle(
        char* titleID);
/**
 * タイトルを番組情報にて修正する。<br>
 * @param [in] pDapInPrgInfo 修正内容を指定する番組情報(タイトルリスト取得で取得した番組情報ID)<br>
 * @param [out] pDapOutPrgInfo 修正される番組情報<br>
 * @return タイトル修正の処理結果<br>
 * <ul>
 * <li>#DAP_OK
 * <li>#DAP_ERROR_CANNOT_PROCCES_REQUEST
 * <li>#DAP_ERROR_SYSTEM_STARTING_UP
 * <li>#DAP_ERROR_OTHER_ERROR_ABOUT_TITLE
 * <li>#DAP_ERROR_INVALID_VALUE_EDIT_TITLE
 * <li>#DAP_ERROR_NO_SUCH_TITLEID
 * <li>#DAP_ERROR_TITLE_ACTIVELY_RECORDING
 * <li>#DAP_ERROR_TITLE_ACTIVELY_PLAYING
 * <li>#DAP_ERROR_TITLE_PROTECTED_BY_USER
 * <li>#DAP_ERROR_CANNOT_DELETE_OR_EDIT_SPECIFIC_TITLE
 * </ul>
 */
INT DapUpdateTitle(
        DapProgramInfo* pDapInPrgInfo,
        DapProgramInfo* pDapOutPrgInfo);
/** @} */
/* ==========================================================================
 * レコーダーシステム状態取得（DAP->レコーダーシステム）
 * ========================================================================== */
/**
 * @defgroup RecorderStatusIF_DAP_SYS レコーダーシステム状態取得（DAP->レコーダーシステム）
 * @{
 */
/**
 * レコーダーシステムからの要求受付可否状態を取得する。<br>
 * <br>
 * サーバからの要求に対し、レコーダーシステムの受付可否状態を取得する<br>
 * wait_timeには、受け付け可能状態（#DAP_RECORDER_STATUS_RECEIVING）へ遷移するまでに要するおおよその時間（秒）を指定する。<br>
 * 例えば、現在準備中状態（#DAP_RECORDER_STATUS_PREPARING）であり、準備中状態から受け付け可能状態になるまでに、<br>
 * 5秒かかる場合は、statusに#DAP_RECORDER_STATUS_PREPARING、wait_timeに5を指定する。<br>
 * 受け付け可能状態（#DAP_RECORDER_STATUS_RECEIVING）の場合は、wait_timeに0を、<br>
 * 一時利用不可（#DAP_RECORDER_STATUS_UNAVAILABLE）の場合は、wait_timeに-1を指定する。<br>
 * また、本関数がコールされた場合は、サーバから何らかの要求が来たことを意味する。<br>
 * よって、レコーダーシステムは本関数がコールされ、受付可否状態を返却した後、受け付け可能状態（#DAP_RECORDER_STATUS_RECEIVING）に遷移すること。
 *
 * @param [out] status レコーダーシステムの状態。
 * <ul>
 * <li>#DAP_RECORDER_STATUS_RECEIVING:		受け付け可能状態
 * <li>#DAP_RECORDER_STATUS_PREPARING:		準備中状態
 * <li>#DAP_RECORDER_STATUS_UNAVAILABLE:	一時利用不可状態
 * </ul>
 * @param [out] wait_time 受け付け可能状態になるまでの時間（秒）
 * @return 受付可否状態取得の処理結果<br>
 * <ul>
 * <li>#TRUE
 * <li>#FALSE
 * </ul>
 */
BOOLEAN DapGetRecorderStatus(
        INT* status,
        INT* wait_time);

/** @} */

#ifdef __cplusplus
}
#endif
#endif /* _DAP_PORTING_H_ */
