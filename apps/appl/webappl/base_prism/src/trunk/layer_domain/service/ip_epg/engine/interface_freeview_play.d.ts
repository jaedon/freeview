/**
 * Created by bglee on 2015-03-31.
 */
/// <reference path="spec_freeview_play.d.ts" />

interface IParsedScheduleInternal {
    program?: IProgramInformation;
    schedule?: IScheduleEvent;
    ondemand?: IOnDemandProgram;
    available? : boolean;
    availableIconUrl? : string;
}
interface IParsedSchedule {
    schedules: IScheduleEvent[];
    crids: {
        [crid: number]: IParsedScheduleInternal;
    };
    serviceId: string;
}
interface IParsedApplication {
    title: string;
    image: string;
    ait: string;
    index: number;
}
interface IParsedOnDemand {
    crids: {
        [crid: number]: IParsedScheduleInternal;
    };
    paginations: {
        [pagingIndex: string]: string;
    };
    numOfItems: Number;
}
interface IParsedSuggestion {
    keyword: string;
    suggestions: [string];
}
interface IParsedCategory {
    index: number;
    groupId: string;
    image: string;
    title: string;
    synopsis: string;
}
interface IParsedRecommendation {
    crids: {
        [crid: number]: IParsedScheduleInternal;
    };
    numOfItems: Number;
}
interface IPerson {
    GivenName: string;
    FamilyName: string;
}
interface IParsedProgram {
    crid: string;
    title: {
        main: string;
        secondary: string;
    };
    synopsis: string;
    keyword: string[];
    genre: string[];
    parentalGuidance: {
        isGuidance: boolean;
        explanatoryText: string;
    };
    credits: IPerson[];
    icon: {
        type: string;
        url: string;
    };
    index: number;
    // lcn, channel name, starttime, endtime, guidance, ondemand, series, episode
    //   o,       o     ,   o      ,     o  ,  o      ,    o    ,    x  ,     x
}
interface ISpecParser {
    element: {
        ondemand(ondemand: IOnDemandProgram);
    };
    xml: {
        schedule(xml: XMLDocument): IParsedSchedule;
        program(xml: XMLDocument): IParsedProgram;
        ondemand(xml: XMLDocument): IParsedOnDemand;
    };
    util: {
        duration(duration: string): number;
    };
}
interface IParsedElementOndemand {
    programURL: string;
    auxiliaryURL: string;
    startOfAvailability: number;
    endOfAvailability: number;
    publishedDuration: number;
    free: boolean;
    deliveryMode: string;
    mediaAvailabilityCS: boolean;
    fEpgAvailabilityCS: boolean;
    serviceIDRef: string;
}
interface IParsedElementScheduleEvent {
    crid: string;
    programURL: string;
    startTime: Date;
    duration: number;
    programmeCrid: string;
    seriesCrid: string;
}
interface IParsedElementProgram {
    crid: string;
    title: {
        main: string;
        secondary: string;
    };
    synopsis: string;
    keyword: string[];
    genre: string[];
    parentalGuidance: {
        isGuidance: boolean;
        explanatoryText: string;
    };
    credits: IPerson[];
    icon: {
        type: string;
        url: string;
    };
    index: number;
}