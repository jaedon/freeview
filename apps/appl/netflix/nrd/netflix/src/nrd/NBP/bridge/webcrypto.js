/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/* global nrdp */

nrdp.webcrypto = {
    _nextIdx: 1,
    _cbs: {},

    // properties
    get authenticationType() { return this._syncData.authenticationType; },

    _fn: function _fn(name, params, cb)
    {
        if (!params) params = {};
        params.idx = this._nextIdx++;
        this._cbs[params.idx] = cb;
        nrdp._invoke("webcrypto", name, params);
        return params.idx;
    },

    _handleEvent: function _handleEvent(event)
    {
        var cb, idx;

        if (event.name != "result")
        {
            return false;
        }

        idx = event.data.idx;

        if (typeof this._cbs[idx] == "function")
        {
            cb = this._cbs[idx];
            delete event.data.idx;
            cb(event.data);
        }
        delete this._cbs[idx];

        return true;
    },

    // keyformat constants: matches IWebCrypto::KeyFormat
    get RAW() { return 0; },
    get PKCS8() { return 1; },
    get SPKI() { return 2; },
    get JWK() { return 3; },

    // keyusage contants: matches IWebCrypto::KeyUsage
    get ENCRYPT() { return 0x01; },
    get DECRYPT() { return 0x02; },
    get SIGN() { return 0x04; },
    get VERIFY() { return 0x08; },
    get DERIVE() { return 0x10; },
    get WRAP() { return 0x20; },
    get UNWRAP() { return 0x40; },

    // keytype constants: matches IWebCrypto::KeyType
    get SECRET() { return 0; },
    get PUBLIC() { return 1; },
    get PRIVATE() { return 2; },

    // keywrap method: matches IWebCrypto::JweEncMethod
    get A128GCM() { return 0; },
    get A256GCM() { return 1; },

    setBinary: function setBinary(binary, cb)
    {
        return nrdp.webcrypto._fn("setBinary", {binary: binary}, cb);
    },

    // Algorithm objects
    // Valid algorithm names are: HMAC, AES-CBC, AES-GCM, AES-CTR, RSAES-PKCS1-v1_5,
    // RSASSA-PKCS1-v1_5, RSA-OAEP
    // Valid hash names are:
    // SHA-1, SHA-224, SHA-256, SHA-384, SHA-512
    // var hashparams = {
    //       hash: {
    //          name: string // hashname
    //       }
    // };
    // var rsakeyparams = {
    //       modulusLength: integer,
    //       publicExponent: string
    // };
    // var symkeyparams = {
    //       length: integer // key length in bits
    // };
    // var algorithm = {
    //       name: string, // algorithm name
    //       params: hashparams || rsakeyparams || symkeyparams
    // };

    // Key information
    // When a key is returned in a result, it will have the following properties:
    //  handle (integer)
    //  type (KeyType constants above)
    //  extractable (bool)
    //  usage (array of 0 or more KeyUsage constants above)
    //  algorithm (an algorithm object as described above)

    // import a key
    // @param data: The raw data of the key. This can either be Base64 encoded or a
    //              a DataBuffer.
    // @param keyformat: A keyformat constant (RAW, PKCS8, SPKI, JWK)
    // @param algorithm: An object describing the key algorithm
    // @param extractable: Whether or not the raw keying material may be extracted by app
    // @param keyusage: An array of constants indicating how the key may be used
    // @param cb: Continuation function(event)
    //   Event named "result" will have a property "success" indicating success.
    //   On success, result will have a "key" property with key information
    importKey: function importKey(data, keyformat, algorithm, extractable, keyusage,
                                  cb)
    {
        return nrdp.webcrypto._fn(
            "importKey", {data: data, keyformat: keyformat, algorithm: algorithm,
                          extractable: extractable, usage: keyusage}, cb);
    },

    // export a key
    // @param key: The keyhandle to export
    // @param keyformat: The keyformat to export the key to
    // @param cb: Continuation function(event)
    //   Event named "result" will have a property "success" indicating success.
    //   On success, result will have a property "data" with the raw key data
    exportKey: function exportKey(key, keyformat, cb)
    {
        return nrdp.webcrypto._fn("exportKey", {key: key, keyformat: keyformat}, cb);
    },

    // get key info
    // @param key: The keyhandle to get info on
    // @param cb: Continuation function(event)
    //   Event named "result" will have a property "success" indicating success.
    //   On success, result will have a "key" property with key information.
    getKeyInfo: function getKeyInfo(key, cb)
    {
        return nrdp.webcrypto._fn("getKeyInfo", {key: key}, cb);
    },

    // Compute message digest
    // @param data: The raw data to hash. Either Base64 encoded or a DataBuffer
    // @param hashalgorithm: The algorithm for hashing
    //                       (SHA-1, SHA-224, SHA-256, SHA-384, SHA-512)
    // @param cb: Continuation function(event)
    //   Event named "result" will have a property "success" indicating success.
    //   On success, result will have a "data" property containing the hash
    digest: function digest(data, hashalgorithm, cb)
    {
        return nrdp.webcrypto._fn(
            "digest", {data: data, algorithm: hashalgorithm}, cb);
    },

    // AES-128-CBC encrypt data
    // @param key: The keyhandle (returned from importKey) for key to encrypt with
    // @param init: Initialization vector for encryption. Either Base64 or a DataBuffer
    // @param data: The raw data to encrypt. Either Base64 encoded or a DataBuffer
    // @param cb: Continuation function(event)
    //   Event named "result" will have a property "success" indicating success.
    //   On success, result will haven a "data" property containing the encrypted
    //   buffer (Base64 encoded)
    aesEncrypt: function aesEncrypt(key, init, data, cb)
    {
        return nrdp.webcrypto._fn(
            "aesCrypt", {key: key, init: init, data: data, operation: 0}, cb);
    },

    // AES-128-CBC decrypt data
    // @param key: The keyhandle (returned from importKey) for key to decrypt with
    // @param init: Initialization vector for encryption. Either Base64 or a DataBuffer
    // @param data: The raw data to decrypt. Either Base64 encoded or a DataBuffer
    // @param cb: Continuation function(event):
    //   Event named "result" will have a property "success" indicating success.
    //   On success, result will have a "data" property containing the decrypted
    //   buffer (Base64 encoded)
    aesDecrypt: function aesDecrypt(key, init, data, cb)
    {
        return nrdp.webcrypto._fn(
            "aesCrypt", {key: key, init: init, data: data, operation: 1}, cb);
    },

    // AES unwrap - RFC 3394
    // @param data: The wrapped data
    // @param wrappedAlgorithm: The algorithm for the wrapped key
    // @param wrappingKey: The handle to the key used to unwrap
    // @param usage: The usage for the unwrapped key
    // @param cb: Continuation function(event)
    //   Event named "result" will have a property "success" indicating success.
    //   On success, result will have a "key" property with key information
    aesUnwrap: function aesUnwrap(data, wrappedAlgorithm, wrappingKey, usage, cb)
    {
        return nrdp.webcrypto._fn(
            "aesUnwrap", {data: data, algorithm: wrappedAlgorithm, key: wrappingKey,
                          usage: usage}, cb);
    },

    // Generate a symmetric key
    // This method generates a single random key and places it in the key store.
    // @param algorithm In. The algorithm used to generate the key.
    // @param extractable In. Whether the key should be marked as extractable
    // @param keyusage: An array of constants indicating how the key may be used
    // @param cb: Continuation function(event)
    //   Event named "result" will have a property "success" indicating success.
    //   On success, result will have a "key" property with key information.
    symKeyGen: function symKeyGen(algorithm, extractable, keyusage, cb)
    {
        return nrdp.webcrypto._fn(
            "symKeyGen", {algorithm: algorithm, extractable: extractable,
                          usage: keyusage}, cb);
    },

    // Generate a RSA public/private key pair
    // @param algorithm: The algorithm object describing how the key will be generated
    // @param extractable: Whether or not the raw keying material may be extracted by app
    // @param keyusage: An array of constants indicating how the key may be used
    // @param cb: Continuation function(event)
    //   Event named "result" will have a property "success" indicating success.
    //   On success, result will have "publickey" and "privatekey" properties each
    //   containing key information.
    rsaKeyGen: function rsaKeyGen(algorithm, extractable, keyusage, cb)
    {
        return nrdp.webcrypto._fn(
            "rsaKeyGen", {algorithm: algorithm, extractable: extractable,
                          usage: keyusage}, cb);
    },

    // RSAES-PKCS1-v1_5 encrypt data
    // @param key: The keyhandle (returned from rsaKeyGen) for key to encrypt with
    // @param data: The raw data to encrypt. Either Base64 encoded or a DataBuffer
    // @param cb: Continuation function(event)
    //   Event named "result" will have a property "success" indicating success.
    //   On success, result will have a "data" property containing the encrypted
    //   buffer (Base64 encoded)
    rsaEncrypt: function rsaEncrypt(key, data, cb)
    {
        return nrdp.webcrypto._fn(
            "rsaCrypt", {key: key, data: data, operation: 0}, cb);
    },

    // RSAES-PKCS1-v1_5 decrypt data
    // @param key: The keyhandle (returned from rsaKeyGen) for key to decrypt with
    // @param data: The raw data to decrypt. Either Base64 encoded or a DataBuffer
    // @param cb: Continuation function(event)
    //   Event named "result" will have a property "success" indicating success.
    //   On success, result will have a "data" property containing the decrypted
    //   buffer (Base64 encoded)
    rsaDecrypt: function rsaDecrypt(key, data, cb)
    {
        return nrdp.webcrypto._fn(
            "rsaCrypt", {key: key, data: data, operation: 1}, cb);
    },

    // RSA sign a block of data
    // @param key: The keyhandle (returned from rsaKeyGen) for key to sign with
    // @param data: The raw data to sign. Either Base64 encoded or a DataBuffer
    // @param hashalgorithm: The algorithm for hashing
    //                       (SHA-1, SHA-224, SHA-256, SHA-384, SHA-512)
    // @param cb: Continuation function(event)
    //   Event named "result" will have a property "success" indicating success.
    //   On success, result will have a "data" property containing the rsa
    //   signature (Base64 encoded)
    rsaSign: function rsaSign(key, data, hashalgorithm, cb)
    {
        return nrdp.webcrypto._fn(
            "rsaSign", {key: key, data: data, algorithm: hashalgorithm}, cb);
    },

    // RSA verify a block of data
    // @param key: The keyhandle (returned from rsaKeyGen) for key to verify with
    // @param data: The raw data to compute the signature of. Either Base64 encoded or a
    //              DataBuffer
    // @param hashalgorithm: The algorithm for hashing
    //                       (SHA-1, SHA-224, SHA-256, SHA-384, SHA-512)
    // @param signature: The signature to verify against
    // @param cb: Continuation function(event)
    //   Event named "result" will have a property "success" indicating success.
    //   On success, result will have a "verified" property which will be true if the
    //   computed signature matches the passed in signature
    rsaVerify: function rsaVerify(key, data, hashalgorithm, signature, cb)
    {
        return nrdp.webcrypto._fn(
            "rsaVerify", {key: key, data: data, algorithm: hashalgorithm,
                          signature: signature}, cb);
    },

    // Generate a Diffie-Hellman public/private key pair
    // This method computes DH public/private key pair
    // @param algorithm In. The full details about the key gen algorithm, including
    //     the prime and generator values
    // @param extractable In. Whether or not the raw key material may be exported
    // @param keyUsage In. The allowed usages of the keys
    // @param cb: Continuation function(event)
    //   Event named "result" will have a property "success" indicating success.
    //   On success, result will have "publickey" and "privatekey" properties each
    //   containing key information.
    dhKeyGen: function dhKeyGen(algorithm, extractable, keyusage, cb)
    {
        return nrdp.webcrypto._fn(
            "dhKeyGen", {algorithm: algorithm, extractable: extractable,
                         usage: keyusage}, cb);
    },

    // Derive a shared private key
    // This method computes a shared private key using a baseKey produced by
    // dhKeyGen() plus the public key from the remote peer who has previously
    // obtained the public baseKey.
    // @param basekey In. The handle of the key that started the DH exchange,
    //   produced by a call to dhKeyGen
    // @param peerkey In. The raw public key received from the remote peer, base64-encoded
    // @param algorithm In. The full details about the algorithm to be associated with
    //   the derived key
    // @param extractable In. Whether or not the raw key material of the derived
    //   key may be exported
    // @param keyUsage In. The allowed usages of the derived key
    // @param cb: Continuation function(event)
    //   Event named "result" will have a property "success" indicating success.
    //   On success, result will have a "key" property with key information.
    dhDerive: function dhDerive(basekey, peerkey, algorithm, extractable, keyusage, cb)
    {
        return nrdp.webcrypto._fn(
            "dhDerive", {basekey: basekey, peerkey: peerkey, algorithm: algorithm,
                         extractable: extractable, usage: keyusage}, cb);
    },

    nflxDhDerive: function nflxDhDerive(basekey, peerKeyData, derivationKey, cb)
    {
        return nrdp.webcrypto._fn(
            "nflxDhDerive", {basekey: basekey, peerkey: peerKeyData, derivationKey: derivationKey}, cb);
    },

    // Compute the HMAC signature of the data
    // @param key: The keyhandle for key to HMAC with
    // @param data: The raw data to HMAC. Either Base64 encoded or a DataBuffer
    // @param hashalgorithm: The algorithm for hashing
    //                       (SHA-1, SHA-224, SHA-256, SHA-384, SHA-512)
    // @param cb: Continuation function(event)
    //   Event named "result" will have a property "success" indicating success.
    //   On success, result will have a "data" property containing the
    //   HMAC (Base64 encoded)
    hmac: function hmac(key, data, hashalgorithm, cb)
    {
        return nrdp.webcrypto._fn(
            "hmac", {key: key, data: data, algorithm: hashalgorithm}, cb);
    },

    // Verify an HMAC signature
    // @param key: The keyhandle for key to HMAC with
    // @param data: The raw data to HMAC. Either Base64 encoded or a DataBuffer
    // @param hashalgorithm: The algorithm for hashing
    //                       (SHA-1, SHA-224, SHA-256, SHA-384, SHA-512)
    // @param signature: The signature to verify against
    // @param cb: Continuation function(event)
    //   Event named "result" will have a property "success" indicating success.
    //   On success, result will have a "verified" property which will be true if the
    //   computed signature matches the passed in signature
    hmacVerify: function hmacVerify(key, data, hashalgorithm, signature, cb)
    {
        return nrdp.webcrypto._fn(
            "hmacVerify", {key: key, data: data, algorithm: hashalgorithm,
                           signature: signature}, cb);
    },

    // Unwrap a JWE-wrapped key
    // @param data: The base64-encoded wrapped key formatted to the JWE spec
    // @param key: The keyhandle for key to decrypt with
    // @param algorithm In. In case the unwrapped JDK does not have the 'alg'
    //                      field inside it, use this value, otherwise ignore
    // @param extractable: Whether or not the raw keying material may be extracted by app
    // @param keyusage: An array of constants indicating how the key may be used
    // @param cb: Continuation function(event)
    //   Event named "result" will have a property "success" indicating success.
    //   On success, result will have a "key" property containing key information
    unwrapJwe: function unwrapJwe(data, key, algorithm, extractable, keyusage, cb)
    {
        return nrdp.webcrypto._fn(
            "unwrapJwe", {data: data, key: key, algorithm: algorithm,
                          extractable: extractable, usage: keyusage}, cb);
    },

    //* JWE-wrap an existing key
    // This method wraps an existing key in the keystore according to the rules
    // in draft-ietf-jose-json-web-encryption-08, using an existing wrapping key
    // also in the keystore. The result is a base-64 encoded JWE Compact
    // Serialization described above.
    // @param key In. The handle of the key to use in during the key wrap process.
    // @param keytowrap In. The handle of the key to be wrapped
    // @param algorithm In. The encryption algorithm to be applied to the
    //    Content Master Key (CMK) during the wrapping process. This must be
    //    consistent with the algorithm associated with wrappingKeyHandle.
    //    Only RSA-OAEP and AES-KW algorithms are supported.
    // @param method In. The keywrap method to wrap with.
    //    For AES-KW, only A128GCM is supported,
    //    while RSA-OAEP supports A128GCM and A256GCM.
    // @param cb: Continuation function(event)
    //   Event named "result" will have a property "success" indicating success.
    //   On success, result will have a "data" property containing the wrapped key
    wrapJwe: function wrapJwe(key, keytowrap, algorithm, method, cb)
    {
        return nrdp.webcrypto._fn(
            "wrapJwe", {key: key, keytowrap: keytowrap, algorithm: algorithm,
                        meth: method}, cb);
    },


    // ECC sign a block of data
    // @param key: The keyhandle for key to sign with
    // @param data: The raw data to sign. Either Base64 encoded or a DataBuffer
    // @param hashalgorithm: The algorithm for hashing
    //                       (SHA-1, SHA-224, SHA-256, SHA-384, SHA-512)
    // @param cb: Continuation function(event)
    //   Event named "result" will have a property "success" indicating success.
    //   On success, result will have a "data" property containing the ECC
    //   signature (Base64 encoded)
    eccSign: function eccSign(key, data, hashalgorithm, cb)
    {
        return nrdp.webcrypto._fn(
            "eccSign", {key: key, data: data, algorithm: hashalgorithm}, cb);
    },

    // Get a key by name
    // @param name. In. string
    // @param cb: Continuation function(event)
    //   Event named "result" will have a property "success" indicating success.
    //   On success, and if the key was found, result will have a "key" property
    //   containing the keyInfo (with the name) for the key. If the key was not
    //   found, success will still be true, but key will be undefined.

    getKeyByName: function getKeysByName(name, cb)
    {
        return nrdp.webcrypto._fn(
            "getKeyByName", {name: name}, cb);
    },

    // Given an array of keys, they are persisted so that they will be
    // restored later with the same handles.
    // @param keys: In. array of key objects
    // @param cb: Continuation function(event)
    //   Event named "result" will have a property "success" indicating success.

    persistKeys: function persistKeys(keys, cb)
    {
        return nrdp.webcrypto._fn(
            "persistKeys", {keys: keys}, cb);
    },

    deleteKey: function deleteKey(key, cb)
    {
        return nrdp.webcrypto._fn(
            "deleteKey", {key: key}, cb);
    }
};
