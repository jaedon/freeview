#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#include <directfb.h>
#include <directfb_strings.h>
#include <directfb_util.h>

/* macro for a safe call to DirectFB functions */
#define DFBCHECK(...)                                                     \
     {                                                                    \
          ret = __VA_ARGS__;                                              \
          if (ret != DFB_OK) {                                            \
               fprintf( stderr, "%s <%d>:\n\t", __FILE__, __LINE__ );     \
               DirectFBErrorFatal( #__VA_ARGS__, ret );                   \
          }                                                               \
     }

/**********************************************************************************************************************/

int main( int argc, char *argv[] )
{
    int i, xres, yres;
    DFBResult               ret;
    DFBSurfaceDescription   desc;
    IDirectFB              *dfb;
    IDirectFBImageProvider *image_provider   = NULL;
    IDirectFBSurface       *primary          = NULL;
    IDirectFBSurface       *source_image     = NULL;
    struct timeval          start_time, end_time;

    /* Initialize DirectFB. */
    ret = DirectFBInit( &argc, &argv );
    if (ret) {
        D_DERROR( ret, "DirectFBInit() failed!\n" );
        return ret;
    }

    /* Create super interface. */
    ret = DirectFBCreate( &dfb );
    if (ret) {
        D_DERROR( ret, "DirectFBCreate() failed!\n" );
        return ret;
    }

    /* Fill description for a primary surface. */
    desc.flags = DSDESC_CAPS;
    desc.caps  = DSCAPS_PRIMARY | DSCAPS_FLIPPING | DSCAPS_DOUBLE;

    dfb->SetCooperativeLevel( dfb, DFSCL_FULLSCREEN );

    /* Create a primary surface. */
    DFBCHECK(dfb->CreateSurface( dfb, &desc, &primary ));

    DFBCHECK(primary->GetSize( primary, &xres, &yres ));

    /* Loop over the images passed in */
    for (i = 2; i < argc; i++) {

        printf("Testing image provider with: %s\n", argv[i]);

        /* Create an image provider for the image to be loaded. */
        DFBCHECK(dfb->CreateImageProvider(dfb, argv[i], &image_provider));

        /* Get the surface description. */
        DFBCHECK(image_provider->GetSurfaceDescription(image_provider, &desc));

        printf("Source is %dx%d using %s\n",
                desc.width, desc.height, dfb_pixelformat_name(desc.pixelformat));

        /* Create a surface for the image. */
        DFBCHECK(dfb->CreateSurface(dfb, &desc, &source_image));

        gettimeofday( &start_time, NULL );

        DFBCHECK(image_provider->RenderTo(image_provider, source_image, NULL));

        gettimeofday( &end_time, NULL );

        printf("RenderTo took %lldms\n",
            (((end_time.tv_sec * 1000000LL) + end_time.tv_usec) - ((start_time.tv_sec * 1000000LL) + start_time.tv_usec)) / 1000LL);

        /* Display image onscreen */
        DFBCHECK(primary->StretchBlit(primary, source_image, NULL, NULL));
        DFBCHECK(primary->Flip(primary, NULL, DSFLIP_NONE));

        if (image_provider)
            image_provider->Release(image_provider);

        if (source_image)
            source_image->Release(source_image);
    }

    printf("End of images, cleaning up\n");

    if (primary)
        primary->Release( primary );

    /* Shutdown DirectFB. */
    dfb->Release( dfb );

    return ret;
}

