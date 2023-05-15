var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", 'pages/util/controls'], function (require, exports, __CONTROLS__) {
    var mPrism;
    (function (mPrism) {
        var CDialog = (function (_super) {
            __extends(CDialog, _super);
            function CDialog(layeredGroupControl) {
                _super.call(this);
                this._stackIndex = -1;
                if (arguments.length) {
                    this.init(layeredGroupControl);
                }
            }
            CDialog.prototype.fnAfterTransition = function () {
            };
            CDialog.prototype.fnAfterDraw = function () {
            };
            CDialog.prototype.onContextMenuClose = function (aParam) {
            };
            CDialog.prototype.init = function (layeredGroupControl) {
                var a = this;
                _super.prototype.init.call(a);
                a.addClass('dlg');
                a._parent = layeredGroupControl;
                a._registerEvent(['Close', 'Closing']);
                a._blockKey = false;
                a.setCommonTag({
                    'optionMenuInd': 'optionMenuInd',
                    'optionMenuIndBg': 'optionMenuIndBg'
                });
            };
            CDialog.prototype.destroy = function () {
                var a = this;
                a.clearTimeout();
                delete a._timeoutValue;
                if (typeof a._closedParam === 'function') {
                    a._closedParam();
                }
                else {
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
            };
            CDialog.prototype.setTimeout = function (timeoutValue) {
                var a = this;
                a._timeoutValue = timeoutValue;
                clearTimeout(a._timer);
                a._timer = setTimeout(function () {
                    a.close('timedOut');
                }, timeoutValue);
            };
            CDialog.prototype.resetTimeout = function () {
                var a = this;
                if (a._timeoutValue) {
                    a.setTimeout(a._timeoutValue);
                }
            };
            CDialog.prototype.clearTimeout = function () {
                var a = this;
                clearTimeout(a._timer);
            };
            CDialog.prototype._onFocusHandlers = function (control, focusInfo) {
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
                    }
                    else {
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
            };
            CDialog.prototype.setOptionMenuProvider = function (optionMenuProvider) {
                var a = this;
                a._optionMenuProvider = optionMenuProvider;
            };
            CDialog.prototype.doKey = function (key) {
                var a = this;
                a.resetTimeout();
                var handled = CVerticalGroupControl.prototype.doKey.call(a, key);
                return handled;
            };
            CDialog.prototype.doKeyOption = function () {
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
            };
            CDialog.prototype.reloadContextMenuItems = function (control) {
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
            };
            CDialog.prototype.addOptionMenuTarget = function (control) {
                var a = this;
                (function (c) {
                    c.onActiveFocusedGained = function (newTarget) {
                        a._onFocusHandlers(c);
                    };
                    c.onActiveFocusedLost = function () {
                        a._onFocusHandlers(null);
                    };
                    c.onFocusChanged = function (oldTarget, newTarget) {
                        a._onFocusHandlers(c, newTarget);
                    };
                })(control);
            };
            CDialog.prototype.open = function (param) {
                var a = this;
                var createLayerParam = {};
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
                createLayerParam.fnAfterCreate = function () {
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
                                }
                                else if ($.browser.opera) {
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
            };
            CDialog.prototype.close = function (param) {
                var a = this;
                var layerName = a._layerName || null;
                a._closedParam = param;
                if (a._parent) {
                    if (a._parent.getFocusedChild() === a) {
                        a._parent.removeLayer(layerName);
                        prism['focusInterrupt'](a._parent.$layer(), false);
                    }
                    else {
                        a._parent.removeLayer(layerName || a);
                    }
                    delete a._parent;
                }
            };
            CDialog.prototype.doOpen = function (aParam) {
                throw 'must override';
            };
            CDialog.prototype.setBlockKey = function (param) {
                var a = this;
                a._blockKey = param;
            };
            CDialog.prototype.setOptionMenuDisplayDepth = function (param) {
                var a = this;
                a._optionMenuDisplayDepth = param;
            };
            return CDialog;
        })(__CONTROLS__.CVerticalGroupControl);
        mPrism.CDialog = CDialog;
        if (!window.prism) {
            window.prism = {};
        }
        prism.CDialog = CDialog;
    })(mPrism || (mPrism = {}));
    return mPrism;
});
