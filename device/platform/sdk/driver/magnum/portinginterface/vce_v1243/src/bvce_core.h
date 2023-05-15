/***************************************************************************
 *     Copyright (c) 2003-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvce_core.h $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 4/8/13 2:56p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vce/src/bvce_core.h $
 * 
 * Hydra_Software_Devel/4   4/8/13 2:56p nilesh
 * SW7425-4676: Removed support for VCE Core v1.1.1.0 (7435 A0)
 * 
 * Hydra_Software_Devel/3   3/28/13 1:57p nilesh
 * SW7425-4676: VCE FW v4.0.9.0
 * 
 * Hydra_Software_Devel/2   3/19/13 1:28p nilesh
 * SW7425-4615: Fixed non-secure memory size for VICE2 v2
 * 
 * Hydra_Software_Devel/1   3/6/13 5:54p nilesh
 * SW7425-4615: Merge refactor of platform vs core parameters
 * 
 * Hydra_Software_Devel/SW7425-4615/1   3/6/13 5:42p nilesh
 * SW7425-4615: Refactor platform vs core specific parameters
 *
 ***************************************************************************/

#ifndef BVCE_CORE_H_
#define BVCE_CORE_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef BVCE_P_CORE_MAJOR
#error BVCE_P_CORE_MAJOR undefined
#endif
#ifndef BVCE_P_CORE_MINOR
#error BVCE_P_CORE_MINOR undefined
#endif
#ifndef BVCE_P_CORE_SUBMINOR
#error BVCE_P_CORE_SUBMINOR undefined
#endif
#ifndef BVCE_P_CORE_REVISION
#error BVCE_P_CORE_REVISION undefined
#endif

/* Core 0.1.8.2 */
#if ( ( BVCE_P_CORE_MAJOR == 0 ) && ( BVCE_P_CORE_MINOR == 1 ) && ( BVCE_P_CORE_SUBMINOR == 8 ) && ( BVCE_P_CORE_REVISION == 2 ) )
#define BVCE_PLATFORM_P_NUM_ENCODE_CHANNELS 2
#define BVCE_PLATFORM_P_NUM_OUTPUT_CHANNELS 2
#define BVCE_PLATFORM_P_ITB_ALIGNMENT 128

#define MIN_PICTURE_BUFFER_SIZE_IN_BYTES MIN_NONSECURE_BUFFER_SIZE_IN_BYTES_FOR_VICE2_V1

#ifndef MIN_SECURE_BUFFER_SIZE_IN_BYTES
#define MIN_SECURE_BUFFER_SIZE_IN_BYTES MIN_SECURE_BUFFER_SIZE_IN_BYTES_FOR_VICE2_V1_AND_V2_0
#endif

/* Core 1.1.1.1 */
#elif ( ( BVCE_P_CORE_MAJOR == 1 ) && ( BVCE_P_CORE_MINOR == 1 ) && ( BVCE_P_CORE_SUBMINOR == 1 ) && ( BVCE_P_CORE_REVISION == 1 ) )
#define BVCE_PLATFORM_P_NUM_ENCODE_CHANNELS 2
#define BVCE_PLATFORM_P_NUM_OUTPUT_CHANNELS 2
#define BVCE_PLATFORM_P_ITB_ALIGNMENT 128

#define MIN_PICTURE_BUFFER_SIZE_IN_BYTES MIN_NONSECURE_BUFFER_SIZE_IN_BYTES_FOR_VICE2_V1

#ifndef MIN_SECURE_BUFFER_SIZE_IN_BYTES
#define MIN_SECURE_BUFFER_SIZE_IN_BYTES MIN_SECURE_BUFFER_SIZE_IN_BYTES_FOR_VICE2_V1_AND_V2_0
#endif

/* Core 2.0.0.2 */
#elif ( ( BVCE_P_CORE_MAJOR == 2 ) && ( BVCE_P_CORE_MINOR == 0 ) && ( BVCE_P_CORE_SUBMINOR == 0 ) && ( BVCE_P_CORE_REVISION == 2 ) )
#define BVCE_PLATFORM_P_NUM_ENCODE_CHANNELS 2
#define BVCE_PLATFORM_P_NUM_OUTPUT_CHANNELS 2
#define BVCE_PLATFORM_P_ITB_ALIGNMENT 160

#define MIN_PICTURE_BUFFER_SIZE_IN_BYTES MIN_NONSECURE_BUFFER_SIZE_IN_BYTES_FOR_VICE2_V2

#ifndef MIN_SECURE_BUFFER_SIZE_IN_BYTES
#define MIN_SECURE_BUFFER_SIZE_IN_BYTES MIN_SECURE_BUFFER_SIZE_IN_BYTES_FOR_VICE2_V1_AND_V2_0
#endif

#define BVCE_PLATFORM_P_SUPPORT_MULTIPLE_SLICES 1

/* Core 2.0.1.2 */
#elif ( ( BVCE_P_CORE_MAJOR == 2 ) && ( BVCE_P_CORE_MINOR == 0 ) && ( BVCE_P_CORE_SUBMINOR == 1 ) && ( BVCE_P_CORE_REVISION == 2 ) )
#define BVCE_PLATFORM_P_NUM_ENCODE_CHANNELS 2
#define BVCE_PLATFORM_P_NUM_OUTPUT_CHANNELS 2
#define BVCE_PLATFORM_P_ITB_ALIGNMENT 160

#define MIN_PICTURE_BUFFER_SIZE_IN_BYTES MIN_NONSECURE_BUFFER_SIZE_IN_BYTES_FOR_VICE2_V2

#ifndef MIN_SECURE_BUFFER_SIZE_IN_BYTES
#define MIN_SECURE_BUFFER_SIZE_IN_BYTES MIN_SECURE_BUFFER_SIZE_IN_BYTES_FOR_VICE2_V1_AND_V2_0
#endif

#define BVCE_PLATFORM_P_SUPPORT_MULTIPLE_SLICES 1

/* Core 2.0.f.2 */
#elif ( ( BVCE_P_CORE_MAJOR == 2 ) && ( BVCE_P_CORE_MINOR == 0 ) && ( BVCE_P_CORE_SUBMINOR == 15 ) && ( BVCE_P_CORE_REVISION == 2 ) )
#define BVCE_PLATFORM_P_NUM_ENCODE_CHANNELS 2
#define BVCE_PLATFORM_P_NUM_OUTPUT_CHANNELS 2
#define BVCE_PLATFORM_P_ITB_ALIGNMENT 160

#define MIN_PICTURE_BUFFER_SIZE_IN_BYTES MIN_NONSECURE_BUFFER_SIZE_IN_BYTES_FOR_VICE2_V2

#ifndef MIN_SECURE_BUFFER_SIZE_IN_BYTES
#define MIN_SECURE_BUFFER_SIZE_IN_BYTES MIN_SECURE_BUFFER_SIZE_IN_BYTES_FOR_VICE2_V1_AND_V2_0
#endif

#define BVCE_PLATFORM_P_SUPPORT_MULTIPLE_SLICES 1

/* Core 2.1.0.3 */
#elif ( ( BVCE_P_CORE_MAJOR == 2 ) && ( BVCE_P_CORE_MINOR == 1 ) && ( BVCE_P_CORE_SUBMINOR == 0 ) && ( BVCE_P_CORE_REVISION == 3 ) )
#define BVCE_PLATFORM_P_NUM_ENCODE_CHANNELS 3
#define BVCE_PLATFORM_P_NUM_OUTPUT_CHANNELS 3
#define BVCE_PLATFORM_P_ITB_ALIGNMENT 160

#define MIN_PICTURE_BUFFER_SIZE_IN_BYTES MIN_NONSECURE_BUFFER_SIZE_IN_BYTES_FOR_VICE2_V2

#ifndef MIN_SECURE_BUFFER_SIZE_IN_BYTES
#define MIN_SECURE_BUFFER_SIZE_IN_BYTES MIN_SECURE_BUFFER_SIZE_IN_BYTES_FOR_VICE2_V1_AND_V2_0
#endif

#define BVCE_PLATFORM_P_SUPPORT_MULTIPLE_SLICES 1

/* Core Unknown */
#else
#error Unrecognized core version
#endif




#ifdef __cplusplus
}
#endif

#endif /* BVCE_CORE_H_ */
/* End of File */