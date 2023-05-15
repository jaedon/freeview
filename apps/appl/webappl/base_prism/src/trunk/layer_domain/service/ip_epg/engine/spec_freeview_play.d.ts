/**
 * Created by bglee on 2015-03-31.
 */
interface IList<T extends Element> extends NodeList {
    length: number;
    item(index: number): T;
    [index: number]: T;
}

interface ITVAMain extends Element {
    querySelector(element: string): Element;
    querySelector(element: 'ProgramDescription'): IProgramDescription;
    //querySelector(element: 'ProgramDescription'): Element;
}

interface IProgramDescription extends Element {
    querySelector(element: string): Element;
    // optional
    querySelector(element: 'GroupInformationTable'): IGroupInformationTable;
    querySelector(element: 'ServiceInformationTable'): IServiceInformationTable;
    querySelector(element: 'ProgramInformationTable'): IProgramInformationTable;
    querySelector(element: 'ProgramLocationTable'): IProgramLocationTable;
}

interface IGroupInformationTable extends Element {
    querySelectorAll(element: string): NodeList;
    querySelectorAll(element: 'GroupInformation'): IList<IGroupInformationApplication|IGroupInformation_BasicDescription_ApplicationGroup>;
}
interface IServiceInformationTable extends Element {
    querySelectorAll(element: string): NodeList;
    querySelectorAll(element: 'ServiceInformation'): IList<IServiceInformation>;
}
interface IProgramInformationTable extends Element {
    querySelectorAll(element: string): NodeList;
    querySelectorAll(element: 'ProgramInformation'): IList<IProgramInformation>;
}
interface IProgramLocationTable extends Element {
    querySelectorAll(element: string): NodeList;
    querySelectorAll(element: 'ScheduleEvent'): IList<IScheduleEvent>;
    querySelectorAll(element: 'OnDemandProgram'): IList<IOnDemandProgram>;
}

interface IGroupInformation extends Element {
    getAttribute(property: string): string;
    getAttribute(property: 'groupId'): string;
    querySelector(element: string): Element;
    querySelector(element: 'groupType'): Element;
}
interface IGroupInformationApplication extends IGroupInformation {
    getAttribute(property: string): string;
    querySelector(element: string): Element;
    querySelector(element: 'BasicDescription'): IGroupInformation_BasicDescription_Application;
    // optional
    querySelector(element: 'MemberOf'): IGroupInformation_BasicDescription_Application;
}
interface IGroupInformationApplicationGroup extends IGroupInformation {
    getAttribute(property: string): string;
    getAttribute(property: 'ordered'): string;
    getAttribute(property: 'numberOfItems'): string;
    querySelector(element: string): Element;
    querySelector(element: 'BasicDescription'): IGroupInformation_BasicDescription_ApplicationGroup;
}
interface IServiceInformation extends Element {
    getAttribute(property: string): string;
    querySelector(element: string): Element;
    getAttribute(property: 'serviceId'): string;
    querySelector(element: 'Name'): Element;
    querySelector(element: 'ServiceURL'): Element;
    // optional
    querySelector(element: 'Owner'): Element;
    querySelector(element: 'serviceGenre'): Element;
    querySelector(element: 'RelatedMaterial'): Element;
}
interface IProgramInformation extends Element {
    getAttribute(property: string): string;
    querySelector(element: string): Element;
    getAttribute(property: 'programId'): string;
    querySelector(element: 'BasicDescription'): IProgramInformation_BasicDescription;
}
interface IScheduleEvent extends Element {
    querySelector(element: string): Element;
    querySelector(element: 'Program'): Element;
    querySelector(element: 'ProgramURL'): Element;
    // optional
    querySelector(element: 'InstanceDescription'): IScheduleEvent_InstanceDescription_AVAttributes;
    querySelector(element: 'PublishedStartTime'): Element;
    querySelector(element: 'PublishedDuration'): Element;
    querySelector(element: 'FirstShowing'): Element;
}
interface IOnDemandProgram extends Element {
    querySelector(element: string): Element;
    getAttribute(property: string): string;
    getAttribute(property: 'serviceIdRef'): string;
    querySelector(element: 'Program'): Element;
    querySelector(element: 'ProgramURL'): Element;
    querySelector(element: 'AuxiliaryURL'): Element;
    querySelector(element: 'InstanceDescription'): IOnDemandProgram_InstanceDescription_AVAttributes;
    querySelector(element: 'StartOfAvailability'): Element;
    querySelector(element: 'EndOfAvailability'): Element;
    querySelector(element: 'PublishedDuration'): Element;
    querySelector(element: 'Free'): Element;
    querySelector(element: 'DeliveryMode'): Element;
}

interface IProgramInformation_BasicDescription extends Element {
    querySelector(element: string): Element;
    querySelector(element: 'Title'): Element;
    querySelector(element: 'Sysnopsis'): Element;
    // optional
    querySelector(element: 'Genre'): Element;
    querySelector(element: 'ParentalGuidance'): Element;
    querySelector(element: 'CreditsList'): Element; // not supplied with schedules api response;
    querySelector(element: 'RelatedMaterial'): IRelatedMaterial;
}
interface IGroupInformation_BasicDescription_Application extends Element {
    querySelector(element: string): Element;
    querySelector(element: 'Title'): Element;
    querySelector(element: 'Sysnopsis'): Element;
    // optional
    querySelector(element: 'RelatedMaterial'): IRelatedMaterial;
}
interface IGroupInformation_BasicDescription_ApplicationGroup extends Element {
    querySelector(element: string): Element;
    querySelector(element: 'Title'): Element;
}
interface IScheduleEvent_InstanceDescription_AVAttributes extends Element {
    querySelector(element: string): Element;
    // optional
    querySelector(element: 'AudioAttributes'): Element;
    querySelector(element: 'VideoAttributes'): Element;
    querySelector(element: 'CaptioningAttributes'): Element;
}
interface IOnDemandProgram_InstanceDescription_AVAttributes extends Element {
    querySelector(element: string): Element;
    querySelector(element: 'VideoAttributes'): Element;
    // optional
    querySelector(element: 'AudioAttributes'): Element;
    querySelector(element: 'CaptioningAttributes'): Element;
}
interface IAudioAttributes extends Element {
    querySelector(element: string): Element;
    // optional
    querySelector(element: 'MixType'): Element;
    querySelector(element: 'AudioLanguage'): Element;
}
interface IVideoAttributes extends Element {
    querySelector(element: string): Element;
    // optional
    querySelector(element: 'HorizontalSize'): Element;
    querySelector(element: 'VerticalSize'): Element;
    querySelector(element: 'AspectRratio'): Element;
}
interface IRelatedMaterial extends Element {
    querySelector(element: string): Element;
    querySelector(element: 'HowRelated'): Element;
    querySelector(element: 'MediaLocator'): Element;
    // optional
    querySelector(element: 'Promotional_Text'): Element;
}
// ~p68 v0.98
