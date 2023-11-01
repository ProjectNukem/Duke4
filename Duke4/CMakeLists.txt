cmake_minimum_required(VERSION 3.13)

# Environment
cmake_path(CONVERT "$ENV{MASM_BIN}" TO_CMAKE_PATH_LIST MASM_BIN)
set(MASM_DIR ${MASM_BIN} CACHE PATH "Path to directory containing ML.EXE")

# Set MASM and NASM locations
set(CMAKE_ASM_MASM_COMPILER "${MASM_DIR}/ML.EXE")

# The Visual Studio generator seems to ignore CMAKE_ASM_NASM_COMPILE_OBJECT,
# so for that scenario we'll just allow CMake to fall back to whatever NASM
# is installed on the machine.
if ("${CMAKE_GENERATOR}" STREQUAL "NMake Makefiles")
    set(CMAKE_ASM_NASM_COMPILER "${CMAKE_SOURCE_DIR}/BIN/NASM/NASM.EXE")

    # We don't want to forward include paths or defines to NASM, because it breaks the compile.
    # Notably, the build of NASM included with the leaked source can't handle -I include paths.
    set(CMAKE_ASM_NASM_OBJECT_FORMAT "win32")
    set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> -f ${CMAKE_ASM_NASM_OBJECT_FORMAT} -DWIN32 -o <OBJECT> <SOURCE>")
endif()

# Macros for adding compile options and definitions scoped to C/C++ only.
# This avoids passing these options to NASM which does not support them.
macro(add_c_cxx_compile_option COMPILE_OPTION)
    string(JOIN " " CMAKE_C_FLAGS "${CMAKE_C_FLAGS}" "${COMPILE_OPTION}")
    string(JOIN " " CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" "${COMPILE_OPTION}")
endmacro()

macro(add_c_cxx_compile_definition COMPILE_DEFINITION)
    add_c_cxx_compile_option("/D${COMPILE_DEFINITION}")
endmacro()

# Init project
project(Duke4 C CXX ASM_MASM ASM_NASM)

# Output binaries to the System directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/System)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# There are multiple locations in the source that invoke
# va_start with a C++ reference. Newer versions of VS need
# this to be defined in order to suppress errors for it.
add_c_cxx_compile_definition("_CRT_NO_VA_START_VALIDATION")

# CRT warnings can become noisy when using newer tooling.
add_c_cxx_compile_definition("_CRT_SECURE_NO_WARNINGS")

# Add default compile options (structure packing, etc)
add_c_cxx_compile_option("/EHsc" "/vd0" "/Zp4")
add_c_cxx_compile_definition("WINDOWS_IGNORE_PACKING_MISMATCH")

# Enable /G6 (optimize for Pentium Pro) on VS5 and VS6
if (MSVC_VERSION EQUAL 1100 OR MSVC_VERSION EQUAL 1200)
    add_c_cxx_compile_option("/G6")
endif()

# Add MSVC conformance and compatibility options
if (MSVC_VERSION GREATER_EQUAL 1400)
    # Force Conformance in for Loop Scope
    add_c_cxx_compile_option("/Zc:forScope-")

    # wchar_t Is Native Type
    add_c_cxx_compile_option("/Zc:wchar_t-")

    # Call member new/delete on arrays
    add_c_cxx_compile_option("/Zc:zeroSizeArrayNew-")

    # Assume operator new throws
    add_c_cxx_compile_option("/Zc:throwingNew-")

    # Enable Global Sized Deallocation Functions
    add_c_cxx_compile_option("/Zc:sizedDealloc-")

    # Disable safe exception handlers
    add_link_options("/SAFESEH:NO")
endif()

# DirectX
include_directories(DirectX8/Inc)
link_directories(DirectX8/Lib)

# Projects
add_subdirectory(Cannibal)
add_subdirectory(CannibalEditor)
add_subdirectory(CblMacEd)
add_subdirectory(D3DDrv)
add_subdirectory(DukeEd)
add_subdirectory(Editor)
add_subdirectory(Engine)
add_subdirectory(Fire)
add_subdirectory(Galaxy)
add_subdirectory(GalaxyLib)
add_subdirectory(GlideDrv)
add_subdirectory(Imp3DS)
add_subdirectory(ImpBP2)
#add_subdirectory(ImpC3S)
add_subdirectory(ImpGMA)
add_subdirectory(ImpLWO)
add_subdirectory(ImpMDX)
add_subdirectory(Launch)
add_subdirectory(SoftDrv)
add_subdirectory(UCC)
add_subdirectory(UWeb)
add_subdirectory(xcore)
add_subdirectory(ximage)

# Galaxy needs to be assembled using MASM 6.13, so we need to ensure it uses the proper binary.
set_target_properties(GalaxyLib PROPERTIES VS_USER_PROPS "${CMAKE_SOURCE_DIR}/../VS6/Masm.props")

if (MSVC_VERSION GREATER 1200)
    set(MSVC6_LIB_DIR "${CMAKE_SOURCE_DIR}/../VS6/VC98/LIB")

    # Cannibal needs to be manually linked to MSVCIRT from VS6 under newer compilers, because of MRG.
    target_link_libraries(Cannibal PRIVATE debug "${MSVC6_LIB_DIR}/MSVCIRTd.lib")
    target_link_libraries(Cannibal PRIVATE optimized "${MSVC6_LIB_DIR}/MSVCIRT.lib")
endif()

if (MSVC_VERSION GREATER_EQUAL 1400)
    # Launch is not compatible with data execution prevention.
    # This is necessary to avoid crashes when executing code in Galaxy.
    target_link_options(Launch PRIVATE "/NXCOMPAT:NO")
    target_link_options(DukeEd PRIVATE "/NXCOMPAT:NO")
endif()
