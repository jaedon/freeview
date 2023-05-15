/// <reference path="../def/prism.d.ts" />
// file name : prismux/prism.dlg.sync.js
/*jslint sloppy:true, nomen:true, vars:true */
import __CONTROLS__ = require('pages/util/controls');
import __PRISM_CDIALOG__ = require('prismux/prism.dlg');

module mPrism {
    var $minus, $plus, $delay;
    export class CSyncDlg extends __PRISM_CDIALOG__.CDialog {
        private _syncTime: number;
        constructor(layeredControl) {
            super(layeredControl);
            var a = this;
            a._registerEvent(['MinusSelected', 'PlusSelected']);
            a.addClass('sync');
            a._syncTime = 0;
        }
        setSyncTime(syncTime) {
            this._syncTime = syncTime || 0;
        }
        doOpen() {
            var a = this;
            var layout = new CHorizontalLayoutControl(null);
            var drawers = [
                function (index, $item) {
                    $item.removeClass('-fd').addClass('title').text(tr('LOC_SYNC_SUBTITLE_ID'));
                },
                function (index, $item) {
                    $plus = $('<div>', {
                        'class': 'plus',
                        'html': '<span></span>'
                    });
                    $minus = $('<div>', {
                        'class': 'minus',
                        'html': '<span></span>'
                    });
                    $item.append($plus).append($minus);
                    $item.removeClass('-fd').addClass('line');
                    a.setPosition(a._syncTime);
                },
                function (index, $item) {
                    $item.removeClass('-fd').addClass('delay').text(a._syncTime + ' sec');
                    $delay = $item;
                }];
            layout.setItemDrawers(drawers);
            layout.setDrawParam({
                padding: 30,
                width: 1280,
                align: 'hcenter',
                childAlign: 'hcenter'
            });
            a.setChildControl([layout], 0);
            a.setTimeout(5000);
        }
        setPosition(syncTime) {
            var a = this;
            if (a._syncTime > 30 || a._syncTime < -30) {
                return;
            }
            var $position = syncTime < 0 ? $minus : $plus;
            $position.find('span').css('width', Math.floor((Math.abs(syncTime) * 100) / 30) + '%');
            if (syncTime == 0) {
                $minus.find('span').css('width', '0%');
            }
            a._syncTime = syncTime;
        }
        doKeyEscape() {
            this.close();
            return true;
        }
        doKeyOk() {
            this.close('userAction');
            return true;
        }
        doKeyLeft() {
            var a = this;
            var syncTime = a._syncTime -= 0.5;
            if (syncTime < -30) {
                a._syncTime = -30;
                return true;
            }
            $delay.text(syncTime + ' sec');
            a.setPosition(syncTime);
            this._fireEvent('MinusSelected', syncTime);
            return true;
        }
        doKeyRight() {
            var a = this;
            var syncTime = a._syncTime += 0.5;
            if (syncTime > 30) {
                a._syncTime = 30;
                return true;
            }
            $delay.text(syncTime + ' sec');
            a.setPosition(syncTime);
            this._fireEvent('PlusSelected', syncTime);
            return true;
        }
    }

    if (!(<any>window).prism) {
        (<any>window).prism = {}
    }
    prism.createSyncDlg = function (layeredControl) {
        var syncDlg = new CSyncDlg(layeredControl);
        return syncDlg;
    };
}
export = mPrism;
