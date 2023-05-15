
# set color schemes
#colors="#5555aa #55aa55 #aa5555 #aaaa55 #55aaaa #aa55aa #3333cc #33cc33 #cc3333 #cccc33 #33cccc #cc33cc #1111ff #11ff11 #ff1111 #ffff11 #11ffff #ff11ff #cccccc"
#colors="#1111ff #11ff11 #ff1111 #ffff11 #11ffff #ff11ff #3333cc #33cc33 #cc3333 #cccc33 #33cccc #cc33cc #5555aa #55aa55 #aa5555 #aaaa55 #55aaaa #aa55aa #cccccc"
#colors="#5555aa #55aa55 #aa5555 #aaaa55 #55aaaa #aa55aa #3333cc #33cc33 #cc3333 #cccc33 #33cccc #cc33cc #cccccc"
#colors="#805080 #409020 #ff2020 #ff6010 #2020ff #ffff30 #703020 #e01070 #20ffc0 #cccccc"

#colors="#945C94 #60AD16 #E5263B #FF8030 #5050FF #519ACA #FFFF60 #E05090 #97FFA7 #CCCCCC"
#colors="#4F0031 #B51532 #FF6700 #F6D002 #6C9C00 #4471FF #42FFB3 #E853B8 #FFFF60 #33FF33 #FF3333 #3333FF #33FFFF #FF33FF #FFFF33 #aaaaaa #cccccc"
colors="#4F0031 #B51532 #FF6700 #F6D002 #6C9C00 #4471FF #42FFB3 #E853B8 #FFFF60 #33cc33 #cc3333 #3333cc #33cccc #cc33cc  #cccc33 #cccccc #999999"

# set terminal
set terminal wxt noraise title 'monitored smaps analysis on ' . finalSummary

# set legends
set key outside;

# set axis and grid
set xdata time
set timefmt "%Y%m%d-%H%M%S"
set format x "%H:%M:%S"
set ytics 10
set y2tics 10
set grid front xtics ytics
set ylabel "Resident Set Size (MB)"

