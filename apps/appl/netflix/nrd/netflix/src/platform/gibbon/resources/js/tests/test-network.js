// this only works if you disable file access in NetworkManager,
// either by disabling the file system, creating a release build, or
// hacking the code. See handleFileRequest().
var urls = [
    "file:///etc/passwd", false,
    "file://data/resources/js/NetflixBridge.js", false,
    "http://localcontrol.netflix.com/js/NetflixBridge.js", true,
    "http://localcontrol.netflix.com/js/../../etc/conf/gibbon.xml", false,
    "http://localcontrol.netflix.com/js/tests/../NetflixBridge.js", true,
    "./test-network.js", true,
    "../tests/test-network.js", true,
    "//localcontrol.netflix.com/js/NetflixBridge.js", true
];

var cnt = urls.length / 2;
var got = 0;
var fail = 0;

function completedOne() {
    got++;
    if (got == cnt) {
        nrdp.log.warn("test run complete with " + fail + "/" + cnt + " failures");
//        nrdp.exit();
    }
}

function shouldBeGood(url) {
    return function(evt) {
        if (evt.statusCode != 200 || evt.reason != nrdp.gibbon.SUCCESS) {
            nrdp.log.error("Incorrectly failed to load " + url);
            fail++;
        }
        completedOne();
    };
}

function shouldFail(url) {
    return function(evt) {
        if (evt.statusCode == 200 && evt.reason == nrdp.gibbon.SUCCESS) {
            nrdp.log.error("Incorrectly successfully loaded " + url + ": " + JSON.stringify(evt));
            fail++;
        }
        completedOne();
    };
}

function runTest() {
    for (var i = 0; i < urls.length; i += 2) {
        nrdp.gibbon.load({url: urls[i]}, urls[i+1] ? shouldBeGood(urls[i]) : shouldFail(urls[i]));
    }
}

nrdp.qa.init(runTest);
