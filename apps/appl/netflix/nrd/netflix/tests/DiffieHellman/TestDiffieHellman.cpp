#include <nrddpi/WebCrypto.h>
#include <nrddpi/WebCryptoTee.h>
#include <nrddpi/TeeApiMgmt.h>
#include <nrddpi/TeeApiCrypto.h>
#include <nrddpi/DebugTypes.h>

using namespace netflix::crypto;
using namespace netflix::device;
using namespace netflix::base;

#define PREFIX(isTee)           ((isTee == true) ? "TEE" : "REE")
#define REE                     false
#define TEE                     true

ISystem::AuthenticationType authType = ISystem::MODEL_GROUP_KEYS;
extern DataBuffer ncf_kpe;
extern DataBuffer ncf_kph;

uint8_t prime[] = {
   0x96,0x94,0xe9,0xd8, 0xd9,0x3a,0x5a,0xc7, 0x4c,0x50,0x9b,0x4b, 0xbc,0xe8,0x5e,0x92,
   0x13,0x2c,0xd1,0x9c, 0xce,0x47,0x7d,0x1a, 0x7e,0x47,0xd5,0x27, 0xd9,0xec,0x29,0x15,
   0x15,0xf0,0xb8,0xb3, 0xe1,0xea,0xed,0x50, 0x06,0xe1,0xb1,0xb9, 0x1e,0xa2,0x5b,0x91,
   0xa0,0x1b,0x10,0xe2, 0xe8,0x34,0xb8,0xd6, 0x60,0xb2,0xe3,0x21, 0xad,0x64,0x4c,0xe1,
   0xa8,0x3b,0x32,0x8d, 0x90,0x14,0xee,0x7e, 0x16,0xf1,0xe4,0x4f, 0xfe,0x89,0x57,0x9a,
   0xc3,0xee,0x47,0xd6, 0x68,0xb6,0xb7,0x66, 0x87,0xc2,0xfe,0x90, 0xa3,0x5b,0x5e,0x60,
   0x28,0xfd,0x04,0xef, 0xea,0x88,0x23,0x73, 0xec,0xf6,0x0b,0xa2, 0xf6,0x37,0xe4,0xcd,
   0xaa,0x1b,0x60,0x89, 0xd6,0xc0,0xb5,0x61, 0xa8,0xe5,0x20,0xe7, 0x96,0xde,0x27,0xdf
};

uint8_t generator[] = { 0x5 };

shared_ptr<WebCrypto> getReeWebCrypto() {

   shared_ptr<WebCrypto> reeCrypto(new WebCrypto(authType));
   ncf_kpe = DataBuffer::fromBase64(std::string("049PnYztO2GI73CXvBzZ6A=="));
   ncf_kph = DataBuffer::fromBase64(std::string("tZ32s/dQnyPnheN6shLOA/5BLP0QYgpMWBgsXNoAG0k="));
   reeCrypto->init();

   return reeCrypto;
}

shared_ptr<WebCryptoTee> getTeeWebCrypto(char *manuf) {
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

std::map<std::string, Variant> getAlgVar() {
   std::map<std::string, Variant> algVar;
   algVar["name"] = "NFLX-DH";
   std::map<std::string, Variant> params;
   DataBuffer p = DataBuffer(prime, sizeof(prime));
   params["prime"] = p;
   DataBuffer g = DataBuffer(generator, sizeof(generator));
   params["generator"] = g;
   algVar["params"] = params;

   return algVar;
}

void dhKeyGen(bool isTee, shared_ptr<IWebCrypto> webCrypto, uint32_t& pubKey, uint32_t& privKey, DataBuffer& pubKeyData, uint32_t& dkw) {

   netflix::NFErr err;
   bool extractable = false;
   unsigned int keyUsage = IWebCrypto::DERIVE;
   std::map<std::string, Variant> algVar = getAlgVar();

   // This is never supposed to happen.
   if(pubKeyData.size() > 128) {
      std::cout << "<" << PREFIX(isTee) << "> Public Key Size > 128, pubKeyData.size() = " << pubKeyData.size() << "\n";
      exit(1);
   }

   //generate key
   err = webCrypto->dhKeyGen(algVar, extractable, keyUsage,
                             /*out*/pubKey, /*out*/privKey);
   if(!err.ok()) {
      std::cout << "<" << PREFIX(isTee) <<  "> DhKeyGen : " << err.peekCode() << "\n";
      exit(1);
   }

   err = webCrypto->exportKey(pubKey, IWebCrypto::RAW, pubKeyData);

   if(!err.ok()) {
      std::cout << "<" << PREFIX(isTee) <<  "> Export PubKey : " << err.peekCode() << "\n";
      exit(1);
   }

   // TODO: remove this workaround after NRDP-3554 is fixed.
   if(pubKeyData.size() < 128) {
      //std::cout << "<" << PREFIX(isTee) <<  "> PubKey Length : " <<  pubKeyData.size() << std::endl;
      int paddingLength = 128-pubKeyData.size();
      DataBuffer temp(128);
      temp.replace(0, paddingLength, DataBuffer(paddingLength, 0));
      temp.replace(paddingLength, pubKeyData.size(), pubKeyData);
      pubKeyData = temp;
   }

   err = webCrypto->getNamedKey("DKW", dkw);
   if(!err.ok()) {
      std::cout << "<" << PREFIX(isTee) <<  "> REE getNamedKey: " << err.peekCode() << "\n";
      exit(1);
   }
}

void nflxDhDerive(bool isTee, shared_ptr<IWebCrypto> webCrypto, uint32_t privKey, uint32_t dkw, const DataBuffer& peerPubKeyData,
                  uint32_t& encryptionKeyHandle, uint32_t& hmacKeyHandle, uint32_t& wrappingKeyHandle) {

   netflix::NFErr err;
   err = webCrypto->nflxDhDerive(privKey, peerPubKeyData, dkw,
                                 encryptionKeyHandle, hmacKeyHandle, wrappingKeyHandle);
   if(!err.ok()) {
      std::cout << "<" << PREFIX(isTee) <<  "> NFLXDHDerive : " << err.peekCode() << "\n";
      exit(1);
   }
}

void aesCbc(bool isTee, shared_ptr<IWebCrypto> webCrypto, uint32_t keyHandle, const DataBuffer& ivInBuf, const DataBuffer& dataInBuf, IWebCrypto::CipherOp cipherOp, DataBuffer& dataOutBuf) {

   netflix::NFErr err;
   err = webCrypto->aesCbc(keyHandle, ivInBuf, dataInBuf, cipherOp, dataOutBuf);
   if(!err.ok()) {
      std::cout << "<" << PREFIX(isTee) <<  "> AES-CBC : " << err.peekCode() << "\n";
      exit(1);
   }
}

void deleteKey(bool isTee, shared_ptr<IWebCrypto> webCrypto, uint32_t keyHandle) {
   netflix::NFErr err;
   err = webCrypto->deleteKey(keyHandle);
   if(!err.ok()) {
      std::cout << "<" << PREFIX(isTee) <<  "> DeleteKey : " << err.peekCode() << "\n";
      exit(1);
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
      std::cout << "Missing arguments\n" << "Example invocation: TestDiffieHellman <path to manuf>";
      exit(EXIT_FAILURE);
   }

   shared_ptr<WebCrypto> reeWebCrypto = getReeWebCrypto();
   shared_ptr<WebCryptoTee> teeWebCrypto = getTeeWebCrypto(argv[1]);

   //REE generate key
   uint32_t reePubKey;
   uint32_t reePrivKey;
   DataBuffer reePubkeyData;
   uint32_t reeDKW;

   //TEE generate key
   uint32_t teePubKey;
   uint32_t teePrivKey;
   DataBuffer teePubkeyData;
   uint32_t teeDKW;

   uint32_t reeEncryptionKeyHandle;
   uint32_t reeHmacKeyHandle;
   uint32_t reeWrappingKeyHandle;
   uint32_t teeEncryptionKeyHandle;
   uint32_t teeHmacKeyHandle;
   uint32_t teeWrappingKeyHandle;

   int numOfIterations = 0;
   while(numOfIterations++ < 10000) {
      std::cout << numOfIterations << "\n";

      dhKeyGen(REE, reeWebCrypto, reePubKey, reePrivKey, reePubkeyData, reeDKW);
      dhKeyGen(TEE, teeWebCrypto, teePubKey, teePrivKey, teePubkeyData, teeDKW);

      //REE Derive Key
      nflxDhDerive(REE, reeWebCrypto, reePrivKey, reeDKW, teePubkeyData,
                   reeEncryptionKeyHandle, reeHmacKeyHandle, reeWrappingKeyHandle);

      //TEE Derive Key
      nflxDhDerive(TEE, teeWebCrypto, teePrivKey, teeDKW, reePubkeyData,
                   teeEncryptionKeyHandle, teeHmacKeyHandle, teeWrappingKeyHandle);

      int ssKeyLength = reeWebCrypto->getDHKeySharedSecretLength(reePrivKey);
      if(ssKeyLength != 128)
         std::cout << "\tDH Shared Secret length : " << ssKeyLength << "\n";

      DataBuffer clearData(DataBuffer("Hello", 5));
      DataBuffer encryptedData;
      DataBuffer decryptedData;
      // Encrypt with REE
      aesCbc(REE, reeWebCrypto, reeEncryptionKeyHandle, ivBuf, clearData, IWebCrypto::DOENCRYPT, encryptedData);
      // Decrypt with TEE
      aesCbc(TEE, teeWebCrypto, teeEncryptionKeyHandle, ivBuf, encryptedData, IWebCrypto::DODECRYPT, decryptedData);
      if(clearData != decryptedData) {
         std::cout << "REE DH Shared Secret length : " << ssKeyLength << "\n";
         exit(1);
      }

      deleteKey(REE, reeWebCrypto, reePrivKey);
      deleteKey(REE, reeWebCrypto, reePubKey);
      deleteKey(REE, reeWebCrypto, reeEncryptionKeyHandle);
      deleteKey(REE, reeWebCrypto, reeHmacKeyHandle);
      deleteKey(REE, reeWebCrypto, reeWrappingKeyHandle);

      deleteKey(TEE, teeWebCrypto, teePrivKey);
      deleteKey(TEE, teeWebCrypto, teePubKey);
      deleteKey(TEE, teeWebCrypto, teeEncryptionKeyHandle);
      deleteKey(TEE, teeWebCrypto, teeHmacKeyHandle);
      deleteKey(TEE, teeWebCrypto, teeWrappingKeyHandle);
   }
   return 0;
}






