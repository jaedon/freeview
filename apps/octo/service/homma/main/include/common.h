/*************************************************************************************************************
	File            : common.h
	author          : IPM tipmsw1@humaxdigital.com
	comment         :
	date            :
	attention       :

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. (Humax) and
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
	non-assignable, non-transferable and non-exclusive license to use this Software.
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
	you agree to the responsibility to take all reasonable efforts to protect the any information
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
	reverse engineer or extract the source code of this Software unless you have Humax's written permission
	to do so.
	If you have no authorized license, discontinue using this Software immediately.

	THE SOFTWARE IS PROVIDED ¡°AS IS¡± AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
	EITHER EXPRESS, IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
	IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR
	INDIRECT, SPECIAL, CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR
	INABILITY TO USE THE SOFTWARE.

	This License is effective until terminated. You may terminate this License at any time
	by destroying all copies of the Software including all documentation. This License will terminate
	immediately without notice from Humax to you if you fail to comply with any provision of this License.
	Upon termination, you must destroy all copies of the Software and all documentation.

	The laws of the Republic of Korea will apply to any disputes arising out of or relating to
	this Copyright Notice. All claims arising out of or relating to this Copyright Notice will be litigated
	in the Seoul Central District Court, in the Republic of Korea.
*************************************************************************************************************/

#ifndef	__COMMON_H__
#define	__COMMON_H__

#define __________Header_Files_______________

#include "homma_interop.h"

#include <arpa/inet.h>
#include <resolv.h>

#include <sys/socket.h>
#include <netinet/ether.h>
#include <sys/socket.h>
#include <unistd.h>

#include <hlib.h>
#include <dlib.h>

#include "define.h"

#include <dapi.h>


#if 0
#define	FRIENDLY_NAME_MAX					(128)	/* Reference => 2011 DLNA Spec: 7.5.4.3.2.37.2 */
#define	UDN_MAX								(16)	/* Reference => 2011 DLNA Spec: 7.3.2.26.6 */
#define	MODEL_NAME_MAX						(32)	/* Not refernece. We fix length for the model name, Length for model name is 32 byte.  */
#define MODEL_DESCRIPTION_MAX				(128)	/* Not refernece. We fix length for the model description, Length for model description is 128 byte.  */
#define MODEL_URL_MAX						(256)	/* Not refernece. We fix length for the model url, Length for model url is 256 byte.  */
#define MODEL_NUMBER_MAX					(32)	/* Not refernece. We fix length for the model number, Length for model number is 32 byte.  */
#endif


#define SUPPORT_DMR_FLING

/* Default Network Define */
#define WLAN_KEY_MAX						(128)
#define WLAN_NAME_MAX						(48)

#define	NETWORK_INTERFACE_NAME_MAX			(16)
#define	NETWORK_INTERFACE_HA_MAX			(8)
#define	NETWORK_INTERFACE_NUM_MAX			(6)

/* struct Reference
struct sockaddr{
	sa_family_t sa_family;  // 소켓의 주소체계. PF_INET= IPv4 주소체계.
	char sa_data[14];      // 해당 주소체계에서 사용하는 주소 정보.
}
*/


/* Dial extention (huamx app remote start) */
// #define SUPPORT_DIAL_EXTENTION


typedef enum {
	eBUS_EVENT_MOD_START = 0,
	eBUS_EVENT_MOD_STOP,

	eBUS_EVENT_SYS_NET_IF_ONLINE = 10,
	eBUS_EVENT_SYS_NET_IF_OFFLINE,
	eBUS_EVENT_SYS_NET_IF_SWITCHING,

	eBUS_EVENT_SYS_FS_MOUNTED = 20,
	eBUS_EVENT_SYS_FS_UNMOUNTED,
	eBUS_EVENT_SYS_MS_FORMAT_TRY,
	eBUS_EVENT_SYS_MS_FORMAT_DONE,

	eBUS_EVENT_SYS_POWER_OPERATION = 30,
	eBUS_EVENT_SYS_POWER_SHUTDOWN,
	eBUS_EVENT_SYS_POWER_SYNC,
	eBUS_EVENT_SYS_POWER_SHUTDOWNDONE,
	eBUS_EVENT_SYS_POWER_REBOOT,
	eBUS_EVENT_SYS_PW_STANDBY,
	eBUS_EVENT_SYS_PW_FACTORYDEFAULT,

	eBUS_EVENT_SYS_FILE_COPY_DONE = 40,
	eBUS_EVENT_SYS_FILE_DELETE_DONE, /* record delete 포함 */
	eBUS_EVENT_SYS_FILE_MOVE_DONE,
	eBUS_EVENT_SYS_FILE_CREATE_DONE,
	eBUS_EVENT_SYS_REC_ADDED,
	eBUS_EVENT_SYS_REC_DELETED, /* 12. record file will be process by FILE_DELETE_DONE */
	eBUS_EVENT_SYS_REC_UPDATED,

	eBUS_EVENT_DMR_REGISTER_MEDIA_CALLBACK,
	eBUS_EVENT_DMR_UNREGISTER_MEDIA_CALLBACK,

	eBUS_EVENT_UNKNOWN,

	eBUS_EVENT_MOD_DEFINE,
} HOMMA_BUS_Event_e;


typedef	enum {
	eHOM_Result_Ok=0, /* HERROR, ERR_OK */
	eHOM_Result_Error,
	eHOM_Result_Error_Used,
	eHOM_Result_Error_NotInit,
	eHOM_Result_Error_NullPoint,
	eHOM_Result_Error_Invalid_Arguments,
	eHOM_Result_Error_Resource_Notfound,
	eHOM_Result_Error_Resource_Full,
	eHOM_Result_Error_InitFail,
	eHOM_Result_Error_OutofMem,
	eHOM_Result_Error_Destroyed,
	eHOM_Result_Error_Conflict,
	eHOM_Result_Error_CreateFail,
	eHOM_Result_Error_NotExist,
	eHOM_Result_Error_NoSignalLock,

	eHOM_Result_Ok_ButNotEffect,
	eHOM_Result_Error_InvalidValue,
	eHOM_Result_Error_Timeout,

	/* 위로 추가하세요 */
	eHOM_Result_Max
} HOM_Result_e;

#define	NETWORK_IP4ADDR_MAX	(16)
#define	NETWORK_MACADDR_MAX	(20)
#define NETWORK_IP6ADDR_MAX	(40)

/**
 * @enum CO_NET_IF_Type_e
 * @brief Network Interface Type 값.
 */
typedef enum {
	eNET_IF_WIRED = 0,  ///< 랜케이블을 사용하는 인터페이스 타입.
	eNET_IF_WIRELESS,	///< 무선으로 통해 동작하는 인터페이스 타입.
	eNET_IF_CM_ETH,		///< CableModem 설정을 위해 chipset 내부에 있는 인터페이스 타입.
	eNET_IF_CM_EXT,		///< 실제 Signal선을 통해 동작하는 인터페이스 타입.
	eNET_IF_UNKNOWN,	///< 알수 없는 인터페이스 타입.
} CO_NET_IF_Type_e;


/**
 * @struct CO_NET_Iface_t
 * @brief Network Interfae 정보
 */
typedef struct CO_NET_Iface_s {

	HINT32	nIndex;		///< 해당 인퍼페이스의 Index.
	CO_NET_IF_Type_e	eType;	///< 인터페이스 타입.
	HCHAR	szName[NETWORK_INTERFACE_NAME_MAX]; ///< 장치 이름. 예)eth0, wlan0 등
	HCHAR	szMacAddr[NETWORK_MACADDR_MAX]; ///< MAC 주소.

	/* IPv4 Address Info */
	HBOOL	bIPv4_Enable;	///< IPv4로 사용할지 여부. 현재 구현되지 않은 필드.
	HULONG	ulIpAddr;
	HULONG 	ulNetmask;
	HULONG 	ulGateway;
	HCHAR	szIpAddr[NETWORK_IP4ADDR_MAX];		///< IP 주소.
	HCHAR	szNetmask[NETWORK_IP4ADDR_MAX];		///< Netmask.
	HCHAR	szGateway[NETWORK_IP4ADDR_MAX];		///< GateWay 주소
	HCHAR	szDns1[NETWORK_IP4ADDR_MAX];		///< DNS 1 주소.
	HCHAR	szDns2[NETWORK_IP4ADDR_MAX];		///< DNS 2 주소.

	/* IPv6 Address Info */
	HBOOL	bIPv6_Enable;	///< IPv6로 사용할지 여부. 현재 구현되지 않은 필드.
	HCHAR	szIpAddr_v6[NETWORK_IP6ADDR_MAX];	///< IPv6 주소.
	HCHAR	szGateway_v6[NETWORK_IP6ADDR_MAX];	///< Gateway 주소(IPv6).
	HCHAR	szDns1_v6[NETWORK_IP6ADDR_MAX];		///< DNS 1 주소(IPv6).
	HCHAR	szDns2_v6[NETWORK_IP6ADDR_MAX];		///< DNS 2 주소(IPv6).
	HUINT32 	nPrefix;	///< IPv6용 Prefix.

	HBOOL	bConnected;		///< 물리적 통신라인 연결 유무.
	HBOOL	bEnabled; 		///< 인터페이스 UP/DOWN 유무.( UP == TRUE )
	HBOOL	bIpSetup;		///< IP 설정 유무.
	HBOOL	bValidIP;		///< IP validation
} CO_NET_Iface_t;

/**
 * @struct CO_NET_Info_t
 * @brief 시스템내 Network 설정 정보
 */
typedef struct CO_Net_Info_s {
	HINT32 nDevCnt;	///< Interface 장치 개수.
	HINT32 nDefIdx; ///< 사용되는 Default 장치 Index;
	HBOOL bValidDev;

	const CO_NET_Iface_t *pstIfaces;	////< NetworkInterface 리스트.
} CO_NET_Info_t;

#define	STORAGE_MOUNT_NUM_MAX	16
#define MOUNT_PATH_LEN_MAX 32
#define STORAGE_UUID_LEN_MAX 37

/**
 * @enum CO_STORAGE_Type_e
 * @brief Storage 종류.
 */
typedef enum
{
	eSTORAGE_TYPE_HDD,	///< HARD DISK.
	eSTORAGE_TYPE_MEM,	///< MEMORY  Storage.
	eSTORAGE_TYPE_SMB,	///< SAMBA  Storage.
	eSTORAGE_TYPE_UNKNOWN, /// Unknown Storage.

} CO_STORAGE_Type_e;

/**
 * @enum CO_STORAGE_Iface_e
 * @brief Storage 연결 형태.
 */
typedef enum
{
	eSTORAGE_IFACE_SATA,	///< SATA로 연결.
	eSTORAGE_IFACE_USB,		///< USB로 연결.
	eSTORAGE_IFACE_SDMMC,	///< SD로 연결.
	eSTORAGE_IFACE_SMBCLI,	///< SAMBA 네크워크로 연결.
	eSTORAGE_IFACE_UNKNOWN,

} CO_STORAGE_Iface_e;

/**
 * @enum CO_STORAGE_Location_e
 * @brief Storage 위치.
 */
typedef enum
{
	eSTORAGE_LOC_INTERNAL,	///< STB 내부에 연결된 STORAGE.
	eSTORAGE_LOC_EXTERNAL,	///< 외부 장치 연결을 통해 제공된 STORAGE.
	eSTORAGE_LOC_NETWORK, 	///< 네트워크 연결을 통해 제공된 STORAGE.예) SAMBA
	eSTORAGE_LOC_UNKNOWN,	///< 알수 없는 위치.

} CO_STORAGE_Location_e;

/**
 * @enum CO_STORAGE_ScanFlag_e
 * @brief DMS Scan에 포함되는 DxStorage 정보 포함 여부
 */
typedef enum
{
	eSTORAGE_SCAN_FLAG_NONE				= 0x00000000,
	eSTORAGE_SCAN_FLAG_INT_MEDIA		= 0x00000001, /* Internal M/V/P */
	eSTORAGE_SCAN_FLAG_INT_MEDIA_V 		= 0x00000002,
	eSTORAGE_SCAN_FLAG_INT_MEDIA_M		= 0x00000004,
	eSTORAGE_SCAN_FLAG_INT_MEDIA_P 		= 0x00000008,
	eSTORAGE_SCAN_FLAG_INT_MEDIA_D		= 0x00010000, /* Writable "Download" directory for samba/FTP server */
	eSTORAGE_SCAN_FLAG_INT_RECORD		= 0x00020000,
	eSTORAGE_SCAN_FLAG_2ND_HDD_MEDIA	= 0x00040000, /* Detachable-HDD */
	eSTORAGE_SCAN_FLAG_2ND_HDD_RECORD	= 0x00080000,
	eSTORAGE_SCAN_FLAG_USB_HDD_MEDIA	= 0x00100000, /* USB-HDD */
	eSTORAGE_SCAN_FLAG_USB_HDD_RECORD	= 0x00200000
} CO_STORAGE_ScanFlag_e;

/**
 * @enum CO_STORAGE_Mount_t
 * @brief Mount 정보. 연결된 Storage 내부에 다수의 Partition이 존재할 수 있기에 Mount 형태로 관리.
 */
typedef struct CO_STORAGE_Mount_s {

	HUINT32		ulStorageId;	///< Device ID. 동일 Device라도 재연결시 ID는 바뀐다.
	HCHAR		szUUID[STORAGE_UUID_LEN_MAX];	///< UUID. HOMMA에서는 필요하지 않을 듯...

	HUINT32		ulPartitionId;	///< 구현되지 않은 필드값.

	CO_STORAGE_Type_e		eType;		///< Storage 종류.
	CO_STORAGE_Iface_e 		eInterface;	///< Storage 연결 형태.
	CO_STORAGE_Location_e	eLocation;	///< Storage 위치.

	HUINT32		ulTotalSizeKb;
	HUINT32		ulTotalSizeGb;

	HCHAR	szPath[MOUNT_PATH_LEN_MAX];		///< Mount 경로.

	HBOOL	bPvrFlag;		///< PVR flag.
	HBOOL	IsScanable;

	HCHAR 	szRecordPath[MOUNT_PATH_LEN_MAX];     ///< Reocrding  경로.
	HCHAR 	szMediaPath[MOUNT_PATH_LEN_MAX];

	DxStorage_DevType_e	eDevType;
	const HCHAR  *szDevType;

} CO_STORAGE_Mount_t;

/**
 * @struct CO_STORAGE_Info_t
 * @brief 시스템내 Storages 정보
 */
typedef struct CO_STORAGE_Info_s {
	HINT32 nDevCnt;
	HINT32 nMountCnt;

	HINT32 nDefaultIdx;
	const CO_STORAGE_Mount_t *pstMounts;
} CO_STORAGE_Info_t;

#define UDN_LEN             	( 36 )          /* UDN은 보통 8-4-4-4-12이므로 36이면 된다. */
#define UDN_MAX             	( 48 )          /* Prefix를 포함할 경우의 최대 Length */

#define URL_MAX             	1024
#define CID_SIZE            	512
#define TITLE_SIZE          	128
#define GENRE_SIZE         		32
#define RES_SIZE            	12
#define	IP_SIZE            		20

#define	DTCP_PORT_SIZE			(6)
#define	DTCP_IPV4ADDR_SIZE		(16)

typedef enum
{
	ThumbnailType_No = 0,
	ThumbnailType_Media,
	ThumbnailType_Album,
} CO_ThumbnailType_t;

typedef enum
{
	FolderType_Unknown = 1,
	FolderType_Music,
	FolderType_Video,
	FolderType_Image,
} CO_FolderType_t;

typedef enum {
	//MediaType_Undefiend,
	MediaType_Container,
	MediaType_Video,
	MediaType_Music,
	MediaType_Photo,
	MediaType_Server,
	MediaType_Unknown,

	MediaType_Max,
} CO_MediaType_t;

typedef struct _CO_MediaInstence {
	CO_FolderType_t			eFolderType;
	CO_MediaType_t			eItemType;

	HCHAR                   szUDN[ 48 ];
	HCHAR                   ID[TITLE_SIZE];

	HINT32                  ChildCount;
	HCHAR                   IDofUpper[ TITLE_SIZE];

	/* item name, content name or directory name or file name */
	HCHAR                   Title[TITLE_SIZE];

	/* reource */
	HCHAR                   URI[URL_MAX];
	HCHAR                   ThumbnailURI[URL_MAX];
	CO_ThumbnailType_t		thumbnailType;

	/* media common */
	HLONG                   FileSize;
	HCHAR                   Protocol[URL_MAX];

	/* video / music */
	HINT32                  Bitrate;
	HUINT32                 Duration;

	/* music */
	HINT32                  NrAudioChannels;
	HINT32                  SampleFrequency;

	HCHAR                   AlbumTitle[TITLE_SIZE];
	HCHAR                   Genre[GENRE_SIZE];
	HCHAR                   Artist[TITLE_SIZE];

	/* photo */
	HCHAR                   resolution[RES_SIZE];

	/* DLNA Relative Properties */
	HBOOL                   ByteSeekable;
	HBOOL                   TimeSeekable;

	/* dtcp */
	HCHAR                   dtcpenable;
	HCHAR                   dtcp_portnumber[DTCP_PORT_SIZE];
	HCHAR                   dtcp_ipaddress[DTCP_IPV4ADDR_SIZE];

	/* DirectBrowse */
	HINT32                  totalMatches;

	void					*res_data;
} CO_MediaInstence_t;

/** @brief Transponder를 정의하는 Structure. */
typedef struct CO_TS_INFO_s {
	HINT32				uid;
	HINT32				netuid;
	DxTuneParam_t		tuningParam;
} CO_TS_INFO_t;

/** @brief Channel Network를 정의하는 Structure */
typedef struct CO_NET_INFO_s {
	HINT32				uid;
	HCHAR				name[DxNAME_LEN];
} CO_NET_INFO_t;

/** @brief service info structure, */
typedef struct CO_CHANNEL_Info_s {
	HINT32				nSvcUid;
	HCHAR				name[DxNAME_LEN];

	HINT32				tsuid;
	HINT32				svcid;
	HINT32				tsid;
	HINT32				onid;

	HINT32				lcn;

	DxSvcType_e			svcType;
	DxOrgSvcType_e		orgSvcType;	/* DxOrgSvcType_e */
	HINT32				optype;
	DxDeliveryType_e 	deliType;
	HCHAR				satType[DxANTINFO_NAME_LEN];
	HBOOL				locked;
	DxVideoType_e		highDefinition;
	DxCasType_e			casType;

	//	PID
	HUINT16				usPmtPid;				/**< pmt pid, pat에 정의됨 */
	HUINT16				usPcrPid;				/**< pcr pid from pmt */
	HUINT16				usTtxPid;				/**< teletext pid from pmt */
	HUINT16				usVideoPid;				/**< video pid from pmt */
	HUINT16				usAudioPid;				/**< audio pid from pmt */

	CO_TS_INFO_t		tTsInfo;
	CO_NET_INFO_t		tNetInfo;
} CO_CHANNEL_Info_t;

/** @brief service info structure, 반드시 usTsIdx와 usPrvIdx가 지정되어야 한다. */
typedef struct CO_CHANNELLIST_Info_s {
	HUINT32						unChannelCount;
	HxList_t					*pChannelList;
	DxAntennaConnection_e		eAntConntectType;
} CO_CHANNELLIST_Info_t;

typedef struct
{
	HBOOL				bValid;
	DxService_t 		*svc;
	DxTransponder_t		*ts;
//	DxNetwork_t			*net;
//	DxChannelLogo_t	 	*logo;
} CO_CHANNEL_t;

/** @brief 32비트 handle type definition - handle type을 안 쓰는 거 보단 이렇게 쓰는 게 더 유용한 것 같음 */
typedef	HUINT32         HOM_Handle_t;

typedef enum
{
	FILE_OP_TYPE_ADDED,
	FILE_OP_TYPE_MODIFIED,
	FILE_OP_TYPE_REMOVED,
} CO_FileOperation_Type_e;

typedef struct
{
	CO_FileOperation_Type_e type;
	HCHAR					szPath[1024];
} CO_FileOperation_Event_t;

// crash checking tool
extern unsigned char HOMMA_CheckPreviouslyCrashed(void); // checking if homma was crashed at last
extern void HOMMA_RemoveCrashCheckerFile(void); // remove crash checking file e.g. /mnt/hd2/...

extern void HOMMA_GetPortalURL( char *pszUrl, unsigned int ulLen); // url_portal.txt or default portal url

#endif	/* __COMMON_H__ */

/* end od file */

