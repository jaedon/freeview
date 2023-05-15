var msgTimer;

WebInspector.InspectorFrontendHostStub.prototype.copyText = function(text) {
    if (text.indexOf('\n') == -1) {
        prompt("Copy", text);
    } else {
        var w = window.open("CopyPage.html");
        var enc = btoa(text);
        // keep trying until we get a pong

        addEventListener('message', function() { clearInterval(msgTimer); }, false);
        msgTimer = setInterval(function() { w.postMessage(enc, "*"); }, 1000);
    }
};
