/**
 * @file LoaderCoreSPI_Module.h
 * Header for module methods. This file contains the prototypes for all module functions 
 * that are used by the Loader Core.
 *
 */

#ifndef LOADERCORESPI_MODULE_H__INCLUDED__
#define LOADERCORESPI_MODULE_H__INCLUDED__

#include "LoaderCore_Types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup loadercorespis Loader Core SPIs
 *  Loader Core SPIs
 *
 *  
 *  @{
 */


/** @defgroup loadercoremodulespis Loader Core Module SPIs
 *  Loader Core Module SPIs
 *
 *  Methods to read/write module.
 *  
 *  @{
 */


/** 
 *  Pre-defined Module IDs
 *
 *  Error codes that indicate the execution result. The result code will be shared by both Boot Strap and Boot Loader.
 */
typedef enum _lc_module_id
{
    /**
     * 0x0000
     * 
     * Module ID for Unique Key.
     * This module ID is used to indicate unique key.
     */
    MODULE_DRV_ID_UK = 0x0000,

    /**
     * 0x0001
     * 
     * Module ID for Loader Core Software.
     * This module ID is used to indicate loader software, including Boot Strap and Boot Loader.
     */
    MODULE_DRV_ID_LOADER_CORE = 0x0001,

    /**
     * 0x0002
     * 
     * Module ID for Boot Block Control Block.
     * This module ID is used to indicate Boot Block Control Block whose format is defined in the loader spec.
     */
    MODULE_DRV_ID_BBCB = 0x0002,

    /**
     * 0x0003
     * 
     * Module ID for Loader Partition.
     * This module ID is used to indicate Loader Partition which mainly contains download parameters.
     */
    MODULE_DRV_ID_LOADER_PT = 0x0003,

    /**
     * 0x0004
     * 
     * Module ID for variant.
     * This module ID is used to indicate variant module which mainly contains variant parameters.
     */
    MODULE_DRV_ID_VARIANT = 0x0004

} lc_module_id;


/** 
 * Retrieve a list containing all the modules of high level application already owned by this device .
 * 
 * The SPI gets all module IDs of high level application owned by this device. The module list 
 * has a variable number of modules and must be well managed, being updated immediately at any changes 
 * (i.e., at receiving a new module or after deleting a useless module).
 * The SPI must allocate the memory to hold all module IDs and set the module ID count correctly. 
 * Loader Core will free the memory after using it.
 *
 * @param ppModuleIdList    [OUT] Pointer to an lc_uint16 array of module ids
 * @param pModuleIdCount    [OUT] Total number of modules in the module list
 * 
 * @retval
 *     ::LC_SUCCESS
 * @retval
 *     ::LC_ERROR_MODULE_NOT_FOUND
 * @retval
 *     ::LC_ERROR_MODULE_NOT_SUPPORTED
 * @retval
 *     ::LC_ERROR_MODULE_ACCESS_ERROR
 * @retval
 *     ::LC_ERROR_ILLEGAL_MODULE_CONTENT
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
lc_result LoaderCoreSPI_Module_GetModuleIdList(lc_uint16 **ppModuleIdList, lc_uint32 *pModuleIdCount);

/** 
 * Retrieve a module's header.
 *
 * This SPI reads in the module header. Loader Core allocates the memory for pModuleHeader. SPI driver 
 * shall allocate the buffer pModuleHeader->bytes and set pModuleHeader->length correctly to indicate 
 * the length of the whole module header
 * Loader Core will free the memory after using it.
 * 
 * @param moduleId      [IN] ID of module to read.
 * @param pModuleHeader [IN, OUT] pData Buffer to read module header data into. 
 *     On input, the lc_buffer_st::bytes member points to a buffer in memory, and the lc_buffer_st::length member 
 *     to the length of the buffer, in bytes. The function will read lc_buffer_st::length bytes from the resource
 *     and store them in lc_buffer_st::bytes.
 * 
 * @retval
 *     ::LC_SUCCESS
 * @retval
 *     ::LC_ERROR_MODULE_NOT_FOUND
 * @retval
 *     ::LC_ERROR_MODULE_NOT_SUPPORTED
 * @retval
 *     ::LC_ERROR_MODULE_ACCESS_ERROR
 * @retval
 *     ::LC_ERROR_ILLEGAL_MODULE_CONTENT
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
lc_result LoaderCoreSPI_Module_ReadModuleHeader(lc_uint16 moduleId, lc_buffer_st *pModuleHeader);

/** 
 * Retrieve a module's data, without the header.
 *
 * It is up to the user to free the memory used by the data.
 * 
 * @param moduleId       [IN] ID of module to read.
 * @param pModulePayload [IN, OUT] pData Buffer to read module payload data into. 
 *     On input, the lc_buffer_st::bytes member points to a buffer in memory, and the lc_buffer_st::length member 
 *     to the length of the buffer, in bytes. The function will read lc_buffer_st::length bytes from the resource
 *     and store them in lc_buffer_st::bytes.
 * 
 * @retval
 *     ::LC_SUCCESS
 * @retval
 *     ::LC_ERROR_MODULE_NOT_FOUND
 * @retval
 *     ::LC_ERROR_MODULE_NOT_SUPPORTED
 * @retval
 *     ::LC_ERROR_MODULE_ACCESS_ERROR
 * @retval
 *     ::LC_ERROR_ILLEGAL_MODULE_CONTENT
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
lc_result LoaderCoreSPI_Module_ReadModulePayload(lc_uint16 moduleId, lc_buffer_st *pModulePayload);
/** 
 * Write the entire module. 
 *
 * This SPI writes the entire module to driver, including module header and module payload.
 *
 * @param moduleId  [IN] ID of module to write.
 * @param pModule   [IN] pData Buffer containing module data to write to the resource. The lc_buffer_st::bytes member 
 *     points to the start of the buffer, and the lc_buffer_st::length member contains the number of bytes in the buffer
 *     to write.
 * 
 * @retval
 *     ::LC_SUCCESS
 * @retval
 *     ::LC_ERROR_MODULE_NOT_FOUND
 * @retval
 *     ::LC_ERROR_MODULE_NOT_SUPPORTED
 * @retval
 *     ::LC_ERROR_MODULE_ACCESS_ERROR
 * @retval
 *     ::LC_ERROR_ILLEGAL_MODULE_CONTENT
 * @retval
 *     ::LC_ERROR_MODULE_UPDATE_DISALLOWED
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
lc_result LoaderCoreSPI_Module_WriteModule(lc_uint16 moduleId, lc_buffer_st *pModule);

/** 
 * Delete a module completely.
 * 
 * @param moduleId [IN] ID of module to delete.
 * 
 * @retval
 *     ::LC_SUCCESS
 * @retval
 *     ::LC_ERROR_MODULE_NOT_FOUND
 * @retval
 *     ::LC_ERROR_MODULE_NOT_SUPPORTED
 * @retval
 *     ::LC_ERROR_MODULE_ACCESS_ERROR
 * @retval
 *     ::LC_ERROR_ILLEGAL_MODULE_CONTENT
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List". 
 */
lc_result LoaderCoreSPI_Module_Delete(lc_uint16 moduleId);

/** @} */ /* End of Loader Core Module SPIs */

/** @} */ /* End of Loader Core SPIs */


#ifdef __cplusplus
}
#endif

#endif /* !LOADERCORESPI_MODULE_H__INCLUDED__ */

