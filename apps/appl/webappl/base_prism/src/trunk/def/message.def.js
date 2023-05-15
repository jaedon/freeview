/**
 * Created by mskang2 on 2014-06-09.
 */
define(["require", "exports"], function (require, exports) {
    (function (eGreenBtnCommand) {
        eGreenBtnCommand[eGreenBtnCommand["RECORD"] = 0] = "RECORD";
        eGreenBtnCommand[eGreenBtnCommand["SERIES_RECORD"] = 1] = "SERIES_RECORD";
        eGreenBtnCommand[eGreenBtnCommand["REMINDER"] = 2] = "REMINDER";
    })(exports.eGreenBtnCommand || (exports.eGreenBtnCommand = {}));
    var eGreenBtnCommand = exports.eGreenBtnCommand;
    (function (eWatchinHDBtnCommand) {
        eWatchinHDBtnCommand[eWatchinHDBtnCommand["GOTO_THE_HD_CHANNEL"] = 0] = "GOTO_THE_HD_CHANNEL";
        eWatchinHDBtnCommand[eWatchinHDBtnCommand["RECORD_THE_HD_CHANNEL"] = 1] = "RECORD_THE_HD_CHANNEL";
    })(exports.eWatchinHDBtnCommand || (exports.eWatchinHDBtnCommand = {}));
    var eWatchinHDBtnCommand = exports.eWatchinHDBtnCommand;
});
