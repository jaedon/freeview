/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef COMMONINCLUDES_H_
#define COMMONINCLUDES_H_

#include <stdint.h>
#include <stdio.h>
#include <assert.h>

/*
 * Each operation can produce a response, and each response has a response
 * code. There are different levels of response codes, and we'd like to
 * encode enough info within the code itself to be able to get a rough
 * idea of what happened just by looking at the code. For example, we'd
 * like to know the following just from looking a the code:
 *
 *      - what TEE module or application produced this result?  For example,
 *        commAgent, cryptoAgent, playreadyAgent, etc.
 *
 *      - is it a success or failure?
 *
 *      - what module within the specific TA produced this response (e.g.
 *        (key mgmt module within cryptoAgent, ECC crypto module within
 *        playready agent, etc.)
 *
 * We can encode TA information by defining result bases. We'll allocate the
 * top 8 bits of the result for the TA/module identifier. This limits us to
 * 255 TEE modules, but this seems quite reasonable. If we start off using
 * only the leftmost 4 bits (bits 0-3 starting with MSB), we'll have the
 * option of using bits 4-7 for something else later if we need to.
 *
 * #define nfTEE_GENERAL_RESULT_BASE    0xF00000
 * #define nfTEE_AGENT_RESULT_BASE      0xE00000
 * #define nfCRYPTO_AGENT_RESULT_BASE   0xD00000
 * #define nfPLAYREAD_AGENT_RESULT_BASE 0xC00000
 *
 * Then, we can further subdivide the result space of each agent, module, etc,
 * and define macros to interpret these.
 *
 */


// used for return codes from all modules
typedef uint32_t nfTeeResult_t;

/*
 * Error codes for each module start at a specific offset
 */
#define nfTEE_GENERAL_RESULT_BASE       0xF0000000
#define nfMGMT_AGENT_RESULT_BASE        0xE0000000
#define nfCRYPTO_AGENT_RESULT_BASE      0xD0000000
#define nfDRM_AGENT_RESULT_BASE         0xC0000000
#define nfSTORAGE_AGENT_RESULT_BASE     0xB0000000

#define nfMASK_RESULT_BASE              0xF0000000

// general TEE results
typedef enum nfTeeGeneralResult
{
    nfTGR_SUCCESS = nfTEE_GENERAL_RESULT_BASE,
    nfTGR_INMSG_INVALID,
    nfTGR_INMSG_TOO_SHORT,
    nfTGR_OUTMSG_INVALID,
    nfTGR_OUTMSG_TOO_SHORT,
    nfTGR_INVALID_OPERATION,
    nfTGR_OPERATION_NOT_ALLOWED,
    nfTGR_INTERNAL_ERROR,
    nfTGR_INVALID_PARAM_COUNT,
    nfTGR_INVALID_PARM0_DESCRIPTION,
    nfTGR_INVALID_PARM1_DESCRIPTION,
    nfTGR_INVALID_PARM2_DESCRIPTION,
    nfTGR_INVALID_PARM3_DESCRIPTION,
    nfTGR_INVALID_PARM0,
    nfTGR_INVALID_PARM1,
    nfTGR_INVALID_PARM2,
    nfTGR_INVALID_PARM3,


} nfTeeGeneralResult_t;

/*
 * TEE mgmt operation results
 */
typedef enum nfTeeMgmtResult
{
    nfTMR_NOT_INITIALIZED = nfMGMT_AGENT_RESULT_BASE,
    nfTMR_ALREADY_INITIALIZED,
    nfTMR_PARAM_TOO_SMALL,
    nfTMR_PARAM_SIZE_MISMATCH,
} nfTeeMgmtResult_t;

/*
 * TEE crypto agent operation results
 * For cryptoAgent, response codes start at nfCRYPTO_AGENT_RESULT_BASE
 */
typedef enum nfTeeCryptoResult
{
    nfTCR_NOT_INITIALIZED = nfCRYPTO_AGENT_RESULT_BASE,
    nfTCR_KEY_STORE_FULL,
    nfTCR_KEY_NOT_FOUND,
    nfTCR_OPERATION_NOT_ALLOWED,
    nfTCR_MALFORMED_DATA,
    nfTCR_UNSUPPORTED_KEY_ENCODING,
    nfTCR_BAD_PARAMETER,
    nfTCR_CIPHER_ERROR,
    nfTCR_OUT_OF_MEMORY,
    nfTCR_INTERNAL_ERROR,
    nfTCR_KEY_TYPE_ALG_MISMATCH,
    nfTCR_KEY_TYPE_FORMAT_MISMATCH,
    nfTCR_KEY_LENGTH_UNSUPPORTED,
    nfTCR_OPERAND_TOO_SHORT,
    nfTCR_IMPORT_ENC_KEY_FAILED,
    nfTCR_IMPORT_HMAC_KEY_FAILED,
    nfTCR_ALREADY_INITIALIZED,
    nfTCR_UNKNOWN_KEY_TYPE,
    nfTCR_UNSUPPORTED_KEY_PARAMETER,
    nfTCR_UNKOWN_CLIENTID,
    nfTCR_CRYPT_RANDOM_FAILURE,
    nfTCR_SEALED_KEY_DATA_TOO_LARGE,
    nfTCR_SEALED_KEY_VERSION_UNSUPPORTED,
    nfTCR_SEALED_KEY_VALIDATION_ERROR,
    nfTCR_BUFFER_TOO_SMALL,
    nfTCR_HMAC_VERIFY_FAILED,
    nfTCR_INVALID_DH_GENERATOR,
    nfTCR_INVALID_KEY_HANDLE,


} nfTeeCryptoResult_t;

/*
 * TEE drm operation results
 */
typedef enum nfTeeDrmResult
{
    nfTDR_NOT_INITIALIZED = nfDRM_AGENT_RESULT_BASE,
    nfTDR_ALREADY_INITIALIZED,
    nfTDR_INTERNAL_ERROR,
    nfTDR_OPERAND_TOO_SHORT,
    nfTDR_INVALID_OPERAND,
    nfTDR_SIGNATURE_INVALID,


} nfTeeDrmResult_t;

/*
 * Storage agent operation results
 */
typedef enum nfTeeStorageResult
{
    nfTSR_MANUF_SSTORE_INVALID = nfSTORAGE_AGENT_RESULT_BASE,
    nfTSR_OPERAND_TOO_SHORT,
    nfTSR_BUFFER_TOO_SHORT,
    nfTSR_NOT_FOUND,
    nfTSR_KEY_DERIV_ERROR,
    nfTSR_CIPHER_ERROR,
    nfTSR_INTERNAL_ERROR,
    nfTSR_CHUNK_SIZE_EXCEEDED,
    nfTSR_CRYPT_RANDOM_FAILURE,
    nfTSR_OPERAND_TOO_LONG,
    nfTSR_UNRECOGNIZED_VERSION,
    nfTSR_INIT_FAILED,
    nfTSR_INVALID_PARM0,
    nfTSR_INVALID_PARM1,
    nfTSR_INVALID_PARM2,


} nfTeeStorageResult_t;


//TEE general result strings
#define         nfTGR_SUCCESS_STR                   "nfTGR_SUCCESS"
#define         nfTGR_INMSG_INVALID_STR             "nfTGR_INMSG_INVALID"
#define         nfTGR_INMSG_TOO_SHORT_STR           "nfTGR_INMSG_TOO_SHORT"
#define         nfTGR_OUTMSG_INVALID_STR            "nfTGR_OUTMSG_INVALID"
#define         nfTGR_OUTMSG_TOO_SHORT_STR          "nfTGR_OUTMSG_TOO_SHORT"
#define         nfTGR_INVALID_OPERATION_STR         "nfTGR_INVALID_OPERATION"
#define         nfTGR_OPERATION_NOT_ALLOWED_STR     "nfTGR_OPERATION_NOT_ALLOWED"
#define         nfTGR_INTERNAL_ERROR_STR            "nfTGR_INTERNAL_ERROR"
#define 		nfTGR_INVALID_PARAM_COUNT_STR		"nfTGR_INVALID_PARAM_COUNT"
#define         nfTGR_INVALID_PARM0_DESCRIPTION_STR "nfTGR_INVALID_PARM0_DESCRIPTION"
#define         nfTGR_INVALID_PARM1_DESCRIPTION_STR "nfTGR_INVALID_PARM1_DESCRIPTION"
#define         nfTGR_INVALID_PARM2_DESCRIPTION_STR "nfTGR_INVALID_PARM2_DESCRIPTION"
#define         nfTGR_INVALID_PARM3_DESCRIPTION_STR "nfTGR_INVALID_PARM3_DESCRIPTION"
#define         nfTGR_INVALID_PARM0_STR             "nfTGR_INVALID_PARM0"
#define         nfTGR_INVALID_PARM1_STR             "nfTGR_INVALID_PARM1"
#define         nfTGR_INVALID_PARM2_STR             "nfTGR_INVALID_PARM2"
#define         nfTGR_INVALID_PARM3_STR             "nfTGR_INVALID_PARM3"

//TEE mgmt agent operation result strings
#define        nfTMR_NOT_INITIALIZED_STR              "nfTMR_NOT_INITIALIZED_STR"
#define        nfTMR_ALREADY_INITIALIZED_STR          "nfTMR_ALREADY_INITIALIZED_STR"
#define        nfTMR_PARAM_TOO_SMALL_STR              "nfTMR_PARAM_TOO_SMALL_STR"
#define        nfTMR_PARAM_SIZE_MISMATCH_STR          "nfTMR_PARAM_SIZE_MISMATCH_STR"

//TEE crypto agent operation result strings
#define        nfTCR_NOT_INITIALIZED_STR                 "nfTCR_NOT_INITIALIZED"
#define        nfTCR_KEY_STORE_FULL_STR                  "nfTCR_KEY_STORE_FULL"
#define        nfTCR_KEY_NOT_FOUND_STR                   "nfTCR_KEY_NOT_FOUND"
#define        nfTCR_OPERATION_NOT_ALLOWED_STR           "nfTCR_OPERATION_NOT_ALLOWED"
#define        nfTCR_MALFORMED_DATA_STR                  "nfTCR_MALFORMED_DATA"
#define        nfTCR_UNSUPPORTED_KEY_ENCODING_STR        "nfTCR_UNSUPPORTED_KEY_ENCODING"
#define        nfTCR_BAD_PARAMETER_STR                   "nfTCR_BAD_PARAMETER"
#define        nfTCR_CIPHER_ERROR_STR                    "nfTCR_CIPHER_ERROR"
#define        nfTCR_OUT_OF_MEMORY_STR                   "nfTCR_OUT_OF_MEMORY"
#define        nfTCR_INTERNAL_ERROR_STR                  "nfTCR_INTERNAL_ERROR"
#define        nfTCR_KEY_TYPE_ALG_MISMATCH_STR           "nfTCR_KEY_TYPE_ALG_MISMATCH"
#define        nfTCR_KEY_TYPE_FORMAT_MISMATCH_STR        "nfTCR_KEY_TYPE_FORMAT_MISMATCH"
#define        nfTCR_KEY_LENGTH_UNSUPPORTED_STR          "nfTCR_KEY_LENGTH_UNSUPPORTED"
#define        nfTCR_OPERAND_TOO_SHORT_STR               "nfTCR_OPERAND_TOO_SHORT"
#define        nfTCR_IMPORT_ENC_KEY_FAILED_STR           "nfTCR_IMPORT_ENC_KEY_FAILED"
#define        nfTCR_IMPORT_HMAC_KEY_FAILED_STR          "nfTCR_IMPORT_HMAC_KEY_FAILED"
#define        nfTCR_ALREADY_INITIALIZED_STR             "nfTCR_ALREADY_INITIALIZED"
#define        nfTCR_UNKNOWN_KEY_TYPE_STR                "nfTCR_UNKNOWN_KEY_TYPE"
#define        nfTCR_UNSUPPORTED_KEY_PARAMETER_STR       "nfTCR_UNSUPPORTED_KEY_PARAMETER"
#define        nfTCR_UNKOWN_CLIENTID_STR                 "nfTCR_UNKOWN_CLIENTID"
#define        nfTCR_CRYPT_RANDOM_FAILURE_STR            "nfTCR_CRYPT_RANDOM_FAILURE"
#define        nfTCR_SEALED_KEY_DATA_TOO_LARGE_STR       "nfTCR_SEALED_KEY_DATA_TOO_LARGE"
#define        nfTCR_SEALED_KEY_VERSION_UNSUPPORTED_STR  "nfTCR_SEALED_KEY_VERSION_UNSUPPORTED"
#define        nfTCR_SEALED_KEY_VALIDATION_ERROR_STR     "nfTCR_SEALED_KEY_VALIDATION_ERROR"
#define        nfTCR_BUFFER_TOO_SMALL_STR                "nfTCR_BUFFER_TOO_SMALL"
#define        nfTCR_HMAC_VERIFY_FAILED_STR              "nfTCR_HMAC_VERIFY_FAILED"
#define        nfTCR_INVALID_DH_GENERATOR_STR            "nfTCR_INVALID_DH_GENERATOR"
#define        nfTCR_INVALID_KEY_HANDLE_STR              "nfTCR_INVALID_KEY_HANDLE"

//TEE DRM agent operation result strings
#define        nfTDR_NOT_INITIALIZED_STR                "nfTDR_NOT_INITIALIZED"
#define        nfTDR_ALREADY_INITIALIZED_STR            "nfTDR_ALREADY_INITIALIZED"
#define        nfTDR_INTERNAL_ERROR_STR                 "nfTDR_INTERNAL_ERROR"
#define        nfTDR_OPERAND_TOO_SHORT_STR              "nfTDR_OPERAND_TOO_SHORT"
#define        nfTDR_INVALID_OPERAND_STR                "nfTDR_INVALID_OPERAND"
#define        nfTDR_SIGNATURE_INVALID_STR              "nfTDR_SIGNATURE_INVALID"

//TEE storage agent operation result strings
#define        nfTSR_MANUF_SSTORE_INVALID_STR       "nfTSR_MANUF_SSTORE_INVALID"
#define        nfTSR_OPERAND_TOO_SHORT_STR          "nfTSR_OPERAND_TOO_SHORT"
#define        nfTSR_BUFFER_TOO_SHORT_STR           "nfTSR_BUFFER_TOO_SHORT"
#define        nfTSR_NOT_FOUND_STR                  "nfTSR_NOT_FOUND"
#define        nfTSR_KEY_DERIV_ERROR_STR            "nfTSR_KEY_DERIV_ERROR"
#define        nfTSR_CIPHER_ERROR_STR               "nfTSR_CIPHER_ERROR"
#define        nfTSR_INTERNAL_ERROR_STR             "nfTSR_INTERNAL_ERROR"
#define        nfTSR_CHUNK_SIZE_EXCEEDED_STR        "nfTSR_CHUNK_SIZE_EXCEEDED"
#define        nfTSR_CRYPT_RANDOM_FAILURE_STR       "nfTSR_CRYPT_RANDOM_FAILURE"
#define        nfTSR_OPERAND_TOO_LONG_STR           "nfTSR_OPERAND_TOO_LONG"
#define        nfTSR_UNRECOGNIZED_VERSION_STR       "nfTSR_UNRECOGNIZED_VERSION"
#define		   nfTSR_INIT_FAILED_STR				"nfTSR_INIT_FAILED"
#define        nfTSR_INVALID_PARM0_STR				"nfTSR_INVALID_PARM0"
#define        nfTSR_INVALID_PARM1_STR				"nfTSR_INVALID_PARM1"
#define        nfTSR_INVALID_PARM2_STR				"nfTSR_INVALID_PARM2"



/*
 * @function  getTeeResultStr
 * @brief     returns a string representation of TEE general result code
 *
 * @param     nfTeeResult_t resultCode
 *
 * @return   string representation of resultCode
 */
#ifdef __cplusplus
extern "C" {
#endif

const char* getTeeResultStr (nfTeeResult_t resultCode);

#ifdef __cplusplus
}
#endif

#endif
