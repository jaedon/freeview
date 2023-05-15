/*
 *  Copyright 2014 Netflix, Inc.
 *
 *     Licensed under the Apache License, Version 2.0 (the "License");
 *     you may not use this file except in compliance with the License.
 *     You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0
 *
 *     Unless required by applicable law or agreed to in writing, software
 *     distributed under the License is distributed on an "AS IS" BASIS,
 *     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *     See the License for the specific language governing permissions and
 *     limitations under the License.
 *
 */

#include "WebCrypto.h"

#include <iosfwd>
#include <set>
#include <iomanip>

#include <nrdbase/Base64.h>
#include <nrdbase/Log.h>
#include <nrdbase/OpenSSLLib.h>
#include <nrdbase/Random.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/StringUtils.h>
#include <nrdbase/JSONFormatter.h>

#include <nrdbase/DigestAlgo.h>
#include <nrdbase/Digester.h>
#include <nrdbase/HMAC.h>

#include <nrd/NrdApplication.h>

#include "AesCbcCipher.h"
#include "AesGcmCipher.h"
#include "DiffieHellmanContext.h"
#include "RsaContext.h"
#include "AesKeyWrapper.h"

using namespace netflix;
using namespace netflix::device;

/**
 * These globals are used as a backdoor by which we can set the
 * pre-shared or model group keys.
 */
extern DataBuffer ncf_kpe;
extern DataBuffer ncf_kph;
extern DataBuffer ncf_kav;

// Utility functions, mostly for debug

#if 0
static inline std::string toString(const std::vector<IWebCrypto::KeyUsage>& kusage)
{
    std::string output = "[ ";
    std::vector<IWebCrypto::KeyUsage>::const_iterator it;
    for (it = kusage.begin(); it != kusage.end(); ++it)
    {
        switch (*it)
        {
        default:
        case IWebCrypto::ENCRYPT: output += "encrypt "; break;
        case IWebCrypto::DECRYPT: output += "decrypt "; break;
        case IWebCrypto::SIGN:    output += "sign ";    break;
        case IWebCrypto::VERIFY:  output += "verify ";  break;
        case IWebCrypto::DERIVE:  output += "derive ";  break;
        }
    }
    output += "]";
    return output;
}
#endif

RsaContext::ShaAlgo WebCrypto::xShaAlgo(WebCrypto::Algorithm shaAlgo)
{
    switch (shaAlgo)
    {
    case WebCrypto::SHA1:   return RsaContext::SHA1;
    case WebCrypto::SHA224: return RsaContext::SHA224;
    case WebCrypto::SHA256: return RsaContext::SHA256;
    case WebCrypto::SHA384: return RsaContext::SHA384;
    case WebCrypto::SHA512: return RsaContext::SHA512;
    default:
        assert(false);
        break;
    }
    return RsaContext::SHA256;  // to keep compiler happy
}

bool WebCrypto::isAlgorithmRsa(WebCrypto::Algorithm algorithm)
{
    return (algorithm == WebCrypto::RSAES_PKCS1_V1_5  ||
            algorithm == WebCrypto::RSASSA_PKCS1_V1_5 ||
            algorithm == WebCrypto::RSA_OAEP);
}

bool WebCrypto::isAlgorithmAes(WebCrypto::Algorithm algorithm)
{
    return (algorithm == WebCrypto::AES_CBC ||
            algorithm == WebCrypto::AES_GCM ||
            algorithm == WebCrypto::AES_CTR ||
            algorithm == WebCrypto::AES_KW);
}

bool WebCrypto::isAlgorithmHmac(WebCrypto::Algorithm algorithm)
{
    return (algorithm == WebCrypto::HMAC);
}

bool WebCrypto::isAlgorithmSha(WebCrypto::Algorithm algorithm)
{
    return (algorithm == WebCrypto::SHA1   ||
            algorithm == WebCrypto::SHA224 ||
            algorithm == WebCrypto::SHA256 ||
            algorithm == WebCrypto::SHA384 ||
            algorithm == WebCrypto::SHA512);
}

bool WebCrypto::isAlgorithmDh(WebCrypto::Algorithm algorithm)
{
    return (algorithm == WebCrypto::DH ||
        algorithm == WebCrypto::NFLX_DH);
}

WebCrypto::Algorithm WebCrypto::toAlgorithm(const std::string& algorithmStr)
{
    if (strCaseCmp(algorithmStr, "HMAC") == 0)
        return WebCrypto::HMAC;
    else if (strCaseCmp(algorithmStr, "AES-CBC") == 0)
        return WebCrypto::AES_CBC;
    else if (strCaseCmp(algorithmStr, "AES-GCM") == 0)
        return WebCrypto::AES_GCM;
    else if (strCaseCmp(algorithmStr, "AES-CTR") == 0)
        return WebCrypto::AES_CTR;
    else if (strCaseCmp(algorithmStr, "RSAES-PKCS1-v1_5") == 0)
        return WebCrypto::RSAES_PKCS1_V1_5;
    else if (strCaseCmp(algorithmStr, "RSASSA-PKCS1-v1_5") == 0)
        return WebCrypto::RSASSA_PKCS1_V1_5;
    else if (strCaseCmp(algorithmStr, "RSA-OAEP") == 0)
        return WebCrypto::RSA_OAEP;
    else if (strCaseCmp(algorithmStr, "SHA-1") == 0)
        return WebCrypto::SHA1;
    else if (strCaseCmp(algorithmStr, "SHA-224") == 0)
        return WebCrypto::SHA224;
    else if (strCaseCmp(algorithmStr, "SHA-256") == 0)
        return WebCrypto::SHA256;
    else if (strCaseCmp(algorithmStr, "SHA-384") == 0)
        return WebCrypto::SHA384;
    else if (strCaseCmp(algorithmStr, "SHA-512") == 0)
        return WebCrypto::SHA512;
    else if (strCaseCmp(algorithmStr, "DH") == 0)
        return WebCrypto::DH;
    else if (strCaseCmp(algorithmStr, "NFLX-DH") == 0)
        return WebCrypto::NFLX_DH;
    else if (strCaseCmp(algorithmStr, "AES-KW") == 0)
        return WebCrypto::AES_KW;
    else if (strCaseCmp(algorithmStr, "ECC") == 0)
        return WebCrypto::ECC;
    else
        return WebCrypto::INVALID_ALGORITHM;
}

std::string WebCrypto::toString(IWebCrypto::Algorithm algorithm)
{
    switch (algorithm)
    {
    case IWebCrypto::HMAC: return "HMAC";
    case IWebCrypto::AES_CBC: return "AES-CBC";
    case IWebCrypto::AES_GCM: return "AES-GCM";
    case IWebCrypto::AES_CTR: return "AES-CTR";
    case IWebCrypto::RSAES_PKCS1_V1_5: return "RSAES-PKCS1-v1_5";
    case IWebCrypto::RSASSA_PKCS1_V1_5: return "RSASSA-PKCS1-v1_5";
    case IWebCrypto::RSA_OAEP: return "RSA-OAEP";
    case IWebCrypto::SHA1: return "SHA-1";
    case IWebCrypto::SHA224: return "SHA-224";
    case IWebCrypto::SHA256: return "SHA-256";
    case IWebCrypto::SHA384: return "SHA-384";
    case IWebCrypto::SHA512: return "SHA-512";
    case IWebCrypto::AES_KW: return "AES-KW";
    case IWebCrypto::DH: return "DH";
    case IWebCrypto::NFLX_DH: return "NFLX-DH";
    case IWebCrypto::SYSTEM: return "SYSTEM";
    case IWebCrypto::ECC: return "ECC";
    default:
    case IWebCrypto::INVALID_ALGORITHM: return "";
    }
}

IWebCrypto::Algorithm WebCrypto::getAlgorithmFromVar(const Variant &algVar)
{
    bool success = true;
    std::string algNameStr;
    if (algVar.isString())
    {
        algNameStr = algVar.string();
    }
    else if (algVar.isStringMap())
    {
        algNameStr = algVar.mapValue<std::string>("name", &success);
        if (!success)
        {
            NTRACE(TRACE_CRYPTO,
                   "getAlgorithmFromVar: unable to extract name from algorithm object");
        }
    }
    return toAlgorithm(algNameStr);
}

IWebCrypto::Algorithm WebCrypto::getHashAlgorithmFromVar(const Variant &algoVar)
{
    Variant hashVar = algoVar["params"]["hash"];
    return getAlgorithmFromVar(hashVar);
}

bool WebCrypto::reconcileAlgVsUsage(WebCrypto::Algorithm algorithm, unsigned int keyUsage)
{
    // empty usage means default usage, so always ok
    if (!keyUsage)
        return true;

    unsigned int allowed;
    switch (algorithm) {
    case IWebCrypto::HMAC:
    case IWebCrypto::RSASSA_PKCS1_V1_5:
        allowed = IWebCrypto::SIGN|IWebCrypto::VERIFY;
        break;
    case IWebCrypto::AES_CBC:
        allowed = IWebCrypto::ENCRYPT|IWebCrypto::DECRYPT|IWebCrypto::UNWRAP;
        break;
    case IWebCrypto::RSAES_PKCS1_V1_5:
    case IWebCrypto::AES_GCM:
    case IWebCrypto::AES_CTR:
        allowed = IWebCrypto::ENCRYPT|IWebCrypto::DECRYPT;
        break;
    case IWebCrypto::RSA_OAEP:
        allowed = IWebCrypto::ENCRYPT|IWebCrypto::DECRYPT|IWebCrypto::UNWRAP|IWebCrypto::WRAP;
        break;
    case IWebCrypto::AES_KW:
        allowed = IWebCrypto::WRAP|IWebCrypto::UNWRAP;
        break;
    case IWebCrypto::ECC:
        allowed = IWebCrypto::SIGN;
        break;
    default:
        return false;
    }
    return !(keyUsage & ~(allowed));
}

bool WebCrypto::isHashSpecPresent(const Variant& algVar)
{
    const WebCrypto::Algorithm hashType = getHashAlgorithmFromVar(algVar);
    if (hashType == WebCrypto::INVALID_ALGORITHM)
    {
        NTRACE(TRACE_CRYPTO, "algorithm missing inner hash spec");
        return false;
    }
    if (!isAlgorithmSha(hashType))
    {
        NTRACE(TRACE_CRYPTO, "algorithm inner hash is not SHA");
        return false;
    }
    return true;
}

int WebCrypto::extractIntFromString(const std::string& str)
{
    int number = 0;
    std::string temp;
    for (size_t i=0; i < str.size(); i++)
    {
        //iterate the string to find the first "number" character
        //if found create another loop to extract it
        //and then break the current one
        //thus extracting the FIRST encountered numeric block
        if (std::isdigit(str[i]))
        {
            for (size_t a=i; a<str.size(); a++)
                temp += str[a];
            //the first numeric block is extracted
            break;
        }
    }
    std::istringstream stream(temp);
    stream >> number;
    return number;
}

bool WebCrypto::isJweJs(const DataBuffer& input)
{
    return !input.isEmpty() && input[0] == '{';
}


//////////
//
// WebCrypto
//
//////////
WebCrypto::WebCrypto(ISystem::AuthenticationType authType)
    : mutex_(WEBCRYPTO_MUTEX, "WebCryptoMutex"),
      isInited_(false),
      authType_(authType)
{
}

WebCrypto::~WebCrypto()
{
}

#if 0
void WebCrypto::test()
{
    // Import an AES-CBC key

    const std::string kpe("77tTjHbTSCgvGbzP4KNXag==");

    Variant algorithm("AES-CBC");

    const unsigned int usage = ENCRYPT|DECRYPT;

    uint32_t key;
    IWebCrypto::KeyType key_type;

    NFErr err = importKey(IWebCrypto::RAW, kpe, algorithm, false, usage, key, key_type);

    if (!err.ok()) {
        Log::fatal() << "Failed to import AES-CBC key for test";
        return;
    }

    // Create and fill a source buffer and an IV

    const size_t source_len = 4 * 1024;
    unsigned char source[source_len];
    for (size_t i = 0; i < source_len; ++i)
        source[i] = i & 0xFF;

    const size_t iv_len = 16;
    unsigned char iv[iv_len];
    for (size_t i = 0; i < iv_len; ++i)
        iv[i] = i & 0xFF;

    const size_t iterations = 1000;

    double total;

    {
        total = 0;
        for(size_t it = 0; it < iterations; ++it) {
            Stopwatch sw;

            std::string iv64((DataBuffer::fromRawData(iv, iv_len)).toBase64String());
            std::string data64((DataBuffer::fromRawData(source, source_len)).toBase64String());
            std::string result64;
            err = aesCbc(key, iv64, data64, IWebCrypto::DOENCRYPT, result64);
            if (!err.ok()) {
                Log::fatal() << "FAILED O ENCRYPT!";
            }
            std::string result = Base64::decode(result64);
            sw.stop();
            //Log::fatal() << "ELAPSED " << sw.elapsed();
            total += sw.elapsed();
        }

        Log::fatal() << "B64 AVERAGE = " << total / iterations << " ms OVER " << iterations << " ITERATIONS";
    }

    {
        std::vector<unsigned char> key_vector = DataBuffer::fromBase64(kpe).toVector();
        std::vector<unsigned char> iv_vector = DataBuffer::fromRawData(iv, iv_len).toVector();
        std::vector<unsigned char> source_vector = DataBuffer::fromRawData(source, source_len).toVector();

        total = 0;
        for(size_t it = 0; it < iterations; ++it) {
            Stopwatch sw;

            AesCbcCipher cipher(key_vector, iv_vector);
            std::vector<unsigned char> result;
            if (!cipher.encrypt(source_vector, result)) {
                Log::fatal() << "FAILED TO ENCRYPT";
            }
            sw.stop();
            //Log::fatal() << "ELAPSED " << sw.elapsed();
            total += sw.elapsed();
        }

        Log::fatal() << "DIRECT AVERAGE = " << total / iterations << " ms OVER " << iterations << " ITERATIONS";
    }
}
#endif

//////////
//
// init
//
//////////
NFErr WebCrypto::init(/*const DataBuffer &prngSeed*/)
{
    ScopedMutex lock(mutex_);

    if (isInited_)
        return NFErr_Bad; // already initialized

    isInited_ = true; // mark ourselves as initted so we can call importKey

    restorePersistedKeys();
    importNamedKeys();

#if 0
    test();
#endif

    return NFErr_OK;
}

//////////
//
// importNamedKeys
//
//////////
void WebCrypto::importNamedKeys()
{
    ScopedMutex lock(mutex_);

    NFErr err;

    // Import device keys based on the auth type

    switch(authType_)
    {
    case ISystem::PRESHARED_KEYS:
    case ISystem::MODEL_GROUP_KEYS:
    {
        KeyType keyType;

        {
            // import encryption key
            NTRACE(TRACE_CRYPTO, "kpe size is %u", ncf_kpe.size());

            Variant kpeAlg;
            kpeAlg["name"] = "AES-CBC";

            const unsigned int kpeUsage = ENCRYPT|DECRYPT|UNWRAP;

            uint32_t ke;

            err = importKey(IWebCrypto::RAW, ncf_kpe, kpeAlg, false/*extractable*/, kpeUsage,
                            /*out*/ke, /*out*/keyType);
            if (!err.ok()) {
                Log::error(TRACE_CRYPTO) << "Failed to import device encryption key : " << err.toString();
            } else {
                addNamedKey("DKE", ke);
            }
        }

        {
            // import hmac key
            Variant kphParams;
            kphParams["hash"] = "SHA-256";
            Variant kphAlg;
            kphAlg["name"] = "HMAC";
            kphAlg["params"] = kphParams;

            const unsigned int kphUsage = SIGN|VERIFY;

            uint32_t kh;

            err = importKey(IWebCrypto::RAW, ncf_kph,
                            kphAlg, false/*extractable*/, kphUsage, /*out*/kh, /*out*/keyType);
            if (!err.ok()) {
                Log::error(TRACE_CRYPTO) << "Failed to import device signing key : " << err.toString();
            } else {
                addNamedKey("DKH", kh);
            }
        }

        {
            // /** Wrapping key derivation algorithm salt. */
            // private static final byte[] SALT = {
            //     (byte)0x02, (byte)0x76, (byte)0x17, (byte)0x98, (byte)0x4f, (byte)0x62, (byte)0x27, (byte)0x53,
            //     (byte)0x9a, (byte)0x63, (byte)0x0b, (byte)0x89, (byte)0x7c, (byte)0x01, (byte)0x7d, (byte)0x69 };
            const unsigned char salty_bytes[] = {
                0x02, 0x76, 0x17, 0x98, 0x4f, 0x62, 0x27, 0x53,
                0x9a, 0x63, 0x0b, 0x89, 0x7c, 0x01, 0x7d, 0x69
            };
            const std::vector<unsigned char> SALT(salty_bytes, salty_bytes + sizeof(salty_bytes) / sizeof(salty_bytes[0]) );
            // /** Wrapping key derivation algorithm info. */
            // private static final byte[] INFO = {
            //     (byte)0x80, (byte)0x9f, (byte)0x82, (byte)0xa7, (byte)0xad, (byte)0xdf, (byte)0x54, (byte)0x8d,
            //     (byte)0x3e, (byte)0xa9, (byte)0xdd, (byte)0x06, (byte)0x7f, (byte)0xf9, (byte)0xbb, (byte)0x91, };
            const unsigned char info_bytes[] = {
                0x80, 0x9f, 0x82, 0xa7, 0xad, 0xdf, 0x54, 0x8d,
                0x3e, 0xa9, 0xdd, 0x06, 0x7f, 0xf9, 0xbb, 0x91
            };
            const std::vector<unsigned char> INFO(info_bytes, info_bytes + sizeof(info_bytes) / sizeof(info_bytes[0]) );
            // /** Wrapping key length in bytes. */
            // private static final int WRAPPING_KEY_LENGTH = 128 / Byte.SIZE;
            const int WRAPPING_KEY_LENGTH = 128 / 8;

            // public static byte[] deriveWrappingKey(final byte[] encryptionKey, final byte[] hmacKey) throws CryptoException
            // Concatenate the keys.
            // final byte[] bits = Arrays.copyOf(encryptionKey, encryptionKey.length + hmacKey.length);
            // System.arraycopy(hmacKey, 0, bits, encryptionKey.length, hmacKey.length);
            DataBuffer bits = ncf_kpe;
            bits += ncf_kph;
            NTRACE(TRACE_CRYPTO, "kpe size is %u, kph size is %u, bits size is %d", ncf_kpe.size(), ncf_kph.size(), bits.size());

            // HMAC-SHA256 the keys with the salt as the HMAC key.
            // final HMACKey saltKey = HMACKey.create(SALT);
            // final byte[] intermediateBits = HMAC.hmac(HMACSpec.HMAC_SHA256, saltKey, bits);
            netflix::HMAC saltKey(SALT, DigestAlgo::SHA256());
            saltKey.init();
            saltKey.update(bits);
            const DataBuffer intermediateBits = saltKey.final<DataBuffer>();
            NTRACE(TRACE_CRYPTO, "intermediate size is %u", intermediateBits.size());

            // HMAC-SHA256 the info with the intermediate key as the HMAC key.
            // final HMACKey intermediateKey = HMACKey.create(intermediateBits);
            // final byte[] finalBits = HMAC.hmac(HMACSpec.HMAC_SHA256, intermediateKey, INFO);
            netflix::HMAC intermediateKey(intermediateBits, DigestAlgo::SHA256());
            intermediateKey.update(INFO);
            DataBuffer kpw = intermediateKey.final<DataBuffer>();
            NTRACE(TRACE_CRYPTO, "kpw size is %u", kpw.size());

            // Grab the first 128 bits.
            // return Arrays.copyOf(finalBits, WRAPPING_KEY_LENGTH);
            kpw.resize(WRAPPING_KEY_LENGTH);

            Variant kpwAlg;
            kpwAlg["name"] = "AES-KW";

            const unsigned int kpwUsage = UNWRAP;

            uint32_t kw;

            err = importKey(IWebCrypto::RAW, kpw, kpwAlg, false/*extractable*/, kpwUsage,
                            /*out*/kw, /*out*/keyType);
            if (!err.ok()) {
                Log::error(TRACE_CRYPTO) << "Failed to import device wrapping key : " << err.toString();
                assert(false);
            } else {
                addNamedKey("DKW", kw);
            }
        }

        break;
    }

    default:
    {
        Log::warn(TRACE_CRYPTO, "Not importing device keys");
        break;
    }
    }

    // import appboot public key

    Variant kavAlg;
    kavAlg["name"] = "RSASSA-PKCS1-v1_5";
    const unsigned int kavUsage = VERIFY;

    KeyType keyType;
    uint32_t abkp;

    err = importKey(IWebCrypto::SPKI, ncf_kav, kavAlg, true, kavUsage, /*out*/abkp, /*out*/keyType);
    if (!err.ok()) {
        Log::error(TRACE_CRYPTO) << "Failed to import appboot public key : " << err.toString();
    } else {
        addNamedKey("ABKP", abkp);
    }
}

//////////
//
// addEntropy
//
//////////
void WebCrypto::addEntropy(const DataBuffer& entropyBytesVuc)
{
    ScopedMutex lock(mutex_);

    if (!isInited_)
        return;
    NTRACE(TRACE_CRYPTO, "WebCrypto::addEntropy: adding %d bytes of entropy to OpenSSL",
           entropyBytesVuc.size());
    // assume this is pure entropy (2nd arg, # bytes entropy == # bytes of the data)
    OpenSSLLib::add_entropy(entropyBytesVuc.data(), entropyBytesVuc.size(), entropyBytesVuc.size());
}

//////////
//
// hasKey
//
//////////
bool WebCrypto::hasKey(uint32_t keyHandle) const
{
    ScopedMutex lock(mutex_);
    return keyMap_.count(keyHandle);
}

//////////
//
// isUsageAllowed
//
//////////
bool WebCrypto::isUsageAllowed(uint32_t keyHandle, KeyUsage keyUsage)
{
    assert(hasKey(keyHandle));
    const Key key = keyMap_[keyHandle];
    return !key.keyUsage || (key.keyUsage & keyUsage);   // empty keyUsage means all allowed
}

//////////
//
// isKeyAlgMatch
//
//////////
bool WebCrypto::isKeyAlgMatch(uint32_t keyHandle, Algorithm algorithm)
{
    assert(hasKey(keyHandle));
    const Algorithm keyAlgorithm = getAlgorithmFromVar(keyMap_[keyHandle].algVar);
    return keyAlgorithm == algorithm;
}

//////////
//
// digest
//
//////////
NFErr WebCrypto::digest(const Variant &algoVar, const DataBuffer& data,
                        /*out*/DataBuffer& digest)
{
    ScopedMutex lock(mutex_);
    if (!isInited_)
        return NFErr_Uninitialized;

    if (data.empty())
    {
        NTRACE(TRACE_CRYPTO, "Error: digest input is empty");
        return NFErr_BadParameter;
    }

    // determine the algorithm
    IWebCrypto::Algorithm algorithm = getAlgorithmFromVar(algoVar);

    // pick the algorithm
    shared_ptr<const DigestAlgo> algo;
    switch (algorithm)
    {
    case SHA1:   algo = DigestAlgo::SHA1();   break;
    case SHA224: algo = DigestAlgo::SHA224(); break;
    case SHA256: algo = DigestAlgo::SHA256(); break;
    case SHA384: algo = DigestAlgo::SHA384(); break;
    case SHA512: algo = DigestAlgo::SHA512(); break;
    default:
        NTRACE(TRACE_CRYPTO, "Error: digest requires SHA algorithm");
        return NFErr_BadParameter;
    }

    // compute the digest
    Digester digester(algo);
    digester.init();
    digester.update(data);
    digester.final(digest);
    return NFErr_OK;
}

//////////
//
// importKey
//
//////////
NFErr WebCrypto::importKey(KeyFormat keyFormat, const DataBuffer& key,
                           const Variant& algVar, bool extractable,
                           unsigned int keyUsage,
                           /*out*/uint32_t& keyHandle, /*out*/KeyType& keyType)
{
    ScopedMutex lock(mutex_);

    const Algorithm algType = getAlgorithmFromVar(algVar);

    keyHandle = kInvalidKeyHandle;

    if (!isInited_)
        return NFErr_Uninitialized;

    if (key.empty())
        return NFErr_BadParameter;

    // JWK format is a special case, since the internal contents of the JWK may
    // override the algorithm, extractable, etc. input parms. Shunt it off to
    // a separate handler now.
    if (keyFormat == JWK)
        return importJwk(key, algVar, extractable, keyUsage, keyHandle, keyType);

    //    const Algorithm algType = getAlgorithmFromVar(algVar);

    if (!reconcileAlgVsUsage(algType, keyUsage))
    {
        NTRACE(TRACE_CRYPTO,
               "WebCrypto::importKey: ERROR: inconsistent algorithm vs usage");
        return NFErr_BadParameter;
    }

    shared_ptr<RsaContext> pRsaContext(new RsaContext());
    switch (algType)
    {
    case HMAC:
    case AES_CBC:
    case AES_GCM:
    case AES_CTR:
    case AES_KW:
    {
        if (keyFormat != RAW)
        {
            Log::warn(TRACE_CRYPTO, "WebCrypto::importKey: raw or jwk required for "
                      "algorithm %s", toString(algType).c_str());
            return NFErr_BadParameter;
        }

        // We assume RAW key input is always a symmetric secret key
        keyType = SECRET;

        // Wes and Mark say HMAC import must always include a hash
        if (isAlgorithmHmac(algType))
        {
            // For HMAC specifically, you must provide a hash
            if (!isHashSpecPresent(algVar)) {
                return NFErr_MalformedData; // unknown algo
            }
        }
        break;
    }
    case RSASSA_PKCS1_V1_5:
    case RSA_OAEP:
    case RSAES_PKCS1_V1_5:
    {
        switch (keyFormat)
        {
        case SPKI:
        {
            // initialize the RSA context with the public SPKI-formatted key
            if (!pRsaContext->setPublicSpki(key)) {
                return NFErr_MalformedData; // cipher error
            }

            // SPKI is always public
            keyType = PUBLIC;

            // Since this is a public key, it should be forced to be
            // extractable.
            extractable = true;
            break;
        }
        case PKCS8:
        {
            // initialize the RSA context with the private PKCS#8-formatted key
            if (!pRsaContext->setPrivatePkcs8(key))
                return NFErr_MalformedData; // cipher error

            // PKCS8 is always private
            keyType = PRIVATE;
            break;
        }
        default:
        {
            NTRACE(TRACE_CRYPTO, "WebCrypto::importKey: spki, pkcs8, or jwk "
                   "required for algorithm %s", toString(algType).c_str());
            return NFErr_MalformedData; // UNSUPPORTED_KEY_ENCODING;
        }
        }
        break;
    }
    default:
    {
        Log::warn(TRACE_CRYPTO, "WebCrypto::importKey: unrecognized algorithm %d",
                  algType);
        return NFErr_BadParameter;
    }
    }

    // Finally, make a new Key object containing the extracted key and add it to
    // the key store, indexed by (output) keyHandle.
    // Note that extractable and keyUsage are copied through without processing.
    keyHandle = allocateKeyHandle();
    Key k(key, pRsaContext, keyType, extractable, algVar, keyUsage);
    keyMap_[keyHandle] = k;

    NTRACEIF(!key.empty(), TRACE_CRYPTO, "importKey: vuc=%s, handle=%d, type=%d",
             key.toHex().c_str(), keyHandle, keyType);

    return NFErr_OK;
}

//////////
//
// importJwk
//
//////////
NFErr WebCrypto::importJwk(const DataBuffer &key,
                           const Variant& algVar, bool extractable,
                           unsigned int keyUsage,
                           /*out*/uint32_t& keyHandle, /*out*/KeyType& keyType)
{
    ScopedMutex lock(mutex_);

    // NOTE: the input parms algVar, extractable, and keyUsage are used only if
    // corresponding fields in the JWK not present. They are fallbacks only.

    // Parse the JWK JSON
    // {
    //      'kty':     'RSA' or 'oct'         REQUIRED
    //      'alg:      'RSA1_5', 'A128'       OPTIONAL
    //      'use':     'sig', 'enc', 'wrap'   OPTIONAL
    //      'extractable':                    OPTIONAL
    //      <type-specific parms>             OPTIONAL
    // }
    NTRACE(TRACE_CRYPTO, "WebCrypto::importJwk: %s", key.c_str());
    const Variant jwk(Variant::fromJSON(key));
    bool isFound;
    const std::string jwkKty = jwk.mapValue<std::string>("kty", &isFound);
    if (!isFound)
    {
        Log::warn(TRACE_CRYPTO, "WebCrypto::importJwk: JWK missing required 'kty' field");
        return NFErr_BadParameter;
    }
    const std::string jwkAlg = jwk.mapValue<std::string>("alg");
    const std::string jwkUse = jwk.mapValue<std::string>("use");
    const std::string jwkExtractable = jwk.mapValue<std::string>("extractable");
    NTRACE(TRACE_CRYPTO, "\tjwkKty = %s", jwkKty.c_str());
    NTRACEIF(jwkAlg.size(), TRACE_CRYPTO, "\tjwkAlg = %s", jwkAlg.c_str());
    NTRACEIF(jwkUse.size(), TRACE_CRYPTO, "\tjwkUse = %s", jwkUse.c_str());
    NTRACEIF(jwkExtractable.size(), TRACE_CRYPTO, "\tjwkExtractable = %s",
             jwkExtractable.c_str());

    // resolve conflicts between JWK and input args

    // 'extractable' - should be the AND of the API and JWK values, if the latter exists
    bool myExtractable = jwkExtractable.empty() ?
        extractable : extractable && (jwkExtractable == "true");

    // 'alg'
    // Wes says JWK 'alg' contents, if present, should override input
    /*
      From an email from Mark. These are the JWK alg parameters to support
      | HS256        | HMAC using SHA-256 hash        | REQUIRED          |
      |              | algorithm                      |                   |
      | HS384        | HMAC using SHA-384 hash        | OPTIONAL          |
      |              | algorithm                      |                   |
      | HS512        | HMAC using SHA-512 hash        | OPTIONAL          |
      |              | algorithm                      |                   |
      | RS256        | RSASSA using SHA-256 hash      | RECOMMENDED       |
      |              | algorithm                      |                   |
      | RS384        | RSASSA using SHA-384 hash      | OPTIONAL          |
      |              | algorithm                      |                   |
      | RS512        | RSASSA using SHA-512 hash      | OPTIONAL          |
      |              | algorithm                      |                   |
      +----------------+---------------------------------+----------------+
      | RSA1_5         | RSAES-PKCS1-V1_5 [RFC3447]      | REQUIRED       |
      | RSA-OAEP       | RSAES using Optimal Asymmetric  | OPTIONAL       |
      |                | Encryption Padding (OAEP)       |                |
      |                | [RFC3447], with the default     |                |
      |                | parameters specified by RFC     |                |
      |                | 3447 in Section A.2.1           |                |
      | A128KW         | Advanced Encryption Standard    | RECOMMENDED    |
      |                | (AES) Key Wrap Algorithm        |                |
      |                | [RFC3394] using 128 bit keys    |                |
      | A256KW         | AES Key Wrap Algorithm using    | RECOMMENDED    |
      |                | 256 bit keys                    |                |
      | A128GCM       | AES in Galois/Counter Mode (GCM) | RECOMMENDED    |
      |               | [AES] [NIST.800-38D] using 128   |                |
      |               | bit keys                         |                |
      | A256GCM       | AES GCM using 256 bit keys       | RECOMMENDED    |
      +---------------+----------------------------------+----------------+

      What is missing is A128CBC (and A128CTR, though we don't need that), because
      there is no JSON Web Algorithm that uses A128CBC alone.

      I don't think there can be much doubt what the string should be (AxxxCBC) for
      xxx=128, 256, 384 etc. so we're on safe ground.
    */

    Variant myAlgVar;
    if (jwkAlg.size())
    {
        if (startsWith(jwkAlg, "HS"))
        {
            Variant hash;
            if (jwkAlg == "HS256")
                hash["name"] = "SHA-256";
            else if (jwkAlg == "HS384")
                hash["name"] = "SHA-384";
            else if (jwkAlg == "HS512")
                hash["name"] = "SHA-512";
            else
            {
                NTRACE(TRACE_CRYPTO, "WebCrypto::importJwk: invalid HMAC hash");
                return NFErr_MalformedData; // unknown algo
            }
            myAlgVar["name"] = "HMAC";
            myAlgVar["params"]["hash"] = hash;
        }
        else if (startsWith(jwkAlg, "RSA"))  // do before "RS"
        {
            if (jwkAlg == "RSA1_5")
            {
                myAlgVar["name"] = "RSAES-PKCS1-v1_5";
            }
            else if (jwkAlg == "RSA-OAEP")
            {
                myAlgVar["name"] = "RSA-OAEP";
                Variant hash;
                hash["name"] = "SHA-1";
                myAlgVar["params"]["hash"] = hash;
            }
        }
        else if (startsWith(jwkAlg, "RS"))  // do after "RSA"
        {
            Variant hash;
            if (jwkAlg == "RS256")
            {
                hash["name"] = "SHA-256";
            }
            else if (jwkAlg == "RS384")
            {
                hash["name"] = "SHA-384";
            }
            else if (jwkAlg == "RS512")
            {
                hash["name"] = "SHA-512";
            }
            else
            {
                NTRACE(TRACE_CRYPTO,
                       "WebCrypto::importJwk: invalid RSASSA-PKCS1-v1_5 hash");
                return NFErr_MalformedData; // unknown algo
            }
            myAlgVar["name"] = "RSASSA-PKCS1-v1_5";
            myAlgVar["params"]["hash"] = hash;
        }
        else if (endsWith(jwkAlg, "KW"))
        {
            if ( (jwkAlg != "A128KW") && (jwkAlg != "A256KW") )
            {
                NTRACE(TRACE_CRYPTO, "WebCrypto::importJwk: unrecognized JWK KW alg %s",
                       jwkAlg.c_str());
                return NFErr_MalformedData; // unknown algo
            }
            myAlgVar["name"] = "AES-KW";
        }
        else if (endsWith(jwkAlg, "GCM"))
        {
            myAlgVar["name"] = "AES-GCM";
            // assume the inner hash output (tag) length must equal the key size
            int taglength = 0;
            if (jwkAlg == "A128GCM")
                taglength = 128;
            else if (jwkAlg == "A256GCM")
                taglength = 256;
            else
            {
                NTRACE(TRACE_CRYPTO,
                       "WebCrypto::importJwk: unsupported AES-GCM algorithm");
                return NFErr_MalformedData; // unknown algo
            }
            myAlgVar["params"]["tagLength"] = taglength;
            const std::string jwkIvStr = jwk.mapValue<std::string>("iv");
            if (jwkIvStr.size())
                myAlgVar["params"]["iv"] = jwkIvStr;
            const std::string jwkAad = jwk.mapValue<std::string>("data");
            if (jwkAad.size())
                myAlgVar["params"]["additionalData"] = jwk.mapValue<std::string>("data");
        }
        else if (endsWith(jwkAlg, "CBC"))
        {
            myAlgVar["name"] = "AES-CBC";
            const std::string jwkIvStr = jwk.mapValue<std::string>("iv");
            if (jwkIvStr.size())
                myAlgVar["params"]["iv"] = jwkIvStr;
        }
        else
        {
            NTRACE(TRACE_CRYPTO, "WebCrypto::importJwk: JWK unrecognized algorithm");
            return NFErr_MalformedData; // unknown algo
        }
    }
    else    // no 'alg' in the JWK, use input instead
    {
        if (algVar.isNull())
        {
            NTRACE(TRACE_CRYPTO, "WebCrypto::importJwk: JDK contains no 'alg' and no "
                   "algorithm provided as fallback");
            return NFErr_MalformedData; // UNKNOWN_ALGO;
        }
        myAlgVar = algVar;
    }

    // FIXME: Wes and Mark disagree on behavior here.
    // keyUsage:
    //    Mark - should be intersection of API and JWE values, if the latter exists
    //    Wes  - If present, the JWE value should override the API value completely
    // Below I have implemented Wes's behavior since it was the most recently
    // provided and backed by his implementation.
    unsigned int myKeyUsage = 0;
    if (jwkUse.size())
    {
        if (jwkUse == "sig")
        {
            myKeyUsage |= SIGN;
            myKeyUsage |= VERIFY;
        }
        else if (jwkUse == "enc")
        {
            myKeyUsage |= ENCRYPT;
            myKeyUsage |= DECRYPT;
        }
        else if (jwkUse == "wrap")
        {
            myKeyUsage |= WRAP;
            myKeyUsage |= UNWRAP;
        }
        else
        {
            myKeyUsage = keyUsage;
        }
    }
    else
    {
        myKeyUsage = keyUsage;
    }

    // verify key usage
    if (!reconcileAlgVsUsage(getAlgorithmFromVar(myAlgVar), myKeyUsage))
    {
        NTRACE(TRACE_CRYPTO,
               "WebCrypto::importJwk: ERROR: inconsistent algorithm vs usage");
        return NFErr_BadParameter;
    }

    // extract / make the key material
    shared_ptr<RsaContext> pRsaContext;
    std::vector<unsigned char> jwkKVuc;
    if (jwkKty == "oct")    // raw octet string in 'k' field
    {
        const std::string jwkKBuf = jwk.mapValue<std::string>("k", &isFound);
        if (!isFound)
        {
            Log::warn(TRACE_CRYPTO, "WebCrypto::importJwk: missing k field");
            return NFErr_BadParameter;
        }
        NTRACE(TRACE_CRYPTO, "\tjwkK = %s", jwkKBuf.c_str());
        const std::vector<unsigned char> jwkKVuc64(jwkKBuf.begin(), jwkKBuf.end());
        jwkKVuc = Base64::decode(jwkKVuc64); // already detects url-safe chars

        const uint32_t actualKeySizeBits = jwkKVuc.size() * 8;
        NTRACE(TRACE_CRYPTO, "\tkeyLenghBits: %d", actualKeySizeBits);

        // JWK encodes the key length in bits in the algorithm name, like
        // 'A128CBC'. Validate the length of the key in 'k' against this.
        const uint32_t algKeySizeBits = extractIntFromString(jwkAlg);
        if (algKeySizeBits != actualKeySizeBits)
        {
            NTRACE(TRACE_CRYPTO,
                   "WebCrypto::importJwk: ERROR - raw key length from 'oct' "
                   "field inconsistent with JWK algorithm name in 'alg' field");
            return NFErr_MalformedData; // bad encoding
        }

        // We assume RAW key input is always a symmetric secret key
        keyType = SECRET;
    }
    else if (jwkKty == "RSA")   // 'n', 'e', and possibly 'd' fields provided
    {
        // keyData is base64-encoded JSON. It must always include the 'n'
        // (public modulus) and 'e' (public exponent) fields. If only these
        // fields are provided, we assume we want to create a public key. If in
        // addition the 'd' (private modulus) field is provided, we assume are
        // creating a private key.
        // FIXME: Officially there are a ton more fields for a JWK containing an
        // RSA private key!
        const DataBuffer pubMod = jwk.mapValue<DataBuffer>("n").decode(DataBuffer::Encoding_Base64);
        const DataBuffer pubExp = jwk.mapValue<DataBuffer>("e").decode(DataBuffer::Encoding_Base64);

        if (pubMod.isEmpty() || pubExp.isEmpty())
        {
            Log::warn(TRACE_CRYPTO,
                      "WebCrypto::importKey: JKW RSA missing required fields");
            return NFErr_MalformedData;
        }
        const DataBuffer privMod = jwk.mapValue<DataBuffer>("d").decode(DataBuffer::Encoding_Base64);

        NTRACE(TRACE_CRYPTO, "WebCrypto::importKey: n = %s", pubMod.toBase64().c_str());
        NTRACE(TRACE_CRYPTO, "WebCrypto::importKey: e = %s", pubExp.toBase64().c_str());
        NTRACEIF(!privMod.isEmpty(), TRACE_CRYPTO,
                 "WebCrypto::importKey: d = %s", privMod.toBase64().c_str());

        // initialize the RSA context with this raw key
        pRsaContext.reset(new RsaContext());
        if (!pRsaContext->setRaw(pubMod, pubExp, privMod)) {
            return NFErr_MalformedData; // cipher error
        }

        // We assume here we are making a public key if we were NOT provided a
        // private modulus, and conversely if we WERE given a private modulus
        // then we are are making a private key.
        // Also, any public key must be forced to extractable regardless of the
        // jwk or input parameter values.
        if (privMod.isEmpty())
        {
            keyType = PUBLIC;
            myExtractable = true;
        }
        else
        {
            keyType = PRIVATE;
        }

        NTRACE(TRACE_CRYPTO, "WebCrypto::importKey: JWK");
        NTRACE(TRACE_CRYPTO, "WebCrypto::importKey: %s",
               ((keyType == PUBLIC) ? "PUBLIC" : "PRIVATE"));
    }
    else
    {
        Log::warn(TRACE_CRYPTO, "WebCrypto::importJwk: invalid JWK kty field value %s",
                  jwkKty.c_str());
        return NFErr_BadParameter;
    }

    // Finally, make the actual Key object
    keyHandle = allocateKeyHandle();
    Key k(jwkKVuc, pRsaContext, keyType, myExtractable, myAlgVar, myKeyUsage);
    keyMap_[keyHandle] = k;

    return NFErr_OK;
}

//////////
//
// exportKey
//
//////////
NFErr WebCrypto::exportKey(uint32_t keyHandle, KeyFormat format,
                           /*out*/DataBuffer& keyBuf)
{
    ScopedMutex lock(mutex_);

    if (!isInited_)
        return NFErr_Uninitialized;

    if (!hasKey(keyHandle))
        return NFErr_BadParameter;

    // the key must be extractable
    Key key = keyMap_[keyHandle];
    if (!key.extractable)
    {
        NTRACE(TRACE_CRYPTO, "WebCrypto::exportKey: key not extractable");
        return NFErr_NotAllowed;
    }

    // shunt to special handling for JWK export
    if (format == JWK)
        return exportJwk(key, keyBuf);

    // Besides JWK, the other allowed export formats are:
    // RSA*:            spki (only) for public key, pkcs8 (only) for private key
    // AES* and HMAC:   raw
    const Algorithm algType = getAlgorithmFromVar(key.algVar);
    if (isAlgorithmRsa(algType))
    {
        bool success;
        if (format == SPKI)
        {
            success = key.pRsaContext->getPublicSpki(keyBuf);
        }
        else if (format == PKCS8)
        {
            success = key.pRsaContext->getPrivatePkcs8(keyBuf);
        }
        else
        {
            NTRACE(TRACE_CRYPTO,
                   "WebCrypto::exportKey: Invalid format for RSA key export");
            return NFErr_MalformedData; // cipher error
        }
        if (!success)
        {
            NTRACE(TRACE_CRYPTO, "WebCrypto::exportKey: could not extract key");
            return NFErr_MalformedData; // cipher error
        }
    }
    else if (isAlgorithmAes(algType) || isAlgorithmHmac(algType) ||
             isAlgorithmDh(algType))
    {
        if (format != RAW)
        {
            NTRACE(TRACE_CRYPTO, "WebCrypto::exportKey: AES and HMAC algorithms require "
                   "RAW or JWK export");
            return NFErr_MalformedData;
        }
        keyBuf = key.key;
    }
    else
    {
        NTRACE(TRACE_CRYPTO, "WebCrypto::exportKey: unknown algorithm");
        return NFErr_MalformedData;
    }

    return NFErr_OK;
}

//////////
//
// exportJwk
//
//////////
NFErr WebCrypto::exportJwk(const Key& key, /*out*/DataBuffer& jwkJson)
{
    ScopedMutex lock(mutex_);

    // JWK has the following JSON structure
    // {
    //      'kty':          key type, e.g. 'RSA', 'EC', 'oct' (REQUIRED)
    //      'use':          key usage, e.g. 'sig', 'enc', 'wrap' (OPTIONAL)
    //      'alg:           key algorithm, e.g. 'RSA1_5, 'A128CBC', 'A128GCM', 'HS256',
    //                                          'A128KW' (OPTIONAL)
    //      'kid':          key ID, e.g. ignore this (OPTIONAL)
    //      'extractable':  true or false (OPTIONAL)
    //      <kty-dependent parms>
    // }

    Variant jwkMap;
    // Note: we only support JWK export of symmetric and RSA public keys

    const Variant& algVar = key.algVar;
    const Algorithm algType = getAlgorithmFromVar(algVar);

    // ---- 'kty' and kty-dependent parms
    if ( isAlgorithmAes(algType) || isAlgorithmHmac(algType) )
    {
        jwkMap["kty"] = "oct";
        // For a symmetric key, the only extra field is 'k', containing the
        // raw key.
        jwkMap["k"] = key.key.encode(DataBuffer::Encoding_Base64_UrlSafe);
    }
    else if (isAlgorithmRsa(algType))
    {
        jwkMap["kty"] = "RSA";
        if (key.type != PUBLIC)
        {
            NTRACE(TRACE_CRYPTO,
                   "WebCrypto::exportJwk: RSA private key export not implemented");
            return NFErr_NotImplemented;
        }
        if (!key.pRsaContext->hasPublicKey())
        {
            NTRACE(TRACE_CRYPTO, "WebCrypto::exportJwk: No RSA public key available");
            return NFErr_Internal;
        }
        // For an RSA public key, we need the public modulus 'n' and public
        // exponent 'e' fields.
        DataBuffer publicModulus, publicExponent;
        if (!key.pRsaContext->getPublicRaw(publicModulus, publicExponent))
        {
            NTRACE(TRACE_CRYPTO, "WebCrypto::exportJwk: Error retrieving RSA public key");
            return NFErr_Internal;
        }

        jwkMap["n"] = publicModulus.encode(DataBuffer::Encoding_Base64_UrlSafe);
        jwkMap["e"] = publicExponent.encode(DataBuffer::Encoding_Base64_UrlSafe);
    }
    else
    {
        NTRACE(TRACE_CRYPTO,
               "WebCrypto::exportJwk: JWK export unsupported key algorithm");
        return NFErr_Internal;
    }

    // ---- 'use'
    // assume the the key.keyUsage vector is self-consistent and consistent with
    // key.algorithm, since this was checked when the key was created
    // Note there is an incomplete translation from WebCrypto use to JWK use.
    struct {
        const char *name;
        const unsigned int value;
    } const groups[] = {
        { "enc", ENCRYPT|DECRYPT },
        { "sig", SIGN|VERIFY },
        { "wrap", WRAP|UNWRAP },
        { "", DERIVE },
        { 0, 0 }
    };
    const char *name = 0;
    for (int i=0; groups[i].name; ++i) {
        if (key.keyUsage & groups[i].value) {
            if (name) {
                name = 0;
                break;
            }
            name = groups[i].name;
        }
    }
    if (name && *name)
        jwkMap["use"] = name;

    // ---- 'alg'
    const size_t keyLengthBits = key.key.size() * 8;
    switch (algType)
    {
    case HMAC:
    {
        switch (keyLengthBits)
        {
        case 256: jwkMap["alg"] = "HS256"; break;
        case 384: jwkMap["alg"] = "HS384"; break;
        case 512: jwkMap["alg"] = "HS512"; break;
        default:
            NTRACE(TRACE_CRYPTO, "WebCrypto::exportJwk: could not find JWK "
                   "HMAC alg for keylength = %zu", keyLengthBits);
            return NFErr_Internal;
        }
        break;
    }
    case AES_CBC:
    {
        switch (keyLengthBits)
        {
        case 128: jwkMap["alg"] = "A128CBC"; break;
        case 256: jwkMap["alg"] = "A256CBC"; break;
        case 384: jwkMap["alg"] = "A384CBC"; break;
        case 512: jwkMap["alg"] = "A512CBC"; break;
        default:
            NTRACE(TRACE_CRYPTO, "WebCrypto::exportJwk: could not find JWK "
                   "AES-CBC alg for keylength = %zu", keyLengthBits);
            return NFErr_Internal;
        }
        break;
    }
    case AES_GCM:
    {
        switch (keyLengthBits)
        {
        case 128: jwkMap["alg"] = "A128GCM"; break;
        case 256: jwkMap["alg"] = "A256GCM"; break;
        default:
            NTRACE(TRACE_CRYPTO, "WebCrypto::exportJwk: could not find JWK "
                   "AES-GCM alg for keylength = %zu", keyLengthBits);
            return NFErr_Internal;
        }
        break;
    }
    case AES_CTR:
    {
        if (keyLengthBits != 128)
        {
            NTRACE(TRACE_CRYPTO, "WebCrypto::exportJwk: could not find JWK "
                   "AES-CTR alg for keylength = %zu", keyLengthBits);
            return NFErr_Internal;
        }
        jwkMap["alg"] = "A128CTR";
        break;
    }
    case RSAES_PKCS1_V1_5:
    {
        jwkMap["alg"] = "RSA1_5";
        break;
    }
    case RSASSA_PKCS1_V1_5:
    {
        const Algorithm shaAlgType = getHashAlgorithmFromVar(key.algVar);
        assert(isAlgorithmSha(shaAlgType));
        switch (shaAlgType)
        {
        case SHA256: jwkMap["alg"] = "RS256"; break;
        case SHA384: jwkMap["alg"] = "RS384"; break;
        case SHA512: jwkMap["alg"] = "RS512"; break;
        default:
            NTRACE(TRACE_CRYPTO, "WebCrypto::exportJwk: found RSASSA_PKCS1_V1_5 "
                   "alg with invalid inner SHA %s", toString(shaAlgType).c_str());
            return NFErr_Internal;
        }
        break;
    }
    case RSA_OAEP:
    {
        jwkMap["alg"] = "RSA-OAEP";
        break;
    }
    case AES_KW:
    {
        switch (keyLengthBits)
        {
        case 128:
            jwkMap["alg"] = "A128KW";
            break;
        case 256:
            jwkMap["alg"] = "A256KW";
            break;
        default:
            NTRACE(TRACE_CRYPTO,
                   "WebCrypto::exportJwk: AES keywrap incompatible key length %zu "
                   "bits, only 128 and 256 supported", keyLengthBits);
            return NFErr_BadParameter;
        }
        break;
    }
    default:
    {
        NTRACE(TRACE_CRYPTO, "WebCrypto::exportJwk: Unable to translate exported key "
               "algorithm %s to JWK alg value", toString(algType).c_str());
        return NFErr_BadParameter;
    }
    }

    // ---- 'extractable'
    jwkMap["extractable"] = key.extractable;

    // make the JSON representation
    jwkJson = jwkMap.toJSON();
    NTRACE(TRACE_CRYPTO, "WebCrypto::exportJwk: JSON = %s", jwkJson.c_str());

    return NFErr_OK;
}

//////////
//
// getKeyInfo
//
//////////
NFErr WebCrypto::getKeyInfo(uint32_t keyHandle, /*out*/KeyType &type,
                            /*out*/bool &extractable, /*out*/Variant &algVar,
                            /*out*/unsigned int &usage) const
{
    ScopedMutex lock(mutex_);

    if (!isInited_)
        return NFErr_Uninitialized;

    if (!hasKey(keyHandle))
        return NFErr_BadParameter;

    // STL map operator[] is intolerant to const args...
    const Key key = const_cast<WebCrypto *>(this)->keyMap_[keyHandle];
    type = key.type;
    extractable = key.extractable;
    algVar = key.algVar;
    usage = key.keyUsage;

    return NFErr_OK;
}

//////////
//
// aesPre
//
//////////
NFErr WebCrypto::aesPre(uint32_t keyHandle, KeyUsage keyUsage,
                        const DataBuffer& ivInBuf,
                        Algorithm algorithm,
                        /*out*/DataBuffer &keyBuf)
{
    ScopedMutex lock(mutex_);

    if (!isInited_)
        return NFErr_Uninitialized;

    if (!hasKey(keyHandle))
        return NFErr_BadParameter;

    // verify the provided key is permitted this usage
    if (!isUsageAllowed(keyHandle, keyUsage))
    {
        NTRACE(TRACE_CRYPTO, "WebCrypto::aes: operation disallowed by keyUsage");
        return NFErr_BadParameter;
    }

    // verify the provided key is intended for this algorithm
    if (!isKeyAlgMatch(keyHandle, algorithm))
    {
        NTRACE(TRACE_CRYPTO,
               "WebCrypto::aes: operation incompatible with key algorithm");
        return NFErr_BadParameter;
    }

    if (ivInBuf.size() < AesCbcCipher::BLOCKSIZE) // same block size for all AES ciphers
    {
        NTRACE(TRACE_CRYPTO, "WebCrypto::aes: IV too short (%d), must be %d bytes or longer",
               ivInBuf.size(), AesCbcCipher::BLOCKSIZE);
        return NFErr_BadParameter;
    }

    // check the key
    keyBuf = keyMap_[keyHandle].key;
    if (keyBuf.size() != AesCbcCipher::KL128 &&  // same key sizes for all AES ciphers
        keyBuf.size() != AesCbcCipher::KL192 &&
        keyBuf.size() != AesCbcCipher::KL256)
    {
        NTRACE(TRACE_CRYPTO,
               "WebCrypto::aesCbc: incompatible key length (%d), must be 128, 192, or 256 (%d)",
               keyBuf.size(), keyHandle);
        keyBuf.clear();
        return NFErr_BadParameter;
    }

    return NFErr_OK;
}

//////////
//
// aesCbc
//
//////////
NFErr WebCrypto::aesCbc(uint32_t keyHandle, const DataBuffer& ivInBuf,
                        const DataBuffer& dataInBuf, CipherOp cipherOp,
                        /*out*/DataBuffer& dataOutBuf)
{
    ScopedMutex lock(mutex_);

    const KeyUsage keyUsage = (cipherOp == DOENCRYPT) ? ENCRYPT : DECRYPT;

    DataBuffer keyBuf;
    NFErr err = aesPre(keyHandle, keyUsage, ivInBuf, AES_CBC, keyBuf);
    if (!err.ok())
        return err;

    // do the operation
    AesCbcCipher cipher(keyBuf, ivInBuf);
    bool success;
    if (cipherOp == DOENCRYPT)
        success = cipher.encrypt(dataInBuf, dataOutBuf);
    else
        success = cipher.decrypt(dataInBuf, dataOutBuf);
    if (!success) {
        return NFErr_MalformedData;
    }

    return NFErr_OK;
}

//////////
//
// aesGcmEncrypt
//
//////////
NFErr WebCrypto::aesGcmEncrypt(uint32_t keyHandle, const DataBuffer& ivInBuf,
                               const DataBuffer& dataInBuf,
                               const DataBuffer& aadInBuf,
                               /*out*/DataBuffer& tagOutBuf,
                               /*out*/DataBuffer& dataOutBuf)
{
    ScopedMutex lock(mutex_);

    DataBuffer keyBuf;
    const NFErr err = aesPre(keyHandle, ENCRYPT, ivInBuf, AES_GCM, keyBuf);
    if (!err.ok())
        return err;

    // encrypt
    AesGcmCipher cipher(keyBuf, ivInBuf);
    const bool success = cipher.encrypt(dataInBuf, aadInBuf, dataOutBuf, tagOutBuf);
    if (!success) {
        return NFErr_MalformedData;
    }

    return NFErr_OK;
}

//////////
//
// aesGcmDecrypt
//
//////////
NFErr WebCrypto::aesGcmDecrypt(uint32_t keyHandle, const DataBuffer &ivInBuf,
                               const DataBuffer &dataInBuf,
                               const DataBuffer &aadInBuf,
                               const DataBuffer &tagInBuf,
                               /*out*/DataBuffer& dataOutBuf)
{
    ScopedMutex lock(mutex_);

    DataBuffer keyBuf;
    NFErr err = aesPre(keyHandle, DECRYPT, ivInBuf, AES_GCM, keyBuf);
    if (!err.ok())
        return err;


    // decrypt
    AesGcmCipher cipher(keyBuf, ivInBuf);
    const bool success = cipher.decrypt(dataInBuf, aadInBuf, tagInBuf, dataOutBuf);
    if (!success) {
        return NFErr_MalformedData;
    }

    return NFErr_OK;
}

//////////
//
// aesUnwrap
//
//////////
NFErr WebCrypto::aesUnwrap(const DataBuffer &dataInBuf, const Variant & algVar,
                           uint32_t wrappingKeyHandle,
                           unsigned int keyUsage,
                           /*out*/uint32_t& keyHandle)
{
    ScopedMutex lock(mutex_);

    if (!isInited_)
        return NFErr_Uninitialized;

    if (!hasKey(wrappingKeyHandle))
        return NFErr_BadParameter;

    // ---- check the wrapping key
    if (!hasKey(wrappingKeyHandle)) {
        return NFErr_MalformedData;
    }
    Key key = keyMap_[wrappingKeyHandle];
    const Algorithm wrappingKeyAlg = getAlgorithmFromVar(key.algVar);
    if (!(wrappingKeyAlg == AES_CBC || wrappingKeyAlg == AES_KW))
    {
        Log::error(TRACE_CRYPTO,
                   "WebCrypto::aesUnwrap: wrapping key algorithm is not AES-CBC or AES-KW");
        return NFErr_MalformedData;
    }

    if (!(key.keyUsage & UNWRAP))
    {
        Log::error(TRACE_CRYPTO, "WebCrypto::aesUnwrap: wrapping key cannot unwrap.");
        return NFErr_MalformedData;
    }

    const unsigned char iv[8] = { 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6 };

    AesKeyWrapper wrapper(key.key, DataBuffer::fromRawData(iv, 8));
    DataBuffer unwrappedKeyData;
    const bool success = wrapper.unwrap(dataInBuf, unwrappedKeyData);

    if (!success)
    {
        Log::error(TRACE_CRYPTO, "WebCrypto::aesUnwrap: unwrap operation failed");
        return NFErr_MalformedData;
    }

    KeyType keyType;
    return importKey(RAW, unwrappedKeyData, algVar, false, keyUsage, keyHandle, keyType);
}

//////////
//
// hmac
//
//////////
NFErr WebCrypto::hmac(uint32_t keyHandle, const Variant &algoVar,
                      const DataBuffer &dataBuf, /*out*/DataBuffer& hmacBuf)
{
    ScopedMutex lock(mutex_);

    if (!isInited_)
        return NFErr_Uninitialized;

    if (!hasKey(keyHandle))
        return NFErr_BadParameter;

    if (dataBuf.empty())
    {
        NTRACE(TRACE_CRYPTO, "WebCrypto::hmac: input is empty");
        return NFErr_BadParameter;
    }

    if (!isUsageAllowed(keyHandle, SIGN))
    {
        NTRACE(TRACE_CRYPTO, "WebCrypto::hmac: operation disallowed by keyUsage");
        return NFErr_BadParameter;
    }

    // verify the provided key is intended for this algorithm
    if (!isKeyAlgMatch(keyHandle, HMAC))
    {
        NTRACE(TRACE_CRYPTO,
               "WebCrypto::hmac: operation incompatible with key algorithm");
        return NFErr_BadParameter;
    }

    // Wes and Mark say that we must validate the hash associated with the key
    // matches that of the hmac requested
    const Algorithm shaAlgo = getHashAlgorithmFromVar(algoVar);
    const Algorithm keyShaAlgo = getHashAlgorithmFromVar(keyMap_[keyHandle].algVar);
    if (shaAlgo != keyShaAlgo)
    {
        NTRACE(TRACE_CRYPTO,
               "WebCrypto::hmac: request HMAC hash does not match key hash");
        return NFErr_BadParameter;
    }

    // determine the hash algorithm
    IWebCrypto::Algorithm algorithm = getHashAlgorithmFromVar(algoVar);

    // select the inner hash function
    shared_ptr<const DigestAlgo> digestAlgo;
    switch (algorithm)
    {
    case SHA1:   digestAlgo = DigestAlgo::SHA1();   break;
    case SHA224: digestAlgo = DigestAlgo::SHA224(); break;
    case SHA256: digestAlgo = DigestAlgo::SHA256(); break;
    case SHA384: digestAlgo = DigestAlgo::SHA384(); break;
    case SHA512: digestAlgo = DigestAlgo::SHA512(); break;
    default:
        NTRACE(TRACE_CRYPTO, "hmac: bad algorithm %d", algorithm);
        return NFErr_BadParameter;
    }

    // do the HMAC; this operation results in a base64-encoded Vuc
    netflix::HMAC hmac(keyMap_[keyHandle].key, digestAlgo);
    hmac.update(dataBuf.data(), dataBuf.size());
    hmacBuf = hmac.final<DataBuffer>();

    return NFErr_OK;
}

NFErr WebCrypto::hmacVerify(uint32_t keyHandle, const Variant &algoVar,
                      const DataBuffer &dataBuf,
                      const DataBuffer &signature,
                      /*out*/bool& verified)
{
    ScopedMutex lock(mutex_);

    if (!isInited_)
        return NFErr_Uninitialized;

    if (!hasKey(keyHandle))
        return NFErr_BadParameter;

    if (dataBuf.empty())
    {
        NTRACE(TRACE_CRYPTO, "WebCrypto::hmac: input is empty");
        return NFErr_BadParameter;
    }

    if (!isUsageAllowed(keyHandle, VERIFY))
    {
        NTRACE(TRACE_CRYPTO, "WebCrypto::hmac: operation disallowed by keyUsage");
        return NFErr_BadParameter;
    }

    // verify the provided key is intended for this algorithm
    if (!isKeyAlgMatch(keyHandle, HMAC))
    {
        NTRACE(TRACE_CRYPTO,
               "WebCrypto::hmac: operation incompatible with key algorithm");
        return NFErr_BadParameter;
    }

    // Wes and Mark say that we must validate the hash associated with the key
    // matches that of the hmac requested
    const Algorithm shaAlgo = getHashAlgorithmFromVar(algoVar);
    const Algorithm keyShaAlgo = getHashAlgorithmFromVar(keyMap_[keyHandle].algVar);
    if (shaAlgo != keyShaAlgo)
    {
        NTRACE(TRACE_CRYPTO,
               "WebCrypto::hmac: request HMAC hash does not match key hash");
        return NFErr_BadParameter;
    }

    // determine the hash algorithm
    IWebCrypto::Algorithm algorithm = getHashAlgorithmFromVar(algoVar);

    // select the inner hash function
    shared_ptr<const DigestAlgo> digestAlgo;
    switch (algorithm)
    {
    case SHA1:   digestAlgo = DigestAlgo::SHA1();   break;
    case SHA224: digestAlgo = DigestAlgo::SHA224(); break;
    case SHA256: digestAlgo = DigestAlgo::SHA256(); break;
    case SHA384: digestAlgo = DigestAlgo::SHA384(); break;
    case SHA512: digestAlgo = DigestAlgo::SHA512(); break;
    default:
        NTRACE(TRACE_CRYPTO, "hmac: bad algorithm %d", algorithm);
        return NFErr_BadParameter;
    }

    // do the HMAC; this operation results in a base64-encoded Vuc
    netflix::HMAC hmac(keyMap_[keyHandle].key, digestAlgo);
    hmac.update(dataBuf.data(), dataBuf.size());
    const DataBuffer mySignature = hmac.final<DataBuffer>();
    verified = (mySignature == signature);

    return NFErr_OK;
}

//////////
//
// rsaKeyGen
//
//////////
NFErr WebCrypto::rsaKeyGen(const Variant &algVar, bool extractable,
                           unsigned int keyUsage,
                           /*out*/uint32_t& pubKeyHandle, /*out*/uint32_t& privKeyHandle)
{
    ScopedMutex lock(mutex_);

    if (!isInited_)
        return NFErr_Uninitialized;

    const Algorithm algType = getAlgorithmFromVar(algVar);

    if (!isAlgorithmRsa(algType))
    {
        Log::warn(TRACE_CRYPTO, "WebCrypto::rsaKeyGen: not an RSA algorithm");
        return NFErr_BadParameter;
    }

    if (!reconcileAlgVsUsage(algType, keyUsage))
    {
        Log::warn(TRACE_CRYPTO, "WebCrypto::rsaKeyGen: inconsistent algorithm vs usage");
        return NFErr_NotAllowed;
    }

    // get modulusLength from algorithm object
    if (!algVar.contains("params") && !algVar["params"].contains("modulusLength"))
    {
        Log::warn(TRACE_CRYPTO,
                  "WebCrypto::rsaKeyGen: algorithm missing modulusLength param");
        return NFErr_BadParameter;
    }
    const int modLen = algVar["params"].mapValue<int>("modulusLength");
    NTRACE(TRACE_CRYPTO, "\tmodulusLength: %d", modLen);

    // get publicExponent from algorithm object
    if (!algVar["params"].contains("publicExponent"))
    {
        Log::warn(TRACE_CRYPTO,
                  "WebCrypto::rsaKeyGen: algorithm missing publicExponent param");
        return NFErr_BadParameter;
    }
    const std::string pubExpBuf =
        algVar["params"].mapValue<std::string>("publicExponent");
    NTRACE(TRACE_CRYPTO, "\tpublicExponent: %s", pubExpBuf.c_str());
    if (pubExpBuf.empty())
    {
        Log::warn(TRACE_CRYPTO,
                  "WebCrypto::rsaKeyGen: publicExponent is empty");
        return NFErr_BadParameter;
    }


    // We need an uint64_t for the public exponent arg to RsaContext.generate()
    size_t nBytes = sizeof(unsigned long);
    size_t pubExpBufSize = pubExpBuf.size();
    size_t foo = std::min(nBytes, pubExpBufSize);
    uint64_t pubExp = 0;
    for (size_t i = 0; i < foo; ++i)
        pubExp |= (pubExpBuf[i] << i*8);

    // make a new RSA context and generate keys
    shared_ptr<RsaContext> pRsaContext(new RsaContext());
    bool success = pRsaContext->generate(modLen, pubExp);
    if (!success) {
        return NFErr_MalformedData;
    }

    // Note: The input extractable value only applies to the private key. The
    // public key should always be forced to extractable = true.

    // make a Key object for the public key and record it in the keyMap
    const std::vector<unsigned char> emptyVuc;
    pubKeyHandle = allocateKeyHandle();    // pubKeyHandle is output parm
    const Key pubKey(emptyVuc, pRsaContext, PUBLIC, true, algVar, keyUsage);
    keyMap_[pubKeyHandle] = pubKey;

    // likewise for the private key
    privKeyHandle = allocateKeyHandle();   // privKeyHandle is output parm
    const Key privKey(emptyVuc, pRsaContext, PRIVATE, extractable, algVar, keyUsage);
    keyMap_[privKeyHandle] = privKey;

    NTRACE(TRACE_CRYPTO, "rsaKeyGen: publicKey %d, privateKey: %d",
           pubKeyHandle, privKeyHandle);

    return NFErr_OK;
}

//////////
//
// rsaCrypt
//
//////////
NFErr WebCrypto::rsaCrypt(uint32_t keyHandle, const DataBuffer &dataInBuf,
                          CipherOp cipherOp, /*out*/DataBuffer& dataOutBuf)
{
    ScopedMutex lock(mutex_);

    if (!isInited_)
        return NFErr_Uninitialized;

    if (!hasKey(keyHandle))
        return NFErr_BadParameter;

    // verify the provided key is permitted this usage
    const KeyUsage keyUsage = (cipherOp == DOENCRYPT) ? ENCRYPT : DECRYPT;
    if (!isUsageAllowed(keyHandle, keyUsage))
    {
        NTRACE(TRACE_CRYPTO, "WebCrypto::rsaCrypt: operation disallowed by keyUsage");
        return NFErr_BadParameter;
    }

    // verify the provided key is intended for this algorithm
    if (!isKeyAlgMatch(keyHandle, RSAES_PKCS1_V1_5))
    {
        NTRACE(TRACE_CRYPTO,
               "WebCrypto::rsaCrypt: operation incompatible with key algorithm");
        return NFErr_BadParameter;
    }

    NTRACE(TRACE_CRYPTO, "WebCrypto::rsaCrypt: inData = %s", dataInBuf.toBase64().c_str());

    // do the operation
    if (cipherOp == DOENCRYPT)
    {
        bool success = keyMap_[keyHandle].pRsaContext->publicEncrypt(dataInBuf, dataOutBuf);
        if (!success) {
            return NFErr_MalformedData;
        }
    }
    else
    {
        bool success = keyMap_[keyHandle].pRsaContext->privateDecrypt(dataInBuf, dataOutBuf);
        if (!success) {
            return NFErr_MalformedData;
        }
    }

    NTRACE(TRACE_CRYPTO, "WebCrypto::rsaCrypt: outData = %s", dataOutBuf.toBase64().c_str());

    return NFErr_OK;
}

//////////
//
// rsaSign
//
//////////
NFErr WebCrypto::rsaSign(uint32_t keyHandle, const Variant &algoVar,
                         const DataBuffer &dataBuf, /*out*/DataBuffer& sigBuf)
{
    ScopedMutex lock(mutex_);

    if (!isInited_)
        return NFErr_Uninitialized;

    if (!hasKey(keyHandle))
        return NFErr_BadParameter;

    // verify the provided key is permitted this usage
    if (!isUsageAllowed(keyHandle, SIGN))
    {
        NTRACE(TRACE_CRYPTO, "WebCrypto::rsaSign: operation disallowed by keyUsage");
        return NFErr_BadParameter;
    }

    // verify the provided key is intended for this algorithm
    if (!isKeyAlgMatch(keyHandle, RSASSA_PKCS1_V1_5))
    {
        NTRACE(TRACE_CRYPTO,
               "WebCrypto::rsaSign: operation incompatible with key algorithm");
        return NFErr_BadParameter;
    }

    // determine the algorithm
    IWebCrypto::Algorithm algorithm = getHashAlgorithmFromVar(algoVar);
    if (!isAlgorithmSha(algorithm))
    {
        NTRACE(TRACE_CRYPTO, "rsaSign: bad algorithm %d", algorithm);
        return NFErr_BadParameter;
    }

    bool success = keyMap_[keyHandle].pRsaContext->privateSign(
        dataBuf, xShaAlgo(algorithm), sigBuf);
    if (!success) {
        return NFErr_MalformedData;
    }

    return NFErr_OK;
}

//////////
//
// rsaVerify
//
//////////
NFErr WebCrypto::rsaVerify(uint32_t keyHandle, const Variant &algoVar,
                           const DataBuffer& dataBuf, const DataBuffer& sigBuf,
                           /*out*/bool& isVerified)
{
    ScopedMutex lock(mutex_);

    NTRACE(TRACE_CRYPTO, "WebCrypto::rsaVerify: keyHandle %d", keyHandle);
    if (!isInited_)
        return NFErr_Uninitialized;

    if (!hasKey(keyHandle))
        return NFErr_BadParameter;

    // verify the provided key is permitted this usage
    if (!isUsageAllowed(keyHandle, VERIFY))
    {
        NTRACE(TRACE_CRYPTO, "WebCrypto::rsaVerify: operation disallowed by keyUsage");
        return NFErr_BadParameter;
    }

    // verify the provided key is intended for this algorithm
    if (!isKeyAlgMatch(keyHandle, RSASSA_PKCS1_V1_5))
    {
        NTRACE(TRACE_CRYPTO,
               "WebCrypto::rsaVerify: operation incompatible with key algorithm");
        return NFErr_BadParameter;
    }

    // determine the algorithm
    IWebCrypto::Algorithm algorithm = getHashAlgorithmFromVar(algoVar);
    if (!isAlgorithmSha(algorithm))
    {
        NTRACE(TRACE_CRYPTO, "rsaVerify: bad algorithm %d", algorithm);
        return NFErr_BadParameter;
    }

    isVerified = keyMap_[keyHandle].pRsaContext->publicVerify(dataBuf, xShaAlgo(algorithm), sigBuf);

    return NFErr_OK;
}

//////////
//
// dhKeyGen
//
//////////
NFErr WebCrypto::dhKeyGen(const Variant& algVar, bool extractable,
                          unsigned int keyUsage,
                          /*out*/uint32_t& pubKeyHandle,
                          /*out*/uint32_t& privKeyHandle)
{
    ScopedMutex lock(mutex_);

    if (!isInited_)
        return NFErr_Uninitialized;

    const Algorithm algType = getAlgorithmFromVar(algVar);

    assert(isAlgorithmDh(algType));
    NRDP_UNUSED(algType);

    // get prime from algorithm object
    if (!algVar.contains("params") && !algVar["params"].contains("prime"))
    {
        NTRACE(TRACE_CRYPTO, "WebCrypto::dhKeyGen: algorithm params missing prime");
        return NFErr_BadParameter;
    }
    const DataBuffer primeBuf = algVar["params"].mapValue<DataBuffer>("prime");
    NTRACE(TRACE_CRYPTO, "\tprime: %s", primeBuf.toBase64().c_str());
    if (primeBuf.empty())
    {
        NTRACE(TRACE_CRYPTO, "WebCrypto::dhKeyGen: prime is empty");
        return NFErr_BadParameter;
    }

    // get generator from algorithm object
    if (!algVar.contains("params") && !algVar["params"].contains("generator"))
    {
        NTRACE(TRACE_CRYPTO, "WebCrypto::dhKeyGen: algorithm params missing generator");
        return NFErr_BadParameter;
    }
    const DataBuffer generatorBuf = algVar["params"].mapValue<DataBuffer>("generator");
    NTRACE(TRACE_CRYPTO, "\tgenerator: %s", generatorBuf.toBase64().c_str());
    if (generatorBuf.empty())
    {
        NTRACE(TRACE_CRYPTO, "WebCrypto::dhKeyGen: generator is empty");
        return NFErr_BadParameter;
    }

    // make a new DH context and generate keys
    shared_ptr<DiffieHellmanContext> pDhContext(new DiffieHellmanContext());
    bool success = pDhContext->init(primeBuf, generatorBuf);
    if (!success) {
        return NFErr_MalformedData; // CAD_ERR_KEYGEN;
    }

    // Note: The input extractable value only applies to the private key. The
    // public key should always be forced to extractable = true.

    // make a Key object for the public key and record it in the keyMap
    pubKeyHandle = allocateKeyHandle();    // pubKeyHandle is output parm
    const Key pubKey(pDhContext->getPubKey(), pDhContext, PUBLIC, true, algVar, keyUsage);
    keyMap_[pubKeyHandle] = pubKey;

    // likewise for the private key
    privKeyHandle = allocateKeyHandle();   // privKeyHandle is output parm
    const Key privKey(pDhContext->getPrivKey(), pDhContext, PRIVATE, extractable, algVar,
                      keyUsage);
    keyMap_[privKeyHandle] = privKey;

    return NFErr_OK;
}

//////////
//
// dhDerive
//
//////////
NFErr WebCrypto::dhDerive(uint32_t baseKeyHandle, const DataBuffer& peerPublicKeyBuf,
                          const Variant& derivedAlgObj, bool derivedExtractable,
                          unsigned int derivedKeyUsage,
                          /*out*/uint32_t& derivedKeyHandle)
{
    ScopedMutex lock(mutex_);

    if (!isInited_)
        return NFErr_Uninitialized;

    // check base key
    if (!hasKey(baseKeyHandle))
        return NFErr_BadParameter;

    // verify the provided base key is permitted this usage
    if (!isUsageAllowed(baseKeyHandle, DERIVE))
    {
        NTRACE(TRACE_CRYPTO, "WebCrypto::dhDerive: operation disallowed by keyUsage");
        return NFErr_BadParameter;
    }

    // verify the provided base key is intended for this algorithm
    if (!isKeyAlgMatch(baseKeyHandle, DH))
    {
        NTRACE(TRACE_CRYPTO,
               "WebCrypto::dhDerive: operation incompatible with key algorithm");
        return NFErr_BadParameter;
    }

    // compute the shared secret
    const shared_ptr<DiffieHellmanContext> pDhContext(keyMap_[baseKeyHandle].pDhContext);
    assert(pDhContext);
    bool success = pDhContext->computeSharedSecret(peerPublicKeyBuf);
    if (!success)
    {
        NTRACE(TRACE_CRYPTO, "WebCrypto::dhDerive: shared secret computation failed");
        return NFErr_MalformedData;
    }

    // retrieve the shared secret and create a new Key object in the key store
    const DataBuffer sharedSecret = pDhContext->getSharedSecret();
    assert(!sharedSecret.isEmpty());
    derivedKeyHandle = allocateKeyHandle();    // derivedKeyHandle is output parm
    const Key derivedKey(sharedSecret, pDhContext, SECRET, derivedExtractable,
                         derivedAlgObj, derivedKeyUsage);
    keyMap_[derivedKeyHandle] = derivedKey;


    return NFErr_OK;
}

//=============================================================================
// TODO: pablo: not sure if we should put these here - as they contain 'secrets'?

static void deriveSessionKeys(const DataBuffer& kD,
                              const DataBuffer& data,
                              /*out*/DataBuffer& k1,
                              /*out*/DataBuffer& k2)
{
    // Derive Kenc and Khmac from Kd and data according to the following algorithm.
    // See section 4.3.3 of
    // https://confluence.netflix.com/display/SCRF/Message+Security+Layer+%28MSL%29+Configuration+Guide
    // bits = HMAC-SHA384(SHA384(Kd), data);
    // Kenc' = bits[0...15]
    // Khmac' = bits[16...47]
    Digester digester(DigestAlgo::SHA384());
    digester.init();
    digester.update(kD);
    const DataBuffer hash(digester.final());
    netflix::HMAC hmac(hash, DigestAlgo::SHA384());
    hmac.init();
    hmac.update(data);
    const DataBuffer bits(hmac.final<DataBuffer>());
    assert(bits.size() >= (16 + 32));
    k1 = DataBuffer(bits.data(), 16);
    k2 = DataBuffer(bits.data() + 16, 32);

    NTRACES(TRACE_CRYPTO, "Kd " << kD.toHex().toString());
    NTRACES(TRACE_CRYPTO, "SHA384(Kd) " << hash.toHex().toString());
    NTRACES(TRACE_CRYPTO, "data " << data.toHex().toString());
    NTRACES(TRACE_CRYPTO, "HMAC-SHA384(SHA384(Kd),data) " << bits.toHex().toString());
    NTRACES(TRACE_CRYPTO, "Kenc " << k1.toHex().toString());
    NTRACES(TRACE_CRYPTO, "Khmac " << k2.toHex().toString());
}


static const DataBuffer deriveWrappingKey(const DataBuffer& kEnc,
        const DataBuffer& kHmac)
{
    // Derive kWrap key data from kEnc and kHmac, according to the following algorithm.
    // See section 2.3 of
    // https://confluence.netflix.com/display/SCRF/Message+Security+Layer+%28MSL%29+Configuration+Guide
    // kWrap = trunc_128(HMAC-SHA256(HMAC-SHA256(salt, concat(kEnc, kHmac)), info))
    // where the first argument to HMAC-SHA256 is the key.
    // salt = 02 76 17 98 4f 62 27 53 9a 63 0b 89 7c 01 7d 69
    // info = 80 9f 82 a7 ad df 54 8d 3e a9 dd 06 7f f9 bb 91
    const unsigned char saltAry[] =
    {0x02, 0x76, 0x17, 0x98, 0x4f, 0x62, 0x27, 0x53, 0x9a, 0x63, 0x0b, 0x89, 0x7c, 0x01, 0x7d, 0x69};
    const DataBuffer salt = DataBuffer::fromRawData(saltAry, sizeof(saltAry));
    const unsigned char infoAry[] =
    {0x80, 0x9f, 0x82, 0xa7, 0xad, 0xdf, 0x54, 0x8d, 0x3e, 0xa9, 0xdd, 0x06, 0x7f, 0xf9, 0xbb, 0x91};
    const DataBuffer info = DataBuffer::fromRawData(infoAry, sizeof(infoAry));
    netflix::HMAC hmac1(salt, DigestAlgo::SHA256());
    hmac1.init();
    hmac1.update(kEnc);                              // HMAC-SHA256(salt, cat)
    hmac1.update(kHmac);
    netflix::HMAC hmac2(hmac1.final<DataBuffer>(), DigestAlgo::SHA256());
    hmac2.init();
    hmac2.update(info);                              // HMAC-SHA256(hmac1, info)
    DataBuffer kWrap(hmac2.final<DataBuffer>());
    assert(kWrap.size() > 128/8);
    kWrap.resize(128/8);                            // truncate kWrap to 128 bits
    return kWrap;
}

//=============================================================================

NFErr WebCrypto::nflxDhDerive(uint32_t baseKeyHandle, const DataBuffer& peerPublicKeyData,
        uint32_t derivationKeyHandle,
        /*out*/uint32_t& encryptionKeyHandle,
        /*out*/uint32_t& hmacKeyHandle,
        /*out*/uint32_t& wrappingKeyHandle)
{
    ScopedMutex lock(mutex_);

    if (!isInited_)
        return NFErr_Uninitialized;

    // check base key
    if (!hasKey(baseKeyHandle))
        return NFErr_BadParameter;

    // check derivation key
    if (!hasKey(derivationKeyHandle))
        return NFErr_BadParameter;

    // verify the provided base key is permitted this usage
    if (!isUsageAllowed(baseKeyHandle, DERIVE))
    {
        NTRACE(TRACE_CRYPTO, "WebCrypto::nflxDhDerive: operation disallowed by keyUsage");
        return NFErr_BadParameter;
    }

    // verify the provided base key is intended for this algorithm
    if (!isKeyAlgMatch(baseKeyHandle, NFLX_DH))
    {
        NTRACE(TRACE_CRYPTO, "WebCrypto::nflxDhDerive: operation incompatible with base key algorithm");
        return NFErr_BadParameter;
    }

    // verify the provided derivation key is intended for this algorithm
    if (!isKeyAlgMatch(derivationKeyHandle, AES_KW))
    {
        NTRACE(TRACE_CRYPTO, "WebCrypto::nflxDhDerive: operation incompatible with derivation key algorithm");
        return NFErr_BadParameter;
    }

    // TODO: should we check the usage of the derivation key? what should it be?

    if (peerPublicKeyData.empty())
    {
        NTRACE(TRACE_CRYPTO, "WebCrypto::nflxDhDerive: empty peer public key");
        return NFErr_BadParameter;
    }

    NTRACES(TRACE_CRYPTO, "Peer public data " << peerPublicKeyData.toHex().toString());

    // compute the shared secret
    const shared_ptr<DiffieHellmanContext> pDhContext(keyMap_[baseKeyHandle].pDhContext);
    assert(pDhContext);
    bool success = pDhContext->computeSharedSecret(peerPublicKeyData);
    if (!success)
    {
        NTRACE(TRACE_CRYPTO, "WebCrypto::nflxDhDerive: shared secret computation failed");
        return NFErr_MalformedData;
    }
    DataBuffer sharedSecret = pDhContext->getSharedSecret();
    assert(!sharedSecret.empty());

    /*
    Per MSL spec:

    Since the computed shared secret is a numeric value (typically a BigInteger) it
    must be converted into a byte array when computing the HMAC-SHA384. The byte array
    will be the minimum number of bytes required for the two's complement representation
    in big-endian byte-order (the most significant byte is first) including at least one
    sign bit, with exactly one zero byte in the zeroth element. As a result, a shared
    secret value of zero will be represented by an array of length one containing a single
    byte with a value of zero. This representation is compatible with the Java
    BigInteger.toByteArray() function and BigInteger(byte[]) constructor.

    So, essentially, if the first byte of the shared secret is not zero, we prepend
    a zero byte.
    */

    if (sharedSecret.empty())
    {
        sharedSecret = DataBuffer(1, 0);
    }
    else if (sharedSecret[0] != 0)
    {
        DataBuffer temp(sharedSecret.size() + 1);
        temp.replace(0, 1, DataBuffer(1, 0));
        temp.replace(1, sharedSecret.size(), sharedSecret);
        sharedSecret = temp;
    }

    // Derive the keys
    const DataBuffer& kDRaw = keyMap_[derivationKeyHandle].key;
    DataBuffer kEncRaw;
    DataBuffer kHmacRaw;
    deriveSessionKeys(kDRaw, sharedSecret, kEncRaw, kHmacRaw);
    const DataBuffer kWrapRaw(deriveWrappingKey(kEncRaw, kHmacRaw));
    NTRACES(TRACE_CRYPTO, "Kwrap " << kWrapRaw.toHex().toString());
    if (!kEncRaw.size() || !kHmacRaw.size() || !kWrapRaw.size())
    {
        NTRACE(TRACE_CRYPTO, "WebCrypto::nflxDhDerive: key derivation failed");
        return NFErr_MalformedData;
    }

    // Store the keys in the key map
    {
        encryptionKeyHandle = allocateKeyHandle();  // encryptionKeyHandle is output parm
        Variant algVar;
        algVar["name"] = toString(WebCrypto::AES_CBC);
        keyMap_[encryptionKeyHandle] = Key(kEncRaw, SECRET, false, algVar, ENCRYPT | DECRYPT);
    }
    {
        hmacKeyHandle = allocateKeyHandle();  // hmacKeyHandle is output parm
        Variant algVar;
        algVar["name"] = toString(WebCrypto::HMAC);
        algVar["params"]["hash"]["name"] = toString(WebCrypto::SHA256);
        keyMap_[hmacKeyHandle] = Key(kHmacRaw, SECRET, false, algVar, SIGN | VERIFY);
    }
    {
        // TODO: according to Wes's spec, this key shoul have no usage, but we don't
        // have a way to express that - because a zero usage means ALL usages.

        wrappingKeyHandle = allocateKeyHandle();  // wrappingKeyHandle is output parm
        Variant algVar;
        algVar["name"] = toString(WebCrypto::AES_KW);
        keyMap_[wrappingKeyHandle] = Key(kWrapRaw, SECRET, false, algVar, 0);
    }

    return NFErr_OK;
}

//////////
//
// unwrapJwe
//
//////////
NFErr WebCrypto::unwrapJwe(const DataBuffer& keyDataBuf, uint32_t wrappingKeyHandle,
                           const Variant &algVar, bool extractable,
                           unsigned int keyUsage,
                           /*out*/uint32_t& keyHandle)
{
    ScopedMutex lock(mutex_);

    if (!isInited_)
        return NFErr_Uninitialized;

    // http://www.w3.org/2012/webcrypto/wiki/KeyWrap_Proposal

    // NOTE: the input parms algVar, extractable, and keyUsage are not used by
    // this method. They are passed through to the final importJwk call to be
    // applied to the unwrapped key.

    // try each recipient in a JWE-JS until one passes or they are exhausted
    if (isJweJs(keyDataBuf))
    {
        const Variant jweJsVar = Variant::fromJSON(keyDataBuf);
        const Variant recipients = jweJsVar["recipients"];
        const int size = recipients.size();
        for (int rcpIdx = 0; rcpIdx < size; ++rcpIdx)
        {
            NTRACE(TRACE_CRYPTO, "WebCrypto::unwrapJwe: trying JWE-JS recipient %d of %d",
                   rcpIdx+1, recipients.size());
            std::vector<DataBuffer> jweVec;
            const Variant &recipientVar = recipients[rcpIdx];
            jweVec.push_back(recipientVar.mapValue<DataBuffer>("header"));
            jweVec.push_back(recipientVar.mapValue<DataBuffer>("encrypted_key"));
            jweVec.push_back(jweJsVar.mapValue<DataBuffer>("initialization_vector"));
            jweVec.push_back(jweJsVar.mapValue<DataBuffer>("ciphertext"));
            jweVec.push_back(recipientVar.mapValue<DataBuffer>("integrity_value"));
            if (unwrapJwe(jweVec, wrappingKeyHandle, algVar, extractable,
                          keyUsage, keyHandle) == NFErr_OK)
            {
                NTRACE(TRACE_CRYPTO, "WebCrypto::unwrapJwe: JWE-JS recipient %d OK",
                       rcpIdx+1);
                return NFErr_OK;
            }
        }
        NTRACE(TRACE_CRYPTO,
               "WebCrypto::unwrapJwe: Failed to unwrap after trying all recipients");
        return NFErr_MalformedData; // BADENCODING
    }
    else
    {
        const std::vector<DataBuffer> jweVec = keyDataBuf.split('.');
        if (jweVec.size() != 5)
            return NFErr_MalformedData;
        return unwrapJwe(jweVec, wrappingKeyHandle, algVar, extractable,
                         keyUsage, keyHandle);
    }
}

NFErr WebCrypto::unwrapJwe(const std::vector<DataBuffer>& jweStrVec,
                           uint32_t wrappingKeyHandle, const Variant& algVar,
                           bool extractable, unsigned int keyUsage,
                           /*out*/uint32_t& keyHandle)
{

    assert(jweStrVec.size() == 5);
    ScopedMutex lock(mutex_);
    const DataBuffer headerBuf = jweStrVec[0].decode(DataBuffer::Encoding_Base64); // Encoded JWE Header
    const DataBuffer encCmkBuf = jweStrVec[1].decode(DataBuffer::Encoding_Base64); // Encoded JWE Encrypted Content Master Key (CMK)
    const DataBuffer ivBuf = jweStrVec[2].decode(DataBuffer::Encoding_Base64); // Encoded JWE Initialization Vector
    const DataBuffer cipherTextBuf = jweStrVec[3].decode(DataBuffer::Encoding_Base64); // Encoded JWE Ciphertext (the actual target key)
    const DataBuffer macBuf = jweStrVec[4].decode(DataBuffer::Encoding_Base64); // Encoded JWE Integrity Value

    NTRACE(TRACE_CRYPTO, "\theader = %s/%s", headerBuf.c_str(), jweStrVec[0].c_str());
    NTRACE(TRACE_CRYPTO, "\tcmk = %s", jweStrVec[1].c_str());
    NTRACE(TRACE_CRYPTO, "\tiv = %s", jweStrVec[2].c_str());

    // ---- process JWE header
    if (headerBuf.empty())
    {
        Log::warn(TRACE_CRYPTO, "WebCrypto::unwrapJwe: JWE Header bad encoding");
        return NFErr_MalformedData;
    }
    const Variant headerVar = Variant::fromJSON(headerBuf);

    // ---- validate JWE header 'alg'
    const std::string jweHeaderAlgStr = headerVar.mapValue<std::string>("alg");
    if (jweHeaderAlgStr.empty())
    {
        NTRACE(TRACE_CRYPTO, "WebCrypto::unwrapJwe: JWE Header alg field not found");
        return NFErr_MalformedData;
    }
    if (jweHeaderAlgStr != "RSA-OAEP" && jweHeaderAlgStr != "A128KW" && jweHeaderAlgStr != "A256KW")
    {
        NTRACE(TRACE_CRYPTO, "WebCrypto::unwrapJwe: JWE Header alg field must be "
               "RSA-OAEP, A128KW, or A256KW, found %s", jweHeaderAlgStr.c_str());
        return NFErr_MalformedData; // UNKNOWN_ALGO
    }
    const Algorithm jweHeaderAlg = (jweHeaderAlgStr == "A128KW" || jweHeaderAlgStr == "A256KW") ? AES_KW : RSA_OAEP;

    // ---- validate JWE header 'enc'
    const std::string jweHeaderEncStr = headerVar.mapValue<std::string>("enc");

    // For AES key upwrap, 'enc' must be A128GCM (the spec also allows
    // A128CBC+HS256 but we don't support it)
    if (jweHeaderAlg == AES_KW && jweHeaderEncStr != "A128GCM")
    {
        NTRACE(TRACE_CRYPTO, "WebCrypto::unwrapJwe: For AES key unwrap, the JWE Header "
               "'enc' field must be A128GCM");
        return NFErr_MalformedData;
    }

    // For RSA-OAEP key unwrap, 'enc' must be A128GCM or A256GCM
    if (jweHeaderAlg == RSA_OAEP &&
        (jweHeaderEncStr != "A128GCM" && jweHeaderEncStr != "A256GCM"))
    {
        NTRACE(TRACE_CRYPTO, "WebCrypto::unwrapJwe: For RSA_OAEP key unwrap, the JWE "
               "Header 'enc' field must be A128GCM or A256GCM");
        return NFErr_MalformedData;
    }

    // get the key length to check the CMK length once it is decrypted
    const int jweCmkKeyLen = (jweHeaderEncStr == "A128GCM") ? 128/8 : 256/8;

    // ---- check the wrapping key
    if (!hasKey(wrappingKeyHandle)) {
        return NFErr_MalformedData;
    }
    if (!isUsageAllowed(wrappingKeyHandle, UNWRAP))
    {
        NTRACE(TRACE_CRYPTO, "WebCrypto::unwrapJwe: operation disallowed by keyUsage");
        return NFErr_BadParameter;
    }
    Key key = keyMap_[wrappingKeyHandle];
    const Algorithm wrappingKeyAlg = getAlgorithmFromVar(key.algVar);
    if (wrappingKeyAlg != jweHeaderAlg)
    {
        Log::warn(TRACE_CRYPTO, "WebCrypto::unwrapJwe: Wrapping key algorithm does not "
                   "match JWE header algorithm");
        return NFErr_MalformedData;
    }
    if (wrappingKeyAlg == RSA_OAEP && !key.pRsaContext.get())
    {
        // this should probably be an assert
        Log::warn(TRACE_CRYPTO, "WebCrypto::unwrapJwe: RSA wrapping key not initialized");
        return NFErr_Internal;
    }
    if (wrappingKeyAlg == AES_KW && (extractIntFromString(jweHeaderAlgStr) != (int)key.key.size() * 8))
    {
        // this should probably be an assert
        NTRACE(TRACE_CRYPTO,
               "WebCrypto::unwrapJwe: AES wrapping key has incompatible key size");
        return NFErr_BadParameter;
    }

    // ---- extract and decrypt the CMK
    NTRACE(TRACE_CRYPTO, "encrypted CMK = %s", encCmkBuf.toBase64().c_str());
    DataBuffer cmkBuf;
    bool success;
    if (jweHeaderAlg == RSA_OAEP)
    {
        success = key.pRsaContext->privateDecrypt(
            encCmkBuf, cmkBuf, RsaContext::PKCS1_OAEP);
    }
    else // jweHeaderAlg == AES_KW
    {
        // Wes is seeing a problem where if the encrypted CMK is set very short,
        // he gets an out of memory error when OpenSSL's AES_unwrap_key is called
        // inside AesKeyWrapper.unwrap(). This does not happen on any other
        // machine so far. But anyway try to work around the problem by testing
        // the input encrypted CMK size against the expected decrypted CMK size,
        // the latter as read from the JWE header 'enc' field. For AES key
        // wrapping specifically, we know that the ciphertext size is always the
        // cleartext size + 8.
        if (encCmkBuf.size() < jweCmkKeyLen)
        {
            NTRACE(TRACE_CRYPTO, "encrypted CMK seems too short, aborting");
            return NFErr_MalformedData; // CIPHERERROR
        }
        AesKeyWrapper aesKeyWrapper(key.key);
        success = aesKeyWrapper.unwrap(encCmkBuf, cmkBuf);
    }
    if (!success) {
        return NFErr_MalformedData; // cipher error
    }
    NTRACE(TRACE_CRYPTO, "CMK = %s", cmkBuf.toBase64().c_str());

    // ---- verify the CMK is the proper length (must be 128 or 256 bits), and
    // consistent with the JWE header "enc" field
    if (cmkBuf.size() != jweCmkKeyLen && cmkBuf.size() != 128/8 && cmkBuf.size() != 256/8)
    {
        NTRACE(TRACE_CRYPTO, "WebCrypto::unwrapJwe: bad CMK length");
        return NFErr_MalformedData; // cipher error
    }

    NTRACE(TRACE_CRYPTO, "IV = %s", ivBuf.toBase64().c_str());

    // ---- extract the ciphertext
    NTRACE(TRACE_CRYPTO, "ciphertext = %s", cipherTextBuf.toBase64().c_str());

    // ---- extract the MAC
    NTRACE(TRACE_CRYPTO, "authentication tag = %s", macBuf.toBase64().c_str());

    // ---- construct the AAD
    DataBuffer aad;
    aad.reserve(jweStrVec[0].size() + jweStrVec[1].size() + jweStrVec[2].size() + 2);
    aad << jweStrVec[0] << '.' << jweStrVec[1] << '.' << jweStrVec[2];
    NTRACE(TRACE_CRYPTO, "AAD = %s", aad.c_str());

    // ---- do the decrypt, if successfull clearText will contain the JWK
    AesGcmCipher aesGcmCipher(cmkBuf, ivBuf);
    DataBuffer clearText;
    success = aesGcmCipher.decrypt(cipherTextBuf, aad, macBuf, clearText);
    NTRACE(TRACE_CRYPTO, "cleartext64 = %s", clearText.toBase64().c_str());
    if (!success)
    {
        NTRACE(TRACE_CRYPTO, "WebCrypto::unwrapJwe: aesGcmCipher.decrypt fail");
        return NFErr_MalformedData; // cipher error
    }

    NTRACE(TRACE_CRYPTO,"Decrypt / Authentication success!!");

    // clearTextVuc contains the unwrapped JWK key to import

    // ---- import the JWK and we are done
    KeyType keyType;
    return importJwk(clearText, algVar, extractable, keyUsage, keyHandle, keyType);
}

//////////
//
// wrapJwe
//
//////////
NFErr WebCrypto::wrapJwe(uint32_t toBeWrappedKeyHandle, uint32_t wrappingKeyHandle,
                         const Variant& wrappingAlgoObj, JweEncMethod jweEncMethod,
                         /*out*/DataBuffer& wrappedKeyJcsBuf)
{
    ScopedMutex lock(mutex_);

    if (!isInited_)
        return NFErr_Uninitialized;

    // http://www.w3.org/2012/webcrypto/wiki/KeyWrap_Proposal

    NTRACE(TRACE_CRYPTO, "WebCrypto::wrapJwe:");

    // ---- Input checks

    // wrappingAlgoObj may be null. If not make sure it is a supported algorithm.
    // We support only RSA-OAEP and AES-KW algorithms
    Algorithm wrappingAlgoType = INVALID_ALGORITHM;
    if (!wrappingAlgoObj.isNull())
    {
        wrappingAlgoType = getAlgorithmFromVar(wrappingAlgoObj);
        if (wrappingAlgoType != RSA_OAEP && wrappingAlgoType != AES_KW)
        {
            NTRACE(TRACE_CRYPTO,
                   "WebCrypto::wrapJwe: Only RSA-OAEP and AES-KW algorithms supported");
            return NFErr_NotAllowed;
        }
    }

    // verify the keys exist
    if (!hasKey(toBeWrappedKeyHandle) || !hasKey(wrappingKeyHandle))
    {
        NTRACE(TRACE_CRYPTO, "WebCrypto::wrapJwe: key does not exist");
        return NFErr_BadParameter;
    }

    // check the wrapping key
    if (!isUsageAllowed(wrappingKeyHandle, WRAP))
    {
        NTRACE(TRACE_CRYPTO,
               "WebCrypto::wrapJwe: operation disallowed by keyUsage");
        return NFErr_BadParameter;
    }

    const Key wrappingKey = keyMap_[wrappingKeyHandle];
    const Algorithm wrappingKeyAlgoType = getAlgorithmFromVar(wrappingKey.algVar);

    // if provided, the API alg must match the wrapping key's alg
    if (!wrappingAlgoObj.isNull() && (wrappingAlgoType != wrappingKeyAlgoType))
    {
        NTRACE(TRACE_CRYPTO, "WebCrypto::wrapJwe: wrapping key and wrapping algorithm "
               "are inconsistent");
        return NFErr_BadParameter;
    }
    const size_t wrappingKeyLengthBits = wrappingKey.key.size() * 8;
    switch (wrappingKeyAlgoType)
    {
    case RSA_OAEP:
    {
        // make sure the key has an RSA context already
        if (!wrappingKey.pRsaContext.get())
        {
            NTRACE(TRACE_CRYPTO,
                   "WebCrypto::wrapJwe: RSA-OAEP wrapping key not initialized");
            return NFErr_MalformedData; // UNKNOWN_ALGO
        }
        break;
    }
    case AES_KW:
    {
        // make sure the key is the right size for AES key wrap
        if (wrappingKeyAlgoType == AES_KW && wrappingKeyLengthBits != 128 &&
            wrappingKeyLengthBits != 256)
        {
            NTRACE(TRACE_CRYPTO,
                   "WebCrypto::wrapJwe: AES wrapping key must be 128 or 256 bits");
            return NFErr_MalformedData; // UNKNOWN_ALGO
        }
        break;
    }
    default:
    {
        // wrapping key alg must be either RSA-OAEP or AES_KW
        NTRACE(TRACE_CRYPTO,
               "WebCrypto::wrapJwe: Only RSA-OAEP and AES-KW algorithms supported");
        return NFErr_MalformedData; // UNKNOWN_ALGO
    }
    }

    // ---- Construct JWE header
    DataBuffer jweHeaderStr;
    jweHeaderStr.reserve(128);
    jweHeaderStr += "{\"alg\":";
    if (wrappingKeyAlgoType == RSA_OAEP)
        jweHeaderStr += "\"RSA-OAEP\"";
    else    // wrappingKeyAlgoType == AES_KW
        jweHeaderStr += (wrappingKeyLengthBits == 128) ? "\"A128KW\"" : "\"A256KW\"";
    jweHeaderStr += ",\"enc\":";
    if (jweEncMethod == A128GCM)
        jweHeaderStr += "\"A128GCM\"}";
    else // jweEncMethod == A256GCM
        jweHeaderStr += "\"A256GCM\"}";
    NTRACE(TRACE_CRYPTO, "jweHeader = %s", jweHeaderStr.c_str());
    const DataBuffer jweHeaderBufUrl = jweHeaderStr.encode(DataBuffer::Encoding_Base64_UrlSafe);
    NTRACE(TRACE_CRYPTO, "jweHeader64url = %s", jweHeaderBufUrl.c_str());

    // ---- Generate a random Content Master Key (CMK)
    const int cmkKeyLenBytes = (jweEncMethod == A128GCM) ? 16 : 32;
    DataBuffer cmk;
    cmk.resize(cmkKeyLenBytes);
    Random::next(cmk.data(), cmkKeyLenBytes);
    NTRACE(TRACE_CRYPTO, "CMK = %s", cmk.toBase64().c_str());

    // ---- Encrypt the CMK according to the wrapping algo type
    DataBuffer encCmkBuf;
    bool success;
    if (wrappingKeyAlgoType == RSA_OAEP)
    {
        success = wrappingKey.pRsaContext->publicEncrypt(cmk, encCmkBuf, RsaContext::PKCS1_OAEP);
    }
    else // wrappingKeyAlgoType == AES_KW
    {
        AesKeyWrapper aesKeyWrapper(wrappingKey.key);
        success = aesKeyWrapper.wrap(cmk, encCmkBuf);
    }
    if (!success) {
        return NFErr_MalformedData; // CIPHERERROR;
    }
    NTRACE(TRACE_CRYPTO, "encrypted CMK = %s", encCmkBuf.toBase64().c_str());

    // ---- Generate a random 96-bit initialization vector (IV)
    DataBuffer iv;
    iv.resize(12);
    Random::next(iv.data(), 12);

    // ---- Construct the GCM "Additional Authenticated Data" parameter.
    // This is a concatenation of the JWE Header, the CMK, and the IV, each
    // base64url-encoded and separated with a '.'.
    DataBuffer encCmk64url = encCmkBuf.encode(DataBuffer::Encoding_Base64_UrlSafe);
    const DataBuffer iv64Url = iv.encode(DataBuffer::Encoding_Base64_UrlSafe);
    NTRACE(TRACE_CRYPTO, "IV = %s", iv64Url.c_str());
    DataBuffer aadBuf = jweHeaderBufUrl;
    aadBuf.reserve(jweHeaderBufUrl.size() + encCmk64url.size() +
                   iv64Url.size() + 2);
    aadBuf << '.' << encCmk64url << '.' << iv64Url;
    NTRACE(TRACE_CRYPTO, "AAD = %s", aadBuf.c_str());

    // ---- Get a JKW representation of the key to be wrapped
    const Key toBeWrappedKey(keyMap_[toBeWrappedKeyHandle]);
    DataBuffer jwkBuf;
    NFErr err = exportJwk(keyMap_[toBeWrappedKeyHandle], jwkBuf);
    if (!err.ok())
        return err;
    NTRACE(TRACE_CRYPTO, "JWK to wrap = %s", jwkBuf.toBase64().c_str());

    // ---- Encrypt
    // Encrypt the cleartext JWK with AES GCM using the CMK as the
    // encryption key, the JWE Initialization Vector, and the AAD value
    // above, with a 128 bit "authentication tag" output
    AesGcmCipher aesGcmCipher(cmk, iv);
    DataBuffer cipherTextBuf, macBuf;
    success = aesGcmCipher.encrypt(jwkBuf, aadBuf, cipherTextBuf, macBuf);
    if (!success) {
        return NFErr_MalformedData; // CIPHERERROR;
    }
    NTRACE(TRACE_CRYPTO, "Ciphertext = %s", cipherTextBuf.toBase64().c_str());
    NTRACE(TRACE_CRYPTO, "MAC = %s", macBuf.toBase64().c_str());

    // ---- Assemble the final representation
    // The Compact Serialization of this result is the concatenation of:
    // - the base64url-Encoded JWE Header
    // - the base64url-Encoded JWE Encrypted Key (CMK)
    // - the base64url-Encoded JWE Initialization Vector
    // - the base64url-Encoded JWE Ciphertext
    // - the base64url-Encoded JWE Integrity Value (Mac)
    // in that order, with the five strings being separated by four period
    // ('.') characters.

    // Already have the base64url-Encoded JWE Header in jweHeaderBufUrl

    // the base64url-Encoded JWE Ciphertext
    const DataBuffer cipherTextBufUrl = cipherTextBuf.encode(DataBuffer::Encoding_Base64_UrlSafe);

    const DataBuffer macBufUrl = macBuf.encode(DataBuffer::Encoding_Base64_UrlSafe);

#ifdef JWECS_WRAP_OUTPUT    // output JWE-CS format

    wrappedKeyJcsBuf.reserve(jweHeaderBufUrl.size()
                             + encCmk64url.size()
                             + iv64Url.size()
                             + cipherTextBufUrl.size()
                             + macBufUrl.size()
                             + 4);
    wrappedKeyJcsBuf << jweHeaderBufUrl << '.' << encCmk64url << '.' << iv64Url << '.' << cipherTextBufUrl << '.' << macBufUrl;
#else   // output JWE-JS format

    //var jweJsData = latin1.parse(JSON.stringify({
    //    recipients:[{
    //        header:            jweDataAry[0],
    //        encrypted_key:     jweDataAry[1],
    //        integrity_value:   jweDataAry[4],
    //    }],
    //    initialization_vector: jweDataAry[2],
    //    ciphertext:            jweDataAry[3]

    Variant jsonVar;
    Variant recipientsVarAry;
    Variant recipientVar;
    recipientVar["header"] = jweHeaderBufUrl;
    recipientVar["encrypted_key"] = encCmk64url;
    recipientVar["integrity_value"] = macBufUrl;
    recipientsVarAry.push_back(recipientVar);
    jsonVar["recipients"] = recipientsVarAry;
    jsonVar["initialization_vector"] = iv64Url;
    jsonVar["ciphertext"] = cipherTextBufUrl;
    JSONFormatter formatter;
    formatter.format(jsonVar, wrappedKeyJcsBuf);
#endif

    NTRACE(TRACE_CRYPTO, "Final JWE = %s", wrappedKeyJcsBuf.c_str());

    return NFErr_OK;
}

//////////
//
// symKeyGen
//
//////////
NFErr WebCrypto::symKeyGen(const Variant &algVar, bool extractable,
                           unsigned int keyUsage,
                           /*out*/uint32_t &keyHandle)
{
    ScopedMutex lock(mutex_);

    if (!isInited_)
        return NFErr_Uninitialized;

    const Algorithm algType = getAlgorithmFromVar(algVar);
    if (isAlgorithmRsa(algType))
    {
        Log::warn(TRACE_CRYPTO, "WebCrypto::symKeyGen: not a symmetric algorithm");
        return NFErr_BadParameter;
    }

    // make sure we have a params object
    if (!algVar.contains("params"))
    {
        Log::warn(TRACE_CRYPTO, "WebCrypto::symKeyGen: algorithm missing params field");
        return NFErr_BadParameter;
    }

    if (!reconcileAlgVsUsage(algType, keyUsage))
    {
        NTRACE(TRACE_CRYPTO, "WebCrypto::symKeyGen: inconsistent algorithm vs usage");
        return NFErr_BadParameter;
    }

    // get the key length
    int keyLengthBits;
    if (isAlgorithmAes(algType))
    {
        NTRACE(TRACE_CRYPTO, "\tAES generate key");

        // params is a AesKeyGenParams
        if (!algVar["params"].contains("length"))
        {
            Log::warn(TRACE_CRYPTO,
                      "WebCrypto::symKeyGen: algorithm params missing length field");
            return NFErr_BadParameter;
        }

        // extract the key length
        keyLengthBits = algVar["params"].mapValue<int>("length");
        NTRACE(TRACE_CRYPTO, "\tkey length bits: %d", keyLengthBits);
    }
    else // (algType == WebCrypto::HMAC)
    {
        NTRACE(TRACE_CRYPTO, "\tHMAC generate key");

        // paramsObj is a HmacParams
        if (!algVar["params"].contains("hash"))
        {
            Log::warn(TRACE_CRYPTO, "WebCrypto::symKeyGen: algorithm params missing "
                      "hash algorithm field");
            return NFErr_BadParameter;
        }

        const Algorithm hashType = getHashAlgorithmFromVar(algVar);
        if (hashType == IWebCrypto::INVALID_ALGORITHM)
        {
            Log::warn(TRACE_CRYPTO, "WebCrypto::symKeyGen: algorithm params hash "
                      "algorithm missing name field");
            return NFErr_BadParameter;
        }

        // Deduce the required key length from the hash name. RFC4868 recommends
        // the key length be equal to the output length of the hash function.
        switch (hashType)
        {
        case WebCrypto::SHA1:      keyLengthBits = 160;    break;
        case WebCrypto::SHA224:    keyLengthBits = 224;    break;
        case WebCrypto::SHA256:    keyLengthBits = 256;    break;
        case WebCrypto::SHA384:    keyLengthBits = 384;    break;
        case WebCrypto::SHA512:    keyLengthBits = 512;    break;
        default:
            Log::warn(TRACE_CRYPTO,
                      "WebCrypto::symKeyGen: unknown HMAC inner hash algorithm");
            return NFErr_Internal;
        }
    }

    const DataBuffer randBytes = Random::next((keyLengthBits + 7) / 8);
    keyHandle = allocateKeyHandle();
    Key key(randBytes, shared_ptr<RsaContext>(), SECRET, extractable, algVar, keyUsage);
    keyMap_[keyHandle] = key;

    return NFErr_OK;
}

//////////
//
// eccSign
//
//////////
NFErr WebCrypto::eccSign(uint32_t keyHandle, const Variant &algoVar,
                         const DataBuffer &dataBuf, /*out*/DataBuffer& sigBuf)
{
    ScopedMutex lock(mutex_);

    if (!isInited_)
        return NFErr_Uninitialized;

    if (!hasKey(keyHandle))
        return NFErr_BadParameter;

    // verify the provided key is permitted this usage
    if (!isUsageAllowed(keyHandle, SIGN))
    {
        NTRACE(TRACE_CRYPTO, "WebCrypto::eccSign: operation disallowed by keyUsage");
        return NFErr_BadParameter;
    }

    // verify the provided key is intended for this algorithm
    if (!isKeyAlgMatch(keyHandle, ECC))
    {
        NTRACE(TRACE_CRYPTO,
               "WebCrypto::eccSign: operation incompatible with key algorithm");
        return NFErr_BadParameter;
    }

    // determine the algorithm
    IWebCrypto::Algorithm algorithm = getHashAlgorithmFromVar(algoVar);
    if (!isAlgorithmSha(algorithm))
    {
        NTRACE(TRACE_CRYPTO, "WebCrypto::eccSign: bad hash algorithm %d", algorithm);
        return NFErr_BadParameter;
    }

    NTRACE(TRACE_CRYPTO, "WebCrypto::eccSign: not implemented");

    NRDP_UNUSED(dataBuf);
    NRDP_UNUSED(sigBuf);

    return NFErr_NotImplemented;
}

//////////
//
// addNamedKey
//
//////////
NFErr WebCrypto::addNamedKey(const std::string & name, uint32_t keyHandle)
{
    ScopedMutex lock(mutex_);
    if (!hasKey(keyHandle))
    {
        Log::warn(TRACE_CRYPTO,
                  "WebCrypto::addNamedKey: trying to add non-existent named key");
        return NFErr_BadParameter;
    }

    namedKeyMap_[name] = keyHandle;
    return NFErr_OK;
}

uint32_t WebCrypto::addInternalKey(const std::string & name, const Variant& algVar, unsigned int usage)
{
    ScopedMutex lock(mutex_);
    const uint32_t keyHandle = allocateKeyHandle();
    const Key key(DataBuffer(), SECRET, false, algVar, usage);
    keyMap_[keyHandle] = key;
    if (!name.empty()) {
        namedKeyMap_[name] = keyHandle;
    }
    return keyHandle;
}


//////////
//
// getNamedKeys
//
//////////
NFErr WebCrypto::getNamedKey(const std::string & name,
                             /*out*/ uint32_t & keyHandle)
{
    ScopedMutex lock(mutex_);

    NamedKeyMap::const_iterator it = namedKeyMap_.find(name);
    if (it == namedKeyMap_.end())
        return NFErr_NotFound;
    keyHandle = it->second;
    return NFErr_OK;
}

//////////
//
// persistKeys
//
//////////

static const std::string PersistedKeysStorageKey("WC/keys");

NFErr WebCrypto::persistKeys(const std::vector<uint32_t> & keyHandles)
{
    ScopedMutex lock(mutex_);

    persistentKeys_.clear();

    Variant keys;
    for(std::vector<uint32_t>::const_iterator it = keyHandles.begin(); it != keyHandles.end(); ++it)
    {
        const uint32_t handle(*it);
        KeyMap::const_iterator k = keyMap_.find(handle);
        if (k == keyMap_.end())
        {
            Log::trace(TRACE_CRYPTO,
                       "WebCrypto::persistKeys: invalid key handle");
            return NFErr_BadParameter;
        }
        Variant key = k->second.toJSON();
        key["handle"] = handle;
        keys.push_back(key);
        persistentKeys_.insert(handle);
    }

    NrdApplication * app = nrdApp();

    std::string value = keys.empty() ? std::string() : keys.toJSON();

    app->writeSystemConfiguration(PersistedKeysStorageKey, value);

    return NFErr_OK;
}

//////////
//
// restorePersistedKeys
//
//////////
void WebCrypto::restorePersistedKeys()
{
    ScopedMutex lock(mutex_);

    persistentKeys_.clear();
    NrdApplication * app = nrdApp();
    if (!app)
    {
        Log::warn(TRACE_CRYPTO, "Not restoring persisted keys, no app");
        return;
    }
    const std::string json = app->readSystemConfiguration(PersistedKeysStorageKey);
    if (json.empty())
        return;

    Variant keyArray = Variant::fromJSON(json);
    if (!keyArray.isArray()) {
        Log::warn(TRACE_CRYPTO, "Failed to parse persisted keys as JSON");
        return;
    }

    const std::vector<Variant> keys(keyArray.array());
    for (Variant::ArrayIterator it = keys.begin(); it != keys.end(); ++it)
    {
        if (uint32_t handle = (*it)["handle"].integer())
        {
            assert(!hasKey(handle));
            keyMap_[handle] = Key(*it);
            persistentKeys_.insert(handle);
        }
    }
}

//////////
//
// allocateKeyHandle
//
//////////
uint32_t WebCrypto::allocateKeyHandle()
{
    ScopedMutex lock(mutex_);

    static uint32_t nextHandle = kStartKeyHandle;
    const KeyMap::const_iterator end(keyMap_.end());
    while (keyMap_.find(nextHandle) != end)
        ++nextHandle;
    return nextHandle++;
}

//////////
//
// clearKeys
//
//////////
NFErr WebCrypto::clearKeys(std::set<uint32_t> & keyHandles)
{
    ScopedMutex lock(mutex_);

    //This drops all un-named keys and non-persistent keys
    //dumpKeys();
    //size_t oldSize = keyMap_.size();

    // Build a set of key handles to keep
    KeyHandleSet keep(persistentKeys_);
    for (NamedKeyMap::const_iterator it = namedKeyMap_.begin(); it != namedKeyMap_.end(); ++it)
        keep.insert(it->second);

    // Now drop any that are not in the set
    for (KeyMap::iterator it = keyMap_.begin(); it != keyMap_.end();) {
        if (keep.find(it->first) == keep.end()) {
            keyHandles.insert(it->first);
            keyMap_.erase(it++);
        } else {
            ++it;
        }
    }

    //Log::fatal() << "DROPPED " << oldSize - keyMap_.size() << " KEYS";
    //dumpKeys();
    return NFErr_OK;
}

NFErr WebCrypto::deleteKey(uint32_t keyHandle)
{
    ScopedMutex lock(mutex_);
    if (keyMap_.erase(keyHandle) == 1) {
        for (NamedKeyMap::iterator it = namedKeyMap_.begin(); it != namedKeyMap_.end();) {
            if (it->second == keyHandle)
                namedKeyMap_.erase(it++);
            else
                ++it;
        }
        persistentKeys_.erase(keyHandle);
        return NFErr_OK;
    }
    return NFErr_BadParameter;
}


void WebCrypto::dumpKeys()
{
    std::map<uint32_t, std::string> names;
    for (NamedKeyMap::const_iterator it = namedKeyMap_.begin(); it != namedKeyMap_.end(); ++it)
        names[it->second] = it->first;

    for (KeyMap::const_iterator it = keyMap_.begin(); it != keyMap_.end(); ++it)
    {
        const std::string name(names[it->first]);
        const bool persistent(persistentKeys_.find(it->first) != persistentKeys_.end());

        Variant k = it->second.toJSON();
        k.remove("key");
        Log::warn()
            << std::setw(2) << it->first
            << " : "
            << name
            << (persistent ? "persistent" : "")
            << " : " << k.toJSON();
    }
}

#if defined(BUILD_DEBUG)

int WebCrypto::getDHKeySharedSecretLength(uint32_t keyHandle) const
{
    ScopedMutex lock(mutex_);
    KeyMap::const_iterator it = keyMap_.find(keyHandle);
    if (it != keyMap_.end() && it->second.pDhContext) {
        return it->second.pDhContext->getSharedSecret().size();
    }
    return -1;
}

#endif

//////////
//
// WebCrypto::Key
//
//////////
WebCrypto::Key::Key()
    : type(SECRET),
      extractable(false)
{
}

WebCrypto::Key::Key(const DataBuffer &key,
                    shared_ptr<RsaContext> pRsaContext, KeyType type, bool extractable,
                    const Variant &algVar, unsigned int keyUsage)
    : key(key),
      pRsaContext(pRsaContext),
      type(type),
      extractable(extractable),
      algVar(algVar),
      keyUsage(keyUsage)
{
}

WebCrypto::Key::Key(const DataBuffer &key,
                    KeyType type, bool extractable,
                    const Variant &algVar, unsigned int keyUsage)
    : key(key),
      type(type),
      extractable(extractable),
      algVar(algVar),
      keyUsage(keyUsage)
{
}

WebCrypto::Key::Key(const DataBuffer &key,
                    shared_ptr<DiffieHellmanContext> pDhContext, KeyType type,
                    bool extractable, const Variant& algVar,
                    unsigned int keyUsage)
    : key(key),
      pDhContext(pDhContext),
      type(type),
      extractable(extractable),
      algVar(algVar),
      keyUsage(keyUsage)
{
}

WebCrypto::Key::Key(const Key& rhs)
    : key(rhs.key),
      pRsaContext(rhs.pRsaContext),
      pDhContext(rhs.pDhContext),
      type(rhs.type),
      extractable(rhs.extractable),
      algVar(rhs.algVar),
      keyUsage(rhs.keyUsage)
{
}

WebCrypto::Key::Key(const Variant & json)
{
    key = DataBuffer::fromBase64(json["key"].string());
    type = static_cast<KeyType>(json["type"].integer());
    extractable = json["extractable"].boolean();
    algVar = json["algorithm"];
    bool ok;
    keyUsage = json.mapValue<int>("keyUsage", &ok);
    if (!ok) {
        // This is migrating from the old style "usage" property that
        // was an array of enumns not compatible with our current ones.
        const Variant & oldUsage = json["usage"];
        if (oldUsage.type() == Variant::Type_Array) {
            const int size = oldUsage.size();
            keyUsage = 0;
            enum IWebCrypto::KeyUsage usages[] = {
                IWebCrypto::ENCRYPT,
                IWebCrypto::DECRYPT,
                IWebCrypto::SIGN,
                IWebCrypto::VERIFY,
                IWebCrypto::DERIVE,
                IWebCrypto::WRAP,
                IWebCrypto::UNWRAP
            };
            const int count = sizeof(usages) / sizeof(usages[0]);
            for (int i=0; i<size; ++i) {
                const int u = oldUsage[i].value<int>();
                if (u >= 0 && u < count)
                    keyUsage |= usages[u];
            }
        }
    }

    switch (getAlgorithmFromVar(algVar)) {
        case IWebCrypto::RSASSA_PKCS1_V1_5:
        case IWebCrypto::RSAES_PKCS1_V1_5:
        case IWebCrypto::RSA_OAEP:
            pRsaContext.reset(new RsaContext());
            if (type == IWebCrypto::PRIVATE) {
                pRsaContext->setPrivatePkcs1(key);
            } else if (type == IWebCrypto::PUBLIC) {
                pRsaContext->setPublicPkcs1(key);
            }
            break;
        default:
            break;
    }

}

WebCrypto::Key& WebCrypto::Key::operator=(const Key& rhs)
{
    if (this != &rhs)
    {
        key = rhs.key;
        pRsaContext = rhs.pRsaContext;
        pDhContext = rhs.pDhContext;
        type = rhs.type;
        extractable = rhs.extractable;
        algVar = rhs.algVar;
        keyUsage = rhs.keyUsage;
    }
    return *this;
}

Variant WebCrypto::Key::toJSON() const
{
    Variant result;
    result["key"] = key.toBase64String();
    result["type"] = type;
    result["extractable"] = extractable;
    result["algorithm"] = algVar;
    result["keyUsage"] = keyUsage;

    switch (getAlgorithmFromVar(algVar)) {
    case IWebCrypto::RSASSA_PKCS1_V1_5:
    case IWebCrypto::RSAES_PKCS1_V1_5:
    case IWebCrypto::RSA_OAEP:
        if (pRsaContext) {
            DataBuffer buf;
            if (type == IWebCrypto::PRIVATE) {
                if (!pRsaContext->getPrivatePkcs1(buf))
                    buf.clear();
            } else if (type == IWebCrypto::PUBLIC) {
                if (!pRsaContext->getPublicPkcs1(buf))
                    buf.clear();
            }
            if (!buf.isEmpty())
                result["key"] = buf.toBase64String();
        }
        break;
    default:
        break;
    }
    return result;
}
