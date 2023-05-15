var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", 'pages/util/controls', 'prismux/prism.dlg'], function (require, exports, __CONTROLS__, __PRISM_CDIALOG__) {
    var mPrism;
    (function (mPrism) {
        var mOpenCount = 0;
        var CQueryPasswordDlg = (function (_super) {
            __extends(CQueryPasswordDlg, _super);
            function CQueryPasswordDlg(layeredGroupControl) {
                _super.call(this);
                if (arguments.length) {
                    this.init(layeredGroupControl);
                }
            }
            CQueryPasswordDlg.isOpened = function () {
                return mOpenCount ? true : false;
            };
            CQueryPasswordDlg.prototype.init = function (layeredGroupControl) {
                var a = this;
                _super.prototype.init.call(a, layeredGroupControl);
                a.addClass('passwordDlg');
                mOpenCount += 1;
            };
            CQueryPasswordDlg.prototype.destroy = function () {
                var a = this;
                mOpenCount -= 1;
                _super.prototype.destroy.call(a);
            };
            CQueryPasswordDlg.prototype.setTextHtml = function (textHtml) {
                var a = this;
                a._textHtml = textHtml;
                a._passKey = [];
            };
            CQueryPasswordDlg.prototype.reset = function (show) {
                var a = this;
                if (a._passwordEntry) {
                    a._$self.find('.passwordEntryItem').removeClass('entered');
                    a._passwordEntry.setFocusedItem(0);
                }
            };
            CQueryPasswordDlg.prototype.showInvalidInd = function (show) {
                var a = this;
                if (show && !a._$invalidInd) {
                    var $passwordEntry = a._$self.find('.passwordEntry');
                    var posPassEntry = $passwordEntry.position();
                    var $invalidInd = $('<div>', { 'class': 'invalidInd' });
                    $invalidInd.css({
                        position: 'absolute',
                        top: posPassEntry.top + 'px',
                        left: posPassEntry.left + $passwordEntry.outerWidth() + 'px'
                    });
                    $passwordEntry.parent().append($invalidInd);
                    a._$invalidInd = $invalidInd;
                }
                else if (!show && a._$invalidInd) {
                    a._$invalidInd.remove();
                    delete a._$invalidInd;
                }
            };
            CQueryPasswordDlg.prototype.doOpen = function (param) {
                var a = this;
                var verticalLayout = new __CONTROLS__.CVerticalLayoutControl(null);
                verticalLayout.setItemDrawers([
                    function (index, $item) {
                        $item.addClass('text');
                        $item.removeClass('-f');
                        $item.html(a._textHtml);
                        return false;
                    }
                ]);
                var entryDrawer = function (index, $item) {
                    $item.addClass('passwordEntryItem');
                };
                var horizontalLayout = new __CONTROLS__.CHorizontalLayoutControl(null);
                horizontalLayout.addClass('passwordEntry');
                horizontalLayout.setDrawParam({
                    childAlign: 'center',
                    padding: 15
                });
                horizontalLayout.setItemDrawers([entryDrawer, entryDrawer, entryDrawer, entryDrawer]);
                horizontalLayout['doKeyLeft'] = function () {
                    var $item = horizontalLayout.getFocusedItem();
                    $item.removeClass('entered');
                    horizontalLayout.doNavi('l');
                    return true;
                };
                horizontalLayout['doKeyRight'] = function () {
                    var $item = horizontalLayout.getFocusedItem();
                    var index = parseInt($item.attr('data'), 10);
                    if (index < a._passKey.length) {
                        horizontalLayout.doNavi('r');
                    }
                    return true;
                };
                horizontalLayout['doKeyNumber'] = function (number) {
                    var $item = horizontalLayout.getFocusedItem();
                    var index = parseInt($item.attr('data'), 10);
                    a._passKey[index] = number;
                    $item.addClass('entered');
                    if (index < a._passKey.length) {
                        horizontalLayout.doNavi('r');
                    }
                    if (index === 3) {
                        if (param.onPasswordEntered) {
                            param.onPasswordEntered(a._passKey.join(''));
                        }
                    }
                    return true;
                };
                a._passwordEntry = horizontalLayout;
                a.setChildControl([verticalLayout, horizontalLayout], 1);
                a.setDrawParam({
                    $parent: a._parent.$layer(),
                    align: 'center',
                    childAlign: 'hcenter',
                    padding: 33,
                    margins: [33, 50, 30]
                });
            };
            return CQueryPasswordDlg;
        })(__PRISM_CDIALOG__.CDialog);
        mPrism.CQueryPasswordDlg = CQueryPasswordDlg;
        var CPasswordConfirmationDlg = (function (_super) {
            __extends(CPasswordConfirmationDlg, _super);
            function CPasswordConfirmationDlg(layeredGroupControl) {
                _super.call(this);
                if (arguments.length) {
                    this.init(layeredGroupControl);
                }
            }
            CPasswordConfirmationDlg.prototype.setTextHtml = function (titleHtml, newHtml, verifyHtml) {
                var a = this;
                a._titleHtml = titleHtml;
                a._newHtml = newHtml;
                a._verifyHtml = verifyHtml;
                a._passKey1 = [];
                a._passKey2 = [];
            };
            CPasswordConfirmationDlg.prototype.reset = function (show) {
                var a = this;
                if (a._horizontalLayout1) {
                    a._$self.find('.passwordEntryItem').removeClass('entered');
                    a._horizontalLayout1.setFocusedItem(0);
                    a._horizontalLayout2.setFocusedItem(0);
                    a.doNavi('u');
                }
            };
            CPasswordConfirmationDlg.prototype.doOpen = function (param) {
                var a = this;
                var verticalLayout = new CVerticalLayoutControl(null);
                a.addClass('confirm');
                verticalLayout.setItemDrawers([
                    function (index, $item) {
                        $item.addClass('text');
                        $item.removeClass('-f');
                        $item.html(a._titleHtml);
                        return false;
                    }
                ]);
                var textDrawer1 = function (index, $item) {
                    $item.removeClass('-f');
                    $item.append($('<div>', {
                        'class': 'label',
                        'html': a._newHtml
                    }));
                    $item.addClass('title');
                };
                var textDrawer2 = function (index, $item) {
                    $item.removeClass('-f');
                    $item.append($('<div>', {
                        'class': 'label',
                        'html': a._verifyHtml
                    }));
                    $item.addClass('title');
                };
                var entryDrawer = function (index, $item) {
                    $item.addClass('passwordEntryItem');
                };
                var horizontalLayout1 = new CHorizontalLayoutControl(null);
                horizontalLayout1.addClass('passwordEntry');
                horizontalLayout1.setDrawParam({
                    childAlign: 'center',
                    padding: 15
                });
                horizontalLayout1.setItemDrawers([entryDrawer, entryDrawer, entryDrawer, entryDrawer]);
                horizontalLayout1.doKeyLeft = function () {
                    var $item = horizontalLayout1.getFocusedItem();
                    $item.removeClass('entered');
                    horizontalLayout1.doNavi('l');
                    return true;
                };
                horizontalLayout1.doKeyRight = function () {
                    var $item = horizontalLayout1.getFocusedItem();
                    var index = parseInt($item.attr('data'), 10);
                    if (index < a._passKey1.length) {
                        horizontalLayout2.doNavi('r');
                    }
                    return true;
                };
                horizontalLayout1.doKeyNumber = function (number) {
                    var $item = horizontalLayout1.getFocusedItem();
                    var index = parseInt($item.attr('data'), 10);
                    a._passKey1[index] = number;
                    $item.addClass('entered');
                    if (index < a._passKey1.length) {
                        horizontalLayout1.doNavi('r');
                    }
                    if (index === 3) {
                        a.doNavi('d');
                    }
                    return true;
                };
                var horizontalLayout2 = new CHorizontalLayoutControl(null);
                horizontalLayout2.addClass('passwordEntry');
                horizontalLayout2.setDrawParam({
                    childAlign: 'center',
                    padding: 15
                });
                horizontalLayout2.setItemDrawers([entryDrawer, entryDrawer, entryDrawer, entryDrawer]);
                horizontalLayout2.doKeyLeft = function () {
                    var $item = horizontalLayout2.getFocusedItem();
                    $item.removeClass('entered');
                    if (!horizontalLayout2.doNavi('l')) {
                        a.doNavi('u');
                    }
                    return true;
                };
                horizontalLayout2.doKeyRight = function () {
                    var $item = horizontalLayout2.getFocusedItem();
                    var index = parseInt($item.attr('data'), 10);
                    if (index < a._passKey2.length) {
                        horizontalLayout2.doNavi('r');
                    }
                    return true;
                };
                horizontalLayout2.doKeyNumber = function (number) {
                    var $item = horizontalLayout2.getFocusedItem();
                    var index = parseInt($item.attr('data'), 10);
                    a._passKey2[index] = number;
                    $item.addClass('entered');
                    if (index < a._passKey2.length) {
                        horizontalLayout2.doNavi('r');
                    }
                    if (index === 3) {
                        if (param.onPasswordEntered) {
                            param.onPasswordEntered(a._passKey1.join(''), a._passKey2.join(''));
                        }
                    }
                    return true;
                };
                a._horizontalLayout1 = horizontalLayout1;
                a._horizontalLayout2 = horizontalLayout2;
                var horizontalLayout3 = new CHorizontalLayoutControl(null);
                horizontalLayout3.setItemDrawers([textDrawer1]);
                var horizontalLayout4 = new CHorizontalLayoutControl(null);
                horizontalLayout4.setItemDrawers([textDrawer2]);
                var horizontalViewGroupControl1 = new CHorizontalGroupControl(null);
                horizontalViewGroupControl1.setDrawParam({
                    width: 'inherit'
                });
                horizontalViewGroupControl1.setChildControl([horizontalLayout3, horizontalLayout1], 1);
                var horizontalViewGroupControl2 = new CHorizontalGroupControl(null);
                horizontalViewGroupControl2.setDrawParam({
                    width: 'inherit'
                });
                horizontalViewGroupControl2.setChildControl([horizontalLayout4, horizontalLayout2], 1);
                a.setChildControl([verticalLayout, horizontalViewGroupControl1, horizontalViewGroupControl2], 1);
                a.setDrawParam({
                    $parent: a._parent.$layer(),
                    align: 'center',
                    childAlign: 'hcenter',
                    padding: 33,
                    margins: [33, 50, 30]
                });
            };
            return CPasswordConfirmationDlg;
        })(CQueryPasswordDlg);
        mPrism.CPasswordConfirmationDlg = CPasswordConfirmationDlg;
        if (!window.prism) {
            window.prism = {};
        }
        prism.CQueryPasswordDlg = CQueryPasswordDlg;
        prism.CPasswordConfirmationDlg = CPasswordConfirmationDlg;
        prism.createQueryPasswordDlg = function (layeredGroupControl) {
            var passwordDlg = new CQueryPasswordDlg(layeredGroupControl);
            return passwordDlg;
        };
        prism.createPasswordConfirmationDlg = function (layeredGroupControl) {
            var passwordDlg = new CPasswordConfirmationDlg(layeredGroupControl);
            return passwordDlg;
        };
    })(mPrism || (mPrism = {}));
    return mPrism;
});