define(["require", "exports", 'pages/util/controls'], function (require, exports, __CONTROLS__) {
    var CTRDConflict = (function () {
        function CTRDConflict(aWhenOpened, aWhenClosed, aWhenFinished) {
            var service = hx.svc.CHANNEL_SEARCH_T;
            var depthTrd = 0;
            var selectedRegions = [];
            service.getTrdConflict(depthTrd, function (aCount, aList) {
                var makeTrdConflictData = function (aList) {
                    var ret = [];
                    for (var i = 0; i < aList.length; i++) {
                        ret[i] = {
                            text: aList[i],
                            fnAsync: function (aIndex, aCallback) {
                                service.setTrdConflict(depthTrd++, aIndex);
                                service.getTrdConflict(depthTrd, function (aCount, aListNext) {
                                    if (aListNext.length) {
                                        selectedRegions.push(aList[aIndex]);
                                        aCallback({
                                            title: selectedRegions.join(", "),
                                            data: makeTrdConflictData(aListNext)
                                        });
                                    }
                                    else {
                                        trdDlg.destroy();
                                        aWhenFinished();
                                    }
                                });
                            }
                        };
                    }
                    return ret;
                };
                if (aCount === 0) {
                    aWhenFinished();
                }
                else {
                    var $dlgTRD = $('<div>', { "class": '-page-full' });
                    var dlg = new __CONTROLS__.CLayeredGroupControl($dlgTRD);
                    var trdDlg = prism.createTRDConflictButtonDlg(dlg, makeTrdConflictData(aList));
                    trdDlg.open({
                        onClosed: function () {
                            aWhenClosed(trdDlg);
                            $dlgTRD.remove();
                            dlg.removeChildControl(trdDlg);
                            dlg.destroy();
                        }
                    });
                    aWhenOpened(trdDlg);
                }
            });
        }
        return CTRDConflict;
    })();
    return CTRDConflict;
});
