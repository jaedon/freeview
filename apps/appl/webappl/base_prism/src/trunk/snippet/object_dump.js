/**
 * Created by deptno on 2014. 6. 25..
 */
function object_dump(aObject, aTargetObject) {
    var item;
    aTargetObject = aTargetObject || {};
    for (var key in aObject) {
        item = aObject[key];
        if (typeof item == "object") {
            console.log(key + " make object");
            aTargetObject[key] = object_dump(item);
        } else {
            aTargetObject[key] = item;
            console.log(key);
        }
    }
    return aTargetObject;
}

// aProperties: string array
// aSpecityObjectProperties { object_type: aProperties }

var plugin_object_dump = (function() {
    var depth = 0;
    var tab = "    ";
    var log = function(msg) {
        var loop = depth;
        var ret = "";
        while (loop && loop--) {
            msg = tab + msg;
        }
        console.log(msg)
    };
    function plugin_object_dump(aTargetObject, aObject, aProperties, aSpecityObjectProperties) {
        aTargetObject = aTargetObject || {};
        for (var i = 0; i < aProperties.length; i++) {
            var key = aProperties[i];
            var value = aObject[key];
            if (aSpecityObjectProperties.hasOwnProperty(key)) {
                // object
                var props = aSpecityObjectProperties[key];
                depth++;
                log("[Object]: " + key);
                aTargetObject[key] = plugin_object_dump(null, value, props, aSpecityObjectProperties);
                depth --;
            } else {
                log("[" + key + "]: " + value);
                aTargetObject[key] = value;
                // primitive
            }
        }
        return aTargetObject;
    }
    return plugin_object_dump;
})();

[
    "linkType",
    "howRelatedSchemId",
    "termId",
    "groupId",
    "precedence",
    "mediaUri",
    "dvbBinLocator",
    "proTextNum",
    "promotionalText",
    "promotionalTextLangCode",
    "isDefaultIcon",
    "iconId"
]

[
    "identifierType",
    "scheTimeReliability",
    "inlineSvc: number",
    "startDate",
    "dvbSvcTripletId",
    "tsId",
    "onId",
    "svcId",
    "startTime",
    "duration",
    "evtId",
    "tvaId",
    "component",
    "earlyStartWin",
    "lateEndWin"
]

[
    "iconId",
    "transPortMode",
    "positionFlag",
    "coordinateSystem",
    "horizontalOrigin",
    "verticalOrigin",
    "iconType",
    "dataByteSize",
    "dataByte",
    "iconUrl"
]

[
    "imageIconInfo",
    "totalLinkCnt",
    "serviceId",
    "yearOffset"
]
