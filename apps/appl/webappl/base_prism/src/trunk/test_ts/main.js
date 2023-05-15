define(["require", "exports", 'pages/util/controls'], function (require, exports, __CONTROLS__) {
    var CTestMain = (function () {
        function CTestMain() {
            var _this = this;
            hx.svc.KEY_EVENT_BINDER.unBind();
            hx.svc.KEY_EVENT_BINDER.bind(document, this, function (aEvent) {
                _this.onKey(aEvent);
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
        CTestMain.prototype.destroy = function () {
            hx.svc.KEY_EVENT_BINDER.unBind();
            hx.svc.KEY_EVENT_BINDER.bind(document, hx.al.entity);
            document.body.removeChild(this._cRoot._$self[0]);
        };
        CTestMain.prototype.onKey = function (aEvent) {
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
        };
        CTestMain.prototype.doKeyEscape = function () {
            this.destroy();
        };
        CTestMain.prototype.doKeyChannelUp = function () {
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
        };
        CTestMain.prototype.doKeyChannelDown = function () {
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
        };
        CTestMain.prototype.doKey0 = function () {
            var _this = this;
            require(["snippet/box/loader"], function (aLoader) {
                new aLoader(_this._cRoot, __CONTROLS__, function () {
                    _this.notifyEndChildApp();
                });
            });
        };
        CTestMain.prototype.notifyEndChildApp = function () {
            console.log("info", "AppEnd");
            this.destroy();
        };
        return CTestMain;
    })();
    return CTestMain;
});
