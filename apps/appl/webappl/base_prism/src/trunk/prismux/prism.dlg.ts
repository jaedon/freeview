/// <reference path="../def/prism.d.ts" />
// file name : prismux/prism.dlg.js
/*jslint sloppy:true, nomen:true, vars:true */
import __CONTROLS__ = require('pages/util/controls');

module mPrism {
    export class CDialog extends __CONTROLS__.CVerticalGroupControl {
        _parent: any;//: __CONTROLS__.CLayeredGroupControl;
        _blockKey: boolean;
        _openParam: any;
        _closedParam: any;
        _timer: number;
        _timeoutValue: any;
        _contextMenuDlg: any;
        $optionMenuInd: any;
        $optionMenuIndBg: any;
        _optionMenuProvider: any;
        _activeOptionMenuInfo: any;
        _buttonInfoList: any[];
        _stackIndex = -1;
        _optionMenuDisplayDepth: any;
        _layerName: String;
        constructor(layeredGroupControl?: __CONTROLS__.CLayeredGroupControl) {
            super();
            if (arguments.length) {
                this.init(layeredGroupControl);
            }
        }
        fnAfterTransition(): void {

        }
        fnAfterDraw(): void {

        }
        onContextMenuClose(aParam?): void {

        }
        init(layeredGroupControl: __CONTROLS__.CLayeredGroupControl) {
            var a = this;
            super.init.call(a);
            a.addClass('dlg');
            a._parent = layeredGroupControl;
            a._registerEvent(['Close', 'Closing']);
            a._blockKey = false;
            a.setCommonTag({
                'optionMenuInd': 'optionMenuInd',
                'optionMenuIndBg': 'optionMenuIndBg'
            });
        }
        destroy() {
            var a = this;
            a.clearTimeout();
            delete a._timeoutValue;
            if (typeof a._closedParam === 'function') {
                a._closedParam();
            } else {
                a._fireEvent('Close', a._closedParam, a._stackIndex);
            }
            if (a._openParam) {
                if (a._openParam.onClosed) {
                    var onClosed = a._openParam.onClosed;
                    var closedParam = a._closedParam;
                    onClosed(closedParam);
                }
            }
            if (a._contextMenuDlg) {
                delete a._contextMenuDlg;
                a._contextMenuDlg = undefined;
            }
            if (a._activeOptionMenuInfo) {
                delete a._activeOptionMenuInfo;
                a._activeOptionMenuInfo = undefined;
            }
            if (a._optionMenuProvider) {
                delete a._optionMenuProvider;
                a._optionMenuProvider = undefined;
            }
            if (a.$optionMenuInd && a.$optionMenuInd()) {
                a.$optionMenuInd().remove();
            }
            if (a.$optionMenuIndBg && a.$optionMenuIndBg()) {
                a.$optionMenuIndBg().remove();
            }
            delete a._layerName;
            delete a._openParam;
            delete a._closedParam;
        }
        setTimeout(timeoutValue) {
            var a = this;
            a._timeoutValue = timeoutValue;
            clearTimeout(a._timer);
            a._timer = setTimeout(function () {
                a.close('timedOut');
            }, timeoutValue);
        }
        resetTimeout() {
            var a = this;
            if (a._timeoutValue) {
                a.setTimeout(a._timeoutValue);
            }
        }
        clearTimeout() {
            var a = this;
            clearTimeout(a._timer);
        }
        _onFocusHandlers(control, focusInfo?) {
            var a = this;
            if (a._optionMenuProvider && !a._contextMenuDlg) {
                var activeFocusInfo = {
                    control: control,
                    $item: focusInfo
                };
                var optionMenuInfo = a._optionMenuProvider(activeFocusInfo);
                if (optionMenuInfo) {
                    optionMenuInfo.control = control;
                    a._activeOptionMenuInfo = optionMenuInfo;
                    if (a.$optionMenuIndBg) {
                        a.$optionMenuIndBg();
                    }
                    if (a.$optionMenuInd && a.$optionMenuInd()) {
                        a.$optionMenuInd().text(optionMenuInfo.text);
                    }
                } else {
                    if (a._activeOptionMenuInfo) {
                        delete a._activeOptionMenuInfo;
                        a._activeOptionMenuInfo = undefined;
                    }
                    if (a.$optionMenuInd && a.$optionMenuInd()) {
                        a.$optionMenuInd().remove();
                    }
                    if (a.$optionMenuIndBg && a.$optionMenuIndBg()) {
                        a.$optionMenuIndBg().remove();
                    }
                }
            }
        }
        setOptionMenuProvider(optionMenuProvider) {
            var a = this;
            a._optionMenuProvider = optionMenuProvider;
        }
        doKey(key?) {
            var a = this;
            a.resetTimeout();
            var handled = CVerticalGroupControl.prototype.doKey.call(a, key);
            return handled;
        }
        doKeyOption() {
            var a = this, i, controls = a._controls.slice(0);
            for (i = 0; i < controls.length; i += 1) {
                var c = controls[i];
                if (c.hasFocus() && c['doKeyOption'] && c['doKeyOption']()) {
                    return true;
                }
            }
            if (a._activeOptionMenuInfo) {
                var contextMenuDlg = prism.createContextMenu(a._parent);
                contextMenuDlg.setContextMenuInfo(a._activeOptionMenuInfo);
                contextMenuDlg.setDisplayDepth(a._optionMenuDisplayDepth);
                contextMenuDlg.open({
                    onClosed: function (param) {
                        if (a.onContextMenuClose) {
                            a.onContextMenuClose(param);
                        }
                        if (a._contextMenuDlg) {
                            delete a._contextMenuDlg;
                            a._contextMenuDlg = undefined;
                        }
                    },
                    drawParam: {
                        $parent: a._$self
                    }
                });
                a._contextMenuDlg = contextMenuDlg;
                return true;
            }
            return false;
        }
        reloadContextMenuItems(control) {
            var a = this;
            var optionMenuInfo;
            if (!a._contextMenuDlg) {
                if (a._activeOptionMenuInfo) {
                    delete a._activeOptionMenuInfo;
                    a._activeOptionMenuInfo = undefined;
                }
                if (a.$optionMenuInd && a.$optionMenuInd()) {
                    a.$optionMenuInd().remove();
                }
                if (a.$optionMenuIndBg && a.$optionMenuIndBg()) {
                    a.$optionMenuIndBg().remove();
                }
            }
            a._contextMenuDlg.setContextMenuInfo(a._activeOptionMenuInfo);
        }
        addOptionMenuTarget(control) {
            var a = this;
            ((c) => {
                c.onActiveFocusedGained = (newTarget) => {
                    a._onFocusHandlers(c);
                };
                c.onActiveFocusedLost = () => {
                    a._onFocusHandlers(null);
                };
                c.onFocusChanged = (oldTarget, newTarget) => {
                    a._onFocusHandlers(c, newTarget);
                };
            })(control);
        }
        open(param?) {
            var a = this;
            var createLayerParam: any = {};
            if (param) {
                param.visibleAfterDraw = true;
            }
            a._openParam = param;
            if (param) {
                if (param.dimm) {
                    createLayerParam.addClass = 'dimm';
                }
                if (param.zindex) {
                    createLayerParam.zindex = param.zindex;
                }
                if (param.transition) {
                    createLayerParam.transition = param.transition;
                    if (a.fnAfterTransition) {
                        createLayerParam.transition.fnAfterTransition = function () {
                            a.fnAfterTransition();
                            if (createLayerParam.fnAfterTransition) {
                                createLayerParam.fnAfterTransition();
                            }
                        };
                    }
                }
                if (param.layerName) {
                    a._layerName = param.layerName;
                }
            }
            createLayerParam.fnAfterCreate = () => {
                a.doOpen(param);
                a._parent.setChildControl([a]);
                a._parent.draw({
                    setActiveFocus: true
                });
                if (a.fnAfterDraw) {
                    a.fnAfterDraw();
                }
                if (param && param.transition) {
                    var newLayer = param.transition.newLayer;
                    var cssValue = {};
                    switch (newLayer) {
                        case 'scaleY':
                            if ($.browser.webkit) {
                                cssValue['-webkit-transform'] = 'scale(1, 0)';
                                cssValue['-webkit-transition'] = 'all .3s linear';
                                a._$self.css(cssValue);
                                setTimeout(function () {
                                    a._$self.css('-webkit-transform', 'scale(1, 1)');
                                }, 1);
                            } else if ($.browser.opera) {
                                cssValue['-o-transform'] = 'scale(1, 0)';
                                cssValue['-o-transition'] = 'all .3s linear';
                                a._$self.css(cssValue);
                                setTimeout(function () {
                                    a._$self.css('-o-transform', 'scale(1, 1)');
                                }, 1);
                            }
                            break;
                    }
                }
            };
            prism.focusInterrupt(a._parent.$layer(), true);
            this._stackIndex = a._parent.createLayer(createLayerParam);
            a._shown = true;
            prism.processRequest(CDialog, null, 'showMsgbox', {
                'status': true
            }, function (response) {
            });
        }
        close(param?) {
            var a = this;
            var layerName = a._layerName || null;
            a._closedParam = param;
            if (a._parent) {
                if (a._parent.getFocusedChild() === a) {
                    a._parent.removeLayer(layerName);
                    prism['focusInterrupt'](a._parent.$layer(), false);
                } else {
                    a._parent.removeLayer(layerName || a);
                }
                delete a._parent;
            }
        }
        doOpen(aParam?) {
            throw 'must override';
        }
        setBlockKey(param) {
            var a = this;
            a._blockKey = param;
        }
        setOptionMenuDisplayDepth(param) {
            var a = this;
            a._optionMenuDisplayDepth = param;
        }
    }

    if (!(<any>window).prism) {
        (<any>window).prism = {}
    }

    prism.CDialog = CDialog;
}
export = mPrism;
