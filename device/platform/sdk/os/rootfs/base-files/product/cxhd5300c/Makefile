curdir=${shell pwd}

rootfs-product:
ifeq ($(DEBUG_RELEASE_ENV),release)
	@echo "copy files for IPTABLES"
	@if [ -d "$(curdir)/iptables" ] ; then \
		cp -fa $(curdir)/iptables/* $(prefix)/; \
	fi;
endif

