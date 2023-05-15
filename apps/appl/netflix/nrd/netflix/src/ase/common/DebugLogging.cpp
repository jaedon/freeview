/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <nrdase/DebugLogging.h>
#include <nrdase/IAseConfigParameter.h>

namespace netflix {
namespace ase
{

std::vector<std::pair<const char*,bool*> >& getLoggers()
{
    static std::vector<std::pair<const char*,bool*> > sLoggers;
    return sLoggers;
}

void configureLoggers( IAseConfigParameter const& configuration )
{
    std::string value = configuration.queryParameter( "logger.default" );
    bool bDefault = ( value != "false" );

    std::vector<std::pair<const char*,bool*> >::const_iterator it = getLoggers().begin();
    for( ; it != getLoggers().end(); ++it )
    {
        std::string name( "logger." );
        name += it->first;
        value = configuration.queryParameter( name );
        *(it->second) = ( value == "true" )
                            ? true
                            : ( ( value == "false" ) ? false : bDefault );
    }
}


}}
