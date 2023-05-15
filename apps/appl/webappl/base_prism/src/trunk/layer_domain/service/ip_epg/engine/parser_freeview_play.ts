/**
 * Created by bglee on 2015-03-31.
 */
/// <reference path="../../../../def/sugar.d.ts" />
/// <reference path="spec_freeview_play.d.ts" />
/// <reference path="interface_freeview_play.d.ts" />
declare var hx: ifSugar;
var util = {
    duration(duration: string) {
        var pt = /^PT([0-9]*H)*([0-9]*M)*([0-9]*S)*/.exec(duration);
        var ret = 0;
        if (pt) {
            if (pt[1]) {
                ret += parseInt(pt[1], 10) * 60 * 60;
            }
            if (pt[2]) {
                ret += parseInt(pt[2], 10) * 60;
            }
            if (pt[3]) {
                ret += parseInt(pt[3], 10);
            }
        }
        return ret;
    },
    programIdParser(programId: string) {
        var ret = {
            onid: '',
            tsid: '',
            svcid: '',
            eventid: ''
        };
        if (!programId) {
            return ret;
        }
        var splited = programId.split('://');
        if (splited[1]) {
            splited = splited[1].split(';');
            if (splited[1]) {
                ret.eventid = splited[1] || '';
            }
            if (splited[0]) {
                splited = splited[0].split('.');
                ret.onid = splited[0] || '';
                ret.tsid = splited[1] || '';
                ret.svcid = splited[2] || '';
            }
        }
        return ret;
    },
    parentalRatingParser(href: string): boolean {
        switch(href) {
            case "urn:dtg:cs:BBFCContentRatingCS:2002:PG":
            case "urn:dtg:cs:BBFCContentRatingCS:2002:12":
            case "urn:dtg:cs:BBFCContentRatingCS:2002:15":
            case "urn:dtg:cs:BBFCContentRatingCS:2002:18":
            case "urn:fvc:metadata:cs:ContentRatingCS:2014-07:parental_guidance":
            case "urn:fvc:metadata:cs:ContentRatingCS:2014-07:twelve":
            case "urn:fvc:metadata:cs:ContentRatingCS:2014-07:fifteen":
            case "urn:fvc:metadata:cs:ContentRatingCS:2014-07:sixteen":
            case "urn:fvc:metadata:cs:ContentRatingCS:2014-07:eighteen":
            case "urn:dtg:metadata:cs:DTGContentWarningCS:2011:G":
                return true;
            default:
                return false;
        }
    },
    genreParser(href: string): string[] {
        switch (href) {
            case "urn:fvc:metadata:cs:ContentSubjectCS:2014-07:0":
                return ["Unclassified"];
            case "urn:fvc:metadata:cs:ContentSubjectCS:2014-07:1":
                return ["Movie"];
            case "urn:fvc:metadata:cs:ContentSubjectCS:2014-07:2":
                return ["News and Factual"];
            case "urn:fvc:metadata:cs:ContentSubjectCS:2014-07:2.1":
                return ["News/Current affairs"];
            case "urn:fvc:metadata:cs:ContentSubjectCS:2014-07:2.2":
                return ["Arts/Culture (without music)"];
            case "urn:fvc:metadata:cs:ContentSubjectCS:2014-07:2.3":
                return ["Social/Political Issues/Economics"];
            case "urn:fvc:metadata:cs:ContentSubjectCS:2014-07:3":
                return ["Entertainment"];
            case "urn:fvc:metadata:cs:ContentSubjectCS:2014-07:3.1":
                return ["Show/Game show"];
            case "urn:fvc:metadata:cs:ContentSubjectCS:2014-07:3.2":
                return ["Music/Ballet/Dance"];
            case "urn:fvc:metadata:cs:ContentSubjectCS:2014-07:4":
                return ["Sport"];
            case "urn:fvc:metadata:cs:ContentSubjectCS:2014-07:5":
                return ["Children's/Youth's programming"];
            case "urn:fvc:metadata:cs:ContentSubjectCS:2014-07:6":
                return ["Education/Science/Factual topics"];
            case "urn:fvc:metadata:cs:ContentSubjectCS:2014-07:7":
                return ["Lifestyle/Leisure hobbies"];
            case "urn:fvc:metadata:cs:ContentSubjectCS:2014-07:8":
                return ["Drama"];
        }
    },
    audioPurposeParser(nodeList: NodeList): boolean {
        for (var i = 0 ; i < nodeList.length ; i++) {
            var href = (<any>nodeList[i]).getAttribute('purpose');
            if (href === "urn:tva:metadata:cs:AudioPurposeCS:2007:1") {
                return true;
            }
        }
        return false;
    },
    verticalSizeParser(size: string): boolean {
        var isHD: boolean = false;
        var nSize = Number(size);
        if (nSize >= 720) {
            isHD = true;
        }
        return isHD;
    }
};
var parserElement = {
    ondemand(ondemand: IOnDemandProgram): IParsedElementOndemand {
        if (!ondemand) {
            return null;
        }
        var nodeInstanceDescription = ondemand.querySelector('InstanceDescription');
        if (nodeInstanceDescription) {
            var nodelistGenre: Element[] = <any>nodeInstanceDescription.querySelectorAll('Genre');
            var fEpgAvailabilityCS = nodelistGenre[1] ? /fepg_available/.test(nodelistGenre[1].getAttribute('href')) : false;
            //var aspectRatio = nodeInstanceDescription.querySelector('AspectRatio');
            var verticalSize = nodeInstanceDescription.querySelector('VerticalSize');
            var audioPurpose = nodeInstanceDescription.querySelectorAll('AudioLanguage');
        }
        var programUrlEl = ondemand.querySelector('ProgramURL');
        var auxiliaryUrlEl = ondemand.querySelector('AuxiliaryURL');
        var startOfAvailabilityEl = ondemand.querySelector('StartOfAvailability');
        var endOfAvailabilityEl = ondemand.querySelector('EndOfAvailability');
        var publishedDurationEl = ondemand.querySelector('PublishedDuration');
        var freeEl = ondemand.querySelector('Free');
        var deliveryModeEl = ondemand.querySelector('DeliveryMode');
        var serviceIDRef = ondemand.getAttribute('serviceIDRef');
        return {
            programURL: programUrlEl ? programUrlEl.textContent : null,
            contentType: programUrlEl ? programUrlEl.getAttribute('contentType') : null,
            auxiliaryURL: auxiliaryUrlEl ? auxiliaryUrlEl.textContent : null,
            startOfAvailability: startOfAvailabilityEl ? new Date(startOfAvailabilityEl.textContent).valueOf() / 1000 : null,
            endOfAvailability: endOfAvailabilityEl ? new Date(endOfAvailabilityEl.textContent).valueOf() / 1000 : null,
            publishedDuration: publishedDurationEl ? util.duration(publishedDurationEl.textContent) : null,
            free: freeEl ? JSON.parse(freeEl.getAttribute('value')) : null,
            deliveryMode: deliveryModeEl ? deliveryModeEl.textContent : null,
            mediaAvailabilityCS: nodelistGenre ? /media_available/.test(nodelistGenre[0].getAttribute('href')) : null,
            fEpgAvailabilityCS: fEpgAvailabilityCS || null,
            audioPurpose: audioPurpose ? util.audioPurposeParser(audioPurpose): false,
            isHD: verticalSize ? util.verticalSizeParser(verticalSize.textContent) : false,
            serviceIDRef: serviceIDRef || null
        }
    },
    scheduleEvent(scheduleEvent: IScheduleEvent): IParsedElementScheduleEvent {
        if (!scheduleEvent) {
            return null;
        }
        var programEl = scheduleEvent.querySelector('Program');
        var programUrlEl = scheduleEvent.querySelector('ProgramURL');
        var startTimeEl = scheduleEvent.querySelector('PublishedStartTime');
        var durationEl = scheduleEvent.querySelector('PublishedDuration');
        var InstanceDescriptionEl = scheduleEvent.querySelector('InstanceDescription');
        var programmeCridEl = InstanceDescriptionEl ? InstanceDescriptionEl.querySelector('OtherIdentifier[type="eit-programme-crid"]') : null;
        var seriesCridEl = InstanceDescriptionEl ? InstanceDescriptionEl.querySelector('OtherIdentifier[type="eit-series-crid"]') : null;
        return {
            crid: programEl ? programEl.getAttribute('crid') : '',
            programURL: programUrlEl ? programUrlEl.textContent : '',
            startTime: startTimeEl ? new Date(startTimeEl.textContent) : null,
            duration: durationEl ? util.duration(durationEl.textContent) : 0,
            programmeCrid: programmeCridEl ? programmeCridEl.textContent : '',
            seriesCrid: seriesCridEl ? seriesCridEl.textContent : ''
        };
    },
    programInformation(programInformation: IProgramInformation): IParsedElementProgram {
        if (!programInformation) {
            return null;
        }
        var bd: IProgramInformation_BasicDescription = programInformation.querySelector('BasicDescription');
        var mo = programInformation.querySelector('MemberOf');
        var title = bd.querySelector('Title[type="main"]');
        var ret = {
            crid: programInformation.getAttribute('programId'),
            title: {
                main: title ? title.textContent : '',
                secondary: ''
            },
            synopsis: '',
            keyword: [],
            genre: [],
            parentalGuidance: {
                isGuidance: false,
                explanatoryText: ''
            },
            credits: [],
            icon: {
                type: '',
                url: ''
            },
            index: mo ? Number(mo.getAttribute('index')) : 9999
        };
        var secondaryTitle = bd.querySelector('Title[type="secondary"]');
        if (secondaryTitle) {
            ret.title.secondary = secondaryTitle.textContent;
        }
        var synopsis = (bd.querySelector('Synopsis[length="long"]') || bd.querySelector('Synopsis[length="medium"]') || bd.querySelector('Synopsis[length="short"]'));
        if (synopsis) {
            ret.synopsis = synopsis.textContent;
        }
        var keywords = bd.querySelectorAll('Keyword');
        if (keywords) {
            ret.keyword = <string[]>Array.prototype.map.call(<any>keywords, function(el: Element) { return el.textContent; });
        }
        var genre = bd.querySelector('Genre');
        if (genre) {
            ret.genre = util.genreParser(genre.getAttribute('href'));
        }
        var parentalRating = bd.querySelector('ParentalRating');
        if (parentalRating) {
            var href = parentalRating.getAttribute('href');
            ret.parentalGuidance.isGuidance = util.parentalRatingParser(href);
            var explanatoryText = bd.querySelector('ExplanatoryText');
            if (explanatoryText) {
                ret.parentalGuidance.explanatoryText = explanatoryText.textContent;
            }
        }
        var mediaUri = bd.querySelector('MediaUri');
        if (mediaUri) {
            ret.icon.type = mediaUri.getAttribute('contentType');
            ret.icon.url = mediaUri.textContent;
        }
        return ret;
    }
};
var parserXML = {
    schedule(xml: XMLDocument): IParsedSchedule {
        var root = xml.documentElement;
        var programInformationTable = root.querySelector('ProgramInformationTable');
        var programLocationTable = root.querySelector('ProgramLocationTable');
        if (!programInformationTable) {
            //throw 'Programe Information Table';
        }
        if (!programLocationTable) {
            //throw 'Programe Information Table';
        }
        var schedules = programLocationTable ? programLocationTable.querySelectorAll('ScheduleEvent') : null;
        var programs = programInformationTable ? programInformationTable.querySelectorAll('ProgramInformation') : null;
        var ondemands = programLocationTable ? programLocationTable.querySelectorAll('OnDemandProgram') : null;
        //console.log(programInformationTable);
        //console.log(programLocationTable);

        var ret: IParsedSchedule = <any>{
            schedules: schedules || [],
            crids: <{
                program: IProgramInformation[];
                ondemand: IOnDemandProgram[];
            }>{},
            serviceId: -1
        };
        var elSchedule = programLocationTable ? programLocationTable.querySelector('Schedule') : null;
        if (elSchedule) {
            /* COMMENT
             * serviceIdRef is bug for MDS Test;
             * * bglee@humaxdigital.com, 20:30, 2015. 4. 25.
             */

            ret.serviceId = elSchedule.getAttribute('serviceIDRef') || elSchedule.getAttribute('serviceIdRef');
        }
        //for (var i = 0, schedule; i < schedules.length; i++) {
        //    schedule = schedules.item(i);
        //    var crid = schedule.querySelector('Program').getAttribute('crid');
        //    if (!ret.crids[crid]) {
        //        ret.crids[crid] = {};
        //    }
        //    ret.crids[crid].schedule = schedule;
        //}
        if (ondemands) {
            for (var i = 0, ondemand; i < ondemands.length; i++) {
                ondemand = ondemands.item(i);
                var crid = ondemand.querySelector('Program').getAttribute('crid');
                if (!ret.crids[crid]) {
                    ret.crids[crid] = {};
                }
                ret.crids[crid].ondemand = ondemand;
            }
        }
        if (programs) {
            for (var i = 0, program; i < programs.length; i++) {
                program = programs.item(i);
                var crid = program.getAttribute('programId');
                if (!ret.crids[crid]) {
                    ret.crids[crid] = {};
                }
                ret.crids[crid].program = program;
            }
        }
        return ret;
    },
    program(xml: XMLDocument): IParsedProgram {
        var root: IProgramInformation = xml.documentElement;
        var programeInformation = root.querySelector('ProgramInformation');
        var bd: IProgramInformation_BasicDescription = programeInformation.querySelector('BasicDescription');
        var mo = programeInformation.querySelector('MemberOf');
        var ret = {
            crid: programeInformation.getAttribute('programId'),
            title: {
                main: bd.querySelector('Title[type="main"]').textContent,
                secondary: ''
            },
            synopsis: '',
            keyword: <string[]>Array.prototype.map.call(<any>bd.querySelectorAll('Keyword'), function(el: Element) { return el.textContent; }),
            genre: [],
            parentalGuidance: {
                isGuidance: false,
                explanatoryText: ''
            },
            credits: [],
            icon: {
                type: '',
                url: ''
            },
            index: mo ? Number(mo.getAttribute('index')) : 9999
        };
        var secondaryTitle = bd.querySelector('Title[type="secondary"]');
        if (secondaryTitle) {
            ret.title.secondary = secondaryTitle.textContent;
        }
        var synopsis = (bd.querySelector('Synopsis[length="long"]') || bd.querySelector('Synopsis[length="medium"]') || bd.querySelector('Synopsis[length="short"]'));
        if (synopsis) {
            ret.synopsis = synopsis.textContent;
        }
        var genre = bd.querySelector('Genre');
        if (genre) {
            ret.genre = util.genreParser(genre.getAttribute('href'));
        }
        var parentalRating = bd.querySelector('ParentalRating');
        if (parentalRating) {
            var href = parentalRating.getAttribute('href');
            ret.parentalGuidance.isGuidance = util.parentalRatingParser(href);
            var explanatoryText = bd.querySelector('ExplanatoryText');
            if (explanatoryText) {
                ret.parentalGuidance.explanatoryText = explanatoryText.textContent;
            }

        }
        /* author: bglee@humaxdigital.com
         * date: 2015-04-05 오후 3:24
         * comments:
            - do parse credits
         */
        var mediaUri = bd.querySelector('MediaUri');
        if (mediaUri) {
            ret.icon.type = mediaUri.getAttribute('contentType');
            ret.icon.url = mediaUri.textContent;
        }
        return ret;
    },
    application(xml: XMLDocument): {[group: string]: IParsedApplication[]} {
        var ret = <{[group: string]: IParsedApplication[]}>{ 'undefined': [] };
        var groupInformations = xml.querySelectorAll('GroupInformation');
        var applications = [];
        var collections = {};
        var distributor = {
            'application': function(element) {
                applications.push(element)
            },
            'otherCollection': function(element) {
                //var id = element.querySelector('Title').textContent;
                var id = element.getAttribute('groupId');
                id = id.substring(id.lastIndexOf('/'));
                collections[element.getAttribute('groupId')] = id;
                ret[id] = [];
            }
        };
        for (var i = 0; i < groupInformations.length; i++) {
            distributor[(<IGroupInformation>groupInformations[i]).querySelector('GroupType').getAttribute('value')](groupInformations[i]);
        }
        var crid, position, groupId, title, xmlAitUrl, imageUrl, relatedMaterials;
        for (var i = 0, groupInformation; i < applications.length; i++) {
            groupInformation = <IGroupInformation_BasicDescription_Application>applications[i];
            crid = '';
            position = 9999;
            groupId = groupInformation.getAttribute('groupId');
            title = groupInformation.querySelector('Title').textContent;
            //var synopsis = groupInformation.querySelector('Synopsis').textContent;
            xmlAitUrl = '';
            imageUrl = '';
            relatedMaterials = groupInformation.querySelectorAll('RelatedMaterial');
            for (var j = 0; j < relatedMaterials.length; j++) {
                var material = relatedMaterials[j];
                var how = material.querySelector('HowRelated') ? material.querySelector('HowRelated').getAttribute('href') : null;
                if (how === 'urn:tva:metadata:cs:HowRelatedCS:2012:19') {//base image url
                    imageUrl = material.querySelector('MediaUri').textContent;
                } else if (how.indexOf('urn:fvc:metadata:cs:ImageVariantCS:2017-02') === 0) {//variant image url
                    imageUrl = material.querySelector('MediaUri').textContent;
                } else if (how === 'urn:tva:metadata:cs:HowRelatedCS:2012:10.5') {//xml ait url
                    xmlAitUrl = material.querySelector('MediaUri').textContent;
                }
            }
            var memberOf = groupInformation.querySelector('MemberOf');
            if (memberOf) {
                crid = memberOf.getAttribute('crid');
                position = parseInt(memberOf.getAttribute('index'));
            }
            ret[collections[crid]].push({
                title: title,
                image: imageUrl,
                ait: xmlAitUrl,
                index: position
            });
        }
        for (var group in ret) {
            ret[group].sort(function(prev, after) {
                return prev.index - after.index >= 0 ? 1 : -1;
            })
        }
        return ret;
    },
    suggestion(xml: XMLDocument) : IParsedSuggestion {
        var suggestionsTable = xml.querySelector('Suggestions');
        if (!suggestionsTable) {
            throw 'invalid XML for ondemand suggestions';
        }
        var suggestionsTerm = suggestionsTable.getAttribute('suggestionsTerm');
        var suggestions = suggestionsTable.querySelectorAll('Suggestion');
        //console.log(suggestions);
        var ret: IParsedSuggestion = <any>{
            keyword: String,
            suggestions: []
        };
        // <Suggestions>
        ret.keyword = suggestionsTerm;
        var i = 0, suggestion, len = suggestions.length;
        for (i = 0; i < len; i+=1) {
            suggestion = suggestions.item(i);
            ret.suggestions.push(suggestion.textContent);
        }
        return ret;
    },
    ondemand(xml: XMLDocument): IParsedOnDemand {
        var root = xml.documentElement;
        var programInformationTable = root.querySelector('ProgramInformationTable');
        var groupInformationTable = root.querySelector('GroupInformationTable');
        var programLocationTable = root.querySelector('ProgramLocationTable');
        if (!programInformationTable || !groupInformationTable || !programLocationTable) {
            throw 'invalid XML for ondemand search';
        }
        var programs = programInformationTable.querySelectorAll('ProgramInformation');
        var groupinfo = groupInformationTable.querySelectorAll('GroupInformation');
        var ondemands = programLocationTable.querySelectorAll('OnDemandProgram');
        //console.log(programInformationTable);
        //console.log(programLocationTable);
        var ret: IParsedOnDemand = <any>{
            crids: <{
                program: IProgramInformation[];
                ondemand: IOnDemandProgram[];
            }>{},
            paginations: <{
                paginationUrl: String;
            }>{},
            numOfItems: Number
        };
        var i = 0, j = 0, program, crid, ondemand, len = programs.length;
        var len2, pageInformation, paginationUrl, material, how, indexStr, relatedMaterials;

        // <ProgramInformationTable>
        for (i = 0; i < len; i+=1) {
            program = programs.item(i);
            crid = program.getAttribute('programId');
            if (!ret.crids[crid]) {
                ret.crids[crid] = {};
            }
            ret.crids[crid].program = program;
        }
        // <GroupInformationTable>
        var applications = [];
        var collections = [];
        var distributor = {
            'application': function(element) {
                applications.push(element)
            },
            'otherCollection': function(element) {
                collections.push(element);
            }
        };
        len = groupinfo.length;
        for (i = 0; i < len; i+=1) {
            distributor[(<IGroupInformation>groupinfo[i]).querySelector('GroupType').getAttribute('value')](groupinfo[i]);
        }
        len = collections.length;
        if (len) {
            ret.numOfItems = Number(collections[0].getAttribute('numOfItems'));
            for (i = 0; i < len; i+=1) {
                pageInformation = collections[i];
                relatedMaterials = pageInformation.querySelectorAll('RelatedMaterial');
                len2 = relatedMaterials.length;
                for (j = 0; j < len2; j+=1) {
                    material = relatedMaterials[j];
                    how = material.querySelector('HowRelated').getAttribute('href');
                    indexStr = how.substring(how.lastIndexOf(':') + 1); // last, next..
                    if (how.indexOf("urn:fvc:metadata:cs:HowRelatedCS:2015-12:pagination") > -1) {
                        paginationUrl = material.querySelector('MediaUri').textContent;
                        ret.paginations[indexStr] = paginationUrl;
                    }
                }
            }
        }
        // <ProgramLocationTable >
        len = ondemands.length;
        for (i = 0; i < len; i+=1) {
            ondemand = ondemands.item(i);
            crid = ondemand.querySelector('Program').getAttribute('crid');
            if (!ret.crids[crid]) {
                ret.crids[crid] = {};
            }
            ret.crids[crid].ondemand = ondemand;
        }
        return ret;
    },
    category(xml: XMLDocument): IParsedCategory[] {
        var ret = <IParsedCategory[]>[];
        var groupInformations = xml.querySelectorAll('GroupInformation');
        var groupInformation;
        var groupId = '', numOfItems, xmlAitUrl = '', imageUrl = '', position = 9999, title, synopsis, relatedMaterials, memberOf, how, material;
        for (var i = 0, lenGroup = groupInformations.length; i < lenGroup; i += 1) {
            groupInformation = <IGroupInformation>groupInformations[i];
            groupId = groupInformation.getAttribute('groupId');
            numOfItems = groupInformation.getAttribute('numOfItems') || numOfItems;
            title = groupInformation.querySelector('Title');
            synopsis = groupInformation.querySelector('Synopsis');
            relatedMaterials = groupInformation.querySelectorAll('RelatedMaterial');
            for (var j = 0, lenMaterial = relatedMaterials.length; j < lenMaterial; j += 1) {
                material = relatedMaterials[j];
                how = material.querySelector('HowRelated') ? material.querySelector('HowRelated').getAttribute('href') : null;
                if (how === 'urn:tva:metadata:cs:HowRelatedCS:2012:19') {//base image url
                    imageUrl = material.querySelector('MediaUri').textContent;
                } else if (how === 'urn:tva:metadata:cs:HowRelatedCS:2012:10.5') {//xml ait url
                    xmlAitUrl = material.querySelector('MediaUri').textContent;
                }
            }
            memberOf = groupInformation.querySelector('MemberOf');
            if (!memberOf) {
                // should be ordered by 'index' of MemberOf
                continue;
            }
            ret.push({
                title: title ? title.textContent : '',
                groupId: groupId,
                image: imageUrl,
                ait: xmlAitUrl,
                synopsis: synopsis ? synopsis.textContent : '',
                index: memberOf ? parseInt(memberOf.getAttribute('index')) : position
            });
        }
        if (Number(numOfItems) !== ret.length) {
            hx.log('debug', '[MDS] numOfItems of category(recommendation) is not matched.');
        }
        ret.sort(function(prev, after) {
            return prev.index - after.index >= 0 ? 1 : -1;
        });
        return ret;
    },
    recommendation(xml: XMLDocument): IParsedRecommendation {
        var root = xml.documentElement;
        var programInformationTable = root.querySelector('ProgramInformationTable');
        var groupInformationTable = root.querySelector('GroupInformationTable');
        var programLocationTable = root.querySelector('ProgramLocationTable');
        if (!programInformationTable || !groupInformationTable || !programLocationTable) {
            throw 'invalid XML for ondemand search';
        }
        var programs = programInformationTable.querySelectorAll('ProgramInformation');
        var groupinfos = groupInformationTable.querySelectorAll('GroupInformation');
        var ondemands = programLocationTable.querySelectorAll('OnDemandProgram');
        //console.log(programInformationTable);
        //console.log(programLocationTable);
        var ret: IParsedRecommendation = <any>{
            crids: <{
                program: IProgramInformation[];
                ondemand: IOnDemandProgram[];
            }>{},
            numOfItems: Number
        };
        var i = 0, j = 0, len = programs ? programs.length : 0;
        var program, crid, ondemand, group;
        // <ProgramInformationTable>
        for (i = 0; i < len; i+=1) {
            program = programs.item(i);
            crid = program.getAttribute('programId');
            if (!ret.crids[crid]) {
                ret.crids[crid] = {};
            }
            ret.crids[crid].program = program;
        }
        // <GroupInformationTable>
        len = groupinfos ? groupinfos.length : 0;
        for (i = 0; i < len; i+=1) {
            group = groupinfos.item(i);
            ret.numOfItems = group.getAttribute('numOfItems') || ret.numOfItems;
        }
        // <ProgramLocationTable >
        len = ondemands ? ondemands.length : 0;
        for (i = 0; i < len; i+=1) {
            ondemand = ondemands.item(i);
            crid = ondemand.querySelector('Program').getAttribute('crid');
            if (!ret.crids[crid]) {
                ret.crids[crid] = {};
            }
            ret.crids[crid].ondemand = ondemand;
        }
        return ret;
    },
};
var CSpecParser = {
    util: util,
    xml: parserXML,
    element: parserElement
};
export = CSpecParser;