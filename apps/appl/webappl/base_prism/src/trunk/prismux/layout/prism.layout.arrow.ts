/**
 * Created by bglee on 2015-02-06.
 */
class CArrawLayout {
    static CLASS_ARROW = "-arrow";
    static CLASS_SHOW = "-show";
    static CLASS_FADE_IN = "-fadeIn";
    static CLASS_FADE_OUT = "-fadeOut";

    private _elArrow: HTMLElement;
    constructor(aParent: HTMLElement, aClassName: string, aImageSrc: string) {
        var el = document.createElement("div");
        el.classList.add(CArrawLayout.CLASS_ARROW);
        el.classList.add(aClassName);
        el.style.backgroundImage = aImageSrc;
        el.style.backgroundRepeat = 'no-repeat';
        this._elArrow = el;
        aParent.appendChild(el);
    }
    show() {
        this._elArrow.classList.add(CArrawLayout.CLASS_SHOW);
        this._elArrow.classList.remove(CArrawLayout.CLASS_FADE_OUT);
        this._elArrow.classList.add(CArrawLayout.CLASS_FADE_IN);
        return this;
    }
    hide() {
        this._elArrow.classList.remove(CArrawLayout.CLASS_SHOW);
        this._elArrow.classList.remove(CArrawLayout.CLASS_FADE_IN);
        this._elArrow.classList.add(CArrawLayout.CLASS_FADE_OUT);
        return this;
    }
}
export = CArrawLayout;
