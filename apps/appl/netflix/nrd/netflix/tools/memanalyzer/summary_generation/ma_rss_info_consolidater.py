#! /usr/bin/python
import re, sys
from operator import itemgetter

# ############################################
# function that parses Smaps Analyzer log
# ###
def maps_entry(line):
    try:
       #m = re.match(r'([ 0-9]+)([ ]+)([0-9A-Za-z]+)([ ]+)([0-9A-Za-z\[\.\:\-\]\;.*_\& \(\)\<\>]+)', line)
       m = re.match(r'([ 0-9]+)([ ]+)([0-9A-Za-z]+)([ ]+)([0-9A-Za-z\[\.\:\-\]\;.*_\& \(\)\<\>\+\/]+)', line)
#    print m.group(5)
       return [int(m.group(1), 10),m.group(3),m.group(5)]
    except:
       return [0, 'invalid', 'invalid']
#    return line.split()

# ############################################
# function that reads the category files
# ###
def category_entry(line):
    category = []
    idx = 0
    tmp = line.split()
    for e in tmp:
        #print e
        category.append([])
        category[idx] = (e.split(','))
        idx = idx + 1
    return category
              
#    m = re.match(r'([0-9A-Za-z_\(\)]+)([ ]+)([a-z,*]+)([ ]+)([0-9A-Za-z\[\.\:\-\]*]+)', line)
#    #print m.group(1), m.group(3), m.group(5)
#    return [m.group(1), m.group(3), m.group(5)]

# ############################################
# function that initializes the report
# ###
def init_report(report, ca_entries):
    index = 0
    for ce in ca_entries:
        if ( len(report) == 0 ):
            report.append([index, 0.0, ce[0][0]])
            ce.append(index)
            index = index + 1
        else:
            found = 0
            for i in report:
                if ( i[2] == ce[0][0] ):
                    ce.append(i[0])
                    found = 1
            if ( found == 0 ):
                report.append([index, 0.0, ce[0][0]])
                ce.append(index)
                index = index + 1

# ############################################
# function that does the match
# ###
def my_match(me, ce):
    # match the category
    for category in ce[1]:
        if ( me[1] == 'total' ):
            return -1
        if ( category == "*" or me[1] == category ):
            for tgt in ce[2]:
                if ( tgt == "*" or tgt in me[2]):
                    return 1
    return 0
    #for i in ce.

# ############################################
# function that prints out the report
# ###
def print_report(report, real_total, enable_exclude):
    total = 0.
    for e in sorted(report, key=itemgetter(2)):
        if ( enable_exclude == 'true' and 'exclude' in e[2] ):
            real_total = real_total - e[1]
        else:
            print '{0:7.2f} {1}'.format(e[1]/1024/1024, e[2])
            # print e[2], e[1]/1024/1024
            total += e[1]
    if ( real_total > 0.0 ):
        print '{0:7.2f} {1}'.format((real_total - total)/1024/1024, 'Diff_bw_smaps_and_mincore')
        print '{0:7.2f} {1}'.format(real_total/1024/1024, 'Total')
    else:
        # missing total log
        # [ HACK ]
        print ' 0.0 Diff_bw_smaps_and_mincore'
        print '{0:7.2f} {1}'.format(total/1024/1024, 'Total')


# ############################################
# function that generates Maps Analyzer report
# ###
def consolidate_ma_report(maresult, category, enable_exclude):
    real_total = 0.0;
    ## Open and read ma result
    ma_file = open(maresult, 'r')
    ma_entries = map(maps_entry, ma_file.readlines())
    ma_file.close()

    ## Open and read ma category file
    ca_file = open(category, 'r')
    ca_entries = map(category_entry, ca_file.readlines())
    ca_file.close()

    report = []
    # initialize report
    init_report(report, ca_entries)

    #print report
    #print ca_entries
    #print ma_entries

    for me in ma_entries:
        #print me[0], me[1], me[2]
        for ce in ca_entries:
            #print ce[0], ce[1], ce[2]
            #print ce[2], me[2]
            match_result = my_match(me, ce)
            if ( match_result == 1):
                report[ce[3]][1] += me[0]
                #print "matched!!"
                break
            elif ( match_result == -1 ): # total
                real_total = me[0]
                break
            #print "not matched!!"

    print_report(report, real_total, enable_exclude)



if __name__ == "__main__":
    if ( len(sys.argv) < 4 ):
        print "usage: ./consolidate_ma_report.pl [maps_analyzer_result.txt] [category_file] [exclude_flag]"
    else:
        consolidate_ma_report(sys.argv[1], sys.argv[2], sys.argv[3])


