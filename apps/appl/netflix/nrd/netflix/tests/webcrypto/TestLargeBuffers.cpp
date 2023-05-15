#include <nrddpi/WebCrypto.h>
#include <nrddpi/WebCryptoTee.h>
#include <nrddpi/TeeApiMgmt.h>
#include <nrddpi/TeeApiCrypto.h>
#include <nrddpi/DebugTypes.h>
#include <nrddpi/CryptoTypes.h>

using namespace netflix::device;

extern DataBuffer ncf_kpe;
extern DataBuffer ncf_kph;

#define PRINTGREEN(s)		std::cout << "\t\033[1;32m" << s << "\033[0m" << "\n";
#define PRINTRED(s)			std::cout << "\t\033[1;31m" << s << "\033[0m" << "\n";
#define PRINT(s)			std::cout << "\t" << s << "\n";
#define PRINTBOLD(s)		std::cout << "\033[1;36m" << s << "\033[0m";
#define FAIL				PRINTRED(" FAIL.");
#define PASS				PRINTGREEN(" PASS.");
#define INVALID_KEYHANDLE	0

ISystem::AuthenticationType authType = ISystem::MODEL_GROUP_KEYS;

//Uncomment if you need tee debug output.
//#define DEBUG

shared_ptr<WebCryptoTee> getTeeWebCrypto(char* manuf) {

    DataBuffer manufSS = DataBuffer::fromFile(manuf);
#ifdef DEBUG
    shared_ptr<TeeApiMgmt> teeMgmtPtr(new TeeApiMgmt(nfDEBUG_CRYPTO));
#else
    shared_ptr<TeeApiMgmt> teeMgmtPtr(new TeeApiMgmt(0x00000000));
#endif
    teeMgmtPtr->initTee(manufSS);
    shared_ptr<TeeApiCrypto> teeCryptoPtr(new TeeApiCrypto());
    shared_ptr<WebCryptoTee> teeCrypto(new WebCryptoTee(authType, teeMgmtPtr, teeCryptoPtr));

    return teeCrypto;
}

enum Tests
{
    ENCRYPT_DECRYPT,
    HMAC_SIGN_VERIFY
};

void importKeys(shared_ptr<IWebCrypto> webCrypto,
               /*out*/uint32_t &encryptDecryptKeyHandle,
               /*out*/uint32_t &hmacSignVerifyKeyHandle) {

    IWebCrypto::KeyFormat keyFormat = IWebCrypto::RAW;
    bool extractable = false;
    IWebCrypto::KeyType keyType;

    // AES-CBC Key
    DataBuffer keyData = DataBuffer::fromBase64(std::string("049PnYztO2GI73CXvBzZ6A=="));

    Variant algorithm;
    algorithm["name"] = "AES-CBC";
    unsigned int keyUsage = IWebCrypto::ENCRYPT
                          | IWebCrypto::DECRYPT;

    netflix::NFErr err = webCrypto->importKey(keyFormat, keyData, algorithm, extractable,
                                              keyUsage, encryptDecryptKeyHandle, keyType);

    if(!err.ok()) {
        FAIL;
        PRINTRED("Failed to import extractable key : " << err.toString());
        exit(EXIT_FAILURE);
    }

    //HMAC Key
    keyData = DataBuffer::fromBase64(std::string("tZ32s/dQnyPnheN6shLOA/5BLP0QYgpMWBgsXNoAG0k="));

    std::map<std::string, Variant> algVar;
    std::map<std::string, Variant> params;
    params["hash"] = "SHA-256";
    algVar["params"] = params;
    algVar["name"] = "HMAC";
    keyUsage = IWebCrypto::SIGN
             | IWebCrypto::VERIFY;

    err = webCrypto->importKey(keyFormat, keyData, algVar, extractable,
                                              keyUsage, hmacSignVerifyKeyHandle, keyType);

    if(!err.ok()) {
        FAIL;
        PRINTRED("Failed to import extractable key : " << err.toString());
        exit(EXIT_FAILURE);
    }
}

void aesCbc(shared_ptr<IWebCrypto> webCrypto, uint32_t keyHandle, const DataBuffer& ivIn,
                         const DataBuffer& dataIn, IWebCrypto::CipherOp cipherOp,
                         /*out*/DataBuffer& dataOut) {

    netflix::NFErr err = webCrypto->aesCbc(keyHandle, ivIn, dataIn,
                                            cipherOp, dataOut);

    if(!err.ok()) {
        FAIL;
        PRINTRED("Unable to encrypt : " << err.toString());
        exit(EXIT_FAILURE);
    } else {
        assert(dataOut.data() != NULL);
    }
}

void hmac(shared_ptr<IWebCrypto> webCrypto, uint32_t keyHandle, const DataBuffer &dataIn,
            /*out*/ DataBuffer &dataOut) {
    std::map<std::string, Variant> algVar;
    std::map<std::string, Variant> params;
    params["hash"] = "SHA-256";
    algVar["params"] = params;
    algVar["name"] = "HMAC";

    netflix::NFErr err = webCrypto->hmac(keyHandle, algVar, dataIn, dataOut);

    if(!err.ok()) {
        FAIL;
        PRINTRED("Failed to hmac : " << err.toString());
        exit(EXIT_FAILURE);
    } else {
        assert(dataOut.data() != NULL);
    }
}

void hmacVerify(shared_ptr<IWebCrypto> webCrypto, uint32_t DKHHandle,
                const DataBuffer &data, const DataBuffer &signature,
                /*out*/ bool &isVerified) {

    std::map<std::string, Variant> algVar;
    std::map<std::string, Variant> params;
    params["hash"] = "SHA-256";
    algVar["params"] = params;
    algVar["name"] = "HMAC";

    netflix::NFErr err = webCrypto->hmacVerify(DKHHandle, algVar, data, signature, isVerified);

    if(!err.ok()) {
        FAIL;
        PRINTRED("Failed to hmac verify : " << err.toString());
        exit(EXIT_FAILURE);
    }
}

DataBuffer getivBuf() {
    const size_t iv_len = 16;
    unsigned char iv[iv_len];
    for (size_t i = 0; i < iv_len; ++i) {
        iv[i] = i & 0xFF;
    }

    return DataBuffer(iv, iv_len);
}

int main(int argc, char** argv) {
    netflix::NFErr err;
    const DataBuffer ivBuf(getivBuf());

    if(argc < 3) {
        PRINTRED("Missing arguments\n" << "Example invocation: ./TestWebCrypto <path to manuf> <path to large file>");
        exit(EXIT_FAILURE);
    }

    shared_ptr<WebCryptoTee> teeWebCrypto = getTeeWebCrypto(argv[1]);

    uint32_t encryptDecryptKeyHandle, hmacSignVerifyKeyHandle;
    importKeys(teeWebCrypto, encryptDecryptKeyHandle, hmacSignVerifyKeyHandle);
    const DataBuffer clearData(DataBuffer::fromFile(argv[2]));

    {
        PRINTBOLD("Encrypt Decrypt Test:");

        DataBuffer encryptedData;
        DataBuffer decryptedData;

        //AES-CBC encrypt
        aesCbc(teeWebCrypto, encryptDecryptKeyHandle, ivBuf, clearData,
                IWebCrypto::DOENCRYPT, encryptedData);

        //AES-CBC decrypt
        aesCbc(teeWebCrypto, encryptDecryptKeyHandle, ivBuf, encryptedData,
                IWebCrypto::DODECRYPT, decryptedData);

        assert(clearData == decryptedData);

        PASS;
    }
    {
        PRINTBOLD("Hmac Test :");

        /**
         * --> HMAC sign
         * --> HMAC verify of the hmac sign
         */

        DataBuffer hmacData;
        bool isVerified;

        // HMAC Sign
        hmac(teeWebCrypto, hmacSignVerifyKeyHandle, clearData, hmacData);

        isVerified = false;

        // HMAC Verify
        hmacVerify(teeWebCrypto, hmacSignVerifyKeyHandle, clearData, hmacData, isVerified);
        assert(isVerified);

        PASS;
    }
}
