Introduction of interop
Redmine : http://svn:3000/issues/85100

1. Author: tjjang

2. Description: This place is made to support outer communication with other modules in octo

3. How to enable this?
 - Please open the octo/service/homma/main/include/homma_interop.h
 - Comment out "#define USE_APK" to enable it
 - make homma

4. Known issue
 - apk.h is still necessary because there're some definitions that affect compile
  > apkMetaSvc_CHLOGO
  > apkMetaSvc_DUMP_ONCE
  > apkMetaSvc_DUMP_ON_UPDATE
  > APKMETASVC_USE_VCLONE

Thanks
