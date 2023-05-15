/**
 * Created by deptno on 2014. 7. 12..
 */
define(["require", "exports"], function(require, exports) {
    var loader = function(aLayeredGroupControl, __CONTROLS__, __FX_END_NOTIFIER__) {
        var __prefix = "./";
        require(["text!" + __prefix + "list.json"], function (aList) {
            function getObject() {

            }
            var listFile = JSON.parse(aList);
            for (var i = 0; i < listFile.length; i++) {
                listFile[i] = __prefix + listFile[i];
            }
            var listControl = new __CONTROLS__.CListControl(null);
            var dataProvider = new __CONTROLS__.CDataProvider([]);
            listControl.setDrawParam({
                scrollByItemSize: true,
                itemHeight: 126
            });
            listControl.setDataDrawer(function(aParam) {
                var index = aParam.index;
                var item = aParam.item;
                var $item = aParam.$item;
                $item.text("[" + index + "] " + item.desc);

            });
            listControl.onItemSelected = function(aParam) {
                var index = aParam.index;
                var item = aParam.item;
                var $item = aParam.$item;
                aLayeredGroupControl.removeChildControl(listControl);
                listControl.destroy();
                __FX_END_NOTIFIER__();
                item.fx();
            };
            listControl.setDataProvider(dataProvider);
            require(listFile, function(aList) {
                var object;
                for (var i = 0; i < arguments.length; i++) {
                    object = arguments[i];
                    var keys = Object.keys(object);
                    for (var j = 0; j < keys.length; j++) {
                        dataProvider.appendItem(object[keys[j]]);
                    }
                }
                listControl.draw();
            });
            aLayeredGroupControl.$layer().append(listControl._$self);
            aLayeredGroupControl.setChildControl([listControl]);
            aLayeredGroupControl.draw();
        });
    };
    return loader;
});
