typedef enum
{
	eDxSAT_TRANS_DVBS			= 0,
	eDxSAT_TRANS_DVBS2 		= 1
}  DxSat_TransportSpec_e;

typedef enum
{
	eDxSAT_PSK_AUTO			= 0,
	eDxSAT_PSK_QPSK			= 1,
	eDxSAT_PSK_8PSK			= 2
} DxSat_PskModulation_e;

typedef enum
{
	eDxSAT_PILOT_AUTO			= 0,			/**< Pilot : Auto detect */
	eDxSAT_PILOT_OFF			= 1,			/**< Pilot : Off */
	eDxSAT_PILOT_ON			= 2 			/**< Pilot : On */
} DxSat_Pilot_e;

typedef	enum
{
	eDxDELIVERY_TYPE_ALL		= 0x00,
	eDxDELIVERY_TYPE_SAT		= 0x01,
	eDxDELIVERY_TYPE_TER		= 0x02,
	eDxDELIVERY_TYPE_CAB		= 0x04,
	eDxDELIVERY_TYPE_MULTICAST	= 0x08
} DxDeliveryType_b;

typedef DxDeliveryType_b	DxDeliveryType_e;

typedef enum
{
	eDxANT_TYPE_LNB_ONLY			= 0,	/**< LNB only */
	eDxANT_TYPE_DISEQC				= 1,	/**< Diseqc switch, Motor */
	eDxANT_TYPE_SCD					= 2,	/**< Single Cable Distribution */
	eDxANT_TYPE_SMATV 				= 3,	/**< SMATV */
	eDxANT_TYPE_MONOBLOCK_LNB 		= 4
} DxAntennaType_e;
typedef enum
{
	eDxSAT_CODERATE_AUTO			= 0,		/* Code rate : Auto detect */
	eDxSAT_CODERATE_NONE			= 1,		/* DVBS Code Rate */
	eDxSAT_CODERATE_1_2				= 2,		/* Code rate : 1/2 */
	eDxSAT_CODERATE_2_3				= 3,		/* Code rate : 2/3 */
	eDxSAT_CODERATE_3_4				= 4,		/* Code rate : 3/4 */
	eDxSAT_CODERATE_5_6				= 5,		/* Code rate : 5/6 */
	eDxSAT_CODERATE_7_8				= 6,		/* Code rate : 7/8 */

	/* DVBS2 Code Rate */
	eDxSAT_CODERATE_3_5				= 7,		/* Code rate : 3/5 */
	eDxSAT_CODERATE_4_5				= 8,		/* Code rate : 4/5 */
	eDxSAT_CODERATE_5_11			= 9,		/* Code rate : 5/11 */
	eDxSAT_CODERATE_6_7				= 10,		/* Code rate : 6/7 */
	eDxSAT_CODERATE_8_9				= 11,		/* Code rate : 8/9 */
	eDxSAT_CODERATE_9_10			= 12		/* Code rate : 9/10 */

} DxSat_CodeRate_e;

typedef enum
{
	eDxSAT_POLAR_AUTO				= 0,
	eDxSAT_POLAR_HOR				= 1,
	eDxSAT_POLAR_VER				= 2,
	eDxSAT_POLAR_LEFT				= 3,
	eDxSAT_POLAR_RIGHT 				= 4
}  DxSat_Polarization_e;


typedef enum
{
  eDxLNB_VOLT_STD			= 0,			   /**< Vertical : 11v, Horizontal 15v */
  eDxLNB_VOLT_HIGH			= 1 			   /**< Vertical : 12v, Horizontal 16v */
} DxSAT_LnbVoltage_e;

typedef enum
{
	eDxDISEQC_VER_1_0 		= 0,
	eDxDISEQC_VER_1_1 		= 1,
	eDxDISEQC_VER_1_2 		= 2,
	eDxDISEQC_VER_1_3 		= 3,
	eDxDISEQC_VER_2_0 		= 4
} DxSAT_DiseqcVer_e;

typedef enum
{
	eDxDISEQC_INPUT_NONE		= 0,
	eDxDISEQC_INPUT_A 		= 1,
	eDxDISEQC_INPUT_B 		= 2,
	eDxDISEQC_INPUT_C 		= 3,
	eDxDISEQC_INPUT_D 		= 4
} DxSAT_DiseqcInput_e;


typedef enum
{
	eDxTONE_BURST_NONE		= 0,
	eDxTONE_BURST_A			= 1,
	eDxTONE_BURST_B			= 2
} DxSAT_ToneBurst_e;

typedef enum
{
	eDxSCD_INPUT_NONE 		= 0,
	eDxSCD_INPUT_SCD_A		= 1,
	eDxSCD_INPUT_SCD_B		= 2
} DxSAT_ScdInput_e;

typedef enum
{
	eDxTER_BANDWIDTH_8M			= 0,	/**< 8MHz (DVB-T/DVB-H/DMB-T/H). ï¿½ï¿½ï¿? DAB Eureka 147(1.712MHz), IEEE 802.11a(20MHz) */
	eDxTER_BANDWIDTH_7M			= 1,	/**< 7MHz */
	eDxTER_BANDWIDTH_6M			= 2,	/**< 6MHz */
	eDxTER_T2BANDWIDTH_5MHZ		= 3,
	eDxTER_T2BANDWIDTH_10MHZ		= 4,
	eDxTER_T2BANDWIDTH_1P7MHZ		= 5
} DxTer_BandWidth_e;

typedef enum
{
	eDxTER_CONSTELLATION_AUTO	= 0,
	eDxTER_CONSTELLATION_QPSK	= 1,
	eDxTER_CONSTELLATION_16QAM = 2,
	eDxTER_CONSTELLATION_64QAM = 3,
	eDxTER_CONSTELLATION_256QAM= 4

} DxTer_Constellation_e;

typedef enum
{
	eDxTER_HIERACHY_AUTO		= 0,
	eDxTER_HIERACHY_NONE		= 1,
	eDxTER_HIERACHY_1			= 2,
	eDxTER_HIERACHY_2			= 3,
	eDxTER_HIERACHY_4			= 4
} DxTer_Hierachy_e;

typedef enum
{
	eDxTER_CODERATE_AUTO	= 0,
	eDxTER_CODERATE_1_2		= 1,	/**< 1/2 */
	eDxTER_CODERATE_2_3		= 2,	/**< 2/3 */
	eDxTER_CODERATE_3_4		= 3,	/**< 3/4 */
	eDxTER_CODERATE_5_6		= 4,	/**< 5/6 */
	eDxTER_CODERATE_7_8		= 5 	/**< 7/8 */
} DxTer_CodeRate_e;

typedef enum
{
	eDxTER_TRANSMODE_AUTO		= 0,
	eDxTER_TRANSMODE_2K			= 1,	/**< FFT 2048. 1705 subcarriers */
	eDxTER_TRANSMODE_8K			= 2,	/**< FFT 8192. 6817 subcarriers */
	eDxTER_T2TRANSMODE_1K		= 3,
	eDxTER_T2TRANSMODE_4K		= 4,
	eDxTER_T2TRANSMODE_16K 		= 5,
	eDxTER_T2TRANSMODE_32K 		= 6
} DxTer_TransMode_e;

/**< Additional guard interval */
typedef enum
{
	eDxTER_GUARDINTERVAL_AUTO	= 0,
	eDxTER_GUARDINTERVAL_1_4	=	1,	/**< 1/4 */
	eDxTER_GUARDINTERVAL_1_8	= 2,	/**< 1/8 */
	eDxTER_GUARDINTERVAL_1_16	= 3,	/**< 1/16 */
	eDxTER_GUARDINTERVAL_1_32	= 4,	/**< 1/32 */
	eDxTER_T2GUARDINTERVAL_1_128	= 5,	/**< 1/128 */
	eDxTER_T2GUARDINTERVAL_19_128	= 6,	/**< 19/128 */
	eDxTER_T2GUARDINTERVAL_19_256	= 7 /**< 19/256 */

} DxTer_GuardInterval_e;

typedef enum
{
	eDxTER_STREAM_HP			= 0,	/**< HP stream */
	eDxTER_STREAM_LP			= 1 	/**< LP stream */
} DxTer_Stream_e;

typedef enum
{
	eDxTER_OFFSET_AUTO 		= 0,
	eDxTER_OFFSET_NONE 		= 1,	/**< No offset */
	eDxTER_OFFSET_1_6_P		= 2,	/**< +1/6MHz */
	eDxTER_OFFSET_1_6_N		= 3,	/**< -1/6MHz */
	eDxTER_OFFSET_2_6_P		= 4,	/**< +2/6MHz */
	eDxTER_OFFSET_2_6_N		= 5,	/**< -2/6MHz */
	eDxTER_OFFSET_3_6_P		= 6,	/**< +3/6MHz */
	eDxTER_OFFSET_3_6_N		= 7 	/**< -3/6MHz */
} DxTer_Offset_e;
typedef enum
{
	eDxTER_DVB_T			= 0,		/**< T2 stream */
	eDxTER_DVB_T2			= 1 		/**< T stream */
} DxTer_System_e;

typedef enum
{
	eDxTER_SISO			= 0,
	eDxTER_MISO			= 1
} DxTer_PreambleFormat_e;
typedef enum
{
	eDxTER_NOT_MIXED		= 0,
	eDxTER_MIXED			= 1
} DxTer_Mixed_e;
typedef enum
{
	eDxTER_PP_AUTO 	= 0,
	eDxTER_PP1 		= 1,
	eDxTER_PP2 		= 2,
	eDxTER_PP3 		= 3,
	eDxTER_PP4 		= 4,
	eDxTER_PP5 		= 5,
	eDxTER_PP6 		= 6,
	eDxTER_PP7 		= 7,
	eDxTER_PP8 		= 8
} DxTer_Pilot_Pattern_e;

typedef enum
{
	eDxTER_T2NOT_USED		= 0,
	eDxTER_T2USED			= 1
} DxTer_ExtendedCarrier_e;

typedef enum
{
	eDxTER_REDUCTION_NONE			= 0,
	eDxTER_ACE 						= 1,
	eDxTER_TR						= 2,
	eDxTER_AVE_TR					= 3
} DxTer_PAPR_Reduction_e;


typedef enum
{
	eDxCAB_CONSTELLATION_AUTO		= 0,
	eDxCAB_CONSTELLATION_16QAM 		= 1,
	eDxCAB_CONSTELLATION_32QAM 		= 2,
	eDxCAB_CONSTELLATION_64QAM 		= 3,
	eDxCAB_CONSTELLATION_128QAM		= 4,
	eDxCAB_CONSTELLATION_256QAM		= 5,
	eDxCAB_CONSTELLATION_1024QAM	= 6,
	eDxCAB_CONSTELLATION_4096QAM	= 7
} DxCab_Constellation_e;

typedef enum
{
	eDxCAB_SPECTRUM_AUTO			= 0,
	eDxCAB_SPECTRUM_NORMAL 			= 1,
	eDxCAB_SPECTRUM_INVERTED		= 2
} DxCab_Spectrum_e;

typedef enum
{
	eDxSCART_FORMAT_UNKNOWN	= 0x0,
	eDxSCART_FORMAT_CVBS	= 0x1,
	eDxSCART_FORMAT_RGB		= 0x2,
	eDxSCART_FORMAT_SVIDEO	= 0x4
} DxScartFormat_b;

typedef enum
{
	eDxASPECTRATIO_UNKNOWN,
	eDxASPECTRATIO_4X3,
	eDxASPECTRATIO_16X9,
	eDxASPECTRATIO_14X9,
	eDxASPECTRATIO_1X1,
	eDxASPECTRATIO_MAX_NUM
} DxAspectRatio_e;

typedef enum
{
	eINET_IP_CONFIG_DHCP = 0,
	eINET_IP_CONFIG_MANUAL
} INET_IP_CONFIG_e;


typedef enum
{
	eVERSION_TYPE_NEXTOTA,			// OTA Loader¿¡°Ô ¾Ë·ÁÁÖ±â À§ÇÑ UpdateµÉ OTA Image Version
	eVERSION_TYPE_LOADER,
	eVERSION_TYPE_APPL,
	eVERSION_TYPE_EEPROM,
	eVERSION_TYPE_KERNEL,
	eVERSION_TYPE_ROOTFS,
	eVERSION_TYPE_RESOURCE,
	eVERSION_TYPE_DB,
	eVERSION_TYPE_BOOTLOADER

} VersionType_t;

typedef enum
{
	eSWUP_OUI_DVB		= 0x00015A,
	eSWUP_OUI_HUMAX		= 0x000378,
} SWUP_OuiType_e;

typedef enum
{
	eSWUP_INFO_SLOT_0				= 0,
	eSWUP_INFO_SLOT_1,
	eSWUP_INFO_SLOT_2,
	eSWUP_INFO_SLOT_MAX
} SWUP_InfoSlot_e;

typedef enum
{
	eSWUP_DOWNLOAD_DVBSSU = 0,
	eSWUP_DOWNLOAD_DTG,
	eSWUP_DOWNLOAD_HUMAXTYPE,
	eSWUP_DOWNLOAD_SKYD,
	eSWUP_DOWNLOAD_NORDIG,
	eSWUP_DOWNLOAD_FILE,
	eSWUP_DOWNLOAD_MAX
} SWUP_DownloadType_e;

typedef enum
{
	eSWUP_CHANNEL_SAT		= 	1,
	eSWUP_CHANNEL_CAB		= 	2,
	eSWUP_CHANNEL_TER		=	3,
	eSWUP_CHANNEL_FLASH		=	4,
	eSWUP_CHANNEL_FTP		=	5
} SWUP_ChannelType_e;

typedef enum
{
	eSWUP_DATATYPE_INVALID = 0,
	eSWUP_DATATYPE_LOADER,
	eSWUP_DATATYPE_APPL,
	eSWUP_DATATYPE_EEPROM,
	eSWUP_DATATYPE_FLASH,
	eSWUP_DATATYPE_PRODID,				// 5
	eSWUP_DATATYPE_GAME,
	eSWUP_DATATYPE_DVD,
	eSWUP_DATATYPE_FILE					// 8
} SWUP_DataType_e;

typedef enum
{
	eSWUP_FLAG_NO_INFO,
	eSWUP_FLAG_INFO_DETECTED
} SWUP_Flag_e;












