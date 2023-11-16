project(xcore CXX ASM_NASM)
add_library(xcore SHARED)
set_source_files_properties(malloc.asm stuff.asm PROPERTIES LANGUAGE ASM_NASM)

# Shared include directories
set(INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/Core/Inc)

# File generation
add_executable(genoff genmem.cpp)
add_executable(gendata gendata.cpp)
target_include_directories(genoff PRIVATE ${INCLUDE_DIRECTORIES})
target_include_directories(gendata PRIVATE ${INCLUDE_DIRECTORIES})

# Remove stale generated files
file(REMOVE ${CMAKE_CURRENT_SOURCE_DIR}/flagdata.cpp)
file(REMOVE ${CMAKE_CURRENT_SOURCE_DIR}/memoff.inc)

add_custom_command(
    OUTPUT memoff.inc
    COMMAND "$<TARGET_FILE:genoff>" > memoff.inc
    DEPENDS genoff
    VERBATIM)

add_custom_command(
    OUTPUT flagdata.cpp
    COMMAND "$<TARGET_FILE:gendata>" > flagdata.cpp
    DEPENDS gendata
    VERBATIM)

set(SOURCE_FILES
    list.cpp
    file.cpp
    find.cpp
    malloc.asm
    winalloc.cpp
    dll.cpp
    error.cpp
    global.cpp
    printf.cpp
    stream.cpp
    string.cpp
    stuff.asm
    win.cpp
    winfile.cpp
    winmem.cpp
    xmisc.cpp
    flagdata.cpp
    memoff.inc
    stdcore.cpp)

source_group("Source Files" FILES ${SOURCE_FILES})
target_sources(xcore PRIVATE ${SOURCE_FILES})

set(SOURCE_FILES
    filex.h
    winalloc.h
    xclass.h
    xcore.h
    xipc.h
    xstream.h
    xstring.h)

source_group("Header Files" FILES ${SOURCE_FILES})
target_sources(xcore PRIVATE ${SOURCE_FILES})

set(SOURCE_FILES
    xcore.def)

source_group("Resource Files" FILES ${SOURCE_FILES})
target_sources(xcore PRIVATE ${SOURCE_FILES})

add_library(xcore_winapp STATIC winapp.cpp xwnd.cpp)
add_library(xcore_console STATIC conapp.cpp)

target_link_options(xcore PRIVATE "/BASE:0x21000000")

target_compile_definitions(xcore PRIVATE $<$<COMPILE_LANGUAGE:C,CXX>:XCORE_LIBRARY>)
target_compile_definitions(xcore_winapp PRIVATE XCORE_PURE)
target_compile_definitions(xcore_console PRIVATE XCORE_PURE)

target_include_directories(xcore PRIVATE $<$<COMPILE_LANGUAGE:C,CXX>:${INCLUDE_DIRECTORIES}>)
target_include_directories(xcore_winapp PRIVATE ${INCLUDE_DIRECTORIES})
target_include_directories(xcore_console PRIVATE ${INCLUDE_DIRECTORIES})
