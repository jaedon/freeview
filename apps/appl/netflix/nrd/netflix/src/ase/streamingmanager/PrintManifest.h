/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef PRINT_MANIFEST_H

#include <iostream>
#include <nrdase/Manifest.h>

namespace std {
std::ostream& operator<<( std::ostream& os, netflix::ase::ManifestUrl const& url );
std::ostream& operator<<( std::ostream& os, netflix::ase::ManifestUrls const& urls );
std::ostream& operator<<( std::ostream& os, netflix::ase::ManifestStream const& stream );
std::ostream& operator<<( std::ostream& os, netflix::ase::ManifestTrack const& track );
std::ostream& operator<<( std::ostream& os, netflix::ase::Manifest const& manifest );
}

#endif /* PRINT_MANIFEST_H */
