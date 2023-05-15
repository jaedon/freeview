# (c) 1997-2015 Netflix, Inc.  All content herein is protected by
# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights.  Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.

cmake_minimum_required(VERSION 2.8)
if("${CMAKE_VERSION}" VERSION_GREATER 2.8.12.2)
    cmake_policy(SET CMP0045 OLD)
    cmake_policy(SET CMP0046 OLD)
endif()

include(CMakeParseArguments)

# Resource system
# ------------------------------------------------------------------------------

# Adds resource to the list of resources to process
macro(ADD_RESOURCE target resource)
    list(APPEND RESOURCES_${target} ${resource})
endmacro()

macro(ADD_RESOURCE_DEPENDENCIES target dependency)
    set_property(GLOBAL APPEND PROPERTY RESOURCES_DEPENDENCIES_${target} ${dependency})
endmacro()

macro(COPY_JAVASCRIPT_LIBRARY target)
    cmake_parse_arguments("CGR" "" "LIBRARY;DESTINATION" "" ${ARGN})

    if(NOT CGR_LIBRARY)
        message(FATAL_ERROR "copy_javascript_library: missing LIBRARY parameter")
    endif()

    if(NOT CGR_DESTINATION)
        message(FATAL_ERROR "copy_javascript_library: missing DESTINATION parameter")
    endif()

    get_property(${CGR_LIBRARY}_sources GLOBAL PROPERTY ${CGR_LIBRARY}_sources)
    get_property(${CGR_LIBRARY}_output_debug GLOBAL PROPERTY ${CGR_LIBRARY}_output_debug)
    get_property(${CGR_LIBRARY}_output_production GLOBAL PROPERTY ${CGR_LIBRARY}_output_production)

    if(${CGR_LIBRARY}_sources STREQUAL "")
        message(FATAL_ERROR "copy_javascript_library: the specified library ${CGR_LIBRARY} has no source files")
    endif()

    if(${CGR_LIBRARY}_output_debug STREQUAL "")
        message(FATAL_ERROR "copy_javascript_library: the specified library ${CGR_LIBRARY} has no debug output")
    endif()

    if(${CGR_LIBRARY}_output_production STREQUAL "")
        message(FATAL_ERROR "copy_javascript_library: the specified library ${CGR_LIBRARY} has no production output")
    endif()

    if(CMAKE_BUILD_TYPE MATCHES "debug")
        set(CGR_SOURCE ${${CGR_LIBRARY}_output_debug})
    else()
        set(CGR_SOURCE ${${CGR_LIBRARY}_output_production})
    endif()

    copy_resource(${target} "${CGR_SOURCE}" "${CGR_DESTINATION}/${CGR_LIBRARY}" DEPENDS ${${CGR_LIBRARY}_sources})

    add_resource_dependencies(${target} "${CGR_LIBRARY}_library")
    add_resource_dependencies(${target} "${CGR_DESTINATION}/${CGR_LIBRARY}")
endmacro()


# Process a list of resources one by one
macro(COPY_RESOURCE target in_source in_destination)
    cmake_parse_arguments("CR" "NON_COMPILABLE;NON_RECURSIVE;DEBUG" "" "DEPENDS" ${ARGN})

    set(recurse TRUE)
    set(type COMPILABLE) # Default values to pass to compile_resource

    # Kept this way for backward compatiblity
    if(CR_NON_COMPILABLE)
        list(REMOVE_ITEM type COMPILABLE)
    endif()

    if(CR_UNPARSED_ARGUMENTS)
        list(APPEND type ${CR_UNPARSED_ARGUMENTS})
    endif()

    if(CR_NON_RECURSIVE)
        set(recurse FALSE)
    endif()

    if(CR_DEBUG)
        list(APPEND type "DEBUG")
    endif()

    if(DEFINED CR_DEPENDS)
        list(APPEND type "DEPENDS")
        list(APPEND type ${CR_DEPENDS})
    endif()

    get_target_property(target_directory ${target} RUNTIME_OUTPUT_DIRECTORY)
    if(recurse)
        file(GLOB_RECURSE sources ${in_source})
    else()
        file(GLOB sources ${in_source})
    endif()
    if(NOT sources AND NOT ${in_source} MATCHES "\\*")
         set(sources ${in_source})
    endif()

    foreach(source ${sources})
        set(destination "${in_destination}")
        if(destination MATCHES "/$")
            get_filename_component(relative_source "${in_source}" PATH)
            file(RELATIVE_PATH relative_source ${relative_source} ${source})
            set(destination "${destination}${relative_source}")
        endif()
        if(NOT IS_DIRECTORY ${source})
            compile_resource("${target}" "${source}" "${destination}" ${type})
        endif()
    endforeach()
endmacro()

# Copies or compiles a resource, expects CMAKE_RESOURCE_COMPILER and CMAKE_RESOURCE_COMPILER_FLAGS to be set
macro(COMPILE_RESOURCE target source destination)
    cmake_parse_arguments("COR" "COMPILABLE;DEBUG" "" "DEPENDS" ${ARGN})
    #message("Resource(${target}): ${source}->${destination} [${ARGN}]")

    if(COR_DEBUG)
        set(prefix "DEBUG")
    else()
        set(prefix "COMMON")
    endif()

    add_resource(${target} ${source})

    get_filename_component(destination_path "${destination}" PATH)
    if(COR_COMPILABLE AND BUILD_COMPILE_RESOURCES) # Compile
        file(RELATIVE_PATH symbol ${APP_DATA_DIR} ${destination})
        set(pretty_path ${symbol})
        string(REPLACE "/" "_" symbol "${symbol}")
        string(REPLACE "." "_" symbol "${symbol}")
        string(REPLACE "-" "_" symbol "${symbol}")
        string(REPLACE " " "_" symbol "${symbol}")

        set(destination_o "${NRDP_BINARY_ROOT_DIR}/src/generated/${target}/resources/${symbol}${CMAKE_C_OUTPUT_EXTENSION}")
        get_filename_component(destination_path "${destination_o}" PATH)
        get_filename_component(source_path "${source}" PATH)

        set(resource_compiler_flags "${CMAKE_RESOURCE_COMPILER_FLAGS}")
        string(REPLACE "<SOURCE>" "${symbol}" resource_compiler_flags "${resource_compiler_flags}")
        string(REPLACE "<ABS_SOURCE>" "${destination_path}/${symbol}" resource_compiler_flags "${resource_compiler_flags}")
        string(REPLACE "<DESTINATION>" "${destination_o}" resource_compiler_flags "${resource_compiler_flags}")
        string(REPLACE "<DESTINATION_O>" "${symbol}${CMAKE_C_OUTPUT_EXTENSION}" resource_compiler_flags "${resource_compiler_flags}")

        file(MAKE_DIRECTORY ${destination_path})

        add_custom_command(
            OUTPUT ${destination_o}
            DEPENDS "${source}" ${COR_DEPENDS}
            COMMAND ${CMAKE_COMMAND} -E copy "${source}" "${destination_path}/${symbol}"
            COMMAND ${CMAKE_RESOURCE_COMPILER} ${resource_compiler_flags}
            COMMAND ${CMAKE_COMMAND} -E remove -f "${destination_path}/${symbol}"
            COMMENT "Generating resource for ${pretty_path} (${symbol}${CMAKE_C_OUTPUT_EXTENSION})"
            WORKING_DIRECTORY ${destination_path}
            VERBATIM
        )
        add_resource(${target} ${destination_o} ${type})

        # Generate C++ code
        append(CPP_RESOURCES_${prefix}_EXTERN "    // ${pretty_path}\n")
        append(CPP_RESOURCES_${prefix}_EXTERN "    extern const char _binary_${symbol}_start[];\n")
        append(CPP_RESOURCES_${prefix}_EXTERN "    extern const char _binary_${symbol}_end[];\n")
        append(CPP_RESOURCES_${prefix}_EXTERN "    extern unsigned int _binary_${symbol}_size;\n\n")

        append(CPP_RESOURCES_${prefix}_LIST "ADD_RESOURCE(\"${prefix}\",\"${pretty_path}\",${symbol})\n")
    elseif(NOT debug OR ${CMAKE_BUILD_TYPE} STREQUAL "debug" ) #Copy
            add_custom_command(OUTPUT "${destination}"
                COMMENT "Copying resource [${target}]: ${source}"
                DEPENDS ${source} ${COR_DEPENDS}
                COMMAND ${CMAKE_COMMAND} -E make_directory "${destination_path}"
                COMMAND ${CMAKE_COMMAND} -E copy "${source}" "${destination}"
            )
            add_resource(${target} ${destination} ${type})
    endif()
endmacro()


# To be called when all the resources have been copied, adds the target to the make file and creates and
# links the resource library if building with compiled resources
macro(CREATE_RESOURCES_LIBRARY target)

    get_property(target_dependencies GLOBAL PROPERTY RESOURCES_DEPENDENCIES_${target})

        get_target_property(target_sources ${target} SOURCES)

        if(target_sources)
            add_custom_target(${target}_resources ALL
                DEPENDS ${RESOURCES_${target}}
                COMMENT "Processed data directory for ${target}"
            )
        endif()

    if(BUILD_COMPILE_RESOURCES)
        configure_file_if_different(${NRDP_ROOT_DIR}/src/nrd/Core/ResourceList.cpp.in ${NRDP_BINARY_ROOT_DIR}/src/generated/${target}/ResourceList.cpp)

        foreach(resource ${RESOURCES_${target}})
            if(BUILD_NRDSYSTEM MATCHES "linux")
                if(NOT "${resource}" MATCHES ".*[/.]o$")
                    list(REMOVE_ITEM RESOURCES_${target} ${resource})
                endif()
            endif()

        endforeach()

            set_source_files_properties(${RESOURCES_${target}} PROPERTIES EXTERNAL_OBJECT true GENERATED true)
        set(library_name "nrdresources_${target}")
        add_library(${library_name} STATIC ${NRDP_BINARY_ROOT_DIR}/src/generated/${target}/ResourceList.cpp ${RESOURCES_${target}})
        set_target_properties(${library_name} PROPERTIES LINKER_LANGUAGE C)


        if(target_sources)
            target_link_libraries(${target} ${library_name})
        endif()

        if(target_dependencies)
            foreach(dependency ${target_dependencies})
                add_dependencies(nrdresources_${target} ${dependency})
            endforeach()
        endif()
    else()
        if(target_dependencies)
            add_dependencies(${target} ${target_dependencies})
        endif()
    endif()
endmacro()
