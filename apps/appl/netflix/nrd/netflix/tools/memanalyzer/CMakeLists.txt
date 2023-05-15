# (c) 1997-2015 Netflix, Inc.  All content herein is protected by
# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights.  Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.

cmake_minimum_required( VERSION 2.8 )

message(" ")
message("=============================================================")
message("Memory Analyzer")
message("=============================================================")
message(" ")

# copy the scripts over to the build directory
add_custom_target(memanalyzer_scripts ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/rss_info_generation
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/gp_script_generation
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/summary_generation
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/summary_generation/configurations
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/run_memanalyzer.sh ${CMAKE_CURRENT_BINARY_DIR}/run_memanalyzer.sh
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/ma_rss_monitor_driver.sh ${CMAKE_CURRENT_BINARY_DIR}/ma_rss_monitor_driver.sh
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/gp_script_generation/gnuplot_script_example.txt ${CMAKE_CURRENT_BINARY_DIR}/gp_script_generation/gnuplot_script_example.txt
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/gp_script_generation/gnuplot_script_template1.gp ${CMAKE_CURRENT_BINARY_DIR}/gp_script_generation/gnuplot_script_template1.gp
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/gp_script_generation/gnuplot_script_template2.gp ${CMAKE_CURRENT_BINARY_DIR}/gp_script_generation/gnuplot_script_template2.gp
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/gp_script_generation/gp_script_generator.py ${CMAKE_CURRENT_BINARY_DIR}/gp_script_generation/gp_script_generator.py

    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/summary_generation/ma_rss_info_consolidater.py ${CMAKE_CURRENT_BINARY_DIR}/summary_generation/ma_rss_info_consolidater.py
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/summary_generation/ma_rss_summary_generator.py ${CMAKE_CURRENT_BINARY_DIR}/summary_generation/ma_rss_summary_generator.py
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/summary_generation/configurations/nrdp41_conf.txt ${CMAKE_CURRENT_BINARY_DIR}/summary_generation/configurations/nrdp41_conf.txt

    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/rss_info_generation/ma_rss_info_retriever.py ${CMAKE_CURRENT_BINARY_DIR}/rss_info_generation/ma_rss_info_retriever.py
    COMMENT "Copying rss analyzer scripts"
)

#add_dependencies(memanalyzer memanalyzer_scripts)

