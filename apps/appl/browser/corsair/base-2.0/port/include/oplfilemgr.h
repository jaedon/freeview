/*************************************************************************************************************
	File 		: oplfilemgr.h
	author 		: WEB TFT tft_webapp@humaxdigital.com
	comment		:
	date    	: 2012/10/23
	attention 	:

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. (¡°Humax¡±) and
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

#ifndef __eOPL_FILE_MANAGER_H__
#define __eOPL_FILE_MANAGER_H__

#include <list>
#include "hlib.h"

typedef HUINT32 fmgr_ticket_t;	/* handle */

typedef enum{
	eOPL_FILEBRS_STATE_START			= 0,
	eOPL_FILEBRS_STATE_ADD,
	eOPL_FILEBRS_STATE_COMPLETE,
	eOPL_FILEBRS_STATE_ERROR,
} eOplFileBrowserState;

typedef enum{
	eOPL_CMD_COPY							= 0x00010000,
	eOPL_CMD_REMOVE							= 0x00020000,
	eOPL_CMD_MOVE							= 0x00030000,
	eOPL_CMD_RENAME							= 0x00040000,
	eOPL_CMD_MKDIR							= 0X00050000,
	eOPL_CMD_STOP							= 0X00060000,
	eOPL_CMD_SCAN							= 0X00070000,
	eOPL_CMD_RMDIR							= 0x00080000,
	eOPL_CMD_AUTOSCAN						= 0x00090000,
} eOplOperationCommand;

typedef enum{
	eOPL_RET_ADD,
	eOPL_RET_REMOVE,
	eOPL_RET_UPDATE,
} eOplExtMonitor;

typedef enum{
	eOPL_RET_SCANNING,
	eOPL_RET_SCAN_COMPLETE,
} eOplScanSurvey;

typedef enum{
	eOPL_ING_INDEX		= 0,
	eOPL_COMPLETE_INDEX	= 1,
	eOPL_ERROR_INDEX		= 2,

} eOplBaseReturnShift;

typedef enum {
	/*
		eOPL_XXX_XXX_ING : OPL���� ���� �ø��� ���°�
		eOPL_XXX_XXX_COMPLETE
		eOPL_XXX_XXX_ERROR : APPKIT �ݹ��� �޾Ƽ� �ø��� ���°�
	*/
	eOPL_FILEMGR_UNKNOWN_TICKET			= 0x00FFFFFF,//-1,
	eOPL_FILEMGR_STATE_IDLE				= 0x00000000,

	eOPL_FILEMGR_STATE_COPYING			= (eOPL_CMD_COPY   | 0x00000001), // 1
	eOPL_FILEMGR_STATE_COPY_COMPLETE,
	eOPL_FILEMGR_STATE_COPY_ERROR,

	eOPL_FILEMGR_STATE_REMOVING			= (eOPL_CMD_REMOVE | 0x00000004), // 4
	eOPL_FILEMGR_STATE_REMOVE_COMPLETE,
	eOPL_FILEMGR_STATE_REMOVE_ERROR,

	eOPL_FILEMGR_STATE_MOVING			= (eOPL_CMD_MOVE   | 0x00000007), // 7
	eOPL_FILEMGR_STATE_MOVE_COMPLETE,
	eOPL_FILEMGR_STATE_MOVE_ERROR,

	eOPL_FILEMGR_STATE_RENAMING			= (eOPL_CMD_RENAME | 0x0000000A), // 10
	eOPL_FILEMGR_STATE_RENAME_COMPLETE,
	eOPL_FILEMGR_STATE_RENAME_ERROR,

	eOPL_FILEMGR_STATE_MAKING_DIR		= (eOPL_CMD_MKDIR  | 0x0000000D), // 13
	eOPL_FILEMGR_STATE_MAKING_DIR_COMPLETE,
	eOPL_FILEMGR_STATE_MAKING_DIR_ERROR,

	eOPL_FILEMGR_STATE_STOPPING			= (eOPL_CMD_STOP   | 0x00000010), // 16
	eOPL_FILEMGR_STATE_STOP_COMPLETE,
	eOPL_FILEMGR_STATE_STOP_ERROR,

	eOPL_FILEMGR_STATE_SCANNING			= (eOPL_CMD_SCAN   | 0x00000013), // 19
	eOPL_FILEMGR_STATE_SCAN_COMPLETE,
	eOPL_FILEMGR_STATE_SCAN_ERROR,

	eOPL_FILEMGR_STATE_REMOVING_DIR		= (eOPL_CMD_RMDIR  | 0x00000016), // 22
	eOPL_FILEMGR_STATE_REMOVE_DIR_COMPLETE,
	eOPL_FILEMGR_STATE_REMOVE_DIR_ERROR,

	eOPL_FILEMGR_STATE_AUTOSCAN_STARTED	= (eOPL_CMD_AUTOSCAN | 0x000001a),// 25
	eOPL_FILEMGR_STATE_AUTOSCAN_COMPLETE,
	eOPL_FILEMGR_STATE_AUTOSCAN_ERROR,

	eOPL_FILEMGR_STATE_DEVICE_INFO_ERROR = 0x00000050, 	// 80  ����̽� ������ ���� �� ��� ���
	eOPL_FILEMGR_STATE_4G_FAT32,						// 81  fat32 �ý��ۿ��� 4G�̻��� copy�� �Ϸ��� ���
	eOPL_FILEMGR_STATE_LACK_SPACE,						// 82  ���� ����� ���� ���� ������� ���

	eOPL_FILEMGR_STATE_UNKNOWN_TICKET	= 0x000000EE, 					// 22 --> 238
} eOplFileManagerState;

typedef enum
{
	eOPL_FILEMGR_BASE_COPY 		=	eOPL_FILEMGR_STATE_COPYING,
	eOPL_FILEMGR_BASE_REMOVE	= 	eOPL_FILEMGR_STATE_REMOVING,
	eOPL_FILEMGR_BASE_MOVE		= 	eOPL_FILEMGR_STATE_MOVING,
	eOPL_FILEMGR_BASE_RENAME	= 	eOPL_FILEMGR_STATE_RENAMING,
	eOPL_FILEMGR_BASE_MKDIR		= 	eOPL_FILEMGR_STATE_MAKING_DIR,
	eOPL_FILEMGR_BASE_STOP		= 	eOPL_FILEMGR_STATE_STOPPING,
	eOPL_FILEMGR_BASE_SCAN		= 	eOPL_FILEMGR_STATE_SCANNING,
	eOPL_FILEMGR_BASE_RMDIR  	= 	eOPL_FILEMGR_STATE_REMOVING_DIR,
	eOPL_FILEMGR_BASE_AUTOSCAN	=  	eOPL_FILEMGR_STATE_AUTOSCAN_STARTED,
	eOPL_FILEMGR_BASE_UNKNOWN	= 	eOPL_FILEMGR_STATE_UNKNOWN_TICKET,
} eOplFileManagerBaseState;
// plugin data structure.
typedef enum
{
	eFILE_TYPE,
	eFOLDER_TYPE,
	eGROUP_TYPE,
	eUNKNOWN_FILE_TYPE,
} eOPL_FILE_TYPE_e;

typedef enum
{
	eVIDEO_TYPE,
	eMUSIC_TYPE,
	ePHOTO_TYPE,
	eUNKNOWN_MEDIA_TYPE,
} eOPL_FILE_MEDIATYPE_e;

typedef enum
{
	eCONTENT_NOT_WATCHED,
	eCONTENT_WATCHED_HALF,
	eCONTENT_WATCHED_FULL,
} eOPL_FILE_WATCHED_e;

typedef enum
{
	eBROWSE_SORT_ALPHA,
} eOPL_FILE_BROWSE_SortType_e;
typedef enum
{
	eBROWSE_BY_1DEPTH_VIEW = 0,
	eBROWSE_BY_FOLDER_VIEW,
} eOPL_FILE_BROWSE_TYPE_e;

typedef enum
{
	eFILE_MAX_THREAD_STACK_SIZE = 1 * 1024 * 1024,
	eFILE_MAX_PATH_LEN			= 768,
	eFILE_MAX_DATE_LEN			= 32,
	eFILE_MAX_URL_LEN			= 512,
	eFILE_MAX_RESOLUTION_LEN	= 128,
	eFILE_MAX_TEXT_LEN			= 256,
	eFILE_MAX_EXT_LEN			= 256,
} eOPL_FILE_MANAGER_Const_e;

/* End of Content Manager (LocalMedia) **************************************************************/

typedef void * OplBrowsedContentHandle;

typedef void (*OplFileBrowserCallback)(HINT32 event, HUINT32 requestId, HINT32 size, void **contentList);
typedef void (*OplFileSearchCallback)(HINT32 event, HUINT32 requestId, HINT32 size, void **contentList);
typedef void (*OplFileExtensionMonitorCallback)(HINT32 state, void *pszTargetSource);
typedef void (*OplFileScanSurveyCallback)(HINT32 state, void *pszTargetSource, HUINT32 ulRequestId);
typedef void (*OplFileManagerCallback)(fmgr_ticket_t id, eOplFileManagerState state);
typedef void (*OplRecFileManagerCallback)(int ticketKey, eOplFileManagerState state);

typedef fmgr_ticket_t (*OplFileBrowseThreadCall)(void *pParam);

/****************************************************************************************
 * Global Util Function
 ***************************************************************************************/
extern HBOOL IsExist(const char* path);
extern HBOOL isDirectory(const char *path);
extern HBOOL HasFile(const char *path);

/****************************************************************************************
 * OplBrowsedContent
 ***************************************************************************************/
class OplBrowsedContent
{
public:
	OplBrowsedContent(OplBrowsedContentHandle Handle);
	OplBrowsedContent( const OplBrowsedContent &content );
	virtual ~OplBrowsedContent();

	HINT32 				GetContentType()		{ return m_nContentType; 	};
	HINT32 				GetFileType()				{ return m_nFileType; 		};
	void  				SetFileType(int type)		{ m_nFileType = type; 		};
	HCHAR*				GetFullPath()				{ return m_szFullPath;		};
	HUINT64 				GetSize()				{ return m_nSize; 		};
	HCHAR* 				GetDate()					{ return m_szDate;			};
	HINT32				GetDuration()				{ return (m_nDuration < 0 ? 0 : m_nDuration);		};
	HINT32				GetBitRate()				{ return m_nMP3_Bitrate;	};
	HINT32				GetSampleRate()				{ return m_nMP3_SampleRate;	};
	HINT32				GetLastViewTime()		{ return m_nLastViewTime;	};		// not use
	HCHAR*				GetLargeThumbnailUrl()		{return m_szLargeThumbnailUrl;	};
	HCHAR*				GetThumbnailUrl(); // move to cpp file.
	HCHAR*				GetResolution()			{ return m_szResolution;	};
	HCHAR*				GetThumbResolution()	{return m_szThumbResolution;};  	// not use
	HCHAR*				GetPictureDate()			{ return m_szPictureDate;	};
	HCHAR* 				GetAlbum()					{ return m_szAlbum;			};
	HCHAR*				GetArtist()					{ return m_szArtist;		};
	HINT32				GetWatched()				{ return m_nWatched;		};		// not use
	HCHAR*				GetTitle()					{ return m_szTitle;			};
	HCHAR*				GetGenre()					{ return m_szGenre; 		};
	HINT32				GetStorageCategory()		{return m_nStorageCategory;	};
	HBOOL				GetHasSubItem() 			{ return m_bHasSubItem; };

	HCHAR*				GetCompare()				{ return m_pszCompare;		};
	void				SetCompare(HCHAR *pszComp)	{ m_pszCompare = pszComp;	};

private:
	//1 ### use field by  WEB_APPLICATION
	eOPL_FILE_MEDIATYPE_e	m_nContentType;
	HINT32 				m_nStorageCategory;
	HINT32				m_nFileType;
	HUINT64				m_nSize;
	HCHAR				m_szFullPath[eFILE_MAX_PATH_LEN];
	HCHAR				m_szName[eFILE_MAX_PATH_LEN];
	HCHAR 				m_szDate[eFILE_MAX_DATE_LEN];
	HINT32				m_nDuration;
	HINT32 				m_nLastViewTime;
	HCHAR				m_szLargeThumbnailUrl[eFILE_MAX_URL_LEN];
	HCHAR				m_szThumbnailUrl[eFILE_MAX_URL_LEN];
	HCHAR				m_szResolution[eFILE_MAX_RESOLUTION_LEN];
	HCHAR				m_szThumbResolution[eFILE_MAX_RESOLUTION_LEN];
	HCHAR 				m_szPictureDate[eFILE_MAX_DATE_LEN];
	HCHAR				m_szAlbum[eFILE_MAX_TEXT_LEN];
	HCHAR				m_szArtist[eFILE_MAX_TEXT_LEN];
	HCHAR				m_szTitle[eFILE_MAX_TEXT_LEN];
	HCHAR				m_szGenre[eFILE_MAX_TEXT_LEN];
	HINT32				m_nWatched;
	HBOOL				m_bHasSubItem; // for Directory Item
	HBOOL				m_bExistThumb;

	//1 ##sring for sort
	HCHAR*				m_pszCompare;

	//1 ##appkit define AV
	HCHAR				m_szAV_CodecName[32];
	HINT32				m_nAV_StreamType;
	HINT32				m_nAV_Duration;
	HINT64				m_lAV_FrameCount;
	HINT32				m_nAV_Width;
	HINT32 				m_nAV_Height;

	//1 ##appkit define MP3
	HINT32 				m_nMP3_AudioVersion;
	HINT32 				m_nMP3_Protection;
	HINT32 				m_nMP3_Bitrate;
	HINT32 				m_nMP3_SampleRate;

	//1 ##appkit define PNG
	HINT32				m_nPNG_Width;
	HINT32				m_nPNG_Height;
	HUINT32				m_ulPNG_Pitch;
	HUINT8				m_ucPNG_Format;
	HUINT8				m_ucPNG_BitPerPixel;


	//1 ##appkit define JPEG
	HINT32				m_nJPG_Width;
	HINT32				m_nJPG_Height;
	HCHAR				m_szJPG_PictureDate[eFILE_MAX_DATE_LEN];

	//1 ##appkit define RecordedFile
	HINT32				m_ulRecFile_RecDuration;
	HINT32				m_ulRecFile_LastViewTime;
	HCHAR				m_szRecFile_ThumbnailPath[eFILE_MAX_URL_LEN];

	void 				print_Content_Info();
	const HCHAR* 		print_Content_Type( HINT32 status );
	const HCHAR* 		print_File_Type( HINT32 status );

	void 				init_Content_info();
	void 				set_Common_Info(void *pstFsInfo);

	void 				init_ThumbnailPath(HCHAR *pszPath);
	void 				set_ThumbnailUrl_Info(eOPL_FILE_MEDIATYPE_e type);

	HBOOL 				get_SubFile(const HCHAR *path, HCHAR *pszFile);
	void 				set_Content_Type(void *pstFsInfo);

	void 				set_AV_Info(void *pstFsInfo);
	void 				set_MP3_Info(void *pstFsInfo);
	void 				set_PNG_Info(void *pstFsInfo);
	void 				set_JPEG_Info(void *pstFsInfo);
	void 				set_RECORD_Info(void *pstFsInfo);
	void 				set_DIR_Info(void *pstFsInfo);

};
/****************************************************************************************
 * OplFMTicketInfo
 ***************************************************************************************/
class OplFMTicketInfo {
public:
	OplFMTicketInfo() {}
	OplFMTicketInfo(fmgr_ticket_t ticket, eOplOperationCommand cmd)
		: m_ticket(ticket), m_state(eOPL_FILEMGR_STATE_IDLE), m_command(cmd) {}
	virtual ~OplFMTicketInfo() {}

	fmgr_ticket_t 			getTicket() { return m_ticket; }
	void 					setTicket(fmgr_ticket_t ticket) { m_ticket = ticket; }

	eOplFileManagerState 	getState() { return m_state; }
	void 					setState(eOplFileManagerState state) { m_state = state; }

	eOplOperationCommand 	getCommand(){	return m_command;}
private:
	fmgr_ticket_t			m_ticket;
	eOplFileManagerState	m_state;
	eOplOperationCommand	m_command;
};

/****************************************************************************************
 * OplFileBrowser
 ***************************************************************************************/
class OplFileBrowser{
public:
	OplFileBrowser();
	virtual 	~OplFileBrowser();

	static eOplFileBrowserState 	GetState();
	static void 					SetListener(OplFileBrowserCallback callback);
	static fmgr_ticket_t			BrowseContent(const HCHAR *szBrowsePath, HINT32 nBrowsingType, HINT32 nReqItemCount, HCHAR *szFileExt, HINT32 nSortType, const HCHAR *szRootPath);
private:
	static OplFileBrowser			*m_oplFileBrowserInstance;
	static OplFileBrowserCallback	m_callback;

	static eOplFileBrowserState		m_nCurrentState;

	static HCHAR 					m_szBrowsePath[eFILE_MAX_PATH_LEN];
	static HINT32 					m_nBrowseType;
	static HINT32 					m_nReqItemCount;
	static HCHAR 					m_szFileExt[eFILE_MAX_EXT_LEN];
	static HINT32 					m_nSortType;
	static HCHAR 					m_szRootPath[eFILE_MAX_PATH_LEN];

	static void						dama_callback(int requestId, void *pvUserData, HxList_t *pstResultList, void *pvCbInfo);
	static void						umma_callback(HUINT32 handleId, HINT32 event, HINT32 result, void *pvData);
//		static void 					dama_callback(int requestId, HxList_t *pstResultList);//HCHAR *pszPath


	static void 					init_BrowseContext(const HCHAR *szBrowsePath, HINT32 nBrowsingType, HINT32 nReqItemCount, HCHAR *szFileExt, HINT32 nSortType, const HCHAR *szRootPath);
	static void 					send_NotifyMessage(eOplFileBrowserState nMode, HUINT32 requestId, HINT32 size=0, void **pContentList = NULL);

	static fmgr_ticket_t 			browseContent_byOndepth(void *pParam);
	static fmgr_ticket_t 			browseContent_byFolderView(void *pParam);

	/* file browsing by umma process */
	static fmgr_ticket_t 			browseContent_byUmma(void *pParam);
	static HUINT32 					m_nHandleId;
	static HINT32 					m_nIndex;

};

/****************************************************************************************
 * OplFileSearch
 ***************************************************************************************/
class OplFileSearch{
public:
	OplFileSearch();
	virtual 	~OplFileSearch();

	static eOplFileBrowserState 	GetState();
	static void 					SetListener(OplFileBrowserCallback callback);
	static fmgr_ticket_t			SearchContent(const HCHAR *szSearchPath, const HCHAR *szCategory, const HCHAR *szKeyword, HINT32 nReqItemCount, HINT32 nSortType);
private:
	static OplFileSearch			*m_oplFileSearchInstance;
	static OplFileBrowserCallback	m_callback;

	static eOplFileBrowserState		m_nCurrentState;

	static HCHAR 					m_szCategory[eFILE_MAX_TEXT_LEN];
	static HCHAR 					m_szKeyword[eFILE_MAX_TEXT_LEN];
	static HINT32 					m_nReqItemCount;
	static HINT32 					m_nSortType;
	static HBOOL					m_bReqGroup;

	static void						dama_callback(int requestId, void *pvUserData, HxList_t *pstResultList, void *pvCbInfo);

	static void 					init_SearchContext(const HCHAR *szCategory, const HCHAR *szKeyword, HINT32 nReqItemCount, HINT32 nSortType);
	static void 					send_NotifyMessage(eOplFileBrowserState nMode, HUINT32 requestId, HINT32 size=0, void **pContentList = NULL);
	static int						sort_asc( const void *cmp1, const void *cmp2 );
	static int						sort_rasc( const void *cmp1, const void *cmp2 );
};
/****************************************************************************************
 * OplRecFileManager
 ***************************************************************************************/
class OplRecFMReqIdInfo {
public:
	OplRecFMReqIdInfo() {}
 	OplRecFMReqIdInfo(HUINT32 reqId, eOplBaseReturnShift state)
		: m_reqId(reqId), m_state(state) {}
	virtual ~OplRecFMReqIdInfo() {}

	HUINT32					getReqId() { return m_reqId; }
	eOplBaseReturnShift		getState() { return m_state; }

	void					setState(eOplBaseReturnShift state) { m_state = state; }

private:
	unsigned int 			m_reqId;
	eOplBaseReturnShift		m_state;
};

class OplRecFMTicketInfo {
public:
	OplRecFMTicketInfo(const char *id, unsigned int key, bool bDecrypt, eOplOperationCommand cmd);
	~OplRecFMTicketInfo();
	OplRecFMReqIdInfo*		getReqIdInfo(unsigned int id);
	eOplFileManagerState 			getState() { return m_state; }
	unsigned int					getKey() { return m_key; }
	eOplOperationCommand			getCMD() { return m_command; }
	bool							getDecrypt() { return m_decrypt; }
	void 							setState(eOplFileManagerState state) { m_state = state; }
	void							transitState(eOplFileManagerBaseState base);
	void							stopOperation();

	void insertReqId(OplRecFMReqIdInfo *reqInfo){
		m_reqIdList.push_back(reqInfo);
	}
private:
	char*									m_recordingId;
	unsigned int							m_key;
	eOplFileManagerState					m_state;
	bool									m_decrypt;
	eOplOperationCommand					m_command;
	std::list<OplRecFMReqIdInfo *>	m_reqIdList;
};

class OplRecFileManager{
public:
	OplRecFileManager();
	virtual ~OplRecFileManager();

	static void SetListener(OplRecFileManagerCallback callback);
	/**
		@fn 	copyRecording
		@breif	copy recording src files(group) to dst directory
		@param	id recording's url (HPVR://hdd_uid/triple_id;svc_uid;start_time
		@param	ulDevId device index
 		@return identifier, you can use this when you get the event through callback.
				return -1 if errror.
	*/
	static HBOOL CopyRecording (const HCHAR *id, HUINT32 ulDevId, int *pTicketKey);
	static HBOOL MoveRecording (const HCHAR *id, HUINT32 ulDevId, int *pTicketKey);

	/**
		@fn 	removeRecording
		@breif	remove the recording files
		@param	srcPath files's path (group)
		@param	fileName filename
 		@return identifier, you can use this when you get the event through callback.
				return -1 if errror.
	*/
	static HBOOL RemoveRecording(const HCHAR *id, const HCHAR *srcPath, const HCHAR *fileName, int *pTicketKey);

	/**
		@fn 	removeRecording
		@breif	remove the recording files
		@param	srcPath files's path (group)
		@param	fileName filename
 		@return identifier, you can use this when you get the event through callback.
				return -1 if errror.
	*/
	static HBOOL stopRecordingFileOp(int ticketKey, int *pTicketKey);

private:
	static OplRecFMTicketInfo* getRecTicketInfo(unsigned int key);
	static unsigned int		createTicketKey();

	static HERROR			getMountPath (HUINT32 ulDeviceId, HCHAR *szDstPath);
	static HERROR			getRecordPath (HUINT32 ulDeviceId, HCHAR *szDstPath);

	static void				recordings_callback(HERROR err, void *userdata, ...);
	static void				umma_callback(HINT32 nRequestId, void *pvUserData, HxList_t *pstResultList, void *pvCbInfo);
//	static void 			umma_callback(void *info,void *userdata);	// for example

	static void 			send_NotifyMessage(int ticketKey, eOplFileManagerState state);

	static OplRecFileManager						*m_oplRecFileManagerInstance;
	static OplRecFileManagerCallback				m_callback;
	static std::list<OplRecFMTicketInfo *>	m_tickets;
	static HUINT32									m_ticketKey;
};

/****************************************************************************************
 * OplFileManager
 ***************************************************************************************/
class OplFileManager {
public:
	OplFileManager();
	virtual 	~OplFileManager();

	static void 		SetListener(OplFileManagerCallback callback);
	static void 		SetExtMonitorListener(OplFileExtensionMonitorCallback callback);
	static void 		SetScanSurveryListener(OplFileScanSurveyCallback callback);

	static fmgr_ticket_t Rescan(const HCHAR *szScanPath, HBOOL bRecursive, HCHAR *szFileExt);
	static fmgr_ticket_t StartAutoScan( HUINT32 ulCycleTime );
	static HBOOL 		 StopAutoScan( void );
	static HBOOL		 ExtenstionMonitor(const HCHAR *pszExt);
	static fmgr_ticket_t IsScanning(const char *pszPath, HBOOL bOnlyCheck);

	/**
		@fn 	copy
		@breif	copy src file to dst file
		@param	src source file path
		@param	dst destination file path where will be coplied
 		@return identifier, you can use this when you get the event through callback.
				return -1 if errror.
	*/
	static fmgr_ticket_t Copy(const HCHAR *src, const HCHAR *dst, HBOOL bRecursive, HCHAR *ext = NULL, HBOOL decrypt = FALSE);

	/**
		@fn 	copy
		@breif	copy src files(group) to dst directory
		@param	src source files's path (array)
		@param	n source files's count
		@param	dst destination directory path where will be coplied
 		@return identifier, you can use this when you get the event through callback.
				return -1 if errror.
	*/
	static fmgr_ticket_t Copy(const HCHAR *src[], HINT32 n, const HCHAR *dst_dir,  HBOOL bRecursive, HCHAR *ext = NULL, HBOOL decrypt = FALSE);


	/**
		@fn 	move
		@breif	move src file to dst file
		@param	src source file path
		@param	dst destination file path where will be moved
 		@return identifier, you can use this when you get the event through callback.
				return -1 if errror.
	*/
	static fmgr_ticket_t Move(const HCHAR *src, const HCHAR *dst,  HBOOL bRecursive, HCHAR *ext = NULL, HBOOL decrypt = FALSE);

	/**
		@fn 	move
		@breif	move src files(group) to dst directory
		@param	src source files's path
		@param	n source files's count
		@param	dst destination directory path where will be moved
 		@return identifier, you can use this when you get the event through callback.
				return -1 if errror.
	*/
	static fmgr_ticket_t Move(const HCHAR *src[], HINT32 n, const HCHAR *dst_dir,  HBOOL bRecursive, HCHAR *ext = NULL, HBOOL decrypt = FALSE);

	/**
		@fn 	remove
		@breif	remove the file
		@param	src file path
 		@return identifier, you can use this when you get the event through callback.
				return -1 if errror.
	*/
	static fmgr_ticket_t Remove(const HCHAR *path,  HBOOL bRecursive, HCHAR *ext = NULL);

	/**
		@fn 	remove
		@breif	remove the files
		@param	path files's path (group)
		@param	n files's count
 		@return identifier, you can use this when you get the event through callback.
				return -1 if errror.
	*/
	static fmgr_ticket_t Remove(const HCHAR *path[], HINT32 n,  HBOOL bRecursive, HCHAR *ext = NULL);


	/**
		@fn 	rename
		@breif	change the file name
		@param	from file pathname
		@param	to the pathname what you want to change
 		@return identifier, you can use this when you get the event through callback.
				return -1 if errror.
	*/
	static fmgr_ticket_t Rename(const HCHAR *from, const HCHAR *to, HCHAR *ext = NULL);

	/**
		@fn 	mkdir
		@breif	create new directory
		@param	dir the directory path
 		@return identifier, you can use this when you get the event through callback.
				return -1 if errror.
	*/
	static fmgr_ticket_t Mkdir(const HCHAR *dir);
	static fmgr_ticket_t Rmdir(const HCHAR *dir);
	static HUINT64 GetSize(const HCHAR *path, const HCHAR *pExt);

	/**
		@fn 	stop
		@breif 	stop the file operation on @ticket
 		@return true if successful.
	*/
	static fmgr_ticket_t Stop(fmgr_ticket_t ticket);

	/**
		@fn 	getState
		@breif	get state
 		@return current state
	*/
	static eOplFileManagerState GetState(fmgr_ticket_t ticket);

	static void 					send_NotifyMessage(HINT32 ticket, eOplFileManagerState state);
	static eOplFileManagerBaseState	getBaseIndex(eOplOperationCommand eCmd);
private:
	static void 					addListenerToUmma(fmgr_ticket_t ticket);
	static eOplFileManagerState		addTicketInfo(HERROR eError, fmgr_ticket_t ticket, eOplOperationCommand eCmd);
	static OplFMTicketInfo *		getTicketInfo(fmgr_ticket_t ticket);
	static eOplFileManagerState 	makeNotifyStateMsg(eOplOperationCommand eCmd, HINT32 shift);
	static void 					removeListenerFromUmma(fmgr_ticket_t ticket);
	static void						umma_callback(HINT32 nRequestId, void *pvUserData, HxList_t *pstResultList, void *pvCbInfo);
//	static void 					umma_callback(void *info,void *userdata);	// for example
//	static void 					umma_ExtensionMonitorCallback(void *pstInfo, void *userdata);
//	static void 					umma_ScanSurveyCallback(void *pstInfo, void *userdata);

	static OplFileManager					*m_oplFileManagerInstance;
	static OplFileManagerCallback			m_callback;

	static OplFileExtensionMonitorCallback	m_cbExtMonitor;
	static OplFileScanSurveyCallback 		m_cbScanSurvey;
	static std::list<OplFMTicketInfo *>		m_tickets;
};

/****************************************************************************************
 * OplFileOperToken
 ***************************************************************************************/
class OplFileOperToken {
public:
	OplFileOperToken();
	virtual ~OplFileOperToken();
	HERROR 	init( const HCHAR *pszExtFilter );
	HERROR  addPath( const HCHAR *pszPath );
	HERROR  removePath( const HCHAR *pszPath );

	HERROR	copy( const HCHAR *pszPath );
	HERROR	move( const HCHAR *pszPath );
	HERROR	remove(void);
	HERROR	cancel(void);
	HINT32	getRequestId(void) { return m_nReqId; };
	HINT32	setState(eOplFileManagerState eState) { return m_eState = eState; };
	eOplFileManagerState	getState(void) { return m_eState; };
	eOplOperationCommand getCommand(void) { return m_eOperCmd; };
	static OplFileOperToken *getInstance( HINT32 reqId );
private:
	void onCallback(HINT32 nHandleId, HINT32 eEvent, HINT32 nError, void *pvData);
	static void	umma_callback(HINT32 nRequestId, void *pvUserData, HxList_t *pstResultList, void *pvCbInfo);

private:
	HUINT32 m_ulHndId;
	HINT32	m_nReqId;
	HBOOL	m_bUseable;
	HCHAR	m_szFilter[256];
	eOplOperationCommand m_eOperCmd;
	eOplFileManagerState m_eState;
	static	std::list<OplFileOperToken *>	m_instanceList;
};

#endif
