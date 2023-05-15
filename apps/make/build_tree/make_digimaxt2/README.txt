
[ -- Build tree making sequence -- ]
 * at first, you have to execute "make buildtree" in apps/make/.
 * if you executed "make buildtree", you can see several folders in "apps/make/build_tree/make_[model name]/"
 * finally, you just execute "make world" in your model's build tree folder.
   you can install nfs rootfs and vmlinuz for tftp. and link your specific binaries..

 * if you want to modify your tftpboot and nfsroot folder path, you can just modify two symboles in 
   "apps/make/build_tree/make_[model name]/Makefile".
	- TARGET_TFT_FOLDER
        - TARGET_NFSROOT_FOLDER
 * if you want to modify your model(project) name.
   you can just change TARGET_MODEL_NAME in "apps/make/build_tree/make_[model name]/Makefile".

 * if you have to modify your link option,
   you can just add  various link option at LIB_PROJECT_SPECIFIC symbol in "apps/make/build_tree/make_[model name]/Makefile.common".

[ -- Help -- ]
 - For extract your project's link option, for HUMAX buildtree release engineer.
  - "apps/make/humaxtv_make/scripts/HumaxPlatform.mak" file have link target.
  - if you just change "$(HUMAX_CC)" to $(HUMAX_CROSS)gcc. you can see your link options...