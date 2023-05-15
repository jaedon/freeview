/* global nrdp */

nrdp.addEventListener("init", loadNrdJS);
nrdp.init();

function loadNrdJS() {
    /* want to test nrdjs */
    nrdp.gibbon.loadScript({url: 'http://localcontrol.netflix.com/js/nrdjs.js'}, initNrdjs);
}

function initNrdjs() {
    nrdp.js.init(playMovie);
}

function playMovie() {
    /* if 5 minutes pass and we haven't finished playing, assert */
    nrdp.gibbon.setTimeout(function() { nrdp.assert(false, "5 minute timer: quickplay wasn't quick"); },
                           5 /*min*/ * 60 /*sec/min*/ * 1000 /*ms/sec*/);

    /* if using directfb, this will make sure video goes in the same window */
    var a = nrdp.gibbon.scene.widget = nrdp.gibbon.makeWidget();
    a.rect = { x: 0, y: 0, width: nrdp.gibbon.scene.width, height: nrdp.gibbon.scene.height };
    a.color = { r: 255, g: 255, b: 255, a: 0 };
    nrdp.media.throttled = false;
    nrdp.qa.play(70120072);

    /* as soon as we start playing, skip to the end of the movie */
    nrdp.media.addEventListener("updatepts", function() {
        nrdp.media.removeEventListener("updatepts", arguments.callee);
        nrdp.log.fatal("XID is " + nrdp.media.xid);
        nrdp.media.throttled = true;
        nrdp.media.swim(nrdp.media.duration - (30 /*sec*/ * 1000 /*ms/sec*/), true, 30, true);
    });

    /* if we hit endofstream, we're done, exit successfully */
    nrdp.media.addEventListener("endofstream", shutdownPlayback);
    /* if we get any error, assert */
    nrdp.media.addEventListener("error", function() { nrdp.assert(false, "Got an error trying to play back"); });

    /* we have 15 seconds to complete authorization */
    var to = nrdp.gibbon.setTimeout(function() { nrdp.assert(false, "Authorization did not complete within 15 seconds"); },
                                    15 /*sec*/ * 1000 /*ms/sec*/);
    nrdp.media.addEventListener("opencomplete", function(){ nrdp.gibbon.clearTimeout(to); });
}

function shutdownPlayback() {
    nrdp.media.addEventListener("statechanged", flushLogs);
    nrdp.media.close();
}

function flushLogs() {
    if (nrdp.media.state != nrdp.media.CLOSED) return;
    nrdp.log.flush();
    nrdp.gibbon.setTimeout(nrdp.exit.bind(nrdp), 3000);
}
