/// <reference path="../def/prism.d.ts" />
// file name : prismux/prism.dlg.help.js
/*jslint sloppy:true, nomen:true, vars:true */
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", 'prismux/prism.dlg'], function (require, exports, __PRISM_CDIALOG__) {
    var mPrism;
    (function (mPrism) {
        var CHelpDlg = (function (_super) {
            __extends(CHelpDlg, _super);
            function CHelpDlg(layeredGroupControl) {
                _super.call(this, layeredGroupControl);
            }
            CHelpDlg.prototype.init = function (layeredGroupControl) {
                var a = this;
                _super.prototype.init.call(a, layeredGroupControl);
                a.addClass('helpDlg');
            };
            CHelpDlg.prototype.setTextHtml = function (textHtml) {
                var a = this;
                a._textHtml = textHtml;
            };
            CHelpDlg.prototype.setData = function (data) {
                var a = this;
                a._data = data;
            };
            CHelpDlg.prototype.doOpen = function (param) {
                var a = this;
                var i;
                var initialFocus = 0;
                var childControl = [];
                var data = a._data;
                var helpSlideControl = new CCarouselControl(null);
                helpSlideControl.setData(data);
                helpSlideControl.setDrawParam({
                    viewCount: 1,
                    anchorIndex: 0,
                    itemWidth: 70,
                    itemHeight: 531,
                    anchorWidth: 70,
                    transparentAnchor: false,
                    animation: true,
                    align: 'horizontal',
                    animationInterval: 0.1,
                    maxKeyQueueCount: 0,
                    applyOnce: {
                        drawEffect: 'spreadOut'
                    }
                });
                helpSlideControl.setDataDrawer(function (param) {
                    var $item = param.$item;
                    var item = param.item;
                    var scene = item.scene;
                    var caption = item.caption;
                    var i;
                    var src = scene;
                    var $scene = $('<img>', {
                        'class': 'scene',
                        'src': src
                    });
                    $item.append($scene.addClass('scene').removeClass('-f'));
                    var $caption = $('<div>', {
                        'class': 'caption'
                    });
                    $item.append($caption.html(tr(caption)).addClass('caption').removeClass('-f'));
                });
                var $edgeLine = $('<div>', {
                    'class': 'edgeLine'
                });
                $edgeLine.addClass('edgeLine');
                $edgeLine.removeClass('-f');
                var $img = $('<div>');
                $img.addClass('border');
                var $arrow = $('<div>');
                var $arrowR = $('<div>', {
                    'class': 'rightArrow'
                });
                var $arrowL = $('<div>', {
                    'class': 'leftArrow'
                });
                $arrow.addClass('arrow');
                var $indicateSpace = $('<div>', {
                    'class': 'indicateSpace'
                });
                var $indicator = $('<div>');
                $indicator.addClass('indicator');
                a._$self.append($img).append($edgeLine).append($arrowR).append($arrowL).append($indicateSpace);
                //$indicateBtn.addClass('btnOn');
                var $btn = $('<div>', {
                    'class': 'closeBtn'
                });
                $btn.addClass('show');
                a._$self.append($btn);
                var $number = $('<div>', {
                    'class': 'number'
                });
                var $title = $('<div>', {
                    'class': 'title'
                });
                a._$self.append($number.addClass('number'));
                a._$self.append($title.addClass('title'));
                for (i = 0; i < data.length; i += 1) {
                    $indicateSpace.append($('<div>', {
                        'class': 'indicator'
                    }));
                }
                helpSlideControl.onFocusStartToChange = function (param) {
                    //helpSlideControl._$self.find('.-carousel-dist0').fadeOut(300);
                    //helpSlideControl._$self.find('.-carousel-dist1').fadeIn(300);
                };
                helpSlideControl.onFocusChanged = function (param) {
                    var data = a._data;
                    var item = param.item;
                    var no = item.no;
                    var title = item.title;
                    var currentIndex = helpSlideControl.getCurrentIndex();
                    if (data.length !== 1) {
                        if (currentIndex === 0) {
                            $arrowL.removeClass('show');
                            $arrowR.addClass('show');
                            $btn.removeClass('show');
                        }
                        else if (currentIndex === data.length - 1) {
                            $arrowL.addClass('show');
                            $arrowR.removeClass('show');
                            $btn.addClass('show');
                        }
                        else if (currentIndex !== 0) {
                            $arrowL.addClass('show');
                            $arrowR.addClass('show');
                            $btn.removeClass('show');
                        }
                        $indicator = $('.indicateSpace .indicator');
                        $indicator.removeClass('on');
                        $($indicator[currentIndex]).addClass('on');
                    }
                    $number.html(no).removeClass('-f');
                    $title.html(tr(title)).removeClass('-f');
                };
                helpSlideControl.onItemSelected = function (param) {
                    //alert(param.item + ' Selected');
                };
                helpSlideControl.doKeyLeft = function () {
                    var currentIndex = helpSlideControl.getCurrentIndex();
                    if (currentIndex !== 0) {
                        CCarouselControl.prototype.doKeyLeft.call(helpSlideControl);
                    }
                    return true;
                };
                helpSlideControl.doKeyRight = function () {
                    var currentIndex = helpSlideControl.getCurrentIndex();
                    if (currentIndex !== a._data.length - 1) {
                        CCarouselControl.prototype.doKeyRight.call(helpSlideControl);
                    }
                    return true;
                };
                helpSlideControl.doKeyBack = function () {
                    var currentIndex = helpSlideControl.getCurrentIndex();
                    if (currentIndex !== 0) {
                        CCarouselControl.prototype.doKeyLeft.call(helpSlideControl);
                        return true;
                    }
                    return false;
                };
                a._helpSlideControl = helpSlideControl;
                childControl.push(helpSlideControl);
                a.setChildControl(childControl, initialFocus);
            };
            CHelpDlg.prototype.doKeyOk = function () {
                var a = this;
                var currentIndex = a._helpSlideControl.getCurrentIndex();
                if (currentIndex === 4) {
                    a.close();
                }
                return true;
            };
            return CHelpDlg;
        })(__PRISM_CDIALOG__.CDialog);
        mPrism.CHelpDlg = CHelpDlg;
        if (!window.prism) {
            window.prism = {};
        }
        prism.createHelpDlg = function (layeredGroupControl, htmlText, callback) {
            if (layeredGroupControl) {
                var helpDlg = new CHelpDlg(layeredGroupControl);
                if (htmlText) {
                    helpDlg.setTextHtml(htmlText);
                }
                return helpDlg;
            }
            else {
                var globalDlg = prism.createGlobalDialog('createHelpDlg', htmlText, callback);
                return globalDlg;
            }
        };
    })(mPrism || (mPrism = {}));
    return mPrism;
});