nexus unittests

Building:

    export DESTDIR=/your/nexus/run/dir
    make install

Running:

    # run all unit tests
    ./run_unittests

    # capture results
    ./run_unittests 2&>1|>results.txt

    # run one unit tests
    nexus unittests/video_decoder/open

Unit test requirements:

    1. app must run with no user interaction
    2. app must run in finite amount of time, preferrable <2 minutes
    3. app must assert, crash or return a non-zero error code if anything fails

Notes

If there are problem apps, remove them from the subdir's Makefile
If there are problem dirs, you can add a "grep -v" clause to the top Makefile
