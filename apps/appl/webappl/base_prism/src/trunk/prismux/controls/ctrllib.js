/// <reference path="../../def/jquery.d.ts"/>
/// <reference path="../../def/require.d.ts"/>
/// <reference path="common.ts"/>
/// <reference path='ctrlbase.ts'/>
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "./ctrlbase"], function (require, exports, CtrlBase) {
    var CtrlLib;
    (function (CtrlLib) {
        console.log("CtrlLib Start");
        var CFsatMediaListControl = (function (_super) {
            __extends(CFsatMediaListControl, _super);
            function CFsatMediaListControl(aElement, aDrawer, aData, aDataRoll) {
                if (aData === void 0) { aData = []; }
                if (aDataRoll === void 0) { aDataRoll = false; }
                _super.call(this, aElement);
                this.registerSignal([CFsatMediaListControl.KEventEmpty]);
                this._element.classList.add(CFsatMediaListControl.KClassFsatMediaList);
                this.setItemWidth(764);
                this.setItemHeight(50);
                this.setOrientation(1 /* EVertical */);
                this.setOwnedDataProvider(this.doGetDataProvider(aData, aDataRoll));
                this.setScrollScheme(3 /* EByFixedFocusRemains */);
                this.setDataRolling(aDataRoll);
                this.setDataDrawer(aDrawer || CFsatMediaListControl.FN_DRAWER_MEDIA);
                var elEmpty = document.createElement('div');
                elEmpty.classList.add(CFsatMediaListControl.KClassEmpty);
                elEmpty.classList.add(CFsatMediaListControl.KClassLoading);
                this._elEmpty = elEmpty;
                this._elEmpty.innerText = 'Loading...';
                this._container.appendChild(elEmpty);
                this.draw();
            }
            CFsatMediaListControl.prototype.setListData = function (aData, aDataRoll) {
                this.setDataRolling(aDataRoll);
                this.setOwnedDataProvider(this.doGetDataProvider(aData, aDataRoll));
            };
            CFsatMediaListControl.prototype.doGetDataProvider = function (aData, aDataRolling) {
                return new CtrlBase.CMediaListDataProvider(aData, aDataRolling);
            };
            CFsatMediaListControl.prototype.checkEmpty = function (aByUpdate) {
                /* COMMENT
                 * replaced this._keyMap to this.getItemLength();
                 * * bglee@humaxdigital.com, 오후 6:28, 2013. 10. 30.
                 */
                var count = this.getItemLength();
                if (count) {
                    this._elEmpty.classList.add(CFsatMediaListControl.KClassHide);
                }
                else {
                    this._elEmpty.classList.remove(CFsatMediaListControl.KClassHide);
                    this._elEmpty.classList.remove(CFsatMediaListControl.KClassLoading);
                    if (aByUpdate) {
                        this._elEmpty.innerText = 'No items';
                        this._emitEmpty();
                    }
                }
            };
            CFsatMediaListControl.prototype.draw = function (aRect) {
                _super.prototype.draw.call(this, aRect);
                this.checkEmpty();
            };
            CFsatMediaListControl.prototype.update = function (aFirstCalled) {
                if (aFirstCalled === void 0) { aFirstCalled = true; }
                _super.prototype.update.call(this);
                this.checkEmpty(true);
            };
            CFsatMediaListControl.prototype.connectEmpty = function (aHolder, aSlotName, aSlot) {
                this.connect("Empty", aHolder, aSlotName);
            };
            CFsatMediaListControl.prototype._emitEmpty = function () {
                this.emit.call(this, "Empty");
            };
            CFsatMediaListControl.FN_DRAWER_MEDIA = (function () {
                var _fnCreateDiv = function (aClass, aInnertText) {
                    var elDiv = document.createElement('div');
                    elDiv.classList.add(aClass);
                    if (aInnertText !== undefined) {
                        elDiv.innerText = aInnertText;
                    }
                    return elDiv;
                };
                var _fnGetContentType = function (aContentType) {
                    /* to test */
                    var contentType = [
                        '-folder',
                        '-folder-plus',
                        '-video',
                        '-photo',
                        '-music'
                    ];
                    return contentType[aContentType];
                };
                var _fnGetDuration = function (aDuration) {
                    if (aDuration > 0 && aDuration < 60) {
                        return aDuration + ' sec';
                    }
                    aDuration = Math.floor(aDuration / 60) + ((aDuration % 60) >= 30 ? 1 : 0);
                    var hrs = Math.floor(aDuration / 60);
                    var mins = aDuration % 60;
                    var hrTxt = (hrs === 0 ? "" : hrs + (hrs == 1 ? " hr " : " hrs "));
                    var minTxt = (mins === 0 ? "" : mins + (mins == 1 ? " min " : " mins "));
                    return hrTxt + minTxt;
                };
                var FN_DRAWER_MEDIA = function (aKey, aItem, aEl) {
                    var elFrame = _fnCreateDiv('-frame');
                    var elMediaType = _fnCreateDiv('-media-type');
                    var elTitle = _fnCreateDiv('-title', aItem.title);
                    var elDuration = _fnCreateDiv('-duration', _fnGetDuration(aItem.duration));
                    elMediaType.classList.add(_fnGetContentType(aItem.contentType));
                    elFrame.appendChild(elMediaType);
                    elFrame.appendChild(elTitle);
                    elFrame.appendChild(elDuration);
                    aEl.appendChild(elFrame);
                    return 2 /* KFocusAble */;
                };
                return FN_DRAWER_MEDIA;
            })();
            CFsatMediaListControl.KClassFsatMediaList = '-fsat-media-list';
            CFsatMediaListControl.KClassLoading = '-loading';
            CFsatMediaListControl.KClassEmpty = '-empty';
            CFsatMediaListControl.KClassHide = '-hide';
            CFsatMediaListControl.KEventEmpty = 'Empty';
            return CFsatMediaListControl;
        })(CtrlBase.CMediaListControl);
        CtrlLib.CFsatMediaListControl = CFsatMediaListControl;
        var CFsatMediaShareListControl = (function (_super) {
            __extends(CFsatMediaShareListControl, _super);
            function CFsatMediaShareListControl() {
                _super.apply(this, arguments);
            }
            CFsatMediaShareListControl.prototype.doGetDataProvider = function (aData, aDataRolling) {
                return new CtrlBase.CMediaMediaShareDataProvider(aData, aDataRolling);
            };
            return CFsatMediaShareListControl;
        })(CFsatMediaListControl);
        CtrlLib.CFsatMediaShareListControl = CFsatMediaShareListControl;
        var CHelpString = (function () {
            function CHelpString(aElRoot, aListControl, aArrayKeySet, aCustomStringSet) {
                aElRoot.classList.add(CHelpString.KClassHelpString);
                aListControl.connectFocusedDataItemChanged(this, "_slFocusedDataItemChanged", this._slFocusedDataItemChanged);
                aListControl.connectTransitioningChanged(this, "_slTransitioningChanged", this._slTransitioningChanged);
                this._elRoot = aElRoot;
                this._ifHelp = this._makeIfHelpString(aArrayKeySet, aCustomStringSet);
                this._listControl = aListControl;
                this.update();
            }
            CHelpString.prototype.destroy = function () {
                this._listControl.disconnect(this);
            };
            CHelpString.prototype.addClass = function (aClass) {
                this._elRoot.classList.add(aClass);
            };
            CHelpString.prototype.removeClass = function (aClass) {
                this._elRoot.classList.remove(aClass);
            };
            CHelpString.prototype._slTransitioningChanged = function (aTransitioning) {
                var _this = this;
                if (!aTransitioning) {
                    setTimeout(function () {
                        _this.update();
                    }, 100);
                }
            };
            CHelpString.prototype._slFocusedDataItemChanged = function (aKey, aItem, aEl) {
                this.update();
            };
            CHelpString.prototype._makeIfHelpString = function (aKeyString, aCustomStringSet) {
                if (aCustomStringSet === void 0) { aCustomStringSet = []; }
                var fnCreateElementWithClass = function (aClass, aIndex, aString) {
                    var el = document.createElement('div');
                    el.classList.add('-item');
                    el.classList.add('-index0' + aIndex);
                    el.classList.add(aClass);
                    el.innerText = aString;
                    return el;
                };
                var i, len;
                var elRoot = this._elRoot;
                var ret = [];
                var keyString;
                var element;
                for (i = 0, len = aKeyString.length; i < len; i++) {
                    keyString = aKeyString[i];
                    element = fnCreateElementWithClass(keyString, i, aCustomStringSet[i]);
                    ret.push({
                        element: element,
                        keyString: keyString
                    });
                    elRoot.appendChild(element);
                }
                element = fnCreateElementWithClass('-navi', i, '');
                ret.push({
                    element: element,
                    keyString: ''
                });
                elRoot.appendChild(element);
                return ret;
            };
            CHelpString.prototype.update = function () {
                var i, len, item, key, items = this._ifHelp;
                for (i = 0, len = items.length; i < len; i++) {
                    item = items[i];
                    key = item.keyString;
                    if (key) {
                        if (this._isPossible(item.keyString)) {
                            item.element.classList.remove(CHelpString.KClassDisable);
                        }
                        else {
                            item.element.classList.add(CHelpString.KClassDisable);
                        }
                    }
                    else {
                        this._updateNaviInfo(item.element);
                    }
                }
            };
            CHelpString.prototype._isPossible = function (aKeyString) {
                return this._listControl.isMovable(aKeyString);
            };
            CHelpString.prototype._updateNaviInfo = function (aEl) {
                var list = this._listControl;
                var onePage = list.getCountOnPage();
                var count = list.getItemLength();
                var focusedItem = list.getFocusedElement();
                var scheme = list.getVerticalScrollScheme();
                var text = '';
                if (focusedItem) {
                    if (scheme === 3 /* EByFixedFocusRemains */) {
                        var pos = focusedItem.attributes['data'];
                        var index = list.getFocusedElementIndex();
                        var countNextView = onePage - 1;
                        var pageLast = Math.floor(count / countNextView);
                        var quotient = Math.floor(pos / countNextView);
                        var reminder = pos % countNextView;
                        if (reminder === 0 && index !== 0) {
                            quotient = quotient - 1;
                        }
                        var start = quotient * countNextView;
                        var end = pageLast === quotient ? count : Math.min(count, start + onePage);
                        text = (start + 1) + '-' + end;
                    }
                    else {
                        throw 'do implement other scheme algorithm';
                    }
                    text += ' of ' + count;
                }
                aEl.innerHTML = text;
            };
            CHelpString.KClassHelpString = '-help-string';
            CHelpString.KClassDisable = '-disable';
            return CHelpString;
        })();
        CtrlLib.CHelpString = CHelpString;
        console.log("CtrlLib End");
    })(CtrlLib || (CtrlLib = {}));
});
