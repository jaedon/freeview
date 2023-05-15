
#ifndef LOADER_PARTITION_H__INCLUDED__
#define LOADER_PARTITION_H__INCLUDED__


/******************************************************************************/

/*        << NVRAM MEMORY MAP >>

      +=============================+
      |                             |
      |                             |
      |      General Partition      |
      |                             |
      |                             |
      +-----------------------------+
      |                             |
      |        CA Partition         |
      |         (128 bytes)         |
      +-----------------------------+
      |                             |
      |      Loader Partition       |
      |                             |
      +-----------------------------+
      |                             |
      |      Irdeto Partition       |
      |         (512 bytes)         |
      +=============================+ 0x0000
*/

/******************************************************************************/

/**
 * Download type.
 */
#if 0
/** (0x00) No download required. */
#define NONE_DOWNLOAD           0x00

/** (0x01) Serial download */
#define SERIAL_DOWNLOAD           0x01

/** (0x02) OTA download */
#define OTA_DOWNLOAD            0x02
#endif
/*****************************************************************************/



/**** DEFINITIONS ************************************************************/

/* Size of Download info */
#define LDR_AREA_DOWNLOAD_INFO_SIZE (14)  /* size of download_info */

/* Size of OTA parameters. */
// by jichoi #define OTA_PARAMETER_SIZE (60)
#define OTA_PARAMETER_SIZE (6/*IP*/ + 2 /*ssu pid */+ LC_MANUFACTURER_PRIVATE_INFO_LENGTH + 4 /*crc32*/ ) // 268byte

/* Size of shared memory. */
#define LDR_SHARED_MEM_SIZE (OTA_PARAMETER_SIZE + 1 + 1 + 4)





/**************************** Function Declarations ***************************/

/**
 * Create a new Loader Partition module.
 * This function is used to create a new Loader Partition module with default content.
 */
lc_result LC_CreateLoaderPartition(void);

/**
 * Read and parse Loader Partition.
 */
lc_result LC_ReadLoaderPartition(lc_loader_pt_st *pLoaderPt);

/**
 * Read and parse Shared memory fields from Loader Partition.
 */
lc_result LC_ReadSharedMemory( lc_shared_memory_st *pSharedMemory );

/**
 * Write Loader Partition.
 */
lc_result LC_WriteSharedMemory( lc_bool createNew, lc_shared_memory_st* pSharedMemory );

/**
 * Get download sequence number stored in Loader Partition.
 */
lc_result LC_GetDownloadSequence(lc_uint16 *pDsn);

/**
 * Check the validity of download sequence number.
 */
lc_result LC_CheckDownloadSequence(lc_uint16 newDsn, lc_uint16 deviceDsn);

/**
 * Store latest download information into Loader Partition.
 * This function is used for saving the key information of a download file,
 * and called after a successful download.
 */
lc_result LC_StoreDownloadInformation(lc_last_download_info_st *pDownloadInformation);

/**
 * Read and parse the download information stored in Loader Partition.
 */
lc_result LC_GetDownloadInformation(lc_last_download_info_st *pDownloadInformation);

#if defined (IP_CLIENT)
/**
 * Verify IP parameters for Multicast download.
 * \note: It can be extended by the manufacturer.
 */
lc_result LC_VerifyIPParameters(const lc_ip_dl_info_st *pIpParameters);
#endif

#if defined (DVB_CLIENT)
/**
 * Verify DVB-SSU parameters for DVB-SSU download.
 * \note: It can be extended by the manufacturer.
 */
lc_result LC_VerifyDVBSSUParameters(const lc_dvbssu_dl_info_st *pDvbParameters);
#endif

/**
 * Verify the integrity of parameters for OTA download.
 * This function is used to check the integrity of the parameters by checking CRC32.
 */
lc_result LC_VerifyOtaDownloadParameters(const lc_ota_dl_para_st *pOtaParameters);

/**
 * Set download flag and download type into Loader Partition.
 * \note: It does not actually modify the Loader Partition stored in the persistent storage.
 */
lc_result LC_SetDownloadIndicator(lc_bool enableDl, lc_uchar dlType, lc_shared_memory_st *pSharedMemory);

/**
 * Set download flag into Loader Partition.
 * \note: It does not actually modify the Loader Partition stored in the persistent storage.
 */
lc_result LC_SetDownloadFlag(lc_bool enableDl, lc_shared_memory_st *pSharedMemory);

/**
 * Set download type into Loader Partition.
 * \note: It does not actually modify the Loader Partition stored in the persistent storage.
 */
lc_result LC_SetDownloadType(lc_uchar dlType, lc_shared_memory_st *pSharedMemory);

/**
 * Get download flag from Loader Partition.
 * \note: It does not actually obtain the Loader Partition stored in the persistent storage,
 * but from a structure variable stored in RAM.
 */
lc_bool LC_GetDownloadFlag(const lc_shared_memory_st *pSharedMemory);

/**
 * Get download type from Loader Partition.
 * \note: It does not actually obtain the Loader Partition stored in the persistent storage,
 * but from a structure variable stored in RAM.
 */
lc_uchar LC_GetDownloadType(const lc_shared_memory_st *pSharedMemory);


#endif /* LOADER_PARTITION_H__INCLUDED__ */

