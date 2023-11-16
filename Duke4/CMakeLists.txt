cmake_minimum_required(VERSION 3.15)

# Environment
cmake_path(CONVERT "$ENV{MASM_BIN}" TO_CMAKE_PATH_LIST MASM_BIN)
set(MASM_DIR ${MASM_BIN} CACHE PATH "Path to directory containing ML.EXE")

# Set MASM and NASM locations
set(CMAKE_ASM_MASM_COMPILER "${MASM_DIR}/ML.EXE")
set(CMAKE_ASM_NASM_COMPILER "${CMAKE_SOURCE_DIR}/BIN/NASM/NASM.EXE")

# Init project
project(Duke4 C CXX ASM_MASM ASM_NASM)

# Output binaries to the System directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/System)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

add_compile_definitions(
    # There are multiple locations in the source that invoke
    # va_start with a C++ reference. Newer versions of VS need
    # this to be defined in order to suppress errors for it.
    $<$<COMPILE_LANGUAGE:C,CXX>:_CRT_NO_VA_START_VALIDATION>

    # CRT warnings can become noisy when using newer tooling.
    $<$<COMPILE_LANGUAGE:C,CXX>:_CRT_SECURE_NO_WARNINGS>

    # Prevent compile errors caused by 4-byte struct packing.
    $<$<COMPILE_LANGUAGE:C,CXX>:WINDOWS_IGNORE_PACKING_MISMATCH>
)

add_compile_options(
    # Enable standard C++ stack unwinding and assume that extern "C"
    # functions will never throw a C++ exception. This setting was
    # present in the original .dsp project files.
    $<$<COMPILE_LANGUAGE:C,CXX>:/EHsc>

    # Suppress the vtordisp constructor/destructor displacement member.
    # This setting was present in the original .dsp project files.
    $<$<COMPILE_LANGUAGE:C,CXX>:/vd0>

    # 4-byte structure packing. Necessary to ensure struct layouts match
    # what UnrealScript expects.
    $<$<COMPILE_LANGUAGE:C,CXX>:/Zp4>
)

# Enable /G6 (optimize for Pentium Pro) on VS5 and VS6
if (MSVC_VERSION EQUAL 1100 OR MSVC_VERSION EQUAL 1200)
    add_compile_options($<$<COMPILE_LANGUAGE:C,CXX>:/G6>)
endif()

# Add MSVC conformance and compatibility options
if (MSVC_VERSION GREATER_EQUAL 1400)
    add_compile_options(
        # Force Conformance in for Loop Scope
        $<$<COMPILE_LANGUAGE:C,CXX>:/Zc:forScope->

        # wchar_t Is Native Type
        $<$<COMPILE_LANGUAGE:C,CXX>:/Zc:wchar_t->
        
        # Call member new/delete on arrays
        $<$<COMPILE_LANGUAGE:C,CXX>:/Zc:zeroSizeArrayNew->
        
        # Assume operator new throws
        $<$<COMPILE_LANGUAGE:C,CXX>:/Zc:throwingNew->
        
        # Enable Global Sized Deallocation Functions
        $<$<COMPILE_LANGUAGE:C,CXX>:/Zc:sizedDealloc->
    )

    # Disable safe exception handlers
    add_link_options("/SAFESEH:NO")
endif()

# DirectX
include_directories($<$<COMPILE_LANGUAGE:C,CXX>:${CMAKE_SOURCE_DIR}/DirectX8/Inc>)
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
