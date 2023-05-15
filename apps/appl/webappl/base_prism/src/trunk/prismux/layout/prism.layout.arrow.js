define(["require", "exports"], function (require, exports) {
    /**
     * Created by bglee on 2015-02-06.
     */
    var CArrawLayout = (function () {
        function CArrawLayout(aParent, aClassName, aImageSrc) {
            var el = document.createElement("div");
            el.classList.add(CArrawLayout.CLASS_ARROW);
            el.classList.add(aClassName);
            el.style.backgroundImage = aImageSrc;
            el.style.backgroundRepeat = 'no-repeat';
            this._elArrow = el;
            aParent.appendChild(el);
        }
        CArrawLayout.prototype.show = function () {
            this._elArrow.classList.add(CArrawLayout.CLASS_SHOW);
            this._elArrow.classList.remove(CArrawLayout.CLASS_FADE_OUT);
            this._elArrow.classList.add(CArrawLayout.CLASS_FADE_IN);
            return this;
        };
        CArrawLayout.prototype.hide = function () {
            this._elArrow.classList.remove(CArrawLayout.CLASS_SHOW);
            this._elArrow.classList.remove(CArrawLayout.CLASS_FADE_IN);
            this._elArrow.classList.add(CArrawLayout.CLASS_FADE_OUT);
            return this;
        };
        CArrawLayout.CLASS_ARROW = "-arrow";
        CArrawLayout.CLASS_SHOW = "-show";
        CArrawLayout.CLASS_FADE_IN = "-fadeIn";
        CArrawLayout.CLASS_FADE_OUT = "-fadeOut";
        return CArrawLayout;
    })();
    return CArrawLayout;
});
