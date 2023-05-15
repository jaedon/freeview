/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <Benchmark.h>
#include <Sprite.h>
#include <Timer.h>


namespace netflix {

ISurface::SharedPtr sImage;

bool BlitBenchmark::init()
{
    if (sImage == NULL) {
        sImage = ISurface::create();
        if (!sImage->load("Netflix_Logo.png")) {
            sImage.reset();
            return false;
        }
    }

    return true;
}


void BlitBenchmark::shutdown()
{
    sImage.reset();
}


unsigned BlitBenchmark::run(ISurface& screen)
{
    Rect r(0, 0, screen.getWidth(), screen.getHeight());

    const int spriteCount = 35;
    Sprite sprites[spriteCount];
    int x = 0, y = 0;
    int w = sImage->getWidth();

    for (int i = 0; i < spriteCount; ++i) {
        Sprite* s = sprites + i;

        s->setSurface(sImage);

        s->moveTo(x, y);
        s->setVelocity(0, 1);

        if ((mRenderFlags & BLIT_STRETCH) != 0) {
            s->mRect.width += 10;
            s->mRect.height += 10;
        }

        x += (w + 10);
        if ((x + w) > r.width) {
            x = 0;
            y += sImage->getHeight() + 10;
        }
    }

    Timer testTimer;
    Timer paintTimer;

    testTimer.start();

    unsigned frameCount = 0;

    screen.setCompositionMode((mRenderFlags & BLIT_ALPHA) != 0 ? ISurface::COMPMODE_BLEND : ISurface::COMPMODE_NONE);

    while (testTimer.elapsed() < TEST_DURATION_MS) {

        for (int k = 0; k < spriteCount; ++k) {
            sprites[k].tick();
        }

        for (int k = 0; k < spriteCount; ++k) {
            Sprite *s = &sprites[k];

            if (s->mRect.y >= r.height) {
                s->mRect.y -= s->mRect.height + r.height + 10;
            }
        }

//        screen.fill(r, Color());

        paintTimer.start();

        for (int k = 0; k < spriteCount; ++k) {
            sprites[k].render(screen);
        }

//        cout << "Paint time: " << paintTimer.elapsed() << " ms" << endl;

        screen.flip(r);

        ++frameCount;
    }

    screen.setCompositionMode(ISurface::COMPMODE_NONE);

    return frameCount;
}


unsigned FillBenchmark::run(ISurface& screen)
{
    Rect r(0, 0, screen.getWidth(), screen.getHeight());

    const int spriteCount = 35;
    Sprite sprites[spriteCount];
    int x = 0, y = 0;
    int w = sImage->getWidth();

    for (int i = 0; i < spriteCount; ++i) {
        Sprite* s = sprites + i;

        s->setColor(Color(100 + ((i * 10) % 155), 0, 0, (mRenderFlags & FILL_ALPHA) != 0 ? 128 : 255));

        s->mRect.width = sImage->getWidth();
        s->mRect.height = sImage->getHeight();

        s->moveTo(x, y);
        s->setVelocity(0, 1);

        x += (w + 10);
        if ((x + w) > r.width) {
            x = 0;
            y += sImage->getHeight() + 10;
        }
    }

    Timer testTimer;
    Timer paintTimer;

    testTimer.start();

    unsigned frameCount = 0;

    screen.setCompositionMode((mRenderFlags & FILL_ALPHA) != 0 ? ISurface::COMPMODE_BLEND : ISurface::COMPMODE_NONE);

    while (testTimer.elapsed() < TEST_DURATION_MS) {

        for (int k = 0; k < spriteCount; ++k) {
            sprites[k].tick();
        }

        for (int k = 0; k < spriteCount; ++k) {
            Sprite *s = &sprites[k];

            if (s->mRect.y >= r.height) {
                s->mRect.y -= s->mRect.height + r.height + 10;
            }
        }

//        screen.fill(r, Color());

        paintTimer.start();

        for (int k = 0; k < spriteCount; ++k) {
            sprites[k].render(screen);
        }

//        cout << "Paint time: " << paintTimer.elapsed() << " ms" << endl;

        screen.flip(r);

        ++frameCount;
    }

    screen.setCompositionMode(ISurface::COMPMODE_NONE);

    return frameCount;
}


unsigned TextBenchmark::run(ISurface& screen)
{
    Rect r(0, 0, screen.getWidth(), screen.getHeight());

    const int spriteCount = 19;
    Sprite sprites[spriteCount];
    int x = 20, y = 10;

    for (int i = 0; i < spriteCount; ++i) {
        Sprite* s = sprites + i;

        s->setText("01234567890ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");

        s->mRect.width = 1240;
        s->mRect.height = 35;

        s->moveTo(x, y);
        s->setVelocity(0, 1);

        y += 40;
    }

    Timer testTimer;
    Timer paintTimer;

    testTimer.start();

    unsigned frameCount = 0;

    screen.setCompositionMode(ISurface::COMPMODE_NONE);

    while (testTimer.elapsed() < TEST_DURATION_MS) {

        for (int k = 0; k < spriteCount; ++k) {
            sprites[k].tick();
        }

        for (int k = 0; k < spriteCount; ++k) {
            Sprite *s = &sprites[k];

            if (s->mRect.y >= r.height) {
                s->mRect.y -= s->mRect.height + r.height + 10;
            }
        }

//        screen.fill(r, Color());

        paintTimer.start();

        for (int k = 0; k < spriteCount; ++k) {
            sprites[k].render(screen);
        }

//        cout << "Paint time: " << paintTimer.elapsed() << " ms" << endl;

        screen.flip(r);

        ++frameCount;
    }

    return frameCount;
}

}
