/*
 * Copyright (c) 2010 Strategy and Technology Limited. All Rights Reserved.
 *
 * Copyright in the whole and every part of this file belongs to
 * Strategy and Technology Limited ("the Owner") and may not be used,
 * sold, supplied, transferred, copied, reproduced in whole or in part,
 * in any manner or form or in or on any media for the benefit of any person
 * other than in accordance with the terms of the Owner's agreement or
 * otherwise without the prior written consent of the Owner.
 *
 */

#if !defined ITK_SSU_H__
#define ITK_SSU_H__

/**
 * @file
 * SSU Interface
 *
 * The SSU interface can be used to download an SSU image from a broadacst data carousel.
 * It signals available downloads to the host and delivers data from selected downloads.
 * See iTK User Guide for details.
 */

#include "itk_types.h"
#include "itk_os.h"

/**SSU Download info type */
typedef enum {
    ITK_INFO_HARDWARE = 0x01,       /**< System Hardware descriptor */
    ITK_INFO_SOFTWARE = 0x02        /**< System Software descriptor */
}itk_info_type_t;

/**SSU download info */
typedef struct itk_ssu_info_t
{
    itk_info_type_t type;           /**< Type of this info struct */
    itk_uint16_t model;             /**< model from GroupInfo */
    itk_uint16_t version;           /**< version from GroupInfo */
    itk_uint8_t subDescriptorCount; /**< sub descriptor count from CompatibilityDescriptor */
    itk_uint8_t const *subDescriptorByte; /**< pointer to sub descriptor loop from CompatibilityDescriptor */
} itk_ssu_info_t;

typedef struct itk_ssu_group_info_t
{
    itk_uint32_t groupId;           /**< id of group, ie download identifier */
    itk_uint16_t numInfos;          /**< number of itk_ssu_info_t elements in infos array */
    itk_ssu_info_t *infos;          /**< infos array */
} itk_ssu_group_info_t;

/**SSU download data message */
typedef struct itk_ssu_download_data_t
{
    size_t total;                   /**< number of total bytes in the download */
    size_t offset;                  /**< number of bytes offset into the download */
    size_t size;                    /**< number of bytes of data in this message */
    void *data;                     /**< the download data */
} itk_ssu_download_data_t;

/**SSU callback message type */
typedef enum itk_ssu_message_type_t
{
    ITK_SSU_GRP_INFO,               /**< download matching OUI found */
    ITK_SSU_GRP_END,                /**< no more downloads matching OUI */
    ITK_SSU_DL_DATA,                /**< download data message type */
    ITK_SSU_DL_ERROR,               /**< download error message */
} itk_ssu_message_type_t;

/**SSU callback message */
typedef union itk_ssu_message_t
{
    itk_ssu_group_info_t grpInfo;   /**< ITK_SSU_GRP_INFO - group info message */
    itk_ssu_download_data_t data;   /**< ITK_SSU_DL_DATA - download data */
    itk_errcode_t error;            /**< ITK_SSU_DL_ERROR - download error */
} itk_ssu_message_t;

/**Interface to control SSU download */
typedef struct itk_ssu_download_t
{
    /**Return service identifier of download.
     * @param[in] thiz  the ssu download interface.
     */
    itk_nsap_t (*nsap)(struct itk_ssu_download_t *thiz);

    /**Return OUI of download.
     * @param[in] thiz  the ssu download interface
     */
    itk_uint32_t (*OUI)(struct itk_ssu_download_t *thiz);

    /**Begin the SSU image download for given SSU info.
     * @param[in] thiz  the ssu download interface.
     * @param[in] info  the ssu download info as returned by the ssu callback.
     */
    itk_present_t (*start)(struct itk_ssu_download_t *thiz, itk_uint32_t groupId);

    /**Stops the current SSU image download.
     * @param[in] thiz  the ssu download interface.
     * @note If no download has been started the 
     */
    void (*stop)(struct itk_ssu_download_t *thiz);

    /**Release ssu download interface.
     * @param[in] thiz  the ssu download interface.
     * @note Any active download should be stopped before releasing the interface.
     */
    void (*release)(struct itk_ssu_download_t *thiz);

} itk_ssu_download_t;

/**Callback function passed to SSU_newDownload 
 * @param[in] context   Application pointer supplied to SSU_newDownload.
 * @param[in] download  SSU download interface pointer returned from SSU_newDownload.
 * @param[in] type      SSU message type.
 * @param[in] msg       SSU download message.
 */
typedef void (*itk_ssu_callback_t) (void *context, itk_ssu_download_t *download, 
                                    itk_ssu_message_type_t type, itk_ssu_message_t *msg);

/**Initilize the SSU client.
 * @param[in] heap   The heap to use. See User Guide for size recommendation.
 */
ITK_EXPORT void ITK_initSSU(itk_heap_t *heap);

/**Terminate the SSU client.
 */
ITK_EXPORT void ITK_termSSU(void);

/**Create download on given service with given OUI.
 * @param[in] heap      Heap for the download session. See User Guide for size recommendation.
 * @param[in] nsap      Service nsap.
 * @param[in] OUI       Manufacturer's unique identifier.
 * @param[in] fn        Application callbacks.
 * @param[in] context   Application pointer to return in callbacks.
 */
ITK_EXPORT itk_ssu_download_t *ITK_newSSUDownload( itk_heap_t *heap,
                                    itk_nsap_t nsap, itk_uint32_t OUI, 
                                    itk_ssu_callback_t fn, void *context);

/**
 * @}
 */
#endif
