var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", 'prismux/prism.dlg'], function (require, exports, __PRISM_CDIALOG__) {
    var mPrism;
    (function (mPrism) {
        var $minus, $plus, $delay;
        var CSyncDlg = (function (_super) {
            __extends(CSyncDlg, _super);
            function CSyncDlg(layeredControl) {
                _super.call(this, layeredControl);
                var a = this;
                a._registerEvent(['MinusSelected', 'PlusSelected']);
                a.addClass('sync');
                a._syncTime = 0;
            }
            CSyncDlg.prototype.setSyncTime = function (syncTime) {
                this._syncTime = syncTime || 0;
            };
            CSyncDlg.prototype.doOpen = function () {
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
                    }
                ];
                layout.setItemDrawers(drawers);
                layout.setDrawParam({
                    padding: 30,
                    width: 1280,
                    align: 'hcenter',
                    childAlign: 'hcenter'
                });
                a.setChildControl([layout], 0);
                a.setTimeout(5000);
            };
            CSyncDlg.prototype.setPosition = function (syncTime) {
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
            };
            CSyncDlg.prototype.doKeyEscape = function () {
                this.close();
                return true;
            };
            CSyncDlg.prototype.doKeyOk = function () {
                this.close('userAction');
                return true;
            };
            CSyncDlg.prototype.doKeyLeft = function () {
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
            };
            CSyncDlg.prototype.doKeyRight = function () {
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
            };
            return CSyncDlg;
        })(__PRISM_CDIALOG__.CDialog);
        mPrism.CSyncDlg = CSyncDlg;
        if (!window.prism) {
            window.prism = {};
        }
        prism.createSyncDlg = function (layeredControl) {
            var syncDlg = new CSyncDlg(layeredControl);
            return syncDlg;
        };
    })(mPrism || (mPrism = {}));
    return mPrism;
});
