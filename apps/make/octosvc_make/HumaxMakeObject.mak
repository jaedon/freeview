#
# Copyright (C) 2006 HUMAX Co., Ltd. All rights reserved.
#

###############################################################
octo.one: oapi papi dapi hapi uapi thapi homapi napi sapi tapi
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/octo.one/svc_make $(PARALLEL_MAKE_OPTION) $(APP_CONFIG)  2>&1 || exit -1;
octo.one-clean: oapi-clean papi-clean dapi-clean hapi-clean uapi-clean thapi-clean homapi-clean napi-clean sapi-clean tapi-clean trapi-clean
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/octo.one/svc_make $(APP_CONFIG) clean 2>&1 || exit -1;
octo.one-distclean: oapi-distclean papi-distclean dapi-distclean hapi-distclean uapi-distclean thapi-distclean homapi-distclean napi-distclean sapi-distclean tapi-distclean trapi-distclean
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/octo.one/svc_make $(APP_CONFIG) distclean 2>&1 || exit -1;

.PHONY: octo.one octo.one-clean, octo.one-distclean

tima: setima tapi
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/tima/svc_make $(PARALLEL_MAKE_OPTION) $(APP_CONFIG)  2>&1 || exit -1;
tima-clean: setima-clean tapi-clean
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/tima/svc_make $(APP_CONFIG) clean 2>&1 || exit -1;
tima-distclean: setima-distclean tapi-distclean
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/tima/svc_make $(APP_CONFIG) distclean 2>&1 || exit -1;

.PHONY: tima tima-clean tima-distclean

setima:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/tima/secure_make $(PARALLEL_MAKE_OPTION) $(APP_CONFIG)  2>&1 || exit -1;
setima-clean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/tima/secure_make $(APP_CONFIG) clean 2>&1 || exit -1;
setima-distclean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/tima/secure_make $(APP_CONFIG) distclean 2>&1 || exit -1;

.PHONY: setima setima-clean setima-distclean

tapi:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/tima/api_make $(PARALLEL_MAKE_OPTION) $(APP_CONFIG)  2>&1 || exit -1;
tapi-clean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/tima/api_make $(APP_CONFIG) clean 2>&1 || exit -1;
tapi-distclean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/tima/api_make $(APP_CONFIG) distclean 2>&1 || exit -1;

.PHONY: tapi tapi-clean tapi-distclean

imma:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/imma/svc_make $(PARALLEL_MAKE_OPTION) $(APP_CONFIG)  2>&1 || exit -1;
imma-clean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/imma/svc_make $(APP_CONFIG) clean 2>&1 || exit -1;
	@$(RM) $(HUMAX_MAKE_DIR)/_gen_log/imma-compile.log
imma-distclean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/imma/svc_make $(APP_CONFIG) distclean 2>&1 || exit -1;

.PHONY: imma imma-clean imma-distclean

senima:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/nima/secure_make $(PARALLEL_MAKE_OPTION) $(APP_CONFIG)  2>&1 || exit -1;
senima-clean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/nima/secure_make $(APP_CONFIG) clean 2>&1 || exit -1;
senima-distclean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/nima/secure_make $(APP_CONFIG) distclean 2>&1 || exit -1;
.PHONY: senima senima-clean senima-distclean

napi:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/nima/api_make $(PARALLEL_MAKE_OPTION) $(APP_CONFIG) $(1) || exit -1;
napi-clean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/nima/api_make $(APP_CONFIG) $(1) clean || exit -1;
napi-distclean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/nima/api_make $(APP_CONFIG) $(1) distclean || exit -1;
.PHONY: napi napi-clean napi-distclean

nima: napi
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/nima/svc_make $(PARALLEL_MAKE_OPTION) $(APP_CONFIG) 2>&1 || exit -1;
nima-clean: napi-clean
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/nima/svc_make $(APP_CONFIG) clean 2>&1 || exit -1;
nima-distclean: napi-distclean
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/nima/svc_make $(APP_CONFIG) distclean 2>&1 || exit -1;
.PHONY: nima nima-clean nima-distclean

ummalib:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/umma/lib_make $(PARALLEL_MAKE_OPTION) $(APP_CONFIG) 2>&1 || exit -1;
ummalib-clean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/umma/lib_make $(APP_CONFIG) clean 2>&1 || exit -1;
	@$(RM) $(HUMAX_MAKE_DIR)/_gen_log/ummalib-compile.log
ummalib-distclean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/umma/lib_make $(APP_CONFIG) distclean 2>&1 || exit -1;
.PHONY: ummalib ummalib-clean ummalib-distclean

umma: seumma uapi
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/umma/svc_make $(PARALLEL_MAKE_OPTION) $(APP_CONFIG)  2>&1 || exit -1;
umma-clean: seumma-clean uapi-clean
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/umma/svc_make $(APP_CONFIG)  clean 2>&1 || exit -1;
umma-distclean: seumma-distclean seumma-distclean
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/umma/svc_make $(APP_CONFIG)  distclean 2>&1 || exit -1;
.PHONY: umma umma-clean umma-distclean

uapi:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/umma/api_make $(PARALLEL_MAKE_OPTION) $(APP_CONFIG) 2>&1 || exit -1;
uapi-clean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/umma/api_make $(APP_CONFIG) clean 2>&1 || exit -1;
uapi-distclean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/umma/api_make $(APP_CONFIG) distclean 2>&1 || exit -1;
.PHONY: uapi uapi-clean uapi-distclean

seumma:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/umma/secure_make $(PARALLEL_MAKE_OPTION) $(APP_CONFIG)  2>&1 || exit -1;
seumma-clean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/umma/secure_make $(APP_CONFIG) clean 2>&1 || exit -1;
seumma-distclean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/umma/secure_make $(APP_CONFIG) distclean 2>&1 || exit -1;

.PHONY: seumma seumma-clean seumma-distclean


thapi:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/thma/api_make $(PARALLEL_MAKE_OPTION) $(APP_CONFIG) 2>&1 || exit -1;
thapi-clean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/thma/api_make $(APP_CONFIG) clean 2>&1 || exit -1;
thapi-distclean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/thma/api_make $(APP_CONFIG) distclean 2>&1 || exit -1;
.PHONY: thapi thapi-clean thapi-distclean

thma:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/thma/svc_make $(PARALLEL_MAKE_OPTION) $(APP_CONFIG) 2>&1 || exit -1;
thma-clean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/thma/svc_make $(APP_CONFIG) clean 2>&1 || exit -1;
	$(call build_thma,clean)
thma-distclean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/umma/svc_make $(APP_CONFIG) distclean 2>&1 || exit -1;
.PHONY: thma thma-clean thma-distclean

obama:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/obama/svc_make $(PARALLEL_MAKE_OPTION) $(APP_CONFIG) 2>&1 || exit -1;
obama-clean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/obama/svc_make $(APP_CONFIG) clean 2>&1 || exit -1;
	@$(RM) $(HUMAX_MAKE_DIR)/_gen_log/obama-compile.log
obama-distclean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/obama/svc_make $(APP_CONFIG) distclean 2>&1 || exit -1;

oapi:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/obama/api_make $(PARALLEL_MAKE_OPTION) $(APP_CONFIG) 2>&1 || exit -1;
oapi-clean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/obama/api_make $(APP_CONFIG) clean 2>&1 || exit -1;
	@$(RM) $(HUMAX_MAKE_DIR)/_gen_log/oapi-compile.log
oapi-distclean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/obama/api_make $(APP_CONFIG) distclean 2>&1 || exit -1;

.PHONY: obama obama-clean obama-distclean oapi oapi-clean oapi-distclean

homma:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/homma/svc_make $(PARALLEL_MAKE_OPTION) $(APP_CONFIG) 2>&1 || exit -1;
homma-clean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/homma/svc_make $(APP_CONFIG) clean 2>&1 || exit -1;
	@$(RM) $(HUMAX_MAKE_DIR)/_gen_log/homma-compile.log
homma-distclean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/homma/svc_make $(APP_CONFIG) distclean 2>&1 || exit -1;

homapi:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/homma/api_make $(PARALLEL_MAKE_OPTION) $(APP_CONFIG) 2>&1 || exit -1;
homapi-clean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/homma/api_make $(APP_CONFIG) clean 2>&1 || exit -1;
	@$(RM) $(HUMAX_MAKE_DIR)/_gen_log/homapi-compile.log
homapi-distclean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/homma/api_make $(APP_CONFIG) distclean 2>&1 || exit -1;

.PHONY: homma homma-clean homma-distclean homapi homapi-clean homapi-distclean

pama: sepama papi
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/pama/svc_make $(PARALLEL_MAKE_OPTION) $(APP_CONFIG)  2>&1 || exit -1;
pama-clean: sepama-clean papi-clean
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/pama/svc_make $(APP_CONFIG) clean 2>&1 || exit -1;
	@$(RM) $(HUMAX_MAKE_DIR)/_gen_log/pama-compile.log
pama-distclean: sepama-distclean papi-distclean
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/pama/svc_make $(APP_CONFIG) distclean 2>&1 || exit -1;

.PHONY: pama pama-clean pama-distclean

sepama:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/pama/secure_make $(PARALLEL_MAKE_OPTION) $(APP_CONFIG) 2>&1 || exit -1;
sepama-clean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/pama/secure_make $(APP_CONFIG) clean 2>&1 || exit -1;
sepama-distclean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/pama/secure_make $(APP_CONFIG) distclean 2>&1 || exit -1;

.PHONY: sepama sepama-clean sepama-distclean

papi:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/pama/api_make $(PARALLEL_MAKE_OPTION) $(APP_CONFIG) 2>&1 || exit -1;
papi-clean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/pama/api_make $(APP_CONFIG) clean $(1) 2>&1 || exit -1;
	@$(RM) $(HUMAX_MAKE_DIR)/_gen_log/papi-compile.log
papi-distclean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/pama/api_make $(APP_CONFIG) distclean 2>&1 || exit -1;

.PHONY: papi papi-clean papi-distclean

dama:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/dama/svc_make $(PARALLEL_MAKE_OPTION) $(APP_CONFIG) 2>&1 || exit -1;
dama-clean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/dama/svc_make $(APP_CONFIG) clean 2>&1 || exit -1;
	@$(RM) $(HUMAX_MAKE_DIR)/_gen_log/dama-compile.log
dama-distclean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/dama/svc_make $(APP_CONFIG) distclean 2>&1 || exit -1;

dapi:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/dama/api_make $(PARALLEL_MAKE_OPTION) $(APP_CONFIG) 2>&1 || exit -1;
dapi-clean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/dama/api_make $(APP_CONFIG) clean 2>&1 || exit -1;
	@$(RM) $(HUMAX_MAKE_DIR)/_gen_log/dapi-compile.log
dapi-distclean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/dama/api_make $(APP_CONFIG) distclean 2>&1 || exit -1;

.PHONY: dama dama-clean dama-distclean dapi dapi-clean dapi-distclean

hama: hamalauncher hapi
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/hama/svc_make $(PARALLEL_MAKE_OPTION) $(APP_CONFIG) 2>&1 || exit -1;
hama-clean: hamalauncher-clean hapi-clean
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/hama/svc_make $(APP_CONFIG) clean 2>&1 || exit -1;
	@$(RM) $(HUMAX_MAKE_DIR)/_gen_log/hama-compile.log
hama-distclean: hamalauncher-distclean hapi-distclean
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/hama/svc_make $(APP_CONFIG) distclean 2>&1 || exit -1;

hapi:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/hama/api_make $(PARALLEL_MAKE_OPTION) $(APP_CONFIG) 2>&1 || exit -1;
hapi-clean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/hama/api_make $(APP_CONFIG) clean 2>&1 || exit -1;
	@$(RM) $(HUMAX_MAKE_DIR)/_gen_log/hapi-compile.log
hapi-distclean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/hama/api_make $(APP_CONFIG) distclean 2>&1 || exit -1;

.PHONY: hama hama-clean hama-distclean hapi hapi-clean hapi-distclean

hamalauncher: hapi
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/hamalauncher/svc_make $(PARALLEL_MAKE_OPTION) $(APP_CONFIG) 2>&1 || exit -1;
hamalauncher-clean: hapi-clean
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/hamalauncher/svc_make $(APP_CONFIG) clean 2>&1 || exit -1;
hamalauncher-distclean: hapi-distclean
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/hamalauncher/svc_make $(APP_CONFIG) distclean 2>&1 || exit -1;

.PHONY: hamalauncher hamalauncher-clean hamalauncher-distclean

secma:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/secma/svc_make $(PARALLEL_MAKE_OPTION) $(APP_CONFIG) 2>&1 || exit -1;
secma-clean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/secma/svc_make $(APP_CONFIG) clean 2>&1 || exit -1;
	@$(RM) $(HUMAX_MAKE_DIR)/_gen_log/secma-compile.log
secma-distclean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/secma/svc_make $(APP_CONFIG) distclean 2>&1 || exit -1;

secapi:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/secma/api_make $(PARALLEL_MAKE_OPTION) $(APP_CONFIG) 2>&1 || exit -1;
secapi-clean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/secma/api_make $(APP_CONFIG) clean 2>&1 || exit -1;
	@$(RM) $(HUMAX_MAKE_DIR)/_gen_log/secapi-compile.log
secapi-distclean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/secma/api_make $(APP_CONFIG) distclean 2>&1 || exit -1;

.PHONY: secma secma-clean secma-distclean secapi secapi-clean secapi-distclean

comma:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/comma/svc_make $(PARALLEL_MAKE_OPTION) $(APP_CONFIG) 2>&1 || exit -1;
comma-clean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/comma/svc_make $(APP_CONFIG) clean 2>&1 || exit -1;
	@$(RM) $(HUMAX_MAKE_DIR)/_gen_log/comma-compile.log
comma-distclean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/comma/svc_make $(APP_CONFIG) distclean 2>&1 || exit -1;

.PHONY: comma comma-clean comma-distclean


samalib:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/sama/lib_make $(PARALLEL_MAKE_OPTION) $(APP_CONFIG) 2>&1 || exit -1;
samalib-clean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/sama/lib_make $(APP_CONFIG) clean 2>&1 || exit -1;
	@$(RM) $(HUMAX_MAKE_DIR)/_gen_log/samalib-compile.log
samalib-distclean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/sama/lib_make $(APP_CONFIG) distclean 2>&1 || exit -1;

sama:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/sama/svc_make $(PARALLEL_MAKE_OPTION) $(APP_CONFIG) 2>&1 || exit -1;
sama-clean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/sama/svc_make $(APP_CONFIG) clean 2>&1 || exit -1;
	@$(RM) $(HUMAX_MAKE_DIR)/_gen_log/sama-compile.log
sama-distclean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/sama/svc_make $(APP_CONFIG) distclean 2>&1 || exit -1;

sapi:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/sama/api_make $(PARALLEL_MAKE_OPTION) $(APP_CONFIG) 2>&1 || exit -1;
sapi-clean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/sama/api_make $(APP_CONFIG) clean 2>&1 || exit -1;
	@$(RM) $(HUMAX_MAKE_DIR)/_gen_log/sapi-compile.log
sapi-distclean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/sama/api_make $(APP_CONFIG) distclean 2>&1 || exit -1;

.PHONY: sama sama-clean sama-distclean sapi sapi-clean sapi-distclean samalib samalib-clean samalib-distclean

trauma:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/trauma/svc_make $(PARALLEL_MAKE_OPTION) $(APP_CONFIG) 2>&1 || exit -1;
trauma-clean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/trauma/svc_make $(APP_CONFIG) clean 2>&1 || exit -1;
	@$(RM) $(HUMAX_MAKE_DIR)/_gen_log/trauma-compile.log
trauma-distclean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/trauma/svc_make $(APP_CONFIG) distclean 2>&1 || exit -1;

trapi:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/trauma/api_make $(PARALLEL_MAKE_OPTION) $(APP_CONFIG) 2>&1 || exit -1;
trapi-clean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/trauma/api_make $(APP_CONFIG) clean 2>&1 || exit -1;
	@$(RM) $(HUMAX_MAKE_DIR)/_gen_log/trapi-compile.log
trapi-distclean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/trauma/api_make $(APP_CONFIG) distclean 2>&1 || exit -1;

.PHONY: trauma trauma-clean trauma-distclean trapi trapi-clean trapi-distclean


rema:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/rema/svc_make $(PARALLEL_MAKE_OPTION) $(APP_CONFIG) 2>&1 || exit -1;
rema-clean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/rema/svc_make $(APP_CONFIG) clean 2>&1 || exit -1;
	@$(RM) $(HUMAX_MAKE_DIR)/_gen_log/rema-compile.log
rema-distclean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/rema/svc_make $(APP_CONFIG) distclean 2>&1 || exit -1;

remapi:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/rema/api_make $(PARALLEL_MAKE_OPTION) $(APP_CONFIG) 2>&1 || exit -1;
remapi-clean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/rema/api_make $(APP_CONFIG) clean 2>&1 || exit -1;
	@$(RM) $(HUMAX_MAKE_DIR)/_gen_log/remapi-compile.log
remapi-distclean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/rema/api_make $(APP_CONFIG) distclean 2>&1 || exit -1;

unit_rema:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/rema/unit_svc_make $(PARALLEL_MAKE_OPTION) $(APP_CONFIG) 2>&1 || exit -1;
unit_rema-clean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/rema/unit_svc_make $(APP_CONFIG) clean 2>&1 || exit -1;
	@$(RM) $(HUMAX_MAKE_DIR)/_gen_log/unit_rema-compile.log
unit_rema-distclean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/rema/unit_svc_make $(APP_CONFIG) distclean 2>&1 || exit -1;

.PHONY: rema rema-clean rema-distclean remapi remapi-clean remapi-distclean unit_rema unit_rema-clean unit_rema-distclean

lapi:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/luna/svc_make $(PARALLEL_MAKE_OPTION) $(APP_CONFIG)  2>&1 || exit -1;
lapi-clean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/luna/svc_make $(APP_CONFIG) clean 2>&1 || exit -1;
lapi-distclean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/luna/svc_make $(APP_CONFIG) distclean 2>&1 || exit -1;

lunatest:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/luna/svc_make $(PARALLEL_MAKE_OPTION) $(APP_CONFIG)  2>&1 || exit -1;
lunatest-clean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/luna/svc_make $(APP_CONFIG) clean 2>&1 || exit -1;
lunatest-distclean:
	@$(MAKE) -C $(OCTOSVC_MAKE_DIR)/luna/svc_make $(APP_CONFIG) distclean 2>&1 || exit -1;

.PHONY: lapi lapi-clean lapi-distclean lunatest lunatest-clean lunatest-distclean