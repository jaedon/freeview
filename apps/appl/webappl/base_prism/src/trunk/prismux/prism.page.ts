/// <reference path="../def/prism.d.ts" />
// file name : prismux/prism.page.js
/*jslint sloppy:true, nomen:true, vars:true */
import __CONTROLS__ = require('pages/util/controls');
import __PRISM_CDIALOG__ = require('prismux/prism.dlg');

module mPrism {
    export enum EViewType {
        ELIST = 0,
        EGRID,
    }
    export class CPage extends __CONTROLS__.CLayeredGroupControl {
        private _$date: JQuery;
        private _$time: JQuery;
        private _$title: JQuery;
        private _$titleArea: JQuery;
        private _optionMenuProvider: (aParam) => any;
        private _activeOptionMenuInfo: any;
        private _contextMenu: any;
        _optionMenuOnGoing: string;
        $optionMenuInd: () => JQuery;
        $contentArea: () => JQuery;
        $menuArea: () => JQuery;
        $bottomBg: () => JQuery;

        constructor($el) {
            super($el);
        }
        init($el) {
            var a = this;
            super.init.call(a, $el);
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
        }
        destroy() {
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
        }
        onContextMenuClose(aParam?): void {

        }
        setTitleText(html) {
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
            } else {
                a._$title.html(html);
            }
        }
        setTitleTextWithStorageStatus(html, $storageStatusArea) {
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
            } else {
                a._$title.html(html);
            }
        }
        changeTitleText(html) {
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
        }
        showDateTime() {
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
        }
        setDateText(text) {
            var a = this;
            a._$date.text(text);
        }
        setTimeText(text) {
            var a = this;
            a._$time.text(text);
        }
        setOptionMenuProvider(optionMenuProvider) {
            var a = this;
            a._optionMenuProvider = optionMenuProvider;
        }
        doKeyOption() {
            var a = this;
            if (a._optionMenuOnGoing === 'schduled.delete') {
                // TODO :  ExceptionHandling
                // Recordings Schedule ����  Delete �߿� Option�� ���� ���ϸ� Delete �Ϸ� �Ǹ鼭 Ŀ���� �̵��ϴµ� �׷���� �������� ���δ�.
                // �׷��� Delete �������϶��� OPtion ���� ���� �� �F��.
                return    true;
            }
            if (a._contextMenu) {
                a._contextMenu.close();
                return true;
            } else if (a._activeOptionMenuInfo && !(a._activeOptionMenuInfo.control && a._activeOptionMenuInfo.control.isTransitioning())) {
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
        }
        draw(param?) {
            var a = this;
            super.draw.call(a, param);
            var $contentArea = a.$contentArea();
            setTimeout(function () {
                $contentArea.addClass('show');
            }, 800);
        }
        reloadContextMenuItems(control?) {
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
                } else {
                    if (a._activeOptionMenuInfo) {
                        delete a._activeOptionMenuInfo;
                        a._activeOptionMenuInfo = undefined;
                    }
                    if (a.$optionMenuInd && a.$optionMenuInd()) {
                        a.$optionMenuInd().remove();
                    }
                }
            } else {
                var activeFocusInfo = a.getActiveFocusInfo();
                if (a._optionMenuProvider) {
                    optionMenuInfo = a._optionMenuProvider(activeFocusInfo);
                }
                if (optionMenuInfo) {
                    a._activeOptionMenuInfo = optionMenuInfo;
                    if (a._contextMenu) {
                        a._contextMenu.setContextMenuInfo(a._activeOptionMenuInfo);
                    }
                } else {
                    if (a._activeOptionMenuInfo) {
                        delete a._activeOptionMenuInfo;
                        a._activeOptionMenuInfo = undefined;
                    }
                    if (a.$optionMenuInd && a.$optionMenuInd()) {
                        a.$optionMenuInd().remove();
                    }
                }
            }
        }
        addOptionMenuTarget(control) {
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
        }
        _onFocusHandlers(control, focusInfo?) {
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
                } else {
                    if (a._activeOptionMenuInfo) {
                        delete a._activeOptionMenuInfo;
                        a._activeOptionMenuInfo = undefined;
                    }
                    if (a.$optionMenuInd && a.$optionMenuInd()) {
                        a.$optionMenuInd().remove();
                    }
                }
            }
        }
    }

    if (!(<any>window).prism) {
        (<any>window).prism = {}
    }
    prism.CPage = CPage;
}
export = mPrism;
