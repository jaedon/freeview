var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", 'pages/util/controls'], function (require, exports, __CONTROLS__) {
    var mPrism;
    (function (mPrism) {
        (function (EViewType) {
            EViewType[EViewType["ELIST"] = 0] = "ELIST";
            EViewType[EViewType["EGRID"] = 1] = "EGRID";
        })(mPrism.EViewType || (mPrism.EViewType = {}));
        var EViewType = mPrism.EViewType;
        var CPage = (function (_super) {
            __extends(CPage, _super);
            function CPage($el) {
                _super.call(this, $el);
            }
            CPage.prototype.init = function ($el) {
                var a = this;
                _super.prototype.init.call(a, $el);
                a.setCommonTag({
                    'menuArea': 'menuArea',
                    'contentArea': 'contentArea',
                    'bottomBg': 'bottomBg',
                    'optionMenuInd': 'optionMenuInd',
                    'optionMenuIndBg': 'optionMenuIndBg'
                });
                a.setContextualProperties([
                    '_activeOptionMenuInfo'
                ]);
                setTimeout(function () {
                    a._$self.addClass('show');
                }, 1);
            };
            CPage.prototype.destroy = function () {
                var a = this;
                prism.clock.stopClock();
                prism.clock.removeEventHandler(a);
                if (a._contextMenu) {
                    delete a._contextMenu;
                    a._contextMenu = undefined;
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
                CLayeredGroupControl.prototype.destroy.call(a);
            };
            CPage.prototype.onContextMenuClose = function (aParam) {
            };
            CPage.prototype.setTitleText = function (html) {
                var a = this;
                if (!a._$titleArea) {
                    var $title = $('<div>', {
                        'class': 'title'
                    });
                    var $date = $('<div>', {
                        'class': 'date'
                    });
                    var $time = $('<div>', {
                        'class': 'time'
                    });
                    var $titleArea = $('<div>', {
                        'class': 'titleArea'
                    }).append($title).append($('<div>', {
                        'class': 'dateTime'
                    }).append($date).append($time));
                    $title.html(html);
                    a.$layer().append($titleArea);
                    a._$date = $date;
                    a._$time = $time;
                    a._$title = $title;
                    a._$titleArea = $titleArea;
                    a.showDateTime();
                }
                else {
                    a._$title.html(html);
                }
            };
            CPage.prototype.setTitleTextWithStorageStatus = function (html, $storageStatusArea) {
                var a = this;
                if (!a._$titleArea) {
                    var $title = $('<div>', {
                        'class': 'title'
                    });
                    var $titleArea = $('<div>', {
                        'class': 'titleArea'
                    }).append($title);
                    if (!!$storageStatusArea) {
                        $titleArea.append($storageStatusArea);
                    }
                    $title.html(html);
                    a.$layer().append($titleArea);
                    a._$title = $title;
                    a._$titleArea = $titleArea;
                }
                else {
                    a._$title.html(html);
                }
            };
            CPage.prototype.changeTitleText = function (html) {
                var a = this;
                if (!a._$titleArea) {
                    a.setTitleText(html);
                }
                else {
                    var $newTitle = $('<div>', {
                        'class': 'newTitle'
                    }).html(html);
                    a._$title.animate({
                        opacity: 0,
                        top: "-=30"
                    }, 300, function () {
                        a._$titleArea.remove('.title');
                        a._$title = $newTitle;
                    });
                    a._$titleArea.append($newTitle);
                    $newTitle.fadeIn(400, function () {
                        $newTitle.addClass('title').removeClass('newTitle');
                    });
                }
            };
            CPage.prototype.showDateTime = function () {
                var a = this;
                var now = new Date();
                prism.clock.startClock();
                a.setDateText(prism.getDateText(now));
                a.setTimeText(prism.getTimeText(now));
                prism.clock.addEventHandler('UpdateDateText', a, function (dateText) {
                    a.setDateText(dateText);
                });
                prism.clock.addEventHandler('UpdateTimeText', a, function (timeText) {
                    a.setTimeText(timeText);
                });
            };
            CPage.prototype.setDateText = function (text) {
                var a = this;
                a._$date.text(text);
            };
            CPage.prototype.setTimeText = function (text) {
                var a = this;
                a._$time.text(text);
            };
            CPage.prototype.setOptionMenuProvider = function (optionMenuProvider) {
                var a = this;
                a._optionMenuProvider = optionMenuProvider;
            };
            CPage.prototype.doKeyOption = function () {
                var a = this;
                if (a._optionMenuOnGoing === 'schduled.delete') {
                    // TODO :  ExceptionHandling
                    // Recordings Schedule ����  Delete �߿� Option�� ���� ���ϸ� Delete �Ϸ� �Ǹ鼭 Ŀ���� �̵��ϴµ� �׷���� �������� ���δ�.
                    // �׷��� Delete �������϶��� OPtion ���� ���� �� �F��.
                    return true;
                }
                if (a._contextMenu) {
                    a._contextMenu.close();
                    return true;
                }
                else if (a._activeOptionMenuInfo && !(a._activeOptionMenuInfo.control && a._activeOptionMenuInfo.control.isTransitioning())) {
                    var contextMenu = prism.createContextMenu(a);
                    contextMenu.setContextMenuInfo(a._activeOptionMenuInfo);
                    if (a._activeOptionMenuInfo.accept && !a._activeOptionMenuInfo.accept())
                        return true;
                    contextMenu.open({
                        onClosed: function (param) {
                            if (a.onContextMenuClose) {
                                a.onContextMenuClose(param);
                            }
                            if (a._contextMenu) {
                                delete a._contextMenu;
                                a._contextMenu = undefined;
                            }
                        }
                    });
                    a._contextMenu = contextMenu;
                    return true;
                }
                else {
                    return false;
                }
            };
            CPage.prototype.draw = function (param) {
                var a = this;
                _super.prototype.draw.call(a, param);
                var $contentArea = a.$contentArea();
                setTimeout(function () {
                    $contentArea.addClass('show');
                }, 800);
            };
            CPage.prototype.reloadContextMenuItems = function (control) {
                var a = this;
                var optionMenuInfo;
                if (control) {
                    if (a._optionMenuProvider) {
                        optionMenuInfo = a._optionMenuProvider({
                            control: control
                        });
                    }
                    if (optionMenuInfo) {
                        a._activeOptionMenuInfo = optionMenuInfo;
                        if (a._contextMenu) {
                            a._contextMenu.setContextMenuInfo(a._activeOptionMenuInfo);
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
                    }
                }
                else {
                    var activeFocusInfo = a.getActiveFocusInfo();
                    if (a._optionMenuProvider) {
                        optionMenuInfo = a._optionMenuProvider(activeFocusInfo);
                    }
                    if (optionMenuInfo) {
                        a._activeOptionMenuInfo = optionMenuInfo;
                        if (a._contextMenu) {
                            a._contextMenu.setContextMenuInfo(a._activeOptionMenuInfo);
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
                    }
                }
            };
            CPage.prototype.addOptionMenuTarget = function (control) {
                var a = this;
                (function (c) {
                    var onActiveFocusedGained, onActiveFocusedLost, onFocusChanged;
                    if (c.onActiveFocusedGained) {
                        onActiveFocusedGained = c.onActiveFocusedGained;
                    }
                    if (c.onActiveFocusedLost) {
                        onActiveFocusedLost = c.onActiveFocusedLost;
                    }
                    if (c.onFocusChanged) {
                        onFocusChanged = c.onFocusChanged;
                    }
                    c.onActiveFocusedGained = function () {
                        a._onFocusHandlers(c);
                        if (onActiveFocusedGained) {
                            onActiveFocusedGained.call(c);
                        }
                    };
                    c.onActiveFocusedLost = function () {
                        a._onFocusHandlers(null);
                        if (onActiveFocusedLost) {
                            onActiveFocusedLost.call(c);
                        }
                    };
                    if (c.onFocusChanged) {
                        onFocusChanged = c.onFocusChanged;
                    }
                    c.onFocusChanged = function (oldTarget, newTarget) {
                        a._onFocusHandlers(c, newTarget);
                        if (onFocusChanged) {
                            onFocusChanged.call(c, oldTarget, newTarget);
                        }
                    };
                }(control));
            };
            CPage.prototype._onFocusHandlers = function (control, focusInfo) {
                var a = this;
                if (a._optionMenuProvider && !a._contextMenu) {
                    var activeFocusInfo = {
                        control: control,
                        $item: focusInfo
                    };
                    var optionMenuInfo = a._optionMenuProvider(activeFocusInfo);
                    if (optionMenuInfo) {
                        optionMenuInfo.control = control;
                        a._activeOptionMenuInfo = optionMenuInfo;
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
                    }
                }
            };
            return CPage;
        })(__CONTROLS__.CLayeredGroupControl);
        mPrism.CPage = CPage;
        if (!window.prism) {
            window.prism = {};
        }
        prism.CPage = CPage;
    })(mPrism || (mPrism = {}));
    return mPrism;
});
