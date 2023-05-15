/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "DeviceLibNative.h"

#include <cassert>
#include <fstream>

#include <nrdbase/Assert.h>
#include <nrdbase/Application.h>
#include <nrdbase/TraceAreas.h>
#include <nrdbase/ReadDir.h>
#include <nrddpi/WebCryptoTee.h>

#include <nrd/AppLog.h>

#if defined(PLAYREADY)
# include "PlayReadyDrmSystem.h"
#endif

#include "BufferManagerNative.h"
#include "FileSystem.h"
#include "ESManagerNative.h"
#include "ManufSS.h"

#ifdef AUDIO_MIXER_SOFTWARE
#include <nrddpi/AudioMixerSoftware.h>
#ifdef AUDIO_MIXER_OUTPUT_ALSA
#include "x86/AudioMixerOutputALSA.h"
#endif
#ifdef AUDIO_MIXER_OUTPUT_AUDIOQUEUE
#include "x86/AudioMixerOutputAudioQueue.h"
#endif
#ifdef AUDIO_MIXER_OUTPUT_SINK
#include "sink/AudioMixerOutputSink.h"
#endif
#endif

#ifdef AUDIO_MIXER_NATIVE
#include "x86/AudioMixerNative.h"
#endif

using namespace std;
using namespace netflix;
using namespace netflix::device;


// Allocated in the extern function getDeviceLibOptions() and freed in
// ~DeviceLibNative(). Stores the reference app's settings for the ISystem
// capability parameters. Also stores a few reference-app-specific configuration
// parameters.
//
// Used to allow netflix-internal QA to run the netflix reference application
// with command-line options.  Device-specific implementations won't necessarily
// need this.
namespace netflix {
namespace device {
referenceDpiConfig* sConfiguration = 0;
}}

// This is the name of the attribute that identifies the checksum.
static string const CHECKSUM_ATTRIBUTE = "CHECKSUM";

// Helper function used when migrating older encrypted system data files to the
// newer encrypted format.
static void removeObsoleteKeys(map<string, string> &params)
{
    static const char *s_obsoleteKeys[] =
        {
            "MC/SDS",
            "MC/PDR",
            "MC/PFR",
            "MC/LNC",
            "Util/sF",
            NULL
        };

    map<string, string>::iterator it = params.begin();
    while (it != params.end())
    {
        bool found = false;
        for (size_t i=0; s_obsoleteKeys[i] != NULL; ++i)
        {
            if (it->first.find(string(s_obsoleteKeys[i])) != string::npos)
            {
                found = true;
                break;
            }
        }

        if (found)
            params.erase(it++);
        else
            ++it;
    }
}

// Earlier nrdapp versions used an unencrypted system data file.  If an old
// unencrypted data file exists on the device, the DeviceLibNative constructor
// uses this function to migrate it to the newer secure system data format.
static void migrateSystemData(const string &encryptedFile,
                              const string &nonEncryptedFile)
{
    // check for nonEncryptedFile
    ifstream is(nonEncryptedFile.c_str());
    if (!is.fail())
    {
        // migrate data from nonEncrypted to encryptedFile
        shared_ptr<ISystem> system(new FileSystem(encryptedFile));

        // read in existing encrypted pairs
        map<string, string> params(system->loadEncrypted());

        // read in nonencrypted pairs
        while (true)
        {
            string key, value;
            getline(is, key); if (!is) break;
            getline(is, value); if (!is) break;

            params[key] = value;
        }
        is.close();

        // erase any obsolete keys
        removeObsoleteKeys(params);

        // set checksum of new map to backdoor checksum
        params[CHECKSUM_ATTRIBUTE] = CHECKSUM_ATTRIBUTE;

        // store the map
        system->storeEncrypted(params);

        // remove the old non encrypted file
        remove(nonEncryptedFile.c_str());
    }
}


//
// The NrdLib creates a single instance of the device-specific implementation of
// IDeviceLib using the extern "C" call createDeviceLib()
// defined below. The createDeviceLib() call occurs after the
// getDeviceLibOptions() call that instantiates the sConfiguration stuct
// containing parameters referenced in the DeviceLibNative constructor.
DeviceLibNative::DeviceLibNative() :
    mutex_(DEVICELIB_MUTEX, "DeviceLibNative")
{
    string dataDir = Configuration::dataWritePath();
    NTRACE(TRACE_DPI, "\nNetflix reference DPI configuration\n"
           "Data directory path: %s\n\n"
           "%s",
           dataDir.c_str(),
           sConfiguration->toString().c_str());

    string nonEncryptedFile = dataDir + "/nrd/systemData";
    string encryptedFile = nonEncryptedFile + ".secure";
    string modelName = "";

    // Earlier versions used an uncrypted system data file.  If one exists, we
    // encrypt the data and move it to the newer secure system data file.
    migrateSystemData(encryptedFile, nonEncryptedFile);

    //If present, read idfile content
    DataBuffer fileContent;

    //
    // This code will get stripped out on SDK releases and won't be seen by partners
    //

    sConfiguration->mgk = true;
    if (sConfiguration->idFile.empty())
    {
        Log::warn(TRACE_DPI, "No individualization data file specified.");

        std::string esn = "NFDEV-MGK-";
        esn += DataBuffer(getenv("USER")).toHex().c_str();

        const string fakeIdFilePath = dataDir + "/fakeIdFile.tmp";
        ReadDir::recursiveMakeDirectory(dataDir);
        std::ofstream fakeIdFile(fakeIdFilePath.c_str(), std::ofstream::out | std::ofstream::trunc);
        fakeIdFile.imbue(std::locale::classic());
        fakeIdFile << esn << std::endl;
        fakeIdFile << "AAAAAAAAAAAAAAAAAAAAAA==" << std::endl;
        fakeIdFile << "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA=" << std::endl;
        fakeIdFile.close();
        sConfiguration->idFile = fakeIdFilePath;
        sConfiguration->mgk = false;
    }

    bool ok;
    fileContent = DataBuffer::fromFile(sConfiguration->idFile, -1, &ok);

    if (!ok) {
        Log::error(TRACE_DPI, "Can't read id file: %s", sConfiguration->idFile.c_str());
    }

    bool isIdFile = (!fileContent.isBinary(-1));
    Log::warn(TRACE_DPI, "IDFILE CONFIG specified \"%s\" file", (isIdFile ? "individualization" : "manufacturing secure store"));

    if (isIdFile)
    {
        /* TODO: WTF: reading the file again?! */
        //read the model name from 4th line (optional) of idfile
        std::ifstream modelData(sConfiguration->idFile.c_str());
        if (modelData)
        {
            modelData.imbue(std::locale::classic());
            // ignore first 3 lines which are esn, kpe, kph
            string temp;
            for (int i=0; i<3; ++i)
                getline(modelData, temp);

            //if 4th line exists then read it into modelName
            if (false == modelData.eof())
                modelData >> modelName;
            modelData.close();
        }

        // generate manufacturing secure store contents
        string drmDir = Configuration::dataReadPath() + "/dpi/playready/";
        int ret_val = ManufSS::createContent(sConfiguration->idFile, drmDir, fileContent);
        if (ret_val != 0){
            Log::error(TRACE_CRYPTO) << "Could not generate manufacturing secure store contents, got error " << ret_val;
            assert(0);
        }
    }


    // allocate and initialize TEE and create a system object with TEE parameters
    teeCryptoPtr_.reset(new TeeApiCrypto());

    //find if TRACE_TEE is enabled and initialize TEE debug flags accordingly
    uint32_t teeDebugFlag = 0;
    if (app()) {
        teeDebugFlag = (app()->traceAreas()->isEnabled(TraceArea::find("TEE"))) ? 0xFFFFFFFF : 0x00000000;
    }

    teeMgmtPtr_.reset(new TeeApiMgmt(teeDebugFlag));

    NTRACE(TRACE_CRYPTO, "TEE: initializing");
    NFErr err = teeMgmtPtr_->initTee(fileContent);
    if (err != NFErr_OK){
        Log::error(TRACE_CRYPTO) << "Aborting since we could not initialize TEE. Error: " << err.toString();
        exit(0);
    }else{
        NTRACE(TRACE_CRYPTO, "successfully initialized TEE");
    }

    // creating system object
    theSystem_.reset(new FileSystem(encryptedFile,
                                    sConfiguration->pubkeyFile,
                                    teeCryptoPtr_->getEsn(),
                                    modelName));
    theSystem_->setAuthenticationType(sConfiguration->mgk ? ISystem::MODEL_GROUP_KEYS : ISystem::UNKNOWN);

    teeStoragePtr_.reset(new TeeApiStorage());
    err = teeStoragePtr_->init((uint32_t) theSystem_->getSecureStoreSize());
    if (err != NFErr_OK) {
        Log::fatal(TRACE_CRYPTO) << "Failed to initialize TEE API storage instance: " << err.toString();
        exit(0);
    }

    if (teeStoragePtr_)
        theSystem_->setTeeStorage(teeStoragePtr_);

    theBufferManager_.reset(new BufferManagerNative(
                                sConfiguration->audioBufferPoolSize,
                                sConfiguration->videoBufferPoolSize));

    theElementaryStreamManager_.reset(new esplayer::ESManagerNative);

#ifdef AUDIO_MIXER_SOFTWARE
#ifdef AUDIO_MIXER_OUTPUT_ALSA
    theMixerOutput_.reset(new AudioMixerOutputALSA);
    theMixer_.reset(new AudioMixerSoftware(theMixerOutput_));
#endif

#ifdef AUDIO_MIXER_OUTPUT_AUDIOQUEUE
    theMixerOutput_.reset(new AudioMixerOutputAudioQueue);
    theMixer_.reset(new AudioMixerSoftware(theMixerOutput_));
#endif

#ifdef AUDIO_MIXER_OUTPUT_SINK
    theMixerOutput_.reset(new AudioMixerOutputSink);
    theMixer_.reset(new AudioMixerSoftware(theMixerOutput_));
#endif
#endif

#ifdef AUDIO_MIXER_NATIVE
    theMixer_.reset(new AudioMixerNative);
#endif


#if defined(PLAYREADY)
// PlayReadyDrmSystem is inheriting for enable_shared_from_this. Both to get
// this inheritance work and for compiler not to complain theDrmSystem_'s type,
// here we have somewhat complex type conversion
    theDrmSystem_ = static_pointer_cast<IDrmSystem> ( shared_ptr<PlayReadyDrmSystem> (new PlayReadyDrmSystem) );
#endif
}


DeviceLibNative::~DeviceLibNative()
{
    assert(theSystem_.get() != NULL);
    assert(theBufferManager_.get() != NULL);
    assert(theElementaryStreamManager_.get() != NULL);
    theElementaryStreamManager_.reset();
    theDrmSystem_.reset();
    theBufferManager_.reset();
    theSystem_.reset();
    theMixer_.reset();
    theMixerOutput_.reset();
    delete sConfiguration;
    sConfiguration = 0;
}

shared_ptr<IDrmSystem> DeviceLibNative::getDrmSystem()
{
    return theDrmSystem_;
}

shared_ptr<ISystem> DeviceLibNative::getSystem()
{
    return theSystem_;
}

shared_ptr<IWebCrypto> DeviceLibNative::getWebCrypto()
{
    // create on demand
    ScopedMutex lock(mutex_);
    if (!theWebCrypto_.get())
    {
        theWebCrypto_.reset(new WebCryptoTee(theSystem_->getAuthenticationType(), teeMgmtPtr_, teeCryptoPtr_));
    }
    return theWebCrypto_;
}

shared_ptr<esplayer::IElementaryStreamManager> DeviceLibNative::getESManager()
{
    return theElementaryStreamManager_;
}

shared_ptr<IBufferManager> DeviceLibNative::getBufferManager()
{
    return theBufferManager_;
}

shared_ptr<IAudioMixer> DeviceLibNative::getAudioMixer()
{
    return theMixer_;
}

extern "C" {

//
// The NrdLib creates a single instance of the device-specific
// implementation of IDeviceLib using createDeviceLib()
// extern function defined here. A device-specific implementation should
// modify this so that it creates the device-specific implementation of
// IDeviceLib.
//
IDeviceLib *createDeviceLib()
{
    return new DeviceLibNative();
}

//
// ----------------------------------------------------------------------
// Used only by test applications.
//
// Test applications can be run on a dpi implementation before the
// IDeviceLib implementation is completed. The different tools requires
// a few functions to be implemented, however.
//
esplayer::IElementaryStreamManager *createElementaryStreamManager()
{
    return new esplayer::ESManagerNative();
}

IBufferManager *createBufferManager(int audioPoolSize, int videoPoolSize)
{
    return new BufferManagerNative(audioPoolSize, videoPoolSize);
}

ISystem *createSystem()
{
    ASSERT(!sConfiguration->idFile.empty(),
           "\nNo individualization data file specified.");

    string dataDir = Configuration::dataWritePath();
    NTRACE(TRACE_DPI, "\nNetflix reference DPI configuration\n"
           "Data directory path: %s\n\n"
           "%s",
           dataDir.c_str(),
           sConfiguration->toString().c_str());

    string nonEncryptedFile = dataDir + "/nrd/systemData";
    string encryptedFile = nonEncryptedFile + ".secure";

    if (sConfiguration->mgk) {
        return new ModelGroupFileSystem(
            sConfiguration->idFile,
            sConfiguration->pubkeyFile,
            encryptedFile);
    } else {
        return new PresharedFileSystem(
            sConfiguration->idFile,
            sConfiguration->pubkeyFile,
            encryptedFile);
    }

}

#ifdef AUDIO_MIXER_SOFTWARE
IAudioMixer *createAudioMixer()
{
    shared_ptr<IAudioMixerOutput> theMixerOutput_;
#ifdef AUDIO_MIXER_OUTPUT_ALSA
    theMixerOutput_.reset(new AudioMixerOutputALSA);
#endif
#ifdef AUDIO_MIXER_OUTPUT_SINK
    theMixerOutput_.reset(new AudioMixerOutputSink);
#endif
    return new AudioMixerSoftware(theMixerOutput_);
}
#endif

#ifdef AUDIO_MIXER_NATIVE
IAudioMixer *createAudioMixer()
{
    return new AudioMixerNative;
}
#endif

//
// End used only by test applications.
// ----------------------------------------------------------------------
}

// Used by the netflix reference implementation so that the netflix app can
// be run with command-line parameters for netflix-internal QA of the
// netflix application.  Device-specific implementations other than the
// reference implementation do not need to define this extern function.

// Partners are not expected to ever need this functionality, so this should never be defined outside of Netflix's Reference DPI
ConfigurationOptions getDeviceLibOptions()
{
    if(!sConfiguration)
        sConfiguration = new referenceDpiConfig;
    //DeviceLibNative::sConfig = new DeviceLibNative::referenceDpiConfig();
    ConfigurationOptions options;
    options.push_back("Reference DPI Options");
    options.push_back(ConfigurationOption('I', "idfile", ConfigurationOption::STRING_ARGUMENT, &sConfiguration->idFile, "Path to Individualization or \"Manufacturing Secure Store\" file"));
    options.push_back(ConfigurationOption(0, "appboot-key", ConfigurationOption::DATAPATH_ARGUMENT, &sConfiguration->pubkeyFile, "Path to appboot public key file"));
    options.push_back(ConfigurationOption(0, "dpi-mgk",  &sConfiguration->mgk, "true if the ID file is for Model Group Key"));

    options.push_back(ConfigurationOption(0, "dpi-language", ConfigurationOption::STRING_ARGUMENT, &sConfiguration->language, "The language code the DPI will report."));
    options.push_back(ConfigurationOption(0, "dpi-friendlyname", ConfigurationOption::STRING_ARGUMENT, &sConfiguration->friendlyName, "The name that this device will advertise with for MDX and DIAL features."));

    options.push_back(ConfigurationOption(0, "dpi-has-pointer",  &sConfiguration->hasPointer, "true if the device has a pointing device connected"));
    options.push_back(ConfigurationOption(0, "dpi-has-keyboard",  &sConfiguration->hasKeyboard, "true if the device has a keyboard connected"));
    options.push_back(ConfigurationOption(0, "dpi-has-osk",  &sConfiguration->hasOnScreenKeyboard, "true if the device provides a built-in on-screen keyboard for text input"));
    options.push_back(ConfigurationOption(0, "dpi-has-suspend",  &sConfiguration->hasSuspend, "true if the device supports suspend mode"));
    options.push_back(ConfigurationOption(0, "dpi-has-background",  &sConfiguration->hasBackground, "true if the device supports background mode"));
    options.push_back(ConfigurationOption(0, "dpi-support-2DVideoResize",  &sConfiguration->support2DVideoResize, "true if the device can resize the video window"));
    options.push_back(ConfigurationOption(0, "dpi-support-video2DResizeAnimation",  &sConfiguration->support2DVideoResizeAnimation, "true if the device can resize the video window during playback 5 times or more per second"));
    options.push_back(ConfigurationOption(0, "dpi-support-DrmPlaybackTransition",  &sConfiguration->supportDrmPlaybackTransition, "true if the device can make a seamless transition from non-DRMed playback to DRMed playback"));
    options.push_back(ConfigurationOption(0, "dpi-support-MultipleDrmDecryption",  &sConfiguration->supportMultipleDrmDecryption, "true if the device can handle multiple drmed content simultaneously"));
    options.push_back(ConfigurationOption(0, "dpi-support-LimitedDurationLicense",  &sConfiguration->supportLimitedDurationLicenses, "true if the device can support limited duration license"));
    options.push_back(ConfigurationOption(0, "dpi-support-SecureStop",  &sConfiguration->supportSecureStop, "true if the drm subsystem can support secure stop"));
    options.push_back(ConfigurationOption(0, "dpi-support-DrmStorageDeletion",  &sConfiguration->supportDrmStorageDeletion, "true if the drm subsystem has drm storage deletion feature"));
    options.push_back(ConfigurationOption(0, "dpi-support-OnTheFlyAudioSwitch",  &sConfiguration->supportOnTheFlyAudioSwitch, "true if the device can make a on the fly transition to different audio track"));
    options.push_back(ConfigurationOption(0, "dpi-support-OnTheFlyAudioCodecSwitch",  &sConfiguration->supportOnTheFlyAudioCodecSwitch, "true if the device can make a on the fly transition to different audio codec"));
    options.push_back(ConfigurationOption(0, "dpi-videobufferpoolsize", ConfigurationOption::SIZE_ARGUMENT, &sConfiguration->videoBufferPoolSize, "size in bytes of the pool of buffers used to receive video stream data"));
    options.push_back(ConfigurationOption(0, "dpi-audiobufferpoolsize", ConfigurationOption::SIZE_ARGUMENT, &sConfiguration->audioBufferPoolSize, "size in bytes of the pool of buffers used to receive audio stream data"));

    options.push_back(ConfigurationOption(0, "dpi-support-hevc", &sConfiguration->hevcEnabled, "whether hevc is enabled"));

    options.push_back(ConfigurationOption(0, "dpi-videoscreenwidth", ConfigurationOption::INT_ARGUMENT, &sConfiguration->videoRendererScreenWidth, "Video renderer screen width"));
    options.push_back(ConfigurationOption(0, "dpi-videoscreenheight", ConfigurationOption::INT_ARGUMENT, &sConfiguration->videoRendererScreenHeight, "Video renderer screen height"));

    options.push_back(ConfigurationOption(0, "dpi-minAudioPtsGap", ConfigurationOption::SIZE_ARGUMENT, &sConfiguration->minAudioPtsGap, "minimal audio pts gap that device can support"));

    options.push_back(ConfigurationOption(0, "enable-sso", &sConfiguration->enableSSO, "true if app supports SSO, false otherwise"));
    options.push_back(ConfigurationOption(0, "enable-signup", &sConfiguration->enableSignup, "true if app supports Signup, false otherwise"));
    options.push_back(ConfigurationOption(0, "dpi-pincode", ConfigurationOption::STRING_ARGUMENT, &sConfiguration->pinCode, "pin code passed from the platform"));
    options.push_back(ConfigurationOption(0, "dpi-token-url", ConfigurationOption::STRING_ARGUMENT, &sConfiguration->tokenUrl, "BOBO URL"));
    options.push_back(ConfigurationOption(0, "dpi-header-list", ConfigurationOption::STRING_ARGUMENT, &sConfiguration->headerList, "BOBO comma separated custom headers"));



#if defined(NRDP_HAS_IPV6)
    options.push_back(ConfigurationOption(0,
                                          "dpi-ip-connectivity-mode",
                                          ConfigurationOption::INT_ARGUMENT,
                                          &sConfiguration->ipConnectivityMode,
                                          "ip connectivity mode selection [4 for ipv4/ 6 for ipv6/ 10 for dualstack]"));
#endif



    return options;
}
