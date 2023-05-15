/*
 * Copyright (c) 2010 Strategy and Technology Limited. All Rights Reserved.
 *
 * Copyright in the whole and every part of this file belongs to
 * Strategy and Technology Limited ("the Owner") and may not be used,
 * sold, supplied, transferred, copied, reproduced in whole or in part,
 * in any manner or form or in or on any media for the benefit of any person
 * other than in accordance with the terms of the Owner's agreement or
 * otherwise without the prior written consent of the Owner.
 */
#if !defined ITK_DSMCCFS_H__
#define ITK_DSMCCFS_H__

#include "itk_types.h"
#include "itk_os.h"

/**
 * @file
 *
 * DSM-CC File System Interface
 */

/**DSM-CC file interface.  
 * Interface for file objects returned in #itk_dsmccfs_t::newFile
 * and used to load file data.
 */
typedef struct itk_dsmccfs_file_t
{
    /**Loads file data and returns via callback registered in 
     * #itk_dsmccfs_t::newFile
     * @param[in]   thiz        A pointer to the file object.
     * @return ITKE_OK if successful.
     * @return ITKE_FAIL if not successful.
     * @note The callback associated with the file may be called from the context 
     * of the load and before the function has exited.
     */
    itk_errcode_t (*load)(struct itk_dsmccfs_file_t *thiz);

    /**Releases a file handle. 
     * Cancels any pending load and releases all file resources.
     * @param[in]   thiz        A pointer to the file object.
     */
    void (*release)(struct itk_dsmccfs_file_t *thiz);

} itk_dsmccfs_file_t;

/**Container for file data.
 */
typedef struct itk_dsmccfs_file_data_t
{
    size_t total;       /**< number of total bytes in the file */
    size_t offset;      /**< number of bytes offset into file */
    size_t size;        /**< number of bytes of data */
    void *data;         /**< the file data */
} itk_dsmccfs_file_data_t;

/**DSM-CC file system file callback.
 * @param[in]   context     Aplication supplied context pointer.
 * @param[in]   file        A pointer to the file object.
 * @param[in]   result      The result of the file load operation.
 * @param[in]   data        A pointer to the file object data.
 * The result may be:
 * - ITKE_OK if successful.
 * - ITKE_NOT_FOUND if failed becuase the file reference is not valid.
 * - ITKE_NOT_READY if failed because the carousel is not attached.
 * - ITKE_NO_RESOURCE if failed due to insufficient resources.
 * - ITKE_TIMEOUT if failed due to timeout.
 * - ITKE_FAIL if failed for some other reason.
 *
 * The data pointer is only valid if the result is ITKE_OK.
 *
 * Under normal operation the file data will be returned in a single callback,
 * the itk_dsmccfs_file_data_t structure and data will persist in memory until the
 * file is released and the application need not copy the data.  This condition is 
 * indicated by total being equal to size and offset being zero.  
 *
 * If resources are limited the file data may be returned over mutliple callbacks,
 * in which case the itk_dsmccfs_file_data_t structure and data pointer will _NOT_
 * persist after the the callback has exited and the application should copy the 
 * data to local buffers. This condition is indicated by the total being greater
 * than the size.
 */
typedef void (*dsmccfs_file_callback_t)(void *context, struct itk_dsmccfs_file_t *file,
                                        itk_errcode_t result, itk_dsmccfs_file_data_t *data );


/**DSM-CC file system interface.
 * Interface returned from #newDsmccFs and used to create file objects.
 */
typedef struct itk_dsmccfs_t
{
    /**Creates a file object.
     * @param[in]   thiz    A pointer to the file system.
     * @param[in]   name    Fully qualified file name.
     * @param[in]   cb      Callback function.
     * @param[in]   context Callback context to be returned in callback.
     * @return NULL if unsuccessful otherwise a valid file object pointer.
     */
    itk_dsmccfs_file_t *(*newFile)(struct itk_dsmccfs_t *thiz, const char *name, dsmccfs_file_callback_t cb, void *context);

    /**Releases the file system interface.
     * @param[in]   thiz    A pointer to the file system.
     * @note All files should be released by the application before releasing the file system.
     */
    void (*release)(struct itk_dsmccfs_t *thiz);
} itk_dsmccfs_t;

/**Creates a dsmcc file system and returns a file system interface.
 * @param[in]   heap    A pointer to the heap to use for internal allocations.
 * @note If the heap pointer is NULL then the internal heap will be used.
 */
itk_dsmccfs_t *newItkDsmccFs(itk_heap_t *heap);

#endif//ITK_DSMCCFS_H__
