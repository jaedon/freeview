/**
 * Created by bglee@humaxdigital.com on 2014. 3. 21..
 */
define(["require", "exports"], function (require, exports) {
    (function (eProductName) {
        eProductName[eProductName["DEFAULT"] = 0] = "DEFAULT";
        eProductName[eProductName["FVP4000T"] = 1] = "FVP4000T";
        eProductName[eProductName["HDFOXIRCONNECT"] = 2] = "HDFOXIRCONNECT";
        eProductName[eProductName["IR4000HD"] = 3] = "IR4000HD";
        eProductName[eProductName["HMS1000S"] = 4] = "HMS1000S";
        eProductName[eProductName["HMS1000T"] = 5] = "HMS1000T";
        eProductName[eProductName["ICORDPRO"] = 6] = "ICORDPRO";
        eProductName[eProductName["HD9000i"] = 7] = "HD9000i";
    })(exports.eProductName || (exports.eProductName = {}));
    var eProductName = exports.eProductName;
    (function (eOP) {
        eOP[eOP["MIDDLE_EAST"] = 0] = "MIDDLE_EAST";
        eOP[eOP["AUSDTT"] = 1] = "AUSDTT";
        eOP[eOP["UKDTT"] = 2] = "UKDTT";
        eOP[eOP["SES"] = 3] = "SES";
        eOP[eOP["ORF"] = 4] = "ORF";
    })(exports.eOP || (exports.eOP = {}));
    var eOP = exports.eOP;
    (function (eOIPF) {
        eOIPF[eOIPF["OIPF"] = 0] = "OIPF";
        eOIPF[eOIPF["HUMAX"] = 1] = "HUMAX";
    })(exports.eOIPF || (exports.eOIPF = {}));
    var eOIPF = exports.eOIPF;
    (function (eCAS) {
        eCAS[eCAS["NONE"] = 0] = "NONE";
        eCAS[eCAS["IR"] = 1] = "IR";
        eCAS[eCAS["NA"] = 2] = "NA";
        eCAS[eCAS["CI"] = 3] = "CI";
        eCAS[eCAS["IRUC"] = 4] = "IRUC";
    })(exports.eCAS || (exports.eCAS = {}));
    var eCAS = exports.eCAS;
    (function (eCASOTA) {
        eCASOTA[eCASOTA["NONE"] = 0] = "NONE";
        eCASOTA[eCASOTA["IRDETO"] = 1] = "IRDETO";
    })(exports.eCASOTA || (exports.eCASOTA = {}));
    var eCASOTA = exports.eCASOTA;
    (function (eModeDistribution) {
        eModeDistribution[eModeDistribution["DEBUG"] = 0] = "DEBUG";
        eModeDistribution[eModeDistribution["RELEASE"] = 1] = "RELEASE";
    })(exports.eModeDistribution || (exports.eModeDistribution = {}));
    var eModeDistribution = exports.eModeDistribution;
    exports.eISO639 = {
        eng: "eng",
        ger: "ger",
        tur: "tur",
        rus: "rus",
        fre: "fre",
        ara: "ara",
        per: "per",
        gre: "gre",
        ita: "ita",
        spa: "spa",
        gla: "gla",
        wel: "wel",
        gle: "gle",
        slo: "slo",
        ces: "cze"
    };
});
