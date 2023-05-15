/**
 * Created by bglee@humaxdigital.com on 2014. 4. 1..
 */
interface Date {
    format(mask: string, utc?: boolean): string;
}

declare var dateFormat: {
    (date: any, mask?: string, utc?: string): string;
    set(masks, dayNames,monthNames): void;
}
