/**
 * Created by bglee@humaxdigital.com on 2014. 8. 8..
 * Button Promotion class
 * ex: green button, hd watch, etc...
 */
/// <reference path="../../def/sugar.d.ts" />
/// <reference path="../../def/layout.d.ts" />
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports"], function (require, exports) {
    var CButtonPromotion = (function () {
        function CButtonPromotion(aElement) {
            this._isBlocked = false;
            this._element = aElement;
        }
        CButtonPromotion.prototype.showButton = function (aForceShowing) {
            if (aForceShowing || !this._isBlocked) {
                this._element.classList.add(CButtonPromotion.CLASS_SHOW);
                this._isBlocked = false;
            }
            return this._doShowButton();
        };
        CButtonPromotion.prototype.hideButton = function (aKeepHiding) {
            this._element.classList.remove(CButtonPromotion.CLASS_SHOW);
            this._isBlocked = aKeepHiding;
            this._doHideButton();
        };
        CButtonPromotion.prototype.isShowing = function () {
            return this._element.classList.contains(CButtonPromotion.CLASS_SHOW);
        };
        CButtonPromotion.prototype.getElement = function () {
            return this._element;
        };
        CButtonPromotion.prototype._doShowButton = function () {
            //TODO "do implement at inherited class";
            return false;
        };
        CButtonPromotion.prototype._doHideButton = function () {
            //TODO "do implement at inherited class";
        };
        CButtonPromotion.prototype.setData = function (aData) {
            if (aData === void 0) { aData = null; }
            this._data = aData;
        };
        CButtonPromotion.prototype.getData = function () {
            return this._data;
        };
        CButtonPromotion.CLASS_SHOW = "-show";
        return CButtonPromotion;
    })();
    exports.CButtonPromotion = CButtonPromotion;
    if (!ENV.isModeRelease) {
        var __fnDebugger = function (aElementID) {
            var el = document.createElement("div");
            el.id = aElementID;
            el.classList.add("-debug");
            document.getElementById("DIA").appendChild(el);
            return el;
        };
    }
    var CGreenButtonPromotion = (function (_super) {
        __extends(CGreenButtonPromotion, _super);
        function CGreenButtonPromotion() {
            var _this = this;
            _super.call(this, document.createElement("div"));
            $(document).on("load", "img .test", function () {
            });
            var elContainer = this.getElement();
            var elText = document.createElement("div");
            var elIcon = document.createElement("div");
            var elImageIcon = document.createElement("img");
            elContainer.setAttribute("id", CGreenButtonPromotion.ID_GREEN_BUTTON);
            elText.setAttribute("class", CGreenButtonPromotion.CLASS_TEXT);
            elIcon.setAttribute("class", CGreenButtonPromotion.CLASS_GREEN_BUTTON);
            elImageIcon.setAttribute("class", CGreenButtonPromotion.CLASS_IMAGE_ICON);
            elText.appendChild(elIcon);
            elContainer.appendChild(elText);
            elContainer.appendChild(elImageIcon);
            document.getElementById("DIA").appendChild(elContainer);
            elImageIcon.onerror = function () {
                _this.hideButton();
            };
            elImageIcon.alt = "";
            this._elDefault = elText;
            this._elImage = elImageIcon;
        }
        CGreenButtonPromotion.prototype.setDebugModeToggle = function () {
            if (this._debugger) {
                this._debugger.parentElement.removeChild(this._debugger);
                delete this._debugger;
            }
            else {
                this._debugger = __fnDebugger("debugger-green-button");
                this.showButton();
            }
        };
        CGreenButtonPromotion.prototype.setData = function (aRctInfoCollection) {
            if (aRctInfoCollection === void 0) { aRctInfoCollection = null; }
            _super.prototype.setData.call(this, aRctInfoCollection);
        };
        CGreenButtonPromotion.prototype.getData = function () {
            return _super.prototype.getData.call(this);
        };
        CGreenButtonPromotion.prototype.getRctInfoCount = function () {
            var rctInfoCollection = this.getData();
            var ret = 0;
            if (rctInfoCollection) {
                var item;
                for (var i = 0, len = rctInfoCollection.totalLinkCnt; i < len; i++) {
                    item = rctInfoCollection.item(i);
                    if (item) {
                        ret++;
                    }
                    else {
                        break;
                    }
                }
            }
            return ret;
        };
        CGreenButtonPromotion.prototype.showButton = function (aForceShowing) {
            var ret = false;
            if (this.getData()) {
                ret = _super.prototype.showButton.call(this, aForceShowing);
            }
            return ret;
        };
        CGreenButtonPromotion.prototype._doShowButton = function () {
            var _this = this;
            var showDefaultIcon = function (aPosition) {
                if (aPosition) {
                    _this._elDefault.style.left = aPosition[0] + "px";
                    _this._elDefault.style.top = aPosition[1] + "px";
                }
                else {
                    _this._elDefault.style.left = "";
                    _this._elDefault.style.top = "";
                }
                _this._elDefault.classList.add(CButtonPromotion.CLASS_SHOW);
                _this._elImage.classList.remove(CButtonPromotion.CLASS_SHOW);
            };
            var uri = null;
            var position = null;
            var rctInfoCollection = this.getData();
            var rctInfo = rctInfoCollection.item(0);
            var imageInfo = rctInfoCollection.imageIconInfo;
            var showDescriptorIcon = function (aFxIfFail) {
                var positions;
                var scailingWidth = 1;
                var scailingHeight = 1;
                var mwIcon;
                var workaround = false;
                var osdScreenWidth = 1280;
                var osdScreenHeight = 720;
                var rctIconWidth = 167;
                var rctIconHeight = 29;
                var positionFlag;
                var mwIconTypes = {
                    "/png": "file://tmp/RctImageIcon.png",
                    "/jpg": "file://tmp/RctImageIcon.jpg",
                    "/jpeg": "file://tmp/RctImageIcon.jpg"
                };
                var timeoutTimer = -1;
                var compensation = [
                    [720, 576],
                    [1280, 720],
                    [1920, 1080]
                ][imageInfo.coordinateSystem];
                uri = rctInfoCollection.imageIconInfo.iconUrl;
                if (!uri) {
                    mwIcon = mwIconTypes[imageInfo.iconType];
                }
                else if (uri.indexOf("dvb:/") !== -1) {
                    mwIcon = "file://tmp/RctImageIcon.dvb";
                    workaround = true;
                }
                if (mwIcon) {
                    uri = mwIcon;
                }
                positionFlag = imageInfo.positionFlag;
                if (positionFlag) {
                    position = imageInfo.horizontalOrigin + "x" + imageInfo.verticalOrigin;
                }
                if (compensation instanceof Array) {
                    scailingWidth = (osdScreenWidth / compensation[0]);
                    scailingHeight = (osdScreenHeight / compensation[1]);
                }
                if (position) {
                    positions = position.split("x");
                    hx.log("debug", "position info: " + positions);
                    _this._elImage.style.left = ((positions[0] || 0) * scailingWidth) + "px";
                    _this._elImage.style.top = ((positions[1] || 0) * scailingHeight) + "px";
                }
                if (uri) {
                    uri += "?" + Date.now();
                    _this._elImage.onload = function () {
                        hx.log("warning", "[onload] width: " + _this._elImage.naturalWidth);
                        hx.log("warning", "[onload] height: " + _this._elImage.naturalHeight);
                        clearTimeout(timeoutTimer);
                        if (positionFlag) {
                            rctIconWidth = _this._elImage.naturalWidth * scailingWidth;
                            rctIconHeight = _this._elImage.naturalHeight * scailingHeight;
                        }
                        _this._elImage.style.width = rctIconWidth + "px";
                        _this._elImage.style.height = rctIconHeight + "px";
                    };
                    if (workaround) {
                        _this._elImage.onerror = (function () {
                            var opportunities = 2;
                            return function () {
                                var _this = this;
                                hx.log("warning", "[onerror]");
                                if (opportunities-- > 0) {
                                    setTimeout(function () {
                                        hx.log("warning", "[onerror]: retry opportunities: " + opportunities);
                                        _this.src = uri;
                                    }, 1 * 500);
                                }
                                else {
                                    aFxIfFail(positions);
                                }
                            };
                        })();
                    }
                    else {
                        _this._elImage.onerror = function () {
                            hx.log("warning", "[onerror]");
                            clearTimeout(timeoutTimer);
                            aFxIfFail(positions);
                        };
                    }
                    if (!workaround) {
                        timeoutTimer = setTimeout(function () {
                            aFxIfFail(positions);
                        }, 5000);
                    }
                    _this._elImage.src = "";
                    _this._elImage.src = uri;
                    hx.log("info", "Uri: " + uri);
                }
                else {
                    aFxIfFail(positions);
                }
                _this._elDefault.classList.remove(CButtonPromotion.CLASS_SHOW);
                _this._elImage.classList.add(CButtonPromotion.CLASS_SHOW);
            };
            if (rctInfo.isDefaultIcon) {
                if (rctInfoCollection.imageIconInfo.iconId > 0) {
                    showDescriptorIcon(function (aPosition) {
                        showDefaultIcon(aPosition);
                    });
                }
                else {
                    showDefaultIcon();
                }
            }
            else {
                if (rctInfoCollection.imageIconInfo.iconId > 0) {
                    showDescriptorIcon(function () {
                        _this.hideButton();
                    });
                }
                else {
                    this.hideButton();
                }
            }
            if (!ENV.isModeRelease && this._debugger) {
                var debugInfo = [
                    "URI: " + uri,
                    "positionFlag: " + imageInfo.positionFlag,
                    "position: " + position,
                    "isDefaultIcon in RctInfo: " + (rctInfo.isDefaultIcon ? 1 : 0),
                    "iconId in RctInfoCollection: " + rctInfoCollection.imageIconInfo.iconId,
                    "coordinatingSystem: " + imageInfo.coordinateSystem,
                    "width: " + this._elImage.style.width,
                    "height: " + this._elImage.style.height
                ];
                this._debugger.innerHTML = debugInfo.join("<br>");
            }
            return this._elImage.classList.contains(CButtonPromotion.CLASS_SHOW) || this._elDefault.classList.contains(CButtonPromotion.CLASS_SHOW);
        };
        CGreenButtonPromotion.prototype._doHideButton = function () {
            this._elImage.style.left = "";
            this._elImage.style.top = "";
        };
        CGreenButtonPromotion.prototype.isShowing = function () {
            var ret = this.getElement().classList.contains(CButtonPromotion.CLASS_SHOW);
            return ret;
        };
        CGreenButtonPromotion.ID_GREEN_BUTTON = "green-button";
        CGreenButtonPromotion.CLASS_GREEN_BUTTON = "-green-button";
        CGreenButtonPromotion.CLASS_TEXT = "-text";
        CGreenButtonPromotion.CLASS_IMAGE_ICON = "-image-icon";
        return CGreenButtonPromotion;
    })(CButtonPromotion);
    exports.CGreenButtonPromotion = CGreenButtonPromotion;
    var CWatchInHDPromotion = (function (_super) {
        __extends(CWatchInHDPromotion, _super);
        function CWatchInHDPromotion(aElement) {
            _super.call(this, aElement);
        }
        return CWatchInHDPromotion;
    })(CButtonPromotion);
    exports.CWatchInHDPromotion = CWatchInHDPromotion;
});
