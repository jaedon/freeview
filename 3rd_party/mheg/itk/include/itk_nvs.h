/*
 * Copyright (c) 2008-2009 Strategy & Technology Limited. All Rights Reserved.
 * Copyright in the whole and every part of this file belongs to
 * Strategy & Technology Limited ("the Owner") and may not be used, sold,
 * supplied, transferred, copied, reproduced in whole or in part, in any
 * manner or form or in or on any media for the benefit of any person other
 * than in accordance with the terms of the Owner's agreement or otherwise
 * without the prior written consent of the Owner.
 */
#ifndef ITK_NVS_H__
#define ITK_NVS_H__
/**
 * @file
 * Non-Volatile Storage Interface
 *
 * This file defines the API for the Non-Volatile Storage (NVS) module. The client uses
 * this module for NVM, File System Acceleration and Freesat DVR carousel storage.
 * NVS file systems implement a flat, non-hierarchical architecture, which means that
 * no directories are used. The client may create several file systems, and every
 * file system can contain multiple files.
 * Access to both file systems and file entries is provided via iterators.
 */
#include "itk_types.h"
#define ITK_NVS_METADATA_LENGTH 16

/**
 * Defines the type of a file system. The Host can use this information for storage
 * allocation of a particular file system.
 */
typedef enum
{
    ITK_NVS_FS_DVR = 0, /**< File System used for Freesat DVR */
    ITK_NVS_FS_NVM,     /**< File System used for NVM interface */
    ITK_NVS_FS_FSA,     /**< File System used for File System Acceleration */
    ITK_NVS_FS_RAM,     /**< File System used for D-Book 6 RAM File System */
	ITK_NVS_FS_PST,     /**< File System used for True Persistent Storage */
    ITK_NVS_FS_APL      /**< File System used for launching applications via NVS */
}itk_nvs_fs_type_t;

/**
 * The iTK NVS file iterator interface.
 * This interface implements a mechanism to access all existing files in a file system
 * using iterators in a list-like manner.
 * Some iterator operations, for example itk_nvs_file_t::next(), need a means to signal if
 * no further files can be identified.
 * In order to indentify this, an End-Of-List (EOL) element is defined. An EOL element is a valid
 * element of type itk_nvs_file_t that returns NULL on calls to itk_nvs_file_t::name().
 * Other operations will not be called on iterators pointing to EOL.
 * This mechanism is similar to the end() method provided by C++ containers.
 * An NVS file has a name, data, data size and (for Freesat) metadata.
 */
typedef struct itk_nvs_file_t
{
    /**
     * Release the file iterator.
     *
     * @param thiz The file iterator.
     * @remark The iterator is released but the file persists.
     * To permanently delete a file the client will call itk_nvs_file_t::erase().
     * If thiz points to a partially written file, this call shall discard
     * the file data and the file shall be removed from the file system.
     */
    void (*release)(struct itk_nvs_file_t * thiz);

    /**
     * Moves the iterator to the next file in the file system.
     *
     * @param thiz The NVS file iterator.
     * @return ITK_PRESENT If the iterator points to a valid entry after the call.
     * @return ITK_NOT_PRESENT If the iterator has gone past the last file.
	 * @remark If the call succeeds then further calls on the iterator
     * shall operate on the next file. If the current file is the last
     * one, the iterator will move to EOL and the call returns ITK_NOT_PRESENT.
	 * If the iterator was pointing to a partially written file, that file
	 * shall be discarded and the iterator shall move to next file.
     */
    itk_present_t (* next)(struct itk_nvs_file_t * thiz);

    /**
     * Get the name of the file the iterator is currently pointing to.
     *
     * @param thiz The file iterator
     * @return The name string.
     * @remark An iterator that points to EOL shall return NULL.
     */
    char const * (* name)(struct itk_nvs_file_t * thiz);

    /**
     * Get the data size of the file identified by the iterator.
     *
     * @param thiz The file iterator.
     * @return The file size in bytes.
     */
    size_t (* dataSize)(struct itk_nvs_file_t * thiz);

    /**
     * Write data to the file the iterator is pointing to.
     *
     * @param thiz The file iterator.
     * @param dataSize The number of bytes being written
     * @param buffer A pointer to the data buffer to write.
     * @return ITKE_OK on success,
     * @return ITKE_FAIL if the write failed.
     * @remark File content can be written in one step or in several steps, i.e. it might
     * take several calls to this function before a file is complete.
     * A file is only complete when the sum of all written data, identified by parameter data_size,
     * is equal to the file size passed to function itk_nvs_fs_t::newNvsFile().
     * Partially written file data is always provided in sequence. Only complete files can be read.
     * If an iterator pointing to a partially written file is released,
     * the file data shall be discarded and the file shall be removed from the file system.
     */
    itk_errcode_t (* write)(struct itk_nvs_file_t * thiz, size_t data_size, void const * buffer);

    /**
     * Read file data into memory.
     *
     * @param thiz The file iterator.
     * @param buffer A buffer supplied by the client. The buffer size is determined
     * through a call to itk_nvs_file_t::dataSize().
     * @return ITKE_OK on success.
     * @return ITKE_FAIL if the read failed.
     */
    itk_errcode_t (* read)(struct itk_nvs_file_t * thiz, itk_uint8_t * buffer);

    /**
     * Read the file metadata.
     *
     * @param thiz The file iterator.
     * @param metadata Pointer to a buffer of size ITK_NVS_METADATA_LENGTH,
     * which will be filled as a result of this call.
     * @return ITKE_OK on success.
     * @return ITKE_FAIL if the read failed.
     */
    itk_errcode_t (* readMetadata)(struct itk_nvs_file_t * thiz, itk_uint8_t * metadata);

    /**
     * Write the file metadata.
     *
     * @param thiz The file iterator.
     * @param metadata Pointer to a buffer of size ITK_NVS_METADATA_LENGTH.
     * @return ITKE_OK on success.
     * @return ITKE_FAIL if the write failed.
     */
    itk_errcode_t (* writeMetadata)(struct itk_nvs_file_t * thiz,
                                    itk_uint8_t const * metadata);

    /**
     * Erases the file pointed to by the iterator and advances the
     * iterator to the next file.
     *
     * @param thiz The file iterator.
     * @return ITK_PRESENT If the iterator points to a valid file after the deletion.
     * @return ITK_NOT_PRESENT Either the deletion of the file or advancement 
	 * to the next valid file failed.
	 *
	 * @remark If the iterator is pointing to a valid file then this call
     * shall permanently remove the file its metadata and advance
     * the iterator to the next file. 
     */
    itk_present_t (* erase)(struct itk_nvs_file_t * thiz);
} itk_nvs_file_t;

/**
 * NVS file system iterator interface.
 * This interface implements a mechanism to access all existing file systems using iterators
 * in a list-like manner.
 * Some iterator operations, for example itk_nvs_fs_t::next(), need a means to signal if
 * no further file system can be identified.
 * In order to indentify this, an End-Of-List (EOL) element is defined. An EOL element is a valid
 * element of type itk_nvs_fs_t that returns NULL on calls to itk_nvs_fs_t::name().
 * Other operations will not be called on iterators pointing to EOL.
 * This mechanism is similar to the end() method provided by C++ containers.
 */
typedef struct itk_nvs_fs_t
{
    /**
     * Release the NVS file system iterator.
     *
     * The iterator is released but the file system persists. To permanently
     * remove a file system the client will call itk_nvs_fs_t::erase().
     *
     * @param thiz The NVS file system iterator.
     */
    void (* release)(struct itk_nvs_fs_t * thiz);

    /**
     * Moves the file system iterator to the next file system.
     *
     * @param thiz The NVS file system iterator.
     * @return ITK_PRESENT If the iterator is advanced to a valid file system.
     * @return ITK_NOT_PRESENT If the iterator does not point to a valid file system.
	 *
	 * @remark This call causes the iterator to point to the next available file 
	 * system. If the call succeeds then further calls to iterator functions
	 * shall operate on the next available file system.
	 * If the iterator has traversed over all existing file systems, the call
	 * returns ITK_NOT_PRESENT and the iterator points at EOL.
     */
    itk_present_t (* next)(struct itk_nvs_fs_t * thiz);

    /**
     * Creates a file iterator that points at the specified file.
     *
     * @param thiz The NVS file system iterator.
     * @param name File name.
     * @return The file iterator.
     * @remark If name is NULL then the newly created iterator shall point to the first
     * in the file system or, if no file exists, the function should return
     * a NULL pointer. If the name identifies an existing file, the iterator shall point
     * to that file. If no file with the specified name exists then this call 
	 * will return NULL.
	 */
    itk_nvs_file_t * (* getNvsFile)(struct itk_nvs_fs_t * thiz, const char *name);

    /**
     * Creates a new file and returns a file iterator pointing to it.
     *
     * @param thiz The NVS file system iterator.
     * @param name The new file name.
     * @param metadata The new file's metadata. metadata can be NULL. 
     * @param dataSize The file size.
     * @return A file iterator.
     * @remark If no file with the same name exists then the function shall create
     * a new file and return a file iterator pointing to it.
     * If a file with the same name exists then the function shall return NULL. 
	 */
    itk_nvs_file_t * (* newNvsFile)(struct itk_nvs_fs_t * thiz,
                            char const * entryName, itk_uint8_t const * metadata,
                            size_t dataSize);

    /**
     * Returns the type of the file system currently pointed to by thiz.
     *
     * @param thiz The NVS file system iterator.
     * @return File system type.
     */
    itk_nvs_fs_type_t (*type)(struct itk_nvs_fs_t * thiz);

    /**
     * Returns the name of the file system currently identified by the file system iterator.
     *
     * @param thiz The NVS file system iterator.
     * @return the NULL-terminated name string.
     * @remark An iterator that points to EOL shall return NULL.
     */
    const char *(*name)(struct itk_nvs_fs_t * thiz);

    /**
     * Get free data space.
     *
     * @param thiz The NVS file system iterator.
     * @return The data space available on the file system.
     * @remark The free space is the amount of memory available to store data + metadata.
     * Unused memory not available for storage due to the use of block-based storage
     * devices is not considered.
     */
    size_t (* freeSpace)(struct itk_nvs_fs_t * thiz);

    /**
     * Returns the file system metadata size.
     *
     * @param thiz The NVS file system. 
     * @remark This call shall return 0 if no metadata is available for this file system.
     */
    size_t (*metadataSize)(struct itk_nvs_fs_t * thiz);

    /**
     * Reads the file system metadata into the supplied buffer.
     *
     * @param thiz      The NVS file system iterator.
     * @param buffer    Buffer to copy the file system metadata into.
     * @param bufferLen Buffer length.
     * @return ITKE_OK on success,
     * @return ITKE_FAIL if no metadata are available or the call fails for
     * other reasons.
     */
    itk_errcode_t (* readMetadata)(struct itk_nvs_fs_t * thiz,
        itk_uint8_t *buffer, size_t bufferLen);

    /**
     * Write the file system metadata.
     *
     * @param thiz The file system iterator.
     * @param metadata Pointer to the metadata buffer. The length of the metadata
     * is same as provided in itk_nvs_t::newNvsFs().
     * @return ITKE_OK on success,
     * @return ITKE_FAIL if the write fails.
     */
    itk_errcode_t (* writeMetadata)(struct itk_nvs_fs_t * thiz,
        itk_uint8_t const * metadata);

    /**
     * Erases the file system pointed to by thiz and advances the
     * iterator to the next file system.
     *
     * @param thiz The file system iterator.
     * @return ITK_PRESENT If the iterator points at a valid file system after the deletion.
     * @return ITK_NOT_PRESENT Either the deletion of the file system or advancement 
	 * to the next valid file system failed.
	 *
	 * @remark If the iterator is pointing to a valid file system then this call
     * shall permanently remove the file system and all associated data
     * and advance the iterator to the next file system. 
     */
    itk_present_t (* erase)(struct itk_nvs_fs_t * thiz);

    /**
     * Checks whether the user has enabled True Persistent Storage.
     *
     * Receivers may implement a mechanism to allow users to disable True Persistent Storage.
     * This function queries the availability of this storage type (file system).
     *
     * @param thiz The file system iterator.
     * @return ITK_PRESENT	If the file system pointed to by the iterator is enabled.
     * @return ITK_NOT_PRESENT	If the file system pointed to by the iterator is disabled.
	 *
	 * @remark This function is only called for file systems of type ITK_NVS_FS_PST.
     */
    itk_present_t (* enabled)(struct itk_nvs_fs_t * thiz);

} itk_nvs_fs_t;

/**
 * Non-Volatile Storage interface.
 *
 * Refer to the iTK User Guide for details.
 */
typedef struct itk_nvs_t
{
    /**
     * Creates a new file system iterator for existing file systems.
     *
     * @param thiz      The registered NVS interface.
     * @param fsType    File system type.
     * @param fsName    Name of the file system.
     * @return			A file system iterator.
	 *
     * @remark If fsName is NULL then the created iterator shall point to the first
     * file system or, if no file system exists, NULL is returned.
     * If fsName is a valid name and if a file system with the specified name and type exists,
     * the iterator shall point to that file system. If the specified file system does not
     * exist, the call returns NULL.
     */
    itk_nvs_fs_t* (*getNvsFs)(struct itk_nvs_t * thiz, itk_nvs_fs_type_t fsType, const char *fsName);

    /**
     * Creates a new file system and file system iterator.
     *
     * @param thiz The registered NVS interface.
     * @param fsType Informative parameter to indicate the usage of the file system.
     * @param fsName The name of the file system.
     * @param minimumCapacity The minimum storage space needed for this file system.
     * @param priority The relative priority of the file system, 0 being lowest and 255 highest.
     * @param metadata The metadata to associate with the file system. metadata can be NULL.
     * @param metadataSize The metadata size in bytes. Will be 0 if metadata is NULL.
     * @return A file system iterator.
     * @return NULL if unable to create file system.
     *
     * @remark If a file system with identical name and type does not exist then this
     * function shall create a new file system and return a file system iterator that points
     * to it. If the file system already exists or the host is unable to create the file system
     * for any reason then it shall return NULL. If the storage device cannot provide the
     * minimumCapacity it shall remove file systems of the same type that have a lower priority.
     * Together, type and name uniquely identify a file system.
     * File system names are unique within each file system type but can be duplicated 
	 * in other types. Implementations can use the file system type and name to 
	 * partition various file systems on a single storage device.
     */
    itk_nvs_fs_t* (*newNvsFs)(struct itk_nvs_t * thiz, itk_nvs_fs_type_t fsType,
                const char *fsName, size_t minimumCapacity, itk_uint8_t priority,
                itk_uint8_t const * metadata, size_t metadataSize);


} itk_nvs_t;

#endif /* ITK_NVS_FS_H_INCLUDED */
