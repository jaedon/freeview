#
# Top level makefile for the Nagra NOCS3 driver libraries
#

OS_LIBS = cert csd sec nocscommon

all:
	@$(foreach lib, $(OS_LIBS), echo "[Build... $(lib) (nocs3)]" && $(MAKE) -s -C os/$(lib) && ) true

clean:
	@$(foreach lib, $(OS_LIBS), echo "[Clean... $(lib) (nocs3)]" && $(MAKE) -s -C os/$(lib) clean && ) true
