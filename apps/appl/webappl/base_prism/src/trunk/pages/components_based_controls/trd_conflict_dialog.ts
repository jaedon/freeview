///<reference path="../../def/sugar.d.ts" />
///<reference path="../../def/prism.d.ts" />
/**
 * Created by bglee@humaxdigital.com on 2014. 6. 14..
 */
import __CONTROLS__ = require('pages/util/controls');
declare var hx: ifSugar;

class CTRDConflict {
    constructor(aWhenOpened: (aDlg) => void, aWhenClosed: (aDlg) => void, aWhenFinished) {
        var service = hx.svc.CHANNEL_SEARCH_T;
        var depthTrd = 0;
        var selectedRegions: string[] = [];
        service.getTrdConflict(depthTrd, (aCount: number, aList: string[]) => {
            var makeTrdConflictData = (aList): ifPrismDlgButtonInfoAsync[] => {
                var ret = [];
                for (var i = 0; i < aList.length; i++) {
                    ret[i] = {
                        text: aList[i],
                        fnAsync: (aIndex: number, aCallback: (aNextInfo) => void) => {
                            service.setTrdConflict(depthTrd++, aIndex);
                            service.getTrdConflict(depthTrd, (aCount: number, aListNext: string[]) => {
                                if (aListNext.length) {
                                    selectedRegions.push(aList[aIndex]);
                                    aCallback({
                                        title: selectedRegions.join(", "),
                                        data: makeTrdConflictData(aListNext)
                                    });
                                } else {
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
            } else {
                var $dlgTRD : JQuery = $('<div>', { "class": '-page-full'});
                var dlg  = new __CONTROLS__.CLayeredGroupControl($dlgTRD);
                var trdDlg : any = prism.createTRDConflictButtonDlg(dlg, makeTrdConflictData(aList));
                trdDlg.open({
                    onClosed: () => {
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
}
export = CTRDConflict;
