var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", 'prismux/prism.dlg'], function (require, exports, __PRISM_CDIALOG__) {
    var mPrism;
    (function (mPrism) {
        var entryDrawer = function (index, $item) {
            $item.addClass('entryItem');
        };
        var CQueryDlg = (function (_super) {
            __extends(CQueryDlg, _super);
            function CQueryDlg(layeredGroupControl) {
                _super.call(this, layeredGroupControl);
            }
            CQueryDlg.prototype.init = function (layeredGroupControl) {
                var a = this;
                _super.prototype.init.call(a, layeredGroupControl);
                a.addClass('queryDlg');
                a._inputChars = [];
                a._fromRight = false;
            };
            CQueryDlg.prototype._createEntry = function (aParam) {
            };
            CQueryDlg.prototype.setTextHtml = function (textHtml) {
                var a = this;
                a._textHtml = textHtml;
            };
            CQueryDlg.prototype.setFocusRight = function () {
                var a = this;
                if (a._entry) {
                    a._entry.setFocusedItem(a._numberOfDigit - 1);
                }
            };
            CQueryDlg.prototype.reset = function (focusIndex) {
                var a = this;
                if (a._entry) {
                    a._$self.find('.entryItem').text('').removeClass('entered');
                    a._entry.setFocusedItem(focusIndex ? focusIndex : 0);
                    a._inputChars = [];
                }
            };
            CQueryDlg.prototype.showInvalidInd = function (show) {
                var a = this;
                if (show && !a._$invalidInd) {
                    var $entry = a._$self.find('.entry');
                    var posEntry = $entry.position();
                    var $invalidInd = $('<div>', {
                        'class': 'invalidInd'
                    });
                    $invalidInd.css({
                        position: 'absolute',
                        top: posEntry.top + 'px',
                        left: posEntry.left + $entry.outerWidth() + 'px'
                    });
                    $entry.parent().append($invalidInd);
                    a._$invalidInd = $invalidInd;
                }
                else if (!show && a._$invalidInd) {
                    a._$invalidInd.remove();
                    delete a._$invalidInd;
                }
            };
            CQueryDlg.prototype.doOpen = function (param) {
                var a = this;
                var verticalLayout = new CVerticalLayoutControl(null);
                verticalLayout.setItemDrawers([
                    function (index, $item) {
                        $item.addClass('text');
                        $item.removeClass('-f');
                        $item.html(a._textHtml);
                        return false;
                    }
                ]);
                a._entry = a._createEntry(param);
                a.setChildControl([verticalLayout, a._entry], 1);
                a.setDrawParam({
                    $parent: a._parent.$layer(),
                    align: 'center',
                    childAlign: 'hcenter',
                    padding: 33,
                    margins: [33, 50, 30]
                });
            };
            return CQueryDlg;
        })(__PRISM_CDIALOG__.CDialog);
        mPrism.CQueryDlg = CQueryDlg;
        var CQueryPasswordDlg = (function (_super) {
            __extends(CQueryPasswordDlg, _super);
            function CQueryPasswordDlg(layeredGroupControl) {
                _super.call(this, layeredGroupControl);
            }
            CQueryPasswordDlg.prototype.init = function (layeredGroupControl) {
                var a = this;
                _super.prototype.init.call(a, layeredGroupControl);
                a._$self.addClass('queryPassword');
            };
            CQueryPasswordDlg.prototype._createEntry = function (param) {
                var a = this;
                var horizontalLayout = new CHorizontalLayoutControl(null);
                horizontalLayout.addClass('entry');
                horizontalLayout.setDrawParam({
                    childAlign: 'center',
                    padding: 15
                });
                horizontalLayout.setItemDrawers([entryDrawer, entryDrawer, entryDrawer, entryDrawer]);
                horizontalLayout.doKeyRight = function () {
                    var $item = horizontalLayout.getFocusedItem();
                    var index = parseInt($item.attr('data'), 10);
                    if (index < a._inputChars.length) {
                        horizontalLayout.doNavi('r');
                    }
                    return true;
                };
                horizontalLayout.doKeyNumber = function (number) {
                    var $item = horizontalLayout.getFocusedItem();
                    var index = parseInt($item.attr('data'), 10);
                    a._inputChars[index] = number;
                    $item.addClass('entered');
                    if (index < a._inputChars.length) {
                        horizontalLayout.doNavi('r');
                    }
                    if (index == 3) {
                        if (param.onPasswordEntered) {
                            param.onPasswordEntered(a._inputChars.join(''));
                        }
                    }
                    return true;
                };
                return horizontalLayout;
            };
            return CQueryPasswordDlg;
        })(CQueryDlg);
        mPrism.CQueryPasswordDlg = CQueryPasswordDlg;
        var fnGetHours = function (inputChars) {
            if (2 <= inputChars.length) {
                return parseInt(inputChars.slice(0, 2).join(''), 10);
            }
        };
        var fnGetMinutes = function (inputChars) {
            var a = this;
            if (inputChars.length == 4) {
                return parseInt(inputChars.slice(2, 4).join(''), 10);
            }
        };
        var fnValidate = function (index, number, inputChars) {
            if (index == 0 && 2 < number) {
                return false;
            }
            if (index == 2 && 5 < number) {
                return false;
            }
            var hours = fnGetHours(inputChars);
            if (hours !== undefined && 23 < hours) {
                return false;
            }
            var minutes = fnGetMinutes(inputChars);
            if (minutes !== undefined && 59 < minutes) {
                return false;
            }
            return true;
        };
        var CQueryTimeDlg = (function (_super) {
            __extends(CQueryTimeDlg, _super);
            function CQueryTimeDlg(layeredGroupControl) {
                _super.call(this, layeredGroupControl);
            }
            CQueryTimeDlg.prototype.init = function (layeredGroupControl) {
                var a = this;
                _super.prototype.init.call(a, layeredGroupControl);
                a._$self.addClass('queryTime');
                a._fromRight = true;
            };
            CQueryTimeDlg.prototype._createEntry = function (param) {
                var a = this;
                var horizontalLayout = new CHorizontalLayoutControl(null);
                horizontalLayout.addClass('entry');
                horizontalLayout.setDrawParam({
                    childAlign: 'center',
                    padding: 15
                });
                var timeDrawer = function (index, $item) {
                    entryDrawer(index, $item);
                    index = index < 2 ? index : index - 1;
                    $item.addClass('entered');
                    $item.text(a._inputChars[index]);
                };
                if (a._inputChars.length) {
                    horizontalLayout.setItemDrawers([timeDrawer, timeDrawer, function (index, $item) {
                        $item.removeClass('-f').addClass('timeSeparator').text(':');
                    }, timeDrawer, timeDrawer]);
                }
                else {
                    horizontalLayout.setItemDrawers([entryDrawer, entryDrawer, function (index, $item) {
                        $item.removeClass('-f').addClass('timeSeparator').text(':');
                    }, entryDrawer, entryDrawer]);
                }
                horizontalLayout.doKeyRight = function () {
                    var $item = horizontalLayout.getFocusedItem();
                    var index = parseInt($item.attr('data'), 10);
                    if (index < a._inputChars.length) {
                        horizontalLayout.doNavi('r');
                    }
                    return true;
                };
                horizontalLayout.doKeyNumber = function (number) {
                    var $item = horizontalLayout.getFocusedItem();
                    var index = parseInt($item.attr('data'), 10);
                    index = index < 2 ? index : index - 1;
                    var inputChars = a._inputChars.slice(0);
                    inputChars[index] = number;
                    if (fnValidate(index, number, inputChars)) {
                        a._inputChars = inputChars;
                        $item.addClass('entered');
                        $item.text('' + number);
                        if (index < a._inputChars.length) {
                            horizontalLayout.doNavi('r');
                        }
                    }
                    return true;
                };
                return horizontalLayout;
            };
            CQueryTimeDlg.prototype.getHours = function () {
                var a = this;
                return fnGetHours(a._inputChars);
            };
            CQueryTimeDlg.prototype.getMinutes = function () {
                var a = this;
                return fnGetMinutes(a._inputChars);
            };
            CQueryTimeDlg.prototype.setTime = function (hours, minutes) {
                var a = this;
                var strHours = '' + hours;
                strHours = strHours.length == 1 ? '0' + strHours : strHours;
                var strMinutes = '' + minutes;
                strMinutes = strMinutes.length == 1 ? '0' + strMinutes : strMinutes;
                var inputChars = (strHours + strMinutes).split('');
                if (fnValidate(0, 0, inputChars)) {
                    a._inputChars = inputChars;
                    return true;
                }
                return false;
            };
            CQueryTimeDlg.prototype.doKeyOk = function () {
                var a = this;
                if (a._inputChars.length == 4 && fnValidate(0, 0, a._inputChars)) {
                    a.onTimeEntered(a.getHours(), a.getMinutes());
                }
                return true;
            };
            return CQueryTimeDlg;
        })(CQueryDlg);
        mPrism.CQueryTimeDlg = CQueryTimeDlg;
        var CQueryNumberDlg = (function (_super) {
            __extends(CQueryNumberDlg, _super);
            function CQueryNumberDlg(layeredGroupControl, config) {
                _super.call(this, layeredGroupControl);
                this._$self.addClass('queryDuration');
                this._numberOfDigit = (config) ? ((config.numberOfDigit) ? config.numberOfDigit : 4) : 4;
                this._fromRight = (config) ? ((config.fromRight === false) ? config.fromRight : true) : true;
            }
            CQueryNumberDlg.prototype.setTrailText = function (trailText) {
                var a = this;
                a._trailText = trailText;
            };
            CQueryNumberDlg.prototype._createEntry = function (param) {
                var a = this;
                var i = 0;
                var horizontalLayout = new CHorizontalLayoutControl(null);
                horizontalLayout.addClass('entry');
                horizontalLayout.setDrawParam({
                    childAlign: 'center',
                    padding: 15
                });
                var $items = [];
                var durationDrawer = function (index, $item) {
                    if (a._inputChars.length) {
                        entryDrawer(index, $item);
                        index = index < 2 ? index : index - 1;
                        $item.addClass('entered');
                        $item.text(a._inputChars[index]);
                    }
                    else {
                        entryDrawer(index, $item);
                        $items.push($item);
                    }
                };
                var drawers = [];
                for (i = 0; i < a._numberOfDigit; i += 1) {
                    drawers.push(durationDrawer);
                }
                if (a._trailText) {
                    drawers.push(function (index, $item) {
                        $item.removeClass('-f').addClass('durationUnitTxt').text(a._trailText);
                    });
                }
                horizontalLayout.setItemDrawers(drawers);
                horizontalLayout.doKeyLeft = function () {
                    return true;
                };
                horizontalLayout.doKeyRight = function () {
                    return true;
                };
                horizontalLayout.doKeyNumber = function (number) {
                    var $item = horizontalLayout.getFocusedItem();
                    var index = parseInt($item.attr('data'), 10);
                    var len = a._inputChars.length;
                    if (a._fromRight) {
                        if (!a._replace && index == a._numberOfDigit - 1 && len < a._numberOfDigit) {
                            inputNumber(number, len);
                        }
                        else {
                            a.reset(a._numberOfDigit - 1);
                            if ((len === 0) || (len === 4)) {
                                $item = horizontalLayout.getFocusedItem();
                                index = parseInt($item.attr('data'), 10);
                                len = a._inputChars.length;
                                inputNumber(number, len);
                            }
                        }
                    }
                    else {
                        if (!a._replace && index <= a._numberOfDigit - 1 && len < a._numberOfDigit) {
                            a._inputChars.push(number);
                            $item.addClass('entered');
                            $item.text('' + number);
                            horizontalLayout.setFocusedItem((index < a._numberOfDigit - 1) ? index + 1 : index);
                        }
                        else {
                            a.reset(0);
                        }
                    }
                    function inputNumber(number, len) {
                        var prevIndex = index - 1;
                        var i, start = a._numberOfDigit - 1 - len;
                        for (i = 0; i < len; i++) {
                            var prevNumber = a._inputChars[i];
                            var $prevItem = $items[start + i];
                            $prevItem.addClass('entered');
                            $prevItem.text('' + prevNumber);
                        }
                        a._inputChars.push(number);
                        $item.text('' + number);
                        $item.addClass('entered');
                    }
                    return true;
                };
                return horizontalLayout;
            };
            CQueryNumberDlg.prototype.getHours = function () {
                var a = this;
                return fnGetHours(a._inputChars);
            };
            CQueryNumberDlg.prototype.getMinutes = function () {
                var a = this;
                return fnGetMinutes(a._inputChars);
            };
            CQueryNumberDlg.prototype.setTime = function (hours, minutes) {
                var a = this;
                var strHours = '' + hours;
                strHours = strHours.length == 1 ? '0' + strHours : strHours;
                var strMinutes = '' + minutes;
                strMinutes = strMinutes.length == 1 ? '0' + strMinutes : strMinutes;
                var inputChars = (strHours + strMinutes).split('');
                if (fnValidate(0, 0, inputChars)) {
                    a._inputChars = inputChars;
                    return true;
                }
                return false;
            };
            CQueryNumberDlg.prototype.doKeyOk = function () {
                var a = this;
                if (a._inputChars.length) {
                    a.onNumberEntered(parseInt(a._inputChars.join(''), 10));
                }
                return true;
            };
            return CQueryNumberDlg;
        })(CQueryDlg);
        mPrism.CQueryNumberDlg = CQueryNumberDlg;
        /*
         prism.createQueryPasswordDlg = function(layeredGroupControl) {
         var passwordDlg = new CQueryPasswordDlg(layeredGroupControl);
         return passwordDlg;
         };
         */
        if (!window.prism) {
            window.prism = {};
        }
        prism.createQueryTimeDlg = function (layeredGroupControl) {
            var queryDateDlg = new CQueryTimeDlg(layeredGroupControl);
            return queryDateDlg;
        };
        prism.createQueryNumberDlg = function (layeredGroupControl, config) {
            var queryNumberDlg = new CQueryNumberDlg(layeredGroupControl, config);
            return queryNumberDlg;
        };
    })(mPrism || (mPrism = {}));
    return mPrism;
});
