/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: ts_psi.h $
 * $brcm_Revision: 11 $
 * $brcm_Date: 6/4/12 5:52p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mpeg2_ts_parse/ts_psi.h $
 * 
 * 11   6/4/12 5:52p jgarrett
 * SW7425-3065: Adding BD audio codecs to PSI parser
 * 
 * 11   6/4/12 5:51p jgarrett
 * SW7425-3065: Adding BD audio codecs to PSI parser
 * 
 * 10   12/29/11 3:12p vsilyaev
 * SW7231-500: Merged IEEE-1394 LPCM support
 * 
 * SW7231-500/1   12/29/11 1:11p jgarrett
 * SW7231-500: Adding IEEE-1394 LPCM support
 * 
 * 9   12/14/10 1:31p gmohile
 * SW7422-134 : Add SVC/MVC probe
 * 
 * 8   11/15/10 2:52p gmohile
 * SW35230-2181 : Add DTS Hi-Res profile
 * 
 * 7   3/3/10 12:01p vsilyaev
 * SW7405-3990: Added stream_type for DTS audio
 * 
 * 6   11/23/09 5:51p katrep
 * SW7405-3457:Add DRA support
 * 
 * 5   5/14/09 3:16p haisongw
 * PR55058: Add more ATSC Descriptors
 * 
 * 4   1/20/08 10:59p katrep
 * PR38591: Added support for AVS streams.
 * 
 * 3   7/10/07 3:01p erickson
 * PR31916: update tspsimgr for wider range of specs and codecs.
 * consolidated duplicate code.
 * 
 * 2   1/25/06 5:05p arbisman
 * PR19269: Add support for ISO 14496-3 audio type
 * 
 * 1   2/7/05 11:31p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/3   12/29/04 7:24p marcusk
 * PR13701: Updated with support for H.264 Stream Type (0xB1)
 * 
 * Irvine_BSEAVSW_Devel/2   9/10/03 4:21p marcusk
 * Updated to work with linux get pids.
 * 
 * Irvine_BSEAVSW_Devel/1   8/29/03 5:06p marcusk
 * Initial Version.
 *
 ***************************************************************************/
#ifndef TS_PSI_H__
#define TS_PSI_H__

#ifdef __cplusplus
extern "C" {
#endif

#define TS_PSI_MAX_PSI_TABLE_SIZE     1024
#define TS_PSI_MAX_PRIVATE_PSI_SIZE   4096
#define TS_PSI_TS_CRC_32_SIZE           4
#define TS_PSI_TS_HEADER_SIZE           3

/* Table ID assignment values */
#define TS_PSI_TID_PAS          0
#define TS_PSI_TID_CAS          1
#define TS_PSI_TID_PMS          2
#define TS_PSI_TID_forbidden    0xFF

/* Adaptation Field Control Values */
#define TS_PSI_AFC_payload_only             1
#define TS_PSI_AFC_adaptation_only          2
#define TS_PSI_AFC_adaptation_and_payload   3

/* Stream ID Assignments */
#define TS_PSI_SID_PM               0xBC
#define TS_PSI_SID_private_1        0xBD
#define TS_PSI_SID_padding          0xBE
#define TS_PSI_SID_private_2        0xBF
    /* Audio Streams are numbered from 0xC0 - 0xDF */
    /* Video Streams are numbered from 0xE0 - 0xEF */
#define TS_PSI_SID_ECM              0xF0
#define TS_PSI_SID_EMM              0xF1
#define TS_PSI_SID_DSMCC            0xF2
#define TS_PSI_SID_13522            0xF3
#define TS_PSI_SID_type_A           0xF4
#define TS_PSI_SID_type_B           0xF5
#define TS_PSI_SID_type_C           0xF6
#define TS_PSI_SID_type_D           0xF7
#define TS_PSI_SID_type_E           0xF8
#define TS_PSI_SID_ancillary        0xF9
    /* Reserved streams are numbered from 0xFA - 0xFE */
#define TS_PSI_SID_PSD              0xFF

/* Stream Type Assignments */
/* 0x00 Reserved */
/* MPEG Stream Type Definitions 0x00-0x7F */
#define TS_PSI_ST_11172_2_Video         0x01
#define TS_PSI_ST_13818_2_Video         0x02
#define TS_PSI_ST_11172_3_Audio         0x03
#define TS_PSI_ST_13818_3_Audio         0x04
#define TS_PSI_ST_13818_1_PrivateSection 0x05
#define TS_PSI_ST_13818_1_PrivatePES    0x06
#define TS_PSI_ST_13522_MHEG            0x07
#define TS_PSI_ST_13818_1_DSMCC         0x08
#define TS_PSI_ST_ITU_H222_1            0x09
#define TS_PSI_ST_13818_6_TypeA         0x0A
#define TS_PSI_ST_13818_6_TypeB         0x0B
#define TS_PSI_ST_13818_6_TypeC         0x0C
#define TS_PSI_ST_13818_6_TypeD         0x0D
#define TS_PSI_ST_13818_1_Aux           0x0E
#define TS_PSI_ST_13818_7_AAC           0x0F
#define TS_PSI_ST_14496_2_Video         0x10   
#define TS_PSI_ST_14496_3_Audio         0x11
#define TS_PSI_ST_14496_1_FlexMuxPES    0x12
#define TS_PSI_ST_14496_1_FlexMuxSection 0x13
#define TS_PSI_ST_13818_6_SyncDownload  0x14
#define TS_PSI_ST_MetadataPES           0x15
#define TS_PSI_ST_MetadataSection       0x16
#define TS_PSI_ST_MetadataDataCarousel  0x17
#define TS_PSI_ST_MetadataObjectCarousel 0x18
#define TS_PSI_ST_MetadataSyncDownload  0x19
#define TS_PSI_ST_13818_11_IPMP         0x1A
#define TS_PSI_ST_14496_10_Video        0x1B
#define TS_PSI_ST_14496_10_AnnexG_Video 0x1F
#define TS_PSI_ST_14496_10_AnnexH_Video 0x20

#define TS_PSI_ST_AVS_Video             0x42
#define TS_PSI_ST_AVS_Audio             0x43
#define TS_PSI_ST_DRA_Audio             0xDA
/* Other Stream Type Definitions */
/* 0x80-0xFF used by non-MPEG standards */ 
#define TS_PSI_ST_ATSC_Video            0x80
#define TS_PSI_ST_ATSC_AC3              0x81
#define TS_PSI_ST_IEEE1394_LPCM         0x83
#define TS_PSI_ST_ATSC_DTS_HiRes        0x85
#define TS_PSI_ST_ATSC_DTS              0x86
#define TS_PSI_ST_ATSC_EAC3             0x87
#define TS_PSI_ST_SMPTE_VC1             0xEA

#define TS_PSI_ST_BD_LPCM               0x80
#define TS_PSI_ST_BD_AC3                0x81
#define TS_PSI_ST_BD_DTS                0x82
#define TS_PSI_ST_BD_TRUEHD             0x83
#define TS_PSI_ST_BD_EAC3               0x84
#define TS_PSI_ST_BD_DTS_HD_HRA         0x85
#define TS_PSI_ST_BD_DTS_HD_MA          0x86


/* Descriptor Tag Assignments */
/* MPEG Descriptor Tag Definitions 0x00-0x3F */
/* 0x00 Reserved */
/* 0x01 Reserved */
#define TS_PSI_DT_VideoStream           0x02
#define TS_PSI_DT_AudioStream           0x03
#define TS_PSI_DT_Hierarchy             0x04
#define TS_PSI_DT_Registration          0x05
#define TS_PSI_DT_DataStreamAlign       0x06
#define TS_PSI_DT_TargetBgGrid          0x07
#define TS_PSI_DT_VideoWindow           0x08
#define TS_PSI_DT_CA                    0x09
#define TS_PSI_DT_ISO_639_Language      0x0A
#define TS_PSI_DT_SystemClock           0x0B
#define TS_PSI_DT_MultiplexBufferUtilization 0x0C
#define TS_PSI_DT_Copyright             0x0D
#define TS_PSI_DT_MaximumBitrate        0x0E
#define TS_PSI_DT_PrivateDataIndicator  0x0F
#define TS_PSI_DT_SmoothingBuffer       0x10
#define TS_PSI_DT_STD                   0x11
#define TS_PSI_DT_IBP                   0x12
#define TS_PSI_DT_13818_6_DSMCC_1       0x13
#define TS_PSI_DT_13818_6_DSMCC_2       0x14
#define TS_PSI_DT_13818_6_DSMCC_3       0x15
#define TS_PSI_DT_13818_6_DSMCC_4       0x16
#define TS_PSI_DT_13818_6_DSMCC_5       0x17
#define TS_PSI_DT_13818_6_DSMCC_6       0x18
#define TS_PSI_DT_13818_6_DSMCC_7       0x19
#define TS_PSI_DT_13818_6_DSMCC_8       0x1A
#define TS_PSI_DT_MPEG4_Video           0x1B
#define TS_PSI_DT_MPEG4_Audio           0x1C
#define TS_PSI_DT_IOD                   0x1D
#define TS_PSI_DT_SL                    0x1E
#define TS_PSI_DT_FMC                   0x1F
#define TS_PSI_DT_External_ES_ID        0x20
#define TS_PSI_DT_MuxCode               0x21
#define TS_PSI_DT_FmxBufferSize         0x22
#define TS_PSI_DT_MultiplexBuffer       0x23
#define TS_PSI_DT_ContentLabeling       0x24
#define TS_PSI_DT_MetadataPointer       0x25
#define TS_PSI_DT_Metadata              0x26
#define TS_PSI_DT_MetadataSTD           0x27
#define TS_PSI_DT_AVC                   0x28
#define TS_PSI_DT_13818_11_IPMP         0x29
#define TS_PSI_DT_AVCTimingHRD          0x2A
#define TS_PSI_DT_MPEG2_AAC             0x2B
#define TS_PSI_DT_FlexMuxTiming         0x2C
#define TS_PSI_DT_SVC                   0x30
#define TS_PSI_DT_MVC                   0x31

/* AVS video,can be assigned stream_type of 0x06(private  pes)
   in that case AVS video is identfied by unique descriptor tag.
   AVS Audio is not supported with private pes */
#define TS_PSI_DT_AVS_Video             0x3F

/* Other Stream Type Definitions */
/* 0x40-0x7F used by DVB */ 
#define TS_PSI_DT_DVB_NetworkName       0x40
#define TS_PSI_DT_DVB_ServiceList       0x41
#define TS_PSI_DT_DVB_Stuffing          0x42
#define TS_PSI_DT_DVB_SatelliteDelivery 0x43
#define TS_PSI_DT_DVB_CableDelivery     0x44
#define TS_PSI_DT_DVB_VBI_Data          0x45
#define TS_PSI_DT_DVB_VBI_Teletext      0x46
#define TS_PSI_DT_DVB_BouquetName       0x47
#define TS_PSI_DT_DVB_Service           0x48
#define TS_PSI_DT_DVB_CountryAvailablity 0x49
#define TS_PSI_DT_DVB_Linkage           0x4A
#define TS_PSI_DT_DVB_NVOD_Reference    0x4B
#define TS_PSI_DT_DVB_TimeShiftedService 0x4C
#define TS_PSI_DT_DVB_ShortEvent        0x4D
#define TS_PSI_DT_DVB_ExtendedEvent     0x4E
#define TS_PSI_DT_DVB_TimeShiftedEvent  0x4F
#define TS_PSI_DT_DVB_Component         0x50
#define TS_PSI_DT_DVB_Mosaic            0x51
#define TS_PSI_DT_DVB_StreamIdentifier  0x52
#define TS_PSI_DT_DVB_CA_Indentifier    0x53
#define TS_PSI_DT_DVB_Content           0x54
#define TS_PSI_DT_DVB_ParentalRating    0x55
#define TS_PSI_DT_DVB_Teletext          0x56
#define TS_PSI_DT_DVB_Telephone         0x57
#define TS_PSI_DT_DVB_LocalTimeOffset   0x58
#define TS_PSI_DT_DVB_Subtitling        0x59
#define TS_PSI_DT_DVB_TerrestrialDeliverySystem 0x5A
#define TS_PSI_DT_DVB_MultilingualNetworkName   0x5B
#define TS_PSI_DT_DVB_MultilingualBouquetName   0x5C
#define TS_PSI_DT_DVB_MultilingualServiceName   0x5D
#define TS_PSI_DT_DVB_MultilingualComponent     0x5E
#define TS_PSI_DT_DVB_PrivateDataSpecifier      0x5F
#define TS_PSI_DT_DVB_ServiceMove       0x60
#define TS_PSI_DT_DVB_ShortSmoothingBuffer      0x61
#define TS_PSI_DT_DVB_FrequencyList     0x62
#define TS_PSI_DT_DVB_PartialTransportStream    0x63
#define TS_PSI_DT_DVB_DataBroadcast     0x64
#define TS_PSI_DT_DVB_Scrambling        0x65
#define TS_PSI_DT_DVB_DataBroadcastID   0x66
#define TS_PSI_DT_DVB_TransportStream   0x67
#define TS_PSI_DT_DVB_DSNG              0x68
#define TS_PSI_DT_DVB_PDC               0x69
#define TS_PSI_DT_DVB_AC3               0x6A
#define TS_PSI_DT_DVB_AncillaryData     0x6B
#define TS_PSI_DT_DVB_CellList          0x6C
#define TS_PSI_DT_DVB_CellFrequencyLink 0x6D
#define TS_PSI_DT_DVB_AnnouncementSupport       0x6E
#define TS_PSI_DT_DVB_ApplicationSignalling     0x6F
#define TS_PSI_DT_DVB_AdaptationFieldData       0x70
#define TS_PSI_DT_DVB_ServiceIdentifier 0x71
#define TS_PSI_DT_DVB_ServiceAvailability       0x72
#define TS_PSI_DT_DVB_DefaultAuthority  0x73
#define TS_PSI_DT_DVB_RelatedContent    0x74
#define TS_PSI_DT_DVB_TVA_ID            0x75
#define TS_PSI_DT_DVB_ContentIdentifier 0x76
#define TS_PSI_DT_DVB_TimeSliceFEC      0x77
#define TS_PSI_DT_DVB_ECMRepetitionRate 0x78
#define TS_PSI_DT_DVB_S2SatelliteDeliverySystem 0x79
#define TS_PSI_DT_DVB_EnhancedAC3       0x7A
#define TS_PSI_DT_DVB_DTS               0x7B
#define TS_PSI_DT_DVB_AAC               0x7C
#define TS_PSI_DT_DVB_DRA               0xA0
/* 0x80-0x8F used by ATSC */ 
#define TS_PSI_DT_ATSC_Stuffing         0x80
#define TS_PSI_DT_ATSC_AC3_Audio        0x81
#define TS_PSI_DT_ATSC_SCTE_FrameRate   0x82
#define TS_PSI_DT_ATSC_SCTE_ExtendedVideo       0x83
#define TS_PSI_DT_ATSC_SCTE_ComponentName       0x84
#define TS_PSI_DT_ATSC_Program          0x85
#define TS_PSI_DT_ATSC_CaptionService   0x86
#define TS_PSI_DT_ATSC_ContentAdvisory  0x87
#define TS_PSI_DT_ATSC_CA               0x88
#define TS_PSI_DT_ATSC_Descriptor       0x89
#define TS_PSI_DT_ATSC_SCTE_35_CueIdentifier    0x8A
/* 0x8B Reserved */
#define TS_PSI_DT_ATSC_TimeStamp        0x8C
#define TS_PSI_DT_ATSC_ComponentName    0xA3
#define TS_PSI_DT_ATSC_RedistributionControl    0xAA
#define TS_PSI_DT_ATSC_PrivateInfomation    0xAD
#define TS_PSI_DT_ATSC_ContenIdentifier 0xB6


/* PES Scrambling Control Values */
#define TS_PSI_PES_SCV_not_scrambled    0x0
#define TS_PSI_PES_SCV_even_key         0x2
#define TS_PSI_PES_SCV_odd_key          0x3

typedef const uint8_t * TS_PSI_descriptor;

typedef struct
{
    uint8_t     table_id;
    bool        section_syntax_indicator;
    bool        private_indicator;
    uint16_t    section_length;
    uint16_t    table_id_extension;
    uint8_t     version_number;
    bool        current_next_indicator;
    uint8_t     section_number;
    uint8_t     last_section_number;
    uint32_t    CRC_32;
} TS_PSI_header;

void TS_PSI_getSectionHeader( const uint8_t *buf, TS_PSI_header *p_header );

#ifdef __cplusplus
}
#endif
#endif
/* End of File */
