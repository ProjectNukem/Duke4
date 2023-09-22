project(GlideDrv)
add_library(GlideDrv SHARED)
target_link_libraries(GlideDrv PRIVATE Engine glide2x)

set(SOURCE_FILES
    UnGlide.cpp)

source_group("Src" FILES ${SOURCE_FILES})
target_sources(GlideDrv PRIVATE ${SOURCE_FILES})

target_include_directories(GlideDrv PRIVATE
    ${CMAKE_SOURCE_DIR}/xcore
    ${CMAKE_SOURCE_DIR}/Cannibal
    ${CMAKE_SOURCE_DIR}/Core/Inc
    ${CMAKE_SOURCE_DIR}/Engine/Inc
    ${CMAKE_SOURCE_DIR}/Engine/Src
    ${CMAKE_SOURCE_DIR}/Glide/Inc)

target_link_directories(GlideDrv PRIVATE
    ${CMAKE_SOURCE_DIR}/Glide/Lib/Win32)

target_compile_definitions(GlideDrv PRIVATE UNICODE _UNICODE)
target_link_options(GlideDrv PRIVATE "/base:0x10700000")
