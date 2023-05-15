curdir=${shell pwd}

rootfs-product:
ifeq ($(DEBUG_RELEASE_ENV),release)
	@echo "copy files for release"
	@if [ -d "$(curdir)/release" ] ; then \
		cp -fa $(curdir)/release/* $(prefix)/; \
	fi;
	@rm -f $(prefix)/etc/shadow;
	@rm -f $(prefix)/etc/fstab;
endif
