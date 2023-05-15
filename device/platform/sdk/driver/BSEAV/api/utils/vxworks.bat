@echo off
rem The Make processor under VxWorks 5.5 doesn't support a for loop to process the different apps.
rem Therefore, we have to use this (BATCH script) to do the looping.
rem This is equivalent to:
rem   @for tool in $(TOOLS); do \
rem      echo "=== Run $@ for $$tool"; \
rem      $(MAKE) -C $$tool $@; \
rem   done
rem

rem fist parameter passed is the make option (clean, veryclean, etc.)
set option=%1
shift

:loop
if "%1" == "" goto done
  echo === Run %option% for %1
  @make -C %1 %option%
shift
goto loop

:done
