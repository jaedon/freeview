/// <reference path="../../def/sugar.d.ts" />
// file name : fw/service/rcu/oipf_emul_console.js
/* jslint sloppy:true, nomen:true, vars:true */
(function () {
    var elKeyHelper = document.createElement('div');
    var style = elKeyHelper.style;
    var $elKeyHelper = $(elKeyHelper);
    elKeyHelper.setAttribute("id", "key-helper");
    style.position = "absolute";
    style.left = "540px";
    style.top = "320px";
    style.padding = "15px";
    style.borderRadius = "8px";
    style.fontSize = "larger";
    style.backgroundColor = "rgba(0,0,0,.4)";
    style.width = "200px";
    style.textAlign = "center";
    style.zIndex = "1000";
    style.display = "none";
    document.body.appendChild(elKeyHelper);
    function fnHandler(aEvent) {
        var keyCode = hx.key['KEY_' + this.getAttribute("id")];
        if (keyCode === undefined) {
            console.warn("[oipf_emul_console] : keycode [" + aEvent.which + "] undefined. it will be skipped");
            return;
        }
        var e = window.crossBrowser_initKeyboardEvent("keydown", {
            which: keyCode,
            keyCode: keyCode
        });
        elKeyHelper.innerText = keyCode + ':' + hx.key[keyCode];
        $elKeyHelper.stop(true, true).fadeIn(200, function () {
            $elKeyHelper.stop(true, true).fadeOut(1000);
        });
        aEvent.preventDefault();
        document.dispatchEvent(e);
    }
    var elMap = document.getElementById("mapRemoteController");
    var elAreas = elMap.getElementsByTagName("area");
    var elArea;
    var event = "mousedown";
    var title = "title";
    var id;
    var codeMappedKey;
    var keyMapped;
    setTimeout(function () {
        for (var i = 0, len = elAreas.length; i < len; i++) {
            elArea = elAreas[i];
            id = elArea.getAttribute("id");
            codeMappedKey = hx.key[hx.key[hx.key["KEY_" + id]]];
            keyMapped = (codeMappedKey > 256) ? "NONE" : String.fromCharCode(codeMappedKey);
            elArea.addEventListener(event, fnHandler, false);
            elArea.setAttribute(title, id + ": mapped [" + keyMapped + "]");
        }
    }, 1000);
    $('.mapper').maphilight({
        fade: true
    });
})();
//# sourceMappingURL=oipf_emul_console.js.map