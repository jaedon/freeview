# (c) 1997-2014 Netflix, Inc.  All content herein is protected by
# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights.  Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.

macro(set_vstudio_folder target folder)
    if(TARGET ${target})
        set_target_properties(${target} PROPERTIES FOLDER ${folder})
    endif()
endmacro()

# set filters for playready
if (UPGRADE_TO_PLAYREADY25)
    set_vstudio_folder(playready-2.5-ss-tee "nrdp/3rdparty")
else()
    set_vstudio_folder(playready-2.0-tee "nrdp/3rdparty")
endif()
set_vstudio_folder(playready-tools "nrdp/3rdparty")

# set filters for nrdp
set_vstudio_folder(mongoose "nrdp")
set_vstudio_folder(nrd "nrdp")
set_vstudio_folder(nrdapp "nrdp")
set_vstudio_folder(nrdase "nrdp")
set_vstudio_folder(nrdase_heuristics "nrdp")
set_vstudio_folder(nrdbase "nrdp")
set_vstudio_folder(nrddemux "nrdp")
set_vstudio_folder(nrddpi "nrdp")
set_vstudio_folder(nrdnet "nrdp")
set_vstudio_folder(nrdnet_http "nrdp")
set_vstudio_folder(nrdntba "nrdp")
set_vstudio_folder(nrdtee "nrdp")
set_vstudio_folder(nrdil "nrdp")

# set filters for mdx
set_vstudio_folder(mdx_3rdparty "nrdp/mdx")
set_vstudio_folder(mdxlib "nrdp/mdx")
set_vstudio_folder(mdxlib_ixml "nrdp/mdx")
set_vstudio_folder(mdxlib_threadutil "nrdp/mdx")

# set filters for documentation
set_vstudio_folder(documentation "nrdp/documentation")
set_vstudio_folder(nrd_documentation "nrdp/documentation")
set_vstudio_folder(nrdbase_documentation "nrdp/documentation")
set_vstudio_folder(nrddpi_documentation "nrdp/documentation")

# set filters for gibbon
set_vstudio_folder(gibbon "nrdp/gibbon")
set_vstudio_folder(nrdresources_gibbon "nrdp/gibbon")
set_vstudio_folder(JavaScriptCore "nrdp/gibbon")
set_vstudio_folder(WTF "nrdp/gibbon")

# configuration options to build nrdjs locally
#set(BUILD_NRDJS_DIRECTORY "${CMAKE_SOURCE_DIR}/${NRDJS_DIRECTORY}")
#set_vstudio_folder(js "nrdp/nrdjs")
#set_vstudio_folder(boot.js_library "nrdp/nrdjs")
#set_vstudio_folder(msl.js_library "nrdp/nrdjs")
#set_vstudio_folder(nrdjs.js_library "nrdp/nrdjs")
#set_vstudio_folder(test.js_library "nrdp/nrdjs")
