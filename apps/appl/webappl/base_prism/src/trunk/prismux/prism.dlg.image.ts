/// <reference path="../def/prism.d.ts" />
// file name : prismux/prism.dlg.image.js
/*jslint sloppy:true, nomen:true, vars:true */

import __CONTROLS__ = require('pages/util/controls');
import __PRISM_CDIALOG__ = require('prismux/prism.dlg');

module mPrism {
    export class CImageDlg extends __PRISM_CDIALOG__.CDialog {
        _textHtml;
        constructor(layeredGroupControl) {
            super(layeredGroupControl);
        }
        init(layeredGroupControl) {
            var a = this;
            super.init.call(a, layeredGroupControl);
            a.addClass('imageDlg');
        }
        setTextHtml(textHtml) {
            var a = this;
            a._textHtml = textHtml;
        }
        doOpen(param?) {
            var a = this;
            var upperHorizontalDrawers = [
                function(index, $item) {
                    var $title = $('<div>', {
                        'class' : 'TVAppsTitle'
                    });
                    $item.addClass('TVAppsTitle');
                    $item.append($title);
                    $item.removeClass('-f');
                    $item.html(tr('LOC_TVAPPS_NETWORK_ERROR_ID'));
                },
                function(index, $item) {
                    var $btn = $('<div>', {
                        'class' : 'closeBtn'
                    });
                    $item.append($btn);
                }];
            var imageHorizontalDrawers = [
                function(index, $item) {
                    var $edgeImage = $('<div>', {
                        'class' : 'edgeImage'
                    });
                    $item.addClass('edgeImage');
                    $item.removeClass('-f');
                },
                function(index, $item) {
                    var $msgImage = $('<div>', {
                        'class' : 'msgImage'
                    });
                    $item.addClass('msgImage');
                    $item.removeClass('-f');
                },
                function(index, $item) {
                    var $errMsg = $('<div>', {
                        'class' : 'errMsg'
                    });
                    $item.addClass('errMsg');
                    $item.append($errMsg);
                    $item.removeClass('-f');
                    $item.html(tr('LOC_TVAPPS_NETWORK_ERROR_TEXT_ID'));
                }];

            var imageHorizontalLayout = new CHorizontalLayoutControl(null);
            imageHorizontalDrawers.push(function(index, $item) {
                $item.addClass('text');
                $item.html(a._textHtml);
            });
            imageHorizontalLayout.setItemDrawers(imageHorizontalDrawers);
            imageHorizontalLayout.setDrawParam({
                padding : 0
            });

            var upperHorizontalLayout = new CHorizontalLayoutControl(null);
            upperHorizontalDrawers.push(function(index, $item) {
                $item.addClass('text');
                $item.html(a._textHtml);
            });
            upperHorizontalLayout.setItemDrawers(upperHorizontalDrawers);
            upperHorizontalLayout.onItemSelected = function(index, $item) {
                /*imageDlg.open({
                 dimm : true,
                 });*/
            };
            upperHorizontalLayout.setDrawParam({
                padding : 0
            });
            a.setChildControl([upperHorizontalLayout, imageHorizontalLayout]);
            //a.setDrawParam();
        }
        doKeyBack() {
            var a = this;
            a.close();
            return true;
        }
        doKeyOk() {
            var a = this;
            a.close();
            return true;
        }
    }
    if (!(<any>window).prism) {
        (<any>window).prism = {}
    }
    prism.CImageDlg = CImageDlg;
    prism.createImageDlg = function(layeredGroupControl, htmlText?, callback?) {
        if (layeredGroupControl) {
            var imageDlg = new CImageDlg(layeredGroupControl);
            imageDlg.addClass('imageDlg');
            if (htmlText) {
                imageDlg.setTextHtml(htmlText);
            }
            return imageDlg;
        } else {
            var globalDlg = prism.createGlobalDialog('createImageDlg', htmlText, callback);
            return globalDlg;
        }
    };
}
export = mPrism;
