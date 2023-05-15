# (c) 1997-2015 Netflix, Inc.  All content herein is protected by
# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights.  Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.

gibbon_add_sources(main_posix.cpp)
gibbon_set_eventloop("posix")

gibbon_add_js(partner_signup.js)
gibbon_add_js(partner_sso.js)

