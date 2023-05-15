#ifndef DSMCC_RECEIVER_H
#define DSMCC_RECEIVER_H

#include "dsmcc-carousel.h"
#include "dsmcc-biop.h"

struct dsmcc_status;

#define MAXCAROUSELS 16

#define DSMCC_SYNC_BYTE		0x47
#define DSMCC_TRANSPORT_ERROR	0x80
#define DSMCC_START_INDICATOR	0x40

#define DSMCC_MESSAGE_DSI	0x1006
#define DSMCC_MESSAGE_DII	0x1002
#define DSMCC_MESSAGE_DDB	0x1003

#define DSMCC_SECTION_USER_NETWORK_MESSAGE		0x3B
#define DSMCC_SECTION_DOWNLOAD_DATA_MESSAGE		0x3C
#define DSMCC_SECTION_DESCRIPTOR_LIST				0x3D

#define DSMCC_MESSAGE_HEADER_OFFSET			8	// DSMCC 의 UserNetworkMessag에서 message_id가 DSI 와 DII Message 인경우는 dsmccMessageHeader
#define DSMCC_DOWNLOAD_DATA_HEADER_OFFSET	8	// DSMCC 의 DownloadDataMessage 인경우는 dsmccDownloadDataHeader

#define DSMCC_DSI_OFFSET	20
#define DSMCC_DII_OFFSET	20
#define DSMCC_DDB_OFFSET	20

#define DSMCC_BIOP_OFFSET	24

#define BLOCK_GOT(s,i)	(s[i/8]& (1<<(i%8)))
#define BLOCK_SET(s,i)  (s[i/8]|=(1<<(i%8)))

typedef struct dsmcc_module_info {
        unsigned short module_id;
        unsigned long  module_size;
        unsigned char module_version;
        unsigned char module_info_len;
        struct biop_module_info modinfo;
}DsmccModuelInfo_t;


// User Network Message :: DownloadServerInitiate 인 경우..
typedef struct dsmm_message_header
{
        unsigned char		protocol;    		/* 0x11 */
        unsigned char		dsmcctype;              /* 0x03 U-N */
        unsigned short		message_id;      	/* 0x1002 -DDI */
	unsigned long		transaction_id;	// same with download_Id of dsmcc_download_data_header
	unsigned char		adaptation_len;	/* 0x00 or 0x08 */
	unsigned short		message_len;
}DsmccMessageHeader_t;

// Download Data Message :: DownloadDataBlock 인 경우..
typedef struct  dsmcc_download_data_header
{
	unsigned char		protocol;				/* 0x11 */
	unsigned char		dsmcctype;			/* 0x03 */
	unsigned short		message_id;			/* 0x1003 */
	unsigned long		download_id;			// same with transaction_id of dsmcc_message_header
	unsigned char		adaptation_len;		/* 0x00 or 0x08 */
	unsigned short 	message_len;
}DsmccDownloadDataHeader_t;

// User Network Message :: DownloadServerInitiate
typedef struct  dsmcc_dsi
{
        unsigned short 	private_data_len;
        unsigned short 	num_groups;
        BIOP_Ior_t 		stBiopIor;
        unsigned short 	user_data_len;
        unsigned char *	user_data;
}DsmccDsi_t;


// User Network Message :: DownloadInfoIndication
typedef struct dsmcc_dii
{
	unsigned long 		download_id;
	unsigned short 	block_size;
	unsigned long 		tc_download_scenario;
	unsigned short 	number_modules;
	DsmccModuelInfo_t *pstDsmccModules;
	unsigned short private_data_len;
	unsigned char *private_data;
}DsmccDii_t;

// Download Data Message :: DownloadDataBlock
typedef struct dsmcc_ddb
{
	unsigned short 	module_id;
	unsigned char 		module_version;
	unsigned short 	block_number;
	unsigned char 		*blockdata;
	unsigned int 		len;
	struct dsmcc_ddb *next;	/* Needed for caching */
}DsmccDdb_t;

// DSMCC Section
typedef struct dsmcc_section
{
	union {
		DsmccMessageHeader_t 			stDsmccMsgHeader;
		DsmccDownloadDataHeader_t 	stDownloadDataHeader;
	} dsmcc_header;

	union {
		DsmccDsi_t	stDsi;
		DsmccDii_t	stDii;
		DsmccDdb_t	stDdb;
	} dsmcc_msg;
}DsmccSection_t;

typedef struct cache_module_data
{
	unsigned long carousel_id;
	unsigned short module_id;

	unsigned char module_version;
	unsigned long module_size;
	unsigned long ulReadPosition;
	unsigned long ulNextPosition;

	unsigned short block_num;
	char *bstatus;	/* Block status bit field */
	DsmccDdb_t *blocks;
	char cached;

	unsigned char *pucModuleData;
	unsigned short assoc_tag;

	struct cache_module_data *next, *prev;

	DsmccDes_t	*pstDsmccDescriptors;
}CacheModuleData_t;

typedef struct
{
	unsigned char		protocol;    		/* 0x11 */
	unsigned char		dsmcctype;          /* 0x03 U-N */
	unsigned short		message_id;      	/* 0x1002 -DDI */
	unsigned long		transaction_id;		// same with download_Id of dsmcc_download_data_header
	unsigned short		biop_connbinder_assoctag;
	unsigned long		biop_objloc_carouselid;
	unsigned long		biop_connbinder_transactionid;
	unsigned long		biop_connbinder_timeout;
} DsmccDsiInfo_t;

typedef struct
{
	unsigned char		protocol;    		/* 0x11 */
	unsigned char		dsmcctype;          /* 0x03 U-N */
	unsigned short		message_id;      	/* 0x1002 -DDI */
	unsigned long		transaction_id;		// same with download_Id of dsmcc_download_data_header
	unsigned long 		download_id;
	unsigned short		num_module;
	unsigned long		module_total_size;
	unsigned int		num_assoctag;
	unsigned short		biop_module_assoctag[MAXCAROUSELS];
} DsmccDiiInfo_t;

typedef struct
{
	unsigned char		protocol;				/* 0x11 */
	unsigned char		dsmcctype;			/* 0x03 */
	unsigned short		message_id;			/* 0x1003 */
	unsigned long		download_id;			// same with transaction_id of dsmcc_message_header
	unsigned short		module_id;
	unsigned char		module_version;
	unsigned short		block_number;
} DsmccDdbInfo_t;

void dsmcc_objcar_free(ObjectCarousel_t *pstObjCarousel);

void dsmcc_init(struct dsmcc_status *status, const char *channel);
void dsmcc_free(struct dsmcc_status *status);

//void dsmcc_add_module_info(struct dsmcc_status *pstDsmccStatus, DsmccDii_t	*pstDii, ObjectCarousel_t *pstObjCarousel);
//void dsmcc_add_module_data (struct dsmcc_status *pstDsmccStatus, DsmccDownloadDataHeader_t *pstDownloadDataHeader, DsmccDdb_t *pstDdb, unsigned char *pucRawData, unsigned short usPid);
void dsmcc_process_section(struct dsmcc_status *pstDsmccStatus,unsigned char *pucRawData , int nSecLen, unsigned short usPid);

int	dsmcc_parse_dsi_section (unsigned char *pucRawData, int nSecLen, DsmccDsiInfo_t *pstDsiInfo);
int dsmcc_parse_dii_section (unsigned char *pucRawData, int nSecLen, DsmccDiiInfo_t *pstDiiInfo);
int dsmcc_parse_ddb_section (unsigned char *pucRawData, int nSecLen, DsmccDdbInfo_t *pstDdbInfo);

#endif

