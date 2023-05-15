/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "WebCryptoBridge.h"

#include <nrdbase/Variant.h>
#include <nrd/Request.h>
#include <nrd/Response.h>

#include "ISystem.h"
#include "IWebCrypto.h"
#include "NrdApplication.h"

using namespace netflix;
using namespace netflix::device;

//////////
//
// properties
//
//////////
#define WebCryptoBridgePropertyList(Property)           \
    Property(authenticationType, NfObject::ReadOnly)

PropertyTable(WebCryptoBridge, WebCryptoBridgePropertyList, properties)

//////////
//
// methods
//
//////////
#define WebCryptoBridgeMethodList(Method)       \
    Method(aesCrypt)                            \
    Method(aesUnwrap)                           \
    Method(deleteKey)                           \
    Method(dhDerive)                            \
    Method(dhKeyGen)                            \
    Method(digest)                              \
    Method(eccSign)                             \
    Method(exportKey)                           \
    Method(getKeyByName)                        \
    Method(getKeyInfo)                          \
    Method(hmac)                                \
    Method(hmacVerify)                          \
    Method(importKey)                           \
    Method(nflxDhDerive)                        \
    Method(persistKeys)                         \
    Method(rsaCrypt)                            \
    Method(rsaKeyGen)                           \
    Method(rsaSign)                             \
    Method(rsaVerify)                           \
    Method(setBinary)                           \
    Method(symKeyGen)                           \
    Method(unwrapJwe)                           \
    Method(wrapJwe)

MethodTable(WebCryptoBridge, WebCryptoBridgeMethodList)

//////////
//
// WebCryptoBridge
//
//////////
WebCryptoBridge::WebCryptoBridge()
    : NfObject("webcrypto",
               Properties::WebCryptoBridge::properties, Properties::WebCryptoBridge::PropertyCount,
               Methods::WebCryptoBridge::methods, Methods::WebCryptoBridge::MethodCount),
      m_crypto(nrdApp()->getDeviceLib()->getWebCrypto()),
      m_binary(false)
{
}

WebCryptoBridge::~WebCryptoBridge()
{
}

void WebCryptoBridge::init()
{
    startEventListener();
}

unsigned WebCryptoBridge::requestFlags(const Request &request) const
{
    unsigned ret = Request::NoLogArgs;
    if (request.method() != Methods::WebCryptoBridge::rsaKeyGen)
        ret |= Request::Synchronous;
    return ret;
}

//////////
//
// getProperty
//
//////////
bool WebCryptoBridge::getProperty(int index, /*out*/Variant *result) const
{
    switch (index)
    {
    case Properties::WebCryptoBridge::authenticationType:
    {
        shared_ptr<netflix::device::ISystem> sys = nrdApp()->getSystem();
        switch (sys->getAuthenticationType())
        {
        case ISystem::PRESHARED_KEYS:
            *result = "PSK";
            break;
        case ISystem::WEB:
            *result = "WEB";
            break;
        case ISystem::X509:
            *result = "x509";
            break;
        case ISystem::ECC_LINK:
            *result = "ecclink";
            break;
        case ISystem::NPTICKET:
            *result = "npticket";
            break;
        case ISystem::MODEL_GROUP_KEYS:
            *result = "MGK";
            break;
        case ISystem::UNKNOWN:
            *result = "UNKNOWN";
            break;
        }
        break;
    }
    default:
        return false;
    }

    return true;
}

void WebCryptoBridge::event(ApplicationEvent::Type type, const Variant &/*data*/)
{
    if (type == ApplicationEvent::Event_UrlAboutToChange) {
        NTRACE(TRACE_CRYPTO, "Resetting key store");
        m_binary = false;
        std::set<uint32_t> cleared;
        m_crypto->clearKeys(cleared);
    }
}

//////////
//
// invoke
//
//////////
Variant WebCryptoBridge::invoke(int method, const Variant &var)
{
    // if we haven't created m_crypto yet, there's nothing we can do
    if (!m_crypto.get())
        return NfObject::invoke(method, var);

    Variant result;

    // make sure we pass back the index of the caller
    const std::string index  = var.mapValue<std::string>("idx");
    result["idx"] = index;
    result["success"] = false; // assume failure for now

    switch (method)
    {
#define FORWARD_METHOD(METHODNAME, ...)                                 \
        case Methods::WebCryptoBridge::METHODNAME: return METHODNAME(result, method,var);

        // Forward all the methods to the same named function
        WebCryptoBridgeMethodList(FORWARD_METHOD)
            }

    // should never get here, but pass to default handler
    return NfObject::invoke(method, var);
}

#define READ_DATA(name)                                                 \
    DataBuffer name;                                                    \
    do {                                                                \
        const Variant::StringMapIterator it = var.find(#name);          \
        if (it == var.stringMapEnd()) {                                 \
            invalidArgumentError(method, #name);                        \
            sendEvent("result", result);                                \
            return false;                                               \
        }  else if (it->second.type() == Variant::Type_DataBuffer) {    \
            name = it->second.dataBuffer();                             \
        } else if (it->second.type() == Variant::Type_String) {         \
            name = DataBuffer::fromBase64(it->second.string());         \
        } else {                                                        \
            invalidArgumentError(method, #name);                        \
            sendEvent("result", result);                                \
            return false;                                               \
        }                                                               \
    } while (0)

#define READ_VAR(type, name)                    \
    var.mapValue<type>(name, &ok);              \
    do {                                        \
        if (!ok) {                              \
            invalidArgumentError(method, name); \
            sendEvent("result", result);        \
            return false;                       \
        }                                       \
    } while (0)

#define READ_VARIANT(name)                      \
    var.value(name, &ok);                       \
    do {                                        \
        if (!ok) {                              \
            invalidArgumentError(method, name); \
            sendEvent("result", result);        \
            return false;                       \
        }                                       \
    } while (0)

#define RETURN_BINARY_DATA(db)                      \
    do {                                            \
        if(m_binary) {                              \
            result["data"] = db;                    \
        } else {                                    \
            result["data"] = db.toBase64String();   \
        } \
    } while(0)

//////////
//
// setBinary
//
//////////

bool WebCryptoBridge::setBinary(Variant &result, int /*method*/, const Variant &var)
{
    const bool old = m_binary;
    m_binary = var.mapValue<bool>("binary");
    result["success"] = true;
    sendEvent("result", result);
    NTRACE(TRACE_CRYPTO, "Binary was %d and is now %d", old, m_binary);
    (void)old;
    return true;
}

//////////
//
// importKey
//
//////////
bool WebCryptoBridge::importKey(Variant &result, int method, const Variant &var)
{
    bool ok;

    // get params
    READ_DATA(data);
    const IWebCrypto::KeyFormat format =
        (IWebCrypto::KeyFormat)READ_VAR(uint32_t, "keyformat");
    const Variant algorithm = READ_VARIANT("algorithm");
    const bool extractable = READ_VAR(bool, "extractable");

    // usage should be an array or an bit mask
    const Variant &vusage = var["usage"];
    if (!vusage.isArray() && !vusage.isNumber())
    {
        invalidArgumentError(method, "usage");
        sendEvent("result", result);
        return false;
    }
    const unsigned int keyUsage = IWebCrypto::convertKeyUsage(vusage);

    // import the key
    uint32_t keyHandle;
    IWebCrypto::KeyType keyType;
    NFErr err = m_crypto->importKey(format, data, algorithm, extractable, keyUsage,
                                    /*out*/keyHandle, /*out*/keyType);
    if (err.ok())
    {
        // return the key
        Variant keyInfo;
        getKeyInfoInternal(keyHandle, keyInfo);
        result["key"] = keyInfo;
        result["success"] = true;
    }
    else
    {
        NTRACE(TRACE_CRYPTO, "importKey failed with %s",
               err.toString().c_str());
    }

    sendEvent("result", result);

    return true;
}

//////////
//
// exportKey
//
//////////
bool WebCryptoBridge::exportKey(Variant &result, int method, const Variant &var)
{
    bool ok;

    // get params
    const uint32_t key = READ_VAR(uint32_t, "key");
    const IWebCrypto::KeyFormat format =
        (IWebCrypto::KeyFormat)READ_VAR(uint32_t, "keyformat");

    // export the key
    DataBuffer output;
    NFErr err = m_crypto->exportKey(key, format, /*out*/output);
    NRDP_OBJECTCOUNT_DESCRIBE(output, "EXPORTKEY");
    if (err.ok())
    {
        // return the key data
        RETURN_BINARY_DATA(output);
        result["success"] = true;
    }
    else
    {
        NTRACE(TRACE_CRYPTO, "exportKey failed with %s",
               err.toString().c_str());
    }

    sendEvent("result", result);

    return true;
}

//////////
//
// getKeyInfoInternal
//
//////////
bool WebCryptoBridge::getKeyInfoInternal(uint32_t key, /*out*/Variant & result) const
{
    // get info
    IWebCrypto::KeyType keyType;
    bool extractable;
    Variant algoVar;
    unsigned int keyUsage = 0;
    NFErr err = m_crypto->getKeyInfo(key, /*out*/keyType, /*out*/extractable,
                                     /*out*/algoVar, /*out*/keyUsage);
    if (err.ok())
    {
        // return the key info
        result["handle"] = key;
        result["type"] = keyType;
        result["extractable"] = extractable;
        result["algorithm"] = algoVar;

        // convert usage to an array
        result["usage"] = IWebCrypto::convertKeyUsage(keyUsage);
        result["keyUsage"] = keyUsage;
        return true;
    }
    NTRACE(TRACE_CRYPTO, "getKeyInfo failed with %s",
           err.toString().c_str());
    return false;
}

//////////
//
// getKeyInfo
//
//////////
bool WebCryptoBridge::getKeyInfo(Variant &result, int method, const Variant &var)
{
    bool ok;

    // get params
    const uint32_t key = READ_VAR(uint32_t, "key");
    Variant keyInfo;
    if (getKeyInfoInternal(key, keyInfo))
    {
        result["key"] = keyInfo;
        result["success"] = true;
    }
    else
    {
        result["success" ] = false;
    }
    sendEvent("result", result);

    return true;
}

//////////
//
// digest
//
//////////
bool WebCryptoBridge::digest(Variant &result, int method, const Variant &var)
{
    bool ok;

    // get params
    READ_DATA(data);
    const Variant algorithm = READ_VARIANT("algorithm");

    // compute the digest
    DataBuffer output;
    NFErr err = m_crypto->digest(algorithm, data, /*out*/output);
    NRDP_OBJECTCOUNT_DESCRIBE(output, "DIGEST");
    if (err.ok())
    {
        // return the digest
        RETURN_BINARY_DATA(output);
        result["success"] = true;
    }
    else
    {
        NTRACE(TRACE_CRYPTO, "digest failed with %s",
               err.toString().c_str());
    }

    sendEvent("result", result);

    return true;
}

//////////
//
// aesCrypt
//
//////////
bool WebCryptoBridge::aesCrypt(Variant &result, int method, const Variant &var)
{
    bool ok;

    // get params
    const uint32_t key = READ_VAR(uint32_t, "key");
    READ_DATA(init);
    READ_DATA(data);
    const IWebCrypto::CipherOp op =
        (IWebCrypto::CipherOp)READ_VAR(uint32_t, "operation");

    // do the operation
    DataBuffer output;
    NFErr err = m_crypto->aesCbc(key, init, data, op, /*out*/output);
    NRDP_OBJECTCOUNT_DESCRIBE(output, "AESCBC");
    if (err.ok())
    {
        // return the output
        RETURN_BINARY_DATA(output);
        result["success"] = true;
    }
    else
    {
        NTRACE(TRACE_CRYPTO, "aesCrypt failed with %s",
               err.toString().c_str());
    }

    sendEvent("result", result);

    return true;
}

//////////
//
// aesUnwrap
//
//////////

bool WebCryptoBridge::aesUnwrap(Variant &result, int method, const Variant &var)
{
    bool ok;

    READ_DATA(data);
    const Variant algorithm = READ_VARIANT("algorithm");
    const uint32_t key = READ_VAR(uint32_t, "key");

    // usage should be an array or an bit mask
    const Variant &vusage = var["usage"];
    if (!vusage.isArray() && !vusage.isNumber())
    {
        invalidArgumentError(method, "usage");
        sendEvent("result", result);
        return false;
    }
    const unsigned int keyUsage = IWebCrypto::convertKeyUsage(vusage);

    uint32_t keyHandle;
    NFErr err = m_crypto->aesUnwrap(data, algorithm, key, keyUsage,
                                    /*out*/keyHandle);

    if (err.ok())
    {
        // return the key
        Variant keyInfo;
        getKeyInfoInternal(keyHandle, keyInfo);
        result["key"] = keyInfo;
        result["success"] = true;
    }
    else
    {
        NTRACE(TRACE_CRYPTO, "aesUnwrap failed with %s",
               err.toString().c_str());
    }

    sendEvent("result", result);

    return true;
}


//////////
//
// rsaCrypt
//
//////////
bool WebCryptoBridge::rsaCrypt(Variant &result, int method, const Variant &var)
{
    bool ok;

    // get params
    const uint32_t key = READ_VAR(uint32_t, "key");
    READ_DATA(data);
    const IWebCrypto::CipherOp op =
        (IWebCrypto::CipherOp)READ_VAR(uint32_t, "operation");

    // do the operation
    DataBuffer output;
    NFErr err = m_crypto->rsaCrypt(key, data, op, /*out*/output);
    NRDP_OBJECTCOUNT_DESCRIBE(output, "RSACRYPT");
    if (err.ok())
    {
        // return the output
        RETURN_BINARY_DATA(output);
        result["success"] = true;
    }
    else
    {
        NTRACE(TRACE_CRYPTO, "rsaCrypt failed with %s",
               err.toString().c_str());
    }

    sendEvent("result", result);

    return true;
}

//////////
//
// rsaKeyGen
//
//////////
bool WebCryptoBridge::rsaKeyGen(Variant &result, int method, const Variant &var)
{
    bool ok;

    // get params
    const Variant algorithm = READ_VARIANT("algorithm");
    const bool extractable = READ_VAR(bool, "extractable");

    // usage should be an array or an bit mask
    const Variant &vusage = var["usage"];
    if (!vusage.isArray() && !vusage.isNumber())
    {
        invalidArgumentError(method, "usage");
        sendEvent("result", result);
        return false;
    }
    const unsigned int keyUsage = IWebCrypto::convertKeyUsage(vusage);

    // generate the key
    uint32_t pubKey;
    uint32_t privKey;
    NFErr err = m_crypto->rsaKeyGen(algorithm, extractable, keyUsage,
                                    /*out*/pubKey, /*out*/privKey);
    if (err.ok())
    {
        // return the output
        Variant pubKeyInfo;
        Variant privKeyInfo;
        getKeyInfoInternal(pubKey, pubKeyInfo);
        getKeyInfoInternal(privKey, privKeyInfo);
        result["publickey"] = pubKeyInfo;
        result["privatekey"] = privKeyInfo;
        result["success"] = true;
    }
    else
    {
        NTRACE(TRACE_CRYPTO, "rsaKeyGen failed with %s",
               err.toString().c_str());
    }

    sendEvent("result", result);

    return true;
}

//////////
//
// rsaSign
//
//////////
bool WebCryptoBridge::rsaSign(Variant &result, int method, const Variant &var)
{
    bool ok;

    // get params
    const uint32_t key = READ_VAR(uint32_t, "key");
    READ_DATA(data);
    const Variant algorithm = READ_VARIANT("algorithm");

    // do the hmac
    DataBuffer output;
    NFErr err = m_crypto->rsaSign(key, algorithm, data, /*out*/output);
    NRDP_OBJECTCOUNT_DESCRIBE(output, "RSASIGN");
    if (err.ok())
    {
        // return the signature
        RETURN_BINARY_DATA(output);
        result["success"] = true;
    }
    else
    {
        NTRACE(TRACE_CRYPTO, "rsaSign failed with %s",
               err.toString().c_str());
    }

    sendEvent("result", result);

    return true;
}

//////////
//
// eccSign
//
//////////
bool WebCryptoBridge::eccSign(Variant &result, int method, const Variant &var)
{
    bool ok;

    // get params
    const uint32_t key = READ_VAR(uint32_t, "key");
    READ_DATA(data);
    const Variant algorithm = READ_VARIANT("algorithm");

    DataBuffer output;
    NFErr err = m_crypto->eccSign(key, algorithm, data, /*out*/output);
    NRDP_OBJECTCOUNT_DESCRIBE(output, "ECCSIGN");
    if (err.ok())
    {
        // return the signature
        RETURN_BINARY_DATA(output);
        result["success"] = true;
    }
    else
    {
        NTRACE(TRACE_CRYPTO, "eccSign failed with %s",
               err.toString().c_str());
    }

    sendEvent("result", result);

    return true;
}

//////////
//
// rsaVerify
//
//////////
bool WebCryptoBridge::rsaVerify(Variant &result, int method, const Variant &var)
{
    bool ok;

    // get params
    const uint32_t key = READ_VAR(uint32_t, "key");
    READ_DATA(data);
    const Variant algorithm = READ_VARIANT("algorithm");
    READ_DATA(signature);

    // do the hmac
    bool verified = false;
    NFErr err = m_crypto->rsaVerify(key, algorithm, data, signature, /*out*/verified);
    if (err.ok())
    {
        // return the signature
        result["verified"] = verified;
        result["success"] = true;
    }
    else
    {
        NTRACE(TRACE_CRYPTO, "rsaVerify failed with %s",
               err.toString().c_str());
    }

    sendEvent("result", result);

    return true;
}

//////////
//
// hmac
//
//////////
bool WebCryptoBridge::hmac(Variant &result, int method, const Variant &var)
{
    bool ok;

    // get params
    const uint32_t key = READ_VAR(uint32_t, "key");
    READ_DATA(data);
    const Variant algorithm = READ_VARIANT("algorithm");

    // do the hmac
    DataBuffer output;
    NFErr err = m_crypto->hmac(key, algorithm, data, /*out*/output);
    NRDP_OBJECTCOUNT_DESCRIBE(output, "HMAC");
    if (err.ok())
    {
        // return the hmac
        RETURN_BINARY_DATA(output);
        result["success"] = true;
    }
    else
    {
        NTRACE(TRACE_CRYPTO, "hmac failed with %s",
               err.toString().c_str());
    }

    sendEvent("result", result);

    return true;
}

//////////
//
// hmacVerify
//
//////////
bool WebCryptoBridge::hmacVerify(Variant &result, int method, const Variant &var)
{
    bool ok;

    // get params
    const uint32_t key = READ_VAR(uint32_t, "key");
    READ_DATA(data);
    const Variant algorithm = READ_VARIANT("algorithm");
    READ_DATA(signature);

    // do the hmac
    bool verified = false;
    NFErr err = m_crypto->hmacVerify(key, algorithm, data, signature, /*out*/verified);
    if (err.ok())
    {
        // verify the signature
        result["verified"] = verified;
        result["success"] = true;
    }
    else
    {
        NTRACE(TRACE_CRYPTO, "hmacVerify failed with %s",
               err.toString().c_str());
    }

    sendEvent("result", result);

    return true;
}

//////////
//
// unwrapJwe
//
//////////
bool WebCryptoBridge::unwrapJwe(Variant &result, int method, const Variant &var)
{
    bool ok;

    // get params
    READ_DATA(data);
    const uint32_t handle = READ_VAR(uint32_t, "key");
    const Variant algorithm = READ_VARIANT("algorithm");
    const bool extractable = READ_VAR(bool, "extractable");

    // usage should be an array or an bit mask
    const Variant &vusage = var["usage"];
    if (!vusage.isArray() && !vusage.isNumber())
    {
        invalidArgumentError(method, "usage");
        sendEvent("result", result);
        return false;
    }
    const unsigned int keyUsage = IWebCrypto::convertKeyUsage(vusage);

    // unwrap the key
    uint32_t key;
    NFErr err = m_crypto->unwrapJwe(data, handle, algorithm, extractable, keyUsage,
                                    /*out*/key);
    if (err.ok())
    {
        // return the key
        Variant keyInfo;
        getKeyInfoInternal(key, keyInfo);
        result["key"] = keyInfo;
        result["success"] = true;
    }
    else
    {
        NTRACE(TRACE_CRYPTO, "unwrapJwe failed with %s",
               err.toString().c_str());
    }

    sendEvent("result", result);

    return true;
}

//////////
//
// wrapJwe
//
//////////
bool WebCryptoBridge::wrapJwe(Variant &result, int method, const Variant &var)
{
    bool ok;

    // get params
    const uint32_t key = READ_VAR(uint32_t, "key");
    const uint32_t keyToWrap = READ_VAR(uint32_t, "keytowrap");
    const Variant algorithm = READ_VARIANT("algorithm");
    const IWebCrypto::JweEncMethod jwemethod =
        (IWebCrypto::JweEncMethod)READ_VAR(uint32_t, "meth");

    // wrap the key
    DataBuffer output;
    NFErr err = m_crypto->wrapJwe(keyToWrap, key, algorithm, jwemethod, /*out*/output);
    NRDP_OBJECTCOUNT_DESCRIBE(output, "WRAPJWE");
    if (err.ok())
    {
        // return the wrapped data
        Variant keyInfo;
        RETURN_BINARY_DATA(output);
        result["success"] = true;
    }
    else
    {
        NTRACE(TRACE_CRYPTO, "wrapJwe failed with %s",
               err.toString().c_str());
    }

    sendEvent("result", result);

    return true;
}

//////////
//
// symKeyGen
//
//////////
bool WebCryptoBridge::symKeyGen(Variant &result, int method, const Variant &var)
{
    bool ok;

    // get params
    const Variant algorithm = READ_VARIANT("algorithm");
    const bool extractable = READ_VAR(bool, "extractable");

    // usage should be an array or an bit mask
    const Variant &vusage = var["usage"];
    if (!vusage.isArray() && !vusage.isNumber())
    {
        invalidArgumentError(method, "usage");
        sendEvent("result", result);
        return false;
    }
    const unsigned int keyUsage = IWebCrypto::convertKeyUsage(vusage);

    // generate key
    uint32_t key;
    NFErr err = m_crypto->symKeyGen(algorithm, extractable, keyUsage, /*out*/key);
    if (err.ok())
    {
        // return the key
        Variant keyInfo;
        getKeyInfoInternal(key, keyInfo);
        result["key"] = keyInfo;
        result["success"] = true;
    }
    else
    {
        NTRACE(TRACE_CRYPTO, "symKeyGen failed with %s",
               err.toString().c_str());
    }

    sendEvent("result", result);

    return true;
}

//////////
//
// dhKeyGen
//
//////////
bool WebCryptoBridge::dhKeyGen(Variant &result, int method, const Variant &var)
{
    bool ok;

    // get params
    const Variant algorithm = READ_VARIANT("algorithm");
    const bool extractable = READ_VAR(bool, "extractable");

    // usage should be an array or an bit mask
    const Variant &vusage = var["usage"];
    if (!vusage.isArray() && !vusage.isNumber())
    {
        invalidArgumentError(method, "usage");
        sendEvent("result", result);
        return false;
    }
    const unsigned int keyUsage = IWebCrypto::convertKeyUsage(vusage);

    // generate key
    uint32_t pubKey;
    uint32_t privKey;
    NFErr err = m_crypto->dhKeyGen(algorithm, extractable, keyUsage,
                                   /*out*/pubKey, /*out*/privKey);
    if (err.ok())
    {
        // return the keys
        Variant pubKeyInfo;
        Variant privKeyInfo;
        getKeyInfoInternal(pubKey, pubKeyInfo);
        getKeyInfoInternal(privKey, privKeyInfo);
        result["publickey"] = pubKeyInfo;
        result["privatekey"] = privKeyInfo;
        result["success"] = true;
    }
    else
    {
        NTRACE(TRACE_CRYPTO, "dhKeyGen failed with %s",
               err.toString().c_str());
    }

    sendEvent("result", result);

    return true;
}

//////////
//
// dhDerive
//
//////////
bool WebCryptoBridge::dhDerive(Variant &result, int method, const Variant &var)
{
    bool ok;

    // get params
    const uint32_t baseKey = READ_VAR(uint32_t, "basekey");
    READ_DATA(peerkey);
    const Variant algorithm = READ_VARIANT("algorithm");
    const bool extractable = READ_VAR(bool, "extractable");

    // usage should be an array or an bit mask
    const Variant &vusage = var["usage"];
    if (!vusage.isArray() && !vusage.isNumber())
    {
        invalidArgumentError(method, "usage");
        sendEvent("result", result);
        return false;
    }
    const unsigned int keyUsage = IWebCrypto::convertKeyUsage(vusage);

    // generate key
    uint32_t key;
    NFErr err = m_crypto->dhDerive(baseKey, peerkey, algorithm, extractable, keyUsage,
                                   /*out*/key);
    if (err.ok())
    {
        // return the key
        Variant keyInfo;
        getKeyInfoInternal(key, keyInfo);
        result["key"] = keyInfo;
        result["success"] = true;
    }
    else
    {
        NTRACE(TRACE_CRYPTO, "dhDerive failed with %s",
               err.toString().c_str());
    }

    sendEvent("result", result);

    return true;
}

//////////
//
// nflxDhDerive
//
//////////
bool WebCryptoBridge::nflxDhDerive(Variant &result, int method, const Variant &var)
{
    bool ok;

    // get params
    const uint32_t baseKey = READ_VAR(uint32_t, "basekey");
    READ_DATA(peerkey);
    const uint32_t derivationKey = READ_VAR(uint32_t, "derivationKey");

    // generate keys
    uint32_t encryptionKey;
    uint32_t hmacKey;
    uint32_t wrappingKey;

    NFErr err = m_crypto->nflxDhDerive(baseKey, peerkey, derivationKey,
                                   /*out*/encryptionKey,
                                   /*out*/hmacKey,
                                   /*out*/wrappingKey);
    if (err.ok())
    {
        // return the key
        Variant ekinfo;
        getKeyInfoInternal(encryptionKey, ekinfo);
        result["encryptionKey"] = ekinfo;
        Variant hkinfo;
        getKeyInfoInternal(hmacKey, hkinfo);
        result["hmacKey"] = hkinfo;
        Variant wkinfo;
        getKeyInfoInternal(wrappingKey, wkinfo);
        result["wrappingKey"] = wkinfo;
        result["success"] = true;
    }
    else
    {
        NTRACE(TRACE_CRYPTO, "nflxDhDerive failed with %s",
               err.toString().c_str());
    }

    sendEvent("result", result);

    return true;
}

//////////
//
// getKeyByName
//
//////////

bool WebCryptoBridge::getKeyByName(Variant &result, int method, const Variant &var)
{
    bool ok;

    const std::string name = READ_VAR(std::string, "name");

    uint32_t handle;

    NFErr err = m_crypto->getNamedKey(name, handle);

    if (err == NFErr_NotFound)
    {
        result["success"] = false;
    }
    else if (err.ok())
    {
        Variant keyInfo;
        getKeyInfoInternal(handle, keyInfo);
        keyInfo["name"] = name;
        result["key"] = keyInfo;
        result["success"] = true;
    }
    else
    {
        NTRACE(TRACE_CRYPTO, "getKeysByName failed with %s",
               err.toString().c_str());
    }

    sendEvent("result", result);

    return true;
}


bool WebCryptoBridge::persistKeys(Variant &result, int method, const Variant &var)
{
    bool ok;

    const Variant keyArray = READ_VARIANT("keys");

    if (!keyArray.isArray())
    {
        invalidArgumentError(method, "keys");
        sendEvent("result", result);
        return false;
    }

    const std::vector<Variant> keys(keyArray.array());
    std::vector<uint32_t> handles;

    for (Variant::ArrayIterator it = keys.begin(); it != keys.end(); ++it)
    {
        uint32_t handle = it->mapValue("handle", &ok, 0);
        if (!ok || !handle)
        {
            invalidArgumentError(method, "keys");
            sendEvent("result", result);
            return false;
        }
        handles.push_back(handle);
    }

    NFErr err = m_crypto->persistKeys(handles);

    if (err.ok())
    {
        result["success"] = true;
    }
    else
    {
        NTRACE(TRACE_CRYPTO, "persistKeys failed with %s",
               err.toString().c_str());
    }

    sendEvent("result", result);

    return true;
}

bool WebCryptoBridge::deleteKey(Variant &result, int method, const Variant &var)
{
    bool ok;

    // get params
    const uint32_t key = READ_VAR(uint32_t, "key");

    NFErr err = m_crypto->deleteKey(key);

    if (err.ok())
    {
        result["success"] = true;
    }
    else
    {
        NTRACE(TRACE_CRYPTO, "deleteKey failed with: %s",
               err.toString().c_str());
    }
    sendEvent("result", result);

    return true;
}


#undef READ_VAR
#undef READ_VARIANT
#undef FORWARD_METHOD
