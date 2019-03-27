cmake_minimum_required(VERSION 3.5.1)
project(CrossGuid
    LANGUAGES CXX
    VERSION 0.3)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

set(MASTER_PROJECT OFF)
if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    set(MASTER_PROJECT ON)
endif()

option(CROSSGUID_TESTS "Build tests" ${MASTER_PROJECT})
option(CROSSGUID_EXAMPLES "Build examples" ${MASTER_PROJECT})
option(CROSSGUID_INSTALL "Generate install target" ${MASTER_PROJECT})

option(CROSSGUID_WERROR "Halt compilation in case of a warning" OFF)

set(CROSSGUID_STANDARDESE_TOOL "" CACHE PATH "Path to standardese binary")
set(CROSSGUID_STANDARDESE_CONFIG "" CACHE PATH "Path to standardese CMake config (standardese-config.cmake)")

function(get_warning_flags flags)
    set(${flags}
	$<$<CXX_COMPILER_ID:Clang>:
	    -Weverything
	    -Wpedantic -pedantic-errors
	    -Wno-documentation-unknown-command
	    -Wno-c++98-compat
	    -Wno-c++98-compat-pedantic
	    -Wno-c++98-compat-bind-to-temporary-copy
	    -Wno-c++98-compat-local-type-template-args>
	$<$<CXX_COMPILER_ID:GNU>:
	    -Wall -Wextra -Wpedantic
	    -pedantic-errors
	    -Wconversion -Wsign-conversion
	    -Wold-style-cast -Wfloat-equal
	    -Wlogical-op -Wundef
	    -Wredundant-decls -Wshadow
	    -Wwrite-strings
	    -Wpointer-arith -Wcast-qual
	    -Wformat=2 -Wswitch-default
	    -Wmissing-include-dirs -Wcast-align
	    -Wswitch-enum -Wnon-virtual-dtor
	    -Wctor-dtor-privacy -Wdisabled-optimization
	    -Winvalid-pch -Wnoexcept
	    -Wmissing-declarations -Woverloaded-virtual
	    $<$<NOT:$<VERSION_LESS:CXX_COMPILER_VERSION,5.0>>:
	    -Wdouble-promotion -Wtrampolines
	    -Wzero-as-null-pointer-constant
	    -Wuseless-cast -Wvector-operation-performance>
	    $<$<NOT:$<VERSION_LESS:CXX_COMPILER_VERSION,6.0>>:
	    -Wshift-overflow=2 -Wnull-dereference
	    -Wduplicated-cond>
	    $<$<NOT:$<VERSION_LESS:CXX_COMPILER_VERSION,7.0>>:
	    -Walloc-zero -Walloca
	    -Wduplicated-branches>
	    $<$<NOT:$<VERSION_LESS:CXX_COMPILER_VERSION,8.0>>:
	    -Wcast-align=strict>
	    >
	$<$<CXX_COMPILER_ID:MSVC>:
	    /W4
	    /D_SCN_SECURE_NO_WARNINGS
	    /D_CRT_SECURE_NO_WARNINGS>

            PARENT_SCOPE)
endfunction()
function(get_werror_flags flags)
    set(${flags}
        $<$<CXX_COMPILER_ID:Clang>:
            -Werror>
        $<$<CXX_COMPILER_ID:GNU>:
            -Werror>
        $<$<CXX_COMPILER_ID:MSVC>:
            /WX>

            PARENT_SCOPE)
endfunction()

function (set_private_flags target)
    get_warning_flags(warning_flags)
    target_compile_options(${target} PRIVATE ${warning_flags})
    if (XG_WERROR)
        get_werror_flags(werror_flags)
        target_compile_options(${target} PRIVATE ${werror_flags})
    endif ()
endfunction ()

add_library(crossguid
    src/guid.cpp
    include/crossguid/guid.hpp)
add_library(crossguid::crossguid ALIAS crossguid)
target_include_directories(crossguid PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)
target_compile_features(crossguid PUBLIC cxx_std_11)
set_private_flags(crossguid)

if(WIN32)
    target_compile_definitions(crossguid PRIVATE GUID_WINDOWS)
elseif(APPLE)
    find_library(CFLIB CoreFoundation)
    target_link_libraries(crossguid ${CFLIB})
    target_compile_definitions(crossguid PRIVATE GUID_CFUUID)
else()
    find_package(Libuuid REQUIRED)
    if (NOT LIBUUID_FOUND)
        message(FATAL_ERROR
            "You might need to run 'sudo apt-get install uuid-dev' or similar")
    endif()
    target_include_directories(crossguid PRIVATE ${LIBUUID_INCLUDE_DIR})
    target_link_libraries(crossguid ${LIBUUID_LIBRARY})
    target_compile_definitions(crossguid PRIVATE GUID_LIBUUID)
endif()

set_target_properties(crossguid PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR})

if(CROSSGUID_INSTALL)
    include(GNUInstallDirs)

    set(CROSSGUID_INC_INSTALL_DIR "${CMAKE_INSTALL_INCLUDEDIR}")
    set(CROSSGUID_RUNTIME_INSTALL_DIR "${CMAKE_INSTALL_BINDIR}")
    set(CROSSGUID_LIBRARY_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}")
    set(CROSSGUID_ARCHIVE_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}")
    set(CROSSGUID_FRAMEWORK_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}")

    set(CROSSGUID_CMAKE_CONFIG_INSTALL_DIR "${CMAKE_INSTALL_DATADIR}/crossguid/cmake")
    set(CROSSGUID_ADDITIONAL_FILES_INSTALL_DIR "${CMAKE_INSTALL_DATADIR}/crossguid")

    # Install target
    install(TARGETS crossguid EXPORT crossguidTargets
	    RUNTIME       DESTINATION ${CROSSGUID_RUNTIME_INSTALL_DIR}
	    LIBRARY       DESTINATION ${CROSSGUID_LIBRARY_INSTALL_DIR}
	    ARCHIVE       DESTINATION ${CROSSGUID_ARCHIVE_INSTALL_DIR}
	    FRAMEWORK     DESTINATION ${CROSSGUID_FRAMEWORK_INSTALL_DIR})

    # Install headers
    install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/"
	    DESTINATION ${CROSSGUID_INC_INSTALL_DIR})

    # Make cmake config files for all targets
    install(EXPORT  crossguidTargets
	    DESTINATION ${CROSSGUID_CMAKE_CONFIG_INSTALL_DIR}
	    FILE crossguid-config.cmake)

    # Install readme and license
    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE" "${CMAKE_CURRENT_SOURCE_DIR}/README.md"
	    DESTINATION ${CROSSGUID_ADDITIONAL_FILES_INSTALL_DIR})
endif()

if (CROSSGUID_TESTS)
    include(CTest)
    enable_testing()
    add_subdirectory(test)
endif()

if (CROSSGUID_EXAMPLES)
    add_subdirectory(examples)
endif()
