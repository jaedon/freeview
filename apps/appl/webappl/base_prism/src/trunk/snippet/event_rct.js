/**
 * Created by deptno on 2014. 8. 11..
 */
var t = new window['Collection']();
t.push({
    termId: 2,
    eventName: "EastEnders3",
    eventText: "EastEndersis a British television soap opera, first broadcast in the United Kingdom on BBC One on 19 February 1985 and continuing to today.<br>" +
        "EastEnders storylines examine the domestic and professional lives of the people who live and work in the fictional London Borough of Walford in the East End of London. The series primarily centres around the EastEndersis a British television soap opera, first broadcast in the United Kingdom on BBC One on 19 February 1985 and continuing to today. EastEnders storylines examine the domestic and professional lives of the people who live and work in the fictional London Borough of \
    Walford in the East End of London. The series primarily centres around the East End of London. The series primarily centres around the EastEndersis"})
t.push({
    termId: 5,
    eventName: "bglee # 05",
    eventText: new Date().toString()
});
t.push({
    termId: 2,
    eventName: "bglee # 04",
    eventText: new Date().getTime()
});
hx.svc.AV_CTRL.fireEvent("RctInfoChanged", 3, t);
