/**
 * Created by bglee on 2015-01-26.
 */

interface ifCButtonPromotion {
    showButton(aForceShowing?): boolean;
    hideButton(aKeepHiding?: boolean): void;
    isShowing(): boolean;
    getElement(): HTMLElement;
    setData(aData: any): void;
    getData(): any;
}