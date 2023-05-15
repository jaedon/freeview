#####################################################
# Private application link option
#####################################################

# humax shared lib
APP-LINK-y += -lmemory-null
APP-LINK-y += -lhlib -lhapi -lvkernel -llunawrapapi

#############################################################################

# standard shared lib
APP-LINK-y								+= -pthread -lssp

# dfb & nexus lib
APP-LINK-y								+= -lnexus_client
APP-LINK-y								+= -ldirectfb -lfusion -ldirect -ldl -lsawman -lstdc++ -lz
######################################################################################
#
#        Package Include Path
#
######################################################################################
