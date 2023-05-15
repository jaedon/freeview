/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <nrdase/Manifest.h>

using namespace netflix::ase;
using namespace std;

NRDP_DEFINE_OBJECTCOUNT(Manifest);
NRDP_DEFINE_OBJECTCOUNT(ManifestServer);
NRDP_DEFINE_OBJECTCOUNT(ManifestServingLocation);
NRDP_DEFINE_OBJECTCOUNT(ManifestStream);
NRDP_DEFINE_OBJECTCOUNT(ManifestTrack);
NRDP_DEFINE_OBJECTCOUNT(ManifestUrl);

bool ManifestUrl::UrlCompareLess::operator()( ManifestUrl const& lhs, ManifestUrl const& rhs )
{
    bool retVal = false;
    // CHECK: two different locations can not have both the same level and same rank
    ManifestPtr pManifest = lhs.getParent()->getParent()->getParent();
    map<uint32_t, ManifestServerPtr> servers = pManifest->getServers();

    // TODO: handle the case that the url's server/location is not in the server/location list
    ManifestServingLocationPtr pLServingLocation = pManifest->findServingLocation(lhs.getLocationKey());
    ManifestServingLocationPtr pRServingLocation = pManifest->findServingLocation(rhs.getLocationKey());
    assert(pLServingLocation != ManifestServingLocationPtr());
    assert(pRServingLocation != ManifestServingLocationPtr());
    if (pLServingLocation->getLevel() < pRServingLocation->getLevel())
    {
        retVal = true;
    }
    else if (pLServingLocation->getLevel() > pRServingLocation->getLevel())
    {
        retVal = false;
    }
    else
    {
        // the same location level
        if (pLServingLocation->getRank() < pRServingLocation->getRank())
        {
            retVal = true;
        }
        else if (pLServingLocation->getRank() > pRServingLocation->getRank())
        {
            retVal = false;
        }
        else
        {
            // The same location rank
            ManifestServerPtr pLServer = servers[lhs.getCdnId()];
            ManifestServerPtr pRServer = servers[rhs.getCdnId()];
            if (pLServer->getRank() < pRServer->getRank())
            {
                retVal = true;
            }
            else
            {
                // return false for equal values too
                retVal = false;
            }
        }
    }
    return retVal;
}

ManifestStream::ManifestStream( ManifestTrackPtr pParentTrack )
    : mParentTrackPtr( pParentTrack )
    , mEnabled(true)
    , mVideoWidth( 0 )
    , mVideoHeight( 0 )
    , mParx( 1 )
    , mPary( 1 )
    , mFrameRate( 0, 1 )
    , mAudioSamplingFrequency( 0 )
    , mHD(false)
    , mSuperHD(false)
    , mNominalBitrate( 0 )
{
    NRDP_OBJECTCOUNT_REF(ManifestStream, this);
}

ManifestStream::~ManifestStream()
{
    NRDP_OBJECTCOUNT_DEREF(ManifestStream, this);
}

bool ManifestStream::NominalBitrateCompare::operator()( ManifestStream const& lhs, ManifestStream const& rhs )
{
    return ( lhs.getNominalBitrate() < rhs.getNominalBitrate() );
}

std::vector<std::string> Manifest::getLocationKeys() const
{
    std::vector<std::string> keys;

    std::vector<ManifestTrackPtr>::const_iterator it = getTracks().begin();
    if (it != getTracks().end())
    {
        std::vector<ManifestStreamPtr>::const_iterator jt = (*it)->getStreams().begin();
        if(jt != (*it)->getStreams().end())
        {
            ManifestUrls const& urls = (*jt)->getUrls();
            ManifestUrls::const_iterator kt = urls.begin();
            for( ; kt != urls.end(); ++kt )
            {
                keys.push_back( (*kt)->getLocationKey() );
            }
        }
    }

    return keys;
}
