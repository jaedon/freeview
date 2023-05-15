/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __WEBCRYPTOBRIDGE_H__
#define __WEBCRYPTOBRIDGE_H__

#include <nrd/NfObject.h>

#include <nrdbase/Variant.h>

namespace netflix {

class IWebCrypto;


class WebCryptoBridge : public NfObject
{
public:
    WebCryptoBridge();
    virtual ~WebCryptoBridge();

    void init();

    virtual unsigned requestFlags(const Request &) const;

    virtual bool getProperty(int index, /*out*/Variant *result) const;
    virtual Variant invoke(int method, const Variant &var);

protected:
    virtual void event(ApplicationEvent::Type type, const Variant &data);

private:
    // invoke handlers
    bool importKey(Variant &result, int method, const Variant &var);
    bool exportKey(Variant &result, int method, const Variant &var);
    bool getKeyInfo(Variant &result, int method, const Variant &var);
    bool digest(Variant &result, int method, const Variant &var);
    bool aesCrypt(Variant &result, int method, const Variant &var);
    bool aesUnwrap(Variant &result, int method, const Variant &var);
    bool rsaCrypt(Variant &result, int method, const Variant &var);
    bool rsaKeyGen(Variant &result, int method, const Variant &var);
    bool rsaSign(Variant &result, int method, const Variant &var);
    bool rsaVerify(Variant &result, int method, const Variant &var);
    bool hmac(Variant &result, int method, const Variant &var);
    bool hmacVerify(Variant &result, int method, const Variant &var);
    bool unwrapJwe(Variant &result, int method, const Variant &var);
    bool wrapJwe(Variant &result, int method, const Variant &var);
    bool symKeyGen(Variant &result, int method, const Variant &var);
    bool dhKeyGen(Variant &result, int method, const Variant &var);
    bool dhDerive(Variant &result, int method, const Variant &var);
    bool nflxDhDerive(Variant &result, int method, const Variant &var);
    bool eccSign(Variant &result, int method, const Variant &var);

    bool getKeyByName(Variant &result, int method, const Variant &var);
    bool persistKeys(Variant &result, int method, const Variant &var);

    bool setBinary(Variant &result, int method, const Variant &var);

    bool deleteKey(Variant &result, int method, const Variant &var);
private:
    bool getKeyInfoInternal(uint32_t key, /*out*/Variant &result) const;

    shared_ptr<IWebCrypto> m_crypto;
    bool m_binary;
};

} // namespace netflix

#endif // __WEBCRYPTOBRIDGE_H__
