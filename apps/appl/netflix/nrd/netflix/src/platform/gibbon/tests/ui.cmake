add_test(NAME gibbon_ui_chaos COMMAND ${GIBBON_TEST_HARNESS} ${CMAKE_CURRENT_BINARY_DIR}/netflix -0 http://ppd.builds.test.netflix.net/view/NRDJS/job/PPD-NRDJS-master/lastSuccessfulBuild/artifact/origin/master/boot.max.js ${GIBBON_TEST_HARNESS_FLAGS})
set_tests_properties(gibbon_ui_chaos PROPERTIES LABELS "ui;ui_chaos")
