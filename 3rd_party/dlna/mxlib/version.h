#pragma once

#define VERSION_MAJOR	1
#define VERSION_MINOR	0
#define VERSION_BUILD	0
#define VERSION_QFE		11

#define _STR(x) #x
#define STR(x) _STR(x)

#define VERSION_NUMBER VERSION_MAJOR,VERSION_MINOR, VERSION_BUILD,VERSION_QFE

#define VERSION_STRING STR(VERSION_MAJOR) "." STR(VERSION_MINOR) "." \
                                 STR(VERSION_BUILD) "." STR(VERSION_QFE)
