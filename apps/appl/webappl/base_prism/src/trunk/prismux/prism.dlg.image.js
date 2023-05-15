/// <reference path="../def/prism.d.ts" />
// file name : prismux/prism.dlg.image.js
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
        var CImageDlg = (function (_super) {
            __extends(CImageDlg, _super);
            function CImageDlg(layeredGroupControl) {
                _super.call(this, layeredGroupControl);
            }
            CImageDlg.prototype.init = function (layeredGroupControl) {
                var a = this;
                _super.prototype.init.call(a, layeredGroupControl);
                a.addClass('imageDlg');
            };
            CImageDlg.prototype.setTextHtml = function (textHtml) {
                var a = this;
                a._textHtml = textHtml;
            };
            CImageDlg.prototype.doOpen = function (param) {
                var a = this;
                var upperHorizontalDrawers = [
                    function (index, $item) {
                        var $title = $('<div>', {
                            'class': 'TVAppsTitle'
                        });
                        $item.addClass('TVAppsTitle');
                        $item.append($title);
                        $item.removeClass('-f');
                        $item.html(tr('LOC_TVAPPS_NETWORK_ERROR_ID'));
                    },
                    function (index, $item) {
                        var $btn = $('<div>', {
                            'class': 'closeBtn'
                        });
                        $item.append($btn);
                    }
                ];
                var imageHorizontalDrawers = [
                    function (index, $item) {
                        var $edgeImage = $('<div>', {
                            'class': 'edgeImage'
                        });
                        $item.addClass('edgeImage');
                        $item.removeClass('-f');
                    },
                    function (index, $item) {
                        var $msgImage = $('<div>', {
                            'class': 'msgImage'
                        });
                        $item.addClass('msgImage');
                        $item.removeClass('-f');
                    },
                    function (index, $item) {
                        var $errMsg = $('<div>', {
                            'class': 'errMsg'
                        });
                        $item.addClass('errMsg');
                        $item.append($errMsg);
                        $item.removeClass('-f');
                        $item.html(tr('LOC_TVAPPS_NETWORK_ERROR_TEXT_ID'));
                    }
                ];
                var imageHorizontalLayout = new CHorizontalLayoutControl(null);
                imageHorizontalDrawers.push(function (index, $item) {
                    $item.addClass('text');
                    $item.html(a._textHtml);
                });
                imageHorizontalLayout.setItemDrawers(imageHorizontalDrawers);
                imageHorizontalLayout.setDrawParam({
                    padding: 0
                });
                var upperHorizontalLayout = new CHorizontalLayoutControl(null);
                upperHorizontalDrawers.push(function (index, $item) {
                    $item.addClass('text');
                    $item.html(a._textHtml);
                });
                upperHorizontalLayout.setItemDrawers(upperHorizontalDrawers);
                upperHorizontalLayout.onItemSelected = function (index, $item) {
                    /*imageDlg.open({
                     dimm : true,
                     });*/
                };
                upperHorizontalLayout.setDrawParam({
                    padding: 0
                });
                a.setChildControl([upperHorizontalLayout, imageHorizontalLayout]);
                //a.setDrawParam();
            };
            CImageDlg.prototype.doKeyBack = function () {
                var a = this;
                a.close();
                return true;
            };
            CImageDlg.prototype.doKeyOk = function () {
                var a = this;
                a.close();
                return true;
            };
            return CImageDlg;
        })(__PRISM_CDIALOG__.CDialog);
        mPrism.CImageDlg = CImageDlg;
        if (!window.prism) {
            window.prism = {};
        }
        prism.CImageDlg = CImageDlg;
        prism.createImageDlg = function (layeredGroupControl, htmlText, callback) {
            if (layeredGroupControl) {
                var imageDlg = new CImageDlg(layeredGroupControl);
                imageDlg.addClass('imageDlg');
                if (htmlText) {
                    imageDlg.setTextHtml(htmlText);
                }
                return imageDlg;
            }
            else {
                var globalDlg = prism.createGlobalDialog('createImageDlg', htmlText, callback);
                return globalDlg;
            }
        };
    })(mPrism || (mPrism = {}));
    return mPrism;
});
