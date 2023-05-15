/*
 * MHEGEngine.h
 */

#ifndef __MHEGENGINE_H__
#define __MHEGENGINE_H__

#include <stdarg.h>
#if !defined(WIN32)
#include <stdbool.h>
#endif
#include <time.h>

#include "ISO13522-MHEG-5.h"
#include "MHEGDisplay.h"
#if 0 // hmkim : commented out (StreamPlayer : CI+ MHEG Profile ø°º± ∫“« ø‰. UK MHEG Profile ¿ª ¿ß«ÿº≠¥¬ √ﬂ∞° ¿€æ˜ « ø‰«‘!)
#include "MHEGVideoOutput.h"
#include "MHEGAudioOutput.h"
#endif
#include "MHEGBackend.h"
#include "MHEGApp.h"
#include "der_decode.h"
#include "listof.h"

/* where to start searching for unused object numbers for clones */
#define FIRST_CLONED_OBJ_NUM		(1<<16)

/* engine ID strings, for GetEngineSupport "UKEngineProfile" feature and "WhoAmI" ResidentProgram */
#define MHEG_RECEIVER_ID	"HMXKHM001"	/* receiver ("mmmcccvvv") */
#define MHEG_ENGINE_ID	"MHGHMX001"	/* MHEG engine ("MHGmmmvvv") */
#define MHEG_DSMCC_ID		"DSMHMX001"	/* DSM-CC client ("DSMmmmvvv") */

/* EventTag key numbers for UserInput events */
/* UK MHEG Profile (D-BOOK 5.0) :
   Register 3 group : Cancel, Text, ƒ√∑Ø≈∞ (Red, Green, Yellow, Blue) -> always available to a running MHEG-5 application.
   Register 4 group : º˝¿⁄≈∞ -> sometimes available to a running MHEG-5 application.
   Register 5 group : Select, πÊ«‚≈∞ (Up, Down, Left, Right) -> sometimes available to a running MHEG-5 application.
   Receiver group : Power, Subtitle, Wide, Info, Guide, Prog+/-, Vol+/-, Mute -> never available to a running MHEG-5 application. */
#define MHEGKey_Up			1
#define MHEGKey_Down		2
#define MHEGKey_Left		3
#define MHEGKey_Right		4
#define MHEGKey_0			5
#define MHEGKey_1			6
#define MHEGKey_2			7
#define MHEGKey_3			8
#define MHEGKey_4			9
#define MHEGKey_5			10
#define MHEGKey_6			11
#define MHEGKey_7			12
#define MHEGKey_8			13
#define MHEGKey_9			14
#define MHEGKey_Select	15
#define MHEGKey_Cancel	16
#define MHEGKey_Help		17	// Info
#define MHEGKey_Red		100
#define MHEGKey_Green		101
#define MHEGKey_Yellow	102
#define MHEGKey_Blue		103
#define MHEGKey_Text		104
#define MHEGKey_EPG		300	// Guide : NZ profile

/* ContentHook values - UK MHEG Profile (D-BOOK 5.0) : section 13.5, Table 13-7 */
#define MHEG_ContentHook_Bitmap_MPEG	2	// MPEG-2 I-frame (MPEG stills)
#define MHEG_ContentHook_Bitmap_PNG		4
#define MHEG_ContentHook_OpenType_Font	10	// TrueType¢‚ outlines published on http://www.microsoft.com/typography/otspec/default.htm and http://partners.adobe.com/asn/tech/type/opentype/index.jsp (CI+ MHEG Profile)
#define MHEG_ContentHook_Text			10
#define MHEG_ContentHook_EntryField		10
#define MHEG_ContentHook_HyperText		10
#define MHEG_ContentHook_Stream_MPEG	10	// StreamClass's Storage will be stream (UK MHEG Profile (D-BOOK 5.0) : Video can only be played from stream and the only StreamComponent that accepts a Storage specification of memory is Audio)
#define MHEG_ContentHook_Stream_File	11	// StreamClass's Storage will be memory (CI+ MHEG Profile : no requirement for stream memory formats)

/* Names within the receiver - UK MHEG Profile (D-BOOK 5.0) : section 18.1, Table 18-1 */
/* Names within the Host - CI+ MHEG Profile (CI+ spec) : section 12.7.1, Table 12.9 */
#define MHEG_REC_SVC_DEF		"rec://svc/def"		// for ContentReference for a Stream object
#define MHEG_REC_SVC_CUR		"rec://svc/cur"		// for ContentReference for a Stream object
#define MHEG_REC_SVC_LCN		"rec://svc/lcn/"	// for ContentReference for a Stream object
#define MHEG_REC_FONT_UK1		"rec://font/uk1"	// for Font attribute of Application class or OriginalFont attribute of Text class
#define MHEG_REC_FONT_CI1		"rec://font/CI1"	// for Font attribute of Application class or OriginalFont attribute of Text class (CI+ MHEG Profile)
#define MHEG_REC_FONT_ci1		"rec://font/ci1"	// for Font attribute of Application class or OriginalFont attribute of Text class (CI+ MHEG Profile)
#define MHEG_RAM_PERSISTENT	"ram://"			// for Name space for persistent storage.
#define MHEG_REC_HTEXT_TOP	"rec://htext/top"	// for Anchor text for navigation boundary events within the HyperText class.
#define MHEG_REC_HTEXT_BOT	"rec://htext/bot"	// for Anchor text for navigation boundary events within the HyperText class.

/* Service gateway location (Carousel root) - UK MHEG Profile (D-BOOK 5.0) : section 16.2.7, 16.2.8, 16.11, 18.3.2.3, Table 18-2 */
#define MHEG_SRG_SRC_DSM		"DSM:"
#define MHEG_SRG_LOC_DSM		"DSM://"
#define MHEG_SRG_SRC_CI		"CI:"
#define MHEG_SRG_LOC_CI		"CI://"

/* URL formats for access to broadcast services - UK MHEG Profile (D-BOOK 5.0) : section 18.3.3 */
#define MHEG_DVB_SERVICE		"dvb://"	// "dvb://<original_network_id>.[<transport_stream_id>].<service_id>" like dvb://abcd.1234.5679 or dvb://abcd..5679

/* persistent storage */
typedef struct
{
	OctetString filename;
	LIST_OF(OriginalValue) *data;
} PersistentData;

DEFINE_LIST_OF(PersistentData);

/* reasons for stopping the current app *//* UK MHEG Profile (D-BOOK 5.0) : 16.2.2 Launching and terminating MHEG-5 applications & CI+ MHEG Profile (CI+ spec) : 12.6 CI Application Life Cycle ¬¸∞Ì«“ ∞Õ */
typedef enum
{
	QuitReason_DontQuit,				/* don't need to quit yet */
	QuitReason_QuitByApp,			/* Quit called in the current app */
	QuitReason_QuitByChannelChange,/* by the receiver(host) following a channel change */
	QuitReason_QuitByCI,				/* CI module generates a RequestStart or AppAbortRequest message */
	QuitReason_QuitByGUI,			/* GUI wants us to exit the programme */
	QuitReason_QuitByConsole,		/* console wants us to exit the programme (≈◊Ω∫∆Æ ƒø∏«µÂ ∂«¥¬ ∆Ø¡§ ≈∞ø° ¿««— ∞≠¡¶ ¡æ∑·øÎ) */
	QuitReason_Launch,				/* Launch called, new app is in engine.quit_data */
	QuitReason_Spawn,					/* Spawn called, new app is in engine.quit_data */
	QuitReason_Retune					/* SI_TuneIndex programme called, channel URL is in engine.quit_data */
} QuitReason;

/* engine profile */
typedef enum
{
	MHEGIB_Profile_Unknown	= 0x00,
	MHEGIB_Profile_CI			= 0x01,
	MHEGIB_Profile_UK			= 0x02,
	MHEGIB_Profile_UK_FSAT	= 0x04,
	MHEGIB_Profile_NZ			= 0x10,
	MHEGIB_Profile_HK			= 0x20
} MHEGIB_Profile;

/* prototypes */
bool MHEGEngine_started(void); /* ø‹∫Œø°º≠ ø£¡¯¿ª ±∏µø«œ±‚ ¿¸ø° ø£¡¯¿Ã ±∏µøµ» ªÛ≈¬¿Œ¡ˆ∏¶ ∆«¥‹«œ±‚ ¿ß«— øÎµµ */
bool MHEGEngine_running(void); /* ø‹∫Œø°º≠ ø£¡¯ø° ∏ﬁΩ√¡ˆ∏¶ ¿¸¥ﬁ«œ±‚ ¿¸ø° ø£¡¯¿Ã ∏ﬁΩ√¡ˆ ºˆΩ≈ ∞°¥…«— ªÛ≈¬¿Œ¡ˆ∏¶ ∆«¥‹«œ±‚ ¿ß«— øÎµµ */

MHEGDisplay *MHEGEngine_getDisplay(void);
#if 0 // hmkim : commented out (StreamPlayer : CI+ MHEG Profile ø°º± ∫“« ø‰. UK MHEG Profile ¿ª ¿ß«ÿº≠¥¬ √ﬂ∞° ¿€æ˜ « ø‰«‘!)
char *MHEGEngine_getAudioOutputDevice(void);
MHEGVideoOutputMethod *MHEGEngine_getVideoOutputMethod(void);
#endif
bool MHEGEngine_avDisabled(void);

void MHEGEngine_TransitionTo(TransitionTo *, OctetString *);

void MHEGEngine_quit(QuitReason, OctetString *);
void MHEGEngine_quitByChannelChange(void);
void MHEGEngine_quitByGui(void);


ApplicationClass *MHEGEngine_getActiveApplication(void);
SceneClass *MHEGEngine_getActiveScene(void);

void MHEGEngine_addVisibleObject(RootClass *);
void MHEGEngine_removeVisibleObject(RootClass *);
void MHEGEngine_bringToFront(RootClass *);
void MHEGEngine_sendToBack(RootClass *);
void MHEGEngine_putBefore(RootClass *, RootClass *);
void MHEGEngine_putBehind(RootClass *, RootClass *);
void MHEGEngine_redrawArea(XYPosition *, OriginalBoxSize *);

void MHEGEngine_addActiveLink(LinkClass *);
void MHEGEngine_removeActiveLink(LinkClass *);

PersistentData *MHEGEngine_findPersistentData(OctetString *, bool);

#if (EVT_VERBOSE < 1)
void MHEGEngine_generateEvent(ExternalReference *, EventType, EventData *);
void MHEGEngine_generateAsyncEvent(ExternalReference *, EventType, EventData *);
#else
void MHEGEngine_generateEvent_(ExternalReference *, EventType, EventData *, char *caller, int line);
void MHEGEngine_generateAsyncEvent_(ExternalReference *, EventType, EventData *, char *caller, int line);
#define MHEGEngine_generateEvent(src, type, data)  MHEGEngine_generateEvent_(src, type, data, (char*)__FUNCTION__, __LINE__)
#define MHEGEngine_generateAsyncEvent(src, type, data)  MHEGEngine_generateAsyncEvent_(src, type, data, (char*)__FUNCTION__, __LINE__)
#endif

void MHEGEngine_addToTempActionQ(ActionClass *, ExternalReference *); // hmkim : org is void MHEGEngine_addToTempActionQ(ActionClass *, OctetString *);

void MHEGEngine_setDERObject(OctetString *);
void MHEGEngine_resolveDERObjectReference(ObjectReference *, ExternalReference *);

void MHEGEngine_addObjectReference(RootClass *);
void MHEGEngine_removeObjectReference(RootClass *);
#if (ENG_VERBOSE < 1)
RootClass *MHEGEngine_findObjectReference(ObjectReference *, OctetString *);
#else
RootClass *MHEGEngine_findObjectReference_(ObjectReference *, OctetString *, char *caller, int line);
#define MHEGEngine_findObjectReference(ref, caller_gid)  MHEGEngine_findObjectReference_(ref, caller_gid, (char*)__FUNCTION__, __LINE__)
#endif

RootClass *MHEGEngine_findGroupObject(OctetString *);
unsigned int MHEGEngine_getUnusedObjectNumber(RootClass *);

extern unsigned long g_ulMhegIbMissingContentSemId;
#define MHEGEngine_enterCriticalMissingContent VK_SEM_Get(g_ulMhegIbMissingContentSemId)
#define MHEGEngine_leaveCriticalMissingContent VK_SEM_Release(g_ulMhegIbMissingContentSemId)

void MHEGEngine_addMissingContent(RootClass *, OctetString *);
void MHEGEngine_removeMissingContent(RootClass *);
void MHEGEngine_pollMissingContent(bool);
MHEGFileObject *MHEGEngine_getReceivedContent(char *);
MHEGFileObject *MHEGEngine_getReceivedBitmap(char *);

bool MHEGEngine_checkContentRef(ContentReference *);
bool MHEGEngine_loadFile(OctetString *, OctetString *);
FILE *MHEGEngine_openFile(OctetString *);
MHEGStream *MHEGEngine_openStream(int, bool, int *, int *, bool, int *, int *);
void MHEGEngine_closeStream(MHEGStream *);
void MHEGEngine_retune(OctetString *);

const OctetString *MHEGEngine_getRecSvcDef(void);
const OctetString *MHEGEngine_getRecSvcCur(void);
const OctetString *MHEGEngine_getRecSvcLcn(OctetString *ref);

bool MHEGEngine_isServiceAvailable(OctetString *);

bool MHEGEngine_isNotAbsoluteFilename(OctetString *);
char *MHEGEngine_absoluteFilename(OctetString *);
char *MHEGEngine_externalFilename(OctetString *);

/* convert PNG to internal format */
MHEGBitmap *MHEGEngine_newBitmap(OctetString *, bool, int);
void MHEGEngine_freeBitmap(MHEGBitmap *);

/* Public functions for MHEG internal browser */
HERROR MHEGIB_Init(void); // init environment for MHEG internal browser
HERROR MHEGIB_SendKey(HINT32 lMessage, HINT32 lParam1, HINT32 lParam2, HINT32 lParam3); // send key to MHEG engine
MHEGIB_Profile MHEGIB_GetProfile(void);

/* Public functions for CI Application MMI */
unsigned short MHEGIB_CiAppMmi_getSessionId(void);
#ifdef CONFIG_DEBUG
void MHEGIB_CiAppMmi_test(unsigned char *name);
#endif

typedef struct
{
	unsigned short			usSessionId;
	unsigned char				ucAppDomainIdLen;
	unsigned char *			pucAppDomainId;	// in case of CI+ : <applicationDomainIndentifier>[;<option1>;<option2>;¬°¬¶;<option#>], <applicationDomainIndentifier> = CI_AMMI_APP_DOMAIN_ID
	unsigned char				ucInitialObjectLen;
	unsigned char *			pucInitialObject;
} sCiAmmiTypeReqStart_t;

// CI_AMMI_TYPE_FILE_ACK
typedef struct
{
	unsigned short			usSessionId;
	unsigned char				ucFileOK;	// 1 ?√å¬æ√Æ¬æ√ü ?¬Ø√à¬ø, 0 ?√å¬∏√© ¬π¬´√à¬ø.
	unsigned int			enReqType;
	//	eMhegIb_ReqType_t		enReqType;
	unsigned long				ulBytesLen;	// if enReqType is CI_AMMI_REQUEST_TYPE_FILE, "1 + file name length (x) + 4 + file data length (y)" in case of using S&T MHEG engine
	unsigned char *			pucBytes;	// if enReqType is CI_AMMI_REQUEST_TYPE_FILE, "1 byte for x + x bytes for file name + 4 bytes for y + y bytes for file data" in case of using S&T MHEG engine
} sCiAmmiTypeFileAck_t;


// CI_AMMI_TYPE_REQ_APP_ABORT
typedef struct
{
	unsigned short				usSessionId;
	unsigned int				enAbortReqCode;
//	eMhegIb_AbortReqCode_t		enAbortReqCode;
} sCiAmmiTypeReqAppAbort_t;

// CI_AMMI_TYPE_APP_ABORT_ACK
typedef struct
{
	unsigned short				usSessionId;
	unsigned char   				ucAbortAckCode;		// CI+ ¬Ω¬∫√Ü√• 1.2, TS 101 699 1.1.1, D-BOOK 5.0 ¬∏√∞¬µ√é ¬∫√ê¬∏√≠√à√∑ ¬∏√≠¬Ω√É¬µ√á¬æ√Æ ?√ñ√Å√∂ ¬æ√ä¬æ√Ü ?√è¬¥√ú ¬∏¬∑¬æ√Ü¬µ√í
} sCiAmmiTypeAppAbortAck_t;

//CI_AMMI_CONTENT
typedef union
{
	sCiAmmiTypeReqStart_t			typeReqStart;
	sCiAmmiTypeFileAck_t				typeFileAck;
	sCiAmmiTypeReqAppAbort_t			typeReqAppAbort;
	sCiAmmiTypeAppAbortAck_t			typeAppAbortAck;
} uCiAmmiContent_t;


typedef enum
{
	eCiAmmiMsgType_reqStart,
	eCiAmmiMsgType_FileAck,
	eCiAmmiMsgType_ReqAppAbort,
	eCiAmmiMsgType_AppAbortAck,
	eCiAmmiMsgType_MAX
} eCiAmmiMsgType_t;

//CI_AMMI_REQUEST_START_ACK_CODE
typedef enum
{
	eCiAmmiRequestStartAckCode_OK 			= 0x01,
	eCiAmmiRequestStartAckCode_WRONG_API	= 0x02,	// Application domain not supported
	eCiAmmiRequestStartAckCode_API_BUSY		= 0x03	// Application domain supported but not currently available
} eCiAmmiRequestStartAckCode_t; // TS 101 699 ¬Ω¬∫√Ü√•

HERROR MHEGIB_CiAppMmi_SendData(eCiAmmiMsgType_t enMsgType, uCiAmmiContent_t *pustContent);

/* for debugging */
char *MHEGEngine_RTTI_name(RunTimeTypeInfo rtti);
void MHEGEngine_print(void);

#endif	/* __MHEGENGINE_H__ */

