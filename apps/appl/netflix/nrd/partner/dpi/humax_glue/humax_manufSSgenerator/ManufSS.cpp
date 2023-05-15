#include "ManufSS.h"
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

using namespace netflix::manufacturing;

#if defined(HUMAX_PLATFORM_BASE)
extern "C" int DI_MI_Init(void);
extern "C" void DI_MI_Uninit(void);
extern "C" void* DI_MI_DECRYPTOR_SOC_Malloc(unsigned int unBytes);
extern "C" int DI_MI_DECRYPTOR_SOC_OTPKEY_DoEncrypt(unsigned char *in, unsigned char *out, unsigned int size);
extern "C" void DI_MI_DECRYPTOR_SOC_Free(void *pMemory);
#endif

uint8_t ManufSS::secStoreHmacKey[32] =
{ 0x4b, 0x40, 0xf4, 0x7e, 0xf6, 0x29, 0xd0, 0x1c, 0x1f, 0x37, 0xe8, 0x3d, 0x3d,
		0x30, 0xcd, 0x28, 0x70, 0x22, 0xee, 0x40, 0x10, 0xf5, 0x81, 0x61, 0x64,
		0x2a, 0xe1, 0x7e, 0xe1, 0x7e, 0xec, 0x19 };

uint8_t ManufSS::secStoreAesKey[16] =
{ 0xd8, 0x95, 0x7a, 0x33, 0x14, 0x4d, 0xe9, 0x30, 0x02, 0xcd, 0x32, 0x89, 0x6e,
		0x57, 0xdf, 0xfc };
uint8_t ManufSS::fileBuffer[nsMAX_SS_PROVISION_SIZE];
char ManufSS::readBuffer[nsMAX_SS_PROVISION_SIZE];
const char ManufSS::cb64[] =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
const char ManufSS::cd64[] =
		"|$$$}rstuvwxyz{$$$$$$$>?@ABCDEFGHIJKLMNOPQRSTUVW$$$$$$XYZ[\\]^_`abcdefghijklmnopq";

ManufSS::ManufSS(std::string targetDir, std::string idfile,
		std::string drmDir)
		:idfilePath (idfile),
		 drmDirPath (drmDir)
{
	manufSSPath = targetDir + nfSS_FILENAME;
}

int ManufSS::exists()
{
	int ret = 0;

	FILE *fp = NULL;
	if ((fp = fopen(manufSSPath.c_str(), "r")) == NULL)
	{
		// just printing out the distinction between really missing and cannot open file
		if (errno == ENOENT) // file does not exist
			printf("%s does not exist\n", manufSSPath.c_str());
		else
			printf("%s could not be opened because of error %d\n",
					manufSSPath.c_str(), errno);
	}
	else // file exists
	{
		ret = 1;
		printf("%s exists\n", manufSSPath.c_str());
	}

	return ret;
}

int ManufSS::create()
{
	FILE *ofp = NULL;
	int rc;
	int bytesToWrite;
	int bytesWritten;

	uint8_t *bufPtr;
	uint8_t *totalLengthPtr;

	uint8_t *padLengthPtr;
	uint16_t totalLength = 0;
	uint16_t padLength = 0;
	uint16_t remainder;

	// clear buffer, get pointer to start
	(void) bzero(fileBuffer, nsMAX_SS_PROVISION_SIZE);
	bufPtr = fileBuffer;

	/* first, create secure store header */
	// write file magic (secstore is big-endian)
	*bufPtr++ = (nfSS_FILE_MAGIC1 >> 24) & 0xff;
	*bufPtr++ = (nfSS_FILE_MAGIC1 >> 16) & 0xff;
	*bufPtr++ = (nfSS_FILE_MAGIC1 >> 8) & 0xff;
	*bufPtr++ = (nfSS_FILE_MAGIC1 & 0xff);
	*bufPtr++ = (nfSS_FILE_MAGIC2 >> 24) & 0xff;
	*bufPtr++ = (nfSS_FILE_MAGIC2 >> 16) & 0xff;
	*bufPtr++ = (nfSS_FILE_MAGIC2 >> 8) & 0xff;
	*bufPtr++ = (nfSS_FILE_MAGIC2 & 0xff);

	// write version
	*bufPtr++ = (nfSS_FILE_VERSION >> 8) & 0xff;
	*bufPtr++ = (nfSS_FILE_VERSION & 0xff);

	// save pointer to total length, we'll update later
	totalLengthPtr = bufPtr;
	bufPtr += sizeof(uint16_t);

	bufPtr += nfSS_HEADER_HMAC_SIZE;

	// save pointer to pad length, we'll update later
	padLengthPtr = bufPtr;
	bufPtr += sizeof(uint16_t);

	/*
	 * now, time to start building sections
	 */

	// first section: management section
	printf("calling AddMgmtSection().\n");
	if ((rc = addMgmtSection(&bufPtr,
	nsMAX_SS_PROVISION_SIZE - (bufPtr - fileBuffer))) != 0)
	{
		printf("AddMgmtSection failed, bailing.\n");
		goto ERROR;
	}

	// next section: Crypto read-only section (esn, kpe, kph)
	printf("calling AddCryptoSection().\n");
	if ((rc = addCryptoSection(&bufPtr,
	nsMAX_SS_PROVISION_SIZE - (bufPtr - fileBuffer), idfilePath)) != 0)
	{
		printf("AddMgmtSection failed, bailing.\n");
		goto ERROR;
	}

	// next section: DRM read-only section (model cert/privkey, device cert template)
	// add DRM section only if the required files are present
    if ((rc = checkDrmFiles(drmDirPath)) == 0)
    {
        printf("calling AddDrmSection().\n");
        if ((rc = addDrmSection(&bufPtr,
        nsMAX_SS_PROVISION_SIZE - (bufPtr - fileBuffer), drmDirPath)) != 0)
        {
            printf("AddDrmSection failed, bailing.\n");
            goto ERROR;
        }
    }
    else
    {
        printf("WARNING! Not adding DRM section because required file(s) are missing\n" );
    }

	// Okay, no more sections, time to hash/encrypt
	printf("Calculating pad.\n");

	// calculate pad length and apply pkcs7 padding if appropriate
	totalLength = (uint16_t) (bufPtr - padLengthPtr);
	printf("totalLength is %d\n", totalLength);
	if ((remainder = (totalLength + SHA1_BLOCK_SIZE) % CIPHER_BLOCK_SIZE) != 0)
	{
		int i;

		padLength = CIPHER_BLOCK_SIZE - remainder;

		for (i = 1; i <= padLength; i++)
			*bufPtr++ = (uint8_t) i;
	}

	printf("padLength is %d\n", padLength);

	// update totalLength (it is the total, including padding and MAC)
	totalLength += padLength;

	// save padLength
	*padLengthPtr = (padLength >> 8) & 0xFF;
	*(padLengthPtr + 1) = padLength & 0xFF;

	/*
	 * compute ICV over data (SHA1), add to buffer
	 */
	rc = sha1(padLengthPtr, totalLength, bufPtr);
	if (rc != 0)
	{
		printf("sha1 returned %d, exiting.\n", rc);
		return -1;
	}

	// update pointer, section length
	bufPtr += SHA1_BLOCK_SIZE;
	totalLength += SHA1_BLOCK_SIZE;
	*totalLengthPtr = (totalLength >> 8) & 0xFF;
	*(totalLengthPtr + 1) = totalLength & 0xFF;

	printf("I think totalLength is %d\n", totalLength);

	printf("calling Aes with %d bytes...\n",
			(uint32_t) (bufPtr - padLengthPtr));

	// compute ICV over secstore header
#if defined(HUMAX_PLATFORM_BASE)
	{
		DI_MI_Init();
		//data (Mgmt Section + Crypto (MTBA) Section + Drm Section) Encrypt
		printf("\n\nMI ENCRYPTOR Called(data encrypted)\n\n");
		uint8_t *bufferptr = (uint8_t *)DI_MI_DECRYPTOR_SOC_Malloc((uint32_t) (bufPtr - padLengthPtr));
		if(bufferptr == NULL)
		{
			printf("(Nexus) mem alloc failed\n");
			DI_MI_Uninit();
			return -1;
		}
		memset(bufferptr, 0, (uint32_t) (bufPtr - padLengthPtr));
		memcpy((void *)bufferptr, (void *)padLengthPtr, (uint32_t) (bufPtr - padLengthPtr));
		if(DI_MI_DECRYPTOR_SOC_OTPKEY_DoEncrypt((unsigned char *)bufferptr, (unsigned char *)bufferptr, (unsigned int) (bufPtr - padLengthPtr))!=0)
		{
			printf("Data Encrypt Failed\n");
			DI_MI_DECRYPTOR_SOC_Free(bufferptr);
			bufferptr = NULL;
			DI_MI_Uninit();
			return -1;
		}
		else
		{
			printf("Data Encrypt Success\n");
			memcpy(padLengthPtr, bufferptr, (uint32_t) (bufPtr - padLengthPtr));
			DI_MI_DECRYPTOR_SOC_Free(bufferptr);
			bufferptr = NULL;
		}

		printf("\n\nMI ENCRYPTOR Called(HMACKey encrypted)\n\n");
		//secStoreHmacKey Encrypt
		bufferptr = (uint8_t *)DI_MI_DECRYPTOR_SOC_Malloc(nfSS_HEADER_HMAC_SIZE);
		if(bufferptr == NULL)
		{
			printf("(Nexus) mem alloc failed\n");
			DI_MI_Uninit();
			return -1;
		}
		memset(bufferptr, 0, nfSS_HEADER_HMAC_SIZE);
		memcpy(bufferptr, (void *)secStoreHmacKey, nfSS_HEADER_HMAC_SIZE);
		if(DI_MI_DECRYPTOR_SOC_OTPKEY_DoEncrypt((unsigned char *)bufferptr, (unsigned char *)bufferptr, (unsigned int)nfSS_HEADER_HMAC_SIZE)!=0)
		{
			printf("HMACKey Encrypt Failed\n");
			DI_MI_DECRYPTOR_SOC_Free(bufferptr);
			bufferptr = NULL;
			DI_MI_Uninit();
			return -1;
		}
		else
		{
			printf("HMACKey Encrypt Success\n");
			memcpy(secStoreHmacKey, bufferptr, nfSS_HEADER_HMAC_SIZE);
			DI_MI_DECRYPTOR_SOC_Free(bufferptr);
			bufferptr = NULL;
		}
		DI_MI_Uninit();
	}
#else
	if ((rc = aesCbc128EncryptZeroIV(padLengthPtr,
			(uint32_t) (bufPtr - padLengthPtr), secStoreAesKey)) != 0)
	{
		// encryption failed, need to bail...
		printf("EncryptWithClientKey returned %d, exiting.\n", rc);
		return -1;
	}

#endif
	rc = hmacSha256(fileBuffer,
	nfSS_FILE_MAGIC_SIZE + nfSS_FILE_VERSION_SIZE + sizeof(uint16_t),
			secStoreHmacKey,
			nfSS_HEADER_HMAC_SIZE,
			fileBuffer + nfSS_FILE_MAGIC_SIZE + nfSS_FILE_VERSION_SIZE
					+ sizeof(uint16_t));
	if (rc < 0)
	{
		printf("hmacSha256 returned %d, exiting.\n", rc);
		return -1;
	}

	// construct output filename
	if ((ofp = fopen(manufSSPath.c_str(), "w")) == NULL)
	{
		printf("failed to open %s for writing, aborting.\n", manufSSPath.c_str());
		return -1;
	}

	// how many bytes?
	bytesToWrite = (int) (bufPtr - fileBuffer);
	printf("secstore file is %d bytes\n", bytesToWrite);

	// write file
	bufPtr = fileBuffer;
	do
	{
		// write output file
		bytesWritten = fwrite(bufPtr, 1, (size_t) bytesToWrite, ofp);
		if (bytesWritten < 0)
		{
			printf("failed to write secstore.\n");
			goto ERROR;
		}
		bytesToWrite -= bytesWritten;
		bufPtr += bytesWritten;
	} while (bytesToWrite > 0);

	printf("secstore file is written.\n");

	(void) fclose(ofp);

	return 0;

	/* NOTREACHED */
	ERROR: if (ofp != NULL)
		fclose(ofp);
	return -1;
} //createSecureStore

int ManufSS::addMgmtSection(uint8_t **bufPtrPtr, size_t maxLength)
{
	// no mercy
	if ((bufPtrPtr == NULL) || (*bufPtrPtr == NULL)
			|| (maxLength <= nfSS_MGMT_SECTION_SIZE))
	{
		printf("wrong input arguments\n");
		return -1;
	}

	unsigned int i;
	uint8_t *bufPtr;
	uint8_t *sectionLengthPtr;
	uint16_t sectionLength = 0;

	uint8_t CryptoROClientId[] = nfSS_CLIENT_ID_TEE_CRYPTO_RO;
	uint8_t CryptoRWClientId[] = nfSS_CLIENT_ID_TEE_CRYPTO_RW;
	uint8_t DrmROClientId[] = nfSS_CLIENT_ID_TEE_DRM_RO;
	uint8_t DrmRWClientId[] = nfSS_CLIENT_ID_TEE_DRM_RW;
	uint8_t NrdClientId[] = nfSS_CLIENT_ID_NRD_APP;

	/* **************** NEW WAY *********************
	 * Here's what we need to add:
	 *
	 * TEE MGMT Section ID                 4 bytes
	 * section length                      2 bytes
	 * CRYPTO RO section ID                4 bytes
	 * CRYPTO RO client ID                 16 bytes
	 * CRYPTO RW section ID                4 bytes
	 * CRYPTO RW client ID                 16 bytes
	 * DRM RO section ID                  4 bytes
	 * DRM RO client ID                  16 bytes
	 * DRM RW section ID                  4 bytes
	 * DRM RW client ID                  16 bytes
	 * NRD section ID                     4 bytes
	 * NRD client ID                     16 bytes
	 */

	// get working pointer into buffer; we'll update *bufPtrPtr at the end
	bufPtr = *bufPtrPtr;

	// add TEE MGMT section ID
	*bufPtr++ = (teeSID_TEE_MGMT >> 24) & 0xff;
	*bufPtr++ = (teeSID_TEE_MGMT >> 16) & 0xff;
	*bufPtr++ = (teeSID_TEE_MGMT >> 8) & 0xff;
	*bufPtr++ = (teeSID_TEE_MGMT & 0xff);

	// save section length pointer, we'll fill this in shortly
	sectionLengthPtr = bufPtr;
	bufPtr += sizeof(uint16_t);

	// add CRYPTO RO section ID
	*bufPtr++ = (teeSID_CRYPTO_RO >> 24) & 0xff;
	*bufPtr++ = (teeSID_CRYPTO_RO >> 16) & 0xff;
	*bufPtr++ = (teeSID_CRYPTO_RO >> 8) & 0xff;
	*bufPtr++ = (teeSID_CRYPTO_RO & 0xff);

	// add CRYPTO RO Client ID
	for (i = 0; i < sizeof(CryptoROClientId); ++i)
		*bufPtr++ = CryptoROClientId[i];

	// add CRYPTO RW section ID
	*bufPtr++ = (teeSID_CRYPTO_RW >> 24) & 0xff;
	*bufPtr++ = (teeSID_CRYPTO_RW >> 16) & 0xff;
	*bufPtr++ = (teeSID_CRYPTO_RW >> 8) & 0xff;
	*bufPtr++ = (teeSID_CRYPTO_RW & 0xff);

	// add CRYPTO RW Client ID
	for (i = 0; i < sizeof(CryptoRWClientId); ++i)
		*bufPtr++ = CryptoRWClientId[i];

	// add DRM RO section ID
	*bufPtr++ = (teeSID_DRM_RO >> 24) & 0xff;
	*bufPtr++ = (teeSID_DRM_RO >> 16) & 0xff;
	*bufPtr++ = (teeSID_DRM_RO >> 8) & 0xff;
	*bufPtr++ = (teeSID_DRM_RO & 0xff);

	// add DRM RO Client ID
	for (i = 0; i < sizeof(DrmROClientId); ++i)
		*bufPtr++ = DrmROClientId[i];

	// add DRM RW section ID
	*bufPtr++ = (teeSID_DRM_RW >> 24) & 0xff;
	*bufPtr++ = (teeSID_DRM_RW >> 16) & 0xff;
	*bufPtr++ = (teeSID_DRM_RW >> 8) & 0xff;
	*bufPtr++ = (teeSID_DRM_RW & 0xff);

	// add DRM RW Client ID
	for (i = 0; i < sizeof(DrmRWClientId); ++i)
		*bufPtr++ = DrmRWClientId[i];

	// add NRD section ID
	*bufPtr++ = (teeSID_NRD_APP1 >> 24) & 0xff;
	*bufPtr++ = (teeSID_NRD_APP1 >> 16) & 0xff;
	*bufPtr++ = (teeSID_NRD_APP1 >> 8) & 0xff;
	*bufPtr++ = (teeSID_NRD_APP1 & 0xff);

	// add NRD APP Client ID
	for (i = 0; i < sizeof(NrdClientId); ++i)
		*bufPtr++ = NrdClientId[i];

	// update sectionLength
	sectionLength = (uint16_t) (bufPtr - *bufPtrPtr) - sizeof(teeSectionIDType)
			- sizeof(uint16_t);
	printf("sectionLength is %d\n", sectionLength);
	*sectionLengthPtr = (sectionLength >> 8) & 0xFF;
	*(sectionLengthPtr + 1) = sectionLength & 0xFF;

	// adjust *bufPtrPtr before returning
	*bufPtrPtr = bufPtr;
	return 0;

}/* end AddMgmtSection */

int ManufSS::addCryptoSection(uint8_t **bufPtrPtr, size_t maxLength,
		std::string &idFilename)
{
	// no mercy
	if ((bufPtrPtr == NULL) || (*bufPtrPtr == NULL)
			|| (maxLength <= nsMAX_SS_CRYPTO_RO_SIZE) || (idFilename.empty()))
	{
		printf("wrong input arguments\n");
		return -1;
	}

	FILE *ifp = NULL;
	int esnLength;
	char esn[ESN_MAX + 1];
	char kpe[CIPHER_BLOCK_SIZE];
	char kph[HMAC_BLOCK_SIZE];
// sizes of b64-encoded keys
#define B64_KPE_MAX 24
#define B64_KPH_MAX 48
	uint8_t *bufPtr;
	uint8_t *sectionLengthPtr;
	uint16_t sectionLength = 0;

	/*
	 * Here's what we need to do:
	 *
	 *  - construct idfile path/filename
	 * 	- open idfile
	 *  - read in esn, kpe, kph
	 *  - ADD CRYPTO-RO section to *bufPtrPtr
	 */

	// open input file
	if ((ifp = fopen(idFilename.c_str(), "r")) == NULL)
	{
		printf("failed to open idfile %s for input, aborting.\n", idFilename.c_str());
		return -1;
	}

	// first, read in the esn
	if (fgets(readBuffer, sizeof(readBuffer), ifp) == NULL)
	{
		// error or EOF, give message and bail
		printf("failed to read esn from idfile, aborting.\n");
		if (ifp != NULL)
			fclose(ifp);
		return -1;
	}

	// remove newline, if present
	char* readBufferPtr = readBuffer;
	trimNewLine(&readBufferPtr);

	// esn should not be more than ESN_MAX characters
	if (strlen(readBuffer) > ESN_MAX)
	{
		printf("esn too long (%zu/%d) - exiting.\n", strlen(readBuffer),
				ESN_MAX);
		if (ifp != NULL)
			fclose(ifp);
		return -1;
	}

	// XXX - should we do esn validation here???
	printf("XXX - what about esn validation???\n");

	// save esn
	(void) strcpy(esn, readBuffer);

	// now, read b64-encoded kpe
	*readBuffer = '\0';

	if (fgets(readBuffer, sizeof(readBuffer), ifp) == NULL)
	{
		// error or EOF, give message and bail
		printf("failed to read esn, exiting.\n");
		if (ifp != NULL)
			fclose(ifp);
		return -1;
	}

	// remove newline, if present
	readBufferPtr = readBuffer;
	trimNewLine(&readBufferPtr);

	// B64 kpe should not be more than B64_KPE_MAX characters
	if (strlen(readBuffer) > B64_KPE_MAX)
	{
		printf("kpe too long (%zu/%d) - exiting.\n", strlen(readBuffer),
				B64_KPE_MAX);
		if (ifp != NULL)
			fclose(ifp);
		return -1;
	}

	// save decoded kpe
	bzero(kpe, sizeof(kpe));
	if (b64_decode(readBuffer, kpe) != 0)
	{
		printf("kpe decode failed, exiting.\n");
		if (ifp != NULL)
			fclose(ifp);
		return -1;
	}

	// now, read b64-encoded kph
	*readBuffer = '\0';

	if (fgets(readBuffer, sizeof(readBuffer), ifp) == NULL)
	{
		// error or EOF, give message and bail
		printf("failed to read esn, exiting.\n");
		if (ifp != NULL)
			fclose(ifp);
		return -1;
	}

	// remove newline, if present
	readBufferPtr = readBuffer;
	trimNewLine(&readBufferPtr);
	readBufferPtr = NULL;

	// B64 kph should not be more than B64_KPH_MAX characters
	if (strlen(readBuffer) > B64_KPH_MAX)
	{
		printf("kph too long (%zu/%d) - exiting.\n", strlen(readBuffer),
				B64_KPH_MAX);
		if (ifp != NULL)
			fclose(ifp);
		return -1;
	}

	// save decoded kph
	bzero(kph, sizeof(kph));
	if (b64_decode(readBuffer, kph) != 0)
	{
		printf("kph decode failed, exiting.\n");
		if (ifp != NULL)
			fclose(ifp);
		return -1;
	}

	// we're done with file, so close it
	(void) fclose(ifp);

	/*
	 * add section to buffer...
	 */
	bufPtr = *bufPtrPtr;

	// add section ID
	*bufPtr++ = (teeSID_CRYPTO_RO >> 24) & 0xff;
	*bufPtr++ = (teeSID_CRYPTO_RO >> 16) & 0xff;
	*bufPtr++ = (teeSID_CRYPTO_RO >> 8) & 0xff;
	*bufPtr++ = (teeSID_CRYPTO_RO & 0xff);

	// save section length pointer, we'll fill this in shortly
	sectionLengthPtr = bufPtr;
	bufPtr += sizeof(uint16_t);

	// add esn key, length, and esn
	*bufPtr++ = (cryptoAGENT_KEY_ESN >> 24) & 0xff;
	*bufPtr++ = (cryptoAGENT_KEY_ESN >> 16) & 0xff;
	*bufPtr++ = (cryptoAGENT_KEY_ESN >> 8) & 0xff;
	*bufPtr++ = (cryptoAGENT_KEY_ESN & 0xff);

	esnLength = strlen(esn);
	*bufPtr++ = (esnLength >> 8) & 0xff;
	*bufPtr++ = (esnLength & 0xff);

	(void) memcpy(bufPtr, esn, esnLength);
	bufPtr += esnLength;

	// add kpe key, length, and kpe
	*bufPtr++ = (cryptoAGENT_KEY_KPE >> 24) & 0xff;
	*bufPtr++ = (cryptoAGENT_KEY_KPE >> 16) & 0xff;
	*bufPtr++ = (cryptoAGENT_KEY_KPE >> 8) & 0xff;
	*bufPtr++ = (cryptoAGENT_KEY_KPE & 0xff);

	// length of KPE is always the same
	*bufPtr++ = (sizeof(kpe) >> 8) & 0xff; // == 0
	*bufPtr++ = sizeof(kpe);

	(void) memcpy(bufPtr, kpe, sizeof(kpe));
	bufPtr += sizeof(kpe);

	// add kph key, length, and kph
	*bufPtr++ = (cryptoAGENT_KEY_KPH >> 24) & 0xff;
	*bufPtr++ = (cryptoAGENT_KEY_KPH >> 16) & 0xff;
	*bufPtr++ = (cryptoAGENT_KEY_KPH >> 8) & 0xff;
	*bufPtr++ = (cryptoAGENT_KEY_KPH & 0xff);

	// length of KPH is always the same
	*bufPtr++ = (sizeof(kph) >> 8) & 0xff; // == 0
	*bufPtr++ = sizeof(kph);

	(void) memcpy(bufPtr, kph, sizeof(kph));
	bufPtr += sizeof(kph);

	// update sectionLength
	sectionLength = (uint16_t) (bufPtr - *bufPtrPtr) - sizeof(teeSectionIDType)
			- sizeof(uint16_t);
	printf("sectionLength is %d\n", sectionLength);
	*sectionLengthPtr = (sectionLength >> 8) & 0xFF;
	*(sectionLengthPtr + 1) = sectionLength & 0xFF;

	// if we get here, we need to adjust *bufPtrPtr before returning
	*bufPtrPtr = bufPtr;
	return 0;
}/* end AddCryptoSection */

int ManufSS::addDrmSection(uint8_t **bufPtrPtr, size_t maxLength,
		std::string &drmDir)
{
	// no mercy
	if ((bufPtrPtr == NULL) || (*bufPtrPtr == NULL)
			|| (maxLength <= nsMAX_SS_DRM_RO_SIZE) || (drmDir.empty()))
	{
		printf("wrong input arguments\n");
		return -1;
	}

	FILE *ifp = NULL;
	uint8_t *bufPtr;
	std::string filePath;
	uint8_t *sectionLengthPtr;
	uint16_t sectionLength = 0;
	struct stat statBuf;
	int bytesToRead;
	int bytesRead;

	/*
	 * Here's what we need to do:
	 *
	 *	- add DRM-RO section header
	 *  - for file in {devcerttemplate.dat,groupcert.dat,grouppriv.dat} do
	 *  	- construct path/filename
	 *      - get filesize, validate against remaining space
	 * 		- open file
	 *  	- add key, length
	 *		- read in data from file
	 *		- close file
	 *  - update total length
	 *  - update *bufPtrPtr
	 */

	// Model Private Key
	// open input file
	filePath = drmDir + nfSS_DRM_GROUPPRIVKEY_FILENAME;
	printf("opening >%s< for reading.\n", nfSS_DRM_GROUPPRIVKEY_FILENAME);
	if ((ifp = fopen(filePath.c_str(), "r")) == NULL)
	{
		printf("failed to open %s for input, aborting.\n",
		nfSS_DRM_GROUPPRIVKEY_FILENAME);
		return -1;
	}
	if (stat(filePath.c_str(), &statBuf) != 0)
	{
		printf("stat() failed for %s, aborting.\n",
		nfSS_DRM_GROUPPRIVKEY_FILENAME);
		goto ERROR;
	}

	printf(">%s< is %d bytes in length.\n", nfSS_DRM_GROUPPRIVKEY_FILENAME,
			(int) statBuf.st_size);

	/*
	 * add section to buffer...
	 */
	bufPtr = *bufPtrPtr;
	// add section ID
	*bufPtr++ = (teeSID_DRM_RO >> 24) & 0xff;
	*bufPtr++ = (teeSID_DRM_RO >> 16) & 0xff;
	*bufPtr++ = (teeSID_DRM_RO >> 8) & 0xff;
	*bufPtr++ = (teeSID_DRM_RO & 0xff);

	// save section length pointer, we'll fill this in shortly
	sectionLengthPtr = bufPtr;
	bufPtr += sizeof(uint16_t);

	// add key, length, and PlayReady model private key
	*bufPtr++ = (drmAGENT_KEY_MODEL_PRIV >> 24) & 0xff;
	*bufPtr++ = (drmAGENT_KEY_MODEL_PRIV >> 16) & 0xff;
	*bufPtr++ = (drmAGENT_KEY_MODEL_PRIV >> 8) & 0xff;
	*bufPtr++ = (drmAGENT_KEY_MODEL_PRIV & 0xff);

	// length is in statBuf.st_size
	if (statBuf.st_size >= 65536)
	{
		printf("statbuf size is greater than  65536, aborting.\n");
		goto ERROR;
	}
	*bufPtr++ = (statBuf.st_size >> 8) & 0xff;
	*bufPtr++ = (statBuf.st_size & 0xff);

	// read into memory
	bytesToRead = statBuf.st_size;
	do
	{
		bytesRead = fread(bufPtr, 1, bytesToRead, ifp);
		printf("I read %d bytes (of %d requested)\n", bytesRead, bytesToRead);
		if (bytesRead < 0)
		{
			printf("fread(%s) returned %d, aborting.\n",
					nfSS_DRM_GROUPPRIVKEY_FILENAME, bytesRead);
			goto ERROR;
		}
		if (bytesRead == 0)
		{
			printf("fread is returning 0 (of %d)!\n", bytesToRead);
			exit(1);
		}
		bytesToRead -= bytesRead;
		bufPtr += bytesRead;

	} while (bytesToRead > 0);

	// close file
	(void) fclose(ifp);
	printf("closed >%s<\n", nfSS_DRM_GROUPPRIVKEY_FILENAME);

	// Model Certificate
	filePath = drmDir + nfSS_DRM_GROUPCERT_FILENAME;
	printf("opening >%s< for reading.\n", nfSS_DRM_GROUPCERT_FILENAME);
	// open input file
	if ((ifp = fopen(filePath.c_str(), "r")) == NULL)
	{
		printf("failed to open %s for input, aborting.\n",
		nfSS_DRM_GROUPCERT_FILENAME);
		goto ERROR;
	}

	if (stat(filePath.c_str(), &statBuf) != 0)
	{
		printf("stat() failed for %s, aborting.\n",
		nfSS_DRM_GROUPCERT_FILENAME);
		goto ERROR;
	}
	printf(">%s< is %d bytes in length.\n", nfSS_DRM_GROUPCERT_FILENAME,
			(int) statBuf.st_size);

	// add key, length, and PlayReady model cert
	*bufPtr++ = (drmAGENT_KEY_MODEL_CERT >> 24) & 0xff;
	*bufPtr++ = (drmAGENT_KEY_MODEL_CERT >> 16) & 0xff;
	*bufPtr++ = (drmAGENT_KEY_MODEL_CERT >> 8) & 0xff;
	*bufPtr++ = (drmAGENT_KEY_MODEL_CERT & 0xff);

	// length is in statBuf.st_size
	if (statBuf.st_size >= 65536)
	{
		printf("statbuf size is greater than  65536, aborting.\n");
		goto ERROR;
	}
	*bufPtr++ = (statBuf.st_size >> 8) & 0xff;
	*bufPtr++ = (statBuf.st_size & 0xff);

	// read into memory
	bytesToRead = statBuf.st_size;
	do
	{
		bytesRead = fread(bufPtr, 1, bytesToRead, ifp);
		if (bytesRead < 0)
		{
			printf("fread(%s) returned %d, aborting.\n",
					nfSS_DRM_GROUPCERT_FILENAME, bytesRead);
			goto ERROR;
		}

		bytesToRead -= bytesRead;
		bufPtr += bytesRead;

	} while (bytesToRead > 0);

	// close file
	(void) fclose(ifp);
	printf("closed >%s<.\n", nfSS_DRM_GROUPCERT_FILENAME);

	// open input file
	filePath = drmDir + nfSS_DRM_DEVCERT_TEMPLATE_FILENAME;
	printf("opening >%s< for reading.\n", nfSS_DRM_DEVCERT_TEMPLATE_FILENAME);
	if ((ifp = fopen(filePath.c_str(), "r")) == NULL)
	{
		printf("failed to open %s for input, aborting.\n",
		nfSS_DRM_DEVCERT_TEMPLATE_FILENAME);
		goto ERROR;
	}

	if (stat(filePath.c_str(), &statBuf) != 0)
	{
		printf("stat() failed for %s, aborting.\n",
		nfSS_DRM_DEVCERT_TEMPLATE_FILENAME);
		goto ERROR;
	}
	printf(">%s< is %d bytes in length.\n", nfSS_DRM_DEVCERT_TEMPLATE_FILENAME,
			(int) statBuf.st_size);

	// add key, length, and PlayReady model cert
	*bufPtr++ = (drmAGENT_KEY_DEVCERT_TEMPLATE >> 24) & 0xff;
	*bufPtr++ = (drmAGENT_KEY_DEVCERT_TEMPLATE >> 16) & 0xff;
	*bufPtr++ = (drmAGENT_KEY_DEVCERT_TEMPLATE >> 8) & 0xff;
	*bufPtr++ = (drmAGENT_KEY_DEVCERT_TEMPLATE & 0xff);

	// length is in statBuf.st_size
	if (statBuf.st_size >= 65536)
	{
		printf("statbuf size is greater than  65536, aborting.\n");
		goto ERROR;
	}
	*bufPtr++ = (statBuf.st_size >> 8) & 0xff;
	*bufPtr++ = (statBuf.st_size & 0xff);

	// read into memory
	bytesToRead = statBuf.st_size;
	do
	{
		bytesRead = fread(bufPtr, 1, bytesToRead, ifp);
		if (bytesRead < 0)
		{
			printf("fread(%s) returned %d, aborting.\n",
					nfSS_DRM_DEVCERT_TEMPLATE_FILENAME, bytesRead);
			goto ERROR;
		}

		bytesToRead -= bytesRead;
		bufPtr += bytesRead;

	} while (bytesToRead > 0);

	// close file
	(void) fclose(ifp);
	printf("closed >%s<.\n", nfSS_DRM_DEVCERT_TEMPLATE_FILENAME);

	// update sectionLength
	sectionLength = (uint16_t) (bufPtr - *bufPtrPtr) - sizeof(teeSectionIDType)
			- sizeof(uint16_t);
	printf("sectionLength is %d\n", sectionLength);
	*sectionLengthPtr = (sectionLength >> 8) & 0xFF;
	*(sectionLengthPtr + 1) = sectionLength & 0xFF;

	// if we get here, we need to adjust *bufPtrPtr before returning
	*bufPtrPtr = bufPtr;
	return 0;

	/* NOTREACHED */
	ERROR: if (ifp != NULL)
		fclose(ifp);
	return -1;

}/* end AddDrmSection */

int ManufSS::aesCbc128EncryptZeroIV(uint8_t *cleartextPtr,
		uint32_t cleartextLength, uint8_t *aesKeyPtr)
{
	// no mercy
	if ((cleartextPtr == NULL) || (cleartextLength <= 0) || (aesKeyPtr == NULL))
	{
		printf("wrong input arguments\n");
		return -1;
	}

	if (cleartextLength % 16 != 0)
	{
		printf("%d-byte buffer needs %d bytes of padding.\n", cleartextLength,
				cleartextLength % 16);
		return -1;
	}

	uint8_t *tempPtr;
	if ((tempPtr = (uint8_t *) malloc(cleartextLength)) == NULL)
	{
		printf("can't allocated %d bytes.\n", cleartextLength);
		return -1;
	}

	AES_KEY localKey;
	(void) bzero(&localKey, sizeof(AES_KEY));
	if (AES_set_encrypt_key(aesKeyPtr, 128, &localKey) < 0)
	{
		printf("AES_set_encrypt_key() failed\n");
		free(tempPtr);
		return -1;
	}

	uint8_t iv[16];
	(void) bzero(iv, sizeof(iv));
	printf("calling AES_cbc_encrypt(%p, %p, %d )\n", cleartextPtr, tempPtr,
			cleartextLength);
	AES_cbc_encrypt(cleartextPtr, tempPtr, cleartextLength, &localKey, iv,
			AES_ENCRYPT);

	// now, copy encrypted data over top of cleartext
	(void) memcpy(cleartextPtr, tempPtr, cleartextLength);

	uint8_t tempbuf[8092];
	(void) bzero(tempbuf, sizeof(tempbuf));
	(void) bzero(&localKey, sizeof(AES_KEY));
	if (AES_set_decrypt_key(aesKeyPtr, 128, &localKey) < 0)
	{
		printf("AES_set_decrypt_key() failed\n");
		free(tempPtr);
		return -1;
	}
	(void) bzero(iv, sizeof(iv));
	AES_cbc_encrypt(tempPtr, tempbuf, cleartextLength, &localKey, iv,
			AES_DECRYPT);

	// free malloc'd mem...
	(void) free(tempPtr);
	return 0;
}/* end aesCbc128EncryptZeroIV */

int ManufSS::sha1(uint8_t *dataPtr, uint32_t dataLength, uint8_t *resultPtr)
{
	// no mercy
	if ((dataPtr == NULL) || (dataLength <= 0) || (resultPtr == NULL))
	{
		printf("wrong input arguments\n");
		return -1;
	}

	//initialize hash function
	SHA_CTX sha1Context;
	SHA1_Init(&sha1Context);

	// add the input buffer for hash creation
	SHA1_Update(&sha1Context, dataPtr, dataLength);

	// Finalize and save the hash data
	SHA1_Final(resultPtr, &sha1Context);

	return 0;
}/* end sha1 */

int ManufSS::hmacSha256(uint8_t *dataPtr, uint32_t dataLength, uint8_t *keyPtr,
		uint32_t keyLength, uint8_t *resultPtr)
{
	// no mercy
	if ((dataPtr == NULL) || (dataLength <= 0) || (keyPtr == NULL)
			|| (keyLength <= 0) || (resultPtr == NULL))
	{
		printf("wrong input arguments\n");
		return -1;
	}

	printf("called with %d bytes of data, %d-byte key.\n", dataLength,
			keyLength);

	HMAC_CTX context;
	uint32_t length = 0;
	HMAC_Init(&context, keyPtr, keyLength, EVP_sha256());
	HMAC_Update(&context, dataPtr, dataLength);
	HMAC_Final(&context, resultPtr, &length);

	printf("HMAC length is %d\n", length);
	return length > 0 ? length : -1;
}/* end hmacSha256 */

void ManufSS::decodeblock(unsigned char *in, unsigned char *out)
{
	out[0] = (unsigned char) (in[0] << 2 | in[1] >> 4);
	out[1] = (unsigned char) (in[1] << 4 | in[2] >> 2);
	out[2] = (unsigned char) (((in[2] << 6) & 0xc0) | in[3]);
}

int ManufSS::b64_decode(char *inPtr, char *outPtr)
{
	int retcode = 0;
	unsigned char in[4];
	unsigned char out[3];
	int v;
	int i, len;

	*in = (unsigned char) 0;
	*out = (unsigned char) 0;
	while (*inPtr != '\0')
	{
		for (len = 0, i = 0; i < 4 && *inPtr != '\0'; i++)
		{
			v = 0;
			while (*inPtr && v == 0)
			{
				v = *inPtr++;
				v = ((v < 43 || v > 122) ? 0 : (int) cd64[v - 43]);
				if (v != 0)
				{
					v = ((v == (int) '$') ? 0 : v - 61);
				}
			}/* end while ( *inPtr && v == 0 ) */
			if (*inPtr)
			{
				len++;
				if (v != 0)
				{
					in[i] = (unsigned char) (v - 1);
				}
			}
			else
			{
				in[i] = (unsigned char) 0;
			}
		}
		if (len > 0)
		{
			decodeblock(in, out);
			for (i = 0; i < len - 1; i++)
			{
				*outPtr++ = out[i];
			}
		}/* end if ( len > 0 ) */

	}/* end while( *inPtr != '\0' ) */
	return (retcode);
}

void ManufSS::trimNewLine(char** strPtr)
{
	char* str = *strPtr;
	int len = strlen(str);

	if (str != NULL)
	{
		if (str[len - 1] == '\r')	// ends in \r
			str[len - 1] = '\0';
		else if ((str[len - 1] == '\n') && (str[len - 2] == '\r')) // ends in /r/n
			str[len - 2] = '\0';
		else if (str[len - 1] == '\n') // ends in \n
			str[len - 1] = '\0';
	}
}

int ManufSS::checkDrmFiles(std::string &drmDir)
{
    std::string filePath;
    FILE *file;

    filePath = drmDir + nfSS_DRM_GROUPPRIVKEY_FILENAME;
    if ((file = fopen(filePath.c_str(), "r"))) {
        fclose(file);
    } else {
        printf("WARNING! %s is missing\n", nfSS_DRM_GROUPPRIVKEY_FILENAME);
        return -1;
    }

    filePath = drmDir + nfSS_DRM_GROUPCERT_FILENAME;
    if ((file = fopen(filePath.c_str(), "r"))) {
        fclose(file);
    } else {
        printf("WARNING! %s is missing\n", nfSS_DRM_GROUPCERT_FILENAME);
        return -1;
    }

    filePath = drmDir + nfSS_DRM_DEVCERT_TEMPLATE_FILENAME;
    if ((file = fopen(filePath.c_str(), "r"))) {
        fclose(file);
    } else {
        printf("WARNING! %s is missing\n", nfSS_DRM_DEVCERT_TEMPLATE_FILENAME);
        return -1;
    }

    return 0;
}
