/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef EXPORTABLE_H_
#define EXPORTABLE_H_

#if (defined _WIN32 || defined __CYGWIN__)
#ifdef BUILDING_DLL
#ifdef __GNUC__
      #define NRDP_EXPORTABLE __attribute__ ((dllexport))
#else
      #define NRDP_EXPORTABLE __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
    #endif
#else
#ifdef __GNUC__
      #define NRDP_EXPORTABLE __attribute__ ((dllimport))
#else
      #define NRDP_EXPORTABLE __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
    #endif
  #endif
  #define NRDP_HIDDEN
#else
#if __GNUC__ >= 4
    #define NRDP_EXPORTABLE __attribute__ ((visibility ("default")))
    #define NRDP_HIDDEN  __attribute__ ((visibility ("hidden")))
#else
    #define NRDP_EXPORTABLE
    #define NRDP_HIDDEN
  #endif
#endif

#endif /* EXPORTABLE_H_ */
