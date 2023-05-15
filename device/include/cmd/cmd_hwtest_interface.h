#ifndef __CMD_HWTEST_INTERFACE_H__
#define __CMD_HWTEST_INTERFACE_H__

#include <fcntl.h>
#include "di_channel.h"

#define HWTEST_UNUSED(x) ((void)x)
#define HI_UART_Print	VK_printf

#define HI_KEY_REMOCON_ARROWUP 		DI_KEY_REMOCON_ARROWUP
#define HI_KEY_REMOCON_ARROWDOWN	DI_KEY_REMOCON_ARROWDOWN
#define HI_KEY_REMOCON_ARROWLEFT		DI_KEY_REMOCON_ARROWLEFT
#define HI_KEY_REMOCON_ARROWRIGHT	DI_KEY_REMOCON_ARROWRIGHT
#define HI_KEY_REMOCON_OK				DI_KEY_REMOCON_OK
#define HI_KEY_REMOCON_MENU			DI_KEY_REMOCON_MENU
#define HI_KEY_REMOCON_INFO			DI_KEY_REMOCON_INFO
#define HI_KEY_REMOCON_0				DI_KEY_REMOCON_0
#define HI_KEY_REMOCON_1				DI_KEY_REMOCON_1
#define HI_KEY_REMOCON_2				DI_KEY_REMOCON_2
#define HI_KEY_REMOCON_3				DI_KEY_REMOCON_3
#define HI_KEY_REMOCON_4				DI_KEY_REMOCON_4
#define HI_KEY_REMOCON_5				DI_KEY_REMOCON_5
#define HI_KEY_REMOCON_6				DI_KEY_REMOCON_6
#define HI_KEY_REMOCON_7				DI_KEY_REMOCON_7
#define HI_KEY_REMOCON_8				DI_KEY_REMOCON_8
#define HI_KEY_REMOCON_9				DI_KEY_REMOCON_9
#define HI_KEY_REMOCON_RED			DI_KEY_REMOCON_RED
#define HI_KEY_REMOCON_GREEN			DI_KEY_REMOCON_GREEN
#define HI_KEY_REMOCON_YELLOW			DI_KEY_REMOCON_YELLOW
#define HI_KEY_REMOCON_BLUE			DI_KEY_REMOCON_BLUE
#define HI_KEY_REMOCON_MEDIA			DI_KEY_REMOCON_MEDIA
#define HI_KEY_REMOCON_CHLIST			DI_KEY_REMOCON_CHLIST
#define HI_KEY_REMOCON_GUIDE			DI_KEY_REMOCON_GUIDE
#define HI_KEY_REMOCON_CH_PLUS		DI_KEY_REMOCON_CH_PLUS
#define HI_KEY_REMOCON_CH_MINUS		DI_KEY_REMOCON_CH_MINUS
#define HI_KEY_REMOCON_VOLUMEUP		DI_KEY_REMOCON_VOLUMEUP
#define HI_KEY_REMOCON_VOLUMEDOWN		DI_KEY_REMOCON_VOLUMEDOWN
#define HI_KEY_REMOCON_PLAY	DI_KEY_REMOCON_PLAY
#define HI_KEY_REMOCON_EXIT	DI_KEY_REMOCON_EXIT
#define HI_KEY_REMOCON_STANDBY DI_KEY_REMOCON_STANDBY

typedef void (*CALLBACK_TYPE1)(void*);
typedef unsigned long HI_OSD_HANDLE;

typedef enum
{
	HI_HDMI_EVENT_HOTPLUG_CONNECT=0,
	HI_HDMI_EVENT_HOTPLUG_DISCONNECT,
	HI_HDMI_EVENT_HOTPLUG_POWER_DOWN,
	HI_HDMI_EVENT_HDCP_SUCCESS,
	HI_HDMI_EVENT_HDCP_FAIL,
	HI_HDMI_EVENT_CEC_MSG_RECEIVED,
	HI_HDMI_EVENT_MAX
} HI_HDMI_EVENT;

typedef struct
{
	unsigned char  ucDestAddr;
	unsigned char  ucInitAddr;
	unsigned char  ucMessage[16];
	unsigned char  ucMegLength;
} HI_HDMI_CEC_MSG_t;


typedef enum
{
	HI_VK_SUSPENDTYPE_FIFO,
	HI_VK_SUSPENDTYPE_PRIORITY,
	HI_VK_SUSPENDTYPE_MAX
} HI_VK_SUSPENDTYPE;

typedef	enum
{
	HI_VIDEO_NOTIFY_EVT_SEQUENCE_HEADER_INFO_CHANGED,		/**< Video sequence info event is changed */
	HI_VIDEO_NOTIFY_EVT_FORMAT_CHANGED,						/**< Video format event is changed */
	HI_VIDEO_NOTIFY_EVT_RUNNING_STATE_CHANGED,				/**< Video running state is changed */
	HI_VIDEO_NOTIFY_EVT_AFD_CHANGED,						/**< Video afd info event is changed */
	// IDTV API
	HI_VIDEO_NOTIFY_EVT_ATV_SYNC_CHANGED,					/**< Analog tv video sync status is changed */
	// IDTV API <<
	HI_VIDEO_NOTIFY_EVT_END
}HI_VIDEO_NOTIFY_EVT;

typedef	struct
{
	unsigned long			ulDecId;
	HI_VIDEO_NOTIFY_EVT		eEvent;
	void					*pvEventData; /* Sequence Header Changed, Format Changed, Running State Changed */
} HI_VIDEO_CALLBACK_DATA_t;

typedef enum
{
	HI_VIDEO_RUNNING_STATE_UNKNOWN = 0,				/**< unknown video state */
	HI_VIDEO_RUNNING_STATE_DECODING ,				/**< decoding video state */
	HI_VIDEO_RUNNING_STATE_NODATA					/**< nodata video state */
} HI_VIDEO_RUNNING_STATE;

typedef struct
{
	HI_VIDEO_RUNNING_STATE state;
} HI_VIDEO_RUNNING_STATE_EVT_t;


typedef enum
{
	HI_VIDEO_HD_RESOLUTION_480I = 0,
	HI_VIDEO_HD_RESOLUTION_480P,
	HI_VIDEO_HD_RESOLUTION_576I,
	HI_VIDEO_HD_RESOLUTION_576P,
	HI_VIDEO_HD_RESOLUTION_720P,
	HI_VIDEO_HD_RESOLUTION_1080I,
	HI_VIDEO_HD_RESOLUTION_1080P,
	HI_VIDEO_HD_RESOLUTION_720P_50HZ,
	HI_VIDEO_HD_RESOLUTION_1080I_50HZ,
	HI_VIDEO_HD_RESOLUTION_1080P_50HZ,
	HI_VIDEO_HD_RESOLUTION_480I_J,
	HI_VIDEO_HD_RESOLUTION_1080P_24HZ,
	HI_VIDEO_HD_RESOLUTION_1080P_30HZ,
	HI_VIDEO_HD_RESOLUTION_1080P_60HZ,
	HI_VIDEO_HD_RESOLUTION_2160P_25HZ,
	HI_VIDEO_HD_RESOLUTION_2160P_30HZ,
	HI_VIDEO_HD_RESOLUTION_MAX
}HI_VIDEO_HD_RESOLUTION;

typedef enum
{
	HI_AUDIO_RUNNING_STATE_CHANGED = 0,
	HI_AUDIO_FORMAT_CHANGED,
	HI_AUDIO_MEMORY_STREAM_STARTED,
	HI_AUDIO_MEMORY_STREAM_COMPLETED,
 /* only use it for TV platform*/
	HI_AUDIO_SIF_MODE_CHANGED,
	HI_AUDIO_HEADPHONE_CONNECTION,
	HI_AUDIO_EVENT_NOTIFY_END
}HI_AUDIO_NOTIFY_EVT;

typedef struct
{
	unsigned long			ulDecId;
	unsigned long			ulRequestId;
	HI_AUDIO_NOTIFY_EVT		nEvent;
	void					*pvEventData;	// (DI_AUDIO_FORMAT_CHANGED_EVT_t *) or (DI_AUDIO_RUNNING_STATE_EVT_t *)
}HI_AUDIO_CALLBACK_DATA_t;

typedef enum
{
	HI_AUDIO_STATE_UNKNOWN = 0,
	HI_AUDIO_STATE_DECODING,
	HI_AUDIO_STATE_NODATA
} HI_AUDIO_RUNNING_STATE;

typedef struct HI_AUDIO_RUNNING_STATE_EVT_s {
	HI_AUDIO_RUNNING_STATE state;
} HI_AUDIO_RUNNING_STATE_EVT_t;

typedef enum
{
	HI_SCART_EVENT_VCR_AR_CHANGE,							// STB
	HI_SCART_EVENT_VCR_PWR_CHANGE,						// STB
	HI_SCART_EVENT_TV_CONTROL_CHANGE,						// TV
	HI_SCART_EVENT_MAX
} HI_SCART_EVENT;

typedef struct tagHI_OSD_RECT{
	int		x;
	int		y;
	int		width;
	int		height;
} HI_OSD_RECT;

typedef enum tagHI_OSD_LAYER{
	HI_OSD_LAYER_0,
	HI_OSD_LAYER_1,
	HI_OSD_LAYER_2,
	HI_OSD_LAYER_3,
	HI_OSD_LAYER_4,
	HI_OSD_LAYER_5
} HI_OSD_LAYER;

typedef enum tagHI_OSD_PIXEL_FORMAT{
	HI_OSD_PIXEL_FORMAT_ARGB_8888,
	HI_OSD_PIXEL_FORMAT_RGBA_8888,
	HI_OSD_PIXEL_FORMAT_ARGB_4444,
	HI_OSD_PIXEL_FORMAT_RGBA_4444,
	HI_OSD_PIXEL_FORMAT_ARGB_1555,
	HI_OSD_PIXEL_FORMAT_CLUT_8,
	HI_OSD_PIXEL_FORMAT_A_8,
	HI_OSD_PIXEL_FORMAT_YCBCR_444,
	HI_OSD_PIXEL_FORMAT_YCBCR_422
} HI_OSD_PIXEL_FORMAT;

typedef enum
{
	HI_CI_EXTRACTED = 0,
	HI_CI_INSERTED,
	HI_CI_ENABLED
}HI_CI_STATE;

typedef enum
{
	HI_USB_HID_DEVICE_USBKEYBOARD = 0x1,
	HI_USB_HID_DEVICE_USBMOUSE = 0x2,
	HI_USB_HID_DEVICE_USBJOYSTICK = 0x4
} HI_USB_HID_DEVICE;

#if defined(CONFIG_VK_STDLIB)
struct timeval
{
  long tv_sec;				  // Seconds
  long tv_usec;				 // Microseconds
};
#endif

typedef struct {
	struct timeval time;
	unsigned short type;
	unsigned short code;
	int value;
} HI_USB_HID_DATA;

typedef enum
{
	HI_USB_Event_Insert,
	HI_USB_Event_Extract,
	HI_USB_Event_OverCurrent,
	HI_USB_Event_MAX
}HI_USB_Event_t;

typedef int (* pfnHI_USB_HID_Notify)(HI_USB_HID_DEVICE eDevice, HI_USB_HID_DATA tData);
typedef int (*pfnHI_USB_NotifyFUNC)(HI_USB_Event_t etUsbEventType,void *pucUsbInfo);

typedef int (*pfnHI_ProgressbarUI)(char *pStr, unsigned int percent);
typedef int (*pfnHI_InfoUI)(char *pStr);
typedef int (*pfnHI_MsgUI)(char *pStr);
typedef int (*pfnHI_TimeoutInfoUI)(char *pStr, int strLength, int timeout_sec);


typedef enum
{
	HI_HDD_Event_Pwroff,
	HI_HDD_Event_Pwron
}HI_HDD_Event_t;

typedef enum
{
	HI_HDD_PWR_OFF,
	HI_HDD_PWR_ON
}HI_HDD_PWR;

typedef int (*pfnHI_HDD_NotifyFUNC)(HI_HDD_Event_t etHddEventType,void *pucHddInfo);


typedef enum
{
	HI_CH_CMOD_AUTO		= 0, 		/* Auto order : 64-> 256-> 128-> 32-> 16 */
	HI_CH_CMOD_16QAM	= 1,
	HI_CH_CMOD_32QAM	= 2,
	HI_CH_CMOD_64QAM	= 3,
	HI_CH_CMOD_128QAM	= 4,
	HI_CH_CMOD_256QAM	= 5
} HI_CH_CAB_Modulation_e;

typedef enum
{
	HI_CH_BW_8MHZ 	= 0,
	HI_CH_BW_7MHZ 	= 1,
	HI_CH_BW_6MHZ 	= 2,
	HI_CH_BW_5MHZ 	= 3,
	HI_CH_BW_10MHZ 	= 4,
	HI_CH_BW_1P7MHZ = 5
} HI_CH_TER_BandWidth_e;

typedef enum
{
	HI_CH_TMOD_AUTO		= 0,
	HI_CH_TMOD_QPSK		= 1,
	HI_CH_TMOD_16QAM	= 2,
	HI_CH_TMOD_64QAM	= 3,
	HI_CH_TMOD_256QAM	= 4
} HI_CH_TER_Moudulation_e;

typedef enum
{
	HI_CH_PSK_AUTO = 0,
	HI_CH_PSK_QPSK = 1,
	HI_CH_PSK_8PSK = 2
} HI_CH_SAT_Modulation_e;

typedef enum
{
	HI_CH_DISEQC_INPUT_NONE		= 0,
	HI_CH_DISEQC_INPUT_A		= 1,
	HI_CH_DISEQC_INPUT_B		= 2,
	HI_CH_DISEQC_INPUT_C		= 3,
	HI_CH_DISEQC_INPUT_D		= 4,
	HI_CH_DISEQC_INPUT_TB_A		= 5,
	HI_CH_DISEQC_INPUT_TB_B		= 6,
	HI_CH_DISEQC_INPUT_SCD_A 	= 7,
	HI_CH_DISEQC_INPUT_SCD_B	= 8
} HI_CH_SAT_DiseqcInput_e;

typedef enum
{
	HI_CH_TONE_BURST_NONE 	= 0,
	HI_CH_TONE_BURST_A		= 1,
	HI_CH_TONE_BURST_B		= 2
} HI_CH_SAT_ToneBurst_e;

typedef enum
{
    HI_CH_PILOT_AUTO	= 0,
    HI_CH_PILOT_OFF		= 1,
    HI_CH_PILOT_ON		= 2
} HI_CH_SAT_Pilot_e;

typedef enum
{
	HI_CH_TRANS_DVBS	= 0,
	HI_CH_TRANS_DVBS2	= 1
} HI_CH_SAT_TransportSpec_e;

typedef enum
{
	HI_CH_SAT_RFINPUT_1	= 0,
	HI_CH_SAT_RFINPUT_2	= 1,
	HI_CH_SAT_RFINPUT_3	= 2,
	HI_CH_SAT_RFINPUT_4	= 3
} HI_CH_SAT_RFInput_e;

typedef enum
{
	HI_FRONT_KEY = 0,
	HI_RCU_KEY,
	HI_RF4CE_KEY
} HI_KEY_DEVICE ;

typedef enum
{
	HI_KEY_PRESSED = 0,
	HI_KEY_REPEATED,
	HI_KEY_RELEASED
} HI_KEY_TYPE ;

typedef enum
{
	HI_KEY_FRONT_VOL_DOWN 				= 0x00000001,
	HI_KEY_FRONT_VOL_UP					= 0x00000002,
	HI_KEY_FRONT_CH_DOWN				= 0x00000004,
	HI_KEY_FRONT_CH_UP					= 0x00000008,
	HI_KEY_FRONT_MENU					= 0x00000010,
	HI_KEY_FRONT_INPUT					= 0x00000020,
	HI_KEY_FRONT_PWR					= 0x00000040,
	HI_KEY_FRONT_OK						= 0x00000080,
	HI_KEY_FRONT_DVD_PLAY_PAUSE			= 0x00000100,
	HI_KEY_FRONT_DVD_STOP				= 0x00000200,
	HI_KEY_FRONT_DVD_OPEN_CLOSE			= 0x00000400,
	HI_KEY_FRONT_TV_RADIO				= 0x00000800,
	HI_KEY_FRONT_UP						= 0x00001000,
	HI_KEY_FRONT_DOWN					= 0x00002000,
	HI_KEY_FRONT_LEFT					= 0x00004000,
	HI_KEY_FRONT_RIGHT					= 0x00008000,
	HI_KEY_FRONT_GUIDE					= 0x00010000,
	HI_KEY_FRONT_BACK					= 0x00020000,
	HI_KEY_FRONT_DETAIL_INFO			= 0x00040000,
	HI_KEY_FRONT_HIDDEN					= 0x00080000, /* jc3500, for hidden menu key */
	HI_KEY_FRONT_WPS				= 0x00100000,
	HI_KEY_FRONT_NETWORK				= 0x00200000,
	HI_KEY_FRONT_RESERVED_01			= 0x00400000,
	HI_KEY_FRONT_RESERVED_02			= 0x00800000,
	HI_KEY_FRONT_RESERVED_03			= 0x01000000,
	HI_KEY_FRONT_RESERVED_04			= 0x02000000,
	HI_KEY_FRONT_RESERVED_05			= 0x04000000,
	HI_KEY_FRONT_RESERVED_06			= 0x08000000,
	HI_KEY_FRONT_RESERVED_07			= 0x10000000,
	HI_KEY_FRONT_RESERVED_08			= 0x20000000,
	HI_KEY_FRONT_MAX					= 0x40000000
} HI_KEY_FRONT_VALUES;

typedef enum
{
    HI_SYSTEM_INFO_CHIP_ID = 0
} HI_SYSTEM_INFO;

typedef enum
{
	HI_CH_TYPE_SAT = 0x1,			/**< Satellite channel */
	HI_CH_TYPE_TER = 0x2,			/**< Terrestrial channel */
	HI_CH_TYPE_CAB = 0x4,			/**< Cable channel */
	HI_CH_TYPE_ANA = 0x8			/**< Analog channel */
} HI_CH_Type_e;

typedef enum
{
	HI_DEVINFO_TYPE_CPU = 0,
	HI_DEVINFO_TYPE_HDD,
	HI_DEVINFO_TYPE_FAN,
	HI_DEVINFO_TYPE_BER,
	HI_DEVINFO_TYPE_NET,
	HI_DEVINFO_TYPE_RCU,
	HI_DEVINFO_TYPE_USB,
	HI_DEVINFO_TYPE_MAX
} HI_DEVINFO_TYPE_E;

typedef struct
{
	HI_CH_Type_e channelType;
	unsigned long ul_Frequency;
	unsigned long ul_SymbolRate;
	unsigned long ul_Cable_ModulationValue;
	unsigned long ul_Ter_Bandwidth;
	unsigned long ul_Ter_Modulation;
	unsigned long ul_Sat_DvbMode;
	unsigned long ul_Sat_Modulation;
	unsigned long ul_Sat_FEC;
	unsigned long ul_Sat_Polarity;
	unsigned long ul_Sat_Diseqc;
	unsigned long ul_Sat_ucTone;
	unsigned long ul_Sat_Pilot;
}HI_CH_LockingInfo;

typedef struct
{
	unsigned long ul_Sat_Ber;
	unsigned long ul_Sat_RfAgc;
	unsigned long ul_Sat_IfAgc;
	unsigned long ul_Sat_Uncorrect;
	float f_Sat_Power;
}HI_CH_ChannelStatus;

typedef struct
{
	unsigned long ul_Strength;			/**< Signal strength percentage Unit */
	unsigned long ul_Quality;			/**< Signal quality percentage Unit */
	unsigned long ul_Agc;				/**< AGC */
	float f_Ber;				/**< BER */
	float f_Snr;				/**< SNR */

	unsigned long ul_Emergencyflag;		/**< bEmergencyflag */

	unsigned long ul_CorrectedNo;		/**< ulCorrectedNo */
	unsigned long ul_UnCorrectedNo;	/**< ulUnCorrectedNo */
	unsigned long ul_UnErroredNo;		/**< ulUnErroredNo */
	unsigned long ul_InbandBer;		/**< ulInbandBer */
	unsigned long ul_UnCorrectedFlag;	/**< ulUnCorrectedFlag */

	unsigned long ul_RfAgc;				/**< Dual AGC */
	unsigned long ul_IfAgc;				/**< Dual AGC */

	float f_SignalInputPower;			/**< Uint is Flexible (dBm, dBmV, dBuV) */
}HI_CH_SignalInfo;
typedef enum
{
	HI_CH_SIGNAL_LOCKED			= 0,	/**< Carrier locked */
	HI_CH_SIGNAL_UNLOCK			= 1,	/**< Carrier unlocked */
	HI_CH_SIGNAL_ANTENNA_FAULT	= 2,	/**< LNB fault or Antenna fault */
	HI_CH_SIGNAL_ANTENNA_OK		= 3,	/**< LNB or Antenna fault is recovered */
	HI_CH_SIGNAL_TSMF_GOOD		= 4,	/**< For Japan */
	HI_CH_SIGNAL_TSMF_BAD		= 5,	/**< For Japan */
	HI_CH_SIGNAL_TSMF_EWS_ON	= 6,	/**< For Japan */
	HI_CH_SIGNAL_TSMF_EWS_OFF	= 7		/**< For Japan */
} HI_CH_SignalEvents;
typedef struct
{
	unsigned long				etAntennaType;				/**< DiSEqC Switch type */

	/* DVBS parameters */
	unsigned long								ulFrequency;				/**< tuning frequency : Unit [MHz] */
	unsigned long								ulSymbolRate;				/**< Symbol rate */
	unsigned long					etFecCodeRate;				/**< FEC code rate */
	unsigned long				etPolarization;				/**< Polarization */

	/* DVBS2 parameters */
	unsigned long				etTransSpec;				/**< DBV S/S2 */
	unsigned long 				etModulation;				/**< Modulation */
	unsigned long						etPilot;					/**< S2 Pilot on/off */
	unsigned long					etRollOff;					/**< S2 Roll off */

	/* LNB parameters */
	unsigned long 				etLnbVoltage;				/**< LNB voltage level */

	/* Diseqc parameters */
	unsigned long					etDiseqcVersion;			/**< DiSEqC version */
	unsigned long				etDiseqcInput;				/**< Input position */
	unsigned long					etToneBurst;				/**< Control tone-burst sw */
	unsigned long								b22kTone;					/**< 22KHz tone on/off */

	/* SCD patameters */
	unsigned long 							ulScdMasterUnitId;			/**< SCD Master Unit Id which is used for multi tuner */
	unsigned long 							ulScdUserBandNum;			/**< SCD Band number */
	unsigned long 							ulScdUserBandFreq;			/**< SCD Band Freq. */

} HI_SAT_TuneParam_t;

typedef struct
{
	unsigned long 						ulFrequency;	/**< tuning frequency : Unit [KHz] */
	unsigned long  						ulSymbolRate;	/**< Symbol rate */
	unsigned long  			etModulation;	/**< Modulation */
	unsigned long   			etSpectrum;		/**< spectral inversion */

	unsigned long 							nNetId;			/**< for Japan project */
	unsigned long								nTsId;			/**< for Japan project */
} HI_CAB_TuneParam_t;
typedef struct
{
	unsigned long							ulFrequency;		/**< tuning frequency : Unit [KHz] */
	unsigned long				etBandWidth;		/**< 7MHz or 8 MHz */
	unsigned long			etModulation;		/**< Modulation, TPS : Transmission Parameter Signalling */
	unsigned long				etHierarchy;		/**< TPS : Transmission Parameter Signalling */
	unsigned long				etCodeRate;			/**< TPS : Transmission Parameter Signalling */
	unsigned long				etTransMode;		/**< TPS : Transmission Parameter Signalling */
	unsigned long			etGuardInterval;	/**< TPS : Transmission Parameter Signalling */
	unsigned long					etStream;			/**< HP or LP */
	unsigned long					etOffset;			/**< AFC, frequency offset */
	unsigned long					etTerSystem;		/**< DVB-T or T2 */
	unsigned long							ulPlpId;			/**< DVB-T2 PLP Index */
	unsigned long			etPreambleFormat;
	unsigned long			etPreambleMixed;
	unsigned long			etPilotPattern;
	unsigned long		etExtendedCarrier;
	unsigned long				etPaprMode;
	unsigned long							ulPlpNumBlocks;

} HI_TER_TuneParam_t;

typedef struct
{
	HI_SAT_TuneParam_t satParams;
	HI_CAB_TuneParam_t cabParams;
	HI_TER_TuneParam_t terParams;

} HI_TuneParams;

typedef void ( *pfnHI_CH_NotifySignalInfo )( unsigned long ulInstanceId, unsigned long ulRequestId, HI_CH_Type_e etChannelType , HI_CH_SignalEvents etSignalInfo );

typedef int (* pfnHI_KEY_Notify)(HI_KEY_DEVICE eDevice, unsigned int ulKeyCode, HI_KEY_TYPE eKeyType);
typedef int (* pfnHI_smartCardCallback )(unsigned long nDeivce, unsigned long nStatus);
typedef int (* pfnHI_HDD_Dst_NotifyFUNC)(int result);

void HI_CMD_Init(void);

int HI_EEPROM_Read(unsigned long address, unsigned char *data, unsigned long numToRead);
int HI_EEPROM_Write(unsigned long address, unsigned char *data, unsigned long numToWrite);
int HI_HDMI_EnableOutput(int iEnable);
int HI_HDMI_EnableHDCP(int iEnable);
int HI_HDMI_GetHDResolution(int *hdResolution);
int HI_HDMI_RegisterEventNotify(HI_HDMI_EVENT eEvent, CALLBACK_TYPE1 pfCallback);
int HI_VK_TASK_Create(void (*start_func)(void *), unsigned long prio,unsigned long stack_size, const char *name, void *arg, unsigned long *taskId, int IsJoinable);
int HI_VK_TASK_Destroy(unsigned long taskId);
int HI_VK_TASK_Start(unsigned long taskId);
int HI_VK_TASK_Stop(unsigned long taskId);
int HI_VK_TASK_Sleep(unsigned long ultime);
int HI_VK_MSG_Create(unsigned long q_count, unsigned long msg_size, const char *q_name, unsigned long *q_id, HI_VK_SUSPENDTYPE eSuspendType);
int HI_VK_MSG_Send(unsigned long q_id, const void *msg, unsigned long size);
int HI_VK_MSG_Receive(unsigned long q_id, void *msg, unsigned long size);
int HI_VK_MSG_ReceiveTimeout(unsigned long q_id, void *msg, unsigned long size, unsigned long timeout);
int HI_VK_SEM_Create(unsigned long * sem_id, const char *name, HI_VK_SUSPENDTYPE eSuspendType);
int HI_VK_SEM_Get(unsigned long sem_id);
int HI_VK_SEM_Release(unsigned long sem_id);
void *HI_VK_MEM_Alloc(unsigned long size);
unsigned long HI_VK_MEM_Memcpy(void *dest, void *src, unsigned long size);
unsigned long HI_VK_MEM_Memset(void *dest, unsigned char c, unsigned  long count);
int HI_VK_MEM_Free(void *ap);
int HI_VIDEO_RegisterEventCallback(unsigned long ulDeviceId, HI_VIDEO_NOTIFY_EVT nEvent, CALLBACK_TYPE1 fnVideoCallback, unsigned long* pulCallerId);
int HI_SCART_RegisterEventNotify(HI_SCART_EVENT eScartEvent, CALLBACK_TYPE1 pfScartEventCallback);
int HI_OSD_CreateSurface(HI_OSD_LAYER nOsdLayer, HI_OSD_RECT rect, HI_OSD_PIXEL_FORMAT format, HI_OSD_HANDLE *pOsdHandle);
int HI_OSD_GetFrameBuffer(HI_OSD_HANDLE hOsdHandle, void **pFrameBuffer);
int HI_OSD_GetOsdHandle(HI_OSD_LAYER nOsdLayer, HI_OSD_HANDLE *pOsdHandle);

void HI_ReadTemp(unsigned char* pucTemp);

unsigned int HI_CH_DI_GetLockedInfo(unsigned int ulInstanceId, unsigned char ucChType);

void HI_CH_C_GetSignalInfo(unsigned int ulInstanceId, DI_CH_SignalQuality_t *pstHwTest );
void HI_CH_S_GetSignalInfo(unsigned int ulInstanceId, DI_CH_SignalQuality_t *pstHwTest );
void HI_CH_T_GetSignalInfo(unsigned int ulInstanceId, DI_CH_SignalQuality_t *pstHwTest );

void HI_CreateI2C_TestTask(void);
int HI_CI_RegisterCallback (void (*CiCallbackFunc)(unsigned short usSlotId, unsigned short flag));
int HI_AUDIO_RegisterEventCallback(unsigned long ulDeviceId, HI_AUDIO_NOTIFY_EVT nEvent, CALLBACK_TYPE1 fnAudioCallback, unsigned long* pulCallerId);
void HI_KEY_RegisterKeyEventCallback(pfnHI_KEY_Notify pfnNotify);
int HI_USB_HID_RegisterHIDEventCallback (
	const HI_USB_HID_DEVICE eDevices,
	pfnHI_USB_HID_Notify pfnNotify);
void HI_USB_RegisterCallback(pfnHI_USB_NotifyFUNC pfnNotify);
void HI_HDD_RegisterCallback(pfnHI_HDD_NotifyFUNC pfnNotify);
int HI_HDD_PWR_Ctrl(HI_HDD_PWR OnOff);
int HI_HDD_PowerControl(int portnum, HI_HDD_PWR OnOff);

int HI_Ch_GetNumOfInstance(void);
int HI_CH_S_GetMaxNumOfRfInput(int nNumOfInstance);
void HI_RegisterUIFuncs(pfnHI_ProgressbarUI progFunc, pfnHI_InfoUI infoFunc, pfnHI_MsgUI msgFunc, pfnHI_TimeoutInfoUI timeoutInfoUI);


/* These APIs will be deprecated */
void HI_CH_C_GetLockInfo( unsigned int ulInstanceId, int *nSnr, int *nber_decimal, int *nber_float, int *nber_exp, int *nUncorrected, int *nSignallevel);
void HI_CH_S_GetChannelStatus(unsigned int  ulInstanceId, int *pulBer, unsigned int *pulRfAgc, unsigned int *pulIfAgc, unsigned int *pulUncorrect, float *pfPower);
int HI_GetSystemInfos(HI_SYSTEM_INFO infoType, void *pInfo, unsigned int infoSize);
HINT32 HI_DEVINFO_GetInfo(HI_DEVINFO_TYPE_E type, char *strbuf, int size, unsigned int *ext_data);
#endif
