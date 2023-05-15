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

shared_ptr<WebCrypto> getReeWebCrypto() {

    shared_ptr<WebCrypto> reeCrypto(new WebCrypto(authType));
    ncf_kpe = DataBuffer::fromBase64(std::string("049PnYztO2GI73CXvBzZ6A=="));
    ncf_kph = DataBuffer::fromBase64(std::string("tZ32s/dQnyPnheN6shLOA/5BLP0QYgpMWBgsXNoAG0k="));
    reeCrypto->init();

    return reeCrypto;
}

shared_ptr<WebCryptoTee> getTeeWebCrypto(char* manuf) {

    DataBuffer manufSS = DataBuffer::fromFile(manuf);//"./manuf_ss.bin");
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
    IMPORT_KEY_EXTRACTABLE,
    IMPORT_KEY_NON_EXTRACTABLE,
    EXPORT_KEY_EXTRACTABLE,
    EXPORT_KEY_NON_EXTRACTABLE,
    GET_KEY_INFO,
    ENCRYPT_DECRYPT,
    GET_NAMED_KEY,
    HMAC_SIGN_VERIFY,
//	DH_KEY_GEN,					Covered in Diffie-Hellman stress test
//	NFLX_DH_DERIVE,				Covered in Diffie-Hellman stress test
//	PERSIST_KEYS,
    CLEAR_KEYS,
    DELETE_KEYS
};

void importKey(shared_ptr<IWebCrypto> webCrypto, IWebCrypto::KeyFormat keyFormat,
               const DataBuffer &keyData, bool extractable,
               /*out*/uint32_t &keyHandle, /*out*/IWebCrypto::KeyType &keyType) {

    Variant algorithm;
    algorithm["name"] = "AES-CBC";
    const unsigned int keyUsage = IWebCrypto::ENCRYPT
                                | IWebCrypto::DECRYPT;

    netflix::NFErr err = webCrypto->importKey(keyFormat, keyData, algorithm, extractable,
                                              keyUsage, keyHandle, keyType);

    if(!err.ok()) {
        FAIL;
        PRINTRED("Failed to import extractable key : " << err.toString());
        exit(EXIT_FAILURE);
    }
}

void exportKey(shared_ptr<IWebCrypto> webCrypto, uint32_t keyHandle,
               /*out*/DataBuffer &keyData,
               bool errorExpected) {

    IWebCrypto::KeyFormat format = IWebCrypto::RAW;

    // Export Key
    netflix::NFErr err = webCrypto->exportKey(keyHandle, format, /*out*/keyData);

    if(errorExpected == false) {
        if(!err.ok()) {
            FAIL;
            PRINTRED("Failed to export extractable key : " << err.toString());
            exit(EXIT_FAILURE);
        }
    }

    if(errorExpected == true) {
        if(err.ok() && errorExpected == true) {
            FAIL;
            PRINTRED("Exported non extractable key");
            PRINTRED("KeyBuffer : " << keyData.toBase64String());
            exit(EXIT_FAILURE);
        } else {
            assert(err.contains(netflix::NFErr_NotAllowed));
        }
    }
}

void getKeyInfo(shared_ptr<IWebCrypto> webCrypto, uint32_t keyHandle,
                /*out*/bool &extractable) {

    IWebCrypto::KeyType keyType;
    Variant algorithm;
    unsigned int usage;
    Variant expectAlgorithm;
    expectAlgorithm["name"] = "AES-CBC";

    // Get Key Info for extractable Key;
    netflix::NFErr err = webCrypto->getKeyInfo(keyHandle, keyType,
                                extractable, algorithm, usage);
    if(!err.ok()) {
        FAIL;
        PRINTRED("Unable to get KeyInfo for keyHandle, " << keyHandle << " : " << err.toString());
        exit(EXIT_FAILURE);
    } else {
        assert(IWebCrypto::SECRET == keyType);
        assert(algorithm == expectAlgorithm);
        assert(usage == (IWebCrypto::ENCRYPT | IWebCrypto::DECRYPT));
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

void getNamedKey(shared_ptr<IWebCrypto> webCrypto, const std::string &name,
                  /*out*/ uint32_t &keyHandle) {
    // Get Named Key
    keyHandle = INVALID_KEYHANDLE;
    netflix::NFErr err = webCrypto->getNamedKey(name, keyHandle);

    if(!err.ok()) {
        FAIL;
        PRINTRED("Unable to get named key DKE : " << err.toString());
        exit(EXIT_FAILURE);
    } else {
        assert(keyHandle != INVALID_KEYHANDLE);
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
    if(argc < 2) {
        PRINTRED("Missing arguments\n" << "Example invocation: ./TestWebCrypto <path to manuf>");
        exit(EXIT_FAILURE);
    }

    shared_ptr<WebCrypto> reeWebCrypto = getReeWebCrypto();
    shared_ptr<WebCryptoTee> teeWebCrypto = getTeeWebCrypto(argv[1]);


    DataBuffer rawKey = DataBuffer::fromBase64(std::string("049PnYztO2GI73CXvBzZ6A=="));
    uint32_t reeExtractableKeyHandle, teeExtractableKeyHandle;
    uint32_t reeNonExtractableKeyHandle, teeNonExtractableKeyHandle;
    uint32_t reeDKE, reeDKH, reeDKW;
    uint32_t teeDKE, teeDKH, teeDKW;
    DataBuffer encryptedData;
    const DataBuffer clearData(DataBuffer("Hello", 5));

    for(Tests t=IMPORT_KEY_EXTRACTABLE; t <= DELETE_KEYS; t = Tests(t+1)) {
        switch(t) {
        case IMPORT_KEY_EXTRACTABLE:
        {
            PRINTBOLD("Import Key Extractable Test :");

            IWebCrypto::KeyType keyType;

            // Import extractable Key to REE
            importKey(reeWebCrypto, IWebCrypto::RAW, rawKey, /*extractable*/true,
                        /*out*/reeExtractableKeyHandle, /*out*/keyType);
            assert(IWebCrypto::SECRET == keyType);

            // Import extractable Key to TEE
            importKey(teeWebCrypto, IWebCrypto::RAW, rawKey, /*extractable*/true,
                        /*out*/teeExtractableKeyHandle, /*out*/keyType);
            assert(IWebCrypto::SECRET == keyType);

            PASS;

            break;
        }
        case IMPORT_KEY_NON_EXTRACTABLE:
        {
            PRINTBOLD("Import Key Non Extractable Test :");

            IWebCrypto::KeyType keyType;

            // Import non extractable Key to REE
            importKey(reeWebCrypto, IWebCrypto::RAW, rawKey, /*extractable*/false,
                        /*out*/reeNonExtractableKeyHandle, /*out*/keyType);
            assert(IWebCrypto::SECRET == keyType);

            // Import non extractable Key to TEE
            importKey(teeWebCrypto, IWebCrypto::RAW, rawKey, /*extractable*/false,
                        /*out*/teeNonExtractableKeyHandle, /*out*/keyType);
            assert(IWebCrypto::SECRET == keyType);

            PASS;

            break;
        }
        case EXPORT_KEY_EXTRACTABLE:
        {
            PRINTBOLD("Export Extractable Key Test :");

            DataBuffer keyData;

            // Export extractable Key from REE
            exportKey(reeWebCrypto, reeExtractableKeyHandle, /*out*/keyData, false);
            assert(keyData.data() != NULL);

            // Export extractable Key from TEE
            exportKey(teeWebCrypto, teeExtractableKeyHandle, /*out*/keyData, false);
            assert(keyData.data() != NULL);

            PASS;

            break;
        }
        case EXPORT_KEY_NON_EXTRACTABLE:
        {
            PRINTBOLD("Export Non Extractable Key Test:");

            DataBuffer keyData;

            // Export non extractable Key from REE
            exportKey(reeWebCrypto, reeNonExtractableKeyHandle, /*out*/keyData, true);
            assert(keyData.data() == NULL);

            // Export non extractable Key from TEE
            exportKey(teeWebCrypto, teeNonExtractableKeyHandle, /*out*/keyData, true);
            assert(keyData.data() == NULL);

            PASS;

            break;
        }
        case GET_KEY_INFO:
        {
            PRINTBOLD("Get Key Info Test :");

            bool extractable;

            // Get Key Info for extractable Key from REE
            getKeyInfo(reeWebCrypto, reeExtractableKeyHandle, extractable);
            assert(extractable == true);

            // Get Key Info for extractable Key from TEE
            getKeyInfo(teeWebCrypto, teeExtractableKeyHandle, extractable);
            assert(extractable == true);

            // Get Key Info for non extractable Key from REE
            getKeyInfo(reeWebCrypto, reeNonExtractableKeyHandle, extractable);
            assert(extractable == false);

            // Get Key Info for non extractable Key from TEE
            getKeyInfo(teeWebCrypto, teeNonExtractableKeyHandle, extractable);
            assert(extractable == false);

            PASS;

            break;
        }
        case ENCRYPT_DECRYPT:
        {
            PRINTBOLD("Encrypt Decrypt Test:");

            DataBuffer decryptedData;

            //AES-CBC encrypt with REE non extractable key
            aesCbc(reeWebCrypto, reeNonExtractableKeyHandle, ivBuf, clearData,
                    IWebCrypto::DOENCRYPT, encryptedData);

            //AES-CBC decrypt with TEE extractable key
            aesCbc(teeWebCrypto, teeExtractableKeyHandle, ivBuf, encryptedData,
                    IWebCrypto::DODECRYPT, decryptedData);

            assert(clearData == decryptedData);

            //AES-CBC encrypt with TEE non extractable key
            aesCbc(teeWebCrypto, teeNonExtractableKeyHandle, ivBuf, clearData,
                    IWebCrypto::DOENCRYPT, encryptedData);

            //AES-CBC decrypt with REE extractable key
            aesCbc(reeWebCrypto, reeExtractableKeyHandle, ivBuf, encryptedData,
                    IWebCrypto::DODECRYPT, decryptedData);

            assert(clearData == decryptedData);

            PASS;

            break;
        }
        case GET_NAMED_KEY:
        {
            PRINTBOLD("Get Named Key Test :");

            getNamedKey(reeWebCrypto, nfTEE_PSK_KPE_NAME, reeDKE);
            getNamedKey(reeWebCrypto, nfTEE_PSK_KPH_NAME, reeDKH);
            getNamedKey(reeWebCrypto, nfTEE_PSK_KPW_NAME, reeDKW);
            getNamedKey(teeWebCrypto, nfTEE_PSK_KPE_NAME, teeDKE);
            getNamedKey(teeWebCrypto, nfTEE_PSK_KPH_NAME, teeDKH);
            getNamedKey(teeWebCrypto, nfTEE_PSK_KPW_NAME, teeDKW);

            PASS;

            break;
        }
        case HMAC_SIGN_VERIFY:
        {
            PRINTBOLD("Hmac Test :");

            /**
             * --> HMAC sign
             * --> HMAC verify of the hmac sign
             */

            DataBuffer hmacData;
            bool isVerified;

            // Hmac signed with REE DKH
            hmac(reeWebCrypto, reeDKH, encryptedData, hmacData);
            isVerified = false;
            // Hmac verified with TEE DKH
            hmacVerify(teeWebCrypto, teeDKH, encryptedData, hmacData, isVerified);
            assert(isVerified);

            // Hmac signed with TEE DKH
            hmac(teeWebCrypto, teeDKH, encryptedData, hmacData);
            isVerified = false;
            // Hmac verified with REE DKH
            hmacVerify(reeWebCrypto, reeDKH, encryptedData, hmacData, isVerified);
            assert(isVerified);

            PASS;

            break;
        }
        case CLEAR_KEYS:
        {
            PRINTBOLD("Clear Keys Test :");

            /**
             * --> Clear the keys
             * --> Check number of keys cleared
             */

            std::set<uint32_t> keyHandles;

            // Clear keys
            err = teeWebCrypto->clearKeys(keyHandles);

            if(!err.ok()) {
                FAIL;
                PRINTRED("Failed to clear key : " << err.toString());
                exit(EXIT_FAILURE);
            } else {
                // Expecting 2 keys to be deleted
                // Both the keys (Extractable and NonExtractable) are imported in import key test
                assert(keyHandles.size() == 2);
                PASS;
            }

            break;
        }
        case DELETE_KEYS:
        {
            PRINTBOLD("Delete Keys Test :");

            /**
             * --> Delete the key
             * --> GetKeyInfo should return NFErr_BadParameter
             */

            // Delete DKW Key
            err = teeWebCrypto->deleteKey(teeDKH);

            if(!err.ok()) {
                FAIL;
                PRINTRED("Failed to delete key : " << err.toString());
                exit(EXIT_FAILURE);
            }

            IWebCrypto::KeyType keyType;
            bool extractable;
            Variant alg;
            unsigned int usage;

            // Get KeyInfo for deleted key (DKW).
            err = teeWebCrypto->getKeyInfo(teeDKH, keyType,
                                        extractable, alg, usage);
            if(!err.ok()) {
                assert(err.contains(netflix::NFErr_BadParameter));
                PASS;
            } else {
                FAIL;
                PRINTRED("Able to get KeyInfo for deleted keyHandle : " << teeDKH);
                PRINTRED("KeyType : " << keyType);
                PRINTRED("extractable : " << extractable);
                PRINTRED("algVar : " << alg.toJSON(true));
                PRINTRED("KeyUsage : " << usage);
                exit(EXIT_FAILURE);
            }

            break;
        }
        default:
            PRINTBOLD("Test Undefined");
        }
    }
}
