/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "ManufSS.h"

using namespace netflix::manufacturing;

int main(int argc, char* argv[])
{
    int result = 0;

    // we require 3 argumnets in addition to the executable itself
    if (argc < 4)
    {
    	printf("\nUsage:"
    		   "\n   ./manufss <target_directory> <path to idfile> <path to drm directory>"
    		   "\nExample invocation:"
    		   "\n   ./manufss ~/netflix/build/src/platform/gibbon ~/idfile ~/netflix/build/src/platform/gibbon/data/dpi/playready"
    		   "\n");
    }
    else
    {
    	printf("\nReceived arguments:"
    	    	"\n Target Directory: %s"
    	        "\n IdFile: %s"
    	        "\n DRM Directory: %s\n", argv[1], argv[2], argv[3]);

    	ManufSS manufSS(argv[1], argv[2], argv[3]);
    	result = manufSS.exists();
    	if (0 == result)
    	{
    		printf("manuf_ss.bin file does not exist, creating one\n");
    		result = manufSS.create();
    	}
    	else
    	{
    		printf("manuf_ss.bin already exists, aborting\n");
    	}
    }

    return result;
}
