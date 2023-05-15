/**
 * Created by bglee@humaxdigital.com on 2014. 6. 14..
 */
///<reference path="../def/sugar.d.ts" />
///<reference path="../def/require.d.ts" />
import __CONTROLS__ = require('pages/util/controls');
import __CONTROL_TRD_CONFLICT_DLG__ = require("pages/components_based_controls/trd_conflict_dialog");
declare var hx: ifSugar;
class CTestMain {
    private _cRoot: __CONTROLS__.CLayeredGroupControl;

    constructor() {
        hx.svc.KEY_EVENT_BINDER.unBind();
        hx.svc.KEY_EVENT_BINDER.bind(document, this, (aEvent) => {
            this.onKey(aEvent);
        });
        var $root = $("<div>", {
            "id": "test-main",
            "class": "-page-full"
        });
        var cRoot = new __CONTROLS__.CLayeredGroupControl($root);
        cRoot.createLayer({
            addClass: "-color-background"
        });
        document.body.appendChild(cRoot._$self[0]);
        this._cRoot = cRoot;
    }

    destroy() {
        hx.svc.KEY_EVENT_BINDER.unBind();
        hx.svc.KEY_EVENT_BINDER.bind(document, hx.al.entity);
        document.body.removeChild(this._cRoot._$self[0]);
    }

    onKey(aEvent: KeyboardEvent) {
        if (aEvent.metaKey) {
            /* COMMENT
             * for debug env
             * * bglee@humaxdigital.com, 10:25, 2014. 4. 5.
             */
            hx.log("warn", "[CTestMain] : meta key + [" + hx.key[aEvent.which] + "] will be skipped");
            return;
        }
        var keyString = hx.key[aEvent.which];
        if (keyString) {
            var split = keyString.split("_");
            var part;
            split[0] = "doKey";
            for (var i = 1; i < split.length; i++) {
                part = split[i];
                if (part.length > 1) {
                    part = part[0] + part.substr(1).toLowerCase();
                }
                split[i] = part;
            }
            keyString = split.join("");
        }
        hx.log("info", "[CTestMain] : " + keyString + " : is handler exist? : " + !!this[keyString]);
        var handled = false;
        if (this._cRoot._controls && this._cRoot._controls.length) {
            handled = this._cRoot.doKey(aEvent.which);
        }
        if (!handled && this[keyString]) {
            this[keyString]();
        }
    }

    doKeyEscape() {
        this.destroy();
    }

    doKeyChannelUp() {
        var $layer = this._cRoot.$layer();
        var classes = $layer.attr("class");
        var target = "-opacity-";
        var index = classes.indexOf(target);
        var degree = 9;
        if (index !== -1) {
            degree = parseInt(classes[index + target.length]);
            $layer.removeClass(target + degree.toString());
            if (++degree > 9) {
                degree = 0;
            }
        }
        $layer.addClass(target + degree.toString());
    }

    doKeyChannelDown() {
        var $layer = this._cRoot.$layer();
        var classes = $layer.attr("class");
        var target = "-opacity-";
        var index = classes.indexOf(target);
        var degree = 0;
        if (index !== -1) {
            degree = parseInt(classes[index + target.length]);
            $layer.removeClass(target + degree.toString());
            if (--degree < 0) {
                degree = 9;
            }
        }
        $layer.addClass(target + degree.toString());
    }

    doKey0() {
        require(["snippet/box/loader"], (aLoader) => {
            new aLoader(this._cRoot, __CONTROLS__, () => {
                this.notifyEndChildApp();
            });
        });
    }

    notifyEndChildApp() {
        console.log("info", "AppEnd");
        this.destroy();
    }
}
export = CTestMain;
