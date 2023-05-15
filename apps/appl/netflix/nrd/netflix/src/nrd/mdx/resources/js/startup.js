(function() {
    var ploc = window.location.toString();
    var urlParams = location.search;

    getParam = function(key) {
        var sIdx = urlParams.indexOf(key), sep, eIdx;
        if (sIdx !== -1) {
            sep = urlParams.charAt(sIdx - 1);
            if (!sep || sep === "?" || sep === "&") {
                eIdx = urlParams.indexOf("&", sIdx);
                return urlParams.substring(sIdx + (key.length + 1), eIdx !== -1 && eIdx || urlParams.length);
            }
        }
    }

    var nbpdHost = getParam("nbpdHost") || "localcontrol.netflix.com";
    var mdxurl = getParam("mdxurl") || ((ploc.indexOf("localhost") != -1) ? "../js/mdx.js" : "http://" + nbpdHost + "/js/mdx.js");

    if (window["IS_ANDROID"] !== true) {
        if (window.location.host.indexOf("localhost") != -1) {
            document.write('<scr' + 'ipt type="text/javascript" src="../js/NetflixBridge.js"></scr' + 'ipt>');
            document.write('<scr' + 'ipt type="text/javascript" src="../js/PartnerBridge.js"></scr' + 'ipt>');
        } else {
            document.write('<scr' + 'ipt type="text/javascript" src="http://' + nbpdHost + '/js/NetflixBridge.js"></scr' + 'ipt>');
            document.write('<scr' + 'ipt type="text/javascript" src="http://' + nbpdHost + '/js/PartnerBridge.js"></scr' + 'ipt>');
            document.write('<scr' + 'ipt type="text/javascript" src="/js/nrdjs/video.js"></scr' + 'ipt>');
        }
    }

    document.write('<scr' + 'ipt type="text/javascript" src="' + mdxurl + '"></scr' + 'ipt>');

    var plugin = getParam("mdxplugin");
    if(plugin) {
        document.write('<scr' + 'ipt type="text/javascript" src="' + plugin + '"></scr' + 'ipt>');
    }

})();

// the NRDP bridge version to use
//       true = qt bridge, can only be used in 3.1.4
//      false = mongoose (original bridge), use this in 3.1.3 or 3.1.4
//  undefined = the bridge will be whatever the Partner has compiled to be the default bridge

var USE_QT_BRIDGE = undefined;

// 3.1.3 will only use mongoose, even if 'true' or 'false' is supplied as the
//   2nd parameter to nrdp.init(), since in 3.1.3 this 2nd parm will be ignored.
// 3.1.4 (and beyond) can use either mongoose or qt bridges.
// 3.2 - Support isn't here yet for selecting mongoose transport, default is always QT (NRDP-102)

function initNRDPWithCorrectTransport(){
    if ( USE_QT_BRIDGE === true ) {
        nrdp.init(undefined,true);
    }
    else if ( USE_QT_BRIDGE === false ) {
        nrdp.init(undefined,false);
    }
    else {
        nrdp.init(undefined,undefined);
    }
}
