/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef TEEAPICRYPTO_H_
#define TEEAPICRYPTO_H_

#include <nrd/IWebCrypto.h>
#include <nrdbase/DataBuffer.h>
#include <nrdbase/Variant.h>
#include <nrddpi/TeeApiComm.h>
#include <nrddpi/CryptoTypes.h>
#include <pthread.h>
#include <nrdbase/tr1.h>
#include <string>
#include <openssl/bn.h>

//TODO: error handling when TEE returns an error code. Should it be logged here or in WebCrypto.cpp?

/**
 * @class 	TeeApiCrypto
 * @brief 	a class with member functions that convert nrd crypto operations into messages for TEE to perform
 * 			and interpret the result sent back.
 */
namespace netflix {
class TeeApiCrypto
{
public:
    /**
     * constructor
     */
    TeeApiCrypto();

    /**
     * destructor
     */
    ~TeeApiCrypto();

    /*
     * @function  importKey
     * @brief 	  calls the corresponding functionality in TEE to import the key
     * 			  and gives the handle of imported key
     *
     * @param	  keyFormat, format of key as defined in IWebCrypto::KeyFormat
     * @param	  keyData, actual data/bits of the key
     * @param	  algType, type of algorithm as defined in IWebCrypto::Algorithm
     * @param	  extractable, is the key extractable or not
     * @param     keyUsage, usage of the key as defined in IWebCrypto::KeyUsage
     * @param 	  (out) keyHandle, the handle to refer to imported key
     * @param 	  (out) keyType, the type of key
     * @return 	  int error, 0 on success, <0 on failure
     *
     */
    netflix::NFErr importKey (IWebCrypto::KeyFormat keyFormat, const DataBuffer& keyData,
                              IWebCrypto::Algorithm algType, bool extractable,
                              unsigned int keyUsage, nfCryptoKeyHandle_t &keyHandle, IWebCrypto::KeyType &keyType);

    /*
     * @function  exportKey
     * @brief 	  calls the corresponding functionality in TEE to export the key
     *
     * @param	  keyHandle, handle of key to be exported
     * @param	  keyFormat, format of key to be exported
     * @param 	  (out) keyData, the exported keyhandle to refer to imported key
     * @return 	  int error, 0 on success, <0 on failure
     *
     */
    netflix::NFErr exportKey (nfCryptoKeyHandle_t keyHandle, IWebCrypto::KeyFormat keyFormat,
                              /*out*/ DataBuffer& keyData);

    /*
     * @function  deleteKey
     * @brief 	  calls the corresponding functionality in TEE to delete the key
     *
     * @param	  keyHandle, handle of key to be deleted
     * @return 	  int error, 0 on success, <0 on failure
     *
     */
    netflix::NFErr deleteKey (nfCryptoKeyHandle_t keyHandle);

    /*
     * @function  exportSealedKey
     * @brief 	  Kce/Kch can be "sealed" for export (e.g. to "persist" the keys)
     *
     * @param	  keyHandle, handle for key to seal/export
     * @param	  (out) keyData, sealed key  will be stored here
     * @return 	  int error, 0 on success, <0 on failure
     *
     */
    netflix::NFErr exportSealedKey (nfCryptoKeyHandle_t keyHandle, /*out*/ DataBuffer& keyData);

    /*
     * @function  importSealedKey
     * @brief 	  load a sealed key ("persisted" Kce/Kch)
     *
     * @param	  keyData, sealed key
     * @param	  (out) keyHandle, key handle is stored here
     * @param	  (out) keyType, key type is stored here
     * @param	  (out) algorithm, key algorithm is stored here
     * @param	  (out) keyUsageFlags, key usage flags are stored here
     * @param	  (out) keyBitCount, number of bits in key
     *
     * @return 	  int error, 0 on success, <0 on failure
     *
     */
    netflix::NFErr importSealedKey (DataBuffer keyData, /*out*/ nfCryptoKeyHandle_t& keyHandle,
                                        /*out*/ nfKeyType_t& keyType, /*out*/nfCryptoAlgorithm_t& algorithm,
                                        /*out*/ nfKeyUsageFlagsWord_t& 	keyUsageFlags,
                                        /*out*/ uint32_t& keyBitCount );

    /*
     * @function  getNamedKey
     * @brief 	  calls corresponding functionality in TEE to get handle of the named key
     *
     * @param	  KeyName, Name of the key
     * @param	  (out) keyHandle, handle to the named key
     * @return 	  int error, 0 on success, <0 on failure
     *
     */
    netflix::NFErr getNamedKey (std::string KeyName, nfCryptoKeyHandle_t& keyHandle);

    /*
     * @function  getKeyInfo
     * @brief 	  calls the corresponding functionality in TEE to get info about key
     *
     * @param	  keyHandle, handle of key in question
     * @param	  (out) keyType, type of key
     * @param	  (out) extractable, if key is extractable
     * @param	  (out) algo, algorithm of key
     * @param	  (out) usage, usage of key
     * @return 	  int error, 0 on success, <0 on failure
     *
     */
    netflix::NFErr getKeyInfo (nfCryptoKeyHandle_t keyHandle, IWebCrypto::KeyType &keyType,
                               bool &extractable, IWebCrypto::Algorithm &algo, unsigned int &usage);

    /*
     * @function  hmac
     * @brief 	  calls the corresponding functionality in TEE to compute HMAC on given data
     *
     * @param	  keyHandle, handle of key to use for HMAC
     * @param	  shaAlg, SHA algorithm for HMAC
     * @param	  data, data to compute hmac
     * @param 	  (out) hmac, hmac of data
     * @return 	  int error, 0 on success, <0 on failure
     *
     */
    netflix::NFErr hmac (nfCryptoKeyHandle_t keyHandle, IWebCrypto::Algorithm shaAlg,
                    const DataBuffer& data, DataBuffer& hmac);

    /*
     * @function  hmacVerify
     * @brief 	  calls the corresponding functionality in TEE to verify HMAC on given data
     *
     * @param	  keyHandle, handle of key to use for HMAC
     * @param	  shaAlg, SHA algorithm for HMAC
     * @param	  data, data to compute hmac
     * @param 	  hmac, hmac of data
     * @param     (out) verified, indicates hmac verification passed/failed
     * @return 	  int error, 0 on success, <0 on failure
     *
     */
    netflix::NFErr hmacVerify (nfCryptoKeyHandle_t keyHandle, IWebCrypto::Algorithm shaAlg,
                    const DataBuffer& data, const DataBuffer& hmac, /*out*/bool& verified);

    /*
     * @function  aescbc
     * @brief 	  calls the corresponding aescbc encrypt or decrypt functionality in TEE on given data
     *
     * @param	  keyHandle, handle of key to use for aescbc encrypt/decrypt
     * @param	  cipherOp, cipher operation to perform i.e. encrypt or decrypt
     * @param	  ivBuf, initial vector string
     * @param	  inBuf, input data (for encrypt/decrypt)
     * @param 	  (out) outBuf, (for encrypt/decrypt)
     * @return 	  int error, 0 on success, <0 on failure
     *
     */
    netflix::NFErr aescbc (nfCryptoKeyHandle_t keyHandle, IWebCrypto::CipherOp cipherOp,
                const DataBuffer& ivBuf, const DataBuffer& inBuf, DataBuffer& outBuf);

    /*
     * @function  dhKeyGen
     * @brief 	  generate Diffie-Hellman keys
     *
     * @param	  algVar: algVar The full details about the key gen algorithm, including
   *                  the prime and generator values
     * @param	  keyUsage: The allowed usages of the keys
     * @param	  (out) pubKeyHandle: public key handle
     * @param 	(out) privKeyHandle: private key handle
     * @return 	  int error, 0 on success, <0 on failure
     *
     */
#if 0
    netflix::NFErr dhKeyGen(const Variant& algVar,
               unsigned int keyUsage,
               /*out*/uint32_t& pubKeyHandle,
               /*out*/uint32_t& privKeyHandle);
#else
    netflix::NFErr dhKeyGen(DataBuffer generatorBuf,
                           DataBuffer primeBuf,
                             unsigned int keyUsage,
                             /*out*/uint32_t& pubKeyHandle,
                             /*out*/uint32_t& privKeyHandle);
#endif
    /*
     * @function  nflxDhDerive
     * @brief 	  derive Diffie-Hellman keys
     *
   * @param[in] baseKeyHandle The handle of the key that started the DH
   *   exchange, produced by a call to dhKeyGen
   * @param[in] peerPublicKeyData The raw public key received from the remote
   *   peer
   * @param[in] deriveKeyHandle The caller supplied derivation key Kd
   * @param[out] encryptionKeyHandle The handle of the derived session
   *   encryption key in the key map
   * @param[out] hmacKeyHandle The handle of the derived session hmac key in
   *   the key map
   * @param[out] wrappingKeyHandle The handle of the derived wrapping key in
   *   the key map
   * @return NFErr_OK, if no error
     */
    netflix::NFErr nflxDhDerive(uint32_t baseKeyHandle, const DataBuffer& peerPublicKeyData,
                   uint32_t derivationKeyHandle,
                   /*out*/uint32_t& encryptionKeyHandle,
                   /*out*/uint32_t& hmacKeyHandle,
                   /*out*/uint32_t& wrappingKeyHandle);

     /* @function  clearKeys
     * @brief 	   Remove all keys that are not named from the key store
     * @param[out] keyHandles A set of key handles that were removed.
    */
    netflix::NFErr clearKeys(/*out*/std::set<uint32_t> & keyHandles);

    //TODO: add all other crypto operations.

    /************ utility functions ****************/

    /*
    * @function  translateTeeError
    * @brief     translates Tee error related to crypto operations into NFErr object
    */
    static netflix::NFErr translateTeeError (nfTeeResult_t teeResult);


    /*
     * @function  convertAlgo
     * @brief 	  converts IWebCrypto::Algorithm value to nfCryptoAlgorithm_t defined in CryptoTypes
     */
    static nfCryptoAlgorithm_t convertAlgo (IWebCrypto::Algorithm algType);

    /*
     * @function  convertAlgo
     * @brief 	  converts nfCryptoAlgorithm_t to IWebCrypto::Algorithm
     */
    static IWebCrypto::Algorithm convertAlgo(nfCryptoAlgorithm_t nfAlgType);

    /*
     * @function  convertUsage
     * @brief 	  converts IWebCrypto::KeyUsage value to nfKeyUsageFlag_t defined in CryptoTypes
     */
    static nfKeyUsageFlagsWord_t convertWcUsage (unsigned int keyUsage);

    /*
     * @function  convertUsage
     * @brief 	  converts nfKeyUsageFlag_t to IWebCrypto::KeyUsage value
     */
    static unsigned int convertTeeUsage(nfKeyUsageFlagsWord_t nfKeyUsage);

    /*
     * @function  convertKeyFormat
     * @brief 	  converts IWebCrypto::KeyFormat value to nfKeyFormat_t defined in CryptoTypes
     */
    static nfKeyFormat_t convertKeyFormat(IWebCrypto::KeyFormat keyFormat);

    /*
     * @function  convertKeyType
     * @brief 	  converts nfKeyType_t value to IWebCrypto::KeyType
     */
    static IWebCrypto::KeyType convertKeyType(nfKeyType_t nfKeyType);

    /*
     * @function	convertBnToB64
     * @brief		convert BIGNUM* to base 64 encoded string
     */
    static void convertBnToB64(BIGNUM* bn, int maxBinStrLen, std::string &b64);

    /*
     * @function	convertB64ToBn
     * @brief		convert base 64 encoded string to BIGNUM*
     */
    static void convertB64ToBn(char* b64, BIGNUM* bn);

    /*
     * @function	getEsn
     * @brief		get the esn from Tee
     */
    std::string getEsn();

private:
    shared_ptr<TeeApiComm> teeApiComm;
};
}
#endif /* TEEAPICRYPTO_H_ */
