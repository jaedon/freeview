
#include "LoaderCoreSPI_Device.h"
#include "LoaderCorePrivate.h"
#include "irv5_dev.h"

//#include "cas_ir_util_v5.h"

#ifdef __cplusplus
extern "C" {
#endif

//static lc_sint32 s_HxSelectedAlgorithm = LC_CHECKSUM_ALGORITHM_SHA256;

/** @defgroup loadercoredevicespis Loader Core Device SPIs
 *  Loader Core Device SPIs
 *
 *  Chipset-specific or device-specific methods, mainly about algorithms that are supported by the device.
 *  Device SPIs are mainly used for checksum calculation. Hardware is used for checksum calculation in order to improve performance (reduce calculation time).
 *  Loader may adopt several algorithms, but a device may choose one of the algorithms that will be used.
 *
 *  @{
 */

/**
 * Get the algorithm chosen by the manufacturer.
 *
 * The SPI is to get the algorithm chosen by the manufacturer. This algorithm will later be used for checksum calculation.
 * The SPI driver must set the selected algorithm to * pSelectedAlgorithm.
 * \note The algorithm is used for checksum calculation. It has nothing to do with descrambling key ladder.
 *
 * @param[out] pSelectedAlgorithm To save the checksum algorithm chosen by the manufacturer
 *
 * @retval
 *     ::LC_SUCCESS
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
lc_result LoaderCoreSPI_Device_ChooseAlgorithm(lc_checksum_algorithm *pSelectedAlgorithm)
{
	LC_UNUSED(pSelectedAlgorithm);
#if 1
	dbgerr0("########## NOT SUPPORT HW HASH #########\n");
	return LC_FAILURE;
#else
	dev_going;
	*pSelectedAlgorithm = s_HxSelectedAlgorithm;
	dbgwarn1(" device hash algo. [%x]\n", *pSelectedAlgorithm );

	return LC_SUCCESS;
#endif
}


/**
 * Get the algorithm chosen by the manufacturer.
 *
 * This SPI will require the device to return the checksum to Loader Core.
 * ::lc_checksum_info must at least contain checksum algorithm and necessary control fields if needed.
 * \note The algorithm is used for checksum calculation. It has nothing to do with descrambling key ladder.
 *
 * @param[in] pChecksumInfo Pointer to structure that describes necessary checksum information, at least
 *     the selected checksum algorithm.
 * @param[in] pRawData Pointer to buffer containing raw data for calculating checksum. The lc_buffer_st::bytes member
 *     points to the start of the buffer, and the lc_buffer_st::length member contains the number of bytes in the buffer
 *     to write.
 * @param[in,out] pChecksum To save the checksum result. The lc_buffer_st::bytes member points to the start of the buffer,
 *     whose memory should be allocated by Loader Core, and the lc_buffer_st::length member contains the actual number of
 *     bytes in the buffer.
 *
 * @retval
 *     ::LC_SUCCESS
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
lc_result LoaderCoreSPI_Device_GetChecksum(lc_checksum_info *pChecksumInfo, lc_buffer_st * pRawData, lc_buffer_st * pChecksum)
{
		LC_UNUSED(pChecksumInfo);
				LC_UNUSED(pRawData);
						LC_UNUSED(pChecksum);
#if 1
	dbgerr0("########## NOT SUPPORT CHECKSUM #########\n");
	dbgassert(0);
	return LC_FAILURE;
#else
	dbgerr0("return FAILUER\n");
	return LC_FAILURE;
#endif
}


/** @} */ /* End of Loader Core Device SPIs */

/** @} */ /* End of Loader Core SPIs */


#ifdef __cplusplus
}
#endif



