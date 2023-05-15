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

void importKey(shared_ptr<IWebCrypto> webCrypto, /*out*/uint32_t &keyHandle) {

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
											  keyUsage, keyHandle, keyType);

	if(!err.ok()) {
		FAIL;
		PRINTRED("Failed to import extractable key : " << err.toString());
		exit(EXIT_FAILURE);
	}
}


int main(int argc, char** argv) {
	netflix::NFErr err;

	if(argc < 2) {
		PRINTRED("Missing arguments\n" << "Example invocation: ./TestMaxKeys <path to manuf>");
		exit(EXIT_FAILURE);
	}

	shared_ptr<WebCryptoTee> teeWebCrypto = getTeeWebCrypto(argv[1]);

	uint32_t keyHandle;
   for(int i=1; i<=64; i++) {
      importKey(teeWebCrypto, keyHandle);
      PRINT("Imported Key: count = " << i);
   }
}
