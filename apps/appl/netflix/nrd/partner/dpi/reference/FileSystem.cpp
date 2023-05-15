/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include "FileSystem.h"

#include <errno.h>
#include <fcntl.h>
#include <fstream>
#include <iterator>
#include <libgen.h>
#include <locale>
#include <signal.h>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#ifdef __linux__
#include <link.h>
#include <sys/mman.h>
#endif

#include <nrdbase/Assert.h>
#include <nrdbase/Base64.h>
#include <nrdbase/OpenSSLLib.h>
#include <nrdbase/ReadDir.h>

#include <nrd/AppLog.h>
#include <nrd/NrdApplication.h>
#include <nrd/NrdConfiguration.h>

#include "DeviceLibNative.h"
#include "RendererManager.h"

#ifdef BUILD_QA
#include "QADeviceBridge.h"
#endif

#ifdef BUILD_SSO
#include "SingleSignOnBridge.h"
#endif

#ifdef BUILD_SIGNUP
#include "SignupBridge.h"
#endif

#ifdef BUILD_VOICE
#include "VoiceBridge.h"
#endif

#include "SystemValues.h"
#include "SignupBridge.h"
#include "SSOBridge.h"

using namespace netflix;
using namespace netflix::device;

// The private keys we store in memory (not TEE compliant)
DataBuffer ncf_kpe;
DataBuffer ncf_kph;
DataBuffer ncf_kav;

// Signatures of in-process shared objects
std::set<std::string> mSharedObjects;

namespace {


//#define MIGRATE_40_TO_41_SECURESTORE
#ifdef MIGRATE_40_TO_41_SECURESTORE
/* ***************************************************************************
 * NRDP4.0 -> 4.1 SecureStore migration sample codea
 *
 *  - Special handling may be needed IF secure store format is different in
 *    between NRDP4.0 and NRDP4.1.
 *  - In the case of this reference implementation (partner/dpi/reference),
 *    we introduced the following differences in between NRDP4.0 and 4.1
 *     0) The secure store contents are compatible between NRDP4.0 and NRDP4.1
 *     1) NRDP4.1 encrypts the secure store data, whereas NRDP4.0 does not
 *     2) NRDP4.1 does not store key-value pair if value is empty, whereas
 *        NRDP4.0 stores them.
 *
 *    For the above differences, here is what needs to be done
 *     0) Nothing
 *     1) The simple format conversion is needed
 *     2) Checksum value that is embedded in NRDP4.0 secure store must be
 *        re-calculated without key-value pair with empty value
 *
 *  < NOTE >
 *    This macro (MIGRATE_40_TO_41_SECURESTORE) only functions properly when
 *    needToMigrateSecureStore() is implemented properly. The migration code
 *    does not even run unless you modify needToMigrateSecureStore().
 *
 * ************************************************************************** */
#include <openssl/md5.h>

// This is the name of the attribute that identifies the checksum.
static const std::string CHECKSUM_ATTRIBUTE("CHECKSUM");

// Calculates the MD5 of a map, and returns a base-64 encoding of
// the digest.
static std::string calculateChecksum(const std::map<std::string, std::string> &m)
{
    MD5_CTX md5;
    MD5_Init(&md5);

    std::map<std::string, std::string>::const_iterator i;
    std::map<std::string, std::string>::const_iterator const end = m.end();
    for (i = m.begin(); i != end; ++i)
    {
        MD5_Update(&md5, i->first.data(), i->first.length());
        MD5_Update(&md5, i->second.data(), i->second.length());
    }

    unsigned char digest[MD5_DIGEST_LENGTH];
    MD5_Final(digest, &md5);
    std::string value;
    Base64::encode(digest, digest + sizeof digest, back_inserter(value));
    return value;
}

static std::map<std::string, std::string> loadNRDP40SecureStore(const char* secureStoreFile)
{
    std::map<std::string,std::string> pairs;
    std::ifstream is(secureStoreFile);
    do
    {
        std::string key, value;
        getline(is, key); if (!is) break;
        getline(is, value); if (!is) break;
        pairs[key] = value;
    } while (true);
    return pairs;
}

static std::map<std::string,std::string> convertSecureStore(const char* secureStoreFile)
{
    std::map<std::string,std::string> pairs = loadNRDP40SecureStore(secureStoreFile);

    // First, validate the checksum
    if ( !pairs.empty() )
    {
        std::map<std::string, std::string>::iterator it = pairs.find(CHECKSUM_ATTRIBUTE);
        if (it != pairs.end())
        {
            const std::string checksum = it->second;
            pairs.erase(it); // remove checksum when calculating
            if ( checksum != calculateChecksum(pairs) )
            {
                Log::error(TRACE_DPI, "Invalid checksum, clearing secure store");
                pairs.clear();  // invalid checksum
            }
        }
        else
        {
            Log::error(TRACE_DPI, "No checksum found in secure store, clearing secure store");
            pairs.clear(); // no checksum
        }
    }

    // Next, remove empty key-value pairs as TEE reference code cannot handle them
    for (std::map<std::string, std::string>::iterator it = pairs.begin(); it != pairs.end(); ++it)
    {
        if ( it->second.empty()) {
            pairs.erase(it);
        }
    }

    // Finally, calculate checksum
    std::string checksum(calculateChecksum(pairs));
    pairs[CHECKSUM_ATTRIBUTE] = checksum;

    return pairs;
}

static bool needToMigrateSecureStore()
{
    // This function must be modified to correctly determine if migration is needed or not.
    // And of course, migration only needs to happen ONCE when upgrading from NRDP4.0 to NRDP4.1
    // The migration code will be triggered only when true is returned
    return false;
}

#endif // MIGRATE_40_TO_41_SECURESTORE

/**
* Verifies the specified filenames are not empty and creates their
 * parent directories if they do not already exist.
 *
 * @param[in] encrypted encrypted filename.
 */
static void verifyFile(const std::string& fileName)
{
    ASSERT(!fileName.empty(), "No file specified.");
    char * fileNameCopy = strdup(fileName.c_str());
    char * dirName = dirname(fileNameCopy);
    if (strcmp(dirName, ".") != 0) {
        struct stat statBuf;
        if (stat(dirName, &statBuf) != 0 && !ReadDir::recursiveMakeDirectory(dirName, S_IRWXU)) {
            ASSERT(0, "Directory could not be created %d", errno);
        }
    }
    free(fileNameCopy);
}

} // namespace anonymous

bool FileSystem::sQaBridgeRenderDolbyVisionBL = true;
bool FileSystem::sQaBridgeRenderDolbyVisionEL = true;
std::string FileSystem::sQaBridgeDolbyFileDump = "";

FileSystem::FileSystem(const std::string& encryptedFile,
                       const std::string& pubkeyFile,
                       const std::string& esn,
                       const std::string& model)
    : mIpConnectivity(IP_V4_ONLY_MODE),
      mEsn(esn),
      mAuthType(UNKNOWN),
      mVolumeControlType(NO_VOLUME_CONTROL),
      mQaBridgeHEVCProfileEnvelope(ISystem::HEVC_MAIN10_L51),
      mQaBridgeAVCProfileEnvelope(ISystem::AVC_MAIN_L40),
      mEncryptedFile(encryptedFile),
      mScreensaverOn(false)
{
    if (false == model.empty())
    {
        mDeviceModel = model;
    } else {
        mDeviceModel = "Netflix_refapp";
        Variant configDataValues = netflix::Configuration::configDataValues();
        if(configDataValues.isStringMap() &&
           configDataValues.find("bundleVersion")!=configDataValues.stringMapEnd()){
            Variant bundleVersion=configDataValues["bundleVersion"];
            if (bundleVersion.isStringMap() &&
                bundleVersion.find("platformVersion")!=bundleVersion.stringMapEnd()){
                Variant platformVersion = bundleVersion["platformVersion"];
                if (platformVersion.isString()){
                    NTRACE(TRACE_DPI, "FileSystem::FileSystem() adding platformVersion %s to the mDeviceModel", platformVersion.string().c_str());
                    mDeviceModel += std::string("_") + platformVersion.string().c_str();
                }
            }
        }
    }

    // Verify storage files.
    verifyFile(mEncryptedFile);

    THREAD_MC_EVENT.SetPriority(80);
    THREAD_MC_PUMPING.SetPriority(80);

    // Only request main profile. Base Profile is for mobile devices
    // capability.videoProfiles.push_back(VIDEO_PLAYREADY_H264_BPL30_DASH);
    mVideoProfiles.push_back(VIDEO_PLAYREADY_H264_MPL30_DASH);
    mVideoProfiles.push_back(VIDEO_PLAYREADY_H264_MPL31_DASH);
    mVideoProfiles.push_back(VIDEO_PLAYREADY_H264_MPL40_DASH);
    mVideoProfiles.push_back(VIDEO_PLAYREADY_H264_SHPL31_DASH);
    mVideoProfiles.push_back(VIDEO_PLAYREADY_H264_SHPL40_DASH);
    mVideoProfiles.push_back(VIDEO_NONE_H264_MPL30_DASH);

#if defined(BUILD_VANGUARDHEVC_VIDEO_DECODER)
    if (sConfiguration->hevcEnabled) {

        /*
         * CE devices should not declare any HEVC L20 or L21 profiles
         */
        
        mVideoProfiles.push_back(HEVC_MAIN_L30_DASH_CENC);
        mVideoProfiles.push_back(HEVC_MAIN_L31_DASH_CENC);
        mVideoProfiles.push_back(HEVC_MAIN_L40_DASH_CENC);
        mVideoProfiles.push_back(HEVC_MAIN_L41_DASH_CENC);
        mVideoProfiles.push_back(HEVC_MAIN_L50_DASH_CENC);
        mVideoProfiles.push_back(HEVC_MAIN_L51_DASH_CENC);

        mVideoProfiles.push_back(HEVC_MAIN10_L30_DASH_CENC);
        mVideoProfiles.push_back(HEVC_MAIN10_L31_DASH_CENC);
        mVideoProfiles.push_back(HEVC_MAIN10_L40_DASH_CENC);
        mVideoProfiles.push_back(HEVC_MAIN10_L41_DASH_CENC);
        mVideoProfiles.push_back(HEVC_MAIN10_L50_DASH_CENC);
        mVideoProfiles.push_back(HEVC_MAIN10_L51_DASH_CENC);

        mVideoProfiles.push_back(HEVC_MAIN_L30_L31_DASH_CENC_TL);
        mVideoProfiles.push_back(HEVC_MAIN_L31_L40_DASH_CENC_TL);
        mVideoProfiles.push_back(HEVC_MAIN_L40_L41_DASH_CENC_TL);
        mVideoProfiles.push_back(HEVC_MAIN_L50_L51_DASH_CENC_TL);

        mVideoProfiles.push_back(HEVC_MAIN10_L30_L31_DASH_CENC_TL);
        mVideoProfiles.push_back(HEVC_MAIN10_L31_L40_DASH_CENC_TL);
        mVideoProfiles.push_back(HEVC_MAIN10_L40_L41_DASH_CENC_TL);
        mVideoProfiles.push_back(HEVC_MAIN10_L50_L51_DASH_CENC_TL);
#ifdef NRDP_HAS_HDR_VIDEO
        mVideoProfiles.push_back(HEVC_DV_MAIN10_L30_DASH_CENC);
        mVideoProfiles.push_back(HEVC_DV_MAIN10_L31_DASH_CENC);
        mVideoProfiles.push_back(HEVC_DV_MAIN10_L40_DASH_CENC);
        mVideoProfiles.push_back(HEVC_DV_MAIN10_L41_DASH_CENC);
        mVideoProfiles.push_back(HEVC_DV_MAIN10_L50_DASH_CENC);
        mVideoProfiles.push_back(HEVC_DV_MAIN10_L51_DASH_CENC);
#endif
    }
#endif

#ifdef REQUEST_HD_NON_DRM_PROFILES
    mVideoProfiles.push_back(VIDEO_NONE_H264_MPL31_DASH);
    mVideoProfiles.push_back(VIDEO_NONE_H264_MPL40_DASH);
#endif

    mAudioProfiles.push_back(AUDIO_HEAAC_2_DASH);
#ifdef HAVE_DDPLUS
    mAudioProfiles.push_back(AUDIO_DDPLUS_5_1_DASH);
# endif

    mAudioEaseProfiles.push_back(EASE_LINEAR);
    mAudioEaseProfiles.push_back(EASE_INCUBIC);
    mAudioEaseProfiles.push_back(EASE_OUTCUBIC);

    mAudioDecodeProfiles.push_back(AUDIO_HEAAC_2_DASH);
#ifdef HAVE_DDPLUS
    mAudioDecodeProfiles.push_back(AUDIO_DDPLUS_5_1_DASH);
# endif

    mQaBridgeIpConnectivityMode = 0;

    // supported video output
    VideoOutputInfo videoOutputInfo;
    videoOutputInfo.name = "TV Internal Display";
    videoOutputInfo.videoOutput = DIGITAL_INTERNAL;
    videoOutputInfo.hdcpVersion = HDCP_NOT_APPLICABLE;
    mSupportedVideoOutput.push_back(videoOutputInfo);

    // currently active video output
    VideoOutputState videoOutputState;
    videoOutputState.videoOutput = DIGITAL_INTERNAL;
    videoOutputState.hdcpVersion = HDCP_NOT_APPLICABLE;
    mActiveVideoOutput.push_back(videoOutputState);

#ifndef PLAYREADY
# warning "Building the reference DPI with no DRM support!"
#endif

    // Open appboot public key data file.
    ASSERT(!pubkeyFile.empty(), "No appboot public key data file specified.");
    ncf_kav = Configuration::resourceContent(pubkeyFile).decode(DataBuffer::Encoding_Base64);
    if (ncf_kav.empty()) {
        ncf_kav = DataBuffer::fromFile(pubkeyFile.c_str()).decode(DataBuffer::Encoding_Base64);
    }
    NRDP_OBJECTCOUNT_DESCRIBE(ncf_kav, "DPI_NCF_KAV");
    ASSERT(!ncf_kav.isEmpty(), "Cannot open %s for reading.", pubkeyFile.c_str());

    // video pipeline capability setting for dual video
    struct ISystem::VideoPipelineCapability primaryPipeline;
    primaryPipeline.isFullFeaturedPipeline = true;
    primaryPipeline.pipelineId = 1;
    primaryPipeline.maxHEVCCodecProfile = ISystem::HEVC_MAIN10_L51;
    primaryPipeline.maxAVCCodecProfile = ISystem::AVC_MAIN_L40;
    primaryPipeline.isDrmCapable = true;
    primaryPipeline.isFullPictureQuality = true;
    primaryPipeline.zOrder = ISystem::ZORDER_ANY;
    mVideoPipelineCapabilities.push_back(primaryPipeline);

    struct ISystem::VideoPipelineCapability secondaryPipeline;
    primaryPipeline.isFullFeaturedPipeline = true;
    secondaryPipeline.pipelineId = 2;
    secondaryPipeline.maxHEVCCodecProfile = ISystem::HEVC_MAIN10_L51;
    secondaryPipeline.maxAVCCodecProfile = ISystem::AVC_MAIN_L40;
    secondaryPipeline.isDrmCapable = true;
    secondaryPipeline.isFullPictureQuality = true;
    secondaryPipeline.zOrder = ISystem::ZORDER_ANY;
    mVideoPipelineCapabilities.push_back(secondaryPipeline);

}

FileSystem::~FileSystem()
{
    // make sure the static DataBuffers are destroyed before ObjectCount::sMutex
    // from static destructors.
    ncf_kpe.clear();
    ncf_kph.clear();
    ncf_kav.clear();
}

void FileSystem::init(shared_ptr<EventDispatcher> eventDispatcher)
{
    mEventDispatcher = eventDispatcher;
    assert(!mNwMon.get());
    mNwMon.reset(new NetworkMonitor(this));
    Application::instance()->startTimer(mNwMon);
    mNwMon->readNetwork();

    shared_ptr<NfObject> nrdpBridge = nrdApp()->findObject("nrdp");

#ifdef BUILD_QA
    assert(nrdpBridge);
    assert(nrdpBridge->child("qa"));
    nrdpBridge->child("qa")->addChild(shared_ptr<NfObject>(new QADeviceBridge(static_pointer_cast<FileSystem>(shared_from_this()))));
#endif

#ifdef BUILD_VOICE
    nrdApp()->nrdpBridge()->addChild(shared_ptr<NfObject>(new VoiceBridge()));
#endif

    if (sConfiguration->enableSSO) {
        nrdpBridge->addChild(shared_ptr<NfObject>(new SSOBridge()));
    }

    if (sConfiguration->enableSignup) {
        shared_ptr<SignupBridge> bridge(new SignupBridge());
        nrdpBridge->addChild(bridge);
        bridge->init();
    }
}

void FileSystem::shutdown()
{
    mEventDispatcher.reset();
}

PresharedFileSystem::PresharedFileSystem(const std::string& idFile,
                                         const std::string& pubkeyFile,
                                         const std::string& encryptedFile)
  : FileSystem(encryptedFile, pubkeyFile)
{
    mAuthType = PRESHARED_KEYS;

    // Open individualization data file.
    ASSERT(!idFile.empty(), "No individualization data file specified.");
    std::ifstream idData(idFile.c_str());
    idData.imbue(std::locale::classic());
    ASSERT(idData, "Cannot open %s for reading.", idFile.c_str());

    // Load individualization data.
    std::string kpe, kph;
    idData >> mEsn;
    idData >> kpe;
    idData >> kph;

    std::string deviceModel;
    idData >> deviceModel;
    if(!deviceModel.empty()){
        mDeviceModel = deviceModel;
    }

    ncf_kpe = Base64::decode(kpe);
    NRDP_OBJECTCOUNT_DESCRIBE(ncf_kpe, "DPI_NCF_KPE");
    ncf_kph = Base64::decode(kph);
    NRDP_OBJECTCOUNT_DESCRIBE(ncf_kph, "DPI_NCF_KPH");
}

ModelGroupFileSystem::ModelGroupFileSystem(const std::string& idFile,
                                           const std::string& pubkeyFile,
                                           const std::string& encryptedFile)
    : FileSystem(encryptedFile, pubkeyFile)
{
    // Model group keys work the same way as pre-shared, so we still
    // assign to ncf_kpe and ncf_kph. Only the advertised
    // authentication type is different.
    mAuthType = MODEL_GROUP_KEYS;

    // Open individualization data file.
    ASSERT(!idFile.empty(), "No individualization data file specified.");
    std::ifstream idData(idFile.c_str());
    idData.imbue(std::locale::classic());
    ASSERT(idData, "Cannot open %s for reading.", idFile.c_str());

    // Load individualization data.
    std::string kpe, kph;
    idData >> mEsn;
    idData >> kpe;
    idData >> kph;

    std::string deviceModel;
    idData >> deviceModel;
    if(!deviceModel.empty()){
        mDeviceModel = deviceModel;
    }

    ncf_kpe = Base64::decode(kpe);
    ncf_kph = Base64::decode(kph);
    NRDP_OBJECTCOUNT_DESCRIBE(ncf_kpe, "DPI_NCF_KPE");
    NRDP_OBJECTCOUNT_DESCRIBE(ncf_kph, "DPI_NCF_KPH");
}

bool FileSystem::isScreensaverOn() const
{
    // FIXME: When we have a screen saver.
    return mScreensaverOn;
}

void FileSystem::setRegistered(bool registered)
{
    static std::string activatedFile =
             (Configuration::dataWritePath() + "/nrdapp/activated");

    if (registered)
    {
        int fd = creat(activatedFile.c_str(), 0644);
        if( fd != -1 ) close(fd);
    }
    else
        unlink(activatedFile.c_str());
}

void FileSystem::storeEncrypted(const std::map<std::string,std::string>& data)
{
    // clear out values in our RAM map so that the map doesn't grow with stale
    // values.
    mLastMap = data;
}

std::map<std::string, std::string> FileSystem::loadEncrypted()
{
#ifdef MIGRATE_40_TO_41_SECURESTORE
    // needToMigrateSecureStore() must be implemented properly
    if ( needToMigrateSecureStore() ) {
        mLastMap = convertSecureStore(mEncryptedFile.c_str());
        flushEncrypted();
    }
#endif
    std::map<std::string,std::string> pairs;
    std::ifstream is(mEncryptedFile.c_str(), std::ios::in | std::ios::binary);
    if (!is.is_open()){
        return pairs;
    }
    do
    {
        int keySize=0;
        is.read((char *)(&keySize), sizeof(int));
        DataBuffer keyEnc(keySize);
        is.read((char *)(keyEnc.data()), keySize);
        int valueSize=0;
        is.read((char *)(&valueSize), sizeof(int));
        DataBuffer valueEnc(valueSize);
        is.read((char*)(valueEnc.data()), valueSize);

        if (!is) {
            break;
        }

        if (teeStoragePtr_){
            // decrypt the key value pair first
            DataBuffer keyClear;
            DataBuffer valueClear;

            if ((NFErr_OK == teeStoragePtr_->loadProtected(keyEnc, keyClear)) &&
                (NFErr_OK == teeStoragePtr_->loadProtected(valueEnc, valueClear))){
                NTRACE(TRACE_DPI, "FileSystem::loadEncrypted() using TEE key %s, value %s",
                       keyClear.c_str(), valueClear.c_str());
                pairs[keyClear.toString()] = valueClear.toString();
            }
        }else{
            NTRACE(TRACE_DPI, "FileSystem::loadEncrypted() using TEE key %s, value %s",
                   keyEnc.toString().c_str(), valueEnc.toString().c_str());
            pairs[keyEnc.toString()] = valueEnc.toString();
        }
    } while (true);
    return pairs;
}

const std::string FileSystem::getLanguage() const
{
    return sConfiguration->language;
}

/**
 * NOTE TO PARTNERS:
 *
 * If you are using FileSystem::getCapability() as a starting point for your
 * DPI implementation, please ensure that these values are correct for your
 * specific device.
 *
 * sConfiguration is defined in DeviceLibNative.cpp and its purpose is to
 * allow options to be overridden on the command line.  You may not need
 * this capability in your application, in which case the proper values for
 * the configuration parameters can just be hardcoded here.  In any case,
 * be aware that the default values taken from sConfiguration will probably
 * not be correct for your device, and must be changed.
 */
ISystem::Capability FileSystem::getCapability() const
{
    Capability capability;
    capability.hasPointer = sConfiguration->hasPointer; // "true" if the device has a pointing device connected
    capability.hasKeyboard = sConfiguration->hasKeyboard; // "true" if the device has a keyboard connected
    capability.hasOnScreenKeyboard = sConfiguration->hasOnScreenKeyboard; // "true" if the device provides a built-in on-screen keyboard for text input
    capability.hasSuspend = sConfiguration->hasSuspend; // "true" if the device supports suspend mode
    capability.hasBackground = sConfiguration->hasBackground; // "true" if the device supports background mode
    capability.hasScreensaver = sConfiguration->hasScreensaver; // "true" if the device has a native screen saver

    capability.supportFrameRateSeamlessSwitch = sConfiguration->supportFrameRateSeamlessSwitch;

    capability.tcpReceiveBufferSize = sConfiguration->tcpReceiveBufferSize;

    capability.videoBufferPoolSize = sConfiguration->videoBufferPoolSize;
    capability.audioBufferPoolSize = sConfiguration->audioBufferPoolSize;

    capability.support2DVideoResize =
        sConfiguration->support2DVideoResize; // "true" if the device allows the
                                              // video window to be resized

    capability.support2DVideoResizeAnimation =         // "true" if the device can
        sConfiguration->support2DVideoResizeAnimation; // resize the video window
                                                       // at least 5 times
                                                       // a second.
    capability.supportDrmPlaybackTransition =
        sConfiguration->supportDrmPlaybackTransition;

    capability.supportDrmToDrmTransition = sConfiguration->supportDrmToDrmTransition;
    
    capability.supportMultipleDrmDecryption =
        sConfiguration->supportMultipleDrmDecryption;

    capability.supportLimitedDurationLicenses = sConfiguration->supportLimitedDurationLicenses;

    capability.supportSecureStop = sConfiguration->supportSecureStop;

    capability.supportDrmStorageDeletion = sConfiguration->supportDrmStorageDeletion;

    capability.videoProfiles = mVideoProfiles;
    capability.audioProfiles = mAudioProfiles;
    capability.audioDecodeProfiles = mAudioDecodeProfiles;
    capability.audioEaseProfiles = mAudioEaseProfiles;

    capability.supportOnTheFlyAudioSwitch = sConfiguration->supportOnTheFlyAudioSwitch;
    capability.supportOnTheFlyAudioCodecSwitch = sConfiguration->supportOnTheFlyAudioCodecSwitch;

    capability.supportDolbyVisionComposerControl = sConfiguration->supportDolbyVisionComposerControl;
    capability.supportDynamicVideoPipelines = sConfiguration->supportDynamicVideoPipelines;

    capability.minAudioPtsGap = sConfiguration->minAudioPtsGap;
    return capability;
}

uint32_t FileSystem::getCertificationVersion() const
{
    return 0;
}

const std::string FileSystem::getEsn() const
{
    return mEsn;
}

void FileSystem::setEsn(std::string esn)
{
    mEsn=esn;
}

bool FileSystem::getDeviceModel(std::string &deviceModel) const
{
    deviceModel = mDeviceModel;
    NTRACE(TRACE_DPI, "FillSystem::getDeviceModel() %s", deviceModel.c_str());
    return true;
}

const std::string FileSystem::getFriendlyName() const
{
    return sConfiguration->friendlyName;
}

const std::string FileSystem::getSoftwareVersion() const
{
    static const std::string version("Netflix Reference DPI Implementation");
    return version;
}

ISystem::AuthenticationType FileSystem::getAuthenticationType() const
{
    return mAuthType;
}

void FileSystem::setAuthenticationType(ISystem::AuthenticationType authType)
{
    mAuthType = authType;

}

void FileSystem::getVideoOutputResolution(uint32_t &width, uint32_t &height) const
{
    width  = sConfiguration->videoRendererScreenWidth;
    height = sConfiguration->videoRendererScreenHeight;
}

void FileSystem::notifyApplicationState(netflix::device::ISystem::ApplicationState)
{
}

std::map<std::string, std::string> FileSystem::getStartupLogTags()
{
    return std::map<std::string, std::string>();
}

void FileSystem::reseedSslEntropy()
{
    int retVal;

    unsigned char entropyData[OpenSSLLib::MIN_SEED_LEN];

    if(FILE *entropyFile = fopen( "/dev/urandom", "r" )) {
        retVal = fread( entropyData, 1, OpenSSLLib::MIN_SEED_LEN, entropyFile );
        if (retVal > 0)
            OpenSSLLib::add_entropy( entropyData, retVal, 0.0 );
        fclose( entropyFile );
    }

    return;
}

/** Get the platform preferred IP connectivity mode. */
IpConnectivityMode FileSystem::getIpConnectivityMode()
{
#ifdef NRDP_HAS_IPV6

    //return IP_V6_ONLY_MODE;

    // To test app behavior from app launch, command line has priority
    if (sConfiguration->ipConnectivityMode == 4) {
        return IP_V4_ONLY_MODE;
    } else if (sConfiguration->ipConnectivityMode == 6) {
        return IP_V6_ONLY_MODE;
    } else if (sConfiguration->ipConnectivityMode == 10) {
        return IP_DUAL_STACK_MODE;
    }

    // if sConfiguration->ipConnectivityMode is not specified by command line
    // (value 0), honor QABridge value
    if (mQaBridgeIpConnectivityMode == 4)
    {
        return IP_V4_ONLY_MODE;
    }
    else if (mQaBridgeIpConnectivityMode == 6)
    {
        return IP_V6_ONLY_MODE;
    }
    else if (mQaBridgeIpConnectivityMode == 10)
    {
        return IP_DUAL_STACK_MODE;
    }
    else
    {
        return mIpConnectivity;
    }

#else
     return IP_V4_ONLY_MODE;
#endif
}

std::vector<std::string> FileSystem::getDNSList()
{
    std::vector<std::string> dnslist;
    FILE *f = fopen("/etc/resolv.conf", "r");
    if (f) {
        char line[1024];
        while ((fgets(line, sizeof(line), f))) {
            if (!strncmp("nameserver", line, 10)) {
                char *ch = line + 10;
                if (isspace(*ch++)) {
                    while (isspace(*ch))
                        ++ch;
                    if (*ch) {
                        char *start = ch;
                        while (!isspace(*ch))
                            ++ch;
                        dnslist.push_back(std::string(start, ch - start));
                    }
                }
            }
        }
        fclose(f);
    }
    return dnslist;
}

#ifdef NRDP_HAS_TRACING
static std::string networkInterfacesToString(const std::vector<FileSystem::NetworkInterface> &interfaces)
{
    std::ostringstream str;
    for (std::vector<FileSystem::NetworkInterface>::const_iterator it = interfaces.begin(); it != interfaces.end(); ++it) {
        if (it != interfaces.begin())
            str << '\n';
        str << "Name: " << it->name
            << " PType: " << it->physicalLayerType
            << " PSubType: " << it->physicalLayerSubtype
            << " IP: " << it->ipAddress
            << " IPV6: " << StringTokenizer::join(it->ipv6Addresses, ", ")
            << " HWaddr: " << it->macAddress
            << " SSID: " << it->ssid
            << " WifiChannel: " << it->wirelessChannel
            << " Carrier: " << it->mobileCarrier << " (" << it->mobileCountryCode << ", " << it->mobileNetworkCode << ")"
            << " Link: " << it->linkConnected
            << " Internet: " << it->internetConnected;
        if (!it->additionalInfo.empty())
            str << " Additional: " << it->additionalInfo;
        if (it->isDefault)
            str << " (Default)";
    }
    return str.str();
}
#endif

bool FileSystem::NetworkMonitor::readNetwork()
{
    char *str1;
    unsigned int ipmap = 0x0;  // bit 0 = valid ipv4  bit 1 = valid ipv6

    std::vector<NetworkInterface > *pnif = &(this->m_nif);
    std::vector<NetworkInterface > nif_test;
    nif_test = m_sys->getNetworkInterfaces();
    if(pnif->empty())
        *pnif=nif_test;

    //Check IP connectivity flag see if there is any change
    for (std::vector<ISystem::NetworkInterface>::iterator  i = nif_test.begin();
         i != nif_test.end();
         i++)
    {
        if(i->isDefault){
            char tmp[256];
            memset(tmp,0,256);
            if(!i->ipAddress.empty()) {
                strcpy(tmp,(char*)(i->ipAddress.c_str()));
                str1 =strtok(tmp,".");
                if(!strcmp(str1,"127") || !strcmp(str1,"169"))
                    ipmap = ipmap | 0x00;
                else
                    ipmap = ipmap | 0x01;
            }
            else  //empty IP, mark as invalid Ipv4
                ipmap = ipmap | 0x00;
            std::vector<std::string> v = i->ipv6Addresses;
            for(std::vector<std::string>::iterator x = v.begin();
                x!= v.end(); x++)
            {
                memset(tmp,0,256);
                strcpy(tmp,(char*)(x->c_str()));
                str1 =strtok(tmp,":");
                if(!strcmp(str1,"") || !strcmp(str1,"fe80"))
                    ipmap = ipmap | 0x00;
                else
                    ipmap = ipmap | 0x02;
            }
        }
    }
    //Process the ipconnectivity flags
    // NTRACE(TRACE_DPI, "\n ==Connectivity mode: 0x%x == \n", ipmap );
    if(ipmap == 0x01)
        m_sys->mIpConnectivity = IP_V4_ONLY_MODE;     //ipv4
    else if(ipmap == 0x02)
        m_sys->mIpConnectivity = IP_V6_ONLY_MODE;     // ipv6
    else if (ipmap == 0x03)
        m_sys->mIpConnectivity = IP_DUAL_STACK_MODE;  //dual mode
    else
        m_sys->mIpConnectivity = IP_DUAL_STACK_MODE;  //default

    // see if there was a change in the DNS server list
    std::vector<std::string> dnslist =m_sys->getDNSList();
    bool update = false;
    if( dnslist.size() != mDnsList.size() ) update = true;
    if( !update )
    {
        // lists are the same size but may have different entries
        // Using an n squared algorithm assuming a small number
        // of DNS servers.
        std::vector<std::string>::iterator i;
        for ( i = dnslist.begin(); i != dnslist.end(); i++)
        {
            std::vector<std::string>::iterator j;
            for ( j = mDnsList.begin(); j != mDnsList.end(); j++)
            {
                if( (*i).compare((*j)) == 0 ) break; // found it!
            }
            if( j == mDnsList.end() )
            {
                // we didn't find this entry in mDnsList,
                // lists are different
                update = true;
                break;
            }
        }
    }
    if( update ) {
        NTRACE(TRACE_DPI, "DNS list changed");
    }

    size_t size = pnif->size();
    for (size_t i=0;!update && i<size && i<nif_test.size();i++) {
//          NTRACE(TRACE_DPI, "\n\n\n [IF %s %s]\n\n",nif_test[i].ipAddress.c_str(),(*pnif)[i].ipAddress.c_str());
        if((*pnif)[i].name !=nif_test[i].name){
            NTRACE(TRACE_DPI, "\n [IF name changed]");
            update = true;
        } else if((*pnif)[i].ipAddress !=nif_test[i].ipAddress){
            NTRACE(TRACE_DPI, "\n [IF IP changed] interface %s",nif_test[i].name.c_str());
            update = true;
        } else if((*pnif)[i].ipv6Addresses !=nif_test[i].ipv6Addresses) {
            NTRACE(TRACE_DPI, "\n [IF IPv6 changed] interface %s",nif_test[i].name.c_str());
            update = true;
        } else if((*pnif)[i].ssid !=nif_test[i].ssid){
            NTRACE(TRACE_DPI, "\n [IF SSID changed] interface %s",nif_test[i].name.c_str());
            update = true;
        } else if((*pnif)[i].wirelessChannel !=nif_test[i].wirelessChannel){
            NTRACE(TRACE_DPI, "\n [IF WIRELESS channel changed] interface %s",nif_test[i].name.c_str());
            update = true;
        } else if((*pnif)[i].macAddress !=nif_test[i].macAddress){
            NTRACE(TRACE_DPI, "\n [IF MAC changed] interface %s",nif_test[i].name.c_str());
            update = true;
        } else if((*pnif)[i].isDefault !=nif_test[i].isDefault){
            NTRACE(TRACE_DPI, "\n [IF Default changed] interface %s",nif_test[i].name.c_str());
            update = true;
        } else if((*pnif)[i].linkConnected !=nif_test[i].linkConnected){
            NTRACE(TRACE_DPI, "\n [IF PHY Connect changed] interface %s",nif_test[i].name.c_str());
            update = true;
        } else if((*pnif)[i].internetConnected !=nif_test[i].internetConnected){
            NTRACE(TRACE_DPI, "\n [IF Internet Connect changed] interface %s",nif_test[i].name.c_str());
            update = true;
        } else {
            //NTRACE(TRACE_DPI, "\n [IF NO changed]");
        }
    }
    if (update) {
        NTRACE(TRACE_DPI, "Network changed DNS:\n%s\n=>\n%s\nIFace:\n%s\n=>\n%s",
               StringTokenizer::join(mDnsList, ", ").c_str(), StringTokenizer::join(dnslist, ", ").c_str(),
               networkInterfacesToString(*pnif).c_str(), networkInterfacesToString(nif_test).c_str());

        *pnif=nif_test;
        mDnsList = dnslist;
    }
    return update;
}

void FileSystem::NetworkMonitor::timerFired()
{
    if (readNetwork())
        m_sys->mEventDispatcher->networkChanged();
}

/**
 * The reference app only supports 2D view mode.
 */
ViewModeType FileSystem::getCurrentViewMode() const
{
    return UNKNOWN_VIEWMODE_TYPE;
}

/**
 * The reference app only supports 2D view mode.
 */
std::vector<ViewModeType> FileSystem::getAvailableViewModes() const
{
    std::vector<ViewModeType> types;
    types.push_back(UNKNOWN_VIEWMODE_TYPE);
    return types;
}

/**
 * The reference app only supports 2D view mode.
 */
void FileSystem::setViewMode(ViewModeType& viewMode)
{
    NRDP_UNUSED(viewMode);
}

VolumeControlType FileSystem::getVolumeControlType()
{
    return mVolumeControlType;
}

double FileSystem::getVolume()
{
    return 1;
}

void FileSystem::setVolume(double)
{
    mEventDispatcher->outputVolumeChanged();
}

double FileSystem::getCurrentMinimumVolumeStep()
{
    return 0.001;
}

bool FileSystem::isMuted()
{
    return false;
}

void FileSystem::setMute(bool)
{
    mEventDispatcher->outputVolumeChanged();
}

void FileSystem::flushEncrypted()
{
    std::ofstream os(mEncryptedFile.c_str(), std::ios::out | std::ios::binary);
    if (!os) return;
    os.imbue(std::locale::classic());
    typedef std::map<std::string,std::string>::const_iterator MSSCI;
    for (MSSCI pair = mLastMap.begin(); pair != mLastMap.end(); ++pair)
    {
        if (teeStoragePtr_){
            // encrypte the data first
            DataBuffer keyClr((*pair).first);
            DataBuffer key;
            DataBuffer valueClr((*pair).second);
            DataBuffer value;
            if ((NFErr_OK == teeStoragePtr_->storeProtected(keyClr, key)) &&
                (NFErr_OK == teeStoragePtr_->storeProtected(valueClr, value))){
                NTRACE(TRACE_DPI, "FileSystem::flushEncrypted() using TEE key %s, value %s\n encKey %s encValue %s",
                       (*pair).first.c_str(), (*pair).second.c_str(),
                       key.toString().c_str(), value.toString().c_str());
                // write data buffer as binary
                // write key size
                int keySize = key.size();
                os.write((const char*)(&keySize), sizeof(int));
                // write key
                os.write((const char*)(key.data()), keySize);
                // write value size
                int valueSize = value.size();
                os.write((const char*)(&valueSize), sizeof(int));
                // write value
                os.write((const char*)(value.data()), valueSize);
            }
        }else{
            NTRACE(TRACE_DPI, "FileSystem::flushEncrypted() using TEE key %s, value %s\n",
                   (*pair).first.c_str(), (*pair).second.c_str());
            // write directly to file
            int keySize = (*pair).first.length();
            int valueSize = (*pair).second.length();
            os.write((const char*)(&keySize), sizeof(int));
            os.write((const char*)((*pair).first.c_str()), keySize);
            os.write((const char*)(&valueSize), sizeof(int));
            os.write((const char*)((*pair).second.c_str()), valueSize);
        }
    }
}

/**
 * Device Display Status
 */
std::vector<struct ISystem::VideoOutputInfo> FileSystem::getSupportedVideoOutput()
{
    return mSupportedVideoOutput;
}


std::vector<struct ISystem::VideoOutputState> FileSystem::getActiveVideoOutput()
{
    return mActiveVideoOutput;
}


void FileSystem::getDisplaySize(uint32_t& widthInCm, uint32_t& heightInCm)
{
    widthInCm = 160;
    heightInCm = 90;
}

#ifdef __linux__
static int dl_iterate_phdr_callback(struct dl_phdr_info *info, size_t size, void *data)
{
    (void)size;
    (void)data;

    if (info->dlpi_name[0]) {
        mSharedObjects.insert(info->dlpi_name);
    }
    return 0;
}
#endif

Variant FileSystem::getSignatures()
{
    Variant map;

#ifdef __linux__

    Dl_info info;
    dladdr((const void *)&dl_iterate_phdr_callback, &info);

    mSharedObjects.insert(info.dli_fname);

    dl_iterate_phdr(dl_iterate_phdr_callback, NULL);

    for (std::set<std::string>::iterator it = mSharedObjects.begin(); it != mSharedObjects.end(); ++it) {
        struct stat buf;
        buf.st_size = 0xDEADBEEF;
        uint32_t crc = 0xDEADBEEF;

        if (!stat(it->c_str(), &buf)) {
            int fd;
            if ((fd = open(it->c_str(), O_RDONLY)) != -1) {
                Bytef * p = (Bytef*)mmap(NULL, buf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
                crc = crc32(0L, Z_NULL, 0);
                crc = crc32(crc, p, buf.st_size);
                munmap(p, buf.st_size);
                close(fd);
            }
        }

        Variant propmap;
        propmap["size"] =  buf.st_size;
        propmap["crc32"] = crc;

        map[it->c_str()] = propmap;
    }
#endif

    return map;
}

void FileSystem::setTeeStorage(shared_ptr<TeeApiStorage> teeStoragePtr)
{
    teeStoragePtr_=teeStoragePtr;
}

#ifdef NRDP_APPLICATION_MANAGER
extern void ApplicationManagerRequestSuspend(void);
extern void ApplicationManagerRequestBackground(void);
#endif

void FileSystem::requestSuspend(const std::string &reason)
{
#ifdef NRDP_APPLICATION_MANAGER
    // Partners should hook here when the UI requests to move into either background
    // or suspend state.  If the partner application manager doesn't support background
    // mode then they should use suspend mode.  This example will ignore the "reason"
    // and always request the application manager suspend Netflix.

    if (reason == "background") {
        ApplicationManagerRequestBackground();
    } else {
        ApplicationManagerRequestSuspend();
    }
#else
    nrdApp()->setSuspended(true, reason);
#endif
}

Variant FileSystem::getSystemValue(const std::string& key)
{
    return SystemValues::getSystemValue(key);
}

Variant FileSystem::getReport()
{
    Variant report;
    std::string reason;
    std::string dump;

    const char * reportFile = "/tmp/netflix_report";

    do {
        std::string tmp;

        std::ifstream is(reportFile);
        getline(is, reason); if (!is) break;
        getline(is, dump); if (!is) break;

        do {
            getline(is, tmp); if (!is) break;
            dump.append("\n");
            dump.append(tmp);
        } while (1);

        report["crashCode"] = SIGKILL;
        report["reason"] = reason;
        report["dump"] = dump;
    } while (0);

    unlink(reportFile);

    return report;
}

void FileSystem::getUptime(uint32_t & uptime, uint32_t & ontime)
{
    // Reference x86 implementation doesn't have a real application manager
    // so the best we can do is use Linux methods and use the power manager
    // messages in the kernel log.

    const char * powerFile = "/sys/power/wakeup_count";
    const char * uptimeFile = "/proc/uptime";
    const char * kernlogFile = "/var/log/kern.log";
    const char * wakemsg = "PM: Finishing wakeup.";
    const char * kernmsg = "kernel: [";

    std::string tmp;

    std::ifstream us(uptimeFile);
    getline(us, tmp);
    uptime = atoi(tmp.c_str());

    std::ifstream is(powerFile);
    getline(is, tmp);

    if (tmp == "0") {
        // Device has never slept so ontime is Linux uptime
        ontime = uptime;
    } else {
        // Device has slept so find last PM wakeup message
        uint32_t eventTime = 0;
        std::ifstream file(kernlogFile);
        std::string str;
        while (std::getline(file, str)) {
            const char * a = strstr(str.c_str(), wakemsg);
            if (!a) continue;
            a = strstr(str.c_str(), kernmsg);
            if (!a) continue;
            a += strlen(kernmsg);
            eventTime = atoi(a);
        }
        ontime = uptime - eventTime;
    }
}

