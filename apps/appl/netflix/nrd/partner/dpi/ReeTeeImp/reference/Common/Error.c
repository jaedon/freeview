#include "CommonIncludes.h"

const char* getTeeResultStr (nfTeeResult_t resultCode)
{
    switch (resultCode & nfMASK_RESULT_BASE)
    {
        case nfTEE_GENERAL_RESULT_BASE:
            {
                nfTeeGeneralResult_t genResultCode = (nfTeeGeneralResult_t) resultCode;
                switch (genResultCode)
                {
                    case nfTGR_SUCCESS:
                        return nfTGR_SUCCESS_STR;
                    case nfTGR_INMSG_INVALID:
                        return nfTGR_INMSG_INVALID_STR;
                    case nfTGR_INMSG_TOO_SHORT:
                        return nfTGR_INMSG_TOO_SHORT_STR;
                    case nfTGR_OUTMSG_INVALID:
                        return nfTGR_OUTMSG_INVALID_STR;
                    case nfTGR_OUTMSG_TOO_SHORT:
                        return nfTGR_OUTMSG_TOO_SHORT_STR;
                    case nfTGR_INVALID_OPERATION:
                        return nfTGR_INVALID_OPERATION_STR;
                    case nfTGR_OPERATION_NOT_ALLOWED:
                        return nfTGR_OPERATION_NOT_ALLOWED_STR;
                    case nfTGR_INTERNAL_ERROR:
                        return nfTGR_INTERNAL_ERROR_STR;
                    case nfTGR_INVALID_PARAM_COUNT:
                    	return nfTGR_INVALID_PARAM_COUNT_STR;
                    case nfTGR_INVALID_PARM0_DESCRIPTION:
                        return nfTGR_INVALID_PARM0_DESCRIPTION_STR;
                    case nfTGR_INVALID_PARM1_DESCRIPTION:
                        return nfTGR_INVALID_PARM1_DESCRIPTION_STR;
                    case nfTGR_INVALID_PARM2_DESCRIPTION:
                        return nfTGR_INVALID_PARM2_DESCRIPTION_STR;
                    case nfTGR_INVALID_PARM3_DESCRIPTION:
                        return nfTGR_INVALID_PARM3_DESCRIPTION_STR;
                    case nfTGR_INVALID_PARM0:
                        return nfTGR_INVALID_PARM0_STR;
                    case nfTGR_INVALID_PARM1:
                        return nfTGR_INVALID_PARM1_STR;
                    case nfTGR_INVALID_PARM2:
                        return nfTGR_INVALID_PARM2_STR;
                    case nfTGR_INVALID_PARM3:
                        return nfTGR_INVALID_PARM3_STR;

                }
            }
            break;
        case nfMGMT_AGENT_RESULT_BASE:
            {
                nfTeeMgmtResult_t mgmtResultCode = (nfTeeMgmtResult_t) resultCode;
                switch (mgmtResultCode)
                {
                    case nfTMR_NOT_INITIALIZED:
                        return nfTMR_NOT_INITIALIZED_STR;
                    case nfTMR_ALREADY_INITIALIZED:
                        return nfTMR_ALREADY_INITIALIZED_STR;
                    case nfTMR_PARAM_TOO_SMALL:
                        return nfTMR_PARAM_TOO_SMALL_STR;
                    case nfTMR_PARAM_SIZE_MISMATCH:
                        return nfTMR_PARAM_SIZE_MISMATCH_STR;
                }
            }
            break;
        case nfCRYPTO_AGENT_RESULT_BASE:
            {
                nfTeeCryptoResult_t cryptoResultCode = (nfTeeCryptoResult_t) resultCode;
                switch (cryptoResultCode)
                {
                    case nfTCR_NOT_INITIALIZED:
                        return nfTCR_NOT_INITIALIZED_STR;
                    case nfTCR_KEY_STORE_FULL:
                        return nfTCR_KEY_STORE_FULL_STR;
                    case nfTCR_KEY_NOT_FOUND:
                        return nfTCR_KEY_NOT_FOUND_STR;
                    case nfTCR_OPERATION_NOT_ALLOWED:
                        return nfTCR_OPERATION_NOT_ALLOWED_STR;
                    case nfTCR_MALFORMED_DATA:
                        return nfTCR_MALFORMED_DATA_STR;
                    case nfTCR_UNSUPPORTED_KEY_ENCODING:
                        return nfTCR_UNSUPPORTED_KEY_ENCODING_STR;
                    case nfTCR_BAD_PARAMETER:
                        return nfTCR_BAD_PARAMETER_STR;
                    case nfTCR_CIPHER_ERROR:
                        return nfTCR_CIPHER_ERROR_STR;
                    case nfTCR_OUT_OF_MEMORY:
                        return nfTCR_OUT_OF_MEMORY_STR;
                    case nfTCR_INTERNAL_ERROR:
                        return nfTCR_INTERNAL_ERROR_STR;
                    case nfTCR_KEY_TYPE_ALG_MISMATCH:
                        return nfTCR_KEY_TYPE_ALG_MISMATCH_STR;
                    case nfTCR_KEY_TYPE_FORMAT_MISMATCH:
                        return nfTCR_KEY_TYPE_FORMAT_MISMATCH_STR;
                    case nfTCR_KEY_LENGTH_UNSUPPORTED:
                        return nfTCR_KEY_LENGTH_UNSUPPORTED_STR;
                    case nfTCR_OPERAND_TOO_SHORT:
                        return nfTCR_OPERAND_TOO_SHORT_STR;
                    case nfTCR_IMPORT_ENC_KEY_FAILED:
                        return nfTCR_IMPORT_ENC_KEY_FAILED_STR;
                    case nfTCR_IMPORT_HMAC_KEY_FAILED:
                        return nfTCR_IMPORT_HMAC_KEY_FAILED_STR;
                    case nfTCR_ALREADY_INITIALIZED:
                        return nfTCR_ALREADY_INITIALIZED_STR;
                    case nfTCR_UNKNOWN_KEY_TYPE:
                        return nfTCR_UNKNOWN_KEY_TYPE_STR;
                    case nfTCR_UNSUPPORTED_KEY_PARAMETER:
                        return nfTCR_UNSUPPORTED_KEY_PARAMETER_STR;
                    case nfTCR_UNKOWN_CLIENTID:
                        return nfTCR_UNKOWN_CLIENTID_STR;
                    case nfTCR_CRYPT_RANDOM_FAILURE:
                        return nfTCR_CRYPT_RANDOM_FAILURE_STR;
                    case nfTCR_SEALED_KEY_DATA_TOO_LARGE:
                        return nfTCR_SEALED_KEY_DATA_TOO_LARGE_STR;
                    case nfTCR_SEALED_KEY_VERSION_UNSUPPORTED:
                        return nfTCR_SEALED_KEY_VERSION_UNSUPPORTED_STR;
                    case nfTCR_SEALED_KEY_VALIDATION_ERROR:
                        return nfTCR_SEALED_KEY_VALIDATION_ERROR_STR;
                    case nfTCR_BUFFER_TOO_SMALL:
                        return nfTCR_BUFFER_TOO_SMALL_STR;
                    case nfTCR_HMAC_VERIFY_FAILED:
                        return nfTCR_HMAC_VERIFY_FAILED_STR;
                    case nfTCR_INVALID_DH_GENERATOR:
                        return nfTCR_INVALID_DH_GENERATOR_STR;
                    case nfTCR_INVALID_KEY_HANDLE:
                        return nfTCR_INVALID_KEY_HANDLE_STR;
                }
            }
            break;
        case nfDRM_AGENT_RESULT_BASE:
            {
                nfTeeDrmResult_t drmResultCode = (nfTeeDrmResult_t) resultCode;
                switch (drmResultCode)
                {
                    case nfTDR_NOT_INITIALIZED:
                        return nfTDR_NOT_INITIALIZED_STR;
                    case nfTDR_ALREADY_INITIALIZED:
                        return nfTDR_ALREADY_INITIALIZED_STR;
                    case nfTDR_INTERNAL_ERROR:
                        return nfTDR_INTERNAL_ERROR_STR;
                    case nfTDR_OPERAND_TOO_SHORT:
                        return nfTDR_OPERAND_TOO_SHORT_STR;
                    case nfTDR_INVALID_OPERAND:
                        return nfTDR_INVALID_OPERAND_STR;
                    case nfTDR_SIGNATURE_INVALID:
                        return nfTDR_SIGNATURE_INVALID_STR;
                }
            }
            break;
        case nfSTORAGE_AGENT_RESULT_BASE:
            {
                nfTeeStorageResult_t strResultCode = (nfTeeStorageResult_t) resultCode;
                switch (strResultCode)
                {
                    case nfTSR_MANUF_SSTORE_INVALID:
                        return nfTSR_MANUF_SSTORE_INVALID_STR;
                    case nfTSR_OPERAND_TOO_SHORT:
                        return nfTSR_OPERAND_TOO_SHORT_STR;
                    case nfTSR_BUFFER_TOO_SHORT:
                        return nfTSR_BUFFER_TOO_SHORT_STR;
                    case nfTSR_NOT_FOUND:
                        return nfTSR_NOT_FOUND_STR;
                    case nfTSR_KEY_DERIV_ERROR:
                        return nfTSR_KEY_DERIV_ERROR_STR;
                    case nfTSR_CIPHER_ERROR:
                        return nfTSR_CIPHER_ERROR_STR;
                    case nfTSR_INTERNAL_ERROR:
                        return nfTSR_INTERNAL_ERROR_STR;
                    case nfTSR_CHUNK_SIZE_EXCEEDED:
                        return nfTSR_CHUNK_SIZE_EXCEEDED_STR;
                    case nfTSR_CRYPT_RANDOM_FAILURE:
                        return nfTSR_CRYPT_RANDOM_FAILURE_STR;
                    case nfTSR_OPERAND_TOO_LONG:
                        return nfTSR_OPERAND_TOO_LONG_STR;
                    case nfTSR_UNRECOGNIZED_VERSION:
                        return nfTSR_UNRECOGNIZED_VERSION_STR;
                    case nfTSR_INIT_FAILED:
                    	return nfTSR_INIT_FAILED_STR;
                    case nfTSR_INVALID_PARM0:
                    	return nfTSR_INVALID_PARM0_STR;
                    case nfTSR_INVALID_PARM1:
                    	return nfTSR_INVALID_PARM0_STR;
                    case nfTSR_INVALID_PARM2:
                    	return nfTSR_INVALID_PARM0_STR;
                }
            }
            break;
    }
    return "";
}
